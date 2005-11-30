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

/*
  call_ denotes a sort check (i.e. inclusion between e1 and e2 where the sorts
  of e1 and e2 are unknown or untrusted)

  _ind denotes induced, i.e. a constraint not introduced at the top level
  for induced inclusions/equations, no sort checks are necessary.
  the invariant is that the non-induced relations are ONLY called from the
  toplevel. induced functions must have the signature
  gen_e * gen_e -> void 
  which means they can be passed as function pointers (incl_fn_ptr)
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "banshee.h"
#include "flowrow-sort.h"
#include "flow-var.h"
#include "setif-sort.h"
#include "setif-var.h"
#include "setst-sort.h"
#include "setst-var.h"
#include "term-sort.h"
#include "term-var.h"
#include "utils.h"
#include "hash.h"
#include "ufind.h"
#include "banshee_persist_kinds.h"
#include "banshee_region_persist_kinds.h"

/* Types defined here MUST be larger than LARGEST_BUILTIN_TYPE (banshee.h). */
#define GROUP_PROJ_PAT_TYPE 11

/* Update this whenever a new term type is defined. It MUST be even. */
#define NUM_EXTRA_TYPES 2

typedef enum 
{
  vnc_pos,
  vnc_neg,
  vnc_non
} vnc_kind;

struct sig_elt_
{
  vnc_kind variance;
  sort_kind sort;
};

typedef struct sig_elt_ sig_elt;

typedef sig_elt* sig_elt_ptr;

struct cons_group_ 
{
  int arity;
  char *name;
  sig_elt *sig;
  int gid;
};

typedef struct cons_group_ *cons_group;

DECLARE_LIST(cons_group_list,cons_group);
DEFINE_LIST(cons_group_list,cons_group);
DECLARE_LIST(sig_elt_list, sig_elt_ptr);
DEFINE_NONPTR_LIST(sig_elt_list, sig_elt_ptr);

struct constructor_
{
  sort_kind sort;
  int type;
  int arity;
  char *name;
  sig_elt *sig;
  cons_group_list groups;
};

typedef struct constructor_ *constructor;

typedef struct proj_pat_
{
  sort_kind sort;
  int type;
  stamp st;
  int i;
  gen_e exp;
  vnc_kind variance;
  constructor c;
} *proj_pat;

typedef struct gproj_pat_
{
  sort_kind sort;
  int type;
  stamp st;
  int i;
  gen_e exp;
  cons_group g;
} *gproj_pat;

typedef struct cons_expr_  
{
  sort_kind sort;
  int type;
  stamp st;
  int arity;
  char *name;
  sig_elt *sig;
  gen_e *exps;
  constructor c;
} * cons_expr;

struct decon
{
  char *name;
  int arity;
  gen_e *elems;
};

struct nonspec_stats_
{
  int dispatches;
} nonspec_stats_;

static const int smallest_special_type = LARGEST_BUILTIN_TYPE + NUM_EXTRA_TYPES;

region constructor_region;
region cons_expr_region;
region cons_group_region;
region proj_pat_region;
region gproj_pat_region;
region gen_e_ptr_region;

/* TODO -- this state really needs to be saved, it's a total
   coincidence that it works as is */
static int new_type()
{
  static int next_type = LARGEST_BUILTIN_TYPE + NUM_EXTRA_TYPES;
  int ret;

  assert(next_type %2 == 0);

  ret = next_type;
  next_type += 2;
  return ret;
}
		
// A sort is fixed if it does not permit user-defined constructors
static bool fixed_sort(sort_kind s)
{
  return (s == flowrow_sort);
}

static void term_con_match(gen_e e1, gen_e e2);
static bool term_occurs(term_var v, gen_e e2);
static void setif_con_match(gen_e e1, gen_e e2);
static void setst_con_match(gen_e e1, gen_e e2);
static bool setif_res_proj(setif_var v1,gen_e e2);
static void call_inclusion_ind(gen_e e1, gen_e e2);
static void call_unify_ind(gen_e e1, gen_e e2);
int call_term_unify(gen_e e1, gen_e e2);
void expr_print(FILE *f, gen_e e);

void term_unify_ind(gen_e e1, gen_e e2)
{
  term_unify(term_con_match,term_occurs,e1,e2);
}

static bool setif_is_gpat(gen_e e)
{
  int type = ((setif_term)e)->type;
  return type == GROUP_PROJ_PAT_TYPE;
}

/* 
   Convention : constructor types are even, pats are odd.
   The smallest specialized type is smallest_special_type.
*/
static bool type_is_pat(int type)
{
  return ( (type % 2) && (type > smallest_special_type) );
}

static bool type_is_cons_expr(int type)
{
 return ( !(type % 2) && (type >= smallest_special_type) );
}

static bool setif_is_pat(gen_e e)
{
  int type = ((setif_term)e)->type;
  return type_is_pat(type);
}

static bool setst_is_pat(gen_e e)
{
  int type = ((setst_term)e)->type;
  return type_is_pat(type);
}

static bool setif_is_cons_expr(gen_e e)
{
  int type = ((setif_term)e)->type;
  return type_is_cons_expr(type);
}
 
static bool setst_is_cons_expr(gen_e e)
{
  int type = ((setst_term)e)->type;
 
  return ( !(type % 2) && (type >= smallest_special_type) );
}

static bool term_is_cons_expr(gen_e e)
{
  int type = ((gen_term) term_get_ecr(e))->type;

  return ( !(type % 2) && (type >= smallest_special_type) );
}

static get_stamp_fn_ptr get_sort_stamp(sort_kind s)
{
switch (s)
    {
    case setif_sort:
      return setif_get_stamp;
    case setst_sort:
      return setst_get_stamp;
    case term_sort:
      return term_get_stamp;
    case flowrow_sort:
      return flowrow_get_stamp;
    default:
      fail("Unmatched sort in get_sort_stamp\n");
      return NULL;
    }
  return NULL;
}

static gen_e get_sort_zero(sort_kind s)
{
switch (s)
    {
    case setif_sort:
      return setif_zero();
    case setst_sort:
      return setst_zero();
    case term_sort:
      return term_zero();
    case flowrow_sort:
      // return flowrow_zero();
    default:
      fail("Unmatched sort in get_sort_zero\n");
      return NULL;
    }
  return NULL;
}



static term_hash get_sort_hash(sort_kind s)
{
  switch (s)
    {
    case setif_sort:
      return setif_hash;
    case setst_sort:
      return setst_hash;
    case flowrow_sort:
      return flowrow_hash;
    case term_sort:
      return term_sort_hash;
    default:
      fail("Unmatched sort in get_sort_hash\n");
      return NULL;
    }
  return NULL;
}

static gen_e get_named_proj_var(sort_kind s, bool large, char *name) /* HACK */
{
  switch (s)
    {
    case setif_sort:
      {
	if (large)
	  return (gen_e)setif_fresh_large(name);
	else return (gen_e)setif_fresh(name);	  
      }
      break;
    case setst_sort:
      {
	if (large)
	  return (gen_e)setst_fresh_large(name);
	else return (gen_e)setst_fresh(name);	
      }
      break;
    case flowrow_sort:
      {
	if (large)
	  return (gen_e)flowrow_fresh_large(name,setif_sort);
	else return (gen_e)flowrow_fresh(name,setif_sort);
      }
      break;
    case term_sort:
      {
	if (large)
	  return (gen_e)term_fresh_large(name);
	else return (gen_e)term_fresh(name);
      }	
      break;
    default:
      {
	fail("Unmatched sort in get_proj_var\n");
	return NULL;
      }
      break;
    }

  return NULL;
}

static gen_e get_vinv_proj_var(sort_kind s, constructor c, int i,gen_e e)
{
  char temp[512];
  snprintf(temp,512,"%s-%d(%s)",c->name,i,sv_get_name((setif_var)e));
  return get_named_proj_var(s,FALSE,temp);
}

