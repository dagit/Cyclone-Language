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
#include "regions.h"
#include "term-sort.h"
#include "hash.h"
#include "banshee_persist_kinds.h"
#include "banshee_region_persist_kinds.h"

struct term_constant_ /* extends gen_e */
{
#ifdef NONSPEC
  sort_kind sort;
#endif
  int type;
  stamp st;
  char *name;
};

typedef struct term_constant_ *term_constant_;


typedef struct term_rollback_info_ { /* extends banshee_rollback_info */
  int time;
  sort_kind kind;
  hash_table added_edges; 	/* a mapping from bounds to gen_e's added */
} * term_rollback_info; 

region term_rollback_info_region;
region term_constant_region; 
region term_var_region;
region term_sort_region;
term_hash term_sort_hash;
bool flag_occurs_check = FALSE;

term_rollback_info term_current_rollback_info = NULL;

struct term_stats term_stats;

stamp term_get_stamp(gen_e e)
{
  if ( ((gen_term)e)->type == VAR_TYPE )
    return ((gen_term)term_get_ecr(e))->st;
  else 
    return ((gen_term)e)->st;
}

gen_e term_fresh(const char *name)
{
  term_stats.fresh++;
  return (gen_e)tv_fresh(term_sort_region,name);
}

gen_e term_fresh_large(const char *name)
{
  term_stats.fresh_large++;
  return (gen_e)tv_fresh_large(term_sort_region,name);
}

gen_e term_fresh_small(const char *name)
{
  term_stats.fresh_small++;
  return (gen_e)tv_fresh_small(term_sort_region,name);
}


#ifdef NONSPEC
static struct gen_term zero = {ZERO_TYPE,term_sort,ZERO_TYPE};
static struct gen_term one  = {ONE_TYPE,term_sort,ONE_TYPE};
static struct gen_term wild = {WILD_TYPE,term_sort, WILD_TYPE};
#else
static struct gen_term zero = {ZERO_TYPE,ZERO_TYPE};
static struct gen_term one  = {ONE_TYPE,ONE_TYPE};
static struct gen_term wild = {WILD_TYPE,WILD_TYPE};
#endif /* NONSPEC */

gen_e term_zero(void)
{
  return (gen_e)&zero;
}

gen_e term_one(void)
{
  return (gen_e)&one;
}

gen_e term_wild(void)
{
  return (gen_e)&wild;
}

gen_e term_constant(const char *str)
{
  stamp st[2];
  gen_e result;
  char *name = rstrdup(banshee_nonptr_region,str);

  assert (str != NULL);
  
  st[0] = CONSTANT_TYPE;
  st[1] = stamp_string(name); 

  if ( (result = term_hash_find(term_sort_hash,st,2)) == NULL)
    {
      term_constant_ c = ralloc(term_constant_region, struct term_constant_);
      c->type = CONSTANT_TYPE;
      c->st = stamp_fresh();
      c->name = name;

      result = (gen_e) c;
      term_hash_insert(term_sort_hash,result,st,2);
      
      return result;
    }
  
  else
    {
      return result;
    }

}

static bool term_is_bottom(gen_e e)
{
  return (term_is_zero(e) || term_is_var(e));
}

bool term_is_zero(gen_e e)
{
  return ( ((gen_term)term_get_ecr(e))->type == ZERO_TYPE);
}

bool term_is_wild(gen_e e) 
{
  return ( ((gen_term)term_get_ecr(e))->type == WILD_TYPE);
}

bool term_is_one(gen_e e)
{
  return ( ((gen_term)term_get_ecr(e))->type == ONE_TYPE);
}

bool term_is_var(gen_e e)
{
  return ( ((gen_term)term_get_ecr(e))->type == VAR_TYPE);
}

bool term_is_initial_var(gen_e e)
{
  return ((gen_term)e)->type == VAR_TYPE;
}

bool term_is_constant(gen_e e)
{
  return ( ((gen_term)term_get_ecr(e))->type == CONSTANT_TYPE);
}

char *term_get_constant_name(gen_e e)
{
  gen_e ecr = term_get_ecr(e);
  if(! term_is_constant(ecr))
    return NULL;
  else
    return ((term_constant_)ecr)->name;
}

gen_e term_get_ecr(gen_e e)
{
  if (((gen_term)e)->type == VAR_TYPE)
    return tv_get_ecr((term_var)e);
  else return e;
}

static void fire_pending(term_var v, gen_e e, 
			 con_match_fn_ptr con_match, 
			 occurs_check_fn_ptr occurs)
{
  bounds_scanner scan;
  gen_e temp;

  if (tv_is_valid_pending(v)) {
    
    tv_invalidate_pending(v);
    bounds_scan(tv_get_pending(v),&scan);
    while (bounds_next(&scan,&temp))
      {
	term_unify(con_match,occurs,temp,e);
      }
  }
}

static bool eq(gen_e e1, gen_e e2)
{
  /*
    return ((gen_term)term_get_ecr(e1))->st == ((gen_term)term_get_ecr(e2))->st; */
  return term_get_stamp(e1) == term_get_stamp(e2);
}

