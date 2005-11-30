/* This file was modified in 2000-2001 by David Gay for the RC compiler.
   The changes are 
   Copyright (c) 2000-2001 The Regents of the University of California.

   This file is distributed under the terms of the GNU General Public License
   (see below).
*/

/* YACC parser for C syntax and for Objective C.  -*-c-*-
   Copyright (C) 1987, 88, 89, 92-97, 1998 Free Software Foundation, Inc.

This file is part of GNU CC.

GNU CC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU CC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU CC; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

/* This file defines the grammar of C */
/* To whomever it may concern: I have heard that such a thing was once
   written by AT&T, but I have never seen it.  */

%expect 46

/* These are the 23 conflicts you should get in parse.output;
   the state numbers may vary if minor changes in the grammar are made.

State 42 contains 1 shift/reduce conflict.  (Two ways to parse ATTRIBUTE.)
State 44 contains 1 shift/reduce conflict.  (Two ways to recover from error.)
State 103 contains 1 shift/reduce conflict.  (Two ways to recover from error.)
State 110 contains 1 shift/reduce conflict.  (Two ways to parse ATTRIBUTE.)
State 111 contains 1 shift/reduce conflict.  (Two ways to recover from error.)
State 115 contains 1 shift/reduce conflict.  (Two ways to recover from error.)
State 132 contains 1 shift/reduce conflict.  (See comment at component_decl.)
State 180 contains 1 shift/reduce conflict.  (Two ways to parse ATTRIBUTE.)
State 194 contains 2 shift/reduce conflict.  (Four ways to parse this.)
State 202 contains 1 shift/reduce conflict.  (Two ways to recover from error.)
State 214 contains 1 shift/reduce conflict.  (Two ways to recover from error.)
State 220 contains 1 shift/reduce conflict.  (Two ways to recover from error.)
State 304 contains 2 shift/reduce conflicts.  (Four ways to parse this.)
State 335 contains 2 shift/reduce conflicts.  (Four ways to parse this.)
State 347 contains 1 shift/reduce conflict.  (Two ways to parse ATTRIBUTES.)
State 352 contains 1 shift/reduce conflict.  (Two ways to parse ATTRIBUTES.)
State 383 contains 2 shift/reduce conflicts.  (Four ways to parse this.)
State 434 contains 2 shift/reduce conflicts.  (Four ways to parse this.)  */


%{
#include <stdio.h>
#include <errno.h>
#include <setjmp.h>

#include "parser.h"
#include "c-parse.h"
#include "c-lex.h"
#include "semantics.h"
#include "input.h"
#include "expr.h"
#include "stmt.h"

int yyparse(void) deletes;

void yyerror();

/* Like YYERROR but do call yyerror.  */
#define YYERROR1 { yyerror ("syntax error"); YYERROR; }

/* Cause the `yydebug' variable to be defined.  */
#define YYDEBUG 1
%}

%start program

/* All identifiers that are not reserved words
   and are not declared typedefs in the current block */
%token IDENTIFIER

/* All identifiers that are declared typedefs in the current block.
   In some contexts, they are treated just like IDENTIFIER,
   but they can also serve as typespecs in declarations.  */
%token TYPENAME

/* Reserved words that specify storage class.
   yylval contains an IDENTIFIER_NODE which indicates which one.  */
%token <u.itoken> SCSPEC

/* Reserved words that specify type.
   yylval contains an IDENTIFIER_NODE which indicates which one.  */
%token <u.itoken> TYPESPEC

/* Reserved words that qualify types/functions: "const" or "volatile", 
   "deletes".
   yylval contains an IDENTIFIER_NODE which indicates which one.  */
%token <u.itoken> TYPE_QUAL FN_QUAL

/* Character or numeric constants.
   yylval is the node for the constant.  */
%token CONSTANT

/* String constants in raw form.
   yylval is a STRING_CST node.  */
%token STRING MAGIC_STRING

/* "...", used for functions with variable arglists.  */
%token <u.itoken> ELLIPSIS

/* the reserved words */
/* SCO include files test "ASM", so use something else. */
%token <u.itoken> SIZEOF ENUM STRUCT UNION IF ELSE WHILE DO FOR SWITCH CASE DEFAULT
%token <u.itoken> BREAK CONTINUE RETURN GOTO ASM_KEYWORD TYPEOF ALIGNOF
%token <u.itoken> ATTRIBUTE EXTENSION LABEL
%token <u.itoken> REALPART IMAGPART VA_ARG

/* Add precedence rules to solve dangling else s/r conflict */
%nonassoc IF
%nonassoc ELSE

/* Define the operator tokens and their precedences.
   The value is an integer because, if used, it is the tree code
   to use in the expression made from the operator.  */

%right <u.itoken> ASSIGN '='
%right <u.itoken> '?' ':'
%left <u.itoken> OROR
%left <u.itoken> ANDAND
%left <u.itoken> '|'
%left <u.itoken> '^'
%left <u.itoken> '&'
%left <u.itoken> EQCOMPARE
%left <u.itoken> ARITHCOMPARE
%left <u.itoken> LSHIFT RSHIFT
%left <u.itoken> '+' '-'
%left <u.itoken> '*' '/' '%'
%right <u.itoken> UNARY PLUSPLUS MINUSMINUS
%left HYPERUNARY
%left <u.itoken> POINTSAT '.' '(' '['

%type <u.asm_operand> asm_operand asm_operands nonnull_asm_operands
%type <u.asm_stmt> maybeasm
%type <u.attribute> maybe_attribute attributes attribute attribute_list attrib
%type <u.constant> CONSTANT
%type <u.decl> datadecl datadecls datadef decl decls extdef extdefs fndef
%type <u.decl> initdecls nested_function notype_nested_function notype_initdecls
%type <u.decl> old_style_parm_decls initdcl
%type <u.decl> component_decl_list component_decl_list2 component_decl
%type <u.decl> components component_declarator enumerator enumlist
%type <u.decl> parmlist parmlist_1 parmlist_2 parms parm
%type <u.decl> parmlist_or_identifiers identifiers notype_initdcl
%type <u.decl> parmlist_or_identifiers_1 old_parameter
%type <u.declarator> declarator after_type_declarator notype_declarator
%type <u.declarator> absdcl absdcl1 parm_declarator
%type <u.expr> cast_expr expr expr_no_commas exprlist init initlist_maybe_comma initlist1 initelt nonnull_exprlist primary string_component STRING string_list
%type <u.expr> unary_expr xexpr
%type <u.id_label> id_label maybe_label_decls label_decls label_decl
%type <u.id_label> identifiers_or_typenames
%type <idtoken> identifier IDENTIFIER TYPENAME MAGIC_STRING
%type <u.iexpr> if_prefix
%type <u.istmt> stmt_or_labels simple_if stmt_or_label
%type <u.itoken> unop extension '~' '!' compstmt_start '{' ';'
%type <u.itoken> sizeof alignof
%type <u.label> label
%type <u.stmt> stmts xstmts compstmt_or_error compstmt
%type <u.stmt> labeled_stmt stmt
%type <u.cstmt> do_stmt_start
%type <u.string> asm_clobbers string
%type <u.telement> scspec type_qual type_spec
%type <u.telement> declmods declmods_no_prefix_attr 
%type <u.telement> reserved_declspecs reserved_declspecs_no_prefix_attr 
%type <u.telement> typed_declspecs typed_declspecs_no_prefix_attr
%type <u.telement> typed_typespecs reserved_typespecquals 
%type <u.telement> typespec typespecqual_reserved structsp
%type <u.telement> nonempty_type_quals type_quals
%type <u.telement> maybe_type_qual fn_qual fn_quals
%type <u.type> typename
%type <u.word> idword any_word tag

