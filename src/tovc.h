/* Eliminate GCC specific forms from abstract syntax.
   Copyright (C) 2001 Dan Grossman.
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

#ifndef TOVC_H
#define TOVC_H

#include "absyn.h"

namespace Tovc {

extern int elim_array_initializers;
extern List::list_t<Absyn::decl_t> tovc(List::list_t<Absyn::decl_t> decls);

}
#endif
