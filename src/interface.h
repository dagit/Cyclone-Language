/* Interface checking.
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


#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include "tcenv.h"

namespace Interface {
  using List;
  using Absyn;
  using Tcenv;

  // abstract type for program interfaces
  extern struct I;
  typedef struct I @ i_t;

  // a new empty interface
  extern i_t empty();

  // a new interface which describes what is automatically provided and needed when
  // building an executable file, so far :
  //   @extensible datatype exn;
  //   extern int main(int, char ??);
  // the test whether a bunch of files whose interfaces are li = [i1..in] is linkable
  // to an executable file can be written :
  //   is_subinterface(empty(), merge_list(new List(final(), li), null))
  extern i_t final();
  
  // build a maximal interface from the global environment computed by the type-checker
  // also check that each (non extern) function prototype is eventually provided
  extern i_t extract(genv_t, list_t<decl_t>);

  // check if i1 may safely be used instead of i2 (ie. i1 is more restrictive than i2) :
  // _ everything provided by in i1 is provided by i2
  // _ everything needed by i2 was needed by i1
  // special case : typedefs may not be included, compatibility is enough 
  // if info == null, no error message is printed
  // else we use (*info)[k] as a name for ik
  extern bool is_subinterface(i_t i1, i_t i2, $(string_t<`H>, string_t<`H>) * `r info);

  // try to merge 2 interfaces
  // _ if they're incompatible, return null (and if info != null, print some error messages)
  // _ the new exports are e = e1 U[disjoint] e2
  // _ the new imports are i = (i1 U[compatible] i2) -[included] e
  // special case : a same typedef can appear in both e1 and e2
  // if info == null, no error message is printed
  // else we use (*info)[k] as a name for ik
  extern struct I * merge(i_t i1, i_t i2, $(string_t<`H>, string_t<`H>) * `r info);

  // try to merge a list of interfaces
  // use linfo (if long enough) for the error messages
  extern struct I * merge_list(list_t<i_t,`H> li, list_t<stringptr_t<`H,`H>,`H> linfo);
  // same but ask for the interfaces one by one
  extern struct I * get_and_merge_list(i_t (@get)(`a), list_t<`a,`H> la, list_t<stringptr_t<`H,`H>,`H> linfo);

  // pretty print to a text file 
  extern void print(i_t, FILE @);

  // load from a text file
  extern i_t parse(FILE @);

  // binary save to a file
  extern void save(i_t, FILE @);

  // load from a text file
  extern i_t load(FILE @);
}

typedef Interface::i_t interface_t;

#endif /* _INTERFACE_H_ */
