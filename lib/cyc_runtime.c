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

struct _tagged_string xprintf(char *fmt, ...) {
  va_list argp;
  int len1;
  int len2;
  struct _tagged_string result;

  va_start(argp,fmt);
  len1 = vsnprintf(NULL,0,fmt,argp); // how much space do we need
  va_end(argp);

  // Presumably the Cyclone typechecker rules this out, but check anyway
  if (len1 < 0) {
    fprintf(stderr,"internal error: encoding error in xprintf\n");
    exit(1);
  }

  // Careful: we need space for a trailing zero (???)
  result.base     = (char *)GC_malloc_atomic(len1+1);
  result.base[len1] = '\0';
  result.curr     = result.base;
  result.last_plus_one = result.base + (len1+1);

  va_start(argp,fmt);
  len2 = vsnprintf(result.curr,len1+1,fmt,argp);
  va_end(argp);

  if (len1 != len2) {
    fprintf(stderr, "internal error: encoding error in xprintf\n");
    exit(1);
  }
  return result;
}

// Exceptions

// Need one of these per thread (we don't have threads)
static struct _handler_cons *_current_handler = NULL;

// create a new handler, put it on the stack, and return it so its
// jmp_buf can be filled in by the caller
void _push_handler(struct _handler_cons * new_handler) {
  new_handler->tail = _current_handler;
  _current_handler  = new_handler;
}

// set _current_handler to it's n+1'th tail
// Invariant: result is non-null
void _npop_handler(int n) {
  if (n<0) {
    fprintf(stderr,
            "internal error: _npop_handler called with negative argument\n");
    exit(1);
  }
  for(; n>=0; n--) {
    if(_current_handler == NULL) {
      fprintf(stderr,"internal error: empty handler stack\n");
      exit(1);
    } 
    _current_handler = _current_handler->tail;
  }
}

void _pop_handler() {
  _npop_handler(0);
}
void throw(exn e) {
  struct _handler_cons * my_handler = _current_handler;
  _npop_handler(0);
  longjmp(my_handler->handler,(int)e);
}
void _throw(exn e) {
  throw(e);
}

////////////////////////////////////////////////////////////////
// The rest of the code is stuff declared in core.h or otherwise
// used in core.c
// It's taken from talc/runtime/stdlib.c
////////////////////////////////////////////////////////////////

// The C include file precore_c.h is produced (semi) automatically
// from the Cyclone include file precore.h.
#include "precore_c.h"

struct _tagged_string Cstring_to_string(Cstring s) {
  struct _tagged_string str;
  int sz=(s?strlen(s):0);
  str.base = (char *)GC_malloc_atomic(sz);
  str.curr = str.base;
  str.last_plus_one = str.base + sz;

  while(--sz>=0)
    // Copy the string in case the C code frees it or mangles it
    str.curr[sz]=s[sz];

  return str;
}

char *Cyc_new_string(char *s) {
  char *n = GC_malloc_atomic(strlen(s));
  char *t = n;
  while (*s) {
    *t = *s;
    t++;
    s++;
  };
  return n;
}

Cstring string_to_Cstring(string s) {
  int i;
  char *contents = s.curr;
  size_t sz = s.last_plus_one - s.curr;
  char *str;

  if (s.curr < s.base || s.curr >= s.last_plus_one)
    throw(Null_Exception);
  str = (char *)GC_malloc_atomic(sz+1);
  if (str == NULL) {
    fprintf(stderr,"internal error: out of memory in string_to_Cstring\n");
    exit(1);
  }

  for(i=0; i < sz; i++) str[i]=contents[i];
  str[sz]='\0';
  return str;
}

// FIX:  this isn't really needed since you can cast char[?] to char[]
Cstring underlying_Cstring(string s) {
  char *str=s.curr;
  // FIX:  should throw exception?  
  if (s.curr < s.base || s.curr >= s.last_plus_one)
    throw(Null_Exception);
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
  char *new_curr = dest.curr + dest_offset;
  size_t sz = dest.last_plus_one - new_curr;
  if (new_curr < dest.base || new_curr >= dest.last_plus_one)
    throw(Null_Exception);
  check_fd(fd);
  if(dest_offset + max_count > sz) {
    fprintf(stderr,"Attempt to read off end of string.\n");
    exit(255);
  }
  return fread(new_curr, 1, max_count, fd);
}
int Cyc_Stdio_file_string_write(FILE *fd, string src, int src_offset, 
				int max_count) {
  size_t sz = src.last_plus_one - src.curr;
  char *new_curr = src.curr + src_offset;
  if (new_curr < src.base || new_curr >= src.last_plus_one)
    throw(Null_Exception);
  check_fd(fd);
  if(src_offset + max_count > sz) {
    fprintf(stderr,"Attempt to write off end of string.\n");
    exit(255);
  } 
  return fwrite(new_curr, 1, max_count, fd);
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

struct _tagged_string next_arg() {
  struct _tagged_string arg;
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
  struct _handler_cons top_handler;
  int status = setjmp(top_handler.handler);
  _push_handler(&top_handler);
  if(status) {
    fprintf(stderr,"Uncaught exception %s\n",((exn)status)->tag);
    return 1;
  }
  init_stdlib_io();
  cyc_argc = argc;
  cyc_argv = argv;
  return Cyc_main();
}
