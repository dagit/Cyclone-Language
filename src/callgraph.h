/* Constructing call graphs.
   Copyright (C) 2004 Greg Morrisett, AT&T
   This file is part of the Cyclone compiler.

   The Cyclone compiler is free software; you can redistribute it
   and/or modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   The Cyclone compiler is distributed in the hope that it will be
   useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with the Cyclone compiler; see the file COPYING. If not,
   write to the Free Software Foundation, Inc., 59 Temple Place -
   Suite 330, Boston, MA 02111-1307, USA. */
#ifndef _CALLGRAPH_H
#define _CALLGRAPH_H

#include "absyn.h"
#include <graph.h>

namespace Callgraph {
  typedef Graph::graph_t<Absyn::fndecl_t> callgraph_t;
  // Computes the call graph of the functions in a list of declarations.
  // We'll have an edge from f to g if g occurs anywhere in f (not just
  // a function call.)  
  callgraph_t compute_callgraph(List::list_t<Absyn::decl_t>);
  void print_callgraph(FILE @, callgraph_t);
}

#endif
