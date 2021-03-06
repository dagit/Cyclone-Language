/* config.h.  Generated automatically by configure.  */
/* This file is part of the RC compiler.
   Copyright (C) 2000-2001 The Regents of the University of California.

RC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

RC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with RC; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

#ifndef CONFIG_H
#define CONFIG_H

/* Configuration section */

#define CC1PATH "/usr/lib/gcc-lib/i386-redhat-linux/2.96/cc1"

/* defined if char is an unsigned type (note that for type checking purposes,
   char,unsigned char and signed char are always distinct) */
/* #undef __CHAR_UNSIGNED__ */

#define SIZEOF_LONG_LONG 8

#define SIZEOF_LONG_DOUBLE 12
#define SIZEOF_DOUBLE 8

#define HAVE_STRTOLD 1

/* Target type structure information.
   This setup assumes that we are compiling on self, with gcc 
*/

/* Assume the target machine of the compilation is the current machine, 
   and other miscellaneous assumptions marked under ASSSUME: in types.c.
   This allows constant folding and correct handling of types. */
#define SELF_TARGET

/* Seems unlikely to change, but... */
#define BITSPERBYTE 8U

/* Largest signed and unsigned int types (for constants and constant folding) */
typedef long long largest_int;
typedef unsigned long long largest_uint;

#define LARGEST_UINTBITS (SIZEOF_LONG_LONG * BITSPERBYTE)

/* Convert a string to a long double */
#ifndef HAVE_STRTOLD
#if SIZEOF_DOUBLE == SIZEOF_LONG_DOUBLE
#define HAVE_STRTOLD
#define strtold strtod
#else
long double strtold(const char *str, char **endptr); /* in utils.c */
#endif
#endif



/* Miscellaneous config */


#ifndef BOOL_H
#include <bool.h>
/* typedef unsigned char bool; */
#endif

#include "cstring.h"

#ifndef NULL
#define NULL ((void *)0)
#endif

#define TRUE 1
#define FALSE 0

#define SUCCESS_EXIT_CODE 0
#define FATAL_EXIT_CODE 33

/* Use node postorder id's rather than nodes themselves in "find def" analysis
   for finding temps (see temps.c) */
#define NODEREFINT

/* Target C dialect is gcc */
#define ASSUME_GCC

/* Define to emit RC code that checks when the old value is null
   (less work, but an extra branch)
   (definitely helps lcc) */
#define NULLRC

/* Call check_child to check for parent/child region relationship rather than
   emitting inline code	*/
/* #undef USE_CHECK_CHILD */

/* Set to 1 to always use rc_adjust_xxx fns, and never create or use
   rc_update_xxx functions. Automatically turns -frc-safe-adjustfn on.
   Set to 0 to use rc_update_xxx */
#define USE_RC_ADJUST_FOR_UPDATE 1

#endif
