#ifndef QUEUE_H
#define QUEUE_H
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

extern struct queue<`a>;
typedef struct queue<`a> @Queue<`a>;

// true when the queue is empty
extern bool is_empty<`a>(Queue<`a>);

// raised by queue_take and queue_peek
extern xenum exn { Empty };

// create a new queue
extern Queue<`a> create<`a>();

// insert an element into the rear of the queue (side effect)
extern void add<`a>(Queue<`a>,`a x);

// get and remove an element from the front of the queue (side effect)
extern `a take<`a>(Queue<`a>);

// return the first element in the queue without removing it
extern `a peek<`a>(Queue<`a>);

// clear out the entire queue (side effect)
extern void clear<`a>(Queue<`a>);

// return the number of lements in the queue
extern int length<`a>(Queue<`a>);

// apply f to each element in the queue from the front to the back
extern void iter<`a>(void f(`a), Queue<`a>);
extern void app<`a,`b>(`b f(`a), Queue<`a>);

}

#endif
