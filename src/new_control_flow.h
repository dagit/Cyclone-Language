#ifndef NEW_CONTROL_FLOW_H
#define NEW_CONTROL_FLOW_H

#include "list.h"
#include "dict.h"
#include "stdio.h"
#include "absyn.h"
#include "cf_flowinfo.h"

namespace NewControlFlow {

using List;
using CfFlowInfo;

extern flow_info_t cf_analyze_stmt(Absyn::stmt, flow_info_t);

// The entry point (all the stuff above is just for cf_absexp)
void cf_check(list_t<Absyn::decl> ds);
}

#endif
