/* This file is part of the Cyclone Library.
   Copyright (C) 2000-2001 Greg Morrisett

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

#ifndef _PWD_H
#define _PWD_H

#include <core.h> // for string_t
#include <sys/types.h>

struct passwd {
  string_t pw_name;
  string_t pw_passwd;
  uid_t pw_uid;
  gid_t pw_gid;
  string_t pw_gecos;
  string_t pw_dir;
  string_t pw_shell;
};

extern struct passwd *getpwnam(string_t name);
extern struct passwd *getpwuid(uid_t uid);

#endif