%{
/* Region in which to allocate parse structures. Idea: the AST user can set
   this to different regions at appropriate junctures depending on what's
   being done with the AST */
region parse_region;
/* We'll see this a LOT below */
#define pr parse_region

/* Number of statements (loosely speaking) and compound statements 
   seen so far.  */
static int stmt_count;
static int compstmt_count;
  
/* List of types and structure classes of the current declaration.  */
static type_element current_declspecs = NULL;
static attribute prefix_attributes = NULL;

/* >0 if currently parsing an expression that will not be evaluated (argument
   to alignof, sizeof. Currently not typeof though that could be considered
   a bug) */
int unevaluated_expression;

#ifdef RC_ADJUST
static size_t rc_adjust_yystype(void *x, int by) 
{
  struct yystype *p = x;
  RC_ADJUST_PREAMBLE;

  RC_ADJUST(p->u.ptr, by);
  RC_ADJUST(p->idtoken.location.filename, by);
  RC_ADJUST(p->idtoken.id.data, by);
  RC_ADJUST(p->idtoken.decl, by);

  return sizeof *p;
}

static void rc_update_yystype(struct yystype *old, struct yystype *new)
{
  regionid base = regionidof(old);

  RC_UPDATE(base, old->u.ptr, new->u.ptr);
  RC_UPDATE(base, old->idtoken.location.filename, new->idtoken.location.filename);
  RC_UPDATE(base, old->idtoken.id.data, new->idtoken.id.data);
  RC_UPDATE(base, old->idtoken.decl, new->idtoken.decl);
}
#endif

/* A stack of declspecs and attributes for use during parsing */
typedef struct spec_stack *spec_stack;
struct spec_stack { 
  type_element parentptr declspecs;
  attribute parentptr attributes;
  spec_stack sameregion next;
};

/* Stack of saved values of current_declspecs and prefix_attributes.  */
/* In an ideal world, we would be able to eliminate most rc ops for
   declspec_stack and ds_region assignments. Seems tricky though. */
static spec_stack declspec_stack;
static region ds_region;

/* Pop top entry of declspec_stack back into current_declspecs,
   prefix_attributes */
static void pop_declspec_stack(void) deletes
{
  current_declspecs = declspec_stack->declspecs;
  prefix_attributes = declspec_stack->attributes;
  declspec_stack = declspec_stack->next;
  if (!declspec_stack)
    deleteregion_ptr(&ds_region);
}

static void push_declspec_stack(void)
{
  spec_stack news;

  if (!ds_region) ds_region = newsubregion(parse_region);
  news = ralloc(ds_region, struct spec_stack);
  news->declspecs = current_declspecs;
  news->attributes = prefix_attributes;
  news->next = declspec_stack;
  declspec_stack = news;
}

/* Tell yyparse how to print a token's value, if yydebug is set.  */

#define YYPRINT(FILE,YYCHAR,YYLVAL) yyprint(FILE,YYCHAR,YYLVAL)
void yyprint();
%}

%%
program: /* empty */
		{ if (pedantic)
		    pedwarn("ANSI C forbids an empty source file");
		  the_program = NULL;
		}
	| extdefs
		{
		  the_program = $1;
		}
	;

/* the reason for the strange actions in this rule
 is so that notype_initdecls when reached via datadef
 can find a valid list of type and sc specs in $0. */

extdefs:
	{ $<u.telement>$ = NULL; } extdef { $$ = $2; }
	| extdefs { $<u.telement>$ = NULL; } extdef
		{ $$ = declaration_chain($1, $3); }	  
	;

extdef:
	fndef
	| datadef
	| ASM_KEYWORD '(' expr ')' ';'
		{ 
		  $$ = CAST(declaration, new_asm_decl
		    (pr, $1.location,
		     new_asm_stmt(pr, $1.location, $3, NULL, NULL, NULL, NULL))); }
	| extension extdef
		{ pedantic = $1.i; 
		  $$ = CAST(declaration, new_extension_decl(pr, $1.location, $2)); }
	;

datadef:
	  setspecs notype_initdecls ';'
		{ if (pedantic)
		    error("ANSI C forbids data definition with no type or storage class");
		  else if (!flag_traditional)
		    warning("data definition has no type or storage class"); 

		  $$ = CAST(declaration, new_data_decl(pr, $2->location, NULL, NULL, $2));
		  pop_declspec_stack(); }
        | declmods setspecs notype_initdecls ';'
		{ $$ = CAST(declaration, new_data_decl(pr, $1->location, current_declspecs, prefix_attributes, $3));
		  pop_declspec_stack(); }
	| typed_declspecs setspecs initdecls ';'
		{ $$ = CAST(declaration, new_data_decl(pr, $1->location, current_declspecs, prefix_attributes, $3));
		  pop_declspec_stack(); }
        | declmods ';'
	  { pedwarn("empty declaration"); }
	| typed_declspecs setspecs ';'
	  { shadow_tag($1); 
	    $$ = CAST(declaration, new_data_decl(pr, $1->location, current_declspecs, prefix_attributes, NULL));
	    pop_declspec_stack(); }
	| error ';' { $$ = new_error_decl(pr, last_location); }
	| error '}' { $$ = new_error_decl(pr, last_location); }
	| ';'
		{ if (pedantic)
		    pedwarn("ANSI C does not allow extra `;' outside of a function");
		  $$ = NULL; }
	;

fndef:
	  typed_declspecs setspecs declarator
		{ if (!start_function(current_declspecs, $3,
				      prefix_attributes, 0))
		    YYERROR1; }
	  old_style_parm_decls
		{ store_parm_decls($5); }
	  compstmt_or_error
		{ $$ = finish_function($7);
		  pop_declspec_stack(); }
	| typed_declspecs setspecs declarator error
		{ $$ = new_error_decl(pr, last_location);
		  pop_declspec_stack(); }
	| declmods setspecs notype_declarator
		{ if (!start_function(current_declspecs, $3,
				      prefix_attributes, 0))
		    YYERROR1; }
	  old_style_parm_decls
		{ store_parm_decls($5); }
	  compstmt_or_error
		{ $$ = finish_function($7); 
		  pop_declspec_stack(); }
	| declmods setspecs notype_declarator error
		{ $$ = new_error_decl(pr, last_location);
		  pop_declspec_stack(); }
	| setspecs notype_declarator
		{ if (!start_function(NULL, $2,
				      prefix_attributes, 0))
		    YYERROR1; }
	  old_style_parm_decls
		{ store_parm_decls($4); }
	  compstmt_or_error
		{ $$ = finish_function($6); 
		  pop_declspec_stack(); }
	| setspecs notype_declarator error
		{ $$ = new_error_decl(pr, last_location);
		  pop_declspec_stack(); }
	;

identifier:
	IDENTIFIER
	| TYPENAME
	;

id_label:
	identifier { $$ = new_id_label(pr, $1.location, $1.id); }
	;

idword:
	identifier { $$ = new_word(pr, $1.location, $1.id); }

unop:     '&'
		{ $$ = $1; $$.i = kind_address_of; }
	| '-'
		{ $$ = $1; $$.i = kind_unary_minus; }
	| '+'
		{ $$ = $1; $$.i = kind_unary_plus; }
	| PLUSPLUS
		{ $$ = $1; $$.i = kind_preincrement; }
	| MINUSMINUS
		{ $$ = $1; $$.i = kind_predecrement; }
	| '~'
		{ $$ = $1; $$.i = kind_bitnot; }
	| '!'
		{ $$ = $1; $$.i = kind_not; }
	| REALPART
		{ $$ = $1; $$.i = kind_realpart; }
	| IMAGPART
		{ $$ = $1; $$.i = kind_imagpart; }
	;

expr:	nonnull_exprlist
		{ if ($1->next)
		    $$ = make_comma($1->location, $1);
		  else
		    $$ = $1; }
	;

exprlist:
	  /* empty */
		{ $$ = NULL; }
	| nonnull_exprlist
	;

nonnull_exprlist:
	expr_no_commas
		{ $$ = $1; }
	| nonnull_exprlist ',' expr_no_commas
		{ $$ = expression_chain($1, $3); }
	;

