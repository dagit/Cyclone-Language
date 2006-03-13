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
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "bool.h"
#include "bfix.h" 		/* bit manipulations */
#include "hash.h"		/* hash tables */
#include "utils.h"

typedef unsigned char letter;

/* Kleene star is represented as a marker STAR followed by a pointer
   indicating how many characters forward to jump */
static char STAR = 1;

struct regexp_ {
  unsigned char length; 	/* doesn't include the null terminator */
  letter *expr;			/* a null terminated string */
  unsigned char necessary[32]; 
};

typedef struct regexp_ * regexp;

static regexp regexp_calloc() {
  return (regexp) calloc (1, sizeof(struct regexp_)); 
}

//regexp regexp_alloc() {
//  return (regexp) malloc (sizeof(struct regexp_)); 
//}

static letter* regexp_alloc_str(unsigned char length) {
  return (unsigned char *) malloc (sizeof(letter) * length);
}

/* Make an empty regular expression */
regexp regexp_empty() {
  regexp result = regexp_calloc();
  result->length = 0;
  result->expr = regexp_alloc_str(1);
  result->expr[0] = 0;
  return result;
}

/* Make a copy of r, appending l to the end */
regexp regexp_append(regexp r, letter let) {
  regexp result = regexp_calloc();
  result->length = r->length + 1;
  result->expr = regexp_alloc_str(result->length + 1);
  strncpy((char *)result->expr, (char *)r->expr, r->length);
  result->expr[result->length-1] = let;
  result->expr[result->length] = 0;

  /* Copy the necessary letters bit vector */
  memcpy(result->necessary, r->necessary, sizeof(unsigned char) * 32);
  /* Set the bit at let's position  */
  bfi(result->necessary, let + 1, 1, 1);
  return result;
}

regexp regexp_concat(regexp r1, regexp r2) {
  int i;
  regexp result = regexp_calloc();
  result->length = r1->length + r2->length;
  result->expr = regexp_alloc_str(result->length + 1);
  strncpy((char *)result->expr, (char *)r1->expr, r1->length);
  strncpy((char *)&result->expr[r1->length], (char *)r2->expr, r2->length);
  result->expr[result->length] = 0;
  /* Set the necessary bitvector to be the OR of r1->necessary and
     r2->necessary */
  for (i = 0; i < 32; i++) {
    result->necessary[i] = r1->necessary[i] | r2->necessary[i];
  }

  return result;
}

/* Make a copy of r, transforming it into r* */
regexp regexp_star(regexp r) {
  regexp result = regexp_calloc();
  result->length = r->length + 2;
  result->expr = regexp_alloc_str(result->length + 2);
  strncpy((char *)&result->expr[2], (char *)r->expr, r->length);
  result->expr[0] = STAR;
  result->expr[1] = result->length + 1;
  result->expr[result->length] = 0;


  /* Copy the necessary letters bit vector */
  memcpy(result->necessary, r->necessary, sizeof(unsigned char) * 32);

  return result;
}

/* Return 1 if r1 is definitely not a subset of r2. Otherwise, return
   -1, indicating that r1 may or may not be a subset of r2 */
int regexp_fast_disinclusion(regexp r1, regexp r2) {
  if (memcmp(r1->necessary, r2->necessary, sizeof(unsigned char) * 32) > 0) {
    return 1;
  }
  return -1;  
}

