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
#ifndef _LINENO_H_
#define _LINENO_H_

#include <list.h>

namespace Lineno {
extern struct Pos {
  string_t logical_file;
  mstring_t line;
  int    line_no;
  int    col;
};
typedef struct Pos @pos_t;
pos_t pos_of_abs(string_t,int);
void  poss_of_abss(string_t filename, List::list_t<$(int,pos_t)@,`H> places);
}
#endif
