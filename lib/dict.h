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

#include <list.h>

// The entire interface is the same as SlowDict, except there is no
// delete_present.
// Mathieu : now a few more functions

// TODO:  add region support

namespace Dict {
  using List {

extern struct Dict<`a,`b,`r::R,`e::E>;
typedef struct Dict<`a,`b,`r,`e> @`r gdict_t<`a,`b,`r,`e>;
typedef gdict_t<`a,`b,`H,`e> hdict_t<`a,`b,`e>;
typedef hdict_t<`a,`b,{}> dict_t<`a,`b>;

// Raised when a key is present but not expected (e.g., insert_new) 
extern xtunion exn {extern Present};
// Raised when a key is not present but expected (e.g., lookup) 
extern xtunion exn {extern Absent};

// Given a comparison function, return an empty dict. 
extern hdict_t<`a,`b,`e> empty(int comp(`a,`a;`e));
extern gdict_t<`a,`b,`r,`e> rempty(region_t<`r>,int comp(`a,`a;`e));

// Determine whether a dict is empty 
extern bool is_empty(gdict_t<`a,`b,`r,`e> d);

// Return true if entry indexed by key is present, false otherwise 
extern bool member(gdict_t<`a,`b,`r,`e> d,`a key);

// Inserts a key/data pair into a dictionary, replacing any existing
// pair with the same key. 
extern gdict_t<`a,`b,`r,`e> insert(gdict_t<`a,`b,`r,`e> d,`a key,`b data);

// Inserts a key/data pair into a dictionary, raising Present if
// there is any existing pair with the same key.
extern gdict_t<`a,`b,`r,`e> insert_new(gdict_t<`a,`b,`r,`e> d,`a key,`b data);

// Insert a list of key/data pairs into a dictionary, replacing
// duplicate keys.
extern gdict_t<`a,`b,`r,`e> inserts(gdict_t<`a,`b,`r,`e> d,
                                    glist_t<$(`a,`b)@`r1,`r2> kds);

// Return a dictionary containing exactly one key/data pair. 
extern hdict_t<`a,`b,`e> singleton(int comp(`a,`a;`e),`a key,`b data);
extern gdict_t<`a,`b,`r,`e> rsingleton(region_t<`r>,
                                       int comp(`a,`a;`e),`a key,`b data);

// Lookup a key in the dictionary, returning its associated data. If the key
// is not present, raise Absent.
extern `b lookup(gdict_t<`a,`b,`r,`e> d,`a key);

// Same as lookup but doesnt raise an exception -- rather, returns an option.
extern Core::opt_t<`b> lookup_opt(gdict_t<`a,`b,`r,`e> d,`a key);

// A third version of lookup that if present assigns through its third param
// and returns true, else returns false.
extern bool lookup_bool(gdict_t<`a,`b,`r2,`d> d, `a key, `b @`r ans_place);

// Yet another version of lookup that allocates the option in a region
extern `b*`res rlookup_opt(region_t<`res>,gdict_t<`a,`b,`r,`e> d,`a key);

// Fold a function f across the dictionary yielding an accumulator. 
extern `c fold(`c f(`a,`b,`c),gdict_t<`a,`b,`r,`e> d,`c accum);
// Same but fold an unboxed closure across the dictionary
extern `c fold_c(`c f(`d,`a,`b,`c),`d env,gdict_t<`a,`b,`r,`e> dict,`c accum);

// Apply function f to every element in the dictionary.  Ignore result. 
extern void app(`c f(`a,`b),gdict_t<`a,`b,`r,`e> d);
// Same but apply an unboxed closure across the dictionary
extern void app_c(`c f(`d,`a,`b),`d env,gdict_t<`a,`b,`r,`e> d);
// void versions of the above
extern void iter(void f(`a,`b),gdict_t<`a,`b,`r,`e> d);
extern void iter_c(void f(`c,`a,`b),`c env,gdict_t<`a,`b,`r,`e> d);

// the next 3 all raise Absent if an element of d1 is not in d2
extern void iter2(void (@f)(`b,`b),
		  gdict_t<`a,`b,`r1,`e1> d1, 
		  gdict_t<`a,`b,`r2,`e2> d2);
extern void iter2_c(void (@f)(`c,`b,`b), `c env,
		    gdict_t<`a,`b,`r1,`e1> d1, 
		    gdict_t<`a,`b,`r2,`e2> d2);
extern `c fold2_c(`c (@f)(`d,`a,`b1,`b2,`c), `d inner_env,
		  gdict_t<`a,`b1,`r1,`e1> d1, 
		  gdict_t<`a,`b2,`r2,`e2> d2,
		  `c accum);


// make a copy of a dictionary into a new region -- same as rmap'ing
// the identity function -- useful to do something like a union into
// a new region
extern gdict_t<`a,`b,`r2,`e> rcopy(region_t<`r2>, gdict_t<`a,`b,`r1,`e>);
extern hdict_t<`a,`b,`e> copy(gdict_t<`a,`b,`r1,`e>);


// Given a function that maps 'b values to 'c values, convert an
// gdict_t<'a,'b> to a gdict_t<'a,'c> by applying the function to each
// data item.
extern hdict_t<`a,`c,`e> map(`c f(`b),gdict_t<`a,`b,`r,`e> d);
extern gdict_t<`a,`c,`r2,`e> rmap(region_t<`r2>,
                                  `c f(`b),gdict_t<`a,`b,`r1,`e> d);
// Same but map an unboxed closure across the dictionary
extern hdict_t<`a,`c,`e> map_c(`c f(`d,`b),`d env,gdict_t<`a,`b,`r,`e> d);
extern gdict_t<`a,`c,`r2,`e> rmap_c(region_t<`r2>, `c f(`d,`b),`d env,
                                    gdict_t<`a,`b,`r1,`e> d);

// Combine two dicts. Domain is union of old domains.  For values in both
// dictionaries, compute new value using f.
extern gdict_t<`a,`b,`r2,`e> union_two(`b (@f)(`b,`b),
                                       gdict_t<`a,`b,`r1,`e> d1,
                                       gdict_t<`a,`b,`r2,`e> d2);

extern gdict_t<`a,`b,`r1,`e> intersect(`b (@f)(`b,`b),
                                       gdict_t<`a,`b,`r1,`e> d1, 
                                       gdict_t<`a,`b,`r1,`e> d2);

extern gdict_t<`a,`b,`r1,`e> intersect_c(`b (@f)(`c,`b,`b), `c env,
                                         gdict_t<`a,`b,`r1,`e> d1, 
                                         gdict_t<`a,`b,`r1,`e> d2);

extern bool forall_c(bool f(`c,`a,`b), `c env, gdict_t<`a,`b,`r,`e> d);

extern bool forall_intersect(bool (@f)(`a,`b,`b), 
			     gdict_t<`a,`b,`r1,`d> d1,
			     gdict_t<`a,`b,`r2,`d> d2);
// Return a key/data pair (in this case -- the first one in the dict).
// If the dict is empty, raise Absent.
extern $(`a,`b)@ choose(gdict_t<`a,`b,`r,`e> d);
extern $(`a,`b)@`r rchoose(region_t<`r>,gdict_t<`a,`b,`r2,`e> d);

// Return an association list containing all the elements
extern list_t<$(`a,`b)@> to_list(gdict_t<`a,`b,`r,`e> d);
extern glist_t<$(`a,`b)@`r,`r> rto_list(region_t<`r>,gdict_t<`a,`b,`r2,`e> d);

// pretty inefficient...
extern hdict_t<`a,`b,`e> filter_c(bool f(`c,`a,`b), `c env, 
                                  gdict_t<`a,`b,`r,`e> d);

extern gdict_t<`a,`b,`r2,`e> rfilter_c(region_t<`r2>,
                                       bool f(`c,`a,`b), `c env, 
                                       gdict_t<`a,`b,`r1,`e> d);

extern hdict_t<`a,`b,`e> filter(bool f(`a,`b), gdict_t<`a,`b,`r,`e> d);
extern gdict_t<`a,`b,`r2,`e> rfilter(region_t<`r2>,
                                     bool f(`a,`b), gdict_t<`a,`b,`r1,`e> d);
// returns d1 - d2
extern hdict_t<`a,`b,`e> difference(gdict_t<`a,`b,`r1,`e> d1, 
                                    gdict_t<`a,`b,`r2,`e> d2); 

extern gdict_t<`a,`b,`r,`e> rdifference(region_t<`r>,
                                        gdict_t<`a,`b,`r1,`e> d1, 
                                        gdict_t<`a,`b,`r2,`e> d2); 

extern hdict_t<`a,`b,`e> delete(gdict_t<`a,`b,`r1,`e>, `a);
extern gdict_t<`a,`b,`r2,`e> rdelete(region_t<`r2>, gdict_t<`a,`b,`r1,`e>, `a);
// similar but result is in same region -- a bit faster when the object
// isn't actually in the region
extern gdict_t<`a,`b,`r,`e> rdelete_same(gdict_t<`a,`b,`r,`e>, `a);

}}
#endif
