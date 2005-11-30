/* Automatically generated file, do not edit. See nodetypes.def */

/* Copyright (c) 2000-200 The Regents of the University of California. */
/* See the copyright notice in nodetypes.def for distribution restrictions */
node new_node(region r, location location);
declaration new_declaration(region r, location location);
statement new_statement(region r, location location);
expression new_expression(region r, location location);
type_element new_type_element(region r, location location);
declarator new_declarator(region r, location location);
label new_label(region r, location location);
asm_decl new_asm_decl(region r, location location, asm_stmt asm_stmt);
data_decl new_data_decl(region r, location location, type_element modifiers, attribute attributes, declaration decls);
extension_decl new_extension_decl(region r, location location, declaration decl);
ellipsis_decl new_ellipsis_decl(region r, location location);
enumerator new_enumerator(region r, location location, cstring cstring, expression arg1, data_declaration ddecl);
oldidentifier_decl new_oldidentifier_decl(region r, location location, cstring cstring, data_declaration ddecl);
function_decl new_function_decl(region r, location location, declarator declarator, type_element qualifiers, attribute attributes, declaration old_parms, statement stmt, function_decl parent_function, data_declaration ddecl);
implicit_decl new_implicit_decl(region r, location location, identifier ident);
variable_decl new_variable_decl(region r, location location, declarator declarator, attribute attributes, expression arg1, asm_stmt asm_stmt, data_declaration ddecl);
field_decl new_field_decl(region r, location location, declarator declarator, attribute attributes, expression arg1);
asttype new_asttype(region r, location location, declarator declarator, type_element qualifiers);
typename new_typename(region r, location location, data_declaration ddecl);
typeof_expr new_typeof_expr(region r, location location, expression arg1);
typeof_type new_typeof_type(region r, location location, asttype asttype);
attribute new_attribute(region r, location location, word word1, word word2, expression args);
rid new_rid(region r, location location, enum rid id);
qualifier new_qualifier(region r, location location, enum rid id);
tag_ref new_tag_ref(region r, location location, word word1, attribute attributes, declaration fields, bool defined);
function_declarator new_function_declarator(region r, location location, declarator declarator, declaration parms, type_element qualifiers, environment env);
pointer_declarator new_pointer_declarator(region r, location location, declarator declarator, type_element qualifiers);
array_declarator new_array_declarator(region r, location location, declarator declarator, expression arg1);
identifier_declarator new_identifier_declarator(region r, location location, cstring cstring);
asm_stmt new_asm_stmt(region r, location location, expression arg1, asm_operand asm_operands1, asm_operand asm_operands2, string asm_clobbers, type_element qualifiers);
compound_stmt new_compound_stmt(region r, location location, id_label id_labels, declaration decls, statement stmts, environment env);
if_stmt new_if_stmt(region r, location location, expression condition, statement stmt1, statement stmt2);
labeled_stmt new_labeled_stmt(region r, location location, label label, statement stmt);
expression_stmt new_expression_stmt(region r, location location, expression arg1);
conditional_stmt new_conditional_stmt(region r, location location, expression condition, statement stmt);
switch_stmt new_switch_stmt(region r, location location, expression condition, statement stmt);
for_stmt new_for_stmt(region r, location location, expression arg1, expression arg2, expression arg3, statement stmt);
break_stmt new_break_stmt(region r, location location);
continue_stmt new_continue_stmt(region r, location location);
return_stmt new_return_stmt(region r, location location, expression arg1);
goto_stmt new_goto_stmt(region r, location location, id_label id_label);
computed_goto_stmt new_computed_goto_stmt(region r, location location, expression arg1);
empty_stmt new_empty_stmt(region r, location location);
unary new_unary(region r, location location, expression arg1);
binary new_binary(region r, location location, expression arg1, expression arg2);
comma new_comma(region r, location location, expression arg1);
sizeof_type new_sizeof_type(region r, location location, asttype asttype);
alignof_type new_alignof_type(region r, location location, asttype asttype);
label_address new_label_address(region r, location location, id_label id_label);
cast new_cast(region r, location location, expression arg1, asttype asttype);
cast_list new_cast_list(region r, location location, asttype asttype, expression init_expr);
conditional new_conditional(region r, location location, expression condition, expression arg1, expression arg2);
identifier new_identifier(region r, location location, cstring cstring, data_declaration ddecl);
compound_expr new_compound_expr(region r, location location, statement stmt);
function_call new_function_call(region r, location location, expression arg1, expression args, asttype va_arg_call);
field_ref new_field_ref(region r, location location, expression arg1, cstring cstring);
assign new_assign(region r, location location, expression arg1, expression arg2);
init_list new_init_list(region r, location location, expression args);
init_index new_init_index(region r, location location, expression arg1, expression arg2, expression init_expr);
init_field new_init_field(region r, location location, word word1, expression init_expr);
known_cst new_known_cst(region r, location location, cval cval);
lexical_cst new_lexical_cst(region r, location location, cstring cstring);
string_cst new_string_cst(region r, location location, cstring cstring, wchar_t * chars, size_t length);
string new_string(region r, location location, expression strings, data_declaration ddecl);
id_label new_id_label(region r, location location, cstring cstring);
case_label new_case_label(region r, location location, expression arg1, expression arg2);
default_label new_default_label(region r, location location);
word new_word(region r, location location, cstring cstring);
asm_operand new_asm_operand(region r, location location, string string, expression arg1);
adjust_rc new_adjust_rc(region r, location location, expression arg1, bool incrementrc);
generic_rc new_generic_rc(region r, location location, expression arg1, expression arg2);
update_rc new_update_rc(region r, location location, expression arg1, expression arg2, data_declaration ddecl);
adjust_function_decl new_adjust_function_decl(region r, location location);
adjustarray_function_decl new_adjustarray_function_decl(region r, location location);
update_function_decl new_update_function_decl(region r, location location);
error_decl new_error_decl(region r, location location);
struct_ref new_struct_ref(region r, location location, word word1, attribute attributes, declaration fields, bool defined);
union_ref new_union_ref(region r, location location, word word1, attribute attributes, declaration fields, bool defined);
enum_ref new_enum_ref(region r, location location, word word1, attribute attributes, declaration fields, bool defined);
error_stmt new_error_stmt(region r, location location);
while_stmt new_while_stmt(region r, location location, expression condition, statement stmt);
dowhile_stmt new_dowhile_stmt(region r, location location, expression condition, statement stmt);
error_expr new_error_expr(region r, location location);
array_ref new_array_ref(region r, location location, expression arg1, expression arg2);
dereference new_dereference(region r, location location, expression arg1);
extension_expr new_extension_expr(region r, location location, expression arg1);
sizeof_expr new_sizeof_expr(region r, location location, expression arg1);
alignof_expr new_alignof_expr(region r, location location, expression arg1);
realpart new_realpart(region r, location location, expression arg1);
imagpart new_imagpart(region r, location location, expression arg1);
address_of new_address_of(region r, location location, expression arg1);
unary_minus new_unary_minus(region r, location location, expression arg1);
unary_plus new_unary_plus(region r, location location, expression arg1);
conjugate new_conjugate(region r, location location, expression arg1);
preincrement new_preincrement(region r, location location, expression arg1);
predecrement new_predecrement(region r, location location, expression arg1);
postincrement new_postincrement(region r, location location, expression arg1);
postdecrement new_postdecrement(region r, location location, expression arg1);
bitnot new_bitnot(region r, location location, expression arg1);
not new_not(region r, location location, expression arg1);
plus new_plus(region r, location location, expression arg1, expression arg2);
minus new_minus(region r, location location, expression arg1, expression arg2);
times new_times(region r, location location, expression arg1, expression arg2);
divide new_divide(region r, location location, expression arg1, expression arg2);
modulo new_modulo(region r, location location, expression arg1, expression arg2);
lshift new_lshift(region r, location location, expression arg1, expression arg2);
rshift new_rshift(region r, location location, expression arg1, expression arg2);
leq new_leq(region r, location location, expression arg1, expression arg2);
geq new_geq(region r, location location, expression arg1, expression arg2);
lt new_lt(region r, location location, expression arg1, expression arg2);
gt new_gt(region r, location location, expression arg1, expression arg2);
eq new_eq(region r, location location, expression arg1, expression arg2);
ne new_ne(region r, location location, expression arg1, expression arg2);
bitand new_bitand(region r, location location, expression arg1, expression arg2);
bitor new_bitor(region r, location location, expression arg1, expression arg2);
bitxor new_bitxor(region r, location location, expression arg1, expression arg2);
andand new_andand(region r, location location, expression arg1, expression arg2);
oror new_oror(region r, location location, expression arg1, expression arg2);
plus_assign new_plus_assign(region r, location location, expression arg1, expression arg2);
minus_assign new_minus_assign(region r, location location, expression arg1, expression arg2);
times_assign new_times_assign(region r, location location, expression arg1, expression arg2);
divide_assign new_divide_assign(region r, location location, expression arg1, expression arg2);
modulo_assign new_modulo_assign(region r, location location, expression arg1, expression arg2);
lshift_assign new_lshift_assign(region r, location location, expression arg1, expression arg2);
rshift_assign new_rshift_assign(region r, location location, expression arg1, expression arg2);
bitand_assign new_bitand_assign(region r, location location, expression arg1, expression arg2);
bitor_assign new_bitor_assign(region r, location location, expression arg1, expression arg2);
bitxor_assign new_bitxor_assign(region r, location location, expression arg1, expression arg2);
global_rc new_global_rc(region r, location location, expression arg1, expression arg2);


