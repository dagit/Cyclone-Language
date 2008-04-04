#ifndef EARLEYPARSETREE_H
#define EARLEYPARSETREE_H

#include <list.h>
#include "semiring.h"

namespace EarleyParsetree {
  
// DEPRECATED
  typedef List::list_t<datatype SharedParseTree @`H,`H> shared_parse_forest_t;
  typedef List::List_t<datatype SharedParseTree @`H,`H> Shared_parse_forest_t;

  typedef datatype SharedParseTree @`H shared_parse_tree_t;

  extern datatype SharedParseTree {
    NonTerm(const char ?`H, int, int, Semiring::weight_t, shared_parse_forest_t);
    SharedNonTerm(const char ?`H, int, int, Semiring::weight_t,
		  List::List_t<shared_parse_forest_t,`H>);
  };
  extern void print_LT(datatype SharedParseTree @pt, int depth);
// END DEPRECATED

  typedef struct parse_tree @parse_tree_t;
  struct parse_tree {
    string_t nonterminal;
    int left;
    int right;
    Semiring::weight_t weight;
    parse_tree_t ?children;
  };

  typedef struct shared_parse_forest @spf_t;
  struct shared_parse_forest {
    string_t nonterminal;
    int left;
    int right;
    Semiring::weight_t weight;
    /**
     * Each tree represents an alternative RHS.
     * each RHS is a list.
     */
    List::list_t<spf_t> ? trees; 
  };

  extern void print_tree(parse_tree_t pt, int depth);
  extern int print_first_ambiguity(spf_t spf);
  extern void print_forest(spf_t spf, string_t indent);
}

#endif /*EARLEYPARSETREE_H*/
