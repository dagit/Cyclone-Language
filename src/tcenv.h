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

#include <dict.h>
#include "absyn.h"
#include "rgnorder.h"

namespace Tcenv {
using Core;
using List;
using Absyn;

// This is thrown whenever we try to lookup a local variable but we're
// not within a function definition...
datatype exn {extern Env_error};

// Global environments -- what's declared in a global scope 
// Warning: ordinaries should really be abstract so we can ensure that any
// lookup sets the bool field to true!
// FIX: We should tree-shake the type declarations too!
extern struct Genv {
  Dict::dict_t<qvar_t,aggrdecl_t@>     aggrdecls;
  Dict::dict_t<qvar_t,datatypedecl_t@> datatypedecls;
  Dict::dict_t<qvar_t,enumdecl_t@>     enumdecls;
  // no indirection b/c no redeclaration
  Dict::dict_t<qvar_t,typedefdecl_t>   typedefs; 
  // bool for tree-shaking, binding_t really only for interface.cyc (FIX)
  Dict::dict_t<qvar_t,$(binding_t,bool)@> ordinaries;
};
typedef struct Genv@ genv_t;

// Local function environments
extern struct Fenv;
typedef struct Fenv@ fenv_t;

// Type environments
extern struct Tenv {
  list_t<var_t> ns; // current namespace
  genv_t        ae; // absolute environment
  struct Fenv * le; // local environment
  bool allow_valueof;   // controls whether we allow valueof(T) in an expr
  bool in_extern_c_include;
};
typedef struct Tenv@ tenv_t; 

extern `a env_err(string_t msg) __attribute__((noreturn));

extern tenv_t tc_init();
extern fenv_t new_fenv(seg_t,fndecl_t);
extern fenv_t nested_fenv(seg_t,fenv_t old_fenv, fndecl_t new_fn);
// bogus fenv is used for checking requires/ensures clauses
extern fenv_t bogus_fenv(type_t ret_type,
			 list_t<$(var_opt_t,tqual_t,type_t)@> args);

// when is_use is true, we are looking up a free occurrence of the variable
extern binding_t lookup_ordinary_global(tenv_t,seg_t,qvar_t,bool is_use);
extern aggrdecl_t@       lookup_aggrdecl(tenv_t,seg_t,qvar_t);
extern datatypedecl_t@     lookup_datatypedecl(tenv_t,seg_t,qvar_t);
extern datatypedecl_t@*`r  lookup_xdatatypedecl(region_t<`r>,tenv_t,seg_t,qvar_t);
extern enumdecl_t@       lookup_enumdecl(tenv_t,seg_t,qvar_t);
extern typedefdecl_t     lookup_typedefdecl(tenv_t,seg_t,qvar_t);

extern tenv_t allow_valueof(tenv_t);
extern tenv_t enter_extern_c_include(tenv_t);

enum NewStatus { NoneNew, InNew, InNewAggr };  
extern tenv_t set_new_status(enum NewStatus, tenv_t);
extern enum NewStatus new_status(tenv_t);

extern bool abstract_val_ok(tenv_t);
extern tenv_t enter_abstract_val_ok(tenv_t);
extern tenv_t clear_abstract_val_ok(tenv_t);

extern type_t  return_typ(tenv_t);

extern tenv_t copy_tenv(tenv_t);

extern list_t<tvar_t> lookup_type_vars(tenv_t);
extern opt_t<list_t<tvar_t>> lookup_opt_type_vars(tenv_t);
extern tenv_t add_type_vars(seg_t,tenv_t,list_t<tvar_t>);

extern tenv_t set_fallthru(tenv_t te, 
			   list_t<tvar_t,`H> new_tvs, 
			   list_t<vardecl_t> vds,
			   switch_clause_t clause);
extern tenv_t clear_fallthru(tenv_t);

// used to record whether we're type-checking an expression that is doing
// a read.  Some expressions (e.g., x++) are both reading and writing so
// this always returns true in those contexts.  We even return true for
// &x since the result may later be read.  We need this info to determine
// whether or not (conservatively) we're reading a union member.  If
// so, we must enforce that the member is bits-only.
extern tenv_t enter_notreadctxt(tenv_t);
extern tenv_t clear_notreadctxt(tenv_t);
extern bool in_notreadctxt(tenv_t);

//used to track whether we're in a left-hand-side or right-hand-side context
extern tenv_t enter_lhs(tenv_t);
extern tenv_t clear_lhs(tenv_t);
extern bool in_lhs(tenv_t te);

extern tenv_t enter_stmt_exp(tenv_t);
extern bool in_stmt_exp(tenv_t);

// assigns through its last arg
extern $(switch_clause_t,list_t<tvar_t>,list_t<type_t>)const* const
process_fallthru(tenv_t,stmt_t,switch_clause_t *@);
                                     
extern tenv_t new_block(seg_t,tenv_t);
extern tenv_t new_named_block(seg_t,tenv_t,tvar_t);
extern tenv_t new_outlives_constraints(tenv_t,list_t<$(type_t,type_t)@>, seg_t);
extern tenv_t add_region_equality(tenv_t te, type_t r1, type_t r2, 
				  $(tvar_t,kindbound_t) * @oldtv,
				  seg_t loc);

extern type_t curr_rgn(tenv_t);

extern tenv_t add_region(tenv_t, type_t, bool opened);
// Check that the region is in the current capability
extern void check_rgn_accessible(tenv_t,seg_t,type_t rgn);
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
