#ifndef TCSTMT_H
#define TCSTMT_H

#include "absyn.h"
#include "tcenv.h"

namespace Tcstmt {
extern void tcStmt(Tcenv::tenv te, Absyn::stmt s, bool new_block);
}

#endif
