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
#include "cfg.h"
#include "dd_list.h"
#include "edit.h"
#include "fix.h"
#include "AST_utils.h"

static edge new_edge(region r, int kind, node info, node from, node to,
		     edge next_out, edge next_in, float ecount)
{
  static bool nicekind[] = {
    TRUE,  /* edge_fn_entry */
    TRUE,  /* edge_fn_exit */
    TRUE,  /* edge_compound */
    TRUE,  /* edge_compound_end */
    TRUE,  /* edge_if_true */
    TRUE,  /* edge_if_true_done */
    TRUE,  /* edge_if_false */
    TRUE,  /* edge_if_false_done */
    TRUE,  /* edge_if_no_else */
    TRUE,  /* edge_label */
    TRUE,  /* edge_while_true */
    TRUE,  /* edge_while_repeat */
    TRUE,  /* edge_while_done */
    TRUE,  /* edge_dowhile_dummy */
    TRUE,  /* edge_dowhile_test */
    FALSE, /* edge_dowhile_true */
    TRUE,  /* edge_dowhile_done */
    FALSE, /* edge_switch */
    TRUE,  /* edge_switch_fallout */
    FALSE, /* edge_switch_nodefault */
    TRUE,  /* edge_for_enterloop */
    TRUE,  /* edge_for_exitloop */
    TRUE,  /* edge_for_true */
    TRUE,  /* edge_for_stmtdone */
    TRUE,  /* edge_for_continue */
    TRUE,  /* edge_for_repeat */
    TRUE,  /* edge_break */
    TRUE,  /* edge_continue */
    TRUE,  /* edge_goto */
    FALSE, /* edge_computed_goto */
    FALSE, /* edge_return_expr */
    TRUE,  /* edge_return */
    TRUE,  /* edge_comma */
    FALSE, /* edge_conditional_true */
    FALSE, /* edge_conditional_true_done */
    FALSE, /* edge_conditional_false */
    FALSE, /* edge_conditional_false_done */
    FALSE, /* edge_function_call */
    FALSE, /* edge_unary */
    FALSE, /* edge_unary_lvalue */
    FALSE, /* edge_binary */
    FALSE, /* edge_binary_lvalue */
    FALSE  /* edge_assign_local */
  };
  edge e = ralloc(r, struct edge);

  e->kind = kind;
  e->info = info;
  e->nice = nicekind[kind];
  e->next_out = next_out;
  e->next_in = next_in;
  e->from = from;
  e->to = to;
  /* Move execution counts to int (avoid dealing with FP issues in
     maxflow algo), limit max (int) ecount to 1e9 */
  if (ecount > 1e6)
    ecount = 1e6;
  e->ecount = ecount * 1e3;
#if 0
  e->mf_flow = 0;
  e->sr_vertex = NULL;
  e->incvars = e->decvars = NULL;
#endif

  return e;
}

static void add_edge(region r, function_decl fd, int kind, void *info, void *from, void *to,
		     float ecount)
{
  if (from && to)
    {
      node nfrom = CAST(node, from);
      node nto = CAST(node, to);

      edge e = new_edge(r, kind, CAST(node, info), nfrom, nto,
			nfrom->edges_out, nto->edges_in, ecount);

      nfrom->edges_out = e;
      nto->edges_in = e;
    }
}

static void add_cfg_node(region r, function_decl fd, void *n)
{
  if (fd->cfg_nextnode >= fd->cfg_size)
    {
      long oldsize = fd->cfg_size;
      node parentptr *oldnodes = fd->cfg_nodes;
      long newsize = 2 * oldsize, i;
      node parentptr *newnodes = rarrayalloc(r, newsize, node parentptr);

      for (i = 0; i < oldsize; i++)
	newnodes[i] = oldnodes[i];

      fd->cfg_size = newsize;
      fd->cfg_nodes = newnodes;
    }
  fd->cfg_nodes[fd->cfg_nextnode++] = CAST(node, n);
}

