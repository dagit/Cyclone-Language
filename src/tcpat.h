#ifndef TCPAT_H
#define TCPAT_H

#include "absyn.h"
#include "tcenv.h"
#include "list.h"

namespace Tcpat {

using Absyn;
using Tcenv;
using List;

extern $(list<tvar>, list<vardecl>)@ tcPat(tenv,pat);
extern void check_switch_exhaustive(segment,list<switch_clause>);
extern bool check_let_pat_exhaustive(segment,pat p); // true => exhaustive
extern void check_catch_overlap(segment,list<switch_clause>);

}

#endif
