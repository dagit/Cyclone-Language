/* Copyright (C) 2005 Greg Morrisett, AT&T.
   This file is part of the Cyclone project.

   Cyclone is free software; you can redistribute it
   and/or modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   Cyclone is distributed in the hope that it will be
   useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Cyclone; see the file COPYING. If not,
   write to the Free Software Foundation, Inc., 59 Temple Place -
   Suite 330, Boston, MA 02111-1307, USA. */

#ifndef FLATTEN_H
#define FLATTEN_H
#include <core.h>
#include "bnf.h"

namespace Flatten {
  // Flag: for alts, put every alternative as a separate symbol definition.
  // default is 0;
  extern int fully_flatten_alts;
  extern int is_flattened_symb(string_t s);
  extern grammar_t flatten_grammar(grammar_t<`H> g);
}
#endif
