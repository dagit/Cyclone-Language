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
// A cute hack to avoid defining the abstract syntax twice.
#ifdef TYPEREP_CYC
#define EXTERN_TYPEREP
#else
#define EXTERN_TYPEREP extern
#endif

#define gen(t) __gen(t)

namespace Typerep {
using Core;

typedef unsigned int var_t;

EXTERN_TYPEREP datatype Typestruct {
  /** [Var(name)] indicates a type parameter variable [name] **/
  //Var(var_t);
  /** [Int(sn, sz)] indicates integer types with sign [sn] having [sz] bits **/
  Int(bool, unsigned int);
  /** [Float] indicates a float type **/
  Float;
  /** [Double] indicates a double type **/
  Double;
  /** [ThinPtr(sz,typ)] a pointer to buffer of [typ] values of length [sz] **/
  ThinPtr(unsigned int,datatype Typestruct);
  /** [FatPtr(typ)] a fat pointer to a buffer of [typ] values **/
  FatPtr(datatype Typestruct);
  /** [Struct(name,szb,(ofs_1,nm_1,typ_1)::...)] represents a struct type 
      named [*name] ([name==NULL] if the struct is anonymous).  
      The data is a buffer of values, one per
      element in the array.  The value at index i has name [nm_i], type [typ_i] and
      is at offset [ofs_i].  The total size is [szb] bytes. 
  **/
  Struct(string_t<`H>*, unsigned int, $(unsigned int, string_t<`H>, datatype Typestruct)@?);
  /** [Tuple(szb,(ofs_1,typ_1)::...)] is a buffer of values, one per
      element in the array.  The value at index i has type [typ_i] and
      is at offset [ofs_i].  The total size is [szb] bytes. 
  **/
  Tuple(unsigned int, $(unsigned int,datatype Typestruct)@?);
  /** [TUnion(name,tagonly,fields)] is a tunion type [name], where each element of
      the array [tagonly] is a pair [(tag,tagname)] of a tag-only tag value [tag]
      and its name [tagname], and 
      the array [fields] is a triple [(tag,tagname,ts)] of a tag value [tag], 
      its name [tagname], 
      and a Typestruct [ts] representing the type of the tagged data.
      Tag-only fields are not included in the array (since
      we can just check the tunion pointer to see if it's tag-only).
  **/
  TUnion(string_t<`H>,$(unsigned int,string_t<`H>)@?,
			$(unsigned int, string_t<`H>,datatype Typestruct)@?);
  /** [TUnionField(tname,fname,ts)] is a tunionfield type [tname.fname] where
      [ts] is a Tuple describing the tunionfield data.  (the same tuple as
      in the tunion or xtunion typestruct. 
  **/
  // should this have tag? or same data as tunion?
  TUnionField(string_t<`H>,string_t<`H>,unsigned int, $(unsigned int,datatype Typestruct)@?);
  /** [name, XTUnion(fields)] is an xtunion type [name], where each element of
      the array [fields] is a pair [(tag,ts)] of a tag value [tag]
      and a Typestruct [ts] representing the type of the tagged data.
      Tag-only fields are not included in the array (since
      we can just check the xtunion pointer to see if it's tag-only).
  **/
  XTUnion(string_t<`H>,$(string_t<`H>, datatype Typestruct)@?);
  /** [Union(name,szb,fields)] is a union type [*name] ([name == NULL] if 
      anonymous), of byte size [szb], where each element of
      the array [fields] is a pair [(name,ts)] where [name] is the 
      union field name and ts is a Typestruct represnting the data
      for a possible case of the union.
  **/
  Union(string_t<`H>*,int,$(string_t<`H>,datatype Typestruct)@?);
  /** [Enum(name,szb,l)] is an enum of [szb] size whose value-tag bindings 
      are the entries [(value,tag)] of [l].  [name] is an optional name 
      for the enum.
   **/
  Enum(string_t<`H>*,int,$(unsigned int,string_t)@?);
  /** [Forall(vars, ts)] is a polymorphic type wrapper.
      The string array [vars] lists the variables bound by this forall in [ts].
  **/
  //  Forall(var_t?,tunion Typestruct@);
  /** [App(ts,ts_params)] describes an instantiation of a polymorphic
      type.  Typestruct [ts] must be a [Forall(vars,ts2)].  The [ts_params]
      represent the instantiations of the type variables bound inside [ts2].
      The correspondence between variables and type parameters is by position
      in the arrays; [ts_param[i]] binds to [vars[i]] (and the arrays must 
      have the same length)
   **/
  
  //  App(tunion Typestruct, tunion Typestruct?);
};
  
  typedef datatype `r Typestruct typestruct_t<`r>;
  
  extern void print_typestruct(typestruct_t rep);
  extern void xmlize_typestruct(typestruct_t rep);
  //  extern typestruct_t<`H> normalize(typestruct_t<`H> ts);
  
  extern $(int,typestruct_t<`r>)
  get_unionbranch(unsigned int tag,
		  $(unsigned int,string_t<`H>,typestruct_t<`r>)@ ? l);
  extern string_t<`H>
  get_tagname(unsigned int tag,
		  $(unsigned int,string_t<`H>)@ ? l);
  extern string_t<`H>
  get_tagname2(unsigned int tag,
		  $(unsigned int,string_t<`H>,typestruct_t<`r>)@ ? l);
  extern $(int,typestruct_t<`r>)
  get_xtunionbranch(string_t<`H> tag,
		    $(string_t<`H>,typestruct_t<`r>)@ ? l);
  unsigned int size_type(typestruct_t rep);

}

#endif
