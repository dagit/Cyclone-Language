#ifndef TC_H
#define TC_H

#include "list.h"
#include "absyn.h"
#include "tcenv.h"

namespace Tc {

using List;
using Absyn;
using Tcenv;

extern void tc(tenv te, list<decl>);

  // if called, must be after tc is called
  // removes extern variables that are never referred to
extern list<decl> treeshake(tenv te, list<decl>);

}
#endif
