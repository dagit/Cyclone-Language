#ifndef SET_H
#define SET_H
////////////////////////////////////////////////////////////////////////////
// Popcorn library, file set.h                                            //
// Copyright Greg Morrisett, Dan Grossman                                 //
// January 1999, all rights reserved                                      //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

#include "list.h"

namespace Set {
using List;

// set.h:    defines polymorphic, functional, finite sets over elements
//           with a total order, following the conventions of the Ocaml set
//           library as much as possible.  

// sets are polymorphic and abstract
extern struct set<`a>;
typedef struct set<`a> @Set<`a>;

// The set creation functions take a functional argument that takes
// two elements and returns 0 when equal, <0 when the first is less
// than the second, and >0 when the second is less than the first.

// create an empty set given a comparison function
extern Set<`a> empty<`a>(int comp(`a,`a));

// create a singleton set given a comparison function and element
extern Set<`a> singleton<`a>(int comp(`a,`a),`a x);

// create a set out of a comparison function and a list of elements
extern Set<`a> from_list<`a>(int comp(`a,`a),list<`a> x);

// insert an element into a set
extern Set<`a> insert<`a>(Set<`a> s,`a elt);

// union of two sets
extern Set<`a> union_two<`a>(Set<`a> s1,Set<`a> s2);

// intersection of two sets
extern Set<`a> intersect<`a>(Set<`a> s1,Set<`a> s2);

// set difference -- remove from s1 all elements in s2
extern Set<`a> diff<`a>(Set<`a> s1,Set<`a> s2);

// deletes an element if it's present, otherwise returns same set
extern Set<`a> delete<`a>(Set<`a> s,`a elt);

// number of unique elements in the set
extern int cardinality<`a>(Set<`a> s);

// true when the set is empty
extern bool is_empty<`a>(Set<`a> s);

// true when elt is in the set
extern bool member<`a>(Set<`a> s,`a elt);

// true when s1 is a (not necessarily proper) subset of s2
extern bool subset<`a>(Set<`a> s1,Set<`a> s2);

// true when s1 and s2 contain the same elements
extern int  compare<`a>(Set<`a> s1,Set<`a> s2);
extern bool equals <`a>(Set<`a> s1,Set<`a> s2);

// returns a list of the elements (in no guarateed order)
extern list<`a> elements<`a>(Set<`a> s);

// if s = {x1,x2,...,xn} then return f(x1,f(x2,f(...,f(xn,accum)...)))
extern `b fold<`a,`b>(`b f(`a,`b),Set<`a> s,`b accum);
extern `b fold_c<`a,`b,`c>(`b f(`c,`a,`b),`c env,Set<`a> s,`b accum);

// apply the function f to each element of the set and discard the result
extern void app<`a,`b>(`b f(`a),Set<`a> s);
extern void iter<`a>(void f(`a),Set<`a> s);
extern void iter_c<`a,`c>(void f(`c,`a),`c env,Set<`a> s);

extern xenum exn {Absent};
extern `a choose<`a>(Set<`a> s);

}

#endif
