#ifndef COMPACT_H
#define COMPACT_H

#include "lexgen.h"

namespace Compact {

extern struct Lex_tables {
  int ?base;     // perform/shift
  int ?backtrk;  // No_remember / Remember
  int ?defaultX; // Default transition
  int ?trans;    // Transitions (compacted)
  int ?check;    // Check (compacted)
};
typedef struct Lex_tables @ lex_tables_t;

extern lex_tables_t compact_tables(Lexgen::automata_t ?);

}

#endif
