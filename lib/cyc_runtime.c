// This is the C "runtime library" to be used with the output of the
// Cyclone to C translator

// FIX: for now we use malloc instead of GC_malloc

#include <stdio.h>
#include <stdarg.h>
#include "cyc_include.h"
#include <errno.h>

//////////////////////////////////////////////////////////
// First, definitions for things declared in cyc_include.h
//////////////////////////////////////////////////////////

char _Null_Exception_tag[15] = "Null_Exception";
struct _xenum_struct _Null_Exception_struct = { _Null_Exception_tag };
exn Null_Exception = &_Null_Exception_struct;
char _Match_Exception_tag[16] = "Match_Exception";
struct _xenum_struct _Match_Exception_struct = { _Match_Exception_tag };
exn Match_Exception = &_Match_Exception_struct;

struct _tagged_string *xprintf(char *fmt, ...) {
  va_list argp;
  int len1;
  int len2;
  struct _tagged_string *result;

  va_start(argp,fmt);
  len1 = vsnprintf(NULL,0,fmt,argp); // how much space do we need
  va_end(argp);

  // Presumably the Cyclone typechecker rules this out, but check anyway
  if (len1 < 0) {
    fprintf(stderr,"internal error: encoding error in xprintf\n");
    exit(1);
  }

  // Careful: we need space for a trailing zero (???)
  result = (struct _tagged_string *)GC_malloc(sizeof(struct _tagged_string));
  result->sz       = len1+1;
  result->contents = (char *)GC_malloc(len1+1);
  result->contents[len1] = '\0';

  va_start(argp,fmt);
  len2 = vsnprintf(result->contents,len1+1,fmt,argp);
  va_end(argp);

  if (len1 != len2) {
    fprintf(stderr, "internal error: encoding error in xprintf\n");
    exit(1);
  }
  return result;
}

// Exceptions

static struct _handler_cons *_handler_stack = NULL;

// create a new handler, put it on the stack, and return it so its
// jmp_buf can be filled in by the caller
struct _handler_cons *_push_handler() {
  struct _handler_cons *h =
    (struct _handler_cons *)GC_malloc(sizeof(struct _handler_cons));
  if (h == NULL) {
    fprintf(stderr,"internal error: out of memory in _push_handler\n");
    exit(1);
  }
  h->tail = _handler_stack;
  _handler_stack = h;
  return h;
}

// Pop n handlers off the stack, then one more, returning it
// Invariant: result is non-null
struct _handler_cons *_npop_handler(int n) {
  struct _handler_cons *result;
  if (n<0) {
    fprintf(stderr,
            "internal error: _npop_handler called with negative argument\n");
    exit(1);
  }
  for (;n>=0;n--) {
    if (_handler_stack == NULL) {
      fprintf(stderr,"internal error: empty handler stack\n");
      exit(1);
    }
    result = _handler_stack;
    _handler_stack = (struct _handler_cons*)_handler_stack->tail;
  }
  return result;
}

static void _init_handler_stack() {
  exn e;
  struct _handler_cons *h =
    (struct _handler_cons *)GC_malloc(sizeof(struct _handler_cons));
  if (h == NULL) {
    fprintf(stderr,"internal error: out of memory in _init_handler_stack\n");
    exit(1);
  }
  h->tail = NULL;
  _handler_stack = h;
  e = (exn)setjmp(h->handler);
  if (e) {
    fprintf(stderr,"Uncaught exception %s\n",e->tag);
    exit(1);
  }
}

void _pop_handler() {
  _npop_handler(0);
}
void throw(exn e) {
  struct _handler_cons *h;
  // fprintf(stderr,"throwing exception %s\n",e->tag); fflush(stderr);
  h = _npop_handler(0);
  longjmp(h->handler,(int)e);
}
void _throw(exn e) {
  throw(e);
}

/*

PROBLEM: the strategy below does not work.  _trycatch is supposed to
return NULL the first time it returns, and, if it returns a second
time, it should return a non-NULL exn that was thrown.  In fact, this
is what happens.  HOWEVER, because of the way setjmp is implemented,
the second non-NULL return value does not make it into e.  The only
way I've found to get around this is to essentially inline the
_trycatch, but, this means exposing setjmp everywhere, so it's not
great.

exn _trycatch() {
  struct _handler_cons *h = _push_handler();
  return (exn)setjmp(h->handler);
}

// Now try works as follows:
exn e = _trycatch();
if (!e) {
  // try body
  _pop_handler();
} else {
  // handlers -- switch on e
} */


