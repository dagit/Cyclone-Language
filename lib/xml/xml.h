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

typedef string chardata;
typedef string reference;
typedef string comment;
typedef string name;

EXTERN_DEFINITION
enum attvalue {
  Attvalue1(string), // should be delimited by single quotes
  Attvalue2(string)  // should be delimited by double quotes
};
typedef enum attvalue attvalue_t;

typedef $(name,attvalue_t)@ attribute_t;

typedef $(name,string)@ pi_t;

extern enum content;
typedef enum content content_t;

EXTERN_DEFINITION
enum element {
  StartEnd(name,list_t<attribute_t>, // Element with start and end tags
           list_t<content_t>);
  Empty(name,list_t<attribute_t>);   // Element with no content
};
typedef enum element element_t;

EXTERN_DEFINITION
enum content {
  Element(element_t);   // A nested element
  Chardata(chardata);   // Character data
  Reference(reference); // Character reference: &foo; or &#0;
  Cdsect(chardata);     // CDATA section: <![CDATA[ ]]>
  Pi(pi_t);             // Processing instruction: <?foo ?>
  Comment(comment);     // Comment: <!-- -->
};

}

#endif
