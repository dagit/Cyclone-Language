#ifndef TOC_H
#define TOC_H

#include "list.h"
#include "absyn.h"

namespace Toc {
using List;
using Absyn;

// translate the declarations to C
extern list<decl> toc(list<decl> ds);
// set to false/true to turn off/on null checking in generated code
extern bool check_for_null;
}
#endif
