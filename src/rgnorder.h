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

#include <core.h>
#include <list.h>
#include <position.h>
#include "absyn.h"

namespace RgnOrder {
using Absyn;
using List;

extern struct RgnPO<`r::R>;
typedef struct RgnPO<`r> @`r rgn_po_t<`r>;

rgn_po_t<`r> initial_fn_po(region_t<`r>, list_t<tvar_t> tvs, 
                           list_t<$(type_t,type_t)@> po,
                           type_t effect,
                           tvar_t fst_rgn, Position::seg_t);
rgn_po_t<`r> add_outlives_constraint(region_t<`r>, rgn_po_t<`r> po, type_t eff, type_t rgn);
rgn_po_t<`r> add_youngest(region_t<`r>, rgn_po_t<`r> po, tvar_t rgn, bool resetable, bool opened);
bool is_region_resetable(rgn_po_t po, tvar_t r);
bool effect_outlives(rgn_po_t po, type_t eff, type_t rgn);
bool satisfies_constraints(rgn_po_t po, list_t<$(type_t,type_t)@> constraints,
			   type_t default_bound, bool do_pin);
bool eff_outlives_eff(rgn_po_t po, type_t eff1, type_t eff2);

//DEBUGGING
void print_region_po(rgn_po_t po);
}

#endif
