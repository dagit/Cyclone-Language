#include "cyc_include.h"

 struct _tagged_ptr0{ int* curr; int* base; int* last_plus_one; } ; struct
_tagged_ptr1{ void** curr; void** base; void** last_plus_one; } ; struct _tuple0{
void* f1; struct _tagged_string* f2; } ; struct _tuple1{ struct Cyc_Core_Opt* f1;
struct Cyc_Absyn_Tqual* f2; void* f3; } ; struct _tuple2{ struct Cyc_Absyn_Exp*
f1; struct Cyc_Absyn_Stmt* f2; } ; struct _tuple3{ void* f1; int f2; } ; struct
_tuple4{ struct Cyc_Declarator* f1; struct Cyc_Absyn_Exp* f2; } ; struct _tuple5{
struct Cyc_Absyn_Tqual* f1; struct Cyc_List_List* f2; } ; struct _tuple6{ struct
Cyc_List_List* f1; int f2; struct Cyc_Core_Opt* f3; } ; struct _tuple7{ struct
Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; struct _tuple8{ struct
_tagged_string f1; short f2; } ; struct _tuple9{ struct Cyc_Lex_Trie** f1; } ;
typedef unsigned int Cyc_uint; typedef char* Cyc_Cstring; typedef struct
_tagged_string Cyc_string; typedef struct _tagged_string Cyc_string_t; typedef
struct _tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void* exit(
int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; typedef struct Cyc_Core_Opt*
Cyc_Core_opt_t; extern struct _tagged_string Cyc_Core_new_string( int); extern
char Cyc_Core_InvalidArg_tag[ 11u]; struct Cyc_Core_InvalidArg_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Failure_tag[ 8u]; struct Cyc_Core_Failure_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Core_Impossible_tag[ 11u];
struct Cyc_Core_Impossible_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Core_Not_found_tag[ 10u]; struct Cyc_Core_Not_found_struct{ char*
tag; } ; extern char Cyc_Core_Unreachable_tag[ 12u]; struct Cyc_Core_Unreachable_struct{
char* tag; struct _tagged_string f1; } ; extern char* string_to_Cstring( struct
_tagged_string); extern char* underlying_Cstring( struct _tagged_string); extern
struct _tagged_string Cstring_to_string( char*); extern int system( char*);
struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
typedef unsigned int Cyc_Stdio_size_t; typedef int Cyc_Stdio_fpos_t; extern char
Cyc_Stdio_FileOpenError_tag[ 14u]; struct Cyc_Stdio_FileOpenError_struct{ char*
tag; struct _tagged_string f1; } ; extern char Cyc_Stdio_FileCloseError_tag[ 15u];
struct Cyc_Stdio_FileCloseError_struct{ char* tag; } ; struct Cyc_List_List{
void* hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List* Cyc_List_glist_t;
typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t;
extern char Cyc_List_List_empty_tag[ 11u]; struct Cyc_List_List_empty_struct{
char* tag; } ; extern char Cyc_List_List_mismatch_tag[ 14u]; struct Cyc_List_List_mismatch_struct{
char* tag; } ; extern struct Cyc_List_List* Cyc_List_imp_rev( struct Cyc_List_List*
x); extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x,
struct Cyc_List_List* y); extern char Cyc_List_Nth_tag[ 4u]; struct Cyc_List_Nth_struct{
char* tag; } ; extern char Cyc_Lexing_Error_tag[ 6u]; struct Cyc_Lexing_Error_struct{
char* tag; struct _tagged_string f1; } ; struct Cyc_Lexing_lexbuf{ void(*
refill_buff)( struct Cyc_Lexing_lexbuf*); void* refill_state; struct
_tagged_string lex_buffer; int lex_buffer_len; int lex_abs_pos; int
lex_start_pos; int lex_curr_pos; int lex_last_pos; int lex_last_action; int
lex_eof_reached; } ; typedef struct Cyc_Lexing_lexbuf* Cyc_Lexing_Lexbuf; struct
Cyc_Lexing_function_lexbuf_state{ int(* read_fun)( struct _tagged_string, int,
void*); void* read_fun_state; } ; typedef struct Cyc_Lexing_function_lexbuf_state*
Cyc_Lexing_Function_lexbuf_state; struct Cyc_Lexing_lex_tables{ struct
_tagged_ptr0 lex_base; struct _tagged_ptr0 lex_backtrk; struct _tagged_ptr0
lex_default; struct _tagged_ptr0 lex_trans; struct _tagged_ptr0 lex_check; } ;
typedef struct Cyc_Lexing_lex_tables* Cyc_Lexing_LexTables; extern struct
_tagged_string Cyc_Lexing_lexeme( struct Cyc_Lexing_lexbuf*); extern char Cyc_Lexing_lexeme_char(
struct Cyc_Lexing_lexbuf*, int); extern int Cyc_Lexing_lexeme_start( struct Cyc_Lexing_lexbuf*);
extern int Cyc_Lexing_lexeme_end( struct Cyc_Lexing_lexbuf*); extern int Cyc_Lexing_lex_engine(
struct Cyc_Lexing_lex_tables*, int, struct Cyc_Lexing_lexbuf*); struct Cyc_Set_Set;
typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct Cyc_Set_Set* Cyc_Set_hset_t;
typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern struct Cyc_Set_Set* Cyc_Set_empty(
int(* comp)( void*, void*)); extern struct Cyc_Set_Set* Cyc_Set_insert( struct
Cyc_Set_Set* s, void* elt); extern int Cyc_Set_member( struct Cyc_Set_Set* s,
void* elt); extern void Cyc_Set_iter( void(* f)( void*), struct Cyc_Set_Set* s);
extern char Cyc_Set_Absent_tag[ 7u]; struct Cyc_Set_Absent_struct{ char* tag; }
; extern unsigned int Cyc_String_strlen( struct _tagged_string s); extern int
Cyc_String_zstrptrcmp( struct _tagged_string*, struct _tagged_string*); extern
struct _tagged_string Cyc_String_str_sepstr( struct Cyc_List_List*, struct
_tagged_string); extern struct _tagged_string Cyc_String_zstrncpy( struct
_tagged_string, int, struct _tagged_string, int, unsigned int); extern struct
_tagged_string Cyc_String_substring( struct _tagged_string, int ofs,
unsigned int n); struct Cyc_Xarray_Xarray{ struct _tagged_ptr1 elmts; int
num_elmts; } ; typedef struct Cyc_Xarray_Xarray* Cyc_Xarray_xarray_t; extern
void* Cyc_Xarray_get( struct Cyc_Xarray_Xarray*, int); extern struct Cyc_Xarray_Xarray*
Cyc_Xarray_create( int, void*); extern int Cyc_Xarray_add_ind( struct Cyc_Xarray_Xarray*,
void*); struct Cyc_Lineno_Pos{ struct _tagged_string logical_file; struct
_tagged_string line; int line_no; int col; } ; typedef struct Cyc_Lineno_Pos*
Cyc_Lineno_pos_t; extern char Cyc_Position_Exit_tag[ 5u]; struct Cyc_Position_Exit_struct{
char* tag; } ; struct Cyc_Position_Segment; typedef struct Cyc_Position_Segment*
Cyc_Position_seg_t; extern struct Cyc_Position_Segment* Cyc_Position_segment_of_abs(
int, int); typedef void* Cyc_Position_Error_kind; extern void* Cyc_Position_Lex;
extern void* Cyc_Position_Parse; extern void* Cyc_Position_Elab; typedef void*
Cyc_Position_error_kind_t; struct Cyc_Position_Error{ struct _tagged_string
source; struct Cyc_Position_Segment* seg; void* kind; struct _tagged_string desc;
} ; typedef struct Cyc_Position_Error* Cyc_Position_error_t; extern struct Cyc_Position_Error*
Cyc_Position_mk_err_lex( struct Cyc_Position_Segment*, struct _tagged_string);
extern struct Cyc_Position_Error* Cyc_Position_mk_err_parse( struct Cyc_Position_Segment*,
struct _tagged_string); extern char Cyc_Position_Nocontext_tag[ 10u]; struct Cyc_Position_Nocontext_struct{
char* tag; } ; extern void Cyc_Position_post_error( struct Cyc_Position_Error*);
typedef struct _tagged_string* Cyc_Absyn_field_name; typedef struct
_tagged_string* Cyc_Absyn_var; typedef struct _tagged_string* Cyc_Absyn_tvarname_t;
typedef void* Cyc_Absyn_Nmspace; typedef struct _tuple0* Cyc_Absyn_qvar; typedef
struct _tuple0* Cyc_Absyn_qvar_opt_t; typedef struct _tuple0* Cyc_Absyn_typedef_name_t;
typedef struct _tuple0* Cyc_Absyn_typedef_name_opt_t; struct Cyc_Absyn_Tvar;
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
typedef void* Cyc_Absyn_Attribute; struct Cyc_Absyn_Structfield; typedef void*
Cyc_Absyn_scope; typedef struct Cyc_Absyn_Tqual* Cyc_Absyn_tqual; typedef void*
Cyc_Absyn_size_of_t; typedef void* Cyc_Absyn_kind_t; typedef struct Cyc_Absyn_Tvar*
Cyc_Absyn_tvar; typedef void* Cyc_Absyn_sign_t; typedef struct Cyc_Absyn_Conref*
Cyc_Absyn_conref; typedef void* Cyc_Absyn_constraint_t; typedef void* Cyc_Absyn_bounds_t;
typedef struct Cyc_Absyn_PtrInfo Cyc_Absyn_ptr_info_t; typedef struct Cyc_Absyn_FnInfo
Cyc_Absyn_fn_info_t; typedef void* Cyc_Absyn_typ; typedef void* Cyc_Absyn_funcparams_t;
typedef void* Cyc_Absyn_type_modifier; typedef void* Cyc_Absyn_cnst_t; typedef
void* Cyc_Absyn_primop; typedef void* Cyc_Absyn_incrementor_t; typedef void* Cyc_Absyn_raw_exp_t;
typedef struct Cyc_Absyn_Exp* Cyc_Absyn_exp; typedef struct Cyc_Absyn_Exp* Cyc_Absyn_exp_opt;
typedef void* Cyc_Absyn_raw_stmt_t; typedef struct Cyc_Absyn_Stmt* Cyc_Absyn_stmt;
typedef struct Cyc_Absyn_Stmt* Cyc_Absyn_stmt_opt; typedef void* Cyc_Absyn_raw_pat_t;
typedef struct Cyc_Absyn_Pat* Cyc_Absyn_pat; typedef void* Cyc_Absyn_binding_t;
typedef struct Cyc_Absyn_Switch_clause* Cyc_Absyn_switch_clause; typedef struct
Cyc_Absyn_Fndecl* Cyc_Absyn_fndecl; typedef struct Cyc_Absyn_Structdecl* Cyc_Absyn_structdecl;
typedef struct Cyc_Absyn_Enumfield* Cyc_Absyn_enumfield; typedef struct Cyc_Absyn_Enumdecl*
Cyc_Absyn_enumdecl; typedef struct Cyc_Absyn_Xenumdecl* Cyc_Absyn_xenumdecl;
typedef struct Cyc_Absyn_Typedefdecl* Cyc_Absyn_typedefdecl; typedef struct Cyc_Absyn_Vardecl*
Cyc_Absyn_vardecl; typedef void* Cyc_Absyn_raw_decl; typedef struct Cyc_Absyn_Decl*
Cyc_Absyn_decl; typedef void* Cyc_Absyn_designator; typedef struct _xenum_struct*
Cyc_Absyn_stmt_annot_t; typedef void* Cyc_Absyn_attribute_t; typedef struct Cyc_List_List*
Cyc_Absyn_attributes_t; typedef struct Cyc_Absyn_Structfield* Cyc_Absyn_structfield_t;
extern void* Cyc_Absyn_Loc_n; extern const int Cyc_Absyn_Rel_n_tag; struct Cyc_Absyn_Rel_n_struct{
int tag; struct Cyc_List_List* f1; } ; extern const int Cyc_Absyn_Abs_n_tag;
struct Cyc_Absyn_Abs_n_struct{ int tag; struct Cyc_List_List* f1; } ; extern
void* Cyc_Absyn_Static; extern void* Cyc_Absyn_Abstract; extern void* Cyc_Absyn_Public;
extern void* Cyc_Absyn_Extern; extern void* Cyc_Absyn_ExternC; struct Cyc_Absyn_Tqual{
int q_const; int q_volatile; int q_restrict; } ; extern void* Cyc_Absyn_B1;
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
struct Cyc_Absyn_EnumType_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Enumdecl** f3; } ; extern const int Cyc_Absyn_XenumType_tag;
struct Cyc_Absyn_XenumType_struct{ int tag; struct _tuple0* f1; struct Cyc_Absyn_Xenumdecl**
f2; } ; extern const int Cyc_Absyn_PointerType_tag; struct Cyc_Absyn_PointerType_struct{
int tag; struct Cyc_Absyn_PtrInfo f1; } ; extern const int Cyc_Absyn_IntType_tag;
struct Cyc_Absyn_IntType_struct{ int tag; void* f1; void* f2; } ; extern void*
Cyc_Absyn_FloatType; extern void* Cyc_Absyn_DoubleType; extern const int Cyc_Absyn_ArrayType_tag;
struct Cyc_Absyn_ArrayType_struct{ int tag; void* f1; struct Cyc_Absyn_Tqual* f2;
struct Cyc_Absyn_Exp* f3; } ; extern const int Cyc_Absyn_FnType_tag; struct Cyc_Absyn_FnType_struct{
int tag; struct Cyc_Absyn_FnInfo f1; } ; extern const int Cyc_Absyn_TupleType_tag;
struct Cyc_Absyn_TupleType_struct{ int tag; struct Cyc_List_List* f1; } ; extern
const int Cyc_Absyn_StructType_tag; struct Cyc_Absyn_StructType_struct{ int tag;
struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Structdecl** f3;
} ; extern void* Cyc_Absyn_UnionType; extern const int Cyc_Absyn_RgnHandleType_tag;
struct Cyc_Absyn_RgnHandleType_struct{ int tag; void* f1; } ; extern const int
Cyc_Absyn_TypedefType_tag; struct Cyc_Absyn_TypedefType_struct{ int tag; struct
_tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3; } ; extern void*
Cyc_Absyn_HeapRgn; extern const int Cyc_Absyn_AccessEff_tag; struct Cyc_Absyn_AccessEff_struct{
int tag; void* f1; } ; extern const int Cyc_Absyn_JoinEff_tag; struct Cyc_Absyn_JoinEff_struct{
int tag; struct Cyc_List_List* f1; } ; extern const int Cyc_Absyn_NoTypes_tag;
struct Cyc_Absyn_NoTypes_struct{ int tag; struct Cyc_List_List* f1; struct Cyc_Position_Segment*
f2; } ; extern const int Cyc_Absyn_WithTypes_tag; struct Cyc_Absyn_WithTypes_struct{
int tag; struct Cyc_List_List* f1; int f2; struct Cyc_Core_Opt* f3; } ; typedef
void* Cyc_Absyn_Pointer_Sort; extern const int Cyc_Absyn_NonNullable_ps_tag;
struct Cyc_Absyn_NonNullable_ps_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
extern const int Cyc_Absyn_Nullable_ps_tag; struct Cyc_Absyn_Nullable_ps_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; extern void* Cyc_Absyn_TaggedArray_ps;
extern const int Cyc_Absyn_Regparm_att_tag; struct Cyc_Absyn_Regparm_att_struct{
int tag; int f1; } ; extern void* Cyc_Absyn_Stdcall_att; extern void* Cyc_Absyn_Cdecl_att;
extern void* Cyc_Absyn_Noreturn_att; extern void* Cyc_Absyn_Const_att; extern
const int Cyc_Absyn_Aligned_att_tag; struct Cyc_Absyn_Aligned_att_struct{ int
tag; int f1; } ; extern void* Cyc_Absyn_Packed_att; extern const int Cyc_Absyn_Section_att_tag;
struct Cyc_Absyn_Section_att_struct{ int tag; struct _tagged_string f1; } ;
extern void* Cyc_Absyn_Nocommon_att; extern void* Cyc_Absyn_Shared_att; extern
void* Cyc_Absyn_Unused_att; extern void* Cyc_Absyn_Weak_att; extern void* Cyc_Absyn_Dllimport_att;
extern void* Cyc_Absyn_Dllexport_att; extern void* Cyc_Absyn_No_instrument_function_att;
extern void* Cyc_Absyn_Constructor_att; extern void* Cyc_Absyn_Destructor_att;
extern void* Cyc_Absyn_No_check_memory_usage_att; extern void* Cyc_Absyn_Carray_mod;
extern const int Cyc_Absyn_ConstArray_mod_tag; struct Cyc_Absyn_ConstArray_mod_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; extern const int Cyc_Absyn_Pointer_mod_tag;
struct Cyc_Absyn_Pointer_mod_struct{ int tag; void* f1; void* f2; struct Cyc_Absyn_Tqual*
f3; } ; extern const int Cyc_Absyn_Function_mod_tag; struct Cyc_Absyn_Function_mod_struct{
int tag; void* f1; } ; extern const int Cyc_Absyn_TypeParams_mod_tag; struct Cyc_Absyn_TypeParams_mod_struct{
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
Cyc_Absyn_String_c_struct{ int tag; int f1; struct _tagged_string f2; } ; extern
void* Cyc_Absyn_Null_c; extern void* Cyc_Absyn_Plus; extern void* Cyc_Absyn_Times;
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
Cyc_Absyn_Const_e_tag; struct Cyc_Absyn_Const_e_struct{ int tag; void* f1; } ;
extern const int Cyc_Absyn_Var_e_tag; struct Cyc_Absyn_Var_e_struct{ int tag;
struct _tuple0* f1; void* f2; } ; extern const int Cyc_Absyn_UnknownId_e_tag;
struct Cyc_Absyn_UnknownId_e_struct{ int tag; struct _tuple0* f1; } ; extern
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
int Cyc_Absyn_Sizeof_e_tag; struct Cyc_Absyn_Sizeof_e_struct{ int tag; void* f1;
} ; extern const int Cyc_Absyn_Deref_e_tag; struct Cyc_Absyn_Deref_e_struct{ int
tag; struct Cyc_Absyn_Exp* f1; } ; extern const int Cyc_Absyn_StructMember_e_tag;
struct Cyc_Absyn_StructMember_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct _tagged_string* f2; } ; extern const int Cyc_Absyn_StructArrow_e_tag;
struct Cyc_Absyn_StructArrow_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
_tagged_string* f2; } ; extern const int Cyc_Absyn_Subscript_e_tag; struct Cyc_Absyn_Subscript_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2; } ; extern const
int Cyc_Absyn_Tuple_e_tag; struct Cyc_Absyn_Tuple_e_struct{ int tag; struct Cyc_List_List*
f1; } ; extern const int Cyc_Absyn_CompoundLit_e_tag; struct Cyc_Absyn_CompoundLit_e_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; } ; extern const int Cyc_Absyn_Array_e_tag;
struct Cyc_Absyn_Array_e_struct{ int tag; int f1; struct Cyc_List_List* f2; } ;
extern const int Cyc_Absyn_Comprehension_e_tag; struct Cyc_Absyn_Comprehension_e_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp*
f3; } ; extern const int Cyc_Absyn_Struct_e_tag; struct Cyc_Absyn_Struct_e_struct{
int tag; struct _tuple0* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List* f3;
struct Cyc_Absyn_Structdecl* f4; } ; extern const int Cyc_Absyn_Enum_e_tag;
struct Cyc_Absyn_Enum_e_struct{ int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Enumdecl* f4; struct Cyc_Absyn_Enumfield*
f5; } ; extern const int Cyc_Absyn_Xenum_e_tag; struct Cyc_Absyn_Xenum_e_struct{
int tag; struct Cyc_Core_Opt* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Xenumdecl*
f3; struct Cyc_Absyn_Enumfield* f4; } ; extern const int Cyc_Absyn_UnresolvedMem_e_tag;
struct Cyc_Absyn_UnresolvedMem_e_struct{ int tag; struct Cyc_Core_Opt* f1;
struct Cyc_List_List* f2; } ; extern const int Cyc_Absyn_StmtExp_e_tag; struct
Cyc_Absyn_StmtExp_e_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; extern const
int Cyc_Absyn_Codegen_e_tag; struct Cyc_Absyn_Codegen_e_struct{ int tag; struct
Cyc_Absyn_Fndecl* f1; } ; extern const int Cyc_Absyn_Fill_e_tag; struct Cyc_Absyn_Fill_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; struct Cyc_Absyn_Exp{ struct Cyc_Core_Opt*
topt; void* r; struct Cyc_Position_Segment* loc; } ; extern void* Cyc_Absyn_Skip_s;
extern const int Cyc_Absyn_Exp_s_tag; struct Cyc_Absyn_Exp_s_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; extern const int Cyc_Absyn_Seq_s_tag; struct Cyc_Absyn_Seq_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_Absyn_Stmt* f2; } ; extern const
int Cyc_Absyn_Return_s_tag; struct Cyc_Absyn_Return_s_struct{ int tag; struct
Cyc_Absyn_Exp* f1; } ; extern const int Cyc_Absyn_IfThenElse_s_tag; struct Cyc_Absyn_IfThenElse_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; struct Cyc_Absyn_Stmt*
f3; } ; extern const int Cyc_Absyn_While_s_tag; struct Cyc_Absyn_While_s_struct{
int tag; struct _tuple2 f1; struct Cyc_Absyn_Stmt* f2; } ; extern const int Cyc_Absyn_Break_s_tag;
struct Cyc_Absyn_Break_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; extern
const int Cyc_Absyn_Continue_s_tag; struct Cyc_Absyn_Continue_s_struct{ int tag;
struct Cyc_Absyn_Stmt* f1; } ; extern const int Cyc_Absyn_Goto_s_tag; struct Cyc_Absyn_Goto_s_struct{
int tag; struct _tagged_string* f1; struct Cyc_Absyn_Stmt* f2; } ; extern const
int Cyc_Absyn_For_s_tag; struct Cyc_Absyn_For_s_struct{ int tag; struct Cyc_Absyn_Exp*
f1; struct _tuple2 f2; struct _tuple2 f3; struct Cyc_Absyn_Stmt* f4; } ; extern
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
struct Cyc_Absyn_Do_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; struct _tuple2
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
Cyc_Absyn_Enum_p_struct{ int tag; struct _tuple0* f1; struct Cyc_Core_Opt* f2;
struct Cyc_List_List* f3; struct Cyc_List_List* f4; struct Cyc_Absyn_Enumdecl*
f5; struct Cyc_Absyn_Enumfield* f6; } ; extern const int Cyc_Absyn_Xenum_p_tag;
struct Cyc_Absyn_Xenum_p_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Xenumdecl* f4; struct Cyc_Absyn_Enumfield*
f5; } ; extern const int Cyc_Absyn_UnknownId_p_tag; struct Cyc_Absyn_UnknownId_p_struct{
int tag; struct _tuple0* f1; } ; extern const int Cyc_Absyn_UnknownCall_p_tag;
struct Cyc_Absyn_UnknownCall_p_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; } ; extern const int Cyc_Absyn_UnknownFields_p_tag;
struct Cyc_Absyn_UnknownFields_p_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
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
void* sc; struct _tuple0* name; struct Cyc_Absyn_Tqual* tq; void* type; struct
Cyc_Absyn_Exp* initializer; int shadow; struct Cyc_Core_Opt* region; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc; int is_inline; struct _tuple0*
name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt* effect; void* ret_type;
struct Cyc_List_List* args; int varargs; struct Cyc_Absyn_Stmt* body; struct Cyc_Core_Opt*
cached_typ; struct Cyc_Core_Opt* param_vardecls; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Structfield{ struct _tagged_string* name;
struct Cyc_Absyn_Tqual* tq; void* type; struct Cyc_List_List* attributes; } ;
struct Cyc_Absyn_Structdecl{ void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List* attributes; } ; struct
Cyc_Absyn_Enumfield{ struct _tuple0* name; struct Cyc_Absyn_Exp* tag; struct Cyc_List_List*
tvs; struct Cyc_List_List* typs; struct Cyc_Position_Segment* loc; } ; struct
Cyc_Absyn_Enumdecl{ void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; } ; struct Cyc_Absyn_Xenumdecl{ void* sc;
struct _tuple0* name; struct Cyc_List_List* fields; } ; struct Cyc_Absyn_Typedefdecl{
struct _tuple0* name; struct Cyc_List_List* tvs; void* defn; } ; extern const
int Cyc_Absyn_Var_d_tag; struct Cyc_Absyn_Var_d_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; } ; extern const int Cyc_Absyn_Fn_d_tag; struct Cyc_Absyn_Fn_d_struct{ int
tag; struct Cyc_Absyn_Fndecl* f1; } ; extern const int Cyc_Absyn_Let_d_tag;
struct Cyc_Absyn_Let_d_struct{ int tag; struct Cyc_Absyn_Pat* f1; struct Cyc_Core_Opt*
f2; struct Cyc_Core_Opt* f3; struct Cyc_Absyn_Exp* f4; int f5; } ; extern const
int Cyc_Absyn_Struct_d_tag; struct Cyc_Absyn_Struct_d_struct{ int tag; struct
Cyc_Absyn_Structdecl* f1; } ; extern void* Cyc_Absyn_Union_d; extern const int
Cyc_Absyn_Enum_d_tag; struct Cyc_Absyn_Enum_d_struct{ int tag; struct Cyc_Absyn_Enumdecl*
f1; } ; extern const int Cyc_Absyn_Xenum_d_tag; struct Cyc_Absyn_Xenum_d_struct{
int tag; struct Cyc_Absyn_Xenumdecl* f1; } ; extern const int Cyc_Absyn_Typedef_d_tag;
struct Cyc_Absyn_Typedef_d_struct{ int tag; struct Cyc_Absyn_Typedefdecl* f1; }
; extern const int Cyc_Absyn_Namespace_d_tag; struct Cyc_Absyn_Namespace_d_struct{
int tag; struct _tagged_string* f1; struct Cyc_List_List* f2; } ; extern const
int Cyc_Absyn_Using_d_tag; struct Cyc_Absyn_Using_d_struct{ int tag; struct
_tuple0* f1; struct Cyc_List_List* f2; } ; extern const int Cyc_Absyn_ExternC_d_tag;
struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ; struct
Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; extern const int
Cyc_Absyn_ArrayElement_tag; struct Cyc_Absyn_ArrayElement_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; extern const int Cyc_Absyn_FieldName_tag; struct
Cyc_Absyn_FieldName_struct{ int tag; struct _tagged_string* f1; } ; extern char
Cyc_Absyn_EmptyAnnot_tag[ 11u]; struct Cyc_Absyn_EmptyAnnot_struct{ char* tag; }
; extern int Cyc_Absyn_varlist_cmp( struct Cyc_List_List*, struct Cyc_List_List*);
extern struct Cyc_Core_Opt* Cyc_Parse_lbuf; typedef void* Cyc_Struct_or_union;
typedef void* Cyc_struct_or_union_t; typedef void* Cyc_Blockitem; typedef void*
Cyc_blockitem_t; typedef void* Cyc_Type_specifier; typedef void* Cyc_type_specifier_t;
typedef void* Cyc_Storage_class; typedef void* Cyc_storage_class_t; struct Cyc_Declaration_spec;
typedef struct Cyc_Declaration_spec* Cyc_decl_spec_t; struct Cyc_Declarator;
typedef struct Cyc_Declarator* Cyc_declarator_t; struct Cyc_Abstractdeclarator;
typedef struct Cyc_Abstractdeclarator* Cyc_abstractdeclarator_t; typedef struct
_xenum_struct* Cyc_YYSTYPE; extern char Cyc_Okay_tok_tag[ 9u]; struct Cyc_Okay_tok_struct{
char* tag; } ; extern char Cyc_Int_tok_tag[ 8u]; struct Cyc_Int_tok_struct{ char*
tag; struct _tuple3* f1; } ; extern char Cyc_Char_tok_tag[ 9u]; struct Cyc_Char_tok_struct{
char* tag; char f1; } ; extern char Cyc_Pointer_Sort_tok_tag[ 17u]; struct Cyc_Pointer_Sort_tok_struct{
char* tag; void* f1; } ; extern char Cyc_Short_tok_tag[ 10u]; struct Cyc_Short_tok_struct{
char* tag; short f1; } ; extern char Cyc_String_tok_tag[ 11u]; struct Cyc_String_tok_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Stringopt_tok_tag[ 14u];
struct Cyc_Stringopt_tok_struct{ char* tag; struct Cyc_Core_Opt* f1; } ; extern
char Cyc_Type_tok_tag[ 9u]; struct Cyc_Type_tok_struct{ char* tag; void* f1; } ;
extern char Cyc_TypeList_tok_tag[ 13u]; struct Cyc_TypeList_tok_struct{ char*
tag; struct Cyc_List_List* f1; } ; extern char Cyc_Exp_tok_tag[ 8u]; struct Cyc_Exp_tok_struct{
char* tag; struct Cyc_Absyn_Exp* f1; } ; extern char Cyc_ExpList_tok_tag[ 12u];
struct Cyc_ExpList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ; extern
char Cyc_Primop_tok_tag[ 11u]; struct Cyc_Primop_tok_struct{ char* tag; void* f1;
} ; extern char Cyc_Primopopt_tok_tag[ 14u]; struct Cyc_Primopopt_tok_struct{
char* tag; struct Cyc_Core_Opt* f1; } ; extern char Cyc_QualId_tok_tag[ 11u];
struct Cyc_QualId_tok_struct{ char* tag; struct _tuple0* f1; } ; extern char Cyc_Stmt_tok_tag[
9u]; struct Cyc_Stmt_tok_struct{ char* tag; struct Cyc_Absyn_Stmt* f1; } ;
extern char Cyc_SwitchClauseList_tok_tag[ 21u]; struct Cyc_SwitchClauseList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; extern char Cyc_Pattern_tok_tag[ 12u];
struct Cyc_Pattern_tok_struct{ char* tag; struct Cyc_Absyn_Pat* f1; } ; extern
char Cyc_PatternList_tok_tag[ 16u]; struct Cyc_PatternList_tok_struct{ char* tag;
struct Cyc_List_List* f1; } ; extern char Cyc_FnDecl_tok_tag[ 11u]; struct Cyc_FnDecl_tok_struct{
char* tag; struct Cyc_Absyn_Fndecl* f1; } ; extern char Cyc_DeclList_tok_tag[ 13u];
struct Cyc_DeclList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ; extern
char Cyc_DeclSpec_tok_tag[ 13u]; struct Cyc_DeclSpec_tok_struct{ char* tag;
struct Cyc_Declaration_spec* f1; } ; extern char Cyc_InitDecl_tok_tag[ 13u];
struct Cyc_InitDecl_tok_struct{ char* tag; struct _tuple4* f1; } ; extern char
Cyc_InitDeclList_tok_tag[ 17u]; struct Cyc_InitDeclList_tok_struct{ char* tag;
struct Cyc_List_List* f1; } ; extern char Cyc_StorageClass_tok_tag[ 17u]; struct
Cyc_StorageClass_tok_struct{ char* tag; void* f1; } ; extern char Cyc_TypeSpecifier_tok_tag[
18u]; struct Cyc_TypeSpecifier_tok_struct{ char* tag; void* f1; } ; extern char
Cyc_QualSpecList_tok_tag[ 17u]; struct Cyc_QualSpecList_tok_struct{ char* tag;
struct _tuple5* f1; } ; extern char Cyc_TypeQual_tok_tag[ 13u]; struct Cyc_TypeQual_tok_struct{
char* tag; struct Cyc_Absyn_Tqual* f1; } ; extern char Cyc_StructFieldDeclList_tok_tag[
24u]; struct Cyc_StructFieldDeclList_tok_struct{ char* tag; struct Cyc_List_List*
f1; } ; extern char Cyc_StructFieldDeclListList_tok_tag[ 28u]; struct Cyc_StructFieldDeclListList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; extern char Cyc_Declarator_tok_tag[ 15u];
struct Cyc_Declarator_tok_struct{ char* tag; struct Cyc_Declarator* f1; } ;
extern char Cyc_DeclaratorList_tok_tag[ 19u]; struct Cyc_DeclaratorList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; extern char Cyc_AbstractDeclarator_tok_tag[
23u]; struct Cyc_AbstractDeclarator_tok_struct{ char* tag; struct Cyc_Abstractdeclarator*
f1; } ; extern char Cyc_EnumeratorField_tok_tag[ 20u]; struct Cyc_EnumeratorField_tok_struct{
char* tag; struct Cyc_Absyn_Enumfield* f1; } ; extern char Cyc_EnumeratorFieldList_tok_tag[
24u]; struct Cyc_EnumeratorFieldList_tok_struct{ char* tag; struct Cyc_List_List*
f1; } ; extern char Cyc_ParamDecl_tok_tag[ 14u]; struct Cyc_ParamDecl_tok_struct{
char* tag; struct _tuple1* f1; } ; extern char Cyc_ParamDeclList_tok_tag[ 18u];
struct Cyc_ParamDeclList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ;
extern char Cyc_ParamDeclListBool_tok_tag[ 22u]; struct Cyc_ParamDeclListBool_tok_struct{
char* tag; struct _tuple6* f1; } ; extern char Cyc_StructOrUnion_tok_tag[ 18u];
struct Cyc_StructOrUnion_tok_struct{ char* tag; void* f1; } ; extern char Cyc_IdList_tok_tag[
11u]; struct Cyc_IdList_tok_struct{ char* tag; struct Cyc_List_List* f1; } ;
extern char Cyc_Designator_tok_tag[ 15u]; struct Cyc_Designator_tok_struct{ char*
tag; void* f1; } ; extern char Cyc_DesignatorList_tok_tag[ 19u]; struct Cyc_DesignatorList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; extern char Cyc_TypeModifierList_tok_tag[
21u]; struct Cyc_TypeModifierList_tok_struct{ char* tag; struct Cyc_List_List*
f1; } ; extern char Cyc_Rgn_tok_tag[ 8u]; struct Cyc_Rgn_tok_struct{ char* tag;
void* f1; } ; extern char Cyc_InitializerList_tok_tag[ 20u]; struct Cyc_InitializerList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; extern char Cyc_FieldPattern_tok_tag[
17u]; struct Cyc_FieldPattern_tok_struct{ char* tag; struct _tuple7* f1; } ;
extern char Cyc_FieldPatternList_tok_tag[ 21u]; struct Cyc_FieldPatternList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; extern char Cyc_BlockItem_tok_tag[ 14u];
struct Cyc_BlockItem_tok_struct{ char* tag; void* f1; } ; extern char Cyc_Kind_tok_tag[
9u]; struct Cyc_Kind_tok_struct{ char* tag; void* f1; } ; extern char Cyc_Attribute_tok_tag[
14u]; struct Cyc_Attribute_tok_struct{ char* tag; void* f1; } ; extern char Cyc_AttributeList_tok_tag[
18u]; struct Cyc_AttributeList_tok_struct{ char* tag; struct Cyc_List_List* f1;
} ; struct Cyc_Yyltype{ int timestamp; int first_line; int first_column; int
last_line; int last_column; struct _tagged_string text; } ; typedef struct Cyc_Yyltype
Cyc_yyltype; extern struct Cyc_Yyltype Cyc_yylloc; extern struct _xenum_struct*
Cyc_yylval; struct Cyc_Dict_Dict; typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t;
typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t; extern char Cyc_Dict_Present_tag[
8u]; struct Cyc_Dict_Present_struct{ char* tag; } ; extern char Cyc_Dict_Absent_tag[
7u]; extern struct Cyc_Dict_Dict* Cyc_Dict_empty( int(* comp)( void*, void*));
extern struct Cyc_Dict_Dict* Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* key,
void* data); extern void* Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* key);
void Cyc_yyerror( struct _tagged_string s){ Cyc_Position_post_error( Cyc_Position_mk_err_parse(
Cyc_Position_segment_of_abs( Cyc_yylloc.first_line, Cyc_yylloc.last_line), s));}
struct Cyc_Lex_Trie; typedef struct Cyc_Lex_Trie* Cyc_Lex_trie_t; typedef struct
Cyc_Lex_Trie** Cyc_Lex_trie_child_v_t; typedef struct Cyc_Lex_Trie*** Cyc_Lex_trie_child_t;
struct Cyc_Lex_Trie{ struct Cyc_Lex_Trie*** children; int shared_str; } ; static
int Cyc_Lex_num_kws= 0; static struct _tagged_ptr0 Cyc_Lex_kw_nums={ 0, 0, 0};
static struct Cyc_Xarray_Xarray* Cyc_Lex_symbols= 0; static struct Cyc_Lex_Trie*
Cyc_Lex_ids_trie= 0; static struct Cyc_Lex_Trie* Cyc_Lex_typedefs_trie= 0;
static int Cyc_Lex_comment_depth= 0; static struct _tuple3 Cyc_Lex_token_int_pair=(
struct _tuple3){.f1=( void*) 0u,.f2= 0}; static char _temp2[ 8u]="*bogus*";
static struct _tagged_string Cyc_Lex_bogus_string=( struct _tagged_string){
_temp2, _temp2, _temp2 + 8u}; static struct Cyc_Absyn_Abs_n_struct _temp3={ 1u,
0}; static struct _tuple0 Cyc_Lex_token_id_pair=( struct _tuple0){.f1=( struct
Cyc_Absyn_Abs_n_struct*)& _temp3,.f2=& Cyc_Lex_bogus_string}; static char Cyc_Lex_token_char='\000';
static char _temp6[ 1u]=""; static struct _tagged_string Cyc_Lex_token_string=(
struct _tagged_string){ _temp6, _temp6, _temp6 + 1u}; static struct _tuple3* Cyc_Lex_token_int=&
Cyc_Lex_token_int_pair; static struct _tuple0* Cyc_Lex_token_qvar=& Cyc_Lex_token_id_pair;
static int Cyc_Lex_runaway_start= 0; static void Cyc_Lex_err( struct
_tagged_string msg, struct Cyc_Lexing_lexbuf* lb){ struct Cyc_Position_Segment*
s= Cyc_Position_segment_of_abs((( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lexeme_start)(
lb),(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lexeme_end)( lb)); Cyc_Position_post_error(
Cyc_Position_mk_err_lex( s, msg));} static void Cyc_Lex_runaway_err( struct
_tagged_string msg, struct Cyc_Lexing_lexbuf* lb){ struct Cyc_Position_Segment*
s= Cyc_Position_segment_of_abs( Cyc_Lex_runaway_start,(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lexing_lexeme_start)( lb)); Cyc_Position_post_error( Cyc_Position_mk_err_lex(
s, msg));} static char _temp9[ 14u]="__attribute__"; static char _temp12[ 9u]="abstract";
static char _temp15[ 5u]="auto"; static char _temp18[ 6u]="break"; static char
_temp21[ 5u]="case"; static char _temp24[ 6u]="catch"; static char _temp27[ 5u]="char";
static char _temp30[ 8u]="codegen"; static char _temp33[ 6u]="const"; static
char _temp36[ 9u]="continue"; static char _temp39[ 4u]="cut"; static char
_temp42[ 8u]="default"; static char _temp45[ 3u]="do"; static char _temp48[ 7u]="double";
static char _temp51[ 5u]="else"; static char _temp54[ 5u]="enum"; static char
_temp57[ 7u]="extern"; static char _temp60[ 9u]="fallthru"; static char _temp63[
5u]="fill"; static char _temp66[ 6u]="float"; static char _temp69[ 4u]="for";
static char _temp72[ 8u]="fprintf"; static char _temp75[ 7u]="fscanf"; static
char _temp78[ 5u]="goto"; static char _temp81[ 3u]="if"; static char _temp84[ 7u]="inline";
static char _temp87[ 4u]="int"; static char _temp90[ 4u]="let"; static char
_temp93[ 5u]="long"; static char _temp96[ 10u]="namespace"; static char _temp99[
4u]="new"; static char _temp102[ 5u]="null"; static char _temp105[ 7u]="printf";
static char _temp108[ 9u]="register"; static char _temp111[ 9u]="restrict";
static char _temp114[ 7u]="return"; static char _temp117[ 6u]="scanf"; static
char _temp120[ 6u]="short"; static char _temp123[ 7u]="signed"; static char
_temp126[ 7u]="sizeof"; static char _temp129[ 7u]="splice"; static char _temp132[
7u]="sscanf"; static char _temp135[ 7u]="static"; static char _temp138[ 7u]="struct";
static char _temp141[ 7u]="switch"; static char _temp144[ 6u]="throw"; static
char _temp147[ 4u]="try"; static char _temp150[ 8u]="typedef"; static char
_temp153[ 6u]="union"; static char _temp156[ 9u]="unsigned"; static char
_temp159[ 6u]="using"; static char _temp162[ 5u]="void"; static char _temp165[ 9u]="volatile";
static char _temp168[ 6u]="while"; static char _temp171[ 6u]="xenum"; static
char _temp174[ 8u]="xprintf"; static struct _tuple8 Cyc_Lex_rw_array[ 56u]={(
struct _tuple8){.f1=( struct _tagged_string){ _temp9, _temp9, _temp9 + 14u},.f2=
(short)345},( struct _tuple8){.f1=( struct _tagged_string){ _temp12, _temp12,
_temp12 + 9u},.f2= (short)298},( struct _tuple8){.f1=( struct _tagged_string){
_temp15, _temp15, _temp15 + 5u},.f2= (short)258},( struct _tuple8){.f1=( struct
_tagged_string){ _temp18, _temp18, _temp18 + 6u},.f2= (short)288},( struct
_tuple8){.f1=( struct _tagged_string){ _temp21, _temp21, _temp21 + 5u},.f2=
(short)277},( struct _tuple8){.f1=( struct _tagged_string){ _temp24, _temp24,
_temp24 + 6u},.f2= (short)296},( struct _tuple8){.f1=( struct _tagged_string){
_temp27, _temp27, _temp27 + 5u},.f2= (short)264},( struct _tuple8){.f1=( struct
_tagged_string){ _temp30, _temp30, _temp30 + 8u},.f2= (short)304},( struct
_tuple8){.f1=( struct _tagged_string){ _temp33, _temp33, _temp33 + 6u},.f2=
(short)272},( struct _tuple8){.f1=( struct _tagged_string){ _temp36, _temp36,
_temp36 + 9u},.f2= (short)287},( struct _tuple8){.f1=( struct _tagged_string){
_temp39, _temp39, _temp39 + 4u},.f2= (short)305},( struct _tuple8){.f1=( struct
_tagged_string){ _temp42, _temp42, _temp42 + 8u},.f2= (short)278},( struct
_tuple8){.f1=( struct _tagged_string){ _temp45, _temp45, _temp45 + 3u},.f2=
(short)284},( struct _tuple8){.f1=( struct _tagged_string){ _temp48, _temp48,
_temp48 + 7u},.f2= (short)269},( struct _tuple8){.f1=( struct _tagged_string){
_temp51, _temp51, _temp51 + 5u},.f2= (short)281},( struct _tuple8){.f1=( struct
_tagged_string){ _temp54, _temp54, _temp54 + 5u},.f2= (short)291},( struct
_tuple8){.f1=( struct _tagged_string){ _temp57, _temp57, _temp57 + 7u},.f2=
(short)261},( struct _tuple8){.f1=( struct _tagged_string){ _temp60, _temp60,
_temp60 + 9u},.f2= (short)299},( struct _tuple8){.f1=( struct _tagged_string){
_temp63, _temp63, _temp63 + 5u},.f2= (short)303},( struct _tuple8){.f1=( struct
_tagged_string){ _temp66, _temp66, _temp66 + 6u},.f2= (short)268},( struct
_tuple8){.f1=( struct _tagged_string){ _temp69, _temp69, _temp69 + 4u},.f2=
(short)285},( struct _tuple8){.f1=( struct _tagged_string){ _temp72, _temp72,
_temp72 + 8u},.f2= (short)308},( struct _tuple8){.f1=( struct _tagged_string){
_temp75, _temp75, _temp75 + 7u},.f2= (short)311},( struct _tuple8){.f1=( struct
_tagged_string){ _temp78, _temp78, _temp78 + 5u},.f2= (short)286},( struct
_tuple8){.f1=( struct _tagged_string){ _temp81, _temp81, _temp81 + 3u},.f2=
(short)280},( struct _tuple8){.f1=( struct _tagged_string){ _temp84, _temp84,
_temp84 + 7u},.f2= (short)279},( struct _tuple8){.f1=( struct _tagged_string){
_temp87, _temp87, _temp87 + 4u},.f2= (short)266},( struct _tuple8){.f1=( struct
_tagged_string){ _temp90, _temp90, _temp90 + 4u},.f2= (short)293},( struct
_tuple8){.f1=( struct _tagged_string){ _temp93, _temp93, _temp93 + 5u},.f2=
(short)267},( struct _tuple8){.f1=( struct _tagged_string){ _temp96, _temp96,
_temp96 + 10u},.f2= (short)301},( struct _tuple8){.f1=( struct _tagged_string){
_temp99, _temp99, _temp99 + 4u},.f2= (short)297},( struct _tuple8){.f1=( struct
_tagged_string){ _temp102, _temp102, _temp102 + 5u},.f2= (short)292},( struct
_tuple8){.f1=( struct _tagged_string){ _temp105, _temp105, _temp105 + 7u},.f2=
(short)307},( struct _tuple8){.f1=( struct _tagged_string){ _temp108, _temp108,
_temp108 + 9u},.f2= (short)259},( struct _tuple8){.f1=( struct _tagged_string){
_temp111, _temp111, _temp111 + 9u},.f2= (short)274},( struct _tuple8){.f1=(
struct _tagged_string){ _temp114, _temp114, _temp114 + 7u},.f2= (short)289},(
struct _tuple8){.f1=( struct _tagged_string){ _temp117, _temp117, _temp117 + 6u},.f2=
(short)310},( struct _tuple8){.f1=( struct _tagged_string){ _temp120, _temp120,
_temp120 + 6u},.f2= (short)265},( struct _tuple8){.f1=( struct _tagged_string){
_temp123, _temp123, _temp123 + 7u},.f2= (short)270},( struct _tuple8){.f1=(
struct _tagged_string){ _temp126, _temp126, _temp126 + 7u},.f2= (short)290},(
struct _tuple8){.f1=( struct _tagged_string){ _temp129, _temp129, _temp129 + 7u},.f2=
(short)306},( struct _tuple8){.f1=( struct _tagged_string){ _temp132, _temp132,
_temp132 + 7u},.f2= (short)312},( struct _tuple8){.f1=( struct _tagged_string){
_temp135, _temp135, _temp135 + 7u},.f2= (short)260},( struct _tuple8){.f1=(
struct _tagged_string){ _temp138, _temp138, _temp138 + 7u},.f2= (short)275},(
struct _tuple8){.f1=( struct _tagged_string){ _temp141, _temp141, _temp141 + 7u},.f2=
(short)282},( struct _tuple8){.f1=( struct _tagged_string){ _temp144, _temp144,
_temp144 + 6u},.f2= (short)294},( struct _tuple8){.f1=( struct _tagged_string){
_temp147, _temp147, _temp147 + 4u},.f2= (short)295},( struct _tuple8){.f1=(
struct _tagged_string){ _temp150, _temp150, _temp150 + 8u},.f2= (short)262},(
struct _tuple8){.f1=( struct _tagged_string){ _temp153, _temp153, _temp153 + 6u},.f2=
(short)276},( struct _tuple8){.f1=( struct _tagged_string){ _temp156, _temp156,
_temp156 + 9u},.f2= (short)271},( struct _tuple8){.f1=( struct _tagged_string){
_temp159, _temp159, _temp159 + 6u},.f2= (short)300},( struct _tuple8){.f1=(
struct _tagged_string){ _temp162, _temp162, _temp162 + 5u},.f2= (short)263},(
struct _tuple8){.f1=( struct _tagged_string){ _temp165, _temp165, _temp165 + 9u},.f2=
(short)273},( struct _tuple8){.f1=( struct _tagged_string){ _temp168, _temp168,
_temp168 + 6u},.f2= (short)283},( struct _tuple8){.f1=( struct _tagged_string){
_temp171, _temp171, _temp171 + 6u},.f2= (short)302},( struct _tuple8){.f1=(
struct _tagged_string){ _temp174, _temp174, _temp174 + 8u},.f2= (short)309}};
static int Cyc_Lex_str_index( struct _tagged_string buff, int offset, int len){
int i= offset; int last=( offset + len) - 1; struct Cyc_Lex_Trie* t= Cyc_Lex_ids_trie;
while( i <= last) { int ch=( int)({ struct _tagged_string _temp175= buff; char*
_temp177= _temp175.curr + i; if( _temp177 < _temp175.base? 1: _temp177 >=
_temp175.last_plus_one){ _throw( Null_Exception);}* _temp177;}) - 48; if( t->children
== 0){ while( i <= last) { t->children=( struct Cyc_Lex_Trie***)({ struct Cyc_Lex_Trie***
_temp178=( struct Cyc_Lex_Trie***) GC_malloc( sizeof( struct Cyc_Lex_Trie**) * 1);
_temp178[ 0]=( struct Cyc_Lex_Trie**)({ unsigned int _temp179= 75u; struct Cyc_Lex_Trie**
_temp180=( struct Cyc_Lex_Trie**) GC_malloc( sizeof( struct Cyc_Lex_Trie*) *
_temp179); unsigned int i; for( i= 0; i < _temp179; i ++){ _temp180[ i]= 0;}
_temp180;}); _temp178;});({ struct Cyc_Lex_Trie** _temp181=* t->children;
unsigned int _temp182= ch; if( _temp182 >= 75u){ _throw( Null_Exception);}
_temp181[ _temp182]=({ struct Cyc_Lex_Trie* _temp183=( struct Cyc_Lex_Trie*)
GC_malloc( sizeof( struct Cyc_Lex_Trie)); _temp183->children= 0; _temp183->shared_str=
- 1; _temp183;});}); t=({ struct Cyc_Lex_Trie** _temp184=* t->children;
unsigned int _temp185= ch; if( _temp185 >= 75u){ _throw( Null_Exception);}
_temp184[ _temp185];}); ++ i; ch=( int)({ struct _tagged_string _temp186= buff;
char* _temp188= _temp186.curr + i; if( _temp188 < _temp186.base? 1: _temp188 >=
_temp186.last_plus_one){ _throw( Null_Exception);}* _temp188;}) - 48;}{ struct
_tagged_string newstr= Cyc_Core_new_string( len + 1); Cyc_String_zstrncpy(
newstr, 0, buff, offset,( unsigned int) len);{ int ans=(( int(*)( struct Cyc_Xarray_Xarray*,
struct _tagged_string*)) Cyc_Xarray_add_ind)(( struct Cyc_Xarray_Xarray*)({
struct Cyc_Xarray_Xarray* _temp189= Cyc_Lex_symbols; if( _temp189 == 0){ _throw(
Null_Exception);} _temp189;}),({ struct _tagged_string* _temp190=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp190[ 0]=
newstr; _temp190;})); t->shared_str= ans; return ans;}}} if(({ struct Cyc_Lex_Trie**
_temp191=* t->children; unsigned int _temp192= ch; if( _temp192 >= 75u){ _throw(
Null_Exception);} _temp191[ _temp192];}) == 0){({ struct Cyc_Lex_Trie** _temp193=*
t->children; unsigned int _temp194= ch; if( _temp194 >= 75u){ _throw(
Null_Exception);} _temp193[ _temp194]=({ struct Cyc_Lex_Trie* _temp195=( struct
Cyc_Lex_Trie*) GC_malloc( sizeof( struct Cyc_Lex_Trie)); _temp195->children= 0;
_temp195->shared_str= - 1; _temp195;});});} t=({ struct Cyc_Lex_Trie** _temp196=*
t->children; unsigned int _temp197= ch; if( _temp197 >= 75u){ _throw(
Null_Exception);} _temp196[ _temp197];}); ++ i;} if( t->shared_str == - 1){
struct _tagged_string newstr= Cyc_Core_new_string( len + 1); Cyc_String_zstrncpy(
newstr, 0, buff, offset,( unsigned int) len);{ int ans=(( int(*)( struct Cyc_Xarray_Xarray*,
struct _tagged_string*)) Cyc_Xarray_add_ind)(( struct Cyc_Xarray_Xarray*)({
struct Cyc_Xarray_Xarray* _temp198= Cyc_Lex_symbols; if( _temp198 == 0){ _throw(
Null_Exception);} _temp198;}),({ struct _tagged_string* _temp199=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp199[ 0]=
newstr; _temp199;})); t->shared_str= ans; return ans;}} return t->shared_str;}
static int Cyc_Lex_str_index_lbuf( struct Cyc_Lexing_lexbuf* lbuf){ return Cyc_Lex_str_index(
lbuf->lex_buffer, lbuf->lex_start_pos, lbuf->lex_curr_pos - lbuf->lex_start_pos);}
static void Cyc_Lex_insert_typedef( struct _tagged_string* sptr){ struct
_tagged_string s=* sptr; int len=( int)(({ struct _tagged_string _temp200= s;(
unsigned int)( _temp200.last_plus_one - _temp200.curr);}) -( unsigned int) 1);
struct Cyc_Lex_Trie* t= Cyc_Lex_typedefs_trie;{ int i= 0; for( 0; i < len; ++ i){
int ch=( int)({ struct _tagged_string _temp201= s; char* _temp203= _temp201.curr
+ i; if( _temp203 < _temp201.base? 1: _temp203 >= _temp201.last_plus_one){
_throw( Null_Exception);}* _temp203;}) - 48; if( t->children == 0){ while( i <
len) { t->children=( struct Cyc_Lex_Trie***)({ struct _tuple9* _temp204=( struct
_tuple9*) GC_malloc( sizeof( struct _tuple9)); _temp204->f1=({ unsigned int
_temp205= 75u; struct Cyc_Lex_Trie** _temp206=( struct Cyc_Lex_Trie**) GC_malloc(
sizeof( struct Cyc_Lex_Trie*) * _temp205); unsigned int i; for( i= 0; i <
_temp205; i ++){ _temp206[ i]= 0;} _temp206;}); _temp204;});({ struct Cyc_Lex_Trie**
_temp207=* t->children; unsigned int _temp208= ch; if( _temp208 >= 75u){ _throw(
Null_Exception);} _temp207[ _temp208]=({ struct Cyc_Lex_Trie* _temp209=( struct
Cyc_Lex_Trie*) GC_malloc( sizeof( struct Cyc_Lex_Trie)); _temp209->children= 0;
_temp209->shared_str= 0; _temp209;});}); t=({ struct Cyc_Lex_Trie** _temp210=* t->children;
unsigned int _temp211= ch; if( _temp211 >= 75u){ _throw( Null_Exception);}
_temp210[ _temp211];}); ++ i; ch=( int)({ struct _tagged_string _temp212= s;
char* _temp214= _temp212.curr + i; if( _temp214 < _temp212.base? 1: _temp214 >=
_temp212.last_plus_one){ _throw( Null_Exception);}* _temp214;}) - 48;} t->shared_str=
1; return;} if(({ struct Cyc_Lex_Trie** _temp215=* t->children; unsigned int
_temp216= ch; if( _temp216 >= 75u){ _throw( Null_Exception);} _temp215[ _temp216];})
== 0){({ struct Cyc_Lex_Trie** _temp217=* t->children; unsigned int _temp218= ch;
if( _temp218 >= 75u){ _throw( Null_Exception);} _temp217[ _temp218]=({ struct
Cyc_Lex_Trie* _temp219=( struct Cyc_Lex_Trie*) GC_malloc( sizeof( struct Cyc_Lex_Trie));
_temp219->children= 0; _temp219->shared_str= 0; _temp219;});});} t=({ struct Cyc_Lex_Trie**
_temp220=* t->children; unsigned int _temp221= ch; if( _temp221 >= 75u){ _throw(
Null_Exception);} _temp220[ _temp221];});}} t->shared_str= 1; return;} static
struct _tagged_string* Cyc_Lex_get_symbol( int symbol_num){ return(( struct
_tagged_string*(*)( struct Cyc_Xarray_Xarray*, int)) Cyc_Xarray_get)(( struct
Cyc_Xarray_Xarray*)({ struct Cyc_Xarray_Xarray* _temp222= Cyc_Lex_symbols; if(
_temp222 == 0){ _throw( Null_Exception);} _temp222;}), symbol_num);} static int
Cyc_Lex_int_of_char( char c){ if(( int)'0' <=( int) c?( int) c <=( int)'9': 0){
return( int) c -( int)'0';} else{ if(( int)'a' <=( int) c?( int) c <=( int)'f':
0){ return( 10 +( int) c) -( int)'a';} else{ if(( int)'A' <=( int) c?( int) c <=(
int)'F': 0){ return( 10 +( int) c) -( int)'A';} else{( void) _throw(({ struct
Cyc_Core_InvalidArg_struct* _temp223=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));* _temp223=( struct Cyc_Core_InvalidArg_struct){.tag=
Cyc_Core_InvalidArg_tag,.f1=( struct _tagged_string)({ char* _temp224=( char*)"string to integer conversion";
struct _tagged_string _temp225; _temp225.curr= _temp224; _temp225.base= _temp224;
_temp225.last_plus_one= _temp224 + 29; _temp225;})};( struct _xenum_struct*)
_temp223;}));}}}} struct _tuple3* Cyc_Lex_intconst( struct Cyc_Lexing_lexbuf*
lbuf, int start, int base){ unsigned int n= 0; int end= lbuf->lex_curr_pos;
struct _tagged_string buff= lbuf->lex_buffer; int i= start + lbuf->lex_start_pos;{
int i= start + lbuf->lex_start_pos; for( 0; i < end; ++ i){ char c=({ struct
_tagged_string _temp226= buff; char* _temp228= _temp226.curr + i; if( _temp228 <
_temp226.base? 1: _temp228 >= _temp226.last_plus_one){ _throw( Null_Exception);}*
_temp228;}); switch( c){ case 'u': _LL229: goto _LL230; case 'U': _LL230: return({
struct _tuple3* _temp232=( struct _tuple3*) GC_malloc( sizeof( struct _tuple3) *
1); _temp232[ 0]=({ struct _tuple3 _temp233; _temp233.f1= Cyc_Absyn_Unsigned;
_temp233.f2=( int) n; _temp233;}); _temp232;}); case 'l': _LL231: break; case 'L':
_LL234: break; default: _LL235: n= n *( unsigned int) base +( unsigned int) Cyc_Lex_int_of_char(
c); break;}}} return({ struct _tuple3* _temp237=( struct _tuple3*) GC_malloc(
sizeof( struct _tuple3) * 1); _temp237[ 0]=({ struct _tuple3 _temp238; _temp238.f1=
Cyc_Absyn_Signed; _temp238.f2=( int) n; _temp238;}); _temp237;});} static char
Cyc_Lex_char_for_octal_code( struct Cyc_Lexing_lexbuf* lb, int start, int
howmany){ int c= 0;{ int i= 0; for( 0; i < howmany; i ++){ c=( 8 * c +( int)((
char(*)( struct Cyc_Lexing_lexbuf*, int)) Cyc_Lexing_lexeme_char)( lb, start + i))
- 48;}} return( char) c;} static char Cyc_Lex_char_for_hex_code( struct
_tagged_string s, int start){ int c= 0; int len=( int) Cyc_String_strlen( s);{
int i= 0; for( 0; start + i < len; i ++){ char x=({ struct _tagged_string
_temp239= s; char* _temp241= _temp239.curr +( start + i); if( _temp241 <
_temp239.base? 1: _temp241 >= _temp239.last_plus_one){ _throw( Null_Exception);}*
_temp241;}); if(( int)'0' <=( int) x?( int) x <=( int)'9': 0){ c=( 16 * c +( int)
x) -( int)'0';} else{ if(( int)'A' <=( int) x?( int) x <=( int)'F': 0){ c=( 16 *
c +( int) x) -( int)'A';} else{ if(( int)'a' <=( int) x?( int) x <=( int)'f': 0){
c=( 16 * c +( int) x) -( int)'a';} else{ break;}}}}} return( char) c;} static
char _temp244[ 11u]="xxxxxxxxxx"; struct _tagged_string Cyc_Lex_string_buffer=(
struct _tagged_string){ _temp244, _temp244, _temp244 + 11u}; int Cyc_Lex_string_pos=
0; void Cyc_Lex_store_string_char( char c){ int sz=( int)({ struct
_tagged_string _temp245= Cyc_Lex_string_buffer;( unsigned int)( _temp245.last_plus_one
- _temp245.curr);}); if( Cyc_Lex_string_pos >= sz){ int newsz= sz; while( Cyc_Lex_string_pos
>= newsz) { newsz= newsz * 2;}{ struct _tagged_string str=({ unsigned int
_temp249=( unsigned int) newsz; char* _temp250=( char*) GC_malloc_atomic(
sizeof( char) * _temp249); unsigned int i; struct _tagged_string _temp251={
_temp250, _temp250, _temp250 + _temp249}; for( i= 0; i < _temp249; i ++){
_temp250[ i]= i <( unsigned int) sz?({ struct _tagged_string _temp246= Cyc_Lex_string_buffer;
char* _temp248= _temp246.curr +( int) i; if( _temp248 < _temp246.base? 1:
_temp248 >= _temp246.last_plus_one){ _throw( Null_Exception);}* _temp248;}):'\000';}
_temp251;}); Cyc_Lex_string_buffer= str;}}({ struct _tagged_string _temp252= Cyc_Lex_string_buffer;
char* _temp254= _temp252.curr + Cyc_Lex_string_pos; if( _temp254 < _temp252.base?
1: _temp254 >= _temp252.last_plus_one){ _throw( Null_Exception);}* _temp254= c;});
++ Cyc_Lex_string_pos;} struct _tagged_string Cyc_Lex_get_stored_string(){
struct _tagged_string str= Cyc_String_substring( Cyc_Lex_string_buffer, 0,(
unsigned int) Cyc_Lex_string_pos); Cyc_Lex_string_pos= 0; return str;} struct
Cyc_Lex_Ldecls{ struct Cyc_Set_Set* typedefs; struct Cyc_Set_Set* namespaces; }
; typedef struct Cyc_Lex_Ldecls* Cyc_Lex_ldecls_t; struct Cyc_Lex_Lvis{ struct
Cyc_List_List* current_namespace; struct Cyc_List_List* imported_namespaces; } ;
typedef struct Cyc_Lex_Lvis* Cyc_Lex_lvis; struct Cyc_Lex_Lstate{ struct Cyc_List_List*
lstack; struct Cyc_Dict_Dict* decls; } ; typedef struct Cyc_Lex_Lstate* Cyc_Lex_lstate_t;
static struct Cyc_Core_Opt* Cyc_Lex_lstate= 0; static void Cyc_Lex_typedef_init(){
struct Cyc_Lex_Lvis* init_lvis=({ struct Cyc_Lex_Lvis* _temp255=( struct Cyc_Lex_Lvis*)
GC_malloc( sizeof( struct Cyc_Lex_Lvis)); _temp255->current_namespace= 0;
_temp255->imported_namespaces= 0; _temp255;}); struct Cyc_List_List* init_lstack=({
struct Cyc_List_List* _temp256=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp256->hd=( void*) init_lvis; _temp256->tl= 0;
_temp256;}); struct Cyc_Dict_Dict* init_decls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key, struct Cyc_Lex_Ldecls* data))
Cyc_Dict_insert)((( struct Cyc_Dict_Dict*(*)( int(* comp)( struct Cyc_List_List*,
struct Cyc_List_List*))) Cyc_Dict_empty)( Cyc_Absyn_varlist_cmp), 0,({ struct
Cyc_Lex_Ldecls* _temp257=( struct Cyc_Lex_Ldecls*) GC_malloc( sizeof( struct Cyc_Lex_Ldecls));
_temp257->typedefs=(( struct Cyc_Set_Set*(*)( int(* comp)( struct _tagged_string*,
struct _tagged_string*))) Cyc_Set_empty)( Cyc_String_zstrptrcmp); _temp257->namespaces=((
struct Cyc_Set_Set*(*)( int(* comp)( struct _tagged_string*, struct
_tagged_string*))) Cyc_Set_empty)( Cyc_String_zstrptrcmp); _temp257;})); Cyc_Lex_lstate=({
struct Cyc_Core_Opt* _temp258=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp258->v=( void*)({ struct Cyc_Lex_Lstate* _temp259=( struct
Cyc_Lex_Lstate*) GC_malloc( sizeof( struct Cyc_Lex_Lstate)); _temp259->lstack=
init_lstack; _temp259->decls= init_decls; _temp259;}); _temp258;});} static
struct Cyc_List_List* Cyc_Lex_get_absolute_namespace( struct Cyc_List_List* ns){
struct _tagged_string* n=( struct _tagged_string*) ns->hd;{ struct Cyc_List_List*
ls=( struct Cyc_List_List*)(( struct Cyc_Lex_Lstate*) Cyc_Lex_lstate->v)->lstack;
for( 0; ls != 0; ls= ls->tl){ struct Cyc_Lex_Lvis* lv=( struct Cyc_Lex_Lvis*) ls->hd;
struct Cyc_List_List* x=({ struct Cyc_List_List* _temp260=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp260->hd=( void*) lv->current_namespace;
_temp260->tl= lv->imported_namespaces; _temp260;}); for( 0; x != 0; x= x->tl){
struct Cyc_Lex_Ldecls* ld=(( struct Cyc_Lex_Ldecls*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key)) Cyc_Dict_lookup)((( struct Cyc_Lex_Lstate*) Cyc_Lex_lstate->v)->decls,(
struct Cyc_List_List*) x->hd); if((( int(*)( struct Cyc_Set_Set* s, struct
_tagged_string* elt)) Cyc_Set_member)( ld->namespaces, n)){ return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(( struct Cyc_List_List*)
x->hd,( struct Cyc_List_List*) ns);}}}} Cyc_yyerror(({ struct _tagged_string
_temp263= Cyc_String_str_sepstr(( struct Cyc_List_List*) ns,( struct
_tagged_string)({ char* _temp261=( char*)"::"; struct _tagged_string _temp262;
_temp262.curr= _temp261; _temp262.base= _temp261; _temp262.last_plus_one=
_temp261 + 3; _temp262;})); xprintf("undeclared namespace %.*s", _temp263.last_plus_one
- _temp263.curr, _temp263.curr);})); return 0;} static void Cyc_Lex_recompute_typedefs(){
Cyc_Lex_typedefs_trie=({ struct Cyc_Lex_Trie* _temp264=( struct Cyc_Lex_Trie*)
GC_malloc( sizeof( struct Cyc_Lex_Trie)); _temp264->children= 0; _temp264->shared_str=
0; _temp264;});{ struct Cyc_List_List* ls=( struct Cyc_List_List*)(( struct Cyc_Lex_Lstate*)
Cyc_Lex_lstate->v)->lstack; for( 0; ls != 0; ls= ls->tl){ struct Cyc_Lex_Lvis*
lv=( struct Cyc_Lex_Lvis*) ls->hd; struct Cyc_List_List* x=({ struct Cyc_List_List*
_temp265=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp265->hd=( void*) lv->current_namespace; _temp265->tl= lv->imported_namespaces;
_temp265;}); for( 0; x != 0; x= x->tl){ struct Cyc_Lex_Ldecls* ld=(( struct Cyc_Lex_Ldecls*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)((( struct
Cyc_Lex_Lstate*) Cyc_Lex_lstate->v)->decls,( struct Cyc_List_List*) x->hd);((
void(*)( void(* f)( struct _tagged_string*), struct Cyc_Set_Set* s)) Cyc_Set_iter)(
Cyc_Lex_insert_typedef, ld->typedefs);}}}} static int Cyc_Lex_is_typedef_in_namespace(
struct Cyc_List_List* ns, struct _tagged_string* v){ struct Cyc_List_List* ans=
Cyc_Lex_get_absolute_namespace( ns); struct _handler_cons _temp266;
_push_handler(& _temp266);{ struct _xenum_struct* _temp267=( struct
_xenum_struct*) setjmp( _temp266.handler); if( ! _temp267){{ struct Cyc_Lex_Ldecls*
ld=(( struct Cyc_Lex_Ldecls*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)((( struct Cyc_Lex_Lstate*) Cyc_Lex_lstate->v)->decls, ans);
int _temp268=(( int(*)( struct Cyc_Set_Set* s, struct _tagged_string* elt)) Cyc_Set_member)(
ld->typedefs, v); _npop_handler( 0u); return _temp268;}; _pop_handler();} else{
struct _xenum_struct* _temp270= _temp267; _LL272: if( _temp270->tag == Cyc_Dict_Absent_tag){
goto _LL273;} else{ goto _LL274;} _LL274: goto _LL275; _LL273: return 0; _LL275:(
void) _throw( _temp270); _LL271:;}}} static int Cyc_Lex_is_typedef( struct Cyc_List_List*
ns, struct _tagged_string* v){ if( ns != 0){ return Cyc_Lex_is_typedef_in_namespace((
struct Cyc_List_List*)({ struct Cyc_List_List* _temp276= ns; if( _temp276 == 0){
_throw( Null_Exception);} _temp276;}), v);}{ struct _tagged_string s=* v; int
len=( int)(({ struct _tagged_string _temp277= s;( unsigned int)( _temp277.last_plus_one
- _temp277.curr);}) -( unsigned int) 1); struct Cyc_Lex_Trie* t= Cyc_Lex_typedefs_trie;{
int i= 0; for( 0; i < len; ++ i){ if( t == 0? 1: t->children == 0){ return 0;}
else{ t=({ struct Cyc_Lex_Trie** _temp281=* t->children; unsigned int _temp282=(
int)({ struct _tagged_string _temp278= s; char* _temp280= _temp278.curr + i; if(
_temp280 < _temp278.base? 1: _temp280 >= _temp278.last_plus_one){ _throw(
Null_Exception);}* _temp280;}) - 48; if( _temp282 >= 75u){ _throw(
Null_Exception);} _temp281[ _temp282];});}}} if( t == 0){ return 0;} return t->shared_str;}}
void Cyc_Lex_enter_namespace( struct _tagged_string* s){ struct Cyc_List_List*
ns=(( struct Cyc_Lex_Lvis*)((( struct Cyc_Lex_Lstate*) Cyc_Lex_lstate->v)->lstack)->hd)->current_namespace;
struct Cyc_List_List* new_ns=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( ns,({ struct Cyc_List_List*
_temp283=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp283->hd=( void*) s; _temp283->tl= 0; _temp283;}));(( struct Cyc_Lex_Lstate*)
Cyc_Lex_lstate->v)->lstack=({ struct Cyc_List_List* _temp284=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp284->hd=( void*)({ struct Cyc_Lex_Lvis*
_temp285=( struct Cyc_Lex_Lvis*) GC_malloc( sizeof( struct Cyc_Lex_Lvis));
_temp285->current_namespace= new_ns; _temp285->imported_namespaces= 0; _temp285;});
_temp284->tl=( struct Cyc_List_List*)(( struct Cyc_Lex_Lstate*) Cyc_Lex_lstate->v)->lstack;
_temp284;});{ struct Cyc_Lex_Ldecls* ld=(( struct Cyc_Lex_Ldecls*(*)( struct Cyc_Dict_Dict*
d, struct Cyc_List_List* key)) Cyc_Dict_lookup)((( struct Cyc_Lex_Lstate*) Cyc_Lex_lstate->v)->decls,
ns); if( !(( int(*)( struct Cyc_Set_Set* s, struct _tagged_string* elt)) Cyc_Set_member)(
ld->namespaces, s)){(( struct Cyc_Lex_Lstate*) Cyc_Lex_lstate->v)->decls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key,
struct Cyc_Lex_Ldecls* data)) Cyc_Dict_insert)((( struct Cyc_Lex_Lstate*) Cyc_Lex_lstate->v)->decls,
ns,({ struct Cyc_Lex_Ldecls* _temp286=( struct Cyc_Lex_Ldecls*) GC_malloc(
sizeof( struct Cyc_Lex_Ldecls)); _temp286->typedefs= ld->typedefs; _temp286->namespaces=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tagged_string* elt)) Cyc_Set_insert)(
ld->namespaces, s); _temp286;}));(( struct Cyc_Lex_Lstate*) Cyc_Lex_lstate->v)->decls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key,
struct Cyc_Lex_Ldecls* data)) Cyc_Dict_insert)((( struct Cyc_Lex_Lstate*) Cyc_Lex_lstate->v)->decls,
new_ns,({ struct Cyc_Lex_Ldecls* _temp287=( struct Cyc_Lex_Ldecls*) GC_malloc(
sizeof( struct Cyc_Lex_Ldecls)); _temp287->typedefs=(( struct Cyc_Set_Set*(*)(
int(* comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Set_empty)(
Cyc_String_zstrptrcmp); _temp287->namespaces=(( struct Cyc_Set_Set*(*)( int(*
comp)( struct _tagged_string*, struct _tagged_string*))) Cyc_Set_empty)( Cyc_String_zstrptrcmp);
_temp287;}));} Cyc_Lex_recompute_typedefs();}} void Cyc_Lex_leave_namespace(){((
struct Cyc_Lex_Lstate*) Cyc_Lex_lstate->v)->lstack=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp288=((( struct Cyc_Lex_Lstate*) Cyc_Lex_lstate->v)->lstack)->tl;
if( _temp288 == 0){ _throw( Null_Exception);} _temp288;}); Cyc_Lex_recompute_typedefs();}
void Cyc_Lex_enter_using( struct _tuple0* q){ struct Cyc_List_List* ns;{ void*
_temp289=(* q).f1; struct Cyc_List_List* _temp297; struct Cyc_List_List*
_temp299; _LL291: if( _temp289 == Cyc_Absyn_Loc_n){ goto _LL292;} else{ goto
_LL293;} _LL293: if(( unsigned int) _temp289 > 1u?(( struct _enum_struct*)
_temp289)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL298: _temp297=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp289)->f1; goto _LL294;} else{ goto _LL295;}
_LL295: if(( unsigned int) _temp289 > 1u?(( struct _enum_struct*) _temp289)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL300: _temp299=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp289)->f1; goto _LL296;} else{ goto _LL290;} _LL292:
ns=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp301=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp301->hd=( void*)(* q).f2;
_temp301->tl= 0; _temp301;}); goto _LL290; _LL294: _temp299= _temp297; goto
_LL296; _LL296: ns=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp303=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp299,({ struct Cyc_List_List* _temp302=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp302->hd=( void*)(* q).f2; _temp302->tl= 0;
_temp302;})); if( _temp303 == 0){ _throw( Null_Exception);} _temp303;}); goto
_LL290; _LL290:;}{ struct Cyc_List_List* ans= Cyc_Lex_get_absolute_namespace( ns);
struct Cyc_List_List* imported=(( struct Cyc_Lex_Lvis*)((( struct Cyc_Lex_Lstate*)
Cyc_Lex_lstate->v)->lstack)->hd)->imported_namespaces;(( struct Cyc_Lex_Lvis*)(((
struct Cyc_Lex_Lstate*) Cyc_Lex_lstate->v)->lstack)->hd)->imported_namespaces=({
struct Cyc_List_List* _temp304=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp304->hd=( void*) ans; _temp304->tl= imported;
_temp304;}); Cyc_Lex_recompute_typedefs();}} void Cyc_Lex_leave_using(){ struct
Cyc_List_List* imported=(( struct Cyc_Lex_Lvis*)((( struct Cyc_Lex_Lstate*) Cyc_Lex_lstate->v)->lstack)->hd)->imported_namespaces;((
struct Cyc_Lex_Lvis*)((( struct Cyc_Lex_Lstate*) Cyc_Lex_lstate->v)->lstack)->hd)->imported_namespaces=
imported->tl; Cyc_Lex_recompute_typedefs();} void Cyc_Lex_register_typedef(
struct _tuple0* q){ struct Cyc_List_List* current=(( struct Cyc_Lex_Lvis*)(((
struct Cyc_Lex_Lstate*) Cyc_Lex_lstate->v)->lstack)->hd)->current_namespace;
struct Cyc_Dict_Dict* ds=(( struct Cyc_Lex_Lstate*) Cyc_Lex_lstate->v)->decls;
struct Cyc_Lex_Ldecls* ld=(( struct Cyc_Lex_Ldecls*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key)) Cyc_Dict_lookup)( ds, current); struct Cyc_Lex_Ldecls*
new_ld=({ struct Cyc_Lex_Ldecls* _temp305=( struct Cyc_Lex_Ldecls*) GC_malloc(
sizeof( struct Cyc_Lex_Ldecls)); _temp305->typedefs=(( struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set* s, struct _tagged_string* elt)) Cyc_Set_insert)( ld->typedefs,(*
q).f2); _temp305->namespaces= ld->namespaces; _temp305;});(( struct Cyc_Lex_Lstate*)
Cyc_Lex_lstate->v)->decls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct Cyc_List_List* key, struct Cyc_Lex_Ldecls* data)) Cyc_Dict_insert)( ds,
current, new_ld); Cyc_Lex_insert_typedef((* q).f2);} static short Cyc_Lex_process_id(
struct Cyc_Lexing_lexbuf* lbuf){ int symbol_num=(( int(*)( struct Cyc_Lexing_lexbuf*
lbuf)) Cyc_Lex_str_index_lbuf)( lbuf); if( symbol_num < Cyc_Lex_num_kws){ return(
short)({ struct _tagged_ptr0 _temp306= Cyc_Lex_kw_nums; int* _temp308= _temp306.curr
+ symbol_num; if( _temp308 < _temp306.base? 1: _temp308 >= _temp306.last_plus_one){
_throw( Null_Exception);}* _temp308;});}{ struct _tagged_string* s= Cyc_Lex_get_symbol(
symbol_num); if( Cyc_Lex_is_typedef( 0, s)){ Cyc_Lex_token_qvar=({ struct
_tuple0* _temp309=( struct _tuple0*) GC_malloc( sizeof( struct _tuple0));
_temp309->f1=({ struct Cyc_Absyn_Rel_n_struct* _temp310=( struct Cyc_Absyn_Rel_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct)); _temp310->tag= Cyc_Absyn_Rel_n_tag;
_temp310->f1= 0;( void*) _temp310;}); _temp309->f2= s; _temp309;}); return
(short)344;} Cyc_Lex_token_string=* s; return (short)337;}} static short Cyc_Lex_process_qual_id(
struct Cyc_Lexing_lexbuf* lbuf){ int i= lbuf->lex_start_pos; int end= lbuf->lex_curr_pos;
struct _tagged_string s= lbuf->lex_buffer; struct Cyc_List_List* rev_vs= 0;
while( i < end) { int start= i; for( 0; i < end?( int)({ struct _tagged_string
_temp311= s; char* _temp313= _temp311.curr + i; if( _temp313 < _temp311.base? 1:
_temp313 >= _temp311.last_plus_one){ _throw( Null_Exception);}* _temp313;}) !=(
int)':': 0; i ++){;} if( start == i){( void) _throw(({ struct Cyc_Core_Impossible_struct*
_temp314=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));*
_temp314=( struct Cyc_Core_Impossible_struct){.tag= Cyc_Core_Impossible_tag,.f1=(
struct _tagged_string)({ char* _temp315=( char*)"bad namespace"; struct
_tagged_string _temp316; _temp316.curr= _temp315; _temp316.base= _temp315;
_temp316.last_plus_one= _temp315 + 14; _temp316;})};( struct _xenum_struct*)
_temp314;}));}{ int vlen= i - start; struct _tagged_string* v= Cyc_Lex_get_symbol(
Cyc_Lex_str_index( s, start, vlen)); rev_vs=({ struct Cyc_List_List* _temp317=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp317->hd=(
void*) v; _temp317->tl= rev_vs; _temp317;}); i += 2;}} if( rev_vs == 0){( void)
_throw(({ struct Cyc_Core_Impossible_struct* _temp318=( struct Cyc_Core_Impossible_struct*)
GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));* _temp318=( struct Cyc_Core_Impossible_struct){.tag=
Cyc_Core_Impossible_tag,.f1=( struct _tagged_string)({ char* _temp319=( char*)"bad namespace";
struct _tagged_string _temp320; _temp320.curr= _temp319; _temp320.base= _temp319;
_temp320.last_plus_one= _temp319 + 14; _temp320;})};( struct _xenum_struct*)
_temp318;}));}{ struct _tagged_string* v=( struct _tagged_string*) rev_vs->hd;
struct Cyc_List_List* vs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( rev_vs->tl); Cyc_Lex_token_qvar=({ struct _tuple0* _temp321=(
struct _tuple0*) GC_malloc( sizeof( struct _tuple0)); _temp321->f1=({ struct Cyc_Absyn_Rel_n_struct*
_temp322=( struct Cyc_Absyn_Rel_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct));
_temp322->tag= Cyc_Absyn_Rel_n_tag; _temp322->f1= vs;( void*) _temp322;});
_temp321->f2= v; _temp321;}); if( Cyc_Lex_is_typedef( vs, v)){ return (short)344;}
else{ return (short)343;}}} extern int Cyc_Lex_token( struct Cyc_Lexing_lexbuf*);
extern int Cyc_Lex_strng( struct Cyc_Lexing_lexbuf*); extern int Cyc_Lex_comment(
struct Cyc_Lexing_lexbuf*); int Cyc_yylex(){ int ans=(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lex_token)(( struct Cyc_Lexing_lexbuf*) Cyc_Parse_lbuf->v); Cyc_yylloc.first_line=((
int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lexeme_start)(( struct Cyc_Lexing_lexbuf*)
Cyc_Parse_lbuf->v); Cyc_yylloc.last_line=(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lexing_lexeme_end)(( struct Cyc_Lexing_lexbuf*) Cyc_Parse_lbuf->v); switch(
ans){ case 337: _LL323: Cyc_yylval=({ struct Cyc_String_tok_struct* _temp325=(
struct Cyc_String_tok_struct*) GC_malloc( sizeof( struct Cyc_String_tok_struct));*
_temp325=( struct Cyc_String_tok_struct){.tag= Cyc_String_tok_tag,.f1= Cyc_Lex_token_string};(
struct _xenum_struct*) _temp325;}); break; case 343: _LL324: Cyc_yylval=({
struct Cyc_QualId_tok_struct* _temp327=( struct Cyc_QualId_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualId_tok_struct));* _temp327=( struct Cyc_QualId_tok_struct){.tag=
Cyc_QualId_tok_tag,.f1= Cyc_Lex_token_qvar};( struct _xenum_struct*) _temp327;});
break; case 344: _LL326: Cyc_yylval=({ struct Cyc_QualId_tok_struct* _temp329=(
struct Cyc_QualId_tok_struct*) GC_malloc( sizeof( struct Cyc_QualId_tok_struct));*
_temp329=( struct Cyc_QualId_tok_struct){.tag= Cyc_QualId_tok_tag,.f1= Cyc_Lex_token_qvar};(
struct _xenum_struct*) _temp329;}); break; case 342: _LL328: Cyc_yylval=({
struct Cyc_String_tok_struct* _temp331=( struct Cyc_String_tok_struct*)
GC_malloc( sizeof( struct Cyc_String_tok_struct));* _temp331=( struct Cyc_String_tok_struct){.tag=
Cyc_String_tok_tag,.f1= Cyc_Lex_token_string};( struct _xenum_struct*) _temp331;});
break; case 338: _LL330: Cyc_yylval=({ struct Cyc_Int_tok_struct* _temp333=(
struct Cyc_Int_tok_struct*) GC_malloc( sizeof( struct Cyc_Int_tok_struct));*
_temp333=( struct Cyc_Int_tok_struct){.tag= Cyc_Int_tok_tag,.f1= Cyc_Lex_token_int};(
struct _xenum_struct*) _temp333;}); break; case 340: _LL332: Cyc_yylval=({
struct Cyc_Char_tok_struct* _temp335=( struct Cyc_Char_tok_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Char_tok_struct));* _temp335=( struct Cyc_Char_tok_struct){.tag=
Cyc_Char_tok_tag,.f1= Cyc_Lex_token_char};( struct _xenum_struct*) _temp335;});
break; case 341: _LL334: Cyc_yylval=({ struct Cyc_String_tok_struct* _temp337=(
struct Cyc_String_tok_struct*) GC_malloc( sizeof( struct Cyc_String_tok_struct));*
_temp337=( struct Cyc_String_tok_struct){.tag= Cyc_String_tok_tag,.f1= Cyc_Lex_token_string};(
struct _xenum_struct*) _temp337;}); break; case 339: _LL336: Cyc_yylval=({
struct Cyc_String_tok_struct* _temp339=( struct Cyc_String_tok_struct*)
GC_malloc( sizeof( struct Cyc_String_tok_struct));* _temp339=( struct Cyc_String_tok_struct){.tag=
Cyc_String_tok_tag,.f1= Cyc_Lex_token_string};( struct _xenum_struct*) _temp339;});
break; default: _LL338: break;} return ans;} struct Cyc_Lexing_lex_tables* Cyc_Lex_lt=
0; int Cyc_Lex_lbase[ 151u]={ 0, 113, 17, 83, 16, 2, - 3, - 1, - 2, - 19, - 20,
4, 118, 119, - 21, 5, - 13, - 12, 85, - 14, - 11, - 4, - 5, - 6, - 7, - 8, - 9,
- 10, 209, 305, 111, - 15, 166, - 20, - 57, 8, 30, - 39, 12, 31, 116, 175, 32,
135, 141, 192, 145, 384, 427, 83, 82, 84, 87, 497, 85, 572, 647, 94, - 56, - 22,
- 28, 722, 797, 93, 855, 930, 98, - 23, 1005, 96, - 26, 119, - 31, - 25, - 34,
1080, 1109, 292, 96, 107, 394, 1119, 1149, 465, 1090, 1182, 1213, 1251, 103, 114,
1321, 1359, 106, 117, 109, 119, 114, 128, - 6, - 38, 13, - 37, 6, 138, 1291, -
33, - 16, - 18, - 32, - 17, - 19, 2, 1399, 150, 158, 413, 162, 168, 169, 170,
171, 173, 180, 185, 189, 1472, 1556, - 54, - 46, - 45, - 44, - 43, - 42, - 41, -
40, - 47, - 50, - 53, 488, - 52, 191, - 51, - 48, - 49, - 55, - 27, - 24, 14, -
35, 18, 477}; int Cyc_Lex_lbacktrk[ 151u]={ - 1, - 1, - 1, 5, 3, 4, - 1, - 1, -
1, - 1, - 1, 18, 1, 21, - 1, 2, - 1, - 1, 16, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, 17, 15, - 1, - 1, - 1, - 1, 35, 56, - 1, 56, 56, 56, 56, 56, 56,
56, 56, 56, 5, 7, 56, 56, 56, 56, 0, 56, 56, 56, 56, - 1, - 1, - 1, 3, 1, - 1, -
1, 2, - 1, - 1, 0, 29, - 1, 28, - 1, - 1, - 1, 9, 7, - 1, 7, 7, - 1, 8, 9, - 1,
- 1, 9, 5, 6, 5, 5, - 1, 4, 4, 4, 6, 6, 5, 5, - 1, - 1, - 1, - 1, 36, - 1, 9, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 34, 35}; int Cyc_Lex_ldefault[
151u]={ 34, 9, 3, 3, - 1, - 1, 0, 0, 0, 0, 0, - 1, - 1, - 1, 0, - 1, 0, 0, - 1,
0, 0, 0, 0, 0, 0, 0, 0, 0, - 1, - 1, - 1, 0, - 1, 0, 0, - 1, - 1, 0, 147, - 1, -
1, 111, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, 0, 0, 0, - 1, - 1, - 1, - 1, - 1, - 1, 0, - 1, - 1, 0, - 1, 0, 0, 0, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, 0, 0, 100, 0, - 1, - 1, - 1, 0, 0, 0, 0, 0, 0, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, - 1, 0, - 1, 0, 0, 0, 0, 0, 0, 147, 0, - 1, - 1}; int
Cyc_Lex_ltrans[ 1813u]={ 0, 0, 0, 0, 0, 0, 0, 0, 0, 35, 35, 35, 35, 35, 33, 6,
101, 150, 150, 150, 150, 150, 148, 101, 148, 149, 102, 149, 148, 0, 0, 0, 35, 36,
37, 38, 0, 39, 40, 41, 150, 144, 42, 43, 7, 44, 45, 46, 47, 48, 48, 48, 48, 48,
48, 48, 48, 48, 49, 4, 50, 51, 52, 8, 5, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 17, 14, 110, 54,
55, 56, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
53, 53, 53, 53, 53, 53, 53, 53, 10, 57, - 1, 11, 32, 32, 6, - 1, 32, 15, 30, 30,
30, 30, 30, 30, 30, 30, 74, 71, 16, 72, 20, 67, 12, 19, 69, 32, 64, 7, 16, 145,
59, 64, 70, 17, 31, 31, 31, 31, 31, 31, 31, 31, 18, 18, 18, 18, 18, 18, 18, 18,
32, 32, 146, 31, 32, 73, 25, 19, 25, 105, - 1, 106, 99, 23, 143, 23, 22, 100, 22,
24, 24, 109, 142, 32, 98, 7, 136, 107, 108, 98, 13, 33, 135, 134, 133, 132, 20,
131, 25, - 1, 25, 21, 22, 60, 130, 23, 23, 23, 22, 129, 22, 24, 24, 128, 24, 141,
98, 0, 25, 0, 26, 98, 27, 103, 28, 104, 104, 104, 104, 104, 104, 104, 104, 104,
104, 0, 0, 0, 0, 0, 0, 58, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 112, - 1, - 1,
- 1, 0, 0, 6, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
29, 29, 29, 29, 29, 29, 29, 29, 29, 0, 0, 0, 0, 0, 0, 29, 29, 29, 29, 29, 29, 29,
29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 0, 0,
0, 80, 0, 80, 0, - 1, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 0, 0, 0, 29, 29,
29, 29, 29, 29, 29, 29, 29, 29, 0, 0, 0, 0, 0, 0, 14, 29, 29, 29, 29, 29, 29, 29,
29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 0, 0,
0, 0, 0, 0, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
29, 29, 29, 29, 29, 29, 29, 29, 29, 0, 0, 75, - 1, 86, 86, 86, 86, 86, 86, 86,
86, 87, 87, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 137, 77, 0, 0, 0, 0, 0, 0,
88, 138, 138, 138, 138, 138, 138, 138, 138, 89, 0, 0, 90, 75, 0, 76, 76, 76, 76,
76, 76, 76, 76, 76, 76, 77, 150, 150, 150, 150, 150, 0, 88, 0, 0, 0, 77, 0, 0, 0,
0, 89, 0, 78, 90, 0, 0, 0, 84, 150, 84, 0, 79, 85, 85, 85, 85, 85, 85, 85, 85,
85, 85, 0, 0, 0, 0, 139, 77, 0, 0, 0, 0, 0, 0, 78, 140, 140, 140, 140, 140, 140,
140, 140, 79, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 63, 0, 0, 0, 0, 0, 0, 68,
68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
68, 68, 68, 68, 68, 0, 0, 0, 0, 68, 0, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 62, 62, 62, 62,
62, 62, 62, 62, 62, 62, 63, 0, 0, 0, 0, 0, 0, 62, 62, 62, 62, 62, 62, 62, 62, 62,
62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 0, 0, 0, 0,
62, 0, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
62, 62, 62, 62, 62, 62, 62, 62, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 0, 0, 0,
0, 0, 0, 0, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
61, 61, 61, 61, 61, 61, 61, 61, 61, 0, 0, 0, 0, 61, 0, 61, 61, 61, 61, 61, 61,
61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 0, 0, 0, 0, 0, 0, 0, 61, 61, 61, 61, 61,
61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
61, 0, 0, 0, 0, 61, 0, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 62, 62, 62, 62, 62, 62, 62, 62,
62, 62, 63, 0, 0, 0, 0, 0, 0, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 0, 0, 0, 0, 62, 0, 62, 62,
62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
62, 62, 62, 62, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 0, 0, 0, 0, 65, 0, 65, 65, 65, 65, 65,
65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 66, 0, 0, 0, 0, 0, 0, 65, 65, 65, 65,
65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
65, 65, 0, 0, 0, 0, 65, 0, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 68, 68, 68, 68, 68, 68, 68,
68, 68, 68, 63, 0, 0, 0, 0, 0, 0, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 0, 0, 0, 0, 68, 0, 68,
68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
68, 68, 68, 68, 68, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82, 85, 85, 85, 85, 85,
85, 85, 85, 85, 85, 0, 83, 27, 0, 0, 0, 0, 75, 27, 76, 76, 76, 76, 76, 76, 76,
76, 76, 76, 81, 81, 81, 81, 81, 81, 81, 81, 81, 81, 0, 77, 0, 0, 83, 27, 0, 0,
78, 0, 0, 27, 26, 0, 0, 0, 0, 79, 26, 0, 82, 82, 82, 82, 82, 82, 82, 82, 82, 82,
0, 0, 0, 77, 0, 0, 0, 0, 0, 0, 78, 83, 27, 0, 26, 0, 0, 0, 27, 79, 26, 0, 0, 85,
85, 85, 85, 85, 85, 85, 85, 85, 85, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 83, 27, 27, 0,
0, 0, 0, 27, 27, 75, 0, 86, 86, 86, 86, 86, 86, 86, 86, 87, 87, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 77, 0, 27, 0, 0, 0, 0, 96, 27, 0, 0, 0, 0, 0, 0, 75, 97, 87, 87,
87, 87, 87, 87, 87, 87, 87, 87, 0, 0, 0, 0, 0, 77, 0, 0, 0, 0, 0, 77, 96, 0, 0,
0, 0, 0, 94, 0, 0, 97, 0, 0, 0, 0, 0, 95, 0, 0, 104, 104, 104, 104, 104, 104,
104, 104, 104, 104, 0, 0, 0, 77, 0, 0, 0, 0, 0, 0, 94, 83, 27, 0, 0, 0, 0, 0, 27,
95, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 0, 0, 0, 0, 0, 0, 0, 91, 91, 91, 91,
91, 91, 83, 27, 0, 0, 0, 0, 0, 27, 0, 0, 0, 0, 0, 0, 0, 91, 91, 91, 91, 91, 91,
91, 91, 91, 91, 0, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 91, 0, 0, 0, 113,
0, 92, 0, 0, 114, 0, 0, 0, 0, 0, 93, 0, 0, 115, 115, 115, 115, 115, 115, 115,
115, 0, 91, 91, 91, 91, 91, 91, 116, 0, 0, 0, 0, 92, 0, 0, 0, 0, 0, 0, 0, 0, 93,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 117, 0, 0, 0, 0, 118, 119, 0, 0, 0,
120, 0, 0, 0, 0, 0, 0, 0, 121, 0, 0, 0, 122, 0, 123, 0, 124, 0, 125, 126, 126,
126, 126, 126, 126, 126, 126, 126, 126, 0, 0, 0, 0, 0, 0, 0, 126, 126, 126, 126,
126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126,
126, 126, 126, 126, 126, 126, 0, 0, 0, 0, 0, 0, 126, 126, 126, 126, 126, 126,
126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126,
126, 126, 126, 126, 127, 0, 0, 0, 0, 0, 0, 0, 0, 126, 126, 126, 126, 126, 126,
126, 126, 126, 126, 0, 0, 0, 0, 0, 0, 0, 126, 126, 126, 126, 126, 126, 126, 126,
126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126,
126, 126, 0, 0, 0, 0, 0, 0, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126,
126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
int Cyc_Lex_lcheck[ 1813u]={ - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 0, 0,
0, 0, 0, 11, 15, 102, 35, 35, 35, 35, 35, 38, 100, 147, 38, 100, 147, 149, - 1,
- 1, - 1, 0, 0, 0, 0, - 1, 0, 0, 0, 35, 111, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 2, 0, 0, 0, 4, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 36, 39, 42, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 3, 1, 12, 12, 13, 3, 12,
13, 18, 18, 18, 18, 18, 18, 18, 18, 49, 50, 50, 50, 51, 54, 1, 52, 52, 12, 63,
12, 13, 40, 57, 66, 69, 13, 30, 30, 30, 30, 30, 30, 30, 30, 13, 13, 13, 13, 13,
13, 13, 13, 32, 32, 40, 43, 32, 71, 78, 13, 79, 103, 41, 44, 46, 88, 113, 89, 92,
46, 93, 94, 95, 43, 114, 32, 96, 32, 116, 44, 44, 97, 1, 46, 117, 118, 119, 120,
13, 121, 78, 41, 79, 13, 13, 57, 122, 88, 13, 89, 92, 123, 93, 94, 95, 124, 13,
140, 96, - 1, 13, - 1, 13, 97, 13, 45, 13, 45, 45, 45, 45, 45, 45, 45, 45, 45,
45, - 1, - 1, - 1, - 1, - 1, - 1, 0, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 41,
38, 100, 147, - 1, - 1, 2, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, - 1, - 1, - 1, - 1, - 1, - 1,
28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
28, 28, 28, 28, 28, 28, - 1, - 1, - 1, 77, - 1, 77, - 1, 3, 77, 77, 77, 77, 77,
77, 77, 77, 77, 77, - 1, - 1, - 1, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, - 1,
- 1, - 1, - 1, - 1, - 1, 1, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, - 1, - 1, - 1, - 1, - 1, - 1,
29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
29, 29, 29, 29, 29, 29, - 1, - 1, 47, 41, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
80, 80, 80, 80, 80, 80, 80, 80, 80, 80, 115, 47, - 1, - 1, - 1, - 1, - 1, - 1,
47, 115, 115, 115, 115, 115, 115, 115, 115, 47, - 1, - 1, 47, 48, - 1, 48, 48,
48, 48, 48, 48, 48, 48, 48, 48, 47, 150, 150, 150, 150, 150, - 1, 47, - 1, - 1,
- 1, 48, - 1, - 1, - 1, - 1, 47, - 1, 48, 47, - 1, - 1, - 1, 83, 150, 83, - 1,
48, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, - 1, - 1, - 1, - 1, 138, 48, - 1, -
1, - 1, - 1, - 1, - 1, 48, 138, 138, 138, 138, 138, 138, 138, 138, 48, 53, 53,
53, 53, 53, 53, 53, 53, 53, 53, 53, - 1, - 1, - 1, - 1, - 1, - 1, 53, 53, 53, 53,
53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
53, 53, - 1, - 1, - 1, - 1, 53, - 1, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 55, 55, 55, 55, 55,
55, 55, 55, 55, 55, 55, - 1, - 1, - 1, - 1, - 1, - 1, 55, 55, 55, 55, 55, 55, 55,
55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, - 1,
- 1, - 1, - 1, 55, - 1, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55,
55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 56, 56, 56, 56, 56, 56, 56, 56,
56, 56, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 56, 56, 56, 56, 56, 56, 56, 56, 56,
56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, - 1, - 1, -
1, - 1, 56, - 1, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, - 1, - 1, - 1, - 1,
61, - 1, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61, 61,
61, 61, 61, 61, 61, 61, 61, 61, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, - 1,
- 1, - 1, - 1, - 1, - 1, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, - 1, - 1, - 1, - 1, 62, - 1, 62,
62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62, 62,
62, 62, 62, 62, 62, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, - 1, - 1, - 1, - 1, 64, - 1, 64, 64,
64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
64, 64, 64, 64, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, - 1, - 1, - 1, - 1,
- 1, - 1, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
65, 65, 65, 65, 65, 65, 65, 65, - 1, - 1, - 1, - 1, 65, - 1, 65, 65, 65, 65, 65,
65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65, 65,
65, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, - 1, - 1, - 1, - 1, - 1, - 1, 68,
68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68,
68, 68, 68, 68, 68, - 1, - 1, - 1, - 1, 68, - 1, 68, 68, 68, 68, 68, 68, 68, 68,
68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 68, 75, 75,
75, 75, 75, 75, 75, 75, 75, 75, 84, 84, 84, 84, 84, 84, 84, 84, 84, 84, - 1, 75,
75, - 1, - 1, - 1, - 1, 76, 75, 76, 76, 76, 76, 76, 76, 76, 76, 76, 76, 81, 81,
81, 81, 81, 81, 81, 81, 81, 81, - 1, 76, - 1, - 1, 75, 75, - 1, - 1, 76, - 1, -
1, 75, 81, - 1, - 1, - 1, - 1, 76, 81, - 1, 82, 82, 82, 82, 82, 82, 82, 82, 82,
82, - 1, - 1, - 1, 76, - 1, - 1, - 1, - 1, - 1, - 1, 76, 82, 82, - 1, 81, - 1, -
1, - 1, 82, 76, 81, - 1, - 1, 85, 85, 85, 85, 85, 85, 85, 85, 85, 85, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 82, 82, 85, - 1, - 1, - 1, - 1, 82, 85,
86, - 1, 86, 86, 86, 86, 86, 86, 86, 86, 86, 86, - 1, - 1, - 1, - 1, - 1, - 1, -
1, - 1, - 1, - 1, - 1, 86, - 1, 85, - 1, - 1, - 1, - 1, 86, 85, - 1, - 1, - 1, -
1, - 1, - 1, 87, 86, 87, 87, 87, 87, 87, 87, 87, 87, 87, 87, - 1, - 1, - 1, - 1,
- 1, 86, - 1, - 1, - 1, - 1, - 1, 87, 86, - 1, - 1, - 1, - 1, - 1, 87, - 1, - 1,
86, - 1, - 1, - 1, - 1, - 1, 87, - 1, - 1, 104, 104, 104, 104, 104, 104, 104,
104, 104, 104, - 1, - 1, - 1, 87, - 1, - 1, - 1, - 1, - 1, - 1, 87, 104, 104, -
1, - 1, - 1, - 1, - 1, 104, 87, 90, 90, 90, 90, 90, 90, 90, 90, 90, 90, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, 90, 90, 90, 90, 90, 90, 104, 104, - 1, - 1, - 1, - 1, -
1, 104, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 91, 91, 91, 91, 91, 91, 91, 91, 91,
91, - 1, 90, 90, 90, 90, 90, 90, 91, 91, 91, 91, 91, 91, - 1, - 1, - 1, 112, - 1,
91, - 1, - 1, 112, - 1, - 1, - 1, - 1, - 1, 91, - 1, - 1, 112, 112, 112, 112,
112, 112, 112, 112, - 1, 91, 91, 91, 91, 91, 91, 112, - 1, - 1, - 1, - 1, 91, -
1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 91, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, 112, - 1, - 1, - 1, - 1, 112, 112, - 1, - 1, - 1,
112, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 112, - 1, - 1, - 1, 112, - 1, 112, - 1,
112, - 1, 112, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, - 1, - 1,
- 1, - 1, - 1, - 1, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125,
125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 125, 126, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, 126, 126, 126, 126, 126, 126, 126, 126, 126,
126, - 1, - 1, - 1, - 1, - 1, - 1, - 1, 126, 126, 126, 126, 126, 126, 126, 126,
126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126,
126, 126, - 1, - 1, - 1, - 1, - 1, - 1, 126, 126, 126, 126, 126, 126, 126, 126,
126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126, 126,
126, 126, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1,
- 1, - 1, - 1, - 1, - 1, - 1, - 1, - 1}; int Cyc_Lex_token_rec( struct Cyc_Lexing_lexbuf*
lexbuf, int lexstate){ if( Cyc_Lex_lt == 0){ Cyc_Lex_lt=({ struct Cyc_Lexing_lex_tables*
_temp341=( struct Cyc_Lexing_lex_tables*) GC_malloc( sizeof( struct Cyc_Lexing_lex_tables));
_temp341->lex_base=( struct _tagged_ptr0)({ int* _temp350=( int*)(( int*) Cyc_Lex_lbase);
struct _tagged_ptr0 _temp351; _temp351.curr= _temp350; _temp351.base= _temp350;
_temp351.last_plus_one= _temp350 + 151; _temp351;}); _temp341->lex_backtrk=(
struct _tagged_ptr0)({ int* _temp348=( int*)(( int*) Cyc_Lex_lbacktrk); struct
_tagged_ptr0 _temp349; _temp349.curr= _temp348; _temp349.base= _temp348;
_temp349.last_plus_one= _temp348 + 151; _temp349;}); _temp341->lex_default=(
struct _tagged_ptr0)({ int* _temp346=( int*)(( int*) Cyc_Lex_ldefault); struct
_tagged_ptr0 _temp347; _temp347.curr= _temp346; _temp347.base= _temp346;
_temp347.last_plus_one= _temp346 + 151; _temp347;}); _temp341->lex_trans=(
struct _tagged_ptr0)({ int* _temp344=( int*)(( int*) Cyc_Lex_ltrans); struct
_tagged_ptr0 _temp345; _temp345.curr= _temp344; _temp345.base= _temp344;
_temp345.last_plus_one= _temp344 + 1813; _temp345;}); _temp341->lex_check=(
struct _tagged_ptr0)({ int* _temp342=( int*)(( int*) Cyc_Lex_lcheck); struct
_tagged_ptr0 _temp343; _temp343.curr= _temp342; _temp343.base= _temp342;
_temp343.last_plus_one= _temp342 + 1813; _temp343;}); _temp341;});} lexstate=((
int(*)( struct Cyc_Lexing_lex_tables*, int, struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lex_engine)((
struct Cyc_Lexing_lex_tables*)({ struct Cyc_Lexing_lex_tables* _temp352= Cyc_Lex_lt;
if( _temp352 == 0){ _throw( Null_Exception);} _temp352;}), lexstate, lexbuf);
switch( lexstate){ case 0: _LL353: return( int)(( short(*)( struct Cyc_Lexing_lexbuf*
lbuf)) Cyc_Lex_process_id)( lexbuf); case 1: _LL354: return( int)(( short(*)(
struct Cyc_Lexing_lexbuf* lbuf)) Cyc_Lex_process_id)( lexbuf); case 2: _LL355:
return( int)(( short(*)( struct Cyc_Lexing_lexbuf* lbuf)) Cyc_Lex_process_qual_id)(
lexbuf); case 3: _LL356: Cyc_Lex_token_string=* Cyc_Lex_get_symbol((( int(*)(
struct Cyc_Lexing_lexbuf* lbuf)) Cyc_Lex_str_index_lbuf)( lexbuf)); return 342;
case 4: _LL357: Cyc_Lex_token_int=(( struct _tuple3*(*)( struct Cyc_Lexing_lexbuf*
lbuf, int start, int base)) Cyc_Lex_intconst)( lexbuf, 2, 16); return 338; case
5: _LL358: Cyc_Lex_token_int=(( struct _tuple3*(*)( struct Cyc_Lexing_lexbuf*
lbuf, int start, int base)) Cyc_Lex_intconst)( lexbuf, 0, 8); return 338; case 6:
_LL359: Cyc_Lex_token_int=(( struct _tuple3*(*)( struct Cyc_Lexing_lexbuf* lbuf,
int start, int base)) Cyc_Lex_intconst)( lexbuf, 0, 10); return 338; case 7:
_LL360: Cyc_Lex_token_int=(( struct _tuple3*(*)( struct Cyc_Lexing_lexbuf* lbuf,
int start, int base)) Cyc_Lex_intconst)( lexbuf, 0, 10); return 338; case 8:
_LL361: Cyc_Lex_token_string=(( struct _tagged_string(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lexing_lexeme)( lexbuf); return 341; case 9: _LL362: Cyc_Lex_token_string=((
struct _tagged_string(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lexeme)( lexbuf);
return 341; case 10: _LL363: return 320; case 11: _LL364: return 321; case 12:
_LL365: return 318; case 13: _LL366: return 319; case 14: _LL367: return 314;
case 15: _LL368: return 315; case 16: _LL369: return 327; case 17: _LL370:
return 328; case 18: _LL371: return 324; case 19: _LL372: return 325; case 20:
_LL373: return 326; case 21: _LL374: return 333; case 22: _LL375: return 332;
case 23: _LL376: return 331; case 24: _LL377: return 329; case 25: _LL378:
return 330; case 26: _LL379: return 322; case 27: _LL380: return 323; case 28:
_LL381: return 316; case 29: _LL382: return 317; case 30: _LL383: return 335;
case 31: _LL384: return 313; case 32: _LL385: return 334; case 33: _LL386:
return 336; case 34: _LL387: return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_token)(
lexbuf); case 35: _LL388: return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_token)(
lexbuf); case 36: _LL389: return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_token)(
lexbuf); case 37: _LL390: Cyc_Lex_comment_depth= 1; Cyc_Lex_runaway_start=(( int(*)(
struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lexeme_start)( lexbuf);(( int(*)( struct
Cyc_Lexing_lexbuf*)) Cyc_Lex_comment)( lexbuf); return(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lex_token)( lexbuf); case 38: _LL391: Cyc_Lex_string_pos= 0; Cyc_Lex_runaway_start=((
int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lexeme_start)( lexbuf);(( int(*)(
struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); Cyc_Lex_token_string= Cyc_Lex_get_stored_string();
return 339; case 39: _LL392: Cyc_Lex_token_char='\a'; return 340; case 40:
_LL393: Cyc_Lex_token_char='\b'; return 340; case 41: _LL394: Cyc_Lex_token_char='\f';
return 340; case 42: _LL395: Cyc_Lex_token_char='\n'; return 340; case 43:
_LL396: Cyc_Lex_token_char='\r'; return 340; case 44: _LL397: Cyc_Lex_token_char='\t';
return 340; case 45: _LL398: Cyc_Lex_token_char='\v'; return 340; case 46:
_LL399: Cyc_Lex_token_char='\\'; return 340; case 47: _LL400: Cyc_Lex_token_char='\'';
return 340; case 48: _LL401: Cyc_Lex_token_char='"'; return 340; case 49: _LL402:
Cyc_Lex_token_char='?'; return 340; case 50: _LL403: Cyc_Lex_token_char=(( char(*)(
struct Cyc_Lexing_lexbuf* lb, int start, int howmany)) Cyc_Lex_char_for_octal_code)(
lexbuf, 2, 3); return 340; case 51: _LL404: Cyc_Lex_token_char=(( char(*)(
struct Cyc_Lexing_lexbuf* lb, int start, int howmany)) Cyc_Lex_char_for_octal_code)(
lexbuf, 2, 2); return 340; case 52: _LL405: Cyc_Lex_token_char=(( char(*)(
struct Cyc_Lexing_lexbuf* lb, int start, int howmany)) Cyc_Lex_char_for_octal_code)(
lexbuf, 2, 1); return 340; case 53: _LL406: Cyc_Lex_token_char= Cyc_Lex_char_for_hex_code(((
struct _tagged_string(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lexeme)( lexbuf),
3); return 340; case 54: _LL407: Cyc_Lex_token_char=(( char(*)( struct Cyc_Lexing_lexbuf*,
int)) Cyc_Lexing_lexeme_char)( lexbuf, 1); return 340; case 55: _LL408: return -
1; case 56: _LL409: return( int)(( char(*)( struct Cyc_Lexing_lexbuf*, int)) Cyc_Lexing_lexeme_char)(
lexbuf, 0); default: _LL410:( lexbuf->refill_buff)( lexbuf); return(( int(*)(
struct Cyc_Lexing_lexbuf* lexbuf, int lexstate)) Cyc_Lex_token_rec)( lexbuf,
lexstate);}( void) _throw(({ struct Cyc_Lexing_Error_struct* _temp412=( struct
Cyc_Lexing_Error_struct*) GC_malloc( sizeof( struct Cyc_Lexing_Error_struct));*
_temp412=( struct Cyc_Lexing_Error_struct){.tag= Cyc_Lexing_Error_tag,.f1=(
struct _tagged_string)({ char* _temp413=( char*)"some action didn't return!";
struct _tagged_string _temp414; _temp414.curr= _temp413; _temp414.base= _temp413;
_temp414.last_plus_one= _temp413 + 27; _temp414;})};( struct _xenum_struct*)
_temp412;}));} int Cyc_Lex_token( struct Cyc_Lexing_lexbuf* lexbuf){ return((
int(*)( struct Cyc_Lexing_lexbuf* lexbuf, int lexstate)) Cyc_Lex_token_rec)(
lexbuf, 0);} int Cyc_Lex_strng_rec( struct Cyc_Lexing_lexbuf* lexbuf, int
lexstate){ if( Cyc_Lex_lt == 0){ Cyc_Lex_lt=({ struct Cyc_Lexing_lex_tables*
_temp415=( struct Cyc_Lexing_lex_tables*) GC_malloc( sizeof( struct Cyc_Lexing_lex_tables));
_temp415->lex_base=( struct _tagged_ptr0)({ int* _temp424=( int*)(( int*) Cyc_Lex_lbase);
struct _tagged_ptr0 _temp425; _temp425.curr= _temp424; _temp425.base= _temp424;
_temp425.last_plus_one= _temp424 + 151; _temp425;}); _temp415->lex_backtrk=(
struct _tagged_ptr0)({ int* _temp422=( int*)(( int*) Cyc_Lex_lbacktrk); struct
_tagged_ptr0 _temp423; _temp423.curr= _temp422; _temp423.base= _temp422;
_temp423.last_plus_one= _temp422 + 151; _temp423;}); _temp415->lex_default=(
struct _tagged_ptr0)({ int* _temp420=( int*)(( int*) Cyc_Lex_ldefault); struct
_tagged_ptr0 _temp421; _temp421.curr= _temp420; _temp421.base= _temp420;
_temp421.last_plus_one= _temp420 + 151; _temp421;}); _temp415->lex_trans=(
struct _tagged_ptr0)({ int* _temp418=( int*)(( int*) Cyc_Lex_ltrans); struct
_tagged_ptr0 _temp419; _temp419.curr= _temp418; _temp419.base= _temp418;
_temp419.last_plus_one= _temp418 + 1813; _temp419;}); _temp415->lex_check=(
struct _tagged_ptr0)({ int* _temp416=( int*)(( int*) Cyc_Lex_lcheck); struct
_tagged_ptr0 _temp417; _temp417.curr= _temp416; _temp417.base= _temp416;
_temp417.last_plus_one= _temp416 + 1813; _temp417;}); _temp415;});} lexstate=((
int(*)( struct Cyc_Lexing_lex_tables*, int, struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lex_engine)((
struct Cyc_Lexing_lex_tables*)({ struct Cyc_Lexing_lex_tables* _temp426= Cyc_Lex_lt;
if( _temp426 == 0){ _throw( Null_Exception);} _temp426;}), lexstate, lexbuf);
switch( lexstate){ case 0: _LL427: return(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lex_strng)( lexbuf); case 1: _LL428: return 0; case 2: _LL429: return(( int(*)(
struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 3: _LL430: Cyc_Lex_store_string_char('\a');
return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 4:
_LL431: Cyc_Lex_store_string_char('\b'); return(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lex_strng)( lexbuf); case 5: _LL432: Cyc_Lex_store_string_char('\f'); return((
int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 6: _LL433: Cyc_Lex_store_string_char('\n');
return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 7:
_LL434: Cyc_Lex_store_string_char('\r'); return(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lex_strng)( lexbuf); case 8: _LL435: Cyc_Lex_store_string_char('\t'); return((
int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 9: _LL436: Cyc_Lex_store_string_char('\v');
return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 10:
_LL437: Cyc_Lex_store_string_char('\\'); return(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lex_strng)( lexbuf); case 11: _LL438: Cyc_Lex_store_string_char('\'');
return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 12:
_LL439: Cyc_Lex_store_string_char('"'); return(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lex_strng)( lexbuf); case 13: _LL440: Cyc_Lex_store_string_char('?'); return((
int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 14: _LL441:
Cyc_Lex_store_string_char((( char(*)( struct Cyc_Lexing_lexbuf* lb, int start,
int howmany)) Cyc_Lex_char_for_octal_code)( lexbuf, 1, 3)); return(( int(*)(
struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 15: _LL442: Cyc_Lex_store_string_char(((
char(*)( struct Cyc_Lexing_lexbuf* lb, int start, int howmany)) Cyc_Lex_char_for_octal_code)(
lexbuf, 1, 2)); return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)(
lexbuf); case 16: _LL443: Cyc_Lex_store_string_char((( char(*)( struct Cyc_Lexing_lexbuf*
lb, int start, int howmany)) Cyc_Lex_char_for_octal_code)( lexbuf, 1, 1));
return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 17:
_LL444: Cyc_Lex_store_string_char( Cyc_Lex_char_for_hex_code((( struct
_tagged_string(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lexeme)( lexbuf), 2));
return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); case 18:
_LL445: Cyc_Lex_store_string_char((( char(*)( struct Cyc_Lexing_lexbuf*, int))
Cyc_Lexing_lexeme_char)( lexbuf, 0)); return(( int(*)( struct Cyc_Lexing_lexbuf*))
Cyc_Lex_strng)( lexbuf); case 19: _LL446:(( void(*)( struct _tagged_string msg,
struct Cyc_Lexing_lexbuf* lb)) Cyc_Lex_runaway_err)(( struct _tagged_string)({
char* _temp448=( char*)"string ends in newline"; struct _tagged_string _temp449;
_temp449.curr= _temp448; _temp449.base= _temp448; _temp449.last_plus_one=
_temp448 + 23; _temp449;}), lexbuf); return 0; case 20: _LL447:(( void(*)(
struct _tagged_string msg, struct Cyc_Lexing_lexbuf* lb)) Cyc_Lex_runaway_err)((
struct _tagged_string)({ char* _temp451=( char*)"unterminated string"; struct
_tagged_string _temp452; _temp452.curr= _temp451; _temp452.base= _temp451;
_temp452.last_plus_one= _temp451 + 20; _temp452;}), lexbuf); return 0; case 21:
_LL450:(( void(*)( struct _tagged_string msg, struct Cyc_Lexing_lexbuf* lb)) Cyc_Lex_err)((
struct _tagged_string)({ char* _temp454=( char*)"bad character following backslash in string";
struct _tagged_string _temp455; _temp455.curr= _temp454; _temp455.base= _temp454;
_temp455.last_plus_one= _temp454 + 44; _temp455;}), lexbuf); return(( int(*)(
struct Cyc_Lexing_lexbuf*)) Cyc_Lex_strng)( lexbuf); default: _LL453:( lexbuf->refill_buff)(
lexbuf); return(( int(*)( struct Cyc_Lexing_lexbuf* lexbuf, int lexstate)) Cyc_Lex_strng_rec)(
lexbuf, lexstate);}( void) _throw(({ struct Cyc_Lexing_Error_struct* _temp457=(
struct Cyc_Lexing_Error_struct*) GC_malloc( sizeof( struct Cyc_Lexing_Error_struct));*
_temp457=( struct Cyc_Lexing_Error_struct){.tag= Cyc_Lexing_Error_tag,.f1=(
struct _tagged_string)({ char* _temp458=( char*)"some action didn't return!";
struct _tagged_string _temp459; _temp459.curr= _temp458; _temp459.base= _temp458;
_temp459.last_plus_one= _temp458 + 27; _temp459;})};( struct _xenum_struct*)
_temp457;}));} int Cyc_Lex_strng( struct Cyc_Lexing_lexbuf* lexbuf){ return((
int(*)( struct Cyc_Lexing_lexbuf* lexbuf, int lexstate)) Cyc_Lex_strng_rec)(
lexbuf, 1);} int Cyc_Lex_comment_rec( struct Cyc_Lexing_lexbuf* lexbuf, int
lexstate){ if( Cyc_Lex_lt == 0){ Cyc_Lex_lt=({ struct Cyc_Lexing_lex_tables*
_temp460=( struct Cyc_Lexing_lex_tables*) GC_malloc( sizeof( struct Cyc_Lexing_lex_tables));
_temp460->lex_base=( struct _tagged_ptr0)({ int* _temp469=( int*)(( int*) Cyc_Lex_lbase);
struct _tagged_ptr0 _temp470; _temp470.curr= _temp469; _temp470.base= _temp469;
_temp470.last_plus_one= _temp469 + 151; _temp470;}); _temp460->lex_backtrk=(
struct _tagged_ptr0)({ int* _temp467=( int*)(( int*) Cyc_Lex_lbacktrk); struct
_tagged_ptr0 _temp468; _temp468.curr= _temp467; _temp468.base= _temp467;
_temp468.last_plus_one= _temp467 + 151; _temp468;}); _temp460->lex_default=(
struct _tagged_ptr0)({ int* _temp465=( int*)(( int*) Cyc_Lex_ldefault); struct
_tagged_ptr0 _temp466; _temp466.curr= _temp465; _temp466.base= _temp465;
_temp466.last_plus_one= _temp465 + 151; _temp466;}); _temp460->lex_trans=(
struct _tagged_ptr0)({ int* _temp463=( int*)(( int*) Cyc_Lex_ltrans); struct
_tagged_ptr0 _temp464; _temp464.curr= _temp463; _temp464.base= _temp463;
_temp464.last_plus_one= _temp463 + 1813; _temp464;}); _temp460->lex_check=(
struct _tagged_ptr0)({ int* _temp461=( int*)(( int*) Cyc_Lex_lcheck); struct
_tagged_ptr0 _temp462; _temp462.curr= _temp461; _temp462.base= _temp461;
_temp462.last_plus_one= _temp461 + 1813; _temp462;}); _temp460;});} lexstate=((
int(*)( struct Cyc_Lexing_lex_tables*, int, struct Cyc_Lexing_lexbuf*)) Cyc_Lexing_lex_engine)((
struct Cyc_Lexing_lex_tables*)({ struct Cyc_Lexing_lex_tables* _temp471= Cyc_Lex_lt;
if( _temp471 == 0){ _throw( Null_Exception);} _temp471;}), lexstate, lexbuf);
switch( lexstate){ case 0: _LL472: ++ Cyc_Lex_comment_depth; return(( int(*)(
struct Cyc_Lexing_lexbuf*)) Cyc_Lex_comment)( lexbuf); case 1: _LL473: -- Cyc_Lex_comment_depth;
if( Cyc_Lex_comment_depth > 0){ return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_comment)(
lexbuf);} return 0; case 2: _LL474:(( void(*)( struct _tagged_string msg, struct
Cyc_Lexing_lexbuf* lb)) Cyc_Lex_runaway_err)(( struct _tagged_string)({ char*
_temp476=( char*)"unterminated comment"; struct _tagged_string _temp477;
_temp477.curr= _temp476; _temp477.base= _temp476; _temp477.last_plus_one=
_temp476 + 21; _temp477;}), lexbuf); return 0; case 3: _LL475: return(( int(*)(
struct Cyc_Lexing_lexbuf*)) Cyc_Lex_comment)( lexbuf); case 4: _LL478: return((
int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_comment)( lexbuf); case 5: _LL479:
return(( int(*)( struct Cyc_Lexing_lexbuf*)) Cyc_Lex_comment)( lexbuf); default:
_LL480:( lexbuf->refill_buff)( lexbuf); return(( int(*)( struct Cyc_Lexing_lexbuf*
lexbuf, int lexstate)) Cyc_Lex_comment_rec)( lexbuf, lexstate);}( void) _throw(({
struct Cyc_Lexing_Error_struct* _temp482=( struct Cyc_Lexing_Error_struct*)
GC_malloc( sizeof( struct Cyc_Lexing_Error_struct));* _temp482=( struct Cyc_Lexing_Error_struct){.tag=
Cyc_Lexing_Error_tag,.f1=( struct _tagged_string)({ char* _temp483=( char*)"some action didn't return!";
struct _tagged_string _temp484; _temp484.curr= _temp483; _temp484.base= _temp483;
_temp484.last_plus_one= _temp483 + 27; _temp484;})};( struct _xenum_struct*)
_temp482;}));} int Cyc_Lex_comment( struct Cyc_Lexing_lexbuf* lexbuf){ return((
int(*)( struct Cyc_Lexing_lexbuf* lexbuf, int lexstate)) Cyc_Lex_comment_rec)(
lexbuf, 2);} void Cyc_Lex_lex_init(){ Cyc_Lex_ids_trie=( struct Cyc_Lex_Trie*)({
struct Cyc_Lex_Trie* _temp485=( struct Cyc_Lex_Trie*) GC_malloc( sizeof( struct
Cyc_Lex_Trie) * 1); _temp485[ 0]=({ struct Cyc_Lex_Trie _temp486; _temp486.children=
0; _temp486.shared_str= - 1; _temp486;}); _temp485;}); Cyc_Lex_typedefs_trie=(
struct Cyc_Lex_Trie*)({ struct Cyc_Lex_Trie* _temp487=( struct Cyc_Lex_Trie*)
GC_malloc( sizeof( struct Cyc_Lex_Trie) * 1); _temp487[ 0]=({ struct Cyc_Lex_Trie
_temp488; _temp488.children= 0; _temp488.shared_str= 0; _temp488;}); _temp487;});
Cyc_Lex_symbols=( struct Cyc_Xarray_Xarray*)(( struct Cyc_Xarray_Xarray*(*)( int,
struct _tagged_string*)) Cyc_Xarray_create)( 101,({ struct _tagged_string*
_temp489=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp489[ 0]=( struct _tagged_string)({ char* _temp490=( char*)""; struct
_tagged_string _temp491; _temp491.curr= _temp490; _temp491.base= _temp490;
_temp491.last_plus_one= _temp490 + 1; _temp491;}); _temp489;})); Cyc_Lex_num_kws=(
int) 56u; Cyc_Lex_kw_nums=({ unsigned int _temp492=( unsigned int) Cyc_Lex_num_kws;
int* _temp493=( int*) GC_malloc_atomic( sizeof( int) * _temp492); unsigned int i;
struct _tagged_ptr0 _temp494={ _temp493, _temp493, _temp493 + _temp492}; for( i=
0; i < _temp492; i ++){ _temp493[ i]= 0;} _temp494;});{ int i= 0; for( 0; i <
Cyc_Lex_num_kws; ++ i){ struct _tagged_string str=({ struct _tuple8* _temp495=(
struct _tuple8*) Cyc_Lex_rw_array; unsigned int _temp496= i; if( _temp496 >= 56u){
_throw( Null_Exception);} _temp495[ _temp496];}).f1; Cyc_Lex_str_index( str, 0,(
int) Cyc_String_strlen( str));({ struct _tagged_ptr0 _temp497= Cyc_Lex_kw_nums;
int* _temp499= _temp497.curr + i; if( _temp499 < _temp497.base? 1: _temp499 >=
_temp497.last_plus_one){ _throw( Null_Exception);}* _temp499=( int)({ struct
_tuple8* _temp500=( struct _tuple8*) Cyc_Lex_rw_array; unsigned int _temp501= i;
if( _temp501 >= 56u){ _throw( Null_Exception);} _temp500[ _temp501];}).f2;});}}
Cyc_Lex_typedef_init(); Cyc_Lex_comment_depth= 0;}