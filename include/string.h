#ifndef _STRING_H_
#define _STRING_H_

#include "core.h"
#include "list.h"
#include "stdio.h" // for size_t

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

///////////////////////// LENGTH
extern size_t strlen(string s);

///////////////////////// COMPARISONS
// Return <0 if s1 < s2, 0 if s1 == s2, and >0 if s1 > s2.
// There are various flavors:
// strcmp is the main function.
// strncmp is like strcmp but compares no more than n characters.
//   (If n<0 strncmp returns 0) 
// strncasecmp is like strncmp but is case insensitive.
// zstrcmp and zstrncmp do not consider zero (null) characters
//   to be end-of-string markers. 
extern int strcmp(string s1, string s2);
extern int strptrcmp(stringptr s1, stringptr s2);
extern int strncmp(string s1, string s2, size_t len);
extern int strncasecmp(string s1, string s2, size_t len);
extern int zstrcmp(string,string);
extern int zstrncmp(string s1,string s2,size_t n);
extern int zstrptrcmp(stringptr,stringptr);

///////////////////////// CONCATENATION
// Destructive concatenation: first arg modified and returned
extern string strcat(string dest,string src);
// These functions return newly allocated strings
extern string strconcat(string,string);
extern string strconcat_l(glist_t<stringptr,`r>);
extern string str_sepstr(glist_t<stringptr,`r>,string);

///////////////////////// COPYING
// Non-allocating
extern string strcpy(string dest,string src); 
extern string strncpy(string,int,string,int,size_t);
extern string zstrncpy(string,int,string,int,size_t);
// Allocating
extern string expand(string s, size_t sz); /* like realloc */
extern string realloc_str(string str, size_t sz); /* conditional realloc */
extern string strdup(string src);

///////////////////////// TRANSFORMATIONS
// Return a substring of a string, by allocation.
extern string substring(string,int ofs, size_t n);

// replace last with second at end of first
// raise InvalidArg if second is not end of first.
extern string replace_suffix(string,string,string);

////////////////////////// SEARCHING
extern string strchr(string s, char c);
extern string strrchr(string s, char c);
extern string strpbrk(string s, string accept);
extern size_t strspn(string s, string accept);

////////////////////////// CONVERSIONS

extern list_t<int> explode(string s);
extern string implode(glist_t<int,`r> c);

// convert s to an int.  if offset is null, starts at offset 0. 
// otherwise, starts at *offset.  Skips leading whitespace.  Raises
// InvalidArg if no valid int is found.  If offset is non-null, 
// returns the index of the last character consumed + 1.
extern int to_int(string s, int *`r offset);

// convert s to a double.  if offset is null, starts at offset 0. 
// otherwise, starts at *offset.  Skips leading whitespace.  Raises
// InvalidArg if no valid int is found.  If offset is non-null, 
// returns the index of the last character consumed + 1.
extern double to_double(string s, int *`r offset); // raises InvalidArg
}

#endif
