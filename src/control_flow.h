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


#ifndef _CONTROL_FLOW_H_
#define _CONTROL_FLOW_H_

#include <list.h>
#include "absyn.h"

namespace ControlFlow {

  // to be used by the type-checking pass to get things set up for us
  extern void set_encloser(Absyn::stmt enclosee, Absyn::stmt encloser);

  // top-level entry for this pass
  extern void cf_check(List::list_t<Absyn::decl>);

  // needed for NewControlFlow for the moment
  extern Absyn::stmt get_encloser(Absyn::stmt enclosee);
}

#endif
