/* This file is part of the Cyclone Library.
   Copyright (C) 1999-2001 Greg Morrisett, Dan Grossman

   This library is free software; you can redistribute it and/or it
   under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of
   the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place, Suite
   330, Boston, MA 02111-1307 USA. */


#ifndef _ID_H_
#define _ID_H_

#include <core.h>
#include <cstdio.h>

namespace Id {

  /*** \subsection{\texttt{<id.h>}} */
  /*** Defines a namespace Id that implements a datatype of
       identifiers for use in compilers, type checkers, etc. */

using Core;
using Std;

extern struct id;
typedef struct id @Id;
  /** [Id] is the abstract type of identifiers. */

extern Id id_make(string_t<`H>,int);
  /** [id_make(s,n)] returns a new identifier with name [s] and number
      [n].  Two identifiers are equal if they have the same name and
      number. */

extern Id id_new(string_t<`H>);
  /** [id_new(s)] returns an identifier with name [s] and a fresh number. */

extern Id id_renew(Id);
  /** [id_renew(id)] returns a fresh identifier with the same name as
      [id]. */

extern Id id_of_string(string_t<`H>);
  /** [id_of_string(s)] returns an identifier with name [s] and number
      -1. */

extern Id id_unique(string_t<`H>);
  /** [id_unique(s)] is the same as [id_new(s)]. */

extern mstring_t id_to_string(Id);
  /** [id_to_string(id)] returns a string representation of [id]: its
      string and number separated by \$. */

extern void id_prn(FILE @,Id);
  /** [id_prn(f,id)] prints a string representation of [id] to [f]. */

extern mstring_t id_to_source(Id);
  /** [id_to_source(id)] returns the name of [id]. */

extern int id_compare(Id,Id);
  /** [id_compare] is a comparison function on identifiers:
      [id_compare(id1,id2)] returns a number less than, equal to, or
      greater than 0 according to whether [id1] is less than, equal
      to, or greater than [id2] in the total ordering on
      identifiers. */
}

#endif
