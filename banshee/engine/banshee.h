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

#ifndef BANSHEE_H
#define BANSHEE_H

#include <stdio.h>
#include "linkage.h"
#include "stamp.h"
#include "list.h"
#include "utils.h"

#define ALIAS_TYPE -2
#define VAR_TYPE -1
#define ZERO_TYPE 0
#define ONE_TYPE 1
#define UNION_TYPE 2
#define INTER_TYPE 3
#define CONSTANT_TYPE 4

#define ABS_TYPE 5
#define WILD_TYPE 6
#define ROW_TYPE 7

/* This must be an even number larger than any of the types defined above */
#define LARGEST_BUILTIN_TYPE 10

EXTERN_C_BEGIN

typedef enum sort_kind
{
  flowrow_sort, 
  setif_sort,
  setst_sort,
  term_sort
} sort_kind;

#ifdef NONSPEC
typedef struct gen_e_
{
  sort_kind sort;
} *gen_e;

#else
typedef struct gen_e_ *gen_e;
#endif

DECLARE_LIST(gen_e_list,gen_e);

// Keep these in sync with nonspec.h and the code generator
typedef enum banshee_error_kind
{
  bek_cons_mismatch, 	        // c(...) <= d(...)
  bek_occurs_check,		// occurs check failed (cyclic unification)
} banshee_error_kind;

/* type for error callbacks */
typedef void (*banshee_error_handler_fn) 
     (gen_e e1, gen_e e2, banshee_error_kind kind);

extern banshee_error_handler_fn handle_error;

/* Generic rollback info structure */
typedef struct banshee_rollback_info_ {
  int time;
  sort_kind kind;
} * banshee_rollback_info;

/* Read the global banshee clock */
int banshee_get_time(void);

/* Backtrack 1 constraint */
void banshee_rollback(void);

/* Backtrack to time t. If t is >= the current time, this has no effect */
void banshee_backtrack(int t);

/* Set a rollback info structure's time */
void banshee_set_time(banshee_rollback_info);

/* Advance the banshee clock */
void banshee_clock_tick(void);

/* Check if a rollback info structure of kind k exists for the current time */
bool banshee_check_rollback(sort_kind k);

/* Register a rollback info structure */
void banshee_register_rollback(banshee_rollback_info);

/* 
   Function pointers that are common to all sorts
*/

/* inclusion */
typedef void (*incl_fn_ptr) (gen_e, gen_e) deletes;   

/* match constructed terms */  
typedef void (*con_match_fn_ptr) (gen_e, gen_e) deletes;

/* print a term */
typedef void (*gen_e_pr_fn_ptr) (FILE *, gen_e);

/* make fresh variables */
typedef gen_e (*fresh_fn_ptr) (const char *);
typedef gen_e (*fresh_small_fn_ptr) (const char *);
typedef gen_e (*fresh_large_fn_ptr) (const char *);     

/* get a stamp */
typedef stamp (*get_stamp_fn_ptr) (gen_e);

/* extract a term from a proj pat */
typedef gen_e (*get_proj_fn_ptr) (gen_e_list);

void engine_init(void);
void engine_reset(void) deletes;
void engine_stats(FILE *f);

/* Persistence */
void engine_serialize(FILE *f);
void engine_deserialize(FILE *f);
void engine_set_fields(void);

/* Region persistence */
void write_module_engine(FILE *f);
void update_module_engine(translation t, FILE *f);

void print_constraint_graphs(FILE *f);

EXTERN_C_END

#endif /* BANSHEE_H */
