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

// A cute hack to avoid defining the abstract syntax twice.
#ifdef TYPEREP_CYC
#define EXTERN_TYPEREP
#else
#define EXTERN_TYPEREP extern
#endif

namespace Typerep {

EXTERN_TYPEREP tunion Typestruct {
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
      the array [fields] is a TunionField that contains
      data.  Tag-only fields are not included in the array (since
      we can just check the tunion pointer to see if it's a tag).
  **/
  TUnion($(unsigned int, tunion Typestruct)@?);
};

typedef tunion Typestruct typestruct_t;

extern void print_type(typestruct_t rep, `a@ val);
extern void print_typestruct(typestruct_t rep);

}

#endif
