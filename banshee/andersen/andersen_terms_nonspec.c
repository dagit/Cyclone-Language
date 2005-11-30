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

#include "nonspec.h"

typedef gen_e alabel_t;
typedef gen_e T;
typedef gen_e argT;
typedef flowrow_field argT_field;

static constructor ref_c;
static constructor lam_c;

typedef gen_e_list argT_list;
typedef gen_e_list alabel_t_list;
typedef gen_e_list T_list;
typedef flowrow_map argT_map;

DECLARE_OPAQUE_LIST(alabel_t_list,gen_e);
DECLARE_OPAQUE_LIST(T_list, gen_e);
DECLARE_OPAQUE_LIST(argT_list,gen_e);
DECLARE_OPAQUE_LIST(argT_map,argT_field);
DEFINE_LIST(alabel_t_list,gen_e);
DEFINE_LIST(T_list, gen_e);
DEFINE_LIST(argT_list,gen_e);
DEFINE_LIST(argT_map,flowrow_field);

/*
typedef gen_e_list argT_list;
typedef gen_e_list alabel_t_list;
typedef gen_e_list T_list;
typedef flowrow_map argT_map;
*/

alabel_t alabel_t_zero(void)
{
  return setif_zero();
}

alabel_t alabel_t_one(void)
{
  return setif_one();
}
alabel_t alabel_t_fresh(const char *name)
{
  return setif_fresh(name);
}

alabel_t alabel_t_union(alabel_t_list exprs) deletes
{
  return setif_union(exprs);
}

alabel_t alabel_t_inter(alabel_t_list exprs) deletes
{
  return setif_inter(exprs);
}

alabel_t alabel_t_constant(const char *name) deletes
{
  return setif_constant(name);
}

/* TODO */
bool alabel_t_is_constant(alabel_t e,const char *name)
{
  return FALSE;
}

int alabel_t_inclusion(alabel_t e1,alabel_t e2) deletes
{
  return call_setif_inclusion(e1,e2);
}

int alabel_t_unify(alabel_t e1,alabel_t e2) deletes
{
  return call_setif_unify(e1,e2);
}

alabel_t_list alabel_t_tlb(alabel_t e) deletes
{
  return setif_tlb(e);
}

void alabel_t_print(FILE* arg1,alabel_t arg2) deletes
{
  return expr_print(arg1,arg2);
}

argT_field argT_make_field(const char *label,T expr)
{
  return flowrow_make_field(label,expr);
}

argT argT_zero(void)
{
  return flowrow_zero(setif_sort);
}

argT argT_one(void)
{
  return flowrow_one(setif_sort);
}

argT argT_abs(void)
{
  return flowrow_abs(setif_sort);
}

argT argT_wild(void)
{
  return flowrow_wild(setif_sort);
}

argT argT_fresh(const char *name)
{
  return flowrow_fresh(name,setif_sort);
}

argT argT_row(argT_map fields,argT rest) deletes
{
  return flowrow_make_row(fields,rest);
}

T argT_extract_field(const char *field_name,argT row)
{
  return flowrow_extract_field(field_name,row);
}

argT argT_extract_rest(argT row)
{
  return flowrow_extract_rest(row);
}

argT_map argT_extract_fields(argT row)
{
  return flowrow_extract_fields(row);
}

int argT_inclusion(argT row1,argT row2) deletes
{
  return call_flowrow_inclusion(row1,row2);
}

int argT_unify(argT row1,argT row2) deletes
{
  return call_flowrow_unify(row1,row2);
}

void argT_print(FILE *f,argT row) deletes
{
  expr_print(f,row);
}

T T_zero(void)
{
  return setif_zero();
}

T T_one(void)
{
  return setif_one();
}

T T_fresh(const char *name)
{
  return setif_fresh(name);
}

T T_union(T_list exprs) deletes
{
  return setif_union(exprs);
}

T T_inter(T_list exprs) deletes
{
  return setif_inter(exprs);
}

T T_constant(const char *name) deletes
{
  return setif_constant(name);
}

/* TODO */
bool T_is_constant(T e,const char *name)
{
  return FALSE;
}

int T_inclusion(T e1,T e2) deletes
{
  return call_setif_inclusion(e1,e2);
}

int T_unify(T e1,T e2) deletes
{
  return call_setif_unify(e1,e2);
}

T_list T_tlb(T e) deletes
{
  return setif_tlb(e);
}

/* TODO */
bool T_is_ref(T e)
{
  return FALSE;
}

T ref(alabel_t arg1,T arg2,T arg3) deletes
{
  gen_e exps[3] = {arg1,arg2,arg3};
  return constructor_expr(ref_c,exps,3);
}

struct ref_decon
{
alabel_t f0;
T f1;
T f2;
};

struct ref_decon ref_decon(T arg1)
{
  struct decon d = deconstruct_expr(ref_c,arg1);
  
