#ifndef EVEXP_H
#define EVEXP_H

#include "absyn.h"
#include "position.h"

using Absyn;
using Position;

namespace Evexp {
extern unsigned int eval_const_uint_exp(exp e);
extern void exp_err(segment sg,string msg);
}
#endif
