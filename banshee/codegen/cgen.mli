(*
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
 *)

type decl
type def

type expr = string
type ident = string
      
type qual = Const | Volatile | Deletes | Sameregion | ParentPtr | Traditional
  
type ctype = 
    Struct of ident
  | Pointer of qctype
  | Int
  | Float
  | Char
  | Enum of ident
  | Void
  | Ident of ident
and qctype = ctype * qual list
      
type storage = 
    Typedef
  | Extern
  | Static
  | Auto
  | Register

type declarator = 
    DExtern
  | DStatic
  | DInline
      
type statement = 
    Return of expr
  | Compound of statement list
  | Condition of expr * statement * statement
  | Expr of expr
  | Switch of expr * (expr * statement) list * statement
	
type actuals = ident list
type formals = (qctype * ident) list
type body = statement list
type func = qctype * ident * formals * body * (qual list)
      
class type header = 
  object
    method add_include : decl -> unit
    method add_includes : decl list -> unit
    method add_macro : decl -> unit
    method add_macros : decl list -> unit
    method add_tdecl : decl -> unit
    method add_tdecls : decl list -> unit
    method add_gdecl : decl -> unit
    method add_gdecls : decl list -> unit
    method print : out_channel -> unit
  end
      
class type file = 
  object
    inherit header
    method add_fdef : def -> unit
    method add_fdefs : def list -> unit
  end
    
val decl_substitution : (string * string) list -> string -> decl  
val comment : string -> decl
val block_comment : string ->decl 
val blank : decl
val prototype : ?dec:(declarator) -> func -> decl
val typedef : qctype -> ident -> decl
val opaque : ident -> decl
val define : string -> decl
val macro : string -> decl
val define_val : string -> string -> decl
val ifdef : string -> decl
val ifndef : string -> decl
val include_header : bool -> string -> decl
val endif : decl
val compound_decl : decl list -> decl
val line : decl
val var : ?init:(expr) -> qctype -> ident -> storage option -> decl
val struct_decl : ident -> (qctype * ident) list -> decl
val qctype_to_string : qctype -> string

(* definitions *)
val def_substitution : (string * string) list -> string -> def
val func : ?dec:(declarator) -> func -> def
val compound_def : def list -> def 

(* printing *)
val decl_to_string : decl -> string
val def_to_string : def -> string
val out_decl : out_channel -> decl -> unit
val out_def : out_channel -> def -> unit

(* utils *)
val no_qual : ctype -> qctype
val const :  ctype -> qctype
val void : qctype
    
val empty_header : header
val empty_file : file
    
    
   
