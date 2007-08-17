/* Copyright (C) 2005 Greg Morrisett, AT&T.
   This file is part of the Cyclone project.

   Cyclone is free software; you can redistribute it
   and/or modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   Cyclone is distributed in the hope that it will be
   useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Cyclone; see the file COPYING. If not,
   write to the Free Software Foundation, Inc., 59 Temple Place -
   Suite 330, Boston, MA 02111-1307, USA. */

#ifndef CS_H
#define CS_H
#include <stdio.h>
#include <list.h>

/* Character (action) sets */

// TODO: remove BUCKETS and MAXACTION from interface
#define BUCKETS 53
#define MAXACTION ((BUCKETS*8)-1)
typedef unsigned char @{BUCKETS}@nozeroterm cs_t;
typedef unsigned char *{BUCKETS}@nozeroterm cs_opt_t;

extern void cs_intersect(cs_t a,cs_t b);
extern void cs_union(cs_t a,cs_t b);
extern void cs_xor(cs_t a,cs_t b);
extern void cs_insert(cs_t a,unsigned int x);
extern void cs_difference(cs_t a,cs_t b);
extern void cs_complement(cs_t a);
extern cs_t cs_dup(cs_t a);
extern cs_t cs_empty();
extern cs_t cs_full();
extern cs_t cs_singleton(unsigned int x);
extern cs_t cs_range(unsigned int lb, unsigned int ub); /* half-open interval [lb,ub) */
extern cs_t string2cs(const char ?s);

extern int cs_member(cs_t a,unsigned int c);
extern int is_cs_empty(cs_t a);
extern int is_cs_full(cs_t a);
extern int is_cs_singleton(cs_t a);
extern int first_of_cs(cs_t a);
/* return min and max characters for non-empty set */
extern $(unsigned int,unsigned int) cs_bounds(cs_t rng);
/* return a list of all characters in the set */
extern List::list_t<unsigned int> cs2list(cs_t a);
/* return list of half-open intervals in sorted order */
extern List::list_t<$(unsigned int,unsigned int)@> cs2ranges(cs_t a);

/* convert/print in (nonstandard) readable format */
extern const char ?cs2string(cs_t rng);
extern void cs_print(FILE @f,cs_t rng);
/* suitable for Graphviz */
extern void cs_dot_print(FILE @f,cs_t cs);

extern const char ?cs_char_escape(unsigned int c);
extern const char ?fsm_char_escape(unsigned int c);
#endif
