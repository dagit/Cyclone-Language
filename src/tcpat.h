/* Type checking for patterns.
   Copyright (C) 2001 Greg Morrisett
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

#ifndef _TCPAT_H_
#define _TCPAT_H_

#include <list.h>
#include <position.h>
#include "absyn.h"
#include "tcenv.h"

namespace Tcpat {

using List;
using Position;
using Absyn;
using Tcenv;

  // You must call tcPat, then unify with the type of the value on which
  // you're switching, then call check_pat_regions.
  // If someone has a less clumsy proposal, I'd love to hear it.
extern 
$(list_t<tvar_t>, list_t<vardecl_t>)
   tcPat(tenv_t te,pat_t p,type_t *`r1 topt, type_t *`r2 region_opt);
extern void check_pat_regions(tenv_t te, pat_t p);
extern void check_switch_exhaustive(seg_t,list_t<switch_clause_t>);
extern bool check_let_pat_exhaustive(seg_t,pat_t p); // true => exhaustive
extern void check_catch_overlap(seg_t,list_t<switch_clause_t>);

}

#endif
