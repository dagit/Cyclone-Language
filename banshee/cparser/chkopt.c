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
#include <math.h>
#include "parser.h"
#include "cfg.h"
#include "unparse.h"
#include "chkopt.h"
#include "graph.h"
#include "edit.h"
#include "constants.h"
#include "temps.h"
#include "c-parse.h"
#include "semantics.h"
#include "AST_utils.h"

struct sr_node
{
  enum { sr_nop, sr_fcall, sr_kill, sr_null, sr_notnull, sr_sameregion,
	 sr_assign, sr_assign_sameregion, sr_assign_null,
	 sr_lessthan, sr_assign_lessthan } kind;
  /* The sr_assign nodes are shortcuts and include an implicit sr_kill of
     p1 */
  place p1, p2; /* p1 and p2 are never PNULL or PUNKNOWN */
  gplace *sameregion fcall_args;
  function_decl fcall_fn;
  struct sr_state in, out; /* Relation before and after this constraint */
};

struct sr_state zero_state;

static double chkopt_time;

static bool interesting_decl(data_declaration d)
{
  /* XXX: Can't rely on interesting field when inspecting other functions (may not
     be defined yet) */
  return d->kind == decl_variable && d->islocal && !d->addressed && type_pointer(d->type);
}

static gnode end_of_chain(gnode n)
{
  gedge out;

  while ((out = graph_first_edge_out(n)))
    n = graph_edge_to(out);
  
  return n;
}

#define GPSR 1
#define GPLT 2

/* Detect, set, or clear the "sameregion" property of generalised places */
bool gp_sr(gplace p)
{
  return p >= 0 && (p & GPSR);
}

/* Detect, set, or clear the "<=" property of generalised places */
bool gp_lt(gplace p)
{
  return p >= 0 && (p & GPLT);
}

gplace gp_setsr(gplace p)
{
  if (p < 0)
    return p;
  assert(!gp_lt(p));
  return p | GPSR;
}

gplace gp_clearsr(gplace p)
{
  if (p < 0) /* This can even occur with PNULL due to offsetof... */
    return p;
  return p & ~GPSR;
}

gplace gp_setlt(gplace p)
{
  if (p < 0)
    return p;
  assert(!gp_sr(p));
  return p | GPLT;
}

gplace gp_clearlt(gplace p)
{
  if (p < 0) /* This can even occur with PNULL due to offsetof... */
    return p;
  return p & ~GPLT;
}

place gp2place(gplace p)
{
  /* Catch use of unitialised places when chkopt enabled */
  assert(!rc_chkopt || p != PERROR);
  if (p < 0)
    return p; /* Ok, ok, this isn't a real place, but it makes add_constraint simpler */
  return p >> 2;
}

gplace place2gp(place p)
{
  return p << 2;
}

gplace gplace_union(expression e, gplace p1, gplace p2)
{
  /* Properties are contagious */
  if (gp2place(p1) == gp2place(p2))
    {
      if (gp_lt(p1) || gp_lt(p2))
	return gp_setlt(gp_clearsr(p1));
      if (gp_sr(p1) || gp_sr(p2))
	return gp_setsr(p1);

      return p1;
    }

  if (p1 == PNULL)
    return gp_lt(p2) ? p2 : gp_setsr(p2);
  
  if (p2 == PNULL)
    return gp_lt(p1) ? p1 : gp_setsr(p1);

  if (p1 != PUNKNOWN && p2 != PUNKNOWN/* && type_pointer(e->type)*/)
    warning_with_location(e->location, "?: chkopt imprecision");

  return PUNKNOWN;
}

/* Requires: lregion of e be known.
   Returns: rregion of e assuming e is the result of some read operation (so may
     return NULL)
*/
gplace gplaceof_read(expression e)
{
  type t = e->type;

  if (type_array(t))
    return gp_clearsr(e->lregion); /* Can't be NULL */
  if (type_sameregion(t))
    return gp_lt(e->lregion) ? e->lregion : gp_setsr(e->lregion);
  if (type_parentptr(t)) /* It is NOT SR */
    return gp_setlt(gp_clearsr(e->lregion));
  if (type_traditional(t))
    return gp_setsr(PGLOBAL);
  return PUNKNOWN;
}

struct sr_node *new_sr_node(region r, int kind, place p1, place p2,
			    place *fcall_args, function_decl fcall_fn)
{
  struct sr_node *new = ralloc(r, struct sr_node);

  assert(p1 >= 0 && p2 >= 0);
  new->kind = kind;
  new->p1 = p1;
  new->p2 = p2;
  new->fcall_args = fcall_args;
  new->fcall_fn = fcall_fn;

  return new;
}

void really_add_constraint(region r, function_decl fd, node to, int kind, place p1, place p2, 
			    place *fcall_args, function_decl fcall_fn)
{
  struct sr_node *new = new_sr_node(r, kind, p1, p2, fcall_args, fcall_fn);
  gnode newnode = graph_add_node(fd->sr_graph, new);

  if (to->sr_vertex)
    graph_add_edge(newnode, to->sr_vertex, NULL);
  to->sr_vertex = newnode;
}

/* assert gp1 SRas gp2 */
void add_sameregion_constraint(region r, function_decl fd, node to, gplace gp1, gplace gp2)
{
  place p1 = gp2place(gp1), p2 = gp2place(gp2);
  assert(p1 < fd->sr_nvars && p2 < fd->sr_nvars);
  
  /* Tricky, as must handle all cases correctly :-( */
  if (p1 == PNULL || p1 == PUNKNOWN || gp_lt(gp1) || gp_sr(gp1) || p2 == PUNKNOWN || p1 == p2)
    return;

  if (p2 == PNULL)
    really_add_constraint(r, fd, to, sr_null, p1, 0, NULL, NULL);
  else if (gp_lt(gp2))
    really_add_constraint(r, fd, to, sr_lessthan, p1, p2, NULL, NULL);
  else
    really_add_constraint(r, fd, to, sr_sameregion, p1, p2, NULL, NULL);
}

/* assert gp1 <= gp2 */
void add_parent_constraint(region r, function_decl fd, node to, gplace gp1, gplace gp2)
{
  add_sameregion_constraint(r, fd, to, gp1, gp_setlt(gp_clearsr(gp2)));
}

/* This is for assigning to locals, gp1 = gp2 */
void add_assign_constraint(region r, function_decl fd, node to, gplace gp1, gplace gp2)
{
  place p1 = gp2place(gp1), p2 = gp2place(gp2);

  if (gp1 == gp2)
    return;

  assert(p1 >= 0 && p1 < fd->sr_nvars && p2 < fd->sr_nvars);
  
  assert(!gp_lt(gp1) && !gp_sr(gp1) && gp1 > PGLOBAL);

  if (p2 == PUNKNOWN) /* We've assigned to p1, but don't know anything about new value */
    really_add_constraint(r, fd, to, sr_kill, p1, 0, NULL, NULL);
  else if (p2 == PNULL)
    really_add_constraint(r, fd, to, sr_assign_null, p1, 0, NULL, NULL);
  else if (gp_sr(gp2))
    really_add_constraint(r, fd, to, sr_assign_sameregion, p1, p2, NULL, NULL);
  else if (gp_lt(gp2))
    really_add_constraint(r, fd, to, sr_assign_lessthan, p1, p2, NULL, NULL);
  else /* Regular assignment */
    really_add_constraint(r, fd, to, sr_assign, p1, p2, NULL, NULL);
}

void add_notnull_constraint(region r, function_decl fd, node to, gplace gp)
{
  if (!gp_sr(gp) && gp > PGLOBAL)
    really_add_constraint(r, fd, to, sr_notnull, gp2place(gp), 0, NULL, NULL);
}

void make_sr_vertex_assign(region r, function_decl fd, binary assign)
{
  expression dest = assign->arg1, source = assign->arg2;
  node n = CAST(node, assign);

  if (type_pointer(assign->type))
    {
      if (type_sameregion(dest->type))
	add_sameregion_constraint(r, fd, n, source->rregion, dest->lregion);
      if (type_parentptr(dest->type))
	add_parent_constraint(r, fd, n, source->rregion, dest->lregion);
      if (type_traditional(dest->type))
	add_sameregion_constraint(r, fd, n, source->rregion, PGLOBAL);
      add_notnull_constraint(r, fd, n, dest->lregion);

      if (is_localvar(dest) && CAST(identifier, dest)->ddecl->interesting)
	{
	  /* Temps should not cause any constraints */
	  assert(!CAST(identifier, dest)->ddecl->istemp ||
		 dest->rregion == source->rregion);
	  add_assign_constraint(r, fd, n, dest->rregion, source->rregion);
	}
    }
  else if (type_contains_qualified_pointers(assign->type))
    warning_with_location(assign->location, "assignment of struct with sameregion, parentptr or traditional pointers");
}

/* A simple check on whether a function call is interesting */
static bool has_interesting_ptrs(function_call fce, function_decl called,
				 int *nptrargs)
{
  declaration parms;
  expression args;
  bool interesting = FALSE;

  *nptrargs = 0;

  if (fce->rregion != PUNKNOWN)
    interesting = TRUE;

  args = fce->args;
  scan_declaration (parms, called->fdeclarator->parms)
    {
      data_declaration parmdecl = get_parameter(parms);

      if (parmdecl && interesting_decl(parmdecl))
	{
	  (*nptrargs)++;
	  if (args && args->rregion != PUNKNOWN)
	    interesting = TRUE;
	}
      if (args)
	args = CAST(expression, args->next);
    }

  return interesting;
}

