#ifndef CFABSEXP_H
#define CFABSEXP_H

#include "list.h"
#include "absyn.h"
#include "position.h"
#include "cf_flowinfo.h"
#include "new_control_flow.h"

namespace CfAbsexp {
using Absyn;
using CfFlowInfo;
using NewControlFlow;

extern xenum exn {BadAbsexp};

extern enum Absexp;
extern enum Absexpgroup { OrderedG; UnorderedG; OneofG };
extern enum Absop;
typedef enum Absexp      absexp_t;
typedef enum Absexpgroup absexpgroup_t;
typedef enum Absop       absop_t;

extern void check_absexp(absexp_t);
extern string absexp2string(absexp_t ae, int depth);

// new invariant: for any function, MallocAE(e) should occur at most once
// for any e.

// Basically, AnyOrderG is used in the following situations:
// * None of the AEs contain Assign or Malloc.
//   (Example use: arguments to a function)
// * The Assign AEs are exactly the outermost list and every lhs will evaluate
//   to something not reachable from any rhs.
//   (Example use: initializing malloc fields)
// The client could just use OrderedG but this function provides
// better documentation that we're not relying on left-to-right, rather
// we need and deserve the precision of Ordered despite non-determinism.
extern absexpgroup_t mkAnyOrderG();

// Violating the restrictions listed below causes BadAbsexp to be thrown.
extern absop_t mkUnknownOp();
extern absop_t mkAddressOp(absop_t ao);//ao must be Local, Member, or Malloc
extern absop_t mkLocalOp(vardecl vd);
extern absop_t mkMemberOp(absop_t ao,field_t f);//ao can't be Address or Unknown
extern absop_t mkMallocOp(exp e);
extern absop_t mkDerefOp(absop_t ao);//ao must be Local, Member, or Deref

extern absexp_t mkBottomAE();
extern absexp_t mkSkipAE();
extern absexp_t mkUseAE(absop_t ao);
extern absexp_t mkAssignAE(absop_t l, absop_t r);
extern absexp_t mkMallocAE(exp e);
extern absexp_t mkStmtAE(stmt s);
extern absexp_t mkGroupAE(absexpgroup_t g, absexp_t ae1, absexp_t ae2);
extern absexp_t mkGroupAE_l(absexpgroup_t g, List::list_t<absexp_t> ael);

extern bool isUnknownOp(absop_t);

extern flow_info_t eval_absexp(analenv_t env, Position::seg_t loc, 
			       absexp_t ae, flow_info_t in_flow);
}

#endif
