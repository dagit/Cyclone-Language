%{
#include <core.h>    // must include both core.h and string.h
#include <string.h>
#include <stdio.h>
  //#define YYDEBUG 1  // turn this on to debug the parser engine
#define YYERROR_VERBOSE 1 // turn this on to get better error messages

// a linked list associating identifiers with integer values
typedef struct Node {
  struct Node *next;
  string_t id;
  int val;
} *node_t;

node_t env = NULL;

// lookup the value associated with an identifier -- if not there,
// insert a new entry and initialize it to 0.  Returns a pointer to
// the value so that it can be updated in place.
int @lookup(string_t<`H> id) {
  for (let e = env; e != NULL; e = e->next) {
    if (strcmp(e->id,id) == 0) return &e->val;
  }
  env = new Node{.next = env, .id = id, .val = 0};
  return &env->val;
}

%}
%token INT IDENTIFIER   // define tokens here

%union {              // define constructors here
  Integer(int);
  Identifier(string_t<`H>);
}

%type <int> INT exp exp0 exp1  // give "types" to productions as constructors
%type <string_t<`H>> IDENTIFIER

%start prog
%%
prog:
  // the $1 implicitly "downcasts" from Integer
  exp ';' { printf("result is %d\n",$1); return 1;} 
| IDENTIFIER '=' exp ';' 
  { string_t id = $1;
    int v = $3;
    int@ vp = lookup(id); // lookup the identifier's value pointer
    *vp = v;              // set the identifier's value to the new value
    printf("set %s = %d\n",id,v); 
    return 1;
  }
| /* empty */ { return 0; }
;

exp:
  // $$ is assigned the result of exp0 (no downcast with $!1)
  exp0 { $$ = $!1; }
  // $$ is assigned the upcast (^$) of the sum of the downcasts of $1 and $3
| exp0 '+' exp { $$ = ^$($1 + $3); }
| exp0 '-' exp { $$ = ^$($1 - $3); }
;

exp0:
  exp1 { $$ = $!1; }
| exp1 '*' exp1 { $$ = ^$($1 * $3); }

exp1:
  INT { $$ = $!1; }
| IDENTIFIER { $$ = ^$(*lookup($1)); }
| '(' exp ')' { $$ = $!2; }
;
%%