void make_sr_vertex_return(region r, function_decl fd)
{
  edge ret;

  if (rc_chkopt_perfile && type_pointer(type_function_return_type(fd->ddecl->type)))
    /* Add return statement edges (return statements are found on the
       edges into fd */
    for (ret = fd->edges_in; ret; ret = ret->next_in)
      switch (ret->kind)
	{
	case edge_fn_exit:
	  /* Fall through - while this logically means we don't 
	     know the result it is also undefined behaviour. And
	     we don't do a good job of detecting when this cannot
	     happen (e.g., functions ending with abort()), so we
	     assume these edges are not executable */
	  break;
	case edge_return: case edge_return_expr: {
	  return_stmt rs = CAST(return_stmt, ret->info);

	  add_assign_constraint(r, fd, CAST(node, fd), place2gp(fd->sr_return_id),
				rs->arg1 ? rs->arg1->rregion : PUNKNOWN);
	  /* Transfer to edge */
	  ret->sr_vertex = fd->sr_vertex;
	  fd->sr_vertex = NULL;
	  break;
	}
	default:
	  assert(0);
	}
}

void make_sr_vertex_function_call(region r, function_decl fd, function_call fce)
{
  expression calling = fce->arg1;

  /* We do nothing for:
     - calls via function pointers
     - calls to unknown functions (i.e., no body, and not library functions like
       ralloc whose behaviour is known)
     - calls for which all pointer arguments are PUNKNOWN (note that this covers
       functions with no pointer arguments ;-)) and without a pointer result
       which is used
     If rc_chkopt_perfile is not defined, then only library calls will cause
     fcall constraints to be generated.
  */
  if (is_identifier(calling))
    {
      data_declaration called = base_identifier(CAST(identifier, calling)->ddecl);

      if (called->kind == decl_function && called->definition && rc_chkopt_perfile)
	{
	  bool some_known_args = FALSE;
	  function_decl calleddef = CAST(function_decl, called->definition);
	  gplace *argplaces;
	  int nptrargs;

	  if (has_interesting_ptrs(fce, calleddef, &nptrargs))
	    {
	      /* Extra slot for the function result */
	      argplaces = rarrayalloc(r, nptrargs + 1, gplace);
	      argplaces[nptrargs] = fce->rregion;

	      if (nptrargs > 0)
		{
		  declaration parms;
		  expression args;
		  int nptrarg;

		  nptrarg = 0;
		  parms = calleddef->fdeclarator->parms;
		  scan_expression (args, fce->args)
		    {
		      data_declaration parmdecl;

		      if (!parms) /* Extra args, ignore them */
			break;

		      parmdecl = get_parameter(parms);
		      if (parmdecl && interesting_decl(parmdecl))
			{
			  argplaces[nptrarg++] = args->rregion;

			  if (args->rregion != PUNKNOWN)
			    some_known_args = TRUE;
			}
		      parms = CAST(declaration, parms->next);
		    }

		  /* Handle any missing args */
		  while (nptrarg < nptrargs)
		    argplaces[nptrarg++] = PUNKNOWN;
		  assert(nptrarg == nptrargs);
		}
	      really_add_constraint(r, fd, CAST(node, fce),
				    sr_fcall, 0, 0, argplaces, calleddef);
	    }

	  if (!some_known_args)
	    /* This call-site knows nothing about the arguments to called, so there's
	       no point in trying to find out characteristics of called's arguments.
	       We have to keep this information because we may not add a constraint for
	       this function call */
	    calleddef->sr_unknown_arguments = TRUE;
	}
    }
}

void build_sameregion_nodes(region r, function_decl fd)
{
  int i, nnodes = fd->postorder_size;
  node parentptr *gnodes = fd->postorder_nodes;

  /* Create entry and exit nodes for sameregion cfg */
  fd->sr_entry = fd->cfg_entry->sr_vertex =
    graph_add_node(fd->sr_graph, new_sr_node(r, sr_nop, 0, 0, NULL, NULL));

  if (rc_chkopt_perfile)
    fd->sr_return_id = fd->sr_nvars++;

  /* Build cfg nodes */
  for (i = 0; i < nnodes; i++)
    {
      node n = gnodes[i];

      if (is_assign(n))
	make_sr_vertex_assign(r, fd, CAST(binary, n));
      else if (is_function_call(n))
	make_sr_vertex_function_call(r, fd, CAST(function_call, n));
    }
  make_sr_vertex_return(r, fd);

  fd->sr_vertex = graph_add_node(fd->sr_graph, new_sr_node(r, sr_nop, 0, 0, NULL, NULL));
}

static void add_reachable_sr_nodes(bool first, gnode source, node n)
{
  edge out;

  if (vertex_marked(n))
    return;

  mark_vertex(n);

  if (!first && n->sr_vertex)
    graph_add_edge(source, n->sr_vertex, NULL);
  else
    for (out = n->edges_out; out; out = out->next_out)
      {
	if (out->sr_vertex)
	  graph_add_edge(source, out->sr_vertex, NULL);
	else
	  add_reachable_sr_nodes(FALSE, source, out->to);
      }
}

void build_sameregion_cfg(region r, function_decl fd)
{
  int i, nnodes = fd->postorder_size;
  node parentptr *gnodes = fd->postorder_nodes;
  gnode srn;
  edge out;

  fd->sr_graph = new_graph(r);

  build_sameregion_nodes(r, fd);


  /* decide which merge nodes to keep (to reduce number of cfg merges) */
  /* add_sameregion_cfg_merges(r, fd); */

  /* Build cfg */
  for (i = 0; i < nnodes; i++)
    {
      node n = gnodes[i];

      if ((srn = n->sr_vertex))
	{
	  clear_all_marks(); /* on the original cfg */
	  add_reachable_sr_nodes(TRUE, end_of_chain(srn), n);
	}
      for (out = n->edges_out; out; out = out->next_out)
	{
	  if ((srn = out->sr_vertex))
	    {
	      clear_all_marks(); /* on the original cfg */
	      add_reachable_sr_nodes(FALSE, end_of_chain(srn), out->to);
	    }
	}
    }      
}

void count_use(function_decl fd, gplace gp)
{
  int id = gp2place(gp) - 1;

  if (id >= 0 && id < fd->nlocals)
    fd->vmap[gp2place(gp) - 1]->sr_uses++;
}

void count_uses_assign(function_decl fd, binary assign)
{
  count_use(fd, assign->arg1->lregion);
  count_use(fd, assign->arg2->rregion);
}

void count_uses_return(function_decl fd)
{
  edge ret;

  for (ret = fd->edges_in; ret; ret = ret->next_in)
    switch (ret->kind)
      {
      case edge_fn_exit:
	break;
      case edge_return: case edge_return_expr: {
	return_stmt rs = CAST(return_stmt, ret->info);

	if (rs->arg1)
	  count_use(fd, rs->arg1->rregion);
	break;
      }
      default:
	assert(0);
      }
}

void count_uses_function_call(function_decl fd, function_call fce)
{
  expression args;

  scan_expression (args, fce->args)
    count_use(fd, args->rregion);
}

void keep_interesting_temps(function_decl fd)
{
  int i, nnodes = fd->postorder_size;
  node parentptr *gnodes = fd->postorder_nodes;

  for (i = 0; i < nnodes; i++)
    {
      node n = gnodes[i];

      if (is_assign(n))
	count_uses_assign(fd, CAST(binary, n));
      else if (is_function_call(n))
	count_uses_function_call(fd, CAST(function_call, n));
    }
  count_uses_return(fd);

  for (i = 0; i < nnodes; i++)
    {
      node n = gnodes[i];

      if (is_assign(n))
	{
	  assign a = CAST(assign, n);
	  expression dest = a->arg1, source = a->arg2;

	  if (type_pointer(a->type) && is_localvar(dest))
	    {
	      data_declaration decl = CAST(identifier, dest)->ddecl;

	      if (decl->istemp && decl->interesting)
		{
		  /* Must keep if defined from PUNKNOWN, gp_sr/lt(something)
		     and sr_uses != 1 */
		  if ((source->rregion == PUNKNOWN || gp_sr(source->rregion) ||
		       gp_lt(source->rregion)) &&
		      decl->sr_uses > 1)
		    decl->istemp = FALSE;
		}
	    }
	}
    }
}

/* XXX: don't really need to save lregion/rregion in every node (only needed in
   assignments) */
void collect_regions_stmt(function_decl fd, statement stmt);
void collect_regions_expr(function_decl fd, expression expr);

static void collect_regions_elist(function_decl fd, expression elist)
{
  expression e;

  scan_expression (e, elist)
    collect_regions_expr(fd, e);
}

