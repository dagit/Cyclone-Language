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
#include <cycboot.h> // for size_t

// A string is a char[?].  Most functions consider a zero (NUL)
// character as an end of string marker, so we may have
// strlen(s) < size(s).  Functions that do not follow this convention
// have names starting with 'z'.  Many functions accept a string
// and an offset; these functions may have names ending in 'o'.
// Functions whose arguments have range errors throw the
// Core::Invalid_argument exn.

///////////////////////// WRAPPERS
extern "C" Cstring strerror(int errnum);

///////////////////////// LENGTH
extern size_t strlen(buffer_t s);

///////////////////////// COMPARISONS
// Return <0 if s1 < s2, 0 if s1 == s2, and >0 if s1 > s2.
// There are various flavors:
// strcmp is the main function.
// strncmp is like strcmp but compares no more than n characters.
//   (If n<0 strncmp returns 0) 
// strncasecmp is like strncmp but is case insensitive.
// zstrcmp and zstrncmp do not consider zero (NUL) characters
//   to be end-of-string markers. 
extern int strcmp(buffer_t s1, buffer_t s2);
extern int strptrcmp(stringptr_t s1, stringptr_t s2);
extern int strncmp(buffer_t s1, buffer_t s2, size_t len);
extern int zstrcmp(buffer_t,buffer_t);
extern int zstrncmp(buffer_t s1, buffer_t s2, size_t n);
extern int zstrptrcmp(stringptr_t, stringptr_t);
extern int strcasecmp(buffer_t, buffer_t);
extern int strncasecmp(buffer_t s1, buffer_t s2, size_t len);

///////////////////////// CONCATENATION
// Destructive concatenation: first arg modified and returned
extern mstring_t<`r> strcat(mstring_t<`r> dest,buffer_t src);
// These functions return newly allocated strings
extern mstring_t     strconcat(buffer_t,buffer_t);
extern mstring_t<`r> rstrconcat(region_t<`r>,buffer_t,buffer_t);
extern mstring_t     strconcat_l(List::list_t<stringptr_t>);
extern mstring_t<`r> rstrconcat_l(region_t<`r>,List::list_t<stringptr_t>);
extern mstring_t     str_sepstr(List::list_t<stringptr_t>,buffer_t);
extern mstring_t<`r> rstr_sepstr(region_t<`r>,List::list_t<stringptr_t>,buffer_t);

///////////////////////// COPYING
// Non-allocating
extern mstring_t<`r> strcpy(mstring_t<`r> dest,buffer_t src); 
extern mbuffer_t<`r> strncpy(mbuffer_t<`r>,buffer_t,size_t);
extern mbuffer_t<`r> zstrncpy(mbuffer_t<`r>,buffer_t,size_t);

// realloc
extern mbuffer_t realloc(mbuffer_t<`H>, size_t);
extern mbuffer_t<`r> rrealloc(region_t<`r::TR>,mbuffer_t s, size_t sz);

// memcpy and memmove behave like C's except that instead of
// calling it memcpy(d,s,n*sizeof(t)), separate out the sizeof(t).
extern `a::A?`r _memcpy(`a?`r d, const `a? s, size_t, Core::sizeof_t<`a>);
extern `a::A?`r _memmove(`a?`r d, const `a? s, size_t, Core::sizeof_t<`a>);
extern int memcmp(buffer_t s1, buffer_t s2, size_t n);
extern buffer_t<`r> memchr(buffer_t<`r> s, char c, size_t n);
extern mbuffer_t<`r> mmemchr(mbuffer_t<`r> s, char c, size_t n);
extern mbuffer_t<`r> memset(mbuffer_t<`r> s, char c, size_t n);
extern void bzero(mbuffer_t s, size_t n);
extern void _bcopy(const `a::A? src, `a?`r dst, size_t n, Core::sizeof_t<`a> sz);

// jcheney: might want to define this so that partial copies are an error 
//instead of silently copying up to the end of the last-touched record
#define _rounddiv(q,d) (((q)/(d)) + ((((q)%(d)) == 0) ? 0 : 1))

#define memcpy(d,s,n) _memcpy((d),(s),_rounddiv(n,sizeof(*(s))),sizeof(*(s)))
#define memmove(d,s,n) _memmove((d),(s),_rounddiv(n,sizeof(*(s))),sizeof(*(s)))
#define bcopy(s,d,n) _bcopy((s),(d),_rounddiv(n,sizeof(*(s))),sizeof(*(s)))
                                        
// Allocating
extern mstring_t     expand(buffer_t s, size_t sz); /* like realloc */
extern mstring_t<`r> rexpand(region_t<`r::TR>,buffer_t s, size_t sz);
extern mstring_t     realloc_str(mstring_t<`H> str, size_t sz);
extern mstring_t<`r> cond_rrealloc_str(region_t<`r::TR> r, mstring_t<`r> str, size_t sz);
/* cond_rrealloc_str returns NULL if the given string is >= the given size */
extern mstring_t     strdup(buffer_t src);
extern mstring_t<`r> rstrdup(region_t<`r::TR>,buffer_t src);

///////////////////////// TRANSFORMATIONS
// Return a substring of a string, by allocation.
extern mstring_t     substring(buffer_t,int ofs, size_t n);
extern mstring_t<`r> rsubstring(region_t<`r>,buffer_t,int ofs, size_t n);

// replace last with second at end of first
// raise Invalid_argument if second is not end of first.
extern mstring_t     replace_suffix(buffer_t,buffer_t,buffer_t);
extern mstring_t<`r> rreplace_suffix (region_t<`r> r,buffer_t src, 
                                      buffer_t curr_suffix, 
                                      buffer_t new_suffix);

////////////////////////// SEARCHING
extern string_t<`r>  strchr(string_t<`r> s, char c);
extern mstring_t<`r> mstrchr(mstring_t<`r> s, char c);
extern mstring_t<`r> mstrrchr(mstring_t<`r> s, char c);
extern string_t<`r>  strrchr(string_t<`r> s, char c);
extern mstring_t<`r> mstrstr(mstring_t<`r> haystack, string_t needle);
extern string_t<`r>  strstr(string_t<`r> haystack, string_t needle);
extern string_t<`r>  strpbrk(string_t<`r> s, buffer_t accept);
extern mstring_t<`r> mstrpbrk(mstring_t<`r> s, buffer_t accept);
extern size_t        strspn(string_t s, string_t accept);
extern size_t        strcspn(string_t s, string_t accept);
extern mstring_t<`H> strtok (mstring_t<`H> s, string_t delim);

////////////////////////// CONVERSIONS

extern List::list_t<int>    explode(buffer_t s);
extern List::list_t<int,`r> rexplode(region_t<`r>,buffer_t s);
extern mstring_t      implode(List::list_t<int> c);

#endif
