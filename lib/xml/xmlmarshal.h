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

#ifndef _XMLMARSHAL_H_
#define _XMLMARSHAL_H_

#include <typerep.h>
#include "xml.h"

namespace XmlMarshal {

extern Xml::content_t to_xml(tunion Typerep::Typestruct rep, `a::A@ val);
extern `a::A@ from_xml(tunion Typerep::Typestruct rep, Xml::content_t);

}
#endif // _XMLMARSHAL_H_
