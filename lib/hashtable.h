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

// TODO: add region support

namespace Hashtable {

extern struct Table<`a,`b,`e1::E,`e2::E>;
typedef struct Table<`a,`b,`e1,`e2> @table_t<`a,`b,`e1,`e2>;

// if cmp(x,y) is 0 then it better be that hash(x) == hash(y) 
extern table_t<`a,`b,`e1,`e2> create(int sz, int cmp(`a,`a;`e1), 
                                   int hash(`a;`e2));
extern void insert(table_t<`a,`b,`e1,`e2> t, `a key, `b val);
extern `b lookup(table_t<`a,`b,`e1,`e2> t, `a key); // raises Not_found
extern void resize(table_t<`a,`b,`e1,`e2> t);

// remove only most recent binding.  Silently return if none.  Does a
// destructive list update since this data structure is not functional
extern void remove(table_t<`a,`b,`e1,`e2> t, `a key);

extern int hash_string(string_t s);
extern int hash_stringptr(stringptr_t s);
extern void iter(void f(`a,`b), table_t<`a,`b,`e1,`e2> t);

// debugging
extern 
void print_table_map(table_t<`a,`b,`e1,`e2> t, void prn_key(`a), 
                     void prn_val(`b));

}
#endif
