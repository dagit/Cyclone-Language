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

  // You must call tcPat, then unify with the type of the value on which
  // you're switching, then call check_pat_regions.
  // If someone has a less clumsy proposal, I'd love to hear it.
extern 
$(list<tvar>, list<vardecl>)@ tcPat<`p>(tenv_t te,pat p,typ *`p region_opt);
extern void check_pat_regions(tenv_t te, pat p);
extern void check_switch_exhaustive(segment,list<switch_clause>);
extern bool check_let_pat_exhaustive(segment,pat p); // true => exhaustive
extern void check_catch_overlap(segment,list<switch_clause>);

}

#endif
