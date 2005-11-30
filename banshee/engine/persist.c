/*
 * Copyright (c) 2000-2004
 *      The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

#include <string.h>

#include "persist.h"
#include "regions.h"
#include "hash.h"
#include "list.h"
#include "utils.h"
#include "banshee_persist_kinds.h"

/* Types */
typedef enum {
  persist_raw,
  persist_serializing,
  persist_deserializing,
} persist_state;

typedef struct persist_entry_ {
  int kind;
  void *obj;
} *persist_entry;

DECLARE_LIST(persist_entry_queue, persist_entry);
DEFINE_NONPTR_LIST(persist_entry_queue, persist_entry);

/* Globals */
static persist_state current_state = persist_raw;
static FILE *current_file;
static persist_entry_queue serialize_queue;
static serialize_fn_ptr *serialize_fns;
region persist_rgn;
static int num_kinds;
static hash_table object_map;
static hash_table serialized_objects;

/* Handling function pointer serialization */
/* A mapping from function pointers to ints (id's) for serialization  */
extern hash_table fn_ptr_table;
/* A mapping from ints (id's) to function pointers for deserialization */
extern void *fn_ptr_array[];

/* Serialization */
void serialize_start(FILE *f, serialize_fn_ptr kind_map[], int length)
{
  assert(current_state == persist_raw);
  assert(f); 

  persist_rgn = newregion();
  current_state = persist_serializing;

  serialize_queue = new_persist_entry_queue(persist_rgn);
  serialized_objects = make_hash_table(persist_rgn, 128, ptr_hash, ptr_eq);
  
  current_file = f;
  num_kinds = length;
  serialize_fns = rarrayalloc(persist_rgn, length, serialize_fn_ptr);
  rarraycopy(serialize_fns, kind_map, length, serialize_fn_ptr);
}

static bool do_serialize_object(void *obj, int kind)
{
  bool success = TRUE;
  assert(current_state == persist_serializing);
  assert(persist_rgn);
  assert(serialize_fns);
  assert(kind < num_kinds);
  assert(current_file);

  success = fwrite((void *)&kind,sizeof(int),1,current_file);
  success &= fwrite((void*)&obj, sizeof(void *), 1, current_file);

  success &= serialize_fns[kind](current_file, obj);

  return success;
}

bool serialize_object(void *obj, int kind)
{
  persist_entry entry;

  assert(kind >= 0);
  assert(current_state == persist_serializing);

  /* Don't serialize null objects */
  if (!obj) return TRUE;

  if ( hash_table_insert(serialized_objects, (hash_key)obj, (hash_data) NULL)) {
    entry = ralloc(persist_rgn, struct persist_entry_);
    entry->kind = kind;
    entry->obj = obj;
    persist_entry_queue_append_tail(entry,serialize_queue);
  }  

  return TRUE;
}

void serialize_end(void)
{
  persist_entry next_entry;
  assert(current_state == persist_serializing);

  while(!persist_entry_queue_empty(serialize_queue)) {
    next_entry = persist_entry_queue_head(serialize_queue);
    do_serialize_object(next_entry->obj, next_entry->kind);
    persist_entry_queue_tail(serialize_queue);
  }

  deleteregion(persist_rgn);
  num_kinds = 0;
  current_file = NULL;
  serialize_queue = NULL;
  persist_rgn = NULL;
  current_state = persist_raw;
}

static bool create_objects(FILE *f, deserialize_fn_ptr deserialize_obj[], 
			  int length)
{
  void *id = NULL;
  void *obj = NULL;
  int kind;
  int num_objects = 0;
  bool success = TRUE;
  persist_entry entry;

  assert(f);

  while ( 1 == fread((void *)&kind, sizeof(int), 1, f) ) {
    assert(kind < length);
    
    success &= fread((void *)&id, sizeof(void *), 1, f);

    obj = deserialize_obj[kind](f);

    /* For any kind other than nonptr and string, add to an object map */
    if (kind != 0 && obj) {
      assert(id);
      assert(object_map);
      entry = ralloc(persist_rgn, struct persist_entry_);
      entry->kind = kind;
      entry->obj = obj;
      success &= 
	hash_table_insert(object_map, (hash_key) id, (hash_data) entry);
    }
    num_objects++;
  }

  assert(success);
  return success;
}

static bool set_object_fields(set_fields_fn_ptr set_fields[], int length)
{
  hash_table_scanner scan;
  hash_key key;
  persist_entry entry;
  bool success = TRUE;

  hash_table_scan(object_map, &scan); 

  while(hash_table_next(&scan,&key, (hash_data *)&entry)) {
    assert(entry->kind < length);
    success &= set_fields[entry->kind](entry->obj);
  }

  assert(success);
  return success;
}

/* Deserialization */
bool deserialize_all(FILE *f, deserialize_fn_ptr deserialize_obj[], 
		set_fields_fn_ptr set_fields[], int length)
{
  assert(current_state == persist_raw);
  assert(length > 0);
  assert(persist_rgn == NULL);
  
  persist_rgn = newregion();
  current_state = persist_deserializing;
  
  object_map = make_hash_table(persist_rgn, 128, ptr_hash, ptr_eq);

  create_objects(f, deserialize_obj, length);
  set_object_fields(set_fields, length);

  return TRUE;
}

void deserialize_end(void)
{
  object_map = NULL;
  deleteregion(persist_rgn);
  current_state = persist_raw;
}

void *deserialize_get_obj(void *old_field)
{
  persist_entry entry = NULL;

  assert(current_state == persist_deserializing);
  assert(object_map); 

  if (old_field == NULL) return NULL;
  else if (! hash_table_lookup(object_map, old_field, (hash_data *)&entry)) {
    return NULL;
  }
  else return entry->obj;
}

bool deserialize_set_obj(void **old_obj_ptr)
{
  void *result = deserialize_get_obj(*old_obj_ptr);
  
  if (result) {
    *old_obj_ptr = result;  
    return TRUE;
  }
  else return FALSE;
}

bool nonptr_data_serialize(FILE *f, void *obj)
{
  return TRUE;
}

void *nonptr_data_deserialize(FILE *f)
{
  return NULL;
}

bool nonptr_data_set_fields(void *obj)
{
  return TRUE;
}

bool funptr_data_serialize(FILE *f, void *obj)
{
  int id = 0;
  bool success;
  assert(f);
  assert(obj);

  success = hash_table_lookup(fn_ptr_table,(hash_key)obj, (hash_data*)&id);
  
  success &= fwrite((void *)&id, sizeof(int), 1, f);

  if (!success) {
    fprintf(stderr, "Warning: failed to serialize a function pointer\n");
  }

  return success;  
}

void *funptr_data_deserialize(FILE *f)
{
  int id = 0;
  assert(f);

  fread((void *)&id, sizeof(int), 1, f);

  return fn_ptr_array[id];
}

void *update_funptr_data(int id) 
{
  return fn_ptr_array[id];
}

bool funptr_data_set_fields(void *obj)
{
  return TRUE;
}

bool string_data_serialize(FILE *f, void *obj)
{
  int length = strlen((char *)obj);
  /* write the string length */
  fwrite((void *)&length, sizeof(long), 1, f);
  fwrite((void *)obj, sizeof(char),strlen((char *)obj), f);

  return TRUE;
}

void *string_data_deserialize(FILE *f)
{
  long length;
  char buf[512];

  fread((void *)&length, sizeof(long), 1, f);
  assert(length < 512);
  fread((void *)buf, sizeof(char), length, f);
  buf[length] = '\0';
  
  return (void *)strdup(buf);
}

bool string_data_set_fields(void *obj)
{
  return TRUE;
}