////////////////////////////////////////////////////////////////
// The rest of the code is stuff declared in core.h or otherwise
// used in core.c
// It's taken from talc/runtime/stdlib.c
////////////////////////////////////////////////////////////////

// The C include file precore_c.h is produced (semi) automatically
// from the Cyclone include file precore.h.
#include "precore_c.h"

struct _tagged_string *Cstring_to_string(Cstring s) {
  struct _tagged_string *str;
  int sz=(s?strlen(s):0);
  str = (struct _tagged_string *)GC_malloc(sizeof(struct _tagged_string));
  str->sz       = sz;
  str->contents = (char *)GC_malloc(sz);

  while(--sz>=0)
    // Copy the string in case the C code frees it or mangles it
    str->contents[sz]=s[sz];

  return str;
}

Cstring string_to_Cstring(string s) {
  int i;
  char *contents;
  char *str=(char *)GC_malloc(s->sz+1);
  if (str == NULL) {
    fprintf(stderr,"internal error: out of memory in string_to_Cstring\n");
    exit(1);
  }
  contents = s->contents;

  for(i=0; i<s->sz; i++) str[i]=contents[i];
  str[s->sz]='\0';
  return str;
}

Cstring underlying_Cstring(string s) {
  char *str=s->contents;
  if (s->sz == 0) 
    str = NULL;
  return str;
}

extern int system(Cstring);

static void check_fd(FILE *fd) {
  if(!fd) {
    fprintf(stderr,"Attempt to access null file descriptor.\n");
    exit(255);
  }
}
int Cyc_Stdio_file_string_read(FILE *fd, string dest, int dest_offset, 
			       int max_count) {
  check_fd(fd);
  if(dest_offset + max_count > dest->sz) {
    fprintf(stderr,"Attempt to read off end of string.\n");
    exit(255);
  }
  return fread((dest->contents)+dest_offset, 1, max_count, fd);
}
int Cyc_Stdio_file_string_write(FILE *fd, string src, int src_offset, 
				int max_count) {
  check_fd(fd);
  if(src_offset + max_count > src->sz) {
    fprintf(stderr,"Attempt to write off end of string.\n");
    exit(255);
  } 
  return fwrite((src->contents)+src_offset, 1, max_count, fd);
}
// FIX: Make more C-like.
int f_seek(FILE *fd, int offset) {
  check_fd(fd);
  if(fseek(fd, (long int) offset, SEEK_SET)) {
    fprintf(stderr, "file seek failed, offset %d.\n", offset);
    exit(255);
  }
  return 0;
}
// extern int fflush(FILE *); // supplied by stdio
// extern int fgetc(FILE *);  // supplied by stdio


/////////////////////////////////////
// THIS SECTION ONLY USED IN CORE.CYC

// extern FILE    *fopen            (Cstring,Cstring);  // supplied by stdio
/*
int f_close(FILE *fd) {
  if(!fd) {
    fprintf(stderr,"Attempt to close null file descriptor.\n");
    exit(255);
  }
  return fclose(fd);
}
*/
///////////////////////////////////////////////


#include <stdio.h>

FILE *Cyc_Stdio_stdin = NULL;
FILE *Cyc_Stdio_stdout = NULL;
FILE *Cyc_Stdio_stderr = NULL;

void init_stdlib_io() {
  Cyc_Stdio_stdin = stdin;
  Cyc_Stdio_stdout = stdout;
  Cyc_Stdio_stderr = stderr;
}


// To be used in core.cyc only
int cyc_argc;
static char **cyc_argv;

static int current_argc;
static char **current_argv;

void start_args() {
  current_argc = cyc_argc;
  current_argv = cyc_argv;
}

struct _tagged_string *next_arg() {
  struct _tagged_string *arg;
  if (current_argc <= 0) {
    fprintf(stderr,"next_arg called when there is no next arg\n");
    exit(1);
  }
  arg = Cstring_to_string(*current_argv);
  current_argv++;
  current_argc--;
  return arg;
}

extern int Cyc_main(void);

int main(int argc, char **argv)
{
  _init_handler_stack();
  init_stdlib_io();
  cyc_argc = argc;
  cyc_argv = argv;
  return Cyc_main();
}
