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

extern typ compress(typ t);
extern conref<`a> compress_conref(conref<`a> x);
extern `a conref_val(conref<`a> x);
extern void unchecked_cast(tenv, exp, typ);
extern bool coerce_arg(tenv, exp, typ); 
extern bool coerce_assign(tenv, exp, typ);
extern bool coerce_to_bool(tenv, exp);
extern bool coerce_list(tenv, typ, list_t<exp>);
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

// Check that the type is valid assuming that the free type variables are
// drawn from the given list.  As a side-effect, expand any typedefs and
// calculate any default effects.
extern void check_valid_type(seg_t,tenv,list_t<tvar>,typ);
// Same as above, but allow abstract mem types (`a::M or an abstract struct)
extern void check_type(seg_t,tenv,list_t<tvar>,bool abs_type_okay,typ);
// Similar to the above except that (a) there are no bound type variables,
// and (b) for function types, we bind the free type variables.
extern void check_valid_toplevel_type(seg_t,tenv,typ);
// Special cased for function declarations
extern void check_fndecl_valid_type(seg_t,tenv,fndecl);
extern void check_valid_kinded_type(seg_t loc, tenv te, list_t<tvar> 
                                    bound_tvars, kind_t k, typ t);

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
