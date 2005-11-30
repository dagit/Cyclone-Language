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
#include <stdlib.h>		// random, srandom, atoi
#include <bool.h>	// bool
#include <sys/time.h>
#include "nonspec.h"
#include "dyckcfl.h"
#include "mr_dyckcfl.h"

/* Change these to enable/disable various debugging options */

// Dump the graph out in dot format
// #define DOT_DEBUG

// Enable verbose debugging, and DOT output
// #define DYCK_RANDOM_DEBUG

// Enable the Melski-Reps implementation
#define ENABLE_MR_DYCK
// Enable my implementation
#define ENABLE_DYCK

// Enable global marking
#define MARK_GLOBALS

// If both implementations are enabled,
// turn on the consistency checks
#ifdef ENABLE_MR_DYCK
#ifdef ENABLE_DYCK
#define ENABLE_CONSISTENCY_CHECK
#endif
#endif

#ifdef DYCK_RANDOM_DEBUG
#define DOT_DEBUG
#define DEFAULT_SEED 42
#endif /* DYCK_RANDOM_DEBUG */


// Number of nodes in the graph
#define DEFAULT_NUM_NODES 50
#define NUM_INDICES 20

#define S_DENSITY 1
#define O_DENSITY 3
#define C_DENSITY 2
#define GLOBAL_DENSITY 5
#define NUM_CHECKS 500

#ifdef DOT_DEBUG
static FILE *outfile;
#endif

static int num_nodes;

#ifdef DYCK_RANDOM_DEBUG
DECLARE_LIST(relevant_edge_list,int);
DEFINE_LIST(relevant_edge_list,int);
static relevant_edge_list relevant_edges = NULL;
#endif
static int edge_count = 0;

// Ghetto seeding: good enough for this purpose
// Set it to a constant value for consistent output
static int seedRandom()
{
  int seed;
  struct timeval tv;
  struct timezone tz;

  gettimeofday(&tv,&tz);

// If debugging is enabled, use a constant seed to force deterministic behavior
#ifdef DYCK_RANDOM_DEBUG
  seed = DEFAULT_SEED;
#else
  seed = (tv.tv_usec);  
#endif
   srandom(seed);

   return seed;
}

// return TRUE if we should add an edge
static bool shouldAddSEdge()
{
  return (random() % 100 < S_DENSITY);
}

static bool shouldAddOEdge()
{
  return (random() % 100 < O_DENSITY);
}

static bool shouldAddCEdge()
{
  return (random() % 100 < C_DENSITY);
}

static bool shouldMarkGlobal()
{
  return (random() % 100 < GLOBAL_DENSITY);
}

static bool pickIndex()
{
  return (random() % NUM_INDICES);
}

static bool pickNode()
{
  return (random() % num_nodes );
}

#ifdef DYCK_RANDOM_DEBUG
static bool relevant_node(int i)
{
//   if (
//       (i == 0) ||
//       (i == 1) ||
//       (i == 2) ||
//       (i == 3) || 
//       //       (i == 4) ||
//       //       (i == 5) ||
//        (i == 6) ||
//       //       (i == 7) ||
//        (i == 8) ||
//       //       (i == 9) ||
//       //       (i == 10) ||
//       //       (i == 11) ||
//       (i == 12) || 
// //       (i == 13) ||
// //       (i == 14) || 
//        (i == 15) ||
//       // (i == 16) ||
//       // (i == 17) || 
//       (i == 18) ||
//       // (i == 19) ||
//       (i == 20) ||
// //       (i == 21) ||
// //       (i == 22) || 
// //       (i == 23) ||
//       (i == 24) ||
//       (i == 25) || 
// //       (i == 26) ||
// //       (i == 27) ||
//       // (i == 28) || 
//       // (i == 29) ||
//       (i == 30) ||
// //       (i == 31) ||
//       (i == 32) ||
// //       (i == 33) ||
//       // (i == 34) ||
// //       (i == 35) ||
//       // (i == 36) ||
//       (i == 37) ||
//       // (i == 38) ||
//       (i == 39) ||
//       (i == 40) ||
//       (i == 41) ||
//       // (i == 42) ||
//       (i == 43) ||
// //       (i == 44) ||
//       // (i == 45) ||
//       (i == 46) ||
// //       (i == 47) ||
//       // (i == 48) ||
//       // (i == 49) ||
// FALSE
//       ) return FALSE;

//   else

 return TRUE;
}


#endif

static bool relevant_edge(int edge_num)
{
#ifdef DYCK_RANDOM_DEBUG
  // If no file was specified, everything is relevant
  if (! relevant_edges)
    return TRUE;
  // Otherwise, read off the list of edges specified
  // If the edge is in the list, it is relevant
  {
    relevant_edge_list_scanner scan;
    int next;

    relevant_edge_list_scan(relevant_edges,&scan);
    
    while (relevant_edge_list_next(&scan,&next)) {
      if (next == edge_num) return TRUE;
    }
    
  }
  return FALSE;
#else
  return TRUE;
#endif
}
static bool relevant_pair(int i,int j)
{
#ifdef DYCK_RANDOM_DEBUG
  return (relevant_node(i) && relevant_node(j));
#else
  return TRUE;
#endif
}