static gen_e get_cinv_proj_var(sort_kind s,constructor c, int i)
{
  char temp[512];
  snprintf(temp,512,"%s<-%d>",c->name,i);
  return get_named_proj_var(s,FALSE,temp);
}

static gen_e get_proj_var(sort_kind s, bool large)
{
  return get_named_proj_var(s,large,NULL);
}

static void setif_inclusion_ind(gen_e e1,gen_e e2)
{
  setif_inclusion(setif_con_match,setif_res_proj,expr_print,e1,e2);
}

static void setst_inclusion_ind(gen_e e1, gen_e e2)
{
  setst_inclusion(setst_con_match,expr_print,e1,e2);
}

static bool pat_match(int t1, int t2)
{
  return (t1 - 1 == t2);
}

static bool gpat_match(cons_expr ce, gproj_pat pat)
{
  assert(ce->c->groups);
  return cons_group_list_member(ce->c->groups,pat->g);
}

/*
static char * sort_to_string(sort_kind s)
{
  switch(s)
    {
    case setif_sort:
      return "setif";
    case setst_sort:
      return "setst";
    case term_sort:
      return "term";
    case flowrow_sort:
      return "flowrow";
    default:
      return NULL;
    }
}
*/

constructor make_constructor(const char *name,sort_kind sort, sig_elt s[],
			     int arity)
{
  constructor c = ralloc(constructor_region,struct constructor_);
  sig_elt *sig = rarrayalloc(banshee_nonptr_region,arity,sig_elt);
  
  c->type = new_type();

  if (arity) {
    memcpy(sig,s,sizeof(sig_elt)*arity);
  }

  if ( fixed_sort(sort) )
    fail("Specified sort does not allow constructor types\n");
  
  c->sort = sort;
  c->arity = arity;
  c->name = rstrdup(banshee_nonptr_region,name);
  c->sig = sig;

  if (sort == setif_sort) {
    c->groups = new_persistent_cons_group_list();
  }
  else c->groups = NULL;

  return c;
}

constructor make_constructor_from_list(const char*name, sort_kind sort,
				       sig_elt_list elts)
{
  sig_elt_list_scanner scan;
  sig_elt *temp;
  int i = 0;
  int arity = sig_elt_list_length(elts);
  constructor c = ralloc(constructor_region,struct constructor_);
  sig_elt *sig = rarrayalloc(banshee_nonptr_region,arity,sig_elt);
  
  sig_elt_list_scan(elts,&scan);

  while(sig_elt_list_next(&scan,&temp)) {
    sig[i].variance = temp->variance;
    sig[i].sort = temp->sort;
    i++;
  }
  c->sort = sort;
  c->arity = arity;
  c->name = rstrdup(banshee_nonptr_region,name);
  c->sig = sig;
  c->type = new_type();

  if (sort == setif_sort) { 
    c->groups = new_persistent_cons_group_list();
  }
  else c->groups = NULL;

  return c;
}

gen_e constructor_expr(constructor c, gen_e exps[], int arity)
{
  cons_expr result;
  int i;
  get_stamp_fn_ptr get_stamp;
  term_hash sort_hash = get_sort_hash(c->sort);
  
  stamp *st = rarrayalloc(banshee_nonptr_region,arity + 1,stamp);
  st[0] = c->type;
  
  // Dynamic arity check
  if(arity != c->arity)
    {
      fail("Signature mismatch\n");
      return NULL;
    }
  // Dynamic sort checks
  for (i = 0; i < arity; i++)
    {
      if ( c->sig[i].sort != exps[i]->sort)
	{
	  fail("Signature mismatch\n");
	  return NULL;
	}
      get_stamp = get_sort_stamp(c->sig[i].sort);
      st[i+1] = get_stamp(exps[i]);
    }

  // Hash-consing of terms
  if (!(result = (cons_expr)term_hash_find(sort_hash,st,arity+1)) 
      || arity == 0 )
    {
      gen_e *e = rarrayalloc(banshee_ptr_region,arity,gen_e);
      
      if (arity)
	memcpy(e,exps,sizeof(gen_e)*arity);
      else 
	e = NULL;

      result = ralloc(cons_expr_region,struct cons_expr_);  
      result->type = st[0];
      result->st = stamp_fresh();
      result->sort = c->sort;
      result->arity = c->arity;
      result->name = c->name;
      result->sig = c->sig;
      result->exps = e;
      result->c = c;
      
      term_hash_insert(sort_hash,(gen_e)result,st,arity+1);
    }

  return (gen_e)result;
}

/*  gen_e constructor_expr_from_list(constructor c, gen_e_list exps) */
/*  { */
/*    int arity = gen_e_list_length(exps); */
/*  } */


static gen_e make_proj_pat(constructor c, int i, gen_e e)
{
  proj_pat pat;
  term_hash sort_hash = get_sort_hash(e->sort);
  get_stamp_fn_ptr get_stamp = get_sort_stamp(e->sort);
  
  stamp s[3];
  s[0] = c->type + 1;
  s[1] = get_stamp(e);
  s[2] = i;

  if (! (pat = (proj_pat)term_hash_find(sort_hash,s,3)) )
    {
      pat = ralloc(proj_pat_region,struct proj_pat_);
      pat->type = s[0];
      pat->st = stamp_fresh();
      pat->sort = c->sort;
      pat->exp = e;
      pat->variance = c->sig[i].variance;
      pat->c = c;
      pat->i = i;
      term_hash_insert(sort_hash,(gen_e)pat,s,3);
    }
  
  return (gen_e)pat;
}

gen_e setif_proj_pat(constructor c,int i,gen_e e)
{
  if (c->sort != setif_sort)
    {
      fail("Sort check failed: proj_pat\n");
      return NULL;
    }
  return make_proj_pat(c,i,e);
}

gen_e setst_proj_pat(constructor c, int i, gen_e e)
{
  if (c->sort != setst_sort)
    {
      fail("Sort check failed: proj_pat\n");
      return NULL;
    }
  return make_proj_pat(c,i,e);
}

/* for proj, sort(e) must be setif */
gen_e setif_proj(constructor c, int i, gen_e e) 
{
  setif_var v;
  gen_e proj_var, proj;

  gen_e nonspec_get_proj(gen_e_list arg1)
    {
      proj_pat pat;
      gen_e_list_scanner scan;
      gen_e temp;
      
      gen_e_list_scan(arg1,&scan);
      while (gen_e_list_next(&scan,&temp))
	{
	  if (! setif_is_pat(temp) ) continue;
	  pat = (proj_pat)temp;
	  if ( pat_match(pat->type,c->type) && i == pat->i )
	    return pat->exp;
	}
      return NULL;
    }

  banshee_clock_tick();

  if (e->sort != setif_sort)
    {
      fail("Sort check failed: setif_proj\n");
      return NULL;
    }
  
  else if (i < 0 || i >= c->arity)
    {
      fail("Signature mismatch\n");
      return NULL;
    }

  else if (setif_is_zero(e))
    {
      return get_sort_zero(c->sig[i].sort);
    }
  
  else if ( ((setif_term)e)->type == c->type )
    {
      cons_expr constructed = (cons_expr)e;
      return constructed->exps[i];
    }
  
  else if (setif_is_var(e))
    {
      v = (setif_var)e;
      if ( (proj = sv_get_ub_proj(v,nonspec_get_proj)) )
	{
	  return proj;
	}
      else
	{
	  gen_e pat;
	  bounds_scanner scan;
	  gen_e lb;
	  //	  proj_var = get_proj_var(c->sig[i].sort,FALSE);
	  proj_var = get_vinv_proj_var(c->sig[i].sort,c,i,e);
	  pat = setif_proj_pat(c,i,proj_var);
	  sv_add_ub_proj(v,pat);

	  if (!banshee_check_rollback(setif_sort)) {
	    setif_register_rollback();
	  }
	  setif_register_ub_proj(sv_get_ub_projs(v),pat);

	  bounds_scan(sv_get_lbs(v),&scan);
	  while (bounds_next(&scan,&lb))
	    {
	      setif_inclusion_ind(lb,pat);
	    }
	  return proj_var;
	}
    }

  else if (setif_is_union(e))
    {
      if( (proj = nonspec_get_proj(setif_get_proj_cache(e))) )
	return proj;
      else
	{
	  gen_e pat;
	  // proj_var = get_proj_var(c->sig[i].sort,FALSE);
	  proj_var = get_cinv_proj_var(c->sig[i].sort,c,i);
	  pat = setif_proj_pat(c,i,proj_var);
	  
	  setif_set_proj_cache(e,pat);
	  
	  setif_inclusion_ind(e,pat);
	  return proj_var;
	}
    }
  else 
    {
      gen_e pat;
      //     proj_var = get_proj_var(c->sig[i].sort,FALSE);
      proj_var = get_cinv_proj_var(c->sig[i].sort,c,i);
      pat = setif_proj_pat(c,i,proj_var);
      setif_inclusion_ind(e,pat);
      return proj_var;
    }
}