/* The common supertype of all AST nodes
VERTEX points to the node that represents the vertex of this cfg node after
collapsing the graph. VERTEX_NEXT forms a list of all nodes collapsed to
the same vertex.
MF_HEIGHT and MF_EXCESS are used by the maxflow algorithm.
SR_VERTEX is the vertex (or null for none) of this node in the reduced
sameregion-optimisation cfg */
struct AST_node { /* extends nil */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
};

/* The common type of all definitions */
struct AST_declaration { /* extends node */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
};

/* The common type of all statements
PARENT_LOOP: 
  - for break and continue: the containing for/while/do-while/switch
    statement they escape from
  - for for/while/do-while: the containing for/while/do-while/switch
    statement
BREAK_DEST and CONTINUE_DEST: for for/while/do-while/switch statements
the CFG node to which break or continue should go to */
struct AST_statement { /* extends node */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  statement parent_loop;
  node break_dest;
  node continue_dest;
};

/* Expression nodes are subtypes of this type.
LVALUE is true if this expression can be used in a context requiring an lvalue.
BITFIELD is true if this lvalue is a bitfield.
ISREGISTER is true if this lvalue is (declared to be) in a register.
SIDE_EFFECTS is true if the expression has side effects.
CST is non-null (and points to an appropriate constant) if this expression is
constant.
STATIC_ADDRESS is true for lvalues whose address is a constant expression
LREGION and RREGION specify the region where the address of this value (when
meaningful) and the actual value are known to live. See chkopt.[ch]. LREGION
and RREGION are generalised places. */
struct AST_expression { /* extends node */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  bool lvalue;
  bool side_effects;
  known_cst cst;
  bool bitfield;
  bool isregister;
  known_cst static_address;
  gplace lregion;
  gplace rregion;
};

/* A common super-type for all type-building elements (qualifiers, etc) */
struct AST_type_element { /* extends node */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
};

/* A common super-type for all declarator elements */
struct AST_declarator { /* extends node */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
};

/* A common super-type for all labels.
NEXT_LABEL points to the next case or default label of a switch 
(for case or default labels only) */
struct AST_label { /* extends node */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  label next_label;
};

/* Asm statement STMT at the top level of a file (GCC) */
struct AST_asm_decl { /* extends declaration */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  asm_stmt sameregion asm_stmt;
};

/* The declaration MODIFIERS ATTRIBUTES DECLS;
DECLS is a list */
struct AST_data_decl { /* extends declaration */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  type_element sameregion modifiers;
  attribute sameregion attributes;
  declaration sameregion decls;
};

/* __extension__ DECL; (GCC) */
struct AST_extension_decl { /* extends declaration */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  declaration sameregion decl;
};

/* A pseudo-declaration to represent ... in a function argument list */
struct AST_ellipsis_decl { /* extends declaration */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
};

/* The enumeration element CSTRING = ARG1. CSTRING is optional */
struct AST_enumerator { /* extends declaration */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  cstring cstring;
  expression sameregion arg1;
  data_declaration ddecl;
};

/* CSTRING in an old-style parameter list */
struct AST_oldidentifier_decl { /* extends declaration */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  cstring cstring;
  data_declaration ddecl;
};

/* A function definition with body STMT.
OLD_PARMS is the old-style parameter declaration list.
CFG_ENTRY is the first vertex of the function's cfg.
RC_FNS is a list of adjust_function_decl or update_function_decl that
should be generated before this function.
 */
