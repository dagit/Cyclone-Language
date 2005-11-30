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
#include "rc.h"
#include "fix.h"
#include "edit.h"
#include "cfg.h"
#include "live.h"
#include "dd_list.h"
#include "unparse.h"
#include "c-parse.h"
#include "c-lex.h"
#include "constants.h"
#include "vars.h"
#include "semantics.h"
#include "callcc1.h"
#include "flags.h"
#include "builtins.h"
#include "optrc.h"
#include "chkopt.h"
#include "AST_utils.h"

//#undef RCDEBUG
static FILE *debug_output;

data_declaration internal_rctypeof_decl;

tag_declaration region_decl;
type region_type, ptrint_type, rc_adjust_fn_type;
asttype ptrint_type_ast;

bool flag_perturb;
long perturb_seed;

static bool rc_init(void)
{
  typelist rc_adjust_fn_args;
  tag_ref region_tag;
  declarator ptd;
  type_element ptm;
  region r;

  /* XXX: Should do some consistency checks on this symbol */
  internal_rctypeof_decl = lookup_global_id("internal_rctypeof");
  if (!internal_rctypeof_decl)
    return FALSE;

  r = newregion();

  ptrint_type = unsigned_long_type;

  rc_adjust_fn_args = new_typelist(r);
  typelist_append(rc_adjust_fn_args, ptr_void_type);
  typelist_append(rc_adjust_fn_args, int_type);
  rc_adjust_fn_type = make_function_type(size_t_type, rc_adjust_fn_args,
					 FALSE, FALSE);

  region_tag = new_struct_ref(r, dummy_location,
			      new_word(r, dummy_location,
				       str2cstring(r, "region")),
			      NULL, NULL, FALSE);
  region_decl = lookup_global_tag(region_tag);
  if (!region_decl)
    region_decl = declare_global_tag(region_tag);
  pending_xref_error(); /* In case someone declared an enum or union region */

  if (rc_pairs_array)
    region_type = unsigned_short_type; /* must have as much range as __rcid in regions.h */
  else
    region_type = make_pointer_type(make_tagged_type(region_decl));

  type2ast(r, dummy_location, ptrint_type, NULL, &ptd, &ptm);
  ptrint_type_ast = new_asttype(r, dummy_location, ptd, ptm);

  return TRUE;
}

static bool rc_init_once(void)
{
  static bool done = FALSE;

  if (!done && rc_init())
    done = TRUE;

  return done;
}

data_declaration declare_region_temporary(region r, compound_stmt block)
{
  return declare_temporary(r, block, region_type);
}

void print_vars(bitset vars, data_declaration *vmap)
{
  int vid;

  fprintf(debug_output, "VARS:");
  bitset_scan (vid, vars)
    {
      data_declaration vd = vmap[vid];

      fprintf(debug_output, " %s(%p)", vd->name, vd);
    }
  fprintf(debug_output, "\n");
}

void add_rcop(region r, edge e, data_declaration v, bool increment)
{
  dd_list *vlist = increment ? &e->incvars : &e->decvars;

  if (!*vlist)
    *vlist = dd_new_list(r);
  assert(!dd_find(*vlist, v));
  dd_add_last(r, *vlist, v);
}

