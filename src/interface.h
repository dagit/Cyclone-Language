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
  extern struct T;
  typedef struct T @ t;
    
  // build a maximal interface from the global environment computed by the type-checker
  // also check that each (non extern) function prototyp is eventually provided
  extern t extract(Dict::dict_t<list_t<var_t>, genv_t> ae);

  // check if i1 may safely be used instead of i2 :
  // _ everything provided by in i2 is provided by i1
  // _ everything needed by i1 was needed by i2
  extern bool is_subinterface(t i1, t i2);

  // extern bool consistent(list_t<t>);

  // try to merge 2 interfaces
  // extern t merge(t, t);

  // pretty print to a text file 
  extern void print(t, FILE @);

  // load from a text file
  extern t parse(FILE @);

  /*
  // binary save to a file
  extern void save(t, FILE @);

  // load from a text file
  extern t load(FILE @);
  */
}

#endif /* _INTERFACE_H_ */
