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
extern size_t strlen(const char ?`r s);

///////////////////////// COMPARISONS
// Return <0 if s1 < s2, 0 if s1 == s2, and >0 if s1 > s2.
// There are various flavors:
// strcmp is the main function.
// strncmp is like strcmp but compares no more than n characters.
//   (If n<0 strncmp returns 0) 
// strncasecmp is like strncmp but is case insensitive.
// zstrcmp and zstrncmp do not consider zero (null) characters
//   to be end-of-string markers. 
extern int strcmp(const char ?`r1 s1, const char ?`r2 s2);
extern int strptrcmp(const char ?`r1@`r2 s1, const char ?`r3@`r4 s2);
extern int strncmp(const char ?`r1 s1, const char ?`r2 s2, size_t len);
extern int strncasecmp(const char ?`r1 s1, const char ?`r2 s2, size_t len);
extern int zstrcmp(const char ?`r1,const char ?`r2);
extern int zstrncmp(const char ?`r1 s1,const char ?`r2 s2,size_t n);
extern int zstrptrcmp(const char ?`r1@`r2,const char ?`r3@`r4);

///////////////////////// CONCATENATION
// Destructive concatenation: first arg modified and returned
extern char ?`r1 strcat(char ?`r1 dest,const char ?`r2 src);
// These functions return newly allocated strings
extern char ? strconcat(const char ?`r1,const char ?`r2);
extern char ?`r rstrconcat(region_t<`r>,const char ?`r1,const char ?`r2);
extern char ? strconcat_l(glist_t<const char ?`r1@`r2,`r>);
extern char ?`r rstrconcat_l(region_t<`r>,glist_t<const char ?`r1@`r2,`r3>);
extern char ? str_sepstr(glist_t<const char ?`r1@`r2,`r>,const char ?`r3);
extern char ?`r rstr_sepstr(region_t<`r>,glist_t<const char ?`r1@`r2,`r3>,const char ?`r4);

///////////////////////// COPYING
// Non-allocating
extern char ?`r1 strcpy(char ?`r1 dest,const char ?`r2 src); 
extern char ?`r1 strncpy(char ?`r1,int,const char ?`r2,int,size_t);
extern char ?`r1 zstrncpy(char ?`r1,int,const char ?`r2,int,size_t);
// Allocating
extern char ? expand(const char ?`r1 s, size_t sz); /* like realloc */
extern char ?`r rexpand(region_t<`r>,const char ?`r1 s, size_t sz);
extern char ? realloc_str(char ? str, size_t sz); /* conditional realloc */
extern char ?`r rrealloc_str(region_t<`r> r, char ?`r str, size_t sz);
extern char ? strdup(const char ?`r1 src);
extern char ?`r rstrdup(region_t<`r>,const char ?`r1 src);

///////////////////////// TRANSFORMATIONS
// Return a substring of a string, by allocation.
extern char ? substring(const char ?`r1,int ofs, size_t n);
extern char ?`r rsubstring(region_t<`r>,const char ?`r1,int ofs, size_t n);

// replace last with second at end of first
// raise InvalidArg if second is not end of first.
extern char ? replace_suffix(const char ?`r1,const char ?`r2,
                             const char ?`r3);
extern char ?`r rreplace_suffix (region_t<`r> r,const char ?`r1 src, 
                                 const char ?`r2 curr_suffix, 
                                 const char ?`r3 new_suffix);

////////////////////////// SEARCHING
extern const char ?`r1 strchr(const char ?`r1 s, char c);
extern char ?`r1 strrchr(char ?`r1 s, char c);
extern char ?`r1 strpbrk(char ?`r1 s, const char ?`r2 accept);
extern size_t strspn(const char ?`r1 s, const char ?`r2 accept);

////////////////////////// CONVERSIONS

extern list_t<int> explode(const char ?`r s);
extern glist_t<int,`r> rexplode(region_t<`r>,const char ?`r1 s);
extern char ? implode(glist_t<int,`r> c);

// convert s to an int.  if offset is null, starts at offset 0. 
// otherwise, starts at *offset.  Skips leading whitespace.  Raises
// InvalidArg if no valid int is found.  If offset is non-null, 
// returns the index of the last character consumed + 1.
extern int to_int(const char ?`r1 s, int *`r offset);

// convert s to a double.  if offset is null, starts at offset 0. 
// otherwise, starts at *offset.  Skips leading whitespace.  Raises
// InvalidArg if no valid int is found.  If offset is non-null, 
// returns the index of the last character consumed + 1.
extern double to_double(const char ?`r1 s, int *`r offset); // raises InvalidArg
}

#endif
