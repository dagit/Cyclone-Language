#ifndef _PRATT_PROVER_H_
#define _PRATT_PROVER_H_

#include "assndef.h"


namespace PrattProver{
  bool constraint_prove(AssnDef::assn_t ctxt, AssnDef::assn_t a);
  AssnDef::assn_set_opt_t filter_implies(AssnDef::assn_set_t ctxt,
                                         AssnDef::assn_set_t a);
  //AssnDef::assn_t merge_assn(AssnDef::assn_t a1, AssnDef::assn_t a2);
}

#endif
