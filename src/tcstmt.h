#ifndef TCSTMT_H
#define TCSTMT_H

#include "absyn.h"
#include "tcenv.h"

namespace Tcstmt {

extern void tcStmt(Tcenv::tenv,Absyn::stmt);

}

#endif
