#ifndef EARLEYPARSETREE_H
#define EARLEYPARSETREE_H

#include <list.h>
#include "semiring.h"

namespace EarleyParsetree {
  typedef List::list_t<datatype SharedParseTree @`H,`H> shared_parse_forest_t;
  typedef List::List_t<datatype SharedParseTree @`H,`H> Shared_parse_forest_t;

  typedef datatype SharedParseTree @`H shared_parse_tree_t;

  extern datatype SharedParseTree {
    NonTerm(const char ?`H, int, int, Semiring::weight_t, shared_parse_forest_t);
    SharedNonTerm(const char ?`H, int, int, Semiring::weight_t,
		  List::List_t<shared_parse_forest_t,`H>);
  };

  typedef struct parse_tree @parse_tree_t;
  struct parse_tree {
    string_t nonterminal;
    int left;
    int right;
    Semiring::weight_t weight;
    parse_tree_t ?children;
  };

  extern void print_LT(datatype SharedParseTree @pt, int depth);
  extern void print_tree(parse_tree_t pt, int depth);
}

#endif /*EARLEYPARSETREE_H*/
