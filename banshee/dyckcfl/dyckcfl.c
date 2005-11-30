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
#include "dyckcfl.h"
#include "nonspec.h"
#include "hash.h"

#define CONS_OPEN "open"
#define CONS_CLOSE "close"
#define CONS_OPEN_CLUSTER "opencluster"

//  #define DYCK_DOT_DEBUG		

/*****************************************************************************
 *                                                                           *
 *   Type Declarations                                                       *
 *                                                                           *
 *****************************************************************************/

/* NOTE: the states aren't relevant. This solver is completely online */
typedef enum {dyck_raw = 0, dyck_inited = 0, dyck_query = 0} dyck_state;

struct dyck_node_ {
  gen_e node_constant;
  gen_e node_variable;
  char *unique_name;
};

// An open constructor cluster
struct cluster_cons_ {
  constructor c;
  int length;
  int *indices;
};

typedef struct cluster_cons_ *cluster_cons;

DEFINE_NONPTR_LIST(dyck_node_list,dyck_node);

/*****************************************************************************
 *                                                                           *
 *   Global Variables                                                        *
 *                                                                           *
 *****************************************************************************/

#ifdef DYCK_DOT_DEBUG
static FILE *dotfile = NULL;
#endif

// A map from indices to the clustered open constructor containing that index 
static hash_table constant_to_node_map = NULL;
static hash_table clustered_indices = NULL;
static hash_table unclustered_indices = NULL;
static hash_table built_constructors = NULL; 
static region dyckregion = NULL;
static dyck_state state;
static constructor p_constructor = NULL;
static bool pn_reach = FALSE;
static cons_group n_group = NULL;
static cons_group n_cluster_group = NULL;

int flag_dyck_print_constraints = 0;

static dyck_node_list all_nodes = NULL;

/*****************************************************************************
 *                                                                           *
 *   Static Utility Functions                                                *
 *                                                                           *
 *****************************************************************************/

static void my_call_setif_inclusion(gen_e e1, gen_e e2)
{
  assert(state == dyck_inited);
  if (flag_dyck_print_constraints) {
    expr_print(stdout,e1);
    printf(" <= " );
    expr_print(stdout,e2);
    printf("\n");
  }
  call_setif_inclusion(e1,e2);
}

static constructor get_constructor(int index) 
{
  constructor result = NULL;
  
  sig_elt c_sig[1] = {{vnc_pos,setif_sort}};

  // Check the hash to see if the constructor has been built yet
  // If not, build the constructor
  if (!hash_table_lookup(built_constructors,(void *)index,(hash_data *)&result)) {
    result = make_constructor(CONS_OPEN,setif_sort,c_sig,1);
    // Make it part of the n group
    cons_group_add(n_group,result);
    // Hash it so that it can be be retrieved later
    hash_table_insert(built_constructors,(void *)index,result);
  }

  assert(hash_table_lookup(built_constructors,(void *)index,NULL));
  assert(result);
  return result;
}

/*****************************************************************************
 *                                                                           *
 *   Initialization/ Resetting                                               *
 *                                                                           *
 *****************************************************************************/

void dyck_init(bool pn)
{
  sig_elt p_sig[1] = {{vnc_pos,setif_sort}};

  assert(state == dyck_raw);
  
  dyckregion = newregion();

#ifdef DYCK_DOT_DEBUG
  dotfile = fopen("dyckcfl-test.dot","w");
  fprintf(dotfile,"digraph G {\n");
  fprintf(dotfile,"size=\"8,10\";\n");
#endif
  pn_reach = pn;
  n_group = make_cons_group("ngroup",p_sig,1);
  n_cluster_group = make_cons_group("nclustergroup",p_sig,-1);
  clustered_indices = make_hash_table(dyckregion, 32, ptr_hash, ptr_eq);
  unclustered_indices = make_hash_table(dyckregion, 32, ptr_hash, ptr_eq);
  built_constructors = make_hash_table(dyckregion, 32, ptr_hash, ptr_eq);
  constant_to_node_map = make_hash_table(dyckregion, 32, ptr_hash, ptr_eq);
  p_constructor = make_constructor(CONS_CLOSE,setif_sort,p_sig,1);
  all_nodes = new_dyck_node_list(dyckregion);
  state = dyck_inited;
}

