#ifndef _TOC_H_
#define _TOC_H_

#include "list.h"
#include "absyn.h"

namespace Toc {
using List;
using Absyn;

// translate the declarations to C
extern list_t<decl_t> toc(list_t<decl_t> ds);
// set to false/true to turn off/on null checking in generated code
extern bool check_for_null;
}
#endif
