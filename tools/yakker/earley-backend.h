#ifndef EARLEYBACKEND_H_
#define EARLEYBACKEND_H_

#include <list.h>
//#include "bnf.h"
//#include "pm_bnf.h"
#include "semiring.h"

#ifdef USE_COMPILED_DFA

#define DFA_TY int

#else

#include "dfa.h"
#define DFA_TY DFA::Rep::dfa_rep_t

#endif

namespace EarleyBackend {

//	extern struct DFA::Rep::dfa_rep *rule2dfa(rule_t r, DFA::Kit::dfa_kit_t dfa_kit);
//	extern struct DFA::Rep::dfa_rep *pat2dfa(rule_pat_t p, DFA::Kit::dfa_kit_t dfa_kit);

  typedef List::list_t<datatype ParseTree @`H,`H> parse_forest_t;
  typedef List::List_t<datatype ParseTree @`H,`H> Parse_forest_t;

  typedef datatype ParseTree @`H parse_tree_t;

  extern datatype ParseTree{
    NonTerm(const char ?`H, int, int, Semiring::weight_t, parse_forest_t);
    SharedNonTerm(const char ?`H, int, int, Semiring::weight_t, 
    		List::List_t<parse_forest_t,`H>);
//		List::List_t<$(parse_forest_t,Semiring::weight_t) @`H,`H>);
  };
  

	extern $(List::list_t<$(parse_tree_t,Semiring::weight_t)@>,int,int)
	parse(DFA_TY dfa_rep, const char? input);
  extern void print_LT(datatype ParseTree @pt, int depth);
}

#endif /*EARLEYBACKEND_H_*/
