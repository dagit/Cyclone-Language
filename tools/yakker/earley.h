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
  extern int recognize(dfa_t dfa, Set::set_t<st_t> dfa_final, const char ?input, int show_progress);

  /* Global variable that saves last derivation tree.
   !! Treat as read-only outside of earley.cyc.
   */
  extern datatype ParseTree @last_derivation_tree;
  /* Print left-most parse tree to depth of "depth". Use -1 to print entire tree. */
  extern void print_LT(datatype ParseTree @pt, int depth);
  /* Print right-most parse tree to depth of "depth". Use -1 to print entire tree. */
  extern void print_RT(datatype ParseTree @pt, int depth);
}

#endif
