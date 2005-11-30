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

#include <stdio.h>
#include "regions.h"
#include "banshee.h"
#include "setif-sort.h"
#include "setst-sort.h"
#include "flowrow-sort.h"
#include "setif-var.h"
#include "term-sort.h"
#include "ufind.h"
#include "list.h"
#include "termhash.h"
#include "banshee_region_persist_kinds.h"

DECLARE_LIST(banshee_rollback_stack, banshee_rollback_info);
DEFINE_LIST(banshee_rollback_stack, banshee_rollback_info);
DEFINE_LIST(gen_e_list ,gen_e);

EXTERN_C void seed_fn_ptr_table(region r);
static int banshee_clock = 0;
static banshee_rollback_stack rb_stack;
static region engine_region;
banshee_error_handler_fn handle_error = NULL;

static void default_error_handler(gen_e e1, gen_e e2, banshee_error_kind k)
{
  switch (k) {
  case bek_cons_mismatch:
    fprintf(stderr, "Unhandled banshee error: constructor mismatch\n");
    break;
  case bek_occurs_check:
    fprintf(stderr, "Unhandled banshee error: occurs check\n");
    break;
  default:
    fprintf(stderr,"Unhandled banshee error: unknown code %d\n",k);
  }
}

void engine_init(void)
{
  region_init();

  engine_region = newregion();

  hash_table_init();
  seed_fn_ptr_table(permanent);

  banshee_region_persistence_init();
  bounds_init();
  stamp_init();
  list_init();
  uf_init();
  term_hash_init();

  rb_stack = new_persistent_banshee_rollback_stack();
  handle_error = default_error_handler;

}

void engine_reset(void) deletes
{
  stamp_reset();
  // TODO: figure out what to do about this: list_reset();
  uf_reset();
  bounds_reset();
  /* TODO -- need to rename this because the name hash_table_reset is
     taken  */
/*   hash_table_reset(); */
  term_hash_reset();

  banshee_clock = 0;
  deleteregion(engine_region);
  engine_region = newregion();
  rb_stack = new_persistent_banshee_rollback_stack();
}

void engine_stats(FILE *f)
{
  setif_print_stats(f);
  setst_print_stats(f);
  flowrow_print_stats(f);
}

void print_constraint_graphs(FILE *f)
{
  setif_print_constraint_graph(f);
}

int banshee_get_time(void)
{
  return banshee_clock;
}

void banshee_set_time(banshee_rollback_info info)
{
  info->time = banshee_clock;
}

void banshee_clock_tick()
{
  banshee_clock++;
  uf_tick();
}

/* Return TRUE if there is already a rollback entry for this sort at
   the current time */
bool banshee_check_rollback(sort_kind k)
{
  banshee_rollback_stack_scanner scan;
  banshee_rollback_info info;

  banshee_rollback_stack_scan(rb_stack,&scan);

  while(banshee_rollback_stack_next(&scan,&info)) {
    if (info->time != banshee_clock) return FALSE;
    else if (info->kind == k) return TRUE;
  }
  return FALSE;
}

void banshee_register_rollback(banshee_rollback_info info)
{
  /* This rollback must not be present already */
  assert(!banshee_check_rollback(info->kind));
  
  banshee_rollback_stack_cons(info, rb_stack);
}

static void banshee_rollback_dispatch(banshee_rollback_info info) {
  switch(info->kind) {
  case flowrow_sort:
    flowrow_rollback(info);
    break;
  case setif_sort:
    setif_rollback(info);
    break;
  case setst_sort:
    setst_rollback(info);
    break;
  case term_sort:
    term_rollback(info);
    break;
  default:
    fail("Unknown sort in banshee_rollback_dispatch.\n");
  }
}

void banshee_rollback()
{
  banshee_rollback_info info;

  if (banshee_clock > 0) {

    uf_rollback();

    while(1) {
      if (banshee_rollback_stack_empty(rb_stack)) break;
      info = banshee_rollback_stack_head(rb_stack);
      if (info->time < banshee_clock) break;
      banshee_rollback_dispatch(info);
      rb_stack = banshee_rollback_stack_tail(rb_stack);
    }
    banshee_clock--;
  }
}

