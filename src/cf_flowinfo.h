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
typedef datatype `r Root root_t<`r>;

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

  // NOTE: UniquePlace not currently in use; will finish up later ...
EXTERN_CFFLOW struct UniquePlace<`r::R> {
  struct Place<`r> place;
  List::list_t<List::list_t<int,`r>,`r> path;
  //this is a path between unique places.  That is, each element in
  //the path is a list of fields that define the place reached from
  //there, before doing a dereference.  For example, the difference
  //between x.hd and x->hd is that we will have x.0 in both cases, but
  //a path of NULL in the former case, and a path of [[ ]] in the
  //latter, signifying the dereference.  Likewise, x->tl->hd would
  //have initial place x.1, and then path [[0]; []].
};
typedef struct UniquePlace<`r1> @`r2 unique_place_t<`r1,`r2>;

EXTERN_CFFLOW datatype InitLevel { 
  NoneIL, // may not be initialized
  ThisIL, // this is initialized, but things it points to may not be
  AllIL   // initialized, and everything it points to is initialized
};
typedef datatype InitLevel initlevel_t;

// primitive relations that we track for non-escaping, integral variables
EXTERN_CFFLOW __flat__ datatype RelnOp {
  EqualConst(unsigned int);        // == c
  LessVar(Absyn::vardecl_t,Absyn::type_t); // < y -- type is needed due to xlation
  LessNumelts(Absyn::vardecl_t);   // < numelts(y)
  LessConst(unsigned int);         // < c
  LessEqNumelts(Absyn::vardecl_t); // <= numelts(y)
};
typedef datatype RelnOp reln_op_t;
EXTERN_CFFLOW struct Reln {
  Absyn::vardecl_t vd; 
  reln_op_t rop;
};
typedef struct Reln @`r reln_t<`r>;
typedef List::list_t<reln_t<`r>,`r> relns_t<`r>;

EXTERN_CFFLOW struct TagCmp {
  Absyn::primop_t cmp; // for now, MUST be Lt, Lte, or Eq
  Absyn::type_t   bd;  // IntKind
};
typedef struct TagCmp @`r tag_cmp_t<`r>;

  // as with AbsRVal below, HasTagCmps forgets zero-ness which technically
  // is bad but shouldn't matter in practice.
@extensible datatype Absyn::AbsynAnnot { 
  EXTERN_CFFLOW IsZero;
  EXTERN_CFFLOW NotZero(relns_t<`H>);
  EXTERN_CFFLOW UnknownZ(relns_t<`H>);
  EXTERN_CFFLOW HasTagCmps(List::list_t<tag_cmp_t<`H>,`H>);
};
extern List::list_t<tag_cmp_t<`r2>,`r2> copy_tagcmps(region_t<`r2>,
                                                     List::list_t<tag_cmp_t>);

EXTERN_CFFLOW __flat__ datatype AbsLVal<`r::R> {
  PlaceL(place_t<`r,`r>);
  UnknownL;
};
typedef datatype AbsLVal<`r> absLval_t<`r>;

EXTERN_CFFLOW datatype AbsRVal<`r::R>;
typedef datatype `r AbsRVal<`r> absRval_t<`r>;
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
  // TagCmps forgets zero-ness which technically is bad but shouldn't
  // matter in practice (why would an array index also be used in tests?)
  // and joins punt to UnknownR/Esc when comparing a TagCmp w/ something else
  // Can always add zero-ness as another field.
  TagCmps(List::list_t<tag_cmp_t<`r>,`r>);
  // if you're a tagged union, struct, or tuple, you should always
  // evaluate to an Aggregate in the abstract interpretation (datatype?)
  Aggregate(bool is_union, aggrdict_t<`r>);
};

typedef Dict::dict_t<`a,Position::seg_t,`r> dict_set_t<`a,`r>;

