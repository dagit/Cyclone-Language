/* Parser for the augmented BNF of RFC 2616, the HTTP specification */
/* Copyright (C) 2005 Greg Morrisett, AT&T.
   This file is part of the Cyclone project.

   Cyclone is free software; you can redistribute it
   and/or modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   Cyclone is distributed in the hope that it will be
   useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Cyclone; see the file COPYING. If not,
   write to the Free Software Foundation, Inc., 59 Temple Place -
   Suite 330, Boston, MA 02111-1307, USA. */

%{
#include <string.h>
#include <lexing.h>
#include <list.h>
#include <stdio.h>
using List;
#include "bnf.h"
#define YYLEX_PARAM_ARG Lexing::Lexbuf<`a>,union YYSTYPE @yylval_ptr
#define YYLEX_ARG yylex_buf,&yylval
#define YYPARSE_PARAM_ARG Lexing::Lexbuf<`a> yylex_buf

/* List of symbols which should/should not have whitespace/re */
List::list_t<const char ?@> nowslist = NULL;
List::list_t<const char ?@> wslist = NULL;
List::list_t<const char ?@> norelist = NULL;
List::list_t<const char ?@> relist = NULL;

// In lex.cyl
extern void warn(const char ? fmt, ... inject parg_t ap)
  __attribute__((format(printf,1,2)));

void yyerror(const char ?s, int state, int token) { 
  warn("parse error in state [%d] on token [%d]: %s\n",
       state,token,s);
}
static grammar_t parse_result = NULL;
List::list_t<const char ?@> textblobs = NULL;
static int after_definition = 0;
int ws_on = 0;
int re_on = 0;
static int ws_for_id(const char ?id) {
  let result = ws_on;
  for (let x = nowslist; x; x = x->tl) {
    if (strcmp(id,*x->hd) == 0) {
      result = 0; break;
    }
  }        
  return result;
}
static int re_for_id(const char ?id) {
  let result = re_on;
  return result;
}
%}
%token LPAREN     /* ( */
%token RPAREN     /* ) */
%token LBRACKET   /* [ */
%token RBRACKET   /* ] */
%token STAR_T     /* * */
%token HASH_T     /* # */
%token DEF        /* = or := */
%token DEF_WS     /* .= */
%token DEF_RE     /* @= */
%token DEF_WSRE   /* .@= */
%token BAR        /* | or / */
%token DOT        /* . */
%token MINUS_T    /* - */
%token AT         /* @ */
%token COMMA      /* , */
%token DOLLAR     /* $ */
%token RANGE
%token CHARSEQ
%token ID
%token NUM
%token STRING     /* double-quoted strings and <"> for double quote itself */
%token PROSE_T    /* text in between < and > */
%token SEMACTION  /* text in between { and } */
%token PRED       /* text in between @when( and ) */
%token RCOUNT_T   /* text in between @repeat( and ) */
%token PARAMS     /* text in between @( and ) */
%token DPARAMS    /* text in between $( and ) */
%token WS_ON
%token WS_OFF
%token RE_ON
%token RE_OFF
%union {
  Int_tok(unsigned int);
  String_tok(const char ?`H); /* THE `H IS CRUCIAL ELSE EXTRA CASES CROP UP FROM ID AND STRING!! */
  Range_tok($(unsigned int,unsigned int));
  Rule_tok(rule_t);
}
%type <unsigned int>                  NUM
%type <$(unsigned int,unsigned int)>  RANGE
%type <$(int,int)>                    def
%type <rule_t>                        CHARSEQ
%type <const char ?`H>                ID STRING PROSE_T SEMACTION PARAMS DPARAMS PRED RCOUNT_T
%type <grammar_t<`H>>                 definitions
%type <definition_t>                  definition
%type <$(const char ?`H,const char ?`H)> typestuff
%type <rule_t>                        rule rule1 rule2 rule3 rule0
%type <const char ?`H>                dd_init_opt
%start grammar
%%
grammar: definitions
{ parse_result = $1; }

definitions:
{ $$ = ^$(NULL); }
| definition definitions
{ after_definition = 1;
  $$ = ^$(new List($1,$2)); }
| directive definitions
{ $$ = $!2; }
| SEMACTION definitions
{ if (after_definition && !textblobs)
    textblobs = new List::List(new "",textblobs);
  textblobs = new List::List(new $1,textblobs);
  $$ = $!2; }

definition:
  ID typestuff def rule DOT
{
  let $(ws,re) = $3;
  ws = ws || ws_for_id($1);
  re = re || re_for_id($1);
  let $(ptype,rtype) = $2;
  $$ = ^$(new $($1,$4,Attr{.ws=ws,.re=re,.paramtype=ptype,.rettype=rtype}));
}
| PROSE_T DEF rule DOT
{
  $$ = ^$(new $($1,$3,Attr{.ws=0,.re=0,.paramtype=NULL,.rettype=NULL}));
}

def:
  DEF
{ $$ = ^$($(0,0)); }
| DEF_WS
{ $$ = ^$($(1,0)); }
| DEF_RE
{ $$ = ^$($(0,1)); }
| DEF_WSRE
{ $$ = ^$($(1,1)); }

typestuff:
/*empty*/
{ $$ = ^$($(NULL,NULL)); }
| PARAMS
{ $$ = ^$($($1,NULL)); }
| PARAMS DPARAMS
{ $$ = ^$($($1,$2)); }
| DPARAMS
{ $$ = ^$($(NULL,$1)); }

directive:
  WS_ON
{ ws_on = 1;}
| WS_OFF
{ ws_on = 0; }
| RE_ON
{ re_on = 1;}
| RE_OFF
{ re_on = 0; }

rule:
  rule0
{ $$ = $!1; }
| rule BAR rule0
{ $$ = ^$(ALT($1,$3));}

rule0:
  rule1
| PRED rule0
{ $$ = ^$(new arule($2->a,$1,$2->v,$2->css,$2->r)); } // NB: THROW AWAY PRED FROM $2

rule1:
  rule2
{ $$ = $!1; }
| rule1 rule2
{ $$ = ^$(SEQ($1,$2));}
| rule1 SEMACTION
{ $$ = ^$(new arule($2,$1->p,$1->v,$1->css,$1->r)); } // NB: THROW AWAY SEMACTION FROM $1

rule2:
  rule3
{ $$ = $!1; }
| rule2 DOLLAR ID
{ $$ = ^$(new arule($1->a,$1->p,$3,$1->css,$1->r)); } // NB: THROW AWAY VID FROM $1
| rule2 DOLLAR DOLLAR ID dd_init_opt // extra dollar indicates ID should be bound in rule2, with (optional) initializer.
{ let init_rule = LIT("");
  init_rule->a = strconcat_l(List::list(new $4, new "=", new ($5? $5 : "NULL"), new ";"));
  let new_r = new arule($1->a,$1->p,NULL,$1->css,$1->r); // NB: THROW AWAY VID FROM $1
  $$ = ^$(new arule(NULL,NULL,strconcat("$",$4) ,$1->css,new Seq(init_rule, new_r))); } 
// | rule2 DOLLAR DOLLAR ID // extra dollar indicates ID should be bound in rule2.
// { $$ = ^$(new arule($1->a,$1->p,strconcat("$",$4),$1->css,$1->r)); } // NB: THROW AWAY VID FROM $1
|     RCOUNT_T   rule3
{ $$ = ^$(RCOUNT($1,$2)); }
|     STAR_T     rule3
{ $$ = ^$(STAR(0,new Infinity,$2)); }
| NUM STAR_T     rule3
{ $$ = ^$(STAR($1,new Infinity,$3)); }
|     STAR_T NUM rule3
{ $$ = ^$(STAR(0,new Num($2),$3)); }
| NUM STAR_T NUM rule3
{ if ($1 > $3)
    warn("Error: cannot repeat at least %d times and at most %d times\n",$1,$3);
  $$ = ^$(STAR($1,new Num($3),$4)); }
|     NUM        rule3
{ $$ = ^$(STAR($1,new Num($1),$2)); }
|     HASH_T     rule3
{ $$ = ^$(HASH(0,new Infinity,$2)); }
| NUM HASH_T     rule3
{ $$ = ^$(HASH($1,new Infinity,$3)); }
|     HASH_T NUM rule3
{ $$ = ^$(HASH(0,new Num($2),$3)); }
| NUM HASH_T NUM rule3
{ if ($1 > $3)
    warn("Error: cannot repeat at least %d times and at most %d times\n",$1,$3);
  $$ = ^$(HASH($1,new Num($3),$4)); }

rule3:
  STRING
{ $$ = ^$(LIT($1)); }
| CHARSEQ
{ $$ = $!1; }
| RANGE
{ let $(low,high) = $1;
  $$ = ^$(CHARRANGE(low,high)); }
| PROSE_T
{ $$ = ^$(PROSE($1)); }
| ID PARAMS
{ $$ = ^$(arule_inj(new Symb($1,$2))); }
| ID
{ $$ = ^$(SYMB($1)); }
| LPAREN rule MINUS_T rule RPAREN
{ $$ = ^$(MINUS($2,$4));}
| LPAREN rule RPAREN
{ $$ = $!2; }
| LBRACKET rule RBRACKET
{ $$ = ^$(OPT($2)); }

dd_init_opt:
  DEF SEMACTION
{ $$ = ^$($2); }
| 
{ $$ = ^$(NULL); }

%%

/* TODO: hook this in.  Unfortunately, it means understanding
   a lot about lexing.h.  Note that in newer versions of ocamllex,
   there is built-in support. */
/* Get the character positions of newlines in a file;
   used in the output so that we can get better error messages. */
List::list_t<unsigned int> newlines = NULL;
void get_newlines(FILE @`H f) {
  newlines = NULL;
  rewind(f);
  unsigned int i = 0;
  int c;
  while ( (c = fgetc(f)) != EOF) {
    if (c == '\n') newlines = new List(i,newlines);
    i++;
  }
}

grammar_t parse_file(FILE @`H f) {
  parse_result = NULL;
  yyparse(Lexing::from_file(f));
  textblobs = List::imp_rev(textblobs);
  if (!textblobs)
    textblobs = new List::List(new "",textblobs);
  //  get_newlines(f);
  return parse_result;
}

grammar_t parse_string(char ?s) {
  newlines = NULL;
  parse_result = NULL;
  yyparse(Lexing::from_string(s));
  textblobs = List::imp_rev(textblobs);
  if (!textblobs)
    textblobs = new List::List(new "",textblobs);
  return parse_result;
}