void dyck_reset(void)
{
  deleteregion(dyckregion);
  clustered_indices = NULL;
  unclustered_indices = NULL;
  built_constructors = NULL;
  dyckregion = NULL;
  pn_reach = FALSE;
  p_constructor = NULL;
  all_nodes = NULL;
  state = dyck_raw;
}

/*****************************************************************************
 *                                                                           *
 *   Graph Construction API                                                  *
 *                                                                           *
 *****************************************************************************/

dyck_node make_tagged_dyck_node(const char *name)
{
  static int count = 0;
  char unique_name[512];
  dyck_node result = ralloc(dyckregion,struct dyck_node_);
  assert(state == dyck_inited);

  snprintf(unique_name,512,"%s_%d",name,count++);
  result->node_constant = setif_constant(unique_name);
  result->node_variable = setif_fresh(name);
  result->unique_name = rstrdup(dyckregion,unique_name);
  my_call_setif_inclusion(result->node_constant, result->node_variable);

  dyck_node_list_cons(result, all_nodes);
  hash_table_insert(constant_to_node_map, (hash_key) result->node_constant,
		    (hash_data)result);


  return result;
}

dyck_node make_untagged_dyck_node(const char *name)
{
  static int count = 0;
  char unique_name[512];
  dyck_node result = ralloc(dyckregion,struct dyck_node_);
  assert(state == dyck_inited);

  snprintf(unique_name,512,"%s_%d",name,count++);
  result->node_constant = setif_constant(unique_name);
  result->node_variable = setif_fresh(name);
  result->unique_name = rstrdup(dyckregion,unique_name);

  dyck_node_list_cons(result, all_nodes);
  hash_table_insert(constant_to_node_map, (hash_key) result->node_constant,
		    (hash_data)result);


  return result;
}

dyck_node make_tagged_empty_dyck_node(const char *name)
{
  dyck_node result = ralloc(dyckregion,struct dyck_node_);
  assert(state == dyck_inited);
  result->node_constant = setif_zero();
  result->node_variable = setif_fresh(name);
  // switch the inclusion, since putting 0 in the lower bounds has no effect
  my_call_setif_inclusion(result->node_variable, result->node_constant);

  dyck_node_list_cons(result, all_nodes);

  return result;
}

dyck_node make_tagged_universal_dyck_node(const char *name)
{
  dyck_node result = ralloc(dyckregion,struct dyck_node_);
  assert(state == dyck_inited);
  result->node_constant = setif_one();
  result->node_variable = setif_fresh(name);
  my_call_setif_inclusion(result->node_constant, result->node_variable);

  dyck_node_list_cons(result, all_nodes);

  return result;
}

void mark_dyck_node_global(dyck_node n)
{
  assert(n);
  assert(state == dyck_inited);

  my_call_setif_inclusion(n->node_variable,setif_group_proj_pat(n_group,-1,n->node_variable));
}

void make_dyck_subtype_edge(dyck_node n1, dyck_node n2)
{
  assert(state == dyck_inited);
  my_call_setif_inclusion(n1->node_variable, n2->node_variable);

#ifdef DYCK_DOT_DEBUG
  fprintf(dotfile,"\"%s\" -> \"%s\" [label=\"s\"];\n",n1->unique_name,n2->unique_name);
#endif
}

void make_dyck_open_edge(dyck_node n1, dyck_node n2, int index)
{
  gen_e exps[1];
  constructor c = get_constructor(index);
  assert(state == dyck_inited);
  exps[0] = n1->node_variable;
  // Make the constructed term and add the inclusion constraint
  my_call_setif_inclusion(constructor_expr(c,exps,1), n2->node_variable);

#ifdef DYCK_DOT_DEBUG
  fprintf(dotfile,"\"%s\" -> \"%s\" [label=\"(_%d\"];\n",n1->unique_name,n2->unique_name,index);
#endif
}

// n-edges for pn reachability (until a better method is devised)
static void make_dyck_p_edge(dyck_node n1, dyck_node n2)
{
  gen_e exps[1];
  assert(state == dyck_inited);
  assert(p_constructor);

  exps[0] = n1->node_variable;

  my_call_setif_inclusion(constructor_expr(p_constructor,exps,1),n2->node_variable);
}


