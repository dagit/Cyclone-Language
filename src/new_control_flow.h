/* Control flow analysis.
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

#ifndef NEW_CONTROL_FLOW_H
#define NEW_CONTROL_FLOW_H

#include <set.h>
#include <dict.h>
#include "absyn.h"
#include "jump_analysis.h"
#include "cf_flowinfo.h"

namespace NewControlFlow {

// the entry point
extern void cf_check(JumpAnalysis::jump_anal_res_t tables,
		     List::list_t<Absyn::decl_t> ds);

// controls whether or not we warn when we lose a unique pointer --
// costs some performance probably.
extern bool warn_lose_unique;
}

#endif
