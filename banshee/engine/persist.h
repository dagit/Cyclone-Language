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

/* persist: manager for serializing/deserializing an arbitrary object
   graph with cycles, joins, etc.

   Initialize the manager by calling serialize_start with an opened
   file.

   Calls to serialize_object will enqueue new serialization
   requests. These are hashed so that multiple attempts to serialize
   the same object are ignored. Object identity is just pointer
   equality.
   
   Each object is written out as a triple (kind, id, data). The
   meaning of these fields is as follows:
   
   1. kind -- an unsigned int identifying the type of the
   object. Clients of the persistence manager must provide a mapping
   from kinds to serialization function pointers (for serialization)
   or from kinds to deserialization function pointers (for
   deserialization). three kinds are reserved: 0 for non-pointer, 1
   for function pointer data, and 2 for string data

   2. id -- this is the address of the object. It is needed to
   reconstruct the object graph during deserialization.
   
   3. data -- the contents of the object. It is expected that
   pointer-valued objects are just written out as addresses. During
   deserialization a second pass over the object graph will fill in
   these pointers after an initial construction phase where all
   pointers are invalid (contain object id's, not valid
   pointers). 

   Each struct must provide the following methods to support
   persistence:
   
   a) bool serialize(FILE *f, void *obj)

      This function should serialize the struct. All pointer-valued
      fields should be written out as addresses and the function
      serialize_object called on them.

   b) void *deserialize(FILE *f)

      This function should deserialize the struct. All pointer-valued
      fields should be read in. These pointers will NOT be valid until
      set_fields has been called on them. This function should return
      the constructed object

   c) bool deserialize_set_obj(void **objptr)

      This function should fill in all the pointer valued fields of
      the struct.  Essentially it should just do this:

      deserialize_set_obj(&obj->f1)
      ...
      deserialize_set_obj(&obj->fn)

      This works because deserialize(...) is expected to store the
      object id's in the fields temporarily. deserialize_set_obj(...)
      has a mapping from these object id's to valid object pointers
*/

#ifndef PERSIST_H
#define PERSIST_H

#include <stdio.h>
#include "bool.h"
#include "linkage.h"

#define NONPTR_PERSIST_KIND 0
#define FUNPTR_PERSIST_KIND 1
#define STRING_PERSIST_KIND 2


EXTERN_C_BEGIN

/* Serialize an object, returning success (TRUE-- success) */
typedef bool (*serialize_fn_ptr) (FILE *f,void *obj);

/* Deserialize an object from the given file */
typedef void * (*deserialize_fn_ptr) (FILE *f);

/* In the 2nd deserialization stage, this is called on every object */
/* it is expected to call deserialize_get_obj for each field */
typedef bool (*set_fields_fn_ptr) (void *obj);

/* Serialization */
void serialize_start(FILE *f, serialize_fn_ptr kind_map[], int length);
bool serialize_object(void *obj, int kind); 
void serialize_end(void);

/* Deserialization */
bool deserialize_all(FILE *f, deserialize_fn_ptr deserialize_obj[], 
		set_fields_fn_ptr set_fields[], int length);
void *deserialize_get_obj(void *old_obj);
bool deserialize_set_obj(void **old_obj);

void deserialize_end(void);

/* Support for special data kinds (non pointer, function pointer, string) */
bool nonptr_data_serialize(FILE *f, void *obj);
void *nonptr_data_deserialize(FILE *f);
bool nonptr_data_set_fields(void *obj);
bool funptr_data_serialize(FILE *f, void *obj);
void *funptr_data_deserialize(FILE *f);
bool funptr_data_set_fields(void *obj);
bool string_data_serialize(FILE *f, void *obj);
void *string_data_deserialize(FILE *f);
bool string_data_set_fields(void *obj);

void *update_funptr_data(int id);

EXTERN_C_END

#endif /* PERSIST_H */
