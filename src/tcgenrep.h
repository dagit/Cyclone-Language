/* Generating typereps from types.
   Copyright (C) 2001 Greg Morrisett, AT&T
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

#ifndef _TCGENREP_H_
#define _TCGENREP_H_

#include "absyn.h"
#include "tcenv.h"
#include <position.h>

namespace Tcgenrep {
  using List;
  using Absyn;
  using Tcenv;
  using Position;

extern $(list_t<decl_t>,exp_t) @tcGenrep(tenv_t te, genv_t ge, 
					 seg_t loc, type_t type);

}

#endif
