/* Computing Strongest Post-conditions
   Copyright (C) 2003 Greg Morrisett
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

/* this file contains the definition of assertions, the terms used in
   assertions as well as utilities on them (such as comparison, debugging
   printing etc.). */
#ifndef _ASSNDEF_H_
#define _ASSNDEF_H_

#include "absyn.h"
#include "absynpp.h"
#include "warn.h"
#include "evexp.h"
#include "tcutil.h"
#include <list.h>
#include <string.h>
#include <hashtable.h>

namespace AssnDef{
  using List;
  typedef Absyn::type_t type_t;
  typedef Absyn::type_opt_t type_opt_t;
  typedef Absyn::vardecl_t vardecl_t;
  typedef Absyn::vardecl_opt_t vardecl_opt_t;
  typedef Absyn::exp_t exp_t;
  typedef Absyn::decl_t decl_t;
  typedef Absyn::stmt_t stmt_t;
  typedef Absyn::stmt_opt_t stmt_opt_t;
  typedef Absyn::switch_clause_t switch_clause_t;
  typedef Absyn::var_opt_t var_opt_t;
  typedef Absyn::tqual_t tqual_t;
  typedef Absyn::exp_opt_t exp_opt_t;
  typedef Absyn::cnst_t cnst_t;
  
  typedef datatype Term @term_t, *term_opt_t;

  datatype Term {
    Uint(unsigned int); // a constant value known to cyclone at compile time
    Const(exp_t); // a Cyclone "constant" expression (e.g., 3, sizeof(e), etc.)
    Var(vardecl_t);
    LogicVar(vardecl_opt_t,int,type_opt_t);
    Primop(Absyn::primop_t,list_t<term_t>,type_opt_t);
    Cast(Absyn::type_t, term_t);
  };

  string_t term2string(term_t t);
  
  term_t cnst(Absyn::exp_t e);
  term_t zero();
  term_t one();
  
  term_t var(vardecl_t vd);

  //  bool is_commutative(Absyn::primop_t p);
  term_t primop(Absyn::primop_t p, list_t<term_t,`H> ts, type_opt_t);
  term_t cast(Absyn::type_t tp, term_t tm);
  term_t fresh_var(type_opt_t);

  int cmp_term(term_t t1, term_t t2);
  Absyn::type_opt_t get_term_type(term_t t);
  ///////////////////// Assertions /////////////////////
  typedef datatype Assn @assn_t;
  typedef datatype Assn *assn_opt_t;
  typedef enum Primreln { Eq, Neq, SLt, SLte, ULt, ULte } prim_reln_t;

  datatype Assn {
    True;
    False;
    Prim(term_t,prim_reln_t,term_t);
    And(assn_t,assn_t);
    Or(assn_t,assn_t);
    Subst(vardecl_t x,term_t t,assn_t a); // a[t/x] -- lazy substitution
    Kill(assn_t); // replace all escaping variables with fresh logic variables
  };

  int assncmp(datatype Assn@a1, datatype Assn@a2);  

  string_t assn2string(assn_t a);

  // assertion constants for true and false
  extern datatype Assn.True true_assn;
  extern datatype Assn.False false_assn;

  //////////////////////////////////////////////////////////////
  // hash-consing to try to preserve as much sharing as we
  // can -- has a nice impact on performance (much better than
  // lazy substitution), but alas, we still end up allocating a lot.  
  // It would be better if we didn't have to allocate the thing 
  // that want to lookup.  That could happen if we had a different 
  // type for hashtable lookup (as we did with dictionaries.)

  void reset_hash_cons_table(void);

  //  int assnhash(datatype Assn @a);

  // compare two assertions for structural equality -- does not assume
  // the components have been hash-consed, but probably could.
  //  int assncmp(datatype Assn @a1, datatype Assn @a2);
  assn_t kill(assn_t a);
  
  assn_t and(assn_t a1, assn_t a2);
  assn_t or(assn_t a1, assn_t a2);
  assn_t subst(vardecl_t x, term_t newx, assn_t a);

  assn_t prim(term_t t1, prim_reln_t p, term_t t2);
  assn_t eq(term_opt_t, term_opt_t);
  assn_t neq(term_opt_t, term_opt_t);
  assn_t slt(term_opt_t, term_opt_t);
  assn_t slte(term_opt_t, term_opt_t);
  assn_t ult(term_opt_t, term_opt_t);
  assn_t ulte(term_opt_t, term_opt_t);

    
  assn_t reduce(assn_t a);
  term_t subst_term(term_t t, vardecl_t x, term_t newx);
  assn_t not(assn_t a);
  bool simple_prove(assn_t ctxt, assn_t a);

  term_t kill_mem_term(term_t t);

}
#endif
