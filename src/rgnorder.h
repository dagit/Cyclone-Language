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

struct RgnPO;
typedef List::list_t<struct RgnPO @> rgn_po_t;

  // means initial for a function, currently rejects cycles but needn't
  // (with effects in outlives, cycles -- especially of length 1 probably
  //  should be permitted)
rgn_po_t initial_region_po(Position::seg_t loc, List::list_t<Absyn::tvar_t> tvs,
			   List::list_t<$(Absyn::type_t,Absyn::type_t)@> po);
rgn_po_t add_region_po(rgn_po_t po, Absyn::tvar_t tv, bool resetable);
bool is_region_resetable(rgn_po_t po, Absyn::tvar_t r); // going away?
bool check_region_outlives(rgn_po_t po, Absyn::tvar_t r1, Absyn::tvar_t r2);
}

#endif
