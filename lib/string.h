#ifndef STRING_H
#define STRING_H

#include "core.h"
#include "list.h"

namespace String {
using Core;
using List;

// A string is a char[?].  Most functions consider a zero (null)
// character as an end of string marker, so we may have
// strlen(s) < size(s).  Functions that do not follow this convention
// have names starting with 'z'.  Many functions accept a string
// and an offset; these functions may have names ending in 'o'.
// Functions whose arguments have range errors throw the
// Core::InvalidArg exn.

///////////////////////// LENGTH
extern size_t strlen(string s);
extern size_t strleno(string s, int ofs);

///////////////////////// COMPARISONS
// Return <0 if s1 < s2, 0 if s1 == s2, and >0 if s1 > s2.
// There are various flavors:
// strcmp is the main function.
// strcmpo is like strcmp but it starts comparing at offsets.
// strncmp is like strcmpo but compares no more than n characters.
//   (If n<0 strncmp returns 0) 
// strncasecmp is like strncmp but is case insensitive.
// zstrcmp and zstrncmp do not consider zero (null) characters
//   to be end-of-string markers. 
extern int strcmp(string s1, string s2);
extern int strcmpo(string s1, int ofs1, string s2, int ofs2);
extern int strncmp(string s1, int ofs1, string s2, int ofs2, size_t len);
extern int strncasecmp(string s1, int ofs1, string s2, int ofs2, size_t len);
extern int zstrcmp(string,string);
extern int zstrncmp(string s1,string s2,size_t n);

///////////////////////// CONCATENATION
// These functions modify their first arguments, and return them
extern string strcat(string dest,string src);
extern string strcato(string dest,string src,int srcOfs);
// These functions return newly allocated strings
extern string strconcat(string,string);
extern string strconcat_l(list<string>);
extern string str_sepstr(list<string>,string);

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
extern int strchr(string s, int ofs, char c);
extern int strrchr(string s, int ofs, char c);
extern int strpbrk(string s, int ofs, string accept);
extern int strspn(string s, int ofs, string accept);

////////////////////////// CONVERSIONS

extern list<Char> explode(string s);
extern string implode(list<Char> c);

// convert s to an int.  if offset is null, starts at offset 0. 
// otherwise, starts at *offset.  Skips leading whitespace.  Raises
// InvalidArg if no valid int is found.  If offset is non-null, 
// returns the index of the last character consumed + 1.
extern int to_int<`r>(string s, int *`r offset);

// convert s to a double.  if offset is null, starts at offset 0. 
// otherwise, starts at *offset.  Skips leading whitespace.  Raises
// InvalidArg if no valid int is found.  If offset is non-null, 
// returns the index of the last character consumed + 1.
extern double to_double<`r>(string s, int *`r offset); // raises InvalidArg
}

#endif
