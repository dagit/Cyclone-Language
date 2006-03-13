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

/*****************************************************************************
 *                                                                           *
 *   Nonspecialized Interface for Solving Mixed Constraints w/ Banshee       *
 *                                                                           *
 *****************************************************************************/

#ifndef NONSPEC_H
#define NONSPEC_H
#include <stdio.h>
#include "linkage.h"
#include "bool.h"
#include "list.h"
#include "hash.h"
#include "banshee_persist_kinds.h"

EXTERN_C_BEGIN

/*****************************************************************************
 *                                                                           *
 *   Type declarations                                                       *
 *                                                                           *
 *****************************************************************************/

/* The sorts supported by banshee */
typedef enum  {
  flowrow_sort,
  setif_sort,
  setst_sort,
  term_sort,
} sort_kind;

/* Fully expanded sort kinds. Translate to sort_kinds as needed. These
   are needed when we have row(base) and need to know the base which
   occurs when we have 0,1, or _ row expressions (for ibanshee) */
typedef enum {
  e_setif_sort,
  e_term_sort,
  e_flowrow_setif_sort,
  e_flowrow_term_sort
} e_sort;

/* Variance kinds. Positive (covariant), negative (contravariant), and
   non (invariant) */
typedef enum {
  vnc_pos,
  vnc_neg,
  vnc_non
} vnc_kind;

/* Signature elements: a (variance, sort) pair */
struct sig_elt_
{
  vnc_kind variance;
  sort_kind sort;
};

/* These are opaque types: generic expressions, signature elements,
   constructors, and fields for rows */
typedef struct gen_e_ *gen_e;
typedef struct sig_elt_ sig_elt;
typedef struct constructor_ *constructor;
typedef struct flowrow_field_ *flowrow_field;

/* Pattern data type for projections. Used by ibanshee */
typedef struct {
  constructor c;
  int i;
  gen_e e;
} pattern;

/*****************************************************************************
 *                                                                           *
 *   List Declarations                                                       *
 *                                                                           *
 *****************************************************************************/

/* These macros define type safe list operations. The complete list of
   operations is in list.h */

DECLARE_LIST(gen_e_list, gen_e);
DECLARE_LIST(sig_elt_list,sig_elt*);
DECLARE_LIST(flowrow_map,flowrow_field);

/*****************************************************************************
 *                                                                           *
 *   Flags                                                                   *
 *                                                                           *
 *****************************************************************************/

/* Flags are used to control banshee's internal solvers. Most of the
   flags control certain optimizations. To enable a flag, set the
   variable equal to TRUE, or FALSE to disable. For example:

   flag_elimininate_cycles = TRUE;

   This enables cycle elimination. You should set these ONCE for the
   lifetime of the solver. Behavior is unspecified if you change the
   value of a flag after a constraint has been added to the system. */


/* Enable/disable projection merging. It should be disabled if
   constructor groups are used (due to a bug). */
extern bool flag_merge_projections;

/* Enable/disable partial online cycle elimination */
extern bool flag_eliminate_cycles; 

/* Enable/disable the occurs check for term sort unification */
extern bool flag_occurs_check;

/*****************************************************************************
 *                                                                           *
 *   Initialize/Reset the Constraint Solver                                  *
 *                                                                           *
 *****************************************************************************/

/* Initialize the constraint solver. This must be called once before
   any other function is called */
void nonspec_init(void);

/* Reset the constraint solver. Frees all memory, resets any internal
   state. Note this handles calling nonspec_init for you */
void nonspec_reset(void);

/*****************************************************************************
 *                                                                           *
 *   Error Handling                                                          *
 *                                                                           *
 *****************************************************************************/

/* Banshee allows you to specify your own error handling
   function. Banshee errors occur when there is a problem adding a
   constraint to the current constraint system: for example, if the
   added constraint causes the system to be inconsistent. The intent
   is to allow you to examine the error, ignore it, or recover
   (e.g. by backtracking) as you see fit. The default error handler
   will just print a message to stderr. */

/* These are the different error kinds. Keep these in sync with
   banshee.h and the code generator */