/* Recursive complete inclusion check, using backtracking */
bool regexp_complete_inclusion(regexp r1, regexp r2, unsigned char pos1,
			      unsigned char pos2, hash_table visited) {

  unsigned int state = (pos1 << (8 * sizeof(unsigned char))) + pos2;

  /* case 0: empty/empty */
  if (!r1->expr[pos1] && !r2->expr[pos2]) {
    return TRUE;
  }

  /* check if this state has been visited. if so, return true. If not,
     insert */
  if (!hash_table_insert(visited, (hash_key)state, 0)) {
    return TRUE;
  }

  /* case 1: concat/concat */
  /* make the transition in both r1 and r2, or this branch fails */
  if (r1->expr[pos1] != STAR && r2->expr[pos2] != STAR) {
    if (r1->expr[pos1] == r2->expr[pos2]) {
      return regexp_complete_inclusion(r1, r2, pos1 + 1, pos2 + 1, visited);
    }
    else {
      return FALSE;
    }
  }

  /* case 2: concat/star */
  /*  in r2 try both transitions. one of the branches must succeed */
  if (r1->expr[pos1] != STAR && r2->expr[pos2] == STAR) {
    return 
      regexp_complete_inclusion(r1, r2, pos1, pos2 + r2->expr[pos2 + 1], 
				visited) 
      ||
      regexp_complete_inclusion(r1, r2, pos1, pos2 + 2, visited);
  }

  /* case 3: star/_ */
  /* transition in r1 both ways, both must succeed */
  if (r1->expr[pos1] == STAR) {
    return 
      regexp_complete_inclusion(r1, r2, pos1 + r1->expr[pos1 + 1] , pos2, 
				visited) 
      &&
      regexp_complete_inclusion(r1, r2, pos1 + 2, pos2, visited);
  }

  else {
    fail("Error: unknown case in regexp_complete_inclusion: %c, %c",
	 r1->expr[pos1], r2->expr[pos2]);
  }
}

//static unsigned long state_hash(void *ptr)
//{
//  return (unsigned long) ptr;
//}

// see stamp.c 
static unsigned long state_hash(hash_key key)
{
  unsigned long keyval = (unsigned long)key;
  keyval += (keyval << 12);
  keyval ^= (keyval >> 22);
  keyval += (keyval << 4);
  keyval ^= (keyval >> 9);
  keyval += (keyval << 10);
  keyval ^= (keyval >> 2);
  keyval += (keyval << 7);
  keyval ^= (keyval >> 12);
  return keyval;
}


static bool state_eq(hash_key state1, hash_key state2)
{
  return state1 == state2;
}

/* Complete inclusion check. First fast checks to eliminate simple
   cases. Eventually defaults to a complete check */
bool regexp_inclusion(regexp r1, regexp r2) {
  /* a hashtable of visited states (positions in the regexps) */
  //  unsigned char visited[8192];

  /* Fast equality check */
  if (!strcmp((char *)r1->expr,(char *)r2->expr)) {
    return TRUE;
  }		

  /* Fast disinclusion check */
  else if (regexp_fast_disinclusion(r1, r2) == 1) {
    return FALSE;
  }
 
  /* Complete inclusion check */
  else {
    bool result = FALSE;
    region temp = newregion();
    hash_table visited = make_hash_table(temp, 32, state_hash, state_eq);
    /* TODO: I'd prefer to use a worklist instead of recursion */
    result = regexp_complete_inclusion(r1, r2, 0, 0, visited);
    deleteregion(temp);
    return result;
  }

}

/* Debugging routines */

static void regexp_print_subexpr(regexp r, unsigned char start, 
				 unsigned char end) {
  while (start < end && r->expr[start] != STAR) {
    putchar(r->expr[start]);
    start++;
  }

  if (r->expr[start] == STAR) {
    putchar('(');
    regexp_print_subexpr(r, start + 2, start + r->expr[start + 1]);
    putchar(')');
    putchar('*');
    regexp_print_subexpr(r, start + r->expr[start+1] + 1, end);
  }
}

/* Print out the expression itself */
void regexp_print_expr(regexp r) {
  regexp_print_subexpr(r, 0, r->length);
  fflush(stdout);
}

/* Print out all the necessary letters in r */
void regexp_print_necessary(regexp r) {
  int i;
  printf("Necessary characters in %s:\n", r->expr);
  for (i = 0; i < 256; i++) {
    if (bfx(r->necessary, i+1, 1)) {
      printf("%c (%d) \n",i, i);
    }
  }
}