void build_cfg_stmt(region r, function_decl fd, statement stmt,
		    node *entry, node *exit, float ecount);

void build_cfg_expr(region r, function_decl fd, expression expr,
		    node *entry, node *exit, float ecount);

void build_cfg_elist(region r, function_decl fd, expression elist, int edge_kind,
		     node *entry, node *exit, float ecount)
{
  node eentry, eexit, previous = NULL;
  expression e;

  *entry = NULL;
  scan_expression (e, elist)
    {
      build_cfg_expr(r, fd, e, &eentry, &eexit, ecount);
      if (!*entry)
	*entry = eentry;
      add_edge(r, fd, edge_kind, e, previous, eentry, ecount);
      previous = eexit;
    }
  *exit = previous;
}

void build_cfg_expr(region r, function_decl fd, expression expr,
		    node *entry, node *exit, float ecount)
{
  add_cfg_node(r, fd, expr);

  if (expr->cst)
    {
      *entry = *exit = CAST(node, expr);
      return;
    }
  switch (expr->kind)
    {
    case kind_identifier: 
      *entry = *exit = CAST(node, expr);
      break;

    case kind_comma:
      build_cfg_elist(r, fd, CAST(comma, expr)->arg1, edge_comma, entry, exit,
		      ecount);
      break;

    case kind_cast_list: {
      node clexit;

      build_cfg_expr(r, fd, CAST(cast_list, expr)->init_expr,
		     entry, &clexit, ecount);
      add_edge(r, fd, edge_cast_list, NULL, clexit, expr, ecount);
      *exit = CAST(node, expr);
      break;
    }
    case kind_init_index: {
      init_index init = CAST(init_index, expr);

      build_cfg_expr(r, fd, init->init_expr, entry, exit, ecount);
      break;
    }
    case kind_init_field: {
      init_field init = CAST(init_field, expr);

      build_cfg_expr(r, fd, init->init_expr, entry, exit, ecount);
      break;
    }
    case kind_init_list: {
      build_cfg_elist(r, fd, CAST(init_list, expr)->args, 
		      edge_init_list_comma, entry, exit, ecount);
      break;
    }
    case kind_conditional: {
      conditional ce = CAST(conditional, expr);
      node cexit, a1entry, a1exit, a2entry, a2exit;

      build_cfg_expr(r, fd, ce->condition, entry, &cexit, ecount);
      build_cfg_expr(r, fd, ce->arg1, &a1entry, &a1exit, ecount / 2);
      build_cfg_expr(r, fd, ce->arg2, &a2entry, &a2exit, ecount / 2);
      add_edge(r, fd, edge_conditional_true, ce, cexit, a1entry, ecount / 2);
      add_edge(r, fd, edge_conditional_false, ce, cexit, a2entry, ecount / 2);
      add_edge(r, fd, edge_conditional_true_done, ce, a1exit, ce, ecount / 2);
      add_edge(r, fd, edge_conditional_false_done, ce, a2exit, ce, ecount / 2);
      *exit = CAST(node, ce);
      break;
    }
    case kind_compound_expr:
      build_cfg_stmt(r, fd, CAST(compound_expr, expr)->stmt, entry, exit,
		     ecount);
      break;

    case kind_function_call: {
      function_call fce = CAST(function_call, expr);
      node fnexit;

      /* The arguments are simple after fix_parse_tree so we don't represent them */
      /* Note: a call to __builtin_classify_type will pass the following test,
	 so we don't need a special case here. */
      if (fce->arg1->cst)
	*entry = CAST(node, fce);
      else
	{
	  build_cfg_expr(r, fd, fce->arg1, entry, &fnexit, ecount);
	  add_edge(r, fd, edge_function_call, NULL, fnexit, fce, ecount);
	}
      *exit = CAST(node, fce);
      break;
    }
    case kind_extension_expr:
      build_cfg_expr(r, fd, CAST(unary, expr)->arg1, entry, exit, ecount);
      break;

    case kind_field_ref:
      /* We don't want a node for the field access because it is not
	 a good place to add code. */
      build_cfg_expr(r, fd, CAST(field_ref, expr)->arg1, entry, exit, ecount);
      break;

    case kind_assign: {
      /* Distinguish lvalue = simple from localvar = expression (we
	 classify localvar = simple in the first category) */
      assign ae = CAST(assign, expr);
      node aexit;

      *exit = CAST(node, ae);
      if (simple_arg(ae->arg2))
	{
	  if (simple_arg(ae->arg1))
	    *entry = CAST(node, ae);
	  else
	    {
	      build_cfg_expr(r, fd, ae->arg1, entry, &aexit, ecount);
	      assert(is_dereference(aexit));
	      add_edge(r, fd, edge_binary_lvalue, NULL, aexit, ae, ecount);
	    }
	}
      else
	{
	  assert(is_localvar(ae->arg1));
	  build_cfg_expr(r, fd, ae->arg2, entry, &aexit, ecount);
	  add_edge(r, fd, edge_assign_local, NULL, aexit, ae, ecount);
	}
      break;
    }
    case kind_andand: case kind_oror: {
      binary be = CAST(binary, expr);
      node b1exit, b2entry, b2exit;

      build_cfg_expr(r, fd, be->arg1, entry, &b1exit, ecount);
      build_cfg_expr(r, fd, be->arg2, &b2entry, &b2exit, ecount / 2);
      add_edge(r, fd, edge_short_second, be, b1exit, b2entry, ecount / 2);
      add_edge(r, fd, edge_short_circuit, be, b1exit, be, ecount / 2);
      add_edge(r, fd, edge_short_done, be, b2exit, be, ecount / 2);
      *exit = CAST(node, be);
      break;
    }
    default:
      if (is_unary(expr))
	{
	  unary ue = CAST(unary, expr);
	  node uexit;

	  if (ue->arg1->cst)
	    *entry = CAST(node, ue);
	  else
	    {
	      build_cfg_expr(r, fd, ue->arg1, entry, &uexit, ecount);
	      add_edge(r, fd, ue->kind == kind_address_of ? edge_unary_lvalue :
		       edge_unary, NULL, uexit, ue, ecount);
	    }
	  *exit = CAST(node, ue);
	}
      else if (is_binary(expr))
	{
	  binary be = CAST(binary, expr);
	  node bexit;
	  int edgekind =
	    is_assignment(expr) ? edge_binary_lvalue : edge_binary;

	  assert(simple_arg(be->arg2));
	  if (be->arg1->cst)
	    *entry = CAST(node, be);
	  else
	    {
	      build_cfg_expr(r, fd, be->arg1, entry, &bexit, ecount);
	      add_edge(r, fd, edgekind, NULL, bexit, be, ecount);
	    }
	  *exit = CAST(node, be);
	}
      else 
	assert(0);
      break;
    }

  assert(*entry);
  /* Should check nothing incoming to *entry, but can't do that with
     current rep */
  if (*exit)
    assert(!(*exit)->edges_out);
}

