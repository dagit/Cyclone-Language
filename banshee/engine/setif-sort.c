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
#include <setjmp.h>
#include "regions.h"
#include "bounds.h"
#include "jcollection.h"
#include "setif-sort.h"
#include "utils.h"
#include "banshee_persist_kinds.h"
#include "banshee_region_persist_kinds.h"

struct setif_union_ /* extends gen_e */
{
#ifdef NONSPEC
  sort_kind sort;
#endif
  int type;
  stamp st;
  gen_e_list exprs;
  gen_e_list proj_cache;
};

struct setif_inter_ /* extends gen_e */
{
#ifdef NONSPEC
  sort_kind sort;
#endif
  int type;
  stamp st;
  gen_e_list exprs;
};

struct setif_constant_ /* extends gen_e */
{
#ifdef NONSPEC
  sort_kind sort;
#endif
  int type;
  stamp st;
  char *name;
};

typedef struct added_ub_proj_info_ {
  gen_e_list ub_projs;
  gen_e_list el;
} *added_ub_proj_info;

typedef struct setif_rollback_info_ { /* extends banshee_rollback_info */
  int time;
  sort_kind kind;
  hash_table added_edges; 	/* from bounds stamps to added_edge_infos */
  hash_table added_ub_projs;    
} *setif_rollback_info;

typedef struct setif_inter_ *setif_inter_;
typedef struct setif_union_ *setif_union_;
typedef struct setif_constant_ *setif_constant_;

bool flag_eliminate_cycles = TRUE;
bool flag_merge_projections = TRUE;
static bool tlb_cache_valid = FALSE;
static region tlb_cache_region;
static setif_var_list tlb_var_cache;
static jcoll_dict tlb_dict;
region setif_var_region;
region sv_info_region;
region setif_union_region;
region setif_inter_region;
region setif_constant_region;
region added_ub_proj_info_region;
region setif_rollback_info_region;
region setif_term_region; 	/* unused */

setif_rollback_info setif_current_rollback_info = NULL;

term_hash setif_hash;
struct setif_stats setif_stats;

stamp setif_get_stamp(gen_e e) 
{
#ifdef NONSPEC
  assert(e->sort == setif_sort);
#endif
  
  if ( ((setif_term)e)->type == VAR_TYPE)
    return sv_get_stamp( (setif_var)e );
  
  else
    return ((setif_term)e)->st;
}

static void tlv_lower_aux(jmp_buf buf,stamp st, gen_e e)
{
  if ( setif_is_var(e) && (setif_get_stamp(e) > st) ) 
    longjmp(buf,1);
  
  else if (setif_is_union(e))
    {
      gen_e temp;
      gen_e_list exprs = ((setif_union_)e)->exprs;
      gen_e_list_scanner scan;
      
      gen_e_list_scan(exprs,&scan);
      while (gen_e_list_next(&scan,&temp))
	tlv_lower_aux(buf,st,temp);
    }
}

static bool tlv_lower(stamp st, gen_e e)
{
  jmp_buf buf;
  int higher;
  
  higher = setjmp(buf);
  if (higher)
    return FALSE;

  tlv_lower_aux(buf,st,e);
  
  return TRUE;
}

static void lazy_invalidate_tlb_cache(void) deletes
{
  tlb_cache_valid = FALSE;
}

static void invalidate_tlb_cache(void) deletes
{
  assert(tlb_cache_region);

  setif_var_list_app(tlb_var_cache,sv_clear_tlb_cache);
  jcoll_delete_dict(tlb_dict);
  deleteregion_ptr(&tlb_cache_region);

  tlb_cache_region = newregion();
  tlb_dict = jcoll_create_dict(tlb_cache_region,setif_get_stamp);
  tlb_var_cache = new_setif_var_list(tlb_cache_region);
}

static void set_tlb_cache(setif_var v, jcoll j)
{
  setif_var_list_cons(v,tlb_var_cache);
  sv_set_tlb_cache(v,j);
}

/* 
   A constraint e1 <= e2 is l-inductive iff e2 is a variable x and
   for each y in tlv(e1), stamp(y) < stamp(x) 
*/ 
static bool l_inductive(gen_e e1, gen_e e2)
{
  if (setif_is_var(e2) && tlv_lower(setif_get_stamp(e2), e1))
    return TRUE;
  
  else return FALSE;
}

/*  
    A constraint e1 <= e2 is r-inductive iff e1 is a variable x and
    for each y in tlv(e2), stamp(y) < stamp(x) 
*/ 
static bool r_inductive(gen_e e1, gen_e e2)
{
  if (setif_is_var(e1) && tlv_lower(setif_get_stamp(e1), e2))
    return TRUE;
  
  else return FALSE;
}

static bool eq(gen_e e1, gen_e e2)
{
  return ( setif_get_stamp(e1) == setif_get_stamp(e2) );
}

bool setif_eq(gen_e e1, gen_e e2)
{
  return eq(e1,e2);
}

gen_e_list setif_get_union(gen_e e)
{
  assert ( ((setif_term)e)->type == UNION_TYPE);

  return ( (setif_union_) e)->exprs;
}

gen_e_list setif_get_inter(gen_e e)
{
  assert ( ((setif_term)e)->type == INTER_TYPE);

  return ( (setif_inter_) e)->exprs;
}

static setif_var_list search_ubs(region r, setif_var v1, setif_var goal)
{
  bool found;
  setif_var_list cycle;
  
  void search_ubs_aux(setif_var v)
    {
      assert(! found);

      if (sv_eq(v,goal))
	{
	  found = TRUE;
	  return;
	}
      else if (sv_lt(v,goal))
	{
	  return;
	}
      else 
	{
	  bounds_scanner scan;
	  gen_e ub;
	  bounds ubs = sv_get_ubs(v);
	  
	  bounds_scan(ubs,&scan);
	  while (bounds_next(&scan,&ub))
	  {
	    if (setif_is_var(ub))
	      {
		search_ubs_aux((setif_var)ub);
		if (found)
		  {
		    setif_var_list_cons(v,cycle);
		    return;
		  }
	      }
	  }
	}
    }

  found = FALSE;
  cycle = new_setif_var_list(r);
  search_ubs_aux(v1);

  return cycle;
}

