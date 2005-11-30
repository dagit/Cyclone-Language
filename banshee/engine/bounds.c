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

/* Hashset/list based bounds implementation. Deletion is an
 * expensive operation. */

#include <stdlib.h>
#include <assert.h>
#include "bounds.h"

struct bounds_
{
  hash_set set;
  gen_e_list elems;
};

bounds bounds_create(region r)
{
  bounds result;
  
  result = ralloc(r, struct bounds_);
  result->set = hs_create(r);
  result->elems = new_gen_e_list(r);

  return result;
}

gen_e_list bounds_exprs(bounds b)
{
  return b->elems;
}

void bounds_scan(bounds b, bounds_scanner *bs)
{
  gen_e_list_scan(b->elems, &(bs->ls));
}

bool bounds_next(bounds_scanner *bs, gen_e *e)
{
  return gen_e_list_next(&(bs->ls), e);
}

bool bounds_add(bounds b, gen_e e, stamp s)
{
  bounds_query(b,s);
  if (hs_member(b->set, s))	
    return TRUE;
  
  else
    {
      gen_e_list_cons(e,b->elems);
      return FALSE;
    }
}

/* TODO */
bool bounds_remove(bounds b, stamp s)
{
  fprintf(stderr,"Bounds remove not implemented!\n");
  assert(0);
  return FALSE;
}

stamp bounds_stamp(bounds b)
{
  return 0;
}

bool bounds_empty(bounds b)
{
  return (gen_e_list_empty(b->elems));
}

bool bounds_query(bounds b, stamp st)
{
  bool result = (hs_query(b->set, st));
  return result;
}

void bounds_set(bounds b,gen_e_list l)
{
  b->elems = l;
}

int bounds_size(bounds b)
{
  return gen_e_list_length(b->elems);
}

void bounds_delete(bounds b)
{
  hs_delete(b->set);
  gen_e_list_clear(b->elems);
}

/* Persistence */
bool bounds_serialize(FILE *f, void *obj)
{
  fail("bounds persistence not implemented\n");
  return FALSE;
}

/* TODO */
void *bounds_deserialize(FILE *f)
{
  fail("bounds persistence not implemented\n");
  return NULL;
}

/* TODO */
bool bounds_set_fields(void *obj)
{
  fail("bounds persistence not implemented\n");
  return FALSE;
}

region added_edge_info_region;

void bounds_init()
{
  added_edge_info_region = newregion();
}

void bounds_reset()
{
}

bounds bounds_persistent_create()
{
  return bounds_create(permanent);
}

int update_bounds(translation t, void *m)
{
  return sizeof(struct bounds_);
}

int update_added_edge_info(translation t, void *m)
{
  added_edge_info info = (added_edge_info)m;

  update_pointer(t, (void **)&info->b);
  update_pointer(t, (void **)&info->sl);

  return sizeof(struct added_edge_info_);
}

region bounds_region = NULL;

bool added_edge_info_serialize(FILE *f, void *obj)
{
  return FALSE;
}

void *added_edge_info_deserialize(FILE *f)
{
  return NULL;
}

bool added_edge_info_set_fields(void *obj)
{
  return FALSE;
}
