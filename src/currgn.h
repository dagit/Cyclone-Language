/* Resolves uses of the "current region" metavariables in code.
   Copyright (C) 2004 Michael Hicks, Greg Morrisett, AT&T
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
#ifndef _CURRGN_H_
#define _CURRGN_H_

#include "absyn.h"
#include "tcenv.h"

namespace CurRgn {
  using Absyn;
  using List;

  string_t curr_rgn_name;
  tvar_t curr_rgn_tvar (void);
  type_t curr_rgn_type (void);

  $(list_t<tvar_t,`H>,type_t) add_curr_tvar(list_t<tvar_t,`H> tvs,type_t cap);

  void inst_curr_rgn(Tcenv::tenv_t te, $(tvar_t,type_t)@`r instv);

  void adjust_all(list_t<decl_t,`H> tds);
}

#endif
