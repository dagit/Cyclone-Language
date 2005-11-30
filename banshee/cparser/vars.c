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

/* At each node of a function's (FD) CFG set the 'inscope_vars' field to a
   bitset of all local vars currently inscope. Also build a map from local
   variable ids to local variables in 'FD->vmap' and store the
   'label_declaration' for all addressed labels in 'FD->addressed_labels'
*/

#include "parser.h"
#include "vars.h"
#include "edit.h"
#include "c-parse.h"
#include "dd_list.h"
#include "semantics.h"
#include "edit.h"

/* Update maps for inscope vars on entry to a new scope, i.e.,
   for each local variable V in top level of 'env':
   - set bit 'V->id' in 'b'
   - set 'vmap[V->id]' to V */
static void add_vars(environment env, bitset b, data_declaration *vmap)
{
  env_scanner scan;
  const char *id;
  void *idval;

  env_scan(env->id_env, &scan);
  while (env_next(&scan, &id, &idval))
    {
      data_declaration iddecl = idval;

      if (iddecl->kind == decl_variable && iddecl->islocal)
	{
	  assert(iddecl->id >= 0 && iddecl->id < b->size);
	  vmap[iddecl->id] = iddecl;
	  SET_BITB(b, iddecl->id);
	}
    }
}

/* Set 'n0->inscope_vars' to 'inscope' if 'n0' is in the CFG
   ('n0' is 'void *' to simplify caller's life) */
static void set_inscope_vars(void *n0, bitset inscope)
{
  node n = CAST(node, n0);

  /* We only set inscope_vars on nodes actually in the CFG
     (makes life easier when we want to clear the refs) */
  if (n->edges_in || n->edges_out)
    n->inscope_vars = inscope;
}

/* 'collect_XXX' scans the AST to collect the information described
   at the top of this file. Region 'r' is used to allocate the memory
   needed. 'inscope' is the local variables in scope before the AST
   node, 'vmap' the variable map for FD, 'addressed_labels' is the
   addressed labels list for FD. */

void collect_expr(region r, expression expr, bitset inscope,
		  data_declaration *vmap, dd_list addressed_labels);
void collect_stmt(region r, statement stmt, bitset inscope,
		  data_declaration *vmap, dd_list addressed_labels);


void collect_expr_list(region r, expression elist, bitset inscope,
		       data_declaration *vmap, dd_list addressed_labels)
{
  while (elist)
    {
      collect_expr(r, elist, inscope, vmap, addressed_labels);
      elist = CAST(expression, elist->next);
    }
}

void collect_expr(region r, expression expr, bitset inscope,
		  data_declaration *vmap, dd_list addressed_labels)
{
  set_inscope_vars(expr, inscope);
  switch (expr->kind)
    {
    case kind_sizeof_type: case kind_alignof_type:
    case kind_sizeof_expr: case kind_alignof_expr: case kind_identifier:
      break;

    case kind_label_address:
      dd_add_last(r, addressed_labels,
		  CAST(label_address, expr)->id_label->ldecl);
      break;

    case kind_comma:
      collect_expr_list(r, CAST(comma, expr)->arg1, inscope, vmap,
			addressed_labels);
      break;

    case kind_cast_list: {
      collect_expr_list(r, CAST(cast_list, expr)->init_expr,
			inscope, vmap, addressed_labels);
      break;
    }
    case kind_init_index: {
      collect_expr(r, CAST(init_index, expr)->init_expr,
		   inscope, vmap, addressed_labels);
      break;
    }
    case kind_init_field: {
      collect_expr(r, CAST(init_field, expr)->init_expr,
		   inscope, vmap, addressed_labels);
      break;
    }
    case kind_init_list: {
      collect_expr_list(r, CAST(init_list, expr)->args,
			inscope, vmap, addressed_labels);
      break;
    }
    case kind_conditional: {
      conditional ce = CAST(conditional, expr);

      collect_expr(r, ce->condition, inscope, vmap, addressed_labels);
      collect_expr(r, ce->arg1, inscope, vmap, addressed_labels);
      collect_expr(r, ce->arg2, inscope, vmap, addressed_labels);
      break;
    }
    case kind_compound_expr:
      collect_stmt(r, CAST(compound_expr, expr)->stmt, inscope, vmap,
		   addressed_labels);
      break;

    case kind_function_call: {
      function_call fce = CAST(function_call, expr);

      collect_expr(r, fce->arg1, inscope, vmap, addressed_labels);
      collect_expr_list(r, fce->args, inscope, vmap, addressed_labels);
      break;
    }
    default:
      if (is_unary(expr))
	{
	  unary ue = CAST(unary, expr);
	  collect_expr(r, ue->arg1, inscope, vmap, addressed_labels);
	}
      else if (is_binary(expr))
	{
	  binary be = CAST(binary, expr);
	  
	  collect_expr(r, be->arg1, inscope, vmap, addressed_labels);
	  collect_expr(r, be->arg2, inscope, vmap, addressed_labels);
	}
      else if (!expr->cst)
	assert(0);
      break;
    }
}

