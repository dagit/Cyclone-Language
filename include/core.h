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

// The routines declared in precore.h are actually written in C,
// but clients don't need to know that.  They are defined in
// cyc_runtime.c

#include <precore.h>

// The rest is defined in Cyclone, in core.cyc
namespace Core {
extern mstring_t get_env(string_t);
extern mstring_t ?std_args();
struct Opt<`a> { `a v; };
typedef struct Opt<`a> *opt_t<`a>;
extern opt_t<`b> opt_map(`b f(`a), opt_t<`a> x);
extern mstring_t new_string(int);
extern char ?`r rnew_string(region_t<`r>,int);
extern bool true_f(`a);
extern bool false_f(`a);
extern `a fst($(`a,`b)@`r);
extern `b snd($(`a,`b)@`r);
extern `c third($(`a,`b,`c)@`r);
extern `a identity(`a);
extern int intcmp(int,int);
extern int charcmp(char,char);
extern int ptrcmp(`a::A @ `r, `a::A @ `r); 
extern xtunion exn { extern InvalidArg(string_t) };
extern xtunion exn { extern Failure(string_t) };
extern xtunion exn { extern Impossible(string_t) };
extern xtunion exn { extern Not_found };
extern xtunion exn { extern Unreachable(string_t) };
extern bool is_space(char);
extern int    int_of_string(string_t);
extern mstring_t string_of_int(int);
extern mstring_t string_of_uint(unsigned int);
extern mstring_t string_of_char(char);
extern region_t<`H> heap_region;
// copies the string, making sure there's a zero at the end
extern "C" Cstring string_to_Cstring(const char ?`r);
// extracts the underlying char[] from the char[?] -- returns null
// when the string is empty
extern "C" Cstring underlying_Cstring(const char ?`r);
extern "C" mstring_t Cstring_to_string(Cstring);
extern "C" mstring_t wrap_Cstring_as_string(Cstring, size_t);
extern "C" mstring_t ?ntCsl_to_ntsl(Cstring @);
extern "C" int system(Cstring);
extern "C" int @__errno(void);
}

#endif
