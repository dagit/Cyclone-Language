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

  /*** \subsection{\texttt{<set.h>}} */
  /*** Defines namespace Set, which implements polymorphic,
       functional, finite sets over elements with a total order,
       following the conventions of the Objective Caml set library as
       much as possible.
  */

extern struct Set<`a,`r::R>;

typedef struct Set<`a,`r> @`r set_t<`a,`r>;
  /** A value of type [set_t<`a,`r>] is a set with elements of type
      [`a].  The data structures used to implement the set (not the
      elements of the set!) are in region [`r]. */

  /*** The set creation functions require a comparison function as an
       argument.  The comparison function should return a number less
       than, equal to, or greater than 0 according to whether its
       first argument is less than, equal to, or greater than its
       second argument. */

extern set_t<`a> empty(int comp(`a,`a));
  /** [empty(comp)] creates an empty set given comparison function
      [comp].  The set is heap-allocated. */
extern set_t<`a,`r> rempty(region_t<`r> r, int comp(`a,`a));
  /** [rempty(r,comp)] creates an empty set in the region with handle
      [r]. */
extern set_t<`a> singleton(int comp(`a,`a),`a x);
  /** [singleton(comp,x)] creates a set on the heap with a single
      element, [x]. */
extern set_t<`a> from_list(int comp(`a,`a),list_t<`a> l);
  /** [from_list(comp,l)] creates a set on the heap; the elements of
      the set are the elements of the list [l]. */

extern set_t<`a> insert(set_t<`a,`H> s,`a elt);
  /** [insert(s,elt)] returns a set containing all the elements of
      [s], plus [elt].  The set [s] is not modified. */
extern set_t<`a,`r> rinsert(region_t<`r> r, set_t<`a,`r> s, `a elt);
  /** [rinsert(r,s,elt)] is like [insert(s,elt)], except that it works
      on sets allocated in the region with handle [r]. */
extern set_t<`a> union_two(set_t<`a,`H> s1,set_t<`a,`H> s2);
  /** [union_two(s1,s2)] returns a set whose elements are the union of
      the elements of [s1] and [s2].  (We use the name [union_two]
      because [union] is a keyword in Cyclone.) */
extern set_t<`a> intersect(set_t<`a,`H> s1,set_t<`a,`H> s2);
  /** [intersect(s1,s2)] returns a set whose elements are the
      intersection of the elements of [s1] and [s2]. */
extern set_t<`a> diff(set_t<`a,`H> s1,set_t<`a,`H> s2);
  /** [diff(s1,s2)] returns a set whose elements are the
      elements of [s1] that are not members of [s2]. */
extern set_t<`a> delete(set_t<`a,`H> s,`a elt);
  /** [delete(s,elt)] returns a set whose elements are the elements of
      [s], minus [elt]. */

extern int cardinality(set_t s);
  /** [cardinality(s)] returns the number of elements in the set [s]. */

extern bool is_empty(set_t s);
  /** [is_empty(s)] returns true if [s] has no members, and returns
      false otherwise. */
extern bool member(set_t<`a> s,`a elt);
  /** [member(s,elt)] returns true if [elt] is a member of [s], and
      returns false otherwise. */
extern bool subset(set_t<`a> s1,set_t<`a> s2);
  /** [subset(s1,s2)] returns true if [s1] is a subset of [s2], and
      returns false otherwise. */
extern int  compare(set_t<`a> s1,set_t<`a> s2);
  /** [compare(s1,s2)] returns a number less than, equal to, or
      greater than 0 according to whether [s1] is less than, equal to,
      or greater than [s2] in the subset order. */
extern bool equals (set_t<`a> s1,set_t<`a> s2);
  /** [equals(s1,s2)] returns true if [s1] equals [s2] have the same
      elements, and returns false otherwise. */

extern list_t<`a,`r> elements(set_t<`a,`r> s);
  /** [elements(s)] returns a list of the elements of [s], in no
      particular order.  Note that the returned list is allocated in
      the same region as the set [s]. */

extern `b fold(`b f(`a,`b),set_t<`a> s,`b accum);
  /** If [s] is a set with elements [x1] through [xn], then
      [fold(f,s,accum)] returns
      [f(x1,f(x2,f(...,f(xn,accum)...)))]. */
extern `b fold_c(`b f(`c,`a,`b),`c env,set_t<`a> s,`b accum);
  /** [fold_c(f,env,s,accum)] is like [fold], except that the function
      [f] takes an extra (closure) argument, [env]. */

extern void app(`b f(`a),set_t<`a> s);
  /** [app(f,s)] applies [f] to each element of [s], in no particular
      order; the result of the application is discared.  Notice that
      [f] cannot return [void]; use [iter] instead of [app] for
      that. */
extern void iter(void f(`a),set_t<`a> s);
  /** [iter(f,s)] is like [app(f,s)], except that [f] must return
      [void]. */
extern void iter_c(void f(`c,`a),`c env,set_t<`a> s);
  /** [iter_c] is a version of [iter] where the function argument [f]
      requires a closure. */

extern xtunion exn {extern Absent};
  /** [Absent] is an exception thrown by the [choose] function. */

extern `a choose(set_t<`a> s);
  /** [choose(s)] returns some element of the set [s]; if the set is
      empty, [choose] throws [Absent]. */

}

#endif