unary_expr:
	primary
	| '*' cast_expr   %prec UNARY
		{ $$ = make_dereference($1.location, $2); }
	/* __extension__ turns off -pedantic for following primary.  */
	| extension cast_expr	  %prec UNARY
		{ $$ = make_extension_expr($1.location, $2);
		  pedantic = $1.i; }
	| unop cast_expr  %prec UNARY
		{ $$ = make_unary($1.location, $1.i, $2);
#if 0
		  overflow_warning($$); 
#endif
		}
	/* Refer to the address of a label as a pointer.  */
	| ANDAND id_label
		{
		  $$ = CAST(expression, make_label_address($1.location, $2));
		  use_label($2);
		}
	| sizeof unary_expr  %prec UNARY
		{ 
#if 0
		  if (TREE_CODE ($2) == COMPONENT_REF
		      && DECL_C_BIT_FIELD (TREE_OPERAND ($2, 1)))
		    error("`sizeof' applied to a bit-field");
		  $$ = c_sizeof (TREE_TYPE ($2)); 
#endif
		  $$ = make_sizeof_expr($1.location, $2);
		  unevaluated_expression--; }
	| sizeof '(' typename ')'  %prec HYPERUNARY
		{ $$ = make_sizeof_type($1.location, $3);
		  unevaluated_expression--; }
	| alignof unary_expr  %prec UNARY
		{ $$ = make_alignof_expr($1.location, $2);
		  unevaluated_expression--; }
	| alignof '(' typename ')'  %prec HYPERUNARY
		{ $$ = make_alignof_type($1.location, $3); 
		  unevaluated_expression--; }
	;

sizeof:
	SIZEOF { unevaluated_expression++; $$ = $1; }
	;

alignof:
	ALIGNOF { unevaluated_expression++; $$ = $1; }
	;

cast_expr:
	unary_expr
	| '(' typename ')' cast_expr  %prec UNARY
	  	{ $$ = make_cast($1.location, $2, $4); }
	| '(' typename ')' '{' 
		{ 
#if 0
		  start_init (NULL, NULL, 0);
		  $2 = groktypename ($2);
		  really_start_incremental_init ($2); 
#endif
		}
	  initlist_maybe_comma '}'  %prec UNARY
		{ 
		  $$ = CAST(expression, new_cast_list(pr, $1.location, $2, CAST(expression, new_init_list(pr, $6->location, $6))));
		  $$->type = $2->type;
		  /* XXX: Evil hack for foo((int[5]) {1, 2, 3}) */
		  /* XXX: what does gcc do ? */
		  if (type_array($$->type))
		    $$->lvalue = TRUE;

		  if (pedantic)
		    pedwarn("ANSI C forbids constructor expressions");
#if 0
		  char *name;
		  tree result = pop_init_level (0);
		  tree type = $2;
		  finish_init ();

		  if (TYPE_NAME (type) != 0)
		    {
		      if (TREE_CODE (TYPE_NAME (type)) == IDENTIFIER_NODE)
			name = IDENTIFIER_POINTER (TYPE_NAME (type));
		      else
			name = IDENTIFIER_POINTER (DECL_NAME (TYPE_NAME (type)));
		    }
		  else
		    name = "";
		  $$ = result;
		  if (TREE_CODE (type) == ARRAY_TYPE && TYPE_SIZE (type) == 0)
		    {
		      int failure = complete_array_type (type, $$, 1);
		      if (failure)
			abort ();
		    }
#endif
		}
	;

expr_no_commas:
	  cast_expr
	| expr_no_commas '+' expr_no_commas
	    	{ $$ = make_binary($2.location, kind_plus, $1, $3); }
	| expr_no_commas '-' expr_no_commas
	    	{ $$ = make_binary($2.location, kind_minus, $1, $3); }
	| expr_no_commas '*' expr_no_commas
	    	{ $$ = make_binary($2.location, kind_times, $1, $3); }
	| expr_no_commas '/' expr_no_commas
	    	{ $$ = make_binary($2.location, kind_divide, $1, $3); }
	| expr_no_commas '%' expr_no_commas
	    	{ $$ = make_binary($2.location, kind_modulo, $1, $3); }
	| expr_no_commas LSHIFT expr_no_commas
	    	{ $$ = make_binary($2.location, kind_lshift, $1, $3); }
	| expr_no_commas RSHIFT expr_no_commas
	    	{ $$ = make_binary($2.location, kind_rshift, $1, $3); }
	| expr_no_commas ARITHCOMPARE expr_no_commas
	    	{ $$ = make_binary($2.location, $2.i, $1, $3); }
	| expr_no_commas EQCOMPARE expr_no_commas
	    	{ $$ = make_binary($2.location, $2.i, $1, $3); }
	| expr_no_commas '&' expr_no_commas
	    	{ $$ = make_binary($2.location, kind_bitand, $1, $3); }
	| expr_no_commas '|' expr_no_commas
	    	{ $$ = make_binary($2.location, kind_bitor, $1, $3); }
	| expr_no_commas '^' expr_no_commas
	    	{ $$ = make_binary($2.location, kind_bitxor, $1, $3); }
	| expr_no_commas ANDAND expr_no_commas
	    	{ $$ = make_binary($2.location, kind_andand, $1, $3); }
	| expr_no_commas OROR expr_no_commas
	    	{ $$ = make_binary($2.location, kind_oror, $1, $3); }
	| expr_no_commas '?' expr ':' expr_no_commas
	  	{ $$ = make_conditional($2.location, $1, $3, $5); }
	| expr_no_commas '?'
		{ if (pedantic)
		    pedwarn("ANSI C forbids omitting the middle term of a ?: expression"); 
		}
	  ':' expr_no_commas
	  	{ $$ = make_conditional($2.location, $1, NULL, $5); }
	| expr_no_commas '=' expr_no_commas
	    	{ $$ = make_assign($2.location, kind_assign, $1, $3); }
	| expr_no_commas ASSIGN expr_no_commas
	    	{ $$ = make_assign($2.location, $2.i, $1, $3); }
	;

primary:
	IDENTIFIER
		{ 
		  if (yychar == YYEMPTY)
		    yychar = YYLEX;
		  $$ = make_identifier($1.location, $1.id, yychar == '('); 
		}
	| CONSTANT { $$ = CAST(expression, $1); }
	| string { $$ = CAST(expression, $1); }
	| '(' expr ')'
		{ $$ = $2; }
	| '(' error ')'
		{ $$ = make_error_expr(last_location); }
	| '('
		{ if (current_function_decl == 0)
		    {
		      error("braced-group within expression allowed only inside a function");
		      YYERROR;
		    }
		    push_label_level();
		}
	  compstmt ')'
		{ 
		  pop_label_level();
		  if (pedantic)
		    pedwarn("ANSI C forbids braced-groups within expressions");
		  $$ = make_compound_expr($1.location, $3);
		}
	| primary '(' exprlist ')'   %prec '.'
	  	{ $$ = make_function_call($2.location, $1, $3); }
	| VA_ARG '(' expr_no_commas ',' typename ')'
		{ $$ = make_va_arg($1.location, $3, $5); }
	| primary '[' expr ']'   %prec '.'
		{ $$ = make_array_ref($2.location, $1, $3); }
	| primary '.' identifier
		{ $$ = make_field_ref($2.location, $1, $3.id); }
	| primary POINTSAT identifier
		{ $$ = make_field_ref($2.location, make_dereference($2.location, $1),
				      $3.id); }
	| primary PLUSPLUS
		{ $$ = make_postincrement($2.location, $1); }
	| primary MINUSMINUS
		{ $$ = make_postdecrement($2.location, $1); }
	;

/* Produces a STRING_CST with perhaps more STRING_CSTs chained onto it.  */
string:
	string_list { $$ = make_string($1->location, $1); }

string_list:
	string_component { $$ = $1; }
	| string_list string_component
		{ $$ = expression_chain($1, $2); }
	;

string_component:
	STRING { $$ = CAST(expression, $1); }
	| MAGIC_STRING
	  { $$ = make_identifier($1.location, $1.id, FALSE);
	  }
	;


old_style_parm_decls:
	/* empty */ { $$ = NULL; }
	| datadecls
	| datadecls ELLIPSIS
		/* ... is used here to indicate a varargs function.  */
		{ if (pedantic)
		    pedwarn("ANSI C does not permit use of `varargs.h'"); 
		  $$ = declaration_chain($1, CAST(declaration, new_ellipsis_decl(pr, $2.location)));
		}
	;

/* The following are analogous to decls and decl
   except that they do not allow nested functions.
   They are used for old-style parm decls.  */
datadecls:
	datadecl
	| errstmt { $$ = new_error_decl(pr, last_location); }
	| datadecls datadecl { $$ = declaration_chain($1, $2); }
	| datadecl errstmt { $$ = new_error_decl(pr, last_location); }
	;

/* We don't allow prefix attributes here because they cause reduce/reduce
   conflicts: we can't know whether we're parsing a function decl with
   attribute suffix, or function defn with attribute prefix on first old
   style parm.  */
