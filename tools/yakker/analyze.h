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

#ifndef ANALYZE_H
#define ANALYZE_H
#include <graph.h>
#include "bnf.h"
#include "cs.h"
extern strset_t recursive_symbols(grammar_t grm);
extern int white_edge_symbol(str_t s);
extern void reset_globals();
extern void report_empty_nonterminals(grammar_t grm);
extern void report_left_recursion(grammar_t grm);
extern void report_right_recursion(grammar_t grm);
extern void report_white_edges(grammar_t grm);
extern void report_first_follow(grammar_t grm);
extern void report_conflicts(grammar_t<`H> grm);
extern void report_glush(grammar_t<`H> grm);
extern void report_whitespace(grammar_t<`H> grm);
extern void first(cs_t result, rule_t r);
extern void last(cs_t result, rule_t r);
extern void init_recursive (grammar_t ds);
extern void init_white_edge_symbols(grammar_t ds);
extern void init_firstt(grammar_t grm);
extern void init_mayt(grammar_t grm);
extern void init_mustt(grammar_t grm);
extern void init_maybe_empty(grammar_t grm);
extern void init_arityt(grammar_t grm);
extern unsigned int arity_rule(rule_t r);
extern const char ?follow_symbol(const char ?n);
extern grammar_t follow_grammar(grammar_t grm);

#endif
