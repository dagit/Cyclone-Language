/* Utilities for control flow analysis.
   Copyright (C) 2001 Dan Grossman, Greg Morrisett
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
#ifndef CF_FLOWINFO_H
#define CF_FLOWINFO_H

#include <list.h>
#include <set.h>
#include <dict.h>
#include <position.h>
#include "absyn.h"

// Note: Okasaki's dictionaries may be the wrong thing here because
//       we're doing a lot of intersections.  I don't know what's better,
//       but we do have all the dictionarys' domains known in advance, so 
//       there probably is something better.

// A cute hack to avoid defining the abstract syntax twice.
#ifdef CF_FLOWINFO_CYC
#define EXTERN_CFFLOW
#else
#define EXTERN_CFFLOW extern
#endif

namespace CfFlowInfo {

// Do not ever mutate any data structures built from these -- they share a lot!
EXTERN_CFFLOW tunion Root {
  VarRoot(Absyn::vardecl_t);
  // the type below is the type of the result of the malloc
  MallocPt(Absyn::exp_t,Absyn::type_t); // misnamed when do other analyses??
  InitParam(int,Absyn::type_t); // int is parameter number, type is w/o @
};
typedef tunion Root root_t;

EXTERN_CFFLOW struct Place<`r::R> {
  root_t root; 
  List::list_t<int,`r> fields; // projections off the root -- these correspond
  //to either tuple offsets or field names.  For field names, we use the order
  //in the struct declaration to determine the index.
};
typedef struct Place<`r1> @`r2 place_t<`r1,`r2>;

EXTERN_CFFLOW tunion InitLevel { 
  NoneIL, // may not be initialized
  ThisIL, // this is initialized, but things it points to may not be
  AllIL   // initialized, and everything it points to is initialized
};
typedef tunion InitLevel initlevel_t;

// primitive relations that we track for non-escaping, integral variables
EXTERN_CFFLOW __flat__ tunion RelnOp {
  EqualConst(unsigned int);     // == c
  LessVar(Absyn::vardecl_t);    // < y
  LessSize(Absyn::vardecl_t);   // < y.size
  LessConst(unsigned int);      // < c
  LessEqSize(Absyn::vardecl_t); // <= y.size
};
typedef tunion RelnOp reln_op_t;
EXTERN_CFFLOW struct Reln {
  Absyn::vardecl_t vd; 
  reln_op_t rop;
};
typedef struct Reln @reln_t;
typedef List::list_t<reln_t> relns_t;

EXTERN_CFFLOW struct TagCmp {
  Absyn::primop_t cmp; // for now, MUST be Lt, Lte, or Eq
  Absyn::type_t   bd;  // IntKind
};
typedef struct TagCmp @tag_cmp_t;

  // as with AbsRVal below, HasTagCmps forgets zero-ness which technically
  // is bad but shouldn't matter in practice.
xtunion Absyn::AbsynAnnot { 
  EXTERN_CFFLOW IsZero;
  EXTERN_CFFLOW NotZero(relns_t);
  EXTERN_CFFLOW UnknownZ(relns_t);
  EXTERN_CFFLOW HasTagCmps(List::list_t<tag_cmp_t>);
};

EXTERN_CFFLOW tunion AbsLVal { PlaceL(place_t); UnknownL; };
typedef tunion AbsLVal absLval_t;

typedef tunion AbsRVal absRval_t;
typedef Dict::dict_t<root_t,             absRval_t> flowdict_t;
typedef absRval_t ?aggrdict_t;
extern aggrdict_t aggrfields_to_aggrdict(List::list_t<Absyn::aggrfield_t>, absRval_t);
extern flowdict_t empty_flowdict();
EXTERN_CFFLOW tunion AbsRVal {
  Zero;      // the value is zero and initialized
  NotZeroAll; // the value is not zero & everything reachable from it is init
  NotZeroThis; // the value is not zero, it is initialized, but not necessarily
               // what it points to
  UnknownR(initlevel_t); // don't know what the value is
  Esc(initlevel_t); // as an rval means same thing as UnknownR!!
  AddressOf(place_t); // I am a pointer to this place (implies not zero)
  // TagCmps forgets zero-ness which technically is bad but shouldn't
  // matter in practice (why would an array index also be used in tests?)
  // and joins punt to UnknownR/Esc when comparing a TagCmp w/ something else
  // Can always add zero-ness as another field.
  TagCmps(List::list_t<tag_cmp_t>);
  Aggregate(aggrdict_t); // if you're a struct or tuple, you should always
  // evaluate to an Aggregate in the abstract interpretation (tunion?)
};

typedef Dict::dict_t<place_t,Position::seg_t> place_set_t;
extern place_set_t mt_place_set();
extern bool update_place_set(place_set_t *set, place_t<`H,`H> place,
			     Position::seg_t loc);
extern place_set_t union_place_set(place_set_t s1, place_set_t s2, bool disjoint);
extern bool place_set_subset(place_set_t s1, place_set_t s2);
extern bool place_set_equals(place_set_t s1, place_set_t s2);

EXTERN_CFFLOW struct ConsumeInfo {
  place_set_t consumed;              // variables consumed by this flow
  List::list_t<place_t> may_consume; // variables to possibly consume
    // INVARIANT: should be sorted at all times!
};
typedef struct ConsumeInfo consume_t;

extern consume_t and_consume(consume_t c1, consume_t c2);
  // checks that the same variables are not consumed
extern bool consume_approx(consume_t c1, consume_t c2);

// Note: It would be correct to make the domain of the flowdict_t
//       constant (all roots in the function), but it easy to argue
//       that we at program point p, we only need those roots that
//       are the target of an AddressOf or are locals in scope (so they
//       might be mentioned explicitly in the program text).  A property
//       of the analysis must be that at least these roots stay in the dict;
//       for scalability, we don't have others.
// join takes the intersection of the dictionaries.
EXTERN_CFFLOW __flat__ tunion FlowInfo {
  BottomFL;
  ReachableFL(flowdict_t,relns_t,consume_t);
};
typedef tunion FlowInfo flow_t;

extern absRval_t unknown_none;
extern absRval_t unknown_this;
extern absRval_t unknown_all;
extern absRval_t esc_none;
extern absRval_t esc_this;
extern absRval_t esc_all;

extern int root_cmp(root_t, root_t);
extern int place_cmp(place_t, place_t);

extern absRval_t typ_to_absrval(Absyn::type_t t, absRval_t leafval);

extern initlevel_t initlevel   (flowdict_t d, absRval_t r);
extern absRval_t   lookup_place(flowdict_t d, place_t place);
extern bool        is_unescaped(flowdict_t d, place_t place);
extern bool flow_lessthan_approx(flow_t f1, flow_t f2);

extern void print_absrval(absRval_t rval);
extern void print_initlevel(initlevel_t il);
extern void print_root(root_t root);
extern void print_place_set(place_set_t p);
extern void print_place_list(List::list_t<place_t> p);
extern void print_flowdict(flowdict_t d);
extern void print_flow(flow_t f);

// debugging
  // #define DEBUG_FLOW
#ifdef DEBUG_FLOW
#define DEBUG_PRINT(arg...) fprintf(stderr,##arg)
#define DEBUG_PRINT_F(f,arg...) f ## (##arg)
#else
#define DEBUG_PRINT_F(f,arg...) {}
#define DEBUG_PRINT(arg...) {}
#endif

extern relns_t reln_assign_var(relns_t, Absyn::vardecl_t, Absyn::exp_t);
extern relns_t reln_assign_exp(relns_t, Absyn::exp_t, Absyn::exp_t);
extern relns_t reln_kill_var(relns_t, Absyn::vardecl_t);
extern relns_t reln_kill_exp(relns_t, Absyn::exp_t);
extern void print_relns(relns_t);

  // all of the following throw EscNotInit as appropriate
  // the field list in the thrown place_t might be empty even if it shouldn't be
extern flowdict_t escape_deref(flowdict_t d, place_set_t * all_changed,
			       absRval_t r);
extern flowdict_t assign_place(Position::seg_t loc, flowdict_t d,
			       place_set_t * all_changed, place_t<`H,`H> place,
			       absRval_t r);
extern flow_t join_flow(place_set_t*,flow_t,flow_t,bool); 
extern $(flow_t,absRval_t) join_flow_and_rval(place_set_t* all_changed,
					      $(flow_t,absRval_t) pr1,
					      $(flow_t,absRval_t) pr2,
					      bool);
extern flow_t after_flow(place_set_t*,flow_t,flow_t,place_set_t,place_set_t);
  // reset anything that points into rgn to be uninitialized
extern flow_t kill_flow_region(flow_t f, Absyn::type_t rgn);

tunion KillRgn {
  UniqueRgn_k;
  Region_k(Absyn::tvar_t);
};
typedef tunion `r KillRgn killrgn_t<`r::R>;

extern bool contains_region(killrgn_t rgn, Absyn::type_t t);

extern flow_t consume_unique_rvals(Position::seg_t loc,flow_t f);
extern void check_unique_rvals(Position::seg_t loc, flow_t f);
extern flow_t readthrough_unique_rvals(Position::seg_t loc,flow_t f);
extern flow_t drop_unique_rvals(Position::seg_t loc,flow_t f);

extern $(consume_t,flow_t) save_consume_info(flow_t f, bool clear);
extern flow_t restore_consume_info(flow_t f, consume_t c);
extern string_t place_err_string(place_t place);
}
#endif
