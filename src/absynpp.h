#ifndef _ABSYNPP_H_
#define _ABSYNPP_H_

#include "core.h"
#include "stdio.h"
#include "list.h"
#include "pp.h"
#include "absyn.h"

namespace Absynpp {
using Stdio;
using Absyn;
using List;
using PP;

extern bool expand_typedefs;
extern bool qvar_to_Cids;
extern bool decls_first;
//extern bool dont_rewrite_temp_tvars;
extern bool print_all_tvars;
extern bool print_all_kinds;
extern bool print_using_stmts;
extern bool print_externC_stmts;

extern void decllist2file(list_t<decl_t> tdl, FILE @f);

extern doc_t decl2doc(decl_t d);


extern string typ2string(type_t); // doesn't rewrite temp tvars
extern string kind2string(kind_t);
extern string ckind2string(conref_t<kind_t>);
extern string exp2string(exp_t);
extern string stmt2string(stmt_t);
extern string qvar2string(qvar_t);
extern string decllist2string(list_t<decl_t> tdl);
extern string prim2string(primop_t p);
extern string pat2string(pat_t p);
extern string scope2string(scope_t sc);

  // These are only exposed so Absyndump can use them:
  extern int exp_prec(exp_t);
  extern string char_escape(char);
  extern string string_escape(string);
  extern string prim2str(primop_t p);
  extern bool is_declaration(stmt_t s);
  extern $(Core::opt_t<var_t>,tqual_t,type_t)@ 
                  arg_mk_opt($(var_t,tqual_t,type_t)@ arg);
  extern $(tqual_t,type_t,list_t<type_modifier_t>) to_tms(tqual_t tq,type_t t);
}
#endif
