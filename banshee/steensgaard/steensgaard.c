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
#include <assert.h>
#include <stdio.h>
#include "utils.h"
#include "regions.h"

#ifdef NOTAG
#include "steensgaard_terms_notag.h"
#else
#include "steensgaard_terms.h"
#endif /* NOTAG */

/* Implements the interface defined in pta.h */

/* Contents are a pair of ptr and fun */
typedef struct contents_type_ {
  T ptr;
  L fun;
} *contents_type;

DECLARE_LIST(contents_type_list, contents_type);
DEFINE_NONPTR_LIST(contents_type_list, contents_type);

extern int flag_print_constraints;

T bottom;

/* Hooks for debugging */
static void T_cunify_hook(T t1, T t2)
{
  if (flag_print_constraints)
    {
      T_print(stdout,t1);
      fprintf(stdout," <= ");
      T_print(stdout,t2);
      puts("");
    }
  T_cunify(t1,t2);
}

static void T_unify_hook(T t1, T t2)
{
  if (flag_print_constraints)
    {
      T_print(stdout,t1);
      fprintf(stdout," == ");
      T_print(stdout,t2);
      puts("");
    }
  T_unify(t1,t2);
}

static void L_cunify_hook(L t1, L t2)
{
  if (flag_print_constraints)
    {
      L_print(stdout,t1);
      fprintf(stdout," <= ");
      L_print(stdout,t2);
      puts("");
    }
  L_cunify(t1,t2);
}

static void L_unify_hook(L t1, L t2)
{
  if (flag_print_constraints)
    {
      L_print(stdout,t1);
      fprintf(stdout," == ");
      L_print(stdout,t2);
      puts("");
    }
  L_unify(t1,t2);
}

static T get_ref(T t) {
  struct ref_decon r_decon = ref_decon(t);

  assert(r_decon.f1);

  return r_decon.f1;  
}

/* If t is ref(...), return its contents, otherwise, build a ref
   constructor and unify it with t, returning its contents */
static struct contents_type_ decompose_ref_or_fresh(T t) {
  if (T_is_ref(t)) {
    struct ref_decon decon = ref_decon(t);
    return  (struct contents_type_) {decon.f1, decon.f2};
  }
  else {
    T ptr = T_fresh("'fv");
    L fun = L_fresh("'fv");
    T_cunify_hook(t, ref(alabel_t_wild(), ptr, fun));
    return (struct contents_type_){ptr,fun};
  }
  assert(0);
  return (struct contents_type_){NULL,NULL};
}

static argT fun_rec_T(T_list args)
{
  region scratch = newregion();
  int counter = 0;
  argT rest, result;
  T_list_scanner scan;
  T temp;
  char field_name[512];
  
  argT_map map = new_argT_map(scratch);
  T_list_scan(args,&scan);
  while(T_list_next(&scan,&temp))
    {
      snprintf(field_name,512,"%d",counter++);
      argT_map_cons(argT_make_field(field_name,temp),map);
    }
  
  rest = argT_wild();
  
  // safe since field_add makes a copy of the string
  result = argT_row(map,rest);
  
  deleteregion(scratch);
  
  return result;
}

void pta_init() {
#ifdef NOTAG
  steensgaard_terms_notag_init();
#else
  steensgaard_terms_init();
#endif
  flag_hash_cons = FALSE;
  bottom = T_zero();
}

void pta_reset() {
#ifdef NOTAG
  steensgaard_terms_notag_reset();
#else
  steensgaard_terms_reset();
#endif
}

/* ref(l,id_ptr, id_fun) */
T pta_make_ref(const char *id) {
  alabel_t loc;

#ifndef NOTAG
  alabel_t tag;
#endif
  T ptr;
  L fun;

#ifndef NOTAG  
  tag = alabel_t_constant(id);
#endif
  loc = alabel_t_fresh(id);
  
  ptr = T_fresh(id);
  fun = L_fresh(id);

#ifndef NOTAG
  alabel_t_inclusion(tag, loc);
#endif

  return ref(loc, ptr, fun);  
}

/* The no information case  */
T pta_bottom(void) {
  return bottom;
}

/* Join t1 and t2's contents, but not their tags  */
T pta_join(T t1, T t2) {
  if (t1 == bottom) return t2;
  else if (t2 == bottom) return t1;
  else {
    T ptr;
    L fun;  
    struct contents_type_ c1;
    struct contents_type_ c2;
    
    ptr = T_fresh("join_ptr");
    fun = L_fresh("join_fun");
    
    c1 = decompose_ref_or_fresh(t1);
    c2 = decompose_ref_or_fresh(t2);
    
    T_cunify_hook(c1.ptr, ptr);
    T_cunify_hook(c2.ptr, ptr);
    L_cunify_hook(c1.fun, fun);
    L_cunify_hook(c2.fun, fun);
    
    return ref(alabel_t_wild(), ptr, fun);
  }
}