static setif_var_list search_lbs(region r, setif_var v1, setif_var goal)
{
  bool found;
  setif_var_list cycle;
 
  void search_lbs_aux(setif_var v)
    {
      assert (! found);
      if (sv_eq(v,goal))
	{
	  found = TRUE;
	  return;
	}
      else if (sv_lt(v,goal))
	{
	  return;
	}
      else
	{
	  bounds_scanner scan;
	  gen_e lb;
	  bounds lbs = sv_get_lbs(v);
	  
	  bounds_scan(lbs,&scan);
	  while (bounds_next(&scan,&lb))
	  {
	    if (setif_is_var(lb))
	      {
		search_lbs_aux((setif_var)lb);
		if (found)
		  {
		    setif_var_list_cons(v,cycle);
		    return;
		  }
	      }
	  }
	}
	
    }

  found = FALSE;
  cycle = new_setif_var_list(r);
  search_lbs_aux(v1);

  return cycle; 
}

static setif_var_list cycle_detect(region r,setif_var v1,setif_var v2)
{
  if (sv_union_component(v1,v2))
    return new_setif_var_list(r);

  else
    {
      setif_stats.cycles_searched_forward++;
      return search_ubs(r, v2, v1);
    }
}


static setif_var_list cycle_detect_rev(region r, setif_var v1, setif_var v2)
{
  if (sv_union_component(v1,v2))
    return new_setif_var_list(r);

  else
    {
      setif_stats.cycles_searched_backward++;
      return search_lbs(r, v1, v2);
    }
}

void setif_register_rollback(void) 
{
#ifdef BANSHEE_ROLLBACK
  setif_current_rollback_info = ralloc(setif_rollback_info_region,
				 struct setif_rollback_info_);
  banshee_set_time((banshee_rollback_info)setif_current_rollback_info);
  setif_current_rollback_info->kind = setif_sort;
  setif_current_rollback_info->added_edges = 
    make_persistent_hash_table(4, stamp_hash, stamp_eq,
			       BANSHEE_PERSIST_KIND_nonptr,
			       BANSHEE_PERSIST_KIND_added_edge_info);

  setif_current_rollback_info->added_ub_projs = 
    make_persistent_hash_table(4, stamp_hash, stamp_eq,
			       BANSHEE_PERSIST_KIND_nonptr,
			       BANSHEE_PERSIST_KIND_added_ub_proj_info);
  
  banshee_register_rollback((banshee_rollback_info)setif_current_rollback_info);
#endif /* BANSHEE_ROLLBACK */
}

void setif_register_edge(const bounds b, stamp st) {
#ifdef BANSHEE_ROLLBACK
  added_edge_info info = NULL;
  assert(setif_current_rollback_info);
  
  /* The current rollback info already has an edge list associated
   * with this bounds */
  if (hash_table_lookup(setif_current_rollback_info->added_edges,
			(hash_key)bounds_stamp(b),
			(hash_data *)&info)) {
    assert(info);
    stamp_list_cons(st,info->sl);
  }
  else {
    stamp_list sl = new_persistent_stamp_list();
    stamp_list_cons(st,sl);
    info = ralloc(added_edge_info_region, struct added_edge_info_);
    info->b = b;
    info->sl = sl;
    hash_table_insert(setif_current_rollback_info->added_edges,
		      (hash_key)bounds_stamp(b),
		      (hash_data)info);
  }
#endif /* BANSHEE_ROLLBACK */
}

void setif_register_ub_proj(gen_e_list ub_projs, gen_e e) {
#ifdef BANSHEE_ROLLBACK
  added_ub_proj_info info = NULL;
  assert(setif_current_rollback_info);
  
  if (hash_table_lookup(setif_current_rollback_info->added_ub_projs,
			(hash_key)gen_e_list_stamp(ub_projs),
			(hash_data *)&info)) {
    assert(info);
    gen_e_list_cons(e,info->el);
  }
  else {
    gen_e_list el = new_persistent_gen_e_list();
    gen_e_list_cons(e,el);
    info = ralloc(added_ub_proj_info_region, struct added_ub_proj_info_);
    info->ub_projs = ub_projs;
    info->el = el;
    hash_table_insert(setif_current_rollback_info->added_ub_projs,
		      (hash_key)gen_e_list_stamp(ub_projs),
		      (hash_data)info);
  }
#endif /* BANSHEE_ROLLBACK */
}