struct AST_function_decl { /* extends declaration */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  declarator sameregion declarator;
  type_element sameregion qualifiers;
  attribute sameregion attributes;
  declaration old_parms;
  statement sameregion stmt;
  function_decl parent_function;
  data_declaration ddecl;
  function_declarator fdeclarator;
  type declared_type;
  env undeclared_variables;
  env base_labels;
  env scoped_labels;
  statement current_loop;
  node cfg_entry;
  node parentptr * cfg_nodes;
  long cfg_nextnode;
  long cfg_size;
  node parentptr * postorder_nodes;
  node parentptr * preorder_nodes;
  long postorder_size;
  dd_list addressed_labels;
  int nlocals;
  data_declaration * vmap;
  declaration rc_fns;
  ggraph sr_graph;
  gnode sr_entry;
  data_declaration * sr_vmap;
  int sr_nvars;
  place sr_return_id;
  int sr_nptrargs;
  bool sr_unknown_arguments;
  struct sr_state sr_temp;
  place * sr_local2placeidmap;
  gnode * sr_preorder;
  int sr_size;
  double chkopt_time;
  generic_rc sameregion *sameregion allrcops;
};

/* Used as the AST node for implicit declarations. IDENT points to the
identifier node that implicitly declared the function */
struct AST_implicit_decl { /* extends declaration */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  identifier ident;
};

/* Declaration of ATTRIBUTES ASM_STMT DECLARATOR = ARG1.
ATTRIBUTEES is a list. ASM_STMT is optional (GCC specific).
ARG1 is an optional initialiser.
DDECL points to the declaration for this item.
DECLARED_TYPE is the type in this declaration (which may be different than that
in DDECL->TYPE) */
struct AST_variable_decl { /* extends declaration */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  declarator sameregion declarator;
  attribute sameregion attributes;
  expression sameregion arg1;
  asm_stmt sameregion asm_stmt;
  data_declaration ddecl;
  type declared_type;
};

/* Declaration of field QUALIFIERS ATTRIBUTES DECLARATOR : ARG1.
QUALIFIERS and ATTRIBUTEES are lists. ARG1 is an optional bitfield specifier */
struct AST_field_decl { /* extends declaration */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  declarator sameregion declarator;
  attribute sameregion attributes;
  expression sameregion arg1;
};

/* The source-level type QUALIFIERS DECLARATOR */
struct AST_asttype { /* extends node */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  declarator sameregion declarator;
  type_element sameregion qualifiers;
};

/* typedef-type with declaration DDECL. The name is ddecl->name */
struct AST_typename { /* extends type_element */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  data_declaration ddecl;
};

/* typeof(ARG1) */
struct AST_typeof_expr { /* extends type_element */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  expression sameregion arg1;
};

/* typeof(ASTTYPE) */
struct AST_typeof_type { /* extends type_element */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  asttype sameregion asttype;
};

/* The attribute WORD1(WORD2, ARGS). word2 and args are optional */
struct AST_attribute { /* extends type_element */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  word sameregion word1;
  word sameregion word2;
  expression sameregion args;
};

/* Storage class specifier, type specifier or type qualifier ID (see RID_xxx) */
struct AST_rid { /* extends type_element */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  enum rid id;
};

/* Type or function qualifier ID (see qualifiers.h and type_quals in types.h) */
struct AST_qualifier { /* extends type_element */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  enum rid id;
};

/* struct/union/enum WORD1 { FIELDS; }  ATTRIBUTES
ATTRIBUTES and FIELDS are lists.
ATTRIBUTES is GCC specific. WORD1 is optional.
DEFINED is TRUE if this declaration defines the struct/union/enum.
DEFINED == FALSE => FIELDS == NULL

TDECL points to the internal declaration node for this type */
struct AST_tag_ref { /* extends type_element */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  word sameregion word1;
  attribute sameregion attributes;
  declaration sameregion fields;
  bool defined;
  tag_declaration tdecl;
};

/* Function declarator DECLARATOR(PARMS). PARMS is a list of declarations.
ENV is the environment for parms */
struct AST_function_declarator { /* extends declarator */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  declarator sameregion declarator;
  declaration sameregion parms;
  type_element sameregion qualifiers;
  environment env;
};

/* Pointer declarator *QUALIFIERS DECLARATOR */
struct AST_pointer_declarator { /* extends declarator */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  declarator sameregion declarator;
  type_element sameregion qualifiers;
};

/* Array declarator DECLARATOR[ARG1]. ARG1 is optional */
struct AST_array_declarator { /* extends declarator */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  declarator sameregion declarator;
  expression sameregion arg1;
};

/* Declaration of CSTRING */
struct AST_identifier_declarator { /* extends declarator */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  cstring cstring;
};

/* The statement asm QUALIFIERS (ARG1 : ASM_OPERANDS1 : ASM_OPERANDS2 : ASM_CLOBBERS) 
where ASM_OPERANDS1, ASM_OPERANDS2, QUALIFIERS are optional, ASM_CLOBBERS is a list (GCC) */
struct AST_asm_stmt { /* extends statement */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  statement parent_loop;
  node break_dest;
  node continue_dest;
  expression sameregion arg1;
  asm_operand sameregion asm_operands1;
  asm_operand sameregion asm_operands2;
  string sameregion asm_clobbers;
  type_element sameregion qualifiers;
};

/* { ID_LABELS DECLS STMTS }. The ID_LABELS are GCC-specific. ID_LABELS, DECLS,
STMTS are lists
ENV is the environment for the block */
struct AST_compound_stmt { /* extends statement */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  statement parent_loop;
  node break_dest;
  node continue_dest;
  id_label sameregion id_labels;
  declaration sameregion decls;
  statement sameregion stmts;
  environment env;
};

/* IF (CONDITION) STMT1 ELSE STMT2. STMT2 is optional */
struct AST_if_stmt { /* extends statement */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  statement parent_loop;
  node break_dest;
  node continue_dest;
  expression sameregion condition;
  statement sameregion stmt1;
  statement sameregion stmt2;
};

/* LABEL: STMT */
struct AST_labeled_stmt { /* extends statement */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  statement parent_loop;
  node break_dest;
  node continue_dest;
  label sameregion label;
  statement sameregion stmt;
};

/* EXPR; */
struct AST_expression_stmt { /* extends statement */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  statement parent_loop;
  node break_dest;
  node continue_dest;
  expression sameregion arg1;
};

/* Basic type for all conditional statements */
struct AST_conditional_stmt { /* extends statement */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  statement parent_loop;
  node break_dest;
  node continue_dest;
  expression sameregion condition;
  statement sameregion stmt;
};

/* SWITCH (CONDITION) STMT.
NEXT_LABEL points to the switches first label */
struct AST_switch_stmt { /* extends conditional_stmt */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  statement parent_loop;
  node break_dest;
  node continue_dest;
  expression sameregion condition;
  statement sameregion stmt;
  label next_label;
};

/* FOR (ARG1; ARG2; ARG3) STMT. ARG1, ARG2, ARG3 are optional */
struct AST_for_stmt { /* extends statement */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  statement parent_loop;
  node break_dest;
  node continue_dest;
  expression sameregion arg1;
  expression sameregion arg2;
  expression sameregion arg3;
  statement sameregion stmt;
};

