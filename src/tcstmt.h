#ifndef TCSTMT_H
#define TCSTMT_H

#include "absyn.h"
#include "tcenv.h"

namespace Tcstmt {

using Absyn;
using Tcenv;

extern synth tcStmt(tenv,stmt);

}

#endif