void setif_inclusion(con_match_fn_ptr con_match, res_proj_fn_ptr res_proj, 
		     gen_e_pr_fn_ptr pr, gen_e e1, gen_e e2) deletes
{
  
  void collapse_cycle_lower(region r, setif_var witness, 
			    setif_var_list cycle) deletes
    {
      gen_e lb;
      bounds_scanner scan_bounds;
      setif_var_list_scanner scan_cycle;
      setif_var var;

#ifndef NDEBUG
      stamp lowest = sv_get_stamp(witness);
#endif
      bounds b = bounds_create(r);
      
      // Collect all lower bounds in the cycle, and add transitive edges
      setif_var_list_scan(cycle,&scan_cycle);
      while(setif_var_list_next(&scan_cycle,&var))
	{
	  assert( sv_get_stamp(var) > lowest);
	  bounds_scan(sv_get_lbs(var),&scan_bounds);
	  while(bounds_next(&scan_bounds,&lb))
	    bounds_add(b,lb,setif_get_stamp(lb));
	}

      sv_unify(witness,cycle);
      assert(sv_get_stamp(witness) == lowest);
      
      bounds_scan(b,&scan_bounds);
      while (bounds_next(&scan_bounds,&lb))
	setif_inclusion(con_match,res_proj,pr,lb, (gen_e) witness);
      
      bounds_delete(b);
      lazy_invalidate_tlb_cache();

      setif_stats.cycles_collapsed_backward++;
      setif_stats.cycles_length_backward += setif_var_list_length(cycle);
    }
  
  void collapse_cycle_upper(region r, setif_var witness,
			    setif_var_list cycle) deletes
    {
      gen_e ub;
      bounds_scanner scan_bounds;
      gen_e_list_scanner scan_projs;
      setif_var_list_scanner scan_cycle;
      setif_var var;

#ifndef NDEBUG
      stamp lowest = sv_get_stamp(witness);
#endif
      bounds b = bounds_create(r);
     
      // Collect all upper bounds in the cycle, and add transitive edges
      setif_var_list_scan(cycle,&scan_cycle);
      while(setif_var_list_next(&scan_cycle,&var))
	{ 
	  assert( sv_get_stamp(var) > lowest);

	  bounds_scan(sv_get_ubs(var),&scan_bounds);
	  while(bounds_next(&scan_bounds,&ub))
	    bounds_add(b,ub,setif_get_stamp(ub));
	  
	  gen_e_list_scan(sv_get_ub_projs(var),&scan_projs);
	  while(gen_e_list_next(&scan_projs,&ub))
	    bounds_add(b,ub,setif_get_stamp(ub));
	}

      sv_unify(witness,cycle);
      assert(sv_get_stamp(witness) == lowest);

      bounds_scan(b,&scan_bounds);
      while (bounds_next(&scan_bounds,&ub))
	setif_inclusion(con_match,res_proj,pr,(gen_e) witness, ub);
	
      bounds_delete(b);
      lazy_invalidate_tlb_cache();

      setif_stats.cycles_collapsed_forward++;
      setif_stats.cycles_length_backward += setif_var_list_length(cycle);
    }
  
  void update_lower_bound(setif_var v, gen_e e) deletes
    {
      if (sv_add_lb(v,e,setif_get_stamp(e)))
	{
	  if (setif_is_var(e))
	    setif_stats.redundant_succ++;
	  
	  else
	    setif_stats.redundant_source++;
	}

      else
	{
	  gen_e_list_scanner scan;
	  gen_e ub;
	  bounds_scanner scan_bounds;
	  
	  if (setif_is_var(e))
	    setif_stats.added_succ++;
	  else
	    setif_stats.added_source++;
	  
	  setif_register_edge(sv_get_lbs(v),setif_get_stamp(e));

	  lazy_invalidate_tlb_cache();

	  bounds_scan(sv_get_ubs(v),&scan_bounds);
	  while(bounds_next(&scan_bounds,&ub))
	    setif_inclusion(con_match,res_proj,pr,e,ub);
	  
	  gen_e_list_scan(sv_get_ub_projs(v),&scan);
	  while (gen_e_list_next(&scan,&ub))
	    setif_inclusion(con_match,res_proj,pr,e,ub);

	}
      
    }

  void update_upper_bound(setif_var v, gen_e e) deletes
    {
      if (sv_add_ub(v,e,setif_get_stamp(e)))
	{
	  if (setif_is_var(e))
	    setif_stats.redundant_pred++;
	
	  else
	    setif_stats.redundant_sink++;
	}
      
      else
	{
	  bounds_scanner scan;
	  gen_e lb;

	  if (setif_is_var(e))
	    setif_stats.added_pred++;
	  else
	    setif_stats.added_sink++;

	  setif_register_edge(sv_get_ubs(v),setif_get_stamp(e));

	  lazy_invalidate_tlb_cache();
	  
	  bounds_scan(sv_get_lbs(v),&scan);
	  while (bounds_next(&scan,&lb))
	    setif_inclusion(con_match,res_proj,pr,lb,e);

	}
      
    }

/*    pr(stdout,e1); */
/*    fprintf(stdout,"<="); */
/*    pr(stdout,e2); */
/*    fprintf(stdout,"\n"); */
  
  if (!banshee_check_rollback(setif_sort)) {
    setif_register_rollback();
  }

  if ( setif_is_wild(e1) || setif_is_wild(e2)) {
    return;
  }

  if ( setif_is_zero(e1) || setif_is_one(e2) )
    return;
  
  else if (eq(e1,e2))
    return;
  
  /* c <= d */
  else if ( setif_is_constant(e1) && setif_is_constant(e2) )
    {

      handle_error(e1,e2,bek_cons_mismatch);

      return;
    }

  else if ( setif_is_union(e1) )
    {
      gen_e_list_scanner scan;
      gen_e temp;

      gen_e_list exprs = setif_get_union(e1);
      
      gen_e_list_scan(exprs,&scan);
      while (gen_e_list_next(&scan,&temp))
	{
	  setif_inclusion(con_match,res_proj,pr,temp,e2);
	}

      return;
    }
  
  else if ( setif_is_inter(e2) )
    {
      gen_e_list_scanner scan;
      gen_e temp;

      gen_e_list exprs = setif_get_inter(e2);

      gen_e_list_scan(exprs,&scan);
      while (gen_e_list_next(&scan,&temp))
	{
	  setif_inclusion(con_match,res_proj,pr,e1,temp);
	}

      return;
    }

  else if ( l_inductive(e1,e2) ) /* _ <= 'x */ 
    {
      setif_var v2 = ((setif_var)e2);

      if (setif_is_var(e1))
	{
	  setif_var v1 = ((setif_var)e1);
	  
	  if (flag_eliminate_cycles)
	    {
	      region scratch = newregion();
	      setif_var_list cycle = cycle_detect(scratch,v1,v2);
	      
	      if (! setif_var_list_empty(cycle))
		collapse_cycle_upper(scratch,v1,cycle);
	      else
		update_lower_bound(v2,e1);
	      
	      deleteregion(scratch);
	    }
	  
	  else 
	    update_lower_bound(v2,e1);
	}
      else // e1 is a source
	update_lower_bound(v2,e1);
    }

  else if ( r_inductive(e1,e2) ) /* 'x <= _ */
    {
      setif_var v1 = ((setif_var)e1);
     
      if (setif_is_var(e2))
	{
	  setif_var v2 = ((setif_var)e2);
	  
	  if (flag_eliminate_cycles)
	    {
	      region scratch = newregion();
	      setif_var_list cycle = cycle_detect_rev(scratch,v1,v2);
	      
	      if (! setif_var_list_empty(cycle))
		collapse_cycle_lower(scratch,v2,cycle);
	      else
		update_upper_bound(v1,e2);
	      
	      deleteregion(scratch);
	    }
      
	  else
	    update_upper_bound(v1,e2);
	}
      else // e2 is a sink
	{
	  if (flag_merge_projections && res_proj(v1,e2))
	    return;
	  else
	    update_upper_bound(v1,e2);
	}
    }

  else /* c(...) <= c(...) or c(...) <= projpat(c,i,e) */
    return con_match(e1,e2);
  
}

