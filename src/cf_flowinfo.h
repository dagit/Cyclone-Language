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
//       but we do have all the dictionary's domains known in advance, so 
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
  MallocPt(Absyn::exp_t); // misnamed when do other analyses??
};
typedef tunion Root root_t;

EXTERN_CFFLOW struct Place<`r::R> {
  root_t root;
  List::list_t<Absyn::field_name_t,`r> fields;
};
typedef struct Place<`r1> @`r2 place_t<`r1,`r2>;

EXTERN_CFFLOW tunion InitLevel { NoneIL, ThisIL, AllIL };
typedef tunion InitLevel initlevel_t;

EXTERN_CFFLOW tunion AbsLVal { PlaceL(place_t); UnknownL; };
typedef tunion AbsLVal absLval_t;

EXTERN_CFFLOW tunion AbsRVal;
typedef tunion AbsRVal absRval_t;
typedef Dict::dict_t<root_t,             absRval_t> flowdict_t;
typedef Dict::dict_t<Absyn::field_name_t,absRval_t> aggrdict_t;
EXTERN_CFFLOW tunion AbsRVal {
  Zero;
  NotZeroAll;
  NotZeroThis;
  UnknownR(initlevel_t);
  Esc(initlevel_t); // as an rval means same thing as UnknownR!!
  AddressOf(place_t);
  Aggregate(aggrdict_t);
};
// Note: It would be correct to make the domain of the pinfo_dict_t
//       constant (all local roots in the function), but it easy to argue
//       that we at program point p, we only need those roots that
//       are the target of a MustPointTo or are locals in scope (so they
//       might be mentioned explicitly in the program text).  A property
//       of the analysis must be that at least these roots stay in the dict;
//       for scalability reasons, we don't have others.
// join takes the intersection of the dictionaries.
EXTERN_CFFLOW tunion FlowInfo {
  BottomFL;
  ReachableFL(flowdict_t);
};
typedef tunion FlowInfo flow_t;

typedef Set::set_t<place_t> place_set_t;
extern place_set_t mt_place_set();

extern absRval_t unknown_none;
extern absRval_t unknown_this;
extern absRval_t unknown_all;

extern int root_cmp(root_t, root_t);
extern int place_cmp(place_t, place_t);

extern absRval_t typ_to_absrval(Absyn::type_t t, absRval_t leafval);

extern initlevel_t initlevel   (flowdict_t d, absRval_t r);
extern absRval_t   lookup_place(flowdict_t d, place_t place);
extern bool        is_unescaped(flowdict_t d, place_t place);
extern bool flow_lessthan_approx(flow_t f1, flow_t f2);

  // all of the following throw EscNotInit as appropriate
  // the field list in the thrown place_t might be empty even if it shouldn't be
extern flowdict_t escape_deref(flowdict_t d, place_set_t * all_changed,
			       absRval_t r);
extern flowdict_t assign_place(Position::seg_t loc, flowdict_t d,
			       place_set_t * all_changed, place_t<`H,`H> place,
			       absRval_t r);
extern flow_t join_flow(place_set_t*,flow_t,flow_t); 
extern flow_t after_flow(place_set_t*,flow_t,flow_t,place_set_t,place_set_t);
}
#endif
