
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
using Stdio;
using List;

using Syntax;

namespace Lexer {
  extern int lexmain(Lexing::Lexbuf<`a>);
  extern xtunion exn { Lexical_error(string,int,int) };
  extern int line_num;
  extern int line_start_pos;
}

namespace Parser {

opt_t<Lexing::Lexbuf<Lexing::Function_lexbuf_state<FILE@>>> lbuf = null;

xtunion exn {Parser_error(string)};
typedef struct Hashtable::Table<stringptr,regular_expression_t,{},{}> htbl;
  // must be initialized!
htbl * named_regexps = null;
lexer_definition_t parse_result = null;

regular_expression_t regexp_for_string(string s) {
  int len = String::strlen(s);
  if(len == 0)
    return Epsilon;
  regular_expression_t ans = new Characters(new List((int)(s[len-1]), null));
  for(int n = len - 2; n >= 0; --n)
    ans = new Sequence(new Characters(new List((int)(s[n]), null)), ans);
  return ans;
}

list_t<int> char_class(int c1, int c2) {
  list_t<int> ans = null;
  for(int n = c2; n >= c1; --n)
    ans = new List(n,ans);
  return ans;
}

static list_t<int> all_chars_s = null;
static list_t<int> all_chars() {  
  if(all_chars_s == null)
    all_chars_s = char_class(0,255);
  return all_chars_s;
}

static list_t<int> subtract(list_t<int> l1, list_t<int> l2) {
  list_t<int> rev_ans = null;
  for(; l1 != null; l1 = l1->tl)
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
  Entrypoint_List_tok(list_t<entrypoint_t>);
  Entrypoint_tok(entrypoint_t);
  Acase_List_tok(list_t<acase_t>);
  Acase_tok(acase_t);
  Regexp_tok(regular_expression_t);
  Char_tok(char);
  String_tok(string);
  Charclass_tok(list_t<int>);
}

%type <Lexer_definition_tok> lexer_definition
%type <Location_tok> header TACTION
%type <Int_tok> named_regexps
%type <Entrypoint_List_tok> other_definitions
%type <Entrypoint_tok> definition
%type <Acase_List_tok> entry rest_of_entry
%type <Acase_tok> acase
%type <Regexp_tok> regexp
%type <Char_tok> TCHAR
%type <String_tok> TSTRING TIDENT
%type <Charclass_tok> char_class char_class1

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
    { Hashtable::insert((Parser::htbl @)named_regexps, new {$3}, $5); 
      $$=^$(0); }
| /* empty */ { $$=^$(0); }

other_definitions:
  other_definitions TAND definition 
     { $$=^$(new List($3,$1)); }
| /* empty */ { $$=^$(null); }

definition: TIDENT TEQUAL entry { $$=^$(new $($1,$3)); }

entry:
  TPARSE acase rest_of_entry { $$=^$(new List($2,rev($3))); }
| TPARSE rest_of_entry       { $$=^$(rev($2)); }

rest_of_entry:
  rest_of_entry TOR acase { $$=^$(new List($3,$1)); }
| /* empty */ { $$=^$(null); }

acase: regexp TACTION { $$=^$(new $($1,$2)); }

regexp:
  TUNDERSCORE { $$=^$(new Characters(all_chars())); }
| TEOF        { /* FIX: may change to -1 like C library? */ 
                $$=^$(new Characters(new List(256,null))); }
| TCHAR       { $$=^$(new Characters(new List((int)($1),null))); }
| TSTRING     { $$=^$(regexp_for_string($1)); }
| TLBRACKET char_class TRBRACKET { $$=^$(new Characters($2)); }
| regexp TSTAR      { $$=^$(new Repetition($1)); }
| regexp TMAYBE     { $$=^$(new Alternative($1,Epsilon)); }
| regexp TPLUS      { $$=^$(new Sequence($1, new Repetition($1))); }
| regexp TOR regexp { $$=^$(new Alternative($1,$3)); }
| regexp regexp %prec CONCAT { $$=^$(new Sequence($1,$2)); }
| TLPAREN regexp TRPAREN { $$=^$($2); }
| TIDENT { try $$=^$(Hashtable::lookup((Parser::htbl @)named_regexps,
				       new {$1}));
	   catch { 
	     case Not_found:
	     yyerror(xprintf("Reference to unbound regexp name `%s'", $1));
	     break;
	   }
         }

char_class:
  TCARET char_class1 { $$=^$(subtract(all_chars(),$2)); }
| char_class1        { $$=$!1; }

char_class1:
  TCHAR TDASH TCHAR                    { $$=^$(char_class($1,$3)); }
| TCHAR                                { $$=^$(new List((int)($1),null)); }
| char_class1 char_class1 %prec CONCAT { $$=^$(append($1,$2)); }

%%

namespace Parser {
lexer_definition_t parse_file(FILE @f) {
  named_regexps = Hashtable::create(13, String::strptrcmp, 
				    Hashtable::hash_stringptr);
  parse_result = null;
  lbuf = new Opt(Lexing::from_file(f));
  yyparse();
  return parse_result;
}
}
