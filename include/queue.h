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

namespace Queue {

// queue.h:  defines generic imperative queues and various operations
//           following the conventions of the Ocaml queue library as much
//           as possible.  

extern struct Queue<`a,`r::R>;
typedef struct Queue<`a,`r> @`r queue_t<`a,`r>;

// true when the queue is empty
extern bool is_empty(queue_t);

// raised by queue_take and queue_peek
extern xtunion exn { extern Empty };

// create a new queue
extern queue_t create();

// insert an element into the rear of the queue (side effect)
extern void add(queue_t<`a,`H>,`a x);
extern void radd(region_t<`r>, queue_t<`a,`r>,`a x);

// get and remove an element from the front of the queue (side effect)
extern `a take(queue_t<`a>);

// return the first element in the queue without removing it
extern `a peek(queue_t<`a>);

// clear out the entire queue (side effect)
extern void clear(queue_t<`a>);

// return the number of lements in the queue
extern int length(queue_t<`a>);

// apply f to each element in the queue from the front to the back
extern void iter(void f(`a), queue_t<`a>);
extern void app(`b f(`a), queue_t<`a>);

}

#endif