void insert_assignment_rcops(region r, function_decl fd, bitset vars)
{
  node parentptr *gnodes = fd->postorder_nodes;
  int nnodes = fd->postorder_size, i, vid;
  data_declaration *vmap = fd->vmap;

  /* Simple algorithm 1: On all nodes n:
     if n is an assignment to v:
       increment RC(v) on all edges from n to nodes where v is live on the way in
     for all v live on the way IN to n:
       decrement RC(v) on all edges from n to nodes where v is dead on the way in
  */
     
  /* Increment RC for all live parameters */
  assert(gnodes[nnodes - 1] == fd->cfg_entry);
  bitset_scan (vid, gnodes[nnodes - 1]->live_in)
    if (vmap[vid]->isparameter)
      add_rcop(r, gnodes[nnodes - 1]->edges_out, vmap[vid], TRUE);

  for (i = 0; i < nnodes; i++)
    {
      node n = gnodes[i];
      edge out;

      if (is_assign(n))
	{
	  assign a = CAST(assign, n);

	  if (is_identifier(a->arg1))
	    {
	      data_declaration id = CAST(identifier, a->arg1)->ddecl;

	      /* The test for membership in vars is not necessary, but
		 avoids useless work (only members of vars are marked
		 live) */
	      if (id->id != -1 && BIT_SETP(vars, id->id) &&
		  !zero_expression(a->arg2))
		for (out = n->edges_out; out; out = out->next_out)
		  if (BIT_SETP(out->to->live_in, id->id))
		    add_rcop(r, out, id, TRUE);
	    }
	}

      /* We need to decrement the RC of v in the following conditions:
	 - For all edges e, if v is live on the way out of source(e) and dead
	 on the way in to dest(e) (i.e., source(e) is a control-flow split
	 and the v is dead on the dest(e) branch). Decrement RC(v) on execution
	 of edge e.
	 - For all nodes n, if v is live on entrance to n and dead on
	 exit from n (i.e., n is the last use of v). Decrement RC(v) before
	 executing n.

	 As we currently only support addition of code on edges, we handle
	 the node case by adding the decrement RC(v) operation to all 
	 edges incoming to n. This is detected in the code below by the
	 test of liveonout (i.e. live on the way in to n and dead on the
	 way out of n is equivalent to live on the way out of a predecessor
	 of n and dead on the way out of n) */
      for (out = n->edges_out; out; out = out->next_out)
	{
	  bitset liveonin = out->to->live_in;
	  bitset liveonout = out->to->live_out;

	  bitset_scan (vid, n->live_out)
	    if (!BIT_SETP(liveonout, vid) || !BIT_SETP(liveonin, vid))
	      add_rcop(r, out, vmap[vid], FALSE);
	}
    }
}

void insert_functions_rcops(region r, function_decl fd, bitset vars)
{
  node parentptr *gnodes = fd->postorder_nodes;
  int nnodes = fd->postorder_size, i, vid;
  data_declaration *vmap = fd->vmap;

  /* Simple algorithm 2: On all nodes n:
     if n is a function call that may delete a region, for all variables v
     in vars live at exit from n:
       increment RC(v) on all edges into n
       decrement RC(v) on all edges out of n
  */
     
  for (i = 0; i < nnodes; i++)
    {
      node n = gnodes[i];
      edge out, in;

      /* Only calls to 'deletes' function where local is live before and after are black */
	if (is_function_call(n) &&
	    type_deletes(function_call_type(CAST(function_call, n))))
	  bitset_scan (vid, n->live_out)
	    {
	      data_declaration v = vmap[vid];

	      for (in = n->edges_in; in; in = in->next_in)
		add_rcop(r, in, v, TRUE);
	      for (out = n->edges_out; out; out = out->next_out)
		add_rcop(r, out, v, FALSE);
	    }
    }
}

static void add_rc_fn(function_decl fd, declaration rc_fn)
{
  rc_fn->next = CAST(node, fd->rc_fns);
  fd->rc_fns = rc_fn;
}

