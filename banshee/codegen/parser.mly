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
open Engspec
let r x = List.rev x
%}
%token EOF
%token <string> IDENT
%token <Engspec.sort_gen> SORT
%token POS NEG OR COLON EQ LPAREN RPAREN CROSS
%token SPECIFICATION SPEC END DATA OF AND 
%start engspec
%type <Engspec.engspec> engspec
%type <Engspec.dataspec list> dataspec_list
%type <Engspec.dataspec> dataspec 
%type <Engspec.dataspec> and_bindings
%type <Engspec.exprid * Engspec.sort_gen * Engspec.databody> and_elt
%type <Engspec.databody> conspec_option
%type <Engspec.databody> conspec
%type <Engspec.conid *Engspec.consig option> conid
%type <Engspec.bconsig list> consig
%type <Engspec.bconsig> bconsig

%%

engspec :
  SPECIFICATION IDENT COLON IDENT EQ SPEC dataspec_list END EOF {($2,$4,r $7) }
;

dataspec_list : dataspec { [$1] }
        | dataspec_list dataspec { (r $2)::$1 }
;

dataspec : 
          DATA IDENT COLON SORT conspec_option { [($2,$4,$5)] }
        | DATA IDENT COLON SORT conspec_option and_bindings { ($2,$4,$5)::$6}
;

and_bindings :
          and_elt { [$1] }
        | and_bindings and_elt { $2::$1 }
;

and_elt :
          AND IDENT COLON SORT conspec_option { ($2,$4,$5) }
;

conspec_option :
        /* EMPTY */     { [] }
	| EQ conspec    { r $2  }
;

conspec :
          conid { [$1] }
	| conspec OR conid { $3::$1 }
;

conid :
	| IDENT OF consig { ($1,Some (r $3) ) }
	| IDENT           { ($1,None) }
;

consig :  bconsig { [$1] }
	| consig CROSS bconsig { $3::$1 }
;

bconsig : 
	  POS IDENT { ($2,POSvariance) }
	| NEG IDENT { ($2,NEGvariance)  }
        | IDENT { ($1,NOvariance)  }
