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

#ifndef _DICT_H_
#define _DICT_H_

#include <cycboot.h>
#include <list.h>
#include <iter.h>

namespace Dict {

  /*** \subsection{\texttt{<dict.h>}}*/
  /*** Defines namespace Dict, which implements dictionaries: mappings
       from keys to values.  The dictionaries are implemented
       functionally: adding a mapping to an existing dictionary
       produces a new dictionary, without affecting the existing
       dictionary.  To enable an efficient implementation, you are
       required to provide a total order on keys (a comparison
       function).

       We follow the conventions of the Objective Caml Dict library as
       much as possible.

       Namespace Dict implements a superset of namespace SlowDict,
       except that [delete_present] is not supported. */

using List;

extern struct T<`a::B,`b::B,`r::R>;
typedef struct T<`a,`b,`r> const *`r tree<`a,`b,`r>;
extern struct Dict<`a,`b,`r::R> {
  int (@`H rel)(`a,`a);
  region_t<`r> r;
  tree<`a,`b,`r> t;
};
typedef struct Dict<`a,`b,`r> dict_t<`a,`b,`r>;
/** A value of type [dict_t<`a,`b,`r>] is a dictionary that maps
    keys of type [`a] to values of type [`b]; the dictionary
    datatypes live in region [`r]. */

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
extern dict_t<`a,`b,`r> rempty(region_t<`r>,int (@`H cmp)(`a,`a));
/** [rempty(r,cmp)] is like [empty(cmp)] except that the dictionary is
    allocated in the region with handle [r]. */

extern dict_t<`a,`b,`r> rshare(region_t<`r>,dict_t<`a,`b,`r2> : {`r2} > `r);
/** [rshare(r,d)] creates a virtual copy in region [`r] of the dictionary 
    [d] that lives in region [`r2].  The internal data structures of the
    new dictionary share with the old one. */

extern bool is_empty(dict_t d);
/** [is_empty(d)] returns true if [d] is empty, and returns false otherwise. */

extern int cardinality(dict_t d);
/** [cardinality(d)] returns the number of keys in the dictionary. */

extern bool member(dict_t<`a> d,`a k);
/** [member(d,k)] returns true if [k] is mapped to some value in [d],
    and returns false otherwise. */

extern dict_t<`a,`b,`r> insert(dict_t<`a,`b,`r> d,`a k,`b v);
/** [insert(d,k,v)] returns a dictionary with the same mappings as
    [d], except that [k] is mapped to [v].  The dictionary [d] is not
    modified. */

extern dict_t<`a,`b,`r> insert_new(dict_t<`a,`b,`r> d,`a k,`b v);
/** [insert_new(d,k,v)] is like [insert(d,k,v)], except that it throws
    [Present] if [k] is already mapped to some value in [d]. */

extern dict_t<`a,`b,`r> inserts(dict_t<`a,`b,`r> d,
                                list_t<$(`a,`b)@> l);
/** [inserts(d,l)] inserts each key, value pair into [d], returning
    the resulting dictionary. */

extern dict_t<`a,`b> singleton(int (@`H cmp)(`a,`a),`a k,`b v);
/** [singleton(cmp,k,v)] returns a new heap-allocated dictionary with
    a single mapping, from [k] to [v]. */
extern dict_t<`a,`b,`r> rsingleton(region_t<`r>,
                                   int (@`H cmp)(`a,`a),`a k,`b v);
/** [rsingleton(r,cmp,k,v)] is like [singleton(cmp,k,v)], except the
    resulting dictionary is allocated in the region with handle
    [r]. */

extern `b lookup(dict_t<`a,`b> d,`a k);
/** [lookup(d,k)] returns a pointer to the value associated with 
    key [k] in [d], or throws [Absent] if [k] is not mapped to any value. */
extern `b lookup_other(dict_t<`a,`b> d, int cmp(`c,`a), `c k);
/** [lookup_other(d,cmp,k)] returns a pointer to the value associated with 
    key [k] in [d], or throws [Absent] if [k] is not mapped to any value. 
    The comparison function associated with the dictionary is ignored and
    instead, the [cmp] argument is used.  Note that [cmp] must respect
    the same ordering constraints as the dictionary's built-in comparison
    in order to succeed.  This is useful when the dictionary has keys that
    are pointers into one region, but you want to look up with a key that
    is a pointer into another region. */
extern `b*`r lookup_opt(dict_t<`a,`b,`r> d,`a k);
/** [lookup_opt(d,k)] returns [NULL] if [k] is not mapped to any value
    in [d], and returns a non-NULL, heap-allocated option containing
    the value [k] is mapped to in [d] otherwise. */
extern bool lookup_bool(dict_t<`a,`b> d, `a k, `b @ans);
/** If [d] maps [k] to a value, then [lookup_bool(d,k,ans)] assigns
    that value to [*ans] and returns true; otherwise, it returns
    false. */

