/* Type environments.
   Copyright (C) 2001 Greg Morrisett, AT&T
   This file is part of the Cyclone compiler.

   The Cyclone compiler is free software; you can redistribute it
   and/or modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   The Cyclone compiler is distributed in the hope that it will be
   useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with the Cyclone compiler; see the file COPYING. If not,
   write to the Free Software Foundation, Inc., 59 Temple Place -
   Suite 330, Boston, MA 02111-1307, USA. */
#ifndef _TCENV_H_
#define _TCENV_H_

#include <core.h>
#include <list.h>
#include <set.h>
#include <dict.h>
#include "absyn.h"
#include "rgnorder.h"

namespace Tcenv {
using Core;
using List;
using Set;
using Dict;
using Absyn;
using Position;

// This is thrown whenever we try to lookup a local variable but we're
// not within a function definition...
datatype exn {extern Env_error};

extern struct CList<`a,`r::R> { `a hd; struct CList<`a,`r> const *`r const tl;};
typedef struct CList<`a,`r> const *`r mclist_t<`a,`r>;
typedef mclist_t<`a,`r> const clist_t<`a,`r>;

// Used to tell what an ordinary identifer refers to 
extern datatype Resolved {
  VarRes(binding_t); // includes unresolved variant
  AggrRes(aggrdecl_t);
  DatatypeRes(datatypedecl_t,datatypefield_t);
  EnumRes(enumdecl_t,enumfield_t);
  AnonEnumRes(type_t,enumfield_t);
};
typedef datatype Resolved @`r resolved_t<`r>;

// Global environments -- what's declared in a global scope 
// Warning: ordinaries should really be abstract so we can ensure that any
// lookup sets the bool field to true!
// FIX: We should tree-shake the type declarations too!
extern struct Genv<`g::R> {
  region_t<`g> grgn;
  dict_t<qvar_t,aggrdecl_t@,`g>     aggrdecls;
  dict_t<qvar_t,datatypedecl_t@,`g> datatypedecls;
  dict_t<qvar_t,enumdecl_t@,`g>     enumdecls;
  // no indirection b/c no redeclaration
  dict_t<qvar_t,typedefdecl_t,`g>   typedefs; 
  // bool for tree-shaking
  dict_t<qvar_t,$(resolved_t,bool)@`g,`g> ordinaries;
};
typedef struct Genv<`r> @`r genv_t<`r>;

// Local function environments
extern struct Fenv<`l::R>;
typedef struct Fenv<`l> @`l fenv_t<`l>; 

// Type environments -- `g is the region for global information
// and `l is the region for local information
extern struct Tenv<`g::R,`l::R> {
  list_t<var_t>       ns; // current namespace
  genv_t<`g>          ae; // absolute environment
  struct Fenv<`l> *`l le; // local environment
  bool allow_valueof;   // controls whether we allow valueof(T) in an expr
  bool in_extern_c_include;
};
typedef struct Tenv<`g,`l> @`l tenv_t<`g,`l>; 

extern `a env_err(string_t msg) __attribute__((noreturn));

extern region_t<`r> get_fnrgn(tenv_t<`g,`r>);

// The COARSE_REGIONS flag, when on, uses many fewer, coarse-grained
// regions.  Basically, we use one region for each function during
// each of the type-checking, flow analysis, and translation stages.
//#define COARSE_REGIONS
#ifdef COARSE_REGIONS
#define TEMP_RGN(te,x) let x = Tcenv::get_fnrgn(te);
#else
#define TEMP_RGN(te,x) region x;
#endif

extern tenv_t<`r,`r> tc_init(region_t<`r>);
extern fenv_t<`r> new_fenv(region_t<`r>,seg_t,fndecl_t);
extern fenv_t<`r> nested_fenv(seg_t,fenv_t<`r> old_fenv, fndecl_t new_fn);
// bogus fenv is used for checking requires/ensures clauses
extern fenv_t<`r> bogus_fenv(region_t<`r>, type_t ret_type, 
                             list_t<$(var_opt_t,tqual_t,type_t)@> args);

// when is_use is true, we are looking up a free occurrence of the variable
extern resolved_t<`r>    lookup_ordinary(region_t<`r>,tenv_t,seg_t,qvar_t,bool is_use);
extern aggrdecl_t@       lookup_aggrdecl(tenv_t,seg_t,qvar_t);
extern datatypedecl_t@     lookup_datatypedecl(tenv_t,seg_t,qvar_t);
extern datatypedecl_t@*`r  lookup_xdatatypedecl(region_t<`r>,tenv_t,seg_t,qvar_t);
extern enumdecl_t@       lookup_enumdecl(tenv_t,seg_t,qvar_t);
extern typedefdecl_t     lookup_typedefdecl(tenv_t,seg_t,qvar_t);

extern tenv_t<`g,`r2> allow_valueof(region_t<`r2>,tenv_t<`g,`r>: {`r} > `r2);
extern tenv_t<`g,`r2> enter_extern_c_include(region_t<`r2>,tenv_t<`g,`r>: {`r} > `r2);

enum NewStatus { NoneNew, InNew, InNewAggr };  
extern tenv_t<`g,`r2> set_new_status(region_t<`r2>,enum NewStatus status, 
				     tenv_t<`g,`r>: {`r} > `r2);
extern enum NewStatus new_status(tenv_t te);

extern bool abstract_val_ok(tenv_t te);
extern tenv_t<`g,`r> enter_abstract_val_ok(region_t<`r> r,
					   tenv_t<`g,`r2> te : {`r2}>`r);
extern tenv_t<`g,`r> clear_abstract_val_ok(region_t<`r> r,
					   tenv_t<`g,`r2> te : {`r2}>`r);

extern type_t  return_typ(tenv_t);

extern tenv_t<`g,`r> copy_tenv(region_t<`r>,tenv_t<`g,`r2> : {`r2} > `r);
extern tenv_t<`g,`r> add_local_var(region_t<`r>,seg_t,tenv_t<`g,`r2>,vardecl_t : {`r2} > `r);
extern tenv_t<`g,`r> add_pat_var  (region_t<`r>,seg_t,tenv_t<`g,`r2>,vardecl_t : {`r2} > `r);

extern list_t<tvar_t> lookup_type_vars(tenv_t<`r>);
extern opt_t<list_t<tvar_t>> lookup_opt_type_vars(tenv_t te);
extern tenv_t<`g,`r> add_type_vars(region_t<`r>,seg_t,tenv_t<`g,`r2>,list_t<tvar_t> : {`r2} > `r);

extern tenv_t<`g,`r> set_fallthru(region_t<`r>,
                                  tenv_t<`g,`r2> te, 
                                  list_t<tvar_t,`H> new_tvs, 
                                  list_t<vardecl_t> vds,
                                  switch_clause_t clause : {`r2} > `r);
extern tenv_t<`g,`r> clear_fallthru(region_t<`r>,tenv_t<`g,`r2> : {`r2} > `r);

// used to record whether we're type-checking an expression that is doing
// a read.  Some expressions (e.g., x++) are both reading and writing so
// this always returns true in those contexts.  We even return true for
// &x since the result may later be read.  We need this info to determine
// whether or not (conservatively) we're reading a union member.  If
// so, we must enforce that the member is bits-only.
extern tenv_t<`g,`r> enter_notreadctxt(region_t<`r>,tenv_t<`g,`r2> te : {`r2}>`r);
extern tenv_t<`g,`r> clear_notreadctxt(region_t<`r>,tenv_t<`g,`r2> te : {`r2}>`r);
extern bool in_notreadctxt(tenv_t te);

//used to track whether we're in a left-hand-side or right-hand-side context
extern tenv_t<`g,`r> enter_lhs(region_t<`r>,tenv_t<`g,`r2> te : {`r2}>`r);
extern tenv_t<`g,`r> clear_lhs(region_t<`r>,tenv_t<`g,`r2> te : {`r2}>`r);
extern bool in_lhs(tenv_t te);

// assigns through its last arg
extern $(switch_clause_t,list_t<tvar_t>,clist_t<type_t,`r>)const*`r const
process_fallthru(tenv_t<`g,`r>,stmt_t,switch_clause_t *@);
                                     
extern tenv_t<`g,`r> new_block(region_t<`r>,seg_t,tenv_t<`g,`r2>:{`r2}>`r);
extern tenv_t<`g,`r> new_named_block(region_t<`r>,seg_t,tenv_t<`g,`r2>,tvar_t name:{`r2}>`r);
extern tenv_t<`g,`r> new_outlives_constraints(region_t<`r>, tenv_t<`g,`r2> te, list_t<$(type_t,type_t)@> cs, seg_t loc:{`r2}>`r);
extern tenv_t<`g,`r> add_region_equality(region_t<`r> r, tenv_t<`g,`r2> te, 
					 type_t r1, type_t r2, 
					 $(tvar_t,kindbound_t) *`r @oldtv,
					 seg_t loc: {`r2} > `r);

extern type_t curr_rgn(tenv_t);

extern tenv_t<`g,`r> add_region(region_t<`r>, tenv_t<`g,`r2> te, type_t r, bool resetable, bool opened : {`r2} > `r);
// Check that the region is in the current capability
extern void check_rgn_accessible(tenv_t,seg_t,type_t rgn);
// Check that the region is in the current capability and is resetable
extern void check_rgn_resetable(tenv_t,seg_t,type_t rgn);
// Check that an effect is a sub-effect of the current capability, may delay
extern void check_effect_accessible(tenv_t te, seg_t loc, type_t eff);
// Returns true when region r1 outlives region r2 -- assumes r1 <> r2
extern bool region_outlives(tenv_t, type_t r1, type_t r2);
// Checks that for each pair of regions (r1,r2), r1 outlives r2 under
// the current partial order on region lifetimes in the environment, may delay
extern void check_rgn_partial_order(tenv_t te, seg_t loc, 
                                    list_t<$(type_t,type_t)@`H,`H> po);

extern void check_delayed_effects(tenv_t te);
extern void check_delayed_constraints(tenv_t te);

}
#endif
