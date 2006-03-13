/*
 * Copyright (c) 2000-2004
 *      The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */


/* Parser for iBanshee */
%{
#include "nonspec.h"
#include "regions.h"
#include "hash.h"
#include "banshee_persist_kinds.h"
#include "banshee_region_persist_kinds.h"

static region ibanshee_region;
static hash_table constructor_env;
static hash_table named_env;
static hash_table var_env;
static int interactive = 1;
static sort_kind current_row_base_sort;

extern FILE *yyin;

void ibanshee_error_handler(gen_e e1, gen_e e2, banshee_error_kind bek) 
{
  fprintf(stderr,"Warning: ");
  switch(bek) {
  case bek_cons_mismatch:         // c(...) <= d(...)
    fprintf(stderr, "constructor mismatch (inconsistent constraint) ");
    break;
  case bek_occurs_check:	// occurs check failed (cyclic unification)
    fprintf(stderr, "occurs check failure ");
    break;
  default:
    fprintf(stderr, "unknown error ");
  }
  fprintf(stderr,"between expressions: ");
  expr_print(stderr,e1);
  fprintf(stderr," and ");
  expr_print(stderr,e2);
  fprintf(stderr,"\n");
}

static void save_cs(const char *filename)
{
  FILE *f;
  hash_table entry_points[3] = {constructor_env,named_env,var_env};
  
  f = fopen(filename, "wb");
  
  assert(f);

  serialize_cs(f, entry_points, 3);
}

static void rsave_cs(const char *filename)
{
  FILE *f;
  f = fopen(filename, "wb");
  assert(f);

  fwrite((void *)&constructor_env, sizeof(hash_table), 1, f);
  fwrite((void *)&named_env, sizeof(hash_table), 1, f);
  fwrite((void *)&var_env, sizeof(hash_table), 1, f);
  
  serialize(get_persistent_regions("extras"), "data", "offsets");
  write_module_nonspec(f);
}

extern region *get_persistent_regions(const char *filename);

static void rload_cs(const char *filename)
{
  FILE *f;
  translation t;
  region temp = newregion();
  f = fopen(filename, "rb");

  assert(f);

  t = deserialize("data","offsets", get_updater_functions("extras"), temp);

  fread((void *)&constructor_env, sizeof(hash_table), 1, f);
  fread((void *)&named_env, sizeof(hash_table), 1, f);
  fread((void *)&var_env, sizeof(hash_table), 1, f);

  update_pointer(t, (void **)&constructor_env);
  update_pointer(t, (void **)&named_env);
  update_pointer(t, (void **)&var_env);

  update_module_nonspec(t, f);
}


static void load_cs(const char *filename)
{
  FILE *f = fopen(filename, "rb");
  hash_table *entry_points = deserialize_cs(f);
  constructor_env = entry_points[0];
  named_env = entry_points[1];
  var_env = entry_points[2];
}

static void show_help()
{
  printf("--- iBanshee Quick Reference ---\n\
ident            : [A-Z a-z]([A-Z a-z 0-9 _])*\n\
integers (i)     : [0-9]+\n\
\n\
Variables (v)    : '{ident}\n\
Constructors (c) : {ident}\n\
Labels (l)       : {ident}\n\
Names (n)        : {ident}\n\
\n\
Expressions (e)  : v | c | n | c(e1,...,en) | e1 && e2 | e1 || e2\n\
                 | <l1=e1,...,ln=en [| e]> | 0:s | 1:s | _:s\n\
                 | pat(c,i,e) | proj(c,i,e) | ( e ) | c^-i(e)\n\
\n\
sorts            : basesort | row(basesort)\n\
\n\
basesort         : setIF | term\n\
\n\
Var decl         : v : sort\n\
Constructor decl : c(s1,...,sn) : basesort\n\
Name decl        : n = e \n\
Sig (s)          : + sort | - sort | = sort\n\
\n\
Constraints      : e1 <= e2 | e1 == e2\n\
\n\
Commands         : !help\n\
                   !tlb e\n\
                   !ecr e\n\
                   !undo [i]\n\
                   !trace [i]\n\
                   !quit\n\
                   !save \"filename\"\n\
                   !load \"filename\"\n\
                   !rsave\n\
                   !rload\n\
                   !exit\n");
}

static void print_tlb(gen_e e) 
{
  gen_e_list sol = setif_tlb(e);
	       
  if (gen_e_list_length(sol) == 0) {
    printf("{}\n");
  }
  else {
    gen_e next;
    gen_e_list_scanner scan;
    
    gen_e_list_scan(sol,&scan);
    
    gen_e_list_next(&scan,&next);
    printf("{");
    expr_print(stdout,next);
    
    while(gen_e_list_next(&scan,&next)) {
      printf(", ");
      expr_print(stdout,next);
    }
    printf("}\n");
  }
}

static void ibanshee_init(void) {
  region_init();
  nonspec_init();
  register_error_handler(ibanshee_error_handler);

  ibanshee_region = newregion();
  constructor_env = 
    make_persistent_string_hash_table(32,
				      BANSHEE_PERSIST_KIND_constructor);
  named_env = 
    make_persistent_string_hash_table(32,
				      BANSHEE_PERSIST_KIND_gen_e);
  var_env = 
    make_persistent_string_hash_table(32,
				      BANSHEE_PERSIST_KIND_gen_e);
}

void flush_lexer(void);

%}			

