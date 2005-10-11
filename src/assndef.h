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
#include <dict.h>
#include <string.h>
#include <set.h>
namespace AssnDef{
  typedef Absyn::type_t type_t;
  typedef Absyn::type_opt_t type_opt_t;
  typedef Absyn::vardecl_t vardecl_t;
  typedef Absyn::vardecl_opt_t vardecl_opt_t;
  typedef Absyn::exp_t exp_t;
  typedef Absyn::decl_t decl_t;
  typedef Absyn::stmt_t stmt_t;
  typedef Absyn::stmt_opt_t stmt_opt_t;
  typedef Absyn::switch_clause_t switch_clause_t;
  typedef Absyn::exp_opt_t exp_opt_t;
  typedef Absyn::cnst_t cnst_t;
  typedef Absyn::field_name_t field_name_t;
  typedef Absyn::var_opt_t var_opt_t;
  typedef Absyn::tqual_t tqual_t;

  ////////////// Term Definition and Manipulation////////////

  typedef datatype Term @term_t;
  typedef datatype Term *term_opt_t;

  typedef struct Set::Set<term_t> @term_set_t;
  
  /********************************************************** 
   t ::= i | c | loc | select(m,loc) | update(m,loc,t) | v | 
         op t | t1 op t2 | (\tau) t | {tag, [t1,t2,...tn]} | 
         t.fi
   loc ::= &x | alloc(e,i) | [i,loc] | [offset(field),loc]
  ***********************************************************/
  datatype Term {
    Uint(unsigned int,type_opt_t); // a constant value known to cyclone at compile time
    Const(exp_t); // a Cyclone "constant" expression (e.g., sizeof(e), etc.)
    // logic variable -- the int is the real identity -- the vardecl is
    // intended to be used to associate the logic variable with a program
    // variable.
    LogicVar(vardecl_opt_t,int,type_opt_t); 
    Unop(Absyn::primop_t,term_t,type_opt_t); // Not, Bitnot, Numelts, etc.
    Binop(Absyn::primop_t,term_t,term_t,type_opt_t); // all other primops
    Cast(Absyn::type_t, term_t); // type-cast
    // represents a lookup in memory mem at location addr
    Select(term_t mem,term_t addr,type_opt_t);
    // represents the same memory function as mem, but updated so that
    // it maps address addr to value.  
    Update(term_t mem,term_t addr,term_t value);

    // We combine structs, datatypes, unions, and tagged unions into
    // one big "aggregate" term.  
    // Unions are represented by one tagged value.  The tag records
    // the member that was last written, and we can only reduce a
    // projection when the member being read was the same as the one last
    // written.  (This holds for both @tagged and bits-only unions.)
    // For datatypes, the is_union field is false, and the tag field
    // records which constructor we're dealing with.  For structs,
    // the is_union field is false, and the tag is set to -1.
    Aggr(bool is_union, unsigned tag, List::list_t<term_t>,type_opt_t); 
    // projection off of aggr -- as noted above, this does not reduce
    // when the aggr term is a union Aggr, and the tag does not match
    // the index.  Otherwise, Proj(Aggr(_,_list(t0,t1,...,tn)),i,_) -> ti.
    Proj(term_t aggr, unsigned index, type_opt_t); 
    // This is a functional update on aggregates.  
    // AggrUpdate(Aggr[t1,...,ti,...,tn],i,v) --> Aggr[t1,...,v,...,tn]
    // When a union is updated, the tag changes.
    AggrUpdate(term_t aggr, unsigned index, term_t value);
    // Addr(x) represents the address of a program variable x (i.e., &x).
    // The term invok should always be a logic variable and represents
    // an invocation of a function.  Different invocations yield
    // Addr(...) terms with different invok terms.  If you like,
    // invok represents the base of the stack frame, whereas x represents
    // some symbolic offset relative to that frame.  Given the same
    // invok but different vardecls, we know the addresses are distinct.
    // Addr(-) terms are really only used on escaping variables and
    // are used as "roots" into memory.  
    Addr(vardecl_t,term_t invok,type_opt_t); 
    // Represents a location created during memory allocation (i.e., new,
    // malloc, calloc, etc.)  The expression is used to distinguish
    // distinct allocation points in the program so that we can determine
    // when two allocation points cannot be the same value.  The nelts
    // field determines the number of elements in the object that was
    // allocated.  For instance, new {1,2,3} will yield an Alloc with
    // nelts = 3.  The invok term is used as in Addr(-) to distinguish 
    // allocation points from distinct invocations of a procedure.
    // The id field is used to distinguish different invocations of
    // the same allocation point.  Note that we assume Addr(-) and
    // Alloc(-) terms are always distinct, that two Alloc terms with
    // different expressions, different ids, or provably not equal
    // nelts or invok are distinct.  This is actually *not* sound since
    // the garbage collector could recycle a location and this can be
    // observed through pointer equality, casting to an int, etc.
    // Nevertheless, the assumption is important for reducing the
    // selects/updates on memory and is fairly reasonable.  
    Alloc(exp_t, int id, term_t nelts, term_t invok, type_opt_t); 
    // When t is the address of an aggregate, Offsetf(t,i) represents
    // the address of the ith member of the aggregate.  At source
    // level, this is the same as (char *)t+offsetof(T,f) where T is the
    // aggregate type and f is the ith member's name.  
    Offsetf(term_t, unsigned index, type_opt_t);
    // When t is the address of an array, Offseti(t,i) represents
    // the address of the ith element of the array.  At source
    // level, this is the same as t+i (or (char *)t + i*sizeof(T))
    // where T is the element type.  Note that index is a signed
    // term.  
    Offseti(term_t, term_t, type_opt_t);
    // When t is an aggregate, Tagof(t) reduces to the tag for that
    // aggregate.  Recall that structs have tag -1, whereas for datatypes
    // and (tagged) unions, the the tag determines which member of
    // the datatype/union was last written.  
    Tagof(term_t);
  };