typedef enum banshee_error_kind
{
  bek_cons_mismatch, 	        // c(...) <= d(...)
  bek_occurs_check,		// occurs check failed (cyclic unification)
  bek_context_incompatibility
} banshee_error_kind;

/* This is the type for error callbacks. Error handling functions are
   passed the two expressions that caused the problem, as well as the
   error kind. User-defined error functions should match this
   signature */
typedef void (*banshee_error_handler_fn) (gen_e e1, gen_e e2, banshee_error_kind kind);

/* Register a user defined error handler */
void register_error_handler(banshee_error_handler_fn error_handler);

/*****************************************************************************
 *                                                                           *
 *   Operations for Building Expressions                                     *
 *                                                                           *
 *****************************************************************************/

/* Defines a new constructor for sort s with the given signature */
constructor make_constructor(const char *name,sort_kind sort, sig_elt[],
			     int arity);

/* An alternate way of defining a constructor, if you have a list
   available instead of an array */
constructor make_constructor_from_list(const char *name, sort_kind sort,
				       sig_elt_list elts);

/* Build the term c(exps[0]....exps[n]) */
gen_e constructor_expr(constructor c, gen_e exps[], int arity);

/* Make a constant (0-ary constructor) for each sort kind that
   supports them */
gen_e setif_constant(const char *name);
gen_e setst_constant(const char *name);
gen_e term_constant(const char *name);

/* Creates a projection pattern projpat(c,i,e) */
gen_e setif_proj_pat(constructor c,int i,gen_e e);
gen_e setst_proj_pat(constructor c, int i, gen_e e);

/* Projections. Adds a constraint e <= projpat(c,i,fv) where fv is a
   fresh variable */
gen_e setif_proj(constructor c, int i, gen_e e);
gen_e setst_proj(constructor c, int i, gen_e e);

/* Make a new variable for each sort that supports variables */
gen_e setif_fresh(const char *name);
gen_e term_fresh(const char *name);
gen_e flowrow_fresh(const char *name,sort_kind base_sort);
gen_e setst_fresh(const char *name);

/* Create a union expression. Unions are l-compatible: they should
   only ever appear on the left-hand-side of an inclusion constraint */
gen_e setif_union(gen_e_list exps);
gen_e setst_union(gen_e_list exps);

/* Create an intersection expression. Intersections are r-compatible:
   they should only ever appear on the right-hand-side of an inclusion
   constraint */
gen_e setif_inter(gen_e_list exps);
gen_e setst_inter(gen_e_list exps);

/* Empty set of sort s */
gen_e setif_zero(void);
gen_e setst_zero(void);
gen_e flowrow_zero(sort_kind base_sort);
gen_e term_zero(void);

/* Wild set of sort s */
gen_e term_wild(void);
gen_e setif_wild(void);

/* Universal set of sort s */
gen_e setif_one(void);
gen_e setst_one(void);
gen_e flowrow_one(sort_kind base_sort);
gen_e term_one(void);

/* Closed flowrow of base sort s */
gen_e flowrow_abs(sort_kind base_sort);

/* Wild flowrow of base sort s */
gen_e flowrow_wild(sort_kind base_sort);

/* Create a flowrow field */
flowrow_field flowrow_make_field(const char *name, gen_e e);

/* Build a flowrow of <l : e>_fields o <rest>  */
gen_e flowrow_make_row(flowrow_map fields, gen_e rest);

/* Constructor groups */
typedef struct cons_group_ *cons_group;

/* Create a constructor group */
cons_group make_cons_group(const char *name, sig_elt s[], int arity);

/* Add a constructor to a cons group. Must have the same signature as
   the group, and be a setif constructor */
void cons_group_add(cons_group g, constructor c);

/* Create a group projection pattern */
gen_e setif_group_proj_pat(cons_group g, int i, gen_e e);

/* Create a group constructor expression */
gen_e setif_group_cons_expr(cons_group g, gen_e *e, int arity);

/*****************************************************************************
 *                                                                           *
 *   Expression Introspection Operations                                     *
 *                                                                           *
 *****************************************************************************/

/* These are operations for examining expressions that have been
   built. These do not add constraints, so you can call them anytime
   without invalidating any previously computed solutions */

