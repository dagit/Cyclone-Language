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

#ifndef _TIME_H_
#define _TIME_H_

#include <core.h>
#include <sys/ctypes.h>
#include <cstddef.h>

namespace Std {

  // What should this be under Cygwin?
#define CLOCKS_PER_SEC 1000000l

  struct tm {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
  };

  extern "C" void tzset(void);
  extern "C" time_t time(time_t *`r t);
  extern mstring_t asctime(const struct tm @`r timeptr);
  extern mstring_t ctime(const time_t @`r timep);
  extern "C" struct tm @gmtime(const time_t @`r timep);
  extern "C" struct tm @localtime(const time_t @`r timep);
  extern size_t strftime(string_t s, size_t maxsize, string_t fmt, 
                         const struct tm @`r t);
  extern mstring_t asctime_r(const struct tm @`r, mstring_t);
  extern mstring_t ctime_r(const time_t @`r, mstring_t);

  extern "C" long timezone;
  extern "C" int daylight;
}

#endif
