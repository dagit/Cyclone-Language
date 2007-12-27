/*
 * "Functor" implementing interface to extensible DFA. Parameters are checked below.
 */

#if !defined(EXT_DFA_NAMESPACE) || !defined(EB_NAMESPACE) || !defined(EB_EXT_NAMESPACE)
#error "Missing parameters to dfa_ext.h"
#endif

#include <core.h>
#include "fa.h"
#include "pm_bnf.h"

namespace EXT_DFA_NAMESPACE{

using EB_EXT_NAMESPACE;

extern st_t trans(DFA::edfa_t ed, st_t s, act_t a);

extern $(st_t,Semiring::weight_t) trans_w(DFA::edfa_t ed, st_t s, act_t a);

extern int in_final(DFA::edfa_t ed, act_t a, st_t s);

extern int is_final(DFA::edfa_t ed, st_t s);

extern Semiring::weight_t final_weight(DFA::edfa_t ed, st_t s);

extern act_t ?final_attrs(DFA::edfa_t ed, st_t s);

extern st_t get_start(DFA::edfa_t ed);

extern string_t act_2_symb(DFA::edfa_t ed, act_t a);

extern $(act_t, act_t, st_t, st_t) get_repeat_info(DFA::edfa_t ed, st_t state);

extern st_t construct_repeat_dfa(DFA::edfa_t ed, act_t next, 
                          act_t target_act, st_t target_s, 
                          act_t final_act, st_t final);
}

#define DFA_TRANS(dfa,s,a) (EXT_DFA_NAMESPACE::trans(dfa,s,a))
#define DFA_TRANS_W(dfa,s,a) (EXT_DFA_NAMESPACE::trans_w(dfa,s,a))

#define DFA_GET_REPEAT_INFO(dfa,s) EXT_DFA_NAMESPACE::get_repeat_info(dfa,s)
#define DFA_R_EXTEND(dfa,nt,nt_start,repeat_follow)  \
  EXT_DFA_NAMESPACE::construct_repeat_dfa(dfa,nt,repeat_decr_action(),nt_start,\
                                          repeat_final_action(), repeat_follow)

#define DFA_IN_FINAL(dfa,nt,s) (EXT_DFA_NAMESPACE::in_final(dfa,nt,s))
#define DFA_IS_FINAL(dfa,s) (EXT_DFA_NAMESPACE::is_final(dfa,s))
#define DFA_FINAL_WEIGHT(dfa,f) (EXT_DFA_NAMESPACE::final_weight(dfa,f))
#define DFA_FINAL_ATTRS(dfa,f) (EXT_DFA_NAMESPACE::final_attrs(dfa,f))
#define DFA_GET_START(dfa) (EXT_DFA_NAMESPACE::get_start(dfa))
#define DFA_ACT2SYMB(dfa,a) (EXT_DFA_NAMESPACE::act_2_symb(dfa,a))

// An extensible DFA does not implement these.
#define GRM_DFA_GET_SYMB_ACTION(dfa,symb) ({fprintf(stderr,"Failure: GRM_DFA_GET_SYMB_ACTION unimplemented.\n");exit(1);0;})
#define GRM_DFA_GET_SYMB_START(dfa,a) ({fprintf(stderr,"Failure: GRM_DFA_GET_SYMB_START unimplemented.\n");exit(1);0;})
#define GRM_DFA_GET_NUM_STATES(dfa) ({fprintf(stderr,"Failure: GRM_DFA_GET_NUM_STATES unimplemented.\n");exit(1);0;})
#define GRM_DFA_GET_DFA(dfa,start) ({fprintf(stderr,"Failure: GRM_DFA_GET_DFA unimplemented.\n");exit(1);0;})
