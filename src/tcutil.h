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
#include <cycboot.h>
#include "absyn.h"
#include "tcenv.h"

namespace Tcutil {

using List;
using Absyn;
using Position;
using Tcenv;

extern `a impos(string_t fmt, ... inject parg_t<`r2> ap)
   __attribute__((format(printf,1,2), noreturn)) ;
extern void terr(seg_t, string_t fmt, ... inject parg_t<`r2> ap)
   __attribute__((format(printf,2,3))) ;
extern void warn(seg_t, string_t fmt, ... inject parg_t<`r2> ap)
   __attribute__((format(printf,2,3))) ;
extern void flush_warnings();
  // bextern void err_noloc(string_t);

// set by by Tcenv::tc_init, just to reduce allocation and calls to Set::empty.
extern Core::opt_t<Set::set_t<var_t>> empty_var_set;

// returns a deep copy of a type -- note that the evars will
// still share and if the identity is set on type variables,
// they will share, otherwise they won't.
extern type_t copy_type(type_t t);
// deep copy of an expression; uses copy_type above, so same rules apply
extern exp_t deep_copy_exp(exp_t);

// returns true if kind k1 is a sub-kind of k2
extern bool kind_leq(kind_t k1, kind_t k2);

// returns the type of a function declaration
extern type_t fd_type(fndecl_t fd); 
extern kind_t tvar_kind(tvar_t t,kind_t def);
extern kind_t typ_kind(type_t t);
extern bool kind_eq(kind_t k1, kind_t k2);
extern type_t compress(type_t t);
extern void unchecked_cast(tenv_t, exp_t, type_t, coercion_t);
extern bool coerce_arg(tenv_t, exp_t, type_t, bool@ alias_coercion); 
extern bool coerce_assign(tenv_t, exp_t, type_t);
extern bool coerce_to_bool(tenv_t, exp_t);
extern bool coerce_list(tenv_t, type_t, list_t<exp_t>);
extern bool coerce_uint_typ(tenv_t, exp_t);
extern bool coerce_sint_typ(tenv_t, exp_t);
extern bool coerce_use(tenv_t, exp_t, type_t);
extern bool coerceable(type_t); // true if numeric or character
// true when expressions of type t1 can be implicitly cast to t2
extern bool silent_castable(tenv_t,seg_t,type_t,type_t);
// true when expressions of type t1 can be cast to t2 -- call silent first
extern coercion_t castable(tenv_t,seg_t,type_t,type_t);

// used to alias the given expression, assumed to have non-Aliasable type
extern $(decl_t,exp_t) insert_alias(exp_t e, type_t e_typ);
// prints a warning when an alias coercion is inserted
extern bool warn_alias_coerce;

extern bool is_integral(exp_t);
extern bool is_numeric(exp_t);
extern bool is_function_type(type_t t);
extern bool is_pointer_type(type_t t);
extern bool is_zero(exp_t e);
extern bool is_pointer_or_boxed(type_t t,bool @is_dyneither_ptr);
extern type_t pointer_elt_type(type_t t);
extern type_t pointer_region(type_t t);

// useful kinds
extern struct Kind rk;
extern struct Kind ak;
extern struct Kind bk;
extern struct Kind mk;
extern struct Kind ek;
extern struct Kind ik;

extern struct Kind trk;
extern struct Kind tak;
extern struct Kind tbk;
extern struct Kind tmk;

extern struct Kind urk;
extern struct Kind uak;
extern struct Kind ubk;
extern struct Kind umk;

extern struct Core::Opt<kind_t> rko;
extern struct Core::Opt<kind_t> ako;
extern struct Core::Opt<kind_t> bko;
extern struct Core::Opt<kind_t> mko;
extern struct Core::Opt<kind_t> iko;
extern struct Core::Opt<kind_t> eko;

extern struct Core::Opt<kind_t> trko;
extern struct Core::Opt<kind_t> tako;
extern struct Core::Opt<kind_t> tbko;
extern struct Core::Opt<kind_t> tmko;