/* BREAK; */
struct AST_break_stmt { /* extends statement */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  statement parent_loop;
  node break_dest;
  node continue_dest;
};

/* CONTINUE; */
struct AST_continue_stmt { /* extends statement */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  statement parent_loop;
  node break_dest;
  node continue_dest;
};

/* RETURN ARG1. ARG1 is optional */
struct AST_return_stmt { /* extends statement */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  statement parent_loop;
  node break_dest;
  node continue_dest;
  expression sameregion arg1;
};

/* GOTO ID_LABEL */
struct AST_goto_stmt { /* extends statement */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  statement parent_loop;
  node break_dest;
  node continue_dest;
  id_label sameregion id_label;
};

/* GOTO *ARG1 (GCC) */
struct AST_computed_goto_stmt { /* extends statement */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  statement parent_loop;
  node break_dest;
  node continue_dest;
  expression sameregion arg1;
};

/* ; */
struct AST_empty_stmt { /* extends statement */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  statement parent_loop;
  node break_dest;
  node continue_dest;
};

/* Unary expression nodes, kind defines operator */
struct AST_unary { /* extends expression */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  bool lvalue;
  bool side_effects;
  known_cst cst;
  bool bitfield;
  bool isregister;
  known_cst static_address;
  gplace lregion;
  gplace rregion;
  expression sameregion arg1;
};

/* Binary expression nodes, kind defines operator */
struct AST_binary { /* extends expression */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  bool lvalue;
  bool side_effects;
  known_cst cst;
  bool bitfield;
  bool isregister;
  known_cst static_address;
  gplace lregion;
  gplace rregion;
  expression sameregion arg1;
  expression sameregion arg2;
};

/* A comma separated list of expressions ARG1 */
struct AST_comma { /* extends expression */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  bool lvalue;
  bool side_effects;
  known_cst cst;
  bool bitfield;
  bool isregister;
  known_cst static_address;
  gplace lregion;
  gplace rregion;
  expression sameregion arg1;
};

/* sizeof (ASTTYPE) */
struct AST_sizeof_type { /* extends expression */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  bool lvalue;
  bool side_effects;
  known_cst cst;
  bool bitfield;
  bool isregister;
  known_cst static_address;
  gplace lregion;
  gplace rregion;
  asttype sameregion asttype;
};

/* __alignof (ASTTYPE) (GCC) */
struct AST_alignof_type { /* extends expression */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  bool lvalue;
  bool side_effects;
  known_cst cst;
  bool bitfield;
  bool isregister;
  known_cst static_address;
  gplace lregion;
  gplace rregion;
  asttype sameregion asttype;
};

/* &&ID_LABEL (GCC) */
struct AST_label_address { /* extends expression */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  bool lvalue;
  bool side_effects;
  known_cst cst;
  bool bitfield;
  bool isregister;
  known_cst static_address;
  gplace lregion;
  gplace rregion;
  id_label sameregion id_label;
};

/* (ASTTYPE)ARG1 */
struct AST_cast { /* extends unary */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  bool lvalue;
  bool side_effects;
  known_cst cst;
  bool bitfield;
  bool isregister;
  known_cst static_address;
  gplace lregion;
  gplace rregion;
  expression sameregion arg1;
  asttype sameregion asttype;
};

/* (ASTTYPE){INIT_EXPR} (GCC) */
struct AST_cast_list { /* extends expression */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  bool lvalue;
  bool side_effects;
  known_cst cst;
  bool bitfield;
  bool isregister;
  known_cst static_address;
  gplace lregion;
  gplace rregion;
  asttype sameregion asttype;
  expression sameregion init_expr;
};

/* CONDITION ? ARG1 : ARG2
   GCC specific: ARG1 is optional */
struct AST_conditional { /* extends expression */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  bool lvalue;
  bool side_effects;
  known_cst cst;
  bool bitfield;
  bool isregister;
  known_cst static_address;
  gplace lregion;
  gplace rregion;
  expression sameregion condition;
  expression sameregion arg1;
  expression sameregion arg2;
};

/* The identrifier CSTRING.
DEAD_USE is true for identifiers used as the target of a regular assignment */
struct AST_identifier { /* extends expression */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  bool lvalue;
  bool side_effects;
  known_cst cst;
  bool bitfield;
  bool isregister;
  known_cst static_address;
  gplace lregion;
  gplace rregion;
  cstring cstring;
  data_declaration ddecl;
};

/* ({stmt}) (GCC) */
struct AST_compound_expr { /* extends expression */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  bool lvalue;
  bool side_effects;
  known_cst cst;
  bool bitfield;
  bool isregister;
  known_cst static_address;
  gplace lregion;
  gplace rregion;
  statement sameregion stmt;
};

/* ARG1(ARGS). ARGS is a list of expressions
If VA_ARG_CALL is non-null, this is actually a call to the pseudo-function
__builtin_va_arg(args, va_arg_call) (where va_arg_call is a type). In 
this case arg1 is a dummy identifier. */
struct AST_function_call { /* extends expression */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  bool lvalue;
  bool side_effects;
  known_cst cst;
  bool bitfield;
  bool isregister;
  known_cst static_address;
  gplace lregion;
  gplace rregion;
  expression sameregion arg1;
  expression sameregion args;
  asttype va_arg_call;
};

/* ARG1.CSTRING */
struct AST_field_ref { /* extends unary */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  bool lvalue;
  bool side_effects;
  known_cst cst;
  bool bitfield;
  bool isregister;
  known_cst static_address;
  gplace lregion;
  gplace rregion;
  expression sameregion arg1;
  cstring cstring;
};

/* ARG1 = ARG2.
CLEARING is true for assignments used to pre-set local pointers to NULL */
struct AST_assign { /* extends binary */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  bool lvalue;
  bool side_effects;
  known_cst cst;
  bool bitfield;
  bool isregister;
  known_cst static_address;
  gplace lregion;
  gplace rregion;
  expression sameregion arg1;
  expression sameregion arg2;
  bool clearing;
  generic_rc rcop;
};

/* { ARGS }. ARGS is a list of expressions */
struct AST_init_list { /* extends expression */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  bool lvalue;
  bool side_effects;
  known_cst cst;
  bool bitfield;
  bool isregister;
  known_cst static_address;
  gplace lregion;
  gplace rregion;
  expression sameregion args;
};

/* [ ARG1 ... ARG2] INIT_EXPR (GCC). ARG2 is optional */
struct AST_init_index { /* extends expression */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  bool lvalue;
  bool side_effects;
  known_cst cst;
  bool bitfield;
  bool isregister;
  known_cst static_address;
  gplace lregion;
  gplace rregion;
  expression sameregion arg1;
  expression sameregion arg2;
  expression sameregion init_expr;
};