datadecl:
	typed_declspecs_no_prefix_attr setspecs initdecls ';'
		{ $$ = CAST(declaration, new_data_decl(pr, $1->location, current_declspecs, prefix_attributes, $3));
		  pop_declspec_stack(); }
	| declmods_no_prefix_attr setspecs notype_initdecls ';'
		{ $$ = CAST(declaration, new_data_decl(pr, $1->location, current_declspecs, prefix_attributes, $3));
		  pop_declspec_stack(); }
	| typed_declspecs_no_prefix_attr setspecs ';'
		{ shadow_tag_warned($1, 1);
		  $$ = CAST(declaration, new_data_decl(pr, $1->location, current_declspecs, prefix_attributes, NULL));
		  pop_declspec_stack();
		  pedwarn("empty declaration"); }
	| declmods_no_prefix_attr ';'
		{ pedwarn("empty declaration"); 
		  $$ = NULL; }
	;

/* This combination which saves a lineno before a decl
   is the normal thing to use, rather than decl itself.
   This is to avoid shift/reduce conflicts in contexts
   where statement labels are allowed.  */
decls:
	decl
	| errstmt { $$ = new_error_decl(pr, last_location); }
	| decls decl { $$ = declaration_chain($1, $2); }
	| decl errstmt { $$ = new_error_decl(pr, last_location); }
	;

/* records the type and storage class specs to use for processing
   the declarators that follow.
   Maintains a stack of outer-level values of current_declspecs,
   for the sake of parm declarations nested in function declarators.  */
setspecs: /* empty */
		{ 
		  push_declspec_stack();
		  pending_xref_error();
		  split_type_elements($<u.telement>0,
				      &current_declspecs, &prefix_attributes);
		}
	;

/* ??? Yuck.  See after_type_declarator.  */
setattrs: /* empty */
		{ prefix_attributes = attribute_chain(prefix_attributes,
						      $<u.attribute>0); 
		/* This syntax is broken as it will apply to all remaining
		   declarations, not just the current declarator 
		   (this is broken in base GCC too) */
		/* XXX: Used extensively in the linux kernel. YUCK. */
		/*error("Unsupported attribute syntax");*/ }
	;

decl:
	typed_declspecs setspecs initdecls ';'
		{ $$ = CAST(declaration, new_data_decl(pr, $1->location, current_declspecs, prefix_attributes, $3));
		  pop_declspec_stack(); }
	| declmods setspecs notype_initdecls ';'
		{ $$ = CAST(declaration, new_data_decl(pr, $1->location, current_declspecs, prefix_attributes, $3));
		  pop_declspec_stack(); }
	| typed_declspecs setspecs nested_function
		{ $$ = $3;
		  pop_declspec_stack(); }
	| declmods setspecs notype_nested_function
		{ $$ = $3;
		  pop_declspec_stack(); }
	| typed_declspecs setspecs ';'
		{ shadow_tag($1);
		  $$ = CAST(declaration, new_data_decl(pr, $1->location, current_declspecs, prefix_attributes, NULL));
		  pop_declspec_stack(); }
	| declmods ';'
		{ pedwarn("empty declaration"); }
	| extension decl
		{ pedantic = $1.i; 
		  $$ = CAST(declaration, new_extension_decl(pr, $1.location, $2)); }
	;

/* Declspecs which contain at least one type specifier or typedef name.
   (Just `const' or `volatile' is not enough.)
   A typedef'd name following these is taken as a name to be declared. */

typed_declspecs:
	  typespec reserved_declspecs
		{ $$ = $1; $1->next = CAST(node, $2); }
	| declmods typespec reserved_declspecs
		{ $$ = type_element_chain($1, $2); $2->next = CAST(node, $3); }
	;

reserved_declspecs:  /* empty */
		{ $$ = NULL; }
	| reserved_declspecs typespecqual_reserved
		{ $$ = type_element_chain($1, $2); }
	| reserved_declspecs scspec
		{ if (extra_warnings)
		    warning("`%s' is not at beginning of declaration",
			    rid_name(CAST(rid, $2)));
		  $$ = type_element_chain($1, $2); }
	| reserved_declspecs attributes
		{ $$ = type_element_chain($1, CAST(type_element, $2)); }
	;

typed_declspecs_no_prefix_attr:
	  typespec reserved_declspecs_no_prefix_attr
		{ $$ = $1; $1->next = CAST(node, $2); }
	| declmods_no_prefix_attr typespec reserved_declspecs_no_prefix_attr
		{ $$ = type_element_chain($1, $2); $2->next = CAST(node, $3); }
	;

reserved_declspecs_no_prefix_attr:
	  /* empty */
		{ $$ = NULL; }
	| reserved_declspecs_no_prefix_attr typespecqual_reserved
		{ $$ = type_element_chain($1, $2); }
	| reserved_declspecs_no_prefix_attr scspec
		{ if (extra_warnings)
		    warning("`%s' is not at beginning of declaration",
			    rid_name(CAST(rid, $2)));
		  $$ = type_element_chain($1, $2); }
	;

/* List of just storage classes, type modifiers, and prefix attributes.
   A declaration can start with just this, but then it cannot be used
   to redeclare a typedef-name.
   Declspecs have a non-NULL TREE_VALUE, attributes do not.  */

declmods:
	  declmods_no_prefix_attr
	| attributes { $$ = CAST(type_element, $1); }
	| declmods declmods_no_prefix_attr
		{ $$ = type_element_chain($1, $2); }
	| declmods attributes
		{ $$ = type_element_chain($1, CAST(type_element, $2)); }
	;

declmods_no_prefix_attr:
	  type_qual
	| scspec
	| declmods_no_prefix_attr type_qual
		{ $$ = type_element_chain($1, $2); }
	| declmods_no_prefix_attr scspec
		{ if (extra_warnings /*&& TREE_STATIC ($1)*/)
		    warning("`%s' is not at beginning of declaration",
			    rid_name(CAST(rid, $2)));
		  $$ = type_element_chain($1, $2); }
	;


/* Used instead of declspecs where storage classes are not allowed
   (that is, for typenames and structure components).
   Don't accept a typedef-name if anything but a modifier precedes it.  */

typed_typespecs:
	  typespec reserved_typespecquals
		{ $$ = $1; $1->next = CAST(node, $2); }
	| nonempty_type_quals typespec reserved_typespecquals
		{ $$ = type_element_chain($1, $2); $2->next = CAST(node, $3); }
	;

reserved_typespecquals:  /* empty */
		{ $$ = NULL; }
	| reserved_typespecquals typespecqual_reserved
		{ $$ = type_element_chain($1, $2); }
	;

/* A typespec (but not a type qualifier).
   Once we have seen one of these in a declaration,
   if a typedef name appears then it is being redeclared.  */

typespec: type_spec
	| structsp
	| TYPENAME
		{ /* For a typedef name, record the meaning, not the name.
		     In case of `foo foo, bar;'.  */
		  $$ = CAST(type_element, new_typename(pr, $1.location, $1.decl)); }
	| TYPEOF '(' expr ')'
		{ $$ = CAST(type_element, new_typeof_expr(pr, $1.location, $3)); }
	| TYPEOF '(' typename ')'
		{ $$ = CAST(type_element, new_typeof_type(pr, $1.location, $3)); }
	;

/* A typespec that is a reserved word, or a type qualifier.  */

typespecqual_reserved: type_spec
	| type_qual
	| structsp
	;

initdecls:
	initdcl
	| initdecls ',' initdcl { $$ = declaration_chain($1, $3); }
	;

notype_initdecls:
	notype_initdcl { $$ = $1; }
	| notype_initdecls ',' initdcl { $$ = declaration_chain($1, $3); }
	;

maybeasm:
	  /* empty */
		{ $$ = NULL; }
	| ASM_KEYWORD '(' string ')'
		{ $$ = new_asm_stmt(pr, $1.location, CAST(expression, $3),
				    NULL, NULL, NULL, NULL); }
	;

initdcl:
	  declarator maybeasm maybe_attribute '='
		{ $<u.decl>$ = start_decl($1, $2, current_declspecs, 1,
					$3, prefix_attributes); }
	  init
