#ifndef DFA_H_
#define DFA_H_

#include <set.h>
#include <hashtable.h>
#include "fa.h"
#include "axarray.h"
#include "earley.h"
#include "pm_bnf.h"
//#include "util.h"

/*
 * Abstractions:
 * 
 * edfa: a representation of an "executable" Earley dfa. Implemented
 *   either with code or with tables.
 * 
 * grammar_edfa: a dfa containing sub-dfas for each nonterminal in a grammar. augmented with table
 *   mapping nonterminals to their dfas. Implemented either with code or with tables.
 * 
 * ext_edfa: A dfa which supports extension with new states and transitions. Intended for use
 *   together w/ a grammar_dfa_t in order to compile simple regular expressoins over a grammar
 *   into "executable" dfas. An extensible dfa can be implemented in the same way for both
 *   code- or table-based dfas.
 */ 

namespace CycDFA {
  typedef st_t edfa_t; // the dfa is just its start state.
  typedef int grammar_edfa_t; // the grammar has no state
  
  extern st_t grm_get_symb_start(grammar_edfa_t dfa, act_t symb_act);
}

//
//namespace CycDFA {
//extern EarleyCycBackend::DFA::grammar_edfa_t cyc2grm_edfa();
//}

namespace EarleyFsmBackend{
namespace DFA {
  struct edfa {
    dfa_t d;
    st_t start;
    Hashtable::table_t<act_t,str_t> action_symb_table;
  };

  typedef struct edfa@ edfa_t;

  extern edfa_t create_edfa(dfa_t dfa, Earley::symb_info_t si);

  struct grammar_edfa {
    dfa_t d;
    Hashtable::table_t<act_t,str_t> action_symb_table;
    Hashtable::table_t<str_t,act_t> symb_action_table;
    Axarray::xarray_t<st_t> symb_start_states;
  };

  typedef struct grammar_edfa @grammar_edfa_t;
}
}

namespace FsmDFA {
extern EarleyFsmBackend::DFA::grammar_edfa_t fsm2grm_edfa(string_t filename);
}

namespace EarleyAnyBackend{
namespace DFA {
  struct edfa {<`a_dfa> : regions(`a_dfa) <= `H
    `a_dfa impl;
    st_t (@trans)(`a_dfa adfa, st_t s, act_t a);
    $(st_t,Semiring::weight_t) (@trans_w)(`a_dfa adfa, st_t s, act_t a);
    int (@in_final)(`a_dfa adfa, act_t a, st_t s);
    int (@is_final)(`a_dfa adfa, st_t s);
    Semiring::weight_t (@final_weight)(`a_dfa adfa, st_t s);
    act_t ?(@final_attrs)(`a_dfa adfa, st_t s);
    st_t (@get_start)(`a_dfa adfa);
    string_t (@act_2_symb)(`a_dfa adfa, act_t a);
    $(act_t, act_t, st_t, st_t) (@get_repeat_info)(`a_dfa adfa, st_t state);
    st_t (@construct_repeat_dfa)(`a_dfa adfa, act_t next, 
                              act_t target_act, st_t target_s, 
                              act_t final_act, st_t final);  
  };
  
  typedef struct edfa @ edfa_t;

  struct grammar_edfa {<`a_dfa> : regions(`a_dfa) <= `H
    `a_dfa impl;
    act_t (@get_symb_action)(`a_dfa dfa, string_t<`H> symb);
    st_t (@get_symb_start)(`a_dfa dfa, act_t symb_act);
    unsigned int (@get_num_states)(`a_dfa dfa);
    edfa_t (@get_dfa)(`a_dfa dfa, st_t start);
  };
  
  typedef struct grammar_edfa@ grammar_edfa_t;

}
}

#define EEB_DECLS(EXT_DFA_NAMESPACE, EB_NAMESPACE, EB_EXT_NAMESPACE)\
namespace EB_EXT_NAMESPACE {\
namespace DFA {\
  struct edfa {\
    Hashtable::table_t<$(st_t,act_t) @`H, $(st_t,Semiring::weight_t) @`H> trans_exts;\
    unsigned int number_of_states;\
    EB_NAMESPACE::DFA::edfa_t d;\
    st_t final; /* final state of the DFA.*/\
  };\
\
  typedef struct edfa@ edfa_t;\
}\
}\
\
namespace EXT_DFA_NAMESPACE{\
/* build an extensible DFA from a grammar DFA.*/\
extern EB_EXT_NAMESPACE::DFA::edfa_t \
pat2dfa(rule_pat_t p, EB_NAMESPACE::DFA::grammar_edfa_t grm_dfa);\
/* Print an extensible DFA to stderr. */\
extern void dfa_dot(EB_EXT_NAMESPACE::DFA::edfa_t ed);\
extern EarleyAnyBackend::DFA::edfa_t mk_any_eb(EB_EXT_NAMESPACE::DFA::edfa_t impl_dfa);\
}\
/* END EEB_DECLS*/  

EEB_DECLS(ExtDFA,   EarleyFsmBackend, EarleyExtFsmBackend)
//EEB_DECLS(ExtCycDFA,EarleyCycBackend, EarleyExtCycBackend)
EEB_DECLS(ExtAnyDFA,EarleyAnyBackend, EarleyExtAnyBackend)

/* This macro should be called within the namespace in which the dfa functions
 * are declared, or surrounded by a "using" declaration.*/
#define DEFINE_MK_ANY_EB(EB_NS) \
  EarleyAnyBackend::DFA::edfa_t mk_any_eb(EB_NS::DFA::edfa_t impl_dfa){\
    return new EarleyAnyBackend::DFA::edfa(impl_dfa,\
        trans,trans_w,in_final,is_final,final_weight,final_attrs,get_start,act_2_symb,\
        get_repeat_info,construct_repeat_dfa);\
  }

#endif /*DFA_H_*/

