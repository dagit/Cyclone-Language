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

#include <assert.h>		// assert
#include <string.h>		// strcmp
#include "mr_dyckcfl.h"
#include "nonspec.h"
#include "hash.h"

/*****************************************************************************
 *                                                                           *
 *   Type Declarations                                                       *
 *                                                                           *
 *****************************************************************************/

typedef enum {mr_dyck_raw, mr_dyck_inited, mr_dyck_query} mr_dyck_state;
typedef enum {mr_o, mr_k, mr_c} mr_edge_kind;

struct mr_dyck_node_ {
  gen_e node_constant;
  gen_e node_variable;
  bool global;
};

DECLARE_LIST(mr_dyck_node_list,mr_dyck_node);
DEFINE_NONPTR_LIST(mr_dyck_node_list,mr_dyck_node);

#define MINIMIZE_CONSTRAINTS

#ifdef MINIMIZE_CONSTRAINTS
DECLARE_LIST(relevant_constraint_list,int);
DEFINE_NONPTR_LIST(relevant_constraint_list,int);
static relevant_constraint_list relevant_constraints;
#endif

static int constraint_count = 0;

/*****************************************************************************
 *                                                                           *
 *   Global Variables                                                        *
 *                                                                           *
 *****************************************************************************/

static mr_dyck_state state = mr_dyck_raw;
static region mr_dyckregion = NULL;
static mr_dyck_node_list mr_all_nodes = NULL;
static hash_table mr_k_hash = NULL;
static hash_table mr_o_hash = NULL;
static hash_table mr_c_hash = NULL;
static hash_table mr_seen_indices = NULL;
static constructor mr_s_constructor = NULL;
static constructor mr_n_constructor = NULL;
static constructor mr_p_constructor = NULL;
static constructor mr_start_constructor = NULL;
static bool pn_reach = FALSE;

int flag_mr_dyck_print_constraints = 0;

#ifdef DEBUG_ERRONEOUS_EDGES
static mr_dyck_node_list mr_erroneous_sources = NULL;
static mr_dyck_node mr_erroneous_sink = NULL;
#endif

/*****************************************************************************
 *                                                                           *
 *   Static Utility Functions                                                *
 *                                                                           *
 *****************************************************************************/


static void check_erroneous_edges(void)
{
#ifdef DEBUG_ERRONEOUS_EDGES
  bool found = FALSE;
  mr_dyck_node_list_scanner scan;
  mr_dyck_node next_source;
  
  mr_dyck_node_list_scan(mr_erroneous_sources,&scan);
  
  while(mr_dyck_node_list_next(&scan,&next_source)) {
    state = mr_dyck_query; 	// HACK to allow queries here
    if (mr_dyck_check_reaches(next_source,mr_erroneous_sink)) {
      found = TRUE;
      expr_print(stdout, next_source->node_constant);
      printf(" --> ");
      expr_print(stdout, mr_erroneous_sink->node_constant);
      printf(" has been added to the graph.\n");
    }
    state = mr_dyck_inited;
  }
  if (found) { assert(FALSE);}
#endif /* DEBUG_ERRONEOUS_EDGES */
}

static bool relevant_constraint(int num)
{
#ifdef MINIMIZE_CONSTRAINTS
  if (! relevant_constraints)
    return TRUE;
  {
    relevant_constraint_list_scanner scan;
    int next;

    relevant_constraint_list_scan(relevant_constraints,&scan);

    while(relevant_constraint_list_next(&scan,&next)) {
      if (num == next) return TRUE;
    }
  }
  return FALSE;
#else
  return TRUE;
#endif
}

static void mr_call_setif_inclusion(gen_e e1, gen_e e2)
{
  assert(state == mr_dyck_inited);
  if (relevant_constraint(++constraint_count)) {
    if (flag_mr_dyck_print_constraints) {
      expr_print(stdout,e1);
      printf(" <= " );
      expr_print(stdout,e2);
      printf("\n");
    }
    call_setif_inclusion(e1,e2);
  }

  check_erroneous_edges();
}

