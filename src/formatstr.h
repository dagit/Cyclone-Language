/* Parsing and checking format strings.
   Copyright (C) 2001 AT&T
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


#ifndef _FORMATSTR_H_
#define _FORMATSTR_H_

#include "tcenv.h"

namespace Formatstr {

// Returns the list of types corresponding to the specifiers in the string.
// Does all consistency checking w.r.t. the conversion specifiers
extern List::list_t<Absyn::type_t> get_format_types(Tcenv::tenv_t, string_t, bool isCproto, Position::seg_t);

// Same as above, but the rules and types are different for scanf
extern List::list_t<Absyn::type_t> get_scanf_types(Tcenv::tenv_t, string_t, bool isCproto, Position::seg_t);

}

#endif
