#ifndef _SPLAY_H_
#define _SPLAY_H_

// TO DO:  add region support

#include "core.h"
#include "list.h"

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
