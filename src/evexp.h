#ifndef EVEXP_H
#define EVEXP_H

#include "absyn.h"
#include "position.h"

namespace Evexp {

using Absyn;
using Position;

extern unsigned int eval_const_uint_exp(exp e);
extern unsigned int szof(typ t, segment loc);

}
#endif
