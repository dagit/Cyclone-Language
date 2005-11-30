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
#ifndef UFIND_H
#define UFIND_H

#include "linkage.h"
#include "regions.h"
#include "bool.h"
#include "banshee_persist_kinds.h"

/* Type safe, generic union-find data structure with support for
 * backtracking. Use DECLARE_UFIND and DEFINE_UFIND macros to get type
 * safety. DO NOT USE the untyped defs. This module *must* be
 * initialized prior to use via a call to uf_init()  */


EXTERN_C_BEGIN

// struct uf_element_;

typedef struct uf_element_ *uf_element;
typedef void *uf_info;

/* Given two infos, return a representative for them. The infos are
 * const so that the original information for the separate equivalence
 * classes can be saved and restored in the event of a rollback */
typedef uf_info (*combine_fn_ptr)(const uf_info, const uf_info);

/* Initialize the union find module. Must be called before any other
 * call to uf_ */
void uf_init();

/* Reset the union find module. Invalidates any terms created before
   the reset, and calls uf_init() */
void uf_reset();

/* Create a new union find element with info i in region r  */
uf_element new_uf_element(region r,uf_info i, int persist_kind);  

/* Return the info associated with the element's equivalence class */
uf_info uf_get_info(uf_element ); 

/* Put two elements in the same equivalence class. The information for
 * the equivalence class is determined by the combine_fn_ptr */
bool uf_unify(combine_fn_ptr, uf_element , uf_element );

/* Put two elements in the same equivalence class. The information for
 * the equivalence class is picked from one of the two elements */
bool uf_union(uf_element ,uf_element );

/* Check whether two elements are in the same equivalence class  */
bool uf_eq(uf_element , uf_element );

/* Update the information associated with the element's equivalence
 * class */
void uf_update(uf_element ,uf_info i);

/* Put a mark on the history for rollback */
void uf_tick();		

/* Undo just the last union/unify/update/tick */
void uf_backtrack();		

/* Backtrack to the last tick */
void uf_rollback(); 		

/* Persistence */
void uf_serialize(FILE *f);
void uf_deserialize(FILE *f);
void uf_set_fields(void);

/* Region persistence */
void write_module_uf(FILE *f);
void update_module_uf(translation t, FILE *f);

#define DECLARE_UFIND(name,type) \
typedef struct name *name; \
typedef type (* name ## _combine_fn_ptr)(const type info1, const type info2); \
name new_ ## name(region r, type info); \
type name ## _get_info(name); \
bool name ## _unify(name ## _combine_fn_ptr,name e1, name e2); \
bool name ## _union(name e1, name e2); \
bool name ## _eq(name e1, name e2); \
void name ## _update(name e1, type info);

#define DEFINE_NONPTR_UFIND(name,type) \
name new_ ## name(region r, type info) \
{\
  return (name)new_uf_element(r,(void *)info, 0);\
}\
type name ## _get_info(name elem) \
{\
 return (type)uf_get_info((uf_element )elem);\
}\
bool name ## _unify(name ## _combine_fn_ptr cmb,name e1, name e2) \
{\
 return uf_unify((combine_fn_ptr)cmb,(uf_element )e1,(uf_element )e2); \
}\
bool name ## _union(name e1, name e2) \
{\
 return uf_union((uf_element )e1,(uf_element )e2); \
}\
bool name ## _eq(name e1, name e2) \
{\
 return uf_eq((uf_element )e1,(uf_element )e2); \
}\
void name ##_update(name e1, type info) \
{\
 return uf_update((uf_element )e1,(uf_info)info); \
}\

#define DEFINE_UFIND(name,type) \
name new_ ## name(region r, type info) \
{\
  return (name)new_uf_element(r,(void *)info, BANSHEE_PERSIST_KIND_ ## type);\
}\
type name ## _get_info(name elem) \
{\
 return (type)uf_get_info((uf_element )elem);\
}\
bool name ## _unify(name ## _combine_fn_ptr cmb,name e1, name e2) \
{\
 return uf_unify((combine_fn_ptr)cmb,(uf_element )e1,(uf_element )e2); \
}\
bool name ## _union(name e1, name e2) \
{\
 return uf_union((uf_element )e1,(uf_element )e2); \
}\
bool name ## _eq(name e1, name e2) \
{\
 return uf_eq((uf_element )e1,(uf_element )e2); \
}\
void name ##_update(name e1, type info) \
{\
 return uf_update((uf_element )e1,(uf_info)info); \
}\

EXTERN_C_END

#endif /* UFIND_H */






















































































