#ifndef CFABSEXP_H
#define CFABSEXP_H

#include "list.h"
#include "absyn.h"
#include "new_control_flow.h"

namespace CFabsexp {
using Absyn;
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
 
// violate the restrictions listed below and BadAbsexp is thrown.
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
}

#endif
