#ifndef EARLEYBACKEND_H_
#define EARLEYBACKEND_H_

#include <list.h>
//#include "dfa.h"
//#include "bnf.h"
//#include "pm_bnf.h"
#include "semiring.h"

#define DFA_TY int
//#define DFA_TY DFA::Rep::dfa_rep_t

namespace EarleyBackend {

//	extern struct DFA::Rep::dfa_rep *rule2dfa(rule_t r, DFA::Kit::dfa_kit_t dfa_kit);
//	extern struct DFA::Rep::dfa_rep *pat2dfa(rule_pat_t p, DFA::Kit::dfa_kit_t dfa_kit);

	typedef datatype ParseTree @`H parse_tree_t;

	extern $(List::list_t<$(parse_tree_t,Semiring::weight_t)@>,int,int)
	parse(DFA_TY dfa_rep, const char? input);
}

#endif /*EARLEYBACKEND_H_*/
