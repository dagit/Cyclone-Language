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

#ifndef EARLEY_H
#define EARLEY_H
#include "bnf.h"
#include "fa.h"

namespace Earley {
  extern $(dfa_t,Set::set_t<st_t>) compile(grammar_t grm, rule_t r);
  // returns: int pair: a) the number of valid complete parses of the string.
  //                    b) the number of valid prefix parses of the string.
  extern $(int,int) recognize(dfa_t dfa, Set::set_t<st_t> dfa_final, const char ?input, int show_progress);

  typedef List::list_t<datatype ParseTree @`H,`H> parse_forest_t;
  typedef List::List_t<datatype ParseTree @`H,`H> Parse_forest_t;

  typedef datatype ParseTree @`H parse_tree_t;

  extern datatype ParseTree{
    NonTerm(const char ?`H, int, int, parse_forest_t);
    SharedNonTerm(const char ?`H, int, int, List::List_t<parse_forest_t,`H>);
  };

  /* Global variable that saves last derivation tree.
   !! Treat as read-only outside of earley.cyc.
   */
  extern datatype ParseTree @last_derivation_tree;
  /* Print left-most parse tree to depth of "depth". Use -1 to print entire tree. */
  extern void print_LT(datatype ParseTree @pt, int depth);
  /* Print right-most parse tree to depth of "depth". Use -1 to print entire tree. */
  extern void print_RT(datatype ParseTree @pt, int depth);

  extern void act2symb_haxe(FILE @f);

  extern int isAmb_parse_tree(datatype ParseTree @pt);

  // Convert and print the rule as an NFA in FSM format.
  // Writes two files based on the filename_base: .fsm and .sym. The
  // former holds the NFA, the latter the mapping from symbolic names to
  // numbers.
  extern void rule_fsm(const char ?filename_base, grammar_t grm, rule_t r);
  extern $(dfa_t,Set::set_t<st_t>) *fsm2dfa(const char ?filename, const char ?`H start_symb);
  extern void dfa2dot(dfa_t dfa, const char ?dot_file);
}

#endif
