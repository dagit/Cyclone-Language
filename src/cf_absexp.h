/* Abstract expressions for control flow.
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

#ifndef CFABSEXP_H
#define CFABSEXP_H

#include <list.h>
#include <dict.h>
#include <position.h>
#include "absyn.h"
#include "cf_flowinfo.h"
#include "new_control_flow.h"

namespace CfAbsexp {
using Absyn;
using CfFlowInfo;
using NewControlFlow;

extern xtunion exn {extern BadAbsexp};

extern tunion Absexp;
extern tunion Absop;
extern tunion Absexpgroup { OrderedG; UnorderedG; OneofG };

typedef tunion Absexp      absexp_t;
typedef tunion Absexpgroup absexpgroup_t;
typedef tunion Absop       absop_t;

extern void check_absexp(absexp_t);
extern string_t absexp2string(absexp_t ae, int depth);

// invariant: for any function, MallocAE(e) should occur at most once for any e.

// Violating the restrictions listed below causes BadAbsexp to be thrown.
extern absop_t mkUnknownOp();
extern absop_t mkLocalOp(vardecl_t);
extern absop_t mkMallocOp(exp_t);
extern absop_t mkDictOp(Dict::dict_t<field_name_t,absop_t,`H>);
extern absop_t mkAddressOp(absop_t);//op must be Local, Member, or Malloc
extern absop_t mkMemberOp(absop_t,field_name_t f);//op can't be Address or Unknown
extern absop_t mkDerefOp(absop_t);//ao must be Local, Member, or Deref

extern absexp_t mkBottomAE();
extern absexp_t mkSkipAE();
extern absexp_t mkUseAE(absop_t ao);
extern absexp_t mkAssignAE(absop_t l, absop_t r);
extern absexp_t mkMallocAE(exp_t e);
extern absexp_t mkStmtAE(stmt_t s);
extern absexp_t mkComprehensionAE(vardecl_t vd, 
				  absexp_t  size_true_exp,
				  absexp_t  size_false_exp,
				  absop_t   size_aop, // assigned to vd
				  absexp_t  body_exp, 
				  absop_t   body_op); // body_op used
extern absexp_t mkGroupAE(absexpgroup_t g, absexp_t ae1, absexp_t ae2);
extern absexp_t mkGroupAE_l(absexpgroup_t g, List::list_t<absexp_t,`H> ael);

extern bool isUnknownOp(absop_t);

extern flow_info_t eval_absexp(analenv_t env, Position::seg_t loc, 
			       absexp_t ae, flow_info_t in_flow);
}

#endif