data_declaration lookup_rc_fn(region r, location loc, function_decl fd, type t,
			      bool update, bool do_pointer)
{
  data_declaration adjust_fn = NULL;
  char *adjustname = NULL;
  const char *tagname = NULL;

  if (type_pointer(t))
    {
      if (do_pointer)
	{
	  assert(!update);
	  adjustname = "__rc_adjustptr";
	}
    }
  else
    {
      tag_declaration tag = type_tag(t);

      name_tag(tag);
      tagname = tag->name;
      adjustname = rstralloc(r, strlen(tagname) + 11);
      sprintf(adjustname, "rc_%s_%s", update ? "update" : "adjust", tagname);
    }

  if (adjustname)
    {
      adjust_fn = lookup_global_id(adjustname);

      if (!adjust_fn)
	{
	  declaration rcfn;
	  data_declaration *rcfn_decl;
	  struct data_declaration tempdecl;
	  type rcfn_type;

	  /* We still create a "bad" function when this error occurs to avoid
	     duplicate messages */
	  if (type_contains_union_with_pointers(t))
	    error_with_location(loc, "must provide %s function for struct or union %s", adjustname, tagname);

	  if (update)
	    {
	      update_function_decl updfn = new_update_function_decl(r, loc);
	      type ptr_to_t = make_pointer_type(t);
	      typelist rc_update_fn_args;

	      rcfn = CAST(declaration, updfn);
	      rcfn_decl = &updfn->rcfn_decl;

	      rc_update_fn_args = new_typelist(r);
	      typelist_append(rc_update_fn_args, ptr_to_t);
	      typelist_append(rc_update_fn_args, ptr_to_t);
	      rcfn_type = make_function_type(void_type, rc_update_fn_args,
					     FALSE, FALSE);
	    }
	  else
	    {
	      adjust_function_decl adjfn = new_adjust_function_decl(r, loc);

	      rcfn = CAST(declaration, adjfn);
	      rcfn_decl = &adjfn->rcfn_decl;
	      rcfn_type = rc_adjust_fn_type;
	    }

	  init_data_declaration(&tempdecl, rcfn, adjustname, rcfn_type);
	  tempdecl.kind = decl_function;
	  tempdecl.needsmemory = TRUE;
	  tempdecl.isused = TRUE;
	  tempdecl.ftype = function_static;
	  tempdecl.isinline = TRUE;

	  *rcfn_decl = adjust_fn = declare(global_env, &tempdecl, FALSE);
	  rcfn->type = t;

	  add_rc_fn(fd, rcfn);
	}
      else if (adjust_fn->kind != decl_function)
	{
	  error_with_location(loc, "%s is not an rc_adjust function for struct or union %s not found", adjustname, tagname);
	  adjust_fn = NULL;
	}
    }
  return adjust_fn;
}

static expression *build_rcop(region r, location loc, function_decl fd,
			      expression idv, bool increment, expression *lastrcop)
{
  adjust_rc rcop = new_adjust_rc(r, loc, idv, increment);

  rcop->type = void_type;
  if (!type_pointer(idv->type))
    {
#ifndef ASSUME_GCC
      if (type_array(idv->type))
	{
	XXX:
	  ensure presence of __rc_adjustarray, or add an adjust_array_function_decl
	  node to fd
	}
#endif
      rcop->rcfn_decl =
	lookup_rc_fn(r, loc, fd, type_array_of_base(idv->type), FALSE, TRUE);
    }

  set_parent(CASTSRPTR(node, &rcop->arg1), CAST(node, rcop));
  *lastrcop = CAST(expression, rcop);

  return (expression *)&rcop->next;
}

void generate_rcops(region r, function_decl fd)
{
  node parentptr *gnodes = fd->postorder_nodes;
  int nnodes = fd->postorder_size, i;
     
  for (i = 0; i < nnodes; i++)
    {
      node n = gnodes[i];
      edge out;

      for (out = n->edges_out; out; out = out->next_out)
	{
	  dd_list_pos v;
	  expression rcops = NULL, *lastrcop = &rcops;
	  location loc = out->from->location;

	  /* Collect all rcops on this edge */
	  if (out->incvars)
	    dd_scan (v, out->incvars)
	      {
		dd_list_pos decpos;
		data_declaration vv = DD_GET(data_declaration, v);

		/* Don't generate ++/-- on same var on same edge */
		if (out->decvars && (decpos = dd_find(out->decvars, vv)))
		  dd_remove(decpos);
		else
		  lastrcop = build_rcop(r, loc, fd, build_identifier(r, loc, vv),
					TRUE, lastrcop);
	      }
	  if (out->decvars)
	    dd_scan (v, out->decvars)
	      lastrcop = build_rcop(r, loc, fd, build_identifier(r, loc, DD_GET(data_declaration, v)),
				    FALSE, lastrcop);

	  if (rcops)
	    {
	      /* If more than 1, wrap in comma expression */
	      if (rcops->next)
		{
		  rcops = CAST(expression, new_comma(r, n->location, rcops));
		  rcops->type = void_type;
		}
	      add_expression_to_edge(r, out, rcops);
	    }
	}
    }
}