/* Note how the declaration of the variable is in effect while its init is parsed! */
		{ $$ = finish_decl($<u.decl>5, $6); }
	| declarator maybeasm maybe_attribute
		{ declaration d = start_decl($1, $2, current_declspecs, 0,
					     $3, prefix_attributes);
		  $$ = finish_decl(d, NULL); }
	;

notype_initdcl:
	  notype_declarator maybeasm maybe_attribute '='
		{ $<u.decl>$ = start_decl($1, $2, current_declspecs, 1,
					 $3, prefix_attributes); }
	  init
/* Note how the declaration of the variable is in effect while its init is parsed! */
		{ $$ = finish_decl($<u.decl>5, $6); }
	| notype_declarator maybeasm maybe_attribute
		{ declaration d = start_decl($1, $2, current_declspecs, 0,
					     $3, prefix_attributes);
		  $$ = finish_decl(d, NULL); }
	;
maybe_attribute:
      /* empty */
  		{ $$ = NULL; }
	| attributes
		{ $$ = $1; }
	;
 
attributes:
      attribute
		{ $$ = $1; }
	| attributes attribute
		{ $$ = attribute_chain($1, $2); }
	;

attribute:
      ATTRIBUTE '(' '(' attribute_list ')' ')'
		{ $$ = $4; }
	;

attribute_list:
      attrib
		{ $$ = $1; }
	| attribute_list ',' attrib
		{ $$ = attribute_chain($1, $3); }
	;
 
attrib:
    /* empty */
		{ $$ = NULL; }
	| any_word
		{ $$ = new_attribute(pr, $1->location, $1, NULL, NULL); }
	| any_word '(' IDENTIFIER ')'
		{ $$ = new_attribute
		    (pr, $1->location, $1, new_word(pr, $3.location, $3.id), NULL); }
	| any_word '(' IDENTIFIER ',' nonnull_exprlist ')'
		{ $$ = new_attribute
		    (pr, $2.location, $1, new_word(pr, $3.location, $3.id), $5);
		}
	| any_word '(' exprlist ')'
		{ $$ = new_attribute(pr, $2.location, $1, NULL, $3); }
	;

/* This still leaves out most reserved keywords,
   shouldn't we include them?  */

any_word:
	  idword
	| scspec { $$ = new_word(pr, $1->location, str2cstring(pr, rid_name(CAST(rid, $1)))); }
	| type_spec { $$ = new_word(pr, $1->location, str2cstring(pr, rid_name(CAST(rid, $1)))); }
	| type_qual { $$ = new_word(pr, $1->location, str2cstring(pr, qualifier_name(CAST(qualifier, $1)->id))); }
	;

/* Initializers.  `init' is the entry point.  */

init:
	expr_no_commas
	| '{'
	  initlist_maybe_comma '}'
		{ $$ = CAST(expression, new_init_list(pr, $1.location, $2)); 
		  $$->type = error_type; }
	| error
		{ $$ = make_error_expr(last_location); }
	;

/* `initlist_maybe_comma' is the guts of an initializer in braces.  */
initlist_maybe_comma:
	  /* empty */
		{ if (pedantic)
		    pedwarn("ANSI C forbids empty initializer braces"); 
		  $$ = NULL; }
	| initlist1 maybecomma { $$ = $1; }
	;

initlist1:
	  initelt
	| initlist1 ',' initelt { $$ = expression_chain($1, $3); }
	;

/* `initelt' is a single element of an initializer.
   It may use braces.  */
initelt:
	expr_no_commas { $$ = $1; }
	| '{' initlist_maybe_comma '}'
		{ $$ = CAST(expression, new_init_list(pr, $1.location, $2)); }
	| error { $$ = make_error_expr(last_location); }
	/* These are for labeled elements.  The syntax for an array element
	   initializer conflicts with the syntax for an Objective-C message,
	   so don't include these productions in the Objective-C grammar.  */
	| '[' expr_no_commas ELLIPSIS expr_no_commas ']' '=' initelt
	    	{ $$ = CAST(expression, new_init_index(pr, $1.location, $2, $4, $7)); }
	| '[' expr_no_commas ']' '=' initelt
	    	{ $$ = CAST(expression, new_init_index(pr, $1.location, $2, NULL, $5)); }
	| '[' expr_no_commas ']' initelt
	    	{ $$ = CAST(expression, new_init_index(pr, $1.location, $2, NULL, $4)); }
	| idword ':' initelt
	    	{ $$ = CAST(expression, new_init_field(pr, $1->location, $1, $3)); }
	| '.' idword '=' initelt
	    	{ $$ = CAST(expression, new_init_field(pr, $1.location, $2, $4)); }
	;

nested_function:
	  declarator
		{ if (!start_function(current_declspecs, $1,
				      prefix_attributes, 1))
		    {
		      YYERROR1;
		    }
		  }
	   old_style_parm_decls
		{ store_parm_decls($3); }
/* This used to use compstmt_or_error.
   That caused a bug with input `f(g) int g {}',
   where the use of YYERROR1 above caused an error
   which then was handled by compstmt_or_error.
   There followed a repeated execution of that same rule,
   which called YYERROR1 again, and so on.  */
	  compstmt
		{ $$ = finish_function($5); }
	;

notype_nested_function:
	  notype_declarator
		{ if (!start_function(current_declspecs, $1,
				      prefix_attributes, 1))
		    {
		      YYERROR1;
		    }
		}
	  old_style_parm_decls
		{ store_parm_decls($3); }
/* This used to use compstmt_or_error.
   That caused a bug with input `f(g) int g {}',
   where the use of YYERROR1 above caused an error
   which then was handled by compstmt_or_error.
   There followed a repeated execution of that same rule,
   which called YYERROR1 again, and so on.  */
	  compstmt
		{ $$ = finish_function($5); }
	;

/* Any kind of declarator (thus, all declarators allowed
   after an explicit typespec).  */

declarator:
	  after_type_declarator
	| notype_declarator
	;

/* A declarator that is allowed only after an explicit typespec.  */

after_type_declarator:
	  '(' after_type_declarator ')'
		{ $$ = $2; }
	| after_type_declarator '(' parmlist_or_identifiers fn_quals %prec '.'
		{ $$ = make_function_declarator($2.location, $1, $3, $4); }
	| after_type_declarator '[' expr ']'  %prec '.'
		{ $$ = CAST(declarator, new_array_declarator(pr, $2.location, $1, $3)); }
	| after_type_declarator '[' ']'  %prec '.'
		{ $$ = CAST(declarator, new_array_declarator(pr, $2.location, $1, NULL)); }
	| '*' type_quals after_type_declarator  %prec UNARY
		{ $$ = CAST(declarator, new_pointer_declarator(pr, $1.location, $3, $2)); }
	/* ??? Yuck.  setattrs is a quick hack.  We can't use
	   prefix_attributes because $1 only applies to this
	   declarator.  We assume setspecs has already been done.
	   setattrs also avoids 5 reduce/reduce conflicts (otherwise multiple
	   attributes could be recognized here or in `attributes').  */
	| attributes setattrs after_type_declarator
		{ $$ = $3; }
	| TYPENAME { $$ = CAST(declarator, new_identifier_declarator(pr, $1.location, $1.id)); }
	;

/* Kinds of declarator that can appear in a parameter list
   in addition to notype_declarator.  This is like after_type_declarator
   but does not allow a typedef name in parentheses as an identifier
   (because it would conflict with a function with that typedef as arg).  */

parm_declarator:
	  parm_declarator '(' parmlist_or_identifiers fn_quals  %prec '.'
		{ $$ = make_function_declarator($2.location, $1, $3, $4); }
	| parm_declarator '[' expr ']'  %prec '.'
		{ $$ = CAST(declarator, new_array_declarator(pr, $2.location, $1, $3)); }
	| parm_declarator '[' ']'  %prec '.'
		{ $$ = CAST(declarator, new_array_declarator(pr, $2.location, $1, NULL)); }
	| '*' type_quals parm_declarator  %prec UNARY
		{ $$ = CAST(declarator, new_pointer_declarator(pr, $1.location, $3, $2)); }
	/* ??? Yuck.  setattrs is a quick hack.  We can't use
	   prefix_attributes because $1 only applies to this
	   declarator.  We assume setspecs has already been done.
	   setattrs also avoids 5 reduce/reduce conflicts (otherwise multiple
	   attributes could be recognized here or in `attributes').  */
	| attributes setattrs parm_declarator
		{ $$ = $3; }
	| TYPENAME { $$ = CAST(declarator, new_identifier_declarator(pr, $1.location, $1.id)); }
	;

