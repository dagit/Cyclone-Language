// This is a C header file to be used by the output of the Cyclone
// to C translator.  The corresponding definitions are in
// the file cyc_helpers.c

#ifndef CYC_INCLUDE_H
#define CYC_INCLUDE_H

///////////////////// Basic types
typedef int Int;
typedef int Long;
typedef int Short;
typedef int Float;
typedef struct _boxed_double_struct { double v; } *Double;
typedef struct _boxed_long_long_struct { long long v; } *_LongLong;
typedef int Char;

///////////////////// Strings
struct _tagged_string { unsigned int sz; char *contents; };
extern struct _tagged_string *xprintf(char *fmt, ...);
//extern struct _tagged_string *new_string(unsigned int sz);

///////////////////// Exceptions
struct _enum_struct { int tag; };
struct _xenum_struct { char *tag; };
typedef struct _xenum_struct *exn;
extern char _Null_Exception_tag[15];
extern struct _xenum_struct _Null_Exception_struct;
extern exn Null_Exception;
extern char _Match_Exception_tag[16];
extern struct _xenum_struct _Match_Exception_struct;
extern exn Match_Exception;

#include <setjmp.h>
struct _handler_cons {
  jmp_buf handler;
  struct _handler_cons *tail;
};
extern struct _handler_cons *_push_handler();
extern struct _handler_cons *_npop_handler(int);
extern void _pop_handler();
extern void _throw(exn e);
//extern exn _trycatch();

extern void *GC_malloc(int);

#endif
