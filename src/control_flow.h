#ifndef CONTROL_FLOW_H
#define CONTROL_FLOW_H

#include "list.h"
#include "absyn.h"

namespace ControlFlow {

  // to be used by the type-checking pass to get things set up for us
  extern void set_encloser(Absyn::stmt enclosee, Absyn::stmt encloser);

  // top-level entry for this pass
  extern void cf_check(List::list<Absyn::decl>);
}

#endif
