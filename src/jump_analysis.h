/* Type checking for top-level declarations
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
#ifndef _JUMP_ANALYSIS_H_
#define _JUMP_ANALYSIS_H_

#include <hashtable.h>
#include "absyn.h"

namespace JumpAnalysis {
using List;
using Absyn;

typedef Hashtable::table_t<`a,`b> table_t<`a,`b>;

// pop_tables: used by code-gen for how many exception handlers to pop
//     before doing a jump
// pat_pop_tables: used by flow-analysis on a jump for 
//     unconsuming unique pointers matched against.  The int is how 
//     many patterns with binding variables (so pattern 34 and _ don't count) 
//     are jumped out of
// succ_tables: used by flow-analysis to determine successor of a jump.
//     NULL in the range of a succ_table means destination is end of function
struct Jump_Anal_Result {
  table_t<fndecl_t,table_t<stmt_t,int>>        pop_tables;
  table_t<fndecl_t,table_t<stmt_t,stmt_opt_t>> succ_tables;
  table_t<fndecl_t,table_t<stmt_t,int>>        pat_pop_tables;
};
typedef struct Jump_Anal_Result @jump_anal_res_t;

jump_anal_res_t jump_analysis(list_t<decl_t,`H> tds);

}
#endif