void collect_regions_expr(function_decl fd, expression expr)
{
  /* Convenient defaults */
  expr->lregion = PUNKNOWN;
  expr->rregion = PUNKNOWN;

  switch (expr->kind)
    {
    case kind_identifier: {
      data_declaration decl = CAST(identifier, expr)->ddecl;

      /* This should be done in fix.c, but is easier to change here:
	 detect functions used as values (i.e., not called directly) */
      if (decl->kind == decl_function && !is_function_call(expr->parent))
	decl->addressed = TRUE;

      expr->lregion = PGLOBAL;
      switch (decl->kind)
	{
	case decl_constant:
	  /*expr->rregion = PUNKNOWN;*/
	  break;
	case decl_function:
	  expr->rregion = PGLOBAL;
	  break;
	case decl_variable:
	  if (decl->islocal && decl->interesting)
	    expr->rregion = place2gp(decl->id + 1); /* off by 1 for PGLOBAL */
#if 1
	  /* The system doesn't properly support locals which are sameregion/traditional */
	  else if (decl->islocal)
	    /*expr->rregion = PUNKNOWN*/;
#endif
	  else
	    expr->rregion = gplaceof_read(expr);
	  break;
	default: assert(0);
	}
      break;
    }
    case kind_field_ref: {
      field_ref fr = CAST(field_ref, expr);

      collect_regions_expr(fd, fr->arg1);
      expr->lregion = fr->arg1->lregion;
      expr->rregion = gplaceof_read(expr);
      break;
    }
    case kind_dereference: {
      dereference dr = CAST(dereference, expr);

      collect_regions_expr(fd, dr->arg1);
      expr->lregion = gp_clearsr(dr->arg1->rregion); /* Successful deref implies that not null */
      expr->rregion = gplaceof_read(expr);
      break;
    }
    case kind_address_of: {
      address_of ao = CAST(address_of, expr);

      collect_regions_expr(fd, ao->arg1);
      /*expr->lregion = PUNKNOWN;*/
      expr->rregion = ao->arg1->lregion;
      break;
    }
    case kind_comma: {
      expression e;
      scan_expression (e, CAST(comma, expr)->arg1)
	{
	  collect_regions_expr(fd, e);
	  if (!e->next)
	    {
	      expr->lregion = e->lregion;
	      expr->rregion = e->rregion;
	    }
	}
      break;
    }
    case kind_cast_list: {
      collect_regions_expr(fd, CAST(cast_list, expr)->init_expr);
      break;
    }
    case kind_init_index: {
      collect_regions_expr(fd, CAST(init_index, expr)->init_expr);
      break;
    }
    case kind_init_field: {
      collect_regions_expr(fd, CAST(init_field, expr)->init_expr);
      break;
    }
    case kind_init_list: {
      collect_regions_elist(fd, CAST(init_list, expr)->args);
      break;
    }
    case kind_conditional: {
      conditional ce = CAST(conditional, expr);
      collect_regions_expr(fd, ce->condition);
      collect_regions_expr(fd, ce->arg1);
      collect_regions_expr(fd, ce->arg2);

      expr->lregion = gplace_union(expr, ce->arg1->lregion, ce->arg2->lregion);
      expr->rregion = gplace_union(expr, ce->arg1->rregion, ce->arg2->rregion);
      break;
    }
    case kind_compound_expr: {
      compound_expr ce = CAST(compound_expr, expr);
      expression value = expression_of_stmt(ce);

      collect_regions_stmt(fd, ce->stmt);
      expr->lregion = value->lregion;
      expr->rregion = value->rregion;
      break;
    }
    case kind_function_call: {
      function_call fce = CAST(function_call, expr);
      expression arg;

      scan_expression (arg, fce->args)
	collect_regions_expr(fd, arg);
      collect_regions_expr(fd, fce->arg1);
      /*expr->lregion = PUNKNOWN;*/
      expr->rregion = expression_used(expr) && type_pointer(expr->type) ?
			place2gp(fd->sr_nvars++) : PUNKNOWN;
      break;
    }
    case kind_cast: {
      cast ce = CAST(cast, expr);

      collect_regions_expr(fd, ce->arg1);
      expr->lregion = ce->arg1->lregion;
      /* Check we haven't excessively narrowed (at which point rregion goes
         to unknown) */
      if (type_size_cc(ce->type) &&
	  type_size(ce->type) >= type_size(ptr_void_type))
	expr->rregion = ce->arg1->rregion;
      else
	expr->rregion = PUNKNOWN;
      break;
    }
    case kind_extension_expr:
    case kind_predecrement: case kind_preincrement:
    case kind_postdecrement: case kind_postincrement: {
      unary ue = CAST(unary, expr);

      collect_regions_expr(fd, ue->arg1);
      expr->lregion = ue->arg1->lregion;
      expr->rregion = ue->arg1->rregion;
      break;
    }
    case kind_sizeof_expr: case kind_alignof_expr:
      /*expr->lregion = expr->rregion = PUNKNOWN;*/
      break;

    case kind_plus: case kind_plus_assign:
    case kind_minus: case kind_minus_assign: {
      binary be = CAST(binary, expr);

      collect_regions_expr(fd, be->arg1);
      collect_regions_expr(fd, be->arg2);
      /*expr->lregion = PUNKNOWN;*/
      expr->rregion = 
	(type_array(be->arg1->type) || type_pointer(be->arg1->type)) ? be->arg1->rregion :
	(type_array(be->arg1->type) || type_pointer(be->arg2->type)) ? be->arg2->rregion :
	PUNKNOWN;
      break;
    }
    case kind_assign: {
      binary be = CAST(binary, expr);

      collect_regions_expr(fd, be->arg1);
      collect_regions_expr(fd, be->arg2);
      expr->lregion = be->arg2->lregion;
      expr->rregion = be->arg2->rregion;
      break;
    }
    default:
      if (is_unary(expr))
	collect_regions_expr(fd, CAST(unary, expr)->arg1);
      else if (is_binary(expr))
	{
	  binary be = CAST(binary, expr);
	  collect_regions_expr(fd, be->arg1);
	  collect_regions_expr(fd, be->arg2);
	}
      else
	{
	  if (is_string(expr))
	    expr->rregion = PGLOBAL;
	  else
	    {
	      if (!expr->cst)
		abort();
	      else if (definite_zero(expr))
		expr->rregion = PNULL;
	    }
	}
      break;
    }
}

void collect_regions_stmt(function_decl fd, statement stmt)
{
  switch (stmt->kind)
    {
    case kind_asm_stmt: {
      asm_stmt as = CAST(asm_stmt, stmt);
      asm_operand ao;

      scan_asm_operand (ao, as->asm_operands1) /* Outputs */
	collect_regions_expr(fd, ao->arg1);

      scan_asm_operand (ao, as->asm_operands2) /* Inputs */
	collect_regions_expr(fd, ao->arg1);
      break;
    }
    case kind_compound_stmt: {
      statement s;
      scan_statement (s, CAST(compound_stmt, stmt)->stmts)
	collect_regions_stmt(fd, s);
      break;
    }
    case kind_if_stmt: {
      if_stmt is = CAST(if_stmt, stmt);

      collect_regions_expr(fd, is->condition);
      collect_regions_stmt(fd, is->stmt1);
      if (is->stmt2)
	collect_regions_stmt(fd, is->stmt2);
      break;
    }
    case kind_labeled_stmt: {
      labeled_stmt ls = CAST(labeled_stmt, stmt);

      collect_regions_stmt(fd, ls->stmt);
      break;
    }
    case kind_expression_stmt: {
      collect_regions_expr(fd, CAST(expression_stmt, stmt)->arg1);
      break;
    }
    case kind_while_stmt: {
      conditional_stmt cs = CAST(conditional_stmt, stmt);

      collect_regions_expr(fd, cs->condition);
      collect_regions_stmt(fd, cs->stmt);
      break;
    }
    case kind_dowhile_stmt: {
      conditional_stmt cs = CAST(conditional_stmt, stmt);

      collect_regions_stmt(fd, cs->stmt);
      collect_regions_expr(fd, cs->condition);
      break;
    }
    case kind_switch_stmt: {
      switch_stmt sws = CAST(switch_stmt, stmt);

      collect_regions_expr(fd, sws->condition);
      collect_regions_stmt(fd, sws->stmt);
      break;
    }
    case kind_for_stmt: {
      for_stmt fs = CAST(for_stmt, stmt);

      collect_regions_stmt(fd, fs->stmt);
      if (fs->arg1)
	collect_regions_expr(fd, fs->arg1);
      if (fs->arg2)
	collect_regions_expr(fd, fs->arg2);
      if (fs->arg3)
	collect_regions_expr(fd, fs->arg3);
      break;
    }
    case kind_break_stmt:
    case kind_continue_stmt:
    case kind_goto_stmt: 
    case kind_empty_stmt:
      break;

    case kind_computed_goto_stmt:
      collect_regions_expr(fd, CAST(computed_goto_stmt, stmt)->arg1);
      break;

    case kind_return_stmt: {
      return_stmt rs = CAST(return_stmt, stmt);

      if (rs->arg1)
	collect_regions_expr(fd, rs->arg1);
      break;
    }
    default: assert(0);
    }
}

void collect_expression_regions(function_decl fd)
{
  fd->sr_nvars = fd->nlocals + 1;
  collect_regions_stmt(fd, fd->stmt);

  /* Using shorts for places */
  assert(fd->sr_nvars <= SHRT_MAX); /* Should be SHRT_MAX + 1, but that could overflow... */
}

void find_addressed_functions(expression init)
{
  expression e;

  /* Hack of the day, reuse collect_regions_expr which also finds addressed
     fns to help find addressed functions in initialisers */

  switch (init->kind)
    {
    case kind_init_list:
      scan_expression (e, CAST(init_list, init)->args)
	find_addressed_functions(e);
      break;
    case kind_init_index:
      find_addressed_functions(CAST(init_index, init)->init_expr);
      break;
    case kind_init_field:
      find_addressed_functions(CAST(init_field, init)->init_expr);
      break;
    default:
      collect_regions_expr(NULL, init);
      break;
    }
}

/* Place ids are sparse (because of the large number of temps and non-pointer
   vars) - make the space nice and small */

static void rewrite_expr(expression expr, int *placeidmap)
{
  expr->lregion = placeidmap[expr->lregion];
  expr->rregion = placeidmap[expr->rregion];
}

