/* This file is part of the Cyclone Library.
   Copyright (C) 2001 Greg Morrisett, AT&T

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

/* Some definitions taken from the GNU C Library, released under LGPL:
   Copyright (C) 1991-1999, 2000, 2001 Free Software Foundation, Inc. */

/*
 *	ISO C99 Standard: 7.20 General utilities	<stdlib.h>
 */

#ifndef _STDLIB_H
#define	_STDLIB_H	1

/* Get size_t from <stddef.h> */
#include <cstddef.h>
#include <core.h>
#include <array.h>

namespace Std {
extern "C" {

/* Returned by `div'.  */
typedef struct _Div
  {
    int quot;			/* Quotient.  */
    int rem;			/* Remainder.  */
  } div_t;

/* Returned by `ldiv'.  */
typedef struct _Ldiv
  {
    long int quot;		/* Quotient.  */
    long int rem;		/* Remainder.  */
  } ldiv_t;

/* The largest number rand will return (same as INT_MAX).  */
#define	RAND_MAX	2147483647

/* We define these the same for all machines.
   Changes from this to the outside world should be done in `_exit'.  */
#define	EXIT_FAILURE	1	/* Failing exit status.  */
#define	EXIT_SUCCESS	0	/* Successful exit status.  */

/* Maximum length of a multibyte character in the current locale.  */
/*
  // THIS IS HANDLED DIFFERENTLY IN LINUX:
#define	MB_CUR_MAX	(__ctype_get_mb_cur_max ())
extern size_t __ctype_get_mb_cur_max (void);
  // AND CYGWIN:
extern int __mb_cur_max;
#define MB_CUR_MAX __mb_cur_max
  // AND I DON'T KNOW WHO'S USING IT SO IT'S COMMENTED OUT FOR NOW
*/


/* Return the absolute value of X.  */
extern int abs (int __x);

/* Register a function to be called when `exit' is called.  */
extern int atexit (void (*__func) (void));

/* Return the `div_t' or `ldiv_t' representation
   of the value of NUMER over DENOM. */
/* GCC may have built-ins for these someday.  */
extern div_t div (int __numer, int __denom);
extern ldiv_t ldiv (long int __numer, long int __denom);

/* These are the functions that actually do things.  The `random', `srandom',
   `initstate' and `setstate' functions are those from BSD Unices.
   The `rand' and `srand' functions are required by the ANSI standard.
   We provide both interfaces to the same random number generator.  */
/* Return a random long integer between 0 and RAND_MAX inclusive.  */
extern long int random (void);

/* Seed the random number generator with the given number.  */
extern void srandom (unsigned int __seed);

/* Return a random integer between 0 and RAND_MAX inclusive.  */
extern int rand (void);
/* Seed the random number generator with the given number.  */
extern void srand (unsigned int __seed);
/* Reentrant interface according to POSIX.1.  */
extern int rand_r (unsigned int *__seed);

  // THE NEXT TWO FUNCTIONS MIGHT NEED TO BE COMMENTED OUT IN LINUX
/* Chown the slave to the calling user.  */
extern int grantpt (int __fd);

/* Release an internal lock so the slave can be opened.
   Call after grantpt().  */
extern int unlockpt (int __fd);

}

/* Convert a string to a floating-point number.  */
extern double atof(const char ?`r);
/* Convert a string to an integer.  */
extern int atoi(const char ?`r);
/* Convert a string to a long integer.  */
extern long atol(const char ?`r);

/* Return the value of envariable NAME, or NULL if it doesn't exist.  */
extern char ?getenv(const char ?);

/* Convert a string to a floating-point number.  */
extern double strtod(const char ?`r n, const char ?`r *`r2 end);
/* Convert a string to a long integer.  */
extern long strtol(const char ?`r n, const char ?`r *`r2 end, int base);
/* Convert a string to an unsigned long integer.  */
extern unsigned long strtoul(const char ?`r n,const char ?`r *`r2 end, int base);
extern unsigned long mstrtoul(char ?`r n,char ?`r *`r2 endptr,int base);
/* Here for compatibility reasons.  Will call Array::qsort */
  //extern void qsort<`a::B,`r::R>(`a ?`r tab, size_t nmemb, size_t szmemb, Array::cmpfn_t<`a,`r,`r>);
extern void qsort<`a::A,`r::R>(`a ?`r tab, size_t nmemb, sizeof_t<`a> szmemb, 
			    int (@`H compar)(const `a@`r, const `a@`r));

extern int system(string_t);

/* This doesn't do anything: it's just here so that we don't have to erase existing free's in C code */
/* Maybe there should be a warning if you actually use this. */
extern void free(`a::A ?);
}
#endif
