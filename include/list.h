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

  // list.h:   defines generic lists and various operations over them,
  //           following the conventions of the Ocaml list library as much
  //           as possible.  Also, a reasonable attempt to make things as
  //           space efficient as possible, while remaining "functional".

  struct List<`a,`r::R>{`a hd; struct List<`a,`r> *`r tl;};
  typedef struct List<`a,`r> *`r list_t<`a,`r>;
  typedef struct List<`a,`r> @List_t<`a,`r>;

  // calling list(e1,e2,...,en) will generate the list [e1,e2,...,en]
  extern list_t<`a> list(... `a);
  extern list_t<`a,`r> rlist(region_t<`r>,... `a);

  // Return the length of a list.
  extern int length(list_t x);
  
  // raised when some function expects a non-empty list
  extern xtunion exn {extern List_empty};
  
  // return the first element in a list, if any, raise List_empty otherwise. 
  extern `a hd(list_t<`a> x);
  
  // return the tail of a list, if any, raise List_empty otherwise. 
  extern list_t<`a,`r> tl(list_t<`a,`r> x);
  
  // return a fresh copy of the list (same as map of the identity)
  extern list_t<`a> copy(list_t<`a> src);
  extern list_t<`a,`r> rcopy(region_t<`r>, list_t<`a> src);

  // Apply a function to each element in a list, returning a new list. 
  extern list_t<`b> map(`b f(`a),list_t<`a> x);
  extern list_t<`b,`r> rmap(region_t<`r>,`b f(`a),list_t<`a> x);
  
  // Apply a "closure" to each element in a list, returning a new list.
  extern list_t<`b> map_c(`b f(`c,`a),`c env,list_t<`a> x);
  extern list_t<`b,`r> rmap_c(region_t<`r>,`b f(`c,`a),`c env, list_t<`a> x);
  
  // raised when two lists don't have the same size 
  extern xtunion exn {extern List_mismatch};
  
  // Given two lists of the same length and a function, apply the function
  // to each pair of elements (in order) and collect the results in a new list.
  // Raises List_mismatch if the sizes of the lists aren't the same.
  extern list_t<`c> map2(`c f(`a,`b),list_t<`a> x,list_t<`b> y);
  extern list_t<`c,`r> rmap2(region_t<`r>,`c f(`a,`b),
                               list_t<`a> x,list_t<`b> y);
  
  // Apply some function to each element of the list, but don't bother to
  // save the result.  Similar to Ocaml's List.iter but doesn't require
  // void type for the result of the function.
  extern void app(`b f(`a),list_t<`a> x);
  // Same but function gets an extra argument to simulate closures.
  extern void app_c(`c f(`a,`b),`a,list_t<`b> x);
  
  // Same as app, but generalized to `a pair of lists.  Raises List_mismatch
  // if the lengths of the list aren't the same.  
  extern void app2(`c f(`a,`b),list_t<`a> x,list_t<`b> y);
  extern void app2_c(`d f(`a,`b,`c),`a env, list_t<`b> x,list_t<`c> y);
  
  // Similar to app but assumes function returns void type
  extern void iter(void f(`a),list_t<`a> x);
  extern void iter_c(void f(`b,`a),`b env,list_t<`a> x);
  extern void iter2(void f(`a,`b),list_t<`a> x,list_t<`b> y);
  extern void iter2_c(void f(`a,`b,`c),`a env,list_t<`b> x,list_t<`c> y);
  
  // Given a list [x1,x2,...,xn-1,xn], a function f, and an accumulator a,
  // return f(f(...(f(x2,f(x1,a))),xn-1),xn).  Notice that the function is
  // first applied to the left-most element of the list.
  extern `a fold_left(`a f(`a,`b),`a accum,list_t<`b> x);
  extern `a fold_left_c(`a f(`c,`a,`b),`c,`a accum,list_t<`b> x);
  
  // Given a list [x1,x2,....,xn-1,xn], a function f, and an accumulator a,
  // return f(x1,f(x2,...,f(xn-1,f(xn,a))...)).  Notice that the function is
  // first applied to the right-most element of the list. 
  extern `b fold_right(`b f(`a,`b),list_t<`a> x,`b accum);
  extern `b fold_right_c(`b f(`c,`a,`b),`c,list_t<`a> x,`b accum);
  
  // Given [x1,...,xn] and [y1,...,ym], return [xn,...,x1,y1,...,ym].
  // That is, the first list reversed and appended to the second list.
  extern list_t<`a> revappend(list_t<`a,`r> x,list_t<`a,`H> y);
  extern list_t<`a,`r> rrevappend(region_t<`r>,list_t<`a> x,list_t<`a,`r> y);
  
  // Return the reverse of a list. 
  extern list_t<`a> rev(list_t<`a> x);
  extern list_t<`a,`r> rrev(region_t<`r>,list_t<`a> x);
  // Imperatively reverse the list
  extern list_t<`a,`r> imp_rev(list_t<`a,`r> x);
  
  // Return a new list that has elements of x followed by elements of y
  extern list_t<`a> append(list_t<`a> x,list_t<`a,`H> y);
  extern list_t<`a,`r> rappend(region_t<`r>,list_t<`a> x,list_t<`a,`r> y);
  // Modify x so that y is appended to it destructively--if x is empty return y
  extern list_t<`a,`r> imp_append(list_t<`a,`r> x,list_t<`a,`r> y);
  
  // flatten a list of lists into a single list
  extern list_t<`a> flatten(list_t<list_t<`a,`H>> x);
  extern list_t<`a,`r3> rflatten(region_t<`r3>, list_t<list_t<`a,`r3>> x);
  
  // Given a partial order less_eq on `a elements and a list, return
  // the list sorted by less_eq.  Uses a merge sort.  The less_eq
  // function should return 0 if the elements are equal, i < 0 if
  // the first is less than the second, and i > 0 otherwise.
  extern list_t<`a> merge_sort(int less_eq(`a,`a), list_t<`a> x);
  extern list_t<`a,`r> rmerge_sort(region_t<`r>, int less_eq(`a,`a), 
                                   list_t<`a> x);
                                     
  // imperative version of merge_sort
  extern list_t<`a,`r> rimp_merge_sort(int less_eq(`a,`a), list_t<`a,`r> x);
  
  // Merge two (sorted) lists using the less_eq operation.
  extern list_t<`a> merge(int less_eq(`a,`a),list_t<`a,`H> a,list_t<`a,`H> b);
  extern list_t<`a,`r3> rmerge(region_t<`r3>, int less_eq(`a,`a),
                                list_t<`a> a,list_t<`a> b);
  // Imperative version of merge.
  extern list_t<`a,`r> imp_merge(int less_eq(`a,`a),
                                  list_t<`a,`r> a,list_t<`a,`r> b);
  
  // raised when list_nth doesn't have enough elements in the list. 
  extern xtunion exn {extern Nth};

  // Given [x0,x1,...,xn], return the ith element of the list (0 <= i <= n).
  // Raise Nth if the list doesn't have enough elements.  Notice that the
  // indexing is zero-based.
  extern `a nth(list_t<`a> x,int i);
  // Given [x0,x1,...,xi-1,xi,xi+1,...,xn], return [xi,xi+1,...,xn].  Raise
  // Nth if the list doesn't have enough elements.  
  extern list_t<`a,`r> nth_tail(list_t<`a,`r> x,int i);

  // Given a predicate on `a values, determine whether every element in a list
  // satisfies the predicate.
  extern bool forall(bool pred(`a),list_t<`a> x);
  extern bool forall_c(bool pred(`a,`b),`a env,list_t<`b> x);
  
  // Given a predicate on `a values, determine whether there exists an element
  // in the list that satisfies the predicate.
  extern bool exists(bool pred(`a),list_t<`a> x);
  extern bool exists_c(bool pred(`a,`b),`a env,list_t<`b> x);
  
  // Given [x1,...,xn] and [y1,...,yn], return [(x1,y1),...,(xn,yn)].  
  // Raises List_mismatch if the lengths are not the same.
  extern list_t<$(`a,`b)@`H,`H> zip(list_t<`a> x,list_t<`b> y);
  extern list_t<$(`a,`b)@`r4,`r3> rzip(region_t<`r3> r3, region_t<`r4> r4,
                                        list_t<`a> x, list_t<`b> y);

  // Given [(x1,y1),...,(xn,yn)], return ([x1,...,xn],[y1,...,yn])
  extern $(list_t<`a>,list_t<`b>) split(list_t<$(`a,`b)@> x);
  extern $(list_t<`a>,list_t<`b>,list_t<`c>) split3(list_t<$(`a,`b,`c)@> x);
  extern $(list_t<`a,`r3>,list_t<`b,`r4>) 
    rsplit(region_t<`r3> r3, region_t<`r4> r4,
           list_t<$(`a,`b)@> x);
  extern $(list_t<`a,`r3>,list_t<`b,`r4>,list_t<`c,`r5>) 
    rsplit3(region_t<`r3> r3, region_t<`r4> r4, region_t<`r5> r5,
            list_t<$(`a,`b,`c)@> x);


  // Given a list [x1,...,xn] and x, determine if x is in the list.  Uses
  // physical equality for comparison.
  extern bool memq(list_t<`a> l,`a x);
  extern bool mem(int compare(`a,`a), list_t<`a> l, `a x);
  
  // Raised by list_assoc and list_mem_assoc (now defined in Core)
  //extern exception Not_found;
  
  // Given an association list [(k1,d1),...,(kn,dn)] and a key x,
  // return the first di such that ki = x.  Uses physical equality.
  // Raises Not_found if no such ki exists.
  extern `b assoc(list_t<$(`a,`b)@> l,`a x);
  extern `b assoc_cmp(int compare(`a,`a), list_t<$(`a,`b)@> l,`a x);
  
  // Given an association list [(k1,d1),...,(kn,dn)] and a key x,
  // returns true iff there exists a ki = x.
  extern bool mem_assoc(list_t<$(`a,`b)@> l,`a x);
  
  // checks that a list of elements is unique -- assumes they're sorted
  // returns an element that is duplicated if one exists, otherwise returns
  // NULL.
  extern Core::opt_t<`c> check_unique(int compare(`c,`c),list_t<`c> x);
  
  // makes a new array with index i being the ith element of the list
  extern `a ?`H to_array(list_t<`a> x);
  extern `a ?`r2 rto_array(region_t<`r2> r2, list_t<`a> x);

  
  // makes a new list with ith element arr[i]
  extern list_t<`a> from_array(`a ? arr);
  extern list_t<`a,`r2> rfrom_array(region_t<`r2> r2, `a ? arr);
  
  extern list_t<`a> tabulate(int n, `a f(int));
  extern list_t<`a> tabulate_c(int n, `a f(`b,int), `b env);
  extern list_t<`a,`r> rtabulate(region_t<`r> r, int n, `a f(int));
  extern list_t<`a,`r> rtabulate_c(region_t<`r> r,int n,`a f(`b,int),`b env);
  
  // See .cyc for why the two lists are in the same region.
  extern int list_cmp(int cmp(`a,`a), list_t<`a,`r1> l1, list_t<`a,`r1> l2);
  // true iff l1 is prefix to l2 (according to cmp)
  extern bool list_prefix(int cmp(`a,`a), list_t<`a,`r1> l1, list_t<`a,`r1> l2);

  // Warning: Fairly inefficient implementation.
  extern list_t<`a> filter_c(bool f(`b,`a), `b env, list_t<`a> l);
  extern list_t<`a,`r> rfilter_c(region_t<`r> r2, bool f(`b,`a), 
                                 `b env, list_t<`a> l);
  
  extern list_t<`a> filter(bool f(`a), list_t<`a> l);
  extern list_t<`a,`r> rfilter(region_t<`r> r2,bool f(`a),list_t<`a> l);
}
#endif
