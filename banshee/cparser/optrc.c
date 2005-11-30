/* This file is part of the RC compiler.
   Copyright (C) 2000-2001 The Regents of the University of California.

RC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

RC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with RC; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

#include <limits.h>

#include "parser.h"
#include "rc.h"
#include "optrc.h"
#include "cfg.h"

double largest_optrc;

node collapse(node super, node n)
{
  node oldnext;

  if (!super)
    {
      assert(!n->vertex && !n->vertex_next);
      return n;
    }

  assert(!n->vertex && !super->vertex && n != super);

  /* Collapse n, and update list of nodes collapsed to 'super' */

  oldnext = super->vertex_next;
  super->vertex_next = n;

  /* If stuff is collapsed to n, we need to now collapse it to super */
  while (n->vertex_next) {
    n->vertex = super;
    n = n->vertex_next;
  }
  n->vertex = super;
  n->vertex_next = oldnext;

  return super;
}

static __inline node realnode(node n)
{
  if (n->vertex)
    return n->vertex;
  else
    return n;
}

void clear_collapsed_graph(function_decl fd)
{
  node parentptr *gnodes = fd->postorder_nodes;
  int i;

  /* Clear any previous collapsed graph */
  for (i = 0; i < fd->postorder_size; i++)
    gnodes[i]->vertex = gnodes[i]->vertex_next = NULL;
}

bool collapse_graph(function_decl fd, data_declaration local,
		    node *white, node *black)
{
  node parentptr *gnodes = fd->postorder_nodes;
  node superwhite = NULL, superblack = NULL;
  int i, lid = local->id;

  assert(lid >= 0);

  /* White: RC excluded. Black: RC included */

  /* Function entry is white */
  superwhite = collapse(superwhite, fd->cfg_entry);
  /* So is exit, but all vars dead on exit so automatically handled */

  for (i = 0; i < fd->postorder_size; i++)
    {
      node n = gnodes[i];

      if (n == fd->cfg_entry)
	continue; /* Already processed */

      /* White if not live on entry or on exit to/from n */
      if (!BIT_SETP(n->live_in, lid) || !BIT_SETP(n->live_out, lid))
	superwhite = collapse(superwhite, n);
      else if (is_assign(n))
	{
	  /* Assignments to local are white. Normally these are caught
	     by the liveness tests, except for weird cases like x=x */
	  assign a = CAST(assign, n);

	  if (is_identifier(a->arg1))
	    {
	      data_declaration id = CAST(identifier, a->arg1)->ddecl;

	      if (id == local)
		superwhite = collapse(superwhite, n);
	    }
	}
      /* Only calls to 'deletes' function where local is live before and after are black */
      else if (is_function_call(n) &&
	       type_deletes(function_call_type(CAST(function_call, n))))
	superblack = collapse(superblack, n);

      /* Everything else is gray */
    }

  *white = superwhite;
  *black = superblack;

  /* Nothing to do if no black nodes */
  return superblack != NULL;
}

long collapsed_cfg_size(function_decl fd)
{
  node parentptr *gnodes = fd->postorder_nodes;
  long i, nnodes = fd->postorder_size, size = 0;

  for (i = 0; i < nnodes; i++)
    if (!gnodes[i]->vertex)
      size++;

  return size;
}

static bool all_pred_equal(node n, node equalto)
{
  edge e;

  for (e = n->edges_in; e; e = e->next_in)
    if (realnode(e->from) != equalto)
      return FALSE;

  return TRUE;
}

static bool all_succ_equal(node n, node equalto)
{
  edge e;

  for (e = n->edges_out; e; e = e->next_out)
    if (realnode(e->to) != equalto)
      return FALSE;

  return TRUE;
}

static long mark_grey(node n, node white, node black)
{
  edge in, out;
  long cnt = 0;
  node rn;

  if (vertex_marked(n) || n->vertex || n == white || n == black || !n->in_postorder)
    return 0;

  mark_vertex(n);

  for (rn = n; rn; rn = rn->vertex_next)
    {
      for (out = rn->edges_out; out; out = out->next_out)
	cnt += mark_grey(realnode(out->to), white, black);

      for (in = rn->edges_in; in; in = in->next_in)
	cnt += mark_grey(realnode(in->from), white, black);
    }

  return cnt + 1;
}

void count_grey_components(function_decl fd, node white, node black)
{
  node parentptr *gnodes = fd->postorder_nodes;
  long nnodes = fd->postorder_size, i;

  clear_all_marks();
  for (i = 0; i < nnodes; i++)
    {
      node n = gnodes[i];

      if (!(n->vertex || n == white || n == black || vertex_marked(n)))
	{
	  long compsize = mark_grey(n, white, black);

	  fprintf(stderr, "grey component of size %ld\n", compsize);
	}
    }
}