%start program

/* Things with multiple spellings */
%token <num> TOK_INTEGER
%token <str> TOK_IDENT
%token <str> TOK_VAR

/* Punctuators */
%token TOK_LPAREN "("
%token TOK_RPAREN ")"
%token TOK_LANGLE "<"
%token TOK_RANGLE ">"
%token TOK_REST "|"
%token TOK_POS "+"
%token TOK_NEG "-"
%token TOK_EQ "="
%token TOK_DECL "#"
%token TOK_COMMA ","
%token TOK_WILD "_"
%token TOK_COLON ":"
%token TOK_CMD "!"
%token TOK_QUOTE "\""
%left TOK_INTER "&&"
%left TOK_UNION "||"
%token TOK_LEQ "<="
%token TOK_DEQ "=="
%token TOK_CARET "^"
%token TOK_LINE
%token TOK_ERROR
%token TOK_EOF

/* Keywords */
/* Commands (help,tlb,undo,time,trace,quit) are treated as
   identifiers,and interpreted after lexing. */

%token TOK_SETIF "setIF"
%token TOK_TERM "term" 
%token TOK_FLOW "flow"
%token TOK_ROW "row"
%token TOK_PROJ "proj"
%token TOK_PAT "pat"

%type <expr> expr
%type <exprs> expr_list
%type <esort> esort
%type <sort> basesort
%type <sort> sort 
%type <sig_elt_ptr> sig_elt
%type <pat> pattern
%type <rowmap> rowmap
%type <expr> row
%type <sig> signature

%union {
  int num;
  char *str;
  gen_e expr;
  gen_e_list exprs;
  e_sort esort;
  sort_kind sort;
  sig_elt* sig_elt_ptr;
  flowrow_map rowmap;
  sig_elt_list sig;
  pattern pat;
}
			      
 
%%

program: 
         line 
         { }
        | program line
         { }
;

line:      TOK_LINE
           { if (interactive) YYACCEPT; }
         | toplev TOK_LINE
           { if (interactive) YYACCEPT; }
         | TOK_EOF
           {exit(0);}
;

toplev:    decl
           { }
         | constraint
           {  }
         | cmd
           { }
;

