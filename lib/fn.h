#ifndef FN_H
#define FN_H
////////////////////////////////////////////////////////////////////////////
// Cylone library, file fn.cyc                                            //
// Copyright Greg Morrisett, Trevor Jim                                   //
// September 2000, all rights reserved                                    //
//                                                                        //
// Defines "closures" and operations on them                              //
////////////////////////////////////////////////////////////////////////////
#include "list.h"

namespace Fn {

extern enum Function<`arg,`res> {
  Fun<`env>(`res (@)(`env,`arg), `env)
};

typedef enum Function<`arg,`res> fn<`arg,`res>;

// make a closure out of a function pointer and environment
extern fn<`arg,`res> make_fn<`arg,`res,`env>(`res f(`env,`arg), `env x);

// convert a function pointer to a closure
extern fn<`arg,`res> fp2fn<`arg,`res>(`res f(`arg));

// apply closure f to argument x
extern `res apply<`arg,`res>(fn<`arg,`res> f, `arg x);

// compose closures
extern fn<`a,`c> compose<`a,`b,`c>(fn<`a,`b> g, fn<`b,`c> f);

// curry a closure that takes a pair
extern fn<`a,fn<`b,`c>> curry<`a,`b,`c>(fn<$(`a,`b)@,`c> f);

// uncurry a closure
extern fn<$(`a,`b)@,`c> uncurry<`a,`b,`c>(fn<`a,fn<`b,`c>> f);

// map a closure across a list
extern List::list<`b> map_fn<`a,`b>(fn<`a,`b> f,List::list<`a> x);
}
#endif
