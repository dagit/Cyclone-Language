/* Type checking declarations.
   Copyright (C) 2001 Greg Morrisett
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
#ifndef _TCDECL_H_
#define _TCDECL_H_

#include "absyn.h"
#include <cycboot.h>

namespace Tcdecl {
  using Absyn;

  // used by Interface
  extern datatype exn {extern Incompatible};
  struct Xdatatypefielddecl {
    datatypedecl_t base; //no fields here
    datatypefield_t field;
  };
  typedef struct Xdatatypefielddecl @ xdatatypefielddecl_t;

  // if msg0 == null, don't print any message; just throw exception Incompatible
  // otherwise call terr(loc, msg) where msg is *msg0 + " " + msg1
  void merr(seg_t loc, string_t<`r2> * msg1, string_t fmt,
	  ... inject parg_t<`r> ap : {`r2} > `r)
    __attribute__((format(printf,3,4)));
  
  // used by Interface and Tc

  // all these functions work the same way :
  // _ they check compatibility between d0 (previous top-level declaration)
  //   and d1 (new top-level declaration)
  // _ if something's wrong, call merr and return Error
  // _ don't modify d0 or d1
  // _ if d0 or d1 is the answer return First or Second (respectively)
  //   hence the test for inclusion d1 <= d0 can be written is 
  //      merge(d0, d1) is First
  // _ from a 'semantic' point of view, merge(d0, d1) = merge(d1, d0)
  // _ we assume the declarations have already been type-checked
struct Aggrdecl * merge_aggrdecl(aggrdecl_t, aggrdecl_t,
				 seg_t, string_t * msg);

struct Datatypedecl * merge_datatypedecl(datatypedecl_t,datatypedecl_t,
					 seg_t, string_t * msg);
struct Enumdecl * merge_enumdecl(enumdecl_t,enumdecl_t, 
				 seg_t, string_t * msg);
struct Vardecl * merge_vardecl(vardecl_t, vardecl_t, seg_t, 
			       string_t * msg);
struct Typedefdecl * merge_typedefdecl(typedefdecl_t, typedefdecl_t,
				       seg_t, string_t * msg);

datatype Binding * merge_binding(binding_t,binding_t,seg_t,string_t*msg);
  
struct Xdatatypefielddecl *
 merge_xdatatypefielddecl (xdatatypefielddecl_t, xdatatypefielddecl_t, 
			   seg_t, string_t * msg);

  // sort the list of fields, trying to merge the duplicate
  // *res is set to false if something's wrong
List::list_t<datatypefield_t>
sort_xdatatype_fields(List::list_t<datatypefield_t,`H> f, bool @ res,
		      var_t v, seg_t, string_t * msg);
}

#endif /* _TCDECL_H_ */
