#ifndef _TOC_H_
#define _TOC_H_

#include <list.h>
#include "absyn.h"

namespace Toc {
using List;
using Absyn;

// translate the declarations to C
extern list_t<decl_t> toc(list_t<decl_t> ds);

  // this is just for Tovc
  extern qvar_t temp_var();
}
#endif
