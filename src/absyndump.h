#ifndef _ABSYNDUMP_H_
#define _ABSYNDUMP_H_

#include <core.h>
#include <stdio.h>
#include <list.h>
#include "absyn.h"

// not re-entrant.  Entire file ends up on one line!

namespace Absyndump {
extern void set_params(struct Absynpp::Params @ `r fs);
extern void dumpdecllist2file(List::list_t<Absyn::decl_t> tdl, Stdio::FILE @f);
}

#endif
