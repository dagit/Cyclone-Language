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

%{
#include <xml/xmlparse.h>
#include "xmlscan.h"
#include <string.h>
#include <stdio.h>

using Core;
using List;
using Xml;              // XML abstract syntax
using Lexing;


// global state (we're not re-entrant)
namespace XmlParse {
Core::opt_t<Lexbuf<Function_lexbuf_state<FILE@>>> lbuf = NULL;
void error(string_t<`H> msg) {
  throw new Core::Failure(msg);
}

static enum context current = Normal;

void setContext(enum context c) {
  current = c;
}

enum context getContext() {
  return current;
} 

}

using XmlParse;
#define RNEW rnew(yyoutregion)

%}
%pure_parser

// Tokens
%token ATTVALUE1
%token ATTVALUE2
%token CDSECT
%token CHARDATA
%token CLOSE
%token COMMENT
%token ENDOFFILE
%token EQ
%token NAME
%token OPEN
%token OPENSLASH
%token PI
%token REFERENCE
%token SLASHCLOSE

// Union of types of productions
%union <`yy> {
  String_tok(string_t<`yy>);
  Pi_tok($(Xml::name<`yy>,string_t<`yy>)@`yy);
  Element_tok(Xml::element_t<`yy>);
  Content_tok(List::list_t<Xml::content_t<`yy>,`yy>);
  Attribute_tok(Xml::attribute_t<`yy>);
  Attributes_tok(List::list_t<Xml::attribute_t<`yy>,`yy>);
  STag_tok($(string_t<`yy>,List::list_t<Xml::attribute_t<`yy>,`yy>)@`yy);
}

// Tags for productions
%type <string_t<`yy>> ATTVALUE1
%type <string_t<`yy>> ATTVALUE2
%type <string_t<`yy>> CDSECT
%type <string_t<`yy>> CHARDATA
%type <string_t<`yy>> COMMENT
%type <string_t<`yy>> NAME
%type <string_t<`yy>> OPEN
%type <string_t<`yy>> opn
%type <string_t<`yy>> OPENSLASH
%type <string_t<`yy>> opnslash
%type <$(Xml::name<`yy>,string_t<`yy>)@`yy>     PI
%type <string_t<`yy>> REFERENCE
%type <Xml::element_t<`yy>> element
%type <List::list_t<Xml::content_t<`yy>,`yy>> content
%type <List::list_t<Xml::content_t<`yy>,`yy>> content0
%type <Xml::attribute_t<`yy>> attribute
%type <List::list_t<Xml::attribute_t<`yy>,`yy>> attributes
%type <$(string_t<`yy>,List::list_t<Xml::attribute_t<`yy>,`yy>)@`yy> sTag
%type <string_t<`yy>> eTag
%type <$(string_t<`yy>,List::list_t<Xml::attribute_t<`yy>,`yy>)@`yy> emptyElemTag

%start content0

%%

content0: content     { $$ = $!1;
                        *parse_result = $1;
                      }

element:
  emptyElemTag      { let $(n,a) = *($1);
                      $$ = ^$(RNEW Empty(n,a));
                    }
| sTag content eTag { let $(sn,a) = *($1);
                      let en = $3;
                      if (strcmp(sn,en) == 0)
                        $$ = ^$(RNEW StartEnd(sn,a,$2));
                      else error("tag mismatch");
                    }

sTag:
  opn attributes cls { $$ = ^$(RNEW $($1,$2)); }

opn:
  OPEN { setContext(InTag); $$ = $!1; }

attributes:
  /* empty */          { $$ = ^$(NULL); }
| attribute attributes { $$ = ^$(RNEW List($1,$2)); }

attribute:
  NAME EQ ATTVALUE1 { $$ = ^$(RNEW $($1,RNEW Attvalue1($3))); }
| NAME EQ ATTVALUE2 { $$ = ^$(RNEW $($1,RNEW Attvalue2($3))); }

cls:
  CLOSE { setContext(Normal); }

content:
  /* empty */       { $$ = ^$(NULL); }
| element content   { $$ = ^$(RNEW List(RNEW Element($1),$2)); }
| CHARDATA content  { $$ = ^$(RNEW List(RNEW Chardata($1),$2)); }
| REFERENCE content { $$ = ^$(RNEW List(RNEW Reference($1),$2)); }
| CDSECT content    { $$ = ^$(RNEW List(RNEW Cdsect($1),$2)); }
| PI content        { $$ = ^$(RNEW List(RNEW Pi($1),$2)); }
| COMMENT content   { $$ = ^$(RNEW List(RNEW Comment($1),$2)); }

eTag:
  opnslash cls { $$ = $!1; }

opnslash:
  OPENSLASH { setContext(InTag); $$ = $!1; }

emptyElemTag:
  opn attributes slashcls { $$ = ^$(RNEW $($1,$2)); }

slashcls:
  SLASHCLOSE { setContext(Normal); }

%%

void yyerror(string_t s,int state,int token) { return; } 

namespace XmlParse{
  list_t<content_t<`rr>,`rr> rparse_file(region_t<`rr> rr,FILE @`H f) {
    list_t<content_t<`rr>,`rr> parse_result = NULL;
    lbuf = new Core::Opt(from_file(f));
    XmlScan::init();
    yyparse(rr,&parse_result);
    return parse_result;
  }
  list_t<content_t> parse_file(FILE @`H f) {
    return rparse_file(Core::heap_region,f);
  }
}