void make_dyck_close_edge(dyck_node n1, dyck_node n2, int index)
{
  constructor c = get_constructor(index);
  assert(state == dyck_inited);
  my_call_setif_inclusion(n1->node_variable,setif_proj_pat(c, 0, n2->node_variable));
  if (pn_reach) make_dyck_p_edge(n1, n2);

#ifdef DYCK_DOT_DEBUG
  fprintf(dotfile,"\"%s\" -> \"%s\" [label=\")_%d\"];\n",n1->unique_name,n2->unique_name,index);
#endif
}

// FIX: Clusters must be the same size. 
void make_clustered_dyck_open_edges(dyck_node n1s[], dyck_node n2,
				    int indices[], int length)
{
  // This is a degenerate cluster: mark it as such and go on
  if (length == 1) {
    hash_table_insert(unclustered_indices, (void *)indices[0], 
		      (void *) indices[0]);
    make_dyck_open_edge(n1s[0],n2,indices[0]);
  }
  else {
    cluster_cons last_result = NULL, result = NULL;
    int i,j;
    gen_e exprs[length];

    // Safety check: if any of the indices occured before, they must
    // have occurred in a cluster with exactly the same indices
    for (i = 0; i < length; i++) {
      if (hash_table_lookup(clustered_indices, (void *)indices[i], (hash_data *)&result)) {
	if (last_result) { assert(last_result == result); }
	last_result = result;
#ifdef DYCK_DOT_DEBUG
	fprintf(dotfile,"\"%s\" -> \"%s\" [label=\"(_%d\"];\n",n1s[i]->unique_name,n2->unique_name,indices[i]);
#endif
      }
    }

    // Safety check: no index should appear more than once 
    for (i = 0; i < length; i++) {
      exprs[i] = n1s[i]->node_variable;
      for (j = i+1; j < length; j++) {
	assert(indices[i] != indices[j]);
      }
    }
  
    // First time we've seen this cluster, so make a constructor for it
    if (result == NULL) {
      sig_elt oc_sig[length];
      for (i = 0; i < length; i++) {
	oc_sig[i].variance = vnc_pos;
	oc_sig[i].sort = setif_sort;
      }
      result = ralloc(dyckregion, struct cluster_cons_);
      result->c = make_constructor(CONS_OPEN_CLUSTER,setif_sort,oc_sig,length);
      result->length = length;
      result->indices = rarrayalloc(dyckregion,length,int);
      rarraycopy(result->indices,indices,length,int);

      for (i = 0; i < length; i++) {
	hash_table_insert(clustered_indices, (void *)indices[i], result);
      }

    }
    // This cluster isn't the same size as the last, so make
    // a declustered version
    if (result->length != length) {
      printf("Warning: forced to uncluster a length %d cluster.\n",length);
      for (i = 0; i < length; i++) {
	hash_table_insert(unclustered_indices, (void *)indices[i], 
			  (void *) indices[i]);
	make_dyck_open_edge(n1s[i],n2,indices[i]);
      }

    }
    else {
      my_call_setif_inclusion(constructor_expr(result->c,exprs,length), 
			      n2->node_variable);
    }
  }
}

static bool is_unclustered_index(int index)
{
  return hash_table_lookup(unclustered_indices,(void *)index,NULL);
}

static cluster_cons get_cluster_constructor(int index)
{
  cluster_cons result = NULL;

  hash_table_lookup(clustered_indices,(void *)index,(hash_data *) &result);

  // FIX : put a warning here, if the thing doesn't exist

  return result;
}

static int get_position_of(cluster_cons cc, int index)
{
  int i;
  for (i = 0; i < cc->length; i++) {
    if (cc->indices[i] == index) return i; 
  }
  assert(0);
  return -1;
}

