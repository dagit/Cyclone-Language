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
//#include "pta.h"
#include "regions.h"

#include "andersen_terms.h"

/* Implements the interface defined in pta.h */

typedef T contents_type;

DECLARE_LIST(contents_type_list, T);
DEFINE_NONPTR_LIST(contents_type_list, T);

extern int flag_print_constraints;

// Hook for debugging
static void term_inclusion(T t1, T t2)
{
  if (flag_print_constraints)
    {
      T_print(stdout,t1);
      fprintf(stdout," <= ");
      T_print(stdout,t2);
      puts("");
    }
  T_inclusion(t1,t2);
}

void pta_init()
{
  //flag_hash_cons = FALSE;
  andersen_terms_init();
}

void pta_reset()
{
  andersen_terms_reset();
}


static T get_ref(T t)
{
  struct ref_decon r_decon = ref_decon(t);
  
  assert(r_decon.f1);

  return r_decon.f1;
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
  // rest = argT_fresh();

  // safe since field_add makes a copy of the string
  result = argT_row(map,rest);
  
  deleteregion(scratch);

  return result;
}
 
static T pta_make_lam(const char *id, T ret, T_list args)
{
  return lam (alabel_t_constant(id), fun_rec_T(args), ret);
}

T pta_make_ref(const char *id)
{

  T var = T_fresh(id);

  alabel_t tag = alabel_t_constant(id);
  
  return ref(tag, var,var);
}

T pta_bottom(void)
{
  return T_zero();
}

T pta_join(T t1,T t2)
{
  T result;
  region scratch_rgn = newregion();
  T_list list = new_T_list(scratch_rgn);
  
  T_list_cons(t1,list);
  T_list_cons(t2,list);

  
  result = T_union(list);
  deleteregion(scratch_rgn);

  return result;
}

T pta_deref(T t1)
{
  return ref_proj2(t1);
}

T pta_rvalue(T t1)
{
  return pta_deref(t1);
}

T pta_address(T t1)
{
  return ref(alabel_t_one(), T_one(), t1);
}

void pta_assignment(T t1,T t2)
{
  term_inclusion(t1, ref_pat1(t2));
}

T pta_make_fun(const char *name, T ret, T_list args)
{
  T temp;
  T_list_scanner scan;
  region scratch_rgn = newregion();
  T_list arg_list = new_T_list(scratch_rgn);
  
  T_list_scan(args,&scan);

  while (T_list_next(&scan,&temp))
    {
      T_list_cons(get_ref(temp),arg_list);
    }

  return pta_make_lam(name,get_ref(ret),arg_list);
}

T pta_application(T t, T_list actuals)
{
  argT args = fun_rec_T(actuals);
  
  term_inclusion(t, lam_pat1(args));
  return pta_address(lam_proj2(t));
}

contents_type pta_get_contents(T t)
{
  struct ref_decon t_decon = ref_decon(t);

  return t_decon.f1;
}

static void pr_ptset_T_elem(T t)
{
  struct ref_decon ref = ref_decon(t);
  struct lam_decon lam = lam_decon(t);
  
  printf(",");
  if (ref.f0)
      alabel_t_print(stdout,ref.f0);
  else if (lam.f0)
      alabel_t_print(stdout,lam.f0);
  /*
  printf(",");
  T_pr(stdout,(T)t);
  */
}


void pta_pr_ptset(contents_type t)
{
  int size;
  region scratch_rgn = newregion();
  T_list ptset = T_list_copy(scratch_rgn,T_tlb(t));
  
  size = T_list_length(ptset);
  
  printf("{");
  if (! T_list_empty(ptset) )
    {
      struct ref_decon ref = ref_decon(T_list_head(ptset));
      struct lam_decon lam = lam_decon(T_list_head(ptset));

      if (ref.f0)
	  alabel_t_print(stdout,ref.f0);
      else if (lam.f0)
	  alabel_t_print(stdout,lam.f0);

      /*
            T_pr(stdout,T_hd(ptset));
      */
      ptset = T_list_tail(ptset);
    }
  T_list_app(ptset,pr_ptset_T_elem);
  printf("}(%d)",size);
  deleteregion(scratch_rgn);
}

int pta_get_ptsize(contents_type t)
{
  T_list ptset = T_tlb(t);
  return T_list_length(ptset);
}

void pta_serialize(FILE *f, hash_table *entry_points, unsigned long sz)
{
  assert(f);
  andersen_terms_serialize(f,entry_points,sz);
}

hash_table *pta_deserialize(FILE *f)
{
  assert(f);
  return andersen_terms_deserialize(f);
}

void pta_region_serialize(FILE *f)
{
  andersen_terms_region_serialize(f);
}

void pta_region_deserialize(translation t, FILE *f)
{
  andersen_terms_region_deserialize(t, f);
}