bool term_eq(gen_e e1, gen_e e2)
{
  return eq(e1,e2);
}


void term_register_rollback(void) 
{
#ifdef BANSHEE_ROLLBACK
  term_current_rollback_info = 
    ralloc(term_rollback_info_region, struct term_rollback_info_); 
  banshee_set_time((banshee_rollback_info)term_current_rollback_info);
  term_current_rollback_info->kind = term_sort;
  term_current_rollback_info->added_edges = 
    make_persistent_hash_table(4, stamp_hash, stamp_eq,
			       BANSHEE_PERSIST_KIND_nonptr,
			       BANSHEE_PERSIST_KIND_added_edge_info);
  
  banshee_register_rollback((banshee_rollback_info)term_current_rollback_info);
#endif /* BANSHEE_ROLLBACK */
}

static void term_register_edge(const bounds b, stamp st) {
#ifdef BANSHEE_ROLLBACK
  added_edge_info info = NULL;
  assert(term_current_rollback_info);
  
  /* The current rollback info already has an edge list associated
   * with this bounds */
  if (hash_table_lookup(term_current_rollback_info->added_edges,
			(hash_key)bounds_stamp(b),
			(hash_data *)&info)) {
    assert(info);
    stamp_list_cons(st,info->sl);
  }
  else {
    stamp_list sl = new_persistent_stamp_list();
    stamp_list_cons(st,sl);
    info  = ralloc(added_edge_info_region, struct added_edge_info_);
    info->b = b;
    info->sl = sl;
    hash_table_insert(term_current_rollback_info->added_edges,
		      (hash_key)bounds_stamp(b),
		      (hash_data)info);
  }
#endif /* BANSHEE_ROLLBACK */ 
}

void term_unify(con_match_fn_ptr con_match, occurs_check_fn_ptr occurs,
		gen_e a, gen_e b)
{
  gen_e e1 = term_get_ecr(a),
    e2 = term_get_ecr(b);

  /* DEBUG */
  // printf("Term unify: %d == %d\n", ((gen_term)a)->st, ((gen_term)b)->st); 
  

#ifdef BANSHEE_ROLLBACK
  if (!banshee_check_rollback(term_sort)) {
    term_register_rollback();
  }
#endif /* BANSHEE_ROLLBACK */

  if (term_is_wild(e1) || term_is_wild(e2)) 
    {
      return;
    }

  if ( term_eq(e1,e2) )
    {
      return;
    }
  if (term_is_constant(e1) && term_is_constant(e2))
    { 
      handle_error(e1,e2,bek_cons_mismatch);
    }
  else if (term_is_var(e1))
    {
      term_var v = (term_var)e1;
   
      if (! term_is_bottom(e2))
	fire_pending(v,e2,con_match,occurs);

      if (term_is_var(e2)) {
	bounds_scanner scan;
	gen_e temp;
	const bounds pending1 = tv_get_pending((term_var)e2),
	  pending2 = tv_get_pending(v);
	tv_unify_vars(v,(term_var)e2);
	
	bounds_scan(pending1,&scan);

	while(bounds_next(&scan,&temp)) {
	  if (! tv_add_pending(v,temp,term_get_stamp(temp))) {
	    term_register_edge(tv_get_pending(v),term_get_stamp(temp));
	  }
	}
	
	bounds_scan(pending2, &scan);
	
	while(bounds_next(&scan,&temp)) {
	  if (! tv_add_pending(v,temp,term_get_stamp(temp))) {
	    term_register_edge(tv_get_pending(v),term_get_stamp(temp));
	  }
	}

      }
      else /* v = e2, e2 is not a var */
	{ 
	  if (flag_occurs_check && occurs(v,e2))
	    handle_error(e1,e2,bek_occurs_check);
	  tv_unify(v,e2); 
	}
    }
  else if (term_is_var(e2))
    {
      term_var v = (term_var)e2;

      if (! term_is_bottom(e1))
	fire_pending(v,e1,con_match,occurs);
      
      /* v = e1, e1 is not a var */
      if (flag_occurs_check && occurs(v,e1))
	handle_error(e1,e2,bek_occurs_check);
      tv_unify(v,e1); 
      
    }
  else { 
    stamp new_stamp = ((gen_term)e1)->st < ((gen_term)e2)->st ?
      ((gen_term)e1)-> st : ((gen_term)e2)-> st;

    ((gen_term)e1)->st = new_stamp;
    ((gen_term)e2)->st = new_stamp;

    con_match(e1,e2);
  }
}

