#ifndef ABSYNDUMP_H
#define ABSYNDUMP_H

#include "core.h"
#include "list.h"
#include "absyn.h"

// not re-entrant.  Entire file ends up on one line!

namespace Absyndump {
extern void dumpdecllist2file(List::list<Absyn::decl> tdl, FILE @f);
}

#endif
