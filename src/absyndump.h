#ifndef _ABSYNDUMP_H_
#define _ABSYNDUMP_H_

#include "core.h"
#include "stdio.h"
#include "list.h"
#include "absyn.h"

// not re-entrant.  Entire file ends up on one line!

namespace Absyndump {
extern void dumpdecllist2file(List::list<Absyn::decl> tdl, Stdio::FILE @f);
}

#endif
