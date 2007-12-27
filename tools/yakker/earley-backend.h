#ifndef EARLEYBACKEND_H_
#define EARLEYBACKEND_H_

#include <list.h>
#include "semiring.h"
#include "earley-parsetree.h"
#include "dfa.h"

namespace EarleyAnyBackend {
extern $(List::list_t<$(EarleyParsetree::parse_tree_t,Semiring::weight_t)@>,int,int)
  parse(DFA::edfa_t dfa_rep, const char? input);
}

#endif /*EARLEYBACKEND_H_*/
