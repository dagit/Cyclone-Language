#ifndef TCPAT_H
#define TCPAT_H

#include "list.h"
#include "position.h"
#include "absyn.h"
#include "tcenv.h"

namespace Tcpat {

using List;
using Position;
using Absyn;
using Tcenv;

extern $(list<tvar>, list<vardecl>)@ tcPat(tenv,pat);
extern void check_switch_exhaustive(segment,list<switch_clause>);
extern bool check_let_pat_exhaustive(segment,pat p); // true => exhaustive
extern void check_catch_overlap(segment,list<switch_clause>);

}

#endif
