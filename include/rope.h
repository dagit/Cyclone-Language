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

#ifndef _ROPE_H_
#define _ROPE_H_
#include <core.h>
#include <list.h>

namespace Rope {
extern struct Rope_node;
typedef struct Rope_node @ rope_t;

extern rope_t from_string(string_t<`H>);
extern mstring_t to_string(rope_t);
extern rope_t concat(rope_t,rope_t);
extern rope_t concata(rope_t ?`H);
extern rope_t concatl(List::list_t<rope_t>);
extern uint length(rope_t);
extern int cmp(rope_t,rope_t);
}
#endif
