#include "cyc_include.h"

 struct _tagged_ptr0{ int* curr; int* base; int* last_plus_one; } ; struct
_tuple0{ struct Cyc_List_List* f1; struct Cyc_List_List* f2; } ; struct _tuple1{
void* f1; struct _tagged_string* f2; } ; struct _tuple2{ struct Cyc_Core_Opt* f1;
struct Cyc_Absyn_Tqual* f2; void* f3; } ; struct _tuple3{ struct Cyc_Absyn_Exp*
f1; struct Cyc_Absyn_Stmt* f2; } ; struct _tuple4{ struct _tagged_string* f1;
struct Cyc_Absyn_Tqual* f2; void* f3; } ; struct _tuple5{ void* f1; struct Cyc_Core_Opt*
f2; } ; struct _tuple6{ struct Cyc_Absyn_Tqual* f1; void* f2; struct Cyc_List_List*
f3; struct Cyc_List_List* f4; } ; struct _tuple7{ struct _tuple1* f1; struct Cyc_Absyn_Tqual*
f2; void* f3; struct Cyc_List_List* f4; struct Cyc_List_List* f5; } ; struct
_tuple8{ struct _tuple7* f1; struct Cyc_Core_Opt* f2; } ; struct _tuple9{ struct
Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual* f2; void* f3; struct Cyc_List_List* f4;
} ; struct _tuple10{ struct Cyc_Absyn_Tqual* f1; void* f2; } ; struct _tuple11{
struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; } ; struct _tuple12{
void* f1; int f2; } ; struct _tuple13{ struct Cyc_Parse_Declarator* f1; struct
Cyc_Absyn_Exp* f2; } ; struct _tuple14{ struct Cyc_Absyn_Tqual* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; } ; struct _tuple15{ struct Cyc_Parse_Declarator*
f1; struct Cyc_Core_Opt* f2; } ; struct _tuple16{ struct Cyc_List_List* f1; int
f2; struct Cyc_Core_Opt* f3; } ; struct _tuple17{ struct Cyc_List_List* f1;
struct Cyc_Absyn_Pat* f2; } ; struct _tagged_ptr1{ short* curr; short* base;
short* last_plus_one; } ; struct _tagged_ptr2{ struct _xenum_struct** curr;
struct _xenum_struct** base; struct _xenum_struct** last_plus_one; } ; struct
_tagged_ptr3{ struct Cyc_Yyltype* curr; struct Cyc_Yyltype* base; struct Cyc_Yyltype*
last_plus_one; } ; struct _tuple18{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp*
f2; } ; typedef unsigned int Cyc_uint; typedef char* Cyc_Cstring; typedef struct
_tagged_string Cyc_string; typedef struct _tagged_string Cyc_string_t; typedef
struct _tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void* exit(
int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; typedef struct Cyc_Core_Opt*
Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg_tag[ 11u]; struct Cyc_Core_InvalidArg_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Core_Failure_tag[ 8u];
struct Cyc_Core_Failure_struct{ char* tag; struct _tagged_string f1; } ; extern
char Cyc_Core_Impossible_tag[ 11u]; struct Cyc_Core_Impossible_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Not_found_tag[ 10u]; struct
Cyc_Core_Not_found_struct{ char* tag; } ; extern char Cyc_Core_Unreachable_tag[
12u]; struct Cyc_Core_Unreachable_struct{ char* tag; struct _tagged_string f1; }
; extern char* string_to_Cstring( struct _tagged_string); extern char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( char*); extern int system( char*); struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; extern struct Cyc_Stdio___sFILE*
Cyc_Stdio_stderr; typedef unsigned int Cyc_Stdio_size_t; typedef int Cyc_Stdio_fpos_t;
extern char Cyc_Stdio_FileOpenError_tag[ 14u]; struct Cyc_Stdio_FileOpenError_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Stdio_FileCloseError_tag[
15u]; struct Cyc_Stdio_FileCloseError_struct{ char* tag; } ; extern char Cyc_Lexing_Error_tag[
6u]; struct Cyc_Lexing_Error_struct{ char* tag; struct _tagged_string f1; } ;
struct Cyc_Lexing_lexbuf{ void(* refill_buff)( struct Cyc_Lexing_lexbuf*); void*
refill_state; struct _tagged_string lex_buffer; int lex_buffer_len; int
lex_abs_pos; int lex_start_pos; int lex_curr_pos; int lex_last_pos; int
lex_last_action; int lex_eof_reached; } ; typedef struct Cyc_Lexing_lexbuf* Cyc_Lexing_Lexbuf;
struct Cyc_Lexing_function_lexbuf_state{ int(* read_fun)( struct _tagged_string,
int, void*); void* read_fun_state; } ; typedef struct Cyc_Lexing_function_lexbuf_state*
Cyc_Lexing_Function_lexbuf_state; struct Cyc_Lexing_lex_tables{ struct
_tagged_ptr0 lex_base; struct _tagged_ptr0 lex_backtrk; struct _tagged_ptr0
lex_default; struct _tagged_ptr0 lex_trans; struct _tagged_ptr0 lex_check; } ;
typedef struct Cyc_Lexing_lex_tables* Cyc_Lexing_LexTables; extern struct Cyc_Lexing_lexbuf*
Cyc_Lexing_from_file( struct Cyc_Stdio___sFILE*); struct Cyc_List_List{ void* hd;
struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List* Cyc_List_glist_t;
typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t;
extern char Cyc_List_List_empty_tag[ 11u]; struct Cyc_List_List_empty_struct{
char* tag; } ; extern struct Cyc_List_List* Cyc_List_map( void*(* f)( void*),
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_map_c( void*(* f)(
void*, void*), void* env, struct Cyc_List_List* x); extern char Cyc_List_List_mismatch_tag[
14u]; struct Cyc_List_List_mismatch_struct{ char* tag; } ; extern void Cyc_List_iter(
void(* f)( void*), struct Cyc_List_List* x); extern void Cyc_List_iter_c( void(*
f)( void*, void*), void* env, struct Cyc_List_List* x); extern void* Cyc_List_fold_right(
void*(* f)( void*, void*), struct Cyc_List_List* x, void* accum); extern struct
Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_append( struct Cyc_List_List* x, struct Cyc_List_List* y); extern
struct Cyc_List_List* Cyc_List_imp_append( struct Cyc_List_List* x, struct Cyc_List_List*
y); extern struct Cyc_List_List* Cyc_List_flatten( struct Cyc_List_List* x);
extern char Cyc_List_Nth_tag[ 4u]; struct Cyc_List_Nth_struct{ char* tag; } ;
extern struct Cyc_List_List* Cyc_List_zip( struct Cyc_List_List* x, struct Cyc_List_List*
y); extern struct _tuple0 Cyc_List_split( struct Cyc_List_List* x); extern
unsigned int Cyc_String_strlen( struct _tagged_string s); extern int Cyc_String_strcmp(
struct _tagged_string s1, struct _tagged_string s2); extern int Cyc_String_zstrcmp(
struct _tagged_string, struct _tagged_string); extern int Cyc_String_zstrptrcmp(
struct _tagged_string*, struct _tagged_string*); extern struct _tagged_string
Cyc_String_strcat( struct _tagged_string dest, struct _tagged_string src);
extern struct _tagged_string Cyc_String_strcpy( struct _tagged_string dest,
struct _tagged_string src); struct Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t;
typedef struct Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t;
extern char Cyc_Set_Absent_tag[ 7u]; struct Cyc_Set_Absent_struct{ char* tag; }
; struct Cyc_Lineno_Pos{ struct _tagged_string logical_file; struct
_tagged_string line; int line_no; int col; } ; typedef struct Cyc_Lineno_Pos*
Cyc_Lineno_pos_t; extern char Cyc_Position_Exit_tag[ 5u]; struct Cyc_Position_Exit_struct{
char* tag; } ; struct Cyc_Position_Segment; typedef struct Cyc_Position_Segment*
Cyc_Position_seg_t; extern struct Cyc_Position_Segment* Cyc_Position_segment_of_abs(
int, int); extern struct Cyc_Position_Segment* Cyc_Position_segment_join( struct
Cyc_Position_Segment*, struct Cyc_Position_Segment*); extern struct
_tagged_string Cyc_Position_string_of_segment( struct Cyc_Position_Segment*);
typedef void* Cyc_Position_Error_kind; extern void* Cyc_Position_Lex; extern
void* Cyc_Position_Parse; extern void* Cyc_Position_Elab; typedef void* Cyc_Position_error_kind_t;
struct Cyc_Position_Error{ struct _tagged_string source; struct Cyc_Position_Segment*
seg; void* kind; struct _tagged_string desc; } ; typedef struct Cyc_Position_Error*
Cyc_Position_error_t; extern struct Cyc_Position_Error* Cyc_Position_mk_err_parse(
struct Cyc_Position_Segment*, struct _tagged_string); extern char Cyc_Position_Nocontext_tag[
10u]; struct Cyc_Position_Nocontext_struct{ char* tag; } ; extern void Cyc_Position_post_error(
struct Cyc_Position_Error*); typedef struct _tagged_string* Cyc_Absyn_field_name;
typedef struct _tagged_string* Cyc_Absyn_var; typedef struct _tagged_string* Cyc_Absyn_tvarname_t;
typedef void* Cyc_Absyn_Nmspace; typedef struct _tuple1* Cyc_Absyn_qvar; typedef
struct _tuple1* Cyc_Absyn_qvar_opt_t; typedef struct _tuple1* Cyc_Absyn_typedef_name_t;
typedef struct _tuple1* Cyc_Absyn_typedef_name_opt_t; struct Cyc_Absyn_Tvar;
typedef void* Cyc_Absyn_Scope; struct Cyc_Absyn_Tqual; typedef void* Cyc_Absyn_Size_of;
typedef void* Cyc_Absyn_Kind; typedef void* Cyc_Absyn_Array_kind; typedef void*
Cyc_Absyn_Sign; struct Cyc_Absyn_Conref; typedef void* Cyc_Absyn_Constraint;
typedef void* Cyc_Absyn_Bounds; struct Cyc_Absyn_PtrInfo; struct Cyc_Absyn_FnInfo;
typedef void* Cyc_Absyn_Typ; typedef void* Cyc_Absyn_Funcparams; typedef void*
Cyc_Absyn_Type_modifier; typedef void* Cyc_Absyn_Cnst; typedef void* Cyc_Absyn_Primop;
typedef void* Cyc_Absyn_Incrementor; typedef void* Cyc_Absyn_Raw_exp; struct Cyc_Absyn_Exp;
typedef void* Cyc_Absyn_Raw_stmt; struct Cyc_Absyn_Stmt; typedef void* Cyc_Absyn_Raw_pat;
struct Cyc_Absyn_Pat; typedef void* Cyc_Absyn_Binding; struct Cyc_Absyn_Switch_clause;
struct Cyc_Absyn_Fndecl; struct Cyc_Absyn_Structdecl; struct Cyc_Absyn_Uniondecl;
struct Cyc_Absyn_Enumfield; struct Cyc_Absyn_Enumdecl; struct Cyc_Absyn_Xenumdecl;
struct Cyc_Absyn_Typedefdecl; struct Cyc_Absyn_Vardecl; typedef void* Cyc_Absyn_Raw_decl;
struct Cyc_Absyn_Decl; typedef void* Cyc_Absyn_Designator; typedef struct
_xenum_struct* Cyc_Absyn_StmtAnnot; typedef void* Cyc_Absyn_Attribute; struct
Cyc_Absyn_Structfield; typedef void* Cyc_Absyn_Mallocarg; typedef void* Cyc_Absyn_scope;
typedef struct Cyc_Absyn_Tqual* Cyc_Absyn_tqual; typedef void* Cyc_Absyn_size_of_t;
typedef void* Cyc_Absyn_kind_t; typedef struct Cyc_Absyn_Tvar* Cyc_Absyn_tvar;
typedef void* Cyc_Absyn_sign_t; typedef struct Cyc_Absyn_Conref* Cyc_Absyn_conref_t;
typedef void* Cyc_Absyn_constraint_t; typedef void* Cyc_Absyn_bounds_t; typedef
struct Cyc_Absyn_PtrInfo Cyc_Absyn_ptr_info_t; typedef struct Cyc_Absyn_FnInfo
Cyc_Absyn_fn_info_t; typedef void* Cyc_Absyn_typ; typedef void* Cyc_Absyn_funcparams_t;
typedef void* Cyc_Absyn_type_modifier; typedef void* Cyc_Absyn_cnst_t; typedef
void* Cyc_Absyn_primop; typedef void* Cyc_Absyn_incrementor_t; typedef void* Cyc_Absyn_raw_exp_t;
typedef struct Cyc_Absyn_Exp* Cyc_Absyn_exp; typedef struct Cyc_Absyn_Exp* Cyc_Absyn_exp_opt_t;
typedef void* Cyc_Absyn_raw_stmt_t; typedef struct Cyc_Absyn_Stmt* Cyc_Absyn_stmt;
typedef struct Cyc_Absyn_Stmt* Cyc_Absyn_stmt_opt_t; typedef void* Cyc_Absyn_raw_pat_t;
typedef struct Cyc_Absyn_Pat* Cyc_Absyn_pat; typedef void* Cyc_Absyn_binding_t;
typedef struct Cyc_Absyn_Switch_clause* Cyc_Absyn_switch_clause; typedef struct
Cyc_Absyn_Fndecl* Cyc_Absyn_fndecl; typedef struct Cyc_Absyn_Structdecl* Cyc_Absyn_structdecl;
typedef struct Cyc_Absyn_Uniondecl* Cyc_Absyn_uniondecl_t; typedef struct Cyc_Absyn_Enumfield*
Cyc_Absyn_enumfield; typedef struct Cyc_Absyn_Enumdecl* Cyc_Absyn_enumdecl;
typedef struct Cyc_Absyn_Xenumdecl* Cyc_Absyn_xenumdecl; typedef struct Cyc_Absyn_Typedefdecl*
Cyc_Absyn_typedefdecl; typedef struct Cyc_Absyn_Vardecl* Cyc_Absyn_vardecl;
typedef void* Cyc_Absyn_raw_decl; typedef struct Cyc_Absyn_Decl* Cyc_Absyn_decl;
typedef void* Cyc_Absyn_designator; typedef struct _xenum_struct* Cyc_Absyn_stmt_annot_t;
typedef void* Cyc_Absyn_attribute_t; typedef struct Cyc_List_List* Cyc_Absyn_attributes_t;
typedef struct Cyc_Absyn_Structfield* Cyc_Absyn_structfield_t; typedef void* Cyc_Absyn_mallocarg_t;
extern void* Cyc_Absyn_Loc_n; extern const int Cyc_Absyn_Rel_n_tag; struct Cyc_Absyn_Rel_n_struct{
int tag; struct Cyc_List_List* f1; } ; extern const int Cyc_Absyn_Abs_n_tag;
struct Cyc_Absyn_Abs_n_struct{ int tag; struct Cyc_List_List* f1; } ; extern
void* Cyc_Absyn_Static; extern void* Cyc_Absyn_Abstract; extern void* Cyc_Absyn_Public;
extern void* Cyc_Absyn_Extern; extern void* Cyc_Absyn_ExternC; struct Cyc_Absyn_Tqual{
int q_const: 1; int q_volatile: 1; int q_restrict: 1; } ; extern void* Cyc_Absyn_B1;
extern void* Cyc_Absyn_B2; extern void* Cyc_Absyn_B4; extern void* Cyc_Absyn_B8;
extern void* Cyc_Absyn_AnyKind; extern void* Cyc_Absyn_MemKind; extern void* Cyc_Absyn_BoxKind;
extern void* Cyc_Absyn_RgnKind; extern void* Cyc_Absyn_EffKind; extern void* Cyc_Absyn_Signed;
extern void* Cyc_Absyn_Unsigned; struct Cyc_Absyn_Conref{ void* v; } ; extern
const int Cyc_Absyn_Eq_constr_tag; struct Cyc_Absyn_Eq_constr_struct{ int tag;
void* f1; } ; extern const int Cyc_Absyn_Forward_constr_tag; struct Cyc_Absyn_Forward_constr_struct{
int tag; struct Cyc_Absyn_Conref* f1; } ; extern void* Cyc_Absyn_No_constr;
struct Cyc_Absyn_Tvar{ struct _tagged_string* name; struct Cyc_Absyn_Conref*
kind; } ; extern void* Cyc_Absyn_Unknown_b; extern const int Cyc_Absyn_Upper_b_tag;
struct Cyc_Absyn_Upper_b_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; struct
Cyc_Absyn_PtrInfo{ void* elt_typ; void* rgn_typ; struct Cyc_Absyn_Conref*
nullable; struct Cyc_Absyn_Tqual* tq; struct Cyc_Absyn_Conref* bounds; } ;
struct Cyc_Absyn_FnInfo{ struct Cyc_List_List* tvars; struct Cyc_Core_Opt*
effect; void* ret_typ; struct Cyc_List_List* args; int varargs; struct Cyc_List_List*
attributes; } ; extern void* Cyc_Absyn_VoidType; extern const int Cyc_Absyn_Evar_tag;
struct Cyc_Absyn_Evar_struct{ int tag; void* f1; struct Cyc_Core_Opt* f2; int f3;
} ; extern const int Cyc_Absyn_VarType_tag; struct Cyc_Absyn_VarType_struct{ int
tag; struct Cyc_Absyn_Tvar* f1; } ; extern const int Cyc_Absyn_EnumType_tag;
struct Cyc_Absyn_EnumType_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Enumdecl** f3; } ; extern const int Cyc_Absyn_XenumType_tag;
struct Cyc_Absyn_XenumType_struct{ int tag; struct _tuple1* f1; struct Cyc_Absyn_Xenumdecl**
f2; } ; extern const int Cyc_Absyn_PointerType_tag; struct Cyc_Absyn_PointerType_struct{
int tag; struct Cyc_Absyn_PtrInfo f1; } ; extern const int Cyc_Absyn_IntType_tag;
struct Cyc_Absyn_IntType_struct{ int tag; void* f1; void* f2; } ; extern void*
Cyc_Absyn_FloatType; extern void* Cyc_Absyn_DoubleType; extern const int Cyc_Absyn_ArrayType_tag;
struct Cyc_Absyn_ArrayType_struct{ int tag; void* f1; struct Cyc_Absyn_Tqual* f2;
struct Cyc_Absyn_Exp* f3; } ; extern const int Cyc_Absyn_FnType_tag; struct Cyc_Absyn_FnType_struct{
int tag; struct Cyc_Absyn_FnInfo f1; } ; extern const int Cyc_Absyn_TupleType_tag;
struct Cyc_Absyn_TupleType_struct{ int tag; struct Cyc_List_List* f1; } ; extern
const int Cyc_Absyn_StructType_tag; struct Cyc_Absyn_StructType_struct{ int tag;
struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Structdecl** f3;
} ; extern const int Cyc_Absyn_UnionType_tag; struct Cyc_Absyn_UnionType_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Uniondecl**
f3; } ; extern const int Cyc_Absyn_RgnHandleType_tag; struct Cyc_Absyn_RgnHandleType_struct{
int tag; void* f1; } ; extern const int Cyc_Absyn_TypedefType_tag; struct Cyc_Absyn_TypedefType_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3;
} ; extern void* Cyc_Absyn_HeapRgn; extern const int Cyc_Absyn_AccessEff_tag;
struct Cyc_Absyn_AccessEff_struct{ int tag; void* f1; } ; extern const int Cyc_Absyn_JoinEff_tag;
struct Cyc_Absyn_JoinEff_struct{ int tag; struct Cyc_List_List* f1; } ; extern
const int Cyc_Absyn_NoTypes_tag; struct Cyc_Absyn_NoTypes_struct{ int tag;
struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; } ; extern const int
Cyc_Absyn_WithTypes_tag; struct Cyc_Absyn_WithTypes_struct{ int tag; struct Cyc_List_List*
f1; int f2; struct Cyc_Core_Opt* f3; } ; typedef void* Cyc_Absyn_Pointer_Sort;
extern const int Cyc_Absyn_NonNullable_ps_tag; struct Cyc_Absyn_NonNullable_ps_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; extern const int Cyc_Absyn_Nullable_ps_tag;
struct Cyc_Absyn_Nullable_ps_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
extern void* Cyc_Absyn_TaggedArray_ps; extern const int Cyc_Absyn_Regparm_att_tag;
struct Cyc_Absyn_Regparm_att_struct{ int tag; int f1; } ; extern void* Cyc_Absyn_Stdcall_att;
extern void* Cyc_Absyn_Cdecl_att; extern void* Cyc_Absyn_Noreturn_att; extern
void* Cyc_Absyn_Const_att; extern const int Cyc_Absyn_Aligned_att_tag; struct
Cyc_Absyn_Aligned_att_struct{ int tag; int f1; } ; extern void* Cyc_Absyn_Packed_att;
extern const int Cyc_Absyn_Section_att_tag; struct Cyc_Absyn_Section_att_struct{
int tag; struct _tagged_string f1; } ; extern void* Cyc_Absyn_Nocommon_att;
extern void* Cyc_Absyn_Shared_att; extern void* Cyc_Absyn_Unused_att; extern
void* Cyc_Absyn_Weak_att; extern void* Cyc_Absyn_Dllimport_att; extern void* Cyc_Absyn_Dllexport_att;
extern void* Cyc_Absyn_No_instrument_function_att; extern void* Cyc_Absyn_Constructor_att;
extern void* Cyc_Absyn_Destructor_att; extern void* Cyc_Absyn_No_check_memory_usage_att;
extern void* Cyc_Absyn_Carray_mod; extern const int Cyc_Absyn_ConstArray_mod_tag;
struct Cyc_Absyn_ConstArray_mod_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
extern const int Cyc_Absyn_Pointer_mod_tag; struct Cyc_Absyn_Pointer_mod_struct{
int tag; void* f1; void* f2; struct Cyc_Absyn_Tqual* f3; } ; extern const int
Cyc_Absyn_Function_mod_tag; struct Cyc_Absyn_Function_mod_struct{ int tag; void*
f1; } ; extern const int Cyc_Absyn_TypeParams_mod_tag; struct Cyc_Absyn_TypeParams_mod_struct{
int tag; struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; int f3; } ;
extern const int Cyc_Absyn_Attributes_mod_tag; struct Cyc_Absyn_Attributes_mod_struct{
int tag; struct Cyc_Position_Segment* f1; struct Cyc_List_List* f2; } ; extern
const int Cyc_Absyn_Char_c_tag; struct Cyc_Absyn_Char_c_struct{ int tag; void*
f1; char f2; } ; extern const int Cyc_Absyn_Short_c_tag; struct Cyc_Absyn_Short_c_struct{
int tag; void* f1; short f2; } ; extern const int Cyc_Absyn_Int_c_tag; struct
Cyc_Absyn_Int_c_struct{ int tag; void* f1; int f2; } ; extern const int Cyc_Absyn_LongLong_c_tag;
struct Cyc_Absyn_LongLong_c_struct{ int tag; void* f1; long long f2; } ; extern
const int Cyc_Absyn_Float_c_tag; struct Cyc_Absyn_Float_c_struct{ int tag;
struct _tagged_string f1; } ; extern const int Cyc_Absyn_String_c_tag; struct
Cyc_Absyn_String_c_struct{ int tag; struct _tagged_string f1; } ; extern void*
Cyc_Absyn_Null_c; extern void* Cyc_Absyn_Plus; extern void* Cyc_Absyn_Times;
extern void* Cyc_Absyn_Minus; extern void* Cyc_Absyn_Div; extern void* Cyc_Absyn_Mod;
extern void* Cyc_Absyn_Eq; extern void* Cyc_Absyn_Neq; extern void* Cyc_Absyn_Gt;
extern void* Cyc_Absyn_Lt; extern void* Cyc_Absyn_Gte; extern void* Cyc_Absyn_Lte;
extern void* Cyc_Absyn_Not; extern void* Cyc_Absyn_Bitnot; extern void* Cyc_Absyn_Bitand;
extern void* Cyc_Absyn_Bitor; extern void* Cyc_Absyn_Bitxor; extern void* Cyc_Absyn_Bitlshift;
extern void* Cyc_Absyn_Bitlrshift; extern void* Cyc_Absyn_Bitarshift; extern
void* Cyc_Absyn_Size; extern void* Cyc_Absyn_Printf; extern void* Cyc_Absyn_Fprintf;
extern void* Cyc_Absyn_Xprintf; extern void* Cyc_Absyn_Scanf; extern void* Cyc_Absyn_Fscanf;
extern void* Cyc_Absyn_Sscanf; extern void* Cyc_Absyn_PreInc; extern void* Cyc_Absyn_PostInc;
extern void* Cyc_Absyn_PreDec; extern void* Cyc_Absyn_PostDec; extern const int
Cyc_Absyn_Typ_m_tag; struct Cyc_Absyn_Typ_m_struct{ int tag; void* f1; } ;
extern const int Cyc_Absyn_Unresolved_m_tag; struct Cyc_Absyn_Unresolved_m_struct{
int tag; struct _tuple1* f1; } ; extern const int Cyc_Absyn_Enum_m_tag; struct
Cyc_Absyn_Enum_m_struct{ int tag; struct Cyc_Absyn_Enumdecl* f1; struct Cyc_Absyn_Enumfield*
f2; } ; extern const int Cyc_Absyn_Xenum_m_tag; struct Cyc_Absyn_Xenum_m_struct{
int tag; struct Cyc_Absyn_Xenumdecl* f1; struct Cyc_Absyn_Enumfield* f2; } ;
extern const int Cyc_Absyn_Const_e_tag; struct Cyc_Absyn_Const_e_struct{ int tag;
void* f1; } ; extern const int Cyc_Absyn_Var_e_tag; struct Cyc_Absyn_Var_e_struct{
int tag; struct _tuple1* f1; void* f2; } ; extern const int Cyc_Absyn_UnknownId_e_tag;
struct Cyc_Absyn_UnknownId_e_struct{ int tag; struct _tuple1* f1; } ; extern
const int Cyc_Absyn_Primop_e_tag; struct Cyc_Absyn_Primop_e_struct{ int tag;
void* f1; struct Cyc_List_List* f2; } ; extern const int Cyc_Absyn_AssignOp_e_tag;
struct Cyc_Absyn_AssignOp_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
Cyc_Core_Opt* f2; struct Cyc_Absyn_Exp* f3; } ; extern const int Cyc_Absyn_Increment_e_tag;
struct Cyc_Absyn_Increment_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; void* f2;
} ; extern const int Cyc_Absyn_Conditional_e_tag; struct Cyc_Absyn_Conditional_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp*
f3; } ; extern const int Cyc_Absyn_SeqExp_e_tag; struct Cyc_Absyn_SeqExp_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2; } ; extern const
int Cyc_Absyn_UnknownCall_e_tag; struct Cyc_Absyn_UnknownCall_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; extern const int Cyc_Absyn_FnCall_e_tag;
struct Cyc_Absyn_FnCall_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List*
f2; } ; extern const int Cyc_Absyn_Throw_e_tag; struct Cyc_Absyn_Throw_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; extern const int Cyc_Absyn_NoInstantiate_e_tag;
struct Cyc_Absyn_NoInstantiate_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
extern const int Cyc_Absyn_Instantiate_e_tag; struct Cyc_Absyn_Instantiate_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; extern const
int Cyc_Absyn_Cast_e_tag; struct Cyc_Absyn_Cast_e_struct{ int tag; void* f1;
struct Cyc_Absyn_Exp* f2; } ; extern const int Cyc_Absyn_Address_e_tag; struct
Cyc_Absyn_Address_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; extern const
int Cyc_Absyn_New_e_tag; struct Cyc_Absyn_New_e_struct{ int tag; struct Cyc_Absyn_Exp*
f1; } ; extern const int Cyc_Absyn_Sizeoftyp_e_tag; struct Cyc_Absyn_Sizeoftyp_e_struct{
int tag; void* f1; } ; extern const int Cyc_Absyn_Sizeofexp_e_tag; struct Cyc_Absyn_Sizeofexp_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; extern const int Cyc_Absyn_Deref_e_tag;
struct Cyc_Absyn_Deref_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; extern
const int Cyc_Absyn_StructMember_e_tag; struct Cyc_Absyn_StructMember_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct _tagged_string* f2; } ; extern const
int Cyc_Absyn_StructArrow_e_tag; struct Cyc_Absyn_StructArrow_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; struct _tagged_string* f2; } ; extern const int Cyc_Absyn_Subscript_e_tag;
struct Cyc_Absyn_Subscript_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
Cyc_Absyn_Exp* f2; } ; extern const int Cyc_Absyn_Tuple_e_tag; struct Cyc_Absyn_Tuple_e_struct{
int tag; struct Cyc_List_List* f1; } ; extern const int Cyc_Absyn_CompoundLit_e_tag;
struct Cyc_Absyn_CompoundLit_e_struct{ int tag; struct _tuple2* f1; struct Cyc_List_List*
f2; } ; extern const int Cyc_Absyn_Array_e_tag; struct Cyc_Absyn_Array_e_struct{
int tag; struct Cyc_List_List* f1; } ; extern const int Cyc_Absyn_Comprehension_e_tag;
struct Cyc_Absyn_Comprehension_e_struct{ int tag; struct Cyc_Absyn_Vardecl* f1;
struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp* f3; } ; extern const int Cyc_Absyn_Struct_e_tag;
struct Cyc_Absyn_Struct_e_struct{ int tag; struct _tuple1* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Structdecl* f4; } ; extern const
int Cyc_Absyn_Enum_e_tag; struct Cyc_Absyn_Enum_e_struct{ int tag; struct Cyc_Core_Opt*
f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Enumdecl*
f4; struct Cyc_Absyn_Enumfield* f5; } ; extern const int Cyc_Absyn_Xenum_e_tag;
struct Cyc_Absyn_Xenum_e_struct{ int tag; struct Cyc_Core_Opt* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Xenumdecl* f3; struct Cyc_Absyn_Enumfield* f4; } ; extern
const int Cyc_Absyn_Malloc_e_tag; struct Cyc_Absyn_Malloc_e_struct{ int tag;
void* f1; } ; extern const int Cyc_Absyn_UnresolvedMem_e_tag; struct Cyc_Absyn_UnresolvedMem_e_struct{
int tag; struct Cyc_Core_Opt* f1; struct Cyc_List_List* f2; } ; extern const int
Cyc_Absyn_StmtExp_e_tag; struct Cyc_Absyn_StmtExp_e_struct{ int tag; struct Cyc_Absyn_Stmt*
f1; } ; extern const int Cyc_Absyn_Codegen_e_tag; struct Cyc_Absyn_Codegen_e_struct{
int tag; struct Cyc_Absyn_Fndecl* f1; } ; extern const int Cyc_Absyn_Fill_e_tag;
struct Cyc_Absyn_Fill_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; struct
Cyc_Absyn_Exp{ struct Cyc_Core_Opt* topt; void* r; struct Cyc_Position_Segment*
loc; } ; extern void* Cyc_Absyn_Skip_s; extern const int Cyc_Absyn_Exp_s_tag;
struct Cyc_Absyn_Exp_s_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; extern
const int Cyc_Absyn_Seq_s_tag; struct Cyc_Absyn_Seq_s_struct{ int tag; struct
Cyc_Absyn_Stmt* f1; struct Cyc_Absyn_Stmt* f2; } ; extern const int Cyc_Absyn_Return_s_tag;
struct Cyc_Absyn_Return_s_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; extern
const int Cyc_Absyn_IfThenElse_s_tag; struct Cyc_Absyn_IfThenElse_s_struct{ int
tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; struct Cyc_Absyn_Stmt*
f3; } ; extern const int Cyc_Absyn_While_s_tag; struct Cyc_Absyn_While_s_struct{
int tag; struct _tuple3 f1; struct Cyc_Absyn_Stmt* f2; } ; extern const int Cyc_Absyn_Break_s_tag;
struct Cyc_Absyn_Break_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; extern
const int Cyc_Absyn_Continue_s_tag; struct Cyc_Absyn_Continue_s_struct{ int tag;
struct Cyc_Absyn_Stmt* f1; } ; extern const int Cyc_Absyn_Goto_s_tag; struct Cyc_Absyn_Goto_s_struct{
int tag; struct _tagged_string* f1; struct Cyc_Absyn_Stmt* f2; } ; extern const
int Cyc_Absyn_For_s_tag; struct Cyc_Absyn_For_s_struct{ int tag; struct Cyc_Absyn_Exp*
f1; struct _tuple3 f2; struct _tuple3 f3; struct Cyc_Absyn_Stmt* f4; } ; extern
const int Cyc_Absyn_Switch_s_tag; struct Cyc_Absyn_Switch_s_struct{ int tag;
struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; extern const int Cyc_Absyn_Fallthru_s_tag;
struct Cyc_Absyn_Fallthru_s_struct{ int tag; struct Cyc_List_List* f1; struct
Cyc_Absyn_Switch_clause** f2; } ; extern const int Cyc_Absyn_Decl_s_tag; struct
Cyc_Absyn_Decl_s_struct{ int tag; struct Cyc_Absyn_Decl* f1; struct Cyc_Absyn_Stmt*
f2; } ; extern const int Cyc_Absyn_Cut_s_tag; struct Cyc_Absyn_Cut_s_struct{ int
tag; struct Cyc_Absyn_Stmt* f1; } ; extern const int Cyc_Absyn_Splice_s_tag;
struct Cyc_Absyn_Splice_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; extern
const int Cyc_Absyn_Label_s_tag; struct Cyc_Absyn_Label_s_struct{ int tag;
struct _tagged_string* f1; struct Cyc_Absyn_Stmt* f2; } ; extern const int Cyc_Absyn_Do_s_tag;
struct Cyc_Absyn_Do_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; struct _tuple3
f2; } ; extern const int Cyc_Absyn_TryCatch_s_tag; struct Cyc_Absyn_TryCatch_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_List_List* f2; } ; struct Cyc_Absyn_Stmt{
void* r; struct Cyc_Position_Segment* loc; struct Cyc_List_List* non_local_preds;
int try_depth; struct _xenum_struct* annot; } ; extern void* Cyc_Absyn_Wild_p;
extern const int Cyc_Absyn_Var_p_tag; struct Cyc_Absyn_Var_p_struct{ int tag;
struct Cyc_Absyn_Vardecl* f1; } ; extern void* Cyc_Absyn_Null_p; extern const
int Cyc_Absyn_Int_p_tag; struct Cyc_Absyn_Int_p_struct{ int tag; void* f1; int
f2; } ; extern const int Cyc_Absyn_Char_p_tag; struct Cyc_Absyn_Char_p_struct{
int tag; char f1; } ; extern const int Cyc_Absyn_Float_p_tag; struct Cyc_Absyn_Float_p_struct{
int tag; struct _tagged_string f1; } ; extern const int Cyc_Absyn_Tuple_p_tag;
struct Cyc_Absyn_Tuple_p_struct{ int tag; struct Cyc_List_List* f1; } ; extern
const int Cyc_Absyn_Pointer_p_tag; struct Cyc_Absyn_Pointer_p_struct{ int tag;
struct Cyc_Absyn_Pat* f1; } ; extern const int Cyc_Absyn_Reference_p_tag; struct
Cyc_Absyn_Reference_p_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ; extern
const int Cyc_Absyn_Struct_p_tag; struct Cyc_Absyn_Struct_p_struct{ int tag;
struct Cyc_Absyn_Structdecl* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List*
f3; struct Cyc_List_List* f4; } ; extern const int Cyc_Absyn_Enum_p_tag; struct
Cyc_Absyn_Enum_p_struct{ int tag; struct _tuple1* f1; struct Cyc_Core_Opt* f2;
struct Cyc_List_List* f3; struct Cyc_List_List* f4; struct Cyc_Absyn_Enumdecl*
f5; struct Cyc_Absyn_Enumfield* f6; } ; extern const int Cyc_Absyn_Xenum_p_tag;
struct Cyc_Absyn_Xenum_p_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Xenumdecl* f4; struct Cyc_Absyn_Enumfield*
f5; } ; extern const int Cyc_Absyn_UnknownId_p_tag; struct Cyc_Absyn_UnknownId_p_struct{
int tag; struct _tuple1* f1; } ; extern const int Cyc_Absyn_UnknownCall_p_tag;
struct Cyc_Absyn_UnknownCall_p_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; } ; extern const int Cyc_Absyn_UnknownFields_p_tag;
struct Cyc_Absyn_UnknownFields_p_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; } ; struct Cyc_Absyn_Pat{ void* r; struct Cyc_Core_Opt*
topt; struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Switch_clause{
struct Cyc_Absyn_Pat* pattern; struct Cyc_Core_Opt* pat_vars; struct Cyc_Absyn_Exp*
where_clause; struct Cyc_Absyn_Stmt* body; struct Cyc_Position_Segment* loc; } ;
extern void* Cyc_Absyn_Unresolved_b; extern const int Cyc_Absyn_Global_b_tag;
struct Cyc_Absyn_Global_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ;
extern const int Cyc_Absyn_Funname_b_tag; struct Cyc_Absyn_Funname_b_struct{ int
tag; struct Cyc_Absyn_Fndecl* f1; } ; extern const int Cyc_Absyn_Param_b_tag;
struct Cyc_Absyn_Param_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ;
extern const int Cyc_Absyn_Local_b_tag; struct Cyc_Absyn_Local_b_struct{ int tag;
struct Cyc_Absyn_Vardecl* f1; } ; extern const int Cyc_Absyn_Pat_b_tag; struct
Cyc_Absyn_Pat_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ; struct Cyc_Absyn_Vardecl{
void* sc; struct _tuple1* name; struct Cyc_Absyn_Tqual* tq; void* type; struct
Cyc_Absyn_Exp* initializer; int shadow; struct Cyc_Core_Opt* region; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc; int is_inline; struct _tuple1*
name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt* effect; void* ret_type;
struct Cyc_List_List* args; int varargs; struct Cyc_Absyn_Stmt* body; struct Cyc_Core_Opt*
cached_typ; struct Cyc_Core_Opt* param_vardecls; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Structfield{ struct _tagged_string* name;
struct Cyc_Absyn_Tqual* tq; void* type; struct Cyc_Core_Opt* width; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Structdecl{ void* sc; struct Cyc_Core_Opt* name;
struct Cyc_List_List* tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Uniondecl{ void* sc; struct Cyc_Core_Opt* name;
struct Cyc_List_List* tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Enumfield{ struct _tuple1* name; struct Cyc_Absyn_Exp*
tag; struct Cyc_List_List* tvs; struct Cyc_List_List* typs; struct Cyc_Position_Segment*
loc; } ; struct Cyc_Absyn_Enumdecl{ void* sc; struct Cyc_Core_Opt* name; struct
Cyc_List_List* tvs; struct Cyc_Core_Opt* fields; } ; struct Cyc_Absyn_Xenumdecl{
void* sc; struct _tuple1* name; struct Cyc_List_List* fields; } ; struct Cyc_Absyn_Typedefdecl{
struct _tuple1* name; struct Cyc_List_List* tvs; void* defn; } ; extern const
int Cyc_Absyn_Var_d_tag; struct Cyc_Absyn_Var_d_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; } ; extern const int Cyc_Absyn_Fn_d_tag; struct Cyc_Absyn_Fn_d_struct{ int
tag; struct Cyc_Absyn_Fndecl* f1; } ; extern const int Cyc_Absyn_Let_d_tag;
struct Cyc_Absyn_Let_d_struct{ int tag; struct Cyc_Absyn_Pat* f1; struct Cyc_Core_Opt*
f2; struct Cyc_Core_Opt* f3; struct Cyc_Absyn_Exp* f4; int f5; } ; extern const
int Cyc_Absyn_Struct_d_tag; struct Cyc_Absyn_Struct_d_struct{ int tag; struct
Cyc_Absyn_Structdecl* f1; } ; extern const int Cyc_Absyn_Union_d_tag; struct Cyc_Absyn_Union_d_struct{
int tag; struct Cyc_Absyn_Uniondecl* f1; } ; extern const int Cyc_Absyn_Enum_d_tag;
struct Cyc_Absyn_Enum_d_struct{ int tag; struct Cyc_Absyn_Enumdecl* f1; } ;
extern const int Cyc_Absyn_Xenum_d_tag; struct Cyc_Absyn_Xenum_d_struct{ int tag;
struct Cyc_Absyn_Xenumdecl* f1; } ; extern const int Cyc_Absyn_Typedef_d_tag;
struct Cyc_Absyn_Typedef_d_struct{ int tag; struct Cyc_Absyn_Typedefdecl* f1; }
; extern const int Cyc_Absyn_Namespace_d_tag; struct Cyc_Absyn_Namespace_d_struct{
int tag; struct _tagged_string* f1; struct Cyc_List_List* f2; } ; extern const
int Cyc_Absyn_Using_d_tag; struct Cyc_Absyn_Using_d_struct{ int tag; struct
_tuple1* f1; struct Cyc_List_List* f2; } ; extern const int Cyc_Absyn_ExternC_d_tag;
struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ; struct
Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; extern const int
Cyc_Absyn_ArrayElement_tag; struct Cyc_Absyn_ArrayElement_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; extern const int Cyc_Absyn_FieldName_tag; struct
Cyc_Absyn_FieldName_struct{ int tag; struct _tagged_string* f1; } ; extern char
Cyc_Absyn_EmptyAnnot_tag[ 11u]; struct Cyc_Absyn_EmptyAnnot_struct{ char* tag; }
; extern struct Cyc_Absyn_Tqual* Cyc_Absyn_combine_tqual( struct Cyc_Absyn_Tqual*
x, struct Cyc_Absyn_Tqual* y); extern struct Cyc_Absyn_Tqual* Cyc_Absyn_empty_tqual();
extern struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref( void* x); extern struct
Cyc_Absyn_Conref* Cyc_Absyn_empty_conref(); extern void* Cyc_Absyn_new_evar(
void*); extern void* Cyc_Absyn_uchar_t; extern void* Cyc_Absyn_uint_t; extern
void* Cyc_Absyn_sint_t; extern void* Cyc_Absyn_float_t; extern void* Cyc_Absyn_double_t;
extern void* Cyc_Absyn_starb_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual* tq,
void* b); extern void* Cyc_Absyn_atb_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual*
tq, void* b); extern void* Cyc_Absyn_tagged_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual*
tq); extern struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp( void*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_New_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_null_exp( struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_true_exp( struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_false_exp( struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_int_exp( void*,
int, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_signed_int_exp(
int, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp(
unsigned int, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_char_exp(
char c, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_float_exp(
struct _tagged_string f, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_string_exp( struct _tagged_string s, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_primop_exp( void*, struct Cyc_List_List*
es, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_prim1_exp(
void*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_prim2_exp( void*, struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_eq_exp( struct
Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern
struct Cyc_Absyn_Exp* Cyc_Absyn_neq_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_gt_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_lt_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_gte_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_lte_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Core_Opt*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_post_inc_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_post_dec_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_pre_inc_exp( struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_pre_dec_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_conditional_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_and_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_or_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_seq_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_unknowncall_exp(
struct Cyc_Absyn_Exp*, struct Cyc_List_List*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_throw_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_noinstantiate_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_instantiate_exp( struct Cyc_Absyn_Exp*, struct Cyc_List_List*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_cast_exp( void*,
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_address_exp( struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_sizeoftyp_exp( void* t, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_sizeofexp_exp( struct Cyc_Absyn_Exp* e,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_deref_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_structmember_exp( struct Cyc_Absyn_Exp*, struct _tagged_string*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_structarrow_exp(
struct Cyc_Absyn_Exp*, struct _tagged_string*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_subscript_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_tuple_exp( struct Cyc_List_List*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_stmt_exp( struct Cyc_Absyn_Stmt*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_new_stmt( void*
s, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_skip_stmt(
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_exp_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_seq_stmt( struct Cyc_Absyn_Stmt* s1, struct Cyc_Absyn_Stmt* s2, struct
Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_return_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_ifthenelse_stmt( struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Stmt* s1,
struct Cyc_Absyn_Stmt* s2, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_while_stmt( struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Stmt* s, struct
Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_break_stmt(
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_continue_stmt(
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_for_stmt(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3,
struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_switch_stmt( struct Cyc_Absyn_Exp* e, struct Cyc_List_List*, struct
Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_fallthru_stmt(
struct Cyc_List_List* el, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_do_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_trycatch_stmt( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* scs, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_goto_stmt( struct _tagged_string* lab, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Pat* Cyc_Absyn_new_pat( void* p, struct Cyc_Position_Segment*
s); extern struct Cyc_Absyn_Decl* Cyc_Absyn_new_decl( void* r, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_let_decl( struct Cyc_Absyn_Pat* p,
struct Cyc_Core_Opt* t_opt, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl( struct _tuple1* x,
void* t, struct Cyc_Absyn_Exp* init); extern struct Cyc_Absyn_Decl* Cyc_Absyn_struct_decl(
void* s, struct Cyc_Core_Opt* n, struct Cyc_List_List* ts, struct Cyc_Core_Opt*
fs, struct Cyc_List_List* atts, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Decl* Cyc_Absyn_union_decl( void* s, struct Cyc_Core_Opt* n, struct
Cyc_List_List* ts, struct Cyc_Core_Opt* fs, struct Cyc_List_List* atts, struct
Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_enum_decl(
void* s, struct Cyc_Core_Opt* n, struct Cyc_List_List* ts, struct Cyc_Core_Opt*
fs, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_xenum_decl(
void* s, struct _tuple1* n, struct Cyc_List_List* fs, struct Cyc_Position_Segment*
loc); extern void* Cyc_Absyn_function_typ( struct Cyc_List_List* tvs, struct Cyc_Core_Opt*
eff_typ, void* ret_typ, struct Cyc_List_List* args, int varargs, struct Cyc_List_List*);
extern struct _tagged_string Cyc_Absyn_attribute2string( void*); struct Cyc_PP_Ppstate;
typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef
struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc*
Cyc_PP_doc_t; extern void Cyc_Lex_register_typedef( struct _tuple1* s); extern
void Cyc_Lex_enter_namespace( struct _tagged_string*); extern void Cyc_Lex_leave_namespace();
extern void Cyc_Lex_enter_using( struct _tuple1*); extern void Cyc_Lex_leave_using();
typedef void* Cyc_Parse_Struct_or_union; void* Cyc_Parse_Struct_su=( void*) 0;
void* Cyc_Parse_Union_su=( void*) 1; typedef void* Cyc_Parse_struct_or_union_t;
typedef void* Cyc_Parse_Blockitem; const int Cyc_Parse_TopDecls_bl_tag= 0;
struct Cyc_Parse_TopDecls_bl_struct{ int tag; struct Cyc_List_List* f1; } ;
const int Cyc_Parse_Stmt_bl_tag= 1; struct Cyc_Parse_Stmt_bl_struct{ int tag;
struct Cyc_Absyn_Stmt* f1; } ; const int Cyc_Parse_FnDecl_bl_tag= 2; struct Cyc_Parse_FnDecl_bl_struct{
int tag; struct Cyc_Absyn_Fndecl* f1; } ; typedef void* Cyc_Parse_blockitem_t;
typedef void* Cyc_Parse_Type_specifier; const int Cyc_Parse_Signed_spec_tag= 0;
struct Cyc_Parse_Signed_spec_struct{ int tag; struct Cyc_Position_Segment* f1; }
; const int Cyc_Parse_Unsigned_spec_tag= 1; struct Cyc_Parse_Unsigned_spec_struct{
int tag; struct Cyc_Position_Segment* f1; } ; const int Cyc_Parse_Short_spec_tag=
2; struct Cyc_Parse_Short_spec_struct{ int tag; struct Cyc_Position_Segment* f1;
} ; const int Cyc_Parse_Long_spec_tag= 3; struct Cyc_Parse_Long_spec_struct{ int
tag; struct Cyc_Position_Segment* f1; } ; const int Cyc_Parse_Type_spec_tag= 4;
struct Cyc_Parse_Type_spec_struct{ int tag; void* f1; struct Cyc_Position_Segment*
f2; } ; const int Cyc_Parse_Decl_spec_tag= 5; struct Cyc_Parse_Decl_spec_struct{
int tag; struct Cyc_Absyn_Decl* f1; } ; typedef void* Cyc_Parse_type_specifier_t;
typedef void* Cyc_Parse_Storage_class; void* Cyc_Parse_Typedef_sc=( void*) 0;
void* Cyc_Parse_Extern_sc=( void*) 1; void* Cyc_Parse_ExternC_sc=( void*) 2;
void* Cyc_Parse_Static_sc=( void*) 3; void* Cyc_Parse_Auto_sc=( void*) 4; void*
Cyc_Parse_Register_sc=( void*) 5; void* Cyc_Parse_Abstract_sc=( void*) 6;
typedef void* Cyc_Parse_storage_class_t; struct Cyc_Parse_Declaration_spec{
struct Cyc_Core_Opt* sc; struct Cyc_Absyn_Tqual* tq; struct Cyc_List_List*
type_specs; int is_inline; struct Cyc_List_List* attributes; } ; typedef struct
Cyc_Parse_Declaration_spec* Cyc_Parse_decl_spec_t; struct Cyc_Parse_Declarator{
struct _tuple1* id; struct Cyc_List_List* tms; } ; typedef struct Cyc_Parse_Declarator*
Cyc_Parse_declarator_t; struct Cyc_Parse_Abstractdeclarator{ struct Cyc_List_List*
tms; } ; typedef struct Cyc_Parse_Abstractdeclarator* Cyc_Parse_abstractdeclarator_t;
static struct _tuple4* Cyc_Parse_fnargspec_to_arg( struct Cyc_Position_Segment*
loc, struct _tuple2* t); static struct _tuple5 Cyc_Parse_collapse_type_specifiers(
struct Cyc_List_List* ts, struct Cyc_Position_Segment* loc); static struct
_tuple6 Cyc_Parse_apply_tms( struct Cyc_Absyn_Tqual*, void*, struct Cyc_List_List*,
struct Cyc_List_List*); static struct Cyc_Absyn_Decl* Cyc_Parse_v_typ_to_typedef(
struct Cyc_Position_Segment* loc, struct _tuple7* t); struct Cyc_Core_Opt* Cyc_Parse_lbuf=
0; static struct Cyc_List_List* Cyc_Parse_parse_result= 0; static void Cyc_Parse_err(
struct _tagged_string msg, struct Cyc_Position_Segment* sg){ Cyc_Position_post_error(
Cyc_Position_mk_err_parse( sg, msg));} static void* Cyc_Parse_abort( struct
_tagged_string msg, struct Cyc_Position_Segment* sg){ Cyc_Parse_err( msg, sg);(
void) _throw(({ struct Cyc_Position_Exit_struct* _temp0=( struct Cyc_Position_Exit_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Position_Exit_struct)); _temp0->tag= Cyc_Position_Exit_tag;(
struct _xenum_struct*) _temp0;}));} static void Cyc_Parse_warn( struct
_tagged_string msg, struct Cyc_Position_Segment* sg){({ struct _tagged_string
_temp1= Cyc_Position_string_of_segment( sg); struct _tagged_string _temp2= msg;
fprintf( Cyc_Stdio_stderr,"%.*s: Warning: %.*s\n", _temp1.last_plus_one - _temp1.curr,
_temp1.curr, _temp2.last_plus_one - _temp2.curr, _temp2.curr);}); return;}
static void* Cyc_Parse_unimp( struct _tagged_string msg, struct Cyc_Position_Segment*
sg){ return(( void*(*)( struct _tagged_string msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)(({ struct _tagged_string _temp3= msg; xprintf("%.*s unimplemented",
_temp3.last_plus_one - _temp3.curr, _temp3.curr);}), sg);} static void Cyc_Parse_unimp2(
struct _tagged_string msg, struct Cyc_Position_Segment* sg){({ struct
_tagged_string _temp4= Cyc_Position_string_of_segment( sg); struct
_tagged_string _temp5= msg; fprintf( Cyc_Stdio_stderr,"%.*s: Warning: Cyclone does not yet support %.*s\n",
_temp4.last_plus_one - _temp4.curr, _temp4.curr, _temp5.last_plus_one - _temp5.curr,
_temp5.curr);}); return;} static struct Cyc_Absyn_Structfield* Cyc_Parse_make_struct_field(
struct Cyc_Position_Segment* loc, struct _tuple8* field_info){ struct Cyc_Core_Opt*
_temp8; struct _tuple7* _temp10; struct _tuple8 _temp6=* field_info; _LL11:
_temp10= _temp6.f1; goto _LL9; _LL9: _temp8= _temp6.f2; goto _LL7; _LL7: if((*
_temp10).f4 != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp12=( char*)"bad type params in struct field";
struct _tagged_string _temp13; _temp13.curr= _temp12; _temp13.base= _temp12;
_temp13.last_plus_one= _temp12 + 32; _temp13;}), loc);}{ struct _tuple1* qid=(*
_temp10).f1;{ void* _temp14=(* qid).f1; struct Cyc_List_List* _temp24; struct
Cyc_List_List* _temp26; _LL16: if(( unsigned int) _temp14 > 1u?(( struct
_enum_struct*) _temp14)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL25: _temp24=( struct
Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp14)->f1; if( _temp24 == 0){
goto _LL17;} else{ goto _LL18;}} else{ goto _LL18;} _LL18: if(( unsigned int)
_temp14 > 1u?(( struct _enum_struct*) _temp14)->tag == Cyc_Absyn_Abs_n_tag: 0){
_LL27: _temp26=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp14)->f1; if( _temp26 == 0){ goto _LL19;} else{ goto _LL20;}} else{ goto
_LL20;} _LL20: if( _temp14 == Cyc_Absyn_Loc_n){ goto _LL21;} else{ goto _LL22;}
_LL22: goto _LL23; _LL17: goto _LL15; _LL19: goto _LL15; _LL21: goto _LL15;
_LL23: Cyc_Parse_err(( struct _tagged_string)({ char* _temp28=( char*)"struct field cannot be qualified with a module name";
struct _tagged_string _temp29; _temp29.curr= _temp28; _temp29.base= _temp28;
_temp29.last_plus_one= _temp28 + 52; _temp29;}), loc); goto _LL15; _LL15:;}{
struct Cyc_List_List* atts=(* _temp10).f5; return({ struct Cyc_Absyn_Structfield*
_temp30=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp30->name=(* qid).f2; _temp30->tq=(* _temp10).f2; _temp30->type=( void*)(*
_temp10).f3; _temp30->width= _temp8; _temp30->attributes= atts; _temp30;});}}}
static struct _tuple2* Cyc_Parse_make_param( struct Cyc_Position_Segment* loc,
struct _tuple9* field){ struct _tuple9 _temp33; struct Cyc_List_List* _temp34;
void* _temp36; struct Cyc_Absyn_Tqual* _temp38; struct Cyc_Core_Opt* _temp40;
struct _tuple9* _temp31= field; _temp33=* _temp31; _LL41: _temp40= _temp33.f1;
goto _LL39; _LL39: _temp38= _temp33.f2; goto _LL37; _LL37: _temp36= _temp33.f3;
goto _LL35; _LL35: _temp34= _temp33.f4; goto _LL32; _LL32: { struct Cyc_Core_Opt*
idopt= 0; if( _temp40 != 0){ idopt=({ struct Cyc_Core_Opt* _temp42=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp42->v=( void*)(*(( struct _tuple1*)
_temp40->v)).f2; _temp42;});{ void* _temp43=(*(( struct _tuple1*) _temp40->v)).f1;
struct Cyc_List_List* _temp53; struct Cyc_List_List* _temp55; _LL45: if((
unsigned int) _temp43 > 1u?(( struct _enum_struct*) _temp43)->tag == Cyc_Absyn_Rel_n_tag:
0){ _LL54: _temp53=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp43)->f1; if( _temp53 == 0){ goto _LL46;} else{ goto _LL47;}} else{ goto
_LL47;} _LL47: if(( unsigned int) _temp43 > 1u?(( struct _enum_struct*) _temp43)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL56: _temp55=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp43)->f1; if( _temp55 == 0){ goto _LL48;} else{ goto _LL49;}} else{ goto
_LL49;} _LL49: if( _temp43 == Cyc_Absyn_Loc_n){ goto _LL50;} else{ goto _LL51;}
_LL51: goto _LL52; _LL46: goto _LL44; _LL48: goto _LL44; _LL50: goto _LL44;
_LL52: Cyc_Parse_err(( struct _tagged_string)({ char* _temp57=( char*)"parameter cannot be qualified with a module name";
struct _tagged_string _temp58; _temp58.curr= _temp57; _temp58.base= _temp57;
_temp58.last_plus_one= _temp57 + 49; _temp58;}), loc); goto _LL44; _LL44:;}} if(
_temp34 != 0){(( void(*)( struct _tagged_string msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)(( struct _tagged_string)({ char* _temp59=( char*)"parameter should have no type parameters";
struct _tagged_string _temp60; _temp60.curr= _temp59; _temp60.base= _temp59;
_temp60.last_plus_one= _temp59 + 41; _temp60;}), loc);} return({ struct _tuple2*
_temp61=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2)); _temp61->f1=
idopt; _temp61->f2= _temp38; _temp61->f3= _temp36; _temp61;});}} static void*
Cyc_Parse_type_spec( void* t, struct Cyc_Position_Segment* loc){ return({ struct
Cyc_Parse_Type_spec_struct* _temp62=( struct Cyc_Parse_Type_spec_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Type_spec_struct)); _temp62->tag= Cyc_Parse_Type_spec_tag;
_temp62->f1=( void*) t; _temp62->f2= loc;( void*) _temp62;});} static void* Cyc_Parse_array2ptr(
void* t){ void* _temp63= t; struct Cyc_Absyn_Exp* _temp69; struct Cyc_Absyn_Tqual*
_temp71; void* _temp73; _LL65: if(( unsigned int) _temp63 > 4u?(( struct
_enum_struct*) _temp63)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL74: _temp73=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp63)->f1; goto _LL72; _LL72:
_temp71=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*) _temp63)->f2;
goto _LL70; _LL70: _temp69=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp63)->f3; goto _LL66;} else{ goto _LL67;} _LL67: goto _LL68; _LL66: return
Cyc_Absyn_starb_typ( _temp73, Cyc_Absyn_HeapRgn, _temp71, _temp69 == 0? Cyc_Absyn_Unknown_b:({
struct Cyc_Absyn_Upper_b_struct* _temp75=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp75->tag= Cyc_Absyn_Upper_b_tag;
_temp75->f1=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp76= _temp69;
if( _temp76 == 0){ _throw( Null_Exception);} _temp76;});( void*) _temp75;}));
_LL68: return t; _LL64:;} static void Cyc_Parse_arg_array2ptr( struct _tuple2* x){(*
x).f3= Cyc_Parse_array2ptr((* x).f3);} static struct _tuple10* Cyc_Parse_get_tqual_typ(
struct Cyc_Position_Segment* loc, struct _tuple2* t){ return({ struct _tuple10*
_temp77=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp77->f1=(*
t).f2; _temp77->f2=(* t).f3; _temp77;});} static void Cyc_Parse_only_vardecl(
struct Cyc_List_List* params, struct Cyc_Absyn_Decl* x){ struct _tagged_string
decl_kind;{ void* _temp78=( void*) x->r; struct Cyc_Absyn_Vardecl* _temp102; int
_temp104; struct Cyc_Absyn_Exp* _temp106; struct Cyc_Core_Opt* _temp108; struct
Cyc_Core_Opt* _temp110; struct Cyc_Absyn_Pat* _temp112; struct Cyc_Absyn_Fndecl*
_temp114; struct Cyc_Absyn_Structdecl* _temp116; struct Cyc_Absyn_Uniondecl*
_temp118; struct Cyc_Absyn_Enumdecl* _temp120; struct Cyc_Absyn_Typedefdecl*
_temp122; struct Cyc_Absyn_Xenumdecl* _temp124; struct Cyc_List_List* _temp126;
struct _tagged_string* _temp128; struct Cyc_List_List* _temp130; struct _tuple1*
_temp132; struct Cyc_List_List* _temp134; _LL80: if((( struct _enum_struct*)
_temp78)->tag == Cyc_Absyn_Var_d_tag){ _LL103: _temp102=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_d_struct*) _temp78)->f1; goto _LL81;} else{ goto _LL82;}
_LL82: if((( struct _enum_struct*) _temp78)->tag == Cyc_Absyn_Let_d_tag){ _LL113:
_temp112=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp78)->f1;
goto _LL111; _LL111: _temp110=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp78)->f2; goto _LL109; _LL109: _temp108=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp78)->f3; goto _LL107; _LL107: _temp106=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Let_d_struct*) _temp78)->f4; goto _LL105; _LL105: _temp104=( int)((
struct Cyc_Absyn_Let_d_struct*) _temp78)->f5; goto _LL83;} else{ goto _LL84;}
_LL84: if((( struct _enum_struct*) _temp78)->tag == Cyc_Absyn_Fn_d_tag){ _LL115:
_temp114=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*) _temp78)->f1;
goto _LL85;} else{ goto _LL86;} _LL86: if((( struct _enum_struct*) _temp78)->tag
== Cyc_Absyn_Struct_d_tag){ _LL117: _temp116=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_d_struct*) _temp78)->f1; goto _LL87;} else{ goto _LL88;}
_LL88: if((( struct _enum_struct*) _temp78)->tag == Cyc_Absyn_Union_d_tag){
_LL119: _temp118=( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*)
_temp78)->f1; goto _LL89;} else{ goto _LL90;} _LL90: if((( struct _enum_struct*)
_temp78)->tag == Cyc_Absyn_Enum_d_tag){ _LL121: _temp120=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_d_struct*) _temp78)->f1; goto _LL91;} else{ goto _LL92;}
_LL92: if((( struct _enum_struct*) _temp78)->tag == Cyc_Absyn_Typedef_d_tag){
_LL123: _temp122=( struct Cyc_Absyn_Typedefdecl*)(( struct Cyc_Absyn_Typedef_d_struct*)
_temp78)->f1; goto _LL93;} else{ goto _LL94;} _LL94: if((( struct _enum_struct*)
_temp78)->tag == Cyc_Absyn_Xenum_d_tag){ _LL125: _temp124=( struct Cyc_Absyn_Xenumdecl*)((
struct Cyc_Absyn_Xenum_d_struct*) _temp78)->f1; goto _LL95;} else{ goto _LL96;}
_LL96: if((( struct _enum_struct*) _temp78)->tag == Cyc_Absyn_Namespace_d_tag){
_LL129: _temp128=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp78)->f1; goto _LL127; _LL127: _temp126=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Namespace_d_struct*) _temp78)->f2; goto _LL97;} else{ goto _LL98;}
_LL98: if((( struct _enum_struct*) _temp78)->tag == Cyc_Absyn_Using_d_tag){
_LL133: _temp132=( struct _tuple1*)(( struct Cyc_Absyn_Using_d_struct*) _temp78)->f1;
goto _LL131; _LL131: _temp130=( struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*)
_temp78)->f2; goto _LL99;} else{ goto _LL100;} _LL100: if((( struct _enum_struct*)
_temp78)->tag == Cyc_Absyn_ExternC_d_tag){ _LL135: _temp134=( struct Cyc_List_List*)((
struct Cyc_Absyn_ExternC_d_struct*) _temp78)->f1; goto _LL101;} else{ goto _LL79;}
_LL81: if( _temp102->initializer != 0){(( void(*)( struct _tagged_string msg,
struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct _tagged_string)({
char* _temp136=( char*)"initializers are not allowed in parameter declarations";
struct _tagged_string _temp137; _temp137.curr= _temp136; _temp137.base= _temp136;
_temp137.last_plus_one= _temp136 + 55; _temp137;}), x->loc);}{ void* _temp138=(*
_temp102->name).f1; struct Cyc_List_List* _temp148; struct Cyc_List_List*
_temp150; _LL140: if( _temp138 == Cyc_Absyn_Loc_n){ goto _LL141;} else{ goto
_LL142;} _LL142: if(( unsigned int) _temp138 > 1u?(( struct _enum_struct*)
_temp138)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL149: _temp148=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp138)->f1; if( _temp148 == 0){ goto _LL143;}
else{ goto _LL144;}} else{ goto _LL144;} _LL144: if(( unsigned int) _temp138 > 1u?((
struct _enum_struct*) _temp138)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL151:
_temp150=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp138)->f1;
if( _temp150 == 0){ goto _LL145;} else{ goto _LL146;}} else{ goto _LL146;}
_LL146: goto _LL147; _LL141: goto _LL139; _LL143: goto _LL139; _LL145: goto
_LL139; _LL147: Cyc_Parse_err(( struct _tagged_string)({ char* _temp152=( char*)"module names not allowed on parameter declarations";
struct _tagged_string _temp153; _temp153.curr= _temp152; _temp153.base= _temp152;
_temp153.last_plus_one= _temp152 + 51; _temp153;}), x->loc); goto _LL139; _LL139:;}{
int found= 0; for( 0; params != 0; params= params->tl){ if( Cyc_String_zstrptrcmp((*
_temp102->name).f2,( struct _tagged_string*) params->hd) == 0){ found= 1; break;}}
if( ! found){(( void(*)( struct _tagged_string msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)(({ struct _tagged_string _temp154=*(* _temp102->name).f2;
xprintf("%.*s is not listed as a parameter", _temp154.last_plus_one - _temp154.curr,
_temp154.curr);}), x->loc);} return;} _LL83: decl_kind=( struct _tagged_string)({
char* _temp155=( char*)"let declaration"; struct _tagged_string _temp156;
_temp156.curr= _temp155; _temp156.base= _temp155; _temp156.last_plus_one=
_temp155 + 16; _temp156;}); goto _LL79; _LL85: decl_kind=( struct _tagged_string)({
char* _temp157=( char*)"function declaration"; struct _tagged_string _temp158;
_temp158.curr= _temp157; _temp158.base= _temp157; _temp158.last_plus_one=
_temp157 + 21; _temp158;}); goto _LL79; _LL87: decl_kind=( struct _tagged_string)({
char* _temp159=( char*)"struct declaration"; struct _tagged_string _temp160;
_temp160.curr= _temp159; _temp160.base= _temp159; _temp160.last_plus_one=
_temp159 + 19; _temp160;}); goto _LL79; _LL89: decl_kind=( struct _tagged_string)({
char* _temp161=( char*)"union declaration"; struct _tagged_string _temp162;
_temp162.curr= _temp161; _temp162.base= _temp161; _temp162.last_plus_one=
_temp161 + 18; _temp162;}); goto _LL79; _LL91: decl_kind=( struct _tagged_string)({
char* _temp163=( char*)"enum declaration"; struct _tagged_string _temp164;
_temp164.curr= _temp163; _temp164.base= _temp163; _temp164.last_plus_one=
_temp163 + 17; _temp164;}); goto _LL79; _LL93: decl_kind=( struct _tagged_string)({
char* _temp165=( char*)"typedef"; struct _tagged_string _temp166; _temp166.curr=
_temp165; _temp166.base= _temp165; _temp166.last_plus_one= _temp165 + 8;
_temp166;}); goto _LL79; _LL95: decl_kind=( struct _tagged_string)({ char*
_temp167=( char*)"xenum declaration"; struct _tagged_string _temp168; _temp168.curr=
_temp167; _temp168.base= _temp167; _temp168.last_plus_one= _temp167 + 18;
_temp168;}); goto _LL79; _LL97: decl_kind=( struct _tagged_string)({ char*
_temp169=( char*)"namespace declaration"; struct _tagged_string _temp170;
_temp170.curr= _temp169; _temp170.base= _temp169; _temp170.last_plus_one=
_temp169 + 22; _temp170;}); goto _LL79; _LL99: decl_kind=( struct _tagged_string)({
char* _temp171=( char*)"using declaration"; struct _tagged_string _temp172;
_temp172.curr= _temp171; _temp172.base= _temp171; _temp172.last_plus_one=
_temp171 + 18; _temp172;}); goto _LL79; _LL101: decl_kind=( struct
_tagged_string)({ char* _temp173=( char*)"extern C declaration"; struct
_tagged_string _temp174; _temp174.curr= _temp173; _temp174.base= _temp173;
_temp174.last_plus_one= _temp173 + 21; _temp174;}); goto _LL79; _LL79:;}(( void(*)(
struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({
struct _tagged_string _temp175= decl_kind; xprintf("%.*s appears in parameter type",
_temp175.last_plus_one - _temp175.curr, _temp175.curr);}), x->loc); return;}
static struct _tuple2* Cyc_Parse_get_param_type( struct _tuple11* env, struct
_tagged_string* x){ struct _tuple11 _temp178; struct Cyc_Position_Segment*
_temp179; struct Cyc_List_List* _temp181; struct _tuple11* _temp176= env;
_temp178=* _temp176; _LL182: _temp181= _temp178.f1; goto _LL180; _LL180:
_temp179= _temp178.f2; goto _LL177; _LL177: if( _temp181 == 0){ return(( struct
_tuple2*(*)( struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({
struct _tagged_string _temp183=* x; xprintf("missing type for parameter %.*s",
_temp183.last_plus_one - _temp183.curr, _temp183.curr);}), _temp179);}{ void*
_temp184=( void*)(( struct Cyc_Absyn_Decl*) _temp181->hd)->r; struct Cyc_Absyn_Vardecl*
_temp190; _LL186: if((( struct _enum_struct*) _temp184)->tag == Cyc_Absyn_Var_d_tag){
_LL191: _temp190=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp184)->f1; goto _LL187;} else{ goto _LL188;} _LL188: goto _LL189; _LL187:{
void* _temp192=(* _temp190->name).f1; struct Cyc_List_List* _temp202; struct Cyc_List_List*
_temp204; _LL194: if( _temp192 == Cyc_Absyn_Loc_n){ goto _LL195;} else{ goto
_LL196;} _LL196: if(( unsigned int) _temp192 > 1u?(( struct _enum_struct*)
_temp192)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL203: _temp202=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp192)->f1; if( _temp202 == 0){ goto _LL197;}
else{ goto _LL198;}} else{ goto _LL198;} _LL198: if(( unsigned int) _temp192 > 1u?((
struct _enum_struct*) _temp192)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL205:
_temp204=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp192)->f1;
if( _temp204 == 0){ goto _LL199;} else{ goto _LL200;}} else{ goto _LL200;}
_LL200: goto _LL201; _LL195: goto _LL193; _LL197: goto _LL193; _LL199: goto
_LL193; _LL201: Cyc_Parse_err(( struct _tagged_string)({ char* _temp206=( char*)"module name not allowed on parameter";
struct _tagged_string _temp207; _temp207.curr= _temp206; _temp207.base= _temp206;
_temp207.last_plus_one= _temp206 + 37; _temp207;}), _temp179); goto _LL193;
_LL193:;} if( Cyc_String_zstrptrcmp((* _temp190->name).f2, x) == 0){ return({
struct _tuple2* _temp208=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2) *
1); _temp208[ 0]=({ struct _tuple2 _temp209; _temp209.f1=({ struct Cyc_Core_Opt*
_temp210=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt) * 1);
_temp210[ 0]=({ struct Cyc_Core_Opt _temp211; _temp211.v=( void*)(* _temp190->name).f2;
_temp211;}); _temp210;}); _temp209.f2= _temp190->tq; _temp209.f3=( void*)
_temp190->type; _temp209;}); _temp208;});} else{ return Cyc_Parse_get_param_type(({
struct _tuple11* _temp212=( struct _tuple11*) GC_malloc( sizeof( struct _tuple11));
_temp212->f1= _temp181->tl; _temp212->f2= _temp179; _temp212;}), x);} _LL189:
return(( struct _tuple2*(*)( struct _tagged_string msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)(( struct _tagged_string)({ char* _temp213=( char*)"non-variable declaration";
struct _tagged_string _temp214; _temp214.curr= _temp213; _temp214.base= _temp213;
_temp214.last_plus_one= _temp213 + 25; _temp214;}),(( struct Cyc_Absyn_Decl*)
_temp181->hd)->loc); _LL185:;}} static int Cyc_Parse_is_typeparam( void* tm){
void* _temp215= tm; int _temp221; struct Cyc_Position_Segment* _temp223; struct
Cyc_List_List* _temp225; _LL217: if(( unsigned int) _temp215 > 1u?(( struct
_enum_struct*) _temp215)->tag == Cyc_Absyn_TypeParams_mod_tag: 0){ _LL226:
_temp225=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp215)->f1; goto _LL224; _LL224: _temp223=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp215)->f2; goto _LL222; _LL222:
_temp221=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp215)->f3; goto
_LL218;} else{ goto _LL219;} _LL219: goto _LL220; _LL218: return 1; _LL220:
return 0; _LL216:;} static void* Cyc_Parse_id2type( struct _tagged_string s,
struct Cyc_Absyn_Conref* k){ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({
char* _temp227=( char*)"`H"; struct _tagged_string _temp228; _temp228.curr=
_temp227; _temp228.base= _temp227; _temp228.last_plus_one= _temp227 + 3;
_temp228;})) == 0){ return Cyc_Absyn_HeapRgn;} else{ return({ struct Cyc_Absyn_VarType_struct*
_temp229=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp229->tag= Cyc_Absyn_VarType_tag; _temp229->f1=({ struct Cyc_Absyn_Tvar*
_temp230=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof( struct Cyc_Absyn_Tvar));
_temp230->name=({ struct _tagged_string* _temp231=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp231[ 0]= s; _temp231;});
_temp230->kind= k; _temp230;});( void*) _temp229;});}} static struct Cyc_Absyn_Tvar*
Cyc_Parse_typ2tvar( struct Cyc_Position_Segment* loc, void* t){ void* _temp232=
t; struct Cyc_Absyn_Tvar* _temp238; _LL234: if(( unsigned int) _temp232 > 4u?((
struct _enum_struct*) _temp232)->tag == Cyc_Absyn_VarType_tag: 0){ _LL239:
_temp238=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*) _temp232)->f1;
goto _LL235;} else{ goto _LL236;} _LL236: goto _LL237; _LL235: return _temp238;
_LL237: return(( struct Cyc_Absyn_Tvar*(*)( struct _tagged_string msg, struct
Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct _tagged_string)({ char*
_temp240=( char*)"expecting a list of type variables, not types"; struct
_tagged_string _temp241; _temp241.curr= _temp240; _temp241.base= _temp240;
_temp241.last_plus_one= _temp240 + 46; _temp241;}), loc); _LL233:;} static void*
Cyc_Parse_tvar2typ( struct Cyc_Absyn_Tvar* pr){ return({ struct Cyc_Absyn_VarType_struct*
_temp242=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp242->tag= Cyc_Absyn_VarType_tag; _temp242->f1= pr;( void*) _temp242;});}
static struct Cyc_List_List* Cyc_Parse_oldstyle2newstyle( struct Cyc_List_List*
tms, struct Cyc_List_List* tds, struct Cyc_Position_Segment* loc){ if( tds == 0){
return tms;} if( tms == 0){ return 0;}{ void* _temp243=( void*) tms->hd; void*
_temp249; _LL245: if(( unsigned int) _temp243 > 1u?(( struct _enum_struct*)
_temp243)->tag == Cyc_Absyn_Function_mod_tag: 0){ _LL250: _temp249=( void*)((
struct Cyc_Absyn_Function_mod_struct*) _temp243)->f1; goto _LL246;} else{ goto
_LL247;} _LL247: goto _LL248; _LL246: if( tms->tl == 0? 1:( Cyc_Parse_is_typeparam((
void*)( tms->tl)->hd)?( tms->tl)->tl == 0: 0)){ void* _temp251= _temp249; struct
Cyc_Core_Opt* _temp257; int _temp259; struct Cyc_List_List* _temp261; struct Cyc_Position_Segment*
_temp263; struct Cyc_List_List* _temp265; _LL253: if((( struct _enum_struct*)
_temp251)->tag == Cyc_Absyn_WithTypes_tag){ _LL262: _temp261=( struct Cyc_List_List*)((
struct Cyc_Absyn_WithTypes_struct*) _temp251)->f1; goto _LL260; _LL260: _temp259=(
int)(( struct Cyc_Absyn_WithTypes_struct*) _temp251)->f2; goto _LL258; _LL258:
_temp257=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_WithTypes_struct*) _temp251)->f3;
goto _LL254;} else{ goto _LL255;} _LL255: if((( struct _enum_struct*) _temp251)->tag
== Cyc_Absyn_NoTypes_tag){ _LL266: _temp265=( struct Cyc_List_List*)(( struct
Cyc_Absyn_NoTypes_struct*) _temp251)->f1; goto _LL264; _LL264: _temp263=( struct
Cyc_Position_Segment*)(( struct Cyc_Absyn_NoTypes_struct*) _temp251)->f2; goto
_LL256;} else{ goto _LL252;} _LL254: Cyc_Parse_warn(( struct _tagged_string)({
char* _temp267=( char*)"function declaration with both new- and old-style parameterdeclarations; ignoring old-style";
struct _tagged_string _temp268; _temp268.curr= _temp267; _temp268.base= _temp267;
_temp268.last_plus_one= _temp267 + 92; _temp268;}), loc); return tms; _LL256:((
void(*)( void(* f)( struct Cyc_List_List*, struct Cyc_Absyn_Decl*), struct Cyc_List_List*
env, struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Parse_only_vardecl,
_temp265, tds);{ struct _tuple11* env=({ struct _tuple11* _temp269=( struct
_tuple11*) GC_malloc( sizeof( struct _tuple11)); _temp269->f1= tds; _temp269->f2=
loc; _temp269;}); return({ struct Cyc_List_List* _temp270=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp270->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp271=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp271->tag= Cyc_Absyn_Function_mod_tag; _temp271->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp272=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp272->tag= Cyc_Absyn_WithTypes_tag; _temp272->f1=(( struct Cyc_List_List*(*)(
struct _tuple2*(* f)( struct _tuple11*, struct _tagged_string*), struct _tuple11*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_get_param_type, env,
_temp265); _temp272->f2= 0; _temp272->f3= 0;( void*) _temp272;});( void*)
_temp271;}); _temp270->tl= 0; _temp270;});} _LL252:;} else{ return({ struct Cyc_List_List*
_temp273=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp273->hd=( void*)(( void*) tms->hd); _temp273->tl= Cyc_Parse_oldstyle2newstyle(
tms->tl, tds, loc); _temp273;});} _LL248: return({ struct Cyc_List_List*
_temp274=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp274->hd=( void*)(( void*) tms->hd); _temp274->tl= Cyc_Parse_oldstyle2newstyle(
tms->tl, tds, loc); _temp274;}); _LL244:;}} static struct Cyc_Absyn_Fndecl* Cyc_Parse_make_function(
struct Cyc_Core_Opt* dso, struct Cyc_Parse_Declarator* d, struct Cyc_List_List*
tds, struct Cyc_Absyn_Stmt* body, struct Cyc_Position_Segment* loc){ if( tds !=
0){ d=({ struct Cyc_Parse_Declarator* _temp275=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp275->id= d->id; _temp275->tms=
Cyc_Parse_oldstyle2newstyle( d->tms, tds, loc); _temp275;});}{ void* sc= Cyc_Absyn_Public;
struct Cyc_List_List* tss= 0; struct Cyc_Absyn_Tqual* tq= Cyc_Absyn_empty_tqual();
int is_inline= 0; struct Cyc_List_List* atts= 0; if( dso != 0){ tss=(( struct
Cyc_Parse_Declaration_spec*) dso->v)->type_specs; tq=(( struct Cyc_Parse_Declaration_spec*)
dso->v)->tq; is_inline=(( struct Cyc_Parse_Declaration_spec*) dso->v)->is_inline;
atts=(( struct Cyc_Parse_Declaration_spec*) dso->v)->attributes; if((( struct
Cyc_Parse_Declaration_spec*) dso->v)->sc != 0){ void* _temp276=( void*)(((
struct Cyc_Parse_Declaration_spec*) dso->v)->sc)->v; _LL278: if( _temp276 == Cyc_Parse_Extern_sc){
goto _LL279;} else{ goto _LL280;} _LL280: if( _temp276 == Cyc_Parse_ExternC_sc){
goto _LL281;} else{ goto _LL282;} _LL282: if( _temp276 == Cyc_Parse_Static_sc){
goto _LL283;} else{ goto _LL284;} _LL284: goto _LL285; _LL279: sc= Cyc_Absyn_Extern;
goto _LL277; _LL281: sc= Cyc_Absyn_ExternC; goto _LL277; _LL283: sc= Cyc_Absyn_Static;
goto _LL277; _LL285: Cyc_Parse_err(( struct _tagged_string)({ char* _temp286=(
char*)"bad storage class on function"; struct _tagged_string _temp287; _temp287.curr=
_temp286; _temp287.base= _temp286; _temp287.last_plus_one= _temp286 + 30;
_temp287;}), loc); goto _LL277; _LL277:;}}{ struct Cyc_Core_Opt* _temp290; void*
_temp292; struct _tuple5 _temp288= Cyc_Parse_collapse_type_specifiers( tss, loc);
_LL293: _temp292= _temp288.f1; goto _LL291; _LL291: _temp290= _temp288.f2; goto
_LL289; _LL289: { struct Cyc_List_List* _temp296; struct Cyc_List_List* _temp298;
void* _temp300; struct Cyc_Absyn_Tqual* _temp302; struct _tuple6 _temp294= Cyc_Parse_apply_tms(
tq, _temp292, atts, d->tms); _LL303: _temp302= _temp294.f1; goto _LL301; _LL301:
_temp300= _temp294.f2; goto _LL299; _LL299: _temp298= _temp294.f3; goto _LL297;
_LL297: _temp296= _temp294.f4; goto _LL295; _LL295: if( _temp290 != 0){ Cyc_Parse_warn((
struct _tagged_string)({ char* _temp304=( char*)"nested type declaration within function prototype";
struct _tagged_string _temp305; _temp305.curr= _temp304; _temp305.base= _temp304;
_temp305.last_plus_one= _temp304 + 50; _temp305;}), loc);} if( _temp298 != 0){
Cyc_Parse_warn(( struct _tagged_string)({ char* _temp306=( char*)"bad type params, ignoring";
struct _tagged_string _temp307; _temp307.curr= _temp306; _temp307.base= _temp306;
_temp307.last_plus_one= _temp306 + 26; _temp307;}), loc);}{ void* _temp308=
_temp300; struct Cyc_Absyn_FnInfo _temp314; struct Cyc_List_List* _temp316; int
_temp318; struct Cyc_List_List* _temp320; void* _temp322; struct Cyc_Core_Opt*
_temp324; struct Cyc_List_List* _temp326; _LL310: if(( unsigned int) _temp308 >
4u?(( struct _enum_struct*) _temp308)->tag == Cyc_Absyn_FnType_tag: 0){ _LL315:
_temp314=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*) _temp308)->f1;
_LL327: _temp326=( struct Cyc_List_List*) _temp314.tvars; goto _LL325; _LL325:
_temp324=( struct Cyc_Core_Opt*) _temp314.effect; goto _LL323; _LL323: _temp322=(
void*) _temp314.ret_typ; goto _LL321; _LL321: _temp320=( struct Cyc_List_List*)
_temp314.args; goto _LL319; _LL319: _temp318=( int) _temp314.varargs; goto
_LL317; _LL317: _temp316=( struct Cyc_List_List*) _temp314.attributes; goto
_LL311;} else{ goto _LL312;} _LL312: goto _LL313; _LL311: { struct Cyc_List_List*
args2=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_fnargspec_to_arg, loc, _temp320); return({ struct Cyc_Absyn_Fndecl*
_temp328=( struct Cyc_Absyn_Fndecl*) GC_malloc( sizeof( struct Cyc_Absyn_Fndecl));
_temp328->sc=( void*) sc; _temp328->name= d->id; _temp328->tvs= _temp326;
_temp328->is_inline= is_inline; _temp328->effect= _temp324; _temp328->ret_type=(
void*) _temp322; _temp328->args= args2; _temp328->varargs= _temp318; _temp328->body=
body; _temp328->cached_typ= 0; _temp328->param_vardecls= 0; _temp328->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp316, _temp296); _temp328;});} _LL313: return(( struct Cyc_Absyn_Fndecl*(*)(
struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_string)({ char* _temp329=( char*)"declarator is not a function prototype";
struct _tagged_string _temp330; _temp330.curr= _temp329; _temp330.base= _temp329;
_temp330.last_plus_one= _temp329 + 39; _temp330;}), loc); _LL309:;}}}}} static
struct _tuple4* Cyc_Parse_fnargspec_to_arg( struct Cyc_Position_Segment* loc,
struct _tuple2* t){ if((* t).f1 == 0){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp331=( char*)"missing argument variable in function prototype"; struct
_tagged_string _temp332; _temp332.curr= _temp331; _temp332.base= _temp331;
_temp332.last_plus_one= _temp331 + 48; _temp332;}), loc); return({ struct
_tuple4* _temp333=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp333->f1=({ struct _tagged_string* _temp334=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp334[ 0]=( struct
_tagged_string)({ char* _temp335=( char*)"?"; struct _tagged_string _temp336;
_temp336.curr= _temp335; _temp336.base= _temp335; _temp336.last_plus_one=
_temp335 + 2; _temp336;}); _temp334;}); _temp333->f2=(* t).f2; _temp333->f3=(* t).f3;
_temp333;});} else{ return({ struct _tuple4* _temp337=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp337->f1=( struct _tagged_string*)((* t).f1)->v;
_temp337->f2=(* t).f2; _temp337->f3=(* t).f3; _temp337;});}} static char
_temp340[ 52u]="at most one type may appear within a type specifier"; static
struct _tagged_string Cyc_Parse_msg1=( struct _tagged_string){ _temp340,
_temp340, _temp340 + 52u}; static char _temp343[ 63u]="const or volatile may appear only once within a type specifier";
static struct _tagged_string Cyc_Parse_msg2=( struct _tagged_string){ _temp343,
_temp343, _temp343 + 63u}; static char _temp346[ 50u]="type specifier includes more than one declaration";
static struct _tagged_string Cyc_Parse_msg3=( struct _tagged_string){ _temp346,
_temp346, _temp346 + 50u}; static char _temp349[ 60u]="sign specifier may appear only once within a type specifier";
static struct _tagged_string Cyc_Parse_msg4=( struct _tagged_string){ _temp349,
_temp349, _temp349 + 60u}; static struct _tuple5 Cyc_Parse_collapse_type_specifiers(
struct Cyc_List_List* ts, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt*
declopt= 0; int seen_type= 0; int seen_sign= 0; int seen_size= 0; void* t= Cyc_Absyn_VoidType;
void* sz= Cyc_Absyn_B4; void* sgn= Cyc_Absyn_Signed; struct Cyc_Position_Segment*
last_loc= loc; for( 0; ts != 0; ts= ts->tl){ void* _temp350=( void*) ts->hd;
struct Cyc_Position_Segment* _temp364; void* _temp366; struct Cyc_Position_Segment*
_temp368; struct Cyc_Position_Segment* _temp370; struct Cyc_Position_Segment*
_temp372; struct Cyc_Position_Segment* _temp374; struct Cyc_Absyn_Decl* _temp376;
_LL352: if((( struct _enum_struct*) _temp350)->tag == Cyc_Parse_Type_spec_tag){
_LL367: _temp366=( void*)(( struct Cyc_Parse_Type_spec_struct*) _temp350)->f1;
goto _LL365; _LL365: _temp364=( struct Cyc_Position_Segment*)(( struct Cyc_Parse_Type_spec_struct*)
_temp350)->f2; goto _LL353;} else{ goto _LL354;} _LL354: if((( struct
_enum_struct*) _temp350)->tag == Cyc_Parse_Signed_spec_tag){ _LL369: _temp368=(
struct Cyc_Position_Segment*)(( struct Cyc_Parse_Signed_spec_struct*) _temp350)->f1;
goto _LL355;} else{ goto _LL356;} _LL356: if((( struct _enum_struct*) _temp350)->tag
== Cyc_Parse_Unsigned_spec_tag){ _LL371: _temp370=( struct Cyc_Position_Segment*)((
struct Cyc_Parse_Unsigned_spec_struct*) _temp350)->f1; goto _LL357;} else{ goto
_LL358;} _LL358: if((( struct _enum_struct*) _temp350)->tag == Cyc_Parse_Short_spec_tag){
_LL373: _temp372=( struct Cyc_Position_Segment*)(( struct Cyc_Parse_Short_spec_struct*)
_temp350)->f1; goto _LL359;} else{ goto _LL360;} _LL360: if((( struct
_enum_struct*) _temp350)->tag == Cyc_Parse_Long_spec_tag){ _LL375: _temp374=(
struct Cyc_Position_Segment*)(( struct Cyc_Parse_Long_spec_struct*) _temp350)->f1;
goto _LL361;} else{ goto _LL362;} _LL362: if((( struct _enum_struct*) _temp350)->tag
== Cyc_Parse_Decl_spec_tag){ _LL377: _temp376=( struct Cyc_Absyn_Decl*)(( struct
Cyc_Parse_Decl_spec_struct*) _temp350)->f1; goto _LL363;} else{ goto _LL351;}
_LL353: if( seen_type){ Cyc_Parse_err( Cyc_Parse_msg1, _temp364);} last_loc=
_temp364; seen_type= 1; t= _temp366; goto _LL351; _LL355: if( seen_sign){ Cyc_Parse_err(
Cyc_Parse_msg4, _temp368);} if( seen_type){ Cyc_Parse_err(( struct
_tagged_string)({ char* _temp378=( char*)"signed qualifier must come before type";
struct _tagged_string _temp379; _temp379.curr= _temp378; _temp379.base= _temp378;
_temp379.last_plus_one= _temp378 + 39; _temp379;}), _temp368);} last_loc=
_temp368; seen_sign= 1; sgn= Cyc_Absyn_Signed; goto _LL351; _LL357: if(
seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp370);} if( seen_type){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp380=( char*)"signed qualifier must come before type";
struct _tagged_string _temp381; _temp381.curr= _temp380; _temp381.base= _temp380;
_temp381.last_plus_one= _temp380 + 39; _temp381;}), _temp370);} last_loc=
_temp370; seen_sign= 1; sgn= Cyc_Absyn_Unsigned; goto _LL351; _LL359: if(
seen_size){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp382=( char*)"integral size may appear only once within a type specifier";
struct _tagged_string _temp383; _temp383.curr= _temp382; _temp383.base= _temp382;
_temp383.last_plus_one= _temp382 + 59; _temp383;}), _temp372);} if( seen_type){
Cyc_Parse_err(( struct _tagged_string)({ char* _temp384=( char*)"short modifier must come before base type";
struct _tagged_string _temp385; _temp385.curr= _temp384; _temp385.base= _temp384;
_temp385.last_plus_one= _temp384 + 42; _temp385;}), _temp372);} last_loc=
_temp372; seen_size= 1; sz= Cyc_Absyn_B2; goto _LL351; _LL361: if( seen_type){
Cyc_Parse_err(( struct _tagged_string)({ char* _temp386=( char*)"long modifier must come before base type";
struct _tagged_string _temp387; _temp387.curr= _temp386; _temp387.base= _temp386;
_temp387.last_plus_one= _temp386 + 41; _temp387;}), _temp374);} if( seen_size){
void* _temp388= sz; _LL390: if( _temp388 == Cyc_Absyn_B4){ goto _LL391;} else{
goto _LL392;} _LL392: goto _LL393; _LL391: sz= Cyc_Absyn_B8; goto _LL389; _LL393:
Cyc_Parse_err(( struct _tagged_string)({ char* _temp394=( char*)"extra long in type specifier";
struct _tagged_string _temp395; _temp395.curr= _temp394; _temp395.base= _temp394;
_temp395.last_plus_one= _temp394 + 29; _temp395;}), _temp374); goto _LL389;
_LL389:;} last_loc= _temp374; seen_size= 1; goto _LL351; _LL363: last_loc=
_temp376->loc; if( declopt == 0? ! seen_type: 0){ seen_type= 1;{ void* _temp396=(
void*) _temp376->r; struct Cyc_Absyn_Structdecl* _temp408; struct Cyc_Absyn_Enumdecl*
_temp410; struct Cyc_Absyn_Xenumdecl* _temp412; struct Cyc_Absyn_Uniondecl*
_temp414; _LL398: if((( struct _enum_struct*) _temp396)->tag == Cyc_Absyn_Struct_d_tag){
_LL409: _temp408=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*)
_temp396)->f1; goto _LL399;} else{ goto _LL400;} _LL400: if((( struct
_enum_struct*) _temp396)->tag == Cyc_Absyn_Enum_d_tag){ _LL411: _temp410=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*) _temp396)->f1;
goto _LL401;} else{ goto _LL402;} _LL402: if((( struct _enum_struct*) _temp396)->tag
== Cyc_Absyn_Xenum_d_tag){ _LL413: _temp412=( struct Cyc_Absyn_Xenumdecl*)((
struct Cyc_Absyn_Xenum_d_struct*) _temp396)->f1; goto _LL403;} else{ goto _LL404;}
_LL404: if((( struct _enum_struct*) _temp396)->tag == Cyc_Absyn_Union_d_tag){
_LL415: _temp414=( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*)
_temp396)->f1; goto _LL405;} else{ goto _LL406;} _LL406: goto _LL407; _LL399: {
struct Cyc_List_List* args=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp408->tvs); t=({
struct Cyc_Absyn_StructType_struct* _temp416=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp416->tag= Cyc_Absyn_StructType_tag;
_temp416->f1=( struct _tuple1*)(( struct _tuple1*)( _temp408->name)->v);
_temp416->f2= args; _temp416->f3= 0;( void*) _temp416;}); if( _temp408->fields
!= 0){ declopt=({ struct Cyc_Core_Opt* _temp417=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp417->v=( void*) _temp376;
_temp417;});} goto _LL397;} _LL401: { struct Cyc_List_List* args=(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Parse_tvar2typ, _temp410->tvs); t=({ struct Cyc_Absyn_EnumType_struct*
_temp418=( struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp418->tag= Cyc_Absyn_EnumType_tag; _temp418->f1=( struct _tuple1*)(( struct
_tuple1*)( _temp410->name)->v); _temp418->f2= args; _temp418->f3= 0;( void*)
_temp418;}); if( _temp410->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp419=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp419->v=( void*) _temp376; _temp419;});} goto _LL397;} _LL403: t=({ struct
Cyc_Absyn_XenumType_struct* _temp420=( struct Cyc_Absyn_XenumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XenumType_struct)); _temp420->tag= Cyc_Absyn_XenumType_tag;
_temp420->f1= _temp412->name; _temp420->f2= 0;( void*) _temp420;}); if( _temp412->fields
!= 0){ declopt=({ struct Cyc_Core_Opt* _temp421=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp421->v=( void*) _temp376;
_temp421;});} goto _LL397; _LL405: { struct Cyc_List_List* args=(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Parse_tvar2typ, _temp414->tvs); t=({ struct Cyc_Absyn_UnionType_struct*
_temp422=( struct Cyc_Absyn_UnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp422->tag= Cyc_Absyn_UnionType_tag; _temp422->f1=( struct _tuple1*)(( struct
_tuple1*)( _temp414->name)->v); _temp422->f2= args; _temp422->f3= 0;( void*)
_temp422;}); if( _temp414->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp423=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp423->v=( void*) _temp376; _temp423;});} goto _LL397;} _LL407:(( void(*)(
struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_string)({ char* _temp424=( char*)"bad declaration within type specifier";
struct _tagged_string _temp425; _temp425.curr= _temp424; _temp425.base= _temp424;
_temp425.last_plus_one= _temp424 + 38; _temp425;}), _temp376->loc); goto _LL397;
_LL397:;}} else{ Cyc_Parse_err( Cyc_Parse_msg3, _temp376->loc);} goto _LL351;
_LL351:;} if( ! seen_type){ if( ! seen_sign? ! seen_size: 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp426=( char*)"missing type withing specifier";
struct _tagged_string _temp427; _temp427.curr= _temp426; _temp427.base= _temp426;
_temp427.last_plus_one= _temp426 + 31; _temp427;}), last_loc);} t=({ struct Cyc_Absyn_IntType_struct*
_temp428=( struct Cyc_Absyn_IntType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp428->tag= Cyc_Absyn_IntType_tag; _temp428->f1=( void*) sgn; _temp428->f2=(
void*) sz;( void*) _temp428;});} else{ if( seen_sign){ void* _temp429= t; void*
_temp435; void* _temp437; _LL431: if(( unsigned int) _temp429 > 4u?(( struct
_enum_struct*) _temp429)->tag == Cyc_Absyn_IntType_tag: 0){ _LL438: _temp437=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp429)->f1; goto _LL436; _LL436:
_temp435=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp429)->f2; goto _LL432;}
else{ goto _LL433;} _LL433: goto _LL434; _LL432: t=({ struct Cyc_Absyn_IntType_struct*
_temp439=( struct Cyc_Absyn_IntType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp439->tag= Cyc_Absyn_IntType_tag; _temp439->f1=( void*) sgn; _temp439->f2=(
void*) _temp435;( void*) _temp439;}); goto _LL430; _LL434: Cyc_Parse_err((
struct _tagged_string)({ char* _temp440=( char*)"sign specification on non-integral type";
struct _tagged_string _temp441; _temp441.curr= _temp440; _temp441.base= _temp440;
_temp441.last_plus_one= _temp440 + 40; _temp441;}), last_loc); goto _LL430;
_LL430:;} if( seen_size){ void* _temp442= t; void* _temp448; void* _temp450;
_LL444: if(( unsigned int) _temp442 > 4u?(( struct _enum_struct*) _temp442)->tag
== Cyc_Absyn_IntType_tag: 0){ _LL451: _temp450=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp442)->f1; goto _LL449; _LL449: _temp448=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp442)->f2; goto _LL445;} else{ goto _LL446;} _LL446: goto _LL447; _LL445: t=({
struct Cyc_Absyn_IntType_struct* _temp452=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp452->tag= Cyc_Absyn_IntType_tag;
_temp452->f1=( void*) _temp450; _temp452->f2=( void*) sz;( void*) _temp452;});
goto _LL443; _LL447: Cyc_Parse_err(( struct _tagged_string)({ char* _temp453=(
char*)"size qualifier on non-integral type"; struct _tagged_string _temp454;
_temp454.curr= _temp453; _temp454.base= _temp453; _temp454.last_plus_one=
_temp453 + 36; _temp454;}), last_loc); goto _LL443; _LL443:;}} return({ struct
_tuple5 _temp455; _temp455.f1= t; _temp455.f2= declopt; _temp455;});} static
struct Cyc_List_List* Cyc_Parse_apply_tmss( struct Cyc_Absyn_Tqual* tq, void* t,
struct Cyc_List_List* ds, struct Cyc_List_List* shared_atts){ if( ds == 0){
return 0;}{ struct Cyc_Parse_Declarator* d=( struct Cyc_Parse_Declarator*) ds->hd;
struct _tuple1* q= d->id; struct Cyc_List_List* _temp458; struct Cyc_List_List*
_temp460; void* _temp462; struct Cyc_Absyn_Tqual* _temp464; struct _tuple6
_temp456= Cyc_Parse_apply_tms( tq, t, shared_atts, d->tms); _LL465: _temp464=
_temp456.f1; goto _LL463; _LL463: _temp462= _temp456.f2; goto _LL461; _LL461:
_temp460= _temp456.f3; goto _LL459; _LL459: _temp458= _temp456.f4; goto _LL457;
_LL457: return({ struct Cyc_List_List* _temp466=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp466->hd=( void*)({ struct
_tuple7* _temp467=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
_temp467->f1= q; _temp467->f2= _temp464; _temp467->f3= _temp462; _temp467->f4=
_temp460; _temp467->f5= _temp458; _temp467;}); _temp466->tl= Cyc_Parse_apply_tmss(
_temp464, t, ds->tl, shared_atts); _temp466;});}} static int Cyc_Parse_fn_type_att(
void* a){ void* _temp468= a; int _temp482; _LL470: if(( unsigned int) _temp468 >
15u?(( struct _enum_struct*) _temp468)->tag == Cyc_Absyn_Regparm_att_tag: 0){
_LL483: _temp482=( int)(( struct Cyc_Absyn_Regparm_att_struct*) _temp468)->f1;
goto _LL471;} else{ goto _LL472;} _LL472: if( _temp468 == Cyc_Absyn_Stdcall_att){
goto _LL473;} else{ goto _LL474;} _LL474: if( _temp468 == Cyc_Absyn_Cdecl_att){
goto _LL475;} else{ goto _LL476;} _LL476: if( _temp468 == Cyc_Absyn_Noreturn_att){
goto _LL477;} else{ goto _LL478;} _LL478: if( _temp468 == Cyc_Absyn_Const_att){
goto _LL479;} else{ goto _LL480;} _LL480: goto _LL481; _LL471: goto _LL473;
_LL473: goto _LL475; _LL475: goto _LL477; _LL477: goto _LL479; _LL479: return 1;
_LL481: return 0; _LL469:;} static struct _tuple6 Cyc_Parse_apply_tms( struct
Cyc_Absyn_Tqual* tq, void* t, struct Cyc_List_List* atts, struct Cyc_List_List*
tms){ if( tms == 0){ return({ struct _tuple6 _temp484; _temp484.f1= tq; _temp484.f2=
t; _temp484.f3= 0; _temp484.f4= atts; _temp484;});}{ void* _temp485=( void*) tms->hd;
struct Cyc_Absyn_Exp* _temp499; void* _temp501; int _temp503; struct Cyc_Position_Segment*
_temp505; struct Cyc_List_List* _temp507; struct Cyc_Absyn_Tqual* _temp509; void*
_temp511; void* _temp513; struct Cyc_List_List* _temp515; struct Cyc_Position_Segment*
_temp517; _LL487: if( _temp485 == Cyc_Absyn_Carray_mod){ goto _LL488;} else{
goto _LL489;} _LL489: if(( unsigned int) _temp485 > 1u?(( struct _enum_struct*)
_temp485)->tag == Cyc_Absyn_ConstArray_mod_tag: 0){ _LL500: _temp499=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ConstArray_mod_struct*) _temp485)->f1; goto
_LL490;} else{ goto _LL491;} _LL491: if(( unsigned int) _temp485 > 1u?(( struct
_enum_struct*) _temp485)->tag == Cyc_Absyn_Function_mod_tag: 0){ _LL502:
_temp501=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp485)->f1; goto
_LL492;} else{ goto _LL493;} _LL493: if(( unsigned int) _temp485 > 1u?(( struct
_enum_struct*) _temp485)->tag == Cyc_Absyn_TypeParams_mod_tag: 0){ _LL508:
_temp507=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp485)->f1; goto _LL506; _LL506: _temp505=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp485)->f2; goto _LL504; _LL504:
_temp503=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp485)->f3; goto
_LL494;} else{ goto _LL495;} _LL495: if(( unsigned int) _temp485 > 1u?(( struct
_enum_struct*) _temp485)->tag == Cyc_Absyn_Pointer_mod_tag: 0){ _LL514: _temp513=(
void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp485)->f1; goto _LL512;
_LL512: _temp511=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp485)->f2;
goto _LL510; _LL510: _temp509=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp485)->f3; goto _LL496;} else{ goto _LL497;} _LL497: if(( unsigned int)
_temp485 > 1u?(( struct _enum_struct*) _temp485)->tag == Cyc_Absyn_Attributes_mod_tag:
0){ _LL518: _temp517=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp485)->f1; goto _LL516; _LL516: _temp515=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Attributes_mod_struct*) _temp485)->f2; goto _LL498;} else{ goto _LL486;}
_LL488: return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(),({ struct Cyc_Absyn_ArrayType_struct*
_temp519=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp519->tag= Cyc_Absyn_ArrayType_tag; _temp519->f1=( void*) t; _temp519->f2=
tq; _temp519->f3= 0;( void*) _temp519;}), atts, tms->tl); _LL490: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(),({ struct Cyc_Absyn_ArrayType_struct* _temp520=( struct
Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp520->tag= Cyc_Absyn_ArrayType_tag; _temp520->f1=( void*) t; _temp520->f2=
tq; _temp520->f3=( struct Cyc_Absyn_Exp*) _temp499;( void*) _temp520;}), atts,
tms->tl); _LL492: { void* _temp521= _temp501; struct Cyc_Core_Opt* _temp527; int
_temp529; struct Cyc_List_List* _temp531; struct Cyc_Position_Segment* _temp533;
struct Cyc_List_List* _temp535; _LL523: if((( struct _enum_struct*) _temp521)->tag
== Cyc_Absyn_WithTypes_tag){ _LL532: _temp531=( struct Cyc_List_List*)(( struct
Cyc_Absyn_WithTypes_struct*) _temp521)->f1; goto _LL530; _LL530: _temp529=( int)((
struct Cyc_Absyn_WithTypes_struct*) _temp521)->f2; goto _LL528; _LL528: _temp527=(
struct Cyc_Core_Opt*)(( struct Cyc_Absyn_WithTypes_struct*) _temp521)->f3; goto
_LL524;} else{ goto _LL525;} _LL525: if((( struct _enum_struct*) _temp521)->tag
== Cyc_Absyn_NoTypes_tag){ _LL536: _temp535=( struct Cyc_List_List*)(( struct
Cyc_Absyn_NoTypes_struct*) _temp521)->f1; goto _LL534; _LL534: _temp533=( struct
Cyc_Position_Segment*)(( struct Cyc_Absyn_NoTypes_struct*) _temp521)->f2; goto
_LL526;} else{ goto _LL522;} _LL524: { struct Cyc_List_List* typvars= 0; struct
Cyc_List_List* fn_atts= 0; struct Cyc_List_List* new_atts= 0;{ struct Cyc_List_List*
as= atts; for( 0; as != 0; as= as->tl){ if( Cyc_Parse_fn_type_att(( void*) as->hd)){
fn_atts=({ struct Cyc_List_List* _temp537=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp537->hd=( void*)(( void*) as->hd); _temp537->tl=
fn_atts; _temp537;});} else{ new_atts=({ struct Cyc_List_List* _temp538=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp538->hd=( void*)((
void*) as->hd); _temp538->tl= new_atts; _temp538;});}}} if( tms->tl != 0){ void*
_temp539=( void*)( tms->tl)->hd; int _temp545; struct Cyc_Position_Segment*
_temp547; struct Cyc_List_List* _temp549; _LL541: if(( unsigned int) _temp539 >
1u?(( struct _enum_struct*) _temp539)->tag == Cyc_Absyn_TypeParams_mod_tag: 0){
_LL550: _temp549=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp539)->f1; goto _LL548; _LL548: _temp547=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp539)->f2; goto _LL546; _LL546:
_temp545=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp539)->f3; goto
_LL542;} else{ goto _LL543;} _LL543: goto _LL544; _LL542: typvars= _temp549; tms=
tms->tl; goto _LL540; _LL544: goto _LL540; _LL540:;} if(((( ! _temp529? _temp531
!= 0: 0)? _temp531->tl == 0: 0)?(*(( struct _tuple2*) _temp531->hd)).f1 == 0: 0)?(*((
struct _tuple2*) _temp531->hd)).f3 == Cyc_Absyn_VoidType: 0){ _temp531= 0;
_temp529= 0;} t= Cyc_Parse_array2ptr( t);(( void(*)( void(* f)( struct _tuple2*),
struct Cyc_List_List* x)) Cyc_List_iter)( Cyc_Parse_arg_array2ptr, _temp531);
return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(), Cyc_Absyn_function_typ(
typvars, _temp527, t, _temp531, _temp529, fn_atts), new_atts, tms->tl);} _LL526:(
void) _throw((( struct _xenum_struct*(*)( struct _tagged_string msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)(( struct _tagged_string)({ char* _temp551=( char*)"function declaration without parameter types";
struct _tagged_string _temp552; _temp552.curr= _temp551; _temp552.base= _temp551;
_temp552.last_plus_one= _temp551 + 45; _temp552;}), _temp533)); _LL522:;} _LL494:
if( tms->tl == 0){ return({ struct _tuple6 _temp553; _temp553.f1= tq; _temp553.f2=
t; _temp553.f3= _temp507; _temp553.f4= atts; _temp553;});}( void) _throw(((
struct _xenum_struct*(*)( struct _tagged_string msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)(( struct _tagged_string)({ char* _temp554=( char*)"type parameters must appear before function arguments in declarator";
struct _tagged_string _temp555; _temp555.curr= _temp554; _temp555.base= _temp554;
_temp555.last_plus_one= _temp554 + 68; _temp555;}), _temp505)); _LL496: { void*
_temp556= _temp513; struct Cyc_Absyn_Exp* _temp564; struct Cyc_Absyn_Exp*
_temp566; _LL558: if(( unsigned int) _temp556 > 1u?(( struct _enum_struct*)
_temp556)->tag == Cyc_Absyn_NonNullable_ps_tag: 0){ _LL565: _temp564=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp556)->f1; goto
_LL559;} else{ goto _LL560;} _LL560: if(( unsigned int) _temp556 > 1u?(( struct
_enum_struct*) _temp556)->tag == Cyc_Absyn_Nullable_ps_tag: 0){ _LL567: _temp566=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Nullable_ps_struct*) _temp556)->f1;
goto _LL561;} else{ goto _LL562;} _LL562: if( _temp556 == Cyc_Absyn_TaggedArray_ps){
goto _LL563;} else{ goto _LL557;} _LL559: return Cyc_Parse_apply_tms( _temp509,
Cyc_Absyn_atb_typ( t, _temp511, tq,({ struct Cyc_Absyn_Upper_b_struct* _temp568=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp568->tag= Cyc_Absyn_Upper_b_tag; _temp568->f1= _temp564;( void*) _temp568;})),
atts, tms->tl); _LL561: return Cyc_Parse_apply_tms( _temp509, Cyc_Absyn_starb_typ(
t, _temp511, tq,({ struct Cyc_Absyn_Upper_b_struct* _temp569=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp569->tag= Cyc_Absyn_Upper_b_tag;
_temp569->f1= _temp566;( void*) _temp569;})), atts, tms->tl); _LL563: return Cyc_Parse_apply_tms(
_temp509, Cyc_Absyn_tagged_typ( t, _temp511, tq), atts, tms->tl); _LL557:;}
_LL498: return Cyc_Parse_apply_tms( tq, t,(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( atts, _temp515),
tms->tl); _LL486:;}} void* Cyc_Parse_speclist2typ( struct Cyc_List_List* tss,
struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt* _temp572; void* _temp574;
struct _tuple5 _temp570= Cyc_Parse_collapse_type_specifiers( tss, loc); _LL575:
_temp574= _temp570.f1; goto _LL573; _LL573: _temp572= _temp570.f2; goto _LL571;
_LL571: if( _temp572 != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char*
_temp576=( char*)"ignoring nested type declaration(s) in specifier list"; struct
_tagged_string _temp577; _temp577.curr= _temp576; _temp577.base= _temp576;
_temp577.last_plus_one= _temp576 + 54; _temp577;}), loc);} return _temp574;}
static struct Cyc_Absyn_Stmt* Cyc_Parse_flatten_decl( struct Cyc_Absyn_Decl* d,
struct Cyc_Absyn_Stmt* s){ return Cyc_Absyn_new_stmt(({ struct Cyc_Absyn_Decl_s_struct*
_temp578=( struct Cyc_Absyn_Decl_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
_temp578->tag= Cyc_Absyn_Decl_s_tag; _temp578->f1= d; _temp578->f2= s;( void*)
_temp578;}), Cyc_Position_segment_join( d->loc, s->loc));} static struct Cyc_Absyn_Stmt*
Cyc_Parse_flatten_declarations( struct Cyc_List_List* ds, struct Cyc_Absyn_Stmt*
s){ return(( struct Cyc_Absyn_Stmt*(*)( struct Cyc_Absyn_Stmt*(* f)( struct Cyc_Absyn_Decl*,
struct Cyc_Absyn_Stmt*), struct Cyc_List_List* x, struct Cyc_Absyn_Stmt* accum))
Cyc_List_fold_right)( Cyc_Parse_flatten_decl, ds, s);} static struct Cyc_List_List*
Cyc_Parse_make_declarations( struct Cyc_Parse_Declaration_spec* ds, struct Cyc_List_List*
ids, struct Cyc_Position_Segment* loc){ struct Cyc_List_List* tss= ds->type_specs;
struct Cyc_Absyn_Tqual* tq= ds->tq; int istypedef= 0; void* s= Cyc_Absyn_Public;
struct Cyc_List_List* atts= ds->attributes; if( ds->is_inline){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp579=( char*)"inline is only allowed on function definitions";
struct _tagged_string _temp580; _temp580.curr= _temp579; _temp580.base= _temp579;
_temp580.last_plus_one= _temp579 + 47; _temp580;}), loc);} if( tss == 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp581=( char*)"missing type specifiers in declaration";
struct _tagged_string _temp582; _temp582.curr= _temp581; _temp582.base= _temp581;
_temp582.last_plus_one= _temp581 + 39; _temp582;}), loc); return 0;} if( ds->sc
!= 0){ void* _temp583=( void*)( ds->sc)->v; _LL585: if( _temp583 == Cyc_Parse_Typedef_sc){
goto _LL586;} else{ goto _LL587;} _LL587: if( _temp583 == Cyc_Parse_Extern_sc){
goto _LL588;} else{ goto _LL589;} _LL589: if( _temp583 == Cyc_Parse_ExternC_sc){
goto _LL590;} else{ goto _LL591;} _LL591: if( _temp583 == Cyc_Parse_Static_sc){
goto _LL592;} else{ goto _LL593;} _LL593: if( _temp583 == Cyc_Parse_Auto_sc){
goto _LL594;} else{ goto _LL595;} _LL595: if( _temp583 == Cyc_Parse_Register_sc){
goto _LL596;} else{ goto _LL597;} _LL597: if( _temp583 == Cyc_Parse_Abstract_sc){
goto _LL598;} else{ goto _LL584;} _LL586: istypedef= 1; goto _LL584; _LL588: s=
Cyc_Absyn_Extern; goto _LL584; _LL590: s= Cyc_Absyn_ExternC; goto _LL584; _LL592:
s= Cyc_Absyn_Static; goto _LL584; _LL594: s= Cyc_Absyn_Public; goto _LL584;
_LL596: s= Cyc_Absyn_Public; goto _LL584; _LL598: s= Cyc_Absyn_Abstract; goto
_LL584; _LL584:;}{ struct Cyc_List_List* _temp601; struct Cyc_List_List*
_temp603; struct _tuple0 _temp599=(( struct _tuple0(*)( struct Cyc_List_List* x))
Cyc_List_split)( ids); _LL604: _temp603= _temp599.f1; goto _LL602; _LL602:
_temp601= _temp599.f2; goto _LL600; _LL600: { int exps_empty= 1;{ struct Cyc_List_List*
es= _temp601; for( 0; es != 0; es= es->tl){ if(( struct Cyc_Absyn_Exp*) es->hd
!= 0){ exps_empty= 0; break;}}}{ struct _tuple5 ts_info= Cyc_Parse_collapse_type_specifiers(
tss, loc); if( _temp603 == 0){ struct Cyc_Core_Opt* _temp607; void* _temp609;
struct _tuple5 _temp605= ts_info; _LL610: _temp609= _temp605.f1; goto _LL608;
_LL608: _temp607= _temp605.f2; goto _LL606; _LL606: if( _temp607 != 0){ struct
Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*) _temp607->v;{ void* _temp611=( void*)
d->r; struct Cyc_Absyn_Structdecl* _temp623; struct Cyc_Absyn_Uniondecl*
_temp625; struct Cyc_Absyn_Enumdecl* _temp627; struct Cyc_Absyn_Xenumdecl*
_temp629; _LL613: if((( struct _enum_struct*) _temp611)->tag == Cyc_Absyn_Struct_d_tag){
_LL624: _temp623=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*)
_temp611)->f1; goto _LL614;} else{ goto _LL615;} _LL615: if((( struct
_enum_struct*) _temp611)->tag == Cyc_Absyn_Union_d_tag){ _LL626: _temp625=(
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*) _temp611)->f1;
goto _LL616;} else{ goto _LL617;} _LL617: if((( struct _enum_struct*) _temp611)->tag
== Cyc_Absyn_Enum_d_tag){ _LL628: _temp627=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_d_struct*) _temp611)->f1; goto _LL618;} else{ goto _LL619;}
_LL619: if((( struct _enum_struct*) _temp611)->tag == Cyc_Absyn_Xenum_d_tag){
_LL630: _temp629=( struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_d_struct*)
_temp611)->f1; goto _LL620;} else{ goto _LL621;} _LL621: goto _LL622; _LL614:(
void*)( _temp623->sc=( void*) s); _temp623->attributes= atts; goto _LL612;
_LL616:( void*)( _temp625->sc=( void*) s); _temp625->attributes= atts; goto
_LL612; _LL618:( void*)( _temp627->sc=( void*) s); if( atts != 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp631=( char*)"bad attributes on enum"; struct
_tagged_string _temp632; _temp632.curr= _temp631; _temp632.base= _temp631;
_temp632.last_plus_one= _temp631 + 23; _temp632;}), loc);} goto _LL612; _LL620:(
void*)( _temp629->sc=( void*) s); if( atts != 0){ Cyc_Parse_err(( struct
_tagged_string)({ char* _temp633=( char*)"bad attributes on xenum"; struct
_tagged_string _temp634; _temp634.curr= _temp633; _temp634.base= _temp633;
_temp634.last_plus_one= _temp633 + 24; _temp634;}), loc);} goto _LL612; _LL622:
Cyc_Parse_err(( struct _tagged_string)({ char* _temp635=( char*)"bad declaration";
struct _tagged_string _temp636; _temp636.curr= _temp635; _temp636.base= _temp635;
_temp636.last_plus_one= _temp635 + 16; _temp636;}), loc); return 0; _LL612:;}
return({ struct Cyc_List_List* _temp637=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp637->hd=( void*) d; _temp637->tl= 0;
_temp637;});} else{ void* _temp638= _temp609; struct Cyc_Absyn_Structdecl**
_temp650; struct Cyc_List_List* _temp652; struct _tuple1* _temp654; struct Cyc_Absyn_Enumdecl**
_temp656; struct Cyc_List_List* _temp658; struct _tuple1* _temp660; struct Cyc_Absyn_Xenumdecl**
_temp662; struct _tuple1* _temp664; struct Cyc_Absyn_Uniondecl** _temp666;
struct Cyc_List_List* _temp668; struct _tuple1* _temp670; _LL640: if((
unsigned int) _temp638 > 4u?(( struct _enum_struct*) _temp638)->tag == Cyc_Absyn_StructType_tag:
0){ _LL655: _temp654=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp638)->f1; goto _LL653; _LL653: _temp652=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp638)->f2; goto _LL651; _LL651: _temp650=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp638)->f3;
goto _LL641;} else{ goto _LL642;} _LL642: if(( unsigned int) _temp638 > 4u?((
struct _enum_struct*) _temp638)->tag == Cyc_Absyn_EnumType_tag: 0){ _LL661:
_temp660=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*) _temp638)->f1;
goto _LL659; _LL659: _temp658=( struct Cyc_List_List*)(( struct Cyc_Absyn_EnumType_struct*)
_temp638)->f2; goto _LL657; _LL657: _temp656=( struct Cyc_Absyn_Enumdecl**)((
struct Cyc_Absyn_EnumType_struct*) _temp638)->f3; goto _LL643;} else{ goto
_LL644;} _LL644: if(( unsigned int) _temp638 > 4u?(( struct _enum_struct*)
_temp638)->tag == Cyc_Absyn_XenumType_tag: 0){ _LL665: _temp664=( struct _tuple1*)((
struct Cyc_Absyn_XenumType_struct*) _temp638)->f1; goto _LL663; _LL663: _temp662=(
struct Cyc_Absyn_Xenumdecl**)(( struct Cyc_Absyn_XenumType_struct*) _temp638)->f2;
goto _LL645;} else{ goto _LL646;} _LL646: if(( unsigned int) _temp638 > 4u?((
struct _enum_struct*) _temp638)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL671:
_temp670=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*) _temp638)->f1;
goto _LL669; _LL669: _temp668=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp638)->f2; goto _LL667; _LL667: _temp666=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp638)->f3; goto _LL647;} else{ goto
_LL648;} _LL648: goto _LL649; _LL641: { struct Cyc_List_List* ts2=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc,
_temp652); struct Cyc_Absyn_Structdecl* sd=({ struct Cyc_Absyn_Structdecl*
_temp672=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp672->sc=( void*) s; _temp672->name=({ struct Cyc_Core_Opt* _temp673=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp673->v=(
void*)(( struct _tuple1*)({ struct _tuple1* _temp674= _temp654; if( _temp674 ==
0){ _throw( Null_Exception);} _temp674;})); _temp673;}); _temp672->tvs= ts2;
_temp672->fields= 0; _temp672->attributes= 0; _temp672;}); if( atts != 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp675=( char*)"bad attributes on struct";
struct _tagged_string _temp676; _temp676.curr= _temp675; _temp676.base= _temp675;
_temp676.last_plus_one= _temp675 + 25; _temp676;}), loc);} return({ struct Cyc_List_List*
_temp677=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp677->hd=( void*)({ struct Cyc_Absyn_Decl* _temp678=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp678->r=( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp679=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp679->tag= Cyc_Absyn_Struct_d_tag; _temp679->f1= sd;( void*) _temp679;});
_temp678->loc= loc; _temp678;}); _temp677->tl= 0; _temp677;});} _LL643: { struct
Cyc_List_List* ts2=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc, _temp658); struct
Cyc_Absyn_Decl* ed= Cyc_Absyn_enum_decl( s,({ struct Cyc_Core_Opt* _temp680=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp680->v=(
void*)(( struct _tuple1*)({ struct _tuple1* _temp681= _temp660; if( _temp681 ==
0){ _throw( Null_Exception);} _temp681;})); _temp680;}), ts2, 0, loc); if( atts
!= 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp682=( char*)"bad attributes on enum";
struct _tagged_string _temp683; _temp683.curr= _temp682; _temp683.base= _temp682;
_temp683.last_plus_one= _temp682 + 23; _temp683;}), loc);} return({ struct Cyc_List_List*
_temp684=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp684->hd=( void*) ed; _temp684->tl= 0; _temp684;});} _LL645: { struct Cyc_Absyn_Xenumdecl*
ed=({ struct Cyc_Absyn_Xenumdecl* _temp685=( struct Cyc_Absyn_Xenumdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Xenumdecl)); _temp685->sc=( void*) s;
_temp685->name= _temp664; _temp685->fields= 0; _temp685;}); if( atts != 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp686=( char*)"bad attributes on xenum";
struct _tagged_string _temp687; _temp687.curr= _temp686; _temp687.base= _temp686;
_temp687.last_plus_one= _temp686 + 24; _temp687;}), loc);} return({ struct Cyc_List_List*
_temp688=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp688->hd=( void*)({ struct Cyc_Absyn_Decl* _temp689=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp689->r=( void*)({ struct Cyc_Absyn_Xenum_d_struct*
_temp690=( struct Cyc_Absyn_Xenum_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Xenum_d_struct));
_temp690->tag= Cyc_Absyn_Xenum_d_tag; _temp690->f1= ed;( void*) _temp690;});
_temp689->loc= loc; _temp689;}); _temp688->tl= 0; _temp688;});} _LL647: { struct
Cyc_List_List* ts2=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc, _temp668); struct
Cyc_Absyn_Uniondecl* ud=({ struct Cyc_Absyn_Uniondecl* _temp691=( struct Cyc_Absyn_Uniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl)); _temp691->sc=( void*) s;
_temp691->name=({ struct Cyc_Core_Opt* _temp692=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp692->v=( void*)(( struct _tuple1*)({
struct _tuple1* _temp693= _temp670; if( _temp693 == 0){ _throw( Null_Exception);}
_temp693;})); _temp692;}); _temp691->tvs= ts2; _temp691->fields= 0; _temp691->attributes=
0; _temp691;}); if( atts != 0){ Cyc_Parse_err(( struct _tagged_string)({ char*
_temp694=( char*)"bad attributes on union"; struct _tagged_string _temp695;
_temp695.curr= _temp694; _temp695.base= _temp694; _temp695.last_plus_one=
_temp694 + 24; _temp695;}), loc);} return({ struct Cyc_List_List* _temp696=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp696->hd=(
void*)({ struct Cyc_Absyn_Decl* _temp697=( struct Cyc_Absyn_Decl*) GC_malloc(
sizeof( struct Cyc_Absyn_Decl)); _temp697->r=( void*)({ struct Cyc_Absyn_Union_d_struct*
_temp698=( struct Cyc_Absyn_Union_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp698->tag= Cyc_Absyn_Union_d_tag; _temp698->f1= ud;( void*) _temp698;});
_temp697->loc= loc; _temp697;}); _temp696->tl= 0; _temp696;});} _LL649: Cyc_Parse_err((
struct _tagged_string)({ char* _temp699=( char*)"missing declarator"; struct
_tagged_string _temp700; _temp700.curr= _temp699; _temp700.base= _temp699;
_temp700.last_plus_one= _temp699 + 19; _temp700;}), loc); return 0; _LL639:;}}
else{ void* t= ts_info.f1; struct Cyc_List_List* fields= Cyc_Parse_apply_tmss(
tq, t, _temp603, atts); if( istypedef){ if( ! exps_empty){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp701=( char*)"initializer in typedef declaration";
struct _tagged_string _temp702; _temp702.curr= _temp701; _temp702.base= _temp701;
_temp702.last_plus_one= _temp701 + 35; _temp702;}), loc);}{ struct Cyc_List_List*
decls=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Decl*(* f)( struct Cyc_Position_Segment*,
struct _tuple7*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_v_typ_to_typedef, loc, fields); if( ts_info.f2 != 0){
struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)( ts_info.f2)->v;{ void*
_temp703=( void*) d->r; struct Cyc_Absyn_Structdecl* _temp715; struct Cyc_Absyn_Enumdecl*
_temp717; struct Cyc_Absyn_Xenumdecl* _temp719; struct Cyc_Absyn_Uniondecl*
_temp721; _LL705: if((( struct _enum_struct*) _temp703)->tag == Cyc_Absyn_Struct_d_tag){
_LL716: _temp715=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*)
_temp703)->f1; goto _LL706;} else{ goto _LL707;} _LL707: if((( struct
_enum_struct*) _temp703)->tag == Cyc_Absyn_Enum_d_tag){ _LL718: _temp717=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*) _temp703)->f1;
goto _LL708;} else{ goto _LL709;} _LL709: if((( struct _enum_struct*) _temp703)->tag
== Cyc_Absyn_Xenum_d_tag){ _LL720: _temp719=( struct Cyc_Absyn_Xenumdecl*)((
struct Cyc_Absyn_Xenum_d_struct*) _temp703)->f1; goto _LL710;} else{ goto _LL711;}
_LL711: if((( struct _enum_struct*) _temp703)->tag == Cyc_Absyn_Union_d_tag){
_LL722: _temp721=( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*)
_temp703)->f1; goto _LL712;} else{ goto _LL713;} _LL713: goto _LL714; _LL706:(
void*)( _temp715->sc=( void*) s); _temp715->attributes= atts; atts= 0; goto
_LL704; _LL708:( void*)( _temp717->sc=( void*) s); goto _LL704; _LL710:( void*)(
_temp719->sc=( void*) s); goto _LL704; _LL712:( void*)( _temp721->sc=( void*) s);
goto _LL704; _LL714: Cyc_Parse_err(( struct _tagged_string)({ char* _temp723=(
char*)"declaration within typedef is not a struct, union, enum, or xenum";
struct _tagged_string _temp724; _temp724.curr= _temp723; _temp724.base= _temp723;
_temp724.last_plus_one= _temp723 + 66; _temp724;}), loc); goto _LL704; _LL704:;}
decls=({ struct Cyc_List_List* _temp725=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp725->hd=( void*) d; _temp725->tl= decls;
_temp725;});} if( atts != 0){ Cyc_Parse_err(({ struct _tagged_string _temp726=
Cyc_Absyn_attribute2string(( void*) atts->hd); xprintf("bad attribute %.*s in typedef",
_temp726.last_plus_one - _temp726.curr, _temp726.curr);}), loc);} return decls;}}
else{ if( ts_info.f2 != 0){ Cyc_Parse_unimp2(( struct _tagged_string)({ char*
_temp727=( char*)"nested type declaration within declarator"; struct
_tagged_string _temp728; _temp728.curr= _temp727; _temp728.base= _temp727;
_temp728.last_plus_one= _temp727 + 42; _temp728;}), loc);}{ struct Cyc_List_List*
decls= 0;{ struct Cyc_List_List* ds= fields; for( 0; ds != 0; ds= ds->tl,
_temp601= _temp601->tl){ struct _tuple7 _temp731; struct Cyc_List_List* _temp732;
struct Cyc_List_List* _temp734; void* _temp736; struct Cyc_Absyn_Tqual* _temp738;
struct _tuple1* _temp740; struct _tuple7* _temp729=( struct _tuple7*) ds->hd;
_temp731=* _temp729; _LL741: _temp740= _temp731.f1; goto _LL739; _LL739:
_temp738= _temp731.f2; goto _LL737; _LL737: _temp736= _temp731.f3; goto _LL735;
_LL735: _temp734= _temp731.f4; goto _LL733; _LL733: _temp732= _temp731.f5; goto
_LL730; _LL730: if( _temp734 != 0){ Cyc_Parse_warn(( struct _tagged_string)({
char* _temp742=( char*)"bad type params, ignoring"; struct _tagged_string
_temp743; _temp743.curr= _temp742; _temp743.base= _temp742; _temp743.last_plus_one=
_temp742 + 26; _temp743;}), loc);} if( _temp601 == 0){(( void(*)( struct
_tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct
_tagged_string)({ char* _temp744=( char*)"unexpected null in parse!"; struct
_tagged_string _temp745; _temp745.curr= _temp744; _temp745.base= _temp744;
_temp745.last_plus_one= _temp744 + 26; _temp745;}), loc);}{ struct Cyc_Absyn_Exp*
eopt=( struct Cyc_Absyn_Exp*) _temp601->hd; struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_new_vardecl(
_temp740, _temp736, eopt); vd->tq= _temp738;( void*)( vd->sc=( void*) s); vd->attributes=
_temp732;{ struct Cyc_Absyn_Decl* d=({ struct Cyc_Absyn_Decl* _temp746=( struct
Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp746->r=( void*)({
struct Cyc_Absyn_Var_d_struct* _temp747=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp747->tag= Cyc_Absyn_Var_d_tag;
_temp747->f1= vd;( void*) _temp747;}); _temp746->loc= loc; _temp746;}); decls=({
struct Cyc_List_List* _temp748=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp748->hd=( void*) d; _temp748->tl= decls; _temp748;});}}}}
return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
decls);}}}}}}} static void* Cyc_Parse_id_to_kind( struct _tagged_string s,
struct Cyc_Position_Segment* loc){ if( Cyc_String_strlen( s) !=( unsigned int) 1){
Cyc_Parse_err(({ struct _tagged_string _temp749= s; xprintf("bad kind: %.*s",
_temp749.last_plus_one - _temp749.curr, _temp749.curr);}), loc); return Cyc_Absyn_BoxKind;}
else{ switch(({ struct _tagged_string _temp750= s; char* _temp752= _temp750.curr
+ 0; if( _temp752 < _temp750.base? 1: _temp752 >= _temp750.last_plus_one){
_throw( Null_Exception);}* _temp752;})){ case 'A': _LL753: return Cyc_Absyn_AnyKind;
case 'M': _LL754: return Cyc_Absyn_MemKind; case 'B': _LL755: return Cyc_Absyn_BoxKind;
case 'R': _LL756: return Cyc_Absyn_RgnKind; case 'E': _LL757: return Cyc_Absyn_EffKind;
default: _LL758: Cyc_Parse_err(({ struct _tagged_string _temp760= s; xprintf("bad kind: %.*s",
_temp760.last_plus_one - _temp760.curr, _temp760.curr);}), loc); return Cyc_Absyn_BoxKind;}}}
static struct Cyc_Absyn_Decl* Cyc_Parse_v_typ_to_typedef( struct Cyc_Position_Segment*
loc, struct _tuple7* t){ struct _tuple1* x=(* t).f1; Cyc_Lex_register_typedef( x);
if((* t).f5 != 0){ Cyc_Parse_err(({ struct _tagged_string _temp761= Cyc_Absyn_attribute2string((
void*)((* t).f5)->hd); xprintf("bad attribute %.*s within typedef", _temp761.last_plus_one
- _temp761.curr, _temp761.curr);}), loc);} return Cyc_Absyn_new_decl(({ struct
Cyc_Absyn_Typedef_d_struct* _temp762=( struct Cyc_Absyn_Typedef_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct)); _temp762->tag= Cyc_Absyn_Typedef_d_tag;
_temp762->f1=({ struct Cyc_Absyn_Typedefdecl* _temp763=( struct Cyc_Absyn_Typedefdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedefdecl)); _temp763->name= x; _temp763->tvs=(*
t).f4; _temp763->defn=( void*)(* t).f3; _temp763;});( void*) _temp762;}), loc);}
typedef struct _xenum_struct* Cyc_YYSTYPE; char Cyc_Okay_tok_tag[ 9u]="Okay_tok";
struct Cyc_Okay_tok_struct{ char* tag; } ; char Cyc_Int_tok_tag[ 8u]="Int_tok";
struct Cyc_Int_tok_struct{ char* tag; struct _tuple12* f1; } ; char Cyc_Char_tok_tag[
9u]="Char_tok"; struct Cyc_Char_tok_struct{ char* tag; char f1; } ; char Cyc_Pointer_Sort_tok_tag[
17u]="Pointer_Sort_tok"; struct Cyc_Pointer_Sort_tok_struct{ char* tag; void* f1;
} ; char Cyc_Short_tok_tag[ 10u]="Short_tok"; struct Cyc_Short_tok_struct{ char*
tag; short f1; } ; char Cyc_String_tok_tag[ 11u]="String_tok"; struct Cyc_String_tok_struct{
char* tag; struct _tagged_string f1; } ; char Cyc_Stringopt_tok_tag[ 14u]="Stringopt_tok";
struct Cyc_Stringopt_tok_struct{ char* tag; struct Cyc_Core_Opt* f1; } ; char
Cyc_Type_tok_tag[ 9u]="Type_tok"; struct Cyc_Type_tok_struct{ char* tag; void*
f1; } ; char Cyc_TypeList_tok_tag[ 13u]="TypeList_tok"; struct Cyc_TypeList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; char Cyc_Exp_tok_tag[ 8u]="Exp_tok";
struct Cyc_Exp_tok_struct{ char* tag; struct Cyc_Absyn_Exp* f1; } ; char Cyc_ExpList_tok_tag[
12u]="ExpList_tok"; struct Cyc_ExpList_tok_struct{ char* tag; struct Cyc_List_List*
f1; } ; char Cyc_Primop_tok_tag[ 11u]="Primop_tok"; struct Cyc_Primop_tok_struct{
char* tag; void* f1; } ; char Cyc_Primopopt_tok_tag[ 14u]="Primopopt_tok";
struct Cyc_Primopopt_tok_struct{ char* tag; struct Cyc_Core_Opt* f1; } ; char
Cyc_QualId_tok_tag[ 11u]="QualId_tok"; struct Cyc_QualId_tok_struct{ char* tag;
struct _tuple1* f1; } ; char Cyc_Stmt_tok_tag[ 9u]="Stmt_tok"; struct Cyc_Stmt_tok_struct{
char* tag; struct Cyc_Absyn_Stmt* f1; } ; char Cyc_SwitchClauseList_tok_tag[ 21u]="SwitchClauseList_tok";
struct Cyc_SwitchClauseList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ;
char Cyc_Pattern_tok_tag[ 12u]="Pattern_tok"; struct Cyc_Pattern_tok_struct{
char* tag; struct Cyc_Absyn_Pat* f1; } ; char Cyc_PatternList_tok_tag[ 16u]="PatternList_tok";
struct Cyc_PatternList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ; char
Cyc_FnDecl_tok_tag[ 11u]="FnDecl_tok"; struct Cyc_FnDecl_tok_struct{ char* tag;
struct Cyc_Absyn_Fndecl* f1; } ; char Cyc_DeclList_tok_tag[ 13u]="DeclList_tok";
struct Cyc_DeclList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ; char
Cyc_DeclSpec_tok_tag[ 13u]="DeclSpec_tok"; struct Cyc_DeclSpec_tok_struct{ char*
tag; struct Cyc_Parse_Declaration_spec* f1; } ; char Cyc_InitDecl_tok_tag[ 13u]="InitDecl_tok";
struct Cyc_InitDecl_tok_struct{ char* tag; struct _tuple13* f1; } ; char Cyc_InitDeclList_tok_tag[
17u]="InitDeclList_tok"; struct Cyc_InitDeclList_tok_struct{ char* tag; struct
Cyc_List_List* f1; } ; char Cyc_StorageClass_tok_tag[ 17u]="StorageClass_tok";
struct Cyc_StorageClass_tok_struct{ char* tag; void* f1; } ; char Cyc_TypeSpecifier_tok_tag[
18u]="TypeSpecifier_tok"; struct Cyc_TypeSpecifier_tok_struct{ char* tag; void*
f1; } ; char Cyc_QualSpecList_tok_tag[ 17u]="QualSpecList_tok"; struct Cyc_QualSpecList_tok_struct{
char* tag; struct _tuple14* f1; } ; char Cyc_TypeQual_tok_tag[ 13u]="TypeQual_tok";
struct Cyc_TypeQual_tok_struct{ char* tag; struct Cyc_Absyn_Tqual* f1; } ; char
Cyc_StructFieldDeclList_tok_tag[ 24u]="StructFieldDeclList_tok"; struct Cyc_StructFieldDeclList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; char Cyc_StructFieldDeclListList_tok_tag[
28u]="StructFieldDeclListList_tok"; struct Cyc_StructFieldDeclListList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; char Cyc_Declarator_tok_tag[ 15u]="Declarator_tok";
struct Cyc_Declarator_tok_struct{ char* tag; struct Cyc_Parse_Declarator* f1; }
; char Cyc_DeclaratorExpopt_tok_tag[ 21u]="DeclaratorExpopt_tok"; struct Cyc_DeclaratorExpopt_tok_struct{
char* tag; struct _tuple15* f1; } ; char Cyc_DeclaratorExpoptList_tok_tag[ 25u]="DeclaratorExpoptList_tok";
struct Cyc_DeclaratorExpoptList_tok_struct{ char* tag; struct Cyc_List_List* f1;
} ; char Cyc_AbstractDeclarator_tok_tag[ 23u]="AbstractDeclarator_tok"; struct
Cyc_AbstractDeclarator_tok_struct{ char* tag; struct Cyc_Parse_Abstractdeclarator*
f1; } ; char Cyc_EnumeratorField_tok_tag[ 20u]="EnumeratorField_tok"; struct Cyc_EnumeratorField_tok_struct{
char* tag; struct Cyc_Absyn_Enumfield* f1; } ; char Cyc_EnumeratorFieldList_tok_tag[
24u]="EnumeratorFieldList_tok"; struct Cyc_EnumeratorFieldList_tok_struct{ char*
tag; struct Cyc_List_List* f1; } ; char Cyc_ParamDecl_tok_tag[ 14u]="ParamDecl_tok";
struct Cyc_ParamDecl_tok_struct{ char* tag; struct _tuple2* f1; } ; char Cyc_ParamDeclList_tok_tag[
18u]="ParamDeclList_tok"; struct Cyc_ParamDeclList_tok_struct{ char* tag; struct
Cyc_List_List* f1; } ; char Cyc_ParamDeclListBool_tok_tag[ 22u]="ParamDeclListBool_tok";
struct Cyc_ParamDeclListBool_tok_struct{ char* tag; struct _tuple16* f1; } ;
char Cyc_StructOrUnion_tok_tag[ 18u]="StructOrUnion_tok"; struct Cyc_StructOrUnion_tok_struct{
char* tag; void* f1; } ; char Cyc_IdList_tok_tag[ 11u]="IdList_tok"; struct Cyc_IdList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; char Cyc_Designator_tok_tag[ 15u]="Designator_tok";
struct Cyc_Designator_tok_struct{ char* tag; void* f1; } ; char Cyc_DesignatorList_tok_tag[
19u]="DesignatorList_tok"; struct Cyc_DesignatorList_tok_struct{ char* tag;
struct Cyc_List_List* f1; } ; char Cyc_TypeModifierList_tok_tag[ 21u]="TypeModifierList_tok";
struct Cyc_TypeModifierList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ;
char Cyc_Rgn_tok_tag[ 8u]="Rgn_tok"; struct Cyc_Rgn_tok_struct{ char* tag; void*
f1; } ; char Cyc_InitializerList_tok_tag[ 20u]="InitializerList_tok"; struct Cyc_InitializerList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; char Cyc_FieldPattern_tok_tag[ 17u]="FieldPattern_tok";
struct Cyc_FieldPattern_tok_struct{ char* tag; struct _tuple17* f1; } ; char Cyc_FieldPatternList_tok_tag[
21u]="FieldPatternList_tok"; struct Cyc_FieldPatternList_tok_struct{ char* tag;
struct Cyc_List_List* f1; } ; char Cyc_BlockItem_tok_tag[ 14u]="BlockItem_tok";
struct Cyc_BlockItem_tok_struct{ char* tag; void* f1; } ; char Cyc_Kind_tok_tag[
9u]="Kind_tok"; struct Cyc_Kind_tok_struct{ char* tag; void* f1; } ; char Cyc_Attribute_tok_tag[
14u]="Attribute_tok"; struct Cyc_Attribute_tok_struct{ char* tag; void* f1; } ;
char Cyc_AttributeList_tok_tag[ 18u]="AttributeList_tok"; struct Cyc_AttributeList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; struct _tuple12* Cyc_yyget_Int_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp764= yy1; struct _tuple12*
_temp770; _LL766: if((*(( struct _xenum_struct*) _temp764)).tag == Cyc_Int_tok_tag){
_LL771: _temp770=(( struct Cyc_Int_tok_struct*) _temp764)->f1; goto _LL767;}
else{ goto _LL768;} _LL768: goto _LL769; _LL767: return _temp770; _LL769:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp772=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp772->tag= Cyc_Core_Failure_tag;
_temp772->f1=( struct _tagged_string)({ char* _temp773=( char*)"Int_tok"; struct
_tagged_string _temp774; _temp774.curr= _temp773; _temp774.base= _temp773;
_temp774.last_plus_one= _temp773 + 8; _temp774;});( struct _xenum_struct*)
_temp772;})); _LL765:;} struct _tagged_string Cyc_yyget_String_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp775= yy1; struct _tagged_string
_temp781; _LL777: if((*(( struct _xenum_struct*) _temp775)).tag == Cyc_String_tok_tag){
_LL782: _temp781=(( struct Cyc_String_tok_struct*) _temp775)->f1; goto _LL778;}
else{ goto _LL779;} _LL779: goto _LL780; _LL778: return _temp781; _LL780:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp783=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp783->tag= Cyc_Core_Failure_tag;
_temp783->f1=( struct _tagged_string)({ char* _temp784=( char*)"String_tok";
struct _tagged_string _temp785; _temp785.curr= _temp784; _temp785.base= _temp784;
_temp785.last_plus_one= _temp784 + 11; _temp785;});( struct _xenum_struct*)
_temp783;})); _LL776:;} char Cyc_yyget_Char_tok( struct _xenum_struct* yy1){
struct _xenum_struct* _temp786= yy1; char _temp792; _LL788: if((*(( struct
_xenum_struct*) _temp786)).tag == Cyc_Char_tok_tag){ _LL793: _temp792=(( struct
Cyc_Char_tok_struct*) _temp786)->f1; goto _LL789;} else{ goto _LL790;} _LL790:
goto _LL791; _LL789: return _temp792; _LL791:( void) _throw(({ struct Cyc_Core_Failure_struct*
_temp794=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp794->tag= Cyc_Core_Failure_tag; _temp794->f1=( struct _tagged_string)({
char* _temp795=( char*)"Char_tok"; struct _tagged_string _temp796; _temp796.curr=
_temp795; _temp796.base= _temp795; _temp796.last_plus_one= _temp795 + 9;
_temp796;});( struct _xenum_struct*) _temp794;})); _LL787:;} void* Cyc_yyget_Pointer_Sort_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp797= yy1; void* _temp803;
_LL799: if((*(( struct _xenum_struct*) _temp797)).tag == Cyc_Pointer_Sort_tok_tag){
_LL804: _temp803=(( struct Cyc_Pointer_Sort_tok_struct*) _temp797)->f1; goto
_LL800;} else{ goto _LL801;} _LL801: goto _LL802; _LL800: return _temp803;
_LL802:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp805=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp805->tag= Cyc_Core_Failure_tag;
_temp805->f1=( struct _tagged_string)({ char* _temp806=( char*)"Pointer_Sort_tok";
struct _tagged_string _temp807; _temp807.curr= _temp806; _temp807.base= _temp806;
_temp807.last_plus_one= _temp806 + 17; _temp807;});( struct _xenum_struct*)
_temp805;})); _LL798:;} struct Cyc_Absyn_Exp* Cyc_yyget_Exp_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp808= yy1; struct Cyc_Absyn_Exp*
_temp814; _LL810: if((*(( struct _xenum_struct*) _temp808)).tag == Cyc_Exp_tok_tag){
_LL815: _temp814=(( struct Cyc_Exp_tok_struct*) _temp808)->f1; goto _LL811;}
else{ goto _LL812;} _LL812: goto _LL813; _LL811: return _temp814; _LL813:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp816=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp816->tag= Cyc_Core_Failure_tag;
_temp816->f1=( struct _tagged_string)({ char* _temp817=( char*)"Exp_tok"; struct
_tagged_string _temp818; _temp818.curr= _temp817; _temp818.base= _temp817;
_temp818.last_plus_one= _temp817 + 8; _temp818;});( struct _xenum_struct*)
_temp816;})); _LL809:;} struct Cyc_List_List* Cyc_yyget_ExpList_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp819= yy1; struct Cyc_List_List*
_temp825; _LL821: if((*(( struct _xenum_struct*) _temp819)).tag == Cyc_ExpList_tok_tag){
_LL826: _temp825=(( struct Cyc_ExpList_tok_struct*) _temp819)->f1; goto _LL822;}
else{ goto _LL823;} _LL823: goto _LL824; _LL822: return _temp825; _LL824:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp827=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp827->tag= Cyc_Core_Failure_tag;
_temp827->f1=( struct _tagged_string)({ char* _temp828=( char*)"ExpList_tok";
struct _tagged_string _temp829; _temp829.curr= _temp828; _temp829.base= _temp828;
_temp829.last_plus_one= _temp828 + 12; _temp829;});( struct _xenum_struct*)
_temp827;})); _LL820:;} struct Cyc_List_List* Cyc_yyget_InitializerList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp830= yy1; struct Cyc_List_List*
_temp836; _LL832: if((*(( struct _xenum_struct*) _temp830)).tag == Cyc_InitializerList_tok_tag){
_LL837: _temp836=(( struct Cyc_InitializerList_tok_struct*) _temp830)->f1; goto
_LL833;} else{ goto _LL834;} _LL834: goto _LL835; _LL833: return _temp836;
_LL835:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp838=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp838->tag= Cyc_Core_Failure_tag;
_temp838->f1=( struct _tagged_string)({ char* _temp839=( char*)"InitializerList_tok";
struct _tagged_string _temp840; _temp840.curr= _temp839; _temp840.base= _temp839;
_temp840.last_plus_one= _temp839 + 20; _temp840;});( struct _xenum_struct*)
_temp838;})); _LL831:;} void* Cyc_yyget_Primop_tok( struct _xenum_struct* yy1){
struct _xenum_struct* _temp841= yy1; void* _temp847; _LL843: if((*(( struct
_xenum_struct*) _temp841)).tag == Cyc_Primop_tok_tag){ _LL848: _temp847=((
struct Cyc_Primop_tok_struct*) _temp841)->f1; goto _LL844;} else{ goto _LL845;}
_LL845: goto _LL846; _LL844: return _temp847; _LL846:( void) _throw(({ struct
Cyc_Core_Failure_struct* _temp849=( struct Cyc_Core_Failure_struct*) GC_malloc(
sizeof( struct Cyc_Core_Failure_struct)); _temp849->tag= Cyc_Core_Failure_tag;
_temp849->f1=( struct _tagged_string)({ char* _temp850=( char*)"Primop_tok";
struct _tagged_string _temp851; _temp851.curr= _temp850; _temp851.base= _temp850;
_temp851.last_plus_one= _temp850 + 11; _temp851;});( struct _xenum_struct*)
_temp849;})); _LL842:;} struct Cyc_Core_Opt* Cyc_yyget_Primopopt_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp852= yy1; struct Cyc_Core_Opt*
_temp858; _LL854: if((*(( struct _xenum_struct*) _temp852)).tag == Cyc_Primopopt_tok_tag){
_LL859: _temp858=(( struct Cyc_Primopopt_tok_struct*) _temp852)->f1; goto _LL855;}
else{ goto _LL856;} _LL856: goto _LL857; _LL855: return _temp858; _LL857:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp860=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp860->tag= Cyc_Core_Failure_tag;
_temp860->f1=( struct _tagged_string)({ char* _temp861=( char*)"Primopopt_tok";
struct _tagged_string _temp862; _temp862.curr= _temp861; _temp862.base= _temp861;
_temp862.last_plus_one= _temp861 + 14; _temp862;});( struct _xenum_struct*)
_temp860;})); _LL853:;} struct _tuple1* Cyc_yyget_QualId_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp863= yy1; struct _tuple1*
_temp869; _LL865: if((*(( struct _xenum_struct*) _temp863)).tag == Cyc_QualId_tok_tag){
_LL870: _temp869=(( struct Cyc_QualId_tok_struct*) _temp863)->f1; goto _LL866;}
else{ goto _LL867;} _LL867: goto _LL868; _LL866: return _temp869; _LL868:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp871=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp871->tag= Cyc_Core_Failure_tag;
_temp871->f1=( struct _tagged_string)({ char* _temp872=( char*)"QualId_tok";
struct _tagged_string _temp873; _temp873.curr= _temp872; _temp873.base= _temp872;
_temp873.last_plus_one= _temp872 + 11; _temp873;});( struct _xenum_struct*)
_temp871;})); _LL864:;} struct Cyc_Absyn_Stmt* Cyc_yyget_Stmt_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp874= yy1; struct Cyc_Absyn_Stmt*
_temp880; _LL876: if((*(( struct _xenum_struct*) _temp874)).tag == Cyc_Stmt_tok_tag){
_LL881: _temp880=(( struct Cyc_Stmt_tok_struct*) _temp874)->f1; goto _LL877;}
else{ goto _LL878;} _LL878: goto _LL879; _LL877: return _temp880; _LL879:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp882=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp882->tag= Cyc_Core_Failure_tag;
_temp882->f1=( struct _tagged_string)({ char* _temp883=( char*)"Stmt_tok";
struct _tagged_string _temp884; _temp884.curr= _temp883; _temp884.base= _temp883;
_temp884.last_plus_one= _temp883 + 9; _temp884;});( struct _xenum_struct*)
_temp882;})); _LL875:;} void* Cyc_yyget_BlockItem_tok( struct _xenum_struct* yy1){
struct _xenum_struct* _temp885= yy1; void* _temp891; _LL887: if((*(( struct
_xenum_struct*) _temp885)).tag == Cyc_BlockItem_tok_tag){ _LL892: _temp891=((
struct Cyc_BlockItem_tok_struct*) _temp885)->f1; goto _LL888;} else{ goto _LL889;}
_LL889: goto _LL890; _LL888: return _temp891; _LL890:( void) _throw(({ struct
Cyc_Core_Failure_struct* _temp893=( struct Cyc_Core_Failure_struct*) GC_malloc(
sizeof( struct Cyc_Core_Failure_struct)); _temp893->tag= Cyc_Core_Failure_tag;
_temp893->f1=( struct _tagged_string)({ char* _temp894=( char*)"BlockItem_tok";
struct _tagged_string _temp895; _temp895.curr= _temp894; _temp895.base= _temp894;
_temp895.last_plus_one= _temp894 + 14; _temp895;});( struct _xenum_struct*)
_temp893;})); _LL886:;} struct Cyc_List_List* Cyc_yyget_SwitchClauseList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp896= yy1; struct Cyc_List_List*
_temp902; _LL898: if((*(( struct _xenum_struct*) _temp896)).tag == Cyc_SwitchClauseList_tok_tag){
_LL903: _temp902=(( struct Cyc_SwitchClauseList_tok_struct*) _temp896)->f1; goto
_LL899;} else{ goto _LL900;} _LL900: goto _LL901; _LL899: return _temp902;
_LL901:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp904=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp904->tag= Cyc_Core_Failure_tag;
_temp904->f1=( struct _tagged_string)({ char* _temp905=( char*)"SwitchClauseList_tok";
struct _tagged_string _temp906; _temp906.curr= _temp905; _temp906.base= _temp905;
_temp906.last_plus_one= _temp905 + 21; _temp906;});( struct _xenum_struct*)
_temp904;})); _LL897:;} struct Cyc_Absyn_Pat* Cyc_yyget_Pattern_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp907= yy1; struct Cyc_Absyn_Pat*
_temp913; _LL909: if((*(( struct _xenum_struct*) _temp907)).tag == Cyc_Pattern_tok_tag){
_LL914: _temp913=(( struct Cyc_Pattern_tok_struct*) _temp907)->f1; goto _LL910;}
else{ goto _LL911;} _LL911: goto _LL912; _LL910: return _temp913; _LL912:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp915=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp915->tag= Cyc_Core_Failure_tag;
_temp915->f1=( struct _tagged_string)({ char* _temp916=( char*)"Pattern_tok";
struct _tagged_string _temp917; _temp917.curr= _temp916; _temp917.base= _temp916;
_temp917.last_plus_one= _temp916 + 12; _temp917;});( struct _xenum_struct*)
_temp915;})); _LL908:;} struct Cyc_List_List* Cyc_yyget_PatternList_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp918= yy1; struct Cyc_List_List*
_temp924; _LL920: if((*(( struct _xenum_struct*) _temp918)).tag == Cyc_PatternList_tok_tag){
_LL925: _temp924=(( struct Cyc_PatternList_tok_struct*) _temp918)->f1; goto
_LL921;} else{ goto _LL922;} _LL922: goto _LL923; _LL921: return _temp924;
_LL923:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp926=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp926->tag= Cyc_Core_Failure_tag;
_temp926->f1=( struct _tagged_string)({ char* _temp927=( char*)"PatternList_tok";
struct _tagged_string _temp928; _temp928.curr= _temp927; _temp928.base= _temp927;
_temp928.last_plus_one= _temp927 + 16; _temp928;});( struct _xenum_struct*)
_temp926;})); _LL919:;} struct _tuple17* Cyc_yyget_FieldPattern_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp929= yy1; struct _tuple17*
_temp935; _LL931: if((*(( struct _xenum_struct*) _temp929)).tag == Cyc_FieldPattern_tok_tag){
_LL936: _temp935=(( struct Cyc_FieldPattern_tok_struct*) _temp929)->f1; goto
_LL932;} else{ goto _LL933;} _LL933: goto _LL934; _LL932: return _temp935;
_LL934:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp937=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp937->tag= Cyc_Core_Failure_tag;
_temp937->f1=( struct _tagged_string)({ char* _temp938=( char*)"FieldPattern_tok";
struct _tagged_string _temp939; _temp939.curr= _temp938; _temp939.base= _temp938;
_temp939.last_plus_one= _temp938 + 17; _temp939;});( struct _xenum_struct*)
_temp937;})); _LL930:;} struct Cyc_List_List* Cyc_yyget_FieldPatternList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp940= yy1; struct Cyc_List_List*
_temp946; _LL942: if((*(( struct _xenum_struct*) _temp940)).tag == Cyc_FieldPatternList_tok_tag){
_LL947: _temp946=(( struct Cyc_FieldPatternList_tok_struct*) _temp940)->f1; goto
_LL943;} else{ goto _LL944;} _LL944: goto _LL945; _LL943: return _temp946;
_LL945:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp948=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp948->tag= Cyc_Core_Failure_tag;
_temp948->f1=( struct _tagged_string)({ char* _temp949=( char*)"FieldPatternList_tok";
struct _tagged_string _temp950; _temp950.curr= _temp949; _temp950.base= _temp949;
_temp950.last_plus_one= _temp949 + 21; _temp950;});( struct _xenum_struct*)
_temp948;})); _LL941:;} struct Cyc_Absyn_Fndecl* Cyc_yyget_FnDecl_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp951= yy1; struct Cyc_Absyn_Fndecl*
_temp957; _LL953: if((*(( struct _xenum_struct*) _temp951)).tag == Cyc_FnDecl_tok_tag){
_LL958: _temp957=(( struct Cyc_FnDecl_tok_struct*) _temp951)->f1; goto _LL954;}
else{ goto _LL955;} _LL955: goto _LL956; _LL954: return _temp957; _LL956:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp959=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp959->tag= Cyc_Core_Failure_tag;
_temp959->f1=( struct _tagged_string)({ char* _temp960=( char*)"FnDecl_tok";
struct _tagged_string _temp961; _temp961.curr= _temp960; _temp961.base= _temp960;
_temp961.last_plus_one= _temp960 + 11; _temp961;});( struct _xenum_struct*)
_temp959;})); _LL952:;} struct Cyc_List_List* Cyc_yyget_DeclList_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp962= yy1; struct Cyc_List_List*
_temp968; _LL964: if((*(( struct _xenum_struct*) _temp962)).tag == Cyc_DeclList_tok_tag){
_LL969: _temp968=(( struct Cyc_DeclList_tok_struct*) _temp962)->f1; goto _LL965;}
else{ goto _LL966;} _LL966: goto _LL967; _LL965: return _temp968; _LL967:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp970=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp970->tag= Cyc_Core_Failure_tag;
_temp970->f1=( struct _tagged_string)({ char* _temp971=( char*)"DeclList_tok";
struct _tagged_string _temp972; _temp972.curr= _temp971; _temp972.base= _temp971;
_temp972.last_plus_one= _temp971 + 13; _temp972;});( struct _xenum_struct*)
_temp970;})); _LL963:;} struct Cyc_Parse_Declaration_spec* Cyc_yyget_DeclSpec_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp973= yy1; struct Cyc_Parse_Declaration_spec*
_temp979; _LL975: if((*(( struct _xenum_struct*) _temp973)).tag == Cyc_DeclSpec_tok_tag){
_LL980: _temp979=(( struct Cyc_DeclSpec_tok_struct*) _temp973)->f1; goto _LL976;}
else{ goto _LL977;} _LL977: goto _LL978; _LL976: return _temp979; _LL978:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp981=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp981->tag= Cyc_Core_Failure_tag;
_temp981->f1=( struct _tagged_string)({ char* _temp982=( char*)"DeclSpec_tok";
struct _tagged_string _temp983; _temp983.curr= _temp982; _temp983.base= _temp982;
_temp983.last_plus_one= _temp982 + 13; _temp983;});( struct _xenum_struct*)
_temp981;})); _LL974:;} struct _tuple13* Cyc_yyget_InitDecl_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp984= yy1; struct _tuple13*
_temp990; _LL986: if((*(( struct _xenum_struct*) _temp984)).tag == Cyc_InitDecl_tok_tag){
_LL991: _temp990=(( struct Cyc_InitDecl_tok_struct*) _temp984)->f1; goto _LL987;}
else{ goto _LL988;} _LL988: goto _LL989; _LL987: return _temp990; _LL989:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp992=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp992->tag= Cyc_Core_Failure_tag;
_temp992->f1=( struct _tagged_string)({ char* _temp993=( char*)"InitDecl_tok";
struct _tagged_string _temp994; _temp994.curr= _temp993; _temp994.base= _temp993;
_temp994.last_plus_one= _temp993 + 13; _temp994;});( struct _xenum_struct*)
_temp992;})); _LL985:;} struct Cyc_List_List* Cyc_yyget_InitDeclList_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp995= yy1; struct Cyc_List_List*
_temp1001; _LL997: if((*(( struct _xenum_struct*) _temp995)).tag == Cyc_InitDeclList_tok_tag){
_LL1002: _temp1001=(( struct Cyc_InitDeclList_tok_struct*) _temp995)->f1; goto
_LL998;} else{ goto _LL999;} _LL999: goto _LL1000; _LL998: return _temp1001;
_LL1000:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1003=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp1003->tag= Cyc_Core_Failure_tag;
_temp1003->f1=( struct _tagged_string)({ char* _temp1004=( char*)"InitDeclList_tok";
struct _tagged_string _temp1005; _temp1005.curr= _temp1004; _temp1005.base=
_temp1004; _temp1005.last_plus_one= _temp1004 + 17; _temp1005;});( struct
_xenum_struct*) _temp1003;})); _LL996:;} void* Cyc_yyget_StorageClass_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1006= yy1; void*
_temp1012; _LL1008: if((*(( struct _xenum_struct*) _temp1006)).tag == Cyc_StorageClass_tok_tag){
_LL1013: _temp1012=(( struct Cyc_StorageClass_tok_struct*) _temp1006)->f1; goto
_LL1009;} else{ goto _LL1010;} _LL1010: goto _LL1011; _LL1009: return _temp1012;
_LL1011:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1014=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp1014->tag= Cyc_Core_Failure_tag;
_temp1014->f1=( struct _tagged_string)({ char* _temp1015=( char*)"StorageClass_tok";
struct _tagged_string _temp1016; _temp1016.curr= _temp1015; _temp1016.base=
_temp1015; _temp1016.last_plus_one= _temp1015 + 17; _temp1016;});( struct
_xenum_struct*) _temp1014;})); _LL1007:;} void* Cyc_yyget_TypeSpecifier_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1017= yy1; void*
_temp1023; _LL1019: if((*(( struct _xenum_struct*) _temp1017)).tag == Cyc_TypeSpecifier_tok_tag){
_LL1024: _temp1023=(( struct Cyc_TypeSpecifier_tok_struct*) _temp1017)->f1; goto
_LL1020;} else{ goto _LL1021;} _LL1021: goto _LL1022; _LL1020: return _temp1023;
_LL1022:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1025=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp1025->tag= Cyc_Core_Failure_tag;
_temp1025->f1=( struct _tagged_string)({ char* _temp1026=( char*)"TypeSpecifier_tok";
struct _tagged_string _temp1027; _temp1027.curr= _temp1026; _temp1027.base=
_temp1026; _temp1027.last_plus_one= _temp1026 + 18; _temp1027;});( struct
_xenum_struct*) _temp1025;})); _LL1018:;} void* Cyc_yyget_StructOrUnion_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1028= yy1; void*
_temp1034; _LL1030: if((*(( struct _xenum_struct*) _temp1028)).tag == Cyc_StructOrUnion_tok_tag){
_LL1035: _temp1034=(( struct Cyc_StructOrUnion_tok_struct*) _temp1028)->f1; goto
_LL1031;} else{ goto _LL1032;} _LL1032: goto _LL1033; _LL1031: return _temp1034;
_LL1033:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1036=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp1036->tag= Cyc_Core_Failure_tag;
_temp1036->f1=( struct _tagged_string)({ char* _temp1037=( char*)"StructOrUnion_tok";
struct _tagged_string _temp1038; _temp1038.curr= _temp1037; _temp1038.base=
_temp1037; _temp1038.last_plus_one= _temp1037 + 18; _temp1038;});( struct
_xenum_struct*) _temp1036;})); _LL1029:;} struct Cyc_Absyn_Tqual* Cyc_yyget_TypeQual_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1039= yy1; struct Cyc_Absyn_Tqual*
_temp1045; _LL1041: if((*(( struct _xenum_struct*) _temp1039)).tag == Cyc_TypeQual_tok_tag){
_LL1046: _temp1045=(( struct Cyc_TypeQual_tok_struct*) _temp1039)->f1; goto
_LL1042;} else{ goto _LL1043;} _LL1043: goto _LL1044; _LL1042: return _temp1045;
_LL1044:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1047=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp1047->tag= Cyc_Core_Failure_tag;
_temp1047->f1=( struct _tagged_string)({ char* _temp1048=( char*)"TypeQual_tok";
struct _tagged_string _temp1049; _temp1049.curr= _temp1048; _temp1049.base=
_temp1048; _temp1049.last_plus_one= _temp1048 + 13; _temp1049;});( struct
_xenum_struct*) _temp1047;})); _LL1040:;} struct Cyc_List_List* Cyc_yyget_StructFieldDeclList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1050= yy1; struct Cyc_List_List*
_temp1056; _LL1052: if((*(( struct _xenum_struct*) _temp1050)).tag == Cyc_StructFieldDeclList_tok_tag){
_LL1057: _temp1056=(( struct Cyc_StructFieldDeclList_tok_struct*) _temp1050)->f1;
goto _LL1053;} else{ goto _LL1054;} _LL1054: goto _LL1055; _LL1053: return
_temp1056; _LL1055:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1058=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1058->tag= Cyc_Core_Failure_tag; _temp1058->f1=( struct _tagged_string)({
char* _temp1059=( char*)"StructFieldDeclList_tok"; struct _tagged_string
_temp1060; _temp1060.curr= _temp1059; _temp1060.base= _temp1059; _temp1060.last_plus_one=
_temp1059 + 24; _temp1060;});( struct _xenum_struct*) _temp1058;})); _LL1051:;}
struct Cyc_List_List* Cyc_yyget_StructFieldDeclListList_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp1061= yy1; struct Cyc_List_List*
_temp1067; _LL1063: if((*(( struct _xenum_struct*) _temp1061)).tag == Cyc_StructFieldDeclListList_tok_tag){
_LL1068: _temp1067=(( struct Cyc_StructFieldDeclListList_tok_struct*) _temp1061)->f1;
goto _LL1064;} else{ goto _LL1065;} _LL1065: goto _LL1066; _LL1064: return
_temp1067; _LL1066:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1069=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1069->tag= Cyc_Core_Failure_tag; _temp1069->f1=( struct _tagged_string)({
char* _temp1070=( char*)"StructFieldDeclListList_tok"; struct _tagged_string
_temp1071; _temp1071.curr= _temp1070; _temp1071.base= _temp1070; _temp1071.last_plus_one=
_temp1070 + 28; _temp1071;});( struct _xenum_struct*) _temp1069;})); _LL1062:;}
struct Cyc_List_List* Cyc_yyget_TypeModifierList_tok( struct _xenum_struct* yy1){
struct _xenum_struct* _temp1072= yy1; struct Cyc_List_List* _temp1078; _LL1074:
if((*(( struct _xenum_struct*) _temp1072)).tag == Cyc_TypeModifierList_tok_tag){
_LL1079: _temp1078=(( struct Cyc_TypeModifierList_tok_struct*) _temp1072)->f1;
goto _LL1075;} else{ goto _LL1076;} _LL1076: goto _LL1077; _LL1075: return
_temp1078; _LL1077:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1080=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1080->tag= Cyc_Core_Failure_tag; _temp1080->f1=( struct _tagged_string)({
char* _temp1081=( char*)"TypeModifierList_tok"; struct _tagged_string _temp1082;
_temp1082.curr= _temp1081; _temp1082.base= _temp1081; _temp1082.last_plus_one=
_temp1081 + 21; _temp1082;});( struct _xenum_struct*) _temp1080;})); _LL1073:;}
void* Cyc_yyget_Rgn_tok( struct _xenum_struct* yy1){ struct _xenum_struct*
_temp1083= yy1; void* _temp1089; _LL1085: if((*(( struct _xenum_struct*)
_temp1083)).tag == Cyc_Rgn_tok_tag){ _LL1090: _temp1089=(( struct Cyc_Rgn_tok_struct*)
_temp1083)->f1; goto _LL1086;} else{ goto _LL1087;} _LL1087: goto _LL1088;
_LL1086: return _temp1089; _LL1088:( void) _throw(({ struct Cyc_Core_Failure_struct*
_temp1091=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1091->tag= Cyc_Core_Failure_tag; _temp1091->f1=( struct _tagged_string)({
char* _temp1092=( char*)"Rgn_tok"; struct _tagged_string _temp1093; _temp1093.curr=
_temp1092; _temp1093.base= _temp1092; _temp1093.last_plus_one= _temp1092 + 8;
_temp1093;});( struct _xenum_struct*) _temp1091;})); _LL1084:;} struct Cyc_Parse_Declarator*
Cyc_yyget_Declarator_tok( struct _xenum_struct* yy1){ struct _xenum_struct*
_temp1094= yy1; struct Cyc_Parse_Declarator* _temp1100; _LL1096: if((*(( struct
_xenum_struct*) _temp1094)).tag == Cyc_Declarator_tok_tag){ _LL1101: _temp1100=((
struct Cyc_Declarator_tok_struct*) _temp1094)->f1; goto _LL1097;} else{ goto
_LL1098;} _LL1098: goto _LL1099; _LL1097: return _temp1100; _LL1099:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp1102=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp1102->tag= Cyc_Core_Failure_tag;
_temp1102->f1=( struct _tagged_string)({ char* _temp1103=( char*)"Declarator_tok";
struct _tagged_string _temp1104; _temp1104.curr= _temp1103; _temp1104.base=
_temp1103; _temp1104.last_plus_one= _temp1103 + 15; _temp1104;});( struct
_xenum_struct*) _temp1102;})); _LL1095:;} struct _tuple15* Cyc_yyget_DeclaratorExpopt_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1105= yy1; struct
_tuple15* _temp1111; _LL1107: if((*(( struct _xenum_struct*) _temp1105)).tag ==
Cyc_DeclaratorExpopt_tok_tag){ _LL1112: _temp1111=(( struct Cyc_DeclaratorExpopt_tok_struct*)
_temp1105)->f1; goto _LL1108;} else{ goto _LL1109;} _LL1109: goto _LL1110;
_LL1108: return _temp1111; _LL1110:( void) _throw(({ struct Cyc_Core_Failure_struct*
_temp1113=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1113->tag= Cyc_Core_Failure_tag; _temp1113->f1=( struct _tagged_string)({
char* _temp1114=( char*)"DeclaratorExpopt_tok"; struct _tagged_string _temp1115;
_temp1115.curr= _temp1114; _temp1115.base= _temp1114; _temp1115.last_plus_one=
_temp1114 + 21; _temp1115;});( struct _xenum_struct*) _temp1113;})); _LL1106:;}
struct Cyc_List_List* Cyc_yyget_DeclaratorExpoptList_tok( struct _xenum_struct*
yy1){ struct _xenum_struct* _temp1116= yy1; struct Cyc_List_List* _temp1122;
_LL1118: if((*(( struct _xenum_struct*) _temp1116)).tag == Cyc_DeclaratorExpoptList_tok_tag){
_LL1123: _temp1122=(( struct Cyc_DeclaratorExpoptList_tok_struct*) _temp1116)->f1;
goto _LL1119;} else{ goto _LL1120;} _LL1120: goto _LL1121; _LL1119: return
_temp1122; _LL1121:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1124=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1124->tag= Cyc_Core_Failure_tag; _temp1124->f1=( struct _tagged_string)({
char* _temp1125=( char*)"DeclaratorExpoptList_tok"; struct _tagged_string
_temp1126; _temp1126.curr= _temp1125; _temp1126.base= _temp1125; _temp1126.last_plus_one=
_temp1125 + 25; _temp1126;});( struct _xenum_struct*) _temp1124;})); _LL1117:;}
struct Cyc_Parse_Abstractdeclarator* Cyc_yyget_AbstractDeclarator_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp1127= yy1; struct Cyc_Parse_Abstractdeclarator*
_temp1133; _LL1129: if((*(( struct _xenum_struct*) _temp1127)).tag == Cyc_AbstractDeclarator_tok_tag){
_LL1134: _temp1133=(( struct Cyc_AbstractDeclarator_tok_struct*) _temp1127)->f1;
goto _LL1130;} else{ goto _LL1131;} _LL1131: goto _LL1132; _LL1130: return
_temp1133; _LL1132:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1135=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1135->tag= Cyc_Core_Failure_tag; _temp1135->f1=( struct _tagged_string)({
char* _temp1136=( char*)"AbstractDeclarator_tok"; struct _tagged_string
_temp1137; _temp1137.curr= _temp1136; _temp1137.base= _temp1136; _temp1137.last_plus_one=
_temp1136 + 23; _temp1137;});( struct _xenum_struct*) _temp1135;})); _LL1128:;}
struct Cyc_Absyn_Enumfield* Cyc_yyget_EnumeratorField_tok( struct _xenum_struct*
yy1){ struct _xenum_struct* _temp1138= yy1; struct Cyc_Absyn_Enumfield*
_temp1144; _LL1140: if((*(( struct _xenum_struct*) _temp1138)).tag == Cyc_EnumeratorField_tok_tag){
_LL1145: _temp1144=(( struct Cyc_EnumeratorField_tok_struct*) _temp1138)->f1;
goto _LL1141;} else{ goto _LL1142;} _LL1142: goto _LL1143; _LL1141: return
_temp1144; _LL1143:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1146=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1146->tag= Cyc_Core_Failure_tag; _temp1146->f1=( struct _tagged_string)({
char* _temp1147=( char*)"EnumeratorField_tok"; struct _tagged_string _temp1148;
_temp1148.curr= _temp1147; _temp1148.base= _temp1147; _temp1148.last_plus_one=
_temp1147 + 20; _temp1148;});( struct _xenum_struct*) _temp1146;})); _LL1139:;}
struct Cyc_List_List* Cyc_yyget_EnumeratorFieldList_tok( struct _xenum_struct*
yy1){ struct _xenum_struct* _temp1149= yy1; struct Cyc_List_List* _temp1155;
_LL1151: if((*(( struct _xenum_struct*) _temp1149)).tag == Cyc_EnumeratorFieldList_tok_tag){
_LL1156: _temp1155=(( struct Cyc_EnumeratorFieldList_tok_struct*) _temp1149)->f1;
goto _LL1152;} else{ goto _LL1153;} _LL1153: goto _LL1154; _LL1152: return
_temp1155; _LL1154:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1157=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1157->tag= Cyc_Core_Failure_tag; _temp1157->f1=( struct _tagged_string)({
char* _temp1158=( char*)"EnumeratorFieldList_tok"; struct _tagged_string
_temp1159; _temp1159.curr= _temp1158; _temp1159.base= _temp1158; _temp1159.last_plus_one=
_temp1158 + 24; _temp1159;});( struct _xenum_struct*) _temp1157;})); _LL1150:;}
struct _tuple14* Cyc_yyget_QualSpecList_tok( struct _xenum_struct* yy1){ struct
_xenum_struct* _temp1160= yy1; struct _tuple14* _temp1166; _LL1162: if((*((
struct _xenum_struct*) _temp1160)).tag == Cyc_QualSpecList_tok_tag){ _LL1167:
_temp1166=(( struct Cyc_QualSpecList_tok_struct*) _temp1160)->f1; goto _LL1163;}
else{ goto _LL1164;} _LL1164: goto _LL1165; _LL1163: return _temp1166; _LL1165:(
void) _throw(({ struct Cyc_Core_Failure_struct* _temp1168=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp1168->tag= Cyc_Core_Failure_tag;
_temp1168->f1=( struct _tagged_string)({ char* _temp1169=( char*)"QualSpecList_tok";
struct _tagged_string _temp1170; _temp1170.curr= _temp1169; _temp1170.base=
_temp1169; _temp1170.last_plus_one= _temp1169 + 17; _temp1170;});( struct
_xenum_struct*) _temp1168;})); _LL1161:;} struct Cyc_List_List* Cyc_yyget_IdList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1171= yy1; struct Cyc_List_List*
_temp1177; _LL1173: if((*(( struct _xenum_struct*) _temp1171)).tag == Cyc_IdList_tok_tag){
_LL1178: _temp1177=(( struct Cyc_IdList_tok_struct*) _temp1171)->f1; goto
_LL1174;} else{ goto _LL1175;} _LL1175: goto _LL1176; _LL1174: return _temp1177;
_LL1176:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1179=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp1179->tag= Cyc_Core_Failure_tag;
_temp1179->f1=( struct _tagged_string)({ char* _temp1180=( char*)"IdList_tok";
struct _tagged_string _temp1181; _temp1181.curr= _temp1180; _temp1181.base=
_temp1180; _temp1181.last_plus_one= _temp1180 + 11; _temp1181;});( struct
_xenum_struct*) _temp1179;})); _LL1172:;} struct _tuple2* Cyc_yyget_ParamDecl_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1182= yy1; struct _tuple2*
_temp1188; _LL1184: if((*(( struct _xenum_struct*) _temp1182)).tag == Cyc_ParamDecl_tok_tag){
_LL1189: _temp1188=(( struct Cyc_ParamDecl_tok_struct*) _temp1182)->f1; goto
_LL1185;} else{ goto _LL1186;} _LL1186: goto _LL1187; _LL1185: return _temp1188;
_LL1187:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1190=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp1190->tag= Cyc_Core_Failure_tag;
_temp1190->f1=( struct _tagged_string)({ char* _temp1191=( char*)"ParamDecl_tok";
struct _tagged_string _temp1192; _temp1192.curr= _temp1191; _temp1192.base=
_temp1191; _temp1192.last_plus_one= _temp1191 + 14; _temp1192;});( struct
_xenum_struct*) _temp1190;})); _LL1183:;} struct Cyc_List_List* Cyc_yyget_ParamDeclList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1193= yy1; struct Cyc_List_List*
_temp1199; _LL1195: if((*(( struct _xenum_struct*) _temp1193)).tag == Cyc_ParamDeclList_tok_tag){
_LL1200: _temp1199=(( struct Cyc_ParamDeclList_tok_struct*) _temp1193)->f1; goto
_LL1196;} else{ goto _LL1197;} _LL1197: goto _LL1198; _LL1196: return _temp1199;
_LL1198:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1201=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp1201->tag= Cyc_Core_Failure_tag;
_temp1201->f1=( struct _tagged_string)({ char* _temp1202=( char*)"ParamDeclList_tok";
struct _tagged_string _temp1203; _temp1203.curr= _temp1202; _temp1203.base=
_temp1202; _temp1203.last_plus_one= _temp1202 + 18; _temp1203;});( struct
_xenum_struct*) _temp1201;})); _LL1194:;} struct _tuple16* Cyc_yyget_ParamDeclListBool_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1204= yy1; struct
_tuple16* _temp1210; _LL1206: if((*(( struct _xenum_struct*) _temp1204)).tag ==
Cyc_ParamDeclListBool_tok_tag){ _LL1211: _temp1210=(( struct Cyc_ParamDeclListBool_tok_struct*)
_temp1204)->f1; goto _LL1207;} else{ goto _LL1208;} _LL1208: goto _LL1209;
_LL1207: return _temp1210; _LL1209:( void) _throw(({ struct Cyc_Core_Failure_struct*
_temp1212=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1212->tag= Cyc_Core_Failure_tag; _temp1212->f1=( struct _tagged_string)({
char* _temp1213=( char*)"ParamDeclListBool_tok"; struct _tagged_string _temp1214;
_temp1214.curr= _temp1213; _temp1214.base= _temp1213; _temp1214.last_plus_one=
_temp1213 + 22; _temp1214;});( struct _xenum_struct*) _temp1212;})); _LL1205:;}
struct Cyc_List_List* Cyc_yyget_TypeList_tok( struct _xenum_struct* yy1){ struct
_xenum_struct* _temp1215= yy1; struct Cyc_List_List* _temp1221; _LL1217: if((*((
struct _xenum_struct*) _temp1215)).tag == Cyc_TypeList_tok_tag){ _LL1222:
_temp1221=(( struct Cyc_TypeList_tok_struct*) _temp1215)->f1; goto _LL1218;}
else{ goto _LL1219;} _LL1219: goto _LL1220; _LL1218: return _temp1221; _LL1220:(
void) _throw(({ struct Cyc_Core_Failure_struct* _temp1223=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp1223->tag= Cyc_Core_Failure_tag;
_temp1223->f1=( struct _tagged_string)({ char* _temp1224=( char*)"TypeList_tok";
struct _tagged_string _temp1225; _temp1225.curr= _temp1224; _temp1225.base=
_temp1224; _temp1225.last_plus_one= _temp1224 + 13; _temp1225;});( struct
_xenum_struct*) _temp1223;})); _LL1216:;} struct Cyc_List_List* Cyc_yyget_DesignatorList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1226= yy1; struct Cyc_List_List*
_temp1232; _LL1228: if((*(( struct _xenum_struct*) _temp1226)).tag == Cyc_DesignatorList_tok_tag){
_LL1233: _temp1232=(( struct Cyc_DesignatorList_tok_struct*) _temp1226)->f1;
goto _LL1229;} else{ goto _LL1230;} _LL1230: goto _LL1231; _LL1229: return
_temp1232; _LL1231:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1234=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1234->tag= Cyc_Core_Failure_tag; _temp1234->f1=( struct _tagged_string)({
char* _temp1235=( char*)"DesignatorList_tok"; struct _tagged_string _temp1236;
_temp1236.curr= _temp1235; _temp1236.base= _temp1235; _temp1236.last_plus_one=
_temp1235 + 19; _temp1236;});( struct _xenum_struct*) _temp1234;})); _LL1227:;}
void* Cyc_yyget_Designator_tok( struct _xenum_struct* yy1){ struct _xenum_struct*
_temp1237= yy1; void* _temp1243; _LL1239: if((*(( struct _xenum_struct*)
_temp1237)).tag == Cyc_Designator_tok_tag){ _LL1244: _temp1243=(( struct Cyc_Designator_tok_struct*)
_temp1237)->f1; goto _LL1240;} else{ goto _LL1241;} _LL1241: goto _LL1242;
_LL1240: return _temp1243; _LL1242:( void) _throw(({ struct Cyc_Core_Failure_struct*
_temp1245=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1245->tag= Cyc_Core_Failure_tag; _temp1245->f1=( struct _tagged_string)({
char* _temp1246=( char*)"Designator_tok"; struct _tagged_string _temp1247;
_temp1247.curr= _temp1246; _temp1247.base= _temp1246; _temp1247.last_plus_one=
_temp1246 + 15; _temp1247;});( struct _xenum_struct*) _temp1245;})); _LL1238:;}
void* Cyc_yyget_Kind_tok( struct _xenum_struct* yy1){ struct _xenum_struct*
_temp1248= yy1; void* _temp1254; _LL1250: if((*(( struct _xenum_struct*)
_temp1248)).tag == Cyc_Kind_tok_tag){ _LL1255: _temp1254=(( struct Cyc_Kind_tok_struct*)
_temp1248)->f1; goto _LL1251;} else{ goto _LL1252;} _LL1252: goto _LL1253;
_LL1251: return _temp1254; _LL1253:( void) _throw(({ struct Cyc_Core_Failure_struct*
_temp1256=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1256->tag= Cyc_Core_Failure_tag; _temp1256->f1=( struct _tagged_string)({
char* _temp1257=( char*)"Kind_tok"; struct _tagged_string _temp1258; _temp1258.curr=
_temp1257; _temp1258.base= _temp1257; _temp1258.last_plus_one= _temp1257 + 9;
_temp1258;});( struct _xenum_struct*) _temp1256;})); _LL1249:;} void* Cyc_yyget_Type_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1259= yy1; void*
_temp1265; _LL1261: if((*(( struct _xenum_struct*) _temp1259)).tag == Cyc_Type_tok_tag){
_LL1266: _temp1265=(( struct Cyc_Type_tok_struct*) _temp1259)->f1; goto _LL1262;}
else{ goto _LL1263;} _LL1263: goto _LL1264; _LL1262: return _temp1265; _LL1264:(
void) _throw(({ struct Cyc_Core_Failure_struct* _temp1267=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp1267->tag= Cyc_Core_Failure_tag;
_temp1267->f1=( struct _tagged_string)({ char* _temp1268=( char*)"Type_tok";
struct _tagged_string _temp1269; _temp1269.curr= _temp1268; _temp1269.base=
_temp1268; _temp1269.last_plus_one= _temp1268 + 9; _temp1269;});( struct
_xenum_struct*) _temp1267;})); _LL1260:;} struct Cyc_List_List* Cyc_yyget_AttributeList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1270= yy1; struct Cyc_List_List*
_temp1276; _LL1272: if((*(( struct _xenum_struct*) _temp1270)).tag == Cyc_AttributeList_tok_tag){
_LL1277: _temp1276=(( struct Cyc_AttributeList_tok_struct*) _temp1270)->f1; goto
_LL1273;} else{ goto _LL1274;} _LL1274: goto _LL1275; _LL1273: return _temp1276;
_LL1275:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1278=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp1278->tag= Cyc_Core_Failure_tag;
_temp1278->f1=( struct _tagged_string)({ char* _temp1279=( char*)"AttributeList_tok";
struct _tagged_string _temp1280; _temp1280.curr= _temp1279; _temp1280.base=
_temp1279; _temp1280.last_plus_one= _temp1279 + 18; _temp1280;});( struct
_xenum_struct*) _temp1278;})); _LL1271:;} void* Cyc_yyget_Attribute_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp1281= yy1; void* _temp1287;
_LL1283: if((*(( struct _xenum_struct*) _temp1281)).tag == Cyc_Attribute_tok_tag){
_LL1288: _temp1287=(( struct Cyc_Attribute_tok_struct*) _temp1281)->f1; goto
_LL1284;} else{ goto _LL1285;} _LL1285: goto _LL1286; _LL1284: return _temp1287;
_LL1286:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1289=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp1289->tag= Cyc_Core_Failure_tag;
_temp1289->f1=( struct _tagged_string)({ char* _temp1290=( char*)"Attribute_tok";
struct _tagged_string _temp1291; _temp1291.curr= _temp1290; _temp1291.base=
_temp1290; _temp1291.last_plus_one= _temp1290 + 14; _temp1291;});( struct
_xenum_struct*) _temp1289;})); _LL1282:;} struct Cyc_Yyltype{ int timestamp; int
first_line; int first_column; int last_line; int last_column; struct
_tagged_string text; } ; typedef struct Cyc_Yyltype Cyc_yyltype; struct Cyc_Yyltype
Cyc_yynewloc(){ return({ struct Cyc_Yyltype _temp1292; _temp1292.timestamp= 0;
_temp1292.first_line= 0; _temp1292.first_column= 0; _temp1292.last_line= 0;
_temp1292.last_column= 0; _temp1292.text=( struct _tagged_string)({ char*
_temp1293=( char*)""; struct _tagged_string _temp1294; _temp1294.curr= _temp1293;
_temp1294.base= _temp1293; _temp1294.last_plus_one= _temp1293 + 1; _temp1294;});
_temp1292;});} static char _temp1297[ 1u]=""; struct Cyc_Yyltype Cyc_yylloc=(
struct Cyc_Yyltype){.timestamp= 0,.first_line= 0,.first_column= 0,.last_line= 0,.last_column=
0,.text=( struct _tagged_string){ _temp1297, _temp1297, _temp1297 + 1u}}; static
short Cyc_yytranslate[ 347u]={ (short)0, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)118, (short)2, (short)2, (short)100,
(short)116, (short)112, (short)2, (short)96, (short)97, (short)106, (short)109,
(short)98, (short)111, (short)110, (short)115, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)103,
(short)92, (short)101, (short)95, (short)102, (short)108, (short)107, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)104, (short)2, (short)105, (short)114, (short)99, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)93, (short)113, (short)94, (short)117, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)1, (short)2, (short)3, (short)4, (short)5, (short)6, (short)7, (short)8,
(short)9, (short)10, (short)11, (short)12, (short)13, (short)14, (short)15,
(short)16, (short)17, (short)18, (short)19, (short)20, (short)21, (short)22,
(short)23, (short)24, (short)25, (short)26, (short)27, (short)28, (short)29,
(short)30, (short)31, (short)32, (short)33, (short)34, (short)35, (short)36,
(short)37, (short)38, (short)39, (short)40, (short)41, (short)42, (short)43,
(short)44, (short)45, (short)46, (short)47, (short)48, (short)49, (short)50,
(short)51, (short)52, (short)53, (short)54, (short)55, (short)56, (short)57,
(short)58, (short)59, (short)60, (short)61, (short)62, (short)63, (short)64,
(short)65, (short)66, (short)67, (short)68, (short)69, (short)70, (short)71,
(short)72, (short)73, (short)74, (short)75, (short)76, (short)77, (short)78,
(short)79, (short)80, (short)81, (short)82, (short)83, (short)84, (short)85,
(short)86, (short)87, (short)88, (short)89, (short)90, (short)91}; static char
_temp1300[ 2u]="$"; static char _temp1303[ 6u]="error"; static char _temp1306[
12u]="$undefined."; static char _temp1309[ 5u]="AUTO"; static char _temp1312[ 9u]="REGISTER";
static char _temp1315[ 7u]="STATIC"; static char _temp1318[ 7u]="EXTERN"; static
char _temp1321[ 8u]="TYPEDEF"; static char _temp1324[ 5u]="VOID"; static char
_temp1327[ 5u]="CHAR"; static char _temp1330[ 6u]="SHORT"; static char _temp1333[
4u]="INT"; static char _temp1336[ 5u]="LONG"; static char _temp1339[ 6u]="FLOAT";
static char _temp1342[ 7u]="DOUBLE"; static char _temp1345[ 7u]="SIGNED"; static
char _temp1348[ 9u]="UNSIGNED"; static char _temp1351[ 6u]="CONST"; static char
_temp1354[ 9u]="VOLATILE"; static char _temp1357[ 9u]="RESTRICT"; static char
_temp1360[ 7u]="STRUCT"; static char _temp1363[ 6u]="UNION"; static char
_temp1366[ 5u]="CASE"; static char _temp1369[ 8u]="DEFAULT"; static char
_temp1372[ 7u]="INLINE"; static char _temp1375[ 3u]="IF"; static char _temp1378[
5u]="ELSE"; static char _temp1381[ 7u]="SWITCH"; static char _temp1384[ 6u]="WHILE";
static char _temp1387[ 3u]="DO"; static char _temp1390[ 4u]="FOR"; static char
_temp1393[ 5u]="GOTO"; static char _temp1396[ 9u]="CONTINUE"; static char
_temp1399[ 6u]="BREAK"; static char _temp1402[ 7u]="RETURN"; static char
_temp1405[ 7u]="SIZEOF"; static char _temp1408[ 5u]="ENUM"; static char
_temp1411[ 8u]="NULL_kw"; static char _temp1414[ 4u]="LET"; static char
_temp1417[ 6u]="THROW"; static char _temp1420[ 4u]="TRY"; static char _temp1423[
6u]="CATCH"; static char _temp1426[ 4u]="NEW"; static char _temp1429[ 9u]="ABSTRACT";
static char _temp1432[ 9u]="FALLTHRU"; static char _temp1435[ 6u]="USING";
static char _temp1438[ 10u]="NAMESPACE"; static char _temp1441[ 6u]="XENUM";
static char _temp1444[ 5u]="FILL"; static char _temp1447[ 8u]="CODEGEN"; static
char _temp1450[ 4u]="CUT"; static char _temp1453[ 7u]="SPLICE"; static char
_temp1456[ 7u]="PRINTF"; static char _temp1459[ 8u]="FPRINTF"; static char
_temp1462[ 8u]="XPRINTF"; static char _temp1465[ 6u]="SCANF"; static char
_temp1468[ 7u]="FSCANF"; static char _temp1471[ 7u]="SSCANF"; static char
_temp1474[ 7u]="MALLOC"; static char _temp1477[ 7u]="PTR_OP"; static char
_temp1480[ 7u]="INC_OP"; static char _temp1483[ 7u]="DEC_OP"; static char
_temp1486[ 8u]="LEFT_OP"; static char _temp1489[ 9u]="RIGHT_OP"; static char
_temp1492[ 6u]="LE_OP"; static char _temp1495[ 6u]="GE_OP"; static char
_temp1498[ 6u]="EQ_OP"; static char _temp1501[ 6u]="NE_OP"; static char
_temp1504[ 7u]="AND_OP"; static char _temp1507[ 6u]="OR_OP"; static char
_temp1510[ 11u]="MUL_ASSIGN"; static char _temp1513[ 11u]="DIV_ASSIGN"; static
char _temp1516[ 11u]="MOD_ASSIGN"; static char _temp1519[ 11u]="ADD_ASSIGN";
static char _temp1522[ 11u]="SUB_ASSIGN"; static char _temp1525[ 12u]="LEFT_ASSIGN";
static char _temp1528[ 13u]="RIGHT_ASSIGN"; static char _temp1531[ 11u]="AND_ASSIGN";
static char _temp1534[ 11u]="XOR_ASSIGN"; static char _temp1537[ 10u]="OR_ASSIGN";
static char _temp1540[ 9u]="ELLIPSIS"; static char _temp1543[ 11u]="LEFT_RIGHT";
static char _temp1546[ 12u]="COLON_COLON"; static char _temp1549[ 11u]="IDENTIFIER";
static char _temp1552[ 17u]="INTEGER_CONSTANT"; static char _temp1555[ 7u]="STRING";
static char _temp1558[ 19u]="CHARACTER_CONSTANT"; static char _temp1561[ 18u]="FLOATING_CONSTANT";
static char _temp1564[ 9u]="TYPE_VAR"; static char _temp1567[ 16u]="QUAL_IDENTIFIER";
static char _temp1570[ 18u]="QUAL_TYPEDEF_NAME"; static char _temp1573[ 10u]="ATTRIBUTE";
static char _temp1576[ 4u]="';'"; static char _temp1579[ 4u]="'{'"; static char
_temp1582[ 4u]="'}'"; static char _temp1585[ 4u]="'='"; static char _temp1588[ 4u]="'('";
static char _temp1591[ 4u]="')'"; static char _temp1594[ 4u]="','"; static char
_temp1597[ 4u]="'_'"; static char _temp1600[ 4u]="'$'"; static char _temp1603[ 4u]="'<'";
static char _temp1606[ 4u]="'>'"; static char _temp1609[ 4u]="':'"; static char
_temp1612[ 4u]="'['"; static char _temp1615[ 4u]="']'"; static char _temp1618[ 4u]="'*'";
static char _temp1621[ 4u]="'@'"; static char _temp1624[ 4u]="'?'"; static char
_temp1627[ 4u]="'+'"; static char _temp1630[ 4u]="'.'"; static char _temp1633[ 4u]="'-'";
static char _temp1636[ 4u]="'&'"; static char _temp1639[ 4u]="'|'"; static char
_temp1642[ 4u]="'^'"; static char _temp1645[ 4u]="'/'"; static char _temp1648[ 4u]="'%'";
static char _temp1651[ 4u]="'~'"; static char _temp1654[ 4u]="'!'"; static char
_temp1657[ 5u]="prog"; static char _temp1660[ 17u]="translation_unit"; static
char _temp1663[ 21u]="translation_unit_opt"; static char _temp1666[ 21u]="external_declaration";
static char _temp1669[ 20u]="function_definition"; static char _temp1672[ 21u]="function_definition2";
static char _temp1675[ 13u]="using_action"; static char _temp1678[ 15u]="unusing_action";
static char _temp1681[ 17u]="namespace_action"; static char _temp1684[ 19u]="unnamespace_action";
static char _temp1687[ 12u]="declaration"; static char _temp1690[ 17u]="declaration_list";
static char _temp1693[ 23u]="declaration_specifiers"; static char _temp1696[ 24u]="storage_class_specifier";
static char _temp1699[ 15u]="attributes_opt"; static char _temp1702[ 11u]="attributes";
static char _temp1705[ 15u]="attribute_list"; static char _temp1708[ 10u]="attribute";
static char _temp1711[ 15u]="type_specifier"; static char _temp1714[ 5u]="kind";
static char _temp1717[ 15u]="type_qualifier"; static char _temp1720[ 26u]="struct_or_union_specifier";
static char _temp1723[ 16u]="type_params_opt"; static char _temp1726[ 16u]="struct_or_union";
static char _temp1729[ 24u]="struct_declaration_list"; static char _temp1732[ 25u]="struct_declaration_list0";
static char _temp1735[ 21u]="init_declarator_list"; static char _temp1738[ 22u]="init_declarator_list0";
static char _temp1741[ 16u]="init_declarator"; static char _temp1744[ 19u]="struct_declaration";
static char _temp1747[ 25u]="specifier_qualifier_list"; static char _temp1750[
23u]="struct_declarator_list"; static char _temp1753[ 24u]="struct_declarator_list0";
static char _temp1756[ 18u]="struct_declarator"; static char _temp1759[ 15u]="enum_specifier";
static char _temp1762[ 16u]="enumerator_list"; static char _temp1765[ 11u]="enumerator";
static char _temp1768[ 11u]="declarator"; static char _temp1771[ 18u]="direct_declarator";
static char _temp1774[ 8u]="pointer"; static char _temp1777[ 13u]="pointer_char";
static char _temp1780[ 8u]="rgn_opt"; static char _temp1783[ 20u]="type_qualifier_list";
static char _temp1786[ 20u]="parameter_type_list"; static char _temp1789[ 11u]="effect_set";
static char _temp1792[ 14u]="atomic_effect"; static char _temp1795[ 11u]="region_set";
static char _temp1798[ 15u]="parameter_list"; static char _temp1801[ 22u]="parameter_declaration";
static char _temp1804[ 16u]="identifier_list"; static char _temp1807[ 17u]="identifier_list0";
static char _temp1810[ 12u]="initializer"; static char _temp1813[ 17u]="initializer_list";
static char _temp1816[ 12u]="designation"; static char _temp1819[ 16u]="designator_list";
static char _temp1822[ 11u]="designator"; static char _temp1825[ 10u]="type_name";
static char _temp1828[ 14u]="any_type_name"; static char _temp1831[ 15u]="type_name_list";
static char _temp1834[ 20u]="abstract_declarator"; static char _temp1837[ 27u]="direct_abstract_declarator";
static char _temp1840[ 10u]="statement"; static char _temp1843[ 18u]="labeled_statement";
static char _temp1846[ 21u]="expression_statement"; static char _temp1849[ 19u]="compound_statement";
static char _temp1852[ 16u]="block_item_list"; static char _temp1855[ 11u]="block_item";
static char _temp1858[ 20u]="selection_statement"; static char _temp1861[ 15u]="switch_clauses";
static char _temp1864[ 20u]="iteration_statement"; static char _temp1867[ 15u]="jump_statement";
static char _temp1870[ 8u]="pattern"; static char _temp1873[ 19u]="tuple_pattern_list";
static char _temp1876[ 20u]="tuple_pattern_list0"; static char _temp1879[ 14u]="field_pattern";
static char _temp1882[ 19u]="field_pattern_list"; static char _temp1885[ 20u]="field_pattern_list0";
static char _temp1888[ 11u]="expression"; static char _temp1891[ 22u]="assignment_expression";
static char _temp1894[ 20u]="assignment_operator"; static char _temp1897[ 23u]="conditional_expression";
static char _temp1900[ 20u]="constant_expression"; static char _temp1903[ 22u]="logical_or_expression";
static char _temp1906[ 23u]="logical_and_expression"; static char _temp1909[ 24u]="inclusive_or_expression";
static char _temp1912[ 24u]="exclusive_or_expression"; static char _temp1915[ 15u]="and_expression";
static char _temp1918[ 20u]="equality_expression"; static char _temp1921[ 22u]="relational_expression";
static char _temp1924[ 17u]="shift_expression"; static char _temp1927[ 20u]="additive_expression";
static char _temp1930[ 26u]="multiplicative_expression"; static char _temp1933[
16u]="cast_expression"; static char _temp1936[ 17u]="unary_expression"; static
char _temp1939[ 14u]="format_primop"; static char _temp1942[ 15u]="unary_operator";
static char _temp1945[ 19u]="postfix_expression"; static char _temp1948[ 19u]="primary_expression";
static char _temp1951[ 25u]="argument_expression_list"; static char _temp1954[
26u]="argument_expression_list0"; static char _temp1957[ 9u]="constant"; static
char _temp1960[ 20u]="qual_opt_identifier"; static struct _tagged_string Cyc_yytname[
221u]={( struct _tagged_string){ _temp1300, _temp1300, _temp1300 + 2u},( struct
_tagged_string){ _temp1303, _temp1303, _temp1303 + 6u},( struct _tagged_string){
_temp1306, _temp1306, _temp1306 + 12u},( struct _tagged_string){ _temp1309,
_temp1309, _temp1309 + 5u},( struct _tagged_string){ _temp1312, _temp1312,
_temp1312 + 9u},( struct _tagged_string){ _temp1315, _temp1315, _temp1315 + 7u},(
struct _tagged_string){ _temp1318, _temp1318, _temp1318 + 7u},( struct
_tagged_string){ _temp1321, _temp1321, _temp1321 + 8u},( struct _tagged_string){
_temp1324, _temp1324, _temp1324 + 5u},( struct _tagged_string){ _temp1327,
_temp1327, _temp1327 + 5u},( struct _tagged_string){ _temp1330, _temp1330,
_temp1330 + 6u},( struct _tagged_string){ _temp1333, _temp1333, _temp1333 + 4u},(
struct _tagged_string){ _temp1336, _temp1336, _temp1336 + 5u},( struct
_tagged_string){ _temp1339, _temp1339, _temp1339 + 6u},( struct _tagged_string){
_temp1342, _temp1342, _temp1342 + 7u},( struct _tagged_string){ _temp1345,
_temp1345, _temp1345 + 7u},( struct _tagged_string){ _temp1348, _temp1348,
_temp1348 + 9u},( struct _tagged_string){ _temp1351, _temp1351, _temp1351 + 6u},(
struct _tagged_string){ _temp1354, _temp1354, _temp1354 + 9u},( struct
_tagged_string){ _temp1357, _temp1357, _temp1357 + 9u},( struct _tagged_string){
_temp1360, _temp1360, _temp1360 + 7u},( struct _tagged_string){ _temp1363,
_temp1363, _temp1363 + 6u},( struct _tagged_string){ _temp1366, _temp1366,
_temp1366 + 5u},( struct _tagged_string){ _temp1369, _temp1369, _temp1369 + 8u},(
struct _tagged_string){ _temp1372, _temp1372, _temp1372 + 7u},( struct
_tagged_string){ _temp1375, _temp1375, _temp1375 + 3u},( struct _tagged_string){
_temp1378, _temp1378, _temp1378 + 5u},( struct _tagged_string){ _temp1381,
_temp1381, _temp1381 + 7u},( struct _tagged_string){ _temp1384, _temp1384,
_temp1384 + 6u},( struct _tagged_string){ _temp1387, _temp1387, _temp1387 + 3u},(
struct _tagged_string){ _temp1390, _temp1390, _temp1390 + 4u},( struct
_tagged_string){ _temp1393, _temp1393, _temp1393 + 5u},( struct _tagged_string){
_temp1396, _temp1396, _temp1396 + 9u},( struct _tagged_string){ _temp1399,
_temp1399, _temp1399 + 6u},( struct _tagged_string){ _temp1402, _temp1402,
_temp1402 + 7u},( struct _tagged_string){ _temp1405, _temp1405, _temp1405 + 7u},(
struct _tagged_string){ _temp1408, _temp1408, _temp1408 + 5u},( struct
_tagged_string){ _temp1411, _temp1411, _temp1411 + 8u},( struct _tagged_string){
_temp1414, _temp1414, _temp1414 + 4u},( struct _tagged_string){ _temp1417,
_temp1417, _temp1417 + 6u},( struct _tagged_string){ _temp1420, _temp1420,
_temp1420 + 4u},( struct _tagged_string){ _temp1423, _temp1423, _temp1423 + 6u},(
struct _tagged_string){ _temp1426, _temp1426, _temp1426 + 4u},( struct
_tagged_string){ _temp1429, _temp1429, _temp1429 + 9u},( struct _tagged_string){
_temp1432, _temp1432, _temp1432 + 9u},( struct _tagged_string){ _temp1435,
_temp1435, _temp1435 + 6u},( struct _tagged_string){ _temp1438, _temp1438,
_temp1438 + 10u},( struct _tagged_string){ _temp1441, _temp1441, _temp1441 + 6u},(
struct _tagged_string){ _temp1444, _temp1444, _temp1444 + 5u},( struct
_tagged_string){ _temp1447, _temp1447, _temp1447 + 8u},( struct _tagged_string){
_temp1450, _temp1450, _temp1450 + 4u},( struct _tagged_string){ _temp1453,
_temp1453, _temp1453 + 7u},( struct _tagged_string){ _temp1456, _temp1456,
_temp1456 + 7u},( struct _tagged_string){ _temp1459, _temp1459, _temp1459 + 8u},(
struct _tagged_string){ _temp1462, _temp1462, _temp1462 + 8u},( struct
_tagged_string){ _temp1465, _temp1465, _temp1465 + 6u},( struct _tagged_string){
_temp1468, _temp1468, _temp1468 + 7u},( struct _tagged_string){ _temp1471,
_temp1471, _temp1471 + 7u},( struct _tagged_string){ _temp1474, _temp1474,
_temp1474 + 7u},( struct _tagged_string){ _temp1477, _temp1477, _temp1477 + 7u},(
struct _tagged_string){ _temp1480, _temp1480, _temp1480 + 7u},( struct
_tagged_string){ _temp1483, _temp1483, _temp1483 + 7u},( struct _tagged_string){
_temp1486, _temp1486, _temp1486 + 8u},( struct _tagged_string){ _temp1489,
_temp1489, _temp1489 + 9u},( struct _tagged_string){ _temp1492, _temp1492,
_temp1492 + 6u},( struct _tagged_string){ _temp1495, _temp1495, _temp1495 + 6u},(
struct _tagged_string){ _temp1498, _temp1498, _temp1498 + 6u},( struct
_tagged_string){ _temp1501, _temp1501, _temp1501 + 6u},( struct _tagged_string){
_temp1504, _temp1504, _temp1504 + 7u},( struct _tagged_string){ _temp1507,
_temp1507, _temp1507 + 6u},( struct _tagged_string){ _temp1510, _temp1510,
_temp1510 + 11u},( struct _tagged_string){ _temp1513, _temp1513, _temp1513 + 11u},(
struct _tagged_string){ _temp1516, _temp1516, _temp1516 + 11u},( struct
_tagged_string){ _temp1519, _temp1519, _temp1519 + 11u},( struct _tagged_string){
_temp1522, _temp1522, _temp1522 + 11u},( struct _tagged_string){ _temp1525,
_temp1525, _temp1525 + 12u},( struct _tagged_string){ _temp1528, _temp1528,
_temp1528 + 13u},( struct _tagged_string){ _temp1531, _temp1531, _temp1531 + 11u},(
struct _tagged_string){ _temp1534, _temp1534, _temp1534 + 11u},( struct
_tagged_string){ _temp1537, _temp1537, _temp1537 + 10u},( struct _tagged_string){
_temp1540, _temp1540, _temp1540 + 9u},( struct _tagged_string){ _temp1543,
_temp1543, _temp1543 + 11u},( struct _tagged_string){ _temp1546, _temp1546,
_temp1546 + 12u},( struct _tagged_string){ _temp1549, _temp1549, _temp1549 + 11u},(
struct _tagged_string){ _temp1552, _temp1552, _temp1552 + 17u},( struct
_tagged_string){ _temp1555, _temp1555, _temp1555 + 7u},( struct _tagged_string){
_temp1558, _temp1558, _temp1558 + 19u},( struct _tagged_string){ _temp1561,
_temp1561, _temp1561 + 18u},( struct _tagged_string){ _temp1564, _temp1564,
_temp1564 + 9u},( struct _tagged_string){ _temp1567, _temp1567, _temp1567 + 16u},(
struct _tagged_string){ _temp1570, _temp1570, _temp1570 + 18u},( struct
_tagged_string){ _temp1573, _temp1573, _temp1573 + 10u},( struct _tagged_string){
_temp1576, _temp1576, _temp1576 + 4u},( struct _tagged_string){ _temp1579,
_temp1579, _temp1579 + 4u},( struct _tagged_string){ _temp1582, _temp1582,
_temp1582 + 4u},( struct _tagged_string){ _temp1585, _temp1585, _temp1585 + 4u},(
struct _tagged_string){ _temp1588, _temp1588, _temp1588 + 4u},( struct
_tagged_string){ _temp1591, _temp1591, _temp1591 + 4u},( struct _tagged_string){
_temp1594, _temp1594, _temp1594 + 4u},( struct _tagged_string){ _temp1597,
_temp1597, _temp1597 + 4u},( struct _tagged_string){ _temp1600, _temp1600,
_temp1600 + 4u},( struct _tagged_string){ _temp1603, _temp1603, _temp1603 + 4u},(
struct _tagged_string){ _temp1606, _temp1606, _temp1606 + 4u},( struct
_tagged_string){ _temp1609, _temp1609, _temp1609 + 4u},( struct _tagged_string){
_temp1612, _temp1612, _temp1612 + 4u},( struct _tagged_string){ _temp1615,
_temp1615, _temp1615 + 4u},( struct _tagged_string){ _temp1618, _temp1618,
_temp1618 + 4u},( struct _tagged_string){ _temp1621, _temp1621, _temp1621 + 4u},(
struct _tagged_string){ _temp1624, _temp1624, _temp1624 + 4u},( struct
_tagged_string){ _temp1627, _temp1627, _temp1627 + 4u},( struct _tagged_string){
_temp1630, _temp1630, _temp1630 + 4u},( struct _tagged_string){ _temp1633,
_temp1633, _temp1633 + 4u},( struct _tagged_string){ _temp1636, _temp1636,
_temp1636 + 4u},( struct _tagged_string){ _temp1639, _temp1639, _temp1639 + 4u},(
struct _tagged_string){ _temp1642, _temp1642, _temp1642 + 4u},( struct
_tagged_string){ _temp1645, _temp1645, _temp1645 + 4u},( struct _tagged_string){
_temp1648, _temp1648, _temp1648 + 4u},( struct _tagged_string){ _temp1651,
_temp1651, _temp1651 + 4u},( struct _tagged_string){ _temp1654, _temp1654,
_temp1654 + 4u},( struct _tagged_string){ _temp1657, _temp1657, _temp1657 + 5u},(
struct _tagged_string){ _temp1660, _temp1660, _temp1660 + 17u},( struct
_tagged_string){ _temp1663, _temp1663, _temp1663 + 21u},( struct _tagged_string){
_temp1666, _temp1666, _temp1666 + 21u},( struct _tagged_string){ _temp1669,
_temp1669, _temp1669 + 20u},( struct _tagged_string){ _temp1672, _temp1672,
_temp1672 + 21u},( struct _tagged_string){ _temp1675, _temp1675, _temp1675 + 13u},(
struct _tagged_string){ _temp1678, _temp1678, _temp1678 + 15u},( struct
_tagged_string){ _temp1681, _temp1681, _temp1681 + 17u},( struct _tagged_string){
_temp1684, _temp1684, _temp1684 + 19u},( struct _tagged_string){ _temp1687,
_temp1687, _temp1687 + 12u},( struct _tagged_string){ _temp1690, _temp1690,
_temp1690 + 17u},( struct _tagged_string){ _temp1693, _temp1693, _temp1693 + 23u},(
struct _tagged_string){ _temp1696, _temp1696, _temp1696 + 24u},( struct
_tagged_string){ _temp1699, _temp1699, _temp1699 + 15u},( struct _tagged_string){
_temp1702, _temp1702, _temp1702 + 11u},( struct _tagged_string){ _temp1705,
_temp1705, _temp1705 + 15u},( struct _tagged_string){ _temp1708, _temp1708,
_temp1708 + 10u},( struct _tagged_string){ _temp1711, _temp1711, _temp1711 + 15u},(
struct _tagged_string){ _temp1714, _temp1714, _temp1714 + 5u},( struct
_tagged_string){ _temp1717, _temp1717, _temp1717 + 15u},( struct _tagged_string){
_temp1720, _temp1720, _temp1720 + 26u},( struct _tagged_string){ _temp1723,
_temp1723, _temp1723 + 16u},( struct _tagged_string){ _temp1726, _temp1726,
_temp1726 + 16u},( struct _tagged_string){ _temp1729, _temp1729, _temp1729 + 24u},(
struct _tagged_string){ _temp1732, _temp1732, _temp1732 + 25u},( struct
_tagged_string){ _temp1735, _temp1735, _temp1735 + 21u},( struct _tagged_string){
_temp1738, _temp1738, _temp1738 + 22u},( struct _tagged_string){ _temp1741,
_temp1741, _temp1741 + 16u},( struct _tagged_string){ _temp1744, _temp1744,
_temp1744 + 19u},( struct _tagged_string){ _temp1747, _temp1747, _temp1747 + 25u},(
struct _tagged_string){ _temp1750, _temp1750, _temp1750 + 23u},( struct
_tagged_string){ _temp1753, _temp1753, _temp1753 + 24u},( struct _tagged_string){
_temp1756, _temp1756, _temp1756 + 18u},( struct _tagged_string){ _temp1759,
_temp1759, _temp1759 + 15u},( struct _tagged_string){ _temp1762, _temp1762,
_temp1762 + 16u},( struct _tagged_string){ _temp1765, _temp1765, _temp1765 + 11u},(
struct _tagged_string){ _temp1768, _temp1768, _temp1768 + 11u},( struct
_tagged_string){ _temp1771, _temp1771, _temp1771 + 18u},( struct _tagged_string){
_temp1774, _temp1774, _temp1774 + 8u},( struct _tagged_string){ _temp1777,
_temp1777, _temp1777 + 13u},( struct _tagged_string){ _temp1780, _temp1780,
_temp1780 + 8u},( struct _tagged_string){ _temp1783, _temp1783, _temp1783 + 20u},(
struct _tagged_string){ _temp1786, _temp1786, _temp1786 + 20u},( struct
_tagged_string){ _temp1789, _temp1789, _temp1789 + 11u},( struct _tagged_string){
_temp1792, _temp1792, _temp1792 + 14u},( struct _tagged_string){ _temp1795,
_temp1795, _temp1795 + 11u},( struct _tagged_string){ _temp1798, _temp1798,
_temp1798 + 15u},( struct _tagged_string){ _temp1801, _temp1801, _temp1801 + 22u},(
struct _tagged_string){ _temp1804, _temp1804, _temp1804 + 16u},( struct
_tagged_string){ _temp1807, _temp1807, _temp1807 + 17u},( struct _tagged_string){
_temp1810, _temp1810, _temp1810 + 12u},( struct _tagged_string){ _temp1813,
_temp1813, _temp1813 + 17u},( struct _tagged_string){ _temp1816, _temp1816,
_temp1816 + 12u},( struct _tagged_string){ _temp1819, _temp1819, _temp1819 + 16u},(
struct _tagged_string){ _temp1822, _temp1822, _temp1822 + 11u},( struct
_tagged_string){ _temp1825, _temp1825, _temp1825 + 10u},( struct _tagged_string){
_temp1828, _temp1828, _temp1828 + 14u},( struct _tagged_string){ _temp1831,
_temp1831, _temp1831 + 15u},( struct _tagged_string){ _temp1834, _temp1834,
_temp1834 + 20u},( struct _tagged_string){ _temp1837, _temp1837, _temp1837 + 27u},(
struct _tagged_string){ _temp1840, _temp1840, _temp1840 + 10u},( struct
_tagged_string){ _temp1843, _temp1843, _temp1843 + 18u},( struct _tagged_string){
_temp1846, _temp1846, _temp1846 + 21u},( struct _tagged_string){ _temp1849,
_temp1849, _temp1849 + 19u},( struct _tagged_string){ _temp1852, _temp1852,
_temp1852 + 16u},( struct _tagged_string){ _temp1855, _temp1855, _temp1855 + 11u},(
struct _tagged_string){ _temp1858, _temp1858, _temp1858 + 20u},( struct
_tagged_string){ _temp1861, _temp1861, _temp1861 + 15u},( struct _tagged_string){
_temp1864, _temp1864, _temp1864 + 20u},( struct _tagged_string){ _temp1867,
_temp1867, _temp1867 + 15u},( struct _tagged_string){ _temp1870, _temp1870,
_temp1870 + 8u},( struct _tagged_string){ _temp1873, _temp1873, _temp1873 + 19u},(
struct _tagged_string){ _temp1876, _temp1876, _temp1876 + 20u},( struct
_tagged_string){ _temp1879, _temp1879, _temp1879 + 14u},( struct _tagged_string){
_temp1882, _temp1882, _temp1882 + 19u},( struct _tagged_string){ _temp1885,
_temp1885, _temp1885 + 20u},( struct _tagged_string){ _temp1888, _temp1888,
_temp1888 + 11u},( struct _tagged_string){ _temp1891, _temp1891, _temp1891 + 22u},(
struct _tagged_string){ _temp1894, _temp1894, _temp1894 + 20u},( struct
_tagged_string){ _temp1897, _temp1897, _temp1897 + 23u},( struct _tagged_string){
_temp1900, _temp1900, _temp1900 + 20u},( struct _tagged_string){ _temp1903,
_temp1903, _temp1903 + 22u},( struct _tagged_string){ _temp1906, _temp1906,
_temp1906 + 23u},( struct _tagged_string){ _temp1909, _temp1909, _temp1909 + 24u},(
struct _tagged_string){ _temp1912, _temp1912, _temp1912 + 24u},( struct
_tagged_string){ _temp1915, _temp1915, _temp1915 + 15u},( struct _tagged_string){
_temp1918, _temp1918, _temp1918 + 20u},( struct _tagged_string){ _temp1921,
_temp1921, _temp1921 + 22u},( struct _tagged_string){ _temp1924, _temp1924,
_temp1924 + 17u},( struct _tagged_string){ _temp1927, _temp1927, _temp1927 + 20u},(
struct _tagged_string){ _temp1930, _temp1930, _temp1930 + 26u},( struct
_tagged_string){ _temp1933, _temp1933, _temp1933 + 16u},( struct _tagged_string){
_temp1936, _temp1936, _temp1936 + 17u},( struct _tagged_string){ _temp1939,
_temp1939, _temp1939 + 14u},( struct _tagged_string){ _temp1942, _temp1942,
_temp1942 + 15u},( struct _tagged_string){ _temp1945, _temp1945, _temp1945 + 19u},(
struct _tagged_string){ _temp1948, _temp1948, _temp1948 + 19u},( struct
_tagged_string){ _temp1951, _temp1951, _temp1951 + 25u},( struct _tagged_string){
_temp1954, _temp1954, _temp1954 + 26u},( struct _tagged_string){ _temp1957,
_temp1957, _temp1957 + 9u},( struct _tagged_string){ _temp1960, _temp1960,
_temp1960 + 20u}}; static short Cyc_yyr1[ 378u]={ (short)0, (short)119, (short)120,
(short)120, (short)120, (short)120, (short)120, (short)120, (short)120, (short)121,
(short)121, (short)122, (short)122, (short)123, (short)123, (short)123, (short)123,
(short)124, (short)124, (short)125, (short)126, (short)127, (short)128, (short)129,
(short)129, (short)129, (short)130, (short)130, (short)131, (short)131, (short)131,
(short)131, (short)131, (short)131, (short)131, (short)131, (short)132, (short)132,
(short)132, (short)132, (short)132, (short)132, (short)132, (short)133, (short)133,
(short)134, (short)135, (short)135, (short)136, (short)136, (short)136, (short)136,
(short)137, (short)137, (short)137, (short)137, (short)137, (short)137, (short)137,
(short)137, (short)137, (short)137, (short)137, (short)137, (short)137, (short)137,
(short)137, (short)137, (short)138, (short)138, (short)139, (short)139, (short)139,
(short)140, (short)140, (short)140, (short)140, (short)140, (short)141, (short)141,
(short)141, (short)142, (short)142, (short)143, (short)144, (short)144, (short)145,
(short)146, (short)146, (short)147, (short)147, (short)148, (short)149, (short)149,
(short)149, (short)149, (short)150, (short)151, (short)151, (short)152, (short)152,
(short)152, (short)153, (short)153, (short)153, (short)153, (short)153, (short)154,
(short)154, (short)154, (short)154, (short)155, (short)155, (short)155, (short)156,
(short)156, (short)157, (short)157, (short)157, (short)157, (short)157, (short)157,
(short)157, (short)157, (short)157, (short)157, (short)157, (short)158, (short)158,
(short)158, (short)158, (short)159, (short)159, (short)159, (short)159, (short)159,
(short)160, (short)160, (short)160, (short)160, (short)161, (short)161, (short)162,
(short)162, (short)162, (short)163, (short)163, (short)164, (short)164, (short)164,
(short)164, (short)165, (short)165, (short)165, (short)165, (short)166, (short)166,
(short)167, (short)167, (short)167, (short)168, (short)169, (short)169, (short)170,
(short)170, (short)170, (short)170, (short)170, (short)171, (short)171, (short)171,
(short)171, (short)172, (short)173, (short)173, (short)174, (short)174, (short)175,
(short)175, (short)176, (short)176, (short)176, (short)176, (short)177, (short)177,
(short)178, (short)178, (short)178, (short)179, (short)179, (short)179, (short)179,
(short)179, (short)179, (short)179, (short)179, (short)179, (short)179, (short)179,
(short)179, (short)179, (short)179, (short)180, (short)180, (short)180, (short)180,
(short)180, (short)180, (short)180, (short)180, (short)181, (short)182, (short)182,
(short)183, (short)183, (short)184, (short)184, (short)185, (short)185, (short)185,
(short)186, (short)186, (short)186, (short)186, (short)187, (short)187, (short)187,
(short)187, (short)187, (short)187, (short)188, (short)188, (short)188, (short)188,
(short)188, (short)188, (short)188, (short)188, (short)188, (short)188, (short)188,
(short)188, (short)188, (short)188, (short)189, (short)189, (short)189, (short)189,
(short)189, (short)189, (short)189, (short)189, (short)190, (short)190, (short)190,
(short)190, (short)190, (short)190, (short)190, (short)190, (short)190, (short)190,
(short)190, (short)190, (short)190, (short)190, (short)191, (short)191, (short)192,
(short)192, (short)193, (short)193, (short)194, (short)195, (short)195, (short)196,
(short)196, (short)197, (short)197, (short)197, (short)198, (short)198, (short)198,
(short)198, (short)198, (short)198, (short)198, (short)198, (short)198, (short)198,
(short)198, (short)199, (short)199, (short)200, (short)201, (short)201, (short)202,
(short)202, (short)203, (short)203, (short)204, (short)204, (short)205, (short)205,
(short)206, (short)206, (short)206, (short)207, (short)207, (short)207, (short)207,
(short)207, (short)208, (short)208, (short)208, (short)209, (short)209, (short)209,
(short)210, (short)210, (short)210, (short)210, (short)211, (short)211, (short)212,
(short)212, (short)212, (short)212, (short)212, (short)212, (short)212, (short)212,
(short)212, (short)212, (short)212, (short)212, (short)212, (short)213, (short)213,
(short)213, (short)213, (short)213, (short)213, (short)214, (short)214, (short)214,
(short)215, (short)215, (short)215, (short)215, (short)215, (short)215, (short)215,
(short)215, (short)215, (short)215, (short)215, (short)215, (short)215, (short)215,
(short)216, (short)216, (short)216, (short)216, (short)216, (short)216, (short)216,
(short)216, (short)216, (short)217, (short)218, (short)218, (short)219, (short)219,
(short)219, (short)219, (short)220, (short)220}; static short Cyc_yyr2[ 378u]={
(short)0, (short)1, (short)1, (short)2, (short)3, (short)5, (short)3, (short)5,
(short)6, (short)0, (short)1, (short)1, (short)1, (short)2, (short)3, (short)3,
(short)4, (short)3, (short)4, (short)2, (short)1, (short)2, (short)1, (short)2,
(short)3, (short)5, (short)1, (short)2, (short)2, (short)3, (short)2, (short)3,
(short)2, (short)3, (short)2, (short)3, (short)1, (short)1, (short)1, (short)1,
(short)2, (short)1, (short)1, (short)0, (short)1, (short)6, (short)1, (short)3,
(short)1, (short)1, (short)4, (short)4, (short)1, (short)1, (short)1, (short)1,
(short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1,
(short)1, (short)3, (short)2, (short)4, (short)1, (short)1, (short)1, (short)1,
(short)1, (short)4, (short)6, (short)6, (short)3, (short)3, (short)0, (short)3,
(short)3, (short)1, (short)1, (short)1, (short)1, (short)2, (short)1, (short)1,
(short)3, (short)1, (short)3, (short)3, (short)2, (short)3, (short)2, (short)3,
(short)1, (short)1, (short)3, (short)1, (short)2, (short)3, (short)4, (short)6,
(short)3, (short)5, (short)2, (short)1, (short)2, (short)3, (short)3, (short)1,
(short)3, (short)5, (short)1, (short)2, (short)1, (short)3, (short)3, (short)4,
(short)4, (short)3, (short)5, (short)4, (short)4, (short)4, (short)2, (short)2,
(short)3, (short)3, (short)4, (short)1, (short)1, (short)4, (short)4, (short)1,
(short)0, (short)1, (short)3, (short)1, (short)1, (short)2, (short)1, (short)3,
(short)3, (short)1, (short)3, (short)2, (short)3, (short)1, (short)3, (short)1,
(short)3, (short)3, (short)5, (short)1, (short)3, (short)2, (short)1, (short)2,
(short)1, (short)1, (short)3, (short)1, (short)2, (short)3, (short)4, (short)8,
(short)1, (short)2, (short)3, (short)4, (short)2, (short)1, (short)2, (short)3,
(short)2, (short)1, (short)2, (short)1, (short)2, (short)3, (short)3, (short)1,
(short)3, (short)1, (short)1, (short)2, (short)3, (short)2, (short)3, (short)3,
(short)4, (short)2, (short)4, (short)3, (short)3, (short)5, (short)4, (short)4,
(short)4, (short)2, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1,
(short)2, (short)2, (short)3, (short)1, (short)2, (short)2, (short)3, (short)1,
(short)2, (short)1, (short)1, (short)1, (short)5, (short)7, (short)7, (short)6,
(short)0, (short)3, (short)4, (short)5, (short)6, (short)7, (short)5, (short)7,
(short)6, (short)7, (short)7, (short)8, (short)7, (short)8, (short)8, (short)9,
(short)6, (short)7, (short)7, (short)8, (short)3, (short)2, (short)2, (short)2,
(short)3, (short)2, (short)4, (short)5, (short)1, (short)3, (short)1, (short)2,
(short)1, (short)1, (short)1, (short)1, (short)5, (short)4, (short)4, (short)5,
(short)2, (short)2, (short)0, (short)1, (short)1, (short)3, (short)1, (short)2,
(short)1, (short)1, (short)3, (short)1, (short)3, (short)1, (short)3, (short)2,
(short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1,
(short)1, (short)1, (short)1, (short)1, (short)5, (short)1, (short)1, (short)3,
(short)1, (short)3, (short)1, (short)3, (short)1, (short)3, (short)1, (short)3,
(short)1, (short)3, (short)3, (short)1, (short)3, (short)3, (short)3, (short)3,
(short)1, (short)3, (short)3, (short)1, (short)3, (short)3, (short)1, (short)3,
(short)3, (short)3, (short)1, (short)4, (short)1, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)4, (short)2, (short)2, (short)4, (short)7,
(short)7, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1,
(short)1, (short)1, (short)1, (short)4, (short)3, (short)4, (short)3, (short)3,
(short)3, (short)3, (short)2, (short)2, (short)6, (short)7, (short)4, (short)4,
(short)1, (short)1, (short)1, (short)3, (short)2, (short)5, (short)4, (short)4,
(short)5, (short)1, (short)1, (short)3, (short)1, (short)1, (short)1, (short)1,
(short)1, (short)1}; static short Cyc_yydefact[ 711u]={ (short)0, (short)36,
(short)37, (short)38, (short)39, (short)41, (short)52, (short)54, (short)55,
(short)56, (short)57, (short)58, (short)59, (short)60, (short)61, (short)70,
(short)71, (short)72, (short)81, (short)82, (short)43, (short)0, (short)0,
(short)42, (short)0, (short)0, (short)0, (short)376, (short)64, (short)377,
(short)78, (short)0, (short)53, (short)0, (short)131, (short)132, (short)135,
(short)1, (short)2, (short)11, (short)0, (short)0, (short)12, (short)0, (short)43,
(short)43, (short)43, (short)62, (short)0, (short)63, (short)0, (short)114,
(short)0, (short)136, (short)116, (short)40, (short)0, (short)34, (short)44,
(short)0, (short)78, (short)258, (short)254, (short)257, (short)256, (short)0,
(short)252, (short)0, (short)0, (short)0, (short)0, (short)0, (short)259,
(short)19, (short)21, (short)106, (short)0, (short)0, (short)66, (short)0,
(short)0, (short)0, (short)0, (short)3, (short)0, (short)0, (short)0, (short)0,
(short)23, (short)0, (short)86, (short)87, (short)89, (short)28, (short)30,
(short)32, (short)78, (short)0, (short)78, (short)39, (short)0, (short)26,
(short)0, (short)0, (short)13, (short)0, (short)0, (short)0, (short)126, (short)115,
(short)137, (short)139, (short)127, (short)0, (short)0, (short)35, (short)0,
(short)107, (short)111, (short)104, (short)0, (short)266, (short)265, (short)255,
(short)264, (short)0, (short)0, (short)0, (short)68, (short)69, (short)65,
(short)0, (short)43, (short)43, (short)177, (short)179, (short)183, (short)0,
(short)117, (short)158, (short)0, (short)155, (short)0, (short)375, (short)0,
(short)0, (short)0, (short)0, (short)337, (short)338, (short)339, (short)340,
(short)341, (short)342, (short)0, (short)0, (short)0, (short)372, (short)362,
(short)373, (short)374, (short)0, (short)0, (short)0, (short)0, (short)345,
(short)0, (short)343, (short)344, (short)0, (short)277, (short)291, (short)294,
(short)296, (short)298, (short)300, (short)302, (short)304, (short)307, (short)312,
(short)315, (short)318, (short)322, (short)0, (short)0, (short)324, (short)346,
(short)361, (short)360, (short)0, (short)4, (short)0, (short)6, (short)0,
(short)24, (short)0, (short)0, (short)0, (short)14, (short)29, (short)31,
(short)33, (short)77, (short)0, (short)83, (short)84, (short)0, (short)76,
(short)40, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)376, (short)211,
(short)213, (short)0, (short)219, (short)217, (short)0, (short)218, (short)202,
(short)203, (short)204, (short)0, (short)215, (short)205, (short)206, (short)207,
(short)0, (short)275, (short)27, (short)15, (short)89, (short)161, (short)0,
(short)121, (short)0, (short)142, (short)0, (short)160, (short)0, (short)118,
(short)0, (short)0, (short)140, (short)129, (short)128, (short)0, (short)0,
(short)102, (short)108, (short)0, (short)0, (short)0, (short)0, (short)253,
(short)268, (short)0, (short)267, (short)0, (short)0, (short)266, (short)0,
(short)151, (short)180, (short)0, (short)92, (short)94, (short)0, (short)0,
(short)185, (short)178, (short)186, (short)0, (short)80, (short)0, (short)79,
(short)0, (short)157, (short)185, (short)159, (short)67, (short)0, (short)0,
(short)332, (short)0, (short)333, (short)0, (short)279, (short)163, (short)0,
(short)0, (short)0, (short)325, (short)326, (short)0, (short)0, (short)0,
(short)0, (short)328, (short)322, (short)329, (short)327, (short)133, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)281, (short)282, (short)283, (short)284, (short)285,
(short)286, (short)287, (short)288, (short)289, (short)290, (short)280, (short)0,
(short)0, (short)330, (short)0, (short)354, (short)355, (short)0, (short)0,
(short)0, (short)364, (short)0, (short)0, (short)134, (short)20, (short)9,
(short)22, (short)9, (short)88, (short)90, (short)16, (short)0, (short)73,
(short)85, (short)0, (short)0, (short)96, (short)97, (short)99, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)245, (short)246,
(short)247, (short)0, (short)0, (short)249, (short)0, (short)208, (short)209,
(short)0, (short)0, (short)89, (short)214, (short)216, (short)212, (short)0,
(short)149, (short)0, (short)0, (short)145, (short)120, (short)0, (short)0,
(short)123, (short)0, (short)125, (short)124, (short)119, (short)138, (short)141,
(short)130, (short)9, (short)49, (short)48, (short)0, (short)46, (short)110,
(short)109, (short)293, (short)112, (short)0, (short)0, (short)261, (short)0,
(short)25, (short)262, (short)0, (short)0, (short)0, (short)0, (short)173,
(short)270, (short)273, (short)0, (short)272, (short)0, (short)105, (short)0,
(short)0, (short)181, (short)93, (short)95, (short)0, (short)193, (short)0,
(short)0, (short)189, (short)0, (short)187, (short)0, (short)0, (short)0,
(short)201, (short)182, (short)184, (short)156, (short)0, (short)0, (short)0,
(short)164, (short)168, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)363, (short)370, (short)0, (short)369,
(short)295, (short)0, (short)297, (short)299, (short)301, (short)303, (short)305,
(short)306, (short)310, (short)311, (short)308, (short)309, (short)313, (short)314,
(short)316, (short)317, (short)319, (short)320, (short)321, (short)278, (short)0,
(short)352, (short)353, (short)348, (short)0, (short)0, (short)350, (short)351,
(short)0, (short)0, (short)10, (short)5, (short)7, (short)0, (short)100, (short)91,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)244, (short)248, (short)0, (short)0, (short)0,
(short)210, (short)0, (short)17, (short)276, (short)0, (short)147, (short)0,
(short)122, (short)0, (short)144, (short)143, (short)162, (short)8, (short)0,
(short)0, (short)0, (short)0, (short)103, (short)269, (short)0, (short)176,
(short)271, (short)172, (short)174, (short)263, (short)0, (short)260, (short)153,
(short)152, (short)0, (short)195, (short)188, (short)191, (short)0, (short)196,
(short)0, (short)0, (short)190, (short)0, (short)331, (short)0, (short)0,
(short)165, (short)0, (short)169, (short)358, (short)359, (short)0, (short)0,
(short)0, (short)0, (short)323, (short)366, (short)0, (short)0, (short)334,
(short)349, (short)347, (short)367, (short)0, (short)0, (short)75, (short)98,
(short)101, (short)74, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)224, (short)250, (short)0, (short)18,
(short)150, (short)148, (short)146, (short)0, (short)0, (short)45, (short)47,
(short)113, (short)175, (short)274, (short)0, (short)194, (short)0, (short)198,
(short)200, (short)199, (short)192, (short)0, (short)166, (short)170, (short)0,
(short)0, (short)0, (short)368, (short)0, (short)371, (short)292, (short)365,
(short)220, (short)224, (short)230, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)251, (short)50, (short)51, (short)154, (short)197, (short)0, (short)171,
(short)0, (short)0, (short)356, (short)0, (short)0, (short)0, (short)0, (short)232,
(short)0, (short)0, (short)0, (short)240, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)223, (short)0, (short)335,
(short)336, (short)357, (short)221, (short)222, (short)231, (short)233, (short)234,
(short)0, (short)242, (short)241, (short)0, (short)236, (short)0, (short)0,
(short)0, (short)0, (short)224, (short)225, (short)0, (short)235, (short)243,
(short)237, (short)238, (short)0, (short)0, (short)224, (short)226, (short)167,
(short)239, (short)224, (short)227, (short)224, (short)228, (short)229, (short)0,
(short)0, (short)0}; static short Cyc_yydefgoto[ 102u]={ (short)708, (short)502,
(short)503, (short)38, (short)39, (short)226, (short)40, (short)358, (short)41,
(short)360, (short)42, (short)197, (short)43, (short)44, (short)57, (short)58,
(short)413, (short)414, (short)45, (short)130, (short)46, (short)47, (short)78,
(short)48, (short)203, (short)204, (short)89, (short)90, (short)91, (short)205,
(short)139, (short)368, (short)369, (short)370, (short)49, (short)116, (short)117,
(short)50, (short)51, (short)52, (short)53, (short)112, (short)256, (short)443,
(short)397, (short)398, (short)275, (short)247, (short)141, (short)248, (short)249,
(short)459, (short)460, (short)461, (short)428, (short)429, (short)135, (short)136,
(short)137, (short)444, (short)282, (short)229, (short)230, (short)231, (short)232,
(short)233, (short)234, (short)235, (short)643, (short)236, (short)237, (short)266,
(short)267, (short)268, (short)431, (short)432, (short)433, (short)238, (short)239,
(short)344, (short)170, (short)418, (short)171, (short)172, (short)173, (short)174,
(short)175, (short)176, (short)177, (short)178, (short)179, (short)180, (short)181,
(short)182, (short)183, (short)184, (short)185, (short)186, (short)470, (short)471,
(short)187, (short)188}; static short Cyc_yypact[ 711u]={ (short)820, - (short)-32768,
- (short)-32768, - (short)-32768, (short)8, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)61, (short)174,
(short)430, - (short)-32768, (short)29, (short)31, (short)29, - (short)-32768,
(short)107, - (short)-32768, (short)71, (short)565, - (short)-32768, (short)95,
(short)118, (short)129, - (short)-32768, - (short)-32768, (short)820, - (short)-32768,
(short)111, (short)254, - (short)-32768, (short)173, - (short)61, - (short)61, -
(short)61, - (short)-32768, (short)290, - (short)-32768, (short)1830, (short)137,
(short)164, - (short)70, - (short)-32768, (short)131, (short)139, (short)1879, -
(short)-32768, (short)29, (short)71, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, (short)430, - (short)-32768, (short)178, (short)175, (short)203,
(short)430, (short)209, (short)89, - (short)-32768, - (short)-32768, (short)219,
(short)119, (short)2080, - (short)-32768, (short)217, (short)2124, (short)3742,
(short)3742, - (short)-32768, (short)820, (short)820, (short)820, (short)820, -
(short)-32768, (short)229, (short)228, - (short)-32768, (short)1781, (short)1879,
(short)1879, (short)1879, (short)71, (short)2124, (short)71, (short)243, (short)1061,
- (short)-32768, (short)1830, (short)173, - (short)-32768, (short)1934, (short)2080,
(short)677, - (short)-32768, (short)137, (short)276, - (short)-32768, (short)90,
(short)820, (short)281, - (short)-32768, (short)278, (short)45, (short)240,
(short)289, (short)298, (short)430, - (short)-32768, - (short)-32768, - (short)-32768,
(short)3742, (short)144, (short)29, - (short)-32768, - (short)-32768, - (short)-32768,
(short)47, - (short)61, - (short)61, (short)246, - (short)-32768, (short)330,
(short)40, - (short)-32768, (short)358, (short)277, - (short)-32768, (short)3910,
- (short)-32768, (short)3994, (short)2482, (short)328, (short)346, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
(short)348, (short)3994, (short)3994, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)-32768, (short)1404, (short)355, (short)4078, (short)4078,
- (short)-32768, (short)4078, - (short)-32768, - (short)-32768, (short)362, -
(short)-32768, - (short)22, (short)351, (short)360, (short)363, (short)369,
(short)326, (short)69, (short)352, - (short)58, (short)204, - (short)-32768,
(short)292, (short)394, (short)4078, (short)189, - (short)-32768, - (short)-32768,
(short)106, (short)401, - (short)-32768, (short)411, - (short)-32768, (short)414,
- (short)-32768, (short)565, (short)2482, (short)1830, - (short)-32768, -
(short)-32768, - (short)-32768, - (short)-32768, (short)406, (short)418, (short)2124,
- (short)-32768, (short)462, (short)422, - (short)-32768, (short)425, (short)427,
(short)438, (short)1955, (short)447, (short)435, (short)460, (short)467, (short)2566,
(short)1955, (short)62, (short)1955, (short)1955, (short)464, - (short)-32768, -
(short)-32768, (short)477, - (short)-32768, - (short)-32768, (short)173, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)485,
(short)1177, - (short)-32768, - (short)-32768, - (short)-32768, (short)134, -
(short)-32768, - (short)-32768, - (short)-32768, (short)490, - (short)-32768, -
(short)3, - (short)-32768, (short)489, (short)179, (short)492, (short)493,
(short)42, - (short)-32768, (short)487, (short)119, - (short)-32768, - (short)-32768,
(short)90, (short)499, (short)16, - (short)-32768, (short)29, (short)29, (short)4078,
(short)498, (short)29, - (short)-32768, - (short)-32768, (short)503, (short)497,
(short)241, (short)349, (short)430, (short)502, - (short)1, - (short)-32768,
(short)507, (short)2124, (short)2124, (short)1675, (short)2650, - (short)55, -
(short)-32768, (short)207, - (short)3, - (short)-32768, (short)2080, - (short)-32768,
(short)1573, - (short)-32768, (short)70, - (short)-32768, - (short)-32768,
(short)2124, (short)1404, - (short)-32768, (short)1404, - (short)-32768, (short)2146,
- (short)-32768, - (short)-32768, (short)3742, (short)1631, (short)567, -
(short)-32768, - (short)-32768, (short)1177, (short)506, (short)325, (short)3742,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
(short)4078, (short)3742, (short)4078, (short)4078, (short)4078, (short)4078,
(short)4078, (short)4078, (short)4078, (short)4078, (short)4078, (short)4078,
(short)4078, (short)4078, (short)4078, (short)4078, (short)4078, (short)4078,
(short)4078, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, (short)3742, (short)3742, - (short)-32768,
(short)171, - (short)-32768, - (short)-32768, (short)2734, (short)3742, (short)193,
- (short)-32768, (short)2398, (short)504, - (short)-32768, - (short)-32768,
(short)820, - (short)-32768, (short)820, - (short)-32768, - (short)-32768, -
(short)-32768, (short)2124, - (short)-32768, - (short)-32768, (short)4078,
(short)517, (short)513, - (short)-32768, (short)509, (short)2124, (short)3742,
(short)3742, (short)3742, (short)587, (short)1293, (short)525, - (short)-32768,
- (short)-32768, - (short)-32768, (short)251, (short)577, - (short)-32768,
(short)2818, - (short)-32768, - (short)-32768, (short)1955, (short)1515, (short)1781,
- (short)-32768, - (short)-32768, - (short)-32768, (short)3742, (short)537,
(short)267, (short)524, (short)514, - (short)-32768, - (short)3, (short)1459, -
(short)-32768, (short)539, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)-32768, - (short)-32768, (short)820, - (short)-32768,
(short)529, (short)530, (short)528, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, (short)2124, (short)535, - (short)-32768, (short)430, - (short)-32768,
- (short)-32768, (short)4078, (short)548, (short)430, (short)9, - (short)-32768,
- (short)-32768, - (short)-32768, (short)538, (short)536, (short)540, - (short)-32768,
(short)119, (short)547, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)3, - (short)-32768, (short)541, (short)549, - (short)-32768, (short)542,
(short)207, (short)2066, (short)2080, (short)2902, - (short)-32768, - (short)-32768,
(short)330, - (short)-32768, (short)559, (short)561, (short)562, - (short)-32768,
- (short)-32768, (short)197, (short)2482, (short)331, (short)568, (short)565,
(short)566, (short)573, (short)3826, - (short)-32768, - (short)-32768, (short)572,
(short)576, (short)351, - (short)32, (short)360, (short)363, (short)369, (short)326,
(short)69, (short)69, (short)352, (short)352, (short)352, (short)352, - (short)58,
- (short)58, (short)204, (short)204, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, (short)578, - (short)-32768, - (short)-32768, - (short)-32768,
(short)579, (short)196, - (short)-32768, - (short)-32768, (short)211, (short)2080,
- (short)-32768, - (short)-32768, - (short)-32768, (short)584, - (short)-32768,
- (short)-32768, (short)462, (short)4078, (short)585, (short)371, (short)374,
(short)378, (short)586, (short)2986, (short)3070, (short)308, - (short)-32768, -
(short)-32768, (short)590, (short)596, (short)592, - (short)-32768, (short)1830,
- (short)-32768, - (short)-32768, (short)119, - (short)-32768, (short)597, -
(short)-32768, - (short)3, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)-32768, (short)395, (short)593, (short)16, (short)400, - (short)-32768, -
(short)-32768, (short)589, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)-32768, (short)4114, - (short)-32768, (short)594, -
(short)-32768, (short)598, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)3, - (short)-32768, (short)599, (short)54, - (short)-32768, (short)595,
(short)606, (short)606, (short)600, - (short)-32768, (short)2230, - (short)-32768,
- (short)-32768, - (short)-32768, (short)1830, (short)1348, (short)605, (short)2398,
- (short)-32768, - (short)-32768, (short)3742, (short)4078, - (short)-32768, -
(short)-32768, - (short)-32768, - (short)-32768, (short)2398, (short)307, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)1955,
(short)610, (short)1955, (short)3742, (short)3154, (short)310, (short)3238,
(short)312, (short)3322, (short)470, - (short)-32768, (short)612, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, (short)608, (short)609, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
(short)547, - (short)-32768, (short)611, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)-32768, (short)3742, - (short)-32768, - (short)-32768,
(short)2482, (short)614, (short)616, - (short)-32768, (short)318, - (short)-32768,
- (short)-32768, - (short)-32768, (short)681, (short)470, - (short)-32768,
(short)405, (short)1955, (short)434, (short)3406, (short)1955, (short)441,
(short)3490, (short)3574, (short)315, (short)430, (short)615, (short)621, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
(short)12, - (short)-32768, (short)620, (short)623, - (short)-32768, (short)2314,
(short)1955, (short)627, (short)617, - (short)-32768, (short)1955, (short)1955,
(short)450, - (short)-32768, (short)1955, (short)1955, (short)463, (short)1955,
(short)465, (short)3658, (short)43, (short)1177, - (short)-32768, (short)3742, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, (short)1955, - (short)-32768,
- (short)-32768, (short)1955, - (short)-32768, (short)1955, (short)1955, (short)478,
(short)3742, (short)927, - (short)-32768, (short)323, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, (short)1955, (short)103, (short)470, - (short)-32768,
- (short)-32768, - (short)-32768, (short)927, - (short)-32768, (short)470, -
(short)-32768, - (short)-32768, (short)723, (short)724, - (short)-32768}; static
short Cyc_yypgoto[ 102u]={ - (short)-32768, (short)92, - (short)328, - (short)-32768,
(short)440, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)46, - (short)39, - (short)50, - (short)-32768, -
(short)7, - (short)48, (short)198, - (short)-32768, (short)321, - (short)241,
(short)279, - (short)-32768, (short)121, - (short)-32768, - (short)338, -
(short)-32768, - (short)-32768, - (short)-32768, (short)544, (short)543, (short)39,
- (short)-32768, - (short)-32768, (short)234, - (short)-32768, - (short)99, -
(short)-32768, - (short)12, - (short)37, - (short)64, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)100, - (short)376, (short)461, - (short)382, - (short)59,
- (short)270, - (short)-32768, - (short)-32768, - (short)132, - (short)346, -
(short)264, - (short)-32768, (short)317, - (short)145, (short)458, - (short)104,
(short)34, - (short)120, (short)0, - (short)-32768, - (short)-32768, - (short)33,
- (short)233, - (short)-32768, - (short)-32768, - (short)603, - (short)-32768, -
(short)-32768, - (short)2, (short)475, - (short)-32768, (short)202, - (short)-32768,
- (short)-32768, - (short)85, - (short)72, - (short)-32768, - (short)239, -
(short)342, - (short)-32768, (short)437, (short)433, (short)436, (short)439,
(short)446, (short)257, (short)162, (short)201, (short)252, - (short)86, (short)486,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)256,
- (short)-32768, - (short)-32768, (short)36}; static short Cyc_yytable[ 4227u]={
(short)103, (short)392, (short)250, (short)108, (short)101, (short)246, (short)427,
(short)115, (short)500, (short)169, (short)189, (short)102, (short)407, (short)298,
(short)529, (short)109, (short)306, (short)104, (short)110, (short)79, (short)71,
(short)140, (short)454, (short)417, (short)532, (short)506, (short)505, (short)656,
(short)272, (short)111, (short)56, (short)92, (short)504, (short)411, (short)510,
(short)252, (short)54, (short)93, (short)94, (short)95, (short)269, (short)278,
(short)103, (short)199, (short)200, (short)201, (short)101, (short)314, (short)255,
(short)279, (short)228, (short)328, (short)103, (short)329, (short)227, (short)551,
(short)240, (short)60, (short)72, (short)198, (short)73, (short)108, (short)75,
(short)120, (short)362, (short)552, (short)394, (short)54, (short)124, (short)241,
(short)280, (short)577, (short)466, (short)299, (short)54, (short)289, (short)307,
(short)309, (short)311, (short)54, (short)312, (short)436, (short)535, (short)542,
(short)98, (short)395, (short)315, (short)700, (short)54, (short)492, (short)396,
(short)242, (short)37, (short)55, (short)496, (short)118, (short)704, (short)437,
(short)346, (short)412, (short)706, (short)72, (short)707, (short)284, (short)545,
(short)404, (short)72, (short)15, (short)16, (short)17, (short)394, (short)689,
(short)27, (short)425, (short)74, (short)672, (short)134, (short)615, (short)29,
(short)426, (short)54, (short)54, (short)54, (short)54, (short)299, (short)276,
(short)277, (short)288, (short)417, (short)522, (short)83, (short)454, (short)382,
(short)322, (short)323, (short)273, (short)206, (short)260, (short)285, (short)54,
(short)285, (short)274, (short)286, (short)261, (short)405, (short)134, (short)690,
(short)103, (short)455, (short)54, (short)456, (short)240, (short)285, (short)27,
(short)384, (short)603, (short)616, (short)72, (short)385, (short)29, (short)447,
(short)415, (short)416, (short)118, (short)363, (short)420, (short)287, (short)586,
(short)281, (short)447, (short)324, (short)325, (short)77, (short)290, (short)279,
(short)54, (short)190, (short)191, (short)192, (short)193, (short)613, (short)119,
- (short)78, (short)242, (short)228, - (short)78, (short)417, (short)353,
(short)227, (short)76, (short)77, (short)80, (short)409, (short)126, (short)371,
(short)550, (short)34, (short)35, (short)36, (short)354, (short)134, (short)394,
(short)128, (short)84, (short)85, (short)257, (short)703, (short)446, (short)307,
(short)129, (short)307, (short)81, (short)376, (short)355, (short)280, (short)462,
(short)390, (short)202, (short)383, (short)207, (short)386, (short)387, (short)82,
(short)289, (short)113, (short)299, (short)393, (short)625, (short)56, (short)647,
(short)473, (short)54, (short)394, (short)105, (short)451, (short)114, (short)469,
(short)270, (short)106, (short)263, (short)271, (short)107, (short)54, (short)206,
(short)488, (short)489, (short)490, (short)27, (short)347, (short)348, (short)349,
(short)464, (short)109, (short)29, (short)493, (short)228, (short)27, (short)27,
(short)122, (short)227, (short)31, (short)494, (short)29, (short)29, (short)54,
(short)88, (short)497, (short)59, (short)430, (short)31, (short)417, (short)400,
(short)491, (short)469, (short)121, (short)79, (short)498, (short)401, (short)469,
(short)34, (short)35, (short)36, (short)299, (short)499, (short)427, (short)350,
(short)601, (short)123, (short)511, (short)512, (short)513, (short)565, (short)517,
(short)351, (short)394, (short)566, (short)118, (short)118, (short)56, (short)352,
(short)118, (short)580, (short)621, (short)448, (short)125, (short)581, (short)72,
(short)72, (short)449, (short)582, (short)330, (short)450, (short)127, (short)469,
(short)138, (short)439, (short)440, (short)469, (short)621, (short)331, (short)332,
(short)194, (short)526, (short)54, (short)134, (short)54, (short)195, (short)103,
(short)208, (short)567, (short)140, (short)516, (short)134, (short)423, (short)134,
(short)262, - (short)78, (short)54, (short)627, (short)394, (short)103, (short)77,
(short)278, (short)519, (short)101, (short)559, (short)86, (short)87, (short)558,
(short)394, (short)279, (short)524, (short)34, (short)35, (short)36, (short)273,
(short)133, (short)525, (short)253, (short)133, (short)539, (short)528, (short)333,
(short)334, (short)335, (short)336, (short)337, (short)338, (short)339, (short)340,
(short)341, (short)342, (short)259, (short)27, (short)291, (short)292, (short)133,
(short)258, (short)561, (short)29, (short)96, (short)574, (short)264, (short)97,
(short)133, (short)133, (short)61, (short)343, (short)523, (short)299, (short)621,
(short)254, (short)320, (short)321, (short)54, (short)265, (short)54, (short)583,
(short)132, (short)451, (short)596, (short)132, (short)635, (short)206, (short)638,
(short)285, (short)394, (short)668, (short)394, (short)628, (short)394, (short)206,
(short)653, (short)394, (short)326, (short)327, (short)654, (short)701, (short)132,
(short)316, (short)541, (short)394, (short)468, (short)394, (short)300, (short)544,
(short)132, (short)132, (short)568, (short)394, (short)593, (short)595, (short)27,
(short)62, (short)620, (short)63, (short)64, (short)691, (short)29, (short)283,
(short)133, (short)27, (short)301, (short)424, (short)302, (short)65, (short)54,
(short)29, (short)66, (short)67, (short)620, (short)308, (short)570, (short)425,
(short)287, (short)68, (short)313, (short)699, (short)72, (short)426, (short)69,
(short)70, (short)279, (short)72, (short)34, (short)35, (short)36, (short)61,
(short)588, (short)394, (short)705, (short)589, (short)394, (short)317, (short)103,
(short)590, (short)394, (short)318, (short)240, (short)604, (short)605, (short)319,
(short)132, (short)133, (short)480, (short)481, (short)482, (short)483, (short)134,
(short)650, (short)345, (short)600, (short)641, (short)642, (short)299, (short)356,
(short)371, (short)608, (short)292, (short)364, (short)54, (short)299, (short)657,
(short)394, (short)626, (short)357, (short)632, (short)634, (short)359, (short)637,
(short)299, (short)640, (short)365, (short)27, (short)62, (short)372, (short)63,
(short)64, (short)378, (short)29, (short)103, (short)373, (short)620, (short)374,
(short)101, (short)132, (short)65, (short)484, (short)485, (short)66, (short)67,
(short)659, (short)394, (short)649, (short)375, (short)408, (short)68, (short)198,
(short)663, (short)394, (short)134, (short)69, (short)70, (short)377, (short)54,
(short)27, (short)430, (short)681, (short)394, (short)299, (short)661, (short)29,
(short)379, (short)665, (short)667, (short)133, (short)133, (short)133, (short)31,
(short)380, (short)684, (short)394, (short)686, (short)394, (short)133, (short)367,
(short)133, (short)388, (short)34, (short)35, (short)36, (short)133, (short)133,
(short)389, (short)133, (short)697, (short)394, (short)478, (short)479, (short)391,
(short)486, (short)487, (short)299, (short)688, (short)72, (short)196, (short)399,
(short)692, (short)629, (short)402, (short)631, (short)403, (short)406, (short)410,
(short)419, (short)422, (short)435, (short)132, (short)132, (short)132, (short)421,
(short)438, (short)465, (short)467, (short)698, (short)501, (short)132, (short)623,
(short)132, (short)507, (short)622, (short)508, (short)509, (short)132, (short)132,
(short)514, (short)132, (short)518, (short)520, (short)527, (short)228, (short)530,
(short)534, (short)531, (short)227, (short)536, (short)538, (short)537, (short)294,
(short)540, (short)296, (short)543, (short)547, (short)658, (short)548, (short)273,
(short)662, (short)549, (short)553, (short)669, (short)228, (short)303, (short)304,
(short)133, (short)227, (short)564, (short)554, (short)555, (short)27, (short)310,
(short)310, (short)133, (short)310, (short)228, (short)29, (short)676, (short)562,
(short)227, (short)563, (short)679, (short)680, (short)31, (short)571, (short)682,
(short)683, (short)569, (short)685, (short)572, (short)133, (short)575, (short)310,
(short)34, (short)35, (short)36, (short)576, (short)578, (short)579, (short)72,
(short)584, (short)587, (short)133, (short)693, (short)591, (short)597, (short)694,
(short)132, (short)695, (short)696, (short)598, (short)599, (short)606, (short)602,
(short)611, (short)132, (short)609, (short)612, (short)614, (short)702, (short)133,
(short)573, (short)617, (short)618, (short)624, (short)630, (short)644, (short)645,
(short)646, (short)655, (short)648, (short)678, (short)132, (short)651, (short)142,
(short)652, (short)143, (short)671, (short)144, (short)673, (short)670, (short)145,
(short)674, (short)677, (short)132, (short)709, (short)710, (short)146, (short)147,
(short)133, (short)133, (short)148, (short)149, (short)150, (short)151, (short)152,
(short)153, (short)154, (short)607, (short)155, (short)156, (short)361, (short)132,
(short)463, (short)585, (short)453, (short)452, (short)546, (short)434, (short)366,
(short)310, (short)474, (short)610, (short)472, (short)0, (short)475, (short)0,
(short)0, (short)0, (short)476, (short)0, (short)0, (short)27, (short)157,
(short)158, (short)159, (short)160, (short)477, (short)29, (short)0, (short)0,
(short)132, (short)132, (short)0, (short)0, (short)161, (short)0, (short)0,
(short)0, (short)162, (short)0, (short)0, (short)133, (short)0, (short)251,
(short)163, (short)0, (short)0, (short)164, (short)0, (short)165, (short)166,
(short)0, (short)0, (short)0, (short)0, (short)167, (short)168, (short)0,
(short)0, (short)0, (short)0, (short)310, (short)0, (short)310, (short)310,
(short)310, (short)310, (short)310, (short)310, (short)310, (short)310, (short)310,
(short)310, (short)310, (short)310, (short)310, (short)310, (short)310, (short)310,
(short)310, (short)0, (short)0, (short)0, (short)132, (short)1, (short)2,
(short)3, (short)4, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)0, (short)0, (short)20, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)133, (short)0, (short)0, (short)310, (short)0,
(short)0, (short)21, (short)0, (short)22, (short)0, (short)0, (short)0, (short)0,
(short)23, (short)0, (short)24, (short)25, (short)26, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)132, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)27, (short)0, (short)0, (short)0, (short)0, (short)28, (short)29,
(short)30, (short)310, (short)0, (short)0, (short)0, (short)0, (short)31,
(short)0, (short)0, (short)32, (short)33, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)34, (short)35, (short)36, (short)0, (short)1, (short)2, (short)3,
(short)99, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)641, (short)642, (short)20, (short)209, (short)310, (short)210,
(short)211, (short)212, (short)213, (short)214, (short)215, (short)216, (short)217,
(short)142, (short)21, (short)143, (short)22, (short)144, (short)218, (short)0,
(short)145, (short)23, (short)219, (short)0, (short)0, (short)26, (short)146,
(short)147, (short)220, (short)221, (short)148, (short)149, (short)150, (short)151,
(short)152, (short)153, (short)154, (short)0, (short)155, (short)156, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)310, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)222, (short)157, (short)158,
(short)159, (short)160, (short)28, (short)29, (short)30, (short)0, (short)223,
(short)100, (short)0, (short)0, (short)161, (short)0, (short)0, (short)32,
(short)225, (short)0, (short)0, (short)0, (short)0, (short)0, (short)163,
(short)0, (short)0, (short)164, (short)0, (short)165, (short)166, (short)0,
(short)0, (short)0, (short)0, (short)167, (short)168, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)310,
(short)1, (short)2, (short)3, (short)99, (short)5, (short)6, (short)7, (short)8,
(short)9, (short)10, (short)11, (short)12, (short)13, (short)14, (short)15,
(short)16, (short)17, (short)18, (short)19, (short)0, (short)0, (short)20,
(short)209, (short)0, (short)210, (short)211, (short)212, (short)213, (short)214,
(short)215, (short)216, (short)217, (short)142, (short)21, (short)143, (short)22,
(short)144, (short)218, (short)0, (short)145, (short)23, (short)219, (short)0,
(short)0, (short)26, (short)146, (short)147, (short)220, (short)221, (short)148,
(short)149, (short)150, (short)151, (short)152, (short)153, (short)154, (short)0,
(short)155, (short)156, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)222, (short)157, (short)158, (short)159, (short)160, (short)28, (short)29,
(short)30, (short)0, (short)223, (short)100, (short)224, (short)0, (short)161,
(short)0, (short)0, (short)32, (short)225, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)163, (short)0, (short)0, (short)164, (short)0, (short)165,
(short)166, (short)0, (short)0, (short)0, (short)0, (short)167, (short)168,
(short)1, (short)2, (short)3, (short)99, (short)5, (short)6, (short)7, (short)8,
(short)9, (short)10, (short)11, (short)12, (short)13, (short)14, (short)15,
(short)16, (short)17, (short)18, (short)19, (short)0, (short)0, (short)20,
(short)209, (short)0, (short)210, (short)211, (short)212, (short)213, (short)214,
(short)215, (short)216, (short)217, (short)142, (short)21, (short)143, (short)22,
(short)144, (short)218, (short)0, (short)145, (short)23, (short)219, (short)0,
(short)0, (short)26, (short)146, (short)147, (short)220, (short)221, (short)148,
(short)149, (short)150, (short)151, (short)152, (short)153, (short)154, (short)0,
(short)155, (short)156, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)222, (short)157, (short)158, (short)159, (short)160, (short)28, (short)29,
(short)30, (short)0, (short)223, (short)100, (short)0, (short)0, (short)161,
(short)0, (short)0, (short)32, (short)225, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)163, (short)0, (short)0, (short)164, (short)0, (short)165,
(short)166, (short)0, (short)0, (short)0, (short)0, (short)167, (short)168,
(short)1, (short)2, (short)3, (short)99, (short)5, (short)6, (short)7, (short)8,
(short)9, (short)10, (short)11, (short)12, (short)13, (short)14, (short)15,
(short)16, (short)17, (short)18, (short)19, (short)0, (short)0, (short)20,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)142, (short)21, (short)143, (short)22, (short)144,
(short)0, (short)0, (short)145, (short)23, (short)0, (short)0, (short)0, (short)26,
(short)146, (short)147, (short)0, (short)0, (short)148, (short)149, (short)150,
(short)151, (short)152, (short)153, (short)154, (short)0, (short)155, (short)156,
(short)0, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)27, (short)157,
(short)158, (short)159, (short)160, (short)28, (short)29, (short)30, (short)21,
(short)515, (short)0, (short)0, (short)0, (short)161, (short)0, (short)0,
(short)32, (short)225, (short)0, (short)26, (short)0, (short)0, (short)0,
(short)163, (short)0, (short)0, (short)164, (short)0, (short)165, (short)166,
(short)0, (short)0, (short)0, (short)0, (short)167, (short)168, (short)6,
(short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)27, (short)0, (short)0, (short)0,
(short)0, (short)28, (short)29, (short)30, (short)142, (short)21, (short)143,
(short)0, (short)144, (short)0, (short)0, (short)145, (short)32, (short)33,
(short)0, (short)0, (short)26, (short)146, (short)147, (short)0, (short)0,
(short)148, (short)149, (short)150, (short)151, (short)152, (short)153, (short)154,
(short)0, (short)155, (short)156, (short)0, (short)6, (short)7, (short)8,
(short)9, (short)10, (short)11, (short)12, (short)13, (short)14, (short)15,
(short)16, (short)17, (short)18, (short)19, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)27, (short)157, (short)158, (short)159,
(short)160, (short)28, (short)29, (short)30, (short)21, (short)0, (short)305,
(short)0, (short)0, (short)161, (short)0, (short)0, (short)32, (short)225,
(short)0, (short)26, (short)0, (short)0, (short)0, (short)163, (short)0, (short)0,
(short)164, (short)0, (short)165, (short)166, (short)0, (short)0, (short)0,
(short)0, (short)167, (short)168, (short)6, (short)7, (short)8, (short)9,
(short)10, (short)11, (short)12, (short)13, (short)14, (short)15, (short)16,
(short)17, (short)18, (short)19, (short)0, (short)0, (short)533, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)28, (short)0,
(short)30, (short)142, (short)21, (short)143, (short)0, (short)144, (short)0,
(short)0, (short)145, (short)32, (short)33, (short)0, (short)0, (short)26,
(short)146, (short)147, (short)0, (short)0, (short)148, (short)149, (short)150,
(short)151, (short)152, (short)153, (short)154, (short)0, (short)155, (short)156,
(short)0, (short)0, (short)0, (short)0, (short)6, (short)7, (short)8, (short)9,
(short)10, (short)11, (short)12, (short)13, (short)14, (short)15, (short)16,
(short)17, (short)18, (short)19, (short)0, (short)0, (short)0, (short)27,
(short)157, (short)158, (short)159, (short)160, (short)28, (short)29, (short)30,
(short)0, (short)0, (short)0, (short)21, (short)0, (short)161, (short)0, (short)0,
(short)32, (short)225, (short)0, (short)0, (short)0, (short)0, (short)26,
(short)163, (short)0, (short)0, (short)164, (short)0, (short)165, (short)166,
(short)0, (short)0, (short)0, (short)0, (short)167, (short)168, (short)1,
(short)2, (short)3, (short)99, (short)5, (short)6, (short)7, (short)8, (short)9,
(short)10, (short)11, (short)12, (short)13, (short)14, (short)15, (short)16,
(short)17, (short)18, (short)19, (short)0, (short)0, (short)20, (short)27,
(short)0, (short)0, (short)0, (short)0, (short)28, (short)29, (short)30, (short)0,
(short)441, (short)0, (short)21, (short)0, (short)287, (short)442, (short)0,
(short)32, (short)33, (short)23, (short)0, (short)0, (short)279, (short)26,
(short)34, (short)35, (short)36, (short)0, (short)6, (short)7, (short)8, (short)9,
(short)10, (short)11, (short)12, (short)13, (short)14, (short)15, (short)16,
(short)17, (short)18, (short)19, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)21, (short)0, (short)0, (short)27, (short)0, (short)0, (short)0,
(short)0, (short)28, (short)29, (short)30, (short)26, (short)0, (short)0,
(short)0, (short)0, (short)31, (short)0, (short)0, (short)32, (short)33, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)34, (short)35, (short)36, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)28, (short)0,
(short)30, (short)0, (short)441, (short)0, (short)0, (short)0, (short)278,
(short)442, (short)0, (short)32, (short)33, (short)0, (short)0, (short)0,
(short)279, (short)0, (short)34, (short)35, (short)36, (short)1, (short)2,
(short)3, (short)99, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)0, (short)0, (short)20, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)21, (short)0, (short)22, (short)0, (short)0, (short)0, (short)0, (short)23,
(short)0, (short)0, (short)0, (short)26, (short)0, (short)0, (short)0, (short)0,
(short)1, (short)2, (short)3, (short)99, (short)5, (short)6, (short)7, (short)8,
(short)9, (short)10, (short)11, (short)12, (short)13, (short)14, (short)15,
(short)16, (short)17, (short)18, (short)19, (short)0, (short)0, (short)20,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)21, (short)0, (short)22, (short)28, (short)0,
(short)30, (short)0, (short)23, (short)100, (short)0, (short)196, (short)26,
(short)0, (short)0, (short)32, (short)33, (short)1, (short)2, (short)3, (short)99,
(short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)0, (short)0, (short)20, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)21, (short)0,
(short)0, (short)28, (short)0, (short)30, (short)0, (short)23, (short)100,
(short)0, (short)0, (short)26, (short)0, (short)0, (short)32, (short)33, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)28, (short)0, (short)30, (short)21,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)32,
(short)33, (short)209, (short)26, (short)210, (short)211, (short)212, (short)213,
(short)214, (short)215, (short)216, (short)217, (short)142, (short)0, (short)143,
(short)0, (short)144, (short)218, (short)0, (short)145, (short)0, (short)219,
(short)0, (short)0, (short)0, (short)146, (short)147, (short)220, (short)221,
(short)148, (short)149, (short)150, (short)151, (short)152, (short)153, (short)154,
(short)0, (short)155, (short)156, (short)243, (short)0, (short)0, (short)0,
(short)0, (short)28, (short)0, (short)30, (short)0, (short)244, (short)0,
(short)0, (short)0, (short)0, (short)245, (short)0, (short)32, (short)33,
(short)0, (short)0, (short)0, (short)222, (short)157, (short)158, (short)159,
(short)160, (short)0, (short)29, (short)0, (short)0, (short)223, (short)100,
(short)0, (short)0, (short)161, (short)0, (short)0, (short)0, (short)162,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)163, (short)0, (short)0,
(short)164, (short)0, (short)165, (short)166, (short)0, (short)0, (short)0,
(short)0, (short)167, (short)168, (short)6, (short)7, (short)8, (short)9,
(short)10, (short)11, (short)12, (short)13, (short)14, (short)15, (short)16,
(short)17, (short)18, (short)19, (short)6, (short)7, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)21, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)26, (short)0, (short)0,
(short)21, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)26, (short)0, (short)0, (short)0, (short)0,
(short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)28,
(short)0, (short)30, (short)0, (short)556, (short)0, (short)21, (short)0,
(short)0, (short)557, (short)0, (short)32, (short)33, (short)0, (short)28,
(short)0, (short)30, (short)26, (short)0, (short)131, (short)0, (short)0,
(short)457, (short)0, (short)0, (short)32, (short)33, (short)142, (short)0,
(short)143, (short)0, (short)144, (short)0, (short)0, (short)145, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)146, (short)147, (short)0,
(short)0, (short)148, (short)149, (short)150, (short)151, (short)152, (short)153,
(short)154, (short)0, (short)155, (short)156, (short)0, (short)0, (short)0,
(short)0, (short)28, (short)0, (short)30, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)32, (short)33, (short)0, (short)0,
(short)0, (short)0, (short)27, (short)157, (short)158, (short)159, (short)160,
(short)0, (short)29, (short)0, (short)0, (short)0, (short)297, (short)458,
(short)0, (short)161, (short)0, (short)0, (short)0, (short)162, (short)0,
(short)0, (short)0, (short)425, (short)0, (short)163, (short)0, (short)0,
(short)164, (short)426, (short)165, (short)166, (short)0, (short)0, (short)0,
(short)0, (short)167, (short)168, (short)142, (short)0, (short)143, (short)0,
(short)144, (short)0, (short)0, (short)145, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)146, (short)147, (short)0, (short)0, (short)148,
(short)149, (short)150, (short)151, (short)152, (short)153, (short)154, (short)0,
(short)155, (short)156, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)27, (short)157, (short)158, (short)159, (short)160, (short)0, (short)29,
(short)0, (short)0, (short)0, (short)297, (short)619, (short)0, (short)161,
(short)0, (short)0, (short)0, (short)162, (short)0, (short)0, (short)0, (short)425,
(short)0, (short)163, (short)0, (short)0, (short)164, (short)426, (short)165,
(short)166, (short)0, (short)0, (short)0, (short)0, (short)167, (short)168,
(short)142, (short)0, (short)143, (short)0, (short)144, (short)0, (short)0,
(short)145, (short)0, (short)0, (short)0, (short)0, (short)0, (short)146,
(short)147, (short)0, (short)0, (short)148, (short)149, (short)150, (short)151,
(short)152, (short)153, (short)154, (short)0, (short)155, (short)156, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)27, (short)157, (short)158,
(short)159, (short)160, (short)0, (short)29, (short)0, (short)0, (short)0,
(short)297, (short)675, (short)0, (short)161, (short)0, (short)0, (short)0,
(short)162, (short)0, (short)0, (short)0, (short)425, (short)0, (short)163,
(short)0, (short)0, (short)164, (short)426, (short)165, (short)166, (short)0,
(short)0, (short)0, (short)0, (short)167, (short)168, (short)142, (short)0,
(short)143, (short)0, (short)144, (short)0, (short)0, (short)145, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)146, (short)147, (short)0,
(short)0, (short)148, (short)149, (short)150, (short)151, (short)152, (short)153,
(short)154, (short)0, (short)155, (short)156, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)27, (short)157, (short)158, (short)159, (short)160,
(short)0, (short)29, (short)0, (short)0, (short)0, (short)297, (short)0, (short)0,
(short)161, (short)0, (short)0, (short)0, (short)162, (short)0, (short)0,
(short)0, (short)425, (short)0, (short)163, (short)0, (short)0, (short)164,
(short)426, (short)165, (short)166, (short)0, (short)0, (short)0, (short)0,
(short)167, (short)168, (short)142, (short)0, (short)143, (short)0, (short)144,
(short)0, (short)0, (short)145, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)146, (short)147, (short)0, (short)0, (short)148, (short)149, (short)150,
(short)151, (short)152, (short)153, (short)154, (short)0, (short)155, (short)156,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)27, (short)157, (short)158,
(short)159, (short)160, (short)0, (short)29, (short)0, (short)0, (short)0,
(short)297, (short)0, (short)0, (short)161, (short)0, (short)0, (short)0,
(short)162, (short)0, (short)0, (short)0, (short)0, (short)0, (short)163,
(short)0, (short)0, (short)164, (short)0, (short)165, (short)166, (short)0,
(short)0, (short)0, (short)0, (short)167, (short)168, (short)142, (short)0,
(short)143, (short)0, (short)144, (short)0, (short)0, (short)145, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)146, (short)147, (short)0,
(short)0, (short)148, (short)149, (short)150, (short)151, (short)152, (short)153,
(short)154, (short)0, (short)155, (short)156, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)27, (short)157, (short)158, (short)159, (short)160,
(short)0, (short)29, (short)0, (short)0, (short)381, (short)0, (short)0, (short)0,
(short)161, (short)0, (short)0, (short)0, (short)162, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)163, (short)0, (short)0, (short)164,
(short)0, (short)165, (short)166, (short)0, (short)0, (short)0, (short)0,
(short)167, (short)168, (short)142, (short)0, (short)143, (short)0, (short)144,
(short)0, (short)0, (short)145, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)146, (short)147, (short)0, (short)0, (short)148, (short)149, (short)150,
(short)151, (short)152, (short)153, (short)154, (short)0, (short)155, (short)156,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)27, (short)157, (short)158,
(short)159, (short)160, (short)0, (short)29, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)161, (short)0, (short)0, (short)0, (short)162,
(short)0, (short)0, (short)0, (short)0, (short)445, (short)163, (short)0,
(short)0, (short)164, (short)0, (short)165, (short)166, (short)0, (short)0,
(short)0, (short)0, (short)167, (short)168, (short)142, (short)0, (short)143,
(short)0, (short)144, (short)0, (short)0, (short)145, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)146, (short)147, (short)0, (short)0,
(short)148, (short)149, (short)150, (short)151, (short)152, (short)153, (short)154,
(short)0, (short)155, (short)156, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)27, (short)157, (short)158, (short)159, (short)160, (short)0,
(short)29, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)161,
(short)495, (short)0, (short)0, (short)162, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)163, (short)0, (short)0, (short)164, (short)0,
(short)165, (short)166, (short)0, (short)0, (short)0, (short)0, (short)167,
(short)168, (short)142, (short)0, (short)143, (short)0, (short)144, (short)0,
(short)0, (short)145, (short)0, (short)0, (short)0, (short)0, (short)0, (short)146,
(short)147, (short)0, (short)0, (short)148, (short)149, (short)150, (short)151,
(short)152, (short)153, (short)154, (short)0, (short)155, (short)156, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)27, (short)157, (short)158,
(short)159, (short)160, (short)0, (short)29, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)161, (short)521, (short)0, (short)0,
(short)162, (short)0, (short)0, (short)0, (short)0, (short)0, (short)163,
(short)0, (short)0, (short)164, (short)0, (short)165, (short)166, (short)0,
(short)0, (short)0, (short)0, (short)167, (short)168, (short)142, (short)0,
(short)143, (short)0, (short)144, (short)0, (short)0, (short)145, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)146, (short)147, (short)0,
(short)0, (short)148, (short)149, (short)150, (short)151, (short)152, (short)153,
(short)154, (short)0, (short)155, (short)156, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)27, (short)157, (short)158, (short)159, (short)160,
(short)0, (short)29, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)161, (short)0, (short)0, (short)0, (short)162, (short)0, (short)0,
(short)0, (short)0, (short)560, (short)163, (short)0, (short)0, (short)164,
(short)0, (short)165, (short)166, (short)0, (short)0, (short)0, (short)0,
(short)167, (short)168, (short)142, (short)0, (short)143, (short)0, (short)144,
(short)0, (short)0, (short)145, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)146, (short)147, (short)0, (short)0, (short)148, (short)149, (short)150,
(short)151, (short)152, (short)153, (short)154, (short)0, (short)155, (short)156,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)27, (short)157, (short)158,
(short)159, (short)160, (short)0, (short)29, (short)0, (short)0, (short)592,
(short)0, (short)0, (short)0, (short)161, (short)0, (short)0, (short)0, (short)162,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)163, (short)0, (short)0,
(short)164, (short)0, (short)165, (short)166, (short)0, (short)0, (short)0,
(short)0, (short)167, (short)168, (short)142, (short)0, (short)143, (short)0,
(short)144, (short)0, (short)0, (short)145, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)146, (short)147, (short)0, (short)0, (short)148,
(short)149, (short)150, (short)151, (short)152, (short)153, (short)154, (short)0,
(short)155, (short)156, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)27, (short)157, (short)158, (short)159, (short)160, (short)0, (short)29,
(short)0, (short)0, (short)594, (short)0, (short)0, (short)0, (short)161,
(short)0, (short)0, (short)0, (short)162, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)163, (short)0, (short)0, (short)164, (short)0, (short)165,
(short)166, (short)0, (short)0, (short)0, (short)0, (short)167, (short)168,
(short)142, (short)0, (short)143, (short)0, (short)144, (short)0, (short)0,
(short)145, (short)0, (short)0, (short)0, (short)0, (short)0, (short)146,
(short)147, (short)0, (short)0, (short)148, (short)149, (short)150, (short)151,
(short)152, (short)153, (short)154, (short)0, (short)155, (short)156, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)27, (short)157, (short)158,
(short)159, (short)160, (short)0, (short)29, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)161, (short)633, (short)0, (short)0,
(short)162, (short)0, (short)0, (short)0, (short)0, (short)0, (short)163,
(short)0, (short)0, (short)164, (short)0, (short)165, (short)166, (short)0,
(short)0, (short)0, (short)0, (short)167, (short)168, (short)142, (short)0,
(short)143, (short)0, (short)144, (short)0, (short)0, (short)145, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)146, (short)147, (short)0,
(short)0, (short)148, (short)149, (short)150, (short)151, (short)152, (short)153,
(short)154, (short)0, (short)155, (short)156, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)27, (short)157, (short)158, (short)159, (short)160,
(short)0, (short)29, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)161, (short)636, (short)0, (short)0, (short)162, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)163, (short)0, (short)0, (short)164,
(short)0, (short)165, (short)166, (short)0, (short)0, (short)0, (short)0,
(short)167, (short)168, (short)142, (short)0, (short)143, (short)0, (short)144,
(short)0, (short)0, (short)145, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)146, (short)147, (short)0, (short)0, (short)148, (short)149, (short)150,
(short)151, (short)152, (short)153, (short)154, (short)0, (short)155, (short)156,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)27, (short)157, (short)158,
(short)159, (short)160, (short)0, (short)29, (short)0, (short)0, (short)639,
(short)0, (short)0, (short)0, (short)161, (short)0, (short)0, (short)0, (short)162,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)163, (short)0, (short)0,
(short)164, (short)0, (short)165, (short)166, (short)0, (short)0, (short)0,
(short)0, (short)167, (short)168, (short)142, (short)0, (short)143, (short)0,
(short)144, (short)0, (short)0, (short)145, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)146, (short)147, (short)0, (short)0, (short)148,
(short)149, (short)150, (short)151, (short)152, (short)153, (short)154, (short)0,
(short)155, (short)156, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)27, (short)157, (short)158, (short)159, (short)160, (short)0, (short)29,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)161, (short)660,
(short)0, (short)0, (short)162, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)163, (short)0, (short)0, (short)164, (short)0, (short)165, (short)166,
(short)0, (short)0, (short)0, (short)0, (short)167, (short)168, (short)142,
(short)0, (short)143, (short)0, (short)144, (short)0, (short)0, (short)145,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)146, (short)147,
(short)0, (short)0, (short)148, (short)149, (short)150, (short)151, (short)152,
(short)153, (short)154, (short)0, (short)155, (short)156, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)27, (short)157, (short)158, (short)159,
(short)160, (short)0, (short)29, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)161, (short)664, (short)0, (short)0, (short)162, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)163, (short)0, (short)0, (short)164,
(short)0, (short)165, (short)166, (short)0, (short)0, (short)0, (short)0,
(short)167, (short)168, (short)142, (short)0, (short)143, (short)0, (short)144,
(short)0, (short)0, (short)145, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)146, (short)147, (short)0, (short)0, (short)148, (short)149, (short)150,
(short)151, (short)152, (short)153, (short)154, (short)0, (short)155, (short)156,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)27, (short)157, (short)158,
(short)159, (short)160, (short)0, (short)29, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)161, (short)666, (short)0, (short)0,
(short)162, (short)0, (short)0, (short)0, (short)0, (short)0, (short)163,
(short)0, (short)0, (short)164, (short)0, (short)165, (short)166, (short)0,
(short)0, (short)0, (short)0, (short)167, (short)168, (short)142, (short)0,
(short)143, (short)0, (short)144, (short)0, (short)0, (short)145, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)146, (short)147, (short)0,
(short)0, (short)148, (short)149, (short)150, (short)151, (short)152, (short)153,
(short)154, (short)0, (short)155, (short)156, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)27, (short)157, (short)158, (short)159, (short)160,
(short)0, (short)29, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)161, (short)687, (short)0, (short)0, (short)162, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)163, (short)0, (short)0, (short)164,
(short)0, (short)165, (short)166, (short)0, (short)0, (short)0, (short)0,
(short)167, (short)168, (short)142, (short)0, (short)143, (short)0, (short)144,
(short)0, (short)0, (short)145, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)146, (short)147, (short)0, (short)0, (short)148, (short)149, (short)150,
(short)151, (short)152, (short)153, (short)154, (short)0, (short)155, (short)156,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)27, (short)157, (short)158,
(short)159, (short)160, (short)0, (short)29, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)161, (short)0, (short)0, (short)0, (short)162,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)163, (short)0, (short)0,
(short)164, (short)0, (short)165, (short)166, (short)0, (short)0, (short)0,
(short)0, (short)167, (short)168, (short)142, (short)0, (short)143, (short)0,
(short)144, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)146, (short)147, (short)0, (short)0, (short)148, (short)149,
(short)150, (short)151, (short)152, (short)153, (short)154, (short)0, (short)155,
(short)156, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)27, (short)157,
(short)158, (short)159, (short)160, (short)0, (short)29, (short)0, (short)0,
(short)0, (short)573, (short)0, (short)0, (short)161, (short)0, (short)0,
(short)0, (short)162, (short)0, (short)0, (short)0, (short)0, (short)0, (short)163,
(short)0, (short)0, (short)164, (short)0, (short)165, (short)166, (short)0,
(short)0, (short)0, (short)0, (short)167, (short)168, (short)142, (short)0,
(short)143, (short)0, (short)144, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)146, (short)147, (short)0,
(short)0, (short)148, (short)149, (short)150, (short)151, (short)152, (short)153,
(short)154, (short)0, (short)155, (short)156, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)27, (short)157, (short)158, (short)159, (short)160,
(short)0, (short)29, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)293, (short)0, (short)0, (short)0, (short)162, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)163, (short)0, (short)0, (short)164,
(short)0, (short)165, (short)166, (short)0, (short)0, (short)0, (short)0,
(short)167, (short)168, (short)142, (short)0, (short)143, (short)0, (short)144,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)146, (short)147, (short)0, (short)0, (short)148, (short)149, (short)150,
(short)151, (short)152, (short)153, (short)154, (short)0, (short)155, (short)156,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)27, (short)157, (short)158,
(short)159, (short)160, (short)0, (short)29, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)295, (short)0, (short)0, (short)0, (short)162,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)163, (short)0, (short)0,
(short)164, (short)0, (short)165, (short)166, (short)0, (short)0, (short)0,
(short)0, (short)167, (short)168, (short)142, (short)0, (short)143, (short)0,
(short)144, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)146, (short)147, (short)0, (short)0, (short)148, (short)149,
(short)150, (short)151, (short)152, (short)153, (short)154, (short)0, (short)155,
(short)156, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)61, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)27, (short)157,
(short)158, (short)159, (short)160, (short)0, (short)29, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)161, (short)0, (short)0, (short)0,
(short)162, (short)0, (short)0, (short)0, (short)0, (short)0, (short)163,
(short)0, (short)0, (short)164, (short)0, (short)165, (short)166, (short)0,
(short)0, (short)0, (short)0, (short)167, (short)168, (short)27, (short)62,
(short)0, (short)63, (short)64, (short)0, (short)29, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)65, (short)0, (short)0, (short)66, (short)67,
(short)0, (short)0, (short)0, (short)425, (short)0, (short)68, (short)0, (short)0,
(short)0, (short)426, (short)69, (short)70}; static short Cyc_yycheck[ 4227u]={
(short)50, (short)234, (short)106, (short)51, (short)50, (short)105, (short)270,
(short)57, (short)354, (short)81, (short)82, (short)50, (short)253, (short)145,
(short)396, (short)52, (short)161, (short)50, (short)88, (short)31, (short)22,
(short)80, (short)292, (short)262, (short)400, (short)367, (short)364, (short)630,
(short)127, (short)99, (short)91, (short)43, (short)360, (short)17, (short)372,
(short)107, (short)0, (short)44, (short)45, (short)46, (short)125, (short)96,
(short)92, (short)93, (short)94, (short)95, (short)92, (short)69, (short)112,
(short)104, (short)100, (short)109, (short)102, (short)111, (short)100, (short)437,
(short)102, (short)21, (short)22, (short)92, (short)24, (short)109, (short)26,
(short)65, (short)196, (short)441, (short)98, (short)31, (short)70, (short)102,
(short)134, (short)103, (short)305, (short)145, (short)38, (short)139, (short)161,
(short)163, (short)164, (short)43, (short)166, (short)82, (short)410, (short)425,
(short)48, (short)88, (short)108, (short)690, (short)52, (short)345, (short)93,
(short)103, (short)0, (short)85, (short)350, (short)59, (short)699, (short)98,
(short)184, (short)83, (short)703, (short)65, (short)705, (short)63, (short)95,
(short)63, (short)70, (short)17, (short)18, (short)19, (short)98, (short)68,
(short)83, (short)104, (short)83, (short)103, (short)77, (short)63, (short)89,
(short)110, (short)84, (short)85, (short)86, (short)87, (short)196, (short)132,
(short)133, (short)139, (short)367, (short)385, (short)38, (short)401, (short)217,
(short)64, (short)65, (short)88, (short)97, (short)92, (short)98, (short)103,
(short)98, (short)94, (short)102, (short)98, (short)102, (short)106, (short)103,
(short)197, (short)293, (short)113, (short)295, (short)197, (short)98, (short)83,
(short)92, (short)531, (short)102, (short)121, (short)96, (short)89, (short)280,
(short)260, (short)261, (short)127, (short)197, (short)264, (short)96, (short)509,
(short)134, (short)289, (short)101, (short)102, (short)101, (short)139, (short)104,
(short)139, (short)84, (short)85, (short)86, (short)87, (short)556, (short)60,
(short)93, (short)195, (short)234, (short)96, (short)425, (short)81, (short)234,
(short)82, (short)101, (short)96, (short)256, (short)72, (short)206, (short)436,
(short)106, (short)107, (short)108, (short)93, (short)161, (short)98, (short)83,
(short)92, (short)93, (short)113, (short)103, (short)279, (short)293, (short)90,
(short)295, (short)93, (short)212, (short)107, (short)278, (short)300, (short)228,
(short)96, (short)218, (short)98, (short)220, (short)221, (short)93, (short)287,
(short)93, (short)297, (short)92, (short)573, (short)91, (short)611, (short)315,
(short)195, (short)98, (short)96, (short)282, (short)96, (short)308, (short)93,
(short)101, (short)118, (short)96, (short)104, (short)206, (short)204, (short)330,
(short)331, (short)332, (short)83, (short)59, (short)60, (short)61, (short)301,
(short)289, (short)89, (short)83, (short)305, (short)83, (short)83, (short)83,
(short)305, (short)96, (short)90, (short)89, (short)89, (short)228, (short)92,
(short)351, (short)93, (short)270, (short)96, (short)509, (short)92, (short)344,
(short)345, (short)96, (short)287, (short)83, (short)98, (short)350, (short)106,
(short)107, (short)108, (short)354, (short)90, (short)548, (short)96, (short)527,
(short)84, (short)373, (short)374, (short)375, (short)94, (short)377, (short)104,
(short)98, (short)98, (short)260, (short)261, (short)91, (short)110, (short)264,
(short)105, (short)566, (short)96, (short)95, (short)94, (short)270, (short)271,
(short)101, (short)98, (short)106, (short)104, (short)93, (short)385, (short)97,
(short)276, (short)277, (short)389, (short)582, (short)115, (short)116, (short)92,
(short)394, (short)287, (short)285, (short)289, (short)98, (short)377, (short)85,
(short)461, (short)389, (short)377, (short)293, (short)92, (short)295, (short)95,
(short)96, (short)301, (short)577, (short)98, (short)390, (short)101, (short)96,
(short)92, (short)390, (short)449, (short)92, (short)93, (short)448, (short)98,
(short)104, (short)390, (short)106, (short)107, (short)108, (short)88, (short)77,
(short)390, (short)82, (short)80, (short)419, (short)94, (short)70, (short)71,
(short)72, (short)73, (short)74, (short)75, (short)76, (short)77, (short)78,
(short)79, (short)94, (short)83, (short)97, (short)98, (short)97, (short)96,
(short)450, (short)89, (short)90, (short)467, (short)93, (short)93, (short)105,
(short)106, (short)37, (short)95, (short)388, (short)461, (short)654, (short)112,
(short)66, (short)67, (short)358, (short)97, (short)360, (short)501, (short)77,
(short)447, (short)92, (short)80, (short)92, (short)364, (short)92, (short)98,
(short)98, (short)92, (short)98, (short)102, (short)98, (short)372, (short)94,
(short)98, (short)62, (short)63, (short)98, (short)94, (short)97, (short)68,
(short)422, (short)98, (short)97, (short)98, (short)96, (short)427, (short)105,
(short)106, (short)97, (short)98, (short)515, (short)516, (short)83, (short)84,
(short)566, (short)86, (short)87, (short)670, (short)89, (short)109, (short)161,
(short)83, (short)96, (short)94, (short)96, (short)96, (short)410, (short)89,
(short)99, (short)100, (short)582, (short)96, (short)464, (short)104, (short)96,
(short)106, (short)94, (short)690, (short)422, (short)110, (short)111, (short)112,
(short)104, (short)427, (short)106, (short)107, (short)108, (short)37, (short)97,
(short)98, (short)703, (short)97, (short)98, (short)113, (short)524, (short)97,
(short)98, (short)114, (short)524, (short)84, (short)85, (short)112, (short)161,
(short)204, (short)322, (short)323, (short)324, (short)325, (short)449, (short)621,
(short)96, (short)524, (short)22, (short)23, (short)566, (short)94, (short)508,
(short)97, (short)98, (short)93, (short)464, (short)573, (short)97, (short)98,
(short)576, (short)94, (short)591, (short)592, (short)94, (short)594, (short)582,
(short)596, (short)94, (short)83, (short)84, (short)93, (short)86, (short)87,
(short)83, (short)89, (short)570, (short)96, (short)654, (short)96, (short)570,
(short)204, (short)96, (short)326, (short)327, (short)99, (short)100, (short)97,
(short)98, (short)618, (short)96, (short)256, (short)106, (short)570, (short)97,
(short)98, (short)501, (short)111, (short)112, (short)96, (short)508, (short)83,
(short)548, (short)97, (short)98, (short)621, (short)635, (short)89, (short)92,
(short)638, (short)639, (short)276, (short)277, (short)278, (short)96, (short)92,
(short)97, (short)98, (short)97, (short)98, (short)285, (short)103, (short)287,
(short)103, (short)106, (short)107, (short)108, (short)292, (short)293, (short)96,
(short)295, (short)97, (short)98, (short)320, (short)321, (short)94, (short)328,
(short)329, (short)654, (short)668, (short)548, (short)95, (short)97, (short)672,
(short)588, (short)97, (short)590, (short)98, (short)105, (short)94, (short)96,
(short)98, (short)94, (short)276, (short)277, (short)278, (short)97, (short)94,
(short)35, (short)97, (short)689, (short)101, (short)285, (short)571, (short)287,
(short)92, (short)571, (short)98, (short)103, (short)292, (short)293, (short)28,
(short)295, (short)92, (short)41, (short)82, (short)670, (short)97, (short)83,
(short)109, (short)670, (short)96, (short)98, (short)97, (short)142, (short)94,
(short)144, (short)83, (short)94, (short)633, (short)98, (short)88, (short)636,
(short)97, (short)97, (short)641, (short)690, (short)155, (short)156, (short)364,
(short)690, (short)83, (short)97, (short)105, (short)83, (short)163, (short)164,
(short)372, (short)166, (short)703, (short)89, (short)655, (short)97, (short)703,
(short)97, (short)659, (short)660, (short)96, (short)96, (short)663, (short)664,
(short)97, (short)666, (short)94, (short)389, (short)97, (short)184, (short)106,
(short)107, (short)108, (short)98, (short)97, (short)97, (short)641, (short)94,
(short)94, (short)401, (short)681, (short)96, (short)93, (short)684, (short)364,
(short)686, (short)687, (short)92, (short)97, (short)97, (short)94, (short)98,
(short)372, (short)105, (short)97, (short)97, (short)697, (short)419, (short)93,
(short)105, (short)101, (short)97, (short)93, (short)92, (short)97, (short)97,
(short)26, (short)97, (short)92, (short)389, (short)97, (short)35, (short)97,
(short)37, (short)94, (short)39, (short)97, (short)103, (short)42, (short)97,
(short)94, (short)401, (short)0, (short)0, (short)48, (short)49, (short)448,
(short)449, (short)52, (short)53, (short)54, (short)55, (short)56, (short)57,
(short)58, (short)538, (short)60, (short)61, (short)195, (short)419, (short)301,
(short)508, (short)285, (short)283, (short)428, (short)271, (short)204, (short)262,
(short)316, (short)548, (short)314, - (short)1, (short)317, - (short)1, -
(short)1, - (short)1, (short)318, - (short)1, - (short)1, (short)83, (short)84,
(short)85, (short)86, (short)87, (short)319, (short)89, - (short)1, - (short)1,
(short)448, (short)449, - (short)1, - (short)1, (short)96, - (short)1, - (short)1,
- (short)1, (short)100, - (short)1, - (short)1, (short)501, - (short)1, (short)105,
(short)106, - (short)1, - (short)1, (short)109, - (short)1, (short)111, (short)112,
- (short)1, - (short)1, - (short)1, - (short)1, (short)117, (short)118, -
(short)1, - (short)1, - (short)1, - (short)1, (short)314, - (short)1, (short)316,
(short)317, (short)318, (short)319, (short)320, (short)321, (short)322, (short)323,
(short)324, (short)325, (short)326, (short)327, (short)328, (short)329, (short)330,
(short)331, (short)332, - (short)1, - (short)1, - (short)1, (short)501, (short)3,
(short)4, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)20, (short)21, - (short)1, - (short)1, (short)24, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)571, - (short)1, -
(short)1, (short)367, - (short)1, - (short)1, (short)36, - (short)1, (short)38,
- (short)1, - (short)1, - (short)1, - (short)1, (short)43, - (short)1, (short)45,
(short)46, (short)47, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)571,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, (short)83, - (short)1, - (short)1,
- (short)1, - (short)1, (short)88, (short)89, (short)90, (short)425, - (short)1,
- (short)1, - (short)1, - (short)1, (short)96, - (short)1, - (short)1, (short)99,
(short)100, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)106,
(short)107, (short)108, - (short)1, (short)3, (short)4, (short)5, (short)6,
(short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)20,
(short)21, (short)22, (short)23, (short)24, (short)25, (short)467, (short)27,
(short)28, (short)29, (short)30, (short)31, (short)32, (short)33, (short)34,
(short)35, (short)36, (short)37, (short)38, (short)39, (short)40, - (short)1,
(short)42, (short)43, (short)44, - (short)1, - (short)1, (short)47, (short)48,
(short)49, (short)50, (short)51, (short)52, (short)53, (short)54, (short)55,
(short)56, (short)57, (short)58, - (short)1, (short)60, (short)61, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)509, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)83, (short)84, (short)85, (short)86, (short)87, (short)88, (short)89,
(short)90, - (short)1, (short)92, (short)93, - (short)1, - (short)1, (short)96,
- (short)1, - (short)1, (short)99, (short)100, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)106, - (short)1, - (short)1, (short)109, -
(short)1, (short)111, (short)112, - (short)1, - (short)1, - (short)1, - (short)1,
(short)117, (short)118, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)577,
(short)3, (short)4, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)20, (short)21, - (short)1, - (short)1, (short)24,
(short)25, - (short)1, (short)27, (short)28, (short)29, (short)30, (short)31,
(short)32, (short)33, (short)34, (short)35, (short)36, (short)37, (short)38,
(short)39, (short)40, - (short)1, (short)42, (short)43, (short)44, - (short)1, -
(short)1, (short)47, (short)48, (short)49, (short)50, (short)51, (short)52,
(short)53, (short)54, (short)55, (short)56, (short)57, (short)58, - (short)1,
(short)60, (short)61, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)83, (short)84, (short)85, (short)86,
(short)87, (short)88, (short)89, (short)90, - (short)1, (short)92, (short)93,
(short)94, - (short)1, (short)96, - (short)1, - (short)1, (short)99, (short)100,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)106, -
(short)1, - (short)1, (short)109, - (short)1, (short)111, (short)112, - (short)1,
- (short)1, - (short)1, - (short)1, (short)117, (short)118, (short)3, (short)4,
(short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)20, (short)21, - (short)1, - (short)1, (short)24, (short)25, - (short)1,
(short)27, (short)28, (short)29, (short)30, (short)31, (short)32, (short)33,
(short)34, (short)35, (short)36, (short)37, (short)38, (short)39, (short)40, -
(short)1, (short)42, (short)43, (short)44, - (short)1, - (short)1, (short)47,
(short)48, (short)49, (short)50, (short)51, (short)52, (short)53, (short)54,
(short)55, (short)56, (short)57, (short)58, - (short)1, (short)60, (short)61, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)83, (short)84, (short)85, (short)86, (short)87, (short)88,
(short)89, (short)90, - (short)1, (short)92, (short)93, - (short)1, - (short)1,
(short)96, - (short)1, - (short)1, (short)99, (short)100, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)106, - (short)1, - (short)1, (short)109,
- (short)1, (short)111, (short)112, - (short)1, - (short)1, - (short)1, -
(short)1, (short)117, (short)118, (short)3, (short)4, (short)5, (short)6,
(short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)20,
(short)21, - (short)1, - (short)1, (short)24, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, (short)35, (short)36, (short)37, (short)38, (short)39, - (short)1, -
(short)1, (short)42, (short)43, - (short)1, - (short)1, - (short)1, (short)47,
(short)48, (short)49, - (short)1, - (short)1, (short)52, (short)53, (short)54,
(short)55, (short)56, (short)57, (short)58, - (short)1, (short)60, (short)61, -
(short)1, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)20,
(short)21, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)83, (short)84, (short)85, (short)86, (short)87, (short)88, (short)89,
(short)90, (short)36, (short)92, - (short)1, - (short)1, - (short)1, (short)96,
- (short)1, - (short)1, (short)99, (short)100, - (short)1, (short)47, - (short)1,
- (short)1, - (short)1, (short)106, - (short)1, - (short)1, (short)109, -
(short)1, (short)111, (short)112, - (short)1, - (short)1, - (short)1, - (short)1,
(short)117, (short)118, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)20, (short)21, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)83, - (short)1, - (short)1, - (short)1, - (short)1, (short)88, (short)89,
(short)90, (short)35, (short)36, (short)37, - (short)1, (short)39, - (short)1, -
(short)1, (short)42, (short)99, (short)100, - (short)1, - (short)1, (short)47,
(short)48, (short)49, - (short)1, - (short)1, (short)52, (short)53, (short)54,
(short)55, (short)56, (short)57, (short)58, - (short)1, (short)60, (short)61, -
(short)1, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)20,
(short)21, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)83, (short)84, (short)85, (short)86, (short)87, (short)88, (short)89,
(short)90, (short)36, - (short)1, (short)93, - (short)1, - (short)1, (short)96,
- (short)1, - (short)1, (short)99, (short)100, - (short)1, (short)47, - (short)1,
- (short)1, - (short)1, (short)106, - (short)1, - (short)1, (short)109, -
(short)1, (short)111, (short)112, - (short)1, - (short)1, - (short)1, - (short)1,
(short)117, (short)118, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)20, (short)21, - (short)1, - (short)1, (short)80, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)88, - (short)1,
(short)90, (short)35, (short)36, (short)37, - (short)1, (short)39, - (short)1, -
(short)1, (short)42, (short)99, (short)100, - (short)1, - (short)1, (short)47,
(short)48, (short)49, - (short)1, - (short)1, (short)52, (short)53, (short)54,
(short)55, (short)56, (short)57, (short)58, - (short)1, (short)60, (short)61, -
(short)1, - (short)1, - (short)1, - (short)1, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)20, (short)21, - (short)1, - (short)1, - (short)1,
(short)83, (short)84, (short)85, (short)86, (short)87, (short)88, (short)89,
(short)90, - (short)1, - (short)1, - (short)1, (short)36, - (short)1, (short)96,
- (short)1, - (short)1, (short)99, (short)100, - (short)1, - (short)1, - (short)1,
- (short)1, (short)47, (short)106, - (short)1, - (short)1, (short)109, - (short)1,
(short)111, (short)112, - (short)1, - (short)1, - (short)1, - (short)1, (short)117,
(short)118, (short)3, (short)4, (short)5, (short)6, (short)7, (short)8, (short)9,
(short)10, (short)11, (short)12, (short)13, (short)14, (short)15, (short)16,
(short)17, (short)18, (short)19, (short)20, (short)21, - (short)1, - (short)1,
(short)24, (short)83, - (short)1, - (short)1, - (short)1, - (short)1, (short)88,
(short)89, (short)90, - (short)1, (short)92, - (short)1, (short)36, - (short)1,
(short)96, (short)97, - (short)1, (short)99, (short)100, (short)43, - (short)1,
- (short)1, (short)104, (short)47, (short)106, (short)107, (short)108, - (short)1,
(short)8, (short)9, (short)10, (short)11, (short)12, (short)13, (short)14,
(short)15, (short)16, (short)17, (short)18, (short)19, (short)20, (short)21, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, (short)36, - (short)1, - (short)1, (short)83, - (short)1, - (short)1,
- (short)1, - (short)1, (short)88, (short)89, (short)90, (short)47, - (short)1,
- (short)1, - (short)1, - (short)1, (short)96, - (short)1, - (short)1, (short)99,
(short)100, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)106,
(short)107, (short)108, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)88,
- (short)1, (short)90, - (short)1, (short)92, - (short)1, - (short)1, - (short)1,
(short)96, (short)97, - (short)1, (short)99, (short)100, - (short)1, - (short)1,
- (short)1, (short)104, - (short)1, (short)106, (short)107, (short)108, (short)3,
(short)4, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)20, (short)21, - (short)1, - (short)1, (short)24, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, (short)36, - (short)1, (short)38,
- (short)1, - (short)1, - (short)1, - (short)1, (short)43, - (short)1, - (short)1,
- (short)1, (short)47, - (short)1, - (short)1, - (short)1, - (short)1, (short)3,
(short)4, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)20, (short)21, - (short)1, - (short)1, (short)24, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, (short)36, - (short)1, (short)38,
(short)88, - (short)1, (short)90, - (short)1, (short)43, (short)93, - (short)1,
(short)95, (short)47, - (short)1, - (short)1, (short)99, (short)100, (short)3,
(short)4, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)20, (short)21, - (short)1, - (short)1, (short)24, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, (short)36, - (short)1, - (short)1,
(short)88, - (short)1, (short)90, - (short)1, (short)43, (short)93, - (short)1,
- (short)1, (short)47, - (short)1, - (short)1, (short)99, (short)100, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)20, (short)21, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, (short)88, - (short)1, (short)90, (short)36, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)99,
(short)100, (short)25, (short)47, (short)27, (short)28, (short)29, (short)30,
(short)31, (short)32, (short)33, (short)34, (short)35, - (short)1, (short)37, -
(short)1, (short)39, (short)40, - (short)1, (short)42, - (short)1, (short)44, -
(short)1, - (short)1, - (short)1, (short)48, (short)49, (short)50, (short)51,
(short)52, (short)53, (short)54, (short)55, (short)56, (short)57, (short)58, -
(short)1, (short)60, (short)61, (short)83, - (short)1, - (short)1, - (short)1, -
(short)1, (short)88, - (short)1, (short)90, - (short)1, (short)92, - (short)1, -
(short)1, - (short)1, - (short)1, (short)97, - (short)1, (short)99, (short)100,
- (short)1, - (short)1, - (short)1, (short)83, (short)84, (short)85, (short)86,
(short)87, - (short)1, (short)89, - (short)1, - (short)1, (short)92, (short)93,
- (short)1, - (short)1, (short)96, - (short)1, - (short)1, - (short)1, (short)100,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)106, -
(short)1, - (short)1, (short)109, - (short)1, (short)111, (short)112, - (short)1,
- (short)1, - (short)1, - (short)1, (short)117, (short)118, (short)8, (short)9,
(short)10, (short)11, (short)12, (short)13, (short)14, (short)15, (short)16,
(short)17, (short)18, (short)19, (short)20, (short)21, (short)8, (short)9,
(short)10, (short)11, (short)12, (short)13, (short)14, (short)15, (short)16,
(short)17, (short)18, (short)19, (short)20, (short)21, (short)36, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)47, - (short)1, - (short)1, (short)36, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, (short)47, - (short)1, - (short)1, - (short)1,
- (short)1, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)20,
(short)21, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)88, - (short)1, (short)90, - (short)1, (short)92,
- (short)1, (short)36, - (short)1, - (short)1, (short)97, - (short)1, (short)99,
(short)100, - (short)1, (short)88, - (short)1, (short)90, (short)47, - (short)1,
(short)93, - (short)1, - (short)1, (short)30, - (short)1, - (short)1, (short)99,
(short)100, (short)35, - (short)1, (short)37, - (short)1, (short)39, - (short)1,
- (short)1, (short)42, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)48, (short)49, - (short)1, - (short)1, (short)52, (short)53, (short)54,
(short)55, (short)56, (short)57, (short)58, - (short)1, (short)60, (short)61, -
(short)1, - (short)1, - (short)1, - (short)1, (short)88, - (short)1, (short)90,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, (short)99, (short)100, - (short)1, - (short)1, - (short)1,
- (short)1, (short)83, (short)84, (short)85, (short)86, (short)87, - (short)1,
(short)89, - (short)1, - (short)1, - (short)1, (short)93, (short)94, - (short)1,
(short)96, - (short)1, - (short)1, - (short)1, (short)100, - (short)1, - (short)1,
- (short)1, (short)104, - (short)1, (short)106, - (short)1, - (short)1, (short)109,
(short)110, (short)111, (short)112, - (short)1, - (short)1, - (short)1, -
(short)1, (short)117, (short)118, (short)35, - (short)1, (short)37, - (short)1,
(short)39, - (short)1, - (short)1, (short)42, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)48, (short)49, - (short)1, - (short)1, (short)52,
(short)53, (short)54, (short)55, (short)56, (short)57, (short)58, - (short)1,
(short)60, (short)61, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)83, (short)84, (short)85, (short)86,
(short)87, - (short)1, (short)89, - (short)1, - (short)1, - (short)1, (short)93,
(short)94, - (short)1, (short)96, - (short)1, - (short)1, - (short)1, (short)100,
- (short)1, - (short)1, - (short)1, (short)104, - (short)1, (short)106, -
(short)1, - (short)1, (short)109, (short)110, (short)111, (short)112, - (short)1,
- (short)1, - (short)1, - (short)1, (short)117, (short)118, (short)35, - (short)1,
(short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49, - (short)1,
- (short)1, (short)52, (short)53, (short)54, (short)55, (short)56, (short)57,
(short)58, - (short)1, (short)60, (short)61, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)83, (short)84,
(short)85, (short)86, (short)87, - (short)1, (short)89, - (short)1, - (short)1,
- (short)1, (short)93, (short)94, - (short)1, (short)96, - (short)1, - (short)1,
- (short)1, (short)100, - (short)1, - (short)1, - (short)1, (short)104, -
(short)1, (short)106, - (short)1, - (short)1, (short)109, (short)110, (short)111,
(short)112, - (short)1, - (short)1, - (short)1, - (short)1, (short)117, (short)118,
(short)35, - (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1,
(short)42, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48,
(short)49, - (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55,
(short)56, (short)57, (short)58, - (short)1, (short)60, (short)61, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)83, (short)84, (short)85, (short)86, (short)87, - (short)1, (short)89, -
(short)1, - (short)1, - (short)1, (short)93, - (short)1, - (short)1, (short)96,
- (short)1, - (short)1, - (short)1, (short)100, - (short)1, - (short)1, -
(short)1, (short)104, - (short)1, (short)106, - (short)1, - (short)1, (short)109,
(short)110, (short)111, (short)112, - (short)1, - (short)1, - (short)1, -
(short)1, (short)117, (short)118, (short)35, - (short)1, (short)37, - (short)1,
(short)39, - (short)1, - (short)1, (short)42, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)48, (short)49, - (short)1, - (short)1, (short)52,
(short)53, (short)54, (short)55, (short)56, (short)57, (short)58, - (short)1,
(short)60, (short)61, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)83, (short)84, (short)85, (short)86,
(short)87, - (short)1, (short)89, - (short)1, - (short)1, - (short)1, (short)93,
- (short)1, - (short)1, (short)96, - (short)1, - (short)1, - (short)1, (short)100,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)106, -
(short)1, - (short)1, (short)109, - (short)1, (short)111, (short)112, - (short)1,
- (short)1, - (short)1, - (short)1, (short)117, (short)118, (short)35, - (short)1,
(short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49, - (short)1,
- (short)1, (short)52, (short)53, (short)54, (short)55, (short)56, (short)57,
(short)58, - (short)1, (short)60, (short)61, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)83, (short)84,
(short)85, (short)86, (short)87, - (short)1, (short)89, - (short)1, - (short)1,
(short)92, - (short)1, - (short)1, - (short)1, (short)96, - (short)1, - (short)1,
- (short)1, (short)100, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, (short)106, - (short)1, - (short)1, (short)109, - (short)1, (short)111,
(short)112, - (short)1, - (short)1, - (short)1, - (short)1, (short)117, (short)118,
(short)35, - (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1,
(short)42, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48,
(short)49, - (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55,
(short)56, (short)57, (short)58, - (short)1, (short)60, (short)61, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)83, (short)84, (short)85, (short)86, (short)87, - (short)1, (short)89, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)96,
- (short)1, - (short)1, - (short)1, (short)100, - (short)1, - (short)1, -
(short)1, - (short)1, (short)105, (short)106, - (short)1, - (short)1, (short)109,
- (short)1, (short)111, (short)112, - (short)1, - (short)1, - (short)1, -
(short)1, (short)117, (short)118, (short)35, - (short)1, (short)37, - (short)1,
(short)39, - (short)1, - (short)1, (short)42, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)48, (short)49, - (short)1, - (short)1, (short)52,
(short)53, (short)54, (short)55, (short)56, (short)57, (short)58, - (short)1,
(short)60, (short)61, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)83, (short)84, (short)85, (short)86,
(short)87, - (short)1, (short)89, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)96, (short)97, - (short)1, - (short)1, (short)100,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)106, -
(short)1, - (short)1, (short)109, - (short)1, (short)111, (short)112, - (short)1,
- (short)1, - (short)1, - (short)1, (short)117, (short)118, (short)35, - (short)1,
(short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49, - (short)1,
- (short)1, (short)52, (short)53, (short)54, (short)55, (short)56, (short)57,
(short)58, - (short)1, (short)60, (short)61, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)83, (short)84,
(short)85, (short)86, (short)87, - (short)1, (short)89, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)96, (short)97, - (short)1,
- (short)1, (short)100, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, (short)106, - (short)1, - (short)1, (short)109, - (short)1, (short)111,
(short)112, - (short)1, - (short)1, - (short)1, - (short)1, (short)117, (short)118,
(short)35, - (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1,
(short)42, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48,
(short)49, - (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55,
(short)56, (short)57, (short)58, - (short)1, (short)60, (short)61, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)83, (short)84, (short)85, (short)86, (short)87, - (short)1, (short)89, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)96,
- (short)1, - (short)1, - (short)1, (short)100, - (short)1, - (short)1, -
(short)1, - (short)1, (short)105, (short)106, - (short)1, - (short)1, (short)109,
- (short)1, (short)111, (short)112, - (short)1, - (short)1, - (short)1, -
(short)1, (short)117, (short)118, (short)35, - (short)1, (short)37, - (short)1,
(short)39, - (short)1, - (short)1, (short)42, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)48, (short)49, - (short)1, - (short)1, (short)52,
(short)53, (short)54, (short)55, (short)56, (short)57, (short)58, - (short)1,
(short)60, (short)61, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)83, (short)84, (short)85, (short)86,
(short)87, - (short)1, (short)89, - (short)1, - (short)1, (short)92, - (short)1,
- (short)1, - (short)1, (short)96, - (short)1, - (short)1, - (short)1, (short)100,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)106, -
(short)1, - (short)1, (short)109, - (short)1, (short)111, (short)112, - (short)1,
- (short)1, - (short)1, - (short)1, (short)117, (short)118, (short)35, - (short)1,
(short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49, - (short)1,
- (short)1, (short)52, (short)53, (short)54, (short)55, (short)56, (short)57,
(short)58, - (short)1, (short)60, (short)61, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)83, (short)84,
(short)85, (short)86, (short)87, - (short)1, (short)89, - (short)1, - (short)1,
(short)92, - (short)1, - (short)1, - (short)1, (short)96, - (short)1, - (short)1,
- (short)1, (short)100, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, (short)106, - (short)1, - (short)1, (short)109, - (short)1, (short)111,
(short)112, - (short)1, - (short)1, - (short)1, - (short)1, (short)117, (short)118,
(short)35, - (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1,
(short)42, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48,
(short)49, - (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55,
(short)56, (short)57, (short)58, - (short)1, (short)60, (short)61, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)83, (short)84, (short)85, (short)86, (short)87, - (short)1, (short)89, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)96,
(short)97, - (short)1, - (short)1, (short)100, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)106, - (short)1, - (short)1, (short)109, -
(short)1, (short)111, (short)112, - (short)1, - (short)1, - (short)1, - (short)1,
(short)117, (short)118, (short)35, - (short)1, (short)37, - (short)1, (short)39,
- (short)1, - (short)1, (short)42, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)48, (short)49, - (short)1, - (short)1, (short)52, (short)53,
(short)54, (short)55, (short)56, (short)57, (short)58, - (short)1, (short)60,
(short)61, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)83, (short)84, (short)85, (short)86, (short)87, -
(short)1, (short)89, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)96, (short)97, - (short)1, - (short)1, (short)100, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)106, - (short)1, -
(short)1, (short)109, - (short)1, (short)111, (short)112, - (short)1, - (short)1,
- (short)1, - (short)1, (short)117, (short)118, (short)35, - (short)1, (short)37,
- (short)1, (short)39, - (short)1, - (short)1, (short)42, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)48, (short)49, - (short)1, - (short)1,
(short)52, (short)53, (short)54, (short)55, (short)56, (short)57, (short)58, -
(short)1, (short)60, (short)61, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)83, (short)84, (short)85,
(short)86, (short)87, - (short)1, (short)89, - (short)1, - (short)1, (short)92,
- (short)1, - (short)1, - (short)1, (short)96, - (short)1, - (short)1, - (short)1,
(short)100, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)106,
- (short)1, - (short)1, (short)109, - (short)1, (short)111, (short)112, -
(short)1, - (short)1, - (short)1, - (short)1, (short)117, (short)118, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, (short)58, - (short)1, (short)60, (short)61, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)83,
(short)84, (short)85, (short)86, (short)87, - (short)1, (short)89, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)96, (short)97,
- (short)1, - (short)1, (short)100, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, (short)106, - (short)1, - (short)1, (short)109, - (short)1,
(short)111, (short)112, - (short)1, - (short)1, - (short)1, - (short)1, (short)117,
(short)118, (short)35, - (short)1, (short)37, - (short)1, (short)39, - (short)1,
- (short)1, (short)42, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)48, (short)49, - (short)1, - (short)1, (short)52, (short)53, (short)54,
(short)55, (short)56, (short)57, (short)58, - (short)1, (short)60, (short)61, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)83, (short)84, (short)85, (short)86, (short)87, - (short)1,
(short)89, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)96, (short)97, - (short)1, - (short)1, (short)100, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)106, - (short)1, - (short)1, (short)109,
- (short)1, (short)111, (short)112, - (short)1, - (short)1, - (short)1, -
(short)1, (short)117, (short)118, (short)35, - (short)1, (short)37, - (short)1,
(short)39, - (short)1, - (short)1, (short)42, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)48, (short)49, - (short)1, - (short)1, (short)52,
(short)53, (short)54, (short)55, (short)56, (short)57, (short)58, - (short)1,
(short)60, (short)61, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)83, (short)84, (short)85, (short)86,
(short)87, - (short)1, (short)89, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)96, (short)97, - (short)1, - (short)1, (short)100,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)106, -
(short)1, - (short)1, (short)109, - (short)1, (short)111, (short)112, - (short)1,
- (short)1, - (short)1, - (short)1, (short)117, (short)118, (short)35, - (short)1,
(short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49, - (short)1,
- (short)1, (short)52, (short)53, (short)54, (short)55, (short)56, (short)57,
(short)58, - (short)1, (short)60, (short)61, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)83, (short)84,
(short)85, (short)86, (short)87, - (short)1, (short)89, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)96, (short)97, - (short)1,
- (short)1, (short)100, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, (short)106, - (short)1, - (short)1, (short)109, - (short)1, (short)111,
(short)112, - (short)1, - (short)1, - (short)1, - (short)1, (short)117, (short)118,
(short)35, - (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1,
(short)42, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48,
(short)49, - (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55,
(short)56, (short)57, (short)58, - (short)1, (short)60, (short)61, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)83, (short)84, (short)85, (short)86, (short)87, - (short)1, (short)89, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)96,
- (short)1, - (short)1, - (short)1, (short)100, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, (short)106, - (short)1, - (short)1, (short)109,
- (short)1, (short)111, (short)112, - (short)1, - (short)1, - (short)1, -
(short)1, (short)117, (short)118, (short)35, - (short)1, (short)37, - (short)1,
(short)39, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)48, (short)49, - (short)1, - (short)1, (short)52,
(short)53, (short)54, (short)55, (short)56, (short)57, (short)58, - (short)1,
(short)60, (short)61, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)83, (short)84, (short)85, (short)86,
(short)87, - (short)1, (short)89, - (short)1, - (short)1, - (short)1, (short)93,
- (short)1, - (short)1, (short)96, - (short)1, - (short)1, - (short)1, (short)100,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)106, -
(short)1, - (short)1, (short)109, - (short)1, (short)111, (short)112, - (short)1,
- (short)1, - (short)1, - (short)1, (short)117, (short)118, (short)35, - (short)1,
(short)37, - (short)1, (short)39, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49, - (short)1,
- (short)1, (short)52, (short)53, (short)54, (short)55, (short)56, (short)57,
(short)58, - (short)1, (short)60, (short)61, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)83, (short)84,
(short)85, (short)86, (short)87, - (short)1, (short)89, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)96, - (short)1, - (short)1,
- (short)1, (short)100, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, (short)106, - (short)1, - (short)1, (short)109, - (short)1, (short)111,
(short)112, - (short)1, - (short)1, - (short)1, - (short)1, (short)117, (short)118,
(short)35, - (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48,
(short)49, - (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55,
(short)56, (short)57, (short)58, - (short)1, (short)60, (short)61, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)83, (short)84, (short)85, (short)86, (short)87, - (short)1, (short)89, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)96,
- (short)1, - (short)1, - (short)1, (short)100, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, (short)106, - (short)1, - (short)1, (short)109,
- (short)1, (short)111, (short)112, - (short)1, - (short)1, - (short)1, -
(short)1, (short)117, (short)118, (short)35, - (short)1, (short)37, - (short)1,
(short)39, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)48, (short)49, - (short)1, - (short)1, (short)52,
(short)53, (short)54, (short)55, (short)56, (short)57, (short)58, - (short)1,
(short)60, (short)61, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)37,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, (short)83, (short)84, (short)85, (short)86,
(short)87, - (short)1, (short)89, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)96, - (short)1, - (short)1, - (short)1, (short)100,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)106, -
(short)1, - (short)1, (short)109, - (short)1, (short)111, (short)112, - (short)1,
- (short)1, - (short)1, - (short)1, (short)117, (short)118, (short)83, (short)84,
- (short)1, (short)86, (short)87, - (short)1, (short)89, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)96, - (short)1, - (short)1,
(short)99, (short)100, - (short)1, - (short)1, - (short)1, (short)104, - (short)1,
(short)106, - (short)1, - (short)1, - (short)1, (short)110, (short)111, (short)112};
char Cyc_Yyimpossible_tag[ 13u]="Yyimpossible"; struct Cyc_Yyimpossible_struct{
char* tag; } ; char Cyc_Yystack_overflow_tag[ 17u]="Yystack_overflow"; struct
Cyc_Yystack_overflow_struct{ char* tag; } ; extern void Cyc_yyerror( struct
_tagged_string); extern int Cyc_yylex(); char Cyc_YYINITIALSVAL_tag[ 14u]="YYINITIALSVAL";
struct Cyc_YYINITIALSVAL_struct{ char* tag; } ; static int Cyc_yychar=( int)'\000';
static struct Cyc_YYINITIALSVAL_struct _temp1961={ Cyc_YYINITIALSVAL_tag};
struct _xenum_struct* Cyc_yylval=( struct _xenum_struct*)& _temp1961; static int
Cyc_yynerrs= 0; int Cyc_yyparse(){ int yystate; int yyn= 0; int yyerrstatus; int
yychar1= 0; int yyssp_offset; struct _tagged_ptr1 yyss=( struct _tagged_ptr1)({
short* _temp1964=( short*)({ unsigned int _temp1962= 200u; short* _temp1963=(
short*) GC_malloc_atomic( sizeof( short) * _temp1962); unsigned int i; for( i= 0;
i < _temp1962; i ++){ _temp1963[ i]= (short)0;} _temp1963;}); struct
_tagged_ptr1 _temp1965; _temp1965.curr= _temp1964; _temp1965.base= _temp1964;
_temp1965.last_plus_one= _temp1964 + 200; _temp1965;}); int yyvsp_offset; struct
_tagged_ptr2 yyvs=( struct _tagged_ptr2)({ struct _xenum_struct** _temp1968=(
struct _xenum_struct**)({ unsigned int _temp1966= 200u; struct _xenum_struct**
_temp1967=( struct _xenum_struct**) GC_malloc( sizeof( struct _xenum_struct*) *
_temp1966); unsigned int i; for( i= 0; i < _temp1966; i ++){ _temp1967[ i]= Cyc_yylval;}
_temp1967;}); struct _tagged_ptr2 _temp1969; _temp1969.curr= _temp1968;
_temp1969.base= _temp1968; _temp1969.last_plus_one= _temp1968 + 200; _temp1969;});
int yylsp_offset; struct _tagged_ptr3 yyls=( struct _tagged_ptr3)({ struct Cyc_Yyltype*
_temp1972=( struct Cyc_Yyltype*)({ unsigned int _temp1970= 200u; struct Cyc_Yyltype*
_temp1971=( struct Cyc_Yyltype*) GC_malloc( sizeof( struct Cyc_Yyltype) *
_temp1970); unsigned int i; for( i= 0; i < _temp1970; i ++){ _temp1971[ i]= Cyc_yynewloc();}
_temp1971;}); struct _tagged_ptr3 _temp1973; _temp1973.curr= _temp1972;
_temp1973.base= _temp1972; _temp1973.last_plus_one= _temp1972 + 200; _temp1973;});
int yystacksize= 200; struct _xenum_struct* yyval= Cyc_yylval; int yylen;
yystate= 0; yyerrstatus= 0; Cyc_yynerrs= 0; Cyc_yychar= - 2; yyssp_offset= - 1;
yyvsp_offset= 0; yylsp_offset= 0; yynewstate:({ struct _tagged_ptr1 _temp1974=
yyss; short* _temp1976= _temp1974.curr +( ++ yyssp_offset); if( _temp1976 <
_temp1974.base? 1: _temp1976 >= _temp1974.last_plus_one){ _throw( Null_Exception);}*
_temp1976=( short) yystate;}); if( yyssp_offset >= yystacksize - 1){ if(
yystacksize >= 10000){ Cyc_yyerror(( struct _tagged_string)({ char* _temp1977=(
char*)"parser stack overflow"; struct _tagged_string _temp1978; _temp1978.curr=
_temp1977; _temp1978.base= _temp1977; _temp1978.last_plus_one= _temp1977 + 22;
_temp1978;}));( void) _throw(({ struct Cyc_Yystack_overflow_struct* _temp1979=(
struct Cyc_Yystack_overflow_struct*) GC_malloc_atomic( sizeof( struct Cyc_Yystack_overflow_struct));
_temp1979->tag= Cyc_Yystack_overflow_tag;( struct _xenum_struct*) _temp1979;}));}
yystacksize *= 2; if( yystacksize > 10000){ yystacksize= 10000;}{ struct
_tagged_ptr1 yyss1=({ unsigned int _temp1980=( unsigned int) yystacksize; short*
_temp1981=( short*) GC_malloc_atomic( sizeof( short) * _temp1980); unsigned int
i; struct _tagged_ptr1 _temp1982={ _temp1981, _temp1981, _temp1981 + _temp1980};
for( i= 0; i < _temp1980; i ++){ _temp1981[ i]= (short)0;} _temp1982;}); struct
_tagged_ptr2 yyvs1=({ unsigned int _temp1983=( unsigned int) yystacksize; struct
_xenum_struct** _temp1984=( struct _xenum_struct**) GC_malloc( sizeof( struct
_xenum_struct*) * _temp1983); unsigned int i; struct _tagged_ptr2 _temp1985={
_temp1984, _temp1984, _temp1984 + _temp1983}; for( i= 0; i < _temp1983; i ++){
_temp1984[ i]= Cyc_yylval;} _temp1985;}); struct _tagged_ptr3 yyls1=({
unsigned int _temp1986=( unsigned int) yystacksize; struct Cyc_Yyltype*
_temp1987=( struct Cyc_Yyltype*) GC_malloc( sizeof( struct Cyc_Yyltype) *
_temp1986); unsigned int i; struct _tagged_ptr3 _temp1988={ _temp1987, _temp1987,
_temp1987 + _temp1986}; for( i= 0; i < _temp1986; i ++){ _temp1987[ i]= Cyc_yynewloc();}
_temp1988;});{ int i= 0; for( 0; i <= yyssp_offset; i ++){({ struct _tagged_ptr1
_temp1989= yyss1; short* _temp1991= _temp1989.curr + i; if( _temp1991 <
_temp1989.base? 1: _temp1991 >= _temp1989.last_plus_one){ _throw( Null_Exception);}*
_temp1991=({ struct _tagged_ptr1 _temp1992= yyss; short* _temp1994= _temp1992.curr
+ i; if( _temp1994 < _temp1992.base? 1: _temp1994 >= _temp1992.last_plus_one){
_throw( Null_Exception);}* _temp1994;});});({ struct _tagged_ptr2 _temp1995=
yyvs1; struct _xenum_struct** _temp1997= _temp1995.curr + i; if( _temp1997 <
_temp1995.base? 1: _temp1997 >= _temp1995.last_plus_one){ _throw( Null_Exception);}*
_temp1997=({ struct _tagged_ptr2 _temp1998= yyvs; struct _xenum_struct**
_temp2000= _temp1998.curr + i; if( _temp2000 < _temp1998.base? 1: _temp2000 >=
_temp1998.last_plus_one){ _throw( Null_Exception);}* _temp2000;});});({ struct
_tagged_ptr3 _temp2001= yyls1; struct Cyc_Yyltype* _temp2003= _temp2001.curr + i;
if( _temp2003 < _temp2001.base? 1: _temp2003 >= _temp2001.last_plus_one){ _throw(
Null_Exception);}* _temp2003=({ struct _tagged_ptr3 _temp2004= yyls; struct Cyc_Yyltype*
_temp2006= _temp2004.curr + i; if( _temp2006 < _temp2004.base? 1: _temp2006 >=
_temp2004.last_plus_one){ _throw( Null_Exception);}* _temp2006;});});}} yyss=
yyss1; yyvs= yyvs1; yyls= yyls1;}} goto yybackup; yybackup: yyn=( int)({ short*
_temp2007=( short*) Cyc_yypact; unsigned int _temp2008= yystate; if( _temp2008
>= 711u){ _throw( Null_Exception);} _temp2007[ _temp2008];}); if( yyn == - 32768){
goto yydefault;} if( Cyc_yychar == - 2){ Cyc_yychar= Cyc_yylex();} if( Cyc_yychar
<= 0){ yychar1= 0; Cyc_yychar= 0;} else{ yychar1=( Cyc_yychar > 0? Cyc_yychar <=
346: 0)?( int)({ short* _temp2009=( short*) Cyc_yytranslate; unsigned int
_temp2010= Cyc_yychar; if( _temp2010 >= 347u){ _throw( Null_Exception);}
_temp2009[ _temp2010];}): 221;} yyn += yychar1; if(( yyn < 0? 1: yyn > 4226)? 1:(
int)({ short* _temp2011=( short*) Cyc_yycheck; unsigned int _temp2012= yyn; if(
_temp2012 >= 4227u){ _throw( Null_Exception);} _temp2011[ _temp2012];}) !=
yychar1){ goto yydefault;} yyn=( int)({ short* _temp2013=( short*) Cyc_yytable;
unsigned int _temp2014= yyn; if( _temp2014 >= 4227u){ _throw( Null_Exception);}
_temp2013[ _temp2014];}); if( yyn < 0){ if( yyn == - 32768){ goto yyerrlab;} yyn=
- yyn; goto yyreduce;} else{ if( yyn == 0){ goto yyerrlab;}} if( yyn == 710){
return 0;} if( Cyc_yychar != 0){ Cyc_yychar= - 2;}({ struct _tagged_ptr2
_temp2015= yyvs; struct _xenum_struct** _temp2017= _temp2015.curr +( ++
yyvsp_offset); if( _temp2017 < _temp2015.base? 1: _temp2017 >= _temp2015.last_plus_one){
_throw( Null_Exception);}* _temp2017= Cyc_yylval;});({ struct _tagged_ptr3
_temp2018= yyls; struct Cyc_Yyltype* _temp2020= _temp2018.curr +( ++
yylsp_offset); if( _temp2020 < _temp2018.base? 1: _temp2020 >= _temp2018.last_plus_one){
_throw( Null_Exception);}* _temp2020= Cyc_yylloc;}); if( yyerrstatus != 0){
yyerrstatus --;} yystate= yyn; goto yynewstate; yydefault: yyn=( int)({ short*
_temp2021=( short*) Cyc_yydefact; unsigned int _temp2022= yystate; if( _temp2022
>= 711u){ _throw( Null_Exception);} _temp2021[ _temp2022];}); if( yyn == 0){
goto yyerrlab;} yyreduce: yylen=( int)({ short* _temp2023=( short*) Cyc_yyr2;
unsigned int _temp2024= yyn; if( _temp2024 >= 378u){ _throw( Null_Exception);}
_temp2023[ _temp2024];}); if( yylen > 0){ yyval=({ struct _tagged_ptr2 _temp2025=
yyvs; struct _xenum_struct** _temp2027= _temp2025.curr +(( yyvsp_offset + 1) -
yylen); if( _temp2027 < _temp2025.base? 1: _temp2027 >= _temp2025.last_plus_one){
_throw( Null_Exception);}* _temp2027;});} switch( yyn){ case 1: _LL2028: yyval=({
struct _tagged_ptr2 _temp2030= yyvs; struct _xenum_struct** _temp2032= _temp2030.curr
+ yyvsp_offset; if( _temp2032 < _temp2030.base? 1: _temp2032 >= _temp2030.last_plus_one){
_throw( Null_Exception);}* _temp2032;}); Cyc_Parse_parse_result= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2033= yyvs; struct _xenum_struct** _temp2035= _temp2033.curr
+ yyvsp_offset; if( _temp2035 < _temp2033.base? 1: _temp2035 >= _temp2033.last_plus_one){
_throw( Null_Exception);}* _temp2035;})); break; case 2: _LL2029: yyval=({
struct _tagged_ptr2 _temp2037= yyvs; struct _xenum_struct** _temp2039= _temp2037.curr
+ yyvsp_offset; if( _temp2039 < _temp2037.base? 1: _temp2039 >= _temp2037.last_plus_one){
_throw( Null_Exception);}* _temp2039;}); break; case 3: _LL2036: yyval=({ struct
Cyc_DeclList_tok_struct* _temp2041=( struct Cyc_DeclList_tok_struct*) GC_malloc(
sizeof( struct Cyc_DeclList_tok_struct)); _temp2041->tag= Cyc_DeclList_tok_tag;
_temp2041->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2
_temp2042= yyvs; struct _xenum_struct** _temp2044= _temp2042.curr +(
yyvsp_offset - 1); if( _temp2044 < _temp2042.base? 1: _temp2044 >= _temp2042.last_plus_one){
_throw( Null_Exception);}* _temp2044;})), Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp2045= yyvs; struct _xenum_struct** _temp2047= _temp2045.curr +
yyvsp_offset; if( _temp2047 < _temp2045.base? 1: _temp2047 >= _temp2045.last_plus_one){
_throw( Null_Exception);}* _temp2047;})));( struct _xenum_struct*) _temp2041;});
break; case 4: _LL2040: yyval=({ struct Cyc_DeclList_tok_struct* _temp2049=(
struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2049->tag= Cyc_DeclList_tok_tag; _temp2049->f1=({ struct Cyc_List_List*
_temp2050=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2050->hd=( void*)({ struct Cyc_Absyn_Decl* _temp2051=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp2051->r=( void*)({ struct Cyc_Absyn_Using_d_struct*
_temp2058=( struct Cyc_Absyn_Using_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp2058->tag= Cyc_Absyn_Using_d_tag; _temp2058->f1= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp2059= yyvs; struct _xenum_struct** _temp2061= _temp2059.curr
+( yyvsp_offset - 2); if( _temp2061 < _temp2059.base? 1: _temp2061 >= _temp2059.last_plus_one){
_throw( Null_Exception);}* _temp2061;})); _temp2058->f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2062= yyvs; struct _xenum_struct** _temp2064= _temp2062.curr
+ yyvsp_offset; if( _temp2064 < _temp2062.base? 1: _temp2064 >= _temp2062.last_plus_one){
_throw( Null_Exception);}* _temp2064;}));( void*) _temp2058;}); _temp2051->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2052= yyls; struct Cyc_Yyltype*
_temp2054= _temp2052.curr +( yylsp_offset - 2); if( _temp2054 < _temp2052.base?
1: _temp2054 >= _temp2052.last_plus_one){ _throw( Null_Exception);}* _temp2054;}).first_line,({
struct _tagged_ptr3 _temp2055= yyls; struct Cyc_Yyltype* _temp2057= _temp2055.curr
+ yylsp_offset; if( _temp2057 < _temp2055.base? 1: _temp2057 >= _temp2055.last_plus_one){
_throw( Null_Exception);}* _temp2057;}).last_line); _temp2051;}); _temp2050->tl=
0; _temp2050;});( struct _xenum_struct*) _temp2049;}); Cyc_Lex_leave_using();
break; case 5: _LL2048: yyval=({ struct Cyc_DeclList_tok_struct* _temp2066=(
struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2066->tag= Cyc_DeclList_tok_tag; _temp2066->f1=({ struct Cyc_List_List*
_temp2067=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2067->hd=( void*)({ struct Cyc_Absyn_Decl* _temp2071=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp2071->r=( void*)({ struct Cyc_Absyn_Using_d_struct*
_temp2078=( struct Cyc_Absyn_Using_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp2078->tag= Cyc_Absyn_Using_d_tag; _temp2078->f1= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp2079= yyvs; struct _xenum_struct** _temp2081= _temp2079.curr
+( yyvsp_offset - 4); if( _temp2081 < _temp2079.base? 1: _temp2081 >= _temp2079.last_plus_one){
_throw( Null_Exception);}* _temp2081;})); _temp2078->f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2082= yyvs; struct _xenum_struct** _temp2084= _temp2082.curr
+( yyvsp_offset - 2); if( _temp2084 < _temp2082.base? 1: _temp2084 >= _temp2082.last_plus_one){
_throw( Null_Exception);}* _temp2084;}));( void*) _temp2078;}); _temp2071->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2072= yyls; struct Cyc_Yyltype*
_temp2074= _temp2072.curr +( yylsp_offset - 4); if( _temp2074 < _temp2072.base?
1: _temp2074 >= _temp2072.last_plus_one){ _throw( Null_Exception);}* _temp2074;}).first_line,({
struct _tagged_ptr3 _temp2075= yyls; struct Cyc_Yyltype* _temp2077= _temp2075.curr
+( yylsp_offset - 1); if( _temp2077 < _temp2075.base? 1: _temp2077 >= _temp2075.last_plus_one){
_throw( Null_Exception);}* _temp2077;}).last_line); _temp2071;}); _temp2067->tl=
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2068= yyvs; struct
_xenum_struct** _temp2070= _temp2068.curr + yyvsp_offset; if( _temp2070 <
_temp2068.base? 1: _temp2070 >= _temp2068.last_plus_one){ _throw( Null_Exception);}*
_temp2070;})); _temp2067;});( struct _xenum_struct*) _temp2066;}); break; case 6:
_LL2065: yyval=({ struct Cyc_DeclList_tok_struct* _temp2086=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2086->tag= Cyc_DeclList_tok_tag;
_temp2086->f1=({ struct Cyc_List_List* _temp2087=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2087->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2088=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2088->r=( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp2095=( struct
Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp2095->tag= Cyc_Absyn_Namespace_d_tag; _temp2095->f1=({ struct
_tagged_string* _temp2096=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp2096[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp2097= yyvs; struct _xenum_struct** _temp2099= _temp2097.curr +(
yyvsp_offset - 2); if( _temp2099 < _temp2097.base? 1: _temp2099 >= _temp2097.last_plus_one){
_throw( Null_Exception);}* _temp2099;})); _temp2096;}); _temp2095->f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2100= yyvs; struct _xenum_struct** _temp2102= _temp2100.curr
+ yyvsp_offset; if( _temp2102 < _temp2100.base? 1: _temp2102 >= _temp2100.last_plus_one){
_throw( Null_Exception);}* _temp2102;}));( void*) _temp2095;}); _temp2088->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2089= yyls; struct Cyc_Yyltype*
_temp2091= _temp2089.curr +( yylsp_offset - 2); if( _temp2091 < _temp2089.base?
1: _temp2091 >= _temp2089.last_plus_one){ _throw( Null_Exception);}* _temp2091;}).first_line,({
struct _tagged_ptr3 _temp2092= yyls; struct Cyc_Yyltype* _temp2094= _temp2092.curr
+ yylsp_offset; if( _temp2094 < _temp2092.base? 1: _temp2094 >= _temp2092.last_plus_one){
_throw( Null_Exception);}* _temp2094;}).last_line); _temp2088;}); _temp2087->tl=
0; _temp2087;});( struct _xenum_struct*) _temp2086;}); Cyc_Lex_leave_namespace();
break; case 7: _LL2085: yyval=({ struct Cyc_DeclList_tok_struct* _temp2104=(
struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2104->tag= Cyc_DeclList_tok_tag; _temp2104->f1=({ struct Cyc_List_List*
_temp2105=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2105->hd=( void*)({ struct Cyc_Absyn_Decl* _temp2109=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp2109->r=( void*)({ struct Cyc_Absyn_Namespace_d_struct*
_temp2116=( struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp2116->tag= Cyc_Absyn_Namespace_d_tag; _temp2116->f1=({ struct
_tagged_string* _temp2117=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp2117[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp2118= yyvs; struct _xenum_struct** _temp2120= _temp2118.curr +(
yyvsp_offset - 4); if( _temp2120 < _temp2118.base? 1: _temp2120 >= _temp2118.last_plus_one){
_throw( Null_Exception);}* _temp2120;})); _temp2117;}); _temp2116->f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2121= yyvs; struct _xenum_struct** _temp2123= _temp2121.curr
+( yyvsp_offset - 2); if( _temp2123 < _temp2121.base? 1: _temp2123 >= _temp2121.last_plus_one){
_throw( Null_Exception);}* _temp2123;}));( void*) _temp2116;}); _temp2109->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2110= yyls; struct Cyc_Yyltype*
_temp2112= _temp2110.curr +( yylsp_offset - 4); if( _temp2112 < _temp2110.base?
1: _temp2112 >= _temp2110.last_plus_one){ _throw( Null_Exception);}* _temp2112;}).first_line,({
struct _tagged_ptr3 _temp2113= yyls; struct Cyc_Yyltype* _temp2115= _temp2113.curr
+( yylsp_offset - 1); if( _temp2115 < _temp2113.base? 1: _temp2115 >= _temp2113.last_plus_one){
_throw( Null_Exception);}* _temp2115;}).last_line); _temp2109;}); _temp2105->tl=
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2106= yyvs; struct
_xenum_struct** _temp2108= _temp2106.curr + yyvsp_offset; if( _temp2108 <
_temp2106.base? 1: _temp2108 >= _temp2106.last_plus_one){ _throw( Null_Exception);}*
_temp2108;})); _temp2105;});( struct _xenum_struct*) _temp2104;}); break; case 8:
_LL2103: if( Cyc_String_strcmp( Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp2125= yyvs; struct _xenum_struct** _temp2127= _temp2125.curr +(
yyvsp_offset - 4); if( _temp2127 < _temp2125.base? 1: _temp2127 >= _temp2125.last_plus_one){
_throw( Null_Exception);}* _temp2127;})),( struct _tagged_string)({ char*
_temp2128=( char*)"C"; struct _tagged_string _temp2129; _temp2129.curr=
_temp2128; _temp2129.base= _temp2128; _temp2129.last_plus_one= _temp2128 + 2;
_temp2129;})) != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp2130=(
char*)"only extern \"C\" { ... } is allowed"; struct _tagged_string _temp2131;
_temp2131.curr= _temp2130; _temp2131.base= _temp2130; _temp2131.last_plus_one=
_temp2130 + 35; _temp2131;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2132= yyls; struct Cyc_Yyltype* _temp2134= _temp2132.curr +( yylsp_offset -
5); if( _temp2134 < _temp2132.base? 1: _temp2134 >= _temp2132.last_plus_one){
_throw( Null_Exception);}* _temp2134;}).first_line,({ struct _tagged_ptr3
_temp2135= yyls; struct Cyc_Yyltype* _temp2137= _temp2135.curr +( yylsp_offset -
4); if( _temp2137 < _temp2135.base? 1: _temp2137 >= _temp2135.last_plus_one){
_throw( Null_Exception);}* _temp2137;}).last_line));} yyval=({ struct Cyc_DeclList_tok_struct*
_temp2138=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2138->tag= Cyc_DeclList_tok_tag; _temp2138->f1=({ struct Cyc_List_List*
_temp2139=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2139->hd=( void*)({ struct Cyc_Absyn_Decl* _temp2143=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp2143->r=( void*)({ struct Cyc_Absyn_ExternC_d_struct*
_temp2150=( struct Cyc_Absyn_ExternC_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ExternC_d_struct));
_temp2150->tag= Cyc_Absyn_ExternC_d_tag; _temp2150->f1= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2151= yyvs; struct _xenum_struct** _temp2153= _temp2151.curr
+( yyvsp_offset - 2); if( _temp2153 < _temp2151.base? 1: _temp2153 >= _temp2151.last_plus_one){
_throw( Null_Exception);}* _temp2153;}));( void*) _temp2150;}); _temp2143->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2144= yyls; struct Cyc_Yyltype*
_temp2146= _temp2144.curr +( yylsp_offset - 5); if( _temp2146 < _temp2144.base?
1: _temp2146 >= _temp2144.last_plus_one){ _throw( Null_Exception);}* _temp2146;}).first_line,({
struct _tagged_ptr3 _temp2147= yyls; struct Cyc_Yyltype* _temp2149= _temp2147.curr
+( yylsp_offset - 1); if( _temp2149 < _temp2147.base? 1: _temp2149 >= _temp2147.last_plus_one){
_throw( Null_Exception);}* _temp2149;}).last_line); _temp2143;}); _temp2139->tl=
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2140= yyvs; struct
_xenum_struct** _temp2142= _temp2140.curr + yyvsp_offset; if( _temp2142 <
_temp2140.base? 1: _temp2142 >= _temp2140.last_plus_one){ _throw( Null_Exception);}*
_temp2142;})); _temp2139;});( struct _xenum_struct*) _temp2138;}); break; case 9:
_LL2124: yyval=({ struct Cyc_DeclList_tok_struct* _temp2155=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2155->tag= Cyc_DeclList_tok_tag;
_temp2155->f1= 0;( struct _xenum_struct*) _temp2155;}); break; case 10: _LL2154:
yyval=({ struct _tagged_ptr2 _temp2157= yyvs; struct _xenum_struct** _temp2159=
_temp2157.curr + yyvsp_offset; if( _temp2159 < _temp2157.base? 1: _temp2159 >=
_temp2157.last_plus_one){ _throw( Null_Exception);}* _temp2159;}); break; case
11: _LL2156: yyval=({ struct Cyc_DeclList_tok_struct* _temp2161=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2161->tag= Cyc_DeclList_tok_tag;
_temp2161->f1=({ struct Cyc_List_List* _temp2162=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2162->hd=( void*) Cyc_Absyn_new_decl(({
struct Cyc_Absyn_Fn_d_struct* _temp2163=( struct Cyc_Absyn_Fn_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct)); _temp2163->tag= Cyc_Absyn_Fn_d_tag;
_temp2163->f1= Cyc_yyget_FnDecl_tok(({ struct _tagged_ptr2 _temp2164= yyvs;
struct _xenum_struct** _temp2166= _temp2164.curr + yyvsp_offset; if( _temp2166 <
_temp2164.base? 1: _temp2166 >= _temp2164.last_plus_one){ _throw( Null_Exception);}*
_temp2166;}));( void*) _temp2163;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2167= yyls; struct Cyc_Yyltype* _temp2169= _temp2167.curr +
yylsp_offset; if( _temp2169 < _temp2167.base? 1: _temp2169 >= _temp2167.last_plus_one){
_throw( Null_Exception);}* _temp2169;}).first_line,({ struct _tagged_ptr3
_temp2170= yyls; struct Cyc_Yyltype* _temp2172= _temp2170.curr + yylsp_offset;
if( _temp2172 < _temp2170.base? 1: _temp2172 >= _temp2170.last_plus_one){ _throw(
Null_Exception);}* _temp2172;}).last_line)); _temp2162->tl= 0; _temp2162;});(
struct _xenum_struct*) _temp2161;}); break; case 12: _LL2160: yyval=({ struct
_tagged_ptr2 _temp2174= yyvs; struct _xenum_struct** _temp2176= _temp2174.curr +
yyvsp_offset; if( _temp2176 < _temp2174.base? 1: _temp2176 >= _temp2174.last_plus_one){
_throw( Null_Exception);}* _temp2176;}); break; case 13: _LL2173: yyval=({
struct Cyc_FnDecl_tok_struct* _temp2178=( struct Cyc_FnDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp2178->tag= Cyc_FnDecl_tok_tag;
_temp2178->f1= Cyc_Parse_make_function( 0, Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp2179= yyvs; struct _xenum_struct** _temp2181= _temp2179.curr +(
yyvsp_offset - 1); if( _temp2181 < _temp2179.base? 1: _temp2181 >= _temp2179.last_plus_one){
_throw( Null_Exception);}* _temp2181;})), 0, Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp2182= yyvs; struct _xenum_struct** _temp2184= _temp2182.curr +
yyvsp_offset; if( _temp2184 < _temp2182.base? 1: _temp2184 >= _temp2182.last_plus_one){
_throw( Null_Exception);}* _temp2184;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2185= yyls; struct Cyc_Yyltype* _temp2187= _temp2185.curr +(
yylsp_offset - 1); if( _temp2187 < _temp2185.base? 1: _temp2187 >= _temp2185.last_plus_one){
_throw( Null_Exception);}* _temp2187;}).first_line,({ struct _tagged_ptr3
_temp2188= yyls; struct Cyc_Yyltype* _temp2190= _temp2188.curr + yylsp_offset;
if( _temp2190 < _temp2188.base? 1: _temp2190 >= _temp2188.last_plus_one){ _throw(
Null_Exception);}* _temp2190;}).last_line));( struct _xenum_struct*) _temp2178;});
break; case 14: _LL2177: yyval=({ struct Cyc_FnDecl_tok_struct* _temp2192=(
struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2192->tag= Cyc_FnDecl_tok_tag; _temp2192->f1= Cyc_Parse_make_function(({
struct Cyc_Core_Opt* _temp2193=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2193->v=( void*) Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2194= yyvs; struct _xenum_struct** _temp2196= _temp2194.curr +(
yyvsp_offset - 2); if( _temp2196 < _temp2194.base? 1: _temp2196 >= _temp2194.last_plus_one){
_throw( Null_Exception);}* _temp2196;})); _temp2193;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2197= yyvs; struct _xenum_struct** _temp2199= _temp2197.curr
+( yyvsp_offset - 1); if( _temp2199 < _temp2197.base? 1: _temp2199 >= _temp2197.last_plus_one){
_throw( Null_Exception);}* _temp2199;})), 0, Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp2200= yyvs; struct _xenum_struct** _temp2202= _temp2200.curr +
yyvsp_offset; if( _temp2202 < _temp2200.base? 1: _temp2202 >= _temp2200.last_plus_one){
_throw( Null_Exception);}* _temp2202;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2203= yyls; struct Cyc_Yyltype* _temp2205= _temp2203.curr +(
yylsp_offset - 2); if( _temp2205 < _temp2203.base? 1: _temp2205 >= _temp2203.last_plus_one){
_throw( Null_Exception);}* _temp2205;}).first_line,({ struct _tagged_ptr3
_temp2206= yyls; struct Cyc_Yyltype* _temp2208= _temp2206.curr + yylsp_offset;
if( _temp2208 < _temp2206.base? 1: _temp2208 >= _temp2206.last_plus_one){ _throw(
Null_Exception);}* _temp2208;}).last_line));( struct _xenum_struct*) _temp2192;});
break; case 15: _LL2191: yyval=({ struct Cyc_FnDecl_tok_struct* _temp2210=(
struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2210->tag= Cyc_FnDecl_tok_tag; _temp2210->f1= Cyc_Parse_make_function( 0,
Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp2211= yyvs; struct
_xenum_struct** _temp2213= _temp2211.curr +( yyvsp_offset - 2); if( _temp2213 <
_temp2211.base? 1: _temp2213 >= _temp2211.last_plus_one){ _throw( Null_Exception);}*
_temp2213;})), Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2214= yyvs;
struct _xenum_struct** _temp2216= _temp2214.curr +( yyvsp_offset - 1); if(
_temp2216 < _temp2214.base? 1: _temp2216 >= _temp2214.last_plus_one){ _throw(
Null_Exception);}* _temp2216;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp2217= yyvs; struct _xenum_struct** _temp2219= _temp2217.curr + yyvsp_offset;
if( _temp2219 < _temp2217.base? 1: _temp2219 >= _temp2217.last_plus_one){ _throw(
Null_Exception);}* _temp2219;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2220= yyls; struct Cyc_Yyltype* _temp2222= _temp2220.curr +(
yylsp_offset - 2); if( _temp2222 < _temp2220.base? 1: _temp2222 >= _temp2220.last_plus_one){
_throw( Null_Exception);}* _temp2222;}).first_line,({ struct _tagged_ptr3
_temp2223= yyls; struct Cyc_Yyltype* _temp2225= _temp2223.curr + yylsp_offset;
if( _temp2225 < _temp2223.base? 1: _temp2225 >= _temp2223.last_plus_one){ _throw(
Null_Exception);}* _temp2225;}).last_line));( struct _xenum_struct*) _temp2210;});
break; case 16: _LL2209: yyval=({ struct Cyc_FnDecl_tok_struct* _temp2227=(
struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2227->tag= Cyc_FnDecl_tok_tag; _temp2227->f1= Cyc_Parse_make_function(({
struct Cyc_Core_Opt* _temp2228=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2228->v=( void*) Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2229= yyvs; struct _xenum_struct** _temp2231= _temp2229.curr +(
yyvsp_offset - 3); if( _temp2231 < _temp2229.base? 1: _temp2231 >= _temp2229.last_plus_one){
_throw( Null_Exception);}* _temp2231;})); _temp2228;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2232= yyvs; struct _xenum_struct** _temp2234= _temp2232.curr
+( yyvsp_offset - 2); if( _temp2234 < _temp2232.base? 1: _temp2234 >= _temp2232.last_plus_one){
_throw( Null_Exception);}* _temp2234;})), Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp2235= yyvs; struct _xenum_struct** _temp2237= _temp2235.curr +(
yyvsp_offset - 1); if( _temp2237 < _temp2235.base? 1: _temp2237 >= _temp2235.last_plus_one){
_throw( Null_Exception);}* _temp2237;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp2238= yyvs; struct _xenum_struct** _temp2240= _temp2238.curr +
yyvsp_offset; if( _temp2240 < _temp2238.base? 1: _temp2240 >= _temp2238.last_plus_one){
_throw( Null_Exception);}* _temp2240;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2241= yyls; struct Cyc_Yyltype* _temp2243= _temp2241.curr +(
yylsp_offset - 3); if( _temp2243 < _temp2241.base? 1: _temp2243 >= _temp2241.last_plus_one){
_throw( Null_Exception);}* _temp2243;}).first_line,({ struct _tagged_ptr3
_temp2244= yyls; struct Cyc_Yyltype* _temp2246= _temp2244.curr + yylsp_offset;
if( _temp2246 < _temp2244.base? 1: _temp2246 >= _temp2244.last_plus_one){ _throw(
Null_Exception);}* _temp2246;}).last_line));( struct _xenum_struct*) _temp2227;});
break; case 17: _LL2226: yyval=({ struct Cyc_FnDecl_tok_struct* _temp2248=(
struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2248->tag= Cyc_FnDecl_tok_tag; _temp2248->f1= Cyc_Parse_make_function(({
struct Cyc_Core_Opt* _temp2249=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2249->v=( void*) Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2250= yyvs; struct _xenum_struct** _temp2252= _temp2250.curr +(
yyvsp_offset - 2); if( _temp2252 < _temp2250.base? 1: _temp2252 >= _temp2250.last_plus_one){
_throw( Null_Exception);}* _temp2252;})); _temp2249;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2253= yyvs; struct _xenum_struct** _temp2255= _temp2253.curr
+( yyvsp_offset - 1); if( _temp2255 < _temp2253.base? 1: _temp2255 >= _temp2253.last_plus_one){
_throw( Null_Exception);}* _temp2255;})), 0, Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp2256= yyvs; struct _xenum_struct** _temp2258= _temp2256.curr +
yyvsp_offset; if( _temp2258 < _temp2256.base? 1: _temp2258 >= _temp2256.last_plus_one){
_throw( Null_Exception);}* _temp2258;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2259= yyls; struct Cyc_Yyltype* _temp2261= _temp2259.curr +(
yylsp_offset - 2); if( _temp2261 < _temp2259.base? 1: _temp2261 >= _temp2259.last_plus_one){
_throw( Null_Exception);}* _temp2261;}).first_line,({ struct _tagged_ptr3
_temp2262= yyls; struct Cyc_Yyltype* _temp2264= _temp2262.curr + yylsp_offset;
if( _temp2264 < _temp2262.base? 1: _temp2264 >= _temp2262.last_plus_one){ _throw(
Null_Exception);}* _temp2264;}).last_line));( struct _xenum_struct*) _temp2248;});
break; case 18: _LL2247: yyval=({ struct Cyc_FnDecl_tok_struct* _temp2266=(
struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2266->tag= Cyc_FnDecl_tok_tag; _temp2266->f1= Cyc_Parse_make_function(({
struct Cyc_Core_Opt* _temp2267=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2267->v=( void*) Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2268= yyvs; struct _xenum_struct** _temp2270= _temp2268.curr +(
yyvsp_offset - 3); if( _temp2270 < _temp2268.base? 1: _temp2270 >= _temp2268.last_plus_one){
_throw( Null_Exception);}* _temp2270;})); _temp2267;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2271= yyvs; struct _xenum_struct** _temp2273= _temp2271.curr
+( yyvsp_offset - 2); if( _temp2273 < _temp2271.base? 1: _temp2273 >= _temp2271.last_plus_one){
_throw( Null_Exception);}* _temp2273;})), Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp2274= yyvs; struct _xenum_struct** _temp2276= _temp2274.curr +(
yyvsp_offset - 1); if( _temp2276 < _temp2274.base? 1: _temp2276 >= _temp2274.last_plus_one){
_throw( Null_Exception);}* _temp2276;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp2277= yyvs; struct _xenum_struct** _temp2279= _temp2277.curr +
yyvsp_offset; if( _temp2279 < _temp2277.base? 1: _temp2279 >= _temp2277.last_plus_one){
_throw( Null_Exception);}* _temp2279;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2280= yyls; struct Cyc_Yyltype* _temp2282= _temp2280.curr +(
yylsp_offset - 3); if( _temp2282 < _temp2280.base? 1: _temp2282 >= _temp2280.last_plus_one){
_throw( Null_Exception);}* _temp2282;}).first_line,({ struct _tagged_ptr3
_temp2283= yyls; struct Cyc_Yyltype* _temp2285= _temp2283.curr + yylsp_offset;
if( _temp2285 < _temp2283.base? 1: _temp2285 >= _temp2283.last_plus_one){ _throw(
Null_Exception);}* _temp2285;}).last_line));( struct _xenum_struct*) _temp2266;});
break; case 19: _LL2265: Cyc_Lex_enter_using( Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp2287= yyvs; struct _xenum_struct** _temp2289= _temp2287.curr +
yyvsp_offset; if( _temp2289 < _temp2287.base? 1: _temp2289 >= _temp2287.last_plus_one){
_throw( Null_Exception);}* _temp2289;}))); yyval=({ struct _tagged_ptr2
_temp2290= yyvs; struct _xenum_struct** _temp2292= _temp2290.curr + yyvsp_offset;
if( _temp2292 < _temp2290.base? 1: _temp2292 >= _temp2290.last_plus_one){ _throw(
Null_Exception);}* _temp2292;}); break; case 20: _LL2286: Cyc_Lex_leave_using();
break; case 21: _LL2293: Cyc_Lex_enter_namespace(({ struct _tagged_string*
_temp2295=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp2295[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2296= yyvs;
struct _xenum_struct** _temp2298= _temp2296.curr + yyvsp_offset; if( _temp2298 <
_temp2296.base? 1: _temp2298 >= _temp2296.last_plus_one){ _throw( Null_Exception);}*
_temp2298;})); _temp2295;})); yyval=({ struct _tagged_ptr2 _temp2299= yyvs;
struct _xenum_struct** _temp2301= _temp2299.curr + yyvsp_offset; if( _temp2301 <
_temp2299.base? 1: _temp2301 >= _temp2299.last_plus_one){ _throw( Null_Exception);}*
_temp2301;}); break; case 22: _LL2294: Cyc_Lex_leave_namespace(); break; case 23:
_LL2302: yyval=({ struct Cyc_DeclList_tok_struct* _temp2304=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2304->tag= Cyc_DeclList_tok_tag;
_temp2304->f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2305= yyvs; struct _xenum_struct** _temp2307= _temp2305.curr +(
yyvsp_offset - 1); if( _temp2307 < _temp2305.base? 1: _temp2307 >= _temp2305.last_plus_one){
_throw( Null_Exception);}* _temp2307;})), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2308= yyls; struct Cyc_Yyltype* _temp2310= _temp2308.curr
+( yylsp_offset - 1); if( _temp2310 < _temp2308.base? 1: _temp2310 >= _temp2308.last_plus_one){
_throw( Null_Exception);}* _temp2310;}).first_line,({ struct _tagged_ptr3
_temp2311= yyls; struct Cyc_Yyltype* _temp2313= _temp2311.curr +( yylsp_offset -
1); if( _temp2313 < _temp2311.base? 1: _temp2313 >= _temp2311.last_plus_one){
_throw( Null_Exception);}* _temp2313;}).last_line));( struct _xenum_struct*)
_temp2304;}); break; case 24: _LL2303: yyval=({ struct Cyc_DeclList_tok_struct*
_temp2315=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2315->tag= Cyc_DeclList_tok_tag; _temp2315->f1= Cyc_Parse_make_declarations(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2316= yyvs; struct
_xenum_struct** _temp2318= _temp2316.curr +( yyvsp_offset - 2); if( _temp2318 <
_temp2316.base? 1: _temp2318 >= _temp2316.last_plus_one){ _throw( Null_Exception);}*
_temp2318;})), Cyc_yyget_InitDeclList_tok(({ struct _tagged_ptr2 _temp2319= yyvs;
struct _xenum_struct** _temp2321= _temp2319.curr +( yyvsp_offset - 1); if(
_temp2321 < _temp2319.base? 1: _temp2321 >= _temp2319.last_plus_one){ _throw(
Null_Exception);}* _temp2321;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2322= yyls; struct Cyc_Yyltype* _temp2324= _temp2322.curr +(
yylsp_offset - 2); if( _temp2324 < _temp2322.base? 1: _temp2324 >= _temp2322.last_plus_one){
_throw( Null_Exception);}* _temp2324;}).first_line,({ struct _tagged_ptr3
_temp2325= yyls; struct Cyc_Yyltype* _temp2327= _temp2325.curr + yylsp_offset;
if( _temp2327 < _temp2325.base? 1: _temp2327 >= _temp2325.last_plus_one){ _throw(
Null_Exception);}* _temp2327;}).last_line));( struct _xenum_struct*) _temp2315;});
break; case 25: _LL2314: yyval=({ struct Cyc_DeclList_tok_struct* _temp2329=(
struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2329->tag= Cyc_DeclList_tok_tag; _temp2329->f1=({ struct Cyc_List_List*
_temp2330=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2330->hd=( void*) Cyc_Absyn_let_decl( Cyc_yyget_Pattern_tok(({ struct
_tagged_ptr2 _temp2331= yyvs; struct _xenum_struct** _temp2333= _temp2331.curr +(
yyvsp_offset - 3); if( _temp2333 < _temp2331.base? 1: _temp2333 >= _temp2331.last_plus_one){
_throw( Null_Exception);}* _temp2333;})), 0, Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp2334= yyvs; struct _xenum_struct** _temp2336= _temp2334.curr +(
yyvsp_offset - 1); if( _temp2336 < _temp2334.base? 1: _temp2336 >= _temp2334.last_plus_one){
_throw( Null_Exception);}* _temp2336;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2337= yyls; struct Cyc_Yyltype* _temp2339= _temp2337.curr +(
yylsp_offset - 4); if( _temp2339 < _temp2337.base? 1: _temp2339 >= _temp2337.last_plus_one){
_throw( Null_Exception);}* _temp2339;}).first_line,({ struct _tagged_ptr3
_temp2340= yyls; struct Cyc_Yyltype* _temp2342= _temp2340.curr + yylsp_offset;
if( _temp2342 < _temp2340.base? 1: _temp2342 >= _temp2340.last_plus_one){ _throw(
Null_Exception);}* _temp2342;}).last_line)); _temp2330->tl= 0; _temp2330;});(
struct _xenum_struct*) _temp2329;}); break; case 26: _LL2328: yyval=({ struct
_tagged_ptr2 _temp2344= yyvs; struct _xenum_struct** _temp2346= _temp2344.curr +
yyvsp_offset; if( _temp2346 < _temp2344.base? 1: _temp2346 >= _temp2344.last_plus_one){
_throw( Null_Exception);}* _temp2346;}); break; case 27: _LL2343: yyval=({
struct Cyc_DeclList_tok_struct* _temp2348=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2348->tag= Cyc_DeclList_tok_tag;
_temp2348->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2
_temp2349= yyvs; struct _xenum_struct** _temp2351= _temp2349.curr +(
yyvsp_offset - 1); if( _temp2351 < _temp2349.base? 1: _temp2351 >= _temp2349.last_plus_one){
_throw( Null_Exception);}* _temp2351;})), Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp2352= yyvs; struct _xenum_struct** _temp2354= _temp2352.curr +
yyvsp_offset; if( _temp2354 < _temp2352.base? 1: _temp2354 >= _temp2352.last_plus_one){
_throw( Null_Exception);}* _temp2354;})));( struct _xenum_struct*) _temp2348;});
break; case 28: _LL2347: yyval=({ struct Cyc_DeclSpec_tok_struct* _temp2356=(
struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2356->tag= Cyc_DeclSpec_tok_tag; _temp2356->f1=({ struct Cyc_Parse_Declaration_spec*
_temp2357=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2357->sc=({ struct Cyc_Core_Opt* _temp2361=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2361->v=( void*) Cyc_yyget_StorageClass_tok(({
struct _tagged_ptr2 _temp2362= yyvs; struct _xenum_struct** _temp2364= _temp2362.curr
+( yyvsp_offset - 1); if( _temp2364 < _temp2362.base? 1: _temp2364 >= _temp2362.last_plus_one){
_throw( Null_Exception);}* _temp2364;})); _temp2361;}); _temp2357->tq= Cyc_Absyn_empty_tqual();
_temp2357->type_specs= 0; _temp2357->is_inline= 0; _temp2357->attributes= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2358= yyvs; struct _xenum_struct** _temp2360= _temp2358.curr
+ yyvsp_offset; if( _temp2360 < _temp2358.base? 1: _temp2360 >= _temp2358.last_plus_one){
_throw( Null_Exception);}* _temp2360;})); _temp2357;});( struct _xenum_struct*)
_temp2356;}); break; case 29: _LL2355: if(( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2366= yyvs; struct _xenum_struct** _temp2368= _temp2366.curr +
yyvsp_offset; if( _temp2368 < _temp2366.base? 1: _temp2368 >= _temp2366.last_plus_one){
_throw( Null_Exception);}* _temp2368;})))->sc != 0){ Cyc_Parse_warn(( struct
_tagged_string)({ char* _temp2369=( char*)"Only one storage class is allowed in a declaration";
struct _tagged_string _temp2370; _temp2370.curr= _temp2369; _temp2370.base=
_temp2369; _temp2370.last_plus_one= _temp2369 + 51; _temp2370;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2371= yyls; struct Cyc_Yyltype* _temp2373= _temp2371.curr
+( yylsp_offset - 2); if( _temp2373 < _temp2371.base? 1: _temp2373 >= _temp2371.last_plus_one){
_throw( Null_Exception);}* _temp2373;}).first_line,({ struct _tagged_ptr3
_temp2374= yyls; struct Cyc_Yyltype* _temp2376= _temp2374.curr +( yylsp_offset -
1); if( _temp2376 < _temp2374.base? 1: _temp2376 >= _temp2374.last_plus_one){
_throw( Null_Exception);}* _temp2376;}).last_line));} yyval=({ struct Cyc_DeclSpec_tok_struct*
_temp2377=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2377->tag= Cyc_DeclSpec_tok_tag; _temp2377->f1=({ struct Cyc_Parse_Declaration_spec*
_temp2378=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2378->sc=({ struct Cyc_Core_Opt* _temp2394=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2394->v=( void*) Cyc_yyget_StorageClass_tok(({
struct _tagged_ptr2 _temp2395= yyvs; struct _xenum_struct** _temp2397= _temp2395.curr
+( yyvsp_offset - 2); if( _temp2397 < _temp2395.base? 1: _temp2397 >= _temp2395.last_plus_one){
_throw( Null_Exception);}* _temp2397;})); _temp2394;}); _temp2378->tq=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2391= yyvs; struct _xenum_struct** _temp2393= _temp2391.curr
+ yyvsp_offset; if( _temp2393 < _temp2391.base? 1: _temp2393 >= _temp2391.last_plus_one){
_throw( Null_Exception);}* _temp2393;})))->tq; _temp2378->type_specs=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2388= yyvs; struct _xenum_struct** _temp2390= _temp2388.curr
+ yyvsp_offset; if( _temp2390 < _temp2388.base? 1: _temp2390 >= _temp2388.last_plus_one){
_throw( Null_Exception);}* _temp2390;})))->type_specs; _temp2378->is_inline=(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2385= yyvs; struct
_xenum_struct** _temp2387= _temp2385.curr + yyvsp_offset; if( _temp2387 <
_temp2385.base? 1: _temp2387 >= _temp2385.last_plus_one){ _throw( Null_Exception);}*
_temp2387;})))->is_inline; _temp2378->attributes=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2379= yyvs; struct _xenum_struct** _temp2381= _temp2379.curr
+( yyvsp_offset - 1); if( _temp2381 < _temp2379.base? 1: _temp2381 >= _temp2379.last_plus_one){
_throw( Null_Exception);}* _temp2381;})),( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2382= yyvs; struct _xenum_struct** _temp2384= _temp2382.curr +
yyvsp_offset; if( _temp2384 < _temp2382.base? 1: _temp2384 >= _temp2382.last_plus_one){
_throw( Null_Exception);}* _temp2384;})))->attributes); _temp2378;});( struct
_xenum_struct*) _temp2377;}); break; case 30: _LL2365: yyval=({ struct Cyc_DeclSpec_tok_struct*
_temp2399=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2399->tag= Cyc_DeclSpec_tok_tag; _temp2399->f1=({ struct Cyc_Parse_Declaration_spec*
_temp2400=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2400->sc= 0; _temp2400->tq= Cyc_Absyn_empty_tqual(); _temp2400->type_specs=({
struct Cyc_List_List* _temp2404=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2404->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({
struct _tagged_ptr2 _temp2405= yyvs; struct _xenum_struct** _temp2407= _temp2405.curr
+( yyvsp_offset - 1); if( _temp2407 < _temp2405.base? 1: _temp2407 >= _temp2405.last_plus_one){
_throw( Null_Exception);}* _temp2407;})); _temp2404->tl= 0; _temp2404;});
_temp2400->is_inline= 0; _temp2400->attributes= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2401= yyvs; struct _xenum_struct** _temp2403= _temp2401.curr
+ yyvsp_offset; if( _temp2403 < _temp2401.base? 1: _temp2403 >= _temp2401.last_plus_one){
_throw( Null_Exception);}* _temp2403;})); _temp2400;});( struct _xenum_struct*)
_temp2399;}); break; case 31: _LL2398: yyval=({ struct Cyc_DeclSpec_tok_struct*
_temp2409=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2409->tag= Cyc_DeclSpec_tok_tag; _temp2409->f1=({ struct Cyc_Parse_Declaration_spec*
_temp2410=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2410->sc=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2430= yyvs;
struct _xenum_struct** _temp2432= _temp2430.curr + yyvsp_offset; if( _temp2432 <
_temp2430.base? 1: _temp2432 >= _temp2430.last_plus_one){ _throw( Null_Exception);}*
_temp2432;})))->sc; _temp2410->tq=( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2427= yyvs; struct _xenum_struct** _temp2429= _temp2427.curr +
yyvsp_offset; if( _temp2429 < _temp2427.base? 1: _temp2429 >= _temp2427.last_plus_one){
_throw( Null_Exception);}* _temp2429;})))->tq; _temp2410->type_specs=({ struct
Cyc_List_List* _temp2420=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2420->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({ struct _tagged_ptr2
_temp2424= yyvs; struct _xenum_struct** _temp2426= _temp2424.curr +(
yyvsp_offset - 2); if( _temp2426 < _temp2424.base? 1: _temp2426 >= _temp2424.last_plus_one){
_throw( Null_Exception);}* _temp2426;})); _temp2420->tl=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2421= yyvs; struct _xenum_struct** _temp2423= _temp2421.curr
+ yyvsp_offset; if( _temp2423 < _temp2421.base? 1: _temp2423 >= _temp2421.last_plus_one){
_throw( Null_Exception);}* _temp2423;})))->type_specs; _temp2420;}); _temp2410->is_inline=(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2417= yyvs; struct
_xenum_struct** _temp2419= _temp2417.curr + yyvsp_offset; if( _temp2419 <
_temp2417.base? 1: _temp2419 >= _temp2417.last_plus_one){ _throw( Null_Exception);}*
_temp2419;})))->is_inline; _temp2410->attributes=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2411= yyvs; struct _xenum_struct** _temp2413= _temp2411.curr
+( yyvsp_offset - 1); if( _temp2413 < _temp2411.base? 1: _temp2413 >= _temp2411.last_plus_one){
_throw( Null_Exception);}* _temp2413;})),( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2414= yyvs; struct _xenum_struct** _temp2416= _temp2414.curr +
yyvsp_offset; if( _temp2416 < _temp2414.base? 1: _temp2416 >= _temp2414.last_plus_one){
_throw( Null_Exception);}* _temp2416;})))->attributes); _temp2410;});( struct
_xenum_struct*) _temp2409;}); break; case 32: _LL2408: yyval=({ struct Cyc_DeclSpec_tok_struct*
_temp2434=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2434->tag= Cyc_DeclSpec_tok_tag; _temp2434->f1=({ struct Cyc_Parse_Declaration_spec*
_temp2435=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2435->sc= 0; _temp2435->tq= Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2
_temp2439= yyvs; struct _xenum_struct** _temp2441= _temp2439.curr +(
yyvsp_offset - 1); if( _temp2441 < _temp2439.base? 1: _temp2441 >= _temp2439.last_plus_one){
_throw( Null_Exception);}* _temp2441;})); _temp2435->type_specs= 0; _temp2435->is_inline=
0; _temp2435->attributes= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2
_temp2436= yyvs; struct _xenum_struct** _temp2438= _temp2436.curr + yyvsp_offset;
if( _temp2438 < _temp2436.base? 1: _temp2438 >= _temp2436.last_plus_one){ _throw(
Null_Exception);}* _temp2438;})); _temp2435;});( struct _xenum_struct*)
_temp2434;}); break; case 33: _LL2433: yyval=({ struct Cyc_DeclSpec_tok_struct*
_temp2443=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2443->tag= Cyc_DeclSpec_tok_tag; _temp2443->f1=({ struct Cyc_Parse_Declaration_spec*
_temp2444=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2444->sc=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2463= yyvs;
struct _xenum_struct** _temp2465= _temp2463.curr + yyvsp_offset; if( _temp2465 <
_temp2463.base? 1: _temp2465 >= _temp2463.last_plus_one){ _throw( Null_Exception);}*
_temp2465;})))->sc; _temp2444->tq= Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok(({
struct _tagged_ptr2 _temp2457= yyvs; struct _xenum_struct** _temp2459= _temp2457.curr
+( yyvsp_offset - 2); if( _temp2459 < _temp2457.base? 1: _temp2459 >= _temp2457.last_plus_one){
_throw( Null_Exception);}* _temp2459;})),( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2460= yyvs; struct _xenum_struct** _temp2462= _temp2460.curr +
yyvsp_offset; if( _temp2462 < _temp2460.base? 1: _temp2462 >= _temp2460.last_plus_one){
_throw( Null_Exception);}* _temp2462;})))->tq); _temp2444->type_specs=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2454= yyvs; struct _xenum_struct** _temp2456= _temp2454.curr
+ yyvsp_offset; if( _temp2456 < _temp2454.base? 1: _temp2456 >= _temp2454.last_plus_one){
_throw( Null_Exception);}* _temp2456;})))->type_specs; _temp2444->is_inline=(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2451= yyvs; struct
_xenum_struct** _temp2453= _temp2451.curr + yyvsp_offset; if( _temp2453 <
_temp2451.base? 1: _temp2453 >= _temp2451.last_plus_one){ _throw( Null_Exception);}*
_temp2453;})))->is_inline; _temp2444->attributes=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2445= yyvs; struct _xenum_struct** _temp2447= _temp2445.curr
+( yyvsp_offset - 1); if( _temp2447 < _temp2445.base? 1: _temp2447 >= _temp2445.last_plus_one){
_throw( Null_Exception);}* _temp2447;})),( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2448= yyvs; struct _xenum_struct** _temp2450= _temp2448.curr +
yyvsp_offset; if( _temp2450 < _temp2448.base? 1: _temp2450 >= _temp2448.last_plus_one){
_throw( Null_Exception);}* _temp2450;})))->attributes); _temp2444;});( struct
_xenum_struct*) _temp2443;}); break; case 34: _LL2442: yyval=({ struct Cyc_DeclSpec_tok_struct*
_temp2467=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2467->tag= Cyc_DeclSpec_tok_tag; _temp2467->f1=({ struct Cyc_Parse_Declaration_spec*
_temp2468=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2468->sc= 0; _temp2468->tq= Cyc_Absyn_empty_tqual(); _temp2468->type_specs=
0; _temp2468->is_inline= 1; _temp2468->attributes= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2469= yyvs; struct _xenum_struct** _temp2471= _temp2469.curr
+ yyvsp_offset; if( _temp2471 < _temp2469.base? 1: _temp2471 >= _temp2469.last_plus_one){
_throw( Null_Exception);}* _temp2471;})); _temp2468;});( struct _xenum_struct*)
_temp2467;}); break; case 35: _LL2466: yyval=({ struct Cyc_DeclSpec_tok_struct*
_temp2473=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2473->tag= Cyc_DeclSpec_tok_tag; _temp2473->f1=({ struct Cyc_Parse_Declaration_spec*
_temp2474=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2474->sc=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2487= yyvs;
struct _xenum_struct** _temp2489= _temp2487.curr + yyvsp_offset; if( _temp2489 <
_temp2487.base? 1: _temp2489 >= _temp2487.last_plus_one){ _throw( Null_Exception);}*
_temp2489;})))->sc; _temp2474->tq=( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2484= yyvs; struct _xenum_struct** _temp2486= _temp2484.curr +
yyvsp_offset; if( _temp2486 < _temp2484.base? 1: _temp2486 >= _temp2484.last_plus_one){
_throw( Null_Exception);}* _temp2486;})))->tq; _temp2474->type_specs=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2481= yyvs; struct _xenum_struct** _temp2483= _temp2481.curr
+ yyvsp_offset; if( _temp2483 < _temp2481.base? 1: _temp2483 >= _temp2481.last_plus_one){
_throw( Null_Exception);}* _temp2483;})))->type_specs; _temp2474->is_inline= 1;
_temp2474->attributes=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2475= yyvs; struct _xenum_struct** _temp2477= _temp2475.curr
+( yyvsp_offset - 1); if( _temp2477 < _temp2475.base? 1: _temp2477 >= _temp2475.last_plus_one){
_throw( Null_Exception);}* _temp2477;})),( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2478= yyvs; struct _xenum_struct** _temp2480= _temp2478.curr +
yyvsp_offset; if( _temp2480 < _temp2478.base? 1: _temp2480 >= _temp2478.last_plus_one){
_throw( Null_Exception);}* _temp2480;})))->attributes); _temp2474;});( struct
_xenum_struct*) _temp2473;}); break; case 36: _LL2472: yyval=({ struct Cyc_StorageClass_tok_struct*
_temp2491=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2491->tag= Cyc_StorageClass_tok_tag; _temp2491->f1=( void*) Cyc_Parse_Auto_sc;(
struct _xenum_struct*) _temp2491;}); break; case 37: _LL2490: yyval=({ struct
Cyc_StorageClass_tok_struct* _temp2493=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp2493->tag= Cyc_StorageClass_tok_tag;
_temp2493->f1=( void*) Cyc_Parse_Register_sc;( struct _xenum_struct*) _temp2493;});
break; case 38: _LL2492: yyval=({ struct Cyc_StorageClass_tok_struct* _temp2495=(
struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2495->tag= Cyc_StorageClass_tok_tag; _temp2495->f1=( void*) Cyc_Parse_Static_sc;(
struct _xenum_struct*) _temp2495;}); break; case 39: _LL2494: yyval=({ struct
Cyc_StorageClass_tok_struct* _temp2497=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp2497->tag= Cyc_StorageClass_tok_tag;
_temp2497->f1=( void*) Cyc_Parse_Extern_sc;( struct _xenum_struct*) _temp2497;});
break; case 40: _LL2496: if( Cyc_String_strcmp( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp2499= yyvs; struct _xenum_struct** _temp2501= _temp2499.curr +
yyvsp_offset; if( _temp2501 < _temp2499.base? 1: _temp2501 >= _temp2499.last_plus_one){
_throw( Null_Exception);}* _temp2501;})),( struct _tagged_string)({ char*
_temp2502=( char*)"C"; struct _tagged_string _temp2503; _temp2503.curr=
_temp2502; _temp2503.base= _temp2502; _temp2503.last_plus_one= _temp2502 + 2;
_temp2503;})) != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp2504=(
char*)"only extern or extern \"C\" is allowed"; struct _tagged_string _temp2505;
_temp2505.curr= _temp2504; _temp2505.base= _temp2504; _temp2505.last_plus_one=
_temp2504 + 37; _temp2505;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2506= yyls; struct Cyc_Yyltype* _temp2508= _temp2506.curr +( yylsp_offset -
1); if( _temp2508 < _temp2506.base? 1: _temp2508 >= _temp2506.last_plus_one){
_throw( Null_Exception);}* _temp2508;}).first_line,({ struct _tagged_ptr3
_temp2509= yyls; struct Cyc_Yyltype* _temp2511= _temp2509.curr + yylsp_offset;
if( _temp2511 < _temp2509.base? 1: _temp2511 >= _temp2509.last_plus_one){ _throw(
Null_Exception);}* _temp2511;}).last_line));} yyval=({ struct Cyc_StorageClass_tok_struct*
_temp2512=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2512->tag= Cyc_StorageClass_tok_tag; _temp2512->f1=( void*) Cyc_Parse_ExternC_sc;(
struct _xenum_struct*) _temp2512;}); break; case 41: _LL2498: yyval=({ struct
Cyc_StorageClass_tok_struct* _temp2514=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp2514->tag= Cyc_StorageClass_tok_tag;
_temp2514->f1=( void*) Cyc_Parse_Typedef_sc;( struct _xenum_struct*) _temp2514;});
break; case 42: _LL2513: yyval=({ struct Cyc_StorageClass_tok_struct* _temp2516=(
struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2516->tag= Cyc_StorageClass_tok_tag; _temp2516->f1=( void*) Cyc_Parse_Abstract_sc;(
struct _xenum_struct*) _temp2516;}); break; case 43: _LL2515: yyval=({ struct
Cyc_AttributeList_tok_struct* _temp2518=( struct Cyc_AttributeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct)); _temp2518->tag= Cyc_AttributeList_tok_tag;
_temp2518->f1= 0;( struct _xenum_struct*) _temp2518;}); break; case 44: _LL2517:
yyval=({ struct _tagged_ptr2 _temp2520= yyvs; struct _xenum_struct** _temp2522=
_temp2520.curr + yyvsp_offset; if( _temp2522 < _temp2520.base? 1: _temp2522 >=
_temp2520.last_plus_one){ _throw( Null_Exception);}* _temp2522;}); break; case
45: _LL2519: yyval=({ struct Cyc_AttributeList_tok_struct* _temp2524=( struct
Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2524->tag= Cyc_AttributeList_tok_tag; _temp2524->f1= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2525= yyvs; struct _xenum_struct** _temp2527= _temp2525.curr
+( yyvsp_offset - 2); if( _temp2527 < _temp2525.base? 1: _temp2527 >= _temp2525.last_plus_one){
_throw( Null_Exception);}* _temp2527;}));( struct _xenum_struct*) _temp2524;});
break; case 46: _LL2523: yyval=({ struct Cyc_AttributeList_tok_struct* _temp2529=(
struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2529->tag= Cyc_AttributeList_tok_tag; _temp2529->f1=({ struct Cyc_List_List*
_temp2530=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2530->hd=( void*) Cyc_yyget_Attribute_tok(({ struct _tagged_ptr2 _temp2531=
yyvs; struct _xenum_struct** _temp2533= _temp2531.curr + yyvsp_offset; if(
_temp2533 < _temp2531.base? 1: _temp2533 >= _temp2531.last_plus_one){ _throw(
Null_Exception);}* _temp2533;})); _temp2530->tl= 0; _temp2530;});( struct
_xenum_struct*) _temp2529;}); break; case 47: _LL2528: yyval=({ struct Cyc_AttributeList_tok_struct*
_temp2535=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2535->tag= Cyc_AttributeList_tok_tag; _temp2535->f1=({ struct Cyc_List_List*
_temp2536=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2536->hd=( void*) Cyc_yyget_Attribute_tok(({ struct _tagged_ptr2 _temp2540=
yyvs; struct _xenum_struct** _temp2542= _temp2540.curr +( yyvsp_offset - 2); if(
_temp2542 < _temp2540.base? 1: _temp2542 >= _temp2540.last_plus_one){ _throw(
Null_Exception);}* _temp2542;})); _temp2536->tl= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2537= yyvs; struct _xenum_struct** _temp2539= _temp2537.curr
+ yyvsp_offset; if( _temp2539 < _temp2537.base? 1: _temp2539 >= _temp2537.last_plus_one){
_throw( Null_Exception);}* _temp2539;})); _temp2536;});( struct _xenum_struct*)
_temp2535;}); break; case 48: _LL2534: { struct _tagged_string s= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp2544= yyvs; struct _xenum_struct** _temp2546= _temp2544.curr
+ yyvsp_offset; if( _temp2546 < _temp2544.base? 1: _temp2546 >= _temp2544.last_plus_one){
_throw( Null_Exception);}* _temp2546;})); void* a; if( Cyc_String_zstrcmp( s,(
struct _tagged_string)({ char* _temp2547=( char*)"stdcall"; struct
_tagged_string _temp2548; _temp2548.curr= _temp2547; _temp2548.base= _temp2547;
_temp2548.last_plus_one= _temp2547 + 8; _temp2548;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2549=( char*)"__stdcall__"; struct
_tagged_string _temp2550; _temp2550.curr= _temp2549; _temp2550.base= _temp2549;
_temp2550.last_plus_one= _temp2549 + 12; _temp2550;})) == 0){ a= Cyc_Absyn_Stdcall_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2551=(
char*)"cdecl"; struct _tagged_string _temp2552; _temp2552.curr= _temp2551;
_temp2552.base= _temp2551; _temp2552.last_plus_one= _temp2551 + 6; _temp2552;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2553=( char*)"__cdecl__";
struct _tagged_string _temp2554; _temp2554.curr= _temp2553; _temp2554.base=
_temp2553; _temp2554.last_plus_one= _temp2553 + 10; _temp2554;})) == 0){ a= Cyc_Absyn_Cdecl_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2555=(
char*)"noreturn"; struct _tagged_string _temp2556; _temp2556.curr= _temp2555;
_temp2556.base= _temp2555; _temp2556.last_plus_one= _temp2555 + 9; _temp2556;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2557=( char*)"__noreturn__";
struct _tagged_string _temp2558; _temp2558.curr= _temp2557; _temp2558.base=
_temp2557; _temp2558.last_plus_one= _temp2557 + 13; _temp2558;})) == 0){ a= Cyc_Absyn_Noreturn_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2559=(
char*)"noreturn"; struct _tagged_string _temp2560; _temp2560.curr= _temp2559;
_temp2560.base= _temp2559; _temp2560.last_plus_one= _temp2559 + 9; _temp2560;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2561=( char*)"__noreturn__";
struct _tagged_string _temp2562; _temp2562.curr= _temp2561; _temp2562.base=
_temp2561; _temp2562.last_plus_one= _temp2561 + 13; _temp2562;})) == 0){ a= Cyc_Absyn_Noreturn_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2563=(
char*)"__const__"; struct _tagged_string _temp2564; _temp2564.curr= _temp2563;
_temp2564.base= _temp2563; _temp2564.last_plus_one= _temp2563 + 10; _temp2564;}))
== 0){ a= Cyc_Absyn_Const_att;} else{ if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2565=( char*)"aligned"; struct _tagged_string
_temp2566; _temp2566.curr= _temp2565; _temp2566.base= _temp2565; _temp2566.last_plus_one=
_temp2565 + 8; _temp2566;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2567=( char*)"__aligned__"; struct _tagged_string
_temp2568; _temp2568.curr= _temp2567; _temp2568.base= _temp2567; _temp2568.last_plus_one=
_temp2567 + 12; _temp2568;})) == 0){ a=({ struct Cyc_Absyn_Aligned_att_struct*
_temp2569=( struct Cyc_Absyn_Aligned_att_struct*) GC_malloc_atomic( sizeof(
struct Cyc_Absyn_Aligned_att_struct)); _temp2569->tag= Cyc_Absyn_Aligned_att_tag;
_temp2569->f1= - 1;( void*) _temp2569;});} else{ if( Cyc_String_zstrcmp( s,(
struct _tagged_string)({ char* _temp2570=( char*)"packed"; struct _tagged_string
_temp2571; _temp2571.curr= _temp2570; _temp2571.base= _temp2570; _temp2571.last_plus_one=
_temp2570 + 7; _temp2571;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2572=( char*)"__packed__"; struct _tagged_string
_temp2573; _temp2573.curr= _temp2572; _temp2573.base= _temp2572; _temp2573.last_plus_one=
_temp2572 + 11; _temp2573;})) == 0){ a= Cyc_Absyn_Packed_att;} else{ if( Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2574=( char*)"shared"; struct
_tagged_string _temp2575; _temp2575.curr= _temp2574; _temp2575.base= _temp2574;
_temp2575.last_plus_one= _temp2574 + 7; _temp2575;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2576=( char*)"__shared__"; struct
_tagged_string _temp2577; _temp2577.curr= _temp2576; _temp2577.base= _temp2576;
_temp2577.last_plus_one= _temp2576 + 11; _temp2577;})) == 0){ a= Cyc_Absyn_Shared_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2578=(
char*)"unused"; struct _tagged_string _temp2579; _temp2579.curr= _temp2578;
_temp2579.base= _temp2578; _temp2579.last_plus_one= _temp2578 + 7; _temp2579;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2580=( char*)"__unused__";
struct _tagged_string _temp2581; _temp2581.curr= _temp2580; _temp2581.base=
_temp2580; _temp2581.last_plus_one= _temp2580 + 11; _temp2581;})) == 0){ a= Cyc_Absyn_Unused_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2582=(
char*)"weak"; struct _tagged_string _temp2583; _temp2583.curr= _temp2582;
_temp2583.base= _temp2582; _temp2583.last_plus_one= _temp2582 + 5; _temp2583;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2584=( char*)"__weak__";
struct _tagged_string _temp2585; _temp2585.curr= _temp2584; _temp2585.base=
_temp2584; _temp2585.last_plus_one= _temp2584 + 9; _temp2585;})) == 0){ a= Cyc_Absyn_Weak_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2586=(
char*)"dllimport"; struct _tagged_string _temp2587; _temp2587.curr= _temp2586;
_temp2587.base= _temp2586; _temp2587.last_plus_one= _temp2586 + 10; _temp2587;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2588=( char*)"__dllimport__";
struct _tagged_string _temp2589; _temp2589.curr= _temp2588; _temp2589.base=
_temp2588; _temp2589.last_plus_one= _temp2588 + 14; _temp2589;})) == 0){ a= Cyc_Absyn_Dllimport_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2590=(
char*)"dllexport"; struct _tagged_string _temp2591; _temp2591.curr= _temp2590;
_temp2591.base= _temp2590; _temp2591.last_plus_one= _temp2590 + 10; _temp2591;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2592=( char*)"__dllexport__";
struct _tagged_string _temp2593; _temp2593.curr= _temp2592; _temp2593.base=
_temp2592; _temp2593.last_plus_one= _temp2592 + 14; _temp2593;})) == 0){ a= Cyc_Absyn_Dllexport_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2594=(
char*)"no_instrument_function"; struct _tagged_string _temp2595; _temp2595.curr=
_temp2594; _temp2595.base= _temp2594; _temp2595.last_plus_one= _temp2594 + 23;
_temp2595;})) == 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char*
_temp2596=( char*)"__no_instrument_function__"; struct _tagged_string _temp2597;
_temp2597.curr= _temp2596; _temp2597.base= _temp2596; _temp2597.last_plus_one=
_temp2596 + 27; _temp2597;})) == 0){ a= Cyc_Absyn_No_instrument_function_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2598=(
char*)"constructor"; struct _tagged_string _temp2599; _temp2599.curr= _temp2598;
_temp2599.base= _temp2598; _temp2599.last_plus_one= _temp2598 + 12; _temp2599;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2600=( char*)"__constructor__";
struct _tagged_string _temp2601; _temp2601.curr= _temp2600; _temp2601.base=
_temp2600; _temp2601.last_plus_one= _temp2600 + 16; _temp2601;})) == 0){ a= Cyc_Absyn_Constructor_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2602=(
char*)"destructor"; struct _tagged_string _temp2603; _temp2603.curr= _temp2602;
_temp2603.base= _temp2602; _temp2603.last_plus_one= _temp2602 + 11; _temp2603;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2604=( char*)"__destructor__";
struct _tagged_string _temp2605; _temp2605.curr= _temp2604; _temp2605.base=
_temp2604; _temp2605.last_plus_one= _temp2604 + 15; _temp2605;})) == 0){ a= Cyc_Absyn_Destructor_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2606=(
char*)"no_check_memory_usage"; struct _tagged_string _temp2607; _temp2607.curr=
_temp2606; _temp2607.base= _temp2606; _temp2607.last_plus_one= _temp2606 + 22;
_temp2607;})) == 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char*
_temp2608=( char*)"__no_check_memory_usage__"; struct _tagged_string _temp2609;
_temp2609.curr= _temp2608; _temp2609.base= _temp2608; _temp2609.last_plus_one=
_temp2608 + 26; _temp2609;})) == 0){ a= Cyc_Absyn_No_check_memory_usage_att;}
else{ Cyc_Parse_err(( struct _tagged_string)({ char* _temp2610=( char*)"unrecognized attribute";
struct _tagged_string _temp2611; _temp2611.curr= _temp2610; _temp2611.base=
_temp2610; _temp2611.last_plus_one= _temp2610 + 23; _temp2611;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2612= yyls; struct Cyc_Yyltype* _temp2614= _temp2612.curr
+ yylsp_offset; if( _temp2614 < _temp2612.base? 1: _temp2614 >= _temp2612.last_plus_one){
_throw( Null_Exception);}* _temp2614;}).first_line,({ struct _tagged_ptr3
_temp2615= yyls; struct Cyc_Yyltype* _temp2617= _temp2615.curr + yylsp_offset;
if( _temp2617 < _temp2615.base? 1: _temp2617 >= _temp2615.last_plus_one){ _throw(
Null_Exception);}* _temp2617;}).last_line)); a= Cyc_Absyn_Cdecl_att;}}}}}}}}}}}}}}}}
yyval=({ struct Cyc_Attribute_tok_struct* _temp2618=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp2618->tag= Cyc_Attribute_tok_tag;
_temp2618->f1=( void*) a;( struct _xenum_struct*) _temp2618;}); break;} case 49:
_LL2543: yyval=({ struct Cyc_Attribute_tok_struct* _temp2620=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp2620->tag= Cyc_Attribute_tok_tag;
_temp2620->f1=( void*) Cyc_Absyn_Const_att;( struct _xenum_struct*) _temp2620;});
break; case 50: _LL2619: { struct _tagged_string s= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp2622= yyvs; struct _xenum_struct** _temp2624= _temp2622.curr
+( yyvsp_offset - 3); if( _temp2624 < _temp2622.base? 1: _temp2624 >= _temp2622.last_plus_one){
_throw( Null_Exception);}* _temp2624;})); struct _tuple12 _temp2630; int
_temp2631; void* _temp2633; struct _tuple12* _temp2628= Cyc_yyget_Int_tok(({
struct _tagged_ptr2 _temp2625= yyvs; struct _xenum_struct** _temp2627= _temp2625.curr
+( yyvsp_offset - 1); if( _temp2627 < _temp2625.base? 1: _temp2627 >= _temp2625.last_plus_one){
_throw( Null_Exception);}* _temp2627;})); _temp2630=* _temp2628; _LL2634:
_temp2633= _temp2630.f1; goto _LL2632; _LL2632: _temp2631= _temp2630.f2; goto
_LL2629; _LL2629: { void* a; if( Cyc_String_zstrcmp( s,( struct _tagged_string)({
char* _temp2635=( char*)"regparm"; struct _tagged_string _temp2636; _temp2636.curr=
_temp2635; _temp2636.base= _temp2635; _temp2636.last_plus_one= _temp2635 + 8;
_temp2636;})) == 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char*
_temp2637=( char*)"__regparm__"; struct _tagged_string _temp2638; _temp2638.curr=
_temp2637; _temp2638.base= _temp2637; _temp2638.last_plus_one= _temp2637 + 12;
_temp2638;})) == 0){ if( _temp2631 <= 0? 1: _temp2631 > 3){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp2639=( char*)"regparm requires value between 1 and 3";
struct _tagged_string _temp2640; _temp2640.curr= _temp2639; _temp2640.base=
_temp2639; _temp2640.last_plus_one= _temp2639 + 39; _temp2640;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2641= yyls; struct Cyc_Yyltype* _temp2643= _temp2641.curr
+( yylsp_offset - 1); if( _temp2643 < _temp2641.base? 1: _temp2643 >= _temp2641.last_plus_one){
_throw( Null_Exception);}* _temp2643;}).first_line,({ struct _tagged_ptr3
_temp2644= yyls; struct Cyc_Yyltype* _temp2646= _temp2644.curr +( yylsp_offset -
1); if( _temp2646 < _temp2644.base? 1: _temp2646 >= _temp2644.last_plus_one){
_throw( Null_Exception);}* _temp2646;}).last_line));} a=({ struct Cyc_Absyn_Regparm_att_struct*
_temp2647=( struct Cyc_Absyn_Regparm_att_struct*) GC_malloc_atomic( sizeof(
struct Cyc_Absyn_Regparm_att_struct)); _temp2647->tag= Cyc_Absyn_Regparm_att_tag;
_temp2647->f1= _temp2631;( void*) _temp2647;});} else{ if( Cyc_String_zstrcmp( s,(
struct _tagged_string)({ char* _temp2648=( char*)"aligned"; struct
_tagged_string _temp2649; _temp2649.curr= _temp2648; _temp2649.base= _temp2648;
_temp2649.last_plus_one= _temp2648 + 8; _temp2649;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2650=( char*)"__aligned__"; struct
_tagged_string _temp2651; _temp2651.curr= _temp2650; _temp2651.base= _temp2650;
_temp2651.last_plus_one= _temp2650 + 12; _temp2651;})) == 0){ if( _temp2631 < 0){
Cyc_Parse_err(( struct _tagged_string)({ char* _temp2652=( char*)"aligned requires positive power of two";
struct _tagged_string _temp2653; _temp2653.curr= _temp2652; _temp2653.base=
_temp2652; _temp2653.last_plus_one= _temp2652 + 39; _temp2653;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2654= yyls; struct Cyc_Yyltype* _temp2656= _temp2654.curr
+( yylsp_offset - 1); if( _temp2656 < _temp2654.base? 1: _temp2656 >= _temp2654.last_plus_one){
_throw( Null_Exception);}* _temp2656;}).first_line,({ struct _tagged_ptr3
_temp2657= yyls; struct Cyc_Yyltype* _temp2659= _temp2657.curr +( yylsp_offset -
1); if( _temp2659 < _temp2657.base? 1: _temp2659 >= _temp2657.last_plus_one){
_throw( Null_Exception);}* _temp2659;}).last_line));}{ unsigned int j=(
unsigned int) _temp2631; for( 0;( j &( unsigned int) 1) ==( unsigned int) 0; j=
j >>( unsigned int) 1){;} j= j >>( unsigned int) 1; if( j !=( unsigned int) 0){
Cyc_Parse_err(( struct _tagged_string)({ char* _temp2660=( char*)"aligned requires positive power of two";
struct _tagged_string _temp2661; _temp2661.curr= _temp2660; _temp2661.base=
_temp2660; _temp2661.last_plus_one= _temp2660 + 39; _temp2661;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2662= yyls; struct Cyc_Yyltype* _temp2664= _temp2662.curr
+( yylsp_offset - 1); if( _temp2664 < _temp2662.base? 1: _temp2664 >= _temp2662.last_plus_one){
_throw( Null_Exception);}* _temp2664;}).first_line,({ struct _tagged_ptr3
_temp2665= yyls; struct Cyc_Yyltype* _temp2667= _temp2665.curr +( yylsp_offset -
1); if( _temp2667 < _temp2665.base? 1: _temp2667 >= _temp2665.last_plus_one){
_throw( Null_Exception);}* _temp2667;}).last_line));} a=({ struct Cyc_Absyn_Aligned_att_struct*
_temp2668=( struct Cyc_Absyn_Aligned_att_struct*) GC_malloc_atomic( sizeof(
struct Cyc_Absyn_Aligned_att_struct)); _temp2668->tag= Cyc_Absyn_Aligned_att_tag;
_temp2668->f1= _temp2631;( void*) _temp2668;});}} else{ Cyc_Parse_err(( struct
_tagged_string)({ char* _temp2669=( char*)"unrecognized attribute"; struct
_tagged_string _temp2670; _temp2670.curr= _temp2669; _temp2670.base= _temp2669;
_temp2670.last_plus_one= _temp2669 + 23; _temp2670;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2671= yyls; struct Cyc_Yyltype* _temp2673= _temp2671.curr
+( yylsp_offset - 3); if( _temp2673 < _temp2671.base? 1: _temp2673 >= _temp2671.last_plus_one){
_throw( Null_Exception);}* _temp2673;}).first_line,({ struct _tagged_ptr3
_temp2674= yyls; struct Cyc_Yyltype* _temp2676= _temp2674.curr +( yylsp_offset -
3); if( _temp2676 < _temp2674.base? 1: _temp2676 >= _temp2674.last_plus_one){
_throw( Null_Exception);}* _temp2676;}).last_line)); a= Cyc_Absyn_Cdecl_att;}}
yyval=({ struct Cyc_Attribute_tok_struct* _temp2677=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp2677->tag= Cyc_Attribute_tok_tag;
_temp2677->f1=( void*) a;( struct _xenum_struct*) _temp2677;}); break;}} case 51:
_LL2621: { struct _tagged_string s= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp2679= yyvs; struct _xenum_struct** _temp2681= _temp2679.curr +(
yyvsp_offset - 3); if( _temp2681 < _temp2679.base? 1: _temp2681 >= _temp2679.last_plus_one){
_throw( Null_Exception);}* _temp2681;})); struct _tagged_string str= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp2682= yyvs; struct _xenum_struct** _temp2684= _temp2682.curr
+( yyvsp_offset - 1); if( _temp2684 < _temp2682.base? 1: _temp2684 >= _temp2682.last_plus_one){
_throw( Null_Exception);}* _temp2684;})); void* a; if( Cyc_String_zstrcmp( s,(
struct _tagged_string)({ char* _temp2685=( char*)"section"; struct
_tagged_string _temp2686; _temp2686.curr= _temp2685; _temp2686.base= _temp2685;
_temp2686.last_plus_one= _temp2685 + 8; _temp2686;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2687=( char*)"__section__"; struct
_tagged_string _temp2688; _temp2688.curr= _temp2687; _temp2688.base= _temp2687;
_temp2688.last_plus_one= _temp2687 + 12; _temp2688;}))){ a=({ struct Cyc_Absyn_Section_att_struct*
_temp2689=( struct Cyc_Absyn_Section_att_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Section_att_struct));
_temp2689->tag= Cyc_Absyn_Section_att_tag; _temp2689->f1= str;( void*) _temp2689;});}
else{ Cyc_Parse_err(( struct _tagged_string)({ char* _temp2690=( char*)"unrecognized attribute";
struct _tagged_string _temp2691; _temp2691.curr= _temp2690; _temp2691.base=
_temp2690; _temp2691.last_plus_one= _temp2690 + 23; _temp2691;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2692= yyls; struct Cyc_Yyltype* _temp2694= _temp2692.curr
+( yylsp_offset - 3); if( _temp2694 < _temp2692.base? 1: _temp2694 >= _temp2692.last_plus_one){
_throw( Null_Exception);}* _temp2694;}).first_line,({ struct _tagged_ptr3
_temp2695= yyls; struct Cyc_Yyltype* _temp2697= _temp2695.curr +( yylsp_offset -
3); if( _temp2697 < _temp2695.base? 1: _temp2697 >= _temp2695.last_plus_one){
_throw( Null_Exception);}* _temp2697;}).last_line)); a= Cyc_Absyn_Cdecl_att;}
yyval=({ struct Cyc_Attribute_tok_struct* _temp2698=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp2698->tag= Cyc_Attribute_tok_tag;
_temp2698->f1=( void*) a;( struct _xenum_struct*) _temp2698;}); break;} case 52:
_LL2678: yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2700=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2700->tag= Cyc_TypeSpecifier_tok_tag;
_temp2700->f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_VoidType, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2701= yyls; struct Cyc_Yyltype* _temp2703= _temp2701.curr
+ yylsp_offset; if( _temp2703 < _temp2701.base? 1: _temp2703 >= _temp2701.last_plus_one){
_throw( Null_Exception);}* _temp2703;}).first_line,({ struct _tagged_ptr3
_temp2704= yyls; struct Cyc_Yyltype* _temp2706= _temp2704.curr + yylsp_offset;
if( _temp2706 < _temp2704.base? 1: _temp2706 >= _temp2704.last_plus_one){ _throw(
Null_Exception);}* _temp2706;}).last_line));( struct _xenum_struct*) _temp2700;});
break; case 53: _LL2699: yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2708=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2708->tag= Cyc_TypeSpecifier_tok_tag; _temp2708->f1=( void*) Cyc_Parse_type_spec(
Cyc_Absyn_new_evar( Cyc_Absyn_MemKind), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2709= yyls; struct Cyc_Yyltype* _temp2711= _temp2709.curr +
yylsp_offset; if( _temp2711 < _temp2709.base? 1: _temp2711 >= _temp2709.last_plus_one){
_throw( Null_Exception);}* _temp2711;}).first_line,({ struct _tagged_ptr3
_temp2712= yyls; struct Cyc_Yyltype* _temp2714= _temp2712.curr + yylsp_offset;
if( _temp2714 < _temp2712.base? 1: _temp2714 >= _temp2712.last_plus_one){ _throw(
Null_Exception);}* _temp2714;}).last_line));( struct _xenum_struct*) _temp2708;});
break; case 54: _LL2707: yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2716=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2716->tag= Cyc_TypeSpecifier_tok_tag; _temp2716->f1=( void*) Cyc_Parse_type_spec(
Cyc_Absyn_uchar_t, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2717=
yyls; struct Cyc_Yyltype* _temp2719= _temp2717.curr + yylsp_offset; if(
_temp2719 < _temp2717.base? 1: _temp2719 >= _temp2717.last_plus_one){ _throw(
Null_Exception);}* _temp2719;}).first_line,({ struct _tagged_ptr3 _temp2720=
yyls; struct Cyc_Yyltype* _temp2722= _temp2720.curr + yylsp_offset; if(
_temp2722 < _temp2720.base? 1: _temp2722 >= _temp2720.last_plus_one){ _throw(
Null_Exception);}* _temp2722;}).last_line));( struct _xenum_struct*) _temp2716;});
break; case 55: _LL2715: yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2724=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2724->tag= Cyc_TypeSpecifier_tok_tag; _temp2724->f1=( void*)({ struct Cyc_Parse_Short_spec_struct*
_temp2725=( struct Cyc_Parse_Short_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Short_spec_struct));
_temp2725->tag= Cyc_Parse_Short_spec_tag; _temp2725->f1= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2726= yyls; struct Cyc_Yyltype* _temp2728= _temp2726.curr
+ yylsp_offset; if( _temp2728 < _temp2726.base? 1: _temp2728 >= _temp2726.last_plus_one){
_throw( Null_Exception);}* _temp2728;}).first_line,({ struct _tagged_ptr3
_temp2729= yyls; struct Cyc_Yyltype* _temp2731= _temp2729.curr + yylsp_offset;
if( _temp2731 < _temp2729.base? 1: _temp2731 >= _temp2729.last_plus_one){ _throw(
Null_Exception);}* _temp2731;}).last_line);( void*) _temp2725;});( struct
_xenum_struct*) _temp2724;}); break; case 56: _LL2723: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp2733=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2733->tag= Cyc_TypeSpecifier_tok_tag; _temp2733->f1=( void*) Cyc_Parse_type_spec(
Cyc_Absyn_sint_t, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2734=
yyls; struct Cyc_Yyltype* _temp2736= _temp2734.curr + yylsp_offset; if(
_temp2736 < _temp2734.base? 1: _temp2736 >= _temp2734.last_plus_one){ _throw(
Null_Exception);}* _temp2736;}).first_line,({ struct _tagged_ptr3 _temp2737=
yyls; struct Cyc_Yyltype* _temp2739= _temp2737.curr + yylsp_offset; if(
_temp2739 < _temp2737.base? 1: _temp2739 >= _temp2737.last_plus_one){ _throw(
Null_Exception);}* _temp2739;}).last_line));( struct _xenum_struct*) _temp2733;});
break; case 57: _LL2732: yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2741=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2741->tag= Cyc_TypeSpecifier_tok_tag; _temp2741->f1=( void*)({ struct Cyc_Parse_Long_spec_struct*
_temp2742=( struct Cyc_Parse_Long_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Long_spec_struct));
_temp2742->tag= Cyc_Parse_Long_spec_tag; _temp2742->f1= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2743= yyls; struct Cyc_Yyltype* _temp2745= _temp2743.curr
+ yylsp_offset; if( _temp2745 < _temp2743.base? 1: _temp2745 >= _temp2743.last_plus_one){
_throw( Null_Exception);}* _temp2745;}).first_line,({ struct _tagged_ptr3
_temp2746= yyls; struct Cyc_Yyltype* _temp2748= _temp2746.curr + yylsp_offset;
if( _temp2748 < _temp2746.base? 1: _temp2748 >= _temp2746.last_plus_one){ _throw(
Null_Exception);}* _temp2748;}).last_line);( void*) _temp2742;});( struct
_xenum_struct*) _temp2741;}); break; case 58: _LL2740: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp2750=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2750->tag= Cyc_TypeSpecifier_tok_tag; _temp2750->f1=( void*) Cyc_Parse_type_spec(
Cyc_Absyn_float_t, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2751=
yyls; struct Cyc_Yyltype* _temp2753= _temp2751.curr + yylsp_offset; if(
_temp2753 < _temp2751.base? 1: _temp2753 >= _temp2751.last_plus_one){ _throw(
Null_Exception);}* _temp2753;}).first_line,({ struct _tagged_ptr3 _temp2754=
yyls; struct Cyc_Yyltype* _temp2756= _temp2754.curr + yylsp_offset; if(
_temp2756 < _temp2754.base? 1: _temp2756 >= _temp2754.last_plus_one){ _throw(
Null_Exception);}* _temp2756;}).last_line));( struct _xenum_struct*) _temp2750;});
break; case 59: _LL2749: yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2758=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2758->tag= Cyc_TypeSpecifier_tok_tag; _temp2758->f1=( void*) Cyc_Parse_type_spec(
Cyc_Absyn_double_t, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2759=
yyls; struct Cyc_Yyltype* _temp2761= _temp2759.curr + yylsp_offset; if(
_temp2761 < _temp2759.base? 1: _temp2761 >= _temp2759.last_plus_one){ _throw(
Null_Exception);}* _temp2761;}).first_line,({ struct _tagged_ptr3 _temp2762=
yyls; struct Cyc_Yyltype* _temp2764= _temp2762.curr + yylsp_offset; if(
_temp2764 < _temp2762.base? 1: _temp2764 >= _temp2762.last_plus_one){ _throw(
Null_Exception);}* _temp2764;}).last_line));( struct _xenum_struct*) _temp2758;});
break; case 60: _LL2757: yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2766=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2766->tag= Cyc_TypeSpecifier_tok_tag; _temp2766->f1=( void*)({ struct Cyc_Parse_Signed_spec_struct*
_temp2767=( struct Cyc_Parse_Signed_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Signed_spec_struct));
_temp2767->tag= Cyc_Parse_Signed_spec_tag; _temp2767->f1= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2768= yyls; struct Cyc_Yyltype* _temp2770= _temp2768.curr
+ yylsp_offset; if( _temp2770 < _temp2768.base? 1: _temp2770 >= _temp2768.last_plus_one){
_throw( Null_Exception);}* _temp2770;}).first_line,({ struct _tagged_ptr3
_temp2771= yyls; struct Cyc_Yyltype* _temp2773= _temp2771.curr + yylsp_offset;
if( _temp2773 < _temp2771.base? 1: _temp2773 >= _temp2771.last_plus_one){ _throw(
Null_Exception);}* _temp2773;}).last_line);( void*) _temp2767;});( struct
_xenum_struct*) _temp2766;}); break; case 61: _LL2765: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp2775=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2775->tag= Cyc_TypeSpecifier_tok_tag; _temp2775->f1=( void*)({ struct Cyc_Parse_Unsigned_spec_struct*
_temp2776=( struct Cyc_Parse_Unsigned_spec_struct*) GC_malloc( sizeof( struct
Cyc_Parse_Unsigned_spec_struct)); _temp2776->tag= Cyc_Parse_Unsigned_spec_tag;
_temp2776->f1= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2777=
yyls; struct Cyc_Yyltype* _temp2779= _temp2777.curr + yylsp_offset; if(
_temp2779 < _temp2777.base? 1: _temp2779 >= _temp2777.last_plus_one){ _throw(
Null_Exception);}* _temp2779;}).first_line,({ struct _tagged_ptr3 _temp2780=
yyls; struct Cyc_Yyltype* _temp2782= _temp2780.curr + yylsp_offset; if(
_temp2782 < _temp2780.base? 1: _temp2782 >= _temp2780.last_plus_one){ _throw(
Null_Exception);}* _temp2782;}).last_line);( void*) _temp2776;});( struct
_xenum_struct*) _temp2775;}); break; case 62: _LL2774: yyval=({ struct
_tagged_ptr2 _temp2784= yyvs; struct _xenum_struct** _temp2786= _temp2784.curr +
yyvsp_offset; if( _temp2786 < _temp2784.base? 1: _temp2786 >= _temp2784.last_plus_one){
_throw( Null_Exception);}* _temp2786;}); break; case 63: _LL2783: yyval=({
struct _tagged_ptr2 _temp2788= yyvs; struct _xenum_struct** _temp2790= _temp2788.curr
+ yyvsp_offset; if( _temp2790 < _temp2788.base? 1: _temp2790 >= _temp2788.last_plus_one){
_throw( Null_Exception);}* _temp2790;}); break; case 64: _LL2787: yyval=({
struct Cyc_TypeSpecifier_tok_struct* _temp2792=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2792->tag= Cyc_TypeSpecifier_tok_tag;
_temp2792->f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type( Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp2793= yyvs; struct _xenum_struct** _temp2795= _temp2793.curr
+ yyvsp_offset; if( _temp2795 < _temp2793.base? 1: _temp2795 >= _temp2793.last_plus_one){
_throw( Null_Exception);}* _temp2795;})),(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)()),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2796= yyls; struct Cyc_Yyltype*
_temp2798= _temp2796.curr + yylsp_offset; if( _temp2798 < _temp2796.base? 1:
_temp2798 >= _temp2796.last_plus_one){ _throw( Null_Exception);}* _temp2798;}).first_line,({
struct _tagged_ptr3 _temp2799= yyls; struct Cyc_Yyltype* _temp2801= _temp2799.curr
+ yylsp_offset; if( _temp2801 < _temp2799.base? 1: _temp2801 >= _temp2799.last_plus_one){
_throw( Null_Exception);}* _temp2801;}).last_line));( struct _xenum_struct*)
_temp2792;}); break; case 65: _LL2791: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp2803=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2803->tag= Cyc_TypeSpecifier_tok_tag; _temp2803->f1=( void*) Cyc_Parse_type_spec(
Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2804= yyvs;
struct _xenum_struct** _temp2806= _temp2804.curr +( yyvsp_offset - 2); if(
_temp2806 < _temp2804.base? 1: _temp2806 >= _temp2804.last_plus_one){ _throw(
Null_Exception);}* _temp2806;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
Cyc_yyget_Kind_tok(({ struct _tagged_ptr2 _temp2807= yyvs; struct _xenum_struct**
_temp2809= _temp2807.curr + yyvsp_offset; if( _temp2809 < _temp2807.base? 1:
_temp2809 >= _temp2807.last_plus_one){ _throw( Null_Exception);}* _temp2809;})))),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2810= yyls; struct Cyc_Yyltype*
_temp2812= _temp2810.curr +( yylsp_offset - 2); if( _temp2812 < _temp2810.base?
1: _temp2812 >= _temp2810.last_plus_one){ _throw( Null_Exception);}* _temp2812;}).first_line,({
struct _tagged_ptr3 _temp2813= yyls; struct Cyc_Yyltype* _temp2815= _temp2813.curr
+ yylsp_offset; if( _temp2815 < _temp2813.base? 1: _temp2815 >= _temp2813.last_plus_one){
_throw( Null_Exception);}* _temp2815;}).last_line));( struct _xenum_struct*)
_temp2803;}); break; case 66: _LL2802: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp2817=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2817->tag= Cyc_TypeSpecifier_tok_tag; _temp2817->f1=( void*) Cyc_Parse_type_spec(({
struct Cyc_Absyn_TypedefType_struct* _temp2818=( struct Cyc_Absyn_TypedefType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp2818->tag= Cyc_Absyn_TypedefType_tag;
_temp2818->f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp2819= yyvs;
struct _xenum_struct** _temp2821= _temp2819.curr +( yyvsp_offset - 1); if(
_temp2821 < _temp2819.base? 1: _temp2821 >= _temp2819.last_plus_one){ _throw(
Null_Exception);}* _temp2821;})); _temp2818->f2= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp2822= yyvs; struct _xenum_struct** _temp2824= _temp2822.curr
+ yyvsp_offset; if( _temp2824 < _temp2822.base? 1: _temp2824 >= _temp2822.last_plus_one){
_throw( Null_Exception);}* _temp2824;})); _temp2818->f3= 0;( void*) _temp2818;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2825= yyls; struct Cyc_Yyltype*
_temp2827= _temp2825.curr +( yylsp_offset - 1); if( _temp2827 < _temp2825.base?
1: _temp2827 >= _temp2825.last_plus_one){ _throw( Null_Exception);}* _temp2827;}).first_line,({
struct _tagged_ptr3 _temp2828= yyls; struct Cyc_Yyltype* _temp2830= _temp2828.curr
+ yylsp_offset; if( _temp2830 < _temp2828.base? 1: _temp2830 >= _temp2828.last_plus_one){
_throw( Null_Exception);}* _temp2830;}).last_line));( struct _xenum_struct*)
_temp2817;}); break; case 67: _LL2816: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp2832=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2832->tag= Cyc_TypeSpecifier_tok_tag; _temp2832->f1=( void*) Cyc_Parse_type_spec(({
struct Cyc_Absyn_TupleType_struct* _temp2833=( struct Cyc_Absyn_TupleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp2833->tag= Cyc_Absyn_TupleType_tag;
_temp2833->f1=(( struct Cyc_List_List*(*)( struct _tuple10*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2834= yyls; struct Cyc_Yyltype* _temp2836= _temp2834.curr +(
yylsp_offset - 1); if( _temp2836 < _temp2834.base? 1: _temp2836 >= _temp2834.last_plus_one){
_throw( Null_Exception);}* _temp2836;}).first_line,({ struct _tagged_ptr3
_temp2837= yyls; struct Cyc_Yyltype* _temp2839= _temp2837.curr +( yylsp_offset -
1); if( _temp2839 < _temp2837.base? 1: _temp2839 >= _temp2837.last_plus_one){
_throw( Null_Exception);}* _temp2839;}).last_line),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp2840= yyvs; struct _xenum_struct** _temp2842= _temp2840.curr
+( yyvsp_offset - 1); if( _temp2842 < _temp2840.base? 1: _temp2842 >= _temp2840.last_plus_one){
_throw( Null_Exception);}* _temp2842;}))));( void*) _temp2833;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2843= yyls; struct Cyc_Yyltype* _temp2845= _temp2843.curr
+( yylsp_offset - 3); if( _temp2845 < _temp2843.base? 1: _temp2845 >= _temp2843.last_plus_one){
_throw( Null_Exception);}* _temp2845;}).first_line,({ struct _tagged_ptr3
_temp2846= yyls; struct Cyc_Yyltype* _temp2848= _temp2846.curr + yylsp_offset;
if( _temp2848 < _temp2846.base? 1: _temp2848 >= _temp2846.last_plus_one){ _throw(
Null_Exception);}* _temp2848;}).last_line));( struct _xenum_struct*) _temp2832;});
break; case 68: _LL2831: yyval=({ struct Cyc_Kind_tok_struct* _temp2850=( struct
Cyc_Kind_tok_struct*) GC_malloc( sizeof( struct Cyc_Kind_tok_struct)); _temp2850->tag=
Cyc_Kind_tok_tag; _temp2850->f1=( void*) Cyc_Parse_id_to_kind( Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp2851= yyvs; struct _xenum_struct** _temp2853= _temp2851.curr
+ yyvsp_offset; if( _temp2853 < _temp2851.base? 1: _temp2853 >= _temp2851.last_plus_one){
_throw( Null_Exception);}* _temp2853;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2854= yyls; struct Cyc_Yyltype* _temp2856= _temp2854.curr +
yylsp_offset; if( _temp2856 < _temp2854.base? 1: _temp2856 >= _temp2854.last_plus_one){
_throw( Null_Exception);}* _temp2856;}).first_line,({ struct _tagged_ptr3
_temp2857= yyls; struct Cyc_Yyltype* _temp2859= _temp2857.curr + yylsp_offset;
if( _temp2859 < _temp2857.base? 1: _temp2859 >= _temp2857.last_plus_one){ _throw(
Null_Exception);}* _temp2859;}).last_line));( struct _xenum_struct*) _temp2850;});
break; case 69: _LL2849: { struct _tagged_string* _temp2866; void* _temp2868;
struct _tuple1 _temp2864=* Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp2861=
yyvs; struct _xenum_struct** _temp2863= _temp2861.curr + yyvsp_offset; if(
_temp2863 < _temp2861.base? 1: _temp2863 >= _temp2861.last_plus_one){ _throw(
Null_Exception);}* _temp2863;})); _LL2869: _temp2868= _temp2864.f1; goto _LL2867;
_LL2867: _temp2866= _temp2864.f2; goto _LL2865; _LL2865: if( _temp2868 != Cyc_Absyn_Loc_n){
Cyc_Parse_err(( struct _tagged_string)({ char* _temp2870=( char*)"bad kind in type specifier";
struct _tagged_string _temp2871; _temp2871.curr= _temp2870; _temp2871.base=
_temp2870; _temp2871.last_plus_one= _temp2870 + 27; _temp2871;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2872= yyls; struct Cyc_Yyltype* _temp2874= _temp2872.curr
+ yylsp_offset; if( _temp2874 < _temp2872.base? 1: _temp2874 >= _temp2872.last_plus_one){
_throw( Null_Exception);}* _temp2874;}).first_line,({ struct _tagged_ptr3
_temp2875= yyls; struct Cyc_Yyltype* _temp2877= _temp2875.curr + yylsp_offset;
if( _temp2877 < _temp2875.base? 1: _temp2877 >= _temp2875.last_plus_one){ _throw(
Null_Exception);}* _temp2877;}).last_line));} yyval=({ struct Cyc_Kind_tok_struct*
_temp2878=( struct Cyc_Kind_tok_struct*) GC_malloc( sizeof( struct Cyc_Kind_tok_struct));
_temp2878->tag= Cyc_Kind_tok_tag; _temp2878->f1=( void*) Cyc_Parse_id_to_kind(*
_temp2866, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2879= yyls;
struct Cyc_Yyltype* _temp2881= _temp2879.curr + yylsp_offset; if( _temp2881 <
_temp2879.base? 1: _temp2881 >= _temp2879.last_plus_one){ _throw( Null_Exception);}*
_temp2881;}).first_line,({ struct _tagged_ptr3 _temp2882= yyls; struct Cyc_Yyltype*
_temp2884= _temp2882.curr + yylsp_offset; if( _temp2884 < _temp2882.base? 1:
_temp2884 >= _temp2882.last_plus_one){ _throw( Null_Exception);}* _temp2884;}).last_line));(
struct _xenum_struct*) _temp2878;}); break;} case 70: _LL2860: yyval=({ struct
Cyc_TypeQual_tok_struct* _temp2886=( struct Cyc_TypeQual_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeQual_tok_struct)); _temp2886->tag= Cyc_TypeQual_tok_tag;
_temp2886->f1=({ struct Cyc_Absyn_Tqual* _temp2887=( struct Cyc_Absyn_Tqual*)
GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual)); _temp2887->q_const= 1;
_temp2887->q_volatile= 0; _temp2887->q_restrict= 0; _temp2887;});( struct
_xenum_struct*) _temp2886;}); break; case 71: _LL2885: yyval=({ struct Cyc_TypeQual_tok_struct*
_temp2889=( struct Cyc_TypeQual_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct));
_temp2889->tag= Cyc_TypeQual_tok_tag; _temp2889->f1=({ struct Cyc_Absyn_Tqual*
_temp2890=( struct Cyc_Absyn_Tqual*) GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual));
_temp2890->q_const= 0; _temp2890->q_volatile= 1; _temp2890->q_restrict= 0;
_temp2890;});( struct _xenum_struct*) _temp2889;}); break; case 72: _LL2888:
yyval=({ struct Cyc_TypeQual_tok_struct* _temp2892=( struct Cyc_TypeQual_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct)); _temp2892->tag= Cyc_TypeQual_tok_tag;
_temp2892->f1=({ struct Cyc_Absyn_Tqual* _temp2893=( struct Cyc_Absyn_Tqual*)
GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual)); _temp2893->q_const= 0;
_temp2893->q_volatile= 0; _temp2893->q_restrict= 1; _temp2893;});( struct
_xenum_struct*) _temp2892;}); break; case 73: _LL2891: { struct Cyc_Absyn_Decl*
d;{ void* _temp2898= Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2
_temp2895= yyvs; struct _xenum_struct** _temp2897= _temp2895.curr +(
yyvsp_offset - 3); if( _temp2897 < _temp2895.base? 1: _temp2897 >= _temp2895.last_plus_one){
_throw( Null_Exception);}* _temp2897;})); _LL2900: if( _temp2898 == Cyc_Parse_Struct_su){
goto _LL2901;} else{ goto _LL2902;} _LL2902: if( _temp2898 == Cyc_Parse_Union_su){
goto _LL2903;} else{ goto _LL2899;} _LL2901: d= Cyc_Absyn_struct_decl( Cyc_Absyn_Public,
0, 0,({ struct Cyc_Core_Opt* _temp2904=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp2904->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp2905= yyvs; struct _xenum_struct** _temp2907= _temp2905.curr
+( yyvsp_offset - 1); if( _temp2907 < _temp2905.base? 1: _temp2907 >= _temp2905.last_plus_one){
_throw( Null_Exception);}* _temp2907;})); _temp2904;}), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2908= yyls; struct Cyc_Yyltype* _temp2910= _temp2908.curr
+( yylsp_offset - 3); if( _temp2910 < _temp2908.base? 1: _temp2910 >= _temp2908.last_plus_one){
_throw( Null_Exception);}* _temp2910;}).first_line,({ struct _tagged_ptr3
_temp2911= yyls; struct Cyc_Yyltype* _temp2913= _temp2911.curr + yylsp_offset;
if( _temp2913 < _temp2911.base? 1: _temp2913 >= _temp2911.last_plus_one){ _throw(
Null_Exception);}* _temp2913;}).last_line)); goto _LL2899; _LL2903: d= Cyc_Absyn_union_decl(
Cyc_Absyn_Public, 0, 0,({ struct Cyc_Core_Opt* _temp2914=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2914->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp2915= yyvs; struct _xenum_struct** _temp2917= _temp2915.curr
+( yyvsp_offset - 1); if( _temp2917 < _temp2915.base? 1: _temp2917 >= _temp2915.last_plus_one){
_throw( Null_Exception);}* _temp2917;})); _temp2914;}), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2918= yyls; struct Cyc_Yyltype* _temp2920= _temp2918.curr
+( yylsp_offset - 3); if( _temp2920 < _temp2918.base? 1: _temp2920 >= _temp2918.last_plus_one){
_throw( Null_Exception);}* _temp2920;}).first_line,({ struct _tagged_ptr3
_temp2921= yyls; struct Cyc_Yyltype* _temp2923= _temp2921.curr + yylsp_offset;
if( _temp2923 < _temp2921.base? 1: _temp2923 >= _temp2921.last_plus_one){ _throw(
Null_Exception);}* _temp2923;}).last_line)); goto _LL2899; _LL2899:;} yyval=({
struct Cyc_TypeSpecifier_tok_struct* _temp2924=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2924->tag= Cyc_TypeSpecifier_tok_tag;
_temp2924->f1=( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp2925=( struct
Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2925->tag= Cyc_Parse_Decl_spec_tag; _temp2925->f1= d;( void*) _temp2925;});(
struct _xenum_struct*) _temp2924;}); Cyc_Parse_unimp2(( struct _tagged_string)({
char* _temp2926=( char*)"anonymous structs/unions"; struct _tagged_string
_temp2927; _temp2927.curr= _temp2926; _temp2927.base= _temp2926; _temp2927.last_plus_one=
_temp2926 + 25; _temp2927;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2928= yyls; struct Cyc_Yyltype* _temp2930= _temp2928.curr +( yylsp_offset -
3); if( _temp2930 < _temp2928.base? 1: _temp2930 >= _temp2928.last_plus_one){
_throw( Null_Exception);}* _temp2930;}).first_line,({ struct _tagged_ptr3
_temp2931= yyls; struct Cyc_Yyltype* _temp2933= _temp2931.curr + yylsp_offset;
if( _temp2933 < _temp2931.base? 1: _temp2933 >= _temp2931.last_plus_one){ _throw(
Null_Exception);}* _temp2933;}).last_line)); break;} case 74: _LL2894: { struct
Cyc_List_List* ts=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2935= yyls; struct Cyc_Yyltype* _temp2937= _temp2935.curr
+( yylsp_offset - 3); if( _temp2937 < _temp2935.base? 1: _temp2937 >= _temp2935.last_plus_one){
_throw( Null_Exception);}* _temp2937;}).first_line,({ struct _tagged_ptr3
_temp2938= yyls; struct Cyc_Yyltype* _temp2940= _temp2938.curr +( yylsp_offset -
3); if( _temp2940 < _temp2938.base? 1: _temp2940 >= _temp2938.last_plus_one){
_throw( Null_Exception);}* _temp2940;}).last_line), Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp2941= yyvs; struct _xenum_struct** _temp2943= _temp2941.curr
+( yyvsp_offset - 3); if( _temp2943 < _temp2941.base? 1: _temp2943 >= _temp2941.last_plus_one){
_throw( Null_Exception);}* _temp2943;}))); struct Cyc_Absyn_Decl* d;{ void*
_temp2947= Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2 _temp2944= yyvs;
struct _xenum_struct** _temp2946= _temp2944.curr +( yyvsp_offset - 5); if(
_temp2946 < _temp2944.base? 1: _temp2946 >= _temp2944.last_plus_one){ _throw(
Null_Exception);}* _temp2946;})); _LL2949: if( _temp2947 == Cyc_Parse_Struct_su){
goto _LL2950;} else{ goto _LL2951;} _LL2951: if( _temp2947 == Cyc_Parse_Union_su){
goto _LL2952;} else{ goto _LL2948;} _LL2950: d= Cyc_Absyn_struct_decl( Cyc_Absyn_Public,({
struct Cyc_Core_Opt* _temp2953=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2953->v=( void*) Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp2954= yyvs; struct _xenum_struct** _temp2956= _temp2954.curr +(
yyvsp_offset - 4); if( _temp2956 < _temp2954.base? 1: _temp2956 >= _temp2954.last_plus_one){
_throw( Null_Exception);}* _temp2956;})); _temp2953;}), ts,({ struct Cyc_Core_Opt*
_temp2957=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2957->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({ struct _tagged_ptr2
_temp2958= yyvs; struct _xenum_struct** _temp2960= _temp2958.curr +(
yyvsp_offset - 1); if( _temp2960 < _temp2958.base? 1: _temp2960 >= _temp2958.last_plus_one){
_throw( Null_Exception);}* _temp2960;})); _temp2957;}), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2961= yyls; struct Cyc_Yyltype* _temp2963= _temp2961.curr
+( yylsp_offset - 5); if( _temp2963 < _temp2961.base? 1: _temp2963 >= _temp2961.last_plus_one){
_throw( Null_Exception);}* _temp2963;}).first_line,({ struct _tagged_ptr3
_temp2964= yyls; struct Cyc_Yyltype* _temp2966= _temp2964.curr + yylsp_offset;
if( _temp2966 < _temp2964.base? 1: _temp2966 >= _temp2964.last_plus_one){ _throw(
Null_Exception);}* _temp2966;}).last_line)); goto _LL2948; _LL2952: d= Cyc_Absyn_union_decl(
Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp2967=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2967->v=( void*) Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp2968= yyvs; struct _xenum_struct** _temp2970= _temp2968.curr
+( yyvsp_offset - 4); if( _temp2970 < _temp2968.base? 1: _temp2970 >= _temp2968.last_plus_one){
_throw( Null_Exception);}* _temp2970;})); _temp2967;}), ts,({ struct Cyc_Core_Opt*
_temp2971=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2971->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({ struct _tagged_ptr2
_temp2972= yyvs; struct _xenum_struct** _temp2974= _temp2972.curr +(
yyvsp_offset - 1); if( _temp2974 < _temp2972.base? 1: _temp2974 >= _temp2972.last_plus_one){
_throw( Null_Exception);}* _temp2974;})); _temp2971;}), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2975= yyls; struct Cyc_Yyltype* _temp2977= _temp2975.curr
+( yylsp_offset - 5); if( _temp2977 < _temp2975.base? 1: _temp2977 >= _temp2975.last_plus_one){
_throw( Null_Exception);}* _temp2977;}).first_line,({ struct _tagged_ptr3
_temp2978= yyls; struct Cyc_Yyltype* _temp2980= _temp2978.curr + yylsp_offset;
if( _temp2980 < _temp2978.base? 1: _temp2980 >= _temp2978.last_plus_one){ _throw(
Null_Exception);}* _temp2980;}).last_line)); goto _LL2948; _LL2948:;} yyval=({
struct Cyc_TypeSpecifier_tok_struct* _temp2981=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2981->tag= Cyc_TypeSpecifier_tok_tag;
_temp2981->f1=( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp2982=( struct
Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2982->tag= Cyc_Parse_Decl_spec_tag; _temp2982->f1= d;( void*) _temp2982;});(
struct _xenum_struct*) _temp2981;}); break;} case 75: _LL2934: { struct Cyc_List_List*
ts=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2984=
yyls; struct Cyc_Yyltype* _temp2986= _temp2984.curr +( yylsp_offset - 3); if(
_temp2986 < _temp2984.base? 1: _temp2986 >= _temp2984.last_plus_one){ _throw(
Null_Exception);}* _temp2986;}).first_line,({ struct _tagged_ptr3 _temp2987=
yyls; struct Cyc_Yyltype* _temp2989= _temp2987.curr +( yylsp_offset - 3); if(
_temp2989 < _temp2987.base? 1: _temp2989 >= _temp2987.last_plus_one){ _throw(
Null_Exception);}* _temp2989;}).last_line), Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp2990= yyvs; struct _xenum_struct** _temp2992= _temp2990.curr +(
yyvsp_offset - 3); if( _temp2992 < _temp2990.base? 1: _temp2992 >= _temp2990.last_plus_one){
_throw( Null_Exception);}* _temp2992;}))); struct Cyc_Absyn_Decl* d;{ void*
_temp2996= Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2 _temp2993= yyvs;
struct _xenum_struct** _temp2995= _temp2993.curr +( yyvsp_offset - 5); if(
_temp2995 < _temp2993.base? 1: _temp2995 >= _temp2993.last_plus_one){ _throw(
Null_Exception);}* _temp2995;})); _LL2998: if( _temp2996 == Cyc_Parse_Struct_su){
goto _LL2999;} else{ goto _LL3000;} _LL3000: if( _temp2996 == Cyc_Parse_Union_su){
goto _LL3001;} else{ goto _LL2997;} _LL2999: d= Cyc_Absyn_struct_decl( Cyc_Absyn_Public,({
struct Cyc_Core_Opt* _temp3002=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3002->v=( void*) Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp3003= yyvs; struct _xenum_struct** _temp3005= _temp3003.curr +(
yyvsp_offset - 4); if( _temp3005 < _temp3003.base? 1: _temp3005 >= _temp3003.last_plus_one){
_throw( Null_Exception);}* _temp3005;})); _temp3002;}), ts,({ struct Cyc_Core_Opt*
_temp3006=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3006->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({ struct _tagged_ptr2
_temp3007= yyvs; struct _xenum_struct** _temp3009= _temp3007.curr +(
yyvsp_offset - 1); if( _temp3009 < _temp3007.base? 1: _temp3009 >= _temp3007.last_plus_one){
_throw( Null_Exception);}* _temp3009;})); _temp3006;}), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3010= yyls; struct Cyc_Yyltype* _temp3012= _temp3010.curr
+( yylsp_offset - 5); if( _temp3012 < _temp3010.base? 1: _temp3012 >= _temp3010.last_plus_one){
_throw( Null_Exception);}* _temp3012;}).first_line,({ struct _tagged_ptr3
_temp3013= yyls; struct Cyc_Yyltype* _temp3015= _temp3013.curr + yylsp_offset;
if( _temp3015 < _temp3013.base? 1: _temp3015 >= _temp3013.last_plus_one){ _throw(
Null_Exception);}* _temp3015;}).last_line)); goto _LL2997; _LL3001: d= Cyc_Absyn_union_decl(
Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp3016=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3016->v=( void*) Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3017= yyvs; struct _xenum_struct** _temp3019= _temp3017.curr
+( yyvsp_offset - 4); if( _temp3019 < _temp3017.base? 1: _temp3019 >= _temp3017.last_plus_one){
_throw( Null_Exception);}* _temp3019;})); _temp3016;}), ts,({ struct Cyc_Core_Opt*
_temp3020=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3020->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({ struct _tagged_ptr2
_temp3021= yyvs; struct _xenum_struct** _temp3023= _temp3021.curr +(
yyvsp_offset - 1); if( _temp3023 < _temp3021.base? 1: _temp3023 >= _temp3021.last_plus_one){
_throw( Null_Exception);}* _temp3023;})); _temp3020;}), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3024= yyls; struct Cyc_Yyltype* _temp3026= _temp3024.curr
+( yylsp_offset - 5); if( _temp3026 < _temp3024.base? 1: _temp3026 >= _temp3024.last_plus_one){
_throw( Null_Exception);}* _temp3026;}).first_line,({ struct _tagged_ptr3
_temp3027= yyls; struct Cyc_Yyltype* _temp3029= _temp3027.curr + yylsp_offset;
if( _temp3029 < _temp3027.base? 1: _temp3029 >= _temp3027.last_plus_one){ _throw(
Null_Exception);}* _temp3029;}).last_line)); goto _LL2997; _LL2997:;} yyval=({
struct Cyc_TypeSpecifier_tok_struct* _temp3030=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3030->tag= Cyc_TypeSpecifier_tok_tag;
_temp3030->f1=( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp3031=( struct
Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3031->tag= Cyc_Parse_Decl_spec_tag; _temp3031->f1= d;( void*) _temp3031;});(
struct _xenum_struct*) _temp3030;}); break;} case 76: _LL2983:{ void* _temp3036=
Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2 _temp3033= yyvs; struct
_xenum_struct** _temp3035= _temp3033.curr +( yyvsp_offset - 2); if( _temp3035 <
_temp3033.base? 1: _temp3035 >= _temp3033.last_plus_one){ _throw( Null_Exception);}*
_temp3035;})); _LL3038: if( _temp3036 == Cyc_Parse_Struct_su){ goto _LL3039;}
else{ goto _LL3040;} _LL3040: if( _temp3036 == Cyc_Parse_Union_su){ goto _LL3041;}
else{ goto _LL3037;} _LL3039: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp3042=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3042->tag= Cyc_TypeSpecifier_tok_tag; _temp3042->f1=( void*) Cyc_Parse_type_spec(({
struct Cyc_Absyn_StructType_struct* _temp3043=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp3043->tag= Cyc_Absyn_StructType_tag;
_temp3043->f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3044= yyvs; struct _xenum_struct** _temp3046= _temp3044.curr +(
yyvsp_offset - 1); if( _temp3046 < _temp3044.base? 1: _temp3046 >= _temp3044.last_plus_one){
_throw( Null_Exception);}* _temp3046;})); _temp3043->f2= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3047= yyvs; struct _xenum_struct** _temp3049= _temp3047.curr
+ yyvsp_offset; if( _temp3049 < _temp3047.base? 1: _temp3049 >= _temp3047.last_plus_one){
_throw( Null_Exception);}* _temp3049;})); _temp3043->f3= 0;( void*) _temp3043;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3050= yyls; struct Cyc_Yyltype*
_temp3052= _temp3050.curr +( yylsp_offset - 2); if( _temp3052 < _temp3050.base?
1: _temp3052 >= _temp3050.last_plus_one){ _throw( Null_Exception);}* _temp3052;}).first_line,({
struct _tagged_ptr3 _temp3053= yyls; struct Cyc_Yyltype* _temp3055= _temp3053.curr
+ yylsp_offset; if( _temp3055 < _temp3053.base? 1: _temp3055 >= _temp3053.last_plus_one){
_throw( Null_Exception);}* _temp3055;}).last_line));( struct _xenum_struct*)
_temp3042;}); goto _LL3037; _LL3041: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp3056=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3056->tag= Cyc_TypeSpecifier_tok_tag; _temp3056->f1=( void*) Cyc_Parse_type_spec(({
struct Cyc_Absyn_UnionType_struct* _temp3057=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp3057->tag= Cyc_Absyn_UnionType_tag;
_temp3057->f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3058= yyvs; struct _xenum_struct** _temp3060= _temp3058.curr +(
yyvsp_offset - 1); if( _temp3060 < _temp3058.base? 1: _temp3060 >= _temp3058.last_plus_one){
_throw( Null_Exception);}* _temp3060;})); _temp3057->f2= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3061= yyvs; struct _xenum_struct** _temp3063= _temp3061.curr
+ yyvsp_offset; if( _temp3063 < _temp3061.base? 1: _temp3063 >= _temp3061.last_plus_one){
_throw( Null_Exception);}* _temp3063;})); _temp3057->f3= 0;( void*) _temp3057;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3064= yyls; struct Cyc_Yyltype*
_temp3066= _temp3064.curr +( yylsp_offset - 2); if( _temp3066 < _temp3064.base?
1: _temp3066 >= _temp3064.last_plus_one){ _throw( Null_Exception);}* _temp3066;}).first_line,({
struct _tagged_ptr3 _temp3067= yyls; struct Cyc_Yyltype* _temp3069= _temp3067.curr
+ yylsp_offset; if( _temp3069 < _temp3067.base? 1: _temp3069 >= _temp3067.last_plus_one){
_throw( Null_Exception);}* _temp3069;}).last_line));( struct _xenum_struct*)
_temp3056;}); goto _LL3037; _LL3037:;} break; case 77: _LL3032:{ void* _temp3074=
Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2 _temp3071= yyvs; struct
_xenum_struct** _temp3073= _temp3071.curr +( yyvsp_offset - 2); if( _temp3073 <
_temp3071.base? 1: _temp3073 >= _temp3071.last_plus_one){ _throw( Null_Exception);}*
_temp3073;})); _LL3076: if( _temp3074 == Cyc_Parse_Struct_su){ goto _LL3077;}
else{ goto _LL3078;} _LL3078: if( _temp3074 == Cyc_Parse_Union_su){ goto _LL3079;}
else{ goto _LL3075;} _LL3077: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp3080=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3080->tag= Cyc_TypeSpecifier_tok_tag; _temp3080->f1=( void*) Cyc_Parse_type_spec(({
struct Cyc_Absyn_StructType_struct* _temp3081=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp3081->tag= Cyc_Absyn_StructType_tag;
_temp3081->f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3082= yyvs; struct _xenum_struct** _temp3084= _temp3082.curr +(
yyvsp_offset - 1); if( _temp3084 < _temp3082.base? 1: _temp3084 >= _temp3082.last_plus_one){
_throw( Null_Exception);}* _temp3084;})); _temp3081->f2= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3085= yyvs; struct _xenum_struct** _temp3087= _temp3085.curr
+ yyvsp_offset; if( _temp3087 < _temp3085.base? 1: _temp3087 >= _temp3085.last_plus_one){
_throw( Null_Exception);}* _temp3087;})); _temp3081->f3= 0;( void*) _temp3081;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3088= yyls; struct Cyc_Yyltype*
_temp3090= _temp3088.curr +( yylsp_offset - 2); if( _temp3090 < _temp3088.base?
1: _temp3090 >= _temp3088.last_plus_one){ _throw( Null_Exception);}* _temp3090;}).first_line,({
struct _tagged_ptr3 _temp3091= yyls; struct Cyc_Yyltype* _temp3093= _temp3091.curr
+ yylsp_offset; if( _temp3093 < _temp3091.base? 1: _temp3093 >= _temp3091.last_plus_one){
_throw( Null_Exception);}* _temp3093;}).last_line));( struct _xenum_struct*)
_temp3080;}); goto _LL3075; _LL3079: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp3094=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3094->tag= Cyc_TypeSpecifier_tok_tag; _temp3094->f1=( void*) Cyc_Parse_type_spec(({
struct Cyc_Absyn_UnionType_struct* _temp3095=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp3095->tag= Cyc_Absyn_UnionType_tag;
_temp3095->f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3096= yyvs; struct _xenum_struct** _temp3098= _temp3096.curr +(
yyvsp_offset - 1); if( _temp3098 < _temp3096.base? 1: _temp3098 >= _temp3096.last_plus_one){
_throw( Null_Exception);}* _temp3098;})); _temp3095->f2= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3099= yyvs; struct _xenum_struct** _temp3101= _temp3099.curr
+ yyvsp_offset; if( _temp3101 < _temp3099.base? 1: _temp3101 >= _temp3099.last_plus_one){
_throw( Null_Exception);}* _temp3101;})); _temp3095->f3= 0;( void*) _temp3095;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3102= yyls; struct Cyc_Yyltype*
_temp3104= _temp3102.curr +( yylsp_offset - 2); if( _temp3104 < _temp3102.base?
1: _temp3104 >= _temp3102.last_plus_one){ _throw( Null_Exception);}* _temp3104;}).first_line,({
struct _tagged_ptr3 _temp3105= yyls; struct Cyc_Yyltype* _temp3107= _temp3105.curr
+ yylsp_offset; if( _temp3107 < _temp3105.base? 1: _temp3107 >= _temp3105.last_plus_one){
_throw( Null_Exception);}* _temp3107;}).last_line));( struct _xenum_struct*)
_temp3094;}); goto _LL3075; _LL3075:;} break; case 78: _LL3070: yyval=({ struct
Cyc_TypeList_tok_struct* _temp3109=( struct Cyc_TypeList_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeList_tok_struct)); _temp3109->tag= Cyc_TypeList_tok_tag;
_temp3109->f1= 0;( struct _xenum_struct*) _temp3109;}); break; case 79: _LL3108:
yyval=({ struct Cyc_TypeList_tok_struct* _temp3111=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3111->tag= Cyc_TypeList_tok_tag;
_temp3111->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3112= yyvs; struct
_xenum_struct** _temp3114= _temp3112.curr +( yyvsp_offset - 1); if( _temp3114 <
_temp3112.base? 1: _temp3114 >= _temp3112.last_plus_one){ _throw( Null_Exception);}*
_temp3114;})));( struct _xenum_struct*) _temp3111;}); break; case 80: _LL3110:((
struct Cyc_Lexing_lexbuf*) Cyc_Parse_lbuf->v)->lex_curr_pos -= 1; yyval=({
struct Cyc_TypeList_tok_struct* _temp3116=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3116->tag= Cyc_TypeList_tok_tag;
_temp3116->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3117= yyvs; struct
_xenum_struct** _temp3119= _temp3117.curr +( yyvsp_offset - 1); if( _temp3119 <
_temp3117.base? 1: _temp3119 >= _temp3117.last_plus_one){ _throw( Null_Exception);}*
_temp3119;})));( struct _xenum_struct*) _temp3116;}); break; case 81: _LL3115:
yyval=({ struct Cyc_StructOrUnion_tok_struct* _temp3121=( struct Cyc_StructOrUnion_tok_struct*)
GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct)); _temp3121->tag= Cyc_StructOrUnion_tok_tag;
_temp3121->f1=( void*) Cyc_Parse_Struct_su;( struct _xenum_struct*) _temp3121;});
break; case 82: _LL3120: yyval=({ struct Cyc_StructOrUnion_tok_struct* _temp3123=(
struct Cyc_StructOrUnion_tok_struct*) GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));
_temp3123->tag= Cyc_StructOrUnion_tok_tag; _temp3123->f1=( void*) Cyc_Parse_Union_su;(
struct _xenum_struct*) _temp3123;}); break; case 83: _LL3122: yyval=({ struct
Cyc_StructFieldDeclList_tok_struct* _temp3125=( struct Cyc_StructFieldDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_StructFieldDeclList_tok_struct)); _temp3125->tag=
Cyc_StructFieldDeclList_tok_tag; _temp3125->f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_flatten)((( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_StructFieldDeclListList_tok(({
struct _tagged_ptr2 _temp3126= yyvs; struct _xenum_struct** _temp3128= _temp3126.curr
+ yyvsp_offset; if( _temp3128 < _temp3126.base? 1: _temp3128 >= _temp3126.last_plus_one){
_throw( Null_Exception);}* _temp3128;}))));( struct _xenum_struct*) _temp3125;});
break; case 84: _LL3124: yyval=({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp3130=( struct Cyc_StructFieldDeclListList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct)); _temp3130->tag= Cyc_StructFieldDeclListList_tok_tag;
_temp3130->f1=({ struct Cyc_List_List* _temp3131=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3131->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3132= yyvs; struct _xenum_struct** _temp3134= _temp3132.curr
+ yyvsp_offset; if( _temp3134 < _temp3132.base? 1: _temp3134 >= _temp3132.last_plus_one){
_throw( Null_Exception);}* _temp3134;})); _temp3131->tl= 0; _temp3131;});(
struct _xenum_struct*) _temp3130;}); break; case 85: _LL3129: yyval=({ struct
Cyc_StructFieldDeclListList_tok_struct* _temp3136=( struct Cyc_StructFieldDeclListList_tok_struct*)
GC_malloc( sizeof( struct Cyc_StructFieldDeclListList_tok_struct)); _temp3136->tag=
Cyc_StructFieldDeclListList_tok_tag; _temp3136->f1=({ struct Cyc_List_List*
_temp3137=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3137->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(({ struct _tagged_ptr2
_temp3141= yyvs; struct _xenum_struct** _temp3143= _temp3141.curr + yyvsp_offset;
if( _temp3143 < _temp3141.base? 1: _temp3143 >= _temp3141.last_plus_one){ _throw(
Null_Exception);}* _temp3143;})); _temp3137->tl= Cyc_yyget_StructFieldDeclListList_tok(({
struct _tagged_ptr2 _temp3138= yyvs; struct _xenum_struct** _temp3140= _temp3138.curr
+( yyvsp_offset - 1); if( _temp3140 < _temp3138.base? 1: _temp3140 >= _temp3138.last_plus_one){
_throw( Null_Exception);}* _temp3140;})); _temp3137;});( struct _xenum_struct*)
_temp3136;}); break; case 86: _LL3135: yyval=({ struct Cyc_InitDeclList_tok_struct*
_temp3145=( struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp3145->tag= Cyc_InitDeclList_tok_tag; _temp3145->f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitDeclList_tok(({
struct _tagged_ptr2 _temp3146= yyvs; struct _xenum_struct** _temp3148= _temp3146.curr
+ yyvsp_offset; if( _temp3148 < _temp3146.base? 1: _temp3148 >= _temp3146.last_plus_one){
_throw( Null_Exception);}* _temp3148;})));( struct _xenum_struct*) _temp3145;});
break; case 87: _LL3144: yyval=({ struct Cyc_InitDeclList_tok_struct* _temp3150=(
struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp3150->tag= Cyc_InitDeclList_tok_tag; _temp3150->f1=({ struct Cyc_List_List*
_temp3151=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3151->hd=( void*) Cyc_yyget_InitDecl_tok(({ struct _tagged_ptr2 _temp3152=
yyvs; struct _xenum_struct** _temp3154= _temp3152.curr + yyvsp_offset; if(
_temp3154 < _temp3152.base? 1: _temp3154 >= _temp3152.last_plus_one){ _throw(
Null_Exception);}* _temp3154;})); _temp3151->tl= 0; _temp3151;});( struct
_xenum_struct*) _temp3150;}); break; case 88: _LL3149: yyval=({ struct Cyc_InitDeclList_tok_struct*
_temp3156=( struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp3156->tag= Cyc_InitDeclList_tok_tag; _temp3156->f1=({ struct Cyc_List_List*
_temp3157=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3157->hd=( void*) Cyc_yyget_InitDecl_tok(({ struct _tagged_ptr2 _temp3161=
yyvs; struct _xenum_struct** _temp3163= _temp3161.curr + yyvsp_offset; if(
_temp3163 < _temp3161.base? 1: _temp3163 >= _temp3161.last_plus_one){ _throw(
Null_Exception);}* _temp3163;})); _temp3157->tl= Cyc_yyget_InitDeclList_tok(({
struct _tagged_ptr2 _temp3158= yyvs; struct _xenum_struct** _temp3160= _temp3158.curr
+( yyvsp_offset - 2); if( _temp3160 < _temp3158.base? 1: _temp3160 >= _temp3158.last_plus_one){
_throw( Null_Exception);}* _temp3160;})); _temp3157;});( struct _xenum_struct*)
_temp3156;}); break; case 89: _LL3155: yyval=({ struct Cyc_InitDecl_tok_struct*
_temp3165=( struct Cyc_InitDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp3165->tag= Cyc_InitDecl_tok_tag; _temp3165->f1=({ struct _tuple13*
_temp3166=( struct _tuple13*) GC_malloc( sizeof( struct _tuple13)); _temp3166->f1=
Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3167= yyvs; struct
_xenum_struct** _temp3169= _temp3167.curr + yyvsp_offset; if( _temp3169 <
_temp3167.base? 1: _temp3169 >= _temp3167.last_plus_one){ _throw( Null_Exception);}*
_temp3169;})); _temp3166->f2= 0; _temp3166;});( struct _xenum_struct*) _temp3165;});
break; case 90: _LL3164: yyval=({ struct Cyc_InitDecl_tok_struct* _temp3171=(
struct Cyc_InitDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp3171->tag= Cyc_InitDecl_tok_tag; _temp3171->f1=({ struct _tuple13*
_temp3172=( struct _tuple13*) GC_malloc( sizeof( struct _tuple13)); _temp3172->f1=
Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3176= yyvs; struct
_xenum_struct** _temp3178= _temp3176.curr +( yyvsp_offset - 2); if( _temp3178 <
_temp3176.base? 1: _temp3178 >= _temp3176.last_plus_one){ _throw( Null_Exception);}*
_temp3178;})); _temp3172->f2=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp3173= yyvs; struct _xenum_struct** _temp3175= _temp3173.curr
+ yyvsp_offset; if( _temp3175 < _temp3173.base? 1: _temp3175 >= _temp3173.last_plus_one){
_throw( Null_Exception);}* _temp3175;})); _temp3172;});( struct _xenum_struct*)
_temp3171;}); break; case 91: _LL3170: { struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3180= yyvs; struct _xenum_struct** _temp3182= _temp3180.curr
+( yyvsp_offset - 2); if( _temp3182 < _temp3180.base? 1: _temp3182 >= _temp3180.last_plus_one){
_throw( Null_Exception);}* _temp3182;}))).f1; struct Cyc_List_List* atts=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3183= yyvs; struct _xenum_struct** _temp3185= _temp3183.curr
+( yyvsp_offset - 2); if( _temp3185 < _temp3183.base? 1: _temp3185 >= _temp3183.last_plus_one){
_throw( Null_Exception);}* _temp3185;}))).f3; void* t= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp3186= yyvs; struct
_xenum_struct** _temp3188= _temp3186.curr +( yyvsp_offset - 2); if( _temp3188 <
_temp3186.base? 1: _temp3188 >= _temp3186.last_plus_one){ _throw( Null_Exception);}*
_temp3188;}))).f2, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3189=
yyls; struct Cyc_Yyltype* _temp3191= _temp3189.curr +( yylsp_offset - 2); if(
_temp3191 < _temp3189.base? 1: _temp3191 >= _temp3189.last_plus_one){ _throw(
Null_Exception);}* _temp3191;}).first_line,({ struct _tagged_ptr3 _temp3192=
yyls; struct Cyc_Yyltype* _temp3194= _temp3192.curr +( yylsp_offset - 2); if(
_temp3194 < _temp3192.base? 1: _temp3194 >= _temp3192.last_plus_one){ _throw(
Null_Exception);}* _temp3194;}).last_line)); struct Cyc_List_List* _temp3200;
struct Cyc_List_List* _temp3202; struct _tuple0 _temp3198=(( struct _tuple0(*)(
struct Cyc_List_List* x)) Cyc_List_split)( Cyc_yyget_DeclaratorExpoptList_tok(({
struct _tagged_ptr2 _temp3195= yyvs; struct _xenum_struct** _temp3197= _temp3195.curr
+( yyvsp_offset - 1); if( _temp3197 < _temp3195.base? 1: _temp3197 >= _temp3195.last_plus_one){
_throw( Null_Exception);}* _temp3197;}))); _LL3203: _temp3202= _temp3198.f1;
goto _LL3201; _LL3201: _temp3200= _temp3198.f2; goto _LL3199; _LL3199: { struct
Cyc_List_List* info=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct
Cyc_List_List* y)) Cyc_List_zip)( Cyc_Parse_apply_tmss( tq, t, _temp3202, atts),
_temp3200); yyval=({ struct Cyc_StructFieldDeclList_tok_struct* _temp3204=(
struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_StructFieldDeclList_tok_struct));
_temp3204->tag= Cyc_StructFieldDeclList_tok_tag; _temp3204->f1=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Structfield*(* f)( struct Cyc_Position_Segment*, struct _tuple8*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_make_struct_field,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3205= yyls; struct Cyc_Yyltype*
_temp3207= _temp3205.curr +( yylsp_offset - 2); if( _temp3207 < _temp3205.base?
1: _temp3207 >= _temp3205.last_plus_one){ _throw( Null_Exception);}* _temp3207;}).first_line,({
struct _tagged_ptr3 _temp3208= yyls; struct Cyc_Yyltype* _temp3210= _temp3208.curr
+( yylsp_offset - 1); if( _temp3210 < _temp3208.base? 1: _temp3210 >= _temp3208.last_plus_one){
_throw( Null_Exception);}* _temp3210;}).last_line), info);( struct _xenum_struct*)
_temp3204;}); break;}} case 92: _LL3179: yyval=({ struct Cyc_QualSpecList_tok_struct*
_temp3212=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp3212->tag= Cyc_QualSpecList_tok_tag; _temp3212->f1=({ struct _tuple14*
_temp3213=( struct _tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp3213->f1=
Cyc_Absyn_empty_tqual(); _temp3213->f2=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp3217=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3217->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({ struct _tagged_ptr2
_temp3218= yyvs; struct _xenum_struct** _temp3220= _temp3218.curr +(
yyvsp_offset - 1); if( _temp3220 < _temp3218.base? 1: _temp3220 >= _temp3218.last_plus_one){
_throw( Null_Exception);}* _temp3220;})); _temp3217->tl= 0; _temp3217;});
_temp3213->f3= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp3214=
yyvs; struct _xenum_struct** _temp3216= _temp3214.curr + yyvsp_offset; if(
_temp3216 < _temp3214.base? 1: _temp3216 >= _temp3214.last_plus_one){ _throw(
Null_Exception);}* _temp3216;})); _temp3213;});( struct _xenum_struct*)
_temp3212;}); break; case 93: _LL3211: yyval=({ struct Cyc_QualSpecList_tok_struct*
_temp3222=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp3222->tag= Cyc_QualSpecList_tok_tag; _temp3222->f1=({ struct _tuple14*
_temp3223=( struct _tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp3223->f1=(*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp3237= yyvs; struct
_xenum_struct** _temp3239= _temp3237.curr + yyvsp_offset; if( _temp3239 <
_temp3237.base? 1: _temp3239 >= _temp3237.last_plus_one){ _throw( Null_Exception);}*
_temp3239;}))).f1; _temp3223->f2=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp3230=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3230->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({ struct _tagged_ptr2
_temp3234= yyvs; struct _xenum_struct** _temp3236= _temp3234.curr +(
yyvsp_offset - 2); if( _temp3236 < _temp3234.base? 1: _temp3236 >= _temp3234.last_plus_one){
_throw( Null_Exception);}* _temp3236;})); _temp3230->tl=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3231= yyvs; struct _xenum_struct** _temp3233= _temp3231.curr
+ yyvsp_offset; if( _temp3233 < _temp3231.base? 1: _temp3233 >= _temp3231.last_plus_one){
_throw( Null_Exception);}* _temp3233;}))).f2; _temp3230;}); _temp3223->f3=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp3224= yyvs; struct
_xenum_struct** _temp3226= _temp3224.curr +( yyvsp_offset - 1); if( _temp3226 <
_temp3224.base? 1: _temp3226 >= _temp3224.last_plus_one){ _throw( Null_Exception);}*
_temp3226;})),(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp3227=
yyvs; struct _xenum_struct** _temp3229= _temp3227.curr + yyvsp_offset; if(
_temp3229 < _temp3227.base? 1: _temp3229 >= _temp3227.last_plus_one){ _throw(
Null_Exception);}* _temp3229;}))).f3); _temp3223;});( struct _xenum_struct*)
_temp3222;}); break; case 94: _LL3221: yyval=({ struct Cyc_QualSpecList_tok_struct*
_temp3241=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp3241->tag= Cyc_QualSpecList_tok_tag; _temp3241->f1=({ struct _tuple14*
_temp3242=( struct _tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp3242->f1=
Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2 _temp3246= yyvs; struct
_xenum_struct** _temp3248= _temp3246.curr +( yyvsp_offset - 1); if( _temp3248 <
_temp3246.base? 1: _temp3248 >= _temp3246.last_plus_one){ _throw( Null_Exception);}*
_temp3248;})); _temp3242->f2= 0; _temp3242->f3= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp3243= yyvs; struct _xenum_struct** _temp3245= _temp3243.curr
+ yyvsp_offset; if( _temp3245 < _temp3243.base? 1: _temp3245 >= _temp3243.last_plus_one){
_throw( Null_Exception);}* _temp3245;})); _temp3242;});( struct _xenum_struct*)
_temp3241;}); break; case 95: _LL3240: yyval=({ struct Cyc_QualSpecList_tok_struct*
_temp3250=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp3250->tag= Cyc_QualSpecList_tok_tag; _temp3250->f1=({ struct _tuple14*
_temp3251=( struct _tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp3251->f1=
Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2 _temp3261=
yyvs; struct _xenum_struct** _temp3263= _temp3261.curr +( yyvsp_offset - 2); if(
_temp3263 < _temp3261.base? 1: _temp3263 >= _temp3261.last_plus_one){ _throw(
Null_Exception);}* _temp3263;})),(* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp3264= yyvs; struct _xenum_struct** _temp3266= _temp3264.curr +
yyvsp_offset; if( _temp3266 < _temp3264.base? 1: _temp3266 >= _temp3264.last_plus_one){
_throw( Null_Exception);}* _temp3266;}))).f1); _temp3251->f2=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3258= yyvs; struct _xenum_struct** _temp3260= _temp3258.curr
+ yyvsp_offset; if( _temp3260 < _temp3258.base? 1: _temp3260 >= _temp3258.last_plus_one){
_throw( Null_Exception);}* _temp3260;}))).f2; _temp3251->f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp3252= yyvs; struct _xenum_struct** _temp3254= _temp3252.curr
+( yyvsp_offset - 1); if( _temp3254 < _temp3252.base? 1: _temp3254 >= _temp3252.last_plus_one){
_throw( Null_Exception);}* _temp3254;})),(* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp3255= yyvs; struct _xenum_struct** _temp3257= _temp3255.curr +
yyvsp_offset; if( _temp3257 < _temp3255.base? 1: _temp3257 >= _temp3255.last_plus_one){
_throw( Null_Exception);}* _temp3257;}))).f3); _temp3251;});( struct
_xenum_struct*) _temp3250;}); break; case 96: _LL3249: yyval=({ struct Cyc_DeclaratorExpoptList_tok_struct*
_temp3268=( struct Cyc_DeclaratorExpoptList_tok_struct*) GC_malloc( sizeof(
struct Cyc_DeclaratorExpoptList_tok_struct)); _temp3268->tag= Cyc_DeclaratorExpoptList_tok_tag;
_temp3268->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_DeclaratorExpoptList_tok(({ struct _tagged_ptr2 _temp3269= yyvs;
struct _xenum_struct** _temp3271= _temp3269.curr + yyvsp_offset; if( _temp3271 <
_temp3269.base? 1: _temp3271 >= _temp3269.last_plus_one){ _throw( Null_Exception);}*
_temp3271;})));( struct _xenum_struct*) _temp3268;}); break; case 97: _LL3267:
yyval=({ struct Cyc_DeclaratorExpoptList_tok_struct* _temp3273=( struct Cyc_DeclaratorExpoptList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpoptList_tok_struct)); _temp3273->tag=
Cyc_DeclaratorExpoptList_tok_tag; _temp3273->f1=({ struct Cyc_List_List*
_temp3274=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3274->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(({ struct _tagged_ptr2
_temp3275= yyvs; struct _xenum_struct** _temp3277= _temp3275.curr + yyvsp_offset;
if( _temp3277 < _temp3275.base? 1: _temp3277 >= _temp3275.last_plus_one){ _throw(
Null_Exception);}* _temp3277;})); _temp3274->tl= 0; _temp3274;});( struct
_xenum_struct*) _temp3273;}); break; case 98: _LL3272: yyval=({ struct Cyc_DeclaratorExpoptList_tok_struct*
_temp3279=( struct Cyc_DeclaratorExpoptList_tok_struct*) GC_malloc( sizeof(
struct Cyc_DeclaratorExpoptList_tok_struct)); _temp3279->tag= Cyc_DeclaratorExpoptList_tok_tag;
_temp3279->f1=({ struct Cyc_List_List* _temp3280=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3280->hd=( void*) Cyc_yyget_DeclaratorExpopt_tok(({
struct _tagged_ptr2 _temp3284= yyvs; struct _xenum_struct** _temp3286= _temp3284.curr
+ yyvsp_offset; if( _temp3286 < _temp3284.base? 1: _temp3286 >= _temp3284.last_plus_one){
_throw( Null_Exception);}* _temp3286;})); _temp3280->tl= Cyc_yyget_DeclaratorExpoptList_tok(({
struct _tagged_ptr2 _temp3281= yyvs; struct _xenum_struct** _temp3283= _temp3281.curr
+( yyvsp_offset - 2); if( _temp3283 < _temp3281.base? 1: _temp3283 >= _temp3281.last_plus_one){
_throw( Null_Exception);}* _temp3283;})); _temp3280;});( struct _xenum_struct*)
_temp3279;}); break; case 99: _LL3278: yyval=({ struct Cyc_DeclaratorExpopt_tok_struct*
_temp3288=( struct Cyc_DeclaratorExpopt_tok_struct*) GC_malloc( sizeof( struct
Cyc_DeclaratorExpopt_tok_struct)); _temp3288->tag= Cyc_DeclaratorExpopt_tok_tag;
_temp3288->f1=({ struct _tuple15* _temp3289=( struct _tuple15*) GC_malloc(
sizeof( struct _tuple15)); _temp3289->f1= Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp3290= yyvs; struct _xenum_struct** _temp3292= _temp3290.curr +
yyvsp_offset; if( _temp3292 < _temp3290.base? 1: _temp3292 >= _temp3290.last_plus_one){
_throw( Null_Exception);}* _temp3292;})); _temp3289->f2= 0; _temp3289;});(
struct _xenum_struct*) _temp3288;}); break; case 100: _LL3287: yyval=({ struct
Cyc_DeclaratorExpopt_tok_struct* _temp3294=( struct Cyc_DeclaratorExpopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorExpopt_tok_struct)); _temp3294->tag= Cyc_DeclaratorExpopt_tok_tag;
_temp3294->f1=({ struct _tuple15* _temp3295=( struct _tuple15*) GC_malloc(
sizeof( struct _tuple15)); _temp3295->f1=({ struct Cyc_Parse_Declarator*
_temp3300=( struct Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp3300->id=({ struct _tuple1* _temp3301=( struct _tuple1*) GC_malloc( sizeof(
struct _tuple1)); _temp3301->f1=({ struct Cyc_Absyn_Rel_n_struct* _temp3305=(
struct Cyc_Absyn_Rel_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct));
_temp3305->tag= Cyc_Absyn_Rel_n_tag; _temp3305->f1= 0;( void*) _temp3305;});
_temp3301->f2=({ struct _tagged_string* _temp3302=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string)); _temp3302[ 0]=( struct
_tagged_string)({ char* _temp3303=( char*)""; struct _tagged_string _temp3304;
_temp3304.curr= _temp3303; _temp3304.base= _temp3303; _temp3304.last_plus_one=
_temp3303 + 1; _temp3304;}); _temp3302;}); _temp3301;}); _temp3300->tms= 0;
_temp3300;}); _temp3295->f2=({ struct Cyc_Core_Opt* _temp3296=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3296->v=( void*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp3297= yyvs; struct _xenum_struct** _temp3299= _temp3297.curr
+ yyvsp_offset; if( _temp3299 < _temp3297.base? 1: _temp3299 >= _temp3297.last_plus_one){
_throw( Null_Exception);}* _temp3299;})); _temp3296;}); _temp3295;});( struct
_xenum_struct*) _temp3294;}); break; case 101: _LL3293: yyval=({ struct Cyc_DeclaratorExpopt_tok_struct*
_temp3307=( struct Cyc_DeclaratorExpopt_tok_struct*) GC_malloc( sizeof( struct
Cyc_DeclaratorExpopt_tok_struct)); _temp3307->tag= Cyc_DeclaratorExpopt_tok_tag;
_temp3307->f1=({ struct _tuple15* _temp3308=( struct _tuple15*) GC_malloc(
sizeof( struct _tuple15)); _temp3308->f1= Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp3313= yyvs; struct _xenum_struct** _temp3315= _temp3313.curr +(
yyvsp_offset - 2); if( _temp3315 < _temp3313.base? 1: _temp3315 >= _temp3313.last_plus_one){
_throw( Null_Exception);}* _temp3315;})); _temp3308->f2=({ struct Cyc_Core_Opt*
_temp3309=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3309->v=( void*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp3310= yyvs;
struct _xenum_struct** _temp3312= _temp3310.curr + yyvsp_offset; if( _temp3312 <
_temp3310.base? 1: _temp3312 >= _temp3310.last_plus_one){ _throw( Null_Exception);}*
_temp3312;})); _temp3309;}); _temp3308;});( struct _xenum_struct*) _temp3307;});
break; case 102: _LL3306: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp3317=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3317->tag= Cyc_TypeSpecifier_tok_tag; _temp3317->f1=( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp3318=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3318->tag= Cyc_Parse_Decl_spec_tag; _temp3318->f1= Cyc_Absyn_enum_decl( Cyc_Absyn_Public,
0, 0,({ struct Cyc_Core_Opt* _temp3319=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp3319->v=( void*) Cyc_yyget_EnumeratorFieldList_tok(({
struct _tagged_ptr2 _temp3320= yyvs; struct _xenum_struct** _temp3322= _temp3320.curr
+( yyvsp_offset - 1); if( _temp3322 < _temp3320.base? 1: _temp3322 >= _temp3320.last_plus_one){
_throw( Null_Exception);}* _temp3322;})); _temp3319;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3323= yyls; struct Cyc_Yyltype* _temp3325= _temp3323.curr
+( yylsp_offset - 3); if( _temp3325 < _temp3323.base? 1: _temp3325 >= _temp3323.last_plus_one){
_throw( Null_Exception);}* _temp3325;}).first_line,({ struct _tagged_ptr3
_temp3326= yyls; struct Cyc_Yyltype* _temp3328= _temp3326.curr + yylsp_offset;
if( _temp3328 < _temp3326.base? 1: _temp3328 >= _temp3326.last_plus_one){ _throw(
Null_Exception);}* _temp3328;}).last_line));( void*) _temp3318;});( struct
_xenum_struct*) _temp3317;}); Cyc_Parse_unimp2(( struct _tagged_string)({ char*
_temp3329=( char*)"anonymous enums"; struct _tagged_string _temp3330; _temp3330.curr=
_temp3329; _temp3330.base= _temp3329; _temp3330.last_plus_one= _temp3329 + 16;
_temp3330;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3331= yyls;
struct Cyc_Yyltype* _temp3333= _temp3331.curr +( yylsp_offset - 3); if(
_temp3333 < _temp3331.base? 1: _temp3333 >= _temp3331.last_plus_one){ _throw(
Null_Exception);}* _temp3333;}).first_line,({ struct _tagged_ptr3 _temp3334=
yyls; struct Cyc_Yyltype* _temp3336= _temp3334.curr + yylsp_offset; if(
_temp3336 < _temp3334.base? 1: _temp3336 >= _temp3334.last_plus_one){ _throw(
Null_Exception);}* _temp3336;}).last_line)); break; case 103: _LL3316: { struct
Cyc_List_List* ts=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3338= yyls; struct Cyc_Yyltype* _temp3340= _temp3338.curr
+( yylsp_offset - 3); if( _temp3340 < _temp3338.base? 1: _temp3340 >= _temp3338.last_plus_one){
_throw( Null_Exception);}* _temp3340;}).first_line,({ struct _tagged_ptr3
_temp3341= yyls; struct Cyc_Yyltype* _temp3343= _temp3341.curr +( yylsp_offset -
3); if( _temp3343 < _temp3341.base? 1: _temp3343 >= _temp3341.last_plus_one){
_throw( Null_Exception);}* _temp3343;}).last_line), Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3344= yyvs; struct _xenum_struct** _temp3346= _temp3344.curr
+( yyvsp_offset - 3); if( _temp3346 < _temp3344.base? 1: _temp3346 >= _temp3344.last_plus_one){
_throw( Null_Exception);}* _temp3346;}))); yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp3347=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3347->tag= Cyc_TypeSpecifier_tok_tag; _temp3347->f1=( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp3348=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3348->tag= Cyc_Parse_Decl_spec_tag; _temp3348->f1= Cyc_Absyn_enum_decl( Cyc_Absyn_Public,({
struct Cyc_Core_Opt* _temp3349=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3349->v=( void*) Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp3350= yyvs; struct _xenum_struct** _temp3352= _temp3350.curr +(
yyvsp_offset - 4); if( _temp3352 < _temp3350.base? 1: _temp3352 >= _temp3350.last_plus_one){
_throw( Null_Exception);}* _temp3352;})); _temp3349;}), ts,({ struct Cyc_Core_Opt*
_temp3353=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3353->v=( void*) Cyc_yyget_EnumeratorFieldList_tok(({ struct _tagged_ptr2
_temp3354= yyvs; struct _xenum_struct** _temp3356= _temp3354.curr +(
yyvsp_offset - 1); if( _temp3356 < _temp3354.base? 1: _temp3356 >= _temp3354.last_plus_one){
_throw( Null_Exception);}* _temp3356;})); _temp3353;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3357= yyls; struct Cyc_Yyltype* _temp3359= _temp3357.curr
+( yylsp_offset - 5); if( _temp3359 < _temp3357.base? 1: _temp3359 >= _temp3357.last_plus_one){
_throw( Null_Exception);}* _temp3359;}).first_line,({ struct _tagged_ptr3
_temp3360= yyls; struct Cyc_Yyltype* _temp3362= _temp3360.curr + yylsp_offset;
if( _temp3362 < _temp3360.base? 1: _temp3362 >= _temp3360.last_plus_one){ _throw(
Null_Exception);}* _temp3362;}).last_line));( void*) _temp3348;});( struct
_xenum_struct*) _temp3347;}); break;} case 104: _LL3337: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp3364=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3364->tag= Cyc_TypeSpecifier_tok_tag; _temp3364->f1=( void*) Cyc_Parse_type_spec(({
struct Cyc_Absyn_EnumType_struct* _temp3365=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp3365->tag= Cyc_Absyn_EnumType_tag;
_temp3365->f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3366= yyvs; struct _xenum_struct** _temp3368= _temp3366.curr +(
yyvsp_offset - 1); if( _temp3368 < _temp3366.base? 1: _temp3368 >= _temp3366.last_plus_one){
_throw( Null_Exception);}* _temp3368;})); _temp3365->f2= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3369= yyvs; struct _xenum_struct** _temp3371= _temp3369.curr
+ yyvsp_offset; if( _temp3371 < _temp3369.base? 1: _temp3371 >= _temp3369.last_plus_one){
_throw( Null_Exception);}* _temp3371;})); _temp3365->f3= 0;( void*) _temp3365;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3372= yyls; struct Cyc_Yyltype*
_temp3374= _temp3372.curr +( yylsp_offset - 2); if( _temp3374 < _temp3372.base?
1: _temp3374 >= _temp3372.last_plus_one){ _throw( Null_Exception);}* _temp3374;}).first_line,({
struct _tagged_ptr3 _temp3375= yyls; struct Cyc_Yyltype* _temp3377= _temp3375.curr
+ yylsp_offset; if( _temp3377 < _temp3375.base? 1: _temp3377 >= _temp3375.last_plus_one){
_throw( Null_Exception);}* _temp3377;}).last_line));( struct _xenum_struct*)
_temp3364;}); break; case 105: _LL3363: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp3379=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3379->tag= Cyc_TypeSpecifier_tok_tag; _temp3379->f1=( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp3380=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3380->tag= Cyc_Parse_Decl_spec_tag; _temp3380->f1= Cyc_Absyn_xenum_decl(
Cyc_Absyn_Public, Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3381= yyvs;
struct _xenum_struct** _temp3383= _temp3381.curr +( yyvsp_offset - 3); if(
_temp3383 < _temp3381.base? 1: _temp3383 >= _temp3381.last_plus_one){ _throw(
Null_Exception);}* _temp3383;})), Cyc_yyget_EnumeratorFieldList_tok(({ struct
_tagged_ptr2 _temp3384= yyvs; struct _xenum_struct** _temp3386= _temp3384.curr +(
yyvsp_offset - 1); if( _temp3386 < _temp3384.base? 1: _temp3386 >= _temp3384.last_plus_one){
_throw( Null_Exception);}* _temp3386;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3387= yyls; struct Cyc_Yyltype* _temp3389= _temp3387.curr +(
yylsp_offset - 4); if( _temp3389 < _temp3387.base? 1: _temp3389 >= _temp3387.last_plus_one){
_throw( Null_Exception);}* _temp3389;}).first_line,({ struct _tagged_ptr3
_temp3390= yyls; struct Cyc_Yyltype* _temp3392= _temp3390.curr + yylsp_offset;
if( _temp3392 < _temp3390.base? 1: _temp3392 >= _temp3390.last_plus_one){ _throw(
Null_Exception);}* _temp3392;}).last_line));( void*) _temp3380;});( struct
_xenum_struct*) _temp3379;}); break; case 106: _LL3378: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp3394=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3394->tag= Cyc_TypeSpecifier_tok_tag; _temp3394->f1=( void*) Cyc_Parse_type_spec(({
struct Cyc_Absyn_XenumType_struct* _temp3395=( struct Cyc_Absyn_XenumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XenumType_struct)); _temp3395->tag= Cyc_Absyn_XenumType_tag;
_temp3395->f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3396= yyvs;
struct _xenum_struct** _temp3398= _temp3396.curr + yyvsp_offset; if( _temp3398 <
_temp3396.base? 1: _temp3398 >= _temp3396.last_plus_one){ _throw( Null_Exception);}*
_temp3398;})); _temp3395->f2= 0;( void*) _temp3395;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3399= yyls; struct Cyc_Yyltype* _temp3401= _temp3399.curr
+( yylsp_offset - 1); if( _temp3401 < _temp3399.base? 1: _temp3401 >= _temp3399.last_plus_one){
_throw( Null_Exception);}* _temp3401;}).first_line,({ struct _tagged_ptr3
_temp3402= yyls; struct Cyc_Yyltype* _temp3404= _temp3402.curr + yylsp_offset;
if( _temp3404 < _temp3402.base? 1: _temp3404 >= _temp3402.last_plus_one){ _throw(
Null_Exception);}* _temp3404;}).last_line));( struct _xenum_struct*) _temp3394;});
break; case 107: _LL3393: yyval=({ struct Cyc_EnumeratorFieldList_tok_struct*
_temp3406=( struct Cyc_EnumeratorFieldList_tok_struct*) GC_malloc( sizeof(
struct Cyc_EnumeratorFieldList_tok_struct)); _temp3406->tag= Cyc_EnumeratorFieldList_tok_tag;
_temp3406->f1=({ struct Cyc_List_List* _temp3407=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3407->hd=( void*) Cyc_yyget_EnumeratorField_tok(({
struct _tagged_ptr2 _temp3408= yyvs; struct _xenum_struct** _temp3410= _temp3408.curr
+ yyvsp_offset; if( _temp3410 < _temp3408.base? 1: _temp3410 >= _temp3408.last_plus_one){
_throw( Null_Exception);}* _temp3410;})); _temp3407->tl= 0; _temp3407;});(
struct _xenum_struct*) _temp3406;}); break; case 108: _LL3405: yyval=({ struct
Cyc_EnumeratorFieldList_tok_struct* _temp3412=( struct Cyc_EnumeratorFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_EnumeratorFieldList_tok_struct)); _temp3412->tag=
Cyc_EnumeratorFieldList_tok_tag; _temp3412->f1=({ struct Cyc_List_List*
_temp3413=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3413->hd=( void*) Cyc_yyget_EnumeratorField_tok(({ struct _tagged_ptr2
_temp3414= yyvs; struct _xenum_struct** _temp3416= _temp3414.curr +(
yyvsp_offset - 1); if( _temp3416 < _temp3414.base? 1: _temp3416 >= _temp3414.last_plus_one){
_throw( Null_Exception);}* _temp3416;})); _temp3413->tl= 0; _temp3413;});(
struct _xenum_struct*) _temp3412;}); break; case 109: _LL3411: yyval=({ struct
Cyc_EnumeratorFieldList_tok_struct* _temp3418=( struct Cyc_EnumeratorFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_EnumeratorFieldList_tok_struct)); _temp3418->tag=
Cyc_EnumeratorFieldList_tok_tag; _temp3418->f1=({ struct Cyc_List_List*
_temp3419=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3419->hd=( void*) Cyc_yyget_EnumeratorField_tok(({ struct _tagged_ptr2
_temp3423= yyvs; struct _xenum_struct** _temp3425= _temp3423.curr +(
yyvsp_offset - 2); if( _temp3425 < _temp3423.base? 1: _temp3425 >= _temp3423.last_plus_one){
_throw( Null_Exception);}* _temp3425;})); _temp3419->tl= Cyc_yyget_EnumeratorFieldList_tok(({
struct _tagged_ptr2 _temp3420= yyvs; struct _xenum_struct** _temp3422= _temp3420.curr
+ yyvsp_offset; if( _temp3422 < _temp3420.base? 1: _temp3422 >= _temp3420.last_plus_one){
_throw( Null_Exception);}* _temp3422;})); _temp3419;});( struct _xenum_struct*)
_temp3418;}); break; case 110: _LL3417: yyval=({ struct Cyc_EnumeratorFieldList_tok_struct*
_temp3427=( struct Cyc_EnumeratorFieldList_tok_struct*) GC_malloc( sizeof(
struct Cyc_EnumeratorFieldList_tok_struct)); _temp3427->tag= Cyc_EnumeratorFieldList_tok_tag;
_temp3427->f1=({ struct Cyc_List_List* _temp3428=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3428->hd=( void*) Cyc_yyget_EnumeratorField_tok(({
struct _tagged_ptr2 _temp3432= yyvs; struct _xenum_struct** _temp3434= _temp3432.curr
+( yyvsp_offset - 2); if( _temp3434 < _temp3432.base? 1: _temp3434 >= _temp3432.last_plus_one){
_throw( Null_Exception);}* _temp3434;})); _temp3428->tl= Cyc_yyget_EnumeratorFieldList_tok(({
struct _tagged_ptr2 _temp3429= yyvs; struct _xenum_struct** _temp3431= _temp3429.curr
+ yyvsp_offset; if( _temp3431 < _temp3429.base? 1: _temp3431 >= _temp3429.last_plus_one){
_throw( Null_Exception);}* _temp3431;})); _temp3428;});( struct _xenum_struct*)
_temp3427;}); break; case 111: _LL3426: yyval=({ struct Cyc_EnumeratorField_tok_struct*
_temp3436=( struct Cyc_EnumeratorField_tok_struct*) GC_malloc( sizeof( struct
Cyc_EnumeratorField_tok_struct)); _temp3436->tag= Cyc_EnumeratorField_tok_tag;
_temp3436->f1=({ struct Cyc_Absyn_Enumfield* _temp3437=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp3437->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3444= yyvs; struct _xenum_struct** _temp3446= _temp3444.curr
+ yyvsp_offset; if( _temp3446 < _temp3444.base? 1: _temp3446 >= _temp3444.last_plus_one){
_throw( Null_Exception);}* _temp3446;})); _temp3437->tag= 0; _temp3437->tvs= 0;
_temp3437->typs= 0; _temp3437->loc= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3438= yyls; struct Cyc_Yyltype* _temp3440= _temp3438.curr +
yylsp_offset; if( _temp3440 < _temp3438.base? 1: _temp3440 >= _temp3438.last_plus_one){
_throw( Null_Exception);}* _temp3440;}).first_line,({ struct _tagged_ptr3
_temp3441= yyls; struct Cyc_Yyltype* _temp3443= _temp3441.curr + yylsp_offset;
if( _temp3443 < _temp3441.base? 1: _temp3443 >= _temp3441.last_plus_one){ _throw(
Null_Exception);}* _temp3443;}).last_line); _temp3437;});( struct _xenum_struct*)
_temp3436;}); break; case 112: _LL3435: yyval=({ struct Cyc_EnumeratorField_tok_struct*
_temp3448=( struct Cyc_EnumeratorField_tok_struct*) GC_malloc( sizeof( struct
Cyc_EnumeratorField_tok_struct)); _temp3448->tag= Cyc_EnumeratorField_tok_tag;
_temp3448->f1=({ struct Cyc_Absyn_Enumfield* _temp3449=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp3449->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3459= yyvs; struct _xenum_struct** _temp3461= _temp3459.curr
+( yyvsp_offset - 2); if( _temp3461 < _temp3459.base? 1: _temp3461 >= _temp3459.last_plus_one){
_throw( Null_Exception);}* _temp3461;})); _temp3449->tag=( struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp3456= yyvs; struct _xenum_struct**
_temp3458= _temp3456.curr + yyvsp_offset; if( _temp3458 < _temp3456.base? 1:
_temp3458 >= _temp3456.last_plus_one){ _throw( Null_Exception);}* _temp3458;}));
_temp3449->tvs= 0; _temp3449->typs= 0; _temp3449->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3450= yyls; struct Cyc_Yyltype* _temp3452= _temp3450.curr
+( yylsp_offset - 2); if( _temp3452 < _temp3450.base? 1: _temp3452 >= _temp3450.last_plus_one){
_throw( Null_Exception);}* _temp3452;}).first_line,({ struct _tagged_ptr3
_temp3453= yyls; struct Cyc_Yyltype* _temp3455= _temp3453.curr + yylsp_offset;
if( _temp3455 < _temp3453.base? 1: _temp3455 >= _temp3453.last_plus_one){ _throw(
Null_Exception);}* _temp3455;}).last_line); _temp3449;});( struct _xenum_struct*)
_temp3448;}); break; case 113: _LL3447: { struct Cyc_List_List* typs=(( struct
Cyc_List_List*(*)( struct _tuple10*(* f)( struct Cyc_Position_Segment*, struct
_tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp3463= yyls; struct Cyc_Yyltype* _temp3465= _temp3463.curr +( yylsp_offset -
1); if( _temp3465 < _temp3463.base? 1: _temp3465 >= _temp3463.last_plus_one){
_throw( Null_Exception);}* _temp3465;}).first_line,({ struct _tagged_ptr3
_temp3466= yyls; struct Cyc_Yyltype* _temp3468= _temp3466.curr +( yylsp_offset -
1); if( _temp3468 < _temp3466.base? 1: _temp3468 >= _temp3466.last_plus_one){
_throw( Null_Exception);}* _temp3468;}).last_line),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp3469= yyvs; struct _xenum_struct** _temp3471= _temp3469.curr
+( yyvsp_offset - 1); if( _temp3471 < _temp3469.base? 1: _temp3471 >= _temp3469.last_plus_one){
_throw( Null_Exception);}* _temp3471;})))); struct Cyc_List_List* tvs=(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3472=
yyls; struct Cyc_Yyltype* _temp3474= _temp3472.curr +( yylsp_offset - 3); if(
_temp3474 < _temp3472.base? 1: _temp3474 >= _temp3472.last_plus_one){ _throw(
Null_Exception);}* _temp3474;}).first_line,({ struct _tagged_ptr3 _temp3475=
yyls; struct Cyc_Yyltype* _temp3477= _temp3475.curr +( yylsp_offset - 3); if(
_temp3477 < _temp3475.base? 1: _temp3477 >= _temp3475.last_plus_one){ _throw(
Null_Exception);}* _temp3477;}).last_line), Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp3478= yyvs; struct _xenum_struct** _temp3480= _temp3478.curr +(
yyvsp_offset - 3); if( _temp3480 < _temp3478.base? 1: _temp3480 >= _temp3478.last_plus_one){
_throw( Null_Exception);}* _temp3480;}))); yyval=({ struct Cyc_EnumeratorField_tok_struct*
_temp3481=( struct Cyc_EnumeratorField_tok_struct*) GC_malloc( sizeof( struct
Cyc_EnumeratorField_tok_struct)); _temp3481->tag= Cyc_EnumeratorField_tok_tag;
_temp3481->f1=({ struct Cyc_Absyn_Enumfield* _temp3482=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp3482->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3489= yyvs; struct _xenum_struct** _temp3491= _temp3489.curr
+( yyvsp_offset - 4); if( _temp3491 < _temp3489.base? 1: _temp3491 >= _temp3489.last_plus_one){
_throw( Null_Exception);}* _temp3491;})); _temp3482->tag= 0; _temp3482->tvs= tvs;
_temp3482->typs= typs; _temp3482->loc= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3483= yyls; struct Cyc_Yyltype* _temp3485= _temp3483.curr +(
yylsp_offset - 4); if( _temp3485 < _temp3483.base? 1: _temp3485 >= _temp3483.last_plus_one){
_throw( Null_Exception);}* _temp3485;}).first_line,({ struct _tagged_ptr3
_temp3486= yyls; struct Cyc_Yyltype* _temp3488= _temp3486.curr + yylsp_offset;
if( _temp3488 < _temp3486.base? 1: _temp3488 >= _temp3486.last_plus_one){ _throw(
Null_Exception);}* _temp3488;}).last_line); _temp3482;});( struct _xenum_struct*)
_temp3481;}); break;} case 114: _LL3462: yyval=({ struct _tagged_ptr2 _temp3493=
yyvs; struct _xenum_struct** _temp3495= _temp3493.curr + yyvsp_offset; if(
_temp3495 < _temp3493.base? 1: _temp3495 >= _temp3493.last_plus_one){ _throw(
Null_Exception);}* _temp3495;}); break; case 115: _LL3492: yyval=({ struct Cyc_Declarator_tok_struct*
_temp3497=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3497->tag= Cyc_Declarator_tok_tag; _temp3497->f1=({ struct Cyc_Parse_Declarator*
_temp3498=( struct Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp3498->id=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3505= yyvs;
struct _xenum_struct** _temp3507= _temp3505.curr + yyvsp_offset; if( _temp3507 <
_temp3505.base? 1: _temp3507 >= _temp3505.last_plus_one){ _throw( Null_Exception);}*
_temp3507;})))->id; _temp3498->tms=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_TypeModifierList_tok(({
struct _tagged_ptr2 _temp3499= yyvs; struct _xenum_struct** _temp3501= _temp3499.curr
+( yyvsp_offset - 1); if( _temp3501 < _temp3499.base? 1: _temp3501 >= _temp3499.last_plus_one){
_throw( Null_Exception);}* _temp3501;})),( Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp3502= yyvs; struct _xenum_struct** _temp3504= _temp3502.curr +
yyvsp_offset; if( _temp3504 < _temp3502.base? 1: _temp3504 >= _temp3502.last_plus_one){
_throw( Null_Exception);}* _temp3504;})))->tms); _temp3498;});( struct
_xenum_struct*) _temp3497;}); break; case 116: _LL3496: yyval=({ struct Cyc_Declarator_tok_struct*
_temp3509=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3509->tag= Cyc_Declarator_tok_tag; _temp3509->f1=({ struct Cyc_Parse_Declarator*
_temp3510=( struct Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp3510->id= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3511= yyvs;
struct _xenum_struct** _temp3513= _temp3511.curr + yyvsp_offset; if( _temp3513 <
_temp3511.base? 1: _temp3513 >= _temp3511.last_plus_one){ _throw( Null_Exception);}*
_temp3513;})); _temp3510->tms= 0; _temp3510;});( struct _xenum_struct*)
_temp3509;}); break; case 117: _LL3508: yyval=({ struct _tagged_ptr2 _temp3515=
yyvs; struct _xenum_struct** _temp3517= _temp3515.curr +( yyvsp_offset - 1); if(
_temp3517 < _temp3515.base? 1: _temp3517 >= _temp3515.last_plus_one){ _throw(
Null_Exception);}* _temp3517;}); break; case 118: _LL3514: yyval=({ struct Cyc_Declarator_tok_struct*
_temp3519=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3519->tag= Cyc_Declarator_tok_tag; _temp3519->f1=({ struct Cyc_Parse_Declarator*
_temp3520=( struct Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp3520->id=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3525= yyvs;
struct _xenum_struct** _temp3527= _temp3525.curr +( yyvsp_offset - 2); if(
_temp3527 < _temp3525.base? 1: _temp3527 >= _temp3525.last_plus_one){ _throw(
Null_Exception);}* _temp3527;})))->id; _temp3520->tms=({ struct Cyc_List_List*
_temp3521=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3521->hd=( void*) Cyc_Absyn_Carray_mod; _temp3521->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3522= yyvs; struct _xenum_struct** _temp3524= _temp3522.curr
+( yyvsp_offset - 2); if( _temp3524 < _temp3522.base? 1: _temp3524 >= _temp3522.last_plus_one){
_throw( Null_Exception);}* _temp3524;})))->tms; _temp3521;}); _temp3520;});(
struct _xenum_struct*) _temp3519;}); break; case 119: _LL3518: yyval=({ struct
Cyc_Declarator_tok_struct* _temp3529=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp3529->tag= Cyc_Declarator_tok_tag;
_temp3529->f1=({ struct Cyc_Parse_Declarator* _temp3530=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3530->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3539= yyvs; struct _xenum_struct** _temp3541= _temp3539.curr
+( yyvsp_offset - 3); if( _temp3541 < _temp3539.base? 1: _temp3541 >= _temp3539.last_plus_one){
_throw( Null_Exception);}* _temp3541;})))->id; _temp3530->tms=({ struct Cyc_List_List*
_temp3531=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3531->hd=( void*)({ struct Cyc_Absyn_ConstArray_mod_struct* _temp3535=(
struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct));
_temp3535->tag= Cyc_Absyn_ConstArray_mod_tag; _temp3535->f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp3536= yyvs; struct _xenum_struct** _temp3538= _temp3536.curr
+( yyvsp_offset - 1); if( _temp3538 < _temp3536.base? 1: _temp3538 >= _temp3536.last_plus_one){
_throw( Null_Exception);}* _temp3538;}));( void*) _temp3535;}); _temp3531->tl=(
Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3532= yyvs; struct
_xenum_struct** _temp3534= _temp3532.curr +( yyvsp_offset - 3); if( _temp3534 <
_temp3532.base? 1: _temp3534 >= _temp3532.last_plus_one){ _throw( Null_Exception);}*
_temp3534;})))->tms; _temp3531;}); _temp3530;});( struct _xenum_struct*)
_temp3529;}); break; case 120: _LL3528: { struct _tuple16 _temp3548; struct Cyc_Core_Opt*
_temp3549; int _temp3551; struct Cyc_List_List* _temp3553; struct _tuple16*
_temp3546= Cyc_yyget_ParamDeclListBool_tok(({ struct _tagged_ptr2 _temp3543=
yyvs; struct _xenum_struct** _temp3545= _temp3543.curr +( yyvsp_offset - 1); if(
_temp3545 < _temp3543.base? 1: _temp3545 >= _temp3543.last_plus_one){ _throw(
Null_Exception);}* _temp3545;})); _temp3548=* _temp3546; _LL3554: _temp3553=
_temp3548.f1; goto _LL3552; _LL3552: _temp3551= _temp3548.f2; goto _LL3550;
_LL3550: _temp3549= _temp3548.f3; goto _LL3547; _LL3547: yyval=({ struct Cyc_Declarator_tok_struct*
_temp3555=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3555->tag= Cyc_Declarator_tok_tag; _temp3555->f1=({ struct Cyc_Parse_Declarator*
_temp3556=( struct Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp3556->id=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3563= yyvs;
struct _xenum_struct** _temp3565= _temp3563.curr +( yyvsp_offset - 3); if(
_temp3565 < _temp3563.base? 1: _temp3565 >= _temp3563.last_plus_one){ _throw(
Null_Exception);}* _temp3565;})))->id; _temp3556->tms=({ struct Cyc_List_List*
_temp3557=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3557->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct* _temp3561=(
struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3561->tag= Cyc_Absyn_Function_mod_tag; _temp3561->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp3562=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3562->tag= Cyc_Absyn_WithTypes_tag; _temp3562->f1= _temp3553; _temp3562->f2=
_temp3551; _temp3562->f3= _temp3549;( void*) _temp3562;});( void*) _temp3561;});
_temp3557->tl=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3558= yyvs;
struct _xenum_struct** _temp3560= _temp3558.curr +( yyvsp_offset - 3); if(
_temp3560 < _temp3558.base? 1: _temp3560 >= _temp3558.last_plus_one){ _throw(
Null_Exception);}* _temp3560;})))->tms; _temp3557;}); _temp3556;});( struct
_xenum_struct*) _temp3555;}); break;} case 121: _LL3542: yyval=({ struct Cyc_Declarator_tok_struct*
_temp3567=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3567->tag= Cyc_Declarator_tok_tag; _temp3567->f1=({ struct Cyc_Parse_Declarator*
_temp3568=( struct Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp3568->id=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3575= yyvs;
struct _xenum_struct** _temp3577= _temp3575.curr +( yyvsp_offset - 2); if(
_temp3577 < _temp3575.base? 1: _temp3577 >= _temp3575.last_plus_one){ _throw(
Null_Exception);}* _temp3577;})))->id; _temp3568->tms=({ struct Cyc_List_List*
_temp3569=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3569->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct* _temp3573=(
struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3573->tag= Cyc_Absyn_Function_mod_tag; _temp3573->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp3574=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3574->tag= Cyc_Absyn_WithTypes_tag; _temp3574->f1= 0; _temp3574->f2= 0;
_temp3574->f3= 0;( void*) _temp3574;});( void*) _temp3573;}); _temp3569->tl=(
Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3570= yyvs; struct
_xenum_struct** _temp3572= _temp3570.curr +( yyvsp_offset - 2); if( _temp3572 <
_temp3570.base? 1: _temp3572 >= _temp3570.last_plus_one){ _throw( Null_Exception);}*
_temp3572;})))->tms; _temp3569;}); _temp3568;});( struct _xenum_struct*)
_temp3567;}); break; case 122: _LL3566: yyval=({ struct Cyc_Declarator_tok_struct*
_temp3579=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3579->tag= Cyc_Declarator_tok_tag; _temp3579->f1=({ struct Cyc_Parse_Declarator*
_temp3580=( struct Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp3580->id=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3592= yyvs;
struct _xenum_struct** _temp3594= _temp3592.curr +( yyvsp_offset - 4); if(
_temp3594 < _temp3592.base? 1: _temp3594 >= _temp3592.last_plus_one){ _throw(
Null_Exception);}* _temp3594;})))->id; _temp3580->tms=({ struct Cyc_List_List*
_temp3581=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3581->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct* _temp3585=(
struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3585->tag= Cyc_Absyn_Function_mod_tag; _temp3585->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp3586=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3586->tag= Cyc_Absyn_WithTypes_tag; _temp3586->f1= 0; _temp3586->f2= 0;
_temp3586->f3=({ struct Cyc_Core_Opt* _temp3587=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3587->v=( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp3588=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3588->tag= Cyc_Absyn_JoinEff_tag; _temp3588->f1= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3589= yyvs; struct _xenum_struct** _temp3591= _temp3589.curr
+( yyvsp_offset - 1); if( _temp3591 < _temp3589.base? 1: _temp3591 >= _temp3589.last_plus_one){
_throw( Null_Exception);}* _temp3591;}));( void*) _temp3588;}); _temp3587;});(
void*) _temp3586;});( void*) _temp3585;}); _temp3581->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3582= yyvs; struct _xenum_struct** _temp3584= _temp3582.curr
+( yyvsp_offset - 4); if( _temp3584 < _temp3582.base? 1: _temp3584 >= _temp3582.last_plus_one){
_throw( Null_Exception);}* _temp3584;})))->tms; _temp3581;}); _temp3580;});(
struct _xenum_struct*) _temp3579;}); break; case 123: _LL3578: yyval=({ struct
Cyc_Declarator_tok_struct* _temp3596=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp3596->tag= Cyc_Declarator_tok_tag;
_temp3596->f1=({ struct Cyc_Parse_Declarator* _temp3597=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3597->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3613= yyvs; struct _xenum_struct** _temp3615= _temp3613.curr
+( yyvsp_offset - 3); if( _temp3615 < _temp3613.base? 1: _temp3615 >= _temp3613.last_plus_one){
_throw( Null_Exception);}* _temp3615;})))->id; _temp3597->tms=({ struct Cyc_List_List*
_temp3598=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3598->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct* _temp3602=(
struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3602->tag= Cyc_Absyn_Function_mod_tag; _temp3602->f1=( void*)({ struct Cyc_Absyn_NoTypes_struct*
_temp3603=( struct Cyc_Absyn_NoTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_NoTypes_struct));
_temp3603->tag= Cyc_Absyn_NoTypes_tag; _temp3603->f1= Cyc_yyget_IdList_tok(({
struct _tagged_ptr2 _temp3604= yyvs; struct _xenum_struct** _temp3606= _temp3604.curr
+( yyvsp_offset - 1); if( _temp3606 < _temp3604.base? 1: _temp3606 >= _temp3604.last_plus_one){
_throw( Null_Exception);}* _temp3606;})); _temp3603->f2= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3607= yyls; struct Cyc_Yyltype* _temp3609= _temp3607.curr
+( yylsp_offset - 3); if( _temp3609 < _temp3607.base? 1: _temp3609 >= _temp3607.last_plus_one){
_throw( Null_Exception);}* _temp3609;}).first_line,({ struct _tagged_ptr3
_temp3610= yyls; struct Cyc_Yyltype* _temp3612= _temp3610.curr + yylsp_offset;
if( _temp3612 < _temp3610.base? 1: _temp3612 >= _temp3610.last_plus_one){ _throw(
Null_Exception);}* _temp3612;}).last_line);( void*) _temp3603;});( void*)
_temp3602;}); _temp3598->tl=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2
_temp3599= yyvs; struct _xenum_struct** _temp3601= _temp3599.curr +(
yyvsp_offset - 3); if( _temp3601 < _temp3599.base? 1: _temp3601 >= _temp3599.last_plus_one){
_throw( Null_Exception);}* _temp3601;})))->tms; _temp3598;}); _temp3597;});(
struct _xenum_struct*) _temp3596;}); break; case 124: _LL3595: { struct Cyc_List_List*
ts=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3617=
yyls; struct Cyc_Yyltype* _temp3619= _temp3617.curr +( yylsp_offset - 2); if(
_temp3619 < _temp3617.base? 1: _temp3619 >= _temp3617.last_plus_one){ _throw(
Null_Exception);}* _temp3619;}).first_line,({ struct _tagged_ptr3 _temp3620=
yyls; struct Cyc_Yyltype* _temp3622= _temp3620.curr + yylsp_offset; if(
_temp3622 < _temp3620.base? 1: _temp3622 >= _temp3620.last_plus_one){ _throw(
Null_Exception);}* _temp3622;}).last_line),(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp3623= yyvs; struct _xenum_struct** _temp3625= _temp3623.curr +(
yyvsp_offset - 1); if( _temp3625 < _temp3623.base? 1: _temp3625 >= _temp3623.last_plus_one){
_throw( Null_Exception);}* _temp3625;})))); yyval=({ struct Cyc_Declarator_tok_struct*
_temp3626=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3626->tag= Cyc_Declarator_tok_tag; _temp3626->f1=({ struct Cyc_Parse_Declarator*
_temp3627=( struct Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp3627->id=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3639= yyvs;
struct _xenum_struct** _temp3641= _temp3639.curr +( yyvsp_offset - 3); if(
_temp3641 < _temp3639.base? 1: _temp3641 >= _temp3639.last_plus_one){ _throw(
Null_Exception);}* _temp3641;})))->id; _temp3627->tms=({ struct Cyc_List_List*
_temp3628=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3628->hd=( void*)({ struct Cyc_Absyn_TypeParams_mod_struct* _temp3632=(
struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct));
_temp3632->tag= Cyc_Absyn_TypeParams_mod_tag; _temp3632->f1= ts; _temp3632->f2=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3633= yyls; struct Cyc_Yyltype*
_temp3635= _temp3633.curr +( yylsp_offset - 3); if( _temp3635 < _temp3633.base?
1: _temp3635 >= _temp3633.last_plus_one){ _throw( Null_Exception);}* _temp3635;}).first_line,({
struct _tagged_ptr3 _temp3636= yyls; struct Cyc_Yyltype* _temp3638= _temp3636.curr
+ yylsp_offset; if( _temp3638 < _temp3636.base? 1: _temp3638 >= _temp3636.last_plus_one){
_throw( Null_Exception);}* _temp3638;}).last_line); _temp3632->f3= 0;( void*)
_temp3632;}); _temp3628->tl=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2
_temp3629= yyvs; struct _xenum_struct** _temp3631= _temp3629.curr +(
yyvsp_offset - 3); if( _temp3631 < _temp3629.base? 1: _temp3631 >= _temp3629.last_plus_one){
_throw( Null_Exception);}* _temp3631;})))->tms; _temp3628;}); _temp3627;});(
struct _xenum_struct*) _temp3626;}); break;} case 125: _LL3616:(( struct Cyc_Lexing_lexbuf*)
Cyc_Parse_lbuf->v)->lex_curr_pos -= 1;{ struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3643= yyls; struct Cyc_Yyltype* _temp3645= _temp3643.curr
+( yylsp_offset - 2); if( _temp3645 < _temp3643.base? 1: _temp3645 >= _temp3643.last_plus_one){
_throw( Null_Exception);}* _temp3645;}).first_line,({ struct _tagged_ptr3
_temp3646= yyls; struct Cyc_Yyltype* _temp3648= _temp3646.curr + yylsp_offset;
if( _temp3648 < _temp3646.base? 1: _temp3648 >= _temp3646.last_plus_one){ _throw(
Null_Exception);}* _temp3648;}).last_line),(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp3649= yyvs; struct _xenum_struct** _temp3651= _temp3649.curr +(
yyvsp_offset - 1); if( _temp3651 < _temp3649.base? 1: _temp3651 >= _temp3649.last_plus_one){
_throw( Null_Exception);}* _temp3651;})))); yyval=({ struct Cyc_Declarator_tok_struct*
_temp3652=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3652->tag= Cyc_Declarator_tok_tag; _temp3652->f1=({ struct Cyc_Parse_Declarator*
_temp3653=( struct Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp3653->id=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3665= yyvs;
struct _xenum_struct** _temp3667= _temp3665.curr +( yyvsp_offset - 3); if(
_temp3667 < _temp3665.base? 1: _temp3667 >= _temp3665.last_plus_one){ _throw(
Null_Exception);}* _temp3667;})))->id; _temp3653->tms=({ struct Cyc_List_List*
_temp3654=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3654->hd=( void*)({ struct Cyc_Absyn_TypeParams_mod_struct* _temp3658=(
struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct));
_temp3658->tag= Cyc_Absyn_TypeParams_mod_tag; _temp3658->f1= ts; _temp3658->f2=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3659= yyls; struct Cyc_Yyltype*
_temp3661= _temp3659.curr +( yylsp_offset - 3); if( _temp3661 < _temp3659.base?
1: _temp3661 >= _temp3659.last_plus_one){ _throw( Null_Exception);}* _temp3661;}).first_line,({
struct _tagged_ptr3 _temp3662= yyls; struct Cyc_Yyltype* _temp3664= _temp3662.curr
+ yylsp_offset; if( _temp3664 < _temp3662.base? 1: _temp3664 >= _temp3662.last_plus_one){
_throw( Null_Exception);}* _temp3664;}).last_line); _temp3658->f3= 0;( void*)
_temp3658;}); _temp3654->tl=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2
_temp3655= yyvs; struct _xenum_struct** _temp3657= _temp3655.curr +(
yyvsp_offset - 3); if( _temp3657 < _temp3655.base? 1: _temp3657 >= _temp3655.last_plus_one){
_throw( Null_Exception);}* _temp3657;})))->tms; _temp3654;}); _temp3653;});(
struct _xenum_struct*) _temp3652;}); break;} case 126: _LL3642: yyval=({ struct
Cyc_Declarator_tok_struct* _temp3669=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp3669->tag= Cyc_Declarator_tok_tag;
_temp3669->f1=({ struct Cyc_Parse_Declarator* _temp3670=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3670->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3685= yyvs; struct _xenum_struct** _temp3687= _temp3685.curr
+( yyvsp_offset - 1); if( _temp3687 < _temp3685.base? 1: _temp3687 >= _temp3685.last_plus_one){
_throw( Null_Exception);}* _temp3687;})))->id; _temp3670->tms=({ struct Cyc_List_List*
_temp3671=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3671->hd=( void*)({ struct Cyc_Absyn_Attributes_mod_struct* _temp3675=(
struct Cyc_Absyn_Attributes_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct));
_temp3675->tag= Cyc_Absyn_Attributes_mod_tag; _temp3675->f1= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3676= yyls; struct Cyc_Yyltype* _temp3678= _temp3676.curr
+ yylsp_offset; if( _temp3678 < _temp3676.base? 1: _temp3678 >= _temp3676.last_plus_one){
_throw( Null_Exception);}* _temp3678;}).first_line,({ struct _tagged_ptr3
_temp3679= yyls; struct Cyc_Yyltype* _temp3681= _temp3679.curr + yylsp_offset;
if( _temp3681 < _temp3679.base? 1: _temp3681 >= _temp3679.last_plus_one){ _throw(
Null_Exception);}* _temp3681;}).last_line); _temp3675->f2= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp3682= yyvs; struct _xenum_struct** _temp3684= _temp3682.curr
+ yyvsp_offset; if( _temp3684 < _temp3682.base? 1: _temp3684 >= _temp3682.last_plus_one){
_throw( Null_Exception);}* _temp3684;}));( void*) _temp3675;}); _temp3671->tl=(
Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3672= yyvs; struct
_xenum_struct** _temp3674= _temp3672.curr +( yyvsp_offset - 1); if( _temp3674 <
_temp3672.base? 1: _temp3674 >= _temp3672.last_plus_one){ _throw( Null_Exception);}*
_temp3674;})))->tms; _temp3671;}); _temp3670;});( struct _xenum_struct*)
_temp3669;}); break; case 127: _LL3668: yyval=({ struct Cyc_TypeModifierList_tok_struct*
_temp3689=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct)); _temp3689->tag= Cyc_TypeModifierList_tok_tag;
_temp3689->f1=({ struct Cyc_List_List* _temp3690=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3690->hd=( void*)({ struct Cyc_Absyn_Pointer_mod_struct*
_temp3691=( struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp3691->tag= Cyc_Absyn_Pointer_mod_tag; _temp3691->f1=( void*) Cyc_yyget_Pointer_Sort_tok(({
struct _tagged_ptr2 _temp3692= yyvs; struct _xenum_struct** _temp3694= _temp3692.curr
+( yyvsp_offset - 1); if( _temp3694 < _temp3692.base? 1: _temp3694 >= _temp3692.last_plus_one){
_throw( Null_Exception);}* _temp3694;})); _temp3691->f2=( void*) Cyc_yyget_Rgn_tok(({
struct _tagged_ptr2 _temp3695= yyvs; struct _xenum_struct** _temp3697= _temp3695.curr
+ yyvsp_offset; if( _temp3697 < _temp3695.base? 1: _temp3697 >= _temp3695.last_plus_one){
_throw( Null_Exception);}* _temp3697;})); _temp3691->f3= Cyc_Absyn_empty_tqual();(
void*) _temp3691;}); _temp3690->tl= 0; _temp3690;});( struct _xenum_struct*)
_temp3689;}); break; case 128: _LL3688: yyval=({ struct Cyc_TypeModifierList_tok_struct*
_temp3699=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct)); _temp3699->tag= Cyc_TypeModifierList_tok_tag;
_temp3699->f1=({ struct Cyc_List_List* _temp3700=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3700->hd=( void*)({ struct Cyc_Absyn_Pointer_mod_struct*
_temp3701=( struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp3701->tag= Cyc_Absyn_Pointer_mod_tag; _temp3701->f1=( void*) Cyc_yyget_Pointer_Sort_tok(({
struct _tagged_ptr2 _temp3702= yyvs; struct _xenum_struct** _temp3704= _temp3702.curr
+( yyvsp_offset - 2); if( _temp3704 < _temp3702.base? 1: _temp3704 >= _temp3702.last_plus_one){
_throw( Null_Exception);}* _temp3704;})); _temp3701->f2=( void*) Cyc_yyget_Rgn_tok(({
struct _tagged_ptr2 _temp3705= yyvs; struct _xenum_struct** _temp3707= _temp3705.curr
+( yyvsp_offset - 1); if( _temp3707 < _temp3705.base? 1: _temp3707 >= _temp3705.last_plus_one){
_throw( Null_Exception);}* _temp3707;})); _temp3701->f3= Cyc_yyget_TypeQual_tok(({
struct _tagged_ptr2 _temp3708= yyvs; struct _xenum_struct** _temp3710= _temp3708.curr
+ yyvsp_offset; if( _temp3710 < _temp3708.base? 1: _temp3710 >= _temp3708.last_plus_one){
_throw( Null_Exception);}* _temp3710;}));( void*) _temp3701;}); _temp3700->tl= 0;
_temp3700;});( struct _xenum_struct*) _temp3699;}); break; case 129: _LL3698:
yyval=({ struct Cyc_TypeModifierList_tok_struct* _temp3712=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp3712->tag= Cyc_TypeModifierList_tok_tag;
_temp3712->f1=({ struct Cyc_List_List* _temp3713=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3713->hd=( void*)({ struct Cyc_Absyn_Pointer_mod_struct*
_temp3717=( struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp3717->tag= Cyc_Absyn_Pointer_mod_tag; _temp3717->f1=( void*) Cyc_yyget_Pointer_Sort_tok(({
struct _tagged_ptr2 _temp3718= yyvs; struct _xenum_struct** _temp3720= _temp3718.curr
+( yyvsp_offset - 2); if( _temp3720 < _temp3718.base? 1: _temp3720 >= _temp3718.last_plus_one){
_throw( Null_Exception);}* _temp3720;})); _temp3717->f2=( void*) Cyc_yyget_Rgn_tok(({
struct _tagged_ptr2 _temp3721= yyvs; struct _xenum_struct** _temp3723= _temp3721.curr
+( yyvsp_offset - 1); if( _temp3723 < _temp3721.base? 1: _temp3723 >= _temp3721.last_plus_one){
_throw( Null_Exception);}* _temp3723;})); _temp3717->f3= Cyc_Absyn_empty_tqual();(
void*) _temp3717;}); _temp3713->tl= Cyc_yyget_TypeModifierList_tok(({ struct
_tagged_ptr2 _temp3714= yyvs; struct _xenum_struct** _temp3716= _temp3714.curr +
yyvsp_offset; if( _temp3716 < _temp3714.base? 1: _temp3716 >= _temp3714.last_plus_one){
_throw( Null_Exception);}* _temp3716;})); _temp3713;});( struct _xenum_struct*)
_temp3712;}); break; case 130: _LL3711: yyval=({ struct Cyc_TypeModifierList_tok_struct*
_temp3725=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct)); _temp3725->tag= Cyc_TypeModifierList_tok_tag;
_temp3725->f1=({ struct Cyc_List_List* _temp3726=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3726->hd=( void*)({ struct Cyc_Absyn_Pointer_mod_struct*
_temp3730=( struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp3730->tag= Cyc_Absyn_Pointer_mod_tag; _temp3730->f1=( void*) Cyc_yyget_Pointer_Sort_tok(({
struct _tagged_ptr2 _temp3731= yyvs; struct _xenum_struct** _temp3733= _temp3731.curr
+( yyvsp_offset - 3); if( _temp3733 < _temp3731.base? 1: _temp3733 >= _temp3731.last_plus_one){
_throw( Null_Exception);}* _temp3733;})); _temp3730->f2=( void*) Cyc_yyget_Rgn_tok(({
struct _tagged_ptr2 _temp3734= yyvs; struct _xenum_struct** _temp3736= _temp3734.curr
+( yyvsp_offset - 2); if( _temp3736 < _temp3734.base? 1: _temp3736 >= _temp3734.last_plus_one){
_throw( Null_Exception);}* _temp3736;})); _temp3730->f3= Cyc_yyget_TypeQual_tok(({
struct _tagged_ptr2 _temp3737= yyvs; struct _xenum_struct** _temp3739= _temp3737.curr
+( yyvsp_offset - 1); if( _temp3739 < _temp3737.base? 1: _temp3739 >= _temp3737.last_plus_one){
_throw( Null_Exception);}* _temp3739;}));( void*) _temp3730;}); _temp3726->tl=
Cyc_yyget_TypeModifierList_tok(({ struct _tagged_ptr2 _temp3727= yyvs; struct
_xenum_struct** _temp3729= _temp3727.curr + yyvsp_offset; if( _temp3729 <
_temp3727.base? 1: _temp3729 >= _temp3727.last_plus_one){ _throw( Null_Exception);}*
_temp3729;})); _temp3726;});( struct _xenum_struct*) _temp3725;}); break; case
131: _LL3724: yyval=({ struct Cyc_Pointer_Sort_tok_struct* _temp3741=( struct
Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp3741->tag= Cyc_Pointer_Sort_tok_tag; _temp3741->f1=( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp3742=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp3742->tag= Cyc_Absyn_Nullable_ps_tag; _temp3742->f1= Cyc_Absyn_signed_int_exp(
1, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3743= yyls; struct
Cyc_Yyltype* _temp3745= _temp3743.curr + yylsp_offset; if( _temp3745 < _temp3743.base?
1: _temp3745 >= _temp3743.last_plus_one){ _throw( Null_Exception);}* _temp3745;}).first_line,({
struct _tagged_ptr3 _temp3746= yyls; struct Cyc_Yyltype* _temp3748= _temp3746.curr
+ yylsp_offset; if( _temp3748 < _temp3746.base? 1: _temp3748 >= _temp3746.last_plus_one){
_throw( Null_Exception);}* _temp3748;}).last_line));( void*) _temp3742;});(
struct _xenum_struct*) _temp3741;}); break; case 132: _LL3740: yyval=({ struct
Cyc_Pointer_Sort_tok_struct* _temp3750=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp3750->tag= Cyc_Pointer_Sort_tok_tag;
_temp3750->f1=( void*)({ struct Cyc_Absyn_NonNullable_ps_struct* _temp3751=(
struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_NonNullable_ps_struct));
_temp3751->tag= Cyc_Absyn_NonNullable_ps_tag; _temp3751->f1= Cyc_Absyn_signed_int_exp(
1, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3752= yyls; struct
Cyc_Yyltype* _temp3754= _temp3752.curr + yylsp_offset; if( _temp3754 < _temp3752.base?
1: _temp3754 >= _temp3752.last_plus_one){ _throw( Null_Exception);}* _temp3754;}).first_line,({
struct _tagged_ptr3 _temp3755= yyls; struct Cyc_Yyltype* _temp3757= _temp3755.curr
+ yylsp_offset; if( _temp3757 < _temp3755.base? 1: _temp3757 >= _temp3755.last_plus_one){
_throw( Null_Exception);}* _temp3757;}).last_line));( void*) _temp3751;});(
struct _xenum_struct*) _temp3750;}); break; case 133: _LL3749: yyval=({ struct
Cyc_Pointer_Sort_tok_struct* _temp3759=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp3759->tag= Cyc_Pointer_Sort_tok_tag;
_temp3759->f1=( void*)({ struct Cyc_Absyn_Nullable_ps_struct* _temp3760=( struct
Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp3760->tag= Cyc_Absyn_Nullable_ps_tag; _temp3760->f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp3761= yyvs; struct _xenum_struct** _temp3763= _temp3761.curr
+( yyvsp_offset - 1); if( _temp3763 < _temp3761.base? 1: _temp3763 >= _temp3761.last_plus_one){
_throw( Null_Exception);}* _temp3763;}));( void*) _temp3760;});( struct
_xenum_struct*) _temp3759;}); break; case 134: _LL3758: yyval=({ struct Cyc_Pointer_Sort_tok_struct*
_temp3765=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp3765->tag= Cyc_Pointer_Sort_tok_tag; _temp3765->f1=( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp3766=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp3766->tag= Cyc_Absyn_NonNullable_ps_tag;
_temp3766->f1= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp3767= yyvs; struct
_xenum_struct** _temp3769= _temp3767.curr +( yyvsp_offset - 1); if( _temp3769 <
_temp3767.base? 1: _temp3769 >= _temp3767.last_plus_one){ _throw( Null_Exception);}*
_temp3769;}));( void*) _temp3766;});( struct _xenum_struct*) _temp3765;});
break; case 135: _LL3764: yyval=({ struct Cyc_Pointer_Sort_tok_struct* _temp3771=(
struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp3771->tag= Cyc_Pointer_Sort_tok_tag; _temp3771->f1=( void*) Cyc_Absyn_TaggedArray_ps;(
struct _xenum_struct*) _temp3771;}); break; case 136: _LL3770: yyval=({ struct
Cyc_Rgn_tok_struct* _temp3773=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof(
struct Cyc_Rgn_tok_struct)); _temp3773->tag= Cyc_Rgn_tok_tag; _temp3773->f1=(
void*) Cyc_Absyn_HeapRgn;( struct _xenum_struct*) _temp3773;}); break; case 137:
_LL3772: yyval=({ struct Cyc_Rgn_tok_struct* _temp3775=( struct Cyc_Rgn_tok_struct*)
GC_malloc( sizeof( struct Cyc_Rgn_tok_struct)); _temp3775->tag= Cyc_Rgn_tok_tag;
_temp3775->f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp3776= yyvs; struct _xenum_struct** _temp3778= _temp3776.curr +
yyvsp_offset; if( _temp3778 < _temp3776.base? 1: _temp3778 >= _temp3776.last_plus_one){
_throw( Null_Exception);}* _temp3778;})),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)( Cyc_Absyn_RgnKind));( struct _xenum_struct*) _temp3775;});
break; case 138: _LL3774: if( Cyc_yyget_Kind_tok(({ struct _tagged_ptr2
_temp3780= yyvs; struct _xenum_struct** _temp3782= _temp3780.curr + yyvsp_offset;
if( _temp3782 < _temp3780.base? 1: _temp3782 >= _temp3780.last_plus_one){ _throw(
Null_Exception);}* _temp3782;})) != Cyc_Absyn_RgnKind){ Cyc_Parse_err(( struct
_tagged_string)({ char* _temp3783=( char*)"expecting region kind\n"; struct
_tagged_string _temp3784; _temp3784.curr= _temp3783; _temp3784.base= _temp3783;
_temp3784.last_plus_one= _temp3783 + 23; _temp3784;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3785= yyls; struct Cyc_Yyltype* _temp3787= _temp3785.curr
+ yylsp_offset; if( _temp3787 < _temp3785.base? 1: _temp3787 >= _temp3785.last_plus_one){
_throw( Null_Exception);}* _temp3787;}).first_line,({ struct _tagged_ptr3
_temp3788= yyls; struct Cyc_Yyltype* _temp3790= _temp3788.curr + yylsp_offset;
if( _temp3790 < _temp3788.base? 1: _temp3790 >= _temp3788.last_plus_one){ _throw(
Null_Exception);}* _temp3790;}).last_line));} yyval=({ struct Cyc_Rgn_tok_struct*
_temp3791=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));
_temp3791->tag= Cyc_Rgn_tok_tag; _temp3791->f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp3792= yyvs; struct _xenum_struct** _temp3794= _temp3792.curr
+( yyvsp_offset - 2); if( _temp3794 < _temp3792.base? 1: _temp3794 >= _temp3792.last_plus_one){
_throw( Null_Exception);}* _temp3794;})),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)( Cyc_Absyn_RgnKind));( struct _xenum_struct*) _temp3791;});
break; case 139: _LL3779: yyval=({ struct Cyc_Rgn_tok_struct* _temp3796=( struct
Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct)); _temp3796->tag=
Cyc_Rgn_tok_tag; _temp3796->f1=( void*) Cyc_Absyn_new_evar( Cyc_Absyn_RgnKind);(
struct _xenum_struct*) _temp3796;}); break; case 140: _LL3795: yyval=({ struct
_tagged_ptr2 _temp3798= yyvs; struct _xenum_struct** _temp3800= _temp3798.curr +
yyvsp_offset; if( _temp3800 < _temp3798.base? 1: _temp3800 >= _temp3798.last_plus_one){
_throw( Null_Exception);}* _temp3800;}); break; case 141: _LL3797: yyval=({
struct Cyc_TypeQual_tok_struct* _temp3802=( struct Cyc_TypeQual_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct)); _temp3802->tag= Cyc_TypeQual_tok_tag;
_temp3802->f1= Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok(({ struct
_tagged_ptr2 _temp3803= yyvs; struct _xenum_struct** _temp3805= _temp3803.curr +(
yyvsp_offset - 1); if( _temp3805 < _temp3803.base? 1: _temp3805 >= _temp3803.last_plus_one){
_throw( Null_Exception);}* _temp3805;})), Cyc_yyget_TypeQual_tok(({ struct
_tagged_ptr2 _temp3806= yyvs; struct _xenum_struct** _temp3808= _temp3806.curr +
yyvsp_offset; if( _temp3808 < _temp3806.base? 1: _temp3808 >= _temp3806.last_plus_one){
_throw( Null_Exception);}* _temp3808;})));( struct _xenum_struct*) _temp3802;});
break; case 142: _LL3801: yyval=({ struct Cyc_ParamDeclListBool_tok_struct*
_temp3810=( struct Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct
Cyc_ParamDeclListBool_tok_struct)); _temp3810->tag= Cyc_ParamDeclListBool_tok_tag;
_temp3810->f1=({ struct _tuple16* _temp3811=( struct _tuple16*) GC_malloc(
sizeof( struct _tuple16)); _temp3811->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({ struct _tagged_ptr2
_temp3812= yyvs; struct _xenum_struct** _temp3814= _temp3812.curr + yyvsp_offset;
if( _temp3814 < _temp3812.base? 1: _temp3814 >= _temp3812.last_plus_one){ _throw(
Null_Exception);}* _temp3814;}))); _temp3811->f2= 0; _temp3811->f3= 0; _temp3811;});(
struct _xenum_struct*) _temp3810;}); break; case 143: _LL3809: yyval=({ struct
Cyc_ParamDeclListBool_tok_struct* _temp3816=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct)); _temp3816->tag=
Cyc_ParamDeclListBool_tok_tag; _temp3816->f1=({ struct _tuple16* _temp3817=(
struct _tuple16*) GC_malloc( sizeof( struct _tuple16)); _temp3817->f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp3818= yyvs; struct _xenum_struct** _temp3820= _temp3818.curr
+( yyvsp_offset - 2); if( _temp3820 < _temp3818.base? 1: _temp3820 >= _temp3818.last_plus_one){
_throw( Null_Exception);}* _temp3820;}))); _temp3817->f2= 1; _temp3817->f3= 0;
_temp3817;});( struct _xenum_struct*) _temp3816;}); break; case 144: _LL3815:
yyval=({ struct Cyc_ParamDeclListBool_tok_struct* _temp3822=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct)); _temp3822->tag=
Cyc_ParamDeclListBool_tok_tag; _temp3822->f1=({ struct _tuple16* _temp3823=(
struct _tuple16*) GC_malloc( sizeof( struct _tuple16)); _temp3823->f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp3829= yyvs; struct _xenum_struct** _temp3831= _temp3829.curr
+( yyvsp_offset - 2); if( _temp3831 < _temp3829.base? 1: _temp3831 >= _temp3829.last_plus_one){
_throw( Null_Exception);}* _temp3831;}))); _temp3823->f2= 0; _temp3823->f3=(
struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt* _temp3824=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3824->v=( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp3825=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3825->tag= Cyc_Absyn_JoinEff_tag; _temp3825->f1= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3826= yyvs; struct _xenum_struct** _temp3828= _temp3826.curr
+ yyvsp_offset; if( _temp3828 < _temp3826.base? 1: _temp3828 >= _temp3826.last_plus_one){
_throw( Null_Exception);}* _temp3828;}));( void*) _temp3825;}); _temp3824;});
_temp3823;});( struct _xenum_struct*) _temp3822;}); break; case 145: _LL3821:
yyval=({ struct _tagged_ptr2 _temp3833= yyvs; struct _xenum_struct** _temp3835=
_temp3833.curr + yyvsp_offset; if( _temp3835 < _temp3833.base? 1: _temp3835 >=
_temp3833.last_plus_one){ _throw( Null_Exception);}* _temp3835;}); break; case
146: _LL3832: yyval=({ struct Cyc_TypeList_tok_struct* _temp3837=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3837->tag= Cyc_TypeList_tok_tag;
_temp3837->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2
_temp3838= yyvs; struct _xenum_struct** _temp3840= _temp3838.curr +(
yyvsp_offset - 2); if( _temp3840 < _temp3838.base? 1: _temp3840 >= _temp3838.last_plus_one){
_throw( Null_Exception);}* _temp3840;})), Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp3841= yyvs; struct _xenum_struct** _temp3843= _temp3841.curr +
yyvsp_offset; if( _temp3843 < _temp3841.base? 1: _temp3843 >= _temp3841.last_plus_one){
_throw( Null_Exception);}* _temp3843;})));( struct _xenum_struct*) _temp3837;});
break; case 147: _LL3836: yyval=({ struct Cyc_TypeList_tok_struct* _temp3845=(
struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3845->tag= Cyc_TypeList_tok_tag; _temp3845->f1= 0;( struct _xenum_struct*)
_temp3845;}); break; case 148: _LL3844: yyval=({ struct _tagged_ptr2 _temp3847=
yyvs; struct _xenum_struct** _temp3849= _temp3847.curr +( yyvsp_offset - 1); if(
_temp3849 < _temp3847.base? 1: _temp3849 >= _temp3847.last_plus_one){ _throw(
Null_Exception);}* _temp3849;}); break; case 149: _LL3846: yyval=({ struct Cyc_TypeList_tok_struct*
_temp3851=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3851->tag= Cyc_TypeList_tok_tag; _temp3851->f1=({ struct Cyc_List_List*
_temp3852=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3852->hd=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp3853= yyvs; struct _xenum_struct** _temp3855= _temp3853.curr +
yyvsp_offset; if( _temp3855 < _temp3853.base? 1: _temp3855 >= _temp3853.last_plus_one){
_throw( Null_Exception);}* _temp3855;})),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)( Cyc_Absyn_EffKind)); _temp3852->tl= 0; _temp3852;});(
struct _xenum_struct*) _temp3851;}); break; case 150: _LL3850: if( Cyc_yyget_Kind_tok(({
struct _tagged_ptr2 _temp3857= yyvs; struct _xenum_struct** _temp3859= _temp3857.curr
+ yyvsp_offset; if( _temp3859 < _temp3857.base? 1: _temp3859 >= _temp3857.last_plus_one){
_throw( Null_Exception);}* _temp3859;})) != Cyc_Absyn_EffKind){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp3860=( char*)"expecing effect kind (E)";
struct _tagged_string _temp3861; _temp3861.curr= _temp3860; _temp3861.base=
_temp3860; _temp3861.last_plus_one= _temp3860 + 25; _temp3861;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3862= yyls; struct Cyc_Yyltype* _temp3864= _temp3862.curr
+ yylsp_offset; if( _temp3864 < _temp3862.base? 1: _temp3864 >= _temp3862.last_plus_one){
_throw( Null_Exception);}* _temp3864;}).first_line,({ struct _tagged_ptr3
_temp3865= yyls; struct Cyc_Yyltype* _temp3867= _temp3865.curr + yylsp_offset;
if( _temp3867 < _temp3865.base? 1: _temp3867 >= _temp3865.last_plus_one){ _throw(
Null_Exception);}* _temp3867;}).last_line));} yyval=({ struct Cyc_TypeList_tok_struct*
_temp3868=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3868->tag= Cyc_TypeList_tok_tag; _temp3868->f1=({ struct Cyc_List_List*
_temp3869=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3869->hd=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp3870= yyvs; struct _xenum_struct** _temp3872= _temp3870.curr +(
yyvsp_offset - 2); if( _temp3872 < _temp3870.base? 1: _temp3872 >= _temp3870.last_plus_one){
_throw( Null_Exception);}* _temp3872;})),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)( Cyc_Absyn_EffKind)); _temp3869->tl= 0; _temp3869;});(
struct _xenum_struct*) _temp3868;}); break; case 151: _LL3856: yyval=({ struct
Cyc_TypeList_tok_struct* _temp3874=( struct Cyc_TypeList_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeList_tok_struct)); _temp3874->tag= Cyc_TypeList_tok_tag;
_temp3874->f1=({ struct Cyc_List_List* _temp3875=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3875->hd=( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp3876=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3876->tag= Cyc_Absyn_AccessEff_tag; _temp3876->f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp3877= yyvs; struct
_xenum_struct** _temp3879= _temp3877.curr + yyvsp_offset; if( _temp3879 <
_temp3877.base? 1: _temp3879 >= _temp3877.last_plus_one){ _throw( Null_Exception);}*
_temp3879;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
Cyc_Absyn_RgnKind));( void*) _temp3876;}); _temp3875->tl= 0; _temp3875;});(
struct _xenum_struct*) _temp3874;}); break; case 152: _LL3873: yyval=({ struct
Cyc_TypeList_tok_struct* _temp3881=( struct Cyc_TypeList_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeList_tok_struct)); _temp3881->tag= Cyc_TypeList_tok_tag;
_temp3881->f1=({ struct Cyc_List_List* _temp3882=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3882->hd=( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp3886=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3886->tag= Cyc_Absyn_AccessEff_tag; _temp3886->f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp3887= yyvs; struct
_xenum_struct** _temp3889= _temp3887.curr +( yyvsp_offset - 2); if( _temp3889 <
_temp3887.base? 1: _temp3889 >= _temp3887.last_plus_one){ _throw( Null_Exception);}*
_temp3889;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
Cyc_Absyn_RgnKind));( void*) _temp3886;}); _temp3882->tl= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3883= yyvs; struct _xenum_struct** _temp3885= _temp3883.curr
+ yyvsp_offset; if( _temp3885 < _temp3883.base? 1: _temp3885 >= _temp3883.last_plus_one){
_throw( Null_Exception);}* _temp3885;})); _temp3882;});( struct _xenum_struct*)
_temp3881;}); break; case 153: _LL3880: if( Cyc_yyget_Kind_tok(({ struct
_tagged_ptr2 _temp3891= yyvs; struct _xenum_struct** _temp3893= _temp3891.curr +
yyvsp_offset; if( _temp3893 < _temp3891.base? 1: _temp3893 >= _temp3891.last_plus_one){
_throw( Null_Exception);}* _temp3893;})) != Cyc_Absyn_RgnKind){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp3894=( char*)"expecting region kind (R)";
struct _tagged_string _temp3895; _temp3895.curr= _temp3894; _temp3895.base=
_temp3894; _temp3895.last_plus_one= _temp3894 + 26; _temp3895;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3896= yyls; struct Cyc_Yyltype* _temp3898= _temp3896.curr
+ yylsp_offset; if( _temp3898 < _temp3896.base? 1: _temp3898 >= _temp3896.last_plus_one){
_throw( Null_Exception);}* _temp3898;}).first_line,({ struct _tagged_ptr3
_temp3899= yyls; struct Cyc_Yyltype* _temp3901= _temp3899.curr + yylsp_offset;
if( _temp3901 < _temp3899.base? 1: _temp3901 >= _temp3899.last_plus_one){ _throw(
Null_Exception);}* _temp3901;}).last_line));} yyval=({ struct Cyc_TypeList_tok_struct*
_temp3902=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3902->tag= Cyc_TypeList_tok_tag; _temp3902->f1=({ struct Cyc_List_List*
_temp3903=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3903->hd=( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3904=( struct
Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3904->tag= Cyc_Absyn_AccessEff_tag; _temp3904->f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp3905= yyvs; struct
_xenum_struct** _temp3907= _temp3905.curr +( yyvsp_offset - 2); if( _temp3907 <
_temp3905.base? 1: _temp3907 >= _temp3905.last_plus_one){ _throw( Null_Exception);}*
_temp3907;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
Cyc_Absyn_RgnKind));( void*) _temp3904;}); _temp3903->tl= 0; _temp3903;});(
struct _xenum_struct*) _temp3902;}); break; case 154: _LL3890: if( Cyc_yyget_Kind_tok(({
struct _tagged_ptr2 _temp3909= yyvs; struct _xenum_struct** _temp3911= _temp3909.curr
+( yyvsp_offset - 2); if( _temp3911 < _temp3909.base? 1: _temp3911 >= _temp3909.last_plus_one){
_throw( Null_Exception);}* _temp3911;})) != Cyc_Absyn_RgnKind){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp3912=( char*)"expecting region kind (R)";
struct _tagged_string _temp3913; _temp3913.curr= _temp3912; _temp3913.base=
_temp3912; _temp3913.last_plus_one= _temp3912 + 26; _temp3913;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3914= yyls; struct Cyc_Yyltype* _temp3916= _temp3914.curr
+( yylsp_offset - 2); if( _temp3916 < _temp3914.base? 1: _temp3916 >= _temp3914.last_plus_one){
_throw( Null_Exception);}* _temp3916;}).first_line,({ struct _tagged_ptr3
_temp3917= yyls; struct Cyc_Yyltype* _temp3919= _temp3917.curr +( yylsp_offset -
2); if( _temp3919 < _temp3917.base? 1: _temp3919 >= _temp3917.last_plus_one){
_throw( Null_Exception);}* _temp3919;}).last_line));} yyval=({ struct Cyc_TypeList_tok_struct*
_temp3920=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3920->tag= Cyc_TypeList_tok_tag; _temp3920->f1=({ struct Cyc_List_List*
_temp3921=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3921->hd=( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3925=( struct
Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3925->tag= Cyc_Absyn_AccessEff_tag; _temp3925->f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp3926= yyvs; struct
_xenum_struct** _temp3928= _temp3926.curr +( yyvsp_offset - 4); if( _temp3928 <
_temp3926.base? 1: _temp3928 >= _temp3926.last_plus_one){ _throw( Null_Exception);}*
_temp3928;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
Cyc_Absyn_RgnKind));( void*) _temp3925;}); _temp3921->tl= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3922= yyvs; struct _xenum_struct** _temp3924= _temp3922.curr
+ yyvsp_offset; if( _temp3924 < _temp3922.base? 1: _temp3924 >= _temp3922.last_plus_one){
_throw( Null_Exception);}* _temp3924;})); _temp3921;});( struct _xenum_struct*)
_temp3920;}); break; case 155: _LL3908: yyval=({ struct Cyc_ParamDeclList_tok_struct*
_temp3930=( struct Cyc_ParamDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct));
_temp3930->tag= Cyc_ParamDeclList_tok_tag; _temp3930->f1=({ struct Cyc_List_List*
_temp3931=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3931->hd=( void*) Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp3932=
yyvs; struct _xenum_struct** _temp3934= _temp3932.curr + yyvsp_offset; if(
_temp3934 < _temp3932.base? 1: _temp3934 >= _temp3932.last_plus_one){ _throw(
Null_Exception);}* _temp3934;})); _temp3931->tl= 0; _temp3931;});( struct
_xenum_struct*) _temp3930;}); break; case 156: _LL3929: yyval=({ struct Cyc_ParamDeclList_tok_struct*
_temp3936=( struct Cyc_ParamDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct));
_temp3936->tag= Cyc_ParamDeclList_tok_tag; _temp3936->f1=({ struct Cyc_List_List*
_temp3937=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3937->hd=( void*) Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp3941=
yyvs; struct _xenum_struct** _temp3943= _temp3941.curr + yyvsp_offset; if(
_temp3943 < _temp3941.base? 1: _temp3943 >= _temp3941.last_plus_one){ _throw(
Null_Exception);}* _temp3943;})); _temp3937->tl= Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp3938= yyvs; struct _xenum_struct** _temp3940= _temp3938.curr
+( yyvsp_offset - 2); if( _temp3940 < _temp3938.base? 1: _temp3940 >= _temp3938.last_plus_one){
_throw( Null_Exception);}* _temp3940;})); _temp3937;});( struct _xenum_struct*)
_temp3936;}); break; case 157: _LL3935: { void* t= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3945= yyvs; struct _xenum_struct** _temp3947= _temp3945.curr
+( yyvsp_offset - 1); if( _temp3947 < _temp3945.base? 1: _temp3947 >= _temp3945.last_plus_one){
_throw( Null_Exception);}* _temp3947;}))).f2, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3948= yyls; struct Cyc_Yyltype* _temp3950= _temp3948.curr
+( yylsp_offset - 1); if( _temp3950 < _temp3948.base? 1: _temp3950 >= _temp3948.last_plus_one){
_throw( Null_Exception);}* _temp3950;}).first_line,({ struct _tagged_ptr3
_temp3951= yyls; struct Cyc_Yyltype* _temp3953= _temp3951.curr +( yylsp_offset -
1); if( _temp3953 < _temp3951.base? 1: _temp3953 >= _temp3951.last_plus_one){
_throw( Null_Exception);}* _temp3953;}).last_line)); struct Cyc_List_List* atts=(*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp3954= yyvs; struct
_xenum_struct** _temp3956= _temp3954.curr +( yyvsp_offset - 1); if( _temp3956 <
_temp3954.base? 1: _temp3956 >= _temp3954.last_plus_one){ _throw( Null_Exception);}*
_temp3956;}))).f3; struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3957= yyvs; struct _xenum_struct** _temp3959= _temp3957.curr
+( yyvsp_offset - 1); if( _temp3959 < _temp3957.base? 1: _temp3959 >= _temp3957.last_plus_one){
_throw( Null_Exception);}* _temp3959;}))).f1; struct Cyc_List_List* tms=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3960= yyvs; struct _xenum_struct** _temp3962= _temp3960.curr
+ yyvsp_offset; if( _temp3962 < _temp3960.base? 1: _temp3962 >= _temp3960.last_plus_one){
_throw( Null_Exception);}* _temp3962;})))->tms; struct _tuple6 t_info= Cyc_Parse_apply_tms(
tq, t, atts, tms); if( t_info.f3 != 0){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp3963=( char*)"parameter with bad type params"; struct _tagged_string
_temp3964; _temp3964.curr= _temp3963; _temp3964.base= _temp3963; _temp3964.last_plus_one=
_temp3963 + 31; _temp3964;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp3965= yyls; struct Cyc_Yyltype* _temp3967= _temp3965.curr + yylsp_offset;
if( _temp3967 < _temp3965.base? 1: _temp3967 >= _temp3965.last_plus_one){ _throw(
Null_Exception);}* _temp3967;}).first_line,({ struct _tagged_ptr3 _temp3968=
yyls; struct Cyc_Yyltype* _temp3970= _temp3968.curr + yylsp_offset; if(
_temp3970 < _temp3968.base? 1: _temp3970 >= _temp3968.last_plus_one){ _throw(
Null_Exception);}* _temp3970;}).last_line));}{ struct _tuple1* q=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3971= yyvs; struct _xenum_struct** _temp3973= _temp3971.curr
+ yyvsp_offset; if( _temp3973 < _temp3971.base? 1: _temp3973 >= _temp3971.last_plus_one){
_throw( Null_Exception);}* _temp3973;})))->id;{ void* _temp3974=(* q).f1; struct
Cyc_List_List* _temp3984; struct Cyc_List_List* _temp3986; _LL3976: if(
_temp3974 == Cyc_Absyn_Loc_n){ goto _LL3977;} else{ goto _LL3978;} _LL3978: if((
unsigned int) _temp3974 > 1u?(( struct _enum_struct*) _temp3974)->tag == Cyc_Absyn_Rel_n_tag:
0){ _LL3985: _temp3984=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp3974)->f1; if( _temp3984 == 0){ goto _LL3979;} else{ goto _LL3980;}} else{
goto _LL3980;} _LL3980: if(( unsigned int) _temp3974 > 1u?(( struct _enum_struct*)
_temp3974)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL3987: _temp3986=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp3974)->f1; if( _temp3986 == 0){ goto
_LL3981;} else{ goto _LL3982;}} else{ goto _LL3982;} _LL3982: goto _LL3983;
_LL3977: goto _LL3975; _LL3979: goto _LL3975; _LL3981: goto _LL3975; _LL3983:
Cyc_Parse_err(( struct _tagged_string)({ char* _temp3988=( char*)"parameter cannot be qualified with a module name";
struct _tagged_string _temp3989; _temp3989.curr= _temp3988; _temp3989.base=
_temp3988; _temp3989.last_plus_one= _temp3988 + 49; _temp3989;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3990= yyls; struct Cyc_Yyltype* _temp3992= _temp3990.curr
+( yylsp_offset - 1); if( _temp3992 < _temp3990.base? 1: _temp3992 >= _temp3990.last_plus_one){
_throw( Null_Exception);}* _temp3992;}).first_line,({ struct _tagged_ptr3
_temp3993= yyls; struct Cyc_Yyltype* _temp3995= _temp3993.curr +( yylsp_offset -
1); if( _temp3995 < _temp3993.base? 1: _temp3995 >= _temp3993.last_plus_one){
_throw( Null_Exception);}* _temp3995;}).last_line)); goto _LL3975; _LL3975:;}{
struct Cyc_Core_Opt* idopt=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt*
_temp3996=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3996->v=( void*)(* q).f2; _temp3996;}); if( t_info.f4 != 0){ Cyc_Parse_warn((
struct _tagged_string)({ char* _temp3997=( char*)"extra attributes on parameter, ignoring";
struct _tagged_string _temp3998; _temp3998.curr= _temp3997; _temp3998.base=
_temp3997; _temp3998.last_plus_one= _temp3997 + 40; _temp3998;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3999= yyls; struct Cyc_Yyltype* _temp4001= _temp3999.curr
+( yylsp_offset - 1); if( _temp4001 < _temp3999.base? 1: _temp4001 >= _temp3999.last_plus_one){
_throw( Null_Exception);}* _temp4001;}).first_line,({ struct _tagged_ptr3
_temp4002= yyls; struct Cyc_Yyltype* _temp4004= _temp4002.curr + yylsp_offset;
if( _temp4004 < _temp4002.base? 1: _temp4004 >= _temp4002.last_plus_one){ _throw(
Null_Exception);}* _temp4004;}).last_line));} if( t_info.f3 != 0){ Cyc_Parse_warn((
struct _tagged_string)({ char* _temp4005=( char*)"extra type variables on parameter, ignoring";
struct _tagged_string _temp4006; _temp4006.curr= _temp4005; _temp4006.base=
_temp4005; _temp4006.last_plus_one= _temp4005 + 44; _temp4006;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4007= yyls; struct Cyc_Yyltype* _temp4009= _temp4007.curr
+( yylsp_offset - 1); if( _temp4009 < _temp4007.base? 1: _temp4009 >= _temp4007.last_plus_one){
_throw( Null_Exception);}* _temp4009;}).first_line,({ struct _tagged_ptr3
_temp4010= yyls; struct Cyc_Yyltype* _temp4012= _temp4010.curr + yylsp_offset;
if( _temp4012 < _temp4010.base? 1: _temp4012 >= _temp4010.last_plus_one){ _throw(
Null_Exception);}* _temp4012;}).last_line));} yyval=({ struct Cyc_ParamDecl_tok_struct*
_temp4013=( struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp4013->tag= Cyc_ParamDecl_tok_tag; _temp4013->f1=({ struct _tuple2*
_temp4014=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2)); _temp4014->f1=
idopt; _temp4014->f2= t_info.f1; _temp4014->f3= t_info.f2; _temp4014;});( struct
_xenum_struct*) _temp4013;}); break;}}} case 158: _LL3944: { void* t= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp4016= yyvs; struct
_xenum_struct** _temp4018= _temp4016.curr + yyvsp_offset; if( _temp4018 <
_temp4016.base? 1: _temp4018 >= _temp4016.last_plus_one){ _throw( Null_Exception);}*
_temp4018;}))).f2, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4019=
yyls; struct Cyc_Yyltype* _temp4021= _temp4019.curr + yylsp_offset; if(
_temp4021 < _temp4019.base? 1: _temp4021 >= _temp4019.last_plus_one){ _throw(
Null_Exception);}* _temp4021;}).first_line,({ struct _tagged_ptr3 _temp4022=
yyls; struct Cyc_Yyltype* _temp4024= _temp4022.curr + yylsp_offset; if(
_temp4024 < _temp4022.base? 1: _temp4024 >= _temp4022.last_plus_one){ _throw(
Null_Exception);}* _temp4024;}).last_line)); struct Cyc_List_List* atts=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4025= yyvs; struct _xenum_struct** _temp4027= _temp4025.curr
+ yyvsp_offset; if( _temp4027 < _temp4025.base? 1: _temp4027 >= _temp4025.last_plus_one){
_throw( Null_Exception);}* _temp4027;}))).f3; struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4028= yyvs; struct _xenum_struct** _temp4030= _temp4028.curr
+ yyvsp_offset; if( _temp4030 < _temp4028.base? 1: _temp4030 >= _temp4028.last_plus_one){
_throw( Null_Exception);}* _temp4030;}))).f1; if( atts != 0){ Cyc_Parse_warn((
struct _tagged_string)({ char* _temp4031=( char*)"bad attributes on parameter, ignoring";
struct _tagged_string _temp4032; _temp4032.curr= _temp4031; _temp4032.base=
_temp4031; _temp4032.last_plus_one= _temp4031 + 38; _temp4032;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4033= yyls; struct Cyc_Yyltype* _temp4035= _temp4033.curr
+ yylsp_offset; if( _temp4035 < _temp4033.base? 1: _temp4035 >= _temp4033.last_plus_one){
_throw( Null_Exception);}* _temp4035;}).first_line,({ struct _tagged_ptr3
_temp4036= yyls; struct Cyc_Yyltype* _temp4038= _temp4036.curr + yylsp_offset;
if( _temp4038 < _temp4036.base? 1: _temp4038 >= _temp4036.last_plus_one){ _throw(
Null_Exception);}* _temp4038;}).last_line));} yyval=({ struct Cyc_ParamDecl_tok_struct*
_temp4039=( struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp4039->tag= Cyc_ParamDecl_tok_tag; _temp4039->f1=({ struct _tuple2*
_temp4040=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2)); _temp4040->f1=
0; _temp4040->f2= tq; _temp4040->f3= t; _temp4040;});( struct _xenum_struct*)
_temp4039;}); break;} case 159: _LL4015: { void* t= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp4042= yyvs; struct
_xenum_struct** _temp4044= _temp4042.curr +( yyvsp_offset - 1); if( _temp4044 <
_temp4042.base? 1: _temp4044 >= _temp4042.last_plus_one){ _throw( Null_Exception);}*
_temp4044;}))).f2, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4045=
yyls; struct Cyc_Yyltype* _temp4047= _temp4045.curr +( yylsp_offset - 1); if(
_temp4047 < _temp4045.base? 1: _temp4047 >= _temp4045.last_plus_one){ _throw(
Null_Exception);}* _temp4047;}).first_line,({ struct _tagged_ptr3 _temp4048=
yyls; struct Cyc_Yyltype* _temp4050= _temp4048.curr +( yylsp_offset - 1); if(
_temp4050 < _temp4048.base? 1: _temp4050 >= _temp4048.last_plus_one){ _throw(
Null_Exception);}* _temp4050;}).last_line)); struct Cyc_List_List* atts=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4051= yyvs; struct _xenum_struct** _temp4053= _temp4051.curr
+( yyvsp_offset - 1); if( _temp4053 < _temp4051.base? 1: _temp4053 >= _temp4051.last_plus_one){
_throw( Null_Exception);}* _temp4053;}))).f3; struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4054= yyvs; struct _xenum_struct** _temp4056= _temp4054.curr
+( yyvsp_offset - 1); if( _temp4056 < _temp4054.base? 1: _temp4056 >= _temp4054.last_plus_one){
_throw( Null_Exception);}* _temp4056;}))).f1; struct Cyc_List_List* tms=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp4057= yyvs; struct _xenum_struct** _temp4059= _temp4057.curr
+ yyvsp_offset; if( _temp4059 < _temp4057.base? 1: _temp4059 >= _temp4057.last_plus_one){
_throw( Null_Exception);}* _temp4059;})))->tms; struct _tuple6 t_info= Cyc_Parse_apply_tms(
tq, t, atts, tms); if( t_info.f3 != 0){ Cyc_Parse_warn(( struct _tagged_string)({
char* _temp4060=( char*)"bad type parameters on formal argument, ignoring";
struct _tagged_string _temp4061; _temp4061.curr= _temp4060; _temp4061.base=
_temp4060; _temp4061.last_plus_one= _temp4060 + 49; _temp4061;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4062= yyls; struct Cyc_Yyltype* _temp4064= _temp4062.curr
+( yylsp_offset - 1); if( _temp4064 < _temp4062.base? 1: _temp4064 >= _temp4062.last_plus_one){
_throw( Null_Exception);}* _temp4064;}).first_line,({ struct _tagged_ptr3
_temp4065= yyls; struct Cyc_Yyltype* _temp4067= _temp4065.curr + yylsp_offset;
if( _temp4067 < _temp4065.base? 1: _temp4067 >= _temp4065.last_plus_one){ _throw(
Null_Exception);}* _temp4067;}).last_line));} if( t_info.f4 != 0){ Cyc_Parse_warn((
struct _tagged_string)({ char* _temp4068=( char*)"bad attributes on parameter, ignoring";
struct _tagged_string _temp4069; _temp4069.curr= _temp4068; _temp4069.base=
_temp4068; _temp4069.last_plus_one= _temp4068 + 38; _temp4069;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4070= yyls; struct Cyc_Yyltype* _temp4072= _temp4070.curr
+( yylsp_offset - 1); if( _temp4072 < _temp4070.base? 1: _temp4072 >= _temp4070.last_plus_one){
_throw( Null_Exception);}* _temp4072;}).first_line,({ struct _tagged_ptr3
_temp4073= yyls; struct Cyc_Yyltype* _temp4075= _temp4073.curr + yylsp_offset;
if( _temp4075 < _temp4073.base? 1: _temp4075 >= _temp4073.last_plus_one){ _throw(
Null_Exception);}* _temp4075;}).last_line));} yyval=({ struct Cyc_ParamDecl_tok_struct*
_temp4076=( struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp4076->tag= Cyc_ParamDecl_tok_tag; _temp4076->f1=({ struct _tuple2*
_temp4077=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2)); _temp4077->f1=
0; _temp4077->f2= t_info.f1; _temp4077->f3= t_info.f2; _temp4077;});( struct
_xenum_struct*) _temp4076;}); break;} case 160: _LL4041: yyval=({ struct Cyc_IdList_tok_struct*
_temp4079=( struct Cyc_IdList_tok_struct*) GC_malloc( sizeof( struct Cyc_IdList_tok_struct));
_temp4079->tag= Cyc_IdList_tok_tag; _temp4079->f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_IdList_tok(({ struct
_tagged_ptr2 _temp4080= yyvs; struct _xenum_struct** _temp4082= _temp4080.curr +
yyvsp_offset; if( _temp4082 < _temp4080.base? 1: _temp4082 >= _temp4080.last_plus_one){
_throw( Null_Exception);}* _temp4082;})));( struct _xenum_struct*) _temp4079;});
break; case 161: _LL4078: yyval=({ struct Cyc_IdList_tok_struct* _temp4084=(
struct Cyc_IdList_tok_struct*) GC_malloc( sizeof( struct Cyc_IdList_tok_struct));
_temp4084->tag= Cyc_IdList_tok_tag; _temp4084->f1=({ struct Cyc_List_List*
_temp4085=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4085->hd=( void*)({ struct _tagged_string* _temp4086=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp4086[ 0]=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp4087= yyvs; struct
_xenum_struct** _temp4089= _temp4087.curr + yyvsp_offset; if( _temp4089 <
_temp4087.base? 1: _temp4089 >= _temp4087.last_plus_one){ _throw( Null_Exception);}*
_temp4089;})); _temp4086;}); _temp4085->tl= 0; _temp4085;});( struct
_xenum_struct*) _temp4084;}); break; case 162: _LL4083: yyval=({ struct Cyc_IdList_tok_struct*
_temp4091=( struct Cyc_IdList_tok_struct*) GC_malloc( sizeof( struct Cyc_IdList_tok_struct));
_temp4091->tag= Cyc_IdList_tok_tag; _temp4091->f1=({ struct Cyc_List_List*
_temp4092=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4092->hd=( void*)({ struct _tagged_string* _temp4096=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp4096[ 0]=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp4097= yyvs; struct
_xenum_struct** _temp4099= _temp4097.curr + yyvsp_offset; if( _temp4099 <
_temp4097.base? 1: _temp4099 >= _temp4097.last_plus_one){ _throw( Null_Exception);}*
_temp4099;})); _temp4096;}); _temp4092->tl= Cyc_yyget_IdList_tok(({ struct
_tagged_ptr2 _temp4093= yyvs; struct _xenum_struct** _temp4095= _temp4093.curr +(
yyvsp_offset - 2); if( _temp4095 < _temp4093.base? 1: _temp4095 >= _temp4093.last_plus_one){
_throw( Null_Exception);}* _temp4095;})); _temp4092;});( struct _xenum_struct*)
_temp4091;}); break; case 163: _LL4090: yyval=({ struct _tagged_ptr2 _temp4101=
yyvs; struct _xenum_struct** _temp4103= _temp4101.curr + yyvsp_offset; if(
_temp4103 < _temp4101.base? 1: _temp4103 >= _temp4101.last_plus_one){ _throw(
Null_Exception);}* _temp4103;}); break; case 164: _LL4100: yyval=({ struct Cyc_Exp_tok_struct*
_temp4105=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4105->tag= Cyc_Exp_tok_tag; _temp4105->f1= Cyc_Absyn_new_exp(({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp4106=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp4106->tag= Cyc_Absyn_UnresolvedMem_e_tag;
_temp4106->f1= 0; _temp4106->f2= 0;( void*) _temp4106;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4107= yyls; struct Cyc_Yyltype* _temp4109= _temp4107.curr
+( yylsp_offset - 1); if( _temp4109 < _temp4107.base? 1: _temp4109 >= _temp4107.last_plus_one){
_throw( Null_Exception);}* _temp4109;}).first_line,({ struct _tagged_ptr3
_temp4110= yyls; struct Cyc_Yyltype* _temp4112= _temp4110.curr + yylsp_offset;
if( _temp4112 < _temp4110.base? 1: _temp4112 >= _temp4110.last_plus_one){ _throw(
Null_Exception);}* _temp4112;}).last_line));( struct _xenum_struct*) _temp4105;});
break; case 165: _LL4104: yyval=({ struct Cyc_Exp_tok_struct* _temp4114=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4114->tag=
Cyc_Exp_tok_tag; _temp4114->f1= Cyc_Absyn_new_exp(({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp4115=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp4115->tag= Cyc_Absyn_UnresolvedMem_e_tag;
_temp4115->f1= 0; _temp4115->f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({ struct _tagged_ptr2
_temp4116= yyvs; struct _xenum_struct** _temp4118= _temp4116.curr +(
yyvsp_offset - 1); if( _temp4118 < _temp4116.base? 1: _temp4118 >= _temp4116.last_plus_one){
_throw( Null_Exception);}* _temp4118;})));( void*) _temp4115;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4119= yyls; struct Cyc_Yyltype* _temp4121= _temp4119.curr
+( yylsp_offset - 2); if( _temp4121 < _temp4119.base? 1: _temp4121 >= _temp4119.last_plus_one){
_throw( Null_Exception);}* _temp4121;}).first_line,({ struct _tagged_ptr3
_temp4122= yyls; struct Cyc_Yyltype* _temp4124= _temp4122.curr + yylsp_offset;
if( _temp4124 < _temp4122.base? 1: _temp4124 >= _temp4122.last_plus_one){ _throw(
Null_Exception);}* _temp4124;}).last_line));( struct _xenum_struct*) _temp4114;});
break; case 166: _LL4113: yyval=({ struct Cyc_Exp_tok_struct* _temp4126=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4126->tag=
Cyc_Exp_tok_tag; _temp4126->f1= Cyc_Absyn_new_exp(({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp4127=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp4127->tag= Cyc_Absyn_UnresolvedMem_e_tag;
_temp4127->f1= 0; _temp4127->f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({ struct _tagged_ptr2
_temp4128= yyvs; struct _xenum_struct** _temp4130= _temp4128.curr +(
yyvsp_offset - 2); if( _temp4130 < _temp4128.base? 1: _temp4130 >= _temp4128.last_plus_one){
_throw( Null_Exception);}* _temp4130;})));( void*) _temp4127;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4131= yyls; struct Cyc_Yyltype* _temp4133= _temp4131.curr
+( yylsp_offset - 3); if( _temp4133 < _temp4131.base? 1: _temp4133 >= _temp4131.last_plus_one){
_throw( Null_Exception);}* _temp4133;}).first_line,({ struct _tagged_ptr3
_temp4134= yyls; struct Cyc_Yyltype* _temp4136= _temp4134.curr + yylsp_offset;
if( _temp4136 < _temp4134.base? 1: _temp4136 >= _temp4134.last_plus_one){ _throw(
Null_Exception);}* _temp4136;}).last_line));( struct _xenum_struct*) _temp4126;});
break; case 167: _LL4125: yyval=({ struct Cyc_Exp_tok_struct* _temp4138=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4138->tag=
Cyc_Exp_tok_tag; _temp4138->f1= Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Comprehension_e_struct*
_temp4139=( struct Cyc_Absyn_Comprehension_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Comprehension_e_struct)); _temp4139->tag= Cyc_Absyn_Comprehension_e_tag;
_temp4139->f1= Cyc_Absyn_new_vardecl(({ struct _tuple1* _temp4140=( struct
_tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp4140->f1= Cyc_Absyn_Loc_n;
_temp4140->f2=({ struct _tagged_string* _temp4141=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp4141[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp4142= yyvs; struct _xenum_struct** _temp4144= _temp4142.curr
+( yyvsp_offset - 5); if( _temp4144 < _temp4142.base? 1: _temp4144 >= _temp4142.last_plus_one){
_throw( Null_Exception);}* _temp4144;})); _temp4141;}); _temp4140;}), Cyc_Absyn_uint_t,(
struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp( 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4145= yyls; struct Cyc_Yyltype* _temp4147= _temp4145.curr
+( yylsp_offset - 5); if( _temp4147 < _temp4145.base? 1: _temp4147 >= _temp4145.last_plus_one){
_throw( Null_Exception);}* _temp4147;}).first_line,({ struct _tagged_ptr3
_temp4148= yyls; struct Cyc_Yyltype* _temp4150= _temp4148.curr +( yylsp_offset -
5); if( _temp4150 < _temp4148.base? 1: _temp4150 >= _temp4148.last_plus_one){
_throw( Null_Exception);}* _temp4150;}).last_line))); _temp4139->f2= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4151= yyvs; struct _xenum_struct** _temp4153= _temp4151.curr
+( yyvsp_offset - 3); if( _temp4153 < _temp4151.base? 1: _temp4153 >= _temp4151.last_plus_one){
_throw( Null_Exception);}* _temp4153;})); _temp4139->f3= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4154= yyvs; struct _xenum_struct** _temp4156= _temp4154.curr
+( yyvsp_offset - 1); if( _temp4156 < _temp4154.base? 1: _temp4156 >= _temp4154.last_plus_one){
_throw( Null_Exception);}* _temp4156;}));( void*) _temp4139;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4157= yyls; struct Cyc_Yyltype* _temp4159= _temp4157.curr
+( yylsp_offset - 7); if( _temp4159 < _temp4157.base? 1: _temp4159 >= _temp4157.last_plus_one){
_throw( Null_Exception);}* _temp4159;}).first_line,({ struct _tagged_ptr3
_temp4160= yyls; struct Cyc_Yyltype* _temp4162= _temp4160.curr + yylsp_offset;
if( _temp4162 < _temp4160.base? 1: _temp4162 >= _temp4160.last_plus_one){ _throw(
Null_Exception);}* _temp4162;}).last_line));( struct _xenum_struct*) _temp4138;});
break; case 168: _LL4137: yyval=({ struct Cyc_InitializerList_tok_struct*
_temp4164=( struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct
Cyc_InitializerList_tok_struct)); _temp4164->tag= Cyc_InitializerList_tok_tag;
_temp4164->f1=({ struct Cyc_List_List* _temp4165=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4165->hd=( void*)({ struct
_tuple18* _temp4166=( struct _tuple18*) GC_malloc( sizeof( struct _tuple18));
_temp4166->f1= 0; _temp4166->f2= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp4167= yyvs; struct _xenum_struct** _temp4169= _temp4167.curr + yyvsp_offset;
if( _temp4169 < _temp4167.base? 1: _temp4169 >= _temp4167.last_plus_one){ _throw(
Null_Exception);}* _temp4169;})); _temp4166;}); _temp4165->tl= 0; _temp4165;});(
struct _xenum_struct*) _temp4164;}); break; case 169: _LL4163: yyval=({ struct
Cyc_InitializerList_tok_struct* _temp4171=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp4171->tag= Cyc_InitializerList_tok_tag;
_temp4171->f1=({ struct Cyc_List_List* _temp4172=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4172->hd=( void*)({ struct
_tuple18* _temp4173=( struct _tuple18*) GC_malloc( sizeof( struct _tuple18));
_temp4173->f1= Cyc_yyget_DesignatorList_tok(({ struct _tagged_ptr2 _temp4177=
yyvs; struct _xenum_struct** _temp4179= _temp4177.curr +( yyvsp_offset - 1); if(
_temp4179 < _temp4177.base? 1: _temp4179 >= _temp4177.last_plus_one){ _throw(
Null_Exception);}* _temp4179;})); _temp4173->f2= Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4174= yyvs; struct _xenum_struct** _temp4176= _temp4174.curr +
yyvsp_offset; if( _temp4176 < _temp4174.base? 1: _temp4176 >= _temp4174.last_plus_one){
_throw( Null_Exception);}* _temp4176;})); _temp4173;}); _temp4172->tl= 0;
_temp4172;});( struct _xenum_struct*) _temp4171;}); break; case 170: _LL4170:
yyval=({ struct Cyc_InitializerList_tok_struct* _temp4181=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp4181->tag= Cyc_InitializerList_tok_tag;
_temp4181->f1=({ struct Cyc_List_List* _temp4182=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4182->hd=( void*)({ struct
_tuple18* _temp4186=( struct _tuple18*) GC_malloc( sizeof( struct _tuple18));
_temp4186->f1= 0; _temp4186->f2= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp4187= yyvs; struct _xenum_struct** _temp4189= _temp4187.curr + yyvsp_offset;
if( _temp4189 < _temp4187.base? 1: _temp4189 >= _temp4187.last_plus_one){ _throw(
Null_Exception);}* _temp4189;})); _temp4186;}); _temp4182->tl= Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp4183= yyvs; struct _xenum_struct** _temp4185= _temp4183.curr
+( yyvsp_offset - 2); if( _temp4185 < _temp4183.base? 1: _temp4185 >= _temp4183.last_plus_one){
_throw( Null_Exception);}* _temp4185;})); _temp4182;});( struct _xenum_struct*)
_temp4181;}); break; case 171: _LL4180: yyval=({ struct Cyc_InitializerList_tok_struct*
_temp4191=( struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct
Cyc_InitializerList_tok_struct)); _temp4191->tag= Cyc_InitializerList_tok_tag;
_temp4191->f1=({ struct Cyc_List_List* _temp4192=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4192->hd=( void*)({ struct
_tuple18* _temp4196=( struct _tuple18*) GC_malloc( sizeof( struct _tuple18));
_temp4196->f1= Cyc_yyget_DesignatorList_tok(({ struct _tagged_ptr2 _temp4200=
yyvs; struct _xenum_struct** _temp4202= _temp4200.curr +( yyvsp_offset - 1); if(
_temp4202 < _temp4200.base? 1: _temp4202 >= _temp4200.last_plus_one){ _throw(
Null_Exception);}* _temp4202;})); _temp4196->f2= Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4197= yyvs; struct _xenum_struct** _temp4199= _temp4197.curr +
yyvsp_offset; if( _temp4199 < _temp4197.base? 1: _temp4199 >= _temp4197.last_plus_one){
_throw( Null_Exception);}* _temp4199;})); _temp4196;}); _temp4192->tl= Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp4193= yyvs; struct _xenum_struct** _temp4195= _temp4193.curr
+( yyvsp_offset - 3); if( _temp4195 < _temp4193.base? 1: _temp4195 >= _temp4193.last_plus_one){
_throw( Null_Exception);}* _temp4195;})); _temp4192;});( struct _xenum_struct*)
_temp4191;}); break; case 172: _LL4190: yyval=({ struct Cyc_DesignatorList_tok_struct*
_temp4204=( struct Cyc_DesignatorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp4204->tag= Cyc_DesignatorList_tok_tag; _temp4204->f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_DesignatorList_tok(({
struct _tagged_ptr2 _temp4205= yyvs; struct _xenum_struct** _temp4207= _temp4205.curr
+( yyvsp_offset - 1); if( _temp4207 < _temp4205.base? 1: _temp4207 >= _temp4205.last_plus_one){
_throw( Null_Exception);}* _temp4207;})));( struct _xenum_struct*) _temp4204;});
break; case 173: _LL4203: yyval=({ struct Cyc_DesignatorList_tok_struct*
_temp4209=( struct Cyc_DesignatorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp4209->tag= Cyc_DesignatorList_tok_tag; _temp4209->f1=({ struct Cyc_List_List*
_temp4210=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4210->hd=( void*) Cyc_yyget_Designator_tok(({ struct _tagged_ptr2 _temp4211=
yyvs; struct _xenum_struct** _temp4213= _temp4211.curr + yyvsp_offset; if(
_temp4213 < _temp4211.base? 1: _temp4213 >= _temp4211.last_plus_one){ _throw(
Null_Exception);}* _temp4213;})); _temp4210->tl= 0; _temp4210;});( struct
_xenum_struct*) _temp4209;}); break; case 174: _LL4208: yyval=({ struct Cyc_DesignatorList_tok_struct*
_temp4215=( struct Cyc_DesignatorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp4215->tag= Cyc_DesignatorList_tok_tag; _temp4215->f1=({ struct Cyc_List_List*
_temp4216=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4216->hd=( void*) Cyc_yyget_Designator_tok(({ struct _tagged_ptr2 _temp4220=
yyvs; struct _xenum_struct** _temp4222= _temp4220.curr + yyvsp_offset; if(
_temp4222 < _temp4220.base? 1: _temp4222 >= _temp4220.last_plus_one){ _throw(
Null_Exception);}* _temp4222;})); _temp4216->tl= Cyc_yyget_DesignatorList_tok(({
struct _tagged_ptr2 _temp4217= yyvs; struct _xenum_struct** _temp4219= _temp4217.curr
+( yyvsp_offset - 1); if( _temp4219 < _temp4217.base? 1: _temp4219 >= _temp4217.last_plus_one){
_throw( Null_Exception);}* _temp4219;})); _temp4216;});( struct _xenum_struct*)
_temp4215;}); break; case 175: _LL4214: yyval=({ struct Cyc_Designator_tok_struct*
_temp4224=( struct Cyc_Designator_tok_struct*) GC_malloc( sizeof( struct Cyc_Designator_tok_struct));
_temp4224->tag= Cyc_Designator_tok_tag; _temp4224->f1=( void*)({ struct Cyc_Absyn_ArrayElement_struct*
_temp4225=( struct Cyc_Absyn_ArrayElement_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayElement_struct));
_temp4225->tag= Cyc_Absyn_ArrayElement_tag; _temp4225->f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4226= yyvs; struct _xenum_struct** _temp4228= _temp4226.curr
+( yyvsp_offset - 1); if( _temp4228 < _temp4226.base? 1: _temp4228 >= _temp4226.last_plus_one){
_throw( Null_Exception);}* _temp4228;}));( void*) _temp4225;});( struct
_xenum_struct*) _temp4224;}); break; case 176: _LL4223: yyval=({ struct Cyc_Designator_tok_struct*
_temp4230=( struct Cyc_Designator_tok_struct*) GC_malloc( sizeof( struct Cyc_Designator_tok_struct));
_temp4230->tag= Cyc_Designator_tok_tag; _temp4230->f1=( void*)({ struct Cyc_Absyn_FieldName_struct*
_temp4231=( struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp4231->tag= Cyc_Absyn_FieldName_tag; _temp4231->f1=({ struct _tagged_string*
_temp4232=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp4232[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp4233= yyvs;
struct _xenum_struct** _temp4235= _temp4233.curr + yyvsp_offset; if( _temp4235 <
_temp4233.base? 1: _temp4235 >= _temp4233.last_plus_one){ _throw( Null_Exception);}*
_temp4235;})); _temp4232;});( void*) _temp4231;});( struct _xenum_struct*)
_temp4230;}); break; case 177: _LL4229: { void* t= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4237= yyvs; struct _xenum_struct** _temp4239= _temp4237.curr
+ yyvsp_offset; if( _temp4239 < _temp4237.base? 1: _temp4239 >= _temp4237.last_plus_one){
_throw( Null_Exception);}* _temp4239;}))).f2, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4240= yyls; struct Cyc_Yyltype* _temp4242= _temp4240.curr
+ yylsp_offset; if( _temp4242 < _temp4240.base? 1: _temp4242 >= _temp4240.last_plus_one){
_throw( Null_Exception);}* _temp4242;}).first_line,({ struct _tagged_ptr3
_temp4243= yyls; struct Cyc_Yyltype* _temp4245= _temp4243.curr + yylsp_offset;
if( _temp4245 < _temp4243.base? 1: _temp4245 >= _temp4243.last_plus_one){ _throw(
Null_Exception);}* _temp4245;}).last_line)); struct Cyc_List_List* atts=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4246= yyvs; struct _xenum_struct** _temp4248= _temp4246.curr
+ yyvsp_offset; if( _temp4248 < _temp4246.base? 1: _temp4248 >= _temp4246.last_plus_one){
_throw( Null_Exception);}* _temp4248;}))).f3; if( atts != 0){ Cyc_Parse_warn((
struct _tagged_string)({ char* _temp4249=( char*)"ignoring attributes in type";
struct _tagged_string _temp4250; _temp4250.curr= _temp4249; _temp4250.base=
_temp4249; _temp4250.last_plus_one= _temp4249 + 28; _temp4250;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4251= yyls; struct Cyc_Yyltype* _temp4253= _temp4251.curr
+ yylsp_offset; if( _temp4253 < _temp4251.base? 1: _temp4253 >= _temp4251.last_plus_one){
_throw( Null_Exception);}* _temp4253;}).first_line,({ struct _tagged_ptr3
_temp4254= yyls; struct Cyc_Yyltype* _temp4256= _temp4254.curr + yylsp_offset;
if( _temp4256 < _temp4254.base? 1: _temp4256 >= _temp4254.last_plus_one){ _throw(
Null_Exception);}* _temp4256;}).last_line));}{ struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4257= yyvs; struct _xenum_struct** _temp4259= _temp4257.curr
+ yyvsp_offset; if( _temp4259 < _temp4257.base? 1: _temp4259 >= _temp4257.last_plus_one){
_throw( Null_Exception);}* _temp4259;}))).f1; yyval=({ struct Cyc_ParamDecl_tok_struct*
_temp4260=( struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp4260->tag= Cyc_ParamDecl_tok_tag; _temp4260->f1=({ struct _tuple2*
_temp4261=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2) * 1); _temp4261[
0]=({ struct _tuple2 _temp4262; _temp4262.f1= 0; _temp4262.f2= tq; _temp4262.f3=
t; _temp4262;}); _temp4261;});( struct _xenum_struct*) _temp4260;}); break;}}
case 178: _LL4236: { void* t= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4264= yyvs; struct _xenum_struct** _temp4266= _temp4264.curr
+( yyvsp_offset - 1); if( _temp4266 < _temp4264.base? 1: _temp4266 >= _temp4264.last_plus_one){
_throw( Null_Exception);}* _temp4266;}))).f2, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4267= yyls; struct Cyc_Yyltype* _temp4269= _temp4267.curr
+( yylsp_offset - 1); if( _temp4269 < _temp4267.base? 1: _temp4269 >= _temp4267.last_plus_one){
_throw( Null_Exception);}* _temp4269;}).first_line,({ struct _tagged_ptr3
_temp4270= yyls; struct Cyc_Yyltype* _temp4272= _temp4270.curr +( yylsp_offset -
1); if( _temp4272 < _temp4270.base? 1: _temp4272 >= _temp4270.last_plus_one){
_throw( Null_Exception);}* _temp4272;}).last_line)); struct Cyc_List_List* atts=(*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp4273= yyvs; struct
_xenum_struct** _temp4275= _temp4273.curr +( yyvsp_offset - 1); if( _temp4275 <
_temp4273.base? 1: _temp4275 >= _temp4273.last_plus_one){ _throw( Null_Exception);}*
_temp4275;}))).f3; struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4276= yyvs; struct _xenum_struct** _temp4278= _temp4276.curr
+( yyvsp_offset - 1); if( _temp4278 < _temp4276.base? 1: _temp4278 >= _temp4276.last_plus_one){
_throw( Null_Exception);}* _temp4278;}))).f1; struct Cyc_List_List* tms=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp4279= yyvs; struct _xenum_struct** _temp4281= _temp4279.curr
+ yyvsp_offset; if( _temp4281 < _temp4279.base? 1: _temp4281 >= _temp4279.last_plus_one){
_throw( Null_Exception);}* _temp4281;})))->tms; struct _tuple6 t_info= Cyc_Parse_apply_tms(
tq, t, atts, tms); if( t_info.f3 != 0){ Cyc_Parse_warn(( struct _tagged_string)({
char* _temp4282=( char*)"bad type params, ignoring"; struct _tagged_string
_temp4283; _temp4283.curr= _temp4282; _temp4283.base= _temp4282; _temp4283.last_plus_one=
_temp4282 + 26; _temp4283;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4284= yyls; struct Cyc_Yyltype* _temp4286= _temp4284.curr + yylsp_offset;
if( _temp4286 < _temp4284.base? 1: _temp4286 >= _temp4284.last_plus_one){ _throw(
Null_Exception);}* _temp4286;}).first_line,({ struct _tagged_ptr3 _temp4287=
yyls; struct Cyc_Yyltype* _temp4289= _temp4287.curr + yylsp_offset; if(
_temp4289 < _temp4287.base? 1: _temp4289 >= _temp4287.last_plus_one){ _throw(
Null_Exception);}* _temp4289;}).last_line));} if( t_info.f4 != 0){ Cyc_Parse_warn((
struct _tagged_string)({ char* _temp4290=( char*)"bad specifiers, ignoring";
struct _tagged_string _temp4291; _temp4291.curr= _temp4290; _temp4291.base=
_temp4290; _temp4291.last_plus_one= _temp4290 + 25; _temp4291;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4292= yyls; struct Cyc_Yyltype* _temp4294= _temp4292.curr
+ yylsp_offset; if( _temp4294 < _temp4292.base? 1: _temp4294 >= _temp4292.last_plus_one){
_throw( Null_Exception);}* _temp4294;}).first_line,({ struct _tagged_ptr3
_temp4295= yyls; struct Cyc_Yyltype* _temp4297= _temp4295.curr + yylsp_offset;
if( _temp4297 < _temp4295.base? 1: _temp4297 >= _temp4295.last_plus_one){ _throw(
Null_Exception);}* _temp4297;}).last_line));} yyval=({ struct Cyc_ParamDecl_tok_struct*
_temp4298=( struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp4298->tag= Cyc_ParamDecl_tok_tag; _temp4298->f1=({ struct _tuple2*
_temp4299=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2)); _temp4299->f1=
0; _temp4299->f2= t_info.f1; _temp4299->f3= t_info.f2; _temp4299;});( struct
_xenum_struct*) _temp4298;}); break;} case 179: _LL4263: yyval=({ struct Cyc_Type_tok_struct*
_temp4301=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp4301->tag= Cyc_Type_tok_tag; _temp4301->f1=( void*)(* Cyc_yyget_ParamDecl_tok(({
struct _tagged_ptr2 _temp4302= yyvs; struct _xenum_struct** _temp4304= _temp4302.curr
+ yyvsp_offset; if( _temp4304 < _temp4302.base? 1: _temp4304 >= _temp4302.last_plus_one){
_throw( Null_Exception);}* _temp4304;}))).f3;( struct _xenum_struct*) _temp4301;});
break; case 180: _LL4300: yyval=({ struct Cyc_Type_tok_struct* _temp4306=(
struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp4306->tag= Cyc_Type_tok_tag; _temp4306->f1=( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp4307=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp4307->tag= Cyc_Absyn_JoinEff_tag; _temp4307->f1= 0;( void*) _temp4307;});(
struct _xenum_struct*) _temp4306;}); break; case 181: _LL4305: yyval=({ struct
Cyc_Type_tok_struct* _temp4309=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof(
struct Cyc_Type_tok_struct)); _temp4309->tag= Cyc_Type_tok_tag; _temp4309->f1=(
void*)({ struct Cyc_Absyn_JoinEff_struct* _temp4310=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp4310->tag= Cyc_Absyn_JoinEff_tag;
_temp4310->f1= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp4311= yyvs;
struct _xenum_struct** _temp4313= _temp4311.curr +( yyvsp_offset - 1); if(
_temp4313 < _temp4311.base? 1: _temp4313 >= _temp4311.last_plus_one){ _throw(
Null_Exception);}* _temp4313;}));( void*) _temp4310;});( struct _xenum_struct*)
_temp4309;}); break; case 182: _LL4308: yyval=({ struct Cyc_Type_tok_struct*
_temp4315=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp4315->tag= Cyc_Type_tok_tag; _temp4315->f1=( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp4316=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp4316->tag= Cyc_Absyn_JoinEff_tag; _temp4316->f1=({ struct Cyc_List_List*
_temp4317=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4317->hd=( void*) Cyc_yyget_Type_tok(({ struct _tagged_ptr2 _temp4321= yyvs;
struct _xenum_struct** _temp4323= _temp4321.curr +( yyvsp_offset - 2); if(
_temp4323 < _temp4321.base? 1: _temp4323 >= _temp4321.last_plus_one){ _throw(
Null_Exception);}* _temp4323;})); _temp4317->tl= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4318= yyvs; struct _xenum_struct** _temp4320= _temp4318.curr
+ yyvsp_offset; if( _temp4320 < _temp4318.base? 1: _temp4320 >= _temp4318.last_plus_one){
_throw( Null_Exception);}* _temp4320;})); _temp4317;});( void*) _temp4316;});(
struct _xenum_struct*) _temp4315;}); break; case 183: _LL4314: yyval=({ struct
Cyc_TypeList_tok_struct* _temp4325=( struct Cyc_TypeList_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeList_tok_struct)); _temp4325->tag= Cyc_TypeList_tok_tag;
_temp4325->f1=({ struct Cyc_List_List* _temp4326=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4326->hd=( void*) Cyc_yyget_Type_tok(({
struct _tagged_ptr2 _temp4327= yyvs; struct _xenum_struct** _temp4329= _temp4327.curr
+ yyvsp_offset; if( _temp4329 < _temp4327.base? 1: _temp4329 >= _temp4327.last_plus_one){
_throw( Null_Exception);}* _temp4329;})); _temp4326->tl= 0; _temp4326;});(
struct _xenum_struct*) _temp4325;}); break; case 184: _LL4324: yyval=({ struct
Cyc_TypeList_tok_struct* _temp4331=( struct Cyc_TypeList_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeList_tok_struct)); _temp4331->tag= Cyc_TypeList_tok_tag;
_temp4331->f1=({ struct Cyc_List_List* _temp4332=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4332->hd=( void*) Cyc_yyget_Type_tok(({
struct _tagged_ptr2 _temp4336= yyvs; struct _xenum_struct** _temp4338= _temp4336.curr
+ yyvsp_offset; if( _temp4338 < _temp4336.base? 1: _temp4338 >= _temp4336.last_plus_one){
_throw( Null_Exception);}* _temp4338;})); _temp4332->tl= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4333= yyvs; struct _xenum_struct** _temp4335= _temp4333.curr
+( yyvsp_offset - 2); if( _temp4335 < _temp4333.base? 1: _temp4335 >= _temp4333.last_plus_one){
_throw( Null_Exception);}* _temp4335;})); _temp4332;});( struct _xenum_struct*)
_temp4331;}); break; case 185: _LL4330: yyval=({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4340=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4340->tag= Cyc_AbstractDeclarator_tok_tag;
_temp4340->f1=({ struct Cyc_Parse_Abstractdeclarator* _temp4341=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4341->tms= Cyc_yyget_TypeModifierList_tok(({
struct _tagged_ptr2 _temp4342= yyvs; struct _xenum_struct** _temp4344= _temp4342.curr
+ yyvsp_offset; if( _temp4344 < _temp4342.base? 1: _temp4344 >= _temp4342.last_plus_one){
_throw( Null_Exception);}* _temp4344;})); _temp4341;});( struct _xenum_struct*)
_temp4340;}); break; case 186: _LL4339: yyval=({ struct _tagged_ptr2 _temp4346=
yyvs; struct _xenum_struct** _temp4348= _temp4346.curr + yyvsp_offset; if(
_temp4348 < _temp4346.base? 1: _temp4348 >= _temp4346.last_plus_one){ _throw(
Null_Exception);}* _temp4348;}); break; case 187: _LL4345: yyval=({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4350=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4350->tag= Cyc_AbstractDeclarator_tok_tag;
_temp4350->f1=({ struct Cyc_Parse_Abstractdeclarator* _temp4351=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4351->tms=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_TypeModifierList_tok(({ struct _tagged_ptr2 _temp4352= yyvs; struct
_xenum_struct** _temp4354= _temp4352.curr +( yyvsp_offset - 1); if( _temp4354 <
_temp4352.base? 1: _temp4354 >= _temp4352.last_plus_one){ _throw( Null_Exception);}*
_temp4354;})),( Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2
_temp4355= yyvs; struct _xenum_struct** _temp4357= _temp4355.curr + yyvsp_offset;
if( _temp4357 < _temp4355.base? 1: _temp4357 >= _temp4355.last_plus_one){ _throw(
Null_Exception);}* _temp4357;})))->tms); _temp4351;});( struct _xenum_struct*)
_temp4350;}); break; case 188: _LL4349: yyval=({ struct _tagged_ptr2 _temp4359=
yyvs; struct _xenum_struct** _temp4361= _temp4359.curr +( yyvsp_offset - 1); if(
_temp4361 < _temp4359.base? 1: _temp4361 >= _temp4359.last_plus_one){ _throw(
Null_Exception);}* _temp4361;}); break; case 189: _LL4358: yyval=({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4363=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4363->tag= Cyc_AbstractDeclarator_tok_tag;
_temp4363->f1=({ struct Cyc_Parse_Abstractdeclarator* _temp4364=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4364->tms=({
struct Cyc_List_List* _temp4365=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4365->hd=( void*) Cyc_Absyn_Carray_mod; _temp4365->tl=
0; _temp4365;}); _temp4364;});( struct _xenum_struct*) _temp4363;}); break; case
190: _LL4362: yyval=({ struct Cyc_AbstractDeclarator_tok_struct* _temp4367=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp4367->tag= Cyc_AbstractDeclarator_tok_tag; _temp4367->f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4368=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4368->tms=({ struct Cyc_List_List* _temp4369=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4369->hd=( void*) Cyc_Absyn_Carray_mod;
_temp4369->tl=( Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2
_temp4370= yyvs; struct _xenum_struct** _temp4372= _temp4370.curr +(
yyvsp_offset - 2); if( _temp4372 < _temp4370.base? 1: _temp4372 >= _temp4370.last_plus_one){
_throw( Null_Exception);}* _temp4372;})))->tms; _temp4369;}); _temp4368;});(
struct _xenum_struct*) _temp4367;}); break; case 191: _LL4366: yyval=({ struct
Cyc_AbstractDeclarator_tok_struct* _temp4374=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp4374->tag=
Cyc_AbstractDeclarator_tok_tag; _temp4374->f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4375=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4375->tms=({ struct Cyc_List_List* _temp4376=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4376->hd=( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp4377=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp4377->tag= Cyc_Absyn_ConstArray_mod_tag;
_temp4377->f1= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4378= yyvs; struct
_xenum_struct** _temp4380= _temp4378.curr +( yyvsp_offset - 1); if( _temp4380 <
_temp4378.base? 1: _temp4380 >= _temp4378.last_plus_one){ _throw( Null_Exception);}*
_temp4380;}));( void*) _temp4377;}); _temp4376->tl= 0; _temp4376;}); _temp4375;});(
struct _xenum_struct*) _temp4374;}); break; case 192: _LL4373: yyval=({ struct
Cyc_AbstractDeclarator_tok_struct* _temp4382=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp4382->tag=
Cyc_AbstractDeclarator_tok_tag; _temp4382->f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4383=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4383->tms=({ struct Cyc_List_List* _temp4384=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4384->hd=( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp4388=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp4388->tag= Cyc_Absyn_ConstArray_mod_tag;
_temp4388->f1= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4389= yyvs; struct
_xenum_struct** _temp4391= _temp4389.curr +( yyvsp_offset - 1); if( _temp4391 <
_temp4389.base? 1: _temp4391 >= _temp4389.last_plus_one){ _throw( Null_Exception);}*
_temp4391;}));( void*) _temp4388;}); _temp4384->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp4385= yyvs; struct _xenum_struct** _temp4387= _temp4385.curr
+( yyvsp_offset - 3); if( _temp4387 < _temp4385.base? 1: _temp4387 >= _temp4385.last_plus_one){
_throw( Null_Exception);}* _temp4387;})))->tms; _temp4384;}); _temp4383;});(
struct _xenum_struct*) _temp4382;}); break; case 193: _LL4381: yyval=({ struct
Cyc_AbstractDeclarator_tok_struct* _temp4393=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp4393->tag=
Cyc_AbstractDeclarator_tok_tag; _temp4393->f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4394=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4394->tms=({ struct Cyc_List_List* _temp4395=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4395->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4396=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4396->tag= Cyc_Absyn_Function_mod_tag; _temp4396->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4397=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4397->tag= Cyc_Absyn_WithTypes_tag; _temp4397->f1= 0; _temp4397->f2= 0;
_temp4397->f3= 0;( void*) _temp4397;});( void*) _temp4396;}); _temp4395->tl= 0;
_temp4395;}); _temp4394;});( struct _xenum_struct*) _temp4393;}); break; case
194: _LL4392: yyval=({ struct Cyc_AbstractDeclarator_tok_struct* _temp4399=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp4399->tag= Cyc_AbstractDeclarator_tok_tag; _temp4399->f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4400=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4400->tms=({ struct Cyc_List_List* _temp4401=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4401->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4402=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4402->tag= Cyc_Absyn_Function_mod_tag; _temp4402->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4403=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4403->tag= Cyc_Absyn_WithTypes_tag; _temp4403->f1= 0; _temp4403->f2= 0;
_temp4403->f3=({ struct Cyc_Core_Opt* _temp4404=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp4404->v=( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp4405=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp4405->tag= Cyc_Absyn_JoinEff_tag; _temp4405->f1= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4406= yyvs; struct _xenum_struct** _temp4408= _temp4406.curr
+( yyvsp_offset - 1); if( _temp4408 < _temp4406.base? 1: _temp4408 >= _temp4406.last_plus_one){
_throw( Null_Exception);}* _temp4408;}));( void*) _temp4405;}); _temp4404;});(
void*) _temp4403;});( void*) _temp4402;}); _temp4401->tl= 0; _temp4401;});
_temp4400;});( struct _xenum_struct*) _temp4399;}); break; case 195: _LL4398: {
struct _tuple16 _temp4415; struct Cyc_Core_Opt* _temp4416; int _temp4418; struct
Cyc_List_List* _temp4420; struct _tuple16* _temp4413= Cyc_yyget_ParamDeclListBool_tok(({
struct _tagged_ptr2 _temp4410= yyvs; struct _xenum_struct** _temp4412= _temp4410.curr
+( yyvsp_offset - 1); if( _temp4412 < _temp4410.base? 1: _temp4412 >= _temp4410.last_plus_one){
_throw( Null_Exception);}* _temp4412;})); _temp4415=* _temp4413; _LL4421:
_temp4420= _temp4415.f1; goto _LL4419; _LL4419: _temp4418= _temp4415.f2; goto
_LL4417; _LL4417: _temp4416= _temp4415.f3; goto _LL4414; _LL4414: yyval=({
struct Cyc_AbstractDeclarator_tok_struct* _temp4422=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp4422->tag=
Cyc_AbstractDeclarator_tok_tag; _temp4422->f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4423=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4423->tms=({ struct Cyc_List_List* _temp4424=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4424->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4425=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4425->tag= Cyc_Absyn_Function_mod_tag; _temp4425->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4426=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4426->tag= Cyc_Absyn_WithTypes_tag; _temp4426->f1= _temp4420; _temp4426->f2=
_temp4418; _temp4426->f3= _temp4416;( void*) _temp4426;});( void*) _temp4425;});
_temp4424->tl= 0; _temp4424;}); _temp4423;});( struct _xenum_struct*) _temp4422;});
break;} case 196: _LL4409: yyval=({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4428=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4428->tag= Cyc_AbstractDeclarator_tok_tag;
_temp4428->f1=({ struct Cyc_Parse_Abstractdeclarator* _temp4429=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4429->tms=({
struct Cyc_List_List* _temp4430=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4430->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4434=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4434->tag= Cyc_Absyn_Function_mod_tag; _temp4434->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4435=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4435->tag= Cyc_Absyn_WithTypes_tag; _temp4435->f1= 0; _temp4435->f2= 0;
_temp4435->f3= 0;( void*) _temp4435;});( void*) _temp4434;}); _temp4430->tl=(
Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2 _temp4431= yyvs; struct
_xenum_struct** _temp4433= _temp4431.curr +( yyvsp_offset - 2); if( _temp4433 <
_temp4431.base? 1: _temp4433 >= _temp4431.last_plus_one){ _throw( Null_Exception);}*
_temp4433;})))->tms; _temp4430;}); _temp4429;});( struct _xenum_struct*)
_temp4428;}); break; case 197: _LL4427: { struct Cyc_Core_Opt* eff=({ struct Cyc_Core_Opt*
_temp4437=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp4437->v=( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp4438=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp4438->tag= Cyc_Absyn_JoinEff_tag;
_temp4438->f1= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp4439= yyvs;
struct _xenum_struct** _temp4441= _temp4439.curr +( yyvsp_offset - 1); if(
_temp4441 < _temp4439.base? 1: _temp4441 >= _temp4439.last_plus_one){ _throw(
Null_Exception);}* _temp4441;}));( void*) _temp4438;}); _temp4437;}); yyval=({
struct Cyc_AbstractDeclarator_tok_struct* _temp4442=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp4442->tag=
Cyc_AbstractDeclarator_tok_tag; _temp4442->f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4443=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4443->tms=({ struct Cyc_List_List* _temp4444=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4444->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4448=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4448->tag= Cyc_Absyn_Function_mod_tag; _temp4448->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4449=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4449->tag= Cyc_Absyn_WithTypes_tag; _temp4449->f1= 0; _temp4449->f2= 0;
_temp4449->f3= eff;( void*) _temp4449;});( void*) _temp4448;}); _temp4444->tl=(
Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2 _temp4445= yyvs; struct
_xenum_struct** _temp4447= _temp4445.curr +( yyvsp_offset - 4); if( _temp4447 <
_temp4445.base? 1: _temp4447 >= _temp4445.last_plus_one){ _throw( Null_Exception);}*
_temp4447;})))->tms; _temp4444;}); _temp4443;});( struct _xenum_struct*)
_temp4442;}); break;} case 198: _LL4436: { struct _tuple16 _temp4456; struct Cyc_Core_Opt*
_temp4457; int _temp4459; struct Cyc_List_List* _temp4461; struct _tuple16*
_temp4454= Cyc_yyget_ParamDeclListBool_tok(({ struct _tagged_ptr2 _temp4451=
yyvs; struct _xenum_struct** _temp4453= _temp4451.curr +( yyvsp_offset - 1); if(
_temp4453 < _temp4451.base? 1: _temp4453 >= _temp4451.last_plus_one){ _throw(
Null_Exception);}* _temp4453;})); _temp4456=* _temp4454; _LL4462: _temp4461=
_temp4456.f1; goto _LL4460; _LL4460: _temp4459= _temp4456.f2; goto _LL4458;
_LL4458: _temp4457= _temp4456.f3; goto _LL4455; _LL4455: yyval=({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4463=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4463->tag= Cyc_AbstractDeclarator_tok_tag;
_temp4463->f1=({ struct Cyc_Parse_Abstractdeclarator* _temp4464=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4464->tms=({
struct Cyc_List_List* _temp4465=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4465->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4469=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4469->tag= Cyc_Absyn_Function_mod_tag; _temp4469->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4470=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4470->tag= Cyc_Absyn_WithTypes_tag; _temp4470->f1= _temp4461; _temp4470->f2=
_temp4459; _temp4470->f3= _temp4457;( void*) _temp4470;});( void*) _temp4469;});
_temp4465->tl=( Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2
_temp4466= yyvs; struct _xenum_struct** _temp4468= _temp4466.curr +(
yyvsp_offset - 3); if( _temp4468 < _temp4466.base? 1: _temp4468 >= _temp4466.last_plus_one){
_throw( Null_Exception);}* _temp4468;})))->tms; _temp4465;}); _temp4464;});(
struct _xenum_struct*) _temp4463;}); break;} case 199: _LL4450: { struct Cyc_List_List*
ts=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4472=
yyls; struct Cyc_Yyltype* _temp4474= _temp4472.curr +( yylsp_offset - 2); if(
_temp4474 < _temp4472.base? 1: _temp4474 >= _temp4472.last_plus_one){ _throw(
Null_Exception);}* _temp4474;}).first_line,({ struct _tagged_ptr3 _temp4475=
yyls; struct Cyc_Yyltype* _temp4477= _temp4475.curr + yylsp_offset; if(
_temp4477 < _temp4475.base? 1: _temp4477 >= _temp4475.last_plus_one){ _throw(
Null_Exception);}* _temp4477;}).last_line),(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp4478= yyvs; struct _xenum_struct** _temp4480= _temp4478.curr +(
yyvsp_offset - 1); if( _temp4480 < _temp4478.base? 1: _temp4480 >= _temp4478.last_plus_one){
_throw( Null_Exception);}* _temp4480;})))); yyval=({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4481=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4481->tag= Cyc_AbstractDeclarator_tok_tag;
_temp4481->f1=({ struct Cyc_Parse_Abstractdeclarator* _temp4482=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4482->tms=({
struct Cyc_List_List* _temp4483=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4483->hd=( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp4487=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp4487->tag= Cyc_Absyn_TypeParams_mod_tag;
_temp4487->f1= ts; _temp4487->f2= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4488= yyls; struct Cyc_Yyltype* _temp4490= _temp4488.curr +(
yylsp_offset - 2); if( _temp4490 < _temp4488.base? 1: _temp4490 >= _temp4488.last_plus_one){
_throw( Null_Exception);}* _temp4490;}).first_line,({ struct _tagged_ptr3
_temp4491= yyls; struct Cyc_Yyltype* _temp4493= _temp4491.curr + yylsp_offset;
if( _temp4493 < _temp4491.base? 1: _temp4493 >= _temp4491.last_plus_one){ _throw(
Null_Exception);}* _temp4493;}).last_line); _temp4487->f3= 0;( void*) _temp4487;});
_temp4483->tl=( Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2
_temp4484= yyvs; struct _xenum_struct** _temp4486= _temp4484.curr +(
yyvsp_offset - 3); if( _temp4486 < _temp4484.base? 1: _temp4486 >= _temp4484.last_plus_one){
_throw( Null_Exception);}* _temp4486;})))->tms; _temp4483;}); _temp4482;});(
struct _xenum_struct*) _temp4481;}); break;} case 200: _LL4471:(( struct Cyc_Lexing_lexbuf*)
Cyc_Parse_lbuf->v)->lex_curr_pos -= 1;{ struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4495= yyls; struct Cyc_Yyltype* _temp4497= _temp4495.curr
+( yylsp_offset - 2); if( _temp4497 < _temp4495.base? 1: _temp4497 >= _temp4495.last_plus_one){
_throw( Null_Exception);}* _temp4497;}).first_line,({ struct _tagged_ptr3
_temp4498= yyls; struct Cyc_Yyltype* _temp4500= _temp4498.curr + yylsp_offset;
if( _temp4500 < _temp4498.base? 1: _temp4500 >= _temp4498.last_plus_one){ _throw(
Null_Exception);}* _temp4500;}).last_line),(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp4501= yyvs; struct _xenum_struct** _temp4503= _temp4501.curr +(
yyvsp_offset - 1); if( _temp4503 < _temp4501.base? 1: _temp4503 >= _temp4501.last_plus_one){
_throw( Null_Exception);}* _temp4503;})))); yyval=({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4504=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4504->tag= Cyc_AbstractDeclarator_tok_tag;
_temp4504->f1=({ struct Cyc_Parse_Abstractdeclarator* _temp4505=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4505->tms=({
struct Cyc_List_List* _temp4506=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4506->hd=( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp4510=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp4510->tag= Cyc_Absyn_TypeParams_mod_tag;
_temp4510->f1= ts; _temp4510->f2= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4511= yyls; struct Cyc_Yyltype* _temp4513= _temp4511.curr +(
yylsp_offset - 2); if( _temp4513 < _temp4511.base? 1: _temp4513 >= _temp4511.last_plus_one){
_throw( Null_Exception);}* _temp4513;}).first_line,({ struct _tagged_ptr3
_temp4514= yyls; struct Cyc_Yyltype* _temp4516= _temp4514.curr + yylsp_offset;
if( _temp4516 < _temp4514.base? 1: _temp4516 >= _temp4514.last_plus_one){ _throw(
Null_Exception);}* _temp4516;}).last_line); _temp4510->f3= 0;( void*) _temp4510;});
_temp4506->tl=( Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2
_temp4507= yyvs; struct _xenum_struct** _temp4509= _temp4507.curr +(
yyvsp_offset - 3); if( _temp4509 < _temp4507.base? 1: _temp4509 >= _temp4507.last_plus_one){
_throw( Null_Exception);}* _temp4509;})))->tms; _temp4506;}); _temp4505;});(
struct _xenum_struct*) _temp4504;}); break;} case 201: _LL4494: yyval=({ struct
Cyc_AbstractDeclarator_tok_struct* _temp4518=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp4518->tag=
Cyc_AbstractDeclarator_tok_tag; _temp4518->f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4519=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4519->tms=({ struct Cyc_List_List* _temp4520=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4520->hd=( void*)({ struct Cyc_Absyn_Attributes_mod_struct*
_temp4524=( struct Cyc_Absyn_Attributes_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Attributes_mod_struct)); _temp4524->tag= Cyc_Absyn_Attributes_mod_tag;
_temp4524->f1= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4525=
yyls; struct Cyc_Yyltype* _temp4527= _temp4525.curr + yylsp_offset; if(
_temp4527 < _temp4525.base? 1: _temp4527 >= _temp4525.last_plus_one){ _throw(
Null_Exception);}* _temp4527;}).first_line,({ struct _tagged_ptr3 _temp4528=
yyls; struct Cyc_Yyltype* _temp4530= _temp4528.curr + yylsp_offset; if(
_temp4530 < _temp4528.base? 1: _temp4530 >= _temp4528.last_plus_one){ _throw(
Null_Exception);}* _temp4530;}).last_line); _temp4524->f2= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp4531= yyvs; struct _xenum_struct** _temp4533= _temp4531.curr
+ yyvsp_offset; if( _temp4533 < _temp4531.base? 1: _temp4533 >= _temp4531.last_plus_one){
_throw( Null_Exception);}* _temp4533;}));( void*) _temp4524;}); _temp4520->tl=(
Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2 _temp4521= yyvs; struct
_xenum_struct** _temp4523= _temp4521.curr +( yyvsp_offset - 1); if( _temp4523 <
_temp4521.base? 1: _temp4523 >= _temp4521.last_plus_one){ _throw( Null_Exception);}*
_temp4523;})))->tms; _temp4520;}); _temp4519;});( struct _xenum_struct*)
_temp4518;}); break; case 202: _LL4517: yyval=({ struct _tagged_ptr2 _temp4535=
yyvs; struct _xenum_struct** _temp4537= _temp4535.curr + yyvsp_offset; if(
_temp4537 < _temp4535.base? 1: _temp4537 >= _temp4535.last_plus_one){ _throw(
Null_Exception);}* _temp4537;}); break; case 203: _LL4534: yyval=({ struct
_tagged_ptr2 _temp4539= yyvs; struct _xenum_struct** _temp4541= _temp4539.curr +
yyvsp_offset; if( _temp4541 < _temp4539.base? 1: _temp4541 >= _temp4539.last_plus_one){
_throw( Null_Exception);}* _temp4541;}); break; case 204: _LL4538: yyval=({
struct _tagged_ptr2 _temp4543= yyvs; struct _xenum_struct** _temp4545= _temp4543.curr
+ yyvsp_offset; if( _temp4545 < _temp4543.base? 1: _temp4545 >= _temp4543.last_plus_one){
_throw( Null_Exception);}* _temp4545;}); break; case 205: _LL4542: yyval=({
struct _tagged_ptr2 _temp4547= yyvs; struct _xenum_struct** _temp4549= _temp4547.curr
+ yyvsp_offset; if( _temp4549 < _temp4547.base? 1: _temp4549 >= _temp4547.last_plus_one){
_throw( Null_Exception);}* _temp4549;}); break; case 206: _LL4546: yyval=({
struct _tagged_ptr2 _temp4551= yyvs; struct _xenum_struct** _temp4553= _temp4551.curr
+ yyvsp_offset; if( _temp4553 < _temp4551.base? 1: _temp4553 >= _temp4551.last_plus_one){
_throw( Null_Exception);}* _temp4553;}); break; case 207: _LL4550: yyval=({
struct _tagged_ptr2 _temp4555= yyvs; struct _xenum_struct** _temp4557= _temp4555.curr
+ yyvsp_offset; if( _temp4557 < _temp4555.base? 1: _temp4557 >= _temp4555.last_plus_one){
_throw( Null_Exception);}* _temp4557;}); break; case 208: _LL4554: yyval=({
struct Cyc_Stmt_tok_struct* _temp4559=( struct Cyc_Stmt_tok_struct*) GC_malloc(
sizeof( struct Cyc_Stmt_tok_struct)); _temp4559->tag= Cyc_Stmt_tok_tag;
_temp4559->f1= Cyc_Absyn_new_stmt(({ struct Cyc_Absyn_Cut_s_struct* _temp4560=(
struct Cyc_Absyn_Cut_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Cut_s_struct));
_temp4560->tag= Cyc_Absyn_Cut_s_tag; _temp4560->f1= Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp4561= yyvs; struct _xenum_struct** _temp4563= _temp4561.curr +
yyvsp_offset; if( _temp4563 < _temp4561.base? 1: _temp4563 >= _temp4561.last_plus_one){
_throw( Null_Exception);}* _temp4563;}));( void*) _temp4560;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4564= yyls; struct Cyc_Yyltype* _temp4566= _temp4564.curr
+( yylsp_offset - 1); if( _temp4566 < _temp4564.base? 1: _temp4566 >= _temp4564.last_plus_one){
_throw( Null_Exception);}* _temp4566;}).first_line,({ struct _tagged_ptr3
_temp4567= yyls; struct Cyc_Yyltype* _temp4569= _temp4567.curr + yylsp_offset;
if( _temp4569 < _temp4567.base? 1: _temp4569 >= _temp4567.last_plus_one){ _throw(
Null_Exception);}* _temp4569;}).last_line));( struct _xenum_struct*) _temp4559;});
break; case 209: _LL4558: yyval=({ struct Cyc_Stmt_tok_struct* _temp4571=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp4571->tag= Cyc_Stmt_tok_tag; _temp4571->f1= Cyc_Absyn_new_stmt(({ struct
Cyc_Absyn_Splice_s_struct* _temp4572=( struct Cyc_Absyn_Splice_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Splice_s_struct)); _temp4572->tag= Cyc_Absyn_Splice_s_tag;
_temp4572->f1= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4573= yyvs; struct
_xenum_struct** _temp4575= _temp4573.curr + yyvsp_offset; if( _temp4575 <
_temp4573.base? 1: _temp4575 >= _temp4573.last_plus_one){ _throw( Null_Exception);}*
_temp4575;}));( void*) _temp4572;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4576= yyls; struct Cyc_Yyltype* _temp4578= _temp4576.curr +(
yylsp_offset - 1); if( _temp4578 < _temp4576.base? 1: _temp4578 >= _temp4576.last_plus_one){
_throw( Null_Exception);}* _temp4578;}).first_line,({ struct _tagged_ptr3
_temp4579= yyls; struct Cyc_Yyltype* _temp4581= _temp4579.curr + yylsp_offset;
if( _temp4581 < _temp4579.base? 1: _temp4581 >= _temp4579.last_plus_one){ _throw(
Null_Exception);}* _temp4581;}).last_line));( struct _xenum_struct*) _temp4571;});
break; case 210: _LL4570: yyval=({ struct Cyc_Stmt_tok_struct* _temp4583=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp4583->tag= Cyc_Stmt_tok_tag; _temp4583->f1= Cyc_Absyn_new_stmt(({ struct
Cyc_Absyn_Label_s_struct* _temp4584=( struct Cyc_Absyn_Label_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Label_s_struct)); _temp4584->tag= Cyc_Absyn_Label_s_tag;
_temp4584->f1=({ struct _tagged_string* _temp4585=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp4585[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp4586= yyvs; struct _xenum_struct** _temp4588= _temp4586.curr
+( yyvsp_offset - 2); if( _temp4588 < _temp4586.base? 1: _temp4588 >= _temp4586.last_plus_one){
_throw( Null_Exception);}* _temp4588;})); _temp4585;}); _temp4584->f2= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp4589= yyvs; struct _xenum_struct** _temp4591= _temp4589.curr
+ yyvsp_offset; if( _temp4591 < _temp4589.base? 1: _temp4591 >= _temp4589.last_plus_one){
_throw( Null_Exception);}* _temp4591;}));( void*) _temp4584;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4592= yyls; struct Cyc_Yyltype* _temp4594= _temp4592.curr
+( yylsp_offset - 2); if( _temp4594 < _temp4592.base? 1: _temp4594 >= _temp4592.last_plus_one){
_throw( Null_Exception);}* _temp4594;}).first_line,({ struct _tagged_ptr3
_temp4595= yyls; struct Cyc_Yyltype* _temp4597= _temp4595.curr + yylsp_offset;
if( _temp4597 < _temp4595.base? 1: _temp4597 >= _temp4595.last_plus_one){ _throw(
Null_Exception);}* _temp4597;}).last_line));( struct _xenum_struct*) _temp4583;});
break; case 211: _LL4582: yyval=({ struct Cyc_Stmt_tok_struct* _temp4599=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp4599->tag= Cyc_Stmt_tok_tag; _temp4599->f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4600= yyls; struct Cyc_Yyltype* _temp4602= _temp4600.curr
+ yylsp_offset; if( _temp4602 < _temp4600.base? 1: _temp4602 >= _temp4600.last_plus_one){
_throw( Null_Exception);}* _temp4602;}).first_line,({ struct _tagged_ptr3
_temp4603= yyls; struct Cyc_Yyltype* _temp4605= _temp4603.curr + yylsp_offset;
if( _temp4605 < _temp4603.base? 1: _temp4605 >= _temp4603.last_plus_one){ _throw(
Null_Exception);}* _temp4605;}).last_line));( struct _xenum_struct*) _temp4599;});
break; case 212: _LL4598: yyval=({ struct Cyc_Stmt_tok_struct* _temp4607=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp4607->tag= Cyc_Stmt_tok_tag; _temp4607->f1= Cyc_Absyn_exp_stmt( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4608= yyvs; struct _xenum_struct** _temp4610= _temp4608.curr
+( yyvsp_offset - 1); if( _temp4610 < _temp4608.base? 1: _temp4610 >= _temp4608.last_plus_one){
_throw( Null_Exception);}* _temp4610;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4611= yyls; struct Cyc_Yyltype* _temp4613= _temp4611.curr +(
yylsp_offset - 1); if( _temp4613 < _temp4611.base? 1: _temp4613 >= _temp4611.last_plus_one){
_throw( Null_Exception);}* _temp4613;}).first_line,({ struct _tagged_ptr3
_temp4614= yyls; struct Cyc_Yyltype* _temp4616= _temp4614.curr + yylsp_offset;
if( _temp4616 < _temp4614.base? 1: _temp4616 >= _temp4614.last_plus_one){ _throw(
Null_Exception);}* _temp4616;}).last_line));( struct _xenum_struct*) _temp4607;});
break; case 213: _LL4606: yyval=({ struct Cyc_Stmt_tok_struct* _temp4618=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp4618->tag= Cyc_Stmt_tok_tag; _temp4618->f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4619= yyls; struct Cyc_Yyltype* _temp4621= _temp4619.curr
+( yylsp_offset - 1); if( _temp4621 < _temp4619.base? 1: _temp4621 >= _temp4619.last_plus_one){
_throw( Null_Exception);}* _temp4621;}).first_line,({ struct _tagged_ptr3
_temp4622= yyls; struct Cyc_Yyltype* _temp4624= _temp4622.curr + yylsp_offset;
if( _temp4624 < _temp4622.base? 1: _temp4624 >= _temp4622.last_plus_one){ _throw(
Null_Exception);}* _temp4624;}).last_line));( struct _xenum_struct*) _temp4618;});
break; case 214: _LL4617: yyval=({ struct _tagged_ptr2 _temp4626= yyvs; struct
_xenum_struct** _temp4628= _temp4626.curr +( yyvsp_offset - 1); if( _temp4628 <
_temp4626.base? 1: _temp4628 >= _temp4626.last_plus_one){ _throw( Null_Exception);}*
_temp4628;}); break; case 215: _LL4625:{ void* _temp4633= Cyc_yyget_BlockItem_tok(({
struct _tagged_ptr2 _temp4630= yyvs; struct _xenum_struct** _temp4632= _temp4630.curr
+ yyvsp_offset; if( _temp4632 < _temp4630.base? 1: _temp4632 >= _temp4630.last_plus_one){
_throw( Null_Exception);}* _temp4632;})); struct Cyc_List_List* _temp4641;
struct Cyc_Absyn_Fndecl* _temp4643; struct Cyc_Absyn_Stmt* _temp4645; _LL4635:
if((( struct _enum_struct*) _temp4633)->tag == Cyc_Parse_TopDecls_bl_tag){
_LL4642: _temp4641=( struct Cyc_List_List*)(( struct Cyc_Parse_TopDecls_bl_struct*)
_temp4633)->f1; goto _LL4636;} else{ goto _LL4637;} _LL4637: if((( struct
_enum_struct*) _temp4633)->tag == Cyc_Parse_FnDecl_bl_tag){ _LL4644: _temp4643=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Parse_FnDecl_bl_struct*) _temp4633)->f1;
goto _LL4638;} else{ goto _LL4639;} _LL4639: if((( struct _enum_struct*)
_temp4633)->tag == Cyc_Parse_Stmt_bl_tag){ _LL4646: _temp4645=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Parse_Stmt_bl_struct*) _temp4633)->f1; goto _LL4640;} else{ goto
_LL4634;} _LL4636: yyval=({ struct Cyc_Stmt_tok_struct* _temp4647=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp4647->tag= Cyc_Stmt_tok_tag;
_temp4647->f1= Cyc_Parse_flatten_declarations( _temp4641, Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4648= yyls; struct Cyc_Yyltype*
_temp4650= _temp4648.curr + yylsp_offset; if( _temp4650 < _temp4648.base? 1:
_temp4650 >= _temp4648.last_plus_one){ _throw( Null_Exception);}* _temp4650;}).first_line,({
struct _tagged_ptr3 _temp4651= yyls; struct Cyc_Yyltype* _temp4653= _temp4651.curr
+ yylsp_offset; if( _temp4653 < _temp4651.base? 1: _temp4653 >= _temp4651.last_plus_one){
_throw( Null_Exception);}* _temp4653;}).last_line)));( struct _xenum_struct*)
_temp4647;}); goto _LL4634; _LL4638: yyval=({ struct Cyc_Stmt_tok_struct*
_temp4654=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp4654->tag= Cyc_Stmt_tok_tag; _temp4654->f1= Cyc_Parse_flatten_decl( Cyc_Absyn_new_decl(({
struct Cyc_Absyn_Fn_d_struct* _temp4655=( struct Cyc_Absyn_Fn_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct)); _temp4655->tag= Cyc_Absyn_Fn_d_tag;
_temp4655->f1= _temp4643;( void*) _temp4655;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4656= yyls; struct Cyc_Yyltype* _temp4658= _temp4656.curr
+ yylsp_offset; if( _temp4658 < _temp4656.base? 1: _temp4658 >= _temp4656.last_plus_one){
_throw( Null_Exception);}* _temp4658;}).first_line,({ struct _tagged_ptr3
_temp4659= yyls; struct Cyc_Yyltype* _temp4661= _temp4659.curr + yylsp_offset;
if( _temp4661 < _temp4659.base? 1: _temp4661 >= _temp4659.last_plus_one){ _throw(
Null_Exception);}* _temp4661;}).last_line)), Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4662= yyls; struct Cyc_Yyltype* _temp4664= _temp4662.curr
+ yylsp_offset; if( _temp4664 < _temp4662.base? 1: _temp4664 >= _temp4662.last_plus_one){
_throw( Null_Exception);}* _temp4664;}).first_line,({ struct _tagged_ptr3
_temp4665= yyls; struct Cyc_Yyltype* _temp4667= _temp4665.curr + yylsp_offset;
if( _temp4667 < _temp4665.base? 1: _temp4667 >= _temp4665.last_plus_one){ _throw(
Null_Exception);}* _temp4667;}).last_line)));( struct _xenum_struct*) _temp4654;});
goto _LL4634; _LL4640: yyval=({ struct Cyc_Stmt_tok_struct* _temp4668=( struct
Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp4668->tag=
Cyc_Stmt_tok_tag; _temp4668->f1= _temp4645;( struct _xenum_struct*) _temp4668;});
goto _LL4634; _LL4634:;} break; case 216: _LL4629:{ void* _temp4673= Cyc_yyget_BlockItem_tok(({
struct _tagged_ptr2 _temp4670= yyvs; struct _xenum_struct** _temp4672= _temp4670.curr
+( yyvsp_offset - 1); if( _temp4672 < _temp4670.base? 1: _temp4672 >= _temp4670.last_plus_one){
_throw( Null_Exception);}* _temp4672;})); struct Cyc_List_List* _temp4681;
struct Cyc_Absyn_Fndecl* _temp4683; struct Cyc_Absyn_Stmt* _temp4685; _LL4675:
if((( struct _enum_struct*) _temp4673)->tag == Cyc_Parse_TopDecls_bl_tag){
_LL4682: _temp4681=( struct Cyc_List_List*)(( struct Cyc_Parse_TopDecls_bl_struct*)
_temp4673)->f1; goto _LL4676;} else{ goto _LL4677;} _LL4677: if((( struct
_enum_struct*) _temp4673)->tag == Cyc_Parse_FnDecl_bl_tag){ _LL4684: _temp4683=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Parse_FnDecl_bl_struct*) _temp4673)->f1;
goto _LL4678;} else{ goto _LL4679;} _LL4679: if((( struct _enum_struct*)
_temp4673)->tag == Cyc_Parse_Stmt_bl_tag){ _LL4686: _temp4685=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Parse_Stmt_bl_struct*) _temp4673)->f1; goto _LL4680;} else{ goto
_LL4674;} _LL4676: yyval=({ struct Cyc_Stmt_tok_struct* _temp4687=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp4687->tag= Cyc_Stmt_tok_tag;
_temp4687->f1= Cyc_Parse_flatten_declarations( _temp4681, Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp4688= yyvs; struct _xenum_struct** _temp4690= _temp4688.curr
+ yyvsp_offset; if( _temp4690 < _temp4688.base? 1: _temp4690 >= _temp4688.last_plus_one){
_throw( Null_Exception);}* _temp4690;})));( struct _xenum_struct*) _temp4687;});
goto _LL4674; _LL4678: yyval=({ struct Cyc_Stmt_tok_struct* _temp4691=( struct
Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp4691->tag=
Cyc_Stmt_tok_tag; _temp4691->f1= Cyc_Parse_flatten_decl( Cyc_Absyn_new_decl(({
struct Cyc_Absyn_Fn_d_struct* _temp4692=( struct Cyc_Absyn_Fn_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct)); _temp4692->tag= Cyc_Absyn_Fn_d_tag;
_temp4692->f1= _temp4683;( void*) _temp4692;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4693= yyls; struct Cyc_Yyltype* _temp4695= _temp4693.curr
+( yylsp_offset - 1); if( _temp4695 < _temp4693.base? 1: _temp4695 >= _temp4693.last_plus_one){
_throw( Null_Exception);}* _temp4695;}).first_line,({ struct _tagged_ptr3
_temp4696= yyls; struct Cyc_Yyltype* _temp4698= _temp4696.curr +( yylsp_offset -
1); if( _temp4698 < _temp4696.base? 1: _temp4698 >= _temp4696.last_plus_one){
_throw( Null_Exception);}* _temp4698;}).last_line)), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp4699= yyvs; struct _xenum_struct** _temp4701= _temp4699.curr
+ yyvsp_offset; if( _temp4701 < _temp4699.base? 1: _temp4701 >= _temp4699.last_plus_one){
_throw( Null_Exception);}* _temp4701;})));( struct _xenum_struct*) _temp4691;});
goto _LL4674; _LL4680: yyval=({ struct Cyc_Stmt_tok_struct* _temp4702=( struct
Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp4702->tag=
Cyc_Stmt_tok_tag; _temp4702->f1= Cyc_Absyn_seq_stmt( _temp4685, Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp4703= yyvs; struct _xenum_struct** _temp4705= _temp4703.curr
+ yyvsp_offset; if( _temp4705 < _temp4703.base? 1: _temp4705 >= _temp4703.last_plus_one){
_throw( Null_Exception);}* _temp4705;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4706= yyls; struct Cyc_Yyltype* _temp4708= _temp4706.curr +(
yylsp_offset - 1); if( _temp4708 < _temp4706.base? 1: _temp4708 >= _temp4706.last_plus_one){
_throw( Null_Exception);}* _temp4708;}).first_line,({ struct _tagged_ptr3
_temp4709= yyls; struct Cyc_Yyltype* _temp4711= _temp4709.curr + yylsp_offset;
if( _temp4711 < _temp4709.base? 1: _temp4711 >= _temp4709.last_plus_one){ _throw(
Null_Exception);}* _temp4711;}).last_line));( struct _xenum_struct*) _temp4702;});
goto _LL4674; _LL4674:;} break; case 217: _LL4669: yyval=({ struct Cyc_BlockItem_tok_struct*
_temp4713=( struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));
_temp4713->tag= Cyc_BlockItem_tok_tag; _temp4713->f1=( void*)({ struct Cyc_Parse_TopDecls_bl_struct*
_temp4714=( struct Cyc_Parse_TopDecls_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_TopDecls_bl_struct));
_temp4714->tag= Cyc_Parse_TopDecls_bl_tag; _temp4714->f1= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp4715= yyvs; struct _xenum_struct** _temp4717= _temp4715.curr
+ yyvsp_offset; if( _temp4717 < _temp4715.base? 1: _temp4717 >= _temp4715.last_plus_one){
_throw( Null_Exception);}* _temp4717;}));( void*) _temp4714;});( struct
_xenum_struct*) _temp4713;}); break; case 218: _LL4712: yyval=({ struct Cyc_BlockItem_tok_struct*
_temp4719=( struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));
_temp4719->tag= Cyc_BlockItem_tok_tag; _temp4719->f1=( void*)({ struct Cyc_Parse_Stmt_bl_struct*
_temp4720=( struct Cyc_Parse_Stmt_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_Stmt_bl_struct));
_temp4720->tag= Cyc_Parse_Stmt_bl_tag; _temp4720->f1= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp4721= yyvs; struct _xenum_struct** _temp4723= _temp4721.curr
+ yyvsp_offset; if( _temp4723 < _temp4721.base? 1: _temp4723 >= _temp4721.last_plus_one){
_throw( Null_Exception);}* _temp4723;}));( void*) _temp4720;});( struct
_xenum_struct*) _temp4719;}); break; case 219: _LL4718: yyval=({ struct Cyc_BlockItem_tok_struct*
_temp4725=( struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));
_temp4725->tag= Cyc_BlockItem_tok_tag; _temp4725->f1=( void*)({ struct Cyc_Parse_FnDecl_bl_struct*
_temp4726=( struct Cyc_Parse_FnDecl_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_FnDecl_bl_struct));
_temp4726->tag= Cyc_Parse_FnDecl_bl_tag; _temp4726->f1= Cyc_yyget_FnDecl_tok(({
struct _tagged_ptr2 _temp4727= yyvs; struct _xenum_struct** _temp4729= _temp4727.curr
+ yyvsp_offset; if( _temp4729 < _temp4727.base? 1: _temp4729 >= _temp4727.last_plus_one){
_throw( Null_Exception);}* _temp4729;}));( void*) _temp4726;});( struct
_xenum_struct*) _temp4725;}); break; case 220: _LL4724: yyval=({ struct Cyc_Stmt_tok_struct*
_temp4731=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp4731->tag= Cyc_Stmt_tok_tag; _temp4731->f1= Cyc_Absyn_ifthenelse_stmt( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4732= yyvs; struct _xenum_struct** _temp4734= _temp4732.curr
+( yyvsp_offset - 2); if( _temp4734 < _temp4732.base? 1: _temp4734 >= _temp4732.last_plus_one){
_throw( Null_Exception);}* _temp4734;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp4735= yyvs; struct _xenum_struct** _temp4737= _temp4735.curr +
yyvsp_offset; if( _temp4737 < _temp4735.base? 1: _temp4737 >= _temp4735.last_plus_one){
_throw( Null_Exception);}* _temp4737;})), Cyc_Absyn_skip_stmt( 0), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4738= yyls; struct Cyc_Yyltype* _temp4740= _temp4738.curr
+( yylsp_offset - 4); if( _temp4740 < _temp4738.base? 1: _temp4740 >= _temp4738.last_plus_one){
_throw( Null_Exception);}* _temp4740;}).first_line,({ struct _tagged_ptr3
_temp4741= yyls; struct Cyc_Yyltype* _temp4743= _temp4741.curr + yylsp_offset;
if( _temp4743 < _temp4741.base? 1: _temp4743 >= _temp4741.last_plus_one){ _throw(
Null_Exception);}* _temp4743;}).last_line));( struct _xenum_struct*) _temp4731;});
break; case 221: _LL4730: yyval=({ struct Cyc_Stmt_tok_struct* _temp4745=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp4745->tag= Cyc_Stmt_tok_tag; _temp4745->f1= Cyc_Absyn_ifthenelse_stmt( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4746= yyvs; struct _xenum_struct** _temp4748= _temp4746.curr
+( yyvsp_offset - 4); if( _temp4748 < _temp4746.base? 1: _temp4748 >= _temp4746.last_plus_one){
_throw( Null_Exception);}* _temp4748;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp4749= yyvs; struct _xenum_struct** _temp4751= _temp4749.curr +(
yyvsp_offset - 2); if( _temp4751 < _temp4749.base? 1: _temp4751 >= _temp4749.last_plus_one){
_throw( Null_Exception);}* _temp4751;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp4752= yyvs; struct _xenum_struct** _temp4754= _temp4752.curr +
yyvsp_offset; if( _temp4754 < _temp4752.base? 1: _temp4754 >= _temp4752.last_plus_one){
_throw( Null_Exception);}* _temp4754;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4755= yyls; struct Cyc_Yyltype* _temp4757= _temp4755.curr +(
yylsp_offset - 6); if( _temp4757 < _temp4755.base? 1: _temp4757 >= _temp4755.last_plus_one){
_throw( Null_Exception);}* _temp4757;}).first_line,({ struct _tagged_ptr3
_temp4758= yyls; struct Cyc_Yyltype* _temp4760= _temp4758.curr + yylsp_offset;
if( _temp4760 < _temp4758.base? 1: _temp4760 >= _temp4758.last_plus_one){ _throw(
Null_Exception);}* _temp4760;}).last_line));( struct _xenum_struct*) _temp4745;});
break; case 222: _LL4744: yyval=({ struct Cyc_Stmt_tok_struct* _temp4762=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp4762->tag= Cyc_Stmt_tok_tag; _temp4762->f1= Cyc_Absyn_switch_stmt( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4763= yyvs; struct _xenum_struct** _temp4765= _temp4763.curr
+( yyvsp_offset - 4); if( _temp4765 < _temp4763.base? 1: _temp4765 >= _temp4763.last_plus_one){
_throw( Null_Exception);}* _temp4765;})), Cyc_yyget_SwitchClauseList_tok(({
struct _tagged_ptr2 _temp4766= yyvs; struct _xenum_struct** _temp4768= _temp4766.curr
+( yyvsp_offset - 1); if( _temp4768 < _temp4766.base? 1: _temp4768 >= _temp4766.last_plus_one){
_throw( Null_Exception);}* _temp4768;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4769= yyls; struct Cyc_Yyltype* _temp4771= _temp4769.curr +(
yylsp_offset - 6); if( _temp4771 < _temp4769.base? 1: _temp4771 >= _temp4769.last_plus_one){
_throw( Null_Exception);}* _temp4771;}).first_line,({ struct _tagged_ptr3
_temp4772= yyls; struct Cyc_Yyltype* _temp4774= _temp4772.curr + yylsp_offset;
if( _temp4774 < _temp4772.base? 1: _temp4774 >= _temp4772.last_plus_one){ _throw(
Null_Exception);}* _temp4774;}).last_line));( struct _xenum_struct*) _temp4762;});
break; case 223: _LL4761: yyval=({ struct Cyc_Stmt_tok_struct* _temp4776=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp4776->tag= Cyc_Stmt_tok_tag; _temp4776->f1= Cyc_Absyn_trycatch_stmt( Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp4777= yyvs; struct _xenum_struct** _temp4779= _temp4777.curr
+( yyvsp_offset - 4); if( _temp4779 < _temp4777.base? 1: _temp4779 >= _temp4777.last_plus_one){
_throw( Null_Exception);}* _temp4779;})), Cyc_yyget_SwitchClauseList_tok(({
struct _tagged_ptr2 _temp4780= yyvs; struct _xenum_struct** _temp4782= _temp4780.curr
+( yyvsp_offset - 1); if( _temp4782 < _temp4780.base? 1: _temp4782 >= _temp4780.last_plus_one){
_throw( Null_Exception);}* _temp4782;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4783= yyls; struct Cyc_Yyltype* _temp4785= _temp4783.curr +(
yylsp_offset - 5); if( _temp4785 < _temp4783.base? 1: _temp4785 >= _temp4783.last_plus_one){
_throw( Null_Exception);}* _temp4785;}).first_line,({ struct _tagged_ptr3
_temp4786= yyls; struct Cyc_Yyltype* _temp4788= _temp4786.curr + yylsp_offset;
if( _temp4788 < _temp4786.base? 1: _temp4788 >= _temp4786.last_plus_one){ _throw(
Null_Exception);}* _temp4788;}).last_line));( struct _xenum_struct*) _temp4776;});
break; case 224: _LL4775: yyval=({ struct Cyc_SwitchClauseList_tok_struct*
_temp4790=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp4790->tag= Cyc_SwitchClauseList_tok_tag;
_temp4790->f1= 0;( struct _xenum_struct*) _temp4790;}); break; case 225: _LL4789:
yyval=({ struct Cyc_SwitchClauseList_tok_struct* _temp4792=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp4792->tag= Cyc_SwitchClauseList_tok_tag;
_temp4792->f1=({ struct Cyc_List_List* _temp4793=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4793->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp4794=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp4794->pattern= Cyc_Absyn_new_pat( Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4804= yyls; struct Cyc_Yyltype* _temp4806= _temp4804.curr
+( yylsp_offset - 2); if( _temp4806 < _temp4804.base? 1: _temp4806 >= _temp4804.last_plus_one){
_throw( Null_Exception);}* _temp4806;}).first_line,({ struct _tagged_ptr3
_temp4807= yyls; struct Cyc_Yyltype* _temp4809= _temp4807.curr +( yylsp_offset -
2); if( _temp4809 < _temp4807.base? 1: _temp4809 >= _temp4807.last_plus_one){
_throw( Null_Exception);}* _temp4809;}).last_line)); _temp4794->pat_vars= 0;
_temp4794->where_clause= 0; _temp4794->body= Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp4801= yyvs; struct _xenum_struct** _temp4803= _temp4801.curr +
yyvsp_offset; if( _temp4803 < _temp4801.base? 1: _temp4803 >= _temp4801.last_plus_one){
_throw( Null_Exception);}* _temp4803;})); _temp4794->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4795= yyls; struct Cyc_Yyltype* _temp4797= _temp4795.curr
+( yylsp_offset - 2); if( _temp4797 < _temp4795.base? 1: _temp4797 >= _temp4795.last_plus_one){
_throw( Null_Exception);}* _temp4797;}).first_line,({ struct _tagged_ptr3
_temp4798= yyls; struct Cyc_Yyltype* _temp4800= _temp4798.curr + yylsp_offset;
if( _temp4800 < _temp4798.base? 1: _temp4800 >= _temp4798.last_plus_one){ _throw(
Null_Exception);}* _temp4800;}).last_line); _temp4794;}); _temp4793->tl= 0;
_temp4793;});( struct _xenum_struct*) _temp4792;}); break; case 226: _LL4791:
yyval=({ struct Cyc_SwitchClauseList_tok_struct* _temp4811=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp4811->tag= Cyc_SwitchClauseList_tok_tag;
_temp4811->f1=({ struct Cyc_List_List* _temp4812=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4812->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp4816=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp4816->pattern= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp4829= yyvs;
struct _xenum_struct** _temp4831= _temp4829.curr +( yyvsp_offset - 2); if(
_temp4831 < _temp4829.base? 1: _temp4831 >= _temp4829.last_plus_one){ _throw(
Null_Exception);}* _temp4831;})); _temp4816->pat_vars= 0; _temp4816->where_clause=
0; _temp4816->body= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4823= yyls; struct Cyc_Yyltype* _temp4825= _temp4823.curr +(
yylsp_offset - 1); if( _temp4825 < _temp4823.base? 1: _temp4825 >= _temp4823.last_plus_one){
_throw( Null_Exception);}* _temp4825;}).first_line,({ struct _tagged_ptr3
_temp4826= yyls; struct Cyc_Yyltype* _temp4828= _temp4826.curr +( yylsp_offset -
1); if( _temp4828 < _temp4826.base? 1: _temp4828 >= _temp4826.last_plus_one){
_throw( Null_Exception);}* _temp4828;}).last_line)); _temp4816->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4817= yyls; struct Cyc_Yyltype* _temp4819= _temp4817.curr
+( yylsp_offset - 3); if( _temp4819 < _temp4817.base? 1: _temp4819 >= _temp4817.last_plus_one){
_throw( Null_Exception);}* _temp4819;}).first_line,({ struct _tagged_ptr3
_temp4820= yyls; struct Cyc_Yyltype* _temp4822= _temp4820.curr + yylsp_offset;
if( _temp4822 < _temp4820.base? 1: _temp4822 >= _temp4820.last_plus_one){ _throw(
Null_Exception);}* _temp4822;}).last_line); _temp4816;}); _temp4812->tl= Cyc_yyget_SwitchClauseList_tok(({
struct _tagged_ptr2 _temp4813= yyvs; struct _xenum_struct** _temp4815= _temp4813.curr
+ yyvsp_offset; if( _temp4815 < _temp4813.base? 1: _temp4815 >= _temp4813.last_plus_one){
_throw( Null_Exception);}* _temp4815;})); _temp4812;});( struct _xenum_struct*)
_temp4811;}); break; case 227: _LL4810: yyval=({ struct Cyc_SwitchClauseList_tok_struct*
_temp4833=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp4833->tag= Cyc_SwitchClauseList_tok_tag;
_temp4833->f1=({ struct Cyc_List_List* _temp4834=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4834->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp4838=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp4838->pattern= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp4848= yyvs;
struct _xenum_struct** _temp4850= _temp4848.curr +( yyvsp_offset - 3); if(
_temp4850 < _temp4848.base? 1: _temp4850 >= _temp4848.last_plus_one){ _throw(
Null_Exception);}* _temp4850;})); _temp4838->pat_vars= 0; _temp4838->where_clause=
0; _temp4838->body= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4845= yyvs;
struct _xenum_struct** _temp4847= _temp4845.curr +( yyvsp_offset - 1); if(
_temp4847 < _temp4845.base? 1: _temp4847 >= _temp4845.last_plus_one){ _throw(
Null_Exception);}* _temp4847;})); _temp4838->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4839= yyls; struct Cyc_Yyltype* _temp4841= _temp4839.curr
+( yylsp_offset - 4); if( _temp4841 < _temp4839.base? 1: _temp4841 >= _temp4839.last_plus_one){
_throw( Null_Exception);}* _temp4841;}).first_line,({ struct _tagged_ptr3
_temp4842= yyls; struct Cyc_Yyltype* _temp4844= _temp4842.curr +( yylsp_offset -
1); if( _temp4844 < _temp4842.base? 1: _temp4844 >= _temp4842.last_plus_one){
_throw( Null_Exception);}* _temp4844;}).last_line); _temp4838;}); _temp4834->tl=
Cyc_yyget_SwitchClauseList_tok(({ struct _tagged_ptr2 _temp4835= yyvs; struct
_xenum_struct** _temp4837= _temp4835.curr + yyvsp_offset; if( _temp4837 <
_temp4835.base? 1: _temp4837 >= _temp4835.last_plus_one){ _throw( Null_Exception);}*
_temp4837;})); _temp4834;});( struct _xenum_struct*) _temp4833;}); break; case
228: _LL4832: yyval=({ struct Cyc_SwitchClauseList_tok_struct* _temp4852=(
struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp4852->tag= Cyc_SwitchClauseList_tok_tag; _temp4852->f1=({ struct Cyc_List_List*
_temp4853=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4853->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp4857=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp4857->pattern= Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp4873= yyvs; struct _xenum_struct** _temp4875= _temp4873.curr
+( yyvsp_offset - 4); if( _temp4875 < _temp4873.base? 1: _temp4875 >= _temp4873.last_plus_one){
_throw( Null_Exception);}* _temp4875;})); _temp4857->pat_vars= 0; _temp4857->where_clause=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4870= yyvs;
struct _xenum_struct** _temp4872= _temp4870.curr +( yyvsp_offset - 2); if(
_temp4872 < _temp4870.base? 1: _temp4872 >= _temp4870.last_plus_one){ _throw(
Null_Exception);}* _temp4872;})); _temp4857->body= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4864= yyls; struct Cyc_Yyltype* _temp4866= _temp4864.curr
+( yylsp_offset - 1); if( _temp4866 < _temp4864.base? 1: _temp4866 >= _temp4864.last_plus_one){
_throw( Null_Exception);}* _temp4866;}).first_line,({ struct _tagged_ptr3
_temp4867= yyls; struct Cyc_Yyltype* _temp4869= _temp4867.curr +( yylsp_offset -
1); if( _temp4869 < _temp4867.base? 1: _temp4869 >= _temp4867.last_plus_one){
_throw( Null_Exception);}* _temp4869;}).last_line)); _temp4857->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4858= yyls; struct Cyc_Yyltype* _temp4860= _temp4858.curr
+( yylsp_offset - 5); if( _temp4860 < _temp4858.base? 1: _temp4860 >= _temp4858.last_plus_one){
_throw( Null_Exception);}* _temp4860;}).first_line,({ struct _tagged_ptr3
_temp4861= yyls; struct Cyc_Yyltype* _temp4863= _temp4861.curr + yylsp_offset;
if( _temp4863 < _temp4861.base? 1: _temp4863 >= _temp4861.last_plus_one){ _throw(
Null_Exception);}* _temp4863;}).last_line); _temp4857;}); _temp4853->tl= Cyc_yyget_SwitchClauseList_tok(({
struct _tagged_ptr2 _temp4854= yyvs; struct _xenum_struct** _temp4856= _temp4854.curr
+ yyvsp_offset; if( _temp4856 < _temp4854.base? 1: _temp4856 >= _temp4854.last_plus_one){
_throw( Null_Exception);}* _temp4856;})); _temp4853;});( struct _xenum_struct*)
_temp4852;}); break; case 229: _LL4851: yyval=({ struct Cyc_SwitchClauseList_tok_struct*
_temp4877=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp4877->tag= Cyc_SwitchClauseList_tok_tag;
_temp4877->f1=({ struct Cyc_List_List* _temp4878=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4878->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp4882=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp4882->pattern= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp4895= yyvs;
struct _xenum_struct** _temp4897= _temp4895.curr +( yyvsp_offset - 5); if(
_temp4897 < _temp4895.base? 1: _temp4897 >= _temp4895.last_plus_one){ _throw(
Null_Exception);}* _temp4897;})); _temp4882->pat_vars= 0; _temp4882->where_clause=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4892= yyvs;
struct _xenum_struct** _temp4894= _temp4892.curr +( yyvsp_offset - 3); if(
_temp4894 < _temp4892.base? 1: _temp4894 >= _temp4892.last_plus_one){ _throw(
Null_Exception);}* _temp4894;})); _temp4882->body= Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp4889= yyvs; struct _xenum_struct** _temp4891= _temp4889.curr +(
yyvsp_offset - 1); if( _temp4891 < _temp4889.base? 1: _temp4891 >= _temp4889.last_plus_one){
_throw( Null_Exception);}* _temp4891;})); _temp4882->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4883= yyls; struct Cyc_Yyltype* _temp4885= _temp4883.curr
+( yylsp_offset - 6); if( _temp4885 < _temp4883.base? 1: _temp4885 >= _temp4883.last_plus_one){
_throw( Null_Exception);}* _temp4885;}).first_line,({ struct _tagged_ptr3
_temp4886= yyls; struct Cyc_Yyltype* _temp4888= _temp4886.curr + yylsp_offset;
if( _temp4888 < _temp4886.base? 1: _temp4888 >= _temp4886.last_plus_one){ _throw(
Null_Exception);}* _temp4888;}).last_line); _temp4882;}); _temp4878->tl= Cyc_yyget_SwitchClauseList_tok(({
struct _tagged_ptr2 _temp4879= yyvs; struct _xenum_struct** _temp4881= _temp4879.curr
+ yyvsp_offset; if( _temp4881 < _temp4879.base? 1: _temp4881 >= _temp4879.last_plus_one){
_throw( Null_Exception);}* _temp4881;})); _temp4878;});( struct _xenum_struct*)
_temp4877;}); break; case 230: _LL4876: yyval=({ struct Cyc_Stmt_tok_struct*
_temp4899=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp4899->tag= Cyc_Stmt_tok_tag; _temp4899->f1= Cyc_Absyn_while_stmt( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4900= yyvs; struct _xenum_struct** _temp4902= _temp4900.curr
+( yyvsp_offset - 2); if( _temp4902 < _temp4900.base? 1: _temp4902 >= _temp4900.last_plus_one){
_throw( Null_Exception);}* _temp4902;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp4903= yyvs; struct _xenum_struct** _temp4905= _temp4903.curr +
yyvsp_offset; if( _temp4905 < _temp4903.base? 1: _temp4905 >= _temp4903.last_plus_one){
_throw( Null_Exception);}* _temp4905;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4906= yyls; struct Cyc_Yyltype* _temp4908= _temp4906.curr +(
yylsp_offset - 4); if( _temp4908 < _temp4906.base? 1: _temp4908 >= _temp4906.last_plus_one){
_throw( Null_Exception);}* _temp4908;}).first_line,({ struct _tagged_ptr3
_temp4909= yyls; struct Cyc_Yyltype* _temp4911= _temp4909.curr + yylsp_offset;
if( _temp4911 < _temp4909.base? 1: _temp4911 >= _temp4909.last_plus_one){ _throw(
Null_Exception);}* _temp4911;}).last_line));( struct _xenum_struct*) _temp4899;});
break; case 231: _LL4898: yyval=({ struct Cyc_Stmt_tok_struct* _temp4913=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp4913->tag= Cyc_Stmt_tok_tag; _temp4913->f1= Cyc_Absyn_do_stmt( Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp4914= yyvs; struct _xenum_struct** _temp4916= _temp4914.curr
+( yyvsp_offset - 5); if( _temp4916 < _temp4914.base? 1: _temp4916 >= _temp4914.last_plus_one){
_throw( Null_Exception);}* _temp4916;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4917= yyvs; struct _xenum_struct** _temp4919= _temp4917.curr +(
yyvsp_offset - 2); if( _temp4919 < _temp4917.base? 1: _temp4919 >= _temp4917.last_plus_one){
_throw( Null_Exception);}* _temp4919;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4920= yyls; struct Cyc_Yyltype* _temp4922= _temp4920.curr +(
yylsp_offset - 6); if( _temp4922 < _temp4920.base? 1: _temp4922 >= _temp4920.last_plus_one){
_throw( Null_Exception);}* _temp4922;}).first_line,({ struct _tagged_ptr3
_temp4923= yyls; struct Cyc_Yyltype* _temp4925= _temp4923.curr + yylsp_offset;
if( _temp4925 < _temp4923.base? 1: _temp4925 >= _temp4923.last_plus_one){ _throw(
Null_Exception);}* _temp4925;}).last_line));( struct _xenum_struct*) _temp4913;});
break; case 232: _LL4912: yyval=({ struct Cyc_Stmt_tok_struct* _temp4927=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp4927->tag= Cyc_Stmt_tok_tag; _temp4927->f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp4928= yyvs; struct _xenum_struct** _temp4930= _temp4928.curr
+ yyvsp_offset; if( _temp4930 < _temp4928.base? 1: _temp4930 >= _temp4928.last_plus_one){
_throw( Null_Exception);}* _temp4930;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4931= yyls; struct Cyc_Yyltype* _temp4933= _temp4931.curr +(
yylsp_offset - 5); if( _temp4933 < _temp4931.base? 1: _temp4933 >= _temp4931.last_plus_one){
_throw( Null_Exception);}* _temp4933;}).first_line,({ struct _tagged_ptr3
_temp4934= yyls; struct Cyc_Yyltype* _temp4936= _temp4934.curr + yylsp_offset;
if( _temp4936 < _temp4934.base? 1: _temp4936 >= _temp4934.last_plus_one){ _throw(
Null_Exception);}* _temp4936;}).last_line));( struct _xenum_struct*) _temp4927;});
break; case 233: _LL4926: yyval=({ struct Cyc_Stmt_tok_struct* _temp4938=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp4938->tag= Cyc_Stmt_tok_tag; _temp4938->f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4939=
yyvs; struct _xenum_struct** _temp4941= _temp4939.curr +( yyvsp_offset - 2); if(
_temp4941 < _temp4939.base? 1: _temp4941 >= _temp4939.last_plus_one){ _throw(
Null_Exception);}* _temp4941;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp4942= yyvs; struct _xenum_struct** _temp4944= _temp4942.curr + yyvsp_offset;
if( _temp4944 < _temp4942.base? 1: _temp4944 >= _temp4942.last_plus_one){ _throw(
Null_Exception);}* _temp4944;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4945= yyls; struct Cyc_Yyltype* _temp4947= _temp4945.curr +(
yylsp_offset - 6); if( _temp4947 < _temp4945.base? 1: _temp4947 >= _temp4945.last_plus_one){
_throw( Null_Exception);}* _temp4947;}).first_line,({ struct _tagged_ptr3
_temp4948= yyls; struct Cyc_Yyltype* _temp4950= _temp4948.curr + yylsp_offset;
if( _temp4950 < _temp4948.base? 1: _temp4950 >= _temp4948.last_plus_one){ _throw(
Null_Exception);}* _temp4950;}).last_line));( struct _xenum_struct*) _temp4938;});
break; case 234: _LL4937: yyval=({ struct Cyc_Stmt_tok_struct* _temp4952=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp4952->tag= Cyc_Stmt_tok_tag; _temp4952->f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4953= yyvs; struct
_xenum_struct** _temp4955= _temp4953.curr +( yyvsp_offset - 3); if( _temp4955 <
_temp4953.base? 1: _temp4955 >= _temp4953.last_plus_one){ _throw( Null_Exception);}*
_temp4955;})), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp4956= yyvs; struct _xenum_struct** _temp4958= _temp4956.curr +
yyvsp_offset; if( _temp4958 < _temp4956.base? 1: _temp4958 >= _temp4956.last_plus_one){
_throw( Null_Exception);}* _temp4958;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4959= yyls; struct Cyc_Yyltype* _temp4961= _temp4959.curr +(
yylsp_offset - 6); if( _temp4961 < _temp4959.base? 1: _temp4961 >= _temp4959.last_plus_one){
_throw( Null_Exception);}* _temp4961;}).first_line,({ struct _tagged_ptr3
_temp4962= yyls; struct Cyc_Yyltype* _temp4964= _temp4962.curr + yylsp_offset;
if( _temp4964 < _temp4962.base? 1: _temp4964 >= _temp4962.last_plus_one){ _throw(
Null_Exception);}* _temp4964;}).last_line));( struct _xenum_struct*) _temp4952;});
break; case 235: _LL4951: yyval=({ struct Cyc_Stmt_tok_struct* _temp4966=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp4966->tag= Cyc_Stmt_tok_tag; _temp4966->f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4967= yyvs; struct
_xenum_struct** _temp4969= _temp4967.curr +( yyvsp_offset - 4); if( _temp4969 <
_temp4967.base? 1: _temp4969 >= _temp4967.last_plus_one){ _throw( Null_Exception);}*
_temp4969;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4970= yyvs; struct
_xenum_struct** _temp4972= _temp4970.curr +( yyvsp_offset - 2); if( _temp4972 <
_temp4970.base? 1: _temp4972 >= _temp4970.last_plus_one){ _throw( Null_Exception);}*
_temp4972;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4973= yyvs; struct
_xenum_struct** _temp4975= _temp4973.curr + yyvsp_offset; if( _temp4975 <
_temp4973.base? 1: _temp4975 >= _temp4973.last_plus_one){ _throw( Null_Exception);}*
_temp4975;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4976=
yyls; struct Cyc_Yyltype* _temp4978= _temp4976.curr +( yylsp_offset - 7); if(
_temp4978 < _temp4976.base? 1: _temp4978 >= _temp4976.last_plus_one){ _throw(
Null_Exception);}* _temp4978;}).first_line,({ struct _tagged_ptr3 _temp4979=
yyls; struct Cyc_Yyltype* _temp4981= _temp4979.curr +( yylsp_offset - 1); if(
_temp4981 < _temp4979.base? 1: _temp4981 >= _temp4979.last_plus_one){ _throw(
Null_Exception);}* _temp4981;}).last_line));( struct _xenum_struct*) _temp4966;});
break; case 236: _LL4965: yyval=({ struct Cyc_Stmt_tok_struct* _temp4983=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp4983->tag= Cyc_Stmt_tok_tag; _temp4983->f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4984= yyvs; struct _xenum_struct** _temp4986= _temp4984.curr
+( yyvsp_offset - 4); if( _temp4986 < _temp4984.base? 1: _temp4986 >= _temp4984.last_plus_one){
_throw( Null_Exception);}* _temp4986;})), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4987= yyvs; struct
_xenum_struct** _temp4989= _temp4987.curr + yyvsp_offset; if( _temp4989 <
_temp4987.base? 1: _temp4989 >= _temp4987.last_plus_one){ _throw( Null_Exception);}*
_temp4989;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4990=
yyls; struct Cyc_Yyltype* _temp4992= _temp4990.curr +( yylsp_offset - 6); if(
_temp4992 < _temp4990.base? 1: _temp4992 >= _temp4990.last_plus_one){ _throw(
Null_Exception);}* _temp4992;}).first_line,({ struct _tagged_ptr3 _temp4993=
yyls; struct Cyc_Yyltype* _temp4995= _temp4993.curr + yylsp_offset; if(
_temp4995 < _temp4993.base? 1: _temp4995 >= _temp4993.last_plus_one){ _throw(
Null_Exception);}* _temp4995;}).last_line));( struct _xenum_struct*) _temp4983;});
break; case 237: _LL4982: yyval=({ struct Cyc_Stmt_tok_struct* _temp4997=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp4997->tag= Cyc_Stmt_tok_tag; _temp4997->f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4998= yyvs; struct _xenum_struct** _temp5000= _temp4998.curr
+( yyvsp_offset - 5); if( _temp5000 < _temp4998.base? 1: _temp5000 >= _temp4998.last_plus_one){
_throw( Null_Exception);}* _temp5000;})), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5001= yyvs; struct _xenum_struct** _temp5003= _temp5001.curr
+( yyvsp_offset - 2); if( _temp5003 < _temp5001.base? 1: _temp5003 >= _temp5001.last_plus_one){
_throw( Null_Exception);}* _temp5003;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp5004= yyvs; struct _xenum_struct** _temp5006= _temp5004.curr +
yyvsp_offset; if( _temp5006 < _temp5004.base? 1: _temp5006 >= _temp5004.last_plus_one){
_throw( Null_Exception);}* _temp5006;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5007= yyls; struct Cyc_Yyltype* _temp5009= _temp5007.curr +(
yylsp_offset - 7); if( _temp5009 < _temp5007.base? 1: _temp5009 >= _temp5007.last_plus_one){
_throw( Null_Exception);}* _temp5009;}).first_line,({ struct _tagged_ptr3
_temp5010= yyls; struct Cyc_Yyltype* _temp5012= _temp5010.curr + yylsp_offset;
if( _temp5012 < _temp5010.base? 1: _temp5012 >= _temp5010.last_plus_one){ _throw(
Null_Exception);}* _temp5012;}).last_line));( struct _xenum_struct*) _temp4997;});
break; case 238: _LL4996: yyval=({ struct Cyc_Stmt_tok_struct* _temp5014=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5014->tag= Cyc_Stmt_tok_tag; _temp5014->f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5015= yyvs; struct _xenum_struct** _temp5017= _temp5015.curr
+( yyvsp_offset - 5); if( _temp5017 < _temp5015.base? 1: _temp5017 >= _temp5015.last_plus_one){
_throw( Null_Exception);}* _temp5017;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5018= yyvs; struct _xenum_struct** _temp5020= _temp5018.curr +(
yyvsp_offset - 3); if( _temp5020 < _temp5018.base? 1: _temp5020 >= _temp5018.last_plus_one){
_throw( Null_Exception);}* _temp5020;})), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5021= yyvs; struct _xenum_struct** _temp5023= _temp5021.curr
+ yyvsp_offset; if( _temp5023 < _temp5021.base? 1: _temp5023 >= _temp5021.last_plus_one){
_throw( Null_Exception);}* _temp5023;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5024= yyls; struct Cyc_Yyltype* _temp5026= _temp5024.curr +(
yylsp_offset - 7); if( _temp5026 < _temp5024.base? 1: _temp5026 >= _temp5024.last_plus_one){
_throw( Null_Exception);}* _temp5026;}).first_line,({ struct _tagged_ptr3
_temp5027= yyls; struct Cyc_Yyltype* _temp5029= _temp5027.curr + yylsp_offset;
if( _temp5029 < _temp5027.base? 1: _temp5029 >= _temp5027.last_plus_one){ _throw(
Null_Exception);}* _temp5029;}).last_line));( struct _xenum_struct*) _temp5014;});
break; case 239: _LL5013: yyval=({ struct Cyc_Stmt_tok_struct* _temp5031=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5031->tag= Cyc_Stmt_tok_tag; _temp5031->f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5032= yyvs; struct _xenum_struct** _temp5034= _temp5032.curr
+( yyvsp_offset - 6); if( _temp5034 < _temp5032.base? 1: _temp5034 >= _temp5032.last_plus_one){
_throw( Null_Exception);}* _temp5034;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5035= yyvs; struct _xenum_struct** _temp5037= _temp5035.curr +(
yyvsp_offset - 4); if( _temp5037 < _temp5035.base? 1: _temp5037 >= _temp5035.last_plus_one){
_throw( Null_Exception);}* _temp5037;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5038= yyvs; struct _xenum_struct** _temp5040= _temp5038.curr +(
yyvsp_offset - 2); if( _temp5040 < _temp5038.base? 1: _temp5040 >= _temp5038.last_plus_one){
_throw( Null_Exception);}* _temp5040;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp5041= yyvs; struct _xenum_struct** _temp5043= _temp5041.curr +
yyvsp_offset; if( _temp5043 < _temp5041.base? 1: _temp5043 >= _temp5041.last_plus_one){
_throw( Null_Exception);}* _temp5043;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5044= yyls; struct Cyc_Yyltype* _temp5046= _temp5044.curr +(
yylsp_offset - 8); if( _temp5046 < _temp5044.base? 1: _temp5046 >= _temp5044.last_plus_one){
_throw( Null_Exception);}* _temp5046;}).first_line,({ struct _tagged_ptr3
_temp5047= yyls; struct Cyc_Yyltype* _temp5049= _temp5047.curr + yylsp_offset;
if( _temp5049 < _temp5047.base? 1: _temp5049 >= _temp5047.last_plus_one){ _throw(
Null_Exception);}* _temp5049;}).last_line));( struct _xenum_struct*) _temp5031;});
break; case 240: _LL5030: { struct Cyc_List_List* decls= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp5051= yyvs; struct _xenum_struct** _temp5053= _temp5051.curr
+( yyvsp_offset - 3); if( _temp5053 < _temp5051.base? 1: _temp5053 >= _temp5051.last_plus_one){
_throw( Null_Exception);}* _temp5053;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5054= yyvs; struct _xenum_struct** _temp5056= _temp5054.curr
+ yyvsp_offset; if( _temp5056 < _temp5054.base? 1: _temp5056 >= _temp5054.last_plus_one){
_throw( Null_Exception);}* _temp5056;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5057= yyls; struct Cyc_Yyltype* _temp5059= _temp5057.curr +(
yylsp_offset - 5); if( _temp5059 < _temp5057.base? 1: _temp5059 >= _temp5057.last_plus_one){
_throw( Null_Exception);}* _temp5059;}).first_line,({ struct _tagged_ptr3
_temp5060= yyls; struct Cyc_Yyltype* _temp5062= _temp5060.curr + yylsp_offset;
if( _temp5062 < _temp5060.base? 1: _temp5062 >= _temp5060.last_plus_one){ _throw(
Null_Exception);}* _temp5062;}).last_line)); yyval=({ struct Cyc_Stmt_tok_struct*
_temp5063=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5063->tag= Cyc_Stmt_tok_tag; _temp5063->f1= Cyc_Parse_flatten_declarations(
decls, s);( struct _xenum_struct*) _temp5063;}); break;} case 241: _LL5050: {
struct Cyc_List_List* decls= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2
_temp5065= yyvs; struct _xenum_struct** _temp5067= _temp5065.curr +(
yyvsp_offset - 4); if( _temp5067 < _temp5065.base? 1: _temp5067 >= _temp5065.last_plus_one){
_throw( Null_Exception);}* _temp5067;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5068=
yyvs; struct _xenum_struct** _temp5070= _temp5068.curr +( yyvsp_offset - 3); if(
_temp5070 < _temp5068.base? 1: _temp5070 >= _temp5068.last_plus_one){ _throw(
Null_Exception);}* _temp5070;})), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5071= yyvs; struct _xenum_struct** _temp5073= _temp5071.curr
+ yyvsp_offset; if( _temp5073 < _temp5071.base? 1: _temp5073 >= _temp5071.last_plus_one){
_throw( Null_Exception);}* _temp5073;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5074= yyls; struct Cyc_Yyltype* _temp5076= _temp5074.curr +(
yylsp_offset - 6); if( _temp5076 < _temp5074.base? 1: _temp5076 >= _temp5074.last_plus_one){
_throw( Null_Exception);}* _temp5076;}).first_line,({ struct _tagged_ptr3
_temp5077= yyls; struct Cyc_Yyltype* _temp5079= _temp5077.curr + yylsp_offset;
if( _temp5079 < _temp5077.base? 1: _temp5079 >= _temp5077.last_plus_one){ _throw(
Null_Exception);}* _temp5079;}).last_line)); yyval=({ struct Cyc_Stmt_tok_struct*
_temp5080=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5080->tag= Cyc_Stmt_tok_tag; _temp5080->f1= Cyc_Parse_flatten_declarations(
decls, s);( struct _xenum_struct*) _temp5080;}); break;} case 242: _LL5064: {
struct Cyc_List_List* decls= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2
_temp5082= yyvs; struct _xenum_struct** _temp5084= _temp5082.curr +(
yyvsp_offset - 4); if( _temp5084 < _temp5082.base? 1: _temp5084 >= _temp5082.last_plus_one){
_throw( Null_Exception);}* _temp5084;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5085= yyvs; struct _xenum_struct** _temp5087= _temp5085.curr +(
yyvsp_offset - 2); if( _temp5087 < _temp5085.base? 1: _temp5087 >= _temp5085.last_plus_one){
_throw( Null_Exception);}* _temp5087;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp5088= yyvs; struct _xenum_struct** _temp5090= _temp5088.curr +
yyvsp_offset; if( _temp5090 < _temp5088.base? 1: _temp5090 >= _temp5088.last_plus_one){
_throw( Null_Exception);}* _temp5090;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5091= yyls; struct Cyc_Yyltype* _temp5093= _temp5091.curr +(
yylsp_offset - 6); if( _temp5093 < _temp5091.base? 1: _temp5093 >= _temp5091.last_plus_one){
_throw( Null_Exception);}* _temp5093;}).first_line,({ struct _tagged_ptr3
_temp5094= yyls; struct Cyc_Yyltype* _temp5096= _temp5094.curr + yylsp_offset;
if( _temp5096 < _temp5094.base? 1: _temp5096 >= _temp5094.last_plus_one){ _throw(
Null_Exception);}* _temp5096;}).last_line)); yyval=({ struct Cyc_Stmt_tok_struct*
_temp5097=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5097->tag= Cyc_Stmt_tok_tag; _temp5097->f1= Cyc_Parse_flatten_declarations(
decls, s);( struct _xenum_struct*) _temp5097;}); break;} case 243: _LL5081: {
struct Cyc_List_List* decls= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2
_temp5099= yyvs; struct _xenum_struct** _temp5101= _temp5099.curr +(
yyvsp_offset - 5); if( _temp5101 < _temp5099.base? 1: _temp5101 >= _temp5099.last_plus_one){
_throw( Null_Exception);}* _temp5101;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5102=
yyvs; struct _xenum_struct** _temp5104= _temp5102.curr +( yyvsp_offset - 4); if(
_temp5104 < _temp5102.base? 1: _temp5104 >= _temp5102.last_plus_one){ _throw(
Null_Exception);}* _temp5104;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5105= yyvs; struct _xenum_struct** _temp5107= _temp5105.curr +(
yyvsp_offset - 2); if( _temp5107 < _temp5105.base? 1: _temp5107 >= _temp5105.last_plus_one){
_throw( Null_Exception);}* _temp5107;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp5108= yyvs; struct _xenum_struct** _temp5110= _temp5108.curr +
yyvsp_offset; if( _temp5110 < _temp5108.base? 1: _temp5110 >= _temp5108.last_plus_one){
_throw( Null_Exception);}* _temp5110;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5111= yyls; struct Cyc_Yyltype* _temp5113= _temp5111.curr +(
yylsp_offset - 7); if( _temp5113 < _temp5111.base? 1: _temp5113 >= _temp5111.last_plus_one){
_throw( Null_Exception);}* _temp5113;}).first_line,({ struct _tagged_ptr3
_temp5114= yyls; struct Cyc_Yyltype* _temp5116= _temp5114.curr + yylsp_offset;
if( _temp5116 < _temp5114.base? 1: _temp5116 >= _temp5114.last_plus_one){ _throw(
Null_Exception);}* _temp5116;}).last_line)); yyval=({ struct Cyc_Stmt_tok_struct*
_temp5117=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5117->tag= Cyc_Stmt_tok_tag; _temp5117->f1= Cyc_Parse_flatten_declarations(
decls, s);( struct _xenum_struct*) _temp5117;}); break;} case 244: _LL5098:
yyval=({ struct Cyc_Stmt_tok_struct* _temp5119=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5119->tag= Cyc_Stmt_tok_tag;
_temp5119->f1= Cyc_Absyn_goto_stmt(({ struct _tagged_string* _temp5120=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp5120[ 0]=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp5121= yyvs; struct
_xenum_struct** _temp5123= _temp5121.curr +( yyvsp_offset - 1); if( _temp5123 <
_temp5121.base? 1: _temp5123 >= _temp5121.last_plus_one){ _throw( Null_Exception);}*
_temp5123;})); _temp5120;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5124= yyls; struct Cyc_Yyltype* _temp5126= _temp5124.curr +( yylsp_offset -
2); if( _temp5126 < _temp5124.base? 1: _temp5126 >= _temp5124.last_plus_one){
_throw( Null_Exception);}* _temp5126;}).first_line,({ struct _tagged_ptr3
_temp5127= yyls; struct Cyc_Yyltype* _temp5129= _temp5127.curr +( yylsp_offset -
1); if( _temp5129 < _temp5127.base? 1: _temp5129 >= _temp5127.last_plus_one){
_throw( Null_Exception);}* _temp5129;}).last_line));( struct _xenum_struct*)
_temp5119;}); break; case 245: _LL5118: yyval=({ struct Cyc_Stmt_tok_struct*
_temp5131=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5131->tag= Cyc_Stmt_tok_tag; _temp5131->f1= Cyc_Absyn_continue_stmt( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5132= yyls; struct Cyc_Yyltype* _temp5134= _temp5132.curr
+( yylsp_offset - 1); if( _temp5134 < _temp5132.base? 1: _temp5134 >= _temp5132.last_plus_one){
_throw( Null_Exception);}* _temp5134;}).first_line,({ struct _tagged_ptr3
_temp5135= yyls; struct Cyc_Yyltype* _temp5137= _temp5135.curr +( yylsp_offset -
1); if( _temp5137 < _temp5135.base? 1: _temp5137 >= _temp5135.last_plus_one){
_throw( Null_Exception);}* _temp5137;}).last_line));( struct _xenum_struct*)
_temp5131;}); break; case 246: _LL5130: yyval=({ struct Cyc_Stmt_tok_struct*
_temp5139=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5139->tag= Cyc_Stmt_tok_tag; _temp5139->f1= Cyc_Absyn_break_stmt( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5140= yyls; struct Cyc_Yyltype* _temp5142= _temp5140.curr
+( yylsp_offset - 1); if( _temp5142 < _temp5140.base? 1: _temp5142 >= _temp5140.last_plus_one){
_throw( Null_Exception);}* _temp5142;}).first_line,({ struct _tagged_ptr3
_temp5143= yyls; struct Cyc_Yyltype* _temp5145= _temp5143.curr +( yylsp_offset -
1); if( _temp5145 < _temp5143.base? 1: _temp5145 >= _temp5143.last_plus_one){
_throw( Null_Exception);}* _temp5145;}).last_line));( struct _xenum_struct*)
_temp5139;}); break; case 247: _LL5138: yyval=({ struct Cyc_Stmt_tok_struct*
_temp5147=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5147->tag= Cyc_Stmt_tok_tag; _temp5147->f1= Cyc_Absyn_return_stmt( 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5148= yyls; struct Cyc_Yyltype* _temp5150= _temp5148.curr
+( yylsp_offset - 1); if( _temp5150 < _temp5148.base? 1: _temp5150 >= _temp5148.last_plus_one){
_throw( Null_Exception);}* _temp5150;}).first_line,({ struct _tagged_ptr3
_temp5151= yyls; struct Cyc_Yyltype* _temp5153= _temp5151.curr +( yylsp_offset -
1); if( _temp5153 < _temp5151.base? 1: _temp5153 >= _temp5151.last_plus_one){
_throw( Null_Exception);}* _temp5153;}).last_line));( struct _xenum_struct*)
_temp5147;}); break; case 248: _LL5146: yyval=({ struct Cyc_Stmt_tok_struct*
_temp5155=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5155->tag= Cyc_Stmt_tok_tag; _temp5155->f1= Cyc_Absyn_return_stmt(( struct
Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5156= yyvs; struct
_xenum_struct** _temp5158= _temp5156.curr +( yyvsp_offset - 1); if( _temp5158 <
_temp5156.base? 1: _temp5158 >= _temp5156.last_plus_one){ _throw( Null_Exception);}*
_temp5158;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5159=
yyls; struct Cyc_Yyltype* _temp5161= _temp5159.curr +( yylsp_offset - 2); if(
_temp5161 < _temp5159.base? 1: _temp5161 >= _temp5159.last_plus_one){ _throw(
Null_Exception);}* _temp5161;}).first_line,({ struct _tagged_ptr3 _temp5162=
yyls; struct Cyc_Yyltype* _temp5164= _temp5162.curr +( yylsp_offset - 1); if(
_temp5164 < _temp5162.base? 1: _temp5164 >= _temp5162.last_plus_one){ _throw(
Null_Exception);}* _temp5164;}).last_line));( struct _xenum_struct*) _temp5155;});
break; case 249: _LL5154: yyval=({ struct Cyc_Stmt_tok_struct* _temp5166=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5166->tag= Cyc_Stmt_tok_tag; _temp5166->f1= Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5167= yyls; struct Cyc_Yyltype* _temp5169= _temp5167.curr
+( yylsp_offset - 1); if( _temp5169 < _temp5167.base? 1: _temp5169 >= _temp5167.last_plus_one){
_throw( Null_Exception);}* _temp5169;}).first_line,({ struct _tagged_ptr3
_temp5170= yyls; struct Cyc_Yyltype* _temp5172= _temp5170.curr +( yylsp_offset -
1); if( _temp5172 < _temp5170.base? 1: _temp5172 >= _temp5170.last_plus_one){
_throw( Null_Exception);}* _temp5172;}).last_line));( struct _xenum_struct*)
_temp5166;}); break; case 250: _LL5165: yyval=({ struct Cyc_Stmt_tok_struct*
_temp5174=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5174->tag= Cyc_Stmt_tok_tag; _temp5174->f1= Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5175= yyls; struct Cyc_Yyltype* _temp5177= _temp5175.curr
+( yylsp_offset - 3); if( _temp5177 < _temp5175.base? 1: _temp5177 >= _temp5175.last_plus_one){
_throw( Null_Exception);}* _temp5177;}).first_line,({ struct _tagged_ptr3
_temp5178= yyls; struct Cyc_Yyltype* _temp5180= _temp5178.curr +( yylsp_offset -
3); if( _temp5180 < _temp5178.base? 1: _temp5180 >= _temp5178.last_plus_one){
_throw( Null_Exception);}* _temp5180;}).last_line));( struct _xenum_struct*)
_temp5174;}); break; case 251: _LL5173: yyval=({ struct Cyc_Stmt_tok_struct*
_temp5182=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5182->tag= Cyc_Stmt_tok_tag; _temp5182->f1= Cyc_Absyn_fallthru_stmt( Cyc_yyget_ExpList_tok(({
struct _tagged_ptr2 _temp5183= yyvs; struct _xenum_struct** _temp5185= _temp5183.curr
+( yyvsp_offset - 2); if( _temp5185 < _temp5183.base? 1: _temp5185 >= _temp5183.last_plus_one){
_throw( Null_Exception);}* _temp5185;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5186= yyls; struct Cyc_Yyltype* _temp5188= _temp5186.curr +(
yylsp_offset - 4); if( _temp5188 < _temp5186.base? 1: _temp5188 >= _temp5186.last_plus_one){
_throw( Null_Exception);}* _temp5188;}).first_line,({ struct _tagged_ptr3
_temp5189= yyls; struct Cyc_Yyltype* _temp5191= _temp5189.curr +( yylsp_offset -
4); if( _temp5191 < _temp5189.base? 1: _temp5191 >= _temp5189.last_plus_one){
_throw( Null_Exception);}* _temp5191;}).last_line));( struct _xenum_struct*)
_temp5182;}); break; case 252: _LL5181: yyval=({ struct Cyc_Pattern_tok_struct*
_temp5193=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5193->tag= Cyc_Pattern_tok_tag; _temp5193->f1= Cyc_Absyn_new_pat( Cyc_Absyn_Wild_p,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5194= yyls; struct Cyc_Yyltype*
_temp5196= _temp5194.curr + yylsp_offset; if( _temp5196 < _temp5194.base? 1:
_temp5196 >= _temp5194.last_plus_one){ _throw( Null_Exception);}* _temp5196;}).first_line,({
struct _tagged_ptr3 _temp5197= yyls; struct Cyc_Yyltype* _temp5199= _temp5197.curr
+ yylsp_offset; if( _temp5199 < _temp5197.base? 1: _temp5199 >= _temp5197.last_plus_one){
_throw( Null_Exception);}* _temp5199;}).last_line));( struct _xenum_struct*)
_temp5193;}); break; case 253: _LL5192: yyval=({ struct _tagged_ptr2 _temp5201=
yyvs; struct _xenum_struct** _temp5203= _temp5201.curr +( yyvsp_offset - 1); if(
_temp5203 < _temp5201.base? 1: _temp5203 >= _temp5201.last_plus_one){ _throw(
Null_Exception);}* _temp5203;}); break; case 254: _LL5200: yyval=({ struct Cyc_Pattern_tok_struct*
_temp5205=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5205->tag= Cyc_Pattern_tok_tag; _temp5205->f1= Cyc_Absyn_new_pat(({ struct
Cyc_Absyn_Int_p_struct* _temp5206=( struct Cyc_Absyn_Int_p_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Int_p_struct)); _temp5206->tag= Cyc_Absyn_Int_p_tag;
_temp5206->f1=( void*)(* Cyc_yyget_Int_tok(({ struct _tagged_ptr2 _temp5207=
yyvs; struct _xenum_struct** _temp5209= _temp5207.curr + yyvsp_offset; if(
_temp5209 < _temp5207.base? 1: _temp5209 >= _temp5207.last_plus_one){ _throw(
Null_Exception);}* _temp5209;}))).f1; _temp5206->f2=(* Cyc_yyget_Int_tok(({
struct _tagged_ptr2 _temp5210= yyvs; struct _xenum_struct** _temp5212= _temp5210.curr
+ yyvsp_offset; if( _temp5212 < _temp5210.base? 1: _temp5212 >= _temp5210.last_plus_one){
_throw( Null_Exception);}* _temp5212;}))).f2;( void*) _temp5206;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5213= yyls; struct Cyc_Yyltype* _temp5215= _temp5213.curr
+ yylsp_offset; if( _temp5215 < _temp5213.base? 1: _temp5215 >= _temp5213.last_plus_one){
_throw( Null_Exception);}* _temp5215;}).first_line,({ struct _tagged_ptr3
_temp5216= yyls; struct Cyc_Yyltype* _temp5218= _temp5216.curr + yylsp_offset;
if( _temp5218 < _temp5216.base? 1: _temp5218 >= _temp5216.last_plus_one){ _throw(
Null_Exception);}* _temp5218;}).last_line));( struct _xenum_struct*) _temp5205;});
break; case 255: _LL5204: yyval=({ struct Cyc_Pattern_tok_struct* _temp5220=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5220->tag= Cyc_Pattern_tok_tag; _temp5220->f1= Cyc_Absyn_new_pat(({ struct
Cyc_Absyn_Int_p_struct* _temp5221=( struct Cyc_Absyn_Int_p_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Int_p_struct)); _temp5221->tag= Cyc_Absyn_Int_p_tag;
_temp5221->f1=( void*) Cyc_Absyn_Signed; _temp5221->f2= -(* Cyc_yyget_Int_tok(({
struct _tagged_ptr2 _temp5222= yyvs; struct _xenum_struct** _temp5224= _temp5222.curr
+ yyvsp_offset; if( _temp5224 < _temp5222.base? 1: _temp5224 >= _temp5222.last_plus_one){
_throw( Null_Exception);}* _temp5224;}))).f2;( void*) _temp5221;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5225= yyls; struct Cyc_Yyltype* _temp5227= _temp5225.curr
+( yylsp_offset - 1); if( _temp5227 < _temp5225.base? 1: _temp5227 >= _temp5225.last_plus_one){
_throw( Null_Exception);}* _temp5227;}).first_line,({ struct _tagged_ptr3
_temp5228= yyls; struct Cyc_Yyltype* _temp5230= _temp5228.curr + yylsp_offset;
if( _temp5230 < _temp5228.base? 1: _temp5230 >= _temp5228.last_plus_one){ _throw(
Null_Exception);}* _temp5230;}).last_line));( struct _xenum_struct*) _temp5220;});
break; case 256: _LL5219: yyval=({ struct Cyc_Pattern_tok_struct* _temp5232=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5232->tag= Cyc_Pattern_tok_tag; _temp5232->f1= Cyc_Absyn_new_pat(({ struct
Cyc_Absyn_Float_p_struct* _temp5233=( struct Cyc_Absyn_Float_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Float_p_struct)); _temp5233->tag= Cyc_Absyn_Float_p_tag;
_temp5233->f1= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp5234= yyvs;
struct _xenum_struct** _temp5236= _temp5234.curr + yyvsp_offset; if( _temp5236 <
_temp5234.base? 1: _temp5236 >= _temp5234.last_plus_one){ _throw( Null_Exception);}*
_temp5236;}));( void*) _temp5233;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5237= yyls; struct Cyc_Yyltype* _temp5239= _temp5237.curr +
yylsp_offset; if( _temp5239 < _temp5237.base? 1: _temp5239 >= _temp5237.last_plus_one){
_throw( Null_Exception);}* _temp5239;}).first_line,({ struct _tagged_ptr3
_temp5240= yyls; struct Cyc_Yyltype* _temp5242= _temp5240.curr + yylsp_offset;
if( _temp5242 < _temp5240.base? 1: _temp5242 >= _temp5240.last_plus_one){ _throw(
Null_Exception);}* _temp5242;}).last_line));( struct _xenum_struct*) _temp5232;});
break; case 257: _LL5231: yyval=({ struct Cyc_Pattern_tok_struct* _temp5244=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5244->tag= Cyc_Pattern_tok_tag; _temp5244->f1= Cyc_Absyn_new_pat(({ struct
Cyc_Absyn_Char_p_struct* _temp5245=( struct Cyc_Absyn_Char_p_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Absyn_Char_p_struct)); _temp5245->tag= Cyc_Absyn_Char_p_tag;
_temp5245->f1= Cyc_yyget_Char_tok(({ struct _tagged_ptr2 _temp5246= yyvs; struct
_xenum_struct** _temp5248= _temp5246.curr + yyvsp_offset; if( _temp5248 <
_temp5246.base? 1: _temp5248 >= _temp5246.last_plus_one){ _throw( Null_Exception);}*
_temp5248;}));( void*) _temp5245;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5249= yyls; struct Cyc_Yyltype* _temp5251= _temp5249.curr +
yylsp_offset; if( _temp5251 < _temp5249.base? 1: _temp5251 >= _temp5249.last_plus_one){
_throw( Null_Exception);}* _temp5251;}).first_line,({ struct _tagged_ptr3
_temp5252= yyls; struct Cyc_Yyltype* _temp5254= _temp5252.curr + yylsp_offset;
if( _temp5254 < _temp5252.base? 1: _temp5254 >= _temp5252.last_plus_one){ _throw(
Null_Exception);}* _temp5254;}).last_line));( struct _xenum_struct*) _temp5244;});
break; case 258: _LL5243: yyval=({ struct Cyc_Pattern_tok_struct* _temp5256=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5256->tag= Cyc_Pattern_tok_tag; _temp5256->f1= Cyc_Absyn_new_pat( Cyc_Absyn_Null_p,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5257= yyls; struct Cyc_Yyltype*
_temp5259= _temp5257.curr + yylsp_offset; if( _temp5259 < _temp5257.base? 1:
_temp5259 >= _temp5257.last_plus_one){ _throw( Null_Exception);}* _temp5259;}).first_line,({
struct _tagged_ptr3 _temp5260= yyls; struct Cyc_Yyltype* _temp5262= _temp5260.curr
+ yylsp_offset; if( _temp5262 < _temp5260.base? 1: _temp5262 >= _temp5260.last_plus_one){
_throw( Null_Exception);}* _temp5262;}).last_line));( struct _xenum_struct*)
_temp5256;}); break; case 259: _LL5255: yyval=({ struct Cyc_Pattern_tok_struct*
_temp5264=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5264->tag= Cyc_Pattern_tok_tag; _temp5264->f1= Cyc_Absyn_new_pat(({ struct
Cyc_Absyn_UnknownId_p_struct* _temp5265=( struct Cyc_Absyn_UnknownId_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_p_struct)); _temp5265->tag= Cyc_Absyn_UnknownId_p_tag;
_temp5265->f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp5266= yyvs;
struct _xenum_struct** _temp5268= _temp5266.curr + yyvsp_offset; if( _temp5268 <
_temp5266.base? 1: _temp5268 >= _temp5266.last_plus_one){ _throw( Null_Exception);}*
_temp5268;}));( void*) _temp5265;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5269= yyls; struct Cyc_Yyltype* _temp5271= _temp5269.curr +
yylsp_offset; if( _temp5271 < _temp5269.base? 1: _temp5271 >= _temp5269.last_plus_one){
_throw( Null_Exception);}* _temp5271;}).first_line,({ struct _tagged_ptr3
_temp5272= yyls; struct Cyc_Yyltype* _temp5274= _temp5272.curr + yylsp_offset;
if( _temp5274 < _temp5272.base? 1: _temp5274 >= _temp5272.last_plus_one){ _throw(
Null_Exception);}* _temp5274;}).last_line));( struct _xenum_struct*) _temp5264;});
break; case 260: _LL5263: { struct Cyc_List_List* tvs=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5276= yyls; struct Cyc_Yyltype* _temp5278= _temp5276.curr
+( yylsp_offset - 3); if( _temp5278 < _temp5276.base? 1: _temp5278 >= _temp5276.last_plus_one){
_throw( Null_Exception);}* _temp5278;}).first_line,({ struct _tagged_ptr3
_temp5279= yyls; struct Cyc_Yyltype* _temp5281= _temp5279.curr +( yylsp_offset -
3); if( _temp5281 < _temp5279.base? 1: _temp5281 >= _temp5279.last_plus_one){
_throw( Null_Exception);}* _temp5281;}).last_line), Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp5282= yyvs; struct _xenum_struct** _temp5284= _temp5282.curr
+( yyvsp_offset - 3); if( _temp5284 < _temp5282.base? 1: _temp5284 >= _temp5282.last_plus_one){
_throw( Null_Exception);}* _temp5284;}))); yyval=({ struct Cyc_Pattern_tok_struct*
_temp5285=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5285->tag= Cyc_Pattern_tok_tag; _temp5285->f1= Cyc_Absyn_new_pat(({ struct
Cyc_Absyn_UnknownCall_p_struct* _temp5286=( struct Cyc_Absyn_UnknownCall_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownCall_p_struct)); _temp5286->tag= Cyc_Absyn_UnknownCall_p_tag;
_temp5286->f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp5287= yyvs;
struct _xenum_struct** _temp5289= _temp5287.curr +( yyvsp_offset - 4); if(
_temp5289 < _temp5287.base? 1: _temp5289 >= _temp5287.last_plus_one){ _throw(
Null_Exception);}* _temp5289;})); _temp5286->f2= tvs; _temp5286->f3= Cyc_yyget_PatternList_tok(({
struct _tagged_ptr2 _temp5290= yyvs; struct _xenum_struct** _temp5292= _temp5290.curr
+( yyvsp_offset - 1); if( _temp5292 < _temp5290.base? 1: _temp5292 >= _temp5290.last_plus_one){
_throw( Null_Exception);}* _temp5292;}));( void*) _temp5286;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5293= yyls; struct Cyc_Yyltype* _temp5295= _temp5293.curr
+( yylsp_offset - 4); if( _temp5295 < _temp5293.base? 1: _temp5295 >= _temp5293.last_plus_one){
_throw( Null_Exception);}* _temp5295;}).first_line,({ struct _tagged_ptr3
_temp5296= yyls; struct Cyc_Yyltype* _temp5298= _temp5296.curr + yylsp_offset;
if( _temp5298 < _temp5296.base? 1: _temp5298 >= _temp5296.last_plus_one){ _throw(
Null_Exception);}* _temp5298;}).last_line));( struct _xenum_struct*) _temp5285;});
break;} case 261: _LL5275: yyval=({ struct Cyc_Pattern_tok_struct* _temp5300=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5300->tag= Cyc_Pattern_tok_tag; _temp5300->f1= Cyc_Absyn_new_pat(({ struct
Cyc_Absyn_Tuple_p_struct* _temp5301=( struct Cyc_Absyn_Tuple_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuple_p_struct)); _temp5301->tag= Cyc_Absyn_Tuple_p_tag;
_temp5301->f1= Cyc_yyget_PatternList_tok(({ struct _tagged_ptr2 _temp5302= yyvs;
struct _xenum_struct** _temp5304= _temp5302.curr +( yyvsp_offset - 1); if(
_temp5304 < _temp5302.base? 1: _temp5304 >= _temp5302.last_plus_one){ _throw(
Null_Exception);}* _temp5304;}));( void*) _temp5301;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5305= yyls; struct Cyc_Yyltype* _temp5307= _temp5305.curr
+( yylsp_offset - 3); if( _temp5307 < _temp5305.base? 1: _temp5307 >= _temp5305.last_plus_one){
_throw( Null_Exception);}* _temp5307;}).first_line,({ struct _tagged_ptr3
_temp5308= yyls; struct Cyc_Yyltype* _temp5310= _temp5308.curr + yylsp_offset;
if( _temp5310 < _temp5308.base? 1: _temp5310 >= _temp5308.last_plus_one){ _throw(
Null_Exception);}* _temp5310;}).last_line));( struct _xenum_struct*) _temp5300;});
break; case 262: _LL5299: { struct Cyc_List_List* tvs=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5312= yyls; struct Cyc_Yyltype* _temp5314= _temp5312.curr
+( yylsp_offset - 2); if( _temp5314 < _temp5312.base? 1: _temp5314 >= _temp5312.last_plus_one){
_throw( Null_Exception);}* _temp5314;}).first_line,({ struct _tagged_ptr3
_temp5315= yyls; struct Cyc_Yyltype* _temp5317= _temp5315.curr +( yylsp_offset -
2); if( _temp5317 < _temp5315.base? 1: _temp5317 >= _temp5315.last_plus_one){
_throw( Null_Exception);}* _temp5317;}).last_line), Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp5318= yyvs; struct _xenum_struct** _temp5320= _temp5318.curr
+( yyvsp_offset - 2); if( _temp5320 < _temp5318.base? 1: _temp5320 >= _temp5318.last_plus_one){
_throw( Null_Exception);}* _temp5320;}))); yyval=({ struct Cyc_Pattern_tok_struct*
_temp5321=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5321->tag= Cyc_Pattern_tok_tag; _temp5321->f1= Cyc_Absyn_new_pat(({ struct
Cyc_Absyn_UnknownFields_p_struct* _temp5322=( struct Cyc_Absyn_UnknownFields_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownFields_p_struct)); _temp5322->tag=
Cyc_Absyn_UnknownFields_p_tag; _temp5322->f1= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp5323= yyvs; struct _xenum_struct** _temp5325= _temp5323.curr +(
yyvsp_offset - 3); if( _temp5325 < _temp5323.base? 1: _temp5325 >= _temp5323.last_plus_one){
_throw( Null_Exception);}* _temp5325;})); _temp5322->f2= tvs; _temp5322->f3= 0;(
void*) _temp5322;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5326= yyls; struct Cyc_Yyltype* _temp5328= _temp5326.curr +( yylsp_offset -
3); if( _temp5328 < _temp5326.base? 1: _temp5328 >= _temp5326.last_plus_one){
_throw( Null_Exception);}* _temp5328;}).first_line,({ struct _tagged_ptr3
_temp5329= yyls; struct Cyc_Yyltype* _temp5331= _temp5329.curr + yylsp_offset;
if( _temp5331 < _temp5329.base? 1: _temp5331 >= _temp5329.last_plus_one){ _throw(
Null_Exception);}* _temp5331;}).last_line));( struct _xenum_struct*) _temp5321;});
break;} case 263: _LL5311: { struct Cyc_List_List* tvs=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5333= yyls; struct Cyc_Yyltype* _temp5335= _temp5333.curr
+( yylsp_offset - 3); if( _temp5335 < _temp5333.base? 1: _temp5335 >= _temp5333.last_plus_one){
_throw( Null_Exception);}* _temp5335;}).first_line,({ struct _tagged_ptr3
_temp5336= yyls; struct Cyc_Yyltype* _temp5338= _temp5336.curr +( yylsp_offset -
3); if( _temp5338 < _temp5336.base? 1: _temp5338 >= _temp5336.last_plus_one){
_throw( Null_Exception);}* _temp5338;}).last_line), Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp5339= yyvs; struct _xenum_struct** _temp5341= _temp5339.curr
+( yyvsp_offset - 3); if( _temp5341 < _temp5339.base? 1: _temp5341 >= _temp5339.last_plus_one){
_throw( Null_Exception);}* _temp5341;}))); yyval=({ struct Cyc_Pattern_tok_struct*
_temp5342=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5342->tag= Cyc_Pattern_tok_tag; _temp5342->f1= Cyc_Absyn_new_pat(({ struct
Cyc_Absyn_UnknownFields_p_struct* _temp5343=( struct Cyc_Absyn_UnknownFields_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownFields_p_struct)); _temp5343->tag=
Cyc_Absyn_UnknownFields_p_tag; _temp5343->f1= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp5344= yyvs; struct _xenum_struct** _temp5346= _temp5344.curr +(
yyvsp_offset - 4); if( _temp5346 < _temp5344.base? 1: _temp5346 >= _temp5344.last_plus_one){
_throw( Null_Exception);}* _temp5346;})); _temp5343->f2= tvs; _temp5343->f3= Cyc_yyget_FieldPatternList_tok(({
struct _tagged_ptr2 _temp5347= yyvs; struct _xenum_struct** _temp5349= _temp5347.curr
+( yyvsp_offset - 1); if( _temp5349 < _temp5347.base? 1: _temp5349 >= _temp5347.last_plus_one){
_throw( Null_Exception);}* _temp5349;}));( void*) _temp5343;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5350= yyls; struct Cyc_Yyltype* _temp5352= _temp5350.curr
+( yylsp_offset - 4); if( _temp5352 < _temp5350.base? 1: _temp5352 >= _temp5350.last_plus_one){
_throw( Null_Exception);}* _temp5352;}).first_line,({ struct _tagged_ptr3
_temp5353= yyls; struct Cyc_Yyltype* _temp5355= _temp5353.curr + yylsp_offset;
if( _temp5355 < _temp5353.base? 1: _temp5355 >= _temp5353.last_plus_one){ _throw(
Null_Exception);}* _temp5355;}).last_line));( struct _xenum_struct*) _temp5342;});
break;} case 264: _LL5332: yyval=({ struct Cyc_Pattern_tok_struct* _temp5357=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5357->tag= Cyc_Pattern_tok_tag; _temp5357->f1= Cyc_Absyn_new_pat(({ struct
Cyc_Absyn_Pointer_p_struct* _temp5358=( struct Cyc_Absyn_Pointer_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_p_struct)); _temp5358->tag= Cyc_Absyn_Pointer_p_tag;
_temp5358->f1= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp5359= yyvs;
struct _xenum_struct** _temp5361= _temp5359.curr + yyvsp_offset; if( _temp5361 <
_temp5359.base? 1: _temp5361 >= _temp5359.last_plus_one){ _throw( Null_Exception);}*
_temp5361;}));( void*) _temp5358;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5362= yyls; struct Cyc_Yyltype* _temp5364= _temp5362.curr +(
yylsp_offset - 1); if( _temp5364 < _temp5362.base? 1: _temp5364 >= _temp5362.last_plus_one){
_throw( Null_Exception);}* _temp5364;}).first_line,({ struct _tagged_ptr3
_temp5365= yyls; struct Cyc_Yyltype* _temp5367= _temp5365.curr + yylsp_offset;
if( _temp5367 < _temp5365.base? 1: _temp5367 >= _temp5365.last_plus_one){ _throw(
Null_Exception);}* _temp5367;}).last_line));( struct _xenum_struct*) _temp5357;});
break; case 265: _LL5356: yyval=({ struct Cyc_Pattern_tok_struct* _temp5369=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5369->tag= Cyc_Pattern_tok_tag; _temp5369->f1= Cyc_Absyn_new_pat(({ struct
Cyc_Absyn_Reference_p_struct* _temp5370=( struct Cyc_Absyn_Reference_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Reference_p_struct)); _temp5370->tag= Cyc_Absyn_Reference_p_tag;
_temp5370->f1= Cyc_Absyn_new_vardecl(({ struct _tuple1* _temp5371=( struct
_tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp5371->f1= Cyc_Absyn_Loc_n;
_temp5371->f2=({ struct _tagged_string* _temp5372=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp5372[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp5373= yyvs; struct _xenum_struct** _temp5375= _temp5373.curr
+ yyvsp_offset; if( _temp5375 < _temp5373.base? 1: _temp5375 >= _temp5373.last_plus_one){
_throw( Null_Exception);}* _temp5375;})); _temp5372;}); _temp5371;}), Cyc_Absyn_VoidType,
0);( void*) _temp5370;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5376= yyls; struct Cyc_Yyltype* _temp5378= _temp5376.curr +( yylsp_offset -
1); if( _temp5378 < _temp5376.base? 1: _temp5378 >= _temp5376.last_plus_one){
_throw( Null_Exception);}* _temp5378;}).first_line,({ struct _tagged_ptr3
_temp5379= yyls; struct Cyc_Yyltype* _temp5381= _temp5379.curr + yylsp_offset;
if( _temp5381 < _temp5379.base? 1: _temp5381 >= _temp5379.last_plus_one){ _throw(
Null_Exception);}* _temp5381;}).last_line));( struct _xenum_struct*) _temp5369;});
break; case 266: _LL5368: yyval=({ struct Cyc_PatternList_tok_struct* _temp5383=(
struct Cyc_PatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));
_temp5383->tag= Cyc_PatternList_tok_tag; _temp5383->f1= 0;( struct _xenum_struct*)
_temp5383;}); break; case 267: _LL5382: yyval=({ struct Cyc_PatternList_tok_struct*
_temp5385=( struct Cyc_PatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));
_temp5385->tag= Cyc_PatternList_tok_tag; _temp5385->f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_PatternList_tok(({ struct
_tagged_ptr2 _temp5386= yyvs; struct _xenum_struct** _temp5388= _temp5386.curr +
yyvsp_offset; if( _temp5388 < _temp5386.base? 1: _temp5388 >= _temp5386.last_plus_one){
_throw( Null_Exception);}* _temp5388;})));( struct _xenum_struct*) _temp5385;});
break; case 268: _LL5384: yyval=({ struct Cyc_PatternList_tok_struct* _temp5390=(
struct Cyc_PatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));
_temp5390->tag= Cyc_PatternList_tok_tag; _temp5390->f1=({ struct Cyc_List_List*
_temp5391=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp5391->hd=( void*) Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp5392=
yyvs; struct _xenum_struct** _temp5394= _temp5392.curr + yyvsp_offset; if(
_temp5394 < _temp5392.base? 1: _temp5394 >= _temp5392.last_plus_one){ _throw(
Null_Exception);}* _temp5394;})); _temp5391->tl= 0; _temp5391;});( struct
_xenum_struct*) _temp5390;}); break; case 269: _LL5389: yyval=({ struct Cyc_PatternList_tok_struct*
_temp5396=( struct Cyc_PatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));
_temp5396->tag= Cyc_PatternList_tok_tag; _temp5396->f1=({ struct Cyc_List_List*
_temp5397=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp5397->hd=( void*) Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp5401=
yyvs; struct _xenum_struct** _temp5403= _temp5401.curr + yyvsp_offset; if(
_temp5403 < _temp5401.base? 1: _temp5403 >= _temp5401.last_plus_one){ _throw(
Null_Exception);}* _temp5403;})); _temp5397->tl= Cyc_yyget_PatternList_tok(({
struct _tagged_ptr2 _temp5398= yyvs; struct _xenum_struct** _temp5400= _temp5398.curr
+( yyvsp_offset - 2); if( _temp5400 < _temp5398.base? 1: _temp5400 >= _temp5398.last_plus_one){
_throw( Null_Exception);}* _temp5400;})); _temp5397;});( struct _xenum_struct*)
_temp5396;}); break; case 270: _LL5395: yyval=({ struct Cyc_FieldPattern_tok_struct*
_temp5405=( struct Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp5405->tag= Cyc_FieldPattern_tok_tag; _temp5405->f1=({ struct _tuple17*
_temp5406=( struct _tuple17*) GC_malloc( sizeof( struct _tuple17)); _temp5406->f1=
0; _temp5406->f2= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp5407= yyvs;
struct _xenum_struct** _temp5409= _temp5407.curr + yyvsp_offset; if( _temp5409 <
_temp5407.base? 1: _temp5409 >= _temp5407.last_plus_one){ _throw( Null_Exception);}*
_temp5409;})); _temp5406;});( struct _xenum_struct*) _temp5405;}); break; case
271: _LL5404: yyval=({ struct Cyc_FieldPattern_tok_struct* _temp5411=( struct
Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp5411->tag= Cyc_FieldPattern_tok_tag; _temp5411->f1=({ struct _tuple17*
_temp5412=( struct _tuple17*) GC_malloc( sizeof( struct _tuple17)); _temp5412->f1=
Cyc_yyget_DesignatorList_tok(({ struct _tagged_ptr2 _temp5416= yyvs; struct
_xenum_struct** _temp5418= _temp5416.curr +( yyvsp_offset - 1); if( _temp5418 <
_temp5416.base? 1: _temp5418 >= _temp5416.last_plus_one){ _throw( Null_Exception);}*
_temp5418;})); _temp5412->f2= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2
_temp5413= yyvs; struct _xenum_struct** _temp5415= _temp5413.curr + yyvsp_offset;
if( _temp5415 < _temp5413.base? 1: _temp5415 >= _temp5413.last_plus_one){ _throw(
Null_Exception);}* _temp5415;})); _temp5412;});( struct _xenum_struct*)
_temp5411;}); break; case 272: _LL5410: yyval=({ struct Cyc_FieldPatternList_tok_struct*
_temp5420=( struct Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct
Cyc_FieldPatternList_tok_struct)); _temp5420->tag= Cyc_FieldPatternList_tok_tag;
_temp5420->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_FieldPatternList_tok(({ struct _tagged_ptr2 _temp5421= yyvs; struct
_xenum_struct** _temp5423= _temp5421.curr + yyvsp_offset; if( _temp5423 <
_temp5421.base? 1: _temp5423 >= _temp5421.last_plus_one){ _throw( Null_Exception);}*
_temp5423;})));( struct _xenum_struct*) _temp5420;}); break; case 273: _LL5419:
yyval=({ struct Cyc_FieldPatternList_tok_struct* _temp5425=( struct Cyc_FieldPatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp5425->tag= Cyc_FieldPatternList_tok_tag;
_temp5425->f1=({ struct Cyc_List_List* _temp5426=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5426->hd=( void*) Cyc_yyget_FieldPattern_tok(({
struct _tagged_ptr2 _temp5427= yyvs; struct _xenum_struct** _temp5429= _temp5427.curr
+ yyvsp_offset; if( _temp5429 < _temp5427.base? 1: _temp5429 >= _temp5427.last_plus_one){
_throw( Null_Exception);}* _temp5429;})); _temp5426->tl= 0; _temp5426;});(
struct _xenum_struct*) _temp5425;}); break; case 274: _LL5424: yyval=({ struct
Cyc_FieldPatternList_tok_struct* _temp5431=( struct Cyc_FieldPatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp5431->tag= Cyc_FieldPatternList_tok_tag;
_temp5431->f1=({ struct Cyc_List_List* _temp5432=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5432->hd=( void*) Cyc_yyget_FieldPattern_tok(({
struct _tagged_ptr2 _temp5436= yyvs; struct _xenum_struct** _temp5438= _temp5436.curr
+ yyvsp_offset; if( _temp5438 < _temp5436.base? 1: _temp5438 >= _temp5436.last_plus_one){
_throw( Null_Exception);}* _temp5438;})); _temp5432->tl= Cyc_yyget_FieldPatternList_tok(({
struct _tagged_ptr2 _temp5433= yyvs; struct _xenum_struct** _temp5435= _temp5433.curr
+( yyvsp_offset - 2); if( _temp5435 < _temp5433.base? 1: _temp5435 >= _temp5433.last_plus_one){
_throw( Null_Exception);}* _temp5435;})); _temp5432;});( struct _xenum_struct*)
_temp5431;}); break; case 275: _LL5430: yyval=({ struct _tagged_ptr2 _temp5440=
yyvs; struct _xenum_struct** _temp5442= _temp5440.curr + yyvsp_offset; if(
_temp5442 < _temp5440.base? 1: _temp5442 >= _temp5440.last_plus_one){ _throw(
Null_Exception);}* _temp5442;}); break; case 276: _LL5439: yyval=({ struct Cyc_Exp_tok_struct*
_temp5444=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp5444->tag= Cyc_Exp_tok_tag; _temp5444->f1= Cyc_Absyn_seq_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5445= yyvs; struct _xenum_struct** _temp5447= _temp5445.curr
+( yyvsp_offset - 2); if( _temp5447 < _temp5445.base? 1: _temp5447 >= _temp5445.last_plus_one){
_throw( Null_Exception);}* _temp5447;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5448= yyvs; struct _xenum_struct** _temp5450= _temp5448.curr +
yyvsp_offset; if( _temp5450 < _temp5448.base? 1: _temp5450 >= _temp5448.last_plus_one){
_throw( Null_Exception);}* _temp5450;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5451= yyls; struct Cyc_Yyltype* _temp5453= _temp5451.curr +(
yylsp_offset - 2); if( _temp5453 < _temp5451.base? 1: _temp5453 >= _temp5451.last_plus_one){
_throw( Null_Exception);}* _temp5453;}).first_line,({ struct _tagged_ptr3
_temp5454= yyls; struct Cyc_Yyltype* _temp5456= _temp5454.curr + yylsp_offset;
if( _temp5456 < _temp5454.base? 1: _temp5456 >= _temp5454.last_plus_one){ _throw(
Null_Exception);}* _temp5456;}).last_line));( struct _xenum_struct*) _temp5444;});
break; case 277: _LL5443: yyval=({ struct _tagged_ptr2 _temp5458= yyvs; struct
_xenum_struct** _temp5460= _temp5458.curr + yyvsp_offset; if( _temp5460 <
_temp5458.base? 1: _temp5460 >= _temp5458.last_plus_one){ _throw( Null_Exception);}*
_temp5460;}); break; case 278: _LL5457: yyval=({ struct Cyc_Exp_tok_struct*
_temp5462=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp5462->tag= Cyc_Exp_tok_tag; _temp5462->f1= Cyc_Absyn_assignop_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5463= yyvs; struct _xenum_struct** _temp5465= _temp5463.curr
+( yyvsp_offset - 2); if( _temp5465 < _temp5463.base? 1: _temp5465 >= _temp5463.last_plus_one){
_throw( Null_Exception);}* _temp5465;})), Cyc_yyget_Primopopt_tok(({ struct
_tagged_ptr2 _temp5466= yyvs; struct _xenum_struct** _temp5468= _temp5466.curr +(
yyvsp_offset - 1); if( _temp5468 < _temp5466.base? 1: _temp5468 >= _temp5466.last_plus_one){
_throw( Null_Exception);}* _temp5468;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5469= yyvs; struct _xenum_struct** _temp5471= _temp5469.curr +
yyvsp_offset; if( _temp5471 < _temp5469.base? 1: _temp5471 >= _temp5469.last_plus_one){
_throw( Null_Exception);}* _temp5471;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5472= yyls; struct Cyc_Yyltype* _temp5474= _temp5472.curr +(
yylsp_offset - 2); if( _temp5474 < _temp5472.base? 1: _temp5474 >= _temp5472.last_plus_one){
_throw( Null_Exception);}* _temp5474;}).first_line,({ struct _tagged_ptr3
_temp5475= yyls; struct Cyc_Yyltype* _temp5477= _temp5475.curr + yylsp_offset;
if( _temp5477 < _temp5475.base? 1: _temp5477 >= _temp5475.last_plus_one){ _throw(
Null_Exception);}* _temp5477;}).last_line));( struct _xenum_struct*) _temp5462;});
break; case 279: _LL5461: yyval=({ struct Cyc_Exp_tok_struct* _temp5479=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp5479->tag=
Cyc_Exp_tok_tag; _temp5479->f1= Cyc_Absyn_New_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5480= yyvs; struct _xenum_struct** _temp5482= _temp5480.curr +
yyvsp_offset; if( _temp5482 < _temp5480.base? 1: _temp5482 >= _temp5480.last_plus_one){
_throw( Null_Exception);}* _temp5482;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5483= yyls; struct Cyc_Yyltype* _temp5485= _temp5483.curr +(
yylsp_offset - 1); if( _temp5485 < _temp5483.base? 1: _temp5485 >= _temp5483.last_plus_one){
_throw( Null_Exception);}* _temp5485;}).first_line,({ struct _tagged_ptr3
_temp5486= yyls; struct Cyc_Yyltype* _temp5488= _temp5486.curr +( yylsp_offset +
1); if( _temp5488 < _temp5486.base? 1: _temp5488 >= _temp5486.last_plus_one){
_throw( Null_Exception);}* _temp5488;}).last_line));( struct _xenum_struct*)
_temp5479;}); break; case 280: _LL5478: yyval=({ struct Cyc_Primopopt_tok_struct*
_temp5490=( struct Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp5490->tag= Cyc_Primopopt_tok_tag; _temp5490->f1= 0;( struct _xenum_struct*)
_temp5490;}); break; case 281: _LL5489: yyval=({ struct Cyc_Primopopt_tok_struct*
_temp5492=( struct Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp5492->tag= Cyc_Primopopt_tok_tag; _temp5492->f1=({ struct Cyc_Core_Opt*
_temp5493=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp5493->v=( void*) Cyc_Absyn_Times; _temp5493;});( struct _xenum_struct*)
_temp5492;}); break; case 282: _LL5491: yyval=({ struct Cyc_Primopopt_tok_struct*
_temp5495=( struct Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp5495->tag= Cyc_Primopopt_tok_tag; _temp5495->f1=({ struct Cyc_Core_Opt*
_temp5496=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp5496->v=( void*) Cyc_Absyn_Div; _temp5496;});( struct _xenum_struct*)
_temp5495;}); break; case 283: _LL5494: yyval=({ struct Cyc_Primopopt_tok_struct*
_temp5498=( struct Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp5498->tag= Cyc_Primopopt_tok_tag; _temp5498->f1=({ struct Cyc_Core_Opt*
_temp5499=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp5499->v=( void*) Cyc_Absyn_Mod; _temp5499;});( struct _xenum_struct*)
_temp5498;}); break; case 284: _LL5497: yyval=({ struct Cyc_Primopopt_tok_struct*
_temp5501=( struct Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp5501->tag= Cyc_Primopopt_tok_tag; _temp5501->f1=({ struct Cyc_Core_Opt*
_temp5502=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp5502->v=( void*) Cyc_Absyn_Plus; _temp5502;});( struct _xenum_struct*)
_temp5501;}); break; case 285: _LL5500: yyval=({ struct Cyc_Primopopt_tok_struct*
_temp5504=( struct Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp5504->tag= Cyc_Primopopt_tok_tag; _temp5504->f1=({ struct Cyc_Core_Opt*
_temp5505=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp5505->v=( void*) Cyc_Absyn_Minus; _temp5505;});( struct _xenum_struct*)
_temp5504;}); break; case 286: _LL5503: yyval=({ struct Cyc_Primopopt_tok_struct*
_temp5507=( struct Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp5507->tag= Cyc_Primopopt_tok_tag; _temp5507->f1=({ struct Cyc_Core_Opt*
_temp5508=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp5508->v=( void*) Cyc_Absyn_Bitlshift; _temp5508;});( struct _xenum_struct*)
_temp5507;}); break; case 287: _LL5506: yyval=({ struct Cyc_Primopopt_tok_struct*
_temp5510=( struct Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp5510->tag= Cyc_Primopopt_tok_tag; _temp5510->f1=({ struct Cyc_Core_Opt*
_temp5511=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp5511->v=( void*) Cyc_Absyn_Bitlrshift; _temp5511;});( struct _xenum_struct*)
_temp5510;}); break; case 288: _LL5509: yyval=({ struct Cyc_Primopopt_tok_struct*
_temp5513=( struct Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp5513->tag= Cyc_Primopopt_tok_tag; _temp5513->f1=({ struct Cyc_Core_Opt*
_temp5514=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp5514->v=( void*) Cyc_Absyn_Bitand; _temp5514;});( struct _xenum_struct*)
_temp5513;}); break; case 289: _LL5512: yyval=({ struct Cyc_Primopopt_tok_struct*
_temp5516=( struct Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp5516->tag= Cyc_Primopopt_tok_tag; _temp5516->f1=({ struct Cyc_Core_Opt*
_temp5517=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp5517->v=( void*) Cyc_Absyn_Bitxor; _temp5517;});( struct _xenum_struct*)
_temp5516;}); break; case 290: _LL5515: yyval=({ struct Cyc_Primopopt_tok_struct*
_temp5519=( struct Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp5519->tag= Cyc_Primopopt_tok_tag; _temp5519->f1=({ struct Cyc_Core_Opt*
_temp5520=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp5520->v=( void*) Cyc_Absyn_Bitor; _temp5520;});( struct _xenum_struct*)
_temp5519;}); break; case 291: _LL5518: yyval=({ struct _tagged_ptr2 _temp5522=
yyvs; struct _xenum_struct** _temp5524= _temp5522.curr + yyvsp_offset; if(
_temp5524 < _temp5522.base? 1: _temp5524 >= _temp5522.last_plus_one){ _throw(
Null_Exception);}* _temp5524;}); break; case 292: _LL5521: yyval=({ struct Cyc_Exp_tok_struct*
_temp5526=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp5526->tag= Cyc_Exp_tok_tag; _temp5526->f1= Cyc_Absyn_conditional_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5527= yyvs; struct _xenum_struct** _temp5529= _temp5527.curr
+( yyvsp_offset - 4); if( _temp5529 < _temp5527.base? 1: _temp5529 >= _temp5527.last_plus_one){
_throw( Null_Exception);}* _temp5529;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5530= yyvs; struct _xenum_struct** _temp5532= _temp5530.curr +(
yyvsp_offset - 2); if( _temp5532 < _temp5530.base? 1: _temp5532 >= _temp5530.last_plus_one){
_throw( Null_Exception);}* _temp5532;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5533= yyvs; struct _xenum_struct** _temp5535= _temp5533.curr +
yyvsp_offset; if( _temp5535 < _temp5533.base? 1: _temp5535 >= _temp5533.last_plus_one){
_throw( Null_Exception);}* _temp5535;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5536= yyls; struct Cyc_Yyltype* _temp5538= _temp5536.curr +(
yylsp_offset - 4); if( _temp5538 < _temp5536.base? 1: _temp5538 >= _temp5536.last_plus_one){
_throw( Null_Exception);}* _temp5538;}).first_line,({ struct _tagged_ptr3
_temp5539= yyls; struct Cyc_Yyltype* _temp5541= _temp5539.curr + yylsp_offset;
if( _temp5541 < _temp5539.base? 1: _temp5541 >= _temp5539.last_plus_one){ _throw(
Null_Exception);}* _temp5541;}).last_line));( struct _xenum_struct*) _temp5526;});
break; case 293: _LL5525: yyval=({ struct _tagged_ptr2 _temp5543= yyvs; struct
_xenum_struct** _temp5545= _temp5543.curr + yyvsp_offset; if( _temp5545 <
_temp5543.base? 1: _temp5545 >= _temp5543.last_plus_one){ _throw( Null_Exception);}*
_temp5545;}); break; case 294: _LL5542: yyval=({ struct _tagged_ptr2 _temp5547=
yyvs; struct _xenum_struct** _temp5549= _temp5547.curr + yyvsp_offset; if(
_temp5549 < _temp5547.base? 1: _temp5549 >= _temp5547.last_plus_one){ _throw(
Null_Exception);}* _temp5549;}); break; case 295: _LL5546: yyval=({ struct Cyc_Exp_tok_struct*
_temp5551=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp5551->tag= Cyc_Exp_tok_tag; _temp5551->f1= Cyc_Absyn_or_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5552= yyvs; struct _xenum_struct** _temp5554= _temp5552.curr
+( yyvsp_offset - 2); if( _temp5554 < _temp5552.base? 1: _temp5554 >= _temp5552.last_plus_one){
_throw( Null_Exception);}* _temp5554;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5555= yyvs; struct _xenum_struct** _temp5557= _temp5555.curr +
yyvsp_offset; if( _temp5557 < _temp5555.base? 1: _temp5557 >= _temp5555.last_plus_one){
_throw( Null_Exception);}* _temp5557;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5558= yyls; struct Cyc_Yyltype* _temp5560= _temp5558.curr +(
yylsp_offset - 2); if( _temp5560 < _temp5558.base? 1: _temp5560 >= _temp5558.last_plus_one){
_throw( Null_Exception);}* _temp5560;}).first_line,({ struct _tagged_ptr3
_temp5561= yyls; struct Cyc_Yyltype* _temp5563= _temp5561.curr + yylsp_offset;
if( _temp5563 < _temp5561.base? 1: _temp5563 >= _temp5561.last_plus_one){ _throw(
Null_Exception);}* _temp5563;}).last_line));( struct _xenum_struct*) _temp5551;});
break; case 296: _LL5550: yyval=({ struct _tagged_ptr2 _temp5565= yyvs; struct
_xenum_struct** _temp5567= _temp5565.curr + yyvsp_offset; if( _temp5567 <
_temp5565.base? 1: _temp5567 >= _temp5565.last_plus_one){ _throw( Null_Exception);}*
_temp5567;}); break; case 297: _LL5564: yyval=({ struct Cyc_Exp_tok_struct*
_temp5569=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp5569->tag= Cyc_Exp_tok_tag; _temp5569->f1= Cyc_Absyn_and_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5570= yyvs; struct _xenum_struct** _temp5572= _temp5570.curr
+( yyvsp_offset - 2); if( _temp5572 < _temp5570.base? 1: _temp5572 >= _temp5570.last_plus_one){
_throw( Null_Exception);}* _temp5572;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5573= yyvs; struct _xenum_struct** _temp5575= _temp5573.curr +
yyvsp_offset; if( _temp5575 < _temp5573.base? 1: _temp5575 >= _temp5573.last_plus_one){
_throw( Null_Exception);}* _temp5575;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5576= yyls; struct Cyc_Yyltype* _temp5578= _temp5576.curr +(
yylsp_offset - 2); if( _temp5578 < _temp5576.base? 1: _temp5578 >= _temp5576.last_plus_one){
_throw( Null_Exception);}* _temp5578;}).first_line,({ struct _tagged_ptr3
_temp5579= yyls; struct Cyc_Yyltype* _temp5581= _temp5579.curr + yylsp_offset;
if( _temp5581 < _temp5579.base? 1: _temp5581 >= _temp5579.last_plus_one){ _throw(
Null_Exception);}* _temp5581;}).last_line));( struct _xenum_struct*) _temp5569;});
break; case 298: _LL5568: yyval=({ struct _tagged_ptr2 _temp5583= yyvs; struct
_xenum_struct** _temp5585= _temp5583.curr + yyvsp_offset; if( _temp5585 <
_temp5583.base? 1: _temp5585 >= _temp5583.last_plus_one){ _throw( Null_Exception);}*
_temp5585;}); break; case 299: _LL5582: yyval=({ struct Cyc_Exp_tok_struct*
_temp5587=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp5587->tag= Cyc_Exp_tok_tag; _temp5587->f1= Cyc_Absyn_prim2_exp( Cyc_Absyn_Bitor,
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5588= yyvs; struct _xenum_struct**
_temp5590= _temp5588.curr +( yyvsp_offset - 2); if( _temp5590 < _temp5588.base?
1: _temp5590 >= _temp5588.last_plus_one){ _throw( Null_Exception);}* _temp5590;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5591= yyvs; struct _xenum_struct**
_temp5593= _temp5591.curr + yyvsp_offset; if( _temp5593 < _temp5591.base? 1:
_temp5593 >= _temp5591.last_plus_one){ _throw( Null_Exception);}* _temp5593;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5594= yyls; struct Cyc_Yyltype*
_temp5596= _temp5594.curr +( yylsp_offset - 2); if( _temp5596 < _temp5594.base?
1: _temp5596 >= _temp5594.last_plus_one){ _throw( Null_Exception);}* _temp5596;}).first_line,({
struct _tagged_ptr3 _temp5597= yyls; struct Cyc_Yyltype* _temp5599= _temp5597.curr
+ yylsp_offset; if( _temp5599 < _temp5597.base? 1: _temp5599 >= _temp5597.last_plus_one){
_throw( Null_Exception);}* _temp5599;}).last_line));( struct _xenum_struct*)
_temp5587;}); break; case 300: _LL5586: yyval=({ struct _tagged_ptr2 _temp5601=
yyvs; struct _xenum_struct** _temp5603= _temp5601.curr + yyvsp_offset; if(
_temp5603 < _temp5601.base? 1: _temp5603 >= _temp5601.last_plus_one){ _throw(
Null_Exception);}* _temp5603;}); break; case 301: _LL5600: yyval=({ struct Cyc_Exp_tok_struct*
_temp5605=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp5605->tag= Cyc_Exp_tok_tag; _temp5605->f1= Cyc_Absyn_prim2_exp( Cyc_Absyn_Bitxor,
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5606= yyvs; struct _xenum_struct**
_temp5608= _temp5606.curr +( yyvsp_offset - 2); if( _temp5608 < _temp5606.base?
1: _temp5608 >= _temp5606.last_plus_one){ _throw( Null_Exception);}* _temp5608;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5609= yyvs; struct _xenum_struct**
_temp5611= _temp5609.curr + yyvsp_offset; if( _temp5611 < _temp5609.base? 1:
_temp5611 >= _temp5609.last_plus_one){ _throw( Null_Exception);}* _temp5611;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5612= yyls; struct Cyc_Yyltype*
_temp5614= _temp5612.curr +( yylsp_offset - 2); if( _temp5614 < _temp5612.base?
1: _temp5614 >= _temp5612.last_plus_one){ _throw( Null_Exception);}* _temp5614;}).first_line,({
struct _tagged_ptr3 _temp5615= yyls; struct Cyc_Yyltype* _temp5617= _temp5615.curr
+ yylsp_offset; if( _temp5617 < _temp5615.base? 1: _temp5617 >= _temp5615.last_plus_one){
_throw( Null_Exception);}* _temp5617;}).last_line));( struct _xenum_struct*)
_temp5605;}); break; case 302: _LL5604: yyval=({ struct _tagged_ptr2 _temp5619=
yyvs; struct _xenum_struct** _temp5621= _temp5619.curr + yyvsp_offset; if(
_temp5621 < _temp5619.base? 1: _temp5621 >= _temp5619.last_plus_one){ _throw(
Null_Exception);}* _temp5621;}); break; case 303: _LL5618: yyval=({ struct Cyc_Exp_tok_struct*
_temp5623=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp5623->tag= Cyc_Exp_tok_tag; _temp5623->f1= Cyc_Absyn_prim2_exp( Cyc_Absyn_Bitand,
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5624= yyvs; struct _xenum_struct**
_temp5626= _temp5624.curr +( yyvsp_offset - 2); if( _temp5626 < _temp5624.base?
1: _temp5626 >= _temp5624.last_plus_one){ _throw( Null_Exception);}* _temp5626;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5627= yyvs; struct _xenum_struct**
_temp5629= _temp5627.curr + yyvsp_offset; if( _temp5629 < _temp5627.base? 1:
_temp5629 >= _temp5627.last_plus_one){ _throw( Null_Exception);}* _temp5629;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5630= yyls; struct Cyc_Yyltype*
_temp5632= _temp5630.curr +( yylsp_offset - 2); if( _temp5632 < _temp5630.base?
1: _temp5632 >= _temp5630.last_plus_one){ _throw( Null_Exception);}* _temp5632;}).first_line,({
struct _tagged_ptr3 _temp5633= yyls; struct Cyc_Yyltype* _temp5635= _temp5633.curr
+ yylsp_offset; if( _temp5635 < _temp5633.base? 1: _temp5635 >= _temp5633.last_plus_one){
_throw( Null_Exception);}* _temp5635;}).last_line));( struct _xenum_struct*)
_temp5623;}); break; case 304: _LL5622: yyval=({ struct _tagged_ptr2 _temp5637=
yyvs; struct _xenum_struct** _temp5639= _temp5637.curr + yyvsp_offset; if(
_temp5639 < _temp5637.base? 1: _temp5639 >= _temp5637.last_plus_one){ _throw(
Null_Exception);}* _temp5639;}); break; case 305: _LL5636: yyval=({ struct Cyc_Exp_tok_struct*
_temp5641=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp5641->tag= Cyc_Exp_tok_tag; _temp5641->f1= Cyc_Absyn_eq_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5642= yyvs; struct _xenum_struct** _temp5644= _temp5642.curr
+( yyvsp_offset - 2); if( _temp5644 < _temp5642.base? 1: _temp5644 >= _temp5642.last_plus_one){
_throw( Null_Exception);}* _temp5644;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5645= yyvs; struct _xenum_struct** _temp5647= _temp5645.curr +
yyvsp_offset; if( _temp5647 < _temp5645.base? 1: _temp5647 >= _temp5645.last_plus_one){
_throw( Null_Exception);}* _temp5647;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5648= yyls; struct Cyc_Yyltype* _temp5650= _temp5648.curr +(
yylsp_offset - 2); if( _temp5650 < _temp5648.base? 1: _temp5650 >= _temp5648.last_plus_one){
_throw( Null_Exception);}* _temp5650;}).first_line,({ struct _tagged_ptr3
_temp5651= yyls; struct Cyc_Yyltype* _temp5653= _temp5651.curr + yylsp_offset;
if( _temp5653 < _temp5651.base? 1: _temp5653 >= _temp5651.last_plus_one){ _throw(
Null_Exception);}* _temp5653;}).last_line));( struct _xenum_struct*) _temp5641;});
break; case 306: _LL5640: yyval=({ struct Cyc_Exp_tok_struct* _temp5655=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp5655->tag=
Cyc_Exp_tok_tag; _temp5655->f1= Cyc_Absyn_neq_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5656= yyvs; struct _xenum_struct** _temp5658= _temp5656.curr +(
yyvsp_offset - 2); if( _temp5658 < _temp5656.base? 1: _temp5658 >= _temp5656.last_plus_one){
_throw( Null_Exception);}* _temp5658;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5659= yyvs; struct _xenum_struct** _temp5661= _temp5659.curr +
yyvsp_offset; if( _temp5661 < _temp5659.base? 1: _temp5661 >= _temp5659.last_plus_one){
_throw( Null_Exception);}* _temp5661;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5662= yyls; struct Cyc_Yyltype* _temp5664= _temp5662.curr +(
yylsp_offset - 2); if( _temp5664 < _temp5662.base? 1: _temp5664 >= _temp5662.last_plus_one){
_throw( Null_Exception);}* _temp5664;}).first_line,({ struct _tagged_ptr3
_temp5665= yyls; struct Cyc_Yyltype* _temp5667= _temp5665.curr + yylsp_offset;
if( _temp5667 < _temp5665.base? 1: _temp5667 >= _temp5665.last_plus_one){ _throw(
Null_Exception);}* _temp5667;}).last_line));( struct _xenum_struct*) _temp5655;});
break; case 307: _LL5654: yyval=({ struct _tagged_ptr2 _temp5669= yyvs; struct
_xenum_struct** _temp5671= _temp5669.curr + yyvsp_offset; if( _temp5671 <
_temp5669.base? 1: _temp5671 >= _temp5669.last_plus_one){ _throw( Null_Exception);}*
_temp5671;}); break; case 308: _LL5668: yyval=({ struct Cyc_Exp_tok_struct*
_temp5673=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp5673->tag= Cyc_Exp_tok_tag; _temp5673->f1= Cyc_Absyn_lt_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5674= yyvs; struct _xenum_struct** _temp5676= _temp5674.curr
+( yyvsp_offset - 2); if( _temp5676 < _temp5674.base? 1: _temp5676 >= _temp5674.last_plus_one){
_throw( Null_Exception);}* _temp5676;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5677= yyvs; struct _xenum_struct** _temp5679= _temp5677.curr +
yyvsp_offset; if( _temp5679 < _temp5677.base? 1: _temp5679 >= _temp5677.last_plus_one){
_throw( Null_Exception);}* _temp5679;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5680= yyls; struct Cyc_Yyltype* _temp5682= _temp5680.curr +(
yylsp_offset - 2); if( _temp5682 < _temp5680.base? 1: _temp5682 >= _temp5680.last_plus_one){
_throw( Null_Exception);}* _temp5682;}).first_line,({ struct _tagged_ptr3
_temp5683= yyls; struct Cyc_Yyltype* _temp5685= _temp5683.curr + yylsp_offset;
if( _temp5685 < _temp5683.base? 1: _temp5685 >= _temp5683.last_plus_one){ _throw(
Null_Exception);}* _temp5685;}).last_line));( struct _xenum_struct*) _temp5673;});
break; case 309: _LL5672: yyval=({ struct Cyc_Exp_tok_struct* _temp5687=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp5687->tag=
Cyc_Exp_tok_tag; _temp5687->f1= Cyc_Absyn_gt_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5688= yyvs; struct _xenum_struct** _temp5690= _temp5688.curr +(
yyvsp_offset - 2); if( _temp5690 < _temp5688.base? 1: _temp5690 >= _temp5688.last_plus_one){
_throw( Null_Exception);}* _temp5690;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5691= yyvs; struct _xenum_struct** _temp5693= _temp5691.curr +
yyvsp_offset; if( _temp5693 < _temp5691.base? 1: _temp5693 >= _temp5691.last_plus_one){
_throw( Null_Exception);}* _temp5693;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5694= yyls; struct Cyc_Yyltype* _temp5696= _temp5694.curr +(
yylsp_offset - 2); if( _temp5696 < _temp5694.base? 1: _temp5696 >= _temp5694.last_plus_one){
_throw( Null_Exception);}* _temp5696;}).first_line,({ struct _tagged_ptr3
_temp5697= yyls; struct Cyc_Yyltype* _temp5699= _temp5697.curr + yylsp_offset;
if( _temp5699 < _temp5697.base? 1: _temp5699 >= _temp5697.last_plus_one){ _throw(
Null_Exception);}* _temp5699;}).last_line));( struct _xenum_struct*) _temp5687;});
break; case 310: _LL5686: yyval=({ struct Cyc_Exp_tok_struct* _temp5701=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp5701->tag=
Cyc_Exp_tok_tag; _temp5701->f1= Cyc_Absyn_lte_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5702= yyvs; struct _xenum_struct** _temp5704= _temp5702.curr +(
yyvsp_offset - 2); if( _temp5704 < _temp5702.base? 1: _temp5704 >= _temp5702.last_plus_one){
_throw( Null_Exception);}* _temp5704;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5705= yyvs; struct _xenum_struct** _temp5707= _temp5705.curr +
yyvsp_offset; if( _temp5707 < _temp5705.base? 1: _temp5707 >= _temp5705.last_plus_one){
_throw( Null_Exception);}* _temp5707;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5708= yyls; struct Cyc_Yyltype* _temp5710= _temp5708.curr +(
yylsp_offset - 2); if( _temp5710 < _temp5708.base? 1: _temp5710 >= _temp5708.last_plus_one){
_throw( Null_Exception);}* _temp5710;}).first_line,({ struct _tagged_ptr3
_temp5711= yyls; struct Cyc_Yyltype* _temp5713= _temp5711.curr + yylsp_offset;
if( _temp5713 < _temp5711.base? 1: _temp5713 >= _temp5711.last_plus_one){ _throw(
Null_Exception);}* _temp5713;}).last_line));( struct _xenum_struct*) _temp5701;});
break; case 311: _LL5700: yyval=({ struct Cyc_Exp_tok_struct* _temp5715=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp5715->tag=
Cyc_Exp_tok_tag; _temp5715->f1= Cyc_Absyn_gte_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5716= yyvs; struct _xenum_struct** _temp5718= _temp5716.curr +(
yyvsp_offset - 2); if( _temp5718 < _temp5716.base? 1: _temp5718 >= _temp5716.last_plus_one){
_throw( Null_Exception);}* _temp5718;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5719= yyvs; struct _xenum_struct** _temp5721= _temp5719.curr +
yyvsp_offset; if( _temp5721 < _temp5719.base? 1: _temp5721 >= _temp5719.last_plus_one){
_throw( Null_Exception);}* _temp5721;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5722= yyls; struct Cyc_Yyltype* _temp5724= _temp5722.curr +(
yylsp_offset - 2); if( _temp5724 < _temp5722.base? 1: _temp5724 >= _temp5722.last_plus_one){
_throw( Null_Exception);}* _temp5724;}).first_line,({ struct _tagged_ptr3
_temp5725= yyls; struct Cyc_Yyltype* _temp5727= _temp5725.curr + yylsp_offset;
if( _temp5727 < _temp5725.base? 1: _temp5727 >= _temp5725.last_plus_one){ _throw(
Null_Exception);}* _temp5727;}).last_line));( struct _xenum_struct*) _temp5715;});
break; case 312: _LL5714: yyval=({ struct _tagged_ptr2 _temp5729= yyvs; struct
_xenum_struct** _temp5731= _temp5729.curr + yyvsp_offset; if( _temp5731 <
_temp5729.base? 1: _temp5731 >= _temp5729.last_plus_one){ _throw( Null_Exception);}*
_temp5731;}); break; case 313: _LL5728: yyval=({ struct Cyc_Exp_tok_struct*
_temp5733=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp5733->tag= Cyc_Exp_tok_tag; _temp5733->f1= Cyc_Absyn_prim2_exp( Cyc_Absyn_Bitlshift,
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5734= yyvs; struct _xenum_struct**
_temp5736= _temp5734.curr +( yyvsp_offset - 2); if( _temp5736 < _temp5734.base?
1: _temp5736 >= _temp5734.last_plus_one){ _throw( Null_Exception);}* _temp5736;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5737= yyvs; struct _xenum_struct**
_temp5739= _temp5737.curr + yyvsp_offset; if( _temp5739 < _temp5737.base? 1:
_temp5739 >= _temp5737.last_plus_one){ _throw( Null_Exception);}* _temp5739;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5740= yyls; struct Cyc_Yyltype*
_temp5742= _temp5740.curr +( yylsp_offset - 2); if( _temp5742 < _temp5740.base?
1: _temp5742 >= _temp5740.last_plus_one){ _throw( Null_Exception);}* _temp5742;}).first_line,({
struct _tagged_ptr3 _temp5743= yyls; struct Cyc_Yyltype* _temp5745= _temp5743.curr
+ yylsp_offset; if( _temp5745 < _temp5743.base? 1: _temp5745 >= _temp5743.last_plus_one){
_throw( Null_Exception);}* _temp5745;}).last_line));( struct _xenum_struct*)
_temp5733;}); break; case 314: _LL5732: yyval=({ struct Cyc_Exp_tok_struct*
_temp5747=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp5747->tag= Cyc_Exp_tok_tag; _temp5747->f1= Cyc_Absyn_prim2_exp( Cyc_Absyn_Bitlrshift,
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5748= yyvs; struct _xenum_struct**
_temp5750= _temp5748.curr +( yyvsp_offset - 2); if( _temp5750 < _temp5748.base?
1: _temp5750 >= _temp5748.last_plus_one){ _throw( Null_Exception);}* _temp5750;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5751= yyvs; struct _xenum_struct**
_temp5753= _temp5751.curr + yyvsp_offset; if( _temp5753 < _temp5751.base? 1:
_temp5753 >= _temp5751.last_plus_one){ _throw( Null_Exception);}* _temp5753;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5754= yyls; struct Cyc_Yyltype*
_temp5756= _temp5754.curr +( yylsp_offset - 2); if( _temp5756 < _temp5754.base?
1: _temp5756 >= _temp5754.last_plus_one){ _throw( Null_Exception);}* _temp5756;}).first_line,({
struct _tagged_ptr3 _temp5757= yyls; struct Cyc_Yyltype* _temp5759= _temp5757.curr
+ yylsp_offset; if( _temp5759 < _temp5757.base? 1: _temp5759 >= _temp5757.last_plus_one){
_throw( Null_Exception);}* _temp5759;}).last_line));( struct _xenum_struct*)
_temp5747;}); break; case 315: _LL5746: yyval=({ struct _tagged_ptr2 _temp5761=
yyvs; struct _xenum_struct** _temp5763= _temp5761.curr + yyvsp_offset; if(
_temp5763 < _temp5761.base? 1: _temp5763 >= _temp5761.last_plus_one){ _throw(
Null_Exception);}* _temp5763;}); break; case 316: _LL5760: yyval=({ struct Cyc_Exp_tok_struct*
_temp5765=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp5765->tag= Cyc_Exp_tok_tag; _temp5765->f1= Cyc_Absyn_prim2_exp( Cyc_Absyn_Plus,
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5766= yyvs; struct _xenum_struct**
_temp5768= _temp5766.curr +( yyvsp_offset - 2); if( _temp5768 < _temp5766.base?
1: _temp5768 >= _temp5766.last_plus_one){ _throw( Null_Exception);}* _temp5768;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5769= yyvs; struct _xenum_struct**
_temp5771= _temp5769.curr + yyvsp_offset; if( _temp5771 < _temp5769.base? 1:
_temp5771 >= _temp5769.last_plus_one){ _throw( Null_Exception);}* _temp5771;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5772= yyls; struct Cyc_Yyltype*
_temp5774= _temp5772.curr +( yylsp_offset - 2); if( _temp5774 < _temp5772.base?
1: _temp5774 >= _temp5772.last_plus_one){ _throw( Null_Exception);}* _temp5774;}).first_line,({
struct _tagged_ptr3 _temp5775= yyls; struct Cyc_Yyltype* _temp5777= _temp5775.curr
+ yylsp_offset; if( _temp5777 < _temp5775.base? 1: _temp5777 >= _temp5775.last_plus_one){
_throw( Null_Exception);}* _temp5777;}).last_line));( struct _xenum_struct*)
_temp5765;}); break; case 317: _LL5764: yyval=({ struct Cyc_Exp_tok_struct*
_temp5779=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp5779->tag= Cyc_Exp_tok_tag; _temp5779->f1= Cyc_Absyn_prim2_exp( Cyc_Absyn_Minus,
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5780= yyvs; struct _xenum_struct**
_temp5782= _temp5780.curr +( yyvsp_offset - 2); if( _temp5782 < _temp5780.base?
1: _temp5782 >= _temp5780.last_plus_one){ _throw( Null_Exception);}* _temp5782;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5783= yyvs; struct _xenum_struct**
_temp5785= _temp5783.curr + yyvsp_offset; if( _temp5785 < _temp5783.base? 1:
_temp5785 >= _temp5783.last_plus_one){ _throw( Null_Exception);}* _temp5785;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5786= yyls; struct Cyc_Yyltype*
_temp5788= _temp5786.curr +( yylsp_offset - 2); if( _temp5788 < _temp5786.base?
1: _temp5788 >= _temp5786.last_plus_one){ _throw( Null_Exception);}* _temp5788;}).first_line,({
struct _tagged_ptr3 _temp5789= yyls; struct Cyc_Yyltype* _temp5791= _temp5789.curr
+ yylsp_offset; if( _temp5791 < _temp5789.base? 1: _temp5791 >= _temp5789.last_plus_one){
_throw( Null_Exception);}* _temp5791;}).last_line));( struct _xenum_struct*)
_temp5779;}); break; case 318: _LL5778: yyval=({ struct _tagged_ptr2 _temp5793=
yyvs; struct _xenum_struct** _temp5795= _temp5793.curr + yyvsp_offset; if(
_temp5795 < _temp5793.base? 1: _temp5795 >= _temp5793.last_plus_one){ _throw(
Null_Exception);}* _temp5795;}); break; case 319: _LL5792: yyval=({ struct Cyc_Exp_tok_struct*
_temp5797=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp5797->tag= Cyc_Exp_tok_tag; _temp5797->f1= Cyc_Absyn_prim2_exp( Cyc_Absyn_Times,
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5798= yyvs; struct _xenum_struct**
_temp5800= _temp5798.curr +( yyvsp_offset - 2); if( _temp5800 < _temp5798.base?
1: _temp5800 >= _temp5798.last_plus_one){ _throw( Null_Exception);}* _temp5800;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5801= yyvs; struct _xenum_struct**
_temp5803= _temp5801.curr + yyvsp_offset; if( _temp5803 < _temp5801.base? 1:
_temp5803 >= _temp5801.last_plus_one){ _throw( Null_Exception);}* _temp5803;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5804= yyls; struct Cyc_Yyltype*
_temp5806= _temp5804.curr +( yylsp_offset - 2); if( _temp5806 < _temp5804.base?
1: _temp5806 >= _temp5804.last_plus_one){ _throw( Null_Exception);}* _temp5806;}).first_line,({
struct _tagged_ptr3 _temp5807= yyls; struct Cyc_Yyltype* _temp5809= _temp5807.curr
+ yylsp_offset; if( _temp5809 < _temp5807.base? 1: _temp5809 >= _temp5807.last_plus_one){
_throw( Null_Exception);}* _temp5809;}).last_line));( struct _xenum_struct*)
_temp5797;}); break; case 320: _LL5796: yyval=({ struct Cyc_Exp_tok_struct*
_temp5811=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp5811->tag= Cyc_Exp_tok_tag; _temp5811->f1= Cyc_Absyn_prim2_exp( Cyc_Absyn_Div,
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5812= yyvs; struct _xenum_struct**
_temp5814= _temp5812.curr +( yyvsp_offset - 2); if( _temp5814 < _temp5812.base?
1: _temp5814 >= _temp5812.last_plus_one){ _throw( Null_Exception);}* _temp5814;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5815= yyvs; struct _xenum_struct**
_temp5817= _temp5815.curr + yyvsp_offset; if( _temp5817 < _temp5815.base? 1:
_temp5817 >= _temp5815.last_plus_one){ _throw( Null_Exception);}* _temp5817;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5818= yyls; struct Cyc_Yyltype*
_temp5820= _temp5818.curr +( yylsp_offset - 2); if( _temp5820 < _temp5818.base?
1: _temp5820 >= _temp5818.last_plus_one){ _throw( Null_Exception);}* _temp5820;}).first_line,({
struct _tagged_ptr3 _temp5821= yyls; struct Cyc_Yyltype* _temp5823= _temp5821.curr
+ yylsp_offset; if( _temp5823 < _temp5821.base? 1: _temp5823 >= _temp5821.last_plus_one){
_throw( Null_Exception);}* _temp5823;}).last_line));( struct _xenum_struct*)
_temp5811;}); break; case 321: _LL5810: yyval=({ struct Cyc_Exp_tok_struct*
_temp5825=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp5825->tag= Cyc_Exp_tok_tag; _temp5825->f1= Cyc_Absyn_prim2_exp( Cyc_Absyn_Mod,
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5826= yyvs; struct _xenum_struct**
_temp5828= _temp5826.curr +( yyvsp_offset - 2); if( _temp5828 < _temp5826.base?
1: _temp5828 >= _temp5826.last_plus_one){ _throw( Null_Exception);}* _temp5828;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5829= yyvs; struct _xenum_struct**
_temp5831= _temp5829.curr + yyvsp_offset; if( _temp5831 < _temp5829.base? 1:
_temp5831 >= _temp5829.last_plus_one){ _throw( Null_Exception);}* _temp5831;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5832= yyls; struct Cyc_Yyltype*
_temp5834= _temp5832.curr +( yylsp_offset - 2); if( _temp5834 < _temp5832.base?
1: _temp5834 >= _temp5832.last_plus_one){ _throw( Null_Exception);}* _temp5834;}).first_line,({
struct _tagged_ptr3 _temp5835= yyls; struct Cyc_Yyltype* _temp5837= _temp5835.curr
+ yylsp_offset; if( _temp5837 < _temp5835.base? 1: _temp5837 >= _temp5835.last_plus_one){
_throw( Null_Exception);}* _temp5837;}).last_line));( struct _xenum_struct*)
_temp5825;}); break; case 322: _LL5824: yyval=({ struct _tagged_ptr2 _temp5839=
yyvs; struct _xenum_struct** _temp5841= _temp5839.curr + yyvsp_offset; if(
_temp5841 < _temp5839.base? 1: _temp5841 >= _temp5839.last_plus_one){ _throw(
Null_Exception);}* _temp5841;}); break; case 323: _LL5838: yyval=({ struct Cyc_Exp_tok_struct*
_temp5843=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp5843->tag= Cyc_Exp_tok_tag; _temp5843->f1= Cyc_Absyn_cast_exp((* Cyc_yyget_ParamDecl_tok(({
struct _tagged_ptr2 _temp5844= yyvs; struct _xenum_struct** _temp5846= _temp5844.curr
+( yyvsp_offset - 2); if( _temp5846 < _temp5844.base? 1: _temp5846 >= _temp5844.last_plus_one){
_throw( Null_Exception);}* _temp5846;}))).f3, Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5847= yyvs; struct _xenum_struct** _temp5849= _temp5847.curr +
yyvsp_offset; if( _temp5849 < _temp5847.base? 1: _temp5849 >= _temp5847.last_plus_one){
_throw( Null_Exception);}* _temp5849;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5850= yyls; struct Cyc_Yyltype* _temp5852= _temp5850.curr +(
yylsp_offset - 3); if( _temp5852 < _temp5850.base? 1: _temp5852 >= _temp5850.last_plus_one){
_throw( Null_Exception);}* _temp5852;}).first_line,({ struct _tagged_ptr3
_temp5853= yyls; struct Cyc_Yyltype* _temp5855= _temp5853.curr + yylsp_offset;
if( _temp5855 < _temp5853.base? 1: _temp5855 >= _temp5853.last_plus_one){ _throw(
Null_Exception);}* _temp5855;}).last_line));( struct _xenum_struct*) _temp5843;});
break; case 324: _LL5842: yyval=({ struct _tagged_ptr2 _temp5857= yyvs; struct
_xenum_struct** _temp5859= _temp5857.curr + yyvsp_offset; if( _temp5859 <
_temp5857.base? 1: _temp5859 >= _temp5857.last_plus_one){ _throw( Null_Exception);}*
_temp5859;}); break; case 325: _LL5856: yyval=({ struct Cyc_Exp_tok_struct*
_temp5861=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp5861->tag= Cyc_Exp_tok_tag; _temp5861->f1= Cyc_Absyn_pre_inc_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5862= yyvs; struct _xenum_struct** _temp5864= _temp5862.curr
+ yyvsp_offset; if( _temp5864 < _temp5862.base? 1: _temp5864 >= _temp5862.last_plus_one){
_throw( Null_Exception);}* _temp5864;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5865= yyls; struct Cyc_Yyltype* _temp5867= _temp5865.curr +(
yylsp_offset - 1); if( _temp5867 < _temp5865.base? 1: _temp5867 >= _temp5865.last_plus_one){
_throw( Null_Exception);}* _temp5867;}).first_line,({ struct _tagged_ptr3
_temp5868= yyls; struct Cyc_Yyltype* _temp5870= _temp5868.curr + yylsp_offset;
if( _temp5870 < _temp5868.base? 1: _temp5870 >= _temp5868.last_plus_one){ _throw(
Null_Exception);}* _temp5870;}).last_line));( struct _xenum_struct*) _temp5861;});
break; case 326: _LL5860: yyval=({ struct Cyc_Exp_tok_struct* _temp5872=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp5872->tag=
Cyc_Exp_tok_tag; _temp5872->f1= Cyc_Absyn_pre_dec_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5873= yyvs; struct _xenum_struct** _temp5875= _temp5873.curr
+ yyvsp_offset; if( _temp5875 < _temp5873.base? 1: _temp5875 >= _temp5873.last_plus_one){
_throw( Null_Exception);}* _temp5875;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5876= yyls; struct Cyc_Yyltype* _temp5878= _temp5876.curr +(
yylsp_offset - 1); if( _temp5878 < _temp5876.base? 1: _temp5878 >= _temp5876.last_plus_one){
_throw( Null_Exception);}* _temp5878;}).first_line,({ struct _tagged_ptr3
_temp5879= yyls; struct Cyc_Yyltype* _temp5881= _temp5879.curr + yylsp_offset;
if( _temp5881 < _temp5879.base? 1: _temp5881 >= _temp5879.last_plus_one){ _throw(
Null_Exception);}* _temp5881;}).last_line));( struct _xenum_struct*) _temp5872;});
break; case 327: _LL5871: yyval=({ struct Cyc_Exp_tok_struct* _temp5883=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp5883->tag=
Cyc_Exp_tok_tag; _temp5883->f1= Cyc_Absyn_address_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5884= yyvs; struct _xenum_struct** _temp5886= _temp5884.curr
+ yyvsp_offset; if( _temp5886 < _temp5884.base? 1: _temp5886 >= _temp5884.last_plus_one){
_throw( Null_Exception);}* _temp5886;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5887= yyls; struct Cyc_Yyltype* _temp5889= _temp5887.curr +(
yylsp_offset - 1); if( _temp5889 < _temp5887.base? 1: _temp5889 >= _temp5887.last_plus_one){
_throw( Null_Exception);}* _temp5889;}).first_line,({ struct _tagged_ptr3
_temp5890= yyls; struct Cyc_Yyltype* _temp5892= _temp5890.curr + yylsp_offset;
if( _temp5892 < _temp5890.base? 1: _temp5892 >= _temp5890.last_plus_one){ _throw(
Null_Exception);}* _temp5892;}).last_line));( struct _xenum_struct*) _temp5883;});
break; case 328: _LL5882: yyval=({ struct Cyc_Exp_tok_struct* _temp5894=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp5894->tag=
Cyc_Exp_tok_tag; _temp5894->f1= Cyc_Absyn_deref_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5895= yyvs; struct _xenum_struct** _temp5897= _temp5895.curr +
yyvsp_offset; if( _temp5897 < _temp5895.base? 1: _temp5897 >= _temp5895.last_plus_one){
_throw( Null_Exception);}* _temp5897;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5898= yyls; struct Cyc_Yyltype* _temp5900= _temp5898.curr +(
yylsp_offset - 1); if( _temp5900 < _temp5898.base? 1: _temp5900 >= _temp5898.last_plus_one){
_throw( Null_Exception);}* _temp5900;}).first_line,({ struct _tagged_ptr3
_temp5901= yyls; struct Cyc_Yyltype* _temp5903= _temp5901.curr + yylsp_offset;
if( _temp5903 < _temp5901.base? 1: _temp5903 >= _temp5901.last_plus_one){ _throw(
Null_Exception);}* _temp5903;}).last_line));( struct _xenum_struct*) _temp5894;});
break; case 329: _LL5893: yyval=({ struct _tagged_ptr2 _temp5905= yyvs; struct
_xenum_struct** _temp5907= _temp5905.curr + yyvsp_offset; if( _temp5907 <
_temp5905.base? 1: _temp5907 >= _temp5905.last_plus_one){ _throw( Null_Exception);}*
_temp5907;}); break; case 330: _LL5904: yyval=({ struct Cyc_Exp_tok_struct*
_temp5909=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp5909->tag= Cyc_Exp_tok_tag; _temp5909->f1= Cyc_Absyn_prim1_exp( Cyc_yyget_Primop_tok(({
struct _tagged_ptr2 _temp5910= yyvs; struct _xenum_struct** _temp5912= _temp5910.curr
+( yyvsp_offset - 1); if( _temp5912 < _temp5910.base? 1: _temp5912 >= _temp5910.last_plus_one){
_throw( Null_Exception);}* _temp5912;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5913= yyvs; struct _xenum_struct** _temp5915= _temp5913.curr +
yyvsp_offset; if( _temp5915 < _temp5913.base? 1: _temp5915 >= _temp5913.last_plus_one){
_throw( Null_Exception);}* _temp5915;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5916= yyls; struct Cyc_Yyltype* _temp5918= _temp5916.curr +(
yylsp_offset - 1); if( _temp5918 < _temp5916.base? 1: _temp5918 >= _temp5916.last_plus_one){
_throw( Null_Exception);}* _temp5918;}).first_line,({ struct _tagged_ptr3
_temp5919= yyls; struct Cyc_Yyltype* _temp5921= _temp5919.curr + yylsp_offset;
if( _temp5921 < _temp5919.base? 1: _temp5921 >= _temp5919.last_plus_one){ _throw(
Null_Exception);}* _temp5921;}).last_line));( struct _xenum_struct*) _temp5909;});
break; case 331: _LL5908: yyval=({ struct Cyc_Exp_tok_struct* _temp5923=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp5923->tag=
Cyc_Exp_tok_tag; _temp5923->f1= Cyc_Absyn_sizeoftyp_exp((* Cyc_yyget_ParamDecl_tok(({
struct _tagged_ptr2 _temp5924= yyvs; struct _xenum_struct** _temp5926= _temp5924.curr
+( yyvsp_offset - 1); if( _temp5926 < _temp5924.base? 1: _temp5926 >= _temp5924.last_plus_one){
_throw( Null_Exception);}* _temp5926;}))).f3, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5927= yyls; struct Cyc_Yyltype* _temp5929= _temp5927.curr
+( yylsp_offset - 3); if( _temp5929 < _temp5927.base? 1: _temp5929 >= _temp5927.last_plus_one){
_throw( Null_Exception);}* _temp5929;}).first_line,({ struct _tagged_ptr3
_temp5930= yyls; struct Cyc_Yyltype* _temp5932= _temp5930.curr + yylsp_offset;
if( _temp5932 < _temp5930.base? 1: _temp5932 >= _temp5930.last_plus_one){ _throw(
Null_Exception);}* _temp5932;}).last_line));( struct _xenum_struct*) _temp5923;});
break; case 332: _LL5922: yyval=({ struct Cyc_Exp_tok_struct* _temp5934=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp5934->tag=
Cyc_Exp_tok_tag; _temp5934->f1= Cyc_Absyn_sizeofexp_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5935= yyvs; struct _xenum_struct** _temp5937= _temp5935.curr
+ yyvsp_offset; if( _temp5937 < _temp5935.base? 1: _temp5937 >= _temp5935.last_plus_one){
_throw( Null_Exception);}* _temp5937;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5938= yyls; struct Cyc_Yyltype* _temp5940= _temp5938.curr +(
yylsp_offset - 1); if( _temp5940 < _temp5938.base? 1: _temp5940 >= _temp5938.last_plus_one){
_throw( Null_Exception);}* _temp5940;}).first_line,({ struct _tagged_ptr3
_temp5941= yyls; struct Cyc_Yyltype* _temp5943= _temp5941.curr + yylsp_offset;
if( _temp5943 < _temp5941.base? 1: _temp5943 >= _temp5941.last_plus_one){ _throw(
Null_Exception);}* _temp5943;}).last_line));( struct _xenum_struct*) _temp5934;});
break; case 333: _LL5933: yyval=({ struct Cyc_Exp_tok_struct* _temp5945=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp5945->tag=
Cyc_Exp_tok_tag; _temp5945->f1= Cyc_Absyn_throw_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5946= yyvs; struct _xenum_struct** _temp5948= _temp5946.curr +
yyvsp_offset; if( _temp5948 < _temp5946.base? 1: _temp5948 >= _temp5946.last_plus_one){
_throw( Null_Exception);}* _temp5948;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5949= yyls; struct Cyc_Yyltype* _temp5951= _temp5949.curr +(
yylsp_offset - 1); if( _temp5951 < _temp5949.base? 1: _temp5951 >= _temp5949.last_plus_one){
_throw( Null_Exception);}* _temp5951;}).first_line,({ struct _tagged_ptr3
_temp5952= yyls; struct Cyc_Yyltype* _temp5954= _temp5952.curr + yylsp_offset;
if( _temp5954 < _temp5952.base? 1: _temp5954 >= _temp5952.last_plus_one){ _throw(
Null_Exception);}* _temp5954;}).last_line));( struct _xenum_struct*) _temp5945;});
break; case 334: _LL5944: yyval=({ struct Cyc_Exp_tok_struct* _temp5956=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp5956->tag=
Cyc_Exp_tok_tag; _temp5956->f1= Cyc_Absyn_primop_exp( Cyc_yyget_Primop_tok(({
struct _tagged_ptr2 _temp5957= yyvs; struct _xenum_struct** _temp5959= _temp5957.curr
+( yyvsp_offset - 3); if( _temp5959 < _temp5957.base? 1: _temp5959 >= _temp5957.last_plus_one){
_throw( Null_Exception);}* _temp5959;})), Cyc_yyget_ExpList_tok(({ struct
_tagged_ptr2 _temp5960= yyvs; struct _xenum_struct** _temp5962= _temp5960.curr +(
yyvsp_offset - 1); if( _temp5962 < _temp5960.base? 1: _temp5962 >= _temp5960.last_plus_one){
_throw( Null_Exception);}* _temp5962;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5963= yyls; struct Cyc_Yyltype* _temp5965= _temp5963.curr +(
yylsp_offset - 3); if( _temp5965 < _temp5963.base? 1: _temp5965 >= _temp5963.last_plus_one){
_throw( Null_Exception);}* _temp5965;}).first_line,({ struct _tagged_ptr3
_temp5966= yyls; struct Cyc_Yyltype* _temp5968= _temp5966.curr + yylsp_offset;
if( _temp5968 < _temp5966.base? 1: _temp5968 >= _temp5966.last_plus_one){ _throw(
Null_Exception);}* _temp5968;}).last_line));( struct _xenum_struct*) _temp5956;});
break; case 335: _LL5955: yyval=({ struct Cyc_Exp_tok_struct* _temp5970=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp5970->tag=
Cyc_Exp_tok_tag; _temp5970->f1= Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Malloc_e_struct*
_temp5971=( struct Cyc_Absyn_Malloc_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp5971->tag= Cyc_Absyn_Malloc_e_tag; _temp5971->f1=( void*)({ struct Cyc_Absyn_Typ_m_struct*
_temp5972=( struct Cyc_Absyn_Typ_m_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Typ_m_struct));
_temp5972->tag= Cyc_Absyn_Typ_m_tag; _temp5972->f1=( void*) Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp5973= yyvs; struct
_xenum_struct** _temp5975= _temp5973.curr +( yyvsp_offset - 2); if( _temp5975 <
_temp5973.base? 1: _temp5975 >= _temp5973.last_plus_one){ _throw( Null_Exception);}*
_temp5975;}))).f2, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5976=
yyls; struct Cyc_Yyltype* _temp5978= _temp5976.curr +( yylsp_offset - 2); if(
_temp5978 < _temp5976.base? 1: _temp5978 >= _temp5976.last_plus_one){ _throw(
Null_Exception);}* _temp5978;}).first_line,({ struct _tagged_ptr3 _temp5979=
yyls; struct Cyc_Yyltype* _temp5981= _temp5979.curr +( yylsp_offset - 2); if(
_temp5981 < _temp5979.base? 1: _temp5981 >= _temp5979.last_plus_one){ _throw(
Null_Exception);}* _temp5981;}).last_line));( void*) _temp5972;});( void*)
_temp5971;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5982= yyls;
struct Cyc_Yyltype* _temp5984= _temp5982.curr +( yylsp_offset - 6); if(
_temp5984 < _temp5982.base? 1: _temp5984 >= _temp5982.last_plus_one){ _throw(
Null_Exception);}* _temp5984;}).first_line,({ struct _tagged_ptr3 _temp5985=
yyls; struct Cyc_Yyltype* _temp5987= _temp5985.curr + yylsp_offset; if(
_temp5987 < _temp5985.base? 1: _temp5987 >= _temp5985.last_plus_one){ _throw(
Null_Exception);}* _temp5987;}).last_line));( struct _xenum_struct*) _temp5970;});
break; case 336: _LL5969: yyval=({ struct Cyc_Exp_tok_struct* _temp5989=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp5989->tag=
Cyc_Exp_tok_tag; _temp5989->f1= Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Malloc_e_struct*
_temp5990=( struct Cyc_Absyn_Malloc_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp5990->tag= Cyc_Absyn_Malloc_e_tag; _temp5990->f1=( void*)({ struct Cyc_Absyn_Unresolved_m_struct*
_temp5991=( struct Cyc_Absyn_Unresolved_m_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Unresolved_m_struct));
_temp5991->tag= Cyc_Absyn_Unresolved_m_tag; _temp5991->f1= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp5992= yyvs; struct _xenum_struct** _temp5994= _temp5992.curr
+( yyvsp_offset - 2); if( _temp5994 < _temp5992.base? 1: _temp5994 >= _temp5992.last_plus_one){
_throw( Null_Exception);}* _temp5994;}));( void*) _temp5991;});( void*)
_temp5990;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5995= yyls;
struct Cyc_Yyltype* _temp5997= _temp5995.curr +( yylsp_offset - 6); if(
_temp5997 < _temp5995.base? 1: _temp5997 >= _temp5995.last_plus_one){ _throw(
Null_Exception);}* _temp5997;}).first_line,({ struct _tagged_ptr3 _temp5998=
yyls; struct Cyc_Yyltype* _temp6000= _temp5998.curr + yylsp_offset; if(
_temp6000 < _temp5998.base? 1: _temp6000 >= _temp5998.last_plus_one){ _throw(
Null_Exception);}* _temp6000;}).last_line));( struct _xenum_struct*) _temp5989;});
break; case 337: _LL5988: yyval=({ struct Cyc_Primop_tok_struct* _temp6002=(
struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp6002->tag= Cyc_Primop_tok_tag; _temp6002->f1=( void*) Cyc_Absyn_Printf;(
struct _xenum_struct*) _temp6002;}); break; case 338: _LL6001: yyval=({ struct
Cyc_Primop_tok_struct* _temp6004=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp6004->tag= Cyc_Primop_tok_tag;
_temp6004->f1=( void*) Cyc_Absyn_Fprintf;( struct _xenum_struct*) _temp6004;});
break; case 339: _LL6003: yyval=({ struct Cyc_Primop_tok_struct* _temp6006=(
struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp6006->tag= Cyc_Primop_tok_tag; _temp6006->f1=( void*) Cyc_Absyn_Xprintf;(
struct _xenum_struct*) _temp6006;}); break; case 340: _LL6005: yyval=({ struct
Cyc_Primop_tok_struct* _temp6008=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp6008->tag= Cyc_Primop_tok_tag;
_temp6008->f1=( void*) Cyc_Absyn_Scanf;( struct _xenum_struct*) _temp6008;});
break; case 341: _LL6007: yyval=({ struct Cyc_Primop_tok_struct* _temp6010=(
struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp6010->tag= Cyc_Primop_tok_tag; _temp6010->f1=( void*) Cyc_Absyn_Fscanf;(
struct _xenum_struct*) _temp6010;}); break; case 342: _LL6009: yyval=({ struct
Cyc_Primop_tok_struct* _temp6012=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp6012->tag= Cyc_Primop_tok_tag;
_temp6012->f1=( void*) Cyc_Absyn_Sscanf;( struct _xenum_struct*) _temp6012;});
break; case 343: _LL6011: yyval=({ struct Cyc_Primop_tok_struct* _temp6014=(
struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp6014->tag= Cyc_Primop_tok_tag; _temp6014->f1=( void*) Cyc_Absyn_Bitnot;(
struct _xenum_struct*) _temp6014;}); break; case 344: _LL6013: yyval=({ struct
Cyc_Primop_tok_struct* _temp6016=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp6016->tag= Cyc_Primop_tok_tag;
_temp6016->f1=( void*) Cyc_Absyn_Not;( struct _xenum_struct*) _temp6016;});
break; case 345: _LL6015: yyval=({ struct Cyc_Primop_tok_struct* _temp6018=(
struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp6018->tag= Cyc_Primop_tok_tag; _temp6018->f1=( void*) Cyc_Absyn_Minus;(
struct _xenum_struct*) _temp6018;}); break; case 346: _LL6017: yyval=({ struct
_tagged_ptr2 _temp6020= yyvs; struct _xenum_struct** _temp6022= _temp6020.curr +
yyvsp_offset; if( _temp6022 < _temp6020.base? 1: _temp6022 >= _temp6020.last_plus_one){
_throw( Null_Exception);}* _temp6022;}); break; case 347: _LL6019: yyval=({
struct Cyc_Exp_tok_struct* _temp6024=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp6024->tag= Cyc_Exp_tok_tag; _temp6024->f1=
Cyc_Absyn_subscript_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6025=
yyvs; struct _xenum_struct** _temp6027= _temp6025.curr +( yyvsp_offset - 3); if(
_temp6027 < _temp6025.base? 1: _temp6027 >= _temp6025.last_plus_one){ _throw(
Null_Exception);}* _temp6027;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp6028= yyvs; struct _xenum_struct** _temp6030= _temp6028.curr +(
yyvsp_offset - 1); if( _temp6030 < _temp6028.base? 1: _temp6030 >= _temp6028.last_plus_one){
_throw( Null_Exception);}* _temp6030;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6031= yyls; struct Cyc_Yyltype* _temp6033= _temp6031.curr +(
yylsp_offset - 3); if( _temp6033 < _temp6031.base? 1: _temp6033 >= _temp6031.last_plus_one){
_throw( Null_Exception);}* _temp6033;}).first_line,({ struct _tagged_ptr3
_temp6034= yyls; struct Cyc_Yyltype* _temp6036= _temp6034.curr + yylsp_offset;
if( _temp6036 < _temp6034.base? 1: _temp6036 >= _temp6034.last_plus_one){ _throw(
Null_Exception);}* _temp6036;}).last_line));( struct _xenum_struct*) _temp6024;});
break; case 348: _LL6023: yyval=({ struct Cyc_Exp_tok_struct* _temp6038=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6038->tag=
Cyc_Exp_tok_tag; _temp6038->f1= Cyc_Absyn_unknowncall_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6039= yyvs; struct _xenum_struct** _temp6041= _temp6039.curr
+( yyvsp_offset - 2); if( _temp6041 < _temp6039.base? 1: _temp6041 >= _temp6039.last_plus_one){
_throw( Null_Exception);}* _temp6041;})), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6042= yyls; struct Cyc_Yyltype* _temp6044= _temp6042.curr
+( yylsp_offset - 2); if( _temp6044 < _temp6042.base? 1: _temp6044 >= _temp6042.last_plus_one){
_throw( Null_Exception);}* _temp6044;}).first_line,({ struct _tagged_ptr3
_temp6045= yyls; struct Cyc_Yyltype* _temp6047= _temp6045.curr + yylsp_offset;
if( _temp6047 < _temp6045.base? 1: _temp6047 >= _temp6045.last_plus_one){ _throw(
Null_Exception);}* _temp6047;}).last_line));( struct _xenum_struct*) _temp6038;});
break; case 349: _LL6037: yyval=({ struct Cyc_Exp_tok_struct* _temp6049=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6049->tag=
Cyc_Exp_tok_tag; _temp6049->f1= Cyc_Absyn_unknowncall_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6050= yyvs; struct _xenum_struct** _temp6052= _temp6050.curr
+( yyvsp_offset - 3); if( _temp6052 < _temp6050.base? 1: _temp6052 >= _temp6050.last_plus_one){
_throw( Null_Exception);}* _temp6052;})), Cyc_yyget_ExpList_tok(({ struct
_tagged_ptr2 _temp6053= yyvs; struct _xenum_struct** _temp6055= _temp6053.curr +(
yyvsp_offset - 1); if( _temp6055 < _temp6053.base? 1: _temp6055 >= _temp6053.last_plus_one){
_throw( Null_Exception);}* _temp6055;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6056= yyls; struct Cyc_Yyltype* _temp6058= _temp6056.curr +(
yylsp_offset - 3); if( _temp6058 < _temp6056.base? 1: _temp6058 >= _temp6056.last_plus_one){
_throw( Null_Exception);}* _temp6058;}).first_line,({ struct _tagged_ptr3
_temp6059= yyls; struct Cyc_Yyltype* _temp6061= _temp6059.curr + yylsp_offset;
if( _temp6061 < _temp6059.base? 1: _temp6061 >= _temp6059.last_plus_one){ _throw(
Null_Exception);}* _temp6061;}).last_line));( struct _xenum_struct*) _temp6049;});
break; case 350: _LL6048: yyval=({ struct Cyc_Exp_tok_struct* _temp6063=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6063->tag=
Cyc_Exp_tok_tag; _temp6063->f1= Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6064= yyvs; struct _xenum_struct** _temp6066= _temp6064.curr
+( yyvsp_offset - 2); if( _temp6066 < _temp6064.base? 1: _temp6066 >= _temp6064.last_plus_one){
_throw( Null_Exception);}* _temp6066;})),({ struct _tagged_string* _temp6067=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp6067[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp6068= yyvs;
struct _xenum_struct** _temp6070= _temp6068.curr + yyvsp_offset; if( _temp6070 <
_temp6068.base? 1: _temp6070 >= _temp6068.last_plus_one){ _throw( Null_Exception);}*
_temp6070;})); _temp6067;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp6071= yyls; struct Cyc_Yyltype* _temp6073= _temp6071.curr +( yylsp_offset -
2); if( _temp6073 < _temp6071.base? 1: _temp6073 >= _temp6071.last_plus_one){
_throw( Null_Exception);}* _temp6073;}).first_line,({ struct _tagged_ptr3
_temp6074= yyls; struct Cyc_Yyltype* _temp6076= _temp6074.curr + yylsp_offset;
if( _temp6076 < _temp6074.base? 1: _temp6076 >= _temp6074.last_plus_one){ _throw(
Null_Exception);}* _temp6076;}).last_line));( struct _xenum_struct*) _temp6063;});
break; case 351: _LL6062: { struct _tuple1* q= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp6078= yyvs; struct _xenum_struct** _temp6080= _temp6078.curr +
yyvsp_offset; if( _temp6080 < _temp6078.base? 1: _temp6080 >= _temp6078.last_plus_one){
_throw( Null_Exception);}* _temp6080;}));{ void* _temp6081=(* q).f1; struct Cyc_List_List*
_temp6091; struct Cyc_List_List* _temp6093; _LL6083: if( _temp6081 == Cyc_Absyn_Loc_n){
goto _LL6084;} else{ goto _LL6085;} _LL6085: if(( unsigned int) _temp6081 > 1u?((
struct _enum_struct*) _temp6081)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL6092:
_temp6091=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp6081)->f1;
if( _temp6091 == 0){ goto _LL6086;} else{ goto _LL6087;}} else{ goto _LL6087;}
_LL6087: if(( unsigned int) _temp6081 > 1u?(( struct _enum_struct*) _temp6081)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL6094: _temp6093=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp6081)->f1; if( _temp6093 == 0){ goto _LL6088;}
else{ goto _LL6089;}} else{ goto _LL6089;} _LL6089: goto _LL6090; _LL6084: goto
_LL6082; _LL6086: goto _LL6082; _LL6088: goto _LL6082; _LL6090: Cyc_Parse_err((
struct _tagged_string)({ char* _temp6095=( char*)"struct field name is qualified";
struct _tagged_string _temp6096; _temp6096.curr= _temp6095; _temp6096.base=
_temp6095; _temp6096.last_plus_one= _temp6095 + 31; _temp6096;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6097= yyls; struct Cyc_Yyltype* _temp6099= _temp6097.curr
+ yylsp_offset; if( _temp6099 < _temp6097.base? 1: _temp6099 >= _temp6097.last_plus_one){
_throw( Null_Exception);}* _temp6099;}).first_line,({ struct _tagged_ptr3
_temp6100= yyls; struct Cyc_Yyltype* _temp6102= _temp6100.curr + yylsp_offset;
if( _temp6102 < _temp6100.base? 1: _temp6102 >= _temp6100.last_plus_one){ _throw(
Null_Exception);}* _temp6102;}).last_line)); goto _LL6082; _LL6082:;} yyval=({
struct Cyc_Exp_tok_struct* _temp6103=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp6103->tag= Cyc_Exp_tok_tag; _temp6103->f1=
Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6104=
yyvs; struct _xenum_struct** _temp6106= _temp6104.curr +( yyvsp_offset - 2); if(
_temp6106 < _temp6104.base? 1: _temp6106 >= _temp6104.last_plus_one){ _throw(
Null_Exception);}* _temp6106;})),(* q).f2, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6107= yyls; struct Cyc_Yyltype* _temp6109= _temp6107.curr +(
yylsp_offset - 2); if( _temp6109 < _temp6107.base? 1: _temp6109 >= _temp6107.last_plus_one){
_throw( Null_Exception);}* _temp6109;}).first_line,({ struct _tagged_ptr3
_temp6110= yyls; struct Cyc_Yyltype* _temp6112= _temp6110.curr + yylsp_offset;
if( _temp6112 < _temp6110.base? 1: _temp6112 >= _temp6110.last_plus_one){ _throw(
Null_Exception);}* _temp6112;}).last_line));( struct _xenum_struct*) _temp6103;});
break;} case 352: _LL6077: yyval=({ struct Cyc_Exp_tok_struct* _temp6114=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6114->tag= Cyc_Exp_tok_tag; _temp6114->f1= Cyc_Absyn_structarrow_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6115= yyvs; struct _xenum_struct** _temp6117= _temp6115.curr
+( yyvsp_offset - 2); if( _temp6117 < _temp6115.base? 1: _temp6117 >= _temp6115.last_plus_one){
_throw( Null_Exception);}* _temp6117;})),({ struct _tagged_string* _temp6118=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp6118[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp6119= yyvs;
struct _xenum_struct** _temp6121= _temp6119.curr + yyvsp_offset; if( _temp6121 <
_temp6119.base? 1: _temp6121 >= _temp6119.last_plus_one){ _throw( Null_Exception);}*
_temp6121;})); _temp6118;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp6122= yyls; struct Cyc_Yyltype* _temp6124= _temp6122.curr +( yylsp_offset -
2); if( _temp6124 < _temp6122.base? 1: _temp6124 >= _temp6122.last_plus_one){
_throw( Null_Exception);}* _temp6124;}).first_line,({ struct _tagged_ptr3
_temp6125= yyls; struct Cyc_Yyltype* _temp6127= _temp6125.curr + yylsp_offset;
if( _temp6127 < _temp6125.base? 1: _temp6127 >= _temp6125.last_plus_one){ _throw(
Null_Exception);}* _temp6127;}).last_line));( struct _xenum_struct*) _temp6114;});
break; case 353: _LL6113: { struct _tuple1* q= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp6129= yyvs; struct _xenum_struct** _temp6131= _temp6129.curr +
yyvsp_offset; if( _temp6131 < _temp6129.base? 1: _temp6131 >= _temp6129.last_plus_one){
_throw( Null_Exception);}* _temp6131;}));{ void* _temp6132=(* q).f1; struct Cyc_List_List*
_temp6142; struct Cyc_List_List* _temp6144; _LL6134: if( _temp6132 == Cyc_Absyn_Loc_n){
goto _LL6135;} else{ goto _LL6136;} _LL6136: if(( unsigned int) _temp6132 > 1u?((
struct _enum_struct*) _temp6132)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL6143:
_temp6142=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp6132)->f1;
if( _temp6142 == 0){ goto _LL6137;} else{ goto _LL6138;}} else{ goto _LL6138;}
_LL6138: if(( unsigned int) _temp6132 > 1u?(( struct _enum_struct*) _temp6132)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL6145: _temp6144=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp6132)->f1; if( _temp6144 == 0){ goto _LL6139;}
else{ goto _LL6140;}} else{ goto _LL6140;} _LL6140: goto _LL6141; _LL6135: goto
_LL6133; _LL6137: goto _LL6133; _LL6139: goto _LL6133; _LL6141: Cyc_Parse_err((
struct _tagged_string)({ char* _temp6146=( char*)"struct field is qualified with module name";
struct _tagged_string _temp6147; _temp6147.curr= _temp6146; _temp6147.base=
_temp6146; _temp6147.last_plus_one= _temp6146 + 43; _temp6147;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6148= yyls; struct Cyc_Yyltype* _temp6150= _temp6148.curr
+ yylsp_offset; if( _temp6150 < _temp6148.base? 1: _temp6150 >= _temp6148.last_plus_one){
_throw( Null_Exception);}* _temp6150;}).first_line,({ struct _tagged_ptr3
_temp6151= yyls; struct Cyc_Yyltype* _temp6153= _temp6151.curr + yylsp_offset;
if( _temp6153 < _temp6151.base? 1: _temp6153 >= _temp6151.last_plus_one){ _throw(
Null_Exception);}* _temp6153;}).last_line)); goto _LL6133; _LL6133:;} yyval=({
struct Cyc_Exp_tok_struct* _temp6154=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp6154->tag= Cyc_Exp_tok_tag; _temp6154->f1=
Cyc_Absyn_structarrow_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6155=
yyvs; struct _xenum_struct** _temp6157= _temp6155.curr +( yyvsp_offset - 2); if(
_temp6157 < _temp6155.base? 1: _temp6157 >= _temp6155.last_plus_one){ _throw(
Null_Exception);}* _temp6157;})),(* q).f2, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6158= yyls; struct Cyc_Yyltype* _temp6160= _temp6158.curr +(
yylsp_offset - 2); if( _temp6160 < _temp6158.base? 1: _temp6160 >= _temp6158.last_plus_one){
_throw( Null_Exception);}* _temp6160;}).first_line,({ struct _tagged_ptr3
_temp6161= yyls; struct Cyc_Yyltype* _temp6163= _temp6161.curr + yylsp_offset;
if( _temp6163 < _temp6161.base? 1: _temp6163 >= _temp6161.last_plus_one){ _throw(
Null_Exception);}* _temp6163;}).last_line));( struct _xenum_struct*) _temp6154;});
break;} case 354: _LL6128: yyval=({ struct Cyc_Exp_tok_struct* _temp6165=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6165->tag= Cyc_Exp_tok_tag; _temp6165->f1= Cyc_Absyn_post_inc_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6166= yyvs; struct _xenum_struct** _temp6168= _temp6166.curr
+( yyvsp_offset - 1); if( _temp6168 < _temp6166.base? 1: _temp6168 >= _temp6166.last_plus_one){
_throw( Null_Exception);}* _temp6168;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6169= yyls; struct Cyc_Yyltype* _temp6171= _temp6169.curr +(
yylsp_offset - 1); if( _temp6171 < _temp6169.base? 1: _temp6171 >= _temp6169.last_plus_one){
_throw( Null_Exception);}* _temp6171;}).first_line,({ struct _tagged_ptr3
_temp6172= yyls; struct Cyc_Yyltype* _temp6174= _temp6172.curr + yylsp_offset;
if( _temp6174 < _temp6172.base? 1: _temp6174 >= _temp6172.last_plus_one){ _throw(
Null_Exception);}* _temp6174;}).last_line));( struct _xenum_struct*) _temp6165;});
break; case 355: _LL6164: yyval=({ struct Cyc_Exp_tok_struct* _temp6176=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6176->tag=
Cyc_Exp_tok_tag; _temp6176->f1= Cyc_Absyn_post_dec_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6177= yyvs; struct _xenum_struct** _temp6179= _temp6177.curr
+( yyvsp_offset - 1); if( _temp6179 < _temp6177.base? 1: _temp6179 >= _temp6177.last_plus_one){
_throw( Null_Exception);}* _temp6179;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6180= yyls; struct Cyc_Yyltype* _temp6182= _temp6180.curr +(
yylsp_offset - 1); if( _temp6182 < _temp6180.base? 1: _temp6182 >= _temp6180.last_plus_one){
_throw( Null_Exception);}* _temp6182;}).first_line,({ struct _tagged_ptr3
_temp6183= yyls; struct Cyc_Yyltype* _temp6185= _temp6183.curr + yylsp_offset;
if( _temp6185 < _temp6183.base? 1: _temp6185 >= _temp6183.last_plus_one){ _throw(
Null_Exception);}* _temp6185;}).last_line));( struct _xenum_struct*) _temp6176;});
break; case 356: _LL6175: yyval=({ struct Cyc_Exp_tok_struct* _temp6187=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6187->tag=
Cyc_Exp_tok_tag; _temp6187->f1= Cyc_Absyn_new_exp(({ struct Cyc_Absyn_CompoundLit_e_struct*
_temp6188=( struct Cyc_Absyn_CompoundLit_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_CompoundLit_e_struct)); _temp6188->tag= Cyc_Absyn_CompoundLit_e_tag;
_temp6188->f1= Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp6189= yyvs;
struct _xenum_struct** _temp6191= _temp6189.curr +( yyvsp_offset - 4); if(
_temp6191 < _temp6189.base? 1: _temp6191 >= _temp6189.last_plus_one){ _throw(
Null_Exception);}* _temp6191;})); _temp6188->f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp6192= yyvs; struct _xenum_struct** _temp6194= _temp6192.curr
+( yyvsp_offset - 1); if( _temp6194 < _temp6192.base? 1: _temp6194 >= _temp6192.last_plus_one){
_throw( Null_Exception);}* _temp6194;})));( void*) _temp6188;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6195= yyls; struct Cyc_Yyltype* _temp6197= _temp6195.curr
+( yylsp_offset - 5); if( _temp6197 < _temp6195.base? 1: _temp6197 >= _temp6195.last_plus_one){
_throw( Null_Exception);}* _temp6197;}).first_line,({ struct _tagged_ptr3
_temp6198= yyls; struct Cyc_Yyltype* _temp6200= _temp6198.curr + yylsp_offset;
if( _temp6200 < _temp6198.base? 1: _temp6200 >= _temp6198.last_plus_one){ _throw(
Null_Exception);}* _temp6200;}).last_line));( struct _xenum_struct*) _temp6187;});
break; case 357: _LL6186: yyval=({ struct Cyc_Exp_tok_struct* _temp6202=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6202->tag=
Cyc_Exp_tok_tag; _temp6202->f1= Cyc_Absyn_new_exp(({ struct Cyc_Absyn_CompoundLit_e_struct*
_temp6203=( struct Cyc_Absyn_CompoundLit_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_CompoundLit_e_struct)); _temp6203->tag= Cyc_Absyn_CompoundLit_e_tag;
_temp6203->f1= Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp6204= yyvs;
struct _xenum_struct** _temp6206= _temp6204.curr +( yyvsp_offset - 5); if(
_temp6206 < _temp6204.base? 1: _temp6206 >= _temp6204.last_plus_one){ _throw(
Null_Exception);}* _temp6206;})); _temp6203->f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp6207= yyvs; struct _xenum_struct** _temp6209= _temp6207.curr
+( yyvsp_offset - 2); if( _temp6209 < _temp6207.base? 1: _temp6209 >= _temp6207.last_plus_one){
_throw( Null_Exception);}* _temp6209;})));( void*) _temp6203;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6210= yyls; struct Cyc_Yyltype* _temp6212= _temp6210.curr
+( yylsp_offset - 6); if( _temp6212 < _temp6210.base? 1: _temp6212 >= _temp6210.last_plus_one){
_throw( Null_Exception);}* _temp6212;}).first_line,({ struct _tagged_ptr3
_temp6213= yyls; struct Cyc_Yyltype* _temp6215= _temp6213.curr + yylsp_offset;
if( _temp6215 < _temp6213.base? 1: _temp6215 >= _temp6213.last_plus_one){ _throw(
Null_Exception);}* _temp6215;}).last_line));( struct _xenum_struct*) _temp6202;});
break; case 358: _LL6201: yyval=({ struct Cyc_Exp_tok_struct* _temp6217=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6217->tag=
Cyc_Exp_tok_tag; _temp6217->f1= Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Fill_e_struct*
_temp6218=( struct Cyc_Absyn_Fill_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fill_e_struct));
_temp6218->tag= Cyc_Absyn_Fill_e_tag; _temp6218->f1= Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6219= yyvs; struct _xenum_struct** _temp6221= _temp6219.curr +(
yyvsp_offset - 1); if( _temp6221 < _temp6219.base? 1: _temp6221 >= _temp6219.last_plus_one){
_throw( Null_Exception);}* _temp6221;}));( void*) _temp6218;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6222= yyls; struct Cyc_Yyltype* _temp6224= _temp6222.curr
+( yylsp_offset - 3); if( _temp6224 < _temp6222.base? 1: _temp6224 >= _temp6222.last_plus_one){
_throw( Null_Exception);}* _temp6224;}).first_line,({ struct _tagged_ptr3
_temp6225= yyls; struct Cyc_Yyltype* _temp6227= _temp6225.curr + yylsp_offset;
if( _temp6227 < _temp6225.base? 1: _temp6227 >= _temp6225.last_plus_one){ _throw(
Null_Exception);}* _temp6227;}).last_line));( struct _xenum_struct*) _temp6217;});
break; case 359: _LL6216: yyval=({ struct Cyc_Exp_tok_struct* _temp6229=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6229->tag=
Cyc_Exp_tok_tag; _temp6229->f1= Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Codegen_e_struct*
_temp6230=( struct Cyc_Absyn_Codegen_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Codegen_e_struct));
_temp6230->tag= Cyc_Absyn_Codegen_e_tag; _temp6230->f1= Cyc_yyget_FnDecl_tok(({
struct _tagged_ptr2 _temp6231= yyvs; struct _xenum_struct** _temp6233= _temp6231.curr
+( yyvsp_offset - 1); if( _temp6233 < _temp6231.base? 1: _temp6233 >= _temp6231.last_plus_one){
_throw( Null_Exception);}* _temp6233;}));( void*) _temp6230;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6234= yyls; struct Cyc_Yyltype* _temp6236= _temp6234.curr
+( yylsp_offset - 3); if( _temp6236 < _temp6234.base? 1: _temp6236 >= _temp6234.last_plus_one){
_throw( Null_Exception);}* _temp6236;}).first_line,({ struct _tagged_ptr3
_temp6237= yyls; struct Cyc_Yyltype* _temp6239= _temp6237.curr + yylsp_offset;
if( _temp6239 < _temp6237.base? 1: _temp6239 >= _temp6237.last_plus_one){ _throw(
Null_Exception);}* _temp6239;}).last_line));( struct _xenum_struct*) _temp6229;});
break; case 360: _LL6228: yyval=({ struct Cyc_Exp_tok_struct* _temp6241=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6241->tag=
Cyc_Exp_tok_tag; _temp6241->f1= Cyc_Absyn_new_exp(({ struct Cyc_Absyn_UnknownId_e_struct*
_temp6242=( struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp6242->tag= Cyc_Absyn_UnknownId_e_tag; _temp6242->f1= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp6243= yyvs; struct _xenum_struct** _temp6245= _temp6243.curr
+ yyvsp_offset; if( _temp6245 < _temp6243.base? 1: _temp6245 >= _temp6243.last_plus_one){
_throw( Null_Exception);}* _temp6245;}));( void*) _temp6242;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6246= yyls; struct Cyc_Yyltype* _temp6248= _temp6246.curr
+ yylsp_offset; if( _temp6248 < _temp6246.base? 1: _temp6248 >= _temp6246.last_plus_one){
_throw( Null_Exception);}* _temp6248;}).first_line,({ struct _tagged_ptr3
_temp6249= yyls; struct Cyc_Yyltype* _temp6251= _temp6249.curr + yylsp_offset;
if( _temp6251 < _temp6249.base? 1: _temp6251 >= _temp6249.last_plus_one){ _throw(
Null_Exception);}* _temp6251;}).last_line));( struct _xenum_struct*) _temp6241;});
break; case 361: _LL6240: yyval=({ struct _tagged_ptr2 _temp6253= yyvs; struct
_xenum_struct** _temp6255= _temp6253.curr + yyvsp_offset; if( _temp6255 <
_temp6253.base? 1: _temp6255 >= _temp6253.last_plus_one){ _throw( Null_Exception);}*
_temp6255;}); break; case 362: _LL6252: yyval=({ struct Cyc_Exp_tok_struct*
_temp6257=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6257->tag= Cyc_Exp_tok_tag; _temp6257->f1= Cyc_Absyn_string_exp( Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp6258= yyvs; struct _xenum_struct** _temp6260= _temp6258.curr
+ yyvsp_offset; if( _temp6260 < _temp6258.base? 1: _temp6260 >= _temp6258.last_plus_one){
_throw( Null_Exception);}* _temp6260;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6261= yyls; struct Cyc_Yyltype* _temp6263= _temp6261.curr +
yylsp_offset; if( _temp6263 < _temp6261.base? 1: _temp6263 >= _temp6261.last_plus_one){
_throw( Null_Exception);}* _temp6263;}).first_line,({ struct _tagged_ptr3
_temp6264= yyls; struct Cyc_Yyltype* _temp6266= _temp6264.curr + yylsp_offset;
if( _temp6266 < _temp6264.base? 1: _temp6266 >= _temp6264.last_plus_one){ _throw(
Null_Exception);}* _temp6266;}).last_line));( struct _xenum_struct*) _temp6257;});
break; case 363: _LL6256: yyval=({ struct _tagged_ptr2 _temp6268= yyvs; struct
_xenum_struct** _temp6270= _temp6268.curr +( yyvsp_offset - 1); if( _temp6270 <
_temp6268.base? 1: _temp6270 >= _temp6268.last_plus_one){ _throw( Null_Exception);}*
_temp6270;}); break; case 364: _LL6267: yyval=({ struct Cyc_Exp_tok_struct*
_temp6272=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6272->tag= Cyc_Exp_tok_tag; _temp6272->f1= Cyc_Absyn_noinstantiate_exp( Cyc_Absyn_new_exp(({
struct Cyc_Absyn_UnknownId_e_struct* _temp6273=( struct Cyc_Absyn_UnknownId_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp6273->tag= Cyc_Absyn_UnknownId_e_tag;
_temp6273->f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp6274= yyvs;
struct _xenum_struct** _temp6276= _temp6274.curr +( yyvsp_offset - 1); if(
_temp6276 < _temp6274.base? 1: _temp6276 >= _temp6274.last_plus_one){ _throw(
Null_Exception);}* _temp6276;}));( void*) _temp6273;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6277= yyls; struct Cyc_Yyltype* _temp6279= _temp6277.curr
+( yylsp_offset - 1); if( _temp6279 < _temp6277.base? 1: _temp6279 >= _temp6277.last_plus_one){
_throw( Null_Exception);}* _temp6279;}).first_line,({ struct _tagged_ptr3
_temp6280= yyls; struct Cyc_Yyltype* _temp6282= _temp6280.curr +( yylsp_offset -
1); if( _temp6282 < _temp6280.base? 1: _temp6282 >= _temp6280.last_plus_one){
_throw( Null_Exception);}* _temp6282;}).last_line)), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6283= yyls; struct Cyc_Yyltype* _temp6285= _temp6283.curr
+( yylsp_offset - 1); if( _temp6285 < _temp6283.base? 1: _temp6285 >= _temp6283.last_plus_one){
_throw( Null_Exception);}* _temp6285;}).first_line,({ struct _tagged_ptr3
_temp6286= yyls; struct Cyc_Yyltype* _temp6288= _temp6286.curr + yylsp_offset;
if( _temp6288 < _temp6286.base? 1: _temp6288 >= _temp6286.last_plus_one){ _throw(
Null_Exception);}* _temp6288;}).last_line));( struct _xenum_struct*) _temp6272;});
break; case 365: _LL6271: yyval=({ struct Cyc_Exp_tok_struct* _temp6290=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6290->tag=
Cyc_Exp_tok_tag; _temp6290->f1= Cyc_Absyn_instantiate_exp( Cyc_Absyn_new_exp(({
struct Cyc_Absyn_UnknownId_e_struct* _temp6291=( struct Cyc_Absyn_UnknownId_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp6291->tag= Cyc_Absyn_UnknownId_e_tag;
_temp6291->f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp6292= yyvs;
struct _xenum_struct** _temp6294= _temp6292.curr +( yyvsp_offset - 4); if(
_temp6294 < _temp6292.base? 1: _temp6294 >= _temp6292.last_plus_one){ _throw(
Null_Exception);}* _temp6294;}));( void*) _temp6291;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6295= yyls; struct Cyc_Yyltype* _temp6297= _temp6295.curr
+( yylsp_offset - 4); if( _temp6297 < _temp6295.base? 1: _temp6297 >= _temp6295.last_plus_one){
_throw( Null_Exception);}* _temp6297;}).first_line,({ struct _tagged_ptr3
_temp6298= yyls; struct Cyc_Yyltype* _temp6300= _temp6298.curr +( yylsp_offset -
4); if( _temp6300 < _temp6298.base? 1: _temp6300 >= _temp6298.last_plus_one){
_throw( Null_Exception);}* _temp6300;}).last_line)),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp6301= yyvs; struct _xenum_struct** _temp6303= _temp6301.curr +(
yyvsp_offset - 1); if( _temp6303 < _temp6301.base? 1: _temp6303 >= _temp6301.last_plus_one){
_throw( Null_Exception);}* _temp6303;}))), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6304= yyls; struct Cyc_Yyltype* _temp6306= _temp6304.curr +(
yylsp_offset - 4); if( _temp6306 < _temp6304.base? 1: _temp6306 >= _temp6304.last_plus_one){
_throw( Null_Exception);}* _temp6306;}).first_line,({ struct _tagged_ptr3
_temp6307= yyls; struct Cyc_Yyltype* _temp6309= _temp6307.curr + yylsp_offset;
if( _temp6309 < _temp6307.base? 1: _temp6309 >= _temp6307.last_plus_one){ _throw(
Null_Exception);}* _temp6309;}).last_line));( struct _xenum_struct*) _temp6290;});
break; case 366: _LL6289: yyval=({ struct Cyc_Exp_tok_struct* _temp6311=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6311->tag=
Cyc_Exp_tok_tag; _temp6311->f1= Cyc_Absyn_tuple_exp( Cyc_yyget_ExpList_tok(({
struct _tagged_ptr2 _temp6312= yyvs; struct _xenum_struct** _temp6314= _temp6312.curr
+( yyvsp_offset - 1); if( _temp6314 < _temp6312.base? 1: _temp6314 >= _temp6312.last_plus_one){
_throw( Null_Exception);}* _temp6314;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6315= yyls; struct Cyc_Yyltype* _temp6317= _temp6315.curr +(
yylsp_offset - 3); if( _temp6317 < _temp6315.base? 1: _temp6317 >= _temp6315.last_plus_one){
_throw( Null_Exception);}* _temp6317;}).first_line,({ struct _tagged_ptr3
_temp6318= yyls; struct Cyc_Yyltype* _temp6320= _temp6318.curr + yylsp_offset;
if( _temp6320 < _temp6318.base? 1: _temp6320 >= _temp6318.last_plus_one){ _throw(
Null_Exception);}* _temp6320;}).last_line));( struct _xenum_struct*) _temp6311;});
break; case 367: _LL6310: yyval=({ struct Cyc_Exp_tok_struct* _temp6322=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6322->tag=
Cyc_Exp_tok_tag; _temp6322->f1= Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Struct_e_struct*
_temp6323=( struct Cyc_Absyn_Struct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp6323->tag= Cyc_Absyn_Struct_e_tag; _temp6323->f1= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp6324= yyvs; struct _xenum_struct** _temp6326= _temp6324.curr
+( yyvsp_offset - 3); if( _temp6326 < _temp6324.base? 1: _temp6326 >= _temp6324.last_plus_one){
_throw( Null_Exception);}* _temp6326;})); _temp6323->f2= 0; _temp6323->f3=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp6327= yyvs; struct _xenum_struct** _temp6329= _temp6327.curr
+( yyvsp_offset - 1); if( _temp6329 < _temp6327.base? 1: _temp6329 >= _temp6327.last_plus_one){
_throw( Null_Exception);}* _temp6329;}))); _temp6323->f4= 0;( void*) _temp6323;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6330= yyls; struct Cyc_Yyltype*
_temp6332= _temp6330.curr +( yylsp_offset - 3); if( _temp6332 < _temp6330.base?
1: _temp6332 >= _temp6330.last_plus_one){ _throw( Null_Exception);}* _temp6332;}).first_line,({
struct _tagged_ptr3 _temp6333= yyls; struct Cyc_Yyltype* _temp6335= _temp6333.curr
+ yylsp_offset; if( _temp6335 < _temp6333.base? 1: _temp6335 >= _temp6333.last_plus_one){
_throw( Null_Exception);}* _temp6335;}).last_line));( struct _xenum_struct*)
_temp6322;}); break; case 368: _LL6321: yyval=({ struct Cyc_Exp_tok_struct*
_temp6337=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6337->tag= Cyc_Exp_tok_tag; _temp6337->f1= Cyc_Absyn_stmt_exp( Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp6338= yyvs; struct _xenum_struct** _temp6340= _temp6338.curr
+( yyvsp_offset - 2); if( _temp6340 < _temp6338.base? 1: _temp6340 >= _temp6338.last_plus_one){
_throw( Null_Exception);}* _temp6340;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6341= yyls; struct Cyc_Yyltype* _temp6343= _temp6341.curr +(
yylsp_offset - 4); if( _temp6343 < _temp6341.base? 1: _temp6343 >= _temp6341.last_plus_one){
_throw( Null_Exception);}* _temp6343;}).first_line,({ struct _tagged_ptr3
_temp6344= yyls; struct Cyc_Yyltype* _temp6346= _temp6344.curr + yylsp_offset;
if( _temp6346 < _temp6344.base? 1: _temp6346 >= _temp6344.last_plus_one){ _throw(
Null_Exception);}* _temp6346;}).last_line));( struct _xenum_struct*) _temp6337;});
break; case 369: _LL6336: yyval=({ struct Cyc_ExpList_tok_struct* _temp6348=(
struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp6348->tag= Cyc_ExpList_tok_tag; _temp6348->f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ExpList_tok(({ struct
_tagged_ptr2 _temp6349= yyvs; struct _xenum_struct** _temp6351= _temp6349.curr +
yyvsp_offset; if( _temp6351 < _temp6349.base? 1: _temp6351 >= _temp6349.last_plus_one){
_throw( Null_Exception);}* _temp6351;})));( struct _xenum_struct*) _temp6348;});
break; case 370: _LL6347: yyval=({ struct Cyc_ExpList_tok_struct* _temp6353=(
struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp6353->tag= Cyc_ExpList_tok_tag; _temp6353->f1=({ struct Cyc_List_List*
_temp6354=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp6354->hd=( void*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6355= yyvs;
struct _xenum_struct** _temp6357= _temp6355.curr + yyvsp_offset; if( _temp6357 <
_temp6355.base? 1: _temp6357 >= _temp6355.last_plus_one){ _throw( Null_Exception);}*
_temp6357;})); _temp6354->tl= 0; _temp6354;});( struct _xenum_struct*) _temp6353;});
break; case 371: _LL6352: yyval=({ struct Cyc_ExpList_tok_struct* _temp6359=(
struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp6359->tag= Cyc_ExpList_tok_tag; _temp6359->f1=({ struct Cyc_List_List*
_temp6360=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp6360->hd=( void*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6364= yyvs;
struct _xenum_struct** _temp6366= _temp6364.curr + yyvsp_offset; if( _temp6366 <
_temp6364.base? 1: _temp6366 >= _temp6364.last_plus_one){ _throw( Null_Exception);}*
_temp6366;})); _temp6360->tl= Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2
_temp6361= yyvs; struct _xenum_struct** _temp6363= _temp6361.curr +(
yyvsp_offset - 2); if( _temp6363 < _temp6361.base? 1: _temp6363 >= _temp6361.last_plus_one){
_throw( Null_Exception);}* _temp6363;})); _temp6360;});( struct _xenum_struct*)
_temp6359;}); break; case 372: _LL6358: yyval=({ struct Cyc_Exp_tok_struct*
_temp6368=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6368->tag= Cyc_Exp_tok_tag; _temp6368->f1= Cyc_Absyn_int_exp((* Cyc_yyget_Int_tok(({
struct _tagged_ptr2 _temp6369= yyvs; struct _xenum_struct** _temp6371= _temp6369.curr
+ yyvsp_offset; if( _temp6371 < _temp6369.base? 1: _temp6371 >= _temp6369.last_plus_one){
_throw( Null_Exception);}* _temp6371;}))).f1,(* Cyc_yyget_Int_tok(({ struct
_tagged_ptr2 _temp6372= yyvs; struct _xenum_struct** _temp6374= _temp6372.curr +
yyvsp_offset; if( _temp6374 < _temp6372.base? 1: _temp6374 >= _temp6372.last_plus_one){
_throw( Null_Exception);}* _temp6374;}))).f2, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6375= yyls; struct Cyc_Yyltype* _temp6377= _temp6375.curr
+ yylsp_offset; if( _temp6377 < _temp6375.base? 1: _temp6377 >= _temp6375.last_plus_one){
_throw( Null_Exception);}* _temp6377;}).first_line,({ struct _tagged_ptr3
_temp6378= yyls; struct Cyc_Yyltype* _temp6380= _temp6378.curr + yylsp_offset;
if( _temp6380 < _temp6378.base? 1: _temp6380 >= _temp6378.last_plus_one){ _throw(
Null_Exception);}* _temp6380;}).last_line));( struct _xenum_struct*) _temp6368;});
break; case 373: _LL6367: yyval=({ struct Cyc_Exp_tok_struct* _temp6382=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6382->tag=
Cyc_Exp_tok_tag; _temp6382->f1= Cyc_Absyn_char_exp( Cyc_yyget_Char_tok(({ struct
_tagged_ptr2 _temp6383= yyvs; struct _xenum_struct** _temp6385= _temp6383.curr +
yyvsp_offset; if( _temp6385 < _temp6383.base? 1: _temp6385 >= _temp6383.last_plus_one){
_throw( Null_Exception);}* _temp6385;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6386= yyls; struct Cyc_Yyltype* _temp6388= _temp6386.curr +
yylsp_offset; if( _temp6388 < _temp6386.base? 1: _temp6388 >= _temp6386.last_plus_one){
_throw( Null_Exception);}* _temp6388;}).first_line,({ struct _tagged_ptr3
_temp6389= yyls; struct Cyc_Yyltype* _temp6391= _temp6389.curr + yylsp_offset;
if( _temp6391 < _temp6389.base? 1: _temp6391 >= _temp6389.last_plus_one){ _throw(
Null_Exception);}* _temp6391;}).last_line));( struct _xenum_struct*) _temp6382;});
break; case 374: _LL6381: yyval=({ struct Cyc_Exp_tok_struct* _temp6393=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6393->tag=
Cyc_Exp_tok_tag; _temp6393->f1= Cyc_Absyn_float_exp( Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp6394= yyvs; struct _xenum_struct** _temp6396= _temp6394.curr
+ yyvsp_offset; if( _temp6396 < _temp6394.base? 1: _temp6396 >= _temp6394.last_plus_one){
_throw( Null_Exception);}* _temp6396;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6397= yyls; struct Cyc_Yyltype* _temp6399= _temp6397.curr +
yylsp_offset; if( _temp6399 < _temp6397.base? 1: _temp6399 >= _temp6397.last_plus_one){
_throw( Null_Exception);}* _temp6399;}).first_line,({ struct _tagged_ptr3
_temp6400= yyls; struct Cyc_Yyltype* _temp6402= _temp6400.curr + yylsp_offset;
if( _temp6402 < _temp6400.base? 1: _temp6402 >= _temp6400.last_plus_one){ _throw(
Null_Exception);}* _temp6402;}).last_line));( struct _xenum_struct*) _temp6393;});
break; case 375: _LL6392: yyval=({ struct Cyc_Exp_tok_struct* _temp6404=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6404->tag=
Cyc_Exp_tok_tag; _temp6404->f1= Cyc_Absyn_null_exp( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6405= yyls; struct Cyc_Yyltype* _temp6407= _temp6405.curr
+ yylsp_offset; if( _temp6407 < _temp6405.base? 1: _temp6407 >= _temp6405.last_plus_one){
_throw( Null_Exception);}* _temp6407;}).first_line,({ struct _tagged_ptr3
_temp6408= yyls; struct Cyc_Yyltype* _temp6410= _temp6408.curr + yylsp_offset;
if( _temp6410 < _temp6408.base? 1: _temp6410 >= _temp6408.last_plus_one){ _throw(
Null_Exception);}* _temp6410;}).last_line));( struct _xenum_struct*) _temp6404;});
break; case 376: _LL6403: yyval=({ struct Cyc_QualId_tok_struct* _temp6412=(
struct Cyc_QualId_tok_struct*) GC_malloc( sizeof( struct Cyc_QualId_tok_struct));
_temp6412->tag= Cyc_QualId_tok_tag; _temp6412->f1=({ struct _tuple1* _temp6413=(
struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp6413->f1=({ struct
Cyc_Absyn_Rel_n_struct* _temp6418=( struct Cyc_Absyn_Rel_n_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Rel_n_struct)); _temp6418->tag= Cyc_Absyn_Rel_n_tag;
_temp6418->f1= 0;( void*) _temp6418;}); _temp6413->f2=({ struct _tagged_string*
_temp6414=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp6414[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp6415= yyvs;
struct _xenum_struct** _temp6417= _temp6415.curr + yyvsp_offset; if( _temp6417 <
_temp6415.base? 1: _temp6417 >= _temp6415.last_plus_one){ _throw( Null_Exception);}*
_temp6417;})); _temp6414;}); _temp6413;});( struct _xenum_struct*) _temp6412;});
break; case 377: _LL6411: yyval=({ struct _tagged_ptr2 _temp6420= yyvs; struct
_xenum_struct** _temp6422= _temp6420.curr + yyvsp_offset; if( _temp6422 <
_temp6420.base? 1: _temp6422 >= _temp6420.last_plus_one){ _throw( Null_Exception);}*
_temp6422;}); break; default: _LL6419:( void) _throw(({ struct Cyc_Yyimpossible_struct*
_temp6424=( struct Cyc_Yyimpossible_struct*) GC_malloc_atomic( sizeof( struct
Cyc_Yyimpossible_struct)); _temp6424->tag= Cyc_Yyimpossible_tag;( struct
_xenum_struct*) _temp6424;}));} yyvsp_offset -= yylen; yyssp_offset -= yylen;
yylsp_offset -= yylen;({ struct _tagged_ptr2 _temp6425= yyvs; struct
_xenum_struct** _temp6427= _temp6425.curr +( ++ yyvsp_offset); if( _temp6427 <
_temp6425.base? 1: _temp6427 >= _temp6425.last_plus_one){ _throw( Null_Exception);}*
_temp6427= yyval;}); yylsp_offset ++; if( yylen == 0){({ struct _tagged_ptr3
_temp6428= yyls; struct Cyc_Yyltype* _temp6430= _temp6428.curr + yylsp_offset;
if( _temp6430 < _temp6428.base? 1: _temp6430 >= _temp6428.last_plus_one){ _throw(
Null_Exception);}(* _temp6430).first_line= Cyc_yylloc.first_line;});({ struct
_tagged_ptr3 _temp6431= yyls; struct Cyc_Yyltype* _temp6433= _temp6431.curr +
yylsp_offset; if( _temp6433 < _temp6431.base? 1: _temp6433 >= _temp6431.last_plus_one){
_throw( Null_Exception);}(* _temp6433).first_column= Cyc_yylloc.first_column;});({
struct _tagged_ptr3 _temp6434= yyls; struct Cyc_Yyltype* _temp6436= _temp6434.curr
+ yylsp_offset; if( _temp6436 < _temp6434.base? 1: _temp6436 >= _temp6434.last_plus_one){
_throw( Null_Exception);}(* _temp6436).last_line=({ struct _tagged_ptr3
_temp6437= yyls; struct Cyc_Yyltype* _temp6439= _temp6437.curr +( yylsp_offset -
1); if( _temp6439 < _temp6437.base? 1: _temp6439 >= _temp6437.last_plus_one){
_throw( Null_Exception);}* _temp6439;}).last_line;});({ struct _tagged_ptr3
_temp6440= yyls; struct Cyc_Yyltype* _temp6442= _temp6440.curr + yylsp_offset;
if( _temp6442 < _temp6440.base? 1: _temp6442 >= _temp6440.last_plus_one){ _throw(
Null_Exception);}(* _temp6442).last_column=({ struct _tagged_ptr3 _temp6443=
yyls; struct Cyc_Yyltype* _temp6445= _temp6443.curr +( yylsp_offset - 1); if(
_temp6445 < _temp6443.base? 1: _temp6445 >= _temp6443.last_plus_one){ _throw(
Null_Exception);}* _temp6445;}).last_column;});({ struct _tagged_ptr3 _temp6446=
yyls; struct Cyc_Yyltype* _temp6448= _temp6446.curr + yylsp_offset; if(
_temp6448 < _temp6446.base? 1: _temp6448 >= _temp6446.last_plus_one){ _throw(
Null_Exception);}(* _temp6448).text=( struct _tagged_string)({ char* _temp6449=(
char*)""; struct _tagged_string _temp6450; _temp6450.curr= _temp6449; _temp6450.base=
_temp6449; _temp6450.last_plus_one= _temp6449 + 1; _temp6450;});});} else{({
struct _tagged_ptr3 _temp6451= yyls; struct Cyc_Yyltype* _temp6453= _temp6451.curr
+ yylsp_offset; if( _temp6453 < _temp6451.base? 1: _temp6453 >= _temp6451.last_plus_one){
_throw( Null_Exception);}(* _temp6453).last_line=({ struct _tagged_ptr3
_temp6454= yyls; struct Cyc_Yyltype* _temp6456= _temp6454.curr +(( yylsp_offset
+ yylen) - 1); if( _temp6456 < _temp6454.base? 1: _temp6456 >= _temp6454.last_plus_one){
_throw( Null_Exception);}* _temp6456;}).last_line;});({ struct _tagged_ptr3
_temp6457= yyls; struct Cyc_Yyltype* _temp6459= _temp6457.curr + yylsp_offset;
if( _temp6459 < _temp6457.base? 1: _temp6459 >= _temp6457.last_plus_one){ _throw(
Null_Exception);}(* _temp6459).last_column=({ struct _tagged_ptr3 _temp6460=
yyls; struct Cyc_Yyltype* _temp6462= _temp6460.curr +(( yylsp_offset + yylen) -
1); if( _temp6462 < _temp6460.base? 1: _temp6462 >= _temp6460.last_plus_one){
_throw( Null_Exception);}* _temp6462;}).last_column;});} yyn=( int)({ short*
_temp6463=( short*) Cyc_yyr1; unsigned int _temp6464= yyn; if( _temp6464 >= 378u){
_throw( Null_Exception);} _temp6463[ _temp6464];}); yystate=( int)({ short*
_temp6465=( short*) Cyc_yypgoto; unsigned int _temp6466= yyn - 119; if(
_temp6466 >= 102u){ _throw( Null_Exception);} _temp6465[ _temp6466];}) +( int)({
struct _tagged_ptr1 _temp6467= yyss; short* _temp6469= _temp6467.curr +
yyssp_offset; if( _temp6469 < _temp6467.base? 1: _temp6469 >= _temp6467.last_plus_one){
_throw( Null_Exception);}* _temp6469;}); if(( yystate >= 0? yystate <= 4226: 0)?(
int)({ short* _temp6470=( short*) Cyc_yycheck; unsigned int _temp6471= yystate;
if( _temp6471 >= 4227u){ _throw( Null_Exception);} _temp6470[ _temp6471];}) ==(
int)({ struct _tagged_ptr1 _temp6472= yyss; short* _temp6474= _temp6472.curr +
yyssp_offset; if( _temp6474 < _temp6472.base? 1: _temp6474 >= _temp6472.last_plus_one){
_throw( Null_Exception);}* _temp6474;}): 0){ yystate=( int)({ short* _temp6475=(
short*) Cyc_yytable; unsigned int _temp6476= yystate; if( _temp6476 >= 4227u){
_throw( Null_Exception);} _temp6475[ _temp6476];});} else{ yystate=( int)({
short* _temp6477=( short*) Cyc_yydefgoto; unsigned int _temp6478= yyn - 119; if(
_temp6478 >= 102u){ _throw( Null_Exception);} _temp6477[ _temp6478];});} goto
yynewstate; yyerrlab: if( yyerrstatus == 0){ ++ Cyc_yynerrs; yyn=( int)({ short*
_temp6479=( short*) Cyc_yypact; unsigned int _temp6480= yystate; if( _temp6480
>= 711u){ _throw( Null_Exception);} _temp6479[ _temp6480];}); if( yyn > - 32768?
yyn < 4226: 0){ int sze= 0; struct _tagged_string msg; int x; int count; count=
0; for( x= yyn < 0? - yyn: 0;( unsigned int) x < 221u / sizeof( char*); x ++){
if(( int)({ short* _temp6481=( short*) Cyc_yycheck; unsigned int _temp6482= x +
yyn; if( _temp6482 >= 4227u){ _throw( Null_Exception);} _temp6481[ _temp6482];})
== x){( unsigned int)( sze += Cyc_String_strlen(({ struct _tagged_string*
_temp6483=( struct _tagged_string*) Cyc_yytname; unsigned int _temp6484= x; if(
_temp6484 >= 221u){ _throw( Null_Exception);} _temp6483[ _temp6484];})) +(
unsigned int) 15), count ++;}} msg=({ unsigned int _temp6485=( unsigned int)(
sze + 15); char* _temp6486=( char*) GC_malloc_atomic( sizeof( char) * _temp6485);
unsigned int i; struct _tagged_string _temp6487={ _temp6486, _temp6486,
_temp6486 + _temp6485}; for( i= 0; i < _temp6485; i ++){ _temp6486[ i]='\000';}
_temp6487;}); Cyc_String_strcpy( msg,( struct _tagged_string)({ char* _temp6488=(
char*)"parse error"; struct _tagged_string _temp6489; _temp6489.curr= _temp6488;
_temp6489.base= _temp6488; _temp6489.last_plus_one= _temp6488 + 12; _temp6489;}));
if( count < 5){ count= 0; for( x= yyn < 0? - yyn: 0;( unsigned int) x < 221u /
sizeof( char*); x ++){ if(( int)({ short* _temp6490=( short*) Cyc_yycheck;
unsigned int _temp6491= x + yyn; if( _temp6491 >= 4227u){ _throw( Null_Exception);}
_temp6490[ _temp6491];}) == x){ Cyc_String_strcat( msg, count == 0?( struct
_tagged_string)({ char* _temp6492=( char*)", expecting `"; struct _tagged_string
_temp6493; _temp6493.curr= _temp6492; _temp6493.base= _temp6492; _temp6493.last_plus_one=
_temp6492 + 14; _temp6493;}):( struct _tagged_string)({ char* _temp6494=( char*)" or `";
struct _tagged_string _temp6495; _temp6495.curr= _temp6494; _temp6495.base=
_temp6494; _temp6495.last_plus_one= _temp6494 + 6; _temp6495;})); Cyc_String_strcat(
msg,({ struct _tagged_string* _temp6496=( struct _tagged_string*) Cyc_yytname;
unsigned int _temp6497= x; if( _temp6497 >= 221u){ _throw( Null_Exception);}
_temp6496[ _temp6497];})); Cyc_String_strcat( msg,( struct _tagged_string)({
char* _temp6498=( char*)"'"; struct _tagged_string _temp6499; _temp6499.curr=
_temp6498; _temp6499.base= _temp6498; _temp6499.last_plus_one= _temp6498 + 2;
_temp6499;})); count ++;}}} Cyc_yyerror( msg);} else{ Cyc_yyerror(( struct
_tagged_string)({ char* _temp6500=( char*)"parse error"; struct _tagged_string
_temp6501; _temp6501.curr= _temp6500; _temp6501.base= _temp6500; _temp6501.last_plus_one=
_temp6500 + 12; _temp6501;}));}} goto yyerrlab1; yyerrlab1: if( yyerrstatus == 3){
if( Cyc_yychar == 0){ return 1;} Cyc_yychar= - 2;} yyerrstatus= 3; goto
yyerrhandle; yyerrdefault: yyerrpop: if( yyssp_offset == 0){ return 1;}
yyvsp_offset --; yystate=( int)({ struct _tagged_ptr1 _temp6502= yyss; short*
_temp6504= _temp6502.curr +( -- yyssp_offset); if( _temp6504 < _temp6502.base? 1:
_temp6504 >= _temp6502.last_plus_one){ _throw( Null_Exception);}* _temp6504;});
yylsp_offset --; yyerrhandle: yyn=( int)({ short* _temp6505=( short*) Cyc_yypact;
unsigned int _temp6506= yystate; if( _temp6506 >= 711u){ _throw( Null_Exception);}
_temp6505[ _temp6506];}); if( yyn == - 32768){ goto yyerrdefault;} yyn += 1; if((
yyn < 0? 1: yyn > 4226)? 1:( int)({ short* _temp6507=( short*) Cyc_yycheck;
unsigned int _temp6508= yyn; if( _temp6508 >= 4227u){ _throw( Null_Exception);}
_temp6507[ _temp6508];}) != 1){ goto yyerrdefault;} yyn=( int)({ short*
_temp6509=( short*) Cyc_yytable; unsigned int _temp6510= yyn; if( _temp6510 >=
4227u){ _throw( Null_Exception);} _temp6509[ _temp6510];}); if( yyn < 0){ if(
yyn == - 32768){ goto yyerrpop;} yyn= - yyn; goto yyreduce;} else{ if( yyn == 0){
goto yyerrpop;}} if( yyn == 710){ return 0;}({ struct _tagged_ptr2 _temp6511=
yyvs; struct _xenum_struct** _temp6513= _temp6511.curr +( ++ yyvsp_offset); if(
_temp6513 < _temp6511.base? 1: _temp6513 >= _temp6511.last_plus_one){ _throw(
Null_Exception);}* _temp6513= Cyc_yylval;});({ struct _tagged_ptr3 _temp6514=
yyls; struct Cyc_Yyltype* _temp6516= _temp6514.curr +( ++ yylsp_offset); if(
_temp6516 < _temp6514.base? 1: _temp6516 >= _temp6514.last_plus_one){ _throw(
Null_Exception);}* _temp6516= Cyc_yylloc;}); goto yynewstate;} void Cyc_yyprint(
int i, struct _xenum_struct* v){ struct _xenum_struct* _temp6517= v; struct
_tuple12* _temp6537; struct _tuple12 _temp6539; int _temp6540; void* _temp6542;
char _temp6544; short _temp6546; struct _tagged_string _temp6548; struct Cyc_Core_Opt*
_temp6550; struct Cyc_Core_Opt* _temp6552; struct Cyc_Core_Opt _temp6554; struct
_tagged_string* _temp6555; struct _tuple1* _temp6557; struct _tuple1 _temp6559;
struct _tagged_string* _temp6560; void* _temp6562; _LL6519: if( _temp6517->tag
== Cyc_Okay_tok_tag){ goto _LL6520;} else{ goto _LL6521;} _LL6521: if((*((
struct _xenum_struct*) _temp6517)).tag == Cyc_Int_tok_tag){ _LL6538: _temp6537=((
struct Cyc_Int_tok_struct*) _temp6517)->f1; _temp6539=* _temp6537; _LL6543:
_temp6542= _temp6539.f1; goto _LL6541; _LL6541: _temp6540= _temp6539.f2; goto
_LL6522;} else{ goto _LL6523;} _LL6523: if((*(( struct _xenum_struct*) _temp6517)).tag
== Cyc_Char_tok_tag){ _LL6545: _temp6544=(( struct Cyc_Char_tok_struct*)
_temp6517)->f1; goto _LL6524;} else{ goto _LL6525;} _LL6525: if((*(( struct
_xenum_struct*) _temp6517)).tag == Cyc_Short_tok_tag){ _LL6547: _temp6546=((
struct Cyc_Short_tok_struct*) _temp6517)->f1; goto _LL6526;} else{ goto _LL6527;}
_LL6527: if((*(( struct _xenum_struct*) _temp6517)).tag == Cyc_String_tok_tag){
_LL6549: _temp6548=(( struct Cyc_String_tok_struct*) _temp6517)->f1; goto
_LL6528;} else{ goto _LL6529;} _LL6529: if((*(( struct _xenum_struct*) _temp6517)).tag
== Cyc_Stringopt_tok_tag){ _LL6551: _temp6550=(( struct Cyc_Stringopt_tok_struct*)
_temp6517)->f1; if( _temp6550 == 0){ goto _LL6530;} else{ goto _LL6531;}} else{
goto _LL6531;} _LL6531: if((*(( struct _xenum_struct*) _temp6517)).tag == Cyc_Stringopt_tok_tag){
_LL6553: _temp6552=(( struct Cyc_Stringopt_tok_struct*) _temp6517)->f1; if(
_temp6552 == 0){ goto _LL6533;} else{ _temp6554=* _temp6552; _LL6556: _temp6555=(
struct _tagged_string*) _temp6554.v; goto _LL6532;}} else{ goto _LL6533;}
_LL6533: if((*(( struct _xenum_struct*) _temp6517)).tag == Cyc_QualId_tok_tag){
_LL6558: _temp6557=(( struct Cyc_QualId_tok_struct*) _temp6517)->f1; _temp6559=*
_temp6557; _LL6563: _temp6562= _temp6559.f1; goto _LL6561; _LL6561: _temp6560=
_temp6559.f2; goto _LL6534;} else{ goto _LL6535;} _LL6535: goto _LL6536; _LL6520:
fprintf( Cyc_Stdio_stderr,"ok"); goto _LL6518; _LL6522: fprintf( Cyc_Stdio_stderr,"%d",
_temp6540); goto _LL6518; _LL6524: fprintf( Cyc_Stdio_stderr,"%c",( int)
_temp6544); goto _LL6518; _LL6526: fprintf( Cyc_Stdio_stderr,"%ds",( int)
_temp6546); goto _LL6518; _LL6528:({ struct _tagged_string _temp6564= _temp6548;
fprintf( Cyc_Stdio_stderr,"\"%.*s\"", _temp6564.last_plus_one - _temp6564.curr,
_temp6564.curr);}); goto _LL6518; _LL6530: fprintf( Cyc_Stdio_stderr,"null");
goto _LL6518; _LL6532:({ struct _tagged_string _temp6565=* _temp6555; fprintf(
Cyc_Stdio_stderr,"\"%.*s\"", _temp6565.last_plus_one - _temp6565.curr, _temp6565.curr);});
goto _LL6518; _LL6534: { struct Cyc_List_List* prefix= 0;{ void* _temp6566=
_temp6562; struct Cyc_List_List* _temp6574; struct Cyc_List_List* _temp6576;
_LL6568: if(( unsigned int) _temp6566 > 1u?(( struct _enum_struct*) _temp6566)->tag
== Cyc_Absyn_Rel_n_tag: 0){ _LL6575: _temp6574=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Rel_n_struct*) _temp6566)->f1; goto _LL6569;} else{ goto _LL6570;}
_LL6570: if(( unsigned int) _temp6566 > 1u?(( struct _enum_struct*) _temp6566)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL6577: _temp6576=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp6566)->f1; goto _LL6571;} else{ goto _LL6572;}
_LL6572: if( _temp6566 == Cyc_Absyn_Loc_n){ goto _LL6573;} else{ goto _LL6567;}
_LL6569: prefix= _temp6574; goto _LL6567; _LL6571: prefix= _temp6576; goto
_LL6567; _LL6573: goto _LL6567; _LL6567:;} for( 0; prefix != 0; prefix= prefix->tl){({
struct _tagged_string _temp6578=*(( struct _tagged_string*) prefix->hd); fprintf(
Cyc_Stdio_stderr,"%.*s::", _temp6578.last_plus_one - _temp6578.curr, _temp6578.curr);});}({
struct _tagged_string _temp6579=* _temp6560; fprintf( Cyc_Stdio_stderr,"%.*s::",
_temp6579.last_plus_one - _temp6579.curr, _temp6579.curr);}); goto _LL6518;}
_LL6536: fprintf( Cyc_Stdio_stderr,"?"); goto _LL6518; _LL6518:;} struct Cyc_List_List*
Cyc_Parse_parse_file( struct Cyc_Stdio___sFILE* f){ Cyc_Parse_parse_result= 0;
Cyc_Parse_lbuf=({ struct Cyc_Core_Opt* _temp6580=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp6580->v=( void*) Cyc_Lexing_from_file(
f); _temp6580;}); Cyc_yyparse(); return Cyc_Parse_parse_result;}