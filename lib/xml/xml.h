/* Trevor Jim, 2001 */

#ifndef _XML_H_
#define _XML_H_

#ifdef _XML_CYC
#define EXTERN_DEFINITION
#else
#define EXTERN_DEFINITION extern
#endif

#include "core.h"
#include "list.h"

namespace Xml {

using Core;
using List;

typedef string_t chardata;
typedef string_t reference;
typedef string_t comment;
typedef string_t name;

EXTERN_DEFINITION
tunion attvalue {
  Attvalue1(string_t), // should be delimited by single quotes
  Attvalue2(string_t)  // should be delimited by double quotes
};
typedef tunion attvalue attvalue_t;

typedef $(name,attvalue_t)@ attribute_t;

typedef $(name,string_t)@ pi_t;

extern tunion content;
typedef tunion content content_t;

EXTERN_DEFINITION
tunion element {
  StartEnd(name,list_t<attribute_t>, // Element with start and end tags
           list_t<content_t>);
  Empty(name,list_t<attribute_t>);   // Element with no content
};
typedef tunion element element_t;

EXTERN_DEFINITION
tunion content {
  Element(element_t);   // A nested element
  Chardata(chardata);   // Character data
  Reference(reference); // Character reference: &foo; or &#0;
  Cdsect(chardata);     // CDATA section: <![CDATA[ ]]>
  Pi(pi_t);             // Processing instruction: <?foo ?>
  Comment(comment);     // Comment: <!-- -->
};

}

#endif