void build_cfg_stmt(region r, function_decl fd, statement stmt,
		    node *entry, node *exit, float ecount)
{
  add_cfg_node(r, fd, stmt);

  switch (stmt->kind)
    {
    case kind_asm_stmt: {
      *entry = *exit = CAST(node, stmt);
      break;
    }
    case kind_compound_stmt: {
      compound_stmt cs = CAST(compound_stmt, stmt);
      statement s;
      node sentry, sexit, lastexit = NULL;

      *entry = NULL;
      scan_statement (s, cs->stmts)
	{
	  build_cfg_stmt(r, fd, s, &sentry, &sexit, ecount);
	  if (!*entry) *entry = sentry;
	  add_edge(r, fd, edge_compound, s, lastexit, sentry, ecount);
	  lastexit = sexit;
	}
      add_edge(r, fd, edge_compound_end, NULL, lastexit, stmt, ecount);
      if (!*entry)
	*entry = CAST(node, stmt);
      *exit = CAST(node, stmt);
      break;
    }
    case kind_if_stmt: {
      if_stmt is = CAST(if_stmt, stmt);
      node cexit, s1entry, s1exit, s2entry, s2exit;

      build_cfg_expr(r, fd, is->condition, entry, &cexit, ecount / 2);
      build_cfg_stmt(r, fd, is->stmt1, &s1entry, &s1exit, ecount / 2);
      add_edge(r, fd, edge_if_true, is, cexit, s1entry, ecount / 2);
      add_edge(r, fd, edge_if_true_done, is, s1exit, is, ecount / 2);
      if (is->stmt2)
	{
	  build_cfg_stmt(r, fd, is->stmt2, &s2entry, &s2exit, ecount / 2);
	  add_edge(r, fd, edge_if_false, is, cexit, s2entry, ecount / 2);
	  add_edge(r, fd, edge_if_false_done, is, s2exit, is, ecount / 2);
	}
      else
	add_edge(r, fd, edge_if_no_else, is, cexit, is, ecount / 2);
      *exit = CAST(node, is);
      break;
    }
    case kind_labeled_stmt: {
      labeled_stmt ls = CAST(labeled_stmt, stmt);
      node sentry;

      *entry = CAST(node, ls->label);
      add_cfg_node(r, fd, ls->label);

      build_cfg_stmt(r, fd, ls->stmt, &sentry, exit, ecount);
      add_edge(r, fd, edge_label, ls, ls->label, sentry, ecount);
      break;
    }
    case kind_expression_stmt: {
      expression_stmt es = CAST(expression_stmt, stmt);

      build_cfg_expr(r, fd, es->arg1, entry, exit, ecount);
      break;
    }
    case kind_while_stmt: {
      conditional_stmt cs = CAST(conditional_stmt, stmt);
      node cexit, sentry, sexit;

      build_cfg_expr(r, fd, cs->condition, entry, &cexit, ecount * 5);
      cs->continue_dest = *entry;
      cs->break_dest = CAST(node, cs);
      build_cfg_stmt(r, fd, cs->stmt, &sentry, &sexit, ecount * 5);
      add_edge(r, fd, edge_while_true, cs, cexit, sentry, ecount * 5);
      add_edge(r, fd, edge_while_repeat, cs, sexit, *entry, ecount * 5);
      add_edge(r, fd, edge_while_done, cs, cexit, stmt, ecount);
      *exit = CAST(node, stmt);
      break;
    }
    case kind_dowhile_stmt: {
      conditional_stmt cs = CAST(conditional_stmt, stmt);
      node centry, cexit, sentry, sexit;

      cs->continue_dest = *entry =
	CAST(node, new_empty_stmt(r, cs->location));
      add_cfg_node(r, fd, cs->continue_dest);
      cs->break_dest = CAST(node, cs);
      build_cfg_stmt(r, fd, cs->stmt, &sentry, &sexit, ecount * 5);
      build_cfg_expr(r, fd, cs->condition, &centry, &cexit, ecount * 5);
      add_edge(r, fd, edge_dowhile_dummy, cs, *entry, sentry, ecount * 5);
      add_edge(r, fd, edge_dowhile_test, cs, sexit, centry, ecount * 5);
      add_edge(r, fd, edge_dowhile_true, cs, cexit, *entry, ecount * 5);
      add_edge(r, fd, edge_dowhile_done, cs, cexit, stmt, ecount);
      *exit = CAST(node, stmt);
      break;
    }
    case kind_switch_stmt: {
      switch_stmt sws = CAST(switch_stmt, stmt);
      node cexit, sentry, sexit;
      label cases;
      int casecount;
      bool has_default = FALSE;

      /* Count cases. We will assume they are all equiprobable */
      casecount = 0;
      for (cases = sws->next_label; cases; cases = cases->next_label)
	{
	  if (is_default_label(cases))
	    has_default = TRUE;
	  casecount++;
	}
      if (!has_default)
	casecount++;

      sws->break_dest = CAST(node, sws);
      build_cfg_expr(r, fd, sws->condition, entry, &cexit, ecount);
      /* The execution count of the body is set to ecount/casecount. This
	 will apply to all cases as our rather simplistic execution counts
	 ignore breaks, etc */
      build_cfg_stmt(r, fd, sws->stmt, &sentry, &sexit, ecount / casecount);
      *exit = CAST(node, sws);

      for (cases = sws->next_label; cases; cases = cases->next_label)
	add_edge(r, fd, edge_switch, sws, cexit, cases, ecount / casecount);

      add_edge(r, fd, edge_switch_fallout, sws, sexit, sws, ecount / casecount);
      if (!has_default)
	add_edge(r, fd, edge_switch_nodefault, sws, cexit, sws,
		 ecount / casecount);
      break;
    }
    case kind_for_stmt: {
      for_stmt fs = CAST(for_stmt, stmt);
      node c1entry, c1exit, loopentry, c2exit, c3entry, c3exit, sentry, sexit;

      fs->break_dest = CAST(node, fs);
      fs->continue_dest = CAST(node, new_empty_stmt(r, fs->location));
      add_cfg_node(r, fd, fs->continue_dest);

      /* This is icky */
      build_cfg_stmt(r, fd, fs->stmt, &sentry, &sexit, ecount * 5);
      if (fs->arg1)
	build_cfg_expr(r, fd, fs->arg1, &c1entry, &c1exit, ecount);
      if (fs->arg2)
	build_cfg_expr(r, fd, fs->arg2, &loopentry, &c2exit, ecount * 5);
      else
	loopentry = sentry;
      if (fs->arg3)
	build_cfg_expr(r, fd, fs->arg3, &c3entry, &c3exit, ecount * 5);

      *entry = fs->arg1 ? c1entry : loopentry;
      *exit = CAST(node, fs);

      if (fs->arg1)
	add_edge(r, fd, edge_for_enterloop, fs, c1exit, loopentry, ecount);
      if (fs->arg2)
	{
	  add_edge(r, fd, edge_for_exitloop, fs, c2exit, fs, ecount);
	  add_edge(r, fd, edge_for_true, fs, c2exit, sentry, ecount * 5);
	}
      add_edge(r, fd, edge_for_stmtdone, fs, sexit, fs->continue_dest, ecount * 5);
      if (fs->arg3)
	{
	  add_edge(r, fd, edge_for_continue, fs, fs->continue_dest, c3entry,
		   ecount * 5);
	  add_edge(r, fd, edge_for_repeat, fs, c3exit, loopentry, ecount * 5);
	}
      else
	add_edge(r, fd, edge_for_continue, fs, fs->continue_dest, loopentry,
		 ecount * 5);
      break;
    }
    case kind_break_stmt:
      *entry = CAST(node, stmt);
      *exit = NULL;
      add_edge(r, fd, edge_break, NULL, stmt, stmt->parent_loop->break_dest,
	       ecount);
      break;

    case kind_continue_stmt:
      *entry = CAST(node, stmt);
      *exit = NULL;
      add_edge(r, fd, edge_continue, NULL, stmt, stmt->parent_loop->continue_dest,
	       ecount);
      break;

    case kind_goto_stmt: {
      *entry = CAST(node, stmt);
      *exit = NULL;
      add_edge(r, fd, edge_goto, NULL, stmt,
	       CAST(goto_stmt, stmt)->id_label->ldecl->definition, ecount);
      break;
    }
    case kind_empty_stmt:
      *entry = *exit = CAST(node, stmt);
      break;

    case kind_computed_goto_stmt: {
      computed_goto_stmt cgs = CAST(computed_goto_stmt, stmt);
      node gexit;
      dd_list_pos label;
      int labelcount;

      build_cfg_expr(r, fd, cgs->arg1, entry, &gexit, ecount);
      *exit = NULL;

      labelcount = 0;
      dd_scan (label, fd->addressed_labels)
	labelcount++;

      dd_scan (label, fd->addressed_labels)
	add_edge(r, fd, edge_computed_goto, cgs, gexit,
		 DD_GET(label_declaration, label)->definition,
		 ecount / labelcount);
      break;
    }
    case kind_return_stmt: {
      return_stmt rs = CAST(return_stmt, stmt);

      if (rs->arg1 && !rs->arg1->cst)
	{
	  node rexit;

	  build_cfg_expr(r, fd, rs->arg1, entry, &rexit, ecount);
	  add_edge(r, fd, edge_return_expr, rs, rexit, fd, ecount);
	}
      else
	{
	  *entry = CAST(node, rs);
	  add_edge(r, fd, edge_return, rs, rs, fd, ecount);
	}
      *exit = NULL;
      break;
    }
    default: assert(0);
    }

  assert(*entry);
  /* Should check nothing incoming to *entry, but can't do that with
     current rep */
  if (*exit)
    assert(!(*exit)->edges_out);
}

