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

#ifndef CFG_H
#define CFG_H

struct edge
{
  enum { edge_fn_entry,		/* From function entry to first "real" node
				   info is function_decl */
	 edge_fn_exit,		/* Fall through function exit
				   info is function_decl */

	 edge_compound,		/* Between two statements in a block, or
				   between last declaration and first statement
				   info is second statement. */
	 edge_compound_end,	/* From last statement in a block 
				   no info */

	 edge_if_true,		/* From condition to true consequence
				   info is if statement */
	 edge_if_true_done,	/* From true consequence to end of if
				   info is if statement */
	 edge_if_false,		/* From condition to false consequence
				   info is if statement */
	 edge_if_false_done,	/* From false consequence to end of if
				   info is if statement */
	 edge_if_no_else,	/* From condition to end of if (for if's with
				   no else). info is if statement */

	 edge_label,		/* From label to labeled statement
				   info is the labeled_stmt node */

	 edge_while_true,	/* From end of condition to loop body
				   info is while statement */
	 edge_while_repeat,	/* From end of loop body to condition
				   info is while statement */
	 edge_while_done,	/* From end of condition to loop exit
				   info is while statement */

	 edge_dowhile_dummy,	/* From dummy node at start of while loop to
				   loop body. info is dowhile statement */
	 edge_dowhile_test,	/* From end of loop body to condition
				   info is dowhile statement */
	 edge_dowhile_true,	/* From end of condition to dummy node
				   info is dowhile statement */
	 edge_dowhile_done,	/* From end of condition to loop exit
				   info is dowhile statement */

	 edge_switch,		/* From end of expression to switch label
				   info is switch statement */
	 edge_switch_fallout,	/* From end of switch statement to switch exit
				   info is switch statement */
	 edge_switch_nodefault,	/* From end of expression to switch exit (no
				   default: label). info is switch statement */

	 edge_for_enterloop,	/* From end of first expression to loop entry
				   info is for statement */
	 edge_for_exitloop,	/* From end of condition to loop exit
				   info is for statement */
	 edge_for_true,		/* From end of condition to loop body
				   info is for statement */
	 edge_for_stmtdone,	/* From end of loop body to  dummy continue
				   node. info is for statement */
	 edge_for_continue,	/* Out of dummy continue node
				   info is for statement */
	 edge_for_repeat,	/* From end of continue expression to loop body
				   info is for statement */

	 edge_break,		/* From break statement to its destination
				   no info */

	 edge_continue,		/* From continue statement to its destination
				   no info */

	 edge_goto,		/* From goto statement to its destination
				   no info */

	 edge_computed_goto,	/* From computed_goto statement to a possible
				   destination. info is goto statement */

	 edge_return_expr,	/* From end of return expression to function
				   exit. info is return statement */
	 edge_return,		/* From return; statement to function exit
				   info is return statement */

	 edge_comma,		/* Between two expressions in a comma expression
				   info is second expression */

	 edge_init_list_comma,	/* Between two expressions in an init list
				   info is second expression */

	 edge_cast_list,	/* From the last cast list expression to the
				   actual cast. no info */

	 edge_conditional_true,	/* From condition to true consequence
				   info is ?: expression */
	 edge_conditional_true_done, /* From true consequence to end of ?:
				   info is ?: expression */
	 edge_conditional_false, /* From condition to false consequence
				   info is ?: expression */
	 edge_conditional_false_done, /* From false consequence to end of ?:
				   info is ?: expression */

	 edge_short_second,	/* From end of first expression of ||, && to
				   second expression. info is ||, && expr. */
	 edge_short_circuit,	/* From end of first expression of ||, && to
				   ||, && expr. info is ||, && expr. */
	 edge_short_done,	/* From end of second expression of ||, && to
				   ||, && expr. info is ||, && expr. */

	 edge_function_call,	/* From end of function expression to function
				   call. no info */

	 edge_unary,		/* From end of expression to unary op
				   no info */
	 edge_unary_lvalue,	/* From end of expression to unary op that
				   requires an lvalue. no info */

	 edge_binary,		/* From end of first expression to binary op
				   no info */
	 edge_binary_lvalue,	/* From end of first expression to binary op
				   that requires an lvalue arg1. no info */

	 edge_assign_local	/* From expression to assignment to localvar
				   no info */
  } kind;
  node parentptr info;		/* The high-level node to which this
				   edge best corresponds (selected to allow
				   insertion of rcops). See edge_xxx for
				   details */
  bool nice; /* TRUE for nice edges */
  node parentptr from, to;
  edge sameregion next_out, next_in;

  dd_list incvars;		/* variables whose RC should be incremented */
  dd_list decvars;		/* variables whose RC should be decremented */
  int ecount;			/* Execution count */

  /* Maxflow data */
  int mf_flow;			/* Current flow over this edge (may be negative
				   for backwards flow) */

  gnode sr_vertex;
};

void build_cfg(region r, function_decl fd);
long cfg_size(function_decl fd);
void cfg_build_postorder(region r, function_decl fd);

bool vertex_marked(node n);
void mark_vertex(node n);
void clear_all_marks(void);

#endif
