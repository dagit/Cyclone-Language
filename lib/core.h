
////////////////////////////////////////////////////////////////////////////
// Popcorn library, file core.h                                           //
// Copyright Greg Morrisett, Dan Grossman                                 //
// January 1999, all rights reserved                                      //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

#ifndef CORE_H
#define CORE_H

// The routines declared in precore.h are actually written in C,
// but clients don't need to know that.  They are defined in
// cyc_runtime.c

#include "precore.h"

// The rest is defined in Cyclone, in core.cyc
namespace Core {

extern xenum exn { FileOpenError(string) };
extern xenum exn { FileCloseError };
//extern FILE @file_using(string,string);
extern FILE @file_open(string fname, string mode);
extern void file_close(FILE @);
extern int  file_delete(string);
extern int  file_length(string);
extern string get_env(string);

extern string std_args()[?];

#define EOF ((char)(-1))

struct Opt<`a> { `a v; };
typedef struct Opt<`a> *Opt_t<`a>;

extern Opt_t<`b> opt_map<`a,`b>(`b f(`a), Opt_t<`a> x);

extern string new_string(int);

extern bool true_f<`a>(`a);
extern bool false_f<`a>(`a);

extern `a fst<`a,`b>($(`a,`b)@);
extern `b snd<`a,`b>($(`a,`b)@);
extern `c third<`a,`b,`c>($(`a,`b,`c)@);

extern `a identity<`a>(`a);

extern int intcmp(int,int);
extern int charcmp(char,char);
// fairly useless until we can give `a a memory kind
extern int ptrcmp<`a>(`a @, `a @); 

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

extern Char box_char(char);
extern Short box_short(short);
extern Long Long box_long_long(long long);
extern Float box_float(float);
extern Double box_double(double);

extern char unbox_char(Char);
extern short unbox_short(Short);
extern long long unbox_long_long(Long Long);
extern float unbox_float(Float);
extern double unbox_double(Double);

typedef unsigned int size_t;

}

#endif
