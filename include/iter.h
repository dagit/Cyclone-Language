/* This file is part of the Cyclone Library.
   Copyright (C) 2001 Greg Morrisett, AT&T

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

#ifndef _ITER_H_
#define _ITER_H_
#include <core.h>
namespace Iter {
  /*** \subsection{\texttt{<iter.h>}} */
  /*** Defines namespace Iter, which implements imperative iterators
       over sets/sequences of elements.
  */

  struct Iter<`a::A,`bd::R> { 
    <`env> : regions(`env) > `bd
    `env env;
    // return false when no more elts.
    bool (@next)<`r>(`env env, `a @`r dest); 
  };
  typedef struct Iter<`a,`bd> iter_t<`a,`bd>;
  /** A value of type [iter_t<`a,`bd>] is an iterator over elements
      of type [`a]. */
  bool next(iter_t<`a::A>,`a @);
  /** If there is a next element, [next(i,p)] returns [true] and
      assigns the next element to [*p].  If there is no next element,
      [next(i,p)] returns [false] without assigning anything to [*p]. */
}
#endif
