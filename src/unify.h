/* Unifying types
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
#ifndef _UNIFY_H_
#define _UNIFY_H_

#include "absyn.h"

namespace Unify {
// explain_failure() explains why unify failed and at what particular types.
// Output goes to stderr.
void explain_failure();
bool unify_kindbound(Absyn::kindbound_t, Absyn::kindbound_t);
bool unify(Absyn::type_t, Absyn::type_t);

  // only here for Tcutil -- still fleshing out interface
  void occurs(Absyn::type_t evar,region_t<`r> r,
	      List::list_t<Absyn::tvar_t,`r> env,Absyn::type_t t);
}
#endif
