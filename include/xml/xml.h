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

typedef string_t<`r> chardata<`r>;
typedef string_t<`r> reference<`r>;
typedef string_t<`r> comment<`r>;
typedef string_t<`r> name<`r>;

EXTERN_DEFINITION
datatype attvalue<`r> {
  Attvalue1(string_t<`r>), // should be delimited by single quotes
  Attvalue2(string_t<`r>)  // should be delimited by double quotes
};
typedef datatype attvalue<`r> @`r attvalue_t<`r>;

typedef $(name<`r>,attvalue_t<`r>)@`r attribute_t<`r>;

typedef $(name<`r>,string_t<`r>)@`r pi_t<`r>;

extern datatype content<`r::R>; // forward definition
typedef datatype content<`r::R> @`r content_t<`r>;

EXTERN_DEFINITION
datatype element<`r::R> {
  StartEnd(name<`r>,list_t<attribute_t<`r>,`r>, // Element with start and end tags
           list_t<content_t<`r>,`r>);
  Empty(name<`r>,list_t<attribute_t<`r>,`r>);   // Element with no content
};
typedef datatype element<`r> @`r element_t<`r>;

EXTERN_DEFINITION
datatype content<`r> {
  Element(element_t<`r>);   // A nested element
  Chardata(chardata<`r>);   // Character data
  Reference(reference<`r>); // Character reference: &foo; or &#0;
  Cdsect(chardata<`r>);     // CDATA section: <![CDATA[ ]]>
  Pi(pi_t<`r>);             // Processing instruction: <?foo ?>
  Comment(comment<`r>);     // Comment: <!-- -->
};

// various routines to help document processing */

EXTERN_DEFINITION 
datatype exn { EXTERN_DEFINITION procXMLdocFailed(string_t) };
  /** [procXMLdocFailed] is thrown if one of the following routines
      fails to process the document as specified. **/

extern attvalue_t<`r> getAttribute(list_t<attribute_t<`r>> atrs, name nm);
  /** [getAttribute(a,n)] returns the attribute value from [a] that
      whose name matches [n]; throws [procXMLdocFailed]
      if not found. **/

extern attvalue_t<`r> matchAttribute(attribute_t<`r> atr, name nm);
  /** [matchAttribute(atr,nm)] returns the value associated with
      attribute [atr] if it has name [nm]; throws [procXMLdocFailed]
      if not. **/

extern list_t<attvalue_t<`s> *,`r>
getAttributes(region_t<`r> r,
              list_t<attribute_t<`s>> atrs,list_t<name @> names);
  /** [getAttributes(r,a,n)] returns the attribute values from [a] that
      match the names in [n], returned in the order the names appear in
      [n].  If a name is missing, a NULL is inserted instead.  Order
      does not matter.  **/

extern list_t<attvalue_t<`s>,`r>
matchAttributes(region_t<`r>,list_t<attribute_t<`s>> atrs,list_t<name @> names);
  /** [matchAttributes(r,a,n)] returns the attribute values from [a] that
      match the names in [n].  The lists must match up exactly
      (i.e., order matters).  If order shouldn't matter, you can sort
      both lists first.  **/

extern $(list_t<content_t<`r>,`r>,list_t<attribute_t<`r>,`r>)
getElemData(content_t<`r> doc, string_t tagname);
  /** [getElemData(doc,tag)] checks that the document [doc] is a
      single element with tag [tag], and returns the contents and
      attributes of that element.  On error, a [parseConfigFailed]
      exception is thrown.  **/

extern list_t<content_t<`r>,`r> 
clearWhitespaceContent(list_t<content_t<`r>,`r> docs);
  /** [clearWhitespaceContent(docs)] removes all occurences of
      Chardata variants in docs that have only whitespace
      content. **/

}

#endif
