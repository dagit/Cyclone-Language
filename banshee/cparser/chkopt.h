/* This file is part of the RC compiler.
   Copyright (C) 2000-2001 The Regents of the University of California.

RC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

RC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with RC; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

#ifndef CHKOPT_H
#define CHKOPT_H

/* This code uses two notions:
   A "place" is an abstraction of regions. A value v has place p:
   - p = 'local variable a':
       if a = null, then v = null
       if a is in region r, then v is in region r
   - p = 'PGLOBAL': v is in the global memory area (malloc, stack, global data)

   A "generalised place" extends places with the following values:
   - p = 'SRas local variable a':
       if a = null, then v = null
       if a is in region r, then v is null or in region r
   - p = 'SRas PGLOBAL': v is null or in the global memory area
   - p = '<=local variable a': regionof v <= regionof a
   - p = '<=PGLOBAL': regionof v <= global region
   - p = 'PNULL': v is null
   - p = 'PUNKNOWN': v is null, in the global memory area, or in any region
   Generalised places can be viewed as a restricted representation of sets formed
   from PNULL, PGLOBAL, and local vars.

   The encoding of places is as an integer with PGLOBAL=0, a positive value
   otherwise (the ids are compacted to a small range before optimisation begins
   to allow the use of bitsets to hold the constraint relation)

   The encoding of generalised places is also an integer with
   PNULL = -1, PUNKNOWN = -2, PGLOBAL = 0, <=PGLOBAL = 1, a=2*place id of a,
   <=a=2*place id of a + 1
   (thus for positive numbers, odd indicates <=, the place id is found by
   dividing by 2)
*/

#define PGLOBAL 0
#define PNULL -1
#define PUNKNOWN -2
#define PERROR -3 /* should never be seen */

typedef int place;
typedef int gplace; /* generalised place */

typedef struct sr_state
{
  bitset sr;
  bitset lt;
  bitset notnull;
  bitset null;
} *sr_state;

extern struct sr_state zero_state;

void chkopt_libfn_init(void);

void optimise_rc_checks(region r, function_decl fd, bitset locals) deletes;
void optimise_rc_checks_perfile(declaration the_program);
void output_chkopt_debug(binary e);
bool known_traditional_write(binary rcop);
bool known_sameregion_write(update_rc rcop);
bool known_parentptr_write(update_rc rcop);

#endif
