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

#ifndef FA_H
#define FA_H
extern unsigned int num_conflicts_ll;
extern unsigned int num_conflicts_dfa;
extern unsigned int num_conflicts_change_order;
extern unsigned int num_conflicts_resolved;
extern unsigned int num_conflicts_unresolved;
extern unsigned int num_conflicts_unbounded;
extern struct DFA;
typedef struct DFA @dfa_t;
extern dfa_t lookahead_dfa(grammar_t grm,List::list_t<rule_t> rules, rule_t right_ctxt);
extern void test_nfa2dfa(grammar_t<`H> grm);
extern void test_tc(grammar_t<`H> grm);
extern void dfa_generate1(dfa_t dfa);
extern void dfa_generate2(dfa_t dfa);
#endif
