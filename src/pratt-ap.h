#ifndef PRATT_H
#define PRATT_H
/* Copyright (C) 2003 Greg Morrisett
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
   Suite 330, Boston, MA 02111-1307, USA. 


   This file implements Pratt's algorithm for determining whether
   a set of constraints over integer-valued variables is satisfiable.
   In particular, the algorithm can handle constraints of the form
   x - y <= c where x and y are variables and c is a constant.  
   Note that you can encode all kinds of other constraints into
   this form (e.g., x = y + c, x > y, x < y, etc.)  Note also that
   you can treat x <= c using a constraint of the form x - Z <= c
   where Z is some distinguished variable used to represent 0.  In
   turn, this allows us to encode more constraints (e.g., x = c,
   x < c, etc.)

   The algorithm works by essentially treating x - y <= c as a
   directed, weighted edge in a graph of variable nodes.  In
   particular, we represent this as x ---c---> y.  When adding new
   constraint to the graph, the resulting set of constraints is
   satisfiable iff there is no negative-weight cycle between nodes.
   To track this incrementally, we calculate the shortest path between
   each node, which means that when we add a new constraint, we're
   doing O(n^2) work.  So, adding a whole bunch of edges results in
   O(n^3) work.  There's a better algorithm, but this seems to work
   well enough for now, since we have a relatively small number
   of variables and constraints.
*/
#include "absyn.h"
#include <ap.h>
namespace Pratt {
  using Absyn;
  // nodes of the graph are either variables, the distinguished constant zero,
  // the number of elements of a variable, or an integer-kinded type variable.
  @tagged union Node {
    int       NZero;  // int unused
    vardecl_t NVar;
    vardecl_t NNumelts;
    type_t    NType;
    unsigned  NParam;
    unsigned  NParamNumelts;
    int       NReturn;  // int unused
  };
  // constructors for nodes
  extern union Node zero_node;
  typedef union Node node_t;
  extern node_t NVar(vardecl_t);
  extern node_t NType(type_t);
  extern node_t NNumelts(vardecl_t);
  extern node_t NParam(unsigned);
  extern node_t NParamNumelts(unsigned);
  extern node_t NReturn();

  // keep graphs abstract -- we're likely to want to change the rep.
  struct Graph;
  typedef struct Graph graph_t;
  extern void print_graph(graph_t *);

  // create an empty graph
  extern struct Graph @empty_graph();
  extern graph_t@ copy_graph(graph_t@);
  // Add a constraint that i - j <= a.  Returns the new graph if it is
  // consistent, otherwise returns null.  
  // This is an *imperative* update, so the old graph is destroyed.
  extern graph_t*`r add_edge(graph_t @`r G, node_t i, node_t j, AP_T a) ;
}
#endif
