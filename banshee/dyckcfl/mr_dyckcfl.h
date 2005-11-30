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
 *   Melski-Reps Dyck CFL Reachability Using Banshee                         *
 *                                                                           *
 *****************************************************************************/
#ifndef MR_DYCKCFL_H
#define MR_DYCKCFL_H

#include <stdio.h>
#include "bool.h"
#include "list.h"

// DECLARE_LIST(string_list,char *);

typedef struct mr_dyck_node_ * mr_dyck_node;

// If true, inclusion constraints will be printed
extern int flag_mr_dyck_print_constraints; 

/*****************************************************************************
 *                                                                           *
 *   Initialization/ Resetting                                               *
 *                                                                           *
 *****************************************************************************/

// Initialize the engine (must call nonspec_init beforehand). If
// enable_pn is true, pn-reachability queries may be asked.
void mr_dyck_init(bool enable_pn, FILE *f);

// Reset all internal state
void mr_dyck_reset(void);

/*****************************************************************************
 *                                                                           *
 *   Graph Construction API                                                  *
 *                                                                           *
 *****************************************************************************/

// Makes a node tagged with a unique version of the given name. Always
// creates a new node and tag, even if a node with the same name already
// exists. The node will be called name.
mr_dyck_node make_tagged_mr_dyck_node(const char *name);

// Make a node which we never want to query (as a source) directly. If
// source queries are made, the semantics are unspecified. What this
// does in practice is give us a cheaper representation of the node,
// at the expense of asking source queries about it. So note: if n is
// an untagged node, dyck_check_reaches(n',n), where n' is tagged has
// the expected semantics, but dyck_check_reaches(n,x) for any node x
// is unspecified (even if x is n!). The node will be called name.
mr_dyck_node make_untagged_mr_dyck_node(const char *name);

// Make a node tagged "empty". If any tag (besides empty) reaches this
// node, the error callback will be triggered. The node will be called
// name. 
mr_dyck_node make_tagged_empty_mr_dyck_node(const char *name);

// Make a node tagged "universal". If this node reaches any tag (other
// than another universal tag), the error callback will be
// triggered. The node will be called name
mr_dyck_node make_tagged_universal_mr_dyck_node(const char *name);

// Mark the argument node "global". Flow through global nodes is
// always promoted to "matched".
void mark_mr_dyck_node_global(mr_dyck_node n);

// Make an s edge between n1 and n2
void make_mr_dyck_subtype_edge(mr_dyck_node n1, mr_dyck_node n2);

// Make an (_{index} edge between n1 and n2
void make_mr_dyck_open_edge(mr_dyck_node n1, mr_dyck_node n2, int index);

// Make an )_{index} edge between n1 and n2
void make_mr_dyck_close_edge(mr_dyck_node n1, mr_dyck_node n2, int index);

// This API does not support clustering (see dyckcfl.h)

/*****************************************************************************
 *                                                                           *
 *   Query API                                                               *
 *                                                                           *
 *****************************************************************************/

// Must be called before any queries and after all nodes/edges have been added
void mr_dyck_finished_adding(void);

// Return true if n1 reaches n2 by standard dyck reachability
bool mr_dyck_check_reaches(mr_dyck_node n1, mr_dyck_node n2);

// Return true if n1 reaches n2 by pn-dyck reachability (requires enable_pn)
bool mr_dyck_check_pn_reaches(mr_dyck_node n1, mr_dyck_node n2);

/*****************************************************************************
 *                                                                           *
 *   Debugging API                                                           *
 *                                                                           *
 *****************************************************************************/

// Print (in dot format) a representation of all the edges found during closure
void mr_dyck_print_closed_graph(FILE *f);

// define this in order to enable the next two debugging features
// #define DEBUG_ERRONEOUS_EDGES

// Note that node n was at the source end of an erroneous subtype edge
void add_erroneous_source(mr_dyck_node n);

// Note that node n was the sink of all erroneous subtype edges
void set_erroneous_sink(mr_dyck_node n);

#endif /* MR_DYCKCFL_H */