static void simplify_graph(function_decl fd, node white, node black)
{
  node parentptr *gnodes = fd->postorder_nodes;
  long nnodes = fd->postorder_size, i;
  bool change;

#ifdef DPRINT
  fprintf(stderr, "cfg size for %s: %ld, collapses to %ld\n",
	  fd->ddecl->name, nnodes, collapsed_cfg_size(fd));
#endif

  do {
    change = FALSE;

    for (i = 0; i < nnodes; i++)
      {
	node n = gnodes[i];

	if (!(n->vertex || n == white || n == black))
	  {
	    edge e;

	    if (all_pred_equal(n, white))
	      {
		collapse(white, n);
		change = TRUE;
	      }
	    else if (all_pred_equal(n, black))
	      {
		collapse(black, n);
		change = TRUE;
	      }
	    else if (all_succ_equal(n, white))
	      {
		collapse(white, n);
		change = TRUE;
	      }
	    else if (all_succ_equal(n, black))
	      {
		collapse(black, n);
		change = TRUE;
	      }
#if 1
	    else 
	      /* if there is exactly one edge leaving X, and this is the only
		 edge to Y, then X and Y can be collapsed. */
	      if ((e = n->edges_out) && !e->next_out)
		{
		  node to = realnode(e->to);
		  if (!to->edges_in->next_in && to != n)
		    {
		      /* to cannot be white or black: the all_pred/succ tests
			 catch that case */
		      collapse(n, to);
		      change = TRUE;
		    }
		}
#endif
	  }
      }
  }
  while (change);
      
#ifdef DPRINT
  fprintf(stderr, "simplifies to %ld\n", collapsed_cfg_size(fd));
  count_grey_components(fd, white, black);
#endif
}

void max_flow(int nodecount, node parentptr *gnodes, node source, node sink)
{
  int i;
  int nnodes = 0;
  edge e;
  node n;
  bool active;

  /* mincut on the collapsed graph, viewed as undirected (this occasions minor
     differences from the directed version, which are noted below) */

  /* Initialize preflow */
  for (i = 0; i < nodecount; i++)
    {
      node n = gnodes[i];

      if (!n->vertex) /* A non-collapsed node */
	{
	  n->mf_height = 0;
	  n->mf_excess = 0;
	  nnodes++;
	}
      for (e = n->edges_out; e; e = e->next_out)
	{
	  e->mf_flow = 0;
	  assert(e->ecount >= 0 && e->ecount < INT_MAX / 2); /* Problems otherwise */
	}
    }
  source->mf_height = nnodes;
  /* Push out from source */
  for (n = source; n; n = n->vertex_next)
    {
      /* Pushes through source self-edges, but this is of no import */
      for (e = n->edges_out; e; e = e->next_out)
	{
	  e->mf_flow = e->ecount;
	  e->to->mf_excess += e->ecount;
	}
      /* Extra for undirected graph */
      /* This is done for edges to dead code, but does no harm */
      for (e = n->edges_in; e; e = e->next_in)
	{
	  e->mf_flow = -e->ecount;
	  e->from->mf_excess += e->ecount;
	}
    }

  /* Generic preflow-push algo (complexity V^2*E) */
  do
    {
      active = FALSE;

      /* Do all possible pushes */
      for (i = 0; i < nodecount; i++)
	{
	  node n = gnodes[i];
	  node rn = n;
	  bool pushed;

	  if (n->vertex || !n->mf_excess || n->mf_height >= nnodes ||
	      n == sink || n == source)
	    continue;		/* No overflow */

	  active = TRUE;

	  pushed = FALSE;
	  for (; n; n = n->vertex_next)
	    {
	      for (e = n->edges_out; e; e = e->next_out)
		{
		  int residual = e->ecount - e->mf_flow;
		  node rto = realnode(e->to);

		  assert(residual >= 0);
		  if (residual && rto != rn && rn->mf_height == 1 + rto->mf_height)
		    {
		      /* Push */
		      int delta = residual > rn->mf_excess ? rn->mf_excess : residual;

		      e->mf_flow = e->mf_flow + delta;
		      rn->mf_excess -= delta;
		      rto->mf_excess += delta;
		      pushed = TRUE;
		    }
		}

	      /* Ick! cut&paste programming */
	      for (e = n->edges_in; e; e = e->next_in)
		{
		  /* For directed graph: residual = e->mf_flow (base capacity is 0) */
		  int residual = e->ecount + e->mf_flow;
		  node rfrom = realnode(e->from);

		  /* Don't follow edges back into dead code */
		  if (!rfrom->in_postorder)
		    continue;

		  assert(residual >= 0);
		  if (residual && rfrom != rn && rn->mf_height == 1 + rfrom->mf_height)
		    {
		      /* Push */
		      int delta = residual > rn->mf_excess ? rn->mf_excess : residual;

		      e->mf_flow = e->mf_flow - delta;
		      rn->mf_excess -= delta;
		      rfrom->mf_excess += delta;
		      pushed = TRUE;
		    }
		}
	    }
	  if (!pushed)
	    {
	      int minheight;

	      /* Lift this node (must be possible because there is excess) */
	      minheight = 2 * nnodes; /* actual max in graph is 2nnodes - 1 */
	      for (n = gnodes[i]; n; n = n->vertex_next)
		{
		  for (e = n->edges_out; e; e = e->next_out)
		    {
		      int residual = e->ecount - e->mf_flow;

		      if (residual)
			{
			  node rto = realnode(e->to);

			  if (rto->mf_height < minheight)
			    minheight = rto->mf_height;
			}
		    }

		  /* Ick! cut&paste programming */
		  for (e = n->edges_in; e; e = e->next_in)
		    {
		      /* For directed graph: residual = e->mf_flow */
		      int residual = e->ecount + e->mf_flow;

		      if (residual)
			{
			  node rfrom = realnode(e->from);

			  /* Don't follow edges back into dead code */
			  if (!rfrom->in_postorder)
			    continue;

			  if (rfrom->mf_height < minheight)
			    minheight = rfrom->mf_height;
			}
		    }
		}
	      /* There must be at least one residual edge */
	      assert(minheight < 2 * nnodes);
	      gnodes[i]->mf_height = minheight + 1;
	    }
	}
    }
  while (active);
}

