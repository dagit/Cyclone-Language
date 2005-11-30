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

#include "banshee_persist_kinds.h"
#include "hash.h"

bool nonptr_data_serialize(FILE *f, void *obj);
bool funptr_data_serialize(FILE *f, void *obj);
bool string_data_serialize(FILE *f, void *obj);
bool uf_element_serialize(FILE *f, void *obj);
bool setif_var_serialize(FILE *f, void *obj);
bool sv_info_serialize(FILE *f, void *obj);
bool list_serialize(FILE *f, void *obj);
bool bounds_serialize(FILE *f, void *obj);
bool contour_serialize(FILE *f, void *obj);
bool gen_e_serialize(FILE *f, void *obj);
bool banshee_rollback_info_serialize(FILE *f, void *obj);
bool flow_var_serialize(FILE *f, void *obj);
bool flowrow_field_serialize(FILE *f, void *obj);
bool cons_group_serialize(FILE *f, void *obj);
bool setst_var_serialize(FILE *f, void *obj);
bool st_info_serialize(FILE *f, void *obj);
bool term_var_serialize(FILE *f, void *obj);
bool ustack_elt_serialize(FILE *f, void *obj);
bool hash_table_serialize(FILE *f, void *obj);
bool constructor_serialize(FILE *f, void *obj);
bool term_hash_serialize(FILE *f, void *obj);
bool added_edge_info_serialize(FILE *f, void *obj);
bool added_ub_proj_info_serialize(FILE *f, void *obj);

void *nonptr_data_deserialize(FILE *f);
void *funptr_data_deserialize(FILE *f);
void *string_data_deserialize(FILE *f);
void *uf_element_deserialize(FILE *f);
void *setif_var_deserialize(FILE *f);
void *sv_info_deserialize(FILE *f);
void *list_deserialize(FILE *f);
void *bounds_deserialize(FILE *f);
void *contour_deserialize(FILE *f);
void *gen_e_deserialize(FILE *f);
void *banshee_rollback_info_deserialize(FILE *f);
void *flow_var_deserialize(FILE *f);
void *flowrow_field_deserialize(FILE *f);
void *cons_group_deserialize(FILE *f);
void *setst_var_deserialize(FILE *f);
void *st_info_deserialize(FILE *f);
void *term_var_deserialize(FILE *f);
void *ustack_elt_deserialize(FILE *f);
void *hash_table_deserialize(FILE *f);
void *constructor_deserialize(FILE *f);
void *term_hash_deserialize(FILE *f);
void *added_edge_info_deserialize(FILE *f);
void *added_ub_proj_info_deserialize(FILE *f);

bool nonptr_data_set_fields(void *obj);
bool funptr_data_set_fields(void *obj);
bool string_data_set_fields(void *obj);
bool uf_element_set_fields(void *obj);
bool setif_var_set_fields(void *obj);
bool sv_info_set_fields(void *obj);
bool list_set_fields(void *obj);
bool bounds_set_fields(void *obj);
bool contour_set_fields(void *obj);
bool gen_e_set_fields(void *obj);
bool banshee_rollback_info_set_fields(void *obj);
bool flow_var_set_fields(void *obj);
bool flowrow_field_set_fields(void *obj);
bool cons_group_set_fields(void *obj);
bool setst_var_set_fields(void *obj);
bool st_info_set_fields(void *obj);
bool term_var_set_fields(void *obj);
bool ustack_elt_set_fields(void *obj);
bool hash_table_set_fields(void *obj);
bool constructor_set_fields(void *obj);
bool term_hash_set_fields(void *obj);
bool added_edge_info_set_fields(void *obj);
bool added_ub_proj_info_set_fields(void *obj);

/* TODO -- this is a temporary hack to get spec. code to compile */
#ifndef NONSPEC
bool gen_e_serialize(FILE *f, void *obj)
{
  return FALSE;
}

void *gen_e_deserialize(FILE *f)
{
  return NULL;
}

bool gen_e_set_fields(void *obj)
{
  return TRUE;
}

void serialize_cs(const char *filename, hash_table *entry_points, unsigned long sz)
{
  return;
}
#endif	/* NONSPEC */


static serialize_fn_ptr serialize_fns[NUM_PERSIST_KINDS] = {
  nonptr_data_serialize,
  funptr_data_serialize,
  string_data_serialize,
  uf_element_serialize,
  setif_var_serialize,
  sv_info_serialize,
  list_serialize,
  bounds_serialize,
  contour_serialize,
  banshee_rollback_info_serialize,
  flow_var_serialize,
  flowrow_field_serialize,
  setst_var_serialize,
  st_info_serialize,
  term_var_serialize,
  ustack_elt_serialize,
  hash_table_serialize,
  term_hash_serialize,
  gen_e_serialize,
  added_edge_info_serialize,
  added_ub_proj_info_serialize,
#ifdef NONSPEC
  cons_group_serialize,
  constructor_serialize,
#endif
};

static deserialize_fn_ptr deserialize_fns[NUM_PERSIST_KINDS] = {
  nonptr_data_deserialize,
  funptr_data_deserialize,
  string_data_deserialize,
  uf_element_deserialize,
  setif_var_deserialize,
  sv_info_deserialize,
  list_deserialize,
  bounds_deserialize,
  contour_deserialize,
  banshee_rollback_info_deserialize,
  flow_var_deserialize,
  flowrow_field_deserialize,
  setst_var_deserialize,
  st_info_deserialize,
  term_var_deserialize,
  ustack_elt_deserialize,
  hash_table_deserialize,
  term_hash_deserialize,
  gen_e_deserialize,
  added_edge_info_deserialize,
  added_ub_proj_info_deserialize,
#ifdef NONSPEC
  cons_group_deserialize,
  constructor_deserialize,
#endif
};


static set_fields_fn_ptr set_fields_fns[NUM_PERSIST_KINDS] = {
  nonptr_data_set_fields,
  funptr_data_set_fields,
  string_data_set_fields,
  uf_element_set_fields,
  setif_var_set_fields,
  sv_info_set_fields,
  list_set_fields,
  bounds_set_fields,
  contour_set_fields,
  banshee_rollback_info_set_fields,
  flow_var_set_fields,
  flowrow_field_set_fields,
  setst_var_set_fields,
  st_info_set_fields,
  term_var_set_fields,
  ustack_elt_set_fields,
  hash_table_set_fields,
  term_hash_set_fields,
  gen_e_set_fields,
  added_edge_info_set_fields,
  added_ub_proj_info_set_fields,
#ifdef NONSPEC
  cons_group_set_fields,
  constructor_set_fields,
#endif
};

void banshee_serialize_start(FILE *f)
{
  serialize_start(f, serialize_fns, NUM_PERSIST_KINDS);
}

void banshee_serialize_end()
{
  serialize_end();
}

void banshee_deserialize_all(FILE *f)
{
  deserialize_all(f, deserialize_fns, set_fields_fns, NUM_PERSIST_KINDS);
}

void banshee_deserialize_end(void)
{
  deserialize_end();
}