static void rewrite_places(node n, gplace *placeidmap)
{
  edge out;
  gplace restoreid = -1;
  gplace restoreval1 = PUNKNOWN, restoreval2 = PUNKNOWN, restoreval3 = PUNKNOWN;

  if (vertex_marked(n))
    return;
  mark_vertex(n);

  if (is_expression(n))
    {
      /* We only rewrite the places of "interesting" nodes (source & dest
	 of assignments). We don't do the obvious thing of rewriting all
	 exprs because not all expression nodes are in the weird cfg :-( */

      if (is_assign(n) && type_pointer(n->type))
	{
	  binary a = CAST(binary, n);
	  expression dest = a->arg1, source = a->arg2;

	  /* We rewrite the places of source & dest. */
	  if (!is_function_call(source)) /* XXX: Yuck */
	    rewrite_expr(source, placeidmap);
	  assert(!is_function_call(dest));
	  rewrite_expr(dest, placeidmap); /* dest can't be fn call */

	  if (is_localvar(dest))
	    {
	      data_declaration decl = CAST(identifier, dest)->ddecl;

	      if (decl->istemp)
		{
		  gplace rr = source->rregion;

		  assert(decl->interesting);

		  restoreid = place2gp(decl->id + 1);
		  restoreval1 = placeidmap[restoreid];
		  restoreval2 = placeidmap[gp_setsr(restoreid)];
		  restoreval3 = placeidmap[gp_setlt(restoreid)];
		  /* We rewrite the region of the node holding this temp
		     too (so that make_sr_vertex_assign will not make a
		     constraint for this node) */
		  placeidmap[restoreid] = dest->rregion = rr;
		  placeidmap[gp_setsr(restoreid)] = gp_lt(rr) ? rr : gp_setsr(rr);
		  placeidmap[gp_setlt(restoreid)] = gp_setlt(gp_clearsr(rr));

		}
	    }
	}
      else if (is_function_call(n))
	{
	  /* Rewrite arguments */
	  expression args;
	  function_call fce = CAST(function_call, n);

	  scan_expression (args, fce->args)
	    rewrite_expr(args, placeidmap);
	  rewrite_expr(CAST(expression, fce), placeidmap);
	}
    }

  for (out = n->edges_out; out; out = out->next_out)
    {
      /* Rewrite return statement arguments */
      if (out->kind == edge_return_expr || out->kind == edge_return)
	{
	  return_stmt rs = CAST(return_stmt, out->info);

	  if (rs->arg1 && !is_function_call(rs->arg1)) /* XXX:Yuck, fn call test */
	    rewrite_expr(rs->arg1, placeidmap);
	}
      rewrite_places(out->to, placeidmap);
    }

  if (restoreid >= 0)
    {
      placeidmap[restoreid] = restoreval1;
      placeidmap[gp_setsr(restoreid)] = restoreval2;
      placeidmap[gp_setlt(restoreid)] = restoreval3;
    }
}

void eliminate_temp_place_ids(region r, function_decl fd) deletes
{
  int *placeidmap = rarrayalloc(r, 4 * fd->sr_nvars + 2, int), i;
  place *localidmap = rarrayalloc(r, fd->nlocals, int);
  int nextid = 1;

  compute_temps(fd);
  keep_interesting_temps(fd);

  placeidmap += 2;
  placeidmap[PNULL] = PNULL;
  placeidmap[PUNKNOWN] = PUNKNOWN;

  /* The off-by-one stuff is due to reserving id 0 for PGLOBAL */
  for (i = 0; i < fd->nlocals; i++)
    {
      data_declaration decl = fd->vmap[i];

      if (decl->interesting && !decl->istemp)
	{
	  gplace gi = place2gp(i + 1);
	  gplace gnextid = place2gp(nextid);

	  localidmap[i] = nextid;
	  nextid++;
	  placeidmap[gi] = gnextid;
	  placeidmap[gp_setsr(gi)] = gp_setsr(gnextid);
	  placeidmap[gp_setlt(gi)] = gp_setlt(gnextid);
	}
    }
#ifdef DPRINT
  fprintf(stderr, "%s: %d original locals, %d real locals\n",
	  fd->ddecl->name, fd->nlocals, nextid - 1);
#endif

  /* Now bring function ids to reduced range */
  for (i++; i < fd->sr_nvars; i++)
    {
      gplace gi = place2gp(i);
      gplace gnextid = place2gp(nextid++);

      placeidmap[gi] = gnextid;
      placeidmap[gp_setsr(gi)] = gp_setsr(gnextid);
      placeidmap[gp_setlt(gi)] = gp_setlt(gnextid);
    }

#ifndef NDEBUG
  /* This is only for debugging purposes */
  fd->sr_vmap = rarrayalloc(r, nextid, data_declaration);
  for (i = 0; i < fd->nlocals; i++)
    if (localidmap[i])
      fd->sr_vmap[localidmap[i]] = fd->vmap[i];
#endif

  fd->sr_nvars = nextid;
  fd->sr_local2placeidmap = localidmap;

  /* By walking the graph in preorder, we are guaranteed to see all temp
     defs before their corresponding uses. We have to do the recursive
     traversal rather than a simple iteration because we have to restore
     the placeidmap after modification to take account of the fact that
     a variable identified as a temp may be defined more than once */

  clear_all_marks();
  rewrite_places(fd->cfg_entry, placeidmap);
}

/* I use false for the relation holding in the bitset because bitsets are allocated
   with all false */
#define NVRELBIT(nvars, a, b) ((a) * (nvars) + (b))
#define NVRELHOLDS(r, nvars, a, b) (!BIT_SETP((r), NVRELBIT((nvars), (a), (b))))
#define NVRELDENY(r, nvars, a, b) (SET_BITB((r), NVRELBIT((nvars), (a), (b))))
#define NVRELASSERT(r, nvars, a, b) (CLEAR_BITB((r), NVRELBIT((nvars), (a), (b))))

/* a SRas b */
#define RELHOLDS_SR(s, a, b) NVRELHOLDS((s)->sr, nvars, (a), (b))
#define RELDENY_SR(s, a, b) NVRELDENY((s)->sr, nvars, (a), (b))
#define RELASSERT_SR(s, a, b) NVRELASSERT((s)->sr, nvars, (a), (b))

/* a <= b */
#define RELHOLDS_LT(s, a, b) NVRELHOLDS((s)->lt, nvars, (a), (b))
#define RELDENY_LT(s, a, b) NVRELDENY((s)->lt, nvars, (a), (b))
#define RELASSERT_LT(s, a, b) NVRELASSERT((s)->lt, nvars, (a), (b))

#define PROPHOLDS(prop, a) BIT_SETP((prop), (a))
#define PROPASSERT(prop, a) SET_BITB((prop), (a))
#define PROPDENY(prop, a) CLEAR_BITB((prop), (a))

#define PROPASSERT_NULL(s, a) PROPASSERT((s)->null, (a))
#define PROPDENY_NULL(s, a) PROPDENY((s)->null, (a))
#define PROPASSERT_NOTNULL(s, a) PROPASSERT((s)->notnull, (a))
#define PROPDENY_NOTNULL(s, a) PROPDENY((s)->notnull, (a))

void rel_kill(sr_state s, int nvars, place p)
{
  int i;

  /* nothing is known about p */
  for (i = 0; i < nvars; i++)
    {
      if (!PROPHOLDS(s->null, i))
	{
	  RELDENY_SR(s, i, p);
	  RELDENY_LT(s, i, p);
	}
      RELDENY_SR(s, p, i);
      RELDENY_LT(s, p, i);
    }
  RELASSERT_SR(s, p, p);
  RELASSERT_LT(s, p, p);

  PROPDENY_NOTNULL(s, p);
  PROPDENY_NULL(s, p);
}

void rel_assign_null(sr_state s, int nvars, place p)
{
  int i;

  /* p is now less than everything (and nothing is less than p) */
  for (i = 0; i < nvars; i++)
    {
      if (!PROPHOLDS(s->null, i))
	{
	  RELDENY_SR(s, i, p);
	  RELDENY_LT(s, i, p);
	}
      /* Doing these second guarantees that i <=/SRas i holds */
      RELASSERT_SR(s, p, i);
      RELASSERT_LT(s, p, i);
    }
  PROPDENY_NOTNULL(s, p);
  PROPASSERT_NULL(s, p);
}

void rel_assign(sr_state s, int nvars, place dest, place src)
{
  int i;

  assert(dest != src);
  /* old knowledge about dest is removed, dest has the same properties as src */
  for (i = 0; i < nvars; i++)
    {
      if (RELHOLDS_SR(s, i, src))
	RELASSERT_SR(s, i, dest);
      else
	RELDENY_SR(s, i, dest);

      if (RELHOLDS_SR(s, src, i))
	RELASSERT_SR(s, dest, i);
      else
	RELDENY_SR(s, dest, i);

      if (RELHOLDS_LT(s, i, src))
	RELASSERT_LT(s, i, dest);
      else
	RELDENY_LT(s, i, dest);

      if (RELHOLDS_LT(s, src, i))
	RELASSERT_LT(s, dest, i);
      else
	RELDENY_LT(s, dest, i);
    }
  RELASSERT_SR(s, dest, dest);
  RELASSERT_LT(s, dest, dest);

  if (PROPHOLDS(s->notnull, src))
    PROPASSERT_NOTNULL(s, dest);
  else
    PROPDENY_NOTNULL(s, dest);

  if (PROPHOLDS(s->null, src))
    PROPASSERT_NULL(s, dest);
  else
    PROPDENY_NULL(s, dest);
}

