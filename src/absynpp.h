#ifndef ABSYNPP_H
#define ABSYNPP_H

#include "core.h"
#include "list.h"
#include "pp.h"
#include "absyn.h"

namespace Absynpp {
using Absyn;
using List;
using PP;

extern bool expand_typedefs;

extern void decllist2file(list<decl> tdl, FILE @f);

extern Doc decl2doc(decl d);

extern string typ2string(typ);
extern string exp2string(exp);
extern string stmt2string(stmt);
extern string qvar2string(qvar);
extern string decllist2string(list<decl> tdl);
extern string prim2string(primop p);
extern string pat2string(pat p);
}
#endif
