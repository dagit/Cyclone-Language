#ifndef TCEXP_H
#define TCEXP_H

#include "absyn.h"
#include "tcenv.h"

namespace Tcexp{

using Absyn;
using Tcenv;

extern synth tcExp(tenv, Core::Opt_t<typ>, exp);
extern void tcTest(tenv te, exp e, string msg_part);
extern bool is_const_exp(tenv, exp);
}
#endif
