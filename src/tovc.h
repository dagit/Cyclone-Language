#ifndef TOVC_H
#define TOVC_H

#include "list.h"

namespace Tovc {

extern List::list_t<Absyn::decl_t> tovc(List::list_t<Absyn::decl_t> decls);

}
#endif