gen_e setst_proj(constructor c, int i, gen_e e)
{
  /* TODO */
  fail("setst_proj\n");
  return NULL;
}

static void setif_con_match(gen_e e1, gen_e e2)
{
  // Case where e1 is a constructor expression and e2 is a gproj_pat
  if (setif_is_cons_expr(e1) && setif_is_gpat(e2) && 
      gpat_match((cons_expr)e1, (gproj_pat)e2) ) {
    cons_expr c = (cons_expr)e1;
    gproj_pat p = (gproj_pat)e2;
    int i = p->i;

    assert(i == -1 || i < c->arity);

    // Calling with i == -1 makes the projection operate over every subterm
    // FIX : check, if this term exists w/ i == -1, it subsumes other pats
    // should they be created?
    if (i == -1) {
      int j;
      
      for (j = 0; j < c->arity; j++) {
	if (c->sig[j].variance == vnc_pos)
	  call_inclusion_ind(c->exps[j],p->exp);
	else if (c->sig[i].variance == vnc_neg)
	  call_inclusion_ind(p->exp,c->exps[j]);
	else
	  call_unify_ind(c->exps[j],p->exp);
      }
    }
    else {
      if (c->sig[i].variance == vnc_pos)
	call_inclusion_ind(c->exps[i],p->exp);
      else if (c->sig[i].variance == vnc_neg)
	call_inclusion_ind(p->exp,c->exps[i]);
      else
	call_unify_ind(c->exps[i],p->exp);
    }
  }
  else if (setif_is_gpat(e2)) { // no match
    return;
  }
  // Case where e1 is a constructor expression and e2 is a proj_pat
  else if (pat_match(((setif_term)e2)->type,((setif_term)e1)->type))
    {
      cons_expr c = (cons_expr)e1;
      proj_pat p = (proj_pat)e2;
      int i = p->i;
      
      if (c->sig[i].variance == vnc_pos)
	call_inclusion_ind(c->exps[i],p->exp);
      else if (c->sig[i].variance == vnc_neg)
	call_inclusion_ind(p->exp,c->exps[i]);
      else
	call_unify_ind(c->exps[i],p->exp);
    }
  else if (setif_is_pat(e2)) 	//  no match
    {
      return;
    }
  
  // Case where e1 and e2 are constructor expressions
  else 
    {
      cons_expr c1 = (cons_expr)e1,
	c2 = (cons_expr)e2;
      
      if (c1->type != c2->type)
	{
	  handle_error(e1,e2,bek_cons_mismatch);
	}
      else
	{
	  int i;
	  for (i = 0; i < c1->arity; i++)
	    {
	      if (c1->sig[i].variance == vnc_pos)
		call_inclusion_ind(c1->exps[i],c2->exps[i]);
	      else if (c1->sig[i].variance == vnc_neg)
		call_inclusion_ind(c2->exps[i],c1->exps[i]);
	      else
		call_unify_ind(c1->exps[i],c2->exps[i]);
	    }
	  
	}
    } 
}


static void setst_con_match(gen_e e1, gen_e e2)
{
  // Case where e1 is a constructor expression and e2 is a proj_pat
  if (pat_match(((setst_term)e2)->type,((setst_term)e1)->type))
    {
      cons_expr c = (cons_expr)e1;
      proj_pat p = (proj_pat)e2;
      int i = p->i;
      
      if (c->sig[i].variance == vnc_pos)
	call_inclusion_ind(c->exps[i],p->exp);
      else if (c->sig[i].variance == vnc_neg)
	call_inclusion_ind(p->exp,c->exps[i]);
      else
	call_unify_ind(c->exps[i],p->exp);
    }
  else if (setst_is_pat(e2)) 
    {
      return;
    }
  
  // Case where e1 and e2 are constructor expressions
  else 
    {
      cons_expr c1 = (cons_expr)e1,
	c2 = (cons_expr)e2;
      
      if (c1->type != c2->type)
	{
	  handle_error(e1,e2,bek_cons_mismatch);
	}
      else
	{
	  int i;
	  for (i = 0; i < c1->arity; i++)
	    {
	      if (c1->sig[i].variance == vnc_pos)
		call_inclusion_ind(e1,e2);
	      else if (c1->sig[i].variance == vnc_neg)
		call_inclusion_ind(e2,e1);
	      else
		call_unify_ind(e1,e2);
	    }
	  
	}
    } 
}

// given x <= proj(c,i,e)
// proj_merge(region,e,get_proj_i_arg,fresh_large_fn_ptr,
// sort_inclusion_fn_ptr,set_inclusion)
static bool setif_res_proj(setif_var v1,gen_e e2)
{
  if (setif_is_pat(e2) ) {
    proj_pat projection_pat;
  

    gen_e setif_get_proj(gen_e_list arg1)
      {
	gen_e_list_scanner scan;
	gen_e temp;
	proj_pat pat;
	
	gen_e_list_scan(arg1,&scan);
	while(gen_e_list_next(&scan,&temp))
	  {
	    if (!setif_is_pat(temp)) continue;
	    pat = (proj_pat)temp;
	    if ( pat->type == ((setif_term)e2)->type && 
		 pat->i == ((proj_pat)e2)->i)
	      return pat->exp;
	  }
	return NULL;
      }
    
    gen_e fresh_large(const char *name)
      {
	return get_proj_var( ((proj_pat)e2)->exp->sort,TRUE);
      }
    
    void sort_inclusion(gen_e e1, gen_e e2)
      {
	if ( projection_pat->variance == vnc_pos )
	  call_inclusion_ind(e1,e2);
	else if ( projection_pat->variance == vnc_neg)
	  call_inclusion_ind(e2,e1);
	else 
	  call_unify_ind(e1,e2);
      }
    
    gen_e proj_con(gen_e e)
      {
	return make_proj_pat( ((proj_pat)e2)->c, ((proj_pat)e2)->i,e);
      }
    
    projection_pat = (proj_pat)e2;
    return setif_proj_merge(v1,((proj_pat)e2)->exp,
			    setif_get_proj,proj_con,
			    fresh_large,sort_inclusion,
			    setif_inclusion_ind);
  }
  else return FALSE;
}

/* Add a field */
flowrow_field flowrow_make_field(const char *name, gen_e e)
{
  flowrow_field result = ralloc(flowrow_field_region,struct flowrow_field_);
  result->label = rstrdup(banshee_nonptr_region,name);
  result->expr = e;
  return result;
}

gen_e flowrow_make_row(flowrow_map fields, gen_e rest)
{
  get_stamp_fn_ptr get_stamp = get_sort_stamp(flowrow_base_sort(rest));
  
  return flowrow_row(get_stamp,fields,rest);
}