static constructor get_constructor(int index, mr_edge_kind kind)
{
  hash_table built_constructors = NULL;
  constructor result = NULL;
  char name[512];

  sig_elt c_sig[1] = {{vnc_pos,setif_sort}};

  // Add this index to the list of seen indices
  if (!hash_table_lookup(mr_seen_indices,(void *)index, NULL)) {
    hash_table_insert(mr_seen_indices,(void *)index,(void *)index);
  }

  switch (kind) {
  case mr_o: 
    built_constructors = mr_o_hash;
    snprintf(name,512,"(_%d",index);
    break;
  case mr_c:
    built_constructors = mr_c_hash;
    snprintf(name,512,")_%d",index);
    break;
  case mr_k:
    built_constructors = mr_k_hash;
    snprintf(name,512,"K_%d",index);
    break;
  }
  assert(built_constructors);
  assert(name);

  // Check the hash to see if the constructor has been built yet
  // If not, build the constructor
  if (!hash_table_lookup(built_constructors,(void *)index,(hash_data *)&result)) {
    result = make_constructor(name,setif_sort,c_sig,1);
    // Hash it so that it can be be retrieved later
    hash_table_insert(built_constructors,(void *)index,result);
  }

  assert(hash_table_lookup(built_constructors,(void *)index,NULL));
  assert(result);
  return result;
}

static void encode_SSS_production(mr_dyck_node n)
{
  gen_e exps[1];
  gen_e rch,dst;

  rch = setif_proj(mr_s_constructor,0,n->node_variable);
  dst = setif_proj(mr_s_constructor,0,rch);
  exps[0] = dst;
  mr_call_setif_inclusion(constructor_expr(mr_s_constructor,exps,1),n->node_variable);
}

static void encode_KOS_production(mr_dyck_node n, int index)
{
  gen_e exps[1];
  gen_e rch,dst;

  rch = setif_proj(get_constructor(index,mr_o),0,n->node_variable);
  dst = setif_proj(mr_s_constructor,0,rch);
  exps[0] = dst;
  mr_call_setif_inclusion(constructor_expr(get_constructor(index,mr_k),exps,1),n->node_variable);
}

static void encode_SKC_production(mr_dyck_node n, int index)
{
  gen_e exps[1];
  gen_e rch,dst;

  rch = setif_proj(get_constructor(index,mr_k),0,n->node_variable);
  dst = setif_proj(get_constructor(index,mr_c),0,rch);
  exps[0] = dst;
  mr_call_setif_inclusion(constructor_expr(mr_s_constructor,exps,1),n->node_variable);
}

static void encode_startPN_production(mr_dyck_node n)
{
  gen_e exps[1];
  gen_e rch,dst;

  rch = setif_proj(mr_p_constructor,0,n->node_variable);
  dst = setif_proj(mr_n_constructor,0,rch);
  exps[0] = dst;
  mr_call_setif_inclusion(constructor_expr(mr_start_constructor,exps,1),n->node_variable);
  
}

static void encode_PE_production(mr_dyck_node n)
{
  gen_e exps[1];
  exps[0] = n->node_variable;

  mr_call_setif_inclusion(constructor_expr(mr_p_constructor,exps,1),n->node_variable);
}

static void encode_NE_production(mr_dyck_node n)
{
  gen_e exps[1];
  exps[0] = n->node_variable;

  mr_call_setif_inclusion(constructor_expr(mr_n_constructor,exps,1),n->node_variable);
}

static void encode_PPP_production(mr_dyck_node n)
{
  gen_e exps[1];
  gen_e rch,dst;

  rch = setif_proj(mr_p_constructor,0,n->node_variable);
  dst = setif_proj(mr_p_constructor,0,rch);
  exps[0] = dst;
  mr_call_setif_inclusion(constructor_expr(mr_p_constructor,exps,1),n->node_variable);
}

static void encode_NNN_production(mr_dyck_node n)
{
  gen_e exps[1];
  gen_e rch,dst;

  rch = setif_proj(mr_n_constructor,0,n->node_variable);
  dst = setif_proj(mr_n_constructor,0,rch);
  exps[0] = dst;
  mr_call_setif_inclusion(constructor_expr(mr_n_constructor,exps,1),n->node_variable);
}

static void encode_PSP_production(mr_dyck_node n)
{
  gen_e exps[1];
  gen_e rch,dst;

  rch = setif_proj(mr_s_constructor,0,n->node_variable);
  dst = setif_proj(mr_p_constructor,0,rch);
  exps[0] = dst;
  mr_call_setif_inclusion(constructor_expr(mr_p_constructor,exps,1),n->node_variable);
}

