#include "cyc_include.h"

 struct _tagged_ptr0{ int* curr; int* base; int* last_plus_one;}; struct _tuple0{
struct Cyc_List_List* f1; struct Cyc_List_List* f2;}; struct _tuple1{ void* f1;
struct _tagged_string* f2;}; struct _tuple2{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual*
f2; void* f3;}; struct _tuple3{ struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt*
f2;}; struct _tuple4{ struct _tagged_string* f1; struct Cyc_Absyn_Tqual* f2;
void* f3;}; struct _tuple5{ void* f1; struct Cyc_Core_Opt* f2;}; struct _tuple6{
struct Cyc_Absyn_Tqual* f1; void* f2; struct Cyc_List_List* f3;}; struct _tuple7{
struct _tuple1* f1; struct Cyc_Absyn_Tqual* f2; void* f3; struct Cyc_List_List*
f4;}; struct _tuple8{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual* f2; void*
f3; struct Cyc_List_List* f4;}; struct _tuple9{ struct Cyc_Absyn_Tqual* f1; void*
f2;}; struct _tuple10{ struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2;};
struct _tuple11{ void* f1; int f2;}; struct _tuple12{ struct Cyc_Parse_Declarator*
f1; struct Cyc_Absyn_Exp* f2;}; struct _tuple13{ struct Cyc_Absyn_Tqual* f1;
struct Cyc_List_List* f2;}; struct _tuple14{ struct Cyc_List_List* f1; int f2;
struct Cyc_Core_Opt* f3;}; struct _tuple15{ struct Cyc_List_List* f1; struct Cyc_Absyn_Pat*
f2;}; struct _tagged_ptr1{ short* curr; short* base; short* last_plus_one;};
struct _tagged_ptr2{ struct _xenum_struct** curr; struct _xenum_struct** base;
struct _xenum_struct** last_plus_one;}; struct _tagged_ptr3{ struct Cyc_Yyltype*
curr; struct Cyc_Yyltype* base; struct Cyc_Yyltype* last_plus_one;}; struct
_tuple16{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2;}; typedef
unsigned int Cyc_uint; typedef char* Cyc_Cstring; typedef struct _tagged_string
Cyc_string; typedef struct _tagged_string Cyc_string_t; typedef struct
_tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void* exit( int);
extern void* abort(); struct Cyc_Core_Opt{ void* v;}; typedef struct Cyc_Core_Opt*
Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg_tag[ 11u]; struct Cyc_Core_InvalidArg_struct{
char* tag; struct _tagged_string f1;}; extern char Cyc_Core_Failure_tag[ 8u];
struct Cyc_Core_Failure_struct{ char* tag; struct _tagged_string f1;}; extern
char Cyc_Core_Impossible_tag[ 11u]; struct Cyc_Core_Impossible_struct{ char* tag;
struct _tagged_string f1;}; extern char Cyc_Core_Not_found_tag[ 10u]; struct Cyc_Core_Not_found_struct{
char* tag;}; extern char Cyc_Core_Unreachable_tag[ 12u]; struct Cyc_Core_Unreachable_struct{
char* tag; struct _tagged_string f1;}; extern char* string_to_Cstring( struct
_tagged_string); extern char* underlying_Cstring( struct _tagged_string); extern
struct _tagged_string Cstring_to_string( char*); extern int system( char*);
struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
extern struct Cyc_Stdio___sFILE* Cyc_Stdio_stderr; typedef unsigned int Cyc_Stdio_size_t;
typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileOpenError_tag[ 14u];
struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1;};
extern char Cyc_Stdio_FileCloseError_tag[ 15u]; struct Cyc_Stdio_FileCloseError_struct{
char* tag;}; extern char Cyc_Lexing_Error_tag[ 6u]; struct Cyc_Lexing_Error_struct{
char* tag; struct _tagged_string f1;}; struct Cyc_Lexing_lexbuf{ void(*
refill_buff)( struct Cyc_Lexing_lexbuf*); void* refill_state; struct
_tagged_string lex_buffer; int lex_buffer_len; int lex_abs_pos; int
lex_start_pos; int lex_curr_pos; int lex_last_pos; int lex_last_action; int
lex_eof_reached;}; typedef struct Cyc_Lexing_lexbuf* Cyc_Lexing_Lexbuf; struct
Cyc_Lexing_function_lexbuf_state{ int(* read_fun)( struct _tagged_string, int,
void*); void* read_fun_state;}; typedef struct Cyc_Lexing_function_lexbuf_state*
Cyc_Lexing_Function_lexbuf_state; struct Cyc_Lexing_lex_tables{ struct
_tagged_ptr0 lex_base; struct _tagged_ptr0 lex_backtrk; struct _tagged_ptr0
lex_default; struct _tagged_ptr0 lex_trans; struct _tagged_ptr0 lex_check;};
typedef struct Cyc_Lexing_lex_tables* Cyc_Lexing_LexTables; extern struct Cyc_Lexing_lexbuf*
Cyc_Lexing_from_file( struct Cyc_Stdio___sFILE*); struct Cyc_List_List{ void* hd;
struct Cyc_List_List* tl;}; typedef struct Cyc_List_List* Cyc_List_glist_t;
typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t;
extern char Cyc_List_List_empty_tag[ 11u]; struct Cyc_List_List_empty_struct{
char* tag;}; extern struct Cyc_List_List* Cyc_List_map( void*(* f)( void*),
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_map_c( void*(* f)(
void*, void*), void* env, struct Cyc_List_List* x); extern char Cyc_List_List_mismatch_tag[
14u]; struct Cyc_List_List_mismatch_struct{ char* tag;}; extern void Cyc_List_iter(
void(* f)( void*), struct Cyc_List_List* x); extern void Cyc_List_iter_c( void(*
f)( void*, void*), void* env, struct Cyc_List_List* x); extern void* Cyc_List_fold_right(
void*(* f)( void*, void*), struct Cyc_List_List* x, void* accum); extern struct
Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List* x); extern struct Cyc_List_List*
Cyc_List_imp_append( struct Cyc_List_List* x, struct Cyc_List_List* y); extern
struct Cyc_List_List* Cyc_List_flatten( struct Cyc_List_List* x); extern char
Cyc_List_Nth_tag[ 4u]; struct Cyc_List_Nth_struct{ char* tag;}; extern struct
_tuple0 Cyc_List_split( struct Cyc_List_List* x); extern unsigned int Cyc_String_strlen(
struct _tagged_string s); extern int Cyc_String_strcmp( struct _tagged_string s1,
struct _tagged_string s2); extern int Cyc_String_zstrcmp( struct _tagged_string,
struct _tagged_string); extern int Cyc_String_zstrptrcmp( struct _tagged_string*,
struct _tagged_string*); extern struct _tagged_string Cyc_String_strcat( struct
_tagged_string dest, struct _tagged_string src); extern struct _tagged_string
Cyc_String_strcpy( struct _tagged_string dest, struct _tagged_string src);
struct Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct
Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern
char Cyc_Set_Absent_tag[ 7u]; struct Cyc_Set_Absent_struct{ char* tag;}; struct
Cyc_Lineno_Pos{ struct _tagged_string logical_file; struct _tagged_string line;
int line_no; int col;}; typedef struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t; extern
char Cyc_Position_Exit_tag[ 5u]; struct Cyc_Position_Exit_struct{ char* tag;};
struct Cyc_Position_Segment; typedef struct Cyc_Position_Segment* Cyc_Position_seg_t;
extern struct Cyc_Position_Segment* Cyc_Position_segment_of_abs( int, int);
extern struct Cyc_Position_Segment* Cyc_Position_segment_join( struct Cyc_Position_Segment*,
struct Cyc_Position_Segment*); extern struct _tagged_string Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*); typedef void* Cyc_Position_Error_kind; extern
void* Cyc_Position_Lex; extern void* Cyc_Position_Parse; extern void* Cyc_Position_Elab;
typedef void* Cyc_Position_error_kind_t; struct Cyc_Position_Error{ struct
_tagged_string source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_string desc;}; typedef struct Cyc_Position_Error* Cyc_Position_error_t;
extern struct Cyc_Position_Error* Cyc_Position_mk_err_parse( struct Cyc_Position_Segment*,
struct _tagged_string); extern char Cyc_Position_Nocontext_tag[ 10u]; struct Cyc_Position_Nocontext_struct{
char* tag;}; extern void Cyc_Position_post_error( struct Cyc_Position_Error*);
typedef struct _tagged_string* Cyc_Absyn_field_name; typedef struct
_tagged_string* Cyc_Absyn_var; typedef struct _tagged_string* Cyc_Absyn_tvarname_t;
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
struct Cyc_Absyn_Fndecl; struct Cyc_Absyn_Structdecl; struct Cyc_Absyn_Enumfield;
struct Cyc_Absyn_Enumdecl; struct Cyc_Absyn_Xenumdecl; struct Cyc_Absyn_Typedefdecl;
struct Cyc_Absyn_Vardecl; typedef void* Cyc_Absyn_Raw_decl; struct Cyc_Absyn_Decl;
typedef void* Cyc_Absyn_Designator; typedef struct _xenum_struct* Cyc_Absyn_StmtAnnot;
typedef void* Cyc_Absyn_scope; typedef struct Cyc_Absyn_Tqual* Cyc_Absyn_tqual;
typedef void* Cyc_Absyn_size_of_t; typedef void* Cyc_Absyn_kind_t; typedef
struct Cyc_Absyn_Tvar* Cyc_Absyn_tvar; typedef void* Cyc_Absyn_sign_t; typedef
struct Cyc_Absyn_Conref* Cyc_Absyn_conref; typedef void* Cyc_Absyn_constraint_t;
typedef void* Cyc_Absyn_bounds_t; typedef struct Cyc_Absyn_PtrInfo Cyc_Absyn_ptr_info_t;
typedef struct Cyc_Absyn_FnInfo Cyc_Absyn_fn_info_t; typedef void* Cyc_Absyn_typ;
typedef void* Cyc_Absyn_funcparams_t; typedef void* Cyc_Absyn_type_modifier;
typedef void* Cyc_Absyn_cnst_t; typedef void* Cyc_Absyn_primop; typedef void*
Cyc_Absyn_incrementor_t; typedef void* Cyc_Absyn_raw_exp_t; typedef struct Cyc_Absyn_Exp*
Cyc_Absyn_exp; typedef struct Cyc_Absyn_Exp* Cyc_Absyn_exp_opt; typedef void*
Cyc_Absyn_raw_stmt_t; typedef struct Cyc_Absyn_Stmt* Cyc_Absyn_stmt; typedef
struct Cyc_Absyn_Stmt* Cyc_Absyn_stmt_opt; typedef void* Cyc_Absyn_raw_pat_t;
typedef struct Cyc_Absyn_Pat* Cyc_Absyn_pat; typedef void* Cyc_Absyn_binding_t;
typedef struct Cyc_Absyn_Switch_clause* Cyc_Absyn_switch_clause; typedef struct
Cyc_Absyn_Fndecl* Cyc_Absyn_fndecl; typedef struct Cyc_Absyn_Structdecl* Cyc_Absyn_structdecl;
typedef struct Cyc_Absyn_Enumfield* Cyc_Absyn_enumfield; typedef struct Cyc_Absyn_Enumdecl*
Cyc_Absyn_enumdecl; typedef struct Cyc_Absyn_Xenumdecl* Cyc_Absyn_xenumdecl;
typedef struct Cyc_Absyn_Typedefdecl* Cyc_Absyn_typedefdecl; typedef struct Cyc_Absyn_Vardecl*
Cyc_Absyn_vardecl; typedef void* Cyc_Absyn_raw_decl; typedef struct Cyc_Absyn_Decl*
Cyc_Absyn_decl; typedef void* Cyc_Absyn_designator; typedef struct _xenum_struct*
Cyc_Absyn_stmt_annot_t; extern void* Cyc_Absyn_Loc_n; extern const int Cyc_Absyn_Rel_n_tag;
struct Cyc_Absyn_Rel_n_struct{ int tag; struct Cyc_List_List* f1;}; extern const
int Cyc_Absyn_Abs_n_tag; struct Cyc_Absyn_Abs_n_struct{ int tag; struct Cyc_List_List*
f1;}; extern void* Cyc_Absyn_Static; extern void* Cyc_Absyn_Abstract; extern
void* Cyc_Absyn_Public; extern void* Cyc_Absyn_Extern; extern void* Cyc_Absyn_ExternC;
struct Cyc_Absyn_Tqual{ int q_const; int q_volatile; int q_restrict;}; extern
void* Cyc_Absyn_B1; extern void* Cyc_Absyn_B2; extern void* Cyc_Absyn_B4; extern
void* Cyc_Absyn_B8; extern void* Cyc_Absyn_AnyKind; extern void* Cyc_Absyn_MemKind;
extern void* Cyc_Absyn_BoxKind; extern void* Cyc_Absyn_RgnKind; extern void* Cyc_Absyn_EffKind;
extern void* Cyc_Absyn_Signed; extern void* Cyc_Absyn_Unsigned; struct Cyc_Absyn_Conref{
void* v;}; extern const int Cyc_Absyn_Eq_constr_tag; struct Cyc_Absyn_Eq_constr_struct{
int tag; void* f1;}; extern const int Cyc_Absyn_Forward_constr_tag; struct Cyc_Absyn_Forward_constr_struct{
int tag; struct Cyc_Absyn_Conref* f1;}; extern void* Cyc_Absyn_No_constr; struct
Cyc_Absyn_Tvar{ struct _tagged_string* name; struct Cyc_Absyn_Conref* kind;};
extern void* Cyc_Absyn_Unknown_b; extern const int Cyc_Absyn_Upper_b_tag; struct
Cyc_Absyn_Upper_b_struct{ int tag; struct Cyc_Absyn_Exp* f1;}; struct Cyc_Absyn_PtrInfo{
void* elt_typ; void* rgn_typ; struct Cyc_Absyn_Conref* nullable; struct Cyc_Absyn_Tqual*
tq; struct Cyc_Absyn_Conref* bounds;}; struct Cyc_Absyn_FnInfo{ struct Cyc_List_List*
tvars; struct Cyc_Core_Opt* effect; void* ret_typ; struct Cyc_List_List* args;
int varargs;}; extern void* Cyc_Absyn_VoidType; extern const int Cyc_Absyn_Evar_tag;
struct Cyc_Absyn_Evar_struct{ int tag; void* f1; struct Cyc_Core_Opt* f2; int f3;};
extern const int Cyc_Absyn_VarType_tag; struct Cyc_Absyn_VarType_struct{ int tag;
struct Cyc_Absyn_Tvar* f1;}; extern const int Cyc_Absyn_EnumType_tag; struct Cyc_Absyn_EnumType_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Enumdecl**
f3;}; extern const int Cyc_Absyn_XenumType_tag; struct Cyc_Absyn_XenumType_struct{
int tag; struct _tuple1* f1; struct Cyc_Absyn_Xenumdecl** f2;}; extern const int
Cyc_Absyn_PointerType_tag; struct Cyc_Absyn_PointerType_struct{ int tag; struct
Cyc_Absyn_PtrInfo f1;}; extern const int Cyc_Absyn_IntType_tag; struct Cyc_Absyn_IntType_struct{
int tag; void* f1; void* f2;}; extern void* Cyc_Absyn_FloatType; extern void*
Cyc_Absyn_DoubleType; extern const int Cyc_Absyn_ArrayType_tag; struct Cyc_Absyn_ArrayType_struct{
int tag; void* f1; struct Cyc_Absyn_Tqual* f2; struct Cyc_Absyn_Exp* f3;};
extern const int Cyc_Absyn_FnType_tag; struct Cyc_Absyn_FnType_struct{ int tag;
struct Cyc_Absyn_FnInfo f1;}; extern const int Cyc_Absyn_TupleType_tag; struct
Cyc_Absyn_TupleType_struct{ int tag; struct Cyc_List_List* f1;}; extern const
int Cyc_Absyn_StructType_tag; struct Cyc_Absyn_StructType_struct{ int tag;
struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Structdecl** f3;};
extern void* Cyc_Absyn_UnionType; extern const int Cyc_Absyn_RgnHandleType_tag;
struct Cyc_Absyn_RgnHandleType_struct{ int tag; void* f1;}; extern const int Cyc_Absyn_TypedefType_tag;
struct Cyc_Absyn_TypedefType_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; struct Cyc_Core_Opt* f3;}; extern void* Cyc_Absyn_HeapRgn; extern const int
Cyc_Absyn_AccessEff_tag; struct Cyc_Absyn_AccessEff_struct{ int tag; void* f1;};
extern const int Cyc_Absyn_JoinEff_tag; struct Cyc_Absyn_JoinEff_struct{ int tag;
struct Cyc_List_List* f1;}; extern const int Cyc_Absyn_NoTypes_tag; struct Cyc_Absyn_NoTypes_struct{
int tag; struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2;}; extern
const int Cyc_Absyn_WithTypes_tag; struct Cyc_Absyn_WithTypes_struct{ int tag;
struct Cyc_List_List* f1; int f2; struct Cyc_Core_Opt* f3;}; typedef void* Cyc_Absyn_Pointer_Sort;
extern const int Cyc_Absyn_NonNullable_ps_tag; struct Cyc_Absyn_NonNullable_ps_struct{
int tag; struct Cyc_Absyn_Exp* f1;}; extern const int Cyc_Absyn_Nullable_ps_tag;
struct Cyc_Absyn_Nullable_ps_struct{ int tag; struct Cyc_Absyn_Exp* f1;}; extern
void* Cyc_Absyn_TaggedArray_ps; extern void* Cyc_Absyn_Carray_mod; extern const
int Cyc_Absyn_ConstArray_mod_tag; struct Cyc_Absyn_ConstArray_mod_struct{ int
tag; struct Cyc_Absyn_Exp* f1;}; extern const int Cyc_Absyn_Pointer_mod_tag;
struct Cyc_Absyn_Pointer_mod_struct{ int tag; void* f1; void* f2; struct Cyc_Absyn_Tqual*
f3;}; extern const int Cyc_Absyn_Function_mod_tag; struct Cyc_Absyn_Function_mod_struct{
int tag; void* f1;}; extern const int Cyc_Absyn_TypeParams_mod_tag; struct Cyc_Absyn_TypeParams_mod_struct{
int tag; struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; int f3;};
extern const int Cyc_Absyn_Char_c_tag; struct Cyc_Absyn_Char_c_struct{ int tag;
void* f1; char f2;}; extern const int Cyc_Absyn_Short_c_tag; struct Cyc_Absyn_Short_c_struct{
int tag; void* f1; short f2;}; extern const int Cyc_Absyn_Int_c_tag; struct Cyc_Absyn_Int_c_struct{
int tag; void* f1; int f2;}; extern const int Cyc_Absyn_LongLong_c_tag; struct
Cyc_Absyn_LongLong_c_struct{ int tag; void* f1; long long f2;}; extern const int
Cyc_Absyn_Float_c_tag; struct Cyc_Absyn_Float_c_struct{ int tag; struct
_tagged_string f1;}; extern const int Cyc_Absyn_String_c_tag; struct Cyc_Absyn_String_c_struct{
int tag; int f1; struct _tagged_string f2;}; extern void* Cyc_Absyn_Null_c;
extern void* Cyc_Absyn_Plus; extern void* Cyc_Absyn_Times; extern void* Cyc_Absyn_Minus;
extern void* Cyc_Absyn_Div; extern void* Cyc_Absyn_Mod; extern void* Cyc_Absyn_Eq;
extern void* Cyc_Absyn_Neq; extern void* Cyc_Absyn_Gt; extern void* Cyc_Absyn_Lt;
extern void* Cyc_Absyn_Gte; extern void* Cyc_Absyn_Lte; extern void* Cyc_Absyn_Not;
extern void* Cyc_Absyn_Bitnot; extern void* Cyc_Absyn_Bitand; extern void* Cyc_Absyn_Bitor;
extern void* Cyc_Absyn_Bitxor; extern void* Cyc_Absyn_Bitlshift; extern void*
Cyc_Absyn_Bitlrshift; extern void* Cyc_Absyn_Bitarshift; extern void* Cyc_Absyn_Size;
extern void* Cyc_Absyn_Printf; extern void* Cyc_Absyn_Fprintf; extern void* Cyc_Absyn_Xprintf;
extern void* Cyc_Absyn_Scanf; extern void* Cyc_Absyn_Fscanf; extern void* Cyc_Absyn_Sscanf;
extern void* Cyc_Absyn_PreInc; extern void* Cyc_Absyn_PostInc; extern void* Cyc_Absyn_PreDec;
extern void* Cyc_Absyn_PostDec; extern const int Cyc_Absyn_Const_e_tag; struct
Cyc_Absyn_Const_e_struct{ int tag; void* f1;}; extern const int Cyc_Absyn_Var_e_tag;
struct Cyc_Absyn_Var_e_struct{ int tag; struct _tuple1* f1; void* f2;}; extern
const int Cyc_Absyn_UnknownId_e_tag; struct Cyc_Absyn_UnknownId_e_struct{ int
tag; struct _tuple1* f1;}; extern const int Cyc_Absyn_Primop_e_tag; struct Cyc_Absyn_Primop_e_struct{
int tag; void* f1; struct Cyc_List_List* f2;}; extern const int Cyc_Absyn_AssignOp_e_tag;
struct Cyc_Absyn_AssignOp_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
Cyc_Core_Opt* f2; struct Cyc_Absyn_Exp* f3;}; extern const int Cyc_Absyn_Increment_e_tag;
struct Cyc_Absyn_Increment_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; void* f2;};
extern const int Cyc_Absyn_Conditional_e_tag; struct Cyc_Absyn_Conditional_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp*
f3;}; extern const int Cyc_Absyn_SeqExp_e_tag; struct Cyc_Absyn_SeqExp_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2;}; extern const int
Cyc_Absyn_UnknownCall_e_tag; struct Cyc_Absyn_UnknownCall_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2;}; extern const int Cyc_Absyn_FnCall_e_tag;
struct Cyc_Absyn_FnCall_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List*
f2;}; extern const int Cyc_Absyn_Throw_e_tag; struct Cyc_Absyn_Throw_e_struct{
int tag; struct Cyc_Absyn_Exp* f1;}; extern const int Cyc_Absyn_NoInstantiate_e_tag;
struct Cyc_Absyn_NoInstantiate_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;};
extern const int Cyc_Absyn_Instantiate_e_tag; struct Cyc_Absyn_Instantiate_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2;}; extern const int
Cyc_Absyn_Cast_e_tag; struct Cyc_Absyn_Cast_e_struct{ int tag; void* f1; struct
Cyc_Absyn_Exp* f2;}; extern const int Cyc_Absyn_Address_e_tag; struct Cyc_Absyn_Address_e_struct{
int tag; struct Cyc_Absyn_Exp* f1;}; extern const int Cyc_Absyn_Sizeof_e_tag;
struct Cyc_Absyn_Sizeof_e_struct{ int tag; void* f1;}; extern const int Cyc_Absyn_Deref_e_tag;
struct Cyc_Absyn_Deref_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;}; extern
const int Cyc_Absyn_StructMember_e_tag; struct Cyc_Absyn_StructMember_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct _tagged_string* f2;}; extern const int
Cyc_Absyn_StructArrow_e_tag; struct Cyc_Absyn_StructArrow_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; struct _tagged_string* f2;}; extern const int Cyc_Absyn_Subscript_e_tag;
struct Cyc_Absyn_Subscript_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
Cyc_Absyn_Exp* f2;}; extern const int Cyc_Absyn_Tuple_e_tag; struct Cyc_Absyn_Tuple_e_struct{
int tag; struct Cyc_List_List* f1;}; extern const int Cyc_Absyn_CompoundLit_e_tag;
struct Cyc_Absyn_CompoundLit_e_struct{ int tag; struct _tuple2* f1; struct Cyc_List_List*
f2;}; extern const int Cyc_Absyn_Array_e_tag; struct Cyc_Absyn_Array_e_struct{
int tag; int f1; struct Cyc_List_List* f2;}; extern const int Cyc_Absyn_Comprehension_e_tag;
struct Cyc_Absyn_Comprehension_e_struct{ int tag; struct Cyc_Absyn_Vardecl* f1;
struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp* f3;}; extern const int Cyc_Absyn_Struct_e_tag;
struct Cyc_Absyn_Struct_e_struct{ int tag; struct _tuple1* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Structdecl* f4;}; extern const
int Cyc_Absyn_Enum_e_tag; struct Cyc_Absyn_Enum_e_struct{ int tag; struct Cyc_Core_Opt*
f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Enumdecl*
f4; struct Cyc_Absyn_Enumfield* f5;}; extern const int Cyc_Absyn_Xenum_e_tag;
struct Cyc_Absyn_Xenum_e_struct{ int tag; struct Cyc_Core_Opt* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Xenumdecl* f3; struct Cyc_Absyn_Enumfield* f4;}; extern
const int Cyc_Absyn_UnresolvedMem_e_tag; struct Cyc_Absyn_UnresolvedMem_e_struct{
int tag; struct Cyc_Core_Opt* f1; struct Cyc_List_List* f2;}; extern const int
Cyc_Absyn_StmtExp_e_tag; struct Cyc_Absyn_StmtExp_e_struct{ int tag; struct Cyc_Absyn_Stmt*
f1;}; extern const int Cyc_Absyn_Codegen_e_tag; struct Cyc_Absyn_Codegen_e_struct{
int tag; struct Cyc_Absyn_Fndecl* f1;}; extern const int Cyc_Absyn_Fill_e_tag;
struct Cyc_Absyn_Fill_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;}; struct Cyc_Absyn_Exp{
struct Cyc_Core_Opt* topt; void* r; struct Cyc_Position_Segment* loc;}; extern
void* Cyc_Absyn_Skip_s; extern const int Cyc_Absyn_Exp_s_tag; struct Cyc_Absyn_Exp_s_struct{
int tag; struct Cyc_Absyn_Exp* f1;}; extern const int Cyc_Absyn_Seq_s_tag;
struct Cyc_Absyn_Seq_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_Absyn_Stmt*
f2;}; extern const int Cyc_Absyn_Return_s_tag; struct Cyc_Absyn_Return_s_struct{
int tag; struct Cyc_Absyn_Exp* f1;}; extern const int Cyc_Absyn_IfThenElse_s_tag;
struct Cyc_Absyn_IfThenElse_s_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
Cyc_Absyn_Stmt* f2; struct Cyc_Absyn_Stmt* f3;}; extern const int Cyc_Absyn_While_s_tag;
struct Cyc_Absyn_While_s_struct{ int tag; struct _tuple3 f1; struct Cyc_Absyn_Stmt*
f2;}; extern const int Cyc_Absyn_Break_s_tag; struct Cyc_Absyn_Break_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1;}; extern const int Cyc_Absyn_Continue_s_tag;
struct Cyc_Absyn_Continue_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1;}; extern
const int Cyc_Absyn_Goto_s_tag; struct Cyc_Absyn_Goto_s_struct{ int tag; struct
_tagged_string* f1; struct Cyc_Absyn_Stmt* f2;}; extern const int Cyc_Absyn_For_s_tag;
struct Cyc_Absyn_For_s_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct _tuple3
f2; struct _tuple3 f3; struct Cyc_Absyn_Stmt* f4;}; extern const int Cyc_Absyn_Switch_s_tag;
struct Cyc_Absyn_Switch_s_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List*
f2;}; extern const int Cyc_Absyn_Fallthru_s_tag; struct Cyc_Absyn_Fallthru_s_struct{
int tag; struct Cyc_List_List* f1; struct Cyc_Absyn_Switch_clause** f2;}; extern
const int Cyc_Absyn_Decl_s_tag; struct Cyc_Absyn_Decl_s_struct{ int tag; struct
Cyc_Absyn_Decl* f1; struct Cyc_Absyn_Stmt* f2;}; extern const int Cyc_Absyn_Cut_s_tag;
struct Cyc_Absyn_Cut_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1;}; extern
const int Cyc_Absyn_Splice_s_tag; struct Cyc_Absyn_Splice_s_struct{ int tag;
struct Cyc_Absyn_Stmt* f1;}; extern const int Cyc_Absyn_Label_s_tag; struct Cyc_Absyn_Label_s_struct{
int tag; struct _tagged_string* f1; struct Cyc_Absyn_Stmt* f2;}; extern const
int Cyc_Absyn_Do_s_tag; struct Cyc_Absyn_Do_s_struct{ int tag; struct Cyc_Absyn_Stmt*
f1; struct _tuple3 f2;}; extern const int Cyc_Absyn_TryCatch_s_tag; struct Cyc_Absyn_TryCatch_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_List_List* f2;}; struct Cyc_Absyn_Stmt{
void* r; struct Cyc_Position_Segment* loc; struct Cyc_List_List* non_local_preds;
struct _xenum_struct* annot;}; extern void* Cyc_Absyn_Wild_p; extern const int
Cyc_Absyn_Var_p_tag; struct Cyc_Absyn_Var_p_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1;}; extern void* Cyc_Absyn_Null_p; extern const int Cyc_Absyn_Int_p_tag;
struct Cyc_Absyn_Int_p_struct{ int tag; void* f1; int f2;}; extern const int Cyc_Absyn_Char_p_tag;
struct Cyc_Absyn_Char_p_struct{ int tag; char f1;}; extern const int Cyc_Absyn_Float_p_tag;
struct Cyc_Absyn_Float_p_struct{ int tag; struct _tagged_string f1;}; extern
const int Cyc_Absyn_Tuple_p_tag; struct Cyc_Absyn_Tuple_p_struct{ int tag;
struct Cyc_List_List* f1;}; extern const int Cyc_Absyn_Pointer_p_tag; struct Cyc_Absyn_Pointer_p_struct{
int tag; struct Cyc_Absyn_Pat* f1;}; extern const int Cyc_Absyn_Reference_p_tag;
struct Cyc_Absyn_Reference_p_struct{ int tag; struct Cyc_Absyn_Vardecl* f1;};
extern const int Cyc_Absyn_Struct_p_tag; struct Cyc_Absyn_Struct_p_struct{ int
tag; struct Cyc_Absyn_Structdecl* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List*
f3; struct Cyc_List_List* f4;}; extern const int Cyc_Absyn_Enum_p_tag; struct
Cyc_Absyn_Enum_p_struct{ int tag; struct _tuple1* f1; struct Cyc_Core_Opt* f2;
struct Cyc_List_List* f3; struct Cyc_List_List* f4; struct Cyc_Absyn_Enumdecl*
f5; struct Cyc_Absyn_Enumfield* f6;}; extern const int Cyc_Absyn_Xenum_p_tag;
struct Cyc_Absyn_Xenum_p_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Xenumdecl* f4; struct Cyc_Absyn_Enumfield*
f5;}; extern const int Cyc_Absyn_UnknownId_p_tag; struct Cyc_Absyn_UnknownId_p_struct{
int tag; struct _tuple1* f1;}; extern const int Cyc_Absyn_UnknownCall_p_tag;
struct Cyc_Absyn_UnknownCall_p_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3;}; extern const int Cyc_Absyn_UnknownFields_p_tag;
struct Cyc_Absyn_UnknownFields_p_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3;}; struct Cyc_Absyn_Pat{ void* r; struct Cyc_Core_Opt*
topt; struct Cyc_Position_Segment* loc;}; struct Cyc_Absyn_Switch_clause{ struct
Cyc_Absyn_Pat* pattern; struct Cyc_Core_Opt* pat_vars; struct Cyc_Absyn_Exp*
where_clause; struct Cyc_Absyn_Stmt* body; struct Cyc_Position_Segment* loc;};
extern void* Cyc_Absyn_Unresolved_b; extern const int Cyc_Absyn_Global_b_tag;
struct Cyc_Absyn_Global_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1;};
extern const int Cyc_Absyn_Funname_b_tag; struct Cyc_Absyn_Funname_b_struct{ int
tag; struct Cyc_Absyn_Fndecl* f1;}; extern const int Cyc_Absyn_Param_b_tag;
struct Cyc_Absyn_Param_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1;}; extern
const int Cyc_Absyn_Local_b_tag; struct Cyc_Absyn_Local_b_struct{ int tag;
struct Cyc_Absyn_Vardecl* f1;}; extern const int Cyc_Absyn_Pat_b_tag; struct Cyc_Absyn_Pat_b_struct{
int tag; struct Cyc_Absyn_Vardecl* f1;}; struct Cyc_Absyn_Vardecl{ void* sc;
struct _tuple1* name; struct Cyc_Absyn_Tqual* tq; void* type; struct Cyc_Absyn_Exp*
initializer; int shadow; struct Cyc_Core_Opt* region;}; struct Cyc_Absyn_Fndecl{
void* sc; int is_inline; struct _tuple1* name; struct Cyc_List_List* tvs; struct
Cyc_Core_Opt* effect; void* ret_type; struct Cyc_List_List* args; int varargs;
struct Cyc_Absyn_Stmt* body; struct Cyc_Core_Opt* cached_typ; struct Cyc_Core_Opt*
param_vardecls;}; struct Cyc_Absyn_Structdecl{ void* sc; struct Cyc_Core_Opt*
name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt* fields;}; struct Cyc_Absyn_Enumfield{
struct _tuple1* name; struct Cyc_Absyn_Exp* tag; struct Cyc_List_List* tvs;
struct Cyc_List_List* typs; struct Cyc_Position_Segment* loc;}; struct Cyc_Absyn_Enumdecl{
void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields;}; struct Cyc_Absyn_Xenumdecl{ void* sc; struct _tuple1* name; struct Cyc_List_List*
fields;}; struct Cyc_Absyn_Typedefdecl{ struct _tuple1* name; struct Cyc_List_List*
tvs; void* defn;}; extern const int Cyc_Absyn_Var_d_tag; struct Cyc_Absyn_Var_d_struct{
int tag; struct Cyc_Absyn_Vardecl* f1;}; extern const int Cyc_Absyn_Fn_d_tag;
struct Cyc_Absyn_Fn_d_struct{ int tag; struct Cyc_Absyn_Fndecl* f1;}; extern
const int Cyc_Absyn_Let_d_tag; struct Cyc_Absyn_Let_d_struct{ int tag; struct
Cyc_Absyn_Pat* f1; struct Cyc_Core_Opt* f2; struct Cyc_Core_Opt* f3; struct Cyc_Absyn_Exp*
f4; int f5;}; extern const int Cyc_Absyn_Struct_d_tag; struct Cyc_Absyn_Struct_d_struct{
int tag; struct Cyc_Absyn_Structdecl* f1;}; extern void* Cyc_Absyn_Union_d;
extern const int Cyc_Absyn_Enum_d_tag; struct Cyc_Absyn_Enum_d_struct{ int tag;
struct Cyc_Absyn_Enumdecl* f1;}; extern const int Cyc_Absyn_Xenum_d_tag; struct
Cyc_Absyn_Xenum_d_struct{ int tag; struct Cyc_Absyn_Xenumdecl* f1;}; extern
const int Cyc_Absyn_Typedef_d_tag; struct Cyc_Absyn_Typedef_d_struct{ int tag;
struct Cyc_Absyn_Typedefdecl* f1;}; extern const int Cyc_Absyn_Namespace_d_tag;
struct Cyc_Absyn_Namespace_d_struct{ int tag; struct _tagged_string* f1; struct
Cyc_List_List* f2;}; extern const int Cyc_Absyn_Using_d_tag; struct Cyc_Absyn_Using_d_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2;}; extern const int Cyc_Absyn_ExternC_d_tag;
struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1;}; struct
Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc;}; extern const int
Cyc_Absyn_ArrayElement_tag; struct Cyc_Absyn_ArrayElement_struct{ int tag;
struct Cyc_Absyn_Exp* f1;}; extern const int Cyc_Absyn_FieldName_tag; struct Cyc_Absyn_FieldName_struct{
int tag; struct _tagged_string* f1;}; extern char Cyc_Absyn_EmptyAnnot_tag[ 11u];
struct Cyc_Absyn_EmptyAnnot_struct{ char* tag;}; extern struct Cyc_Absyn_Tqual*
Cyc_Absyn_combine_tqual( struct Cyc_Absyn_Tqual* x, struct Cyc_Absyn_Tqual* y);
extern struct Cyc_Absyn_Tqual* Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref*
Cyc_Absyn_new_conref( void* x); extern struct Cyc_Absyn_Conref* Cyc_Absyn_empty_conref();
extern void* Cyc_Absyn_new_evar( void*); extern void* Cyc_Absyn_uchar_t; extern
void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_sint_t; extern void* Cyc_Absyn_float_t;
extern void* Cyc_Absyn_double_t; extern void* Cyc_Absyn_starb_typ( void* t, void*
rgn, struct Cyc_Absyn_Tqual* tq, void* b); extern void* Cyc_Absyn_atb_typ( void*
t, void* rgn, struct Cyc_Absyn_Tqual* tq, void* b); extern void* Cyc_Absyn_tagged_typ(
void* t, void* rgn, struct Cyc_Absyn_Tqual* tq); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_new_exp( void*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_null_exp( struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_true_exp( struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_false_exp( struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_int_exp( void*, int, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_signed_int_exp( int, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*); extern struct
Cyc_Absyn_Exp* Cyc_Absyn_char_exp( char c, struct Cyc_Position_Segment*); extern
struct Cyc_Absyn_Exp* Cyc_Absyn_float_exp( struct _tagged_string f, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_string_exp( int heap_allocate, struct
_tagged_string s, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_primop_exp( void*, struct Cyc_List_List* es, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_prim1_exp( void*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_prim2_exp(
void*, struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_eq_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_neq_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_gt_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_lt_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_gte_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_lte_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_assignop_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Core_Opt*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
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
extern struct Cyc_Absyn_Exp* Cyc_Absyn_sizeof_exp( void* t, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_deref_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_structmember_exp(
struct Cyc_Absyn_Exp*, struct _tagged_string*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_structarrow_exp( struct Cyc_Absyn_Exp*,
struct _tagged_string*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_subscript_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_tuple_exp( struct
Cyc_List_List*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_stmt_exp(
struct Cyc_Absyn_Stmt*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_new_stmt( void* s, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_skip_stmt( struct Cyc_Position_Segment* loc); extern
struct Cyc_Absyn_Stmt* Cyc_Absyn_exp_stmt( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_seq_stmt( struct Cyc_Absyn_Stmt*
s1, struct Cyc_Absyn_Stmt* s2, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_return_stmt( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_ifthenelse_stmt( struct Cyc_Absyn_Exp*
e, struct Cyc_Absyn_Stmt* s1, struct Cyc_Absyn_Stmt* s2, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_while_stmt( struct Cyc_Absyn_Exp*
e, struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_break_stmt( struct Cyc_Position_Segment* loc); extern
struct Cyc_Absyn_Stmt* Cyc_Absyn_continue_stmt( struct Cyc_Position_Segment* loc);
extern struct Cyc_Absyn_Stmt* Cyc_Absyn_for_stmt( struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3, struct Cyc_Absyn_Stmt* s,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_switch_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_List_List*, struct Cyc_Position_Segment* loc);
extern struct Cyc_Absyn_Stmt* Cyc_Absyn_fallthru_stmt( struct Cyc_List_List* el,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_do_stmt(
struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
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
fs, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_enum_decl(
void* s, struct Cyc_Core_Opt* n, struct Cyc_List_List* ts, struct Cyc_Core_Opt*
fs, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_xenum_decl(
void* s, struct _tuple1* n, struct Cyc_List_List* fs, struct Cyc_Position_Segment*
loc); extern void* Cyc_Absyn_function_typ( struct Cyc_List_List* tvs, struct Cyc_Core_Opt*
eff_typ, void* ret_typ, struct Cyc_List_List* args, int varargs); struct Cyc_PP_Ppstate;
typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef
struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc*
Cyc_PP_doc_t; extern void Cyc_Lex_register_typedef( struct _tuple1* s); extern
void Cyc_Lex_enter_namespace( struct _tagged_string*); extern void Cyc_Lex_leave_namespace();
extern void Cyc_Lex_enter_using( struct _tuple1*); extern void Cyc_Lex_leave_using();
typedef void* Cyc_Parse_Struct_or_union; void* Cyc_Parse_Struct_su=( void*) 0;
void* Cyc_Parse_Union_su=( void*) 1; typedef void* Cyc_Parse_struct_or_union_t;
typedef void* Cyc_Parse_Blockitem; const int Cyc_Parse_TopDecls_bl_tag= 0;
struct Cyc_Parse_TopDecls_bl_struct{ int tag; struct Cyc_List_List* f1;}; const
int Cyc_Parse_Stmt_bl_tag= 1; struct Cyc_Parse_Stmt_bl_struct{ int tag; struct
Cyc_Absyn_Stmt* f1;}; const int Cyc_Parse_FnDecl_bl_tag= 2; struct Cyc_Parse_FnDecl_bl_struct{
int tag; struct Cyc_Absyn_Fndecl* f1;}; typedef void* Cyc_Parse_blockitem_t;
typedef void* Cyc_Parse_Type_specifier; const int Cyc_Parse_Signed_spec_tag= 0;
struct Cyc_Parse_Signed_spec_struct{ int tag; struct Cyc_Position_Segment* f1;};
const int Cyc_Parse_Unsigned_spec_tag= 1; struct Cyc_Parse_Unsigned_spec_struct{
int tag; struct Cyc_Position_Segment* f1;}; const int Cyc_Parse_Short_spec_tag=
2; struct Cyc_Parse_Short_spec_struct{ int tag; struct Cyc_Position_Segment* f1;};
const int Cyc_Parse_Long_spec_tag= 3; struct Cyc_Parse_Long_spec_struct{ int tag;
struct Cyc_Position_Segment* f1;}; const int Cyc_Parse_Type_spec_tag= 4; struct
Cyc_Parse_Type_spec_struct{ int tag; void* f1; struct Cyc_Position_Segment* f2;};
const int Cyc_Parse_Decl_spec_tag= 5; struct Cyc_Parse_Decl_spec_struct{ int tag;
struct Cyc_Absyn_Decl* f1;}; typedef void* Cyc_Parse_type_specifier_t; typedef
void* Cyc_Parse_Storage_class; void* Cyc_Parse_Typedef_sc=( void*) 0; void* Cyc_Parse_Extern_sc=(
void*) 1; void* Cyc_Parse_ExternC_sc=( void*) 2; void* Cyc_Parse_Static_sc=(
void*) 3; void* Cyc_Parse_Auto_sc=( void*) 4; void* Cyc_Parse_Register_sc=( void*)
5; void* Cyc_Parse_Abstract_sc=( void*) 6; typedef void* Cyc_Parse_storage_class_t;
struct Cyc_Parse_Declaration_spec{ struct Cyc_Core_Opt* sc; struct Cyc_Absyn_Tqual*
tq; struct Cyc_List_List* type_specs; int is_inline;}; typedef struct Cyc_Parse_Declaration_spec*
Cyc_Parse_decl_spec_t; struct Cyc_Parse_Declarator{ struct _tuple1* id; struct
Cyc_List_List* tms;}; typedef struct Cyc_Parse_Declarator* Cyc_Parse_declarator_t;
struct Cyc_Parse_Abstractdeclarator{ struct Cyc_List_List* tms;}; typedef struct
Cyc_Parse_Abstractdeclarator* Cyc_Parse_abstractdeclarator_t; static struct
_tuple4* Cyc_Parse_fnargspec_to_arg( struct Cyc_Position_Segment* loc, struct
_tuple2* t); static struct _tuple5 Cyc_Parse_collapse_type_specifiers( struct
Cyc_List_List* ts, struct Cyc_Position_Segment* loc); static struct _tuple6 Cyc_Parse_apply_tms(
struct Cyc_Absyn_Tqual* tq, void* t, struct Cyc_List_List* tms); static struct
Cyc_Absyn_Decl* Cyc_Parse_v_typ_to_typedef( struct Cyc_Position_Segment* loc,
struct _tuple7* t); struct Cyc_Core_Opt* Cyc_Parse_lbuf= 0; static struct Cyc_List_List*
Cyc_Parse_parse_result= 0; static void Cyc_Parse_err( struct _tagged_string msg,
struct Cyc_Position_Segment* sg){ Cyc_Position_post_error( Cyc_Position_mk_err_parse(
sg, msg));} static void* Cyc_Parse_abort( struct _tagged_string msg, struct Cyc_Position_Segment*
sg){ Cyc_Parse_err( msg, sg);( void) _throw(({ struct Cyc_Position_Exit_struct*
_temp0=( struct Cyc_Position_Exit_struct*) GC_malloc_atomic( sizeof( struct Cyc_Position_Exit_struct));*
_temp0=( struct Cyc_Position_Exit_struct){.tag= Cyc_Position_Exit_tag};( struct
_xenum_struct*) _temp0;}));} static void Cyc_Parse_warn( struct _tagged_string
msg, struct Cyc_Position_Segment* sg){({ struct _tagged_string _temp1= Cyc_Position_string_of_segment(
sg); struct _tagged_string _temp2= msg; fprintf( Cyc_Stdio_stderr,"%.*s: Warning: %.*s\n",
_temp1.last_plus_one - _temp1.curr, _temp1.curr, _temp2.last_plus_one - _temp2.curr,
_temp2.curr);}); return;} static void* Cyc_Parse_unimp( struct _tagged_string
msg, struct Cyc_Position_Segment* sg){ return(( void*(*)( struct _tagged_string
msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({ struct _tagged_string
_temp3= msg; xprintf("%.*s unimplemented", _temp3.last_plus_one - _temp3.curr,
_temp3.curr);}), sg);} static void Cyc_Parse_unimp2( struct _tagged_string msg,
struct Cyc_Position_Segment* sg){({ struct _tagged_string _temp4= Cyc_Position_string_of_segment(
sg); struct _tagged_string _temp5= msg; fprintf( Cyc_Stdio_stderr,"%.*s: Warning: Cyclone does not yet support %.*s\n",
_temp4.last_plus_one - _temp4.curr, _temp4.curr, _temp5.last_plus_one - _temp5.curr,
_temp5.curr);}); return;} static struct _tuple4* Cyc_Parse_make_struct_field(
struct Cyc_Position_Segment* loc, struct _tuple7* field){ if((* field).f4 != 0){
Cyc_Parse_err(( struct _tagged_string)({ char* _temp6=( char*)"bad type params in struct field";
struct _tagged_string _temp7; _temp7.curr= _temp6; _temp7.base= _temp6; _temp7.last_plus_one=
_temp6 + 32; _temp7;}), loc);}{ struct _tuple1* qid=(* field).f1;{ void* _temp8=(*
qid).f1; struct Cyc_List_List* _temp18; struct Cyc_List_List* _temp20; _LL10:
if(( unsigned int) _temp8 > 1u?(( struct _enum_struct*) _temp8)->tag == Cyc_Absyn_Rel_n_tag:
0){ _LL19: _temp18=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp8)->f1; if( _temp18 == 0){ goto _LL11;} else{ goto _LL12;}} else{ goto
_LL12;} _LL12: if(( unsigned int) _temp8 > 1u?(( struct _enum_struct*) _temp8)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL21: _temp20=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp8)->f1; if( _temp20 == 0){ goto _LL13;} else{ goto _LL14;}} else{ goto
_LL14;} _LL14: if( _temp8 == Cyc_Absyn_Loc_n){ goto _LL15;} else{ goto _LL16;}
_LL16: goto _LL17; _LL11: goto _LL9; _LL13: goto _LL9; _LL15: goto _LL9; _LL17:
Cyc_Parse_err(( struct _tagged_string)({ char* _temp22=( char*)"struct field cannot be qualified with a module name";
struct _tagged_string _temp23; _temp23.curr= _temp22; _temp23.base= _temp22;
_temp23.last_plus_one= _temp22 + 52; _temp23;}), loc); goto _LL9; _LL9:;} return({
struct _tuple4* _temp24=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4) *
1); _temp24[ 0]=({ struct _tuple4 _temp25; _temp25.f1=(* qid).f2; _temp25.f2=(*
field).f2; _temp25.f3=(* field).f3; _temp25;}); _temp24;});}} static struct
_tuple2* Cyc_Parse_make_param( struct Cyc_Position_Segment* loc, struct _tuple8*
field){ struct _tuple8 _temp28; struct Cyc_List_List* _temp29; void* _temp31;
struct Cyc_Absyn_Tqual* _temp33; struct Cyc_Core_Opt* _temp35; struct _tuple8*
_temp26= field; _temp28=* _temp26; _LL36: _temp35= _temp28.f1; goto _LL34; _LL34:
_temp33= _temp28.f2; goto _LL32; _LL32: _temp31= _temp28.f3; goto _LL30; _LL30:
_temp29= _temp28.f4; goto _LL27; _LL27: { struct Cyc_Core_Opt* idopt= 0; if(
_temp35 != 0){ idopt=({ struct Cyc_Core_Opt* _temp37=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp37->v=( void*)(*(( struct _tuple1*)
_temp35->v)).f2; _temp37;});{ void* _temp38=(*(( struct _tuple1*) _temp35->v)).f1;
struct Cyc_List_List* _temp48; struct Cyc_List_List* _temp50; _LL40: if((
unsigned int) _temp38 > 1u?(( struct _enum_struct*) _temp38)->tag == Cyc_Absyn_Rel_n_tag:
0){ _LL49: _temp48=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp38)->f1; if( _temp48 == 0){ goto _LL41;} else{ goto _LL42;}} else{ goto
_LL42;} _LL42: if(( unsigned int) _temp38 > 1u?(( struct _enum_struct*) _temp38)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL51: _temp50=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp38)->f1; if( _temp50 == 0){ goto _LL43;} else{ goto _LL44;}} else{ goto
_LL44;} _LL44: if( _temp38 == Cyc_Absyn_Loc_n){ goto _LL45;} else{ goto _LL46;}
_LL46: goto _LL47; _LL41: goto _LL39; _LL43: goto _LL39; _LL45: goto _LL39;
_LL47: Cyc_Parse_err(( struct _tagged_string)({ char* _temp52=( char*)"parameter cannot be qualified with a module name";
struct _tagged_string _temp53; _temp53.curr= _temp52; _temp53.base= _temp52;
_temp53.last_plus_one= _temp52 + 49; _temp53;}), loc); goto _LL39; _LL39:;}} if(
_temp29 != 0){(( void(*)( struct _tagged_string msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)(( struct _tagged_string)({ char* _temp54=( char*)"parameter should have no type parameters";
struct _tagged_string _temp55; _temp55.curr= _temp54; _temp55.base= _temp54;
_temp55.last_plus_one= _temp54 + 41; _temp55;}), loc);} return({ struct _tuple2*
_temp56=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2)); _temp56->f1=
idopt; _temp56->f2= _temp33; _temp56->f3= _temp31; _temp56;});}} static void*
Cyc_Parse_type_spec( void* t, struct Cyc_Position_Segment* loc){ return({ struct
Cyc_Parse_Type_spec_struct* _temp57=( struct Cyc_Parse_Type_spec_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Type_spec_struct)); _temp57->tag= Cyc_Parse_Type_spec_tag;
_temp57->f1=( void*) t; _temp57->f2= loc;( void*) _temp57;});} static void* Cyc_Parse_array2ptr(
void* t){ void* _temp58= t; struct Cyc_Absyn_Exp* _temp64; struct Cyc_Absyn_Tqual*
_temp66; void* _temp68; _LL60: if(( unsigned int) _temp58 > 5u?(( struct
_enum_struct*) _temp58)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL69: _temp68=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp58)->f1; goto _LL67; _LL67:
_temp66=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*) _temp58)->f2;
goto _LL65; _LL65: _temp64=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp58)->f3; goto _LL61;} else{ goto _LL62;} _LL62: goto _LL63; _LL61: return
Cyc_Absyn_starb_typ( _temp68, Cyc_Absyn_HeapRgn, _temp66,({ struct Cyc_Absyn_Upper_b_struct*
_temp70=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp70->tag= Cyc_Absyn_Upper_b_tag; _temp70->f1= _temp64;( void*) _temp70;}));
_LL63: return t; _LL59:;} static void Cyc_Parse_arg_array2ptr( struct _tuple2* x){(*
x).f3= Cyc_Parse_array2ptr((* x).f3);} static struct _tuple9* Cyc_Parse_get_tqual_typ(
struct Cyc_Position_Segment* loc, struct _tuple2* t){ return({ struct _tuple9*
_temp71=( struct _tuple9*) GC_malloc( sizeof( struct _tuple9)); _temp71->f1=(* t).f2;
_temp71->f2=(* t).f3; _temp71;});} static void Cyc_Parse_only_vardecl( struct
Cyc_List_List* params, struct Cyc_Absyn_Decl* x){ struct _tagged_string
decl_kind;{ void* _temp72=( void*) x->r; struct Cyc_Absyn_Vardecl* _temp96; int
_temp98; struct Cyc_Absyn_Exp* _temp100; struct Cyc_Core_Opt* _temp102; struct
Cyc_Core_Opt* _temp104; struct Cyc_Absyn_Pat* _temp106; struct Cyc_Absyn_Fndecl*
_temp108; struct Cyc_Absyn_Structdecl* _temp110; struct Cyc_Absyn_Enumdecl*
_temp112; struct Cyc_Absyn_Typedefdecl* _temp114; struct Cyc_Absyn_Xenumdecl*
_temp116; struct Cyc_List_List* _temp118; struct _tagged_string* _temp120;
struct Cyc_List_List* _temp122; struct _tuple1* _temp124; struct Cyc_List_List*
_temp126; _LL74: if(( unsigned int) _temp72 > 1u?(( struct _enum_struct*)
_temp72)->tag == Cyc_Absyn_Var_d_tag: 0){ _LL97: _temp96=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_d_struct*) _temp72)->f1; goto _LL75;} else{ goto _LL76;}
_LL76: if(( unsigned int) _temp72 > 1u?(( struct _enum_struct*) _temp72)->tag ==
Cyc_Absyn_Let_d_tag: 0){ _LL107: _temp106=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*)
_temp72)->f1; goto _LL105; _LL105: _temp104=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp72)->f2; goto _LL103; _LL103: _temp102=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp72)->f3; goto _LL101; _LL101: _temp100=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_Let_d_struct*) _temp72)->f4; goto _LL99; _LL99: _temp98=( int)((
struct Cyc_Absyn_Let_d_struct*) _temp72)->f5; goto _LL77;} else{ goto _LL78;}
_LL78: if(( unsigned int) _temp72 > 1u?(( struct _enum_struct*) _temp72)->tag ==
Cyc_Absyn_Fn_d_tag: 0){ _LL109: _temp108=( struct Cyc_Absyn_Fndecl*)(( struct
Cyc_Absyn_Fn_d_struct*) _temp72)->f1; goto _LL79;} else{ goto _LL80;} _LL80: if((
unsigned int) _temp72 > 1u?(( struct _enum_struct*) _temp72)->tag == Cyc_Absyn_Struct_d_tag:
0){ _LL111: _temp110=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*)
_temp72)->f1; goto _LL81;} else{ goto _LL82;} _LL82: if( _temp72 == Cyc_Absyn_Union_d){
goto _LL83;} else{ goto _LL84;} _LL84: if(( unsigned int) _temp72 > 1u?(( struct
_enum_struct*) _temp72)->tag == Cyc_Absyn_Enum_d_tag: 0){ _LL113: _temp112=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*) _temp72)->f1;
goto _LL85;} else{ goto _LL86;} _LL86: if(( unsigned int) _temp72 > 1u?(( struct
_enum_struct*) _temp72)->tag == Cyc_Absyn_Typedef_d_tag: 0){ _LL115: _temp114=(
struct Cyc_Absyn_Typedefdecl*)(( struct Cyc_Absyn_Typedef_d_struct*) _temp72)->f1;
goto _LL87;} else{ goto _LL88;} _LL88: if(( unsigned int) _temp72 > 1u?(( struct
_enum_struct*) _temp72)->tag == Cyc_Absyn_Xenum_d_tag: 0){ _LL117: _temp116=(
struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_d_struct*) _temp72)->f1;
goto _LL89;} else{ goto _LL90;} _LL90: if(( unsigned int) _temp72 > 1u?(( struct
_enum_struct*) _temp72)->tag == Cyc_Absyn_Namespace_d_tag: 0){ _LL121: _temp120=(
struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*) _temp72)->f1;
goto _LL119; _LL119: _temp118=( struct Cyc_List_List*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp72)->f2; goto _LL91;} else{ goto _LL92;} _LL92: if(( unsigned int) _temp72
> 1u?(( struct _enum_struct*) _temp72)->tag == Cyc_Absyn_Using_d_tag: 0){ _LL125:
_temp124=( struct _tuple1*)(( struct Cyc_Absyn_Using_d_struct*) _temp72)->f1;
goto _LL123; _LL123: _temp122=( struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*)
_temp72)->f2; goto _LL93;} else{ goto _LL94;} _LL94: if(( unsigned int) _temp72
> 1u?(( struct _enum_struct*) _temp72)->tag == Cyc_Absyn_ExternC_d_tag: 0){
_LL127: _temp126=( struct Cyc_List_List*)(( struct Cyc_Absyn_ExternC_d_struct*)
_temp72)->f1; goto _LL95;} else{ goto _LL73;} _LL75: if( _temp96->initializer !=
0){(( void(*)( struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_string)({ char* _temp128=( char*)"initializers are not allowed in parameter declarations";
struct _tagged_string _temp129; _temp129.curr= _temp128; _temp129.base= _temp128;
_temp129.last_plus_one= _temp128 + 55; _temp129;}), x->loc);}{ void* _temp130=(*
_temp96->name).f1; struct Cyc_List_List* _temp140; struct Cyc_List_List*
_temp142; _LL132: if( _temp130 == Cyc_Absyn_Loc_n){ goto _LL133;} else{ goto
_LL134;} _LL134: if(( unsigned int) _temp130 > 1u?(( struct _enum_struct*)
_temp130)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL141: _temp140=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp130)->f1; if( _temp140 == 0){ goto _LL135;}
else{ goto _LL136;}} else{ goto _LL136;} _LL136: if(( unsigned int) _temp130 > 1u?((
struct _enum_struct*) _temp130)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL143:
_temp142=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp130)->f1;
if( _temp142 == 0){ goto _LL137;} else{ goto _LL138;}} else{ goto _LL138;}
_LL138: goto _LL139; _LL133: goto _LL131; _LL135: goto _LL131; _LL137: goto
_LL131; _LL139: Cyc_Parse_err(( struct _tagged_string)({ char* _temp144=( char*)"module names not allowed on parameter declarations";
struct _tagged_string _temp145; _temp145.curr= _temp144; _temp145.base= _temp144;
_temp145.last_plus_one= _temp144 + 51; _temp145;}), x->loc); goto _LL131; _LL131:;}{
int found= 0; for( 0; params != 0; params= params->tl){ if( Cyc_String_zstrptrcmp((*
_temp96->name).f2,( struct _tagged_string*) params->hd) == 0){ found= 1; break;}}
if( ! found){(( void(*)( struct _tagged_string msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)(({ struct _tagged_string _temp146=*(* _temp96->name).f2;
xprintf("%.*s is not listed as a parameter", _temp146.last_plus_one - _temp146.curr,
_temp146.curr);}), x->loc);} return;} _LL77: decl_kind=( struct _tagged_string)({
char* _temp147=( char*)"let declaration"; struct _tagged_string _temp148;
_temp148.curr= _temp147; _temp148.base= _temp147; _temp148.last_plus_one=
_temp147 + 16; _temp148;}); goto _LL73; _LL79: decl_kind=( struct _tagged_string)({
char* _temp149=( char*)"function declaration"; struct _tagged_string _temp150;
_temp150.curr= _temp149; _temp150.base= _temp149; _temp150.last_plus_one=
_temp149 + 21; _temp150;}); goto _LL73; _LL81: decl_kind=( struct _tagged_string)({
char* _temp151=( char*)"struct declaration"; struct _tagged_string _temp152;
_temp152.curr= _temp151; _temp152.base= _temp151; _temp152.last_plus_one=
_temp151 + 19; _temp152;}); goto _LL73; _LL83: decl_kind=( struct _tagged_string)({
char* _temp153=( char*)"union declaration"; struct _tagged_string _temp154;
_temp154.curr= _temp153; _temp154.base= _temp153; _temp154.last_plus_one=
_temp153 + 18; _temp154;}); goto _LL73; _LL85: decl_kind=( struct _tagged_string)({
char* _temp155=( char*)"enum declaration"; struct _tagged_string _temp156;
_temp156.curr= _temp155; _temp156.base= _temp155; _temp156.last_plus_one=
_temp155 + 17; _temp156;}); goto _LL73; _LL87: decl_kind=( struct _tagged_string)({
char* _temp157=( char*)"typedef"; struct _tagged_string _temp158; _temp158.curr=
_temp157; _temp158.base= _temp157; _temp158.last_plus_one= _temp157 + 8;
_temp158;}); goto _LL73; _LL89: decl_kind=( struct _tagged_string)({ char*
_temp159=( char*)"xenum declaration"; struct _tagged_string _temp160; _temp160.curr=
_temp159; _temp160.base= _temp159; _temp160.last_plus_one= _temp159 + 18;
_temp160;}); goto _LL73; _LL91: decl_kind=( struct _tagged_string)({ char*
_temp161=( char*)"namespace declaration"; struct _tagged_string _temp162;
_temp162.curr= _temp161; _temp162.base= _temp161; _temp162.last_plus_one=
_temp161 + 22; _temp162;}); goto _LL73; _LL93: decl_kind=( struct _tagged_string)({
char* _temp163=( char*)"using declaration"; struct _tagged_string _temp164;
_temp164.curr= _temp163; _temp164.base= _temp163; _temp164.last_plus_one=
_temp163 + 18; _temp164;}); goto _LL73; _LL95: decl_kind=( struct _tagged_string)({
char* _temp165=( char*)"extern C declaration"; struct _tagged_string _temp166;
_temp166.curr= _temp165; _temp166.base= _temp165; _temp166.last_plus_one=
_temp165 + 21; _temp166;}); goto _LL73; _LL73:;}(( void(*)( struct
_tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({ struct
_tagged_string _temp167= decl_kind; xprintf("%.*s appears in parameter type",
_temp167.last_plus_one - _temp167.curr, _temp167.curr);}), x->loc); return;}
static struct _tuple2* Cyc_Parse_get_param_type( struct _tuple10* env, struct
_tagged_string* x){ struct _tuple10 _temp170; struct Cyc_Position_Segment*
_temp171; struct Cyc_List_List* _temp173; struct _tuple10* _temp168= env;
_temp170=* _temp168; _LL174: _temp173= _temp170.f1; goto _LL172; _LL172:
_temp171= _temp170.f2; goto _LL169; _LL169: if( _temp173 == 0){ return(( struct
_tuple2*(*)( struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({
struct _tagged_string _temp175=* x; xprintf("missing type for parameter %.*s",
_temp175.last_plus_one - _temp175.curr, _temp175.curr);}), _temp171);}{ void*
_temp176=( void*)(( struct Cyc_Absyn_Decl*) _temp173->hd)->r; struct Cyc_Absyn_Vardecl*
_temp182; _LL178: if(( unsigned int) _temp176 > 1u?(( struct _enum_struct*)
_temp176)->tag == Cyc_Absyn_Var_d_tag: 0){ _LL183: _temp182=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_d_struct*) _temp176)->f1; goto _LL179;} else{ goto _LL180;}
_LL180: goto _LL181; _LL179:{ void* _temp184=(* _temp182->name).f1; struct Cyc_List_List*
_temp194; struct Cyc_List_List* _temp196; _LL186: if( _temp184 == Cyc_Absyn_Loc_n){
goto _LL187;} else{ goto _LL188;} _LL188: if(( unsigned int) _temp184 > 1u?((
struct _enum_struct*) _temp184)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL195:
_temp194=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp184)->f1;
if( _temp194 == 0){ goto _LL189;} else{ goto _LL190;}} else{ goto _LL190;}
_LL190: if(( unsigned int) _temp184 > 1u?(( struct _enum_struct*) _temp184)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL197: _temp196=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp184)->f1; if( _temp196 == 0){ goto _LL191;} else{
goto _LL192;}} else{ goto _LL192;} _LL192: goto _LL193; _LL187: goto _LL185;
_LL189: goto _LL185; _LL191: goto _LL185; _LL193: Cyc_Parse_err(( struct
_tagged_string)({ char* _temp198=( char*)"module name not allowed on parameter";
struct _tagged_string _temp199; _temp199.curr= _temp198; _temp199.base= _temp198;
_temp199.last_plus_one= _temp198 + 37; _temp199;}), _temp171); goto _LL185;
_LL185:;} if( Cyc_String_zstrptrcmp((* _temp182->name).f2, x) == 0){ return({
struct _tuple2* _temp200=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2) *
1); _temp200[ 0]=({ struct _tuple2 _temp201; _temp201.f1=( struct Cyc_Core_Opt*)({
struct Cyc_Core_Opt* _temp202=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt) * 1); _temp202[ 0]=({ struct Cyc_Core_Opt _temp203; _temp203.v=(
void*)(* _temp182->name).f2; _temp203;}); _temp202;}); _temp201.f2= _temp182->tq;
_temp201.f3=( void*) _temp182->type; _temp201;}); _temp200;});} else{ return Cyc_Parse_get_param_type(({
struct _tuple10* _temp204=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp204->f1= _temp173->tl; _temp204->f2= _temp171; _temp204;}), x);} _LL181:
return(( struct _tuple2*(*)( struct _tagged_string msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)(( struct _tagged_string)({ char* _temp205=( char*)"non-variable declaration";
struct _tagged_string _temp206; _temp206.curr= _temp205; _temp206.base= _temp205;
_temp206.last_plus_one= _temp205 + 25; _temp206;}),(( struct Cyc_Absyn_Decl*)
_temp173->hd)->loc); _LL177:;}} static int Cyc_Parse_is_typeparam( void* tm){
void* _temp207= tm; int _temp213; struct Cyc_Position_Segment* _temp215; struct
Cyc_List_List* _temp217; _LL209: if(( unsigned int) _temp207 > 1u?(( struct
_enum_struct*) _temp207)->tag == Cyc_Absyn_TypeParams_mod_tag: 0){ _LL218:
_temp217=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp207)->f1; goto _LL216; _LL216: _temp215=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp207)->f2; goto _LL214; _LL214:
_temp213=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp207)->f3; goto
_LL210;} else{ goto _LL211;} _LL211: goto _LL212; _LL210: return 1; _LL212:
return 0; _LL208:;} static void* Cyc_Parse_id2type( struct _tagged_string s,
struct Cyc_Absyn_Conref* k){ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({
char* _temp219=( char*)"`H"; struct _tagged_string _temp220; _temp220.curr=
_temp219; _temp220.base= _temp219; _temp220.last_plus_one= _temp219 + 3;
_temp220;})) == 0){ return Cyc_Absyn_HeapRgn;} else{ return({ struct Cyc_Absyn_VarType_struct*
_temp221=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp221->tag= Cyc_Absyn_VarType_tag; _temp221->f1=({ struct Cyc_Absyn_Tvar*
_temp222=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof( struct Cyc_Absyn_Tvar));
_temp222->name=({ struct _tagged_string* _temp223=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp223[ 0]= s; _temp223;});
_temp222->kind= k; _temp222;});( void*) _temp221;});}} static struct Cyc_Absyn_Tvar*
Cyc_Parse_typ2tvar( struct Cyc_Position_Segment* loc, void* t){ void* _temp224=
t; struct Cyc_Absyn_Tvar* _temp230; _LL226: if(( unsigned int) _temp224 > 5u?((
struct _enum_struct*) _temp224)->tag == Cyc_Absyn_VarType_tag: 0){ _LL231:
_temp230=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*) _temp224)->f1;
goto _LL227;} else{ goto _LL228;} _LL228: goto _LL229; _LL227: return _temp230;
_LL229: return(( struct Cyc_Absyn_Tvar*(*)( struct _tagged_string msg, struct
Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct _tagged_string)({ char*
_temp232=( char*)"expecting a list of type variables, not types"; struct
_tagged_string _temp233; _temp233.curr= _temp232; _temp233.base= _temp232;
_temp233.last_plus_one= _temp232 + 46; _temp233;}), loc); _LL225:;} static void*
Cyc_Parse_tvar2typ( struct Cyc_Absyn_Tvar* pr){ return({ struct Cyc_Absyn_VarType_struct*
_temp234=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp234->tag= Cyc_Absyn_VarType_tag; _temp234->f1= pr;( void*) _temp234;});}
static struct Cyc_List_List* Cyc_Parse_oldstyle2newstyle( struct Cyc_List_List*
tms, struct Cyc_List_List* tds, struct Cyc_Position_Segment* loc){ if( tds == 0){
return tms;} if( tms == 0){ return 0;}{ void* _temp235=( void*) tms->hd; void*
_temp241; _LL237: if(( unsigned int) _temp235 > 1u?(( struct _enum_struct*)
_temp235)->tag == Cyc_Absyn_Function_mod_tag: 0){ _LL242: _temp241=( void*)((
struct Cyc_Absyn_Function_mod_struct*) _temp235)->f1; goto _LL238;} else{ goto
_LL239;} _LL239: goto _LL240; _LL238: if( tms->tl == 0? 1:( Cyc_Parse_is_typeparam((
void*)( tms->tl)->hd)?( tms->tl)->tl == 0: 0)){ void* _temp243= _temp241; struct
Cyc_Core_Opt* _temp249; int _temp251; struct Cyc_List_List* _temp253; struct Cyc_Position_Segment*
_temp255; struct Cyc_List_List* _temp257; _LL245: if((( struct _enum_struct*)
_temp243)->tag == Cyc_Absyn_WithTypes_tag){ _LL254: _temp253=( struct Cyc_List_List*)((
struct Cyc_Absyn_WithTypes_struct*) _temp243)->f1; goto _LL252; _LL252: _temp251=(
int)(( struct Cyc_Absyn_WithTypes_struct*) _temp243)->f2; goto _LL250; _LL250:
_temp249=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_WithTypes_struct*) _temp243)->f3;
goto _LL246;} else{ goto _LL247;} _LL247: if((( struct _enum_struct*) _temp243)->tag
== Cyc_Absyn_NoTypes_tag){ _LL258: _temp257=( struct Cyc_List_List*)(( struct
Cyc_Absyn_NoTypes_struct*) _temp243)->f1; goto _LL256; _LL256: _temp255=( struct
Cyc_Position_Segment*)(( struct Cyc_Absyn_NoTypes_struct*) _temp243)->f2; goto
_LL248;} else{ goto _LL244;} _LL246: Cyc_Parse_warn(( struct _tagged_string)({
char* _temp259=( char*)"function declaration with both new- and old-style parameterdeclarations; ignoring old-style";
struct _tagged_string _temp260; _temp260.curr= _temp259; _temp260.base= _temp259;
_temp260.last_plus_one= _temp259 + 92; _temp260;}), loc); return tms; _LL248:((
void(*)( void(* f)( struct Cyc_List_List*, struct Cyc_Absyn_Decl*), struct Cyc_List_List*
env, struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Parse_only_vardecl,
_temp257, tds);{ struct _tuple10* env=({ struct _tuple10* _temp261=( struct
_tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp261->f1= tds; _temp261->f2=
loc; _temp261;}); return({ struct Cyc_List_List* _temp262=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp262->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp263=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp263->tag= Cyc_Absyn_Function_mod_tag; _temp263->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp264=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp264->tag= Cyc_Absyn_WithTypes_tag; _temp264->f1=(( struct Cyc_List_List*(*)(
struct _tuple2*(* f)( struct _tuple10*, struct _tagged_string*), struct _tuple10*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_get_param_type, env,
_temp257); _temp264->f2= 0; _temp264->f3= 0;( void*) _temp264;});( void*)
_temp263;}); _temp262->tl= 0; _temp262;});} _LL244:;} else{ return({ struct Cyc_List_List*
_temp265=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp265->hd=( void*)(( void*) tms->hd); _temp265->tl= Cyc_Parse_oldstyle2newstyle(
tms->tl, tds, loc); _temp265;});} _LL240: return({ struct Cyc_List_List*
_temp266=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp266->hd=( void*)(( void*) tms->hd); _temp266->tl= Cyc_Parse_oldstyle2newstyle(
tms->tl, tds, loc); _temp266;}); _LL236:;}} static struct Cyc_Absyn_Fndecl* Cyc_Parse_make_function(
struct Cyc_Core_Opt* dso, struct Cyc_Parse_Declarator* d, struct Cyc_List_List*
tds, struct Cyc_Absyn_Stmt* body, struct Cyc_Position_Segment* loc){ if( tds !=
0){ d=({ struct Cyc_Parse_Declarator* _temp267=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp267->id= d->id; _temp267->tms=
Cyc_Parse_oldstyle2newstyle( d->tms, tds, loc); _temp267;});}{ void* sc= Cyc_Absyn_Public;
struct Cyc_List_List* tss= 0; struct Cyc_Absyn_Tqual* tq= Cyc_Absyn_empty_tqual();
int is_inline= 0; if( dso != 0){ tss=(( struct Cyc_Parse_Declaration_spec*) dso->v)->type_specs;
tq=(( struct Cyc_Parse_Declaration_spec*) dso->v)->tq; is_inline=(( struct Cyc_Parse_Declaration_spec*)
dso->v)->is_inline; if((( struct Cyc_Parse_Declaration_spec*) dso->v)->sc != 0){
void* _temp268=( void*)((( struct Cyc_Parse_Declaration_spec*) dso->v)->sc)->v;
_LL270: if( _temp268 == Cyc_Parse_Extern_sc){ goto _LL271;} else{ goto _LL272;}
_LL272: if( _temp268 == Cyc_Parse_ExternC_sc){ goto _LL273;} else{ goto _LL274;}
_LL274: if( _temp268 == Cyc_Parse_Static_sc){ goto _LL275;} else{ goto _LL276;}
_LL276: goto _LL277; _LL271: sc= Cyc_Absyn_Extern; goto _LL269; _LL273: sc= Cyc_Absyn_ExternC;
goto _LL269; _LL275: sc= Cyc_Absyn_Static; goto _LL269; _LL277: Cyc_Parse_err((
struct _tagged_string)({ char* _temp278=( char*)"bad storage class on function";
struct _tagged_string _temp279; _temp279.curr= _temp278; _temp279.base= _temp278;
_temp279.last_plus_one= _temp278 + 30; _temp279;}), loc); goto _LL269; _LL269:;}}{
struct Cyc_Core_Opt* _temp282; void* _temp284; struct _tuple5 _temp280= Cyc_Parse_collapse_type_specifiers(
tss, loc); _LL285: _temp284= _temp280.f1; goto _LL283; _LL283: _temp282=
_temp280.f2; goto _LL281; _LL281: { struct Cyc_List_List* _temp288; void*
_temp290; struct Cyc_Absyn_Tqual* _temp292; struct _tuple6 _temp286= Cyc_Parse_apply_tms(
tq, _temp284, d->tms); _LL293: _temp292= _temp286.f1; goto _LL291; _LL291:
_temp290= _temp286.f2; goto _LL289; _LL289: _temp288= _temp286.f3; goto _LL287;
_LL287: if( _temp282 != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char*
_temp294=( char*)"nested type declaration within function prototype"; struct
_tagged_string _temp295; _temp295.curr= _temp294; _temp295.base= _temp294;
_temp295.last_plus_one= _temp294 + 50; _temp295;}), loc);} if( _temp288 != 0){
Cyc_Parse_warn(( struct _tagged_string)({ char* _temp296=( char*)"bad type params, ignoring";
struct _tagged_string _temp297; _temp297.curr= _temp296; _temp297.base= _temp296;
_temp297.last_plus_one= _temp296 + 26; _temp297;}), loc);}{ void* _temp298=
_temp290; struct Cyc_Absyn_FnInfo _temp304; int _temp306; struct Cyc_List_List*
_temp308; void* _temp310; struct Cyc_Core_Opt* _temp312; struct Cyc_List_List*
_temp314; _LL300: if(( unsigned int) _temp298 > 5u?(( struct _enum_struct*)
_temp298)->tag == Cyc_Absyn_FnType_tag: 0){ _LL305: _temp304=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp298)->f1; _LL315: _temp314=( struct Cyc_List_List*)
_temp304.tvars; goto _LL313; _LL313: _temp312=( struct Cyc_Core_Opt*) _temp304.effect;
goto _LL311; _LL311: _temp310=( void*) _temp304.ret_typ; goto _LL309; _LL309:
_temp308=( struct Cyc_List_List*) _temp304.args; goto _LL307; _LL307: _temp306=(
int) _temp304.varargs; goto _LL301;} else{ goto _LL302;} _LL302: goto _LL303;
_LL301: { struct Cyc_List_List* args2=(( struct Cyc_List_List*(*)( struct
_tuple4*(* f)( struct Cyc_Position_Segment*, struct _tuple2*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_fnargspec_to_arg, loc,
_temp308); return({ struct Cyc_Absyn_Fndecl* _temp316=( struct Cyc_Absyn_Fndecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Fndecl)); _temp316->sc=( void*) sc; _temp316->name=
d->id; _temp316->tvs= _temp314; _temp316->is_inline= is_inline; _temp316->effect=
_temp312; _temp316->ret_type=( void*) _temp310; _temp316->args= args2; _temp316->varargs=
_temp306; _temp316->body= body; _temp316->cached_typ= 0; _temp316->param_vardecls=
0; _temp316;});} _LL303: return(( struct Cyc_Absyn_Fndecl*(*)( struct
_tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct
_tagged_string)({ char* _temp317=( char*)"declarator is not a function prototype";
struct _tagged_string _temp318; _temp318.curr= _temp317; _temp318.base= _temp317;
_temp318.last_plus_one= _temp317 + 39; _temp318;}), loc); _LL299:;}}}}} static
struct _tuple4* Cyc_Parse_fnargspec_to_arg( struct Cyc_Position_Segment* loc,
struct _tuple2* t){ if((* t).f1 == 0){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp319=( char*)"missing argument variable in function prototype"; struct
_tagged_string _temp320; _temp320.curr= _temp319; _temp320.base= _temp319;
_temp320.last_plus_one= _temp319 + 48; _temp320;}), loc); return({ struct
_tuple4* _temp321=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp321->f1=({ struct _tagged_string* _temp322=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp322[ 0]=( struct
_tagged_string)({ char* _temp323=( char*)"?"; struct _tagged_string _temp324;
_temp324.curr= _temp323; _temp324.base= _temp323; _temp324.last_plus_one=
_temp323 + 2; _temp324;}); _temp322;}); _temp321->f2=(* t).f2; _temp321->f3=(* t).f3;
_temp321;});} else{ return({ struct _tuple4* _temp325=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp325->f1=( struct _tagged_string*)((* t).f1)->v;
_temp325->f2=(* t).f2; _temp325->f3=(* t).f3; _temp325;});}} static char
_temp328[ 52u]="at most one type may appear within a type specifier"; static
struct _tagged_string Cyc_Parse_msg1=( struct _tagged_string){ _temp328,
_temp328, _temp328 + 52u}; static char _temp331[ 63u]="const or volatile may appear only once within a type specifier";
static struct _tagged_string Cyc_Parse_msg2=( struct _tagged_string){ _temp331,
_temp331, _temp331 + 63u}; static char _temp334[ 50u]="type specifier includes more than one declaration";
static struct _tagged_string Cyc_Parse_msg3=( struct _tagged_string){ _temp334,
_temp334, _temp334 + 50u}; static char _temp337[ 60u]="sign specifier may appear only once within a type specifier";
static struct _tagged_string Cyc_Parse_msg4=( struct _tagged_string){ _temp337,
_temp337, _temp337 + 60u}; static struct _tuple5 Cyc_Parse_collapse_type_specifiers(
struct Cyc_List_List* ts, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt*
declopt= 0; int seen_type= 0; int seen_sign= 0; int seen_size= 0; void* t= Cyc_Absyn_VoidType;
void* sz= Cyc_Absyn_B4; void* sgn= Cyc_Absyn_Signed; struct Cyc_Position_Segment*
last_loc= loc; for( 0; ts != 0; ts= ts->tl){ void* _temp338=( void*) ts->hd;
struct Cyc_Position_Segment* _temp352; void* _temp354; struct Cyc_Position_Segment*
_temp356; struct Cyc_Position_Segment* _temp358; struct Cyc_Position_Segment*
_temp360; struct Cyc_Position_Segment* _temp362; struct Cyc_Absyn_Decl* _temp364;
_LL340: if((( struct _enum_struct*) _temp338)->tag == Cyc_Parse_Type_spec_tag){
_LL355: _temp354=( void*)(( struct Cyc_Parse_Type_spec_struct*) _temp338)->f1;
goto _LL353; _LL353: _temp352=( struct Cyc_Position_Segment*)(( struct Cyc_Parse_Type_spec_struct*)
_temp338)->f2; goto _LL341;} else{ goto _LL342;} _LL342: if((( struct
_enum_struct*) _temp338)->tag == Cyc_Parse_Signed_spec_tag){ _LL357: _temp356=(
struct Cyc_Position_Segment*)(( struct Cyc_Parse_Signed_spec_struct*) _temp338)->f1;
goto _LL343;} else{ goto _LL344;} _LL344: if((( struct _enum_struct*) _temp338)->tag
== Cyc_Parse_Unsigned_spec_tag){ _LL359: _temp358=( struct Cyc_Position_Segment*)((
struct Cyc_Parse_Unsigned_spec_struct*) _temp338)->f1; goto _LL345;} else{ goto
_LL346;} _LL346: if((( struct _enum_struct*) _temp338)->tag == Cyc_Parse_Short_spec_tag){
_LL361: _temp360=( struct Cyc_Position_Segment*)(( struct Cyc_Parse_Short_spec_struct*)
_temp338)->f1; goto _LL347;} else{ goto _LL348;} _LL348: if((( struct
_enum_struct*) _temp338)->tag == Cyc_Parse_Long_spec_tag){ _LL363: _temp362=(
struct Cyc_Position_Segment*)(( struct Cyc_Parse_Long_spec_struct*) _temp338)->f1;
goto _LL349;} else{ goto _LL350;} _LL350: if((( struct _enum_struct*) _temp338)->tag
== Cyc_Parse_Decl_spec_tag){ _LL365: _temp364=( struct Cyc_Absyn_Decl*)(( struct
Cyc_Parse_Decl_spec_struct*) _temp338)->f1; goto _LL351;} else{ goto _LL339;}
_LL341: if( seen_type){ Cyc_Parse_err( Cyc_Parse_msg1, _temp352);} last_loc=
_temp352; seen_type= 1; t= _temp354; goto _LL339; _LL343: if( seen_sign){ Cyc_Parse_err(
Cyc_Parse_msg4, _temp356);} if( seen_type){ Cyc_Parse_err(( struct
_tagged_string)({ char* _temp366=( char*)"signed qualifier must come before type";
struct _tagged_string _temp367; _temp367.curr= _temp366; _temp367.base= _temp366;
_temp367.last_plus_one= _temp366 + 39; _temp367;}), _temp356);} last_loc=
_temp356; seen_sign= 1; sgn= Cyc_Absyn_Signed; goto _LL339; _LL345: if(
seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp358);} if( seen_type){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp368=( char*)"signed qualifier must come before type";
struct _tagged_string _temp369; _temp369.curr= _temp368; _temp369.base= _temp368;
_temp369.last_plus_one= _temp368 + 39; _temp369;}), _temp358);} last_loc=
_temp358; seen_sign= 1; sgn= Cyc_Absyn_Unsigned; goto _LL339; _LL347: if(
seen_size){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp370=( char*)"integral size may appear only once within a type specifier";
struct _tagged_string _temp371; _temp371.curr= _temp370; _temp371.base= _temp370;
_temp371.last_plus_one= _temp370 + 59; _temp371;}), _temp360);} if( seen_type){
Cyc_Parse_err(( struct _tagged_string)({ char* _temp372=( char*)"short modifier must come before base type";
struct _tagged_string _temp373; _temp373.curr= _temp372; _temp373.base= _temp372;
_temp373.last_plus_one= _temp372 + 42; _temp373;}), _temp360);} last_loc=
_temp360; seen_size= 1; sz= Cyc_Absyn_B2; goto _LL339; _LL349: if( seen_type){
Cyc_Parse_err(( struct _tagged_string)({ char* _temp374=( char*)"long modifier must come before base type";
struct _tagged_string _temp375; _temp375.curr= _temp374; _temp375.base= _temp374;
_temp375.last_plus_one= _temp374 + 41; _temp375;}), _temp362);} if( seen_size){
void* _temp376= sz; _LL378: if( _temp376 == Cyc_Absyn_B4){ goto _LL379;} else{
goto _LL380;} _LL380: goto _LL381; _LL379: sz= Cyc_Absyn_B8; goto _LL377; _LL381:
Cyc_Parse_err(( struct _tagged_string)({ char* _temp382=( char*)"extra long in type specifier";
struct _tagged_string _temp383; _temp383.curr= _temp382; _temp383.base= _temp382;
_temp383.last_plus_one= _temp382 + 29; _temp383;}), _temp362); goto _LL377;
_LL377:;} last_loc= _temp362; seen_size= 1; goto _LL339; _LL351: last_loc=
_temp364->loc; if( declopt == 0? ! seen_type: 0){ seen_type= 1;{ void* _temp384=(
void*) _temp364->r; struct Cyc_Absyn_Structdecl* _temp396; struct Cyc_Absyn_Enumdecl*
_temp398; struct Cyc_Absyn_Xenumdecl* _temp400; _LL386: if(( unsigned int)
_temp384 > 1u?(( struct _enum_struct*) _temp384)->tag == Cyc_Absyn_Struct_d_tag:
0){ _LL397: _temp396=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*)
_temp384)->f1; goto _LL387;} else{ goto _LL388;} _LL388: if(( unsigned int)
_temp384 > 1u?(( struct _enum_struct*) _temp384)->tag == Cyc_Absyn_Enum_d_tag: 0){
_LL399: _temp398=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*)
_temp384)->f1; goto _LL389;} else{ goto _LL390;} _LL390: if(( unsigned int)
_temp384 > 1u?(( struct _enum_struct*) _temp384)->tag == Cyc_Absyn_Xenum_d_tag:
0){ _LL401: _temp400=( struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_d_struct*)
_temp384)->f1; goto _LL391;} else{ goto _LL392;} _LL392: if( _temp384 == Cyc_Absyn_Union_d){
goto _LL393;} else{ goto _LL394;} _LL394: goto _LL395; _LL387: { struct Cyc_List_List*
args=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*), struct
Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp396->tvs); t=({
struct Cyc_Absyn_StructType_struct* _temp402=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp402->tag= Cyc_Absyn_StructType_tag;
_temp402->f1=( struct _tuple1*)(( struct _tuple1*)( _temp396->name)->v);
_temp402->f2= args; _temp402->f3= 0;( void*) _temp402;}); if( _temp396->fields
!= 0){ declopt=({ struct Cyc_Core_Opt* _temp403=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp403->v=( void*) _temp364;
_temp403;});} goto _LL385;} _LL389: { struct Cyc_List_List* args=(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Parse_tvar2typ, _temp398->tvs); t=({ struct Cyc_Absyn_EnumType_struct*
_temp404=( struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp404->tag= Cyc_Absyn_EnumType_tag; _temp404->f1=( struct _tuple1*)(( struct
_tuple1*)( _temp398->name)->v); _temp404->f2= args; _temp404->f3= 0;( void*)
_temp404;}); if( _temp398->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp405=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp405->v=( void*) _temp364; _temp405;});} goto _LL385;} _LL391: t=({ struct
Cyc_Absyn_XenumType_struct* _temp406=( struct Cyc_Absyn_XenumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XenumType_struct)); _temp406->tag= Cyc_Absyn_XenumType_tag;
_temp406->f1= _temp400->name; _temp406->f2= 0;( void*) _temp406;}); if( _temp400->fields
!= 0){ declopt=({ struct Cyc_Core_Opt* _temp407=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp407->v=( void*) _temp364;
_temp407;});} goto _LL385; _LL393: t= Cyc_Absyn_UnionType; goto _LL385; _LL395:((
void(*)( struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_string)({ char* _temp408=( char*)"bad declaration within type specifier";
struct _tagged_string _temp409; _temp409.curr= _temp408; _temp409.base= _temp408;
_temp409.last_plus_one= _temp408 + 38; _temp409;}), _temp364->loc); goto _LL385;
_LL385:;}} else{ Cyc_Parse_err( Cyc_Parse_msg3, _temp364->loc);} goto _LL339;
_LL339:;} if( ! seen_type){ if( ! seen_sign? ! seen_size: 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp410=( char*)"missing type withing specifier";
struct _tagged_string _temp411; _temp411.curr= _temp410; _temp411.base= _temp410;
_temp411.last_plus_one= _temp410 + 31; _temp411;}), last_loc);} t=({ struct Cyc_Absyn_IntType_struct*
_temp412=( struct Cyc_Absyn_IntType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp412->tag= Cyc_Absyn_IntType_tag; _temp412->f1=( void*) sgn; _temp412->f2=(
void*) sz;( void*) _temp412;});} else{ if( seen_sign){ void* _temp413= t; void*
_temp419; void* _temp421; _LL415: if(( unsigned int) _temp413 > 5u?(( struct
_enum_struct*) _temp413)->tag == Cyc_Absyn_IntType_tag: 0){ _LL422: _temp421=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp413)->f1; goto _LL420; _LL420:
_temp419=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp413)->f2; goto _LL416;}
else{ goto _LL417;} _LL417: goto _LL418; _LL416: t=({ struct Cyc_Absyn_IntType_struct*
_temp423=( struct Cyc_Absyn_IntType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp423->tag= Cyc_Absyn_IntType_tag; _temp423->f1=( void*) sgn; _temp423->f2=(
void*) _temp419;( void*) _temp423;}); goto _LL414; _LL418: Cyc_Parse_err((
struct _tagged_string)({ char* _temp424=( char*)"sign specification on non-integral type";
struct _tagged_string _temp425; _temp425.curr= _temp424; _temp425.base= _temp424;
_temp425.last_plus_one= _temp424 + 40; _temp425;}), last_loc); goto _LL414;
_LL414:;} if( seen_size){ void* _temp426= t; void* _temp432; void* _temp434;
_LL428: if(( unsigned int) _temp426 > 5u?(( struct _enum_struct*) _temp426)->tag
== Cyc_Absyn_IntType_tag: 0){ _LL435: _temp434=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp426)->f1; goto _LL433; _LL433: _temp432=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp426)->f2; goto _LL429;} else{ goto _LL430;} _LL430: goto _LL431; _LL429: t=({
struct Cyc_Absyn_IntType_struct* _temp436=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp436->tag= Cyc_Absyn_IntType_tag;
_temp436->f1=( void*) _temp434; _temp436->f2=( void*) sz;( void*) _temp436;});
goto _LL427; _LL431: Cyc_Parse_err(( struct _tagged_string)({ char* _temp437=(
char*)"size qualifier on non-integral type"; struct _tagged_string _temp438;
_temp438.curr= _temp437; _temp438.base= _temp437; _temp438.last_plus_one=
_temp437 + 36; _temp438;}), last_loc); goto _LL427; _LL427:;}} return({ struct
_tuple5 _temp439; _temp439.f1= t; _temp439.f2= declopt; _temp439;});} static
struct Cyc_List_List* Cyc_Parse_apply_tmss( struct Cyc_Absyn_Tqual* tq, void* t,
struct Cyc_List_List* ds){ if( ds == 0){ return 0;}{ struct Cyc_Parse_Declarator*
d=( struct Cyc_Parse_Declarator*) ds->hd; struct _tuple1* q= d->id; struct
_tuple6 p= Cyc_Parse_apply_tms( tq, t, d->tms); return({ struct Cyc_List_List*
_temp440=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp440->hd=( void*)({ struct _tuple7* _temp441=( struct _tuple7*) GC_malloc(
sizeof( struct _tuple7)); _temp441->f1= q; _temp441->f2= p.f1; _temp441->f3= p.f2;
_temp441->f4= p.f3; _temp441;}); _temp440->tl= Cyc_Parse_apply_tmss( tq, t, ds->tl);
_temp440;});}} static struct _tuple6 Cyc_Parse_apply_tms( struct Cyc_Absyn_Tqual*
tq, void* t, struct Cyc_List_List* tms){ if( tms == 0){ return({ struct _tuple6
_temp442; _temp442.f1= tq; _temp442.f2= t; _temp442.f3= 0; _temp442;});}{ void*
_temp443=( void*) tms->hd; struct Cyc_Absyn_Exp* _temp455; void* _temp457; int
_temp459; struct Cyc_Position_Segment* _temp461; struct Cyc_List_List* _temp463;
struct Cyc_Absyn_Tqual* _temp465; void* _temp467; void* _temp469; _LL445: if(
_temp443 == Cyc_Absyn_Carray_mod){ goto _LL446;} else{ goto _LL447;} _LL447: if((
unsigned int) _temp443 > 1u?(( struct _enum_struct*) _temp443)->tag == Cyc_Absyn_ConstArray_mod_tag:
0){ _LL456: _temp455=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ConstArray_mod_struct*)
_temp443)->f1; goto _LL448;} else{ goto _LL449;} _LL449: if(( unsigned int)
_temp443 > 1u?(( struct _enum_struct*) _temp443)->tag == Cyc_Absyn_Function_mod_tag:
0){ _LL458: _temp457=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp443)->f1;
goto _LL450;} else{ goto _LL451;} _LL451: if(( unsigned int) _temp443 > 1u?((
struct _enum_struct*) _temp443)->tag == Cyc_Absyn_TypeParams_mod_tag: 0){ _LL464:
_temp463=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp443)->f1; goto _LL462; _LL462: _temp461=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp443)->f2; goto _LL460; _LL460:
_temp459=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp443)->f3; goto
_LL452;} else{ goto _LL453;} _LL453: if(( unsigned int) _temp443 > 1u?(( struct
_enum_struct*) _temp443)->tag == Cyc_Absyn_Pointer_mod_tag: 0){ _LL470: _temp469=(
void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp443)->f1; goto _LL468;
_LL468: _temp467=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp443)->f2;
goto _LL466; _LL466: _temp465=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp443)->f3; goto _LL454;} else{ goto _LL444;} _LL446: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(),({ struct Cyc_Absyn_ArrayType_struct* _temp471=( struct
Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp471->tag= Cyc_Absyn_ArrayType_tag; _temp471->f1=( void*) t; _temp471->f2=
tq; _temp471->f3= Cyc_Absyn_uint_exp( 0, 0);( void*) _temp471;}), tms->tl);
_LL448: return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(),({ struct Cyc_Absyn_ArrayType_struct*
_temp472=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp472->tag= Cyc_Absyn_ArrayType_tag; _temp472->f1=( void*) t; _temp472->f2=
tq; _temp472->f3= _temp455;( void*) _temp472;}), tms->tl); _LL450: { void*
_temp473= _temp457; struct Cyc_Core_Opt* _temp479; int _temp481; struct Cyc_List_List*
_temp483; struct Cyc_Position_Segment* _temp485; struct Cyc_List_List* _temp487;
_LL475: if((( struct _enum_struct*) _temp473)->tag == Cyc_Absyn_WithTypes_tag){
_LL484: _temp483=( struct Cyc_List_List*)(( struct Cyc_Absyn_WithTypes_struct*)
_temp473)->f1; goto _LL482; _LL482: _temp481=( int)(( struct Cyc_Absyn_WithTypes_struct*)
_temp473)->f2; goto _LL480; _LL480: _temp479=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_WithTypes_struct*) _temp473)->f3; goto _LL476;} else{ goto _LL477;}
_LL477: if((( struct _enum_struct*) _temp473)->tag == Cyc_Absyn_NoTypes_tag){
_LL488: _temp487=( struct Cyc_List_List*)(( struct Cyc_Absyn_NoTypes_struct*)
_temp473)->f1; goto _LL486; _LL486: _temp485=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_NoTypes_struct*) _temp473)->f2; goto _LL478;} else{ goto _LL474;}
_LL476: { struct Cyc_List_List* typvars= 0; if( tms->tl != 0){ void* _temp489=(
void*)( tms->tl)->hd; int _temp495; struct Cyc_Position_Segment* _temp497;
struct Cyc_List_List* _temp499; _LL491: if(( unsigned int) _temp489 > 1u?((
struct _enum_struct*) _temp489)->tag == Cyc_Absyn_TypeParams_mod_tag: 0){ _LL500:
_temp499=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp489)->f1; goto _LL498; _LL498: _temp497=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp489)->f2; goto _LL496; _LL496:
_temp495=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp489)->f3; goto
_LL492;} else{ goto _LL493;} _LL493: goto _LL494; _LL492: typvars= _temp499; tms=
tms->tl; goto _LL490; _LL494: goto _LL490; _LL490:;} if(((( ! _temp481? _temp483
!= 0: 0)? _temp483->tl == 0: 0)?(*(( struct _tuple2*) _temp483->hd)).f1 == 0: 0)?(*((
struct _tuple2*) _temp483->hd)).f3 == Cyc_Absyn_VoidType: 0){ _temp483= 0;
_temp481= 0;} t= Cyc_Parse_array2ptr( t);(( void(*)( void(* f)( struct _tuple2*),
struct Cyc_List_List* x)) Cyc_List_iter)( Cyc_Parse_arg_array2ptr, _temp483);
return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(), Cyc_Absyn_function_typ(
typvars, _temp479, t, _temp483, _temp481), tms->tl);} _LL478:( void) _throw(((
struct _xenum_struct*(*)( struct _tagged_string msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)(( struct _tagged_string)({ char* _temp501=( char*)"function declaration without parameter types";
struct _tagged_string _temp502; _temp502.curr= _temp501; _temp502.base= _temp501;
_temp502.last_plus_one= _temp501 + 45; _temp502;}), _temp485)); _LL474:;} _LL452:
if( tms->tl == 0){ return({ struct _tuple6 _temp503; _temp503.f1= tq; _temp503.f2=
t; _temp503.f3= _temp463; _temp503;});}( void) _throw((( struct _xenum_struct*(*)(
struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_string)({ char* _temp504=( char*)"type parameters must appear before function arguments in declarator";
struct _tagged_string _temp505; _temp505.curr= _temp504; _temp505.base= _temp504;
_temp505.last_plus_one= _temp504 + 68; _temp505;}), _temp461)); _LL454: { void*
_temp506= _temp469; struct Cyc_Absyn_Exp* _temp514; struct Cyc_Absyn_Exp*
_temp516; _LL508: if(( unsigned int) _temp506 > 1u?(( struct _enum_struct*)
_temp506)->tag == Cyc_Absyn_NonNullable_ps_tag: 0){ _LL515: _temp514=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp506)->f1; goto
_LL509;} else{ goto _LL510;} _LL510: if(( unsigned int) _temp506 > 1u?(( struct
_enum_struct*) _temp506)->tag == Cyc_Absyn_Nullable_ps_tag: 0){ _LL517: _temp516=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Nullable_ps_struct*) _temp506)->f1;
goto _LL511;} else{ goto _LL512;} _LL512: if( _temp506 == Cyc_Absyn_TaggedArray_ps){
goto _LL513;} else{ goto _LL507;} _LL509: return Cyc_Parse_apply_tms( _temp465,
Cyc_Absyn_atb_typ( t, _temp467, tq,({ struct Cyc_Absyn_Upper_b_struct* _temp518=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp518->tag= Cyc_Absyn_Upper_b_tag; _temp518->f1= _temp514;( void*) _temp518;})),
tms->tl); _LL511: return Cyc_Parse_apply_tms( _temp465, Cyc_Absyn_starb_typ( t,
_temp467, tq,({ struct Cyc_Absyn_Upper_b_struct* _temp519=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp519->tag= Cyc_Absyn_Upper_b_tag;
_temp519->f1= _temp516;( void*) _temp519;})), tms->tl); _LL513: return Cyc_Parse_apply_tms(
_temp465, Cyc_Absyn_tagged_typ( t, _temp467, tq), tms->tl); _LL507:;} _LL444:;}}
static struct Cyc_Absyn_Stmt* Cyc_Parse_flatten_decl( struct Cyc_Absyn_Decl* d,
struct Cyc_Absyn_Stmt* s){ return Cyc_Absyn_new_stmt(({ struct Cyc_Absyn_Decl_s_struct*
_temp520=( struct Cyc_Absyn_Decl_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
_temp520->tag= Cyc_Absyn_Decl_s_tag; _temp520->f1= d; _temp520->f2= s;( void*)
_temp520;}), Cyc_Position_segment_join( d->loc, s->loc));} static struct Cyc_Absyn_Stmt*
Cyc_Parse_flatten_declarations( struct Cyc_List_List* ds, struct Cyc_Absyn_Stmt*
s){ return(( struct Cyc_Absyn_Stmt*(*)( struct Cyc_Absyn_Stmt*(* f)( struct Cyc_Absyn_Decl*,
struct Cyc_Absyn_Stmt*), struct Cyc_List_List* x, struct Cyc_Absyn_Stmt* accum))
Cyc_List_fold_right)( Cyc_Parse_flatten_decl, ds, s);} static struct Cyc_List_List*
Cyc_Parse_make_declarations( struct Cyc_Parse_Declaration_spec* ds, struct Cyc_List_List*
ids, struct Cyc_Position_Segment* loc){ struct Cyc_List_List* tss= ds->type_specs;
struct Cyc_Absyn_Tqual* tq= ds->tq; int istypedef= 0; void* s= Cyc_Absyn_Public;
if( ds->is_inline){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp521=(
char*)"inline is only allowed on function definitions"; struct _tagged_string
_temp522; _temp522.curr= _temp521; _temp522.base= _temp521; _temp522.last_plus_one=
_temp521 + 47; _temp522;}), loc);} if( tss == 0){ Cyc_Parse_err(( struct
_tagged_string)({ char* _temp523=( char*)"missing type specifiers in declaration";
struct _tagged_string _temp524; _temp524.curr= _temp523; _temp524.base= _temp523;
_temp524.last_plus_one= _temp523 + 39; _temp524;}), loc); return 0;} if( ds->sc
!= 0){ void* _temp525=( void*)( ds->sc)->v; _LL527: if( _temp525 == Cyc_Parse_Typedef_sc){
goto _LL528;} else{ goto _LL529;} _LL529: if( _temp525 == Cyc_Parse_Extern_sc){
goto _LL530;} else{ goto _LL531;} _LL531: if( _temp525 == Cyc_Parse_ExternC_sc){
goto _LL532;} else{ goto _LL533;} _LL533: if( _temp525 == Cyc_Parse_Static_sc){
goto _LL534;} else{ goto _LL535;} _LL535: if( _temp525 == Cyc_Parse_Auto_sc){
goto _LL536;} else{ goto _LL537;} _LL537: if( _temp525 == Cyc_Parse_Register_sc){
goto _LL538;} else{ goto _LL539;} _LL539: if( _temp525 == Cyc_Parse_Abstract_sc){
goto _LL540;} else{ goto _LL526;} _LL528: istypedef= 1; goto _LL526; _LL530: s=
Cyc_Absyn_Extern; goto _LL526; _LL532: s= Cyc_Absyn_ExternC; goto _LL526; _LL534:
s= Cyc_Absyn_Static; goto _LL526; _LL536: s= Cyc_Absyn_Public; goto _LL526;
_LL538: s= Cyc_Absyn_Public; goto _LL526; _LL540: s= Cyc_Absyn_Abstract; goto
_LL526; _LL526:;}{ struct Cyc_List_List* _temp543; struct Cyc_List_List*
_temp545; struct _tuple0 _temp541=(( struct _tuple0(*)( struct Cyc_List_List* x))
Cyc_List_split)( ids); _LL546: _temp545= _temp541.f1; goto _LL544; _LL544:
_temp543= _temp541.f2; goto _LL542; _LL542: { int exps_empty= 1;{ struct Cyc_List_List*
es= _temp543; for( 0; es != 0; es= es->tl){ if(( struct Cyc_Absyn_Exp*) es->hd
!= 0){ exps_empty= 0; break;}}}{ struct _tuple5 ts_info= Cyc_Parse_collapse_type_specifiers(
tss, loc); if( _temp545 == 0){ struct Cyc_Core_Opt* _temp549; void* _temp551;
struct _tuple5 _temp547= ts_info; _LL552: _temp551= _temp547.f1; goto _LL550;
_LL550: _temp549= _temp547.f2; goto _LL548; _LL548: if( _temp549 != 0){ struct
Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*) _temp549->v;{ void* _temp553=( void*)
d->r; struct Cyc_Absyn_Structdecl* _temp563; struct Cyc_Absyn_Enumdecl* _temp565;
struct Cyc_Absyn_Xenumdecl* _temp567; _LL555: if(( unsigned int) _temp553 > 1u?((
struct _enum_struct*) _temp553)->tag == Cyc_Absyn_Struct_d_tag: 0){ _LL564:
_temp563=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*)
_temp553)->f1; goto _LL556;} else{ goto _LL557;} _LL557: if(( unsigned int)
_temp553 > 1u?(( struct _enum_struct*) _temp553)->tag == Cyc_Absyn_Enum_d_tag: 0){
_LL566: _temp565=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*)
_temp553)->f1; goto _LL558;} else{ goto _LL559;} _LL559: if(( unsigned int)
_temp553 > 1u?(( struct _enum_struct*) _temp553)->tag == Cyc_Absyn_Xenum_d_tag:
0){ _LL568: _temp567=( struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_d_struct*)
_temp553)->f1; goto _LL560;} else{ goto _LL561;} _LL561: goto _LL562; _LL556:(
void*)( _temp563->sc=( void*) s); goto _LL554; _LL558:( void*)( _temp565->sc=(
void*) s); goto _LL554; _LL560:( void*)( _temp567->sc=( void*) s); goto _LL554;
_LL562: Cyc_Parse_err(( struct _tagged_string)({ char* _temp569=( char*)"bad declaration";
struct _tagged_string _temp570; _temp570.curr= _temp569; _temp570.base= _temp569;
_temp570.last_plus_one= _temp569 + 16; _temp570;}), loc); return 0; _LL554:;}
return({ struct Cyc_List_List* _temp571=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp571->hd=( void*) d; _temp571->tl= 0;
_temp571;});} else{ void* _temp572= _temp551; struct Cyc_Absyn_Structdecl**
_temp584; struct Cyc_List_List* _temp586; struct _tuple1* _temp588; struct Cyc_Absyn_Enumdecl**
_temp590; struct Cyc_List_List* _temp592; struct _tuple1* _temp594; struct Cyc_Absyn_Xenumdecl**
_temp596; struct _tuple1* _temp598; _LL574: if(( unsigned int) _temp572 > 5u?((
struct _enum_struct*) _temp572)->tag == Cyc_Absyn_StructType_tag: 0){ _LL589:
_temp588=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp572)->f1;
goto _LL587; _LL587: _temp586=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp572)->f2; goto _LL585; _LL585: _temp584=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp572)->f3; goto _LL575;} else{ goto
_LL576;} _LL576: if(( unsigned int) _temp572 > 5u?(( struct _enum_struct*)
_temp572)->tag == Cyc_Absyn_EnumType_tag: 0){ _LL595: _temp594=( struct _tuple1*)((
struct Cyc_Absyn_EnumType_struct*) _temp572)->f1; goto _LL593; _LL593: _temp592=(
struct Cyc_List_List*)(( struct Cyc_Absyn_EnumType_struct*) _temp572)->f2; goto
_LL591; _LL591: _temp590=( struct Cyc_Absyn_Enumdecl**)(( struct Cyc_Absyn_EnumType_struct*)
_temp572)->f3; goto _LL577;} else{ goto _LL578;} _LL578: if(( unsigned int)
_temp572 > 5u?(( struct _enum_struct*) _temp572)->tag == Cyc_Absyn_XenumType_tag:
0){ _LL599: _temp598=( struct _tuple1*)(( struct Cyc_Absyn_XenumType_struct*)
_temp572)->f1; goto _LL597; _LL597: _temp596=( struct Cyc_Absyn_Xenumdecl**)((
struct Cyc_Absyn_XenumType_struct*) _temp572)->f2; goto _LL579;} else{ goto
_LL580;} _LL580: if( _temp572 == Cyc_Absyn_UnionType){ goto _LL581;} else{ goto
_LL582;} _LL582: goto _LL583; _LL575: { struct Cyc_List_List* ts2=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc,
_temp586); struct Cyc_Absyn_Structdecl* sd=({ struct Cyc_Absyn_Structdecl*
_temp600=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp600->sc=( void*) s; _temp600->name=({ struct Cyc_Core_Opt* _temp601=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp601->v=(
void*)(( struct _tuple1*)({ struct _tuple1* _temp602= _temp588; if( _temp602 ==
0){ _throw( Null_Exception);} _temp602;})); _temp601;}); _temp600->tvs= ts2;
_temp600->fields= 0; _temp600;}); return({ struct Cyc_List_List* _temp603=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp603->hd=(
void*)({ struct Cyc_Absyn_Decl* _temp604=( struct Cyc_Absyn_Decl*) GC_malloc(
sizeof( struct Cyc_Absyn_Decl)); _temp604->r=( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp605=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp605->tag= Cyc_Absyn_Struct_d_tag; _temp605->f1= sd;( void*) _temp605;});
_temp604->loc= loc; _temp604;}); _temp603->tl= 0; _temp603;});} _LL577: { struct
Cyc_List_List* ts2=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc, _temp592); struct
Cyc_Absyn_Decl* ed= Cyc_Absyn_enum_decl( s,({ struct Cyc_Core_Opt* _temp606=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp606->v=(
void*)(( struct _tuple1*)({ struct _tuple1* _temp607= _temp594; if( _temp607 ==
0){ _throw( Null_Exception);} _temp607;})); _temp606;}), ts2, 0, loc); return({
struct Cyc_List_List* _temp608=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp608->hd=( void*) ed; _temp608->tl= 0; _temp608;});}
_LL579: { struct Cyc_Absyn_Xenumdecl* ed=({ struct Cyc_Absyn_Xenumdecl* _temp609=(
struct Cyc_Absyn_Xenumdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Xenumdecl));
_temp609->sc=( void*) s; _temp609->name= _temp598; _temp609->fields= 0; _temp609;});
return({ struct Cyc_List_List* _temp610=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp610->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp611=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp611->r=( void*)({ struct Cyc_Absyn_Xenum_d_struct* _temp612=( struct Cyc_Absyn_Xenum_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Xenum_d_struct)); _temp612->tag= Cyc_Absyn_Xenum_d_tag;
_temp612->f1= ed;( void*) _temp612;}); _temp611->loc= loc; _temp611;}); _temp610->tl=
0; _temp610;});} _LL581: return({ struct Cyc_List_List* _temp613=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp613->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp614=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp614->r=( void*) Cyc_Absyn_Union_d; _temp614->loc= loc; _temp614;});
_temp613->tl= 0; _temp613;}); _LL583: Cyc_Parse_err(( struct _tagged_string)({
char* _temp615=( char*)"missing declarator"; struct _tagged_string _temp616;
_temp616.curr= _temp615; _temp616.base= _temp615; _temp616.last_plus_one=
_temp615 + 19; _temp616;}), loc); return 0; _LL573:;}} else{ void* t= ts_info.f1;
struct Cyc_List_List* fields= Cyc_Parse_apply_tmss( tq, t, _temp545); if(
istypedef){ if( ! exps_empty){ Cyc_Parse_err(( struct _tagged_string)({ char*
_temp617=( char*)"initializer in typedef declaration"; struct _tagged_string
_temp618; _temp618.curr= _temp617; _temp618.base= _temp617; _temp618.last_plus_one=
_temp617 + 35; _temp618;}), loc);}{ struct Cyc_List_List* decls=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Decl*(* f)( struct Cyc_Position_Segment*, struct _tuple7*),
struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_v_typ_to_typedef,
loc, fields); if( ts_info.f2 != 0){ struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)(
ts_info.f2)->v;{ void* _temp619=( void*) d->r; struct Cyc_Absyn_Structdecl*
_temp629; struct Cyc_Absyn_Enumdecl* _temp631; struct Cyc_Absyn_Xenumdecl*
_temp633; _LL621: if(( unsigned int) _temp619 > 1u?(( struct _enum_struct*)
_temp619)->tag == Cyc_Absyn_Struct_d_tag: 0){ _LL630: _temp629=( struct Cyc_Absyn_Structdecl*)((
struct Cyc_Absyn_Struct_d_struct*) _temp619)->f1; goto _LL622;} else{ goto
_LL623;} _LL623: if(( unsigned int) _temp619 > 1u?(( struct _enum_struct*)
_temp619)->tag == Cyc_Absyn_Enum_d_tag: 0){ _LL632: _temp631=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_d_struct*) _temp619)->f1; goto _LL624;} else{ goto _LL625;}
_LL625: if(( unsigned int) _temp619 > 1u?(( struct _enum_struct*) _temp619)->tag
== Cyc_Absyn_Xenum_d_tag: 0){ _LL634: _temp633=( struct Cyc_Absyn_Xenumdecl*)((
struct Cyc_Absyn_Xenum_d_struct*) _temp619)->f1; goto _LL626;} else{ goto _LL627;}
_LL627: goto _LL628; _LL622:( void*)( _temp629->sc=( void*) s); goto _LL620;
_LL624:( void*)( _temp631->sc=( void*) s); goto _LL620; _LL626:( void*)(
_temp633->sc=( void*) s); goto _LL620; _LL628: Cyc_Parse_err(( struct
_tagged_string)({ char* _temp635=( char*)"declaration within typedef is not a struct, enum, or xenum";
struct _tagged_string _temp636; _temp636.curr= _temp635; _temp636.base= _temp635;
_temp636.last_plus_one= _temp635 + 59; _temp636;}), loc); goto _LL620; _LL620:;}
decls=({ struct Cyc_List_List* _temp637=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp637->hd=( void*) d; _temp637->tl= decls;
_temp637;});} return decls;}} else{ if( ts_info.f2 != 0){ Cyc_Parse_unimp2((
struct _tagged_string)({ char* _temp638=( char*)"nested type declaration within declarator";
struct _tagged_string _temp639; _temp639.curr= _temp638; _temp639.base= _temp638;
_temp639.last_plus_one= _temp638 + 42; _temp639;}), loc);}{ struct Cyc_List_List*
decls= 0;{ struct Cyc_List_List* ds= fields; for( 0; ds != 0; ds= ds->tl,
_temp543= _temp543->tl){ struct _tuple7 _temp642; struct Cyc_List_List* _temp643;
void* _temp645; struct Cyc_Absyn_Tqual* _temp647; struct _tuple1* _temp649;
struct _tuple7* _temp640=( struct _tuple7*) ds->hd; _temp642=* _temp640; _LL650:
_temp649= _temp642.f1; goto _LL648; _LL648: _temp647= _temp642.f2; goto _LL646;
_LL646: _temp645= _temp642.f3; goto _LL644; _LL644: _temp643= _temp642.f4; goto
_LL641; _LL641: if( _temp643 != 0){ Cyc_Parse_warn(( struct _tagged_string)({
char* _temp651=( char*)"bad type params, ignoring"; struct _tagged_string
_temp652; _temp652.curr= _temp651; _temp652.base= _temp651; _temp652.last_plus_one=
_temp651 + 26; _temp652;}), loc);} if( _temp543 == 0){(( void(*)( struct
_tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct
_tagged_string)({ char* _temp653=( char*)"unexpected null in parse!"; struct
_tagged_string _temp654; _temp654.curr= _temp653; _temp654.base= _temp653;
_temp654.last_plus_one= _temp653 + 26; _temp654;}), loc);}{ struct Cyc_Absyn_Exp*
eopt=( struct Cyc_Absyn_Exp*) _temp543->hd; struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_new_vardecl(
_temp649, _temp645, eopt); vd->tq= _temp647;( void*)( vd->sc=( void*) s);{
struct Cyc_Absyn_Decl* d=({ struct Cyc_Absyn_Decl* _temp655=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp655->r=( void*)({ struct Cyc_Absyn_Var_d_struct*
_temp656=( struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp656->tag= Cyc_Absyn_Var_d_tag; _temp656->f1= vd;( void*) _temp656;});
_temp655->loc= loc; _temp655;}); decls=({ struct Cyc_List_List* _temp657=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp657->hd=(
void*) d; _temp657->tl= decls; _temp657;});}}}} return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( decls);}}}}}}} static void* Cyc_Parse_id_to_kind(
struct _tagged_string s, struct Cyc_Position_Segment* loc){ if( Cyc_String_strlen(
s) !=( unsigned int) 1){ Cyc_Parse_err(({ struct _tagged_string _temp658= s;
xprintf("bad kind: %.*s", _temp658.last_plus_one - _temp658.curr, _temp658.curr);}),
loc); return Cyc_Absyn_BoxKind;} else{ switch(({ struct _tagged_string _temp659=
s; char* _temp661= _temp659.curr + 0; if( _temp661 < _temp659.base? 1: _temp661
>= _temp659.last_plus_one){ _throw( Null_Exception);}* _temp661;})){ case 'A':
_LL662: return Cyc_Absyn_AnyKind; case 'M': _LL663: return Cyc_Absyn_MemKind;
case 'B': _LL664: return Cyc_Absyn_BoxKind; case 'R': _LL665: return Cyc_Absyn_RgnKind;
case 'E': _LL666: return Cyc_Absyn_EffKind; default: _LL667: Cyc_Parse_err(({
struct _tagged_string _temp669= s; xprintf("bad kind: %.*s", _temp669.last_plus_one
- _temp669.curr, _temp669.curr);}), loc); return Cyc_Absyn_BoxKind;}}} static
struct Cyc_Absyn_Decl* Cyc_Parse_v_typ_to_typedef( struct Cyc_Position_Segment*
loc, struct _tuple7* t){ struct _tuple1* x=(* t).f1; Cyc_Lex_register_typedef( x);
return Cyc_Absyn_new_decl(({ struct Cyc_Absyn_Typedef_d_struct* _temp670=(
struct Cyc_Absyn_Typedef_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct));
_temp670->tag= Cyc_Absyn_Typedef_d_tag; _temp670->f1=({ struct Cyc_Absyn_Typedefdecl*
_temp671=( struct Cyc_Absyn_Typedefdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Typedefdecl));
_temp671->name= x; _temp671->tvs=(* t).f4; _temp671->defn=( void*)(* t).f3;
_temp671;});( void*) _temp670;}), loc);} typedef struct _xenum_struct* Cyc_YYSTYPE;
char Cyc_Okay_tok_tag[ 9u]="Okay_tok"; struct Cyc_Okay_tok_struct{ char* tag;};
char Cyc_Int_tok_tag[ 8u]="Int_tok"; struct Cyc_Int_tok_struct{ char* tag;
struct _tuple11* f1;}; char Cyc_Char_tok_tag[ 9u]="Char_tok"; struct Cyc_Char_tok_struct{
char* tag; char f1;}; char Cyc_Pointer_Sort_tok_tag[ 17u]="Pointer_Sort_tok";
struct Cyc_Pointer_Sort_tok_struct{ char* tag; void* f1;}; char Cyc_Short_tok_tag[
10u]="Short_tok"; struct Cyc_Short_tok_struct{ char* tag; short f1;}; char Cyc_String_tok_tag[
11u]="String_tok"; struct Cyc_String_tok_struct{ char* tag; struct
_tagged_string f1;}; char Cyc_Stringopt_tok_tag[ 14u]="Stringopt_tok"; struct
Cyc_Stringopt_tok_struct{ char* tag; struct Cyc_Core_Opt* f1;}; char Cyc_Type_tok_tag[
9u]="Type_tok"; struct Cyc_Type_tok_struct{ char* tag; void* f1;}; char Cyc_TypeList_tok_tag[
13u]="TypeList_tok"; struct Cyc_TypeList_tok_struct{ char* tag; struct Cyc_List_List*
f1;}; char Cyc_Exp_tok_tag[ 8u]="Exp_tok"; struct Cyc_Exp_tok_struct{ char* tag;
struct Cyc_Absyn_Exp* f1;}; char Cyc_ExpList_tok_tag[ 12u]="ExpList_tok"; struct
Cyc_ExpList_tok_struct{ char* tag; struct Cyc_List_List* f1;}; char Cyc_Primop_tok_tag[
11u]="Primop_tok"; struct Cyc_Primop_tok_struct{ char* tag; void* f1;}; char Cyc_Primopopt_tok_tag[
14u]="Primopopt_tok"; struct Cyc_Primopopt_tok_struct{ char* tag; struct Cyc_Core_Opt*
f1;}; char Cyc_QualId_tok_tag[ 11u]="QualId_tok"; struct Cyc_QualId_tok_struct{
char* tag; struct _tuple1* f1;}; char Cyc_Stmt_tok_tag[ 9u]="Stmt_tok"; struct
Cyc_Stmt_tok_struct{ char* tag; struct Cyc_Absyn_Stmt* f1;}; char Cyc_SwitchClauseList_tok_tag[
21u]="SwitchClauseList_tok"; struct Cyc_SwitchClauseList_tok_struct{ char* tag;
struct Cyc_List_List* f1;}; char Cyc_Pattern_tok_tag[ 12u]="Pattern_tok"; struct
Cyc_Pattern_tok_struct{ char* tag; struct Cyc_Absyn_Pat* f1;}; char Cyc_PatternList_tok_tag[
16u]="PatternList_tok"; struct Cyc_PatternList_tok_struct{ char* tag; struct Cyc_List_List*
f1;}; char Cyc_FnDecl_tok_tag[ 11u]="FnDecl_tok"; struct Cyc_FnDecl_tok_struct{
char* tag; struct Cyc_Absyn_Fndecl* f1;}; char Cyc_DeclList_tok_tag[ 13u]="DeclList_tok";
struct Cyc_DeclList_tok_struct{ char* tag; struct Cyc_List_List* f1;}; char Cyc_DeclSpec_tok_tag[
13u]="DeclSpec_tok"; struct Cyc_DeclSpec_tok_struct{ char* tag; struct Cyc_Parse_Declaration_spec*
f1;}; char Cyc_InitDecl_tok_tag[ 13u]="InitDecl_tok"; struct Cyc_InitDecl_tok_struct{
char* tag; struct _tuple12* f1;}; char Cyc_InitDeclList_tok_tag[ 17u]="InitDeclList_tok";
struct Cyc_InitDeclList_tok_struct{ char* tag; struct Cyc_List_List* f1;}; char
Cyc_StorageClass_tok_tag[ 17u]="StorageClass_tok"; struct Cyc_StorageClass_tok_struct{
char* tag; void* f1;}; char Cyc_TypeSpecifier_tok_tag[ 18u]="TypeSpecifier_tok";
struct Cyc_TypeSpecifier_tok_struct{ char* tag; void* f1;}; char Cyc_QualSpecList_tok_tag[
17u]="QualSpecList_tok"; struct Cyc_QualSpecList_tok_struct{ char* tag; struct
_tuple13* f1;}; char Cyc_TypeQual_tok_tag[ 13u]="TypeQual_tok"; struct Cyc_TypeQual_tok_struct{
char* tag; struct Cyc_Absyn_Tqual* f1;}; char Cyc_StructFieldDeclList_tok_tag[
24u]="StructFieldDeclList_tok"; struct Cyc_StructFieldDeclList_tok_struct{ char*
tag; struct Cyc_List_List* f1;}; char Cyc_StructFieldDeclListList_tok_tag[ 28u]="StructFieldDeclListList_tok";
struct Cyc_StructFieldDeclListList_tok_struct{ char* tag; struct Cyc_List_List*
f1;}; char Cyc_Declarator_tok_tag[ 15u]="Declarator_tok"; struct Cyc_Declarator_tok_struct{
char* tag; struct Cyc_Parse_Declarator* f1;}; char Cyc_DeclaratorList_tok_tag[
19u]="DeclaratorList_tok"; struct Cyc_DeclaratorList_tok_struct{ char* tag;
struct Cyc_List_List* f1;}; char Cyc_AbstractDeclarator_tok_tag[ 23u]="AbstractDeclarator_tok";
struct Cyc_AbstractDeclarator_tok_struct{ char* tag; struct Cyc_Parse_Abstractdeclarator*
f1;}; char Cyc_EnumeratorField_tok_tag[ 20u]="EnumeratorField_tok"; struct Cyc_EnumeratorField_tok_struct{
char* tag; struct Cyc_Absyn_Enumfield* f1;}; char Cyc_EnumeratorFieldList_tok_tag[
24u]="EnumeratorFieldList_tok"; struct Cyc_EnumeratorFieldList_tok_struct{ char*
tag; struct Cyc_List_List* f1;}; char Cyc_ParamDecl_tok_tag[ 14u]="ParamDecl_tok";
struct Cyc_ParamDecl_tok_struct{ char* tag; struct _tuple2* f1;}; char Cyc_ParamDeclList_tok_tag[
18u]="ParamDeclList_tok"; struct Cyc_ParamDeclList_tok_struct{ char* tag; struct
Cyc_List_List* f1;}; char Cyc_ParamDeclListBool_tok_tag[ 22u]="ParamDeclListBool_tok";
struct Cyc_ParamDeclListBool_tok_struct{ char* tag; struct _tuple14* f1;}; char
Cyc_StructOrUnion_tok_tag[ 18u]="StructOrUnion_tok"; struct Cyc_StructOrUnion_tok_struct{
char* tag; void* f1;}; char Cyc_IdList_tok_tag[ 11u]="IdList_tok"; struct Cyc_IdList_tok_struct{
char* tag; struct Cyc_List_List* f1;}; char Cyc_Designator_tok_tag[ 15u]="Designator_tok";
struct Cyc_Designator_tok_struct{ char* tag; void* f1;}; char Cyc_DesignatorList_tok_tag[
19u]="DesignatorList_tok"; struct Cyc_DesignatorList_tok_struct{ char* tag;
struct Cyc_List_List* f1;}; char Cyc_TypeModifierList_tok_tag[ 21u]="TypeModifierList_tok";
struct Cyc_TypeModifierList_tok_struct{ char* tag; struct Cyc_List_List* f1;};
char Cyc_Rgn_tok_tag[ 8u]="Rgn_tok"; struct Cyc_Rgn_tok_struct{ char* tag; void*
f1;}; char Cyc_InitializerList_tok_tag[ 20u]="InitializerList_tok"; struct Cyc_InitializerList_tok_struct{
char* tag; struct Cyc_List_List* f1;}; char Cyc_FieldPattern_tok_tag[ 17u]="FieldPattern_tok";
struct Cyc_FieldPattern_tok_struct{ char* tag; struct _tuple15* f1;}; char Cyc_FieldPatternList_tok_tag[
21u]="FieldPatternList_tok"; struct Cyc_FieldPatternList_tok_struct{ char* tag;
struct Cyc_List_List* f1;}; char Cyc_BlockItem_tok_tag[ 14u]="BlockItem_tok";
struct Cyc_BlockItem_tok_struct{ char* tag; void* f1;}; char Cyc_Kind_tok_tag[ 9u]="Kind_tok";
struct Cyc_Kind_tok_struct{ char* tag; void* f1;}; struct _tuple11* Cyc_yyget_Int_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp672= yy1; struct _tuple11*
_temp678; _LL674: if((*(( struct _xenum_struct*) _temp672)).tag == Cyc_Int_tok_tag){
_LL679: _temp678=(( struct Cyc_Int_tok_struct*) _temp672)->f1; goto _LL675;}
else{ goto _LL676;} _LL676: goto _LL677; _LL675: return _temp678; _LL677:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp680=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp680=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp681=( char*)"Int_tok";
struct _tagged_string _temp682; _temp682.curr= _temp681; _temp682.base= _temp681;
_temp682.last_plus_one= _temp681 + 8; _temp682;})};( struct _xenum_struct*)
_temp680;})); _LL673:;} struct _tagged_string Cyc_yyget_String_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp683= yy1; struct _tagged_string
_temp689; _LL685: if((*(( struct _xenum_struct*) _temp683)).tag == Cyc_String_tok_tag){
_LL690: _temp689=(( struct Cyc_String_tok_struct*) _temp683)->f1; goto _LL686;}
else{ goto _LL687;} _LL687: goto _LL688; _LL686: return _temp689; _LL688:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp691=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp691=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp692=( char*)"String_tok";
struct _tagged_string _temp693; _temp693.curr= _temp692; _temp693.base= _temp692;
_temp693.last_plus_one= _temp692 + 11; _temp693;})};( struct _xenum_struct*)
_temp691;})); _LL684:;} char Cyc_yyget_Char_tok( struct _xenum_struct* yy1){
struct _xenum_struct* _temp694= yy1; char _temp700; _LL696: if((*(( struct
_xenum_struct*) _temp694)).tag == Cyc_Char_tok_tag){ _LL701: _temp700=(( struct
Cyc_Char_tok_struct*) _temp694)->f1; goto _LL697;} else{ goto _LL698;} _LL698:
goto _LL699; _LL697: return _temp700; _LL699:( void) _throw(({ struct Cyc_Core_Failure_struct*
_temp702=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));*
_temp702=( struct Cyc_Core_Failure_struct){.tag= Cyc_Core_Failure_tag,.f1=(
struct _tagged_string)({ char* _temp703=( char*)"Char_tok"; struct
_tagged_string _temp704; _temp704.curr= _temp703; _temp704.base= _temp703;
_temp704.last_plus_one= _temp703 + 9; _temp704;})};( struct _xenum_struct*)
_temp702;})); _LL695:;} void* Cyc_yyget_Pointer_Sort_tok( struct _xenum_struct*
yy1){ struct _xenum_struct* _temp705= yy1; void* _temp711; _LL707: if((*((
struct _xenum_struct*) _temp705)).tag == Cyc_Pointer_Sort_tok_tag){ _LL712:
_temp711=(( struct Cyc_Pointer_Sort_tok_struct*) _temp705)->f1; goto _LL708;}
else{ goto _LL709;} _LL709: goto _LL710; _LL708: return _temp711; _LL710:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp713=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp713=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp714=( char*)"Pointer_Sort_tok";
struct _tagged_string _temp715; _temp715.curr= _temp714; _temp715.base= _temp714;
_temp715.last_plus_one= _temp714 + 17; _temp715;})};( struct _xenum_struct*)
_temp713;})); _LL706:;} struct Cyc_Absyn_Exp* Cyc_yyget_Exp_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp716= yy1; struct Cyc_Absyn_Exp*
_temp722; _LL718: if((*(( struct _xenum_struct*) _temp716)).tag == Cyc_Exp_tok_tag){
_LL723: _temp722=(( struct Cyc_Exp_tok_struct*) _temp716)->f1; goto _LL719;}
else{ goto _LL720;} _LL720: goto _LL721; _LL719: return _temp722; _LL721:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp724=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp724=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp725=( char*)"Exp_tok";
struct _tagged_string _temp726; _temp726.curr= _temp725; _temp726.base= _temp725;
_temp726.last_plus_one= _temp725 + 8; _temp726;})};( struct _xenum_struct*)
_temp724;})); _LL717:;} struct Cyc_List_List* Cyc_yyget_ExpList_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp727= yy1; struct Cyc_List_List*
_temp733; _LL729: if((*(( struct _xenum_struct*) _temp727)).tag == Cyc_ExpList_tok_tag){
_LL734: _temp733=(( struct Cyc_ExpList_tok_struct*) _temp727)->f1; goto _LL730;}
else{ goto _LL731;} _LL731: goto _LL732; _LL730: return _temp733; _LL732:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp735=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp735=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp736=( char*)"ExpList_tok";
struct _tagged_string _temp737; _temp737.curr= _temp736; _temp737.base= _temp736;
_temp737.last_plus_one= _temp736 + 12; _temp737;})};( struct _xenum_struct*)
_temp735;})); _LL728:;} struct Cyc_List_List* Cyc_yyget_InitializerList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp738= yy1; struct Cyc_List_List*
_temp744; _LL740: if((*(( struct _xenum_struct*) _temp738)).tag == Cyc_InitializerList_tok_tag){
_LL745: _temp744=(( struct Cyc_InitializerList_tok_struct*) _temp738)->f1; goto
_LL741;} else{ goto _LL742;} _LL742: goto _LL743; _LL741: return _temp744;
_LL743:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp746=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp746=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp747=( char*)"InitializerList_tok";
struct _tagged_string _temp748; _temp748.curr= _temp747; _temp748.base= _temp747;
_temp748.last_plus_one= _temp747 + 20; _temp748;})};( struct _xenum_struct*)
_temp746;})); _LL739:;} void* Cyc_yyget_Primop_tok( struct _xenum_struct* yy1){
struct _xenum_struct* _temp749= yy1; void* _temp755; _LL751: if((*(( struct
_xenum_struct*) _temp749)).tag == Cyc_Primop_tok_tag){ _LL756: _temp755=((
struct Cyc_Primop_tok_struct*) _temp749)->f1; goto _LL752;} else{ goto _LL753;}
_LL753: goto _LL754; _LL752: return _temp755; _LL754:( void) _throw(({ struct
Cyc_Core_Failure_struct* _temp757=( struct Cyc_Core_Failure_struct*) GC_malloc(
sizeof( struct Cyc_Core_Failure_struct));* _temp757=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp758=( char*)"Primop_tok";
struct _tagged_string _temp759; _temp759.curr= _temp758; _temp759.base= _temp758;
_temp759.last_plus_one= _temp758 + 11; _temp759;})};( struct _xenum_struct*)
_temp757;})); _LL750:;} struct Cyc_Core_Opt* Cyc_yyget_Primopopt_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp760= yy1; struct Cyc_Core_Opt*
_temp766; _LL762: if((*(( struct _xenum_struct*) _temp760)).tag == Cyc_Primopopt_tok_tag){
_LL767: _temp766=(( struct Cyc_Primopopt_tok_struct*) _temp760)->f1; goto _LL763;}
else{ goto _LL764;} _LL764: goto _LL765; _LL763: return _temp766; _LL765:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp768=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp768=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp769=( char*)"Primopopt_tok";
struct _tagged_string _temp770; _temp770.curr= _temp769; _temp770.base= _temp769;
_temp770.last_plus_one= _temp769 + 14; _temp770;})};( struct _xenum_struct*)
_temp768;})); _LL761:;} struct _tuple1* Cyc_yyget_QualId_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp771= yy1; struct _tuple1*
_temp777; _LL773: if((*(( struct _xenum_struct*) _temp771)).tag == Cyc_QualId_tok_tag){
_LL778: _temp777=(( struct Cyc_QualId_tok_struct*) _temp771)->f1; goto _LL774;}
else{ goto _LL775;} _LL775: goto _LL776; _LL774: return _temp777; _LL776:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp779=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp779=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp780=( char*)"QualId_tok";
struct _tagged_string _temp781; _temp781.curr= _temp780; _temp781.base= _temp780;
_temp781.last_plus_one= _temp780 + 11; _temp781;})};( struct _xenum_struct*)
_temp779;})); _LL772:;} struct Cyc_Absyn_Stmt* Cyc_yyget_Stmt_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp782= yy1; struct Cyc_Absyn_Stmt*
_temp788; _LL784: if((*(( struct _xenum_struct*) _temp782)).tag == Cyc_Stmt_tok_tag){
_LL789: _temp788=(( struct Cyc_Stmt_tok_struct*) _temp782)->f1; goto _LL785;}
else{ goto _LL786;} _LL786: goto _LL787; _LL785: return _temp788; _LL787:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp790=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp790=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp791=( char*)"Stmt_tok";
struct _tagged_string _temp792; _temp792.curr= _temp791; _temp792.base= _temp791;
_temp792.last_plus_one= _temp791 + 9; _temp792;})};( struct _xenum_struct*)
_temp790;})); _LL783:;} void* Cyc_yyget_BlockItem_tok( struct _xenum_struct* yy1){
struct _xenum_struct* _temp793= yy1; void* _temp799; _LL795: if((*(( struct
_xenum_struct*) _temp793)).tag == Cyc_BlockItem_tok_tag){ _LL800: _temp799=((
struct Cyc_BlockItem_tok_struct*) _temp793)->f1; goto _LL796;} else{ goto _LL797;}
_LL797: goto _LL798; _LL796: return _temp799; _LL798:( void) _throw(({ struct
Cyc_Core_Failure_struct* _temp801=( struct Cyc_Core_Failure_struct*) GC_malloc(
sizeof( struct Cyc_Core_Failure_struct));* _temp801=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp802=( char*)"BlockItem_tok";
struct _tagged_string _temp803; _temp803.curr= _temp802; _temp803.base= _temp802;
_temp803.last_plus_one= _temp802 + 14; _temp803;})};( struct _xenum_struct*)
_temp801;})); _LL794:;} struct Cyc_List_List* Cyc_yyget_SwitchClauseList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp804= yy1; struct Cyc_List_List*
_temp810; _LL806: if((*(( struct _xenum_struct*) _temp804)).tag == Cyc_SwitchClauseList_tok_tag){
_LL811: _temp810=(( struct Cyc_SwitchClauseList_tok_struct*) _temp804)->f1; goto
_LL807;} else{ goto _LL808;} _LL808: goto _LL809; _LL807: return _temp810;
_LL809:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp812=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp812=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp813=( char*)"SwitchClauseList_tok";
struct _tagged_string _temp814; _temp814.curr= _temp813; _temp814.base= _temp813;
_temp814.last_plus_one= _temp813 + 21; _temp814;})};( struct _xenum_struct*)
_temp812;})); _LL805:;} struct Cyc_Absyn_Pat* Cyc_yyget_Pattern_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp815= yy1; struct Cyc_Absyn_Pat*
_temp821; _LL817: if((*(( struct _xenum_struct*) _temp815)).tag == Cyc_Pattern_tok_tag){
_LL822: _temp821=(( struct Cyc_Pattern_tok_struct*) _temp815)->f1; goto _LL818;}
else{ goto _LL819;} _LL819: goto _LL820; _LL818: return _temp821; _LL820:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp823=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp823=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp824=( char*)"Pattern_tok";
struct _tagged_string _temp825; _temp825.curr= _temp824; _temp825.base= _temp824;
_temp825.last_plus_one= _temp824 + 12; _temp825;})};( struct _xenum_struct*)
_temp823;})); _LL816:;} struct Cyc_List_List* Cyc_yyget_PatternList_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp826= yy1; struct Cyc_List_List*
_temp832; _LL828: if((*(( struct _xenum_struct*) _temp826)).tag == Cyc_PatternList_tok_tag){
_LL833: _temp832=(( struct Cyc_PatternList_tok_struct*) _temp826)->f1; goto
_LL829;} else{ goto _LL830;} _LL830: goto _LL831; _LL829: return _temp832;
_LL831:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp834=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp834=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp835=( char*)"PatternList_tok";
struct _tagged_string _temp836; _temp836.curr= _temp835; _temp836.base= _temp835;
_temp836.last_plus_one= _temp835 + 16; _temp836;})};( struct _xenum_struct*)
_temp834;})); _LL827:;} struct _tuple15* Cyc_yyget_FieldPattern_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp837= yy1; struct _tuple15*
_temp843; _LL839: if((*(( struct _xenum_struct*) _temp837)).tag == Cyc_FieldPattern_tok_tag){
_LL844: _temp843=(( struct Cyc_FieldPattern_tok_struct*) _temp837)->f1; goto
_LL840;} else{ goto _LL841;} _LL841: goto _LL842; _LL840: return _temp843;
_LL842:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp845=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp845=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp846=( char*)"FieldPattern_tok";
struct _tagged_string _temp847; _temp847.curr= _temp846; _temp847.base= _temp846;
_temp847.last_plus_one= _temp846 + 17; _temp847;})};( struct _xenum_struct*)
_temp845;})); _LL838:;} struct Cyc_List_List* Cyc_yyget_FieldPatternList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp848= yy1; struct Cyc_List_List*
_temp854; _LL850: if((*(( struct _xenum_struct*) _temp848)).tag == Cyc_FieldPatternList_tok_tag){
_LL855: _temp854=(( struct Cyc_FieldPatternList_tok_struct*) _temp848)->f1; goto
_LL851;} else{ goto _LL852;} _LL852: goto _LL853; _LL851: return _temp854;
_LL853:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp856=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp856=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp857=( char*)"FieldPatternList_tok";
struct _tagged_string _temp858; _temp858.curr= _temp857; _temp858.base= _temp857;
_temp858.last_plus_one= _temp857 + 21; _temp858;})};( struct _xenum_struct*)
_temp856;})); _LL849:;} struct Cyc_Absyn_Fndecl* Cyc_yyget_FnDecl_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp859= yy1; struct Cyc_Absyn_Fndecl*
_temp865; _LL861: if((*(( struct _xenum_struct*) _temp859)).tag == Cyc_FnDecl_tok_tag){
_LL866: _temp865=(( struct Cyc_FnDecl_tok_struct*) _temp859)->f1; goto _LL862;}
else{ goto _LL863;} _LL863: goto _LL864; _LL862: return _temp865; _LL864:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp867=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp867=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp868=( char*)"FnDecl_tok";
struct _tagged_string _temp869; _temp869.curr= _temp868; _temp869.base= _temp868;
_temp869.last_plus_one= _temp868 + 11; _temp869;})};( struct _xenum_struct*)
_temp867;})); _LL860:;} struct Cyc_List_List* Cyc_yyget_DeclList_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp870= yy1; struct Cyc_List_List*
_temp876; _LL872: if((*(( struct _xenum_struct*) _temp870)).tag == Cyc_DeclList_tok_tag){
_LL877: _temp876=(( struct Cyc_DeclList_tok_struct*) _temp870)->f1; goto _LL873;}
else{ goto _LL874;} _LL874: goto _LL875; _LL873: return _temp876; _LL875:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp878=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp878=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp879=( char*)"DeclList_tok";
struct _tagged_string _temp880; _temp880.curr= _temp879; _temp880.base= _temp879;
_temp880.last_plus_one= _temp879 + 13; _temp880;})};( struct _xenum_struct*)
_temp878;})); _LL871:;} struct Cyc_Parse_Declaration_spec* Cyc_yyget_DeclSpec_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp881= yy1; struct Cyc_Parse_Declaration_spec*
_temp887; _LL883: if((*(( struct _xenum_struct*) _temp881)).tag == Cyc_DeclSpec_tok_tag){
_LL888: _temp887=(( struct Cyc_DeclSpec_tok_struct*) _temp881)->f1; goto _LL884;}
else{ goto _LL885;} _LL885: goto _LL886; _LL884: return _temp887; _LL886:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp889=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp889=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp890=( char*)"DeclSpec_tok";
struct _tagged_string _temp891; _temp891.curr= _temp890; _temp891.base= _temp890;
_temp891.last_plus_one= _temp890 + 13; _temp891;})};( struct _xenum_struct*)
_temp889;})); _LL882:;} struct _tuple12* Cyc_yyget_InitDecl_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp892= yy1; struct _tuple12*
_temp898; _LL894: if((*(( struct _xenum_struct*) _temp892)).tag == Cyc_InitDecl_tok_tag){
_LL899: _temp898=(( struct Cyc_InitDecl_tok_struct*) _temp892)->f1; goto _LL895;}
else{ goto _LL896;} _LL896: goto _LL897; _LL895: return _temp898; _LL897:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp900=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp900=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp901=( char*)"InitDecl_tok";
struct _tagged_string _temp902; _temp902.curr= _temp901; _temp902.base= _temp901;
_temp902.last_plus_one= _temp901 + 13; _temp902;})};( struct _xenum_struct*)
_temp900;})); _LL893:;} struct Cyc_List_List* Cyc_yyget_InitDeclList_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp903= yy1; struct Cyc_List_List*
_temp909; _LL905: if((*(( struct _xenum_struct*) _temp903)).tag == Cyc_InitDeclList_tok_tag){
_LL910: _temp909=(( struct Cyc_InitDeclList_tok_struct*) _temp903)->f1; goto
_LL906;} else{ goto _LL907;} _LL907: goto _LL908; _LL906: return _temp909;
_LL908:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp911=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp911=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp912=( char*)"InitDeclList_tok";
struct _tagged_string _temp913; _temp913.curr= _temp912; _temp913.base= _temp912;
_temp913.last_plus_one= _temp912 + 17; _temp913;})};( struct _xenum_struct*)
_temp911;})); _LL904:;} void* Cyc_yyget_StorageClass_tok( struct _xenum_struct*
yy1){ struct _xenum_struct* _temp914= yy1; void* _temp920; _LL916: if((*((
struct _xenum_struct*) _temp914)).tag == Cyc_StorageClass_tok_tag){ _LL921:
_temp920=(( struct Cyc_StorageClass_tok_struct*) _temp914)->f1; goto _LL917;}
else{ goto _LL918;} _LL918: goto _LL919; _LL917: return _temp920; _LL919:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp922=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp922=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp923=( char*)"StorageClass_tok";
struct _tagged_string _temp924; _temp924.curr= _temp923; _temp924.base= _temp923;
_temp924.last_plus_one= _temp923 + 17; _temp924;})};( struct _xenum_struct*)
_temp922;})); _LL915:;} void* Cyc_yyget_TypeSpecifier_tok( struct _xenum_struct*
yy1){ struct _xenum_struct* _temp925= yy1; void* _temp931; _LL927: if((*((
struct _xenum_struct*) _temp925)).tag == Cyc_TypeSpecifier_tok_tag){ _LL932:
_temp931=(( struct Cyc_TypeSpecifier_tok_struct*) _temp925)->f1; goto _LL928;}
else{ goto _LL929;} _LL929: goto _LL930; _LL928: return _temp931; _LL930:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp933=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp933=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp934=( char*)"TypeSpecifier_tok";
struct _tagged_string _temp935; _temp935.curr= _temp934; _temp935.base= _temp934;
_temp935.last_plus_one= _temp934 + 18; _temp935;})};( struct _xenum_struct*)
_temp933;})); _LL926:;} void* Cyc_yyget_StructOrUnion_tok( struct _xenum_struct*
yy1){ struct _xenum_struct* _temp936= yy1; void* _temp942; _LL938: if((*((
struct _xenum_struct*) _temp936)).tag == Cyc_StructOrUnion_tok_tag){ _LL943:
_temp942=(( struct Cyc_StructOrUnion_tok_struct*) _temp936)->f1; goto _LL939;}
else{ goto _LL940;} _LL940: goto _LL941; _LL939: return _temp942; _LL941:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp944=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp944=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp945=( char*)"StructOrUnion_tok";
struct _tagged_string _temp946; _temp946.curr= _temp945; _temp946.base= _temp945;
_temp946.last_plus_one= _temp945 + 18; _temp946;})};( struct _xenum_struct*)
_temp944;})); _LL937:;} struct Cyc_Absyn_Tqual* Cyc_yyget_TypeQual_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp947= yy1; struct Cyc_Absyn_Tqual*
_temp953; _LL949: if((*(( struct _xenum_struct*) _temp947)).tag == Cyc_TypeQual_tok_tag){
_LL954: _temp953=(( struct Cyc_TypeQual_tok_struct*) _temp947)->f1; goto _LL950;}
else{ goto _LL951;} _LL951: goto _LL952; _LL950: return _temp953; _LL952:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp955=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp955=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp956=( char*)"TypeQual_tok";
struct _tagged_string _temp957; _temp957.curr= _temp956; _temp957.base= _temp956;
_temp957.last_plus_one= _temp956 + 13; _temp957;})};( struct _xenum_struct*)
_temp955;})); _LL948:;} struct Cyc_List_List* Cyc_yyget_StructFieldDeclList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp958= yy1; struct Cyc_List_List*
_temp964; _LL960: if((*(( struct _xenum_struct*) _temp958)).tag == Cyc_StructFieldDeclList_tok_tag){
_LL965: _temp964=(( struct Cyc_StructFieldDeclList_tok_struct*) _temp958)->f1;
goto _LL961;} else{ goto _LL962;} _LL962: goto _LL963; _LL961: return _temp964;
_LL963:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp966=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp966=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp967=( char*)"StructFieldDeclList_tok";
struct _tagged_string _temp968; _temp968.curr= _temp967; _temp968.base= _temp967;
_temp968.last_plus_one= _temp967 + 24; _temp968;})};( struct _xenum_struct*)
_temp966;})); _LL959:;} struct Cyc_List_List* Cyc_yyget_StructFieldDeclListList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp969= yy1; struct Cyc_List_List*
_temp975; _LL971: if((*(( struct _xenum_struct*) _temp969)).tag == Cyc_StructFieldDeclListList_tok_tag){
_LL976: _temp975=(( struct Cyc_StructFieldDeclListList_tok_struct*) _temp969)->f1;
goto _LL972;} else{ goto _LL973;} _LL973: goto _LL974; _LL972: return _temp975;
_LL974:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp977=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp977=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp978=( char*)"StructFieldDeclListList_tok";
struct _tagged_string _temp979; _temp979.curr= _temp978; _temp979.base= _temp978;
_temp979.last_plus_one= _temp978 + 28; _temp979;})};( struct _xenum_struct*)
_temp977;})); _LL970:;} struct Cyc_List_List* Cyc_yyget_TypeModifierList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp980= yy1; struct Cyc_List_List*
_temp986; _LL982: if((*(( struct _xenum_struct*) _temp980)).tag == Cyc_TypeModifierList_tok_tag){
_LL987: _temp986=(( struct Cyc_TypeModifierList_tok_struct*) _temp980)->f1; goto
_LL983;} else{ goto _LL984;} _LL984: goto _LL985; _LL983: return _temp986;
_LL985:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp988=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp988=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp989=( char*)"TypeModifierList_tok";
struct _tagged_string _temp990; _temp990.curr= _temp989; _temp990.base= _temp989;
_temp990.last_plus_one= _temp989 + 21; _temp990;})};( struct _xenum_struct*)
_temp988;})); _LL981:;} void* Cyc_yyget_Rgn_tok( struct _xenum_struct* yy1){
struct _xenum_struct* _temp991= yy1; void* _temp997; _LL993: if((*(( struct
_xenum_struct*) _temp991)).tag == Cyc_Rgn_tok_tag){ _LL998: _temp997=(( struct
Cyc_Rgn_tok_struct*) _temp991)->f1; goto _LL994;} else{ goto _LL995;} _LL995:
goto _LL996; _LL994: return _temp997; _LL996:( void) _throw(({ struct Cyc_Core_Failure_struct*
_temp999=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));*
_temp999=( struct Cyc_Core_Failure_struct){.tag= Cyc_Core_Failure_tag,.f1=(
struct _tagged_string)({ char* _temp1000=( char*)"Rgn_tok"; struct
_tagged_string _temp1001; _temp1001.curr= _temp1000; _temp1001.base= _temp1000;
_temp1001.last_plus_one= _temp1000 + 8; _temp1001;})};( struct _xenum_struct*)
_temp999;})); _LL992:;} struct Cyc_Parse_Declarator* Cyc_yyget_Declarator_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1002= yy1; struct Cyc_Parse_Declarator*
_temp1008; _LL1004: if((*(( struct _xenum_struct*) _temp1002)).tag == Cyc_Declarator_tok_tag){
_LL1009: _temp1008=(( struct Cyc_Declarator_tok_struct*) _temp1002)->f1; goto
_LL1005;} else{ goto _LL1006;} _LL1006: goto _LL1007; _LL1005: return _temp1008;
_LL1007:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1010=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp1010=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp1011=( char*)"Declarator_tok";
struct _tagged_string _temp1012; _temp1012.curr= _temp1011; _temp1012.base=
_temp1011; _temp1012.last_plus_one= _temp1011 + 15; _temp1012;})};( struct
_xenum_struct*) _temp1010;})); _LL1003:;} struct Cyc_List_List* Cyc_yyget_DeclaratorList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1013= yy1; struct Cyc_List_List*
_temp1019; _LL1015: if((*(( struct _xenum_struct*) _temp1013)).tag == Cyc_DeclaratorList_tok_tag){
_LL1020: _temp1019=(( struct Cyc_DeclaratorList_tok_struct*) _temp1013)->f1;
goto _LL1016;} else{ goto _LL1017;} _LL1017: goto _LL1018; _LL1016: return
_temp1019; _LL1018:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1021=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));*
_temp1021=( struct Cyc_Core_Failure_struct){.tag= Cyc_Core_Failure_tag,.f1=(
struct _tagged_string)({ char* _temp1022=( char*)"DeclaratorList_tok"; struct
_tagged_string _temp1023; _temp1023.curr= _temp1022; _temp1023.base= _temp1022;
_temp1023.last_plus_one= _temp1022 + 19; _temp1023;})};( struct _xenum_struct*)
_temp1021;})); _LL1014:;} struct Cyc_Parse_Abstractdeclarator* Cyc_yyget_AbstractDeclarator_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1024= yy1; struct Cyc_Parse_Abstractdeclarator*
_temp1030; _LL1026: if((*(( struct _xenum_struct*) _temp1024)).tag == Cyc_AbstractDeclarator_tok_tag){
_LL1031: _temp1030=(( struct Cyc_AbstractDeclarator_tok_struct*) _temp1024)->f1;
goto _LL1027;} else{ goto _LL1028;} _LL1028: goto _LL1029; _LL1027: return
_temp1030; _LL1029:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1032=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));*
_temp1032=( struct Cyc_Core_Failure_struct){.tag= Cyc_Core_Failure_tag,.f1=(
struct _tagged_string)({ char* _temp1033=( char*)"AbstractDeclarator_tok";
struct _tagged_string _temp1034; _temp1034.curr= _temp1033; _temp1034.base=
_temp1033; _temp1034.last_plus_one= _temp1033 + 23; _temp1034;})};( struct
_xenum_struct*) _temp1032;})); _LL1025:;} struct Cyc_Absyn_Enumfield* Cyc_yyget_EnumeratorField_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1035= yy1; struct Cyc_Absyn_Enumfield*
_temp1041; _LL1037: if((*(( struct _xenum_struct*) _temp1035)).tag == Cyc_EnumeratorField_tok_tag){
_LL1042: _temp1041=(( struct Cyc_EnumeratorField_tok_struct*) _temp1035)->f1;
goto _LL1038;} else{ goto _LL1039;} _LL1039: goto _LL1040; _LL1038: return
_temp1041; _LL1040:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1043=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));*
_temp1043=( struct Cyc_Core_Failure_struct){.tag= Cyc_Core_Failure_tag,.f1=(
struct _tagged_string)({ char* _temp1044=( char*)"EnumeratorField_tok"; struct
_tagged_string _temp1045; _temp1045.curr= _temp1044; _temp1045.base= _temp1044;
_temp1045.last_plus_one= _temp1044 + 20; _temp1045;})};( struct _xenum_struct*)
_temp1043;})); _LL1036:;} struct Cyc_List_List* Cyc_yyget_EnumeratorFieldList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1046= yy1; struct Cyc_List_List*
_temp1052; _LL1048: if((*(( struct _xenum_struct*) _temp1046)).tag == Cyc_EnumeratorFieldList_tok_tag){
_LL1053: _temp1052=(( struct Cyc_EnumeratorFieldList_tok_struct*) _temp1046)->f1;
goto _LL1049;} else{ goto _LL1050;} _LL1050: goto _LL1051; _LL1049: return
_temp1052; _LL1051:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1054=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));*
_temp1054=( struct Cyc_Core_Failure_struct){.tag= Cyc_Core_Failure_tag,.f1=(
struct _tagged_string)({ char* _temp1055=( char*)"EnumeratorFieldList_tok";
struct _tagged_string _temp1056; _temp1056.curr= _temp1055; _temp1056.base=
_temp1055; _temp1056.last_plus_one= _temp1055 + 24; _temp1056;})};( struct
_xenum_struct*) _temp1054;})); _LL1047:;} struct _tuple13* Cyc_yyget_QualSpecList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1057= yy1; struct
_tuple13* _temp1063; _LL1059: if((*(( struct _xenum_struct*) _temp1057)).tag ==
Cyc_QualSpecList_tok_tag){ _LL1064: _temp1063=(( struct Cyc_QualSpecList_tok_struct*)
_temp1057)->f1; goto _LL1060;} else{ goto _LL1061;} _LL1061: goto _LL1062;
_LL1060: return _temp1063; _LL1062:( void) _throw(({ struct Cyc_Core_Failure_struct*
_temp1065=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));*
_temp1065=( struct Cyc_Core_Failure_struct){.tag= Cyc_Core_Failure_tag,.f1=(
struct _tagged_string)({ char* _temp1066=( char*)"QualSpecList_tok"; struct
_tagged_string _temp1067; _temp1067.curr= _temp1066; _temp1067.base= _temp1066;
_temp1067.last_plus_one= _temp1066 + 17; _temp1067;})};( struct _xenum_struct*)
_temp1065;})); _LL1058:;} struct Cyc_List_List* Cyc_yyget_IdList_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp1068= yy1; struct Cyc_List_List*
_temp1074; _LL1070: if((*(( struct _xenum_struct*) _temp1068)).tag == Cyc_IdList_tok_tag){
_LL1075: _temp1074=(( struct Cyc_IdList_tok_struct*) _temp1068)->f1; goto
_LL1071;} else{ goto _LL1072;} _LL1072: goto _LL1073; _LL1071: return _temp1074;
_LL1073:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1076=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp1076=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp1077=( char*)"IdList_tok";
struct _tagged_string _temp1078; _temp1078.curr= _temp1077; _temp1078.base=
_temp1077; _temp1078.last_plus_one= _temp1077 + 11; _temp1078;})};( struct
_xenum_struct*) _temp1076;})); _LL1069:;} struct _tuple2* Cyc_yyget_ParamDecl_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1079= yy1; struct _tuple2*
_temp1085; _LL1081: if((*(( struct _xenum_struct*) _temp1079)).tag == Cyc_ParamDecl_tok_tag){
_LL1086: _temp1085=(( struct Cyc_ParamDecl_tok_struct*) _temp1079)->f1; goto
_LL1082;} else{ goto _LL1083;} _LL1083: goto _LL1084; _LL1082: return _temp1085;
_LL1084:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1087=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp1087=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp1088=( char*)"ParamDecl_tok";
struct _tagged_string _temp1089; _temp1089.curr= _temp1088; _temp1089.base=
_temp1088; _temp1089.last_plus_one= _temp1088 + 14; _temp1089;})};( struct
_xenum_struct*) _temp1087;})); _LL1080:;} struct Cyc_List_List* Cyc_yyget_ParamDeclList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1090= yy1; struct Cyc_List_List*
_temp1096; _LL1092: if((*(( struct _xenum_struct*) _temp1090)).tag == Cyc_ParamDeclList_tok_tag){
_LL1097: _temp1096=(( struct Cyc_ParamDeclList_tok_struct*) _temp1090)->f1; goto
_LL1093;} else{ goto _LL1094;} _LL1094: goto _LL1095; _LL1093: return _temp1096;
_LL1095:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1098=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp1098=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp1099=( char*)"ParamDeclList_tok";
struct _tagged_string _temp1100; _temp1100.curr= _temp1099; _temp1100.base=
_temp1099; _temp1100.last_plus_one= _temp1099 + 18; _temp1100;})};( struct
_xenum_struct*) _temp1098;})); _LL1091:;} struct _tuple14* Cyc_yyget_ParamDeclListBool_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1101= yy1; struct
_tuple14* _temp1107; _LL1103: if((*(( struct _xenum_struct*) _temp1101)).tag ==
Cyc_ParamDeclListBool_tok_tag){ _LL1108: _temp1107=(( struct Cyc_ParamDeclListBool_tok_struct*)
_temp1101)->f1; goto _LL1104;} else{ goto _LL1105;} _LL1105: goto _LL1106;
_LL1104: return _temp1107; _LL1106:( void) _throw(({ struct Cyc_Core_Failure_struct*
_temp1109=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));*
_temp1109=( struct Cyc_Core_Failure_struct){.tag= Cyc_Core_Failure_tag,.f1=(
struct _tagged_string)({ char* _temp1110=( char*)"ParamDeclListBool_tok"; struct
_tagged_string _temp1111; _temp1111.curr= _temp1110; _temp1111.base= _temp1110;
_temp1111.last_plus_one= _temp1110 + 22; _temp1111;})};( struct _xenum_struct*)
_temp1109;})); _LL1102:;} struct Cyc_List_List* Cyc_yyget_TypeList_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp1112= yy1; struct Cyc_List_List*
_temp1118; _LL1114: if((*(( struct _xenum_struct*) _temp1112)).tag == Cyc_TypeList_tok_tag){
_LL1119: _temp1118=(( struct Cyc_TypeList_tok_struct*) _temp1112)->f1; goto
_LL1115;} else{ goto _LL1116;} _LL1116: goto _LL1117; _LL1115: return _temp1118;
_LL1117:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1120=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp1120=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp1121=( char*)"TypeList_tok";
struct _tagged_string _temp1122; _temp1122.curr= _temp1121; _temp1122.base=
_temp1121; _temp1122.last_plus_one= _temp1121 + 13; _temp1122;})};( struct
_xenum_struct*) _temp1120;})); _LL1113:;} struct Cyc_List_List* Cyc_yyget_DesignatorList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1123= yy1; struct Cyc_List_List*
_temp1129; _LL1125: if((*(( struct _xenum_struct*) _temp1123)).tag == Cyc_DesignatorList_tok_tag){
_LL1130: _temp1129=(( struct Cyc_DesignatorList_tok_struct*) _temp1123)->f1;
goto _LL1126;} else{ goto _LL1127;} _LL1127: goto _LL1128; _LL1126: return
_temp1129; _LL1128:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1131=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));*
_temp1131=( struct Cyc_Core_Failure_struct){.tag= Cyc_Core_Failure_tag,.f1=(
struct _tagged_string)({ char* _temp1132=( char*)"DesignatorList_tok"; struct
_tagged_string _temp1133; _temp1133.curr= _temp1132; _temp1133.base= _temp1132;
_temp1133.last_plus_one= _temp1132 + 19; _temp1133;})};( struct _xenum_struct*)
_temp1131;})); _LL1124:;} void* Cyc_yyget_Designator_tok( struct _xenum_struct*
yy1){ struct _xenum_struct* _temp1134= yy1; void* _temp1140; _LL1136: if((*((
struct _xenum_struct*) _temp1134)).tag == Cyc_Designator_tok_tag){ _LL1141:
_temp1140=(( struct Cyc_Designator_tok_struct*) _temp1134)->f1; goto _LL1137;}
else{ goto _LL1138;} _LL1138: goto _LL1139; _LL1137: return _temp1140; _LL1139:(
void) _throw(({ struct Cyc_Core_Failure_struct* _temp1142=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp1142=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp1143=( char*)"Designator_tok";
struct _tagged_string _temp1144; _temp1144.curr= _temp1143; _temp1144.base=
_temp1143; _temp1144.last_plus_one= _temp1143 + 15; _temp1144;})};( struct
_xenum_struct*) _temp1142;})); _LL1135:;} void* Cyc_yyget_Kind_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp1145= yy1; void* _temp1151;
_LL1147: if((*(( struct _xenum_struct*) _temp1145)).tag == Cyc_Kind_tok_tag){
_LL1152: _temp1151=(( struct Cyc_Kind_tok_struct*) _temp1145)->f1; goto _LL1148;}
else{ goto _LL1149;} _LL1149: goto _LL1150; _LL1148: return _temp1151; _LL1150:(
void) _throw(({ struct Cyc_Core_Failure_struct* _temp1153=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp1153=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp1154=( char*)"Kind_tok";
struct _tagged_string _temp1155; _temp1155.curr= _temp1154; _temp1155.base=
_temp1154; _temp1155.last_plus_one= _temp1154 + 9; _temp1155;})};( struct
_xenum_struct*) _temp1153;})); _LL1146:;} void* Cyc_yyget_Type_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp1156= yy1; void* _temp1162;
_LL1158: if((*(( struct _xenum_struct*) _temp1156)).tag == Cyc_Type_tok_tag){
_LL1163: _temp1162=(( struct Cyc_Type_tok_struct*) _temp1156)->f1; goto _LL1159;}
else{ goto _LL1160;} _LL1160: goto _LL1161; _LL1159: return _temp1162; _LL1161:(
void) _throw(({ struct Cyc_Core_Failure_struct* _temp1164=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp1164=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp1165=( char*)"Type_tok";
struct _tagged_string _temp1166; _temp1166.curr= _temp1165; _temp1166.base=
_temp1165; _temp1166.last_plus_one= _temp1165 + 9; _temp1166;})};( struct
_xenum_struct*) _temp1164;})); _LL1157:;} struct Cyc_Yyltype{ int timestamp; int
first_line; int first_column; int last_line; int last_column; struct
_tagged_string text;}; typedef struct Cyc_Yyltype Cyc_yyltype; struct Cyc_Yyltype
Cyc_yynewloc(){ return({ struct Cyc_Yyltype _temp1167; _temp1167.timestamp= 0;
_temp1167.first_line= 0; _temp1167.first_column= 0; _temp1167.last_line= 0;
_temp1167.last_column= 0; _temp1167.text=( struct _tagged_string)({ char*
_temp1168=( char*)""; struct _tagged_string _temp1169; _temp1169.curr= _temp1168;
_temp1169.base= _temp1168; _temp1169.last_plus_one= _temp1168 + 1; _temp1169;});
_temp1167;});} static char _temp1172[ 1u]=""; struct Cyc_Yyltype Cyc_yylloc=(
struct Cyc_Yyltype){.timestamp= 0,.first_line= 0,.first_column= 0,.last_line= 0,.last_column=
0,.text=( struct _tagged_string){ _temp1172, _temp1172, _temp1172 + 1u}}; static
short Cyc_yytranslate[ 345u]={ (short)0, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)116, (short)2, (short)2, (short)95,
(short)114, (short)110, (short)2, (short)96, (short)97, (short)104, (short)107,
(short)100, (short)109, (short)108, (short)113, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)101,
(short)90, (short)98, (short)93, (short)99, (short)106, (short)105, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)102, (short)2, (short)103, (short)112, (short)94, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)91, (short)111, (short)92, (short)115, (short)2,
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
(short)86, (short)87, (short)88, (short)89}; static char _temp1175[ 2u]="$";
static char _temp1178[ 6u]="error"; static char _temp1181[ 12u]="$undefined.";
static char _temp1184[ 5u]="AUTO"; static char _temp1187[ 9u]="REGISTER"; static
char _temp1190[ 7u]="STATIC"; static char _temp1193[ 7u]="EXTERN"; static char
_temp1196[ 8u]="TYPEDEF"; static char _temp1199[ 5u]="VOID"; static char
_temp1202[ 5u]="CHAR"; static char _temp1205[ 6u]="SHORT"; static char _temp1208[
4u]="INT"; static char _temp1211[ 5u]="LONG"; static char _temp1214[ 6u]="FLOAT";
static char _temp1217[ 7u]="DOUBLE"; static char _temp1220[ 7u]="SIGNED"; static
char _temp1223[ 9u]="UNSIGNED"; static char _temp1226[ 6u]="CONST"; static char
_temp1229[ 9u]="VOLATILE"; static char _temp1232[ 9u]="RESTRICT"; static char
_temp1235[ 7u]="STRUCT"; static char _temp1238[ 6u]="UNION"; static char
_temp1241[ 5u]="CASE"; static char _temp1244[ 8u]="DEFAULT"; static char
_temp1247[ 7u]="INLINE"; static char _temp1250[ 3u]="IF"; static char _temp1253[
5u]="ELSE"; static char _temp1256[ 7u]="SWITCH"; static char _temp1259[ 6u]="WHILE";
static char _temp1262[ 3u]="DO"; static char _temp1265[ 4u]="FOR"; static char
_temp1268[ 5u]="GOTO"; static char _temp1271[ 9u]="CONTINUE"; static char
_temp1274[ 6u]="BREAK"; static char _temp1277[ 7u]="RETURN"; static char
_temp1280[ 7u]="SIZEOF"; static char _temp1283[ 5u]="ENUM"; static char
_temp1286[ 8u]="NULL_kw"; static char _temp1289[ 4u]="LET"; static char
_temp1292[ 6u]="THROW"; static char _temp1295[ 4u]="TRY"; static char _temp1298[
6u]="CATCH"; static char _temp1301[ 4u]="NEW"; static char _temp1304[ 9u]="ABSTRACT";
static char _temp1307[ 9u]="FALLTHRU"; static char _temp1310[ 6u]="USING";
static char _temp1313[ 10u]="NAMESPACE"; static char _temp1316[ 6u]="XENUM";
static char _temp1319[ 5u]="FILL"; static char _temp1322[ 8u]="CODEGEN"; static
char _temp1325[ 4u]="CUT"; static char _temp1328[ 7u]="SPLICE"; static char
_temp1331[ 7u]="PRINTF"; static char _temp1334[ 8u]="FPRINTF"; static char
_temp1337[ 8u]="XPRINTF"; static char _temp1340[ 6u]="SCANF"; static char
_temp1343[ 7u]="FSCANF"; static char _temp1346[ 7u]="SSCANF"; static char
_temp1349[ 7u]="PTR_OP"; static char _temp1352[ 7u]="INC_OP"; static char
_temp1355[ 7u]="DEC_OP"; static char _temp1358[ 8u]="LEFT_OP"; static char
_temp1361[ 9u]="RIGHT_OP"; static char _temp1364[ 6u]="LE_OP"; static char
_temp1367[ 6u]="GE_OP"; static char _temp1370[ 6u]="EQ_OP"; static char
_temp1373[ 6u]="NE_OP"; static char _temp1376[ 7u]="AND_OP"; static char
_temp1379[ 6u]="OR_OP"; static char _temp1382[ 11u]="MUL_ASSIGN"; static char
_temp1385[ 11u]="DIV_ASSIGN"; static char _temp1388[ 11u]="MOD_ASSIGN"; static
char _temp1391[ 11u]="ADD_ASSIGN"; static char _temp1394[ 11u]="SUB_ASSIGN";
static char _temp1397[ 12u]="LEFT_ASSIGN"; static char _temp1400[ 13u]="RIGHT_ASSIGN";
static char _temp1403[ 11u]="AND_ASSIGN"; static char _temp1406[ 11u]="XOR_ASSIGN";
static char _temp1409[ 10u]="OR_ASSIGN"; static char _temp1412[ 9u]="ELLIPSIS";
static char _temp1415[ 11u]="LEFT_RIGHT"; static char _temp1418[ 12u]="COLON_COLON";
static char _temp1421[ 11u]="IDENTIFIER"; static char _temp1424[ 17u]="INTEGER_CONSTANT";
static char _temp1427[ 7u]="STRING"; static char _temp1430[ 19u]="CHARACTER_CONSTANT";
static char _temp1433[ 18u]="FLOATING_CONSTANT"; static char _temp1436[ 9u]="TYPE_VAR";
static char _temp1439[ 16u]="QUAL_IDENTIFIER"; static char _temp1442[ 18u]="QUAL_TYPEDEF_NAME";
static char _temp1445[ 4u]="';'"; static char _temp1448[ 4u]="'{'"; static char
_temp1451[ 4u]="'}'"; static char _temp1454[ 4u]="'='"; static char _temp1457[ 4u]="'_'";
static char _temp1460[ 4u]="'$'"; static char _temp1463[ 4u]="'('"; static char
_temp1466[ 4u]="')'"; static char _temp1469[ 4u]="'<'"; static char _temp1472[ 4u]="'>'";
static char _temp1475[ 4u]="','"; static char _temp1478[ 4u]="':'"; static char
_temp1481[ 4u]="'['"; static char _temp1484[ 4u]="']'"; static char _temp1487[ 4u]="'*'";
static char _temp1490[ 4u]="'@'"; static char _temp1493[ 4u]="'?'"; static char
_temp1496[ 4u]="'+'"; static char _temp1499[ 4u]="'.'"; static char _temp1502[ 4u]="'-'";
static char _temp1505[ 4u]="'&'"; static char _temp1508[ 4u]="'|'"; static char
_temp1511[ 4u]="'^'"; static char _temp1514[ 4u]="'/'"; static char _temp1517[ 4u]="'%'";
static char _temp1520[ 4u]="'~'"; static char _temp1523[ 4u]="'!'"; static char
_temp1526[ 5u]="prog"; static char _temp1529[ 17u]="translation_unit"; static
char _temp1532[ 21u]="translation_unit_opt"; static char _temp1535[ 21u]="external_declaration";
static char _temp1538[ 20u]="function_definition"; static char _temp1541[ 21u]="function_definition2";
static char _temp1544[ 13u]="using_action"; static char _temp1547[ 15u]="unusing_action";
static char _temp1550[ 17u]="namespace_action"; static char _temp1553[ 19u]="unnamespace_action";
static char _temp1556[ 12u]="declaration"; static char _temp1559[ 17u]="declaration_list";
static char _temp1562[ 23u]="declaration_specifiers"; static char _temp1565[ 24u]="storage_class_specifier";
static char _temp1568[ 15u]="type_specifier"; static char _temp1571[ 5u]="kind";
static char _temp1574[ 15u]="type_qualifier"; static char _temp1577[ 26u]="struct_or_union_specifier";
static char _temp1580[ 16u]="type_params_opt"; static char _temp1583[ 16u]="struct_or_union";
static char _temp1586[ 24u]="struct_declaration_list"; static char _temp1589[ 25u]="struct_declaration_list0";
static char _temp1592[ 21u]="init_declarator_list"; static char _temp1595[ 22u]="init_declarator_list0";
static char _temp1598[ 16u]="init_declarator"; static char _temp1601[ 19u]="struct_declaration";
static char _temp1604[ 25u]="specifier_qualifier_list"; static char _temp1607[
23u]="struct_declarator_list"; static char _temp1610[ 24u]="struct_declarator_list0";
static char _temp1613[ 18u]="struct_declarator"; static char _temp1616[ 15u]="enum_specifier";
static char _temp1619[ 16u]="enumerator_list"; static char _temp1622[ 11u]="enumerator";
static char _temp1625[ 11u]="declarator"; static char _temp1628[ 18u]="direct_declarator";
static char _temp1631[ 8u]="pointer"; static char _temp1634[ 13u]="pointer_char";
static char _temp1637[ 8u]="rgn_opt"; static char _temp1640[ 20u]="type_qualifier_list";
static char _temp1643[ 20u]="parameter_type_list"; static char _temp1646[ 11u]="effect_set";
static char _temp1649[ 14u]="atomic_effect"; static char _temp1652[ 11u]="region_set";
static char _temp1655[ 15u]="parameter_list"; static char _temp1658[ 22u]="parameter_declaration";
static char _temp1661[ 16u]="identifier_list"; static char _temp1664[ 17u]="identifier_list0";
static char _temp1667[ 12u]="initializer"; static char _temp1670[ 17u]="initializer_list";
static char _temp1673[ 12u]="designation"; static char _temp1676[ 16u]="designator_list";
static char _temp1679[ 11u]="designator"; static char _temp1682[ 10u]="type_name";
static char _temp1685[ 14u]="any_type_name"; static char _temp1688[ 15u]="type_name_list";
static char _temp1691[ 20u]="abstract_declarator"; static char _temp1694[ 27u]="direct_abstract_declarator";
static char _temp1697[ 10u]="statement"; static char _temp1700[ 18u]="labeled_statement";
static char _temp1703[ 21u]="expression_statement"; static char _temp1706[ 19u]="compound_statement";
static char _temp1709[ 16u]="block_item_list"; static char _temp1712[ 11u]="block_item";
static char _temp1715[ 20u]="selection_statement"; static char _temp1718[ 15u]="switch_clauses";
static char _temp1721[ 20u]="iteration_statement"; static char _temp1724[ 15u]="jump_statement";
static char _temp1727[ 8u]="pattern"; static char _temp1730[ 19u]="tuple_pattern_list";
static char _temp1733[ 20u]="tuple_pattern_list0"; static char _temp1736[ 14u]="field_pattern";
static char _temp1739[ 19u]="field_pattern_list"; static char _temp1742[ 20u]="field_pattern_list0";
static char _temp1745[ 11u]="expression"; static char _temp1748[ 22u]="assignment_expression";
static char _temp1751[ 20u]="assignment_operator"; static char _temp1754[ 23u]="conditional_expression";
static char _temp1757[ 20u]="constant_expression"; static char _temp1760[ 22u]="logical_or_expression";
static char _temp1763[ 23u]="logical_and_expression"; static char _temp1766[ 24u]="inclusive_or_expression";
static char _temp1769[ 24u]="exclusive_or_expression"; static char _temp1772[ 15u]="and_expression";
static char _temp1775[ 20u]="equality_expression"; static char _temp1778[ 22u]="relational_expression";
static char _temp1781[ 17u]="shift_expression"; static char _temp1784[ 20u]="additive_expression";
static char _temp1787[ 26u]="multiplicative_expression"; static char _temp1790[
16u]="cast_expression"; static char _temp1793[ 17u]="unary_expression"; static
char _temp1796[ 14u]="format_primop"; static char _temp1799[ 15u]="unary_operator";
static char _temp1802[ 19u]="postfix_expression"; static char _temp1805[ 19u]="primary_expression";
static char _temp1808[ 25u]="argument_expression_list"; static char _temp1811[
26u]="argument_expression_list0"; static char _temp1814[ 9u]="constant"; static
char _temp1817[ 20u]="qual_opt_identifier"; static struct _tagged_string Cyc_yytname[
215u]={( struct _tagged_string){ _temp1175, _temp1175, _temp1175 + 2u},( struct
_tagged_string){ _temp1178, _temp1178, _temp1178 + 6u},( struct _tagged_string){
_temp1181, _temp1181, _temp1181 + 12u},( struct _tagged_string){ _temp1184,
_temp1184, _temp1184 + 5u},( struct _tagged_string){ _temp1187, _temp1187,
_temp1187 + 9u},( struct _tagged_string){ _temp1190, _temp1190, _temp1190 + 7u},(
struct _tagged_string){ _temp1193, _temp1193, _temp1193 + 7u},( struct
_tagged_string){ _temp1196, _temp1196, _temp1196 + 8u},( struct _tagged_string){
_temp1199, _temp1199, _temp1199 + 5u},( struct _tagged_string){ _temp1202,
_temp1202, _temp1202 + 5u},( struct _tagged_string){ _temp1205, _temp1205,
_temp1205 + 6u},( struct _tagged_string){ _temp1208, _temp1208, _temp1208 + 4u},(
struct _tagged_string){ _temp1211, _temp1211, _temp1211 + 5u},( struct
_tagged_string){ _temp1214, _temp1214, _temp1214 + 6u},( struct _tagged_string){
_temp1217, _temp1217, _temp1217 + 7u},( struct _tagged_string){ _temp1220,
_temp1220, _temp1220 + 7u},( struct _tagged_string){ _temp1223, _temp1223,
_temp1223 + 9u},( struct _tagged_string){ _temp1226, _temp1226, _temp1226 + 6u},(
struct _tagged_string){ _temp1229, _temp1229, _temp1229 + 9u},( struct
_tagged_string){ _temp1232, _temp1232, _temp1232 + 9u},( struct _tagged_string){
_temp1235, _temp1235, _temp1235 + 7u},( struct _tagged_string){ _temp1238,
_temp1238, _temp1238 + 6u},( struct _tagged_string){ _temp1241, _temp1241,
_temp1241 + 5u},( struct _tagged_string){ _temp1244, _temp1244, _temp1244 + 8u},(
struct _tagged_string){ _temp1247, _temp1247, _temp1247 + 7u},( struct
_tagged_string){ _temp1250, _temp1250, _temp1250 + 3u},( struct _tagged_string){
_temp1253, _temp1253, _temp1253 + 5u},( struct _tagged_string){ _temp1256,
_temp1256, _temp1256 + 7u},( struct _tagged_string){ _temp1259, _temp1259,
_temp1259 + 6u},( struct _tagged_string){ _temp1262, _temp1262, _temp1262 + 3u},(
struct _tagged_string){ _temp1265, _temp1265, _temp1265 + 4u},( struct
_tagged_string){ _temp1268, _temp1268, _temp1268 + 5u},( struct _tagged_string){
_temp1271, _temp1271, _temp1271 + 9u},( struct _tagged_string){ _temp1274,
_temp1274, _temp1274 + 6u},( struct _tagged_string){ _temp1277, _temp1277,
_temp1277 + 7u},( struct _tagged_string){ _temp1280, _temp1280, _temp1280 + 7u},(
struct _tagged_string){ _temp1283, _temp1283, _temp1283 + 5u},( struct
_tagged_string){ _temp1286, _temp1286, _temp1286 + 8u},( struct _tagged_string){
_temp1289, _temp1289, _temp1289 + 4u},( struct _tagged_string){ _temp1292,
_temp1292, _temp1292 + 6u},( struct _tagged_string){ _temp1295, _temp1295,
_temp1295 + 4u},( struct _tagged_string){ _temp1298, _temp1298, _temp1298 + 6u},(
struct _tagged_string){ _temp1301, _temp1301, _temp1301 + 4u},( struct
_tagged_string){ _temp1304, _temp1304, _temp1304 + 9u},( struct _tagged_string){
_temp1307, _temp1307, _temp1307 + 9u},( struct _tagged_string){ _temp1310,
_temp1310, _temp1310 + 6u},( struct _tagged_string){ _temp1313, _temp1313,
_temp1313 + 10u},( struct _tagged_string){ _temp1316, _temp1316, _temp1316 + 6u},(
struct _tagged_string){ _temp1319, _temp1319, _temp1319 + 5u},( struct
_tagged_string){ _temp1322, _temp1322, _temp1322 + 8u},( struct _tagged_string){
_temp1325, _temp1325, _temp1325 + 4u},( struct _tagged_string){ _temp1328,
_temp1328, _temp1328 + 7u},( struct _tagged_string){ _temp1331, _temp1331,
_temp1331 + 7u},( struct _tagged_string){ _temp1334, _temp1334, _temp1334 + 8u},(
struct _tagged_string){ _temp1337, _temp1337, _temp1337 + 8u},( struct
_tagged_string){ _temp1340, _temp1340, _temp1340 + 6u},( struct _tagged_string){
_temp1343, _temp1343, _temp1343 + 7u},( struct _tagged_string){ _temp1346,
_temp1346, _temp1346 + 7u},( struct _tagged_string){ _temp1349, _temp1349,
_temp1349 + 7u},( struct _tagged_string){ _temp1352, _temp1352, _temp1352 + 7u},(
struct _tagged_string){ _temp1355, _temp1355, _temp1355 + 7u},( struct
_tagged_string){ _temp1358, _temp1358, _temp1358 + 8u},( struct _tagged_string){
_temp1361, _temp1361, _temp1361 + 9u},( struct _tagged_string){ _temp1364,
_temp1364, _temp1364 + 6u},( struct _tagged_string){ _temp1367, _temp1367,
_temp1367 + 6u},( struct _tagged_string){ _temp1370, _temp1370, _temp1370 + 6u},(
struct _tagged_string){ _temp1373, _temp1373, _temp1373 + 6u},( struct
_tagged_string){ _temp1376, _temp1376, _temp1376 + 7u},( struct _tagged_string){
_temp1379, _temp1379, _temp1379 + 6u},( struct _tagged_string){ _temp1382,
_temp1382, _temp1382 + 11u},( struct _tagged_string){ _temp1385, _temp1385,
_temp1385 + 11u},( struct _tagged_string){ _temp1388, _temp1388, _temp1388 + 11u},(
struct _tagged_string){ _temp1391, _temp1391, _temp1391 + 11u},( struct
_tagged_string){ _temp1394, _temp1394, _temp1394 + 11u},( struct _tagged_string){
_temp1397, _temp1397, _temp1397 + 12u},( struct _tagged_string){ _temp1400,
_temp1400, _temp1400 + 13u},( struct _tagged_string){ _temp1403, _temp1403,
_temp1403 + 11u},( struct _tagged_string){ _temp1406, _temp1406, _temp1406 + 11u},(
struct _tagged_string){ _temp1409, _temp1409, _temp1409 + 10u},( struct
_tagged_string){ _temp1412, _temp1412, _temp1412 + 9u},( struct _tagged_string){
_temp1415, _temp1415, _temp1415 + 11u},( struct _tagged_string){ _temp1418,
_temp1418, _temp1418 + 12u},( struct _tagged_string){ _temp1421, _temp1421,
_temp1421 + 11u},( struct _tagged_string){ _temp1424, _temp1424, _temp1424 + 17u},(
struct _tagged_string){ _temp1427, _temp1427, _temp1427 + 7u},( struct
_tagged_string){ _temp1430, _temp1430, _temp1430 + 19u},( struct _tagged_string){
_temp1433, _temp1433, _temp1433 + 18u},( struct _tagged_string){ _temp1436,
_temp1436, _temp1436 + 9u},( struct _tagged_string){ _temp1439, _temp1439,
_temp1439 + 16u},( struct _tagged_string){ _temp1442, _temp1442, _temp1442 + 18u},(
struct _tagged_string){ _temp1445, _temp1445, _temp1445 + 4u},( struct
_tagged_string){ _temp1448, _temp1448, _temp1448 + 4u},( struct _tagged_string){
_temp1451, _temp1451, _temp1451 + 4u},( struct _tagged_string){ _temp1454,
_temp1454, _temp1454 + 4u},( struct _tagged_string){ _temp1457, _temp1457,
_temp1457 + 4u},( struct _tagged_string){ _temp1460, _temp1460, _temp1460 + 4u},(
struct _tagged_string){ _temp1463, _temp1463, _temp1463 + 4u},( struct
_tagged_string){ _temp1466, _temp1466, _temp1466 + 4u},( struct _tagged_string){
_temp1469, _temp1469, _temp1469 + 4u},( struct _tagged_string){ _temp1472,
_temp1472, _temp1472 + 4u},( struct _tagged_string){ _temp1475, _temp1475,
_temp1475 + 4u},( struct _tagged_string){ _temp1478, _temp1478, _temp1478 + 4u},(
struct _tagged_string){ _temp1481, _temp1481, _temp1481 + 4u},( struct
_tagged_string){ _temp1484, _temp1484, _temp1484 + 4u},( struct _tagged_string){
_temp1487, _temp1487, _temp1487 + 4u},( struct _tagged_string){ _temp1490,
_temp1490, _temp1490 + 4u},( struct _tagged_string){ _temp1493, _temp1493,
_temp1493 + 4u},( struct _tagged_string){ _temp1496, _temp1496, _temp1496 + 4u},(
struct _tagged_string){ _temp1499, _temp1499, _temp1499 + 4u},( struct
_tagged_string){ _temp1502, _temp1502, _temp1502 + 4u},( struct _tagged_string){
_temp1505, _temp1505, _temp1505 + 4u},( struct _tagged_string){ _temp1508,
_temp1508, _temp1508 + 4u},( struct _tagged_string){ _temp1511, _temp1511,
_temp1511 + 4u},( struct _tagged_string){ _temp1514, _temp1514, _temp1514 + 4u},(
struct _tagged_string){ _temp1517, _temp1517, _temp1517 + 4u},( struct
_tagged_string){ _temp1520, _temp1520, _temp1520 + 4u},( struct _tagged_string){
_temp1523, _temp1523, _temp1523 + 4u},( struct _tagged_string){ _temp1526,
_temp1526, _temp1526 + 5u},( struct _tagged_string){ _temp1529, _temp1529,
_temp1529 + 17u},( struct _tagged_string){ _temp1532, _temp1532, _temp1532 + 21u},(
struct _tagged_string){ _temp1535, _temp1535, _temp1535 + 21u},( struct
_tagged_string){ _temp1538, _temp1538, _temp1538 + 20u},( struct _tagged_string){
_temp1541, _temp1541, _temp1541 + 21u},( struct _tagged_string){ _temp1544,
_temp1544, _temp1544 + 13u},( struct _tagged_string){ _temp1547, _temp1547,
_temp1547 + 15u},( struct _tagged_string){ _temp1550, _temp1550, _temp1550 + 17u},(
struct _tagged_string){ _temp1553, _temp1553, _temp1553 + 19u},( struct
_tagged_string){ _temp1556, _temp1556, _temp1556 + 12u},( struct _tagged_string){
_temp1559, _temp1559, _temp1559 + 17u},( struct _tagged_string){ _temp1562,
_temp1562, _temp1562 + 23u},( struct _tagged_string){ _temp1565, _temp1565,
_temp1565 + 24u},( struct _tagged_string){ _temp1568, _temp1568, _temp1568 + 15u},(
struct _tagged_string){ _temp1571, _temp1571, _temp1571 + 5u},( struct
_tagged_string){ _temp1574, _temp1574, _temp1574 + 15u},( struct _tagged_string){
_temp1577, _temp1577, _temp1577 + 26u},( struct _tagged_string){ _temp1580,
_temp1580, _temp1580 + 16u},( struct _tagged_string){ _temp1583, _temp1583,
_temp1583 + 16u},( struct _tagged_string){ _temp1586, _temp1586, _temp1586 + 24u},(
struct _tagged_string){ _temp1589, _temp1589, _temp1589 + 25u},( struct
_tagged_string){ _temp1592, _temp1592, _temp1592 + 21u},( struct _tagged_string){
_temp1595, _temp1595, _temp1595 + 22u},( struct _tagged_string){ _temp1598,
_temp1598, _temp1598 + 16u},( struct _tagged_string){ _temp1601, _temp1601,
_temp1601 + 19u},( struct _tagged_string){ _temp1604, _temp1604, _temp1604 + 25u},(
struct _tagged_string){ _temp1607, _temp1607, _temp1607 + 23u},( struct
_tagged_string){ _temp1610, _temp1610, _temp1610 + 24u},( struct _tagged_string){
_temp1613, _temp1613, _temp1613 + 18u},( struct _tagged_string){ _temp1616,
_temp1616, _temp1616 + 15u},( struct _tagged_string){ _temp1619, _temp1619,
_temp1619 + 16u},( struct _tagged_string){ _temp1622, _temp1622, _temp1622 + 11u},(
struct _tagged_string){ _temp1625, _temp1625, _temp1625 + 11u},( struct
_tagged_string){ _temp1628, _temp1628, _temp1628 + 18u},( struct _tagged_string){
_temp1631, _temp1631, _temp1631 + 8u},( struct _tagged_string){ _temp1634,
_temp1634, _temp1634 + 13u},( struct _tagged_string){ _temp1637, _temp1637,
_temp1637 + 8u},( struct _tagged_string){ _temp1640, _temp1640, _temp1640 + 20u},(
struct _tagged_string){ _temp1643, _temp1643, _temp1643 + 20u},( struct
_tagged_string){ _temp1646, _temp1646, _temp1646 + 11u},( struct _tagged_string){
_temp1649, _temp1649, _temp1649 + 14u},( struct _tagged_string){ _temp1652,
_temp1652, _temp1652 + 11u},( struct _tagged_string){ _temp1655, _temp1655,
_temp1655 + 15u},( struct _tagged_string){ _temp1658, _temp1658, _temp1658 + 22u},(
struct _tagged_string){ _temp1661, _temp1661, _temp1661 + 16u},( struct
_tagged_string){ _temp1664, _temp1664, _temp1664 + 17u},( struct _tagged_string){
_temp1667, _temp1667, _temp1667 + 12u},( struct _tagged_string){ _temp1670,
_temp1670, _temp1670 + 17u},( struct _tagged_string){ _temp1673, _temp1673,
_temp1673 + 12u},( struct _tagged_string){ _temp1676, _temp1676, _temp1676 + 16u},(
struct _tagged_string){ _temp1679, _temp1679, _temp1679 + 11u},( struct
_tagged_string){ _temp1682, _temp1682, _temp1682 + 10u},( struct _tagged_string){
_temp1685, _temp1685, _temp1685 + 14u},( struct _tagged_string){ _temp1688,
_temp1688, _temp1688 + 15u},( struct _tagged_string){ _temp1691, _temp1691,
_temp1691 + 20u},( struct _tagged_string){ _temp1694, _temp1694, _temp1694 + 27u},(
struct _tagged_string){ _temp1697, _temp1697, _temp1697 + 10u},( struct
_tagged_string){ _temp1700, _temp1700, _temp1700 + 18u},( struct _tagged_string){
_temp1703, _temp1703, _temp1703 + 21u},( struct _tagged_string){ _temp1706,
_temp1706, _temp1706 + 19u},( struct _tagged_string){ _temp1709, _temp1709,
_temp1709 + 16u},( struct _tagged_string){ _temp1712, _temp1712, _temp1712 + 11u},(
struct _tagged_string){ _temp1715, _temp1715, _temp1715 + 20u},( struct
_tagged_string){ _temp1718, _temp1718, _temp1718 + 15u},( struct _tagged_string){
_temp1721, _temp1721, _temp1721 + 20u},( struct _tagged_string){ _temp1724,
_temp1724, _temp1724 + 15u},( struct _tagged_string){ _temp1727, _temp1727,
_temp1727 + 8u},( struct _tagged_string){ _temp1730, _temp1730, _temp1730 + 19u},(
struct _tagged_string){ _temp1733, _temp1733, _temp1733 + 20u},( struct
_tagged_string){ _temp1736, _temp1736, _temp1736 + 14u},( struct _tagged_string){
_temp1739, _temp1739, _temp1739 + 19u},( struct _tagged_string){ _temp1742,
_temp1742, _temp1742 + 20u},( struct _tagged_string){ _temp1745, _temp1745,
_temp1745 + 11u},( struct _tagged_string){ _temp1748, _temp1748, _temp1748 + 22u},(
struct _tagged_string){ _temp1751, _temp1751, _temp1751 + 20u},( struct
_tagged_string){ _temp1754, _temp1754, _temp1754 + 23u},( struct _tagged_string){
_temp1757, _temp1757, _temp1757 + 20u},( struct _tagged_string){ _temp1760,
_temp1760, _temp1760 + 22u},( struct _tagged_string){ _temp1763, _temp1763,
_temp1763 + 23u},( struct _tagged_string){ _temp1766, _temp1766, _temp1766 + 24u},(
struct _tagged_string){ _temp1769, _temp1769, _temp1769 + 24u},( struct
_tagged_string){ _temp1772, _temp1772, _temp1772 + 15u},( struct _tagged_string){
_temp1775, _temp1775, _temp1775 + 20u},( struct _tagged_string){ _temp1778,
_temp1778, _temp1778 + 22u},( struct _tagged_string){ _temp1781, _temp1781,
_temp1781 + 17u},( struct _tagged_string){ _temp1784, _temp1784, _temp1784 + 20u},(
struct _tagged_string){ _temp1787, _temp1787, _temp1787 + 26u},( struct
_tagged_string){ _temp1790, _temp1790, _temp1790 + 16u},( struct _tagged_string){
_temp1793, _temp1793, _temp1793 + 17u},( struct _tagged_string){ _temp1796,
_temp1796, _temp1796 + 14u},( struct _tagged_string){ _temp1799, _temp1799,
_temp1799 + 15u},( struct _tagged_string){ _temp1802, _temp1802, _temp1802 + 19u},(
struct _tagged_string){ _temp1805, _temp1805, _temp1805 + 19u},( struct
_tagged_string){ _temp1808, _temp1808, _temp1808 + 25u},( struct _tagged_string){
_temp1811, _temp1811, _temp1811 + 26u},( struct _tagged_string){ _temp1814,
_temp1814, _temp1814 + 9u},( struct _tagged_string){ _temp1817, _temp1817,
_temp1817 + 20u}}; static short Cyc_yyr1[ 368u]={ (short)0, (short)117, (short)118,
(short)118, (short)118, (short)118, (short)118, (short)118, (short)118, (short)119,
(short)119, (short)120, (short)120, (short)121, (short)121, (short)121, (short)121,
(short)122, (short)122, (short)123, (short)124, (short)125, (short)126, (short)127,
(short)127, (short)127, (short)128, (short)128, (short)129, (short)129, (short)129,
(short)129, (short)129, (short)129, (short)129, (short)129, (short)130, (short)130,
(short)130, (short)130, (short)130, (short)130, (short)130, (short)131, (short)131,
(short)131, (short)131, (short)131, (short)131, (short)131, (short)131, (short)131,
(short)131, (short)131, (short)131, (short)131, (short)131, (short)131, (short)131,
(short)132, (short)132, (short)133, (short)133, (short)133, (short)134, (short)134,
(short)134, (short)134, (short)134, (short)135, (short)135, (short)135, (short)136,
(short)136, (short)137, (short)138, (short)138, (short)139, (short)140, (short)140,
(short)141, (short)141, (short)142, (short)143, (short)143, (short)143, (short)143,
(short)144, (short)145, (short)145, (short)146, (short)146, (short)146, (short)147,
(short)147, (short)147, (short)147, (short)147, (short)148, (short)148, (short)148,
(short)148, (short)149, (short)149, (short)149, (short)150, (short)150, (short)151,
(short)151, (short)151, (short)151, (short)151, (short)151, (short)151, (short)151,
(short)151, (short)151, (short)152, (short)152, (short)152, (short)152, (short)153,
(short)153, (short)153, (short)153, (short)153, (short)154, (short)154, (short)154,
(short)154, (short)155, (short)155, (short)156, (short)156, (short)156, (short)157,
(short)157, (short)158, (short)158, (short)158, (short)158, (short)159, (short)159,
(short)159, (short)159, (short)160, (short)160, (short)161, (short)161, (short)161,
(short)162, (short)163, (short)163, (short)164, (short)164, (short)164, (short)164,
(short)165, (short)165, (short)165, (short)165, (short)166, (short)167, (short)167,
(short)168, (short)168, (short)169, (short)169, (short)170, (short)170, (short)170,
(short)170, (short)171, (short)171, (short)172, (short)172, (short)172, (short)173,
(short)173, (short)173, (short)173, (short)173, (short)173, (short)173, (short)173,
(short)173, (short)173, (short)173, (short)173, (short)173, (short)174, (short)174,
(short)174, (short)174, (short)174, (short)174, (short)174, (short)174, (short)175,
(short)176, (short)176, (short)177, (short)177, (short)178, (short)178, (short)179,
(short)179, (short)179, (short)180, (short)180, (short)180, (short)180, (short)181,
(short)181, (short)181, (short)181, (short)181, (short)181, (short)182, (short)182,
(short)182, (short)182, (short)182, (short)182, (short)182, (short)182, (short)182,
(short)182, (short)182, (short)182, (short)182, (short)182, (short)183, (short)183,
(short)183, (short)183, (short)183, (short)183, (short)183, (short)183, (short)184,
(short)184, (short)184, (short)184, (short)184, (short)184, (short)184, (short)184,
(short)184, (short)184, (short)184, (short)184, (short)184, (short)184, (short)185,
(short)185, (short)186, (short)186, (short)187, (short)187, (short)188, (short)189,
(short)189, (short)190, (short)190, (short)191, (short)191, (short)192, (short)192,
(short)192, (short)192, (short)192, (short)192, (short)192, (short)192, (short)192,
(short)192, (short)192, (short)193, (short)193, (short)194, (short)195, (short)195,
(short)196, (short)196, (short)197, (short)197, (short)198, (short)198, (short)199,
(short)199, (short)200, (short)200, (short)200, (short)201, (short)201, (short)201,
(short)201, (short)201, (short)202, (short)202, (short)202, (short)203, (short)203,
(short)203, (short)204, (short)204, (short)204, (short)204, (short)205, (short)205,
(short)206, (short)206, (short)206, (short)206, (short)206, (short)206, (short)206,
(short)206, (short)206, (short)206, (short)206, (short)207, (short)207, (short)207,
(short)207, (short)207, (short)207, (short)208, (short)208, (short)208, (short)209,
(short)209, (short)209, (short)209, (short)209, (short)209, (short)209, (short)209,
(short)209, (short)209, (short)209, (short)209, (short)209, (short)209, (short)209,
(short)209, (short)209, (short)209, (short)209, (short)210, (short)210, (short)210,
(short)210, (short)210, (short)210, (short)210, (short)210, (short)210, (short)211,
(short)212, (short)212, (short)213, (short)213, (short)213, (short)213, (short)214,
(short)214}; static short Cyc_yyr2[ 368u]={ (short)0, (short)1, (short)1,
(short)2, (short)3, (short)5, (short)3, (short)5, (short)6, (short)0, (short)1,
(short)1, (short)1, (short)2, (short)3, (short)3, (short)4, (short)3, (short)4,
(short)2, (short)1, (short)2, (short)1, (short)2, (short)3, (short)5, (short)1,
(short)2, (short)1, (short)2, (short)1, (short)2, (short)1, (short)2, (short)1,
(short)2, (short)1, (short)1, (short)1, (short)1, (short)2, (short)1, (short)1,
(short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1,
(short)1, (short)1, (short)1, (short)1, (short)1, (short)3, (short)2, (short)4,
(short)1, (short)1, (short)1, (short)1, (short)1, (short)4, (short)6, (short)6,
(short)3, (short)3, (short)0, (short)3, (short)3, (short)1, (short)1, (short)1,
(short)1, (short)2, (short)1, (short)1, (short)3, (short)1, (short)3, (short)3,
(short)1, (short)2, (short)1, (short)2, (short)1, (short)1, (short)3, (short)1,
(short)2, (short)3, (short)4, (short)6, (short)3, (short)5, (short)2, (short)1,
(short)2, (short)3, (short)3, (short)1, (short)3, (short)5, (short)1, (short)2,
(short)1, (short)3, (short)3, (short)4, (short)4, (short)3, (short)5, (short)4,
(short)4, (short)4, (short)2, (short)3, (short)3, (short)4, (short)1, (short)1,
(short)4, (short)4, (short)1, (short)0, (short)1, (short)3, (short)1, (short)1,
(short)2, (short)1, (short)3, (short)3, (short)1, (short)3, (short)2, (short)3,
(short)1, (short)3, (short)1, (short)3, (short)3, (short)5, (short)1, (short)3,
(short)2, (short)1, (short)2, (short)1, (short)1, (short)3, (short)1, (short)2,
(short)3, (short)4, (short)1, (short)2, (short)3, (short)4, (short)2, (short)1,
(short)2, (short)3, (short)2, (short)1, (short)2, (short)1, (short)2, (short)3,
(short)3, (short)1, (short)3, (short)1, (short)1, (short)2, (short)3, (short)2,
(short)3, (short)3, (short)4, (short)2, (short)4, (short)3, (short)3, (short)5,
(short)4, (short)4, (short)4, (short)1, (short)1, (short)1, (short)1, (short)1,
(short)1, (short)2, (short)2, (short)3, (short)1, (short)2, (short)2, (short)3,
(short)1, (short)2, (short)1, (short)1, (short)1, (short)5, (short)7, (short)7,
(short)6, (short)0, (short)3, (short)4, (short)5, (short)6, (short)7, (short)5,
(short)7, (short)6, (short)7, (short)7, (short)8, (short)7, (short)8, (short)8,
(short)9, (short)6, (short)7, (short)7, (short)8, (short)3, (short)2, (short)2,
(short)2, (short)3, (short)2, (short)4, (short)5, (short)1, (short)3, (short)1,
(short)2, (short)1, (short)1, (short)1, (short)1, (short)5, (short)4, (short)4,
(short)5, (short)2, (short)2, (short)0, (short)1, (short)1, (short)3, (short)1,
(short)2, (short)1, (short)1, (short)3, (short)1, (short)3, (short)1, (short)3,
(short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1,
(short)1, (short)1, (short)1, (short)1, (short)5, (short)1, (short)1, (short)3,
(short)1, (short)3, (short)1, (short)3, (short)1, (short)3, (short)1, (short)3,
(short)1, (short)3, (short)3, (short)1, (short)3, (short)3, (short)3, (short)3,
(short)1, (short)3, (short)3, (short)1, (short)3, (short)3, (short)1, (short)3,
(short)3, (short)3, (short)1, (short)4, (short)1, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)4, (short)2, (short)2, (short)4, (short)1,
(short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1,
(short)1, (short)4, (short)3, (short)4, (short)3, (short)3, (short)3, (short)3,
(short)2, (short)2, (short)6, (short)7, (short)3, (short)4, (short)5, (short)9,
(short)2, (short)4, (short)4, (short)1, (short)1, (short)1, (short)3, (short)2,
(short)5, (short)4, (short)4, (short)5, (short)1, (short)1, (short)3, (short)1,
(short)1, (short)1, (short)1, (short)1, (short)1}; static short Cyc_yydefact[
682u]={ (short)0, (short)36, (short)37, (short)38, (short)39, (short)41, (short)43,
(short)45, (short)46, (short)47, (short)48, (short)49, (short)50, (short)51,
(short)52, (short)61, (short)62, (short)63, (short)72, (short)73, (short)34,
(short)0, (short)0, (short)42, (short)0, (short)0, (short)0, (short)366, (short)55,
(short)367, (short)69, (short)44, (short)0, (short)0, (short)121, (short)122,
(short)125, (short)1, (short)2, (short)11, (short)0, (short)0, (short)12,
(short)0, (short)28, (short)30, (short)32, (short)53, (short)0, (short)54,
(short)0, (short)105, (short)0, (short)126, (short)107, (short)40, (short)39,
(short)35, (short)0, (short)69, (short)246, (short)242, (short)245, (short)244,
(short)240, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)247,
(short)19, (short)21, (short)97, (short)0, (short)0, (short)57, (short)0,
(short)0, (short)0, (short)0, (short)3, (short)0, (short)0, (short)0, (short)0,
(short)23, (short)0, (short)77, (short)78, (short)80, (short)29, (short)31,
(short)33, (short)69, (short)0, (short)69, (short)0, (short)26, (short)0,
(short)0, (short)13, (short)0, (short)0, (short)0, (short)106, (short)127,
(short)129, (short)117, (short)0, (short)40, (short)0, (short)98, (short)102,
(short)95, (short)254, (short)0, (short)253, (short)243, (short)252, (short)0,
(short)0, (short)0, (short)59, (short)60, (short)56, (short)0, (short)83,
(short)85, (short)166, (short)168, (short)172, (short)0, (short)148, (short)0,
(short)145, (short)108, (short)0, (short)365, (short)0, (short)0, (short)0,
(short)0, (short)322, (short)323, (short)324, (short)325, (short)326, (short)327,
(short)0, (short)0, (short)362, (short)352, (short)363, (short)364, (short)0,
(short)0, (short)0, (short)0, (short)330, (short)0, (short)328, (short)329,
(short)0, (short)265, (short)278, (short)281, (short)283, (short)285, (short)287,
(short)289, (short)291, (short)294, (short)299, (short)302, (short)305, (short)309,
(short)0, (short)0, (short)311, (short)331, (short)351, (short)350, (short)0,
(short)4, (short)0, (short)6, (short)0, (short)24, (short)0, (short)0, (short)0,
(short)14, (short)68, (short)0, (short)74, (short)75, (short)0, (short)67,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)366, (short)199,
(short)201, (short)0, (short)207, (short)205, (short)0, (short)206, (short)190,
(short)191, (short)192, (short)0, (short)203, (short)193, (short)194, (short)195,
(short)0, (short)263, (short)27, (short)15, (short)80, (short)151, (short)0,
(short)112, (short)0, (short)132, (short)0, (short)150, (short)0, (short)109,
(short)0, (short)0, (short)130, (short)119, (short)118, (short)0, (short)93,
(short)99, (short)0, (short)0, (short)0, (short)0, (short)256, (short)0, (short)255,
(short)241, (short)0, (short)0, (short)254, (short)0, (short)141, (short)169,
(short)0, (short)84, (short)86, (short)0, (short)0, (short)174, (short)167,
(short)175, (short)0, (short)71, (short)70, (short)0, (short)0, (short)147,
(short)174, (short)149, (short)58, (short)0, (short)0, (short)319, (short)0,
(short)320, (short)347, (short)0, (short)0, (short)0, (short)312, (short)313,
(short)0, (short)0, (short)0, (short)0, (short)315, (short)309, (short)316,
(short)314, (short)123, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)268, (short)269,
(short)270, (short)271, (short)272, (short)273, (short)274, (short)275, (short)276,
(short)277, (short)267, (short)0, (short)0, (short)317, (short)0, (short)339,
(short)340, (short)0, (short)0, (short)0, (short)354, (short)0, (short)0,
(short)124, (short)20, (short)9, (short)22, (short)9, (short)79, (short)0,
(short)81, (short)153, (short)16, (short)0, (short)64, (short)76, (short)0,
(short)0, (short)87, (short)88, (short)90, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)233, (short)234, (short)235, (short)0,
(short)0, (short)237, (short)0, (short)196, (short)197, (short)0, (short)0,
(short)80, (short)202, (short)204, (short)200, (short)0, (short)139, (short)0,
(short)0, (short)135, (short)111, (short)0, (short)0, (short)114, (short)0,
(short)116, (short)115, (short)110, (short)128, (short)131, (short)120, (short)9,
(short)101, (short)100, (short)280, (short)103, (short)0, (short)0, (short)249,
(short)0, (short)25, (short)250, (short)0, (short)0, (short)0, (short)0, (short)162,
(short)258, (short)261, (short)0, (short)260, (short)0, (short)96, (short)0,
(short)0, (short)170, (short)0, (short)182, (short)0, (short)0, (short)178,
(short)0, (short)176, (short)0, (short)0, (short)0, (short)171, (short)173,
(short)146, (short)0, (short)0, (short)0, (short)343, (short)157, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)360, (short)0, (short)359,
(short)0, (short)0, (short)353, (short)282, (short)0, (short)284, (short)286,
(short)288, (short)290, (short)292, (short)293, (short)297, (short)298, (short)295,
(short)296, (short)300, (short)301, (short)303, (short)304, (short)306, (short)307,
(short)308, (short)266, (short)0, (short)337, (short)338, (short)333, (short)0,
(short)0, (short)335, (short)336, (short)0, (short)0, (short)10, (short)5,
(short)7, (short)154, (short)0, (short)0, (short)91, (short)82, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)232, (short)236, (short)0, (short)0, (short)0, (short)198,
(short)0, (short)17, (short)264, (short)0, (short)137, (short)0, (short)113,
(short)0, (short)134, (short)133, (short)152, (short)8, (short)0, (short)94,
(short)257, (short)0, (short)165, (short)259, (short)161, (short)163, (short)251,
(short)0, (short)248, (short)143, (short)142, (short)0, (short)184, (short)177,
(short)180, (short)0, (short)185, (short)0, (short)0, (short)179, (short)0,
(short)318, (short)0, (short)0, (short)344, (short)0, (short)158, (short)348,
(short)349, (short)0, (short)356, (short)0, (short)0, (short)0, (short)310,
(short)0, (short)321, (short)334, (short)332, (short)357, (short)0, (short)0,
(short)155, (short)0, (short)66, (short)89, (short)92, (short)65, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)212, (short)238, (short)0, (short)18, (short)140, (short)138, (short)136,
(short)104, (short)164, (short)262, (short)0, (short)183, (short)0, (short)187,
(short)189, (short)188, (short)181, (short)0, (short)345, (short)159, (short)0,
(short)361, (short)358, (short)0, (short)279, (short)355, (short)156, (short)208,
(short)212, (short)218, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)239,
(short)144, (short)186, (short)0, (short)160, (short)341, (short)0, (short)0,
(short)0, (short)0, (short)220, (short)0, (short)0, (short)0, (short)228,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)211, (short)0, (short)342, (short)209, (short)210, (short)219, (short)221,
(short)222, (short)0, (short)230, (short)229, (short)0, (short)224, (short)0,
(short)0, (short)0, (short)0, (short)212, (short)213, (short)0, (short)223,
(short)231, (short)225, (short)226, (short)0, (short)0, (short)212, (short)214,
(short)346, (short)227, (short)212, (short)215, (short)212, (short)216, (short)217,
(short)0, (short)0, (short)0}; static short Cyc_yydefgoto[ 98u]={ (short)679,
(short)484, (short)485, (short)38, (short)39, (short)217, (short)40, (short)346,
(short)41, (short)348, (short)42, (short)192, (short)43, (short)44, (short)45,
(short)126, (short)46, (short)47, (short)77, (short)48, (short)195, (short)196,
(short)88, (short)89, (short)90, (short)197, (short)134, (short)358, (short)359,
(short)360, (short)49, (short)112, (short)113, (short)50, (short)51, (short)52,
(short)53, (short)109, (short)247, (short)427, (short)387, (short)388, (short)265,
(short)238, (short)136, (short)239, (short)240, (short)442, (short)443, (short)444,
(short)414, (short)415, (short)131, (short)132, (short)133, (short)428, (short)272,
(short)220, (short)221, (short)222, (short)223, (short)224, (short)225, (short)226,
(short)620, (short)227, (short)228, (short)255, (short)256, (short)257, (short)417,
(short)418, (short)419, (short)229, (short)230, (short)332, (short)165, (short)404,
(short)166, (short)167, (short)168, (short)169, (short)170, (short)171, (short)172,
(short)173, (short)174, (short)175, (short)176, (short)177, (short)178, (short)179,
(short)180, (short)181, (short)449, (short)450, (short)182, (short)183}; static
short Cyc_yypact[ 682u]={ (short)716, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)52, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, (short)1856, (short)191, (short)1604, -
(short)-32768, (short)149, - (short)38, (short)149, - (short)-32768, - (short)19,
- (short)-32768, (short)50, - (short)-32768, - (short)4, (short)205, - (short)14,
(short)71, - (short)-32768, - (short)-32768, (short)716, - (short)-32768,
(short)166, (short)338, - (short)-32768, (short)112, (short)1856, (short)1856,
(short)1856, - (short)-32768, (short)217, - (short)-32768, (short)1807, (short)279,
(short)124, (short)97, - (short)-32768, (short)118, (short)66, - (short)-32768,
(short)149, (short)50, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)-32768, (short)192, (short)1604, (short)243, (short)215,
(short)1604, (short)240, (short)70, - (short)-32768, - (short)-32768, (short)244,
(short)35, (short)1988, - (short)-32768, (short)2048, (short)271, (short)3949,
(short)3949, - (short)-32768, (short)716, (short)716, (short)716, (short)716, -
(short)-32768, (short)276, (short)289, - (short)-32768, (short)1762, - (short)-32768,
- (short)-32768, - (short)-32768, (short)50, (short)2048, (short)50, (short)1055,
- (short)-32768, (short)1807, (short)112, - (short)-32768, (short)1555, (short)1988,
(short)2555, (short)279, (short)325, - (short)-32768, (short)92, (short)716, -
(short)-32768, (short)318, (short)83, (short)290, (short)327, (short)1604,
(short)336, - (short)-32768, - (short)-32768, - (short)-32768, (short)3949,
(short)263, (short)149, - (short)-32768, - (short)-32768, - (short)-32768,
(short)275, (short)2048, (short)2048, (short)159, - (short)-32768, (short)315, -
(short)3, (short)268, - (short)21, - (short)-32768, - (short)-32768, (short)4031,
- (short)-32768, (short)4113, (short)235, (short)352, (short)367, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
(short)4113, (short)4113, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)-32768, (short)370, (short)1392, (short)3949, (short)3949, - (short)-32768,
(short)3949, - (short)-32768, - (short)-32768, (short)376, - (short)-32768,
(short)13, (short)404, (short)363, (short)366, (short)377, (short)333, (short)89,
(short)399, - (short)16, (short)76, - (short)-32768, (short)613, (short)394,
(short)3949, (short)30, - (short)-32768, - (short)-32768, (short)25, (short)388,
- (short)-32768, (short)402, - (short)-32768, (short)406, - (short)-32768,
(short)205, (short)2637, (short)1807, - (short)-32768, (short)416, (short)418,
(short)2048, - (short)-32768, (short)147, (short)426, (short)425, (short)429,
(short)434, (short)1879, (short)438, (short)453, (short)447, (short)448, (short)2719,
(short)1879, (short)231, (short)1879, (short)1879, (short)440, - (short)-32768,
- (short)-32768, (short)443, - (short)-32768, - (short)-32768, (short)112, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)450,
(short)1169, - (short)-32768, - (short)-32768, - (short)-32768, (short)103, -
(short)-32768, - (short)-32768, - (short)-32768, (short)451, - (short)-32768,
(short)185, - (short)-32768, (short)446, (short)114, (short)452, (short)445,
(short)42, - (short)-32768, (short)444, (short)35, - (short)-32768, - (short)-32768,
(short)92, (short)456, - (short)-32768, (short)149, (short)149, (short)3949,
(short)458, (short)149, - (short)-32768, (short)459, (short)471, - (short)-32768,
(short)121, (short)387, (short)1604, (short)480, (short)79, - (short)-32768,
(short)482, - (short)-32768, - (short)-32768, (short)820, (short)2801, (short)228,
- (short)-32768, (short)282, (short)185, - (short)-32768, - (short)-32768,
(short)1988, (short)1658, - (short)-32768, (short)131, - (short)-32768, -
(short)-32768, (short)2048, (short)1392, - (short)-32768, (short)1392, - (short)-32768,
- (short)-32768, (short)2063, (short)3949, (short)1615, - (short)-32768, -
(short)-32768, (short)3949, (short)1169, (short)479, (short)23, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)3949,
(short)3949, (short)3949, (short)3949, (short)3949, (short)3949, (short)3949,
(short)3949, (short)3949, (short)3949, (short)3949, (short)3949, (short)3949,
(short)3949, (short)3949, (short)3949, (short)3949, (short)3949, (short)3949, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, (short)3949, (short)3949, - (short)-32768, (short)264, -
(short)-32768, - (short)-32768, (short)2883, (short)3949, (short)269, - (short)-32768,
(short)2473, (short)484, - (short)-32768, - (short)-32768, (short)716, - (short)-32768,
(short)716, - (short)-32768, (short)2145, - (short)-32768, - (short)-32768, -
(short)-32768, (short)2048, - (short)-32768, - (short)-32768, (short)3949,
(short)488, (short)483, - (short)-32768, (short)485, (short)2048, (short)3949,
(short)3949, (short)3949, (short)556, (short)1283, (short)497, - (short)-32768,
- (short)-32768, - (short)-32768, (short)138, (short)547, - (short)-32768,
(short)2965, - (short)-32768, - (short)-32768, (short)1879, (short)1501, (short)1762,
- (short)-32768, - (short)-32768, - (short)-32768, (short)3949, (short)509,
(short)298, (short)494, (short)490, - (short)-32768, (short)185, (short)2002, -
(short)-32768, (short)516, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)-32768, - (short)-32768, (short)716, - (short)-32768, -
(short)-32768, - (short)-32768, - (short)-32768, (short)2048, (short)508, -
(short)-32768, (short)1604, - (short)-32768, - (short)-32768, (short)3949,
(short)519, (short)1604, (short)62, - (short)-32768, - (short)-32768, - (short)-32768,
(short)512, (short)510, (short)514, - (short)-32768, (short)35, (short)522, -
(short)-32768, (short)185, - (short)-32768, (short)517, (short)518, - (short)-32768,
(short)513, (short)282, (short)549, (short)1988, (short)3047, - (short)-32768,
(short)315, - (short)-32768, (short)520, (short)523, (short)537, - (short)-32768,
- (short)-32768, (short)183, (short)2637, (short)255, (short)524, (short)205, -
(short)-32768, (short)527, (short)526, (short)538, (short)3129, - (short)-32768,
(short)404, (short)384, (short)363, (short)366, (short)377, (short)333, (short)89,
(short)89, (short)399, (short)399, (short)399, (short)399, - (short)16, -
(short)16, (short)76, (short)76, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, (short)532, - (short)-32768, - (short)-32768, - (short)-32768,
(short)536, (short)291, - (short)-32768, - (short)-32768, (short)197, (short)1988,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)200,
(short)542, - (short)-32768, - (short)-32768, (short)147, (short)3949, (short)548,
(short)308, (short)314, (short)319, (short)546, (short)3211, (short)3293,
(short)140, - (short)-32768, - (short)-32768, (short)554, (short)557, (short)551,
- (short)-32768, (short)1807, - (short)-32768, - (short)-32768, (short)35, -
(short)-32768, (short)558, - (short)-32768, (short)185, - (short)-32768, -
(short)-32768, - (short)-32768, - (short)-32768, (short)326, - (short)-32768, -
(short)-32768, (short)550, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)-32768, (short)764, - (short)-32768, (short)552, -
(short)-32768, (short)560, - (short)-32768, - (short)-32768, - (short)-32768,
(short)185, - (short)-32768, (short)561, (short)59, - (short)-32768, (short)559,
(short)564, (short)564, (short)553, - (short)-32768, (short)2227, - (short)-32768,
- (short)-32768, - (short)-32768, (short)1807, - (short)-32768, (short)3949,
(short)562, (short)2473, - (short)-32768, (short)3949, - (short)-32768, -
(short)-32768, - (short)-32768, - (short)-32768, (short)2473, (short)393, -
(short)-32768, (short)2309, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)-32768, (short)1879, (short)570, (short)1879, (short)3949, (short)3375,
(short)142, (short)3457, (short)154, (short)3539, (short)481, - (short)-32768,
(short)574, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, (short)522, - (short)-32768,
(short)595, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
(short)3949, - (short)-32768, - (short)-32768, (short)2637, - (short)-32768, -
(short)-32768, (short)212, - (short)-32768, - (short)-32768, - (short)-32768,
(short)623, (short)481, - (short)-32768, (short)337, (short)1879, (short)343,
(short)3621, (short)1879, (short)345, (short)3703, (short)3785, (short)169,
(short)1604, (short)592, (short)602, - (short)-32768, - (short)-32768, - (short)-32768,
(short)413, - (short)-32768, - (short)-32768, (short)2391, (short)1879, (short)604,
(short)607, - (short)-32768, (short)1879, (short)1879, (short)347, - (short)-32768,
(short)1879, (short)1879, (short)349, (short)1879, (short)353, (short)3867, -
(short)6, (short)1169, - (short)-32768, (short)3949, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)1879,
- (short)-32768, - (short)-32768, (short)1879, - (short)-32768, (short)1879,
(short)1879, (short)358, (short)3949, (short)941, - (short)-32768, (short)222, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)1879,
(short)423, (short)481, - (short)-32768, - (short)-32768, - (short)-32768,
(short)941, - (short)-32768, (short)481, - (short)-32768, - (short)-32768,
(short)698, (short)700, - (short)-32768}; static short Cyc_yypgoto[ 98u]={ -
(short)-32768, (short)29, - (short)313, - (short)-32768, (short)411, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)49,
- (short)32, - (short)20, - (short)-32768, (short)503, - (short)221, (short)499,
- (short)-32768, (short)11, - (short)-32768, - (short)39, - (short)-32768, -
(short)-32768, - (short)-32768, (short)515, (short)506, (short)53, - (short)-32768,
- (short)-32768, (short)211, - (short)-32768, - (short)76, - (short)-32768, -
(short)27, - (short)45, - (short)82, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)83, - (short)368, (short)431, - (short)350, - (short)66, - (short)263,
- (short)-32768, - (short)-32768, - (short)186, - (short)329, - (short)245, -
(short)-32768, (short)293, - (short)129, (short)432, - (short)90, (short)262, -
(short)233, - (short)201, - (short)-32768, - (short)-32768, - (short)46, -
(short)222, - (short)-32768, - (short)-32768, - (short)539, - (short)-32768, -
(short)-32768, - (short)13, (short)449, - (short)-32768, (short)180, - (short)-32768,
- (short)-32768, - (short)23, - (short)47, - (short)-32768, - (short)212, -
(short)326, - (short)-32768, (short)409, (short)408, (short)410, (short)407,
(short)435, (short)218, (short)91, (short)214, (short)216, - (short)95, (short)361,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)239,
- (short)-32768, - (short)-32768, (short)17}; static short Cyc_yytable[ 4230u]={
(short)57, (short)99, (short)366, (short)382, (short)102, (short)351, (short)79,
(short)106, (short)373, (short)70, (short)376, (short)377, (short)135, (short)482,
(short)241, (short)413, (short)91, (short)54, (short)100, (short)437, (short)237,
(short)488, (short)516, (short)397, (short)92, (short)93, (short)94, (short)246,
(short)295, (short)37, (short)101, (short)490, (short)55, (short)164, (short)184,
(short)486, (short)513, (short)431, (short)59, (short)71, (short)403, (short)72,
(short)99, (short)74, (short)73, (short)193, (short)431, (short)262, (short)270,
(short)218, (short)54, (short)231, (short)279, (short)117, (short)232, (short)54,
(short)120, (short)533, (short)243, (short)274, (short)54, (short)660, (short)75,
(short)297, (short)299, (short)97, (short)300, (short)82, (short)629, (short)54,
(short)115, (short)101, (short)451, (short)532, (short)233, (short)114, (short)281,
(short)80, (short)219, (short)282, (short)101, (short)302, (short)122, (short)71,
(short)334, (short)523, (short)71, (short)519, (short)335, (short)336, (short)337,
(short)316, (short)78, (short)317, (short)474, (short)661, (short)275, (short)276,
(short)259, (short)478, (short)54, (short)54, (short)54, (short)54, (short)394,
(short)341, (short)194, (short)278, (short)199, (short)15, (short)16, (short)17,
(short)185, (short)186, (short)187, (short)188, (short)342, (short)124, (short)54,
(short)303, (short)453, (short)593, (short)671, (short)384, (short)125, (short)253,
(short)338, (short)54, (short)437, (short)130, (short)343, (short)675, (short)339,
(short)71, (short)296, (short)677, (short)506, (short)678, (short)340, (short)248,
(short)114, (short)395, (short)276, (short)231, (short)352, (short)403, (short)353,
(short)585, (short)76, (short)198, (short)111, (short)54, (short)310, (short)311,
(short)438, (short)526, (short)439, (short)130, (short)594, (short)276, (short)422,
- (short)69, (short)81, (short)233, (short)411, (short)399, - (short)69, (short)568,
(short)76, (short)591, (short)412, (short)361, (short)101, (short)250, (short)401,
(short)402, (short)218, (short)507, (short)406, (short)423, (short)318, (short)266,
(short)267, (short)251, (short)107, (short)372, (short)270, (short)312, (short)313,
(short)319, (short)320, (short)108, (short)380, (short)383, (short)27, (short)279,
(short)34, (short)35, (short)36, (short)403, (short)29, (short)531, (short)87,
(short)384, (short)390, (short)219, (short)27, (short)54, (short)33, (short)110,
(short)130, (short)409, (short)29, (short)27, (short)391, (short)54, (short)34,
(short)35, (short)36, (short)29, (short)33, (short)384, (short)430, (short)470,
(short)471, (short)472, (short)602, (short)277, (short)503, (short)27, (short)578,
(short)27, (short)612, (short)269, (short)106, (short)29, (short)54, (short)29,
(short)384, (short)622, (short)384, (short)352, (short)384, (short)33, (short)615,
(short)218, (short)448, (short)416, (short)357, (short)198, (short)79, (short)34,
(short)35, (short)36, (short)384, (short)268, (short)83, (short)84, (short)548,
(short)641, (short)296, (short)269, (short)296, (short)34, (short)35, (short)36,
(short)445, (short)114, (short)114, (short)384, (short)447, (short)114, (short)385,
(short)27, (short)219, (short)546, (short)386, (short)71, (short)71, (short)29,
(short)455, (short)403, (short)58, (short)547, (short)413, (short)473, (short)448,
(short)27, (short)116, (short)561, (short)583, (short)448, (short)564, (short)29,
(short)54, (short)352, (short)54, (short)562, (short)119, (short)27, (short)565,
(short)33, (short)599, (short)352, (short)626, (short)29, (short)95, (short)54,
(short)96, (short)34, (short)35, (short)36, (short)627, (short)135, (short)672,
(short)489, (short)479, (short)599, (short)500, (short)287, (short)599, (short)374,
(short)384, (short)494, (short)268, (short)118, (short)288, (short)375, (short)448,
(short)130, (short)269, (short)99, (short)448, (short)121, (short)509, (short)123,
(short)130, (short)510, (short)130, (short)520, (short)495, (short)496, (short)497,
(short)540, (short)501, (short)603, (short)475, (short)101, (short)508, (short)539,
(short)27, (short)480, (short)549, (short)476, (short)260, (short)384, (short)29,
(short)556, (short)481, (short)261, (short)101, (short)598, (short)263, (short)54,
(short)277, (short)54, (short)189, (short)264, (short)137, (short)606, (short)269,
(short)608, (short)34, (short)35, (short)36, (short)103, (short)598, (short)104,
(short)432, (short)598, (short)433, (short)105, (short)599, (short)252, (short)434,
(short)263, - (short)69, (short)542, (short)76, (short)190, (short)512, (short)384,
(short)271, (short)563, (short)560, (short)522, (short)280, (short)352, (short)308,
(short)309, (short)525, (short)462, (short)463, (short)464, (short)465, (short)570,
(short)244, (short)198, (short)384, (short)631, (short)249, (short)571, (short)635,
(short)625, (short)384, (short)198, (short)572, (short)54, (short)254, (short)384,
(short)551, (short)662, (short)273, (short)586, (short)60, (short)71, (short)282,
(short)647, (short)85, (short)86, (short)71, (short)650, (short)651, (short)258,
(short)630, (short)653, (short)654, (short)384, (short)656, (short)670, (short)632,
(short)598, (short)636, (short)384, (short)652, (short)384, (short)655, (short)384,
(short)289, (short)384, (short)657, (short)664, (short)676, (short)384, (short)665,
(short)668, (short)666, (short)667, (short)384, (short)231, (short)314, (short)315,
(short)582, (short)290, (short)54, (short)361, (short)293, (short)673, (short)301,
(short)27, (short)61, (short)304, (short)62, (short)63, (short)305, (short)29,
(short)575, (short)577, (short)306, (short)410, (short)344, (short)64, (short)65,
(short)66, (short)384, (short)557, (short)130, (short)307, (short)101, (short)411,
(short)333, (short)67, (short)604, (short)276, (short)345, (short)412, (short)68,
(short)69, (short)347, (short)284, (short)352, (short)286, (short)99, (short)618,
(short)619, (short)193, (short)600, (short)354, (short)352, (short)54, (short)355,
(short)291, (short)292, (short)384, (short)645, (short)352, (short)416, (short)362,
(short)352, (short)298, (short)298, (short)363, (short)298, (short)384, (short)674,
(short)364, (short)460, (short)461, (short)466, (short)467, (short)365, (short)101,
(short)468, (short)469, (short)367, (short)368, (short)130, (short)369, (short)370,
(short)379, (short)298, (short)378, (short)381, (short)389, (short)191, (short)393,
(short)71, (short)396, (short)400, (short)392, (short)609, (short)611, (short)352,
(short)614, (short)405, (short)617, (short)407, (short)6, (short)7, (short)8,
(short)9, (short)10, (short)11, (short)12, (short)13, (short)14, (short)15,
(short)16, (short)17, (short)18, (short)19, (short)408, (short)421, (short)624,
(short)424, (short)129, (short)452, (short)129, (short)491, (short)128, (short)352,
(short)128, (short)483, (short)492, (short)498, (short)21, (short)493, (short)502,
(short)504, (short)634, (short)511, (short)514, (short)638, (short)640, (short)218,
(short)129, (short)26, (short)515, (short)518, (short)128, (short)521, (short)524,
(short)129, (short)129, (short)528, (short)642, (short)128, (short)128, (short)245,
(short)263, (short)529, (short)530, (short)218, (short)298, (short)534, (short)535,
(short)536, (short)543, (short)659, (short)545, (short)544, (short)550, (short)663,
(short)219, (short)552, (short)218, (short)553, (short)129, (short)129, (short)558,
(short)554, (short)128, (short)128, (short)559, (short)566, (short)71, (short)28,
(short)669, (short)30, (short)537, (short)569, (short)219, (short)573, (short)31,
(short)32, (short)579, (short)538, (short)580, (short)581, (short)628, (short)584,
(short)596, (short)589, (short)587, (short)219, (short)555, (short)129, (short)590,
(short)592, (short)601, (short)128, (short)607, (short)595, (short)298, (short)621,
(short)298, (short)298, (short)298, (short)298, (short)298, (short)298, (short)298,
(short)298, (short)298, (short)298, (short)298, (short)298, (short)298, (short)298,
(short)298, (short)298, (short)298, (short)321, (short)322, (short)323, (short)324,
(short)325, (short)326, (short)327, (short)328, (short)329, (short)330, (short)623,
(short)643, (short)644, (short)129, (short)648, (short)649, (short)680, (short)128,
(short)681, (short)446, (short)356, (short)567, (short)435, (short)349, (short)331,
(short)527, (short)436, (short)588, (short)420, (short)454, (short)456, (short)458,
(short)0, (short)457, (short)0, (short)0, (short)298, (short)1, (short)2,
(short)3, (short)4, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)0, (short)0, (short)20, (short)0, (short)459,
(short)0, (short)0, (short)0, (short)398, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)21, (short)0, (short)22, (short)0, (short)0, (short)0, (short)0,
(short)23, (short)0, (short)24, (short)25, (short)26, (short)0, (short)0,
(short)0, (short)129, (short)0, (short)0, (short)0, (short)128, (short)298,
(short)0, (short)0, (short)129, (short)129, (short)0, (short)0, (short)128,
(short)128, (short)129, (short)129, (short)0, (short)129, (short)128, (short)128,
(short)0, (short)128, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)27, (short)0, (short)0, (short)60, (short)0,
(short)28, (short)29, (short)30, (short)0, (short)0, (short)0, (short)0, (short)31,
(short)32, (short)33, (short)298, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)34, (short)35, (short)36, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)0, (short)0, (short)0, (short)0, (short)27, (short)61, (short)0,
(short)62, (short)63, (short)0, (short)29, (short)129, (short)298, (short)0,
(short)21, (short)128, (short)64, (short)65, (short)66, (short)129, (short)0,
(short)0, (short)0, (short)128, (short)411, (short)26, (short)67, (short)0,
(short)0, (short)0, (short)412, (short)68, (short)69, (short)0, (short)0,
(short)0, (short)129, (short)0, (short)0, (short)0, (short)128, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)129, (short)0,
(short)0, (short)0, (short)128, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)129, (short)0, (short)0, (short)28,
(short)128, (short)30, (short)425, (short)0, (short)0, (short)0, (short)31,
(short)32, (short)268, (short)426, (short)298, (short)0, (short)0, (short)0,
(short)269, (short)0, (short)34, (short)35, (short)36, (short)0, (short)0,
(short)0, (short)0, (short)129, (short)129, (short)0, (short)0, (short)128,
(short)128, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)1, (short)2, (short)3, (short)56, (short)5, (short)6, (short)7, (short)8,
(short)9, (short)10, (short)11, (short)12, (short)13, (short)14, (short)15,
(short)16, (short)17, (short)18, (short)19, (short)618, (short)619, (short)20,
(short)200, (short)0, (short)201, (short)202, (short)203, (short)204, (short)205,
(short)206, (short)207, (short)208, (short)138, (short)21, (short)139, (short)22,
(short)140, (short)209, (short)129, (short)141, (short)23, (short)210, (short)128,
(short)0, (short)26, (short)142, (short)143, (short)211, (short)212, (short)144,
(short)145, (short)146, (short)147, (short)148, (short)149, (short)0, (short)150,
(short)151, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)213, (short)152,
(short)153, (short)154, (short)155, (short)28, (short)29, (short)30, (short)214,
(short)98, (short)0, (short)0, (short)31, (short)216, (short)157, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)158, (short)0,
(short)0, (short)159, (short)0, (short)160, (short)161, (short)0, (short)0,
(short)0, (short)0, (short)162, (short)163, (short)1, (short)2, (short)3,
(short)56, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)0, (short)0, (short)20, (short)200, (short)0, (short)201,
(short)202, (short)203, (short)204, (short)205, (short)206, (short)207, (short)208,
(short)138, (short)21, (short)139, (short)22, (short)140, (short)209, (short)0,
(short)141, (short)23, (short)210, (short)0, (short)0, (short)26, (short)142,
(short)143, (short)211, (short)212, (short)144, (short)145, (short)146, (short)147,
(short)148, (short)149, (short)0, (short)150, (short)151, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)213, (short)152, (short)153, (short)154,
(short)155, (short)28, (short)29, (short)30, (short)214, (short)98, (short)215,
(short)0, (short)31, (short)216, (short)157, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)158, (short)0, (short)0, (short)159,
(short)0, (short)160, (short)161, (short)0, (short)0, (short)0, (short)0,
(short)162, (short)163, (short)1, (short)2, (short)3, (short)56, (short)5,
(short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)0,
(short)0, (short)20, (short)200, (short)0, (short)201, (short)202, (short)203,
(short)204, (short)205, (short)206, (short)207, (short)208, (short)138, (short)21,
(short)139, (short)22, (short)140, (short)209, (short)0, (short)141, (short)23,
(short)210, (short)0, (short)0, (short)26, (short)142, (short)143, (short)211,
(short)212, (short)144, (short)145, (short)146, (short)147, (short)148, (short)149,
(short)0, (short)150, (short)151, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)213, (short)152, (short)153, (short)154, (short)155, (short)28,
(short)29, (short)30, (short)214, (short)98, (short)0, (short)0, (short)31,
(short)216, (short)157, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)158, (short)0, (short)0, (short)159, (short)0,
(short)160, (short)161, (short)0, (short)0, (short)0, (short)0, (short)162,
(short)163, (short)1, (short)2, (short)3, (short)56, (short)5, (short)6, (short)7,
(short)8, (short)9, (short)10, (short)11, (short)12, (short)13, (short)14,
(short)15, (short)16, (short)17, (short)18, (short)19, (short)0, (short)0,
(short)20, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)138, (short)21, (short)139, (short)22,
(short)140, (short)0, (short)0, (short)141, (short)23, (short)0, (short)0,
(short)0, (short)26, (short)142, (short)143, (short)0, (short)0, (short)144,
(short)145, (short)146, (short)147, (short)148, (short)149, (short)0, (short)150,
(short)151, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)27, (short)152,
(short)153, (short)154, (short)155, (short)28, (short)29, (short)30, (short)499,
(short)0, (short)0, (short)0, (short)31, (short)216, (short)157, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)158, (short)0,
(short)0, (short)159, (short)0, (short)160, (short)161, (short)0, (short)0,
(short)0, (short)0, (short)162, (short)163, (short)6, (short)7, (short)8,
(short)9, (short)10, (short)11, (short)12, (short)13, (short)14, (short)15,
(short)16, (short)17, (short)18, (short)19, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)138, (short)21, (short)139, (short)0, (short)140,
(short)0, (short)0, (short)141, (short)0, (short)0, (short)0, (short)0, (short)26,
(short)142, (short)143, (short)0, (short)0, (short)144, (short)145, (short)146,
(short)147, (short)148, (short)149, (short)0, (short)150, (short)151, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)27, (short)152, (short)153,
(short)154, (short)155, (short)28, (short)29, (short)30, (short)0, (short)294,
(short)0, (short)0, (short)31, (short)216, (short)157, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)158, (short)0, (short)0,
(short)159, (short)0, (short)160, (short)161, (short)0, (short)0, (short)0,
(short)0, (short)162, (short)163, (short)6, (short)7, (short)8, (short)9,
(short)10, (short)11, (short)12, (short)13, (short)14, (short)15, (short)16,
(short)17, (short)18, (short)19, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)138, (short)21, (short)139, (short)0, (short)140, (short)0, (short)0,
(short)141, (short)0, (short)0, (short)0, (short)0, (short)26, (short)142,
(short)143, (short)0, (short)0, (short)144, (short)145, (short)146, (short)147,
(short)148, (short)149, (short)0, (short)150, (short)151, (short)0, (short)6,
(short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)27, (short)152, (short)153,
(short)154, (short)155, (short)28, (short)29, (short)30, (short)21, (short)0,
(short)0, (short)0, (short)31, (short)216, (short)157, (short)0, (short)0,
(short)0, (short)0, (short)26, (short)0, (short)0, (short)158, (short)0, (short)0,
(short)159, (short)0, (short)160, (short)161, (short)0, (short)0, (short)0,
(short)0, (short)162, (short)163, (short)1, (short)2, (short)3, (short)56,
(short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)234, (short)0, (short)20, (short)0, (short)60, (short)28, (short)0,
(short)30, (short)235, (short)0, (short)0, (short)0, (short)31, (short)32,
(short)21, (short)236, (short)0, (short)0, (short)0, (short)0, (short)0, (short)23,
(short)0, (short)0, (short)0, (short)26, (short)0, (short)0, (short)0, (short)6,
(short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)27, (short)61, (short)0,
(short)62, (short)63, (short)0, (short)29, (short)0, (short)21, (short)0,
(short)0, (short)27, (short)64, (short)65, (short)66, (short)0, (short)28,
(short)29, (short)30, (short)26, (short)0, (short)0, (short)67, (short)31,
(short)32, (short)33, (short)0, (short)68, (short)69, (short)0, (short)0,
(short)0, (short)0, (short)34, (short)35, (short)36, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)27,
(short)0, (short)0, (short)0, (short)0, (short)28, (short)29, (short)30, (short)425,
(short)0, (short)0, (short)0, (short)31, (short)32, (short)277, (short)426,
(short)0, (short)0, (short)0, (short)0, (short)269, (short)0, (short)34, (short)35,
(short)36, (short)1, (short)2, (short)3, (short)56, (short)5, (short)6, (short)7,
(short)8, (short)9, (short)10, (short)11, (short)12, (short)13, (short)14,
(short)15, (short)16, (short)17, (short)18, (short)19, (short)0, (short)0,
(short)20, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)21, (short)0, (short)22, (short)0,
(short)0, (short)0, (short)0, (short)23, (short)0, (short)0, (short)0, (short)26,
(short)1, (short)2, (short)3, (short)56, (short)5, (short)6, (short)7, (short)8,
(short)9, (short)10, (short)11, (short)12, (short)13, (short)14, (short)15,
(short)16, (short)17, (short)18, (short)19, (short)0, (short)0, (short)20,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)21, (short)0, (short)22, (short)0, (short)0,
(short)0, (short)28, (short)23, (short)30, (short)0, (short)98, (short)26,
(short)191, (short)31, (short)32, (short)0, (short)1, (short)2, (short)3,
(short)56, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)0, (short)0, (short)20, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)21,
(short)0, (short)28, (short)0, (short)30, (short)0, (short)98, (short)23,
(short)0, (short)31, (short)32, (short)26, (short)200, (short)0, (short)201,
(short)202, (short)203, (short)204, (short)205, (short)206, (short)207, (short)208,
(short)138, (short)0, (short)139, (short)0, (short)140, (short)209, (short)0,
(short)141, (short)0, (short)210, (short)0, (short)0, (short)0, (short)142,
(short)143, (short)211, (short)212, (short)144, (short)145, (short)146, (short)147,
(short)148, (short)149, (short)0, (short)150, (short)151, (short)0, (short)0,
(short)0, (short)28, (short)0, (short)30, (short)0, (short)0, (short)0, (short)0,
(short)31, (short)32, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)213, (short)152, (short)153, (short)154,
(short)155, (short)0, (short)29, (short)0, (short)214, (short)98, (short)0,
(short)0, (short)0, (short)156, (short)157, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)158, (short)0, (short)0, (short)159,
(short)0, (short)160, (short)161, (short)0, (short)0, (short)0, (short)0,
(short)162, (short)163, (short)6, (short)7, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)21, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)26, (short)0, (short)0, (short)21,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)26, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)28, (short)0, (short)30,
(short)0, (short)127, (short)0, (short)517, (short)31, (short)32, (short)21,
(short)0, (short)0, (short)0, (short)0, (short)28, (short)0, (short)30, (short)0,
(short)440, (short)0, (short)26, (short)31, (short)32, (short)138, (short)0,
(short)139, (short)0, (short)140, (short)0, (short)0, (short)141, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)142, (short)143, (short)0,
(short)0, (short)144, (short)145, (short)146, (short)147, (short)148, (short)149,
(short)0, (short)150, (short)151, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)28,
(short)0, (short)30, (short)0, (short)0, (short)0, (short)0, (short)31, (short)32,
(short)0, (short)27, (short)152, (short)153, (short)154, (short)155, (short)0,
(short)29, (short)0, (short)0, (short)350, (short)441, (short)0, (short)0,
(short)156, (short)157, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)411, (short)0, (short)158, (short)0, (short)0, (short)159, (short)412,
(short)160, (short)161, (short)0, (short)0, (short)0, (short)0, (short)162,
(short)163, (short)138, (short)0, (short)139, (short)0, (short)140, (short)0,
(short)0, (short)141, (short)0, (short)0, (short)0, (short)0, (short)0, (short)142,
(short)143, (short)0, (short)0, (short)144, (short)145, (short)146, (short)147,
(short)148, (short)149, (short)0, (short)150, (short)151, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)27, (short)152, (short)153, (short)154,
(short)155, (short)0, (short)29, (short)0, (short)0, (short)350, (short)487,
(short)0, (short)0, (short)156, (short)157, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)411, (short)0, (short)158, (short)0, (short)0,
(short)159, (short)412, (short)160, (short)161, (short)0, (short)0, (short)0,
(short)0, (short)162, (short)163, (short)138, (short)0, (short)139, (short)0,
(short)140, (short)0, (short)0, (short)141, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)142, (short)143, (short)0, (short)0, (short)144,
(short)145, (short)146, (short)147, (short)148, (short)149, (short)0, (short)150,
(short)151, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)27, (short)152,
(short)153, (short)154, (short)155, (short)0, (short)29, (short)0, (short)0,
(short)350, (short)597, (short)0, (short)0, (short)156, (short)157, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)411, (short)0, (short)158,
(short)0, (short)0, (short)159, (short)412, (short)160, (short)161, (short)0,
(short)0, (short)0, (short)0, (short)162, (short)163, (short)138, (short)0,
(short)139, (short)0, (short)140, (short)0, (short)0, (short)141, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)142, (short)143, (short)0,
(short)0, (short)144, (short)145, (short)146, (short)147, (short)148, (short)149,
(short)0, (short)150, (short)151, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)27, (short)152, (short)153, (short)154, (short)155, (short)0,
(short)29, (short)0, (short)0, (short)350, (short)605, (short)0, (short)0,
(short)156, (short)157, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)411, (short)0, (short)158, (short)0, (short)0, (short)159, (short)412,
(short)160, (short)161, (short)0, (short)0, (short)0, (short)0, (short)162,
(short)163, (short)138, (short)0, (short)139, (short)0, (short)140, (short)0,
(short)0, (short)141, (short)0, (short)0, (short)0, (short)0, (short)0, (short)142,
(short)143, (short)0, (short)0, (short)144, (short)145, (short)146, (short)147,
(short)148, (short)149, (short)0, (short)150, (short)151, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)27, (short)152, (short)153, (short)154,
(short)155, (short)0, (short)29, (short)0, (short)0, (short)350, (short)646,
(short)0, (short)0, (short)156, (short)157, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)411, (short)0, (short)158, (short)0, (short)0,
(short)159, (short)412, (short)160, (short)161, (short)0, (short)0, (short)0,
(short)0, (short)162, (short)163, (short)138, (short)0, (short)139, (short)0,
(short)140, (short)0, (short)0, (short)141, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)142, (short)143, (short)0, (short)0, (short)144,
(short)145, (short)146, (short)147, (short)148, (short)149, (short)0, (short)150,
(short)151, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)27, (short)152,
(short)153, (short)154, (short)155, (short)0, (short)29, (short)0, (short)0,
(short)350, (short)0, (short)0, (short)0, (short)156, (short)157, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)411, (short)0, (short)158,
(short)0, (short)0, (short)159, (short)412, (short)160, (short)161, (short)0,
(short)0, (short)0, (short)0, (short)162, (short)163, (short)138, (short)0,
(short)139, (short)0, (short)140, (short)0, (short)0, (short)141, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)142, (short)143, (short)0,
(short)0, (short)144, (short)145, (short)146, (short)147, (short)148, (short)149,
(short)0, (short)150, (short)151, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)27, (short)152, (short)153, (short)154, (short)155, (short)0,
(short)29, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)156,
(short)157, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)242,
(short)158, (short)0, (short)0, (short)159, (short)0, (short)160, (short)161,
(short)0, (short)0, (short)0, (short)0, (short)162, (short)163, (short)138,
(short)0, (short)139, (short)0, (short)140, (short)0, (short)0, (short)141,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)142, (short)143,
(short)0, (short)0, (short)144, (short)145, (short)146, (short)147, (short)148,
(short)149, (short)0, (short)150, (short)151, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)27, (short)152, (short)153, (short)154, (short)155,
(short)0, (short)29, (short)0, (short)0, (short)350, (short)0, (short)0, (short)0,
(short)156, (short)157, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)158, (short)0, (short)0, (short)159, (short)0,
(short)160, (short)161, (short)0, (short)0, (short)0, (short)0, (short)162,
(short)163, (short)138, (short)0, (short)139, (short)0, (short)140, (short)0,
(short)0, (short)141, (short)0, (short)0, (short)0, (short)0, (short)0, (short)142,
(short)143, (short)0, (short)0, (short)144, (short)145, (short)146, (short)147,
(short)148, (short)149, (short)0, (short)150, (short)151, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)27, (short)152, (short)153, (short)154,
(short)155, (short)0, (short)29, (short)0, (short)371, (short)0, (short)0,
(short)0, (short)0, (short)156, (short)157, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)158, (short)0, (short)0, (short)159,
(short)0, (short)160, (short)161, (short)0, (short)0, (short)0, (short)0,
(short)162, (short)163, (short)138, (short)0, (short)139, (short)0, (short)140,
(short)0, (short)0, (short)141, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)142, (short)143, (short)0, (short)0, (short)144, (short)145, (short)146,
(short)147, (short)148, (short)149, (short)0, (short)150, (short)151, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)27, (short)152, (short)153,
(short)154, (short)155, (short)0, (short)29, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)156, (short)157, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)429, (short)158, (short)0,
(short)0, (short)159, (short)0, (short)160, (short)161, (short)0, (short)0,
(short)0, (short)0, (short)162, (short)163, (short)138, (short)0, (short)139,
(short)0, (short)140, (short)0, (short)0, (short)141, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)142, (short)143, (short)0, (short)0,
(short)144, (short)145, (short)146, (short)147, (short)148, (short)149, (short)0,
(short)150, (short)151, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)27, (short)152, (short)153, (short)154, (short)155, (short)0, (short)29,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)156, (short)157,
(short)477, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)158,
(short)0, (short)0, (short)159, (short)0, (short)160, (short)161, (short)0,
(short)0, (short)0, (short)0, (short)162, (short)163, (short)138, (short)0,
(short)139, (short)0, (short)140, (short)0, (short)0, (short)141, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)142, (short)143, (short)0,
(short)0, (short)144, (short)145, (short)146, (short)147, (short)148, (short)149,
(short)0, (short)150, (short)151, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)27, (short)152, (short)153, (short)154, (short)155, (short)0,
(short)29, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)156,
(short)157, (short)505, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)158, (short)0, (short)0, (short)159, (short)0, (short)160,
(short)161, (short)0, (short)0, (short)0, (short)0, (short)162, (short)163,
(short)138, (short)0, (short)139, (short)0, (short)140, (short)0, (short)0,
(short)141, (short)0, (short)0, (short)0, (short)0, (short)0, (short)142,
(short)143, (short)0, (short)0, (short)144, (short)145, (short)146, (short)147,
(short)148, (short)149, (short)0, (short)150, (short)151, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)27, (short)152, (short)153, (short)154,
(short)155, (short)0, (short)29, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)156, (short)157, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)541, (short)158, (short)0, (short)0, (short)159,
(short)0, (short)160, (short)161, (short)0, (short)0, (short)0, (short)0,
(short)162, (short)163, (short)138, (short)0, (short)139, (short)0, (short)140,
(short)0, (short)0, (short)141, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)142, (short)143, (short)0, (short)0, (short)144, (short)145, (short)146,
(short)147, (short)148, (short)149, (short)0, (short)150, (short)151, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)27, (short)152, (short)153,
(short)154, (short)155, (short)0, (short)29, (short)0, (short)0, (short)555,
(short)0, (short)0, (short)0, (short)156, (short)157, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)158, (short)0, (short)0,
(short)159, (short)0, (short)160, (short)161, (short)0, (short)0, (short)0,
(short)0, (short)162, (short)163, (short)138, (short)0, (short)139, (short)0,
(short)140, (short)0, (short)0, (short)141, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)142, (short)143, (short)0, (short)0, (short)144,
(short)145, (short)146, (short)147, (short)148, (short)149, (short)0, (short)150,
(short)151, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)27, (short)152,
(short)153, (short)154, (short)155, (short)0, (short)29, (short)0, (short)574,
(short)0, (short)0, (short)0, (short)0, (short)156, (short)157, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)158, (short)0,
(short)0, (short)159, (short)0, (short)160, (short)161, (short)0, (short)0,
(short)0, (short)0, (short)162, (short)163, (short)138, (short)0, (short)139,
(short)0, (short)140, (short)0, (short)0, (short)141, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)142, (short)143, (short)0, (short)0,
(short)144, (short)145, (short)146, (short)147, (short)148, (short)149, (short)0,
(short)150, (short)151, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)27, (short)152, (short)153, (short)154, (short)155, (short)0, (short)29,
(short)0, (short)576, (short)0, (short)0, (short)0, (short)0, (short)156,
(short)157, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)158, (short)0, (short)0, (short)159, (short)0, (short)160, (short)161,
(short)0, (short)0, (short)0, (short)0, (short)162, (short)163, (short)138,
(short)0, (short)139, (short)0, (short)140, (short)0, (short)0, (short)141,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)142, (short)143,
(short)0, (short)0, (short)144, (short)145, (short)146, (short)147, (short)148,
(short)149, (short)0, (short)150, (short)151, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)27, (short)152, (short)153, (short)154, (short)155,
(short)0, (short)29, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)156, (short)157, (short)610, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)158, (short)0, (short)0, (short)159, (short)0,
(short)160, (short)161, (short)0, (short)0, (short)0, (short)0, (short)162,
(short)163, (short)138, (short)0, (short)139, (short)0, (short)140, (short)0,
(short)0, (short)141, (short)0, (short)0, (short)0, (short)0, (short)0, (short)142,
(short)143, (short)0, (short)0, (short)144, (short)145, (short)146, (short)147,
(short)148, (short)149, (short)0, (short)150, (short)151, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)27, (short)152, (short)153, (short)154,
(short)155, (short)0, (short)29, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)156, (short)157, (short)613, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)158, (short)0, (short)0, (short)159,
(short)0, (short)160, (short)161, (short)0, (short)0, (short)0, (short)0,
(short)162, (short)163, (short)138, (short)0, (short)139, (short)0, (short)140,
(short)0, (short)0, (short)141, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)142, (short)143, (short)0, (short)0, (short)144, (short)145, (short)146,
(short)147, (short)148, (short)149, (short)0, (short)150, (short)151, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)27, (short)152, (short)153,
(short)154, (short)155, (short)0, (short)29, (short)0, (short)616, (short)0,
(short)0, (short)0, (short)0, (short)156, (short)157, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)158, (short)0, (short)0,
(short)159, (short)0, (short)160, (short)161, (short)0, (short)0, (short)0,
(short)0, (short)162, (short)163, (short)138, (short)0, (short)139, (short)0,
(short)140, (short)0, (short)0, (short)141, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)142, (short)143, (short)0, (short)0, (short)144,
(short)145, (short)146, (short)147, (short)148, (short)149, (short)0, (short)150,
(short)151, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)27, (short)152,
(short)153, (short)154, (short)155, (short)0, (short)29, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)156, (short)157, (short)633,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)158, (short)0,
(short)0, (short)159, (short)0, (short)160, (short)161, (short)0, (short)0,
(short)0, (short)0, (short)162, (short)163, (short)138, (short)0, (short)139,
(short)0, (short)140, (short)0, (short)0, (short)141, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)142, (short)143, (short)0, (short)0,
(short)144, (short)145, (short)146, (short)147, (short)148, (short)149, (short)0,
(short)150, (short)151, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)27, (short)152, (short)153, (short)154, (short)155, (short)0, (short)29,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)156, (short)157,
(short)637, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)158,
(short)0, (short)0, (short)159, (short)0, (short)160, (short)161, (short)0,
(short)0, (short)0, (short)0, (short)162, (short)163, (short)138, (short)0,
(short)139, (short)0, (short)140, (short)0, (short)0, (short)141, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)142, (short)143, (short)0,
(short)0, (short)144, (short)145, (short)146, (short)147, (short)148, (short)149,
(short)0, (short)150, (short)151, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)27, (short)152, (short)153, (short)154, (short)155, (short)0,
(short)29, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)156,
(short)157, (short)639, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)158, (short)0, (short)0, (short)159, (short)0, (short)160,
(short)161, (short)0, (short)0, (short)0, (short)0, (short)162, (short)163,
(short)138, (short)0, (short)139, (short)0, (short)140, (short)0, (short)0,
(short)141, (short)0, (short)0, (short)0, (short)0, (short)0, (short)142,
(short)143, (short)0, (short)0, (short)144, (short)145, (short)146, (short)147,
(short)148, (short)149, (short)0, (short)150, (short)151, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)27, (short)152, (short)153, (short)154,
(short)155, (short)0, (short)29, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)156, (short)157, (short)658, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)158, (short)0, (short)0, (short)159,
(short)0, (short)160, (short)161, (short)0, (short)0, (short)0, (short)0,
(short)162, (short)163, (short)138, (short)0, (short)139, (short)0, (short)140,
(short)0, (short)0, (short)141, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)142, (short)143, (short)0, (short)0, (short)144, (short)145, (short)146,
(short)147, (short)148, (short)149, (short)0, (short)150, (short)151, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)27, (short)152, (short)153,
(short)154, (short)155, (short)0, (short)29, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)156, (short)157, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)158, (short)0, (short)0,
(short)159, (short)0, (short)160, (short)161, (short)0, (short)0, (short)0,
(short)0, (short)162, (short)163, (short)138, (short)0, (short)139, (short)0,
(short)140, (short)0, (short)0, (short)141, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)142, (short)143, (short)0, (short)0, (short)144,
(short)145, (short)146, (short)147, (short)148, (short)149, (short)0, (short)150,
(short)151, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)27, (short)152,
(short)153, (short)154, (short)155, (short)0, (short)29, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)156, (short)283, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)158, (short)0,
(short)0, (short)159, (short)0, (short)160, (short)161, (short)0, (short)0,
(short)0, (short)0, (short)162, (short)163, (short)138, (short)0, (short)139,
(short)0, (short)140, (short)0, (short)0, (short)141, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)142, (short)143, (short)0, (short)0,
(short)144, (short)145, (short)146, (short)147, (short)148, (short)149, (short)0,
(short)150, (short)151, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)27, (short)152, (short)153, (short)154, (short)155, (short)0, (short)29,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)156, (short)285,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)158,
(short)0, (short)0, (short)159, (short)0, (short)160, (short)161, (short)0,
(short)0, (short)0, (short)0, (short)162, (short)163}; static short Cyc_yycheck[
4230u]={ (short)20, (short)50, (short)203, (short)225, (short)50, (short)191,
(short)33, (short)52, (short)209, (short)22, (short)211, (short)212, (short)78,
(short)342, (short)104, (short)260, (short)43, (short)0, (short)50, (short)282,
(short)103, (short)350, (short)390, (short)244, (short)44, (short)45, (short)46,
(short)109, (short)157, (short)0, (short)50, (short)357, (short)84, (short)80,
(short)81, (short)348, (short)386, (short)270, (short)21, (short)22, (short)252,
(short)24, (short)91, (short)26, (short)82, (short)91, (short)279, (short)123,
(short)130, (short)98, (short)33, (short)100, (short)134, (short)66, (short)100,
(short)38, (short)69, (short)425, (short)105, (short)62, (short)43, (short)67,
(short)81, (short)158, (short)159, (short)48, (short)161, (short)38, (short)607,
(short)52, (short)59, (short)91, (short)294, (short)423, (short)101, (short)58,
(short)97, (short)91, (short)98, (short)100, (short)100, (short)68, (short)71,
(short)66, (short)179, (short)411, (short)69, (short)400, (short)58, (short)59,
(short)60, (short)107, (short)96, (short)109, (short)333, (short)101, (short)99,
(short)100, (short)121, (short)338, (short)83, (short)84, (short)85, (short)86,
(short)62, (short)80, (short)95, (short)134, (short)97, (short)17, (short)18,
(short)19, (short)83, (short)84, (short)85, (short)86, (short)91, (short)82,
(short)101, (short)106, (short)97, (short)62, (short)661, (short)100, (short)89,
(short)114, (short)96, (short)110, (short)391, (short)76, (short)105, (short)670,
(short)102, (short)116, (short)157, (short)674, (short)375, (short)676, (short)108,
(short)110, (short)123, (short)99, (short)100, (short)192, (short)191, (short)357,
(short)192, (short)515, (short)98, (short)96, (short)84, (short)134, (short)63,
(short)64, (short)283, (short)93, (short)285, (short)104, (short)99, (short)100,
(short)81, (short)91, (short)91, (short)190, (short)102, (short)247, (short)96,
(short)493, (short)98, (short)537, (short)108, (short)198, (short)192, (short)90,
(short)250, (short)251, (short)225, (short)378, (short)254, (short)100, (short)104,
(short)128, (short)129, (short)100, (short)87, (short)208, (short)268, (short)98,
(short)99, (short)113, (short)114, (short)94, (short)219, (short)90, (short)82,
(short)277, (short)104, (short)105, (short)106, (short)411, (short)88, (short)422,
(short)90, (short)100, (short)90, (short)225, (short)82, (short)190, (short)96,
(short)91, (short)157, (short)90, (short)88, (short)82, (short)100, (short)198,
(short)104, (short)105, (short)106, (short)88, (short)96, (short)100, (short)269,
(short)318, (short)319, (short)320, (short)555, (short)96, (short)90, (short)82,
(short)90, (short)82, (short)90, (short)102, (short)279, (short)88, (short)219,
(short)88, (short)100, (short)589, (short)100, (short)288, (short)100, (short)96,
(short)90, (short)294, (short)293, (short)260, (short)101, (short)196, (short)277,
(short)104, (short)105, (short)106, (short)100, (short)96, (short)90, (short)91,
(short)444, (short)90, (short)283, (short)102, (short)285, (short)104, (short)105,
(short)106, (short)289, (short)250, (short)251, (short)100, (short)290, (short)254,
(short)87, (short)82, (short)294, (short)92, (short)91, (short)260, (short)261,
(short)88, (short)303, (short)493, (short)91, (short)100, (short)529, (short)332,
(short)333, (short)82, (short)96, (short)92, (short)511, (short)338, (short)92,
(short)88, (short)277, (short)342, (short)279, (short)100, (short)83, (short)82,
(short)100, (short)96, (short)547, (short)350, (short)92, (short)88, (short)89,
(short)290, (short)91, (short)104, (short)105, (short)106, (short)100, (short)379,
(short)92, (short)354, (short)339, (short)562, (short)367, (short)84, (short)565,
(short)90, (short)100, (short)362, (short)96, (short)82, (short)91, (short)96,
(short)375, (short)276, (short)102, (short)380, (short)379, (short)93, (short)380,
(short)91, (short)283, (short)384, (short)285, (short)405, (short)363, (short)364,
(short)365, (short)433, (short)367, (short)557, (short)82, (short)367, (short)380,
(short)432, (short)82, (short)82, (short)97, (short)89, (short)91, (short)100,
(short)88, (short)452, (short)89, (short)96, (short)380, (short)547, (short)87,
(short)346, (short)96, (short)348, (short)90, (short)92, (short)97, (short)570,
(short)102, (short)572, (short)104, (short)105, (short)106, (short)96, (short)562,
(short)98, (short)96, (short)565, (short)98, (short)102, (short)627, (short)93,
(short)102, (short)87, (short)96, (short)434, (short)98, (short)100, (short)92,
(short)100, (short)130, (short)483, (short)103, (short)408, (short)134, (short)444,
(short)65, (short)66, (short)413, (short)310, (short)311, (short)312, (short)313,
(short)97, (short)81, (short)354, (short)100, (short)610, (short)92, (short)97,
(short)613, (short)599, (short)100, (short)362, (short)97, (short)400, (short)91,
(short)100, (short)447, (short)643, (short)107, (short)97, (short)37, (short)408,
(short)100, (short)628, (short)90, (short)91, (short)413, (short)632, (short)633,
(short)97, (short)97, (short)636, (short)637, (short)100, (short)639, (short)661,
(short)97, (short)627, (short)97, (short)100, (short)97, (short)100, (short)97,
(short)100, (short)96, (short)100, (short)97, (short)652, (short)674, (short)100,
(short)655, (short)97, (short)657, (short)658, (short)100, (short)508, (short)61,
(short)62, (short)508, (short)96, (short)447, (short)492, (short)96, (short)668,
(short)92, (short)82, (short)83, (short)67, (short)85, (short)86, (short)111,
(short)88, (short)499, (short)500, (short)112, (short)92, (short)92, (short)94,
(short)95, (short)96, (short)100, (short)101, (short)433, (short)110, (short)508,
(short)102, (short)96, (short)104, (short)99, (short)100, (short)92, (short)108,
(short)109, (short)110, (short)92, (short)138, (short)547, (short)140, (short)551,
(short)22, (short)23, (short)551, (short)553, (short)91, (short)555, (short)492,
(short)92, (short)150, (short)151, (short)100, (short)101, (short)562, (short)529,
(short)91, (short)565, (short)158, (short)159, (short)96, (short)161, (short)100,
(short)101, (short)96, (short)308, (short)309, (short)314, (short)315, (short)96,
(short)551, (short)316, (short)317, (short)96, (short)82, (short)483, (short)90,
(short)90, (short)96, (short)179, (short)101, (short)92, (short)97, (short)93,
(short)100, (short)529, (short)103, (short)92, (short)97, (short)573, (short)574,
(short)599, (short)576, (short)96, (short)578, (short)97, (short)8, (short)9,
(short)10, (short)11, (short)12, (short)13, (short)14, (short)15, (short)16,
(short)17, (short)18, (short)19, (short)20, (short)21, (short)100, (short)92,
(short)596, (short)92, (short)76, (short)97, (short)78, (short)90, (short)76,
(short)627, (short)78, (short)98, (short)100, (short)28, (short)36, (short)101,
(short)90, (short)41, (short)612, (short)81, (short)97, (short)615, (short)616,
(short)643, (short)96, (short)47, (short)107, (short)82, (short)96, (short)92,
(short)82, (short)103, (short)104, (short)92, (short)618, (short)103, (short)104,
(short)109, (short)87, (short)100, (short)97, (short)661, (short)252, (short)97,
(short)97, (short)103, (short)97, (short)641, (short)82, (short)97, (short)97,
(short)645, (short)643, (short)97, (short)674, (short)100, (short)128, (short)129,
(short)97, (short)92, (short)128, (short)129, (short)97, (short)92, (short)618,
(short)87, (short)660, (short)89, (short)90, (short)92, (short)661, (short)96,
(short)94, (short)95, (short)91, (short)97, (short)90, (short)97, (short)26,
(short)92, (short)98, (short)100, (short)103, (short)674, (short)91, (short)157,
(short)97, (short)97, (short)97, (short)157, (short)91, (short)103, (short)302,
(short)90, (short)304, (short)305, (short)306, (short)307, (short)308, (short)309,
(short)310, (short)311, (short)312, (short)313, (short)314, (short)315, (short)316,
(short)317, (short)318, (short)319, (short)320, (short)69, (short)70, (short)71,
(short)72, (short)73, (short)74, (short)75, (short)76, (short)77, (short)78,
(short)97, (short)101, (short)92, (short)196, (short)92, (short)90, (short)0,
(short)196, (short)0, (short)290, (short)196, (short)492, (short)273, (short)190,
(short)93, (short)414, (short)276, (short)529, (short)261, (short)302, (short)304,
(short)306, - (short)1, (short)305, - (short)1, - (short)1, (short)357, (short)3,
(short)4, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)20, (short)21, - (short)1, - (short)1, (short)24, - (short)1,
(short)307, - (short)1, - (short)1, - (short)1, (short)247, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, (short)36, - (short)1, (short)38,
- (short)1, - (short)1, - (short)1, - (short)1, (short)43, - (short)1, (short)45,
(short)46, (short)47, - (short)1, - (short)1, - (short)1, (short)268, - (short)1,
- (short)1, - (short)1, (short)268, (short)411, - (short)1, - (short)1, (short)276,
(short)277, - (short)1, - (short)1, (short)276, (short)277, (short)282, (short)283,
- (short)1, (short)285, (short)282, (short)283, - (short)1, (short)285, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)82, - (short)1, - (short)1, (short)37, - (short)1,
(short)87, (short)88, (short)89, - (short)1, - (short)1, - (short)1, - (short)1,
(short)94, (short)95, (short)96, (short)452, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)104, (short)105, (short)106, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)8, (short)9,
(short)10, (short)11, (short)12, (short)13, (short)14, (short)15, (short)16,
(short)17, (short)18, (short)19, (short)20, (short)21, - (short)1, - (short)1, -
(short)1, - (short)1, (short)82, (short)83, - (short)1, (short)85, (short)86, -
(short)1, (short)88, (short)354, (short)493, - (short)1, (short)36, (short)354,
(short)94, (short)95, (short)96, (short)362, - (short)1, - (short)1, - (short)1,
(short)362, (short)102, (short)47, (short)104, - (short)1, - (short)1, - (short)1,
(short)108, (short)109, (short)110, - (short)1, - (short)1, - (short)1, (short)379,
- (short)1, - (short)1, - (short)1, (short)379, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)391, - (short)1,
- (short)1, - (short)1, (short)391, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)405,
- (short)1, - (short)1, (short)87, (short)405, (short)89, (short)90, - (short)1,
- (short)1, - (short)1, (short)94, (short)95, (short)96, (short)97, (short)557,
- (short)1, - (short)1, - (short)1, (short)102, - (short)1, (short)104, (short)105,
(short)106, - (short)1, - (short)1, - (short)1, - (short)1, (short)432, (short)433,
- (short)1, - (short)1, (short)432, (short)433, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)3, (short)4,
(short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)20, (short)21, (short)22, (short)23, (short)24, (short)25, - (short)1,
(short)27, (short)28, (short)29, (short)30, (short)31, (short)32, (short)33,
(short)34, (short)35, (short)36, (short)37, (short)38, (short)39, (short)40,
(short)483, (short)42, (short)43, (short)44, (short)483, - (short)1, (short)47,
(short)48, (short)49, (short)50, (short)51, (short)52, (short)53, (short)54,
(short)55, (short)56, (short)57, - (short)1, (short)59, (short)60, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)82, (short)83, (short)84, (short)85, (short)86, (short)87, (short)88,
(short)89, (short)90, (short)91, - (short)1, - (short)1, (short)94, (short)95,
(short)96, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)104, - (short)1, - (short)1, (short)107, - (short)1, (short)109,
(short)110, - (short)1, - (short)1, - (short)1, - (short)1, (short)115, (short)116,
(short)3, (short)4, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)20, (short)21, - (short)1, - (short)1, (short)24,
(short)25, - (short)1, (short)27, (short)28, (short)29, (short)30, (short)31,
(short)32, (short)33, (short)34, (short)35, (short)36, (short)37, (short)38,
(short)39, (short)40, - (short)1, (short)42, (short)43, (short)44, - (short)1, -
(short)1, (short)47, (short)48, (short)49, (short)50, (short)51, (short)52,
(short)53, (short)54, (short)55, (short)56, (short)57, - (short)1, (short)59,
(short)60, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)82, (short)83, (short)84, (short)85, (short)86,
(short)87, (short)88, (short)89, (short)90, (short)91, (short)92, - (short)1,
(short)94, (short)95, (short)96, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)104, - (short)1, - (short)1, (short)107,
- (short)1, (short)109, (short)110, - (short)1, - (short)1, - (short)1, -
(short)1, (short)115, (short)116, (short)3, (short)4, (short)5, (short)6,
(short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)20,
(short)21, - (short)1, - (short)1, (short)24, (short)25, - (short)1, (short)27,
(short)28, (short)29, (short)30, (short)31, (short)32, (short)33, (short)34,
(short)35, (short)36, (short)37, (short)38, (short)39, (short)40, - (short)1,
(short)42, (short)43, (short)44, - (short)1, - (short)1, (short)47, (short)48,
(short)49, (short)50, (short)51, (short)52, (short)53, (short)54, (short)55,
(short)56, (short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)82,
(short)83, (short)84, (short)85, (short)86, (short)87, (short)88, (short)89,
(short)90, (short)91, - (short)1, - (short)1, (short)94, (short)95, (short)96, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)104, - (short)1, - (short)1, (short)107, - (short)1, (short)109, (short)110,
- (short)1, - (short)1, - (short)1, - (short)1, (short)115, (short)116, (short)3,
(short)4, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)20, (short)21, - (short)1, - (short)1, (short)24, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, (short)35, (short)36, (short)37, (short)38,
(short)39, - (short)1, - (short)1, (short)42, (short)43, - (short)1, - (short)1,
- (short)1, (short)47, (short)48, (short)49, - (short)1, - (short)1, (short)52,
(short)53, (short)54, (short)55, (short)56, (short)57, - (short)1, (short)59,
(short)60, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)82, (short)83, (short)84, (short)85, (short)86,
(short)87, (short)88, (short)89, (short)90, - (short)1, - (short)1, - (short)1,
(short)94, (short)95, (short)96, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)104, - (short)1, - (short)1, (short)107,
- (short)1, (short)109, (short)110, - (short)1, - (short)1, - (short)1, -
(short)1, (short)115, (short)116, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)20, (short)21, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, (short)35, (short)36, (short)37, - (short)1,
(short)39, - (short)1, - (short)1, (short)42, - (short)1, - (short)1, - (short)1,
- (short)1, (short)47, (short)48, (short)49, - (short)1, - (short)1, (short)52,
(short)53, (short)54, (short)55, (short)56, (short)57, - (short)1, (short)59,
(short)60, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)82, (short)83, (short)84, (short)85, (short)86,
(short)87, (short)88, (short)89, - (short)1, (short)91, - (short)1, - (short)1,
(short)94, (short)95, (short)96, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)104, - (short)1, - (short)1, (short)107,
- (short)1, (short)109, (short)110, - (short)1, - (short)1, - (short)1, -
(short)1, (short)115, (short)116, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)20, (short)21, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, (short)35, (short)36, (short)37, - (short)1,
(short)39, - (short)1, - (short)1, (short)42, - (short)1, - (short)1, - (short)1,
- (short)1, (short)47, (short)48, (short)49, - (short)1, - (short)1, (short)52,
(short)53, (short)54, (short)55, (short)56, (short)57, - (short)1, (short)59,
(short)60, - (short)1, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)20, (short)21, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)82, (short)83, (short)84, (short)85, (short)86, (short)87,
(short)88, (short)89, (short)36, - (short)1, - (short)1, - (short)1, (short)94,
(short)95, (short)96, - (short)1, - (short)1, - (short)1, - (short)1, (short)47,
- (short)1, - (short)1, (short)104, - (short)1, - (short)1, (short)107, -
(short)1, (short)109, (short)110, - (short)1, - (short)1, - (short)1, - (short)1,
(short)115, (short)116, (short)3, (short)4, (short)5, (short)6, (short)7,
(short)8, (short)9, (short)10, (short)11, (short)12, (short)13, (short)14,
(short)15, (short)16, (short)17, (short)18, (short)19, (short)20, (short)21,
(short)82, - (short)1, (short)24, - (short)1, (short)37, (short)87, - (short)1,
(short)89, (short)90, - (short)1, - (short)1, - (short)1, (short)94, (short)95,
(short)36, (short)97, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)43, - (short)1, - (short)1, - (short)1, (short)47, - (short)1, - (short)1,
- (short)1, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)20,
(short)21, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)82, (short)83, - (short)1, (short)85, (short)86, - (short)1, (short)88, -
(short)1, (short)36, - (short)1, - (short)1, (short)82, (short)94, (short)95,
(short)96, - (short)1, (short)87, (short)88, (short)89, (short)47, - (short)1, -
(short)1, (short)104, (short)94, (short)95, (short)96, - (short)1, (short)109,
(short)110, - (short)1, - (short)1, - (short)1, - (short)1, (short)104, (short)105,
(short)106, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)82,
- (short)1, - (short)1, - (short)1, - (short)1, (short)87, (short)88, (short)89,
(short)90, - (short)1, - (short)1, - (short)1, (short)94, (short)95, (short)96,
(short)97, - (short)1, - (short)1, - (short)1, - (short)1, (short)102, - (short)1,
(short)104, (short)105, (short)106, (short)3, (short)4, (short)5, (short)6,
(short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)20,
(short)21, - (short)1, - (short)1, (short)24, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, (short)36, - (short)1, (short)38, - (short)1, - (short)1,
- (short)1, - (short)1, (short)43, - (short)1, - (short)1, - (short)1, (short)47,
(short)3, (short)4, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)20, (short)21, - (short)1, - (short)1, (short)24, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)36, - (short)1, (short)38,
- (short)1, - (short)1, - (short)1, (short)87, (short)43, (short)89, - (short)1,
(short)91, (short)47, (short)93, (short)94, (short)95, - (short)1, (short)3,
(short)4, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)20, (short)21, - (short)1, - (short)1, (short)24, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, (short)36, - (short)1, (short)87,
- (short)1, (short)89, - (short)1, (short)91, (short)43, - (short)1, (short)94,
(short)95, (short)47, (short)25, - (short)1, (short)27, (short)28, (short)29,
(short)30, (short)31, (short)32, (short)33, (short)34, (short)35, - (short)1,
(short)37, - (short)1, (short)39, (short)40, - (short)1, (short)42, - (short)1,
(short)44, - (short)1, - (short)1, - (short)1, (short)48, (short)49, (short)50,
(short)51, (short)52, (short)53, (short)54, (short)55, (short)56, (short)57, -
(short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1, (short)87, -
(short)1, (short)89, - (short)1, - (short)1, - (short)1, - (short)1, (short)94,
(short)95, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)82, (short)83, (short)84, (short)85,
(short)86, - (short)1, (short)88, - (short)1, (short)90, (short)91, - (short)1,
- (short)1, - (short)1, (short)95, (short)96, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)104, - (short)1, -
(short)1, (short)107, - (short)1, (short)109, (short)110, - (short)1, - (short)1,
- (short)1, - (short)1, (short)115, (short)116, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)20, (short)21, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)20, (short)21, (short)36, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)47, - (short)1, - (short)1, (short)36, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, (short)47, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)20, (short)21, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)87, - (short)1, (short)89, - (short)1, (short)91, - (short)1, (short)79,
(short)94, (short)95, (short)36, - (short)1, - (short)1, - (short)1, - (short)1,
(short)87, - (short)1, (short)89, - (short)1, (short)30, - (short)1, (short)47,
(short)94, (short)95, (short)35, - (short)1, (short)37, - (short)1, (short)39, -
(short)1, - (short)1, (short)42, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)48, (short)49, - (short)1, - (short)1, (short)52, (short)53,
(short)54, (short)55, (short)56, (short)57, - (short)1, (short)59, (short)60, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)87, - (short)1, (short)89,
- (short)1, - (short)1, - (short)1, - (short)1, (short)94, (short)95, - (short)1,
(short)82, (short)83, (short)84, (short)85, (short)86, - (short)1, (short)88, -
(short)1, - (short)1, (short)91, (short)92, - (short)1, - (short)1, (short)95,
(short)96, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)102,
- (short)1, (short)104, - (short)1, - (short)1, (short)107, (short)108, (short)109,
(short)110, - (short)1, - (short)1, - (short)1, - (short)1, (short)115, (short)116,
(short)35, - (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1,
(short)42, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48,
(short)49, - (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55,
(short)56, (short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)82,
(short)83, (short)84, (short)85, (short)86, - (short)1, (short)88, - (short)1, -
(short)1, (short)91, (short)92, - (short)1, - (short)1, (short)95, (short)96, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)102, - (short)1,
(short)104, - (short)1, - (short)1, (short)107, (short)108, (short)109, (short)110,
- (short)1, - (short)1, - (short)1, - (short)1, (short)115, (short)116, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)82, (short)83,
(short)84, (short)85, (short)86, - (short)1, (short)88, - (short)1, - (short)1,
(short)91, (short)92, - (short)1, - (short)1, (short)95, (short)96, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)102, - (short)1, (short)104,
- (short)1, - (short)1, (short)107, (short)108, (short)109, (short)110, -
(short)1, - (short)1, - (short)1, - (short)1, (short)115, (short)116, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)82, (short)83,
(short)84, (short)85, (short)86, - (short)1, (short)88, - (short)1, - (short)1,
(short)91, (short)92, - (short)1, - (short)1, (short)95, (short)96, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)102, - (short)1, (short)104,
- (short)1, - (short)1, (short)107, (short)108, (short)109, (short)110, -
(short)1, - (short)1, - (short)1, - (short)1, (short)115, (short)116, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)82, (short)83,
(short)84, (short)85, (short)86, - (short)1, (short)88, - (short)1, - (short)1,
(short)91, (short)92, - (short)1, - (short)1, (short)95, (short)96, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)102, - (short)1, (short)104,
- (short)1, - (short)1, (short)107, (short)108, (short)109, (short)110, -
(short)1, - (short)1, - (short)1, - (short)1, (short)115, (short)116, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)82, (short)83,
(short)84, (short)85, (short)86, - (short)1, (short)88, - (short)1, - (short)1,
(short)91, - (short)1, - (short)1, - (short)1, (short)95, (short)96, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)102, - (short)1, (short)104,
- (short)1, - (short)1, (short)107, (short)108, (short)109, (short)110, -
(short)1, - (short)1, - (short)1, - (short)1, (short)115, (short)116, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)82, (short)83,
(short)84, (short)85, (short)86, - (short)1, (short)88, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)95, (short)96, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)103, (short)104,
- (short)1, - (short)1, (short)107, - (short)1, (short)109, (short)110, -
(short)1, - (short)1, - (short)1, - (short)1, (short)115, (short)116, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)82, (short)83,
(short)84, (short)85, (short)86, - (short)1, (short)88, - (short)1, - (short)1,
(short)91, - (short)1, - (short)1, - (short)1, (short)95, (short)96, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)104,
- (short)1, - (short)1, (short)107, - (short)1, (short)109, (short)110, -
(short)1, - (short)1, - (short)1, - (short)1, (short)115, (short)116, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)82, (short)83,
(short)84, (short)85, (short)86, - (short)1, (short)88, - (short)1, (short)90, -
(short)1, - (short)1, - (short)1, - (short)1, (short)95, (short)96, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)104,
- (short)1, - (short)1, (short)107, - (short)1, (short)109, (short)110, -
(short)1, - (short)1, - (short)1, - (short)1, (short)115, (short)116, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)82, (short)83,
(short)84, (short)85, (short)86, - (short)1, (short)88, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)95, (short)96, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)103, (short)104,
- (short)1, - (short)1, (short)107, - (short)1, (short)109, (short)110, -
(short)1, - (short)1, - (short)1, - (short)1, (short)115, (short)116, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)82, (short)83,
(short)84, (short)85, (short)86, - (short)1, (short)88, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)95, (short)96, (short)97,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)104,
- (short)1, - (short)1, (short)107, - (short)1, (short)109, (short)110, -
(short)1, - (short)1, - (short)1, - (short)1, (short)115, (short)116, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)82, (short)83,
(short)84, (short)85, (short)86, - (short)1, (short)88, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)95, (short)96, (short)97,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)104,
- (short)1, - (short)1, (short)107, - (short)1, (short)109, (short)110, -
(short)1, - (short)1, - (short)1, - (short)1, (short)115, (short)116, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)82, (short)83,
(short)84, (short)85, (short)86, - (short)1, (short)88, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)95, (short)96, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)103, (short)104,
- (short)1, - (short)1, (short)107, - (short)1, (short)109, (short)110, -
(short)1, - (short)1, - (short)1, - (short)1, (short)115, (short)116, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)82, (short)83,
(short)84, (short)85, (short)86, - (short)1, (short)88, - (short)1, - (short)1,
(short)91, - (short)1, - (short)1, - (short)1, (short)95, (short)96, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)104,
- (short)1, - (short)1, (short)107, - (short)1, (short)109, (short)110, -
(short)1, - (short)1, - (short)1, - (short)1, (short)115, (short)116, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)82, (short)83,
(short)84, (short)85, (short)86, - (short)1, (short)88, - (short)1, (short)90, -
(short)1, - (short)1, - (short)1, - (short)1, (short)95, (short)96, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)104,
- (short)1, - (short)1, (short)107, - (short)1, (short)109, (short)110, -
(short)1, - (short)1, - (short)1, - (short)1, (short)115, (short)116, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)82, (short)83,
(short)84, (short)85, (short)86, - (short)1, (short)88, - (short)1, (short)90, -
(short)1, - (short)1, - (short)1, - (short)1, (short)95, (short)96, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)104,
- (short)1, - (short)1, (short)107, - (short)1, (short)109, (short)110, -
(short)1, - (short)1, - (short)1, - (short)1, (short)115, (short)116, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)82, (short)83,
(short)84, (short)85, (short)86, - (short)1, (short)88, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)95, (short)96, (short)97,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)104,
- (short)1, - (short)1, (short)107, - (short)1, (short)109, (short)110, -
(short)1, - (short)1, - (short)1, - (short)1, (short)115, (short)116, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)82, (short)83,
(short)84, (short)85, (short)86, - (short)1, (short)88, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)95, (short)96, (short)97,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)104,
- (short)1, - (short)1, (short)107, - (short)1, (short)109, (short)110, -
(short)1, - (short)1, - (short)1, - (short)1, (short)115, (short)116, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)82, (short)83,
(short)84, (short)85, (short)86, - (short)1, (short)88, - (short)1, (short)90, -
(short)1, - (short)1, - (short)1, - (short)1, (short)95, (short)96, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)104,
- (short)1, - (short)1, (short)107, - (short)1, (short)109, (short)110, -
(short)1, - (short)1, - (short)1, - (short)1, (short)115, (short)116, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)82, (short)83,
(short)84, (short)85, (short)86, - (short)1, (short)88, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)95, (short)96, (short)97,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)104,
- (short)1, - (short)1, (short)107, - (short)1, (short)109, (short)110, -
(short)1, - (short)1, - (short)1, - (short)1, (short)115, (short)116, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)82, (short)83,
(short)84, (short)85, (short)86, - (short)1, (short)88, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)95, (short)96, (short)97,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)104,
- (short)1, - (short)1, (short)107, - (short)1, (short)109, (short)110, -
(short)1, - (short)1, - (short)1, - (short)1, (short)115, (short)116, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)82, (short)83,
(short)84, (short)85, (short)86, - (short)1, (short)88, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)95, (short)96, (short)97,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)104,
- (short)1, - (short)1, (short)107, - (short)1, (short)109, (short)110, -
(short)1, - (short)1, - (short)1, - (short)1, (short)115, (short)116, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)82, (short)83,
(short)84, (short)85, (short)86, - (short)1, (short)88, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)95, (short)96, (short)97,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)104,
- (short)1, - (short)1, (short)107, - (short)1, (short)109, (short)110, -
(short)1, - (short)1, - (short)1, - (short)1, (short)115, (short)116, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)82, (short)83,
(short)84, (short)85, (short)86, - (short)1, (short)88, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)95, (short)96, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)104,
- (short)1, - (short)1, (short)107, - (short)1, (short)109, (short)110, -
(short)1, - (short)1, - (short)1, - (short)1, (short)115, (short)116, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)82, (short)83,
(short)84, (short)85, (short)86, - (short)1, (short)88, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)95, (short)96, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)104,
- (short)1, - (short)1, (short)107, - (short)1, (short)109, (short)110, -
(short)1, - (short)1, - (short)1, - (short)1, (short)115, (short)116, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)82, (short)83,
(short)84, (short)85, (short)86, - (short)1, (short)88, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)95, (short)96, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)104,
- (short)1, - (short)1, (short)107, - (short)1, (short)109, (short)110, -
(short)1, - (short)1, - (short)1, - (short)1, (short)115, (short)116}; char Cyc_Yyimpossible_tag[
13u]="Yyimpossible"; struct Cyc_Yyimpossible_struct{ char* tag;}; char Cyc_Yystack_overflow_tag[
17u]="Yystack_overflow"; struct Cyc_Yystack_overflow_struct{ char* tag;}; extern
void Cyc_yyerror( struct _tagged_string); extern int Cyc_yylex(); char Cyc_YYINITIALSVAL_tag[
14u]="YYINITIALSVAL"; struct Cyc_YYINITIALSVAL_struct{ char* tag;}; static int
Cyc_yychar=( int)'\000'; static struct Cyc_YYINITIALSVAL_struct _temp1818={ Cyc_YYINITIALSVAL_tag};
struct _xenum_struct* Cyc_yylval=( struct _xenum_struct*)& _temp1818; static int
Cyc_yynerrs= 0; int Cyc_yyparse(){ int yystate; int yyn= 0; int yyerrstatus; int
yychar1= 0; int yyssp_offset; struct _tagged_ptr1 yyss=({ unsigned int _temp1819=(
unsigned int) 200; short* _temp1820=( short*) GC_malloc_atomic( sizeof( short) *
_temp1819); unsigned int i; struct _tagged_ptr1 _temp1821={ _temp1820, _temp1820,
_temp1820 + _temp1819}; for( i= 0; i < _temp1819; i ++){ _temp1820[ i]= (short)0;}
_temp1821;}); int yyvsp_offset; struct _tagged_ptr2 yyvs=({ unsigned int
_temp1822=( unsigned int) 200; struct _xenum_struct** _temp1823=( struct
_xenum_struct**) GC_malloc( sizeof( struct _xenum_struct*) * _temp1822);
unsigned int i; struct _tagged_ptr2 _temp1824={ _temp1823, _temp1823, _temp1823
+ _temp1822}; for( i= 0; i < _temp1822; i ++){ _temp1823[ i]= Cyc_yylval;}
_temp1824;}); int yylsp_offset; struct _tagged_ptr3 yyls=({ unsigned int
_temp1825=( unsigned int) 200; struct Cyc_Yyltype* _temp1826=( struct Cyc_Yyltype*)
GC_malloc( sizeof( struct Cyc_Yyltype) * _temp1825); unsigned int i; struct
_tagged_ptr3 _temp1827={ _temp1826, _temp1826, _temp1826 + _temp1825}; for( i= 0;
i < _temp1825; i ++){ _temp1826[ i]= Cyc_yynewloc();} _temp1827;}); int
yystacksize= 200; struct _xenum_struct* yyval= Cyc_yylval; int yylen; yystate= 0;
yyerrstatus= 0; Cyc_yynerrs= 0; Cyc_yychar= - 2; yyssp_offset= - 1; yyvsp_offset=
0; yylsp_offset= 0; yynewstate:({ struct _tagged_ptr1 _temp1828= yyss; short*
_temp1830= _temp1828.curr +( ++ yyssp_offset); if( _temp1830 < _temp1828.base? 1:
_temp1830 >= _temp1828.last_plus_one){ _throw( Null_Exception);}* _temp1830=(
short) yystate;}); if( yyssp_offset >= yystacksize - 1){ if( yystacksize >=
10000){ Cyc_yyerror(( struct _tagged_string)({ char* _temp1831=( char*)"parser stack overflow";
struct _tagged_string _temp1832; _temp1832.curr= _temp1831; _temp1832.base=
_temp1831; _temp1832.last_plus_one= _temp1831 + 22; _temp1832;}));( void) _throw(({
struct Cyc_Yystack_overflow_struct* _temp1833=( struct Cyc_Yystack_overflow_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Yystack_overflow_struct));* _temp1833=(
struct Cyc_Yystack_overflow_struct){.tag= Cyc_Yystack_overflow_tag};( struct
_xenum_struct*) _temp1833;}));} yystacksize *= 2; if( yystacksize > 10000){
yystacksize= 10000;}{ struct _tagged_ptr1 yyss1=({ unsigned int _temp1834=(
unsigned int) yystacksize; short* _temp1835=( short*) GC_malloc_atomic( sizeof(
short) * _temp1834); unsigned int i; struct _tagged_ptr1 _temp1836={ _temp1835,
_temp1835, _temp1835 + _temp1834}; for( i= 0; i < _temp1834; i ++){ _temp1835[ i]=
(short)0;} _temp1836;}); struct _tagged_ptr2 yyvs1=({ unsigned int _temp1837=(
unsigned int) yystacksize; struct _xenum_struct** _temp1838=( struct
_xenum_struct**) GC_malloc( sizeof( struct _xenum_struct*) * _temp1837);
unsigned int i; struct _tagged_ptr2 _temp1839={ _temp1838, _temp1838, _temp1838
+ _temp1837}; for( i= 0; i < _temp1837; i ++){ _temp1838[ i]= Cyc_yylval;}
_temp1839;}); struct _tagged_ptr3 yyls1=({ unsigned int _temp1840=( unsigned int)
yystacksize; struct Cyc_Yyltype* _temp1841=( struct Cyc_Yyltype*) GC_malloc(
sizeof( struct Cyc_Yyltype) * _temp1840); unsigned int i; struct _tagged_ptr3
_temp1842={ _temp1841, _temp1841, _temp1841 + _temp1840}; for( i= 0; i <
_temp1840; i ++){ _temp1841[ i]= Cyc_yynewloc();} _temp1842;});{ int i= 0; for(
0; i <= yyssp_offset; i ++){({ struct _tagged_ptr1 _temp1843= yyss1; short*
_temp1845= _temp1843.curr + i; if( _temp1845 < _temp1843.base? 1: _temp1845 >=
_temp1843.last_plus_one){ _throw( Null_Exception);}* _temp1845=({ struct
_tagged_ptr1 _temp1846= yyss; short* _temp1848= _temp1846.curr + i; if(
_temp1848 < _temp1846.base? 1: _temp1848 >= _temp1846.last_plus_one){ _throw(
Null_Exception);}* _temp1848;});});({ struct _tagged_ptr2 _temp1849= yyvs1;
struct _xenum_struct** _temp1851= _temp1849.curr + i; if( _temp1851 < _temp1849.base?
1: _temp1851 >= _temp1849.last_plus_one){ _throw( Null_Exception);}* _temp1851=({
struct _tagged_ptr2 _temp1852= yyvs; struct _xenum_struct** _temp1854= _temp1852.curr
+ i; if( _temp1854 < _temp1852.base? 1: _temp1854 >= _temp1852.last_plus_one){
_throw( Null_Exception);}* _temp1854;});});({ struct _tagged_ptr3 _temp1855=
yyls1; struct Cyc_Yyltype* _temp1857= _temp1855.curr + i; if( _temp1857 <
_temp1855.base? 1: _temp1857 >= _temp1855.last_plus_one){ _throw( Null_Exception);}*
_temp1857=({ struct _tagged_ptr3 _temp1858= yyls; struct Cyc_Yyltype* _temp1860=
_temp1858.curr + i; if( _temp1860 < _temp1858.base? 1: _temp1860 >= _temp1858.last_plus_one){
_throw( Null_Exception);}* _temp1860;});});}} yyss= yyss1; yyvs= yyvs1; yyls=
yyls1;}} goto yybackup; yybackup: yyn=( int)({ short* _temp1861=( short*) Cyc_yypact;
unsigned int _temp1862= yystate; if( _temp1862 >= 682u){ _throw( Null_Exception);}
_temp1861[ _temp1862];}); if( yyn == - 32768){ goto yydefault;} if( Cyc_yychar
== - 2){ Cyc_yychar= Cyc_yylex();} if( Cyc_yychar <= 0){ yychar1= 0; Cyc_yychar=
0;} else{ yychar1=( Cyc_yychar > 0? Cyc_yychar <= 344: 0)?( int)({ short*
_temp1863=( short*) Cyc_yytranslate; unsigned int _temp1864= Cyc_yychar; if(
_temp1864 >= 345u){ _throw( Null_Exception);} _temp1863[ _temp1864];}): 215;}
yyn += yychar1; if(( yyn < 0? 1: yyn > 4229)? 1:( int)({ short* _temp1865=(
short*) Cyc_yycheck; unsigned int _temp1866= yyn; if( _temp1866 >= 4230u){
_throw( Null_Exception);} _temp1865[ _temp1866];}) != yychar1){ goto yydefault;}
yyn=( int)({ short* _temp1867=( short*) Cyc_yytable; unsigned int _temp1868= yyn;
if( _temp1868 >= 4230u){ _throw( Null_Exception);} _temp1867[ _temp1868];}); if(
yyn < 0){ if( yyn == - 32768){ goto yyerrlab;} yyn= - yyn; goto yyreduce;} else{
if( yyn == 0){ goto yyerrlab;}} if( yyn == 681){ return 0;} if( Cyc_yychar != 0){
Cyc_yychar= - 2;}({ struct _tagged_ptr2 _temp1869= yyvs; struct _xenum_struct**
_temp1871= _temp1869.curr +( ++ yyvsp_offset); if( _temp1871 < _temp1869.base? 1:
_temp1871 >= _temp1869.last_plus_one){ _throw( Null_Exception);}* _temp1871= Cyc_yylval;});({
struct _tagged_ptr3 _temp1872= yyls; struct Cyc_Yyltype* _temp1874= _temp1872.curr
+( ++ yylsp_offset); if( _temp1874 < _temp1872.base? 1: _temp1874 >= _temp1872.last_plus_one){
_throw( Null_Exception);}* _temp1874= Cyc_yylloc;}); if( yyerrstatus != 0){
yyerrstatus --;} yystate= yyn; goto yynewstate; yydefault: yyn=( int)({ short*
_temp1875=( short*) Cyc_yydefact; unsigned int _temp1876= yystate; if( _temp1876
>= 682u){ _throw( Null_Exception);} _temp1875[ _temp1876];}); if( yyn == 0){
goto yyerrlab;} yyreduce: yylen=( int)({ short* _temp1877=( short*) Cyc_yyr2;
unsigned int _temp1878= yyn; if( _temp1878 >= 368u){ _throw( Null_Exception);}
_temp1877[ _temp1878];}); if( yylen > 0){ yyval=({ struct _tagged_ptr2 _temp1879=
yyvs; struct _xenum_struct** _temp1881= _temp1879.curr +(( yyvsp_offset + 1) -
yylen); if( _temp1881 < _temp1879.base? 1: _temp1881 >= _temp1879.last_plus_one){
_throw( Null_Exception);}* _temp1881;});} switch( yyn){ case 1: _LL1882: yyval=({
struct _tagged_ptr2 _temp1884= yyvs; struct _xenum_struct** _temp1886= _temp1884.curr
+ yyvsp_offset; if( _temp1886 < _temp1884.base? 1: _temp1886 >= _temp1884.last_plus_one){
_throw( Null_Exception);}* _temp1886;}); Cyc_Parse_parse_result= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp1887= yyvs; struct _xenum_struct** _temp1889= _temp1887.curr
+ yyvsp_offset; if( _temp1889 < _temp1887.base? 1: _temp1889 >= _temp1887.last_plus_one){
_throw( Null_Exception);}* _temp1889;})); break; case 2: _LL1883: yyval=({
struct _tagged_ptr2 _temp1891= yyvs; struct _xenum_struct** _temp1893= _temp1891.curr
+ yyvsp_offset; if( _temp1893 < _temp1891.base? 1: _temp1893 >= _temp1891.last_plus_one){
_throw( Null_Exception);}* _temp1893;}); break; case 3: _LL1890: yyval=({ struct
Cyc_DeclList_tok_struct* _temp1895=( struct Cyc_DeclList_tok_struct*) GC_malloc(
sizeof( struct Cyc_DeclList_tok_struct));* _temp1895=( struct Cyc_DeclList_tok_struct){.tag=
Cyc_DeclList_tok_tag,.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp1896= yyvs; struct _xenum_struct** _temp1898= _temp1896.curr +(
yyvsp_offset - 1); if( _temp1898 < _temp1896.base? 1: _temp1898 >= _temp1896.last_plus_one){
_throw( Null_Exception);}* _temp1898;})), Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp1899= yyvs; struct _xenum_struct** _temp1901= _temp1899.curr +
yyvsp_offset; if( _temp1901 < _temp1899.base? 1: _temp1901 >= _temp1899.last_plus_one){
_throw( Null_Exception);}* _temp1901;})))};( struct _xenum_struct*) _temp1895;});
break; case 4: _LL1894: yyval=({ struct Cyc_DeclList_tok_struct* _temp1903=(
struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));*
_temp1903=( struct Cyc_DeclList_tok_struct){.tag= Cyc_DeclList_tok_tag,.f1=({
struct Cyc_List_List* _temp1904=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1904->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1905=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1905->r=( void*)({ struct Cyc_Absyn_Using_d_struct* _temp1912=( struct Cyc_Absyn_Using_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct)); _temp1912->tag= Cyc_Absyn_Using_d_tag;
_temp1912->f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp1913= yyvs;
struct _xenum_struct** _temp1915= _temp1913.curr +( yyvsp_offset - 2); if(
_temp1915 < _temp1913.base? 1: _temp1915 >= _temp1913.last_plus_one){ _throw(
Null_Exception);}* _temp1915;})); _temp1912->f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp1916= yyvs; struct _xenum_struct** _temp1918= _temp1916.curr
+ yyvsp_offset; if( _temp1918 < _temp1916.base? 1: _temp1918 >= _temp1916.last_plus_one){
_throw( Null_Exception);}* _temp1918;}));( void*) _temp1912;}); _temp1905->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp1906= yyls; struct Cyc_Yyltype*
_temp1908= _temp1906.curr +( yylsp_offset - 2); if( _temp1908 < _temp1906.base?
1: _temp1908 >= _temp1906.last_plus_one){ _throw( Null_Exception);}* _temp1908;}).first_line,({
struct _tagged_ptr3 _temp1909= yyls; struct Cyc_Yyltype* _temp1911= _temp1909.curr
+ yylsp_offset; if( _temp1911 < _temp1909.base? 1: _temp1911 >= _temp1909.last_plus_one){
_throw( Null_Exception);}* _temp1911;}).last_line); _temp1905;}); _temp1904->tl=
0; _temp1904;})};( struct _xenum_struct*) _temp1903;}); Cyc_Lex_leave_using();
break; case 5: _LL1902: yyval=({ struct Cyc_DeclList_tok_struct* _temp1920=(
struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));*
_temp1920=( struct Cyc_DeclList_tok_struct){.tag= Cyc_DeclList_tok_tag,.f1=({
struct Cyc_List_List* _temp1921=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1921->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1925=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1925->r=( void*)({ struct Cyc_Absyn_Using_d_struct* _temp1932=( struct Cyc_Absyn_Using_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct)); _temp1932->tag= Cyc_Absyn_Using_d_tag;
_temp1932->f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp1933= yyvs;
struct _xenum_struct** _temp1935= _temp1933.curr +( yyvsp_offset - 4); if(
_temp1935 < _temp1933.base? 1: _temp1935 >= _temp1933.last_plus_one){ _throw(
Null_Exception);}* _temp1935;})); _temp1932->f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp1936= yyvs; struct _xenum_struct** _temp1938= _temp1936.curr
+( yyvsp_offset - 2); if( _temp1938 < _temp1936.base? 1: _temp1938 >= _temp1936.last_plus_one){
_throw( Null_Exception);}* _temp1938;}));( void*) _temp1932;}); _temp1925->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp1926= yyls; struct Cyc_Yyltype*
_temp1928= _temp1926.curr +( yylsp_offset - 4); if( _temp1928 < _temp1926.base?
1: _temp1928 >= _temp1926.last_plus_one){ _throw( Null_Exception);}* _temp1928;}).first_line,({
struct _tagged_ptr3 _temp1929= yyls; struct Cyc_Yyltype* _temp1931= _temp1929.curr
+( yylsp_offset - 1); if( _temp1931 < _temp1929.base? 1: _temp1931 >= _temp1929.last_plus_one){
_throw( Null_Exception);}* _temp1931;}).last_line); _temp1925;}); _temp1921->tl=
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp1922= yyvs; struct
_xenum_struct** _temp1924= _temp1922.curr + yyvsp_offset; if( _temp1924 <
_temp1922.base? 1: _temp1924 >= _temp1922.last_plus_one){ _throw( Null_Exception);}*
_temp1924;})); _temp1921;})};( struct _xenum_struct*) _temp1920;}); break; case
6: _LL1919: yyval=({ struct Cyc_DeclList_tok_struct* _temp1940=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));* _temp1940=( struct Cyc_DeclList_tok_struct){.tag=
Cyc_DeclList_tok_tag,.f1=({ struct Cyc_List_List* _temp1941=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1941->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1942=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1942->r=( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1949=( struct
Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp1949->tag= Cyc_Absyn_Namespace_d_tag; _temp1949->f1=({ struct
_tagged_string* _temp1950=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp1950[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp1951= yyvs; struct _xenum_struct** _temp1953= _temp1951.curr +(
yyvsp_offset - 2); if( _temp1953 < _temp1951.base? 1: _temp1953 >= _temp1951.last_plus_one){
_throw( Null_Exception);}* _temp1953;})); _temp1950;}); _temp1949->f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp1954= yyvs; struct _xenum_struct** _temp1956= _temp1954.curr
+ yyvsp_offset; if( _temp1956 < _temp1954.base? 1: _temp1956 >= _temp1954.last_plus_one){
_throw( Null_Exception);}* _temp1956;}));( void*) _temp1949;}); _temp1942->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp1943= yyls; struct Cyc_Yyltype*
_temp1945= _temp1943.curr +( yylsp_offset - 2); if( _temp1945 < _temp1943.base?
1: _temp1945 >= _temp1943.last_plus_one){ _throw( Null_Exception);}* _temp1945;}).first_line,({
struct _tagged_ptr3 _temp1946= yyls; struct Cyc_Yyltype* _temp1948= _temp1946.curr
+ yylsp_offset; if( _temp1948 < _temp1946.base? 1: _temp1948 >= _temp1946.last_plus_one){
_throw( Null_Exception);}* _temp1948;}).last_line); _temp1942;}); _temp1941->tl=
0; _temp1941;})};( struct _xenum_struct*) _temp1940;}); Cyc_Lex_leave_namespace();
break; case 7: _LL1939: yyval=({ struct Cyc_DeclList_tok_struct* _temp1958=(
struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));*
_temp1958=( struct Cyc_DeclList_tok_struct){.tag= Cyc_DeclList_tok_tag,.f1=({
struct Cyc_List_List* _temp1959=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1959->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1963=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1963->r=( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1970=( struct
Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp1970->tag= Cyc_Absyn_Namespace_d_tag; _temp1970->f1=({ struct
_tagged_string* _temp1971=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp1971[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp1972= yyvs; struct _xenum_struct** _temp1974= _temp1972.curr +(
yyvsp_offset - 4); if( _temp1974 < _temp1972.base? 1: _temp1974 >= _temp1972.last_plus_one){
_throw( Null_Exception);}* _temp1974;})); _temp1971;}); _temp1970->f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp1975= yyvs; struct _xenum_struct** _temp1977= _temp1975.curr
+( yyvsp_offset - 2); if( _temp1977 < _temp1975.base? 1: _temp1977 >= _temp1975.last_plus_one){
_throw( Null_Exception);}* _temp1977;}));( void*) _temp1970;}); _temp1963->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp1964= yyls; struct Cyc_Yyltype*
_temp1966= _temp1964.curr +( yylsp_offset - 4); if( _temp1966 < _temp1964.base?
1: _temp1966 >= _temp1964.last_plus_one){ _throw( Null_Exception);}* _temp1966;}).first_line,({
struct _tagged_ptr3 _temp1967= yyls; struct Cyc_Yyltype* _temp1969= _temp1967.curr
+( yylsp_offset - 1); if( _temp1969 < _temp1967.base? 1: _temp1969 >= _temp1967.last_plus_one){
_throw( Null_Exception);}* _temp1969;}).last_line); _temp1963;}); _temp1959->tl=
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp1960= yyvs; struct
_xenum_struct** _temp1962= _temp1960.curr + yyvsp_offset; if( _temp1962 <
_temp1960.base? 1: _temp1962 >= _temp1960.last_plus_one){ _throw( Null_Exception);}*
_temp1962;})); _temp1959;})};( struct _xenum_struct*) _temp1958;}); break; case
8: _LL1957: if( Cyc_String_strcmp( Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp1979= yyvs; struct _xenum_struct** _temp1981= _temp1979.curr +(
yyvsp_offset - 4); if( _temp1981 < _temp1979.base? 1: _temp1981 >= _temp1979.last_plus_one){
_throw( Null_Exception);}* _temp1981;})),( struct _tagged_string)({ char*
_temp1982=( char*)"C"; struct _tagged_string _temp1983; _temp1983.curr=
_temp1982; _temp1983.base= _temp1982; _temp1983.last_plus_one= _temp1982 + 2;
_temp1983;})) != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp1984=(
char*)"only extern \"C\" { ... } is allowed"; struct _tagged_string _temp1985;
_temp1985.curr= _temp1984; _temp1985.base= _temp1984; _temp1985.last_plus_one=
_temp1984 + 35; _temp1985;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp1986= yyls; struct Cyc_Yyltype* _temp1988= _temp1986.curr +( yylsp_offset -
5); if( _temp1988 < _temp1986.base? 1: _temp1988 >= _temp1986.last_plus_one){
_throw( Null_Exception);}* _temp1988;}).first_line,({ struct _tagged_ptr3
_temp1989= yyls; struct Cyc_Yyltype* _temp1991= _temp1989.curr +( yylsp_offset -
4); if( _temp1991 < _temp1989.base? 1: _temp1991 >= _temp1989.last_plus_one){
_throw( Null_Exception);}* _temp1991;}).last_line));} yyval=({ struct Cyc_DeclList_tok_struct*
_temp1992=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));*
_temp1992=( struct Cyc_DeclList_tok_struct){.tag= Cyc_DeclList_tok_tag,.f1=({
struct Cyc_List_List* _temp1993=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1993->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1997=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1997->r=( void*)({ struct Cyc_Absyn_ExternC_d_struct* _temp2004=( struct
Cyc_Absyn_ExternC_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ExternC_d_struct));
_temp2004->tag= Cyc_Absyn_ExternC_d_tag; _temp2004->f1= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2005= yyvs; struct _xenum_struct** _temp2007= _temp2005.curr
+( yyvsp_offset - 2); if( _temp2007 < _temp2005.base? 1: _temp2007 >= _temp2005.last_plus_one){
_throw( Null_Exception);}* _temp2007;}));( void*) _temp2004;}); _temp1997->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp1998= yyls; struct Cyc_Yyltype*
_temp2000= _temp1998.curr +( yylsp_offset - 5); if( _temp2000 < _temp1998.base?
1: _temp2000 >= _temp1998.last_plus_one){ _throw( Null_Exception);}* _temp2000;}).first_line,({
struct _tagged_ptr3 _temp2001= yyls; struct Cyc_Yyltype* _temp2003= _temp2001.curr
+( yylsp_offset - 1); if( _temp2003 < _temp2001.base? 1: _temp2003 >= _temp2001.last_plus_one){
_throw( Null_Exception);}* _temp2003;}).last_line); _temp1997;}); _temp1993->tl=
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp1994= yyvs; struct
_xenum_struct** _temp1996= _temp1994.curr + yyvsp_offset; if( _temp1996 <
_temp1994.base? 1: _temp1996 >= _temp1994.last_plus_one){ _throw( Null_Exception);}*
_temp1996;})); _temp1993;})};( struct _xenum_struct*) _temp1992;}); break; case
9: _LL1978: yyval=({ struct Cyc_DeclList_tok_struct* _temp2009=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));* _temp2009=( struct Cyc_DeclList_tok_struct){.tag=
Cyc_DeclList_tok_tag,.f1= 0};( struct _xenum_struct*) _temp2009;}); break; case
10: _LL2008: yyval=({ struct _tagged_ptr2 _temp2011= yyvs; struct _xenum_struct**
_temp2013= _temp2011.curr + yyvsp_offset; if( _temp2013 < _temp2011.base? 1:
_temp2013 >= _temp2011.last_plus_one){ _throw( Null_Exception);}* _temp2013;});
break; case 11: _LL2010: yyval=({ struct Cyc_DeclList_tok_struct* _temp2015=(
struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));*
_temp2015=( struct Cyc_DeclList_tok_struct){.tag= Cyc_DeclList_tok_tag,.f1=({
struct Cyc_List_List* _temp2016=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2016->hd=( void*) Cyc_Absyn_new_decl(({ struct Cyc_Absyn_Fn_d_struct*
_temp2017=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2017->tag= Cyc_Absyn_Fn_d_tag; _temp2017->f1= Cyc_yyget_FnDecl_tok(({
struct _tagged_ptr2 _temp2018= yyvs; struct _xenum_struct** _temp2020= _temp2018.curr
+ yyvsp_offset; if( _temp2020 < _temp2018.base? 1: _temp2020 >= _temp2018.last_plus_one){
_throw( Null_Exception);}* _temp2020;}));( void*) _temp2017;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2021= yyls; struct Cyc_Yyltype* _temp2023= _temp2021.curr
+ yylsp_offset; if( _temp2023 < _temp2021.base? 1: _temp2023 >= _temp2021.last_plus_one){
_throw( Null_Exception);}* _temp2023;}).first_line,({ struct _tagged_ptr3
_temp2024= yyls; struct Cyc_Yyltype* _temp2026= _temp2024.curr + yylsp_offset;
if( _temp2026 < _temp2024.base? 1: _temp2026 >= _temp2024.last_plus_one){ _throw(
Null_Exception);}* _temp2026;}).last_line)); _temp2016->tl= 0; _temp2016;})};(
struct _xenum_struct*) _temp2015;}); break; case 12: _LL2014: yyval=({ struct
_tagged_ptr2 _temp2028= yyvs; struct _xenum_struct** _temp2030= _temp2028.curr +
yyvsp_offset; if( _temp2030 < _temp2028.base? 1: _temp2030 >= _temp2028.last_plus_one){
_throw( Null_Exception);}* _temp2030;}); break; case 13: _LL2027: yyval=({
struct Cyc_FnDecl_tok_struct* _temp2032=( struct Cyc_FnDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));* _temp2032=( struct Cyc_FnDecl_tok_struct){.tag=
Cyc_FnDecl_tok_tag,.f1= Cyc_Parse_make_function( 0, Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2033= yyvs; struct _xenum_struct** _temp2035= _temp2033.curr
+( yyvsp_offset - 1); if( _temp2035 < _temp2033.base? 1: _temp2035 >= _temp2033.last_plus_one){
_throw( Null_Exception);}* _temp2035;})), 0, Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp2036= yyvs; struct _xenum_struct** _temp2038= _temp2036.curr +
yyvsp_offset; if( _temp2038 < _temp2036.base? 1: _temp2038 >= _temp2036.last_plus_one){
_throw( Null_Exception);}* _temp2038;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2039= yyls; struct Cyc_Yyltype* _temp2041= _temp2039.curr +(
yylsp_offset - 1); if( _temp2041 < _temp2039.base? 1: _temp2041 >= _temp2039.last_plus_one){
_throw( Null_Exception);}* _temp2041;}).first_line,({ struct _tagged_ptr3
_temp2042= yyls; struct Cyc_Yyltype* _temp2044= _temp2042.curr + yylsp_offset;
if( _temp2044 < _temp2042.base? 1: _temp2044 >= _temp2042.last_plus_one){ _throw(
Null_Exception);}* _temp2044;}).last_line))};( struct _xenum_struct*) _temp2032;});
break; case 14: _LL2031: yyval=({ struct Cyc_FnDecl_tok_struct* _temp2046=(
struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));*
_temp2046=( struct Cyc_FnDecl_tok_struct){.tag= Cyc_FnDecl_tok_tag,.f1= Cyc_Parse_make_function(({
struct Cyc_Core_Opt* _temp2047=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2047->v=( void*) Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2048= yyvs; struct _xenum_struct** _temp2050= _temp2048.curr +(
yyvsp_offset - 2); if( _temp2050 < _temp2048.base? 1: _temp2050 >= _temp2048.last_plus_one){
_throw( Null_Exception);}* _temp2050;})); _temp2047;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2051= yyvs; struct _xenum_struct** _temp2053= _temp2051.curr
+( yyvsp_offset - 1); if( _temp2053 < _temp2051.base? 1: _temp2053 >= _temp2051.last_plus_one){
_throw( Null_Exception);}* _temp2053;})), 0, Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp2054= yyvs; struct _xenum_struct** _temp2056= _temp2054.curr +
yyvsp_offset; if( _temp2056 < _temp2054.base? 1: _temp2056 >= _temp2054.last_plus_one){
_throw( Null_Exception);}* _temp2056;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2057= yyls; struct Cyc_Yyltype* _temp2059= _temp2057.curr +(
yylsp_offset - 2); if( _temp2059 < _temp2057.base? 1: _temp2059 >= _temp2057.last_plus_one){
_throw( Null_Exception);}* _temp2059;}).first_line,({ struct _tagged_ptr3
_temp2060= yyls; struct Cyc_Yyltype* _temp2062= _temp2060.curr + yylsp_offset;
if( _temp2062 < _temp2060.base? 1: _temp2062 >= _temp2060.last_plus_one){ _throw(
Null_Exception);}* _temp2062;}).last_line))};( struct _xenum_struct*) _temp2046;});
break; case 15: _LL2045: yyval=({ struct Cyc_FnDecl_tok_struct* _temp2064=(
struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));*
_temp2064=( struct Cyc_FnDecl_tok_struct){.tag= Cyc_FnDecl_tok_tag,.f1= Cyc_Parse_make_function(
0, Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp2065= yyvs; struct
_xenum_struct** _temp2067= _temp2065.curr +( yyvsp_offset - 2); if( _temp2067 <
_temp2065.base? 1: _temp2067 >= _temp2065.last_plus_one){ _throw( Null_Exception);}*
_temp2067;})), Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2068= yyvs;
struct _xenum_struct** _temp2070= _temp2068.curr +( yyvsp_offset - 1); if(
_temp2070 < _temp2068.base? 1: _temp2070 >= _temp2068.last_plus_one){ _throw(
Null_Exception);}* _temp2070;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp2071= yyvs; struct _xenum_struct** _temp2073= _temp2071.curr + yyvsp_offset;
if( _temp2073 < _temp2071.base? 1: _temp2073 >= _temp2071.last_plus_one){ _throw(
Null_Exception);}* _temp2073;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2074= yyls; struct Cyc_Yyltype* _temp2076= _temp2074.curr +(
yylsp_offset - 2); if( _temp2076 < _temp2074.base? 1: _temp2076 >= _temp2074.last_plus_one){
_throw( Null_Exception);}* _temp2076;}).first_line,({ struct _tagged_ptr3
_temp2077= yyls; struct Cyc_Yyltype* _temp2079= _temp2077.curr + yylsp_offset;
if( _temp2079 < _temp2077.base? 1: _temp2079 >= _temp2077.last_plus_one){ _throw(
Null_Exception);}* _temp2079;}).last_line))};( struct _xenum_struct*) _temp2064;});
break; case 16: _LL2063: yyval=({ struct Cyc_FnDecl_tok_struct* _temp2081=(
struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));*
_temp2081=( struct Cyc_FnDecl_tok_struct){.tag= Cyc_FnDecl_tok_tag,.f1= Cyc_Parse_make_function(({
struct Cyc_Core_Opt* _temp2082=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2082->v=( void*) Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2083= yyvs; struct _xenum_struct** _temp2085= _temp2083.curr +(
yyvsp_offset - 3); if( _temp2085 < _temp2083.base? 1: _temp2085 >= _temp2083.last_plus_one){
_throw( Null_Exception);}* _temp2085;})); _temp2082;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2086= yyvs; struct _xenum_struct** _temp2088= _temp2086.curr
+( yyvsp_offset - 2); if( _temp2088 < _temp2086.base? 1: _temp2088 >= _temp2086.last_plus_one){
_throw( Null_Exception);}* _temp2088;})), Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp2089= yyvs; struct _xenum_struct** _temp2091= _temp2089.curr +(
yyvsp_offset - 1); if( _temp2091 < _temp2089.base? 1: _temp2091 >= _temp2089.last_plus_one){
_throw( Null_Exception);}* _temp2091;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp2092= yyvs; struct _xenum_struct** _temp2094= _temp2092.curr +
yyvsp_offset; if( _temp2094 < _temp2092.base? 1: _temp2094 >= _temp2092.last_plus_one){
_throw( Null_Exception);}* _temp2094;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2095= yyls; struct Cyc_Yyltype* _temp2097= _temp2095.curr +(
yylsp_offset - 3); if( _temp2097 < _temp2095.base? 1: _temp2097 >= _temp2095.last_plus_one){
_throw( Null_Exception);}* _temp2097;}).first_line,({ struct _tagged_ptr3
_temp2098= yyls; struct Cyc_Yyltype* _temp2100= _temp2098.curr + yylsp_offset;
if( _temp2100 < _temp2098.base? 1: _temp2100 >= _temp2098.last_plus_one){ _throw(
Null_Exception);}* _temp2100;}).last_line))};( struct _xenum_struct*) _temp2081;});
break; case 17: _LL2080: yyval=({ struct Cyc_FnDecl_tok_struct* _temp2102=(
struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));*
_temp2102=( struct Cyc_FnDecl_tok_struct){.tag= Cyc_FnDecl_tok_tag,.f1= Cyc_Parse_make_function(({
struct Cyc_Core_Opt* _temp2103=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2103->v=( void*) Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2104= yyvs; struct _xenum_struct** _temp2106= _temp2104.curr +(
yyvsp_offset - 2); if( _temp2106 < _temp2104.base? 1: _temp2106 >= _temp2104.last_plus_one){
_throw( Null_Exception);}* _temp2106;})); _temp2103;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2107= yyvs; struct _xenum_struct** _temp2109= _temp2107.curr
+( yyvsp_offset - 1); if( _temp2109 < _temp2107.base? 1: _temp2109 >= _temp2107.last_plus_one){
_throw( Null_Exception);}* _temp2109;})), 0, Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp2110= yyvs; struct _xenum_struct** _temp2112= _temp2110.curr +
yyvsp_offset; if( _temp2112 < _temp2110.base? 1: _temp2112 >= _temp2110.last_plus_one){
_throw( Null_Exception);}* _temp2112;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2113= yyls; struct Cyc_Yyltype* _temp2115= _temp2113.curr +(
yylsp_offset - 2); if( _temp2115 < _temp2113.base? 1: _temp2115 >= _temp2113.last_plus_one){
_throw( Null_Exception);}* _temp2115;}).first_line,({ struct _tagged_ptr3
_temp2116= yyls; struct Cyc_Yyltype* _temp2118= _temp2116.curr + yylsp_offset;
if( _temp2118 < _temp2116.base? 1: _temp2118 >= _temp2116.last_plus_one){ _throw(
Null_Exception);}* _temp2118;}).last_line))};( struct _xenum_struct*) _temp2102;});
break; case 18: _LL2101: yyval=({ struct Cyc_FnDecl_tok_struct* _temp2120=(
struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));*
_temp2120=( struct Cyc_FnDecl_tok_struct){.tag= Cyc_FnDecl_tok_tag,.f1= Cyc_Parse_make_function(({
struct Cyc_Core_Opt* _temp2121=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2121->v=( void*) Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2122= yyvs; struct _xenum_struct** _temp2124= _temp2122.curr +(
yyvsp_offset - 3); if( _temp2124 < _temp2122.base? 1: _temp2124 >= _temp2122.last_plus_one){
_throw( Null_Exception);}* _temp2124;})); _temp2121;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2125= yyvs; struct _xenum_struct** _temp2127= _temp2125.curr
+( yyvsp_offset - 2); if( _temp2127 < _temp2125.base? 1: _temp2127 >= _temp2125.last_plus_one){
_throw( Null_Exception);}* _temp2127;})), Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp2128= yyvs; struct _xenum_struct** _temp2130= _temp2128.curr +(
yyvsp_offset - 1); if( _temp2130 < _temp2128.base? 1: _temp2130 >= _temp2128.last_plus_one){
_throw( Null_Exception);}* _temp2130;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp2131= yyvs; struct _xenum_struct** _temp2133= _temp2131.curr +
yyvsp_offset; if( _temp2133 < _temp2131.base? 1: _temp2133 >= _temp2131.last_plus_one){
_throw( Null_Exception);}* _temp2133;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2134= yyls; struct Cyc_Yyltype* _temp2136= _temp2134.curr +(
yylsp_offset - 3); if( _temp2136 < _temp2134.base? 1: _temp2136 >= _temp2134.last_plus_one){
_throw( Null_Exception);}* _temp2136;}).first_line,({ struct _tagged_ptr3
_temp2137= yyls; struct Cyc_Yyltype* _temp2139= _temp2137.curr + yylsp_offset;
if( _temp2139 < _temp2137.base? 1: _temp2139 >= _temp2137.last_plus_one){ _throw(
Null_Exception);}* _temp2139;}).last_line))};( struct _xenum_struct*) _temp2120;});
break; case 19: _LL2119: Cyc_Lex_enter_using( Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp2141= yyvs; struct _xenum_struct** _temp2143= _temp2141.curr +
yyvsp_offset; if( _temp2143 < _temp2141.base? 1: _temp2143 >= _temp2141.last_plus_one){
_throw( Null_Exception);}* _temp2143;}))); yyval=({ struct _tagged_ptr2
_temp2144= yyvs; struct _xenum_struct** _temp2146= _temp2144.curr + yyvsp_offset;
if( _temp2146 < _temp2144.base? 1: _temp2146 >= _temp2144.last_plus_one){ _throw(
Null_Exception);}* _temp2146;}); break; case 20: _LL2140: Cyc_Lex_leave_using();
break; case 21: _LL2147: Cyc_Lex_enter_namespace(({ struct _tagged_string*
_temp2149=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp2149[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2150= yyvs;
struct _xenum_struct** _temp2152= _temp2150.curr + yyvsp_offset; if( _temp2152 <
_temp2150.base? 1: _temp2152 >= _temp2150.last_plus_one){ _throw( Null_Exception);}*
_temp2152;})); _temp2149;})); yyval=({ struct _tagged_ptr2 _temp2153= yyvs;
struct _xenum_struct** _temp2155= _temp2153.curr + yyvsp_offset; if( _temp2155 <
_temp2153.base? 1: _temp2155 >= _temp2153.last_plus_one){ _throw( Null_Exception);}*
_temp2155;}); break; case 22: _LL2148: Cyc_Lex_leave_namespace(); break; case 23:
_LL2156: yyval=({ struct Cyc_DeclList_tok_struct* _temp2158=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));* _temp2158=( struct Cyc_DeclList_tok_struct){.tag=
Cyc_DeclList_tok_tag,.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2159= yyvs; struct _xenum_struct** _temp2161= _temp2159.curr
+( yyvsp_offset - 1); if( _temp2161 < _temp2159.base? 1: _temp2161 >= _temp2159.last_plus_one){
_throw( Null_Exception);}* _temp2161;})), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2162= yyls; struct Cyc_Yyltype* _temp2164= _temp2162.curr
+( yylsp_offset - 1); if( _temp2164 < _temp2162.base? 1: _temp2164 >= _temp2162.last_plus_one){
_throw( Null_Exception);}* _temp2164;}).first_line,({ struct _tagged_ptr3
_temp2165= yyls; struct Cyc_Yyltype* _temp2167= _temp2165.curr +( yylsp_offset -
1); if( _temp2167 < _temp2165.base? 1: _temp2167 >= _temp2165.last_plus_one){
_throw( Null_Exception);}* _temp2167;}).last_line))};( struct _xenum_struct*)
_temp2158;}); break; case 24: _LL2157: yyval=({ struct Cyc_DeclList_tok_struct*
_temp2169=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));*
_temp2169=( struct Cyc_DeclList_tok_struct){.tag= Cyc_DeclList_tok_tag,.f1= Cyc_Parse_make_declarations(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2170= yyvs; struct
_xenum_struct** _temp2172= _temp2170.curr +( yyvsp_offset - 2); if( _temp2172 <
_temp2170.base? 1: _temp2172 >= _temp2170.last_plus_one){ _throw( Null_Exception);}*
_temp2172;})), Cyc_yyget_InitDeclList_tok(({ struct _tagged_ptr2 _temp2173= yyvs;
struct _xenum_struct** _temp2175= _temp2173.curr +( yyvsp_offset - 1); if(
_temp2175 < _temp2173.base? 1: _temp2175 >= _temp2173.last_plus_one){ _throw(
Null_Exception);}* _temp2175;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2176= yyls; struct Cyc_Yyltype* _temp2178= _temp2176.curr +(
yylsp_offset - 2); if( _temp2178 < _temp2176.base? 1: _temp2178 >= _temp2176.last_plus_one){
_throw( Null_Exception);}* _temp2178;}).first_line,({ struct _tagged_ptr3
_temp2179= yyls; struct Cyc_Yyltype* _temp2181= _temp2179.curr +( yylsp_offset -
1); if( _temp2181 < _temp2179.base? 1: _temp2181 >= _temp2179.last_plus_one){
_throw( Null_Exception);}* _temp2181;}).last_line))};( struct _xenum_struct*)
_temp2169;}); break; case 25: _LL2168: yyval=({ struct Cyc_DeclList_tok_struct*
_temp2183=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));*
_temp2183=( struct Cyc_DeclList_tok_struct){.tag= Cyc_DeclList_tok_tag,.f1=({
struct Cyc_List_List* _temp2184=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2184->hd=( void*) Cyc_Absyn_let_decl( Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp2185= yyvs; struct _xenum_struct** _temp2187= _temp2185.curr
+( yyvsp_offset - 3); if( _temp2187 < _temp2185.base? 1: _temp2187 >= _temp2185.last_plus_one){
_throw( Null_Exception);}* _temp2187;})), 0, Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp2188= yyvs; struct _xenum_struct** _temp2190= _temp2188.curr +(
yyvsp_offset - 1); if( _temp2190 < _temp2188.base? 1: _temp2190 >= _temp2188.last_plus_one){
_throw( Null_Exception);}* _temp2190;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2191= yyls; struct Cyc_Yyltype* _temp2193= _temp2191.curr +(
yylsp_offset - 4); if( _temp2193 < _temp2191.base? 1: _temp2193 >= _temp2191.last_plus_one){
_throw( Null_Exception);}* _temp2193;}).first_line,({ struct _tagged_ptr3
_temp2194= yyls; struct Cyc_Yyltype* _temp2196= _temp2194.curr + yylsp_offset;
if( _temp2196 < _temp2194.base? 1: _temp2196 >= _temp2194.last_plus_one){ _throw(
Null_Exception);}* _temp2196;}).last_line)); _temp2184->tl= 0; _temp2184;})};(
struct _xenum_struct*) _temp2183;}); break; case 26: _LL2182: yyval=({ struct
_tagged_ptr2 _temp2198= yyvs; struct _xenum_struct** _temp2200= _temp2198.curr +
yyvsp_offset; if( _temp2200 < _temp2198.base? 1: _temp2200 >= _temp2198.last_plus_one){
_throw( Null_Exception);}* _temp2200;}); break; case 27: _LL2197: yyval=({
struct Cyc_DeclList_tok_struct* _temp2202=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));* _temp2202=( struct Cyc_DeclList_tok_struct){.tag=
Cyc_DeclList_tok_tag,.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp2203= yyvs; struct _xenum_struct** _temp2205= _temp2203.curr +(
yyvsp_offset - 1); if( _temp2205 < _temp2203.base? 1: _temp2205 >= _temp2203.last_plus_one){
_throw( Null_Exception);}* _temp2205;})), Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp2206= yyvs; struct _xenum_struct** _temp2208= _temp2206.curr +
yyvsp_offset; if( _temp2208 < _temp2206.base? 1: _temp2208 >= _temp2206.last_plus_one){
_throw( Null_Exception);}* _temp2208;})))};( struct _xenum_struct*) _temp2202;});
break; case 28: _LL2201: yyval=({ struct Cyc_DeclSpec_tok_struct* _temp2210=(
struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));*
_temp2210=( struct Cyc_DeclSpec_tok_struct){.tag= Cyc_DeclSpec_tok_tag,.f1=({
struct Cyc_Parse_Declaration_spec* _temp2211=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2211->sc=({ struct
Cyc_Core_Opt* _temp2212=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2212->v=( void*) Cyc_yyget_StorageClass_tok(({ struct _tagged_ptr2
_temp2213= yyvs; struct _xenum_struct** _temp2215= _temp2213.curr + yyvsp_offset;
if( _temp2215 < _temp2213.base? 1: _temp2215 >= _temp2213.last_plus_one){ _throw(
Null_Exception);}* _temp2215;})); _temp2212;}); _temp2211->tq= Cyc_Absyn_empty_tqual();
_temp2211->type_specs= 0; _temp2211->is_inline= 0; _temp2211;})};( struct
_xenum_struct*) _temp2210;}); break; case 29: _LL2209: if(( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2217= yyvs; struct _xenum_struct** _temp2219= _temp2217.curr
+ yyvsp_offset; if( _temp2219 < _temp2217.base? 1: _temp2219 >= _temp2217.last_plus_one){
_throw( Null_Exception);}* _temp2219;})))->sc != 0){ Cyc_Parse_warn(( struct
_tagged_string)({ char* _temp2220=( char*)"Only one storage class is allowed in a declaration";
struct _tagged_string _temp2221; _temp2221.curr= _temp2220; _temp2221.base=
_temp2220; _temp2221.last_plus_one= _temp2220 + 51; _temp2221;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2222= yyls; struct Cyc_Yyltype* _temp2224= _temp2222.curr
+( yylsp_offset - 1); if( _temp2224 < _temp2222.base? 1: _temp2224 >= _temp2222.last_plus_one){
_throw( Null_Exception);}* _temp2224;}).first_line,({ struct _tagged_ptr3
_temp2225= yyls; struct Cyc_Yyltype* _temp2227= _temp2225.curr + yylsp_offset;
if( _temp2227 < _temp2225.base? 1: _temp2227 >= _temp2225.last_plus_one){ _throw(
Null_Exception);}* _temp2227;}).last_line));} yyval=({ struct Cyc_DeclSpec_tok_struct*
_temp2228=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));*
_temp2228=( struct Cyc_DeclSpec_tok_struct){.tag= Cyc_DeclSpec_tok_tag,.f1=({
struct Cyc_Parse_Declaration_spec* _temp2229=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2229->sc=({ struct
Cyc_Core_Opt* _temp2236=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2236->v=( void*) Cyc_yyget_StorageClass_tok(({ struct _tagged_ptr2
_temp2237= yyvs; struct _xenum_struct** _temp2239= _temp2237.curr +(
yyvsp_offset - 1); if( _temp2239 < _temp2237.base? 1: _temp2239 >= _temp2237.last_plus_one){
_throw( Null_Exception);}* _temp2239;})); _temp2236;}); _temp2229->tq=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2233= yyvs; struct _xenum_struct** _temp2235= _temp2233.curr
+ yyvsp_offset; if( _temp2235 < _temp2233.base? 1: _temp2235 >= _temp2233.last_plus_one){
_throw( Null_Exception);}* _temp2235;})))->tq; _temp2229->type_specs=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2230= yyvs; struct _xenum_struct** _temp2232= _temp2230.curr
+ yyvsp_offset; if( _temp2232 < _temp2230.base? 1: _temp2232 >= _temp2230.last_plus_one){
_throw( Null_Exception);}* _temp2232;})))->type_specs; _temp2229->is_inline= 0;
_temp2229;})};( struct _xenum_struct*) _temp2228;}); break; case 30: _LL2216:
yyval=({ struct Cyc_DeclSpec_tok_struct* _temp2241=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));* _temp2241=( struct Cyc_DeclSpec_tok_struct){.tag=
Cyc_DeclSpec_tok_tag,.f1=({ struct Cyc_Parse_Declaration_spec* _temp2242=(
struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2242->sc= 0; _temp2242->tq= Cyc_Absyn_empty_tqual(); _temp2242->type_specs=({
struct Cyc_List_List* _temp2243=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2243->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({
struct _tagged_ptr2 _temp2244= yyvs; struct _xenum_struct** _temp2246= _temp2244.curr
+ yyvsp_offset; if( _temp2246 < _temp2244.base? 1: _temp2246 >= _temp2244.last_plus_one){
_throw( Null_Exception);}* _temp2246;})); _temp2243->tl= 0; _temp2243;});
_temp2242->is_inline= 0; _temp2242;})};( struct _xenum_struct*) _temp2241;});
break; case 31: _LL2240: yyval=({ struct Cyc_DeclSpec_tok_struct* _temp2248=(
struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));*
_temp2248=( struct Cyc_DeclSpec_tok_struct){.tag= Cyc_DeclSpec_tok_tag,.f1=({
struct Cyc_Parse_Declaration_spec* _temp2249=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2249->sc=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2260= yyvs; struct _xenum_struct** _temp2262= _temp2260.curr
+ yyvsp_offset; if( _temp2262 < _temp2260.base? 1: _temp2262 >= _temp2260.last_plus_one){
_throw( Null_Exception);}* _temp2262;})))->sc; _temp2249->tq=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2257= yyvs; struct _xenum_struct** _temp2259= _temp2257.curr
+ yyvsp_offset; if( _temp2259 < _temp2257.base? 1: _temp2259 >= _temp2257.last_plus_one){
_throw( Null_Exception);}* _temp2259;})))->tq; _temp2249->type_specs=({ struct
Cyc_List_List* _temp2250=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2250->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({ struct _tagged_ptr2
_temp2254= yyvs; struct _xenum_struct** _temp2256= _temp2254.curr +(
yyvsp_offset - 1); if( _temp2256 < _temp2254.base? 1: _temp2256 >= _temp2254.last_plus_one){
_throw( Null_Exception);}* _temp2256;})); _temp2250->tl=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2251= yyvs; struct _xenum_struct** _temp2253= _temp2251.curr
+ yyvsp_offset; if( _temp2253 < _temp2251.base? 1: _temp2253 >= _temp2251.last_plus_one){
_throw( Null_Exception);}* _temp2253;})))->type_specs; _temp2250;}); _temp2249->is_inline=
0; _temp2249;})};( struct _xenum_struct*) _temp2248;}); break; case 32: _LL2247:
yyval=({ struct Cyc_DeclSpec_tok_struct* _temp2264=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));* _temp2264=( struct Cyc_DeclSpec_tok_struct){.tag=
Cyc_DeclSpec_tok_tag,.f1=({ struct Cyc_Parse_Declaration_spec* _temp2265=(
struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2265->sc= 0; _temp2265->tq= Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2
_temp2266= yyvs; struct _xenum_struct** _temp2268= _temp2266.curr + yyvsp_offset;
if( _temp2268 < _temp2266.base? 1: _temp2268 >= _temp2266.last_plus_one){ _throw(
Null_Exception);}* _temp2268;})); _temp2265->type_specs= 0; _temp2265->is_inline=
0; _temp2265;})};( struct _xenum_struct*) _temp2264;}); break; case 33: _LL2263:
yyval=({ struct Cyc_DeclSpec_tok_struct* _temp2270=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));* _temp2270=( struct Cyc_DeclSpec_tok_struct){.tag=
Cyc_DeclSpec_tok_tag,.f1=({ struct Cyc_Parse_Declaration_spec* _temp2271=(
struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2271->sc=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2281= yyvs;
struct _xenum_struct** _temp2283= _temp2281.curr + yyvsp_offset; if( _temp2283 <
_temp2281.base? 1: _temp2283 >= _temp2281.last_plus_one){ _throw( Null_Exception);}*
_temp2283;})))->sc; _temp2271->tq= Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok(({
struct _tagged_ptr2 _temp2275= yyvs; struct _xenum_struct** _temp2277= _temp2275.curr
+( yyvsp_offset - 1); if( _temp2277 < _temp2275.base? 1: _temp2277 >= _temp2275.last_plus_one){
_throw( Null_Exception);}* _temp2277;})),( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2278= yyvs; struct _xenum_struct** _temp2280= _temp2278.curr +
yyvsp_offset; if( _temp2280 < _temp2278.base? 1: _temp2280 >= _temp2278.last_plus_one){
_throw( Null_Exception);}* _temp2280;})))->tq); _temp2271->type_specs=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2272= yyvs; struct _xenum_struct** _temp2274= _temp2272.curr
+ yyvsp_offset; if( _temp2274 < _temp2272.base? 1: _temp2274 >= _temp2272.last_plus_one){
_throw( Null_Exception);}* _temp2274;})))->type_specs; _temp2271->is_inline= 0;
_temp2271;})};( struct _xenum_struct*) _temp2270;}); break; case 34: _LL2269:
yyval=({ struct Cyc_DeclSpec_tok_struct* _temp2285=( struct Cyc_DeclSpec_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));* _temp2285=( struct Cyc_DeclSpec_tok_struct){.tag=
Cyc_DeclSpec_tok_tag,.f1=({ struct Cyc_Parse_Declaration_spec* _temp2286=(
struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2286->sc= 0; _temp2286->tq= Cyc_Absyn_empty_tqual(); _temp2286->type_specs=
0; _temp2286->is_inline= 1; _temp2286;})};( struct _xenum_struct*) _temp2285;});
break; case 35: _LL2284: yyval=({ struct Cyc_DeclSpec_tok_struct* _temp2288=(
struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));*
_temp2288=( struct Cyc_DeclSpec_tok_struct){.tag= Cyc_DeclSpec_tok_tag,.f1=({
struct Cyc_Parse_Declaration_spec* _temp2289=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2289->sc=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2296= yyvs; struct _xenum_struct** _temp2298= _temp2296.curr
+ yyvsp_offset; if( _temp2298 < _temp2296.base? 1: _temp2298 >= _temp2296.last_plus_one){
_throw( Null_Exception);}* _temp2298;})))->sc; _temp2289->tq=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2293= yyvs; struct _xenum_struct** _temp2295= _temp2293.curr
+ yyvsp_offset; if( _temp2295 < _temp2293.base? 1: _temp2295 >= _temp2293.last_plus_one){
_throw( Null_Exception);}* _temp2295;})))->tq; _temp2289->type_specs=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2290= yyvs; struct _xenum_struct** _temp2292= _temp2290.curr
+ yyvsp_offset; if( _temp2292 < _temp2290.base? 1: _temp2292 >= _temp2290.last_plus_one){
_throw( Null_Exception);}* _temp2292;})))->type_specs; _temp2289->is_inline= 1;
_temp2289;})};( struct _xenum_struct*) _temp2288;}); break; case 36: _LL2287:
yyval=({ struct Cyc_StorageClass_tok_struct* _temp2300=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));* _temp2300=( struct Cyc_StorageClass_tok_struct){.tag=
Cyc_StorageClass_tok_tag,.f1= Cyc_Parse_Auto_sc};( struct _xenum_struct*)
_temp2300;}); break; case 37: _LL2299: yyval=({ struct Cyc_StorageClass_tok_struct*
_temp2302=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));*
_temp2302=( struct Cyc_StorageClass_tok_struct){.tag= Cyc_StorageClass_tok_tag,.f1=
Cyc_Parse_Register_sc};( struct _xenum_struct*) _temp2302;}); break; case 38:
_LL2301: yyval=({ struct Cyc_StorageClass_tok_struct* _temp2304=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));* _temp2304=( struct Cyc_StorageClass_tok_struct){.tag=
Cyc_StorageClass_tok_tag,.f1= Cyc_Parse_Static_sc};( struct _xenum_struct*)
_temp2304;}); break; case 39: _LL2303: yyval=({ struct Cyc_StorageClass_tok_struct*
_temp2306=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));*
_temp2306=( struct Cyc_StorageClass_tok_struct){.tag= Cyc_StorageClass_tok_tag,.f1=
Cyc_Parse_Extern_sc};( struct _xenum_struct*) _temp2306;}); break; case 40:
_LL2305: if( Cyc_String_strcmp( Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp2308= yyvs; struct _xenum_struct** _temp2310= _temp2308.curr + yyvsp_offset;
if( _temp2310 < _temp2308.base? 1: _temp2310 >= _temp2308.last_plus_one){ _throw(
Null_Exception);}* _temp2310;})),( struct _tagged_string)({ char* _temp2311=(
char*)"C"; struct _tagged_string _temp2312; _temp2312.curr= _temp2311; _temp2312.base=
_temp2311; _temp2312.last_plus_one= _temp2311 + 2; _temp2312;})) != 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp2313=( char*)"only extern or extern \"C\" is allowed";
struct _tagged_string _temp2314; _temp2314.curr= _temp2313; _temp2314.base=
_temp2313; _temp2314.last_plus_one= _temp2313 + 37; _temp2314;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2315= yyls; struct Cyc_Yyltype* _temp2317= _temp2315.curr
+( yylsp_offset - 1); if( _temp2317 < _temp2315.base? 1: _temp2317 >= _temp2315.last_plus_one){
_throw( Null_Exception);}* _temp2317;}).first_line,({ struct _tagged_ptr3
_temp2318= yyls; struct Cyc_Yyltype* _temp2320= _temp2318.curr + yylsp_offset;
if( _temp2320 < _temp2318.base? 1: _temp2320 >= _temp2318.last_plus_one){ _throw(
Null_Exception);}* _temp2320;}).last_line));} yyval=({ struct Cyc_StorageClass_tok_struct*
_temp2321=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));*
_temp2321=( struct Cyc_StorageClass_tok_struct){.tag= Cyc_StorageClass_tok_tag,.f1=
Cyc_Parse_ExternC_sc};( struct _xenum_struct*) _temp2321;}); break; case 41:
_LL2307: yyval=({ struct Cyc_StorageClass_tok_struct* _temp2323=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));* _temp2323=( struct Cyc_StorageClass_tok_struct){.tag=
Cyc_StorageClass_tok_tag,.f1= Cyc_Parse_Typedef_sc};( struct _xenum_struct*)
_temp2323;}); break; case 42: _LL2322: yyval=({ struct Cyc_StorageClass_tok_struct*
_temp2325=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));*
_temp2325=( struct Cyc_StorageClass_tok_struct){.tag= Cyc_StorageClass_tok_tag,.f1=
Cyc_Parse_Abstract_sc};( struct _xenum_struct*) _temp2325;}); break; case 43:
_LL2324: yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2327=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));* _temp2327=( struct
Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1= Cyc_Parse_type_spec(
Cyc_Absyn_VoidType, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2328=
yyls; struct Cyc_Yyltype* _temp2330= _temp2328.curr + yylsp_offset; if(
_temp2330 < _temp2328.base? 1: _temp2330 >= _temp2328.last_plus_one){ _throw(
Null_Exception);}* _temp2330;}).first_line,({ struct _tagged_ptr3 _temp2331=
yyls; struct Cyc_Yyltype* _temp2333= _temp2331.curr + yylsp_offset; if(
_temp2333 < _temp2331.base? 1: _temp2333 >= _temp2331.last_plus_one){ _throw(
Null_Exception);}* _temp2333;}).last_line))};( struct _xenum_struct*) _temp2327;});
break; case 44: _LL2326: yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2335=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2335=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec( Cyc_Absyn_new_evar( Cyc_Absyn_MemKind), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2336= yyls; struct Cyc_Yyltype* _temp2338= _temp2336.curr
+ yylsp_offset; if( _temp2338 < _temp2336.base? 1: _temp2338 >= _temp2336.last_plus_one){
_throw( Null_Exception);}* _temp2338;}).first_line,({ struct _tagged_ptr3
_temp2339= yyls; struct Cyc_Yyltype* _temp2341= _temp2339.curr + yylsp_offset;
if( _temp2341 < _temp2339.base? 1: _temp2341 >= _temp2339.last_plus_one){ _throw(
Null_Exception);}* _temp2341;}).last_line))};( struct _xenum_struct*) _temp2335;});
break; case 45: _LL2334: yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2343=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2343=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec( Cyc_Absyn_uchar_t, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2344= yyls; struct Cyc_Yyltype* _temp2346= _temp2344.curr +
yylsp_offset; if( _temp2346 < _temp2344.base? 1: _temp2346 >= _temp2344.last_plus_one){
_throw( Null_Exception);}* _temp2346;}).first_line,({ struct _tagged_ptr3
_temp2347= yyls; struct Cyc_Yyltype* _temp2349= _temp2347.curr + yylsp_offset;
if( _temp2349 < _temp2347.base? 1: _temp2349 >= _temp2347.last_plus_one){ _throw(
Null_Exception);}* _temp2349;}).last_line))};( struct _xenum_struct*) _temp2343;});
break; case 46: _LL2342: yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2351=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2351=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=({
struct Cyc_Parse_Short_spec_struct* _temp2352=( struct Cyc_Parse_Short_spec_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Short_spec_struct)); _temp2352->tag= Cyc_Parse_Short_spec_tag;
_temp2352->f1= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2353=
yyls; struct Cyc_Yyltype* _temp2355= _temp2353.curr + yylsp_offset; if(
_temp2355 < _temp2353.base? 1: _temp2355 >= _temp2353.last_plus_one){ _throw(
Null_Exception);}* _temp2355;}).first_line,({ struct _tagged_ptr3 _temp2356=
yyls; struct Cyc_Yyltype* _temp2358= _temp2356.curr + yylsp_offset; if(
_temp2358 < _temp2356.base? 1: _temp2358 >= _temp2356.last_plus_one){ _throw(
Null_Exception);}* _temp2358;}).last_line);( void*) _temp2352;})};( struct
_xenum_struct*) _temp2351;}); break; case 47: _LL2350: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp2360=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2360=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec( Cyc_Absyn_sint_t, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2361= yyls; struct Cyc_Yyltype* _temp2363= _temp2361.curr +
yylsp_offset; if( _temp2363 < _temp2361.base? 1: _temp2363 >= _temp2361.last_plus_one){
_throw( Null_Exception);}* _temp2363;}).first_line,({ struct _tagged_ptr3
_temp2364= yyls; struct Cyc_Yyltype* _temp2366= _temp2364.curr + yylsp_offset;
if( _temp2366 < _temp2364.base? 1: _temp2366 >= _temp2364.last_plus_one){ _throw(
Null_Exception);}* _temp2366;}).last_line))};( struct _xenum_struct*) _temp2360;});
break; case 48: _LL2359: yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2368=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2368=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=({
struct Cyc_Parse_Long_spec_struct* _temp2369=( struct Cyc_Parse_Long_spec_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Long_spec_struct)); _temp2369->tag= Cyc_Parse_Long_spec_tag;
_temp2369->f1= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2370=
yyls; struct Cyc_Yyltype* _temp2372= _temp2370.curr + yylsp_offset; if(
_temp2372 < _temp2370.base? 1: _temp2372 >= _temp2370.last_plus_one){ _throw(
Null_Exception);}* _temp2372;}).first_line,({ struct _tagged_ptr3 _temp2373=
yyls; struct Cyc_Yyltype* _temp2375= _temp2373.curr + yylsp_offset; if(
_temp2375 < _temp2373.base? 1: _temp2375 >= _temp2373.last_plus_one){ _throw(
Null_Exception);}* _temp2375;}).last_line);( void*) _temp2369;})};( struct
_xenum_struct*) _temp2368;}); break; case 49: _LL2367: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp2377=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2377=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec( Cyc_Absyn_float_t, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2378= yyls; struct Cyc_Yyltype* _temp2380= _temp2378.curr +
yylsp_offset; if( _temp2380 < _temp2378.base? 1: _temp2380 >= _temp2378.last_plus_one){
_throw( Null_Exception);}* _temp2380;}).first_line,({ struct _tagged_ptr3
_temp2381= yyls; struct Cyc_Yyltype* _temp2383= _temp2381.curr + yylsp_offset;
if( _temp2383 < _temp2381.base? 1: _temp2383 >= _temp2381.last_plus_one){ _throw(
Null_Exception);}* _temp2383;}).last_line))};( struct _xenum_struct*) _temp2377;});
break; case 50: _LL2376: yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2385=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2385=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec( Cyc_Absyn_double_t, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2386= yyls; struct Cyc_Yyltype* _temp2388= _temp2386.curr +
yylsp_offset; if( _temp2388 < _temp2386.base? 1: _temp2388 >= _temp2386.last_plus_one){
_throw( Null_Exception);}* _temp2388;}).first_line,({ struct _tagged_ptr3
_temp2389= yyls; struct Cyc_Yyltype* _temp2391= _temp2389.curr + yylsp_offset;
if( _temp2391 < _temp2389.base? 1: _temp2391 >= _temp2389.last_plus_one){ _throw(
Null_Exception);}* _temp2391;}).last_line))};( struct _xenum_struct*) _temp2385;});
break; case 51: _LL2384: yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2393=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2393=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=({
struct Cyc_Parse_Signed_spec_struct* _temp2394=( struct Cyc_Parse_Signed_spec_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Signed_spec_struct)); _temp2394->tag= Cyc_Parse_Signed_spec_tag;
_temp2394->f1= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2395=
yyls; struct Cyc_Yyltype* _temp2397= _temp2395.curr + yylsp_offset; if(
_temp2397 < _temp2395.base? 1: _temp2397 >= _temp2395.last_plus_one){ _throw(
Null_Exception);}* _temp2397;}).first_line,({ struct _tagged_ptr3 _temp2398=
yyls; struct Cyc_Yyltype* _temp2400= _temp2398.curr + yylsp_offset; if(
_temp2400 < _temp2398.base? 1: _temp2400 >= _temp2398.last_plus_one){ _throw(
Null_Exception);}* _temp2400;}).last_line);( void*) _temp2394;})};( struct
_xenum_struct*) _temp2393;}); break; case 52: _LL2392: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp2402=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2402=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=({
struct Cyc_Parse_Unsigned_spec_struct* _temp2403=( struct Cyc_Parse_Unsigned_spec_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Unsigned_spec_struct)); _temp2403->tag= Cyc_Parse_Unsigned_spec_tag;
_temp2403->f1= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2404=
yyls; struct Cyc_Yyltype* _temp2406= _temp2404.curr + yylsp_offset; if(
_temp2406 < _temp2404.base? 1: _temp2406 >= _temp2404.last_plus_one){ _throw(
Null_Exception);}* _temp2406;}).first_line,({ struct _tagged_ptr3 _temp2407=
yyls; struct Cyc_Yyltype* _temp2409= _temp2407.curr + yylsp_offset; if(
_temp2409 < _temp2407.base? 1: _temp2409 >= _temp2407.last_plus_one){ _throw(
Null_Exception);}* _temp2409;}).last_line);( void*) _temp2403;})};( struct
_xenum_struct*) _temp2402;}); break; case 53: _LL2401: yyval=({ struct
_tagged_ptr2 _temp2411= yyvs; struct _xenum_struct** _temp2413= _temp2411.curr +
yyvsp_offset; if( _temp2413 < _temp2411.base? 1: _temp2413 >= _temp2411.last_plus_one){
_throw( Null_Exception);}* _temp2413;}); break; case 54: _LL2410: yyval=({
struct _tagged_ptr2 _temp2415= yyvs; struct _xenum_struct** _temp2417= _temp2415.curr
+ yyvsp_offset; if( _temp2417 < _temp2415.base? 1: _temp2417 >= _temp2415.last_plus_one){
_throw( Null_Exception);}* _temp2417;}); break; case 55: _LL2414: yyval=({
struct Cyc_TypeSpecifier_tok_struct* _temp2419=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));* _temp2419=( struct
Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1= Cyc_Parse_type_spec(
Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2420= yyvs;
struct _xenum_struct** _temp2422= _temp2420.curr + yyvsp_offset; if( _temp2422 <
_temp2420.base? 1: _temp2422 >= _temp2420.last_plus_one){ _throw( Null_Exception);}*
_temp2422;})),(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)()), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2423= yyls; struct Cyc_Yyltype* _temp2425= _temp2423.curr
+ yylsp_offset; if( _temp2425 < _temp2423.base? 1: _temp2425 >= _temp2423.last_plus_one){
_throw( Null_Exception);}* _temp2425;}).first_line,({ struct _tagged_ptr3
_temp2426= yyls; struct Cyc_Yyltype* _temp2428= _temp2426.curr + yylsp_offset;
if( _temp2428 < _temp2426.base? 1: _temp2428 >= _temp2426.last_plus_one){ _throw(
Null_Exception);}* _temp2428;}).last_line))};( struct _xenum_struct*) _temp2419;});
break; case 56: _LL2418: yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2430=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2430=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec( Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp2431= yyvs; struct _xenum_struct** _temp2433= _temp2431.curr +(
yyvsp_offset - 2); if( _temp2433 < _temp2431.base? 1: _temp2433 >= _temp2431.last_plus_one){
_throw( Null_Exception);}* _temp2433;})),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)( Cyc_yyget_Kind_tok(({ struct _tagged_ptr2 _temp2434= yyvs;
struct _xenum_struct** _temp2436= _temp2434.curr + yyvsp_offset; if( _temp2436 <
_temp2434.base? 1: _temp2436 >= _temp2434.last_plus_one){ _throw( Null_Exception);}*
_temp2436;})))), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2437=
yyls; struct Cyc_Yyltype* _temp2439= _temp2437.curr +( yylsp_offset - 2); if(
_temp2439 < _temp2437.base? 1: _temp2439 >= _temp2437.last_plus_one){ _throw(
Null_Exception);}* _temp2439;}).first_line,({ struct _tagged_ptr3 _temp2440=
yyls; struct Cyc_Yyltype* _temp2442= _temp2440.curr + yylsp_offset; if(
_temp2442 < _temp2440.base? 1: _temp2442 >= _temp2440.last_plus_one){ _throw(
Null_Exception);}* _temp2442;}).last_line))};( struct _xenum_struct*) _temp2430;});
break; case 57: _LL2429: yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2444=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2444=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec(({ struct Cyc_Absyn_TypedefType_struct* _temp2445=( struct
Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp2445->tag= Cyc_Absyn_TypedefType_tag; _temp2445->f1= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp2446= yyvs; struct _xenum_struct** _temp2448= _temp2446.curr
+( yyvsp_offset - 1); if( _temp2448 < _temp2446.base? 1: _temp2448 >= _temp2446.last_plus_one){
_throw( Null_Exception);}* _temp2448;})); _temp2445->f2= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp2449= yyvs; struct _xenum_struct** _temp2451= _temp2449.curr
+ yyvsp_offset; if( _temp2451 < _temp2449.base? 1: _temp2451 >= _temp2449.last_plus_one){
_throw( Null_Exception);}* _temp2451;})); _temp2445->f3= 0;( void*) _temp2445;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2452= yyls; struct Cyc_Yyltype*
_temp2454= _temp2452.curr +( yylsp_offset - 1); if( _temp2454 < _temp2452.base?
1: _temp2454 >= _temp2452.last_plus_one){ _throw( Null_Exception);}* _temp2454;}).first_line,({
struct _tagged_ptr3 _temp2455= yyls; struct Cyc_Yyltype* _temp2457= _temp2455.curr
+ yylsp_offset; if( _temp2457 < _temp2455.base? 1: _temp2457 >= _temp2455.last_plus_one){
_throw( Null_Exception);}* _temp2457;}).last_line))};( struct _xenum_struct*)
_temp2444;}); break; case 58: _LL2443: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp2459=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2459=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec(({ struct Cyc_Absyn_TupleType_struct* _temp2460=( struct Cyc_Absyn_TupleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp2460->tag= Cyc_Absyn_TupleType_tag;
_temp2460->f1=(( struct Cyc_List_List*(*)( struct _tuple9*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2461= yyls; struct Cyc_Yyltype* _temp2463= _temp2461.curr +(
yylsp_offset - 1); if( _temp2463 < _temp2461.base? 1: _temp2463 >= _temp2461.last_plus_one){
_throw( Null_Exception);}* _temp2463;}).first_line,({ struct _tagged_ptr3
_temp2464= yyls; struct Cyc_Yyltype* _temp2466= _temp2464.curr +( yylsp_offset -
1); if( _temp2466 < _temp2464.base? 1: _temp2466 >= _temp2464.last_plus_one){
_throw( Null_Exception);}* _temp2466;}).last_line),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp2467= yyvs; struct _xenum_struct** _temp2469= _temp2467.curr
+( yyvsp_offset - 1); if( _temp2469 < _temp2467.base? 1: _temp2469 >= _temp2467.last_plus_one){
_throw( Null_Exception);}* _temp2469;}))));( void*) _temp2460;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2470= yyls; struct Cyc_Yyltype* _temp2472= _temp2470.curr
+( yylsp_offset - 3); if( _temp2472 < _temp2470.base? 1: _temp2472 >= _temp2470.last_plus_one){
_throw( Null_Exception);}* _temp2472;}).first_line,({ struct _tagged_ptr3
_temp2473= yyls; struct Cyc_Yyltype* _temp2475= _temp2473.curr + yylsp_offset;
if( _temp2475 < _temp2473.base? 1: _temp2475 >= _temp2473.last_plus_one){ _throw(
Null_Exception);}* _temp2475;}).last_line))};( struct _xenum_struct*) _temp2459;});
break; case 59: _LL2458: yyval=({ struct Cyc_Kind_tok_struct* _temp2477=( struct
Cyc_Kind_tok_struct*) GC_malloc( sizeof( struct Cyc_Kind_tok_struct));*
_temp2477=( struct Cyc_Kind_tok_struct){.tag= Cyc_Kind_tok_tag,.f1= Cyc_Parse_id_to_kind(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2478= yyvs; struct
_xenum_struct** _temp2480= _temp2478.curr + yyvsp_offset; if( _temp2480 <
_temp2478.base? 1: _temp2480 >= _temp2478.last_plus_one){ _throw( Null_Exception);}*
_temp2480;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2481=
yyls; struct Cyc_Yyltype* _temp2483= _temp2481.curr + yylsp_offset; if(
_temp2483 < _temp2481.base? 1: _temp2483 >= _temp2481.last_plus_one){ _throw(
Null_Exception);}* _temp2483;}).first_line,({ struct _tagged_ptr3 _temp2484=
yyls; struct Cyc_Yyltype* _temp2486= _temp2484.curr + yylsp_offset; if(
_temp2486 < _temp2484.base? 1: _temp2486 >= _temp2484.last_plus_one){ _throw(
Null_Exception);}* _temp2486;}).last_line))};( struct _xenum_struct*) _temp2477;});
break; case 60: _LL2476: { struct _tagged_string* _temp2493; void* _temp2495;
struct _tuple1 _temp2491=* Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp2488=
yyvs; struct _xenum_struct** _temp2490= _temp2488.curr + yyvsp_offset; if(
_temp2490 < _temp2488.base? 1: _temp2490 >= _temp2488.last_plus_one){ _throw(
Null_Exception);}* _temp2490;})); _LL2496: _temp2495= _temp2491.f1; goto _LL2494;
_LL2494: _temp2493= _temp2491.f2; goto _LL2492; _LL2492: if( _temp2495 != Cyc_Absyn_Loc_n){
Cyc_Parse_err(( struct _tagged_string)({ char* _temp2497=( char*)"bad kind in type specifier";
struct _tagged_string _temp2498; _temp2498.curr= _temp2497; _temp2498.base=
_temp2497; _temp2498.last_plus_one= _temp2497 + 27; _temp2498;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2499= yyls; struct Cyc_Yyltype* _temp2501= _temp2499.curr
+ yylsp_offset; if( _temp2501 < _temp2499.base? 1: _temp2501 >= _temp2499.last_plus_one){
_throw( Null_Exception);}* _temp2501;}).first_line,({ struct _tagged_ptr3
_temp2502= yyls; struct Cyc_Yyltype* _temp2504= _temp2502.curr + yylsp_offset;
if( _temp2504 < _temp2502.base? 1: _temp2504 >= _temp2502.last_plus_one){ _throw(
Null_Exception);}* _temp2504;}).last_line));} yyval=({ struct Cyc_Kind_tok_struct*
_temp2505=( struct Cyc_Kind_tok_struct*) GC_malloc( sizeof( struct Cyc_Kind_tok_struct));*
_temp2505=( struct Cyc_Kind_tok_struct){.tag= Cyc_Kind_tok_tag,.f1= Cyc_Parse_id_to_kind(*
_temp2493, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2506= yyls;
struct Cyc_Yyltype* _temp2508= _temp2506.curr + yylsp_offset; if( _temp2508 <
_temp2506.base? 1: _temp2508 >= _temp2506.last_plus_one){ _throw( Null_Exception);}*
_temp2508;}).first_line,({ struct _tagged_ptr3 _temp2509= yyls; struct Cyc_Yyltype*
_temp2511= _temp2509.curr + yylsp_offset; if( _temp2511 < _temp2509.base? 1:
_temp2511 >= _temp2509.last_plus_one){ _throw( Null_Exception);}* _temp2511;}).last_line))};(
struct _xenum_struct*) _temp2505;}); break;} case 61: _LL2487: yyval=({ struct
Cyc_TypeQual_tok_struct* _temp2513=( struct Cyc_TypeQual_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeQual_tok_struct));* _temp2513=( struct Cyc_TypeQual_tok_struct){.tag=
Cyc_TypeQual_tok_tag,.f1=({ struct Cyc_Absyn_Tqual* _temp2514=( struct Cyc_Absyn_Tqual*)
GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual)); _temp2514->q_const= 1;
_temp2514->q_volatile= 0; _temp2514->q_restrict= 0; _temp2514;})};( struct
_xenum_struct*) _temp2513;}); break; case 62: _LL2512: yyval=({ struct Cyc_TypeQual_tok_struct*
_temp2516=( struct Cyc_TypeQual_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct));*
_temp2516=( struct Cyc_TypeQual_tok_struct){.tag= Cyc_TypeQual_tok_tag,.f1=({
struct Cyc_Absyn_Tqual* _temp2517=( struct Cyc_Absyn_Tqual*) GC_malloc_atomic(
sizeof( struct Cyc_Absyn_Tqual)); _temp2517->q_const= 0; _temp2517->q_volatile=
1; _temp2517->q_restrict= 0; _temp2517;})};( struct _xenum_struct*) _temp2516;});
break; case 63: _LL2515: yyval=({ struct Cyc_TypeQual_tok_struct* _temp2519=(
struct Cyc_TypeQual_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct));*
_temp2519=( struct Cyc_TypeQual_tok_struct){.tag= Cyc_TypeQual_tok_tag,.f1=({
struct Cyc_Absyn_Tqual* _temp2520=( struct Cyc_Absyn_Tqual*) GC_malloc_atomic(
sizeof( struct Cyc_Absyn_Tqual)); _temp2520->q_const= 0; _temp2520->q_volatile=
0; _temp2520->q_restrict= 1; _temp2520;})};( struct _xenum_struct*) _temp2519;});
break; case 64: _LL2518: { struct Cyc_Absyn_Decl* d;{ void* _temp2525= Cyc_yyget_StructOrUnion_tok(({
struct _tagged_ptr2 _temp2522= yyvs; struct _xenum_struct** _temp2524= _temp2522.curr
+( yyvsp_offset - 3); if( _temp2524 < _temp2522.base? 1: _temp2524 >= _temp2522.last_plus_one){
_throw( Null_Exception);}* _temp2524;})); _LL2527: if( _temp2525 == Cyc_Parse_Struct_su){
goto _LL2528;} else{ goto _LL2529;} _LL2529: if( _temp2525 == Cyc_Parse_Union_su){
goto _LL2530;} else{ goto _LL2526;} _LL2528: d= Cyc_Absyn_struct_decl( Cyc_Absyn_Public,
0, 0,({ struct Cyc_Core_Opt* _temp2531=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp2531->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp2532= yyvs; struct _xenum_struct** _temp2534= _temp2532.curr
+( yyvsp_offset - 1); if( _temp2534 < _temp2532.base? 1: _temp2534 >= _temp2532.last_plus_one){
_throw( Null_Exception);}* _temp2534;})); _temp2531;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2535= yyls; struct Cyc_Yyltype* _temp2537= _temp2535.curr
+( yylsp_offset - 3); if( _temp2537 < _temp2535.base? 1: _temp2537 >= _temp2535.last_plus_one){
_throw( Null_Exception);}* _temp2537;}).first_line,({ struct _tagged_ptr3
_temp2538= yyls; struct Cyc_Yyltype* _temp2540= _temp2538.curr + yylsp_offset;
if( _temp2540 < _temp2538.base? 1: _temp2540 >= _temp2538.last_plus_one){ _throw(
Null_Exception);}* _temp2540;}).last_line)); goto _LL2526; _LL2530: Cyc_Parse_unimp2((
struct _tagged_string)({ char* _temp2541=( char*)"unions"; struct _tagged_string
_temp2542; _temp2542.curr= _temp2541; _temp2542.base= _temp2541; _temp2542.last_plus_one=
_temp2541 + 7; _temp2542;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2543= yyls; struct Cyc_Yyltype* _temp2545= _temp2543.curr +( yylsp_offset -
3); if( _temp2545 < _temp2543.base? 1: _temp2545 >= _temp2543.last_plus_one){
_throw( Null_Exception);}* _temp2545;}).first_line,({ struct _tagged_ptr3
_temp2546= yyls; struct Cyc_Yyltype* _temp2548= _temp2546.curr + yylsp_offset;
if( _temp2548 < _temp2546.base? 1: _temp2548 >= _temp2546.last_plus_one){ _throw(
Null_Exception);}* _temp2548;}).last_line)); d= Cyc_Absyn_new_decl( Cyc_Absyn_Union_d,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2549= yyls; struct Cyc_Yyltype*
_temp2551= _temp2549.curr +( yylsp_offset - 3); if( _temp2551 < _temp2549.base?
1: _temp2551 >= _temp2549.last_plus_one){ _throw( Null_Exception);}* _temp2551;}).first_line,({
struct _tagged_ptr3 _temp2552= yyls; struct Cyc_Yyltype* _temp2554= _temp2552.curr
+ yylsp_offset; if( _temp2554 < _temp2552.base? 1: _temp2554 >= _temp2552.last_plus_one){
_throw( Null_Exception);}* _temp2554;}).last_line)); goto _LL2526; _LL2526:;}
yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2555=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));* _temp2555=( struct
Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=({ struct Cyc_Parse_Decl_spec_struct*
_temp2556=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2556->tag= Cyc_Parse_Decl_spec_tag; _temp2556->f1= d;( void*) _temp2556;})};(
struct _xenum_struct*) _temp2555;}); Cyc_Parse_unimp2(( struct _tagged_string)({
char* _temp2557=( char*)"anonymous structs/unions"; struct _tagged_string
_temp2558; _temp2558.curr= _temp2557; _temp2558.base= _temp2557; _temp2558.last_plus_one=
_temp2557 + 25; _temp2558;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2559= yyls; struct Cyc_Yyltype* _temp2561= _temp2559.curr +( yylsp_offset -
3); if( _temp2561 < _temp2559.base? 1: _temp2561 >= _temp2559.last_plus_one){
_throw( Null_Exception);}* _temp2561;}).first_line,({ struct _tagged_ptr3
_temp2562= yyls; struct Cyc_Yyltype* _temp2564= _temp2562.curr + yylsp_offset;
if( _temp2564 < _temp2562.base? 1: _temp2564 >= _temp2562.last_plus_one){ _throw(
Null_Exception);}* _temp2564;}).last_line)); break;} case 65: _LL2521: { struct
Cyc_List_List* ts=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2566= yyls; struct Cyc_Yyltype* _temp2568= _temp2566.curr
+( yylsp_offset - 3); if( _temp2568 < _temp2566.base? 1: _temp2568 >= _temp2566.last_plus_one){
_throw( Null_Exception);}* _temp2568;}).first_line,({ struct _tagged_ptr3
_temp2569= yyls; struct Cyc_Yyltype* _temp2571= _temp2569.curr +( yylsp_offset -
3); if( _temp2571 < _temp2569.base? 1: _temp2571 >= _temp2569.last_plus_one){
_throw( Null_Exception);}* _temp2571;}).last_line), Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp2572= yyvs; struct _xenum_struct** _temp2574= _temp2572.curr
+( yyvsp_offset - 3); if( _temp2574 < _temp2572.base? 1: _temp2574 >= _temp2572.last_plus_one){
_throw( Null_Exception);}* _temp2574;}))); struct Cyc_Absyn_Decl* d;{ void*
_temp2578= Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2 _temp2575= yyvs;
struct _xenum_struct** _temp2577= _temp2575.curr +( yyvsp_offset - 5); if(
_temp2577 < _temp2575.base? 1: _temp2577 >= _temp2575.last_plus_one){ _throw(
Null_Exception);}* _temp2577;})); _LL2580: if( _temp2578 == Cyc_Parse_Struct_su){
goto _LL2581;} else{ goto _LL2582;} _LL2582: if( _temp2578 == Cyc_Parse_Union_su){
goto _LL2583;} else{ goto _LL2579;} _LL2581: d= Cyc_Absyn_struct_decl( Cyc_Absyn_Public,({
struct Cyc_Core_Opt* _temp2584=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2584->v=( void*) Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp2585= yyvs; struct _xenum_struct** _temp2587= _temp2585.curr +(
yyvsp_offset - 4); if( _temp2587 < _temp2585.base? 1: _temp2587 >= _temp2585.last_plus_one){
_throw( Null_Exception);}* _temp2587;})); _temp2584;}), ts,({ struct Cyc_Core_Opt*
_temp2588=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2588->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({ struct _tagged_ptr2
_temp2589= yyvs; struct _xenum_struct** _temp2591= _temp2589.curr +(
yyvsp_offset - 1); if( _temp2591 < _temp2589.base? 1: _temp2591 >= _temp2589.last_plus_one){
_throw( Null_Exception);}* _temp2591;})); _temp2588;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2592= yyls; struct Cyc_Yyltype* _temp2594= _temp2592.curr
+( yylsp_offset - 5); if( _temp2594 < _temp2592.base? 1: _temp2594 >= _temp2592.last_plus_one){
_throw( Null_Exception);}* _temp2594;}).first_line,({ struct _tagged_ptr3
_temp2595= yyls; struct Cyc_Yyltype* _temp2597= _temp2595.curr + yylsp_offset;
if( _temp2597 < _temp2595.base? 1: _temp2597 >= _temp2595.last_plus_one){ _throw(
Null_Exception);}* _temp2597;}).last_line)); goto _LL2579; _LL2583: Cyc_Parse_unimp2((
struct _tagged_string)({ char* _temp2598=( char*)"unions"; struct _tagged_string
_temp2599; _temp2599.curr= _temp2598; _temp2599.base= _temp2598; _temp2599.last_plus_one=
_temp2598 + 7; _temp2599;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2600= yyls; struct Cyc_Yyltype* _temp2602= _temp2600.curr +( yylsp_offset -
5); if( _temp2602 < _temp2600.base? 1: _temp2602 >= _temp2600.last_plus_one){
_throw( Null_Exception);}* _temp2602;}).first_line,({ struct _tagged_ptr3
_temp2603= yyls; struct Cyc_Yyltype* _temp2605= _temp2603.curr + yylsp_offset;
if( _temp2605 < _temp2603.base? 1: _temp2605 >= _temp2603.last_plus_one){ _throw(
Null_Exception);}* _temp2605;}).last_line)); d= Cyc_Absyn_new_decl( Cyc_Absyn_Union_d,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2606= yyls; struct Cyc_Yyltype*
_temp2608= _temp2606.curr +( yylsp_offset - 5); if( _temp2608 < _temp2606.base?
1: _temp2608 >= _temp2606.last_plus_one){ _throw( Null_Exception);}* _temp2608;}).first_line,({
struct _tagged_ptr3 _temp2609= yyls; struct Cyc_Yyltype* _temp2611= _temp2609.curr
+ yylsp_offset; if( _temp2611 < _temp2609.base? 1: _temp2611 >= _temp2609.last_plus_one){
_throw( Null_Exception);}* _temp2611;}).last_line)); goto _LL2579; _LL2579:;}
yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2612=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));* _temp2612=( struct
Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=({ struct Cyc_Parse_Decl_spec_struct*
_temp2613=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2613->tag= Cyc_Parse_Decl_spec_tag; _temp2613->f1= d;( void*) _temp2613;})};(
struct _xenum_struct*) _temp2612;}); break;} case 66: _LL2565: { struct Cyc_List_List*
ts=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2615=
yyls; struct Cyc_Yyltype* _temp2617= _temp2615.curr +( yylsp_offset - 3); if(
_temp2617 < _temp2615.base? 1: _temp2617 >= _temp2615.last_plus_one){ _throw(
Null_Exception);}* _temp2617;}).first_line,({ struct _tagged_ptr3 _temp2618=
yyls; struct Cyc_Yyltype* _temp2620= _temp2618.curr +( yylsp_offset - 3); if(
_temp2620 < _temp2618.base? 1: _temp2620 >= _temp2618.last_plus_one){ _throw(
Null_Exception);}* _temp2620;}).last_line), Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp2621= yyvs; struct _xenum_struct** _temp2623= _temp2621.curr +(
yyvsp_offset - 3); if( _temp2623 < _temp2621.base? 1: _temp2623 >= _temp2621.last_plus_one){
_throw( Null_Exception);}* _temp2623;}))); struct Cyc_Absyn_Decl* d;{ void*
_temp2627= Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2 _temp2624= yyvs;
struct _xenum_struct** _temp2626= _temp2624.curr +( yyvsp_offset - 5); if(
_temp2626 < _temp2624.base? 1: _temp2626 >= _temp2624.last_plus_one){ _throw(
Null_Exception);}* _temp2626;})); _LL2629: if( _temp2627 == Cyc_Parse_Struct_su){
goto _LL2630;} else{ goto _LL2631;} _LL2631: if( _temp2627 == Cyc_Parse_Union_su){
goto _LL2632;} else{ goto _LL2628;} _LL2630: d= Cyc_Absyn_struct_decl( Cyc_Absyn_Public,({
struct Cyc_Core_Opt* _temp2633=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2633->v=( void*) Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp2634= yyvs; struct _xenum_struct** _temp2636= _temp2634.curr +(
yyvsp_offset - 4); if( _temp2636 < _temp2634.base? 1: _temp2636 >= _temp2634.last_plus_one){
_throw( Null_Exception);}* _temp2636;})); _temp2633;}), ts,({ struct Cyc_Core_Opt*
_temp2637=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2637->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({ struct _tagged_ptr2
_temp2638= yyvs; struct _xenum_struct** _temp2640= _temp2638.curr +(
yyvsp_offset - 1); if( _temp2640 < _temp2638.base? 1: _temp2640 >= _temp2638.last_plus_one){
_throw( Null_Exception);}* _temp2640;})); _temp2637;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2641= yyls; struct Cyc_Yyltype* _temp2643= _temp2641.curr
+( yylsp_offset - 5); if( _temp2643 < _temp2641.base? 1: _temp2643 >= _temp2641.last_plus_one){
_throw( Null_Exception);}* _temp2643;}).first_line,({ struct _tagged_ptr3
_temp2644= yyls; struct Cyc_Yyltype* _temp2646= _temp2644.curr + yylsp_offset;
if( _temp2646 < _temp2644.base? 1: _temp2646 >= _temp2644.last_plus_one){ _throw(
Null_Exception);}* _temp2646;}).last_line)); goto _LL2628; _LL2632: Cyc_Parse_unimp2((
struct _tagged_string)({ char* _temp2647=( char*)"unions"; struct _tagged_string
_temp2648; _temp2648.curr= _temp2647; _temp2648.base= _temp2647; _temp2648.last_plus_one=
_temp2647 + 7; _temp2648;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2649= yyls; struct Cyc_Yyltype* _temp2651= _temp2649.curr +( yylsp_offset -
5); if( _temp2651 < _temp2649.base? 1: _temp2651 >= _temp2649.last_plus_one){
_throw( Null_Exception);}* _temp2651;}).first_line,({ struct _tagged_ptr3
_temp2652= yyls; struct Cyc_Yyltype* _temp2654= _temp2652.curr + yylsp_offset;
if( _temp2654 < _temp2652.base? 1: _temp2654 >= _temp2652.last_plus_one){ _throw(
Null_Exception);}* _temp2654;}).last_line)); d= Cyc_Absyn_new_decl( Cyc_Absyn_Union_d,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2655= yyls; struct Cyc_Yyltype*
_temp2657= _temp2655.curr +( yylsp_offset - 5); if( _temp2657 < _temp2655.base?
1: _temp2657 >= _temp2655.last_plus_one){ _throw( Null_Exception);}* _temp2657;}).first_line,({
struct _tagged_ptr3 _temp2658= yyls; struct Cyc_Yyltype* _temp2660= _temp2658.curr
+ yylsp_offset; if( _temp2660 < _temp2658.base? 1: _temp2660 >= _temp2658.last_plus_one){
_throw( Null_Exception);}* _temp2660;}).last_line)); goto _LL2628; _LL2628:;}
yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2661=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));* _temp2661=( struct
Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=({ struct Cyc_Parse_Decl_spec_struct*
_temp2662=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2662->tag= Cyc_Parse_Decl_spec_tag; _temp2662->f1= d;( void*) _temp2662;})};(
struct _xenum_struct*) _temp2661;}); break;} case 67: _LL2614:{ void* _temp2667=
Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2 _temp2664= yyvs; struct
_xenum_struct** _temp2666= _temp2664.curr +( yyvsp_offset - 2); if( _temp2666 <
_temp2664.base? 1: _temp2666 >= _temp2664.last_plus_one){ _throw( Null_Exception);}*
_temp2666;})); _LL2669: if( _temp2667 == Cyc_Parse_Struct_su){ goto _LL2670;}
else{ goto _LL2671;} _LL2671: if( _temp2667 == Cyc_Parse_Union_su){ goto _LL2672;}
else{ goto _LL2668;} _LL2670: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp2673=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2673=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec(({ struct Cyc_Absyn_StructType_struct* _temp2674=( struct
Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp2674->tag= Cyc_Absyn_StructType_tag; _temp2674->f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp2675= yyvs; struct _xenum_struct** _temp2677= _temp2675.curr
+( yyvsp_offset - 1); if( _temp2677 < _temp2675.base? 1: _temp2677 >= _temp2675.last_plus_one){
_throw( Null_Exception);}* _temp2677;})); _temp2674->f2= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp2678= yyvs; struct _xenum_struct** _temp2680= _temp2678.curr
+ yyvsp_offset; if( _temp2680 < _temp2678.base? 1: _temp2680 >= _temp2678.last_plus_one){
_throw( Null_Exception);}* _temp2680;})); _temp2674->f3= 0;( void*) _temp2674;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2681= yyls; struct Cyc_Yyltype*
_temp2683= _temp2681.curr +( yylsp_offset - 2); if( _temp2683 < _temp2681.base?
1: _temp2683 >= _temp2681.last_plus_one){ _throw( Null_Exception);}* _temp2683;}).first_line,({
struct _tagged_ptr3 _temp2684= yyls; struct Cyc_Yyltype* _temp2686= _temp2684.curr
+ yylsp_offset; if( _temp2686 < _temp2684.base? 1: _temp2686 >= _temp2684.last_plus_one){
_throw( Null_Exception);}* _temp2686;}).last_line))};( struct _xenum_struct*)
_temp2673;}); goto _LL2668; _LL2672: Cyc_Parse_unimp2(( struct _tagged_string)({
char* _temp2687=( char*)"unions"; struct _tagged_string _temp2688; _temp2688.curr=
_temp2687; _temp2688.base= _temp2687; _temp2688.last_plus_one= _temp2687 + 7;
_temp2688;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2689= yyls;
struct Cyc_Yyltype* _temp2691= _temp2689.curr +( yylsp_offset - 2); if(
_temp2691 < _temp2689.base? 1: _temp2691 >= _temp2689.last_plus_one){ _throw(
Null_Exception);}* _temp2691;}).first_line,({ struct _tagged_ptr3 _temp2692=
yyls; struct Cyc_Yyltype* _temp2694= _temp2692.curr + yylsp_offset; if(
_temp2694 < _temp2692.base? 1: _temp2694 >= _temp2692.last_plus_one){ _throw(
Null_Exception);}* _temp2694;}).last_line)); yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp2695=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2695=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=({
struct Cyc_Parse_Decl_spec_struct* _temp2696=( struct Cyc_Parse_Decl_spec_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct)); _temp2696->tag= Cyc_Parse_Decl_spec_tag;
_temp2696->f1= Cyc_Absyn_new_decl( Cyc_Absyn_Union_d, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2697= yyls; struct Cyc_Yyltype* _temp2699= _temp2697.curr
+( yylsp_offset - 2); if( _temp2699 < _temp2697.base? 1: _temp2699 >= _temp2697.last_plus_one){
_throw( Null_Exception);}* _temp2699;}).first_line,({ struct _tagged_ptr3
_temp2700= yyls; struct Cyc_Yyltype* _temp2702= _temp2700.curr + yylsp_offset;
if( _temp2702 < _temp2700.base? 1: _temp2702 >= _temp2700.last_plus_one){ _throw(
Null_Exception);}* _temp2702;}).last_line));( void*) _temp2696;})};( struct
_xenum_struct*) _temp2695;}); goto _LL2668; _LL2668:;} break; case 68: _LL2663:{
void* _temp2707= Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2 _temp2704=
yyvs; struct _xenum_struct** _temp2706= _temp2704.curr +( yyvsp_offset - 2); if(
_temp2706 < _temp2704.base? 1: _temp2706 >= _temp2704.last_plus_one){ _throw(
Null_Exception);}* _temp2706;})); _LL2709: if( _temp2707 == Cyc_Parse_Struct_su){
goto _LL2710;} else{ goto _LL2711;} _LL2711: if( _temp2707 == Cyc_Parse_Union_su){
goto _LL2712;} else{ goto _LL2708;} _LL2710: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp2713=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2713=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec(({ struct Cyc_Absyn_StructType_struct* _temp2714=( struct
Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp2714->tag= Cyc_Absyn_StructType_tag; _temp2714->f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp2715= yyvs; struct _xenum_struct** _temp2717= _temp2715.curr
+( yyvsp_offset - 1); if( _temp2717 < _temp2715.base? 1: _temp2717 >= _temp2715.last_plus_one){
_throw( Null_Exception);}* _temp2717;})); _temp2714->f2= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp2718= yyvs; struct _xenum_struct** _temp2720= _temp2718.curr
+ yyvsp_offset; if( _temp2720 < _temp2718.base? 1: _temp2720 >= _temp2718.last_plus_one){
_throw( Null_Exception);}* _temp2720;})); _temp2714->f3= 0;( void*) _temp2714;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2721= yyls; struct Cyc_Yyltype*
_temp2723= _temp2721.curr +( yylsp_offset - 2); if( _temp2723 < _temp2721.base?
1: _temp2723 >= _temp2721.last_plus_one){ _throw( Null_Exception);}* _temp2723;}).first_line,({
struct _tagged_ptr3 _temp2724= yyls; struct Cyc_Yyltype* _temp2726= _temp2724.curr
+ yylsp_offset; if( _temp2726 < _temp2724.base? 1: _temp2726 >= _temp2724.last_plus_one){
_throw( Null_Exception);}* _temp2726;}).last_line))};( struct _xenum_struct*)
_temp2713;}); goto _LL2708; _LL2712: Cyc_Parse_unimp2(( struct _tagged_string)({
char* _temp2727=( char*)"unions"; struct _tagged_string _temp2728; _temp2728.curr=
_temp2727; _temp2728.base= _temp2727; _temp2728.last_plus_one= _temp2727 + 7;
_temp2728;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2729= yyls;
struct Cyc_Yyltype* _temp2731= _temp2729.curr +( yylsp_offset - 2); if(
_temp2731 < _temp2729.base? 1: _temp2731 >= _temp2729.last_plus_one){ _throw(
Null_Exception);}* _temp2731;}).first_line,({ struct _tagged_ptr3 _temp2732=
yyls; struct Cyc_Yyltype* _temp2734= _temp2732.curr + yylsp_offset; if(
_temp2734 < _temp2732.base? 1: _temp2734 >= _temp2732.last_plus_one){ _throw(
Null_Exception);}* _temp2734;}).last_line)); yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp2735=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2735=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=({
struct Cyc_Parse_Decl_spec_struct* _temp2736=( struct Cyc_Parse_Decl_spec_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct)); _temp2736->tag= Cyc_Parse_Decl_spec_tag;
_temp2736->f1= Cyc_Absyn_new_decl( Cyc_Absyn_Union_d, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2737= yyls; struct Cyc_Yyltype* _temp2739= _temp2737.curr
+( yylsp_offset - 2); if( _temp2739 < _temp2737.base? 1: _temp2739 >= _temp2737.last_plus_one){
_throw( Null_Exception);}* _temp2739;}).first_line,({ struct _tagged_ptr3
_temp2740= yyls; struct Cyc_Yyltype* _temp2742= _temp2740.curr + yylsp_offset;
if( _temp2742 < _temp2740.base? 1: _temp2742 >= _temp2740.last_plus_one){ _throw(
Null_Exception);}* _temp2742;}).last_line));( void*) _temp2736;})};( struct
_xenum_struct*) _temp2735;}); goto _LL2708; _LL2708:;} break; case 69: _LL2703:
yyval=({ struct Cyc_TypeList_tok_struct* _temp2744=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));* _temp2744=( struct Cyc_TypeList_tok_struct){.tag=
Cyc_TypeList_tok_tag,.f1= 0};( struct _xenum_struct*) _temp2744;}); break; case
70: _LL2743: yyval=({ struct Cyc_TypeList_tok_struct* _temp2746=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));* _temp2746=( struct Cyc_TypeList_tok_struct){.tag=
Cyc_TypeList_tok_tag,.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp2747= yyvs;
struct _xenum_struct** _temp2749= _temp2747.curr +( yyvsp_offset - 1); if(
_temp2749 < _temp2747.base? 1: _temp2749 >= _temp2747.last_plus_one){ _throw(
Null_Exception);}* _temp2749;})))};( struct _xenum_struct*) _temp2746;}); break;
case 71: _LL2745:(( struct Cyc_Lexing_lexbuf*) Cyc_Parse_lbuf->v)->lex_curr_pos
-= 1; yyval=({ struct Cyc_TypeList_tok_struct* _temp2751=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));* _temp2751=( struct Cyc_TypeList_tok_struct){.tag=
Cyc_TypeList_tok_tag,.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp2752= yyvs;
struct _xenum_struct** _temp2754= _temp2752.curr +( yyvsp_offset - 1); if(
_temp2754 < _temp2752.base? 1: _temp2754 >= _temp2752.last_plus_one){ _throw(
Null_Exception);}* _temp2754;})))};( struct _xenum_struct*) _temp2751;}); break;
case 72: _LL2750: yyval=({ struct Cyc_StructOrUnion_tok_struct* _temp2756=(
struct Cyc_StructOrUnion_tok_struct*) GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));*
_temp2756=( struct Cyc_StructOrUnion_tok_struct){.tag= Cyc_StructOrUnion_tok_tag,.f1=
Cyc_Parse_Struct_su};( struct _xenum_struct*) _temp2756;}); break; case 73:
_LL2755: yyval=({ struct Cyc_StructOrUnion_tok_struct* _temp2758=( struct Cyc_StructOrUnion_tok_struct*)
GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));* _temp2758=( struct
Cyc_StructOrUnion_tok_struct){.tag= Cyc_StructOrUnion_tok_tag,.f1= Cyc_Parse_Union_su};(
struct _xenum_struct*) _temp2758;}); break; case 74: _LL2757: yyval=({ struct
Cyc_StructFieldDeclList_tok_struct* _temp2760=( struct Cyc_StructFieldDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_StructFieldDeclList_tok_struct));* _temp2760=(
struct Cyc_StructFieldDeclList_tok_struct){.tag= Cyc_StructFieldDeclList_tok_tag,.f1=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)((( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_StructFieldDeclListList_tok(({
struct _tagged_ptr2 _temp2761= yyvs; struct _xenum_struct** _temp2763= _temp2761.curr
+ yyvsp_offset; if( _temp2763 < _temp2761.base? 1: _temp2763 >= _temp2761.last_plus_one){
_throw( Null_Exception);}* _temp2763;}))))};( struct _xenum_struct*) _temp2760;});
break; case 75: _LL2759: yyval=({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp2765=( struct Cyc_StructFieldDeclListList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct));* _temp2765=( struct Cyc_StructFieldDeclListList_tok_struct){.tag=
Cyc_StructFieldDeclListList_tok_tag,.f1=({ struct Cyc_List_List* _temp2766=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2766->hd=(
void*) Cyc_yyget_StructFieldDeclList_tok(({ struct _tagged_ptr2 _temp2767= yyvs;
struct _xenum_struct** _temp2769= _temp2767.curr + yyvsp_offset; if( _temp2769 <
_temp2767.base? 1: _temp2769 >= _temp2767.last_plus_one){ _throw( Null_Exception);}*
_temp2769;})); _temp2766->tl= 0; _temp2766;})};( struct _xenum_struct*)
_temp2765;}); break; case 76: _LL2764: yyval=({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp2771=( struct Cyc_StructFieldDeclListList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct));* _temp2771=( struct Cyc_StructFieldDeclListList_tok_struct){.tag=
Cyc_StructFieldDeclListList_tok_tag,.f1=({ struct Cyc_List_List* _temp2772=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2772->hd=(
void*) Cyc_yyget_StructFieldDeclList_tok(({ struct _tagged_ptr2 _temp2776= yyvs;
struct _xenum_struct** _temp2778= _temp2776.curr + yyvsp_offset; if( _temp2778 <
_temp2776.base? 1: _temp2778 >= _temp2776.last_plus_one){ _throw( Null_Exception);}*
_temp2778;})); _temp2772->tl= Cyc_yyget_StructFieldDeclListList_tok(({ struct
_tagged_ptr2 _temp2773= yyvs; struct _xenum_struct** _temp2775= _temp2773.curr +(
yyvsp_offset - 1); if( _temp2775 < _temp2773.base? 1: _temp2775 >= _temp2773.last_plus_one){
_throw( Null_Exception);}* _temp2775;})); _temp2772;})};( struct _xenum_struct*)
_temp2771;}); break; case 77: _LL2770: yyval=({ struct Cyc_InitDeclList_tok_struct*
_temp2780=( struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));*
_temp2780=( struct Cyc_InitDeclList_tok_struct){.tag= Cyc_InitDeclList_tok_tag,.f1=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitDeclList_tok(({
struct _tagged_ptr2 _temp2781= yyvs; struct _xenum_struct** _temp2783= _temp2781.curr
+ yyvsp_offset; if( _temp2783 < _temp2781.base? 1: _temp2783 >= _temp2781.last_plus_one){
_throw( Null_Exception);}* _temp2783;})))};( struct _xenum_struct*) _temp2780;});
break; case 78: _LL2779: yyval=({ struct Cyc_InitDeclList_tok_struct* _temp2785=(
struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));*
_temp2785=( struct Cyc_InitDeclList_tok_struct){.tag= Cyc_InitDeclList_tok_tag,.f1=({
struct Cyc_List_List* _temp2786=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2786->hd=( void*) Cyc_yyget_InitDecl_tok(({ struct
_tagged_ptr2 _temp2787= yyvs; struct _xenum_struct** _temp2789= _temp2787.curr +
yyvsp_offset; if( _temp2789 < _temp2787.base? 1: _temp2789 >= _temp2787.last_plus_one){
_throw( Null_Exception);}* _temp2789;})); _temp2786->tl= 0; _temp2786;})};(
struct _xenum_struct*) _temp2785;}); break; case 79: _LL2784: yyval=({ struct
Cyc_InitDeclList_tok_struct* _temp2791=( struct Cyc_InitDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));* _temp2791=( struct Cyc_InitDeclList_tok_struct){.tag=
Cyc_InitDeclList_tok_tag,.f1=({ struct Cyc_List_List* _temp2792=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2792->hd=( void*) Cyc_yyget_InitDecl_tok(({
struct _tagged_ptr2 _temp2796= yyvs; struct _xenum_struct** _temp2798= _temp2796.curr
+ yyvsp_offset; if( _temp2798 < _temp2796.base? 1: _temp2798 >= _temp2796.last_plus_one){
_throw( Null_Exception);}* _temp2798;})); _temp2792->tl= Cyc_yyget_InitDeclList_tok(({
struct _tagged_ptr2 _temp2793= yyvs; struct _xenum_struct** _temp2795= _temp2793.curr
+( yyvsp_offset - 2); if( _temp2795 < _temp2793.base? 1: _temp2795 >= _temp2793.last_plus_one){
_throw( Null_Exception);}* _temp2795;})); _temp2792;})};( struct _xenum_struct*)
_temp2791;}); break; case 80: _LL2790: yyval=({ struct Cyc_InitDecl_tok_struct*
_temp2800=( struct Cyc_InitDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct));*
_temp2800=( struct Cyc_InitDecl_tok_struct){.tag= Cyc_InitDecl_tok_tag,.f1=({
struct _tuple12* _temp2801=( struct _tuple12*) GC_malloc( sizeof( struct
_tuple12)); _temp2801->f1= Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2
_temp2802= yyvs; struct _xenum_struct** _temp2804= _temp2802.curr + yyvsp_offset;
if( _temp2804 < _temp2802.base? 1: _temp2804 >= _temp2802.last_plus_one){ _throw(
Null_Exception);}* _temp2804;})); _temp2801->f2= 0; _temp2801;})};( struct
_xenum_struct*) _temp2800;}); break; case 81: _LL2799: yyval=({ struct Cyc_InitDecl_tok_struct*
_temp2806=( struct Cyc_InitDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct));*
_temp2806=( struct Cyc_InitDecl_tok_struct){.tag= Cyc_InitDecl_tok_tag,.f1=({
struct _tuple12* _temp2807=( struct _tuple12*) GC_malloc( sizeof( struct
_tuple12)); _temp2807->f1= Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2
_temp2811= yyvs; struct _xenum_struct** _temp2813= _temp2811.curr +(
yyvsp_offset - 2); if( _temp2813 < _temp2811.base? 1: _temp2813 >= _temp2811.last_plus_one){
_throw( Null_Exception);}* _temp2813;})); _temp2807->f2=( struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp2808= yyvs; struct _xenum_struct**
_temp2810= _temp2808.curr + yyvsp_offset; if( _temp2810 < _temp2808.base? 1:
_temp2810 >= _temp2808.last_plus_one){ _throw( Null_Exception);}* _temp2810;}));
_temp2807;})};( struct _xenum_struct*) _temp2806;}); break; case 82: _LL2805: {
struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp2815= yyvs; struct _xenum_struct** _temp2817= _temp2815.curr +(
yyvsp_offset - 2); if( _temp2817 < _temp2815.base? 1: _temp2817 >= _temp2815.last_plus_one){
_throw( Null_Exception);}* _temp2817;}))).f1; struct Cyc_List_List* tss=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp2818= yyvs; struct _xenum_struct** _temp2820= _temp2818.curr
+( yyvsp_offset - 2); if( _temp2820 < _temp2818.base? 1: _temp2820 >= _temp2818.last_plus_one){
_throw( Null_Exception);}* _temp2820;}))).f2; struct _tuple5 ts_info= Cyc_Parse_collapse_type_specifiers(
tss, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2821= yyls; struct
Cyc_Yyltype* _temp2823= _temp2821.curr +( yylsp_offset - 2); if( _temp2823 <
_temp2821.base? 1: _temp2823 >= _temp2821.last_plus_one){ _throw( Null_Exception);}*
_temp2823;}).first_line,({ struct _tagged_ptr3 _temp2824= yyls; struct Cyc_Yyltype*
_temp2826= _temp2824.curr +( yylsp_offset - 2); if( _temp2826 < _temp2824.base?
1: _temp2826 >= _temp2824.last_plus_one){ _throw( Null_Exception);}* _temp2826;}).last_line));
if( ts_info.f2 != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp2827=(
char*)"struct declaration contains nested type declaration"; struct
_tagged_string _temp2828; _temp2828.curr= _temp2827; _temp2828.base= _temp2827;
_temp2828.last_plus_one= _temp2827 + 52; _temp2828;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2829= yyls; struct Cyc_Yyltype* _temp2831= _temp2829.curr
+( yylsp_offset - 2); if( _temp2831 < _temp2829.base? 1: _temp2831 >= _temp2829.last_plus_one){
_throw( Null_Exception);}* _temp2831;}).first_line,({ struct _tagged_ptr3
_temp2832= yyls; struct Cyc_Yyltype* _temp2834= _temp2832.curr +( yylsp_offset -
2); if( _temp2834 < _temp2832.base? 1: _temp2834 >= _temp2832.last_plus_one){
_throw( Null_Exception);}* _temp2834;}).last_line));}{ void* t= ts_info.f1;
struct Cyc_List_List* info= Cyc_Parse_apply_tmss( tq, t, Cyc_yyget_DeclaratorList_tok(({
struct _tagged_ptr2 _temp2835= yyvs; struct _xenum_struct** _temp2837= _temp2835.curr
+( yyvsp_offset - 1); if( _temp2837 < _temp2835.base? 1: _temp2837 >= _temp2835.last_plus_one){
_throw( Null_Exception);}* _temp2837;}))); yyval=({ struct Cyc_StructFieldDeclList_tok_struct*
_temp2838=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct));* _temp2838=( struct Cyc_StructFieldDeclList_tok_struct){.tag=
Cyc_StructFieldDeclList_tok_tag,.f1=(( struct Cyc_List_List*(*)( struct _tuple4*(*
f)( struct Cyc_Position_Segment*, struct _tuple7*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_make_struct_field, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2839= yyls; struct Cyc_Yyltype* _temp2841= _temp2839.curr
+( yylsp_offset - 2); if( _temp2841 < _temp2839.base? 1: _temp2841 >= _temp2839.last_plus_one){
_throw( Null_Exception);}* _temp2841;}).first_line,({ struct _tagged_ptr3
_temp2842= yyls; struct Cyc_Yyltype* _temp2844= _temp2842.curr +( yylsp_offset -
1); if( _temp2844 < _temp2842.base? 1: _temp2844 >= _temp2842.last_plus_one){
_throw( Null_Exception);}* _temp2844;}).last_line), info)};( struct
_xenum_struct*) _temp2838;}); break;}} case 83: _LL2814: yyval=({ struct Cyc_QualSpecList_tok_struct*
_temp2846=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));*
_temp2846=( struct Cyc_QualSpecList_tok_struct){.tag= Cyc_QualSpecList_tok_tag,.f1=({
struct _tuple13* _temp2847=( struct _tuple13*) GC_malloc( sizeof( struct
_tuple13)); _temp2847->f1= Cyc_Absyn_empty_tqual(); _temp2847->f2=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp2848=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2848->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({
struct _tagged_ptr2 _temp2849= yyvs; struct _xenum_struct** _temp2851= _temp2849.curr
+ yyvsp_offset; if( _temp2851 < _temp2849.base? 1: _temp2851 >= _temp2849.last_plus_one){
_throw( Null_Exception);}* _temp2851;})); _temp2848->tl= 0; _temp2848;});
_temp2847;})};( struct _xenum_struct*) _temp2846;}); break; case 84: _LL2845:
yyval=({ struct Cyc_QualSpecList_tok_struct* _temp2853=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));* _temp2853=( struct Cyc_QualSpecList_tok_struct){.tag=
Cyc_QualSpecList_tok_tag,.f1=({ struct _tuple13* _temp2854=( struct _tuple13*)
GC_malloc( sizeof( struct _tuple13)); _temp2854->f1=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp2862= yyvs; struct _xenum_struct** _temp2864= _temp2862.curr
+ yyvsp_offset; if( _temp2864 < _temp2862.base? 1: _temp2864 >= _temp2862.last_plus_one){
_throw( Null_Exception);}* _temp2864;}))).f1; _temp2854->f2=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp2855=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2855->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({
struct _tagged_ptr2 _temp2859= yyvs; struct _xenum_struct** _temp2861= _temp2859.curr
+( yyvsp_offset - 1); if( _temp2861 < _temp2859.base? 1: _temp2861 >= _temp2859.last_plus_one){
_throw( Null_Exception);}* _temp2861;})); _temp2855->tl=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp2856= yyvs; struct _xenum_struct** _temp2858= _temp2856.curr
+ yyvsp_offset; if( _temp2858 < _temp2856.base? 1: _temp2858 >= _temp2856.last_plus_one){
_throw( Null_Exception);}* _temp2858;}))).f2; _temp2855;}); _temp2854;})};(
struct _xenum_struct*) _temp2853;}); break; case 85: _LL2852: yyval=({ struct
Cyc_QualSpecList_tok_struct* _temp2866=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));* _temp2866=( struct Cyc_QualSpecList_tok_struct){.tag=
Cyc_QualSpecList_tok_tag,.f1=({ struct _tuple13* _temp2867=( struct _tuple13*)
GC_malloc( sizeof( struct _tuple13)); _temp2867->f1= Cyc_yyget_TypeQual_tok(({
struct _tagged_ptr2 _temp2868= yyvs; struct _xenum_struct** _temp2870= _temp2868.curr
+ yyvsp_offset; if( _temp2870 < _temp2868.base? 1: _temp2870 >= _temp2868.last_plus_one){
_throw( Null_Exception);}* _temp2870;})); _temp2867->f2= 0; _temp2867;})};(
struct _xenum_struct*) _temp2866;}); break; case 86: _LL2865: yyval=({ struct
Cyc_QualSpecList_tok_struct* _temp2872=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));* _temp2872=( struct Cyc_QualSpecList_tok_struct){.tag=
Cyc_QualSpecList_tok_tag,.f1=({ struct _tuple13* _temp2873=( struct _tuple13*)
GC_malloc( sizeof( struct _tuple13)); _temp2873->f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2 _temp2877= yyvs; struct
_xenum_struct** _temp2879= _temp2877.curr +( yyvsp_offset - 1); if( _temp2879 <
_temp2877.base? 1: _temp2879 >= _temp2877.last_plus_one){ _throw( Null_Exception);}*
_temp2879;})),(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp2880=
yyvs; struct _xenum_struct** _temp2882= _temp2880.curr + yyvsp_offset; if(
_temp2882 < _temp2880.base? 1: _temp2882 >= _temp2880.last_plus_one){ _throw(
Null_Exception);}* _temp2882;}))).f1); _temp2873->f2=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp2874= yyvs; struct _xenum_struct** _temp2876= _temp2874.curr
+ yyvsp_offset; if( _temp2876 < _temp2874.base? 1: _temp2876 >= _temp2874.last_plus_one){
_throw( Null_Exception);}* _temp2876;}))).f2; _temp2873;})};( struct
_xenum_struct*) _temp2872;}); break; case 87: _LL2871: yyval=({ struct Cyc_DeclaratorList_tok_struct*
_temp2884=( struct Cyc_DeclaratorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorList_tok_struct));*
_temp2884=( struct Cyc_DeclaratorList_tok_struct){.tag= Cyc_DeclaratorList_tok_tag,.f1=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_DeclaratorList_tok(({
struct _tagged_ptr2 _temp2885= yyvs; struct _xenum_struct** _temp2887= _temp2885.curr
+ yyvsp_offset; if( _temp2887 < _temp2885.base? 1: _temp2887 >= _temp2885.last_plus_one){
_throw( Null_Exception);}* _temp2887;})))};( struct _xenum_struct*) _temp2884;});
break; case 88: _LL2883: yyval=({ struct Cyc_DeclaratorList_tok_struct*
_temp2889=( struct Cyc_DeclaratorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorList_tok_struct));*
_temp2889=( struct Cyc_DeclaratorList_tok_struct){.tag= Cyc_DeclaratorList_tok_tag,.f1=({
struct Cyc_List_List* _temp2890=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2890->hd=( void*) Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2891= yyvs; struct _xenum_struct** _temp2893= _temp2891.curr
+ yyvsp_offset; if( _temp2893 < _temp2891.base? 1: _temp2893 >= _temp2891.last_plus_one){
_throw( Null_Exception);}* _temp2893;})); _temp2890->tl= 0; _temp2890;})};(
struct _xenum_struct*) _temp2889;}); break; case 89: _LL2888: yyval=({ struct
Cyc_DeclaratorList_tok_struct* _temp2895=( struct Cyc_DeclaratorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorList_tok_struct));* _temp2895=( struct
Cyc_DeclaratorList_tok_struct){.tag= Cyc_DeclaratorList_tok_tag,.f1=({ struct
Cyc_List_List* _temp2896=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2896->hd=( void*) Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp2900=
yyvs; struct _xenum_struct** _temp2902= _temp2900.curr + yyvsp_offset; if(
_temp2902 < _temp2900.base? 1: _temp2902 >= _temp2900.last_plus_one){ _throw(
Null_Exception);}* _temp2902;})); _temp2896->tl= Cyc_yyget_DeclaratorList_tok(({
struct _tagged_ptr2 _temp2897= yyvs; struct _xenum_struct** _temp2899= _temp2897.curr
+( yyvsp_offset - 2); if( _temp2899 < _temp2897.base? 1: _temp2899 >= _temp2897.last_plus_one){
_throw( Null_Exception);}* _temp2899;})); _temp2896;})};( struct _xenum_struct*)
_temp2895;}); break; case 90: _LL2894: yyval=({ struct _tagged_ptr2 _temp2904=
yyvs; struct _xenum_struct** _temp2906= _temp2904.curr + yyvsp_offset; if(
_temp2906 < _temp2904.base? 1: _temp2906 >= _temp2904.last_plus_one){ _throw(
Null_Exception);}* _temp2906;}); break; case 91: _LL2903: Cyc_Parse_unimp2((
struct _tagged_string)({ char* _temp2908=( char*)"bit fields"; struct
_tagged_string _temp2909; _temp2909.curr= _temp2908; _temp2909.base= _temp2908;
_temp2909.last_plus_one= _temp2908 + 11; _temp2909;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2910= yyls; struct Cyc_Yyltype* _temp2912= _temp2910.curr
+( yylsp_offset - 1); if( _temp2912 < _temp2910.base? 1: _temp2912 >= _temp2910.last_plus_one){
_throw( Null_Exception);}* _temp2912;}).first_line,({ struct _tagged_ptr3
_temp2913= yyls; struct Cyc_Yyltype* _temp2915= _temp2913.curr + yylsp_offset;
if( _temp2915 < _temp2913.base? 1: _temp2915 >= _temp2913.last_plus_one){ _throw(
Null_Exception);}* _temp2915;}).last_line)); yyval=({ struct Cyc_Declarator_tok_struct*
_temp2916=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));*
_temp2916=( struct Cyc_Declarator_tok_struct){.tag= Cyc_Declarator_tok_tag,.f1=({
struct Cyc_Parse_Declarator* _temp2917=( struct Cyc_Parse_Declarator*) GC_malloc(
sizeof( struct Cyc_Parse_Declarator)); _temp2917->id=({ struct _tuple1*
_temp2918=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp2918->f1=({
struct Cyc_Absyn_Rel_n_struct* _temp2922=( struct Cyc_Absyn_Rel_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct)); _temp2922->tag= Cyc_Absyn_Rel_n_tag;
_temp2922->f1= 0;( void*) _temp2922;}); _temp2918->f2=({ struct _tagged_string*
_temp2919=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp2919[ 0]=( struct _tagged_string)({ char* _temp2920=( char*)""; struct
_tagged_string _temp2921; _temp2921.curr= _temp2920; _temp2921.base= _temp2920;
_temp2921.last_plus_one= _temp2920 + 1; _temp2921;}); _temp2919;}); _temp2918;});
_temp2917->tms= 0; _temp2917;})};( struct _xenum_struct*) _temp2916;}); break;
case 92: _LL2907: Cyc_Parse_unimp2(( struct _tagged_string)({ char* _temp2924=(
char*)"bit fields"; struct _tagged_string _temp2925; _temp2925.curr= _temp2924;
_temp2925.base= _temp2924; _temp2925.last_plus_one= _temp2924 + 11; _temp2925;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2926= yyls; struct Cyc_Yyltype*
_temp2928= _temp2926.curr +( yylsp_offset - 2); if( _temp2928 < _temp2926.base?
1: _temp2928 >= _temp2926.last_plus_one){ _throw( Null_Exception);}* _temp2928;}).first_line,({
struct _tagged_ptr3 _temp2929= yyls; struct Cyc_Yyltype* _temp2931= _temp2929.curr
+( yylsp_offset - 1); if( _temp2931 < _temp2929.base? 1: _temp2931 >= _temp2929.last_plus_one){
_throw( Null_Exception);}* _temp2931;}).last_line)); yyval=({ struct
_tagged_ptr2 _temp2932= yyvs; struct _xenum_struct** _temp2934= _temp2932.curr +(
yyvsp_offset - 2); if( _temp2934 < _temp2932.base? 1: _temp2934 >= _temp2932.last_plus_one){
_throw( Null_Exception);}* _temp2934;}); break; case 93: _LL2923: yyval=({
struct Cyc_TypeSpecifier_tok_struct* _temp2936=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));* _temp2936=( struct
Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=({ struct Cyc_Parse_Decl_spec_struct*
_temp2937=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2937->tag= Cyc_Parse_Decl_spec_tag; _temp2937->f1= Cyc_Absyn_enum_decl( Cyc_Absyn_Public,
0, 0,({ struct Cyc_Core_Opt* _temp2938=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp2938->v=( void*) Cyc_yyget_EnumeratorFieldList_tok(({
struct _tagged_ptr2 _temp2939= yyvs; struct _xenum_struct** _temp2941= _temp2939.curr
+( yyvsp_offset - 1); if( _temp2941 < _temp2939.base? 1: _temp2941 >= _temp2939.last_plus_one){
_throw( Null_Exception);}* _temp2941;})); _temp2938;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2942= yyls; struct Cyc_Yyltype* _temp2944= _temp2942.curr
+( yylsp_offset - 3); if( _temp2944 < _temp2942.base? 1: _temp2944 >= _temp2942.last_plus_one){
_throw( Null_Exception);}* _temp2944;}).first_line,({ struct _tagged_ptr3
_temp2945= yyls; struct Cyc_Yyltype* _temp2947= _temp2945.curr + yylsp_offset;
if( _temp2947 < _temp2945.base? 1: _temp2947 >= _temp2945.last_plus_one){ _throw(
Null_Exception);}* _temp2947;}).last_line));( void*) _temp2937;})};( struct
_xenum_struct*) _temp2936;}); Cyc_Parse_unimp2(( struct _tagged_string)({ char*
_temp2948=( char*)"anonymous enums"; struct _tagged_string _temp2949; _temp2949.curr=
_temp2948; _temp2949.base= _temp2948; _temp2949.last_plus_one= _temp2948 + 16;
_temp2949;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2950= yyls;
struct Cyc_Yyltype* _temp2952= _temp2950.curr +( yylsp_offset - 3); if(
_temp2952 < _temp2950.base? 1: _temp2952 >= _temp2950.last_plus_one){ _throw(
Null_Exception);}* _temp2952;}).first_line,({ struct _tagged_ptr3 _temp2953=
yyls; struct Cyc_Yyltype* _temp2955= _temp2953.curr + yylsp_offset; if(
_temp2955 < _temp2953.base? 1: _temp2955 >= _temp2953.last_plus_one){ _throw(
Null_Exception);}* _temp2955;}).last_line)); break; case 94: _LL2935: { struct
Cyc_List_List* ts=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2957= yyls; struct Cyc_Yyltype* _temp2959= _temp2957.curr
+( yylsp_offset - 3); if( _temp2959 < _temp2957.base? 1: _temp2959 >= _temp2957.last_plus_one){
_throw( Null_Exception);}* _temp2959;}).first_line,({ struct _tagged_ptr3
_temp2960= yyls; struct Cyc_Yyltype* _temp2962= _temp2960.curr +( yylsp_offset -
3); if( _temp2962 < _temp2960.base? 1: _temp2962 >= _temp2960.last_plus_one){
_throw( Null_Exception);}* _temp2962;}).last_line), Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp2963= yyvs; struct _xenum_struct** _temp2965= _temp2963.curr
+( yyvsp_offset - 3); if( _temp2965 < _temp2963.base? 1: _temp2965 >= _temp2963.last_plus_one){
_throw( Null_Exception);}* _temp2965;}))); yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp2966=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2966=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=({
struct Cyc_Parse_Decl_spec_struct* _temp2967=( struct Cyc_Parse_Decl_spec_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct)); _temp2967->tag= Cyc_Parse_Decl_spec_tag;
_temp2967->f1= Cyc_Absyn_enum_decl( Cyc_Absyn_Public,({ struct Cyc_Core_Opt*
_temp2968=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2968->v=( void*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp2969=
yyvs; struct _xenum_struct** _temp2971= _temp2969.curr +( yyvsp_offset - 4); if(
_temp2971 < _temp2969.base? 1: _temp2971 >= _temp2969.last_plus_one){ _throw(
Null_Exception);}* _temp2971;})); _temp2968;}), ts,({ struct Cyc_Core_Opt*
_temp2972=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2972->v=( void*) Cyc_yyget_EnumeratorFieldList_tok(({ struct _tagged_ptr2
_temp2973= yyvs; struct _xenum_struct** _temp2975= _temp2973.curr +(
yyvsp_offset - 1); if( _temp2975 < _temp2973.base? 1: _temp2975 >= _temp2973.last_plus_one){
_throw( Null_Exception);}* _temp2975;})); _temp2972;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2976= yyls; struct Cyc_Yyltype* _temp2978= _temp2976.curr
+( yylsp_offset - 5); if( _temp2978 < _temp2976.base? 1: _temp2978 >= _temp2976.last_plus_one){
_throw( Null_Exception);}* _temp2978;}).first_line,({ struct _tagged_ptr3
_temp2979= yyls; struct Cyc_Yyltype* _temp2981= _temp2979.curr + yylsp_offset;
if( _temp2981 < _temp2979.base? 1: _temp2981 >= _temp2979.last_plus_one){ _throw(
Null_Exception);}* _temp2981;}).last_line));( void*) _temp2967;})};( struct
_xenum_struct*) _temp2966;}); break;} case 95: _LL2956: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp2983=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2983=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec(({ struct Cyc_Absyn_EnumType_struct* _temp2984=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp2984->tag= Cyc_Absyn_EnumType_tag;
_temp2984->f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp2985= yyvs; struct _xenum_struct** _temp2987= _temp2985.curr +(
yyvsp_offset - 1); if( _temp2987 < _temp2985.base? 1: _temp2987 >= _temp2985.last_plus_one){
_throw( Null_Exception);}* _temp2987;})); _temp2984->f2= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp2988= yyvs; struct _xenum_struct** _temp2990= _temp2988.curr
+ yyvsp_offset; if( _temp2990 < _temp2988.base? 1: _temp2990 >= _temp2988.last_plus_one){
_throw( Null_Exception);}* _temp2990;})); _temp2984->f3= 0;( void*) _temp2984;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2991= yyls; struct Cyc_Yyltype*
_temp2993= _temp2991.curr +( yylsp_offset - 2); if( _temp2993 < _temp2991.base?
1: _temp2993 >= _temp2991.last_plus_one){ _throw( Null_Exception);}* _temp2993;}).first_line,({
struct _tagged_ptr3 _temp2994= yyls; struct Cyc_Yyltype* _temp2996= _temp2994.curr
+ yylsp_offset; if( _temp2996 < _temp2994.base? 1: _temp2996 >= _temp2994.last_plus_one){
_throw( Null_Exception);}* _temp2996;}).last_line))};( struct _xenum_struct*)
_temp2983;}); break; case 96: _LL2982: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp2998=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2998=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=({
struct Cyc_Parse_Decl_spec_struct* _temp2999=( struct Cyc_Parse_Decl_spec_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct)); _temp2999->tag= Cyc_Parse_Decl_spec_tag;
_temp2999->f1= Cyc_Absyn_xenum_decl( Cyc_Absyn_Public, Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3000= yyvs; struct _xenum_struct** _temp3002= _temp3000.curr
+( yyvsp_offset - 3); if( _temp3002 < _temp3000.base? 1: _temp3002 >= _temp3000.last_plus_one){
_throw( Null_Exception);}* _temp3002;})), Cyc_yyget_EnumeratorFieldList_tok(({
struct _tagged_ptr2 _temp3003= yyvs; struct _xenum_struct** _temp3005= _temp3003.curr
+( yyvsp_offset - 1); if( _temp3005 < _temp3003.base? 1: _temp3005 >= _temp3003.last_plus_one){
_throw( Null_Exception);}* _temp3005;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3006= yyls; struct Cyc_Yyltype* _temp3008= _temp3006.curr +(
yylsp_offset - 4); if( _temp3008 < _temp3006.base? 1: _temp3008 >= _temp3006.last_plus_one){
_throw( Null_Exception);}* _temp3008;}).first_line,({ struct _tagged_ptr3
_temp3009= yyls; struct Cyc_Yyltype* _temp3011= _temp3009.curr + yylsp_offset;
if( _temp3011 < _temp3009.base? 1: _temp3011 >= _temp3009.last_plus_one){ _throw(
Null_Exception);}* _temp3011;}).last_line));( void*) _temp2999;})};( struct
_xenum_struct*) _temp2998;}); break; case 97: _LL2997: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp3013=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp3013=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec(({ struct Cyc_Absyn_XenumType_struct* _temp3014=( struct Cyc_Absyn_XenumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XenumType_struct)); _temp3014->tag= Cyc_Absyn_XenumType_tag;
_temp3014->f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3015= yyvs;
struct _xenum_struct** _temp3017= _temp3015.curr + yyvsp_offset; if( _temp3017 <
_temp3015.base? 1: _temp3017 >= _temp3015.last_plus_one){ _throw( Null_Exception);}*
_temp3017;})); _temp3014->f2= 0;( void*) _temp3014;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3018= yyls; struct Cyc_Yyltype* _temp3020= _temp3018.curr
+( yylsp_offset - 1); if( _temp3020 < _temp3018.base? 1: _temp3020 >= _temp3018.last_plus_one){
_throw( Null_Exception);}* _temp3020;}).first_line,({ struct _tagged_ptr3
_temp3021= yyls; struct Cyc_Yyltype* _temp3023= _temp3021.curr + yylsp_offset;
if( _temp3023 < _temp3021.base? 1: _temp3023 >= _temp3021.last_plus_one){ _throw(
Null_Exception);}* _temp3023;}).last_line))};( struct _xenum_struct*) _temp3013;});
break; case 98: _LL3012: yyval=({ struct Cyc_EnumeratorFieldList_tok_struct*
_temp3025=( struct Cyc_EnumeratorFieldList_tok_struct*) GC_malloc( sizeof(
struct Cyc_EnumeratorFieldList_tok_struct));* _temp3025=( struct Cyc_EnumeratorFieldList_tok_struct){.tag=
Cyc_EnumeratorFieldList_tok_tag,.f1=({ struct Cyc_List_List* _temp3026=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3026->hd=( void*)
Cyc_yyget_EnumeratorField_tok(({ struct _tagged_ptr2 _temp3027= yyvs; struct
_xenum_struct** _temp3029= _temp3027.curr + yyvsp_offset; if( _temp3029 <
_temp3027.base? 1: _temp3029 >= _temp3027.last_plus_one){ _throw( Null_Exception);}*
_temp3029;})); _temp3026->tl= 0; _temp3026;})};( struct _xenum_struct*)
_temp3025;}); break; case 99: _LL3024: yyval=({ struct Cyc_EnumeratorFieldList_tok_struct*
_temp3031=( struct Cyc_EnumeratorFieldList_tok_struct*) GC_malloc( sizeof(
struct Cyc_EnumeratorFieldList_tok_struct));* _temp3031=( struct Cyc_EnumeratorFieldList_tok_struct){.tag=
Cyc_EnumeratorFieldList_tok_tag,.f1=({ struct Cyc_List_List* _temp3032=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3032->hd=( void*)
Cyc_yyget_EnumeratorField_tok(({ struct _tagged_ptr2 _temp3033= yyvs; struct
_xenum_struct** _temp3035= _temp3033.curr +( yyvsp_offset - 1); if( _temp3035 <
_temp3033.base? 1: _temp3035 >= _temp3033.last_plus_one){ _throw( Null_Exception);}*
_temp3035;})); _temp3032->tl= 0; _temp3032;})};( struct _xenum_struct*)
_temp3031;}); break; case 100: _LL3030: yyval=({ struct Cyc_EnumeratorFieldList_tok_struct*
_temp3037=( struct Cyc_EnumeratorFieldList_tok_struct*) GC_malloc( sizeof(
struct Cyc_EnumeratorFieldList_tok_struct));* _temp3037=( struct Cyc_EnumeratorFieldList_tok_struct){.tag=
Cyc_EnumeratorFieldList_tok_tag,.f1=({ struct Cyc_List_List* _temp3038=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3038->hd=( void*)
Cyc_yyget_EnumeratorField_tok(({ struct _tagged_ptr2 _temp3042= yyvs; struct
_xenum_struct** _temp3044= _temp3042.curr +( yyvsp_offset - 2); if( _temp3044 <
_temp3042.base? 1: _temp3044 >= _temp3042.last_plus_one){ _throw( Null_Exception);}*
_temp3044;})); _temp3038->tl= Cyc_yyget_EnumeratorFieldList_tok(({ struct
_tagged_ptr2 _temp3039= yyvs; struct _xenum_struct** _temp3041= _temp3039.curr +
yyvsp_offset; if( _temp3041 < _temp3039.base? 1: _temp3041 >= _temp3039.last_plus_one){
_throw( Null_Exception);}* _temp3041;})); _temp3038;})};( struct _xenum_struct*)
_temp3037;}); break; case 101: _LL3036: yyval=({ struct Cyc_EnumeratorFieldList_tok_struct*
_temp3046=( struct Cyc_EnumeratorFieldList_tok_struct*) GC_malloc( sizeof(
struct Cyc_EnumeratorFieldList_tok_struct));* _temp3046=( struct Cyc_EnumeratorFieldList_tok_struct){.tag=
Cyc_EnumeratorFieldList_tok_tag,.f1=({ struct Cyc_List_List* _temp3047=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3047->hd=( void*)
Cyc_yyget_EnumeratorField_tok(({ struct _tagged_ptr2 _temp3051= yyvs; struct
_xenum_struct** _temp3053= _temp3051.curr +( yyvsp_offset - 2); if( _temp3053 <
_temp3051.base? 1: _temp3053 >= _temp3051.last_plus_one){ _throw( Null_Exception);}*
_temp3053;})); _temp3047->tl= Cyc_yyget_EnumeratorFieldList_tok(({ struct
_tagged_ptr2 _temp3048= yyvs; struct _xenum_struct** _temp3050= _temp3048.curr +
yyvsp_offset; if( _temp3050 < _temp3048.base? 1: _temp3050 >= _temp3048.last_plus_one){
_throw( Null_Exception);}* _temp3050;})); _temp3047;})};( struct _xenum_struct*)
_temp3046;}); break; case 102: _LL3045: yyval=({ struct Cyc_EnumeratorField_tok_struct*
_temp3055=( struct Cyc_EnumeratorField_tok_struct*) GC_malloc( sizeof( struct
Cyc_EnumeratorField_tok_struct));* _temp3055=( struct Cyc_EnumeratorField_tok_struct){.tag=
Cyc_EnumeratorField_tok_tag,.f1=({ struct Cyc_Absyn_Enumfield* _temp3056=(
struct Cyc_Absyn_Enumfield*) GC_malloc( sizeof( struct Cyc_Absyn_Enumfield));
_temp3056->name= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3063= yyvs;
struct _xenum_struct** _temp3065= _temp3063.curr + yyvsp_offset; if( _temp3065 <
_temp3063.base? 1: _temp3065 >= _temp3063.last_plus_one){ _throw( Null_Exception);}*
_temp3065;})); _temp3056->tag= 0; _temp3056->tvs= 0; _temp3056->typs= 0;
_temp3056->loc= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3057=
yyls; struct Cyc_Yyltype* _temp3059= _temp3057.curr + yylsp_offset; if(
_temp3059 < _temp3057.base? 1: _temp3059 >= _temp3057.last_plus_one){ _throw(
Null_Exception);}* _temp3059;}).first_line,({ struct _tagged_ptr3 _temp3060=
yyls; struct Cyc_Yyltype* _temp3062= _temp3060.curr + yylsp_offset; if(
_temp3062 < _temp3060.base? 1: _temp3062 >= _temp3060.last_plus_one){ _throw(
Null_Exception);}* _temp3062;}).last_line); _temp3056;})};( struct _xenum_struct*)
_temp3055;}); break; case 103: _LL3054: yyval=({ struct Cyc_EnumeratorField_tok_struct*
_temp3067=( struct Cyc_EnumeratorField_tok_struct*) GC_malloc( sizeof( struct
Cyc_EnumeratorField_tok_struct));* _temp3067=( struct Cyc_EnumeratorField_tok_struct){.tag=
Cyc_EnumeratorField_tok_tag,.f1=({ struct Cyc_Absyn_Enumfield* _temp3068=(
struct Cyc_Absyn_Enumfield*) GC_malloc( sizeof( struct Cyc_Absyn_Enumfield));
_temp3068->name= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3078= yyvs;
struct _xenum_struct** _temp3080= _temp3078.curr +( yyvsp_offset - 2); if(
_temp3080 < _temp3078.base? 1: _temp3080 >= _temp3078.last_plus_one){ _throw(
Null_Exception);}* _temp3080;})); _temp3068->tag=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp3075= yyvs; struct _xenum_struct** _temp3077= _temp3075.curr
+ yyvsp_offset; if( _temp3077 < _temp3075.base? 1: _temp3077 >= _temp3075.last_plus_one){
_throw( Null_Exception);}* _temp3077;})); _temp3068->tvs= 0; _temp3068->typs= 0;
_temp3068->loc= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3069=
yyls; struct Cyc_Yyltype* _temp3071= _temp3069.curr +( yylsp_offset - 2); if(
_temp3071 < _temp3069.base? 1: _temp3071 >= _temp3069.last_plus_one){ _throw(
Null_Exception);}* _temp3071;}).first_line,({ struct _tagged_ptr3 _temp3072=
yyls; struct Cyc_Yyltype* _temp3074= _temp3072.curr + yylsp_offset; if(
_temp3074 < _temp3072.base? 1: _temp3074 >= _temp3072.last_plus_one){ _throw(
Null_Exception);}* _temp3074;}).last_line); _temp3068;})};( struct _xenum_struct*)
_temp3067;}); break; case 104: _LL3066: { struct Cyc_List_List* typs=(( struct
Cyc_List_List*(*)( struct _tuple9*(* f)( struct Cyc_Position_Segment*, struct
_tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp3082= yyls; struct Cyc_Yyltype* _temp3084= _temp3082.curr +( yylsp_offset -
1); if( _temp3084 < _temp3082.base? 1: _temp3084 >= _temp3082.last_plus_one){
_throw( Null_Exception);}* _temp3084;}).first_line,({ struct _tagged_ptr3
_temp3085= yyls; struct Cyc_Yyltype* _temp3087= _temp3085.curr +( yylsp_offset -
1); if( _temp3087 < _temp3085.base? 1: _temp3087 >= _temp3085.last_plus_one){
_throw( Null_Exception);}* _temp3087;}).last_line),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp3088= yyvs; struct _xenum_struct** _temp3090= _temp3088.curr
+( yyvsp_offset - 1); if( _temp3090 < _temp3088.base? 1: _temp3090 >= _temp3088.last_plus_one){
_throw( Null_Exception);}* _temp3090;})))); struct Cyc_List_List* tvs=(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3091=
yyls; struct Cyc_Yyltype* _temp3093= _temp3091.curr +( yylsp_offset - 3); if(
_temp3093 < _temp3091.base? 1: _temp3093 >= _temp3091.last_plus_one){ _throw(
Null_Exception);}* _temp3093;}).first_line,({ struct _tagged_ptr3 _temp3094=
yyls; struct Cyc_Yyltype* _temp3096= _temp3094.curr +( yylsp_offset - 3); if(
_temp3096 < _temp3094.base? 1: _temp3096 >= _temp3094.last_plus_one){ _throw(
Null_Exception);}* _temp3096;}).last_line), Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp3097= yyvs; struct _xenum_struct** _temp3099= _temp3097.curr +(
yyvsp_offset - 3); if( _temp3099 < _temp3097.base? 1: _temp3099 >= _temp3097.last_plus_one){
_throw( Null_Exception);}* _temp3099;}))); yyval=({ struct Cyc_EnumeratorField_tok_struct*
_temp3100=( struct Cyc_EnumeratorField_tok_struct*) GC_malloc( sizeof( struct
Cyc_EnumeratorField_tok_struct));* _temp3100=( struct Cyc_EnumeratorField_tok_struct){.tag=
Cyc_EnumeratorField_tok_tag,.f1=({ struct Cyc_Absyn_Enumfield* _temp3101=(
struct Cyc_Absyn_Enumfield*) GC_malloc( sizeof( struct Cyc_Absyn_Enumfield));
_temp3101->name= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3108= yyvs;
struct _xenum_struct** _temp3110= _temp3108.curr +( yyvsp_offset - 4); if(
_temp3110 < _temp3108.base? 1: _temp3110 >= _temp3108.last_plus_one){ _throw(
Null_Exception);}* _temp3110;})); _temp3101->tag= 0; _temp3101->tvs= tvs;
_temp3101->typs= typs; _temp3101->loc= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3102= yyls; struct Cyc_Yyltype* _temp3104= _temp3102.curr +(
yylsp_offset - 4); if( _temp3104 < _temp3102.base? 1: _temp3104 >= _temp3102.last_plus_one){
_throw( Null_Exception);}* _temp3104;}).first_line,({ struct _tagged_ptr3
_temp3105= yyls; struct Cyc_Yyltype* _temp3107= _temp3105.curr + yylsp_offset;
if( _temp3107 < _temp3105.base? 1: _temp3107 >= _temp3105.last_plus_one){ _throw(
Null_Exception);}* _temp3107;}).last_line); _temp3101;})};( struct _xenum_struct*)
_temp3100;}); break;} case 105: _LL3081: yyval=({ struct _tagged_ptr2 _temp3112=
yyvs; struct _xenum_struct** _temp3114= _temp3112.curr + yyvsp_offset; if(
_temp3114 < _temp3112.base? 1: _temp3114 >= _temp3112.last_plus_one){ _throw(
Null_Exception);}* _temp3114;}); break; case 106: _LL3111: yyval=({ struct Cyc_Declarator_tok_struct*
_temp3116=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));*
_temp3116=( struct Cyc_Declarator_tok_struct){.tag= Cyc_Declarator_tok_tag,.f1=({
struct Cyc_Parse_Declarator* _temp3117=( struct Cyc_Parse_Declarator*) GC_malloc(
sizeof( struct Cyc_Parse_Declarator)); _temp3117->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3124= yyvs; struct _xenum_struct** _temp3126= _temp3124.curr
+ yyvsp_offset; if( _temp3126 < _temp3124.base? 1: _temp3126 >= _temp3124.last_plus_one){
_throw( Null_Exception);}* _temp3126;})))->id; _temp3117->tms=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_TypeModifierList_tok(({
struct _tagged_ptr2 _temp3118= yyvs; struct _xenum_struct** _temp3120= _temp3118.curr
+( yyvsp_offset - 1); if( _temp3120 < _temp3118.base? 1: _temp3120 >= _temp3118.last_plus_one){
_throw( Null_Exception);}* _temp3120;})),( Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp3121= yyvs; struct _xenum_struct** _temp3123= _temp3121.curr +
yyvsp_offset; if( _temp3123 < _temp3121.base? 1: _temp3123 >= _temp3121.last_plus_one){
_throw( Null_Exception);}* _temp3123;})))->tms); _temp3117;})};( struct
_xenum_struct*) _temp3116;}); break; case 107: _LL3115: yyval=({ struct Cyc_Declarator_tok_struct*
_temp3128=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));*
_temp3128=( struct Cyc_Declarator_tok_struct){.tag= Cyc_Declarator_tok_tag,.f1=({
struct Cyc_Parse_Declarator* _temp3129=( struct Cyc_Parse_Declarator*) GC_malloc(
sizeof( struct Cyc_Parse_Declarator)); _temp3129->id= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3130= yyvs; struct _xenum_struct** _temp3132= _temp3130.curr
+ yyvsp_offset; if( _temp3132 < _temp3130.base? 1: _temp3132 >= _temp3130.last_plus_one){
_throw( Null_Exception);}* _temp3132;})); _temp3129->tms= 0; _temp3129;})};(
struct _xenum_struct*) _temp3128;}); break; case 108: _LL3127: yyval=({ struct
_tagged_ptr2 _temp3134= yyvs; struct _xenum_struct** _temp3136= _temp3134.curr +(
yyvsp_offset - 1); if( _temp3136 < _temp3134.base? 1: _temp3136 >= _temp3134.last_plus_one){
_throw( Null_Exception);}* _temp3136;}); break; case 109: _LL3133: yyval=({
struct Cyc_Declarator_tok_struct* _temp3138=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));* _temp3138=( struct Cyc_Declarator_tok_struct){.tag=
Cyc_Declarator_tok_tag,.f1=({ struct Cyc_Parse_Declarator* _temp3139=( struct
Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp3139->id=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3144= yyvs;
struct _xenum_struct** _temp3146= _temp3144.curr +( yyvsp_offset - 2); if(
_temp3146 < _temp3144.base? 1: _temp3146 >= _temp3144.last_plus_one){ _throw(
Null_Exception);}* _temp3146;})))->id; _temp3139->tms=({ struct Cyc_List_List*
_temp3140=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3140->hd=( void*) Cyc_Absyn_Carray_mod; _temp3140->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3141= yyvs; struct _xenum_struct** _temp3143= _temp3141.curr
+( yyvsp_offset - 2); if( _temp3143 < _temp3141.base? 1: _temp3143 >= _temp3141.last_plus_one){
_throw( Null_Exception);}* _temp3143;})))->tms; _temp3140;}); _temp3139;})};(
struct _xenum_struct*) _temp3138;}); break; case 110: _LL3137: yyval=({ struct
Cyc_Declarator_tok_struct* _temp3148=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));* _temp3148=( struct Cyc_Declarator_tok_struct){.tag=
Cyc_Declarator_tok_tag,.f1=({ struct Cyc_Parse_Declarator* _temp3149=( struct
Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp3149->id=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3158= yyvs;
struct _xenum_struct** _temp3160= _temp3158.curr +( yyvsp_offset - 3); if(
_temp3160 < _temp3158.base? 1: _temp3160 >= _temp3158.last_plus_one){ _throw(
Null_Exception);}* _temp3160;})))->id; _temp3149->tms=({ struct Cyc_List_List*
_temp3150=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3150->hd=( void*)({ struct Cyc_Absyn_ConstArray_mod_struct* _temp3154=(
struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct));
_temp3154->tag= Cyc_Absyn_ConstArray_mod_tag; _temp3154->f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp3155= yyvs; struct _xenum_struct** _temp3157= _temp3155.curr
+( yyvsp_offset - 1); if( _temp3157 < _temp3155.base? 1: _temp3157 >= _temp3155.last_plus_one){
_throw( Null_Exception);}* _temp3157;}));( void*) _temp3154;}); _temp3150->tl=(
Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3151= yyvs; struct
_xenum_struct** _temp3153= _temp3151.curr +( yyvsp_offset - 3); if( _temp3153 <
_temp3151.base? 1: _temp3153 >= _temp3151.last_plus_one){ _throw( Null_Exception);}*
_temp3153;})))->tms; _temp3150;}); _temp3149;})};( struct _xenum_struct*)
_temp3148;}); break; case 111: _LL3147: { struct _tuple14 _temp3167; struct Cyc_Core_Opt*
_temp3168; int _temp3170; struct Cyc_List_List* _temp3172; struct _tuple14*
_temp3165= Cyc_yyget_ParamDeclListBool_tok(({ struct _tagged_ptr2 _temp3162=
yyvs; struct _xenum_struct** _temp3164= _temp3162.curr +( yyvsp_offset - 1); if(
_temp3164 < _temp3162.base? 1: _temp3164 >= _temp3162.last_plus_one){ _throw(
Null_Exception);}* _temp3164;})); _temp3167=* _temp3165; _LL3173: _temp3172=
_temp3167.f1; goto _LL3171; _LL3171: _temp3170= _temp3167.f2; goto _LL3169;
_LL3169: _temp3168= _temp3167.f3; goto _LL3166; _LL3166: yyval=({ struct Cyc_Declarator_tok_struct*
_temp3174=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));*
_temp3174=( struct Cyc_Declarator_tok_struct){.tag= Cyc_Declarator_tok_tag,.f1=({
struct Cyc_Parse_Declarator* _temp3175=( struct Cyc_Parse_Declarator*) GC_malloc(
sizeof( struct Cyc_Parse_Declarator)); _temp3175->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3182= yyvs; struct _xenum_struct** _temp3184= _temp3182.curr
+( yyvsp_offset - 3); if( _temp3184 < _temp3182.base? 1: _temp3184 >= _temp3182.last_plus_one){
_throw( Null_Exception);}* _temp3184;})))->id; _temp3175->tms=({ struct Cyc_List_List*
_temp3176=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3176->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct* _temp3180=(
struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3180->tag= Cyc_Absyn_Function_mod_tag; _temp3180->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp3181=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3181->tag= Cyc_Absyn_WithTypes_tag; _temp3181->f1= _temp3172; _temp3181->f2=
_temp3170; _temp3181->f3= _temp3168;( void*) _temp3181;});( void*) _temp3180;});
_temp3176->tl=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3177= yyvs;
struct _xenum_struct** _temp3179= _temp3177.curr +( yyvsp_offset - 3); if(
_temp3179 < _temp3177.base? 1: _temp3179 >= _temp3177.last_plus_one){ _throw(
Null_Exception);}* _temp3179;})))->tms; _temp3176;}); _temp3175;})};( struct
_xenum_struct*) _temp3174;}); break;} case 112: _LL3161: yyval=({ struct Cyc_Declarator_tok_struct*
_temp3186=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));*
_temp3186=( struct Cyc_Declarator_tok_struct){.tag= Cyc_Declarator_tok_tag,.f1=({
struct Cyc_Parse_Declarator* _temp3187=( struct Cyc_Parse_Declarator*) GC_malloc(
sizeof( struct Cyc_Parse_Declarator)); _temp3187->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3194= yyvs; struct _xenum_struct** _temp3196= _temp3194.curr
+( yyvsp_offset - 2); if( _temp3196 < _temp3194.base? 1: _temp3196 >= _temp3194.last_plus_one){
_throw( Null_Exception);}* _temp3196;})))->id; _temp3187->tms=({ struct Cyc_List_List*
_temp3188=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3188->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct* _temp3192=(
struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3192->tag= Cyc_Absyn_Function_mod_tag; _temp3192->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp3193=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3193->tag= Cyc_Absyn_WithTypes_tag; _temp3193->f1= 0; _temp3193->f2= 0;
_temp3193->f3= 0;( void*) _temp3193;});( void*) _temp3192;}); _temp3188->tl=(
Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3189= yyvs; struct
_xenum_struct** _temp3191= _temp3189.curr +( yyvsp_offset - 2); if( _temp3191 <
_temp3189.base? 1: _temp3191 >= _temp3189.last_plus_one){ _throw( Null_Exception);}*
_temp3191;})))->tms; _temp3188;}); _temp3187;})};( struct _xenum_struct*)
_temp3186;}); break; case 113: _LL3185: yyval=({ struct Cyc_Declarator_tok_struct*
_temp3198=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));*
_temp3198=( struct Cyc_Declarator_tok_struct){.tag= Cyc_Declarator_tok_tag,.f1=({
struct Cyc_Parse_Declarator* _temp3199=( struct Cyc_Parse_Declarator*) GC_malloc(
sizeof( struct Cyc_Parse_Declarator)); _temp3199->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3211= yyvs; struct _xenum_struct** _temp3213= _temp3211.curr
+( yyvsp_offset - 4); if( _temp3213 < _temp3211.base? 1: _temp3213 >= _temp3211.last_plus_one){
_throw( Null_Exception);}* _temp3213;})))->id; _temp3199->tms=({ struct Cyc_List_List*
_temp3200=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3200->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct* _temp3204=(
struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3204->tag= Cyc_Absyn_Function_mod_tag; _temp3204->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp3205=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3205->tag= Cyc_Absyn_WithTypes_tag; _temp3205->f1= 0; _temp3205->f2= 0;
_temp3205->f3=({ struct Cyc_Core_Opt* _temp3206=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3206->v=( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp3207=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3207->tag= Cyc_Absyn_JoinEff_tag; _temp3207->f1= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3208= yyvs; struct _xenum_struct** _temp3210= _temp3208.curr
+( yyvsp_offset - 1); if( _temp3210 < _temp3208.base? 1: _temp3210 >= _temp3208.last_plus_one){
_throw( Null_Exception);}* _temp3210;}));( void*) _temp3207;}); _temp3206;});(
void*) _temp3205;});( void*) _temp3204;}); _temp3200->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3201= yyvs; struct _xenum_struct** _temp3203= _temp3201.curr
+( yyvsp_offset - 4); if( _temp3203 < _temp3201.base? 1: _temp3203 >= _temp3201.last_plus_one){
_throw( Null_Exception);}* _temp3203;})))->tms; _temp3200;}); _temp3199;})};(
struct _xenum_struct*) _temp3198;}); break; case 114: _LL3197: yyval=({ struct
Cyc_Declarator_tok_struct* _temp3215=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));* _temp3215=( struct Cyc_Declarator_tok_struct){.tag=
Cyc_Declarator_tok_tag,.f1=({ struct Cyc_Parse_Declarator* _temp3216=( struct
Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp3216->id=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3232= yyvs;
struct _xenum_struct** _temp3234= _temp3232.curr +( yyvsp_offset - 3); if(
_temp3234 < _temp3232.base? 1: _temp3234 >= _temp3232.last_plus_one){ _throw(
Null_Exception);}* _temp3234;})))->id; _temp3216->tms=({ struct Cyc_List_List*
_temp3217=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3217->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct* _temp3221=(
struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3221->tag= Cyc_Absyn_Function_mod_tag; _temp3221->f1=( void*)({ struct Cyc_Absyn_NoTypes_struct*
_temp3222=( struct Cyc_Absyn_NoTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_NoTypes_struct));
_temp3222->tag= Cyc_Absyn_NoTypes_tag; _temp3222->f1= Cyc_yyget_IdList_tok(({
struct _tagged_ptr2 _temp3223= yyvs; struct _xenum_struct** _temp3225= _temp3223.curr
+( yyvsp_offset - 1); if( _temp3225 < _temp3223.base? 1: _temp3225 >= _temp3223.last_plus_one){
_throw( Null_Exception);}* _temp3225;})); _temp3222->f2= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3226= yyls; struct Cyc_Yyltype* _temp3228= _temp3226.curr
+( yylsp_offset - 3); if( _temp3228 < _temp3226.base? 1: _temp3228 >= _temp3226.last_plus_one){
_throw( Null_Exception);}* _temp3228;}).first_line,({ struct _tagged_ptr3
_temp3229= yyls; struct Cyc_Yyltype* _temp3231= _temp3229.curr + yylsp_offset;
if( _temp3231 < _temp3229.base? 1: _temp3231 >= _temp3229.last_plus_one){ _throw(
Null_Exception);}* _temp3231;}).last_line);( void*) _temp3222;});( void*)
_temp3221;}); _temp3217->tl=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2
_temp3218= yyvs; struct _xenum_struct** _temp3220= _temp3218.curr +(
yyvsp_offset - 3); if( _temp3220 < _temp3218.base? 1: _temp3220 >= _temp3218.last_plus_one){
_throw( Null_Exception);}* _temp3220;})))->tms; _temp3217;}); _temp3216;})};(
struct _xenum_struct*) _temp3215;}); break; case 115: _LL3214: { struct Cyc_List_List*
ts=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3236=
yyls; struct Cyc_Yyltype* _temp3238= _temp3236.curr +( yylsp_offset - 2); if(
_temp3238 < _temp3236.base? 1: _temp3238 >= _temp3236.last_plus_one){ _throw(
Null_Exception);}* _temp3238;}).first_line,({ struct _tagged_ptr3 _temp3239=
yyls; struct Cyc_Yyltype* _temp3241= _temp3239.curr + yylsp_offset; if(
_temp3241 < _temp3239.base? 1: _temp3241 >= _temp3239.last_plus_one){ _throw(
Null_Exception);}* _temp3241;}).last_line),(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp3242= yyvs; struct _xenum_struct** _temp3244= _temp3242.curr +(
yyvsp_offset - 1); if( _temp3244 < _temp3242.base? 1: _temp3244 >= _temp3242.last_plus_one){
_throw( Null_Exception);}* _temp3244;})))); yyval=({ struct Cyc_Declarator_tok_struct*
_temp3245=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));*
_temp3245=( struct Cyc_Declarator_tok_struct){.tag= Cyc_Declarator_tok_tag,.f1=({
struct Cyc_Parse_Declarator* _temp3246=( struct Cyc_Parse_Declarator*) GC_malloc(
sizeof( struct Cyc_Parse_Declarator)); _temp3246->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3258= yyvs; struct _xenum_struct** _temp3260= _temp3258.curr
+( yyvsp_offset - 3); if( _temp3260 < _temp3258.base? 1: _temp3260 >= _temp3258.last_plus_one){
_throw( Null_Exception);}* _temp3260;})))->id; _temp3246->tms=({ struct Cyc_List_List*
_temp3247=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3247->hd=( void*)({ struct Cyc_Absyn_TypeParams_mod_struct* _temp3251=(
struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct));
_temp3251->tag= Cyc_Absyn_TypeParams_mod_tag; _temp3251->f1= ts; _temp3251->f2=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3252= yyls; struct Cyc_Yyltype*
_temp3254= _temp3252.curr +( yylsp_offset - 3); if( _temp3254 < _temp3252.base?
1: _temp3254 >= _temp3252.last_plus_one){ _throw( Null_Exception);}* _temp3254;}).first_line,({
struct _tagged_ptr3 _temp3255= yyls; struct Cyc_Yyltype* _temp3257= _temp3255.curr
+ yylsp_offset; if( _temp3257 < _temp3255.base? 1: _temp3257 >= _temp3255.last_plus_one){
_throw( Null_Exception);}* _temp3257;}).last_line); _temp3251->f3= 0;( void*)
_temp3251;}); _temp3247->tl=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2
_temp3248= yyvs; struct _xenum_struct** _temp3250= _temp3248.curr +(
yyvsp_offset - 3); if( _temp3250 < _temp3248.base? 1: _temp3250 >= _temp3248.last_plus_one){
_throw( Null_Exception);}* _temp3250;})))->tms; _temp3247;}); _temp3246;})};(
struct _xenum_struct*) _temp3245;}); break;} case 116: _LL3235:(( struct Cyc_Lexing_lexbuf*)
Cyc_Parse_lbuf->v)->lex_curr_pos -= 1;{ struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3262= yyls; struct Cyc_Yyltype* _temp3264= _temp3262.curr
+( yylsp_offset - 2); if( _temp3264 < _temp3262.base? 1: _temp3264 >= _temp3262.last_plus_one){
_throw( Null_Exception);}* _temp3264;}).first_line,({ struct _tagged_ptr3
_temp3265= yyls; struct Cyc_Yyltype* _temp3267= _temp3265.curr + yylsp_offset;
if( _temp3267 < _temp3265.base? 1: _temp3267 >= _temp3265.last_plus_one){ _throw(
Null_Exception);}* _temp3267;}).last_line),(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp3268= yyvs; struct _xenum_struct** _temp3270= _temp3268.curr +(
yyvsp_offset - 1); if( _temp3270 < _temp3268.base? 1: _temp3270 >= _temp3268.last_plus_one){
_throw( Null_Exception);}* _temp3270;})))); yyval=({ struct Cyc_Declarator_tok_struct*
_temp3271=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));*
_temp3271=( struct Cyc_Declarator_tok_struct){.tag= Cyc_Declarator_tok_tag,.f1=({
struct Cyc_Parse_Declarator* _temp3272=( struct Cyc_Parse_Declarator*) GC_malloc(
sizeof( struct Cyc_Parse_Declarator)); _temp3272->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3284= yyvs; struct _xenum_struct** _temp3286= _temp3284.curr
+( yyvsp_offset - 3); if( _temp3286 < _temp3284.base? 1: _temp3286 >= _temp3284.last_plus_one){
_throw( Null_Exception);}* _temp3286;})))->id; _temp3272->tms=({ struct Cyc_List_List*
_temp3273=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3273->hd=( void*)({ struct Cyc_Absyn_TypeParams_mod_struct* _temp3277=(
struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct));
_temp3277->tag= Cyc_Absyn_TypeParams_mod_tag; _temp3277->f1= ts; _temp3277->f2=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3278= yyls; struct Cyc_Yyltype*
_temp3280= _temp3278.curr +( yylsp_offset - 3); if( _temp3280 < _temp3278.base?
1: _temp3280 >= _temp3278.last_plus_one){ _throw( Null_Exception);}* _temp3280;}).first_line,({
struct _tagged_ptr3 _temp3281= yyls; struct Cyc_Yyltype* _temp3283= _temp3281.curr
+ yylsp_offset; if( _temp3283 < _temp3281.base? 1: _temp3283 >= _temp3281.last_plus_one){
_throw( Null_Exception);}* _temp3283;}).last_line); _temp3277->f3= 0;( void*)
_temp3277;}); _temp3273->tl=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2
_temp3274= yyvs; struct _xenum_struct** _temp3276= _temp3274.curr +(
yyvsp_offset - 3); if( _temp3276 < _temp3274.base? 1: _temp3276 >= _temp3274.last_plus_one){
_throw( Null_Exception);}* _temp3276;})))->tms; _temp3273;}); _temp3272;})};(
struct _xenum_struct*) _temp3271;}); break;} case 117: _LL3261: yyval=({ struct
Cyc_TypeModifierList_tok_struct* _temp3288=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct));* _temp3288=( struct
Cyc_TypeModifierList_tok_struct){.tag= Cyc_TypeModifierList_tok_tag,.f1=({
struct Cyc_List_List* _temp3289=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3289->hd=( void*)({ struct Cyc_Absyn_Pointer_mod_struct*
_temp3290=( struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp3290->tag= Cyc_Absyn_Pointer_mod_tag; _temp3290->f1=( void*) Cyc_yyget_Pointer_Sort_tok(({
struct _tagged_ptr2 _temp3291= yyvs; struct _xenum_struct** _temp3293= _temp3291.curr
+( yyvsp_offset - 1); if( _temp3293 < _temp3291.base? 1: _temp3293 >= _temp3291.last_plus_one){
_throw( Null_Exception);}* _temp3293;})); _temp3290->f2=( void*) Cyc_yyget_Rgn_tok(({
struct _tagged_ptr2 _temp3294= yyvs; struct _xenum_struct** _temp3296= _temp3294.curr
+ yyvsp_offset; if( _temp3296 < _temp3294.base? 1: _temp3296 >= _temp3294.last_plus_one){
_throw( Null_Exception);}* _temp3296;})); _temp3290->f3= Cyc_Absyn_empty_tqual();(
void*) _temp3290;}); _temp3289->tl= 0; _temp3289;})};( struct _xenum_struct*)
_temp3288;}); break; case 118: _LL3287: yyval=({ struct Cyc_TypeModifierList_tok_struct*
_temp3298=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct));* _temp3298=( struct Cyc_TypeModifierList_tok_struct){.tag=
Cyc_TypeModifierList_tok_tag,.f1=({ struct Cyc_List_List* _temp3299=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3299->hd=( void*)({ struct Cyc_Absyn_Pointer_mod_struct*
_temp3300=( struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp3300->tag= Cyc_Absyn_Pointer_mod_tag; _temp3300->f1=( void*) Cyc_yyget_Pointer_Sort_tok(({
struct _tagged_ptr2 _temp3301= yyvs; struct _xenum_struct** _temp3303= _temp3301.curr
+( yyvsp_offset - 2); if( _temp3303 < _temp3301.base? 1: _temp3303 >= _temp3301.last_plus_one){
_throw( Null_Exception);}* _temp3303;})); _temp3300->f2=( void*) Cyc_yyget_Rgn_tok(({
struct _tagged_ptr2 _temp3304= yyvs; struct _xenum_struct** _temp3306= _temp3304.curr
+( yyvsp_offset - 1); if( _temp3306 < _temp3304.base? 1: _temp3306 >= _temp3304.last_plus_one){
_throw( Null_Exception);}* _temp3306;})); _temp3300->f3= Cyc_yyget_TypeQual_tok(({
struct _tagged_ptr2 _temp3307= yyvs; struct _xenum_struct** _temp3309= _temp3307.curr
+ yyvsp_offset; if( _temp3309 < _temp3307.base? 1: _temp3309 >= _temp3307.last_plus_one){
_throw( Null_Exception);}* _temp3309;}));( void*) _temp3300;}); _temp3299->tl= 0;
_temp3299;})};( struct _xenum_struct*) _temp3298;}); break; case 119: _LL3297:
yyval=({ struct Cyc_TypeModifierList_tok_struct* _temp3311=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct));* _temp3311=( struct
Cyc_TypeModifierList_tok_struct){.tag= Cyc_TypeModifierList_tok_tag,.f1=({
struct Cyc_List_List* _temp3312=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3312->hd=( void*)({ struct Cyc_Absyn_Pointer_mod_struct*
_temp3316=( struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp3316->tag= Cyc_Absyn_Pointer_mod_tag; _temp3316->f1=( void*) Cyc_yyget_Pointer_Sort_tok(({
struct _tagged_ptr2 _temp3317= yyvs; struct _xenum_struct** _temp3319= _temp3317.curr
+( yyvsp_offset - 2); if( _temp3319 < _temp3317.base? 1: _temp3319 >= _temp3317.last_plus_one){
_throw( Null_Exception);}* _temp3319;})); _temp3316->f2=( void*) Cyc_yyget_Rgn_tok(({
struct _tagged_ptr2 _temp3320= yyvs; struct _xenum_struct** _temp3322= _temp3320.curr
+( yyvsp_offset - 1); if( _temp3322 < _temp3320.base? 1: _temp3322 >= _temp3320.last_plus_one){
_throw( Null_Exception);}* _temp3322;})); _temp3316->f3= Cyc_Absyn_empty_tqual();(
void*) _temp3316;}); _temp3312->tl= Cyc_yyget_TypeModifierList_tok(({ struct
_tagged_ptr2 _temp3313= yyvs; struct _xenum_struct** _temp3315= _temp3313.curr +
yyvsp_offset; if( _temp3315 < _temp3313.base? 1: _temp3315 >= _temp3313.last_plus_one){
_throw( Null_Exception);}* _temp3315;})); _temp3312;})};( struct _xenum_struct*)
_temp3311;}); break; case 120: _LL3310: yyval=({ struct Cyc_TypeModifierList_tok_struct*
_temp3324=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct));* _temp3324=( struct Cyc_TypeModifierList_tok_struct){.tag=
Cyc_TypeModifierList_tok_tag,.f1=({ struct Cyc_List_List* _temp3325=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3325->hd=( void*)({ struct Cyc_Absyn_Pointer_mod_struct*
_temp3329=( struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp3329->tag= Cyc_Absyn_Pointer_mod_tag; _temp3329->f1=( void*) Cyc_yyget_Pointer_Sort_tok(({
struct _tagged_ptr2 _temp3330= yyvs; struct _xenum_struct** _temp3332= _temp3330.curr
+( yyvsp_offset - 3); if( _temp3332 < _temp3330.base? 1: _temp3332 >= _temp3330.last_plus_one){
_throw( Null_Exception);}* _temp3332;})); _temp3329->f2=( void*) Cyc_yyget_Rgn_tok(({
struct _tagged_ptr2 _temp3333= yyvs; struct _xenum_struct** _temp3335= _temp3333.curr
+( yyvsp_offset - 2); if( _temp3335 < _temp3333.base? 1: _temp3335 >= _temp3333.last_plus_one){
_throw( Null_Exception);}* _temp3335;})); _temp3329->f3= Cyc_yyget_TypeQual_tok(({
struct _tagged_ptr2 _temp3336= yyvs; struct _xenum_struct** _temp3338= _temp3336.curr
+( yyvsp_offset - 1); if( _temp3338 < _temp3336.base? 1: _temp3338 >= _temp3336.last_plus_one){
_throw( Null_Exception);}* _temp3338;}));( void*) _temp3329;}); _temp3325->tl=
Cyc_yyget_TypeModifierList_tok(({ struct _tagged_ptr2 _temp3326= yyvs; struct
_xenum_struct** _temp3328= _temp3326.curr + yyvsp_offset; if( _temp3328 <
_temp3326.base? 1: _temp3328 >= _temp3326.last_plus_one){ _throw( Null_Exception);}*
_temp3328;})); _temp3325;})};( struct _xenum_struct*) _temp3324;}); break; case
121: _LL3323: yyval=({ struct Cyc_Pointer_Sort_tok_struct* _temp3340=( struct
Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));*
_temp3340=( struct Cyc_Pointer_Sort_tok_struct){.tag= Cyc_Pointer_Sort_tok_tag,.f1=({
struct Cyc_Absyn_Nullable_ps_struct* _temp3341=( struct Cyc_Absyn_Nullable_ps_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct)); _temp3341->tag= Cyc_Absyn_Nullable_ps_tag;
_temp3341->f1= Cyc_Absyn_signed_int_exp( 1, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3342= yyls; struct Cyc_Yyltype* _temp3344= _temp3342.curr
+ yylsp_offset; if( _temp3344 < _temp3342.base? 1: _temp3344 >= _temp3342.last_plus_one){
_throw( Null_Exception);}* _temp3344;}).first_line,({ struct _tagged_ptr3
_temp3345= yyls; struct Cyc_Yyltype* _temp3347= _temp3345.curr + yylsp_offset;
if( _temp3347 < _temp3345.base? 1: _temp3347 >= _temp3345.last_plus_one){ _throw(
Null_Exception);}* _temp3347;}).last_line));( void*) _temp3341;})};( struct
_xenum_struct*) _temp3340;}); break; case 122: _LL3339: yyval=({ struct Cyc_Pointer_Sort_tok_struct*
_temp3349=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));*
_temp3349=( struct Cyc_Pointer_Sort_tok_struct){.tag= Cyc_Pointer_Sort_tok_tag,.f1=({
struct Cyc_Absyn_NonNullable_ps_struct* _temp3350=( struct Cyc_Absyn_NonNullable_ps_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_NonNullable_ps_struct)); _temp3350->tag= Cyc_Absyn_NonNullable_ps_tag;
_temp3350->f1= Cyc_Absyn_signed_int_exp( 1, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3351= yyls; struct Cyc_Yyltype* _temp3353= _temp3351.curr
+ yylsp_offset; if( _temp3353 < _temp3351.base? 1: _temp3353 >= _temp3351.last_plus_one){
_throw( Null_Exception);}* _temp3353;}).first_line,({ struct _tagged_ptr3
_temp3354= yyls; struct Cyc_Yyltype* _temp3356= _temp3354.curr + yylsp_offset;
if( _temp3356 < _temp3354.base? 1: _temp3356 >= _temp3354.last_plus_one){ _throw(
Null_Exception);}* _temp3356;}).last_line));( void*) _temp3350;})};( struct
_xenum_struct*) _temp3349;}); break; case 123: _LL3348: yyval=({ struct Cyc_Pointer_Sort_tok_struct*
_temp3358=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));*
_temp3358=( struct Cyc_Pointer_Sort_tok_struct){.tag= Cyc_Pointer_Sort_tok_tag,.f1=({
struct Cyc_Absyn_Nullable_ps_struct* _temp3359=( struct Cyc_Absyn_Nullable_ps_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct)); _temp3359->tag= Cyc_Absyn_Nullable_ps_tag;
_temp3359->f1= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp3360= yyvs; struct
_xenum_struct** _temp3362= _temp3360.curr +( yyvsp_offset - 1); if( _temp3362 <
_temp3360.base? 1: _temp3362 >= _temp3360.last_plus_one){ _throw( Null_Exception);}*
_temp3362;}));( void*) _temp3359;})};( struct _xenum_struct*) _temp3358;});
break; case 124: _LL3357: yyval=({ struct Cyc_Pointer_Sort_tok_struct* _temp3364=(
struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));*
_temp3364=( struct Cyc_Pointer_Sort_tok_struct){.tag= Cyc_Pointer_Sort_tok_tag,.f1=({
struct Cyc_Absyn_NonNullable_ps_struct* _temp3365=( struct Cyc_Absyn_NonNullable_ps_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_NonNullable_ps_struct)); _temp3365->tag= Cyc_Absyn_NonNullable_ps_tag;
_temp3365->f1= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp3366= yyvs; struct
_xenum_struct** _temp3368= _temp3366.curr +( yyvsp_offset - 1); if( _temp3368 <
_temp3366.base? 1: _temp3368 >= _temp3366.last_plus_one){ _throw( Null_Exception);}*
_temp3368;}));( void*) _temp3365;})};( struct _xenum_struct*) _temp3364;});
break; case 125: _LL3363: yyval=({ struct Cyc_Pointer_Sort_tok_struct* _temp3370=(
struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));*
_temp3370=( struct Cyc_Pointer_Sort_tok_struct){.tag= Cyc_Pointer_Sort_tok_tag,.f1=
Cyc_Absyn_TaggedArray_ps};( struct _xenum_struct*) _temp3370;}); break; case 126:
_LL3369: yyval=({ struct Cyc_Rgn_tok_struct* _temp3372=( struct Cyc_Rgn_tok_struct*)
GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));* _temp3372=( struct Cyc_Rgn_tok_struct){.tag=
Cyc_Rgn_tok_tag,.f1= Cyc_Absyn_HeapRgn};( struct _xenum_struct*) _temp3372;});
break; case 127: _LL3371: yyval=({ struct Cyc_Rgn_tok_struct* _temp3374=( struct
Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));* _temp3374=(
struct Cyc_Rgn_tok_struct){.tag= Cyc_Rgn_tok_tag,.f1= Cyc_Parse_id2type( Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp3375= yyvs; struct _xenum_struct** _temp3377= _temp3375.curr
+ yyvsp_offset; if( _temp3377 < _temp3375.base? 1: _temp3377 >= _temp3375.last_plus_one){
_throw( Null_Exception);}* _temp3377;})),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)( Cyc_Absyn_RgnKind))};( struct _xenum_struct*) _temp3374;});
break; case 128: _LL3373: if( Cyc_yyget_Kind_tok(({ struct _tagged_ptr2
_temp3379= yyvs; struct _xenum_struct** _temp3381= _temp3379.curr + yyvsp_offset;
if( _temp3381 < _temp3379.base? 1: _temp3381 >= _temp3379.last_plus_one){ _throw(
Null_Exception);}* _temp3381;})) != Cyc_Absyn_RgnKind){ Cyc_Parse_err(( struct
_tagged_string)({ char* _temp3382=( char*)"expecting region kind\n"; struct
_tagged_string _temp3383; _temp3383.curr= _temp3382; _temp3383.base= _temp3382;
_temp3383.last_plus_one= _temp3382 + 23; _temp3383;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3384= yyls; struct Cyc_Yyltype* _temp3386= _temp3384.curr
+ yylsp_offset; if( _temp3386 < _temp3384.base? 1: _temp3386 >= _temp3384.last_plus_one){
_throw( Null_Exception);}* _temp3386;}).first_line,({ struct _tagged_ptr3
_temp3387= yyls; struct Cyc_Yyltype* _temp3389= _temp3387.curr + yylsp_offset;
if( _temp3389 < _temp3387.base? 1: _temp3389 >= _temp3387.last_plus_one){ _throw(
Null_Exception);}* _temp3389;}).last_line));} yyval=({ struct Cyc_Rgn_tok_struct*
_temp3390=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));*
_temp3390=( struct Cyc_Rgn_tok_struct){.tag= Cyc_Rgn_tok_tag,.f1= Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp3391= yyvs; struct
_xenum_struct** _temp3393= _temp3391.curr +( yyvsp_offset - 2); if( _temp3393 <
_temp3391.base? 1: _temp3393 >= _temp3391.last_plus_one){ _throw( Null_Exception);}*
_temp3393;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
Cyc_Absyn_RgnKind))};( struct _xenum_struct*) _temp3390;}); break; case 129:
_LL3378: yyval=({ struct Cyc_Rgn_tok_struct* _temp3395=( struct Cyc_Rgn_tok_struct*)
GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));* _temp3395=( struct Cyc_Rgn_tok_struct){.tag=
Cyc_Rgn_tok_tag,.f1= Cyc_Absyn_new_evar( Cyc_Absyn_RgnKind)};( struct
_xenum_struct*) _temp3395;}); break; case 130: _LL3394: yyval=({ struct
_tagged_ptr2 _temp3397= yyvs; struct _xenum_struct** _temp3399= _temp3397.curr +
yyvsp_offset; if( _temp3399 < _temp3397.base? 1: _temp3399 >= _temp3397.last_plus_one){
_throw( Null_Exception);}* _temp3399;}); break; case 131: _LL3396: yyval=({
struct Cyc_TypeQual_tok_struct* _temp3401=( struct Cyc_TypeQual_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct));* _temp3401=( struct Cyc_TypeQual_tok_struct){.tag=
Cyc_TypeQual_tok_tag,.f1= Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok(({
struct _tagged_ptr2 _temp3402= yyvs; struct _xenum_struct** _temp3404= _temp3402.curr
+( yyvsp_offset - 1); if( _temp3404 < _temp3402.base? 1: _temp3404 >= _temp3402.last_plus_one){
_throw( Null_Exception);}* _temp3404;})), Cyc_yyget_TypeQual_tok(({ struct
_tagged_ptr2 _temp3405= yyvs; struct _xenum_struct** _temp3407= _temp3405.curr +
yyvsp_offset; if( _temp3407 < _temp3405.base? 1: _temp3407 >= _temp3405.last_plus_one){
_throw( Null_Exception);}* _temp3407;})))};( struct _xenum_struct*) _temp3401;});
break; case 132: _LL3400: yyval=({ struct Cyc_ParamDeclListBool_tok_struct*
_temp3409=( struct Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct
Cyc_ParamDeclListBool_tok_struct));* _temp3409=( struct Cyc_ParamDeclListBool_tok_struct){.tag=
Cyc_ParamDeclListBool_tok_tag,.f1=({ struct _tuple14* _temp3410=( struct
_tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp3410->f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp3411= yyvs; struct _xenum_struct** _temp3413= _temp3411.curr
+ yyvsp_offset; if( _temp3413 < _temp3411.base? 1: _temp3413 >= _temp3411.last_plus_one){
_throw( Null_Exception);}* _temp3413;}))); _temp3410->f2= 0; _temp3410->f3= 0;
_temp3410;})};( struct _xenum_struct*) _temp3409;}); break; case 133: _LL3408:
yyval=({ struct Cyc_ParamDeclListBool_tok_struct* _temp3415=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct));* _temp3415=(
struct Cyc_ParamDeclListBool_tok_struct){.tag= Cyc_ParamDeclListBool_tok_tag,.f1=({
struct _tuple14* _temp3416=( struct _tuple14*) GC_malloc( sizeof( struct
_tuple14)); _temp3416->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({ struct _tagged_ptr2 _temp3417=
yyvs; struct _xenum_struct** _temp3419= _temp3417.curr +( yyvsp_offset - 2); if(
_temp3419 < _temp3417.base? 1: _temp3419 >= _temp3417.last_plus_one){ _throw(
Null_Exception);}* _temp3419;}))); _temp3416->f2= 1; _temp3416->f3= 0; _temp3416;})};(
struct _xenum_struct*) _temp3415;}); break; case 134: _LL3414: yyval=({ struct
Cyc_ParamDeclListBool_tok_struct* _temp3421=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct));* _temp3421=(
struct Cyc_ParamDeclListBool_tok_struct){.tag= Cyc_ParamDeclListBool_tok_tag,.f1=({
struct _tuple14* _temp3422=( struct _tuple14*) GC_malloc( sizeof( struct
_tuple14)); _temp3422->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({ struct _tagged_ptr2 _temp3428=
yyvs; struct _xenum_struct** _temp3430= _temp3428.curr +( yyvsp_offset - 2); if(
_temp3430 < _temp3428.base? 1: _temp3430 >= _temp3428.last_plus_one){ _throw(
Null_Exception);}* _temp3430;}))); _temp3422->f2= 0; _temp3422->f3=( struct Cyc_Core_Opt*)({
struct Cyc_Core_Opt* _temp3423=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3423->v=( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp3424=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3424->tag= Cyc_Absyn_JoinEff_tag; _temp3424->f1= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3425= yyvs; struct _xenum_struct** _temp3427= _temp3425.curr
+ yyvsp_offset; if( _temp3427 < _temp3425.base? 1: _temp3427 >= _temp3425.last_plus_one){
_throw( Null_Exception);}* _temp3427;}));( void*) _temp3424;}); _temp3423;});
_temp3422;})};( struct _xenum_struct*) _temp3421;}); break; case 135: _LL3420:
yyval=({ struct _tagged_ptr2 _temp3432= yyvs; struct _xenum_struct** _temp3434=
_temp3432.curr + yyvsp_offset; if( _temp3434 < _temp3432.base? 1: _temp3434 >=
_temp3432.last_plus_one){ _throw( Null_Exception);}* _temp3434;}); break; case
136: _LL3431: yyval=({ struct Cyc_TypeList_tok_struct* _temp3436=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));* _temp3436=( struct Cyc_TypeList_tok_struct){.tag=
Cyc_TypeList_tok_tag,.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp3437= yyvs; struct _xenum_struct** _temp3439= _temp3437.curr +(
yyvsp_offset - 2); if( _temp3439 < _temp3437.base? 1: _temp3439 >= _temp3437.last_plus_one){
_throw( Null_Exception);}* _temp3439;})), Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp3440= yyvs; struct _xenum_struct** _temp3442= _temp3440.curr +
yyvsp_offset; if( _temp3442 < _temp3440.base? 1: _temp3442 >= _temp3440.last_plus_one){
_throw( Null_Exception);}* _temp3442;})))};( struct _xenum_struct*) _temp3436;});
break; case 137: _LL3435: yyval=({ struct Cyc_TypeList_tok_struct* _temp3444=(
struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));*
_temp3444=( struct Cyc_TypeList_tok_struct){.tag= Cyc_TypeList_tok_tag,.f1= 0};(
struct _xenum_struct*) _temp3444;}); break; case 138: _LL3443: yyval=({ struct
_tagged_ptr2 _temp3446= yyvs; struct _xenum_struct** _temp3448= _temp3446.curr +(
yyvsp_offset - 1); if( _temp3448 < _temp3446.base? 1: _temp3448 >= _temp3446.last_plus_one){
_throw( Null_Exception);}* _temp3448;}); break; case 139: _LL3445: yyval=({
struct Cyc_TypeList_tok_struct* _temp3450=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));* _temp3450=( struct Cyc_TypeList_tok_struct){.tag=
Cyc_TypeList_tok_tag,.f1=({ struct Cyc_List_List* _temp3451=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3451->hd=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp3452= yyvs; struct
_xenum_struct** _temp3454= _temp3452.curr + yyvsp_offset; if( _temp3454 <
_temp3452.base? 1: _temp3454 >= _temp3452.last_plus_one){ _throw( Null_Exception);}*
_temp3454;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
Cyc_Absyn_EffKind)); _temp3451->tl= 0; _temp3451;})};( struct _xenum_struct*)
_temp3450;}); break; case 140: _LL3449: if( Cyc_yyget_Kind_tok(({ struct
_tagged_ptr2 _temp3456= yyvs; struct _xenum_struct** _temp3458= _temp3456.curr +
yyvsp_offset; if( _temp3458 < _temp3456.base? 1: _temp3458 >= _temp3456.last_plus_one){
_throw( Null_Exception);}* _temp3458;})) != Cyc_Absyn_EffKind){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp3459=( char*)"expecing effect kind (E)";
struct _tagged_string _temp3460; _temp3460.curr= _temp3459; _temp3460.base=
_temp3459; _temp3460.last_plus_one= _temp3459 + 25; _temp3460;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3461= yyls; struct Cyc_Yyltype* _temp3463= _temp3461.curr
+ yylsp_offset; if( _temp3463 < _temp3461.base? 1: _temp3463 >= _temp3461.last_plus_one){
_throw( Null_Exception);}* _temp3463;}).first_line,({ struct _tagged_ptr3
_temp3464= yyls; struct Cyc_Yyltype* _temp3466= _temp3464.curr + yylsp_offset;
if( _temp3466 < _temp3464.base? 1: _temp3466 >= _temp3464.last_plus_one){ _throw(
Null_Exception);}* _temp3466;}).last_line));} yyval=({ struct Cyc_TypeList_tok_struct*
_temp3467=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));*
_temp3467=( struct Cyc_TypeList_tok_struct){.tag= Cyc_TypeList_tok_tag,.f1=({
struct Cyc_List_List* _temp3468=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3468->hd=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp3469= yyvs; struct _xenum_struct** _temp3471= _temp3469.curr
+( yyvsp_offset - 2); if( _temp3471 < _temp3469.base? 1: _temp3471 >= _temp3469.last_plus_one){
_throw( Null_Exception);}* _temp3471;})),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)( Cyc_Absyn_EffKind)); _temp3468->tl= 0; _temp3468;})};(
struct _xenum_struct*) _temp3467;}); break; case 141: _LL3455: yyval=({ struct
Cyc_TypeList_tok_struct* _temp3473=( struct Cyc_TypeList_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeList_tok_struct));* _temp3473=( struct Cyc_TypeList_tok_struct){.tag=
Cyc_TypeList_tok_tag,.f1=({ struct Cyc_List_List* _temp3474=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3474->hd=( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp3475=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3475->tag= Cyc_Absyn_AccessEff_tag; _temp3475->f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp3476= yyvs; struct
_xenum_struct** _temp3478= _temp3476.curr + yyvsp_offset; if( _temp3478 <
_temp3476.base? 1: _temp3478 >= _temp3476.last_plus_one){ _throw( Null_Exception);}*
_temp3478;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
Cyc_Absyn_RgnKind));( void*) _temp3475;}); _temp3474->tl= 0; _temp3474;})};(
struct _xenum_struct*) _temp3473;}); break; case 142: _LL3472: yyval=({ struct
Cyc_TypeList_tok_struct* _temp3480=( struct Cyc_TypeList_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeList_tok_struct));* _temp3480=( struct Cyc_TypeList_tok_struct){.tag=
Cyc_TypeList_tok_tag,.f1=({ struct Cyc_List_List* _temp3481=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3481->hd=( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp3485=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3485->tag= Cyc_Absyn_AccessEff_tag; _temp3485->f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp3486= yyvs; struct
_xenum_struct** _temp3488= _temp3486.curr +( yyvsp_offset - 2); if( _temp3488 <
_temp3486.base? 1: _temp3488 >= _temp3486.last_plus_one){ _throw( Null_Exception);}*
_temp3488;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
Cyc_Absyn_RgnKind));( void*) _temp3485;}); _temp3481->tl= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3482= yyvs; struct _xenum_struct** _temp3484= _temp3482.curr
+ yyvsp_offset; if( _temp3484 < _temp3482.base? 1: _temp3484 >= _temp3482.last_plus_one){
_throw( Null_Exception);}* _temp3484;})); _temp3481;})};( struct _xenum_struct*)
_temp3480;}); break; case 143: _LL3479: if( Cyc_yyget_Kind_tok(({ struct
_tagged_ptr2 _temp3490= yyvs; struct _xenum_struct** _temp3492= _temp3490.curr +
yyvsp_offset; if( _temp3492 < _temp3490.base? 1: _temp3492 >= _temp3490.last_plus_one){
_throw( Null_Exception);}* _temp3492;})) != Cyc_Absyn_RgnKind){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp3493=( char*)"expecting region kind (R)";
struct _tagged_string _temp3494; _temp3494.curr= _temp3493; _temp3494.base=
_temp3493; _temp3494.last_plus_one= _temp3493 + 26; _temp3494;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3495= yyls; struct Cyc_Yyltype* _temp3497= _temp3495.curr
+ yylsp_offset; if( _temp3497 < _temp3495.base? 1: _temp3497 >= _temp3495.last_plus_one){
_throw( Null_Exception);}* _temp3497;}).first_line,({ struct _tagged_ptr3
_temp3498= yyls; struct Cyc_Yyltype* _temp3500= _temp3498.curr + yylsp_offset;
if( _temp3500 < _temp3498.base? 1: _temp3500 >= _temp3498.last_plus_one){ _throw(
Null_Exception);}* _temp3500;}).last_line));} yyval=({ struct Cyc_TypeList_tok_struct*
_temp3501=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));*
_temp3501=( struct Cyc_TypeList_tok_struct){.tag= Cyc_TypeList_tok_tag,.f1=({
struct Cyc_List_List* _temp3502=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3502->hd=( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp3503=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3503->tag= Cyc_Absyn_AccessEff_tag; _temp3503->f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp3504= yyvs; struct
_xenum_struct** _temp3506= _temp3504.curr +( yyvsp_offset - 2); if( _temp3506 <
_temp3504.base? 1: _temp3506 >= _temp3504.last_plus_one){ _throw( Null_Exception);}*
_temp3506;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
Cyc_Absyn_RgnKind));( void*) _temp3503;}); _temp3502->tl= 0; _temp3502;})};(
struct _xenum_struct*) _temp3501;}); break; case 144: _LL3489: if( Cyc_yyget_Kind_tok(({
struct _tagged_ptr2 _temp3508= yyvs; struct _xenum_struct** _temp3510= _temp3508.curr
+( yyvsp_offset - 2); if( _temp3510 < _temp3508.base? 1: _temp3510 >= _temp3508.last_plus_one){
_throw( Null_Exception);}* _temp3510;})) != Cyc_Absyn_RgnKind){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp3511=( char*)"expecting region kind (R)";
struct _tagged_string _temp3512; _temp3512.curr= _temp3511; _temp3512.base=
_temp3511; _temp3512.last_plus_one= _temp3511 + 26; _temp3512;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3513= yyls; struct Cyc_Yyltype* _temp3515= _temp3513.curr
+( yylsp_offset - 2); if( _temp3515 < _temp3513.base? 1: _temp3515 >= _temp3513.last_plus_one){
_throw( Null_Exception);}* _temp3515;}).first_line,({ struct _tagged_ptr3
_temp3516= yyls; struct Cyc_Yyltype* _temp3518= _temp3516.curr +( yylsp_offset -
2); if( _temp3518 < _temp3516.base? 1: _temp3518 >= _temp3516.last_plus_one){
_throw( Null_Exception);}* _temp3518;}).last_line));} yyval=({ struct Cyc_TypeList_tok_struct*
_temp3519=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));*
_temp3519=( struct Cyc_TypeList_tok_struct){.tag= Cyc_TypeList_tok_tag,.f1=({
struct Cyc_List_List* _temp3520=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3520->hd=( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp3524=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3524->tag= Cyc_Absyn_AccessEff_tag; _temp3524->f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp3525= yyvs; struct
_xenum_struct** _temp3527= _temp3525.curr +( yyvsp_offset - 4); if( _temp3527 <
_temp3525.base? 1: _temp3527 >= _temp3525.last_plus_one){ _throw( Null_Exception);}*
_temp3527;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
Cyc_Absyn_RgnKind));( void*) _temp3524;}); _temp3520->tl= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3521= yyvs; struct _xenum_struct** _temp3523= _temp3521.curr
+ yyvsp_offset; if( _temp3523 < _temp3521.base? 1: _temp3523 >= _temp3521.last_plus_one){
_throw( Null_Exception);}* _temp3523;})); _temp3520;})};( struct _xenum_struct*)
_temp3519;}); break; case 145: _LL3507: yyval=({ struct Cyc_ParamDeclList_tok_struct*
_temp3529=( struct Cyc_ParamDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct));*
_temp3529=( struct Cyc_ParamDeclList_tok_struct){.tag= Cyc_ParamDeclList_tok_tag,.f1=({
struct Cyc_List_List* _temp3530=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3530->hd=( void*) Cyc_yyget_ParamDecl_tok(({ struct
_tagged_ptr2 _temp3531= yyvs; struct _xenum_struct** _temp3533= _temp3531.curr +
yyvsp_offset; if( _temp3533 < _temp3531.base? 1: _temp3533 >= _temp3531.last_plus_one){
_throw( Null_Exception);}* _temp3533;})); _temp3530->tl= 0; _temp3530;})};(
struct _xenum_struct*) _temp3529;}); break; case 146: _LL3528: yyval=({ struct
Cyc_ParamDeclList_tok_struct* _temp3535=( struct Cyc_ParamDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct));* _temp3535=( struct
Cyc_ParamDeclList_tok_struct){.tag= Cyc_ParamDeclList_tok_tag,.f1=({ struct Cyc_List_List*
_temp3536=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3536->hd=( void*) Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp3540=
yyvs; struct _xenum_struct** _temp3542= _temp3540.curr + yyvsp_offset; if(
_temp3542 < _temp3540.base? 1: _temp3542 >= _temp3540.last_plus_one){ _throw(
Null_Exception);}* _temp3542;})); _temp3536->tl= Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp3537= yyvs; struct _xenum_struct** _temp3539= _temp3537.curr
+( yyvsp_offset - 2); if( _temp3539 < _temp3537.base? 1: _temp3539 >= _temp3537.last_plus_one){
_throw( Null_Exception);}* _temp3539;})); _temp3536;})};( struct _xenum_struct*)
_temp3535;}); break; case 147: _LL3534: { struct Cyc_List_List* tss=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3544= yyvs; struct _xenum_struct** _temp3546= _temp3544.curr
+( yyvsp_offset - 1); if( _temp3546 < _temp3544.base? 1: _temp3546 >= _temp3544.last_plus_one){
_throw( Null_Exception);}* _temp3546;}))).f2; struct _tuple5 ts_info= Cyc_Parse_collapse_type_specifiers(
tss, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3547= yyls; struct
Cyc_Yyltype* _temp3549= _temp3547.curr +( yylsp_offset - 1); if( _temp3549 <
_temp3547.base? 1: _temp3549 >= _temp3547.last_plus_one){ _throw( Null_Exception);}*
_temp3549;}).first_line,({ struct _tagged_ptr3 _temp3550= yyls; struct Cyc_Yyltype*
_temp3552= _temp3550.curr +( yylsp_offset - 1); if( _temp3552 < _temp3550.base?
1: _temp3552 >= _temp3550.last_plus_one){ _throw( Null_Exception);}* _temp3552;}).last_line));
if( ts_info.f2 != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp3553=(
char*)"parameter contains nested type declaration"; struct _tagged_string
_temp3554; _temp3554.curr= _temp3553; _temp3554.base= _temp3553; _temp3554.last_plus_one=
_temp3553 + 43; _temp3554;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp3555= yyls; struct Cyc_Yyltype* _temp3557= _temp3555.curr +( yylsp_offset -
1); if( _temp3557 < _temp3555.base? 1: _temp3557 >= _temp3555.last_plus_one){
_throw( Null_Exception);}* _temp3557;}).first_line,({ struct _tagged_ptr3
_temp3558= yyls; struct Cyc_Yyltype* _temp3560= _temp3558.curr +( yylsp_offset -
1); if( _temp3560 < _temp3558.base? 1: _temp3560 >= _temp3558.last_plus_one){
_throw( Null_Exception);}* _temp3560;}).last_line));}{ void* t= ts_info.f1;
struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp3561= yyvs; struct _xenum_struct** _temp3563= _temp3561.curr +(
yyvsp_offset - 1); if( _temp3563 < _temp3561.base? 1: _temp3563 >= _temp3561.last_plus_one){
_throw( Null_Exception);}* _temp3563;}))).f1; struct Cyc_List_List* tms=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3564= yyvs; struct _xenum_struct** _temp3566= _temp3564.curr
+ yyvsp_offset; if( _temp3566 < _temp3564.base? 1: _temp3566 >= _temp3564.last_plus_one){
_throw( Null_Exception);}* _temp3566;})))->tms; struct _tuple6 t_info= Cyc_Parse_apply_tms(
tq, t, tms); if( t_info.f3 != 0){ Cyc_Parse_err(( struct _tagged_string)({ char*
_temp3567=( char*)"parameter with bad type params"; struct _tagged_string
_temp3568; _temp3568.curr= _temp3567; _temp3568.base= _temp3567; _temp3568.last_plus_one=
_temp3567 + 31; _temp3568;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp3569= yyls; struct Cyc_Yyltype* _temp3571= _temp3569.curr + yylsp_offset;
if( _temp3571 < _temp3569.base? 1: _temp3571 >= _temp3569.last_plus_one){ _throw(
Null_Exception);}* _temp3571;}).first_line,({ struct _tagged_ptr3 _temp3572=
yyls; struct Cyc_Yyltype* _temp3574= _temp3572.curr + yylsp_offset; if(
_temp3574 < _temp3572.base? 1: _temp3574 >= _temp3572.last_plus_one){ _throw(
Null_Exception);}* _temp3574;}).last_line));}{ struct _tuple1* q=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3575= yyvs; struct _xenum_struct** _temp3577= _temp3575.curr
+ yyvsp_offset; if( _temp3577 < _temp3575.base? 1: _temp3577 >= _temp3575.last_plus_one){
_throw( Null_Exception);}* _temp3577;})))->id;{ void* _temp3578=(* q).f1; struct
Cyc_List_List* _temp3588; struct Cyc_List_List* _temp3590; _LL3580: if(
_temp3578 == Cyc_Absyn_Loc_n){ goto _LL3581;} else{ goto _LL3582;} _LL3582: if((
unsigned int) _temp3578 > 1u?(( struct _enum_struct*) _temp3578)->tag == Cyc_Absyn_Rel_n_tag:
0){ _LL3589: _temp3588=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp3578)->f1; if( _temp3588 == 0){ goto _LL3583;} else{ goto _LL3584;}} else{
goto _LL3584;} _LL3584: if(( unsigned int) _temp3578 > 1u?(( struct _enum_struct*)
_temp3578)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL3591: _temp3590=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp3578)->f1; if( _temp3590 == 0){ goto
_LL3585;} else{ goto _LL3586;}} else{ goto _LL3586;} _LL3586: goto _LL3587;
_LL3581: goto _LL3579; _LL3583: goto _LL3579; _LL3585: goto _LL3579; _LL3587:
Cyc_Parse_err(( struct _tagged_string)({ char* _temp3592=( char*)"parameter cannot be qualified with a module name";
struct _tagged_string _temp3593; _temp3593.curr= _temp3592; _temp3593.base=
_temp3592; _temp3593.last_plus_one= _temp3592 + 49; _temp3593;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3594= yyls; struct Cyc_Yyltype* _temp3596= _temp3594.curr
+( yylsp_offset - 1); if( _temp3596 < _temp3594.base? 1: _temp3596 >= _temp3594.last_plus_one){
_throw( Null_Exception);}* _temp3596;}).first_line,({ struct _tagged_ptr3
_temp3597= yyls; struct Cyc_Yyltype* _temp3599= _temp3597.curr +( yylsp_offset -
1); if( _temp3599 < _temp3597.base? 1: _temp3599 >= _temp3597.last_plus_one){
_throw( Null_Exception);}* _temp3599;}).last_line)); goto _LL3579; _LL3579:;}{
struct Cyc_Core_Opt* idopt=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt*
_temp3600=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3600->v=( void*)(* q).f2; _temp3600;}); yyval=({ struct Cyc_ParamDecl_tok_struct*
_temp3601=( struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));*
_temp3601=( struct Cyc_ParamDecl_tok_struct){.tag= Cyc_ParamDecl_tok_tag,.f1=({
struct _tuple2* _temp3602=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp3602->f1= idopt; _temp3602->f2= t_info.f1; _temp3602->f3= t_info.f2;
_temp3602;})};( struct _xenum_struct*) _temp3601;}); break;}}}} case 148:
_LL3543: { struct Cyc_List_List* tss=(* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp3604= yyvs; struct _xenum_struct** _temp3606= _temp3604.curr +
yyvsp_offset; if( _temp3606 < _temp3604.base? 1: _temp3606 >= _temp3604.last_plus_one){
_throw( Null_Exception);}* _temp3606;}))).f2; struct _tuple5 ts_info= Cyc_Parse_collapse_type_specifiers(
tss, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3607= yyls; struct
Cyc_Yyltype* _temp3609= _temp3607.curr + yylsp_offset; if( _temp3609 < _temp3607.base?
1: _temp3609 >= _temp3607.last_plus_one){ _throw( Null_Exception);}* _temp3609;}).first_line,({
struct _tagged_ptr3 _temp3610= yyls; struct Cyc_Yyltype* _temp3612= _temp3610.curr
+ yylsp_offset; if( _temp3612 < _temp3610.base? 1: _temp3612 >= _temp3610.last_plus_one){
_throw( Null_Exception);}* _temp3612;}).last_line)); if( ts_info.f2 != 0){ Cyc_Parse_warn((
struct _tagged_string)({ char* _temp3613=( char*)"nested type declaration, ignoring";
struct _tagged_string _temp3614; _temp3614.curr= _temp3613; _temp3614.base=
_temp3613; _temp3614.last_plus_one= _temp3613 + 34; _temp3614;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3615= yyls; struct Cyc_Yyltype* _temp3617= _temp3615.curr
+ yylsp_offset; if( _temp3617 < _temp3615.base? 1: _temp3617 >= _temp3615.last_plus_one){
_throw( Null_Exception);}* _temp3617;}).first_line,({ struct _tagged_ptr3
_temp3618= yyls; struct Cyc_Yyltype* _temp3620= _temp3618.curr + yylsp_offset;
if( _temp3620 < _temp3618.base? 1: _temp3620 >= _temp3618.last_plus_one){ _throw(
Null_Exception);}* _temp3620;}).last_line));}{ void* t= ts_info.f1; struct Cyc_Absyn_Tqual*
tq=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp3621= yyvs; struct
_xenum_struct** _temp3623= _temp3621.curr + yyvsp_offset; if( _temp3623 <
_temp3621.base? 1: _temp3623 >= _temp3621.last_plus_one){ _throw( Null_Exception);}*
_temp3623;}))).f1; yyval=({ struct Cyc_ParamDecl_tok_struct* _temp3624=( struct
Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));*
_temp3624=( struct Cyc_ParamDecl_tok_struct){.tag= Cyc_ParamDecl_tok_tag,.f1=({
struct _tuple2* _temp3625=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp3625->f1= 0; _temp3625->f2= tq; _temp3625->f3= t; _temp3625;})};( struct
_xenum_struct*) _temp3624;}); break;}} case 149: _LL3603: { struct Cyc_List_List*
tss=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp3627= yyvs; struct
_xenum_struct** _temp3629= _temp3627.curr +( yyvsp_offset - 1); if( _temp3629 <
_temp3627.base? 1: _temp3629 >= _temp3627.last_plus_one){ _throw( Null_Exception);}*
_temp3629;}))).f2; struct _tuple5 ts_info= Cyc_Parse_collapse_type_specifiers(
tss, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3630= yyls; struct
Cyc_Yyltype* _temp3632= _temp3630.curr +( yylsp_offset - 1); if( _temp3632 <
_temp3630.base? 1: _temp3632 >= _temp3630.last_plus_one){ _throw( Null_Exception);}*
_temp3632;}).first_line,({ struct _tagged_ptr3 _temp3633= yyls; struct Cyc_Yyltype*
_temp3635= _temp3633.curr +( yylsp_offset - 1); if( _temp3635 < _temp3633.base?
1: _temp3635 >= _temp3633.last_plus_one){ _throw( Null_Exception);}* _temp3635;}).last_line));
if( ts_info.f2 != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp3636=(
char*)"nested type declaration, ignoring"; struct _tagged_string _temp3637;
_temp3637.curr= _temp3636; _temp3637.base= _temp3636; _temp3637.last_plus_one=
_temp3636 + 34; _temp3637;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp3638= yyls; struct Cyc_Yyltype* _temp3640= _temp3638.curr +( yylsp_offset -
1); if( _temp3640 < _temp3638.base? 1: _temp3640 >= _temp3638.last_plus_one){
_throw( Null_Exception);}* _temp3640;}).first_line,({ struct _tagged_ptr3
_temp3641= yyls; struct Cyc_Yyltype* _temp3643= _temp3641.curr +( yylsp_offset -
1); if( _temp3643 < _temp3641.base? 1: _temp3643 >= _temp3641.last_plus_one){
_throw( Null_Exception);}* _temp3643;}).last_line));}{ void* t= ts_info.f1;
struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp3644= yyvs; struct _xenum_struct** _temp3646= _temp3644.curr +(
yyvsp_offset - 1); if( _temp3646 < _temp3644.base? 1: _temp3646 >= _temp3644.last_plus_one){
_throw( Null_Exception);}* _temp3646;}))).f1; struct Cyc_List_List* tms=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp3647= yyvs; struct _xenum_struct** _temp3649= _temp3647.curr
+ yyvsp_offset; if( _temp3649 < _temp3647.base? 1: _temp3649 >= _temp3647.last_plus_one){
_throw( Null_Exception);}* _temp3649;})))->tms; struct _tuple6 t_info= Cyc_Parse_apply_tms(
tq, t, tms); if( t_info.f3 != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char*
_temp3650=( char*)"bad type params, ignoring"; struct _tagged_string _temp3651;
_temp3651.curr= _temp3650; _temp3651.base= _temp3650; _temp3651.last_plus_one=
_temp3650 + 26; _temp3651;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp3652= yyls; struct Cyc_Yyltype* _temp3654= _temp3652.curr + yylsp_offset;
if( _temp3654 < _temp3652.base? 1: _temp3654 >= _temp3652.last_plus_one){ _throw(
Null_Exception);}* _temp3654;}).first_line,({ struct _tagged_ptr3 _temp3655=
yyls; struct Cyc_Yyltype* _temp3657= _temp3655.curr + yylsp_offset; if(
_temp3657 < _temp3655.base? 1: _temp3657 >= _temp3655.last_plus_one){ _throw(
Null_Exception);}* _temp3657;}).last_line));} yyval=({ struct Cyc_ParamDecl_tok_struct*
_temp3658=( struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));*
_temp3658=( struct Cyc_ParamDecl_tok_struct){.tag= Cyc_ParamDecl_tok_tag,.f1=({
struct _tuple2* _temp3659=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp3659->f1= 0; _temp3659->f2= t_info.f1; _temp3659->f3= t_info.f2; _temp3659;})};(
struct _xenum_struct*) _temp3658;}); break;}} case 150: _LL3626: yyval=({ struct
Cyc_IdList_tok_struct* _temp3661=( struct Cyc_IdList_tok_struct*) GC_malloc(
sizeof( struct Cyc_IdList_tok_struct));* _temp3661=( struct Cyc_IdList_tok_struct){.tag=
Cyc_IdList_tok_tag,.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( Cyc_yyget_IdList_tok(({ struct _tagged_ptr2 _temp3662= yyvs;
struct _xenum_struct** _temp3664= _temp3662.curr + yyvsp_offset; if( _temp3664 <
_temp3662.base? 1: _temp3664 >= _temp3662.last_plus_one){ _throw( Null_Exception);}*
_temp3664;})))};( struct _xenum_struct*) _temp3661;}); break; case 151: _LL3660:
yyval=({ struct Cyc_IdList_tok_struct* _temp3666=( struct Cyc_IdList_tok_struct*)
GC_malloc( sizeof( struct Cyc_IdList_tok_struct));* _temp3666=( struct Cyc_IdList_tok_struct){.tag=
Cyc_IdList_tok_tag,.f1=({ struct Cyc_List_List* _temp3667=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3667->hd=( void*)({ struct
_tagged_string* _temp3668=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp3668[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp3669= yyvs; struct _xenum_struct** _temp3671= _temp3669.curr + yyvsp_offset;
if( _temp3671 < _temp3669.base? 1: _temp3671 >= _temp3669.last_plus_one){ _throw(
Null_Exception);}* _temp3671;})); _temp3668;}); _temp3667->tl= 0; _temp3667;})};(
struct _xenum_struct*) _temp3666;}); break; case 152: _LL3665: yyval=({ struct
Cyc_IdList_tok_struct* _temp3673=( struct Cyc_IdList_tok_struct*) GC_malloc(
sizeof( struct Cyc_IdList_tok_struct));* _temp3673=( struct Cyc_IdList_tok_struct){.tag=
Cyc_IdList_tok_tag,.f1=({ struct Cyc_List_List* _temp3674=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3674->hd=( void*)({ struct
_tagged_string* _temp3678=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp3678[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp3679= yyvs; struct _xenum_struct** _temp3681= _temp3679.curr + yyvsp_offset;
if( _temp3681 < _temp3679.base? 1: _temp3681 >= _temp3679.last_plus_one){ _throw(
Null_Exception);}* _temp3681;})); _temp3678;}); _temp3674->tl= Cyc_yyget_IdList_tok(({
struct _tagged_ptr2 _temp3675= yyvs; struct _xenum_struct** _temp3677= _temp3675.curr
+( yyvsp_offset - 2); if( _temp3677 < _temp3675.base? 1: _temp3677 >= _temp3675.last_plus_one){
_throw( Null_Exception);}* _temp3677;})); _temp3674;})};( struct _xenum_struct*)
_temp3673;}); break; case 153: _LL3672: yyval=({ struct _tagged_ptr2 _temp3683=
yyvs; struct _xenum_struct** _temp3685= _temp3683.curr + yyvsp_offset; if(
_temp3685 < _temp3683.base? 1: _temp3685 >= _temp3683.last_plus_one){ _throw(
Null_Exception);}* _temp3685;}); break; case 154: _LL3682: yyval=({ struct Cyc_Exp_tok_struct*
_temp3687=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp3687=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp3688=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp3688->tag=
Cyc_Absyn_UnresolvedMem_e_tag; _temp3688->f1= 0; _temp3688->f2= 0;( void*)
_temp3688;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3689= yyls;
struct Cyc_Yyltype* _temp3691= _temp3689.curr +( yylsp_offset - 1); if(
_temp3691 < _temp3689.base? 1: _temp3691 >= _temp3689.last_plus_one){ _throw(
Null_Exception);}* _temp3691;}).first_line,({ struct _tagged_ptr3 _temp3692=
yyls; struct Cyc_Yyltype* _temp3694= _temp3692.curr + yylsp_offset; if(
_temp3694 < _temp3692.base? 1: _temp3694 >= _temp3692.last_plus_one){ _throw(
Null_Exception);}* _temp3694;}).last_line))};( struct _xenum_struct*) _temp3687;});
break; case 155: _LL3686: yyval=({ struct Cyc_Exp_tok_struct* _temp3696=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp3696=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp3697=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp3697->tag=
Cyc_Absyn_UnresolvedMem_e_tag; _temp3697->f1= 0; _temp3697->f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp3698= yyvs; struct _xenum_struct** _temp3700= _temp3698.curr
+( yyvsp_offset - 1); if( _temp3700 < _temp3698.base? 1: _temp3700 >= _temp3698.last_plus_one){
_throw( Null_Exception);}* _temp3700;})));( void*) _temp3697;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3701= yyls; struct Cyc_Yyltype* _temp3703= _temp3701.curr
+( yylsp_offset - 2); if( _temp3703 < _temp3701.base? 1: _temp3703 >= _temp3701.last_plus_one){
_throw( Null_Exception);}* _temp3703;}).first_line,({ struct _tagged_ptr3
_temp3704= yyls; struct Cyc_Yyltype* _temp3706= _temp3704.curr + yylsp_offset;
if( _temp3706 < _temp3704.base? 1: _temp3706 >= _temp3704.last_plus_one){ _throw(
Null_Exception);}* _temp3706;}).last_line))};( struct _xenum_struct*) _temp3696;});
break; case 156: _LL3695: yyval=({ struct Cyc_Exp_tok_struct* _temp3708=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp3708=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp3709=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp3709->tag=
Cyc_Absyn_UnresolvedMem_e_tag; _temp3709->f1= 0; _temp3709->f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp3710= yyvs; struct _xenum_struct** _temp3712= _temp3710.curr
+( yyvsp_offset - 2); if( _temp3712 < _temp3710.base? 1: _temp3712 >= _temp3710.last_plus_one){
_throw( Null_Exception);}* _temp3712;})));( void*) _temp3709;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3713= yyls; struct Cyc_Yyltype* _temp3715= _temp3713.curr
+( yylsp_offset - 3); if( _temp3715 < _temp3713.base? 1: _temp3715 >= _temp3713.last_plus_one){
_throw( Null_Exception);}* _temp3715;}).first_line,({ struct _tagged_ptr3
_temp3716= yyls; struct Cyc_Yyltype* _temp3718= _temp3716.curr + yylsp_offset;
if( _temp3718 < _temp3716.base? 1: _temp3718 >= _temp3716.last_plus_one){ _throw(
Null_Exception);}* _temp3718;}).last_line))};( struct _xenum_struct*) _temp3708;});
break; case 157: _LL3707: yyval=({ struct Cyc_InitializerList_tok_struct*
_temp3720=( struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct
Cyc_InitializerList_tok_struct));* _temp3720=( struct Cyc_InitializerList_tok_struct){.tag=
Cyc_InitializerList_tok_tag,.f1=({ struct Cyc_List_List* _temp3721=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3721->hd=( void*)({ struct
_tuple16* _temp3722=( struct _tuple16*) GC_malloc( sizeof( struct _tuple16));
_temp3722->f1= 0; _temp3722->f2= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp3723= yyvs; struct _xenum_struct** _temp3725= _temp3723.curr + yyvsp_offset;
if( _temp3725 < _temp3723.base? 1: _temp3725 >= _temp3723.last_plus_one){ _throw(
Null_Exception);}* _temp3725;})); _temp3722;}); _temp3721->tl= 0; _temp3721;})};(
struct _xenum_struct*) _temp3720;}); break; case 158: _LL3719: yyval=({ struct
Cyc_InitializerList_tok_struct* _temp3727=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct));* _temp3727=( struct
Cyc_InitializerList_tok_struct){.tag= Cyc_InitializerList_tok_tag,.f1=({ struct
Cyc_List_List* _temp3728=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3728->hd=( void*)({ struct _tuple16* _temp3729=( struct _tuple16*)
GC_malloc( sizeof( struct _tuple16)); _temp3729->f1= Cyc_yyget_DesignatorList_tok(({
struct _tagged_ptr2 _temp3733= yyvs; struct _xenum_struct** _temp3735= _temp3733.curr
+( yyvsp_offset - 1); if( _temp3735 < _temp3733.base? 1: _temp3735 >= _temp3733.last_plus_one){
_throw( Null_Exception);}* _temp3735;})); _temp3729->f2= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp3730= yyvs; struct _xenum_struct** _temp3732= _temp3730.curr
+ yyvsp_offset; if( _temp3732 < _temp3730.base? 1: _temp3732 >= _temp3730.last_plus_one){
_throw( Null_Exception);}* _temp3732;})); _temp3729;}); _temp3728->tl= 0;
_temp3728;})};( struct _xenum_struct*) _temp3727;}); break; case 159: _LL3726:
yyval=({ struct Cyc_InitializerList_tok_struct* _temp3737=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct));* _temp3737=( struct
Cyc_InitializerList_tok_struct){.tag= Cyc_InitializerList_tok_tag,.f1=({ struct
Cyc_List_List* _temp3738=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3738->hd=( void*)({ struct _tuple16* _temp3742=( struct _tuple16*)
GC_malloc( sizeof( struct _tuple16)); _temp3742->f1= 0; _temp3742->f2= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp3743= yyvs; struct _xenum_struct** _temp3745= _temp3743.curr
+ yyvsp_offset; if( _temp3745 < _temp3743.base? 1: _temp3745 >= _temp3743.last_plus_one){
_throw( Null_Exception);}* _temp3745;})); _temp3742;}); _temp3738->tl= Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp3739= yyvs; struct _xenum_struct** _temp3741= _temp3739.curr
+( yyvsp_offset - 2); if( _temp3741 < _temp3739.base? 1: _temp3741 >= _temp3739.last_plus_one){
_throw( Null_Exception);}* _temp3741;})); _temp3738;})};( struct _xenum_struct*)
_temp3737;}); break; case 160: _LL3736: yyval=({ struct Cyc_InitializerList_tok_struct*
_temp3747=( struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct
Cyc_InitializerList_tok_struct));* _temp3747=( struct Cyc_InitializerList_tok_struct){.tag=
Cyc_InitializerList_tok_tag,.f1=({ struct Cyc_List_List* _temp3748=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3748->hd=( void*)({ struct
_tuple16* _temp3752=( struct _tuple16*) GC_malloc( sizeof( struct _tuple16));
_temp3752->f1= Cyc_yyget_DesignatorList_tok(({ struct _tagged_ptr2 _temp3756=
yyvs; struct _xenum_struct** _temp3758= _temp3756.curr +( yyvsp_offset - 1); if(
_temp3758 < _temp3756.base? 1: _temp3758 >= _temp3756.last_plus_one){ _throw(
Null_Exception);}* _temp3758;})); _temp3752->f2= Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp3753= yyvs; struct _xenum_struct** _temp3755= _temp3753.curr +
yyvsp_offset; if( _temp3755 < _temp3753.base? 1: _temp3755 >= _temp3753.last_plus_one){
_throw( Null_Exception);}* _temp3755;})); _temp3752;}); _temp3748->tl= Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp3749= yyvs; struct _xenum_struct** _temp3751= _temp3749.curr
+( yyvsp_offset - 3); if( _temp3751 < _temp3749.base? 1: _temp3751 >= _temp3749.last_plus_one){
_throw( Null_Exception);}* _temp3751;})); _temp3748;})};( struct _xenum_struct*)
_temp3747;}); break; case 161: _LL3746: yyval=({ struct Cyc_DesignatorList_tok_struct*
_temp3760=( struct Cyc_DesignatorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));*
_temp3760=( struct Cyc_DesignatorList_tok_struct){.tag= Cyc_DesignatorList_tok_tag,.f1=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_DesignatorList_tok(({
struct _tagged_ptr2 _temp3761= yyvs; struct _xenum_struct** _temp3763= _temp3761.curr
+( yyvsp_offset - 1); if( _temp3763 < _temp3761.base? 1: _temp3763 >= _temp3761.last_plus_one){
_throw( Null_Exception);}* _temp3763;})))};( struct _xenum_struct*) _temp3760;});
break; case 162: _LL3759: yyval=({ struct Cyc_DesignatorList_tok_struct*
_temp3765=( struct Cyc_DesignatorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));*
_temp3765=( struct Cyc_DesignatorList_tok_struct){.tag= Cyc_DesignatorList_tok_tag,.f1=({
struct Cyc_List_List* _temp3766=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3766->hd=( void*) Cyc_yyget_Designator_tok(({
struct _tagged_ptr2 _temp3767= yyvs; struct _xenum_struct** _temp3769= _temp3767.curr
+ yyvsp_offset; if( _temp3769 < _temp3767.base? 1: _temp3769 >= _temp3767.last_plus_one){
_throw( Null_Exception);}* _temp3769;})); _temp3766->tl= 0; _temp3766;})};(
struct _xenum_struct*) _temp3765;}); break; case 163: _LL3764: yyval=({ struct
Cyc_DesignatorList_tok_struct* _temp3771=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));* _temp3771=( struct
Cyc_DesignatorList_tok_struct){.tag= Cyc_DesignatorList_tok_tag,.f1=({ struct
Cyc_List_List* _temp3772=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3772->hd=( void*) Cyc_yyget_Designator_tok(({ struct _tagged_ptr2 _temp3776=
yyvs; struct _xenum_struct** _temp3778= _temp3776.curr + yyvsp_offset; if(
_temp3778 < _temp3776.base? 1: _temp3778 >= _temp3776.last_plus_one){ _throw(
Null_Exception);}* _temp3778;})); _temp3772->tl= Cyc_yyget_DesignatorList_tok(({
struct _tagged_ptr2 _temp3773= yyvs; struct _xenum_struct** _temp3775= _temp3773.curr
+( yyvsp_offset - 1); if( _temp3775 < _temp3773.base? 1: _temp3775 >= _temp3773.last_plus_one){
_throw( Null_Exception);}* _temp3775;})); _temp3772;})};( struct _xenum_struct*)
_temp3771;}); break; case 164: _LL3770: yyval=({ struct Cyc_Designator_tok_struct*
_temp3780=( struct Cyc_Designator_tok_struct*) GC_malloc( sizeof( struct Cyc_Designator_tok_struct));*
_temp3780=( struct Cyc_Designator_tok_struct){.tag= Cyc_Designator_tok_tag,.f1=({
struct Cyc_Absyn_ArrayElement_struct* _temp3781=( struct Cyc_Absyn_ArrayElement_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayElement_struct)); _temp3781->tag= Cyc_Absyn_ArrayElement_tag;
_temp3781->f1= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp3782= yyvs; struct
_xenum_struct** _temp3784= _temp3782.curr +( yyvsp_offset - 1); if( _temp3784 <
_temp3782.base? 1: _temp3784 >= _temp3782.last_plus_one){ _throw( Null_Exception);}*
_temp3784;}));( void*) _temp3781;})};( struct _xenum_struct*) _temp3780;});
break; case 165: _LL3779: yyval=({ struct Cyc_Designator_tok_struct* _temp3786=(
struct Cyc_Designator_tok_struct*) GC_malloc( sizeof( struct Cyc_Designator_tok_struct));*
_temp3786=( struct Cyc_Designator_tok_struct){.tag= Cyc_Designator_tok_tag,.f1=({
struct Cyc_Absyn_FieldName_struct* _temp3787=( struct Cyc_Absyn_FieldName_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct)); _temp3787->tag= Cyc_Absyn_FieldName_tag;
_temp3787->f1=({ struct _tagged_string* _temp3788=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp3788[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp3789= yyvs; struct _xenum_struct** _temp3791= _temp3789.curr
+ yyvsp_offset; if( _temp3791 < _temp3789.base? 1: _temp3791 >= _temp3789.last_plus_one){
_throw( Null_Exception);}* _temp3791;})); _temp3788;});( void*) _temp3787;})};(
struct _xenum_struct*) _temp3786;}); break; case 166: _LL3785: { struct Cyc_List_List*
tss=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp3793= yyvs; struct
_xenum_struct** _temp3795= _temp3793.curr + yyvsp_offset; if( _temp3795 <
_temp3793.base? 1: _temp3795 >= _temp3793.last_plus_one){ _throw( Null_Exception);}*
_temp3795;}))).f2; struct _tuple5 ts_info= Cyc_Parse_collapse_type_specifiers(
tss, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3796= yyls; struct
Cyc_Yyltype* _temp3798= _temp3796.curr + yylsp_offset; if( _temp3798 < _temp3796.base?
1: _temp3798 >= _temp3796.last_plus_one){ _throw( Null_Exception);}* _temp3798;}).first_line,({
struct _tagged_ptr3 _temp3799= yyls; struct Cyc_Yyltype* _temp3801= _temp3799.curr
+ yylsp_offset; if( _temp3801 < _temp3799.base? 1: _temp3801 >= _temp3799.last_plus_one){
_throw( Null_Exception);}* _temp3801;}).last_line)); if( ts_info.f2 != 0){ Cyc_Parse_warn((
struct _tagged_string)({ char* _temp3802=( char*)"nested type declaration, ignoring";
struct _tagged_string _temp3803; _temp3803.curr= _temp3802; _temp3803.base=
_temp3802; _temp3803.last_plus_one= _temp3802 + 34; _temp3803;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3804= yyls; struct Cyc_Yyltype* _temp3806= _temp3804.curr
+ yylsp_offset; if( _temp3806 < _temp3804.base? 1: _temp3806 >= _temp3804.last_plus_one){
_throw( Null_Exception);}* _temp3806;}).first_line,({ struct _tagged_ptr3
_temp3807= yyls; struct Cyc_Yyltype* _temp3809= _temp3807.curr + yylsp_offset;
if( _temp3809 < _temp3807.base? 1: _temp3809 >= _temp3807.last_plus_one){ _throw(
Null_Exception);}* _temp3809;}).last_line));}{ void* t= ts_info.f1; struct Cyc_Absyn_Tqual*
tq=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp3810= yyvs; struct
_xenum_struct** _temp3812= _temp3810.curr + yyvsp_offset; if( _temp3812 <
_temp3810.base? 1: _temp3812 >= _temp3810.last_plus_one){ _throw( Null_Exception);}*
_temp3812;}))).f1; yyval=({ struct Cyc_ParamDecl_tok_struct* _temp3813=( struct
Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));*
_temp3813=( struct Cyc_ParamDecl_tok_struct){.tag= Cyc_ParamDecl_tok_tag,.f1=({
struct _tuple2* _temp3814=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp3814->f1= 0; _temp3814->f2= tq; _temp3814->f3= t; _temp3814;})};( struct
_xenum_struct*) _temp3813;}); break;}} case 167: _LL3792: { struct Cyc_List_List*
tss=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp3816= yyvs; struct
_xenum_struct** _temp3818= _temp3816.curr +( yyvsp_offset - 1); if( _temp3818 <
_temp3816.base? 1: _temp3818 >= _temp3816.last_plus_one){ _throw( Null_Exception);}*
_temp3818;}))).f2; struct _tuple5 ts_info= Cyc_Parse_collapse_type_specifiers(
tss, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3819= yyls; struct
Cyc_Yyltype* _temp3821= _temp3819.curr +( yylsp_offset - 1); if( _temp3821 <
_temp3819.base? 1: _temp3821 >= _temp3819.last_plus_one){ _throw( Null_Exception);}*
_temp3821;}).first_line,({ struct _tagged_ptr3 _temp3822= yyls; struct Cyc_Yyltype*
_temp3824= _temp3822.curr +( yylsp_offset - 1); if( _temp3824 < _temp3822.base?
1: _temp3824 >= _temp3822.last_plus_one){ _throw( Null_Exception);}* _temp3824;}).last_line));
if( ts_info.f2 != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp3825=(
char*)"nested type declaration, ignoring"; struct _tagged_string _temp3826;
_temp3826.curr= _temp3825; _temp3826.base= _temp3825; _temp3826.last_plus_one=
_temp3825 + 34; _temp3826;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp3827= yyls; struct Cyc_Yyltype* _temp3829= _temp3827.curr +( yylsp_offset -
1); if( _temp3829 < _temp3827.base? 1: _temp3829 >= _temp3827.last_plus_one){
_throw( Null_Exception);}* _temp3829;}).first_line,({ struct _tagged_ptr3
_temp3830= yyls; struct Cyc_Yyltype* _temp3832= _temp3830.curr +( yylsp_offset -
1); if( _temp3832 < _temp3830.base? 1: _temp3832 >= _temp3830.last_plus_one){
_throw( Null_Exception);}* _temp3832;}).last_line));}{ void* t= ts_info.f1;
struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp3833= yyvs; struct _xenum_struct** _temp3835= _temp3833.curr +(
yyvsp_offset - 1); if( _temp3835 < _temp3833.base? 1: _temp3835 >= _temp3833.last_plus_one){
_throw( Null_Exception);}* _temp3835;}))).f1; struct Cyc_List_List* tms=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp3836= yyvs; struct _xenum_struct** _temp3838= _temp3836.curr
+ yyvsp_offset; if( _temp3838 < _temp3836.base? 1: _temp3838 >= _temp3836.last_plus_one){
_throw( Null_Exception);}* _temp3838;})))->tms; struct _tuple6 t_info= Cyc_Parse_apply_tms(
tq, t, tms); if( t_info.f3 != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char*
_temp3839=( char*)"bad type params, ignoring"; struct _tagged_string _temp3840;
_temp3840.curr= _temp3839; _temp3840.base= _temp3839; _temp3840.last_plus_one=
_temp3839 + 26; _temp3840;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp3841= yyls; struct Cyc_Yyltype* _temp3843= _temp3841.curr + yylsp_offset;
if( _temp3843 < _temp3841.base? 1: _temp3843 >= _temp3841.last_plus_one){ _throw(
Null_Exception);}* _temp3843;}).first_line,({ struct _tagged_ptr3 _temp3844=
yyls; struct Cyc_Yyltype* _temp3846= _temp3844.curr + yylsp_offset; if(
_temp3846 < _temp3844.base? 1: _temp3846 >= _temp3844.last_plus_one){ _throw(
Null_Exception);}* _temp3846;}).last_line));} yyval=({ struct Cyc_ParamDecl_tok_struct*
_temp3847=( struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));*
_temp3847=( struct Cyc_ParamDecl_tok_struct){.tag= Cyc_ParamDecl_tok_tag,.f1=({
struct _tuple2* _temp3848=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp3848->f1= 0; _temp3848->f2= t_info.f1; _temp3848->f3= t_info.f2; _temp3848;})};(
struct _xenum_struct*) _temp3847;}); break;}} case 168: _LL3815: yyval=({ struct
Cyc_Type_tok_struct* _temp3850=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof(
struct Cyc_Type_tok_struct));* _temp3850=( struct Cyc_Type_tok_struct){.tag= Cyc_Type_tok_tag,.f1=(*
Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp3851= yyvs; struct
_xenum_struct** _temp3853= _temp3851.curr + yyvsp_offset; if( _temp3853 <
_temp3851.base? 1: _temp3853 >= _temp3851.last_plus_one){ _throw( Null_Exception);}*
_temp3853;}))).f3};( struct _xenum_struct*) _temp3850;}); break; case 169:
_LL3849: yyval=({ struct Cyc_Type_tok_struct* _temp3855=( struct Cyc_Type_tok_struct*)
GC_malloc( sizeof( struct Cyc_Type_tok_struct));* _temp3855=( struct Cyc_Type_tok_struct){.tag=
Cyc_Type_tok_tag,.f1=({ struct Cyc_Absyn_JoinEff_struct* _temp3856=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp3856->tag= Cyc_Absyn_JoinEff_tag;
_temp3856->f1= 0;( void*) _temp3856;})};( struct _xenum_struct*) _temp3855;});
break; case 170: _LL3854: yyval=({ struct Cyc_Type_tok_struct* _temp3858=(
struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));*
_temp3858=( struct Cyc_Type_tok_struct){.tag= Cyc_Type_tok_tag,.f1=({ struct Cyc_Absyn_JoinEff_struct*
_temp3859=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3859->tag= Cyc_Absyn_JoinEff_tag; _temp3859->f1= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3860= yyvs; struct _xenum_struct** _temp3862= _temp3860.curr
+( yyvsp_offset - 1); if( _temp3862 < _temp3860.base? 1: _temp3862 >= _temp3860.last_plus_one){
_throw( Null_Exception);}* _temp3862;}));( void*) _temp3859;})};( struct
_xenum_struct*) _temp3858;}); break; case 171: _LL3857: yyval=({ struct Cyc_Type_tok_struct*
_temp3864=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));*
_temp3864=( struct Cyc_Type_tok_struct){.tag= Cyc_Type_tok_tag,.f1=({ struct Cyc_Absyn_JoinEff_struct*
_temp3865=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3865->tag= Cyc_Absyn_JoinEff_tag; _temp3865->f1=({ struct Cyc_List_List*
_temp3866=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3866->hd=( void*) Cyc_yyget_Type_tok(({ struct _tagged_ptr2 _temp3870= yyvs;
struct _xenum_struct** _temp3872= _temp3870.curr +( yyvsp_offset - 2); if(
_temp3872 < _temp3870.base? 1: _temp3872 >= _temp3870.last_plus_one){ _throw(
Null_Exception);}* _temp3872;})); _temp3866->tl= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3867= yyvs; struct _xenum_struct** _temp3869= _temp3867.curr
+ yyvsp_offset; if( _temp3869 < _temp3867.base? 1: _temp3869 >= _temp3867.last_plus_one){
_throw( Null_Exception);}* _temp3869;})); _temp3866;});( void*) _temp3865;})};(
struct _xenum_struct*) _temp3864;}); break; case 172: _LL3863: yyval=({ struct
Cyc_TypeList_tok_struct* _temp3874=( struct Cyc_TypeList_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeList_tok_struct));* _temp3874=( struct Cyc_TypeList_tok_struct){.tag=
Cyc_TypeList_tok_tag,.f1=({ struct Cyc_List_List* _temp3875=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3875->hd=( void*) Cyc_yyget_Type_tok(({
struct _tagged_ptr2 _temp3876= yyvs; struct _xenum_struct** _temp3878= _temp3876.curr
+ yyvsp_offset; if( _temp3878 < _temp3876.base? 1: _temp3878 >= _temp3876.last_plus_one){
_throw( Null_Exception);}* _temp3878;})); _temp3875->tl= 0; _temp3875;})};(
struct _xenum_struct*) _temp3874;}); break; case 173: _LL3873: yyval=({ struct
Cyc_TypeList_tok_struct* _temp3880=( struct Cyc_TypeList_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeList_tok_struct));* _temp3880=( struct Cyc_TypeList_tok_struct){.tag=
Cyc_TypeList_tok_tag,.f1=({ struct Cyc_List_List* _temp3881=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3881->hd=( void*) Cyc_yyget_Type_tok(({
struct _tagged_ptr2 _temp3885= yyvs; struct _xenum_struct** _temp3887= _temp3885.curr
+ yyvsp_offset; if( _temp3887 < _temp3885.base? 1: _temp3887 >= _temp3885.last_plus_one){
_throw( Null_Exception);}* _temp3887;})); _temp3881->tl= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3882= yyvs; struct _xenum_struct** _temp3884= _temp3882.curr
+( yyvsp_offset - 2); if( _temp3884 < _temp3882.base? 1: _temp3884 >= _temp3882.last_plus_one){
_throw( Null_Exception);}* _temp3884;})); _temp3881;})};( struct _xenum_struct*)
_temp3880;}); break; case 174: _LL3879: yyval=({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3889=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct));* _temp3889=( struct Cyc_AbstractDeclarator_tok_struct){.tag=
Cyc_AbstractDeclarator_tok_tag,.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3890=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3890->tms= Cyc_yyget_TypeModifierList_tok(({ struct _tagged_ptr2 _temp3891=
yyvs; struct _xenum_struct** _temp3893= _temp3891.curr + yyvsp_offset; if(
_temp3893 < _temp3891.base? 1: _temp3893 >= _temp3891.last_plus_one){ _throw(
Null_Exception);}* _temp3893;})); _temp3890;})};( struct _xenum_struct*)
_temp3889;}); break; case 175: _LL3888: yyval=({ struct _tagged_ptr2 _temp3895=
yyvs; struct _xenum_struct** _temp3897= _temp3895.curr + yyvsp_offset; if(
_temp3897 < _temp3895.base? 1: _temp3897 >= _temp3895.last_plus_one){ _throw(
Null_Exception);}* _temp3897;}); break; case 176: _LL3894: yyval=({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3899=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct));* _temp3899=( struct Cyc_AbstractDeclarator_tok_struct){.tag=
Cyc_AbstractDeclarator_tok_tag,.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3900=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3900->tms=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_TypeModifierList_tok(({ struct _tagged_ptr2
_temp3901= yyvs; struct _xenum_struct** _temp3903= _temp3901.curr +(
yyvsp_offset - 1); if( _temp3903 < _temp3901.base? 1: _temp3903 >= _temp3901.last_plus_one){
_throw( Null_Exception);}* _temp3903;})),( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp3904= yyvs; struct _xenum_struct** _temp3906= _temp3904.curr
+ yyvsp_offset; if( _temp3906 < _temp3904.base? 1: _temp3906 >= _temp3904.last_plus_one){
_throw( Null_Exception);}* _temp3906;})))->tms); _temp3900;})};( struct
_xenum_struct*) _temp3899;}); break; case 177: _LL3898: yyval=({ struct
_tagged_ptr2 _temp3908= yyvs; struct _xenum_struct** _temp3910= _temp3908.curr +(
yyvsp_offset - 1); if( _temp3910 < _temp3908.base? 1: _temp3910 >= _temp3908.last_plus_one){
_throw( Null_Exception);}* _temp3910;}); break; case 178: _LL3907: yyval=({
struct Cyc_AbstractDeclarator_tok_struct* _temp3912=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));* _temp3912=(
struct Cyc_AbstractDeclarator_tok_struct){.tag= Cyc_AbstractDeclarator_tok_tag,.f1=({
struct Cyc_Parse_Abstractdeclarator* _temp3913=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp3913->tms=({
struct Cyc_List_List* _temp3914=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3914->hd=( void*) Cyc_Absyn_Carray_mod; _temp3914->tl=
0; _temp3914;}); _temp3913;})};( struct _xenum_struct*) _temp3912;}); break;
case 179: _LL3911: yyval=({ struct Cyc_AbstractDeclarator_tok_struct* _temp3916=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));*
_temp3916=( struct Cyc_AbstractDeclarator_tok_struct){.tag= Cyc_AbstractDeclarator_tok_tag,.f1=({
struct Cyc_Parse_Abstractdeclarator* _temp3917=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp3917->tms=({
struct Cyc_List_List* _temp3918=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3918->hd=( void*) Cyc_Absyn_Carray_mod; _temp3918->tl=(
Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2 _temp3919= yyvs; struct
_xenum_struct** _temp3921= _temp3919.curr +( yyvsp_offset - 2); if( _temp3921 <
_temp3919.base? 1: _temp3921 >= _temp3919.last_plus_one){ _throw( Null_Exception);}*
_temp3921;})))->tms; _temp3918;}); _temp3917;})};( struct _xenum_struct*)
_temp3916;}); break; case 180: _LL3915: yyval=({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3923=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct));* _temp3923=( struct Cyc_AbstractDeclarator_tok_struct){.tag=
Cyc_AbstractDeclarator_tok_tag,.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3924=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3924->tms=({ struct Cyc_List_List* _temp3925=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3925->hd=( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp3926=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp3926->tag= Cyc_Absyn_ConstArray_mod_tag;
_temp3926->f1= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp3927= yyvs; struct
_xenum_struct** _temp3929= _temp3927.curr +( yyvsp_offset - 1); if( _temp3929 <
_temp3927.base? 1: _temp3929 >= _temp3927.last_plus_one){ _throw( Null_Exception);}*
_temp3929;}));( void*) _temp3926;}); _temp3925->tl= 0; _temp3925;}); _temp3924;})};(
struct _xenum_struct*) _temp3923;}); break; case 181: _LL3922: yyval=({ struct
Cyc_AbstractDeclarator_tok_struct* _temp3931=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));* _temp3931=(
struct Cyc_AbstractDeclarator_tok_struct){.tag= Cyc_AbstractDeclarator_tok_tag,.f1=({
struct Cyc_Parse_Abstractdeclarator* _temp3932=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp3932->tms=({
struct Cyc_List_List* _temp3933=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3933->hd=( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp3937=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp3937->tag= Cyc_Absyn_ConstArray_mod_tag;
_temp3937->f1= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp3938= yyvs; struct
_xenum_struct** _temp3940= _temp3938.curr +( yyvsp_offset - 1); if( _temp3940 <
_temp3938.base? 1: _temp3940 >= _temp3938.last_plus_one){ _throw( Null_Exception);}*
_temp3940;}));( void*) _temp3937;}); _temp3933->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp3934= yyvs; struct _xenum_struct** _temp3936= _temp3934.curr
+( yyvsp_offset - 3); if( _temp3936 < _temp3934.base? 1: _temp3936 >= _temp3934.last_plus_one){
_throw( Null_Exception);}* _temp3936;})))->tms; _temp3933;}); _temp3932;})};(
struct _xenum_struct*) _temp3931;}); break; case 182: _LL3930: yyval=({ struct
Cyc_AbstractDeclarator_tok_struct* _temp3942=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));* _temp3942=(
struct Cyc_AbstractDeclarator_tok_struct){.tag= Cyc_AbstractDeclarator_tok_tag,.f1=({
struct Cyc_Parse_Abstractdeclarator* _temp3943=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp3943->tms=({
struct Cyc_List_List* _temp3944=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3944->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp3945=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3945->tag= Cyc_Absyn_Function_mod_tag; _temp3945->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp3946=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3946->tag= Cyc_Absyn_WithTypes_tag; _temp3946->f1= 0; _temp3946->f2= 0;
_temp3946->f3= 0;( void*) _temp3946;});( void*) _temp3945;}); _temp3944->tl= 0;
_temp3944;}); _temp3943;})};( struct _xenum_struct*) _temp3942;}); break; case
183: _LL3941: yyval=({ struct Cyc_AbstractDeclarator_tok_struct* _temp3948=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));*
_temp3948=( struct Cyc_AbstractDeclarator_tok_struct){.tag= Cyc_AbstractDeclarator_tok_tag,.f1=({
struct Cyc_Parse_Abstractdeclarator* _temp3949=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp3949->tms=({
struct Cyc_List_List* _temp3950=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3950->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp3951=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3951->tag= Cyc_Absyn_Function_mod_tag; _temp3951->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp3952=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3952->tag= Cyc_Absyn_WithTypes_tag; _temp3952->f1= 0; _temp3952->f2= 0;
_temp3952->f3=({ struct Cyc_Core_Opt* _temp3953=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3953->v=( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp3954=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3954->tag= Cyc_Absyn_JoinEff_tag; _temp3954->f1= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3955= yyvs; struct _xenum_struct** _temp3957= _temp3955.curr
+( yyvsp_offset - 1); if( _temp3957 < _temp3955.base? 1: _temp3957 >= _temp3955.last_plus_one){
_throw( Null_Exception);}* _temp3957;}));( void*) _temp3954;}); _temp3953;});(
void*) _temp3952;});( void*) _temp3951;}); _temp3950->tl= 0; _temp3950;});
_temp3949;})};( struct _xenum_struct*) _temp3948;}); break; case 184: _LL3947: {
struct _tuple14 _temp3964; struct Cyc_Core_Opt* _temp3965; int _temp3967; struct
Cyc_List_List* _temp3969; struct _tuple14* _temp3962= Cyc_yyget_ParamDeclListBool_tok(({
struct _tagged_ptr2 _temp3959= yyvs; struct _xenum_struct** _temp3961= _temp3959.curr
+( yyvsp_offset - 1); if( _temp3961 < _temp3959.base? 1: _temp3961 >= _temp3959.last_plus_one){
_throw( Null_Exception);}* _temp3961;})); _temp3964=* _temp3962; _LL3970:
_temp3969= _temp3964.f1; goto _LL3968; _LL3968: _temp3967= _temp3964.f2; goto
_LL3966; _LL3966: _temp3965= _temp3964.f3; goto _LL3963; _LL3963: yyval=({
struct Cyc_AbstractDeclarator_tok_struct* _temp3971=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));* _temp3971=(
struct Cyc_AbstractDeclarator_tok_struct){.tag= Cyc_AbstractDeclarator_tok_tag,.f1=({
struct Cyc_Parse_Abstractdeclarator* _temp3972=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp3972->tms=({
struct Cyc_List_List* _temp3973=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3973->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp3974=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3974->tag= Cyc_Absyn_Function_mod_tag; _temp3974->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp3975=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3975->tag= Cyc_Absyn_WithTypes_tag; _temp3975->f1= _temp3969; _temp3975->f2=
_temp3967; _temp3975->f3= _temp3965;( void*) _temp3975;});( void*) _temp3974;});
_temp3973->tl= 0; _temp3973;}); _temp3972;})};( struct _xenum_struct*) _temp3971;});
break;} case 185: _LL3958: yyval=({ struct Cyc_AbstractDeclarator_tok_struct*
_temp3977=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct));* _temp3977=( struct Cyc_AbstractDeclarator_tok_struct){.tag=
Cyc_AbstractDeclarator_tok_tag,.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp3978=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp3978->tms=({ struct Cyc_List_List* _temp3979=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3979->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp3983=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3983->tag= Cyc_Absyn_Function_mod_tag; _temp3983->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp3984=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3984->tag= Cyc_Absyn_WithTypes_tag; _temp3984->f1= 0; _temp3984->f2= 0;
_temp3984->f3= 0;( void*) _temp3984;});( void*) _temp3983;}); _temp3979->tl=(
Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2 _temp3980= yyvs; struct
_xenum_struct** _temp3982= _temp3980.curr +( yyvsp_offset - 2); if( _temp3982 <
_temp3980.base? 1: _temp3982 >= _temp3980.last_plus_one){ _throw( Null_Exception);}*
_temp3982;})))->tms; _temp3979;}); _temp3978;})};( struct _xenum_struct*)
_temp3977;}); break; case 186: _LL3976: { struct Cyc_Core_Opt* eff=({ struct Cyc_Core_Opt*
_temp3986=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3986->v=( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp3987=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp3987->tag= Cyc_Absyn_JoinEff_tag;
_temp3987->f1= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3988= yyvs;
struct _xenum_struct** _temp3990= _temp3988.curr +( yyvsp_offset - 1); if(
_temp3990 < _temp3988.base? 1: _temp3990 >= _temp3988.last_plus_one){ _throw(
Null_Exception);}* _temp3990;}));( void*) _temp3987;}); _temp3986;}); yyval=({
struct Cyc_AbstractDeclarator_tok_struct* _temp3991=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));* _temp3991=(
struct Cyc_AbstractDeclarator_tok_struct){.tag= Cyc_AbstractDeclarator_tok_tag,.f1=({
struct Cyc_Parse_Abstractdeclarator* _temp3992=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp3992->tms=({
struct Cyc_List_List* _temp3993=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3993->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp3997=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3997->tag= Cyc_Absyn_Function_mod_tag; _temp3997->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp3998=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3998->tag= Cyc_Absyn_WithTypes_tag; _temp3998->f1= 0; _temp3998->f2= 0;
_temp3998->f3=( struct Cyc_Core_Opt*) eff;( void*) _temp3998;});( void*)
_temp3997;}); _temp3993->tl=( Cyc_yyget_AbstractDeclarator_tok(({ struct
_tagged_ptr2 _temp3994= yyvs; struct _xenum_struct** _temp3996= _temp3994.curr +(
yyvsp_offset - 4); if( _temp3996 < _temp3994.base? 1: _temp3996 >= _temp3994.last_plus_one){
_throw( Null_Exception);}* _temp3996;})))->tms; _temp3993;}); _temp3992;})};(
struct _xenum_struct*) _temp3991;}); break;} case 187: _LL3985: { struct
_tuple14 _temp4005; struct Cyc_Core_Opt* _temp4006; int _temp4008; struct Cyc_List_List*
_temp4010; struct _tuple14* _temp4003= Cyc_yyget_ParamDeclListBool_tok(({ struct
_tagged_ptr2 _temp4000= yyvs; struct _xenum_struct** _temp4002= _temp4000.curr +(
yyvsp_offset - 1); if( _temp4002 < _temp4000.base? 1: _temp4002 >= _temp4000.last_plus_one){
_throw( Null_Exception);}* _temp4002;})); _temp4005=* _temp4003; _LL4011:
_temp4010= _temp4005.f1; goto _LL4009; _LL4009: _temp4008= _temp4005.f2; goto
_LL4007; _LL4007: _temp4006= _temp4005.f3; goto _LL4004; _LL4004: yyval=({
struct Cyc_AbstractDeclarator_tok_struct* _temp4012=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));* _temp4012=(
struct Cyc_AbstractDeclarator_tok_struct){.tag= Cyc_AbstractDeclarator_tok_tag,.f1=({
struct Cyc_Parse_Abstractdeclarator* _temp4013=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4013->tms=({
struct Cyc_List_List* _temp4014=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4014->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4018=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4018->tag= Cyc_Absyn_Function_mod_tag; _temp4018->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4019=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4019->tag= Cyc_Absyn_WithTypes_tag; _temp4019->f1= _temp4010; _temp4019->f2=
_temp4008; _temp4019->f3= _temp4006;( void*) _temp4019;});( void*) _temp4018;});
_temp4014->tl=( Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2
_temp4015= yyvs; struct _xenum_struct** _temp4017= _temp4015.curr +(
yyvsp_offset - 3); if( _temp4017 < _temp4015.base? 1: _temp4017 >= _temp4015.last_plus_one){
_throw( Null_Exception);}* _temp4017;})))->tms; _temp4014;}); _temp4013;})};(
struct _xenum_struct*) _temp4012;}); break;} case 188: _LL3999: { struct Cyc_List_List*
ts=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4021=
yyls; struct Cyc_Yyltype* _temp4023= _temp4021.curr +( yylsp_offset - 2); if(
_temp4023 < _temp4021.base? 1: _temp4023 >= _temp4021.last_plus_one){ _throw(
Null_Exception);}* _temp4023;}).first_line,({ struct _tagged_ptr3 _temp4024=
yyls; struct Cyc_Yyltype* _temp4026= _temp4024.curr + yylsp_offset; if(
_temp4026 < _temp4024.base? 1: _temp4026 >= _temp4024.last_plus_one){ _throw(
Null_Exception);}* _temp4026;}).last_line),(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp4027= yyvs; struct _xenum_struct** _temp4029= _temp4027.curr +(
yyvsp_offset - 1); if( _temp4029 < _temp4027.base? 1: _temp4029 >= _temp4027.last_plus_one){
_throw( Null_Exception);}* _temp4029;})))); yyval=({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4030=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct));* _temp4030=( struct Cyc_AbstractDeclarator_tok_struct){.tag=
Cyc_AbstractDeclarator_tok_tag,.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4031=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4031->tms=({ struct Cyc_List_List* _temp4032=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4032->hd=( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp4036=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp4036->tag= Cyc_Absyn_TypeParams_mod_tag;
_temp4036->f1= ts; _temp4036->f2= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4037= yyls; struct Cyc_Yyltype* _temp4039= _temp4037.curr +(
yylsp_offset - 2); if( _temp4039 < _temp4037.base? 1: _temp4039 >= _temp4037.last_plus_one){
_throw( Null_Exception);}* _temp4039;}).first_line,({ struct _tagged_ptr3
_temp4040= yyls; struct Cyc_Yyltype* _temp4042= _temp4040.curr + yylsp_offset;
if( _temp4042 < _temp4040.base? 1: _temp4042 >= _temp4040.last_plus_one){ _throw(
Null_Exception);}* _temp4042;}).last_line); _temp4036->f3= 0;( void*) _temp4036;});
_temp4032->tl=( Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2
_temp4033= yyvs; struct _xenum_struct** _temp4035= _temp4033.curr +(
yyvsp_offset - 3); if( _temp4035 < _temp4033.base? 1: _temp4035 >= _temp4033.last_plus_one){
_throw( Null_Exception);}* _temp4035;})))->tms; _temp4032;}); _temp4031;})};(
struct _xenum_struct*) _temp4030;}); break;} case 189: _LL4020:(( struct Cyc_Lexing_lexbuf*)
Cyc_Parse_lbuf->v)->lex_curr_pos -= 1;{ struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4044= yyls; struct Cyc_Yyltype* _temp4046= _temp4044.curr
+( yylsp_offset - 2); if( _temp4046 < _temp4044.base? 1: _temp4046 >= _temp4044.last_plus_one){
_throw( Null_Exception);}* _temp4046;}).first_line,({ struct _tagged_ptr3
_temp4047= yyls; struct Cyc_Yyltype* _temp4049= _temp4047.curr + yylsp_offset;
if( _temp4049 < _temp4047.base? 1: _temp4049 >= _temp4047.last_plus_one){ _throw(
Null_Exception);}* _temp4049;}).last_line),(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp4050= yyvs; struct _xenum_struct** _temp4052= _temp4050.curr +(
yyvsp_offset - 1); if( _temp4052 < _temp4050.base? 1: _temp4052 >= _temp4050.last_plus_one){
_throw( Null_Exception);}* _temp4052;})))); yyval=({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4053=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct));* _temp4053=( struct Cyc_AbstractDeclarator_tok_struct){.tag=
Cyc_AbstractDeclarator_tok_tag,.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4054=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4054->tms=({ struct Cyc_List_List* _temp4055=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4055->hd=( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp4059=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp4059->tag= Cyc_Absyn_TypeParams_mod_tag;
_temp4059->f1= ts; _temp4059->f2= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4060= yyls; struct Cyc_Yyltype* _temp4062= _temp4060.curr +(
yylsp_offset - 2); if( _temp4062 < _temp4060.base? 1: _temp4062 >= _temp4060.last_plus_one){
_throw( Null_Exception);}* _temp4062;}).first_line,({ struct _tagged_ptr3
_temp4063= yyls; struct Cyc_Yyltype* _temp4065= _temp4063.curr + yylsp_offset;
if( _temp4065 < _temp4063.base? 1: _temp4065 >= _temp4063.last_plus_one){ _throw(
Null_Exception);}* _temp4065;}).last_line); _temp4059->f3= 0;( void*) _temp4059;});
_temp4055->tl=( Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2
_temp4056= yyvs; struct _xenum_struct** _temp4058= _temp4056.curr +(
yyvsp_offset - 3); if( _temp4058 < _temp4056.base? 1: _temp4058 >= _temp4056.last_plus_one){
_throw( Null_Exception);}* _temp4058;})))->tms; _temp4055;}); _temp4054;})};(
struct _xenum_struct*) _temp4053;}); break;} case 190: _LL4043: yyval=({ struct
_tagged_ptr2 _temp4067= yyvs; struct _xenum_struct** _temp4069= _temp4067.curr +
yyvsp_offset; if( _temp4069 < _temp4067.base? 1: _temp4069 >= _temp4067.last_plus_one){
_throw( Null_Exception);}* _temp4069;}); break; case 191: _LL4066: yyval=({
struct _tagged_ptr2 _temp4071= yyvs; struct _xenum_struct** _temp4073= _temp4071.curr
+ yyvsp_offset; if( _temp4073 < _temp4071.base? 1: _temp4073 >= _temp4071.last_plus_one){
_throw( Null_Exception);}* _temp4073;}); break; case 192: _LL4070: yyval=({
struct _tagged_ptr2 _temp4075= yyvs; struct _xenum_struct** _temp4077= _temp4075.curr
+ yyvsp_offset; if( _temp4077 < _temp4075.base? 1: _temp4077 >= _temp4075.last_plus_one){
_throw( Null_Exception);}* _temp4077;}); break; case 193: _LL4074: yyval=({
struct _tagged_ptr2 _temp4079= yyvs; struct _xenum_struct** _temp4081= _temp4079.curr
+ yyvsp_offset; if( _temp4081 < _temp4079.base? 1: _temp4081 >= _temp4079.last_plus_one){
_throw( Null_Exception);}* _temp4081;}); break; case 194: _LL4078: yyval=({
struct _tagged_ptr2 _temp4083= yyvs; struct _xenum_struct** _temp4085= _temp4083.curr
+ yyvsp_offset; if( _temp4085 < _temp4083.base? 1: _temp4085 >= _temp4083.last_plus_one){
_throw( Null_Exception);}* _temp4085;}); break; case 195: _LL4082: yyval=({
struct _tagged_ptr2 _temp4087= yyvs; struct _xenum_struct** _temp4089= _temp4087.curr
+ yyvsp_offset; if( _temp4089 < _temp4087.base? 1: _temp4089 >= _temp4087.last_plus_one){
_throw( Null_Exception);}* _temp4089;}); break; case 196: _LL4086: yyval=({
struct Cyc_Stmt_tok_struct* _temp4091=( struct Cyc_Stmt_tok_struct*) GC_malloc(
sizeof( struct Cyc_Stmt_tok_struct));* _temp4091=( struct Cyc_Stmt_tok_struct){.tag=
Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_new_stmt(({ struct Cyc_Absyn_Cut_s_struct*
_temp4092=( struct Cyc_Absyn_Cut_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Cut_s_struct));
_temp4092->tag= Cyc_Absyn_Cut_s_tag; _temp4092->f1= Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp4093= yyvs; struct _xenum_struct** _temp4095= _temp4093.curr +
yyvsp_offset; if( _temp4095 < _temp4093.base? 1: _temp4095 >= _temp4093.last_plus_one){
_throw( Null_Exception);}* _temp4095;}));( void*) _temp4092;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4096= yyls; struct Cyc_Yyltype* _temp4098= _temp4096.curr
+( yylsp_offset - 1); if( _temp4098 < _temp4096.base? 1: _temp4098 >= _temp4096.last_plus_one){
_throw( Null_Exception);}* _temp4098;}).first_line,({ struct _tagged_ptr3
_temp4099= yyls; struct Cyc_Yyltype* _temp4101= _temp4099.curr + yylsp_offset;
if( _temp4101 < _temp4099.base? 1: _temp4101 >= _temp4099.last_plus_one){ _throw(
Null_Exception);}* _temp4101;}).last_line))};( struct _xenum_struct*) _temp4091;});
break; case 197: _LL4090: yyval=({ struct Cyc_Stmt_tok_struct* _temp4103=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4103=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_new_stmt(({
struct Cyc_Absyn_Splice_s_struct* _temp4104=( struct Cyc_Absyn_Splice_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Splice_s_struct)); _temp4104->tag= Cyc_Absyn_Splice_s_tag;
_temp4104->f1= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4105= yyvs; struct
_xenum_struct** _temp4107= _temp4105.curr + yyvsp_offset; if( _temp4107 <
_temp4105.base? 1: _temp4107 >= _temp4105.last_plus_one){ _throw( Null_Exception);}*
_temp4107;}));( void*) _temp4104;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4108= yyls; struct Cyc_Yyltype* _temp4110= _temp4108.curr +(
yylsp_offset - 1); if( _temp4110 < _temp4108.base? 1: _temp4110 >= _temp4108.last_plus_one){
_throw( Null_Exception);}* _temp4110;}).first_line,({ struct _tagged_ptr3
_temp4111= yyls; struct Cyc_Yyltype* _temp4113= _temp4111.curr + yylsp_offset;
if( _temp4113 < _temp4111.base? 1: _temp4113 >= _temp4111.last_plus_one){ _throw(
Null_Exception);}* _temp4113;}).last_line))};( struct _xenum_struct*) _temp4103;});
break; case 198: _LL4102: yyval=({ struct Cyc_Stmt_tok_struct* _temp4115=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4115=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_new_stmt(({
struct Cyc_Absyn_Label_s_struct* _temp4116=( struct Cyc_Absyn_Label_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Label_s_struct)); _temp4116->tag= Cyc_Absyn_Label_s_tag;
_temp4116->f1=({ struct _tagged_string* _temp4117=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp4117[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp4118= yyvs; struct _xenum_struct** _temp4120= _temp4118.curr
+( yyvsp_offset - 2); if( _temp4120 < _temp4118.base? 1: _temp4120 >= _temp4118.last_plus_one){
_throw( Null_Exception);}* _temp4120;})); _temp4117;}); _temp4116->f2= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp4121= yyvs; struct _xenum_struct** _temp4123= _temp4121.curr
+ yyvsp_offset; if( _temp4123 < _temp4121.base? 1: _temp4123 >= _temp4121.last_plus_one){
_throw( Null_Exception);}* _temp4123;}));( void*) _temp4116;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4124= yyls; struct Cyc_Yyltype* _temp4126= _temp4124.curr
+( yylsp_offset - 2); if( _temp4126 < _temp4124.base? 1: _temp4126 >= _temp4124.last_plus_one){
_throw( Null_Exception);}* _temp4126;}).first_line,({ struct _tagged_ptr3
_temp4127= yyls; struct Cyc_Yyltype* _temp4129= _temp4127.curr + yylsp_offset;
if( _temp4129 < _temp4127.base? 1: _temp4129 >= _temp4127.last_plus_one){ _throw(
Null_Exception);}* _temp4129;}).last_line))};( struct _xenum_struct*) _temp4115;});
break; case 199: _LL4114: yyval=({ struct Cyc_Stmt_tok_struct* _temp4131=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4131=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4132= yyls; struct Cyc_Yyltype*
_temp4134= _temp4132.curr + yylsp_offset; if( _temp4134 < _temp4132.base? 1:
_temp4134 >= _temp4132.last_plus_one){ _throw( Null_Exception);}* _temp4134;}).first_line,({
struct _tagged_ptr3 _temp4135= yyls; struct Cyc_Yyltype* _temp4137= _temp4135.curr
+ yylsp_offset; if( _temp4137 < _temp4135.base? 1: _temp4137 >= _temp4135.last_plus_one){
_throw( Null_Exception);}* _temp4137;}).last_line))};( struct _xenum_struct*)
_temp4131;}); break; case 200: _LL4130: yyval=({ struct Cyc_Stmt_tok_struct*
_temp4139=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4139=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_exp_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4140= yyvs; struct _xenum_struct**
_temp4142= _temp4140.curr +( yyvsp_offset - 1); if( _temp4142 < _temp4140.base?
1: _temp4142 >= _temp4140.last_plus_one){ _throw( Null_Exception);}* _temp4142;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4143= yyls; struct Cyc_Yyltype*
_temp4145= _temp4143.curr +( yylsp_offset - 1); if( _temp4145 < _temp4143.base?
1: _temp4145 >= _temp4143.last_plus_one){ _throw( Null_Exception);}* _temp4145;}).first_line,({
struct _tagged_ptr3 _temp4146= yyls; struct Cyc_Yyltype* _temp4148= _temp4146.curr
+ yylsp_offset; if( _temp4148 < _temp4146.base? 1: _temp4148 >= _temp4146.last_plus_one){
_throw( Null_Exception);}* _temp4148;}).last_line))};( struct _xenum_struct*)
_temp4139;}); break; case 201: _LL4138: yyval=({ struct Cyc_Stmt_tok_struct*
_temp4150=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4150=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4151= yyls; struct Cyc_Yyltype*
_temp4153= _temp4151.curr +( yylsp_offset - 1); if( _temp4153 < _temp4151.base?
1: _temp4153 >= _temp4151.last_plus_one){ _throw( Null_Exception);}* _temp4153;}).first_line,({
struct _tagged_ptr3 _temp4154= yyls; struct Cyc_Yyltype* _temp4156= _temp4154.curr
+ yylsp_offset; if( _temp4156 < _temp4154.base? 1: _temp4156 >= _temp4154.last_plus_one){
_throw( Null_Exception);}* _temp4156;}).last_line))};( struct _xenum_struct*)
_temp4150;}); break; case 202: _LL4149: yyval=({ struct _tagged_ptr2 _temp4158=
yyvs; struct _xenum_struct** _temp4160= _temp4158.curr +( yyvsp_offset - 1); if(
_temp4160 < _temp4158.base? 1: _temp4160 >= _temp4158.last_plus_one){ _throw(
Null_Exception);}* _temp4160;}); break; case 203: _LL4157:{ void* _temp4165= Cyc_yyget_BlockItem_tok(({
struct _tagged_ptr2 _temp4162= yyvs; struct _xenum_struct** _temp4164= _temp4162.curr
+ yyvsp_offset; if( _temp4164 < _temp4162.base? 1: _temp4164 >= _temp4162.last_plus_one){
_throw( Null_Exception);}* _temp4164;})); struct Cyc_List_List* _temp4173;
struct Cyc_Absyn_Fndecl* _temp4175; struct Cyc_Absyn_Stmt* _temp4177; _LL4167:
if((( struct _enum_struct*) _temp4165)->tag == Cyc_Parse_TopDecls_bl_tag){
_LL4174: _temp4173=( struct Cyc_List_List*)(( struct Cyc_Parse_TopDecls_bl_struct*)
_temp4165)->f1; goto _LL4168;} else{ goto _LL4169;} _LL4169: if((( struct
_enum_struct*) _temp4165)->tag == Cyc_Parse_FnDecl_bl_tag){ _LL4176: _temp4175=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Parse_FnDecl_bl_struct*) _temp4165)->f1;
goto _LL4170;} else{ goto _LL4171;} _LL4171: if((( struct _enum_struct*)
_temp4165)->tag == Cyc_Parse_Stmt_bl_tag){ _LL4178: _temp4177=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Parse_Stmt_bl_struct*) _temp4165)->f1; goto _LL4172;} else{ goto
_LL4166;} _LL4168: yyval=({ struct Cyc_Stmt_tok_struct* _temp4179=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));* _temp4179=( struct Cyc_Stmt_tok_struct){.tag=
Cyc_Stmt_tok_tag,.f1= Cyc_Parse_flatten_declarations( _temp4173, Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4180= yyls; struct Cyc_Yyltype*
_temp4182= _temp4180.curr + yylsp_offset; if( _temp4182 < _temp4180.base? 1:
_temp4182 >= _temp4180.last_plus_one){ _throw( Null_Exception);}* _temp4182;}).first_line,({
struct _tagged_ptr3 _temp4183= yyls; struct Cyc_Yyltype* _temp4185= _temp4183.curr
+ yylsp_offset; if( _temp4185 < _temp4183.base? 1: _temp4185 >= _temp4183.last_plus_one){
_throw( Null_Exception);}* _temp4185;}).last_line)))};( struct _xenum_struct*)
_temp4179;}); goto _LL4166; _LL4170: yyval=({ struct Cyc_Stmt_tok_struct*
_temp4186=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4186=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(({ struct Cyc_Absyn_Fn_d_struct* _temp4187=( struct Cyc_Absyn_Fn_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct)); _temp4187->tag= Cyc_Absyn_Fn_d_tag;
_temp4187->f1= _temp4175;( void*) _temp4187;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4188= yyls; struct Cyc_Yyltype* _temp4190= _temp4188.curr
+ yylsp_offset; if( _temp4190 < _temp4188.base? 1: _temp4190 >= _temp4188.last_plus_one){
_throw( Null_Exception);}* _temp4190;}).first_line,({ struct _tagged_ptr3
_temp4191= yyls; struct Cyc_Yyltype* _temp4193= _temp4191.curr + yylsp_offset;
if( _temp4193 < _temp4191.base? 1: _temp4193 >= _temp4191.last_plus_one){ _throw(
Null_Exception);}* _temp4193;}).last_line)), Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4194= yyls; struct Cyc_Yyltype* _temp4196= _temp4194.curr
+ yylsp_offset; if( _temp4196 < _temp4194.base? 1: _temp4196 >= _temp4194.last_plus_one){
_throw( Null_Exception);}* _temp4196;}).first_line,({ struct _tagged_ptr3
_temp4197= yyls; struct Cyc_Yyltype* _temp4199= _temp4197.curr + yylsp_offset;
if( _temp4199 < _temp4197.base? 1: _temp4199 >= _temp4197.last_plus_one){ _throw(
Null_Exception);}* _temp4199;}).last_line)))};( struct _xenum_struct*) _temp4186;});
goto _LL4166; _LL4172: yyval=({ struct Cyc_Stmt_tok_struct* _temp4200=( struct
Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4200=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= _temp4177};(
struct _xenum_struct*) _temp4200;}); goto _LL4166; _LL4166:;} break; case 204:
_LL4161:{ void* _temp4205= Cyc_yyget_BlockItem_tok(({ struct _tagged_ptr2
_temp4202= yyvs; struct _xenum_struct** _temp4204= _temp4202.curr +(
yyvsp_offset - 1); if( _temp4204 < _temp4202.base? 1: _temp4204 >= _temp4202.last_plus_one){
_throw( Null_Exception);}* _temp4204;})); struct Cyc_List_List* _temp4213;
struct Cyc_Absyn_Fndecl* _temp4215; struct Cyc_Absyn_Stmt* _temp4217; _LL4207:
if((( struct _enum_struct*) _temp4205)->tag == Cyc_Parse_TopDecls_bl_tag){
_LL4214: _temp4213=( struct Cyc_List_List*)(( struct Cyc_Parse_TopDecls_bl_struct*)
_temp4205)->f1; goto _LL4208;} else{ goto _LL4209;} _LL4209: if((( struct
_enum_struct*) _temp4205)->tag == Cyc_Parse_FnDecl_bl_tag){ _LL4216: _temp4215=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Parse_FnDecl_bl_struct*) _temp4205)->f1;
goto _LL4210;} else{ goto _LL4211;} _LL4211: if((( struct _enum_struct*)
_temp4205)->tag == Cyc_Parse_Stmt_bl_tag){ _LL4218: _temp4217=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Parse_Stmt_bl_struct*) _temp4205)->f1; goto _LL4212;} else{ goto
_LL4206;} _LL4208: yyval=({ struct Cyc_Stmt_tok_struct* _temp4219=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));* _temp4219=( struct Cyc_Stmt_tok_struct){.tag=
Cyc_Stmt_tok_tag,.f1= Cyc_Parse_flatten_declarations( _temp4213, Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp4220= yyvs; struct _xenum_struct** _temp4222= _temp4220.curr
+ yyvsp_offset; if( _temp4222 < _temp4220.base? 1: _temp4222 >= _temp4220.last_plus_one){
_throw( Null_Exception);}* _temp4222;})))};( struct _xenum_struct*) _temp4219;});
goto _LL4206; _LL4210: yyval=({ struct Cyc_Stmt_tok_struct* _temp4223=( struct
Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4223=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(({ struct Cyc_Absyn_Fn_d_struct* _temp4224=( struct Cyc_Absyn_Fn_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct)); _temp4224->tag= Cyc_Absyn_Fn_d_tag;
_temp4224->f1= _temp4215;( void*) _temp4224;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4225= yyls; struct Cyc_Yyltype* _temp4227= _temp4225.curr
+( yylsp_offset - 1); if( _temp4227 < _temp4225.base? 1: _temp4227 >= _temp4225.last_plus_one){
_throw( Null_Exception);}* _temp4227;}).first_line,({ struct _tagged_ptr3
_temp4228= yyls; struct Cyc_Yyltype* _temp4230= _temp4228.curr +( yylsp_offset -
1); if( _temp4230 < _temp4228.base? 1: _temp4230 >= _temp4228.last_plus_one){
_throw( Null_Exception);}* _temp4230;}).last_line)), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp4231= yyvs; struct _xenum_struct** _temp4233= _temp4231.curr
+ yyvsp_offset; if( _temp4233 < _temp4231.base? 1: _temp4233 >= _temp4231.last_plus_one){
_throw( Null_Exception);}* _temp4233;})))};( struct _xenum_struct*) _temp4223;});
goto _LL4206; _LL4212: yyval=({ struct Cyc_Stmt_tok_struct* _temp4234=( struct
Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4234=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_seq_stmt(
_temp4217, Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4235= yyvs; struct
_xenum_struct** _temp4237= _temp4235.curr + yyvsp_offset; if( _temp4237 <
_temp4235.base? 1: _temp4237 >= _temp4235.last_plus_one){ _throw( Null_Exception);}*
_temp4237;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4238=
yyls; struct Cyc_Yyltype* _temp4240= _temp4238.curr +( yylsp_offset - 1); if(
_temp4240 < _temp4238.base? 1: _temp4240 >= _temp4238.last_plus_one){ _throw(
Null_Exception);}* _temp4240;}).first_line,({ struct _tagged_ptr3 _temp4241=
yyls; struct Cyc_Yyltype* _temp4243= _temp4241.curr + yylsp_offset; if(
_temp4243 < _temp4241.base? 1: _temp4243 >= _temp4241.last_plus_one){ _throw(
Null_Exception);}* _temp4243;}).last_line))};( struct _xenum_struct*) _temp4234;});
goto _LL4206; _LL4206:;} break; case 205: _LL4201: yyval=({ struct Cyc_BlockItem_tok_struct*
_temp4245=( struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));*
_temp4245=( struct Cyc_BlockItem_tok_struct){.tag= Cyc_BlockItem_tok_tag,.f1=({
struct Cyc_Parse_TopDecls_bl_struct* _temp4246=( struct Cyc_Parse_TopDecls_bl_struct*)
GC_malloc( sizeof( struct Cyc_Parse_TopDecls_bl_struct)); _temp4246->tag= Cyc_Parse_TopDecls_bl_tag;
_temp4246->f1= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp4247= yyvs;
struct _xenum_struct** _temp4249= _temp4247.curr + yyvsp_offset; if( _temp4249 <
_temp4247.base? 1: _temp4249 >= _temp4247.last_plus_one){ _throw( Null_Exception);}*
_temp4249;}));( void*) _temp4246;})};( struct _xenum_struct*) _temp4245;});
break; case 206: _LL4244: yyval=({ struct Cyc_BlockItem_tok_struct* _temp4251=(
struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));*
_temp4251=( struct Cyc_BlockItem_tok_struct){.tag= Cyc_BlockItem_tok_tag,.f1=({
struct Cyc_Parse_Stmt_bl_struct* _temp4252=( struct Cyc_Parse_Stmt_bl_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Stmt_bl_struct)); _temp4252->tag= Cyc_Parse_Stmt_bl_tag;
_temp4252->f1= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4253= yyvs; struct
_xenum_struct** _temp4255= _temp4253.curr + yyvsp_offset; if( _temp4255 <
_temp4253.base? 1: _temp4255 >= _temp4253.last_plus_one){ _throw( Null_Exception);}*
_temp4255;}));( void*) _temp4252;})};( struct _xenum_struct*) _temp4251;});
break; case 207: _LL4250: yyval=({ struct Cyc_BlockItem_tok_struct* _temp4257=(
struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));*
_temp4257=( struct Cyc_BlockItem_tok_struct){.tag= Cyc_BlockItem_tok_tag,.f1=({
struct Cyc_Parse_FnDecl_bl_struct* _temp4258=( struct Cyc_Parse_FnDecl_bl_struct*)
GC_malloc( sizeof( struct Cyc_Parse_FnDecl_bl_struct)); _temp4258->tag= Cyc_Parse_FnDecl_bl_tag;
_temp4258->f1= Cyc_yyget_FnDecl_tok(({ struct _tagged_ptr2 _temp4259= yyvs;
struct _xenum_struct** _temp4261= _temp4259.curr + yyvsp_offset; if( _temp4261 <
_temp4259.base? 1: _temp4261 >= _temp4259.last_plus_one){ _throw( Null_Exception);}*
_temp4261;}));( void*) _temp4258;})};( struct _xenum_struct*) _temp4257;});
break; case 208: _LL4256: yyval=({ struct Cyc_Stmt_tok_struct* _temp4263=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4263=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4264= yyvs; struct _xenum_struct**
_temp4266= _temp4264.curr +( yyvsp_offset - 2); if( _temp4266 < _temp4264.base?
1: _temp4266 >= _temp4264.last_plus_one){ _throw( Null_Exception);}* _temp4266;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4267= yyvs; struct _xenum_struct**
_temp4269= _temp4267.curr + yyvsp_offset; if( _temp4269 < _temp4267.base? 1:
_temp4269 >= _temp4267.last_plus_one){ _throw( Null_Exception);}* _temp4269;})),
Cyc_Absyn_skip_stmt( 0), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4270= yyls; struct Cyc_Yyltype* _temp4272= _temp4270.curr +( yylsp_offset -
4); if( _temp4272 < _temp4270.base? 1: _temp4272 >= _temp4270.last_plus_one){
_throw( Null_Exception);}* _temp4272;}).first_line,({ struct _tagged_ptr3
_temp4273= yyls; struct Cyc_Yyltype* _temp4275= _temp4273.curr + yylsp_offset;
if( _temp4275 < _temp4273.base? 1: _temp4275 >= _temp4273.last_plus_one){ _throw(
Null_Exception);}* _temp4275;}).last_line))};( struct _xenum_struct*) _temp4263;});
break; case 209: _LL4262: yyval=({ struct Cyc_Stmt_tok_struct* _temp4277=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4277=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4278= yyvs; struct _xenum_struct**
_temp4280= _temp4278.curr +( yyvsp_offset - 4); if( _temp4280 < _temp4278.base?
1: _temp4280 >= _temp4278.last_plus_one){ _throw( Null_Exception);}* _temp4280;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4281= yyvs; struct _xenum_struct**
_temp4283= _temp4281.curr +( yyvsp_offset - 2); if( _temp4283 < _temp4281.base?
1: _temp4283 >= _temp4281.last_plus_one){ _throw( Null_Exception);}* _temp4283;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4284= yyvs; struct _xenum_struct**
_temp4286= _temp4284.curr + yyvsp_offset; if( _temp4286 < _temp4284.base? 1:
_temp4286 >= _temp4284.last_plus_one){ _throw( Null_Exception);}* _temp4286;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4287= yyls; struct Cyc_Yyltype*
_temp4289= _temp4287.curr +( yylsp_offset - 6); if( _temp4289 < _temp4287.base?
1: _temp4289 >= _temp4287.last_plus_one){ _throw( Null_Exception);}* _temp4289;}).first_line,({
struct _tagged_ptr3 _temp4290= yyls; struct Cyc_Yyltype* _temp4292= _temp4290.curr
+ yylsp_offset; if( _temp4292 < _temp4290.base? 1: _temp4292 >= _temp4290.last_plus_one){
_throw( Null_Exception);}* _temp4292;}).last_line))};( struct _xenum_struct*)
_temp4277;}); break; case 210: _LL4276: yyval=({ struct Cyc_Stmt_tok_struct*
_temp4294=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4294=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_switch_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4295= yyvs; struct _xenum_struct**
_temp4297= _temp4295.curr +( yyvsp_offset - 4); if( _temp4297 < _temp4295.base?
1: _temp4297 >= _temp4295.last_plus_one){ _throw( Null_Exception);}* _temp4297;})),
Cyc_yyget_SwitchClauseList_tok(({ struct _tagged_ptr2 _temp4298= yyvs; struct
_xenum_struct** _temp4300= _temp4298.curr +( yyvsp_offset - 1); if( _temp4300 <
_temp4298.base? 1: _temp4300 >= _temp4298.last_plus_one){ _throw( Null_Exception);}*
_temp4300;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4301=
yyls; struct Cyc_Yyltype* _temp4303= _temp4301.curr +( yylsp_offset - 6); if(
_temp4303 < _temp4301.base? 1: _temp4303 >= _temp4301.last_plus_one){ _throw(
Null_Exception);}* _temp4303;}).first_line,({ struct _tagged_ptr3 _temp4304=
yyls; struct Cyc_Yyltype* _temp4306= _temp4304.curr + yylsp_offset; if(
_temp4306 < _temp4304.base? 1: _temp4306 >= _temp4304.last_plus_one){ _throw(
Null_Exception);}* _temp4306;}).last_line))};( struct _xenum_struct*) _temp4294;});
break; case 211: _LL4293: yyval=({ struct Cyc_Stmt_tok_struct* _temp4308=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4308=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_trycatch_stmt(
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4309= yyvs; struct _xenum_struct**
_temp4311= _temp4309.curr +( yyvsp_offset - 4); if( _temp4311 < _temp4309.base?
1: _temp4311 >= _temp4309.last_plus_one){ _throw( Null_Exception);}* _temp4311;})),
Cyc_yyget_SwitchClauseList_tok(({ struct _tagged_ptr2 _temp4312= yyvs; struct
_xenum_struct** _temp4314= _temp4312.curr +( yyvsp_offset - 1); if( _temp4314 <
_temp4312.base? 1: _temp4314 >= _temp4312.last_plus_one){ _throw( Null_Exception);}*
_temp4314;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4315=
yyls; struct Cyc_Yyltype* _temp4317= _temp4315.curr +( yylsp_offset - 5); if(
_temp4317 < _temp4315.base? 1: _temp4317 >= _temp4315.last_plus_one){ _throw(
Null_Exception);}* _temp4317;}).first_line,({ struct _tagged_ptr3 _temp4318=
yyls; struct Cyc_Yyltype* _temp4320= _temp4318.curr + yylsp_offset; if(
_temp4320 < _temp4318.base? 1: _temp4320 >= _temp4318.last_plus_one){ _throw(
Null_Exception);}* _temp4320;}).last_line))};( struct _xenum_struct*) _temp4308;});
break; case 212: _LL4307: yyval=({ struct Cyc_SwitchClauseList_tok_struct*
_temp4322=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct));* _temp4322=( struct Cyc_SwitchClauseList_tok_struct){.tag=
Cyc_SwitchClauseList_tok_tag,.f1= 0};( struct _xenum_struct*) _temp4322;});
break; case 213: _LL4321: yyval=({ struct Cyc_SwitchClauseList_tok_struct*
_temp4324=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct));* _temp4324=( struct Cyc_SwitchClauseList_tok_struct){.tag=
Cyc_SwitchClauseList_tok_tag,.f1=({ struct Cyc_List_List* _temp4325=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4325->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp4326=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp4326->pattern= Cyc_Absyn_new_pat( Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4336= yyls; struct Cyc_Yyltype* _temp4338= _temp4336.curr
+( yylsp_offset - 2); if( _temp4338 < _temp4336.base? 1: _temp4338 >= _temp4336.last_plus_one){
_throw( Null_Exception);}* _temp4338;}).first_line,({ struct _tagged_ptr3
_temp4339= yyls; struct Cyc_Yyltype* _temp4341= _temp4339.curr +( yylsp_offset -
2); if( _temp4341 < _temp4339.base? 1: _temp4341 >= _temp4339.last_plus_one){
_throw( Null_Exception);}* _temp4341;}).last_line)); _temp4326->pat_vars= 0;
_temp4326->where_clause= 0; _temp4326->body= Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp4333= yyvs; struct _xenum_struct** _temp4335= _temp4333.curr +
yyvsp_offset; if( _temp4335 < _temp4333.base? 1: _temp4335 >= _temp4333.last_plus_one){
_throw( Null_Exception);}* _temp4335;})); _temp4326->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4327= yyls; struct Cyc_Yyltype* _temp4329= _temp4327.curr
+( yylsp_offset - 2); if( _temp4329 < _temp4327.base? 1: _temp4329 >= _temp4327.last_plus_one){
_throw( Null_Exception);}* _temp4329;}).first_line,({ struct _tagged_ptr3
_temp4330= yyls; struct Cyc_Yyltype* _temp4332= _temp4330.curr + yylsp_offset;
if( _temp4332 < _temp4330.base? 1: _temp4332 >= _temp4330.last_plus_one){ _throw(
Null_Exception);}* _temp4332;}).last_line); _temp4326;}); _temp4325->tl= 0;
_temp4325;})};( struct _xenum_struct*) _temp4324;}); break; case 214: _LL4323:
yyval=({ struct Cyc_SwitchClauseList_tok_struct* _temp4343=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));* _temp4343=( struct
Cyc_SwitchClauseList_tok_struct){.tag= Cyc_SwitchClauseList_tok_tag,.f1=({
struct Cyc_List_List* _temp4344=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4344->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp4348=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp4348->pattern= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp4361= yyvs;
struct _xenum_struct** _temp4363= _temp4361.curr +( yyvsp_offset - 2); if(
_temp4363 < _temp4361.base? 1: _temp4363 >= _temp4361.last_plus_one){ _throw(
Null_Exception);}* _temp4363;})); _temp4348->pat_vars= 0; _temp4348->where_clause=
0; _temp4348->body= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4355= yyls; struct Cyc_Yyltype* _temp4357= _temp4355.curr +(
yylsp_offset - 1); if( _temp4357 < _temp4355.base? 1: _temp4357 >= _temp4355.last_plus_one){
_throw( Null_Exception);}* _temp4357;}).first_line,({ struct _tagged_ptr3
_temp4358= yyls; struct Cyc_Yyltype* _temp4360= _temp4358.curr +( yylsp_offset -
1); if( _temp4360 < _temp4358.base? 1: _temp4360 >= _temp4358.last_plus_one){
_throw( Null_Exception);}* _temp4360;}).last_line)); _temp4348->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4349= yyls; struct Cyc_Yyltype* _temp4351= _temp4349.curr
+( yylsp_offset - 3); if( _temp4351 < _temp4349.base? 1: _temp4351 >= _temp4349.last_plus_one){
_throw( Null_Exception);}* _temp4351;}).first_line,({ struct _tagged_ptr3
_temp4352= yyls; struct Cyc_Yyltype* _temp4354= _temp4352.curr + yylsp_offset;
if( _temp4354 < _temp4352.base? 1: _temp4354 >= _temp4352.last_plus_one){ _throw(
Null_Exception);}* _temp4354;}).last_line); _temp4348;}); _temp4344->tl= Cyc_yyget_SwitchClauseList_tok(({
struct _tagged_ptr2 _temp4345= yyvs; struct _xenum_struct** _temp4347= _temp4345.curr
+ yyvsp_offset; if( _temp4347 < _temp4345.base? 1: _temp4347 >= _temp4345.last_plus_one){
_throw( Null_Exception);}* _temp4347;})); _temp4344;})};( struct _xenum_struct*)
_temp4343;}); break; case 215: _LL4342: yyval=({ struct Cyc_SwitchClauseList_tok_struct*
_temp4365=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct));* _temp4365=( struct Cyc_SwitchClauseList_tok_struct){.tag=
Cyc_SwitchClauseList_tok_tag,.f1=({ struct Cyc_List_List* _temp4366=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4366->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp4370=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp4370->pattern= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp4380= yyvs;
struct _xenum_struct** _temp4382= _temp4380.curr +( yyvsp_offset - 3); if(
_temp4382 < _temp4380.base? 1: _temp4382 >= _temp4380.last_plus_one){ _throw(
Null_Exception);}* _temp4382;})); _temp4370->pat_vars= 0; _temp4370->where_clause=
0; _temp4370->body= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4377= yyvs;
struct _xenum_struct** _temp4379= _temp4377.curr +( yyvsp_offset - 1); if(
_temp4379 < _temp4377.base? 1: _temp4379 >= _temp4377.last_plus_one){ _throw(
Null_Exception);}* _temp4379;})); _temp4370->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4371= yyls; struct Cyc_Yyltype* _temp4373= _temp4371.curr
+( yylsp_offset - 4); if( _temp4373 < _temp4371.base? 1: _temp4373 >= _temp4371.last_plus_one){
_throw( Null_Exception);}* _temp4373;}).first_line,({ struct _tagged_ptr3
_temp4374= yyls; struct Cyc_Yyltype* _temp4376= _temp4374.curr +( yylsp_offset -
1); if( _temp4376 < _temp4374.base? 1: _temp4376 >= _temp4374.last_plus_one){
_throw( Null_Exception);}* _temp4376;}).last_line); _temp4370;}); _temp4366->tl=
Cyc_yyget_SwitchClauseList_tok(({ struct _tagged_ptr2 _temp4367= yyvs; struct
_xenum_struct** _temp4369= _temp4367.curr + yyvsp_offset; if( _temp4369 <
_temp4367.base? 1: _temp4369 >= _temp4367.last_plus_one){ _throw( Null_Exception);}*
_temp4369;})); _temp4366;})};( struct _xenum_struct*) _temp4365;}); break; case
216: _LL4364: yyval=({ struct Cyc_SwitchClauseList_tok_struct* _temp4384=(
struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));*
_temp4384=( struct Cyc_SwitchClauseList_tok_struct){.tag= Cyc_SwitchClauseList_tok_tag,.f1=({
struct Cyc_List_List* _temp4385=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4385->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp4389=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp4389->pattern= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp4405= yyvs;
struct _xenum_struct** _temp4407= _temp4405.curr +( yyvsp_offset - 4); if(
_temp4407 < _temp4405.base? 1: _temp4407 >= _temp4405.last_plus_one){ _throw(
Null_Exception);}* _temp4407;})); _temp4389->pat_vars= 0; _temp4389->where_clause=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4402= yyvs;
struct _xenum_struct** _temp4404= _temp4402.curr +( yyvsp_offset - 2); if(
_temp4404 < _temp4402.base? 1: _temp4404 >= _temp4402.last_plus_one){ _throw(
Null_Exception);}* _temp4404;})); _temp4389->body= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4396= yyls; struct Cyc_Yyltype* _temp4398= _temp4396.curr
+( yylsp_offset - 1); if( _temp4398 < _temp4396.base? 1: _temp4398 >= _temp4396.last_plus_one){
_throw( Null_Exception);}* _temp4398;}).first_line,({ struct _tagged_ptr3
_temp4399= yyls; struct Cyc_Yyltype* _temp4401= _temp4399.curr +( yylsp_offset -
1); if( _temp4401 < _temp4399.base? 1: _temp4401 >= _temp4399.last_plus_one){
_throw( Null_Exception);}* _temp4401;}).last_line)); _temp4389->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4390= yyls; struct Cyc_Yyltype* _temp4392= _temp4390.curr
+( yylsp_offset - 5); if( _temp4392 < _temp4390.base? 1: _temp4392 >= _temp4390.last_plus_one){
_throw( Null_Exception);}* _temp4392;}).first_line,({ struct _tagged_ptr3
_temp4393= yyls; struct Cyc_Yyltype* _temp4395= _temp4393.curr + yylsp_offset;
if( _temp4395 < _temp4393.base? 1: _temp4395 >= _temp4393.last_plus_one){ _throw(
Null_Exception);}* _temp4395;}).last_line); _temp4389;}); _temp4385->tl= Cyc_yyget_SwitchClauseList_tok(({
struct _tagged_ptr2 _temp4386= yyvs; struct _xenum_struct** _temp4388= _temp4386.curr
+ yyvsp_offset; if( _temp4388 < _temp4386.base? 1: _temp4388 >= _temp4386.last_plus_one){
_throw( Null_Exception);}* _temp4388;})); _temp4385;})};( struct _xenum_struct*)
_temp4384;}); break; case 217: _LL4383: yyval=({ struct Cyc_SwitchClauseList_tok_struct*
_temp4409=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct));* _temp4409=( struct Cyc_SwitchClauseList_tok_struct){.tag=
Cyc_SwitchClauseList_tok_tag,.f1=({ struct Cyc_List_List* _temp4410=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4410->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp4414=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp4414->pattern= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp4427= yyvs;
struct _xenum_struct** _temp4429= _temp4427.curr +( yyvsp_offset - 5); if(
_temp4429 < _temp4427.base? 1: _temp4429 >= _temp4427.last_plus_one){ _throw(
Null_Exception);}* _temp4429;})); _temp4414->pat_vars= 0; _temp4414->where_clause=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4424= yyvs;
struct _xenum_struct** _temp4426= _temp4424.curr +( yyvsp_offset - 3); if(
_temp4426 < _temp4424.base? 1: _temp4426 >= _temp4424.last_plus_one){ _throw(
Null_Exception);}* _temp4426;})); _temp4414->body= Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp4421= yyvs; struct _xenum_struct** _temp4423= _temp4421.curr +(
yyvsp_offset - 1); if( _temp4423 < _temp4421.base? 1: _temp4423 >= _temp4421.last_plus_one){
_throw( Null_Exception);}* _temp4423;})); _temp4414->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4415= yyls; struct Cyc_Yyltype* _temp4417= _temp4415.curr
+( yylsp_offset - 6); if( _temp4417 < _temp4415.base? 1: _temp4417 >= _temp4415.last_plus_one){
_throw( Null_Exception);}* _temp4417;}).first_line,({ struct _tagged_ptr3
_temp4418= yyls; struct Cyc_Yyltype* _temp4420= _temp4418.curr + yylsp_offset;
if( _temp4420 < _temp4418.base? 1: _temp4420 >= _temp4418.last_plus_one){ _throw(
Null_Exception);}* _temp4420;}).last_line); _temp4414;}); _temp4410->tl= Cyc_yyget_SwitchClauseList_tok(({
struct _tagged_ptr2 _temp4411= yyvs; struct _xenum_struct** _temp4413= _temp4411.curr
+ yyvsp_offset; if( _temp4413 < _temp4411.base? 1: _temp4413 >= _temp4411.last_plus_one){
_throw( Null_Exception);}* _temp4413;})); _temp4410;})};( struct _xenum_struct*)
_temp4409;}); break; case 218: _LL4408: yyval=({ struct Cyc_Stmt_tok_struct*
_temp4431=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4431=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_while_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4432= yyvs; struct _xenum_struct**
_temp4434= _temp4432.curr +( yyvsp_offset - 2); if( _temp4434 < _temp4432.base?
1: _temp4434 >= _temp4432.last_plus_one){ _throw( Null_Exception);}* _temp4434;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4435= yyvs; struct _xenum_struct**
_temp4437= _temp4435.curr + yyvsp_offset; if( _temp4437 < _temp4435.base? 1:
_temp4437 >= _temp4435.last_plus_one){ _throw( Null_Exception);}* _temp4437;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4438= yyls; struct Cyc_Yyltype*
_temp4440= _temp4438.curr +( yylsp_offset - 4); if( _temp4440 < _temp4438.base?
1: _temp4440 >= _temp4438.last_plus_one){ _throw( Null_Exception);}* _temp4440;}).first_line,({
struct _tagged_ptr3 _temp4441= yyls; struct Cyc_Yyltype* _temp4443= _temp4441.curr
+ yylsp_offset; if( _temp4443 < _temp4441.base? 1: _temp4443 >= _temp4441.last_plus_one){
_throw( Null_Exception);}* _temp4443;}).last_line))};( struct _xenum_struct*)
_temp4431;}); break; case 219: _LL4430: yyval=({ struct Cyc_Stmt_tok_struct*
_temp4445=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4445=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_do_stmt(
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4446= yyvs; struct _xenum_struct**
_temp4448= _temp4446.curr +( yyvsp_offset - 5); if( _temp4448 < _temp4446.base?
1: _temp4448 >= _temp4446.last_plus_one){ _throw( Null_Exception);}* _temp4448;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4449= yyvs; struct _xenum_struct**
_temp4451= _temp4449.curr +( yyvsp_offset - 2); if( _temp4451 < _temp4449.base?
1: _temp4451 >= _temp4449.last_plus_one){ _throw( Null_Exception);}* _temp4451;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4452= yyls; struct Cyc_Yyltype*
_temp4454= _temp4452.curr +( yylsp_offset - 6); if( _temp4454 < _temp4452.base?
1: _temp4454 >= _temp4452.last_plus_one){ _throw( Null_Exception);}* _temp4454;}).first_line,({
struct _tagged_ptr3 _temp4455= yyls; struct Cyc_Yyltype* _temp4457= _temp4455.curr
+ yylsp_offset; if( _temp4457 < _temp4455.base? 1: _temp4457 >= _temp4455.last_plus_one){
_throw( Null_Exception);}* _temp4457;}).last_line))};( struct _xenum_struct*)
_temp4445;}); break; case 220: _LL4444: yyval=({ struct Cyc_Stmt_tok_struct*
_temp4459=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4459=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp4460= yyvs; struct _xenum_struct** _temp4462= _temp4460.curr
+ yyvsp_offset; if( _temp4462 < _temp4460.base? 1: _temp4462 >= _temp4460.last_plus_one){
_throw( Null_Exception);}* _temp4462;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4463= yyls; struct Cyc_Yyltype* _temp4465= _temp4463.curr +(
yylsp_offset - 5); if( _temp4465 < _temp4463.base? 1: _temp4465 >= _temp4463.last_plus_one){
_throw( Null_Exception);}* _temp4465;}).first_line,({ struct _tagged_ptr3
_temp4466= yyls; struct Cyc_Yyltype* _temp4468= _temp4466.curr + yylsp_offset;
if( _temp4468 < _temp4466.base? 1: _temp4468 >= _temp4466.last_plus_one){ _throw(
Null_Exception);}* _temp4468;}).last_line))};( struct _xenum_struct*) _temp4459;});
break; case 221: _LL4458: yyval=({ struct Cyc_Stmt_tok_struct* _temp4470=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4470=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4471= yyvs; struct _xenum_struct** _temp4473= _temp4471.curr +(
yyvsp_offset - 2); if( _temp4473 < _temp4471.base? 1: _temp4473 >= _temp4471.last_plus_one){
_throw( Null_Exception);}* _temp4473;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp4474= yyvs; struct _xenum_struct** _temp4476= _temp4474.curr +
yyvsp_offset; if( _temp4476 < _temp4474.base? 1: _temp4476 >= _temp4474.last_plus_one){
_throw( Null_Exception);}* _temp4476;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4477= yyls; struct Cyc_Yyltype* _temp4479= _temp4477.curr +(
yylsp_offset - 6); if( _temp4479 < _temp4477.base? 1: _temp4479 >= _temp4477.last_plus_one){
_throw( Null_Exception);}* _temp4479;}).first_line,({ struct _tagged_ptr3
_temp4480= yyls; struct Cyc_Yyltype* _temp4482= _temp4480.curr + yylsp_offset;
if( _temp4482 < _temp4480.base? 1: _temp4482 >= _temp4480.last_plus_one){ _throw(
Null_Exception);}* _temp4482;}).last_line))};( struct _xenum_struct*) _temp4470;});
break; case 222: _LL4469: yyval=({ struct Cyc_Stmt_tok_struct* _temp4484=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4484=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4485=
yyvs; struct _xenum_struct** _temp4487= _temp4485.curr +( yyvsp_offset - 3); if(
_temp4487 < _temp4485.base? 1: _temp4487 >= _temp4485.last_plus_one){ _throw(
Null_Exception);}* _temp4487;})), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp4488= yyvs; struct _xenum_struct** _temp4490= _temp4488.curr
+ yyvsp_offset; if( _temp4490 < _temp4488.base? 1: _temp4490 >= _temp4488.last_plus_one){
_throw( Null_Exception);}* _temp4490;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4491= yyls; struct Cyc_Yyltype* _temp4493= _temp4491.curr +(
yylsp_offset - 6); if( _temp4493 < _temp4491.base? 1: _temp4493 >= _temp4491.last_plus_one){
_throw( Null_Exception);}* _temp4493;}).first_line,({ struct _tagged_ptr3
_temp4494= yyls; struct Cyc_Yyltype* _temp4496= _temp4494.curr + yylsp_offset;
if( _temp4496 < _temp4494.base? 1: _temp4496 >= _temp4494.last_plus_one){ _throw(
Null_Exception);}* _temp4496;}).last_line))};( struct _xenum_struct*) _temp4484;});
break; case 223: _LL4483: yyval=({ struct Cyc_Stmt_tok_struct* _temp4498=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4498=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4499=
yyvs; struct _xenum_struct** _temp4501= _temp4499.curr +( yyvsp_offset - 4); if(
_temp4501 < _temp4499.base? 1: _temp4501 >= _temp4499.last_plus_one){ _throw(
Null_Exception);}* _temp4501;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp4502= yyvs; struct _xenum_struct** _temp4504= _temp4502.curr +(
yyvsp_offset - 2); if( _temp4504 < _temp4502.base? 1: _temp4504 >= _temp4502.last_plus_one){
_throw( Null_Exception);}* _temp4504;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp4505= yyvs; struct _xenum_struct** _temp4507= _temp4505.curr +
yyvsp_offset; if( _temp4507 < _temp4505.base? 1: _temp4507 >= _temp4505.last_plus_one){
_throw( Null_Exception);}* _temp4507;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4508= yyls; struct Cyc_Yyltype* _temp4510= _temp4508.curr +(
yylsp_offset - 7); if( _temp4510 < _temp4508.base? 1: _temp4510 >= _temp4508.last_plus_one){
_throw( Null_Exception);}* _temp4510;}).first_line,({ struct _tagged_ptr3
_temp4511= yyls; struct Cyc_Yyltype* _temp4513= _temp4511.curr +( yylsp_offset -
1); if( _temp4513 < _temp4511.base? 1: _temp4513 >= _temp4511.last_plus_one){
_throw( Null_Exception);}* _temp4513;}).last_line))};( struct _xenum_struct*)
_temp4498;}); break; case 224: _LL4497: yyval=({ struct Cyc_Stmt_tok_struct*
_temp4515=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4515=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_for_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4516= yyvs; struct _xenum_struct**
_temp4518= _temp4516.curr +( yyvsp_offset - 4); if( _temp4518 < _temp4516.base?
1: _temp4518 >= _temp4516.last_plus_one){ _throw( Null_Exception);}* _temp4518;})),
Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp4519= yyvs; struct _xenum_struct** _temp4521= _temp4519.curr +
yyvsp_offset; if( _temp4521 < _temp4519.base? 1: _temp4521 >= _temp4519.last_plus_one){
_throw( Null_Exception);}* _temp4521;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4522= yyls; struct Cyc_Yyltype* _temp4524= _temp4522.curr +(
yylsp_offset - 6); if( _temp4524 < _temp4522.base? 1: _temp4524 >= _temp4522.last_plus_one){
_throw( Null_Exception);}* _temp4524;}).first_line,({ struct _tagged_ptr3
_temp4525= yyls; struct Cyc_Yyltype* _temp4527= _temp4525.curr + yylsp_offset;
if( _temp4527 < _temp4525.base? 1: _temp4527 >= _temp4525.last_plus_one){ _throw(
Null_Exception);}* _temp4527;}).last_line))};( struct _xenum_struct*) _temp4515;});
break; case 225: _LL4514: yyval=({ struct Cyc_Stmt_tok_struct* _temp4529=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4529=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_for_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4530= yyvs; struct _xenum_struct**
_temp4532= _temp4530.curr +( yyvsp_offset - 5); if( _temp4532 < _temp4530.base?
1: _temp4532 >= _temp4530.last_plus_one){ _throw( Null_Exception);}* _temp4532;})),
Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4533= yyvs;
struct _xenum_struct** _temp4535= _temp4533.curr +( yyvsp_offset - 2); if(
_temp4535 < _temp4533.base? 1: _temp4535 >= _temp4533.last_plus_one){ _throw(
Null_Exception);}* _temp4535;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp4536= yyvs; struct _xenum_struct** _temp4538= _temp4536.curr + yyvsp_offset;
if( _temp4538 < _temp4536.base? 1: _temp4538 >= _temp4536.last_plus_one){ _throw(
Null_Exception);}* _temp4538;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4539= yyls; struct Cyc_Yyltype* _temp4541= _temp4539.curr +(
yylsp_offset - 7); if( _temp4541 < _temp4539.base? 1: _temp4541 >= _temp4539.last_plus_one){
_throw( Null_Exception);}* _temp4541;}).first_line,({ struct _tagged_ptr3
_temp4542= yyls; struct Cyc_Yyltype* _temp4544= _temp4542.curr + yylsp_offset;
if( _temp4544 < _temp4542.base? 1: _temp4544 >= _temp4542.last_plus_one){ _throw(
Null_Exception);}* _temp4544;}).last_line))};( struct _xenum_struct*) _temp4529;});
break; case 226: _LL4528: yyval=({ struct Cyc_Stmt_tok_struct* _temp4546=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4546=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_for_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4547= yyvs; struct _xenum_struct**
_temp4549= _temp4547.curr +( yyvsp_offset - 5); if( _temp4549 < _temp4547.base?
1: _temp4549 >= _temp4547.last_plus_one){ _throw( Null_Exception);}* _temp4549;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4550= yyvs; struct _xenum_struct**
_temp4552= _temp4550.curr +( yyvsp_offset - 3); if( _temp4552 < _temp4550.base?
1: _temp4552 >= _temp4550.last_plus_one){ _throw( Null_Exception);}* _temp4552;})),
Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4553=
yyvs; struct _xenum_struct** _temp4555= _temp4553.curr + yyvsp_offset; if(
_temp4555 < _temp4553.base? 1: _temp4555 >= _temp4553.last_plus_one){ _throw(
Null_Exception);}* _temp4555;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4556= yyls; struct Cyc_Yyltype* _temp4558= _temp4556.curr +(
yylsp_offset - 7); if( _temp4558 < _temp4556.base? 1: _temp4558 >= _temp4556.last_plus_one){
_throw( Null_Exception);}* _temp4558;}).first_line,({ struct _tagged_ptr3
_temp4559= yyls; struct Cyc_Yyltype* _temp4561= _temp4559.curr + yylsp_offset;
if( _temp4561 < _temp4559.base? 1: _temp4561 >= _temp4559.last_plus_one){ _throw(
Null_Exception);}* _temp4561;}).last_line))};( struct _xenum_struct*) _temp4546;});
break; case 227: _LL4545: yyval=({ struct Cyc_Stmt_tok_struct* _temp4563=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4563=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_for_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4564= yyvs; struct _xenum_struct**
_temp4566= _temp4564.curr +( yyvsp_offset - 6); if( _temp4566 < _temp4564.base?
1: _temp4566 >= _temp4564.last_plus_one){ _throw( Null_Exception);}* _temp4566;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4567= yyvs; struct _xenum_struct**
_temp4569= _temp4567.curr +( yyvsp_offset - 4); if( _temp4569 < _temp4567.base?
1: _temp4569 >= _temp4567.last_plus_one){ _throw( Null_Exception);}* _temp4569;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4570= yyvs; struct _xenum_struct**
_temp4572= _temp4570.curr +( yyvsp_offset - 2); if( _temp4572 < _temp4570.base?
1: _temp4572 >= _temp4570.last_plus_one){ _throw( Null_Exception);}* _temp4572;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4573= yyvs; struct _xenum_struct**
_temp4575= _temp4573.curr + yyvsp_offset; if( _temp4575 < _temp4573.base? 1:
_temp4575 >= _temp4573.last_plus_one){ _throw( Null_Exception);}* _temp4575;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4576= yyls; struct Cyc_Yyltype*
_temp4578= _temp4576.curr +( yylsp_offset - 8); if( _temp4578 < _temp4576.base?
1: _temp4578 >= _temp4576.last_plus_one){ _throw( Null_Exception);}* _temp4578;}).first_line,({
struct _tagged_ptr3 _temp4579= yyls; struct Cyc_Yyltype* _temp4581= _temp4579.curr
+ yylsp_offset; if( _temp4581 < _temp4579.base? 1: _temp4581 >= _temp4579.last_plus_one){
_throw( Null_Exception);}* _temp4581;}).last_line))};( struct _xenum_struct*)
_temp4563;}); break; case 228: _LL4562: { struct Cyc_List_List* decls= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp4583= yyvs; struct _xenum_struct** _temp4585= _temp4583.curr
+( yyvsp_offset - 3); if( _temp4585 < _temp4583.base? 1: _temp4585 >= _temp4583.last_plus_one){
_throw( Null_Exception);}* _temp4585;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp4586= yyvs; struct _xenum_struct** _temp4588= _temp4586.curr
+ yyvsp_offset; if( _temp4588 < _temp4586.base? 1: _temp4588 >= _temp4586.last_plus_one){
_throw( Null_Exception);}* _temp4588;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4589= yyls; struct Cyc_Yyltype* _temp4591= _temp4589.curr +(
yylsp_offset - 5); if( _temp4591 < _temp4589.base? 1: _temp4591 >= _temp4589.last_plus_one){
_throw( Null_Exception);}* _temp4591;}).first_line,({ struct _tagged_ptr3
_temp4592= yyls; struct Cyc_Yyltype* _temp4594= _temp4592.curr + yylsp_offset;
if( _temp4594 < _temp4592.base? 1: _temp4594 >= _temp4592.last_plus_one){ _throw(
Null_Exception);}* _temp4594;}).last_line)); yyval=({ struct Cyc_Stmt_tok_struct*
_temp4595=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4595=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Parse_flatten_declarations(
decls, s)};( struct _xenum_struct*) _temp4595;}); break;} case 229: _LL4582: {
struct Cyc_List_List* decls= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2
_temp4597= yyvs; struct _xenum_struct** _temp4599= _temp4597.curr +(
yyvsp_offset - 4); if( _temp4599 < _temp4597.base? 1: _temp4599 >= _temp4597.last_plus_one){
_throw( Null_Exception);}* _temp4599;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4600=
yyvs; struct _xenum_struct** _temp4602= _temp4600.curr +( yyvsp_offset - 3); if(
_temp4602 < _temp4600.base? 1: _temp4602 >= _temp4600.last_plus_one){ _throw(
Null_Exception);}* _temp4602;})), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp4603= yyvs; struct _xenum_struct** _temp4605= _temp4603.curr
+ yyvsp_offset; if( _temp4605 < _temp4603.base? 1: _temp4605 >= _temp4603.last_plus_one){
_throw( Null_Exception);}* _temp4605;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4606= yyls; struct Cyc_Yyltype* _temp4608= _temp4606.curr +(
yylsp_offset - 6); if( _temp4608 < _temp4606.base? 1: _temp4608 >= _temp4606.last_plus_one){
_throw( Null_Exception);}* _temp4608;}).first_line,({ struct _tagged_ptr3
_temp4609= yyls; struct Cyc_Yyltype* _temp4611= _temp4609.curr + yylsp_offset;
if( _temp4611 < _temp4609.base? 1: _temp4611 >= _temp4609.last_plus_one){ _throw(
Null_Exception);}* _temp4611;}).last_line)); yyval=({ struct Cyc_Stmt_tok_struct*
_temp4612=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4612=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Parse_flatten_declarations(
decls, s)};( struct _xenum_struct*) _temp4612;}); break;} case 230: _LL4596: {
struct Cyc_List_List* decls= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2
_temp4614= yyvs; struct _xenum_struct** _temp4616= _temp4614.curr +(
yyvsp_offset - 4); if( _temp4616 < _temp4614.base? 1: _temp4616 >= _temp4614.last_plus_one){
_throw( Null_Exception);}* _temp4616;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4617= yyvs; struct _xenum_struct** _temp4619= _temp4617.curr +(
yyvsp_offset - 2); if( _temp4619 < _temp4617.base? 1: _temp4619 >= _temp4617.last_plus_one){
_throw( Null_Exception);}* _temp4619;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp4620= yyvs; struct _xenum_struct** _temp4622= _temp4620.curr +
yyvsp_offset; if( _temp4622 < _temp4620.base? 1: _temp4622 >= _temp4620.last_plus_one){
_throw( Null_Exception);}* _temp4622;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4623= yyls; struct Cyc_Yyltype* _temp4625= _temp4623.curr +(
yylsp_offset - 6); if( _temp4625 < _temp4623.base? 1: _temp4625 >= _temp4623.last_plus_one){
_throw( Null_Exception);}* _temp4625;}).first_line,({ struct _tagged_ptr3
_temp4626= yyls; struct Cyc_Yyltype* _temp4628= _temp4626.curr + yylsp_offset;
if( _temp4628 < _temp4626.base? 1: _temp4628 >= _temp4626.last_plus_one){ _throw(
Null_Exception);}* _temp4628;}).last_line)); yyval=({ struct Cyc_Stmt_tok_struct*
_temp4629=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4629=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Parse_flatten_declarations(
decls, s)};( struct _xenum_struct*) _temp4629;}); break;} case 231: _LL4613: {
struct Cyc_List_List* decls= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2
_temp4631= yyvs; struct _xenum_struct** _temp4633= _temp4631.curr +(
yyvsp_offset - 5); if( _temp4633 < _temp4631.base? 1: _temp4633 >= _temp4631.last_plus_one){
_throw( Null_Exception);}* _temp4633;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4634=
yyvs; struct _xenum_struct** _temp4636= _temp4634.curr +( yyvsp_offset - 4); if(
_temp4636 < _temp4634.base? 1: _temp4636 >= _temp4634.last_plus_one){ _throw(
Null_Exception);}* _temp4636;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp4637= yyvs; struct _xenum_struct** _temp4639= _temp4637.curr +(
yyvsp_offset - 2); if( _temp4639 < _temp4637.base? 1: _temp4639 >= _temp4637.last_plus_one){
_throw( Null_Exception);}* _temp4639;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp4640= yyvs; struct _xenum_struct** _temp4642= _temp4640.curr +
yyvsp_offset; if( _temp4642 < _temp4640.base? 1: _temp4642 >= _temp4640.last_plus_one){
_throw( Null_Exception);}* _temp4642;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4643= yyls; struct Cyc_Yyltype* _temp4645= _temp4643.curr +(
yylsp_offset - 7); if( _temp4645 < _temp4643.base? 1: _temp4645 >= _temp4643.last_plus_one){
_throw( Null_Exception);}* _temp4645;}).first_line,({ struct _tagged_ptr3
_temp4646= yyls; struct Cyc_Yyltype* _temp4648= _temp4646.curr + yylsp_offset;
if( _temp4648 < _temp4646.base? 1: _temp4648 >= _temp4646.last_plus_one){ _throw(
Null_Exception);}* _temp4648;}).last_line)); yyval=({ struct Cyc_Stmt_tok_struct*
_temp4649=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4649=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Parse_flatten_declarations(
decls, s)};( struct _xenum_struct*) _temp4649;}); break;} case 232: _LL4630:
yyval=({ struct Cyc_Stmt_tok_struct* _temp4651=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));* _temp4651=( struct Cyc_Stmt_tok_struct){.tag=
Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_goto_stmt(({ struct _tagged_string* _temp4652=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp4652[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp4653= yyvs;
struct _xenum_struct** _temp4655= _temp4653.curr +( yyvsp_offset - 1); if(
_temp4655 < _temp4653.base? 1: _temp4655 >= _temp4653.last_plus_one){ _throw(
Null_Exception);}* _temp4655;})); _temp4652;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4656= yyls; struct Cyc_Yyltype* _temp4658= _temp4656.curr
+( yylsp_offset - 2); if( _temp4658 < _temp4656.base? 1: _temp4658 >= _temp4656.last_plus_one){
_throw( Null_Exception);}* _temp4658;}).first_line,({ struct _tagged_ptr3
_temp4659= yyls; struct Cyc_Yyltype* _temp4661= _temp4659.curr +( yylsp_offset -
1); if( _temp4661 < _temp4659.base? 1: _temp4661 >= _temp4659.last_plus_one){
_throw( Null_Exception);}* _temp4661;}).last_line))};( struct _xenum_struct*)
_temp4651;}); break; case 233: _LL4650: yyval=({ struct Cyc_Stmt_tok_struct*
_temp4663=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4663=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_continue_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4664= yyls; struct Cyc_Yyltype*
_temp4666= _temp4664.curr +( yylsp_offset - 1); if( _temp4666 < _temp4664.base?
1: _temp4666 >= _temp4664.last_plus_one){ _throw( Null_Exception);}* _temp4666;}).first_line,({
struct _tagged_ptr3 _temp4667= yyls; struct Cyc_Yyltype* _temp4669= _temp4667.curr
+( yylsp_offset - 1); if( _temp4669 < _temp4667.base? 1: _temp4669 >= _temp4667.last_plus_one){
_throw( Null_Exception);}* _temp4669;}).last_line))};( struct _xenum_struct*)
_temp4663;}); break; case 234: _LL4662: yyval=({ struct Cyc_Stmt_tok_struct*
_temp4671=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4671=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_break_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4672= yyls; struct Cyc_Yyltype*
_temp4674= _temp4672.curr +( yylsp_offset - 1); if( _temp4674 < _temp4672.base?
1: _temp4674 >= _temp4672.last_plus_one){ _throw( Null_Exception);}* _temp4674;}).first_line,({
struct _tagged_ptr3 _temp4675= yyls; struct Cyc_Yyltype* _temp4677= _temp4675.curr
+( yylsp_offset - 1); if( _temp4677 < _temp4675.base? 1: _temp4677 >= _temp4675.last_plus_one){
_throw( Null_Exception);}* _temp4677;}).last_line))};( struct _xenum_struct*)
_temp4671;}); break; case 235: _LL4670: yyval=({ struct Cyc_Stmt_tok_struct*
_temp4679=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4679=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_return_stmt(
0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4680= yyls; struct
Cyc_Yyltype* _temp4682= _temp4680.curr +( yylsp_offset - 1); if( _temp4682 <
_temp4680.base? 1: _temp4682 >= _temp4680.last_plus_one){ _throw( Null_Exception);}*
_temp4682;}).first_line,({ struct _tagged_ptr3 _temp4683= yyls; struct Cyc_Yyltype*
_temp4685= _temp4683.curr +( yylsp_offset - 1); if( _temp4685 < _temp4683.base?
1: _temp4685 >= _temp4683.last_plus_one){ _throw( Null_Exception);}* _temp4685;}).last_line))};(
struct _xenum_struct*) _temp4679;}); break; case 236: _LL4678: yyval=({ struct
Cyc_Stmt_tok_struct* _temp4687=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct));* _temp4687=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1=
Cyc_Absyn_return_stmt(( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4688= yyvs; struct _xenum_struct** _temp4690= _temp4688.curr +(
yyvsp_offset - 1); if( _temp4690 < _temp4688.base? 1: _temp4690 >= _temp4688.last_plus_one){
_throw( Null_Exception);}* _temp4690;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4691= yyls; struct Cyc_Yyltype* _temp4693= _temp4691.curr +(
yylsp_offset - 2); if( _temp4693 < _temp4691.base? 1: _temp4693 >= _temp4691.last_plus_one){
_throw( Null_Exception);}* _temp4693;}).first_line,({ struct _tagged_ptr3
_temp4694= yyls; struct Cyc_Yyltype* _temp4696= _temp4694.curr +( yylsp_offset -
1); if( _temp4696 < _temp4694.base? 1: _temp4696 >= _temp4694.last_plus_one){
_throw( Null_Exception);}* _temp4696;}).last_line))};( struct _xenum_struct*)
_temp4687;}); break; case 237: _LL4686: yyval=({ struct Cyc_Stmt_tok_struct*
_temp4698=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4698=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_fallthru_stmt(
0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4699= yyls; struct
Cyc_Yyltype* _temp4701= _temp4699.curr +( yylsp_offset - 1); if( _temp4701 <
_temp4699.base? 1: _temp4701 >= _temp4699.last_plus_one){ _throw( Null_Exception);}*
_temp4701;}).first_line,({ struct _tagged_ptr3 _temp4702= yyls; struct Cyc_Yyltype*
_temp4704= _temp4702.curr +( yylsp_offset - 1); if( _temp4704 < _temp4702.base?
1: _temp4704 >= _temp4702.last_plus_one){ _throw( Null_Exception);}* _temp4704;}).last_line))};(
struct _xenum_struct*) _temp4698;}); break; case 238: _LL4697: yyval=({ struct
Cyc_Stmt_tok_struct* _temp4706=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct));* _temp4706=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1=
Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4707= yyls; struct Cyc_Yyltype* _temp4709= _temp4707.curr +( yylsp_offset -
3); if( _temp4709 < _temp4707.base? 1: _temp4709 >= _temp4707.last_plus_one){
_throw( Null_Exception);}* _temp4709;}).first_line,({ struct _tagged_ptr3
_temp4710= yyls; struct Cyc_Yyltype* _temp4712= _temp4710.curr +( yylsp_offset -
3); if( _temp4712 < _temp4710.base? 1: _temp4712 >= _temp4710.last_plus_one){
_throw( Null_Exception);}* _temp4712;}).last_line))};( struct _xenum_struct*)
_temp4706;}); break; case 239: _LL4705: yyval=({ struct Cyc_Stmt_tok_struct*
_temp4714=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4714=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2 _temp4715= yyvs; struct
_xenum_struct** _temp4717= _temp4715.curr +( yyvsp_offset - 2); if( _temp4717 <
_temp4715.base? 1: _temp4717 >= _temp4715.last_plus_one){ _throw( Null_Exception);}*
_temp4717;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4718=
yyls; struct Cyc_Yyltype* _temp4720= _temp4718.curr +( yylsp_offset - 4); if(
_temp4720 < _temp4718.base? 1: _temp4720 >= _temp4718.last_plus_one){ _throw(
Null_Exception);}* _temp4720;}).first_line,({ struct _tagged_ptr3 _temp4721=
yyls; struct Cyc_Yyltype* _temp4723= _temp4721.curr +( yylsp_offset - 4); if(
_temp4723 < _temp4721.base? 1: _temp4723 >= _temp4721.last_plus_one){ _throw(
Null_Exception);}* _temp4723;}).last_line))};( struct _xenum_struct*) _temp4714;});
break; case 240: _LL4713: yyval=({ struct Cyc_Pattern_tok_struct* _temp4725=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp4725=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(
Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4726=
yyls; struct Cyc_Yyltype* _temp4728= _temp4726.curr + yylsp_offset; if(
_temp4728 < _temp4726.base? 1: _temp4728 >= _temp4726.last_plus_one){ _throw(
Null_Exception);}* _temp4728;}).first_line,({ struct _tagged_ptr3 _temp4729=
yyls; struct Cyc_Yyltype* _temp4731= _temp4729.curr + yylsp_offset; if(
_temp4731 < _temp4729.base? 1: _temp4731 >= _temp4729.last_plus_one){ _throw(
Null_Exception);}* _temp4731;}).last_line))};( struct _xenum_struct*) _temp4725;});
break; case 241: _LL4724: yyval=({ struct _tagged_ptr2 _temp4733= yyvs; struct
_xenum_struct** _temp4735= _temp4733.curr +( yyvsp_offset - 1); if( _temp4735 <
_temp4733.base? 1: _temp4735 >= _temp4733.last_plus_one){ _throw( Null_Exception);}*
_temp4735;}); break; case 242: _LL4732: yyval=({ struct Cyc_Pattern_tok_struct*
_temp4737=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp4737=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(({
struct Cyc_Absyn_Int_p_struct* _temp4738=( struct Cyc_Absyn_Int_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct)); _temp4738->tag= Cyc_Absyn_Int_p_tag;
_temp4738->f1=( void*)(* Cyc_yyget_Int_tok(({ struct _tagged_ptr2 _temp4739=
yyvs; struct _xenum_struct** _temp4741= _temp4739.curr + yyvsp_offset; if(
_temp4741 < _temp4739.base? 1: _temp4741 >= _temp4739.last_plus_one){ _throw(
Null_Exception);}* _temp4741;}))).f1; _temp4738->f2=(* Cyc_yyget_Int_tok(({
struct _tagged_ptr2 _temp4742= yyvs; struct _xenum_struct** _temp4744= _temp4742.curr
+ yyvsp_offset; if( _temp4744 < _temp4742.base? 1: _temp4744 >= _temp4742.last_plus_one){
_throw( Null_Exception);}* _temp4744;}))).f2;( void*) _temp4738;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4745= yyls; struct Cyc_Yyltype* _temp4747= _temp4745.curr
+ yylsp_offset; if( _temp4747 < _temp4745.base? 1: _temp4747 >= _temp4745.last_plus_one){
_throw( Null_Exception);}* _temp4747;}).first_line,({ struct _tagged_ptr3
_temp4748= yyls; struct Cyc_Yyltype* _temp4750= _temp4748.curr + yylsp_offset;
if( _temp4750 < _temp4748.base? 1: _temp4750 >= _temp4748.last_plus_one){ _throw(
Null_Exception);}* _temp4750;}).last_line))};( struct _xenum_struct*) _temp4737;});
break; case 243: _LL4736: yyval=({ struct Cyc_Pattern_tok_struct* _temp4752=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp4752=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(({
struct Cyc_Absyn_Int_p_struct* _temp4753=( struct Cyc_Absyn_Int_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct)); _temp4753->tag= Cyc_Absyn_Int_p_tag;
_temp4753->f1=( void*) Cyc_Absyn_Signed; _temp4753->f2= -(* Cyc_yyget_Int_tok(({
struct _tagged_ptr2 _temp4754= yyvs; struct _xenum_struct** _temp4756= _temp4754.curr
+ yyvsp_offset; if( _temp4756 < _temp4754.base? 1: _temp4756 >= _temp4754.last_plus_one){
_throw( Null_Exception);}* _temp4756;}))).f2;( void*) _temp4753;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4757= yyls; struct Cyc_Yyltype* _temp4759= _temp4757.curr
+( yylsp_offset - 1); if( _temp4759 < _temp4757.base? 1: _temp4759 >= _temp4757.last_plus_one){
_throw( Null_Exception);}* _temp4759;}).first_line,({ struct _tagged_ptr3
_temp4760= yyls; struct Cyc_Yyltype* _temp4762= _temp4760.curr + yylsp_offset;
if( _temp4762 < _temp4760.base? 1: _temp4762 >= _temp4760.last_plus_one){ _throw(
Null_Exception);}* _temp4762;}).last_line))};( struct _xenum_struct*) _temp4752;});
break; case 244: _LL4751: yyval=({ struct Cyc_Pattern_tok_struct* _temp4764=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp4764=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(({
struct Cyc_Absyn_Float_p_struct* _temp4765=( struct Cyc_Absyn_Float_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Float_p_struct)); _temp4765->tag= Cyc_Absyn_Float_p_tag;
_temp4765->f1= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp4766= yyvs;
struct _xenum_struct** _temp4768= _temp4766.curr + yyvsp_offset; if( _temp4768 <
_temp4766.base? 1: _temp4768 >= _temp4766.last_plus_one){ _throw( Null_Exception);}*
_temp4768;}));( void*) _temp4765;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4769= yyls; struct Cyc_Yyltype* _temp4771= _temp4769.curr +
yylsp_offset; if( _temp4771 < _temp4769.base? 1: _temp4771 >= _temp4769.last_plus_one){
_throw( Null_Exception);}* _temp4771;}).first_line,({ struct _tagged_ptr3
_temp4772= yyls; struct Cyc_Yyltype* _temp4774= _temp4772.curr + yylsp_offset;
if( _temp4774 < _temp4772.base? 1: _temp4774 >= _temp4772.last_plus_one){ _throw(
Null_Exception);}* _temp4774;}).last_line))};( struct _xenum_struct*) _temp4764;});
break; case 245: _LL4763: yyval=({ struct Cyc_Pattern_tok_struct* _temp4776=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp4776=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(({
struct Cyc_Absyn_Char_p_struct* _temp4777=( struct Cyc_Absyn_Char_p_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Absyn_Char_p_struct)); _temp4777->tag= Cyc_Absyn_Char_p_tag;
_temp4777->f1= Cyc_yyget_Char_tok(({ struct _tagged_ptr2 _temp4778= yyvs; struct
_xenum_struct** _temp4780= _temp4778.curr + yyvsp_offset; if( _temp4780 <
_temp4778.base? 1: _temp4780 >= _temp4778.last_plus_one){ _throw( Null_Exception);}*
_temp4780;}));( void*) _temp4777;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4781= yyls; struct Cyc_Yyltype* _temp4783= _temp4781.curr +
yylsp_offset; if( _temp4783 < _temp4781.base? 1: _temp4783 >= _temp4781.last_plus_one){
_throw( Null_Exception);}* _temp4783;}).first_line,({ struct _tagged_ptr3
_temp4784= yyls; struct Cyc_Yyltype* _temp4786= _temp4784.curr + yylsp_offset;
if( _temp4786 < _temp4784.base? 1: _temp4786 >= _temp4784.last_plus_one){ _throw(
Null_Exception);}* _temp4786;}).last_line))};( struct _xenum_struct*) _temp4776;});
break; case 246: _LL4775: yyval=({ struct Cyc_Pattern_tok_struct* _temp4788=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp4788=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(
Cyc_Absyn_Null_p, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4789=
yyls; struct Cyc_Yyltype* _temp4791= _temp4789.curr + yylsp_offset; if(
_temp4791 < _temp4789.base? 1: _temp4791 >= _temp4789.last_plus_one){ _throw(
Null_Exception);}* _temp4791;}).first_line,({ struct _tagged_ptr3 _temp4792=
yyls; struct Cyc_Yyltype* _temp4794= _temp4792.curr + yylsp_offset; if(
_temp4794 < _temp4792.base? 1: _temp4794 >= _temp4792.last_plus_one){ _throw(
Null_Exception);}* _temp4794;}).last_line))};( struct _xenum_struct*) _temp4788;});
break; case 247: _LL4787: yyval=({ struct Cyc_Pattern_tok_struct* _temp4796=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp4796=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(({
struct Cyc_Absyn_UnknownId_p_struct* _temp4797=( struct Cyc_Absyn_UnknownId_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_p_struct)); _temp4797->tag= Cyc_Absyn_UnknownId_p_tag;
_temp4797->f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp4798= yyvs;
struct _xenum_struct** _temp4800= _temp4798.curr + yyvsp_offset; if( _temp4800 <
_temp4798.base? 1: _temp4800 >= _temp4798.last_plus_one){ _throw( Null_Exception);}*
_temp4800;}));( void*) _temp4797;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4801= yyls; struct Cyc_Yyltype* _temp4803= _temp4801.curr +
yylsp_offset; if( _temp4803 < _temp4801.base? 1: _temp4803 >= _temp4801.last_plus_one){
_throw( Null_Exception);}* _temp4803;}).first_line,({ struct _tagged_ptr3
_temp4804= yyls; struct Cyc_Yyltype* _temp4806= _temp4804.curr + yylsp_offset;
if( _temp4806 < _temp4804.base? 1: _temp4806 >= _temp4804.last_plus_one){ _throw(
Null_Exception);}* _temp4806;}).last_line))};( struct _xenum_struct*) _temp4796;});
break; case 248: _LL4795: { struct Cyc_List_List* tvs=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4808= yyls; struct Cyc_Yyltype* _temp4810= _temp4808.curr
+( yylsp_offset - 3); if( _temp4810 < _temp4808.base? 1: _temp4810 >= _temp4808.last_plus_one){
_throw( Null_Exception);}* _temp4810;}).first_line,({ struct _tagged_ptr3
_temp4811= yyls; struct Cyc_Yyltype* _temp4813= _temp4811.curr +( yylsp_offset -
3); if( _temp4813 < _temp4811.base? 1: _temp4813 >= _temp4811.last_plus_one){
_throw( Null_Exception);}* _temp4813;}).last_line), Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4814= yyvs; struct _xenum_struct** _temp4816= _temp4814.curr
+( yyvsp_offset - 3); if( _temp4816 < _temp4814.base? 1: _temp4816 >= _temp4814.last_plus_one){
_throw( Null_Exception);}* _temp4816;}))); yyval=({ struct Cyc_Pattern_tok_struct*
_temp4817=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp4817=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(({
struct Cyc_Absyn_UnknownCall_p_struct* _temp4818=( struct Cyc_Absyn_UnknownCall_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownCall_p_struct)); _temp4818->tag= Cyc_Absyn_UnknownCall_p_tag;
_temp4818->f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp4819= yyvs;
struct _xenum_struct** _temp4821= _temp4819.curr +( yyvsp_offset - 4); if(
_temp4821 < _temp4819.base? 1: _temp4821 >= _temp4819.last_plus_one){ _throw(
Null_Exception);}* _temp4821;})); _temp4818->f2= tvs; _temp4818->f3= Cyc_yyget_PatternList_tok(({
struct _tagged_ptr2 _temp4822= yyvs; struct _xenum_struct** _temp4824= _temp4822.curr
+( yyvsp_offset - 1); if( _temp4824 < _temp4822.base? 1: _temp4824 >= _temp4822.last_plus_one){
_throw( Null_Exception);}* _temp4824;}));( void*) _temp4818;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4825= yyls; struct Cyc_Yyltype* _temp4827= _temp4825.curr
+( yylsp_offset - 4); if( _temp4827 < _temp4825.base? 1: _temp4827 >= _temp4825.last_plus_one){
_throw( Null_Exception);}* _temp4827;}).first_line,({ struct _tagged_ptr3
_temp4828= yyls; struct Cyc_Yyltype* _temp4830= _temp4828.curr + yylsp_offset;
if( _temp4830 < _temp4828.base? 1: _temp4830 >= _temp4828.last_plus_one){ _throw(
Null_Exception);}* _temp4830;}).last_line))};( struct _xenum_struct*) _temp4817;});
break;} case 249: _LL4807: yyval=({ struct Cyc_Pattern_tok_struct* _temp4832=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp4832=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(({
struct Cyc_Absyn_Tuple_p_struct* _temp4833=( struct Cyc_Absyn_Tuple_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuple_p_struct)); _temp4833->tag= Cyc_Absyn_Tuple_p_tag;
_temp4833->f1= Cyc_yyget_PatternList_tok(({ struct _tagged_ptr2 _temp4834= yyvs;
struct _xenum_struct** _temp4836= _temp4834.curr +( yyvsp_offset - 1); if(
_temp4836 < _temp4834.base? 1: _temp4836 >= _temp4834.last_plus_one){ _throw(
Null_Exception);}* _temp4836;}));( void*) _temp4833;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4837= yyls; struct Cyc_Yyltype* _temp4839= _temp4837.curr
+( yylsp_offset - 3); if( _temp4839 < _temp4837.base? 1: _temp4839 >= _temp4837.last_plus_one){
_throw( Null_Exception);}* _temp4839;}).first_line,({ struct _tagged_ptr3
_temp4840= yyls; struct Cyc_Yyltype* _temp4842= _temp4840.curr + yylsp_offset;
if( _temp4842 < _temp4840.base? 1: _temp4842 >= _temp4840.last_plus_one){ _throw(
Null_Exception);}* _temp4842;}).last_line))};( struct _xenum_struct*) _temp4832;});
break; case 250: _LL4831: { struct Cyc_List_List* tvs=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4844= yyls; struct Cyc_Yyltype* _temp4846= _temp4844.curr
+( yylsp_offset - 2); if( _temp4846 < _temp4844.base? 1: _temp4846 >= _temp4844.last_plus_one){
_throw( Null_Exception);}* _temp4846;}).first_line,({ struct _tagged_ptr3
_temp4847= yyls; struct Cyc_Yyltype* _temp4849= _temp4847.curr +( yylsp_offset -
2); if( _temp4849 < _temp4847.base? 1: _temp4849 >= _temp4847.last_plus_one){
_throw( Null_Exception);}* _temp4849;}).last_line), Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4850= yyvs; struct _xenum_struct** _temp4852= _temp4850.curr
+( yyvsp_offset - 2); if( _temp4852 < _temp4850.base? 1: _temp4852 >= _temp4850.last_plus_one){
_throw( Null_Exception);}* _temp4852;}))); yyval=({ struct Cyc_Pattern_tok_struct*
_temp4853=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp4853=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(({
struct Cyc_Absyn_UnknownFields_p_struct* _temp4854=( struct Cyc_Absyn_UnknownFields_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownFields_p_struct)); _temp4854->tag=
Cyc_Absyn_UnknownFields_p_tag; _temp4854->f1= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp4855= yyvs; struct _xenum_struct** _temp4857= _temp4855.curr +(
yyvsp_offset - 3); if( _temp4857 < _temp4855.base? 1: _temp4857 >= _temp4855.last_plus_one){
_throw( Null_Exception);}* _temp4857;})); _temp4854->f2= tvs; _temp4854->f3= 0;(
void*) _temp4854;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4858= yyls; struct Cyc_Yyltype* _temp4860= _temp4858.curr +( yylsp_offset -
3); if( _temp4860 < _temp4858.base? 1: _temp4860 >= _temp4858.last_plus_one){
_throw( Null_Exception);}* _temp4860;}).first_line,({ struct _tagged_ptr3
_temp4861= yyls; struct Cyc_Yyltype* _temp4863= _temp4861.curr + yylsp_offset;
if( _temp4863 < _temp4861.base? 1: _temp4863 >= _temp4861.last_plus_one){ _throw(
Null_Exception);}* _temp4863;}).last_line))};( struct _xenum_struct*) _temp4853;});
break;} case 251: _LL4843: { struct Cyc_List_List* tvs=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4865= yyls; struct Cyc_Yyltype* _temp4867= _temp4865.curr
+( yylsp_offset - 3); if( _temp4867 < _temp4865.base? 1: _temp4867 >= _temp4865.last_plus_one){
_throw( Null_Exception);}* _temp4867;}).first_line,({ struct _tagged_ptr3
_temp4868= yyls; struct Cyc_Yyltype* _temp4870= _temp4868.curr +( yylsp_offset -
3); if( _temp4870 < _temp4868.base? 1: _temp4870 >= _temp4868.last_plus_one){
_throw( Null_Exception);}* _temp4870;}).last_line), Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4871= yyvs; struct _xenum_struct** _temp4873= _temp4871.curr
+( yyvsp_offset - 3); if( _temp4873 < _temp4871.base? 1: _temp4873 >= _temp4871.last_plus_one){
_throw( Null_Exception);}* _temp4873;}))); yyval=({ struct Cyc_Pattern_tok_struct*
_temp4874=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp4874=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(({
struct Cyc_Absyn_UnknownFields_p_struct* _temp4875=( struct Cyc_Absyn_UnknownFields_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownFields_p_struct)); _temp4875->tag=
Cyc_Absyn_UnknownFields_p_tag; _temp4875->f1= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp4876= yyvs; struct _xenum_struct** _temp4878= _temp4876.curr +(
yyvsp_offset - 4); if( _temp4878 < _temp4876.base? 1: _temp4878 >= _temp4876.last_plus_one){
_throw( Null_Exception);}* _temp4878;})); _temp4875->f2= tvs; _temp4875->f3= Cyc_yyget_FieldPatternList_tok(({
struct _tagged_ptr2 _temp4879= yyvs; struct _xenum_struct** _temp4881= _temp4879.curr
+( yyvsp_offset - 1); if( _temp4881 < _temp4879.base? 1: _temp4881 >= _temp4879.last_plus_one){
_throw( Null_Exception);}* _temp4881;}));( void*) _temp4875;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4882= yyls; struct Cyc_Yyltype* _temp4884= _temp4882.curr
+( yylsp_offset - 4); if( _temp4884 < _temp4882.base? 1: _temp4884 >= _temp4882.last_plus_one){
_throw( Null_Exception);}* _temp4884;}).first_line,({ struct _tagged_ptr3
_temp4885= yyls; struct Cyc_Yyltype* _temp4887= _temp4885.curr + yylsp_offset;
if( _temp4887 < _temp4885.base? 1: _temp4887 >= _temp4885.last_plus_one){ _throw(
Null_Exception);}* _temp4887;}).last_line))};( struct _xenum_struct*) _temp4874;});
break;} case 252: _LL4864: yyval=({ struct Cyc_Pattern_tok_struct* _temp4889=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp4889=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(({
struct Cyc_Absyn_Pointer_p_struct* _temp4890=( struct Cyc_Absyn_Pointer_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_p_struct)); _temp4890->tag= Cyc_Absyn_Pointer_p_tag;
_temp4890->f1= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp4891= yyvs;
struct _xenum_struct** _temp4893= _temp4891.curr + yyvsp_offset; if( _temp4893 <
_temp4891.base? 1: _temp4893 >= _temp4891.last_plus_one){ _throw( Null_Exception);}*
_temp4893;}));( void*) _temp4890;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4894= yyls; struct Cyc_Yyltype* _temp4896= _temp4894.curr +(
yylsp_offset - 1); if( _temp4896 < _temp4894.base? 1: _temp4896 >= _temp4894.last_plus_one){
_throw( Null_Exception);}* _temp4896;}).first_line,({ struct _tagged_ptr3
_temp4897= yyls; struct Cyc_Yyltype* _temp4899= _temp4897.curr + yylsp_offset;
if( _temp4899 < _temp4897.base? 1: _temp4899 >= _temp4897.last_plus_one){ _throw(
Null_Exception);}* _temp4899;}).last_line))};( struct _xenum_struct*) _temp4889;});
break; case 253: _LL4888: yyval=({ struct Cyc_Pattern_tok_struct* _temp4901=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp4901=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(({
struct Cyc_Absyn_Reference_p_struct* _temp4902=( struct Cyc_Absyn_Reference_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Reference_p_struct)); _temp4902->tag= Cyc_Absyn_Reference_p_tag;
_temp4902->f1= Cyc_Absyn_new_vardecl(({ struct _tuple1* _temp4903=( struct
_tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp4903->f1= Cyc_Absyn_Loc_n;
_temp4903->f2=({ struct _tagged_string* _temp4904=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp4904[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp4905= yyvs; struct _xenum_struct** _temp4907= _temp4905.curr
+ yyvsp_offset; if( _temp4907 < _temp4905.base? 1: _temp4907 >= _temp4905.last_plus_one){
_throw( Null_Exception);}* _temp4907;})); _temp4904;}); _temp4903;}), Cyc_Absyn_VoidType,
0);( void*) _temp4902;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4908= yyls; struct Cyc_Yyltype* _temp4910= _temp4908.curr +( yylsp_offset -
1); if( _temp4910 < _temp4908.base? 1: _temp4910 >= _temp4908.last_plus_one){
_throw( Null_Exception);}* _temp4910;}).first_line,({ struct _tagged_ptr3
_temp4911= yyls; struct Cyc_Yyltype* _temp4913= _temp4911.curr + yylsp_offset;
if( _temp4913 < _temp4911.base? 1: _temp4913 >= _temp4911.last_plus_one){ _throw(
Null_Exception);}* _temp4913;}).last_line))};( struct _xenum_struct*) _temp4901;});
break; case 254: _LL4900: yyval=({ struct Cyc_PatternList_tok_struct* _temp4915=(
struct Cyc_PatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));*
_temp4915=( struct Cyc_PatternList_tok_struct){.tag= Cyc_PatternList_tok_tag,.f1=
0};( struct _xenum_struct*) _temp4915;}); break; case 255: _LL4914: yyval=({
struct Cyc_PatternList_tok_struct* _temp4917=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));* _temp4917=( struct Cyc_PatternList_tok_struct){.tag=
Cyc_PatternList_tok_tag,.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( Cyc_yyget_PatternList_tok(({ struct _tagged_ptr2 _temp4918=
yyvs; struct _xenum_struct** _temp4920= _temp4918.curr + yyvsp_offset; if(
_temp4920 < _temp4918.base? 1: _temp4920 >= _temp4918.last_plus_one){ _throw(
Null_Exception);}* _temp4920;})))};( struct _xenum_struct*) _temp4917;}); break;
case 256: _LL4916: yyval=({ struct Cyc_PatternList_tok_struct* _temp4922=(
struct Cyc_PatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));*
_temp4922=( struct Cyc_PatternList_tok_struct){.tag= Cyc_PatternList_tok_tag,.f1=({
struct Cyc_List_List* _temp4923=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4923->hd=( void*) Cyc_yyget_Pattern_tok(({ struct
_tagged_ptr2 _temp4924= yyvs; struct _xenum_struct** _temp4926= _temp4924.curr +
yyvsp_offset; if( _temp4926 < _temp4924.base? 1: _temp4926 >= _temp4924.last_plus_one){
_throw( Null_Exception);}* _temp4926;})); _temp4923->tl= 0; _temp4923;})};(
struct _xenum_struct*) _temp4922;}); break; case 257: _LL4921: yyval=({ struct
Cyc_PatternList_tok_struct* _temp4928=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));* _temp4928=( struct Cyc_PatternList_tok_struct){.tag=
Cyc_PatternList_tok_tag,.f1=({ struct Cyc_List_List* _temp4929=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4929->hd=( void*) Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp4933= yyvs; struct _xenum_struct** _temp4935= _temp4933.curr
+ yyvsp_offset; if( _temp4935 < _temp4933.base? 1: _temp4935 >= _temp4933.last_plus_one){
_throw( Null_Exception);}* _temp4935;})); _temp4929->tl= Cyc_yyget_PatternList_tok(({
struct _tagged_ptr2 _temp4930= yyvs; struct _xenum_struct** _temp4932= _temp4930.curr
+( yyvsp_offset - 2); if( _temp4932 < _temp4930.base? 1: _temp4932 >= _temp4930.last_plus_one){
_throw( Null_Exception);}* _temp4932;})); _temp4929;})};( struct _xenum_struct*)
_temp4928;}); break; case 258: _LL4927: yyval=({ struct Cyc_FieldPattern_tok_struct*
_temp4937=( struct Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));*
_temp4937=( struct Cyc_FieldPattern_tok_struct){.tag= Cyc_FieldPattern_tok_tag,.f1=({
struct _tuple15* _temp4938=( struct _tuple15*) GC_malloc( sizeof( struct
_tuple15)); _temp4938->f1= 0; _temp4938->f2= Cyc_yyget_Pattern_tok(({ struct
_tagged_ptr2 _temp4939= yyvs; struct _xenum_struct** _temp4941= _temp4939.curr +
yyvsp_offset; if( _temp4941 < _temp4939.base? 1: _temp4941 >= _temp4939.last_plus_one){
_throw( Null_Exception);}* _temp4941;})); _temp4938;})};( struct _xenum_struct*)
_temp4937;}); break; case 259: _LL4936: yyval=({ struct Cyc_FieldPattern_tok_struct*
_temp4943=( struct Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));*
_temp4943=( struct Cyc_FieldPattern_tok_struct){.tag= Cyc_FieldPattern_tok_tag,.f1=({
struct _tuple15* _temp4944=( struct _tuple15*) GC_malloc( sizeof( struct
_tuple15)); _temp4944->f1= Cyc_yyget_DesignatorList_tok(({ struct _tagged_ptr2
_temp4948= yyvs; struct _xenum_struct** _temp4950= _temp4948.curr +(
yyvsp_offset - 1); if( _temp4950 < _temp4948.base? 1: _temp4950 >= _temp4948.last_plus_one){
_throw( Null_Exception);}* _temp4950;})); _temp4944->f2= Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp4945= yyvs; struct _xenum_struct** _temp4947= _temp4945.curr
+ yyvsp_offset; if( _temp4947 < _temp4945.base? 1: _temp4947 >= _temp4945.last_plus_one){
_throw( Null_Exception);}* _temp4947;})); _temp4944;})};( struct _xenum_struct*)
_temp4943;}); break; case 260: _LL4942: yyval=({ struct Cyc_FieldPatternList_tok_struct*
_temp4952=( struct Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct
Cyc_FieldPatternList_tok_struct));* _temp4952=( struct Cyc_FieldPatternList_tok_struct){.tag=
Cyc_FieldPatternList_tok_tag,.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_FieldPatternList_tok(({ struct _tagged_ptr2
_temp4953= yyvs; struct _xenum_struct** _temp4955= _temp4953.curr + yyvsp_offset;
if( _temp4955 < _temp4953.base? 1: _temp4955 >= _temp4953.last_plus_one){ _throw(
Null_Exception);}* _temp4955;})))};( struct _xenum_struct*) _temp4952;}); break;
case 261: _LL4951: yyval=({ struct Cyc_FieldPatternList_tok_struct* _temp4957=(
struct Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct));*
_temp4957=( struct Cyc_FieldPatternList_tok_struct){.tag= Cyc_FieldPatternList_tok_tag,.f1=({
struct Cyc_List_List* _temp4958=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4958->hd=( void*) Cyc_yyget_FieldPattern_tok(({
struct _tagged_ptr2 _temp4959= yyvs; struct _xenum_struct** _temp4961= _temp4959.curr
+ yyvsp_offset; if( _temp4961 < _temp4959.base? 1: _temp4961 >= _temp4959.last_plus_one){
_throw( Null_Exception);}* _temp4961;})); _temp4958->tl= 0; _temp4958;})};(
struct _xenum_struct*) _temp4957;}); break; case 262: _LL4956: yyval=({ struct
Cyc_FieldPatternList_tok_struct* _temp4963=( struct Cyc_FieldPatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct));* _temp4963=( struct
Cyc_FieldPatternList_tok_struct){.tag= Cyc_FieldPatternList_tok_tag,.f1=({
struct Cyc_List_List* _temp4964=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4964->hd=( void*) Cyc_yyget_FieldPattern_tok(({
struct _tagged_ptr2 _temp4968= yyvs; struct _xenum_struct** _temp4970= _temp4968.curr
+ yyvsp_offset; if( _temp4970 < _temp4968.base? 1: _temp4970 >= _temp4968.last_plus_one){
_throw( Null_Exception);}* _temp4970;})); _temp4964->tl= Cyc_yyget_FieldPatternList_tok(({
struct _tagged_ptr2 _temp4965= yyvs; struct _xenum_struct** _temp4967= _temp4965.curr
+( yyvsp_offset - 2); if( _temp4967 < _temp4965.base? 1: _temp4967 >= _temp4965.last_plus_one){
_throw( Null_Exception);}* _temp4967;})); _temp4964;})};( struct _xenum_struct*)
_temp4963;}); break; case 263: _LL4962: yyval=({ struct _tagged_ptr2 _temp4972=
yyvs; struct _xenum_struct** _temp4974= _temp4972.curr + yyvsp_offset; if(
_temp4974 < _temp4972.base? 1: _temp4974 >= _temp4972.last_plus_one){ _throw(
Null_Exception);}* _temp4974;}); break; case 264: _LL4971: yyval=({ struct Cyc_Exp_tok_struct*
_temp4976=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp4976=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_seq_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4977= yyvs; struct _xenum_struct**
_temp4979= _temp4977.curr +( yyvsp_offset - 2); if( _temp4979 < _temp4977.base?
1: _temp4979 >= _temp4977.last_plus_one){ _throw( Null_Exception);}* _temp4979;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4980= yyvs; struct _xenum_struct**
_temp4982= _temp4980.curr + yyvsp_offset; if( _temp4982 < _temp4980.base? 1:
_temp4982 >= _temp4980.last_plus_one){ _throw( Null_Exception);}* _temp4982;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4983= yyls; struct Cyc_Yyltype*
_temp4985= _temp4983.curr +( yylsp_offset - 2); if( _temp4985 < _temp4983.base?
1: _temp4985 >= _temp4983.last_plus_one){ _throw( Null_Exception);}* _temp4985;}).first_line,({
struct _tagged_ptr3 _temp4986= yyls; struct Cyc_Yyltype* _temp4988= _temp4986.curr
+ yylsp_offset; if( _temp4988 < _temp4986.base? 1: _temp4988 >= _temp4986.last_plus_one){
_throw( Null_Exception);}* _temp4988;}).last_line))};( struct _xenum_struct*)
_temp4976;}); break; case 265: _LL4975: yyval=({ struct _tagged_ptr2 _temp4990=
yyvs; struct _xenum_struct** _temp4992= _temp4990.curr + yyvsp_offset; if(
_temp4992 < _temp4990.base? 1: _temp4992 >= _temp4990.last_plus_one){ _throw(
Null_Exception);}* _temp4992;}); break; case 266: _LL4989: yyval=({ struct Cyc_Exp_tok_struct*
_temp4994=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp4994=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_assignop_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4995= yyvs; struct _xenum_struct**
_temp4997= _temp4995.curr +( yyvsp_offset - 2); if( _temp4997 < _temp4995.base?
1: _temp4997 >= _temp4995.last_plus_one){ _throw( Null_Exception);}* _temp4997;})),
Cyc_yyget_Primopopt_tok(({ struct _tagged_ptr2 _temp4998= yyvs; struct
_xenum_struct** _temp5000= _temp4998.curr +( yyvsp_offset - 1); if( _temp5000 <
_temp4998.base? 1: _temp5000 >= _temp4998.last_plus_one){ _throw( Null_Exception);}*
_temp5000;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5001= yyvs; struct
_xenum_struct** _temp5003= _temp5001.curr + yyvsp_offset; if( _temp5003 <
_temp5001.base? 1: _temp5003 >= _temp5001.last_plus_one){ _throw( Null_Exception);}*
_temp5003;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5004=
yyls; struct Cyc_Yyltype* _temp5006= _temp5004.curr +( yylsp_offset - 2); if(
_temp5006 < _temp5004.base? 1: _temp5006 >= _temp5004.last_plus_one){ _throw(
Null_Exception);}* _temp5006;}).first_line,({ struct _tagged_ptr3 _temp5007=
yyls; struct Cyc_Yyltype* _temp5009= _temp5007.curr + yylsp_offset; if(
_temp5009 < _temp5007.base? 1: _temp5009 >= _temp5007.last_plus_one){ _throw(
Null_Exception);}* _temp5009;}).last_line))};( struct _xenum_struct*) _temp4994;});
break; case 267: _LL4993: yyval=({ struct Cyc_Primopopt_tok_struct* _temp5011=(
struct Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));*
_temp5011=( struct Cyc_Primopopt_tok_struct){.tag= Cyc_Primopopt_tok_tag,.f1= 0};(
struct _xenum_struct*) _temp5011;}); break; case 268: _LL5010: yyval=({ struct
Cyc_Primopopt_tok_struct* _temp5013=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));* _temp5013=( struct Cyc_Primopopt_tok_struct){.tag=
Cyc_Primopopt_tok_tag,.f1=({ struct Cyc_Core_Opt* _temp5014=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp5014->v=( void*) Cyc_Absyn_Times;
_temp5014;})};( struct _xenum_struct*) _temp5013;}); break; case 269: _LL5012:
yyval=({ struct Cyc_Primopopt_tok_struct* _temp5016=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));* _temp5016=( struct Cyc_Primopopt_tok_struct){.tag=
Cyc_Primopopt_tok_tag,.f1=({ struct Cyc_Core_Opt* _temp5017=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp5017->v=( void*) Cyc_Absyn_Div;
_temp5017;})};( struct _xenum_struct*) _temp5016;}); break; case 270: _LL5015:
yyval=({ struct Cyc_Primopopt_tok_struct* _temp5019=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));* _temp5019=( struct Cyc_Primopopt_tok_struct){.tag=
Cyc_Primopopt_tok_tag,.f1=({ struct Cyc_Core_Opt* _temp5020=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp5020->v=( void*) Cyc_Absyn_Mod;
_temp5020;})};( struct _xenum_struct*) _temp5019;}); break; case 271: _LL5018:
yyval=({ struct Cyc_Primopopt_tok_struct* _temp5022=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));* _temp5022=( struct Cyc_Primopopt_tok_struct){.tag=
Cyc_Primopopt_tok_tag,.f1=({ struct Cyc_Core_Opt* _temp5023=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp5023->v=( void*) Cyc_Absyn_Plus;
_temp5023;})};( struct _xenum_struct*) _temp5022;}); break; case 272: _LL5021:
yyval=({ struct Cyc_Primopopt_tok_struct* _temp5025=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));* _temp5025=( struct Cyc_Primopopt_tok_struct){.tag=
Cyc_Primopopt_tok_tag,.f1=({ struct Cyc_Core_Opt* _temp5026=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp5026->v=( void*) Cyc_Absyn_Minus;
_temp5026;})};( struct _xenum_struct*) _temp5025;}); break; case 273: _LL5024:
yyval=({ struct Cyc_Primopopt_tok_struct* _temp5028=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));* _temp5028=( struct Cyc_Primopopt_tok_struct){.tag=
Cyc_Primopopt_tok_tag,.f1=({ struct Cyc_Core_Opt* _temp5029=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp5029->v=( void*) Cyc_Absyn_Bitlshift;
_temp5029;})};( struct _xenum_struct*) _temp5028;}); break; case 274: _LL5027:
yyval=({ struct Cyc_Primopopt_tok_struct* _temp5031=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));* _temp5031=( struct Cyc_Primopopt_tok_struct){.tag=
Cyc_Primopopt_tok_tag,.f1=({ struct Cyc_Core_Opt* _temp5032=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp5032->v=( void*) Cyc_Absyn_Bitlrshift;
_temp5032;})};( struct _xenum_struct*) _temp5031;}); break; case 275: _LL5030:
yyval=({ struct Cyc_Primopopt_tok_struct* _temp5034=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));* _temp5034=( struct Cyc_Primopopt_tok_struct){.tag=
Cyc_Primopopt_tok_tag,.f1=({ struct Cyc_Core_Opt* _temp5035=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp5035->v=( void*) Cyc_Absyn_Bitand;
_temp5035;})};( struct _xenum_struct*) _temp5034;}); break; case 276: _LL5033:
yyval=({ struct Cyc_Primopopt_tok_struct* _temp5037=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));* _temp5037=( struct Cyc_Primopopt_tok_struct){.tag=
Cyc_Primopopt_tok_tag,.f1=({ struct Cyc_Core_Opt* _temp5038=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp5038->v=( void*) Cyc_Absyn_Bitxor;
_temp5038;})};( struct _xenum_struct*) _temp5037;}); break; case 277: _LL5036:
yyval=({ struct Cyc_Primopopt_tok_struct* _temp5040=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));* _temp5040=( struct Cyc_Primopopt_tok_struct){.tag=
Cyc_Primopopt_tok_tag,.f1=({ struct Cyc_Core_Opt* _temp5041=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp5041->v=( void*) Cyc_Absyn_Bitor;
_temp5041;})};( struct _xenum_struct*) _temp5040;}); break; case 278: _LL5039:
yyval=({ struct _tagged_ptr2 _temp5043= yyvs; struct _xenum_struct** _temp5045=
_temp5043.curr + yyvsp_offset; if( _temp5045 < _temp5043.base? 1: _temp5045 >=
_temp5043.last_plus_one){ _throw( Null_Exception);}* _temp5045;}); break; case
279: _LL5042: yyval=({ struct Cyc_Exp_tok_struct* _temp5047=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp5047=( struct Cyc_Exp_tok_struct){.tag=
Cyc_Exp_tok_tag,.f1= Cyc_Absyn_conditional_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5048= yyvs; struct _xenum_struct** _temp5050= _temp5048.curr +(
yyvsp_offset - 4); if( _temp5050 < _temp5048.base? 1: _temp5050 >= _temp5048.last_plus_one){
_throw( Null_Exception);}* _temp5050;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5051= yyvs; struct _xenum_struct** _temp5053= _temp5051.curr +(
yyvsp_offset - 2); if( _temp5053 < _temp5051.base? 1: _temp5053 >= _temp5051.last_plus_one){
_throw( Null_Exception);}* _temp5053;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5054= yyvs; struct _xenum_struct** _temp5056= _temp5054.curr +
yyvsp_offset; if( _temp5056 < _temp5054.base? 1: _temp5056 >= _temp5054.last_plus_one){
_throw( Null_Exception);}* _temp5056;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5057= yyls; struct Cyc_Yyltype* _temp5059= _temp5057.curr +(
yylsp_offset - 4); if( _temp5059 < _temp5057.base? 1: _temp5059 >= _temp5057.last_plus_one){
_throw( Null_Exception);}* _temp5059;}).first_line,({ struct _tagged_ptr3
_temp5060= yyls; struct Cyc_Yyltype* _temp5062= _temp5060.curr + yylsp_offset;
if( _temp5062 < _temp5060.base? 1: _temp5062 >= _temp5060.last_plus_one){ _throw(
Null_Exception);}* _temp5062;}).last_line))};( struct _xenum_struct*) _temp5047;});
break; case 280: _LL5046: yyval=({ struct _tagged_ptr2 _temp5064= yyvs; struct
_xenum_struct** _temp5066= _temp5064.curr + yyvsp_offset; if( _temp5066 <
_temp5064.base? 1: _temp5066 >= _temp5064.last_plus_one){ _throw( Null_Exception);}*
_temp5066;}); break; case 281: _LL5063: yyval=({ struct _tagged_ptr2 _temp5068=
yyvs; struct _xenum_struct** _temp5070= _temp5068.curr + yyvsp_offset; if(
_temp5070 < _temp5068.base? 1: _temp5070 >= _temp5068.last_plus_one){ _throw(
Null_Exception);}* _temp5070;}); break; case 282: _LL5067: yyval=({ struct Cyc_Exp_tok_struct*
_temp5072=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5072=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_or_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5073= yyvs; struct _xenum_struct**
_temp5075= _temp5073.curr +( yyvsp_offset - 2); if( _temp5075 < _temp5073.base?
1: _temp5075 >= _temp5073.last_plus_one){ _throw( Null_Exception);}* _temp5075;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5076= yyvs; struct _xenum_struct**
_temp5078= _temp5076.curr + yyvsp_offset; if( _temp5078 < _temp5076.base? 1:
_temp5078 >= _temp5076.last_plus_one){ _throw( Null_Exception);}* _temp5078;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5079= yyls; struct Cyc_Yyltype*
_temp5081= _temp5079.curr +( yylsp_offset - 2); if( _temp5081 < _temp5079.base?
1: _temp5081 >= _temp5079.last_plus_one){ _throw( Null_Exception);}* _temp5081;}).first_line,({
struct _tagged_ptr3 _temp5082= yyls; struct Cyc_Yyltype* _temp5084= _temp5082.curr
+ yylsp_offset; if( _temp5084 < _temp5082.base? 1: _temp5084 >= _temp5082.last_plus_one){
_throw( Null_Exception);}* _temp5084;}).last_line))};( struct _xenum_struct*)
_temp5072;}); break; case 283: _LL5071: yyval=({ struct _tagged_ptr2 _temp5086=
yyvs; struct _xenum_struct** _temp5088= _temp5086.curr + yyvsp_offset; if(
_temp5088 < _temp5086.base? 1: _temp5088 >= _temp5086.last_plus_one){ _throw(
Null_Exception);}* _temp5088;}); break; case 284: _LL5085: yyval=({ struct Cyc_Exp_tok_struct*
_temp5090=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5090=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_and_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5091= yyvs; struct _xenum_struct**
_temp5093= _temp5091.curr +( yyvsp_offset - 2); if( _temp5093 < _temp5091.base?
1: _temp5093 >= _temp5091.last_plus_one){ _throw( Null_Exception);}* _temp5093;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5094= yyvs; struct _xenum_struct**
_temp5096= _temp5094.curr + yyvsp_offset; if( _temp5096 < _temp5094.base? 1:
_temp5096 >= _temp5094.last_plus_one){ _throw( Null_Exception);}* _temp5096;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5097= yyls; struct Cyc_Yyltype*
_temp5099= _temp5097.curr +( yylsp_offset - 2); if( _temp5099 < _temp5097.base?
1: _temp5099 >= _temp5097.last_plus_one){ _throw( Null_Exception);}* _temp5099;}).first_line,({
struct _tagged_ptr3 _temp5100= yyls; struct Cyc_Yyltype* _temp5102= _temp5100.curr
+ yylsp_offset; if( _temp5102 < _temp5100.base? 1: _temp5102 >= _temp5100.last_plus_one){
_throw( Null_Exception);}* _temp5102;}).last_line))};( struct _xenum_struct*)
_temp5090;}); break; case 285: _LL5089: yyval=({ struct _tagged_ptr2 _temp5104=
yyvs; struct _xenum_struct** _temp5106= _temp5104.curr + yyvsp_offset; if(
_temp5106 < _temp5104.base? 1: _temp5106 >= _temp5104.last_plus_one){ _throw(
Null_Exception);}* _temp5106;}); break; case 286: _LL5103: yyval=({ struct Cyc_Exp_tok_struct*
_temp5108=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5108=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_prim2_exp(
Cyc_Absyn_Bitor, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5109= yyvs;
struct _xenum_struct** _temp5111= _temp5109.curr +( yyvsp_offset - 2); if(
_temp5111 < _temp5109.base? 1: _temp5111 >= _temp5109.last_plus_one){ _throw(
Null_Exception);}* _temp5111;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5112= yyvs; struct _xenum_struct** _temp5114= _temp5112.curr + yyvsp_offset;
if( _temp5114 < _temp5112.base? 1: _temp5114 >= _temp5112.last_plus_one){ _throw(
Null_Exception);}* _temp5114;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5115= yyls; struct Cyc_Yyltype* _temp5117= _temp5115.curr +(
yylsp_offset - 2); if( _temp5117 < _temp5115.base? 1: _temp5117 >= _temp5115.last_plus_one){
_throw( Null_Exception);}* _temp5117;}).first_line,({ struct _tagged_ptr3
_temp5118= yyls; struct Cyc_Yyltype* _temp5120= _temp5118.curr + yylsp_offset;
if( _temp5120 < _temp5118.base? 1: _temp5120 >= _temp5118.last_plus_one){ _throw(
Null_Exception);}* _temp5120;}).last_line))};( struct _xenum_struct*) _temp5108;});
break; case 287: _LL5107: yyval=({ struct _tagged_ptr2 _temp5122= yyvs; struct
_xenum_struct** _temp5124= _temp5122.curr + yyvsp_offset; if( _temp5124 <
_temp5122.base? 1: _temp5124 >= _temp5122.last_plus_one){ _throw( Null_Exception);}*
_temp5124;}); break; case 288: _LL5121: yyval=({ struct Cyc_Exp_tok_struct*
_temp5126=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5126=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_prim2_exp(
Cyc_Absyn_Bitxor, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5127= yyvs;
struct _xenum_struct** _temp5129= _temp5127.curr +( yyvsp_offset - 2); if(
_temp5129 < _temp5127.base? 1: _temp5129 >= _temp5127.last_plus_one){ _throw(
Null_Exception);}* _temp5129;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5130= yyvs; struct _xenum_struct** _temp5132= _temp5130.curr + yyvsp_offset;
if( _temp5132 < _temp5130.base? 1: _temp5132 >= _temp5130.last_plus_one){ _throw(
Null_Exception);}* _temp5132;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5133= yyls; struct Cyc_Yyltype* _temp5135= _temp5133.curr +(
yylsp_offset - 2); if( _temp5135 < _temp5133.base? 1: _temp5135 >= _temp5133.last_plus_one){
_throw( Null_Exception);}* _temp5135;}).first_line,({ struct _tagged_ptr3
_temp5136= yyls; struct Cyc_Yyltype* _temp5138= _temp5136.curr + yylsp_offset;
if( _temp5138 < _temp5136.base? 1: _temp5138 >= _temp5136.last_plus_one){ _throw(
Null_Exception);}* _temp5138;}).last_line))};( struct _xenum_struct*) _temp5126;});
break; case 289: _LL5125: yyval=({ struct _tagged_ptr2 _temp5140= yyvs; struct
_xenum_struct** _temp5142= _temp5140.curr + yyvsp_offset; if( _temp5142 <
_temp5140.base? 1: _temp5142 >= _temp5140.last_plus_one){ _throw( Null_Exception);}*
_temp5142;}); break; case 290: _LL5139: yyval=({ struct Cyc_Exp_tok_struct*
_temp5144=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5144=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_prim2_exp(
Cyc_Absyn_Bitand, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5145= yyvs;
struct _xenum_struct** _temp5147= _temp5145.curr +( yyvsp_offset - 2); if(
_temp5147 < _temp5145.base? 1: _temp5147 >= _temp5145.last_plus_one){ _throw(
Null_Exception);}* _temp5147;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5148= yyvs; struct _xenum_struct** _temp5150= _temp5148.curr + yyvsp_offset;
if( _temp5150 < _temp5148.base? 1: _temp5150 >= _temp5148.last_plus_one){ _throw(
Null_Exception);}* _temp5150;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5151= yyls; struct Cyc_Yyltype* _temp5153= _temp5151.curr +(
yylsp_offset - 2); if( _temp5153 < _temp5151.base? 1: _temp5153 >= _temp5151.last_plus_one){
_throw( Null_Exception);}* _temp5153;}).first_line,({ struct _tagged_ptr3
_temp5154= yyls; struct Cyc_Yyltype* _temp5156= _temp5154.curr + yylsp_offset;
if( _temp5156 < _temp5154.base? 1: _temp5156 >= _temp5154.last_plus_one){ _throw(
Null_Exception);}* _temp5156;}).last_line))};( struct _xenum_struct*) _temp5144;});
break; case 291: _LL5143: yyval=({ struct _tagged_ptr2 _temp5158= yyvs; struct
_xenum_struct** _temp5160= _temp5158.curr + yyvsp_offset; if( _temp5160 <
_temp5158.base? 1: _temp5160 >= _temp5158.last_plus_one){ _throw( Null_Exception);}*
_temp5160;}); break; case 292: _LL5157: yyval=({ struct Cyc_Exp_tok_struct*
_temp5162=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5162=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_eq_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5163= yyvs; struct _xenum_struct**
_temp5165= _temp5163.curr +( yyvsp_offset - 2); if( _temp5165 < _temp5163.base?
1: _temp5165 >= _temp5163.last_plus_one){ _throw( Null_Exception);}* _temp5165;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5166= yyvs; struct _xenum_struct**
_temp5168= _temp5166.curr + yyvsp_offset; if( _temp5168 < _temp5166.base? 1:
_temp5168 >= _temp5166.last_plus_one){ _throw( Null_Exception);}* _temp5168;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5169= yyls; struct Cyc_Yyltype*
_temp5171= _temp5169.curr +( yylsp_offset - 2); if( _temp5171 < _temp5169.base?
1: _temp5171 >= _temp5169.last_plus_one){ _throw( Null_Exception);}* _temp5171;}).first_line,({
struct _tagged_ptr3 _temp5172= yyls; struct Cyc_Yyltype* _temp5174= _temp5172.curr
+ yylsp_offset; if( _temp5174 < _temp5172.base? 1: _temp5174 >= _temp5172.last_plus_one){
_throw( Null_Exception);}* _temp5174;}).last_line))};( struct _xenum_struct*)
_temp5162;}); break; case 293: _LL5161: yyval=({ struct Cyc_Exp_tok_struct*
_temp5176=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5176=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_neq_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5177= yyvs; struct _xenum_struct**
_temp5179= _temp5177.curr +( yyvsp_offset - 2); if( _temp5179 < _temp5177.base?
1: _temp5179 >= _temp5177.last_plus_one){ _throw( Null_Exception);}* _temp5179;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5180= yyvs; struct _xenum_struct**
_temp5182= _temp5180.curr + yyvsp_offset; if( _temp5182 < _temp5180.base? 1:
_temp5182 >= _temp5180.last_plus_one){ _throw( Null_Exception);}* _temp5182;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5183= yyls; struct Cyc_Yyltype*
_temp5185= _temp5183.curr +( yylsp_offset - 2); if( _temp5185 < _temp5183.base?
1: _temp5185 >= _temp5183.last_plus_one){ _throw( Null_Exception);}* _temp5185;}).first_line,({
struct _tagged_ptr3 _temp5186= yyls; struct Cyc_Yyltype* _temp5188= _temp5186.curr
+ yylsp_offset; if( _temp5188 < _temp5186.base? 1: _temp5188 >= _temp5186.last_plus_one){
_throw( Null_Exception);}* _temp5188;}).last_line))};( struct _xenum_struct*)
_temp5176;}); break; case 294: _LL5175: yyval=({ struct _tagged_ptr2 _temp5190=
yyvs; struct _xenum_struct** _temp5192= _temp5190.curr + yyvsp_offset; if(
_temp5192 < _temp5190.base? 1: _temp5192 >= _temp5190.last_plus_one){ _throw(
Null_Exception);}* _temp5192;}); break; case 295: _LL5189: yyval=({ struct Cyc_Exp_tok_struct*
_temp5194=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5194=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_lt_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5195= yyvs; struct _xenum_struct**
_temp5197= _temp5195.curr +( yyvsp_offset - 2); if( _temp5197 < _temp5195.base?
1: _temp5197 >= _temp5195.last_plus_one){ _throw( Null_Exception);}* _temp5197;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5198= yyvs; struct _xenum_struct**
_temp5200= _temp5198.curr + yyvsp_offset; if( _temp5200 < _temp5198.base? 1:
_temp5200 >= _temp5198.last_plus_one){ _throw( Null_Exception);}* _temp5200;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5201= yyls; struct Cyc_Yyltype*
_temp5203= _temp5201.curr +( yylsp_offset - 2); if( _temp5203 < _temp5201.base?
1: _temp5203 >= _temp5201.last_plus_one){ _throw( Null_Exception);}* _temp5203;}).first_line,({
struct _tagged_ptr3 _temp5204= yyls; struct Cyc_Yyltype* _temp5206= _temp5204.curr
+ yylsp_offset; if( _temp5206 < _temp5204.base? 1: _temp5206 >= _temp5204.last_plus_one){
_throw( Null_Exception);}* _temp5206;}).last_line))};( struct _xenum_struct*)
_temp5194;}); break; case 296: _LL5193: yyval=({ struct Cyc_Exp_tok_struct*
_temp5208=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5208=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_gt_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5209= yyvs; struct _xenum_struct**
_temp5211= _temp5209.curr +( yyvsp_offset - 2); if( _temp5211 < _temp5209.base?
1: _temp5211 >= _temp5209.last_plus_one){ _throw( Null_Exception);}* _temp5211;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5212= yyvs; struct _xenum_struct**
_temp5214= _temp5212.curr + yyvsp_offset; if( _temp5214 < _temp5212.base? 1:
_temp5214 >= _temp5212.last_plus_one){ _throw( Null_Exception);}* _temp5214;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5215= yyls; struct Cyc_Yyltype*
_temp5217= _temp5215.curr +( yylsp_offset - 2); if( _temp5217 < _temp5215.base?
1: _temp5217 >= _temp5215.last_plus_one){ _throw( Null_Exception);}* _temp5217;}).first_line,({
struct _tagged_ptr3 _temp5218= yyls; struct Cyc_Yyltype* _temp5220= _temp5218.curr
+ yylsp_offset; if( _temp5220 < _temp5218.base? 1: _temp5220 >= _temp5218.last_plus_one){
_throw( Null_Exception);}* _temp5220;}).last_line))};( struct _xenum_struct*)
_temp5208;}); break; case 297: _LL5207: yyval=({ struct Cyc_Exp_tok_struct*
_temp5222=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5222=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_lte_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5223= yyvs; struct _xenum_struct**
_temp5225= _temp5223.curr +( yyvsp_offset - 2); if( _temp5225 < _temp5223.base?
1: _temp5225 >= _temp5223.last_plus_one){ _throw( Null_Exception);}* _temp5225;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5226= yyvs; struct _xenum_struct**
_temp5228= _temp5226.curr + yyvsp_offset; if( _temp5228 < _temp5226.base? 1:
_temp5228 >= _temp5226.last_plus_one){ _throw( Null_Exception);}* _temp5228;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5229= yyls; struct Cyc_Yyltype*
_temp5231= _temp5229.curr +( yylsp_offset - 2); if( _temp5231 < _temp5229.base?
1: _temp5231 >= _temp5229.last_plus_one){ _throw( Null_Exception);}* _temp5231;}).first_line,({
struct _tagged_ptr3 _temp5232= yyls; struct Cyc_Yyltype* _temp5234= _temp5232.curr
+ yylsp_offset; if( _temp5234 < _temp5232.base? 1: _temp5234 >= _temp5232.last_plus_one){
_throw( Null_Exception);}* _temp5234;}).last_line))};( struct _xenum_struct*)
_temp5222;}); break; case 298: _LL5221: yyval=({ struct Cyc_Exp_tok_struct*
_temp5236=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5236=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_gte_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5237= yyvs; struct _xenum_struct**
_temp5239= _temp5237.curr +( yyvsp_offset - 2); if( _temp5239 < _temp5237.base?
1: _temp5239 >= _temp5237.last_plus_one){ _throw( Null_Exception);}* _temp5239;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5240= yyvs; struct _xenum_struct**
_temp5242= _temp5240.curr + yyvsp_offset; if( _temp5242 < _temp5240.base? 1:
_temp5242 >= _temp5240.last_plus_one){ _throw( Null_Exception);}* _temp5242;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5243= yyls; struct Cyc_Yyltype*
_temp5245= _temp5243.curr +( yylsp_offset - 2); if( _temp5245 < _temp5243.base?
1: _temp5245 >= _temp5243.last_plus_one){ _throw( Null_Exception);}* _temp5245;}).first_line,({
struct _tagged_ptr3 _temp5246= yyls; struct Cyc_Yyltype* _temp5248= _temp5246.curr
+ yylsp_offset; if( _temp5248 < _temp5246.base? 1: _temp5248 >= _temp5246.last_plus_one){
_throw( Null_Exception);}* _temp5248;}).last_line))};( struct _xenum_struct*)
_temp5236;}); break; case 299: _LL5235: yyval=({ struct _tagged_ptr2 _temp5250=
yyvs; struct _xenum_struct** _temp5252= _temp5250.curr + yyvsp_offset; if(
_temp5252 < _temp5250.base? 1: _temp5252 >= _temp5250.last_plus_one){ _throw(
Null_Exception);}* _temp5252;}); break; case 300: _LL5249: yyval=({ struct Cyc_Exp_tok_struct*
_temp5254=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5254=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_prim2_exp(
Cyc_Absyn_Bitlshift, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5255= yyvs;
struct _xenum_struct** _temp5257= _temp5255.curr +( yyvsp_offset - 2); if(
_temp5257 < _temp5255.base? 1: _temp5257 >= _temp5255.last_plus_one){ _throw(
Null_Exception);}* _temp5257;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5258= yyvs; struct _xenum_struct** _temp5260= _temp5258.curr + yyvsp_offset;
if( _temp5260 < _temp5258.base? 1: _temp5260 >= _temp5258.last_plus_one){ _throw(
Null_Exception);}* _temp5260;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5261= yyls; struct Cyc_Yyltype* _temp5263= _temp5261.curr +(
yylsp_offset - 2); if( _temp5263 < _temp5261.base? 1: _temp5263 >= _temp5261.last_plus_one){
_throw( Null_Exception);}* _temp5263;}).first_line,({ struct _tagged_ptr3
_temp5264= yyls; struct Cyc_Yyltype* _temp5266= _temp5264.curr + yylsp_offset;
if( _temp5266 < _temp5264.base? 1: _temp5266 >= _temp5264.last_plus_one){ _throw(
Null_Exception);}* _temp5266;}).last_line))};( struct _xenum_struct*) _temp5254;});
break; case 301: _LL5253: yyval=({ struct Cyc_Exp_tok_struct* _temp5268=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp5268=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_prim2_exp( Cyc_Absyn_Bitlrshift,
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5269= yyvs; struct _xenum_struct**
_temp5271= _temp5269.curr +( yyvsp_offset - 2); if( _temp5271 < _temp5269.base?
1: _temp5271 >= _temp5269.last_plus_one){ _throw( Null_Exception);}* _temp5271;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5272= yyvs; struct _xenum_struct**
_temp5274= _temp5272.curr + yyvsp_offset; if( _temp5274 < _temp5272.base? 1:
_temp5274 >= _temp5272.last_plus_one){ _throw( Null_Exception);}* _temp5274;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5275= yyls; struct Cyc_Yyltype*
_temp5277= _temp5275.curr +( yylsp_offset - 2); if( _temp5277 < _temp5275.base?
1: _temp5277 >= _temp5275.last_plus_one){ _throw( Null_Exception);}* _temp5277;}).first_line,({
struct _tagged_ptr3 _temp5278= yyls; struct Cyc_Yyltype* _temp5280= _temp5278.curr
+ yylsp_offset; if( _temp5280 < _temp5278.base? 1: _temp5280 >= _temp5278.last_plus_one){
_throw( Null_Exception);}* _temp5280;}).last_line))};( struct _xenum_struct*)
_temp5268;}); break; case 302: _LL5267: yyval=({ struct _tagged_ptr2 _temp5282=
yyvs; struct _xenum_struct** _temp5284= _temp5282.curr + yyvsp_offset; if(
_temp5284 < _temp5282.base? 1: _temp5284 >= _temp5282.last_plus_one){ _throw(
Null_Exception);}* _temp5284;}); break; case 303: _LL5281: yyval=({ struct Cyc_Exp_tok_struct*
_temp5286=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5286=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_prim2_exp(
Cyc_Absyn_Plus, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5287= yyvs; struct
_xenum_struct** _temp5289= _temp5287.curr +( yyvsp_offset - 2); if( _temp5289 <
_temp5287.base? 1: _temp5289 >= _temp5287.last_plus_one){ _throw( Null_Exception);}*
_temp5289;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5290= yyvs; struct
_xenum_struct** _temp5292= _temp5290.curr + yyvsp_offset; if( _temp5292 <
_temp5290.base? 1: _temp5292 >= _temp5290.last_plus_one){ _throw( Null_Exception);}*
_temp5292;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5293=
yyls; struct Cyc_Yyltype* _temp5295= _temp5293.curr +( yylsp_offset - 2); if(
_temp5295 < _temp5293.base? 1: _temp5295 >= _temp5293.last_plus_one){ _throw(
Null_Exception);}* _temp5295;}).first_line,({ struct _tagged_ptr3 _temp5296=
yyls; struct Cyc_Yyltype* _temp5298= _temp5296.curr + yylsp_offset; if(
_temp5298 < _temp5296.base? 1: _temp5298 >= _temp5296.last_plus_one){ _throw(
Null_Exception);}* _temp5298;}).last_line))};( struct _xenum_struct*) _temp5286;});
break; case 304: _LL5285: yyval=({ struct Cyc_Exp_tok_struct* _temp5300=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp5300=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_prim2_exp( Cyc_Absyn_Minus,
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5301= yyvs; struct _xenum_struct**
_temp5303= _temp5301.curr +( yyvsp_offset - 2); if( _temp5303 < _temp5301.base?
1: _temp5303 >= _temp5301.last_plus_one){ _throw( Null_Exception);}* _temp5303;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5304= yyvs; struct _xenum_struct**
_temp5306= _temp5304.curr + yyvsp_offset; if( _temp5306 < _temp5304.base? 1:
_temp5306 >= _temp5304.last_plus_one){ _throw( Null_Exception);}* _temp5306;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5307= yyls; struct Cyc_Yyltype*
_temp5309= _temp5307.curr +( yylsp_offset - 2); if( _temp5309 < _temp5307.base?
1: _temp5309 >= _temp5307.last_plus_one){ _throw( Null_Exception);}* _temp5309;}).first_line,({
struct _tagged_ptr3 _temp5310= yyls; struct Cyc_Yyltype* _temp5312= _temp5310.curr
+ yylsp_offset; if( _temp5312 < _temp5310.base? 1: _temp5312 >= _temp5310.last_plus_one){
_throw( Null_Exception);}* _temp5312;}).last_line))};( struct _xenum_struct*)
_temp5300;}); break; case 305: _LL5299: yyval=({ struct _tagged_ptr2 _temp5314=
yyvs; struct _xenum_struct** _temp5316= _temp5314.curr + yyvsp_offset; if(
_temp5316 < _temp5314.base? 1: _temp5316 >= _temp5314.last_plus_one){ _throw(
Null_Exception);}* _temp5316;}); break; case 306: _LL5313: yyval=({ struct Cyc_Exp_tok_struct*
_temp5318=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5318=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_prim2_exp(
Cyc_Absyn_Times, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5319= yyvs;
struct _xenum_struct** _temp5321= _temp5319.curr +( yyvsp_offset - 2); if(
_temp5321 < _temp5319.base? 1: _temp5321 >= _temp5319.last_plus_one){ _throw(
Null_Exception);}* _temp5321;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5322= yyvs; struct _xenum_struct** _temp5324= _temp5322.curr + yyvsp_offset;
if( _temp5324 < _temp5322.base? 1: _temp5324 >= _temp5322.last_plus_one){ _throw(
Null_Exception);}* _temp5324;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5325= yyls; struct Cyc_Yyltype* _temp5327= _temp5325.curr +(
yylsp_offset - 2); if( _temp5327 < _temp5325.base? 1: _temp5327 >= _temp5325.last_plus_one){
_throw( Null_Exception);}* _temp5327;}).first_line,({ struct _tagged_ptr3
_temp5328= yyls; struct Cyc_Yyltype* _temp5330= _temp5328.curr + yylsp_offset;
if( _temp5330 < _temp5328.base? 1: _temp5330 >= _temp5328.last_plus_one){ _throw(
Null_Exception);}* _temp5330;}).last_line))};( struct _xenum_struct*) _temp5318;});
break; case 307: _LL5317: yyval=({ struct Cyc_Exp_tok_struct* _temp5332=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp5332=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_prim2_exp( Cyc_Absyn_Div,
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5333= yyvs; struct _xenum_struct**
_temp5335= _temp5333.curr +( yyvsp_offset - 2); if( _temp5335 < _temp5333.base?
1: _temp5335 >= _temp5333.last_plus_one){ _throw( Null_Exception);}* _temp5335;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5336= yyvs; struct _xenum_struct**
_temp5338= _temp5336.curr + yyvsp_offset; if( _temp5338 < _temp5336.base? 1:
_temp5338 >= _temp5336.last_plus_one){ _throw( Null_Exception);}* _temp5338;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5339= yyls; struct Cyc_Yyltype*
_temp5341= _temp5339.curr +( yylsp_offset - 2); if( _temp5341 < _temp5339.base?
1: _temp5341 >= _temp5339.last_plus_one){ _throw( Null_Exception);}* _temp5341;}).first_line,({
struct _tagged_ptr3 _temp5342= yyls; struct Cyc_Yyltype* _temp5344= _temp5342.curr
+ yylsp_offset; if( _temp5344 < _temp5342.base? 1: _temp5344 >= _temp5342.last_plus_one){
_throw( Null_Exception);}* _temp5344;}).last_line))};( struct _xenum_struct*)
_temp5332;}); break; case 308: _LL5331: yyval=({ struct Cyc_Exp_tok_struct*
_temp5346=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5346=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_prim2_exp(
Cyc_Absyn_Mod, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5347= yyvs; struct
_xenum_struct** _temp5349= _temp5347.curr +( yyvsp_offset - 2); if( _temp5349 <
_temp5347.base? 1: _temp5349 >= _temp5347.last_plus_one){ _throw( Null_Exception);}*
_temp5349;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5350= yyvs; struct
_xenum_struct** _temp5352= _temp5350.curr + yyvsp_offset; if( _temp5352 <
_temp5350.base? 1: _temp5352 >= _temp5350.last_plus_one){ _throw( Null_Exception);}*
_temp5352;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5353=
yyls; struct Cyc_Yyltype* _temp5355= _temp5353.curr +( yylsp_offset - 2); if(
_temp5355 < _temp5353.base? 1: _temp5355 >= _temp5353.last_plus_one){ _throw(
Null_Exception);}* _temp5355;}).first_line,({ struct _tagged_ptr3 _temp5356=
yyls; struct Cyc_Yyltype* _temp5358= _temp5356.curr + yylsp_offset; if(
_temp5358 < _temp5356.base? 1: _temp5358 >= _temp5356.last_plus_one){ _throw(
Null_Exception);}* _temp5358;}).last_line))};( struct _xenum_struct*) _temp5346;});
break; case 309: _LL5345: yyval=({ struct _tagged_ptr2 _temp5360= yyvs; struct
_xenum_struct** _temp5362= _temp5360.curr + yyvsp_offset; if( _temp5362 <
_temp5360.base? 1: _temp5362 >= _temp5360.last_plus_one){ _throw( Null_Exception);}*
_temp5362;}); break; case 310: _LL5359: yyval=({ struct Cyc_Exp_tok_struct*
_temp5364=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5364=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_cast_exp((*
Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp5365= yyvs; struct
_xenum_struct** _temp5367= _temp5365.curr +( yyvsp_offset - 2); if( _temp5367 <
_temp5365.base? 1: _temp5367 >= _temp5365.last_plus_one){ _throw( Null_Exception);}*
_temp5367;}))).f3, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5368= yyvs;
struct _xenum_struct** _temp5370= _temp5368.curr + yyvsp_offset; if( _temp5370 <
_temp5368.base? 1: _temp5370 >= _temp5368.last_plus_one){ _throw( Null_Exception);}*
_temp5370;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5371=
yyls; struct Cyc_Yyltype* _temp5373= _temp5371.curr +( yylsp_offset - 3); if(
_temp5373 < _temp5371.base? 1: _temp5373 >= _temp5371.last_plus_one){ _throw(
Null_Exception);}* _temp5373;}).first_line,({ struct _tagged_ptr3 _temp5374=
yyls; struct Cyc_Yyltype* _temp5376= _temp5374.curr + yylsp_offset; if(
_temp5376 < _temp5374.base? 1: _temp5376 >= _temp5374.last_plus_one){ _throw(
Null_Exception);}* _temp5376;}).last_line))};( struct _xenum_struct*) _temp5364;});
break; case 311: _LL5363: yyval=({ struct _tagged_ptr2 _temp5378= yyvs; struct
_xenum_struct** _temp5380= _temp5378.curr + yyvsp_offset; if( _temp5380 <
_temp5378.base? 1: _temp5380 >= _temp5378.last_plus_one){ _throw( Null_Exception);}*
_temp5380;}); break; case 312: _LL5377: yyval=({ struct Cyc_Exp_tok_struct*
_temp5382=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5382=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_pre_inc_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5383= yyvs; struct _xenum_struct**
_temp5385= _temp5383.curr + yyvsp_offset; if( _temp5385 < _temp5383.base? 1:
_temp5385 >= _temp5383.last_plus_one){ _throw( Null_Exception);}* _temp5385;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5386= yyls; struct Cyc_Yyltype*
_temp5388= _temp5386.curr +( yylsp_offset - 1); if( _temp5388 < _temp5386.base?
1: _temp5388 >= _temp5386.last_plus_one){ _throw( Null_Exception);}* _temp5388;}).first_line,({
struct _tagged_ptr3 _temp5389= yyls; struct Cyc_Yyltype* _temp5391= _temp5389.curr
+ yylsp_offset; if( _temp5391 < _temp5389.base? 1: _temp5391 >= _temp5389.last_plus_one){
_throw( Null_Exception);}* _temp5391;}).last_line))};( struct _xenum_struct*)
_temp5382;}); break; case 313: _LL5381: yyval=({ struct Cyc_Exp_tok_struct*
_temp5393=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5393=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_pre_dec_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5394= yyvs; struct _xenum_struct**
_temp5396= _temp5394.curr + yyvsp_offset; if( _temp5396 < _temp5394.base? 1:
_temp5396 >= _temp5394.last_plus_one){ _throw( Null_Exception);}* _temp5396;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5397= yyls; struct Cyc_Yyltype*
_temp5399= _temp5397.curr +( yylsp_offset - 1); if( _temp5399 < _temp5397.base?
1: _temp5399 >= _temp5397.last_plus_one){ _throw( Null_Exception);}* _temp5399;}).first_line,({
struct _tagged_ptr3 _temp5400= yyls; struct Cyc_Yyltype* _temp5402= _temp5400.curr
+ yylsp_offset; if( _temp5402 < _temp5400.base? 1: _temp5402 >= _temp5400.last_plus_one){
_throw( Null_Exception);}* _temp5402;}).last_line))};( struct _xenum_struct*)
_temp5393;}); break; case 314: _LL5392: yyval=({ struct Cyc_Exp_tok_struct*
_temp5404=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5404=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_address_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5405= yyvs; struct _xenum_struct**
_temp5407= _temp5405.curr + yyvsp_offset; if( _temp5407 < _temp5405.base? 1:
_temp5407 >= _temp5405.last_plus_one){ _throw( Null_Exception);}* _temp5407;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5408= yyls; struct Cyc_Yyltype*
_temp5410= _temp5408.curr +( yylsp_offset - 1); if( _temp5410 < _temp5408.base?
1: _temp5410 >= _temp5408.last_plus_one){ _throw( Null_Exception);}* _temp5410;}).first_line,({
struct _tagged_ptr3 _temp5411= yyls; struct Cyc_Yyltype* _temp5413= _temp5411.curr
+ yylsp_offset; if( _temp5413 < _temp5411.base? 1: _temp5413 >= _temp5411.last_plus_one){
_throw( Null_Exception);}* _temp5413;}).last_line))};( struct _xenum_struct*)
_temp5404;}); break; case 315: _LL5403: yyval=({ struct Cyc_Exp_tok_struct*
_temp5415=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5415=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_deref_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5416= yyvs; struct _xenum_struct**
_temp5418= _temp5416.curr + yyvsp_offset; if( _temp5418 < _temp5416.base? 1:
_temp5418 >= _temp5416.last_plus_one){ _throw( Null_Exception);}* _temp5418;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5419= yyls; struct Cyc_Yyltype*
_temp5421= _temp5419.curr +( yylsp_offset - 1); if( _temp5421 < _temp5419.base?
1: _temp5421 >= _temp5419.last_plus_one){ _throw( Null_Exception);}* _temp5421;}).first_line,({
struct _tagged_ptr3 _temp5422= yyls; struct Cyc_Yyltype* _temp5424= _temp5422.curr
+ yylsp_offset; if( _temp5424 < _temp5422.base? 1: _temp5424 >= _temp5422.last_plus_one){
_throw( Null_Exception);}* _temp5424;}).last_line))};( struct _xenum_struct*)
_temp5415;}); break; case 316: _LL5414: yyval=({ struct _tagged_ptr2 _temp5426=
yyvs; struct _xenum_struct** _temp5428= _temp5426.curr + yyvsp_offset; if(
_temp5428 < _temp5426.base? 1: _temp5428 >= _temp5426.last_plus_one){ _throw(
Null_Exception);}* _temp5428;}); break; case 317: _LL5425: yyval=({ struct Cyc_Exp_tok_struct*
_temp5430=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5430=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_prim1_exp(
Cyc_yyget_Primop_tok(({ struct _tagged_ptr2 _temp5431= yyvs; struct
_xenum_struct** _temp5433= _temp5431.curr +( yyvsp_offset - 1); if( _temp5433 <
_temp5431.base? 1: _temp5433 >= _temp5431.last_plus_one){ _throw( Null_Exception);}*
_temp5433;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5434= yyvs; struct
_xenum_struct** _temp5436= _temp5434.curr + yyvsp_offset; if( _temp5436 <
_temp5434.base? 1: _temp5436 >= _temp5434.last_plus_one){ _throw( Null_Exception);}*
_temp5436;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5437=
yyls; struct Cyc_Yyltype* _temp5439= _temp5437.curr +( yylsp_offset - 1); if(
_temp5439 < _temp5437.base? 1: _temp5439 >= _temp5437.last_plus_one){ _throw(
Null_Exception);}* _temp5439;}).first_line,({ struct _tagged_ptr3 _temp5440=
yyls; struct Cyc_Yyltype* _temp5442= _temp5440.curr + yylsp_offset; if(
_temp5442 < _temp5440.base? 1: _temp5442 >= _temp5440.last_plus_one){ _throw(
Null_Exception);}* _temp5442;}).last_line))};( struct _xenum_struct*) _temp5430;});
break; case 318: _LL5429: yyval=({ struct Cyc_Exp_tok_struct* _temp5444=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp5444=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_sizeof_exp((*
Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp5445= yyvs; struct
_xenum_struct** _temp5447= _temp5445.curr +( yyvsp_offset - 1); if( _temp5447 <
_temp5445.base? 1: _temp5447 >= _temp5445.last_plus_one){ _throw( Null_Exception);}*
_temp5447;}))).f3, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5448=
yyls; struct Cyc_Yyltype* _temp5450= _temp5448.curr +( yylsp_offset - 3); if(
_temp5450 < _temp5448.base? 1: _temp5450 >= _temp5448.last_plus_one){ _throw(
Null_Exception);}* _temp5450;}).first_line,({ struct _tagged_ptr3 _temp5451=
yyls; struct Cyc_Yyltype* _temp5453= _temp5451.curr + yylsp_offset; if(
_temp5453 < _temp5451.base? 1: _temp5453 >= _temp5451.last_plus_one){ _throw(
Null_Exception);}* _temp5453;}).last_line))};( struct _xenum_struct*) _temp5444;});
break; case 319: _LL5443: yyval=({ struct Cyc_Exp_tok_struct* _temp5455=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp5455=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_prim1_exp( Cyc_Absyn_Size,
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5456= yyvs; struct _xenum_struct**
_temp5458= _temp5456.curr + yyvsp_offset; if( _temp5458 < _temp5456.base? 1:
_temp5458 >= _temp5456.last_plus_one){ _throw( Null_Exception);}* _temp5458;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5459= yyls; struct Cyc_Yyltype*
_temp5461= _temp5459.curr +( yylsp_offset - 1); if( _temp5461 < _temp5459.base?
1: _temp5461 >= _temp5459.last_plus_one){ _throw( Null_Exception);}* _temp5461;}).first_line,({
struct _tagged_ptr3 _temp5462= yyls; struct Cyc_Yyltype* _temp5464= _temp5462.curr
+ yylsp_offset; if( _temp5464 < _temp5462.base? 1: _temp5464 >= _temp5462.last_plus_one){
_throw( Null_Exception);}* _temp5464;}).last_line))};( struct _xenum_struct*)
_temp5455;}); break; case 320: _LL5454: yyval=({ struct Cyc_Exp_tok_struct*
_temp5466=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5466=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_throw_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5467= yyvs; struct _xenum_struct**
_temp5469= _temp5467.curr + yyvsp_offset; if( _temp5469 < _temp5467.base? 1:
_temp5469 >= _temp5467.last_plus_one){ _throw( Null_Exception);}* _temp5469;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5470= yyls; struct Cyc_Yyltype*
_temp5472= _temp5470.curr +( yylsp_offset - 1); if( _temp5472 < _temp5470.base?
1: _temp5472 >= _temp5470.last_plus_one){ _throw( Null_Exception);}* _temp5472;}).first_line,({
struct _tagged_ptr3 _temp5473= yyls; struct Cyc_Yyltype* _temp5475= _temp5473.curr
+ yylsp_offset; if( _temp5475 < _temp5473.base? 1: _temp5475 >= _temp5473.last_plus_one){
_throw( Null_Exception);}* _temp5475;}).last_line))};( struct _xenum_struct*)
_temp5466;}); break; case 321: _LL5465: yyval=({ struct Cyc_Exp_tok_struct*
_temp5477=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5477=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_primop_exp(
Cyc_yyget_Primop_tok(({ struct _tagged_ptr2 _temp5478= yyvs; struct
_xenum_struct** _temp5480= _temp5478.curr +( yyvsp_offset - 3); if( _temp5480 <
_temp5478.base? 1: _temp5480 >= _temp5478.last_plus_one){ _throw( Null_Exception);}*
_temp5480;})), Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2 _temp5481= yyvs;
struct _xenum_struct** _temp5483= _temp5481.curr +( yyvsp_offset - 1); if(
_temp5483 < _temp5481.base? 1: _temp5483 >= _temp5481.last_plus_one){ _throw(
Null_Exception);}* _temp5483;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5484= yyls; struct Cyc_Yyltype* _temp5486= _temp5484.curr +(
yylsp_offset - 3); if( _temp5486 < _temp5484.base? 1: _temp5486 >= _temp5484.last_plus_one){
_throw( Null_Exception);}* _temp5486;}).first_line,({ struct _tagged_ptr3
_temp5487= yyls; struct Cyc_Yyltype* _temp5489= _temp5487.curr + yylsp_offset;
if( _temp5489 < _temp5487.base? 1: _temp5489 >= _temp5487.last_plus_one){ _throw(
Null_Exception);}* _temp5489;}).last_line))};( struct _xenum_struct*) _temp5477;});
break; case 322: _LL5476: yyval=({ struct Cyc_Primop_tok_struct* _temp5491=(
struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));*
_temp5491=( struct Cyc_Primop_tok_struct){.tag= Cyc_Primop_tok_tag,.f1= Cyc_Absyn_Printf};(
struct _xenum_struct*) _temp5491;}); break; case 323: _LL5490: yyval=({ struct
Cyc_Primop_tok_struct* _temp5493=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct));* _temp5493=( struct Cyc_Primop_tok_struct){.tag=
Cyc_Primop_tok_tag,.f1= Cyc_Absyn_Fprintf};( struct _xenum_struct*) _temp5493;});
break; case 324: _LL5492: yyval=({ struct Cyc_Primop_tok_struct* _temp5495=(
struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));*
_temp5495=( struct Cyc_Primop_tok_struct){.tag= Cyc_Primop_tok_tag,.f1= Cyc_Absyn_Xprintf};(
struct _xenum_struct*) _temp5495;}); break; case 325: _LL5494: yyval=({ struct
Cyc_Primop_tok_struct* _temp5497=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct));* _temp5497=( struct Cyc_Primop_tok_struct){.tag=
Cyc_Primop_tok_tag,.f1= Cyc_Absyn_Scanf};( struct _xenum_struct*) _temp5497;});
break; case 326: _LL5496: yyval=({ struct Cyc_Primop_tok_struct* _temp5499=(
struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));*
_temp5499=( struct Cyc_Primop_tok_struct){.tag= Cyc_Primop_tok_tag,.f1= Cyc_Absyn_Fscanf};(
struct _xenum_struct*) _temp5499;}); break; case 327: _LL5498: yyval=({ struct
Cyc_Primop_tok_struct* _temp5501=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct));* _temp5501=( struct Cyc_Primop_tok_struct){.tag=
Cyc_Primop_tok_tag,.f1= Cyc_Absyn_Sscanf};( struct _xenum_struct*) _temp5501;});
break; case 328: _LL5500: yyval=({ struct Cyc_Primop_tok_struct* _temp5503=(
struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));*
_temp5503=( struct Cyc_Primop_tok_struct){.tag= Cyc_Primop_tok_tag,.f1= Cyc_Absyn_Bitnot};(
struct _xenum_struct*) _temp5503;}); break; case 329: _LL5502: yyval=({ struct
Cyc_Primop_tok_struct* _temp5505=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct));* _temp5505=( struct Cyc_Primop_tok_struct){.tag=
Cyc_Primop_tok_tag,.f1= Cyc_Absyn_Not};( struct _xenum_struct*) _temp5505;});
break; case 330: _LL5504: yyval=({ struct Cyc_Primop_tok_struct* _temp5507=(
struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));*
_temp5507=( struct Cyc_Primop_tok_struct){.tag= Cyc_Primop_tok_tag,.f1= Cyc_Absyn_Minus};(
struct _xenum_struct*) _temp5507;}); break; case 331: _LL5506: yyval=({ struct
_tagged_ptr2 _temp5509= yyvs; struct _xenum_struct** _temp5511= _temp5509.curr +
yyvsp_offset; if( _temp5511 < _temp5509.base? 1: _temp5511 >= _temp5509.last_plus_one){
_throw( Null_Exception);}* _temp5511;}); break; case 332: _LL5508: yyval=({
struct Cyc_Exp_tok_struct* _temp5513=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct));* _temp5513=( struct Cyc_Exp_tok_struct){.tag=
Cyc_Exp_tok_tag,.f1= Cyc_Absyn_subscript_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5514= yyvs; struct _xenum_struct** _temp5516= _temp5514.curr +(
yyvsp_offset - 3); if( _temp5516 < _temp5514.base? 1: _temp5516 >= _temp5514.last_plus_one){
_throw( Null_Exception);}* _temp5516;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5517= yyvs; struct _xenum_struct** _temp5519= _temp5517.curr +(
yyvsp_offset - 1); if( _temp5519 < _temp5517.base? 1: _temp5519 >= _temp5517.last_plus_one){
_throw( Null_Exception);}* _temp5519;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5520= yyls; struct Cyc_Yyltype* _temp5522= _temp5520.curr +(
yylsp_offset - 3); if( _temp5522 < _temp5520.base? 1: _temp5522 >= _temp5520.last_plus_one){
_throw( Null_Exception);}* _temp5522;}).first_line,({ struct _tagged_ptr3
_temp5523= yyls; struct Cyc_Yyltype* _temp5525= _temp5523.curr + yylsp_offset;
if( _temp5525 < _temp5523.base? 1: _temp5525 >= _temp5523.last_plus_one){ _throw(
Null_Exception);}* _temp5525;}).last_line))};( struct _xenum_struct*) _temp5513;});
break; case 333: _LL5512: yyval=({ struct Cyc_Exp_tok_struct* _temp5527=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp5527=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5528= yyvs; struct _xenum_struct**
_temp5530= _temp5528.curr +( yyvsp_offset - 2); if( _temp5530 < _temp5528.base?
1: _temp5530 >= _temp5528.last_plus_one){ _throw( Null_Exception);}* _temp5530;})),
0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5531= yyls; struct
Cyc_Yyltype* _temp5533= _temp5531.curr +( yylsp_offset - 2); if( _temp5533 <
_temp5531.base? 1: _temp5533 >= _temp5531.last_plus_one){ _throw( Null_Exception);}*
_temp5533;}).first_line,({ struct _tagged_ptr3 _temp5534= yyls; struct Cyc_Yyltype*
_temp5536= _temp5534.curr + yylsp_offset; if( _temp5536 < _temp5534.base? 1:
_temp5536 >= _temp5534.last_plus_one){ _throw( Null_Exception);}* _temp5536;}).last_line))};(
struct _xenum_struct*) _temp5527;}); break; case 334: _LL5526: yyval=({ struct
Cyc_Exp_tok_struct* _temp5538=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct));* _temp5538=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1=
Cyc_Absyn_unknowncall_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5539=
yyvs; struct _xenum_struct** _temp5541= _temp5539.curr +( yyvsp_offset - 3); if(
_temp5541 < _temp5539.base? 1: _temp5541 >= _temp5539.last_plus_one){ _throw(
Null_Exception);}* _temp5541;})), Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2
_temp5542= yyvs; struct _xenum_struct** _temp5544= _temp5542.curr +(
yyvsp_offset - 1); if( _temp5544 < _temp5542.base? 1: _temp5544 >= _temp5542.last_plus_one){
_throw( Null_Exception);}* _temp5544;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5545= yyls; struct Cyc_Yyltype* _temp5547= _temp5545.curr +(
yylsp_offset - 3); if( _temp5547 < _temp5545.base? 1: _temp5547 >= _temp5545.last_plus_one){
_throw( Null_Exception);}* _temp5547;}).first_line,({ struct _tagged_ptr3
_temp5548= yyls; struct Cyc_Yyltype* _temp5550= _temp5548.curr + yylsp_offset;
if( _temp5550 < _temp5548.base? 1: _temp5550 >= _temp5548.last_plus_one){ _throw(
Null_Exception);}* _temp5550;}).last_line))};( struct _xenum_struct*) _temp5538;});
break; case 335: _LL5537: yyval=({ struct Cyc_Exp_tok_struct* _temp5552=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp5552=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_structmember_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5553= yyvs; struct _xenum_struct**
_temp5555= _temp5553.curr +( yyvsp_offset - 2); if( _temp5555 < _temp5553.base?
1: _temp5555 >= _temp5553.last_plus_one){ _throw( Null_Exception);}* _temp5555;})),({
struct _tagged_string* _temp5556=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp5556[ 0]= Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp5557= yyvs; struct _xenum_struct** _temp5559= _temp5557.curr +
yyvsp_offset; if( _temp5559 < _temp5557.base? 1: _temp5559 >= _temp5557.last_plus_one){
_throw( Null_Exception);}* _temp5559;})); _temp5556;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5560= yyls; struct Cyc_Yyltype* _temp5562= _temp5560.curr
+( yylsp_offset - 2); if( _temp5562 < _temp5560.base? 1: _temp5562 >= _temp5560.last_plus_one){
_throw( Null_Exception);}* _temp5562;}).first_line,({ struct _tagged_ptr3
_temp5563= yyls; struct Cyc_Yyltype* _temp5565= _temp5563.curr + yylsp_offset;
if( _temp5565 < _temp5563.base? 1: _temp5565 >= _temp5563.last_plus_one){ _throw(
Null_Exception);}* _temp5565;}).last_line))};( struct _xenum_struct*) _temp5552;});
break; case 336: _LL5551: { struct _tuple1* q= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp5567= yyvs; struct _xenum_struct** _temp5569= _temp5567.curr +
yyvsp_offset; if( _temp5569 < _temp5567.base? 1: _temp5569 >= _temp5567.last_plus_one){
_throw( Null_Exception);}* _temp5569;}));{ void* _temp5570=(* q).f1; struct Cyc_List_List*
_temp5580; struct Cyc_List_List* _temp5582; _LL5572: if( _temp5570 == Cyc_Absyn_Loc_n){
goto _LL5573;} else{ goto _LL5574;} _LL5574: if(( unsigned int) _temp5570 > 1u?((
struct _enum_struct*) _temp5570)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL5581:
_temp5580=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp5570)->f1;
if( _temp5580 == 0){ goto _LL5575;} else{ goto _LL5576;}} else{ goto _LL5576;}
_LL5576: if(( unsigned int) _temp5570 > 1u?(( struct _enum_struct*) _temp5570)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL5583: _temp5582=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp5570)->f1; if( _temp5582 == 0){ goto _LL5577;}
else{ goto _LL5578;}} else{ goto _LL5578;} _LL5578: goto _LL5579; _LL5573: goto
_LL5571; _LL5575: goto _LL5571; _LL5577: goto _LL5571; _LL5579: Cyc_Parse_err((
struct _tagged_string)({ char* _temp5584=( char*)"struct field name is qualified";
struct _tagged_string _temp5585; _temp5585.curr= _temp5584; _temp5585.base=
_temp5584; _temp5585.last_plus_one= _temp5584 + 31; _temp5585;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5586= yyls; struct Cyc_Yyltype* _temp5588= _temp5586.curr
+ yylsp_offset; if( _temp5588 < _temp5586.base? 1: _temp5588 >= _temp5586.last_plus_one){
_throw( Null_Exception);}* _temp5588;}).first_line,({ struct _tagged_ptr3
_temp5589= yyls; struct Cyc_Yyltype* _temp5591= _temp5589.curr + yylsp_offset;
if( _temp5591 < _temp5589.base? 1: _temp5591 >= _temp5589.last_plus_one){ _throw(
Null_Exception);}* _temp5591;}).last_line)); goto _LL5571; _LL5571:;} yyval=({
struct Cyc_Exp_tok_struct* _temp5592=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct));* _temp5592=( struct Cyc_Exp_tok_struct){.tag=
Cyc_Exp_tok_tag,.f1= Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5593= yyvs; struct _xenum_struct** _temp5595= _temp5593.curr +(
yyvsp_offset - 2); if( _temp5595 < _temp5593.base? 1: _temp5595 >= _temp5593.last_plus_one){
_throw( Null_Exception);}* _temp5595;})),(* q).f2, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5596= yyls; struct Cyc_Yyltype* _temp5598= _temp5596.curr
+( yylsp_offset - 2); if( _temp5598 < _temp5596.base? 1: _temp5598 >= _temp5596.last_plus_one){
_throw( Null_Exception);}* _temp5598;}).first_line,({ struct _tagged_ptr3
_temp5599= yyls; struct Cyc_Yyltype* _temp5601= _temp5599.curr + yylsp_offset;
if( _temp5601 < _temp5599.base? 1: _temp5601 >= _temp5599.last_plus_one){ _throw(
Null_Exception);}* _temp5601;}).last_line))};( struct _xenum_struct*) _temp5592;});
break;} case 337: _LL5566: yyval=({ struct Cyc_Exp_tok_struct* _temp5603=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5603=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_structarrow_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5604= yyvs; struct _xenum_struct**
_temp5606= _temp5604.curr +( yyvsp_offset - 2); if( _temp5606 < _temp5604.base?
1: _temp5606 >= _temp5604.last_plus_one){ _throw( Null_Exception);}* _temp5606;})),({
struct _tagged_string* _temp5607=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp5607[ 0]= Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp5608= yyvs; struct _xenum_struct** _temp5610= _temp5608.curr +
yyvsp_offset; if( _temp5610 < _temp5608.base? 1: _temp5610 >= _temp5608.last_plus_one){
_throw( Null_Exception);}* _temp5610;})); _temp5607;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5611= yyls; struct Cyc_Yyltype* _temp5613= _temp5611.curr
+( yylsp_offset - 2); if( _temp5613 < _temp5611.base? 1: _temp5613 >= _temp5611.last_plus_one){
_throw( Null_Exception);}* _temp5613;}).first_line,({ struct _tagged_ptr3
_temp5614= yyls; struct Cyc_Yyltype* _temp5616= _temp5614.curr + yylsp_offset;
if( _temp5616 < _temp5614.base? 1: _temp5616 >= _temp5614.last_plus_one){ _throw(
Null_Exception);}* _temp5616;}).last_line))};( struct _xenum_struct*) _temp5603;});
break; case 338: _LL5602: { struct _tuple1* q= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp5618= yyvs; struct _xenum_struct** _temp5620= _temp5618.curr +
yyvsp_offset; if( _temp5620 < _temp5618.base? 1: _temp5620 >= _temp5618.last_plus_one){
_throw( Null_Exception);}* _temp5620;}));{ void* _temp5621=(* q).f1; struct Cyc_List_List*
_temp5631; struct Cyc_List_List* _temp5633; _LL5623: if( _temp5621 == Cyc_Absyn_Loc_n){
goto _LL5624;} else{ goto _LL5625;} _LL5625: if(( unsigned int) _temp5621 > 1u?((
struct _enum_struct*) _temp5621)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL5632:
_temp5631=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp5621)->f1;
if( _temp5631 == 0){ goto _LL5626;} else{ goto _LL5627;}} else{ goto _LL5627;}
_LL5627: if(( unsigned int) _temp5621 > 1u?(( struct _enum_struct*) _temp5621)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL5634: _temp5633=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp5621)->f1; if( _temp5633 == 0){ goto _LL5628;}
else{ goto _LL5629;}} else{ goto _LL5629;} _LL5629: goto _LL5630; _LL5624: goto
_LL5622; _LL5626: goto _LL5622; _LL5628: goto _LL5622; _LL5630: Cyc_Parse_err((
struct _tagged_string)({ char* _temp5635=( char*)"struct field is qualified with module name";
struct _tagged_string _temp5636; _temp5636.curr= _temp5635; _temp5636.base=
_temp5635; _temp5636.last_plus_one= _temp5635 + 43; _temp5636;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5637= yyls; struct Cyc_Yyltype* _temp5639= _temp5637.curr
+ yylsp_offset; if( _temp5639 < _temp5637.base? 1: _temp5639 >= _temp5637.last_plus_one){
_throw( Null_Exception);}* _temp5639;}).first_line,({ struct _tagged_ptr3
_temp5640= yyls; struct Cyc_Yyltype* _temp5642= _temp5640.curr + yylsp_offset;
if( _temp5642 < _temp5640.base? 1: _temp5642 >= _temp5640.last_plus_one){ _throw(
Null_Exception);}* _temp5642;}).last_line)); goto _LL5622; _LL5622:;} yyval=({
struct Cyc_Exp_tok_struct* _temp5643=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct));* _temp5643=( struct Cyc_Exp_tok_struct){.tag=
Cyc_Exp_tok_tag,.f1= Cyc_Absyn_structarrow_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5644= yyvs; struct _xenum_struct** _temp5646= _temp5644.curr +(
yyvsp_offset - 2); if( _temp5646 < _temp5644.base? 1: _temp5646 >= _temp5644.last_plus_one){
_throw( Null_Exception);}* _temp5646;})),(* q).f2, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5647= yyls; struct Cyc_Yyltype* _temp5649= _temp5647.curr
+( yylsp_offset - 2); if( _temp5649 < _temp5647.base? 1: _temp5649 >= _temp5647.last_plus_one){
_throw( Null_Exception);}* _temp5649;}).first_line,({ struct _tagged_ptr3
_temp5650= yyls; struct Cyc_Yyltype* _temp5652= _temp5650.curr + yylsp_offset;
if( _temp5652 < _temp5650.base? 1: _temp5652 >= _temp5650.last_plus_one){ _throw(
Null_Exception);}* _temp5652;}).last_line))};( struct _xenum_struct*) _temp5643;});
break;} case 339: _LL5617: yyval=({ struct Cyc_Exp_tok_struct* _temp5654=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5654=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_post_inc_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5655= yyvs; struct _xenum_struct**
_temp5657= _temp5655.curr +( yyvsp_offset - 1); if( _temp5657 < _temp5655.base?
1: _temp5657 >= _temp5655.last_plus_one){ _throw( Null_Exception);}* _temp5657;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5658= yyls; struct Cyc_Yyltype*
_temp5660= _temp5658.curr +( yylsp_offset - 1); if( _temp5660 < _temp5658.base?
1: _temp5660 >= _temp5658.last_plus_one){ _throw( Null_Exception);}* _temp5660;}).first_line,({
struct _tagged_ptr3 _temp5661= yyls; struct Cyc_Yyltype* _temp5663= _temp5661.curr
+ yylsp_offset; if( _temp5663 < _temp5661.base? 1: _temp5663 >= _temp5661.last_plus_one){
_throw( Null_Exception);}* _temp5663;}).last_line))};( struct _xenum_struct*)
_temp5654;}); break; case 340: _LL5653: yyval=({ struct Cyc_Exp_tok_struct*
_temp5665=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5665=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_post_dec_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5666= yyvs; struct _xenum_struct**
_temp5668= _temp5666.curr +( yyvsp_offset - 1); if( _temp5668 < _temp5666.base?
1: _temp5668 >= _temp5666.last_plus_one){ _throw( Null_Exception);}* _temp5668;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5669= yyls; struct Cyc_Yyltype*
_temp5671= _temp5669.curr +( yylsp_offset - 1); if( _temp5671 < _temp5669.base?
1: _temp5671 >= _temp5669.last_plus_one){ _throw( Null_Exception);}* _temp5671;}).first_line,({
struct _tagged_ptr3 _temp5672= yyls; struct Cyc_Yyltype* _temp5674= _temp5672.curr
+ yylsp_offset; if( _temp5674 < _temp5672.base? 1: _temp5674 >= _temp5672.last_plus_one){
_throw( Null_Exception);}* _temp5674;}).last_line))};( struct _xenum_struct*)
_temp5665;}); break; case 341: _LL5664: yyval=({ struct Cyc_Exp_tok_struct*
_temp5676=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5676=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_CompoundLit_e_struct* _temp5677=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp5677->tag= Cyc_Absyn_CompoundLit_e_tag;
_temp5677->f1= Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp5678= yyvs;
struct _xenum_struct** _temp5680= _temp5678.curr +( yyvsp_offset - 4); if(
_temp5680 < _temp5678.base? 1: _temp5680 >= _temp5678.last_plus_one){ _throw(
Null_Exception);}* _temp5680;})); _temp5677->f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp5681= yyvs; struct _xenum_struct** _temp5683= _temp5681.curr
+( yyvsp_offset - 1); if( _temp5683 < _temp5681.base? 1: _temp5683 >= _temp5681.last_plus_one){
_throw( Null_Exception);}* _temp5683;})));( void*) _temp5677;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5684= yyls; struct Cyc_Yyltype* _temp5686= _temp5684.curr
+( yylsp_offset - 5); if( _temp5686 < _temp5684.base? 1: _temp5686 >= _temp5684.last_plus_one){
_throw( Null_Exception);}* _temp5686;}).first_line,({ struct _tagged_ptr3
_temp5687= yyls; struct Cyc_Yyltype* _temp5689= _temp5687.curr + yylsp_offset;
if( _temp5689 < _temp5687.base? 1: _temp5689 >= _temp5687.last_plus_one){ _throw(
Null_Exception);}* _temp5689;}).last_line))};( struct _xenum_struct*) _temp5676;});
break; case 342: _LL5675: yyval=({ struct Cyc_Exp_tok_struct* _temp5691=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp5691=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_CompoundLit_e_struct* _temp5692=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp5692->tag= Cyc_Absyn_CompoundLit_e_tag;
_temp5692->f1= Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp5693= yyvs;
struct _xenum_struct** _temp5695= _temp5693.curr +( yyvsp_offset - 5); if(
_temp5695 < _temp5693.base? 1: _temp5695 >= _temp5693.last_plus_one){ _throw(
Null_Exception);}* _temp5695;})); _temp5692->f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp5696= yyvs; struct _xenum_struct** _temp5698= _temp5696.curr
+( yyvsp_offset - 2); if( _temp5698 < _temp5696.base? 1: _temp5698 >= _temp5696.last_plus_one){
_throw( Null_Exception);}* _temp5698;})));( void*) _temp5692;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5699= yyls; struct Cyc_Yyltype* _temp5701= _temp5699.curr
+( yylsp_offset - 6); if( _temp5701 < _temp5699.base? 1: _temp5701 >= _temp5699.last_plus_one){
_throw( Null_Exception);}* _temp5701;}).first_line,({ struct _tagged_ptr3
_temp5702= yyls; struct Cyc_Yyltype* _temp5704= _temp5702.curr + yylsp_offset;
if( _temp5704 < _temp5702.base? 1: _temp5704 >= _temp5702.last_plus_one){ _throw(
Null_Exception);}* _temp5704;}).last_line))};( struct _xenum_struct*) _temp5691;});
break; case 343: _LL5690: yyval=({ struct Cyc_Exp_tok_struct* _temp5706=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp5706=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_Array_e_struct* _temp5707=( struct Cyc_Absyn_Array_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp5707->tag= Cyc_Absyn_Array_e_tag;
_temp5707->f1= 1; _temp5707->f2= 0;( void*) _temp5707;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5708= yyls; struct Cyc_Yyltype* _temp5710= _temp5708.curr
+( yylsp_offset - 2); if( _temp5710 < _temp5708.base? 1: _temp5710 >= _temp5708.last_plus_one){
_throw( Null_Exception);}* _temp5710;}).first_line,({ struct _tagged_ptr3
_temp5711= yyls; struct Cyc_Yyltype* _temp5713= _temp5711.curr + yylsp_offset;
if( _temp5713 < _temp5711.base? 1: _temp5713 >= _temp5711.last_plus_one){ _throw(
Null_Exception);}* _temp5713;}).last_line))};( struct _xenum_struct*) _temp5706;});
break; case 344: _LL5705: yyval=({ struct Cyc_Exp_tok_struct* _temp5715=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp5715=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_Array_e_struct* _temp5716=( struct Cyc_Absyn_Array_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp5716->tag= Cyc_Absyn_Array_e_tag;
_temp5716->f1= 1; _temp5716->f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({ struct _tagged_ptr2
_temp5717= yyvs; struct _xenum_struct** _temp5719= _temp5717.curr +(
yyvsp_offset - 1); if( _temp5719 < _temp5717.base? 1: _temp5719 >= _temp5717.last_plus_one){
_throw( Null_Exception);}* _temp5719;})));( void*) _temp5716;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5720= yyls; struct Cyc_Yyltype* _temp5722= _temp5720.curr
+( yylsp_offset - 3); if( _temp5722 < _temp5720.base? 1: _temp5722 >= _temp5720.last_plus_one){
_throw( Null_Exception);}* _temp5722;}).first_line,({ struct _tagged_ptr3
_temp5723= yyls; struct Cyc_Yyltype* _temp5725= _temp5723.curr +( yylsp_offset -
1); if( _temp5725 < _temp5723.base? 1: _temp5725 >= _temp5723.last_plus_one){
_throw( Null_Exception);}* _temp5725;}).last_line))};( struct _xenum_struct*)
_temp5715;}); break; case 345: _LL5714: yyval=({ struct Cyc_Exp_tok_struct*
_temp5727=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5727=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_Array_e_struct* _temp5728=( struct Cyc_Absyn_Array_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp5728->tag= Cyc_Absyn_Array_e_tag;
_temp5728->f1= 1; _temp5728->f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({ struct _tagged_ptr2
_temp5729= yyvs; struct _xenum_struct** _temp5731= _temp5729.curr +(
yyvsp_offset - 2); if( _temp5731 < _temp5729.base? 1: _temp5731 >= _temp5729.last_plus_one){
_throw( Null_Exception);}* _temp5731;})));( void*) _temp5728;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5732= yyls; struct Cyc_Yyltype* _temp5734= _temp5732.curr
+( yylsp_offset - 4); if( _temp5734 < _temp5732.base? 1: _temp5734 >= _temp5732.last_plus_one){
_throw( Null_Exception);}* _temp5734;}).first_line,({ struct _tagged_ptr3
_temp5735= yyls; struct Cyc_Yyltype* _temp5737= _temp5735.curr +( yylsp_offset -
1); if( _temp5737 < _temp5735.base? 1: _temp5737 >= _temp5735.last_plus_one){
_throw( Null_Exception);}* _temp5737;}).last_line))};( struct _xenum_struct*)
_temp5727;}); break; case 346: _LL5726: yyval=({ struct Cyc_Exp_tok_struct*
_temp5739=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5739=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_Comprehension_e_struct* _temp5740=( struct Cyc_Absyn_Comprehension_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Comprehension_e_struct)); _temp5740->tag=
Cyc_Absyn_Comprehension_e_tag; _temp5740->f1= Cyc_Absyn_new_vardecl(({ struct
_tuple1* _temp5741=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp5741->f1= Cyc_Absyn_Loc_n; _temp5741->f2=({ struct _tagged_string*
_temp5742=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp5742[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp5743= yyvs;
struct _xenum_struct** _temp5745= _temp5743.curr +( yyvsp_offset - 5); if(
_temp5745 < _temp5743.base? 1: _temp5745 >= _temp5743.last_plus_one){ _throw(
Null_Exception);}* _temp5745;})); _temp5742;}); _temp5741;}), Cyc_Absyn_uint_t,(
struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp( 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5746= yyls; struct Cyc_Yyltype* _temp5748= _temp5746.curr
+( yylsp_offset - 5); if( _temp5748 < _temp5746.base? 1: _temp5748 >= _temp5746.last_plus_one){
_throw( Null_Exception);}* _temp5748;}).first_line,({ struct _tagged_ptr3
_temp5749= yyls; struct Cyc_Yyltype* _temp5751= _temp5749.curr +( yylsp_offset -
5); if( _temp5751 < _temp5749.base? 1: _temp5751 >= _temp5749.last_plus_one){
_throw( Null_Exception);}* _temp5751;}).last_line))); _temp5740->f2= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5752= yyvs; struct _xenum_struct** _temp5754= _temp5752.curr
+( yyvsp_offset - 3); if( _temp5754 < _temp5752.base? 1: _temp5754 >= _temp5752.last_plus_one){
_throw( Null_Exception);}* _temp5754;})); _temp5740->f3= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5755= yyvs; struct _xenum_struct** _temp5757= _temp5755.curr
+( yyvsp_offset - 1); if( _temp5757 < _temp5755.base? 1: _temp5757 >= _temp5755.last_plus_one){
_throw( Null_Exception);}* _temp5757;}));( void*) _temp5740;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5758= yyls; struct Cyc_Yyltype* _temp5760= _temp5758.curr
+( yylsp_offset - 8); if( _temp5760 < _temp5758.base? 1: _temp5760 >= _temp5758.last_plus_one){
_throw( Null_Exception);}* _temp5760;}).first_line,({ struct _tagged_ptr3
_temp5761= yyls; struct Cyc_Yyltype* _temp5763= _temp5761.curr + yylsp_offset;
if( _temp5763 < _temp5761.base? 1: _temp5763 >= _temp5761.last_plus_one){ _throw(
Null_Exception);}* _temp5763;}).last_line))};( struct _xenum_struct*) _temp5739;});
break; case 347: _LL5738: yyval=({ struct Cyc_Exp_tok_struct* _temp5765=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp5765=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_string_exp( 1,
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp5766= yyvs; struct
_xenum_struct** _temp5768= _temp5766.curr + yyvsp_offset; if( _temp5768 <
_temp5766.base? 1: _temp5768 >= _temp5766.last_plus_one){ _throw( Null_Exception);}*
_temp5768;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5769=
yyls; struct Cyc_Yyltype* _temp5771= _temp5769.curr +( yylsp_offset - 1); if(
_temp5771 < _temp5769.base? 1: _temp5771 >= _temp5769.last_plus_one){ _throw(
Null_Exception);}* _temp5771;}).first_line,({ struct _tagged_ptr3 _temp5772=
yyls; struct Cyc_Yyltype* _temp5774= _temp5772.curr + yylsp_offset; if(
_temp5774 < _temp5772.base? 1: _temp5774 >= _temp5772.last_plus_one){ _throw(
Null_Exception);}* _temp5774;}).last_line))};( struct _xenum_struct*) _temp5765;});
break; case 348: _LL5764: yyval=({ struct Cyc_Exp_tok_struct* _temp5776=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp5776=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_Fill_e_struct* _temp5777=( struct Cyc_Absyn_Fill_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fill_e_struct)); _temp5777->tag= Cyc_Absyn_Fill_e_tag;
_temp5777->f1= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5778= yyvs; struct
_xenum_struct** _temp5780= _temp5778.curr +( yyvsp_offset - 1); if( _temp5780 <
_temp5778.base? 1: _temp5780 >= _temp5778.last_plus_one){ _throw( Null_Exception);}*
_temp5780;}));( void*) _temp5777;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5781= yyls; struct Cyc_Yyltype* _temp5783= _temp5781.curr +(
yylsp_offset - 3); if( _temp5783 < _temp5781.base? 1: _temp5783 >= _temp5781.last_plus_one){
_throw( Null_Exception);}* _temp5783;}).first_line,({ struct _tagged_ptr3
_temp5784= yyls; struct Cyc_Yyltype* _temp5786= _temp5784.curr + yylsp_offset;
if( _temp5786 < _temp5784.base? 1: _temp5786 >= _temp5784.last_plus_one){ _throw(
Null_Exception);}* _temp5786;}).last_line))};( struct _xenum_struct*) _temp5776;});
break; case 349: _LL5775: yyval=({ struct Cyc_Exp_tok_struct* _temp5788=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp5788=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_Codegen_e_struct* _temp5789=( struct Cyc_Absyn_Codegen_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Codegen_e_struct)); _temp5789->tag= Cyc_Absyn_Codegen_e_tag;
_temp5789->f1= Cyc_yyget_FnDecl_tok(({ struct _tagged_ptr2 _temp5790= yyvs;
struct _xenum_struct** _temp5792= _temp5790.curr +( yyvsp_offset - 1); if(
_temp5792 < _temp5790.base? 1: _temp5792 >= _temp5790.last_plus_one){ _throw(
Null_Exception);}* _temp5792;}));( void*) _temp5789;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5793= yyls; struct Cyc_Yyltype* _temp5795= _temp5793.curr
+( yylsp_offset - 3); if( _temp5795 < _temp5793.base? 1: _temp5795 >= _temp5793.last_plus_one){
_throw( Null_Exception);}* _temp5795;}).first_line,({ struct _tagged_ptr3
_temp5796= yyls; struct Cyc_Yyltype* _temp5798= _temp5796.curr + yylsp_offset;
if( _temp5798 < _temp5796.base? 1: _temp5798 >= _temp5796.last_plus_one){ _throw(
Null_Exception);}* _temp5798;}).last_line))};( struct _xenum_struct*) _temp5788;});
break; case 350: _LL5787: yyval=({ struct Cyc_Exp_tok_struct* _temp5800=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp5800=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_UnknownId_e_struct* _temp5801=( struct Cyc_Absyn_UnknownId_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp5801->tag= Cyc_Absyn_UnknownId_e_tag;
_temp5801->f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp5802= yyvs;
struct _xenum_struct** _temp5804= _temp5802.curr + yyvsp_offset; if( _temp5804 <
_temp5802.base? 1: _temp5804 >= _temp5802.last_plus_one){ _throw( Null_Exception);}*
_temp5804;}));( void*) _temp5801;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5805= yyls; struct Cyc_Yyltype* _temp5807= _temp5805.curr +
yylsp_offset; if( _temp5807 < _temp5805.base? 1: _temp5807 >= _temp5805.last_plus_one){
_throw( Null_Exception);}* _temp5807;}).first_line,({ struct _tagged_ptr3
_temp5808= yyls; struct Cyc_Yyltype* _temp5810= _temp5808.curr + yylsp_offset;
if( _temp5810 < _temp5808.base? 1: _temp5810 >= _temp5808.last_plus_one){ _throw(
Null_Exception);}* _temp5810;}).last_line))};( struct _xenum_struct*) _temp5800;});
break; case 351: _LL5799: yyval=({ struct _tagged_ptr2 _temp5812= yyvs; struct
_xenum_struct** _temp5814= _temp5812.curr + yyvsp_offset; if( _temp5814 <
_temp5812.base? 1: _temp5814 >= _temp5812.last_plus_one){ _throw( Null_Exception);}*
_temp5814;}); break; case 352: _LL5811: yyval=({ struct Cyc_Exp_tok_struct*
_temp5816=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5816=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_string_exp(
0, Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp5817= yyvs; struct
_xenum_struct** _temp5819= _temp5817.curr + yyvsp_offset; if( _temp5819 <
_temp5817.base? 1: _temp5819 >= _temp5817.last_plus_one){ _throw( Null_Exception);}*
_temp5819;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5820=
yyls; struct Cyc_Yyltype* _temp5822= _temp5820.curr + yylsp_offset; if(
_temp5822 < _temp5820.base? 1: _temp5822 >= _temp5820.last_plus_one){ _throw(
Null_Exception);}* _temp5822;}).first_line,({ struct _tagged_ptr3 _temp5823=
yyls; struct Cyc_Yyltype* _temp5825= _temp5823.curr + yylsp_offset; if(
_temp5825 < _temp5823.base? 1: _temp5825 >= _temp5823.last_plus_one){ _throw(
Null_Exception);}* _temp5825;}).last_line))};( struct _xenum_struct*) _temp5816;});
break; case 353: _LL5815: yyval=({ struct _tagged_ptr2 _temp5827= yyvs; struct
_xenum_struct** _temp5829= _temp5827.curr +( yyvsp_offset - 1); if( _temp5829 <
_temp5827.base? 1: _temp5829 >= _temp5827.last_plus_one){ _throw( Null_Exception);}*
_temp5829;}); break; case 354: _LL5826: yyval=({ struct Cyc_Exp_tok_struct*
_temp5831=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5831=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_noinstantiate_exp(
Cyc_Absyn_new_exp(({ struct Cyc_Absyn_UnknownId_e_struct* _temp5832=( struct Cyc_Absyn_UnknownId_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp5832->tag= Cyc_Absyn_UnknownId_e_tag;
_temp5832->f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp5833= yyvs;
struct _xenum_struct** _temp5835= _temp5833.curr +( yyvsp_offset - 1); if(
_temp5835 < _temp5833.base? 1: _temp5835 >= _temp5833.last_plus_one){ _throw(
Null_Exception);}* _temp5835;}));( void*) _temp5832;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5836= yyls; struct Cyc_Yyltype* _temp5838= _temp5836.curr
+( yylsp_offset - 1); if( _temp5838 < _temp5836.base? 1: _temp5838 >= _temp5836.last_plus_one){
_throw( Null_Exception);}* _temp5838;}).first_line,({ struct _tagged_ptr3
_temp5839= yyls; struct Cyc_Yyltype* _temp5841= _temp5839.curr +( yylsp_offset -
1); if( _temp5841 < _temp5839.base? 1: _temp5841 >= _temp5839.last_plus_one){
_throw( Null_Exception);}* _temp5841;}).last_line)), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5842= yyls; struct Cyc_Yyltype* _temp5844= _temp5842.curr
+( yylsp_offset - 1); if( _temp5844 < _temp5842.base? 1: _temp5844 >= _temp5842.last_plus_one){
_throw( Null_Exception);}* _temp5844;}).first_line,({ struct _tagged_ptr3
_temp5845= yyls; struct Cyc_Yyltype* _temp5847= _temp5845.curr + yylsp_offset;
if( _temp5847 < _temp5845.base? 1: _temp5847 >= _temp5845.last_plus_one){ _throw(
Null_Exception);}* _temp5847;}).last_line))};( struct _xenum_struct*) _temp5831;});
break; case 355: _LL5830: yyval=({ struct Cyc_Exp_tok_struct* _temp5849=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp5849=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_instantiate_exp(
Cyc_Absyn_new_exp(({ struct Cyc_Absyn_UnknownId_e_struct* _temp5850=( struct Cyc_Absyn_UnknownId_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp5850->tag= Cyc_Absyn_UnknownId_e_tag;
_temp5850->f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp5851= yyvs;
struct _xenum_struct** _temp5853= _temp5851.curr +( yyvsp_offset - 4); if(
_temp5853 < _temp5851.base? 1: _temp5853 >= _temp5851.last_plus_one){ _throw(
Null_Exception);}* _temp5853;}));( void*) _temp5850;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5854= yyls; struct Cyc_Yyltype* _temp5856= _temp5854.curr
+( yylsp_offset - 4); if( _temp5856 < _temp5854.base? 1: _temp5856 >= _temp5854.last_plus_one){
_throw( Null_Exception);}* _temp5856;}).first_line,({ struct _tagged_ptr3
_temp5857= yyls; struct Cyc_Yyltype* _temp5859= _temp5857.curr +( yylsp_offset -
4); if( _temp5859 < _temp5857.base? 1: _temp5859 >= _temp5857.last_plus_one){
_throw( Null_Exception);}* _temp5859;}).last_line)),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp5860= yyvs; struct _xenum_struct** _temp5862= _temp5860.curr +(
yyvsp_offset - 1); if( _temp5862 < _temp5860.base? 1: _temp5862 >= _temp5860.last_plus_one){
_throw( Null_Exception);}* _temp5862;}))), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5863= yyls; struct Cyc_Yyltype* _temp5865= _temp5863.curr +(
yylsp_offset - 4); if( _temp5865 < _temp5863.base? 1: _temp5865 >= _temp5863.last_plus_one){
_throw( Null_Exception);}* _temp5865;}).first_line,({ struct _tagged_ptr3
_temp5866= yyls; struct Cyc_Yyltype* _temp5868= _temp5866.curr + yylsp_offset;
if( _temp5868 < _temp5866.base? 1: _temp5868 >= _temp5866.last_plus_one){ _throw(
Null_Exception);}* _temp5868;}).last_line))};( struct _xenum_struct*) _temp5849;});
break; case 356: _LL5848: yyval=({ struct Cyc_Exp_tok_struct* _temp5870=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp5870=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_tuple_exp( Cyc_yyget_ExpList_tok(({
struct _tagged_ptr2 _temp5871= yyvs; struct _xenum_struct** _temp5873= _temp5871.curr
+( yyvsp_offset - 1); if( _temp5873 < _temp5871.base? 1: _temp5873 >= _temp5871.last_plus_one){
_throw( Null_Exception);}* _temp5873;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5874= yyls; struct Cyc_Yyltype* _temp5876= _temp5874.curr +(
yylsp_offset - 3); if( _temp5876 < _temp5874.base? 1: _temp5876 >= _temp5874.last_plus_one){
_throw( Null_Exception);}* _temp5876;}).first_line,({ struct _tagged_ptr3
_temp5877= yyls; struct Cyc_Yyltype* _temp5879= _temp5877.curr + yylsp_offset;
if( _temp5879 < _temp5877.base? 1: _temp5879 >= _temp5877.last_plus_one){ _throw(
Null_Exception);}* _temp5879;}).last_line))};( struct _xenum_struct*) _temp5870;});
break; case 357: _LL5869: yyval=({ struct Cyc_Exp_tok_struct* _temp5881=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp5881=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_Struct_e_struct* _temp5882=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp5882->tag= Cyc_Absyn_Struct_e_tag;
_temp5882->f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp5883= yyvs;
struct _xenum_struct** _temp5885= _temp5883.curr +( yyvsp_offset - 3); if(
_temp5885 < _temp5883.base? 1: _temp5885 >= _temp5883.last_plus_one){ _throw(
Null_Exception);}* _temp5885;})); _temp5882->f2= 0; _temp5882->f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp5886= yyvs; struct _xenum_struct** _temp5888= _temp5886.curr
+( yyvsp_offset - 1); if( _temp5888 < _temp5886.base? 1: _temp5888 >= _temp5886.last_plus_one){
_throw( Null_Exception);}* _temp5888;}))); _temp5882->f4= 0;( void*) _temp5882;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5889= yyls; struct Cyc_Yyltype*
_temp5891= _temp5889.curr +( yylsp_offset - 3); if( _temp5891 < _temp5889.base?
1: _temp5891 >= _temp5889.last_plus_one){ _throw( Null_Exception);}* _temp5891;}).first_line,({
struct _tagged_ptr3 _temp5892= yyls; struct Cyc_Yyltype* _temp5894= _temp5892.curr
+ yylsp_offset; if( _temp5894 < _temp5892.base? 1: _temp5894 >= _temp5892.last_plus_one){
_throw( Null_Exception);}* _temp5894;}).last_line))};( struct _xenum_struct*)
_temp5881;}); break; case 358: _LL5880: yyval=({ struct Cyc_Exp_tok_struct*
_temp5896=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5896=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_stmt_exp(
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp5897= yyvs; struct _xenum_struct**
_temp5899= _temp5897.curr +( yyvsp_offset - 2); if( _temp5899 < _temp5897.base?
1: _temp5899 >= _temp5897.last_plus_one){ _throw( Null_Exception);}* _temp5899;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5900= yyls; struct Cyc_Yyltype*
_temp5902= _temp5900.curr +( yylsp_offset - 4); if( _temp5902 < _temp5900.base?
1: _temp5902 >= _temp5900.last_plus_one){ _throw( Null_Exception);}* _temp5902;}).first_line,({
struct _tagged_ptr3 _temp5903= yyls; struct Cyc_Yyltype* _temp5905= _temp5903.curr
+ yylsp_offset; if( _temp5905 < _temp5903.base? 1: _temp5905 >= _temp5903.last_plus_one){
_throw( Null_Exception);}* _temp5905;}).last_line))};( struct _xenum_struct*)
_temp5896;}); break; case 359: _LL5895: yyval=({ struct Cyc_ExpList_tok_struct*
_temp5907=( struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));*
_temp5907=( struct Cyc_ExpList_tok_struct){.tag= Cyc_ExpList_tok_tag,.f1=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ExpList_tok(({
struct _tagged_ptr2 _temp5908= yyvs; struct _xenum_struct** _temp5910= _temp5908.curr
+ yyvsp_offset; if( _temp5910 < _temp5908.base? 1: _temp5910 >= _temp5908.last_plus_one){
_throw( Null_Exception);}* _temp5910;})))};( struct _xenum_struct*) _temp5907;});
break; case 360: _LL5906: yyval=({ struct Cyc_ExpList_tok_struct* _temp5912=(
struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));*
_temp5912=( struct Cyc_ExpList_tok_struct){.tag= Cyc_ExpList_tok_tag,.f1=({
struct Cyc_List_List* _temp5913=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp5913->hd=( void*) Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5914= yyvs; struct _xenum_struct** _temp5916= _temp5914.curr +
yyvsp_offset; if( _temp5916 < _temp5914.base? 1: _temp5916 >= _temp5914.last_plus_one){
_throw( Null_Exception);}* _temp5916;})); _temp5913->tl= 0; _temp5913;})};(
struct _xenum_struct*) _temp5912;}); break; case 361: _LL5911: yyval=({ struct
Cyc_ExpList_tok_struct* _temp5918=( struct Cyc_ExpList_tok_struct*) GC_malloc(
sizeof( struct Cyc_ExpList_tok_struct));* _temp5918=( struct Cyc_ExpList_tok_struct){.tag=
Cyc_ExpList_tok_tag,.f1=({ struct Cyc_List_List* _temp5919=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5919->hd=( void*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5923= yyvs; struct _xenum_struct** _temp5925= _temp5923.curr
+ yyvsp_offset; if( _temp5925 < _temp5923.base? 1: _temp5925 >= _temp5923.last_plus_one){
_throw( Null_Exception);}* _temp5925;})); _temp5919->tl= Cyc_yyget_ExpList_tok(({
struct _tagged_ptr2 _temp5920= yyvs; struct _xenum_struct** _temp5922= _temp5920.curr
+( yyvsp_offset - 2); if( _temp5922 < _temp5920.base? 1: _temp5922 >= _temp5920.last_plus_one){
_throw( Null_Exception);}* _temp5922;})); _temp5919;})};( struct _xenum_struct*)
_temp5918;}); break; case 362: _LL5917: yyval=({ struct Cyc_Exp_tok_struct*
_temp5927=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5927=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_int_exp((*
Cyc_yyget_Int_tok(({ struct _tagged_ptr2 _temp5928= yyvs; struct _xenum_struct**
_temp5930= _temp5928.curr + yyvsp_offset; if( _temp5930 < _temp5928.base? 1:
_temp5930 >= _temp5928.last_plus_one){ _throw( Null_Exception);}* _temp5930;}))).f1,(*
Cyc_yyget_Int_tok(({ struct _tagged_ptr2 _temp5931= yyvs; struct _xenum_struct**
_temp5933= _temp5931.curr + yyvsp_offset; if( _temp5933 < _temp5931.base? 1:
_temp5933 >= _temp5931.last_plus_one){ _throw( Null_Exception);}* _temp5933;}))).f2,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5934= yyls; struct Cyc_Yyltype*
_temp5936= _temp5934.curr + yylsp_offset; if( _temp5936 < _temp5934.base? 1:
_temp5936 >= _temp5934.last_plus_one){ _throw( Null_Exception);}* _temp5936;}).first_line,({
struct _tagged_ptr3 _temp5937= yyls; struct Cyc_Yyltype* _temp5939= _temp5937.curr
+ yylsp_offset; if( _temp5939 < _temp5937.base? 1: _temp5939 >= _temp5937.last_plus_one){
_throw( Null_Exception);}* _temp5939;}).last_line))};( struct _xenum_struct*)
_temp5927;}); break; case 363: _LL5926: yyval=({ struct Cyc_Exp_tok_struct*
_temp5941=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5941=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_char_exp(
Cyc_yyget_Char_tok(({ struct _tagged_ptr2 _temp5942= yyvs; struct _xenum_struct**
_temp5944= _temp5942.curr + yyvsp_offset; if( _temp5944 < _temp5942.base? 1:
_temp5944 >= _temp5942.last_plus_one){ _throw( Null_Exception);}* _temp5944;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5945= yyls; struct Cyc_Yyltype*
_temp5947= _temp5945.curr + yylsp_offset; if( _temp5947 < _temp5945.base? 1:
_temp5947 >= _temp5945.last_plus_one){ _throw( Null_Exception);}* _temp5947;}).first_line,({
struct _tagged_ptr3 _temp5948= yyls; struct Cyc_Yyltype* _temp5950= _temp5948.curr
+ yylsp_offset; if( _temp5950 < _temp5948.base? 1: _temp5950 >= _temp5948.last_plus_one){
_throw( Null_Exception);}* _temp5950;}).last_line))};( struct _xenum_struct*)
_temp5941;}); break; case 364: _LL5940: yyval=({ struct Cyc_Exp_tok_struct*
_temp5952=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5952=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_float_exp(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp5953= yyvs; struct
_xenum_struct** _temp5955= _temp5953.curr + yyvsp_offset; if( _temp5955 <
_temp5953.base? 1: _temp5955 >= _temp5953.last_plus_one){ _throw( Null_Exception);}*
_temp5955;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5956=
yyls; struct Cyc_Yyltype* _temp5958= _temp5956.curr + yylsp_offset; if(
_temp5958 < _temp5956.base? 1: _temp5958 >= _temp5956.last_plus_one){ _throw(
Null_Exception);}* _temp5958;}).first_line,({ struct _tagged_ptr3 _temp5959=
yyls; struct Cyc_Yyltype* _temp5961= _temp5959.curr + yylsp_offset; if(
_temp5961 < _temp5959.base? 1: _temp5961 >= _temp5959.last_plus_one){ _throw(
Null_Exception);}* _temp5961;}).last_line))};( struct _xenum_struct*) _temp5952;});
break; case 365: _LL5951: yyval=({ struct Cyc_Exp_tok_struct* _temp5963=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp5963=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_null_exp( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5964= yyls; struct Cyc_Yyltype* _temp5966= _temp5964.curr
+ yylsp_offset; if( _temp5966 < _temp5964.base? 1: _temp5966 >= _temp5964.last_plus_one){
_throw( Null_Exception);}* _temp5966;}).first_line,({ struct _tagged_ptr3
_temp5967= yyls; struct Cyc_Yyltype* _temp5969= _temp5967.curr + yylsp_offset;
if( _temp5969 < _temp5967.base? 1: _temp5969 >= _temp5967.last_plus_one){ _throw(
Null_Exception);}* _temp5969;}).last_line))};( struct _xenum_struct*) _temp5963;});
break; case 366: _LL5962: yyval=({ struct Cyc_QualId_tok_struct* _temp5971=(
struct Cyc_QualId_tok_struct*) GC_malloc( sizeof( struct Cyc_QualId_tok_struct));*
_temp5971=( struct Cyc_QualId_tok_struct){.tag= Cyc_QualId_tok_tag,.f1=({ struct
_tuple1* _temp5972=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp5972->f1=({ struct Cyc_Absyn_Rel_n_struct* _temp5977=( struct Cyc_Absyn_Rel_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct)); _temp5977->tag= Cyc_Absyn_Rel_n_tag;
_temp5977->f1= 0;( void*) _temp5977;}); _temp5972->f2=({ struct _tagged_string*
_temp5973=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp5973[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp5974= yyvs;
struct _xenum_struct** _temp5976= _temp5974.curr + yyvsp_offset; if( _temp5976 <
_temp5974.base? 1: _temp5976 >= _temp5974.last_plus_one){ _throw( Null_Exception);}*
_temp5976;})); _temp5973;}); _temp5972;})};( struct _xenum_struct*) _temp5971;});
break; case 367: _LL5970: yyval=({ struct _tagged_ptr2 _temp5979= yyvs; struct
_xenum_struct** _temp5981= _temp5979.curr + yyvsp_offset; if( _temp5981 <
_temp5979.base? 1: _temp5981 >= _temp5979.last_plus_one){ _throw( Null_Exception);}*
_temp5981;}); break; default: _LL5978:( void) _throw(({ struct Cyc_Yyimpossible_struct*
_temp5983=( struct Cyc_Yyimpossible_struct*) GC_malloc_atomic( sizeof( struct
Cyc_Yyimpossible_struct));* _temp5983=( struct Cyc_Yyimpossible_struct){.tag=
Cyc_Yyimpossible_tag};( struct _xenum_struct*) _temp5983;}));} yyvsp_offset -=
yylen; yyssp_offset -= yylen; yylsp_offset -= yylen;({ struct _tagged_ptr2
_temp5984= yyvs; struct _xenum_struct** _temp5986= _temp5984.curr +( ++
yyvsp_offset); if( _temp5986 < _temp5984.base? 1: _temp5986 >= _temp5984.last_plus_one){
_throw( Null_Exception);}* _temp5986= yyval;}); yylsp_offset ++; if( yylen == 0){({
struct _tagged_ptr3 _temp5987= yyls; struct Cyc_Yyltype* _temp5989= _temp5987.curr
+ yylsp_offset; if( _temp5989 < _temp5987.base? 1: _temp5989 >= _temp5987.last_plus_one){
_throw( Null_Exception);}(* _temp5989).first_line= Cyc_yylloc.first_line;});({
struct _tagged_ptr3 _temp5990= yyls; struct Cyc_Yyltype* _temp5992= _temp5990.curr
+ yylsp_offset; if( _temp5992 < _temp5990.base? 1: _temp5992 >= _temp5990.last_plus_one){
_throw( Null_Exception);}(* _temp5992).first_column= Cyc_yylloc.first_column;});({
struct _tagged_ptr3 _temp5993= yyls; struct Cyc_Yyltype* _temp5995= _temp5993.curr
+ yylsp_offset; if( _temp5995 < _temp5993.base? 1: _temp5995 >= _temp5993.last_plus_one){
_throw( Null_Exception);}(* _temp5995).last_line=({ struct _tagged_ptr3
_temp5996= yyls; struct Cyc_Yyltype* _temp5998= _temp5996.curr +( yylsp_offset -
1); if( _temp5998 < _temp5996.base? 1: _temp5998 >= _temp5996.last_plus_one){
_throw( Null_Exception);}* _temp5998;}).last_line;});({ struct _tagged_ptr3
_temp5999= yyls; struct Cyc_Yyltype* _temp6001= _temp5999.curr + yylsp_offset;
if( _temp6001 < _temp5999.base? 1: _temp6001 >= _temp5999.last_plus_one){ _throw(
Null_Exception);}(* _temp6001).last_column=({ struct _tagged_ptr3 _temp6002=
yyls; struct Cyc_Yyltype* _temp6004= _temp6002.curr +( yylsp_offset - 1); if(
_temp6004 < _temp6002.base? 1: _temp6004 >= _temp6002.last_plus_one){ _throw(
Null_Exception);}* _temp6004;}).last_column;});({ struct _tagged_ptr3 _temp6005=
yyls; struct Cyc_Yyltype* _temp6007= _temp6005.curr + yylsp_offset; if(
_temp6007 < _temp6005.base? 1: _temp6007 >= _temp6005.last_plus_one){ _throw(
Null_Exception);}(* _temp6007).text=( struct _tagged_string)({ char* _temp6008=(
char*)""; struct _tagged_string _temp6009; _temp6009.curr= _temp6008; _temp6009.base=
_temp6008; _temp6009.last_plus_one= _temp6008 + 1; _temp6009;});});} else{({
struct _tagged_ptr3 _temp6010= yyls; struct Cyc_Yyltype* _temp6012= _temp6010.curr
+ yylsp_offset; if( _temp6012 < _temp6010.base? 1: _temp6012 >= _temp6010.last_plus_one){
_throw( Null_Exception);}(* _temp6012).last_line=({ struct _tagged_ptr3
_temp6013= yyls; struct Cyc_Yyltype* _temp6015= _temp6013.curr +(( yylsp_offset
+ yylen) - 1); if( _temp6015 < _temp6013.base? 1: _temp6015 >= _temp6013.last_plus_one){
_throw( Null_Exception);}* _temp6015;}).last_line;});({ struct _tagged_ptr3
_temp6016= yyls; struct Cyc_Yyltype* _temp6018= _temp6016.curr + yylsp_offset;
if( _temp6018 < _temp6016.base? 1: _temp6018 >= _temp6016.last_plus_one){ _throw(
Null_Exception);}(* _temp6018).last_column=({ struct _tagged_ptr3 _temp6019=
yyls; struct Cyc_Yyltype* _temp6021= _temp6019.curr +(( yylsp_offset + yylen) -
1); if( _temp6021 < _temp6019.base? 1: _temp6021 >= _temp6019.last_plus_one){
_throw( Null_Exception);}* _temp6021;}).last_column;});} yyn=( int)({ short*
_temp6022=( short*) Cyc_yyr1; unsigned int _temp6023= yyn; if( _temp6023 >= 368u){
_throw( Null_Exception);} _temp6022[ _temp6023];}); yystate=( int)({ short*
_temp6024=( short*) Cyc_yypgoto; unsigned int _temp6025= yyn - 117; if(
_temp6025 >= 98u){ _throw( Null_Exception);} _temp6024[ _temp6025];}) +( int)({
struct _tagged_ptr1 _temp6026= yyss; short* _temp6028= _temp6026.curr +
yyssp_offset; if( _temp6028 < _temp6026.base? 1: _temp6028 >= _temp6026.last_plus_one){
_throw( Null_Exception);}* _temp6028;}); if(( yystate >= 0? yystate <= 4229: 0)?(
int)({ short* _temp6029=( short*) Cyc_yycheck; unsigned int _temp6030= yystate;
if( _temp6030 >= 4230u){ _throw( Null_Exception);} _temp6029[ _temp6030];}) ==(
int)({ struct _tagged_ptr1 _temp6031= yyss; short* _temp6033= _temp6031.curr +
yyssp_offset; if( _temp6033 < _temp6031.base? 1: _temp6033 >= _temp6031.last_plus_one){
_throw( Null_Exception);}* _temp6033;}): 0){ yystate=( int)({ short* _temp6034=(
short*) Cyc_yytable; unsigned int _temp6035= yystate; if( _temp6035 >= 4230u){
_throw( Null_Exception);} _temp6034[ _temp6035];});} else{ yystate=( int)({
short* _temp6036=( short*) Cyc_yydefgoto; unsigned int _temp6037= yyn - 117; if(
_temp6037 >= 98u){ _throw( Null_Exception);} _temp6036[ _temp6037];});} goto
yynewstate; yyerrlab: if( yyerrstatus == 0){ ++ Cyc_yynerrs; yyn=( int)({ short*
_temp6038=( short*) Cyc_yypact; unsigned int _temp6039= yystate; if( _temp6039
>= 682u){ _throw( Null_Exception);} _temp6038[ _temp6039];}); if( yyn > - 32768?
yyn < 4229: 0){ int sze= 0; struct _tagged_string msg; int x; int count; count=
0; for( x= yyn < 0? - yyn: 0;( unsigned int) x < 215u / sizeof( char*); x ++){
if(( int)({ short* _temp6040=( short*) Cyc_yycheck; unsigned int _temp6041= x +
yyn; if( _temp6041 >= 4230u){ _throw( Null_Exception);} _temp6040[ _temp6041];})
== x){( unsigned int)( sze += Cyc_String_strlen(({ struct _tagged_string*
_temp6042=( struct _tagged_string*) Cyc_yytname; unsigned int _temp6043= x; if(
_temp6043 >= 215u){ _throw( Null_Exception);} _temp6042[ _temp6043];})) +(
unsigned int) 15), count ++;}} msg=({ unsigned int _temp6044=( unsigned int)(
sze + 15); char* _temp6045=( char*) GC_malloc_atomic( sizeof( char) * _temp6044);
unsigned int i; struct _tagged_string _temp6046={ _temp6045, _temp6045,
_temp6045 + _temp6044}; for( i= 0; i < _temp6044; i ++){ _temp6045[ i]='\000';}
_temp6046;}); Cyc_String_strcpy( msg,( struct _tagged_string)({ char* _temp6047=(
char*)"parse error"; struct _tagged_string _temp6048; _temp6048.curr= _temp6047;
_temp6048.base= _temp6047; _temp6048.last_plus_one= _temp6047 + 12; _temp6048;}));
if( count < 5){ count= 0; for( x= yyn < 0? - yyn: 0;( unsigned int) x < 215u /
sizeof( char*); x ++){ if(( int)({ short* _temp6049=( short*) Cyc_yycheck;
unsigned int _temp6050= x + yyn; if( _temp6050 >= 4230u){ _throw( Null_Exception);}
_temp6049[ _temp6050];}) == x){ Cyc_String_strcat( msg, count == 0?( struct
_tagged_string)({ char* _temp6051=( char*)", expecting `"; struct _tagged_string
_temp6052; _temp6052.curr= _temp6051; _temp6052.base= _temp6051; _temp6052.last_plus_one=
_temp6051 + 14; _temp6052;}):( struct _tagged_string)({ char* _temp6053=( char*)" or `";
struct _tagged_string _temp6054; _temp6054.curr= _temp6053; _temp6054.base=
_temp6053; _temp6054.last_plus_one= _temp6053 + 6; _temp6054;})); Cyc_String_strcat(
msg,({ struct _tagged_string* _temp6055=( struct _tagged_string*) Cyc_yytname;
unsigned int _temp6056= x; if( _temp6056 >= 215u){ _throw( Null_Exception);}
_temp6055[ _temp6056];})); Cyc_String_strcat( msg,( struct _tagged_string)({
char* _temp6057=( char*)"'"; struct _tagged_string _temp6058; _temp6058.curr=
_temp6057; _temp6058.base= _temp6057; _temp6058.last_plus_one= _temp6057 + 2;
_temp6058;})); count ++;}}} Cyc_yyerror( msg);} else{ Cyc_yyerror(( struct
_tagged_string)({ char* _temp6059=( char*)"parse error"; struct _tagged_string
_temp6060; _temp6060.curr= _temp6059; _temp6060.base= _temp6059; _temp6060.last_plus_one=
_temp6059 + 12; _temp6060;}));}} goto yyerrlab1; yyerrlab1: if( yyerrstatus == 3){
if( Cyc_yychar == 0){ return 1;} Cyc_yychar= - 2;} yyerrstatus= 3; goto
yyerrhandle; yyerrdefault: yyerrpop: if( yyssp_offset == 0){ return 1;}
yyvsp_offset --; yystate=( int)({ struct _tagged_ptr1 _temp6061= yyss; short*
_temp6063= _temp6061.curr +( -- yyssp_offset); if( _temp6063 < _temp6061.base? 1:
_temp6063 >= _temp6061.last_plus_one){ _throw( Null_Exception);}* _temp6063;});
yylsp_offset --; yyerrhandle: yyn=( int)({ short* _temp6064=( short*) Cyc_yypact;
unsigned int _temp6065= yystate; if( _temp6065 >= 682u){ _throw( Null_Exception);}
_temp6064[ _temp6065];}); if( yyn == - 32768){ goto yyerrdefault;} yyn += 1; if((
yyn < 0? 1: yyn > 4229)? 1:( int)({ short* _temp6066=( short*) Cyc_yycheck;
unsigned int _temp6067= yyn; if( _temp6067 >= 4230u){ _throw( Null_Exception);}
_temp6066[ _temp6067];}) != 1){ goto yyerrdefault;} yyn=( int)({ short*
_temp6068=( short*) Cyc_yytable; unsigned int _temp6069= yyn; if( _temp6069 >=
4230u){ _throw( Null_Exception);} _temp6068[ _temp6069];}); if( yyn < 0){ if(
yyn == - 32768){ goto yyerrpop;} yyn= - yyn; goto yyreduce;} else{ if( yyn == 0){
goto yyerrpop;}} if( yyn == 681){ return 0;}({ struct _tagged_ptr2 _temp6070=
yyvs; struct _xenum_struct** _temp6072= _temp6070.curr +( ++ yyvsp_offset); if(
_temp6072 < _temp6070.base? 1: _temp6072 >= _temp6070.last_plus_one){ _throw(
Null_Exception);}* _temp6072= Cyc_yylval;});({ struct _tagged_ptr3 _temp6073=
yyls; struct Cyc_Yyltype* _temp6075= _temp6073.curr +( ++ yylsp_offset); if(
_temp6075 < _temp6073.base? 1: _temp6075 >= _temp6073.last_plus_one){ _throw(
Null_Exception);}* _temp6075= Cyc_yylloc;}); goto yynewstate;} void Cyc_yyprint(
int i, struct _xenum_struct* v){ struct _xenum_struct* _temp6076= v; struct
_tuple11* _temp6096; struct _tuple11 _temp6098; int _temp6099; void* _temp6101;
char _temp6103; short _temp6105; struct _tagged_string _temp6107; struct Cyc_Core_Opt*
_temp6109; struct Cyc_Core_Opt* _temp6111; struct Cyc_Core_Opt _temp6113; struct
_tagged_string* _temp6114; struct _tuple1* _temp6116; struct _tuple1 _temp6118;
struct _tagged_string* _temp6119; void* _temp6121; _LL6078: if( _temp6076->tag
== Cyc_Okay_tok_tag){ goto _LL6079;} else{ goto _LL6080;} _LL6080: if((*((
struct _xenum_struct*) _temp6076)).tag == Cyc_Int_tok_tag){ _LL6097: _temp6096=((
struct Cyc_Int_tok_struct*) _temp6076)->f1; _temp6098=* _temp6096; _LL6102:
_temp6101= _temp6098.f1; goto _LL6100; _LL6100: _temp6099= _temp6098.f2; goto
_LL6081;} else{ goto _LL6082;} _LL6082: if((*(( struct _xenum_struct*) _temp6076)).tag
== Cyc_Char_tok_tag){ _LL6104: _temp6103=(( struct Cyc_Char_tok_struct*)
_temp6076)->f1; goto _LL6083;} else{ goto _LL6084;} _LL6084: if((*(( struct
_xenum_struct*) _temp6076)).tag == Cyc_Short_tok_tag){ _LL6106: _temp6105=((
struct Cyc_Short_tok_struct*) _temp6076)->f1; goto _LL6085;} else{ goto _LL6086;}
_LL6086: if((*(( struct _xenum_struct*) _temp6076)).tag == Cyc_String_tok_tag){
_LL6108: _temp6107=(( struct Cyc_String_tok_struct*) _temp6076)->f1; goto
_LL6087;} else{ goto _LL6088;} _LL6088: if((*(( struct _xenum_struct*) _temp6076)).tag
== Cyc_Stringopt_tok_tag){ _LL6110: _temp6109=(( struct Cyc_Stringopt_tok_struct*)
_temp6076)->f1; if( _temp6109 == 0){ goto _LL6089;} else{ goto _LL6090;}} else{
goto _LL6090;} _LL6090: if((*(( struct _xenum_struct*) _temp6076)).tag == Cyc_Stringopt_tok_tag){
_LL6112: _temp6111=(( struct Cyc_Stringopt_tok_struct*) _temp6076)->f1; if(
_temp6111 == 0){ goto _LL6092;} else{ _temp6113=* _temp6111; _LL6115: _temp6114=(
struct _tagged_string*) _temp6113.v; goto _LL6091;}} else{ goto _LL6092;}
_LL6092: if((*(( struct _xenum_struct*) _temp6076)).tag == Cyc_QualId_tok_tag){
_LL6117: _temp6116=(( struct Cyc_QualId_tok_struct*) _temp6076)->f1; _temp6118=*
_temp6116; _LL6122: _temp6121= _temp6118.f1; goto _LL6120; _LL6120: _temp6119=
_temp6118.f2; goto _LL6093;} else{ goto _LL6094;} _LL6094: goto _LL6095; _LL6079:
fprintf( Cyc_Stdio_stderr,"ok"); goto _LL6077; _LL6081: fprintf( Cyc_Stdio_stderr,"%d",
_temp6099); goto _LL6077; _LL6083: fprintf( Cyc_Stdio_stderr,"%c",( int)
_temp6103); goto _LL6077; _LL6085: fprintf( Cyc_Stdio_stderr,"%ds",( int)
_temp6105); goto _LL6077; _LL6087:({ struct _tagged_string _temp6123= _temp6107;
fprintf( Cyc_Stdio_stderr,"\"%.*s\"", _temp6123.last_plus_one - _temp6123.curr,
_temp6123.curr);}); goto _LL6077; _LL6089: fprintf( Cyc_Stdio_stderr,"null");
goto _LL6077; _LL6091:({ struct _tagged_string _temp6124=* _temp6114; fprintf(
Cyc_Stdio_stderr,"\"%.*s\"", _temp6124.last_plus_one - _temp6124.curr, _temp6124.curr);});
goto _LL6077; _LL6093: { struct Cyc_List_List* prefix= 0;{ void* _temp6125=
_temp6121; struct Cyc_List_List* _temp6133; struct Cyc_List_List* _temp6135;
_LL6127: if(( unsigned int) _temp6125 > 1u?(( struct _enum_struct*) _temp6125)->tag
== Cyc_Absyn_Rel_n_tag: 0){ _LL6134: _temp6133=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Rel_n_struct*) _temp6125)->f1; goto _LL6128;} else{ goto _LL6129;}
_LL6129: if(( unsigned int) _temp6125 > 1u?(( struct _enum_struct*) _temp6125)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL6136: _temp6135=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp6125)->f1; goto _LL6130;} else{ goto _LL6131;}
_LL6131: if( _temp6125 == Cyc_Absyn_Loc_n){ goto _LL6132;} else{ goto _LL6126;}
_LL6128: prefix= _temp6133; goto _LL6126; _LL6130: prefix= _temp6135; goto
_LL6126; _LL6132: goto _LL6126; _LL6126:;} for( 0; prefix != 0; prefix= prefix->tl){({
struct _tagged_string _temp6137=*(( struct _tagged_string*) prefix->hd); fprintf(
Cyc_Stdio_stderr,"%.*s::", _temp6137.last_plus_one - _temp6137.curr, _temp6137.curr);});}({
struct _tagged_string _temp6138=* _temp6119; fprintf( Cyc_Stdio_stderr,"%.*s::",
_temp6138.last_plus_one - _temp6138.curr, _temp6138.curr);}); goto _LL6077;}
_LL6095: fprintf( Cyc_Stdio_stderr,"?"); goto _LL6077; _LL6077:;} struct Cyc_List_List*
Cyc_Parse_parse_file( struct Cyc_Stdio___sFILE* f){ Cyc_Parse_parse_result= 0;
Cyc_Parse_lbuf=({ struct Cyc_Core_Opt* _temp6139=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp6139->v=( void*) Cyc_Lexing_from_file(
f); _temp6139;}); Cyc_yyparse(); return Cyc_Parse_parse_result;}