/* #ifdef NONSPEC */
/* static struct setif_term zero = {setif_sort,ZERO_TYPE,ZERO_TYPE}; */
/* static struct setif_term one  = {setif_sort,ONE_TYPE,ONE_TYPE}; */
/* #else */
/* static struct setif_term zero = {ZERO_TYPE,ZERO_TYPE}; */
/* static struct setif_term one  = {ONE_TYPE,ONE_TYPE}; */
/* #endif /\* NONSPEC *\/ */

setif_term zero;
setif_term one;
setif_term wild;

gen_e setif_zero(void)
{
  return (gen_e)zero;
}

gen_e setif_one(void)
{
  return (gen_e)one;
}

gen_e setif_wild(void)
{
  return (gen_e)wild;
}

gen_e setif_fresh(const char *name)
{
  setif_var result = sv_fresh(NULL,name);

  setif_stats.fresh++;
  return (gen_e)result;
}

gen_e setif_fresh_large(const char *name)
{
  setif_var result = sv_fresh_large(NULL,name);

  setif_stats.fresh_large++;
  return (gen_e)result;
}

gen_e setif_fresh_small(const char *name)
{
  setif_var result = sv_fresh_small(NULL,name);

  setif_stats.fresh_small++;
  return (gen_e)result;
}

gen_e setif_constant(const char *str) deletes
{
  stamp st[2];
  gen_e result;
  char *name = rstrdup(banshee_nonptr_region,str);

  assert (str != NULL);
  
  st[0] = CONSTANT_TYPE;
  st[1] = stamp_string(name); 

  if ( (result = term_hash_find(setif_hash,st,2)) == NULL)
    {
      setif_constant_ c = ralloc(setif_constant_region, struct setif_constant_);
#ifdef NONSPEC
      c->sort = setif_sort;
#endif
      c->type = CONSTANT_TYPE;
      c->st = stamp_fresh();
      c->name = name;

      result = (gen_e) c;
      term_hash_insert(setif_hash,result,st,2);
      
      setif_stats.distinct_constants++;
      
      return result;
    }
  
  else
    {
      setif_stats.hashed_constants++;
      return result;
    }
}

static bool filter_zero(const gen_e e)
{
  return (!setif_is_zero(e));
}


static bool filter_one(const gen_e e)
{
  return (!setif_is_one(e));
}

gen_e setif_union(gen_e_list exprs) deletes
{
  gen_e_list filtered = gen_e_list_filter(NULL,exprs,filter_zero);
  
  if ( gen_e_list_empty(filtered) )
    {
      setif_stats.filtered_unions++;
      return setif_zero();
    }
  else if (gen_e_list_length(filtered) == 1)
    {
      setif_stats.filtered_unions++;
      return gen_e_list_head(filtered);
    }

  else 
    {
      int i = 0;
      gen_e temp,result;
      gen_e_list_scanner scan;
      stamp st[ gen_e_list_length(filtered) + 1 ];
     
      st[0] = UNION_TYPE;

      gen_e_list_scan(filtered,&scan);
      while (gen_e_list_next(&scan,&temp))
	{
	  st[++i] = setif_get_stamp(temp);
	}
      
      if ( (result = 
	   term_hash_find(setif_hash,st,gen_e_list_length(filtered)+1)) 
	   == NULL )
	{
	  struct setif_union_ *u = 
	    ralloc(setif_union_region,struct setif_union_);
	  
	  u->type = UNION_TYPE;
	  u->st = stamp_fresh();
	  u->proj_cache = new_persistent_gen_e_list();
	  u->exprs = filtered;
#ifdef NONSPEC
	  u->sort = setif_sort;
#endif
	 
	  result = (gen_e)u;
	  term_hash_insert(setif_hash,result,st,gen_e_list_length(filtered)+1);
	
	  setif_stats.distinct_unions++;
	  return result;
	}
      else
	{
	  setif_stats.hashed_unions++;
	  return result;
	}
    }
}

gen_e setif_inter(gen_e_list exprs) deletes
{
  gen_e_list filtered = gen_e_list_filter(NULL,exprs,filter_one);
  
  if ( gen_e_list_empty(filtered) )
    {
      setif_stats.filtered_intersections++;
      return setif_one();
    }
  else if (gen_e_list_length(filtered) == 1)
    {
      setif_stats.filtered_intersections++;
      return gen_e_list_head(filtered);
    }

  else 
    {
      int i = 0;
      gen_e temp,result;
      gen_e_list_scanner scan;
      stamp st[ gen_e_list_length(filtered) + 1 ];
     
      st[0] = INTER_TYPE;

      gen_e_list_scan(filtered,&scan);
      while (gen_e_list_next(&scan,&temp))
	{
	  st[++i] = setif_get_stamp(temp);
	}
      
      if ( (result = 
	   term_hash_find(setif_hash,st,gen_e_list_length(filtered)+1)) 
	   == NULL )
	{
	  struct setif_inter_ *u = 
	    ralloc(setif_inter_region,struct setif_inter_);
	  
	  u->type = UNION_TYPE;
	  u->st = stamp_fresh();
	  u->exprs = filtered;
#ifdef NONSPEC
	  u->sort = setif_sort;
#endif	 

	  result = (gen_e)u;
	  term_hash_insert(setif_hash,result,st,gen_e_list_length(filtered)+1);
	  
	  setif_stats.distinct_intersections++;
	  
	  return result;
	}
      else 
	{
	  setif_stats.hashed_intersections++;
	  return result;
	}
    }
}

bool setif_is_zero(gen_e e)
{
  return ((setif_term)e)->type == ZERO_TYPE;
}

bool setif_is_one(gen_e e)
{  
  return ((setif_term)e)->type == ONE_TYPE;
}

bool setif_is_wild(gen_e e)
{
  return ((setif_term)e)->type == WILD_TYPE;
}

bool setif_is_var(gen_e e)
{
  return ((setif_term)e)->type == VAR_TYPE;
}

bool setif_is_union(gen_e e)
{
  return ((setif_term)e)->type == UNION_TYPE;
}

bool setif_is_inter(gen_e e)
{
  return ((setif_term)e)->type == INTER_TYPE;
}

bool setif_is_constant(gen_e e)
{
  return ((setif_term)e)->type == CONSTANT_TYPE;
}

char *setif_get_constant_name(gen_e e)
{
  assert( ((setif_term)e)->type == CONSTANT_TYPE );
  
  return ((setif_constant_)e)->name;
}

