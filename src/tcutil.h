#ifndef _TCUTIL_H_
#define _TCUTIL_H_

#include "list.h"
#include "set.h"
#include "absyn.h"
#include "position.h"
#include "tcenv.h"

namespace Tcutil {

using List;
using Absyn;
using Position;
using Set;
using Tcenv;

extern xenum exn {TypeErr};
extern `a impos(string);
extern void terr(seg_t, string);
extern void warn(seg_t, string);
extern void flush_warnings();
extern void err_noloc(string);

// set by by Tcenv::tc_init, just to cut down on allocation and calls
// to Set::empty.
extern Core::opt_t<set_t<var>> empty_var_set;

// returns the type of a function declaration
extern typ fd_type(fndecl fd); 
extern kind_t typ_kind(typ t);
extern typ compress(typ t);
extern void unchecked_cast(tenv, exp, typ);
extern bool coerce_arg(tenv, exp, typ); 
extern bool coerce_assign(tenv, exp, typ);
extern bool coerce_to_bool(tenv, exp);
extern bool coerce_list(tenv, typ, glist_t<exp,`r>);
extern bool coerce_uint_typ(tenv, exp);
extern bool coerce_sint_typ(tenv, exp);
extern bool coerce_use(tenv, exp, typ);
extern bool coerceable(typ); // true if numeric or character
// true when expressions of type t1 can be implicitly cast to t2
extern bool silent_castable(tenv,seg_t,typ,typ);
// true when expressions of type t1 can be cast to t2 -- call silent first
extern bool castable(tenv,seg_t,typ,typ);

extern bool integral_promote(tenv, exp);
extern bool arithmetic_promote(tenv, exp);
extern bool comparison_promote(tenv, exp);
extern typ max_arithmetic_type(typ, typ);

extern bool unify(typ, typ);
extern typ substitute(list_t<$(tvar,typ)@>, typ);

// true when e1 is a sub-effect of e2 -- might cause side effects
// to constrain evars...
extern bool subset_effect(bool set_to_empty, typ e1, typ e2);

// true when rgn is in effect -- won't side-effect any evars when
// constrain is false.
extern bool region_in_effect(bool constrain, typ r, typ e);

extern typ fndecl2typ(fndecl);

extern exp default_initializer(tenv,typ,seg_t);

extern $(tvar,typ)@ make_inst_var(tvar);

// prints a warning when an expression contains an assignment
extern void check_contains_assign(exp);


// Checks that a type is well-formed and lives in kind k.
// This adds to the input list of free type variables any new free 
// variables that arise.  This allows us to calculate free variables 
// of types in a bottom-up fashion and then check that they are all bound 
// at the top-level.
//
// This also performs the following side-effects which most of the 
// rest of the compiler rightfully assumes have occurred:
// * expand typedefs
// * set pointers to declarations for StructType, EnumType, and XenumType
// * change relative type names to absolute type names
// * set the kind field of type variables: we use the expected kind
//   initially, but if later constraints force a more constrained kind,
//   then we move to the more constrained kind.  To achieve this, we very
//   carefully share all of the free occurrences of the type variable 
//   in the type data structure.
// * add default effects for function types -- the default effect
//   causes a fresh EffKind type variable e to be generated, and
//   consists of e and any free effect or region variables within
//   the function type.  
extern list_t<tvar> check_valid_type(seg_t,tenv,list_t<tvar>,kind_t k,typ);
// Similar to the above except that (a) there are no bound type variables,
// (b) for function types, we bind the free type variables, (c) the expected
// kind defaults to MemKind.
extern void check_valid_toplevel_type(seg_t,tenv,typ);
// Special cased for function declarations
extern void check_fndecl_valid_type(seg_t,tenv,fndecl);
// Same as check_valid_type but ensures that the resulting free variables
// are compatible with a set of bound type variables.  Note that this has
// the side effect of constraining the kinds of the bound type variables.
extern void check_type(seg_t, tenv, list_t<tvar> bound_tvars, kind_t k,typ);

extern void check_unique_vars(list_t<var> vs, seg_t loc, string err_msg);
extern void check_unique_tvars(seg_t,list_t<tvar>);

// Check that bounds are not zero -- constrain to 1 if necessary
extern void check_nonzero_bound(seg_t, conref<bounds_t>);
// Check that bounds are greater than i -- constrain to i+1 if necessary
extern void check_bound(seg_t, unsigned int i, conref<bounds_t>);

extern bool equal_tqual(tqual tq1, tqual tq2);

extern list_t<$($(field_name,tqual,typ)@,`a)@>
resolve_struct_designators(seg_t loc,
                           list_t<$(list_t<designator>,`a)@> des, 
                           structdecl sd);
// returns true if this is a t ? -- side effect unconstrained bounds
extern bool is_tagged_pointer_typ(typ);

// if exp is of array type, cast it to an appropriate pointer type --
// assumes the expression has already been type-checked and that t is
// the type of the expression e
extern typ array_to_ptr(tenv, typ t, exp e);

// Ensure e is an lvalue or function designator -- return whether
// or not &e is const and what region e is in.
extern $(bool,typ) addressof_props(tenv te, exp e);

// Given a function's effect (or capability) split it into a
// list of effect type variables, and a list of region type
// variables respectively.
extern $(list_t<tvar>, list_t<tvar>) split_effect(Core::opt_t<typ> effect);

// Gensym a new type variable of kind k
extern tvar new_tvar(kind_t k);
}
#endif
