#ifndef PARSER_H
#define PARSER_H

#include "core.h"
#include "stdio.h"
#include "list.h"
#include "lexing.h"
#include "syntax.h"

using Core {
using Stdio {
using List {
using Lexing {
using Syntax {
namespace Parser {
extern lexer_definition_t parse_file(FILE @f);
extern opt_t<Lexbuf<Function_lexbuf_state<FILE@>>> lbuf;
extern xtunion exn {Parser_error(string)};
}
#include "parser_tab.h"
}}}}}

#endif