/* Does a sort check */
int call_setif_inclusion(gen_e e1,gen_e e2)
{
  banshee_clock_tick();

  if (! ( (e1->sort == e2->sort) && (e1->sort == setif_sort) ) )
    {
      fail("Sort check failed during setif inclusion\n");
    }

  setif_inclusion(setif_con_match,setif_res_proj,expr_print,e1,e2);
  return 0;
}

/* Does a sort check */
int call_setif_unify(gen_e e1, gen_e e2)
{  
  banshee_clock_tick();

  if (! ( (e1->sort == e2->sort) && (e1->sort == setif_sort) ) )
    {
      fail("Sort check failed during setif_unify\n");
    }

  setif_inclusion(setif_con_match,setif_res_proj,expr_print,e1,e2);
  setif_inclusion(setif_con_match,setif_res_proj,expr_print,e2,e1);
  return 0;
}

/* Does a sort check */
int call_setst_inclusion(gen_e e1, gen_e e2)
{
  banshee_clock_tick();

  if (! ( (e1->sort == e2->sort) && (e1->sort == setst_sort) ) )
    {
      fail("Sort check failed: setif_inclusion\n");
    }
  
  setst_inclusion(setst_con_match,expr_print,e1,e2);
  return 0;
}

/* Does a sort check */
int call_setst_unify(gen_e e1, gen_e e2)
{
  banshee_clock_tick();
  
  if (! ( (e1->sort == e2->sort) && (e1->sort == setst_sort) ) )
    {
      fail("Sort check failed: setst_unify\n");
    }

  setst_inclusion(setst_con_match,expr_print,e1,e2);
  setst_inclusion(setst_con_match,expr_print,e2,e1);
  return 0;
}

static void flowrow_inclusion_ind(gen_e e1, gen_e e2)
{
  fresh_fn_ptr fresh;
  get_stamp_fn_ptr get_stamp;
  incl_fn_ptr field_incl;
  gen_e zero_elem;

  if (flowrow_base_sort(e1) != flowrow_base_sort(e2))
    fail("Row base sorts do not match\n");

   switch(flowrow_base_sort(e2))
    {
    case setif_sort:
      {
	fresh = setif_fresh;
	get_stamp = setif_get_stamp;
	field_incl = setif_inclusion_ind;
	zero_elem = setif_zero();
      }
      break;
    case setst_sort:
      {
	fresh = setst_fresh;
	get_stamp = setst_get_stamp;
	field_incl = setst_inclusion_ind;
	zero_elem = setst_zero();
      }
      break;
    case term_sort:
      {
	fresh = term_fresh;
	get_stamp = term_get_stamp;
	field_incl = term_unify_ind;
	zero_elem = term_zero();
      }
      break;
    default:
      {
	fresh = NULL;
	get_stamp = NULL;
	field_incl = NULL;
	zero_elem = NULL;
	fail("Flowrow inclusion: unmatched base sort\n");
      }
      break;
    }

  flowrow_inclusion(fresh,get_stamp,field_incl,zero_elem,e1,e2);
}

/* Does a sort check */
int call_flowrow_inclusion(gen_e e1,gen_e e2)
{  
  banshee_clock_tick();

  if ( (e1->sort != flowrow_sort) || (e2->sort != flowrow_sort) )
    {
      fail("Sort check failed: flowrow_inclusion\n");
    }
  if ( flowrow_base_sort(e1) != flowrow_base_sort(e2))
    {
      fail("Base sort check failed: flowrow_inclusion\n");
    }

  
  flowrow_inclusion_ind(e1,e2);
  return 0;
}

/* Does a sort check */
int call_flowrow_unify(gen_e e1, gen_e e2)
{
  banshee_clock_tick();
  
  if ( (e1->sort != flowrow_sort) || (e2->sort != flowrow_sort) )
    {
      fail("Sort check failed: flowrow_inclusion\n");
    }
  if ( flowrow_base_sort(e1) != flowrow_base_sort(e2))
    {
      fail("Base sort check failed: flowrow_inclusion\n");
    }

  flowrow_inclusion_ind(e1,e2);
  flowrow_inclusion_ind(e2,e1);
  return 0;
}

static void term_con_match(gen_e e1, gen_e e2)
{
  cons_expr c1 = (cons_expr)e1,
    c2 = (cons_expr)e2;
  
  if (c1->type != c2->type)
    {
      handle_error(e1,e2,bek_cons_mismatch);
     }
  else
    {
      int i;
      for (i = 0; i < c1->arity; i++)
	{
	  call_unify_ind(c1->exps[i],c2->exps[i]);
	}
      
    }
}

static bool term_occurs(term_var v, gen_e e)
{
  gen_e ecr = term_get_ecr(e);
  
  if (((gen_term)ecr)->type == VAR_TYPE)
    return ( term_get_stamp((gen_e)v) == term_get_stamp(e) );

  else if (((gen_term)ecr)->type >= smallest_special_type)
    {
      cons_expr c_e = (cons_expr) e;
      int i;
      for (i = 0; i < c_e->arity; i++)
	{
	  if (term_occurs(v,c_e->exps[i]))
	    return TRUE;
	}
    }
  
  return FALSE;
}


int call_term_unify(gen_e e1, gen_e e2)
{
  banshee_clock_tick();
  
  if ( (e1->sort != term_sort) || (e2->sort != term_sort) )
    {
      fail("Sort check failed: term_unify\n");
    }

  term_unify(term_con_match,term_occurs,e1,e2);
  return 0;
}

int call_term_cunify(gen_e e1, gen_e e2)
{
  banshee_clock_tick();
  
  if ( (e1->sort != term_sort) || (e2->sort != term_sort) )
    {
      fail("Sort check failed: term_unify\n");
    }

  term_cunify(term_con_match,term_occurs,e1,e2);
  return 0;
}

static void call_inclusion_ind(gen_e e1, gen_e e2)
{
  nonspec_stats_.dispatches++;

  switch (e1->sort)
    {
    case setif_sort:
      {
	setif_inclusion(setif_con_match,setif_res_proj,expr_print,e1,e2);
      }
      break;
    case setst_sort:
      {
	setst_inclusion(setst_con_match,expr_print,e1,e2);
      }
      break;
    case term_sort:
      {
	term_unify(term_con_match,term_occurs,e1,e2);
      }    
      break;
    case flowrow_sort:
      {
	flowrow_inclusion_ind(e1,e2);
      }
      break;
    default :
      fail("Unmatched sort in call inclusion\n");
    }
  return;
}

static void call_unify_ind(gen_e e1, gen_e e2)
{
 nonspec_stats_.dispatches++;

  switch (e1->sort)
    {
    case setif_sort:
      {
	setif_inclusion(setif_con_match,setif_res_proj,expr_print,e1,e2);
	setif_inclusion(setif_con_match,setif_res_proj,expr_print,e2,e1);
      }
      break;
    case setst_sort:
      {
	setst_inclusion(setst_con_match,expr_print,e1,e2);
	setst_inclusion(setst_con_match,expr_print,e2,e1);
      }
      break;
    case term_sort:
      {
	term_unify(term_con_match,term_occurs,e1,e2);
      }    
      break;
    case flowrow_sort:
      {
	flowrow_inclusion_ind(e1,e2);
	flowrow_inclusion_ind(e2,e1);
      }
      break;
    default :
      fail("Unmatched sort in call inclusion\n");
    }
  return;
}

// Returns true if c is NULL, otherwise, checks to see if expr_type
// matches c's type
static bool check_cons_match(constructor c, int expr_type)
{
  if (c) return c->type == expr_type;
  else return TRUE;
}

