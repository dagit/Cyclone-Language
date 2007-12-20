// N.B. do *not* include anything in this file unless you want it
// to be in an earley backend namespace.
extern $(List::list_t<$(EarleyParsetree::parse_tree_t,Semiring::weight_t)@>,int,int)
  parse(DFA::edfa_t dfa_rep, const char? input);
