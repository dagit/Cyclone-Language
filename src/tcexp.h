#ifndef _TCEXP_H_
#define _TCEXP_H_

#include "absyn.h"
#include "tcenv.h"

namespace Tcexp{

using Absyn;
using Tcenv;

extern synth tcExp<`r>(tenv, typ *`r, exp);
extern synth tcExpInitializer<`r>(tenv, typ *`r, exp);
extern void tcTest(tenv te, exp e, string msg_part);
extern bool is_const_exp(tenv, exp);
}
#endif
