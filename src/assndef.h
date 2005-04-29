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
#include <set.h>

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
    // represents a program variable that doesn't escape
    Var(vardecl_t);
    // represents the address of a variable that might escape.  Note
    // that when vd1 != vd2, then we known Addr(vd1) != Addr(vd2) since
    // distinct program variables are given distinct values for addresses.
    Addr(vardecl_t);
    // represents a lookup in memory mem at address addr
    Select(term_t mem,term_t addr,type_opt_t);
    // represents the same memory function as mem, but updated so that
    // it maps address addr to value.  
    Update(term_t mem,term_t addr,term_t value);
    // logic variable.
    LogicVar(vardecl_opt_t,int,type_opt_t);
    Unop(Absyn::primop_t,term_t,type_opt_t); // Not, Bitnot, Numelts
    Binop(Absyn::primop_t,term_t,term_t,type_opt_t); // all other primops
    Cast(Absyn::type_t, term_t); // type-cast
    Aggr(list_t<term_t>,type_opt_t); // an aggregate (struct, tuple, etc.)
    Proj(term_t tuple, unsigned index, type_opt_t); // projection off of aggr
    Okderef(term_t t); // value u>0 iff t is an address of a valid memory location
  };

  // a distinguished program variable representing memory -- when we
  // write to an escaping variable, we substitute a fresh logic variable
  // for this vardecl.
  extern vardecl_t memory;

  extern string_t term2string(term_t t);
  
  extern term_t uint(unsigned int);
  extern term_t cnst(Absyn::exp_t e);
  extern term_t zero();
  extern term_t one();
  
  extern term_t var(vardecl_t vd);
  extern term_t addr(vardecl_t vd);
  extern term_t select(term_t mem, term_t addr, type_opt_t);
  extern term_t update(term_t mem, term_t addr, term_t value);
  extern term_t logicvar(vardecl_opt_t,int,type_opt_t);

  extern term_t binop(Absyn::primop_t p, term_t t1, term_t t2, type_opt_t);
  extern term_t unop(Absyn::primop_t p, term_t, type_opt_t);
  extern term_t plus(term_t t1, term_t t2, type_opt_t);
  extern term_t minus(term_t t1, term_t t2, type_opt_t);
  extern term_t cast(Absyn::type_t tp, term_t tm);
  extern term_t proj(term_t t, unsigned i, type_opt_t);
  extern term_t aggr(list_t<term_t,`H>, type_opt_t tp);
  extern term_t okderef(term_t t);
  extern term_t numelts_term(term_t t);

  extern term_t fresh_var(type_opt_t);

  extern int termhash(datatype Term @t);
  extern int cmp_term(datatype Term @t1, datatype Term @t2);
  extern type_opt_t get_term_type(term_t t);
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
    Not(assn_t);  // logical negation
    Subst(vardecl_t x,term_t t,assn_t a); // a[t/x] -- lazy substitution
  };
  extern unsigned int assn_size(assn_t a);
  extern int assncmp(datatype Assn@a1, datatype Assn@a2);
  extern int assnhash(datatype Assn @a);

  extern string_t assn2string(assn_t a);
  extern string_t dag2string(assn_t a);  // shows sharing

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

  extern void reset_hash_cons_table(void);

  //  int assnhash(datatype Assn @a);

  // compare two assertions for structural equality -- does not assume
  // the components have been hash-consed, but probably could.
  //  int assncmp(datatype Assn @a1, datatype Assn @a2);

  extern assn_t and(assn_t a1, assn_t a2);
  extern assn_t or(assn_t a1, assn_t a2);
  extern assn_t not(assn_t a);
  extern assn_t subst(vardecl_t x, term_t newx, assn_t a);


  extern assn_t prim(term_t t1, prim_reln_t p, term_t t2);
  extern assn_t eq(term_opt_t, term_opt_t);
  extern assn_t neq(term_opt_t, term_opt_t);
  extern assn_t slt(term_opt_t, term_opt_t);
  extern assn_t slte(term_opt_t, term_opt_t);
  extern assn_t ult(term_opt_t, term_opt_t);
  extern assn_t ulte(term_opt_t, term_opt_t);

  // eliminates not, subst, and kill
  extern assn_t reduce(assn_t a);
  extern term_t subst_term(term_t t, vardecl_t x, term_t newx);
  // factors out the common conjuncts in a as if we computed the DNF,
  // but is much more efficient.  
  typedef Set::set_t<assn_t> assn_set_t;
  typedef struct Set::Set<assn_t> *assn_set_opt_t;

  extern assn_set_opt_t widen_it(assn_t a);
  extern assn_t widen(assn_t a);
  // sees if ctxt |- a using only very simple, syntactic proof rules.
  extern bool simple_prove(assn_t ctxt, assn_t a);
}
#endif
