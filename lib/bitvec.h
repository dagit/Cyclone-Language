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
typedef int ?`r bitvec_t<`r>;

extern bool get(bitvec_t, int);
extern void set(bitvec_t, int);
extern void clear(bitvec_t, int);
extern bool get_and_set(bitvec_t, int);
extern void union_two(bitvec_t dest, 
                      bitvec_t src1, bitvec_t src2);
extern void intersect_two(bitvec_t dest, bitvec_t src1, 
                          bitvec_t src2);
extern void diff_two(bitvec_t dest, 
                     bitvec_t src1, bitvec_t src2);
extern bool compare_two(bitvec_t src1, bitvec_t src2);
extern bitvec_t new_empty(int);
extern bitvec_t new_full(int);
extern bitvec_t new_copy(bitvec_t);
extern bitvec_t from_list(Dict::dict_t<`a,`b,`e> d,
                          int f(`b),int sz,List::list_t<`a>);
extern List::list_t<int> to_sorted_list(bitvec_t bvec, int sz);
extern void clear_all(bitvec_t);
extern void set_all(bitvec_t);
extern bool all_set(bitvec_t bvec, int sz);
extern void print_bvec(bitvec_t bvec);
}
#endif
