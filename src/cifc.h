/* Type checking declarations.
   Copyright (C) 2004 Michael Hicks
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
#ifndef _CIFC_H_
#define _CIFC_H_

#include "absyn.h"

namespace Cifc {
using Absyn;
using List;

  /* Merges a C version of definition with a user-provided Cyclone
     version, but only if the Cyclone version is
     "representation-compatible" with the C one.  If so, the c_decl is
     changed in place. */
void merge_sys_user_decl(seg_t,bool is_buildlib,decl_t user_decl,decl_t c_decl);

void user_overrides(seg_t, list_t<decl_t> ds, list_t<decl_t> ovrs);
}

#endif /* _CIFC_H_ */
