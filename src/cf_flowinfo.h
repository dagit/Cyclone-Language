#ifndef CF_FLOWINFO_H
#define CF_FLOWINFO_H

#include "list.h"
#include "set.h"
#include "dict.h"
#include "absyn.h"

// Note: Okasaki's dictionaries may be the wrong thing here because
//       we're doing a lot of intersections.  I don't know what's better,
//       but we do have all the dictionary's domains known in advance, so 
//       there probably is something better.

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
extern enum Escaped;
extern enum InitLevel;
extern enum InitState;
extern enum PathInfo;
extern enum FlowInfo;
typedef enum LocalRoot local_root_t;
typedef enum Field field_t;
typedef struct Place @place_t;
typedef enum Escaped escaped_t;
typedef enum InitLevel init_level_t;
typedef enum InitState init_state_t;
typedef enum PathInfo path_info_t;
typedef Dict::dict_t<`a,path_info_t> pinfo_dict_t<`a>;
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
EXTERN_CFFLOW enum Escaped   { Esc, Unesc };
EXTERN_CFFLOW enum InitLevel { NoneIL, ThisIL, AllIL };
EXTERN_CFFLOW enum InitState {
  UnknownIS(escaped_t,init_level_t);
  MustPointTo(place_t); // cannot have escaped
};
EXTERN_CFFLOW enum PathInfo {
  LeafPI(init_state_t);
  TuplePI(pinfo_dict_t<int>); // used for enum, xenum slots too
  StructPI(pinfo_dict_t<Absyn::field_name>);
};
// Note: It would be correct to make the domain of the pinfo_dict_t
//       constant (all local roots in the function), but it easy to argue
//       that we at program point p, we only need those roots that
//       are the target of a MustPointTo or are locals in scope (so they
//       might be mentioned explicitly in the program text).  A property
//       of the analysis must be that at least these roots stay in the dict;
//       for scalability reasons, we don't have others.
// join takes the intersection of the dictionaries.
EXTERN_CFFLOW enum FlowInfo {
  BottomFL;
  InitsFL(pinfo_dict_t<local_root_t>);
};

extern path_info_t mkLeafPI(escaped_t esc, init_level_t il);

extern int local_root_cmp(local_root_t, local_root_t);
extern int field_cmp(field_t, field_t);
extern int place_cmp(place_t, place_t);
extern path_info_t typ_to_unesc_none_pinfo(Absyn::typ t);

extern path_info_t lookup_place(pinfo_dict_t<local_root_t> d, place_t place);
extern pinfo_dict_t<local_root_t> insert_place(pinfo_dict_t<local_root_t> d, 
					       place_t place, 
					       path_info_t pinfo);

extern bool isAllInit(pinfo_dict_t<local_root_t> pinfo_dict, path_info_t pinfo);

extern pinfo_dict_t<local_root_t> escape_pointsto(path_info_t pinfo,
						  pinfo_dict_t<local_root_t> d);
extern path_info_t assign_unknown_dict(init_level_t, path_info_t);

extern flow_info_t join_flow(flow_info_t,flow_info_t);

extern bool flow_lessthan_approx(flow_info_t f1, flow_info_t f2);
}
#endif
