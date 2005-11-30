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
#include "edit.h"
#include "semantics.h"
#include "cfg.h"
#include "stmt.h"
#include "constants.h"
#include "AST_utils.h"

/* XXX: Do constant folding in all build_xxx functions */

#define LABEL_PREFIX "__rcl"

label_declaration declare_new_label(region r)
{
  char lname[sizeof(LABEL_PREFIX) + 20];
  static int nextid = 38943;
  label_declaration ldecl;
  
  /* XXX: Should make a real AST node, and add to function's label env */
  sprintf(lname, LABEL_PREFIX "%d", nextid++);
  ldecl = new_label_declaration(r, rstrdup(r, lname), NULL);

  return ldecl;
}

#define TEMP_PREFIX "__rct"

char *next_temporary(void)
{
  static long nextid = 42;
  static char idname[sizeof(TEMP_PREFIX) + 20];

  sprintf(idname, TEMP_PREFIX "%ld", nextid++);

  return idname;
}

/* Declare a new temporary that can be assigned a value of type t.
   Place the declaration at the start of block. 
   XXX: See discussion in types.c:tag2ast about the (lack of) correctness of
   this approach.
   Return it's declaration */
data_declaration declare_temporary(region r, compound_stmt block, type t)
{
  /* XXX: fd's toplevel block is the wrong place (because of local struct tags) */
  struct data_declaration tempdecl;
  identifier_declarator id;
  variable_decl vd;
  data_decl dd;
  location loc = block->location;
  declarator tdeclarator;
  type_element tmodifiers;

  /* Compute real type, name */
  if (type_array(t))
    t = make_pointer_type(type_array_of(t));
  else if (type_function(t))
    t = make_pointer_type(t);
  /* Qualifiers must not be present on the temp (the qualifiers of t apply
     to the original location we are building a temp for only) */
  t = make_qualified_type(t, no_qualifiers);

  /* Build AST for the declaration */
  id = new_identifier_declarator(r, loc, str2cstring(r, next_temporary()));
  type2ast(r, loc, t, CAST(declarator, id), &tdeclarator, &tmodifiers);
  vd = new_variable_decl(r, loc, tdeclarator, NULL, NULL, NULL, NULL);
  vd->declared_type = t;
  dd = new_data_decl(r, loc, tmodifiers, NULL, CAST(declaration, vd));

  /* Add to the function's declarations */
  dd->next = CAST(node, block->decls);
  block->decls = CAST(declaration, dd);

  /* Set parent pointers */
  AST_set_parents(CAST(node, dd));
  dd->parent = CAST(node, block);
  dd->parent_ptr = CASTSRPTR(node, &block->decls);
  if (dd->next)
    dd->next->parent_ptr = &dd->next;

  /* Declare the variable */
  init_data_declaration(&tempdecl, CAST(declaration, dd), id->cstring.data, t);
  tempdecl.kind = decl_variable;
  tempdecl.vtype = variable_normal;
  tempdecl.islocal = TRUE;

  return vd->ddecl = declare(block->env, &tempdecl, FALSE);
}

/* Return the comma expression allocated in region r, with location loc which
   evaluations expression in list ELIST followed by expression LAST */
expression build_comma(region r, location loc, expression elist, expression last)
{
  comma ce = new_comma(r, loc, expression_chain(elist, last));

  assert(!last->next);
  ce->type = last->type;

  set_parent_list(CASTSRPTR(node, &ce->arg1), CAST(node, ce));

  return CAST(expression, ce);
}

/* Return the comma expression allocated in region r, with location loc which
   evaluations expression in list ELIST */
expression build_comma2(region r, location loc, expression elist)
{
  comma ce = new_comma(r, loc, elist);

  ce->type = CAST(expression, last_node(CAST(node, elist)))->type;

  set_parent_list(CASTSRPTR(node, &ce->arg1), CAST(node, ce));

  return CAST(expression, ce);
}

expression build_assignment(region r, location loc, int assignop,
			    expression left, expression right)
{
  binary assignment = newkind_binary(r, assignop, loc, left, right);

  assignment->type = make_qualified_type(left->type, no_qualifiers);
  set_parent(CASTSRPTR(node, &assignment->arg1), CAST(node, assignment));
  set_parent(CASTSRPTR(node, &assignment->arg2), CAST(node, assignment));

  return CAST(expression, assignment);
}

