
  #ifdef __APPLE__
  #include <sys/types.h>
  #include <netinet/in.h>
  #include <stdint.h>
  #endif

  #include <setjmp.h> // Needed by precore_c.h
  #include "precore_c.h"

  #include <stdio.h>
  // Define struct __cycFILE, and initialize stdin, stdout, stderr
  struct Cyc___cycFILE { // must match defn in cyclone stub
    FILE *file;
  } Cyc_stdin_v, Cyc_stdout_v, Cyc_stderr_v,
    *Cyc_stdin = &Cyc_stdin_v,
    *Cyc_stdout = &Cyc_stdout_v,
    *Cyc_stderr = &Cyc_stderr_v;

  FILE *_sfile_to_file(struct Cyc___cycFILE *sf) {
    if(!sf) {
      fprintf(stderr,"Attempt to access null file descriptor.\n");
      exit(255);
    }
    if(!sf->file)
      throw(Cyc_Null_Exception); // FIX:  should be more descriptive?
    return sf->file;
  }
  int Cyc_file_string_read(struct Cyc___cycFILE *sf, 
                                 struct _dyneither_ptr dest,
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
  int Cyc_file_string_write(struct Cyc___cycFILE *sf, 
                                  struct _dyneither_ptr src,
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

/* This stuff needed by buildlib only.  Try to get rid of it. */
  #include <fcntl.h>
  int open_with_mode(const char * s, int flags, mode_t m) {
    return open(s,flags,m);
  }
  int open_without_mode(const char * s, int flags) {
    /* Use 0 as the mode argument, in case flags contains O_CREAT.  In
       that case the file will be created with 0 permissions; if we
       omitted the 0 it would be created with random permissions. */
    return open(s,flags,0);
  }
