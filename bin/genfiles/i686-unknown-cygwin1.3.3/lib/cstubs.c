/* This file is part of the Cyclone Library.
   Copyright (C) 2001 Greg Morrisett, AT&T

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

#include <stdio.h>
#include <string.h> // for memcpy
#include <stdarg.h>
#include <signal.h>

// The C include file precore_c.h is produced (semi) automatically
// from the Cyclone include file precore.h.  Note, it now includes
// the contents of cyc_include.h

/* RUNTIME_CYC defined to prevent including parts of precore_c.h 
   that might cause problems, particularly relating to region profiling */
#define RUNTIME_CYC
#include "precore_c.h"

extern void exit(int);

struct Cyc_Std___sFILE {
  FILE *file; // Mirror any changes in stdio.cyc
} Cyc_Std_stdin_v, Cyc_Std_stdout_v, Cyc_Std_stderr_v,
  *Cyc_Std_stdin = &Cyc_Std_stdin_v,
  *Cyc_Std_stdout = &Cyc_Std_stdout_v,
  *Cyc_Std_stderr = &Cyc_Std_stderr_v;

FILE *_sfile_to_file(struct Cyc_Std___sFILE *sf) {
  if(!sf) {
    fprintf(stderr,"Attempt to access null file descriptor.\n");
    exit(255);
  }
  if(!sf->file)
    throw(Cyc_Null_Exception); // FIX:  should be more descriptive?
  return sf->file;
}
FILE *sfile_to_file(struct Cyc_Std___sFILE *sf) {
  if(!sf) {
    fprintf(stderr,"Attempt to access null file descriptor.\n");
    exit(255);
  }
  if(!sf->file)
    throw(Cyc_Null_Exception); // FIX:  should be more descriptive?
  return sf->file;
}

int Cyc_Std_file_string_read(struct Cyc_Std___sFILE *sf, 
                               struct _tagged_arr dest,
                               int dest_offset, int max_count) {
  unsigned char *new_curr = dest.curr + dest_offset;
  size_t sz = dest.last_plus_one - new_curr;
  FILE *fd = _sfile_to_file(sf);
  if (new_curr < dest.base || new_curr >= dest.last_plus_one)
    throw(Cyc_Null_Exception);
  if(dest_offset + max_count > sz) {
    fprintf(stderr,"Attempt to read off end of string.\n");
    exit(255);
  }
  return fread(new_curr, 1, max_count, fd);
}
int Cyc_Std_file_string_write(struct Cyc_Std___sFILE *sf, 
                                struct _tagged_arr src,
                                int src_offset, int max_count) {
  size_t sz = src.last_plus_one - src.curr;
  unsigned char *new_curr = src.curr + src_offset;
  FILE *fd = _sfile_to_file(sf);
  if (new_curr < src.base || new_curr >= src.last_plus_one)
    throw(Cyc_Null_Exception);
  if(src_offset + max_count > sz) {
    fprintf(stderr,"Attempt to write off end of string.\n");
    exit(255);
  } 
  return fwrite(new_curr, 1, max_count, fd);
}

///////////////////////////////////////////////
// Sockets

// This really doesn't belong here; we should change the makefiles so
// C code can live with the libraries and be linked only as necessary.

// This function takes a const xtunion Socket::sockaddr and returns a
// pointer to the body of the union.  We'd return the size of the body
// if we could, but that information isn't available at runtime.

#include <sys/types.h>
/*
#include <sys/socket.h>
struct sa_xtunion {
  unsigned char *tag;
  struct sockaddr sa;
};
struct sockaddr *sockaddr_to_Csockaddr(struct sa_xtunion *xtunionaddr) {
  return &xtunionaddr->sa;
}
extern unsigned char *Cyc_Std_sockaddr_in;

// We handle Unix domain sockets properly here: the sa field is an
// argument, since it could be bigger (or smaller) than 16
struct sa_xtunion *Csockaddr_to_sockaddr(struct sockaddr *addr, int len) {
  // I hope structure alignment doesn't cause any problem here
  struct sa_xtunion *result = GC_malloc_atomic(sizeof(unsigned char*)+len);
  if (result == NULL) 
    _throw_badalloc();

  memcpy(&result->sa, addr, len);
  switch (addr->sa_family) {
  case AF_INET: result->tag = Cyc_Std_sockaddr_in; break;
  default:
    fprintf(stderr, "internal error: Csockaddr_to_sockaddr with unsupported socket type\n");
    exit(1);
  }
  return result;
}
*/

