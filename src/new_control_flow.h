#ifndef NEW_CONTROL_FLOW_H
#define NEW_CONTROL_FLOW_H

#include "list.h"
#include "set.h"
#include "dict.h"
#include "stdio.h"
#include "absyn.h"
#include "cf_flowinfo.h"

namespace NewControlFlow {

using List;
using CfFlowInfo;

// to be used by the type-checking pass to get things set up for us
extern void set_encloser(Absyn::stmt enclosee, Absyn::stmt encloser);

extern struct AnalEnv {
  pinfo_dict_t<local_root_t> roots; // the "uninit leaves" versions
  bool        in_try;  // an optimization for the common case
  flow_info_t tryflow; // updated by UseAE and AssignAE, used by TryCatch_s
};
typedef struct AnalEnv @ analenv_t;

  // used by CFAbsexp::eval_absexp
extern void update_tryflow(analenv_t env, flow_info_t new_flow);

extern flow_info_t cf_analyze_stmt(analenv_t, Absyn::stmt, flow_info_t);

// The entry point (all the stuff above is just for cf_absexp)
void cf_check(list_t<Absyn::decl> ds);
}

#endif
