/* Verification condition generation.
   Copyright (C) 2003 Dan Grossman, AT&T
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
#ifndef _VCGEN_H_
#define _VCGEN_H_

#include <list.h>
#include <hashtable.h>
#include "absyn.h"
#include "jump_analysis.h"
#include "assndef.h"



namespace Vcgen {
  typedef struct ExpChecks {
    AssnDef::assn_t ctxt;
    AssnDef::assn_t bounds_check;
    AssnDef::assn_t null_check;
  } exp_checks_t;
  typedef Hashtable::table_t<Absyn::exp_t, exp_checks_t@> *assn_info_t;
  assn_info_t new_assn_info();
  extern void vcgen_fundecl(Position::seg_t loc,
                            JumpAnalysis::jump_anal_res_t tables, 
			    Absyn::fndecl_t fd,
			    assn_info_t assn_info);
  exp_checks_t@ exp2ctxt_checks(assn_info_t,Absyn::exp_t);
  extern AssnDef::assn_map_t clause2assn(Absyn::exp_t);
}
#endif
