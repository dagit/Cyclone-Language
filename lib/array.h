#ifndef _ARRAY_H_
#define _ARRAY_H_

#include "list.h"

namespace Array {


// Sorting functions (quicksort and mergesort)
// Input is a comparison function, the array to be sorted (in place),
// starting offset, and number of elements from that point to sort.
// The advantage of quicksort is speed (presumed -- feel free to 
// benchmark), however it is not a stable sort.  Mergesort, as 
// implemented here, is stable.

extern void qsort<`a>(int less_eq(`a,`a), `a ?x, int ofs, int len);

extern void msort<`a>(int less_eq(`a,`a), `a ?x, int ofs, int len);


//////// Below are bunches of stuff mimicking the List library

// make array from list
extern `a? from_list<`a>(List::list<`a> x);

// convert array to list
extern List::list<`a> to_list<`a>(`a ?x);

// return a fresh copy of the array (same as map of the identity) 
extern `a ?copy<`a>(`a ?x);

// Apply a function to each element in an array, returning a new array. 
extern `b ?map<`a,`b>(`b f(`a),`a ?x);

// Apply a function to each element in an array, returning a new array. 
// This uses an additional argument to simulate function closures.
extern `b ?map_c<`a,`b,`c>(`b f(`c,`a),`c env,`a ?x);
//
// Apply a function to each element in an array, writing the results
// back into the array.
extern void imp_map<`a>(`a f(`a),`a ?x);
extern void imp_map_c<`a,`b>(`a f(`b,`a),`b env,`a ?x);

// thrown when two arrays don't have the same size 
extern xenum exn {Array_mismatch};

// Given two arrays of the same length and a function, apply the function
// to each pair of elements (in order) and collect the results in a new array.
// Throws Array_mismatch if the sizes of the arrays aren't the same.
extern `c ?map2<`a,`b,`c>(`c f(`a,`b),`a ?x,`b ?y);

// Apply some function to each element of the array, but don't bother to
// save the result.  Similar to Ocaml's List.iter but doesn't require
// void type for the result of the function.
extern void app<`a,`b>(`b f(`a),`a ?x);

extern void app_c<`a,`b,`c>(`c f(`a,`b),`a env,`b ?x);

// same as app but with void functions -- unlike Popcorn, the iter
// functions are no longer needed because we treat "void" as a boxed
// type.  
extern void iter<`a>(void f(`a),`a ?x);

// same as app_c but with void functions
extern void iter_c<`a,`b>(void f(`b,`a),`b env,`a ?x);

// Same as app, but generalized to a pair of arrays.  Throws Array_mismatch
// if the lengths of the arrays aren't the same.  
extern void app2<`a,`b,`c>(`c f(`a,`b),`a ?x,`b ?y);

extern void app2_c<`a,`b,`c,`d>(`d f(`a,`b,`c),`a env,`b ?x,`c ?y);

// Same as iter, but generalized to a pair of arrays.  Throws Array_mismatch
// if the lengths of the arrays aren't the same.  
extern void iter2<`a,`b>(void f(`a,`b),`a ?x,`b ?y);

extern void iter2_c<`a,`b,`c>(void f(`a,`b,`c),`a env,`b ?x,`c ?y);



// Given an array [|x1,x2,...,xn-1,xn|], a function f, and an accumulator
// a, return f(f(...(f(x2,f(x1,a))),xn-1),xn).  Notice that the function
// is first applied to the left-most element of the array.
extern `a fold_left<`a,`b>(`a f(`a,`b),`a accum,`b ?x);
extern `a fold_left_c<`a,`b,`c>(`a f(`c,`a,`b),`c env,`a accum,`b ?x);

// Given an array [|x1,x2,....,xn-1,xn|], a function f, and an accumulator
// a, return f(x1,f(x2,...,f(xn-1,f(xn,a))...)).  Notice that the function
// is first applied to the right-most element of the list. 
extern `b fold_right<`a,`b>(`b f(`a,`b),`a ?x,`b accum);
extern `b fold_right_c<`a,`b,`c>(`b f(`c,`a,`b),`c env,`a ?x,`b accum);

// Return a copy of an array in reverse.
extern `a ?rev_copy<`a>(`a ?x);

// Imperatively reverse an array.
extern void imp_rev<`a>(`a ?x);

// Given a predicate on 'a values, determine whether every element in an
// array satisfies the predicate.
extern bool forall<`a>(bool pred(`a),`a ?x);
extern bool forall_c<`a,`b>(bool pred(`a,`b),`a env,`b ?x);

// Given a predicate on 'a values, determine whether there exists an element
// in the array that satisfies the predicate.
extern bool exists<`a>(bool pred(`a),`a ?x);
extern bool exists_c<`a,`b>(bool pred(`a,`b),`a env,`b ?x);

// Given [|x1,...,xn|] and [|y1,...,yn|], return [|(x1,y1),...,(xn,yn)|].  
// Throws Array_mismatch if the lengths are not the same.
extern $(`a,`b) ?zip<`a,`b>(`a ?x,`b ?y);

// Given [|(x1,y1),...,(xn,yn)|], return ([|x1,...,xn|],[|y1,...,yn|]) 
extern $(`a?,`b?) split<`a,`b>($(`a,`b) ?x);


// Given an array [|x1,...,xn|] and x, determine if x is in the array.
// Uses physical equality for comparison.
extern bool memq<`a>(`a ?l, `a x);

extern bool mem<`a>(int compare(`a,`a), `a ?l, `a x);


// Creates a new (smaller) array containing the specified slice of the
// target array.  The last parameter is an int option of the number of 
// elements to take, or null for "take until the end of the array".
extern `a ?extract<`a>(`a ?x,int start,Core::Opt_t<int> n_opt);



}

#endif
