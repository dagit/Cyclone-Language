/* Type checking for top-level declarations
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

#ifndef _TC_H_
#define _TC_H_

#include <list.h>
#include "absyn.h"
#include "tcenv.h"

namespace Tc {

using List;
using Absyn;
using Tcenv;
using Position;

extern void tc(region_t<`g>, tenv_t<`g> te, bool var_default_init, list_t<decl_t,`H> ds);

  // if called, must be after tc is called
  // removes extern variables that are never referred to
extern list_t<decl_t> treeshake(tenv_t te, list_t<decl_t>);

// internal functions needed by tcutil
extern void tcAggrdecl(tenv_t, genv_t, seg_t, aggrdecl_t);
extern void tcDatatypedecl(tenv_t, genv_t, seg_t, datatypedecl_t);
extern void tcEnumdecl(tenv_t, genv_t, seg_t, enumdecl_t);

}
#endif
