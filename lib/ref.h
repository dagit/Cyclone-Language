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

#ifndef _REF_H_
#define _REF_H_
namespace Ref {
  /*** \subsection{\texttt{<ref.h>}} */
  /*** Defines namespace Ref, which implements reference cells. */

extern struct Ref<`a> {`a v;};
  /** A reference cell is a structure holding a single value. */
typedef struct Ref<`a> @ ref_t<`a>;
  /** The type [ref_t] is used to pass the cell by reference. */
extern ref_t<`a> new_ref(`a);
  /** [new_ref(x)] creates a new reference cell on the heap, with
      initial contents [x], and returns a pointer to the new cell. */
extern `a get(ref_t<`a>);
  /** [get(c)] returns the value contained in the cell [c]. */
extern void set(ref_t<`a>,`a);
  /** [set(c,x)] updates the value in the cell [c] to [x]. */
}
#endif
