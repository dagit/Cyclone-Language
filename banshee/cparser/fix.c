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
#include "fix.h"
#include "edit.h"
#include "c-parse.h"
#include "dd_list.h"
#include "semantics.h"
#include "edit.h"
#include "builtins.h"
#include "constants.h"
#include "AST_utils.h"

/* True if &lvalue and &v may be the same location.
   Requires: lvalue be in fixed form. */
static bool might_alias(data_declaration v, expression lvalue)
{
  lvalue = ignore_fields(lvalue);

  return !(v->islocal && !v->addressed &&
	   !(is_identifier(lvalue) && CAST(identifier, lvalue)->ddecl == v));
}

/* True if execution of 'e' might modify v.
   Requires: 'e' be a fixed expression */
static bool might_modify(data_declaration v, expression e)
{
  switch (e->kind)
    {
    case kind_preincrement: case kind_postincrement:
    case kind_predecrement: case kind_postdecrement:
      return might_alias(v, CAST(unary, e)->arg1);

    default:
      if (is_assignment(e))
	return might_alias(v, CAST(binary, e)->arg1);
      else
	return FALSE;

    case kind_comma: {
      expression elist;

      scan_expression (elist, CAST(comma, e)->arg1)
	if (might_modify(v, elist))
	  return TRUE;

      return FALSE;
    }
    case kind_conditional:  {
      conditional ce = CAST(conditional, e);

      return might_modify(v, ce->condition) ||
	might_modify(v, ce->arg1) || might_modify(v, ce->arg2);
    }
    case kind_compound_expr:
      return TRUE; /* Lazy, etc */

    case kind_function_call: {
      function_call fce = CAST(function_call, e);
      return might_modify(v, fce->arg1) || !v->islocal || v->addressed;
    }
    }
}

/* True if 'e' is a constant, or an identifier (with optional field accesses)
   that can not be modified by the execution of 'before'. 
   'before' can be NULL (it is then ignored).
   Requires: 'e', 'before' be fixed expressions. */
static bool legal_arg(expression e, expression before)
{
  e = ignore_fields(e);

  if (e->cst)
    return TRUE;

  return is_identifier(e) &&
    !(before && might_modify(CAST(identifier, e)->ddecl, before));
}

/* legal_arg with a side-effect-free 'before' expression 
   (so legal(e, before) => simple_arg(e)) */
bool simple_arg(expression e)
{
  return legal_arg(e, NULL);
}

bool simple_lvalue(expression lvalue)
{
  if (lvalue->kind == kind_dereference)
    return simple_arg(CAST(dereference, lvalue)->arg1);
  else
    return simple_arg(lvalue);
}

#if 0
/* TRUE if e is a "complex" lvalue (i.e. not one of the normal C
   lvalues) */
static bool complex_lvalue(expression e)
{
  int kind = e->kind;

  return kind == kind_cast || kind == kind_conditional || kind == kind_comma;
}
#endif

expression fix_expr(region r, expression expr, bool addressed,
		    compound_stmt block);
statement fix_stmt(region r, statement stmt, compound_stmt block);

static void fix_expr_list(region r, expression sameregion *elist, bool addressed,
		   compound_stmt block)
{
  while ((*elist))
    {
      node next = (*elist)->next;
      expression newexpr;

      newexpr = fix_expr(r, *elist, addressed && !next, block);
      newexpr->next = next; /* May be a new expression */
      *elist = newexpr;
      elist = CASTSRPTR(expression, &newexpr->next);
    }
}

expression stabilise_elist(region r, expression *elist, compound_stmt block,
			   expression before)
{
  expression saved = NULL, *lastsaved = &saved;
  
  while ((*elist))
    {
      node next = (*elist)->next;
      expression newexpr;

      newexpr = fix_expr(r, *elist, FALSE, block);
      if (!legal_arg(newexpr, before))
	{
	  /* Stabilise newexpr by adding its stabilised form to saved
	     early args and replacing the expression by the stabilised
	     temporary */
	  stabilise1(r, block, newexpr, &newexpr, lastsaved);
	  lastsaved = (expression *)&(*lastsaved)->next;
	}
      newexpr->next = next;
      *elist = newexpr;
      elist = (expression *)&newexpr->next;
    }
  return saved;
}