/* A declarator allowed whether or not there has been
   an explicit typespec.  These cannot redeclare a typedef-name.  */

notype_declarator:
	  notype_declarator '(' parmlist_or_identifiers fn_quals  %prec '.'
		{ $$ = make_function_declarator($2.location, $1, $3, $4); }
	| '(' notype_declarator ')'
		{ $$ = $2; }
	| '*' type_quals notype_declarator  %prec UNARY
		{ $$ = CAST(declarator, new_pointer_declarator(pr, $1.location, $3, $2)); }
	| notype_declarator '[' expr ']'  %prec '.'
		{ $$ = CAST(declarator, new_array_declarator(pr, $2.location, $1, $3)); }
	| notype_declarator '[' ']'  %prec '.'
		{ $$ = CAST(declarator, new_array_declarator(pr, $2.location, $1, NULL)); }
	/* ??? Yuck.  setattrs is a quick hack.  We can't use
	   prefix_attributes because $1 only applies to this
	   declarator.  We assume setspecs has already been done.
	   setattrs also avoids 5 reduce/reduce conflicts (otherwise multiple
	   attributes could be recognized here or in `attributes').  */
	| attributes setattrs notype_declarator
		{ $$ = $3; }
	| IDENTIFIER { $$ = CAST(declarator, new_identifier_declarator(pr, $1.location, $1.id)); }
	;

tag:
	identifier { $$ = new_word(pr, $1.location, $1.id); }
	;

structsp:
	  STRUCT tag '{'
		{ $$ = start_struct($1.location, kind_struct_ref, $2);
		  /* Start scope of tag before parsing components.  */
		}
	  component_decl_list '}' maybe_attribute 
		{ $$ = finish_struct($<u.telement>4, $5, $7); }
	| STRUCT '{' component_decl_list '}' maybe_attribute
		{ $$ = finish_struct(start_struct($1.location, kind_struct_ref, NULL),
				     $3, $5);
		}
	| STRUCT tag
		{ $$ = xref_tag($1.location, kind_struct_ref, $2); }
	| UNION tag '{'
		{ $$ = start_struct ($1.location, kind_union_ref, $2); }
	  component_decl_list '}' maybe_attribute
		{ $$ = finish_struct($<u.telement>4, $5, $7); }
	| UNION '{' component_decl_list '}' maybe_attribute
		{ $$ = finish_struct(start_struct($1.location, kind_union_ref, NULL),
				     $3, $5);
		}
	| UNION tag
		{ $$ = xref_tag($1.location, kind_union_ref, $2); }
	| ENUM tag '{'
		{ $$ = start_enum($1.location, $2); }
	  enumlist maybecomma_warn '}' maybe_attribute
		{ $$ = finish_enum($<u.telement>4, $5, $8); }
	| ENUM '{'
		{ $$ = start_enum($1.location, NULL); }
	  enumlist maybecomma_warn '}' maybe_attribute
		{ $$ = finish_enum($<u.telement>3, $4, $7); }
	| ENUM tag
		{ $$ = xref_tag($1.location, kind_enum_ref, $2); }
	;

maybecomma:
	  /* empty */
	| ','
	;

maybecomma_warn:
	  /* empty */
	| ','
		{ if (pedantic) pedwarn("comma at end of enumerator list"); }
	;

component_decl_list:
	  component_decl_list2
		{ $$ = $1; }
	| component_decl_list2 component_decl
		{ $$ = declaration_chain($1, $2);
		  pedwarn("no semicolon at end of struct or union"); }
	;

component_decl_list2:	/* empty */
		{ $$ = NULL; }
	| component_decl_list2 component_decl ';'
		{ $$ = declaration_chain($1, $2); }
	| component_decl_list2 ';'
		{ if (pedantic)
		    pedwarn("extra semicolon in struct or union specified"); 
		   $$ = $1; }
	;

/* There is a shift-reduce conflict here, because `components' may
   start with a `typename'.  It happens that shifting (the default resolution)
   does the right thing, because it treats the `typename' as part of
   a `typed_typespecs'.

   It is possible that this same technique would allow the distinction
   between `notype_initdecls' and `initdecls' to be eliminated.
   But I am being cautious and not trying it.  */

component_decl:
	  typed_typespecs setspecs components
		{ $$ = CAST(declaration, new_data_decl(pr, $1->location, current_declspecs, prefix_attributes, $3));
		  pop_declspec_stack(); }
	| typed_typespecs setspecs
		{ if (pedantic)
		    pedwarn("ANSI C forbids member declarations with no members");
		  shadow_tag($1);
		  $$ = CAST(declaration, new_data_decl(pr, $1->location, current_declspecs, prefix_attributes, NULL));
		  pop_declspec_stack(); }
	| nonempty_type_quals setspecs components
		{ $$ = CAST(declaration, new_data_decl(pr, $1->location, current_declspecs, prefix_attributes, $3));
		  pop_declspec_stack(); }
	| nonempty_type_quals setspecs
		{ if (pedantic)
		    pedwarn("ANSI C forbids member declarations with no members");
		  shadow_tag($1);
		  $$ = CAST(declaration, new_data_decl(pr, $1->location, current_declspecs, prefix_attributes, NULL));	
		  pop_declspec_stack(); }
	| error
		{ $$ = new_error_decl(pr, last_location); }
	| extension component_decl
		{ pedantic = $<u.itoken>1.i;
		  $$ = CAST(declaration, new_extension_decl(pr, $1.location, $2)); }
	;

components:
	  component_declarator
	| components ',' component_declarator
		{ $$ = declaration_chain($1, $3); }
	;

component_declarator:
	  declarator maybe_attribute
		{ $$ = make_field($1, NULL, current_declspecs,
				  $2, prefix_attributes); }
	| declarator ':' expr_no_commas maybe_attribute
		{ $$ = make_field($1, $3, current_declspecs,
				  $4, prefix_attributes); }
	| ':' expr_no_commas maybe_attribute
		{ $$ = make_field(NULL, $2, current_declspecs,
				  $3, prefix_attributes); }
	;

enumlist:
	  enumerator
	| enumlist ',' enumerator
		{ $$ = declaration_chain($1, $3); }
	| error
		{ $$ = NULL; }
	;


enumerator:
	  identifier
	  	{ $$ = make_enumerator($1.location, $1.id, NULL); }
	| identifier '=' expr_no_commas
	  	{ $$ = make_enumerator($1.location, $1.id, $3); }
	;

typename:
	typed_typespecs absdcl
		{ $$ = make_type($1, $2); }
	| nonempty_type_quals absdcl
		{ $$ = make_type($1, $2); }
	;

absdcl:   /* an abstract declarator */
	/* empty */
		{ $$ = NULL; }
	| absdcl1
	;

nonempty_type_quals:
	  type_qual
	| nonempty_type_quals type_qual
		{ $$ = type_element_chain($1, $2); }
	;

type_quals:
	  /* empty */
		{ $$ = NULL; }
	| type_quals type_qual
		{ $$ = type_element_chain($1, $2); }
	;

absdcl1:  /* a nonempty abstract declarator */
	  '(' absdcl1 ')'
		{ $$ = $2; }
	  /* `(typedef)1' is `int'.  */
	| '*' type_quals absdcl1  %prec UNARY
		{ $$ = CAST(declarator, new_pointer_declarator(pr, $1.location, $3, $2)); }
	| '*' type_quals  %prec UNARY
		{ $$ = CAST(declarator, new_pointer_declarator(pr, $1.location, NULL, $2)); }
	| absdcl1 '(' parmlist fn_quals  %prec '.'
		{ $$ = make_function_declarator($2.location, $1, $3, $4); }
	| absdcl1 '[' expr ']'  %prec '.'
		{ $$ = CAST(declarator, new_array_declarator(pr, $2.location, $1, $3)); }
	| absdcl1 '[' ']'  %prec '.'
		{ $$ = CAST(declarator, new_array_declarator(pr, $2.location, $1, NULL)); }
	| '(' parmlist fn_quals  %prec '.'
		{ $$ = make_function_declarator($1.location, NULL, $2, $3); }
	| '[' expr ']'  %prec '.'
		{ $$ = CAST(declarator, new_array_declarator(pr, $1.location, NULL, $2)); }
	| '[' ']'  %prec '.'
		{ $$ = CAST(declarator, new_array_declarator(pr, $1.location, NULL, NULL)); }
	/* ??? It appears we have to support attributes here, however
	   using prefix_attributes is wrong.  */
	;

