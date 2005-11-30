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

open Str

type decl = string
type def = string
      
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
      
type statement = 
    Return of expr
  | Compound of statement list
  | Condition of expr * statement * statement
  | Expr of expr
  | Switch of expr * (expr * statement) list * statement
	
type declarator = 
    DExtern
  | DStatic
  | DInline

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
      
      
      
let foldr f =
  let rec localfun y l =
    match l with [] -> y 
    |   x::xs -> f(x, (localfun y xs))
  in localfun
    
let sp = " "
let lf = "\n"
let lff = "\n\n"
let semi = ";"
    
let parens s = "(" ^ s ^ ")"
let brace s = "{\n" ^ s ^ "\n}"
let quote s = "\"" ^ s ^ "\""
let brackets s = "<" ^ s ^ ">"
let block s = "\n{\n" ^ s ^ "\n}\n"
let block_semi s = "\n{" ^ s ^ "\n};"
	
let substitute mapping str = 
  let substitute ((old_str,new_str),target) = 
    global_replace (regexp_string old_str) new_str target
  in foldr substitute str mapping
		
let decl_substitution = substitute

let def_substitution = substitute
	   
let comment s = "/* " ^ s ^ " */"
let block_comment s = "/*" ^ lff ^ s ^ lff ^ "*/"
let blank = ""
    
let no_qual typ = (typ,[])
let const typ = (typ,[Const])
let void = (Void,[])
    
let qual_to_string = function
  | Const -> "const"
  | Volatile -> "volatile"
  | Deletes -> ""
  | Sameregion -> "sameregion"
  | ParentPtr -> "parentptr"
  | Traditional -> "traditional"
	
let quals_to_string =
  let rec aux (list : qual list) (acc : string) = match list with
  | h :: t -> aux t (acc ^ sp ^ (qual_to_string h))
  | [] -> acc
  in function 
    | h :: t -> aux t (qual_to_string h)
    | [] -> "" 
	  
let qctype_to_string typ = 
  let rec ctype_to_string (ctype,quals) suffix = 
    let base = match ctype with
    | Struct name -> "struct" ^ sp ^ name ^ suffix
    | Int -> "int" ^ suffix
    | Float -> "float"  ^ suffix
    | Enum name -> "enum" ^ sp ^ name ^ suffix
    | Void -> "void" ^ suffix
    | Ident name -> name  ^ suffix
    | Char -> "char" ^ suffix
    | _ -> ""
    in
    let qstr = quals_to_string quals in
    if (qstr = "") then base else qstr ^ sp ^ base
  and pointers (ctype,quals) suffix = match ctype with
  | Pointer t -> pointers t suffix ^ "*" ^ quals_to_string quals
  | _ -> ctype_to_string (ctype,quals) suffix
  in pointers typ ""
    
let rec formals_to_string = 
  let formal_to_string (typ,name) = (qctype_to_string typ) ^ sp ^ name
  in function 
    | [h] -> formal_to_string h
    | h :: t -> (formal_to_string h) ^ "," ^ (formals_to_string t)
    | [] -> "void"
	  
let storage_to_string = function
  | Typedef -> "typedef"
  | Extern -> "extern"
  | Static -> "static"
  | Auto -> "auto"
  | Register -> "register"
	
	
let rec var ?(init = "") vtype vname storage  =
  let expr_option e = if (e = "") then ";" else " = " ^ e ^ ";"
  in
  match storage with 
  | None ->
      (qctype_to_string vtype) ^ sp ^ vname ^ (expr_option init)
  | (Some st) ->
      storage_to_string st ^ sp ^ (qctype_to_string vtype) ^ sp ^ vname ^ 
      (expr_option init)
							     
let prototype ?(dec = DExtern) (ret,name,args,_,quals) = 
  let qstr' = quals_to_string (quals) in
  let qstr = if (qstr' = "") then "" else (sp ^ qstr')in 
  match dec with
  | DExtern ->
      qctype_to_string ret ^ sp ^ name ^ (parens (formals_to_string args))
      ^ qstr ^ semi
  | DStatic ->
      "static" ^ sp ^ (qctype_to_string ret) ^ sp ^ name ^ 
      (parens (formals_to_string args)) ^ qstr ^ semi
  | DInline ->
      "inline" ^ sp ^ (qctype_to_string ret) ^ sp ^ name ^ 
      (parens (formals_to_string args)) ^ qstr ^ semi 
									   
let typedef from_type to_type = var  from_type to_type (Some Typedef)
    
