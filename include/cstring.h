/* This file is part of the Cyclone Library.
   Copyright (C) 2001 Greg Morrisett, Mike Hicks, AT&T

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

#ifndef _STRING_H_
#define _STRING_H_

#include <core.h>
#include <list.h>
#include <cstddef.h> // for size_t

namespace Std {
using Core;
using List;

// A string is a char[?].  Most functions consider a zero (NUL)
// character as an end of string marker, so we may have
// strlen(s) < size(s).  Functions that do not follow this convention
// have names starting with 'z'.  Many functions accept a string
// and an offset; these functions may have names ending in 'o'.
// Functions whose arguments have range errors throw the
// Core::Invalid_argument exn.

///////////////////////// WRAPPERS
extern string_t strerror(int);

///////////////////////// LENGTH
extern size_t strlen(string_t s);

///////////////////////// COMPARISONS
// Return <0 if s1 < s2, 0 if s1 == s2, and >0 if s1 > s2.
// There are various flavors:
// strcmp is the main function.
// strncmp is like strcmp but compares no more than n characters.
//   (If n<0 strncmp returns 0) 
// strncasecmp is like strncmp but is case insensitive.
// zstrcmp and zstrncmp do not consider zero (NUL) characters
//   to be end-of-string markers. 
extern int strcmp(string_t s1, string_t s2);
extern int strptrcmp(stringptr_t s1, stringptr_t s2);
extern int strncmp(string_t s1, string_t s2, size_t len);
extern int zstrcmp(string_t,string_t);
extern int zstrncmp(string_t s1, string_t s2, size_t n);
extern int zstrptrcmp(stringptr_t, stringptr_t);

///////////////////////// CONCATENATION
// Destructive concatenation: first arg modified and returned
extern mstring_t<`r> strcat(mstring_t<`r> dest,string_t src);
// These functions return newly allocated strings
extern mstring_t     strconcat(string_t,string_t);
extern mstring_t<`r> rstrconcat(region_t<`r>,string_t,string_t);
extern mstring_t     strconcat_l(list_t<stringptr_t>);
extern mstring_t<`r> rstrconcat_l(region_t<`r>,list_t<stringptr_t>);
extern mstring_t     str_sepstr(list_t<stringptr_t>,string_t);
extern mstring_t<`r> rstr_sepstr(region_t<`r>,list_t<stringptr_t>,string_t);

///////////////////////// COPYING
// Non-allocating
extern mstring_t<`r> strcpy(mstring_t<`r> dest,string_t src); 
extern mstring_t<`r> strncpy(mstring_t<`r>,string_t,size_t);
extern mstring_t<`r> zstrncpy(mstring_t<`r>,string_t,size_t);
#define memcpy(d,s,z) *(d) = *(s) /* hack for now; won't work for strings */
// Allocating
extern mstring_t     expand(string_t s, size_t sz); /* like realloc */
extern mstring_t<`r> rexpand(region_t<`r>,string_t s, size_t sz);
extern mstring_t     realloc_str(mstring_t<`H> str, size_t sz); /* conditional realloc */
extern mstring_t<`r> rrealloc_str(region_t<`r> r, mstring_t<`r> str, size_t sz);
extern mstring_t     strdup(string_t src);
extern mstring_t<`r> rstrdup(region_t<`r>,string_t src);

///////////////////////// TRANSFORMATIONS
// Return a substring of a string, by allocation.
extern mstring_t     substring(string_t,int ofs, size_t n);
extern mstring_t<`r> rsubstring(region_t<`r>,string_t,int ofs, size_t n);

// replace last with second at end of first
// raise Invalid_argument if second is not end of first.
extern mstring_t     replace_suffix(string_t,string_t,string_t);
extern mstring_t<`r> rreplace_suffix (region_t<`r> r,string_t src, 
                                 string_t curr_suffix, 
                                 string_t new_suffix);

////////////////////////// SEARCHING
extern string_t<`r>  strchr(string_t<`r> s, char c);
extern mstring_t<`r> mstrchr(mstring_t<`r> s, char c);
extern mstring_t<`r> strrchr(mstring_t<`r> s, char c);
extern string_t<`r>  strstr(string_t<`r> haystack, string_t needle);
extern mstring_t<`r> strpbrk(mstring_t<`r> s, string_t accept);
extern size_t        strspn(string_t s, string_t accept);

////////////////////////// CONVERSIONS

extern list_t<int>    explode(string_t s);
extern list_t<int,`r> rexplode(region_t<`r>,string_t s);
extern mstring_t      implode(list_t<int> c);

}

#endif