expression build_increment(region r, location loc, int incop, expression lvalue)
{
  unary inc = newkind_unary(r, incop, loc, lvalue);

  inc->type = lvalue->type;

  return CAST(expression, inc);
}

expression build_dereference(region r, location loc, expression ptr)
{
  dereference dr = new_dereference(r, loc, ptr);

  dr->type = type_points_to(type_default_conversion(ptr->type));
  set_parent(CASTSRPTR(node, &dr->arg1), CAST(node, dr));

  return CAST(expression, dr);
}

expression build_conditional(region r, location loc, type result_type,
			     expression cond, expression true, expression false)
{
  conditional ce = new_conditional(r, loc, cond, true, false);

  ce->type = result_type;
  set_parent(CASTSRPTR(node, &ce->arg1), CAST(node, ce));
  set_parent(CASTSRPTR(node, &ce->arg2), CAST(node, ce));
  set_parent(CASTSRPTR(node, &ce->condition), CAST(node, ce));

  return CAST(expression, ce);
}

expression build_sizeof_expr(region r, location loc, expression e)
{
  sizeof_expr sz = new_sizeof_expr(r, loc, e);
  expression result = CAST(expression, sz);

  set_parent(CASTSRPTR(node, &sz->arg1), CAST(node, sz));

  result->type = size_t_type;
  result->cst = fold_sizeof(result, e->type);

  return result;
}

expression build_arithmetic(region r, location loc, int kind,
			    expression arg1, expression arg2)
{
  binary result = newkind_binary(r, kind, loc, arg1, arg2);

  set_parent(CASTSRPTR(node, &result->arg1), CAST(node, result));
  set_parent(CASTSRPTR(node, &result->arg2), CAST(node, result));
  result->type = common_type(type_default_conversion(arg1->type),
			     type_default_conversion(arg2->type));

  return CAST(expression, result);
}

expression build_lt(region r, location loc, expression arg1, expression arg2)
{
  lt result = new_lt(r, loc, arg1, arg2);

  set_parent(CASTSRPTR(node, &result->arg1), CAST(node, result));
  set_parent(CASTSRPTR(node, &result->arg2), CAST(node, result));
  result->type = int_type;

  return CAST(expression, result);
}

expression build_pointer_plus(region r, location loc,
			      expression arg1, expression arg2)
{
  binary result = new_plus(r, loc, arg1, arg2);
  expression e;
  type t1 = type_default_conversion(arg1->type);

  set_parent(CASTSRPTR(node, &result->arg1), CAST(node, result));
  set_parent(CASTSRPTR(node, &result->arg2), CAST(node, result));

  if (type_pointer(t1))
    result->type = t1;
  else
    result->type = type_default_conversion(arg2->type);

  e = CAST(expression, result);
  e->cst = fold_binary(e->type, e);
  return e;
}

expression build_field_ref(region r, location loc, 
			   expression base, const char *name)
{
  field_ref fr = new_field_ref(r, loc, base, str2cstring(r, name));
  field_declaration fdecl =
    env_lookup(type_tag(base->type)->fields, name, FALSE);

  set_parent(CASTSRPTR(node, &fr->arg1), CAST(node, fr));
  fr->type = qualify_type1(fdecl->type, base->type);

  return CAST(expression, fr);
}

statement build_expression_stmt(region r, location loc, expression e)
{
  expression_stmt es = new_expression_stmt(r, loc, e);
  set_parent(CASTSRPTR(node, &es->arg1), CAST(node, es));
  return CAST(statement, es);
}

label build_id_label(region r, location loc, label_declaration name)
{
  id_label l = new_id_label(r, loc, str2cstring(r, name->name));
  l->ldecl = name;
  return CAST(label, l);
}

statement build_labeled_stmt(region r, location loc, label l, statement s)
{
  labeled_stmt ls = new_labeled_stmt(r, loc, l, s);
  set_parent(CASTSRPTR(node, &ls->label), CAST(node, ls));
  set_parent(CASTSRPTR(node, &ls->stmt), CAST(node, ls));
  return CAST(statement, ls);
}

