#ifndef _TC_H_
#define _TC_H_

#include <list.h>
#include "absyn.h"
#include "tcenv.h"

namespace Tc {

using List;
using Absyn;
using Tcenv;

extern void tc(tenv_t te, bool var_default_init, list_t<decl_t> ds);

  // if called, must be after tc is called
  // removes extern variables that are never referred to
extern list_t<decl_t> treeshake(tenv_t te, list_t<decl_t>);

}
#endif
