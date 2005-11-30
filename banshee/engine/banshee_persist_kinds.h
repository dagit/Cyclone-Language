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

/* Defines a kind for each persistent object type used in Banshee, as
   well as pointers to the functions needed to
   serialize/deserialize/restore pointer-valued fields for each object
   type
*/
#ifndef BANSHEE_PERSIST_KINDS_H
#define BANSHEE_PERSIST_KINDS_H

#include "linkage.h"
#include "persist.h"

EXTERN_C_BEGIN

/* This should be kept in sync with the enum below it */
#ifdef NONSPEC
#define NUM_PERSIST_KINDS 23
#else
#define NUM_PERSIST_KINDS 21
#endif 
/* Take each type that should support serialization and prefix it with
   BANSHEE_PERSIST_KIND. Then serialize_banshee_object(object, type)
   will take care of choosing the right kind.
*/
typedef enum banshee_persist_kind_ {
  BANSHEE_PERSIST_KIND_nonptr = 0,  /* for non-pointer data: KEEP THIS 0 */
  BANSHEE_PERSIST_KIND_funptr = 1,
  BANSHEE_PERSIST_KIND_string = 2,
  BANSHEE_PERSIST_KIND_uf_element, /* DONE */
  BANSHEE_PERSIST_KIND_setif_var,  /* DONE */
  BANSHEE_PERSIST_KIND_sv_info,	   /* DONE */
  BANSHEE_PERSIST_KIND_list,	   /* DONE */
  BANSHEE_PERSIST_KIND_bounds,	   /* DONE */
  BANSHEE_PERSIST_KIND_contour,	
  BANSHEE_PERSIST_KIND_banshee_rollback_info,
  BANSHEE_PERSIST_KIND_flow_var,   /* DONE */
  BANSHEE_PERSIST_KIND_flowrow_field,
  BANSHEE_PERSIST_KIND_setst_var,
  BANSHEE_PERSIST_KIND_st_info,
  BANSHEE_PERSIST_KIND_term_var, /* DONE */
  BANSHEE_PERSIST_KIND_ustack_elt, /* DONE */
  BANSHEE_PERSIST_KIND_hash_table, /* DONE */
  BANSHEE_PERSIST_KIND_term_hash,  /* DONE */
  BANSHEE_PERSIST_KIND_gen_e,
  BANSHEE_PERSIST_KIND_added_edge_info, /* DONE */
  BANSHEE_PERSIST_KIND_added_ub_proj_info,
#ifdef NONSPEC
  BANSHEE_PERSIST_KIND_cons_group,	
  BANSHEE_PERSIST_KIND_constructor,
#endif
} banshee_persist_kind;

/* Serialization */
void banshee_serialize_start(FILE *f);
void banshee_serialize_end();

/* Deserialization */
void banshee_deserialize_all(FILE *f);
void banshee_deserialize_end();

/* TODO -- for consistency, rename to banshee_serialize_object */
#define serialize_banshee_object(object, type)			\
  serialize_object(object, BANSHEE_PERSIST_KIND_ ## type)

EXTERN_C_END

#endif /* BANSHEE_PERSIST_KINDS_H */
