#ifndef _TCEXP_H_
#define _TCEXP_H_

#include "absyn.h"
#include "tcenv.h"

namespace Tcexp{

using Absyn;
using Tcenv;

extern type_t tcExp(tenv_t, type_t *`r, exp_t);
extern type_t tcExpInitializer(tenv_t, type_t *`r, exp_t);
extern void tcTest(tenv_t te, exp_t e, string_t msg_part);
}
#endif
