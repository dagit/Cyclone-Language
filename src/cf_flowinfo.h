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
EXTERN_CFFLOW datatype Root {
  VarRoot(Absyn::vardecl_t);
  // the type below is the type of the result of the malloc
  MallocPt(Absyn::exp_t,Absyn::type_t); // misnamed when do other analyses??
  InitParam(int,Absyn::type_t); // int is parameter number, type is w/o @
};
typedef datatype Root @`r root_t<`r>;

EXTERN_CFFLOW struct Place<`r::R> {
  root_t<`r> root; 
  List::list_t<int,`r> fields; 
  //the "path" of projections off the root -- these correspond to
  //either tuple offsets or field names.  For field names, we use the
  //order in the struct declaration to determine the index.  For
  //example, x[0][1] would have fields 0 and 1 if x is a tuple, while
  //x.tl.tl would have fields 1 and 1 if tl was the second field.
};
typedef struct Place<`r1> @`r2 place_t<`r1,`r2>;

EXTERN_CFFLOW enum InitLevel { 
  NoneIL, // may not be initialized
  ThisIL, // this is initialized, but things it points to may not be
  AllIL   // initialized, and everything it points to is initialized
};
typedef enum InitLevel initlevel_t;

// primitive relations that we track for non-escaping, integral variables
EXTERN_CFFLOW @tagged union RelnOp {
  unsigned int EqualConst; // == c
  $(Absyn::vardecl_t,Absyn::type_t) LessVar;// < y -- type is needed in xlation
  Absyn::vardecl_t LessNumelts; // < numelts(y)
  unsigned int LessConst; // < c
  Absyn::vardecl_t LessEqNumelts; // <= numelts(y)
  unsigned int LessEqConst; // <= c
};

typedef union RelnOp reln_op_t;

// constructors
extern reln_op_t 
  EqualConst(unsigned int),
  LessVar(Absyn::vardecl_t,Absyn::type_t),
  LessNumelts(Absyn::vardecl_t),
  LessConst(unsigned int),
  LessEqNumelts(Absyn::vardecl_t),
  LessEqConst(unsigned int);

EXTERN_CFFLOW struct Reln {
  Absyn::vardecl_t vd; 
  reln_op_t rop;
};
typedef struct Reln @`r reln_t<`r>;
typedef List::list_t<reln_t<`r>,`r> relns_t<`r>;

@extensible datatype Absyn::AbsynAnnot { 
  EXTERN_CFFLOW IsZero;
  EXTERN_CFFLOW NotZero(relns_t<`H>);
  EXTERN_CFFLOW UnknownZ(relns_t<`H>);
};
extern_datacon(Absyn::AbsynAnnot,IsZero);

EXTERN_CFFLOW @tagged union AbsLVal<`r::R> {
  place_t<`r,`r> PlaceL;
  int UnknownL; // int unused
};
typedef union AbsLVal<`r> absLval_t<`r>;
extern absLval_t<`r> PlaceL(place_t<`r,`r>);
extern absLval_t<`r> UnknownL();

EXTERN_CFFLOW datatype AbsRVal<`r::R>;
typedef datatype AbsRVal<`r> @`r absRval_t<`r>;
typedef Dict::dict_t<root_t<`r>,absRval_t<`r>,`r> flowdict_t<`r>;
typedef absRval_t<`r> ?`r aggrdict_t<`r>;
EXTERN_CFFLOW datatype AbsRVal<`r::R> {
  Zero;      // the value is zero and initialized
  NotZeroAll; // the value is not zero & everything reachable from it is init
  NotZeroThis; // the value is not zero, it is initialized, but not necessarily
               // what it points to
  UnknownR(initlevel_t); // don't know what the value is
  Esc(initlevel_t); // as an rval means same thing as UnknownR!!
  AddressOf(place_t<`r,`r>); // I am a pointer to this place (implies not zero)
  // if you're a tagged union, struct, or tuple, you should always
  // evaluate to an Aggregate in the abstract interpretation (datatype?)
  Aggregate(bool is_union, aggrdict_t<`r>);
  Consumed(Absyn::exp_t consumer, int iteration, absRval_t<`r> oldvalue);
};

typedef Dict::dict_t<`a,Position::seg_t,`r> dict_set_t<`a,`r>;

extern bool update_place_set(dict_set_t<`a,`r> *set, `a place,
			     Position::seg_t loc);
extern bool place_set_subset(dict_set_t<`a,`r> s1, dict_set_t<`a,`r> s2);
extern bool place_set_equals(dict_set_t<`a,`r> s1, dict_set_t<`a,`r> s2);

typedef Dict::dict_t<place_t<`r,`r>,Position::seg_t,`r> place_set_t<`r>;
extern place_set_t<`r> union_place_set(place_set_t<`r> s1, place_set_t<`r> s2, bool disjoint);


// Note: It would be correct to make the domain of the flowdict_t
//       constant (all roots in the function), but it easy to argue
//       that we at program point p, we only need those roots that
//       are the target of an AddressOf or are locals in scope (so they
//       might be mentioned explicitly in the program text).  A property
//       of the analysis must be that at least these roots stay in the dict;
//       for scalability, we don't have others.
// join takes the intersection of the dictionaries.
EXTERN_CFFLOW @tagged union FlowInfo<`r::R> {
  int BottomFL; // int unused
  $(flowdict_t<`r>,relns_t<`r>) ReachableFL;
};
typedef union FlowInfo<`r> flow_t<`r>;
extern flow_t<`r> BottomFL();
extern flow_t<`r> ReachableFL(flowdict_t<`r>,relns_t<`r>);