expression stabilise_asm_operands(region r, asm_operand alist, compound_stmt block, bool outputs)
{
  expression saved = NULL, *lastsaved = &saved;
  
  while (alist)
    {
      expression newexpr;

      newexpr = fix_expr(r, alist->arg1, outputs, block);
      /* Use simple_arg even with outputs so that we see the uses of
	 x in *x (will become tmp = x, asm( ... (*tmp) ...)) */
      if (!simple_arg(newexpr))
	{
	  /* Stabilise newexpr by adding its stabilised form to saved
	     early args and replacing the expression by the stabilised
	     temporary */
	  (outputs ? stabilise_lvalue : stabilise1)
	    (r, block, newexpr, &newexpr, lastsaved);
	  lastsaved = (expression *)&(*lastsaved)->next;
	}
      alist->arg1 = newexpr;
      alist = CAST(asm_operand, alist->next);
    }
  return saved;
}

static expression prepend(region r, expression elist, expression e)
{
  e->next = NULL;
  return build_comma(r, e->location, elist, e);
}

/* Fix expr as described in fix_parse_tree. addressed is true for lvalue
   expressions that are arguments to '&' or are output arguments of asm
   statements (in which case, the local variables that are named by the
   lvalue must be removed from vars).
   Returns the new expression that replaces expr */