extern bool update_place_set(dict_set_t<`a,`r> *set, `a place,
			     Position::seg_t loc);
extern bool place_set_subset(dict_set_t<`a,`r> s1, dict_set_t<`a,`r> s2);
extern bool place_set_equals(dict_set_t<`a,`r> s1, dict_set_t<`a,`r> s2);

typedef Dict::dict_t<place_t<`r,`r>,Position::seg_t,`r> place_set_t<`r>;
extern place_set_t<`r> union_place_set(place_set_t<`r> s1, place_set_t<`r> s2, bool disjoint);

typedef Dict::dict_t<unique_place_t<`r,`r>,Position::seg_t,`r> uplace_set_t<`r>;
extern uplace_set_t<`r> union_uplace_set(uplace_set_t<`r> s1, uplace_set_t<`r> s2, bool disjoint);

EXTERN_CFFLOW struct ConsumeInfo<`r::R> {
  place_set_t<`r> consumed;              // variables consumed by this flow
  List::list_t<place_t<`r,`r>,`r> may_consume; // variables to possibly consume
    // INVARIANT: should be sorted at all times!
};
typedef struct ConsumeInfo<`r::R> consume_t<`r>;

extern consume_t<`r> and_consume(region_t<`r>, consume_t<`r> c1, consume_t<`r> c2);
  // checks that the same variables are not consumed
extern bool consume_approx(consume_t<`r> c1, consume_t<`r> c2);
// Note: It would be correct to make the domain of the flowdict_t
//       constant (all roots in the function), but it easy to argue
//       that we at program point p, we only need those roots that
//       are the target of an AddressOf or are locals in scope (so they
//       might be mentioned explicitly in the program text).  A property
//       of the analysis must be that at least these roots stay in the dict;
//       for scalability, we don't have others.
// join takes the intersection of the dictionaries.
EXTERN_CFFLOW __flat__ datatype FlowInfo<`r::R> {
  BottomFL;
  ReachableFL(flowdict_t<`r>,relns_t<`r>,consume_t<`r>);
};
typedef datatype FlowInfo<`r> flow_t<`r>;

EXTERN_CFFLOW struct FlowEnv<`r::R> {
  region_t<`r>    r;
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

extern aggrdict_t<`r> aggrfields_to_aggrdict(flow_env_t<`r>, List::list_t<Absyn::aggrfield_t>, absRval_t<`r>);
extern absRval_t<`r> typ_to_absrval(flow_env_t<`r>, Absyn::type_t t, absRval_t<`r> leafval);

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
extern flow_t<`r> join_flow(flow_env_t<`r>,place_set_t<`r>*,flow_t<`r>,flow_t<`r>,bool); 
extern $(flow_t<`r>,absRval_t<`r>) 
  join_flow_and_rval(flow_env_t<`r>,
                     place_set_t<`r>* all_changed,
                     $(flow_t<`r>,absRval_t<`r>) pr1,
                     $(flow_t<`r>,absRval_t<`r>) pr2,
                     bool);
extern flow_t<`r> after_flow(flow_env_t<`r>,place_set_t<`r>*,
                             flow_t<`r>,flow_t<`r>,
                             place_set_t<`r>,place_set_t<`r>);
                             
  // reset anything that points into rgn to be uninitialized
extern flow_t<`r> kill_flow_region(flow_env_t<`r> fenv, flow_t<`r> f, Absyn::type_t rgn);

datatype KillRgn {
  UniqueRgn_k;
  Region_k(Absyn::tvar_t);
};
typedef datatype `r KillRgn killrgn_t<`r::R>;

extern bool contains_region(killrgn_t rgn, Absyn::type_t t);

extern flow_t<`r> consume_unique_rvals(Position::seg_t loc,flow_t<`r> f);
extern void check_unique_rvals(Position::seg_t loc, flow_t<`r> f);
extern flow_t<`r> readthrough_unique_rvals(Position::seg_t loc,flow_t<`r> f);
extern flow_t<`r> drop_unique_rvals(Position::seg_t loc,flow_t<`r> f);

extern $(consume_t<`r>,flow_t<`r>) save_consume_info(flow_env_t<`r>,flow_t<`r> f, bool clear);
extern flow_t<`r> restore_consume_info(flow_t<`r> f, consume_t<`r> c, bool may_consume_only);
extern string_t place_err_string(place_t place);

extern unique_place_t<`r,`r> unique_place_of(region_t<`r> r, Absyn::exp_t e);
  // calculates the unique place of the unique path [e]
}
#endif