/* at least one statement, the first of which parses without error.  */
/* stmts is used only after decls, so an invalid first statement
   is actually regarded as an invalid decl and part of the decls.  */

stmts:
	stmt_or_labels
		{
		  if (pedantic && $1.i)
		    pedwarn("ANSI C forbids label at end of compound statement");
		  /* Add an empty statement to last label if stand-alone */
		  if ($1.i)
		    {
		      statement last_label = CAST(statement, last_node(CAST(node, $1.stmt)));

		      chain_with_labels(last_label, CAST(statement, new_empty_stmt(pr, last_label->location)));
		    }
		  $$ = $1.stmt;
		}
	;

stmt_or_labels:
	  stmt_or_label
	| stmt_or_labels stmt_or_label
		{ $$.i = $2.i; $$.stmt = chain_with_labels($1.stmt, $2.stmt); }
	| stmt_or_labels errstmt
		{ $$.i = 0; $$.stmt = new_error_stmt(pr, last_location); }
	;

xstmts:
	/* empty */ { $$ = NULL; }
	| stmts
	;

errstmt:  error ';'
	;

pushlevel:  /* empty */
		{ pushlevel(FALSE); }
	;

/* Read zero or more forward-declarations for labels
   that nested functions can jump to.  */
maybe_label_decls:
	  /* empty */ { $$ = NULL; }
	| label_decls
		{ if (pedantic)
		    pedwarn("ANSI C forbids label declarations"); 
		  $$ = $1; }
	;

label_decls:
	  label_decl
	| label_decls label_decl { $$ = id_label_chain($1, $2); }
	;

label_decl:
	  LABEL identifiers_or_typenames ';'
		{ $$ = $2; }
	;

/* This is the body of a function definition.
   It causes syntax errors to ignore to the next openbrace.  */
compstmt_or_error:
	  compstmt
	| error compstmt { $$ = $2; }
	;

compstmt_start: '{' { $$ = $1; compstmt_count++; }

compstmt: compstmt_start pushlevel '}'
		{ $$ = CAST(statement, new_compound_stmt(pr, $1.location, NULL, NULL, NULL, poplevel())); }
	| compstmt_start pushlevel maybe_label_decls decls xstmts '}'
		{ $$ = CAST(statement, new_compound_stmt(pr, $1.location, $3, $4, $5, poplevel())); }
	| compstmt_start pushlevel maybe_label_decls error '}'
		{ poplevel();
		  $$ = new_error_stmt(pr, last_location); }
	| compstmt_start pushlevel maybe_label_decls stmts '}'
		{ $$ = CAST(statement, new_compound_stmt(pr, $1.location, $3, NULL, $4, poplevel())); }
	;

/* Value is number of statements counted as of the closeparen.  */
simple_if:
	  if_prefix labeled_stmt
		{ $$.stmt = CAST(statement, new_if_stmt(pr, $1.expr->location, $1.expr, $2, NULL));
		  $$.i = $1.i; }
	| if_prefix error { $$.i = $1.i; $$.stmt = new_error_stmt(pr, last_location); }
	;

if_prefix:
	  IF '(' expr ')'
		{ $$.i = stmt_count;
		  $$.expr = $3;
		  check_condition("if", $3); }
	;

/* This is a subroutine of stmt.
   It is used twice, once for valid DO statements
   and once for catching errors in parsing the end test.  */
do_stmt_start:
	  DO
		{ stmt_count++;
		  compstmt_count++; 
		  $<u.cstmt>$ = CAST(conditional_stmt,
				   new_dowhile_stmt(pr, $1.location, NULL, NULL));
		 push_loop(CAST(statement, $<u.cstmt>$)); }
	  labeled_stmt WHILE
		{ $$ = $<u.cstmt>2; 
		  $$->stmt = $3; }
	;

labeled_stmt:
	  stmt
		{ $$ = $1; }
	| label labeled_stmt
		{ $$ = CAST(statement, new_labeled_stmt(pr, $1->location, $1, $2)); }
	;

stmt_or_label:
	  stmt
		{ $$.i = 0; $$.stmt = $1; }
	| label
		{ $$.i = 1; $$.stmt = CAST(statement, new_labeled_stmt(pr, $1->location, $1, NULL)); }
	;

/* Parse a single real statement, not including any labels.  */
stmt:
	  compstmt
		{ stmt_count++; $$ = $1; }
	| expr ';'
		{ stmt_count++;
		  $$ = CAST(statement, new_expression_stmt(pr, $1->location, $1)); }
	| simple_if ELSE
		{ $1.i = stmt_count; }
	  labeled_stmt
		{ if (extra_warnings && stmt_count == $1.i)
		    warning("empty body in an else-statement");
		  $$ = $1.stmt;
		  CAST(if_stmt, $$)->stmt2 = $4;
		}
	| simple_if %prec IF
		{ /* This warning is here instead of in simple_if, because we
		     do not want a warning if an empty if is followed by an
		     else statement.  Increment stmt_count so we don't
		     give a second error if this is a nested `if'.  */
		  if (extra_warnings && stmt_count++ == $1.i)
		    warning_with_location ($1.stmt->location,
					   "empty body in an if-statement");
		  $$ = $1.stmt; }
	| simple_if ELSE error
		{ $$ = new_error_stmt(pr, last_location); }
	| WHILE
		{ stmt_count++; }
	  '(' expr ')' 
	        { check_condition("while", $4); 
		  $<u.cstmt>$ = CAST(conditional_stmt,
			           new_while_stmt(pr, $1.location, $4, NULL));
		  /* The condition is not "in the loop" for break or continue */
		  push_loop(CAST(statement, $<u.cstmt>$)); }
	  labeled_stmt
		{ $$ = CAST(statement, $<u.cstmt>6);
		  $<u.cstmt>6->stmt = $7; 
		  pop_loop(); }
	| do_stmt_start '(' expr ')' ';'
		{ $$ = CAST(statement, $1);
		  $1->condition = $3;
		  check_condition("do-while", $3); 
		  /* Note that pop_loop should be before the expr to be consistent
		     with while, but GCC is inconsistent. See loop1.c */
		  pop_loop(); }
	| do_stmt_start error
		{ $$ = new_error_stmt(pr, last_location); 
		  pop_loop(); }
	| FOR '(' xexpr ';' { stmt_count++; }
		xexpr ';' { if ($6) check_condition("for", $6); }
		xexpr ')' 
		{ $<u.for_stmt>$ = new_for_stmt(pr, $1.location, $3, $6, $9, NULL);
		  push_loop(CAST(statement, $<u.for_stmt>$)); }
		labeled_stmt
		{ $$ = CAST(statement, $<u.for_stmt>11);
		  $<u.for_stmt>11->stmt = $12; 
		  pop_loop(); }
	| SWITCH '(' expr ')'
	        { stmt_count++; check_switch($3); 
		  $<u.cstmt>$ = CAST(conditional_stmt,
			           new_switch_stmt(pr, $1.location, $3, NULL)); 
		  push_loop(CAST(statement, $<u.cstmt>$)); } 
	  labeled_stmt
		{ $$ = CAST(statement, $<u.cstmt>5); 
		  $<u.cstmt>5->stmt = $6;
		  pop_loop(); }
	| BREAK ';'
		{ stmt_count++;
		  $$ = CAST(statement, new_break_stmt(pr, $1.location));
		  check_break($$);
		}
	| CONTINUE ';'
		{ stmt_count++;
		  $$ = CAST(statement, new_continue_stmt(pr, $1.location));
		  check_continue($$);
		}
	| RETURN ';'
		{ stmt_count++;
		  $$ = CAST(statement, new_return_stmt(pr, $1.location, NULL)); 
		  check_void_return(); }
	| RETURN expr ';'
		{ stmt_count++;
		  $$ = CAST(statement, new_return_stmt(pr, $1.location, $2)); 
		  check_return($2); }
	| ASM_KEYWORD maybe_type_qual '(' expr ')' ';'
		{ stmt_count++;
		  $$ = CAST(statement, new_asm_stmt(pr, $1.location, $4, NULL,
					       NULL, NULL, $2)); }
	/* This is the case with just output operands.  */
	| ASM_KEYWORD maybe_type_qual '(' expr ':' asm_operands ')' ';'
		{ stmt_count++;
		  $$ = CAST(statement, new_asm_stmt(pr, $1.location, $4, $6, NULL,
					       NULL, $2)); }
	/* This is the case with input operands as well.  */
	| ASM_KEYWORD maybe_type_qual '(' expr ':' asm_operands ':' asm_operands ')' ';'
		{ stmt_count++;
		  $$ = CAST(statement, new_asm_stmt(pr, $1.location, $4, $6, $8, NULL, $2)); }
	/* This is the case with clobbered registers as well.  */
	| ASM_KEYWORD maybe_type_qual '(' expr ':' asm_operands ':'
  	  asm_operands ':' asm_clobbers ')' ';'
		{ stmt_count++;
		  $$ = CAST(statement, new_asm_stmt(pr, $1.location, $4, $6, $8, $10, $2)); }
	| GOTO id_label ';'
		{ stmt_count++;
		  $$ = CAST(statement, new_goto_stmt(pr, $1.location, $2));
		  use_label($2);
		}
	| GOTO '*' expr ';'
		{ if (pedantic)
		    pedwarn("ANSI C forbids `goto *expr;'");
		  stmt_count++;
		  $$ = CAST(statement, new_computed_goto_stmt(pr, $1.location, $3)); 
		  check_computed_goto($3); }
	| ';' { $$ = CAST(statement, new_empty_stmt(pr, $1.location)); }
	;