void setif_init(void)
{
  setif_term_region = newregion();
  setif_var_region = newregion();
  sv_info_region = newregion();
  setif_union_region = newregion();
  setif_constant_region = newregion();
  setif_inter_region = newregion();
  added_ub_proj_info_region = newregion();
  setif_rollback_info_region = newregion();
  tlb_cache_region = newregion(); 
  tlb_var_cache = new_setif_var_list(tlb_cache_region);
  setif_hash = make_term_hash(NULL);
  tlb_dict = jcoll_create_dict(tlb_cache_region,setif_get_stamp);

  /* TODO -- these allocated constant terms will be leaked */
  zero = ralloc(banshee_nonptr_region, struct setif_term);
#ifdef NONSPEC
  zero->sort = setif_sort;
#endif
  zero->st = ZERO_TYPE;
  zero->type = ZERO_TYPE;

  one = ralloc(banshee_nonptr_region, struct setif_term);
#ifdef NONSPEC
  one->sort = setif_sort;
#endif
  one->st = ONE_TYPE;
  one->type = ONE_TYPE;

  wild = ralloc(banshee_nonptr_region, struct setif_term);
#ifdef NONSPEC
  wild->sort = setif_sort;
#endif
  wild->st = WILD_TYPE;
  wild->type = WILD_TYPE;  
}



static void setif_reset_stats(void)
{
  setif_stats.fresh = 0;
  setif_stats.fresh_small = 0;
  setif_stats.fresh_large = 0;

  setif_stats.distinct_constructors = 0;
  setif_stats.hashed_constructors = 0;
  setif_stats.distinct_constants = 0;
  setif_stats.hashed_constants = 0;
  setif_stats.distinct_unions = 0;
  setif_stats.filtered_unions = 0;
  setif_stats.hashed_unions = 0;
  setif_stats.distinct_intersections = 0;
  setif_stats.filtered_intersections = 0;
  setif_stats.hashed_intersections = 0;

  setif_stats.redundant_pred = 0;
  setif_stats.redundant_succ = 0;
  setif_stats.redundant_source = 0;
  setif_stats.redundant_sink = 0;
  
  setif_stats.added_pred = 0;
  setif_stats.added_succ = 0;
  setif_stats.added_source = 0;
  setif_stats.added_sink = 0;
  
  setif_stats.cycles_searched_forward = 0;
  setif_stats.cycles_searched_backward = 0;
  
  setif_stats.cycles_collapsed_forward = 0;
  setif_stats.cycles_collapsed_backward = 0;
  
  setif_stats.cycles_length_forward = 0;
  setif_stats.cycles_length_backward = 0;
}

void setif_reset(void) deletes
{
  term_hash_delete(setif_hash);
  invalidate_tlb_cache();
  deleteregion(tlb_cache_region);
  deleteregion(sv_info_region);
  deleteregion(setif_var_region);
  deleteregion(setif_inter_region);
  deleteregion(setif_union_region);
  deleteregion(setif_constant_region);
  deleteregion(added_ub_proj_info_region);
  deleteregion(setif_rollback_info_region);
  deleteregion(setif_term_region);

  setif_reset_stats();
  
  setif_init();
}

static jcoll tlb_aux(gen_e e)
{
  if (setif_is_var(e))
    {
      setif_var v = (setif_var)e;

      if ( sv_get_tlb_cache(v) != NULL)
	return sv_get_tlb_cache(v);
      
      else
	{
	  jcoll result;
	  gen_e_list sources = new_gen_e_list(tlb_cache_region);
	  jcoll_list jvars = new_jcoll_list(tlb_cache_region);
	  bounds_scanner scan;
	  gen_e lb;

	  bounds_scan(sv_get_lbs(v),&scan);
	  while (bounds_next(&scan,&lb))
	    {
	      if (setif_is_var(lb))
		jcoll_list_cons(tlb_aux(lb),jvars);
	      else
		gen_e_list_cons(lb,sources);
		/* jsources = jcoll_jcons(tlb_cache_region,lb,jsources); */
	    }

	  if (! gen_e_list_empty(sources))
	   jcoll_list_cons(jcoll_create_chain(tlb_dict,sources),jvars);
	  
	  result =
	    jcoll_jjoin(tlb_dict,jvars);
	  
	  set_tlb_cache(v,result);
	  return result;	
	}
    }
  else if (setif_is_union(e))
    {
      gen_e_list_scanner scan;
      gen_e temp;
      jcoll_list jexprs = new_jcoll_list(tlb_cache_region);
      
      gen_e_list_scan(setif_get_union(e),&scan);
      while (gen_e_list_next(&scan,&temp))
	{
	  jcoll_list_cons(tlb_aux(temp),jexprs);
	}

      return jcoll_jjoin(tlb_dict,jexprs);
      
    }
  else
    {
      //fail("Unmatched case in setif tlb computation\n");
      return NULL;
    }
}

gen_e_list setif_tlb(gen_e e) deletes
{
  if (!tlb_cache_valid)
    invalidate_tlb_cache();
  tlb_cache_valid = TRUE;
  return jcoll_flatten(tlb_dict,tlb_aux(e));
}

void setif_set_proj_cache(gen_e e,gen_e elem)
{
  if (setif_is_union(e))
    {
      setif_union_ u = (setif_union_)e;
      gen_e_list_cons(elem,u->proj_cache);
    }
}

gen_e_list setif_get_proj_cache(gen_e e)
{
  if (setif_is_union(e))
    {
      setif_union_ u = (setif_union_)e;
      return u->proj_cache;
    }
  else
    {
      fail("Term does not cache projections\n");
      return NULL;
    }
}


bool setif_proj_merge(setif_var v, gen_e se, get_proj_fn_ptr get_proj,
		      proj_con_fn_ptr proj_con,fresh_large_fn_ptr fresh_large,
		      incl_fn_ptr sort_incl, incl_fn_ptr set_incl) deletes
{
  gen_e proj;
  
  if ((proj = sv_get_ub_proj(v,get_proj)) != NULL)
    {
      sort_incl(proj, se);
      return TRUE;
    }
  
  else
    {
      bounds_scanner scan;
      gen_e lb;
      
      gen_e proj_var;
      gen_e proj_cons;
      
      /* create a projection variable for this projection */ 
      proj_var = fresh_large(NULL);
      
      assert(setif_is_var(proj_var));

      proj_cons = proj_con(proj_var);

      sv_add_ub_proj(v, proj_cons);
      setif_register_ub_proj(sv_get_ub_projs(v),proj_cons);

      /* apply the transitive rule to each of v's lower bounds */ 
      bounds_scan(sv_get_lbs(v),&scan);
      while (bounds_next(&scan,&lb))
	{
	  set_incl(lb,proj_cons);
	}
	
      sort_incl(proj_var, se);
      return TRUE;
    }

}


