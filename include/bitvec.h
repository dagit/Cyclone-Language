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
typedef int ?`r1 gbitvec_t<`r1>;
typedef gbitvec_t<`H> bitvec_t;

extern bool get(gbitvec_t<`r>, int);
extern void set(gbitvec_t<`r>, int);
extern void clear(gbitvec_t<`r>, int);
extern bool get_and_set(gbitvec_t<`r>, int);
extern void union_two(gbitvec_t<`r1> dest, 
                      gbitvec_t<`r2> src1, gbitvec_t<`r3> src2);
extern void intersect_two(gbitvec_t<`r1> dest, gbitvec_t<`r2> src1, 
                          gbitvec_t<`r3> src2);
extern void diff_two(gbitvec_t<`r1> dest, 
                     gbitvec_t<`r2> src1, gbitvec_t<`r3> src2);
extern bool compare_two(gbitvec_t<`r1> src1, gbitvec_t<`r2> src2);
extern bitvec_t new_empty(int);
extern bitvec_t new_full(int);
extern bitvec_t new_copy(gbitvec_t<`r>);
extern bitvec_t from_list(Dict::hdict_t<`a,`b,`e> d,
                          int f(`b),int sz,List::glist_t<`a,`r>);
extern List::list_t<int> to_sorted_list(gbitvec_t<`r> bvec, int sz);
extern void clear_all(gbitvec_t<`r>);
extern void set_all(gbitvec_t<`r>);
extern bool all_set(gbitvec_t<`r> bvec, int sz);
extern void print_bvec(gbitvec_t<`r> bvec);
}
#endif
