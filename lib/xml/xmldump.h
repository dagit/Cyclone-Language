/* Trevor Jim, 2001 */

#ifndef _XMLDUMP_H
#define _XMLDUMP_H

#include "xml.h"
#include "list.h"
#include "stdio.h"

namespace XmlDump {

using List;
using Xml;
using Stdio;

void dumpAttvalue(FILE @fd,attvalue_t av);
void dumpAttribute(FILE @fd,attribute_t a);
void dumpAttributes(FILE @fd,list_t<attribute_t> al);
void dumpElement(FILE @fd,element_t e);
void dumpContent(FILE @fd,content_t c);
void dumpContents(FILE @fd,list_t<content_t> cl);

}
#endif
