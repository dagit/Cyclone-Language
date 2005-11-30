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

#include "parser.h"
#include "temps.h"
#include "cfg.h"
#include "AST_utils.h"
#include <limits.h>

/* Simple reaching definitions, only consider
   nodef: represented by NULL
   one def: represented by pointer to defining assignment
   many defs: represented by &top

   Defs are kept in the defs_in/defs_out fields, indexed by variable id.
   Does not handle variables whose address is taken (so istemp is only valid
   if "addressed" is false).
*/

#ifdef NODEREFINT
#define top ((unsigned short)-1)
#define bottom 0
#else
static struct AST_node thetop;
#define top &thetop
#define bottom NULL
#endif

static noderef def_merge(noderef n1, noderef n2)
{
  if (n1 == n2 || n1 == bottom)
    return n2;

  if (n2 == bottom)
    return n1;

  return top;
}

static void def_set(noderef *defs, node n, identifier id)
{
  data_declaration decl = id->ddecl;

  if (decl->islocal)
#ifdef NODEREFINT
    defs[decl->id] = n->in_postorder;
#else
    defs[decl->id] = n;
#endif
}

static void temp_check(noderef *defs, void *id)
{
  data_declaration decl = CAST(identifier, id)->ddecl;

  /* If defs indicates bottom or top for this variable then it is not a temp */
  if (decl->istemp && (defs[decl->id] == top || defs[decl->id] == bottom))
    decl->istemp = FALSE;
}

static void defs_copy(noderef *to, noderef *from, int nvars)
{
  int i;

  for (i = 0; i < nvars; i++)
    to[i] = from[i];
}

static bool defs_equal(noderef *to, noderef *from, int nvars)
{
  int i;

  for (i = 0; i < nvars; i++)
    if (to[i] != from[i])
      return FALSE;

  return TRUE;
}

static void defs_merge(noderef *to, noderef *from, int nvars)
{
  int i;

  for (i = 0; i < nvars; i++)
    to[i] = def_merge(to[i], from[i]);
}

static void compute_defs_in(noderef *defs_in, node n, int nvars)
{
  edge in;
  bool first = TRUE;

  for (in = n->edges_in; in; in = in->next_in)
    if (in->from->defs_out) /* Not all predecessors are reachable */
      {
	if (first)
	  {
	    defs_copy(defs_in, in->from->defs_out, nvars);
	    first = FALSE;
	  }
	else
	  defs_merge(defs_in, in->from->defs_out, nvars);
      }
  
  if (first) /* Entry node is special */
    {
      assert(is_empty_stmt(n));
      defs_copy(defs_in, n->defs_out, nvars);
    }
}

void compute_temps(function_decl fd) deletes
{
  region r = newregion();
  node parentptr *gnodes = fd->preorder_nodes;
  int nvars = fd->nlocals, i, nnodes = fd->postorder_size;
  bool change;
  noderef *old_defs_out, *rentry, *defs_in;
  int itercount = 0;

#ifdef NODEREFINT
  assert(nnodes < USHRT_MAX);
#endif
  old_defs_out = rarrayalloc(r, nnodes, noderef);
  for (i = 0; i < nnodes; i++)
    gnodes[i]->defs_out = rarrayalloc(r, nvars, noderef);

  /* Everything is top at entry (one could argue that undefined means do what you
     want, but seems a bad idea). And parameters should probably be top as well. */
  rentry = fd->cfg_entry->defs_out;
;
  for (i = 0; i < fd->nlocals; i++)
    {
      data_declaration v = fd->vmap[i];

      v->istemp = !(v->addressed || v->isparameter);
      rentry[v->id] = top;
    }

  defs_in = rarrayalloc(r, nvars, noderef);
  do
    {
      change = FALSE;
      itercount++;

      /* Skip entry node (which has no predecessors) */
      for (i = 1; i < nnodes; i++)
	{
	  node n = gnodes[i];

	  if (!change)
	    defs_copy(old_defs_out, n->defs_out, nvars);

	  compute_defs_in(defs_in, n, nvars);
	  defs_copy(n->defs_out, defs_in, nvars);

	  /* Gen & kill */
	  if (is_assignment(n))
	    {
	      binary a = CAST(binary, n);

	      if (is_identifier(a->arg1))
		def_set(n->defs_out, n, CAST(identifier, a->arg1));

	    }
	  else if (is_increment(n))
	    {
	      unary inc = CAST(unary, n);

	      if (is_identifier(inc->arg1))
		def_set(n->defs_out, n, CAST(identifier, inc->arg1));
	    }

	  if (!change && !defs_equal(old_defs_out, n->defs_out, nvars))
	    change = TRUE;
	}
    }
  while (change);

  for (i = 1; i < nnodes; i++)
    {
      node n = gnodes[i];

      compute_defs_in(defs_in, n, nvars);

      if (is_identifier(n))
	temp_check(defs_in, n);
      else if (is_assign(n))
	{
	  assign a = CAST(assign, n);

	  /* After the clear because of x=x */
	  if (is_identifier(a->arg2))
	    temp_check(defs_in, a->arg2);
	}
      else if (is_binary(n))
	{
	  /* arg2 is simple and live into here */
	  binary b = CAST(binary, n);

	  if (is_identifier(b->arg2))
	    temp_check(defs_in, b->arg2);
	}
      else if (is_function_call(n))
	{
	  function_call fce = CAST(function_call, n);
	  expression arg;

	  scan_expression (arg, fce->args)
	    if (is_identifier(arg))
	      temp_check(defs_in, arg);
	}
    }

#ifdef DPRINT
  fprintf(stderr, "non-temps for %s(%d):", fd->ddecl->name, itercount);
  for (i = 0; i < nvars; i++)
    if (!(fd->vmap[i]->istemp || fd->vmap[i]->addressed))
      fprintf(stderr, " %s", fd->vmap[i]->name);
  fprintf(stderr, "\n");
  fflush(stderr);
#endif

  for (i = 0; i < nnodes; i++)
    gnodes[i]->defs_out = NULL;

  deleteregion(r);
}