let opaque name = 
  var  (no_qual (Pointer (no_qual (Struct (name ^ "_"))))) name (Some Typedef)
    
let define name = "#define" ^ sp ^ name
				     
let define_val name1 name2 = "#define" ^ sp ^ name1 ^ sp ^ name2

let macro s = s				
			
let ifdef name = "#ifdef" ^ sp ^ name
				   
let ifndef name = "#ifndef" ^ sp ^ name
				     
let include_header is_local name = 
  if (is_local) then ("#include " ^ (quote name)) 
  else ("#include " ^ (brackets name))
      
let endif = "#endif"
    
let compound_decl l = foldr (function x,y -> x ^ lf ^ y) "" l
    
let line = lf
    
let struct_decl id members = 
  let build_body ((typ,name),acc) = 
    lf ^(qctype_to_string typ) ^ sp ^ name ^ semi  ^ acc
  in "struct" ^ sp ^ id ^ (block_semi (foldr build_body "" members))
	
let rec statement_to_string = function 
  | Return e -> 
      "return" ^ sp ^ e ^ semi
  | Compound stmts -> 
      brace (foldr (function (x,y) -> statement_to_string x ^ lf ^ y) "" stmts)
  | Condition (guard,t,f) -> 
      "if " ^ (parens guard) ^ lf ^  (statement_to_string t) ^ lf
      ^ "else " ^ lf ^  (statement_to_string f)
  | Expr e -> e
  | Switch (e,cases,def) ->   
      let case_to_string (e,s) = 
	"case " ^ e ^ ":\n" ^ statement_to_string s ^ "\nbreak;" in
      let aux (x,y) = case_to_string x ^ lf ^ y in
      "switch" ^ (parens e) ^ 
      block ( (foldr aux "" cases) ^ "\ndefault:\n" ^ statement_to_string def )
					    
let func  ?(dec = DExtern) (return,name,args,body,quals) = 
  let body' = 
    foldr (function (x,y) -> x ^ lf ^ y) "" (List.map statement_to_string body)
  in match dec with 
  | DExtern ->
      qctype_to_string return ^ sp ^ name ^ parens (formals_to_string args) ^ 
      " " ^ (quals_to_string quals) ^ "\n{\n" ^ body' ^ "}\n"
  | DStatic ->
      "static" ^ sp ^ qctype_to_string return ^ sp ^ name ^ 
      parens (formals_to_string args) ^" " ^ (quals_to_string quals) 
      ^ "\n{\n" ^ body' ^ "}\n"
  | DInline -> 
      "inline" ^ qctype_to_string return ^ sp ^ name ^ 
      parens (formals_to_string args) ^ " " ^ (quals_to_string quals) ^
      "\n{\n" ^ body' ^ "}\n"
									    
let compound_def l =  foldr (function x,y -> x ^ lf ^ y) "" l
    
let decl_to_string decl = decl
let def_to_string def = def
let out_decl channel decl = output_string channel decl
let out_def channel decl = output_string channel decl
    
class c_header =
  object
    val mutable includes = ([] : decl list)
    val mutable macros = ([] : decl list)
    val mutable tdecls = ([] : decl list)
    val mutable gdecls = ([] : decl list)
    method add_include inc = includes <- includes @ [inc]
    method add_includes inc = includes <- includes @ inc
    method add_macro m = macros <- macros @ [m]
    method add_macros m = macros <- macros @ m
    method add_tdecl td = tdecls <- tdecls @ [td] 
    method add_tdecls td = tdecls <- tdecls @ td
    method add_gdecl gd = gdecls <- gdecls @ [gd]
    method add_gdecls gd = gdecls <- gdecls @ gd
    method print channel = (out_decl channel (compound_decl includes);
			    out_decl channel line;
			    out_decl channel (compound_decl macros);
			    out_decl channel line;
			    out_decl channel (compound_decl tdecls);
			    out_decl channel line;
			    out_decl channel (compound_decl gdecls);)
  end
    
class c_file =
  object
    inherit c_header
    val mutable defs = ([] : def list)
    method add_fdef fd = defs <- defs @ [fd]
    method add_fdefs fd = defs <- defs @ fd
    method print channel = (out_decl channel (compound_decl includes);
			    out_decl channel line;
			    out_decl channel (compound_decl macros);
			    out_decl channel line;
			    out_decl channel (compound_decl tdecls);
			    out_decl channel line;
			    out_decl channel (compound_decl gdecls);
			    out_decl channel line;
			    out_def channel (compound_def defs))
  end    
    
let empty_header = (new c_header :> header)
let empty_file = (new c_file :> file)
    
    
