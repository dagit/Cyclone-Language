#ifndef _TC_H_
#define _TC_H_

#include "list.h"
#include "absyn.h"
#include "tcenv.h"

namespace Tc {

using List;
using Absyn;
using Tcenv;

extern void tc(tenv te, bool add_cyc_namespace, list_t<decl> ds);

  // if called, must be after tc is called
  // removes extern variables that are never referred to
extern list_t<decl> treeshake(tenv te, list_t<decl>);

}
#endif
