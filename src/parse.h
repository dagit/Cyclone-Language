#ifndef PARSE_H
#define PARSE_H
#include "absyn.h"
#include "list.h"
#include "lexing.h"
using Core {
using Absyn {
using List {
using Lexing {
namespace Parse {
extern list<decl> parse_file(FILE @f);
extern Opt_t<Lexbuf<Function_lexbuf_state<FILE@>>> lbuf;
}
extern enum Struct_or_union;
typedef enum Struct_or_union struct_or_union;
extern enum Blockitem;
typedef enum Blockitem blockitem;
extern enum Type_specifier;
typedef enum Type_specifier type_specifier;
extern enum Storage_class;
typedef enum Storage_class storage_class;
extern struct Declaration_spec;
typedef struct Declaration_spec @decl_spec;
extern struct Declarator;
typedef struct Declarator @declarator;
extern struct Abstractdeclarator;
typedef struct Abstractdeclarator @abstractdeclarator;
#include "parse_tab.h"
}}}}
#endif
