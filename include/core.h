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
typedef char *`r Cstring<`r>;
typedef char @`r CstringNN<`r>;
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
/** [mstringptr_t] is the mutable version of [stringptr_t]. */

typedef char * @nozeroterm`r Cbuffer_t<`r>;
/** [buffer_t] is a non-zero-terminated C buffer */
typedef char @ @nozeroterm`r CbufferNN_t<`r>;
/** [buffer_t] is a non-zero-terminated C buffer */
typedef const char ? @nozeroterm`r buffer_t<`r>;
/** [buffer_t] is a non-zero-terminated dynamically sized buffer */
typedef char ? @nozeroterm`r mbuffer_t<`r>;

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

  /** The [NewRegion] struct is used to return a new dynamic region.  The
      region is allocated as a sub-region of [r2]. */
namespace Core {
struct NewRegion<`r2::R> {
  <`r1::R>
  dynregion_t<`r1,`r2> dynregion;
};
  // Note that this is here because it is used by the runtime
}

#ifndef _CYC_GENERATE_PRECORE_C_
#include <sys/types.h>

/* The rest of these are implemented in Cyclone, in core.cyc. */
namespace Core {
  /** [sizeof_typ<T>] is the singleton type of [sizeof(T)].  */
typedef tag_t<valueof_t(sizeof(`a::A))> sizeof_t<`a>;
extern struct Opt<`a> { `a v; };
  /** A [struct Opt] is a cell with a single field, [v] (for value). */
typedef struct Opt<`a> *`r::TR opt_t<`a,`r>;
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
extern mstring_t<`H> new_string(unsigned int);
  /** [new_string(n)] allocates space for [n] characters on the heap
      and returns a pointer to the space.  All of the characters are
      set to NUL (0). */
extern mstring_t<`r> rnew_string(region_t<`r>,unsigned int);
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
extern int nptrcmp(`a::A * `r, `a::A * `r);
  /** [nptrcmp] is a comparison function on nullable pointers. */
extern datatype exn  { extern Invalid_argument(string_t) };
  /** [Invalid_argument] is an exception thrown by library functions
      when one of their arguments is inappropriate.  */
extern datatype exn  { extern Failure(string_t) };
  /** [Failure] is an exception that's thrown by library functions
      when they encounter an unexpected error. */
extern datatype exn  { extern Impossible(string_t) };
  /** The [Impossible] exception is thrown when a supposedly
      impossible situation occurs (whether in a library or in your own
      code).  For example, you might thrw [Impossible] if an assertion
      fails.  */
extern datatype exn  { extern Not_found };
  /** The [Not_found] exception is thrown by search functions to
      indicate failure.  For example, a function that looks up an
      entry in a table can throw [Not_found] if the entry is not
      found. */
extern datatype exn  { extern Unreachable(string_t) };
extern region_t<`H> heap_region;
  /** [heap_region] is the region handle of the heap. */
extern region_t<`U> unique_region;
  /** [unique_region] is the region handle of the unique pointer region. */
#define unew rnew (Core::unique_region)
#define umalloc rmalloc (Core::unique_region)
  /** [unew] and [umalloc] are for allocating uniquely-pointed-to data. */

extern void ufree(`a::A ?`U ptr);
  /** [ufree] frees a unique pointer. */

  /** A call to [rnew_dynregion(r2)] returns a new dynamic region allocated
      within [r2]. */
extern struct NewRegion<`r2> rnew_dynregion(region_t<`r2>);
  /** A call to [new_dynregion()] returns a new dynamic region allocated
      in the heap. */
extern struct NewRegion new_dynregion();
  /** The [Open_Region] exception is thrown when one attempts to open 
      a dynamic region that is either already open or has been freed. */
extern datatype exn  { extern Open_Region };
  /** The [Free_Region] exception is thrown when one attempts to free
      a dynamic region that is either open or has already been freed. */
extern datatype exn  { extern Free_Region };
  /** A call to [free_dynregion(d)] attempts to free the storage associated
      with the dynamic region handle d.  If d has been opened or d has already
      been freed, then the exception [Free_Region] is thrown. */
extern void free_dynregion(dynregion_t<`r1,`r2>);
  /** A call to [try_free_dynregion(d)] attempts to free the storage associated
      with the dynamic region handle d.  If d has been opened or d has already
      been freed, then it returns 0, and otherwise returns 1 upon success. */
extern bool try_free_dynregion(dynregion_t<`r1,`r2>);

  /** mkfat can be used to convert a thin pointer (@) of elements of type `a
      to a fat pointer (?).  It requires that you pass in the size of the
      element type, as well as the number of elements. */
  extern `a?`r mkfat(`a @{valueof(`n)}`r arr, Core::sizeof_t<`a> s, tag_t<`n> n);


// copies the string, making sure there's a zero at the end
extern "C" Cstring<`H> string_to_Cstring(string_t);
// extracts the underlying char[] from the char[?] -- returns NULL
// when the string is empty
//  extern "C" Cstring<`r> underlying_Cstring(const char ? @nozeroterm`r);
extern "C" mstring_t<`H> Cstring_to_string(Cstring);
extern "C" mbuffer_t<`r> wrap_Cbuffer_as_buffer(Cbuffer_t<`r>, size_t);
//  extern "C" mbuffer_t<`r> wrap_Cstring_as_string(Cbuffer_t<`r>, size_t);
extern "C" mstring_t<`H> ?`H ntCsl_to_ntsl(Cstring @);
extern "C" unsigned int arr_prevsize(`a::A ?,sizeof_t<`a>);
  /** [array_prevsize(p,sz)] returns the buffer space available preceding
      the pointer [p] in the dynamic array [p] points into.  [sz] is the
      size of the elements in the array returned by [sizeof]. */
}
#endif

#endif
