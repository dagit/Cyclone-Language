/* This file is part of the Cyclone Library.
   Copyright (C) 1999-2001 Greg Morrisett, Dan Grossman

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

#ifndef _SPLAY_H_
#define _SPLAY_H_

// TO DO:  add region support

#include <core.h>
#include <list.h>

namespace Splay {

struct node<`a,`b>;

struct noderef<`a,`b> {
  struct node<`a,`b> @v;
};

extern tunion tree<`a,`b> {
  Leaf,
  Node(struct noderef<`a,`b> @)
};

struct node<`a,`b> {
  `a key;
  `b data;
  tunion tree<`a,`b> left;
  tunion tree<`a,`b> right;
};

extern bool splay(int f(`a,`a), `a, tunion tree<`a,`b>);

}
#endif
