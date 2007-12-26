#ifndef DFA_CYC_H_
#define DFA_CYC_H_

#include <core.h>
#include "fa.h"

namespace CycDFA{
//char ?act2symb(act_t);
//extern int is_dfa_final(st_t);
extern $(st_t,Semiring::weight_t) transitions(st_t,act_t);
extern act_t ?attributes(st_t);
extern $(int,Semiring::weight_t) is_final(st_t);
extern string_t act2symb(act_t a);
extern const unsigned int num_dfa_states;
extern $(const char ?, act_t) ?@notnull symbol_table;

// implemented in earley-backend.cyc
extern act_t symb2act(string_t<`H> symb);
extern st_t grm_get_symb_start(EarleyCycBackend::DFA::grammar_edfa_t dfa, act_t symb_act);
}

static int array_find(act_t ?arr,act_t a){
  let n = numelts(arr);
  for (int i=0; i<n; i++){
    if (arr[i] == a) return i;
  }
  return -1;
}

#define DFA_TRANS(dfa,s,a) ({let $(t,w) = CycDFA::transitions(s,a); t;})
#define DFA_TRANS_W(dfa,s,a) (CycDFA::transitions(s,a))

// TODO: Need to implement
#define DFA_GET_REPEAT_ACT(dfa,s) ({fprintf(stderr,"Failure: DFA_GET_REPEAT_ACT unimplemented.\n");exit(1);0;})
#define DFA_GET_REPEATEE_ACT(dfa,s) ({fprintf(stderr,"Failure: DFA_GET_REPEATEE_ACT unimplemented.\n");exit(1);0;})
#define DFA_R_EXTEND(dfa,nt,nt_start,nt_final) ({fprintf(stderr,"Failure: DFA_GET_REPEATEE_ACT unimplemented.\n");exit(1);nt_final;})

#define DFA_IN_FINAL(dfa,nt,s) (array_find(CycDFA::attributes(s),nt) != -1)
#define DFA_IS_FINAL(dfa,s) (CycDFA::is_final(s).f0)
#define DFA_FINAL_WEIGHT(dfa,f) (CycDFA::is_final(f).f1)
#define DFA_FINAL_ATTRS(dfa,f) CycDFA::attributes(f)
#define DFA_GET_START(dfa) 1
#define DFA_ACT2SYMB(dfa,a) CycDFA::act2symb(a)

// Throws Not_found if symbol is not valid.
#define GRM_DFA_GET_SYMB_ACTION(dfa,symb) CycDFA::symb2act(symb)
#define GRM_DFA_GET_SYMB_START(dfa,a) CycDFA::grm_get_symb_start(dfa,a)
#define GRM_DFA_GET_NUM_STATES(dfa) (CycDFA::num_dfa_states)
#define GRM_DFA_GET_DFA(dfa,start) 0

#endif /*DFA_CYC_H_*/