static void adjust_localrc(region r, edge e, data_declaration local, bool increment)
{
  /* Not cross_pointers because the variables concerned are on the stack, so
     pointers are all cross-region anyway */
  if (type_contains_pointers(local->type))
    add_rcop(r, e, local, increment);
}

void add_local_rc(region r, function_decl fd, bitset done)
{
  node parentptr *gnodes = fd->postorder_nodes;
  int nnodes = fd->postorder_size, i, vid;
  data_declaration *vmap = fd->vmap;
  edge entry_edge;
  bool qdeletes = type_deletes(fd->ddecl->type);

  /* Add non-assignment relayed RC adjustments for all local variables not 
     in done. Rules:
     - increment RC for all parameters on entry
     - decrement RC for all locals on edges from in-scope to out-of-scope
  */

  /* Increment RC for all parameters not in done */
  assert(gnodes[nnodes - 1] == fd->cfg_entry);
  entry_edge = gnodes[nnodes - 1]->edges_out;
  bitset_scan (vid, fd->cfg_entry->inscope_vars)
    if (!BIT_SETP(done, vid) && (qdeletes || vmap[vid]->addressed))
      {
	data_declaration parm = vmap[vid];

	assert(parm->isparameter);
	adjust_localrc(r, entry_edge, parm, TRUE);
      }

  for (i = 0; i < nnodes; i++)
    {
      node n = gnodes[i];
      edge out;

      for (out = n->edges_out; out; out = out->next_out)
	{
	  bitset stayinscope = out->to->inscope_vars;

	  bitset_scan (vid, n->inscope_vars)
	    {
	      data_declaration v = vmap[vid];

	      if (!BIT_SETP(done, vid) && !BIT_SETP(stayinscope, vid) &&
		  (qdeletes || v->addressed || type_array(v->type)))
		adjust_localrc(r, out, v, FALSE);
	    }
	}
    }
}

static bool array_access(expression e)
{
  expression nfe;

  /* Find the base pointer in a mess of plus nodes */
  while (is_plus(e))
    {
      plus add = CAST(plus, e);

      if (type_integer(add->arg1->type))
	e = add->arg2;
      else
	e = add->arg1;
    }
  /* We answer yes if we see an array that is accessed (possibly via a bunch
     of fields) from some variable */
  return type_array(e->type) && (nfe = ignore_fields(e), is_identifier(nfe));
}

