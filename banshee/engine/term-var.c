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

#include <stdio.h>
#include <assert.h>
#include "regions.h"
#include "ufind.h"
#include "term-var.h"
#include "bounds.h"
#include "banshee_persist_kinds.h"
#include "banshee_region_persist_kinds.h"

DECLARE_UFIND(tv_elt,gen_e);
DEFINE_UFIND(tv_elt,gen_e);
DEFINE_LIST(term_var_list,term_var);

extern region term_var_region;

struct term_var_
{
#ifdef NONSPEC
  sort_kind sort;
#endif
  int type;
  stamp st;
  bounds pending;
  char *name;
  tv_elt elt;
  bool valid_pending;
};

static term_var make_var(region r, const char *name, stamp st)
{
  term_var result = ralloc(term_var_region, struct term_var_);
  gen_e info = (gen_e) result;

  result->type = VAR_TYPE;
  result->st = st;
  result->pending = bounds_persistent_create();
  result->name = name ? rstrdup(banshee_nonptr_region,name) : rstrdup(banshee_nonptr_region, "fv");
  result->elt = new_tv_elt(r,info);
#ifdef NONSPEC
  result->sort = term_sort;
#endif
  result->valid_pending = TRUE;

  return result;
}

term_var tv_fresh(region r, const char *name)
{
  return make_var(r,name,stamp_fresh());
}

term_var tv_fresh_small(region r, const char *name)
{
  return make_var(r,name,stamp_fresh_small());
}

term_var tv_fresh_large(region r, const char *name)
{
  return make_var(r,name,stamp_fresh_large());
}

static term_var tv_get_v_ecr(term_var v)
{
  term_var ecr = (term_var)tv_get_ecr(v);
  assert (ecr->type == VAR_TYPE); /* this is a hack, but should be ok */

  return ecr;
}

char *tv_get_name(term_var v)
{
  return tv_get_v_ecr(v)->name;
}

bounds tv_get_pending(term_var v)
{
  return tv_get_v_ecr(v)->pending;
}

bool tv_add_pending(term_var v, gen_e e, stamp st)
{
  return bounds_add(tv_get_v_ecr(v)->pending,e,st);
}

bool tv_is_valid_pending(term_var v) {
  return tv_get_v_ecr(v)->valid_pending;
}

bool tv_invalidate_pending(term_var v) {
  return tv_get_v_ecr(v)->valid_pending = FALSE;
}

void tv_unify(term_var v, gen_e e)
{
  tv_elt_update(v->elt,e);
  
  assert(tv_get_ecr(v) == e);
}

static gen_e tv_combine(const gen_e e1,const gen_e e2)
{
  return e1;
}

void tv_unify_vars(term_var v1, term_var v2)
{
  tv_elt_unify(tv_combine,v1->elt, v2->elt);
}

gen_e tv_get_ecr(term_var v)
{
  return tv_elt_get_info(v->elt);
}

/* Persistence */

bool term_var_serialize(FILE *f, void *obj)
{
  term_var var = (term_var)obj;

  assert(f);
  assert(var);

  fwrite((void *)&var->st, sizeof(stamp), 1, f);
  fwrite((void *)&var->pending, sizeof(bounds), 1, f);
  string_data_serialize(f, var->name);
  fwrite((void *)&var->elt, sizeof(tv_elt), 1, f);

  serialize_banshee_object(var->pending, bounds);
  serialize_banshee_object(var->elt, uf_element);

  return TRUE;
}

void *term_var_deserialize(FILE *f)
{
  term_var var = NULL;

  assert(f);
  assert(permanent);
  
  var = ralloc(term_var_region, struct term_var_);

  fread((void *)&var->st, sizeof(stamp), 1, f);
  fread((void *)&var->pending, sizeof(bounds), 1, f);
  var->name = (char *)string_data_deserialize(f);
  fread((void *)&var->elt, sizeof(tv_elt), 1, f);
  
  return var;
}

bool term_var_set_fields(void *obj)
{
  term_var var = (term_var)obj;
  
  deserialize_set_obj((void **)&var->pending);
  deserialize_set_obj((void **)&var->elt);

  return TRUE;
}

int update_term_var(translation t, void *m)
{
  term_var var = (term_var)m;

  update_pointer(t, (void **)&var->pending);
  update_pointer(t, (void **)&var->name);
  update_pointer(t, (void **)&var->elt);

  return sizeof(struct term_var_);
}
