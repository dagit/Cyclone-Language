/* Check insertion
   Copyright (C) 2004 Dan Grossman, AT&T
   This file is part of the Cyclone compiler.

   The Cyclone compiler is free software; you can redistribute it
   and/or modify it under the terms of the GNU General Public License
   as published by the free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   The Cyclone compiler is distributed in the hope that it will be
   useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with the Cyclone compiler; see the file COPYING. If not,
   write to the Free Software Foundation, Inc., 59 Temple Place -
   Suite 330, Boston, MA 02111-1307, USA. */

#ifndef _INSERT_CHECKS_H
#define _INSERT_CHECKS_H

#include "absyn.h"
#include "jump_analysis.h"

namespace InsertChecks {
@extensible datatype Absyn::AbsynAnnot {
  extern NoCheck;
  extern NullOnly;
  extern NullAndFatBound;
  extern FatBound;
  extern NullAndThinBound(Absyn::exp_t); // exp must be a C constant expression
  extern ThinBound(Absyn::exp_t);        // exp must be a C constant expression
};
extern_datacon(Absyn::AbsynAnnot,NoCheck);
extern_datacon(Absyn::AbsynAnnot,NullOnly);
extern_datacon(Absyn::AbsynAnnot,NullAndFatBound);
extern_datacon(Absyn::AbsynAnnot,FatBound);

void insert_checks(List::list_t<Absyn::decl_t,`H>, 
		   JumpAnalysis::jump_anal_res_t,bool);
}

#endif