decl:      TOK_VAR TOK_COLON esort
           { 
	     gen_e fresh_var;

	     if (hash_table_lookup(var_env,$1,NULL)) {
	      fprintf(stderr,"Attempted to redefine existing variable: %s",$1);
	       YYABORT;
	     }
	     else {
	       switch($3) {
	       case e_setif_sort:
		 fresh_var = setif_fresh($1);
		 break;
	       case e_term_sort:
		 fresh_var = term_fresh($1);
		 break;
	       case e_flowrow_setif_sort:
		 fresh_var = flowrow_fresh($1,setif_sort);
		 break;
	       case e_flowrow_term_sort:
                 fresh_var = flowrow_fresh($1,term_sort); 
		 break;
	       }	     
	       hash_table_insert(var_env,rstrdup(banshee_nonptr_region,$1),fresh_var);
	       printf("var: ");
	       expr_print(stdout, fresh_var);
	       printf("\n");
	     }
	   }
         | TOK_IDENT TOK_EQ expr
           {
	     if (hash_table_lookup(constructor_env,$1,NULL)) {
	       fprintf(stderr,"Attempted to redefine existing constructor: %s",$1);
	       YYABORT;
	     }
	     else {
	       hash_table_insert(named_env,rstrdup(banshee_nonptr_region, $1),(hash_data)$3);
	       printf("%s: ",$1);
	       expr_print(stdout,$3);
	       printf("\n");
	     }
           }
         | TOK_IDENT TOK_COLON basesort
           {
	     if (hash_table_lookup(constructor_env,$1,NULL)) {
               fprintf(stderr,"Attempted to redefine existing constructor: %s\n",$1);
	       YYABORT;
	     }
             else if (hash_table_lookup(named_env,$1,NULL)) {
	       fprintf(stderr,"Attempted to redefine existing expression: %s\n",$1);
	       YYABORT;
 	     }
	     else {
               constructor c = make_constructor($1,$3,NULL,0);
	       hash_table_insert(constructor_env,rstrdup(banshee_nonptr_region, $1),(hash_data)c);
	       printf("constructor: %s\n", $1);
	     }
           }
         | TOK_IDENT TOK_LPAREN signature TOK_RPAREN TOK_COLON basesort
           { 
	     if (hash_table_lookup(constructor_env,$1,NULL)) {
               fprintf(stderr,"Attempted to redefine existing constructor: %s\n",$1);
	       YYABORT;
	     }
             else if (hash_table_lookup(named_env,$1,NULL)) {
	       fprintf(stderr,"Attempted to redefine existing expression: %s\n",$1);
	       YYABORT;
 	     }
	     else {
               constructor c = make_constructor_from_list($1,$6,sig_elt_list_reverse($3));
	       hash_table_insert(constructor_env,rstrdup(banshee_nonptr_region, $1),c);
	       printf("constructor: %s\n", $1);
	     }
           }
;

signature: sig_elt
           { 
             sig_elt_list sig = new_sig_elt_list(ibanshee_region);
             sig_elt_list_cons($1,sig);
             $$ = sig;
           }
         | signature TOK_COMMA sig_elt
           {
             sig_elt_list_cons($3,$1);
             $$ = $1; 
           }
;

sig_elt:   TOK_POS sort
           {
	     sig_elt *eltptr = ralloc(banshee_nonptr_region,sig_elt);
	     eltptr->variance = vnc_pos;
	     eltptr->sort = $2;
	     $$ = eltptr;
           }
         | TOK_NEG sort
            {
	     sig_elt *eltptr = ralloc(banshee_nonptr_region,sig_elt);
	     eltptr->variance = vnc_neg;
	     eltptr->sort = $2;
	     $$ = eltptr;
           }
         | TOK_EQ sort
             {
	     sig_elt *eltptr = ralloc(banshee_nonptr_region,sig_elt);
	     eltptr->variance = vnc_non;
	     eltptr->sort = $2;
	     $$ = eltptr;
           }
;
 
sort:       basesort
           { $$ = $1;}
         | TOK_ROW TOK_LPAREN basesort TOK_RPAREN
           { $$ = flowrow_sort; }
;

esort:      basesort
           {
             switch($1) {
 	       case setif_sort: $$ = e_setif_sort; break;
               case term_sort: $$ = e_term_sort; break;
	     default: fprintf(stderr,"Bad base sort\n");
	       YYABORT;
	     } 
           }
         | TOK_ROW TOK_LPAREN basesort TOK_RPAREN
           { 
             switch($3) {
	       case setif_sort: $$ = e_flowrow_setif_sort; break;
               case term_sort: $$ = e_flowrow_term_sort; break;
	     default: fprintf(stderr,"Bad base sort\n");
	       YYABORT;
	     }
           } 
;

basesort:  TOK_SETIF
           { $$ = setif_sort; }
         | TOK_TERM
           { $$ = term_sort; }
;

constraint: expr TOK_LEQ expr
            { call_sort_inclusion($1,$3); }
         |  expr TOK_DEQ expr
            { call_sort_unify($1,$3); }
;

