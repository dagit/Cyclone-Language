/* This file is part of the Cyclone Library.
   Copyright (C) 2001 Greg Morrisett, AT&T

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


#ifndef _FN_H_
#define _FN_H_
////////////////////////////////////////////////////////////////////////////
// Defines "closures" and operations on them                              //
////////////////////////////////////////////////////////////////////////////
#include <list.h>

namespace Fn {

extern tunion Function<`arg,`res,`eff::E> {
  Fun<`env>(`res (@)(`env,`arg;`eff), `env)
};

typedef tunion Function<`arg,`res,`eff> fn<`arg,`res,`eff>;

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