void setif_print_stats(FILE *f)
{
  fprintf(f,"\n========== SetIF Var Stats ==========\n");
  fprintf(f,"(SetIF) Fresh : %d\n",setif_stats.fresh); 
  fprintf(f,"(SetIF) Fresh Small : %d\n",setif_stats.fresh_small);
  fprintf(f,"(SetIF) Fresh Large : %d\n",setif_stats.fresh_large);
  fprintf(f,"(SetIF) Total Vars: %d\n",setif_stats.fresh + setif_stats.fresh_small 
	  + setif_stats.fresh_large);
  fprintf(f,"\n========== SetIF Sort Stats ==========\n");
  fprintf(f,"\n");
  fprintf(f,"\n------------------------------\n");
  fprintf(f,"(SetIF) Additions");
  fprintf(f,"\n------------------------------\n");
  fprintf(f,"(SetIF) Pred Added:   %d\n",setif_stats.added_pred);
  fprintf(f,"(SetIF) Succ Added:   %d\n",setif_stats.added_succ);
  fprintf(f,"(SetIF) Source Added: %d\n",setif_stats.added_source);
  fprintf(f,"(SetIF) Sink Added:   %d",setif_stats.added_sink);
  fprintf(f,"\n------------------------------\n");
  fprintf(f,"(SetIF) Total Added:  %d",setif_stats.added_pred + setif_stats.added_succ
	  + setif_stats.added_source + setif_stats.added_sink);
  fprintf(f,"\n");
  fprintf(f,"\n------------------------------\n");
  fprintf(f,"(SetIF) Redundant");
  fprintf(f,"\n------------------------------\n");
  fprintf(f,"(SetIF) Pred Redundant:   %d\n",setif_stats.redundant_pred);
  fprintf(f,"(SetIF) Succ Redundant:   %d\n",setif_stats.redundant_succ);
  fprintf(f,"(SetIF) Source Redundant: %d\n",setif_stats.redundant_source);
  fprintf(f,"(SetIF) Sink Redundant:   %d",setif_stats.redundant_sink);
  fprintf(f,"\n------------------------------\n");
  fprintf(f,"(SetIF) Total Redundant:  %d\n",
	 setif_stats.redundant_pred+setif_stats.redundant_succ+setif_stats.redundant_source+setif_stats.redundant_sink);

  fprintf(f,"\n");
  
  fprintf(f,"\n------------------------------\n");
  fprintf(f,"(SetIF) Forward Cycles");
  fprintf(f,"\n------------------------------\n");
  fprintf(f,"(SetIF) Collapsed:      %d\n",setif_stats.cycles_collapsed_forward);
  fprintf(f,"(SetIF) Searched:       %d\n",setif_stats.cycles_searched_forward);
  fprintf(f,"(SetIF) Hit rate:       %f\n",
	 ((float)setif_stats.cycles_collapsed_forward)/((float)setif_stats.cycles_searched_forward));
  fprintf(f,"(SetIF) Average Length: %f\n",
	 1+((float)setif_stats.cycles_length_forward) / ((float)setif_stats.cycles_collapsed_forward));
  fprintf(f,"\n");
  fprintf(f,"\n------------------------------\n");
  fprintf(f,"(SetIF) Reverse Cycles");
  fprintf(f,"\n------------------------------\n");
  fprintf(f,"(SetIF) Collapsed:      %d\n",setif_stats.cycles_collapsed_backward);
  fprintf(f,"(SetIF) Searched:       %d\n",setif_stats.cycles_searched_backward);
  fprintf(f,"(SetIF) Hit rate:       %f\n",
	 ((float)setif_stats.cycles_collapsed_backward)/((float)setif_stats.cycles_searched_backward));
  fprintf(f,"(SetIF) Average Length: %f\n",
	 1+((float)setif_stats.cycles_length_backward) / ((float)setif_stats.cycles_collapsed_backward));
  fprintf(f,"=====================================\n");
}

/*
  for now, print stamps and types for sources and sinks.
  must eventually rely on specialized code
*/
void setif_print_constraint_graph(FILE *f)
{ 
  /*
  setif_var_list_scanner scan;
  gen_e_list_scanner scan_edges;
  gen_e edge;
  setif_var v;
  dot_node n1,n2;
  char temp_str[512];
    
  graph_attr graph_style[3] = {{g_size,"\"8.5,11\""},
			       {g_center,"true"},
			       {g_orientation,"portrait"}};
  edge_attr succ_edge[1] = {{e_style,"solid"}};
  edge_attr pred_edge[1] = {{e_style,"dotted"}};

   dot_start(f,"setif",TRUE,TRUE);
  dot_global_graph_style(graph_style,3);

  setif_var_list_scan(setif_vars,&scan);
  while(setif_var_list_next(&scan,&v))
    {
      snprintf(temp_str,512,"%s:%d",sv_get_name(v),sv_get_stamp(v));
      n1 = dot_get_node(temp_str);
      gen_e_list_scan(sv_get_lbs(v),&scan_edges);
      while(gen_e_list_next(&scan_edges,&edge))
	{
	  if (setif_is_var(edge))
	    {
	      snprintf(temp_str,512,"%s:%d",sv_get_name((setif_var)edge),
		       setif_get_stamp(edge));
	      n2 = dot_get_node(temp_str);
	    }
	  else
	    {
	      snprintf(temp_str,512,"source:%d",setif_get_stamp(edge));
	      n2 = dot_get_node(temp_str);
	    }
	  dot_styled_edge(n2,n1,pred_edge,1);
	}

      gen_e_list_scan(sv_get_ubs(v),&scan_edges);
      while(gen_e_list_next(&scan_edges,&edge))
	{
	  if (setif_is_var(edge))
	    {
	      snprintf(temp_str,512,"%s:%d",sv_get_name((setif_var)edge),
		       setif_get_stamp(edge));
	      n2 = dot_get_node(temp_str);
	    }
	  else
	    {
	      snprintf(temp_str,512,"sink:%d",setif_get_stamp(edge));
	      n2 = dot_get_node(temp_str);
	    }
	  dot_styled_edge(n1,n2,succ_edge,1);
	}

      gen_e_list_scan(sv_get_ub_projs(v),&scan_edges);
      while(gen_e_list_next(&scan_edges,&edge))
	{
	  snprintf(temp_str,512,"projpat:%d",setif_get_stamp(edge));
	  n2 = dot_get_node(temp_str);
	  dot_styled_edge(n1,n2,succ_edge,1);
	}
      
    }
  
  dot_end();
  */
}