/* Add RC ops at assignment statements except for local variables whose
   address is not taken.
   Note: destinations of asm statements are considered as having
     their address taken. It is up to the assembly code to make sure the
     RC is correct
*/
void add_other_rc(region r, function_decl fd)
{
  node parentptr *gnodes = fd->postorder_nodes;
  int nnodes = fd->postorder_size, i;
  bool qdeletes = type_deletes(fd->ddecl->type);

  /* Add RC adjustments for all assignments to globals or locals
     and via pointers
  */

  /* The AST has been edited, but the assign nodes are still there */
  for (i = 0; i < nnodes; i++)
    {
      node n = gnodes[i];
      
      if (is_assign(n) && type_contains_pointers(n->type))
	{
	  assign ae = CAST(assign, n);
	  expression stabiliser, update, base_dest, newarg1;
	  location loc = ae->location;
	  data_declaration temp;
	  node sameregion *pp = ae->parent_ptr;
	  node parent = ae->parent, next = ae->next;
	  bool aeused;

	  /* Skip optimised locals and writes to non-addressed local structs in
	     non-deletes functions */
	  base_dest = ignore_fields(ae->arg1);
	  if (is_identifier(base_dest))
	    {
	      identifier id = CAST(identifier, base_dest);

	      if (id->ddecl->islocal &&
		  (id->ddecl->interesting ||
		   (!qdeletes && !id->ddecl->addressed)))
		continue;
	    }
	  /* Skip the special "clear to NULL" initialisations (there is
	     no old value) */
	  if (ae->clearing)
	    continue;

	  aeused = expression_used(CAST(expression, ae));

	  ae->next = NULL;

	  temp = stabilise_lvalue(r, parent_block(n), ae->arg1, &newarg1,
				  &stabiliser);
	  ae->arg1 = newarg1;
	  if (!stabiliser || array_access(CAST(assign, stabiliser)->arg2))
	    {
	      /* This must have been an assignment (possiblty to a field)
		 of a global or local. ae->arg2 may be a complex expression
		 (with side effects). It can also be an array access (of
		 a local or gloabl array) */
	      global_rc rcop;

	      rcop = new_global_rc(r, loc, ae->arg1, ae->arg2);
	      rcop->sr_vertex = ae->sr_vertex;
	      rcop->rcfn_decl = lookup_rc_fn(r, loc, fd, ae->arg1->type, FALSE, FALSE);
	      ae->rcop = CAST(generic_rc, rcop);

	      if (stabiliser)
		update = build_comma(r, loc, stabiliser, CAST(expression, rcop));
	      else
		update = CAST(expression, rcop);
	    }
	  else
	    {
	      /* Assignment via a pointer. */
	      update_rc rcop = new_update_rc(r, loc, ae->arg1, ae->arg2, temp);

	      rcop->sr_vertex = ae->sr_vertex;
	      rcop->rcfn_decl = lookup_rc_fn(r, loc, fd, ae->arg1->type, !USE_RC_ADJUST_FOR_UPDATE, FALSE);
	      ae->rcop = CAST(generic_rc, rcop);

	      set_parent(CASTSRPTR(node, &rcop->arg1), CAST(node, rcop));
	      set_parent(CASTSRPTR(node, &rcop->arg2), CAST(node, rcop));
	      update = build_comma(r, loc, stabiliser, CAST(expression, rcop));
	    }
	  /* Only reproduce arg1 if result used (avoid warnings) */
	  if (aeused)
	    update = build_comma(r, loc, update,
				 copy_lvalue(r, loc, ae->arg1));
	  *pp = CAST(node, update);
	  set_parent(pp, parent);
	  AST_SET_NEXT(update, next);
	}
    }
}

/* Note: global_rc_temps_needed and update_rc_temps_needed depend on
   prt_global_rc and prt_update_rc in unparse.c, and also
   on stats_global_rc* in stats.c */

static int global_rc_temps_needed(global_rc rcop)
{
  expression dest = rcop->arg1, src = rcop->arg2;
  binary brcop = CAST(binary, rcop);

  if (type_aggregate(dest->type) ||
      type_traditional(dest->type) ||
      type_sameregion(dest->type) ||
      type_parentptr(dest->type) ||
      known_traditional_write(brcop) || zero_expression(src))
    return 0;
  else if (!rc_stats_costs && rc_stats_detailed)
    return 3;
  else
    return 2;
}