expression fix_expr(region r, expression expr, bool addressed,
		    compound_stmt block)
{
  switch (expr->kind)
    {
    case kind_sizeof_type: case kind_alignof_type:
    case kind_sizeof_expr: case kind_alignof_expr: 
    case kind_label_address:
      break;

    case kind_comma:
      fix_expr_list(r, &CAST(comma, expr)->arg1, addressed, block);
      break;

    case kind_conditional: {
      conditional ce = CAST(conditional, expr);

      ce->condition = fix_expr(r, ce->condition, FALSE, block);
      if (ce->arg1)
	ce->arg1 = fix_expr(r, ce->arg1, addressed, block);
      ce->arg2 = fix_expr(r, ce->arg2, addressed, block);

      /* Rewrite e1 ? : e2 to (t = e1, t ? t : e2) */
      if (!ce->arg1)
	{
	  expression stabiliser, newarg1, newcondition;

	  stabilise2(r, block, ce->condition,
		     &newcondition, &newarg1, &stabiliser);
	  ce->condition = newcondition;
	  ce->arg1 = newarg1;
	  return prepend(r, stabiliser, expr);
	}
      break;
    }
    case kind_identifier: {
      identifier id = CAST(identifier, expr);
      data_declaration dd = id->ddecl;
      if (dd->islocal && addressed)
	dd->addressed = TRUE;
      break;
    }
    case kind_compound_expr: {
      compound_expr ce = CAST(compound_expr, expr);
      compound_stmt blk;
      statement last_stmt;
      expression_stmt es;
      expression result;
;
      /* Luckily, the last statement cannot be used as an lvalue ;-) */
      ce->stmt = fix_stmt(r, ce->stmt, block);

      /* Stabilise the last statement of the block if it's an expression - we may
	 want to add code to the end of a compound expression block */
      /* XXX: Could check for simple expressions where the extra temp is not needed */
      blk = CAST(compound_stmt, ce->stmt);
      last_stmt = CAST(statement, last_node(CAST(node, blk->stmts)));
      if (last_stmt && is_expression_stmt(last_stmt))
	{
	  expression newarg1;
	  es = CAST(expression_stmt, last_stmt);
	  stabilise1(r, block, es->arg1, &result, &newarg1);
	  es->arg1 = newarg1;
	  last_stmt->next = CAST(node, new_expression_stmt(r, dummy_location, result));
	}
      break;
    }
    case kind_cast_list: {
      /* XXX: Am assuming that initialisers are evaluated left-to-right.
	 No spec in gcc info as far as I can tell. */
      fix_expr(r, CAST(cast_list, expr)->init_expr, FALSE, block);
      break;
    }
    case kind_init_index: {
      init_index init = CAST(init_index, expr);

      init->init_expr = fix_expr(r, init->init_expr, addressed, block);
      break;
    }
    case kind_init_field: {
      init_field init = CAST(init_field, expr);

      init->init_expr = fix_expr(r, init->init_expr, addressed, block);
      break;
    }
    case kind_init_list: {
      /* XXX: Am assuming that initialisers are evaluated left-to-right.
	 No spec in gcc info as far as I can tell. */
      fix_expr_list(r, &CAST(init_list, expr)->args, FALSE, block);
      break;
    }
    case kind_function_call: {
      function_call fce = CAST(function_call, expr);
      expression sameregion *elist;
      expression args = NULL, *lastarg = &args;

      /* Not a real function */
      if (call_to(gcc_builtin_classify_type, fce))
	break;

      fce->arg1 = fix_expr(r, fce->arg1, FALSE, block);

      elist = &fce->args;
      while ((*elist))
	{
	  node next = (*elist)->next;
	  expression newexpr;

	  newexpr = fix_expr(r, *elist, FALSE, block);
	  if (!legal_arg(newexpr, fce->arg1))
	    {
	      /* Stabilise newexpr by adding its stabilised form to the list of
		 early args and replacing the function arg by the stabilised
		 temporary */
	      stabilise1(r, block, newexpr, &newexpr, lastarg);
	      lastarg = (expression *)&(*lastarg)->next;
	    }
	  newexpr->next = next;
	  *elist = newexpr;
	  elist = CASTSRPTR(expression, &newexpr->next);
	}

      /* Build a comma expression if any stabilised args */
      if (args)
	return prepend(r, args, expr);

      break;
    }
    case kind_andand: case kind_oror: {
      binary be = CAST(binary, expr);

      be->arg1 = fix_expr(r, be->arg1, FALSE, block);
      be->arg2 = fix_expr(r, be->arg2, FALSE, block);
      break;
    }
    case kind_array_ref: {
      /* Rewrite as *(arg1+arg2) */
      /* XXX: Should only do this when in lvalue position */
      array_ref ae = CAST(array_ref, expr);
      expression sum = build_pointer_plus(r, ae->location, ae->arg1, ae->arg2);
      expression deref = build_dereference(r, ae->location, sum);
      return fix_expr(r, deref, addressed, block);
    }
    default:
      if (is_unary(expr))
	{
	  unary ue = CAST(unary, expr);
	  bool addressop = ue->kind == kind_address_of ||
	    (addressed && ue->kind == kind_field_ref);

	  /* Eliminate *& sequences (improves RC behaviour as
	     vars whose address is taken are not optimised) */
	  if (is_dereference(ue) && is_address_of(ue->arg1))
	    return fix_expr(r, CAST(unary, ue->arg1)->arg1, addressed, block);

	  ue->arg1 = fix_expr(r, ue->arg1, addressop, block);

	}
      else if (is_binary(expr))
	{
	  binary be = CAST(binary, expr);
	  expression arg2;

	  be->arg1 = fix_expr(r, be->arg1, FALSE, block);
	  arg2 = fix_expr(r, be->arg2, FALSE, block);
	  /* Stabilise arg2 rather than arg1 because arg1 is special in
	     assignments (lvalue) and array_ref (weird gcc rules for indexing 
	     non-lvalue arrays) */
	  if ((be->kind == kind_assign && is_localvar(be->arg1)) ||
	      legal_arg(arg2, be->arg1))
	    be->arg2 = arg2;
	  else
	    {
	      expression stabiliser;

	      /* XXX: Hack. Should do something more general. */
	      if ((is_assign(arg2) || is_comma(arg2)) &&
		  zero_expression(arg2))
		{
		  stabiliser = arg2;
		  be->arg2 = build_zero(r, arg2->location);
		}
	      else
		{
		  expression newarg2;

		  stabilise1(r, block, arg2, &newarg2, &stabiliser);
		  be->arg2 = newarg2;
		}

	      return prepend(r, stabiliser, expr);
	    }
	}
      else if (!expr->cst)
	assert(0);
      break;
    }

  return expr;
}

