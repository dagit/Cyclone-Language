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

extern tunion LocalRoot;
extern struct Place<`r::R>;
extern tunion Escaped;
extern tunion InitLevel;
extern struct InitState;
extern tunion PathInfo;
extern tunion FlowInfo;
typedef tunion LocalRoot local_root_t;
typedef struct Place<`r1> @`r2 place_t<`r1,`r2>;
typedef tunion Escaped escaped_t;
typedef tunion InitLevel init_level_t;
typedef struct InitState init_state_t;
typedef tunion PathInfo path_info_t;
typedef Dict::dict_t<`a,path_info_t,`H> pinfo_dict_t<`a>;
typedef tunion FlowInfo flow_info_t;
// Do not ever mutate any data structures built from these -- they share a lot!
EXTERN_CFFLOW tunion LocalRoot {
  VarRoot(Absyn::vardecl_t);
  MallocPt(Absyn::exp_t); // misnamed when do other analyses??
};
EXTERN_CFFLOW struct Place<`r1::R> {
  local_root_t root;
  List::list_t<Absyn::field_name_t,`r1> fields;
};
EXTERN_CFFLOW tunion Escaped { Esc, Unesc };
EXTERN_CFFLOW tunion InitLevel { NoneIL, ThisIL, AllIL, MustPointTo(place_t) };
EXTERN_CFFLOW struct InitState {
  escaped_t esc;
  init_level_t level; // cannot be MustPointTo if esc is Esc
};
EXTERN_CFFLOW tunion PathInfo {
  LeafPI(init_state_t);
  DictPI(pinfo_dict_t<Absyn::field_name_t>); // for all record-like types
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
  InitsFL(pinfo_dict_t<local_root_t>);
};

typedef Set::set_t<place_t> place_set_t;
extern place_set_t mt_place_set();

extern path_info_t mkLeafPI(escaped_t esc, init_level_t il);

extern int local_root_cmp(local_root_t, local_root_t);
extern int place_cmp(place_t, place_t);
extern path_info_t typ_to_unesc_none_pinfo(Absyn::type_t t);
extern path_info_t lookup_place(pinfo_dict_t<local_root_t> d, place_t place);
extern pinfo_dict_t<local_root_t> insert_place(pinfo_dict_t<local_root_t> d, 
					       place_t place, 
					       path_info_t pinfo);

extern bool isAllInit(pinfo_dict_t<local_root_t> pinfo_dict, path_info_t pinfo);

extern pinfo_dict_t<local_root_t> 
escape_pointsto(path_info_t pinfo,
		pinfo_dict_t<local_root_t> d,
		place_set_t * all_changed);
extern path_info_t assign_unknown_dict(init_level_t, path_info_t);

extern flow_info_t join_flow(place_set_t*,flow_info_t,flow_info_t);

extern flow_info_t after_flow(place_set_t*,flow_info_t,flow_info_t,
			      place_set_t,place_set_t);

extern bool flow_lessthan_approx(flow_info_t f1, flow_info_t f2);
}
#endif