static bool markGlobals()
{
#ifdef MARK_GLOBALS
  return TRUE;
#else
  return FALSE;
#endif
}

static void do_check_reach()
{
  int i = 0, j = 0, total = 0, pn_total = 0;
  int addedSEdges = 0,addedOEdges = 0,addedCEdges = 0;
  int markedGlobal = 0;
#ifdef ENABLE_DYCK
  dyck_node *vertices = calloc(num_nodes,sizeof(dyck_node));
#endif
#ifdef ENABLE_MR_DYCK
  mr_dyck_node *mr_vertices = calloc(num_nodes, sizeof(mr_dyck_node));
#endif


  printf("Building graphs...\n");

#ifdef DOT_DEBUG
  outfile = fopen("dyck-random.dot","w");
  fprintf(outfile,"digraph G {\n");
  fprintf(outfile,"size=\"8,10\";\n");
#endif

  // build the vertices
  for (i = 0; i < num_nodes; i++) {
    char name[512];
    snprintf(name,512,"n%d",i);

#ifdef ENABLE_DYCK
    vertices[i] = make_tagged_dyck_node(name);
#endif

#ifdef ENABLE_MR_DYCK
    mr_vertices[i] = make_tagged_mr_dyck_node(name);
#endif
  }

  // DEBUG!!
// #ifdef ENABLE_MR_DYCK
//   printf("Adding erroneous edge checks\n");
//   set_erroneous_sink(mr_vertices[25]);
//   add_erroneous_source(mr_vertices[0]);
//   add_erroneous_source(mr_vertices[13]);
//   add_erroneous_source(mr_vertices[16]);
//   add_erroneous_source(mr_vertices[17]);
//   add_erroneous_source(mr_vertices[18]);
//   add_erroneous_source(mr_vertices[21]);
//   add_erroneous_source(mr_vertices[22]);
//   add_erroneous_source(mr_vertices[29]);
//   add_erroneous_source(mr_vertices[32]);
//   add_erroneous_source(mr_vertices[38]);
//   add_erroneous_source(mr_vertices[40]);
//   add_erroneous_source(mr_vertices[4]);
//   add_erroneous_source(mr_vertices[7]);
//   add_erroneous_source(mr_vertices[30]);
// #endif


  // build open, close, and subtype edges according to density
  for (i = 0; i < num_nodes; i++) {
    if (shouldMarkGlobal() ) {
      if (relevant_pair(i,i) && markGlobals() ) {
	printf("Marking node %d global.\n",i);
	markedGlobal++;
#ifdef ENABLE_DYCK
	mark_dyck_node_global(vertices[i]);
#endif
#ifdef ENABLE_MR_DYCK
	mark_mr_dyck_node_global(mr_vertices[i]);
#endif
      }
    }
    for (j = 0; j < num_nodes; j++) {
      if (shouldAddSEdge()) {
	edge_count++;
	if (relevant_pair(i,j) && relevant_edge(edge_count) ) {
	  addedSEdges++;

#ifdef ENABLE_DYCK
	  make_dyck_subtype_edge(vertices[i],vertices[j]);
#endif
#ifdef ENABLE_MR_DYCK
	  make_mr_dyck_subtype_edge(mr_vertices[i],mr_vertices[j]);
#endif
#ifdef DOT_DEBUG
	fprintf(outfile,"n%d -> n%d [label=\"s\"];\n",i,j);
#endif
	}
      }
      if (shouldAddOEdge()) {
	int index = pickIndex();
	assert(index < NUM_INDICES);
	edge_count++;
	if (relevant_pair(i,j) && relevant_edge(edge_count) ) {
	  addedOEdges++;
#ifdef ENABLE_DYCK
	  make_dyck_open_edge(vertices[i],vertices[j],index);
#endif
#ifdef ENABLE_MR_DYCK
	  make_mr_dyck_open_edge(mr_vertices[i],mr_vertices[j],index);
#endif
#ifdef DOT_DEBUG
	fprintf(outfile,"n%d -> n%d [label=\"(_%d\"];\n",i,j,index);
#endif
	}
      }
      if (shouldAddCEdge()) {
	int index = pickIndex();
	assert(index < NUM_INDICES);
	edge_count++;
	if (relevant_pair(i,j) && relevant_edge(edge_count) ) {
	  addedCEdges++;
#ifdef ENABLE_DYCK
	  make_dyck_close_edge(vertices[i],vertices[j],index);
#endif
#ifdef ENABLE_MR_DYCK
	  make_mr_dyck_close_edge(mr_vertices[i],mr_vertices[j],index);
#endif
#ifdef DOT_DEBUG
	fprintf(outfile,"n%d -> n%d [label=\")_%d\"];\n",i,j,index);
#endif
	}
      }
    }
  }

#ifdef DOT_DEBUG
  fprintf(outfile,"\n}");
  fclose(outfile);
#endif
  
  printf("Added %d S-edges, %d O-edges, and %d C-edges between %d nodes.\n",
	 addedSEdges,addedOEdges,addedCEdges,num_nodes);

  printf("Marked %d nodes global.\n",markedGlobal);

  printf("Encoding the productions...\n");

#ifdef ENABLE_DYCK
  dyck_finished_adding();
#endif
#ifdef ENABLE_MR_DYCK
  mr_dyck_finished_adding();
#endif

#ifdef ENABLE_CONSISTENCY_CHECK
  printf("Checking consistency...\n");
#endif

  // Now check consistency of random reachability queries
  for (i = 0; i < NUM_CHECKS; i++) {
    int check1 = 0, check2 = 0;

#ifdef ENABLE_DYCK
    int result1 = 0;
    int pnresult1 = 0;
#endif

#ifdef ENABLE_MR_DYCK
    int result2 = 0;
    int pnresult2 = 0;
#endif

    check1 = pickNode();
    check2 = pickNode();

#ifdef ENABLE_DYCK
    result1 = 
      dyck_check_reaches(vertices[check1],vertices[check2]);
    pnresult1 =
      dyck_check_pn_reaches(vertices[check1],vertices[check2]);
#endif
    
#ifdef ENABLE_MR_DYCK
    result2 =
      mr_dyck_check_reaches(mr_vertices[check1],mr_vertices[check2]);
    pnresult2 = 
      mr_dyck_check_pn_reaches(mr_vertices[check1],mr_vertices[check2]);
#endif


#ifdef ENABLE_CONSISTENCY_CHECK
    // reachability answers must be consistent between the isomorphism and reduction
    assert(pnresult1 == pnresult2);
    assert(result1 == result2);

    // matched reachability implies PN reachability
    assert(!result1 || pnresult1);
    assert(!result2 || pnresult2);
#endif
    
#ifdef ENABLE_DYCK
    pn_total += pnresult1;
    total += result1;
#endif

#ifdef ENABLE_MR_DYCK
#ifndef ENABLE_DYCK
    pn_total += pnresult2;
    total += result2;
#endif
#endif

  }

#ifdef ENABLE_CONSISTENCY_CHECK
  printf("Graphs are consistent.\n");
#endif

  printf("Of %d checks, %d pairs reached by matched paths.\n",NUM_CHECKS,total);
  printf("Of %d checks, %d pairs reached by P-N paths.\n",NUM_CHECKS,pn_total);
}

