#ifndef _FCNTL_H
#define _FCNTL_H

#include <sys/types.h>

#define O_RDONLY       00
#define O_WRONLY       01
#define O_RDWR         02
#define O_CREAT      0100
#define O_EXCL       0200
#define O_NOCTTY     0400
#define O_TRUNC     01000
#define O_APPEND    02000
#define O_NONBLOCK  04000
#define O_SYNC     010000

namespace Fcntl {

  struct flock {
    short l_type;
    off_t l_start;
    short l_whence;
    off_t l_len;
    pid_t l_pid;
  };

  tunion FcntlArg {
    Long(long);
    Flock(struct flock *);
  };

  extern int fcntl(int fd, int cmd, ...`r tunion FcntlArg argv);
}

#endif