statement build_default(region r, location loc, statement s)
{
  label dlabel = CAST(label, new_default_label(r, loc));
  labeled_stmt ls = new_labeled_stmt(r, loc, dlabel, s);

  AST_set_parents(CAST(node, ls));
  return CAST(statement, ls);
}

statement build_goto(region r, location loc, id_label l)
{
  goto_stmt gs = new_goto_stmt(r, loc, l);
  set_parent(CASTSRPTR(node, &gs->id_label), CAST(node, gs));
  return CAST(statement, gs);
}

statement build_break_stmt(region r, location loc)
{
  return CAST(statement, new_break_stmt(r, loc));
}

statement build_simple_block(region r, location loc, function_decl fd,
			     statement stmts)
{
  /* XXX: the environment is not linked to the enclosing blocks. 
     (Easy to get a parent, but should we link enclosed environments of
     stmts into this env ?) */
  compound_stmt block = new_compound_stmt(r, loc, NULL, NULL, stmts,
					  new_environment(r, NULL, FALSE));
  block->env->fdecl = fd;
  set_parent_list(CASTSRPTR(node, &block->stmts), CAST(node, block));
  return CAST(statement, block);
}

statement build_for_stmt(region r, location loc, expression e1,
			 expression e2, expression e3, statement body)
{
  for_stmt fs = new_for_stmt(r, loc, e1, e2, e3, body);

  set_parent(CASTSRPTR(node, &fs->arg1), CAST(node, fs));
  set_parent(CASTSRPTR(node, &fs->arg2), CAST(node, fs));
  set_parent(CASTSRPTR(node, &fs->arg3), CAST(node, fs));
  set_parent(CASTSRPTR(node, &fs->stmt), CAST(node, fs));

  return CAST(statement, fs);
}

void append_block(compound_stmt block, statement s)
{
  /* Special handling for blocks which are compound expressions: add before
     the last statement if the last statement is an expression statement
     (this is valid because the fix stage stabilises the last expression,
     and is necessary to avoid changing the value of the block) */
  if (is_compound_expr(block->parent))
    {
      statement last_stmt = CAST(statement, last_node(CAST(node, block->stmts)));

      if (last_stmt && is_expression_stmt(last_stmt))
	{
	  node sameregion *pp = last_stmt->parent_ptr;

	  *pp = CAST(node, s);
	  s->parent_ptr = pp;
	  s->parent = CAST(node, block);
	  s->next = CAST(node, last_stmt);
	  last_stmt->parent_ptr = &s->next;
	  return;
	}
    }

  statement_chain(block->stmts, s);
  set_parent_list(CASTSRPTR(node, &block->stmts), CAST(node, block));
}

void insert_block(compound_stmt block, statement before, statement s)
{
  insert_before(CASTSRPTR(node, &block->stmts), CAST(node, before), CAST(node, s));
  set_parent_list(CASTSRPTR(node, &block->stmts), CAST(node, block));
}

void append_stmt(region r, statement sameregion *after, statement s)
{
  /* If *after is a block, we can just put s at the end */
  if (is_compound_stmt(*after))
    append_block(CAST(compound_stmt, *after), s);
  /* If *after is in a block, just insert s after *after */
  else if (is_compound_stmt((*after)->parent))
    insert_block(CAST(compound_stmt, (*after)->parent),
		 CAST(statement, (*after)->next), s);
  else
    {
      node oldparent = (*after)->parent;
      function_decl fn = parent_function(CAST(node, *after));

      AST_SET_NEXT(*after, CAST(node, s));
      *after = build_simple_block(r, s->location, fn, *after);
      set_parent(CASTSRPTR(node, after), oldparent);
    }
}

