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

/* Hashtable based bounds implementation */

#include <stdlib.h>
#include <assert.h>
#include "hash.h"
#include "bounds.h"
#include "stamp.h"
#include "banshee_persist_kinds.h"

struct bounds_ {
  stamp st;
  hash_table table;
};

region bounds_region = NULL;
region added_edge_info_region = NULL;

bounds bounds_create(region r)
{
  bounds result;

  if (r) {
    result = ralloc(r, struct bounds_);
    result->table = make_hash_table(r, 8, stamp_hash, stamp_eq);
  }
  else {
    result = ralloc(bounds_region, struct bounds_);
    result->table = make_persistent_hash_table(8, stamp_hash, stamp_eq,
					       BANSHEE_PERSIST_KIND_nonptr,
					       BANSHEE_PERSIST_KIND_gen_e);
  }
  result->st = stamp_fresh();

  return result;
}

bounds bounds_persistent_create()
{
  return bounds_create(NULL);
}

/* TODO */
gen_e_list bounds_exprs(bounds b)
{
  assert(0);
  return NULL;
}

void bounds_scan(bounds b, bounds_scanner *scan)
{
  hash_table_scan(b->table,&scan->hs);
}

bool bounds_next(bounds_scanner *scan, gen_e *e)
{
  return hash_table_next(&scan->hs,NULL,(hash_data *) e);
}

bool bounds_add(bounds b, gen_e e, stamp st)
{
  return !hash_table_insert(b->table, (hash_key)st, (hash_data) e);
}

/* Returns TRUE if e was in the bounds */
bool bounds_remove(bounds b, stamp st)
{
  return hash_table_remove(b->table,(hash_key)st);
}

bool bounds_query(bounds b, stamp st)
{
  bool result = hash_table_lookup(b->table, (hash_key)st, NULL);
  return result;
}

bool bounds_empty(bounds b)
{
  return (hash_table_size(b->table) == 0);
}

void bounds_delete(bounds b)
{
  hash_table_reset(b->table);
}

/* TODO */
void bounds_set(bounds b, gen_e_list el)
{
  assert(0);
  fail("bounds_set not implemented\n");
}

int bounds_size(bounds b)
{
  return hash_table_size(b->table);
}


/* Persistence */
bool bounds_serialize(FILE *f, void *obj)
{
  bounds b = (bounds)obj;
  assert(f);
  assert(obj);

  fwrite(b, sizeof(struct bounds_), 1, f);
  serialize_banshee_object(b->table, hash_table);
  
  return TRUE;
}

void *bounds_deserialize(FILE *f)
{
  bounds b = ralloc(bounds_region, struct bounds_);

  assert(f);
  fread(b, sizeof(struct bounds_), 1, f);

  return b;
}

bool bounds_set_fields(void *obj)
{
  bounds b = (bounds)obj;
  assert(b);

  deserialize_set_obj((void **)&b->table);

  return TRUE;
}

stamp bounds_stamp(bounds b)
{
  return b->st;
}

bool added_edge_info_serialize(FILE *f, void *obj)
{
  added_edge_info info = (added_edge_info)obj;
  assert(f);

  fwrite(info, sizeof(struct added_edge_info_), 1, f);

  serialize_banshee_object(info->b, bounds);
  serialize_banshee_object(info->sl, list);

  return TRUE;  
}

void *added_edge_info_deserialize(FILE *f)
{
  added_edge_info info = 
    ralloc(added_edge_info_region, struct added_edge_info_);

  assert(f);
  fread(info, sizeof(struct added_edge_info_), 1, f);

  return info;
}

bool added_edge_info_set_fields(void *obj)
{
  added_edge_info info = (added_edge_info)obj;
  deserialize_set_obj((void **)&info->b);
  deserialize_set_obj((void **)&info->sl);
  
  return TRUE;
}

void bounds_init()
{
  added_edge_info_region = newregion();
  bounds_region = newregion();
}

void bounds_reset()
{
  deleteregion(bounds_region);
  deleteregion(added_edge_info_region);
  
  bounds_init();
}

int update_bounds(translation t, void *m)
{
  update_pointer(t, (void **)&((bounds)m)->table);

  return (sizeof(struct bounds_));
}

int update_added_edge_info(translation t, void *m)
{
  added_edge_info info = (added_edge_info)m;

  update_pointer(t, (void **)&info->b);
  update_pointer(t, (void **)&info->sl);

  return sizeof(struct added_edge_info_);
}
