#ifndef LIST_H
#define LIST_H
////////////////////////////////////////////////////////////////////////////
// Cyclone library, file list.h                                           //
// Copyright Greg Morrisett, Trevor Jim                                   //
// September 2000, all rights reserved                                    //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
#include "core.h"

namespace List {

// list.h:   defines generic lists and various operations over them,
//           following the conventions of the Ocaml list library as much
//           as possible.  Also, a reasonable attempt to make things as
//           space efficient as possible, while remaining "functional".

struct cons<`a>{`a hd; struct cons<`a> *tl;};
typedef struct cons<`a> *list<`a>,@List<`a>;

// Return the length of a list.
extern int length<`a>(list<`a> x);

// raised when some function expects a non-empty list
extern xenum exn {List_empty};

// return the first element in a list, if any, raise List_empty otherwise. 
extern `a hd<`a>(list<`a> x);

// return the tail of a list, if any, raise List_empty otherwise. 
extern list<`a> tl<`a>(list<`a> x);

// return a fresh copy of the list (same as map of the identity)
extern list<`a> copy<`a>(list<`a> src);

// Apply a function to each element in a list, returning a new list. 
extern list<`b> map<`a,`b>(`b f(`a),list<`a> x);

// Apply a "closure" to each element in a list, returning a new list.
extern list<`b> map_c<`a,`b,`c>(`b f(`c,`a),`c env,list<`a> x);

// raised when two lists don't have the same size 
extern xenum exn {List_mismatch};

// Given two lists of the same length and a function, apply the function
// to each pair of elements (in order) and collect the results in a new list.
// Raises List_mismatch if the sizes of the lists aren't the same.
extern list<`c> map2<`a,`b,`c>(`c f(`a,`b),list<`a> x,list<`b> y);

// Apply some function to each element of the list, but don't bother to
// save the result.  Similar to Ocaml's List.iter but doesn't require
// void type for the result of the function.
extern void app<`a,`b>(`b f(`a),list<`a> x);
// Same but function gets an extra argument to simulate closures.
extern void app_c<`a,`b,`c>(`c f(`a,`b),`a,list<`b> x);

// Same as app, but generalized to `a pair of lists.  Raises List_mismatch
// if the lengths of the list aren't the same.  
extern void app2<`a,`b,`c>(`c f(`a,`b),list<`a> x,list<`b> y);
extern void app2_c<`a,`b,`c,`d>(`d f(`a,`b,`c),`a env,list<`b> x,list<`c> y);

// Similar to app but assumes function returns void type
extern void iter<`a>(void f(`a),list<`a> x);
extern void iter_c<`a,`b>(void f(`b,`a),`b env,list<`a> x);
extern void iter2<`a,`b>(void f(`a,`b),list<`a> x,list<`b> y);
extern void iter2_c<`a,`b,`c>(void f(`a,`b,`c),`a env,list<`b> x,list<`c> y);

// Given a list [x1,x2,...,xn-1,xn], a function f, and an accumulator a,
// return f(f(...(f(x2,f(x1,a))),xn-1),xn).  Notice that the function is
// first applied to the left-most element of the list.
extern `a fold_left<`a,`b>(`a f(`a,`b),`a accum,list<`b> x);
extern `a fold_left_c<`a,`b,`c>(`a f(`c,`a,`b),`c,`a accum,list<`b> x);

// Given a list [x1,x2,....,xn-1,xn], a function f, and an accumulator a,
// return f(x1,f(x2,...,f(xn-1,f(xn,a))...)).  Notice that the function is
// first applied to the right-most element of the list. 
extern `b fold_right<`a,`b>(`b f(`a,`b),list<`a> x,`b accum);
extern `b fold_right_c<`a,`b,`c>(`b f(`c,`a,`b),`c,list<`a> x,`b accum);

// Given [x1,...,xn] and [y1,...,ym], return [xn,...,x1,y1,...,ym].
// That is, the first list reversed and appended to the second list.
extern list<`a> revappend<`a>(list<`a> x,list<`a> y);

// Return the reverse of a list. 
extern list<`a> rev<`a>(list<`a> x);
// Imperatively reverse the list
extern list<`a> imp_rev<`a>(list<`a> x);

// Return a new list that has elements of x followed by elements of y
extern list<`a> append<`a>(list<`a> x,list<`a> y);
// Modify x so that y is appended to it destructively -- if x is empty return y
extern list<`a> imp_append<`a>(list<`a> x,list<`a> y);

// flatten a list of lists into a single list
extern list<`a> flatten<`a>(list<list<`a>> x);

