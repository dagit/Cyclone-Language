
  /* OS X needs sys/types.h before dirent.h */
  #include <sys/types.h>
  #include <dirent.h>
  struct Cyc_Std___cycDIR { // must match defn in cyclone stub
    DIR *dir;
  };

  // We do this because errno is probably a C macro
  #include <errno.h>
  int __CYCLONE_ERRNO() {
    return errno;
  }

  #include "precore_c.h"
  #include <fcntl.h>
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

  #include <signal.h>
  typedef void *Cyc_Std_sigarg_t;
  extern Cyc_Std_sigarg_t Cyc_Std_Csignal_make_SIG_DFL();
  extern Cyc_Std_sigarg_t Cyc_Std_Csignal_make_SIG_ERR();
  extern Cyc_Std_sigarg_t Cyc_Std_Csignal_make_SIG_IGN();
  extern Cyc_Std_sigarg_t Cyc_Std_Csignal_make_SIG_HOLD();
  extern Cyc_Std_sigarg_t Cyc_Std_Csignal_make_SIG_HANDLER(void (*)(int));
  Cyc_Std_sigarg_t sig_wrap(void (*f)(int)) {
    if (f == SIG_DFL)
      return Cyc_Std_Csignal_make_SIG_DFL();
    if (f == SIG_ERR)
      return Cyc_Std_Csignal_make_SIG_ERR();
    if (f == SIG_IGN)
      return Cyc_Std_Csignal_make_SIG_IGN();
  #ifdef SIG_HOLD
    if (f == SIG_HOLD)
      return Cyc_Std_Csignal_make_SIG_HOLD();
  #endif
      return Cyc_Std_Csignal_make_SIG_HANDLER(f);
  }

  #ifndef __CYGWIN__
  Cyc_Std_sigarg_t signal_SIG_DFL(int a) {
    return sig_wrap(signal(a,SIG_DFL));
  }
  Cyc_Std_sigarg_t signal_SIG_ERR(int a) {
    return sig_wrap(signal(a,SIG_ERR));
  }
  #ifdef _SIG_HOLD_def_
  Cyc_Std_sigarg_t signal_SIG_HOLD(int a) {
    return sig_wrap(signal(a,SIG_HOLD));
  }
  #endif
  Cyc_Std_sigarg_t signal_SIG_IGN(int a) {
    return sig_wrap(signal(a,SIG_IGN));
  }
  #endif

  #include <stdio.h>
  // Define struct Std::__cycFILE, and initialize stdin, stdout, stderr
  struct Cyc_Std___cycFILE { // must match defn in cyclone stub
    FILE *file;
  } Cyc_Std_stdin_v, Cyc_Std_stdout_v, Cyc_Std_stderr_v,
    *Cyc_Std_stdin = &Cyc_Std_stdin_v,
    *Cyc_Std_stdout = &Cyc_Std_stdout_v,
    *Cyc_Std_stderr = &Cyc_Std_stderr_v;

  FILE *_sfile_to_file(struct Cyc_Std___cycFILE *sf) {
    if(!sf) {
      fprintf(stderr,"Attempt to access null file descriptor.\n");
      exit(255);
    }
    if(!sf->file)
      throw(Cyc_Null_Exception); // FIX:  should be more descriptive?
    return sf->file;
  }
  int Cyc_Std_file_string_read(struct Cyc_Std___cycFILE *sf, 
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
  int Cyc_Std_file_string_write(struct Cyc_Std___cycFILE *sf, 
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

  #include <sys/select.h>
  void __stub_FD_CLR(int a, fd_set *b) {
    FD_CLR(a,b);
  }
  int __stub_FD_ISSET(int a, fd_set *b) {
    return FD_ISSET(a,b);
  }
  void __stub_FD_SET(int a, fd_set *b) {
    FD_SET(a,b);
  }
  void __stub_FD_ZERO(fd_set *a) {
    FD_ZERO(a);
  }

  #include <sys/socket.h>
  #include <netinet/in.h>

  #ifdef __APPLE__
  typedef int socklen_t;
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

  /* sys/time.h is needed on some systems (OS X) for struct timeval */
  #include <sys/time.h>
  int getsockopt_int(int a, int b, int c, int *d, socklen_t *e) {
    return getsockopt(a,b,c,d,e);
  }
  int getsockopt_timeval(int a, int b, int c, struct timeval *d, socklen_t *e) {
    return getsockopt(a,b,c,d,e);
  }

  ssize_t recv_wrapped(int a, struct _tagged_arr b, size_t c, int d) {
    char *p = _check_unknown_subscript(b,1,0);
    unsigned n = _get_arr_size(b,1);  
    if (n<c) _throw_arraybounds();
    return recv(a,p,c,d);
  }

  ssize_t recvfrom_in(int a, struct _tagged_arr b, size_t c, int d,
                      const struct sockaddr_in *e, socklen_t *f) {
    char *p = _check_unknown_subscript(b,1,0);
    unsigned n = _get_arr_size(b,1);  
    if (n<c) _throw_arraybounds();
    return recvfrom(a,p,c,d,(struct sockaddr *)e,f);
  }

  ssize_t send_wrapped(int a, struct _tagged_arr b, size_t c, int d) {
    char *p = _check_unknown_subscript(b,1,0);
    unsigned n = _get_arr_size(b,1);  
    if (n<c) _throw_arraybounds();
    return send(a,p,c,d);
  }

  ssize_t sendto_in(int a, struct _tagged_arr b, size_t c, int d,
                    const struct sockaddr_in *e, socklen_t f) {
    char *p = _check_unknown_subscript(b,1,0);
    unsigned n = _get_arr_size(b,1);  
    if (n<c) _throw_arraybounds();
    return sendto(a,p,c,d,(struct sockaddr *)e,f);
  }

  int setsockopt_int(int a, int b, int c, const int *d, socklen_t e) {
    return setsockopt(a,b,c,d,e);
  }
  int setsockopt_timeval(int a, int b, int c, const struct timeval *d, socklen_t e) {
    return setsockopt(a,b,c,d,e);
  }
