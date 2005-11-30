#ifndef _BANSHEE_IF_H_
#define _BANSHEE_IF_H_
#include <stdio.h>
#include "absyn.h"
#include "core.h"
#include "list.h"

namespace BansheeIf {
  extern datatype Constraint;
  typedef datatype Constraint @constraint_t, *constraint_opt_t;
  
  
  bool banshee_init();
  void add_constant(string_t name, Absyn::cvartype_t c);
  //  void add_variable(Absyn::cvartype_t cvar);

  constraint_t equality_constraint(Absyn::cvartype_t, Absyn::cvartype_t);
  constraint_t cond_equality_constraint(Absyn::cvartype_t, Absyn::cvartype_t);
  constraint_t inclusion_constraint(Absyn::cvartype_t, Absyn::cvartype_t);
  constraint_t implication_constraint(constraint_t, constraint_t);
  constraint_t and_constraint(constraint_t, constraint_t);
  constraint_t or_constraint(constraint_t, constraint_t);
  constraint_t not_constraint(constraint_t);
  constraint_t cmpeq_constraint(Absyn::cvartype_t t1, Absyn::cvartype_t t2);
  constraint_t check_constraint(constraint_t);
  string_t constraint2string(constraint_t);

  bool add_constraint(Absyn::seg_t, constraint_t);
  int discharge_implications();
  bool resolve(Absyn::cvartype_t);

  //persistence stuff
  void persist_cvarmap(FILE @f);
  void reset_local_cvar(Absyn::type_t cvar);
  void register_toplevel_cvar(Absyn::type_t cvar);
  void init_toplevel_cvars(List::list_t<$(Absyn::type_t, List::list_t<constraint_t,`H>)@>);
  //resolution
  void resolve_or_default(Absyn::type_t t);
  //for statistics
  int add_all_constraints(List::list_t<$(Absyn::type_t, List::list_t<constraint_t,`H>)@>);
}
#endif
