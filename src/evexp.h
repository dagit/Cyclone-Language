#ifndef _EVEXP_H_
#define _EVEXP_H_

#include "absyn.h"
#include "position.h"

namespace Evexp {

using Absyn;
using Position;

extern unsigned int eval_const_uint_exp(exp e);
extern unsigned int szof(typ t, seg_t loc);

}
#endif
