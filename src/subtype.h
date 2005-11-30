#ifndef _SUBTYPE_H_
#define _SUBTYPE_H_

#include "absyn.h"
#include "bansheeif.h"

namespace Subtype {
  //  typedef bool (@`H genconstr_t<`r::E>)(List::list_t<BansheeIf::constraint_t,`H>@`r, Absyn::type_t, Absyn::type_t);
  Absyn::coercion_t coercible(Absyn::seg_t,Absyn::type_t t1, Absyn::type_t t2);
  Absyn::coercion_t coercible_warn(Absyn::seg_t,Absyn::type_t t1, Absyn::type_t t2);
  Absyn::coercion_t coercible_exact(Absyn::seg_t,Absyn::type_t t1, Absyn::type_t t2);
//   Absyn::coercion_t coercible_c(Absyn::type_t t1, Absyn::type_t t2, 
// 				bool genconstr<`r::E>(List::list_t<BansheeIf::constraint_t,`H>@`r,
// 						      Absyn::type_t, Absyn::type_t),
// 				bool allow_deep_subtype);
  bool subtype(Absyn::seg_t loc, List::list_t<$(Absyn::type_t,Absyn::type_t)@`H,`H> assume,
	       Absyn::type_t t1, Absyn::type_t t2);
  bool check_aqual_bounds(Absyn::aqualbnds_t aquals_bnd, 
			  Absyn::aqualtype_t aq, 
			  Absyn::aqualtype_t bnd);
}
#endif