expr:    TOK_VAR
           { 
	     gen_e v = NULL;
	     
	     if (hash_table_lookup(var_env,$1,(hash_data*)&v)) {
	       $$ = v;
	     }
	     else {
	       fprintf(stderr,"Could not find variable: %s\n",$1);
	       YYABORT;
	     }
           }
         | TOK_IDENT /* constant or named expression */
           {
	     constructor c = NULL;
	     gen_e n = NULL;
             if (hash_table_lookup(constructor_env,$1,(hash_data *)&c)) {
	       $$ = constructor_expr(c,NULL,0);
	     }
	     else if (hash_table_lookup(named_env,$1,(hash_data*)&n)) {
	       $$ = n;
	     }
	     else {
	       fprintf(stderr,"Could not find constant or named expression: %s\n",$1);
	       YYABORT;
	     }
           }
         | TOK_IDENT TOK_LPAREN expr_list TOK_RPAREN /* constructed term */
           {
	     constructor c = NULL;

	     if (hash_table_lookup(constructor_env,$1,(hash_data *)&c)) {
	       $$ = 
		 constructor_expr(c,
				  gen_e_list_array_from_list(ibanshee_region,gen_e_list_reverse($3)),
				  gen_e_list_length($3));
	     }
	     else {
	       fprintf(stderr,"Could not find constructor: %s\n",$1);
	       YYABORT;
	     }
           }
         | expr TOK_UNION expr	/* the union of e1 and e2 */
           {
             gen_e_list exprs = new_gen_e_list(ibanshee_region); 
             gen_e_list_cons($1, exprs);
             gen_e_list_cons($3, exprs); 
             $$ = setif_union(exprs);
           }
         | expr TOK_INTER expr	/* the intersection of e1 and e2 */
           { 
             gen_e_list exprs = new_gen_e_list(ibanshee_region); 
             gen_e_list_cons($1, exprs);
             gen_e_list_cons($3, exprs); 
             $$ = setif_inter(exprs);
           }
         | TOK_LANGLE row TOK_RANGLE /* a row */
           {
             $$ = $2;
           }
         | TOK_INTEGER TOK_COLON esort /* only 0,1 though */
           { 
             if ($1 == 1) {
              switch($3) {
	       case e_setif_sort: $$ = setif_one(); break;
	       case e_term_sort: $$ = term_one(); break;
	       case e_flowrow_setif_sort: $$ = flowrow_one(setif_sort); break;
	       case e_flowrow_term_sort: $$ = flowrow_one(term_sort); break;
	       default: fprintf(stderr,"Invalid sort for zero expression\n");
		 YYABORT;
              } 

             }
             else if ($1 == 0) {
              switch($3) {
	       case e_setif_sort: $$ = setif_zero(); break;
	       case e_term_sort: $$ = term_zero(); break;
	       case e_flowrow_setif_sort: $$ = flowrow_zero(setif_sort); break;
	       case e_flowrow_term_sort: $$ = flowrow_zero(term_sort); break;
	       default: fprintf(stderr,"Invalid sort for one expression\n");
		 YYABORT;
	      } 
	     }
             else {
               fprintf(stderr,"Invalid expression\n");
	       YYABORT;
	     }
           }
         | TOK_WILD TOK_COLON esort	/* wildcard */
           { 
             switch($3) {
	     case e_flowrow_setif_sort: $$ = flowrow_wild(setif_sort); break;
	     case e_flowrow_term_sort: $$ = flowrow_wild(term_sort); break;
	     default: fprintf(stderr,"Invalid sort for wildcard expression\n");
	       YYABORT;
             }  
           }
         | TOK_PAT pattern	/* a projection pattern */
           { 
             $$ = setif_proj_pat($2.c,$2.i,$2.e); 
           }
         | TOK_PROJ pattern	/* a projection */
           { 
             $$ = setif_proj($2.c,$2.i,$2.e); 
           }
				/* another spelling for projection */
         | TOK_IDENT TOK_CARET TOK_NEG TOK_INTEGER TOK_LPAREN expr TOK_RPAREN
           {
	     constructor c = NULL;

	     if (!hash_table_lookup(constructor_env,$1,(hash_data *)&c)) {
	       fprintf(stderr,"Could not find constructor: %s\n",$1);
	       YYABORT;
	     }
             assert(c);
             $$ = setif_proj(c,$4,$6);
           }
         | TOK_LPAREN expr TOK_RPAREN /* parenthesized expression */
           { $$ = $2; }
;

expr_list: expr 
           {
             gen_e_list exprs = new_gen_e_list(ibanshee_region);
             gen_e_list_cons($1,exprs);
             $$ = exprs; 
           }
         | expr_list TOK_COMMA expr
           {
             gen_e_list_cons($3,$1);
             $$ = $1;
           }
