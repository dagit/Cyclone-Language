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
using Tcenv;

// Checks that a type is well-formed and lives in kind k.  The input
// list of type variables is used to constrain the kinds and identities
// of the free type variables in the type.  Returns the list of free
// type variables in the type.
//
// This also performs the following side-effects which most of the 
// rest of the compiler rightfully assumes have occurred:
// * expand typedefs
// * set pointers to declarations for StructType, DatatypeType
// * change relative type names to absolute type names
// * set the kind field of type variables: we use the expected kind
//   initially, but if later constraints force a more constrained kind,
//   then we move to the more constrained kind.  
// * add default effects for function types -- the default effect
//   causes a fresh EffKind type variable e to be generated, and
//   consists of e and any free effect or region variables within
//   the function type.
// * ensures that for any free evar in the type, it can only be 
//   constrained with types whose free variables are contained in the
//   set of free variables returned.
  // extern list_t<tvar_t> check_valid_type(seg_t,tenv_t,list_t<tvar_t>,kind_t k,type_t);
// Similar to the above except that (a) there are no bound type variables,
// (b) for function types, we bind the free type variables, (c) the expected
// kind defaults to MemKind.
void check_valid_toplevel_type(seg_t,tenv_t,type_t);
// Special cased for function declarations
void check_fndecl_valid_type(seg_t,tenv_t,fndecl_t);
// Same as check_valid_type but ensures that the resulting free variables
// are compatible with a set of bound type variables.  Note that this
// the side effect of constraining the kinds of the bound type variables.
// In addition, if allow_evars is true, then the evars in the type are
// unconstrained.  Otherwise, we set all region evars to the heap and
// all effect evars to the empty set, and signal an error for a free type
// evar.
void check_type(seg_t, tenv_t, List::list_t<tvar_t,`H> bound_tvars, kind_t k,
		bool allow_evars, bool allow_abs_aggr, type_t);
}
#endif