extern struct Core::Opt<kind_t> urko;
extern struct Core::Opt<kind_t> uako;
extern struct Core::Opt<kind_t> ubko;
extern struct Core::Opt<kind_t> umko;

extern Core::opt_t<kind_t> kind_to_opt(kind_t k);
extern kindbound_t kind_to_bound(kind_t k);
extern bool unify_kindbound(kindbound_t, kindbound_t);

extern $(tvar_t,kindbound_t) swap_kind(type_t t, kindbound_t kb);
  // for temporary kind refinement

// if t is a pointer type and e is 0, changes e to null and checks
// that t is nullable pointer type by unifying e's type with t.
extern bool zero_to_null(tenv_t, type_t t, exp_t e);

extern type_t max_arithmetic_type(type_t, type_t);

// explain_failure() explains why unify failed and at what particular types.
// Output goes to stderr.
extern void explain_failure();

extern bool unify(type_t, type_t);
  // linear order on types (needed for dictionary indexing)
extern int typecmp(type_t, type_t);
extern type_t substitute(list_t<$(tvar_t,type_t)@`H,`H>, type_t);
  // could also have a version with two regions, but doesn't seem useful
extern type_t rsubstitute(region_t<`r>,list_t<$(tvar_t,type_t)@`r,`r>,type_t);

// true when e1 is a sub-effect of e2
extern bool subset_effect(bool may_constrain_evars, type_t e1, type_t e2);

// returns true when rgn is in effect -- won't side-effect any evars when
// constrain is false.
extern bool region_in_effect(bool constrain, type_t r, type_t e);

extern type_t fndecl2typ(fndecl_t);

// generate an appropriate evar for a type variable -- used in
// instantiation.  The list of tvars is used to constrain the evar.
extern $(tvar_t,type_t)@   make_inst_var(list_t<tvar_t,`H>,tvar_t);
extern $(tvar_t,type_t)@`r r_make_inst_var($(list_t<tvar_t,`H>,region_t<`r>)@,tvar_t);
					   

// checks that a width given on a struct or union member is consistent
// with the type definition for the member.
extern void check_bitfield(seg_t loc, tenv_t te, type_t field_typ, 
                           exp_opt_t width, stringptr_t fn);

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
// * set pointers to declarations for StructType, DatatypeType
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
// are compatible with a set of bound type variables.  Note that this
// the side effect of constraining the kinds of the bound type variables.
// In addition, if allow_evars is true, then the evars in the type are
// unconstrained.  Otherwise, we set all region evars to the heap and
// all effect evars to the empty set, and signal an error for a free type
// evar.
extern void check_type(seg_t, tenv_t, list_t<tvar_t,`H> bound_tvars, kind_t k,
                       bool allow_evars, type_t);