void prepend_stmt(region r, statement sameregion *before, statement s)
{
  /* If *before is block, we can just put s at the start (this is safe
     because we prohibit locals shadowing locals and because all initialisers
     have been removed) */
  if (is_compound_stmt(*before))
    {
      compound_stmt block = CAST(compound_stmt, *before);
      insert_block(block, block->stmts, s);
    }
  /* If *before is in a block, just insert s in front of *before */
  else if (is_compound_stmt((*before)->parent))
    insert_block(CAST(compound_stmt, (*before)->parent), *before, s);
  else
    {
      node oldparent = (*before)->parent;
      function_decl fn = parent_function(CAST(node, *before));

      AST_SET_NEXT(s, CAST(node, *before));
      *before = build_simple_block(r, s->location, fn, s);
      set_parent(CASTSRPTR(node, before), oldparent);
    }
}

void append_expr(region r, expression sameregion *after, expression e)
{
  expression old = *after;
  node next = old->next;
  node oldparent = old->parent;
  expression comma;

  old->next = NULL;
  comma = build_comma(r, e->location, old, e);

  *after = comma;
  set_parent(CASTSRPTR(node, after), oldparent);
  AST_SET_NEXT(comma, next);
}

void prepend_expr(region r, expression sameregion *before, expression e)
{
  expression old = *before;
  node next = old->next;
  node oldparent = old->parent;
  expression comma;

  old->next = NULL;
  comma = build_comma(r, e->location, e, old);
  *before = comma;
  set_parent(CASTSRPTR(node, before), oldparent);
  AST_SET_NEXT(comma, next);
}

void insert_comma(comma ce, expression before, expression e)
{
  insert_before(CASTSRPTR(node, &ce->arg1), CAST(node, before), CAST(node, e));
  set_parent_list(CASTSRPTR(node, &ce->arg1), CAST(node, ce));
}

/* WARNING: Be careful of using stabilise with constant expressions. A
   temporary is not the same as a constant expression. Use stabilise_cst
   where this is an issue */
data_declaration stabilise(region r, compound_stmt block, expression e,
			   expression *stabiliser)
{
  data_declaration tdecl = declare_temporary(r, block, e->type);
  location loc = e->location;
  expression temp = build_identifier(r, loc, tdecl);

  *stabiliser = build_assignment(r, loc, kind_assign, temp, e);

  return tdecl;
}

/* WARNING: See stabilise above */
data_declaration stabilise1(region r, compound_stmt block, expression e,
			    expression *temp, expression *stabiliser)
{
  gplace erregion = e->rregion;
  data_declaration tdecl = stabilise(r, block, e, stabiliser);

  *temp = build_identifier(r, e->location, tdecl);
  (*temp)->rregion = erregion;

  return tdecl;
}

/* WARNING: See stabilise above */
data_declaration stabilise2(region r, compound_stmt block, expression e,
			    expression *temp1, expression *temp2,
			    expression *stabiliser)
{
  gplace erregion = e->rregion;
  data_declaration tdecl = stabilise(r, block, e, stabiliser);

  *temp1 = build_identifier(r, e->location, tdecl);
  (*temp1)->rregion = erregion;
  *temp2 = build_identifier(r, e->location, tdecl);
  (*temp2)->rregion = erregion;

  return tdecl;
}

/* Sets *temp = e and returns NULL for constant expressions e. */
data_declaration stabilise_cst(region r, compound_stmt block, expression e,
			       expression *temp, expression *stabiliser)
{
  if (e->cst)
    {
      *stabiliser = NULL;
      *temp = e;
      return NULL;
    }
  else
    return stabilise1(r, block, e, temp, stabiliser);
}

data_declaration stabilise_lvalue(region r, compound_stmt block,
				  expression lvalue,
				  expression *use, expression *stabiliser)
{
  data_declaration temp = NULL;

  switch (lvalue->kind)
    {
    case kind_dereference: {
      dereference dr = CAST(dereference, lvalue);
      expression newarg1;

      temp = stabilise_cst(r, block, dr->arg1, &newarg1, stabiliser);
      dr->arg1 = newarg1;
      set_parent(CASTSRPTR(node, &dr->arg1), CAST(node, dr));
      *use = lvalue;
      break;
    }
    case kind_identifier: {
      *use = lvalue;
      *stabiliser = NULL;
      temp = NULL;
      break;
    }
    case kind_field_ref: {
      field_ref fr = CAST(field_ref, lvalue);
      expression newarg1;
      temp = stabilise_lvalue(r, block, fr->arg1, &newarg1, stabiliser);
      fr->arg1 = newarg1;
      set_parent(CASTSRPTR(node, &fr->arg1), CAST(node, fr));
      *use = lvalue;
      break;
    }
    default: assert(0); break;
    }
  return temp;
}

