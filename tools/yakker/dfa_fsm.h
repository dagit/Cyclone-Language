#ifndef DFA_FSM_H_
#define DFA_FSM_H_
#include <core.h>
#include "fa.h"

extern act_t grm_get_symb_action(EarleyFsmBackend::DFA::grammar_edfa_t dfa, string_t<`H> symb);
extern st_t grm_get_symb_start(EarleyFsmBackend::DFA::grammar_edfa_t dfa, act_t symb_act);
extern unsigned int grm_get_num_states(EarleyFsmBackend::DFA::grammar_edfa_t dfa);

#define DFA_TRANS(dfa,s,a) (target(dfa->d,s,a))
#define DFA_TRANS_W(dfa,s,a) (target_w_weight(dfa->d,s,a))
#define DFA_GET_REPEAT_ACT(dfa,s) *(first_action_after(dfa->d,s,callout_action()))
#define DFA_GET_REPEATEE_ACT(dfa,s) *(first_action_after(dfa->d,s,0))
#define DFA_IN_FINAL(dfa,nt,s) (Set::member(dfa_final_attrs(dfa->d,s),nt))
#define DFA_IS_FINAL(dfa,s) (dfa_is_final(dfa->d,s))
#define DFA_R_EXTEND(dfa,nt,nt_start,nt_final) \
	construct_repeat_dfa(dfa->d,nt,repeat_decr_action(),nt_start,\
					                  repeat_final_action(), nt_final)
#define DFA_FINAL_WEIGHT(dfa,f) dfa_final_weight(dfa->d,f)
// XXX: find more efficient way to get array.
#define DFA_FINAL_ATTRS(dfa,f) wrap_final_attrs(dfa->d,f) 
#define DFA_GET_START(dfa) dfa->start
#define DFA_ACT2SYMB(dfa,a) act2symb(attr,dfa->action_symb_table);

// Macros for grammar DFAs.

#define GRM_DFA_GET_SYMB_ACTION(dfa,symb) grm_get_symb_action(dfa,symb)
#define GRM_DFA_GET_SYMB_START(dfa,a) grm_get_symb_start(dfa,a)
#define GRM_DFA_GET_NUM_STATES(dfa) grm_get_num_states(dfa)

#endif /*DFA_FSM_H_*/
