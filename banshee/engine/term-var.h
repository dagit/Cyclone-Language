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

#ifndef TERM_VAR_H
#define TERM_VAR_H

#include "linkage.h"
#include "banshee.h"
#include "bounds.h"

EXTERN_C_BEGIN

typedef struct term_var_ *term_var;

DECLARE_LIST(term_var_list,term_var);

term_var tv_fresh(region r, const char *name);
term_var tv_fresh_small(region r, const char *name);
term_var tv_fresh_large(region r, const char *name);

char *tv_get_name(term_var v);

bounds tv_get_pending(term_var v);

/* Returns true if e was already in pending */
bool tv_add_pending(term_var v,gen_e e, stamp st);

bool tv_is_valid_pending(term_var v);
bool tv_invalidate_pending(term_var v);


void tv_unify(term_var v, gen_e e);
void tv_unify_vars(term_var v1, term_var v2);

gen_e tv_get_ecr(term_var v);

/* Persistence */
bool term_var_serialize(FILE *f, void *obj);
void *term_var_deserialize(FILE *f);
bool term_var_set_fields(void *obj);

EXTERN_C_END

#endif /* TERM_VAR_H */
