/* This file is part of the Cyclone Library.
   Copyright (C) 2004 Greg Morrisett, AT&T

   This library is free software; you can redistribute it and/or it
   under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of
   the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place, Suite
   330, Boston, MA 02111-1307 USA. */

#ifndef GRAPH_H
#define GRAPH_H

#include <set.h>
#include <list.h>
#include <dict.h>

namespace Graph {

  /*** \subsection{\texttt{<graph.h>}} */
  /*** Defines namespace Graph, which implements functional graphs
       that are parameterized over the type of the nodes of the graph.
       To create a graph you must supply a comparison function on
       nodes (the comparison function should return a number less
       than, equal to, or greater than 0 according to whether its
       first argument is less than, equal to, or greater than its
       second argument).

       A node [s] that is the source of an edge ([s],[t]) in a graph
       is called a source node of the graph; a node that appears only
       as a target is not a source node.  Whether a node is a source
       node matters for some of the graph functions, notably, [scc]
       and [tsort].  Use [add_node] to ensure that a node is a source
       node. */

typedef Dict::dict_t<`a,Set::set_t<`a>> graph_t<`a>;
  /** A value of type [graph_t<`a>] is a graph with nodes of type [`a]. */
void print(FILE @`r f, graph_t<`a> g, void (@nodeprint)(FILE @`r,`a));
  /** [print(f,g,nodeprint)] prints the graph [g] to file [f].
      [nodeprint] must be a printing function for nodes.  The output
      is in the DOT language, suitable for display by Graphviz. */
extern graph_t<`a> empty(int (@`H cmp)(`a,`a));
  /** [empty(cmp)] creates an empty graph given comparison function
      [cmp].  The graph is heap-allocated. */
extern graph_t<`a> add_node(graph_t<`a> g, `a s);
  /** [add_node(g,s)] returns a graph identical to [g], except that [s]
      is added as a source node of [g] (if necessary). */
extern graph_t<`a> add_edge(graph_t<`a> g, `a s, `a t);
  /** [add_edge(g,s,t)] returns a graph identical to [g] except that there is an
      edge from [s] to [t].  [t] is not added as a source node of the result. */
extern graph_t<`a> add_edges(graph_t<`a> g, `a s, Set::set_t<`a,`H> T);
  /** [add_edges(g,s,T)] returns a graph identical to [g] except that
      there is an edge from [s] to every node in the set [T].  The
      nodes of [T] are not added as source nodes of the result. */
extern graph_t<`a> remove_edge(graph_t<`a> g, `a s, `a t);
  /** [remove_edge(g,s,t)] returns a graph identical to [g] except that any
      edge from [s] to [t] is removed. */
extern graph_t<`a> remove_edges(graph_t<`a> g, `a s, Set::set_t<`a,`H> T);
  /** [remove_edge(g,s,T)] returns a graph identical to [g] except that any
      edge from [s] to a node in the set [T] is removed. */
extern int is_edge(graph_t<`a> g, `a s, `a t);
  /** [is_edge(g,s,t)] returns true if there is an edge from [s] to [t] in [g],
      and returns false otherwise. */
extern Set::set_t<`a> get_targets(graph_t<`a>,`a s);
  /** [get_targets(g,s)] returns the set of targets of node [s] in graph [g]. */
extern graph_t<`a> tc(graph_t<`a> g);
  /** [tc(g)] returns the transitive closure of graph [g]: the nodes
      of [tc(g)] are the nodes of [g], and there is an edge from a
      node [s] to a node [t] in [tc(g)] iff [s] can reach [t] by one
      or more edges in [g].  Note that [tc(g)] is not necessarily
      reflexive. */
extern graph_t<`a> tkernel(graph_t<`a> g);
  /** [tkernel(g)] returns the transitive kernel of graph [g]: the
      nodes of [tkernel(g)] are the nodes of [g], and the edges
      [tkernel(g)] are a minimal subset of the edges of [g] such that
      [tc(tkernel(g))] is the same as [tc(g)]. */
extern List::list_t<`a> tsort(graph_t<`a> g);
  /** [tsort(g)] returns a list of the source nodes of [g], in
      topological order.  That is, if [s] can reach [t] by one or more
      edges in [g], then [s] appears before [t] in [tsort(g)], unless
      [t] can also reach [s] by one or more edges in [g], in which
      case [s] and [t] will appear in some unspecified order in the
      output. */
extern graph_t<`a> scc(graph_t<`a> g);
  /** [scc(g)] returns the strongly connected components of graph [g]:
      the source nodes of [scc(g)] are the source nodes of [g], and
      the targets of any node [s] in [scc(g)] are the members of the
      strongly connected component of [s] in [g].  We consider every
      [s] to be a member of its own strongly connected component. */
}

#endif
