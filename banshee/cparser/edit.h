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

#ifndef EDIT_H
#define EDIT_H

/* XXX: At this point, the build_xxx functions build AST nodes with correct
   types, but do not set lvalue, side_effects, cst, bitfield, isregister or
   static_address */
/* XXX: Ponder to what extent the corresponding make_xxx and build_xxx
   functions are redundant. What could be merged ? (a possible answer is
   that make_xxx should be using build_xxx) */

/* Declare a new temporary that can be assigned a value of type t.
   Place the declaration at the start of block. 
   XXX: See discussion in types.c:tag2ast about the (lack of) correctness of
   this approach.
   Return it's declaration */
char *next_temporary(void);
data_declaration declare_temporary(region r, compound_stmt block, type t);

/* Return the comma expression allocated in region r, with location loc which
   evaluations expression in list ELIST followed by expression LAST */
expression build_comma(region r, location loc, expression elist, expression last);

/* Return the comma expression allocated in region r, with location loc which
   evaluations expression in list ELIST */
expression build_comma2(region r, location loc, expression elist);

expression build_assignment(region r, location loc, int assigop,
			    expression left, expression right);
expression build_increment(region r, location loc, int incop, expression lvalue);

expression build_conditional(region r, location loc, type result_type,
			     expression cond, expression true, expression false);

expression build_sizeof_expr(region r, location loc, expression e);

expression build_arithmetic(region r, location loc, int kind,
			    expression arg1, expression arg2);
expression build_lt(region r, location loc, expression arg1, expression arg2);

expression build_field_ref(region r, location loc, expression base,
			   const char *name);

statement build_expression_stmt(region r, location loc, expression e);

statement build_default(region r, location loc, statement s);

statement build_break_stmt(region r, location loc);

statement build_simple_block(region r, location loc, function_decl fd,
			     statement stmts);

void append_block(compound_stmt block, statement s);

void insert_block(compound_stmt block, statement before, statement s);

void append_stmt(region r, statement sameregion *after, statement s);

void prepend_stmt(region r, statement sameregion *before, statement s);

void append_expr(region r, expression sameregion *after, expression e);

void prepend_expr(region r, expression sameregion *before, expression e);

void insert_comma(comma ce, expression before, expression e);

data_declaration stabilise(region r, compound_stmt block, expression e,
			   expression *stabiliser);
data_declaration stabilise1(region r, compound_stmt block, expression e,
			    expression *temp, expression *stabiliser);
data_declaration stabilise2(region r, compound_stmt block, expression e,
			    expression *temp1, expression *temp2,
			    expression *stabiliser);

data_declaration stabilise_lvalue(region r, compound_stmt block,
				  expression lvalue,
				  expression *use, expression *stabiliser);

expression copy_lvalue(region r, location loc, expression lvalue);

/* Add expression EXPR to the ast such that it will be evaluated only when
   edge E is followed */
void add_expression_to_edge(region r, edge e, expression expr);

expression build_dereference(region r, location loc, expression ptr);

expression build_pointer_plus(region r, location loc,
			      expression arg1, expression arg2);

statement build_for_stmt(region r, location loc, expression e1,
			 expression e2, expression e3, statement body);

label_declaration declare_new_label(region r);
label build_id_label(region r, location loc, label_declaration name);
statement build_labeled_stmt(region r, location loc, label l, statement s);
statement build_goto(region r, location loc, id_label l);

#endif
