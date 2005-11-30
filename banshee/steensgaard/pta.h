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
#ifndef PTA_H
#define PTA_H

#include "steensgaard_terms.h"
#include "banshee_persist_kinds.h"

typedef struct contents_type_ *contents_type;

DECLARE_LIST(contents_type_list, contents_type);

void pta_init(void);

void pta_reset(void);

T pta_make_ref(const char *id);

T pta_T_to_T(T);

T pta_bottom(void);

T pta_join(T,T);

T pta_deref(T);

T pta_rvalue(T);

T pta_address(T);

T pta_ref_address(T, T);

void pta_assignment(T,T);

T pta_make_fun(const char *name, T ret, T_list args);

T pta_application(T, T_list);

contents_type pta_get_contents(T);

void pta_pr_ptset(contents_type);

int pta_get_ptsize(contents_type);

void pta_serialize(FILE *f, hash_table *entry_points, unsigned long sz);

hash_table *pta_deserialize(FILE *f);

void pta_region_serialize(FILE *f);

void pta_region_deserialize(translation t, FILE *f);


#endif
