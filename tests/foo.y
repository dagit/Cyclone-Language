%{
#include <core.h>    // must include both core.h and string.h
#include <string.h>
#include <stdio.h>

  //#define YYDEBUG 1  // turn this on to debug the parser engine
#define YYERROR_VERBOSE 1 // turn this on to get better error messages
%}
%token INT            // define tokens here

%union {              // define constructors here
  Integer(int);
}

%type <int> INT exp exp0   // give "types" to productions as constructors

%start prog
%%
prog:
  // the $1 implicitly "downcasts" from Integer
  exp ';' { printf("result is %d\n",$1); } 
;

exp:
  // $$ is assigned the result of exp0 (no downcast with $!1)
  exp0 { $$ = $!1; }
  // $$ is assigned the upcast (^$) of the sum of the downcasts of $1 and $3
| exp0 '+' exp { $$ = ^$($1 + $3); }
;

exp0:
  INT { $$ = $!1; }
| '(' exp ')' { $$ = $!2; }
;
%%


