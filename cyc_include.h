// This is a C header file to be used by the output of the Cyclone
// to C translator.  The corresponding definitions are in
// the file cyc_helpers.c

#ifndef _CYC_INCLUDE_H_
#define _CYC_INCLUDE_H_

///////////////////// Strings
struct _tagged_string { char *curr; char *base; char *last_plus_one; };
extern struct _tagged_string xprintf(char *fmt, ...);

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
extern void _push_handler(struct _handler_cons *);
extern void _npop_handler(int);
extern void _pop_handler();
extern void _throw(exn e);

extern void *GC_malloc(int);
extern void *GC_malloc_atomic(int);
extern char *Cyc_new_string(char *);

#endif
