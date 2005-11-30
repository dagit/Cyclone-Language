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

open Cgen
open Engspec
open Spec_to_c 

let foldr f =
  let rec localfun y l =
    match l with [] -> y 
    |   x::xs -> f(x, (localfun y xs))
  in localfun

let foldl f = 
  let rec localfun y l =
    match l with [] -> y
    |  x::xs -> localfun (f(x,y)) xs 
  in localfun 

let parens s = "(" ^ s ^ ")"

let gen_proto_and_fun ?(dec:declarator = DExtern) ?(quals:qual list = []) (ret,name,args,body) = 
  (prototype  ~dec:dec (ret,name,args,body,quals),
	      func ~dec:dec (ret,name,args,body,quals))

let int_to_string x = Int32.to_string (Int32.of_int x)

let args l = 
  let rec args_aux l n = match l with
  | h :: t -> (h,"arg" ^ Int32.to_string n)::(args_aux t (Int32.succ n))
  | [] -> []
  in
  args_aux l Int32.one

let fields l =
  let rec fields_aux l n = match l with
  | h :: t -> (h,"f" ^ Int32.to_string n)::(fields_aux t (Int32.succ n))
  | [] -> [] 
  in
  [(no_qual Int, "type");(no_qual (Ident "stamp"), "st")] @ 
  (fields_aux l Int32.zero) 

let decon_fields l =
  let rec fields_aux l n = match l with
  | h :: t -> (h,"f" ^ Int32.to_string n)::(fields_aux t (Int32.succ n))
  | [] -> [] 
  in
  (fields_aux l Int32.zero)

let decon_struct arity = 
  let str = ref "c->f0" in 
  for i = 1 to (arity-1) do 
    str := (!str) ^ ",c->f" ^ (int_to_string i)
  done;
  (!str)

let decon_bad arity = 
  let str = ref "NULL" in
  for i = 1 to (arity-1) do
    str := (!str) ^ ",NULL"
  done;
  (!str)
    
(* common c types *)
let stamp_type = no_qual (Ident "stamp")
let etype e = no_qual (Ident e)
let vtype e = no_qual (Ident (e ^ "_var"))
let cetype e = const (Ident e)
let cvtype e = const (Ident (e ^ "_var"))
let ltype e = no_qual (Ident (e ^ "_list"))
let bool = no_qual (Ident "bool")
let pcchar = no_qual (Pointer (const Char))
let file_type = no_qual (Pointer (no_qual (Ident "FILE")))
let gen_e_type = no_qual (Ident "gen_e")

(* get_stamp operation for any sort *)
let gen_get_stamp e s = (no_qual (Ident "stamp"),
			 e ^ "_get_stamp",
			 args [gen_e_type],
			 [Return (s ^ "_get_stamp((gen_e)arg1)")],[])
 

(* is_var operation for any sort *)
let gen_is_var e s = (no_qual (Ident "bool"),
			      e ^ "_is_var",
			      args [gen_e_type],
			      [Return (s ^ "_is_var((gen_e)arg1)")],[])


(* cmp operation for any sort *)
(*
let gen_cmp e s = (no_qual (Ident "int"),
		   e ^ "_cmp",
		   args [gen_e_type;gen_e_type], 
		   [Return (s ^ "_get_stamp((gen_e)arg1) - " ^
			    s ^ "_get_stamp((gen_e)arg2)")],[])
*)
