#ifndef LEXGEN_H
#define LEXGEN_H

#include "core.h"
#include "list.h"
#include "syntax.h"

#ifdef LEXGEN_CYC
#define LEXGEN_EXTERN_DEFINITION
#else
#define LEXGEN_EXTERN_DEFINITION extern
#endif

using Core {
using List {
namespace Lexgen {

// Representation of automata
LEXGEN_EXTERN_DEFINITION enum Automata_trans {
  No_remember;
  Remember(int);
};
typedef enum Automata_trans automata_trans_t;

LEXGEN_EXTERN_DEFINITION enum Automata_move {
  Backtrack;
  Goto(int);
};
typedef enum Automata_move automata_move_t;

LEXGEN_EXTERN_DEFINITION enum Automata { 
  Perform(int); 
  Shift(automata_trans_t, automata_move_t?);
};
typedef enum Automata automata_t;

LEXGEN_EXTERN_DEFINITION struct Automata_entry {
  string                           name;
  int                              initial_state;
  list_t<$(int,Syntax::location_t)@> actions;
};
typedef struct Automata_entry @ automata_entry_t;

extern 
$(list_t<automata_entry_t>,automata_t?)@ make_dfa(Syntax::lexer_definition_t);

}}}
#endif
