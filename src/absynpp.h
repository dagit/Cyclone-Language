/* Pretty printer for the abstract syntax.
   Copyright (C) 2001 Greg Morrisett, AT&T
   This file is part of the Cyclone compiler.

   The Cyclone compiler is free software; you can redistribute it
   and/or modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   The Cyclone compiler is distributed in the hope that it will be
   useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with the Cyclone compiler; see the file COPYING. If not,
   write to the Free Software Foundation, Inc., 59 Temple Place -
   Suite 330, Boston, MA 02111-1307, USA. */

#ifndef _ABSYNPP_H_
#define _ABSYNPP_H_

#include <core.h>
#include <cycboot.h>
#include <list.h>
#include <pp.h>
#include "absyn.h"

namespace Absynpp {
using Absyn;
using List;
using PP;

extern struct Params {
  bool expand_typedefs;
  bool qvar_to_Cids;
  bool add_cyc_prefix;
  bool to_VC;
  bool decls_first;
  bool rewrite_temp_tvars;
  bool print_all_tvars;
  bool print_all_kinds;
  bool print_all_effects;
  bool print_using_stmts;
  bool print_externC_stmts;
  bool print_full_evars;
  bool print_zeroterm;
  bool generate_line_directives;
  bool use_curr_namespace;
  list_t<var_t> curr_namespace;
};

// used by cycdoc to suppress printing extern, etc.
extern int print_for_cycdoc;

extern void set_params(struct Params @ `r fs);

extern struct Params cyc_params_r, cyci_params_r, c_params_r, tc_params_r;

extern void decllist2file(list_t<decl_t> tdl, FILE @f);

extern doc_t decl2doc(decl_t d);

extern string_t longlong2string(unsigned long long);
extern string_t typ2string(type_t); // doesn't rewrite temp tvars
extern string_t typ2cstring(type_t); // doesn't rewrite temp tvars
extern string_t kind2string(kind_t);
extern string_t kindbound2string(kindbound_t);
extern string_t cnst2string(cnst_t);
extern string_t exp2string(exp_t);
extern string_t stmt2string(stmt_t);
extern string_t qvar2string(qvar_t);
extern string_t decllist2string(list_t<decl_t> tdl);
extern string_t prim2string(primop_t p);
extern string_t pat2string(pat_t p);
extern string_t scope2string(scope_t sc);
extern string_t tvar2string(tvar_t);
  // These are only exposed so Absyndump can use them:
  extern bool is_anon_aggrtype(type_t t);
  extern string_t cyc_string;
  extern stringptr_t cyc_stringptr;
  extern int exp_prec(exp_t);
  extern string_t char_escape(char);
  extern string_t string_escape(string_t<`H>);
  extern string_t prim2str(primop_t p);
  extern bool is_declaration(stmt_t s);
  extern $(var_opt_t,tqual_t,type_t)@
                  arg_mk_opt($(var_t,tqual_t,type_t)@ arg);
  extern $(tqual_t,type_t,list_t<type_modifier_t<`r>,`r>) to_tms(region_t<`r>,tqual_t tq,type_t t);
}
#endif
