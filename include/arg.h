/* This file is part of the Cyclone Library.
   Copyright (C) 2000-2001 Dan Grossman, Greg Morrisett, AT&T

   This library is free software; you can redistribute it and/or it
   under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of
   the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place, Suite
   330, Boston, MA 02111-1307 USA. */

// Originally ported from Objective Caml:

/***********************************************************************)
(*                                                                     *)
(*                           Objective Caml                            *)
(*                                                                     *)
(*             Damien Doligez, projet Para, INRIA Rocquencourt         *)
(*                                                                     *)
(*  Copyright 1996 Institut National de Recherche en Informatique et   *)
(*  en Automatique.  All rights reserved.  This file is distributed    *)
(*  under the terms of the GNU Library General Public License.         *)
(*                                                                     *)
(***********************************************************************/

#ifndef _ARG_H_
#define _ARG_H_

#include <list.h>
#include <core.h>

namespace Arg {
extern datatype exn {extern Error, extern Bad(string_t)};

extern datatype Spec {
  Unit_spec(void (@f)()),        // Call f with unit argument
  Flag_spec(void (@f)(string_t)),  // Call f with flag argument
  FlagString_spec(void (@f)(string_t,string_t)),
                                    // Call f with flag & string arguments
  Set_spec(bool@),                  // Set the reference to true
  Clear_spec(bool@),                // Set the reference to false
  String_spec(void (@f)(string_t)),// Call f with a string argument
  Int_spec(void (@f)(int)),      // Call f with an int argument
  Rest_spec(void (@f)(string_t))   // Stop interpreting keywords and call the
                                    // function with each remaining argument
};

typedef datatype `r Spec spec_t<`r>;

typedef List::list_t<$(string_t,bool,string_t,spec_t<`r1>,string_t)@`r1,`r2>
speclist_t<`r1,`r2>;

extern void usage(speclist_t<`r1,`r2>,string_t);

extern int current;

extern void parse(speclist_t<`r1,`r2> specs, 
                  void anonfun(string_t<`H>), 
                  string_t errmsg, string_t<`H> ?`H args);

}

#endif