static int update_rc_temps_needed(update_rc rcop)
{
  expression of = rcop->arg1;
  binary brcop = CAST(binary, rcop);

  if (type_aggregate(of->type) ||
      type_traditional(of->type) ||
      type_sameregion(of->type))
    return 0;
  else if (type_parentptr(of->type))
    {
#ifdef USE_CHECK_CHILD
      return 0;
#else
      if (rc_keepchecks ||
	  !(rc_nochecks ||
	    known_parentptr_write(rcop) || zero_expression(rcop->arg2)))
	return 2;
      else
	return 0;
#endif
    }
  else if (known_sameregion_write(rcop) || known_traditional_write(brcop) ||
	   zero_expression(rcop->arg2))
      return 2;
  else
      return 3;
}

static void declare_rcop_temps(region r, function_decl fd)
{
  generic_rc sameregion *rcops = fd->allrcops;
  generic_rc rcop;

  while ((rcop = *rcops++))
    {
      int ntemps, j;
      compound_stmt b;

      if (is_update_rc(rcop))
	ntemps = update_rc_temps_needed(CAST(update_rc, rcop));
      else
	ntemps = global_rc_temps_needed(CAST(global_rc, rcop));
      assert(ntemps <= MAXRCTEMPS);

      b = parent_block(CAST(node, rcop));
      for (j = 0; j < ntemps; j++)
	rcop->temps[j] = declare_region_temporary(r, b);
    }
}

static size_t copy_rcops(function_decl fd, generic_rc sameregion *rcops)
{
  node parentptr *gnodes = fd->postorder_nodes;
  int nnodes = fd->postorder_size, i, count = 0;

  /* The assign nodes we are interested in have been replaced by update_rc
     or global_rc, but the nodes list still contains the original assign
     nodes. These assign nodes point to the update_rc/global_rc node */
  for (i = 0; i < nnodes; i++)
    {
      node n = gnodes[i];
      assign ae;
      generic_rc rcop;

      if (!is_assign(n))
	continue;

      ae = CAST(assign, n);

      if (!(rcop = ae->rcop))
	continue;

      if (rcops)
	rcops[count] = rcop;
      count++;
    }

  return count;
}

static void save_rcops(region r, function_decl fd)
{
  size_t count = copy_rcops(fd, NULL);
  generic_rc sameregion *rcops = rarrayalloc(r, count + 1, generic_rc sameregion);

  copy_rcops(fd, rcops);
  rcops[count] = NULL;
  fd->allrcops = rcops;
}

expression rc_check_function_call(function_call fce)
{
  if (!rc_init_once())
    return CAST(expression, fce); /* not seen regions.h yet */

  if (is_identifier(fce->arg1))
    {
      identifier called = CAST(identifier, fce->arg1);

      if (called->ddecl == internal_rctypeof_decl)
	{
	  /* replace call internal_rctypeof(sizeof(foo)) by the appropriate
	     cleanup function for foo */
	  type allocated_type;
	  location loc = fce->location;


	  /* foo can be:
	     - array or function or variable-size: error
	     - contains no pointers: just return 0
	     - otherwise return appropriate rc_adjust fn */

	  if (!fce->args || fce->args->next
	      || !is_sizeof_type(fce->args))
	    error_with_location(loc, "don't call internal_rctypeof");
	  else
	    {
	      allocated_type = CAST(sizeof_type, fce->args)->asttype->type;

	      if (!type_size_cc(allocated_type))
		error_with_location(loc, "variable-sized types unsupported");
	      else if (type_function(allocated_type))
		error_with_location(loc, "cannot get type of a function");
	      else if (type_array(allocated_type))
		error_with_location(loc, "cannot get type of an array");
	      else if (type_contains_cross_pointers(allocated_type))
		{
		  data_declaration cleanup_fn =
		    lookup_rc_fn(parse_region, loc, current_function_decl,
				 allocated_type, FALSE, TRUE);
		  expression cleanup_id =
		    build_identifier(parse_region, loc, cleanup_fn);

		  return cleanup_id;
		}
	      else
		return build_zero(parse_region, loc);
	    }
	}
    }
  return CAST(expression, fce);
}

