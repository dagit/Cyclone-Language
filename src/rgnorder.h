/* Region orders.
   Copyright (C) 2002 Dan Grossman
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

#ifndef _RGNORDER_H_
#define _RGNORDER_H_

#include "absyn.h"

namespace RgnOrder {
using Absyn;
using List;

extern struct RgnPO;
typedef struct RgnPO @ rgn_po_t;
typedef struct RgnPO * rgn_po_opt_t;

rgn_po_t initial_fn_po(list_t<tvar_t> tvs, 
		       list_t<$(type_t,type_t)@> po,
		       type_t effect,
		       tvar_t fst_rgn, Position::seg_t);
//  bool sub_constraints(region_t<`r>, list_t<tvar_t> tvs,
//  		     list_t<$(type_t,type_t)@> po1, 
//  		     list_t<$(type_t,type_t)@> po2);
rgn_po_t add_outlives_constraint(rgn_po_t, type_t eff, type_t rgn, Position::seg_t);
rgn_po_t add_youngest(rgn_po_t, tvar_t rgn,bool opened);
rgn_po_t add_unordered(rgn_po_t, tvar_t rgn);
bool effect_outlives(seg_t, rgn_po_opt_t, type_t eff, type_t rgn);
bool rgn_outlives_rgn(seg_t, rgn_po_opt_t, type_t rgn1, type_t rgn2);
bool eff_outlives_eff(seg_t, rgn_po_opt_t, type_t eff1, type_t eff2);
bool satisfies_constraints(seg_t, rgn_po_opt_t, list_t<$(type_t,type_t)@> constraints,
			   type_t default_bound, bool do_pin);
  
//DEBUGGING
void print_region_po(rgn_po_t po);
}

#endif
