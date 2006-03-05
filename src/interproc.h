#ifndef _INTERPROC_H_
#define _INTERPROC_H_
#include <core.h>
#include <list.h>
#include "absyn.h"


namespace Interproc {
  void set_inference_prefix(string_t<`H>);
  void dump_tags(string_t sourcefile, List::list_t<Absyn::decl_t>);
  void close_tags_file();
  List::list_t<Absyn::decl_t> resolve_all(string_t<`H> filename, List::list_t<Absyn::decl_t,`H> tds);
  List::list_t<Absyn::decl_t> binding_cvar_resolution(List::list_t<Absyn::decl_t, `H> tds);
  void emit_decls(string_t<`H>, List::list_t<Absyn::decl_t>);
  void inference_statistics();
}

#endif