static void clear_fd(function_decl fd)
{
  long i;

  fd->vmap = NULL;
  fd->addressed_labels = NULL;

  for (i = 0; i < fd->cfg_size; i++)
    {
      node n = fd->cfg_nodes[i];

      n->edges_in = n->edges_out = NULL;
      n->live_in = n->live_out = NULL;
      n->inscope_vars = NULL;

      if (is_statement(n))
	{
	  statement s = CAST(statement, n);

	  s->continue_dest = s->break_dest = NULL;
	}
    }
  fd->cfg_nodes = fd->postorder_nodes = fd->preorder_nodes = NULL;
  fd->cfg_entry = NULL;
}

void rc_process(declaration the_program) deletes
{
  declaration d;
  char *perturb = getenv("RCPERTURB");

  if (perturb)
    {
      flag_perturb = 1;
      perturb_seed = atol(perturb);

      srand48(perturb_seed);
    }

  builtins_init();
  if (!rc_init_once())
    {
      error("regions.h not included");
      exit(2);
    }

  if (rc_chkopt_perfile)
    chkopt_libfn_init();

  /* XXX: Parse tree needs to be fixed ahead of time :-(
     See chkopt.c:has_interesting_pointers */
  scan_declaration (d, the_program)
    if (is_function_decl(d))
      {
	function_decl fd = CAST(function_decl, d);
	fix_parse_tree(parse_region, fd);
      }

  scan_declaration (d, the_program)
    if (is_function_decl(d))
      {
	function_decl fd = CAST(function_decl, d);
	data_declaration *vmap;
	region work = newsubregion(parse_region/*regionof(fd)*/);
	int i;
	bitset interesting;

	build_cfg(work, fd);
	cfg_build_postorder(work, fd);

	AST_set_parents(CAST(node, fd));

	collect_variables(work, fd);

	/* We optimise local pointers whose address is not taken */
	interesting = bclearb(new_bitset(work, fd->nlocals));
	vmap = fd->vmap;
	for (i = 0; i < fd->nlocals; i++)
	  {
	    data_declaration v = vmap[i];

	    if (v->kind == decl_variable && v->islocal && !v->addressed &&
		type_pointer(v->type))
	      {
		SET_BITB(interesting, i);
		v->interesting = TRUE;
	      }
	  }
	  
	compute_liveness(work, fd, interesting);

	if (rc_chkopt || rc_chkopt_perfile)
	  optimise_rc_checks(parse_region, fd, interesting);

	if (rc_locals && type_deletes(fd->ddecl->type))
	  switch (rc_algorithm)
	    {
	    case rc_optimal:
	      insert_optimal_rcops(work, fd, interesting);
	      break;
	    case rc_assignment:
	      insert_assignment_rcops(work, fd, interesting);
	      break;
	    case rc_functions:
	      insert_functions_rcops(work, fd, interesting);
	      break;
	    }

	/* Add RC code for locals whose address was taken */
	if (rc_globals) /* Counting these as globals for rc purposes */
	  add_local_rc(work, fd, interesting);

	generate_rcops(parse_region, fd);

	if (rc_globals)
	  add_other_rc(parse_region, fd);

	save_rcops(parse_region, fd);

	clear_fd(fd);
	deleteregion(work);
      }

  if (errorcount)
    return;

  if (rc_collect_statistics)
    {
      extern double largest_optrc;

      if (largest_optrc)
	fprintf(stderr, "optrc: %.2f\n", largest_optrc);
    }

  if (rc_chkopt_perfile)
    optimise_rc_checks_perfile(the_program);

  scan_declaration (d, the_program)
    if (is_function_decl(d))
      {
	function_decl fd = CAST(function_decl, d);
	declare_rcop_temps(parse_region, fd);
      }

  if (flag_parse_only)
    unparse(stdout, the_program);
  else
    exit(exec_cc1(the_program));
}