/* Any kind of label, including jump labels and case labels.
   ANSI C accepts labels only before statements, but we allow them
   also at the end of a compound statement.  */

label:	  CASE expr_no_commas ':'
		{ $$ = CAST(label, new_case_label(pr, $1.location, $2, NULL)); 
		  check_case($$); }
	| CASE expr_no_commas ELLIPSIS expr_no_commas ':'
		{ $$ = CAST(label, new_case_label(pr, $1.location, $2, $4)); 
		  check_case($$); }
	| DEFAULT ':'
		{ $$ = CAST(label, new_default_label(pr, $1.location)); 
		  check_default($$); }
	| id_label ':'
		{ $$ = CAST(label, $1); 
		  define_label($1); }
	;

/* Either a type-qualifier or nothing.  First thing in an `asm' statement.  */

maybe_type_qual:
	/* empty */
		{ $$ = NULL; }
	| type_qual
	;

xexpr:
	/* empty */
		{ $$ = NULL; }
	| expr
	;

/* These are the operands other than the first string and colon
   in  asm ("addextend %2,%1": "=dm" (x), "0" (y), "g" (*x))  */
asm_operands: /* empty */
		{ $$ = NULL; }
	| nonnull_asm_operands
	;

nonnull_asm_operands:
	  asm_operand
	| nonnull_asm_operands ',' asm_operand
		{ $$ = asm_operand_chain($1, $3); }
	;

asm_operand:
	  STRING '(' expr ')'
		{ $$ = new_asm_operand(pr, $1->location,
				       make_string($1->location, CAST(expression, $1)),
				       $3);  }
	;

asm_clobbers:
	  string
		{ $$ = $1; }
	| asm_clobbers ',' string
		{ $$ = string_chain($1, $3); }
	;

/* This is what appears inside the parens in a function declarator.
   Its value is a list of ..._TYPE nodes.  */
parmlist:
		{ pushlevel(TRUE); }
	  parmlist_1
		{ $$ = $2;
		  /* poplevel() is done when building the declarator */
		}
	;

parmlist_1:
	  parmlist_2 ')' { $$ = $1; }
	| parms ';'
		{ if (pedantic)
		    pedwarn("ANSI C forbids forward parameter declarations");
#if 0
		  /* Mark the forward decls as such.  */
		  for (parm = getdecls (); parm; parm = TREE_CHAIN (parm))
		    TREE_ASM_WRITTEN (parm) = 1;
#endif
		  }
	  parmlist_1
		{ $$ = $4; }
	| error ')'
		{ $$ = new_error_decl(pr, last_location); }
	;

/* This is what appears inside the parens in a function declarator.
   Is value is represented in the format that grokdeclarator expects.  */
parmlist_2:  /* empty */
		{ $$ = NULL; }
	| ELLIPSIS
		{ $$ = new_error_decl(pr, last_location);
		  /* Gcc used to allow this as an extension.  However, it does
		     not work for all targets, and thus has been disabled.
		     Also, since func (...) and func () are indistinguishable,
		     it caused problems with the code in expand_builtin which
		     tries to verify that BUILT_IN_NEXT_ARG is being used
		     correctly.  */
		  error("ANSI C requires a named argument before `...'");
		}
	| parms
		{ $$ = $1; }
	| parms ',' ELLIPSIS
		{ $$ = declaration_chain($1, CAST(declaration, new_ellipsis_decl(pr, $3.location))); }
	;

parms:
	parm
	| parms ',' parm
		{ $$ = declaration_chain($1, $3); }
	;

/* A single parameter declaration or parameter type name,
   as found in a parmlist.  */
parm:
	  typed_declspecs setspecs parm_declarator maybe_attribute
		{ $$ = declare_parameter($3, current_declspecs, $4,
					 prefix_attributes, FALSE);
		  pop_declspec_stack(); }
	| typed_declspecs setspecs notype_declarator maybe_attribute
		{ $$ = declare_parameter($3, current_declspecs, $4,
					 prefix_attributes, FALSE);
		  pop_declspec_stack(); }
	| typed_declspecs setspecs absdcl maybe_attribute
		{ $$ = declare_parameter($3, current_declspecs, $4,
					 prefix_attributes, TRUE);
		pop_declspec_stack(); }
	| declmods setspecs notype_declarator maybe_attribute
		{ $$ = declare_parameter($3, current_declspecs, $4,
					 prefix_attributes, FALSE);
		  pop_declspec_stack(); }
	| declmods setspecs absdcl maybe_attribute
		{ $$ = declare_parameter($3, current_declspecs, $4,
					 prefix_attributes, TRUE);
		  pop_declspec_stack(); }
	;

/* This is used in a function definition
   where either a parmlist or an identifier list is ok.
   Its value is a list of ..._TYPE nodes or a list of identifiers.  */
parmlist_or_identifiers:
		{ pushlevel(TRUE); }
	  parmlist_or_identifiers_1
		{ $$ = $2;
		  /* poplevel is done when building the declarator */ }
	;

parmlist_or_identifiers_1:
	  parmlist_1
	| identifiers ')' { $$ = $1; }
	;

/* A nonempty list of identifiers.  */
identifiers:
	old_parameter
		{ $$ = $1; }
	| identifiers ',' old_parameter
		{ $$ = declaration_chain($1, $3); }
	;

old_parameter:
	IDENTIFIER { $$ = declare_old_parameter($1.location, $1.id); }
	;

/* A nonempty list of identifiers, including typenames.  */
identifiers_or_typenames:
	id_label { $$ = $1; declare_label($1); }
	| identifiers_or_typenames ',' id_label
		{ $$ = id_label_chain($1, $3);
		  declare_label($3); }
	;

/* A possibly empty list of function qualifiers (only one exists so far) */
fn_quals:
	/* empty */ { $$ = NULL; }
	| fn_qual { $$ = $1; }
	;

extension:
	EXTENSION
		{ $$.location = $1.location;
		  $$.i = pedantic;
		  pedantic = 0; }
	;

scspec:
	SCSPEC { $$ = CAST(type_element, new_rid(pr, $1.location, $1.i)); }
	;

type_qual:
	TYPE_QUAL { $$ = CAST(type_element, new_qualifier(pr, $1.location, $1.i)); }
	;

fn_qual:
	FN_QUAL { $$ = CAST(type_element, new_qualifier(pr, $1.location, $1.i)); }
	;

type_spec:
	TYPESPEC { $$ = CAST(type_element, new_rid(pr, $1.location, $1.i)); }
	;


%%
