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

extern void qsort(int less_eq(`a,`a), `a ?x, int ofs, int len);

extern void msort(int less_eq(`a,`a), `a ?x, int ofs, int len);


//////// Below are bunches of stuff mimicking the List library

// make array from list
extern `a? from_list(List::glist_t<`a,`r> x);

// convert array to list
extern List::list_t<`a> to_list(`a ?`r x);

// return a fresh copy of the array (same as map of the identity) 
extern `a ?copy(`a::B ?`r x);

// Apply a function to each element in an array, returning a new array. 
extern `b ?map(`b f(`a),`a ?`r x);

// Apply a function to each element in an array, returning a new array. 
// This uses an additional argument to simulate function closures.
extern `b ?map_c(`b f(`c,`a),`c env,`a ?`r x);
//
// Apply a function to each element in an array, writing the results
// back into the array.
extern void imp_map(`a f(`a),`a ?`r x);
extern void imp_map_c(`a f(`b,`a),`b env,`a ?`r x);

// thrown when two arrays don't have the same size 
extern xtunion exn {extern Array_mismatch};

// Given two arrays of the same length and a function, apply the function
// to each pair of elements (in order) and collect the results in a new array.
// Throws Array_mismatch if the sizes of the arrays aren't the same.
extern `c ?map2(`c f(`a,`b),`a ?`r1 x,`b ?`r2 y);

// Apply some function to each element of the array, but don't bother to
// save the result.  Similar to Ocaml's List.iter but doesn't require
// void type for the result of the function.
extern void app(`b f(`a),`a ?`r x);

extern void app_c(`c f(`a,`b),`a env,`b ?`r x);

// same as app but with void functions -- unlike Popcorn, the iter
// functions are no longer needed because we treat "void" as a boxed
// type.  
extern void iter(void f(`a),`a ?`r x);

// same as app_c but with void functions
extern void iter_c(void f(`b,`a),`b env,`a ?`r x);

// Same as app, but generalized to a pair of arrays.  Throws Array_mismatch
// if the lengths of the arrays aren't the same.  
extern void app2(`c f(`a,`b),`a ?`r1 x,`b ?`r2 y);

extern void app2_c(`d f(`a,`b,`c),`a env,`b ?`r1 x,`c ?`r2 y);

// Same as iter, but generalized to a pair of arrays.  Throws Array_mismatch
// if the lengths of the arrays aren't the same.  
extern void iter2(void f(`a,`b),`a ?`r1 x,`b ?`r2 y);

extern void iter2_c(void f(`a,`b,`c),`a env,`b ?`r1 x,`c ?`r2 y);



// Given an array [|x1,x2,...,xn-1,xn|], a function f, and an accumulator
// a, return f(f(...(f(x2,f(x1,a))),xn-1),xn).  Notice that the function
// is first applied to the left-most element of the array.
extern `a fold_left(`a f(`a,`b),`a accum,`b ?`r x);
extern `a fold_left_c(`a f(`c,`a,`b),`c env,`a accum,`b ?`r x);

// Given an array [|x1,x2,....,xn-1,xn|], a function f, and an accumulator
// a, return f(x1,f(x2,...,f(xn-1,f(xn,a))...)).  Notice that the function
// is first applied to the right-most element of the list. 
extern `b fold_right(`b f(`a,`b),`a ?`r x,`b accum);
extern `b fold_right_c(`b f(`c,`a,`b),`c env,`a ?`r x,`b accum);

// Return a copy of an array in reverse.
extern `a ?rev_copy(`a::B ?`r x);

// Imperatively reverse an array.
extern void imp_rev(`a::B ?`r x);

// Given a predicate on 'a values, determine whether every element in an
// array satisfies the predicate.
extern bool forall(bool pred(`a),`a ?`r x);
extern bool forall_c(bool pred(`a,`b),`a env,`b ?`r x);

// Given a predicate on 'a values, determine whether there exists an element
// in the array that satisfies the predicate.
extern bool exists(bool pred(`a),`a ?`r x);
extern bool exists_c(bool pred(`a,`b),`a env,`b ?`r x);

// Given [|x1,...,xn|] and [|y1,...,yn|], return [|(x1,y1),...,(xn,yn)|].  
// Throws Array_mismatch if the lengths are not the same.
extern $(`a,`b) ?zip(`a ?`r1 x,`b ?`r2 y);

// Given [|(x1,y1),...,(xn,yn)|], return ([|x1,...,xn|],[|y1,...,yn|]) 
extern $(`a?,`b?) split($(`a,`b) ?`r x);


// Given an array [|x1,...,xn|] and x, determine if x is in the array.
// Uses physical equality for comparison.
extern bool memq(`a ?`r l, `a x);

extern bool mem(int compare(`a,`a), `a ?`r l, `a x);


// Creates a new (smaller) array containing the specified slice of the
// target array.  The last parameter is an int option of the number of 
// elements to take, or null for "take until the end of the array".
extern `a ?extract(`a::B ?`r1 x,int start,int *`r2 n_opt);



}

#endif
