#ifndef _DICT_H_
#define _DICT_H_

#include "list.h"

// The entire interface is the same as SlowDict, except there is no
// delete or delete_present.

// TODO:  add region support

namespace Dict {
  using List {

extern struct Dict<`a,`b,`e::E>;
typedef struct Dict<`a,`b,`e> @ hdict_t<`a,`b,`e>;
typedef hdict_t<`a,`b,{}> dict_t<`a,`b>;

// Raised when a key is present but not expected (e.g., insert_new) 
extern xenum exn {Present};
// Raised when a key is not present but expected (e.g., lookup) 
extern xenum exn {Absent};

// Given a comparison function, return an empty dict. 
extern hdict_t<`a,`b,`e> empty(int comp(`a,`a;`e));

// Determine whether a dict is empty 
extern bool is_empty(hdict_t<`a,`b,`e> d);

// Return true if entry indexed by key is present, false otherwise 
extern bool member(hdict_t<`a,`b,`e> d,`a key);

// Inserts a key/data pair into a dictionary, replacing any existing
// pair with the same key. 
extern hdict_t<`a,`b,`e> insert(hdict_t<`a,`b,`e> d,`a key,`b data);

// Inserts a key/data pair into a dictionary, raising Present if
// there is any existing pair with the same key.
extern hdict_t<`a,`b,`e> insert_new(hdict_t<`a,`b,`e> d,`a key,`b data);

// Insert a list of key/data pairs into a dictionary, replacing
// duplicate keys.
extern hdict_t<`a,`b,`e> inserts(hdict_t<`a,`b,`e> d,
                                 glist_t<$(`a,`b)@`r1,`r2> kds);

// Return a dictionary containing exactly one key/data pair. 
extern hdict_t<`a,`b,`e> singleton(int comp(`a,`a;`e),`a key,`b data);

// Lookup a key in the dictionary, returning its associated data. If the key
// is not present, raise Absent.
extern `b lookup(hdict_t<`a,`b,`e> d,`a key);

// Same as lookup but doesnt raise an exception -- rather, returns an
// option.
extern Core::opt_t<`b> lookup_opt(hdict_t<`a,`b,`e> d,`a key);

// Fold a function f across the dictionary yielding an accumulator. 
extern `c fold(`c f(`a,`b,`c),hdict_t<`a,`b,`e> d,`c accum);
// Same but fold an unboxed closure across the dictionary
extern `c fold_c(`c f(`d,`a,`b,`c),`d env,hdict_t<`a,`b,`e> dict,`c accum);

// Apply function f to every element in the dictionary.  Ignore result. 
extern void app(`c f(`a,`b),hdict_t<`a,`b,`e> d);
// Same but apply an unboxed closure across the dictionary
extern void app_c(`c f(`d,`a,`b),`d env,hdict_t<`a,`b,`e> d);
// void versions of the above
extern void iter(void f(`a,`b),hdict_t<`a,`b,`e> d);
extern void iter_c(void f(`c,`a,`b),`c env,hdict_t<`a,`b,`e> d);

// Given a function that maps 'b values to 'c values, convert an
// hdict_t<'a,'b> to a hdict_t<'a,'c> by applying the function to each
// data item.
extern hdict_t<`a,`c,`e> map(`c f(`b),hdict_t<`a,`b,`e> d);
// Same but map an unboxed closure across the dictionary
extern hdict_t<`a,`c,`e> map_c(`c f(`d,`b),`d env,hdict_t<`a,`b,`e> d);

// Combine two dicts. Domain is union of old domains.  For values in both
// dictionaries, compute new value using f.
extern hdict_t<`a,`b,`e> union_two(`b (@f)(`b,`b),
                                  hdict_t<`a,`b,`e> d1,hdict_t<`a,`b,`e> d2);

extern hdict_t<`a,`b,`e> intersect(`b (@f)(`b,`b),
                                   hdict_t<`a,`b,`e> d1, hdict_t<`a,`b,`e> d2);

extern bool forall_c(bool f(`c,`a,`b), `c env, hdict_t<`a,`b,`e> d);

// Return a key/data pair (in this case -- the first one in the dict).
// If the dict is empty, raise Absent.
extern $(`a,`b)@ choose(hdict_t<`a,`b,`e> d);

// Return an association list containing all the elements
extern list_t<$(`a,`b)@> to_list(hdict_t<`a,`b,`e> d);

  }}
#endif