static gen_e current_expr;
static bool expr_eq_fn(const gen_e e)
{
  return setif_eq(current_expr, e);
}

void setif_rollback(banshee_rollback_info info)
{
  
  hash_table_scanner hash_scan;
  stamp_list_scanner stamp_scan;
  added_edge_info next_info;
  added_ub_proj_info  next_ubproj_info;
  stamp next_stamp,st;
  gen_e_list_scanner expr_scan;
  gen_e next_expr;
  
  setif_rollback_info tinfo = (setif_rollback_info)info;
  
  assert(tinfo->kind = setif_sort);

  lazy_invalidate_tlb_cache();
  term_hash_delete(setif_hash);
  setif_hash = make_term_hash(NULL);
  
  hash_table_scan(tinfo->added_edges, &hash_scan);
  while(hash_table_next(&hash_scan,(hash_key *)&st,
			(hash_data *) &next_info)) {
    stamp_list_scan(next_info->sl, &stamp_scan);
    while(stamp_list_next(&stamp_scan,&next_stamp)) {
      bool present = bounds_remove(next_info->b,next_stamp);
      if (! present) fail("Tried to remove a nonexistent bound\n");
    }
  }
  
  hash_table_scan(tinfo->added_ub_projs, &hash_scan);
  while (hash_table_next(&hash_scan,(hash_key *)&st,
			 (hash_data *)&next_ubproj_info)) {
    gen_e_list_scan(next_ubproj_info->el, &expr_scan);
    while(gen_e_list_next(&expr_scan,&next_expr)) {
      current_expr = next_expr;
      gen_e_list_drop(next_ubproj_info->ub_projs,expr_eq_fn);
    }
  }

}

/* Persistence */
bool setif_rollback_serialize(FILE *f, banshee_rollback_info i)
{
  setif_rollback_info info = (setif_rollback_info)i;
  assert(f);
  assert(info);

  fwrite(&info->added_edges, 2 * sizeof(hash_table), 1, f);
/*   fwrite((void *)&info->added_ub_projs, sizeof(hash_table), 1, f); */

  serialize_banshee_object(info->added_edges, hash_table);
  serialize_banshee_object(info->added_ub_projs, hash_table);
  
  return TRUE;
}

banshee_rollback_info setif_rollback_deserialize(FILE *f)
{
  setif_rollback_info info = ralloc(setif_rollback_info_region, 
				    struct setif_rollback_info_);
  assert(f);

/*   fread((void *)&info->added_edges, sizeof(hash_table), 1, f); */
/*   fread((void *)&info->added_ub_projs, sizeof(hash_table), 1, f); */
  fread(&info->added_edges, 2 *sizeof(hash_table), 1, f);

  return (banshee_rollback_info) info;
}

bool setif_rollback_set_fields(banshee_rollback_info i)
{
  setif_rollback_info info = (setif_rollback_info)i;

  deserialize_set_obj((void **)&info->added_edges);
  deserialize_set_obj((void **)&info->added_ub_projs);
  
  return TRUE;
}

bool setif_union_serialize(FILE *f, gen_e e)
{
  setif_union_ expr = (setif_union_)e;
  assert(f);
  assert(expr);

/*   fwrite((void *)&expr->st, sizeof(stamp), 1, f); */
/*   fwrite((void *)&expr->exprs, sizeof(gen_e_list), 1, f); */
/*   fwrite((void *)&expr->proj_cache, sizeof(gen_e_list), 1, f); */
  fwrite(&expr->st, sizeof(stamp) + sizeof(gen_e_list) * 2, 1, f);
  serialize_banshee_object(expr->exprs, list);
  serialize_banshee_object(expr->proj_cache, list);
  
  return TRUE;
}

bool setif_inter_serialize(FILE *f, gen_e e)
{
  setif_inter_ expr = (setif_inter_)e;
  assert(f);
  assert(expr);

/*   fwrite((void *)&expr->st, sizeof(stamp), 1, f); */
/*   fwrite((void *)&expr->exprs, sizeof(gen_e_list), 1, f); */
  fwrite(&expr->st, sizeof(stamp) + sizeof(gen_e_list), 1 ,f);

  serialize_banshee_object(expr->exprs, list);

  return TRUE;
}

bool setif_constant_serialize(FILE *f, gen_e e)
{
  setif_constant_ c = (setif_constant_)e;
  assert(f);
  assert(c);

  fwrite(&c->st, sizeof(stamp), 1, f);
  string_data_serialize(f, c->name);

  return TRUE;
}

void *setif_union_deserialize(FILE *f)
{
  setif_union_ expr = ralloc(setif_term_region, struct setif_union_);
  
/*   fread((void *)&expr->st, sizeof(stamp), 1, f); */
/*   fread((void *)&expr->exprs, sizeof(gen_e_list), 1, f); */
/*   fread((void *)&expr->proj_cache, sizeof(gen_e_list), 1, f); */
  fread(&expr->st, sizeof(stamp) + 2 *sizeof(gen_e_list), 1, f);

  return expr;
}

void *setif_inter_deserialize(FILE *f)
{
  setif_inter_ expr = ralloc(setif_term_region, struct setif_inter_);
/*   fread((void *)&expr->st, sizeof(stamp), 1, f); */
/*   fread((void *)&expr->exprs, sizeof(gen_e_list), 1, f); */
  fread(&expr->st, sizeof(stamp) + sizeof(gen_e_list), 1, f);


  return expr;
}

void *setif_constant_deserialize(FILE *f)
{
  setif_constant_ c = ralloc(setif_term_region, struct setif_constant_);

  fread((void *)&c->st, sizeof(stamp), 1, f);
  c->name = string_data_deserialize(f);

  return c;
}