void term_cunify(con_match_fn_ptr con_match, occurs_check_fn_ptr occurs,
		 gen_e e1, gen_e e2)
{

#ifdef BANSHEE_ROLLBACK
  if (!banshee_check_rollback(term_sort)) {
    term_register_rollback();
  }
#endif /* BANSHEE_ROLLBACK */

  if (term_is_wild(e1) || term_is_wild(e2)) {
    return;
  }

  if (term_is_bottom(e1) && term_is_var(e1))
    {
      term_var v1 = (term_var)e1;
      
      if (!tv_add_pending(v1,e2, term_get_stamp(e2))) {
	term_register_edge(tv_get_pending(v1),term_get_stamp(e2));
      }
    }
  else if (term_is_bottom(e1)) {
    return;
  }
  else 
    {
      term_unify(con_match,occurs,e1,e2);
    }
}

static void term_reset_stats(void)
{
  term_stats.fresh = 0;
  term_stats.fresh_small = 0;
  term_stats.fresh_large = 0;
}

void term_print_stats(FILE *f)
{
  fprintf(f,"\n========== Term Var Stats ==========\n");
  fprintf(f,"Fresh : %d\n",term_stats.fresh); 
  fprintf(f,"Fresh Small : %d\n",term_stats.fresh_small);
  fprintf(f,"Fresh Large : %d\n",term_stats.fresh_large);
  fprintf(f,"=====================================\n");
}

void term_print_constraint_graph(FILE *f)
{
}

void term_init(void)
{
  term_rollback_info_region = newregion();
  term_constant_region = newregion();
  term_var_region = newregion();
  term_sort_region = newregion();
  term_sort_hash = make_term_hash(NULL);
}

void term_reset(void)
{
  term_hash_delete(term_sort_hash);
  deleteregion(term_sort_region);
  deleteregion(term_var_region);
  deleteregion(term_constant_region);
  deleteregion(term_rollback_info_region);

  term_reset_stats();
  term_init();
}

/* For term rollbacks, we just remove conditional unifications. The uf
   rollback will undo any actual unifications */
void term_rollback(banshee_rollback_info info)
{
  hash_table_scanner hash_scan;
  stamp_list_scanner stamp_scan;
  added_edge_info next_info;
  stamp next_stamp,st;

  term_rollback_info tinfo = (term_rollback_info)info;
  
  assert(tinfo->kind = term_sort);
  
  hash_table_scan(tinfo->added_edges, &hash_scan);
  while(hash_table_next(&hash_scan,(hash_key *)&st,
			(hash_data *) &next_info)) {
    stamp_list_scan(next_info->sl, &stamp_scan);
    while(stamp_list_next(&stamp_scan,&next_stamp)) {
      bounds_remove(next_info->b,next_stamp);
    }
  }
						     
}

bool term_rollback_serialize(FILE *f, banshee_rollback_info i)
{
  term_rollback_info info = (term_rollback_info)i;

  fwrite((void *)&info->added_edges, sizeof(hash_table), 1, f);
  serialize_banshee_object(info->added_edges, hash_table);

  return TRUE;
}

banshee_rollback_info term_rollback_deserialize(FILE *f)
{
  term_rollback_info info = ralloc(term_rollback_info_region, 
				   struct term_rollback_info_);
  assert(f);

  fread((void *)&info->added_edges, sizeof(void *), 1, f);

  return (banshee_rollback_info)info;
}

bool term_rollback_set_fields(banshee_rollback_info i)
{
  term_rollback_info info = (term_rollback_info)i;

  deserialize_set_obj((void **)&info->added_edges);

  return TRUE;
}

bool term_constant_serialize(FILE *f, gen_e e)
{
  term_constant_ c = (term_constant_)e;
  assert(f);
  assert(e);

  fwrite((void *)&c->st, sizeof(stamp), 1, f);
  string_data_serialize(f,  c->name);
  
  return TRUE;
}

void *term_constant_deserialize(FILE *f)
{
  term_constant_ c = ralloc(term_constant_region, struct term_constant_);
  assert(f);

  fread((void *)&c->st, sizeof(stamp), 1, f);
  c->name = (char *)string_data_deserialize(f);
  
  return c;
}

void term_serialize(FILE *f)
{
  assert(f);
  fwrite((void *)&flag_occurs_check, sizeof(bool), 1, f);
  fwrite((void *)&term_current_rollback_info, sizeof(term_rollback_info), 1, f);
  
  serialize_banshee_object(term_current_rollback_info, banshee_rollback_info);
}

void term_deserialize(FILE *f)
{
  assert(f);
  fread((void *)&flag_occurs_check, sizeof(bool), 1, f);
  fread((void *)&term_current_rollback_info, sizeof(term_rollback_info), 1, f);
}

void term_set_fields(void)
{
  deserialize_set_obj((void **)&term_current_rollback_info);
}

void write_module_term(FILE *f)
{
  fwrite((void *)&term_current_rollback_info, sizeof(term_rollback_info), 1, f);
  return;
}

void update_module_term(translation t, FILE *f)
{
  fread((void *)&term_current_rollback_info, sizeof(term_rollback_info), 1, f);
  update_pointer(t, (void **)&term_current_rollback_info);
}

int update_term_constant(translation t, void *m)
{
  update_pointer(t, (void **)&((term_constant_)m)->name);

  return sizeof(struct term_constant_);
}
