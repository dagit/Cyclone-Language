/* This file is part of the Cyclone Library.
   Copyright (C) 2001 AT&T

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

#ifndef _XMLDUMP_H
#define _XMLDUMP_H

#include <xml/xml.h>
#include <list.h>
#include <stdio.h>

namespace XmlDump {

using List;
using Xml;

void dumpAttvalue(FILE @fd,attvalue_t av);
void dumpAttribute(FILE @fd,attribute_t a);
void dumpAttributes(FILE @fd,list_t<attribute_t> al);
void dumpElement(FILE @fd,element_t e);
void dumpContent(FILE @fd,content_t c);
void dumpContents(FILE @fd,list_t<content_t> cl);

}
#endif
