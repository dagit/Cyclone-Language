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

/* A few constants taken from the GNU C Library, released under LGPL:
   Copyright (C) 1991-1999, 2000, 2001 Free Software Foundation, Inc. */

#ifndef _UNISTD_H
#define _UNISTD_H

#include <core.h>
#include <sys/ctypes.h>
#include <sys/ctime.h>  // for struct timeval
#include <cgetopt.h>    // Since ordinary getopt() belongs in unistd.h

namespace Std {

/* Values for the WHENCE argument to lseek.  */
#ifndef	_STDIO_H		/* <stdio.h> has the same definitions.  */
# define SEEK_SET	0	/* Seek from beginning of file.  */
# define SEEK_CUR	1	/* Seek from current position.  */
# define SEEK_END	2	/* Seek from end of file.  */
#endif

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

/* For second arg of access */
#define	R_OK 4
#define	W_OK 2
#define	X_OK 1
#define	F_OK 0

  extern "C" {
    unsigned alarm(unsigned seconds);
    int close(int);
    void _exit(int);
    pid_t getpid(void);
    pid_t getppid(void);
    pid_t fork(void);
    int fchdir(int);
    int fchown(int,uid_t,gid_t);
    int fsync(int);
    int ftruncate(int, off_t);
    int dup(int);
    int dup2(int, int);
    uid_t getuid(void);  int setuid(uid_t uid);
    uid_t geteuid(void); int seteuid(uid_t euid);
    gid_t getgid(void);  int setgid(gid_t gid);
    gid_t getegid(void); int setegid(gid_t egid);
    int nice(int);
    int pause(void);
    int pipe(int @{2}`r filedes);
    off_t lseek(int filedes, off_t offset, int whence);
    unsigned sleep(unsigned);
  }

  int access(string_t,int);
  int chdir(string_t);
  int chown(string_t, uid_t, gid_t);
  char ?`r getcwd(char ?`r buf, size_t size);
  int execl(string_t path, string_t arg0, ... string_t argv);
  int execlp(string_t file, string_t arg0, ... string_t argv);
  //int execv(string_t path, string_t const ?argv);
  //int execp(string_t file, string_t const ?argv);
  int execve(string_t filename, string_t const ?argv, string_t const ?envp);
  int link(string_t,string_t);
  ssize_t read(int fd, mstring_t buf, size_t count);
  int rmdir(string_t);
  int symlink(string_t,string_t);
  int truncate(string_t, off_t);
  ssize_t write(int fd, string_t buf, size_t count);
  int unlink(string_t pathname);
}

#endif