/* Replace 'after' by an expression that evaluates 'e' after 'after', but
   returns the value of 'after'
   Requires: 'after' be used for its rvalue (replaces 'after' by
     "(t='after','e',t)" for some new local t
*/
static void evaluate_after(region r, expression e, expression after)
{
  expression stabiliser, temp;
  node sameregion *ptr = after->parent_ptr, parent = after->parent, newafter;

  stabilise_cst(r, parent_block(CAST(node, after)), after, &temp, &stabiliser);
  stabiliser = expression_chain(stabiliser, e);
  newafter = CAST(node, build_comma(r, dummy_location, stabiliser, temp));
  AST_SET(parent, ptr, newafter);
}

static void evaluate_after_lvalue(region r, expression e, expression sameregion *after)
{
  expression stabiliser;
  node parent = (*after)->parent;
  expression newafter;

  stabilise_lvalue(r, parent_block(parent), *after, &newafter, &stabiliser);
  *after = newafter;
  set_parent(CASTSRPTR(node, after), parent);
  /* Build (t = e0, expr, <parent> e1) */
  stabiliser = expression_chain(stabiliser, e);
  prepend_expr(r, CASTSRPTR(expression, parent->parent_ptr), stabiliser);
}

static expression last_expr_of_initialiser(expression init)
{
  for (;;)
    switch (init->kind)
      {
      case kind_init_index:
	init = CAST(init_index, init)->init_expr;
	break;
      case kind_init_field:
	init = CAST(init_field, init)->init_expr;
	break;
      case kind_init_list:
	init = CAST(expression,
		    last_node(CAST(node, CAST(init_list, init)->args)));
	break;
      default:
	return init;
      }
}

/* Add expression EXPR to the ast such that it will be evaluated only when
   edge E is followed */
/* Requires: correct parent/parent_ptr. */
/* Note: Ensure that code is placed in the innermost scope of nodes
   e->from, e->to, e->info (XXX: not true yet)
   Note: Editing must not remove nodes from the AST, otherwise future
   edits might get lost */