;
 
row:       rowmap
           { 
             $$ = flowrow_make_row($1, flowrow_fresh("'rest", 
						     current_row_base_sort ));
           }
         | rowmap TOK_REST expr
           { 
             $$ = flowrow_make_row($1, $3);
           }
;

rowmap:    TOK_IDENT TOK_EQ expr 
           {
	     flowrow_field field;
	     flowrow_map map;
	     map = new_flowrow_map(ibanshee_region);
	     current_row_base_sort = expr_sort($3);
	     field = flowrow_make_field($1,$3);
	     flowrow_map_cons(field,map);
	     $$ = map;
           }
         | rowmap TOK_COMMA TOK_IDENT TOK_EQ expr
           {
	     flowrow_field field;
	     current_row_base_sort = expr_sort($5);
             field = flowrow_make_field($3,$5);
             flowrow_map_cons(field,$1);
             $$ = $1;
           }
;

pattern:   TOK_LPAREN TOK_IDENT TOK_COMMA TOK_INTEGER TOK_COMMA expr TOK_RPAREN
           { 
	     constructor c = NULL;
	     if (hash_table_lookup(constructor_env,$2,(hash_data *)&c)) {
	       $$ = (pattern){c,$4,$6}; 
	     }
	     else {
	       fprintf(stderr,"Could not find constructor: %s\n",$2);
	       YYABORT;
	     }
	   }
;

cmd:       TOK_CMD TOK_IDENT
           {
	     if (!strcmp($2,"quit")) {
	       exit(0);
	     }
	     else if (!strcmp($2,"exit")) {
               exit(0);
	     }
	     else if (!strcmp($2,"undo")) {
	       banshee_rollback();
	     }
	     else if (!strcmp($2,"help")) {
	       show_help();
	     }
	     else if (!strcmp($2,"rsave")) {
	       rsave_cs("statics");
	     }
	     else if (!strcmp($2,"rload")) {
	       rload_cs("statics");
	     }
             else if (!strcmp($2,"save") || !strcmp($2,"load")) {
	       fprintf(stderr,"Missing filename\n");
	     }
	     /* TODO */
	     else if (!strcmp($2,"trace")) {
	       fprintf(stderr,"Trace not yet implemented\n");
	     }
	     else {
	       fprintf(stderr,"Unrecognized command: %s\n",$2);
	       YYABORT;
	     }
	   }
        |  TOK_CMD TOK_IDENT TOK_INTEGER
           {
	     if (!strcmp($2,"undo")) {
	       banshee_backtrack($3);
	     }
	     /* TODO */
	     else if (!strcmp($2,"trace")) {
	       fprintf(stderr,"Trace not yet implemented\n");
	     }
	   }  
	   /* TODO -- really handle strings surrounded by quotes in the lexer*/
        |  TOK_CMD TOK_IDENT TOK_QUOTE TOK_IDENT TOK_QUOTE 
           {
             if (!strcmp($2,"save")) {
	       save_cs($4);
	     }
	     else if (!strcmp($2,"load")) {
	       load_cs($4);
	     }
           }
        |  TOK_CMD TOK_IDENT expr
           { 
	     if (!strcmp($2,"tlb")) {
	       print_tlb($3);
	     }
	     else if (!strcmp($2,"ecr")) {
	       expr_print(stdout,term_get_ecr($3));
	       printf("\n");
	     }
	   }
;

%%
int main(int argc, char **argv) {
  ibanshee_init();
  
  if (argc > 1) {
    if (!strcmp(argv[1],"-f")) {
      yyin = fopen(argv[2],"r");
      interactive = 0;
      
      if (yyin == NULL) {
	fprintf(stderr,"Failed to open file: %s\n",argv[2]);
      }
      else {
	if (yyparse()) {
	  fprintf(stderr,"Warning: some errors occurred\n");
	  exit(1);
	}
	else {
	  exit(0);
	}
      }
    }
  }


  if (interactive) {
    printf("iBanshee version 0.1\n");
    do {
      int time = banshee_get_time();
      printf("[%d] > ",time);
      fflush(stdout);
      
      if (yyparse()) { /* error parsing the line, ignore the remaining input */
	flush_lexer();
      }
    }
    while (1);
  }
}

