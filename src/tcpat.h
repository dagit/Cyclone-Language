#ifndef _TCPAT_H_
#define _TCPAT_H_

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
$(list_t<tvar_t>, list_t<vardecl_t>)@ 
   tcPat(tenv_t te,pat_t p,type_t *`p region_opt);
extern void check_pat_regions(tenv_t te, pat_t p);
extern void check_switch_exhaustive(seg_t,list_t<switch_clause_t>);
extern bool check_let_pat_exhaustive(seg_t,pat_t p); // true => exhaustive
extern void check_catch_overlap(seg_t,list_t<switch_clause_t>);

}

#endif
