/* Control flow analysis.
   Copyright (C) 2001 Dan Grossman, Greg Morrisett
   This file is part of the Cyclone compiler.

   The Cyclone compiler is free software; you can redistribute it
   and/or modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   The Cyclone compiler is distributed in the hope that it will be
   useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with the Cyclone compiler; see the file COPYING. If not,
   write to the Free Software Foundation, Inc., 59 Temple Place -
   Suite 330, Boston, MA 02111-1307, USA. */


#ifndef NEW_CONTROL_FLOW_H
#define NEW_CONTROL_FLOW_H

#include <list.h>
#include <set.h>
#include <dict.h>
#include <stdio.h>
#include "absyn.h"
#include "cf_flowinfo.h"

namespace NewControlFlow {

using List;
using CfFlowInfo;

// to be used by the type-checking pass to get things set up for us
extern void set_encloser(Absyn::stmt_t enclosee, Absyn::stmt_t encloser);

extern struct AnalEnv {
  pinfo_dict_t<local_root_t> roots; // the "uninit leaves" versions
  bool        in_try;  // an optimization for the common case
  flow_info_t tryflow; // updated by UseAE and AssignAE, used by TryCatch_s
};
typedef struct AnalEnv @ analenv_t;

  // used by CFAbsexp::eval_absexp
extern void update_tryflow(analenv_t env, flow_info_t new_flow);

extern flow_info_t cf_analyze_stmt(analenv_t, Absyn::stmt_t, flow_info_t);

// The entry point (all the stuff above is just for cf_absexp)
extern void cf_check(list_t<Absyn::decl_t> ds);
}

#endif
