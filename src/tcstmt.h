#ifndef _TCSTMT_H_
#define _TCSTMT_H_

#include "absyn.h"
#include "tcenv.h"

namespace Tcstmt {
extern void tcStmt(Tcenv::tenv te, Absyn::stmt s, bool new_block);
}

#endif
