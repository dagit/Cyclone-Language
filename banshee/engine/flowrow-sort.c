/*
 * Copyright (c) 2000-2001
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
#include <string.h>
#include "regions.h"
#include "flowrow-sort.h"
#include "termhash.h"
#include "bounds.h"
#include "setif-sort.h"
#include "banshee_persist_kinds.h"
#include "persist.h"
#include "banshee_region_persist_kinds.h"


extern hash_table fn_ptr_table;


/* generic flow row */
struct flowrow_gen
{
#ifdef NONSPEC
  sort_kind sort;
#endif
  int type;
  stamp st;
#ifdef NONSPEC
  sort_kind base_sort; 
#endif
};

typedef struct flowrow_gen *flowrow_gen;

struct flowrow
{
#ifdef NONSPEC
  sort_kind sort;
#endif 
  int type;		
  stamp st;	
#ifdef NONSPEC
  sort_kind base_sort; 
#endif
  flowrow_map fields;
  gen_e rest;
};

typedef struct flowrow *flowrow;

struct field_split
{
  gen_e_list matched1;
  gen_e_list matched2;
  flowrow_map nomatch1;
  flowrow_map nomatch2;
};

typedef struct flowrow_rollback_info_ { /* extends banshee_rollback_info */
  int time;
  sort_kind kind;
  hash_table added_edges;	/* a mapping from bounds to the gen_e's added */
  flow_var_list set_aliases;    /* a list of flow_var's that have
				   become aliases. Call fv_unset_alias
				   on them */
} *flowrow_rollback_info;

region flowrow_rollback_info_region = NULL;
region flow_var_region = NULL;
region flowrow_region = NULL;
region flowrow_field_region = NULL;
region contour_region = NULL;
term_hash flowrow_hash = NULL;
struct flowrow_stats flowrow_stats;
flowrow_rollback_info flowrow_current_rollback_info = NULL;

static void fields_print(FILE *f,flowrow_map m,field_print_fn_ptr field_print);

stamp flowrow_get_stamp(gen_e e)
{
  if ( ((flowrow_gen)e)->type == ALIAS_TYPE)
    return ((flowrow_gen)fv_get_alias( (flow_var)e ))->st;
  else
    return ((flowrow_gen)e)->st;
  
}

static flowrow_map flowrow_get_fields(gen_e e)
{
  assert (flowrow_is_row(e));
  
  return ((flowrow)e)->fields;
}

static gen_e flowrow_get_rest(gen_e e)
{
  assert(flowrow_is_row(e));

  return ((flowrow)e)->rest;
}


static int field_compare(const flowrow_field f1,const flowrow_field f2)
			
{
  int compare = strcmp(f1->label,f2->label);
  return compare;
}


static int field_compare_ne(const flowrow_field f1,const flowrow_field f2)
			
{
  int compare = strcmp(f1->label,f2->label);

  if (! compare) /* rows should never have two fields with the same labels */
    {
      fail("Multiple fields in this row share the same label\n");
    }
  return compare;
}

static struct field_split split_fields(region r, flowrow_map fields1,
				       flowrow_map fields2)
{
  struct field_split split;
  flowrow_map_scanner scan1, scan2;
  flowrow_field field1,field2;
  bool consumed1 = TRUE,consumed2 = TRUE, 
    fields1_remain = TRUE, fields2_remain = TRUE;;

  split.matched1 = new_gen_e_list(r);
  split.matched2 = new_gen_e_list(r);
  split.nomatch1 = new_flowrow_map(r);
  split.nomatch2 = new_flowrow_map(r);

  flowrow_map_scan(fields1,&scan1);
  flowrow_map_scan(fields2,&scan2);
 
  while (TRUE)
    {
      if (consumed1)
	fields1_remain = flowrow_map_next(&scan1,&field1);
      if (consumed2)
     	fields2_remain = flowrow_map_next(&scan2,&field2);

      if (fields1_remain && fields2_remain)
	{
	  int compare_fields = field_compare(field1,field2);

	  if (compare_fields < 0)
	    {
	      flowrow_map_cons(field1,split.nomatch1);
	      consumed1 = TRUE;
	      consumed2 = FALSE;
	    }
	  else if (compare_fields > 0)
	    {
	      flowrow_map_cons(field2,split.nomatch2);
	      consumed2 = TRUE;
	      consumed1 = FALSE;
	    }
	  else /* two fields are equal */
	    {
	      gen_e_list_cons(field1->expr,split.matched1);
	      gen_e_list_cons(field2->expr,split.matched2);
	      consumed1 = TRUE;
	      consumed2 = TRUE;
	      continue;
	    }
	}
      else if (fields1_remain)
	{
	  /* flowrow_map_append(split.nomatch1,flowrow_map_copy(r,fields1)); */
	  flowrow_map_cons(field1,split.nomatch1);
	  
	  while (flowrow_map_next(&scan1,&field1))
	    {
	      flowrow_map_cons(field1,split.nomatch1);
	    }

	  break;
	}
      else if (fields2_remain)
	{
	  /* flowrow_map_append(split.nomatch2,flowrow_map_copy(r,fields2)); */
	  flowrow_map_cons(field2,split.nomatch2);
	  while (flowrow_map_next(&scan2,&field2))
	    {
	      flowrow_map_cons(field2,split.nomatch2);
	    }
	  break;
	}
      else /* no remaining fields, so */ break;
    }
  
  return split;
}

static void flowrow_register_rollback(void)
{
#ifdef BANSHEE_ROLLBACK
  flowrow_current_rollback_info = ralloc(flowrow_rollback_info_region,
					 struct flowrow_rollback_info_);
  banshee_set_time((banshee_rollback_info)flowrow_current_rollback_info);
  flowrow_current_rollback_info->kind = flowrow_sort;
  flowrow_current_rollback_info->added_edges = 
    make_persistent_hash_table(4, stamp_hash, stamp_eq,
			       BANSHEE_PERSIST_KIND_nonptr,
			       BANSHEE_PERSIST_KIND_added_edge_info);
  flowrow_current_rollback_info->set_aliases = 
    new_persistent_flow_var_list();
#endif /* BANSHEE_ROLLBACK */
}

