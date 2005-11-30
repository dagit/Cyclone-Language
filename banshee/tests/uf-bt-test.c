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

/* Test the union find backtracking functionality */
#include <stdio.h>
#include <assert.h>
#include "ufind.h"
#include "list.h"

DECLARE_UFIND(elt,int);
DEFINE_NONPTR_UFIND(elt,int);

int main()
{
  elt e1,e2,e3,e4;
  region scratchregion;
  region_init();
  list_init();
  uf_init();

  scratchregion = newregion();

  e1 = new_elt(scratchregion,1);
  e2 = new_elt(scratchregion,2);
  e3 = new_elt(scratchregion,3);
  e4 = new_elt(scratchregion,4);

  elt_union(e1,e2);		// | e1 e2 | e3 | e4 | 
  //  printf("(%d,%d)\n",elt_get_info(e1),elt_get_info(e2));
  assert(elt_get_info(e2) == elt_get_info(e1));
  uf_backtrack(); 		// | e1 | e2 | e3 | e4 | 
  //  printf("(%d,%d)\n",elt_get_info(e1),elt_get_info(e2));
  assert(elt_get_info(e1) == 1);
  assert(elt_get_info(e2) == 2);
  elt_union(e1,e2);		// | e1 e2 | e3 | e4 | 
  elt_union(e2,e3);		// | e1 e2 e3 | e4 | 
  elt_union(e3,e4);		// | e1 e2 e3 e4 | 

  assert(elt_get_info(e1) == elt_get_info(e2));
  assert(elt_get_info(e2) == elt_get_info(e3));
  assert(elt_get_info(e1) == elt_get_info(e4));
    
  uf_backtrack();		// | e1 e2 e3 | e4 | 
  assert(elt_get_info(e4) == 4);
//   printf("(%d,%d,%d,%d)",elt_get_info(e1),elt_get_info(e2),elt_get_info(e3),
// 	 elt_get_info(e4));
  assert(elt_get_info(e1) == elt_get_info(e3));
  uf_backtrack();		// | e1 e2 | e3 | e4 |
  assert(elt_get_info(e1) == elt_get_info(e2));
  assert(elt_get_info(e3) == 3);
  assert(elt_get_info(e4) == 4);
  uf_backtrack();		// | e1 | e2 | e3 | e4 | 
  assert(elt_get_info(e1) == 1);
  assert(elt_get_info(e2) == 2);
  assert(elt_get_info(e3) == 3);
  assert(elt_get_info(e4) == 4);

  elt_update(e4,5);
  
  assert(elt_get_info(e4) == 5);
  uf_backtrack();
  assert(elt_get_info(e4) == 4);

  return 0;
}
