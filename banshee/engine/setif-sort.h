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

#ifndef SETIF_SORT_H
#define SETIF_SORT_H

#include <stdio.h>
#include "banshee.h"
#include "termhash.h"
#include "setif-var.h"
#include "annotations.h"

EXTERN_C_BEGIN

struct setif_term /* extends gen_e */
{
#ifdef NONSPEC
  sort_kind sort;
#endif
  int type;
  stamp st;
};

typedef struct setif_term *setif_term;

extern bool flag_merge_projections;
extern bool flag_eliminate_cycles;

extern term_hash setif_hash;

typedef bool (*res_proj_fn_ptr) (setif_var v,gen_e proj) deletes;
typedef gen_e (*proj_con_fn_ptr) (gen_e) deletes;

stamp setif_get_stamp(gen_e e);
void setif_inclusion(con_match_fn_ptr,res_proj_fn_ptr, gen_e_pr_fn_ptr,
		     gen_e, gen_e) deletes;

void setif_annotated_inclusion(con_match_fn_ptr,res_proj_fn_ptr, 
			       gen_e_pr_fn_ptr,
			       gen_e, gen_e, 
			       annotation) deletes;



bool setif_proj_merge(setif_var v, gen_e se, get_proj_fn_ptr get_proj,
		      proj_con_fn_ptr make_proj,fresh_large_fn_ptr fresh_var,
		      incl_fn_ptr sort_incl, incl_fn_ptr set_incl) deletes;

gen_e setif_zero(void);
gen_e setif_one(void);
gen_e setif_wild(void);
gen_e setif_fresh(const char *name);
gen_e setif_fresh_large(const char *name);
gen_e setif_fresh_small(const char *name);
gen_e setif_constant(const char *name) deletes;
gen_e setif_union(gen_e_list exprs) deletes;
gen_e setif_inter(gen_e_list exprs) deletes;
bool setif_is_zero(gen_e e);
bool setif_is_one(gen_e e);
bool setif_is_wild(gen_e e);
bool setif_is_var(gen_e e);
bool setif_is_union(gen_e e);
bool setif_is_inter(gen_e e);
bool setif_is_constant(gen_e e);
char *setif_get_constant_name(gen_e e);

bool setif_eq(gen_e e1, gen_e e2);

gen_e_list setif_get_union(gen_e e);
gen_e_list setif_get_inter(gen_e e);

gen_e_list setif_tlb(gen_e e) deletes;

void setif_set_proj_cache(gen_e e, gen_e elem);
gen_e_list setif_get_proj_cache(gen_e e);

void setif_init(void);
void setif_reset(void) deletes;
void setif_print_stats(FILE *f);
void setif_print_constraint_graph(FILE *f);

void setif_register_ub_proj(gen_e_list ubprojs,gen_e e);
void setif_rollback(banshee_rollback_info);
void setif_register_rollback();

/* Persistence  */
bool setif_rollback_serialize(FILE *f, banshee_rollback_info);
banshee_rollback_info setif_rollback_deserialize(FILE *f);
bool setif_rollback_set_fields(banshee_rollback_info i);
bool setif_union_serialize(FILE *f, gen_e e);
bool setif_inter_serialize(FILE *f, gen_e e);
bool setif_constant_serialize(FILE *f, gen_e e);
void *setif_union_deserialize(FILE *f);
void *setif_inter_deserialize(FILE *f);
void *setif_constant_deserialize(FILE *f);
bool setif_union_set_fields(gen_e e);
bool setif_inter_set_fields(gen_e e);
bool setif_constant_set_fields(gen_e e);
void setif_serialize(FILE *F);
void setif_deserialize(FILE *f);
void setif_set_fields(void);

/* Region persistence */
void write_module_setif(FILE *f);
void update_module_setif(translation t, FILE *f);

/* Annotated constraints */
/* extern transition_fn transition; */
/* extern empty_annotation_fn is_empty_annotation; */
/* extern subsumption_fn subsumed; */
/* extern eq_annotation_fn eq_annotation; */

/* Stats */
extern struct setif_stats setif_stats;
struct setif_stats
{
  int fresh;
  int fresh_small;
  int fresh_large;

  int distinct_constructors;
  int hashed_constructors;
  int distinct_constants;
  int hashed_constants;
  int distinct_unions;
  int filtered_unions;
  int hashed_unions;
  int distinct_intersections;
  int filtered_intersections;
  int hashed_intersections;

  int redundant_pred;
  int redundant_succ;
  int redundant_source;
  int redundant_sink;
  
  int added_pred;
  int added_succ;
  int added_source;
  int added_sink;
  
  int cycles_searched_forward;
  int cycles_searched_backward;
  
  int cycles_collapsed_forward;
  int cycles_collapsed_backward;
  
  int cycles_length_forward;
  int cycles_length_backward;
};

EXTERN_C_END

#endif /* SETIF_SORT_H */
