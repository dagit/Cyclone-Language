#ifndef _SET_H_
#define _SET_H_
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

// `a is the type of the elements, `r is the region in which the data
// structures live, and `e is the effect of the comparison function.
extern struct Set<`a,`r::R,`e::E>;

// generic sets abstract the element type, region, and effect -- 
// but without region handles, there's no way to really create these.
typedef struct Set<`a,`r,`e> @gset_t<`a,`r,`e>;
// operations that don't take region handles manipulate sets in the heap
typedef struct Set<`a,`H,`e> @hset_t<`a,`e>;
// default sets use the heap and an empty effect
typedef struct Set<`a,`H,{}> @set_t<`a>;

// The set creation functions take a functional argument that takes
// two elements and returns 0 when equal, <0 when the first is less
// than the second, and >0 when the second is less than the first.

// create an empty set given a comparison function
extern hset_t<`a,`e> empty(int comp(`a,`a;`e));

// create a singleton set given a comparison function and element
extern hset_t<`a,`e> singleton(int comp(`a,`a;`e),`a x);

// create a set out of a comparison function and a list of elements
extern hset_t<`a,`e> from_list(int comp(`a,`a;`e),glist_t<`a,`r> x);

// insert an element into a set
extern hset_t<`a,`e> insert(hset_t<`a,`e> s,`a elt);

// union of two sets
extern hset_t<`a,`e> union_two(hset_t<`a,`e> s1,hset_t<`a,`e> s2);

// intersection of two sets
extern hset_t<`a,`e> intersect(hset_t<`a,`e> s1,hset_t<`a,`e> s2);

// set difference -- remove from s1 all elements in s2
extern hset_t<`a,`e> diff(hset_t<`a,`e> s1,hset_t<`a,`e> s2);

// deletes an element if it's present, otherwise returns same set
extern hset_t<`a,`e> delete(hset_t<`a,`e> s,`a elt);

// number of unique elements in the set
extern int cardinality(gset_t<`a,`r,`e> s);

// true when the set is empty
extern bool is_empty(gset_t<`a,`r,`e> s);

// true when elt is in the set
extern bool member(gset_t<`a,`r,`e> s,`a elt);

// true when s1 is a (not necessarily proper) subset of s2
extern bool subset(gset_t<`a,`r1,`e> s1,gset_t<`a,`r2,`e> s2);

// true when s1 and s2 contain the same elements
extern int  compare(gset_t<`a,`r1,`e> s1,gset_t<`a,`r2,`e> s2);
extern bool equals (gset_t<`a,`r1,`e> s1,gset_t<`a,`r2,`e> s2);

// returns a list of the elements (in no guarateed order)
extern list_t<`a> elements(hset_t<`a,`e> s);

// if s = {x1,x2,...,xn} then return f(x1,f(x2,f(...,f(xn,accum)...)))
extern `b fold(`b f(`a,`b),gset_t<`a,`r,`e> s,`b accum);
extern `b fold_c(`b f(`c,`a,`b),`c env,gset_t<`a,`r,`e> s,`b accum);

// apply the function f to each element of the set and discard the result
extern void app(`b f(`a),gset_t<`a,`r,`e> s);
extern void iter(void f(`a),gset_t<`a,`r,`e> s);
extern void iter_c(void f(`c,`a),`c env,gset_t<`a,`r,`e> s);

extern xtunion exn {Absent};
extern `a choose(gset_t<`a,`r,`e> s);

}

#endif
