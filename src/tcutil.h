#ifndef _TCUTIL_H_
#define _TCUTIL_H_

//#include "core.h"
#include "list.h"
#include "set.h"
#include "absyn.h"
#include "position.h"
#include "tcenv.h"


namespace Tcutil {

//using Core;
using List;
using Absyn;
using Position;
using Set;
using Tcenv;

extern xenum exn {TypeErr};
extern `a impos<`a>(string);
extern void terr(segment, string);
extern void warn(segment, string);
extern void flush_warnings();
extern void err_noloc(string);

// set by by Tcenv::tc_init, just to cut down on allocation and calls
// to Set::empty.
extern Core::Opt_t<Set<var>> empty_var_set;

// returns the type of a function declaration
extern typ fd_type(fndecl fd); 

extern typ compress(typ t);
extern conref<`a> compress_conref<`a>(conref<`a> x);
extern `a conref_val<`a>(conref<`a> x);
extern void unchecked_cast(tenv, exp, typ);
extern bool coerce_arg(tenv, exp, typ); 
extern bool coerce_assign(tenv, exp, typ);
extern bool coerce_to_bool(tenv, exp);
extern bool coerce_list(tenv, typ, list<exp>);
extern bool coerce_uint_typ(tenv, exp);
extern bool coerce_sint_typ(tenv, exp);
extern bool coerce_use(tenv, exp, typ);
extern bool coerceable(typ); // true if numeric or character
// true when expressions of type t1 can be implicitly cast to t2
extern bool silent_castable(tenv,segment,typ,typ);
// true when expressions of type t1 can be cast to t2 -- call silent first
extern bool castable(tenv,segment,typ,typ);

extern bool integral_promote(tenv, exp);
extern bool arithmetic_promote(tenv, exp);
extern bool comparison_promote(tenv, exp);
extern typ max_arithmetic_type(typ, typ);

extern bool unify(typ, typ);

extern typ substitute(list<$(tvar,typ)@>, typ);

extern typ fndecl2typ(fndecl);

extern exp default_initializer(tenv,typ,segment);

extern $(tvar,typ)@ make_inst_var(tvar);

// prints a warning when an expression contains an assignment
extern void check_contains_assign(exp);

// Check that the type is valid assuming that the free type variables are
// drawn from the given list.  As a side-effect, expand any typedefs.
extern void check_valid_type(segment,tenv,list<tvar>,typ);
// Special cased for function declarations
extern void check_fndecl_valid_type(segment,tenv,fndecl);

extern void check_unique_vars(list<var> vs, segment loc, string err_msg);
extern void check_unique_tvars(segment,list<tvar>);

// Check that bounds are not zero -- constrain to 1 if necessary
extern void check_nonzero_bound(segment, conref<bounds_t>);
// Check that bounds are greater than i -- constrain to i+1 if necessary
extern void check_bound(segment, unsigned int i, conref<bounds_t>);

extern bool equal_tqual(tqual tq1, tqual tq2);

extern list<$($(field_name,tqual,typ)@,`a)@>
resolve_struct_designators<`a>(seg_t loc,
			       list<$(list<designator>,`a)@> des, 
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

}
#endif
