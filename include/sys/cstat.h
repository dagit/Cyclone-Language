/* This file is part of the Cyclone Library.
   Copyright (C) 2001 Greg Morrisett

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

/* Some constants taken from the GNU C Library:
   Copyright (C) 1991-1999, 2000, 2001 Free Software Foundation, Inc.
*/

#ifndef _SYS_STAT_H
#define _SYS_STAT_H

#include <ctime.h>

namespace Std {
#define S_IFMT   0170000
#define S_IFSOCK 0140000
#define S_IFLNK  0120000
#define S_IFREG  0100000
#define S_IFBLK  0060000
#define S_IFDIR  0040000
#define S_IFCHR  0020000
#define S_IFIFO  0010000
#define S_ISUID  0004000
#define S_ISGID  0002000
#define S_ISVTX  0001000
#define S_IRWXU  00700
#define S_IRUSR  00400
#define S_IWUSR  00200
#define S_IXUSR  00100
#define S_IEXEC  00100
#define S_IRWXG  00070
#define S_IRGRP  00040
#define S_IWGRP  00020
#define S_IXGRP  00010
#define S_IRWXO  00007
#define S_IROTH  00004
#define S_IWOTH  00002
#define S_IXOTH  00001

#define S_ISREG(m) (((m) & S_IFMT) == (S_IFREG))
#define S_ISDIR(m) (((m) & S_IFMT) == (S_IFDIR))
#define S_ISCHR(m) (((m) & S_IFMT) == (S_IFCHR))
#define S_ISBLK(m) (((m) & S_IFMT) == (S_IFBLK))
#define S_ISFIFO(m) (((m) & S_IFMT) == (S_IFIFO))
#define S_ISLNK(m) (((m) & S_IFMT) == (S_IFLNK))
#define S_ISSOCK(m) (((m) & S_IFMT) == (S_IFSOCK))

#if defined(__CYGWIN__)
/* from Cygwin's sys/stat.h */
struct	stat_t
{
  dev_t		st_dev;
  ino_t		st_ino;
  mode_t	st_mode;
  nlink_t	st_nlink;
  uid_t		st_uid;
  gid_t		st_gid;
  dev_t		st_rdev;
  off_t		st_size;
  /* SysV/sco doesn't have the rest... But Solaris, eabi does.  */
#if defined(__svr4__) && !defined(__PPC__) && !defined(__sun__)
  time_t	st_atime;
  time_t	st_mtime;
  time_t	st_ctime;
#else
  time_t	st_atime;
  long		st_spare1;
  time_t	st_mtime;
  long		st_spare2;
  time_t	st_ctime;
  long		st_spare3;
  long		st_blksize;
  long		st_blocks;
  long	st_spare4[2];
#endif
};
#else
#if defined(__linux__)
  struct stat_t {
    dev_t st_dev;
    unsigned short int __pad1;
    ino_t st_ino;
    mode_t st_mode;
    nlink_t st_nlink;
    uid_t st_uid;
    gid_t st_gid;
    dev_t st_rdev;
    unsigned short int __pad2;
    off_t st_size;
    blksize_t st_blksize;
    blkcnt_t st_blocks;
    time_t st_atime;
    unsigned long int __unused1;
    time_t st_mtime;
    unsigned long int __unused2;
    time_t st_ctime;
    unsigned long int __unused3;
    unsigned long int __unused4;
    unsigned long int __unused5;
  };
#else 
  struct stat_t {
    dev_t st_dev;
    ino_t st_ino;
    mode_t st_mode;
    nlink_t st_nlink;
    uid_t st_uid;
    gid_t st_gid;
    dev_t st_rdev;
    off_t st_size;
    unsigned long st_blksize;
    unsigned long st_blocks;
    time_t st_atime;
    time_t st_mtime;
    time_t st_ctime;
  };
#endif
#endif
  extern int stat(string_t filename, struct stat_t @`r buf);
  extern "C" int fstat(int fd, struct stat_t @`r buf);
  extern int lstat(string_t filename, struct stat_t @`r buf);
  extern "C" mode_t umask(mode_t mask);
  extern int mkdir(string_t pathname, mode_t mode);
  extern int chmod(string_t path,mode_t mode);
  extern "C" int fchmod(int fd, mode_t mode);
}

#endif