/* For sockets.  This should be split out to a separate file. */
#include <sys/socket.h>
#include <netinet/in.h>

#ifdef __APPLE__
typedef int socklen_t;
#include <sys/time.h> // where struct timeval is defined
#endif

int accept_in(int a, const struct sockaddr_in *b, socklen_t *c) {
  return accept(a,(struct sockaddr *)b,c);
}

int bind_in(int a, const struct sockaddr_in *b, socklen_t c) {
  return bind(a,(struct sockaddr *)b,c);
}

int connect_in(int a, const struct sockaddr_in *b, socklen_t c) {
  return connect(a,(struct sockaddr *)b,c);
}

int getpeername_in(int a, const struct sockaddr_in *b, socklen_t *c) {
  return getpeername(a,(struct sockaddr *)b,c);
}

int getsockname_in(int a, const struct sockaddr_in *b, socklen_t *c) {
  return getsockname(a,(struct sockaddr *)b,c);
}

ssize_t recvfrom_in(int a, struct _tagged_arr b, size_t c, int d,
                    const struct sockaddr_in *e, socklen_t *f) {
  char *p = _check_unknown_subscript(b,1,0);
  unsigned n = _get_arr_size(b,1);  
  if (n<c) _throw_arraybounds();
  return recvfrom(a,p,c,d,(struct sockaddr *)e,f);
}

ssize_t sendto_in(int a, struct _tagged_arr b, size_t c, int d,
                  const struct sockaddr_in *e, socklen_t f) {
  char *p = _check_unknown_subscript(b,1,0);
  unsigned n = _get_arr_size(b,1);  
  if (n<c) _throw_arraybounds();
  return sendto(a,p,c,d,(struct sockaddr *)e,f);
}

ssize_t send_wrapped(int a, struct _tagged_arr b, size_t c, int d) {
  char *p = _check_unknown_subscript(b,1,0);
  unsigned n = _get_arr_size(b,1);  
  if (n<c) _throw_arraybounds();
  return send(a,p,c,d);
}

ssize_t recv_wrapped(int a, struct _tagged_arr b, size_t c, int d) {
  char *p = _check_unknown_subscript(b,1,0);
  unsigned n = _get_arr_size(b,1);  
  if (n<c) _throw_arraybounds();
  return recv(a,p,c,d);
}

int getsockopt_int(int a, int b, int c, int *d, socklen_t *e) {
  return getsockopt(a,b,c,d,e);
}

int setsockopt_int(int a, int b, int c, const int *d, socklen_t e) {
  return setsockopt(a,b,c,d,e);
}

int getsockopt_timeval(int a, int b, int c, struct timeval *d, socklen_t *e) {
  return getsockopt(a,b,c,d,e);
}

int setsockopt_timeval(int a, int b, int c, const struct timeval *d, socklen_t e) {
  return setsockopt(a,b,c,d,e);
}

///////////////////////////////////////////////
// File locking

// Goes with fcntl.h

int fcntl_with_arg(int fd, int cmd, long arg) {
  return fcntl(fd, cmd, arg);
}
// We call lock a void* so we don't have to #include anything else
int fcntl_with_lock(int fd, int cmd, void *lock) {
  return fcntl(fd, cmd, lock);
}

int open_with_mode(const Cstring s, int flags, mode_t m) {
  return open(s,flags,m);
}

int open_without_mode(const Cstring s, int flags) {
  /* Use 0 as the mode argument, in case flags contains O_CREAT.  In
     that case the file will be created with 0 permissions; if we
     omitted the 0 it would be created with random permissions. */
  return open(s,flags,0);
}

///////////////////////////////////////////////
// Signals (goes with signal.cyc)

extern void Cyc_Std__SIG_DFL(int);
extern void Cyc_Std__SIG_IGN(int);
extern void Cyc_Std__SIG_ERR(int);
typedef void (*signal_t)(int);
extern signal_t signal(int, signal_t);

signal_t signal_func(int sig, signal_t p) {
  if (p == Cyc_Std__SIG_DFL) p = ((void(*)(int)) 0);
  else if (p == Cyc_Std__SIG_IGN) p = ((void(*)(int)) 1);
  else if (p == Cyc_Std__SIG_ERR) p = ((void(*)(int))-1);

  p = signal(sig, p);

  if (p == (void(*)(int)) 0) p = Cyc_Std__SIG_DFL;
  else if (p == (void(*)(int)) 1) p = Cyc_Std__SIG_IGN;
  else if (p == (void(*)(int))-1) p = Cyc_Std__SIG_ERR;
  return p;
}