// Given a partial order less_eq on `a elements and a list, return
// the list sorted by less_eq.  Uses a merge sort.  The less_eq
// function should return 0 if the elements are equal, i < 0 if
// the first is less than the second, and i > 0 otherwise.
extern list<`a> merge_sort<`a>(int less_eq(`a,`a), list<`a> x);

// Merge two (sorted) lists using the less_eq operation.
extern list<`a> merge<`a>(int less_eq(`a,`a),list<`a> a,list<`a> b);

// raised when list_nth doesn't have enough elements in the list. 
extern xenum exn {Nth};

// Given [x0,x1,...,xn], return the ith element of the list (0 <= i <= n).
// Raise Nth if the list doesn't have enough elements.  Notice that the
// indexing is zero-based.
extern `a nth<`a>(list<`a> x,int i);

// Given a predicate on `a values, determine whether every element in a list
// satisfies the predicate.
extern bool forall<`a>(bool pred(`a),list<`a> x);
extern bool forall_c<`a,`b>(bool pred(`a,`b),`a env,list<`b> x);

// Given a predicate on `a values, determine whether there exists an element
// in the list that satisfies the predicate.
extern bool exists<`a>(bool pred(`a),list<`a> x);

// Given [x1,...,xn] and [y1,...,yn], return [(x1,y1),...,(xn,yn)].  
// Raises List_mismatch if the lengths are not the same.
extern list<$(`a,`b)@> zip<`a,`b>(list<`a> x,list<`b> y);

// Given [(x1,y1),...,(xn,yn)], return ([x1,...,xn],[y1,...,yn])
extern $(list<`a>,list<`b>) split<`a,`b>(list<$(`a,`b)@> x);
extern $(list<`a>,list<`b>,list<`c>) split3<`a,`b,`c>(list<$(`a,`b,`c)@> x);
  
// Given a list [x1,...,xn] and x, determine if x is in the list.  Uses
// physical equality for comparison.
extern bool memq<`a>(list<`a> l,`a x);
extern bool mem<`a>(int compare(`a,`a), list<`a> l, `a x);

// Raised by list_assoc and list_mem_assoc (now defined in Core)
//extern exception Not_found;

// Given an association list [(k1,d1),...,(kn,dn)] and a key x,
// return the first di such that ki = x.  Uses physical equality.
// Raises Not_found if no such ki exists.
extern `b assoc<`a,`b>(list<$(`a,`b)@> l,`a x);
extern `b assoc_cmp<`a,`b>(int compare(`a,`a), list<$(`a,`b)@> l,`a x);

// Given an association list [(k1,d1),...,(kn,dn)] and a key x,
// returns true iff there exists a ki = x.
extern bool mem_assoc<`a,`b>(list<$(`a,`b)@> l,`a x);

// checks that a list of elements is unique -- assumes they're sorted
// returns an element that is duplicated if one exists, otherwise returns
// null.
extern Core::Opt_t<`c> check_unique<`c>(int compare(`c,`c),list<`c> x);

// makes a new array with index i being the ith element of the list
extern `a to_array<`a>(list<`a> x) [?];

// makes a new list with ith element arr[i]
extern list<`a> from_array<`a>(`a arr[?]);

extern list<`a> tabulate<`a>(int n, `a f(int));
extern list<`a> tabulate_c<`a,`b>(int n, `a f(`b,int), `b env);

}
#endif
