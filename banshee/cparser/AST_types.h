/* Automatically generated file, do not edit. See nodetypes.def */

/* Copyright (c) 2000-200 The Regents of the University of California. */
/* See the copyright notice in nodetypes.def for distribution restrictions */
typedef struct AST_node *node;
typedef struct AST_declaration *declaration;
typedef struct AST_statement *statement;
typedef struct AST_expression *expression;
typedef struct AST_type_element *type_element;
typedef struct AST_declarator *declarator;
typedef struct AST_label *label;
typedef struct AST_asm_decl *asm_decl;
typedef struct AST_data_decl *data_decl;
typedef struct AST_extension_decl *extension_decl;
typedef struct AST_ellipsis_decl *ellipsis_decl;
typedef struct AST_enumerator *enumerator;
typedef struct AST_oldidentifier_decl *oldidentifier_decl;
typedef struct AST_function_decl *function_decl;
typedef struct AST_implicit_decl *implicit_decl;
typedef struct AST_variable_decl *variable_decl;
typedef struct AST_field_decl *field_decl;
typedef struct AST_asttype *asttype;
typedef struct AST_typename *typename;
typedef struct AST_typeof_expr *typeof_expr;
typedef struct AST_typeof_type *typeof_type;
typedef struct AST_attribute *attribute;
typedef struct AST_rid *rid;
typedef struct AST_qualifier *qualifier;
typedef struct AST_tag_ref *tag_ref;
typedef struct AST_function_declarator *function_declarator;
typedef struct AST_pointer_declarator *pointer_declarator;
typedef struct AST_array_declarator *array_declarator;
typedef struct AST_identifier_declarator *identifier_declarator;
typedef struct AST_asm_stmt *asm_stmt;
typedef struct AST_compound_stmt *compound_stmt;
typedef struct AST_if_stmt *if_stmt;
typedef struct AST_labeled_stmt *labeled_stmt;
typedef struct AST_expression_stmt *expression_stmt;
typedef struct AST_conditional_stmt *conditional_stmt;
typedef struct AST_switch_stmt *switch_stmt;
typedef struct AST_for_stmt *for_stmt;
typedef struct AST_break_stmt *break_stmt;
typedef struct AST_continue_stmt *continue_stmt;
typedef struct AST_return_stmt *return_stmt;
typedef struct AST_goto_stmt *goto_stmt;
typedef struct AST_computed_goto_stmt *computed_goto_stmt;
typedef struct AST_empty_stmt *empty_stmt;
typedef struct AST_unary *unary;
typedef struct AST_binary *binary;
typedef struct AST_comma *comma;
typedef struct AST_sizeof_type *sizeof_type;
typedef struct AST_alignof_type *alignof_type;
typedef struct AST_label_address *label_address;
typedef struct AST_cast *cast;
typedef struct AST_cast_list *cast_list;
typedef struct AST_conditional *conditional;
typedef struct AST_identifier *identifier;
typedef struct AST_compound_expr *compound_expr;
typedef struct AST_function_call *function_call;
typedef struct AST_field_ref *field_ref;
typedef struct AST_assign *assign;
typedef struct AST_init_list *init_list;
typedef struct AST_init_index *init_index;
typedef struct AST_init_field *init_field;
typedef struct AST_known_cst *known_cst;
typedef struct AST_lexical_cst *lexical_cst;
typedef struct AST_string_cst *string_cst;
typedef struct AST_string *string;
typedef struct AST_id_label *id_label;
typedef struct AST_case_label *case_label;
typedef struct AST_default_label *default_label;
typedef struct AST_word *word;
typedef struct AST_asm_operand *asm_operand;
typedef struct AST_adjust_rc *adjust_rc;
typedef struct AST_generic_rc *generic_rc;
typedef struct AST_update_rc *update_rc;
typedef struct AST_adjust_function_decl *adjust_function_decl;
typedef struct AST_adjustarray_function_decl *adjustarray_function_decl;
typedef struct AST_update_function_decl *update_function_decl;
typedef struct AST_declaration *error_decl;
typedef struct AST_tag_ref *struct_ref;
typedef struct AST_tag_ref *union_ref;
typedef struct AST_tag_ref *enum_ref;
typedef struct AST_statement *error_stmt;
typedef struct AST_conditional_stmt *while_stmt;
typedef struct AST_conditional_stmt *dowhile_stmt;
typedef struct AST_expression *error_expr;
typedef struct AST_binary *array_ref;
typedef struct AST_unary *dereference;
typedef struct AST_unary *extension_expr;
typedef struct AST_unary *sizeof_expr;
typedef struct AST_unary *alignof_expr;
typedef struct AST_unary *realpart;
typedef struct AST_unary *imagpart;
typedef struct AST_unary *address_of;
typedef struct AST_unary *unary_minus;
typedef struct AST_unary *unary_plus;
typedef struct AST_unary *conjugate;
typedef struct AST_unary *preincrement;
typedef struct AST_unary *predecrement;
typedef struct AST_unary *postincrement;
typedef struct AST_unary *postdecrement;
typedef struct AST_unary *bitnot;
typedef struct AST_unary *not;
typedef struct AST_binary *plus;
typedef struct AST_binary *minus;
typedef struct AST_binary *times;
typedef struct AST_binary *divide;
typedef struct AST_binary *modulo;
typedef struct AST_binary *lshift;
typedef struct AST_binary *rshift;
typedef struct AST_binary *leq;
typedef struct AST_binary *geq;
typedef struct AST_binary *lt;
typedef struct AST_binary *gt;
typedef struct AST_binary *eq;
typedef struct AST_binary *ne;
typedef struct AST_binary *bitand;
typedef struct AST_binary *bitor;
typedef struct AST_binary *bitxor;
typedef struct AST_binary *andand;
typedef struct AST_binary *oror;
typedef struct AST_binary *plus_assign;
typedef struct AST_binary *minus_assign;
typedef struct AST_binary *times_assign;
typedef struct AST_binary *divide_assign;
typedef struct AST_binary *modulo_assign;
typedef struct AST_binary *lshift_assign;
typedef struct AST_binary *rshift_assign;
typedef struct AST_binary *bitand_assign;
typedef struct AST_binary *bitor_assign;
typedef struct AST_binary *bitxor_assign;
typedef struct AST_generic_rc *global_rc;
typedef enum {
  kind_node = 42,
  postkind_node = 170,
  kind_declaration = 43,
  postkind_declaration = 57,
  kind_statement = 58,
  postkind_statement = 75,
  kind_expression = 76,
  postkind_expression = 147,
  kind_type_element = 148,
  postkind_type_element = 158,
  kind_declarator = 159,
  postkind_declarator = 163,
  kind_label = 164,
  postkind_label = 167,
  kind_asm_decl = 44,
  postkind_asm_decl = 44,
  kind_data_decl = 45,
  postkind_data_decl = 45,
  kind_extension_decl = 46,
  postkind_extension_decl = 46,
  kind_ellipsis_decl = 47,
  postkind_ellipsis_decl = 47,
  kind_enumerator = 48,
  postkind_enumerator = 48,
  kind_oldidentifier_decl = 49,
  postkind_oldidentifier_decl = 49,
  kind_function_decl = 50,
  postkind_function_decl = 50,
  kind_implicit_decl = 51,
  postkind_implicit_decl = 51,
  kind_variable_decl = 52,
  postkind_variable_decl = 52,
  kind_field_decl = 53,
  postkind_field_decl = 53,
  kind_asttype = 168,
  postkind_asttype = 168,
  kind_typename = 149,
  postkind_typename = 149,
  kind_typeof_expr = 150,
  postkind_typeof_expr = 150,
  kind_typeof_type = 151,
  postkind_typeof_type = 151,
  kind_attribute = 152,
  postkind_attribute = 152,
  kind_rid = 153,
  postkind_rid = 153,
  kind_qualifier = 154,
  postkind_qualifier = 154,
  kind_tag_ref = 155,
  postkind_tag_ref = 158,
  kind_function_declarator = 160,
  postkind_function_declarator = 160,
  kind_pointer_declarator = 161,
  postkind_pointer_declarator = 161,
  kind_array_declarator = 162,
  postkind_array_declarator = 162,
  kind_identifier_declarator = 163,
  postkind_identifier_declarator = 163,
  kind_asm_stmt = 59,
  postkind_asm_stmt = 59,
  kind_compound_stmt = 60,
  postkind_compound_stmt = 60,
  kind_if_stmt = 61,
  postkind_if_stmt = 61,
  kind_labeled_stmt = 62,
  postkind_labeled_stmt = 62,
  kind_expression_stmt = 63,
  postkind_expression_stmt = 63,
  kind_conditional_stmt = 64,
  postkind_conditional_stmt = 67,
  kind_switch_stmt = 65,
  postkind_switch_stmt = 65,
  kind_for_stmt = 68,
  postkind_for_stmt = 68,
  kind_break_stmt = 69,
  postkind_break_stmt = 69,
  kind_continue_stmt = 70,
  postkind_continue_stmt = 70,
  kind_return_stmt = 71,
  postkind_return_stmt = 71,
  kind_goto_stmt = 72,
  postkind_goto_stmt = 72,
  kind_computed_goto_stmt = 73,
  postkind_computed_goto_stmt = 73,
  kind_empty_stmt = 74,
  postkind_empty_stmt = 74,
  kind_unary = 77,
  postkind_unary = 95,
  kind_binary = 96,
  postkind_binary = 129,
  kind_comma = 130,
  postkind_comma = 130,
  kind_sizeof_type = 131,
  postkind_sizeof_type = 131,
  kind_alignof_type = 132,
  postkind_alignof_type = 132,
  kind_label_address = 133,
  postkind_label_address = 133,
  kind_cast = 78,
  postkind_cast = 78,
  kind_cast_list = 134,
  postkind_cast_list = 134,
  kind_conditional = 135,
  postkind_conditional = 135,
  kind_identifier = 136,
  postkind_identifier = 136,
  kind_compound_expr = 137,
  postkind_compound_expr = 137,
  kind_function_call = 138,
  postkind_function_call = 138,
  kind_field_ref = 79,
  postkind_field_ref = 79,
  kind_assign = 97,
  postkind_assign = 97,
  kind_init_list = 139,
  postkind_init_list = 139,
  kind_init_index = 140,
  postkind_init_index = 140,
  kind_init_field = 141,
  postkind_init_field = 141,
  kind_known_cst = 142,
  postkind_known_cst = 142,
  kind_lexical_cst = 143,
  postkind_lexical_cst = 144,
  kind_string_cst = 144,
  postkind_string_cst = 144,
  kind_string = 145,
  postkind_string = 145,
  kind_id_label = 165,
  postkind_id_label = 165,
  kind_case_label = 166,
  postkind_case_label = 166,
  kind_default_label = 167,
  postkind_default_label = 167,
  kind_word = 169,
  postkind_word = 169,
  kind_asm_operand = 170,
  postkind_asm_operand = 170,
  kind_adjust_rc = 146,
  postkind_adjust_rc = 146,
  kind_generic_rc = 98,
  postkind_generic_rc = 100,
  kind_update_rc = 99,
  postkind_update_rc = 99,
  kind_adjust_function_decl = 54,
  postkind_adjust_function_decl = 54,
  kind_adjustarray_function_decl = 55,
  postkind_adjustarray_function_decl = 55,
  kind_update_function_decl = 56,
  postkind_update_function_decl = 56,
  kind_error_decl = 57,
  postkind_error_decl = 57,
  kind_struct_ref = 156,
  postkind_struct_ref = 156,
  kind_union_ref = 157,
  postkind_union_ref = 157,
  kind_enum_ref = 158,
  postkind_enum_ref = 158,
  kind_error_stmt = 75,
  postkind_error_stmt = 75,
  kind_while_stmt = 66,
  postkind_while_stmt = 66,
  kind_dowhile_stmt = 67,
  postkind_dowhile_stmt = 67,
  kind_error_expr = 147,
  postkind_error_expr = 147,
  kind_array_ref = 101,
  postkind_array_ref = 101,
  kind_dereference = 80,
  postkind_dereference = 80,
  kind_extension_expr = 81,
  postkind_extension_expr = 81,
  kind_sizeof_expr = 82,
  postkind_sizeof_expr = 82,
  kind_alignof_expr = 83,
  postkind_alignof_expr = 83,
  kind_realpart = 84,
  postkind_realpart = 84,
  kind_imagpart = 85,
  postkind_imagpart = 85,
  kind_address_of = 86,
  postkind_address_of = 86,
  kind_unary_minus = 87,
  postkind_unary_minus = 87,
  kind_unary_plus = 88,
  postkind_unary_plus = 88,
  kind_conjugate = 89,
  postkind_conjugate = 89,
  kind_preincrement = 90,
  postkind_preincrement = 90,
  kind_predecrement = 91,
  postkind_predecrement = 91,
  kind_postincrement = 92,
  postkind_postincrement = 92,
  kind_postdecrement = 93,
  postkind_postdecrement = 93,
  kind_bitnot = 94,
  postkind_bitnot = 94,
  kind_not = 95,
  postkind_not = 95,
  kind_plus = 102,
  postkind_plus = 102,
  kind_minus = 103,
  postkind_minus = 103,
  kind_times = 104,
  postkind_times = 104,
  kind_divide = 105,
  postkind_divide = 105,
  kind_modulo = 106,
  postkind_modulo = 106,
  kind_lshift = 107,
  postkind_lshift = 107,
  kind_rshift = 108,
  postkind_rshift = 108,
  kind_leq = 109,
  postkind_leq = 109,
  kind_geq = 110,
  postkind_geq = 110,
  kind_lt = 111,
  postkind_lt = 111,
  kind_gt = 112,
  postkind_gt = 112,
  kind_eq = 113,
  postkind_eq = 113,
  kind_ne = 114,
  postkind_ne = 114,
  kind_bitand = 115,
  postkind_bitand = 115,
  kind_bitor = 116,
  postkind_bitor = 116,
  kind_bitxor = 117,
  postkind_bitxor = 117,
  kind_andand = 118,
  postkind_andand = 118,
  kind_oror = 119,
  postkind_oror = 119,
  kind_plus_assign = 120,
  postkind_plus_assign = 120,
  kind_minus_assign = 121,
  postkind_minus_assign = 121,
  kind_times_assign = 122,
  postkind_times_assign = 122,
  kind_divide_assign = 123,
  postkind_divide_assign = 123,
  kind_modulo_assign = 124,
  postkind_modulo_assign = 124,
  kind_lshift_assign = 125,
  postkind_lshift_assign = 125,
  kind_rshift_assign = 126,
  postkind_rshift_assign = 126,
  kind_bitand_assign = 127,
  postkind_bitand_assign = 127,
  kind_bitor_assign = 128,
  postkind_bitor_assign = 128,
  kind_bitxor_assign = 129,
  postkind_bitxor_assign = 129,
  kind_global_rc = 100,
  postkind_global_rc = 100
} ast_kind;
#define is_node(x) ((x)->kind >= kind_node && (x)->kind <= postkind_node)
#define is_declaration(x) ((x)->kind >= kind_declaration && (x)->kind <= postkind_declaration)
#define is_statement(x) ((x)->kind >= kind_statement && (x)->kind <= postkind_statement)
#define is_expression(x) ((x)->kind >= kind_expression && (x)->kind <= postkind_expression)
#define is_type_element(x) ((x)->kind >= kind_type_element && (x)->kind <= postkind_type_element)
#define is_declarator(x) ((x)->kind >= kind_declarator && (x)->kind <= postkind_declarator)
#define is_label(x) ((x)->kind >= kind_label && (x)->kind <= postkind_label)
#define is_asm_decl(x) ((x)->kind >= kind_asm_decl && (x)->kind <= postkind_asm_decl)
#define is_data_decl(x) ((x)->kind >= kind_data_decl && (x)->kind <= postkind_data_decl)
#define is_extension_decl(x) ((x)->kind >= kind_extension_decl && (x)->kind <= postkind_extension_decl)
#define is_ellipsis_decl(x) ((x)->kind >= kind_ellipsis_decl && (x)->kind <= postkind_ellipsis_decl)
#define is_enumerator(x) ((x)->kind >= kind_enumerator && (x)->kind <= postkind_enumerator)
#define is_oldidentifier_decl(x) ((x)->kind >= kind_oldidentifier_decl && (x)->kind <= postkind_oldidentifier_decl)
#define is_function_decl(x) ((x)->kind >= kind_function_decl && (x)->kind <= postkind_function_decl)
#define is_implicit_decl(x) ((x)->kind >= kind_implicit_decl && (x)->kind <= postkind_implicit_decl)
#define is_variable_decl(x) ((x)->kind >= kind_variable_decl && (x)->kind <= postkind_variable_decl)
#define is_field_decl(x) ((x)->kind >= kind_field_decl && (x)->kind <= postkind_field_decl)
#define is_asttype(x) ((x)->kind >= kind_asttype && (x)->kind <= postkind_asttype)
#define is_typename(x) ((x)->kind >= kind_typename && (x)->kind <= postkind_typename)
#define is_typeof_expr(x) ((x)->kind >= kind_typeof_expr && (x)->kind <= postkind_typeof_expr)
#define is_typeof_type(x) ((x)->kind >= kind_typeof_type && (x)->kind <= postkind_typeof_type)
#define is_attribute(x) ((x)->kind >= kind_attribute && (x)->kind <= postkind_attribute)
#define is_rid(x) ((x)->kind >= kind_rid && (x)->kind <= postkind_rid)
#define is_qualifier(x) ((x)->kind >= kind_qualifier && (x)->kind <= postkind_qualifier)
#define is_tag_ref(x) ((x)->kind >= kind_tag_ref && (x)->kind <= postkind_tag_ref)
#define is_function_declarator(x) ((x)->kind >= kind_function_declarator && (x)->kind <= postkind_function_declarator)
#define is_pointer_declarator(x) ((x)->kind >= kind_pointer_declarator && (x)->kind <= postkind_pointer_declarator)
#define is_array_declarator(x) ((x)->kind >= kind_array_declarator && (x)->kind <= postkind_array_declarator)
#define is_identifier_declarator(x) ((x)->kind >= kind_identifier_declarator && (x)->kind <= postkind_identifier_declarator)
#define is_asm_stmt(x) ((x)->kind >= kind_asm_stmt && (x)->kind <= postkind_asm_stmt)
#define is_compound_stmt(x) ((x)->kind >= kind_compound_stmt && (x)->kind <= postkind_compound_stmt)
#define is_if_stmt(x) ((x)->kind >= kind_if_stmt && (x)->kind <= postkind_if_stmt)
#define is_labeled_stmt(x) ((x)->kind >= kind_labeled_stmt && (x)->kind <= postkind_labeled_stmt)
#define is_expression_stmt(x) ((x)->kind >= kind_expression_stmt && (x)->kind <= postkind_expression_stmt)
#define is_conditional_stmt(x) ((x)->kind >= kind_conditional_stmt && (x)->kind <= postkind_conditional_stmt)
#define is_switch_stmt(x) ((x)->kind >= kind_switch_stmt && (x)->kind <= postkind_switch_stmt)
#define is_for_stmt(x) ((x)->kind >= kind_for_stmt && (x)->kind <= postkind_for_stmt)
#define is_break_stmt(x) ((x)->kind >= kind_break_stmt && (x)->kind <= postkind_break_stmt)
#define is_continue_stmt(x) ((x)->kind >= kind_continue_stmt && (x)->kind <= postkind_continue_stmt)
#define is_return_stmt(x) ((x)->kind >= kind_return_stmt && (x)->kind <= postkind_return_stmt)
#define is_goto_stmt(x) ((x)->kind >= kind_goto_stmt && (x)->kind <= postkind_goto_stmt)
#define is_computed_goto_stmt(x) ((x)->kind >= kind_computed_goto_stmt && (x)->kind <= postkind_computed_goto_stmt)
#define is_empty_stmt(x) ((x)->kind >= kind_empty_stmt && (x)->kind <= postkind_empty_stmt)
#define is_unary(x) ((x)->kind >= kind_unary && (x)->kind <= postkind_unary)
#define is_binary(x) ((x)->kind >= kind_binary && (x)->kind <= postkind_binary)
#define is_comma(x) ((x)->kind >= kind_comma && (x)->kind <= postkind_comma)
#define is_sizeof_type(x) ((x)->kind >= kind_sizeof_type && (x)->kind <= postkind_sizeof_type)
#define is_alignof_type(x) ((x)->kind >= kind_alignof_type && (x)->kind <= postkind_alignof_type)
#define is_label_address(x) ((x)->kind >= kind_label_address && (x)->kind <= postkind_label_address)
#define is_cast(x) ((x)->kind >= kind_cast && (x)->kind <= postkind_cast)
#define is_cast_list(x) ((x)->kind >= kind_cast_list && (x)->kind <= postkind_cast_list)
#define is_conditional(x) ((x)->kind >= kind_conditional && (x)->kind <= postkind_conditional)
#define is_identifier(x) ((x)->kind >= kind_identifier && (x)->kind <= postkind_identifier)
#define is_compound_expr(x) ((x)->kind >= kind_compound_expr && (x)->kind <= postkind_compound_expr)
#define is_function_call(x) ((x)->kind >= kind_function_call && (x)->kind <= postkind_function_call)
#define is_field_ref(x) ((x)->kind >= kind_field_ref && (x)->kind <= postkind_field_ref)
#define is_assign(x) ((x)->kind >= kind_assign && (x)->kind <= postkind_assign)
#define is_init_list(x) ((x)->kind >= kind_init_list && (x)->kind <= postkind_init_list)
#define is_init_index(x) ((x)->kind >= kind_init_index && (x)->kind <= postkind_init_index)
#define is_init_field(x) ((x)->kind >= kind_init_field && (x)->kind <= postkind_init_field)
#define is_known_cst(x) ((x)->kind >= kind_known_cst && (x)->kind <= postkind_known_cst)
#define is_lexical_cst(x) ((x)->kind >= kind_lexical_cst && (x)->kind <= postkind_lexical_cst)
#define is_string_cst(x) ((x)->kind >= kind_string_cst && (x)->kind <= postkind_string_cst)
#define is_string(x) ((x)->kind >= kind_string && (x)->kind <= postkind_string)
#define is_id_label(x) ((x)->kind >= kind_id_label && (x)->kind <= postkind_id_label)
#define is_case_label(x) ((x)->kind >= kind_case_label && (x)->kind <= postkind_case_label)
#define is_default_label(x) ((x)->kind >= kind_default_label && (x)->kind <= postkind_default_label)
#define is_word(x) ((x)->kind >= kind_word && (x)->kind <= postkind_word)
#define is_asm_operand(x) ((x)->kind >= kind_asm_operand && (x)->kind <= postkind_asm_operand)
#define is_adjust_rc(x) ((x)->kind >= kind_adjust_rc && (x)->kind <= postkind_adjust_rc)
#define is_generic_rc(x) ((x)->kind >= kind_generic_rc && (x)->kind <= postkind_generic_rc)
#define is_update_rc(x) ((x)->kind >= kind_update_rc && (x)->kind <= postkind_update_rc)
#define is_adjust_function_decl(x) ((x)->kind >= kind_adjust_function_decl && (x)->kind <= postkind_adjust_function_decl)
#define is_adjustarray_function_decl(x) ((x)->kind >= kind_adjustarray_function_decl && (x)->kind <= postkind_adjustarray_function_decl)
#define is_update_function_decl(x) ((x)->kind >= kind_update_function_decl && (x)->kind <= postkind_update_function_decl)
#define is_error_decl(x) ((x)->kind >= kind_error_decl && (x)->kind <= postkind_error_decl)
#define is_struct_ref(x) ((x)->kind >= kind_struct_ref && (x)->kind <= postkind_struct_ref)
#define is_union_ref(x) ((x)->kind >= kind_union_ref && (x)->kind <= postkind_union_ref)
#define is_enum_ref(x) ((x)->kind >= kind_enum_ref && (x)->kind <= postkind_enum_ref)
#define is_error_stmt(x) ((x)->kind >= kind_error_stmt && (x)->kind <= postkind_error_stmt)
#define is_while_stmt(x) ((x)->kind >= kind_while_stmt && (x)->kind <= postkind_while_stmt)
#define is_dowhile_stmt(x) ((x)->kind >= kind_dowhile_stmt && (x)->kind <= postkind_dowhile_stmt)
#define is_error_expr(x) ((x)->kind >= kind_error_expr && (x)->kind <= postkind_error_expr)
#define is_array_ref(x) ((x)->kind >= kind_array_ref && (x)->kind <= postkind_array_ref)
#define is_dereference(x) ((x)->kind >= kind_dereference && (x)->kind <= postkind_dereference)
#define is_extension_expr(x) ((x)->kind >= kind_extension_expr && (x)->kind <= postkind_extension_expr)
#define is_sizeof_expr(x) ((x)->kind >= kind_sizeof_expr && (x)->kind <= postkind_sizeof_expr)
#define is_alignof_expr(x) ((x)->kind >= kind_alignof_expr && (x)->kind <= postkind_alignof_expr)
#define is_realpart(x) ((x)->kind >= kind_realpart && (x)->kind <= postkind_realpart)
#define is_imagpart(x) ((x)->kind >= kind_imagpart && (x)->kind <= postkind_imagpart)
#define is_address_of(x) ((x)->kind >= kind_address_of && (x)->kind <= postkind_address_of)
#define is_unary_minus(x) ((x)->kind >= kind_unary_minus && (x)->kind <= postkind_unary_minus)
#define is_unary_plus(x) ((x)->kind >= kind_unary_plus && (x)->kind <= postkind_unary_plus)
#define is_conjugate(x) ((x)->kind >= kind_conjugate && (x)->kind <= postkind_conjugate)
#define is_preincrement(x) ((x)->kind >= kind_preincrement && (x)->kind <= postkind_preincrement)
#define is_predecrement(x) ((x)->kind >= kind_predecrement && (x)->kind <= postkind_predecrement)
#define is_postincrement(x) ((x)->kind >= kind_postincrement && (x)->kind <= postkind_postincrement)
#define is_postdecrement(x) ((x)->kind >= kind_postdecrement && (x)->kind <= postkind_postdecrement)
#define is_bitnot(x) ((x)->kind >= kind_bitnot && (x)->kind <= postkind_bitnot)
#define is_not(x) ((x)->kind >= kind_not && (x)->kind <= postkind_not)
#define is_plus(x) ((x)->kind >= kind_plus && (x)->kind <= postkind_plus)
#define is_minus(x) ((x)->kind >= kind_minus && (x)->kind <= postkind_minus)
#define is_times(x) ((x)->kind >= kind_times && (x)->kind <= postkind_times)
#define is_divide(x) ((x)->kind >= kind_divide && (x)->kind <= postkind_divide)
#define is_modulo(x) ((x)->kind >= kind_modulo && (x)->kind <= postkind_modulo)
#define is_lshift(x) ((x)->kind >= kind_lshift && (x)->kind <= postkind_lshift)
#define is_rshift(x) ((x)->kind >= kind_rshift && (x)->kind <= postkind_rshift)
#define is_leq(x) ((x)->kind >= kind_leq && (x)->kind <= postkind_leq)
#define is_geq(x) ((x)->kind >= kind_geq && (x)->kind <= postkind_geq)
#define is_lt(x) ((x)->kind >= kind_lt && (x)->kind <= postkind_lt)
#define is_gt(x) ((x)->kind >= kind_gt && (x)->kind <= postkind_gt)
#define is_eq(x) ((x)->kind >= kind_eq && (x)->kind <= postkind_eq)
#define is_ne(x) ((x)->kind >= kind_ne && (x)->kind <= postkind_ne)
#define is_bitand(x) ((x)->kind >= kind_bitand && (x)->kind <= postkind_bitand)
#define is_bitor(x) ((x)->kind >= kind_bitor && (x)->kind <= postkind_bitor)
#define is_bitxor(x) ((x)->kind >= kind_bitxor && (x)->kind <= postkind_bitxor)
#define is_andand(x) ((x)->kind >= kind_andand && (x)->kind <= postkind_andand)
#define is_oror(x) ((x)->kind >= kind_oror && (x)->kind <= postkind_oror)
#define is_plus_assign(x) ((x)->kind >= kind_plus_assign && (x)->kind <= postkind_plus_assign)
#define is_minus_assign(x) ((x)->kind >= kind_minus_assign && (x)->kind <= postkind_minus_assign)
#define is_times_assign(x) ((x)->kind >= kind_times_assign && (x)->kind <= postkind_times_assign)
#define is_divide_assign(x) ((x)->kind >= kind_divide_assign && (x)->kind <= postkind_divide_assign)
#define is_modulo_assign(x) ((x)->kind >= kind_modulo_assign && (x)->kind <= postkind_modulo_assign)
#define is_lshift_assign(x) ((x)->kind >= kind_lshift_assign && (x)->kind <= postkind_lshift_assign)
#define is_rshift_assign(x) ((x)->kind >= kind_rshift_assign && (x)->kind <= postkind_rshift_assign)
#define is_bitand_assign(x) ((x)->kind >= kind_bitand_assign && (x)->kind <= postkind_bitand_assign)
#define is_bitor_assign(x) ((x)->kind >= kind_bitor_assign && (x)->kind <= postkind_bitor_assign)
#define is_bitxor_assign(x) ((x)->kind >= kind_bitxor_assign && (x)->kind <= postkind_bitxor_assign)
#define is_global_rc(x) ((x)->kind >= kind_global_rc && (x)->kind <= postkind_global_rc)