  int termcmp(datatype Term @`r1 _t1, datatype Term @`r2 _t2);
  int termhash(datatype Term @t);

  extern string_t term2string(term_t t);


  // a distinguished program variable representing memory -- when we
  // write to an escaping variable, we substitute a fresh logic variable
  // for this vardecl.
  extern vardecl_t memory;
  // a distinguished program variable representing the exception that 
  // was thrown.  it's a function because we have to call a
  // function to create a exn_type which can't be used in initializng
  // global varibales 
  extern vardecl_t exception_vardecl();

  // some constant terms
  extern term_t zero();
  extern term_t one();

  // functions to create terms using hash_cons
  extern term_t uint(unsigned int);
  extern term_t cnst(exp_t e);
  extern term_t select(term_t mem, term_t addr, type_opt_t);
  extern term_t update(term_t mem, term_t addr, term_t value);
  extern term_t logicvar(vardecl_opt_t,int,type_opt_t);
  extern term_t unop(Absyn::primop_t p, term_t, type_opt_t);
  extern term_t binop(Absyn::primop_t p, term_t t1, term_t t2, type_opt_t);
  extern term_t cast(type_t tp, term_t tm);
  extern term_t aggr(bool is_union, unsigned tag, List::list_t<term_t,`H>, type_opt_t tp);
  extern term_t tagof_tm(term_t);
  extern term_t proj(term_t t, unsigned i, type_opt_t);
  extern term_t aggr_update(term_t t1, unsigned i, term_t t3);
  extern term_t addr(vardecl_t, term_t invok);
  extern term_t alloc(exp_t, term_t nelts, term_t invok, type_opt_t);
  extern term_t offsetf(term_t t, unsigned i, type_opt_t);
  extern term_t offseti(term_t t, term_t i, type_opt_t);

  // some special term constructions:
  extern term_t numelts_term(term_t t);
  extern term_t plus(term_t t1, term_t t2, type_opt_t);
  extern term_t minus(term_t t1, term_t t2, type_opt_t);
  extern term_t struct_aggr(List::list_t<term_t,`H>, type_opt_t tp);
  extern term_t datatype_aggr(unsigned tag, List::list_t<term_t,`H>, type_opt_t tp);
  extern term_t union_aggr(unsigned tag, List::list_t<term_t,`H>, type_opt_t tp);
  extern term_t fresh_var(type_opt_t);

