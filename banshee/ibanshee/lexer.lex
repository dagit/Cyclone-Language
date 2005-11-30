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

%{
  /* IBanshee lexer */
  #include <stdio.h>
  #include <string.h>
  #include "nonspec.h"
  #include "regions.h"
  #include "parser.tab.h"
  #include "hash.h"
  
%}

%option always-interactive
%option noyywrap

WHITESPACE ['\t' ' ']
LINE ['\n''\r']
UPPERCASE [A-Z]
LOWERCASE [a-z]
DIGIT [0-9]
DIGITS ({DIGIT}+)
IDENT [A-Za-z][A-Za-z0-9_]*
TICK [\']
%%
<<EOF>>     return TOK_EOF;
{LINE}      return TOK_LINE;
"setIF"     return TOK_SETIF;
"term"      return TOK_TERM;
"flow"      return TOK_FLOW;
"row"       return TOK_ROW;
"proj"      return TOK_PROJ;
"pat"       return TOK_PAT;
"("         return TOK_LPAREN;
")"         return TOK_RPAREN;
"<"         return TOK_LANGLE;
">"         return TOK_RANGLE;
"|"         return TOK_REST;
"+"         return TOK_POS;
"-"         return TOK_NEG;
"="         return TOK_EQ;
"#"         return TOK_DECL;
","         return TOK_COMMA;
"_"         return TOK_WILD;
":"         return TOK_COLON;
"!"         return TOK_CMD;
"\""        return TOK_QUOTE;
"^"         return TOK_CARET;
"&&"        return TOK_INTER;
"||"        return TOK_UNION;
"<="        return TOK_LEQ;
"=="        return TOK_DEQ;
{TICK}{IDENT} {yylval.str = strdup(yytext); return TOK_VAR; }
{IDENT} { yylval.str = strdup(yytext); return TOK_IDENT; }
{DIGITS} { yylval.num = atoi(yytext); return TOK_INTEGER; }
[ \t]+				/* ignore whitespace */
.
%%

yyerror( char *msg ) { fprintf( stderr, "%s\n", msg ); }

void flush_lexer() { YY_FLUSH_BUFFER; }
