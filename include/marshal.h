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

/* This file uses the low-level representation of Cyclone data
   to implement generic marshallers. 
   Currently uses unsafe_cast */

#ifndef _MARSHAL_H_
#define _MARSHAL_H_

#include <typerep.h>

namespace Marshal {
  /** In all of the following functions, the representation [rep] passed 
      to a function must be a representation of the type `a at 
      which the function is instantiated.  You can use the [__gen(t)]
      primitive to construct representations for types automatically.
      (This gets trickier when type definitions cross module 
      boundaries).
  */

extern void fprint_type(tunion Typerep::Typestruct rep, FILE@ fp,`a::A@ val);
  /** [fprint_type(rep,fp,val)] takes a representation of type [`a], 
      an open file pointer [fp], and [val], a pointer to [`a], and 
      (ugly-)prints a text representation of [*val] to [fp].
  */
extern void print_type(tunion Typerep::Typestruct rep, `a::A@ val);
  /** [print_type(rep,val)] is [fprint_type] directed to [stdout]. 
   */
extern void write_type(tunion Typerep::Typestruct rep, FILE@ fp, `a::A@ val);
  /** [write_type(rep,fp,val)] writes a binary representation out to [fp].
      ([fp] should be in binary mode on operating systems where there is a
      difference).
  */
extern `a::A@`r rread_type(region_t<`r> r,tunion Typerep::Typestruct rep, FILE@ fp);
  /** [rread_type(r,rep,fp)] reads a value encoded in the binary format of 
      [write_type] from [fp], allocating into region [`r], and returns 
      a pointer to the value. 
      Warning: Currently, xtunions values cannot be safely read.
  */
extern `a::A@ read_type(tunion Typerep::Typestruct rep, FILE@ fp);
  /** [read_type(rep,fp)] is [rread_type(heap_region,rep,fp)].
   */
extern `a::A@`r rcopy_type(region_t<`r>,tunion Typerep::Typestruct rep, `a@ val);
  /** [rcopy_type(r,rep,val)] makes an exact structural (deep) copy of 
      [*val], allocating into region [`r].
  */
extern `a::A@ copy_type(tunion Typerep::Typestruct rep, `a@ val);
  /** [copy_type(rep,val)] is [rcopy_type(rep,heap_region,val)].
   */
extern bool leq_type(tunion Typerep::Typestruct rep, `a::A@ x, `a@ y);
  /** [leq_val(rep,x,y)] compares two values of the same type.
   */
extern bool eq_type(tunion Typerep::Typestruct rep, `a::A@ x, `a@ y);
  /** [eq_type(rep,x,y)] tests structural equality of [x] and [y].
      Attempts to handle cycles and sharing efficiently/safely.
  */
extern int cmp_type(tunion Typerep::Typestruct rep, `a::A@ x, `a@ y);
  /** [cmp_type(rep,x,y)] compares [x] and [y], returning 0 if [x == y], 
      1 if [x < y] and -1 if [x > y].  Can be used as a comparison function
      for dictionary, set modules. 
  */

  // Further ideas:
  // hash_type(rep,x)
  // $(data,ptr,padding) size_type(rep,x)
  // doc_t pp_type

}

#endif // _MARSHAL_H_
