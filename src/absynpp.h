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

extern void decllist2file(list_t<decl> tdl, FILE @f);

extern doc_t decl2doc(decl d);


extern string typ2string(typ); // doesn't rewrite temp tvars
extern string kind2string(kind_t);
extern string ckind2string(conref<kind_t>);
extern string exp2string(exp);
extern string stmt2string(stmt);
extern string qvar2string(qvar);
extern string decllist2string(list_t<decl> tdl);
extern string prim2string(primop p);
extern string pat2string(pat p);

  // These are only exposed so Absyndump can use them:
  extern int exp_prec(exp);
  extern string char_escape(char);
  extern string string_escape(string);
  extern string prim2str(primop p);
  extern bool is_declaration(stmt s);
  extern $(Core::opt_t<var>,tqual,typ)@ arg_mk_opt($(var,tqual,typ)@ arg);
  extern $(tqual,typ,list_t<type_modifier>) to_tms(tqual tq,typ t);
}
#endif
