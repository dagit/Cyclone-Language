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

#include <stdio.h>
#include "regions.h"
#include "hash.h"
#include "regexp.h"

int main() {

  region_init();
  hash_table_init();
  {
  regexp r1 = regexp_empty(); 	/* r1 = empty */
  regexp r2 = regexp_append(r1, 'a'); /* r2 = a  */
  regexp r3 = regexp_append(r2, 'b'); /* r3 = ab */
  regexp r4 = regexp_concat(r2,r3);   /* r4 = aab */
  regexp r5 = regexp_concat(r4, r2);  /* r5 = aaba */
  regexp r6 = regexp_star(r5);	      /* r6 = (aaba)* */

  regexp_print_expr(r1);
  putchar('\n');
  regexp_print_expr(r2);
  putchar('\n');
  regexp_print_expr(r3);
  putchar('\n');
  regexp_print_expr(r4);
  putchar('\n');
  regexp_print_expr(r5);
  putchar('\n');
  regexp_print_expr(r6);
  putchar('\n');
  fflush(stdout);

  //regexp_print_necessary(r1);
  //regexp_print_necessary(r2);
  //regexp_print_necessary(r3);
  //regexp_print_necessary(r4);
  //regexp_print_necessary(r5);
  //regexp_print_necessary(r6);

  if (!regexp_inclusion(r1,r1)) {
    printf("Inclusion (r1,r1) is broken!\n");
    return 1;
  }
  if (regexp_inclusion(r1,r2)) {
    printf("Disinclusion (r1,r2) is broken!\n");
    return 1;
  }
  if (!regexp_inclusion(r5,r6)) {
    printf("Inclusion (r5,r6) is broken!\n");
    return 1;
  }
  if (regexp_inclusion(r6,r5)) {
    printf("Disinclusion (r6,r5) is broken!\n");
    return 1;
  }

  //  if (regexp_fast_disinclusion(r3, r2)) {
  // printf("Fast disinclusion check works\n");
  // }

  return 0;
  }
}

