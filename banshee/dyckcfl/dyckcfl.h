/*
 * Copyright (c) 2000-2004
 *      The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/*****************************************************************************
 *                                                                           *
 *   Dyck CFL Reachability Using Banshee                                     *
 *                                                                           *
 *****************************************************************************/
#ifndef DYCKCFL_H
#define DYCKCFL_H

#include <stdio.h>
#include "bool.h"
#include "list.h"

typedef struct dyck_node_ * dyck_node;

DECLARE_LIST(dyck_node_list, dyck_node);

// If true, inclusion constraints will be printed
extern int flag_dyck_print_constraints; 

/*****************************************************************************
 *                                                                           *
 *   Initialization/ Resetting                                               *
 *                                                                           *
 *****************************************************************************/

// Initialize the engine (must call nonspec_init beforehand). If
// enable_pn is true, pn-reachability queries may be asked.
void dyck_init(bool enable_pn);

// Reset all internal state
void dyck_reset(void);

/*****************************************************************************
 *                                                                           *
 *   Graph Construction API                                                  *
 *                                                                           *
 *****************************************************************************/

// Makes a node tagged with a unique version of the given name. Always
// creates a new node and tag, even if a node with the same name already
// exists. The node will be called name.
dyck_node make_tagged_dyck_node(const char *name);

// Make a node which we never want to query (as a source) directly. If
// source queries are made, the semantics are unspecified. What this
// does in practice is give us a cheaper representation of the node,
// at the expense of asking source queries about it. So note: if n is
// an untagged node, dyck_check_reaches(n',n), where n' is tagged has
// the expected semantics, but dyck_check_reaches(n,x) for any node x
// is unspecified (even if x is n!). The node will be called name.
dyck_node make_untagged_dyck_node(const char *name);

// Make a node tagged "empty". If any tag (besides empty) reaches this
// node, the error callback will be triggered. The node will be called
// name. 
dyck_node make_tagged_empty_dyck_node(const char *name);

// Make a node tagged "universal". If this node reaches any tag (other
// than another universal tag), the error callback will be
// triggered. The node will be called name
dyck_node make_tagged_universal_dyck_node(const char *name);

// Mark the argument node "global". Flow through global nodes is
// always promoted to "matched". 
void mark_dyck_node_global(dyck_node n);

// Make an s edge between n1 and n2
void make_dyck_subtype_edge(dyck_node n1, dyck_node n2);

// Make an (_{index} edge between n1 and n2
void make_dyck_open_edge(dyck_node n1, dyck_node n2, int index);

// Make an (_{index} contravariant edge between n1 and n2
void make_dyck_contra_open_edge(dyck_node n1, dyck_node n2, int index);

// Make an )_{index} contravariant edge between n1 and n2
void make_dyck_contra_close_edge(dyck_node n1, dyck_node n2, int index);

// Make an (_{index} co/contravariant edge between n1 and n2
// void make_dyck_cocontra_open_edge(dyck_node n1, dyck_node n2, int index);

// Make )_{index} to match a covariant (_{index} made with cocontra_open_edge
// void make_dyck_co_close_edge(dyck_node n1, dyck_node n2, int index);

// Make )_{index} to match a contravariant (_{index} made with cocontra_close_edge
// void make_dyck_contra_close_edge(dyck_node n1, dyck_node n2, int index);

// Make an )_{index} edge between n1 and n2
void make_dyck_close_edge(dyck_node n1, dyck_node n2, int index);

// Make (_{i} between each of n1s and n2, with the given
// indices. length indicates how many.
void make_clustered_dyck_open_edges(dyck_node n1s[], dyck_node n2, 
				    int indices[], int length);

// Make a )_{index} edge between n1 and n2, where the corresponding open
// edge was created by make_grouped_dyck_open_edges. Open and close edges
// created with clusters don't interact at all with non-cluster edges
void make_dyck_close_edge_for_cluster(dyck_node n1, dyck_node n2, int index);

/*****************************************************************************
 *                                                                           *
 *   Query API                                                               *
 *                                                                           *
 *****************************************************************************/

// Must be called before any queries and after all nodes/edges have been added
// NOTE: only true of the mr_dyckcfl implementation. No need to call
// this with dyckcfl.c
void dyck_finished_adding(void);

// Return true if n1 reaches n2 by standard dyck reachability
bool dyck_check_reaches(dyck_node n1, dyck_node n2);

// Return true if n1 reaches n2 by pn-dyck reachability (requires enable_pn)
bool dyck_check_pn_reaches(dyck_node n1, dyck_node n2);

// Return the list of nodes that reach this node
dyck_node_list dyck_reaches(dyck_node n);

// Return the list of nodes that PN reach this node
/* TODO -- an ugly corner case-- some nodes might appear multiple times if
   they reach by more than one of P, N, and M
*/
dyck_node_list dyck_pn_reaches(dyck_node n);

/*****************************************************************************
 *                                                                           *
 *   Debugging API                                                           *
 *                                                                           *
 *****************************************************************************/

// Print (in dot format) a representation of all the edges found during closure
void dyck_print_closed_graph(FILE *f);

#endif /* DYCKCFL_H */
