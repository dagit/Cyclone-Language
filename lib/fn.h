#ifndef _FN_H_
#define _FN_H_
////////////////////////////////////////////////////////////////////////////
// Cylone library, file fn.cyc                                            //
// Copyright Greg Morrisett, Trevor Jim                                   //
// September 2000, all rights reserved                                    //
//                                                                        //
// Defines "closures" and operations on them                              //
////////////////////////////////////////////////////////////////////////////
#include "list.h"

namespace Fn {

extern enum Function<`arg,`res,`eff> {
  Fun<`env>(`res (@)(`env,`arg;`eff), `env)
};

typedef enum Function<`arg,`res,`eff> fn<`arg,`res,`eff>;

// make a closure out of a function pointer and environment
extern fn<`arg,`res,`e1> make_fn(`res f(`env,`arg;`e1), `env x; `e2);

// convert a function pointer to a closure
extern fn<`arg,`res,`e1> fp2fn(`res f(`arg;`e1));

// apply closure f to argument x
extern `res apply(fn<`arg,`res,`eff> f, `arg x; `eff);

// compose closures
extern fn<`a,`c,`e1+`e2> compose<`a,`b,`c,`e1,`e2,`e3>(fn<`a,`b,`e1> g, 
                                                       fn<`b,`c,`e2> f;
                                                       `e1+`e2+`e3);

// curry a closure that takes a pair
extern fn<`a,fn<`b,`c,`e1>,`e1+`e2> curry(fn<$(`a,`b)@,`c,`e1> f);

// uncurry a closure
extern fn<$(`a,`b)@,`c,`e1+`e2> uncurry(fn<`a,fn<`b,`c,`e1>,`e2> f);

// map a closure across a list
extern List::list_t<`b> map_fn(fn<`a,`b,`e> f,List::glist_t<`a,`r> x);
}
#endif