/* WORD1 : INIT_EXPR (GCC) */
struct AST_init_field { /* extends expression */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  bool lvalue;
  bool side_effects;
  known_cst cst;
  bool bitfield;
  bool isregister;
  known_cst static_address;
  gplace lregion;
  gplace rregion;
  word sameregion word1;
  expression sameregion init_expr;
};

/* A constant representable in CVAL. These do not appear as part of the
AST (they show up in the cst and static_address fields of expressions). */
struct AST_known_cst { /* extends expression */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  bool lvalue;
  bool side_effects;
  known_cst cst;
  bool bitfield;
  bool isregister;
  known_cst static_address;
  gplace lregion;
  gplace rregion;
  cval cval;
};

/* A constant represented as in its unparsed lexical form CSTRING. These
appear in the AST. */
struct AST_lexical_cst { /* extends expression */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  bool lvalue;
  bool side_effects;
  known_cst cst;
  bool bitfield;
  bool isregister;
  known_cst static_address;
  gplace lregion;
  gplace rregion;
  cstring cstring;
};

/* A single lexical string whose LENGTH characters are CHARS. The source
form of the constant can be found in CSTRING. */
struct AST_string_cst { /* extends lexical_cst */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  bool lvalue;
  bool side_effects;
  known_cst cst;
  bool bitfield;
  bool isregister;
  known_cst static_address;
  gplace lregion;
  gplace rregion;
  cstring cstring;
  wchar_t * chars;
  size_t length;
};

/* A list of STRINGS forming a single string constant.
DDECL is the magic_string declaration for this string. */
struct AST_string { /* extends expression */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  bool lvalue;
  bool side_effects;
  known_cst cst;
  bool bitfield;
  bool isregister;
  known_cst static_address;
  gplace lregion;
  gplace rregion;
  expression sameregion strings;
  data_declaration ddecl;
};

/* The label CSTRING: */
struct AST_id_label { /* extends label */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  label next_label;
  cstring cstring;
  label_declaration ldecl;
};

/* CASE ARG1 ... ARG2: ARG2 is optional, it's presence is GCC-specific */
struct AST_case_label { /* extends label */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  label next_label;
  expression sameregion arg1;
  expression sameregion arg2;
};

/* DEFAULT: */
struct AST_default_label { /* extends label */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  label next_label;
};

/* The identifier CSTRING used uninterpreted */
struct AST_word { /* extends node */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  cstring cstring;
};

/* STRING(EXPR) (GCC) */
struct AST_asm_operand { /* extends node */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  string sameregion string;
  expression sameregion arg1;
};

/* Adjust the rc of value of pointers of ARG1 (increment if INCREMENTRC true,
decrement otherwise)
RCFN_DECL is the declaration of any necessary rc_adjust function */
struct AST_adjust_rc { /* extends expression */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  bool lvalue;
  bool side_effects;
  known_cst cst;
  bool bitfield;
  bool isregister;
  known_cst static_address;
  gplace lregion;
  gplace rregion;
  expression sameregion arg1;
  bool incrementrc;
  data_declaration rcfn_decl;
};

/* Update rc for 'ARG1 = ARG2'
RCFN_DECL is the declaration of any necessary rc_adjust/rc_update function */
struct AST_generic_rc { /* extends binary */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  bool lvalue;
  bool side_effects;
  known_cst cst;
  bool bitfield;
  bool isregister;
  known_cst static_address;
  gplace lregion;
  gplace rregion;
  expression sameregion arg1;
  expression sameregion arg2;
  data_declaration temps[MAXRCTEMPS];
  data_declaration rcfn_decl;
};

/* Update rc for 'ARG1 = ARG2', where ARG1 is accessed (possibly as an offset)
off pointer variable DDECL
RCFN_DECL is the declaration of any necessary rc_update function */
struct AST_update_rc { /* extends generic_rc */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  bool lvalue;
  bool side_effects;
  known_cst cst;
  bool bitfield;
  bool isregister;
  known_cst static_address;
  gplace lregion;
  gplace rregion;
  expression sameregion arg1;
  expression sameregion arg2;
  data_declaration temps[MAXRCTEMPS];
  data_declaration rcfn_decl;
  data_declaration ddecl;
};

/* A placeholder representing an automatically generated rc_adjust_x
function for type TYPE */
struct AST_adjust_function_decl { /* extends declaration */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  data_declaration rcfn_decl;
};

/* A placeholder representing an automatically generated __rc_adjustarray
function */
struct AST_adjustarray_function_decl { /* extends declaration */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  data_declaration rcfn_decl;
};

/* A placeholder representing an automatically generated rc_update_x
function for type TYPE */
struct AST_update_function_decl { /* extends declaration */
  ast_kind kind;
  location location;
  type type;
  node sameregion next;
  edge edges_out;
  edge edges_in;
  node vertex;
  node vertex_next;
  long mark;
  bitset live_in;
  bitset live_out;
  node sameregion parent;
  node sameregion *sameregion parent_ptr;
  bitset inscope_vars;
  int mf_height;
  int mf_excess;
  bool mf_marked;
  unsigned long in_postorder;
  gnode sr_vertex;
  noderef * defs_out;
  data_declaration rcfn_decl;
};

