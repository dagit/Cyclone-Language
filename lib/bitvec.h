/* This file is part of the Cyclone Library.
   Copyright (C) 2001 Dan Grossman

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

#ifndef _BITVEC_H_
#define _BITVEC_H_
#include <list.h>
#include <dict.h>

namespace Bitvec {

  /*** \subsection{\texttt{<bitvec.h>}} */
  /*** Defines namespace Bitvec, which implements bit vectors.  Bit
       vectors are useful for representing sets of numbers from 0 to
       [n], where [n] is not too large.  */

typedef int ?`r bitvec_t<`r>;
  /** [bitvec_t] is the type of bit vectors. */

extern bitvec_t new_empty(int);
  /** [new_empty(n)] returns a bit vector containing [n] bits, all set
      to 0. */
extern bitvec_t new_full(int);
  /** [new_full(n)] returns a bit vector containing [n] bits, all set
      to 1. */
extern bitvec_t new_copy(bitvec_t);
  /** [new_copy(v)] returns a copy of bit vector [v]. */
extern bitvec_t from_list(Dict::dict_t<`a,`b,`e> d,
                          int f(`b),int sz,List::list_t<`a>);
extern List::list_t<int> to_sorted_list(bitvec_t bvec, int sz);
extern void print_bvec(bitvec_t bvec);
extern bool get(bitvec_t, int);
  /** [get(v,n)] returns the [n]th bit of [v]. */
extern void set(bitvec_t, int);
  /** [set(v,n)] sets the [n]th bit of [v] to 1. */
extern void clear(bitvec_t, int);
  /** [clear(v,n)] sets the [n]th bit of [v] to 0. */
extern bool get_and_set(bitvec_t, int);
  /** [get_and_set(v,n)] sets the [n]th bit of [v] to 1, and returns
      its value before the set. */
extern void clear_all(bitvec_t);
  /** [clear_all(v)] sets every bit in [v] to 0. */
extern void set_all(bitvec_t);
  /** [set_all(v)] sets every bit in [v] to 1. */
extern bool all_set(bitvec_t bvec, int sz);
  /** [all_set(v)] returns true if every bit in [v] is set to 1, and
      returns false otherwise. */
extern void union_two(bitvec_t dest,
                      bitvec_t src1, bitvec_t src2);
  /** [union_two(dest,src1,src2)] sets [dest] to be the union of
      [src1] and [src2]: a bit of [dest] is 1 if either of the
      corresponding bits of [src1] or [src2] is 1, and is 0
      otherwise. */
extern void intersect_two(bitvec_t dest, bitvec_t src1,
                          bitvec_t src2);
  /** [intersect_two(dest,src1,src2)] sets [dest] to be the
      intersection of [src1] and [src2]: a bit of [dest] is 1 if both
      of the corresponding bits of [src1] and [src2] are 1, and is 0
      otherwise. */
extern void diff_two(bitvec_t dest,
                     bitvec_t src1, bitvec_t src2);
  /** [diff_two(dest,src1,src2)] sets [dest] to be the difference of
      [src1] and [src2]: a bit of [dest] is 1 if the corresponding bit
      of [src1] is 1, and the corresponding bit of [src2] is 0; and is
      0 otherwise. */
extern bool compare_two(bitvec_t src1, bitvec_t src2);
  /** [compare_two(src1,src2)] returns true if [src1] and [src2]
      are equal, and returns false otherwise. */
}
#endif
