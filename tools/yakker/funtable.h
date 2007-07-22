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

#ifndef FUNTABLE_H
#define FUNTABLE_H
#include <core.h>
#include <hashtable.h>
#include "util.h"
#include "ykbuf.h"

namespace Funtable {

  typedef void (@`H parse_fun)<`r>(ykbuf_t @`r ykb);
  typedef Hashtable::table_t<str_t,parse_fun,`H> table_t;

  extern table_t create(int sz);
  extern void insert(table_t, const char ?`H, parse_fun);
  extern parse_fun lookup(table_t, const char ?`H);
  /** [lookup(t,name)] returns the function associated with [name] in [t],
      or throws [Hashtable::Not_found] if there is no value associated with [name]
      in [t]. 
      Based on Hashtable::lookup. Other Hashtable lookup variants can be used if 
      preferred.
  */
}
#endif
