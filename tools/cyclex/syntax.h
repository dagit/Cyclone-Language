#ifndef SYNTAX_H
#define SYNTAX_H

#ifdef SYNTAX_CYC
#define SYNTAX_EXTERN_DEFINITION
#else
#define SYNTAX_EXTERN_DEFINITION extern
#endif

#include "list.h"

namespace Syntax {
using List {
  
SYNTAX_EXTERN_DEFINITION struct Location {
  int start_pos;
  int end_pos;
  int start_line;
  int start_col;
};
typedef struct Location @ location_t;

SYNTAX_EXTERN_DEFINITION enum Regular_expression {
  Epsilon;
  Characters(list<int>);
  Sequence(enum Regular_expression, enum Regular_expression);
  Alternative(enum Regular_expression, enum Regular_expression);
  Repetition(enum Regular_expression);
};
typedef enum Regular_expression regular_expression_t;

typedef $(regular_expression_t,location_t) @ acase_t;
typedef $(string,list<acase_t>) @ entrypoint_t;

SYNTAX_EXTERN_DEFINITION struct Lexer_definition {
  location_t         header;
  list<entrypoint_t> entrypoints;
  location_t         trailer;
};
typedef struct Lexer_definition * lexer_definition_t;

}}

#endif
