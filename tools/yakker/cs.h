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
#include "bnf.h"

extern void cs_intersect(cs_t a,cs_t b);
extern void cs_union(cs_t a,cs_t b);
extern void cs_xor(cs_t a,cs_t b);
extern void cs_insert(cs_t a,unsigned char x);
extern void cs_difference(cs_t a,cs_t b);
extern void cs_complement(cs_t a);
extern cs_t cs_dup(cs_t a);
extern cs_t cs_empty();
extern cs_t cs_full();
extern cs_t cs_singleton(unsigned char x);
extern cs_t cs_range(unsigned int lb, unsigned int ub); /* half-open interval [lb,ub) */
extern $(unsigned char,unsigned char) cs_bounds(cs_t rng);
extern void cs_print(FILE @f,cs_t rng);
extern const char ?cs2string(cs_t rng);
extern cs_t string2cs(const char ?s);
extern int cs_member(cs_t a,unsigned char c);
extern int is_cs(rule_t r);
extern cs_t rule2cs(rule_t r);
extern int is_cs_empty(cs_t a);
extern int is_cs_full(cs_t a);
extern int is_cs_singleton(cs_t a);
extern int first_of_cs(cs_t a);
extern struct grammar @cs_annot(grammar_t<`H> ds);
// convert all rules marked as css into actual css.
extern void cs_optimize(grammar_t<`H> ds);
extern void minus_elim(grammar_t<`H> ds);
extern const char ?fsm_char_escape(unsigned int c);
extern List::list_t<$(unsigned int,unsigned int)@> cs2ranges(cs_t a);
extern rule_t cs2rule(cs_t rng);
extern List::list_t<unsigned int> cs2list(cs_t a);
#endif
