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
#include "xmlparse.h"
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
static list_t<content_t> parse_result = NULL;
void error(string_t<`H> msg) {
  throw new Core::Failure(msg);
}

datatype context {
  InTag,
  Normal
};

static datatype context current = Normal;

void setContext(datatype `H context c) {
  current = c;
}

datatype context getContext() {
  return current;
} 


}

using XmlParse;

%}

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
%union{
  String_tok(string_t<`H>);
  Pi_tok($(Xml::name,string_t<`H>)@`H);
  Element_tok(Xml::element_t);
  Content_tok(List::list_t<Xml::content_t,`H>);
  Attribute_tok(Xml::attribute_t);
  Attributes_tok(List::list_t<Xml::attribute_t,`H>);
  STag_tok($(string_t<`H>,List::list_t<Xml::attribute_t,`H>)@`H);
  /*EmptyElemTag_tok($(string_t,List::list_t<Xml::attribute_t>)@);*/
}

// Tags for productions
%type <string_t<`H>> ATTVALUE1
%type <string_t<`H>> ATTVALUE2
%type <string_t<`H>> CDSECT
%type <string_t<`H>> CHARDATA
%type <string_t<`H>> COMMENT
%type <string_t<`H>> NAME
%type <string_t<`H>> OPEN
%type <string_t<`H>> opn
%type <string_t<`H>> OPENSLASH
%type <string_t<`H>> opnslash
%type <$(Xml::name,string_t<`H>)@`H>     PI
%type <string_t<`H>> REFERENCE
%type <Xml::element_t> element
%type <List::list_t<Xml::content_t,`H>> content
%type <List::list_t<Xml::content_t,`H>> content0
%type <Xml::attribute_t> attribute
%type <List::list_t<Xml::attribute_t,`H>> attributes
%type <$(string_t<`H>,List::list_t<Xml::attribute_t,`H>)@`H> sTag
%type <string_t<`H>> eTag
%type <$(string_t<`H>,List::list_t<Xml::attribute_t,`H>)@`H> emptyElemTag

%start content0

%%

content0: content     { $$ = $!1; XmlParse::parse_result = $1; }

element:
  emptyElemTag      { let $(n,a) = *($1); $$ = ^$(new Empty(n,a)); }
| sTag content eTag { let $(sn,a) = *($1);
                      let en = $3;
                      if (strcmp(sn,en) == 0)
                        $$ = ^$(new StartEnd(sn,a,$2));
                      else error("tag mismatch");
                    }

sTag:
  opn attributes cls { $$ = ^$(new $($1,$2)); }

opn:
  OPEN { setContext(InTag); $$ = $!1; }

attributes:
  /* empty */          { $$ = ^$(NULL); }
| attribute attributes { $$ = ^$(new List($1,$2)); }

attribute:
  NAME EQ ATTVALUE1 { $$ = ^$(new $($1,new Attvalue1($3))); }
| NAME EQ ATTVALUE2 { $$ = ^$(new $($1,new Attvalue2($3))); }

cls:
  CLOSE { setContext(Normal); }

content:
  /* empty */       { $$ = ^$(NULL); }
| element content   { $$ = ^$(new List(new Element($1),$2)); }
| CHARDATA content  { $$ = ^$(new List(new Chardata($1),$2)); }
| REFERENCE content { $$ = ^$(new List(new Reference($1),$2)); }
| CDSECT content    { $$ = ^$(new List(new Cdsect($1),$2)); }
| PI content        { $$ = ^$(new List(new Pi($1),$2)); }
| COMMENT content   { $$ = ^$(new List(new Comment($1),$2)); }

eTag:
  opnslash cls { $$ = $!1; }

opnslash:
  OPENSLASH { setContext(InTag); $$ = $!1; }

emptyElemTag:
  opn attributes slashcls { $$ = ^$(new $($1,$2)); }

slashcls:
  SLASHCLOSE { setContext(Normal); }

%%

void yyerror(string_t s) { return; } 

namespace XmlParse{
  list_t<content_t> parse_file(FILE @`H f) {
    parse_result = NULL;
    lbuf = new Core::Opt(from_file(f));
    XmlScan::init();
    yyparse();
    return parse_result;
  }
}

