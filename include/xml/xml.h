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

#ifndef _XML_H_
#define _XML_H_

#ifdef _XML_CYC
#define EXTERN_DEFINITION
#else
#define EXTERN_DEFINITION extern
#endif

#include <core.h>
#include <list.h>

namespace Xml {

using Core;
using List;

typedef string_t chardata;
typedef string_t reference;
typedef string_t comment;
typedef string_t name;

EXTERN_DEFINITION
datatype attvalue {
  Attvalue1(string_t), // should be delimited by single quotes
  Attvalue2(string_t)  // should be delimited by double quotes
};
typedef datatype attvalue @attvalue_t;

typedef $(name,attvalue_t)@ attribute_t;

typedef $(name,string_t)@ pi_t;

extern datatype content;
typedef datatype content @content_t;

EXTERN_DEFINITION
datatype element {
  StartEnd(name,list_t<attribute_t>, // Element with start and end tags
           list_t<content_t>);
  Empty(name,list_t<attribute_t>);   // Element with no content
};
typedef datatype element @element_t;

EXTERN_DEFINITION
datatype content {
  Element(element_t);   // A nested element
  Chardata(chardata);   // Character data
  Reference(reference); // Character reference: &foo; or &#0;
  Cdsect(chardata);     // CDATA section: <![CDATA[ ]]>
  Pi(pi_t);             // Processing instruction: <?foo ?>
  Comment(comment);     // Comment: <!-- -->
};

// various routines to help document processing */

EXTERN_DEFINITION 
datatype exn { EXTERN_DEFINITION procXMLdocFailed(string_t) };
  /** [procXMLdocFailed] is thrown if one of the following routines
      fails to process the document as specified. **/

extern attvalue_t getAttribute(list_t<attribute_t> atrs, name nm);
  /** [getAttribute(a,n)] returns the attribute value from [a] that
      whose name matches [n]; throws [procXMLdocFailed]
      if not found. **/

extern attvalue_t matchAttribute(attribute_t atr, name nm);
  /** [matchAttribute(atr,nm)] returns the value associated with
      attribute [atr] if it has name [nm]; throws [procXMLdocFailed]
      if not. **/

extern list_t<attvalue_t *,`r>
getAttributes(region_t<`r> r,
              list_t<attribute_t> atrs,list_t<name @> names);
  /** [getAttributes(r,a,n)] returns the attribute values from [a] that
      match the names in [n], returned in the order the names appear in
      [n].  If a name is missing, a NULL is inserted instead.  Order
      does not matter.  **/

extern list_t<attvalue_t,`r>
matchAttributes(region_t<`r>,list_t<attribute_t> atrs,list_t<name @> names);
  /** [matchAttributes(r,a,n)] returns the attribute values from [a] that
      match the names in [n].  The lists must match up exactly
      (i.e. order matters).  If order shouldn't matter, you can sort
      both lists first.  **/

extern $(list_t<content_t>,list_t<attribute_t>)
getElemData(content_t doc, string_t tagname);
  /** [getElemData(doc,tag)] checks that the document [doc] is a
      single element with tag [tag], and returns the contents and
      attributes of that element.  On error, a [parseConfigFailed]
      exception is thrown.  **/

extern list_t<content_t,`r> 
clearWhitespaceContent(list_t<content_t,`r> docs);
  /** [clearWhitespaceContent(docs)] removes all occurences of
      Chardata variants in docs that have only whitespace
      content. **/

}

#endif