void build_cfg(region r, function_decl fd)
{
  node entry, exit;

  fd->cfg_size = 256;
  fd->cfg_nodes = rarrayalloc(r, fd->cfg_size, node parentptr);
  fd->cfg_nextnode = 0;

  fd->cfg_entry = CAST(node, new_empty_stmt(r, fd->location));
  add_cfg_node(r, fd, fd->cfg_entry);
  add_cfg_node(r, fd, fd);

  build_cfg_stmt(r, fd, fd->stmt, &entry, &exit, 1);
  add_edge(r, fd, edge_fn_entry, fd, fd->cfg_entry, entry, 1);
  add_edge(r, fd, edge_fn_exit, fd, exit, fd, 1);

  fd->cfg_size = fd->cfg_nextnode;
}

static unsigned int current_mark = 41; /* Anything > 0 */

bool vertex_marked(node n)
{
  return n->mark == current_mark;
}

void mark_vertex(node n)
{
  n->mark = current_mark;
}

void clear_all_marks(void)
{
  current_mark++;
}

static long count_size(node n)
{
  edge out;
  long cnt = 0;

  if (vertex_marked(n))
    return 0;

  mark_vertex(n);

  for (out = n->edges_out; out; out = out->next_out)
    cnt += count_size(out->to);

  return cnt + 1;
}

