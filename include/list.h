/* This file is part of the Cyclone Library.
   Copyright (C) 2000-2001 Greg Morrisett, AT&T

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

#ifndef _LIST_H_
#define _LIST_H_

#include <core.h>

namespace List {

  /*** \subsection{\texttt{<list.h>}} */
  /*** Defines namespace List, which implements generic lists and
       various operations over them, following the conventions of the
       Objective Caml list library as much as possible. */

  struct List<`a,`r>{`a hd; struct List<`a,`r> *`r tl;};
  /** A [struct List] is a memory cell with a head field containing an
      element and a tail field that points to the rest of the list.
      Such a structure is traditionally called a cons cell.  Note that
      every element of the list must have the same type [`a], and
      every cons cell in the list must be allocated in the same region
      [`r].  */

  typedef struct List<`a,`r> *`r list_t<`a,`r>;
  /** A [list_t] is a possibly-NULL pointer to a [struct List].  Most
      of the functions in namespace List operate on values of type
      [list_t] rather than [struct List].  Note that a [list_t] can be
      empty (NULL) but a [struct List] cannot. */

//   typedef struct List<`a,`r::TR> *`r trlist_t<`a,`r>;
//   /** A [trlist_t] is the same as a list_t except that the list can
//       reside in either a normal region or the unique region.  */

  typedef struct List<`a,`r> @`r List_t<`a,`r>;
  /** A [List_t] is a non-NULL pointer to a [struct List].  This is
      used much less often than [list_t], however it may be useful
      when you want to emphasize that a list has at least one element.  */

  extern list_t<`a> list(... `a);
  /** [list(x1,...,xn)] builds a heap-allocated list with elements
      [x1] through [xn]. */
  extern list_t<`a,`r> rlist(region_t<`r::TR>,... `a);
  /** [rlist(r, x1,...,xn)] builds a list with elements [x1] through
      [xn], allocated in the region with handle [r]. */

  extern int length(list_t<`a::TB> x);
  /** [length(x)] returns the number of elements in list [x].  */

  extern `a hd(List_t<`a> x);
  /** [hd(x)] returns the first element of list [x]. */

  extern list_t<`a,`r> tl(List_t<`a::TB,`r> x);
  /** [tl(x)] returns the tail of list [x]. */

  extern list_t<`a> copy(list_t<`a> x);
  /** [copy(x)] returns a new heap-allocated copy of list [x]. */
  extern list_t<`a,`r> rcopy(region_t<`r>, list_t<`a> x);
  /** [rcopy(r,x)] returns a new copy of list [x], allocated in the
      region with handle [r]. */

  extern list_t<`b> map(`b f(`a),list_t<`a> x);
  /** If [x] has elements [x1] through [xn], then [map(f,x)] returns
      [list(f(x1),...,f(xn))].  */
  extern list_t<`b,`r> rmap(region_t<`r>,`b f(`a),list_t<`a> x);
  /** If [x] has elements [x1] through [xn], then [rmap(r,f,x)] returns
      [rlist(r,f(x1),...,f(xn))].  */

  extern list_t<`b> map_c(`b f(`c,`a),`c env,list_t<`a> x);
  /** [map_c] is a version of [map] where the function argument
      requires a closure as its first argument. */
  extern list_t<`b,`r> rmap_c(region_t<`r>,`b f(`c,`a),`c env, list_t<`a> x);
  /** [rmap_c] is a version of [rmap] where the function argument
      requires a closure as its first argument. */

  extern datatype exn {extern List_mismatch};
  /** [List_mismatch] is thrown when two lists don't have the same
      length. */

  extern list_t<`c> map2(`c f(`a,`b),list_t<`a> x,list_t<`b> y);
  /** If [x] has elements [x1] through [xn], and [y] has elements [y1]
      through [yn], then [map2(f,x,y)] returns a new heap-allocated
      list with elements [f(x1,y1)] through [f(xn,yn)].  If [x] and
      [y] don't have the same number of elements, [List_mismatch] is
      thrown. */
  extern list_t<`c,`r> rmap2(region_t<`r>,`c f(`a,`b),
                               list_t<`a> x,list_t<`b> y);
  /** [rmap2(r,f,x,y)] is like [map2(f,x,y)], except that the
      resulting list is allocated in the region with handle [r]. */
  extern list_t<`d> map3(`d f(`a,`b,`c),
                         list_t<`a> x,list_t<`b> y,list_t<`c> z);
  /** If [x] has elements [x1] through [xn], [y] has elements [y1]
      through [yn], and [z] has elements [z1] through [zn], then 
      [map3(f,x,y,z)] returns a new heap-allocated list with elements 
      [f(x1,y1,z1)] through [f(xn,yn,zn)].  If [x], [y], and
      [z] don't have the same number of elements, [List_mismatch] is
      thrown. */
  extern list_t<`d,`r> rmap3(region_t<`r>,`d f(`a,`b,`c),
                             list_t<`a> x,list_t<`b> y,list_t<`c> z);
  /** [rmap3(r,f,x,y,z)] is like [map3(f,x,y,z)], except that the
      resulting list is allocated in the region with handle [r]. */

  extern void app(`b f(`a),list_t<`a> x);
  /** [app(f,x)] applies [f] to each element of [x], discarding the
      results.  Note that [f] must not return [void]. */
  extern void app_c(`c f(`a,`b),`a,list_t<`b> x);
  /** [app_c] is a version of [app] where the function argument
      requires a closure as its first argument. */

  extern void app2(`c f(`a,`b),list_t<`a> x,list_t<`b> y);
  /** If [x] has elements [x1] through [xn], and [y] has elements [y1]
      through [yn], then [app2(f,x,y)] performs [f(x1,y1)] through
      [f(xn,yn)] and discards the results.  If [x] and [y] don't have
      the same number of elements, [List_mismatch] is thrown. */
  extern void app2_c(`d f(`a,`b,`c),`a env, list_t<`b> x,list_t<`c> y);
  /** [app2_c] is a version of [app2] where the function argument
      requires a closure as its first argument. */

  extern void iter(void f(`a),list_t<`a> x);
  /** [iter(f,x)] is like [app(f,x)], except that [f] returns [void]. */
  extern void iter_c(void f(`b,`a),`b env,list_t<`a> x);
  /** [iter_c] is a version of [iter] where the function argument
      requires a closure as its first argument. */

  extern void iter2(void f(`a,`b),list_t<`a> x,list_t<`b> y);
  /** [iter2] is a version of [app2] where the function returns [void]. */
  extern void iter2_c(void f(`a,`b,`c),`a env,list_t<`b> x,list_t<`c> y);
  /** [iter2_c] is a version of [iter2] where the function argument
      requires a closure as its first argument. */

  extern `a fold_left(`a f(`a,`b),`a accum,list_t<`b> x);
  /** If [x] has elements [x1] through [xn], then
      [fold_left(f,accum,x)] returns
      [f(f(...(f(x2,f(x1,accum))),xn-1),xn)]. */
  extern `a fold_left_c(`a f(`c,`a,`b),`c,`a accum,list_t<`b> x);
  /** [fold_left_c] is a version of [fold_left] where the function
      argument requires a closure as its first argument. */

  extern `b fold_right(`b f(`a,`b),list_t<`a> x,`b accum);
  /** If [x] has elements [x1] through [xn], then
      [fold_left(f,accum,x)] returns
      [f(x1,f(x2,...,f(xn-1,f(xn,a))...))]. */
  extern `b fold_right_c(`b f(`c,`a,`b),`c,list_t<`a> x,`b accum);
  /** [fold_right_c] is a version of [fold_right] where the function
      argument requires a closure as its first argument. */

  extern list_t<`a> revappend(list_t<`a,`r> x,list_t<`a,`H> y);
  /** If [x] has elements [x1] through [xn], [revappend(x,y)] returns
      a list that starts with elements [xn] through [x1], then
      continues with [y].  Cons cells for the first [n] elements are
      newly-allocated on the heap, and [y] must be allocated on the
      heap.  */
  extern list_t<`a,`r> rrevappend(region_t<`r>,list_t<`a> x,list_t<`a,`r> y);
  /** [rrevappend(r,x,y)] is like [revappend(x,y)], except that [y]
      must be allocated in the region with handle [r], and the result
      is allocated in the same region.  */

  extern list_t<`a> rev(list_t<`a> x);
  /** [rev(x)] returns a new heap-allocated list whose elements are
      the elements of [x] in reverse.  */
  extern list_t<`a,`r> rrev(region_t<`r>,list_t<`a> x);
  /** [rrev(r,x)] is like [rev(x)], except that the result is
      allocated in the region with handle [r].  */
  extern list_t<`a,`r> imp_rev(list_t<`a::TB,`r> x);
  /** [imp_rev(x)] imperatively reverses list [x] (the list is
      side-effected).  Note that [imp_rev] returns a list.  This is
      because the first cons cell of the result is the last cons cell
      of the input; a typical use is therefore [x = imp_rev(x)].  */

  extern list_t<`a> append(list_t<`a> x,list_t<`a,`H> y);
  /** If [x] has elements [x1] through [xn], [append(x,y)] returns
      a list that starts with elements [x1] through [xn], then
      continues with [y].  Cons cells for the first [n] elements are
      newly-allocated on the heap, and [y] must be allocated on the
      heap.  */
  extern list_t<`a,`r> rappend(region_t<`r>,list_t<`a> x,list_t<`a,`r> y);
  /** [rappend(r,x,y)] is like [append(x,y)], except that [y] must be
      allocated in the region with handle [r], and the result is
      allocated in the same region.  */

  extern list_t<`a,`r> imp_append(list_t<`a::TB,`r> x,list_t<`a,`r> y);
  /** [imp_append(x,y)] modifies [x] to append [y] to it,
      destructively.  Note that [imp_append] returns a list.  This is
      because [x] might be NULL, in which case, [imp_append(x,y)]
      returns [y]; so a typical use would be [x = imp_append(x,y)]. */

  extern list_t<`a> flatten(list_t<list_t<`a,`H>> x);
  /** In [flatten(x)], [x] is a list of lists, and the result is a new
      heap-allocated list with elements from each list in [x], in
      sequence.  Note that [x] must be allocated on the heap. */
  extern list_t<`a,`r> rflatten(region_t<`r>, list_t<list_t<`a,`r>> x);
  /** [rflatten(r,x)] is like [flatten(x)], except that the result is
      allocated in the region with handle [r], and each element of [x]
      must be allocated in [r].  */

  extern list_t<`a> merge_sort(int cmp(`a,`a), list_t<`a> x);
  /** [merge_sort(cmp,x)] returns a new heap-allocated list whose
      elements are the elements of [x] in ascending order (according to
      the comparison function [cmp]), by the MergeSort algorithm. */
  extern list_t<`a,`r> rmerge_sort(region_t<`r>, int cmp(`a,`a),
                                   list_t<`a> x);
  /** [rmerge_sort(r,x)] is like [merge_sort(x)], except that the result is
      allocated in the region with handle [r].  */


  extern list_t<`a,`r> rimp_merge_sort(int cmp(`a,`a), list_t<`a,`r> x);
  /** [rimp_merge_sort] is an imperative version of [rmerge_sort]: the
      list is sorted in place.  [rimp_merge_sort] returns a list
      because the first cons cell of the sorted list might be
      different from the first cons cell of the input list; a typical
      use is [x = rimp_merge_sort(cmp,x)].  */

  extern list_t<`a> merge(int cmp(`a,`a),list_t<`a,`H> x,list_t<`a,`H> y);
  /** [merge(cmp,x,y)] returns the merge of two sorted lists,
      according to the [cmp] function.  */
  extern list_t<`a,`r3> rmerge(region_t<`r3>, int cmp(`a,`a),
                                list_t<`a> a,list_t<`a> b);
  /** [rmerge(r,cmp,x,y)] is like [merge(cmp,x,y)], except that [x],
      [y], and the result are allocated in the region with handle [r].  */
  extern list_t<`a,`r> imp_merge(int cmp(`a,`a),
                                 list_t<`a,`r> a,list_t<`a,`r> b);
  /** [imp_merge] is an imperative version of [merge]. */

  extern datatype exn {extern Nth};
  /** [Nth] is thrown when [nth] doesn't have enough elements in the
       list.  */

  extern `a nth(list_t<`a> x,int n);
  /** If [x] has elements [x0] through [xm], and [0<=n<=m], then
      [nth(x,n)] returns [xn].  If [n] is out of range, [Nth] is
      thrown.  Note that the indexing is 0-based. */
  extern list_t<`a,`r> nth_tail(list_t<`a,`r> x,int i);
  /** If [x] has elements [x0] through [xm], and [0<=n<=m], then
      [nth(x,n)] returns the list with elements [xn] through [xm].  If
      [n] is out of range, [Nth] is thrown.  */

  extern bool forall(bool pred(`a),list_t<`a> x);
  /** [forall(pred,x)] returns true if [pred] returns true when
      applied to every element of [x], and returns false otherwise. */
  extern bool forall_c(bool pred(`a,`b),`a env,list_t<`b> x);
  /** [forall_c] is a version of [forall] where the function
      argument requires a closure as its first argument. */

  extern bool exists(bool pred(`a),list_t<`a> x);
  /** [exists(pred,x)] returns true if [pred] returns true when
      applied to some element of [x], and returns false otherwise. */
  extern bool exists_c(bool pred(`a,`b),`a env,list_t<`b> x);
  /** [exists_c] is a version of [exists] where the function
      argument requires a closure as its first argument. */

  extern `c *`r find_c(`c::TA *`r pred(`a,`b),`a env,list_t<`b> x);
  /** [find_c] iterates over the given list and returns the first element
      for which [pred] does not return NULL.  Otherwise it returns
      NULL. */

  extern list_t<$(`a,`b)@`H,`H> zip(list_t<`a> x,list_t<`b> y);
  /** If [x] has elements [x1] through [xn], and [y] has elements [y1]
      through [yn], then [zip(x,y)] returns a new heap-allocated array
      with elements [\&\$(x1,y1)] through [\&\$(xn,yn)].  If [x] and
      [y] don't have the same number of elements, [List_mismatch] is
      thrown. */
  extern list_t<$(`a,`b)@`r2,`r1> rzip(region_t<`r1> r1, region_t<`r2> r2,
                                       list_t<`a> x, list_t<`b> y);
  /** [rzip(r1,r2,x,y)] is like [zip(x,y)], except that the list
      returned is allocated in the region with handle [r1], and the
      pairs of that list are allocated in the region with handle [r2].  */
  extern list_t<$(`a,`b,`c)@`H,`H> zip3(list_t<`a> x,list_t<`b> y, list_t<`c> z);
  /** If [x] has elements [x1] through [xn], and [y] has elements [y1]
      through [yn], and [z] has elements [z1]
      through [zn], then [zip3(x,yz)] returns a new heap-allocated array
      with elements [\&\$(x1,y1,z1)] through [\&\$(xn,yn,zn)].  If [x] and
      [y] don't have the same number of elements, [List_mismatch] is
      thrown. */
  extern list_t<$(`a,`b,`c)@`r2,`r1> rzip3(region_t<`r1> r1, region_t<`r2> r2,
                                       list_t<`a> x, list_t<`b> y, list_t<`c> z);
  /** [rzip3(r1,r2,x,y)] is like [zip3(x,y)], except that the list
      returned is allocated in the region with handle [r1], and the
      pairs of that list are allocated in the region with handle [r2].  */

  extern $(list_t<`a>,list_t<`b>) split(list_t<$(`a,`b)@> x);
  /** If [x] has elements [\&\$(a1,b1)] through [\&\$(an,bn)], then
      [split(x)] returns a pair of new heap-allocated arrays with
      elements [a1] through [an], and [b1] through [bn]. */
  extern $(list_t<`a>,list_t<`b>,list_t<`c>) split3(list_t<$(`a,`b,`c)@> x);
  /** If [x] has elements [\&\$(a1,b1,c1)] through [\&\$(an,bn,cn)],
      then [split(x)] returns a triple of new heap-allocated arrays
      with elements [a1] through [an], and [b1] through [bn], and [c1]
      through [cn]. */
  extern $(list_t<`a,`r1>,list_t<`b,`r2>)
    rsplit(region_t<`r1> r1, region_t<`r2> r2,
           list_t<$(`a,`b)@> x);
  /** [rsplit(r1,r2,x)] is like [split(x)], except that the first list
      returned is allocated in the region with handle [r1], and the
      second list returned is allocated in the region with handle
      [r2].  */
  extern $(list_t<`a,`r3>,list_t<`b,`r4>,list_t<`c,`r5>)
    rsplit3(region_t<`r3> r3, region_t<`r4> r4, region_t<`r5> r5,
            list_t<$(`a,`b,`c)@> x);
  /** [rsplit(r1,r2,r3,x)] is like [split3(x)], except that the first
      list returned is allocated in the region with handle [r1], the
      second list returned is allocated in the region with handle
      [r2], and the third list returned is allocated in the region
      with handle [r3]. */

  extern bool memq(list_t<`a> l,`a x);
  /** [memq(l,x)] returns true if [x] is [==] an element of list [l],
      and returns false otherwise. */
  extern bool mem(int compare(`a,`a), list_t<`a> l, `a x);
  /** [mem(cmp,l,x)] is like [memq(l,x)] except that the comparison
      function [cmp] is used to determine if [x] is an element of [l].
      [cmp(a,b)] should return 0 if [a] is equal to [b], and return a
      non-zero number otherwise. */

  extern `b assoc(list_t<$(`a,`b)@> l,`a k);
  /** An association list is a list of pairs where the first element
      of each pair is a key and the second element is a value; the
      association list is said to map keys to values.  [assoc(l,k)]
      returns the first value paired with key [k] in association list
      [l], or throws [Core::Not_found] if [k] is not paired with any
      value in [l].  [assoc] uses [==] to decide if [k] is a key in
      [l]. */
  extern `b assoc_cmp(int cmp(`a,`c),list_t<$(`a,`b)@> l,`c x);
  /** [assoc_cmp(cmp,l,k)] is like [assoc(l,k)] except that the
      comparison function [cmp] is used to decide if [k] is a key in
      [l].  [cmp] should return 0 if two keys are equal, and non-zero
      otherwise.  */

  extern bool mem_assoc(list_t<$(`a,`b)@> l,`a x);
  /** [mem_assoc(l,k)] returns true if [k] is a key in association
      list [l] (according to [==]).  */
  extern bool mem_assoc_cmp(int cmp(`a,`c),list_t<$(`a,`b)@> l,`c x);
  /** Same as [mem_assoc], but uses comparison function [cmp] rather
      than pointer equality ==. */

  extern list_t<`a,`r::R> delete(list_t<`a,`r::R> l, `a x);
  /** [delete(l,k)] returns the list with the first occurence of [x]
      removed from it, if [x] was in the list; otherwise raises
      [Core::Not_found].  Side-effects original list [l]. */

  extern list_t<`a,`r::R> delete_cmp(int cmp(`a,`a), list_t<`a,`r::R> l, `a x);
  /** [delete(l,k)] returns the list with the first [e] in the list
      such that [cmp]([x],[e]) == 0.  If no such [e] exists, raises
      [Core::Not_found].  Side-effects original list [l]. */

  extern Core::opt_t<`c> check_unique(int cmp(`c,`c),list_t<`c> x);
  /** [check_unique(cmp,x)] checks whether the sorted list [x] has
      duplicate elements, according to [cmp].  If there are any
      duplicates, one will be returned; otherwise, NULL is returned.  */

  extern `a ?`H to_array(list_t<`a> x);
  /** [to_array(x)] returns a new heap-allocated array with the same
      elements as list [x].  */
  extern `a ?`r rto_array(region_t<`r> r, list_t<`a> x);
  /** [rto_array(r,x)] is like [to_array(x)], except that the
      resulting array is allocated in the region with handle [r]. */

  extern list_t<`a> from_array(`a ? arr);
  /** [from_array(x)] returns a new heap-allocated list with the same
      elements as array [x].  */
  extern list_t<`a,`r2> rfrom_array(region_t<`r2> r2, `a ? arr);
  /** [rfrom_array(r,x)] is like [from_array(x)], except that the
      resulting list is allocated in the region with handle [r]. */

  extern list_t<`a> tabulate(int n, `a f(int));
  extern list_t<`a> tabulate_c(int n, `a f(`b,int), `b env);
  extern list_t<`a,`r> rtabulate(region_t<`r> r, int n, `a f(int));
  extern list_t<`a,`r> rtabulate_c(region_t<`r> r,int n,`a f(`b,int),`b env);

  extern int list_cmp(int cmp(`a,`b), list_t<`a> l1, list_t<`b> l2);
  /** [list_cmp(cmp,l1,l2)] is a comparison function on lists,
      parameterized by a comparison function [cmp] on list elements.  */

  extern bool list_prefix(int cmp(`a,`b), list_t<`a> l1, list_t<`b> l2);
  /** [list_prefix(cmp,l1,l2)] returns true if [l1] is a prefix of
      [l2], using [cmp] to compare the elements of [l1] and [l2].  */

  extern list_t<`a> filter(bool f(`a), list_t<`a> x);
  /** [filter(f,x)] returns a new heap-allocated list whose elements
      are the elements of [x] on which [f] returns true, in order. */
  extern list_t<`a> filter_c(bool f(`b,`a), `b env, list_t<`a> x);
  /** [filter_c] is a version of [filter] where the function
      argument requires a closure as its first argument. */
  extern list_t<`a,`r> rfilter(region_t<`r> r,bool f(`a),list_t<`a> x);
  /** [rfilter_c(r,f,x)] is like [filter_c(f,x)], except that the
      resulting list is allocated in the region with handle [r]. */
  extern list_t<`a,`r> rfilter_c(region_t<`r> r, bool f(`b,`a),
                                 `b env, list_t<`a> x);
  /** [rfilter_c] is a version of [rfilter] where the function
      argument requires a closure as its first argument. */
}
#endif