extern void check_unique_vars(list_t<var_t,`r> vs, seg_t loc, string_t err_msg);
extern void check_unique_tvars(seg_t,list_t<tvar_t>);

// Sees if the unique region `U occurs in the type t
//  extern void check_no_unique_region(seg_t loc, tenv_t te, type_t t);

// Check that bounds are not zero -- constrain to 1 if necessary
extern void check_nonzero_bound(seg_t, conref_t<bounds_t>);
// Check that bounds are greater than i -- constrain to i+1 if necessary
extern void check_bound(seg_t, unsigned int i, conref_t<bounds_t>);

extern bool is_bound_one(conref_t<bounds_t> b);

extern bool equal_tqual(tqual_t tq1, tqual_t tq2);

extern list_t<$(aggrfield_t,`a)@`r,`r> 
resolve_aggregate_designators(region_t<`r>rgn, seg_t loc,
                              list_t<$(list_t<designator_t>,`a)@`r2,`r3> des, 
                              aggr_kind_t, // struct or union
                              list_t<aggrfield_t> fields);
// returns true if this is a t ? -- side effect unconstrained bounds
extern bool is_tagged_pointer_typ(type_t);
// like above, but puts element type in elt_typ_dest when returning true.
extern bool is_tagged_pointer_typ_elt(type_t t, type_t@`r elt_typ_dest);
// like above but checks to see if the pointer is zero-terminated
extern bool is_zero_pointer_typ_elt(type_t t, type_t@`r elt_typ_dest);

// like above but also works for arrays and returns dynamic pointer
//   stats
extern bool is_zero_ptr_type(type_t t, type_t @ptr_type, 
			     bool @is_dyneither, type_t @elt_type);

// is e1 of the form *ea or ea[eb] where ea is a zero-terminated pointer?
// If so, return true and set ea and eb appropriately (for *ea set eb to 0).
// Finally, if the pointer is fat, set is_dyneither.
extern bool is_zero_ptr_deref(exp_t e1, type_t @ptr_type, 
			      bool @is_dyneither, type_t @elt_type);
			      

// returns true if this a non-aliasable region, e.g. `U, `r::TR, etc.
extern bool is_noalias_region(type_t r, bool must_be_unique);

// returns true if this a non-aliasable pointer, e.g. *`U, *`r::TR, etc.
extern bool is_noalias_pointer(type_t t);

// returns true if this expression only deferences non-aliasable pointers
// and if the ultimate result is a noalias pointer or aggregate.  The
// region is used for allocating temporary stuff.
extern bool is_noalias_path(region_t<`r>, exp_t e);

// returns true if this expression is an aggregate that contains
// non-aliasable pointers or is itself a non-aliasable pointer
// The region is used for allocating temporary stuff
extern bool is_noalias_pointer_or_aggr(region_t<`temp>, type_t t);

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
extern $(list_t<tvar_t>,list_t<tvar_t>) split_effect(Core::opt_t<type_t> effect);

// Given an effect, express/mutate it to have only regions(`a), `r, and joins.
extern type_t normalize_effect(type_t e);

// Gensym a new type variable with kind bounded by k
extern tvar_t new_tvar(kindbound_t k);
// Get an identity for a type variable
extern int new_tvar_id();
// Add an identity to a type variable if it doesn't already have one
extern void add_tvar_identity(tvar_t);
extern void add_tvar_identities(list_t<tvar_t,`r>);
// true iff t has been generated by new_tvar (#xxx)
extern bool is_temp_tvar(tvar_t);
// in this case, we may want to rewrite it in `txxx (before printing it)
extern void rewrite_temp_tvar(tvar_t);

// are the lists of attributes the same?  doesn't require the same order
extern bool same_atts(attributes_t, attributes_t);

// returns true iff t contains only "bits" and no pointers or datatypes. 
// This is used to restrict the members of unions to ensure safety.
extern bool bits_only(type_t t);

// returns true iff e is an expression that can be evaluated at compile time
extern bool is_const_exp(tenv_t te, exp_t e);

// like Core::snd, but first argument is a tqual_t (not a BoxKind)
extern type_t snd_tqt($(tqual_t,type_t)@`r);

// does the type support "0" as a default value? -- used for toplevel
// variables where we don't have an initializer
extern bool supports_default(type_t);

// does the type support "0" as a value?  -- used for testing whether or
// not we can have a zero-terminated array.
extern bool admits_zero(type_t t);

// does the function type have the "noreturn" attribute?
extern bool is_noreturn(type_t);

// If t is a typedef, returns true if the typedef is const, and warns
// if the flag declared_const is true.  Otherwise returns declared_const.
extern bool extract_const_from_typedef(seg_t, bool declared_const, type_t);

// Transfer any function type attributes from the given list to the
// function type.  
extern attributes_t transfer_fn_type_atts(type_t t, attributes_t atts);

// If the given type is a pointer type, returns the region it points
// into.  Returns NULL if not a pointer type.
extern bool rgn_of_pointer(type_t t, type_t @rgn);

// issue a warning if the type is a typedef with non-empty qualifiers
void check_no_qual(seg_t loc, type_t t);

}
#endif
