/* This file is part of the Cyclone Library.
   Copyright (C) 2001 Greg Morrisett, AT&T

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

/* This file defines the low-level representation of Cyclone data
   for implementing generic marshallers. */

#ifndef _TYPEREP_H_
#define _TYPEREP_H_

#include <core.h>
#include <set.h>
using Core;

// A cute hack to avoid defining the abstract syntax twice.
#ifdef TYPEREP_CYC
#define EXTERN_TYPEREP
#else
#define EXTERN_TYPEREP extern
#endif

#define gen(t) __gen(t)

namespace Typerep {
typedef stringptr_t<`H,`H> name_t;

EXTERN_TYPEREP tunion Typestruct {
  /** [Var(name) indicated a type parameter variable [name] **/
  Var(name_t);
  /** [Int(sz)] indicates integer types having [sz] bits **/
  Int(unsigned int);
  /** [Float] indicates a float type **/
  Float;
  /** [Double] indicates a float type **/
  Double;
  /** [ThinPtr(sz,typ)] a pointer to buffer of [typ] values of length [sz] **/
  ThinPtr(unsigned int,tunion Typestruct);
  /** [FatPtr(typ)] a fat pointer to a buffer of [typ] values **/
  FatPtr(tunion Typestruct);
  /** [Tuple(szb,(ofs_1,typ_1)::...)] is a buffer of values, one per
      element in the array.  The value at index i has type [typ_i] and
      is at offset [ofs_i].  The total size is [szb] bytes. 
  **/
  Tuple(unsigned int, $(unsigned int,tunion Typestruct)@?);
  /** [TUnion(fields)] is a tunion type, where each element of
      the array [fields] is a pair [(tag,ts)] of a tag value [tag]
      and a Typestruct [ts] representing the type of the tagged data.
      Tag-only fields are not included in the array (since
      we can just check the tunion pointer to see if it's a tag).
  **/
  TUnion($(unsigned int, tunion Typestruct)@?);
  /** [XTUnion(fields)] is an xtunion type, where each element of
      the array [fields] is a pair [(tag,ts)] of a tag value [tag]
      and a Typestruct [ts] representing the type of the tagged data.
      Tag-only fields are not included in the array (since
      we can just check the xtunion pointer to see if it's a tag).
  **/
  XTUnion($(string_t<`H>, tunion Typestruct)@?);
  /** [Union(fields)] is a union type, where each element of
      the array [fields] is a Typestruct represnting the data
      for a possible case of the union.
  **/
  Union(tunion Typestruct@?);
  /** [Forall(vars, ts)] is a polymorphic type wrapper.
      The string array [vars] lists the variables bound by this forall in [ts].
  **/
  Forall(name_t?,tunion Typestruct@);
  /** [App(ts,ts_params)] describes an instantiation of a polymorphic
      type.  Typestruct [ts] must be a [Forall(vars,ts2)].  The [ts_params]
      represent the instantiations of the type variables bound inside [ts2].
      The correspondence between variables and type parameters is by position
      in the arrays; [ts_param[i]] binds to [vars[i]] (and the arrays must 
      have the same length)
   **/
  App(tunion Typestruct, tunion Typestruct?);
};
  
  typedef tunion `r Typestruct typestruct_t<`r>;
  
  extern void print_typestruct(typestruct_t rep);
  extern void xmlize_typestruct(typestruct_t rep);
  extern typestruct_t<`H> normalize(typestruct_t<`H> ts);
  
  // utilities for addressing
  typedef Set::set_t<unsigned int> addr_set_t;

  extern addr_set_t empty_addr_set();
  extern $(bool,addr_set_t)@ member_insert(addr_set_t, unsigned int);

  extern typestruct_t<`r>
  get_unionbranch(unsigned int tag,
		  $(unsigned int,typestruct_t<`r>)@ ? l);
  extern typestruct_t<`r>
  get_xtunionbranch(string_t<`H> tag,
		    $(string_t<`H>,typestruct_t<`r>)@ ? l);
  unsigned int size_type(typestruct_t rep);

}

#endif
