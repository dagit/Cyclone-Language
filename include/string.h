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
extern const char ? strerror(int);

///////////////////////// LENGTH
extern size_t strlen(const char ? s);

///////////////////////// COMPARISONS
// Return <0 if s1 < s2, 0 if s1 == s2, and >0 if s1 > s2.
// There are various flavors:
// strcmp is the main function.
// strncmp is like strcmp but compares no more than n characters.
//   (If n<0 strncmp returns 0) 
// strncasecmp is like strncmp but is case insensitive.
// zstrcmp and zstrncmp do not consider zero (null) characters
//   to be end-of-string markers. 
extern int strcmp(const char ? s1, const char ? s2);
extern int strptrcmp(const char ?@ s1, const char ?@ s2);
extern int strncmp(const char ? s1, const char ? s2, size_t len);
extern int strncasecmp(const char ? s1, const char ? s2, size_t len);
extern int zstrcmp(const char ?,const char ?);
extern int zstrncmp(const char ? s1,const char ? s2,size_t n);
extern int zstrptrcmp(const char ?@,const char ?@);

///////////////////////// CONCATENATION
// Destructive concatenation: first arg modified and returned
extern char ?`r strcat(char ?`r dest,const char ? src);
// These functions return newly allocated strings
extern char ? strconcat(const char ?,const char ?);
extern char ?`r rstrconcat(region_t<`r>,const char ?,const char ?);
extern char ? strconcat_l(list_t<const char ?@>);
extern char ?`r rstrconcat_l(region_t<`r>,list_t<const char ?@>);
extern char ? str_sepstr(list_t<const char ?@>,const char ?);
extern char ?`r rstr_sepstr(region_t<`r>,list_t<const char ?@>,const char ?);

///////////////////////// COPYING
// Non-allocating
extern char ?`r strcpy(char ?`r dest,const char ? src); 
extern char ?`r strncpy(char ?`r,const char ?,size_t);
extern char ?`r zstrncpy(char ?`r,const char ?,size_t);
// Allocating
extern char ? expand(const char ? s, size_t sz); /* like realloc */
extern char ?`r rexpand(region_t<`r>,const char ? s, size_t sz);
extern char ? realloc_str(char ?`H str, size_t sz); /* conditional realloc */
extern char ?`r rrealloc_str(region_t<`r> r, char ?`r str, size_t sz);
extern char ? strdup(const char ? src);
extern char ?`r rstrdup(region_t<`r>,const char ? src);

///////////////////////// TRANSFORMATIONS
// Return a substring of a string, by allocation.
extern char ? substring(const char ?,int ofs, size_t n);
extern char ?`r rsubstring(region_t<`r>,const char ?,int ofs, size_t n);

// replace last with second at end of first
// raise InvalidArg if second is not end of first.
extern char ? replace_suffix(const char ?,const char ?,const char ?);
extern char ?`r rreplace_suffix (region_t<`r> r,const char ? src, 
                                 const char ? curr_suffix, 
                                 const char ? new_suffix);

////////////////////////// SEARCHING
extern const char ?`r strchr(const char ?`r s, char c);
extern char ?`r strrchr(char ?`r s, char c);
extern char ?`r strpbrk(char ?`r s, const char ? accept);
extern size_t strspn(const char ? s, const char ? accept);

////////////////////////// CONVERSIONS

extern list_t<int> explode(const char ? s);
extern list_t<int,`r> rexplode(region_t<`r>,const char ? s);
extern char ? implode(list_t<int> c);

// convert s to an int.  if offset is null, starts at offset 0. 
// otherwise, starts at *offset.  Skips leading whitespace.  Raises
// InvalidArg if no valid int is found.  If offset is non-null, 
// returns the index of the last character consumed + 1.
extern int to_int(const char ? s, int * offset);

// convert s to a double.  if offset is null, starts at offset 0. 
// otherwise, starts at *offset.  Skips leading whitespace.  Raises
// InvalidArg if no valid int is found.  If offset is non-null, 
// returns the index of the last character consumed + 1.
extern double to_double(const char ? s, int * offset); // raises InvalidArg
}

#endif
