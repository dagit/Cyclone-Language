// This is a C header file to be used by the output of the Cyclone
// to C translator.  The corresponding definitions are in
// the file lib/runtime_cyc.c

#ifndef _CYC_INCLUDE_H_
#define _CYC_INCLUDE_H_

//// Strings
struct _tagged_string { char *curr; char *base; char *last_plus_one; };
extern struct _tagged_string xprintf(char *fmt, ...);

//// Discriminated Unions
struct _xtunion_struct { char *tag; };

// The runtime maintains a stack that contains either _handler_cons
// structs or _RegionHandle structs.  The tag is 0 for a handler_cons
// and 1 for a region handle.  
struct _RuntimeStack {
  int tag; // 0 for an exception handler, 1 for a region handle
  struct _RuntimeStack *next;
};

//// Regions
struct _RegionPage {
  struct _RegionPage *next;
  char data[0];
};

struct _RegionHandle {
  struct _RuntimeStack s;
  struct _RegionPage *curr;
  char               *offset;
  char               *last_plus_one;
};

extern struct _RegionHandle _new_region();
extern void * _region_malloc(struct _RegionHandle *, unsigned int);
extern void   _free_region(struct _RegionHandle *);

//// Exceptions 
#include <setjmp.h>
struct _handler_cons {
  struct _RuntimeStack s;
  jmp_buf handler;
};
extern void _push_handler(struct _handler_cons *);
extern void _push_region(struct _RegionHandle *);
extern void _npop_handler(int);
extern void _pop_handler();
extern void _pop_region();
extern void _throw(void * e);

//// Built-in Exceptions
extern struct _xtunion_struct _Null_Exception_struct;
extern struct _xtunion_struct * Null_Exception;
extern struct _xtunion_struct _Match_Exception_struct;
extern struct _xtunion_struct * Match_Exception;

//// Allocation
extern void * GC_malloc(int);
extern void * GC_malloc_atomic(int);


#endif