  // given an address term a, which could be a path of the form
  // offsetf(...offsetf(offsetf(r,i0,tp0),i1,tp1),...,in,tpn) where r is 
  // a "root", returns the pair $(r,list($(i0,tp0),...,$(in,tpn))).
  extern $(term_t root,List::list_t<$(int,type_opt_t)@> fields) split_addr(term_t a);
  // given an aggregate term a and value v and path of fields 
  // i0,i1,...,in produces the result of substituting v at that
  // path in a.  In particular, produces
  // aggr_update(a,i0,
  //  aggr_update(proj(a,i0),i1,...
  //       (aggr_update(proj(...proj(proj(a,i0),i1)...,in-1)...),in,v)
  // which will hopefully reduce down to something reasonable.
  extern term_t apply_aggr_update(term_t a, List::list_t<$(int,type_opt_t)@> fields, term_t v);

  extern type_opt_t get_term_type(term_t);
  // data structure for a polynomial a1*t1 + a2*t2 + ... + an*tn
  // used in 1) normalizing a term. 
  //         2) generating matrix for Gaussian elimination
  //  typedef List::list_t<$(int,term_t)@,`H> coefficient_term_list_t;
  // a function to retrieve the coefficients and terms in a
  // polynomial from a term, the extra integer represent
  // the constants a0 in the term
  // a0 + a1*t1 + a2*t2 + ...
  //  extern $(coefficient_term_list_t,int) get_coefficient_term_list(term_t);

  ////////////// Assertion Definition and Manipulation////////////
  typedef datatype Assn @assn_t;
  typedef enum Primreln { Eq, Neq, SLt, SLte, ULt, ULte } prim_reln_t;
  datatype Assn {
    True;
    False;
    Prim(term_t,prim_reln_t,term_t); // t1 reln t2
    And(assn_t,assn_t); // a1 /\ a2
    Or(assn_t,assn_t);  // a1 \/ a2
  };

  typedef struct Set::Set<assn_t> @assn_set_t;
  typedef struct Set::Set<assn_t> *assn_set_opt_t;

  int assncmp(datatype Assn @`r1 _a1, datatype Assn @`r2 _a2);
  
  extern string_t assn2dag(assn_t);
  extern string_t assn2string(datatype Assn @);
  extern datatype Assn.True true_assn;
  extern datatype Assn.False false_assn;

  extern assn_t and(assn_t a1, assn_t a2);
  extern assn_t or(assn_t a1, assn_t a2);
  extern assn_t not(assn_t a);
  extern assn_t prim(term_t t1, prim_reln_t p, term_t t2);
  extern assn_t eq(term_t, term_t);
  extern assn_t neq(term_t, term_t);
  extern assn_t slt(term_t, term_t);
  extern assn_t slte(term_t, term_t);
  extern assn_t ult(term_t, term_t);
  extern assn_t ulte(term_t, term_t);

  // AssnFn definitions and utility functions
  struct AssnFn {
    List::list_t <term_t> actuals; // lambda bound vars
    assn_t assn;             // lambda body, 
  };
  typedef struct AssnFn @assnfn_t, *assnfn_opt_t;
  
  extern assnfn_t apply_terms_assnfn(assnfn_t af, List::list_t<term_t> ts);
  
  typedef struct ExistAssnFn {
    assnfn_t af;
    term_set_t existvars;
  } @existassnfn_t;
  
 
  extern string_t existassnfn2string(existassnfn_t);
  // a mapping from a program variable to a term,
  // you may consider this as a special kind of assertion:
  // x == t1 /\ y == t2 /\ .... for all x,y,... in the dict
  typedef Dict::dict_t<vardecl_t,term_t> nonescvar_map_t;

  // assertion and program variable's mapping 
  struct AssnMap{
    assn_t assn;
    nonescvar_map_t map;
  };
  typedef struct AssnMap assn_map_t,*assn_map_opt_t;

  // given an assnmap, extract bound vars corresponding
  // to vds, and turn it into an bound existassnfn
  extern existassnfn_t assnmap2existassnfn(assn_map_t am, List::list_t<vardecl_opt_t> vds);