void rel_assign_lessthan(sr_state s, int nvars, place dest, place src)
{
  int i;

  /* old knowledge about dest is removed, dest is less than src */
  for (i = 0; i < nvars; i++)
    {
      if (!PROPHOLDS(s->null, i))
	{
	  RELDENY_SR(s, i, dest);
	  RELDENY_LT(s, i, dest);
	}

      if (RELHOLDS_LT(s, src, i))
	RELASSERT_LT(s, dest, i);
      else
	RELDENY_LT(s, dest, i);

      RELDENY_SR(s, dest, i);
    }
  RELASSERT_SR(s, dest, dest);
  RELASSERT_LT(s, dest, dest);

  PROPDENY_NOTNULL(s, dest);

  /* Don't need to call rel_null here as we must already be <= everything,
     as src  must have been <= everything */
  if (PROPHOLDS(s->null, src))
    PROPASSERT_NULL(s, dest);
  else
    PROPDENY_NULL(s, dest);
}

void rel_assign_sameregion(sr_state s, int nvars, place dest, place src)
{
  int i;

  /* old knowledge about dest is removed, dest is SRas src */
  for (i = 0; i < nvars; i++)
    {
      if (!PROPHOLDS(s->null, i))
	{
	  RELDENY_SR(s, i, dest);
	  RELDENY_LT(s, i, dest);
	}

      if (RELHOLDS_SR(s, src, i))
	RELASSERT_SR(s, dest, i);
      else
	RELDENY_SR(s, dest, i);

      if (RELHOLDS_LT(s, src, i))
	RELASSERT_LT(s, dest, i);
      else
	RELDENY_LT(s, dest, i);
    }
  RELASSERT_SR(s, dest, dest);
  RELASSERT_LT(s, dest, dest);

  PROPDENY_NOTNULL(s, dest);

  /* Don't need to call rel_null here as we must already be <= everything,
     as src  must have been <= everything */
  if (PROPHOLDS(s->null, src))
    PROPASSERT_NULL(s, dest);
  else
    PROPDENY_NULL(s, dest);
}

void rel_sameregion(sr_state s, int nvars, place a, place b);
void rel_lessthan(sr_state s, int nvars, place a, place b);

void rel_null(sr_state s, int nvars, place p)
{
  int i;

  /* This is most likely to be called for code that dereferences a null
     pointer (with surprising consequences ;-)) */
  if (PROPHOLDS(s->null, p))
    return;

  PROPASSERT_NULL(s, p);

  /* p is now less than everything */
  for (i = 0; i < nvars; i++)
    {
      rel_sameregion(s, nvars, p, i);
      rel_lessthan(s, nvars, p, i);

      if (RELHOLDS_LT(s, i, p))
	rel_null(s, nvars, i);
    }
}

void rel_notnull(sr_state s, int nvars, place p)
{
  int i;

  if (PROPHOLDS(s->notnull, p))
    return;
  PROPASSERT_NOTNULL(s, p);

  /* p is now known not to be NULL, thus p SRas q implies p = q (so we add q SRas p) */
  for (i = 0; i < nvars; i++)
    {
      if (RELHOLDS_SR(s, p, i))
	rel_sameregion(s, nvars, i, p);
      if (RELHOLDS_LT(s, p, i))
	rel_notnull(s, nvars, i);
    }

  /* Not bothering to deal with case where PROPHOLDS(s->null, p) (which
     implies everything), as there is little point ;-) */
}

void rel_sameregion(sr_state s, int nvars, place a, place b)
{
  int i;

  if (RELHOLDS_SR(s, a, b))
    return;

  RELASSERT_SR(s, a, b);

  for (i = 0; i < nvars; i++)
    {
      if (RELHOLDS_SR(s, i, a))
	rel_sameregion(s, nvars, i, b);

      if (RELHOLDS_SR(s, b, i))
	rel_sameregion(s, nvars, a, i);
    }

  if (PROPHOLDS(s->notnull, a))
    {
      rel_sameregion(s, nvars, b, a);
      rel_notnull(s, nvars, b);
    }

  if (PROPHOLDS(s->null, b))
    rel_null(s, nvars, a);

  rel_lessthan(s, nvars, a, b);
}

void rel_lessthan(sr_state s, int nvars, place a, place b)
{
  int i;

  if (RELHOLDS_LT(s, a, b))
    return;

  RELASSERT_LT(s, a, b);

  for (i = 0; i < nvars; i++)
    {
      if (RELHOLDS_LT(s, i, a))
	rel_lessthan(s, nvars, i, b);

      if (RELHOLDS_LT(s, b, i))
	rel_lessthan(s, nvars, a, i);
    }

  if (PROPHOLDS(s->notnull, a))
    rel_notnull(s, nvars, b);

  if (PROPHOLDS(s->null, b))
    rel_null(s, nvars, a);

  if (RELHOLDS_LT(s, b, a))
    {
      rel_sameregion(s, nvars, a, b);
      rel_sameregion(s, nvars, b, a);
    }
}

void rel_glb(sr_state dest, sr_state in, int nvars)
{
#if 0
  int i, j;
  bitset rin = in->sr, rdest = dest->sr;

  for (i = 0; i < nvars; i++)
    {
      for (j = 0; j < nvars; j++)
	if (!NVRELHOLDS(rin, nvars, i, j))
	  NVRELDENY(rdest, nvars, i, j);

      if (!PROPHOLDS(in->null, i))
	PROPDENY_NULL(dest, i);

      if (!PROPHOLDS(in->notnull, i))
	PROPDENY_NOTNULL(dest, i);
    }
#else
  bunionb(dest->sr, in->sr);
  bunionb(dest->lt, in->lt);
  bintersectionb(dest->null, in->null);
  bintersectionb(dest->notnull, in->notnull);
#endif
}

void rel_copy(sr_state dest, sr_state in)
{
  bassignb(dest->sr, in->sr);
  bassignb(dest->lt, in->lt);
  bassignb(dest->notnull, in->notnull);
  bassignb(dest->null, in->null);
}

bool rel_eqp(sr_state s1, sr_state s2)
{
  return bitset_eqp(s1->sr, s2->sr) &&
    bitset_eqp(s1->lt, s2->lt) &&
    bitset_eqp(s1->notnull, s2->notnull) &&
    bitset_eqp(s1->null, s2->null);
}

/* Like rel_sameregion but handles generalised places */
void rel_general_sameregion(sr_state s, int nvars, gplace gp1, gplace gp2)
{
  place p1 = gp2place(gp1), p2 = gp2place(gp2);

  if (p1 == PNULL || p1 == PUNKNOWN || p2 == PUNKNOWN || p1 == p2 ||
      gp_sr(gp1) || gp_lt(gp1))
    return;

  assert(p1 < nvars);

  if (p2 == PNULL)
    {
      int i;

      /* Note that p2 == PNULL actually implies that the program will segfault if
	 it executes this call (arguments are not assignable, so their appearance
	 on the right hand side of <= implies a dereference). This explains the
	 somewhat surprising rule here... */
      for (i = 0; i < nvars; i++)
	rel_sameregion(s, nvars, p1, i);
    }
  else if (gp_lt(gp2))
    rel_lessthan(s, nvars, p1, p2);
  else
    rel_sameregion(s, nvars, p1, p2);
}

/* Like rel_lessthan but handles generalised places */
void rel_general_lessthan(sr_state s, int nvars, gplace gp1, gplace gp2)
{
  rel_general_sameregion(s, nvars, gp1, gp_setlt(gp_clearsr(gp2)));
}

void rel_general_if(sr_state called_s, int called_nvars, place cp1, place cp2,
		    sr_state s, int nvars, gplace gp1, gplace gp2)
{
  if (NVRELHOLDS(called_s->sr, called_nvars, cp1, cp2))
    rel_general_sameregion(s, nvars, gp1, gp2);

  if (NVRELHOLDS(called_s->lt, called_nvars, cp1, cp2))
    rel_general_lessthan(s, nvars, gp1, gp2);
}

void rel_fcall(sr_state s, int nvars, gplace *args, function_decl called)
{
  sr_state called_s = &NODE_GET(struct sr_node *, called->sr_vertex)->out;
  int called_nptrargs = called->sr_nptrargs;
  int called_nvars = called->sr_nvars;
  int called_return_id = called->sr_return_id;
  int i, j;
  gplace result = args[called_nptrargs];

  if (result != PUNKNOWN)
    {
      assert(!gp_sr(result) && !gp_lt(result) && result < 4 * nvars);
      /* Hmm, this should be unecessary (fn result ids are set to top at
	 function entry and can't be used before here) */
      rel_kill(s, nvars, gp2place(result)); /* Lose prior information about result */

      rel_general_if(called_s, called_nvars, PGLOBAL, called_return_id,
		     s, nvars, PGLOBAL, result);
      rel_general_if(called_s, called_nvars, called_return_id, PGLOBAL,
		     s, nvars, result, PGLOBAL);

      if (PROPHOLDS(called_s->notnull, called_return_id))
	rel_notnull(s, nvars, gp2place(result));

      if (PROPHOLDS(called_s->null, called_return_id))
	rel_null(s, nvars, gp2place(result));
    }

  for (i = 0; i < called_nptrargs; i++)
    if (args[i] != PUNKNOWN)
      {
	int parmi = called_nvars - called_nptrargs + i;
	gplace locali = args[i];

	rel_general_if(called_s, called_nvars, PGLOBAL, parmi,
		       s, nvars, PGLOBAL, locali);

	if (result != PUNKNOWN)
	  rel_general_if(called_s, called_nvars, called_return_id, parmi,
			 s, nvars, result, locali);

	if (locali != PNULL && PROPHOLDS(called_s->notnull, parmi))
	  rel_notnull(s, nvars, gp2place(locali));

	if (!(gp_sr(locali) || gp_lt(locali)))
	  {
	    if (locali != PNULL && PROPHOLDS(called_s->null, parmi))
	      rel_null(s, nvars, gp2place(locali));

	    rel_general_if(called_s, called_nvars, parmi, PGLOBAL,
			   s, nvars, locali, PGLOBAL);

	    if (result != PUNKNOWN)
	      rel_general_if(called_s, called_nvars, parmi, called_return_id,
			     s, nvars, locali, result);

	    for (j = 0; j < called_nptrargs; j++)
	      {
		int parmj = called_nvars - called_nptrargs + j;

		rel_general_if(called_s, called_nvars, parmi, parmj,
			       s, nvars, locali, args[j]);
	      }
	  }
      }
}

