#ifndef _TCENV_H_
#define _TCENV_H_

#include "core.h"
#include "list.h"
#include "set.h"
#include "dict.h"
#include "absyn.h"
#include "position.h"

namespace Tcenv {

using Core;
using List;
using Set;
using Dict;
using Absyn;
using Position;

// Used to tell what an ordinary identifer refers to 
extern tunion Resolved {
  VarRes(binding_t); // includes unresolved variant
  StructRes(structdecl_t);
  TunionRes(tuniondecl_t,tunionfield_t);
  XTunionRes(xtuniondecl_t,tunionfield_t);
  EnumRes(enumdecl_t,enumfield_t);
};
typedef tunion Resolved resolved_t;

// Global environments -- what's declared in a global scope 
// Warning: ordinaries should really be abstract so we can ensure that any
// lookup sets the bool field to true!
// FIX: We should tree-shake the type declarations too!
extern struct Genv {
  set_t<var_t>                   namespaces;
  dict_t<var_t,structdecl_t@>    structdecls;
  dict_t<var_t,uniondecl_t@>     uniondecls;
  dict_t<var_t,tuniondecl_t@>    tuniondecls;
  dict_t<var_t,xtuniondecl_t@>   xtuniondecls;
  dict_t<var_t,enumdecl_t@>      enumdecls;
  dict_t<var_t,typedefdecl_t>    typedefs; // indirection unneeded b/c no redeclaration
  Dict::dict_t<var_t,$(resolved_t,bool)@> ordinaries; // bool for tree-shaking
  list_t<list_t<var_t>>        availables; // abs. names of "using" namespaces
};
typedef struct Genv @genv_t;

// Local function environments
extern struct Fenv;
typedef struct Fenv @fenv_t; 

extern tunion Jumpee {
  NotLoop_j;
  CaseEnd_j;
  FnEnd_j;
  Stmt_j(stmt_t);
};
typedef tunion Jumpee jumpee_t;

// Models the nesting of the RTCG constructs 
extern tunion Frames<`a> {
  Outermost(`a);
  Frame(`a,tunion Frames<`a>);
  Hidden(`a,tunion Frames<`a>);
};
typedef tunion Frames<`a> frames_t<`a>;

// Type environments 
extern struct Tenv {
  list_t<var_t>                ns; // current namespace
  dict_t<list_t<var_t>,genv_t> ae; // absolute environment
  opt_t<frames_t<fenv_t>>      le; // local environment, == null except in functions
};
typedef struct Tenv @tenv_t; 

extern tenv_t tc_init();
extern genv_t empty_genv();
extern fenv_t new_fenv(seg_t,fndecl_t);

extern tenv_t enter_ns(tenv_t, var_t);

extern list_t<var_t>     resolve_namespace(tenv_t,seg_t,var_t,list_t<var_t>);
extern resolved_t        lookup_ordinary(tenv_t,seg_t,qvar_t);
extern structdecl_t@     lookup_structdecl(tenv_t,seg_t,qvar_t);
extern uniondecl_t@      lookup_uniondecl(tenv_t,seg_t,qvar_t);
extern tuniondecl_t@     lookup_tuniondecl(tenv_t,seg_t,qvar_t);
extern opt_t<xtuniondecl_t@> lookup_xtuniondecl(tenv_t,seg_t,qvar_t);
extern enumdecl_t@       lookup_enumdecl(tenv_t,seg_t,qvar_t);
extern typedefdecl_t     lookup_typedefdecl(tenv_t,seg_t,qvar_t);

extern type_t  return_typ(tenv_t);

extern tenv_t add_local_var(seg_t,tenv_t,vardecl_t);
extern tenv_t add_pat_var  (seg_t,tenv_t,vardecl_t);

extern list_t<tvar_t> lookup_type_vars(tenv_t);
extern tenv_t     add_type_vars(seg_t,tenv_t,list_t<tvar_t>);

extern tenv_t set_in_loop(tenv_t te, stmt_t continue_dest);
extern tenv_t set_in_switch(tenv_t);
extern tenv_t set_fallthru(tenv_t te, 
			   $(list_t<tvar_t>,list_t<vardecl_t>) * pat_typ,
			   switch_clause_t clause);
extern tenv_t clear_fallthru(tenv_t);
extern tenv_t set_next(tenv_t, jumpee_t);
extern tenv_t enter_try(tenv_t te);
extern int    get_try_depth(tenv_t te);

// The next 4 all assign through their last arg
extern void process_continue(tenv_t,stmt_t,stmt_opt_t@);
extern void process_break   (tenv_t,stmt_t,stmt_opt_t@);
extern void process_goto(tenv_t,stmt_t,var_t,stmt_opt_t@);
extern $(switch_clause_t,list_t<tvar_t>,list_t<type_t>)* 
  process_fallthru(tenv_t,stmt_t,switch_clause_t *@);

extern stmt_t get_encloser(tenv_t);
extern tenv_t set_encloser(tenv_t,stmt_t);

extern tenv_t add_label(tenv_t, var_t, stmt_t);
extern bool all_labels_resolved(tenv_t);

extern tenv_t new_block(seg_t,tenv_t);
extern tenv_t new_named_block(seg_t,tenv_t,tvar_t name);
extern type_t curr_rgn(tenv_t);
// Check that the region is in the current capability
extern void check_rgn_accessible(tenv_t,seg_t,type_t rgn);
// Check that an effect is a sub-effect of the current capability
extern void check_effect_accessible(tenv_t te, seg_t loc, type_t eff);
// Returns the region in which a function's parameters live
extern type_t parameter_rgn(tenv_t);
}
#endif