static statement *add_initialisation(region r, compound_stmt block,
				     statement *insertat,
				     expression dest, expression init,
				     bool clearing)
{
  init = build_assignment(r, dest->location, kind_assign, dest, init);
  CAST(assign, init)->clearing = clearing;
  init = fix_expr(r, init, FALSE, block);
  *insertat = build_expression_stmt(r, dest->location, init);
  return CASTPTR(statement, &(*insertat)->next);
}

typedef struct expr_closure {
  expression (*fn)(struct expr_closure *closure, region r);
  location loc;

  data_declaration var;

  struct expr_closure *parent;
  const char *fieldname;
} *expr_closure;

static expression build_var(expr_closure closure, region r)
{
  return build_identifier(r, closure->loc, closure->var);
}

static expression build_field(expr_closure closure, region r)
{
  return build_field_ref(r, closure->loc, CALL1(closure->parent, r),
			 closure->fieldname);
}

static expression build_array(expr_closure closure, region r)
{
  /* Use *(a+i) (we must produce a fixed expression as this is a lhs) */
  expression address =
    build_pointer_plus(r, closure->loc, CALL1(closure->parent, r),
		       build_identifier(r, closure->loc, closure->var));
  return build_dereference(r, closure->loc, address);
}

statement *clear_pointers(region r, compound_stmt block, statement *insertat,
			  location loc, type basetype, expr_closure base)
{
  if (type_pointer(basetype))
    return add_initialisation(r, block, insertat, CALL1(base, r), build_zero(r, loc),
			      TRUE);

  if (type_array(basetype))
    {
      if (type_contains_pointers(basetype))
	{
	  data_declaration i = declare_temporary(r, block, unsigned_long_type);
	  struct expr_closure barray;
	  statement loopbody;
	  expression cleari, testi, inci, sizeof_array;

	  barray.fn = build_array;
	  barray.loc = loc;
	  barray.parent = base;
	  barray.var = i;

	  clear_pointers(r, block, &loopbody, loc, type_array_of(basetype),
			 &barray);
	  loopbody = build_simple_block(r, loc, block->env->fdecl, loopbody);

	  sizeof_array = build_arithmetic
	    (r, loc, kind_divide,
	     build_sizeof_expr(r, loc, CALL1(base, r)),
	     build_sizeof_expr(r, loc,
			       build_dereference(r, loc, CALL1(base, r))));
	  cleari = build_assignment(r, loc, kind_assign,
				    build_identifier(r, loc, i),
				    build_zero(r, loc));
	  testi = build_lt(r, loc, build_identifier(r, loc, i),
			   sizeof_array);
	  inci = build_increment(r, loc, kind_postincrement,
				 build_identifier(r, loc, i));
	  *insertat = 
	    fix_stmt(r, build_for_stmt(r, loc, cleari, testi, inci, loopbody), block);

	  return CASTPTR(statement, &(*insertat)->next);
	}
    }
  else if (type_struct(basetype) || type_union(basetype))
    {
      tag_declaration tag = type_tag(basetype);
      field_declaration field;
      struct expr_closure bfield;

      bfield.fn = build_field;
      bfield.loc = loc;
      bfield.parent = base;
      
      for (field = tag->fieldlist; field; field = field->next)
	{
	  bfield.fieldname = field->name;
	  insertat = clear_pointers(r, block, insertat,
				    loc, field->type, &bfield);
	}
    }

  return insertat;
}

