
%{
#define YYDEBUG 0

#include "core.h"
#include "stdio.h"
#include "list.h"
#include "hashtable.h"
#include "lexing.h"
#include "string.h"

#include "syntax.h"

using Core;
using List;

using Syntax;

namespace Lexer {
  extern int lexmain(Lexing::Lexbuf<`a>);
  extern datatype exn { extern Lexical_error(string_t,int,int) };
  extern int line_num;
  extern int line_start_pos;
}

namespace Parser {

opt_t<Lexing::Lexbuf<Lexing::Function_lexbuf_state<FILE@>>> lbuf = NULL;

datatype exn {Parser_error(string_t)};
typedef struct Hashtable::Table<stringptr_t,regular_expression_t> htbl;
  // must be initialized!
htbl * named_regexps = NULL;
lexer_definition_t parse_result = NULL;

regular_expression_t regexp_for_string(string_t s) {
  int len = strlen(s);
  if(len == 0)
    return &Epsilon_val;
  regular_expression_t ans = new Characters(new List((int)(s[len-1]), NULL));
  for(int n = len - 2; n >= 0; --n)
    ans = new Sequence(new Characters(new List((int)(s[n]), NULL)), ans);
  return ans;
}

list_t<int> char_class(int c1, int c2) {
  list_t<int> ans = NULL;
  for(int n = c2; n >= c1; --n)
    ans = new List(n,ans);
  return ans;
}

static list_t<int> all_chars_s = NULL;
static list_t<int> all_chars() {  
  if(all_chars_s == NULL)
    all_chars_s = char_class(0,255);
  return all_chars_s;
}

static list_t<int> subtract(list_t<int> l1, list_t<int> l2) {
  list_t<int> rev_ans = NULL;
  for(; l1 != NULL; l1 = l1->tl)
    if(!memq(l2, l1->hd))
      rev_ans = new List(l1->hd, rev_ans);
  return imp_rev(rev_ans);
}

} // end namespace Parser

using Parser;
%}

%token TIDENT TCHAR TSTRING TACTION
%token TRULE TPARSE TAND TEQUAL TOR TUNDERSCORE TEOF TLBRACKET TRBRACKET
%token TSTAR TMAYBE TPLUS TLPAREN TRPAREN TCARET TDASH TLET

%left TOR
%left CONCAT
%nonassoc TMAYBE
%left TSTAR
%left TPLUS

%start lexer_definition

%union {
  Lexer_definition_tok(lexer_definition_t);
  Location_tok(location_t);
  Int_tok(int);
  Entrypoint_List_tok(list_t<entrypoint_t,`H>);
  Entrypoint_tok(entrypoint_t);
  Acase_List_tok(list_t<acase_t,`H>);
  Acase_tok(acase_t);
  Regexp_tok(regular_expression_t);
  Char_tok(char);
  String_tok(string_t<`H>);
  Charclass_tok(list_t<int,`H>);
}

%type <lexer_definition_t> lexer_definition
%type <location_t> header TACTION
%type <int> named_regexps
%type <list_t<entrypoint_t,`H>> other_definitions
%type <entrypoint_t> definition
%type <list_t<acase_t,`H>> entry rest_of_entry
%type <acase_t> acase
%type <regular_expression_t> regexp
%type <char> TCHAR
%type <string_t<`H>> TSTRING TIDENT
%type <list_t<int,`H>> char_class char_class1

%%

lexer_definition: 
  header named_regexps TRULE definition other_definitions header
{ lexer_definition_t ans = new Lexer_definition($1, new List($4,rev($5)), $6);
  $$=^$(ans);
  parse_result = ans;
}

header:
  TACTION     { $$=$!1; }
| /* empty */ { $$=^$(new Location(0,0,1,0)); }

named_regexps:
  named_regexps TLET TIDENT TEQUAL regexp 
    { Hashtable::insert((Parser::htbl @)named_regexps, 
                        new {(string_t)$3}, $5); 
      $$=^$(0); }
| /* empty */ { $$=^$(0); }

other_definitions:
  other_definitions TAND definition 
     { $$=^$(new List($3,$1)); }
| /* empty */ { $$=^$(NULL); }

definition: TIDENT TEQUAL entry {
  $$=^$(new $($1,(string_t)NULL,NULL,$3));
}
| TIDENT TSTRING TEQUAL entry {
  $$=^$(new $($1,$2,NULL,$4));
}
| TIDENT TSTRING TSTRING TSTRING TEQUAL entry {
  $$=^$(new $($1,$2,new $($3,$4),$6));
}

entry:
  TPARSE acase rest_of_entry { $$=^$(new List($2,rev($3))); }
| TPARSE rest_of_entry       { $$=^$(rev($2)); }

rest_of_entry:
  rest_of_entry TOR acase { $$=^$(new List($3,$1)); }
| /* empty */ { $$=^$(NULL); }

acase: regexp TACTION { $$=^$(new $($1,$2)); }

regexp:
  TUNDERSCORE { $$=^$(new Characters(all_chars())); }
| TEOF        { /* FIX: may change to -1 like C library? */ 
                $$=^$(new Characters(new List(256,NULL))); }
| TCHAR       { $$=^$(new Characters(new List((int)($1),NULL))); }
| TSTRING     { $$=^$(regexp_for_string($1)); }
| TLBRACKET char_class TRBRACKET { $$=^$(new Characters($2)); }
| regexp TSTAR      { $$=^$(new Repetition($1)); }
| regexp TMAYBE     { $$=^$(new Alternative($1,&Epsilon_val)); }
| regexp TPLUS      { $$=^$(new Sequence($1, new Repetition($1))); }
| regexp TOR regexp { $$=^$(new Alternative($1,$3)); }
| regexp regexp %prec CONCAT { $$=^$(new Sequence($1,$2)); }
| TLPAREN regexp TRPAREN { $$=^$($2); }
| TIDENT { try $$=^$(Hashtable::lookup((Parser::htbl @)named_regexps,
				       new {(string_t)$1}));
	   catch { 
	     case &Not_found:
	     yyerror(aprintf("Reference to unbound regexp name `%s'", $1),-1,-1);
	     break;
	   }
         }

char_class:
  TCARET char_class1 { $$=^$(subtract(all_chars(),$2)); }
| char_class1        { $$=$!1; }

char_class1:
  TCHAR TDASH TCHAR                    { $$=^$(char_class($1,$3)); }
| TCHAR                                { $$=^$(new List((int)($1),NULL)); }
| char_class1 char_class1 %prec CONCAT { $$=^$(append($1,$2)); }

%%

namespace Parser {
lexer_definition_t parse_file(FILE @`H f) {
  named_regexps = Hashtable::create(13, strptrcmp, 
				    Hashtable::hash_stringptr);
  parse_result = NULL;
  lbuf = new Opt(Lexing::from_file(f));
  yyparse();
  return parse_result;
}
}
