#ifndef CF_FLOWINFO_H
#define CF_FLOWINFO_H

#include "list.h"
#include "dict.h"
#include "absyn.h"

// A cute hack to avoid defining the abstract syntax twice.
#ifdef CF_FLOWINFO_CYC
#define EXTERN_CFFLOW
#else
#define EXTERN_CFFLOW extern
#endif

namespace CfFlowInfo {

extern enum LocalRoot;
extern enum Field;
extern struct Place;
extern enum InitState;
extern enum PathInfo;
extern enum FlowInfo;
typedef enum LocalRoot local_root_t;
typedef enum Field field_t;
typedef struct Place @place_t;
typedef enum InitState init_state_t;
typedef enum PathInfo path_info_t;
typedef Dict::dict_t<local_root_t, path_info_t> path_info_dict_t;
typedef enum FlowInfo flow_info_t;
// Do not ever mutate any data structures built from these -- they share like
// crazy.
// Maybe we'll make a lot of this abstract and cf_flowinfo will just
// publish the evaluation primitives (like assign, getr, and getl) ???
EXTERN_CFFLOW enum LocalRoot {
  VarRoot(Absyn::vardecl);
  MallocPt(Absyn::exp);
};
EXTERN_CFFLOW enum Field {
  StructF(Absyn::field_name);
  TupleF(int); // also used for enum and xenum field slots
  // maybe add some array support some day
};
EXTERN_CFFLOW struct Place {
  local_root_t root;
  List::list_t<field_t> fields;
};
EXTERN_CFFLOW enum InitState {
  Uninit;
  ThisInit;
  AllInit;
  MustPointTo(place_t);
};
// Note: Because each dictionary within a path_info_t has a constnat domain
//     (all tuple or struct fields for the object), there may be a data
//     structure better than a dict, but we must be functional/persistent.
// Note: The second field of InitsFL is "backward points-to malloc",
//       i.e. a list of who points somewhere into a malloc-point.
//       which is necessary to avoid walking through the whole FlowInfo
//       when evaluating MallocAE(e) -- see cf_absexp.cyc.
//       The domain of this dictionary is all the function's malloc points.
EXTERN_CFFLOW enum PathInfo {
  LeafPI(init_state_t);
  TuplePI(Dict::dict_t<int,path_info_t>); // used for enum, xenum slots too
  StructPI(Dict::dict_t<Absyn::field_name,path_info_t>);
};
// Note: It would be correct to make the domain of the path_info_dict_t
//       constnat (all local roots in the function), but for scalability
//       concerns, a local root not in the dict_t is equivalent to it
//       having all Uninit leaves.
EXTERN_CFFLOW enum FlowInfo {
  BottomFL;
  InitsFL(path_info_dict_t,
	  Dict::dict_t<Absyn::exp, List::list_t<place_t>>);
};

extern int local_root_cmp(local_root_t, local_root_t);
extern int field_cmp(field_t, field_t);
extern int place_cmp(place_t, place_t);
extern path_info_t typ_to_pathinfo_uninit(Absyn::typ t);

extern flow_info_t join_flow(flow_info_t,flow_info_t);
}
#endif