/* Fix declaration as described in fix_parse_tree */
void fix_declarations(region r, compound_stmt block)
{
  statement initialisers = NULL, *last_initialiser = &initialisers;
  statement clearers = NULL, *last_clearer = &clearers;
  declaration d;

  scan_declaration (d, block->decls)
    {
      declaration reald = d;
      variable_decl vd;

      while (is_extension_decl(reald))
	reald = CAST(extension_decl, reald)->decl;

      if (is_data_decl(reald))
	scan_variable_decl (vd, CAST(variable_decl,
				     CAST(data_decl, reald)->decls))
	  {
	    data_declaration decl = vd->ddecl;

	    if (decl->kind == decl_variable && decl->islocal)
	      {
		if (type_pointer(decl->type) || type_struct(decl->type) ||
		    type_union(decl->type) || type_array(decl->type))
		  {
		    /* Add initialisation of all pointers to NULL. We do
		       this even if the variable has an initialiser, as the
		       variable is in scope in its initialisation (so int
		       *x = x is legal...)  */
		    struct expr_closure bvar;

		    bvar.fn = build_var;
		    bvar.loc = vd->location;
		    bvar.var = decl;
		    last_clearer =
		      clear_pointers(r, block, last_clearer, vd->location,
				     decl->type, &bvar);
		  }
		if (vd->arg1)
		  {
		    expression init = build_identifier(r, vd->location, decl);

		    if (is_init_list(vd->arg1))
		      {
			/* Make a cast_list expression instead. */
			location l = vd->arg1->location;
			type_element vdt_elems;
			declarator vdt_decl;
			asttype vdt;

			type2ast(parse_region, l, decl->type, NULL,
				 &vdt_decl, &vdt_elems);
			vdt = new_asttype(parse_region, l, vdt_decl, vdt_elems);

			/* XXX: Clean up at some point (see c-parse.y) */
			vd->arg1 = CAST(expression, new_cast_list(r, l, vdt, vd->arg1));
			vd->arg1->type = decl->type;
			if (type_array(decl->type))
			  vd->arg1->lvalue = TRUE;
		      }
		    last_initialiser =
		      add_initialisation(r, block, last_initialiser, init, vd->arg1,
					 FALSE);
		    vd->arg1 = NULL;
		  }
	      }
	  }
    }

  block->stmts = statement_chain(clearers,
		 statement_chain(initialisers, block->stmts));
}

statement fix_transfer(region r, compound_stmt block, statement s,
		      node transfer_dest)
{
  label l;
  label_declaration ldecl;
  statement ls;
  location loc = s->location;
  node next = s->next;

  if (!transfer_dest)
    return s;

  ldecl = CAST(id_label, transfer_dest)->ldecl;
  l = build_id_label(r, loc, ldecl);
  ldecl->definition = CAST(id_label, l);
  ls = build_labeled_stmt(r, loc, l, CAST(statement, new_empty_stmt(r, loc)));
  s->next = CAST(node, ls);
  
  s = build_simple_block(r, loc, block->env->fdecl, s);
  s->next = next;

  return s;
}


