/* This file is part of the Cyclone Library.
   Copyright (C) 1999-2001 Greg Morrisett, Dan Grossman

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

#ifndef _SLOWDICT_H_
#define _SLOWDICT_H_

#include <core.h>
#include <list.h>

namespace SlowDict {
using List;

// slowdict.h:   defines polymorphic, functional, finite maps from types 'a to 
//           'b, where the domain must have a total order.  Follows the
//           conventions of the Ocaml dict library as much as possible.
// unlike Dict, we support delete.

// TODO: add region support

extern struct Dict<`a,`b>;
typedef struct Dict<`a,`b> @ dict_t<`a,`b>;

// Raised when a key is present but not expected (e.g., insert_new) 
extern xtunion exn {extern Present};
// Raised when a key is not present but expected (e.g., lookup) 
extern xtunion exn {extern Absent};

// Given a comparison function, return an empty dict. 
extern dict_t<`a,`b> empty(int comp(`a,`a));

// Determine whether a dict is empty 
extern bool is_empty(dict_t<`a,`b> d);

// Return true if entry indexed by key is present, false otherwise 
extern bool member(dict_t<`a,`b> d,`a key);

// Inserts a key/data pair into a dictionary, replacing any existing
// pair with the same key. 
extern dict_t<`a,`b> insert(dict_t<`a,`b> d,`a key,`b data);

// Inserts a key/data pair into a dictionary, raising Present if
// there is any existing pair with the same key.
extern dict_t<`a,`b> insert_new(dict_t<`a,`b> d,`a key,`b data);

// Insert a list of key/data pairs into a dictionary, replacing
// duplicate keys.
extern dict_t<`a,`b> 
  inserts(dict_t<`a,`b> d,list_t<$(`a,`b)@> kds);

// Return a dictionary containing exactly one key/data pair. 
extern dict_t<`a,`b> singleton(int comp(`a,`a),`a key,`b data);

// Lookup a key in the dictionary, returning its associated data. If the key
// is not present, raise Absent.
extern `b lookup(dict_t<`a,`b> d,`a key);

// Same as lookup but doesnt raise an exception -- rather, returns an
// option.
extern Core::opt_t<`b> lookup_opt(dict_t<`a,`b> d,`a key);

// Delete a key/pair from the dict if present. 
extern dict_t<`a,`b> delete(dict_t<`a,`b> d,`a key);

// Delete a key/pair from the dict.  Raise Absent if key doesn't exist 
extern dict_t<`a,`b> delete_present(dict_t<`a,`b> d,`a key);

// Fold a function f across the dictionary yielding an accumulator. 
extern `c fold(`c f(`a,`b,`c),dict_t<`a,`b> d,`c accum);
// Same but fold an unboxed closure across the dictionary
extern `c fold_c(`c f(`d,`a,`b,`c),`d env, dict_t<`a,`b> dict,`c accum);

// Apply function f to every element in the dictionary.  Ignore result. 
extern void app(`c f(`a,`b),dict_t<`a,`b> d);
// Same but apply an unboxed closure across the dictionary
extern void app_c(`c f(`d,`a,`b),`d env,dict_t<`a,`b> d);
// void versions of the above
extern void iter(void f(`a,`b),dict_t<`a,`b> d);
extern void iter_c(void f(`c,`a,`b),`c env,dict_t<`a,`b> d);

// Given a function that maps 'b values to 'c values, convert an
// dict_t<'a,'b> to a dict_t<'a,'c> by applying the function to each
// data item.
extern dict_t<`a,`c> map(`c f(`b),dict_t<`a,`b> d);
// Same but map an unboxed closure across the dictionary
extern dict_t<`a,`c> map_c(`c f(`d,`b),`d env,dict_t<`a,`b> d);

// Return a key/data pair (in this case -- the first one in the dict).
// If the dict is empty, raise Absent.
extern $(`a,`b)@ choose(dict_t<`a,`b> d);

// Return an association list containing all the elements
extern list_t<$(`a,`b)@> to_list(dict_t<`a,`b> d);

}
#endif