static gnode *sr_preorder_build(gnode n, gnode *pre)
{
  gedge out;

  if (graph_node_markedp(n))
    return pre;

  graph_mark_node(n);

  *pre++ = n;
  graph_scan_out (out, n)
    pre = sr_preorder_build(graph_edge_to(out), pre);

  return pre;
}

void sr_build_preorder(region r, function_decl fd)
{
  int nnodes = 0;
  gnode n;

  graph_scan_nodes (n, fd->sr_graph)
    nnodes++;
  fd->sr_size = nnodes;

  fd->sr_preorder = rarrayalloc(r, nnodes, gnode);
  graph_clear_all_marks(fd->sr_graph);
  sr_preorder_build(fd->sr_entry, fd->sr_preorder);

  /* The function exit can be unreachable (everything else is reachable
     as we are basing ourselves on the preorder cfg) */
  if (!fd->sr_preorder[nnodes - 1])
    fd->sr_size--;
}

void initialise_sameregion_dataflow(region r, function_decl fd)
{
  int nvars = fd->sr_nvars, i, j;
  sr_state sentry;
  gnode g;

#ifdef DPRINT
  i = 0;
  graph_scan_nodes (g, fd->sr_graph)
    i++;
  fprintf(stderr, "%s: %ld cfg nodes, %d srcfg nodes, %d ptr vars\n", fd->ddecl->name,
	  fd->postorder_size, i, nvars);
  fflush(stderr);
#endif

  /* Create constraint relation matrices */
  graph_scan_nodes (g, fd->sr_graph)
    {
      struct sr_node *sr_node = NODE_GET(struct sr_node *, g);

      /* Allocate relation at each constraint node, with relation holding between
	 all vars */
      sr_node->in.sr = new_bitset(r, nvars * nvars);
      sr_node->in.lt = new_bitset(r, nvars * nvars);
      sr_node->in.notnull = bcomplementb(new_bitset(r, nvars));
      sr_node->in.null = bcomplementb(new_bitset(r, nvars));
      sr_node->out.sr = new_bitset(r, nvars * nvars);
      sr_node->out.lt = new_bitset(r, nvars * nvars);
      sr_node->out.notnull = bcomplementb(new_bitset(r, nvars));
      sr_node->out.null = bcomplementb(new_bitset(r, nvars));
    }

  /* Assume everything is distinct at entry, nothing is null or notnull */
  sentry = &NODE_GET(struct sr_node *, fd->sr_entry)->in;
  for (i = 0; i < nvars; i++)
    {
      for (j = 0; j < nvars; j++)
	{
	  RELDENY_SR(sentry, i, j);
	  RELDENY_LT(sentry, i, j);
	}
      RELASSERT_SR(sentry, i, i);
      RELASSERT_LT(sentry, i, i);
    }
  bclearb(sentry->notnull);
  PROPASSERT_NOTNULL(sentry, PGLOBAL);
  bclearb(sentry->null);

  fd->sr_temp.sr = new_bitset(r, nvars * nvars);
  fd->sr_temp.lt = new_bitset(r, nvars * nvars);
  fd->sr_temp.notnull = new_bitset(r, nvars);
  fd->sr_temp.null = new_bitset(r, nvars);

  sr_build_preorder(r, fd);
}

bool sameregion_dataflow(function_decl fd)
{
  int nvars = fd->sr_nvars, i;
  sr_state temp = &fd->sr_temp;
  bool change;
  int itercount = 0;

#ifdef DPRINT
  fprintf(stderr, "%s: ", fd->ddecl->name);
  fflush(stderr);
#endif

  /* Here we go. */
  do
    {
      gnode *sr_nodes = fd->sr_preorder;
      int nnodes = fd->sr_size;

      change = FALSE;
      itercount++;

      for (i = 0; i < nnodes; i++)
	{
	  gnode node = sr_nodes[i];
	  struct sr_node *sr_node = NODE_GET(struct sr_node *, node);
	  gedge in;

	  if (!change)
	    rel_copy(temp, &sr_node->out);

	  /* Merge predecessors */
	  graph_scan_in (in, node)
	    rel_glb(&sr_node->in, &NODE_GET(struct sr_node *, graph_edge_from(in))->out, nvars);

	  /* Apply this node */
	  switch (sr_node->kind)
	    {
	    case sr_nop:
	      sr_node->out = sr_node->in;
	      break;
	    case sr_kill:
	      rel_copy(&sr_node->out, &sr_node->in);
	      rel_kill(&sr_node->out, nvars, sr_node->p1);
	      break;
	    case sr_null:
	      rel_copy(&sr_node->out, &sr_node->in);
	      rel_null(&sr_node->out, nvars, sr_node->p1);
	      break;
	    case sr_notnull:
	      rel_copy(&sr_node->out, &sr_node->in);
	      rel_notnull(&sr_node->out, nvars, sr_node->p1);
	      break;
	    case sr_assign:
	      rel_copy(&sr_node->out, &sr_node->in);
	      rel_assign(&sr_node->out, nvars, sr_node->p1, sr_node->p2);
	      break;
	    case sr_assign_null:
	      rel_copy(&sr_node->out, &sr_node->in);
	      rel_assign_null(&sr_node->out, nvars, sr_node->p1);
	      break;
	    case sr_assign_sameregion:
	      rel_copy(&sr_node->out, &sr_node->in);
	      rel_assign_sameregion(&sr_node->out, nvars, sr_node->p1, sr_node->p2);
	      break;
	    case sr_sameregion:
	      rel_copy(&sr_node->out, &sr_node->in);
	      rel_sameregion(&sr_node->out, nvars, sr_node->p1, sr_node->p2);
	      break;
	    case sr_assign_lessthan:
	      rel_copy(&sr_node->out, &sr_node->in);
	      rel_assign_lessthan(&sr_node->out, nvars, sr_node->p1, sr_node->p2);
	      break;
	    case sr_lessthan:
	      rel_copy(&sr_node->out, &sr_node->in);
	      rel_lessthan(&sr_node->out, nvars, sr_node->p1, sr_node->p2);
	      break;
	    case sr_fcall:
	      rel_copy(&sr_node->out, &sr_node->in);
	      rel_fcall(&sr_node->out, nvars, sr_node->fcall_args, sr_node->fcall_fn);
	      break;
	    }

	  if (!change && !rel_eqp(temp, &sr_node->out))
	    change = TRUE;
	}
    }
  while (change);
#ifdef DPRINT
  fprintf(stderr, " %d iters\n", itercount);
#endif

  return itercount != 1;
}

void optimise_rc_checks(region r, function_decl fd, bitset locals) deletes
{
  double start = gettime();
  double tt;

  /* Step 1: find regions of all expressions, build sameregion-relation cfg nodes */
  collect_expression_regions(fd);

  /* Step 2: reduce the place id space to manageable proportions */
  eliminate_temp_place_ids(r, fd);

  /* Step 3: build reduced cfg */
  build_sameregion_cfg(r, fd);

  if (!rc_chkopt_perfile)
    {
      /* Step 4: perform dataflow on reduced cfg */
      initialise_sameregion_dataflow(r, fd);
      sameregion_dataflow(fd);
    }

  /* As checks are not explicit in the cfg, we don't remove them. The unparsing
     stage will avoid generating checks which this analysis determines to be
     unecessary */

  tt = gettime() - start;
  chkopt_time += tt;
  fd->chkopt_time += tt;
}

