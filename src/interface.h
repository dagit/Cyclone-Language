#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include "core.h"
#include "stdio.h"
#include "list.h"
#include "absyn.h"
#include "tcenv.h"

namespace Interface {
  using List;
  using Absyn;
  using Stdio;
  using Tcenv;

  // abstract type for program interfaces
  extern struct I;
  typedef struct I @ i_t;

  // a new empty interface
  extern i_t empty();
    
  // build a maximal interface from the global environment computed by the type-checker
  // also check that each (non extern) function prototyp is eventually provided
  extern i_t extract(Dict::dict_t<list_t<var_t>, genv_t> ae);

  // check if i1 may safely be used instead of i2 :
  // _ everything provided by in i2 is provided by i1
  // _ everything needed by i1 was needed by i2
  // if info == null, no error message is printed
  // else we use (*info)[k] as a name for ik
  extern bool is_subinterface(i_t i1, i_t i2, $(string, string) * `r info);

  // try to merge 2 interfaces
  // _ if they're incompatible, return null (and if info != null, print some error messages)
  // _ the new exports are e = e1 U[disjoint] e2
  // _ the new imports are i = (i1 U[compatible] i2) -[included] e
  // special cases :
  // _ a typedef can appear in both e1 and e2
  // _ some fields of a xtunion may be exported, whereas some others are imported (it's a mess !)
  extern struct I * merge(i_t i1, i_t i2, $(string, string) * `r info);

  // extern struct I * merge_list(list_t<i_t> li);

  // pretty print to a text file 
  extern void print(i_t, FILE @);

  // load from a text file
  extern i_t parse(FILE @);

  /*
  // binary save to a file
  extern void save(i_t, FILE @);

  // load from a text file
  extern i_t load(FILE @);
  */
}

typedef Interface::i_t interface_t;

#endif /* _INTERFACE_H_ */