static struct decon deconstruct_expr_aux(constructor c,gen_e e)
{
  switch (e->sort)
    {
    case setif_sort:
      {
	if ( setif_is_cons_expr(e) && check_cons_match(c,((setif_term)e)->type) )
	  {
	    cons_expr ce = (cons_expr)e;
	    gen_e *elems = rarrayalloc(banshee_ptr_region,ce->arity,
				       gen_e);
	    memcpy(elems,ce->exps,sizeof(gen_e)*ce->arity);
	    return (struct decon){ce->name,ce->arity,elems};
	  }
	else goto NONE;
      }
      break;
    case setst_sort:
      {
	if ( setst_is_cons_expr(e) && check_cons_match(c,((setst_term)e)->type) )
	  {
	    cons_expr ce = (cons_expr)e;
	    gen_e *elems = rarrayalloc(banshee_ptr_region,ce->arity,
				       gen_e);
	    memcpy(elems,ce->exps,sizeof(gen_e)*ce->arity);
	    return (struct decon){ce->name,ce->arity,elems};
	  }
	else goto NONE;
      }
      break;
    case term_sort:
      {
	if ( term_is_cons_expr(e) && 
	     check_cons_match(c, ((gen_term)term_get_ecr(e))->type) )
	  {
	    cons_expr ce = (cons_expr)term_get_ecr(e);
	    gen_e *elems = rarrayalloc(banshee_ptr_region,ce->arity,
				       gen_e);
	    memcpy(elems,ce->exps,sizeof(gen_e)*ce->arity);
	    return (struct decon){ce->name,ce->arity,elems};
	  }
	else goto NONE;
      }
      break;
    case flowrow_sort:
    default:
      {
	goto NONE;
      }
    }
  
 NONE:
  return (struct decon){NULL,-1,NULL}; // FIX : is it ok to have changed this to -1??
}

struct decon deconstruct_expr(constructor c,gen_e e)
{
  return deconstruct_expr_aux(c,e);
}

struct decon deconstruct_any_expr(gen_e e)
{
  return deconstruct_expr_aux(NULL,e);
}


void nonspec_init(void)
{
  region_init();

  cons_group_region = newregion();
  constructor_region = newregion();
  cons_expr_region = newregion();
  cons_group_region = newregion();
  proj_pat_region = newregion();
  gproj_pat_region = newregion();
  gen_e_ptr_region = newregion();

  engine_init();
  term_init();
  setif_init();
  setst_init();
  flowrow_init();
}

void nonspec_reset(void)
{
  deleteregion(cons_group_region);
  deleteregion(constructor_region);
  deleteregion(cons_expr_region);
  deleteregion(proj_pat_region);
  deleteregion(gproj_pat_region);
  deleteregion(gen_e_ptr_region);

  cons_group_region = newregion();
  constructor_region = newregion();
  cons_expr_region = newregion();
  cons_group_region = newregion();
  proj_pat_region = newregion();
  gproj_pat_region = newregion();
  gen_e_ptr_region = newregion();

  engine_reset();
  flowrow_reset();
  setst_reset();
  setif_reset();
  term_reset();
}

int update_gen_e_ptr(translation t, void *m)
{
  update_pointer(t, m);
  return sizeof(void *);
}

static void cons_expr_print(FILE *f, cons_expr ce)
{
  if (ce->arity == 0)
    fprintf(f,"%s",ce->name);
  else
    {
      int i;
      fprintf(f,"%s(",ce->name);
      expr_print(f,ce->exps[0]);
      for (i = 1; i < ce->arity; i++)
	{
	  fprintf(f,",");
	  expr_print(f,ce->exps[i]);
	}
      fprintf(f,")");
    }
}

static void pat_print(FILE *f, proj_pat pp)
{
  fprintf(f,"Proj[%s,%d,",pp->c->name,pp->i);
  expr_print(f,pp->exp);
  fprintf(f,"]");
}

static void gpat_print(FILE *f, gproj_pat pp)
{
  fprintf(f,"GProj[%s,%d,",pp->g->name,pp->i);
  expr_print(f,pp->exp);
  fprintf(f,"]");
}

static void setif_union_print(FILE *f, gen_e e)
{
  gen_e_list_scanner scan;
  gen_e temp;
  int i = 0;

  gen_e_list_scan(setif_get_union(e),&scan);
  while (gen_e_list_next(&scan,&temp))
    {
      if (i++ == 0)
	expr_print(f,temp);
      else
	{
	  fprintf(f," || ");
	  expr_print(f,temp);
	}
    }
}

static void setst_union_print(FILE *f, gen_e e)
{
  gen_e_list_scanner scan;
  gen_e temp;
  int i = 0;

  gen_e_list_scan(setst_get_union(e),&scan);
  while (gen_e_list_next(&scan,&temp))
    {
      if (i++ == 0)
	expr_print(f,temp);
      else
	{
	  fprintf(f," || ");
	  expr_print(f,temp);
	}
    }
}

static void setif_inter_print(FILE *f, gen_e e)
{
  gen_e_list_scanner scan;
  gen_e temp;
  int i = 0;

  gen_e_list_scan(setif_get_inter(e),&scan);
  while (gen_e_list_next(&scan,&temp))
    {
      if (i++ == 0)
	expr_print(f,temp);
      else
	{
	  fprintf(f," && ");
	  expr_print(f,temp);
	}
    }
}

static void setst_inter_print(FILE *f, gen_e e)
{
  gen_e_list_scanner scan;
  gen_e temp;
  int i = 0;

  gen_e_list_scan(setst_get_inter(e),&scan);
  while (gen_e_list_next(&scan,&temp))
    {
      if (i++ == 0)
	expr_print(f,temp);
      else
	{
	  fprintf(f," && ");
	  expr_print(f,temp);
	}
    }
}


void expr_print(FILE *f,gen_e e)
{
  switch(e->sort)
    {
    case setif_sort:
      {
	if (setif_is_var(e))
	  {
	    fprintf(f,"%s", sv_get_name((setif_var)e));
	  }
	else if (setif_is_zero(e))
	  {
	    fprintf(f,"0");
	  }
	else if (setif_is_one(e))
	  {
	    fprintf(f,"1");
	  }
	else if (setif_is_pat(e))
	  {
	    pat_print(f,(proj_pat)e);
	  }
	else if (setif_is_gpat(e)) 
	  {
	    gpat_print(f,(gproj_pat)e);
	  }
	else if (setif_is_union(e))
	  {
	    setif_union_print(f,e);
	  }
	else if (setif_is_inter(e))
	  {
	    setif_inter_print(f,e);
	  }
	else if (setif_is_constant(e))
	  {
	    fprintf(f,"%s",setif_get_constant_name(e));
	  }
	else 
	  {
	    assert(setif_is_cons_expr(e));
	    cons_expr_print(f,(cons_expr)e);
	  }
      }
      break;
 case setst_sort:
      {
	if (setst_is_var(e))
	  {
	    fprintf(f,"%s", st_get_name((setst_var)e));
	  }
	else if (setst_is_zero(e))
	  {
	    fprintf(f,"0");
	  }
	else if (setst_is_one(e))
	  {
	    fprintf(f,"1");
	  }
	else if (setst_is_pat(e))
	  {
	    pat_print(f,(proj_pat)e);
	  }
	else if (setst_is_union(e))
	  {
	    setst_union_print(f,e);
	  }
	else if (setst_is_inter(e))
	  {
	    setst_inter_print(f,e);
	  }
	else 
	  {
	    cons_expr_print(f,(cons_expr)e);
	  }
      }
      break;
    case term_sort:
      {
	gen_e ecr = term_get_ecr(e);
	if (term_is_var(ecr))
	  {
	    fprintf(f,"%s", tv_get_name((term_var)ecr));
	  }
	else if (term_is_zero(ecr))
	  {
	    fprintf(f,"0");
	  }
	else if (term_is_one(ecr))
	  {
	    fprintf(f,"1");
	  }
	else if (term_is_constant(ecr)) 
	  {
	    fprintf(f,"%s",term_get_constant_name(ecr));
	  }
	else 
	  {
	    cons_expr_print(f,(cons_expr)ecr);
	  }
      }
      break;
    case flowrow_sort:
      {
	flowrow_print(f,get_sort_stamp(flowrow_base_sort(e)),expr_print,e);
      }
      break;
    default:
      {
	fail("Unmatched sort: expr_print\n");
      }
      break;
    }
}