void residual_dfs_mark(node n)
{
  n = realnode(n);
  if (!n->mf_marked)
    return;
  n->mf_marked = FALSE;

  for (; n; n = n->vertex_next)
    {
      edge e;

      for (e = n->edges_out; e; e = e->next_out)
	if (e->ecount + e->mf_flow)
	  residual_dfs_mark(e->to);

      for (e = n->edges_in; e; e = e->next_in)
	/* For directed graph: if (e->mf_flow) */
	if (e->ecount - e->mf_flow)
	  residual_dfs_mark(e->from);
    }
}
  

void min_cut(int nodecount, node parentptr *gnodes, node source, node sink)
{
  int i;

  max_flow(nodecount, gnodes, source, sink);

  /* Unmark all nodes */
  for (i = 0; i < nodecount; i++)
    gnodes[i]->mf_marked = TRUE;

  /* Mark all nodes reachable from source */
  residual_dfs_mark(sink);

  /* Cut edges are all edges between a marked and unmarked node */
}

static void insert_rcops(region r, int nodecount, node parentptr *gnodes,
			 data_declaration v)
{
  int i;

  for (i = 0; i < nodecount; i++)
    {
      node n = gnodes[i];
      node rn = realnode(n);
      bool marked = rn->mf_marked;
      edge e;

      for (e = n->edges_out; e; e = e->next_out)
	if (marked != realnode(e->to)->mf_marked)
	  add_rcop(r, e, v, marked);
    }
}

void insert_optimal_rcops(region r, function_decl fd, bitset vars)
{
  node superwhite, superblack;
  int vid;
  double start = gettime(), total;

  bitset_scan (vid, vars)
    {
      data_declaration local = fd->vmap[vid];

      if (collapse_graph(fd, local, &superwhite, &superblack))
	{
#ifdef DPRINT
	  fprintf(stderr, "rc opt for local %d %s\n", vid, local->name);
#endif
	  simplify_graph(fd, superwhite, superblack);
	  min_cut(fd->postorder_size, fd->postorder_nodes, superwhite, superblack);
	  insert_rcops(r, fd->postorder_size, fd->postorder_nodes, local);
	}
      clear_collapsed_graph(fd);
    }
  total = gettime() - start;
  if (total > largest_optrc)
    largest_optrc = total;
}

int flow_value(node n)
{
  int flow = 0;
  edge e;

  for (e = n->edges_out; e; e = e->next_out)
    flow += e->mf_flow;

  for (e = n->edges_in; e; e = e->next_in)
    flow -= e->mf_flow;

  return flow;
}

int node_number(node n, int nnodes, node *gnodes)
{
  int i;

  for (i = 0; i < nnodes; i++)
    if (n == gnodes[i])
      return i;
  assert(0);
  return -1;
}

