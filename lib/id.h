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


#ifndef _ID_H_
#define _ID_H_

#include <core.h>
#include <stdio.h>

namespace Id {
using Core;
using Stdio;

// id.h:    defines an identifiers used in compilers, type-checkers, etc.

// identifiers are abstract (though there's not much to them)
extern struct id;
typedef struct id @Id;

// make an id that is equal to other id's with the same string and int
extern Id id_make(string_t,int);

// make an id with input string, but not equal to any other id
extern Id id_new(string_t);

// Generate a new unique id from an existing one.
extern Id id_renew(Id);

// make an id with input string, equal to other ids with this string and int -1
extern Id id_of_string(string_t);

// same as id_new
extern Id id_unique(string_t);

// return string and int portions concatenated with a '$' in-between
extern mstring_t id_to_string(Id);

// print an identifier to FILE
extern void id_prn(FILE @,Id);

// return string portion of an id
extern mstring_t id_to_source(Id);

// defines a total ordering on ids, return 0 if equal, -1 if first arg is first,
//                                                     +1 if second arg is first
extern int id_compare(Id,Id);

}

#endif
