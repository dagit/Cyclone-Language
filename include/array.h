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

#ifndef _ARRAY_H_
#define _ARRAY_H_

#include <list.h>

namespace Array {

  /*** \subsection{\texttt{<array.h>}} */
  /*** Defines namespace Array, implementing utility functions on
       arrays. */

typedef int (@cmpfn_t<`a,`r1::R,`r2::R>)(`a @`r1, `a @`r2);

extern void qsort<`a::B,`r::R>(cmpfn_t<`a,`r,`r>, `a ?`r x, int len);
  /** [qsort(cmp,x,len)] sorts the first [len] elements of array [x]
      into ascending order (according to the comparison function
      [cmp]) by the QuickSort algorithm.  [cmp(a,b)] should return a
      number less than, equal to, or greater than 0 according to
      whether [a] is less than, equal to, or greater than [b].
      [qsort] throws [Core::InvalidArg("Array::qsort")] if [len] is
      negative or specifies a segment outside the bounds of [x].

      [qsort] is not a stable sort. */
extern void msort(cmpfn_t<`a::B,`H,`H>, `a ?`H x, int len);
  /** [msort(cmp,x,len)] sorts the first [len] elements of array [x]
      into ascending order (according to the comparison function
      [cmp]), by the MergeSort algorithm.  [msort] throws
      [Core::InvalidArg("Array::msort")] if [len] is negative or
      specifies a segment outside the bounds of [x].

      [msort] is a stable sort. */

extern `a? from_list(List::list_t<`a> l);
  /** [from_list(l)] returns a heap-allocated array with the same
      elements as the list [l]. */

extern List::list_t<`a> to_list(`a ? x);
  /** [to_list(x)] returns a new heap-allocated list with the same
      elements as the array [x]. */

extern `a ?copy(`a::B ?x);
  /** [copy(x)] returns a fresh copy of array [x], allocated on the
      heap. */

extern `b ?map(`b f(`a),`a ? x);
  /** [map(f,x)] applies [f] to each element of [x], returning the
      results in a new heap-allocated array. */

extern `b ?map_c(`b f(`c,`a),`c env,`a ? x);
  /** [map_c(f,env,x)] is like [map(f,x)] except that [f] requires a
      closure [env] as its first argument. */

extern void imp_map(`a f(`a),`a ? x);
  /** [imp_map(f,x)] replaces each element [xi] of [x] with [f(xi)].  */
extern void imp_map_c(`a f(`b,`a),`b env,`a ? x);
  /** [imp_map_c] is a version of [imp_map] where the function
      argument requires a closure as its first argument. */

extern datatype exn {extern Array_mismatch};
  /** [Array_mismatch] is thrown when two arrays don't have the same
      length. */

// Given two arrays of the same length and a function, apply the function
// to each pair of elements (in order) and collect the results in a new array.
// Throws Array_mismatch if the sizes of the arrays aren't the same.
extern `c ?map2(`c f(`a,`b),`a ? x,`b ? y);
  /** If [x] has elements [x1] through [xn], and [y] has elements [y1]
      through [yn], then [map2(f,x,y)] returns a new heap-allocated
      array with elements [f(x1,y1)] through [f(xn,yn)].  If [x] and
      [y] don't have the same number of elements, [Array_mismatch] is
      thrown. */
extern void app(`b f(`a),`a ?`r x);
  /** [app(f,x)] applies [f] to each element of [x], discarding the
      results.  Note that [f] must not return [void]. */
extern void app_c(`c f(`a,`b),`a env,`b ? x);
  /** [app_c(f,env,x)] is like [app(f,x)], except that [f] requires a
      closure [env] as its first argument. */
extern void iter(void f(`a),`a ? x);
  /** [iter(f,x)] is like [app(f,x)], except that [f] returns [void]. */
extern void iter_c(void f(`b,`a),`b env,`a ? x);
  /** [iter_c(f,env,x)] is like [app_c(f,env,x)] except that [f]
      returns [void]. */

extern void app2(`c f(`a,`b),`a ? x,`b ? y);
  /** If [x] has elements [x1] through [xn], and [y] has elements [y1]
      through [yn], then [app2(f,x,y)] performs [f(x1,y1)] through
      [f(xn,yn)] and discards the results.  If [x] and [y] don't have
      the same number of elements, [Array_mismatch] is thrown. */
extern void app2_c(`d f(`a,`b,`c),`a env,`b ? x,`c ? y);
  /** [app2_c] is a version of [app] where the function argument
      requires a closure as its first argument. */
extern void iter2(void f(`a,`b),`a ? x,`b ? y);
  /** [iter2] is a version of [app2] where the function returns [void]. */
extern void iter2_c(void f(`a,`b,`c),`a env,`b ? x,`c ? y);
  /** [iter2_c] is a version of [app2_c] where the function returns
      [void]. */

extern `a fold_left(`a f(`a,`b),`a accum,`b ? x);
  /** If [x] has elements [x1] through [xn], then
      [fold_left(f,accum,x)] returns
      [f(f(...(f(x2,f(x1,accum))),xn-1),xn)]. */
extern `a fold_left_c(`a f(`c,`a,`b),`c env,`a accum,`b ? x);
  /** [fold_left_c] is a version of [fold_left] where the function
      argument requires a closure as its first argument. */
extern `b fold_right(`b f(`a,`b),`a ? x,`b accum);
  /** If [x] has elements [x1] through [xn], then
      [fold_right(f,accum,x)] returns
      [f(x1,f(x2,...,f(xn-1,f(xn,a))...))]. */
extern `b fold_right_c(`b f(`c,`a,`b),`c env,`a ? x,`b accum);
  /** [fold_right_c] is a version of [fold_right] where the function
      argument requires a closure as its first argument. */

extern `a ?rev_copy(`a::B ? x);
  /** [rev_copy(x)] returns a new heap-allocated array whose elements
      are the elements of [x] in reverse order. */
extern void imp_rev(`a::B ? x);
  /** [imp_rev(x)] reverses the elements of array [x]. */

extern bool forall(bool pred(`a),`a ? x);
  /** [forall(pred,x)] returns true if [pred] returns true when
      applied to every element of [x], and returns false otherwise. */
extern bool forall_c(bool pred(`a,`b),`a env,`b ? x);
  /** [forall_c] is a version of [forall] where the predicate argument
      requires a closure as its first argument. */

extern bool exists(bool pred(`a),`a ? x);
  /** [exists(pred,x)] returns true if [pred] returns true when
      applied to some element of [x], and returns false otherwise. */
extern bool exists_c(bool pred(`a,`b),`a env,`b ? x);
  /** [exists_c] is a version of [exists] where the predicate argument
      requires a closure as its first argument. */

extern $(`a,`b) ?zip(`a ?`r1 x,`b ? y);
  /** If [x] has elements [x1] through [xn], and [y] has elements [y1]
      through [yn], then [zip(x,y)] returns a new heap-allocated array
      with elements [\$(x1,y1)] through [\$(xn,yn)].  If [x] and [y]
      don't have the same number of elements, [Array_mismatch] is
      thrown. */
  
extern $(`a?,`b?) split($(`a,`b) ? x);
  /** If [x] has elements [\$(a1,b1)] through [\$(an,bn)], then
      [split(x)] returns a pair of new heap-allocated arrays with
      elements [a1] through [an], and [b1] through [bn]. */

extern bool memq(`a ?l, `a x);
  /** [memq(l,x)] returns true if [x] is [==] an element of array [l],
      and returns false otherwise. */

extern bool mem(int cmp(`a,`a), `a ?l, `a x);
  /** [mem(cmp,l,x)] is like [memq(l,x)] except that the comparison
      function [cmp] is used to determine if [x] is an element of [l].
      [cmp(a,b)] should return 0 if [a] is equal to [b], and return a
      non-zero number otherwise. */

extern `a ?extract(`a::B ? x,int start,int *len_opt);
  /** [extract(x,start,len_opt)] returns a new array whose elements
      are the elements of [x] beginning at index [start], and
      continuing to the end of [x] if [len_opt] is NULL; if [len_opt]
      points to an integer [n], then [n] elements are extracted.  If
      [n<0] or there are less than [n] elements in [x] starting at
      [start], then [Core::InvalidArg("Array::extract")] is thrown. */

}

#endif
