#ifndef _EVEXP_H_
#define _EVEXP_H_

#include "absyn.h"
#include <position.h>

namespace Evexp {

using Absyn;
using Position;

extern unsigned int eval_const_uint_exp(exp_t e);

  // only in this module for historical reasons
extern bool okay_szofarg(type_t t);

}
#endif
