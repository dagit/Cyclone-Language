#ifndef TC_H
#define TC_H

#include "list.h"
#include "absyn.h"
#include "tcenv.h"

namespace Tc {

using List;
using Absyn;
using Tcenv;

extern void tc(tenv te, list<decl>);

}
#endif
