/* Routines for evaluating expressions that should be "constant".
   Copyright (C) 2001 Greg Morrisett, AT&T
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

#ifndef _EVEXP_H_
#define _EVEXP_H_
#include "absyn.h"

namespace Evexp {
  // returns false if e is constant but contains a sizeof or offsetof
  // if it's not constant, an error is reported and $(0,true) returned
extern $(unsigned int,bool) eval_const_uint_exp(Absyn::exp_t e);

  // returns true iff we are sure the two es are the same (or lessthan) constant
  // an error is reported if either is not a constant expression
  // if either has sizeof or offsetof, then the two es must be essentially
  // equal syntactically.  Therefore, false is conservative -- they might
  // be the same or lessthan -- we just don't know.
  // For all of these, the exp should have been type-checked b/c of 
  //   sizeofexp_e
extern bool same_const_exp(Absyn::exp_t e1, Absyn::exp_t e2);
extern bool lte_const_exp(Absyn::exp_t e1, Absyn::exp_t e2);
  // usable for sorting, trees, etc.  returns 0 iff same_const_exp returns true
  // we need this for type_cmp unfortunately
extern int  const_exp_cmp(Absyn::exp_t e1, Absyn::exp_t e2);

  // only in this module for historical reasons
extern bool okay_szofarg(Absyn::type_t t);
}
#endif