extern `c fold(`c f(`a,`b,`c),dict_t<`a,`b> d,`c accum);
/** If [d] has keys [k1] through [kn] mapping to values [v1] through
    [vn], then [fold(f,d,accum)] returns
    [f(k1,v1,...f(kn,vn,accum)...)]. */
extern `c fold_c(`c f(`d,`a,`b,`c),`d env,dict_t<`a,`b> d,`c accum);
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

extern void iter2(void (@f)(`b,`b),dict_t<`a,`b> d1,dict_t<`a,`b> d2);
/** For every key [k] in the domain of both [d1] and [d2],
    [iter2(f,d1,d2)] performs [f(lookup(d1,k), lookup(d2,k))].  If
    there is any key present in [d1] but not [d2], then [Absent] is
    thrown. */
extern void iter2_c(void (@f)(`c,`b,`b), `c env,
		    dict_t<`a,`b> d1, dict_t<`a,`b> d2);
/** [iter2_c] is like [iter] except that [f] takes a closure as its
    first argument. */
extern `c fold2_c(`c (@f)(`d,`a,`b1,`b2,`c), `d env,
		  dict_t<`a,`b1> d1, dict_t<`a,`b2> d2, `c accum);
/** If [k1] through [kn] are the keys of [d1], then
    [fold2_c(f,env,d1,d2,accum)] returns
    [f(env,k1,lookup(k1,d1),lookup(k1,d2), ...
    f(env,kn,lookup(kn,d1),lookup(kn,d2),accum)...)]. If there is any
    key present in [d1] but not [d2], then [Absent] is thrown. */

extern dict_t<`a,`b,`r> rcopy(region_t<`r>, dict_t<`a,`b>);
/** [rcopy(r,d)] returns a copy of [d], newly allocated in the region
    with handle [r]. */
extern dict_t<`a,`b> copy(dict_t<`a,`b>);
/** [copy(r,d)] returns a copy of [d], newly allocated on the heap. */

extern dict_t<`a,`c> map(`c f(`b),dict_t<`a,`b> d);
/** [map(f,d)] applies [f] to each value in [d], and returns a new
    dictionary with the results as values: for every binding of a key
    [k] to a value [v] in [d], the result binds [k] to [f(v)].  The
    returned dictionary is allocated on the heap.  */
extern dict_t<`a,`c,`r> rmap(region_t<`r>, `c f(`b), dict_t<`a,`b> d);
/** [rmap(r,f,d)] is like [map(f,d)], except the resulting dictionary
    is allocated in the region with handle [r]. */
extern dict_t<`a,`c> map_c(`c f(`d,`b),`d env,dict_t<`a,`b> d);
/** [map_c(f,env,d)] is like [map(f,d)] except that [f] takes
    [env] as its first argument. */
extern dict_t<`a,`c,`r> rmap_c(region_t<`r>, `c f(`d,`b),`d env,
                               dict_t<`a,`b> d);
/** [rmap_c(r,f,env,d)] is like [map_c(f,env,d)] except that the
    resulting dictionary is allocated in the region with handle
    [r]. */

extern dict_t<`a,`b,`r> union_two_c(`b (@f)(`c,`a,`b,`b), `c env,
                                   dict_t<`a,`b,`r> d1,
                                   dict_t<`a,`b,`r> d2);
/** [union_two_c(f,env,d1,d2)] returns a new dictionary with a binding
    for every key in [d1] or [d2].  If a key appears in both [d1] and
    [d2], its value in the result is obtained by applying [f] to the
    two values, the key, and env.  Note that the resulting dictionary
    is allocated in the same region as [d2].  (We don't use [union] as
    the name of the function, because [union] is a Cyclone keyword.) */

extern dict_t<`a,`b,`r> intersect(`b (@f)(`a,`b,`b),
                                  dict_t<`a,`b,`r> d1,
                                  dict_t<`a,`b,`r> d2);
/** [intersect(f,d1,d2)] returns a new dictionary with a binding for
    every key in both [d1] and [d2].  For every key appearing in both
    [d1] and [d2], its value in the result is obtained by applying [f]
    to the key and the two values.  Note that the input dictionaries and result
    must be allocated in the same region. */

extern dict_t<`a,`b,`r> intersect_c(`b (@f)(`c,`a,`b,`b), `c env,
                                     dict_t<`a,`b,`r> d1,
                                     dict_t<`a,`b,`r> d2);
/** [intersect_c(f,env,d1,d2)] is like [intersect(f,d1,d2)], except
    that [f] takes [env] as its first argument. */

extern bool forall_c(bool f(`c,`a,`b), `c env, dict_t<`a,`b> d);
/** [forall_c(f,env,d)] returns true if [f(env,k,v)] returns true for
    every key [k] and associated value [v] in [d], and returns false
    otherwise. */
extern bool forall_intersect(bool f(`a,`b,`b), dict_t<`a,`b> d1,
			     dict_t<`a,`b> d2);
/** [forall_intersect(f,d1,d2)] returns true if [f(k,v1,v2)] returns
    true for every key [k] appearing in both [d1] and [d2], where [v1]
    is the value of [k] in [d1], and [v2] is the value of [k] in [d2];
    and it returns false otherwise.  */

extern $(`a,`b)@`r rchoose(region_t<`r>,dict_t<`a,`b> d);
/** [rchoose(r,d)] returns a key/value pair from [d], allocating the pair in
    region [r]; if [d] is empty, [Absent] is thrown. */

extern list_t<$(`a,`b)@> to_list(dict_t<`a,`b> d);
/** [to_list(d)] returns a list of the key/value pairs in [d],
    allocated on the heap. */
extern list_t<$(`a,`b)@`r,`r> rto_list(region_t<`r>,dict_t<`a,`b> d);
/** [rto_list(r,d)] is like [to_list(d)], except that the resulting
    list is allocated in the region with handle [r]. */

extern dict_t<`a,`b> filter(bool f(`a,`b), dict_t<`a,`b> d);
/** [filter(f,d)] returns a dictionary that has a binding of [k] to
    [v] for every binding of [k] to [v] in [d] such that [f(k,v)]
    returns true.  The resulting dictionary is allocated on the heap. */
extern dict_t<`a,`b,`r> rfilter(region_t<`r>, bool f(`a,`b), dict_t<`a,`b> d);
/** [rfilter(r,f,d)] is like [filter(f,d)], except that the resulting
    dictionary is allocated in the region with handle [r]. */
extern dict_t<`a,`b> filter_c(bool f(`c,`a,`b), `c env, dict_t<`a,`b> d);
/** [filter_c(f,env,d)] is like [filter(f,d)] except that [f] takes a
    closure [env] as its first argument. */
extern dict_t<`a,`b,`r> rfilter_c(region_t<`r>,
                                  bool f(`c,`a,`b), `c env,
                                  dict_t<`a,`b> d);
/** [rfilter_c(r,f,env,d)] is like [filter_c(f,env,d)], except that
    the resulting dictionary is allocated in the region with handle
    [r]. */

extern dict_t<`a,`b> difference(dict_t<`a,`b> d1, dict_t<`a,`b> d2);
/** [difference(d1,d2)] returns a dictionary that has a binding of [k]
    to [v] for every binding of [k] to [v] in [d1] where [k] is not in
    [d2].  (Note that the values of [d2] are not relevant to
    [difference(d1,d2)].)  The resulting dictionary is allocated on
    the heap. */
extern dict_t<`a,`b,`r> rdifference(region_t<`r>,
                                    dict_t<`a,`b> d1,
                                    dict_t<`a,`b> d2);
/** [rdifference(d1,d2)] is like [difference(d1,d2)], except that the
    resulting dictionary is allocated in the region with handle
    [r]. */

extern dict_t<`a,`b> delete(dict_t<`a,`b>, `a);
/** [delete(d,k)] returns a dictionary with the same bindings as [d],
    except that any binding of [k] is removed.  The resulting
    dictionary is allocated on the heap. */
extern dict_t<`a,`b,`r> rdelete(region_t<`r>, dict_t<`a,`b>, `a);
/** [rdelete(r,d,k)] is like [delete(d,k)] except that the result is
    allocated in the region with handle [r]. */
extern dict_t<`a,`b,`r> rdelete_same(dict_t<`a,`b,`r>, `a);
/** [rdelete_same(d,k)] is like [delete(d,k)], except that the
    resulting dictionary is allocated in the same region as the input
    dictionary [d].  This can be faster than [delete(d,k)] because it
    avoids a copy when [k] is not a member of [d]. */

extern Iter::iter_t<$(`a,`b),`bd> make_iter(region_t<`r1> rgn, 
					    dict_t<`a,`b,`r2> d
					    : regions($(`a,`b)) > `bd,
                                              {`r1,`r2} > `bd);
/** [make_iter(rgn,d)] returns an iterator over the dictionary [d];
    O(log n) space is allocated in [rgn] where n is the number of
    elements in [d]. */

extern `c marshal(region_t<`r> rgn,
		  `c env,
		  `c (write_key<`a,`c,`r1::R>)(`c, FILE @`r1, `a),
		  `c (write_val<`b,`c,`r1::R>)(`c, FILE @`r1, `b),
		  FILE @ fp,
		  dict_t<`a,`b> dict);

extern dict_t<`a,`b> unmarshal(region_t<`r> rgn,
			       `c::A @`H env,
			       int (@`H cmp)(`a,`a),
			       `a (read_key<`a,`c::A>)(`c @`H, FILE @`r2),
			       `b (read_val<`b,`c::A>)(`c @`H, FILE @`r2),
			       FILE @`r2 fp);

}
#endif