/* ref(tag, ptr, fun) --> ptr */
T pta_deref(T t) {
  if (T_is_ref(t)) {
    struct ref_decon r_decon = ref_decon(t);
    return r_decon.f1;
  }
  else {
    T ptr = T_fresh("'ptr");
    T_unify_hook(t, ref(alabel_t_fresh("wild"), ptr, L_wild()));
    
    return ptr;
  }
  
  assert(0);
  return NULL;
}

T pta_rvalue(T t) {
  if (T_is_ref(t)) {
    struct ref_decon r_decon = ref_decon(t);
    return r_decon.f1;
  }
  else {
    T ptr = T_fresh("'ptr");
    T_cunify_hook(t, ref(alabel_t_fresh("wild"), ptr, L_wild()));
    
    return ptr;
  }
  
  assert(0);
  return NULL;
}

T pta_address(T t1) {
  return ref(alabel_t_wild(), t1,L_wild());
}

void pta_assignment(T t1, T t2) {
  T_cunify_hook(t2, pta_deref(t1));
}

T pta_make_fun(const char *name, T ret, T_list args) {
  T body = pta_make_ref(name);
  region scratch = newregion();
  T_list_scanner scan;
  T temp;
  T_list arg_list = new_T_list(scratch);

  T_list_scan(args, &scan);

  while (T_list_next(&scan, &temp)) {
    T_list_cons(get_ref(temp),arg_list);
  }
  
  T_unify_hook(body, ref(alabel_t_fresh("wild"), T_wild(),
		    lam(alabel_t_fresh(name), fun_rec_T(arg_list), get_ref(ret))));
  return body;
}

/* t is the fun type, actuals are the actual parameters */
T pta_application(T t, T_list actuals) {
  L fun;
  argT args;
  region scratch = newregion();
  T_list formals = new_T_list(scratch);
  T_list_scanner scan;
  T next;
  T retVal = T_fresh("ret");
  struct contents_type_ contents = decompose_ref_or_fresh(t);

  T_list_scan(actuals, &scan);

  while(T_list_next(&scan,&next)) {
    T nextFormal = T_fresh("arg");
    T_cunify_hook(next, nextFormal);
    T_list_append_tail(nextFormal, formals);
  }

  args = fun_rec_T(formals);
  fun = lam(alabel_t_fresh("wild"), args, retVal);
  L_unify_hook(contents.fun, fun);
  deleteregion(scratch);

  return pta_address(retVal);
}

contents_type pta_get_contents(T t) {
  contents_type contents = (contents_type) malloc(sizeof(struct contents_type_));
  struct ref_decon decon = ref_decon(t);
			   
  contents->ptr = decon.f1;
  contents->fun = decon.f2;

  return contents;
}

#ifndef NOTAG
static void pr_tag_elem(alabel_t t) {
  printf(",");
  alabel_t_print(stdout,t);
}
#endif

/* TODO : doesn't print or count fun ptrs */
void pta_pr_ptset(contents_type c) {
#ifdef NOTAG
  return; 
#else
  int size = 0;  
  region scratch_rgn = newregion();

  T ptr_rep = T_ecr(c->ptr);
  // L fun_rep = L_ecr(c->fun);

  printf("{");
  if (T_is_ref(ptr_rep)) {
    alabel_t_list tags = 
      alabel_t_list_copy(scratch_rgn, alabel_t_tlb(ref_decon(ptr_rep).f0));
    
    if (!alabel_t_list_empty(tags)) {
      size += alabel_t_list_length(tags);
      alabel_t_print(stdout,alabel_t_list_head(tags));
      
      tags = alabel_t_list_tail(tags);
      alabel_t_list_app(tags, pr_tag_elem);
    }
  }
  printf("}(%d)", size);
  deleteregion(scratch_rgn);
#endif /* NOTAG */
}

/* TODO : doesn't count fun ptrs */
int pta_get_ptsize(contents_type c) {
#ifndef NOTAG
  T ptr_rep = T_ecr(c->ptr);
  if (T_is_ref(ptr_rep)) {
    alabel_t_list tags = alabel_t_tlb(ref_decon(ptr_rep).f0);
    return alabel_t_list_length(tags);
  }
#endif
  return 0;
}

void pta_serialize(FILE *f, hash_table *entry_points, unsigned long sz)
{ 
  assert(f);
#ifdef NOTAG
  steensgaard_terms_notag_serialize(f, entry_points, sz);
#else
  steensgaard_terms_serialize(f,entry_points,sz);
#endif
}

hash_table *pta_deserialize(FILE *f)
{
  assert(f);
#ifdef NOTAG
  return steensgaard_terms_notag_deserialize(f);
#else
  return steensgaard_terms_deserialize(f);
#endif
}

void pta_region_serialize(FILE *f)
{
#ifdef NOTAG
  steensgaard_terms_notag_region_serialize(f);
#else
  steensgaard_terms_region_serialize(f);
#endif
}

void pta_region_deserialize(translation t, FILE *f)
{
#ifdef NOTAG
  steensgaard_terms_notag_region_deserialize(t, f);
#else
  steensgaard_terms_region_deserialize(t, f);
#endif
}

