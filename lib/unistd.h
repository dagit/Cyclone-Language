#ifndef _UNISTD_H
#define _UNISTD_H

#include <core.h>
#include <sys/types.h>
#include <getopt.h> // Since ordinary getopt() belongs in unistd.h

namespace Unistd {

#ifndef SEEK_SET
#define SEEK_SET        0       // set file offset to offset 
#endif
#ifndef SEEK_CUR
#define SEEK_CUR        1       // set file offset to current plus offset 
#endif
#ifndef SEEK_END
#define SEEK_END        2       // set file offset to EOF plus offset 
#endif

  extern "C" {
    int close(int);
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
    int select(int n, fd_set *readfds, fd_set *writefds,
               fd_set *exceptfds, struct timeval *timeout);
    int pipe(int filedes[2]);
    off_t lseek(int filedes, off_t offset, int whence);
  }

  int chdir(string_t);
  char ?getcwd(char ?buf, size_t size);
  int execl(string_t path, string_t arg0, ...`r string_t argv);
  int execve(string_t filename, string_t ?argv, string_t ?envp);
  ssize_t read(int fd, mstring_t buf, size_t count);
  ssize_t write(int fd, string_t buf, size_t count);
  int unlink(string_t pathname);
}

#endif
