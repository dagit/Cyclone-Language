#ifndef _EVEXP_H_
#define _EVEXP_H_

#include "absyn.h"
#include <position.h>

namespace Evexp {

using Absyn;
using Position;

extern unsigned int eval_const_uint_exp(exp_t e);
extern unsigned int szof(type_t t, seg_t loc);

}
#endif