/* Fix stmt as described in fix_parse_tree */
statement fix_stmt(region r, statement stmt, compound_stmt block)
{
  switch (stmt->kind)
    {
    case kind_asm_stmt: {
      asm_stmt as = CAST(asm_stmt, stmt);
      expression saved_out, saved_in, saved;

      /* outputs */
      saved_out = stabilise_asm_operands(r, as->asm_operands1, block, TRUE);

      /* inputs */
      saved_in = stabilise_asm_operands(r, as->asm_operands2, block, FALSE); 

      saved = expression_chain(saved_out, saved_in);
      if (saved)
	{
	  expression csaved = build_comma2(r, dummy_location, saved);
	  statement ssaved = build_expression_stmt(r, dummy_location, csaved);
	  statement bsaved;
	  node onext;

	  /* Unchain stmt, make list of ssaved, stmt */
	  ssaved->next = CAST(node, stmt);
	  onext = stmt->next;
	  stmt->next = NULL;

	  bsaved = build_simple_block(r, dummy_location, block->env->fdecl,
				      ssaved);
	  bsaved->next = onext;

	  return bsaved;
	}
      break;
    }
    case kind_compound_stmt: {
      compound_stmt cs = CAST(compound_stmt, stmt);
      statement sameregion *slist = &cs->stmts;

      while ((*slist))
	{
	  node next = (*slist)->next;
	  statement newstmt;

	  newstmt = fix_stmt(r, *slist, block);
	  newstmt->next = next; /* May be a new expression */
	  *slist = newstmt;
	  slist = CASTSRPTR(statement, &newstmt->next);
	}

      /* Fix declarations after processing block so we catch the temps generated
	 during block processing */
      fix_declarations(r, cs);

      break;
    }
    case kind_if_stmt: {
      if_stmt is = CAST(if_stmt, stmt);

      is->condition = fix_expr(r, is->condition, FALSE, block);
      is->stmt1 = fix_stmt(r, is->stmt1, block);
      if (is->stmt2)
	is->stmt2 = fix_stmt(r, is->stmt2, block);
      break;
    }
    case kind_labeled_stmt: {
      labeled_stmt ls = CAST(labeled_stmt, stmt);
      ls->stmt = fix_stmt(r, ls->stmt, block);
      break;
    }

    case kind_expression_stmt: {
      expression_stmt es = CAST(expression_stmt, stmt);

      es->arg1 = fix_expr(r, es->arg1, FALSE, block);
      break;
    }
    case kind_while_stmt: case kind_dowhile_stmt: case kind_switch_stmt: {
      conditional_stmt cs = CAST(conditional_stmt, stmt);
      statement newbody;

      cs->condition = fix_expr(r, cs->condition, FALSE, block);
      newbody = fix_stmt(r, cs->stmt, block);
      cs->stmt = fix_transfer(r, block, newbody, cs->continue_dest);

      return fix_transfer(r, block, stmt, cs->break_dest);
    }
    case kind_for_stmt: {
      for_stmt fs = CAST(for_stmt, stmt);
      statement newbody;

      if (fs->arg1)
	fs->arg1 = fix_expr(r, fs->arg1, FALSE, block);
      if (fs->arg2)
	fs->arg2 = fix_expr(r, fs->arg2, FALSE, block);
      if (fs->arg3)
	fs->arg3 = fix_expr(r, fs->arg3, FALSE, block);
      newbody = fix_stmt(r, fs->stmt, block);
      fs->stmt = fix_transfer(r, block, newbody, fs->continue_dest);
      return fix_transfer(r, block, stmt, fs->break_dest);
    }
    case kind_continue_stmt: case kind_break_stmt: {
      node *loop_dest;
      label_declaration transfer_dest;
      id_label transfer_dest_label;
      location loc = stmt->location;

      if (stmt->kind == kind_break_stmt)
	{
	  /* Leave switches alone */
	  if (stmt->parent_loop->kind == kind_switch_stmt)
	    break;
	  loop_dest = &stmt->parent_loop->break_dest;
	}
      else
	loop_dest = &stmt->parent_loop->continue_dest;
      if (*loop_dest)
	transfer_dest = CAST(id_label, *loop_dest)->ldecl;
      else
	transfer_dest = declare_new_label(r);
      transfer_dest_label =
	CAST(id_label, build_id_label(r, loc, transfer_dest));

      *loop_dest = CAST(node, transfer_dest_label);

      return build_goto(r, loc, transfer_dest_label);
    }      

    case kind_goto_stmt: case kind_empty_stmt:
      break;

    case kind_computed_goto_stmt: {
      computed_goto_stmt cgs = CAST(computed_goto_stmt, stmt);

      cgs->arg1 = fix_expr(r, cgs->arg1, FALSE, block);
      break;
    }

    case kind_return_stmt: {
      return_stmt rs = CAST(return_stmt, stmt);

      if (rs->arg1)
	rs->arg1 = fix_expr(r, rs->arg1, FALSE, block);
      break;
    }
    default: assert(0);
    }

  return stmt;
}

/* Change the parse tree of function fd so that:
   - all local pointers are initialised
   - all "complex" lvalues (i.e. involving ?:, casts or the comma operator 
     are eliminated
   - rewrite e1 ? : e2 to (t = e1, t) ? t : e2
   - the order of evaluation of all expressions is well-defined. As a consequence,
     when evaluating a subexpression s of an expression e:
     - the subexpressions s' of e that are "arguments" to s, all but one is stored
       in some local variable
     - the subexpressions s'' of e that are still needed for the computation of e and
       that are not "arguments" to s are stored in some local variable
     these properties ensure that all pointer values are visible at function calls
     and are stored in local variables, which is necessary for computing correct RCs

     More specifically:
     - in a function call e0(e1, ..., en), e1 through en are local variables that
       are not assigned to in e0 or constants
     - in e1 BINOP e2 and e1[e2], e2 is a local variable that is not assigned to in 
       e1 or a constant (for all BINOP except ||, &&)
     - rewrite all array lvalues as *(a+b)

     Replace break/continue in while&dowhile loops by goto's (makes editing
     easier)
*/
void fix_parse_tree(region r, function_decl fd)
{
  fix_stmt(r, fd->stmt, CAST(compound_stmt, fd->stmt));
}