static void encode_NSN_production(mr_dyck_node n)
{
  gen_e exps[1];
  gen_e rch,dst;

  rch = setif_proj(mr_s_constructor,0,n->node_variable);
  dst = setif_proj(mr_n_constructor,0,rch);
  exps[0] = dst;
  mr_call_setif_inclusion(constructor_expr(mr_n_constructor,exps,1),n->node_variable);
}    

/*****************************************************************************
 *                                                                           *
 *   Initialization/ Resetting                                               *
 *                                                                           *
 *****************************************************************************/

void mr_dyck_init(bool pn,FILE *rconstraints)
{
  sig_elt s_sig[1] = {{vnc_pos,setif_sort}};

  assert(state == mr_dyck_raw);
  pn_reach = pn;
  mr_dyckregion = newregion();
  mr_all_nodes = new_mr_dyck_node_list(mr_dyckregion);

  mr_k_hash = make_hash_table(mr_dyckregion, 32, ptr_hash, ptr_eq);
  mr_o_hash = make_hash_table(mr_dyckregion, 32, ptr_hash, ptr_eq);
  mr_c_hash = make_hash_table(mr_dyckregion, 32, ptr_hash, ptr_eq);
  mr_seen_indices = make_hash_table(mr_dyckregion,32,ptr_hash,ptr_eq);

  mr_s_constructor = make_constructor("s",setif_sort,s_sig,1);
  mr_p_constructor = make_constructor("p",setif_sort,s_sig,1);
  mr_n_constructor = make_constructor("n",setif_sort,s_sig,1);
  mr_start_constructor = make_constructor("start",setif_sort,s_sig,1);

#ifdef DEBUG_ERRONEOUS_EDGES
  mr_erroneous_sources = new_mr_dyck_node_list(mr_dyckregion);
#endif

#ifdef MINIMIZE_CONSTRAINTS
  if (rconstraints) {
    char buf[100];
    region scratch = newregion();
    relevant_constraints = new_relevant_constraint_list(scratch);

    printf("Reading relevant constraints\n");

    while (fgets(buf,100,rconstraints)) {
      int next_relevant = atoi(buf);
      assert(next_relevant);
      //	printf("%d\n",next_relevant);
      relevant_constraint_list_cons(next_relevant, relevant_constraints);
    }
    fclose(rconstraints);
  }
#endif

  state = mr_dyck_inited;
}

void mr_dyck_reset()
{
  deleteregion(mr_dyckregion);
  mr_dyckregion = NULL;
  mr_all_nodes = NULL;
  mr_k_hash = NULL;
  mr_o_hash = NULL;
  mr_c_hash = NULL;
  mr_seen_indices = NULL;
  mr_s_constructor = NULL;
  mr_n_constructor = NULL;
  mr_p_constructor = NULL;
  mr_start_constructor = NULL;

#ifdef DEBUG_ERRONEOUS_EDGES
  mr_erroneous_sources = NULL;
  mr_erroneous_sink = NULL;
#endif


  state = mr_dyck_raw;
}

/*****************************************************************************
 *                                                                           *
 *   Graph Construction API                                                  *
 *                                                                           *
 *****************************************************************************/

mr_dyck_node make_tagged_mr_dyck_node(const char *name) 
{
  static int count = 0;
  char unique_name[512];
  mr_dyck_node result = ralloc(mr_dyckregion, struct mr_dyck_node_);
  assert(state == mr_dyck_inited);

  snprintf(unique_name,512,"mr_%s_%d",name,count++);
  result->node_constant = setif_constant(unique_name);
  result->node_variable = setif_fresh(name);
  result->global = FALSE;

  mr_dyck_node_list_cons(result,mr_all_nodes);
  mr_call_setif_inclusion(result->node_constant, result->node_variable);

  return result;
}


mr_dyck_node make_untagged_mr_dyck_node(const char *name)
{
  static int count = 0;
  char unique_name[512];
  mr_dyck_node result = ralloc(mr_dyckregion, struct mr_dyck_node_);
  assert(state == mr_dyck_inited);

  snprintf(unique_name,512,"mr_%s_%d",name,count++);
  result->node_constant = setif_constant(unique_name);
  result->node_variable = setif_fresh(name);
  
  mr_dyck_node_list_cons(result,mr_all_nodes);

  return result;
}

