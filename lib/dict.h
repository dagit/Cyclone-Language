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

extern struct Dict<`a,`b,`r::R>;
typedef struct Dict<`a,`b,`r> @`r dict_t<`a,`b,`r>;

// Raised when a key is present but not expected (e.g., insert_new) 
extern xtunion exn {extern Present};
// Raised when a key is not present but expected (e.g., lookup) 
extern xtunion exn {extern Absent};

// Given a comparison function, return an empty dict. 
extern dict_t<`a,`b> empty(int comp(`a,`a));
extern dict_t<`a,`b,`r> rempty(region_t<`r>,int comp(`a,`a));

// Determine whether a dict is empty 
extern bool is_empty(dict_t d);

// Return true if entry indexed by key is present, false otherwise 
extern bool member(dict_t<`a> d,`a key);

// Inserts a key/data pair into a dictionary, replacing any existing
// pair with the same key. 
extern dict_t<`a,`b,`r> insert(dict_t<`a,`b,`r> d,`a key,`b data);

// Inserts a key/data pair into a dictionary, raising Present if
// there is any existing pair with the same key.
extern dict_t<`a,`b,`r> insert_new(dict_t<`a,`b,`r> d,`a key,`b data);

// Insert a list of key/data pairs into a dictionary, replacing
// duplicate keys.
extern dict_t<`a,`b,`r> inserts(dict_t<`a,`b,`r> d,
                                    list_t<$(`a,`b)@> kds);

// Return a dictionary containing exactly one key/data pair. 
extern dict_t<`a,`b> singleton(int comp(`a,`a),`a key,`b data);
extern dict_t<`a,`b,`r> rsingleton(region_t<`r>,
                                      int comp(`a,`a),`a key,`b data);

// Lookup a key in the dictionary, returning its associated data. If the key
// is not present, raise Absent.
extern `b lookup(dict_t<`a,`b> d,`a key);

// Same as lookup but doesnt raise an exception -- rather, returns an option.
extern Core::opt_t<`b> lookup_opt(dict_t<`a,`b> d,`a key);

// A third version of lookup that if present assigns through its third param
// and returns true, else returns false.
extern bool lookup_bool(dict_t<`a,`b> d, `a key, `b @`r ans_place);

// Yet another version of lookup that allocates the option in a region
extern `b*`res rlookup_opt(region_t<`res>,dict_t<`a,`b> d,`a key);

// Fold a function f across the dictionary yielding an accumulator. 
extern `c fold(`c f(`a,`b,`c),dict_t<`a,`b> d,`c accum);
// Same but fold an unboxed closure across the dictionary
extern `c fold_c(`c f(`d,`a,`b,`c),`d env,dict_t<`a,`b> dict,`c accum);

// Apply function f to every element in the dictionary.  Ignore result. 
extern void app(`c f(`a,`b),dict_t<`a,`b> d);
// Same but apply an unboxed closure across the dictionary
extern void app_c(`c f(`d,`a,`b),`d env,dict_t<`a,`b> d);
// void versions of the above
extern void iter(void f(`a,`b),dict_t<`a,`b> d);
extern void iter_c(void f(`c,`a,`b),`c env,dict_t<`a,`b> d);

// the next 3 all raise Absent if an element of d1 is not in d2
extern void iter2(void (@f)(`b,`b),dict_t<`a,`b> d1,dict_t<`a,`b> d2);
extern void iter2_c(void (@f)(`c,`b,`b), `c env,
		    dict_t<`a,`b> d1, dict_t<`a,`b> d2);
extern `c fold2_c(`c (@f)(`d,`a,`b1,`b2,`c), `d inner_env,
		  dict_t<`a,`b1> d1, dict_t<`a,`b2> d2, `c accum);


// make a copy of a dictionary into a new region -- same as rmap'ing
// the identity function -- useful to do something like a union into
// a new region
extern dict_t<`a,`b,`r2> rcopy(region_t<`r2>, dict_t<`a,`b>);
extern dict_t<`a,`b> copy(dict_t<`a,`b>);


// Given a function that maps 'b values to 'c values, convert an
// dict_t<'a,'b> to a dict_t<'a,'c> by applying the function to each
// data item.
extern dict_t<`a,`c> map(`c f(`b),dict_t<`a,`b> d);
extern dict_t<`a,`c,`r2> rmap(region_t<`r2>,
                                 `c f(`b),dict_t<`a,`b> d);
// Same but map an unboxed closure across the dictionary
extern dict_t<`a,`c> map_c(`c f(`d,`b),`d env,dict_t<`a,`b> d);
extern dict_t<`a,`c,`r2> rmap_c(region_t<`r2>, `c f(`d,`b),`d env,
                                    dict_t<`a,`b> d);

// Combine two dicts. Domain is union of old domains.  For values in both
// dictionaries, compute new value using f.
extern dict_t<`a,`b,`r2> union_two(`b f(`b,`b),
                                       dict_t<`a,`b> d1,
                                       dict_t<`a,`b,`r2> d2);

extern dict_t<`a,`b,`r1> intersect(`b (@f)(`b,`b),
                                       dict_t<`a,`b,`r1> d1, 
                                       dict_t<`a,`b,`r1> d2);

extern dict_t<`a,`b,`r1> intersect_c(`b (@f)(`c,`b,`b), `c env,
                                         dict_t<`a,`b,`r1> d1, 
                                         dict_t<`a,`b,`r1> d2);

extern bool forall_c(bool f(`c,`a,`b), `c env, dict_t<`a,`b,`r> d);

extern bool forall_intersect(bool f(`a,`b,`b), dict_t<`a,`b> d1,
			     dict_t<`a,`b> d2);
// Return a key/data pair (in this case -- the first one in the dict).
// If the dict is empty, raise Absent.
extern $(`a,`b)@ choose(dict_t<`a,`b> d);
extern $(`a,`b)@`r rchoose(region_t<`r>,dict_t<`a,`b> d);

// Return an association list containing all the elements
extern list_t<$(`a,`b)@> to_list(dict_t<`a,`b> d);
extern list_t<$(`a,`b)@`r,`r> rto_list(region_t<`r>,dict_t<`a,`b> d);

// pretty inefficient...
extern dict_t<`a,`b> filter_c(bool f(`c,`a,`b), `c env, 
                                 dict_t<`a,`b> d);

extern dict_t<`a,`b,`r2> rfilter_c(region_t<`r2>,
                                      bool f(`c,`a,`b), `c env, 
                                      dict_t<`a,`b> d);

extern dict_t<`a,`b> filter(bool f(`a,`b), dict_t<`a,`b> d);
extern dict_t<`a,`b,`r2> rfilter(region_t<`r2>,
                                    bool f(`a,`b), dict_t<`a,`b> d);
// returns d1 - d2
extern dict_t<`a,`b> difference(dict_t<`a,`b> d1, 
                                   dict_t<`a,`b> d2); 

extern dict_t<`a,`b,`r> rdifference(region_t<`r>,
                                       dict_t<`a,`b> d1, 
                                       dict_t<`a,`b> d2); 

extern dict_t<`a,`b> delete(dict_t<`a,`b>, `a);
extern dict_t<`a,`b,`r2> rdelete(region_t<`r2>, dict_t<`a,`b>, `a);
// similar but result is in same region -- a bit faster when the object
// isn't actually in the region
extern dict_t<`a,`b,`r> rdelete_same(dict_t<`a,`b,`r>, `a);

}}
#endif