int main(int argc, char **argv)
{
  FILE *rconstraints = NULL;
  int seed = 0;
  if (argc > 1) {
    num_nodes = atoi(argv[1]);
  }
  else {
    num_nodes = DEFAULT_NUM_NODES;
  }
  if (argc > 2) {
    seed = atoi(argv[2]); 
    srandom(seed);
  }
  else {
    seed = seedRandom();
  }

  printf("Number of nodes is %d, seed is %d.\n", num_nodes, seed);

  nonspec_init();
  flag_merge_projections = FALSE;
  

#ifdef DYCK_RANDOM_DEBUG
  {
    FILE *redges = NULL;
    
    if (argc > 3 )
      redges = fopen(argv[3],"r");
    else
      redges = fopen("relevant_edges.tmp","r");
    if (redges) {
      char buf[100];
      region scratch = newregion();
      relevant_edges = new_relevant_edge_list(scratch);
      
      while (fgets(buf,100,redges)) {
	int next_relevant = atoi(buf);
	assert(next_relevant);
	//	printf("%d\n",next_relevant);
	relevant_edge_list_cons(next_relevant, relevant_edges);
      }
      fclose(redges);
    }
  }

  {
    if (argc > 4) 
      rconstraints = fopen(argv[4],"r");
  }

#endif

  dyck_init(TRUE);
  mr_dyck_init(TRUE,rconstraints);
  
  
  // flag_mr_dyck_print_constraints = 1;
  do_check_reach();

#ifdef DYCK_RANDOM_DEBUG
{
  printf("Fooey\n");
  FILE *f = fopen("closed_graph.dot","w");
  FILE *mr_f = fopen("mr_closed_graph.dot","w");
  assert(f);
  assert(mr_f);

#ifdef ENABLE_DYCK
  dyck_print_closed_graph(f);
#endif
#ifdef ENABLE_MR_DYCK
  mr_dyck_print_closed_graph(mr_f);
#endif
  fclose(f);
  fclose(mr_f);
}
#endif


  printf("Exiting...\n");

  return 0;
}
