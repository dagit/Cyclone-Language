/* This file is part of the Cyclone Library.
   Copyright (C) 2000-2001 Greg Morrisett, AT&T

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

#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_

#include <core.h> // for string_t, stringptr_t

namespace Hashtable {

  /*** \subsection{\texttt{<hashtable.h>}} */
  /*** Defines namespace Hashtable, which implements mappings from
       keys to values.  These hashtables are imperative---values are
       added and deleted destructively.  (Use namespace Dict or
       SlowDict if you need functional (non-destructive) mappings.)
       To enable an efficient implementation, you are required to
       provide a total order on keys (a comparison function).  */

extern struct Table<`a,`b,`r::R>;
typedef struct Table<`a,`b,`r> @`r table_t<`a,`b,`r>;
  /** A [table_t<`a,`b>] is a hash table with keys of type [`a]
      and values of type [`b]. */

extern table_t<`a,`b> create(int sz, int (@`H cmp)(`a,`a), int (@`H hash)(`a));
  /** [create(sz,cmp,hash)] returns a new hash table that starts out
      with [sz] buckets.  [cmp] should be a comparison function on
      keys: [cmp(k1,k2)] should return a number less than, equal to,
      or greater than 0 according to whether [k1] is less than, equal
      to, or greater than [k2].  [hash] should be a hash function on
      keys.  [cmp] and [hash] should satisfy the following property:
      if [cmp(k1,k2)] is 0, then [hash(k1)] must equal [hash(k2)]. */
extern table_t<`a,`b,`r> rcreate(region_t<`r> r, int sz, int (@`H cmp)(`a,`a), int (@`H hash)(`a));
  /** [rcreate(r,sz,cmp,hash)] is similar to create but allocates its
      result in the region r instead of the heap.  */
extern void insert(table_t<`a,`b> t, `a key, `b val);
  /** [insert(t,key,val)] binds [key] to [value] in [t]. */
extern `b lookup(table_t<`a,`b> t, `a key);
  /** [lookup(t,key)] returns the value associated with [key] in [t],
      or throws [Not_found] if there is no value associated with [key]
      in [t]. */
extern `b*`r lookup_opt(table_t<`a,`b,`r> t, `a key);
  /** [lookup_opt(t,key)] returns a pointer to the value associated 
      with [key] in [t], or NULL if there is no value associated with [key].*/
extern bool try_lookup(table_t<`a,`b> t, `a key, `b@data);
  /** [try_lookup(t,key,p)] tries to find the [key] in the table [t].
      If successful, it sets [*p] to the value associated with [key]
      and returns true.  If the [key] is not found, then try_lookup
      returns false. */
extern void resize(table_t<`a,`b> t);
  /** [resize(t)] increases the size (number of buckets) in table [t].
      [resize] is called automatically by functions like [insert] when
      the buckets of a hash table get large, however, it can also be
      called by the programmer explicitly. */
extern void remove(table_t<`a,`b> t, `a key);
  /** [remove(t,key)] removes the most recent binding of [key] from
      [t]; the next-most-recent binding of [key] (if any) is restored.
      If there is no value associated with [key] in [t], [remove]
      returns silently. */
extern int hash_string(string_t s);
  /** [hash_string(s)] returns a hash of a string [s].  It is provided
      as a convenience for making hash tables mapping strings to
      values. */
extern int hash_stringptr(stringptr_t p);
  /** [hash_stringptr(p)] returns a hash of a string pointer [p]. */

extern void iter(void f(`a,`b), table_t<`a,`b> t);
  /** [iter(f,t)] applies [f] to each key/value pair in [t]. */

extern void iter_c(void f(`a,`b,`c), table_t<`a,`b> t, `c env);
  /** [iter\_c(f,t,e)] calls [f(k,v,e)] for each key/value pair [(k,v)]. */

// debugging
extern 
void print_table_map(table_t<`a,`b> t, void prn_key(`a), 
                     void prn_val(`b));

}
#endif
