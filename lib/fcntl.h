#ifndef _FCNTL_H
#define _FCNTL_H

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

  // We don't have stdarg yet, so we have to write three cases:
  extern "C" int fcntl(int fd, int cmd);
  extern "C" int fcntl_with_arg(int fd, int cmd, long arg);
  extern "C" int fcntl_with_lock(int fd, int cmd, struct flock *lock);

}

#endif
