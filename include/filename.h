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


#ifndef _FILENAME_H_
#define _FILENAME_H_
#include <core.h>

namespace Filename {
  //extern string current_dir_name;
extern mstring_t concat(string_t, string_t);
extern mstring_t chop_extension(string_t);
extern mstring_t dirname(string_t);
extern mstring_t basename(string_t);
extern bool check_suffix(string_t, string_t);
extern mstring_t gnuify(string_t);
}

#endif
