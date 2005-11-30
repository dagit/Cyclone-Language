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

let gen_flags = ref true

class setsort_gen =
  object (this)
    method get_name = "setif"
	
    (* a counter for user-defined constructors. 0, 1, and 2 are reserved 
       for bottom, top, and union types *)
    val mutable counter = 10;

    val region = "permanent"
    val hash = "setif_hash"

    method get_new_type () = 
      counter <- counter + 1;
      int_to_string (counter)

    method gen_sort_ops (file : file) (header : header) (e : exprid) =
      (* only generate flags the first time around *)
      let flags = 
	"extern bool flag_merge_projections;\n\
	 extern bool flag_eliminate_cycles;\n" in
      let header_decl = 
	"DECLARE_LIST(EXPRID_list,EXPRID);\n\
	 EXPRID EXPRID_zero(void);\n\
	 EXPRID EXPRID_one(void);\n\
         EXPRID EXPRID_wild(void);\n\
	 EXPRID EXPRID_fresh(const char *name);\n\
	 EXPRID EXPRID_union(EXPRID_list exprs);\n\
	 EXPRID EXPRID_inter(EXPRID_list exprs);\n\
	 EXPRID EXPRID_constant(const char *name);\n\
         bool EXPRID_eq(EXPRID e1, EXPRID e2);\n\
         int EXPRID_cmp(const EXPRID e1,const EXPRID e2);\n\
	 bool EXPRID_is_constant(EXPRID e,const char *name);\n\
         void EXPRID_inclusion(EXPRID e1,EXPRID e2);\n\
	 void EXPRID_unify(EXPRID e1,EXPRID e2);\n\
	 EXPRID_list EXPRID_tlb(EXPRID e);\n"
      in
      let file_decl = 
        "DECLARE_OPAQUE_LIST(EXPRID_list,gen_e);\n\
	 EXPRID EXPRID_zero(void);\n\
	 EXPRID EXPRID_one(void);\n\
         EXPRID EXPRID_wild(void);\n\
	 EXPRID EXPRID_fresh(const char *name);\n\
         EXPRID EXPRID_fresh_large(const char *name);\n\
	 EXPRID EXPRID_union(EXPRID_list exprs);\n\
	 EXPRID EXPRID_inter(EXPRID_list exprs);\n\
	 EXPRID EXPRID_constant(const char *name);\n\
         bool EXPRID_is_constant(EXPRID e,const char *name);\n\
	 EXPRID_list EXPRID_tlb(EXPRID e);\n\
	 void EXPRID_inclusion_ind(EXPRID e1,EXPRID e2);\n\
	 void EXPRID_unify_ind(EXPRID e1,EXPRID e2);\n"
      in
      let file_defn = 
	"DEFINE_LIST(EXPRID_list,gen_e);\n\
         EXPRID EXPRID_zero(void)\n\
	 {\n \
	     return setif_zero();\n\
	 }\n\n\
	 EXPRID EXPRID_one(void)\n\
	 {\n \
	    return setif_one();\n\
	 }\n\n\
	 EXPRID EXPRID_wild(void)\n\
	 {\n \
	    return setif_wild();\n\
	 }\n\n\
	 EXPRID EXPRID_fresh(const char *name)\n\
	 {\n \
	     return setif_fresh(name);\n\
	 }\n\n\
	 EXPRID EXPRID_fresh_small(const char *name)\n\
	 {\n \
	    return setif_fresh_small(name);\n\
	 }\n\n\
	 EXPRID EXPRID_fresh_large(const char *name)\n\
	 {\n \
	     return setif_fresh_large(name);\n\
	 }\n\n\
	 EXPRID EXPRID_union(EXPRID_list exprs) \n\
	 {\n \
	    return setif_union(exprs);\n\
	 }\n\n\
	 EXPRID EXPRID_inter(EXPRID_list exprs) \n\
	 {\n \
	    return setif_inter(exprs);\n\
	 }\n\n\
	 EXPRID EXPRID_constant(const char *name) \n\
	 {\n \
	    return setif_constant(name);\n\
	 }\n\n\
         bool EXPRID_eq(EXPRID e1, EXPRID e2) \n\
         {\n \
	    return setif_eq(e1,e2);\n\
	 }\n\n\
         int EXPRID_cmp(const EXPRID e1,const EXPRID e2) \n\
         {\n \
	    return setif_get_stamp(e1) - setif_get_stamp(e2);\n\
	 }\n\n\
	 bool EXPRID_is_constant(EXPRID e, const char *name) \n\
	 {\n \
	   if (setif_is_constant(e))\n\
	       return (! strcmp(name,setif_get_constant_name(e)));\n\
	   else return FALSE;\n\
         }\n\n\
	 EXPRID_list EXPRID_tlb(EXPRID e) \n\
	 {\n \
	    return setif_tlb(e);\n\
	 }\n\n\
	 void EXPRID_inclusion_ind(EXPRID e1, EXPRID e2) \n\
	 {\n \
	    setif_inclusion(EXPRID_con_match,EXPRID_res_proj,EXPRID_print,e1,e2);\n\
	 }\n\n\
	 void EXPRID_inclusion(EXPRID e1, EXPRID e2) \n\
	 {\n \
            banshee_clock_tick();\n\
            EXPRID_inclusion_ind(e1,e2);\n\
	 }\n\n\
	 void EXPRID_inclusion_ind_contra(EXPRID e1, EXPRID e2) \n\
	 {\n \
	    setif_inclusion(EXPRID_con_match,EXPRID_res_proj,EXPRID_print,e2,e1);\n\
	 }\n\n\
	 void EXPRID_unify_ind(EXPRID e1, EXPRID e2) \n\
	 {\n \
	    setif_inclusion(EXPRID_con_match,EXPRID_res_proj,EXPRID_print,e1,e2);\n\
	    setif_inclusion(EXPRID_con_match,EXPRID_res_proj,EXPRID_print,e2,e1);\n\
	 }\n\
	 void EXPRID_unify(EXPRID e1, EXPRID e2) \n\
	 {\n \
            banshee_clock_tick();\n\
            EXPRID_unify_ind(e1,e2);\n\
	 }\n "
      in
      let file_tdecls = 
	"typedef gen_e EXPRID;\n\
	 typedef gen_e_list EXPRID_list;\n"
      in
      let names = [("EXPRID",e);("REGION",region);("HASH",hash)]
      in
      file#add_fdef  (func (gen_get_stamp e "setif"));
      file#add_gdecl (prototype (gen_get_stamp e "setif"));
      file#add_fdef  (func (gen_is_var e "setif"));
      file#add_gdecl (prototype (gen_is_var e "setif"));
      file#add_fdef (def_substitution names file_defn);
      file#add_includes [include_header true "setif-sort.h"];
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
      let body = [Condition ("((setif_term)arg1)->type == " ^ macro,
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
	    return ((setif_term)e)->type == TYPE;\n\
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
      let b2 = [Return ("((setif_term)arg1)->type == "  ^ c ^ "_.st")] in
      let bodies = [|b1;b2|] in
      file#add_macro (define_val (String.uppercase c ^ "_")
			(this#get_new_type()) );
      file#add_gdecl (var ~init:("{" ^ (String.uppercase c) ^ "_," 
				 ^ (String.uppercase c) ^ "_}")
			(no_qual (Struct "setif_term"))
		      ( c ^ "_" ) (None));
      for i = 0 to 1 do
	let p,f =  gen_proto_and_fun (Array.get rets i,
				      Array.get names i,
				      Array.get args i,
				      Array.get bodies i) in 
	(hdr#add_gdecl p; file#add_gdecl p; file#add_fdef f)
      done;

    method private gen_res_proj (file : file) (e : exprid) sigs = 
      let name = e ^ "_res_proj" in
      let args = args [no_qual (Ident "setif_var");gen_e_type] in
      let gen_case c n (e',v) =
	let incl = match v with
	| NEGvariance -> e' ^ "_inclusion_ind_contra"
	| NOvariance -> e' ^ "_unify_ind"
	| POSvariance -> e' ^ "_inclusion_ind" in
	let ret = 
	  "setif_proj_merge(arg1," ^ "(gen_e)((struct " ^
	  c ^ "Proj" ^ n ^ "_ *)arg2)->f0,get_" ^ c ^ "_proj" ^ n ^ 
	  "_arg," ^ c ^ "_pat" ^ n ^ "_con,(fresh_large_fn_ptr)" ^ e' ^
	  "_fresh_large,(incl_fn_ptr)" ^ incl ^ "," ^ e ^ "_inclusion_ind)"
	in
	(String.uppercase c ^ "PROJ" ^ n ^ "_",Return ret) 
      in
      let conid_cases (c,sig_option) = match sig_option with 
      |	None -> []
      |	Some s -> 
	  let counter = ref (-1) in 
	  List.map 
	    (function (e',v) -> 
	      counter := (!counter)+1; 
	      gen_case c (int_to_string (!counter)) (e',v)) s in
      let cases = foldr (function (x,l) -> (conid_cases x) @ l) [] sigs in
      let default = Return "FALSE" in
      let body = 
	[Switch ("((setif_term)arg2)->type",cases,default); Return "FALSE"] in
      let p,f = gen_proto_and_fun ~quals:[] (bool,name,args,body) in
      file#add_gdecl p;
      file#add_fdef f

 method private gen_con_match 
	(file : file) (e : exprid) (sigs : databody) = 
      let name = e ^ "_con_match" in
      let args = args [gen_e_type; gen_e_type] in
      let gen_proj_case c n (e',v) = 
	let incl = match v with
	| NEGvariance -> e' ^ "_inclusion_ind_contra"
	| NOvariance -> e' ^ "_unify_ind"
	| POSvariance -> e' ^ "_inclusion_ind" in
	let ret = 
	  incl ^ "(((struct " ^ c ^ "_ *)arg1)->f" ^ n ^ "," ^
	  "((struct " ^ c ^ "Proj" ^ n ^ "_ *)arg2)->f0);"
	in
	(String.uppercase c ^ "PROJ" ^ n ^ "_",Expr ret) 
      in
      let gen_con_case c consig = 
	let gen_field (e',v) n = 
	  let incl = match v with
	  | NEGvariance -> e' ^ "_inclusion_ind_contra"
	  | NOvariance -> e' ^ "_unify_ind"
	  | POSvariance -> e' ^ "_inclusion_ind" in
	  (incl ^ "(((struct " ^ c ^ "_ *)arg1)->f" ^ n ^
	   ",((struct " ^ c ^ "_ *)arg2)->f" ^ n ^ ")")
	in
	let con_cases = 
	  let counter = ref (-1) in
	  List.map 
	    (function b -> 
	      counter := !counter+1; 
	      Expr( ((gen_field b (int_to_string (!counter))) ^ ";" )) )
	    consig 
	in
	(String.uppercase (c ^ "_"),Compound con_cases)
      in
      let gen_other (c,n) =
	let result = ref ([] : (ident * statement) list) in
	for i = (n-1) downto 0 do
	  result := 
	    (String.uppercase (c ^ "PROJ" ^ (int_to_string i) ^ "_"),
	     Return "")::(!result)
	done; (!result) in
      let gen_others c (others : (conid * int) list) = 
	foldr (function (x,acc) -> (gen_other x) @ acc) []
          (List.filter (function (c',_) -> not (c' = c)) others) in
      let gen_inner_switch ((c,consig_opt),others) = match consig_opt with
      |	Some consig ->
	let proj_cases = 
	  let counter = ref (-1) in
	  List.map 
	    (function b -> 
	      counter := !counter+1; gen_proj_case c 
		(int_to_string (!counter)) b)
	    consig
	in
	(String.uppercase c ^ "_",
	 Switch ("((setif_term)arg2)->type",
		 (gen_con_case c consig)::(proj_cases) @ 
		 (gen_others c others),
		 Expr "handle_error(arg1,arg2,bek_cons_mismatch);") )
      |	None -> (String.uppercase c ^ "_",
		 Expr "if (((setif_term)arg1)->type != ((setif_term)arg2)->type) handle_error(arg1,arg2,bek_cons_mismatch);  ";)
      in
      let body conids = 
      	(Switch("((setif_term)arg1)->type",List.map gen_inner_switch conids, 
		Expr "handle_error(arg1,arg2,bek_cons_mismatch);"))
      in
      let conids' = (foldl 
		       (function ((c,sig_opt),acc) -> 
			 match sig_opt with 
			 | Some s -> (c,List.length s)::acc
			 | None -> (c,0)::acc) [] sigs) in
      let conids = List.map (function x -> (x,conids')) sigs in 
      let p,f = gen_proto_and_fun 
	   ~quals:[] (void,name,args,[body conids; Expr "return;"]) in
      file#add_gdecl p;
      file#add_fdef f      

    method private gen_proj_ops file header e e' c (n : string) = 
      let macro = String.uppercase c ^ "PROJ" ^ n ^ "_" in
      let header_decl =
	"EPRIME CONSTRUCTOR_projNUMBER(EXPRID arg1) ;" in
      let file_decl1 = 
	"gen_e get_CONSTRUCTOR_projNUMBER_arg(gen_e_list arg1)\n\
	  {\n\
	     gen_e temp;\n\
	     gen_e_list_scanner scan;\n\
	     gen_e_list_scan(arg1,&scan);\n\
	     while (gen_e_list_next(&scan,&temp))\n\
		 {\n\
		  if (((setif_term)temp)->type == CAPCONSPROJNUMBER_)\n\
                     	return (gen_e)((struct CONSTRUCTORProjNUMBER_ * ) \
				       temp)->f0;\n\
		} \n\
		 return NULL;\n\
	     }\n"
      in
      let file_decl2 = 
	"EPRIME CONSTRUCTOR_projNUMBER(EXPRID arg1) \n \
	  {\n\
	    EPRIME c;\n\
            banshee_clock_tick();\n\
	    if (setif_is_var(arg1))\n\
	       {\n\
		  setif_var v = (setif_var)arg1;\n\
		  c = (EPRIME)sv_get_ub_proj(v,get_CONSTRUCTOR_proj\
					       NUMBER_arg);\n\
		  if (c != NULL)\n\
		    return c;\n\
	          else\n\
	          {\n\
		   EXPRID e;\n\
		   gen_e lb; \n\
		   bounds_scanner scan; \n\
		   c = EPRIME_fresh(NULL);\n\
		   e = CONSTRUCTOR_patNUMBER(c);\n\
		   sv_add_ub_proj(v,e);\n\
                   if (!banshee_check_rollback(setif_sort)) {\n\
	            setif_register_rollback();\n\
	           }\n\
		   setif_register_ub_proj(sv_get_ub_projs(v),e);\n\
                   bounds_scan(sv_get_lbs(v),&scan);\n\
		   while (bounds_next(&scan,&lb))\n\
		   {\n\
		    setif_inclusion(EXPRID_con_match,EXPRID_res_proj,EXPRID_print,lb,e);\n\
		   }\n\
		   return c;\n\
	          }\n\
	      }\n\
	   else if ( ((setif_term)arg1)->type == CAPCONS_)\n\
	       return ((struct CONSTRUCTOR_ * )arg1)->fNUMBER;\n\
	   else if ( setif_is_zero(arg1))\n\
	       return EPRIME_zero();\n\
           else if ( setif_is_union(arg1))\n\
	       {\n\
		  c = get_CONSTRUCTOR_projNUMBER_arg(setif_get_proj_cache\
						       (arg1));\n\
		if (c != NULL)\n\
		    return c;\n\
   	        else\n\
	        {\n\
		   EXPRID e;\n\
		   c = EPRIME_fresh(NULL);\n\
		   e = CONSTRUCTOR_patNUMBER(c);\n\
		   setif_set_proj_cache(arg1,e);\n\
		   EXPRID_inclusion_ind(arg1,e);\n\
		   return c;\n\
	       }\n\
           }\n\
	   else\n\
	       {\n\
		  EXPRID e;\n\
		  c = EPRIME_fresh(NULL);\n\
		  e = CONSTRUCTOR_patNUMBER(c);\n\
		  EXPRID_inclusion_ind(arg1,e);\n\
		  return c;\n\
	      }\n\
        }\n"
      in
      let names = 
	[("CONSTRUCTOR",c);("NUMBER",n);("EPRIME",e');
	 ("EXPRID",e);("REGION",region);("CAPCONS",String.uppercase c)] 
      in
      let fields = 
	[(no_qual Int, "type");(no_qual (Ident "stamp"), "st")]@
	[(etype e',"f0")] in
      file#add_macro (define_val macro (this#get_new_type()));
      file#add_gdecl (struct_decl (c ^ "Proj" ^ n ^ "_") fields);
      file#add_fdef (def_substitution names file_decl1);
      file#add_fdef (def_substitution names file_decl2);
      header#add_gdecl (decl_substitution names header_decl);

  method private gen_pat_ops file hdr e e' c (n : string) = 
      let macro = String.uppercase c ^ "PROJ" ^ n ^ "_" in
      let name1 = c ^ "_pat" ^ n ^ "_con" in
      let args1 = args [gen_e_type] in 
      let ret1 = gen_e_type in
      let body1 = [Return ("(gen_e)" ^ c ^ "_pat" ^ n ^ 
			   "(" ^ (parens e') ^ "arg1)") ] in 
      let p1,f1 = gen_proto_and_fun ~quals:[] (ret1,name1,args1,body1) in
      let name2 = c ^ "_pat" ^ n in
      let args2 = args [etype e'] in
      let ret2 = etype e in
   let body2 = [ Expr ("struct " ^ c ^ "Proj" ^ n ^ "_* ret;");
                    Expr ("stamp s[2];");
                    Expr ("s[0] = " ^ macro ^ ";");
                    Expr ("s[1] = " ^ e' ^ "_get_stamp((gen_e)arg1);");
              	    Expr ("if ((ret = (struct " ^ c ^ "Proj" ^ n ^ "_ *)" 
			  ^ "term_hash_find(" ^ hash ^ ",s,2)) == NULL) \n{");
		    Expr ("ret = ralloc(" ^ region ^ ",struct " ^ 
			  c ^ "Proj" ^ n ^ "_);");
		    Expr ("ret->type = " ^ macro ^ ";");
		    Expr ("ret->st = stamp_fresh();");
		    Expr ("ret->f0 = arg1;");
		    Expr ("term_hash_insert(" ^ hash ^ ",(gen_e)ret,s,2);\n}");
		    Return ( (parens e) ^ "ret");
		  ]  in

      let p2,f2 = gen_proto_and_fun ~quals:[] (ret2,name2,args2,body2) 
      in
      hdr#add_gdecl p2;
      file#add_gdecls [p1;p2];
      file#add_fdefs [f1;f2]

    method private gen_sig_ops 
	(file : file) (hdr : header) (e : exprid) (c : conid) (s : consig) = 
      this#gen_constructor file hdr e c s;
      this#gen_deconstructor file hdr e c s;
      let rec gen_sig_ops' bconsigs n = match bconsigs with 
      | (e',_) :: t  -> 
	  this#gen_proj_ops file hdr e e' c (int_to_string n);
	  this#gen_pat_ops file hdr e e' c (int_to_string n);
	  gen_sig_ops' t (n+1)
      |	[] -> () in
      let flds = fields (List.map (function (e,_) -> no_qual (Ident e)) s) in
      let decon_flds = 
	decon_fields (List.map (function (e,_) ->no_qual (Ident e)) s) in
      file#add_gdecl (struct_decl (c ^"_") flds);
      file#add_gdecl (struct_decl (c ^ "_decon") decon_flds);
      hdr#add_gdecl (struct_decl (c ^ "_decon") decon_flds);
      gen_sig_ops' s 0
      
    method private gen_pr_fun (file : file) (hdr : header) e b = 
      let pr_name = e ^ "_print" in 
      let pr_args = args [file_type;etype e] in
      let pr_ret = void in
      let base_idents = 
	["VAR_TYPE";"ZERO_TYPE";"ONE_TYPE";"CONSTANT_TYPE";
	 "UNION_TYPE";"INTER_TYPE"] 
      in
      let union_statement = 
	Compound [ Expr ("gen_e_list list = setif_get_union(arg2);");
		   Expr ("gen_e_list_scanner scan;");
		   Expr ("gen_e temp;");
		   Expr ("gen_e_list_scan(list,&scan);");
		   Expr ("if (gen_e_list_next(&scan,&temp))");
		   Expr (e ^ "_print(arg1,temp);");
		   Expr ("while (gen_e_list_next(&scan,&temp))\n{");
		   Expr ("fprintf(arg1,\" || \");");
		   Expr (e ^ "_print(arg1,temp);\n}"); ]
      in
      let inter_statement = 
	Compound [ Expr ("gen_e_list list = setif_get_inter(arg2);");
		   Expr ("gen_e_list_scanner scan;");
		   Expr ("gen_e temp;");
		   Expr ("gen_e_list_scan(list,&scan);");
		   Expr ("if (gen_e_list_next(&scan,&temp))");
		   Expr (e ^ "_print(arg1,temp);");
		   Expr ("while (gen_e_list_next(&scan,&temp))\n{");
		   Expr ("fprintf(arg1,\" && \");");
		   Expr (e ^ "_print(arg1,temp);\n}"); ]
      in
      let base_statements = 
	[ Expr ("fprintf(arg1,\"%s::%d\","
		^"sv_get_name((setif_var)arg2),sv_get_stamp((setif_var)arg2));");
	  Expr ("fprintf(arg1,\"0\");");
	  Expr ("fprintf(arg1,\"1\");");
	  Expr ("fprintf(arg1,setif_get_constant_name(arg2));") ] 
	@ [union_statement] @ [inter_statement]
      in  
      let gen_proj_case c e' n =
	(String.uppercase c ^ "PROJ" ^ n ^ "_",
	 Compound 
	   [ Expr ("fprintf(arg1,\"Proj[" ^ c ^ "," ^ n ^ ",\");");
	     Expr ( e' ^ "_print(arg1,((struct " ^ 
		   c ^ "Proj" ^ n ^ "_ *)arg2)->f0);");
	     Expr ("fprintf(arg1,\"]\");") ] )
      in
      let gen_con_case (c,consig) = 
	let gen_con_rest e' n = 
	  [ Expr ("fprintf(arg1,\",\");");
	    Expr (e' ^ "_print(arg1,((struct " ^ c ^ "_ *)arg2)->f" ^ n ^ ");") ]
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
			       "_print(arg1,((struct " ^ c ^ "_ *)arg2)->f0);");
		       ]  @ 
		       (con_body t) @
		       [ Expr ("fprintf(arg1,\")\");") ]
			 )) 
	| Some nil -> 
	    (String.uppercase c ^ "_",Expr ("fprintf(arg1,\"" ^c ^ "\");"))
	| None ->
	    (String.uppercase c ^ "_",Expr ("fprintf(arg1,\"" ^c ^ "\");"))
      in 
      let gen_proj_cases (c,consig) =
	match consig with
	| Some l -> 
	    let counter = ref (-1) in
	    List.map 
	      (function (e',v) -> 
		counter := !counter + 1;
		(gen_proj_case c e' (int_to_string !counter) ) ) l 
	| None -> []
      in
      let base_cases = List.combine base_idents base_statements in
      let pr_con_cases = List.map gen_con_case b in
      let pr_proj_cases = 
	foldr (function (elt,acc) -> (gen_proj_cases elt) @ acc) [] b in
      let pr_cases = base_cases @ pr_con_cases @ pr_proj_cases in
      let pr_switch = 
	Switch ("((setif_term)arg2)->type",pr_cases, Return "") in
      let p,f = gen_proto_and_fun ~quals:[] (pr_ret,pr_name,pr_args, [pr_switch]) in
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
      this#gen_res_proj file e b;
      this#gen_con_match file e b

    method init (e : exprid) = [Expr ("setif_init();") ]
    method reset (e : exprid) = [Expr ("setif_reset();") ] 
  end

let setsort = (new setsort_gen :> sort_gen)
