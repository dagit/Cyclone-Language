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

// TODO: add region support

namespace Hashtable {

  /*** \subsection{\texttt{<hashtable.h>}} */
  /*** Defines namespace Hashtable, which implements an imperative
       hash table datatype. */

extern struct Table<`a,`b>;
typedef struct Table<`a,`b> @table_t<`a,`b>;
  /** A [table_t<`a,`b>] is a hash table with keys of type [`a]
      and values of type [`b]. */

extern table_t<`a,`b> create(int sz, int cmp(`a,`a), int hash(`a));
  /** [create(sz,cmp,hash)] returns a new hash table that starts out
      with [sz] buckets.  [cmp] should be a comparison function on
      keys: [cmp(k1,k2)] should return a number less than, equal to,
      or greater than 0 according to whether [k1] is less than, equal
      to, or greater than [k2].  [hash] should be a hash function on
      keys, and if [cmp(k1,k2)] is 0, then [hash(k1)] must equal
      [hash(k2)]. */
extern void insert(table_t<`a,`b> t, `a key, `b val);
  /** [insert(t,key,val)] binds [key] to [value] in [t]. */
extern `b lookup(table_t<`a,`b> t, `a key);
  /** [lookup(t,key)] returns the value associated with [key] in [t],
      or throws [Not_found] if there is no value associate with [key]
      in [t]. */
extern void resize(table_t<`a,`b> t);
  /** [resize(t)] increases the size (number of buckets) in table [t]. */
extern void remove(table_t<`a,`b> t, `a key);
  /** [remove(t,key)] removes the most recent binding of [key] from
      [t].  If there is no value associated with [key] in [t],
      [remove] returns silently. */
extern int hash_string(string_t s);
  /** A hash function for strings. */
extern int hash_stringptr(stringptr_t s);
  /** A hash function for string pointers. */
extern void iter(void f(`a,`b), table_t<`a,`b> t);
  /** [iter(f,t)] applies [f] to each [(key,val)] pair in [t]. */

// debugging
extern 
void print_table_map(table_t<`a,`b> t, void prn_key(`a), 
                     void prn_val(`b));

}
#endif