// FIX : think about this, will it work?
mr_dyck_node make_tagged_empty_mr_dyck_node(const char *name)
{
  mr_dyck_node result = ralloc(mr_dyckregion, struct mr_dyck_node_);
  assert(state == mr_dyck_inited);

  result->node_constant = setif_zero();
  result->node_variable = setif_fresh(name);
  
  mr_dyck_node_list_cons(result,mr_all_nodes);
  mr_call_setif_inclusion(result->node_variable, result->node_constant);

  return result;
}
					   
mr_dyck_node make_tagged_universal_mr_dyck_node(const char *name)
{
  mr_dyck_node result = ralloc(mr_dyckregion, struct mr_dyck_node_);
  assert(state == mr_dyck_inited);

  result->node_constant = setif_one();
  result->node_variable = setif_fresh(name);
  
  mr_dyck_node_list_cons(result,mr_all_nodes);
  mr_call_setif_inclusion(result->node_constant, result->node_variable);

  return result;
}

void mark_mr_dyck_node_global(mr_dyck_node n)
{
  n->global = TRUE;
}

void make_mr_dyck_subtype_edge(mr_dyck_node n1, mr_dyck_node n2) 
{ 
  gen_e exps[1];

  assert(state == mr_dyck_inited);

  exps[0] = n2->node_variable;

  mr_call_setif_inclusion(constructor_expr(mr_s_constructor,exps,1),n1->node_variable);
}

void make_mr_dyck_open_edge(mr_dyck_node n1, mr_dyck_node n2, int index) 
{
  gen_e exps[1];
  assert(state == mr_dyck_inited);
 
  exps[0] = n2->node_variable;
  
  mr_call_setif_inclusion(constructor_expr(get_constructor(index,mr_o),
					   exps, 1), n1->node_variable);

  if (pn_reach) {
    mr_call_setif_inclusion(constructor_expr(mr_n_constructor,exps, 1), 
			    n1->node_variable);
  }
}

void make_mr_dyck_close_edge(mr_dyck_node n1, mr_dyck_node n2, int index) 
{
  gen_e exps[1];
  assert(state == mr_dyck_inited);
 
  exps[0] = n2->node_variable;
  
  mr_call_setif_inclusion(constructor_expr(get_constructor(index,mr_c),
					   exps, 1), n1->node_variable);

  if (pn_reach) {
    mr_call_setif_inclusion(constructor_expr(mr_p_constructor,exps, 1), 
			    n1->node_variable);
  }
}

/*****************************************************************************
 *                                                                           *
 *   Query API                                                               *
 *                                                                           *
 *****************************************************************************/

static void encode_productions(void) 
{
  mr_dyck_node_list_scanner scan;
  mr_dyck_node next_node;
  hash_table_scanner index_scan;
  int next_index;

  assert(state == mr_dyck_inited);

  mr_dyck_node_list_scan(mr_all_nodes,&scan);

  // for each node in the graph 
  while (mr_dyck_node_list_next(&scan,&next_node)) {
    // encode each S -> epsilon production
    make_mr_dyck_subtype_edge(next_node,next_node);
    // encode each S -> S S production
    encode_SSS_production(next_node);
 
    hash_table_scan(mr_seen_indices,&index_scan);
    // for each index i
    while (hash_table_next(&index_scan,(hash_data *)&next_index,NULL)) {
      // encode each K_i -> (_i S production
      encode_KOS_production(next_node,next_index);
      // encode each S -> K_i )_i production
      encode_SKC_production(next_node,next_index);
      // encode each n --)_i--> n production (on global nodes)
      if (next_node->global) {
		make_mr_dyck_open_edge(next_node,next_node,next_index);
 		make_mr_dyck_close_edge(next_node,next_node,next_index);
      }
    }
  }
}

static void encode_pn_productions(void)
{
  mr_dyck_node_list_scanner scan;
  mr_dyck_node next_node;

  // start by encoding all the standard productions
  encode_productions();


  mr_dyck_node_list_scan(mr_all_nodes,&scan);
  // for each node in the graph
  while (mr_dyck_node_list_next(&scan,&next_node)) {
    // encode each start -> PN production
    encode_startPN_production(next_node);
    // encode each P -> epsilon production
    encode_PE_production(next_node);
    // encode each N -> epsilon production
    encode_NE_production(next_node);
    // encode each P -> P P production
    encode_PPP_production(next_node);
    // encode each N -> N N production
    encode_NNN_production(next_node);
    // encode each P -> S P production
    encode_PSP_production(next_node);
    // encode each N -> S N production
    encode_NSN_production(next_node);

  }
}

