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

#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <core.h> // for bool
#include <list.h> // for list_t

namespace Queue {

  /*** \subsection{\texttt{<queue.h>}} */
  /*** Defines namespace Queue, which implements generic imperative
       queues and various operations following the conventions of the
       Objective Caml queue library as much as possible.  */

extern struct Queue<`a::TB,`r::R>;
typedef struct Queue<`a,`r> @`r queue_t<`a,`r>;
  /** A value of type [queue_t<`a,`r>] is a first-in, first-out queue
      of elements of type [`a]; the queue data structures are
      allocated in region [`r]. */

extern bool is_empty(queue_t<`a::TB>);
  /** [is_empty(q)] returns true if [q] contains no elements, and
      returns false otherwise. */

extern queue_t<`a::TB> create();
  /** [create()] allocates a new, empty queue on the heap and returns it. */

extern void add(queue_t<`a::TB,`H>,`a x) __attribute__((consume(2)));
  /** [add(q,x)] adds [x] to the end of [q] (by side effect). */
extern void radd(region_t<`r>, queue_t<`a::TB,`r>,`a x) __attribute__((consume(3)));
  /** [radd(r,q,x)] is like [add(q,x)] except that the queue lives in
      the region with handle [r]. */

extern void push(queue_t<`a::TB,`H> q, `a x) __attribute__((consume(2)));
  /** [push(q,x)] adds [x] to the front of [q] (by side effect). */
extern void rpush(region_t<`r> r, queue_t<`a::TB,`r> q,`a x) __attribute__((consume(3)));
  /** [rpush(r,q,x)] is like [push(q,x)] except that the queue lives in
      the region with handle [r]. */

extern datatype exn { extern Empty };
  /** [Empty] is an exception raised by [take] and [peek]. */
extern `a take(queue_t<`a>);
  /** [take(q)] removes the element from the front on [q] and returns
      it; if [q] is empty, exception [Empty] is thrown. */

extern `a peek(queue_t<`a>);
  /** [peek(q)] returns the element at the front of [q], without
      removing it from [q].  If [q] is empty, exception [Empty] is
      thrown. */

extern void clear(queue_t<`a::TB>);
  /** [clear(q)] removes all elements from [q]. */

extern void remove(queue_t<`a>,`a);
  /** [remove(q,x)] removes the first occurence of element [x] from [q], 
      if present. **/

extern int length(queue_t<`a::TB>);
  /** [length(q)] returns the number of elements in [q]. */

extern void iter(void f(`a), queue_t<`a>);
  /** [iter(f,q)] applies [f] to each element of [q], from first to
      last.  Note that [f] must return [void]. */
extern void app(`b f(`a), queue_t<`a>);
  /** [app(f,q)] applies [f] to each element of [q], from first to
      last.  Note that [f] must return a value of kind [M]. */

  /*** The following procedures are specialized to work with
       no-aliasable and/or unique pointers. */

extern `a *`U take_match(region_t<`r> r, queue_t<`a::TA *`U,`r> q,
			 bool (@f)(`b,`a *`U),
			 `b env);
  /** [take_match(r,q,f,c)] looks through the queue (starting from the
      front) and returns the element [x] for which [f(x,c)] returns
      true. */

extern `a noalias_take(queue_t<`a::TB> q, `a null_elem) __attribute__((consume(2)));
  /** [noalias_take(q)] is as take, above, but works when the queue
      contains potentially-unique elements; the caller needs to supply
      a 'null' element to swap with the element in the first spot in
      the queue. */
extern `a *`U ptr_take(queue_t<`a::TA *`U> q);
  /** [ptr_take(q)] is a wrapper for [noalias_take(q,NULL)]. */

}

#endif