bool setif_union_set_fields(gen_e e)
{
  deserialize_set_obj((void **)&((setif_union_)e)->exprs);
  deserialize_set_obj((void **)&((setif_union_)e)->proj_cache);
  return TRUE;
}

bool setif_inter_set_fields(gen_e e)
{
  deserialize_set_obj((void **)&((setif_inter_)e)->exprs);
  return TRUE;
}

bool setif_constant_set_fields(gen_e e)
{
  return TRUE;
}

/* Invalidates any TLB's as well as stats */
void setif_serialize(FILE *f)
{
  assert(f);

  fwrite((void *)&flag_eliminate_cycles, sizeof(bool), 1, f);
  fwrite((void *)&flag_merge_projections, sizeof(bool), 1, f);
  fwrite((void *)&setif_current_rollback_info,
	 sizeof(setif_rollback_info), 1, f);
  fwrite((void *)&setif_hash, sizeof(term_hash), 1, f);

  serialize_banshee_object(setif_current_rollback_info, banshee_rollback_info);
  serialize_banshee_object(setif_hash, term_hash);
}

void setif_deserialize(FILE *f)
{
  assert(f);
  
  lazy_invalidate_tlb_cache();
  fread((void *)&flag_eliminate_cycles, sizeof(bool), 1, f);
  fread((void *)&flag_merge_projections, sizeof(bool), 1, f);
  fread((void *)&setif_current_rollback_info,
	 sizeof(setif_rollback_info), 1, f);
  fread((void *)&setif_hash, sizeof(term_hash), 1, f);
}

void setif_set_fields(void)
{
  deserialize_set_obj((void **)&setif_current_rollback_info);
  deserialize_set_obj((void **)&setif_hash);
}

void write_module_setif(FILE *f)
{
  fwrite((void *)&setif_current_rollback_info,
	 sizeof(setif_rollback_info), 1, f);
  fwrite((void *)&setif_hash, sizeof(term_hash), 1, f);
  fwrite((void *)&zero, sizeof(void *), 1, f);
  fwrite((void *)&one, sizeof(void *), 1, f);
  return;
}

void update_module_setif(translation t, FILE *f)
{
  fread((void *)&setif_current_rollback_info,
	 sizeof(setif_rollback_info), 1, f);
  fread((void *)&setif_hash, sizeof(term_hash), 1, f);
  fread((void *)&zero, sizeof(void *), 1, f);
  fread((void *)&one, sizeof(void *), 1, f);

  update_pointer(t, (void **)&setif_current_rollback_info);
  update_pointer(t, (void **)&setif_hash);
  update_pointer(t, (void **)&zero);
  update_pointer(t, (void **)&one);
}

bool added_ub_proj_info_serialize(FILE *f, void *obj)
{
  added_ub_proj_info info = (added_ub_proj_info)obj;
  assert(f);

  fwrite(info, sizeof(struct added_ub_proj_info_), 1, f);
  
  serialize_banshee_object(info->ub_projs, list);
  serialize_banshee_object(info->el, list);

  return TRUE;
}

void *added_ub_proj_info_deserialize(FILE *f)
{
  added_ub_proj_info info = ralloc(added_ub_proj_info_region, 
				   struct added_ub_proj_info_);

  fread(info, sizeof(struct added_ub_proj_info_), 1, f);

  return info;
}

bool added_ub_proj_info_set_fields(void *obj)
{
  added_ub_proj_info info = (added_ub_proj_info)obj;
  deserialize_set_obj((void **)&info->ub_projs);
  deserialize_set_obj((void **)&info->el);

  return TRUE;
}

int update_setif_union(translation t, void *m)
{
  setif_union_ e = (setif_union_)m;

  if (e->type == UNION_TYPE) {
    update_pointer(t, (void **)&e->exprs);
    update_pointer(t, (void **)&e->proj_cache);
    return sizeof(struct setif_union_);
  }
  else {
    assert(e->type == ZERO_TYPE);
    return sizeof(struct setif_union_);
  }
}

int update_setif_inter(translation t, void *m)
{
  setif_inter_ e = (setif_inter_)m;
  if (e->type == INTER_TYPE) {
    update_pointer(t, (void **)&e->exprs);
    return sizeof(struct setif_inter_);
  }
  else {
    assert(e->type == ZERO_TYPE);
    return sizeof(struct setif_inter_);
  }
}

int update_setif_constant(translation t, void *m)
{
  setif_constant_ e = (setif_constant_)m;

  if (e->type == CONSTANT_TYPE) {
    update_pointer(t, (void **)&e->name);
    return sizeof(struct setif_constant_);
  }
  else {
    assert(e->type == ZERO_TYPE);
    return sizeof(struct setif_constant_);
  }
}


int update_setif_term(translation t, void *m)
{
  setif_term e = (setif_term)m;

  switch(e->type) {
  case UNION_TYPE:
    {
      update_pointer(t, (void **)& ((setif_union_)e)->exprs);
      update_pointer(t, (void **)& ((setif_union_)e)->proj_cache);
      return sizeof(struct setif_union_);
    }
  case INTER_TYPE:
    {
      update_pointer(t, (void **)& ((setif_inter_)e)->exprs);
      return sizeof(struct setif_inter_);
    }
  case CONSTANT_TYPE:
    {
      update_pointer(t, (void **)& ((setif_constant_)e)->name);
      return sizeof(struct setif_constant_);
    }
  case ZERO_TYPE:
    {
      return (sizeof(void *));
    }
  default:
    /* This is a degenerate case, where (hopefully) we've reached the
       end of the region and are just hitting garbage data */
    assert(FALSE);
    return 0;
  }
}

int update_added_ub_proj_info(translation t, void *m)
{
  added_ub_proj_info info = (added_ub_proj_info)m;

  update_pointer(t, (void **)&info->ub_projs);
  update_pointer(t, (void **)&info->el);
  
  return sizeof(struct added_ub_proj_info_);
}

int update_setif_rollback_info(translation t, void *m)
{
  setif_rollback_info info = (setif_rollback_info)m;

  update_pointer(t, (void **)&info->added_edges);
  update_pointer(t, (void **)&info->added_ub_projs);
  
  return sizeof(struct setif_rollback_info_);
}
