#ifndef _UNISTD_H
#define _UNISTD_H

#include <core.h>
#include <sys/types.h>
#include <getopt.h> // Since ordinary getopt() belongs in unistd.h

namespace Unistd {
  extern "C" {
    int close(int);
    ssize_t write(int, const char @, size_t);
    pid_t getpid(void);
    pid_t getppid(void);
    pid_t fork(void);
    int fchdir(int);
    int dup(int);
    int dup2(int, int);
    uid_t getuid(void);
    uid_t geteuid(void);
    gid_t getgid(void);
    gid_t getegid(void);
  }

  int chdir(string_t);
  char ?getcwd(char ?buf, size_t size);
}

#endif
