#ifndef EARLEYBACKEND_H_
#define EARLEYBACKEND_H_

#include <list.h>
#include "semiring.h"
#include "earley-parsetree.h"
#include "dfa.h"
#include "ykbuf.h"

namespace EarleyAnyBackend {

  // various parameters that control printing of earley graph
  extern int print_earley_graph, eg_filt_lb, eg_filt_ub;
  
  
  extern $(List::list_t<$(EarleyParsetree::parse_tree_t,Semiring::weight_t)@>,int,int)
  parse(EarleyAnyBackend::DFA::edfa_t dfa_rep, const char? input);
  
  // If parse_until_eof is not enabled, this function will perform shortest-match.
  extern $(List::list_t<$(EarleyParsetree::parse_tree_t,Semiring::weight_t)@>,int,int)
  ykb_parse(EarleyAnyBackend::DFA::edfa_t dfa_rep, ykbuf_t@ ykb, int parse_until_eof);
  
  extern List::list_t<$(EarleyParsetree::spf_t, Semiring::weight_t) @> 
  ykb_parse_spf(EarleyAnyBackend::DFA::edfa_t dfa_rep, 
                ykbuf_t@ ykb, int parse_until_eof);
}

#endif /*EARLEYBACKEND_H_*/
