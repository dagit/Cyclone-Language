#ifndef _SPLAY_H_
#define _SPLAY_H_

#include "core.h"
#include "list.h"

namespace Splay {

struct node<`a,`b>;

struct noderef<`a,`b> {
  struct node<`a,`b> @v;
};

extern enum tree<`a,`b> {
  Leaf,
  Node(struct noderef<`a,`b> @)
};

struct node<`a,`b> {
  `a key;
  `b data;
  enum tree<`a,`b> left;
  enum tree<`a,`b> right;
};

extern bool splay<`a,`b>(int f(`a,`a), `a, enum tree<`a,`b>);

}
#endif
