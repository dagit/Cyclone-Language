#ifndef _QUEUE_H_
#define _QUEUE_H_
////////////////////////////////////////////////////////////////////////////
// Popcorn library, file queue.h                                          //
// Copyright Greg Morrisett, Dan Grossman                                 //
// January 1999, all rights reserved                                      //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

namespace Queue {

// queue.h:  defines generic imperative queues and various operations
//           following the conventions of the Ocaml queue library as much
//           as possible.  

extern struct Queue<`a,`r::R>;
typedef struct Queue<`a,`r> @gqueue_t<`a,`r>;
typedef struct Queue<`a,`H> @queue_t<`a>;

// true when the queue is empty
extern bool is_empty(gqueue_t<`a,`r>);

// raised by queue_take and queue_peek
extern xenum exn { Empty };

// create a new queue
extern queue_t<`a> create();

// insert an element into the rear of the queue (side effect)
extern void add(queue_t<`a>,`a x);

// get and remove an element from the front of the queue (side effect)
extern `a take(gqueue_t<`a,`r>);

// return the first element in the queue without removing it
extern `a peek(gqueue_t<`a,`r>);

// clear out the entire queue (side effect)
extern void clear(gqueue_t<`a,`r>);

// return the number of lements in the queue
extern int length(gqueue_t<`a,`r>);

// apply f to each element in the queue from the front to the back
extern void iter(void f(`a), gqueue_t<`a,`r>);
extern void app(`b f(`a), gqueue_t<`a,`r>);

}

#endif