static void print_nonspec_stats(FILE *f)
{
  return;
}

void nonspec_stats(FILE *f)
{
  print_nonspec_stats(f);
  engine_stats(f);
}

bool expr_eq(gen_e e1, gen_e e2)
{
  if (e1->sort != e2->sort) {
    return FALSE;
  }
  else {
    bool result = FALSE;
    get_stamp_fn_ptr get_stamp = get_sort_stamp(e1->sort);
    result = (get_stamp(e1) == get_stamp(e2));
    return result;
  }
}

bool expr_is_constant(gen_e e)
{
  if (e->sort != setif_sort) return FALSE;

  return setif_is_constant(e);
}

char *expr_constant_name(gen_e e)
{
  if (! expr_is_constant(e)) return NULL;

  return setif_get_constant_name(e);
}

int expr_stamp(gen_e e)
{
  get_stamp_fn_ptr get_stamp = get_sort_stamp(e->sort);
  return get_stamp(e);
}

sort_kind expr_sort(gen_e e)
{
  return e->sort;
}


void register_error_handler(banshee_error_handler_fn error_handler)
{
  handle_error = error_handler;
}

/* Groups */
cons_group make_cons_group(const char *name, sig_elt s[], int arity)
{
  static int next_gid = 0;
  cons_group g = ralloc(cons_group_region, struct cons_group_);
  sig_elt *sig = NULL;
  
  if (arity > 0) {
    sig = rarrayalloc(banshee_nonptr_region, arity, sig_elt);
    memcpy(sig,s,sizeof(sig_elt)*arity);
  }
  
  g->arity = arity;
  g->name = rstrdup(banshee_nonptr_region,name);
  g->sig = sig;
  g->gid = next_gid++;

  return g;
}

void cons_group_add(cons_group g, constructor c)
{
  int i;
  if (c->sort != setif_sort) {
    fail("Attempted to add %s to group %s, but %s is not a setif constructor", 
	 c->name, g->name); 
  }

  if (c->arity != g->arity && g->arity != -1) {
    fail("Attempted to add %s to group %s, but there was an arity mismatch",
	 c->name, g->name);
  }
  
  if (g->arity != -1) {
    for (i = 0; i < c->arity; i++) {
      if (g->sig[i].variance != c->sig[i].variance || g->sig[i].sort != c->sig[i].sort) {
	fail("Attempted to add %s to group %s, but there was a signature mismatch",
	     c->name, g->name);
      }
    }
  }

  assert(c->groups);
  cons_group_list_cons(g,c->groups);
}

static gen_e make_group_proj_pat(cons_group g, int i, gen_e e)
{
  gproj_pat pat;
  term_hash sort_hash = get_sort_hash(e->sort);
  get_stamp_fn_ptr get_stamp = get_sort_stamp(e->sort);
  
  stamp s[4];
  s[0] = GROUP_PROJ_PAT_TYPE;
  s[1] = g->gid;
  s[2] = get_stamp(e);
  s[3] = i;

  if (! (pat = (gproj_pat)term_hash_find(sort_hash,s,4)) )
    {
      pat = ralloc(gproj_pat_region,struct gproj_pat_);
      pat->sort = setif_sort;
      pat->type = s[0];
      pat->st = stamp_fresh();
      pat->exp = e;


      //pat->variance = g->sig[i].variance;
      pat->g = g;
      pat->i = i;
      term_hash_insert(sort_hash,(gen_e)pat,s,4);
    }
  
  return (gen_e)pat;
}

gen_e setif_group_proj_pat(cons_group g, int i, gen_e e)
{
  return make_group_proj_pat(g,i,e);
}

int call_sort_inclusion(gen_e e1, gen_e e2) 
{
  if (e1->sort != e2->sort)
    {
      fail("Sort check failed during inclusion call\n");
    }

  switch (e1->sort)
    {
    case setif_sort:
      {
	call_setif_inclusion(e1,e2);
      }
      break;
    case setst_sort:
      {
	call_setst_inclusion(e1,e2);
      }
      break;
    case term_sort:
      {
	call_term_unify(e1,e2);
      }    
      break;
    case flowrow_sort:
      {
	call_flowrow_inclusion(e1,e2);
      }
      break;
    default :
      fail("Unmatched sort in call inclusion\n");
    }
 return 0;
}


int call_sort_unify(gen_e e1, gen_e e2)
{
if (e1->sort != e2->sort)
    {
      fail("Sort check failed during inclusion call\n");
    }

  switch (e1->sort)
    {
    case setif_sort:
      {
	call_setif_unify(e1,e2);
      }
      break;
    case setst_sort:
      {
	call_setst_unify(e1,e2);
      }
      break;
    case term_sort:
      {
	call_term_unify(e1,e2);
      }    
      break;
    case flowrow_sort:
      {
	call_flowrow_unify(e1,e2);
      }
      break;
    default :
      fail("Unmatched sort in call unify\n");
    }
 return 0;
}

bool cons_group_serialize(FILE *f, void *obj)
{
  cons_group g = (cons_group)obj;
  assert(f);
  assert(g);

  fwrite((void *)&g->arity, sizeof(int), 1, f);
  string_data_serialize(f,g->name);
  fwrite((void *)g->sig, sizeof(sig_elt), g->arity, f);
  fwrite((void *)&g->gid, sizeof(int), 1, f);
  
  return TRUE;
}

void *cons_group_deserialize(FILE *f)
{
  cons_group g = ralloc(cons_group_region, struct cons_group_);
  assert(f);

  fread((void *)&g->arity, sizeof(int), 1, f);
  g->name = (char *)string_data_deserialize(f);
  g->sig = rarrayalloc(permanent, g->arity, sig_elt);
  fread((void *)g->sig, sizeof(sig_elt), g->arity, f);
  fread((void *)&g->gid, sizeof(int), 1, f);

  return g;
}

bool cons_group_set_fields(void *obj)
{
  return TRUE;
}

/* Persistence */

static bool setif_proj_pat_serialize(FILE *f, proj_pat pat)
{
  assert(f);

  fwrite((void *)&pat->st, sizeof(int), 1, f);
  fwrite((void *)&pat->i, sizeof(int), 1, f);
  fwrite((void *)&pat->exp, sizeof(void *), 1, f);
  fwrite((void *)&pat->variance, sizeof(int), 1, f);
  fwrite((void *)&pat->c, sizeof(constructor), 1, f);

  serialize_banshee_object(pat->exp, gen_e);
  serialize_banshee_object(pat->c, constructor);

  return TRUE;
}

static void *setif_proj_pat_deserialize(FILE *f)
{
  proj_pat pat = ralloc(proj_pat_region, struct proj_pat_);
  assert(f);
  fread((void *)&pat->st, sizeof(int), 1, f);
  fread((void *)&pat->i, sizeof(int), 1, f);
  fread((void *)&pat->exp, sizeof(void *), 1, f);
  fread((void *)&pat->variance, sizeof(int), 1, f);
  fread((void *)&pat->c, sizeof(constructor), 1, f);
  
  return pat;
}

static bool setif_proj_pat_set_fields(proj_pat pat)
{
  deserialize_set_obj((void **)&pat->exp);
  deserialize_set_obj((void **)&pat->c);
  return TRUE;
}

