#ifndef _SLOWDICT_H_
#define _SLOWDICT_H_
////////////////////////////////////////////////////////////////////////////
// Popcorn library, file slowdict.h                                       //
// Copyright Greg Morrisett, Dan Grossman                                 //
// January 1999, all rights reserved                                      //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
#include "core.h"
#include "list.h"

namespace SlowDict {
using List;

// slowdict.h:   defines polymorphic, functional, finite maps from types 'a to 
//           'b, where the domain must have a total order.  Follows the
//           conventions of the Ocaml dict library as much as possible.
// unlike Dict, we support delete.

extern struct dict<`a,`b>;

typedef struct dict<`a,`b> @Dict<`a,`b>;

// Raised when a key is present but not expected (e.g., insert_new) 
extern xenum exn {Present};
// Raised when a key is not present but expected (e.g., lookup) 
extern xenum exn {Absent};

// Given a comparison function, return an empty dict. 
extern Dict<`a,`b> empty<`a,`b>(int comp(`a,`a));

// Determine whether a dict is empty 
extern bool is_empty<`a,`b>(Dict<`a,`b> d);

// Return true if entry indexed by key is present, false otherwise 
extern bool member<`a,`b>(Dict<`a,`b> d,`a key);

// Inserts a key/data pair into a dictionary, replacing any existing
// pair with the same key. 
extern Dict<`a,`b> insert<`a,`b>(Dict<`a,`b> d,`a key,`b data);

// Inserts a key/data pair into a dictionary, raising Present if
// there is any existing pair with the same key.
extern Dict<`a,`b> insert_new<`a,`b>(Dict<`a,`b> d,`a key,`b data);

// Insert a list of key/data pairs into a dictionary, replacing
// duplicate keys.
extern Dict<`a,`b> inserts<`a,`b>(Dict<`a,`b> d,list<$(`a,`b)@> kds);

// Return a dictionary containing exactly one key/data pair. 
extern Dict<`a,`b> singleton<`a,`b>(int comp(`a,`a),`a key,`b data);

// Lookup a key in the dictionary, returning its associated data. If the key
// is not present, raise Absent.
extern `b lookup<`a,`b>(Dict<`a,`b> d,`a key);

// Same as lookup but doesnt raise an exception -- rather, returns an
// option.
extern Core::Opt_t<`b> lookup_opt<`a,`b>(Dict<`a,`b> d,`a key);

// Delete a key/pair from the dict if present. 
extern Dict<`a,`b> delete<`a,`b>(Dict<`a,`b> d,`a key);

// Delete a key/pair from the dict.  Raise Absent if key doesn't exist 
extern Dict<`a,`b> delete_present<`a,`b>(Dict<`a,`b> d,`a key);

// Fold a function f across the dictionary yielding an accumulator. 
extern `c fold<`a,`b,`c>(`c f(`a,`b,`c),Dict<`a,`b> d,`c accum);
// Same but fold an unboxed closure across the dictionary
extern `c fold_c<`a,`b,`c,`d>(`c f(`d,`a,`b,`c),`d env,
			      Dict<`a,`b> dict,`c accum);

// Apply function f to every element in the dictionary.  Ignore result. 
extern void app<`a,`b,`c>(`c f(`a,`b),Dict<`a,`b> d);
// Same but apply an unboxed closure across the dictionary
extern void app_c<`a,`b,`c,`d>(`c f(`d,`a,`b),`d env,Dict<`a,`b> d);
// void versions of the above
extern void iter<`a,`b>(void f(`a,`b),Dict<`a,`b> d);
extern void iter_c<`a,`b,`c>(void f(`c,`a,`b),`c env,Dict<`a,`b> d);

// Given a function that maps 'b values to 'c values, convert an
// Dict<'a,'b> to a Dict<'a,'c> by applying the function to each
// data item.
extern Dict<`a,`c> map<`a,`b,`c>(`c f(`b),Dict<`a,`b> d);
// Same but map an unboxed closure across the dictionary
extern Dict<`a,`c> map_c<`a,`b,`c,`d>(`c f(`d,`b),`d env,Dict<`a,`b> d);

// Return a key/data pair (in this case -- the first one in the dict).
// If the dict is empty, raise Absent.
extern $(`a,`b)@ choose<`a,`b>(Dict<`a,`b> d);

// Return an association list containing all the elements
extern list<$(`a,`b)@> to_list<`a,`b>(Dict<`a,`b> d);

}
#endif