void update_called_function_inputs(function_decl fd)
{
  gnode node;
  int nvars = fd->sr_nvars;

  graph_scan_nodes (node, fd->sr_graph)
    {
      struct sr_node *sr_node = NODE_GET(struct sr_node *, node);

      if (sr_node->kind == sr_fcall && !sr_node->fcall_fn->sr_unknown_arguments &&
	  sr_node->fcall_fn->sr_entry) /* Ignore library functions */
	{
	  sr_state sin = &sr_node->in;
	  gplace *args = sr_node->fcall_args;
	  function_decl called = sr_node->fcall_fn;
	  sr_state input_s = &NODE_GET(struct sr_node *, called->sr_entry)->in;
	  bitset input_sr = input_s->sr;
	  bitset input_lt = input_s->lt;
	  int called_nptrargs = called->sr_nptrargs;
	  int called_nvars = called->sr_nvars;
	  int i, j;

	  /* If something does not hold at sr_node->in, then it must not hold
	     for the corresponding parameters at input_sr */

	  for (i = 0; i < called_nptrargs; i++)
	    {
	      gplace argi = args[i];
	      place parmi = called_nvars - called_nptrargs + i, parmj;

	      if (argi == PUNKNOWN)
		{
		  /* We must deny all knowledge about the ith parameter as we cannot show
		     i <= anything (except i<=i) */
		  for (parmj = called_nvars - called_nptrargs; parmj < called_nvars; parmj++)
		    {
		      NVRELDENY(input_sr, called_nvars, parmi, parmj);
		      NVRELDENY(input_lt, called_nvars, parmi, parmj);
		    }
		  NVRELDENY(input_sr, called_nvars, parmi, PGLOBAL);
		  NVRELDENY(input_lt, called_nvars, parmi, PGLOBAL);
		  NVRELDENY(input_sr, called_nvars, PGLOBAL, parmi);
		  NVRELDENY(input_lt, called_nvars, PGLOBAL, parmi);
		  NVRELASSERT(input_sr, called_nvars, parmi, parmi);
		  NVRELASSERT(input_lt, called_nvars, parmi, parmi);

		  PROPDENY_NOTNULL(input_s, parmi);
		  PROPDENY_NULL(input_s, parmi);
		}
	      else if (argi == PNULL)
		{
		  /* We can show argi <= everything, so only deny parmi != null
		     and GBL <=/SRas argi */
		  PROPDENY_NOTNULL(input_s, parmi);
		  NVRELDENY(input_sr, called_nvars, PGLOBAL, parmi);
		  NVRELDENY(input_lt, called_nvars, PGLOBAL, parmi);
		}
	      else
		{
		  /* Note: gp_sr/lt(argi) is not a problem, as x <= a, a <= b
		     implies x <= b */
		  place placei = gp2place(argi);
		  bool inullp = gp_sr(argi) || gp_lt(argi); 

		  if (!RELHOLDS_SR(sin, placei, PGLOBAL))
		    NVRELDENY(input_sr, called_nvars, parmi, PGLOBAL);

		  if (!RELHOLDS_LT(sin, placei, PGLOBAL))
		    NVRELDENY(input_lt, called_nvars, parmi, PGLOBAL);

		  if (!PROPHOLDS(sr_node->in.null, placei))
		    PROPDENY_NULL(input_s, parmi);

		  if (inullp || !PROPHOLDS(sr_node->in.notnull, placei))
		    PROPDENY_NOTNULL(input_s, parmi);

		  if (inullp || !RELHOLDS_SR(sin, PGLOBAL, placei))
		    NVRELDENY(input_sr, called_nvars, PGLOBAL, parmi);

		  if (inullp || !RELHOLDS_LT(sin, PGLOBAL, placei))
		    NVRELDENY(input_lt, called_nvars, PGLOBAL, parmi);

		  for (j = 0; j < called_nptrargs; j++)
		    {
		      gplace argj = args[j];
		      bool junknown = argj == PUNKNOWN || argj == PNULL ||
			gp_sr(argj) || gp_lt(argj);

		      parmj = called_nvars - called_nptrargs + j;

		      /* If we can't show argi <= argj, deny it at the called fn */
		      if (junknown || !RELHOLDS_SR(sin, placei, gp2place(argj)))
			NVRELDENY(input_sr, called_nvars, parmi, parmj);
		      if (junknown || !RELHOLDS_LT(sin, placei, gp2place(argj)))
			NVRELDENY(input_lt, called_nvars, parmi, parmj);
		    }
		}
	    }
	}
    }
}

void optimise_rc_checks_perfile(declaration the_program)
{
  double start = gettime();
  declaration d;
  region work = newregion();
  bool change;
  int itercount = 0;

  /* XXX: This a) shouldn't be here, b) doesn't notice functions in local static
     initialisers */
  /* Finish finding addressed functions (check global initialisers) */
  scan_declaration (d, the_program)
    {
      declaration reald = d;

      while (is_extension_decl(reald))
	reald = CAST(extension_decl, reald)->decl;

      if (is_data_decl(reald))
	{
	  declaration vars;

	  scan_declaration (vars, CAST(data_decl, reald)->decls)
	    {
	      expression init = CAST(variable_decl, vars)->arg1;

	      if (init)
		find_addressed_functions(init);
	    }
	}
    }

  scan_declaration (d, the_program)
    if (is_function_decl(d))
      {
	function_decl fd = CAST(function_decl, d);
	int oldnvars = fd->sr_nvars - 1;
	int nptrargs = 0;
	declaration parm;

	/* If a function is not static, or has been used as a value, then not all
	   calls will be apparent, so input arguments are unknown */
	if (fd->ddecl->addressed || fd->ddecl->ftype != function_static)
	  fd->sr_unknown_arguments = TRUE;

	/* Scan the arguments and collect the pointer ones */
	scan_declaration (parm, fd->fdeclarator->parms)
	  {
	    data_declaration dd = get_parameter(parm);

	    if (dd && dd->interesting)
	      nptrargs++;
	  }
	fd->sr_nptrargs = nptrargs;

	if (nptrargs)
	  {
	    /* Create some new ids for parameters (we want to pretend that parameters
	       are immutable, we do that by creating new ids for them, and "assigning"
	       the real ids from the new ones) */
	    node new_entry = new_node(work, dummy_location);
	    gnode last_constraint;

	    scan_declaration (parm, fd->fdeclarator->parms)
	      {
		data_declaration dd = get_parameter(parm);

		if (dd && dd->interesting)
		  really_add_constraint(work, fd, new_entry, sr_assign,
					fd->sr_local2placeidmap[dd->id], fd->sr_nvars++,
					NULL, NULL);
	      }

	    /* Link these constraints in at function entry */
	    last_constraint = end_of_chain(new_entry->sr_vertex);
	    graph_add_edge(last_constraint, fd->sr_entry, NULL);
	    fd->sr_entry = new_entry->sr_vertex;
	  }

#ifndef NDEBUG
	{
	  data_declaration *newsr_vmap =
	    rarrayalloc(work, fd->sr_nvars, data_declaration);
	  int ii;

	  for (ii = 0; ii < oldnvars; ii++)
	    newsr_vmap[ii] = fd->sr_vmap[ii];
	  fd->sr_vmap = newsr_vmap;
	}
#endif

	initialise_sameregion_dataflow(work, fd);

	if (!fd->sr_unknown_arguments && nptrargs)
	  {
	    sr_state sentry = &NODE_GET(struct sr_node *, fd->sr_entry)->in;

	    /* Initially assume that all pointer arguments are "equal"
	       (this is the "top" constraint relation value), and
	       that all arguments are both null and non-null (ouch!) */
	    int i, j, nvars = fd->sr_nvars;

#define PARMPLACE(n) (nvars - nptrargs + (n))

	    for (i = 0; i < nptrargs; i++)
	      {
		RELASSERT_SR(sentry, PARMPLACE(i), PGLOBAL);
		RELASSERT_LT(sentry, PARMPLACE(i), PGLOBAL);
		RELASSERT_SR(sentry, PGLOBAL, PARMPLACE(i));
		RELASSERT_LT(sentry, PGLOBAL, PARMPLACE(i));
		for (j = 0; j < nptrargs; j++)
		  {
		    RELASSERT_SR(sentry, PARMPLACE(i), PARMPLACE(j));
		    RELASSERT_LT(sentry, PARMPLACE(i), PARMPLACE(j));
		  }
		PROPASSERT_NOTNULL(sentry, PARMPLACE(i));
		PROPASSERT_NULL(sentry, PARMPLACE(i));
	      }
	  }
      }

  do
    {
      change = FALSE;

      ++itercount;
#ifdef DPRINT
      fprintf(stderr, "\n\nGLOBAL ITERATION %d\n\n\n", itercount);
#endif
      scan_declaration (d, the_program)
	if (is_function_decl(d))
	  {
	    function_decl fd = CAST(function_decl, d);
	    double start0 = gettime();

	    change = sameregion_dataflow(fd) || change;
	    update_called_function_inputs(fd);
	    fd->chkopt_time += gettime() - start0;
	  }
    }
  while (change);

  chkopt_time += gettime() - start;

  if (rc_collect_statistics)
    {
      fprintf(stderr, "chkopt time %.2f/iters %d\n", chkopt_time, itercount);

      scan_declaration (d, the_program)
	if (is_function_decl(d))
	  {
	    function_decl fd = CAST(function_decl, d);

	    fprintf(stderr, "CHKOPT %s %ld %d %.2f\n",
		    fd->ddecl->name, fd->postorder_size, fd->sr_nvars,
		    fd->chkopt_time);
	  }
      
    }

  /*deleteregion(work);*/
}