void make_dyck_close_edge_for_cluster(dyck_node n1, dyck_node n2, int index)
{
  cluster_cons cc = get_cluster_constructor(index);

  assert(state == dyck_inited);

  if (cc) {
    my_call_setif_inclusion(n1->node_variable,
			    setif_proj_pat(cc->c, 
					   get_position_of(cc,index),
					   n2->node_variable));
  }
  // If at any point we unclustered the index, we also have to 
  // make a close edge
  if (is_unclustered_index(index)) {
    make_dyck_close_edge(n1,n2,index);
  }
  else if (pn_reach) make_dyck_p_edge(n1, n2);


#ifdef DYCK_DOT_DEBUG
  fprintf(dotfile,"\"%s\" -> \"%s\" [label=\")_%d\"];\n",n1->unique_name,n2->unique_name,index);
#endif  
}

/*****************************************************************************
 *                                                                           *
 *   Query API                                                               *
 *                                                                           *
 *****************************************************************************/

void dyck_finished_adding()
{
  assert(state == dyck_inited);
  state = dyck_query;

#ifdef DYCK_DOT_DEBUG
  fprintf(dotfile,"\n}");
  fclose(dotfile);
#endif

  return;
}

bool dyck_check_reaches(dyck_node n1, dyck_node n2)
{  
  gen_e_list_scanner scan;
  gen_e next_lb;
  // compute the transitive lower bounds of n2
  gen_e_list tlb = setif_tlb(n2->node_variable);
  
  assert(state == dyck_query);
  
  // check whether n1's constant is a member of n2
  gen_e_list_scan(tlb,&scan);
  
  while (gen_e_list_next(&scan,&next_lb)) {
    if (expr_eq(n1->node_constant,next_lb)) return TRUE;
  }

  return FALSE;
}

// Search for target's pn reachability starting from current, assuming
// that nodes in visited have already been searched
static bool dyck_check_pn_reaches_aux(gen_e target, gen_e current,
				      hash_table visited_n, 
				      hash_table visited_p,
				      bool seen_p,
				      dyck_node_list all_constants)
{
  hash_table visited = seen_p ? visited_p : visited_n;
  
  // Found the target
  if (target == NULL && expr_is_constant(current)) {
    dyck_node n = NULL;
    assert(all_constants);
    insist(hash_table_lookup(constant_to_node_map,
			     (hash_key)current,
			     (hash_data *)&n));
    assert(n);
    dyck_node_list_cons(n, all_constants);
    
  }
  else if (target && expr_eq(target,current)) return TRUE;
  
  // Already searched from this point
  if (hash_table_lookup(visited,(void *)expr_stamp(current),NULL)) return FALSE;
  
  // Otherwise, mark this node visited
  hash_table_insert(visited,(void *)expr_stamp(current),(void *)expr_stamp(current));

  // Compute this node's transitive lower bounds
  {
    gen_e_list_scanner scan;
    gen_e next_lb;
    gen_e_list tlb = setif_tlb(current);
    
    // Scan the lower bounds
    gen_e_list_scan(tlb,&scan);

    while (gen_e_list_next(&scan,&next_lb)) {
      struct decon contents;

      // If target is null, we're finding all PN reaches
      if (target == NULL && expr_is_constant(next_lb)) {
	dyck_node n = NULL;
	assert(all_constants);
	insist(hash_table_lookup(constant_to_node_map,
				 (hash_key)next_lb,
				 (hash_data *)&n));
	assert(n);
	dyck_node_list_cons(n, all_constants);
      }
      
      // Again, we've found the target
      else if (expr_eq(target,next_lb)) return TRUE;

      // Deconstruct any p's 
      // and search their contents
      contents = deconstruct_any_expr(next_lb);
      
      if (!seen_p && contents.name && string_eq(contents.name,CONS_OPEN) ) {
	assert(contents.arity == 1);
	assert(contents.elems);

	if (dyck_check_pn_reaches_aux(target,contents.elems[0],visited_n,
				      visited_p,FALSE, all_constants)) {
	  if (target) return TRUE;
	}
      }
      else if (contents.name && string_eq(contents.name,CONS_CLOSE)) {
	assert(contents.arity == 1);
	assert(contents.elems);

	if (dyck_check_pn_reaches_aux(target,contents.elems[0],visited_n,
				      visited_p,TRUE, all_constants)) {
	  if (target) return TRUE;
	}
      }
      
   
    }
  } 
  // We didn't find the target on this search
  return FALSE;
}

