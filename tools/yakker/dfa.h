#ifndef DFA_H_
#define DFA_H_

#include <set.h>
#include <hashtable.h>
#include "fa.h"
#include "axarray.h"
#include "earley.h"
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



namespace EarleyCycBackend{
namespace DFA {
  typedef int edfa_t;
  typedef int grammar_edfa_t;
}
}

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

namespace EarleyExtFsmBackend {
namespace DFA {
  struct edfa {
    EarleyFsmBackend::DFA::edfa_t d;
    Hashtable::table_t<$(st_t,act_t) @`H, $(st_t,Semiring::weight_t) @`H> trans_exts;
  };

  typedef struct edfa@ edfa_t;
}

}

#endif /*DFA_H_*/

