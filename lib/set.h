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


#ifndef _SET_H_
#define _SET_H_

#include <list.h>

namespace Set {
using List;

// set.h:    defines polymorphic, functional, finite sets over elements
//           with a total order, following the conventions of the Ocaml set
//           library as much as possible.  

// `a is the type of the elements, `r is the region in which the data
// structures live.
extern struct Set<`a,`r::R>;

// generic sets abstract the element type, region, and effect -- 
// but without region handles, there's no way to really create these.
typedef struct Set<`a,`r> @`r set_t<`a,`r>;

// The set creation functions take a functional argument that takes
// two elements and returns 0 when equal, <0 when the first is less
// than the second, and >0 when the second is less than the first.

// create an empty set given a comparison function
extern set_t<`a> empty(int comp(`a,`a));
extern set_t<`a,`r> rempty(region_t<`r> rgn, int comp(`a,`a));

// create a singleton set given a comparison function and element
extern set_t<`a> singleton(int comp(`a,`a),`a x);

// create a set out of a comparison function and a list of elements
extern set_t<`a> from_list(int comp(`a,`a),list_t<`a> x);

// insert an element into a set
extern set_t<`a> insert(set_t<`a,`H> s,`a elt);
extern set_t<`a,`r> rinsert(region_t<`r> rgn, set_t<`a,`r> s, `a elt);

// union of two sets
extern set_t<`a> union_two(set_t<`a,`H> s1,set_t<`a,`H> s2);

// intersection of two sets
extern set_t<`a> intersect(set_t<`a,`H> s1,set_t<`a,`H> s2);

// set difference -- remove from s1 all elements in s2
extern set_t<`a> diff(set_t<`a,`H> s1,set_t<`a,`H> s2);

// deletes an element if it's present, otherwise returns same set
extern set_t<`a> delete(set_t<`a,`H> s,`a elt);

// number of unique elements in the set
extern int cardinality(set_t s);

// true when the set is empty
extern bool is_empty(set_t s);

// true when elt is in the set
extern bool member(set_t<`a> s,`a elt);

// true when s1 is a (not necessarily proper) subset of s2
extern bool subset(set_t<`a> s1,set_t<`a> s2);

// true when s1 and s2 contain the same elements
extern int  compare(set_t<`a> s1,set_t<`a> s2);
extern bool equals (set_t<`a> s1,set_t<`a> s2);

// returns a list of the elements (in no guarateed order)
extern list_t<`a,`r> elements(set_t<`a,`r> s);

// if s = {x1,x2,...,xn} then return f(x1,f(x2,f(...,f(xn,accum)...)))
extern `b fold(`b f(`a,`b),set_t<`a> s,`b accum);
extern `b fold_c(`b f(`c,`a,`b),`c env,set_t<`a> s,`b accum);

// apply the function f to each element of the set and discard the result
extern void app(`b f(`a),set_t<`a> s);
extern void iter(void f(`a),set_t<`a> s);
extern void iter_c(void f(`c,`a),`c env,set_t<`a> s);

extern xtunion exn {extern Absent};
extern `a choose(set_t<`a> s);

}

#endif
