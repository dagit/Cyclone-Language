#ifdef USE_COMPILED_DFA
#include "dfa_cyc.h"

#elif defined(USE_FSM_DFA)
#include "dfa_fsm.h"

#elif defined(USE_EXT_DFA)
#ifndef DFA_EXT_H_
#define DFA_EXT_H_

#define EXT_DFA_NAMESPACE ExtDFA
#define EB_NAMESPACE EarleyFsmBackend
#define EB_EXT_NAMESPACE EarleyExtFsmBackend
#include "dfa_ext.h"

#endif /*DFA_EXT_H_*/

#elif defined(USE_EXT_CYC_DFA)
#ifndef DFA_EXT_CYC_H_
#define DFA_EXT_CYC_H_

#define EXT_DFA_NAMESPACE ExtCycDFA
#define EB_NAMESPACE EarleyCycBackend
#define EB_EXT_NAMESPACE EarleyExtCycBackend
#include "dfa_ext.h"

#endif /*DFA_EXT_CYC_H_*/

#elif defined(USE_ANY_DFA)
#ifndef DFA_ANY_H_
#define DFA_ANY_H_

#define DFA_TRANS(dfa,s,a) ({let &{.impl=impl,.trans=trans,...} = dfa; trans(impl,s,a);})
#define DFA_TRANS_W(dfa,s,a) ({let &{.impl=impl,.trans_w=trans_w,...} = dfa; trans_w(impl,s,a);})
#define DFA_GET_REPEAT_INFO(dfa,s) ({let &{.impl=impl,.get_repeat_info=get_repeat_info,...} = dfa;  \
  get_repeat_info(impl,s);})
#define DFA_R_EXTEND(dfa,nt,nt_start,repeat_follow)  \
  ({let &{.impl=impl,.construct_repeat_dfa=construct_repeat_dfa,...} = dfa;  \
    construct_repeat_dfa(impl,nt,repeat_decr_action(),nt_start,\
                                          repeat_final_action(), repeat_follow);})
#define DFA_IN_FINAL(dfa,nt,s) ({let &{.impl=impl,.in_final=in_final,...} = dfa;  in_final(impl,nt,s);})
#define DFA_IS_FINAL(dfa,s) ({let &{.impl=impl,.is_final=is_final,...} = dfa;  is_final(impl,s);})
#define DFA_FINAL_WEIGHT(dfa,f) ({let &{.impl=impl,.final_weight=final_weight,...} = dfa;  final_weight(impl,f);})
#define DFA_FINAL_ATTRS(dfa,f) ({let &{.impl=impl,.final_attrs=final_attrs,...} = dfa;  final_attrs(impl,f);})
#define DFA_GET_START(dfa) ({let &{.impl=impl,.get_start=get_start,...} = dfa;  get_start(impl);})
#define DFA_ACT2SYMB(dfa,a) ({let &{.impl=impl,.act_2_symb=act_2_symb,...} = dfa;  act_2_symb(impl,a);})

#define GRM_DFA_GET_SYMB_ACTION(dfa,symb) ({let &{.impl=impl,.get_symb_action=gsa,...} = dfa;  gsa(impl,symb);})
#define GRM_DFA_GET_SYMB_START(dfa,a) ({let &{.impl=impl,.get_symb_start=gss,...} = dfa;  gss(impl,a);})
#define GRM_DFA_GET_NUM_STATES(dfa) ({let &{.impl=impl,.get_num_states=gns,...} = dfa;  gns(impl);})
#define GRM_DFA_GET_DFA(dfa,start) ({let &{.impl=impl,.get_dfa=gd,...} = dfa;  gd(impl,start);})

#endif /*DFA_ANY_H_*/

#else
#error "No DFA implementation specified."
#endif