long cfg_size(function_decl fd)
{
  clear_all_marks();
  return count_size(fd->cfg_entry);
}

static void postorder_build(node n, node parentptr **post, node parentptr **pre, unsigned long *id)
{
  edge out;

  if (n->in_postorder)
    return;

  n->in_postorder = TRUE;

  *(*pre)++ = n;
  for (out = n->edges_out; out; out = out->next_out)
    postorder_build(out->to, post, pre, id);
  *(*post)++ = n;
  n->in_postorder = (*id)++;
}

void cfg_build_postorder(region r, function_decl fd)
{
  node parentptr *post, *pre;
  long i;
  unsigned long id;

  /* We don't use the marks because we want to remember which nodes
     are in the post-order anyway (i.e., which nodes are not dead code) */
  for (i = 0; i < fd->cfg_size; i++)
    fd->cfg_nodes[i]->in_postorder = FALSE;

  fd->postorder_size = cfg_size(fd);
  clear_all_marks();
  post = rarrayalloc(r, fd->postorder_size, node parentptr);
  pre = rarrayalloc(r, fd->postorder_size, node parentptr);
  fd->postorder_nodes = post;
  fd->preorder_nodes = pre;
  id = 1;
  postorder_build(fd->cfg_entry, &post, &pre, &id);
}