  if (d.elems)
    return (struct ref_decon){d.elems[0],d.elems[1],d.elems[2]};
  else
    return (struct ref_decon){NULL,NULL,NULL};
}


alabel_t ref_proj0(T arg1) deletes
{
  return setif_proj(ref_c,0,arg1);
}

T ref_pat0(alabel_t arg1) deletes
{
  return setif_proj_pat(ref_c,0,arg1);
}

T ref_proj1(T arg1) deletes
{
  return setif_proj(ref_c,1,arg1);
}

T ref_pat1(T arg1) deletes
{
  return setif_proj_pat(ref_c,1,arg1);
}

T ref_proj2(T arg1) deletes
{
  return setif_proj(ref_c,2,arg1);
}

T ref_pat2(T arg1) deletes
{
  return setif_proj_pat(ref_c,2,arg1);
}

/* TODO */
bool T_is_lam(T e)
{
  return FALSE;
}

T lam(alabel_t arg1,argT arg2,T arg3) deletes
{
  gen_e exps[3] = {arg1,arg2,arg3};
  return constructor_expr(lam_c,exps,3);
}

struct lam_decon
{
alabel_t f0;
argT f1;
T f2;
};

struct lam_decon lam_decon(T arg1)
{
  struct decon d = deconstruct_expr(lam_c,arg1);
  
  if (d.elems)
    return (struct lam_decon){d.elems[0],d.elems[1],d.elems[2]};
  else
    return (struct lam_decon){NULL,NULL,NULL};
}

alabel_t lam_proj0(T arg1) deletes
{
  return setif_proj(lam_c,0,arg1);
}

T lam_pat0(alabel_t arg1) deletes
{
  return setif_proj_pat(lam_c,0,arg1);
}

argT lam_proj1(T arg1) deletes
{
  return setif_proj(lam_c,1,arg1);
}

T lam_pat1(argT arg1) deletes
{
  return setif_proj_pat(lam_c,1,arg1);
}

T lam_proj2(T arg1) deletes
{
  return setif_proj(lam_c,2,arg1);
}

T lam_pat2(T arg1) deletes
{
  return setif_proj_pat(lam_c,2,arg1);
}

void T_print(FILE* arg1,T arg2) deletes
{
  expr_print(arg1,arg2);
}

void andersen_terms_init(void)
{
  nonspec_init();

  { 
    sig_elt ref_sig[3] ={{vnc_pos,setif_sort},
			 {vnc_neg,setif_sort},
			 {vnc_pos,setif_sort}};
    
    sig_elt lam_sig[3] =  {{vnc_pos,setif_sort},
			   {vnc_neg,flowrow_sort},
			   {vnc_pos,setif_sort}};
    
    
    ref_c = 
      make_constructor("ref",setif_sort,ref_sig,3);
    lam_c =
      make_constructor("lam",setif_sort,lam_sig,3);
  }

}

void andersen_terms_serialize(FILE *f, hash_table *entry_points, 
			      unsigned long sz)
{
  unsigned long i = 0;
  hash_table constructors =
    make_persistent_string_hash_table(4,
				      BANSHEE_PERSIST_KIND_constructor);
  hash_table *all_entry_points =
    rarrayalloc(permanent, sz+1, sizeof(hash_table));

  assert(f);
  assert(entry_points);
  assert(sz);
  hash_table_insert(constructors, (hash_key)"ref",(hash_data)ref_c);
  hash_table_insert(constructors, (hash_key)"lam",(hash_data)lam_c);

  all_entry_points[0] = constructors;
  for(i = 1; i <= sz; i++)
    {
      all_entry_points[i] = entry_points[i-1];
    }

  serialize_cs(f,all_entry_points, sz+1);
}

hash_table *andersen_terms_deserialize(FILE *f)
{
  hash_table *result = deserialize_cs(f);
  
  assert(result);

  ref_c = NULL;
  lam_c = NULL;
  hash_table_lookup(result[0], (hash_key)"ref",(hash_data *)&ref_c);
  hash_table_lookup(result[0], (hash_key)"lam",(hash_data *)&lam_c);

  assert(ref_c);
  assert(lam_c);  

  return &result[1];
}

void andersen_terms_region_serialize(FILE *f)
{
  fwrite((void *)&ref_c,sizeof(constructor),1,f);
  fwrite((void *)&lam_c, sizeof(constructor),1,f);
  write_module_nonspec(f);
}

void andersen_terms_region_deserialize(translation t, FILE *f)
{
  fread((void *)&ref_c,sizeof(constructor),1,f);
  fread((void *)&lam_c, sizeof(constructor),1,f);
  update_pointer(t, (void **)&ref_c);
  update_pointer(t, (void **)&lam_c);
  update_module_nonspec(t, f);
}


void andersen_terms_reset(void) deletes
{
  nonspec_reset();
}

void andersen_terms_stats(FILE * arg1)
{
  nonspec_stats(arg1);
}

/* TODO */
void andersen_terms_print_graph(FILE * arg1)
{

}
