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

SYNTAX_EXTERN_DEFINITION datatype Regular_expression {
  Epsilon(int);
  Characters(list_t<int>);
  Sequence(datatype Regular_expression@, datatype Regular_expression@);
  Alternative(datatype Regular_expression@, datatype Regular_expression@);
  Repetition(datatype Regular_expression@);
};
typedef datatype Regular_expression @regular_expression_t;
extern datatype Regular_expression.Epsilon Epsilon_val;
#ifdef SYNTAX_CYC
datatype Regular_expression.Epsilon Epsilon_val = Epsilon(0);
#endif

typedef $(regular_expression_t,location_t) @ acase_t;
typedef $(string_t,string_t,$(string_t,string_t)*,list_t<acase_t>) @ entrypoint_t;

SYNTAX_EXTERN_DEFINITION struct Lexer_definition {
  location_t         header;
  list_t<entrypoint_t> entrypoints;
  location_t         trailer;
};
typedef struct Lexer_definition * lexer_definition_t;

}}

#endif
