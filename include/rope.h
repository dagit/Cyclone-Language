/* This file is part of the Cyclone Library.
   Copyright (C) 2001 Greg Morrisett

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

#ifndef _ROPE_H_
#define _ROPE_H_
#include <core.h> // for string_t, mstring_t
#include <list.h>

namespace Rope {

  /*** \subsection{\texttt{<rope.h>}} */
  /*** Defines namespace Rope, which implements character arrays that
       can be concatenated in constant time. */

extern struct Rope_node;
typedef struct Rope_node @rope_t;
  /** A value of type [rope_t] is a character array that can be
      efficiently concatenated. */

extern rope_t from_string(string_t<`H>);
  /** [from_string(s)] returns a rope that has the same characters as
      string [s].  Note that [s] must be heap-allocated. */
extern mstring_t to_string(rope_t);
  /** [to_string(r)] returns a new, heap-allocated string with the
      same characters as rope [r]. */
extern rope_t concat(rope_t,rope_t);
  /** [concat(r1,r2)] returns a rope whose characters are the
      characters of [r1] followed by the characters of [r2]. */
extern rope_t concata(rope_t ?`H);
  /** [concata(a)] returns a rope that contains the concatenation of
      the characters in the array [a] of ropes. */
extern rope_t concatl(List::list_t<rope_t>);
  /** [concata(l)] returns a rope that contains the concatenation of
      the characters in the list [l] of ropes. */
extern unsigned int length(rope_t);
  /** [length(r)] returns the number of characters in the rope [r], up
   to but not including the first NUL character. */
extern int cmp(rope_t,rope_t);
  /** [cmp(r1,r2)] is a comparison function on ropes: it returns a
      number less than, equal to, or greater than 0 according to
      whether the character array of [r1] is lexicographically less
      than, equal to, or greater than the character array of [r2]. */
}
#endif