bool dyck_check_pn_reaches(dyck_node n1, dyck_node n2)
{
  region scratch = NULL;
  hash_table visited = NULL;
  hash_table visited_p = NULL;

  assert (state == dyck_query);

  if (!pn_reach) fail("PN reachability not enabled.");

  scratch = newregion();
  visited = make_hash_table(scratch, 32, ptr_hash, ptr_eq);
  visited_p = make_hash_table(scratch, 32, ptr_hash, ptr_eq);

  return dyck_check_pn_reaches_aux(n1->node_constant,n2->node_variable,visited,
				   visited_p, FALSE, NULL);

  deleteregion(scratch);
}

dyck_node_list dyck_reaches(dyck_node n)
{
  dyck_node_list result;
  gen_e_list_scanner scan;
  gen_e next_lb;
  // compute the transitive lower bounds of n
  gen_e_list tlb = setif_tlb(n->node_variable);
  
  assert(state == dyck_query);
  
  result = new_dyck_node_list(dyckregion);

  gen_e_list_scan(tlb,&scan);
  
  while (gen_e_list_next(&scan,&next_lb)) {
    if (expr_is_constant(next_lb)) {
      dyck_node next_node = NULL;
      insist(hash_table_lookup(constant_to_node_map, 
			       (hash_key)next_lb, (hash_data *)&next_node));
      assert(next_node);
      dyck_node_list_cons(next_node,result);
    }
  }

  return result;

}

dyck_node_list dyck_pn_reaches(dyck_node n)
{
  region scratch = NULL;
  hash_table visited = NULL;
  hash_table visited_p = NULL;
  dyck_node_list all_constants = NULL;

  assert (state == dyck_query);

  if (!pn_reach) fail("PN reachability not enabled.");

  scratch = newregion();
  visited = make_hash_table(scratch, 32, ptr_hash, ptr_eq);
  visited_p = make_hash_table(scratch, 32, ptr_hash, ptr_eq);
  all_constants = new_dyck_node_list(dyckregion);

  dyck_check_pn_reaches_aux(NULL, n->node_variable,visited,
			    visited_p, FALSE, all_constants);

  deleteregion(scratch);

  return all_constants;
}



// Print (in dot format) a representation of the closed CFL graph, w/o
// PN edges
void dyck_print_closed_graph(FILE *f)
{
  dyck_node_list_scanner anscan;
  dyck_node next_node;

  assert(state == dyck_query);
  fprintf(f,"digraph G {\n");
  fprintf(f,"size=\"8,10\";\n");


  dyck_node_list_scan(all_nodes,&anscan);
  
  while(dyck_node_list_next(&anscan,&next_node)) {
    gen_e_list_scanner scan;
    gen_e next_lb;
    
    gen_e_list tlb = setif_tlb(next_node->node_variable);
    gen_e_list_scan(tlb,&scan);
    while (gen_e_list_next(&scan,&next_lb)) {
      if (expr_is_constant(next_lb)) {
	fprintf(f,"\"");
	expr_print(f,next_lb);
	fprintf(f,"\" -> \"");
	expr_print(f,next_node->node_constant);
	fprintf(f,"\" [label=\"%s\"];\n","s");
      }
    }
  }
  fprintf(f,"\n}");
}

// // Return the list of nodes that reach this one
// string_list dyck_get_reaches_list(dyck_node n)
// {
//   string_list result = new_string_list(dyckregion);
//   gen_e_list_scanner scan;
//   gen_e next_lb;

//   // compute the transitive lower bounds of n2
//   gen_e_list tlb = setif_tlb(n->node_variable);
  
//   assert(state == dyck_query);
  
//   // check whether n1's constant is a member of n2
//   gen_e_list_scan(tlb,&scan);
  
//   while (gen_e_list_next(&scan,&next_lb)) {
//     if (expr_is_constant(next_lb)) string_list_cons(expr_constant_name(next_lb),result);
//   }

//   return result;
// }

// // Return the list of nodes that pn reach this one
// string_list dyck_get_pn_reaches_list(dyck_node n)
// {
//   // FIX : todo
//   assert(FALSE);
//   return NULL;
// }