void banshee_backtrack(int t)
{
  if (t < 0) return;
  while(banshee_clock > t)
    banshee_rollback();  
}

/* Persistence */
static bool banshee_rollback_serialize_dispatch(FILE *f, 
						banshee_rollback_info info)
{
  switch(info->kind) {
  case flowrow_sort:
    return flowrow_rollback_serialize(f, info);
  case setif_sort:
    return setif_rollback_serialize(f, info);
  case setst_sort:
    return setst_rollback_serialize(f, info);
  case term_sort:
    return term_rollback_serialize(f, info);
  default:
    fail("Unknown sort in banshee_rollback_serialize_dispatch.\n");
    return FALSE;
  }
}

static banshee_rollback_info banshee_rollback_deserialize_dispatch(sort_kind k,
								   FILE *f)
{
  switch(k) {
  case flowrow_sort:
    return flowrow_rollback_deserialize(f);
  case setif_sort:
    return setif_rollback_deserialize(f);
  case setst_sort:
    return setst_rollback_deserialize(f);
  case term_sort:
    return term_rollback_deserialize(f);
  default:
    fail("Unknown sort in banshee_rollback_deserialize_dispatch.\n");
    return NULL;
  }
}

static bool banshee_rollback_set_fields_dispatch(banshee_rollback_info info)
{
  switch(info->kind) {
  case flowrow_sort:
    return flowrow_rollback_set_fields(info);
  case setif_sort:
    return setif_rollback_set_fields(info);
  case setst_sort:
    return setst_rollback_set_fields(info);
  case term_sort:
    return term_rollback_set_fields(info);
  default:
    fail("Unknown sort in banshee_rollback_set_fields_dispatch.\n");
    return FALSE;
  }
}

bool banshee_rollback_info_serialize(FILE *f, void *obj)
{
  banshee_rollback_info info = (banshee_rollback_info)obj;
  assert(f);

  fwrite((void *)&info->time, sizeof(int), 1, f);
  fwrite((void *)&info->kind, sizeof(int), 1, f);
  return banshee_rollback_serialize_dispatch(f, info);
}

void *banshee_rollback_info_deserialize(FILE *f)
{
  int time;
  sort_kind kind;
  banshee_rollback_info result = NULL;
  assert(f);

  fread((void *)&time, sizeof(int), 1, f);
  fread((void *)&kind, sizeof(int), 1, f);

  result = banshee_rollback_deserialize_dispatch(kind, f);
			      
  result->time = time;
  result->kind = kind;
  return result;
}

bool banshee_rollback_info_set_fields(void *obj)
{
  assert(obj);

  return banshee_rollback_set_fields_dispatch((banshee_rollback_info)obj);
}

void engine_serialize(FILE *f)
{
  assert(f);
  fwrite((void *)&banshee_clock, sizeof(int), 1, f);
  fwrite((void *)&rb_stack, sizeof(banshee_rollback_stack), 1, f);
  
  serialize_banshee_object(rb_stack, list);
}

void engine_deserialize(FILE *f)
{
  assert(f);

  fread((void *)&banshee_clock, sizeof(int), 1, f);
  fread((void *)&rb_stack, sizeof(banshee_rollback_stack), 1, f);
}

void engine_set_fields(void)
{
  deserialize_set_obj((void **)&rb_stack);
}

void update_module_engine(translation t, FILE *f)
{
  fread((void *)&banshee_clock, sizeof(int), 1, f);
  fread((void *)&rb_stack, sizeof(void *), 1, f);
  update_pointer(t, (void **)&rb_stack);
}

void write_module_engine(FILE *f)
{
  fwrite((void *)&banshee_clock, sizeof(int), 1, f);
  fwrite((void *)&rb_stack, sizeof(void *), 1, f);
}