void add_expression_to_edge(region r, edge e, expression expr)
{
  switch (e->kind)
    {
    case edge_fn_entry:
      /* Add expr; to beginning of function */
      prepend_stmt(r, &CAST(function_decl, e->info)->stmt,
		   build_expression_stmt(r, expr->location, expr));
      break;

    case edge_fn_exit:
      /* Add expr; to end of function's block */
      append_block(CAST(compound_stmt, CAST(function_decl, e->info)->stmt),
		   build_expression_stmt(r, expr->location, expr));
      break;

    case edge_compound: {
      /* Add expr; before statement e->info */
      prepend_stmt(r, CASTSRPTR(statement, e->info->parent_ptr),
		   build_expression_stmt(r, expr->location, expr));
      break;
    }
    case edge_compound_end:
      /* Add expr; to end of function's block */
      append_block(CAST(compound_stmt, e->to),
		   build_expression_stmt(r, expr->location, expr));
      break;

    case edge_if_true:
      /* Add expr; to start of true statement */
      prepend_stmt(r, &CAST(if_stmt, e->info)->stmt1, 
		   build_expression_stmt(r, expr->location, expr));
      break;

    case edge_if_true_done:
      /* Add expr; to end of true statement */
      append_stmt(r, &CAST(if_stmt, e->info)->stmt1, 
		  build_expression_stmt(r, expr->location, expr));
      break;

    case edge_if_false:
      /* Add expr; to start of false statement */
      prepend_stmt(r, &CAST(if_stmt, e->info)->stmt2, 
		   build_expression_stmt(r, expr->location, expr));
      break;

    case edge_if_false_done:
      /* Add expr; to start of true statement */
      append_stmt(r, &CAST(if_stmt, e->info)->stmt2, 
		  build_expression_stmt(r, expr->location, expr));
      break;

    case edge_if_no_else: {
      /* Add expr; as 'else' branch */
      if_stmt is = CAST(if_stmt, e->info);
      statement newelse = build_expression_stmt(r, expr->location, expr);

      AST_SET_FIELD(is, stmt2, newelse);
      break;
    }
    case edge_label:
      /* Add expr; to start of labeled statement */
      prepend_stmt(r, &CAST(labeled_stmt, e->info)->stmt, 
		   build_expression_stmt(r, expr->location, expr));
      break;

    case edge_while_true:
      /* Add expr; to start of loop body */
      prepend_stmt(r, &CAST(while_stmt, e->info)->stmt, 
		   build_expression_stmt(r, expr->location, expr));
      break;

    case edge_while_repeat:
      /* Add expr; to end of loop body */
      append_stmt(r, &CAST(while_stmt, e->info)->stmt, 
		  build_expression_stmt(r, expr->location, expr));
      break;

    case edge_while_done:
      /* Add expr; after while statement */
      append_stmt(r, CASTSRPTR(statement, CAST(while_stmt, e->info)->parent_ptr),
		  build_expression_stmt(r, expr->location, expr));
      break;

    case edge_dowhile_dummy:
      /* Add expr; to start of loop body */
      prepend_stmt(r, &CAST(dowhile_stmt, e->info)->stmt, 
		   build_expression_stmt(r, expr->location, expr));
      break;

    case edge_dowhile_test:
      /* Add expr; to end of loop body */
      append_stmt(r, &CAST(dowhile_stmt, e->info)->stmt, 
		  build_expression_stmt(r, expr->location, expr));
      break;

    case edge_dowhile_true: {
      /* replace condition by (t = condition, (t ? e : 0), t) */
      dowhile_stmt dowhile = CAST(dowhile_stmt, e->info);
      expression stabiliser, temp1, temp2, condexpr, ce;
      location loc = dowhile->condition->location;

      if (dowhile->condition->cst)
	/* Just place at the start of the statement */
	prepend_stmt(r, &dowhile->stmt, 
		     build_expression_stmt(r, expr->location, expr));
      else
	{
	  /* XXX: assumes that expr has type which is compatible with 0
	     in a ?: */
	  stabilise2(r, parent_block(CAST(node, dowhile)), dowhile->condition, 
		     &temp1, &temp2, &stabiliser);
	  condexpr = build_conditional(r, loc, int_type, temp1, expr, build_zero(r, loc));
	  AST_SET_NEXT(stabiliser, CAST(node, condexpr));
	  ce = build_comma(r, loc, stabiliser, temp2);
	  AST_SET_FIELD(dowhile, condition, ce);
	}
      break;
    }
    case edge_dowhile_done:
      /* Add expr; after do-while statement */
      append_stmt(r, CASTSRPTR(statement, CAST(dowhile_stmt, e->info)->parent_ptr),
		  build_expression_stmt(r, expr->location, expr));
      break;

    case edge_switch: {
      labeled_stmt ls = CAST(labeled_stmt, e->to->parent);

      /* If case has no incoming edges other than from the switch
	 expression then it's easy: just prepend to labeled statement.
	 If we have already added the 'skip over case' stuff, we
	 can also do this the easy way */
      if (!e->to->edges_in->next_in || e->nice)
	prepend_stmt(r, &ls->stmt,
		     build_expression_stmt(r, expr->location, expr));
      else
	{
	  /* Rewrite 'label: s' as 'goto foo; label: { expr; foo: s}' */
	  label_declaration newlabel = declare_new_label(r);
	  location loc = ls->location;
	  label newlabel1 = build_id_label(r, loc, newlabel);
	  label newlabel2 = build_id_label(r, loc, newlabel);
	  statement skipovercase = build_goto(r, loc, CAST(id_label, newlabel1));
	  statement nl2s;
	  
	  prepend_stmt(r, CASTSRPTR(statement, ls->parent_ptr), skipovercase);
	  nl2s = build_labeled_stmt(r, loc, newlabel2, ls->stmt);
	  AST_SET_FIELD(ls, stmt, nl2s);
	  prepend_stmt(r, &ls->stmt,
		       build_expression_stmt(r, expr->location, expr));
	  e->nice = TRUE; /* Don't repeat this work. */
	}
      break;
    }
    case edge_switch_fallout:
      /* Add expr; to end of switch statement */
      append_stmt(r, &CAST(switch_stmt, e->info)->stmt,
		  build_expression_stmt(r, expr->location, expr));
      break;

    case edge_switch_nodefault: {
      /* Add "default: expr; break;" to start of switch statement */
      statement sameregion *swbody = &CAST(switch_stmt, e->info)->stmt;
      location loc = (*swbody)->location;
      statement defstmt =
	build_default(r, loc, build_expression_stmt(r, expr->location, expr));

      prepend_stmt(r, swbody, build_break_stmt(r, loc));
      prepend_stmt(r, swbody, defstmt);
      break;
    }
    case edge_for_enterloop:
      /* Add expr to end of first expression */
      append_expr(r, &CAST(for_stmt, e->info)->arg1, expr);
      break;

    case edge_for_exitloop:
      /* Add expr; after for statement */
      append_stmt(r, CASTSRPTR(statement, CAST(for_stmt, e->info)->parent_ptr),
		  build_expression_stmt(r, expr->location, expr));
      break;

    case edge_for_true:
      /* Add expr; to start of loop body */
      prepend_stmt(r, &CAST(for_stmt, e->info)->stmt, 
		   build_expression_stmt(r, expr->location, expr));
      break;

    case edge_for_stmtdone:
      /* Add expr; to end of loop body */
      append_stmt(r, &CAST(for_stmt, e->info)->stmt, 
		  build_expression_stmt(r, expr->location, expr));
      break;

    case edge_for_continue: {
      /* Add expr to start of third expression */
      for_stmt fs = CAST(for_stmt, e->info);

      if (fs->arg3)
	prepend_expr(r, &fs->arg3, expr);
      else
	AST_SET_FIELD(fs, arg3, expr);
      break;
    }
    case edge_for_repeat:
      /* Add expr to end of third expression */
      append_expr(r, &CAST(for_stmt, e->info)->arg3, expr);
      break;

    case edge_break:
      /* Add expr; before break */
      prepend_stmt(r, CASTSRPTR(statement, e->from->parent_ptr),
		   build_expression_stmt(r, expr->location, expr));
      break;

    case edge_continue:
      /* Add expr; before continue */
      prepend_stmt(r, CASTSRPTR(statement, e->from->parent_ptr), 
		   build_expression_stmt(r, expr->location, expr));
      break;

    case edge_goto:
      /* Add expr; before goto */
      prepend_stmt(r, CASTSRPTR(statement, e->from->parent_ptr), 
		   build_expression_stmt(r, expr->location, expr));
      break;

    case edge_computed_goto:
      abort();
      break;

    case edge_return_expr:
      /* Replace 'return arg;' by 'return (t=arg,expr,t);' */
      evaluate_after(r, expr, CAST(return_stmt, e->info)->arg1);
      break;

    case edge_return: 
      /* Add expr; before return; */
      prepend_stmt(r, CASTSRPTR(statement, e->info->parent_ptr), 
		   build_expression_stmt(r, expr->location, expr));
      break;

    case edge_comma: case edge_init_list_comma:
      /* Add expr before second expression */
      prepend_expr(r, CASTSRPTR(expression, e->info->parent_ptr), expr);
      break;

    case edge_cast_list:
      /* Evaluate expr after last initialiser expression */
      evaluate_after
	(r, expr, last_expr_of_initialiser(CAST(cast_list, e->to)->init_expr));
      break;

    case edge_conditional_true:
      /* Add expr to start of first expression */
      prepend_expr(r, &CAST(conditional, e->info)->arg1, expr);
      break;

    case edge_conditional_true_done:
      /* Replace "arg1" by "(t=arg1,expr,t)" */
      evaluate_after(r, expr, CAST(conditional, e->info)->arg1);
      break;

    case edge_conditional_false:
      /* Add expr to start of second expression */
      prepend_expr(r, &CAST(conditional, e->info)->arg2, expr);
      break;

    case edge_conditional_false_done:
      /* Replace "arg2" by "(t=arg2,expr,t)" */
      evaluate_after(r, expr, CAST(conditional, e->info)->arg2);
      break;

    case edge_short_second:
      /* Replace arg1 op arg2 by arg1 op (expr, arg2) */
      prepend_expr(r, &CAST(binary, e->info)->arg2, expr);
      break;

    case edge_short_done: 
      /* Replace arg1 op arg2 by arg1 op (t = arg2, expr, t) */
      evaluate_after(r, expr, CAST(binary, e->info)->arg2);
      break;

    case edge_short_circuit: {
      /* replace arg1 && arg2 by (t = arg1, (t ? 1 : (e, 0))) && arg2,
	 replace arg1 || arg2 by (t = arg1, (t ? (e, 1) : 0)) || arg2 */
      binary be = CAST(binary, e->info);
      expression stabiliser, temp, cond, ct, cf;
      location loc = be->arg1->location;
      expression ce;

      stabilise_cst(r, parent_block(CAST(node, be)), be->arg1,
		    &temp, &stabiliser);
      cf = build_zero(r, loc);
      ct = build_int_constant(r, loc, int_type, 1);
      if (is_andand(be))
	cf = build_comma(r, loc, expr, cf);
      else
	ct = build_comma(r, loc, expr, ct);
      cond = build_conditional(r, loc, int_type, temp, ct, cf);
      ce = stabiliser ? build_comma(r, loc, stabiliser, cond) : cond;
      AST_SET_FIELD(be, arg1, ce);
      break;
    }
    case edge_function_call:
      /* Replace "f(args)" by "(t=f,expr,t)(args)" */
      evaluate_after(r, expr, CAST(function_call, e->to)->arg1);
      break;

    case edge_unary:
      /* Replace "unop arg" by "unop (t=arg,expr,t)" */
      evaluate_after(r, expr, CAST(unary, e->to)->arg1);
      break;

    case edge_unary_lvalue:
      /* Replace "unop arg" by "(t = e0, expr, unop e1)
	 where e1 has no side effects and unop e1 computes the same
	 value as unop arg */
      evaluate_after_lvalue(r, expr, &CAST(unary, e->to)->arg1);
      break;

    case edge_binary:
      /* Replace "arg1 binop x" by "(t = arg1, expr, t) binop x"  */
      evaluate_after(r, expr, CAST(binary, e->to)->arg1);
      break;

    case edge_binary_lvalue:
      /* Replace "arg1 binop x" by "(t = e0, expr, e1 binop x)".
	 See unary_lvalue */
      evaluate_after_lvalue(r, expr, &CAST(binary, e->to)->arg1);
      break;

    case edge_assign_local: {
      /* Replace "localvar.fs = arg2" by "(t = arg2, expr, localvar.fs = t)" */
      assign ae = CAST(assign, e->to);
      expression stabiliser, newarg2;

      stabilise_cst(r, parent_block(CAST(node, ae)), ae->arg2, &newarg2,
		    &stabiliser);
      ae->arg2 = newarg2;
      set_parent(CASTSRPTR(node, &ae->arg2), CAST(node, ae));
      /* Build (t = arg1, expr, localvar = t) */
      stabiliser = expression_chain(stabiliser, expr);
      prepend_expr(r, CASTSRPTR(expression, ae->parent_ptr), stabiliser);
      break;
    }
    default:
      assert(0);
    }
}

expression copy_lvalue(region r, location loc, expression lvalue)
{
  switch (lvalue->kind)
    {
    case kind_dereference: {
      dereference dr = CAST(dereference, lvalue);
      return build_dereference(r, loc, copy_lvalue(r, loc, dr->arg1));
    }
    case kind_identifier: {
      return build_identifier(r, loc, CAST(identifier, lvalue)->ddecl);
    }
    case kind_field_ref: {
      field_ref fr = CAST(field_ref, lvalue);
      return build_field_ref(r, loc, copy_lvalue(r, loc, fr->arg1),
			     fr->cstring.data);
    }
    default: assert(0); return NULL;
    }
}