EXTERN_CFFLOW struct FlowEnv<`r::R> {
  region_t<`r>    r;
  absRval_t<`r>   zero;
  absRval_t<`r>   notzeroall;
  absRval_t<`r>   notzerothis;
  absRval_t<`r>   unknown_none;
  absRval_t<`r>   unknown_this;
  absRval_t<`r>   unknown_all;
  absRval_t<`r>   esc_none;
  absRval_t<`r>   esc_this;
  absRval_t<`r>   esc_all;
  flowdict_t<`r>  mt_flowdict;
  place_set_t<`r> mt_place_set;
  place_t<`r,`r>  dummy_place;
};
typedef struct FlowEnv<`r> @`r flow_env_t<`r>;
extern flow_env_t<`r> new_flow_env(region_t<`r> r);

extern int get_field_index(Absyn::type_t t, Absyn::field_name_t f);
extern int get_field_index_fs(List::list_t<Absyn::aggrfield_t> fs,
			      Absyn::field_name_t f);
extern int root_cmp(root_t, root_t);
extern int place_cmp(place_t, place_t);

extern aggrdict_t<`r> aggrfields_to_aggrdict(flow_env_t<`r>, List::list_t<struct Absyn::Aggrfield@>, absRval_t<`r>);
extern absRval_t<`r> typ_to_absrval(flow_env_t<`r>, Absyn::type_t t, absRval_t<`r> leafval);
extern absRval_t<`r> make_unique_consumed(flow_env_t<`r> fenv, Absyn::type_t t, Absyn::exp_t consumer, int iteration, absRval_t<`r>);
extern bool is_unique_consumed(Absyn::exp_t e, int env_iteration, absRval_t<`r> r);

extern initlevel_t initlevel(flow_env_t,flowdict_t<`r> d, absRval_t<`r> r);
extern absRval_t<`r> lookup_place(flowdict_t<`r> d, place_t<`r,`r> place);
extern bool        is_unescaped(flowdict_t<`r> d, place_t<`r,`r> place);
extern bool flow_lessthan_approx(flow_t<`r> f1, flow_t<`r> f2);

extern void print_absrval(absRval_t rval);
extern void print_initlevel(initlevel_t il);
extern void print_root(root_t root);
extern void print_place(place_t p);
extern void print_dict_set(dict_set_t<`a> p, void (@pr)(`a));
extern void print_list(List::list_t<`a> p, void (@pr)(`a));
extern void print_flowdict(flowdict_t d);
extern void print_flow(flow_t f);

// debugging
// #define DEBUG_FLOW
#define SANITY
#ifdef DEBUG_FLOW
#define DEBUG_PRINT(arg...) fprintf(stderr,##arg)
#define DEBUG_PRINT_F(f,arg...) f ## (##arg)
#else
#define DEBUG_PRINT_F(f,arg...) {}
#define DEBUG_PRINT(arg...) {}
#endif

extern relns_t<`r> reln_assign_var(region_t<`r>, relns_t<`r>, Absyn::vardecl_t, Absyn::exp_t);
extern relns_t<`r> reln_assign_exp(region_t<`r>, relns_t<`r>, Absyn::exp_t, Absyn::exp_t);
extern relns_t<`r> reln_kill_var(region_t<`r>,relns_t<`r>, Absyn::vardecl_t);
extern relns_t<`r> reln_kill_exp(region_t<`r>,relns_t<`r>, Absyn::exp_t);
extern relns_t<`r2> copy_relns(region_t<`r2>, relns_t<`r>);
extern bool same_relns(relns_t, relns_t);
extern void print_relns(relns_t);

  // all of the following throw EscNotInit as appropriate
  // the field list in the thrown place_t might be empty even if it shouldn't be
extern flowdict_t<`r> escape_deref(flow_env_t<`r> fenv,
                                   flowdict_t<`r> d, 
                                   place_set_t<`r> * all_changed,
                                   absRval_t<`r> r);
extern flowdict_t<`r> assign_place(flow_env_t<`r> fenv,
                                   Position::seg_t loc, flowdict_t<`r> d,
                                   place_set_t<`r> * all_changed, 
                                   place_t<`r,`r> place,
                                   absRval_t<`r> r);
extern flow_t<`r> join_flow(flow_env_t<`r>,place_set_t<`r>*,flow_t<`r>,flow_t<`r>); 
extern $(flow_t<`r>,absRval_t<`r>) 
  join_flow_and_rval(flow_env_t<`r>,
                     place_set_t<`r>* all_changed,
                     $(flow_t<`r>,absRval_t<`r>) pr1,
                     $(flow_t<`r>,absRval_t<`r>) pr2);
extern flow_t<`r> after_flow(flow_env_t<`r>,place_set_t<`r>*,
                             flow_t<`r>,flow_t<`r>,
                             place_set_t<`r>,place_set_t<`r>);
                             
  // reset anything that points into rgn to be uninitialized
extern flow_t<`r> kill_flow_region(flow_env_t<`r> fenv, flow_t<`r> f, Absyn::type_t rgn);

datatype KillRgn {
  UniqueRgn_k;
  Region_k(Absyn::tvar_t);
};
extern_datacon(KillRgn,UniqueRgn_k);
typedef datatype KillRgn @`r killrgn_t<`r::R>;

extern bool contains_region(killrgn_t rgn, Absyn::type_t t);

extern string_t place_err_string(place_t place);
}
#endif
