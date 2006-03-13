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

open Sort_utils

exception Variance of string

(* generate a list of short circuited ors *)
let rec gen_short_ors  = function
  | [ h ] -> h
  | h :: t -> h ^ " || " ^ (gen_short_ors t)
  | [ ] -> ""

let gen_flags = ref true

class termsort_gen =
  object (this)
    method get_name = "term"
	
    (* a counter for user-defined constructors. *)
    val mutable counter = 10;

    val region = "term_sort_region"
    val hash = "term_sort_hash"

    method get_new_type () = 
      counter <- counter + 1;
      int_to_string (counter)

    method gen_sort_ops (file : file) (header : header) (e : exprid) =
      (* only generate flags the first time around *)
      let flags = "extern bool flag_occurs_check;\n" in
      let header_decl = 
	"DECLARE_LIST(EXPRID_list,EXPRID);\n\
	 EXPRID EXPRID_zero(void);\n\
	 EXPRID EXPRID_one(void);\n\
         EXPRID EXPRID_wild(void);\n\
	 EXPRID EXPRID_fresh(const char *name);\n\
	 EXPRID EXPRID_constant(const char *name) ;\n\
	 bool EXPRID_is_constant(EXPRID e,const char *name);\n\
         void EXPRID_unify(EXPRID e1, EXPRID e2) ;\n\
         void EXPRID_cunify(EXPRID e1,EXPRID e2) ;\n\
	 EXPRID EXPRID_ecr(EXPRID e) ;\n
         EXPRID EXPRID_cmp(const EXPRID e1,const EXPRID e2);\n
         EXPRID EXPRID_eq(EXPRID e1, EXPRID e2);\n"
      in
      let file_decl = 
        "DECLARE_OPAQUE_LIST(EXPRID_list,gen_e);\n\
	 EXPRID EXPRID_zero(void);\n\
	 EXPRID EXPRID_one(void);\n\
         EXPRID EXPRID_wild(void);\n\
	 EXPRID EXPRID_fresh(const char *name);\n\
	 EXPRID EXPRID_constant(const char *name) ;\n\
         bool EXPRID_is_constant(EXPRID e,const char *name);\n\
	 EXPRID EXPRID_ecr(EXPRID e) ;\n\
	 void EXPRID_inclusion_ind(EXPRID e1,EXPRID e2) ;\n\
	 void EXPRID_cunify_ind(EXPRID e1, EXPRID e2) ;\n\
	 void EXPRID_unify_ind(EXPRID e1,EXPRID e2) ;\n"
      in
      let file_defn = 
	"DEFINE_LIST(EXPRID_list,gen_e);\n\n\
         EXPRID EXPRID_zero(void)\n\
	 {\n \
	     return term_zero();\n\
	 }\n\n\
	 EXPRID EXPRID_one(void)\n\
	 {\n \
	    return term_one();\n\
	 }\n\n\
	 EXPRID EXPRID_wild(void)\n\
	 {\n \
	    return term_wild();\n\
	 }\n\n\
	 EXPRID EXPRID_fresh(const char *name)\n\
	 {\n \
	     return term_fresh(name);\n\
	 }\n\n\
	 EXPRID EXPRID_fresh_small(const char *name)\n\
	 {\n \
	    return term_fresh_small(name);\n\
	 }\n\n\
	 EXPRID EXPRID_fresh_large(const char *name)\n\
	 {\n \
	     return term_fresh_large(name);\n\
	 }\n\n\
	 EXPRID EXPRID_constant(const char *name) \n\
	 {\n \
	    return term_constant(name);\n\
	 }\n\n\
	 bool EXPRID_is_constant(EXPRID e, const char *name) \n\
	 {\n \
	   if (term_is_constant(e))\n\
	       return (! strcmp(name,term_get_constant_name(e)));\n\
	   else return FALSE;\n\
         }\n\n\
	 EXPRID EXPRID_ecr(EXPRID e) \n\
	 {\n \
	    return term_get_ecr(e);\n\
	 }\n\n\
	 void EXPRID_inclusion_ind(EXPRID e1, EXPRID e2) \n\
	 {\n \
	    term_unify(EXPRID_con_match,EXPRID_occurs,e1,e2);\n\
	 }\n\n\
	 bool EXPRID_eq(EXPRID e1, EXPRID e2)\n\
	 {\n \
	    return term_eq(e1,e2);\n\
	 }\n\n\
         int EXPRID_cmp(const EXPRID e1,const EXPRID e2) \n\
         {\n \
	    return term_get_stamp(e1) - term_get_stamp(e2);\n\
	 }\n\n\
	 void EXPRID_inclusion_ind_contra(EXPRID e1, EXPRID e2) \n\
	 {\n \
	    term_unify(EXPRID_con_match,EXPRID_occurs,e2,e1);\n\
	 }\n\n\
	 void EXPRID_unify(EXPRID e1, EXPRID e2) \n\
	 {\n \
           banshee_clock_tick();\n\
           EXPRID_unify_ind(e1,e2);\n\
         }\n\n\
	 void EXPRID_cunify(EXPRID e1, EXPRID e2) \n\
	 {\n \
           banshee_clock_tick();\n\
           EXPRID_cunify_ind(e1,e2);\n\
         }\n\n\
	 void EXPRID_cunify_ind(EXPRID e1, EXPRID e2) \n\
	 {\n \
           term_cunify(EXPRID_con_match,EXPRID_occurs,e1,e2);\n\
         }\n\n\
	 void EXPRID_unify_ind(EXPRID e1, EXPRID e2) \n\
	 {\n \
	    term_unify(EXPRID_con_match,EXPRID_occurs,e1,e2);\n\
	 }\n"
      in
      let file_tdecls = 
	"typedef gen_e EXPRID;\n\
	 typedef gen_e_list EXPRID_list;\n"
      in
      let names = [("EXPRID",e);("REGION",region);("HASH",hash)]
      in
      file#add_fdef  (func (gen_get_stamp e "term"));
      file#add_gdecl (prototype (gen_get_stamp e "term"));
      file#add_fdef  (func (gen_is_var e "term"));
      file#add_gdecl (prototype (gen_is_var e "term"));
      file#add_fdef (def_substitution names file_defn);
      file#add_includes [include_header true "term-sort.h"];
      file#add_gdecl (decl_substitution names file_decl);
      header#add_gdecl (decl_substitution names 
			  (if (!gen_flags) then (flags ^ header_decl)
			  else header_decl));
      file#add_tdecl (decl_substitution names file_tdecls);
      gen_flags := false

    method private gen_deconstructor file hdr e c consig =
      let arity = List.length consig in
      let macro = String.uppercase c ^ "_" in 
      let ret = no_qual (Ident ("struct " ^ c ^ "_decon")) in
      let f_name= c ^ "_decon" in
      let f_args = args [etype e] in
      let cast = Expr ("struct " ^ c ^ "_* c = (struct " ^ c ^ "_ *)arg1;") in
      let build_decon = Return ("(struct " ^ c 
				^ "_decon){" ^ (decon_struct arity) ^ "}") in
      let build_bad = Return ("(struct " ^ c
			      ^ "_decon){" ^ (decon_bad arity) ^ "}") in
      let body = [Condition ("((gen_term)arg1)->type == " ^ macro,
			Compound([cast;build_decon]),
		        build_bad)] in
      let p,f = gen_proto_and_fun (ret,f_name,f_args,body) in
      file#add_gdecl p;
      hdr#add_gdecl p;
      file#add_fdef f

  method private gen_constructor file hdr e c consig = 
      let query_decl = 
	"bool EXPRID_is_CONSTRUCTOR(EXPRID e);" in
      let query_defn = 
	"bool EXPRID_is_CONSTRUCTOR(EXPRID e)\n\
	 {\n \
	    return ((gen_term)e)->type == TYPE;\n\
	 }\n" in 
      let arity = List.length consig in
      let num_args = int_to_string (arity + 1) in
      let ret = (etype e) in 
      let name =  c in
      let ctype = this#get_new_type() in
      let args =
	args (List.map (function (x,_) -> no_qual (Ident x)) consig) in
      let body1 = [ Expr ("struct " ^ c ^ "_ *ret;");
		    Expr ("stamp s[" ^ num_args ^ "];");
		    Expr ("s[0] = " ^ (String.uppercase (c^"_;"))) ] in
      let gen_s e n = 
	Expr ("s[" ^ (int_to_string n) ^"] = "  
	      ^ e ^ "_get_stamp((gen_e)arg" ^(int_to_string n) ^");") in
      let n = ref 0 in
      let body2 = List.rev (
	foldl (function ((x,y),acc) -> n := !n + 1; (gen_s x (!n)):: acc)
	  [] consig ) in
      let body3 = [ Expr ("if ((ret = (struct " ^ c ^ "_ *)" ^ 
			  "term_hash_find(" ^ hash ^ ",s," 
			  ^ num_args ^ ")) == NULL)\n{");
		    Expr ("ret = ralloc(" ^region ^",struct " ^ c ^ "_);");
		    Expr ("ret->type = s[0];");
		    Expr ("ret->st = stamp_fresh();")] in
      let gen_body n = 
	Expr ("ret->f" ^ (int_to_string n) ^ 
	      " = arg" ^ (int_to_string (n+1)) ^";") in
      let body4 : statement list ref = ref [] in
      let _ = 
	for i = 0 to (arity-1) do
	  (body4 := (gen_body i)::(!body4)) done in
      let body5 = [ Expr ("term_hash_insert(" ^ hash ^ ",(gen_e)ret,s," ^ 
			 num_args ^ ");\n}");
		    Return ( parens e ^ "ret");] in
      let body = body1 @ body2 @ body3 @ (List.rev !body4) @ body5 in
      let p,f = gen_proto_and_fun ~quals:[] (ret,name,args,body) in
      let names = 
	[("CONSTRUCTOR",c);("EXPRID",e);("TYPE",ctype)] in
      file#add_macro (define_val (String.uppercase (c ^ "_")) 
			ctype);
      hdr#add_gdecl (decl_substitution names query_decl);
      file#add_fdef (def_substitution names query_defn);
      hdr#add_gdecl p;
      file#add_gdecl p;
      file#add_fdef f
	
    method private gen_nullary_ops 
	(file : file) (hdr : header) (e : exprid) (c : conid) = 
      let names =  [|c;e ^ "_is_"^c|] in
      let args =  (Array.map args [|[];[etype e]|]) in
      let rets = [|etype e;bool|] in
      let b1 = [Return ("(gen_e)&" ^ c ^ "_")] in 
      let b2 = [Return ("((gen_term)arg1)->type == "  ^ c ^ "_.st")] in
      let bodies = [|b1;b2|] in
      file#add_macro (define_val (String.uppercase c ^ "_")
			(this#get_new_type()) );
      file#add_gdecl (var ~init:("{" ^ (String.uppercase c) ^ "_," 
				 ^ (String.uppercase c) ^ "_}")
			(no_qual (Struct "gen_term"))
		      ( c ^ "_" ) (Some Static));
      for i = 0 to 1 do
	let p,f =  gen_proto_and_fun (Array.get rets i,
				      Array.get names i,
				      Array.get args i,
				      Array.get bodies i) in 
	(hdr#add_gdecl p; file#add_gdecl p; file#add_fdef f)
      done;
   
    method private gen_con_match 
	(file : file) (e : exprid) (sigs : databody) = 	
     let name = e ^ "_con_match" in
      let args = args [gen_e_type; gen_e_type] in
      let gen_con_case c consig = 
	let gen_field (e',v) n = 
	  let incl = match v with
	  | NEGvariance -> 
	      (raise (Variance ("Term constructors do not allow variance"));"")
	  | NOvariance -> 
	      e' ^ "_unify_ind"
	  | POSvariance -> 
	      (raise (Variance ("Term constructors do not allow variance"));"")
	  in
	  Expr (incl ^ "(((struct " ^ c ^ "_ *)arg1)->f" ^ n ^
		",((struct " ^ c ^ "_ *)arg2)->f" ^ n ^ ");")
	in
	let con_cases = 
	  let counter = ref (-1) in
	  List.map 
	    (function b -> 
	      counter := !counter+1; gen_field b (int_to_string (!counter))) 
	    consig 
	in
	(String.uppercase (c ^ "_"),(Compound con_cases))
      in
      let gen_inner_switch ((c,consig_opt),others) = match consig_opt with
      |	Some consig ->
	  (String.uppercase c ^ "_",
	   Switch ("((gen_term)arg2)->type",
		   [(gen_con_case c consig)],
		   Expr "handle_error(arg1,arg2,bek_cons_mismatch);") )
      |	None -> (String.uppercase c ^ "_",
		 Expr "if (((gen_term)arg1)->type != ((gen_term)arg2)->type) handle_error(arg1,arg2,bek_cons_mismatch);")
      in
      let body conids = 
      	(Switch("((gen_term)arg1)->type",List.map gen_inner_switch conids, 
		Expr "handle_error(arg1,arg2,bek_cons_mismatch);"))
      in
      let conids' = (foldl 
		       (function ((c,sig_opt),acc) -> 
			 match sig_opt with 
			 | Some s -> (c,List.length s)::acc
			 | None -> (c,0)::acc) [] sigs) in
      let conids = List.map (function x -> (x,conids')) sigs in 
      let p,f = gen_proto_and_fun 
	  (void,name,args,[body conids; Expr "return;"]) in
      file#add_gdecl p;
      file#add_fdef f      

    method private gen_sig_ops 
	(file : file) (hdr : header) (e : exprid) (c : conid) (s : consig) = 
      this#gen_constructor file hdr e c s;
      this#gen_deconstructor file hdr e c s;
      let rec gen_sig_ops' bconsigs n = match bconsigs with 
      | (e',_) :: t  -> 
	  gen_sig_ops' t (n+1)
      |	[] -> () in
      let flds = fields (List.map (function (e,_) -> no_qual (Ident e)) s) in
      let decon_flds = 
	decon_fields (List.map (function (e,_) ->no_qual (Ident e)) s) in
      file#add_gdecl (struct_decl (c ^"_") flds);
      file#add_gdecl (struct_decl (c ^ "_decon") decon_flds);
      hdr#add_gdecl (struct_decl (c ^ "_decon") decon_flds);
      gen_sig_ops' s 0
      
    method private gen_occurs_check (file : file) (hdr : header) e b =
      let occurs_name = e ^ "_occurs" in
      let occurs_args = args [vtype "term";etype e] in
      let occurs_ret = bool in
      let var_case = 
	("VAR_TYPE",Return ("(term_get_stamp((gen_e)arg1) == term_get_stamp(ecr))")) 
      in
      let gen_con_case (c, consig) =
	let gen_con_body consig = 
	  let counter = ref (-1) in 
	  match consig with 
	  | Some signature ->
	      Compound [ (Expr ("struct " ^ c ^ "_ *con = (struct " ^ c ^ "_ *)arg2;"));
			 Return (gen_short_ors(
			 List.map (function (e,_) -> 
			   (counter := !counter+1; occurs_name ^ "(arg1,con->f" 
			    ^ (int_to_string (!counter)) ^")" ) ) signature)) ]
	  | _ -> 
	      Return "FALSE"
	in
	(String.uppercase c ^ "_",gen_con_body consig)
      in
      let switch_cases =  var_case :: (List.map gen_con_case b) in
      let switch = Switch ("((gen_term)ecr)->type",switch_cases, Return "FALSE") in
      let p,f = gen_proto_and_fun ~quals:[] (occurs_ret,occurs_name,occurs_args, [Expr ("gen_e ecr = term_get_ecr(arg2);");switch]) in
      file#add_gdecl p; 
      file#add_fdef f


    method private gen_pr_fun (file : file) (hdr : header) e b = 
      let pr_name = e ^ "_print" in 
      let pr_args = args [file_type;etype e] in
      let pr_ret = void in
      let base_idents = 
	["VAR_TYPE";"ZERO_TYPE";"ONE_TYPE";"CONSTANT_TYPE"] 
      in
      let base_statements = 
	[ 
	  Expr ("fprintf(arg1,\"%s\","
		^"tv_get_name((term_var)ecr));");
	  Expr ("fprintf(arg1,\"0\");");
	  Expr ("fprintf(arg1,\"1\");");
	  Expr ("fprintf(arg1,term_get_constant_name(ecr));") ] 
      in  
      let gen_con_case (c,consig) = 
	let gen_con_rest e' n = 
	  [ Expr ("fprintf(arg1,\",\");");
	    Expr (e' ^ "_print(arg1,((struct " ^ c ^ "_ *)ecr)->f" ^ n ^ ");") ]
	in
	let con_body l = 
	  let counter = ref ((List.length l)+1) in
	  foldr
	    (function ((e',_),acc) -> 
	      counter := !counter-1;
	      (gen_con_rest e' (int_to_string (!counter))) @ acc) 
	    [] l
	in
	match consig with
	| Some ((e',v) :: t) -> 
	    (String.uppercase c ^ "_",
	     Compound ([ Expr ("fprintf(arg1,\"" ^ c ^ "(\");");
			 Expr (e' ^
			       "_print(arg1,((struct " ^ c ^ "_ *)ecr)->f0);");
		       ]  @ 
		       (con_body t) @
		       [ Expr ("fprintf(arg1,\")\");") ]
			 )) 
	| Some nil -> 
	    (String.uppercase c ^ "_",Expr ("fprintf(arg1,\"" ^c ^ "\");"))
	| None ->
	    (String.uppercase c ^ "_",Expr ("fprintf(arg1,\"" ^c ^ "\");"))
      in
      let base_cases = List.combine base_idents base_statements in
      let pr_con_cases = List.map gen_con_case b in
      let pr_cases = base_cases @ pr_con_cases in
      let pr_switch = 
	Switch ("((gen_term)ecr)->type",pr_cases, Return "") in
      let p,f = gen_proto_and_fun ~quals:[] (pr_ret,pr_name,pr_args, [Expr ("gen_e ecr = term_get_ecr(arg2);");pr_switch]) in
      hdr#add_gdecl p; 
      file#add_gdecl p; 
      file#add_fdef f

   method private gen_conspec_ops 
	(f : file) (h : header) (e : exprid) (c : conid) (s : consig option) = 
      match s with 
      |	None -> this#gen_nullary_ops f h e c 
      |	Some signature -> this#gen_sig_ops f h e c signature

    method gen_con_ops (file : file) (hdr : header) ((e,b) : exprid*databody) =
      List.iter (function (c,s) -> this#gen_conspec_ops file hdr e c s) b;
      this#gen_pr_fun file hdr e b;
      this#gen_occurs_check file hdr e b;
      this#gen_con_match file e b

    method init (e : exprid) = [Expr ("term_init();") ]
    method reset (e : exprid) = [Expr ("term_reset();") ] 
  end

let termsort = (new termsort_gen :> sort_gen)
