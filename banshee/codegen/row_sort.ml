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
  
class row_sort_gen (e' : exprid) =
  object (this)
    inherit fixed_sort_gen
    val region = "flowrow_region"

    method get_name = "flowrow"

    method gen_sort_ops (file : file) (header : header) (e : exprid) =
      let header_decl = 
	"DECLARE_LIST(EXPRID_map,EXPRID_field);\n\
	 EXPRID_field EXPRID_make_field(const char *label,EPRIME expr);\n\
	 EXPRID EXPRID_zero(void);\n\
	 EXPRID EXPRID_one(void);\n\
	 EXPRID EXPRID_abs(void);\n\
	 EXPRID EXPRID_wild(void);\n\
	 EXPRID EXPRID_fresh(const char *name);\n\
	 EXPRID EXPRID_row(EXPRID_map fields,EXPRID rest) ;\n\
	 EPRIME EXPRID_extract_field(const char *field_name,EXPRID row);\n\
	 EXPRID EXPRID_extract_rest(EXPRID row);\n\
	 EXPRID_map EXPRID_extract_fields(EXPRID row);\n\
         bool EXPRID_eq(EXPRID row1, EXPRID row2);\n\
         int EXPRID_cmp(const EXPRID row1,const EXPRID row2);\n\
	 void EXPRID_inclusion(EXPRID row1,EXPRID row2) ;\n\
	 void EXPRID_unify(EXPRID row1,EXPRID row2);\n\
	 void EXPRID_print(FILE *f,EXPRID row);\n" 
      in
      let file_decl  = 
	"DECLARE_OPAQUE_LIST(EXPRID_map,flowrow_field);\n\
         void EXPRID_inclusion_ind(EXPRID row1, EXPRID row2);\n\
         void EXPRID_unify_ind(EXPRID row1,EXPRID row2);\n\
	 void EXPRID_print(FILE *f,EXPRID row) ;"
      in
      let file_defn =
	"DEFINE_LIST(EXPRID_map,flowrow_field);\n\
	 EXPRID_field EXPRID_make_field(const char *label, EPRIME expr)\n\
	 {\n \
	     flowrow_field result = ralloc(REGION, struct flowrow_field_);\n\
	     result->label = rstrdup(banshee_nonptr_region,label);\n\
	     result->expr = (gen_e) expr;\n\
	     return result;\n\
	 }\n\n\
	 EXPRID EXPRID_zero(void)\n\
	 {\n \
	     return flowrow_zero();\n\
	 }\n\n\
	 EXPRID EXPRID_one(void)\n\
	 {\n \
	    return flowrow_one();\n\
	 }\n\n\
	 EXPRID EXPRID_abs(void)\n\
	 {\n\
	    return flowrow_abs();\n\
	 }\n\n\
	 EXPRID EXPRID_wild(void)\n\
         {\n\
            return flowrow_wild();\n\
         }\n\n\
         EXPRID EXPRID_fresh(const char *name)\n\
         {\n\
            return flowrow_fresh(name);\n\
         }\n\n\
         EXPRID EXPRID_fresh_small(const char *name)\n\
         {\n\
            return flowrow_fresh_small(name);\n\
         }\n\n\
         EXPRID EXPRID_fresh_large(const char *name)\n\
         {\n\
            return flowrow_fresh_large(name);\n\
         }\n\n\
         EXPRID EXPRID_row(EXPRID_map fields,EXPRID rest) \n\
         {\n\
           return flowrow_row(EPRIME_get_stamp,fields,rest);\n\
         }\n\n\
         EPRIME EXPRID_extract_field(const char *field_name, EXPRID row)\n\
         {\n\
           return flowrow_extract_field(field_name,row);\n\
         }\n\n\
	 EXPRID EXPRID_extract_rest(EXPRID row)\n\
	 {\n\
	   return flowrow_extract_rest(row);\n\
	 }\n\n\
         bool EXPRID_eq(EXPRID row1,EXPRID row2)\n\
         {\n\
           return flowrow_eq(row1,row2);\n\
         }\n\n\
         int EXPRID_cmp(const EXPRID e1,const EXPRID e2) \n\
         {\n \
	    return flowrow_get_stamp(e1) - flowrow_get_stamp(e2);\n\
	 }\n\n\
	 EXPRID_map EXPRID_extract_fields(EXPRID row)\n\
	 {\n\
	   return flowrow_extract_fields(row);\n\
         }\n\n\
         void EXPRID_inclusion_ind(EXPRID row1, EXPRID row2) \n\
         {\n\
         flowrow_inclusion(EPRIME_fresh,EPRIME_get_stamp,EPRIME_inclusion_ind,\
			       EPRIME_zero(), row1,row2);\n\
         }\n\n\
         void EXPRID_inclusion_ind_contra(EXPRID row1, EXPRID row2) \n\
         {\n\
         flowrow_inclusion(EPRIME_fresh,EPRIME_get_stamp,EPRIME_inclusion_ind,\
			       EPRIME_zero(), row2,row1);\n\
         }\n\n\
         void EXPRID_unify_ind(EXPRID row1, EXPRID row2) \n\
         {\n\
           EXPRID_inclusion_ind(row1,row2);\n\
           EXPRID_inclusion_ind(row2,row1);\n\
         }\n\n\
         void EXPRID_unify(EXPRID row1, EXPRID row2) \n\
         {\n\
           banshee_clock_tick();\n\
           EXPRID_unify_ind(row1,row2);\n\
         }\n\n\
         void EXPRID_inclusion(EXPRID row1, EXPRID row2) \n\
         {\n\
           banshee_clock_tick();\n\
           EXPRID_inclusion_ind(row1,row2);\n\
         }\n\n\
         void EXPRID_print(FILE *f,EXPRID row) \n\
         {\n\
            flowrow_print(f,EPRIME_get_stamp,EPRIME_print,row);\n\
         }\n"
      in
      let file_tdecls =
	"typedef struct flowrow_field_ *EXPRID_field;\n\
	 typedef flowrow_map EXPRID_map;\n\
	 typedef gen_e EXPRID;\n"
      in
      let names = [("REGION",region);("EXPRID",e);("EPRIME",e')] in
     (* let p_s,f_s = gen_proto_and_fun (gen_get_stamp e "flowrow") in *)
      file#add_fdef  (func (gen_get_stamp e "flowrow"));
      file#add_gdecl (prototype (gen_get_stamp e "flowrow"));
      file#add_fdef  (func (gen_is_var e "flowrow"));
      file#add_gdecl (prototype (gen_is_var e "flowrow"));
     (* file#add_gdecl p_s; file#add_fdef f_s; *)
      file#add_fdef (def_substitution names file_defn);
      file#add_includes [include_header true "flowrow-sort.h"];
      header#add_gdecl (decl_substitution names header_decl);
      file#add_gdecl (decl_substitution names file_decl);
      header#add_tdecl (opaque (e ^ "_field"));
      file#add_tdecl (decl_substitution names file_tdecls)

    method init (e : exprid) = 
      [Expr ("flowrow_init();")]
	
    method reset (e : exprid) =
      [Expr ("flowrow_reset();")]
  end 
