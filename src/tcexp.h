#ifndef _TCEXP_H_
#define _TCEXP_H_

#include "absyn.h"
#include "tcenv.h"

namespace Tcexp{

using Absyn;
using Tcenv;

extern typ tcExp(tenv, typ *`r, exp);
extern typ tcExpInitializer(tenv, typ *`r, exp);
extern void tcTest(tenv te, exp e, string msg_part);
}
#endif