void flowrow_register_set_alias(flow_var v)
{
#ifdef BANSHEE_ROLLBACK
  assert(flowrow_current_rollback_info);
  flow_var_list_cons(v, flowrow_current_rollback_info->set_aliases);
#endif /* BANSHEE_ROLLBACK */
}

void flowrow_register_edge(const bounds b, stamp st) {
#ifdef BANSHEE_ROLLBACK
  added_edge_info info = NULL;
  assert(flowrow_current_rollback_info);
  
  /* The current rollback info already has an edge list associated
   * with this bounds */
  if (hash_table_lookup(flowrow_current_rollback_info->added_edges,
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
    hash_table_insert(flowrow_current_rollback_info->added_edges,
		      (hash_key)bounds_stamp(b),
		      (hash_data)info);
  }
#endif /* BANSHEE_ROLLBACK */
}

static bool flowrow_is_normalized(gen_e r)
{
 if ( flowrow_is_row(r) )
    {
      gen_e rest = flowrow_get_rest(r);
      
      if ( flowrow_is_row(rest) || flowrow_is_alias(rest) )
	return FALSE;
    }
 else if ( flowrow_is_alias(r) )
    return FALSE;

  return TRUE;
}

static gen_e normalize(get_stamp_fn_ptr get_stamp,
			 flowrow_map m,gen_e r) deletes
{
  if (flowrow_is_row(r))
    {
      flowrow_map_append(m,
			 flowrow_map_copy(NULL,
					  flowrow_get_fields(r)));
      return normalize(get_stamp,m,flowrow_get_rest(r));
    }
  else if (flowrow_is_alias(r))
    {
      assert (! flowrow_is_alias(fv_get_alias((flow_var)r)) );
      return normalize(get_stamp, m,fv_get_alias((flow_var)r));
    }
  else
    return flowrow_row(get_stamp,m,r);
}

static gen_e normalize_row(get_stamp_fn_ptr get_stamp, gen_e r) deletes
{
  if (flowrow_is_normalized(r))
    return r;
  else /* normalize the row */
    return normalize(get_stamp,new_persistent_flowrow_map(),r);
}

static bool eq(gen_e e1, gen_e e2)
{
  return ( flowrow_get_stamp(e1) == flowrow_get_stamp(e2) ); 
}

bool flowrow_eq(gen_e e1, gen_e e2)
{
  return eq(e1,e2);
}

/*
  A row constraint row1 <= row2 is l-inductive iff row2 is a var and for all 
  X = tlv(row1), o(row2) > o(X). 
  
  tlv(row) = {X} if row is a var X, {} otherwise 
*/
static bool l_inductive(gen_e e1, gen_e e2)
{
  if (flowrow_is_var(e2))
    {
      if (flowrow_is_var(e1))
	return flowrow_get_stamp(e2) > flowrow_get_stamp(e1);
      else return TRUE;
    }
  return FALSE;
}
      
/* 
   A row constraint row1 <= row2 is r-inductive iff row1 is a var and for all
   X = tlv(row2), o(row1) > o(X)
*/
static bool r_inductive(gen_e e1, gen_e e2)
{
  if (flowrow_is_var(e1))
    {
      if (flowrow_is_var(e2))
	return flowrow_get_stamp(e1) > flowrow_get_stamp(e2);
      else return TRUE;
    }
  return FALSE;
}

static inline bool flowrow_minimal(flowrow r)
{
  return flowrow_is_zero(r->rest);
}

static inline bool flowrow_maximal(flowrow r)
{
  return flowrow_is_one(r->rest);
}

static inline bool flowrow_closed(flowrow r)
{
  return flowrow_is_abs(r->rest);
}

static inline bool flowrow_wildcard(flowrow r)
{
  return flowrow_is_wild(r->rest);
}

static inline bool flowrow_var(flowrow r)
{
  return flowrow_is_var(r->rest);
}

gen_e contour_instantiate(fresh_fn_ptr fresh,
			  get_stamp_fn_ptr get_stamp, 
			  gen_e e) deletes
{
  if (flowrow_is_row(e))
    {
      gen_e result;
      flowrow_map_scanner scan;
      flowrow_field f;
      gen_e row = normalize_row(get_stamp,e);

      region scratch_rgn = newregion();

      flowrow_map new_fields = new_flowrow_map(scratch_rgn);
            
      flowrow_map_scan(flowrow_get_fields(row),&scan);

      while (flowrow_map_next(&scan,&f))
	{
	  flowrow_field new_field =
	    ralloc(flowrow_field_region,struct flowrow_field_);
	  new_field->label = f->label;
	  new_field->expr = fresh(NULL);
	  
	  flowrow_map_cons(new_field,new_fields);
	}
      
#ifdef NONSPEC
      result = flowrow_row(get_stamp,new_fields,flowrow_fresh(NULL,flowrow_base_sort(e)));
#else
      result = flowrow_row(get_stamp,new_fields,flowrow_fresh(NULL));
#endif

      deleteregion(scratch_rgn);

      assert( flowrow_is_row(result) );
      
      return result;
    }
  
  else
    {
      fail("Unmatched contour\n");
      return NULL;
    }
}

static contour make_contour(fresh_fn_ptr fresh,get_stamp_fn_ptr get_stamp, 
			    gen_e zero_elem,gen_e e)
{
  if (flowrow_is_row(e))
    {
      contour result;

      result = ralloc(contour_region, struct contour);
      result->shape = e;
      result->fresh = fresh;
      result->get_stamp = get_stamp;
      result->instantiate = contour_instantiate;

      hash_table_lookup(fn_ptr_table, (hash_key)result->fresh, 
			(hash_data*)&result->fresh_fn_id);
      hash_table_lookup(fn_ptr_table, (hash_key)result->get_stamp,
			(hash_data*)&result->get_stamp_id);
      hash_table_lookup(fn_ptr_table, (hash_key)result->instantiate,
			(hash_data*)&result->contour_inst_id);

      return result;
    }
  else
    {
      fail("Unmatched contour\n");
      return NULL;
    }
}
  

static  void trans_lbs(fresh_fn_ptr fresh,get_stamp_fn_ptr get_stamp, 
		       incl_fn_ptr field_incl, gen_e zero_elem,
		       flow_var v, gen_e e) deletes
{
  gen_e temp;
  bounds_scanner scan;
  
  bounds_scan(fv_get_lbs(v),&scan);
  while (bounds_next(&scan,&temp))
      flowrow_inclusion(fresh,get_stamp,field_incl,zero_elem,temp,e);
  
}

static  void trans_ubs(fresh_fn_ptr fresh,get_stamp_fn_ptr get_stamp, 
		       incl_fn_ptr field_incl, gen_e zero_elem,
		       flow_var v, gen_e e) deletes
{
  gen_e temp;
  bounds_scanner scan;
  
  bounds_scan(fv_get_ubs(v),&scan);
  while (bounds_next(&scan,&temp))
    flowrow_inclusion(fresh,get_stamp,field_incl,zero_elem,e,temp);
}

static  void update_lower_bound(fresh_fn_ptr fresh,get_stamp_fn_ptr get_stamp, 
				incl_fn_ptr field_incl, gen_e zero_elem,
				flow_var v,gen_e e) deletes
{
  if (fv_has_contour(v)) /* _ <= v, and v has a contour */
    {
      gen_e shape = fv_instantiate_contour(v);
      
      fv_set_alias(v,shape);
      trans_ubs(fresh,get_stamp,field_incl,zero_elem,v,shape);
      trans_lbs(fresh,get_stamp,field_incl,zero_elem,v,shape);
      
      flowrow_inclusion(fresh,get_stamp,field_incl,zero_elem,e,shape);
      
    }
  
  else if (flowrow_is_var(e)) 
    {
      flow_var v_lb = (flow_var)e;
      
      if (fv_has_contour(v_lb)) /* v1 <= v2, v1 has a contour */
	{
	  gen_e shape = fv_instantiate_contour(v_lb);
	  
	  fv_set_alias(v_lb,shape);
	  trans_ubs(fresh,get_stamp,field_incl,zero_elem,v_lb,shape);
	  trans_lbs(fresh,get_stamp,field_incl,zero_elem,v_lb,shape);
	  
	  flowrow_inclusion(fresh,get_stamp,field_incl,zero_elem,
			    shape,(gen_e)v);
	  
	}
      
      else /* we have v1 <= v2, no contours */
	{
	  bool redundant;
	  
	  fv_unify_contour(v,(flow_var)e);
	  redundant = fv_add_lb(v,e,flowrow_get_stamp(e));
	  
	  if (! redundant) {
	    flowrow_register_edge(fv_get_lbs(v),flowrow_get_stamp(e));
	    trans_ubs(fresh,get_stamp,field_incl,zero_elem,v,e);
	  }
	      
	}
    }
  else /* we have c(...) <= v, and v has no contour */
    {
      gen_e shape = NULL;
      fv_set_contour(v,make_contour(fresh,get_stamp,zero_elem,e));
      
      shape = fv_instantiate_contour(v);
      fv_set_alias(v,shape);
      trans_ubs(fresh,get_stamp,field_incl,zero_elem,v,shape);
      trans_lbs(fresh,get_stamp,field_incl,zero_elem,v,shape);
      
      flowrow_inclusion(fresh,get_stamp,field_incl,zero_elem,e,shape);
      
    }
}

static  void update_upper_bound(fresh_fn_ptr fresh,get_stamp_fn_ptr get_stamp, 
				incl_fn_ptr field_incl, gen_e zero_elem,
				flow_var v,gen_e e) deletes
{
  if (fv_has_contour(v)) // v isn't aliased, and we discovered a contour
    {
      gen_e shape = fv_instantiate_contour(v);
      
      fv_set_alias(v,shape);
      trans_ubs(fresh,get_stamp,field_incl,zero_elem,v,shape);
      trans_lbs(fresh,get_stamp,field_incl,zero_elem,v,shape);
      
      flowrow_inclusion(fresh,get_stamp,field_incl,zero_elem,shape,e);
	  
    }
  
  else if (flowrow_is_var(e)) 
    {
      flow_var v2 = (flow_var)e;
	  
      if (fv_has_contour(v2)) // v2 isn't aliased, and we discovered a contour
	{
	  gen_e shape = fv_instantiate_contour(v2);
	     
	  fv_set_alias(v2,shape);
	  trans_ubs(fresh,get_stamp,field_incl,zero_elem,v2,shape);
	  trans_lbs(fresh,get_stamp,field_incl,zero_elem,v2,shape);	      


	  flowrow_inclusion(fresh,get_stamp,field_incl,zero_elem,
			    (gen_e)v,shape);

	}

      else /* we have v1 <= v2, no contours */
	{
	  bool redundant;
	    
	  fv_unify_contour(v,(flow_var)e);
	  redundant = fv_add_ub(v,e,flowrow_get_stamp(e));

	  if (! redundant) {
	    flowrow_register_edge(fv_get_ubs(v), flowrow_get_stamp(e));
	    trans_lbs(fresh,get_stamp,field_incl,zero_elem,v,e);
	  }
	}
    }
  else /* we have v <= c(...), and v has no contour */
    {
      gen_e shape = NULL;
      fv_set_contour(v,make_contour(fresh,get_stamp,zero_elem,e));
	  
      shape = fv_instantiate_contour(v);

      if (! flowrow_is_row(shape) )
	{
	  assert(0);
	}
	  
      fv_set_alias(v,shape);
      trans_ubs(fresh,get_stamp,field_incl,zero_elem,v,shape);
      trans_lbs(fresh,get_stamp,field_incl,zero_elem,v,shape);
	  
	
      flowrow_inclusion(fresh,get_stamp,field_incl,zero_elem,shape,e);
	  
    }

}

// END

void flowrow_inclusion(fresh_fn_ptr fresh,get_stamp_fn_ptr get_stamp, 
		       incl_fn_ptr field_incl, gen_e zero_elem, gen_e a, 
		       gen_e b) deletes
{
  gen_e e1 = normalize_row(get_stamp, a),
    e2 = normalize_row(get_stamp, b);

  if (!banshee_check_rollback(flowrow_sort)) {
    flowrow_register_rollback();
  }

  if (eq(e1,e2))
    return;
  else if (flowrow_is_zero(e1) || flowrow_is_wild(e1))
    return;
  else if (flowrow_is_one(e2) || flowrow_is_wild(e2))
    return;

  else if ( l_inductive(e1,e2) )
    {
      flow_var v2 = (flow_var)e2;
      
      flowrow_stats.rows_l_inductive++;
      
      update_lower_bound(fresh,get_stamp,field_incl,zero_elem,v2,e1);
      return;
    }
  
  else if ( r_inductive(e1,e2) )
    {
      flow_var v1 = (flow_var)e1;

      flowrow_stats.rows_r_inductive++;

      update_upper_bound(fresh,get_stamp,field_incl,zero_elem,v1,e2);
      return;
    }

  else if ( flowrow_is_row(e1) && flowrow_is_row(e2))
    {
      region scratch_rgn = newregion();
  
      flowrow r1 = (flowrow)e1,
	r2 = (flowrow)e2;

      struct field_split split = 
	split_fields(scratch_rgn,r1->fields,r2->fields);
      
      if ( gen_e_list_empty(split.matched1) ) 
	{
	  assert ( gen_e_list_empty(split.matched2) );

	  if (flowrow_wildcard(r1) || flowrow_minimal(r1))
	    {
	      gen_e newrow = 
		flowrow_row(get_stamp,split.nomatch1,flowrow_get_rest(e1));
		
	      flowrow_inclusion(fresh,get_stamp,field_incl, zero_elem,newrow, 
				flowrow_get_rest(e2));
	    }
	  else if (flowrow_maximal(r2) || flowrow_closed(r2))
	    {
	      gen_e newrow =
		flowrow_row(get_stamp,split.nomatch2,flowrow_get_rest(e2));

	      flowrow_inclusion(fresh, get_stamp,field_incl,zero_elem,
				flowrow_get_rest(e1),newrow);
	    }
	  else 
	    {
	      gen_e rest1 = flowrow_get_rest(e1),
		rest2 = flowrow_get_rest(e2);

	      //assert( flowrow_is_var(rest1) && flowrow_is_var(rest2));

	      if ( eq(rest1,rest2))
		  fail("Recursive row resolution\n");
	      else
		{
#ifdef NONSPEC
		  gen_e fv = flowrow_fresh(NULL,flowrow_base_sort(e1));
#else
		  gen_e fv = flowrow_fresh(NULL);
#endif

		  gen_e newrow1 = flowrow_row(get_stamp,split.nomatch1,fv);
		  gen_e newrow2 = flowrow_row(get_stamp,split.nomatch2,fv);
		    
		  flowrow_inclusion(fresh,get_stamp,field_incl,
				    zero_elem,rest1,newrow2);
		  flowrow_inclusion(fresh,get_stamp,field_incl,
				    zero_elem,newrow1,rest2);
		}

	    } 
	}

      else /* some fields matched */
	{
	  gen_e_list_scanner scan1, scan2;
	  gen_e f1,f2;

	  assert( gen_e_list_length(split.matched1) 
		  == gen_e_list_length(split.matched2) );
	  
	  gen_e_list_scan(split.matched1,&scan1);
	  gen_e_list_scan(split.matched2,&scan2);

	  while (gen_e_list_next(&scan1,&f1) &&
		 gen_e_list_next(&scan2,&f2) )
	    {
	      field_incl(f1,f2);
	    }
	  
	  if ( flowrow_wildcard(r1) && flowrow_wildcard(r2) )
	    {
	      goto END;
	    }
	  else
	    {
	      flowrow_map fields1 = split.nomatch1;
	      flowrow_map fields2 = split.nomatch2;
		     
	      gen_e newrow1 = flowrow_row(get_stamp,fields1,r1->rest);
	      gen_e newrow2 = flowrow_row(get_stamp,fields2,r2->rest);
	      
	      flowrow_inclusion(fresh,get_stamp,field_incl,zero_elem,
				newrow1, newrow2);
	    }
	}
    END:
      deleteregion(scratch_rgn);
    }
  
  else /* potentially a problem normalizing a row? */
    {
      fail("Unmatched case in row inclusion\n");
      return;
    }
}

gen_e flowrow_row(get_stamp_fn_ptr get_stamp,flowrow_map f, gen_e rest) deletes
{
  flowrow_map fields = flowrow_map_copy(NULL,f);

  if (flowrow_map_empty(fields))
    {
      return rest;
    }
  else
    { 
      flowrow_map_scanner scan;
      flowrow_field temp;
      gen_e result;
      int i = 2,
	length = flowrow_map_length(fields);
      stamp st[2+2*length];

      st[0] = ROW_TYPE;
      if (rest)
	st[1] = flowrow_get_stamp(rest);
      else
	assert(0);

      flowrow_map_sort(fields,field_compare_ne);
  
      flowrow_map_scan(fields,&scan);
      while(flowrow_map_next(&scan,&temp))
      {
	st[i++] = stamp_string(temp->label);
	if (temp->expr)
	  st[i++] = get_stamp(temp->expr);
	else
	  assert(0);
      }

      if ( (result = term_hash_find(flowrow_hash,st,2 + 2*length)) == NULL)
	{
	  flowrow r = ralloc(flowrow_region, struct flowrow);
	  r->type = ROW_TYPE;
	  r->st = stamp_fresh();
	  r->fields = fields;
	  r->rest = rest;

#ifdef NONSPEC
	  r->base_sort = flowrow_map_head(fields)->expr->sort;
	  r->sort = flowrow_sort;
#endif
	  result = (gen_e) r;
	  term_hash_insert(flowrow_hash,result,st,2+2*length);
	}
      /*  assert(flowrow_is_normalized(result)); */
      return result;

    }
}

#ifndef NONSPEC
static struct flowrow_gen zero_row = {ZERO_TYPE,ZERO_TYPE};
static struct flowrow_gen one_row = {ONE_TYPE,ONE_TYPE};
static struct flowrow_gen abs_row = {ABS_TYPE, ABS_TYPE};
static struct flowrow_gen wild_row = {WILD_TYPE, WILD_TYPE};

gen_e flowrow_zero(void)
{
  return (gen_e)&zero_row;
}

gen_e flowrow_one(void)
{
  return (gen_e)&one_row;
}

gen_e flowrow_abs(void)
{
  return (gen_e)&abs_row;
}

gen_e flowrow_wild(void)
{
  return (gen_e)&wild_row;
}

gen_e flowrow_fresh(const char *name)
{
  flowrow_stats.fresh++;
  return (gen_e)fv_fresh(NULL,name);
}

gen_e flowrow_fresh_small(const char *name)
{
  flowrow_stats.fresh_small++;
  return (gen_e)fv_fresh_small(NULL,name);
}

gen_e flowrow_fresh_large(const char *name)
{
  flowrow_stats.fresh_large++;
  return (gen_e)fv_fresh_large(NULL,name);
}

#else
flowrow_gen term_zero_row = NULL;
flowrow_gen term_one_row = NULL;
flowrow_gen term_abs_row = NULL;
flowrow_gen term_wild_row = NULL;

flowrow_gen setif_zero_row = NULL;
flowrow_gen setif_one_row = NULL;
flowrow_gen setif_abs_row = NULL;
flowrow_gen setif_wild_row = NULL;

flowrow_gen setst_zero_row = NULL;
flowrow_gen setst_one_row = NULL;
flowrow_gen setst_abs_row = NULL;
flowrow_gen setst_wild_row = NULL;

/* static struct flowrow_gen term_zero_row = {flowrow_sort,ZERO_TYPE,ZERO_TYPE,term_sort}; */
/* static struct flowrow_gen term_one_row = {flowrow_sort,ONE_TYPE,ONE_TYPE,term_sort}; */
/* static struct flowrow_gen term_abs_row = {flowrow_sort,ABS_TYPE, ABS_TYPE,term_sort}; */
/* static struct flowrow_gen term_wild_row = {flowrow_sort,WILD_TYPE, WILD_TYPE,term_sort}; */

/* static struct flowrow_gen setif_zero_row = {flowrow_sort,ZERO_TYPE,ZERO_TYPE,setif_sort}; */
/* static struct flowrow_gen setif_one_row = {flowrow_sort,ONE_TYPE,ONE_TYPE,setif_sort}; */
/* static struct flowrow_gen setif_abs_row = {flowrow_sort,ABS_TYPE, ABS_TYPE,setif_sort}; */
/* static struct flowrow_gen setif_wild_row = {flowrow_sort,WILD_TYPE, WILD_TYPE,setif_sort}; */

/* static struct flowrow_gen setst_zero_row = {flowrow_sort,ZERO_TYPE,ZERO_TYPE,setst_sort}; */
/* static struct flowrow_gen setst_one_row = {flowrow_sort,ONE_TYPE,ONE_TYPE,setst_sort}; */
/* static struct flowrow_gen setst_abs_row = {flowrow_sort,ABS_TYPE, ABS_TYPE,setst_sort}; */
/* static struct flowrow_gen setst_wild_row = {flowrow_sort,WILD_TYPE, WILD_TYPE,setst_sort}; */

static flowrow_gen make_constant_row(sort_kind base_sort, int type) {
  flowrow_gen result = ralloc(banshee_nonptr_region, struct flowrow_gen);
  result->base_sort = base_sort;
  result->sort = flowrow_sort;
  result->st = type;
  result->type = type;
  return result;
}

gen_e flowrow_zero(sort_kind base_sort)
{
  switch (base_sort)
    {
    case setif_sort:
      if (!setif_zero_row)
	setif_zero_row = make_constant_row(setif_sort, ZERO_TYPE);
      return (gen_e)setif_zero_row;
    case setst_sort:
      if (!setst_zero_row)
	setst_zero_row = make_constant_row(setst_sort, ZERO_TYPE);
      return (gen_e)setst_zero_row;
    case term_sort:
      if (!term_zero_row)
	term_zero_row = make_constant_row(term_sort, ZERO_TYPE);
      return (gen_e)term_zero_row;
    default:
      {
	assert(FALSE);
	return NULL;
      }
    }
  
  return NULL;
}

gen_e flowrow_one(sort_kind base_sort)
{
  switch (base_sort)
    {
    case setif_sort:
      if (!setif_one_row)
	setif_one_row = make_constant_row(setif_sort, ONE_TYPE);
      return (gen_e)setif_one_row;
    case setst_sort:
      if (!setst_one_row)
	setst_one_row = make_constant_row(setst_sort, ONE_TYPE);
      return (gen_e)setst_one_row;
    case term_sort:
      if (!term_one_row)
	term_one_row = make_constant_row(term_sort, ONE_TYPE);
      return (gen_e)term_one_row;
    default:
      {
	assert(FALSE);
	return NULL;
      }
    }
  
  return NULL;
}

gen_e flowrow_wild(sort_kind base_sort)
{
  switch (base_sort)
    {
    case setif_sort:
      if (!setif_wild_row)
	setif_wild_row = make_constant_row(setif_sort, WILD_TYPE);
      return (gen_e)setif_wild_row;
    case setst_sort:
      if (!setst_wild_row)
	setst_wild_row = make_constant_row(setst_sort, WILD_TYPE);
      return (gen_e)setst_wild_row;
    case term_sort:
      if (!term_wild_row)
	term_wild_row = make_constant_row(term_sort, WILD_TYPE);
      return (gen_e)term_wild_row;
    default:
      {
	assert(FALSE);
	return NULL;
      }
    }
  
  return NULL;
}


gen_e flowrow_abs(sort_kind base_sort)
{
  switch (base_sort)
    {
    case setif_sort:
      if (!setif_abs_row)
	setif_abs_row = make_constant_row(setif_sort, ABS_TYPE);
      return (gen_e)setif_abs_row;
    case setst_sort:
      if (!setst_abs_row)
	setst_abs_row = make_constant_row(setst_sort, ABS_TYPE);
      return (gen_e)setst_abs_row;
    case term_sort:
      if (!term_abs_row)
	term_abs_row = make_constant_row(term_sort, ABS_TYPE);
      return (gen_e)term_abs_row;
    default:
      {
	assert(FALSE);
	return NULL;
      }
    }
  
  return NULL;
}

/* gen_e flowrow_one(sort_kind base_sort) */
/* { */
/*   switch (base_sort) */
/*     { */
/*     case setif_sort: */
/*       return (gen_e)&setif_one_row; */
/*     case setst_sort: */
/*       return (gen_e)&setst_one_row; */
/*     case term_sort: */
/*       return (gen_e)&term_one_row; */
/*     default: */
/*       { */
/* 	assert(FALSE); */
/* 	return NULL; */
/*       } */
/*     } */
  
/*   return NULL; */
/* } */

/* gen_e flowrow_abs(sort_kind base_sort) */
/* { */
/*   switch (base_sort) */
/*     { */
/*     case setif_sort: */
/*       return (gen_e)&setif_abs_row; */
/*     case setst_sort: */
/*       return (gen_e)&setst_abs_row; */
/*     case term_sort: */
/*       return (gen_e)&term_abs_row; */
/*     default: */
/*       { */
/* 	assert(FALSE); */
/* 	return NULL; */
/*       } */
/*     } */
  
/*   return NULL; */
/* } */

/* gen_e flowrow_wild(sort_kind base_sort) */
/* { */

/*   switch (base_sort) */
/*     { */
/*     case setif_sort: */
/*       return (gen_e)&setif_wild_row; */
/*     case setst_sort: */
/*       return (gen_e)&setst_wild_row; */
/*     case term_sort: */
/*       return (gen_e)&term_wild_row; */
/*     default: */
/*       { */
/* 	assert(FALSE); */
/* 	return NULL; */
/*       } */
/*     } */
  
/*   return NULL; */
/* } */

gen_e flowrow_fresh(const char *name,sort_kind base_sort)
{
  flow_var result = fv_fresh_large(flowrow_region,name);
  fv_set_extra_info(result,(void *)base_sort, BANSHEE_PERSIST_KIND_nonptr);
  
  flowrow_stats.fresh++;
  return (gen_e)result;
}

gen_e flowrow_fresh_small(const char *name,sort_kind base_sort)
{
  flow_var result = fv_fresh_small(flowrow_region,name);
  fv_set_extra_info(result,(void *)base_sort, BANSHEE_PERSIST_KIND_nonptr);
  
  flowrow_stats.fresh_small++;
  return (gen_e)result;
}

gen_e flowrow_fresh_large(const char *name,sort_kind base_sort)
{
  flow_var result = fv_fresh_large(flowrow_region,name);
  fv_set_extra_info(result,(void *)base_sort, BANSHEE_PERSIST_KIND_nonptr);
  
  flowrow_stats.fresh_large++;
  return (gen_e)result;
}

sort_kind flowrow_base_sort(gen_e e)
{
  if ( flowrow_is_var(e) || flowrow_is_alias(e) )
    {
      return (sort_kind)fv_get_extra_info((flow_var)e);
    }
  else
    return ((flowrow_gen)e)->base_sort;
}

#endif /* NONSPEC */


static const char *fef_name;

static bool fef_field_eq(const flowrow_field f) {
	return (! strcmp(f->label,fef_name));
}

gen_e flowrow_extract_field(const char *name, gen_e e)
{
  fef_name = name;
  
  if (flowrow_is_row(e))
    {
      flowrow_map fields = flowrow_get_fields(e);
      flowrow_field f = flowrow_map_find(fields,fef_field_eq);

      if (f) return f->expr;
    }
  return NULL;
}

gen_e flowrow_extract_rest(gen_e e)
{
  if (flowrow_is_row(e))
    return flowrow_get_rest(e);
  else
    return NULL;
}

flowrow_map flowrow_extract_fields(gen_e e)
{
  if (flowrow_is_row(e))
    return flowrow_map_copy(flowrow_region,flowrow_get_fields(e));
  else
    return NULL;
}


bool flowrow_is_alias(gen_e e)
{
  return ((flowrow_gen)e)->type == ALIAS_TYPE;
}

bool flowrow_is_zero(gen_e e)
{
  return ((flowrow_gen)e)->type == ZERO_TYPE;
}

bool flowrow_is_one(gen_e e)
{
  return ((flowrow_gen)e)->type == ONE_TYPE;
}

bool flowrow_is_abs(gen_e e)
{
  return ((flowrow_gen)e)->type == ABS_TYPE;
}

bool flowrow_is_wild(gen_e e)
{
  return ((flowrow_gen)e)->type == WILD_TYPE;
}

bool flowrow_is_var(gen_e e)
{
  return ((flowrow_gen)e)->type == VAR_TYPE;
}

bool flowrow_is_row(gen_e e)
{
  return ((flowrow_gen)e)->type == ROW_TYPE;
}

void flowrow_init(void)
{
  flowrow_rollback_info_region = newregion();
  flow_var_region = newregion();
  flowrow_region = newregion();
  contour_region = newregion();
  flowrow_field_region = newregion();
  flowrow_hash = make_term_hash(NULL);
}

static void flowrow_reset_stats(void)
{
  flowrow_stats.fresh = 0;
  flowrow_stats.fresh_small = 0;
  flowrow_stats.fresh_large = 0;

  flowrow_stats.rows_disjoint_wild = 0;
  flowrow_stats.rows_equal = 0;
  flowrow_stats.rows_zero_one_wild = 0;
  flowrow_stats.rows_l_inductive = 0;
  flowrow_stats.rows_r_inductive = 0;
  flowrow_stats.rows_disjoint_r1_minimal = 0;
  flowrow_stats.rows_disjoint_r1_var_r2_minimal = 0;
  flowrow_stats.rows_disjoint_r1_var_r2_maximal = 0;
  flowrow_stats.rows_disjoint_r1_var_r2_closed = 0;
  flowrow_stats.rows_disjoint_r1_var_r2_var_lt = 0;
  flowrow_stats.rows_disjoint_r1_var_r2_var_gt = 0;
  flowrow_stats.rows_equal_domains = 0;
  flowrow_stats.rows_nonempty_intersection = 0;
  flowrow_stats.rows_fresh = 0;
  flowrow_stats.rows_fresh_large = 0;
}

void flowrow_reset(void) deletes
{
  term_hash_delete(flowrow_hash);
  deleteregion(flowrow_region);
  deleteregion(flow_var_region);
  deleteregion(flowrow_rollback_info_region);
  deleteregion(flowrow_field_region);
  deleteregion(contour_region);

  flowrow_reset_stats();

  flowrow_init();
}

static void fields_print(FILE *f,flowrow_map m,field_print_fn_ptr field_print) deletes
{
  flowrow_map_scanner scan;
  flowrow_field temp;

  flowrow_map_scan(m,&scan);

  if (flowrow_map_next(&scan,&temp))
    {
      fprintf(f,"%s : ",temp->label);
      
      if (field_print)
	field_print(f,temp->expr);
      else
	fprintf(f,"?");
    }

  while (flowrow_map_next(&scan,&temp))
    {
      fprintf(f,",%s : ",temp->label);
     
      if (field_print)
	field_print(f,temp->expr);
      else
 	fprintf(f,"?");
    }
}

void flowrow_print(FILE *f,get_stamp_fn_ptr get_stamp, 
		   field_print_fn_ptr field_print,gen_e row) deletes
{
  gen_e e = normalize_row(get_stamp, row);

  switch ( ((flowrow_gen)e)->type)
    {
    case ZERO_TYPE:
      fprintf(f, "0");
      break;
    case ONE_TYPE:
      fprintf(f, "1");
      break;
    case ABS_TYPE:
      fprintf(f, "abs");
      break;
    case WILD_TYPE:
      fprintf(f, "wild");
      break;
    case VAR_TYPE:
      fprintf(f, fv_get_name((flow_var)e));
      break;
    case ROW_TYPE:
      fprintf(f, "<");
      fields_print(f, flowrow_get_fields(e), field_print);
      fprintf(f, " | ");
      flowrow_print(f, get_stamp, field_print, flowrow_get_rest(e));
      fprintf(f, ">");
      break;
    default:
      assert(0);
      break;
    }
}

void flowrow_print_stats(FILE *f)
{
  fprintf(f,"\n========== Flow Var Stats ==========\n");
  fprintf(f,"Fresh : %d\n",flowrow_stats.fresh); 
  fprintf(f,"Fresh Small : %d\n",flowrow_stats.fresh_small);
  fprintf(f,"Fresh Large : %d\n",flowrow_stats.fresh_large);
  fprintf(f,"=====================================\n");
}

DEFINE_LIST(flowrow_map,flowrow_field);

void flowrow_rollback(banshee_rollback_info info)
{
  hash_table_scanner hash_scan;
  stamp_list_scanner stamp_scan;
  added_edge_info next_info;
  stamp next_stamp, st;
  flow_var_list_scanner alias_scan;
  flow_var next_aliased;
  
  flowrow_rollback_info rinfo = (flowrow_rollback_info)info;
  
  assert(rinfo->kind = flowrow_sort);
  
  hash_table_scan(rinfo->added_edges, &hash_scan);
  while(hash_table_next(&hash_scan,(hash_key *)&st,
			(hash_data *) &next_info)) {
    stamp_list_scan(next_info->sl, &stamp_scan);
    while(stamp_list_next(&stamp_scan,&next_stamp)) {
      bounds_remove(next_info->b,next_stamp);
    }
  }

  flow_var_list_scan(rinfo->set_aliases,&alias_scan);
  while(flow_var_list_next(&alias_scan,&next_aliased)) {
    fv_unset_alias(next_aliased);
  }
}

/* Persistence */

bool flowrow_field_serialize(FILE *f, void *obj)
{
  flowrow_field field = (flowrow_field)obj;
  assert(f);
  assert(field);

  string_data_serialize(f, field->label);
  fwrite(&field->expr, sizeof(gen_e), 1, f);
  serialize_banshee_object(field->expr, gen_e);

  return TRUE;
}


void *flowrow_field_deserialize(FILE *f)
{
  flowrow_field result;
  assert(f);
  
  result = ralloc(flowrow_field_region, struct flowrow_field_);

  result->label = (char *)string_data_deserialize(f);
  fread(&result->expr, sizeof(gen_e), 1 ,f);
  
  return result;
}

bool flowrow_field_set_fields(void *obj)
{
  flowrow_field field = (flowrow_field)obj;

  deserialize_set_obj((void **)&field->expr);

  return TRUE;
}


bool flowrow_rollback_serialize(FILE *f, banshee_rollback_info i)
{
  flowrow_rollback_info info = (flowrow_rollback_info)i;
  assert(f);
  assert(info);

/*   fwrite((void *)&info->added_edges, sizeof(hash_table), 1, f); */
/*   fwrite((void *)&info->set_aliases, sizeof(flow_var_list), 1, f); */
  fwrite(&info->added_edges, sizeof(hash_table) + sizeof(flow_var_list), 1 ,f);

  serialize_banshee_object(info->added_edges, hash_table);
  serialize_banshee_object(info->set_aliases, list);

  return TRUE;
}

banshee_rollback_info flowrow_rollback_deserialize(FILE *f)
{
  flowrow_rollback_info info = 
    ralloc(flowrow_rollback_info_region, struct flowrow_rollback_info_);
  
  assert(f);

/*   fread((void *)&info->added_edges, sizeof(hash_table), 1, f); */
/*   fread((void *)&info->set_aliases, sizeof(flow_var_list), 1, f); */
  fread(&info->added_edges, sizeof(hash_table) + sizeof(flow_var_list), 1, f);

  return (banshee_rollback_info)info;
}

bool flowrow_rollback_set_fields(banshee_rollback_info i)
{
  flowrow_rollback_info info = (flowrow_rollback_info)i;

  deserialize_set_obj((void **)&info->added_edges);
  deserialize_set_obj((void **)&info->set_aliases);

  return TRUE;
}

void *flowrow_expr_deserialize(FILE *f)
{
  int type;

#ifdef NONSPEC
  int base_type;
#endif	/* NONSPEC */

  assert(f);
  fread((void *)&type, sizeof(int), 1, f);

#ifdef NONSPEC
  if (type == ZERO_TYPE || type == ONE_TYPE || type == ABS_TYPE ||
      type == WILD_TYPE) {
    fread(&base_type, sizeof(int), 1, f);
  }  
#endif

  switch(type) 
    {
#ifdef NONSPEC
    case ZERO_TYPE:
      return flowrow_zero(base_type);
    case ONE_TYPE:
      return flowrow_one(base_type);
    case ABS_TYPE:
      return flowrow_abs(base_type);
    case WILD_TYPE:
      return flowrow_wild(base_type);
#else
    case ZERO_TYPE:
      return flowrow_zero();
    case ONE_TYPE:
      return flowrow_one();
    case ABS_TYPE:
      return flowrow_abs();
    case WILD_TYPE:
      return flowrow_wild();
#endif	/* NONSPEC */
    case ALIAS_TYPE:
    case VAR_TYPE:
      {
	flowrow_gen result = (flowrow_gen)flow_var_deserialize(f);
	result->type = type;
	return result;
      }
    case ROW_TYPE:
      {
	flowrow frow = ralloc(flowrow_region, struct flowrow);
	fread(&frow->st, sizeof(stamp), 1, f);
#ifdef NONSPEC      
	fread(&frow->base_sort, sizeof(int), 1, f);
#endif
/* 	fread((void *)&frow->fields, sizeof(flowrow_map), 1, f); */
/* 	fread((void *)&frow->rest, sizeof(gen_e), 1, f); */
	fread(&frow->fields, sizeof(flowrow_map) + sizeof(gen_e), 1, f);
	frow->type = type;
	return frow;
      }
    default:
      assert(0);
      return NULL;
    }

}


bool flowrow_expr_serialize(FILE *f, void *obj)
{
  flowrow_gen row = (flowrow_gen)obj;
  assert(f);
  assert(row);

  fwrite(&row->type, sizeof(int), 1, f);
  switch (row->type)
    {
    case ZERO_TYPE:
    case ONE_TYPE:
    case ABS_TYPE:
    case WILD_TYPE:
#ifdef NONSPEC      
      fwrite(&row->base_sort, sizeof(int), 1, f);
#endif
      return TRUE;
    case ALIAS_TYPE:
    case VAR_TYPE:
      return flow_var_serialize(f,row);
    case ROW_TYPE:
      {
	flowrow frow = (flowrow)row;
	fwrite(&frow->st, sizeof(stamp), 1, f);
#ifdef NONSPEC      
	fwrite(&frow->base_sort, sizeof(int), 1, f);
#endif
/* 	fwrite(&frow->fields, sizeof(flowrow_map), 1, f); */
/* 	fwrite(&frow->rest, sizeof(gen_e), 1, f); */
	fwrite(&frow->fields, sizeof(flowrow_map) + sizeof(gen_e), 1 ,f);
	serialize_banshee_object(frow->fields, list);
	serialize_banshee_object(frow->rest, gen_e);
	return TRUE;
    } 
    default:
      assert(0);
      return FALSE;
    }
}

bool flowrow_expr_set_fields(void *obj)
{
  flowrow_gen row = (flowrow_gen)obj;
  assert(row);

  switch(row->type)
    {
    case ALIAS_TYPE:
    case VAR_TYPE:
      return flow_var_set_fields(row);
    case ROW_TYPE:
      {
	flowrow frow = (flowrow)row;
	deserialize_set_obj((void **)&frow->fields);
	deserialize_set_obj((void **)&frow->rest);
	return TRUE;
      } 
    default:
      return TRUE;
    }

}

void flowrow_serialize(FILE *f)
{
  assert(f);

  fwrite(&flowrow_hash, sizeof(term_hash), 1, f);
  fwrite(&flowrow_current_rollback_info, 
	 sizeof(flowrow_rollback_info), 1, f);

  serialize_banshee_object(flowrow_hash, term_hash);
  serialize_banshee_object(flowrow_current_rollback_info, 
			   banshee_rollback_info);
}

void flowrow_deserialize(FILE *f)
{
  fread(&flowrow_hash, sizeof(term_hash), 1, f);
  fread(&flowrow_current_rollback_info, sizeof(flowrow_rollback_info), 1, f);
}

void flowrow_set_fields(void)
{
  deserialize_set_obj((void **)&flowrow_hash);
  deserialize_set_obj((void **)&flowrow_current_rollback_info);
}


void write_module_flowrow(FILE *f)
{
  fwrite(&flowrow_hash, sizeof(term_hash), 1, f);
  fwrite(&flowrow_current_rollback_info, 
	 sizeof(flowrow_rollback_info), 1, f);
  return;
}

void update_module_flowrow(translation t, FILE *f)
{
  fread(&flowrow_hash, sizeof(term_hash), 1, f);
  fread(&flowrow_current_rollback_info, sizeof(flowrow_rollback_info), 1, f);
  update_pointer(t, (void **)&flowrow_hash);
  update_pointer(t, (void **)&flowrow_current_rollback_info);

  /* TODO -- fix */
/* #ifdef NONSPEC */
/*   update_pointer(t, (void **)&term_zero_row); */
/*   update_pointer(t, (void **)&term_one_row); */
/*   update_pointer(t, (void **)&term_abs_row); */
/*   update_pointer(t, (void **)&term_wild_row); */
  
/*   update_pointer(t, (void **)&setif_zero_row); */
/*   update_pointer(t, (void **)&setif_one_row); */
/*   update_pointer(t, (void **)&setif_abs_row); */
/*   update_pointer(t, (void **)&setif_wild_row); */
  
/*   update_pointer(t, (void **)&setst_zero_row); */
/*   update_pointer(t, (void **)&setst_one_row); */
/*   update_pointer(t, (void **)&setst_abs_row); */
/*   update_pointer(t, (void **)&setst_wild_row); */
/* #endif /\* NONSPEC *\/ */
}

int update_flowrow_rollback_info(translation t, void *m)
{
  flowrow_rollback_info info = (flowrow_rollback_info)m;

  update_pointer(t, (void **)&info->added_edges);
  update_pointer(t, (void **)&info->set_aliases);
  
  return sizeof(struct flowrow_rollback_info_);
}

int update_flowrow(translation t, void *m)
{
  flowrow f = (flowrow)m;

  update_pointer(t, (void **)&f->fields);
  update_pointer(t, (void **)&f->rest);

  return sizeof(struct flowrow);
}

int update_flowrow_field(translation t, void *m)
{
  flowrow_field f = (flowrow_field)m;
  update_pointer(t, (void **)&f->label);
  update_pointer(t, (void **)&f->expr);
  
  return sizeof(struct flowrow_field_);
}

int update_contour(translation t, void *m)
{
  contour c = (contour)m;
  update_pointer(t, (void **)&c->shape);

  c->fresh = update_funptr_data(c->fresh_fn_id);
  c->get_stamp = update_funptr_data(c->get_stamp_id);
  c->instantiate = update_funptr_data(c->contour_inst_id);

  hash_table_lookup(fn_ptr_table, (hash_key)c->fresh, 
		    (hash_data*)&c->fresh_fn_id);
  hash_table_lookup(fn_ptr_table, (hash_key)c->get_stamp,
		    (hash_data*)&c->get_stamp_id);
  hash_table_lookup(fn_ptr_table, (hash_key)c->instantiate,
		    (hash_data*)&c->contour_inst_id);

  return sizeof(struct contour);
}
