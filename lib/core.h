
////////////////////////////////////////////////////////////////////////////
// Cyclone library, file core.h                                           //
// Copyright Greg Morrisett, Dan Grossman                                 //
// January 2001, all rights reserved                                      //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

#ifndef _CORE_H_
#define _CORE_H_

// The routines declared in precore.h are actually written in C,
// but clients don't need to know that.  They are defined in
// cyc_runtime.c

#include "precore.h"

// The rest is defined in Cyclone, in core.cyc
namespace Core {
extern string get_env(string);
extern string ?std_args();
struct Opt<`a> { `a v; };
typedef struct Opt<`a> *opt_t<`a>;
extern opt_t<`b> opt_map(`b f(`a), opt_t<`a> x);
extern string new_string(int);
extern bool true_f(`a);
extern bool false_f(`a);
extern `a fst($(`a,`b)@);
extern `b snd($(`a,`b)@);
extern `c third($(`a,`b,`c)@);
extern `a identity(`a);
extern int intcmp(int,int);
extern int charcmp(char,char);
extern int ptrcmp(`a::A @ `r, `a::A @ `r); 
extern xenum exn { InvalidArg(string) };
extern xenum exn { Failure(string) };
extern xenum exn { Impossible(string) };
extern xenum exn { Not_found };
extern xenum exn { Unreachable(string) };
extern bool is_space(char);
extern int    int_of_string(string);
extern string string_of_int(int);
extern string string_of_uint(unsigned int);
extern string string_of_char(char);
// copies the string, making sure there's a zero at the end
extern "C" Cstring string_to_Cstring(string);
// extracts the underlying char[] from the char[?] -- returns null
// when the string is empty
extern "C" Cstring underlying_Cstring(string);
extern "C" string Cstring_to_string(Cstring);
extern "C" int system(Cstring);
}

#endif