  // given a list of logicvar vs and an assn a
  // return exist us. lambda vs.a
  extern existassnfn_t bound_vs_in_assn(assn_t a, List::list_t<term_t,`H> vs);
  
  extern assn_t existassnfn2assn(existassnfn_t eaf, List::list_t<term_t> terms);
  // given an assertion, expand it with an empty map
  // and generate a new assnmap
  extern assn_map_t false_assnmap();
  extern assn_map_t true_assnmap();
  extern string_t map2string(nonescvar_map_t);
  extern string_t assnmap2dag(assn_map_t);
  extern string_t assnmap2string(assn_map_t);
  
  //given a nonescvar's vardecl, retrieve its value from the mapping
  extern $(term_t,assn_map_t) lookup_var_map(vardecl_t, assn_map_t);
  extern assn_map_t update_var_map(vardecl_t, term_t, assn_map_t);

  extern $(assn_t,assn_t,nonescvar_map_t) unify_var_maps(assn_map_t,assn_map_t);
  extern $(assn_t,assn_t,nonescvar_map_t) unify_var_maps_subst(assn_map_t,assn_map_t);
  extern assn_map_t canonical_assnmap(assn_map_t);
  // merging (or) two assertions with nonescvar mapping.
  // (A /\ x == t1) \/ (B /\ x == t2) merge into
  // (A /\ v == t1 \/ B /\ v == t2) /\ x == v
  extern assn_map_t or_assnmap_assnmap(assn_map_t am1, assn_map_t am2);
  //  extern assn_t widen(assn_t);
  extern assn_map_t widen_assnmap(assn_map_t);
  // get a new assnmap where its assn is or-combined with
  // some new assertion.
  extern assn_map_t or_assnmap_assn(assn_map_t, assn_t);
  // get a new assnmap where its assn is and-combined with
  // some new assertion
  extern assn_map_t and_assnmap_assn(assn_map_t, assn_t);
  
  extern bool simple_prove(assn_t ctxt, assn_t a);
 
  // given two assertion_map a1, a2, where the nonescvarmap 
  // only maps vardecls to logical vars of the form:
  // a1 = assn1, x1=v1(or NULL) ,x2=v2(or NULL),...
  // a2 = assn2, y1=u1,          y2=u2,...
  // return a new a2
  // a2 = assn2(vi/ui) yi = ui for each xi=vi in a1.
  // this function is used when we are comparing two
  // function signatures (such as subtyping or type equivalence)
  extern assn_t merge_assnmaps(List::list_t<vardecl_opt_t> vds1,
			       assn_map_t a1,
			       List::list_t<vardecl_opt_t> vds2,
			       assn_map_t a2);

  // given an assertion_map of the form 
  // a1 = assn1, x1 = v1, x2 = v2, ...xn = vn
  // and a new list of vardecls y1, y2, ...yn
  // generating a new assertion_map of the form
  // a2 = assn1[ui/vi] yi = ui
  // this function is used when we are creating a
  // new function signature that has the same type
  // as the old function signature
  extern assn_map_t *subst_vardecls(List::list_t<vardecl_opt_t> vds1,
				    List::list_t<vardecl_opt_t> vds2,
				    assn_map_t *a);

  // given an assnmap that was part of a function's signature
  // of the form assn /\ x1 = v1, x2 = v2,
  // and the actual function argument t1,t2,...
  // return the assertion assn[ti/vi]
  // this function is used during vcgen for function calls
  extern assn_t subst_args(List::list_t<vardecl_opt_t> args, vardecl_opt_t res,
			   List::list_t<term_t> actual_args, term_opt_t actual_res,
                           term_t mem,
			   assn_map_t am);

  // factors out common primitive assertions that hold in a
  extern assn_set_opt_t widen_it(assn_t a);
  extern assn_t widen(assn_t a);

  // basic substitution routines for assertions and terms
  typedef Dict::dict_t<term_t,term_t> term_dict_t;
  extern term_dict_t empty_term_dict();
  extern term_t subst_t(term_dict_t dict,term_t t);
  extern assn_t subst_a(term_dict_t dict, assn_t a);

  // calculate free logic variables of terms and asserts
  extern term_set_t term_fr_logicvar(term_t t);
  extern term_set_t assn_fr_logicvar(assn_t a);

  void reset_hash_cons_table(void);
  int sizeof_hash_cons_table(void);

}
#endif
