/* Trevor Jim, 2001 */

#ifndef _XMLPARSE_H
#define _XMLPARSE_H

#include "xml.h"
#include <lexing.h>
#include <list.h>
#include <core.h>
#include <stdio.h>

namespace XmlParse {
using Lexing;
using List;
using Core;
using Stdio;
using Xml;

extern list_t<content_t> parse_file(FILE @f);
extern opt_t<Lexbuf<Function_lexbuf_state<FILE@>>> lbuf;
extern void error(string_t msg);

extern
tunion context {
  InTag,
  Normal
};

extern void setContext(tunion context c);

extern tunion context getContext();

}

#endif