void collect_stmt(region r, statement stmt, bitset inscope,
		  data_declaration *vmap, dd_list addressed_labels)
{
  set_inscope_vars(stmt, inscope);
  switch (stmt->kind)
    {
    case kind_asm_stmt: {
      asm_stmt as = CAST(asm_stmt, stmt);
      asm_operand arg;
      
      scan_asm_operand (arg, as->asm_operands1)
	collect_expr(r, arg->arg1, inscope, vmap, addressed_labels);

      scan_asm_operand (arg, as->asm_operands2)
	collect_expr(r, arg->arg1, inscope, vmap, addressed_labels);
      break;
    }
    case kind_compound_stmt: {
      compound_stmt cs = CAST(compound_stmt, stmt);
      statement s;

      if (cs->decls)
	{
	  inscope = bitcopy(r, inscope);
	  add_vars(cs->env, inscope, vmap);
	}
      scan_statement (s, cs->stmts)
	collect_stmt(r, s, inscope, vmap, addressed_labels);
      break;
    }
    case kind_if_stmt: {
      if_stmt is = CAST(if_stmt, stmt);

      collect_expr(r, is->condition, inscope, vmap, addressed_labels);
      collect_stmt(r, is->stmt1, inscope, vmap, addressed_labels);
      if (is->stmt2)
	collect_stmt(r, is->stmt2, inscope, vmap, addressed_labels);
      break;
    }
    case kind_labeled_stmt: {
      labeled_stmt ls = CAST(labeled_stmt, stmt);
      set_inscope_vars(ls->label, inscope);
      collect_stmt(r, ls->stmt, inscope, vmap, addressed_labels);
      break;
    }
    case kind_expression_stmt: {
      expression_stmt es = CAST(expression_stmt, stmt);
      collect_expr(r, es->arg1, inscope, vmap, addressed_labels);
      break;
    }
    case kind_while_stmt: case kind_switch_stmt: {
      conditional_stmt cs = CAST(conditional_stmt, stmt);

      collect_expr(r, cs->condition, inscope, vmap, addressed_labels);
      collect_stmt(r, cs->stmt, inscope, vmap, addressed_labels);
      break;
    }
    case kind_dowhile_stmt: {
      conditional_stmt cs = CAST(conditional_stmt, stmt);

      set_inscope_vars(cs->continue_dest, inscope);
      collect_expr(r, cs->condition, inscope, vmap, addressed_labels);
      collect_stmt(r, cs->stmt, inscope, vmap, addressed_labels);
      break;
    }
    case kind_for_stmt: {
      for_stmt fs = CAST(for_stmt, stmt);

      set_inscope_vars(fs->continue_dest, inscope);
      if (fs->arg1)
	collect_expr(r, fs->arg1, inscope, vmap, addressed_labels);
      if (fs->arg2)
	collect_expr(r, fs->arg2, inscope, vmap, addressed_labels);
      if (fs->arg3)
	collect_expr(r, fs->arg3, inscope, vmap, addressed_labels);
      collect_stmt(r, fs->stmt, inscope, vmap, addressed_labels);
      break;
    }
    case kind_break_stmt: case kind_continue_stmt: case kind_goto_stmt:
    case kind_empty_stmt:
      break;

    case kind_computed_goto_stmt: {
      computed_goto_stmt cgs = CAST(computed_goto_stmt, stmt);

      collect_expr(r, cgs->arg1, inscope, vmap, addressed_labels);
      break;
    }

    case kind_return_stmt: {
      return_stmt rs = CAST(return_stmt, stmt);

      if (rs->arg1)
	collect_expr(r, rs->arg1, inscope, vmap, addressed_labels);
      break;
    }
    default: assert(0);
    }
}

/* See comment at start of file. Memory is allocated in 'r'.
   Requires: 'fd' must have a CFG
   Modifies: 'inscope_vars' on all nodes of 'fd', 'fd->vmap',
     'fd->addressed_labels'. All these values are allocated in 'r'.
*/
void collect_variables(region r, function_decl fd)
{
  bitset parameters = bclearb(new_bitset(r, fd->nlocals));
  data_declaration *vmap = rarrayalloc(r, fd->nlocals, data_declaration);

  /* The parameters are considered to be out of scope at fd (because we
     use fd to represent the function's exit) */
  set_inscope_vars(fd, bclearb(new_bitset(r, fd->nlocals)));

  /* The parameters are "in-scope" at fd->cfg_entry */
  add_vars(fd->fdeclarator->env, parameters, vmap);
  set_inscope_vars(fd->cfg_entry, parameters);
  fd->vmap = vmap;

  fd->addressed_labels = dd_new_list(r);
  collect_stmt(r, fd->stmt, parameters, vmap, fd->addressed_labels);
}
