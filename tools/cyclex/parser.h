#ifndef PARSER_H
#define PARSER_H

#include "core.h"
#include "stdio.h"
#include "list.h"
#include "lexing.h"
#include "syntax.h"

using Core {
using List {
using Lexing {
using Syntax {
namespace Parser {
extern lexer_definition_t parse_file(FILE @f);
extern opt_t<Lexbuf<Function_lexbuf_state<FILE@>>> lbuf;
extern datatype exn {extern Parser_error(string_t)};
}
#include "parser_tab.h"
}}}}

#endif
