/* Type checking types
   Copyright (C) 2001 Greg Morrisett, Dan Grossman, AT&T
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
#ifndef _TCTYP_H_
#define _TCTYP_H_

#include "tcenv.h"

namespace Tctyp {
using Absyn;

  // typecheck a type, performing side-effectds which subsequent compilation
  // may assume:
  // * expand typedefs
  // * set pointers to type declarations for aggregate types
  // * set the kind field of type variables
  // * add default effects for function types 

void check_valid_toplevel_type(seg_t,Tcenv::tenv_t,type_t);
void check_fndecl_valid_type(seg_t,Tcenv::tenv_t,fndecl_t);

  // DJG: not sure how out of date this next comment is
// Ensures that the resulting free variables
// are compatible with a set of bound type variables.  This has
// the side effect of constraining the kinds of the bound type variables.
// In addition, if allow_evars is true, then the evars in the type are
// unconstrained.  Otherwise, we set all region evars to the heap and
// all effect evars to the empty set, and signal an error for a free type evar.
void check_type(seg_t, Tcenv::tenv_t, List::list_t<tvar_t,`H> bound_tvars, 
		kind_t k, bool allow_evars, bool allow_abs_aggr, type_t);
}
#endif
