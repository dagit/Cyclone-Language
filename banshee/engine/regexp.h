/*
 * Copyright (c) 2005
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

/* Regular expressions. Regexps are immutable--- each operation
   creates a new regular expression. */
#ifndef REGEXP_H
#define REGEXP_H

#include "bool.h"

/* This representation uses unsigned char for letters. 0 and 1 are
   reserved, so there are effectively 254 available letters  */
typedef unsigned char letter;

typedef struct regexp_ *regexp;

/* Basic operations */

/* Construct an empty regular expression */
regexp regexp_empty(void);

/* Given r, return r* */
regexp regexp_star(regexp r);

/* Given r and letter l, return r * l */
regexp regexp_append(regexp r, letter l);

/* Given r1 and r2, return r1 * r2 */
regexp regexp_concat(regexp r1, regexp r2);

/* Return TRUE if r1 <= r2 (every string in r1 is in r2) */
bool regexp_inclusion(regexp r1, regexp r2); 

/* Debugging */

/* print the necessary characters in r */
void regexp_print_necessary(regexp r);

/* pretty print the regular expression */
void regexp_print_expr(regexp r);

#endif	/* REGEXP_H */
