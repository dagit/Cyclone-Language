#ifndef _ID_H_
#define _ID_H_
////////////////////////////////////////////////////////////////////////////
// Popcorn library, file id.h                                             //
// Copyright Greg Morrisett, Dan Grossman                                 //
// January 1999, all rights reserved                                      //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

#include "core.h"
#include "stdio.h"

namespace Id {
using Core;
using Stdio;

// id.h:    defines an identifiers used in compilers, type-checkers, etc.

// identifiers are abstract (though there's not much to them)
extern struct id;
typedef struct id @Id;

// make an id that is equal to other id's with the same string and int
extern Id id_make(string,int);

// make an id with input string, but not equal to any other id
extern Id id_new(string);

// Generate a new unique id from an existing one.
extern Id id_renew(Id);

// make an id with input string, equal to other ids with this string and int -1
extern Id id_of_string(string);

// same as id_new
extern Id id_unique(string);

// return string and int portions concatenated with a '$' in-between
extern string id_to_string(Id);

// print an identifier to FILE
extern void id_prn(FILE @,Id);

// return string portion of an id
extern string id_to_source(Id);

// defines a total ordering on ids, return 0 if equal, -1 if first arg is first,
//                                                     +1 if second arg is first
extern int id_compare(Id,Id);

}

#endif
