/* Type checking for expressions.
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
#ifndef _TCEXP_H_
#define _TCEXP_H_

#include "tcenv.h"

namespace Tcexp {

Absyn::type_t tcExp(Tcenv::tenv_t, Absyn::type_t*, Absyn::exp_t);
Absyn::type_t tcExpInitializer(Tcenv::tenv_t, Absyn::type_t*, Absyn::exp_t);
void tcTest(Tcenv::tenv_t, Absyn::exp_t, string_t<`H> msg_part);
void check_consume(Position::seg_t,Absyn::aqualbnds_t,Absyn::type_t,Absyn::exp_t);

  // for tcpat:
  Absyn::ptr_info_t fresh_pointer_type(Tcenv::tenv_t);
  Absyn::ptr_info_t fresh_pointer_type_with_aqual(Tcenv::tenv_t, Absyn::aqualtype_t aq);
}
#endif