node node_chain(node l1, node l2);
#define scan_node(var, list) for (var = (list); var; var = CAST(node, var->next))
declaration declaration_chain(declaration l1, declaration l2);
#define scan_declaration(var, list) for (var = (list); var; var = CAST(declaration, var->next))
statement statement_chain(statement l1, statement l2);
#define scan_statement(var, list) for (var = (list); var; var = CAST(statement, var->next))
expression expression_chain(expression l1, expression l2);
#define scan_expression(var, list) for (var = (list); var; var = CAST(expression, var->next))
type_element type_element_chain(type_element l1, type_element l2);
#define scan_type_element(var, list) for (var = (list); var; var = CAST(type_element, var->next))
declarator declarator_chain(declarator l1, declarator l2);
#define scan_declarator(var, list) for (var = (list); var; var = CAST(declarator, var->next))
label label_chain(label l1, label l2);
#define scan_label(var, list) for (var = (list); var; var = CAST(label, var->next))
asm_decl asm_decl_chain(asm_decl l1, asm_decl l2);
#define scan_asm_decl(var, list) for (var = (list); var; var = CAST(asm_decl, var->next))
data_decl data_decl_chain(data_decl l1, data_decl l2);
#define scan_data_decl(var, list) for (var = (list); var; var = CAST(data_decl, var->next))
extension_decl extension_decl_chain(extension_decl l1, extension_decl l2);
#define scan_extension_decl(var, list) for (var = (list); var; var = CAST(extension_decl, var->next))
ellipsis_decl ellipsis_decl_chain(ellipsis_decl l1, ellipsis_decl l2);
#define scan_ellipsis_decl(var, list) for (var = (list); var; var = CAST(ellipsis_decl, var->next))
enumerator enumerator_chain(enumerator l1, enumerator l2);
#define scan_enumerator(var, list) for (var = (list); var; var = CAST(enumerator, var->next))
oldidentifier_decl oldidentifier_decl_chain(oldidentifier_decl l1, oldidentifier_decl l2);
#define scan_oldidentifier_decl(var, list) for (var = (list); var; var = CAST(oldidentifier_decl, var->next))
function_decl function_decl_chain(function_decl l1, function_decl l2);
#define scan_function_decl(var, list) for (var = (list); var; var = CAST(function_decl, var->next))
implicit_decl implicit_decl_chain(implicit_decl l1, implicit_decl l2);
#define scan_implicit_decl(var, list) for (var = (list); var; var = CAST(implicit_decl, var->next))
variable_decl variable_decl_chain(variable_decl l1, variable_decl l2);
#define scan_variable_decl(var, list) for (var = (list); var; var = CAST(variable_decl, var->next))
field_decl field_decl_chain(field_decl l1, field_decl l2);
#define scan_field_decl(var, list) for (var = (list); var; var = CAST(field_decl, var->next))
asttype asttype_chain(asttype l1, asttype l2);
#define scan_asttype(var, list) for (var = (list); var; var = CAST(asttype, var->next))
typename typename_chain(typename l1, typename l2);
#define scan_typename(var, list) for (var = (list); var; var = CAST(typename, var->next))
typeof_expr typeof_expr_chain(typeof_expr l1, typeof_expr l2);
#define scan_typeof_expr(var, list) for (var = (list); var; var = CAST(typeof_expr, var->next))
typeof_type typeof_type_chain(typeof_type l1, typeof_type l2);
#define scan_typeof_type(var, list) for (var = (list); var; var = CAST(typeof_type, var->next))
attribute attribute_chain(attribute l1, attribute l2);
#define scan_attribute(var, list) for (var = (list); var; var = CAST(attribute, var->next))
rid rid_chain(rid l1, rid l2);
#define scan_rid(var, list) for (var = (list); var; var = CAST(rid, var->next))
qualifier qualifier_chain(qualifier l1, qualifier l2);
#define scan_qualifier(var, list) for (var = (list); var; var = CAST(qualifier, var->next))
tag_ref tag_ref_chain(tag_ref l1, tag_ref l2);
#define scan_tag_ref(var, list) for (var = (list); var; var = CAST(tag_ref, var->next))
function_declarator function_declarator_chain(function_declarator l1, function_declarator l2);
#define scan_function_declarator(var, list) for (var = (list); var; var = CAST(function_declarator, var->next))
pointer_declarator pointer_declarator_chain(pointer_declarator l1, pointer_declarator l2);
#define scan_pointer_declarator(var, list) for (var = (list); var; var = CAST(pointer_declarator, var->next))
array_declarator array_declarator_chain(array_declarator l1, array_declarator l2);
#define scan_array_declarator(var, list) for (var = (list); var; var = CAST(array_declarator, var->next))
identifier_declarator identifier_declarator_chain(identifier_declarator l1, identifier_declarator l2);
#define scan_identifier_declarator(var, list) for (var = (list); var; var = CAST(identifier_declarator, var->next))
asm_stmt asm_stmt_chain(asm_stmt l1, asm_stmt l2);
#define scan_asm_stmt(var, list) for (var = (list); var; var = CAST(asm_stmt, var->next))
compound_stmt compound_stmt_chain(compound_stmt l1, compound_stmt l2);
#define scan_compound_stmt(var, list) for (var = (list); var; var = CAST(compound_stmt, var->next))
if_stmt if_stmt_chain(if_stmt l1, if_stmt l2);
#define scan_if_stmt(var, list) for (var = (list); var; var = CAST(if_stmt, var->next))
labeled_stmt labeled_stmt_chain(labeled_stmt l1, labeled_stmt l2);
#define scan_labeled_stmt(var, list) for (var = (list); var; var = CAST(labeled_stmt, var->next))
expression_stmt expression_stmt_chain(expression_stmt l1, expression_stmt l2);
#define scan_expression_stmt(var, list) for (var = (list); var; var = CAST(expression_stmt, var->next))
conditional_stmt conditional_stmt_chain(conditional_stmt l1, conditional_stmt l2);
#define scan_conditional_stmt(var, list) for (var = (list); var; var = CAST(conditional_stmt, var->next))
switch_stmt switch_stmt_chain(switch_stmt l1, switch_stmt l2);
#define scan_switch_stmt(var, list) for (var = (list); var; var = CAST(switch_stmt, var->next))
for_stmt for_stmt_chain(for_stmt l1, for_stmt l2);
#define scan_for_stmt(var, list) for (var = (list); var; var = CAST(for_stmt, var->next))
break_stmt break_stmt_chain(break_stmt l1, break_stmt l2);
#define scan_break_stmt(var, list) for (var = (list); var; var = CAST(break_stmt, var->next))
continue_stmt continue_stmt_chain(continue_stmt l1, continue_stmt l2);
#define scan_continue_stmt(var, list) for (var = (list); var; var = CAST(continue_stmt, var->next))
return_stmt return_stmt_chain(return_stmt l1, return_stmt l2);
#define scan_return_stmt(var, list) for (var = (list); var; var = CAST(return_stmt, var->next))
goto_stmt goto_stmt_chain(goto_stmt l1, goto_stmt l2);
#define scan_goto_stmt(var, list) for (var = (list); var; var = CAST(goto_stmt, var->next))
computed_goto_stmt computed_goto_stmt_chain(computed_goto_stmt l1, computed_goto_stmt l2);
#define scan_computed_goto_stmt(var, list) for (var = (list); var; var = CAST(computed_goto_stmt, var->next))
empty_stmt empty_stmt_chain(empty_stmt l1, empty_stmt l2);
#define scan_empty_stmt(var, list) for (var = (list); var; var = CAST(empty_stmt, var->next))
unary unary_chain(unary l1, unary l2);
#define scan_unary(var, list) for (var = (list); var; var = CAST(unary, var->next))
binary binary_chain(binary l1, binary l2);
#define scan_binary(var, list) for (var = (list); var; var = CAST(binary, var->next))
comma comma_chain(comma l1, comma l2);
#define scan_comma(var, list) for (var = (list); var; var = CAST(comma, var->next))
sizeof_type sizeof_type_chain(sizeof_type l1, sizeof_type l2);
#define scan_sizeof_type(var, list) for (var = (list); var; var = CAST(sizeof_type, var->next))
alignof_type alignof_type_chain(alignof_type l1, alignof_type l2);
#define scan_alignof_type(var, list) for (var = (list); var; var = CAST(alignof_type, var->next))
label_address label_address_chain(label_address l1, label_address l2);
#define scan_label_address(var, list) for (var = (list); var; var = CAST(label_address, var->next))
cast cast_chain(cast l1, cast l2);
#define scan_cast(var, list) for (var = (list); var; var = CAST(cast, var->next))
cast_list cast_list_chain(cast_list l1, cast_list l2);
#define scan_cast_list(var, list) for (var = (list); var; var = CAST(cast_list, var->next))
conditional conditional_chain(conditional l1, conditional l2);
#define scan_conditional(var, list) for (var = (list); var; var = CAST(conditional, var->next))
identifier identifier_chain(identifier l1, identifier l2);
#define scan_identifier(var, list) for (var = (list); var; var = CAST(identifier, var->next))
compound_expr compound_expr_chain(compound_expr l1, compound_expr l2);
#define scan_compound_expr(var, list) for (var = (list); var; var = CAST(compound_expr, var->next))
function_call function_call_chain(function_call l1, function_call l2);
#define scan_function_call(var, list) for (var = (list); var; var = CAST(function_call, var->next))
field_ref field_ref_chain(field_ref l1, field_ref l2);
#define scan_field_ref(var, list) for (var = (list); var; var = CAST(field_ref, var->next))
assign assign_chain(assign l1, assign l2);
#define scan_assign(var, list) for (var = (list); var; var = CAST(assign, var->next))
init_list init_list_chain(init_list l1, init_list l2);
#define scan_init_list(var, list) for (var = (list); var; var = CAST(init_list, var->next))
init_index init_index_chain(init_index l1, init_index l2);
#define scan_init_index(var, list) for (var = (list); var; var = CAST(init_index, var->next))
init_field init_field_chain(init_field l1, init_field l2);
#define scan_init_field(var, list) for (var = (list); var; var = CAST(init_field, var->next))
known_cst known_cst_chain(known_cst l1, known_cst l2);
#define scan_known_cst(var, list) for (var = (list); var; var = CAST(known_cst, var->next))
lexical_cst lexical_cst_chain(lexical_cst l1, lexical_cst l2);
#define scan_lexical_cst(var, list) for (var = (list); var; var = CAST(lexical_cst, var->next))
string_cst string_cst_chain(string_cst l1, string_cst l2);
#define scan_string_cst(var, list) for (var = (list); var; var = CAST(string_cst, var->next))
string string_chain(string l1, string l2);
#define scan_string(var, list) for (var = (list); var; var = CAST(string, var->next))
id_label id_label_chain(id_label l1, id_label l2);
#define scan_id_label(var, list) for (var = (list); var; var = CAST(id_label, var->next))
case_label case_label_chain(case_label l1, case_label l2);
#define scan_case_label(var, list) for (var = (list); var; var = CAST(case_label, var->next))
default_label default_label_chain(default_label l1, default_label l2);
#define scan_default_label(var, list) for (var = (list); var; var = CAST(default_label, var->next))
word word_chain(word l1, word l2);
#define scan_word(var, list) for (var = (list); var; var = CAST(word, var->next))
asm_operand asm_operand_chain(asm_operand l1, asm_operand l2);
#define scan_asm_operand(var, list) for (var = (list); var; var = CAST(asm_operand, var->next))
adjust_rc adjust_rc_chain(adjust_rc l1, adjust_rc l2);
#define scan_adjust_rc(var, list) for (var = (list); var; var = CAST(adjust_rc, var->next))
generic_rc generic_rc_chain(generic_rc l1, generic_rc l2);
#define scan_generic_rc(var, list) for (var = (list); var; var = CAST(generic_rc, var->next))
update_rc update_rc_chain(update_rc l1, update_rc l2);
#define scan_update_rc(var, list) for (var = (list); var; var = CAST(update_rc, var->next))
adjust_function_decl adjust_function_decl_chain(adjust_function_decl l1, adjust_function_decl l2);
#define scan_adjust_function_decl(var, list) for (var = (list); var; var = CAST(adjust_function_decl, var->next))
adjustarray_function_decl adjustarray_function_decl_chain(adjustarray_function_decl l1, adjustarray_function_decl l2);
#define scan_adjustarray_function_decl(var, list) for (var = (list); var; var = CAST(adjustarray_function_decl, var->next))
update_function_decl update_function_decl_chain(update_function_decl l1, update_function_decl l2);
#define scan_update_function_decl(var, list) for (var = (list); var; var = CAST(update_function_decl, var->next))
error_decl error_decl_chain(error_decl l1, error_decl l2);
#define scan_error_decl(var, list) for (var = (list); var; var = CAST(error_decl, var->next))
struct_ref struct_ref_chain(struct_ref l1, struct_ref l2);
#define scan_struct_ref(var, list) for (var = (list); var; var = CAST(struct_ref, var->next))
union_ref union_ref_chain(union_ref l1, union_ref l2);
#define scan_union_ref(var, list) for (var = (list); var; var = CAST(union_ref, var->next))
enum_ref enum_ref_chain(enum_ref l1, enum_ref l2);
#define scan_enum_ref(var, list) for (var = (list); var; var = CAST(enum_ref, var->next))
error_stmt error_stmt_chain(error_stmt l1, error_stmt l2);
#define scan_error_stmt(var, list) for (var = (list); var; var = CAST(error_stmt, var->next))
while_stmt while_stmt_chain(while_stmt l1, while_stmt l2);
#define scan_while_stmt(var, list) for (var = (list); var; var = CAST(while_stmt, var->next))
dowhile_stmt dowhile_stmt_chain(dowhile_stmt l1, dowhile_stmt l2);
#define scan_dowhile_stmt(var, list) for (var = (list); var; var = CAST(dowhile_stmt, var->next))
error_expr error_expr_chain(error_expr l1, error_expr l2);
#define scan_error_expr(var, list) for (var = (list); var; var = CAST(error_expr, var->next))
array_ref array_ref_chain(array_ref l1, array_ref l2);
#define scan_array_ref(var, list) for (var = (list); var; var = CAST(array_ref, var->next))
dereference dereference_chain(dereference l1, dereference l2);
#define scan_dereference(var, list) for (var = (list); var; var = CAST(dereference, var->next))
extension_expr extension_expr_chain(extension_expr l1, extension_expr l2);
#define scan_extension_expr(var, list) for (var = (list); var; var = CAST(extension_expr, var->next))
sizeof_expr sizeof_expr_chain(sizeof_expr l1, sizeof_expr l2);
#define scan_sizeof_expr(var, list) for (var = (list); var; var = CAST(sizeof_expr, var->next))
alignof_expr alignof_expr_chain(alignof_expr l1, alignof_expr l2);
#define scan_alignof_expr(var, list) for (var = (list); var; var = CAST(alignof_expr, var->next))
realpart realpart_chain(realpart l1, realpart l2);
#define scan_realpart(var, list) for (var = (list); var; var = CAST(realpart, var->next))
imagpart imagpart_chain(imagpart l1, imagpart l2);
#define scan_imagpart(var, list) for (var = (list); var; var = CAST(imagpart, var->next))
address_of address_of_chain(address_of l1, address_of l2);
#define scan_address_of(var, list) for (var = (list); var; var = CAST(address_of, var->next))
unary_minus unary_minus_chain(unary_minus l1, unary_minus l2);
#define scan_unary_minus(var, list) for (var = (list); var; var = CAST(unary_minus, var->next))
unary_plus unary_plus_chain(unary_plus l1, unary_plus l2);
#define scan_unary_plus(var, list) for (var = (list); var; var = CAST(unary_plus, var->next))
conjugate conjugate_chain(conjugate l1, conjugate l2);
#define scan_conjugate(var, list) for (var = (list); var; var = CAST(conjugate, var->next))
preincrement preincrement_chain(preincrement l1, preincrement l2);
#define scan_preincrement(var, list) for (var = (list); var; var = CAST(preincrement, var->next))
predecrement predecrement_chain(predecrement l1, predecrement l2);
#define scan_predecrement(var, list) for (var = (list); var; var = CAST(predecrement, var->next))
postincrement postincrement_chain(postincrement l1, postincrement l2);
#define scan_postincrement(var, list) for (var = (list); var; var = CAST(postincrement, var->next))
postdecrement postdecrement_chain(postdecrement l1, postdecrement l2);
#define scan_postdecrement(var, list) for (var = (list); var; var = CAST(postdecrement, var->next))
bitnot bitnot_chain(bitnot l1, bitnot l2);
#define scan_bitnot(var, list) for (var = (list); var; var = CAST(bitnot, var->next))
not not_chain(not l1, not l2);
#define scan_not(var, list) for (var = (list); var; var = CAST(not, var->next))
plus plus_chain(plus l1, plus l2);
#define scan_plus(var, list) for (var = (list); var; var = CAST(plus, var->next))
minus minus_chain(minus l1, minus l2);
#define scan_minus(var, list) for (var = (list); var; var = CAST(minus, var->next))
times times_chain(times l1, times l2);
#define scan_times(var, list) for (var = (list); var; var = CAST(times, var->next))
divide divide_chain(divide l1, divide l2);
#define scan_divide(var, list) for (var = (list); var; var = CAST(divide, var->next))
modulo modulo_chain(modulo l1, modulo l2);
#define scan_modulo(var, list) for (var = (list); var; var = CAST(modulo, var->next))
lshift lshift_chain(lshift l1, lshift l2);
#define scan_lshift(var, list) for (var = (list); var; var = CAST(lshift, var->next))
rshift rshift_chain(rshift l1, rshift l2);
#define scan_rshift(var, list) for (var = (list); var; var = CAST(rshift, var->next))
leq leq_chain(leq l1, leq l2);
#define scan_leq(var, list) for (var = (list); var; var = CAST(leq, var->next))
geq geq_chain(geq l1, geq l2);
#define scan_geq(var, list) for (var = (list); var; var = CAST(geq, var->next))
lt lt_chain(lt l1, lt l2);
#define scan_lt(var, list) for (var = (list); var; var = CAST(lt, var->next))
gt gt_chain(gt l1, gt l2);
#define scan_gt(var, list) for (var = (list); var; var = CAST(gt, var->next))
eq eq_chain(eq l1, eq l2);
#define scan_eq(var, list) for (var = (list); var; var = CAST(eq, var->next))
ne ne_chain(ne l1, ne l2);
#define scan_ne(var, list) for (var = (list); var; var = CAST(ne, var->next))
bitand bitand_chain(bitand l1, bitand l2);
#define scan_bitand(var, list) for (var = (list); var; var = CAST(bitand, var->next))
bitor bitor_chain(bitor l1, bitor l2);
#define scan_bitor(var, list) for (var = (list); var; var = CAST(bitor, var->next))
bitxor bitxor_chain(bitxor l1, bitxor l2);
#define scan_bitxor(var, list) for (var = (list); var; var = CAST(bitxor, var->next))
andand andand_chain(andand l1, andand l2);
#define scan_andand(var, list) for (var = (list); var; var = CAST(andand, var->next))
oror oror_chain(oror l1, oror l2);
#define scan_oror(var, list) for (var = (list); var; var = CAST(oror, var->next))
plus_assign plus_assign_chain(plus_assign l1, plus_assign l2);
#define scan_plus_assign(var, list) for (var = (list); var; var = CAST(plus_assign, var->next))
minus_assign minus_assign_chain(minus_assign l1, minus_assign l2);
#define scan_minus_assign(var, list) for (var = (list); var; var = CAST(minus_assign, var->next))
times_assign times_assign_chain(times_assign l1, times_assign l2);
#define scan_times_assign(var, list) for (var = (list); var; var = CAST(times_assign, var->next))
divide_assign divide_assign_chain(divide_assign l1, divide_assign l2);
#define scan_divide_assign(var, list) for (var = (list); var; var = CAST(divide_assign, var->next))
modulo_assign modulo_assign_chain(modulo_assign l1, modulo_assign l2);
#define scan_modulo_assign(var, list) for (var = (list); var; var = CAST(modulo_assign, var->next))
lshift_assign lshift_assign_chain(lshift_assign l1, lshift_assign l2);
#define scan_lshift_assign(var, list) for (var = (list); var; var = CAST(lshift_assign, var->next))
rshift_assign rshift_assign_chain(rshift_assign l1, rshift_assign l2);
#define scan_rshift_assign(var, list) for (var = (list); var; var = CAST(rshift_assign, var->next))
bitand_assign bitand_assign_chain(bitand_assign l1, bitand_assign l2);
#define scan_bitand_assign(var, list) for (var = (list); var; var = CAST(bitand_assign, var->next))
bitor_assign bitor_assign_chain(bitor_assign l1, bitor_assign l2);
#define scan_bitor_assign(var, list) for (var = (list); var; var = CAST(bitor_assign, var->next))
bitxor_assign bitxor_assign_chain(bitxor_assign l1, bitxor_assign l2);
#define scan_bitxor_assign(var, list) for (var = (list); var; var = CAST(bitxor_assign, var->next))
global_rc global_rc_chain(global_rc l1, global_rc l2);
#define scan_global_rc(var, list) for (var = (list); var; var = CAST(global_rc, var->next))
