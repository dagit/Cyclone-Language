#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include <core.h>
#include <stdio.h>
#include <list.h>
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

  // a new interface which describes what is automatically provided and needed when
  // building an executable file, so far :
  //   xtunion exn;
  //   extern int main(int, char ??);
  // the test whether a bunch of files whose interfaces are li = [i1..in] is linkable
  // to an executable file can be written :
  //   is_subinterface(empty(), merge_list(new List(final(), li), null))
  extern i_t final();
  
  // build a maximal interface from the global environment computed by the type-checker
  // also check that each (non extern) function prototyp is eventually provided
  extern i_t extract(Dict::dict_t<list_t<var_t>, genv_t> ae);

  // check if i1 may safely be used instead of i2 (ie. i1 is more restrictive than i2) :
  // _ everything provided by in i1 is provided by i2
  // _ everything needed by i2 was needed by i1
  // special case : typedefs may not be included, compatibility is enough 
  // if info == null, no error message is printed
  // else we use (*info)[k] as a name for ik
  extern bool is_subinterface(i_t i1, i_t i2, $(string_t, string_t) * `r info);

  // try to merge 2 interfaces
  // _ if they're incompatible, return null (and if info != null, print some error messages)
  // _ the new exports are e = e1 U[disjoint] e2
  // _ the new imports are i = (i1 U[compatible] i2) -[included] e
  // special case : a same typedef can appear in both e1 and e2
  // if info == null, no error message is printed
  // else we use (*info)[k] as a name for ik
  extern struct I * merge(i_t i1, i_t i2, $(string_t, string_t) * `r info);

  // try to merge a list of interfaces
  // use linfo (if long enough) for the error messages
  extern struct I * merge_list(list_t<i_t> li, list_t<string_t @> linfo);

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
