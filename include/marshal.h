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


extern void fprint_type(tunion Typerep::Typestruct rep, FILE@ fp,`a::A@ val);
extern void print_type(tunion Typerep::Typestruct rep, `a::A@ val);

extern void write_type(tunion Typerep::Typestruct rep, FILE@ fp, `a::A@ val);
extern `a::A@`r rread_type(region_t<`r> r,tunion Typerep::Typestruct rep, FILE@ fp);
extern `a::A@ read_type(tunion Typerep::Typestruct rep, FILE@ fp);
extern `a::A@`r rcopy_type(region_t<`r>,tunion Typerep::Typestruct rep, `a@ val);
extern `a::A@ copy_type(tunion Typerep::Typestruct rep, `a@ val);
extern bool leq_type(tunion Typerep::Typestruct rep, `a::A@ x, `a@ y);
extern bool eq_type(tunion Typerep::Typestruct rep, `a::A@ x, `a@ y);
extern int cmp_type(tunion Typerep::Typestruct rep, `a::A@ x, `a@ y);

}

#endif // _MARSHAL_H_
