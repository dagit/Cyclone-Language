#ifndef ROPE_H
#define ROPE_H
#include "core.h"
#include "list.h"

namespace Rope {
extern struct Rope_node;
typedef struct Rope_node @rope;

extern rope from_string(string);
extern string to_string(rope);
extern rope concat(rope,rope);
extern rope concata(rope[?]);
extern rope concatl(List::list<rope>);
extern uint length(rope);
extern int cmp(rope,rope);
}
#endif
