#ifndef _STRING_H_
#define _STRING_H_

#include <core.h>
#include <list.h>
#include <stdio.h> // for size_t

namespace String {
using Core;
using List;
using Stdio; // for size_t

// A string is a char[?].  Most functions consider a zero (null)
// character as an end of string marker, so we may have
// strlen(s) < size(s).  Functions that do not follow this convention
// have names starting with 'z'.  Many functions accept a string
// and an offset; these functions may have names ending in 'o'.
// Functions whose arguments have range errors throw the
// Core::InvalidArg exn.

///////////////////////// WRAPPERS
extern mstring_t strerror(int);

///////////////////////// LENGTH
extern size_t strlen(string_t s);

///////////////////////// COMPARISONS
// Return <0 if s1 < s2, 0 if s1 == s2, and >0 if s1 > s2.
// There are various flavors:
// strcmp is the main function.
// strncmp is like strcmp but compares no more than n characters.
//   (If n<0 strncmp returns 0) 
// strncasecmp is like strncmp but is case insensitive.
// zstrcmp and zstrncmp do not consider zero (null) characters
//   to be end-of-string markers. 
extern int strcmp(string_t s1, string_t s2);
extern int strptrcmp(stringptr_t s1, stringptr_t s2);
extern int strncmp(string_t s1, string_t s2, size_t len);
extern int strncasecmp(string_t s1, string_t s2, size_t len);
extern int zstrcmp(string_t,string_t);
extern int zstrncmp(string_t s1,string_t s2,size_t n);
extern int zstrptrcmp(stringptr_t,stringptr_t);

///////////////////////// CONCATENATION
// Destructive concatenation: first arg modified and returned
extern mstring_t strcat(mstring_t dest,string_t src);
// These functions return newly allocated strings
extern mstring_t strconcat(string_t,string_t);
extern mstring_t strconcat_l(glist_t<stringptr_t,`r>);
extern mstring_t str_sepstr(glist_t<stringptr_t,`r>,string_t);

///////////////////////// COPYING
// Non-allocating
extern mstring_t strcpy(mstring_t dest,string_t src); 
extern mstring_t strncpy(mstring_t,int,string_t,int,size_t);
extern mstring_t zstrncpy(mstring_t,int,string_t,int,size_t);
// Allocating
extern mstring_t expand(string_t s, size_t sz); /* like realloc */
extern mstring_t realloc_str(mstring_t str, size_t sz); /* conditional realloc */
extern mstring_t strdup(string_t src);

///////////////////////// TRANSFORMATIONS
// Return a substring of a string, by allocation.
extern mstring_t substring(string_t,int ofs, size_t n);

// replace last with second at end of first
// raise InvalidArg if second is not end of first.
extern mstring_t replace_suffix(string_t,string_t,string_t);

////////////////////////// SEARCHING
extern string_t strchr(string_t s, char c);
extern mstring_t strrchr(mstring_t s, char c);
extern mstring_t strpbrk(mstring_t s, string_t accept);
extern size_t strspn(string_t s, string_t accept);

////////////////////////// CONVERSIONS

extern list_t<int> explode(string_t s);
extern mstring_t implode(glist_t<int,`r> c);

// convert s to an int.  if offset is null, starts at offset 0. 
// otherwise, starts at *offset.  Skips leading whitespace.  Raises
// InvalidArg if no valid int is found.  If offset is non-null, 
// returns the index of the last character consumed + 1.
extern int to_int(string_t s, int *`r offset);

// convert s to a double.  if offset is null, starts at offset 0. 
// otherwise, starts at *offset.  Skips leading whitespace.  Raises
// InvalidArg if no valid int is found.  If offset is non-null, 
// returns the index of the last character consumed + 1.
extern double to_double(string_t s, int *`r offset); // raises InvalidArg
}

#endif
