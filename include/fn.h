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

#include <list.h>

namespace Fn {

  /*** \subsection{\texttt{<fn.h>}} */
  /*** Defines namespace Fn, which implements closures: a way to
       package up a function with some hidden data (an environment).
       Many of the library functions taking function arguments have
       versions for functions that require an explicit environment;
       the closures of namespace Fn are different, they combine the
       function and environment, and the environment is hidden.  They
       are useful when two functions need environments of different
       type, but you need them to have the same type; you can do this
       by hiding the environment from the type of the pair. */

extern struct Function<`arg,`res,`bd::R> { <`env::B> : regions(`env) > `bd
  `res (@`bd f)(`env,`arg);
  `env env;
};

typedef struct Function<`arg,`res,`bd> @fn_t<`arg,`res,`bd>;
  /** A value of type [fn_t<`arg,`res,`eff>] is a function and its
      closure; [`arg] is the argument type of the function, [`res] is
      the result type, and [`bd] is a region that [regions(`arg)] outlive. */

extern fn_t<`arg,`res,`bd> make_fn(`res (@`bd f)(`env,`arg),`env x :regions(`env)>`bd);
  /** [make_fn(f,env)] builds a closure out of a function and an environment. */

extern fn_t<`arg,`res,`bd> fp2fn(`res (@`bd f)(`arg) : regions($(`arg,`res)) > `bd);
  /** [fp2fn(f)] converts a function pointer to a closure. */

extern `res apply(fn_t<`arg,`res> f, `arg x);
  /** [apply(f,x)] applies closure [f] to argument [x] (taking care of
      the hidden environment in the process). */

extern fn_t<`a,`c,`bd> compose(fn_t<`a,`b,`bd> g, fn_t<`b,`c,`bd> f 
			       : regions($(`a,`b,`c)) > `bd);
  /** [compose(g,f)] returns the composition of closures [f] and [g];
      [apply(compose(g,f),x)] has the same effect as
      [apply(f,apply(g,x))]. */

extern fn_t<`a,fn_t<`b,`c,`bd>,`bd> curry(fn_t<$(`a,`b)@`H,`c,`bd> f 
					  : regions($(`a,`b,`c)) > `bd);
  /** [curry(f)] curries a closure that takes a pair as argument: if
      [x] points to a pair [\$(x1,x2)], then [apply(f,x)] has the same
      effect as [apply(apply(curry(f),x1),x2)].  */

extern fn_t<$(`a,`b)@,`c,`bd> uncurry(fn_t<`a,fn_t<`b,`c,`bd>,`bd> f
				      : regions($(`a,`b,`c)) > `bd);
  /** [uncurry(f)] converts a closure that takes two arguments in
      sequence into a closure that takes the two arguments as a pair:
      if [x] points to a pair [\$(x1,x2)], then [apply(uncurry(f),x)]
      has the same effect as [apply(apply(f,x1),x2)]. */

extern List::list_t<`b> map_fn(fn_t<`a,`b> f,List::list_t<`a> x);
  /** [map_fn(f,x)] maps the closure [f] on the list [x]: if [x] has
      elements [x1] through [xn], then [map_fn(f,x)] returns a new
      heap-allocated list with elements [apply(f,x1)] through
      [apply(f,xn)]. */
}
#endif
