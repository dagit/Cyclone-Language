#ifndef TCUTIL_H
#define TCUTIL_H

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

// returns the type of a function declaration
extern typ fd_type(fndecl fd); 

extern typ compress(typ t);
extern conref<`a> compress_conref<`a>(conref<`a> x);
extern void unchecked_cast(tenv, exp, typ);
extern bool coerce_arg(tenv, exp, typ); 
extern bool coerce_assign(tenv, exp, typ);
extern bool coerce_to_bool(tenv, exp);
extern bool coerce_list(tenv, typ, list<exp>);
extern bool coerce_uint_t(tenv, exp);
extern bool coerce_use(tenv, exp, typ);
extern bool coerceable(typ); // true if numeric or character
// true when expressions of type t1 can be cast to t2
extern bool castable(tenv,segment,typ,typ);

extern bool integral_promote(tenv, exp);
extern bool arithmetic_promote(tenv, exp);
extern bool comparison_promote(tenv, exp);
extern typ max_arithmetic_type(typ, typ);

/*
extern void check_valid_lhs(tenv, exp);

*/

extern bool unify(typ, typ);
extern xenum exn {Unify};
// Raises Unify 
extern void unify_it(typ, typ);
extern typ substitute(list<$(var,typ)@>, typ);

extern typ fndecl2typ(fndecl);

extern exp default_initializer(tenv,typ,segment);

extern $(var,typ)@ make_inst_var(var);

// Check that the type is valid assuming that the free type variables are
// drawn from the given list.  As a side-effect, expand any typedefs.
extern void check_valid_type(segment,tenv,list<tvar>,typ);
// Special cased for function declarations
extern void check_fndecl_valid_type(segment,tenv,fndecl);

extern Set<var> repeated_vars(list<var>);
extern void check_unique_tvars(segment,list<tvar>);

extern xenum exn {MissingField};
extern $($(field_name,tqual,typ)@,list<$(field_name,tqual,typ)@>)@
  remove_field(field_name,list<$(field_name,tqual,typ)@>);
extern bool equal_tqual(tqual tq1, tqual tq2);
}
#endif
