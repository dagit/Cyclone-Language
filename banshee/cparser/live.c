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
#include "live.h"
#include "cfg.h"
#include "AST_utils.h"

static void set_id(bitset b, bitset vars, void *e)
{
  identifier id = CAST(identifier, e);

  if (id->ddecl->id != -1 && BIT_SETP(vars, id->ddecl->id))
    SET_BITB(b, id->ddecl->id);
}

static void clear_id(bitset b, void *e)
{
  identifier id = CAST(identifier, e);

  if (id->ddecl->id != -1)
    CLEAR_BITB(b, id->ddecl->id);
}

static void set_if_id(bitset b, bitset vars, expression e)
{
  e = ignore_fields(e);
  if (is_identifier(e))
    set_id(b, vars, e);
}

static void clear_if_id(bitset b, expression e)
{
  e = ignore_fields(e);
  if (is_identifier(e))
    clear_id(b, e);
}

/* Requires: vars[i] => !fd->vmap[i]->addressed (variables specified in
     vars are not addressed)
*/
   
void compute_liveness(region r, function_decl fd, bitset vars)
{
  node parentptr *gnodes = fd->postorder_nodes;
  int nvars = fd->nlocals, i, nnodes;
  bool change;
  bitset old_live_in;

  nnodes = fd->postorder_size;

  old_live_in = new_bitset(r, nvars);
  for (i = 0; i < nnodes; i++)
    {
      gnodes[i]->live_in = new_bitset(r, nvars);
      gnodes[i]->live_out = new_bitset(r, nvars);
    }

  /* XXX: This should be improved if performance becomes problematic */
  /* XXX: The liveness is somewhat weird because of the weird cfg
     (one argument of unary's, binary's and function calls goes dead
     on the node leading into the unary/binary/function call that uses
     it). cast_list nodes have this happen to every arg. */
  do
    {
      change = FALSE;

      for (i = 0; i < nnodes; i++)
	{
	  edge out;
	  node n = gnodes[i];

	  if (!change)
	    bassignb(old_live_in, n->live_in);

	  for (out = n->edges_out; out; out = out->next_out)
	    bunionb(n->live_out, out->to->live_in);

	  bassignb(n->live_in, n->live_out);
	  /* Gen & kill */
	  if (is_identifier(n))
	    set_id(n->live_in, vars, n);
	  else if (is_assign(n))
	    {
	      assign a = CAST(assign, n);

	      clear_if_id(n->live_in, a->arg1);

	      /* After the clear because of x=x */
	      set_if_id(n->live_in, vars, a->arg2);
	    }
	  else if (is_binary(n))
	    {
	      /* arg2 is simple and live into here */
	      binary b = CAST(binary, n);

	      set_if_id(n->live_in, vars, b->arg2);
	    }
	  else if (is_function_call(n))
	    {
	      function_call fce = CAST(function_call, n);
	      expression arg;

	      scan_expression (arg, fce->args)
		set_if_id(n->live_in, vars, arg);
	    }
	  else if (is_asm_stmt(n))
	    {
	      asm_stmt as = CAST(asm_stmt, n);
	      asm_operand ao;

	      /* This is not really necessary because all outputs of asm 
		 statements are marked as 'addressed', so are not in 'vars' */
	      scan_asm_operand (ao, as->asm_operands1) /* Outputs, killed */
		clear_if_id(n->live_in, ao->arg1);

	      scan_asm_operand (ao, as->asm_operands2) /* Inputs, live */
		set_if_id(n->live_in, vars, ao->arg1);
	    }

	  if (!change && !bitset_eqp(old_live_in, n->live_in))
	    change = TRUE;
	}
    }
  while (change);

#ifndef NDEBUG
  /* Consistency check: no dead->live transitions (can't occur because we 
     initialise all locals in vars) */
  for (i = 0; i < nnodes; i++)
    {
      edge out;
      node n = gnodes[i];
      bool cancheck = TRUE;
      int vid;

      if (is_assign(n))
	{
	  assign a = CAST(assign, n);

	  if (is_identifier(a->arg1) &&
	    CAST(identifier, a->arg1)->ddecl->id != -1)
	    cancheck = FALSE;
	}

      if (cancheck)
	for (out = n->edges_out; out; out = out->next_out)
	  bitset_scan_complement (vid, n->live_in)
	    assert(!BIT_SETP(out->to->live_in, vid));
#endif
    }
}