void mr_dyck_finished_adding()
{
  if (pn_reach) encode_pn_productions();
  else encode_productions();

  //printf("These edges must be present now...\n");
  check_erroneous_edges();
  state = mr_dyck_query;
}

static bool mr_dyck_check_onelevel_reaches(mr_dyck_node n1, mr_dyck_node n2, 
					   constructor c)
{
  gen_e_list_scanner scan;
  gen_e next_lb;
  // compute the transitive lower bounds of n1
  gen_e_list tlb = setif_tlb(n1->node_variable);

  assert(state == mr_dyck_query);

  // check whether C(n2) <= n1
  gen_e_list_scan(tlb,&scan);

  while(gen_e_list_next(&scan,&next_lb)) {
    struct decon sdecon = deconstruct_expr(c,next_lb);

    if (sdecon.name) {
      gen_e_list_scanner contents_scan;
      gen_e contents_next;
      gen_e_list contents_tlb = setif_tlb(sdecon.elems[0]);

      assert(sdecon.arity == 1);
      assert(sdecon.elems);
      
      gen_e_list_scan(contents_tlb,&contents_scan);
      
      while(gen_e_list_next(&contents_scan,&contents_next)) {
	if (expr_eq(n2->node_constant,contents_next)) return TRUE;
      }
    }
  }

  return FALSE;  
}

bool mr_dyck_check_reaches(mr_dyck_node n1, mr_dyck_node n2)
{
  return mr_dyck_check_onelevel_reaches(n1,n2,mr_s_constructor);
}

bool mr_dyck_check_pn_reaches(mr_dyck_node n1, mr_dyck_node n2)
{
  assert(pn_reach);
  return mr_dyck_check_onelevel_reaches(n1,n2,mr_start_constructor);
}

/*****************************************************************************
 *                                                                           *
 *   Debugging API                                                           *
 *                                                                           *
 *****************************************************************************/

// Print (in dot format) a representation of the closed CFL graph, w/o
// PN edges
void mr_dyck_print_closed_graph(FILE *f)
{
  mr_dyck_node_list_scanner scan;
  mr_dyck_node next_node;

  assert(state == mr_dyck_query);

  fprintf(f,"digraph G {\n");
  fprintf(f,"size=\"8,10\";\n");

  mr_dyck_node_list_scan(mr_all_nodes,&scan);

  while(mr_dyck_node_list_next(&scan,&next_node)) {
    gen_e_list_scanner scan;
    gen_e next_lb;
    // compute the transitive lower bounds of next_node
    gen_e_list tlb = setif_tlb(next_node->node_variable);

    // print C(n2) <= n1
    gen_e_list_scan(tlb,&scan);

    while(gen_e_list_next(&scan,&next_lb)) {
      struct decon sdecon = deconstruct_expr(NULL,next_lb);
      
      // Just s constructors
      if (sdecon.arity == 1 && !strcmp(sdecon.name,"s")) {
	gen_e_list_scanner contents_scan;
	gen_e contents_next;
	gen_e_list contents_tlb = setif_tlb(sdecon.elems[0]);
	gen_e_list_scan(contents_tlb,&contents_scan);
	
	while(gen_e_list_next(&contents_scan,&contents_next)) {
	  assert(next_node->node_constant);
	  
	  if (expr_is_constant(contents_next)) {
	    fprintf(f,"\"");
	    expr_print(f,next_node->node_constant);
	    fprintf(f,"\" -> \"");
	    expr_print(f,contents_next);
	    fprintf(f,"\" [label=\"%s\"];\n",sdecon.name);
	  }
	} 
      }

	
    }
  }
  fprintf(f,"\n}");
}
    

// Note that node n was at the source end of an erroneous subtype edge
void add_erroneous_source(mr_dyck_node n)
{
#ifdef DEBUG_ERRONEOUS_EDGES
  assert(mr_erroneous_sources);
  mr_dyck_node_list_cons(n,mr_erroneous_sources);
#endif /* DEBUG_ERRONEOUS_EDGES */
}
// Note that node n was the sink of all erroneous subtype edges
void set_erroneous_sink(mr_dyck_node n)
{
#ifdef DEBUG_ERRONEOUS_EDGES
  mr_erroneous_sink = n;
#endif /* DEBUG_ERRONEOUS_EDGES */
}