/* Print an expression to f */
void expr_print(FILE *f,gen_e e);

/* Check weak equality between two expressions. This is an internal
   equality notion, and you should probably not use it since its
   semantics depend on internal aspects of how the constraints are
   solved */
bool expr_eq(gen_e e1, gen_e e2);

/* Check if the expression is a constant (0-ary constructor) */
bool expr_is_constant(gen_e e);

/* Get the name of the constant e. Return NULL if e is not a constant */
char *expr_constant_name(gen_e e);

/* Get the internal id for e. Again, only use this if you know what
   you're doing.  */
int expr_stamp(gen_e e);

/* Ask what the sort of e is */
sort_kind expr_sort(gen_e e);

/*****************************************************************************
 *                                                                           *
 *   Adding Constraints Between Expressions                                  *
 *                                                                           *
 *****************************************************************************/

/* If you don't happen to know the sorts of the expressions (but you
   at least know they are the same sort) you can use these */
int call_sort_inclusion(gen_e e1, gen_e e2); 
int call_sort_unify(gen_e e1, gen_e e2);

/* Otherwise, call the inclusion/unification function of the
   appropriate sort */
int call_setif_inclusion(gen_e e1,gen_e e2);
int call_setif_unify(gen_e e1, gen_e e2);

int call_setst_inclusion(gen_e e1, gen_e e2);
int call_setst_unify(gen_e e1, gen_e e2);

int call_flowrow_inclusion(gen_e e1,gen_e e2);
int call_flowrow_unify(gen_e e1, gen_e e2);

int call_term_unify(gen_e e1, gen_e e2);
int call_term_cunify(gen_e e1, gen_e e2);

/*****************************************************************************
 *                                                                           *
 *   Operations for Inspecting Solutions of Constraints                      *
 *                                                                           *
 *****************************************************************************/

/* The decon structure represents the contents of c(e_0,...,e_n) for a
   constructor expression. */
struct decon		
{
  char *name;
  int arity;
  gen_e *elems;
};

/* Try to deconstruct expression e, assuming it looks like
   c(e_0,...,e_n). Returns a decon structure. If expression e doesn't
   match the constructor, or isn't a constructor expression (say, if
   it's a variable), return a NULL decon structure. */
struct decon deconstruct_expr(constructor c,gen_e e);

/* Deconstruct expression e, assuming it is some kind of constructor
   expression, but you don't know what constructor. Returns a decon
   structure. If expression e isn't a constructor expression (say, if
   it's a variable), return a NULL decon structure */
struct decon deconstruct_any_expr(gen_e e);

/* Compute the transitive lower bounds of e */
gen_e_list setif_tlb(gen_e e);
gen_e_list setst_tlb(gen_e e);

/* Compute the equivalence class representative of e */
gen_e term_get_ecr(gen_e e);

/* Look for a field matching the given label in the row. Return NULL
   if there isn't a matching field */
gen_e flowrow_extract_field(const char *label,gen_e row);

/* Extract a list of all the fields in the row */
flowrow_map flowrow_extract_fields(gen_e row);

/* Extract the end of the row */
gen_e flowrow_extract_rest(gen_e row);

/*****************************************************************************
 *                                                                           *
 *   Backtracking                                                            *
 *                                                                           *
 *****************************************************************************/

/* Read the global banshee clock */
int banshee_get_time(void);

/* Backtrack 1 constraint */
void banshee_rollback(void);

/* Backtrack to time t. If t is >= the current time, this has no effect */
void banshee_backtrack(int t);

/*****************************************************************************
 *                                                                           *
 *   Persistence                                                             *
 *                                                                           *
 *****************************************************************************/

/* Persistence */
void serialize_cs(FILE *f, hash_table *entry_points, unsigned long sz);
hash_table *deserialize_cs(FILE *f);

/* Region persistence */
void write_module_nonspec(FILE *f);
void update_module_nonspec(translation t, FILE *f);

/*****************************************************************************
 *                                                                           *
 *   Statistics                                                              *
 *                                                                           *
 *****************************************************************************/

/* Print out statistics for the constraint system */
void nonspec_stats(FILE *f);

EXTERN_C_END

#endif /* NONSPEC_H */
