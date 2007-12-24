#ifndef DFA_FSM_H_
#define DFA_FSM_H_
#include <core.h>
#include "fa.h"

// Implementation of FSM-based DFA
namespace FsmDFA {
using EarleyFsmBackend;

extern att_t ?`H wrap_final_attrs(dfa_t dfa,st_t s);
extern const char ?act2symb(act_t act, Hashtable::table_t<act_t,str_t> as_table);

extern act_t grm_get_symb_action(DFA::grammar_edfa_t dfa, string_t<`H> symb);
extern st_t grm_get_symb_start(DFA::grammar_edfa_t dfa, act_t symb_act);
extern unsigned int grm_get_num_states(DFA::grammar_edfa_t dfa);
extern DFA::edfa_t grm_get_dfa(DFA::grammar_edfa_t dfa, st_t start);
}

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
#define DFA_FINAL_ATTRS(dfa,f) FsmDFA::wrap_final_attrs(dfa->d,f) 
#define DFA_GET_START(dfa) dfa->start
#define DFA_ACT2SYMB(dfa,a) FsmDFA::act2symb(a,dfa->action_symb_table)

// Macros for grammar DFAs.

#define GRM_DFA_GET_SYMB_ACTION(dfa,symb) FsmDFA::grm_get_symb_action(dfa,symb)
#define GRM_DFA_GET_SYMB_START(dfa,a) FsmDFA::grm_get_symb_start(dfa,a)
#define GRM_DFA_GET_NUM_STATES(dfa) FsmDFA::grm_get_num_states(dfa)
#define GRM_DFA_GET_DFA(dfa,start) FsmDFA::grm_get_dfa(dfa,start)

#endif /*DFA_FSM_H_*/
