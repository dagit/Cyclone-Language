#ifndef TCEXP_H
#define TCEXP_H

#include "absyn.h"
#include "position.h"
#include "absynpp.h"
#include "tcenv.h"
#include "tcutil.h"
#include "evexp.h"

namespace Tcexp{

using Core;
using List;
using Position;
using Absyn;
using Absynpp;
using Tcenv;
using Tcutil;
using Evexp;

extern synth tcExp(tenv, Opt_t<typ>, exp);
extern bool is_const_exp(tenv, exp);
extern Opt_t<$(list<Char>, list<Char>, list<Char>, list<Char>, char, int)@>
       parse_conversionspecification(string s, int i);
}
#endif
