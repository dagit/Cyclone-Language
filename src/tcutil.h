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

extern xtunion exn {TypeErr};
extern `a impos(string);
extern void terr(seg_t, string);
extern void warn(seg_t, string);
extern void flush_warnings();
extern void err_noloc(string);

// set by by Tcenv::tc_init, just to cut down on allocation and calls
// to Set::empty.
extern Core::opt_t<set_t<var_t>> empty_var_set;

// returns the type of a function declaration
extern type_t fd_type(fndecl_t fd); 
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
extern type_t max_arithmetic_type(type_t, type_t);

extern bool unify(type_t, type_t);
extern type_t substitute(list_t<$(tvar_t,type_t)@>, type_t);

// true when e1 is a sub-effect of e2 -- might cause side effects
// to constrain evars...
extern bool subset_effect(bool set_to_empty, type_t e1, type_t e2);

// true when rgn is in effect -- won't side-effect any evars when
// constrain is false.
extern bool region_in_effect(bool constrain, type_t r, type_t e);

extern type_t fndecl2typ(fndecl_t);

extern exp_t default_initializer(tenv_t,type_t,seg_t);

extern $(tvar_t,type_t)@ make_inst_var(tvar_t);

// prints a warning when an expression contains an assignment
extern void check_contains_assign(exp_t);


// Checks that a type is well-formed and lives in kind k.
// This adds to the input list of free type variables any new free 
// variables that arise.  This allows us to calculate free variables 
// of types in a bottom-up fashion and then check that they are all bound 
// at the top-level.
//
// This also performs the following side-effects which most of the 
// rest of the compiler rightfully assumes have occurred:
// * expand typedefs
// * set pointers to declarations for StructType, TunionType, and XtunionType
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
extern list_t<tvar_t> check_valid_type(seg_t,tenv_t,list_t<tvar_t>,kind_t k,type_t);
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

extern void check_unique_vars(list_t<var_t> vs, seg_t loc, string err_msg);
extern void check_unique_tvars(seg_t,list_t<tvar_t>);

// Check that bounds are not zero -- constrain to 1 if necessary
extern void check_nonzero_bound(seg_t, conref_t<bounds_t>);
// Check that bounds are greater than i -- constrain to i+1 if necessary
extern void check_bound(seg_t, unsigned int i, conref_t<bounds_t>);

extern bool equal_tqual(tqual_t tq1, tqual_t tq2);

extern list_t<$(structfield_t,`a)@> 
resolve_struct_designators(seg_t loc,
                           list_t<$(list_t<designator_t>,`a)@> des, 
                           structdecl_t sd);
// returns true if this is a t ? -- side effect unconstrained bounds
extern bool is_tagged_pointer_typ(type_t);

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
}
#endif
