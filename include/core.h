/* This file is part of the Cyclone Library.
   Copyright (C) 2001 Greg Morrisett, Dan Grossman, AT&T

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

#ifndef _CORE_H_
#define _CORE_H_

/*** \subsection{\texttt{<core.h>}} */
/*** The file \texttt{<core.h>} defines some types and functions
     outside of any namespace, and also defines a namespace Core. */

/* These definitions are made outside of any namespace.  Most of them
   are implemented in C code (cyc_runtime.c).  To make sure that the
   interface is correct, we generate a C .h file from these
   definitions, for use in the C code.  See the Makefile to find out
   how. */

/*** These declarations are made outside of any namespace. */
typedef char *{0}`r Cstring<`r>;
typedef const char ?`r string_t<`r>;
/** A [string_t<`r>] is a constant array of characters allocated in
    region [`r].  */
typedef char ?`r mstring_t<`r>;
/** An [mstring_t<`r>] is a non-const (mutable) array of characters
    allocated in region [`r].  */
typedef string_t<`r1> @`r2 stringptr_t<`r1,`r2>;
/** A [stringptr_t<`r1,`r2>] is used when a ``boxed'' string is
    needed, for example, you can have a list of string pointers, but not
    a list of strings.  */
typedef mstring_t<`r1> @`r2 mstringptr_t<`r1,`r2>;
/** [mstringptr] is the mutable version of [stringptr_t]. */

#ifndef bool
typedef int bool;
/** In Cyclone, we use [bool] as a synonym for [int].  We also define
    macros [true] and [false], which are 1 and 0, respectively.  */
#endif
#ifndef false
#define false (0)
#endif
#ifndef true
#define true (1)
#endif
extern "C" void exit(int) __attribute__((noreturn)) ;
extern "C" `a abort() __attribute__((noreturn));

#ifndef _CYC_GENERATE_PRECORE_C_
#include <sys/types.h>

/* The rest of these are implemented in Cyclone, in core.cyc. */
namespace Core {
extern struct Opt<`a> { `a v; };
  /** A [struct Opt] is a cell with a single field, [v] (for value). */
typedef struct Opt<`a> *`r opt_t<`a,`r>;
  /** An [opt_t] is a pointer to a [struct Opt].  An [opt_t] can be
      used to pass an optional value to a function, or return an
      optional result.  For example, to return no result, return NULL;
      to return a result [x], return [new Opt(x)].

      Another way to return an option result of type [t] would be to
      return a pointer to [t].  The [opt_t] type is useful primarily
      when porting Objective Caml code, which has a corresponding
      type. */
extern opt_t<`b,`H> opt_map(`b f(`a), opt_t<`a,`r> x);
  /** [opt_map(f,x)] applies [f] to the value contained in option [x],
      if any, and returns the result as an option; if [x] is NULL,
      [opt_map(f,x)] returns NULL. */
extern mstring_t<`H> new_string(int);
  /** [new_string(n)] allocates space for [n] characters on the heap
      and returns a pointer to the space.  All of the characters are
      set to NUL (0). */
extern mstring_t<`r> rnew_string(region_t<`r>,int);
  /** [rnew_string(r,n)] allocates space for [n] characters in the
      region with handle [r], and returns a pointer to the space.  All
      of the characters are set to NUL (0). */
extern bool true_f(`a);
  /** [true_f] is the constant [true] function: [true_f(x)] returns
      [true] regardless of the value of [x]. */
extern bool false_f(`a);
  /** [false_f] is the constant [false] function. */
extern `a fst($(`a,`b)@`r);
  /** [fst(x)] returns the first element of the pair pointed to by
      [x].  */
extern `b snd($(`a,`b)@);
  /** [snd(x)] returns the second element of the pair pointed to by
      [x].  */
extern `c third($(`a,`b,`c)@);
  /** [third(x)] returns the third element of the triple pointed to by
      [x].  */
extern `a identity(`a);
  /** [identity] is the identity function: [identity(x)] returns [x].  */
extern int intcmp(int,int);
  /** [intcmp] is a comparison function on integers: [intcmp(i1,i2)]
      returns a number less than, equal to, or greater than 0
      according to whether [i1] is less than, equal to, or greater
      than [i2].  */
extern int charcmp(char,char);
  /** [charcmp] is a comparison function on [char]. */
extern int ptrcmp(`a::A @ `r, `a::A @ `r);
  /** [ptrcmp] is a comparison function on pointers. */
extern xtunion exn { extern Invalid_argument(string_t) };
  /** [Invalid_argument] is an exception thrown by library functions
      when one of their arguments is inappropriate.  */
extern xtunion exn { extern Failure(string_t) };
  /** [Failure] is an exception that's thrown by library functions
      when they encounter an unexpected error. */
extern xtunion exn { extern Impossible(string_t) };
  /** The [Impossible] exception is thrown when a supposedly
      impossible situation occurs (whether in a library or in your own
      code).  For example, you might thrw [Impossible] if an assertion
      fails.  */
extern xtunion exn { extern Not_found };
  /** The [Not_found] exception is thrown by search functions to
      indicate failure.  For example, a function that looks up an
      entry in a table can throw [Not_found] if the entry is not
      found. */
extern xtunion exn { extern Unreachable(string_t) };
extern region_t<`H> heap_region;
  /** [heap_region] is the region handle of the heap. */
// copies the string, making sure there's a zero at the end
extern "C" Cstring<`H> string_to_Cstring(string_t);
// extracts the underlying char[] from the char[?] -- returns NULL
// when the string is empty
extern "C" Cstring<`r> underlying_Cstring(string_t<`r>);
extern "C" mstring_t<`H> Cstring_to_string(Cstring);
extern "C" mstring_t<`H> wrap_Cstring_as_string(Cstring, size_t);
extern "C" mstring_t<`H> ?`H ntCsl_to_ntsl(Cstring @);
}
#endif

#endif