static bool setif_gproj_pat_serialize(FILE *f, gproj_pat gpat)
{
  assert(f);
  assert(gpat);

  fwrite((void *)&gpat->st, sizeof(stamp), 1, f);
  fwrite((void *)&gpat->i, sizeof(int), 1, f);
  fwrite((void *)&gpat->exp, sizeof(void *), 1, f);
  fwrite((void *)&gpat->g, sizeof(cons_group), 1, f);

  serialize_banshee_object(gpat->exp, gen_e);
  serialize_banshee_object(gpat->g, cons_group);

  return TRUE;
}

static void *setif_gproj_pat_deserialize(FILE *f)
{
  gproj_pat gpat = ralloc(gproj_pat_region, struct gproj_pat_);
  assert(f);

  fread((void *)&gpat->st, sizeof(stamp), 1, f);
  fread((void *)&gpat->i, sizeof(int), 1, f);
  fread((void *)&gpat->exp, sizeof(void *), 1, f);
  fread((void *)&gpat->g, sizeof(cons_group), 1, f);

  return gpat;
}

static bool setif_gproj_pat_set_fields(gproj_pat gpat)
{
  deserialize_set_obj((void **)&gpat->exp);
  deserialize_set_obj((void **)&gpat->g);

  return TRUE;
}


static bool cons_expr_serialize(FILE *f, cons_expr e)
{
  int i;
  assert(f);
  assert(e);

  fwrite((void *)&e->st, sizeof(stamp), 1, f);
  fwrite((void *)&e->arity, sizeof(int), 1, f);
  string_data_serialize(f, e->name);
  fwrite((void *)e->sig, sizeof(sig_elt), e->arity, f);
  fwrite((void *)e->exps, sizeof(gen_e), e->arity, f);
  fwrite((void *)&e->c, sizeof(constructor), 1, f);

  for (i = 0; i < e->arity; i++) {
    serialize_banshee_object(e->exps[i], gen_e);
  }
  serialize_banshee_object(e->c, constructor);

  return TRUE;
}

static void *cons_expr_deserialize(FILE *f)
{
  cons_expr e = ralloc(cons_expr_region, struct cons_expr_);
  assert(f);

  fread((void *)&e->st, sizeof(stamp), 1, f);
  fread((void *)&e->arity, sizeof(int), 1, f);
  e->name = (char *)string_data_deserialize(f);
  e->sig = rarrayalloc(permanent, e->arity, sig_elt);
  e->exps = rarrayalloc(permanent, e->arity, gen_e);
  fread((void *)e->sig, sizeof(sig_elt), e->arity, f);
  fread((void *)e->exps, sizeof(gen_e), e->arity, f);
  fread((void *)&e->c, sizeof(constructor), 1, f);

  return e;
}

static bool cons_expr_set_fields(cons_expr e)
{
  int i;
  for (i = 0; i < e->arity; i++) {
    deserialize_set_obj((void **)&e->exps[i]);
  }
  deserialize_set_obj((void **)&e->c);

  return TRUE;
}

static bool setif_expr_serialize(FILE *f, gen_e e)
{
  assert(f);
  fwrite((void *)&((setif_term)e)->type, sizeof(int), 1, f);

  if (setif_is_var(e))
    {
      return setif_var_serialize(f, e);
    }
  else if (setif_is_zero(e))
    {
      return TRUE;
    }
  else if (setif_is_one(e))
    {
      return TRUE;
    }
  else if (setif_is_pat(e))
    {
      return setif_proj_pat_serialize(f, (proj_pat)e);
    }
  else if (setif_is_gpat(e)) 
    {
      return setif_gproj_pat_serialize(f, (gproj_pat)e);
    }
  else if (setif_is_union(e))
    {
      return setif_union_serialize(f, e);
    }
  else if (setif_is_inter(e))
    {
      return setif_inter_serialize(f, e);
    }
  else if (setif_is_constant(e))
    {
      return setif_constant_serialize(f, e);
    }
  else 
    {
      assert(setif_is_cons_expr(e));
      return cons_expr_serialize(f, (cons_expr) e);
    }
}

static void *setif_expr_deserialize(FILE *f)
{
  int expr_type;
  setif_term result = NULL;

  assert(f);
  fread((void *)&expr_type, sizeof(int), 1, f);

  switch(expr_type) 
    {
    case VAR_TYPE:
      result = setif_var_deserialize(f);
      result->type = expr_type;
      break;
    case ZERO_TYPE:
      result = (setif_term)setif_zero();
      break;
    case ONE_TYPE:
      result = (setif_term)setif_one();
      break;
    case UNION_TYPE:
      result = setif_union_deserialize(f);
      result->type = expr_type;
      break;
    case INTER_TYPE:
      result = setif_inter_deserialize(f);
      result->type = expr_type;
      break;
    case CONSTANT_TYPE:
      result = setif_constant_deserialize(f);
      result->type = expr_type;
      break;
    case GROUP_PROJ_PAT_TYPE:
      result = setif_gproj_pat_deserialize(f);
      result->type = expr_type;
    default:
      if (type_is_pat(expr_type)) {
	result = setif_proj_pat_deserialize(f);
      }
      else if (type_is_cons_expr(expr_type)) {
	result = cons_expr_deserialize(f);
      }
      else {
	fail("Unknown type in setif_expr_deserialize.\n");
      }
      result->type = expr_type;
      break;
    }

  return result;
}

static bool setif_expr_set_fields(void *obj)
{
  gen_e e = (gen_e)obj;
  if (setif_is_var(e))
    {
      return setif_var_set_fields(e);
    }
  else if (setif_is_zero(e))
    {
      return TRUE;
    }
  else if (setif_is_one(e))
    {
      return TRUE;
    }
  else if (setif_is_pat(e))
    {
      return setif_proj_pat_set_fields((proj_pat)e);
    }
  else if (setif_is_gpat(e)) 
    {
      return setif_gproj_pat_set_fields((gproj_pat)e);
    }
  else if (setif_is_union(e))
    {
      return setif_union_set_fields(e);
    }
  else if (setif_is_inter(e))
    {
      return setif_inter_set_fields(e);
    }
  else if (setif_is_constant(e))
    {
      return setif_constant_set_fields(e);
    }
  else 
    {
      assert(setif_is_cons_expr(e));
      return cons_expr_set_fields((cons_expr)e);
    }
}

static bool setst_expr_serialize(FILE *f, gen_e e)
{
  assert(0);
  return FALSE;
}

static void * setst_expr_deserialize(FILE *f)
{
  assert(0);
  return NULL;
}

static bool setst_expr_set_fields(void *obj)
{
  assert(0);
  return FALSE;
}

static bool term_expr_serialize(FILE *f, gen_e e)
{
  fwrite((void *)&((gen_term)e)->type, sizeof(int), 1, f);
  if (term_is_initial_var(e))
    {
      return term_var_serialize(f, e);
    }
  else if (term_is_zero(e))
    {
      return TRUE;
    }
  else if (term_is_one(e))
    {
      return TRUE;
    }
  else if (term_is_constant(e))
    {
      return term_constant_serialize(f,e);
    }
  else 
    {
      return cons_expr_serialize(f, (cons_expr)e);
    }
  
  return TRUE;
}

static void *term_expr_deserialize(FILE *f)
{
  int expr_type;
  gen_term result = NULL;
  fread((void *)&expr_type, sizeof(int), 1, f);

  switch(expr_type) 
    {
    case VAR_TYPE:
      result = term_var_deserialize(f);
      result->type = expr_type;
      break;
    case ZERO_TYPE:
      result = (gen_term)term_zero();
      break;
    case ONE_TYPE:
      result = (gen_term)term_one();
      break;
    case CONSTANT_TYPE:
      result = term_constant_deserialize(f);
      result->type = expr_type;
      break;
    default:
      result = cons_expr_deserialize(f);
      result->type = expr_type;
      break;
    }

  return result;
}

