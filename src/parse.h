#ifndef _PARSE_H_
#define _PARSE_H_
#include "absyn.h"
#include "stdio.h"
#include "list.h"
#include "lexing.h"
using Core {
using Stdio {
using Absyn {
using List {
using Lexing {
namespace Parse {
extern list_t<decl> parse_file(FILE @f);
extern opt_t<Lexbuf<Function_lexbuf_state<FILE@>>> lbuf;
}
extern enum Struct_or_union;
typedef enum Struct_or_union struct_or_union_t;
extern enum Blockitem;
typedef enum Blockitem blockitem_t;
extern enum Type_specifier;
typedef enum Type_specifier type_specifier_t;
extern enum Storage_class;
typedef enum Storage_class storage_class_t;
extern struct Declaration_spec;
typedef struct Declaration_spec @decl_spec_t;
extern struct Declarator;
typedef struct Declarator @declarator_t;
extern struct Abstractdeclarator;
typedef struct Abstractdeclarator @abstractdeclarator_t;
#include "parse_tab.h"
}}}}}
#endif
