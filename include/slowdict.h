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

#ifndef _SLOWDICT_H_
#define _SLOWDICT_H_

#include <core.h>
#include <list.h>

namespace SlowDict {

  /*** \subsection{\texttt{<slowdict.h>}}*/
  /*** Defines namespace SlowDict, which implements polymorphic,
       functional, finite maps whose domain must have a total order.
       We follow the conventions of the Objective Caml Dict library as
       much as possible.

       The basic functionality is the same as Dict, except that
       SlowDict supports [delete_present]; but region support still
       needs to be added, and some functions are missing, as well. */

using List;

extern struct Dict<`a,`b>;
typedef struct Dict<`a,`b> @ dict_t<`a,`b>;
/** A value of type [dict_t<`a,`b>] is a dictionary that maps
    keys of type [`a] to values of type [`b]. */

extern datatype exn {extern Present};
/** [Present] is thrown when a key is present but not expected. */
extern datatype exn {extern Absent};
/** [Absent] is thrown when a key is absent but should be present. */

extern dict_t<`a,`b> empty(int (@`H cmp)(`a,`a));
/** [empty(cmp)] returns an empty dictionary, allocated on the
    heap. [cmp] should be a comparison function on keys: [cmp(k1,k2)]
    should return a number less than, equal to, or greater than 0
    according to whether [k1] is less than, equal to, or greater than
    [k2] in the ordering on keys. */

extern bool is_empty(dict_t d);
/** [is_empty(d)] returns true if [d] is empty, and returns false
    otherwise. */

extern bool member(dict_t<`a> d,`a k);
/** [member(d,k)] returns true if [k] is mapped to some value in [d],
    and returns false otherwise. */

extern dict_t<`a,`b> insert(dict_t<`a,`b> d,`a k,`b v);
/** [insert(d,k,v)] returns a dictionary with the same mappings as
    [d], except that [k] is mapped to [v].  The dictionary [d] is not
    modified. */

extern dict_t<`a,`b> insert_new(dict_t<`a,`b> d,`a k,`b v);
/** [insert_new(d,k,v)] is like [insert(d,k,v)], except that it throws
    [Present] if [k] is already mapped to some value in [d]. */

extern dict_t<`a,`b> inserts(dict_t<`a,`b> d,list_t<$(`a,`b)@> l);
/** [inserts(d,l)] inserts each key, value pair into [d], returning
    the resulting dictionary. */

extern dict_t<`a,`b> singleton(int (@`H cmp)(`a,`a),`a k,`b v);
/** [singleton(cmp,k,v)] returns a new heap-allocated dictionary with
    a single mapping, from [k] to [v]. */

extern `b lookup(dict_t<`a,`b> d,`a k);
/** [lookup(d,k)] returns the value associated with key [k] in [d], or
    throws [Absent] if [k] is not mapped to any value. */
extern Core::opt_t<`b> lookup_opt(dict_t<`a,`b> d,`a k);
/** [lookup_opt(d,k)] returns [NULL] if [k] is not mapped to any value
    in [d], and returns a non-NULL, heap-allocated option containing
    the value [k] is mapped to in [d] otherwise. */

extern dict_t<`a,`b> delete(dict_t<`a,`b> d,`a k);
/** [delete(d,k)] returns a dictionary with the same bindings as [d],
    except that any binding of [k] is removed.  The resulting
    dictionary is allocated on the heap. */

extern dict_t<`a,`b> delete_present(dict_t<`a,`b> d,`a k);
  /** [delete_present(d,k)] is like [delete(d,k)], except that
      [Absent] is thrown if [k] has no binding in [d]. */

extern `c fold(`c f(`a,`b,`c),dict_t<`a,`b> d,`c accum);
/** If [d] has keys [k1] through [kn] mapping to values [v1] through
    [vn], then [fold(f,d,accum)] returns
    [f(k1,v1,...f(kn,vn,accum)...)]. */
extern `c fold_c(`c f(`d,`a,`b,`c),`d env, dict_t<`a,`b> d,`c accum);
/** [fold_c(f,env,d,accum)] is like [fold(f,d,accum)] except that [f]
    takes closure [env] as its first argument. */

extern void app(`c f(`a,`b),dict_t<`a,`b> d);
/** [app(f,d)] applies [f] to every key/value pair in [d]; the results
    of the applications are discarded.  Note that [f] cannot return
    [void]. */
extern void app_c(`c f(`d,`a,`b),`d env,dict_t<`a,`b> d);
/** [app_c(f,env,d)] is like [app(f,d)] except that [f] takes closure
    [env] as its first argument. */

extern void iter(void f(`a,`b),dict_t<`a,`b> d);
/** [iter(f,d)] is like [app(f,d)] except that [f] returns [void]. */
extern void iter_c(void f(`c,`a,`b),`c env,dict_t<`a,`b> d);
/** [iter_c(f,env,d)] is like [app_c(f,env,d)] except that [f] returns
    [void]. */

extern dict_t<`a,`c> map(`c f(`b),dict_t<`a,`b> d);
/** [map(f,d)] applies [f] to each value in [d], and returns a new
    dictionary with the results as values: for every binding of a key
    [k] to a value [v] in [d], the result binds [k] to [f(v)].  The
    returned dictionary is allocated on the heap.  */
extern dict_t<`a,`c> map_c(`c f(`d,`b),`d env,dict_t<`a,`b> d);
/** [map_c(f,env,d)] is like [map(f,d)] except that [f] takes a
    closure [env] as its first argument. */

extern $(`a,`b)@ choose(dict_t<`a,`b> d);
/** [choose(d)] returns a key/value pair from [d]; if [d] is empty,
    [Absent] is thrown.  The resulting pair is allocated on the
    heap. */

extern list_t<$(`a,`b)@> to_list(dict_t<`a,`b> d);
/** [to_list(d)] returns a list of the key/value pairs in [d],
    allocated on the heap. */

}
#endif
