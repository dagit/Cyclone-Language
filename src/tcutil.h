/* Utility functions for type checking.
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


#ifndef _TCUTIL_H_
#define _TCUTIL_H_

#include <list.h>
#include <set.h>
#include <position.h>
#include <stdio.h>
#include "absyn.h"
#include "tcenv.h"

namespace Tcutil {

using List;
using Absyn;
using Position;
using Tcenv;
using Stdio;

extern xtunion exn {extern TypeErr};
extern `a impos(string_t fmt, ...`r1 inject parg_t<`r2> ap)
  __attribute__((format(printf,1,2)));
extern void terr(seg_t, string_t fmt, ...`r1 inject parg_t<`r2> ap)
  __attribute__((format(printf,2,3)));
extern void warn(seg_t, string_t fmt, ...`r1 inject parg_t<`r2> ap)
  __attribute__((format(printf,2,3)));
extern void flush_warnings();
extern void err_noloc(string_t);

// set by by Tcenv::tc_init, just to cut down on allocation and calls
// to Set::empty.
extern Core::opt_t<Set::set_t<var_t>> empty_var_set;

// returns a deep copy of a type -- note that the evars will
// still share and if the identity is set on type variables,
// they will share, otherwise they won't.
extern type_t copy_type(type_t t);

// returns the type of a function declaration
extern type_t fd_type(fndecl_t fd); 
extern kind_t tvar_kind(tvar_t t);
extern kind_t typ_kind(type_t t);
extern type_t compress(type_t t);
extern void unchecked_cast(tenv_t, exp_t, type_t);
extern bool coerce_arg(tenv_t, exp_t, type_t); 
extern bool coerce_assign(tenv_t, exp_t, type_t);
extern bool coerce_to_bool(tenv_t, exp_t);
extern bool coerce_list(tenv_t, type_t, glist_t<exp_t,`r>);
extern bool coerce_uint_typ(tenv_t, exp_t);
extern bool coerce_sint_typ(tenv_t, exp_t);
extern bool coerce_use(tenv_t, exp_t, type_t);
extern bool coerceable(type_t); // true if numeric or character
// true when expressions of type t1 can be implicitly cast to t2
extern bool silent_castable(tenv_t,seg_t,type_t,type_t);
// true when expressions of type t1 can be cast to t2 -- call silent first
extern bool castable(tenv_t,seg_t,type_t,type_t);

extern bool is_integral(exp_t);
extern bool is_numeric(exp_t);
extern bool is_function_type(type_t t);

extern type_t max_arithmetic_type(type_t, type_t);

extern bool unify(type_t, type_t);
extern type_t substitute(list_t<$(tvar_t,type_t)@>, type_t);
  // could also have a version with two regions, but doesn't seem useful
extern type_t rsubstitute(region_t<`r>,glist_t<$(tvar_t,type_t)@`r,`r>,type_t);

// true when e1 is a sub-effect of e2 -- might cause side effects
// to constrain evars...
extern bool subset_effect(bool set_to_empty, type_t e1, type_t e2);

// true when rgn is in effect -- won't side-effect any evars when
// constrain is false.
extern bool region_in_effect(bool constrain, type_t r, type_t e);

extern type_t fndecl2typ(fndecl_t);

extern exp_t default_initializer(tenv_t,type_t,seg_t);

// generate an appropriate evar for a type variable -- used in
// instantiation.  The list of tvars is used to constrain the
// evar.
extern $(tvar_t,type_t)@   make_inst_var(list_t<tvar_t>,tvar_t);
extern $(tvar_t,type_t)@`r 
  r_make_inst_var($(list_t<tvar_t>,region_t<`r>)@`r2,tvar_t);
                  

// prints a warning when an expression contains an assignment
extern void check_contains_assign(exp_t);


// Checks that a type is well-formed and lives in kind k.  The input
// list of type variables is used to constrain the kinds and identities
// of the free type variables in the type.  Returns the list of free
// type variables in the type.
//
// This also performs the following side-effects which most of the 
// rest of the compiler rightfully assumes have occurred:
// * expand typedefs
// * set pointers to declarations for StructType, TunionType
// * change relative type names to absolute type names
// * set the kind field of type variables: we use the expected kind
//   initially, but if later constraints force a more constrained kind,
//   then we move to the more constrained kind.  
// * add default effects for function types -- the default effect
//   causes a fresh EffKind type variable e to be generated, and
//   consists of e and any free effect or region variables within
//   the function type.
// * ensures that for any free evar in the type, it can only be 
//   constrained with types whose free variables are contained in the
//   set of free variables returned.
  // extern list_t<tvar_t> check_valid_type(seg_t,tenv_t,list_t<tvar_t>,kind_t k,type_t);
// Similar to the above except that (a) there are no bound type variables,
// (b) for function types, we bind the free type variables, (c) the expected
// kind defaults to MemKind.
extern void check_valid_toplevel_type(seg_t,tenv_t,type_t);
// Special cased for function declarations
extern void check_fndecl_valid_type(seg_t,tenv_t,fndecl_t);
// Same as check_valid_type but ensures that the resulting free variables
// are compatible with a set of bound type variables.  Note that this has
// the side effect of constraining the kinds of the bound type variables.
extern void check_type(seg_t, tenv_t, list_t<tvar_t> bound_tvars, kind_t k,type_t);

extern void check_unique_vars(glist_t<var_t,`r> vs, seg_t loc, string_t err_msg);
extern void check_unique_tvars(seg_t,list_t<tvar_t>);

// Check that bounds are not zero -- constrain to 1 if necessary
extern void check_nonzero_bound(seg_t, conref_t<bounds_t>);
// Check that bounds are greater than i -- constrain to i+1 if necessary
extern void check_bound(seg_t, unsigned int i, conref_t<bounds_t>);

extern bool equal_tqual(tqual_t tq1, tqual_t tq2);

extern glist_t<$(structfield_t,`a)@`r,`r> 
resolve_struct_designators(region_t<`r>rgn, seg_t loc,
                           glist_t<$(list_t<designator_t>,`a)@`r2,`r3> des, 
                           list_t<structfield_t> fields);
// returns true if this is a t ? -- side effect unconstrained bounds
extern bool is_tagged_pointer_typ(type_t);
// like above, but puts element type in elt_typ_dest when returning true.
extern bool is_tagged_pointer_typ_elt(type_t t, type_t@`r elt_typ_dest);

// if exp is of array type, cast it to an appropriate pointer type --
// assumes the expression has already been type-checked and that t is
// the type of the expression e
extern type_t array_to_ptr(tenv_t, type_t t, exp_t e);

// Ensure e is an lvalue or function designator -- return whether
// or not &e is const and what region e is in.
extern $(bool,type_t) addressof_props(tenv_t te, exp_t e);

// Given a function's effect (or capability) split it into a
// list of effect type variables, and a list of region type
// variables respectively.
extern $(list_t<tvar_t>, list_t<tvar_t>) split_effect(Core::opt_t<type_t> effect);

// Gensym a new type variable of kind k
extern tvar_t new_tvar(kind_t k);
// Get an identity for a type variable
extern int *new_tvar_id();
// Add an identity to a type variable if it doesn't already have one
extern void add_tvar_identity(tvar_t);
extern void add_tvar_identities(glist_t<tvar_t,`r>);
// true iff t has been generated by new_tvar (#xxx)
extern bool is_temp_tvar(tvar_t);
// in this case, we may want to rewrite it in `txxx (before printing it)
extern void rewrite_temp_tvar(tvar_t);

// are the lists of attributes the same?  doesn't require the same order
extern bool same_atts(attributes_t, attributes_t);

// returns true iff t contains only "bits" and no pointers or tunions. 
// This is used to restrict the members of unions to ensure safety.
extern bool bits_only(type_t t);

// returns true iff e is an expression that can be evaluated at compile time
extern bool is_const_exp(tenv_t te, exp_t e);

// like Core::snd, but first argument is a tqual_t (not a BoxKind)
extern type_t snd_tqt($(tqual_t,type_t)@`r);

// does the type support "0" as a default value?
extern bool supports_default(type_t);
}

#endif