static bool term_expr_set_fields(void *obj)
{
  gen_e e = (gen_e)obj;

  if (term_is_initial_var(e))
    {
      return term_var_set_fields(e);
    }
  else if (term_is_zero(e))
    {
      return TRUE;
    }
  else if (term_is_one(e))
    {
      return TRUE;
    }
  else if (term_is_constant(e))
    {
      return TRUE;
    }
  else 
    {
      return cons_expr_set_fields((cons_expr)e);
    }
  
  return TRUE;
}

bool gen_e_serialize(FILE *f, void *obj)
{
  gen_e e = (gen_e)obj;
  assert(f);
  assert(e);

  fwrite((void *)&e->sort, sizeof(int), 1, f);
  switch(e->sort)
    {
    case setif_sort:
      return setif_expr_serialize(f, e);
    case setst_sort:
      return setst_expr_serialize(f, e);
    case term_sort:
      return term_expr_serialize(f, e);
    case flowrow_sort:
      return flowrow_expr_serialize(f, e);
    default:
      fail("Unknown sort in gen_e_serialize.\n");
      return FALSE;
    }
}

void *gen_e_deserialize(FILE *f)
{
  gen_e result = NULL;
  sort_kind sort;
  assert(f);

  fread((void *)&sort, sizeof(int), 1, f);

  switch(sort) 
    {
    case setif_sort:
      result = setif_expr_deserialize(f);
      break;
    case setst_sort:
      result = setst_expr_deserialize(f);
      break;
    case term_sort:
      result = term_expr_deserialize(f);
      break;
    case flowrow_sort:
      result = flowrow_expr_deserialize(f);
      break;
    default:
      fail("Unknown sort in gen_e deserialize.\n");
      return NULL;
    }
  
  result->sort = sort;
  return result;
}

bool gen_e_set_fields(void *obj)
{
  gen_e e = (gen_e)obj;
  switch(e->sort)
    {
    case setif_sort:
      return setif_expr_set_fields(obj);
    case setst_sort:
      return setst_expr_set_fields(obj);
    case term_sort:
      return term_expr_set_fields(obj);
    case flowrow_sort:
      return flowrow_expr_set_fields(obj);
    default:
      fail("Unknown sort in gen_e_set_fields.\n");
      return FALSE;
    }
}

bool constructor_serialize(FILE *f, void *obj)
{
  constructor c = (constructor)obj;
  fwrite((void *)&c->sort, sizeof(int), 1, f);
  fwrite((void *)&c->type, sizeof(int), 1, f);
  fwrite((void *)&c->arity, sizeof(int), 1, f);
  string_data_serialize(f,c->name);
  fwrite((void *)c->sig, sizeof(sig_elt), c->arity, f);
  fwrite((void *)&c->groups, sizeof(cons_group_list), 1, f);

  serialize_banshee_object(c->groups, list);

  return TRUE;
}

void *constructor_deserialize(FILE *f)
{
  constructor c = ralloc(constructor_region, struct constructor_);
  fread((void *)&c->sort, sizeof(int), 1, f);
  fread((void *)&c->type, sizeof(int), 1, f);
  fread((void *)&c->arity, sizeof(int), 1, f);
  c->name = (char *)string_data_deserialize(f);
  c->sig = rarrayalloc(permanent, c->arity, sig_elt);
  fread((void *)c->sig, sizeof(sig_elt), c->arity, f);
  fread((void *)&c->groups, sizeof(cons_group_list), 1, f);

  return c;
}

bool constructor_set_fields(void *obj)
{
  deserialize_set_obj((void **)&((constructor)obj)->groups);
  return TRUE;
}

void serialize_cs(FILE *f, hash_table *entry_points, 
		  unsigned long sz)
{
  unsigned long i;

  assert(f);

   /* Start the serialization manager */
  banshee_serialize_start(f);

  /* Write down the size of the array */
  fwrite((void *)&sz, sizeof(unsigned long), 1, f);
  /* Write down the address of each table */
  fwrite((void *)entry_points, sizeof(hash_table), sz, f);

  /* Serialize each of the tables */
  for (i = 0; i < sz; i++) {
    serialize_banshee_object(entry_points[i], hash_table);
  }

  /* Serialize the remaining state */
  engine_serialize(f);
  stamp_serialize(f);
  uf_serialize(f);
  setif_serialize(f);
  setst_serialize(f);
  flowrow_serialize(f);
  term_serialize(f);

  banshee_serialize_end();
  
  /* Finally, close the file */
  fclose(f);
}

/* Write out any nonpointer static data in any modules */
void write_module_nonspec(FILE *f)
{
  write_module_engine(f);
  write_module_stamp(f);
  write_module_uf(f);
  write_module_setif(f);
  write_module_flowrow(f);
  write_module_term(f);
}

/* Call the pointer update routines with the given translation, and
   also read any extra information (static data) from f  */
void update_module_nonspec(translation t, FILE *f)
{
  assert(f);
  
  /* Reset the constraint solver */
  //engine_reset();

  /* Update any static data (pointer and nonpointer) in f */
  update_module_engine(t, f);
  update_module_stamp(t, f);
  update_module_uf(t, f);
  update_module_setif(t, f);
  update_module_flowrow(t, f);
  update_module_term(t, f);

  /* Finally, close the file */
  fclose(f);
}

hash_table *deserialize_cs(FILE *f)
{
  unsigned long sz;
  hash_table *entry_points;
  unsigned long i;

  assert(f);

 /* Reset the constraint solver */
  engine_reset();

  /* Read the size of the array */
  fread((void *)&sz, sizeof(unsigned long), 1, f);
  entry_points = rarrayalloc(permanent, sz, hash_table);

  /* Read the address of each table */
  fread((void *)entry_points, sizeof(hash_table), sz, f);

  /* Call deserialize on each module */
  engine_deserialize(f);
  stamp_deserialize(f);
  uf_deserialize(f);
  setif_deserialize(f);
  setst_deserialize(f);
  flowrow_deserialize(f);
  term_deserialize(f);

  /* Start the deserialization manager */
  banshee_deserialize_all(f);

  /* Call set_obj on each of the tables */
  for (i = 0; i < sz; i++) {
    deserialize_set_obj((void **)&entry_points[i]);
  }
  
  /* Call set_fields on each module */
  engine_set_fields();
  stamp_set_fields();
  uf_set_fields();
  setif_set_fields();
  setst_set_fields();
  flowrow_set_fields();
  term_set_fields();

  /* Finally, close the file */
  fclose(f);
  
  banshee_deserialize_end();

  return entry_points;
}

int update_constructor(translation t, void *m)
{
  constructor c = (constructor)m;

  update_pointer(t, (void **)&c->name);
  update_pointer(t, (void **)&c->sig);
  update_pointer(t, (void **)&c->groups);

  return sizeof(struct constructor_);
}

int update_cons_group(translation t, void *m)
{
  cons_group g = (cons_group)m;
  update_pointer(t, (void **)&g->name);
  update_pointer(t, (void **)&g->sig);

  return sizeof(struct cons_group_);
}

int update_proj_pat(translation t, void *m)
{
  proj_pat p = (proj_pat)m;
  
  update_pointer(t, (void **)&p->exp);
  update_pointer(t, (void **)&p->c);

  return sizeof(struct proj_pat_);
}

int update_gproj_pat(translation t, void *m)
{
  gproj_pat g = (gproj_pat)m;
  
  update_pointer(t, (void **)&g->exp);
  update_pointer(t, (void **)&g->g);

  return sizeof(struct gproj_pat_);
}

int update_cons_expr(translation t, void *m)
{
  cons_expr e = (cons_expr)m;

  update_pointer(t, (void **)&e->name);
  update_pointer(t, (void **)&e->sig);
  update_pointer(t, (void **)&e->exps);
  update_pointer(t, (void **)&e->c);

  return sizeof(struct cons_expr_);
}
