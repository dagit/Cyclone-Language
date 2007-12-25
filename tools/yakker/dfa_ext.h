#ifndef DFA_CYC_H_
#define DFA_CYC_H_

#include <core.h>
#include "fa.h"
#include "pm_bnf.h"

namespace ExtDFA{

using EarleyExtFsmBackend;

extern st_t trans(DFA::edfa_t ed, st_t s, act_t a);

extern $(st_t,Semiring::weight_t) trans_w(DFA::edfa_t ed, st_t s, act_t a);

extern int in_final(DFA::edfa_t ed, act_t a, st_t s);

extern int is_final(DFA::edfa_t ed, st_t s);

extern Semiring::weight_t final_weight(DFA::edfa_t ed, st_t s);

extern act_t ?final_attrs(DFA::edfa_t ed, st_t s);

extern st_t get_start(DFA::edfa_t ed);

extern string_t act_2_symb(DFA::edfa_t ed, act_t a);
}

#define DFA_TRANS(dfa,s,a) (ExtDFA::trans(dfa,s,a))
#define DFA_TRANS_W(dfa,s,a) (ExtDFA::trans_w(dfa,s,a))

// TODO: Need to implement
#define DFA_GET_REPEAT_ACT(dfa,s) ({fprintf(stderr,"Failure: DFA_GET_REPEAT_ACT unimplemented.\n");exit(1);0;})
#define DFA_GET_REPEATEE_ACT(dfa,s) ({fprintf(stderr,"Failure: DFA_GET_REPEATEE_ACT unimplemented.\n");exit(1);0;})
#define DFA_R_EXTEND(dfa,nt,nt_start,nt_final) ({fprintf(stderr,"Failure: DFA_GET_REPEATEE_ACT unimplemented.\n");exit(1);nt_final;})

#define DFA_IN_FINAL(dfa,nt,s) (ExtDFA::in_final(dfa,nt,s))
#define DFA_IS_FINAL(dfa,s) (ExtDFA::is_final(dfa,s))
#define DFA_FINAL_WEIGHT(dfa,f) (ExtDFA::final_weight(dfa,f))
#define DFA_FINAL_ATTRS(dfa,f) (ExtDFA::final_attrs(dfa,f))
#define DFA_GET_START(dfa) (ExtDFA::get_start(dfa))
#define DFA_ACT2SYMB(dfa,a) (ExtDFA::act_2_symb(dfa,a))

// TODO: Need to implement
#define GRM_DFA_GET_SYMB_ACTION(dfa,symb) ({fprintf(stderr,"Failure: GRM_DFA_GET_SYMB_ACTION unimplemented.\n");exit(1);0;})
#define GRM_DFA_GET_SYMB_START(dfa,a) ({fprintf(stderr,"Failure: GRM_DFA_GET_SYMB_START unimplemented.\n");exit(1);0;})
#define GRM_DFA_GET_NUM_STATES(dfa) ({fprintf(stderr,"Failure: GRM_DFA_GET_NUM_STATES unimplemented.\n");exit(1);0;})
#define GRM_DFA_GET_DFA(dfa,start) ({fprintf(stderr,"Failure: GRM_DFA_GET_DFA unimplemented.\n");exit(1);0;})


#endif /*DFA_CYC_H_*/