static void chkopt_return_1st(region r, data_declaration fn_decl, int nptrargs,
			      bool notnull)
{
  function_decl dummydef;
  function_declarator fdecl;
  variable_decl ast_decl;
  int nvars = 1 + nptrargs;
  struct sr_node *sr_node;
  int nptrargs2, i, j;
  declaration args;

  if (!fn_decl || fn_decl->definition)
    return;

  ast_decl = CAST(variable_decl, fn_decl->ast);

  /* Find actual function declarator */
  fdecl = get_fdeclarator(ast_decl->declarator);

  /* Check number of pointer args (sanity check) */
  if (!fdecl)
    return;
  nptrargs2 = 0;
  scan_declaration (args, fdecl->parms)
    switch (args->kind)
      {
      case kind_data_decl:
	if (type_pointer(CAST(variable_decl, CAST(data_decl, args)->decls)->ddecl->type))
	  nptrargs2++;
	break;
      case kind_oldidentifier_decl:
	if (type_pointer(CAST(oldidentifier_decl, args)->ddecl->type))
	  nptrargs2++;
	break;
      default:
	assert(0);
      }
  if (nptrargs2 != nptrargs)
    return;
  
  /* Make a dummy definition and give it the right signature */
  dummydef = new_function_decl(r, dummy_location, ast_decl->declarator,
			       NULL, /* Luckily we won't look at this field */
			       ast_decl->attributes,
			       NULL, NULL, NULL, NULL);
  fn_decl->definition = CAST(declaration, dummydef);
  dummydef->ddecl = fn_decl;

  dummydef->fdeclarator = fdecl;
  dummydef->sr_graph = new_graph(r);
  sr_node = new_sr_node(r, sr_nop, 0, 0, NULL, NULL);
  dummydef->sr_vertex = graph_add_node(dummydef->sr_graph, sr_node);
  sr_node->out.notnull = new_bitset(r, nvars);
  if (notnull)
    PROPASSERT_NOTNULL(&sr_node->out, 1);
  sr_node->out.null = new_bitset(r, nvars);
  sr_node->out.sr = new_bitset(r, nvars * nvars);
  sr_node->out.lt = new_bitset(r, nvars * nvars);
  for (i = 0; i < nvars; i++)
    {
      for (j = 0; j < nvars; j++)
	{
	  RELDENY_SR(&sr_node->out, i, j);
	  RELDENY_LT(&sr_node->out, i, j);
	}
      RELASSERT_SR(&sr_node->out, i, i);
      RELASSERT_LT(&sr_node->out, i, i);
    }
  dummydef->sr_nvars = nvars;
  dummydef->sr_return_id = 1;
  dummydef->sr_nptrargs = nptrargs;
}

void chkopt_libfn_init(void)
{
  chkopt_return_1st(parse_region, lookup_global_id("__rcralloc"), 2, TRUE);
  chkopt_return_1st(parse_region, lookup_global_id("__rcrarrayalloc"), 2, TRUE);
  chkopt_return_1st(parse_region, lookup_global_id("rstralloc"), 1, TRUE);
  chkopt_return_1st(parse_region, lookup_global_id("rstralloc0"), 1, TRUE);
  chkopt_return_1st(parse_region, lookup_global_id("rstrdup"), 2, TRUE);
  chkopt_return_1st(parse_region, lookup_global_id("regionof"), 1, FALSE);
}

static bool place_relholds(gnode sr_vertex, bool lt, place p1, place p2)
{
  sr_state s;
  bitset rel;
  int nvars;

  if (!(rc_chkopt || rc_chkopt_perfile))
    return FALSE;
  
  if (p1 == PNULL)
    return TRUE; /* Even if p2 == PUNKNOWN ! */

  if (p1 == PUNKNOWN || p2 == PUNKNOWN || p2 == PNULL)
    return FALSE;

  if (p1 == p2)
    return TRUE;

  if (!sr_vertex)
    return FALSE;

  s = &NODE_GET(struct sr_node *, sr_vertex)->in;
  rel = lt ? s->lt : s->sr;

  /* Ick! (rather inefficient to do this, this way, here, but not performance 
     critical). The + 0.1 avoids rounding surprises */
  nvars = sqrt(rel->size) + .1;
  return NVRELHOLDS(rel, nvars, p1, p2);
}

bool known_traditional_write(binary rcop)
{
  return place_relholds(rcop->sr_vertex, FALSE, gp2place(rcop->arg2->rregion), PGLOBAL);
}

bool known_sameregion_write(update_rc rcop)
{
  gplace glp = rcop->arg1->lregion;
  place rp = gp2place(rcop->arg2->rregion), lp = gp2place(glp);

  /* If lregion is <= something, we can't deduce anything.
     If its SR as something we're fine as if it's NULL the program will die */
  if (gp_lt(glp))
    return FALSE;

  /* If either dest-rvalue SRas src-lvalue or src-lvalue SRas dest-rvalue we
     know the check is safe:
     - the former is obvious
     - the latter follows from the fact we know that if the write succeeds
       then src is not null, so src SRas dest => src = dest
  */
  return place_relholds(rcop->sr_vertex, FALSE, rp, lp) ||
    place_relholds(rcop->sr_vertex, FALSE, lp, rp);
}

bool known_parentptr_write(update_rc rcop)
{
  gplace glp = rcop->arg1->lregion;
  place rp = gp2place(rcop->arg2->rregion), lp = gp2place(glp);

  /* If lregion is <= something, we can't deduce anything.
     If its SR as something we're fine as if it's NULL the program will die */
  if (gp_lt(glp))
    return FALSE;

  /* If either dest-rvalue <= src-lvalue or src-lvalue SRas dest-rvalue we
     know the check is safe:
     - the former is obvious
     - the latter follows from the fact we know that if the write succeeds
       then src is not null, so src SRas dest => src = dest
  */
  return place_relholds(rcop->sr_vertex, TRUE, rp, lp) ||
    place_relholds(rcop->sr_vertex, FALSE, lp, rp);

}

#if 0
void output_place(place p)
{
  if (p == PNULL)
    output("NULL");
  else if (p == PUNKNOWN)
    output("TOP");
  else if (p == PGLOBAL)
    output("GBL");
  else
    output("%s", srvmap[p]->name);
}
#endif

void output_chkopt_debug(binary e)
{
#if 0
  int nvars = fd->sr_nvars;
  bitset rin;

  output(" /* dest: lv: ");
  output_place(e->arg1->lregion);
  output(" rv: ");
  output_place(e->arg1->rregion);
  output(", src: ");
  output_place(e->arg2->rregion);
  if (!e->sr_vertex)
    output(" no vertex");
    rin = 

  if (place_relholds(e->sr_vertex, FALSE, gp2place(e->arg2->rregion), gp2place(e->arg1->lregion)))
    output(" src SRas lv");
  if (place_relholds(e->sr_vertex, FALSE, gp2place(e->arg2->rregion), PGLOBAL))
    output(" src SRas GBL");

  output(" */ ");
#endif
}
    
static char *itoa(int n)
{
  static char foo[20];

  sprintf(foo, "%d", n);

  return foo;
}

void dbg_rel(function_decl fd, char *name, bitset rel)
{
  int i, j;
  int width = 4, l;
  int nvars = fd->sr_nvars;

  for (i = 0; i < nvars; i++)
    if (fd->sr_vmap[i] && (l = strlen(fd->sr_vmap[i]->name)) > width)
      width = l;

  fprintf(stderr, "%*s", width, name);

  for (i = 0; i < nvars; i++)
    fprintf(stderr, "%*s", width + 1, i ? fd->sr_vmap[i] ? fd->sr_vmap[i]->name : itoa(i) : "GBL");
  fprintf(stderr, "\n");

  for (i = 0; i < nvars; i++)
    {
      fprintf(stderr, "%*s ", width, i ? fd->sr_vmap[i] ? fd->sr_vmap[i]->name : itoa(i) : "GBL");
      for (j = 0; j < nvars; j++)
	fprintf(stderr, "%*s", width + 1, NVRELHOLDS(rel, nvars, i, j) ? " X": "");
      fprintf(stderr, "\n");
    }
  fflush(stderr);
}

void dbg_set(function_decl fd, char *name, bitset set)
{
  int i;
  int nvars = fd->sr_nvars;

  fprintf(stderr, "%s:", name);
  for (i = 0; i < nvars; i++)
    if (PROPHOLDS(set, i))
      fprintf(stderr, " %s", i ? fd->sr_vmap[i] ? fd->sr_vmap[i]->name : itoa(i) : "GBL");

  fprintf(stderr, "\n");
  fflush(stderr);
}

void print_sr_node(gnode n)
{
  struct sr_node *sr_node = NODE_GET(struct sr_node *, n);
  int i;

  fprintf(stderr, " :");
  switch (sr_node->kind)
    {
    case sr_nop:
      fprintf(stderr, " NOP");
      break;
    case sr_kill:
      fprintf(stderr, " KILL %d", sr_node->p1);
      break;
    case sr_null:
      fprintf(stderr, " NULL %d", sr_node->p1);
      break;
    case sr_notnull:
      fprintf(stderr, " NOTNULL %d", sr_node->p1);
      break;
    case sr_sameregion:
      fprintf(stderr, " %d SR as %d", sr_node->p1, sr_node->p2);
      break;
    case sr_assign:
      fprintf(stderr, " %d = %d", sr_node->p1, sr_node->p2);
      break;
    case sr_assign_sameregion:
      fprintf(stderr, " %d =SR as %d", sr_node->p1, sr_node->p2);
      break;
    case sr_assign_null:
      fprintf(stderr, " %d = NULL", sr_node->p1);
      break;
    case sr_fcall:
      fprintf(stderr, " FCALL(%s)", sr_node->fcall_fn->ddecl->name);
      for (i = 0; i <= sr_node->fcall_fn->sr_nptrargs; i++)
	fprintf(stderr, " %d", sr_node->fcall_args[i]);
      break;
    default:
      abort();
    }
}

static function_decl debug_fd;

void print_full_sr_node(gnode n)
{
  struct sr_node *sr_node = NODE_GET(struct sr_node *, n);
  print_sr_node(n);

  fprintf(stderr, "\nIN\n");
  dbg_rel(debug_fd, "SR", sr_node->in.sr);
  dbg_rel(debug_fd, "<=", sr_node->in.lt);
  dbg_set(debug_fd, "notnull", sr_node->in.notnull);
  dbg_set(debug_fd, "null", sr_node->in.null);
  fprintf(stderr, "\nOUT\n");
  dbg_rel(debug_fd, "SR", sr_node->out.sr);
  dbg_rel(debug_fd, "<=", sr_node->out.lt);
  dbg_set(debug_fd, "notnull", sr_node->out.notnull);
  dbg_set(debug_fd, "null", sr_node->out.null);
}

