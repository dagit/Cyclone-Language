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

extern void decllist2file(list<decl> tdl, FILE @f);

extern Doc decl2doc(decl d);

extern string typ2string(typ);
extern string exp2string(exp);
extern string stmt2string(stmt);
extern string qvar2string(qvar);
extern string decllist2string(list<decl> tdl);
extern string prim2string(primop p);
extern string pat2string(pat p);

  // These are only exposed so Abysndump can use them:
  extern int exp_prec(exp);
  extern string char_escape(char);
  extern string string_escape(string);
  extern string prim2str(primop p);
  extern bool is_declaration(stmt s);
  extern $(Core::Opt_t<var>,tqual,typ)@ arg_mk_opt($(var,tqual,typ)@ arg);
  extern $(tqual,typ,list<type_modifier>)@ to_tms(tqual tq,typ t);

}
#endif
