#ifndef NEW_CONTROL_FLOW_H
#define NEW_CONTROL_FLOW_H

namespace NewControlFlow {

#include "list.h"
#include "dict.h"
#include "stdio.h"
#include "absyn.h"

extern enum LocalRoot {
  VarRoot(Absyn::vardecl);
  MallocPt(Absyn::exp); // maybe allow enums and put variant explicitly here?
  // even now, maybe Enum_e should be a MallocPt so fields can be 
  // initialized later??? (nothing to be gained in porting though)
};

extern enum Field {
  StructF(Absyn::field_name);
  TupleF(int); // also used for enum and xenum field slots
  // maybe add some array support some day
};
typedef enum Field field_t;

typedef $(enum LocalRoot, List::list_t<field_t>) @path_t;

extern enum InitState {
  Uninit;
  ThisInit;
  AllInit;
  MustPointTo(path_t); // must this be a leaf??? (isn't in absexp right now!!)
  Bottom; 
};
typedef enum InitState init_state_t;

// Note: Because the dictionary has a known domain (all "local path leaves" for
// a function), there might be a better data structure than a dict_t.  Then
// again, it needs to be persistent in the functional sense.  
// Unknown not in domain
typedef Dict::dict_t<path_t,init_state_t> flow_info_t;

// The entry point (all the stuff above is just for cf_absexp??)
void abstract_decls(List::list_t<Absyn::decl> ds);
}

#endif
