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

#ifndef _XMLPARSE_H
#define _XMLPARSE_H

#include "xml.h"
#include <lexing.h>
#include <list.h>
#include <core.h>
#include <cstdio.h>

namespace XmlParse {
using Lexing;
using List;
using Core;
using std;
using Xml;

extern list_t<content_t> parse_file(FILE @`H f);
extern opt_t<Lexbuf<Function_lexbuf_state<FILE@>>> lbuf;
extern void error(string_t<`H> msg);

extern
tunion context {
  InTag,
  Normal
};

extern void setContext(tunion context c);

extern tunion context getContext();

}

#endif
