#ifndef _ROPE_H_
#define _ROPE_H_
#include "core.h"
#include "list.h"

namespace Rope {
extern struct Rope_node;
typedef struct Rope_node @rope_t;

extern rope_t from_string(string_t);
extern mstring_t to_string(rope_t);
extern rope_t concat(rope_t,rope_t);
extern rope_t concata(rope_t?);
extern rope_t concatl(List::list_t<rope_t>);
extern uint length(rope_t);
extern int cmp(rope_t,rope_t);
}
#endif