void print_flow(int nnodes, node *gnodes, node source, node sink)
{
  int i;
  int cutflow = 0;

  printf("source flow is %d, sink flow is %d\n",
	 flow_value(source), flow_value(sink));

  for (i = 0; i < nnodes; i++)
    {
      edge e;
      node n = gnodes[i];
      node rn = realnode(n);

      printf("%d: flow %d\n", i, flow_value(n));
      if (n->vertex)
	printf("ALIASED to %d\n", node_number(n->vertex, nnodes, gnodes));

      for (e = n->edges_out; e; e = e->next_out)
	{
	  printf("  -> %d: cap %d, flow %d%s\n",
		 node_number(e->to, nnodes, gnodes),
		 e->ecount, e->mf_flow,
		 rn->mf_marked != realnode(e->to)->mf_marked ? " (CUT)" : "");
	  if (rn->mf_marked != realnode(e->to)->mf_marked)
	    cutflow += e->mf_flow > 0 ? e->mf_flow : -e->mf_flow;
	}
      
      for (e = n->edges_in; e; e = e->next_in)
	printf("  <- %d: cap %d, flow %d%s\n",
	       node_number(e->from, nnodes, gnodes),
	       e->ecount, e->mf_flow,
	       rn->mf_marked != realnode(e->from)->mf_marked ? " (CUT)" : "");
      printf("\n");
    }
  printf("cut flow %d\n", cutflow);
}

#ifdef TEST
region r;
node *gnodes;
int nnodes;
location dummy;

node new_node(region r, location location)
{
  node obj = ralloc(r, struct AST_node);

  obj->kind = kind_node;
  obj->location = location;
  obj->type = 0;
  obj->next = 0;
  obj->edges_out = 0;
  obj->edges_in = 0;
  obj->vertex = 0;
  obj->vertex_next = 0;
  obj->mark = 0;
  obj->live_in = 0;
  obj->live_out = 0;
  obj->parent = 0;
  obj->parent_ptr = 0;
  obj->inscope_vars = 0;
  obj->mf_height = 0;
  obj->mf_excess = 0;

  return obj;
}

void readgraph(void)
{
  int i;

  nnodes = 0;
  scanf("%d\n", &nnodes);
  assert(nnodes >= 2);
  gnodes = rarrayalloc(r, nnodes, node);
  for (i = 0; i < nnodes; i++)
    {
      dummy.lineno = i;
      gnodes[i] = new_node(r, dummy);
    }

  for (;;)
    {
      int from, to, weight;
      edge e = ralloc(r, struct edge);

      i = scanf("%d %d %d\n", &from, &to, &weight);
      if (i == 1 && from < 0)
	break;
      assert(i == 3);
      assert(from >= 0 && from < nnodes && to >= 0 && to < nnodes);

      e->ecount = weight;
      e->from = gnodes[from];
      e->next_out = gnodes[from]->edges_out;
      gnodes[from]->edges_out = e;
      e->to = gnodes[to];
      e->next_in = gnodes[to]->edges_in;
      gnodes[to]->edges_in = e;
    }
}

int main(int argc, char **argv)
{
  int i;
  int cutflow = 0;

  r = newregion();
  readgraph();
  min_cut(nnodes, gnodes, gnodes[0], gnodes[nnodes - 1]);
  printf("source flow is %d, sink flow is %d\n",
	 flow_value(gnodes[0]), flow_value(gnodes[nnodes - 1]));

  for (i = 0; i < nnodes; i++)
    {
      edge e;
      node n = gnodes[i];

      printf("%d: flow %d\n", i, flow_value(n));

      for (e = n->edges_out; e; e = e->next_out)
	{
	  printf("  -> %ld: cap %d, flow %d%s\n", e->to->location.lineno,
		 e->ecount, e->mf_flow,
		 e->from->mf_marked != e->to->mf_marked ? " (CUT)" : "");
	  if (e->from->mf_marked != e->to->mf_marked)
	    cutflow += e->mf_flow > 0 ? e->mf_flow : -e->mf_flow;
	}
      
      for (e = n->edges_in; e; e = e->next_in)
	printf("  <- %ld: cap %d, flow %d%s\n", e->from->location.lineno,
	       e->ecount, e->mf_flow,
	       e->from->mf_marked != e->to->mf_marked ? " (CUT)" : "");
      printf("\n");
    }
  printf("cut flow %d\n", cutflow);
  return 0;
}

void add_rcop(region r, edge e, data_declaration v, bool increment)
{
}

#endif
