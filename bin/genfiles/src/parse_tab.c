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
_tuple8{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual* f2; void* f3; struct
Cyc_List_List* f4; } ; struct _tuple9{ struct Cyc_Absyn_Tqual* f1; void* f2; } ;
struct _tuple10{ struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; } ;
struct _tuple11{ void* f1; int f2; } ; struct _tuple12{ struct Cyc_Parse_Declarator*
f1; struct Cyc_Absyn_Exp* f2; } ; struct _tuple13{ struct Cyc_Absyn_Tqual* f1;
struct Cyc_List_List* f2; struct Cyc_List_List* f3; } ; struct _tuple14{ struct
Cyc_List_List* f1; int f2; struct Cyc_Core_Opt* f3; } ; struct _tuple15{ struct
Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; struct _tagged_ptr1{ short*
curr; short* base; short* last_plus_one; } ; struct _tagged_ptr2{ struct
_xenum_struct** curr; struct _xenum_struct** base; struct _xenum_struct**
last_plus_one; } ; struct _tagged_ptr3{ struct Cyc_Yyltype* curr; struct Cyc_Yyltype*
base; struct Cyc_Yyltype* last_plus_one; } ; struct _tuple16{ struct Cyc_List_List*
f1; struct Cyc_Absyn_Exp* f2; } ; typedef unsigned int Cyc_uint; typedef char*
Cyc_Cstring; typedef struct _tagged_string Cyc_string; typedef struct
_tagged_string Cyc_string_t; typedef struct _tagged_string* Cyc_stringptr;
typedef int Cyc_bool; extern void* exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; typedef struct Cyc_Core_Opt* Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg_tag[
11u]; struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1; }
; extern char Cyc_Core_Failure_tag[ 8u]; struct Cyc_Core_Failure_struct{ char*
tag; struct _tagged_string f1; } ; extern char Cyc_Core_Impossible_tag[ 11u];
struct Cyc_Core_Impossible_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Core_Not_found_tag[ 10u]; struct Cyc_Core_Not_found_struct{ char*
tag; } ; extern char Cyc_Core_Unreachable_tag[ 12u]; struct Cyc_Core_Unreachable_struct{
char* tag; struct _tagged_string f1; } ; extern char* string_to_Cstring( struct
_tagged_string); extern char* underlying_Cstring( struct _tagged_string); extern
struct _tagged_string Cstring_to_string( char*); extern int system( char*);
struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
extern struct Cyc_Stdio___sFILE* Cyc_Stdio_stderr; typedef unsigned int Cyc_Stdio_size_t;
typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileOpenError_tag[ 14u];
struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Stdio_FileCloseError_tag[ 15u]; struct Cyc_Stdio_FileCloseError_struct{
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
extern struct _tuple0 Cyc_List_split( struct Cyc_List_List* x); extern
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
typedef void* Cyc_Absyn_sign_t; typedef struct Cyc_Absyn_Conref* Cyc_Absyn_conref;
typedef void* Cyc_Absyn_constraint_t; typedef void* Cyc_Absyn_bounds_t; typedef
struct Cyc_Absyn_PtrInfo Cyc_Absyn_ptr_info_t; typedef struct Cyc_Absyn_FnInfo
Cyc_Absyn_fn_info_t; typedef void* Cyc_Absyn_typ; typedef void* Cyc_Absyn_funcparams_t;
typedef void* Cyc_Absyn_type_modifier; typedef void* Cyc_Absyn_cnst_t; typedef
void* Cyc_Absyn_primop; typedef void* Cyc_Absyn_incrementor_t; typedef void* Cyc_Absyn_raw_exp_t;
typedef struct Cyc_Absyn_Exp* Cyc_Absyn_exp; typedef struct Cyc_Absyn_Exp* Cyc_Absyn_exp_opt;
typedef void* Cyc_Absyn_raw_stmt_t; typedef struct Cyc_Absyn_Stmt* Cyc_Absyn_stmt;
typedef struct Cyc_Absyn_Stmt* Cyc_Absyn_stmt_opt; typedef void* Cyc_Absyn_raw_pat_t;
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
int Cyc_Absyn_Sizeoftyp_e_tag; struct Cyc_Absyn_Sizeoftyp_e_struct{ int tag;
void* f1; } ; extern const int Cyc_Absyn_Sizeofexp_e_tag; struct Cyc_Absyn_Sizeofexp_e_struct{
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
int tag; int f1; struct Cyc_List_List* f2; } ; extern const int Cyc_Absyn_Comprehension_e_tag;
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
struct Cyc_Absyn_Tqual* tq; void* type; struct Cyc_List_List* attributes; } ;
struct Cyc_Absyn_Structdecl{ void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List* attributes; } ; struct
Cyc_Absyn_Uniondecl{ void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List* attributes; } ; struct
Cyc_Absyn_Enumfield{ struct _tuple1* name; struct Cyc_Absyn_Exp* tag; struct Cyc_List_List*
tvs; struct Cyc_List_List* typs; struct Cyc_Position_Segment* loc; } ; struct
Cyc_Absyn_Enumdecl{ void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; } ; struct Cyc_Absyn_Xenumdecl{ void* sc;
struct _tuple1* name; struct Cyc_List_List* fields; } ; struct Cyc_Absyn_Typedefdecl{
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
extern struct Cyc_Absyn_Exp* Cyc_Absyn_null_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_true_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_false_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_int_exp( void*, int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_signed_int_exp( int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_char_exp( char c, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_float_exp( struct _tagged_string f,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_string_exp(
int heap_allocate, struct _tagged_string s, struct Cyc_Position_Segment*);
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
_temp23.last_plus_one= _temp22 + 52; _temp23;}), loc); goto _LL9; _LL9:;}{
struct Cyc_List_List* atts=(* field).f5; return({ struct Cyc_Absyn_Structfield*
_temp24=( struct Cyc_Absyn_Structfield*) GC_malloc( sizeof( struct Cyc_Absyn_Structfield));
_temp24->name=(* qid).f2; _temp24->tq=(* field).f2; _temp24->type=( void*)(*
field).f3; _temp24->attributes= atts; _temp24;});}}} static struct _tuple2* Cyc_Parse_make_param(
struct Cyc_Position_Segment* loc, struct _tuple8* field){ struct _tuple8 _temp27;
struct Cyc_List_List* _temp28; void* _temp30; struct Cyc_Absyn_Tqual* _temp32;
struct Cyc_Core_Opt* _temp34; struct _tuple8* _temp25= field; _temp27=* _temp25;
_LL35: _temp34= _temp27.f1; goto _LL33; _LL33: _temp32= _temp27.f2; goto _LL31;
_LL31: _temp30= _temp27.f3; goto _LL29; _LL29: _temp28= _temp27.f4; goto _LL26;
_LL26: { struct Cyc_Core_Opt* idopt= 0; if( _temp34 != 0){ idopt=({ struct Cyc_Core_Opt*
_temp36=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp36->v=( void*)(*(( struct _tuple1*) _temp34->v)).f2; _temp36;});{ void*
_temp37=(*(( struct _tuple1*) _temp34->v)).f1; struct Cyc_List_List* _temp47;
struct Cyc_List_List* _temp49; _LL39: if(( unsigned int) _temp37 > 1u?(( struct
_enum_struct*) _temp37)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL48: _temp47=( struct
Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp37)->f1; if( _temp47 == 0){
goto _LL40;} else{ goto _LL41;}} else{ goto _LL41;} _LL41: if(( unsigned int)
_temp37 > 1u?(( struct _enum_struct*) _temp37)->tag == Cyc_Absyn_Abs_n_tag: 0){
_LL50: _temp49=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp37)->f1; if( _temp49 == 0){ goto _LL42;} else{ goto _LL43;}} else{ goto
_LL43;} _LL43: if( _temp37 == Cyc_Absyn_Loc_n){ goto _LL44;} else{ goto _LL45;}
_LL45: goto _LL46; _LL40: goto _LL38; _LL42: goto _LL38; _LL44: goto _LL38;
_LL46: Cyc_Parse_err(( struct _tagged_string)({ char* _temp51=( char*)"parameter cannot be qualified with a module name";
struct _tagged_string _temp52; _temp52.curr= _temp51; _temp52.base= _temp51;
_temp52.last_plus_one= _temp51 + 49; _temp52;}), loc); goto _LL38; _LL38:;}} if(
_temp28 != 0){(( void(*)( struct _tagged_string msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)(( struct _tagged_string)({ char* _temp53=( char*)"parameter should have no type parameters";
struct _tagged_string _temp54; _temp54.curr= _temp53; _temp54.base= _temp53;
_temp54.last_plus_one= _temp53 + 41; _temp54;}), loc);} return({ struct _tuple2*
_temp55=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2)); _temp55->f1=
idopt; _temp55->f2= _temp32; _temp55->f3= _temp30; _temp55;});}} static void*
Cyc_Parse_type_spec( void* t, struct Cyc_Position_Segment* loc){ return({ struct
Cyc_Parse_Type_spec_struct* _temp56=( struct Cyc_Parse_Type_spec_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Type_spec_struct)); _temp56->tag= Cyc_Parse_Type_spec_tag;
_temp56->f1=( void*) t; _temp56->f2= loc;( void*) _temp56;});} static void* Cyc_Parse_array2ptr(
void* t){ void* _temp57= t; struct Cyc_Absyn_Exp* _temp63; struct Cyc_Absyn_Tqual*
_temp65; void* _temp67; _LL59: if(( unsigned int) _temp57 > 4u?(( struct
_enum_struct*) _temp57)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL68: _temp67=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp57)->f1; goto _LL66; _LL66:
_temp65=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*) _temp57)->f2;
goto _LL64; _LL64: _temp63=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp57)->f3; goto _LL60;} else{ goto _LL61;} _LL61: goto _LL62; _LL60: return
Cyc_Absyn_starb_typ( _temp67, Cyc_Absyn_HeapRgn, _temp65,({ struct Cyc_Absyn_Upper_b_struct*
_temp69=( struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp69->tag= Cyc_Absyn_Upper_b_tag; _temp69->f1= _temp63;( void*) _temp69;}));
_LL62: return t; _LL58:;} static void Cyc_Parse_arg_array2ptr( struct _tuple2* x){(*
x).f3= Cyc_Parse_array2ptr((* x).f3);} static struct _tuple9* Cyc_Parse_get_tqual_typ(
struct Cyc_Position_Segment* loc, struct _tuple2* t){ return({ struct _tuple9*
_temp70=( struct _tuple9*) GC_malloc( sizeof( struct _tuple9)); _temp70->f1=(* t).f2;
_temp70->f2=(* t).f3; _temp70;});} static void Cyc_Parse_only_vardecl( struct
Cyc_List_List* params, struct Cyc_Absyn_Decl* x){ struct _tagged_string
decl_kind;{ void* _temp71=( void*) x->r; struct Cyc_Absyn_Vardecl* _temp95; int
_temp97; struct Cyc_Absyn_Exp* _temp99; struct Cyc_Core_Opt* _temp101; struct
Cyc_Core_Opt* _temp103; struct Cyc_Absyn_Pat* _temp105; struct Cyc_Absyn_Fndecl*
_temp107; struct Cyc_Absyn_Structdecl* _temp109; struct Cyc_Absyn_Uniondecl*
_temp111; struct Cyc_Absyn_Enumdecl* _temp113; struct Cyc_Absyn_Typedefdecl*
_temp115; struct Cyc_Absyn_Xenumdecl* _temp117; struct Cyc_List_List* _temp119;
struct _tagged_string* _temp121; struct Cyc_List_List* _temp123; struct _tuple1*
_temp125; struct Cyc_List_List* _temp127; _LL73: if((( struct _enum_struct*)
_temp71)->tag == Cyc_Absyn_Var_d_tag){ _LL96: _temp95=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_d_struct*) _temp71)->f1; goto _LL74;} else{ goto _LL75;}
_LL75: if((( struct _enum_struct*) _temp71)->tag == Cyc_Absyn_Let_d_tag){ _LL106:
_temp105=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp71)->f1;
goto _LL104; _LL104: _temp103=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp71)->f2; goto _LL102; _LL102: _temp101=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp71)->f3; goto _LL100; _LL100: _temp99=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp71)->f4; goto _LL98; _LL98: _temp97=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp71)->f5; goto _LL76;} else{ goto _LL77;} _LL77: if((( struct _enum_struct*)
_temp71)->tag == Cyc_Absyn_Fn_d_tag){ _LL108: _temp107=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Fn_d_struct*) _temp71)->f1; goto _LL78;} else{ goto _LL79;}
_LL79: if((( struct _enum_struct*) _temp71)->tag == Cyc_Absyn_Struct_d_tag){
_LL110: _temp109=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*)
_temp71)->f1; goto _LL80;} else{ goto _LL81;} _LL81: if((( struct _enum_struct*)
_temp71)->tag == Cyc_Absyn_Union_d_tag){ _LL112: _temp111=( struct Cyc_Absyn_Uniondecl*)((
struct Cyc_Absyn_Union_d_struct*) _temp71)->f1; goto _LL82;} else{ goto _LL83;}
_LL83: if((( struct _enum_struct*) _temp71)->tag == Cyc_Absyn_Enum_d_tag){
_LL114: _temp113=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*)
_temp71)->f1; goto _LL84;} else{ goto _LL85;} _LL85: if((( struct _enum_struct*)
_temp71)->tag == Cyc_Absyn_Typedef_d_tag){ _LL116: _temp115=( struct Cyc_Absyn_Typedefdecl*)((
struct Cyc_Absyn_Typedef_d_struct*) _temp71)->f1; goto _LL86;} else{ goto _LL87;}
_LL87: if((( struct _enum_struct*) _temp71)->tag == Cyc_Absyn_Xenum_d_tag){
_LL118: _temp117=( struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_d_struct*)
_temp71)->f1; goto _LL88;} else{ goto _LL89;} _LL89: if((( struct _enum_struct*)
_temp71)->tag == Cyc_Absyn_Namespace_d_tag){ _LL122: _temp121=( struct
_tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*) _temp71)->f1; goto
_LL120; _LL120: _temp119=( struct Cyc_List_List*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp71)->f2; goto _LL90;} else{ goto _LL91;} _LL91: if((( struct _enum_struct*)
_temp71)->tag == Cyc_Absyn_Using_d_tag){ _LL126: _temp125=( struct _tuple1*)((
struct Cyc_Absyn_Using_d_struct*) _temp71)->f1; goto _LL124; _LL124: _temp123=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*) _temp71)->f2; goto
_LL92;} else{ goto _LL93;} _LL93: if((( struct _enum_struct*) _temp71)->tag ==
Cyc_Absyn_ExternC_d_tag){ _LL128: _temp127=( struct Cyc_List_List*)(( struct Cyc_Absyn_ExternC_d_struct*)
_temp71)->f1; goto _LL94;} else{ goto _LL72;} _LL74: if( _temp95->initializer !=
0){(( void(*)( struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_string)({ char* _temp129=( char*)"initializers are not allowed in parameter declarations";
struct _tagged_string _temp130; _temp130.curr= _temp129; _temp130.base= _temp129;
_temp130.last_plus_one= _temp129 + 55; _temp130;}), x->loc);}{ void* _temp131=(*
_temp95->name).f1; struct Cyc_List_List* _temp141; struct Cyc_List_List*
_temp143; _LL133: if( _temp131 == Cyc_Absyn_Loc_n){ goto _LL134;} else{ goto
_LL135;} _LL135: if(( unsigned int) _temp131 > 1u?(( struct _enum_struct*)
_temp131)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL142: _temp141=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp131)->f1; if( _temp141 == 0){ goto _LL136;}
else{ goto _LL137;}} else{ goto _LL137;} _LL137: if(( unsigned int) _temp131 > 1u?((
struct _enum_struct*) _temp131)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL144:
_temp143=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp131)->f1;
if( _temp143 == 0){ goto _LL138;} else{ goto _LL139;}} else{ goto _LL139;}
_LL139: goto _LL140; _LL134: goto _LL132; _LL136: goto _LL132; _LL138: goto
_LL132; _LL140: Cyc_Parse_err(( struct _tagged_string)({ char* _temp145=( char*)"module names not allowed on parameter declarations";
struct _tagged_string _temp146; _temp146.curr= _temp145; _temp146.base= _temp145;
_temp146.last_plus_one= _temp145 + 51; _temp146;}), x->loc); goto _LL132; _LL132:;}{
int found= 0; for( 0; params != 0; params= params->tl){ if( Cyc_String_zstrptrcmp((*
_temp95->name).f2,( struct _tagged_string*) params->hd) == 0){ found= 1; break;}}
if( ! found){(( void(*)( struct _tagged_string msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)(({ struct _tagged_string _temp147=*(* _temp95->name).f2;
xprintf("%.*s is not listed as a parameter", _temp147.last_plus_one - _temp147.curr,
_temp147.curr);}), x->loc);} return;} _LL76: decl_kind=( struct _tagged_string)({
char* _temp148=( char*)"let declaration"; struct _tagged_string _temp149;
_temp149.curr= _temp148; _temp149.base= _temp148; _temp149.last_plus_one=
_temp148 + 16; _temp149;}); goto _LL72; _LL78: decl_kind=( struct _tagged_string)({
char* _temp150=( char*)"function declaration"; struct _tagged_string _temp151;
_temp151.curr= _temp150; _temp151.base= _temp150; _temp151.last_plus_one=
_temp150 + 21; _temp151;}); goto _LL72; _LL80: decl_kind=( struct _tagged_string)({
char* _temp152=( char*)"struct declaration"; struct _tagged_string _temp153;
_temp153.curr= _temp152; _temp153.base= _temp152; _temp153.last_plus_one=
_temp152 + 19; _temp153;}); goto _LL72; _LL82: decl_kind=( struct _tagged_string)({
char* _temp154=( char*)"union declaration"; struct _tagged_string _temp155;
_temp155.curr= _temp154; _temp155.base= _temp154; _temp155.last_plus_one=
_temp154 + 18; _temp155;}); goto _LL72; _LL84: decl_kind=( struct _tagged_string)({
char* _temp156=( char*)"enum declaration"; struct _tagged_string _temp157;
_temp157.curr= _temp156; _temp157.base= _temp156; _temp157.last_plus_one=
_temp156 + 17; _temp157;}); goto _LL72; _LL86: decl_kind=( struct _tagged_string)({
char* _temp158=( char*)"typedef"; struct _tagged_string _temp159; _temp159.curr=
_temp158; _temp159.base= _temp158; _temp159.last_plus_one= _temp158 + 8;
_temp159;}); goto _LL72; _LL88: decl_kind=( struct _tagged_string)({ char*
_temp160=( char*)"xenum declaration"; struct _tagged_string _temp161; _temp161.curr=
_temp160; _temp161.base= _temp160; _temp161.last_plus_one= _temp160 + 18;
_temp161;}); goto _LL72; _LL90: decl_kind=( struct _tagged_string)({ char*
_temp162=( char*)"namespace declaration"; struct _tagged_string _temp163;
_temp163.curr= _temp162; _temp163.base= _temp162; _temp163.last_plus_one=
_temp162 + 22; _temp163;}); goto _LL72; _LL92: decl_kind=( struct _tagged_string)({
char* _temp164=( char*)"using declaration"; struct _tagged_string _temp165;
_temp165.curr= _temp164; _temp165.base= _temp164; _temp165.last_plus_one=
_temp164 + 18; _temp165;}); goto _LL72; _LL94: decl_kind=( struct _tagged_string)({
char* _temp166=( char*)"extern C declaration"; struct _tagged_string _temp167;
_temp167.curr= _temp166; _temp167.base= _temp166; _temp167.last_plus_one=
_temp166 + 21; _temp167;}); goto _LL72; _LL72:;}(( void(*)( struct
_tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({ struct
_tagged_string _temp168= decl_kind; xprintf("%.*s appears in parameter type",
_temp168.last_plus_one - _temp168.curr, _temp168.curr);}), x->loc); return;}
static struct _tuple2* Cyc_Parse_get_param_type( struct _tuple10* env, struct
_tagged_string* x){ struct _tuple10 _temp171; struct Cyc_Position_Segment*
_temp172; struct Cyc_List_List* _temp174; struct _tuple10* _temp169= env;
_temp171=* _temp169; _LL175: _temp174= _temp171.f1; goto _LL173; _LL173:
_temp172= _temp171.f2; goto _LL170; _LL170: if( _temp174 == 0){ return(( struct
_tuple2*(*)( struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({
struct _tagged_string _temp176=* x; xprintf("missing type for parameter %.*s",
_temp176.last_plus_one - _temp176.curr, _temp176.curr);}), _temp172);}{ void*
_temp177=( void*)(( struct Cyc_Absyn_Decl*) _temp174->hd)->r; struct Cyc_Absyn_Vardecl*
_temp183; _LL179: if((( struct _enum_struct*) _temp177)->tag == Cyc_Absyn_Var_d_tag){
_LL184: _temp183=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp177)->f1; goto _LL180;} else{ goto _LL181;} _LL181: goto _LL182; _LL180:{
void* _temp185=(* _temp183->name).f1; struct Cyc_List_List* _temp195; struct Cyc_List_List*
_temp197; _LL187: if( _temp185 == Cyc_Absyn_Loc_n){ goto _LL188;} else{ goto
_LL189;} _LL189: if(( unsigned int) _temp185 > 1u?(( struct _enum_struct*)
_temp185)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL196: _temp195=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp185)->f1; if( _temp195 == 0){ goto _LL190;}
else{ goto _LL191;}} else{ goto _LL191;} _LL191: if(( unsigned int) _temp185 > 1u?((
struct _enum_struct*) _temp185)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL198:
_temp197=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp185)->f1;
if( _temp197 == 0){ goto _LL192;} else{ goto _LL193;}} else{ goto _LL193;}
_LL193: goto _LL194; _LL188: goto _LL186; _LL190: goto _LL186; _LL192: goto
_LL186; _LL194: Cyc_Parse_err(( struct _tagged_string)({ char* _temp199=( char*)"module name not allowed on parameter";
struct _tagged_string _temp200; _temp200.curr= _temp199; _temp200.base= _temp199;
_temp200.last_plus_one= _temp199 + 37; _temp200;}), _temp172); goto _LL186;
_LL186:;} if( Cyc_String_zstrptrcmp((* _temp183->name).f2, x) == 0){ return({
struct _tuple2* _temp201=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2) *
1); _temp201[ 0]=({ struct _tuple2 _temp202; _temp202.f1=( struct Cyc_Core_Opt*)({
struct Cyc_Core_Opt* _temp203=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt) * 1); _temp203[ 0]=({ struct Cyc_Core_Opt _temp204; _temp204.v=(
void*)(* _temp183->name).f2; _temp204;}); _temp203;}); _temp202.f2= _temp183->tq;
_temp202.f3=( void*) _temp183->type; _temp202;}); _temp201;});} else{ return Cyc_Parse_get_param_type(({
struct _tuple10* _temp205=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp205->f1= _temp174->tl; _temp205->f2= _temp172; _temp205;}), x);} _LL182:
return(( struct _tuple2*(*)( struct _tagged_string msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)(( struct _tagged_string)({ char* _temp206=( char*)"non-variable declaration";
struct _tagged_string _temp207; _temp207.curr= _temp206; _temp207.base= _temp206;
_temp207.last_plus_one= _temp206 + 25; _temp207;}),(( struct Cyc_Absyn_Decl*)
_temp174->hd)->loc); _LL178:;}} static int Cyc_Parse_is_typeparam( void* tm){
void* _temp208= tm; int _temp214; struct Cyc_Position_Segment* _temp216; struct
Cyc_List_List* _temp218; _LL210: if(( unsigned int) _temp208 > 1u?(( struct
_enum_struct*) _temp208)->tag == Cyc_Absyn_TypeParams_mod_tag: 0){ _LL219:
_temp218=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp208)->f1; goto _LL217; _LL217: _temp216=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp208)->f2; goto _LL215; _LL215:
_temp214=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp208)->f3; goto
_LL211;} else{ goto _LL212;} _LL212: goto _LL213; _LL211: return 1; _LL213:
return 0; _LL209:;} static void* Cyc_Parse_id2type( struct _tagged_string s,
struct Cyc_Absyn_Conref* k){ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({
char* _temp220=( char*)"`H"; struct _tagged_string _temp221; _temp221.curr=
_temp220; _temp221.base= _temp220; _temp221.last_plus_one= _temp220 + 3;
_temp221;})) == 0){ return Cyc_Absyn_HeapRgn;} else{ return({ struct Cyc_Absyn_VarType_struct*
_temp222=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp222->tag= Cyc_Absyn_VarType_tag; _temp222->f1=({ struct Cyc_Absyn_Tvar*
_temp223=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof( struct Cyc_Absyn_Tvar));
_temp223->name=({ struct _tagged_string* _temp224=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp224[ 0]= s; _temp224;});
_temp223->kind= k; _temp223;});( void*) _temp222;});}} static struct Cyc_Absyn_Tvar*
Cyc_Parse_typ2tvar( struct Cyc_Position_Segment* loc, void* t){ void* _temp225=
t; struct Cyc_Absyn_Tvar* _temp231; _LL227: if(( unsigned int) _temp225 > 4u?((
struct _enum_struct*) _temp225)->tag == Cyc_Absyn_VarType_tag: 0){ _LL232:
_temp231=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*) _temp225)->f1;
goto _LL228;} else{ goto _LL229;} _LL229: goto _LL230; _LL228: return _temp231;
_LL230: return(( struct Cyc_Absyn_Tvar*(*)( struct _tagged_string msg, struct
Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct _tagged_string)({ char*
_temp233=( char*)"expecting a list of type variables, not types"; struct
_tagged_string _temp234; _temp234.curr= _temp233; _temp234.base= _temp233;
_temp234.last_plus_one= _temp233 + 46; _temp234;}), loc); _LL226:;} static void*
Cyc_Parse_tvar2typ( struct Cyc_Absyn_Tvar* pr){ return({ struct Cyc_Absyn_VarType_struct*
_temp235=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp235->tag= Cyc_Absyn_VarType_tag; _temp235->f1= pr;( void*) _temp235;});}
static struct Cyc_List_List* Cyc_Parse_oldstyle2newstyle( struct Cyc_List_List*
tms, struct Cyc_List_List* tds, struct Cyc_Position_Segment* loc){ if( tds == 0){
return tms;} if( tms == 0){ return 0;}{ void* _temp236=( void*) tms->hd; void*
_temp242; _LL238: if(( unsigned int) _temp236 > 1u?(( struct _enum_struct*)
_temp236)->tag == Cyc_Absyn_Function_mod_tag: 0){ _LL243: _temp242=( void*)((
struct Cyc_Absyn_Function_mod_struct*) _temp236)->f1; goto _LL239;} else{ goto
_LL240;} _LL240: goto _LL241; _LL239: if( tms->tl == 0? 1:( Cyc_Parse_is_typeparam((
void*)( tms->tl)->hd)?( tms->tl)->tl == 0: 0)){ void* _temp244= _temp242; struct
Cyc_Core_Opt* _temp250; int _temp252; struct Cyc_List_List* _temp254; struct Cyc_Position_Segment*
_temp256; struct Cyc_List_List* _temp258; _LL246: if((( struct _enum_struct*)
_temp244)->tag == Cyc_Absyn_WithTypes_tag){ _LL255: _temp254=( struct Cyc_List_List*)((
struct Cyc_Absyn_WithTypes_struct*) _temp244)->f1; goto _LL253; _LL253: _temp252=(
int)(( struct Cyc_Absyn_WithTypes_struct*) _temp244)->f2; goto _LL251; _LL251:
_temp250=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_WithTypes_struct*) _temp244)->f3;
goto _LL247;} else{ goto _LL248;} _LL248: if((( struct _enum_struct*) _temp244)->tag
== Cyc_Absyn_NoTypes_tag){ _LL259: _temp258=( struct Cyc_List_List*)(( struct
Cyc_Absyn_NoTypes_struct*) _temp244)->f1; goto _LL257; _LL257: _temp256=( struct
Cyc_Position_Segment*)(( struct Cyc_Absyn_NoTypes_struct*) _temp244)->f2; goto
_LL249;} else{ goto _LL245;} _LL247: Cyc_Parse_warn(( struct _tagged_string)({
char* _temp260=( char*)"function declaration with both new- and old-style parameterdeclarations; ignoring old-style";
struct _tagged_string _temp261; _temp261.curr= _temp260; _temp261.base= _temp260;
_temp261.last_plus_one= _temp260 + 92; _temp261;}), loc); return tms; _LL249:((
void(*)( void(* f)( struct Cyc_List_List*, struct Cyc_Absyn_Decl*), struct Cyc_List_List*
env, struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Parse_only_vardecl,
_temp258, tds);{ struct _tuple10* env=({ struct _tuple10* _temp262=( struct
_tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp262->f1= tds; _temp262->f2=
loc; _temp262;}); return({ struct Cyc_List_List* _temp263=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp263->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp264=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp264->tag= Cyc_Absyn_Function_mod_tag; _temp264->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp265=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp265->tag= Cyc_Absyn_WithTypes_tag; _temp265->f1=(( struct Cyc_List_List*(*)(
struct _tuple2*(* f)( struct _tuple10*, struct _tagged_string*), struct _tuple10*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_get_param_type, env,
_temp258); _temp265->f2= 0; _temp265->f3= 0;( void*) _temp265;});( void*)
_temp264;}); _temp263->tl= 0; _temp263;});} _LL245:;} else{ return({ struct Cyc_List_List*
_temp266=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp266->hd=( void*)(( void*) tms->hd); _temp266->tl= Cyc_Parse_oldstyle2newstyle(
tms->tl, tds, loc); _temp266;});} _LL241: return({ struct Cyc_List_List*
_temp267=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp267->hd=( void*)(( void*) tms->hd); _temp267->tl= Cyc_Parse_oldstyle2newstyle(
tms->tl, tds, loc); _temp267;}); _LL237:;}} static struct Cyc_Absyn_Fndecl* Cyc_Parse_make_function(
struct Cyc_Core_Opt* dso, struct Cyc_Parse_Declarator* d, struct Cyc_List_List*
tds, struct Cyc_Absyn_Stmt* body, struct Cyc_Position_Segment* loc){ if( tds !=
0){ d=({ struct Cyc_Parse_Declarator* _temp268=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp268->id= d->id; _temp268->tms=
Cyc_Parse_oldstyle2newstyle( d->tms, tds, loc); _temp268;});}{ void* sc= Cyc_Absyn_Public;
struct Cyc_List_List* tss= 0; struct Cyc_Absyn_Tqual* tq= Cyc_Absyn_empty_tqual();
int is_inline= 0; struct Cyc_List_List* atts= 0; if( dso != 0){ tss=(( struct
Cyc_Parse_Declaration_spec*) dso->v)->type_specs; tq=(( struct Cyc_Parse_Declaration_spec*)
dso->v)->tq; is_inline=(( struct Cyc_Parse_Declaration_spec*) dso->v)->is_inline;
atts=(( struct Cyc_Parse_Declaration_spec*) dso->v)->attributes; if((( struct
Cyc_Parse_Declaration_spec*) dso->v)->sc != 0){ void* _temp269=( void*)(((
struct Cyc_Parse_Declaration_spec*) dso->v)->sc)->v; _LL271: if( _temp269 == Cyc_Parse_Extern_sc){
goto _LL272;} else{ goto _LL273;} _LL273: if( _temp269 == Cyc_Parse_ExternC_sc){
goto _LL274;} else{ goto _LL275;} _LL275: if( _temp269 == Cyc_Parse_Static_sc){
goto _LL276;} else{ goto _LL277;} _LL277: goto _LL278; _LL272: sc= Cyc_Absyn_Extern;
goto _LL270; _LL274: sc= Cyc_Absyn_ExternC; goto _LL270; _LL276: sc= Cyc_Absyn_Static;
goto _LL270; _LL278: Cyc_Parse_err(( struct _tagged_string)({ char* _temp279=(
char*)"bad storage class on function"; struct _tagged_string _temp280; _temp280.curr=
_temp279; _temp280.base= _temp279; _temp280.last_plus_one= _temp279 + 30;
_temp280;}), loc); goto _LL270; _LL270:;}}{ struct Cyc_Core_Opt* _temp283; void*
_temp285; struct _tuple5 _temp281= Cyc_Parse_collapse_type_specifiers( tss, loc);
_LL286: _temp285= _temp281.f1; goto _LL284; _LL284: _temp283= _temp281.f2; goto
_LL282; _LL282: { struct Cyc_List_List* _temp289; struct Cyc_List_List* _temp291;
void* _temp293; struct Cyc_Absyn_Tqual* _temp295; struct _tuple6 _temp287= Cyc_Parse_apply_tms(
tq, _temp285, atts, d->tms); _LL296: _temp295= _temp287.f1; goto _LL294; _LL294:
_temp293= _temp287.f2; goto _LL292; _LL292: _temp291= _temp287.f3; goto _LL290;
_LL290: _temp289= _temp287.f4; goto _LL288; _LL288: if( _temp283 != 0){ Cyc_Parse_warn((
struct _tagged_string)({ char* _temp297=( char*)"nested type declaration within function prototype";
struct _tagged_string _temp298; _temp298.curr= _temp297; _temp298.base= _temp297;
_temp298.last_plus_one= _temp297 + 50; _temp298;}), loc);} if( _temp291 != 0){
Cyc_Parse_warn(( struct _tagged_string)({ char* _temp299=( char*)"bad type params, ignoring";
struct _tagged_string _temp300; _temp300.curr= _temp299; _temp300.base= _temp299;
_temp300.last_plus_one= _temp299 + 26; _temp300;}), loc);}{ void* _temp301=
_temp293; struct Cyc_Absyn_FnInfo _temp307; struct Cyc_List_List* _temp309; int
_temp311; struct Cyc_List_List* _temp313; void* _temp315; struct Cyc_Core_Opt*
_temp317; struct Cyc_List_List* _temp319; _LL303: if(( unsigned int) _temp301 >
4u?(( struct _enum_struct*) _temp301)->tag == Cyc_Absyn_FnType_tag: 0){ _LL308:
_temp307=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*) _temp301)->f1;
_LL320: _temp319=( struct Cyc_List_List*) _temp307.tvars; goto _LL318; _LL318:
_temp317=( struct Cyc_Core_Opt*) _temp307.effect; goto _LL316; _LL316: _temp315=(
void*) _temp307.ret_typ; goto _LL314; _LL314: _temp313=( struct Cyc_List_List*)
_temp307.args; goto _LL312; _LL312: _temp311=( int) _temp307.varargs; goto
_LL310; _LL310: _temp309=( struct Cyc_List_List*) _temp307.attributes; goto
_LL304;} else{ goto _LL305;} _LL305: goto _LL306; _LL304: { struct Cyc_List_List*
args2=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_fnargspec_to_arg, loc, _temp313); return({ struct Cyc_Absyn_Fndecl*
_temp321=( struct Cyc_Absyn_Fndecl*) GC_malloc( sizeof( struct Cyc_Absyn_Fndecl));
_temp321->sc=( void*) sc; _temp321->name= d->id; _temp321->tvs= _temp319;
_temp321->is_inline= is_inline; _temp321->effect= _temp317; _temp321->ret_type=(
void*) _temp315; _temp321->args= args2; _temp321->varargs= _temp311; _temp321->body=
body; _temp321->cached_typ= 0; _temp321->param_vardecls= 0; _temp321->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp309, _temp289); _temp321;});} _LL306: return(( struct Cyc_Absyn_Fndecl*(*)(
struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_string)({ char* _temp322=( char*)"declarator is not a function prototype";
struct _tagged_string _temp323; _temp323.curr= _temp322; _temp323.base= _temp322;
_temp323.last_plus_one= _temp322 + 39; _temp323;}), loc); _LL302:;}}}}} static
struct _tuple4* Cyc_Parse_fnargspec_to_arg( struct Cyc_Position_Segment* loc,
struct _tuple2* t){ if((* t).f1 == 0){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp324=( char*)"missing argument variable in function prototype"; struct
_tagged_string _temp325; _temp325.curr= _temp324; _temp325.base= _temp324;
_temp325.last_plus_one= _temp324 + 48; _temp325;}), loc); return({ struct
_tuple4* _temp326=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp326->f1=({ struct _tagged_string* _temp327=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp327[ 0]=( struct
_tagged_string)({ char* _temp328=( char*)"?"; struct _tagged_string _temp329;
_temp329.curr= _temp328; _temp329.base= _temp328; _temp329.last_plus_one=
_temp328 + 2; _temp329;}); _temp327;}); _temp326->f2=(* t).f2; _temp326->f3=(* t).f3;
_temp326;});} else{ return({ struct _tuple4* _temp330=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp330->f1=( struct _tagged_string*)((* t).f1)->v;
_temp330->f2=(* t).f2; _temp330->f3=(* t).f3; _temp330;});}} static char
_temp333[ 52u]="at most one type may appear within a type specifier"; static
struct _tagged_string Cyc_Parse_msg1=( struct _tagged_string){ _temp333,
_temp333, _temp333 + 52u}; static char _temp336[ 63u]="const or volatile may appear only once within a type specifier";
static struct _tagged_string Cyc_Parse_msg2=( struct _tagged_string){ _temp336,
_temp336, _temp336 + 63u}; static char _temp339[ 50u]="type specifier includes more than one declaration";
static struct _tagged_string Cyc_Parse_msg3=( struct _tagged_string){ _temp339,
_temp339, _temp339 + 50u}; static char _temp342[ 60u]="sign specifier may appear only once within a type specifier";
static struct _tagged_string Cyc_Parse_msg4=( struct _tagged_string){ _temp342,
_temp342, _temp342 + 60u}; static struct _tuple5 Cyc_Parse_collapse_type_specifiers(
struct Cyc_List_List* ts, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt*
declopt= 0; int seen_type= 0; int seen_sign= 0; int seen_size= 0; void* t= Cyc_Absyn_VoidType;
void* sz= Cyc_Absyn_B4; void* sgn= Cyc_Absyn_Signed; struct Cyc_Position_Segment*
last_loc= loc; for( 0; ts != 0; ts= ts->tl){ void* _temp343=( void*) ts->hd;
struct Cyc_Position_Segment* _temp357; void* _temp359; struct Cyc_Position_Segment*
_temp361; struct Cyc_Position_Segment* _temp363; struct Cyc_Position_Segment*
_temp365; struct Cyc_Position_Segment* _temp367; struct Cyc_Absyn_Decl* _temp369;
_LL345: if((( struct _enum_struct*) _temp343)->tag == Cyc_Parse_Type_spec_tag){
_LL360: _temp359=( void*)(( struct Cyc_Parse_Type_spec_struct*) _temp343)->f1;
goto _LL358; _LL358: _temp357=( struct Cyc_Position_Segment*)(( struct Cyc_Parse_Type_spec_struct*)
_temp343)->f2; goto _LL346;} else{ goto _LL347;} _LL347: if((( struct
_enum_struct*) _temp343)->tag == Cyc_Parse_Signed_spec_tag){ _LL362: _temp361=(
struct Cyc_Position_Segment*)(( struct Cyc_Parse_Signed_spec_struct*) _temp343)->f1;
goto _LL348;} else{ goto _LL349;} _LL349: if((( struct _enum_struct*) _temp343)->tag
== Cyc_Parse_Unsigned_spec_tag){ _LL364: _temp363=( struct Cyc_Position_Segment*)((
struct Cyc_Parse_Unsigned_spec_struct*) _temp343)->f1; goto _LL350;} else{ goto
_LL351;} _LL351: if((( struct _enum_struct*) _temp343)->tag == Cyc_Parse_Short_spec_tag){
_LL366: _temp365=( struct Cyc_Position_Segment*)(( struct Cyc_Parse_Short_spec_struct*)
_temp343)->f1; goto _LL352;} else{ goto _LL353;} _LL353: if((( struct
_enum_struct*) _temp343)->tag == Cyc_Parse_Long_spec_tag){ _LL368: _temp367=(
struct Cyc_Position_Segment*)(( struct Cyc_Parse_Long_spec_struct*) _temp343)->f1;
goto _LL354;} else{ goto _LL355;} _LL355: if((( struct _enum_struct*) _temp343)->tag
== Cyc_Parse_Decl_spec_tag){ _LL370: _temp369=( struct Cyc_Absyn_Decl*)(( struct
Cyc_Parse_Decl_spec_struct*) _temp343)->f1; goto _LL356;} else{ goto _LL344;}
_LL346: if( seen_type){ Cyc_Parse_err( Cyc_Parse_msg1, _temp357);} last_loc=
_temp357; seen_type= 1; t= _temp359; goto _LL344; _LL348: if( seen_sign){ Cyc_Parse_err(
Cyc_Parse_msg4, _temp361);} if( seen_type){ Cyc_Parse_err(( struct
_tagged_string)({ char* _temp371=( char*)"signed qualifier must come before type";
struct _tagged_string _temp372; _temp372.curr= _temp371; _temp372.base= _temp371;
_temp372.last_plus_one= _temp371 + 39; _temp372;}), _temp361);} last_loc=
_temp361; seen_sign= 1; sgn= Cyc_Absyn_Signed; goto _LL344; _LL350: if(
seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp363);} if( seen_type){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp373=( char*)"signed qualifier must come before type";
struct _tagged_string _temp374; _temp374.curr= _temp373; _temp374.base= _temp373;
_temp374.last_plus_one= _temp373 + 39; _temp374;}), _temp363);} last_loc=
_temp363; seen_sign= 1; sgn= Cyc_Absyn_Unsigned; goto _LL344; _LL352: if(
seen_size){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp375=( char*)"integral size may appear only once within a type specifier";
struct _tagged_string _temp376; _temp376.curr= _temp375; _temp376.base= _temp375;
_temp376.last_plus_one= _temp375 + 59; _temp376;}), _temp365);} if( seen_type){
Cyc_Parse_err(( struct _tagged_string)({ char* _temp377=( char*)"short modifier must come before base type";
struct _tagged_string _temp378; _temp378.curr= _temp377; _temp378.base= _temp377;
_temp378.last_plus_one= _temp377 + 42; _temp378;}), _temp365);} last_loc=
_temp365; seen_size= 1; sz= Cyc_Absyn_B2; goto _LL344; _LL354: if( seen_type){
Cyc_Parse_err(( struct _tagged_string)({ char* _temp379=( char*)"long modifier must come before base type";
struct _tagged_string _temp380; _temp380.curr= _temp379; _temp380.base= _temp379;
_temp380.last_plus_one= _temp379 + 41; _temp380;}), _temp367);} if( seen_size){
void* _temp381= sz; _LL383: if( _temp381 == Cyc_Absyn_B4){ goto _LL384;} else{
goto _LL385;} _LL385: goto _LL386; _LL384: sz= Cyc_Absyn_B8; goto _LL382; _LL386:
Cyc_Parse_err(( struct _tagged_string)({ char* _temp387=( char*)"extra long in type specifier";
struct _tagged_string _temp388; _temp388.curr= _temp387; _temp388.base= _temp387;
_temp388.last_plus_one= _temp387 + 29; _temp388;}), _temp367); goto _LL382;
_LL382:;} last_loc= _temp367; seen_size= 1; goto _LL344; _LL356: last_loc=
_temp369->loc; if( declopt == 0? ! seen_type: 0){ seen_type= 1;{ void* _temp389=(
void*) _temp369->r; struct Cyc_Absyn_Structdecl* _temp401; struct Cyc_Absyn_Enumdecl*
_temp403; struct Cyc_Absyn_Xenumdecl* _temp405; struct Cyc_Absyn_Uniondecl*
_temp407; _LL391: if((( struct _enum_struct*) _temp389)->tag == Cyc_Absyn_Struct_d_tag){
_LL402: _temp401=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*)
_temp389)->f1; goto _LL392;} else{ goto _LL393;} _LL393: if((( struct
_enum_struct*) _temp389)->tag == Cyc_Absyn_Enum_d_tag){ _LL404: _temp403=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*) _temp389)->f1;
goto _LL394;} else{ goto _LL395;} _LL395: if((( struct _enum_struct*) _temp389)->tag
== Cyc_Absyn_Xenum_d_tag){ _LL406: _temp405=( struct Cyc_Absyn_Xenumdecl*)((
struct Cyc_Absyn_Xenum_d_struct*) _temp389)->f1; goto _LL396;} else{ goto _LL397;}
_LL397: if((( struct _enum_struct*) _temp389)->tag == Cyc_Absyn_Union_d_tag){
_LL408: _temp407=( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*)
_temp389)->f1; goto _LL398;} else{ goto _LL399;} _LL399: goto _LL400; _LL392: {
struct Cyc_List_List* args=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*),
struct Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp401->tvs); t=({
struct Cyc_Absyn_StructType_struct* _temp409=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp409->tag= Cyc_Absyn_StructType_tag;
_temp409->f1=( struct _tuple1*)(( struct _tuple1*)( _temp401->name)->v);
_temp409->f2= args; _temp409->f3= 0;( void*) _temp409;}); if( _temp401->fields
!= 0){ declopt=({ struct Cyc_Core_Opt* _temp410=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp410->v=( void*) _temp369;
_temp410;});} goto _LL390;} _LL394: { struct Cyc_List_List* args=(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Parse_tvar2typ, _temp403->tvs); t=({ struct Cyc_Absyn_EnumType_struct*
_temp411=( struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp411->tag= Cyc_Absyn_EnumType_tag; _temp411->f1=( struct _tuple1*)(( struct
_tuple1*)( _temp403->name)->v); _temp411->f2= args; _temp411->f3= 0;( void*)
_temp411;}); if( _temp403->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp412=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp412->v=( void*) _temp369; _temp412;});} goto _LL390;} _LL396: t=({ struct
Cyc_Absyn_XenumType_struct* _temp413=( struct Cyc_Absyn_XenumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XenumType_struct)); _temp413->tag= Cyc_Absyn_XenumType_tag;
_temp413->f1= _temp405->name; _temp413->f2= 0;( void*) _temp413;}); if( _temp405->fields
!= 0){ declopt=({ struct Cyc_Core_Opt* _temp414=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp414->v=( void*) _temp369;
_temp414;});} goto _LL390; _LL398: { struct Cyc_List_List* args=(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Parse_tvar2typ, _temp407->tvs); t=({ struct Cyc_Absyn_UnionType_struct*
_temp415=( struct Cyc_Absyn_UnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp415->tag= Cyc_Absyn_UnionType_tag; _temp415->f1=( struct _tuple1*)(( struct
_tuple1*)( _temp407->name)->v); _temp415->f2= args; _temp415->f3= 0;( void*)
_temp415;}); if( _temp407->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp416=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp416->v=( void*) _temp369; _temp416;});} goto _LL390;} _LL400:(( void(*)(
struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_string)({ char* _temp417=( char*)"bad declaration within type specifier";
struct _tagged_string _temp418; _temp418.curr= _temp417; _temp418.base= _temp417;
_temp418.last_plus_one= _temp417 + 38; _temp418;}), _temp369->loc); goto _LL390;
_LL390:;}} else{ Cyc_Parse_err( Cyc_Parse_msg3, _temp369->loc);} goto _LL344;
_LL344:;} if( ! seen_type){ if( ! seen_sign? ! seen_size: 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp419=( char*)"missing type withing specifier";
struct _tagged_string _temp420; _temp420.curr= _temp419; _temp420.base= _temp419;
_temp420.last_plus_one= _temp419 + 31; _temp420;}), last_loc);} t=({ struct Cyc_Absyn_IntType_struct*
_temp421=( struct Cyc_Absyn_IntType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp421->tag= Cyc_Absyn_IntType_tag; _temp421->f1=( void*) sgn; _temp421->f2=(
void*) sz;( void*) _temp421;});} else{ if( seen_sign){ void* _temp422= t; void*
_temp428; void* _temp430; _LL424: if(( unsigned int) _temp422 > 4u?(( struct
_enum_struct*) _temp422)->tag == Cyc_Absyn_IntType_tag: 0){ _LL431: _temp430=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp422)->f1; goto _LL429; _LL429:
_temp428=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp422)->f2; goto _LL425;}
else{ goto _LL426;} _LL426: goto _LL427; _LL425: t=({ struct Cyc_Absyn_IntType_struct*
_temp432=( struct Cyc_Absyn_IntType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp432->tag= Cyc_Absyn_IntType_tag; _temp432->f1=( void*) sgn; _temp432->f2=(
void*) _temp428;( void*) _temp432;}); goto _LL423; _LL427: Cyc_Parse_err((
struct _tagged_string)({ char* _temp433=( char*)"sign specification on non-integral type";
struct _tagged_string _temp434; _temp434.curr= _temp433; _temp434.base= _temp433;
_temp434.last_plus_one= _temp433 + 40; _temp434;}), last_loc); goto _LL423;
_LL423:;} if( seen_size){ void* _temp435= t; void* _temp441; void* _temp443;
_LL437: if(( unsigned int) _temp435 > 4u?(( struct _enum_struct*) _temp435)->tag
== Cyc_Absyn_IntType_tag: 0){ _LL444: _temp443=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp435)->f1; goto _LL442; _LL442: _temp441=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp435)->f2; goto _LL438;} else{ goto _LL439;} _LL439: goto _LL440; _LL438: t=({
struct Cyc_Absyn_IntType_struct* _temp445=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp445->tag= Cyc_Absyn_IntType_tag;
_temp445->f1=( void*) _temp443; _temp445->f2=( void*) sz;( void*) _temp445;});
goto _LL436; _LL440: Cyc_Parse_err(( struct _tagged_string)({ char* _temp446=(
char*)"size qualifier on non-integral type"; struct _tagged_string _temp447;
_temp447.curr= _temp446; _temp447.base= _temp446; _temp447.last_plus_one=
_temp446 + 36; _temp447;}), last_loc); goto _LL436; _LL436:;}} return({ struct
_tuple5 _temp448; _temp448.f1= t; _temp448.f2= declopt; _temp448;});} static
struct Cyc_List_List* Cyc_Parse_apply_tmss( struct Cyc_Absyn_Tqual* tq, void* t,
struct Cyc_List_List* ds, struct Cyc_List_List* shared_atts){ if( ds == 0){
return 0;}{ struct Cyc_Parse_Declarator* d=( struct Cyc_Parse_Declarator*) ds->hd;
struct _tuple1* q= d->id; struct Cyc_List_List* _temp451; struct Cyc_List_List*
_temp453; void* _temp455; struct Cyc_Absyn_Tqual* _temp457; struct _tuple6
_temp449= Cyc_Parse_apply_tms( tq, t, shared_atts, d->tms); _LL458: _temp457=
_temp449.f1; goto _LL456; _LL456: _temp455= _temp449.f2; goto _LL454; _LL454:
_temp453= _temp449.f3; goto _LL452; _LL452: _temp451= _temp449.f4; goto _LL450;
_LL450: return({ struct Cyc_List_List* _temp459=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp459->hd=( void*)({ struct
_tuple7* _temp460=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
_temp460->f1= q; _temp460->f2= _temp457; _temp460->f3= _temp455; _temp460->f4=
_temp453; _temp460->f5= _temp451; _temp460;}); _temp459->tl= Cyc_Parse_apply_tmss(
_temp457, t, ds->tl, shared_atts); _temp459;});}} static int Cyc_Parse_fn_type_att(
void* a){ void* _temp461= a; int _temp475; _LL463: if(( unsigned int) _temp461 >
15u?(( struct _enum_struct*) _temp461)->tag == Cyc_Absyn_Regparm_att_tag: 0){
_LL476: _temp475=( int)(( struct Cyc_Absyn_Regparm_att_struct*) _temp461)->f1;
goto _LL464;} else{ goto _LL465;} _LL465: if( _temp461 == Cyc_Absyn_Stdcall_att){
goto _LL466;} else{ goto _LL467;} _LL467: if( _temp461 == Cyc_Absyn_Cdecl_att){
goto _LL468;} else{ goto _LL469;} _LL469: if( _temp461 == Cyc_Absyn_Noreturn_att){
goto _LL470;} else{ goto _LL471;} _LL471: if( _temp461 == Cyc_Absyn_Const_att){
goto _LL472;} else{ goto _LL473;} _LL473: goto _LL474; _LL464: goto _LL466;
_LL466: goto _LL468; _LL468: goto _LL470; _LL470: goto _LL472; _LL472: return 1;
_LL474: return 0; _LL462:;} static struct _tuple6 Cyc_Parse_apply_tms( struct
Cyc_Absyn_Tqual* tq, void* t, struct Cyc_List_List* atts, struct Cyc_List_List*
tms){ if( tms == 0){ return({ struct _tuple6 _temp477; _temp477.f1= tq; _temp477.f2=
t; _temp477.f3= 0; _temp477.f4= atts; _temp477;});}{ void* _temp478=( void*) tms->hd;
struct Cyc_Absyn_Exp* _temp492; void* _temp494; int _temp496; struct Cyc_Position_Segment*
_temp498; struct Cyc_List_List* _temp500; struct Cyc_Absyn_Tqual* _temp502; void*
_temp504; void* _temp506; struct Cyc_List_List* _temp508; struct Cyc_Position_Segment*
_temp510; _LL480: if( _temp478 == Cyc_Absyn_Carray_mod){ goto _LL481;} else{
goto _LL482;} _LL482: if(( unsigned int) _temp478 > 1u?(( struct _enum_struct*)
_temp478)->tag == Cyc_Absyn_ConstArray_mod_tag: 0){ _LL493: _temp492=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ConstArray_mod_struct*) _temp478)->f1; goto
_LL483;} else{ goto _LL484;} _LL484: if(( unsigned int) _temp478 > 1u?(( struct
_enum_struct*) _temp478)->tag == Cyc_Absyn_Function_mod_tag: 0){ _LL495:
_temp494=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp478)->f1; goto
_LL485;} else{ goto _LL486;} _LL486: if(( unsigned int) _temp478 > 1u?(( struct
_enum_struct*) _temp478)->tag == Cyc_Absyn_TypeParams_mod_tag: 0){ _LL501:
_temp500=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp478)->f1; goto _LL499; _LL499: _temp498=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp478)->f2; goto _LL497; _LL497:
_temp496=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp478)->f3; goto
_LL487;} else{ goto _LL488;} _LL488: if(( unsigned int) _temp478 > 1u?(( struct
_enum_struct*) _temp478)->tag == Cyc_Absyn_Pointer_mod_tag: 0){ _LL507: _temp506=(
void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp478)->f1; goto _LL505;
_LL505: _temp504=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp478)->f2;
goto _LL503; _LL503: _temp502=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp478)->f3; goto _LL489;} else{ goto _LL490;} _LL490: if(( unsigned int)
_temp478 > 1u?(( struct _enum_struct*) _temp478)->tag == Cyc_Absyn_Attributes_mod_tag:
0){ _LL511: _temp510=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp478)->f1; goto _LL509; _LL509: _temp508=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Attributes_mod_struct*) _temp478)->f2; goto _LL491;} else{ goto _LL479;}
_LL481: return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(),({ struct Cyc_Absyn_ArrayType_struct*
_temp512=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp512->tag= Cyc_Absyn_ArrayType_tag; _temp512->f1=( void*) t; _temp512->f2=
tq; _temp512->f3= Cyc_Absyn_uint_exp( 0, 0);( void*) _temp512;}), atts, tms->tl);
_LL483: return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(),({ struct Cyc_Absyn_ArrayType_struct*
_temp513=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp513->tag= Cyc_Absyn_ArrayType_tag; _temp513->f1=( void*) t; _temp513->f2=
tq; _temp513->f3= _temp492;( void*) _temp513;}), atts, tms->tl); _LL485: { void*
_temp514= _temp494; struct Cyc_Core_Opt* _temp520; int _temp522; struct Cyc_List_List*
_temp524; struct Cyc_Position_Segment* _temp526; struct Cyc_List_List* _temp528;
_LL516: if((( struct _enum_struct*) _temp514)->tag == Cyc_Absyn_WithTypes_tag){
_LL525: _temp524=( struct Cyc_List_List*)(( struct Cyc_Absyn_WithTypes_struct*)
_temp514)->f1; goto _LL523; _LL523: _temp522=( int)(( struct Cyc_Absyn_WithTypes_struct*)
_temp514)->f2; goto _LL521; _LL521: _temp520=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_WithTypes_struct*) _temp514)->f3; goto _LL517;} else{ goto _LL518;}
_LL518: if((( struct _enum_struct*) _temp514)->tag == Cyc_Absyn_NoTypes_tag){
_LL529: _temp528=( struct Cyc_List_List*)(( struct Cyc_Absyn_NoTypes_struct*)
_temp514)->f1; goto _LL527; _LL527: _temp526=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_NoTypes_struct*) _temp514)->f2; goto _LL519;} else{ goto _LL515;}
_LL517: { struct Cyc_List_List* typvars= 0; struct Cyc_List_List* fn_atts= 0;
struct Cyc_List_List* new_atts= 0;{ struct Cyc_List_List* as= atts; for( 0; as
!= 0; as= as->tl){ if( Cyc_Parse_fn_type_att(( void*) as->hd)){ fn_atts=({
struct Cyc_List_List* _temp530=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp530->hd=( void*)(( void*) as->hd); _temp530->tl=
fn_atts; _temp530;});} else{ new_atts=({ struct Cyc_List_List* _temp531=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp531->hd=( void*)((
void*) as->hd); _temp531->tl= new_atts; _temp531;});}}} if( tms->tl != 0){ void*
_temp532=( void*)( tms->tl)->hd; int _temp538; struct Cyc_Position_Segment*
_temp540; struct Cyc_List_List* _temp542; _LL534: if(( unsigned int) _temp532 >
1u?(( struct _enum_struct*) _temp532)->tag == Cyc_Absyn_TypeParams_mod_tag: 0){
_LL543: _temp542=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp532)->f1; goto _LL541; _LL541: _temp540=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp532)->f2; goto _LL539; _LL539:
_temp538=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp532)->f3; goto
_LL535;} else{ goto _LL536;} _LL536: goto _LL537; _LL535: typvars= _temp542; tms=
tms->tl; goto _LL533; _LL537: goto _LL533; _LL533:;} if(((( ! _temp522? _temp524
!= 0: 0)? _temp524->tl == 0: 0)?(*(( struct _tuple2*) _temp524->hd)).f1 == 0: 0)?(*((
struct _tuple2*) _temp524->hd)).f3 == Cyc_Absyn_VoidType: 0){ _temp524= 0;
_temp522= 0;} t= Cyc_Parse_array2ptr( t);(( void(*)( void(* f)( struct _tuple2*),
struct Cyc_List_List* x)) Cyc_List_iter)( Cyc_Parse_arg_array2ptr, _temp524);
return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(), Cyc_Absyn_function_typ(
typvars, _temp520, t, _temp524, _temp522, fn_atts), new_atts, tms->tl);} _LL519:(
void) _throw((( struct _xenum_struct*(*)( struct _tagged_string msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)(( struct _tagged_string)({ char* _temp544=( char*)"function declaration without parameter types";
struct _tagged_string _temp545; _temp545.curr= _temp544; _temp545.base= _temp544;
_temp545.last_plus_one= _temp544 + 45; _temp545;}), _temp526)); _LL515:;} _LL487:
if( tms->tl == 0){ return({ struct _tuple6 _temp546; _temp546.f1= tq; _temp546.f2=
t; _temp546.f3= _temp500; _temp546.f4= atts; _temp546;});}( void) _throw(((
struct _xenum_struct*(*)( struct _tagged_string msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)(( struct _tagged_string)({ char* _temp547=( char*)"type parameters must appear before function arguments in declarator";
struct _tagged_string _temp548; _temp548.curr= _temp547; _temp548.base= _temp547;
_temp548.last_plus_one= _temp547 + 68; _temp548;}), _temp498)); _LL489: { void*
_temp549= _temp506; struct Cyc_Absyn_Exp* _temp557; struct Cyc_Absyn_Exp*
_temp559; _LL551: if(( unsigned int) _temp549 > 1u?(( struct _enum_struct*)
_temp549)->tag == Cyc_Absyn_NonNullable_ps_tag: 0){ _LL558: _temp557=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp549)->f1; goto
_LL552;} else{ goto _LL553;} _LL553: if(( unsigned int) _temp549 > 1u?(( struct
_enum_struct*) _temp549)->tag == Cyc_Absyn_Nullable_ps_tag: 0){ _LL560: _temp559=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Nullable_ps_struct*) _temp549)->f1;
goto _LL554;} else{ goto _LL555;} _LL555: if( _temp549 == Cyc_Absyn_TaggedArray_ps){
goto _LL556;} else{ goto _LL550;} _LL552: return Cyc_Parse_apply_tms( _temp502,
Cyc_Absyn_atb_typ( t, _temp504, tq,({ struct Cyc_Absyn_Upper_b_struct* _temp561=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp561->tag= Cyc_Absyn_Upper_b_tag; _temp561->f1= _temp557;( void*) _temp561;})),
atts, tms->tl); _LL554: return Cyc_Parse_apply_tms( _temp502, Cyc_Absyn_starb_typ(
t, _temp504, tq,({ struct Cyc_Absyn_Upper_b_struct* _temp562=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp562->tag= Cyc_Absyn_Upper_b_tag;
_temp562->f1= _temp559;( void*) _temp562;})), atts, tms->tl); _LL556: return Cyc_Parse_apply_tms(
_temp502, Cyc_Absyn_tagged_typ( t, _temp504, tq), atts, tms->tl); _LL550:;}
_LL491: return Cyc_Parse_apply_tms( tq, t,(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( atts, _temp508),
tms->tl); _LL479:;}} void* Cyc_Parse_speclist2typ( struct Cyc_List_List* tss,
struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt* _temp565; void* _temp567;
struct _tuple5 _temp563= Cyc_Parse_collapse_type_specifiers( tss, loc); _LL568:
_temp567= _temp563.f1; goto _LL566; _LL566: _temp565= _temp563.f2; goto _LL564;
_LL564: if( _temp565 != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char*
_temp569=( char*)"ignoring nested type declaration(s) in specifier list"; struct
_tagged_string _temp570; _temp570.curr= _temp569; _temp570.base= _temp569;
_temp570.last_plus_one= _temp569 + 54; _temp570;}), loc);} return _temp567;}
static struct Cyc_Absyn_Stmt* Cyc_Parse_flatten_decl( struct Cyc_Absyn_Decl* d,
struct Cyc_Absyn_Stmt* s){ return Cyc_Absyn_new_stmt(({ struct Cyc_Absyn_Decl_s_struct*
_temp571=( struct Cyc_Absyn_Decl_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
_temp571->tag= Cyc_Absyn_Decl_s_tag; _temp571->f1= d; _temp571->f2= s;( void*)
_temp571;}), Cyc_Position_segment_join( d->loc, s->loc));} static struct Cyc_Absyn_Stmt*
Cyc_Parse_flatten_declarations( struct Cyc_List_List* ds, struct Cyc_Absyn_Stmt*
s){ return(( struct Cyc_Absyn_Stmt*(*)( struct Cyc_Absyn_Stmt*(* f)( struct Cyc_Absyn_Decl*,
struct Cyc_Absyn_Stmt*), struct Cyc_List_List* x, struct Cyc_Absyn_Stmt* accum))
Cyc_List_fold_right)( Cyc_Parse_flatten_decl, ds, s);} static struct Cyc_List_List*
Cyc_Parse_make_declarations( struct Cyc_Parse_Declaration_spec* ds, struct Cyc_List_List*
ids, struct Cyc_Position_Segment* loc){ struct Cyc_List_List* tss= ds->type_specs;
struct Cyc_Absyn_Tqual* tq= ds->tq; int istypedef= 0; void* s= Cyc_Absyn_Public;
struct Cyc_List_List* atts= ds->attributes; if( ds->is_inline){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp572=( char*)"inline is only allowed on function definitions";
struct _tagged_string _temp573; _temp573.curr= _temp572; _temp573.base= _temp572;
_temp573.last_plus_one= _temp572 + 47; _temp573;}), loc);} if( tss == 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp574=( char*)"missing type specifiers in declaration";
struct _tagged_string _temp575; _temp575.curr= _temp574; _temp575.base= _temp574;
_temp575.last_plus_one= _temp574 + 39; _temp575;}), loc); return 0;} if( ds->sc
!= 0){ void* _temp576=( void*)( ds->sc)->v; _LL578: if( _temp576 == Cyc_Parse_Typedef_sc){
goto _LL579;} else{ goto _LL580;} _LL580: if( _temp576 == Cyc_Parse_Extern_sc){
goto _LL581;} else{ goto _LL582;} _LL582: if( _temp576 == Cyc_Parse_ExternC_sc){
goto _LL583;} else{ goto _LL584;} _LL584: if( _temp576 == Cyc_Parse_Static_sc){
goto _LL585;} else{ goto _LL586;} _LL586: if( _temp576 == Cyc_Parse_Auto_sc){
goto _LL587;} else{ goto _LL588;} _LL588: if( _temp576 == Cyc_Parse_Register_sc){
goto _LL589;} else{ goto _LL590;} _LL590: if( _temp576 == Cyc_Parse_Abstract_sc){
goto _LL591;} else{ goto _LL577;} _LL579: istypedef= 1; goto _LL577; _LL581: s=
Cyc_Absyn_Extern; goto _LL577; _LL583: s= Cyc_Absyn_ExternC; goto _LL577; _LL585:
s= Cyc_Absyn_Static; goto _LL577; _LL587: s= Cyc_Absyn_Public; goto _LL577;
_LL589: s= Cyc_Absyn_Public; goto _LL577; _LL591: s= Cyc_Absyn_Abstract; goto
_LL577; _LL577:;}{ struct Cyc_List_List* _temp594; struct Cyc_List_List*
_temp596; struct _tuple0 _temp592=(( struct _tuple0(*)( struct Cyc_List_List* x))
Cyc_List_split)( ids); _LL597: _temp596= _temp592.f1; goto _LL595; _LL595:
_temp594= _temp592.f2; goto _LL593; _LL593: { int exps_empty= 1;{ struct Cyc_List_List*
es= _temp594; for( 0; es != 0; es= es->tl){ if(( struct Cyc_Absyn_Exp*) es->hd
!= 0){ exps_empty= 0; break;}}}{ struct _tuple5 ts_info= Cyc_Parse_collapse_type_specifiers(
tss, loc); if( _temp596 == 0){ struct Cyc_Core_Opt* _temp600; void* _temp602;
struct _tuple5 _temp598= ts_info; _LL603: _temp602= _temp598.f1; goto _LL601;
_LL601: _temp600= _temp598.f2; goto _LL599; _LL599: if( _temp600 != 0){ struct
Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*) _temp600->v;{ void* _temp604=( void*)
d->r; struct Cyc_Absyn_Structdecl* _temp616; struct Cyc_Absyn_Uniondecl*
_temp618; struct Cyc_Absyn_Enumdecl* _temp620; struct Cyc_Absyn_Xenumdecl*
_temp622; _LL606: if((( struct _enum_struct*) _temp604)->tag == Cyc_Absyn_Struct_d_tag){
_LL617: _temp616=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*)
_temp604)->f1; goto _LL607;} else{ goto _LL608;} _LL608: if((( struct
_enum_struct*) _temp604)->tag == Cyc_Absyn_Union_d_tag){ _LL619: _temp618=(
struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*) _temp604)->f1;
goto _LL609;} else{ goto _LL610;} _LL610: if((( struct _enum_struct*) _temp604)->tag
== Cyc_Absyn_Enum_d_tag){ _LL621: _temp620=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_d_struct*) _temp604)->f1; goto _LL611;} else{ goto _LL612;}
_LL612: if((( struct _enum_struct*) _temp604)->tag == Cyc_Absyn_Xenum_d_tag){
_LL623: _temp622=( struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_d_struct*)
_temp604)->f1; goto _LL613;} else{ goto _LL614;} _LL614: goto _LL615; _LL607:(
void*)( _temp616->sc=( void*) s); _temp616->attributes= atts; goto _LL605;
_LL609:( void*)( _temp618->sc=( void*) s); _temp618->attributes= atts; goto
_LL605; _LL611:( void*)( _temp620->sc=( void*) s); if( atts != 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp624=( char*)"bad attributes on enum"; struct
_tagged_string _temp625; _temp625.curr= _temp624; _temp625.base= _temp624;
_temp625.last_plus_one= _temp624 + 23; _temp625;}), loc);} goto _LL605; _LL613:(
void*)( _temp622->sc=( void*) s); if( atts != 0){ Cyc_Parse_err(( struct
_tagged_string)({ char* _temp626=( char*)"bad attributes on xenum"; struct
_tagged_string _temp627; _temp627.curr= _temp626; _temp627.base= _temp626;
_temp627.last_plus_one= _temp626 + 24; _temp627;}), loc);} goto _LL605; _LL615:
Cyc_Parse_err(( struct _tagged_string)({ char* _temp628=( char*)"bad declaration";
struct _tagged_string _temp629; _temp629.curr= _temp628; _temp629.base= _temp628;
_temp629.last_plus_one= _temp628 + 16; _temp629;}), loc); return 0; _LL605:;}
return({ struct Cyc_List_List* _temp630=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp630->hd=( void*) d; _temp630->tl= 0;
_temp630;});} else{ void* _temp631= _temp602; struct Cyc_Absyn_Structdecl**
_temp643; struct Cyc_List_List* _temp645; struct _tuple1* _temp647; struct Cyc_Absyn_Enumdecl**
_temp649; struct Cyc_List_List* _temp651; struct _tuple1* _temp653; struct Cyc_Absyn_Xenumdecl**
_temp655; struct _tuple1* _temp657; struct Cyc_Absyn_Uniondecl** _temp659;
struct Cyc_List_List* _temp661; struct _tuple1* _temp663; _LL633: if((
unsigned int) _temp631 > 4u?(( struct _enum_struct*) _temp631)->tag == Cyc_Absyn_StructType_tag:
0){ _LL648: _temp647=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*)
_temp631)->f1; goto _LL646; _LL646: _temp645=( struct Cyc_List_List*)(( struct
Cyc_Absyn_StructType_struct*) _temp631)->f2; goto _LL644; _LL644: _temp643=(
struct Cyc_Absyn_Structdecl**)(( struct Cyc_Absyn_StructType_struct*) _temp631)->f3;
goto _LL634;} else{ goto _LL635;} _LL635: if(( unsigned int) _temp631 > 4u?((
struct _enum_struct*) _temp631)->tag == Cyc_Absyn_EnumType_tag: 0){ _LL654:
_temp653=( struct _tuple1*)(( struct Cyc_Absyn_EnumType_struct*) _temp631)->f1;
goto _LL652; _LL652: _temp651=( struct Cyc_List_List*)(( struct Cyc_Absyn_EnumType_struct*)
_temp631)->f2; goto _LL650; _LL650: _temp649=( struct Cyc_Absyn_Enumdecl**)((
struct Cyc_Absyn_EnumType_struct*) _temp631)->f3; goto _LL636;} else{ goto
_LL637;} _LL637: if(( unsigned int) _temp631 > 4u?(( struct _enum_struct*)
_temp631)->tag == Cyc_Absyn_XenumType_tag: 0){ _LL658: _temp657=( struct _tuple1*)((
struct Cyc_Absyn_XenumType_struct*) _temp631)->f1; goto _LL656; _LL656: _temp655=(
struct Cyc_Absyn_Xenumdecl**)(( struct Cyc_Absyn_XenumType_struct*) _temp631)->f2;
goto _LL638;} else{ goto _LL639;} _LL639: if(( unsigned int) _temp631 > 4u?((
struct _enum_struct*) _temp631)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL664:
_temp663=( struct _tuple1*)(( struct Cyc_Absyn_UnionType_struct*) _temp631)->f1;
goto _LL662; _LL662: _temp661=( struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*)
_temp631)->f2; goto _LL660; _LL660: _temp659=( struct Cyc_Absyn_Uniondecl**)((
struct Cyc_Absyn_UnionType_struct*) _temp631)->f3; goto _LL640;} else{ goto
_LL641;} _LL641: goto _LL642; _LL634: { struct Cyc_List_List* ts2=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc,
_temp645); struct Cyc_Absyn_Structdecl* sd=({ struct Cyc_Absyn_Structdecl*
_temp665=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp665->sc=( void*) s; _temp665->name=({ struct Cyc_Core_Opt* _temp666=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp666->v=(
void*)(( struct _tuple1*)({ struct _tuple1* _temp667= _temp647; if( _temp667 ==
0){ _throw( Null_Exception);} _temp667;})); _temp666;}); _temp665->tvs= ts2;
_temp665->fields= 0; _temp665->attributes= 0; _temp665;}); if( atts != 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp668=( char*)"bad attributes on struct";
struct _tagged_string _temp669; _temp669.curr= _temp668; _temp669.base= _temp668;
_temp669.last_plus_one= _temp668 + 25; _temp669;}), loc);} return({ struct Cyc_List_List*
_temp670=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp670->hd=( void*)({ struct Cyc_Absyn_Decl* _temp671=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp671->r=( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp672=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp672->tag= Cyc_Absyn_Struct_d_tag; _temp672->f1= sd;( void*) _temp672;});
_temp671->loc= loc; _temp671;}); _temp670->tl= 0; _temp670;});} _LL636: { struct
Cyc_List_List* ts2=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc, _temp651); struct
Cyc_Absyn_Decl* ed= Cyc_Absyn_enum_decl( s,({ struct Cyc_Core_Opt* _temp673=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp673->v=(
void*)(( struct _tuple1*)({ struct _tuple1* _temp674= _temp653; if( _temp674 ==
0){ _throw( Null_Exception);} _temp674;})); _temp673;}), ts2, 0, loc); if( atts
!= 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp675=( char*)"bad attributes on enum";
struct _tagged_string _temp676; _temp676.curr= _temp675; _temp676.base= _temp675;
_temp676.last_plus_one= _temp675 + 23; _temp676;}), loc);} return({ struct Cyc_List_List*
_temp677=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp677->hd=( void*) ed; _temp677->tl= 0; _temp677;});} _LL638: { struct Cyc_Absyn_Xenumdecl*
ed=({ struct Cyc_Absyn_Xenumdecl* _temp678=( struct Cyc_Absyn_Xenumdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Xenumdecl)); _temp678->sc=( void*) s;
_temp678->name= _temp657; _temp678->fields= 0; _temp678;}); if( atts != 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp679=( char*)"bad attributes on xenum";
struct _tagged_string _temp680; _temp680.curr= _temp679; _temp680.base= _temp679;
_temp680.last_plus_one= _temp679 + 24; _temp680;}), loc);} return({ struct Cyc_List_List*
_temp681=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp681->hd=( void*)({ struct Cyc_Absyn_Decl* _temp682=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp682->r=( void*)({ struct Cyc_Absyn_Xenum_d_struct*
_temp683=( struct Cyc_Absyn_Xenum_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Xenum_d_struct));
_temp683->tag= Cyc_Absyn_Xenum_d_tag; _temp683->f1= ed;( void*) _temp683;});
_temp682->loc= loc; _temp682;}); _temp681->tl= 0; _temp681;});} _LL640: { struct
Cyc_List_List* ts2=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc, _temp661); struct
Cyc_Absyn_Uniondecl* ud=({ struct Cyc_Absyn_Uniondecl* _temp684=( struct Cyc_Absyn_Uniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl)); _temp684->sc=( void*) s;
_temp684->name=({ struct Cyc_Core_Opt* _temp685=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp685->v=( void*)(( struct _tuple1*)({
struct _tuple1* _temp686= _temp663; if( _temp686 == 0){ _throw( Null_Exception);}
_temp686;})); _temp685;}); _temp684->tvs= ts2; _temp684->fields= 0; _temp684->attributes=
0; _temp684;}); if( atts != 0){ Cyc_Parse_err(( struct _tagged_string)({ char*
_temp687=( char*)"bad attributes on union"; struct _tagged_string _temp688;
_temp688.curr= _temp687; _temp688.base= _temp687; _temp688.last_plus_one=
_temp687 + 24; _temp688;}), loc);} return({ struct Cyc_List_List* _temp689=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp689->hd=(
void*)({ struct Cyc_Absyn_Decl* _temp690=( struct Cyc_Absyn_Decl*) GC_malloc(
sizeof( struct Cyc_Absyn_Decl)); _temp690->r=( void*)({ struct Cyc_Absyn_Union_d_struct*
_temp691=( struct Cyc_Absyn_Union_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct));
_temp691->tag= Cyc_Absyn_Union_d_tag; _temp691->f1= ud;( void*) _temp691;});
_temp690->loc= loc; _temp690;}); _temp689->tl= 0; _temp689;});} _LL642: Cyc_Parse_err((
struct _tagged_string)({ char* _temp692=( char*)"missing declarator"; struct
_tagged_string _temp693; _temp693.curr= _temp692; _temp693.base= _temp692;
_temp693.last_plus_one= _temp692 + 19; _temp693;}), loc); return 0; _LL632:;}}
else{ void* t= ts_info.f1; struct Cyc_List_List* fields= Cyc_Parse_apply_tmss(
tq, t, _temp596, atts); if( istypedef){ if( ! exps_empty){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp694=( char*)"initializer in typedef declaration";
struct _tagged_string _temp695; _temp695.curr= _temp694; _temp695.base= _temp694;
_temp695.last_plus_one= _temp694 + 35; _temp695;}), loc);}{ struct Cyc_List_List*
decls=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Decl*(* f)( struct Cyc_Position_Segment*,
struct _tuple7*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_v_typ_to_typedef, loc, fields); if( ts_info.f2 != 0){
struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)( ts_info.f2)->v;{ void*
_temp696=( void*) d->r; struct Cyc_Absyn_Structdecl* _temp708; struct Cyc_Absyn_Enumdecl*
_temp710; struct Cyc_Absyn_Xenumdecl* _temp712; struct Cyc_Absyn_Uniondecl*
_temp714; _LL698: if((( struct _enum_struct*) _temp696)->tag == Cyc_Absyn_Struct_d_tag){
_LL709: _temp708=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*)
_temp696)->f1; goto _LL699;} else{ goto _LL700;} _LL700: if((( struct
_enum_struct*) _temp696)->tag == Cyc_Absyn_Enum_d_tag){ _LL711: _temp710=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*) _temp696)->f1;
goto _LL701;} else{ goto _LL702;} _LL702: if((( struct _enum_struct*) _temp696)->tag
== Cyc_Absyn_Xenum_d_tag){ _LL713: _temp712=( struct Cyc_Absyn_Xenumdecl*)((
struct Cyc_Absyn_Xenum_d_struct*) _temp696)->f1; goto _LL703;} else{ goto _LL704;}
_LL704: if((( struct _enum_struct*) _temp696)->tag == Cyc_Absyn_Union_d_tag){
_LL715: _temp714=( struct Cyc_Absyn_Uniondecl*)(( struct Cyc_Absyn_Union_d_struct*)
_temp696)->f1; goto _LL705;} else{ goto _LL706;} _LL706: goto _LL707; _LL699:(
void*)( _temp708->sc=( void*) s); _temp708->attributes= atts; atts= 0; goto
_LL697; _LL701:( void*)( _temp710->sc=( void*) s); goto _LL697; _LL703:( void*)(
_temp712->sc=( void*) s); goto _LL697; _LL705:( void*)( _temp714->sc=( void*) s);
goto _LL697; _LL707: Cyc_Parse_err(( struct _tagged_string)({ char* _temp716=(
char*)"declaration within typedef is not a struct, union, enum, or xenum";
struct _tagged_string _temp717; _temp717.curr= _temp716; _temp717.base= _temp716;
_temp717.last_plus_one= _temp716 + 66; _temp717;}), loc); goto _LL697; _LL697:;}
decls=({ struct Cyc_List_List* _temp718=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp718->hd=( void*) d; _temp718->tl= decls;
_temp718;});} if( atts != 0){ Cyc_Parse_err(({ struct _tagged_string _temp719=
Cyc_Absyn_attribute2string(( void*) atts->hd); xprintf("bad attribute %.*s in typedef",
_temp719.last_plus_one - _temp719.curr, _temp719.curr);}), loc);} return decls;}}
else{ if( ts_info.f2 != 0){ Cyc_Parse_unimp2(( struct _tagged_string)({ char*
_temp720=( char*)"nested type declaration within declarator"; struct
_tagged_string _temp721; _temp721.curr= _temp720; _temp721.base= _temp720;
_temp721.last_plus_one= _temp720 + 42; _temp721;}), loc);}{ struct Cyc_List_List*
decls= 0;{ struct Cyc_List_List* ds= fields; for( 0; ds != 0; ds= ds->tl,
_temp594= _temp594->tl){ struct _tuple7 _temp724; struct Cyc_List_List* _temp725;
struct Cyc_List_List* _temp727; void* _temp729; struct Cyc_Absyn_Tqual* _temp731;
struct _tuple1* _temp733; struct _tuple7* _temp722=( struct _tuple7*) ds->hd;
_temp724=* _temp722; _LL734: _temp733= _temp724.f1; goto _LL732; _LL732:
_temp731= _temp724.f2; goto _LL730; _LL730: _temp729= _temp724.f3; goto _LL728;
_LL728: _temp727= _temp724.f4; goto _LL726; _LL726: _temp725= _temp724.f5; goto
_LL723; _LL723: if( _temp727 != 0){ Cyc_Parse_warn(( struct _tagged_string)({
char* _temp735=( char*)"bad type params, ignoring"; struct _tagged_string
_temp736; _temp736.curr= _temp735; _temp736.base= _temp735; _temp736.last_plus_one=
_temp735 + 26; _temp736;}), loc);} if( _temp594 == 0){(( void(*)( struct
_tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct
_tagged_string)({ char* _temp737=( char*)"unexpected null in parse!"; struct
_tagged_string _temp738; _temp738.curr= _temp737; _temp738.base= _temp737;
_temp738.last_plus_one= _temp737 + 26; _temp738;}), loc);}{ struct Cyc_Absyn_Exp*
eopt=( struct Cyc_Absyn_Exp*) _temp594->hd; struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_new_vardecl(
_temp733, _temp729, eopt); vd->tq= _temp731;( void*)( vd->sc=( void*) s); vd->attributes=
_temp725;{ struct Cyc_Absyn_Decl* d=({ struct Cyc_Absyn_Decl* _temp739=( struct
Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp739->r=( void*)({
struct Cyc_Absyn_Var_d_struct* _temp740=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp740->tag= Cyc_Absyn_Var_d_tag;
_temp740->f1= vd;( void*) _temp740;}); _temp739->loc= loc; _temp739;}); decls=({
struct Cyc_List_List* _temp741=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp741->hd=( void*) d; _temp741->tl= decls; _temp741;});}}}}
return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
decls);}}}}}}} static void* Cyc_Parse_id_to_kind( struct _tagged_string s,
struct Cyc_Position_Segment* loc){ if( Cyc_String_strlen( s) !=( unsigned int) 1){
Cyc_Parse_err(({ struct _tagged_string _temp742= s; xprintf("bad kind: %.*s",
_temp742.last_plus_one - _temp742.curr, _temp742.curr);}), loc); return Cyc_Absyn_BoxKind;}
else{ switch(({ struct _tagged_string _temp743= s; char* _temp745= _temp743.curr
+ 0; if( _temp745 < _temp743.base? 1: _temp745 >= _temp743.last_plus_one){
_throw( Null_Exception);}* _temp745;})){ case 'A': _LL746: return Cyc_Absyn_AnyKind;
case 'M': _LL747: return Cyc_Absyn_MemKind; case 'B': _LL748: return Cyc_Absyn_BoxKind;
case 'R': _LL749: return Cyc_Absyn_RgnKind; case 'E': _LL750: return Cyc_Absyn_EffKind;
default: _LL751: Cyc_Parse_err(({ struct _tagged_string _temp753= s; xprintf("bad kind: %.*s",
_temp753.last_plus_one - _temp753.curr, _temp753.curr);}), loc); return Cyc_Absyn_BoxKind;}}}
static struct Cyc_Absyn_Decl* Cyc_Parse_v_typ_to_typedef( struct Cyc_Position_Segment*
loc, struct _tuple7* t){ struct _tuple1* x=(* t).f1; Cyc_Lex_register_typedef( x);
if((* t).f5 != 0){ Cyc_Parse_err(({ struct _tagged_string _temp754= Cyc_Absyn_attribute2string((
void*)((* t).f5)->hd); xprintf("bad attribute %.*s within typedef", _temp754.last_plus_one
- _temp754.curr, _temp754.curr);}), loc);} return Cyc_Absyn_new_decl(({ struct
Cyc_Absyn_Typedef_d_struct* _temp755=( struct Cyc_Absyn_Typedef_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct)); _temp755->tag= Cyc_Absyn_Typedef_d_tag;
_temp755->f1=({ struct Cyc_Absyn_Typedefdecl* _temp756=( struct Cyc_Absyn_Typedefdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedefdecl)); _temp756->name= x; _temp756->tvs=(*
t).f4; _temp756->defn=( void*)(* t).f3; _temp756;});( void*) _temp755;}), loc);}
typedef struct _xenum_struct* Cyc_YYSTYPE; char Cyc_Okay_tok_tag[ 9u]="Okay_tok";
struct Cyc_Okay_tok_struct{ char* tag; } ; char Cyc_Int_tok_tag[ 8u]="Int_tok";
struct Cyc_Int_tok_struct{ char* tag; struct _tuple11* f1; } ; char Cyc_Char_tok_tag[
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
struct Cyc_InitDecl_tok_struct{ char* tag; struct _tuple12* f1; } ; char Cyc_InitDeclList_tok_tag[
17u]="InitDeclList_tok"; struct Cyc_InitDeclList_tok_struct{ char* tag; struct
Cyc_List_List* f1; } ; char Cyc_StorageClass_tok_tag[ 17u]="StorageClass_tok";
struct Cyc_StorageClass_tok_struct{ char* tag; void* f1; } ; char Cyc_TypeSpecifier_tok_tag[
18u]="TypeSpecifier_tok"; struct Cyc_TypeSpecifier_tok_struct{ char* tag; void*
f1; } ; char Cyc_QualSpecList_tok_tag[ 17u]="QualSpecList_tok"; struct Cyc_QualSpecList_tok_struct{
char* tag; struct _tuple13* f1; } ; char Cyc_TypeQual_tok_tag[ 13u]="TypeQual_tok";
struct Cyc_TypeQual_tok_struct{ char* tag; struct Cyc_Absyn_Tqual* f1; } ; char
Cyc_StructFieldDeclList_tok_tag[ 24u]="StructFieldDeclList_tok"; struct Cyc_StructFieldDeclList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; char Cyc_StructFieldDeclListList_tok_tag[
28u]="StructFieldDeclListList_tok"; struct Cyc_StructFieldDeclListList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; char Cyc_Declarator_tok_tag[ 15u]="Declarator_tok";
struct Cyc_Declarator_tok_struct{ char* tag; struct Cyc_Parse_Declarator* f1; }
; char Cyc_DeclaratorList_tok_tag[ 19u]="DeclaratorList_tok"; struct Cyc_DeclaratorList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; char Cyc_AbstractDeclarator_tok_tag[ 23u]="AbstractDeclarator_tok";
struct Cyc_AbstractDeclarator_tok_struct{ char* tag; struct Cyc_Parse_Abstractdeclarator*
f1; } ; char Cyc_EnumeratorField_tok_tag[ 20u]="EnumeratorField_tok"; struct Cyc_EnumeratorField_tok_struct{
char* tag; struct Cyc_Absyn_Enumfield* f1; } ; char Cyc_EnumeratorFieldList_tok_tag[
24u]="EnumeratorFieldList_tok"; struct Cyc_EnumeratorFieldList_tok_struct{ char*
tag; struct Cyc_List_List* f1; } ; char Cyc_ParamDecl_tok_tag[ 14u]="ParamDecl_tok";
struct Cyc_ParamDecl_tok_struct{ char* tag; struct _tuple2* f1; } ; char Cyc_ParamDeclList_tok_tag[
18u]="ParamDeclList_tok"; struct Cyc_ParamDeclList_tok_struct{ char* tag; struct
Cyc_List_List* f1; } ; char Cyc_ParamDeclListBool_tok_tag[ 22u]="ParamDeclListBool_tok";
struct Cyc_ParamDeclListBool_tok_struct{ char* tag; struct _tuple14* f1; } ;
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
struct Cyc_FieldPattern_tok_struct{ char* tag; struct _tuple15* f1; } ; char Cyc_FieldPatternList_tok_tag[
21u]="FieldPatternList_tok"; struct Cyc_FieldPatternList_tok_struct{ char* tag;
struct Cyc_List_List* f1; } ; char Cyc_BlockItem_tok_tag[ 14u]="BlockItem_tok";
struct Cyc_BlockItem_tok_struct{ char* tag; void* f1; } ; char Cyc_Kind_tok_tag[
9u]="Kind_tok"; struct Cyc_Kind_tok_struct{ char* tag; void* f1; } ; char Cyc_Attribute_tok_tag[
14u]="Attribute_tok"; struct Cyc_Attribute_tok_struct{ char* tag; void* f1; } ;
char Cyc_AttributeList_tok_tag[ 18u]="AttributeList_tok"; struct Cyc_AttributeList_tok_struct{
char* tag; struct Cyc_List_List* f1; } ; struct _tuple11* Cyc_yyget_Int_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp757= yy1; struct _tuple11*
_temp763; _LL759: if((*(( struct _xenum_struct*) _temp757)).tag == Cyc_Int_tok_tag){
_LL764: _temp763=(( struct Cyc_Int_tok_struct*) _temp757)->f1; goto _LL760;}
else{ goto _LL761;} _LL761: goto _LL762; _LL760: return _temp763; _LL762:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp765=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp765->tag= Cyc_Core_Failure_tag;
_temp765->f1=( struct _tagged_string)({ char* _temp766=( char*)"Int_tok"; struct
_tagged_string _temp767; _temp767.curr= _temp766; _temp767.base= _temp766;
_temp767.last_plus_one= _temp766 + 8; _temp767;});( struct _xenum_struct*)
_temp765;})); _LL758:;} struct _tagged_string Cyc_yyget_String_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp768= yy1; struct _tagged_string
_temp774; _LL770: if((*(( struct _xenum_struct*) _temp768)).tag == Cyc_String_tok_tag){
_LL775: _temp774=(( struct Cyc_String_tok_struct*) _temp768)->f1; goto _LL771;}
else{ goto _LL772;} _LL772: goto _LL773; _LL771: return _temp774; _LL773:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp776=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp776->tag= Cyc_Core_Failure_tag;
_temp776->f1=( struct _tagged_string)({ char* _temp777=( char*)"String_tok";
struct _tagged_string _temp778; _temp778.curr= _temp777; _temp778.base= _temp777;
_temp778.last_plus_one= _temp777 + 11; _temp778;});( struct _xenum_struct*)
_temp776;})); _LL769:;} char Cyc_yyget_Char_tok( struct _xenum_struct* yy1){
struct _xenum_struct* _temp779= yy1; char _temp785; _LL781: if((*(( struct
_xenum_struct*) _temp779)).tag == Cyc_Char_tok_tag){ _LL786: _temp785=(( struct
Cyc_Char_tok_struct*) _temp779)->f1; goto _LL782;} else{ goto _LL783;} _LL783:
goto _LL784; _LL782: return _temp785; _LL784:( void) _throw(({ struct Cyc_Core_Failure_struct*
_temp787=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp787->tag= Cyc_Core_Failure_tag; _temp787->f1=( struct _tagged_string)({
char* _temp788=( char*)"Char_tok"; struct _tagged_string _temp789; _temp789.curr=
_temp788; _temp789.base= _temp788; _temp789.last_plus_one= _temp788 + 9;
_temp789;});( struct _xenum_struct*) _temp787;})); _LL780:;} void* Cyc_yyget_Pointer_Sort_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp790= yy1; void* _temp796;
_LL792: if((*(( struct _xenum_struct*) _temp790)).tag == Cyc_Pointer_Sort_tok_tag){
_LL797: _temp796=(( struct Cyc_Pointer_Sort_tok_struct*) _temp790)->f1; goto
_LL793;} else{ goto _LL794;} _LL794: goto _LL795; _LL793: return _temp796;
_LL795:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp798=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp798->tag= Cyc_Core_Failure_tag;
_temp798->f1=( struct _tagged_string)({ char* _temp799=( char*)"Pointer_Sort_tok";
struct _tagged_string _temp800; _temp800.curr= _temp799; _temp800.base= _temp799;
_temp800.last_plus_one= _temp799 + 17; _temp800;});( struct _xenum_struct*)
_temp798;})); _LL791:;} struct Cyc_Absyn_Exp* Cyc_yyget_Exp_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp801= yy1; struct Cyc_Absyn_Exp*
_temp807; _LL803: if((*(( struct _xenum_struct*) _temp801)).tag == Cyc_Exp_tok_tag){
_LL808: _temp807=(( struct Cyc_Exp_tok_struct*) _temp801)->f1; goto _LL804;}
else{ goto _LL805;} _LL805: goto _LL806; _LL804: return _temp807; _LL806:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp809=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp809->tag= Cyc_Core_Failure_tag;
_temp809->f1=( struct _tagged_string)({ char* _temp810=( char*)"Exp_tok"; struct
_tagged_string _temp811; _temp811.curr= _temp810; _temp811.base= _temp810;
_temp811.last_plus_one= _temp810 + 8; _temp811;});( struct _xenum_struct*)
_temp809;})); _LL802:;} struct Cyc_List_List* Cyc_yyget_ExpList_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp812= yy1; struct Cyc_List_List*
_temp818; _LL814: if((*(( struct _xenum_struct*) _temp812)).tag == Cyc_ExpList_tok_tag){
_LL819: _temp818=(( struct Cyc_ExpList_tok_struct*) _temp812)->f1; goto _LL815;}
else{ goto _LL816;} _LL816: goto _LL817; _LL815: return _temp818; _LL817:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp820=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp820->tag= Cyc_Core_Failure_tag;
_temp820->f1=( struct _tagged_string)({ char* _temp821=( char*)"ExpList_tok";
struct _tagged_string _temp822; _temp822.curr= _temp821; _temp822.base= _temp821;
_temp822.last_plus_one= _temp821 + 12; _temp822;});( struct _xenum_struct*)
_temp820;})); _LL813:;} struct Cyc_List_List* Cyc_yyget_InitializerList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp823= yy1; struct Cyc_List_List*
_temp829; _LL825: if((*(( struct _xenum_struct*) _temp823)).tag == Cyc_InitializerList_tok_tag){
_LL830: _temp829=(( struct Cyc_InitializerList_tok_struct*) _temp823)->f1; goto
_LL826;} else{ goto _LL827;} _LL827: goto _LL828; _LL826: return _temp829;
_LL828:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp831=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp831->tag= Cyc_Core_Failure_tag;
_temp831->f1=( struct _tagged_string)({ char* _temp832=( char*)"InitializerList_tok";
struct _tagged_string _temp833; _temp833.curr= _temp832; _temp833.base= _temp832;
_temp833.last_plus_one= _temp832 + 20; _temp833;});( struct _xenum_struct*)
_temp831;})); _LL824:;} void* Cyc_yyget_Primop_tok( struct _xenum_struct* yy1){
struct _xenum_struct* _temp834= yy1; void* _temp840; _LL836: if((*(( struct
_xenum_struct*) _temp834)).tag == Cyc_Primop_tok_tag){ _LL841: _temp840=((
struct Cyc_Primop_tok_struct*) _temp834)->f1; goto _LL837;} else{ goto _LL838;}
_LL838: goto _LL839; _LL837: return _temp840; _LL839:( void) _throw(({ struct
Cyc_Core_Failure_struct* _temp842=( struct Cyc_Core_Failure_struct*) GC_malloc(
sizeof( struct Cyc_Core_Failure_struct)); _temp842->tag= Cyc_Core_Failure_tag;
_temp842->f1=( struct _tagged_string)({ char* _temp843=( char*)"Primop_tok";
struct _tagged_string _temp844; _temp844.curr= _temp843; _temp844.base= _temp843;
_temp844.last_plus_one= _temp843 + 11; _temp844;});( struct _xenum_struct*)
_temp842;})); _LL835:;} struct Cyc_Core_Opt* Cyc_yyget_Primopopt_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp845= yy1; struct Cyc_Core_Opt*
_temp851; _LL847: if((*(( struct _xenum_struct*) _temp845)).tag == Cyc_Primopopt_tok_tag){
_LL852: _temp851=(( struct Cyc_Primopopt_tok_struct*) _temp845)->f1; goto _LL848;}
else{ goto _LL849;} _LL849: goto _LL850; _LL848: return _temp851; _LL850:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp853=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp853->tag= Cyc_Core_Failure_tag;
_temp853->f1=( struct _tagged_string)({ char* _temp854=( char*)"Primopopt_tok";
struct _tagged_string _temp855; _temp855.curr= _temp854; _temp855.base= _temp854;
_temp855.last_plus_one= _temp854 + 14; _temp855;});( struct _xenum_struct*)
_temp853;})); _LL846:;} struct _tuple1* Cyc_yyget_QualId_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp856= yy1; struct _tuple1*
_temp862; _LL858: if((*(( struct _xenum_struct*) _temp856)).tag == Cyc_QualId_tok_tag){
_LL863: _temp862=(( struct Cyc_QualId_tok_struct*) _temp856)->f1; goto _LL859;}
else{ goto _LL860;} _LL860: goto _LL861; _LL859: return _temp862; _LL861:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp864=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp864->tag= Cyc_Core_Failure_tag;
_temp864->f1=( struct _tagged_string)({ char* _temp865=( char*)"QualId_tok";
struct _tagged_string _temp866; _temp866.curr= _temp865; _temp866.base= _temp865;
_temp866.last_plus_one= _temp865 + 11; _temp866;});( struct _xenum_struct*)
_temp864;})); _LL857:;} struct Cyc_Absyn_Stmt* Cyc_yyget_Stmt_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp867= yy1; struct Cyc_Absyn_Stmt*
_temp873; _LL869: if((*(( struct _xenum_struct*) _temp867)).tag == Cyc_Stmt_tok_tag){
_LL874: _temp873=(( struct Cyc_Stmt_tok_struct*) _temp867)->f1; goto _LL870;}
else{ goto _LL871;} _LL871: goto _LL872; _LL870: return _temp873; _LL872:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp875=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp875->tag= Cyc_Core_Failure_tag;
_temp875->f1=( struct _tagged_string)({ char* _temp876=( char*)"Stmt_tok";
struct _tagged_string _temp877; _temp877.curr= _temp876; _temp877.base= _temp876;
_temp877.last_plus_one= _temp876 + 9; _temp877;});( struct _xenum_struct*)
_temp875;})); _LL868:;} void* Cyc_yyget_BlockItem_tok( struct _xenum_struct* yy1){
struct _xenum_struct* _temp878= yy1; void* _temp884; _LL880: if((*(( struct
_xenum_struct*) _temp878)).tag == Cyc_BlockItem_tok_tag){ _LL885: _temp884=((
struct Cyc_BlockItem_tok_struct*) _temp878)->f1; goto _LL881;} else{ goto _LL882;}
_LL882: goto _LL883; _LL881: return _temp884; _LL883:( void) _throw(({ struct
Cyc_Core_Failure_struct* _temp886=( struct Cyc_Core_Failure_struct*) GC_malloc(
sizeof( struct Cyc_Core_Failure_struct)); _temp886->tag= Cyc_Core_Failure_tag;
_temp886->f1=( struct _tagged_string)({ char* _temp887=( char*)"BlockItem_tok";
struct _tagged_string _temp888; _temp888.curr= _temp887; _temp888.base= _temp887;
_temp888.last_plus_one= _temp887 + 14; _temp888;});( struct _xenum_struct*)
_temp886;})); _LL879:;} struct Cyc_List_List* Cyc_yyget_SwitchClauseList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp889= yy1; struct Cyc_List_List*
_temp895; _LL891: if((*(( struct _xenum_struct*) _temp889)).tag == Cyc_SwitchClauseList_tok_tag){
_LL896: _temp895=(( struct Cyc_SwitchClauseList_tok_struct*) _temp889)->f1; goto
_LL892;} else{ goto _LL893;} _LL893: goto _LL894; _LL892: return _temp895;
_LL894:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp897=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp897->tag= Cyc_Core_Failure_tag;
_temp897->f1=( struct _tagged_string)({ char* _temp898=( char*)"SwitchClauseList_tok";
struct _tagged_string _temp899; _temp899.curr= _temp898; _temp899.base= _temp898;
_temp899.last_plus_one= _temp898 + 21; _temp899;});( struct _xenum_struct*)
_temp897;})); _LL890:;} struct Cyc_Absyn_Pat* Cyc_yyget_Pattern_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp900= yy1; struct Cyc_Absyn_Pat*
_temp906; _LL902: if((*(( struct _xenum_struct*) _temp900)).tag == Cyc_Pattern_tok_tag){
_LL907: _temp906=(( struct Cyc_Pattern_tok_struct*) _temp900)->f1; goto _LL903;}
else{ goto _LL904;} _LL904: goto _LL905; _LL903: return _temp906; _LL905:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp908=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp908->tag= Cyc_Core_Failure_tag;
_temp908->f1=( struct _tagged_string)({ char* _temp909=( char*)"Pattern_tok";
struct _tagged_string _temp910; _temp910.curr= _temp909; _temp910.base= _temp909;
_temp910.last_plus_one= _temp909 + 12; _temp910;});( struct _xenum_struct*)
_temp908;})); _LL901:;} struct Cyc_List_List* Cyc_yyget_PatternList_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp911= yy1; struct Cyc_List_List*
_temp917; _LL913: if((*(( struct _xenum_struct*) _temp911)).tag == Cyc_PatternList_tok_tag){
_LL918: _temp917=(( struct Cyc_PatternList_tok_struct*) _temp911)->f1; goto
_LL914;} else{ goto _LL915;} _LL915: goto _LL916; _LL914: return _temp917;
_LL916:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp919=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp919->tag= Cyc_Core_Failure_tag;
_temp919->f1=( struct _tagged_string)({ char* _temp920=( char*)"PatternList_tok";
struct _tagged_string _temp921; _temp921.curr= _temp920; _temp921.base= _temp920;
_temp921.last_plus_one= _temp920 + 16; _temp921;});( struct _xenum_struct*)
_temp919;})); _LL912:;} struct _tuple15* Cyc_yyget_FieldPattern_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp922= yy1; struct _tuple15*
_temp928; _LL924: if((*(( struct _xenum_struct*) _temp922)).tag == Cyc_FieldPattern_tok_tag){
_LL929: _temp928=(( struct Cyc_FieldPattern_tok_struct*) _temp922)->f1; goto
_LL925;} else{ goto _LL926;} _LL926: goto _LL927; _LL925: return _temp928;
_LL927:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp930=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp930->tag= Cyc_Core_Failure_tag;
_temp930->f1=( struct _tagged_string)({ char* _temp931=( char*)"FieldPattern_tok";
struct _tagged_string _temp932; _temp932.curr= _temp931; _temp932.base= _temp931;
_temp932.last_plus_one= _temp931 + 17; _temp932;});( struct _xenum_struct*)
_temp930;})); _LL923:;} struct Cyc_List_List* Cyc_yyget_FieldPatternList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp933= yy1; struct Cyc_List_List*
_temp939; _LL935: if((*(( struct _xenum_struct*) _temp933)).tag == Cyc_FieldPatternList_tok_tag){
_LL940: _temp939=(( struct Cyc_FieldPatternList_tok_struct*) _temp933)->f1; goto
_LL936;} else{ goto _LL937;} _LL937: goto _LL938; _LL936: return _temp939;
_LL938:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp941=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp941->tag= Cyc_Core_Failure_tag;
_temp941->f1=( struct _tagged_string)({ char* _temp942=( char*)"FieldPatternList_tok";
struct _tagged_string _temp943; _temp943.curr= _temp942; _temp943.base= _temp942;
_temp943.last_plus_one= _temp942 + 21; _temp943;});( struct _xenum_struct*)
_temp941;})); _LL934:;} struct Cyc_Absyn_Fndecl* Cyc_yyget_FnDecl_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp944= yy1; struct Cyc_Absyn_Fndecl*
_temp950; _LL946: if((*(( struct _xenum_struct*) _temp944)).tag == Cyc_FnDecl_tok_tag){
_LL951: _temp950=(( struct Cyc_FnDecl_tok_struct*) _temp944)->f1; goto _LL947;}
else{ goto _LL948;} _LL948: goto _LL949; _LL947: return _temp950; _LL949:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp952=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp952->tag= Cyc_Core_Failure_tag;
_temp952->f1=( struct _tagged_string)({ char* _temp953=( char*)"FnDecl_tok";
struct _tagged_string _temp954; _temp954.curr= _temp953; _temp954.base= _temp953;
_temp954.last_plus_one= _temp953 + 11; _temp954;});( struct _xenum_struct*)
_temp952;})); _LL945:;} struct Cyc_List_List* Cyc_yyget_DeclList_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp955= yy1; struct Cyc_List_List*
_temp961; _LL957: if((*(( struct _xenum_struct*) _temp955)).tag == Cyc_DeclList_tok_tag){
_LL962: _temp961=(( struct Cyc_DeclList_tok_struct*) _temp955)->f1; goto _LL958;}
else{ goto _LL959;} _LL959: goto _LL960; _LL958: return _temp961; _LL960:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp963=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp963->tag= Cyc_Core_Failure_tag;
_temp963->f1=( struct _tagged_string)({ char* _temp964=( char*)"DeclList_tok";
struct _tagged_string _temp965; _temp965.curr= _temp964; _temp965.base= _temp964;
_temp965.last_plus_one= _temp964 + 13; _temp965;});( struct _xenum_struct*)
_temp963;})); _LL956:;} struct Cyc_Parse_Declaration_spec* Cyc_yyget_DeclSpec_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp966= yy1; struct Cyc_Parse_Declaration_spec*
_temp972; _LL968: if((*(( struct _xenum_struct*) _temp966)).tag == Cyc_DeclSpec_tok_tag){
_LL973: _temp972=(( struct Cyc_DeclSpec_tok_struct*) _temp966)->f1; goto _LL969;}
else{ goto _LL970;} _LL970: goto _LL971; _LL969: return _temp972; _LL971:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp974=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp974->tag= Cyc_Core_Failure_tag;
_temp974->f1=( struct _tagged_string)({ char* _temp975=( char*)"DeclSpec_tok";
struct _tagged_string _temp976; _temp976.curr= _temp975; _temp976.base= _temp975;
_temp976.last_plus_one= _temp975 + 13; _temp976;});( struct _xenum_struct*)
_temp974;})); _LL967:;} struct _tuple12* Cyc_yyget_InitDecl_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp977= yy1; struct _tuple12*
_temp983; _LL979: if((*(( struct _xenum_struct*) _temp977)).tag == Cyc_InitDecl_tok_tag){
_LL984: _temp983=(( struct Cyc_InitDecl_tok_struct*) _temp977)->f1; goto _LL980;}
else{ goto _LL981;} _LL981: goto _LL982; _LL980: return _temp983; _LL982:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp985=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp985->tag= Cyc_Core_Failure_tag;
_temp985->f1=( struct _tagged_string)({ char* _temp986=( char*)"InitDecl_tok";
struct _tagged_string _temp987; _temp987.curr= _temp986; _temp987.base= _temp986;
_temp987.last_plus_one= _temp986 + 13; _temp987;});( struct _xenum_struct*)
_temp985;})); _LL978:;} struct Cyc_List_List* Cyc_yyget_InitDeclList_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp988= yy1; struct Cyc_List_List*
_temp994; _LL990: if((*(( struct _xenum_struct*) _temp988)).tag == Cyc_InitDeclList_tok_tag){
_LL995: _temp994=(( struct Cyc_InitDeclList_tok_struct*) _temp988)->f1; goto
_LL991;} else{ goto _LL992;} _LL992: goto _LL993; _LL991: return _temp994;
_LL993:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp996=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp996->tag= Cyc_Core_Failure_tag;
_temp996->f1=( struct _tagged_string)({ char* _temp997=( char*)"InitDeclList_tok";
struct _tagged_string _temp998; _temp998.curr= _temp997; _temp998.base= _temp997;
_temp998.last_plus_one= _temp997 + 17; _temp998;});( struct _xenum_struct*)
_temp996;})); _LL989:;} void* Cyc_yyget_StorageClass_tok( struct _xenum_struct*
yy1){ struct _xenum_struct* _temp999= yy1; void* _temp1005; _LL1001: if((*((
struct _xenum_struct*) _temp999)).tag == Cyc_StorageClass_tok_tag){ _LL1006:
_temp1005=(( struct Cyc_StorageClass_tok_struct*) _temp999)->f1; goto _LL1002;}
else{ goto _LL1003;} _LL1003: goto _LL1004; _LL1002: return _temp1005; _LL1004:(
void) _throw(({ struct Cyc_Core_Failure_struct* _temp1007=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp1007->tag= Cyc_Core_Failure_tag;
_temp1007->f1=( struct _tagged_string)({ char* _temp1008=( char*)"StorageClass_tok";
struct _tagged_string _temp1009; _temp1009.curr= _temp1008; _temp1009.base=
_temp1008; _temp1009.last_plus_one= _temp1008 + 17; _temp1009;});( struct
_xenum_struct*) _temp1007;})); _LL1000:;} void* Cyc_yyget_TypeSpecifier_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1010= yy1; void*
_temp1016; _LL1012: if((*(( struct _xenum_struct*) _temp1010)).tag == Cyc_TypeSpecifier_tok_tag){
_LL1017: _temp1016=(( struct Cyc_TypeSpecifier_tok_struct*) _temp1010)->f1; goto
_LL1013;} else{ goto _LL1014;} _LL1014: goto _LL1015; _LL1013: return _temp1016;
_LL1015:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1018=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp1018->tag= Cyc_Core_Failure_tag;
_temp1018->f1=( struct _tagged_string)({ char* _temp1019=( char*)"TypeSpecifier_tok";
struct _tagged_string _temp1020; _temp1020.curr= _temp1019; _temp1020.base=
_temp1019; _temp1020.last_plus_one= _temp1019 + 18; _temp1020;});( struct
_xenum_struct*) _temp1018;})); _LL1011:;} void* Cyc_yyget_StructOrUnion_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1021= yy1; void*
_temp1027; _LL1023: if((*(( struct _xenum_struct*) _temp1021)).tag == Cyc_StructOrUnion_tok_tag){
_LL1028: _temp1027=(( struct Cyc_StructOrUnion_tok_struct*) _temp1021)->f1; goto
_LL1024;} else{ goto _LL1025;} _LL1025: goto _LL1026; _LL1024: return _temp1027;
_LL1026:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1029=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp1029->tag= Cyc_Core_Failure_tag;
_temp1029->f1=( struct _tagged_string)({ char* _temp1030=( char*)"StructOrUnion_tok";
struct _tagged_string _temp1031; _temp1031.curr= _temp1030; _temp1031.base=
_temp1030; _temp1031.last_plus_one= _temp1030 + 18; _temp1031;});( struct
_xenum_struct*) _temp1029;})); _LL1022:;} struct Cyc_Absyn_Tqual* Cyc_yyget_TypeQual_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1032= yy1; struct Cyc_Absyn_Tqual*
_temp1038; _LL1034: if((*(( struct _xenum_struct*) _temp1032)).tag == Cyc_TypeQual_tok_tag){
_LL1039: _temp1038=(( struct Cyc_TypeQual_tok_struct*) _temp1032)->f1; goto
_LL1035;} else{ goto _LL1036;} _LL1036: goto _LL1037; _LL1035: return _temp1038;
_LL1037:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1040=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp1040->tag= Cyc_Core_Failure_tag;
_temp1040->f1=( struct _tagged_string)({ char* _temp1041=( char*)"TypeQual_tok";
struct _tagged_string _temp1042; _temp1042.curr= _temp1041; _temp1042.base=
_temp1041; _temp1042.last_plus_one= _temp1041 + 13; _temp1042;});( struct
_xenum_struct*) _temp1040;})); _LL1033:;} struct Cyc_List_List* Cyc_yyget_StructFieldDeclList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1043= yy1; struct Cyc_List_List*
_temp1049; _LL1045: if((*(( struct _xenum_struct*) _temp1043)).tag == Cyc_StructFieldDeclList_tok_tag){
_LL1050: _temp1049=(( struct Cyc_StructFieldDeclList_tok_struct*) _temp1043)->f1;
goto _LL1046;} else{ goto _LL1047;} _LL1047: goto _LL1048; _LL1046: return
_temp1049; _LL1048:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1051=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1051->tag= Cyc_Core_Failure_tag; _temp1051->f1=( struct _tagged_string)({
char* _temp1052=( char*)"StructFieldDeclList_tok"; struct _tagged_string
_temp1053; _temp1053.curr= _temp1052; _temp1053.base= _temp1052; _temp1053.last_plus_one=
_temp1052 + 24; _temp1053;});( struct _xenum_struct*) _temp1051;})); _LL1044:;}
struct Cyc_List_List* Cyc_yyget_StructFieldDeclListList_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp1054= yy1; struct Cyc_List_List*
_temp1060; _LL1056: if((*(( struct _xenum_struct*) _temp1054)).tag == Cyc_StructFieldDeclListList_tok_tag){
_LL1061: _temp1060=(( struct Cyc_StructFieldDeclListList_tok_struct*) _temp1054)->f1;
goto _LL1057;} else{ goto _LL1058;} _LL1058: goto _LL1059; _LL1057: return
_temp1060; _LL1059:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1062=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1062->tag= Cyc_Core_Failure_tag; _temp1062->f1=( struct _tagged_string)({
char* _temp1063=( char*)"StructFieldDeclListList_tok"; struct _tagged_string
_temp1064; _temp1064.curr= _temp1063; _temp1064.base= _temp1063; _temp1064.last_plus_one=
_temp1063 + 28; _temp1064;});( struct _xenum_struct*) _temp1062;})); _LL1055:;}
struct Cyc_List_List* Cyc_yyget_TypeModifierList_tok( struct _xenum_struct* yy1){
struct _xenum_struct* _temp1065= yy1; struct Cyc_List_List* _temp1071; _LL1067:
if((*(( struct _xenum_struct*) _temp1065)).tag == Cyc_TypeModifierList_tok_tag){
_LL1072: _temp1071=(( struct Cyc_TypeModifierList_tok_struct*) _temp1065)->f1;
goto _LL1068;} else{ goto _LL1069;} _LL1069: goto _LL1070; _LL1068: return
_temp1071; _LL1070:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1073=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1073->tag= Cyc_Core_Failure_tag; _temp1073->f1=( struct _tagged_string)({
char* _temp1074=( char*)"TypeModifierList_tok"; struct _tagged_string _temp1075;
_temp1075.curr= _temp1074; _temp1075.base= _temp1074; _temp1075.last_plus_one=
_temp1074 + 21; _temp1075;});( struct _xenum_struct*) _temp1073;})); _LL1066:;}
void* Cyc_yyget_Rgn_tok( struct _xenum_struct* yy1){ struct _xenum_struct*
_temp1076= yy1; void* _temp1082; _LL1078: if((*(( struct _xenum_struct*)
_temp1076)).tag == Cyc_Rgn_tok_tag){ _LL1083: _temp1082=(( struct Cyc_Rgn_tok_struct*)
_temp1076)->f1; goto _LL1079;} else{ goto _LL1080;} _LL1080: goto _LL1081;
_LL1079: return _temp1082; _LL1081:( void) _throw(({ struct Cyc_Core_Failure_struct*
_temp1084=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1084->tag= Cyc_Core_Failure_tag; _temp1084->f1=( struct _tagged_string)({
char* _temp1085=( char*)"Rgn_tok"; struct _tagged_string _temp1086; _temp1086.curr=
_temp1085; _temp1086.base= _temp1085; _temp1086.last_plus_one= _temp1085 + 8;
_temp1086;});( struct _xenum_struct*) _temp1084;})); _LL1077:;} struct Cyc_Parse_Declarator*
Cyc_yyget_Declarator_tok( struct _xenum_struct* yy1){ struct _xenum_struct*
_temp1087= yy1; struct Cyc_Parse_Declarator* _temp1093; _LL1089: if((*(( struct
_xenum_struct*) _temp1087)).tag == Cyc_Declarator_tok_tag){ _LL1094: _temp1093=((
struct Cyc_Declarator_tok_struct*) _temp1087)->f1; goto _LL1090;} else{ goto
_LL1091;} _LL1091: goto _LL1092; _LL1090: return _temp1093; _LL1092:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp1095=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp1095->tag= Cyc_Core_Failure_tag;
_temp1095->f1=( struct _tagged_string)({ char* _temp1096=( char*)"Declarator_tok";
struct _tagged_string _temp1097; _temp1097.curr= _temp1096; _temp1097.base=
_temp1096; _temp1097.last_plus_one= _temp1096 + 15; _temp1097;});( struct
_xenum_struct*) _temp1095;})); _LL1088:;} struct Cyc_List_List* Cyc_yyget_DeclaratorList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1098= yy1; struct Cyc_List_List*
_temp1104; _LL1100: if((*(( struct _xenum_struct*) _temp1098)).tag == Cyc_DeclaratorList_tok_tag){
_LL1105: _temp1104=(( struct Cyc_DeclaratorList_tok_struct*) _temp1098)->f1;
goto _LL1101;} else{ goto _LL1102;} _LL1102: goto _LL1103; _LL1101: return
_temp1104; _LL1103:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1106=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1106->tag= Cyc_Core_Failure_tag; _temp1106->f1=( struct _tagged_string)({
char* _temp1107=( char*)"DeclaratorList_tok"; struct _tagged_string _temp1108;
_temp1108.curr= _temp1107; _temp1108.base= _temp1107; _temp1108.last_plus_one=
_temp1107 + 19; _temp1108;});( struct _xenum_struct*) _temp1106;})); _LL1099:;}
struct Cyc_Parse_Abstractdeclarator* Cyc_yyget_AbstractDeclarator_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp1109= yy1; struct Cyc_Parse_Abstractdeclarator*
_temp1115; _LL1111: if((*(( struct _xenum_struct*) _temp1109)).tag == Cyc_AbstractDeclarator_tok_tag){
_LL1116: _temp1115=(( struct Cyc_AbstractDeclarator_tok_struct*) _temp1109)->f1;
goto _LL1112;} else{ goto _LL1113;} _LL1113: goto _LL1114; _LL1112: return
_temp1115; _LL1114:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1117=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1117->tag= Cyc_Core_Failure_tag; _temp1117->f1=( struct _tagged_string)({
char* _temp1118=( char*)"AbstractDeclarator_tok"; struct _tagged_string
_temp1119; _temp1119.curr= _temp1118; _temp1119.base= _temp1118; _temp1119.last_plus_one=
_temp1118 + 23; _temp1119;});( struct _xenum_struct*) _temp1117;})); _LL1110:;}
struct Cyc_Absyn_Enumfield* Cyc_yyget_EnumeratorField_tok( struct _xenum_struct*
yy1){ struct _xenum_struct* _temp1120= yy1; struct Cyc_Absyn_Enumfield*
_temp1126; _LL1122: if((*(( struct _xenum_struct*) _temp1120)).tag == Cyc_EnumeratorField_tok_tag){
_LL1127: _temp1126=(( struct Cyc_EnumeratorField_tok_struct*) _temp1120)->f1;
goto _LL1123;} else{ goto _LL1124;} _LL1124: goto _LL1125; _LL1123: return
_temp1126; _LL1125:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1128=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1128->tag= Cyc_Core_Failure_tag; _temp1128->f1=( struct _tagged_string)({
char* _temp1129=( char*)"EnumeratorField_tok"; struct _tagged_string _temp1130;
_temp1130.curr= _temp1129; _temp1130.base= _temp1129; _temp1130.last_plus_one=
_temp1129 + 20; _temp1130;});( struct _xenum_struct*) _temp1128;})); _LL1121:;}
struct Cyc_List_List* Cyc_yyget_EnumeratorFieldList_tok( struct _xenum_struct*
yy1){ struct _xenum_struct* _temp1131= yy1; struct Cyc_List_List* _temp1137;
_LL1133: if((*(( struct _xenum_struct*) _temp1131)).tag == Cyc_EnumeratorFieldList_tok_tag){
_LL1138: _temp1137=(( struct Cyc_EnumeratorFieldList_tok_struct*) _temp1131)->f1;
goto _LL1134;} else{ goto _LL1135;} _LL1135: goto _LL1136; _LL1134: return
_temp1137; _LL1136:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1139=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1139->tag= Cyc_Core_Failure_tag; _temp1139->f1=( struct _tagged_string)({
char* _temp1140=( char*)"EnumeratorFieldList_tok"; struct _tagged_string
_temp1141; _temp1141.curr= _temp1140; _temp1141.base= _temp1140; _temp1141.last_plus_one=
_temp1140 + 24; _temp1141;});( struct _xenum_struct*) _temp1139;})); _LL1132:;}
struct _tuple13* Cyc_yyget_QualSpecList_tok( struct _xenum_struct* yy1){ struct
_xenum_struct* _temp1142= yy1; struct _tuple13* _temp1148; _LL1144: if((*((
struct _xenum_struct*) _temp1142)).tag == Cyc_QualSpecList_tok_tag){ _LL1149:
_temp1148=(( struct Cyc_QualSpecList_tok_struct*) _temp1142)->f1; goto _LL1145;}
else{ goto _LL1146;} _LL1146: goto _LL1147; _LL1145: return _temp1148; _LL1147:(
void) _throw(({ struct Cyc_Core_Failure_struct* _temp1150=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp1150->tag= Cyc_Core_Failure_tag;
_temp1150->f1=( struct _tagged_string)({ char* _temp1151=( char*)"QualSpecList_tok";
struct _tagged_string _temp1152; _temp1152.curr= _temp1151; _temp1152.base=
_temp1151; _temp1152.last_plus_one= _temp1151 + 17; _temp1152;});( struct
_xenum_struct*) _temp1150;})); _LL1143:;} struct Cyc_List_List* Cyc_yyget_IdList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1153= yy1; struct Cyc_List_List*
_temp1159; _LL1155: if((*(( struct _xenum_struct*) _temp1153)).tag == Cyc_IdList_tok_tag){
_LL1160: _temp1159=(( struct Cyc_IdList_tok_struct*) _temp1153)->f1; goto
_LL1156;} else{ goto _LL1157;} _LL1157: goto _LL1158; _LL1156: return _temp1159;
_LL1158:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1161=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp1161->tag= Cyc_Core_Failure_tag;
_temp1161->f1=( struct _tagged_string)({ char* _temp1162=( char*)"IdList_tok";
struct _tagged_string _temp1163; _temp1163.curr= _temp1162; _temp1163.base=
_temp1162; _temp1163.last_plus_one= _temp1162 + 11; _temp1163;});( struct
_xenum_struct*) _temp1161;})); _LL1154:;} struct _tuple2* Cyc_yyget_ParamDecl_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1164= yy1; struct _tuple2*
_temp1170; _LL1166: if((*(( struct _xenum_struct*) _temp1164)).tag == Cyc_ParamDecl_tok_tag){
_LL1171: _temp1170=(( struct Cyc_ParamDecl_tok_struct*) _temp1164)->f1; goto
_LL1167;} else{ goto _LL1168;} _LL1168: goto _LL1169; _LL1167: return _temp1170;
_LL1169:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1172=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp1172->tag= Cyc_Core_Failure_tag;
_temp1172->f1=( struct _tagged_string)({ char* _temp1173=( char*)"ParamDecl_tok";
struct _tagged_string _temp1174; _temp1174.curr= _temp1173; _temp1174.base=
_temp1173; _temp1174.last_plus_one= _temp1173 + 14; _temp1174;});( struct
_xenum_struct*) _temp1172;})); _LL1165:;} struct Cyc_List_List* Cyc_yyget_ParamDeclList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1175= yy1; struct Cyc_List_List*
_temp1181; _LL1177: if((*(( struct _xenum_struct*) _temp1175)).tag == Cyc_ParamDeclList_tok_tag){
_LL1182: _temp1181=(( struct Cyc_ParamDeclList_tok_struct*) _temp1175)->f1; goto
_LL1178;} else{ goto _LL1179;} _LL1179: goto _LL1180; _LL1178: return _temp1181;
_LL1180:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1183=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp1183->tag= Cyc_Core_Failure_tag;
_temp1183->f1=( struct _tagged_string)({ char* _temp1184=( char*)"ParamDeclList_tok";
struct _tagged_string _temp1185; _temp1185.curr= _temp1184; _temp1185.base=
_temp1184; _temp1185.last_plus_one= _temp1184 + 18; _temp1185;});( struct
_xenum_struct*) _temp1183;})); _LL1176:;} struct _tuple14* Cyc_yyget_ParamDeclListBool_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1186= yy1; struct
_tuple14* _temp1192; _LL1188: if((*(( struct _xenum_struct*) _temp1186)).tag ==
Cyc_ParamDeclListBool_tok_tag){ _LL1193: _temp1192=(( struct Cyc_ParamDeclListBool_tok_struct*)
_temp1186)->f1; goto _LL1189;} else{ goto _LL1190;} _LL1190: goto _LL1191;
_LL1189: return _temp1192; _LL1191:( void) _throw(({ struct Cyc_Core_Failure_struct*
_temp1194=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1194->tag= Cyc_Core_Failure_tag; _temp1194->f1=( struct _tagged_string)({
char* _temp1195=( char*)"ParamDeclListBool_tok"; struct _tagged_string _temp1196;
_temp1196.curr= _temp1195; _temp1196.base= _temp1195; _temp1196.last_plus_one=
_temp1195 + 22; _temp1196;});( struct _xenum_struct*) _temp1194;})); _LL1187:;}
struct Cyc_List_List* Cyc_yyget_TypeList_tok( struct _xenum_struct* yy1){ struct
_xenum_struct* _temp1197= yy1; struct Cyc_List_List* _temp1203; _LL1199: if((*((
struct _xenum_struct*) _temp1197)).tag == Cyc_TypeList_tok_tag){ _LL1204:
_temp1203=(( struct Cyc_TypeList_tok_struct*) _temp1197)->f1; goto _LL1200;}
else{ goto _LL1201;} _LL1201: goto _LL1202; _LL1200: return _temp1203; _LL1202:(
void) _throw(({ struct Cyc_Core_Failure_struct* _temp1205=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp1205->tag= Cyc_Core_Failure_tag;
_temp1205->f1=( struct _tagged_string)({ char* _temp1206=( char*)"TypeList_tok";
struct _tagged_string _temp1207; _temp1207.curr= _temp1206; _temp1207.base=
_temp1206; _temp1207.last_plus_one= _temp1206 + 13; _temp1207;});( struct
_xenum_struct*) _temp1205;})); _LL1198:;} struct Cyc_List_List* Cyc_yyget_DesignatorList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1208= yy1; struct Cyc_List_List*
_temp1214; _LL1210: if((*(( struct _xenum_struct*) _temp1208)).tag == Cyc_DesignatorList_tok_tag){
_LL1215: _temp1214=(( struct Cyc_DesignatorList_tok_struct*) _temp1208)->f1;
goto _LL1211;} else{ goto _LL1212;} _LL1212: goto _LL1213; _LL1211: return
_temp1214; _LL1213:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1216=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1216->tag= Cyc_Core_Failure_tag; _temp1216->f1=( struct _tagged_string)({
char* _temp1217=( char*)"DesignatorList_tok"; struct _tagged_string _temp1218;
_temp1218.curr= _temp1217; _temp1218.base= _temp1217; _temp1218.last_plus_one=
_temp1217 + 19; _temp1218;});( struct _xenum_struct*) _temp1216;})); _LL1209:;}
void* Cyc_yyget_Designator_tok( struct _xenum_struct* yy1){ struct _xenum_struct*
_temp1219= yy1; void* _temp1225; _LL1221: if((*(( struct _xenum_struct*)
_temp1219)).tag == Cyc_Designator_tok_tag){ _LL1226: _temp1225=(( struct Cyc_Designator_tok_struct*)
_temp1219)->f1; goto _LL1222;} else{ goto _LL1223;} _LL1223: goto _LL1224;
_LL1222: return _temp1225; _LL1224:( void) _throw(({ struct Cyc_Core_Failure_struct*
_temp1227=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1227->tag= Cyc_Core_Failure_tag; _temp1227->f1=( struct _tagged_string)({
char* _temp1228=( char*)"Designator_tok"; struct _tagged_string _temp1229;
_temp1229.curr= _temp1228; _temp1229.base= _temp1228; _temp1229.last_plus_one=
_temp1228 + 15; _temp1229;});( struct _xenum_struct*) _temp1227;})); _LL1220:;}
void* Cyc_yyget_Kind_tok( struct _xenum_struct* yy1){ struct _xenum_struct*
_temp1230= yy1; void* _temp1236; _LL1232: if((*(( struct _xenum_struct*)
_temp1230)).tag == Cyc_Kind_tok_tag){ _LL1237: _temp1236=(( struct Cyc_Kind_tok_struct*)
_temp1230)->f1; goto _LL1233;} else{ goto _LL1234;} _LL1234: goto _LL1235;
_LL1233: return _temp1236; _LL1235:( void) _throw(({ struct Cyc_Core_Failure_struct*
_temp1238=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp1238->tag= Cyc_Core_Failure_tag; _temp1238->f1=( struct _tagged_string)({
char* _temp1239=( char*)"Kind_tok"; struct _tagged_string _temp1240; _temp1240.curr=
_temp1239; _temp1240.base= _temp1239; _temp1240.last_plus_one= _temp1239 + 9;
_temp1240;});( struct _xenum_struct*) _temp1238;})); _LL1231:;} void* Cyc_yyget_Type_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1241= yy1; void*
_temp1247; _LL1243: if((*(( struct _xenum_struct*) _temp1241)).tag == Cyc_Type_tok_tag){
_LL1248: _temp1247=(( struct Cyc_Type_tok_struct*) _temp1241)->f1; goto _LL1244;}
else{ goto _LL1245;} _LL1245: goto _LL1246; _LL1244: return _temp1247; _LL1246:(
void) _throw(({ struct Cyc_Core_Failure_struct* _temp1249=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp1249->tag= Cyc_Core_Failure_tag;
_temp1249->f1=( struct _tagged_string)({ char* _temp1250=( char*)"Type_tok";
struct _tagged_string _temp1251; _temp1251.curr= _temp1250; _temp1251.base=
_temp1250; _temp1251.last_plus_one= _temp1250 + 9; _temp1251;});( struct
_xenum_struct*) _temp1249;})); _LL1242:;} struct Cyc_List_List* Cyc_yyget_AttributeList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1252= yy1; struct Cyc_List_List*
_temp1258; _LL1254: if((*(( struct _xenum_struct*) _temp1252)).tag == Cyc_AttributeList_tok_tag){
_LL1259: _temp1258=(( struct Cyc_AttributeList_tok_struct*) _temp1252)->f1; goto
_LL1255;} else{ goto _LL1256;} _LL1256: goto _LL1257; _LL1255: return _temp1258;
_LL1257:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1260=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp1260->tag= Cyc_Core_Failure_tag;
_temp1260->f1=( struct _tagged_string)({ char* _temp1261=( char*)"AttributeList_tok";
struct _tagged_string _temp1262; _temp1262.curr= _temp1261; _temp1262.base=
_temp1261; _temp1262.last_plus_one= _temp1261 + 18; _temp1262;});( struct
_xenum_struct*) _temp1260;})); _LL1253:;} void* Cyc_yyget_Attribute_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp1263= yy1; void* _temp1269;
_LL1265: if((*(( struct _xenum_struct*) _temp1263)).tag == Cyc_Attribute_tok_tag){
_LL1270: _temp1269=(( struct Cyc_Attribute_tok_struct*) _temp1263)->f1; goto
_LL1266;} else{ goto _LL1267;} _LL1267: goto _LL1268; _LL1266: return _temp1269;
_LL1268:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1271=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp1271->tag= Cyc_Core_Failure_tag;
_temp1271->f1=( struct _tagged_string)({ char* _temp1272=( char*)"Attribute_tok";
struct _tagged_string _temp1273; _temp1273.curr= _temp1272; _temp1273.base=
_temp1272; _temp1273.last_plus_one= _temp1272 + 14; _temp1273;});( struct
_xenum_struct*) _temp1271;})); _LL1264:;} struct Cyc_Yyltype{ int timestamp; int
first_line; int first_column; int last_line; int last_column; struct
_tagged_string text; } ; typedef struct Cyc_Yyltype Cyc_yyltype; struct Cyc_Yyltype
Cyc_yynewloc(){ return({ struct Cyc_Yyltype _temp1274; _temp1274.timestamp= 0;
_temp1274.first_line= 0; _temp1274.first_column= 0; _temp1274.last_line= 0;
_temp1274.last_column= 0; _temp1274.text=( struct _tagged_string)({ char*
_temp1275=( char*)""; struct _tagged_string _temp1276; _temp1276.curr= _temp1275;
_temp1276.base= _temp1275; _temp1276.last_plus_one= _temp1275 + 1; _temp1276;});
_temp1274;});} static char _temp1279[ 1u]=""; struct Cyc_Yyltype Cyc_yylloc=(
struct Cyc_Yyltype){.timestamp= 0,.first_line= 0,.first_column= 0,.last_line= 0,.last_column=
0,.text=( struct _tagged_string){ _temp1279, _temp1279, _temp1279 + 1u}}; static
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
_temp1282[ 2u]="$"; static char _temp1285[ 6u]="error"; static char _temp1288[
12u]="$undefined."; static char _temp1291[ 5u]="AUTO"; static char _temp1294[ 9u]="REGISTER";
static char _temp1297[ 7u]="STATIC"; static char _temp1300[ 7u]="EXTERN"; static
char _temp1303[ 8u]="TYPEDEF"; static char _temp1306[ 5u]="VOID"; static char
_temp1309[ 5u]="CHAR"; static char _temp1312[ 6u]="SHORT"; static char _temp1315[
4u]="INT"; static char _temp1318[ 5u]="LONG"; static char _temp1321[ 6u]="FLOAT";
static char _temp1324[ 7u]="DOUBLE"; static char _temp1327[ 7u]="SIGNED"; static
char _temp1330[ 9u]="UNSIGNED"; static char _temp1333[ 6u]="CONST"; static char
_temp1336[ 9u]="VOLATILE"; static char _temp1339[ 9u]="RESTRICT"; static char
_temp1342[ 7u]="STRUCT"; static char _temp1345[ 6u]="UNION"; static char
_temp1348[ 5u]="CASE"; static char _temp1351[ 8u]="DEFAULT"; static char
_temp1354[ 7u]="INLINE"; static char _temp1357[ 3u]="IF"; static char _temp1360[
5u]="ELSE"; static char _temp1363[ 7u]="SWITCH"; static char _temp1366[ 6u]="WHILE";
static char _temp1369[ 3u]="DO"; static char _temp1372[ 4u]="FOR"; static char
_temp1375[ 5u]="GOTO"; static char _temp1378[ 9u]="CONTINUE"; static char
_temp1381[ 6u]="BREAK"; static char _temp1384[ 7u]="RETURN"; static char
_temp1387[ 7u]="SIZEOF"; static char _temp1390[ 5u]="ENUM"; static char
_temp1393[ 8u]="NULL_kw"; static char _temp1396[ 4u]="LET"; static char
_temp1399[ 6u]="THROW"; static char _temp1402[ 4u]="TRY"; static char _temp1405[
6u]="CATCH"; static char _temp1408[ 4u]="NEW"; static char _temp1411[ 9u]="ABSTRACT";
static char _temp1414[ 9u]="FALLTHRU"; static char _temp1417[ 6u]="USING";
static char _temp1420[ 10u]="NAMESPACE"; static char _temp1423[ 6u]="XENUM";
static char _temp1426[ 5u]="FILL"; static char _temp1429[ 8u]="CODEGEN"; static
char _temp1432[ 4u]="CUT"; static char _temp1435[ 7u]="SPLICE"; static char
_temp1438[ 7u]="PRINTF"; static char _temp1441[ 8u]="FPRINTF"; static char
_temp1444[ 8u]="XPRINTF"; static char _temp1447[ 6u]="SCANF"; static char
_temp1450[ 7u]="FSCANF"; static char _temp1453[ 7u]="SSCANF"; static char
_temp1456[ 7u]="MALLOC"; static char _temp1459[ 7u]="PTR_OP"; static char
_temp1462[ 7u]="INC_OP"; static char _temp1465[ 7u]="DEC_OP"; static char
_temp1468[ 8u]="LEFT_OP"; static char _temp1471[ 9u]="RIGHT_OP"; static char
_temp1474[ 6u]="LE_OP"; static char _temp1477[ 6u]="GE_OP"; static char
_temp1480[ 6u]="EQ_OP"; static char _temp1483[ 6u]="NE_OP"; static char
_temp1486[ 7u]="AND_OP"; static char _temp1489[ 6u]="OR_OP"; static char
_temp1492[ 11u]="MUL_ASSIGN"; static char _temp1495[ 11u]="DIV_ASSIGN"; static
char _temp1498[ 11u]="MOD_ASSIGN"; static char _temp1501[ 11u]="ADD_ASSIGN";
static char _temp1504[ 11u]="SUB_ASSIGN"; static char _temp1507[ 12u]="LEFT_ASSIGN";
static char _temp1510[ 13u]="RIGHT_ASSIGN"; static char _temp1513[ 11u]="AND_ASSIGN";
static char _temp1516[ 11u]="XOR_ASSIGN"; static char _temp1519[ 10u]="OR_ASSIGN";
static char _temp1522[ 9u]="ELLIPSIS"; static char _temp1525[ 11u]="LEFT_RIGHT";
static char _temp1528[ 12u]="COLON_COLON"; static char _temp1531[ 11u]="IDENTIFIER";
static char _temp1534[ 17u]="INTEGER_CONSTANT"; static char _temp1537[ 7u]="STRING";
static char _temp1540[ 19u]="CHARACTER_CONSTANT"; static char _temp1543[ 18u]="FLOATING_CONSTANT";
static char _temp1546[ 9u]="TYPE_VAR"; static char _temp1549[ 16u]="QUAL_IDENTIFIER";
static char _temp1552[ 18u]="QUAL_TYPEDEF_NAME"; static char _temp1555[ 10u]="ATTRIBUTE";
static char _temp1558[ 4u]="';'"; static char _temp1561[ 4u]="'{'"; static char
_temp1564[ 4u]="'}'"; static char _temp1567[ 4u]="'='"; static char _temp1570[ 4u]="'('";
static char _temp1573[ 4u]="')'"; static char _temp1576[ 4u]="','"; static char
_temp1579[ 4u]="'_'"; static char _temp1582[ 4u]="'$'"; static char _temp1585[ 4u]="'<'";
static char _temp1588[ 4u]="'>'"; static char _temp1591[ 4u]="':'"; static char
_temp1594[ 4u]="'['"; static char _temp1597[ 4u]="']'"; static char _temp1600[ 4u]="'*'";
static char _temp1603[ 4u]="'@'"; static char _temp1606[ 4u]="'?'"; static char
_temp1609[ 4u]="'+'"; static char _temp1612[ 4u]="'.'"; static char _temp1615[ 4u]="'-'";
static char _temp1618[ 4u]="'&'"; static char _temp1621[ 4u]="'|'"; static char
_temp1624[ 4u]="'^'"; static char _temp1627[ 4u]="'/'"; static char _temp1630[ 4u]="'%'";
static char _temp1633[ 4u]="'~'"; static char _temp1636[ 4u]="'!'"; static char
_temp1639[ 5u]="prog"; static char _temp1642[ 17u]="translation_unit"; static
char _temp1645[ 21u]="translation_unit_opt"; static char _temp1648[ 21u]="external_declaration";
static char _temp1651[ 20u]="function_definition"; static char _temp1654[ 21u]="function_definition2";
static char _temp1657[ 13u]="using_action"; static char _temp1660[ 15u]="unusing_action";
static char _temp1663[ 17u]="namespace_action"; static char _temp1666[ 19u]="unnamespace_action";
static char _temp1669[ 12u]="declaration"; static char _temp1672[ 17u]="declaration_list";
static char _temp1675[ 23u]="declaration_specifiers"; static char _temp1678[ 24u]="storage_class_specifier";
static char _temp1681[ 15u]="attributes_opt"; static char _temp1684[ 11u]="attributes";
static char _temp1687[ 15u]="attribute_list"; static char _temp1690[ 10u]="attribute";
static char _temp1693[ 15u]="type_specifier"; static char _temp1696[ 5u]="kind";
static char _temp1699[ 15u]="type_qualifier"; static char _temp1702[ 26u]="struct_or_union_specifier";
static char _temp1705[ 16u]="type_params_opt"; static char _temp1708[ 16u]="struct_or_union";
static char _temp1711[ 24u]="struct_declaration_list"; static char _temp1714[ 25u]="struct_declaration_list0";
static char _temp1717[ 21u]="init_declarator_list"; static char _temp1720[ 22u]="init_declarator_list0";
static char _temp1723[ 16u]="init_declarator"; static char _temp1726[ 19u]="struct_declaration";
static char _temp1729[ 25u]="specifier_qualifier_list"; static char _temp1732[
23u]="struct_declarator_list"; static char _temp1735[ 24u]="struct_declarator_list0";
static char _temp1738[ 18u]="struct_declarator"; static char _temp1741[ 15u]="enum_specifier";
static char _temp1744[ 16u]="enumerator_list"; static char _temp1747[ 11u]="enumerator";
static char _temp1750[ 11u]="declarator"; static char _temp1753[ 18u]="direct_declarator";
static char _temp1756[ 8u]="pointer"; static char _temp1759[ 13u]="pointer_char";
static char _temp1762[ 8u]="rgn_opt"; static char _temp1765[ 20u]="type_qualifier_list";
static char _temp1768[ 20u]="parameter_type_list"; static char _temp1771[ 11u]="effect_set";
static char _temp1774[ 14u]="atomic_effect"; static char _temp1777[ 11u]="region_set";
static char _temp1780[ 15u]="parameter_list"; static char _temp1783[ 22u]="parameter_declaration";
static char _temp1786[ 16u]="identifier_list"; static char _temp1789[ 17u]="identifier_list0";
static char _temp1792[ 12u]="initializer"; static char _temp1795[ 17u]="initializer_list";
static char _temp1798[ 12u]="designation"; static char _temp1801[ 16u]="designator_list";
static char _temp1804[ 11u]="designator"; static char _temp1807[ 10u]="type_name";
static char _temp1810[ 14u]="any_type_name"; static char _temp1813[ 15u]="type_name_list";
static char _temp1816[ 20u]="abstract_declarator"; static char _temp1819[ 27u]="direct_abstract_declarator";
static char _temp1822[ 10u]="statement"; static char _temp1825[ 18u]="labeled_statement";
static char _temp1828[ 21u]="expression_statement"; static char _temp1831[ 19u]="compound_statement";
static char _temp1834[ 16u]="block_item_list"; static char _temp1837[ 11u]="block_item";
static char _temp1840[ 20u]="selection_statement"; static char _temp1843[ 15u]="switch_clauses";
static char _temp1846[ 20u]="iteration_statement"; static char _temp1849[ 15u]="jump_statement";
static char _temp1852[ 8u]="pattern"; static char _temp1855[ 19u]="tuple_pattern_list";
static char _temp1858[ 20u]="tuple_pattern_list0"; static char _temp1861[ 14u]="field_pattern";
static char _temp1864[ 19u]="field_pattern_list"; static char _temp1867[ 20u]="field_pattern_list0";
static char _temp1870[ 11u]="expression"; static char _temp1873[ 22u]="assignment_expression";
static char _temp1876[ 20u]="assignment_operator"; static char _temp1879[ 23u]="conditional_expression";
static char _temp1882[ 20u]="constant_expression"; static char _temp1885[ 22u]="logical_or_expression";
static char _temp1888[ 23u]="logical_and_expression"; static char _temp1891[ 24u]="inclusive_or_expression";
static char _temp1894[ 24u]="exclusive_or_expression"; static char _temp1897[ 15u]="and_expression";
static char _temp1900[ 20u]="equality_expression"; static char _temp1903[ 22u]="relational_expression";
static char _temp1906[ 17u]="shift_expression"; static char _temp1909[ 20u]="additive_expression";
static char _temp1912[ 26u]="multiplicative_expression"; static char _temp1915[
16u]="cast_expression"; static char _temp1918[ 17u]="unary_expression"; static
char _temp1921[ 14u]="format_primop"; static char _temp1924[ 15u]="unary_operator";
static char _temp1927[ 19u]="postfix_expression"; static char _temp1930[ 19u]="primary_expression";
static char _temp1933[ 25u]="argument_expression_list"; static char _temp1936[
26u]="argument_expression_list0"; static char _temp1939[ 9u]="constant"; static
char _temp1942[ 20u]="qual_opt_identifier"; static struct _tagged_string Cyc_yytname[
221u]={( struct _tagged_string){ _temp1282, _temp1282, _temp1282 + 2u},( struct
_tagged_string){ _temp1285, _temp1285, _temp1285 + 6u},( struct _tagged_string){
_temp1288, _temp1288, _temp1288 + 12u},( struct _tagged_string){ _temp1291,
_temp1291, _temp1291 + 5u},( struct _tagged_string){ _temp1294, _temp1294,
_temp1294 + 9u},( struct _tagged_string){ _temp1297, _temp1297, _temp1297 + 7u},(
struct _tagged_string){ _temp1300, _temp1300, _temp1300 + 7u},( struct
_tagged_string){ _temp1303, _temp1303, _temp1303 + 8u},( struct _tagged_string){
_temp1306, _temp1306, _temp1306 + 5u},( struct _tagged_string){ _temp1309,
_temp1309, _temp1309 + 5u},( struct _tagged_string){ _temp1312, _temp1312,
_temp1312 + 6u},( struct _tagged_string){ _temp1315, _temp1315, _temp1315 + 4u},(
struct _tagged_string){ _temp1318, _temp1318, _temp1318 + 5u},( struct
_tagged_string){ _temp1321, _temp1321, _temp1321 + 6u},( struct _tagged_string){
_temp1324, _temp1324, _temp1324 + 7u},( struct _tagged_string){ _temp1327,
_temp1327, _temp1327 + 7u},( struct _tagged_string){ _temp1330, _temp1330,
_temp1330 + 9u},( struct _tagged_string){ _temp1333, _temp1333, _temp1333 + 6u},(
struct _tagged_string){ _temp1336, _temp1336, _temp1336 + 9u},( struct
_tagged_string){ _temp1339, _temp1339, _temp1339 + 9u},( struct _tagged_string){
_temp1342, _temp1342, _temp1342 + 7u},( struct _tagged_string){ _temp1345,
_temp1345, _temp1345 + 6u},( struct _tagged_string){ _temp1348, _temp1348,
_temp1348 + 5u},( struct _tagged_string){ _temp1351, _temp1351, _temp1351 + 8u},(
struct _tagged_string){ _temp1354, _temp1354, _temp1354 + 7u},( struct
_tagged_string){ _temp1357, _temp1357, _temp1357 + 3u},( struct _tagged_string){
_temp1360, _temp1360, _temp1360 + 5u},( struct _tagged_string){ _temp1363,
_temp1363, _temp1363 + 7u},( struct _tagged_string){ _temp1366, _temp1366,
_temp1366 + 6u},( struct _tagged_string){ _temp1369, _temp1369, _temp1369 + 3u},(
struct _tagged_string){ _temp1372, _temp1372, _temp1372 + 4u},( struct
_tagged_string){ _temp1375, _temp1375, _temp1375 + 5u},( struct _tagged_string){
_temp1378, _temp1378, _temp1378 + 9u},( struct _tagged_string){ _temp1381,
_temp1381, _temp1381 + 6u},( struct _tagged_string){ _temp1384, _temp1384,
_temp1384 + 7u},( struct _tagged_string){ _temp1387, _temp1387, _temp1387 + 7u},(
struct _tagged_string){ _temp1390, _temp1390, _temp1390 + 5u},( struct
_tagged_string){ _temp1393, _temp1393, _temp1393 + 8u},( struct _tagged_string){
_temp1396, _temp1396, _temp1396 + 4u},( struct _tagged_string){ _temp1399,
_temp1399, _temp1399 + 6u},( struct _tagged_string){ _temp1402, _temp1402,
_temp1402 + 4u},( struct _tagged_string){ _temp1405, _temp1405, _temp1405 + 6u},(
struct _tagged_string){ _temp1408, _temp1408, _temp1408 + 4u},( struct
_tagged_string){ _temp1411, _temp1411, _temp1411 + 9u},( struct _tagged_string){
_temp1414, _temp1414, _temp1414 + 9u},( struct _tagged_string){ _temp1417,
_temp1417, _temp1417 + 6u},( struct _tagged_string){ _temp1420, _temp1420,
_temp1420 + 10u},( struct _tagged_string){ _temp1423, _temp1423, _temp1423 + 6u},(
struct _tagged_string){ _temp1426, _temp1426, _temp1426 + 5u},( struct
_tagged_string){ _temp1429, _temp1429, _temp1429 + 8u},( struct _tagged_string){
_temp1432, _temp1432, _temp1432 + 4u},( struct _tagged_string){ _temp1435,
_temp1435, _temp1435 + 7u},( struct _tagged_string){ _temp1438, _temp1438,
_temp1438 + 7u},( struct _tagged_string){ _temp1441, _temp1441, _temp1441 + 8u},(
struct _tagged_string){ _temp1444, _temp1444, _temp1444 + 8u},( struct
_tagged_string){ _temp1447, _temp1447, _temp1447 + 6u},( struct _tagged_string){
_temp1450, _temp1450, _temp1450 + 7u},( struct _tagged_string){ _temp1453,
_temp1453, _temp1453 + 7u},( struct _tagged_string){ _temp1456, _temp1456,
_temp1456 + 7u},( struct _tagged_string){ _temp1459, _temp1459, _temp1459 + 7u},(
struct _tagged_string){ _temp1462, _temp1462, _temp1462 + 7u},( struct
_tagged_string){ _temp1465, _temp1465, _temp1465 + 7u},( struct _tagged_string){
_temp1468, _temp1468, _temp1468 + 8u},( struct _tagged_string){ _temp1471,
_temp1471, _temp1471 + 9u},( struct _tagged_string){ _temp1474, _temp1474,
_temp1474 + 6u},( struct _tagged_string){ _temp1477, _temp1477, _temp1477 + 6u},(
struct _tagged_string){ _temp1480, _temp1480, _temp1480 + 6u},( struct
_tagged_string){ _temp1483, _temp1483, _temp1483 + 6u},( struct _tagged_string){
_temp1486, _temp1486, _temp1486 + 7u},( struct _tagged_string){ _temp1489,
_temp1489, _temp1489 + 6u},( struct _tagged_string){ _temp1492, _temp1492,
_temp1492 + 11u},( struct _tagged_string){ _temp1495, _temp1495, _temp1495 + 11u},(
struct _tagged_string){ _temp1498, _temp1498, _temp1498 + 11u},( struct
_tagged_string){ _temp1501, _temp1501, _temp1501 + 11u},( struct _tagged_string){
_temp1504, _temp1504, _temp1504 + 11u},( struct _tagged_string){ _temp1507,
_temp1507, _temp1507 + 12u},( struct _tagged_string){ _temp1510, _temp1510,
_temp1510 + 13u},( struct _tagged_string){ _temp1513, _temp1513, _temp1513 + 11u},(
struct _tagged_string){ _temp1516, _temp1516, _temp1516 + 11u},( struct
_tagged_string){ _temp1519, _temp1519, _temp1519 + 10u},( struct _tagged_string){
_temp1522, _temp1522, _temp1522 + 9u},( struct _tagged_string){ _temp1525,
_temp1525, _temp1525 + 11u},( struct _tagged_string){ _temp1528, _temp1528,
_temp1528 + 12u},( struct _tagged_string){ _temp1531, _temp1531, _temp1531 + 11u},(
struct _tagged_string){ _temp1534, _temp1534, _temp1534 + 17u},( struct
_tagged_string){ _temp1537, _temp1537, _temp1537 + 7u},( struct _tagged_string){
_temp1540, _temp1540, _temp1540 + 19u},( struct _tagged_string){ _temp1543,
_temp1543, _temp1543 + 18u},( struct _tagged_string){ _temp1546, _temp1546,
_temp1546 + 9u},( struct _tagged_string){ _temp1549, _temp1549, _temp1549 + 16u},(
struct _tagged_string){ _temp1552, _temp1552, _temp1552 + 18u},( struct
_tagged_string){ _temp1555, _temp1555, _temp1555 + 10u},( struct _tagged_string){
_temp1558, _temp1558, _temp1558 + 4u},( struct _tagged_string){ _temp1561,
_temp1561, _temp1561 + 4u},( struct _tagged_string){ _temp1564, _temp1564,
_temp1564 + 4u},( struct _tagged_string){ _temp1567, _temp1567, _temp1567 + 4u},(
struct _tagged_string){ _temp1570, _temp1570, _temp1570 + 4u},( struct
_tagged_string){ _temp1573, _temp1573, _temp1573 + 4u},( struct _tagged_string){
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
_temp1636 + 4u},( struct _tagged_string){ _temp1639, _temp1639, _temp1639 + 5u},(
struct _tagged_string){ _temp1642, _temp1642, _temp1642 + 17u},( struct
_tagged_string){ _temp1645, _temp1645, _temp1645 + 21u},( struct _tagged_string){
_temp1648, _temp1648, _temp1648 + 21u},( struct _tagged_string){ _temp1651,
_temp1651, _temp1651 + 20u},( struct _tagged_string){ _temp1654, _temp1654,
_temp1654 + 21u},( struct _tagged_string){ _temp1657, _temp1657, _temp1657 + 13u},(
struct _tagged_string){ _temp1660, _temp1660, _temp1660 + 15u},( struct
_tagged_string){ _temp1663, _temp1663, _temp1663 + 17u},( struct _tagged_string){
_temp1666, _temp1666, _temp1666 + 19u},( struct _tagged_string){ _temp1669,
_temp1669, _temp1669 + 12u},( struct _tagged_string){ _temp1672, _temp1672,
_temp1672 + 17u},( struct _tagged_string){ _temp1675, _temp1675, _temp1675 + 23u},(
struct _tagged_string){ _temp1678, _temp1678, _temp1678 + 24u},( struct
_tagged_string){ _temp1681, _temp1681, _temp1681 + 15u},( struct _tagged_string){
_temp1684, _temp1684, _temp1684 + 11u},( struct _tagged_string){ _temp1687,
_temp1687, _temp1687 + 15u},( struct _tagged_string){ _temp1690, _temp1690,
_temp1690 + 10u},( struct _tagged_string){ _temp1693, _temp1693, _temp1693 + 15u},(
struct _tagged_string){ _temp1696, _temp1696, _temp1696 + 5u},( struct
_tagged_string){ _temp1699, _temp1699, _temp1699 + 15u},( struct _tagged_string){
_temp1702, _temp1702, _temp1702 + 26u},( struct _tagged_string){ _temp1705,
_temp1705, _temp1705 + 16u},( struct _tagged_string){ _temp1708, _temp1708,
_temp1708 + 16u},( struct _tagged_string){ _temp1711, _temp1711, _temp1711 + 24u},(
struct _tagged_string){ _temp1714, _temp1714, _temp1714 + 25u},( struct
_tagged_string){ _temp1717, _temp1717, _temp1717 + 21u},( struct _tagged_string){
_temp1720, _temp1720, _temp1720 + 22u},( struct _tagged_string){ _temp1723,
_temp1723, _temp1723 + 16u},( struct _tagged_string){ _temp1726, _temp1726,
_temp1726 + 19u},( struct _tagged_string){ _temp1729, _temp1729, _temp1729 + 25u},(
struct _tagged_string){ _temp1732, _temp1732, _temp1732 + 23u},( struct
_tagged_string){ _temp1735, _temp1735, _temp1735 + 24u},( struct _tagged_string){
_temp1738, _temp1738, _temp1738 + 18u},( struct _tagged_string){ _temp1741,
_temp1741, _temp1741 + 15u},( struct _tagged_string){ _temp1744, _temp1744,
_temp1744 + 16u},( struct _tagged_string){ _temp1747, _temp1747, _temp1747 + 11u},(
struct _tagged_string){ _temp1750, _temp1750, _temp1750 + 11u},( struct
_tagged_string){ _temp1753, _temp1753, _temp1753 + 18u},( struct _tagged_string){
_temp1756, _temp1756, _temp1756 + 8u},( struct _tagged_string){ _temp1759,
_temp1759, _temp1759 + 13u},( struct _tagged_string){ _temp1762, _temp1762,
_temp1762 + 8u},( struct _tagged_string){ _temp1765, _temp1765, _temp1765 + 20u},(
struct _tagged_string){ _temp1768, _temp1768, _temp1768 + 20u},( struct
_tagged_string){ _temp1771, _temp1771, _temp1771 + 11u},( struct _tagged_string){
_temp1774, _temp1774, _temp1774 + 14u},( struct _tagged_string){ _temp1777,
_temp1777, _temp1777 + 11u},( struct _tagged_string){ _temp1780, _temp1780,
_temp1780 + 15u},( struct _tagged_string){ _temp1783, _temp1783, _temp1783 + 22u},(
struct _tagged_string){ _temp1786, _temp1786, _temp1786 + 16u},( struct
_tagged_string){ _temp1789, _temp1789, _temp1789 + 17u},( struct _tagged_string){
_temp1792, _temp1792, _temp1792 + 12u},( struct _tagged_string){ _temp1795,
_temp1795, _temp1795 + 17u},( struct _tagged_string){ _temp1798, _temp1798,
_temp1798 + 12u},( struct _tagged_string){ _temp1801, _temp1801, _temp1801 + 16u},(
struct _tagged_string){ _temp1804, _temp1804, _temp1804 + 11u},( struct
_tagged_string){ _temp1807, _temp1807, _temp1807 + 10u},( struct _tagged_string){
_temp1810, _temp1810, _temp1810 + 14u},( struct _tagged_string){ _temp1813,
_temp1813, _temp1813 + 15u},( struct _tagged_string){ _temp1816, _temp1816,
_temp1816 + 20u},( struct _tagged_string){ _temp1819, _temp1819, _temp1819 + 27u},(
struct _tagged_string){ _temp1822, _temp1822, _temp1822 + 10u},( struct
_tagged_string){ _temp1825, _temp1825, _temp1825 + 18u},( struct _tagged_string){
_temp1828, _temp1828, _temp1828 + 21u},( struct _tagged_string){ _temp1831,
_temp1831, _temp1831 + 19u},( struct _tagged_string){ _temp1834, _temp1834,
_temp1834 + 16u},( struct _tagged_string){ _temp1837, _temp1837, _temp1837 + 11u},(
struct _tagged_string){ _temp1840, _temp1840, _temp1840 + 20u},( struct
_tagged_string){ _temp1843, _temp1843, _temp1843 + 15u},( struct _tagged_string){
_temp1846, _temp1846, _temp1846 + 20u},( struct _tagged_string){ _temp1849,
_temp1849, _temp1849 + 15u},( struct _tagged_string){ _temp1852, _temp1852,
_temp1852 + 8u},( struct _tagged_string){ _temp1855, _temp1855, _temp1855 + 19u},(
struct _tagged_string){ _temp1858, _temp1858, _temp1858 + 20u},( struct
_tagged_string){ _temp1861, _temp1861, _temp1861 + 14u},( struct _tagged_string){
_temp1864, _temp1864, _temp1864 + 19u},( struct _tagged_string){ _temp1867,
_temp1867, _temp1867 + 20u},( struct _tagged_string){ _temp1870, _temp1870,
_temp1870 + 11u},( struct _tagged_string){ _temp1873, _temp1873, _temp1873 + 22u},(
struct _tagged_string){ _temp1876, _temp1876, _temp1876 + 20u},( struct
_tagged_string){ _temp1879, _temp1879, _temp1879 + 23u},( struct _tagged_string){
_temp1882, _temp1882, _temp1882 + 20u},( struct _tagged_string){ _temp1885,
_temp1885, _temp1885 + 22u},( struct _tagged_string){ _temp1888, _temp1888,
_temp1888 + 23u},( struct _tagged_string){ _temp1891, _temp1891, _temp1891 + 24u},(
struct _tagged_string){ _temp1894, _temp1894, _temp1894 + 24u},( struct
_tagged_string){ _temp1897, _temp1897, _temp1897 + 15u},( struct _tagged_string){
_temp1900, _temp1900, _temp1900 + 20u},( struct _tagged_string){ _temp1903,
_temp1903, _temp1903 + 22u},( struct _tagged_string){ _temp1906, _temp1906,
_temp1906 + 17u},( struct _tagged_string){ _temp1909, _temp1909, _temp1909 + 20u},(
struct _tagged_string){ _temp1912, _temp1912, _temp1912 + 26u},( struct
_tagged_string){ _temp1915, _temp1915, _temp1915 + 16u},( struct _tagged_string){
_temp1918, _temp1918, _temp1918 + 17u},( struct _tagged_string){ _temp1921,
_temp1921, _temp1921 + 14u},( struct _tagged_string){ _temp1924, _temp1924,
_temp1924 + 15u},( struct _tagged_string){ _temp1927, _temp1927, _temp1927 + 19u},(
struct _tagged_string){ _temp1930, _temp1930, _temp1930 + 19u},( struct
_tagged_string){ _temp1933, _temp1933, _temp1933 + 25u},( struct _tagged_string){
_temp1936, _temp1936, _temp1936 + 26u},( struct _tagged_string){ _temp1939,
_temp1939, _temp1939 + 9u},( struct _tagged_string){ _temp1942, _temp1942,
_temp1942 + 20u}}; static short Cyc_yyr1[ 381u]={ (short)0, (short)119, (short)120,
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
(short)170, (short)170, (short)170, (short)171, (short)171, (short)171, (short)171,
(short)172, (short)173, (short)173, (short)174, (short)174, (short)175, (short)175,
(short)176, (short)176, (short)176, (short)176, (short)177, (short)177, (short)178,
(short)178, (short)178, (short)179, (short)179, (short)179, (short)179, (short)179,
(short)179, (short)179, (short)179, (short)179, (short)179, (short)179, (short)179,
(short)179, (short)179, (short)180, (short)180, (short)180, (short)180, (short)180,
(short)180, (short)180, (short)180, (short)181, (short)182, (short)182, (short)183,
(short)183, (short)184, (short)184, (short)185, (short)185, (short)185, (short)186,
(short)186, (short)186, (short)186, (short)187, (short)187, (short)187, (short)187,
(short)187, (short)187, (short)188, (short)188, (short)188, (short)188, (short)188,
(short)188, (short)188, (short)188, (short)188, (short)188, (short)188, (short)188,
(short)188, (short)188, (short)189, (short)189, (short)189, (short)189, (short)189,
(short)189, (short)189, (short)189, (short)190, (short)190, (short)190, (short)190,
(short)190, (short)190, (short)190, (short)190, (short)190, (short)190, (short)190,
(short)190, (short)190, (short)190, (short)191, (short)191, (short)192, (short)192,
(short)193, (short)193, (short)194, (short)195, (short)195, (short)196, (short)196,
(short)197, (short)197, (short)198, (short)198, (short)198, (short)198, (short)198,
(short)198, (short)198, (short)198, (short)198, (short)198, (short)198, (short)199,
(short)199, (short)200, (short)201, (short)201, (short)202, (short)202, (short)203,
(short)203, (short)204, (short)204, (short)205, (short)205, (short)206, (short)206,
(short)206, (short)207, (short)207, (short)207, (short)207, (short)207, (short)208,
(short)208, (short)208, (short)209, (short)209, (short)209, (short)210, (short)210,
(short)210, (short)210, (short)211, (short)211, (short)212, (short)212, (short)212,
(short)212, (short)212, (short)212, (short)212, (short)212, (short)212, (short)212,
(short)212, (short)212, (short)212, (short)213, (short)213, (short)213, (short)213,
(short)213, (short)213, (short)214, (short)214, (short)214, (short)215, (short)215,
(short)215, (short)215, (short)215, (short)215, (short)215, (short)215, (short)215,
(short)215, (short)215, (short)215, (short)215, (short)215, (short)215, (short)215,
(short)215, (short)215, (short)215, (short)216, (short)216, (short)216, (short)216,
(short)216, (short)216, (short)216, (short)216, (short)216, (short)217, (short)218,
(short)218, (short)219, (short)219, (short)219, (short)219, (short)220, (short)220};
static short Cyc_yyr2[ 381u]={ (short)0, (short)1, (short)1, (short)2, (short)3,
(short)5, (short)3, (short)5, (short)6, (short)0, (short)1, (short)1, (short)1,
(short)2, (short)3, (short)3, (short)4, (short)3, (short)4, (short)2, (short)1,
(short)2, (short)1, (short)2, (short)3, (short)5, (short)1, (short)2, (short)2,
(short)3, (short)2, (short)3, (short)2, (short)3, (short)2, (short)3, (short)1,
(short)1, (short)1, (short)1, (short)2, (short)1, (short)1, (short)0, (short)1,
(short)6, (short)1, (short)3, (short)1, (short)1, (short)4, (short)4, (short)1,
(short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1,
(short)1, (short)1, (short)1, (short)1, (short)3, (short)2, (short)4, (short)1,
(short)1, (short)1, (short)1, (short)1, (short)4, (short)6, (short)6, (short)3,
(short)3, (short)0, (short)3, (short)3, (short)1, (short)1, (short)1, (short)1,
(short)2, (short)1, (short)1, (short)3, (short)1, (short)3, (short)3, (short)2,
(short)3, (short)2, (short)3, (short)1, (short)1, (short)3, (short)1, (short)2,
(short)3, (short)4, (short)6, (short)3, (short)5, (short)2, (short)1, (short)2,
(short)3, (short)3, (short)1, (short)3, (short)5, (short)1, (short)2, (short)1,
(short)3, (short)3, (short)4, (short)4, (short)3, (short)5, (short)4, (short)4,
(short)4, (short)2, (short)2, (short)3, (short)3, (short)4, (short)1, (short)1,
(short)4, (short)4, (short)1, (short)0, (short)1, (short)3, (short)1, (short)1,
(short)2, (short)1, (short)3, (short)3, (short)1, (short)3, (short)2, (short)3,
(short)1, (short)3, (short)1, (short)3, (short)3, (short)5, (short)1, (short)3,
(short)2, (short)1, (short)2, (short)1, (short)1, (short)3, (short)1, (short)2,
(short)3, (short)4, (short)1, (short)2, (short)3, (short)4, (short)2, (short)1,
(short)2, (short)3, (short)2, (short)1, (short)2, (short)1, (short)2, (short)3,
(short)3, (short)1, (short)3, (short)1, (short)1, (short)2, (short)3, (short)2,
(short)3, (short)3, (short)4, (short)2, (short)4, (short)3, (short)3, (short)5,
(short)4, (short)4, (short)4, (short)2, (short)1, (short)1, (short)1, (short)1,
(short)1, (short)1, (short)2, (short)2, (short)3, (short)1, (short)2, (short)2,
(short)3, (short)1, (short)2, (short)1, (short)1, (short)1, (short)5, (short)7,
(short)7, (short)6, (short)0, (short)3, (short)4, (short)5, (short)6, (short)7,
(short)5, (short)7, (short)6, (short)7, (short)7, (short)8, (short)7, (short)8,
(short)8, (short)9, (short)6, (short)7, (short)7, (short)8, (short)3, (short)2,
(short)2, (short)2, (short)3, (short)2, (short)4, (short)5, (short)1, (short)3,
(short)1, (short)2, (short)1, (short)1, (short)1, (short)1, (short)5, (short)4,
(short)4, (short)5, (short)2, (short)2, (short)0, (short)1, (short)1, (short)3,
(short)1, (short)2, (short)1, (short)1, (short)3, (short)1, (short)3, (short)1,
(short)3, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1,
(short)1, (short)1, (short)1, (short)1, (short)1, (short)5, (short)1, (short)1,
(short)3, (short)1, (short)3, (short)1, (short)3, (short)1, (short)3, (short)1,
(short)3, (short)1, (short)3, (short)3, (short)1, (short)3, (short)3, (short)3,
(short)3, (short)1, (short)3, (short)3, (short)1, (short)3, (short)3, (short)1,
(short)3, (short)3, (short)3, (short)1, (short)4, (short)1, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)4, (short)2, (short)2, (short)4,
(short)7, (short)7, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1,
(short)1, (short)1, (short)1, (short)1, (short)4, (short)3, (short)4, (short)3,
(short)3, (short)3, (short)3, (short)2, (short)2, (short)6, (short)7, (short)3,
(short)4, (short)5, (short)9, (short)2, (short)4, (short)4, (short)1, (short)1,
(short)1, (short)3, (short)2, (short)5, (short)4, (short)4, (short)5, (short)1,
(short)1, (short)3, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1};
static short Cyc_yydefact[ 717u]={ (short)0, (short)36, (short)37, (short)38,
(short)39, (short)41, (short)52, (short)54, (short)55, (short)56, (short)57,
(short)58, (short)59, (short)60, (short)61, (short)70, (short)71, (short)72,
(short)81, (short)82, (short)43, (short)0, (short)0, (short)42, (short)0,
(short)0, (short)0, (short)379, (short)64, (short)380, (short)78, (short)0,
(short)53, (short)0, (short)131, (short)132, (short)135, (short)1, (short)2,
(short)11, (short)0, (short)0, (short)12, (short)0, (short)43, (short)43,
(short)43, (short)62, (short)0, (short)63, (short)0, (short)114, (short)0,
(short)136, (short)116, (short)40, (short)0, (short)34, (short)44, (short)0,
(short)78, (short)257, (short)253, (short)256, (short)255, (short)0, (short)251,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)258, (short)19, (short)21,
(short)106, (short)0, (short)0, (short)66, (short)0, (short)0, (short)0, (short)0,
(short)3, (short)0, (short)0, (short)0, (short)0, (short)23, (short)0, (short)86,
(short)87, (short)89, (short)28, (short)30, (short)32, (short)78, (short)0,
(short)78, (short)39, (short)0, (short)26, (short)0, (short)0, (short)13,
(short)0, (short)0, (short)0, (short)126, (short)115, (short)137, (short)139,
(short)127, (short)0, (short)0, (short)35, (short)0, (short)107, (short)111,
(short)104, (short)0, (short)265, (short)264, (short)254, (short)263, (short)0,
(short)0, (short)0, (short)68, (short)69, (short)65, (short)0, (short)43,
(short)43, (short)176, (short)178, (short)182, (short)0, (short)117, (short)158,
(short)0, (short)155, (short)0, (short)378, (short)0, (short)0, (short)0,
(short)0, (short)335, (short)336, (short)337, (short)338, (short)339, (short)340,
(short)0, (short)0, (short)0, (short)375, (short)365, (short)376, (short)377,
(short)0, (short)0, (short)0, (short)0, (short)343, (short)0, (short)341,
(short)342, (short)0, (short)276, (short)289, (short)292, (short)294, (short)296,
(short)298, (short)300, (short)302, (short)305, (short)310, (short)313, (short)316,
(short)320, (short)0, (short)0, (short)322, (short)344, (short)364, (short)363,
(short)0, (short)4, (short)0, (short)6, (short)0, (short)24, (short)0, (short)0,
(short)0, (short)14, (short)29, (short)31, (short)33, (short)77, (short)0,
(short)83, (short)84, (short)0, (short)76, (short)40, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)379, (short)210, (short)212, (short)0,
(short)218, (short)216, (short)0, (short)217, (short)201, (short)202, (short)203,
(short)0, (short)214, (short)204, (short)205, (short)206, (short)0, (short)274,
(short)27, (short)15, (short)89, (short)161, (short)0, (short)121, (short)0,
(short)142, (short)0, (short)160, (short)0, (short)118, (short)0, (short)0,
(short)140, (short)129, (short)128, (short)0, (short)0, (short)102, (short)108,
(short)0, (short)0, (short)0, (short)0, (short)252, (short)267, (short)0,
(short)266, (short)0, (short)0, (short)265, (short)0, (short)151, (short)179,
(short)0, (short)92, (short)94, (short)0, (short)0, (short)184, (short)177,
(short)185, (short)0, (short)80, (short)0, (short)79, (short)0, (short)157,
(short)184, (short)159, (short)67, (short)0, (short)0, (short)330, (short)0,
(short)331, (short)360, (short)0, (short)0, (short)0, (short)0, (short)323,
(short)324, (short)0, (short)0, (short)0, (short)0, (short)326, (short)320,
(short)327, (short)325, (short)133, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)279,
(short)280, (short)281, (short)282, (short)283, (short)284, (short)285, (short)286,
(short)287, (short)288, (short)278, (short)0, (short)0, (short)328, (short)0,
(short)352, (short)353, (short)0, (short)0, (short)0, (short)367, (short)0,
(short)0, (short)134, (short)20, (short)9, (short)22, (short)9, (short)88,
(short)0, (short)90, (short)163, (short)16, (short)0, (short)73, (short)85,
(short)0, (short)0, (short)96, (short)97, (short)99, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)244, (short)245, (short)246,
(short)0, (short)0, (short)248, (short)0, (short)207, (short)208, (short)0,
(short)0, (short)89, (short)213, (short)215, (short)211, (short)0, (short)149,
(short)0, (short)0, (short)145, (short)120, (short)0, (short)0, (short)123,
(short)0, (short)125, (short)124, (short)119, (short)138, (short)141, (short)130,
(short)9, (short)49, (short)48, (short)0, (short)46, (short)110, (short)109,
(short)291, (short)112, (short)0, (short)0, (short)260, (short)0, (short)25,
(short)261, (short)0, (short)0, (short)0, (short)0, (short)172, (short)269,
(short)272, (short)0, (short)271, (short)0, (short)105, (short)0, (short)0,
(short)180, (short)93, (short)95, (short)0, (short)192, (short)0, (short)0,
(short)188, (short)0, (short)186, (short)0, (short)0, (short)0, (short)200,
(short)181, (short)183, (short)156, (short)0, (short)0, (short)0, (short)356,
(short)167, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)366, (short)373, (short)0, (short)372, (short)293, (short)0,
(short)295, (short)297, (short)299, (short)301, (short)303, (short)304, (short)308,
(short)309, (short)306, (short)307, (short)311, (short)312, (short)314, (short)315,
(short)317, (short)318, (short)319, (short)277, (short)0, (short)350, (short)351,
(short)346, (short)0, (short)0, (short)348, (short)349, (short)0, (short)0,
(short)10, (short)5, (short)7, (short)164, (short)0, (short)0, (short)100,
(short)91, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)243, (short)247, (short)0, (short)0,
(short)0, (short)209, (short)0, (short)17, (short)275, (short)0, (short)147,
(short)0, (short)122, (short)0, (short)144, (short)143, (short)162, (short)8,
(short)0, (short)0, (short)0, (short)0, (short)103, (short)268, (short)0,
(short)175, (short)270, (short)171, (short)173, (short)262, (short)0, (short)259,
(short)153, (short)152, (short)0, (short)194, (short)187, (short)190, (short)0,
(short)195, (short)0, (short)0, (short)189, (short)0, (short)329, (short)0,
(short)0, (short)357, (short)0, (short)168, (short)361, (short)362, (short)0,
(short)0, (short)0, (short)0, (short)321, (short)369, (short)0, (short)0,
(short)332, (short)347, (short)345, (short)370, (short)0, (short)0, (short)165,
(short)0, (short)75, (short)98, (short)101, (short)74, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)223,
(short)249, (short)0, (short)18, (short)150, (short)148, (short)146, (short)0,
(short)0, (short)45, (short)47, (short)113, (short)174, (short)273, (short)0,
(short)193, (short)0, (short)197, (short)199, (short)198, (short)191, (short)0,
(short)358, (short)169, (short)0, (short)0, (short)0, (short)371, (short)0,
(short)374, (short)290, (short)368, (short)166, (short)219, (short)223, (short)229,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)250, (short)50, (short)51,
(short)154, (short)196, (short)0, (short)170, (short)0, (short)0, (short)354,
(short)0, (short)0, (short)0, (short)0, (short)231, (short)0, (short)0, (short)0,
(short)239, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)222, (short)0, (short)333, (short)334, (short)355, (short)220,
(short)221, (short)230, (short)232, (short)233, (short)0, (short)241, (short)240,
(short)0, (short)235, (short)0, (short)0, (short)0, (short)0, (short)223,
(short)224, (short)0, (short)234, (short)242, (short)236, (short)237, (short)0,
(short)0, (short)223, (short)225, (short)359, (short)238, (short)223, (short)226,
(short)223, (short)227, (short)228, (short)0, (short)0, (short)0}; static short
Cyc_yydefgoto[ 102u]={ (short)714, (short)503, (short)504, (short)38, (short)39,
(short)226, (short)40, (short)357, (short)41, (short)359, (short)42, (short)197,
(short)43, (short)44, (short)57, (short)58, (short)414, (short)415, (short)45,
(short)130, (short)46, (short)47, (short)78, (short)48, (short)203, (short)204,
(short)89, (short)90, (short)91, (short)205, (short)139, (short)369, (short)370,
(short)371, (short)49, (short)116, (short)117, (short)50, (short)51, (short)52,
(short)53, (short)112, (short)256, (short)444, (short)398, (short)399, (short)275,
(short)247, (short)141, (short)248, (short)249, (short)460, (short)461, (short)462,
(short)429, (short)430, (short)135, (short)136, (short)137, (short)445, (short)282,
(short)229, (short)230, (short)231, (short)232, (short)233, (short)234, (short)235,
(short)649, (short)236, (short)237, (short)266, (short)267, (short)268, (short)432,
(short)433, (short)434, (short)238, (short)239, (short)343, (short)170, (short)419,
(short)171, (short)172, (short)173, (short)174, (short)175, (short)176, (short)177,
(short)178, (short)179, (short)180, (short)181, (short)182, (short)183, (short)184,
(short)185, (short)186, (short)471, (short)472, (short)187, (short)188}; static
short Cyc_yypact[ 717u]={ (short)774, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)68, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, (short)12, (short)49, (short)113, - (short)-32768,
(short)162, (short)29, (short)162, - (short)-32768, (short)72, - (short)-32768,
(short)128, (short)441, - (short)-32768, (short)79, (short)98, (short)127, -
(short)-32768, - (short)-32768, (short)774, - (short)-32768, - (short)53,
(short)118, - (short)-32768, (short)281, (short)12, (short)12, (short)12, -
(short)-32768, (short)311, - (short)-32768, (short)1837, (short)248, (short)148,
(short)27, - (short)-32768, (short)182, (short)192, (short)1886, - (short)-32768,
(short)162, (short)128, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)-32768, (short)113, - (short)-32768, (short)219, (short)194, (short)259,
(short)113, (short)231, (short)15, - (short)-32768, - (short)-32768, (short)255,
(short)125, (short)2073, - (short)-32768, (short)278, (short)2087, (short)4028,
(short)4028, - (short)-32768, (short)774, (short)774, (short)774, (short)774, -
(short)-32768, (short)290, (short)301, - (short)-32768, (short)1788, (short)1886,
(short)1886, (short)1886, (short)128, (short)2087, (short)128, (short)335,
(short)1068, - (short)-32768, (short)1837, (short)281, - (short)-32768, (short)1941,
(short)2073, (short)2600, - (short)-32768, (short)248, (short)346, - (short)-32768,
(short)74, (short)774, (short)329, - (short)-32768, (short)340, (short)165,
(short)208, (short)366, (short)376, (short)113, - (short)-32768, - (short)-32768,
- (short)-32768, (short)4028, - (short)15, (short)162, - (short)-32768, -
(short)-32768, - (short)-32768, (short)173, (short)12, (short)12, (short)223, -
(short)-32768, (short)331, - (short)26, - (short)-32768, (short)229, (short)150,
- (short)-32768, (short)4112, - (short)-32768, (short)4196, (short)51, (short)383,
(short)389, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, (short)392, (short)4196, (short)4196, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)1411,
(short)394, (short)4028, (short)4028, - (short)-32768, (short)4028, - (short)-32768,
- (short)-32768, (short)413, - (short)-32768, - (short)44, (short)446, (short)403,
(short)406, (short)410, (short)295, (short)75, (short)352, (short)119, (short)40,
- (short)-32768, (short)422, (short)433, (short)4028, (short)262, - (short)-32768,
- (short)-32768, (short)52, (short)444, - (short)-32768, (short)471, - (short)-32768,
(short)472, - (short)-32768, (short)441, (short)2684, (short)1837, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, (short)443, (short)473,
(short)2087, - (short)-32768, (short)323, (short)457, - (short)-32768, (short)474,
(short)475, (short)476, (short)1962, (short)477, (short)491, (short)483, (short)485,
(short)2768, (short)1962, (short)242, (short)1962, (short)1962, (short)479, -
(short)-32768, - (short)-32768, (short)482, - (short)-32768, - (short)-32768,
(short)281, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
(short)486, (short)1184, - (short)-32768, - (short)-32768, - (short)-32768,
(short)204, - (short)-32768, - (short)-32768, - (short)-32768, (short)484, -
(short)-32768, (short)77, - (short)-32768, (short)492, (short)215, (short)493,
(short)490, (short)4, - (short)-32768, (short)488, (short)125, - (short)-32768,
- (short)-32768, (short)74, (short)497, (short)18, - (short)-32768, (short)162,
(short)162, (short)4028, (short)498, (short)162, - (short)-32768, - (short)-32768,
(short)499, (short)500, (short)265, (short)658, (short)113, (short)503, -
(short)11, - (short)-32768, (short)506, (short)2087, (short)2087, (short)1682,
(short)2852, (short)102, - (short)-32768, (short)280, (short)77, - (short)-32768,
(short)2073, - (short)-32768, (short)1580, - (short)-32768, (short)90, - (short)-32768,
- (short)-32768, (short)2087, (short)1411, - (short)-32768, (short)1411, -
(short)-32768, - (short)-32768, (short)2096, (short)4028, (short)1638, (short)566,
- (short)-32768, - (short)-32768, (short)1184, (short)505, (short)325, (short)4028,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
(short)4028, (short)4028, (short)4028, (short)4028, (short)4028, (short)4028,
(short)4028, (short)4028, (short)4028, (short)4028, (short)4028, (short)4028,
(short)4028, (short)4028, (short)4028, (short)4028, (short)4028, (short)4028,
(short)4028, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, (short)4028, (short)4028, - (short)-32768,
(short)159, - (short)-32768, - (short)-32768, (short)2936, (short)4028, (short)163,
- (short)-32768, (short)2516, (short)502, - (short)-32768, - (short)-32768,
(short)774, - (short)-32768, (short)774, - (short)-32768, (short)2180, - (short)-32768,
- (short)-32768, - (short)-32768, (short)2087, - (short)-32768, - (short)-32768,
(short)4028, (short)512, (short)507, - (short)-32768, (short)508, (short)2087,
(short)4028, (short)4028, (short)4028, (short)578, (short)1300, (short)516, -
(short)-32768, - (short)-32768, - (short)-32768, (short)267, (short)569, -
(short)-32768, (short)3020, - (short)-32768, - (short)-32768, (short)1962,
(short)1522, (short)1788, - (short)-32768, - (short)-32768, - (short)-32768,
(short)4028, (short)530, (short)286, (short)517, (short)504, - (short)-32768,
(short)77, (short)1466, - (short)-32768, (short)532, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)774,
- (short)-32768, (short)520, (short)521, (short)519, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, (short)2087, (short)525, - (short)-32768,
(short)113, - (short)-32768, - (short)-32768, (short)4028, (short)537, (short)113,
(short)123, - (short)-32768, - (short)-32768, - (short)-32768, (short)527,
(short)524, (short)534, - (short)-32768, (short)125, (short)539, - (short)-32768,
- (short)-32768, - (short)-32768, (short)77, - (short)-32768, (short)538,
(short)541, - (short)-32768, (short)531, (short)280, (short)714, (short)2073,
(short)3104, - (short)-32768, - (short)-32768, (short)331, - (short)-32768,
(short)544, (short)546, (short)561, - (short)-32768, - (short)-32768, (short)343,
(short)2684, (short)351, (short)548, (short)441, (short)551, (short)554, (short)3188,
- (short)-32768, - (short)-32768, (short)552, (short)553, (short)446, (short)89,
(short)403, (short)406, (short)410, (short)295, (short)75, (short)75, (short)352,
(short)352, (short)352, (short)352, (short)119, (short)119, (short)40, (short)40,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)558,
- (short)-32768, - (short)-32768, - (short)-32768, (short)563, (short)164, -
(short)-32768, - (short)-32768, (short)345, (short)2073, - (short)-32768, -
(short)-32768, - (short)-32768, - (short)-32768, (short)348, (short)564, -
(short)-32768, - (short)-32768, (short)323, (short)4028, (short)567, (short)364,
(short)370, (short)386, (short)568, (short)3272, (short)3356, (short)293, -
(short)-32768, - (short)-32768, (short)575, (short)570, (short)572, - (short)-32768,
(short)1837, - (short)-32768, - (short)-32768, (short)125, - (short)-32768,
(short)576, - (short)-32768, (short)77, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)-32768, (short)425, (short)574, (short)18, (short)414, -
(short)-32768, - (short)-32768, (short)573, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)-32768, - (short)-32768, (short)805, - (short)-32768,
(short)579, - (short)-32768, (short)585, - (short)-32768, - (short)-32768, -
(short)-32768, (short)77, - (short)-32768, (short)588, (short)23, - (short)-32768,
(short)582, (short)595, (short)595, (short)571, - (short)-32768, (short)2264, -
(short)-32768, - (short)-32768, - (short)-32768, (short)1837, (short)1355,
(short)592, (short)2516, - (short)-32768, - (short)-32768, (short)4028, (short)4028,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)2516,
(short)349, - (short)-32768, (short)2348, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)-32768, (short)1962, (short)597, (short)1962, (short)4028,
(short)3440, (short)298, (short)3524, (short)318, (short)3608, (short)496, -
(short)-32768, (short)599, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)-32768, (short)596, (short)600, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)-32768, - (short)-32768, (short)539, - (short)-32768,
(short)601, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
(short)4028, - (short)-32768, - (short)-32768, (short)2684, (short)602, (short)603,
- (short)-32768, (short)356, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, (short)666, (short)496, - (short)-32768, (short)428, (short)1962,
(short)435, (short)3692, (short)1962, (short)442, (short)3776, (short)3860,
(short)319, (short)113, (short)593, (short)608, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, (short)175, - (short)-32768,
(short)606, (short)611, - (short)-32768, (short)2432, (short)1962, (short)615,
(short)613, - (short)-32768, (short)1962, (short)1962, (short)445, - (short)-32768,
(short)1962, (short)1962, (short)448, (short)1962, (short)456, (short)3944, -
(short)3, (short)1184, - (short)-32768, (short)4028, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, (short)1962, - (short)-32768, - (short)-32768, (short)1962, -
(short)-32768, (short)1962, (short)1962, (short)458, (short)4028, (short)952, -
(short)-32768, (short)382, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)-32768, (short)1962, (short)189, (short)496, - (short)-32768, - (short)-32768,
- (short)-32768, (short)952, - (short)-32768, (short)496, - (short)-32768, -
(short)-32768, (short)710, (short)711, - (short)-32768}; static short Cyc_yypgoto[
102u]={ - (short)-32768, (short)82, - (short)332, - (short)-32768, (short)415, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)46, - (short)49, - (short)50, - (short)-32768, (short)39, - (short)43,
(short)172, - (short)-32768, (short)347, - (short)230, (short)179, - (short)-32768,
(short)45, - (short)-32768, - (short)151, - (short)-32768, - (short)-32768, -
(short)-32768, (short)523, (short)515, (short)13, - (short)-32768, - (short)-32768,
(short)203, - (short)-32768, - (short)103, - (short)-32768, - (short)5, -
(short)34, - (short)35, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)96, - (short)381, (short)438, - (short)375, - (short)66, - (short)273, -
(short)-32768, - (short)-32768, - (short)190, - (short)340, - (short)268, -
(short)-32768, (short)287, - (short)131, (short)451, - (short)95, (short)177, -
(short)221, (short)14, - (short)-32768, - (short)-32768, - (short)45, - (short)231,
- (short)-32768, - (short)-32768, - (short)587, - (short)-32768, - (short)-32768,
- (short)10, (short)467, - (short)-32768, (short)188, - (short)-32768, - (short)-32768,
- (short)110, (short)7, - (short)-32768, - (short)233, - (short)352, - (short)-32768,
(short)427, (short)431, (short)432, (short)426, (short)437, (short)238, (short)181,
(short)234, (short)235, - (short)36, (short)510, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)316, - (short)-32768, - (short)-32768,
(short)10}; static short Cyc_yytable[ 4315u]={ (short)103, (short)102, (short)428,
(short)393, (short)101, (short)104, (short)362, (short)115, (short)108, (short)246,
(short)54, (short)250, (short)71, (short)501, (short)140, (short)269, (short)509,
(short)55, (short)109, (short)455, (short)535, (short)507, (short)532, (short)408,
(short)272, (short)313, (short)79, (short)505, (short)493, (short)418, (short)305,
(short)60, (short)72, (short)497, (short)73, (short)412, (short)75, (short)284,
(short)92, (short)84, (short)85, (short)54, (short)103, (short)199, (short)200,
(short)201, (short)101, (short)198, (short)54, (short)662, (short)228, (short)306,
(short)103, (short)54, (short)227, (short)120, (short)240, (short)241, (short)98,
(short)448, (short)124, (short)555, (short)54, (short)554, (short)314, (short)695,
(short)108, (short)405, (short)448, (short)118, (short)525, (short)437, (short)285,
(short)467, (short)545, (short)72, (short)286, (short)255, (short)270, (short)538,
(short)72, (short)271, (short)37, (short)93, (short)94, (short)95, (short)620,
(short)438, (short)169, (short)189, (short)134, (short)15, (short)16, (short)17,
(short)54, (short)54, (short)54, (short)54, (short)242, (short)280, (short)696,
(short)413, (short)285, (short)56, (short)289, (short)119, (short)406, (short)383,
- (short)78, (short)706, (short)206, - (short)78, (short)74, (short)54, (short)252,
(short)110, (short)77, (short)126, (short)710, (short)134, (short)83, (short)285,
(short)712, (short)54, (short)713, (short)621, (short)111, (short)308, (short)310,
(short)455, (short)311, (short)72, (short)27, (short)352, (short)288, (short)418,
(short)297, (short)118, (short)29, (short)321, (short)322, (short)202, (short)59,
(short)207, (short)298, (short)353, (short)329, (short)103, (short)345, (short)54,
(short)61, (short)240, (short)364, (short)608, (short)76, (short)330, (short)331,
(short)416, (short)417, (short)354, (short)591, (short)421, (short)456, (short)263,
(short)457, (short)396, (short)190, (short)191, (short)192, (short)193, (short)397,
(short)276, (short)277, (short)27, (short)134, (short)80, (short)323, (short)324,
(short)618, (short)29, (short)34, (short)35, (short)36, (short)306, (short)228,
(short)306, (short)287, (short)395, (short)227, (short)463, (short)242, (short)81,
(short)580, (short)418, (short)279, (short)257, (short)27, (short)62, (short)278,
(short)63, (short)64, (short)372, (short)29, (short)363, (short)474, (short)54,
(short)279, (short)553, (short)128, (short)65, (short)86, (short)87, (short)66,
(short)67, (short)508, (short)129, (short)54, (short)206, (short)548, (short)68,
(short)82, (short)410, (short)513, (short)391, (short)69, (short)70, (short)377,
(short)426, (short)327, (short)77, (short)328, (short)27, (short)384, (short)427,
(short)387, (short)388, (short)630, (short)29, (short)54, (short)452, (short)498,
(short)653, (short)494, (short)280, (short)31, (short)27, (short)499, (short)291,
(short)292, (short)495, (short)465, (short)29, (short)289, (short)500, (short)228,
(short)109, (short)133, (short)260, (short)227, (short)133, (short)431, (short)273,
(short)395, (short)261, (short)514, (short)515, (short)516, (short)274, (short)520,
(short)583, (short)118, (short)118, (short)570, (short)395, (short)118, (short)113,
(short)133, (short)122, (short)678, (short)418, (short)72, (short)72, (short)79,
(short)428, (short)133, (short)133, (short)447, (short)395, (short)114, (short)440,
(short)441, (short)254, (short)709, (short)489, (short)490, (short)491, (short)394,
(short)54, (short)134, (short)54, (short)606, (short)626, (short)395, (short)262,
- (short)78, (short)363, (short)134, (short)401, (short)134, (short)77, (short)54,
(short)281, (short)27, (short)402, (short)470, (short)121, (short)290, (short)626,
(short)29, (short)278, (short)626, (short)346, (short)347, (short)348, (short)140,
(short)287, (short)125, (short)279, (short)103, (short)34, (short)35, (short)36,
(short)519, (short)279, (short)385, (short)34, (short)35, (short)36, (short)386,
(short)56, (short)133, (short)103, (short)527, (short)123, (short)105, (short)101,
(short)528, (short)632, (short)127, (short)106, (short)492, (short)470, (short)107,
(short)561, (short)542, (short)562, (short)470, (short)424, (short)349, (short)522,
(short)363, (short)319, (short)320, (short)395, (short)27, (short)395, (short)350,
(short)54, (short)363, (short)54, (short)29, (short)56, (short)351, (short)88,
(short)273, (short)138, (short)449, (short)31, (short)206, (short)625, (short)531,
(short)450, (short)194, (short)133, (short)451, (short)601, (short)206, (short)34,
(short)35, (short)36, (short)641, (short)395, (short)626, (short)470, (short)27,
(short)625, (short)395, (short)470, (short)625, (short)195, (short)29, (short)96,
(short)529, (short)526, (short)97, (short)452, (short)27, (short)586, (short)598,
(short)600, (short)644, (short)674, (short)29, (short)544, (short)325, (short)326,
(short)395, (short)395, (short)547, (short)31, (short)208, (short)54, (short)469,
(short)395, (short)132, (short)258, (short)368, (short)132, (short)253, (short)34,
(short)35, (short)36, (short)577, (short)72, (short)259, (short)409, (short)656,
(short)568, (short)72, (short)584, (short)283, (short)569, (short)587, (short)585,
(short)132, (short)697, (short)588, (short)285, (short)571, (short)395, (short)659,
(short)633, (short)132, (short)132, (short)660, (short)133, (short)133, (short)133,
(short)564, (short)264, (short)573, (short)593, (short)395, (short)134, (short)133,
(short)705, (short)133, (short)594, (short)395, (short)363, (short)625, (short)133,
(short)133, (short)265, (short)133, (short)54, (short)707, (short)103, (short)711,
(short)299, (short)395, (short)240, (short)605, (short)595, (short)395, (short)300,
(short)638, (short)640, (short)301, (short)643, (short)307, (short)646, (short)332,
(short)333, (short)334, (short)335, (short)336, (short)337, (short)338, (short)339,
(short)340, (short)341, (short)481, (short)482, (short)483, (short)484, (short)372,
(short)312, (short)132, (short)609, (short)610, (short)613, (short)292, (short)655,
(short)315, (short)134, (short)316, (short)342, (short)647, (short)648, (short)317,
(short)54, (short)318, (short)103, (short)27, (short)663, (short)395, (short)101,
(short)198, (short)344, (short)29, (short)667, (short)665, (short)395, (short)671,
(short)673, (short)365, (short)31, (short)355, (short)669, (short)395, (short)431,
(short)687, (short)395, (short)133, (short)690, (short)395, (short)34, (short)35,
(short)36, (short)373, (short)132, (short)133, (short)692, (short)395, (short)703,
(short)395, (short)479, (short)480, (short)485, (short)486, (short)72, (short)487,
(short)488, (short)694, (short)356, (short)358, (short)366, (short)698, (short)133,
(short)374, (short)375, (short)376, (short)378, (short)379, (short)380, (short)363,
(short)381, (short)390, (short)196, (short)392, (short)133, (short)389, (short)363,
(short)628, (short)704, (short)631, (short)627, (short)404, (short)400, (short)403,
(short)411, (short)363, (short)407, (short)420, (short)363, (short)422, (short)436,
(short)423, (short)133, (short)439, (short)466, (short)468, (short)502, (short)510,
(short)511, (short)517, (short)635, (short)521, (short)637, (short)523, (short)512,
(short)530, (short)534, (short)533, (short)537, (short)539, (short)541, (short)540,
(short)543, (short)546, (short)550, (short)551, (short)132, (short)132, (short)132,
(short)228, (short)273, (short)133, (short)133, (short)227, (short)552, (short)132,
(short)363, (short)132, (short)556, (short)558, (short)675, (short)557, (short)132,
(short)132, (short)565, (short)132, (short)566, (short)567, (short)572, (short)228,
(short)574, (short)575, (short)578, (short)227, (short)579, (short)294, (short)664,
(short)296, (short)581, (short)668, (short)72, (short)589, (short)228, (short)582,
(short)592, (short)603, (short)227, (short)596, (short)302, (short)303, (short)363,
(short)602, (short)604, (short)607, (short)611, (short)623, (short)309, (short)309,
(short)682, (short)309, (short)616, (short)614, (short)685, (short)686, (short)133,
(short)617, (short)688, (short)689, (short)619, (short)691, (short)622, (short)576,
(short)629, (short)636, (short)650, (short)661, (short)651, (short)309, (short)61,
(short)676, (short)652, (short)654, (short)657, (short)658, (short)699, (short)677,
(short)679, (short)700, (short)684, (short)701, (short)702, (short)680, (short)683,
(short)715, (short)716, (short)132, (short)612, (short)590, (short)464, (short)549,
(short)708, (short)360, (short)367, (short)132, (short)453, (short)6, (short)7,
(short)8, (short)9, (short)10, (short)11, (short)12, (short)13, (short)14,
(short)15, (short)16, (short)17, (short)18, (short)19, (short)454, (short)132,
(short)435, (short)615, (short)473, (short)27, (short)62, (short)477, (short)63,
(short)64, (short)475, (short)29, (short)476, (short)132, (short)21, (short)0,
(short)425, (short)133, (short)65, (short)478, (short)0, (short)66, (short)67,
(short)0, (short)0, (short)26, (short)426, (short)0, (short)68, (short)0,
(short)0, (short)132, (short)427, (short)69, (short)70, (short)0, (short)309,
(short)0, (short)0, (short)0, (short)0, (short)1, (short)2, (short)3, (short)4,
(short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)132, (short)132, (short)20, (short)0, (short)0, (short)0, (short)28,
(short)0, (short)30, (short)0, (short)559, (short)0, (short)0, (short)0, (short)21,
(short)560, (short)22, (short)32, (short)33, (short)0, (short)0, (short)23,
(short)0, (short)24, (short)25, (short)26, (short)0, (short)309, (short)0,
(short)309, (short)309, (short)309, (short)309, (short)309, (short)309, (short)309,
(short)309, (short)309, (short)309, (short)309, (short)309, (short)309, (short)309,
(short)309, (short)309, (short)309, (short)61, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)132, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)27, (short)0, (short)0, (short)0, (short)0,
(short)28, (short)29, (short)30, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)31, (short)0, (short)0, (short)32, (short)33, (short)0, (short)0, (short)0,
(short)309, (short)0, (short)34, (short)35, (short)36, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)27, (short)62, (short)0, (short)63, (short)64,
(short)0, (short)29, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)65, (short)0, (short)0, (short)66, (short)67, (short)0, (short)0, (short)0,
(short)426, (short)0, (short)68, (short)0, (short)0, (short)0, (short)427,
(short)69, (short)70, (short)0, (short)0, (short)0, (short)132, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)309, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)1, (short)2, (short)3, (short)99, (short)5, (short)6, (short)7, (short)8,
(short)9, (short)10, (short)11, (short)12, (short)13, (short)14, (short)15,
(short)16, (short)17, (short)18, (short)19, (short)647, (short)648, (short)20,
(short)209, (short)309, (short)210, (short)211, (short)212, (short)213, (short)214,
(short)215, (short)216, (short)217, (short)142, (short)21, (short)143, (short)22,
(short)144, (short)218, (short)0, (short)145, (short)23, (short)219, (short)0,
(short)0, (short)26, (short)146, (short)147, (short)220, (short)221, (short)148,
(short)149, (short)150, (short)151, (short)152, (short)153, (short)154, (short)0,
(short)155, (short)156, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)309, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)222, (short)157, (short)158, (short)159, (short)160, (short)28, (short)29,
(short)30, (short)0, (short)223, (short)100, (short)0, (short)0, (short)161,
(short)0, (short)0, (short)32, (short)225, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)163, (short)0, (short)0, (short)164, (short)0, (short)165,
(short)166, (short)0, (short)0, (short)0, (short)0, (short)167, (short)168,
(short)1, (short)2, (short)3, (short)99, (short)5, (short)6, (short)7, (short)8,
(short)9, (short)10, (short)11, (short)12, (short)13, (short)14, (short)15,
(short)16, (short)17, (short)18, (short)19, (short)309, (short)0, (short)20,
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
(short)518, (short)0, (short)0, (short)0, (short)161, (short)0, (short)0,
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
(short)160, (short)28, (short)29, (short)30, (short)21, (short)0, (short)304,
(short)0, (short)0, (short)161, (short)0, (short)0, (short)32, (short)225,
(short)0, (short)26, (short)0, (short)0, (short)0, (short)163, (short)0, (short)0,
(short)164, (short)0, (short)165, (short)166, (short)0, (short)0, (short)0,
(short)0, (short)167, (short)168, (short)6, (short)7, (short)8, (short)9,
(short)10, (short)11, (short)12, (short)13, (short)14, (short)15, (short)16,
(short)17, (short)18, (short)19, (short)0, (short)0, (short)536, (short)0,
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
(short)442, (short)0, (short)21, (short)0, (short)287, (short)443, (short)0,
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
(short)30, (short)0, (short)442, (short)0, (short)0, (short)0, (short)278,
(short)443, (short)0, (short)32, (short)33, (short)0, (short)0, (short)0,
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
(short)21, (short)0, (short)0, (short)458, (short)0, (short)0, (short)0, (short)0,
(short)142, (short)0, (short)143, (short)26, (short)144, (short)0, (short)0,
(short)145, (short)0, (short)0, (short)0, (short)0, (short)0, (short)146,
(short)147, (short)0, (short)0, (short)148, (short)149, (short)150, (short)151,
(short)152, (short)153, (short)154, (short)0, (short)155, (short)156, (short)0,
(short)0, (short)0, (short)28, (short)0, (short)30, (short)0, (short)0, (short)131,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)32, (short)33, (short)0,
(short)28, (short)0, (short)30, (short)0, (short)27, (short)157, (short)158,
(short)159, (short)160, (short)0, (short)29, (short)32, (short)33, (short)0,
(short)361, (short)459, (short)0, (short)161, (short)0, (short)0, (short)0,
(short)162, (short)0, (short)0, (short)0, (short)426, (short)0, (short)163,
(short)0, (short)0, (short)164, (short)427, (short)165, (short)166, (short)0,
(short)0, (short)0, (short)0, (short)167, (short)168, (short)142, (short)0,
(short)143, (short)0, (short)144, (short)0, (short)0, (short)145, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)146, (short)147, (short)0,
(short)0, (short)148, (short)149, (short)150, (short)151, (short)152, (short)153,
(short)154, (short)0, (short)155, (short)156, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)27, (short)157, (short)158, (short)159, (short)160,
(short)0, (short)29, (short)0, (short)0, (short)0, (short)361, (short)506,
(short)0, (short)161, (short)0, (short)0, (short)0, (short)162, (short)0,
(short)0, (short)0, (short)426, (short)0, (short)163, (short)0, (short)0,
(short)164, (short)427, (short)165, (short)166, (short)0, (short)0, (short)0,
(short)0, (short)167, (short)168, (short)142, (short)0, (short)143, (short)0,
(short)144, (short)0, (short)0, (short)145, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)146, (short)147, (short)0, (short)0, (short)148,
(short)149, (short)150, (short)151, (short)152, (short)153, (short)154, (short)0,
(short)155, (short)156, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)27, (short)157, (short)158, (short)159, (short)160, (short)0, (short)29,
(short)0, (short)0, (short)0, (short)361, (short)624, (short)0, (short)161,
(short)0, (short)0, (short)0, (short)162, (short)0, (short)0, (short)0, (short)426,
(short)0, (short)163, (short)0, (short)0, (short)164, (short)427, (short)165,
(short)166, (short)0, (short)0, (short)0, (short)0, (short)167, (short)168,
(short)142, (short)0, (short)143, (short)0, (short)144, (short)0, (short)0,
(short)145, (short)0, (short)0, (short)0, (short)0, (short)0, (short)146,
(short)147, (short)0, (short)0, (short)148, (short)149, (short)150, (short)151,
(short)152, (short)153, (short)154, (short)0, (short)155, (short)156, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)27, (short)157, (short)158,
(short)159, (short)160, (short)0, (short)29, (short)0, (short)0, (short)0,
(short)361, (short)634, (short)0, (short)161, (short)0, (short)0, (short)0,
(short)162, (short)0, (short)0, (short)0, (short)426, (short)0, (short)163,
(short)0, (short)0, (short)164, (short)427, (short)165, (short)166, (short)0,
(short)0, (short)0, (short)0, (short)167, (short)168, (short)142, (short)0,
(short)143, (short)0, (short)144, (short)0, (short)0, (short)145, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)146, (short)147, (short)0,
(short)0, (short)148, (short)149, (short)150, (short)151, (short)152, (short)153,
(short)154, (short)0, (short)155, (short)156, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)27, (short)157, (short)158, (short)159, (short)160,
(short)0, (short)29, (short)0, (short)0, (short)0, (short)361, (short)681,
(short)0, (short)161, (short)0, (short)0, (short)0, (short)162, (short)0,
(short)0, (short)0, (short)426, (short)0, (short)163, (short)0, (short)0,
(short)164, (short)427, (short)165, (short)166, (short)0, (short)0, (short)0,
(short)0, (short)167, (short)168, (short)142, (short)0, (short)143, (short)0,
(short)144, (short)0, (short)0, (short)145, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)146, (short)147, (short)0, (short)0, (short)148,
(short)149, (short)150, (short)151, (short)152, (short)153, (short)154, (short)0,
(short)155, (short)156, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)27, (short)157, (short)158, (short)159, (short)160, (short)0, (short)29,
(short)0, (short)0, (short)0, (short)361, (short)0, (short)0, (short)161,
(short)0, (short)0, (short)0, (short)162, (short)0, (short)0, (short)0, (short)426,
(short)0, (short)163, (short)0, (short)0, (short)164, (short)427, (short)165,
(short)166, (short)0, (short)0, (short)0, (short)0, (short)167, (short)168,
(short)142, (short)0, (short)143, (short)0, (short)144, (short)0, (short)0,
(short)145, (short)0, (short)0, (short)0, (short)0, (short)0, (short)146,
(short)147, (short)0, (short)0, (short)148, (short)149, (short)150, (short)151,
(short)152, (short)153, (short)154, (short)0, (short)155, (short)156, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)27, (short)157, (short)158,
(short)159, (short)160, (short)0, (short)29, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)161, (short)0, (short)0, (short)0, (short)162,
(short)0, (short)0, (short)0, (short)0, (short)251, (short)163, (short)0,
(short)0, (short)164, (short)0, (short)165, (short)166, (short)0, (short)0,
(short)0, (short)0, (short)167, (short)168, (short)142, (short)0, (short)143,
(short)0, (short)144, (short)0, (short)0, (short)145, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)146, (short)147, (short)0, (short)0,
(short)148, (short)149, (short)150, (short)151, (short)152, (short)153, (short)154,
(short)0, (short)155, (short)156, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)27, (short)157, (short)158, (short)159, (short)160, (short)0,
(short)29, (short)0, (short)0, (short)0, (short)361, (short)0, (short)0, (short)161,
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
(short)159, (short)160, (short)0, (short)29, (short)0, (short)0, (short)382,
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
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)161, (short)0,
(short)0, (short)0, (short)162, (short)0, (short)0, (short)0, (short)0, (short)446,
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
(short)0, (short)161, (short)496, (short)0, (short)0, (short)162, (short)0,
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
(short)0, (short)0, (short)0, (short)161, (short)524, (short)0, (short)0,
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
(short)0, (short)0, (short)563, (short)163, (short)0, (short)0, (short)164,
(short)0, (short)165, (short)166, (short)0, (short)0, (short)0, (short)0,
(short)167, (short)168, (short)142, (short)0, (short)143, (short)0, (short)144,
(short)0, (short)0, (short)145, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)146, (short)147, (short)0, (short)0, (short)148, (short)149, (short)150,
(short)151, (short)152, (short)153, (short)154, (short)0, (short)155, (short)156,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)27, (short)157, (short)158,
(short)159, (short)160, (short)0, (short)29, (short)0, (short)0, (short)0,
(short)576, (short)0, (short)0, (short)161, (short)0, (short)0, (short)0,
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
(short)0, (short)29, (short)0, (short)0, (short)597, (short)0, (short)0, (short)0,
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
(short)159, (short)160, (short)0, (short)29, (short)0, (short)0, (short)599,
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
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)161, (short)639,
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
(short)0, (short)161, (short)642, (short)0, (short)0, (short)162, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)163, (short)0, (short)0, (short)164,
(short)0, (short)165, (short)166, (short)0, (short)0, (short)0, (short)0,
(short)167, (short)168, (short)142, (short)0, (short)143, (short)0, (short)144,
(short)0, (short)0, (short)145, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)146, (short)147, (short)0, (short)0, (short)148, (short)149, (short)150,
(short)151, (short)152, (short)153, (short)154, (short)0, (short)155, (short)156,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)27, (short)157, (short)158,
(short)159, (short)160, (short)0, (short)29, (short)0, (short)0, (short)645,
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
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)161, (short)666,
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
(short)0, (short)161, (short)670, (short)0, (short)0, (short)162, (short)0,
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
(short)0, (short)0, (short)0, (short)161, (short)672, (short)0, (short)0,
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
(short)161, (short)693, (short)0, (short)0, (short)162, (short)0, (short)0,
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
(short)144, (short)0, (short)0, (short)145, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)146, (short)147, (short)0, (short)0, (short)148,
(short)149, (short)150, (short)151, (short)152, (short)153, (short)154, (short)0,
(short)155, (short)156, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)27, (short)157, (short)158, (short)159, (short)160, (short)0, (short)29,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)293, (short)0,
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
(short)0, (short)295, (short)0, (short)0, (short)0, (short)162, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)163, (short)0, (short)0, (short)164,
(short)0, (short)165, (short)166, (short)0, (short)0, (short)0, (short)0,
(short)167, (short)168}; static short Cyc_yycheck[ 4315u]={ (short)50, (short)50,
(short)270, (short)234, (short)50, (short)50, (short)196, (short)57, (short)51,
(short)105, (short)0, (short)106, (short)22, (short)353, (short)80, (short)125,
(short)368, (short)85, (short)52, (short)292, (short)401, (short)361, (short)397,
(short)253, (short)127, (short)69, (short)31, (short)359, (short)344, (short)262,
(short)161, (short)21, (short)22, (short)349, (short)24, (short)17, (short)26,
(short)63, (short)43, (short)92, (short)93, (short)31, (short)92, (short)93,
(short)94, (short)95, (short)92, (short)92, (short)38, (short)636, (short)100,
(short)161, (short)102, (short)43, (short)100, (short)65, (short)102, (short)102,
(short)48, (short)280, (short)70, (short)442, (short)52, (short)438, (short)108,
(short)68, (short)109, (short)63, (short)289, (short)59, (short)386, (short)82,
(short)98, (short)304, (short)426, (short)65, (short)102, (short)112, (short)93,
(short)411, (short)70, (short)96, (short)0, (short)44, (short)45, (short)46,
(short)63, (short)98, (short)81, (short)82, (short)77, (short)17, (short)18,
(short)19, (short)84, (short)85, (short)86, (short)87, (short)103, (short)134,
(short)103, (short)83, (short)98, (short)91, (short)139, (short)60, (short)102,
(short)217, (short)93, (short)696, (short)97, (short)96, (short)83, (short)103,
(short)107, (short)88, (short)101, (short)72, (short)705, (short)106, (short)38,
(short)98, (short)709, (short)113, (short)711, (short)102, (short)99, (short)163,
(short)164, (short)402, (short)166, (short)121, (short)83, (short)81, (short)139,
(short)368, (short)85, (short)127, (short)89, (short)64, (short)65, (short)96,
(short)93, (short)98, (short)93, (short)93, (short)106, (short)197, (short)184,
(short)139, (short)37, (short)197, (short)197, (short)534, (short)82, (short)115,
(short)116, (short)260, (short)261, (short)107, (short)512, (short)264, (short)293,
(short)118, (short)295, (short)88, (short)84, (short)85, (short)86, (short)87,
(short)93, (short)132, (short)133, (short)83, (short)161, (short)96, (short)101,
(short)102, (short)559, (short)89, (short)106, (short)107, (short)108, (short)293,
(short)234, (short)295, (short)96, (short)98, (short)234, (short)299, (short)195,
(short)93, (short)103, (short)426, (short)104, (short)113, (short)83, (short)84,
(short)96, (short)86, (short)87, (short)206, (short)89, (short)196, (short)314,
(short)195, (short)104, (short)437, (short)83, (short)96, (short)92, (short)93,
(short)99, (short)100, (short)365, (short)90, (short)206, (short)204, (short)95,
(short)106, (short)93, (short)256, (short)373, (short)228, (short)111, (short)112,
(short)212, (short)104, (short)109, (short)101, (short)111, (short)83, (short)218,
(short)110, (short)220, (short)221, (short)576, (short)89, (short)228, (short)282,
(short)350, (short)616, (short)83, (short)278, (short)96, (short)83, (short)83,
(short)97, (short)98, (short)90, (short)300, (short)89, (short)287, (short)90,
(short)304, (short)289, (short)77, (short)92, (short)304, (short)80, (short)270,
(short)88, (short)98, (short)98, (short)374, (short)375, (short)376, (short)94,
(short)378, (short)105, (short)260, (short)261, (short)462, (short)98, (short)264,
(short)93, (short)97, (short)83, (short)103, (short)512, (short)270, (short)271,
(short)287, (short)551, (short)105, (short)106, (short)279, (short)98, (short)96,
(short)276, (short)277, (short)112, (short)103, (short)329, (short)330, (short)331,
(short)92, (short)287, (short)285, (short)289, (short)530, (short)569, (short)98,
(short)95, (short)96, (short)298, (short)293, (short)92, (short)295, (short)101,
(short)300, (short)134, (short)83, (short)98, (short)307, (short)96, (short)139,
(short)585, (short)89, (short)96, (short)588, (short)59, (short)60, (short)61,
(short)390, (short)96, (short)95, (short)104, (short)378, (short)106, (short)107,
(short)108, (short)378, (short)104, (short)92, (short)106, (short)107, (short)108,
(short)96, (short)91, (short)161, (short)391, (short)391, (short)84, (short)96,
(short)391, (short)391, (short)580, (short)93, (short)101, (short)343, (short)344,
(short)104, (short)449, (short)420, (short)450, (short)349, (short)92, (short)96,
(short)92, (short)353, (short)66, (short)67, (short)98, (short)83, (short)98,
(short)104, (short)357, (short)361, (short)359, (short)89, (short)91, (short)110,
(short)92, (short)88, (short)97, (short)96, (short)96, (short)365, (short)569,
(short)94, (short)101, (short)92, (short)204, (short)104, (short)92, (short)373,
(short)106, (short)107, (short)108, (short)92, (short)98, (short)660, (short)386,
(short)83, (short)585, (short)98, (short)390, (short)588, (short)98, (short)89,
(short)90, (short)395, (short)389, (short)93, (short)448, (short)83, (short)502,
(short)518, (short)519, (short)92, (short)92, (short)89, (short)423, (short)62,
(short)63, (short)98, (short)98, (short)428, (short)96, (short)85, (short)411,
(short)97, (short)98, (short)77, (short)96, (short)103, (short)80, (short)82,
(short)106, (short)107, (short)108, (short)468, (short)423, (short)94, (short)256,
(short)626, (short)94, (short)428, (short)94, (short)109, (short)98, (short)94,
(short)98, (short)97, (short)676, (short)98, (short)98, (short)97, (short)98,
(short)94, (short)102, (short)105, (short)106, (short)98, (short)276, (short)277,
(short)278, (short)451, (short)93, (short)465, (short)97, (short)98, (short)450,
(short)285, (short)696, (short)287, (short)97, (short)98, (short)462, (short)660,
(short)292, (short)293, (short)97, (short)295, (short)465, (short)94, (short)527,
(short)709, (short)96, (short)98, (short)527, (short)527, (short)97, (short)98,
(short)96, (short)596, (short)597, (short)96, (short)599, (short)96, (short)601,
(short)70, (short)71, (short)72, (short)73, (short)74, (short)75, (short)76,
(short)77, (short)78, (short)79, (short)321, (short)322, (short)323, (short)324,
(short)511, (short)94, (short)161, (short)84, (short)85, (short)97, (short)98,
(short)623, (short)68, (short)502, (short)113, (short)95, (short)22, (short)23,
(short)114, (short)511, (short)112, (short)573, (short)83, (short)97, (short)98,
(short)573, (short)573, (short)96, (short)89, (short)641, (short)97, (short)98,
(short)644, (short)645, (short)93, (short)96, (short)94, (short)97, (short)98,
(short)551, (short)97, (short)98, (short)365, (short)97, (short)98, (short)106,
(short)107, (short)108, (short)93, (short)204, (short)373, (short)97, (short)98,
(short)97, (short)98, (short)319, (short)320, (short)325, (short)326, (short)551,
(short)327, (short)328, (short)674, (short)94, (short)94, (short)94, (short)678,
(short)390, (short)96, (short)96, (short)96, (short)96, (short)83, (short)92,
(short)569, (short)92, (short)96, (short)95, (short)94, (short)402, (short)103,
(short)576, (short)574, (short)695, (short)579, (short)574, (short)98, (short)97,
(short)97, (short)94, (short)585, (short)105, (short)96, (short)588, (short)97,
(short)94, (short)98, (short)420, (short)94, (short)35, (short)97, (short)101,
(short)92, (short)98, (short)28, (short)593, (short)92, (short)595, (short)41,
(short)103, (short)82, (short)109, (short)97, (short)83, (short)96, (short)98,
(short)97, (short)94, (short)83, (short)94, (short)98, (short)276, (short)277,
(short)278, (short)676, (short)88, (short)449, (short)450, (short)676, (short)97,
(short)285, (short)626, (short)287, (short)97, (short)105, (short)647, (short)97,
(short)292, (short)293, (short)97, (short)295, (short)97, (short)83, (short)97,
(short)696, (short)96, (short)94, (short)97, (short)696, (short)98, (short)142,
(short)639, (short)144, (short)97, (short)642, (short)647, (short)94, (short)709,
(short)97, (short)94, (short)92, (short)709, (short)96, (short)155, (short)156,
(short)660, (short)93, (short)97, (short)94, (short)97, (short)101, (short)163,
(short)164, (short)661, (short)166, (short)98, (short)105, (short)665, (short)666,
(short)502, (short)97, (short)669, (short)670, (short)97, (short)672, (short)105,
(short)93, (short)97, (short)93, (short)92, (short)26, (short)97, (short)184,
(short)37, (short)103, (short)97, (short)97, (short)97, (short)97, (short)687,
(short)94, (short)97, (short)690, (short)92, (short)692, (short)693, (short)97,
(short)94, (short)0, (short)0, (short)365, (short)541, (short)511, (short)300,
(short)429, (short)703, (short)195, (short)204, (short)373, (short)283, (short)8,
(short)9, (short)10, (short)11, (short)12, (short)13, (short)14, (short)15,
(short)16, (short)17, (short)18, (short)19, (short)20, (short)21, (short)285,
(short)390, (short)271, (short)551, (short)313, (short)83, (short)84, (short)317,
(short)86, (short)87, (short)315, (short)89, (short)316, (short)402, (short)36,
- (short)1, (short)94, (short)574, (short)96, (short)318, - (short)1, (short)99,
(short)100, - (short)1, - (short)1, (short)47, (short)104, - (short)1, (short)106,
- (short)1, - (short)1, (short)420, (short)110, (short)111, (short)112, -
(short)1, (short)262, - (short)1, - (short)1, - (short)1, - (short)1, (short)3,
(short)4, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)20, (short)21, (short)449, (short)450, (short)24, - (short)1,
- (short)1, - (short)1, (short)88, - (short)1, (short)90, - (short)1, (short)92,
- (short)1, - (short)1, - (short)1, (short)36, (short)97, (short)38, (short)99,
(short)100, - (short)1, - (short)1, (short)43, - (short)1, (short)45, (short)46,
(short)47, - (short)1, (short)313, - (short)1, (short)315, (short)316, (short)317,
(short)318, (short)319, (short)320, (short)321, (short)322, (short)323, (short)324,
(short)325, (short)326, (short)327, (short)328, (short)329, (short)330, (short)331,
(short)37, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)502, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, (short)83, - (short)1, - (short)1, - (short)1, - (short)1,
(short)88, (short)89, (short)90, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)96, - (short)1, - (short)1, (short)99, (short)100, - (short)1,
- (short)1, - (short)1, (short)368, - (short)1, (short)106, (short)107, (short)108,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)83, (short)84,
- (short)1, (short)86, (short)87, - (short)1, (short)89, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)96, - (short)1, - (short)1,
(short)99, (short)100, - (short)1, - (short)1, - (short)1, (short)104, - (short)1,
(short)106, - (short)1, - (short)1, - (short)1, (short)110, (short)111, (short)112,
- (short)1, - (short)1, - (short)1, (short)574, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)426, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)3, (short)4,
(short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)20, (short)21, (short)22, (short)23, (short)24, (short)25, (short)468,
(short)27, (short)28, (short)29, (short)30, (short)31, (short)32, (short)33,
(short)34, (short)35, (short)36, (short)37, (short)38, (short)39, (short)40, -
(short)1, (short)42, (short)43, (short)44, - (short)1, - (short)1, (short)47,
(short)48, (short)49, (short)50, (short)51, (short)52, (short)53, (short)54,
(short)55, (short)56, (short)57, (short)58, - (short)1, (short)60, (short)61, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)512, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)83, (short)84, (short)85, (short)86, (short)87, (short)88,
(short)89, (short)90, - (short)1, (short)92, (short)93, - (short)1, - (short)1,
(short)96, - (short)1, - (short)1, (short)99, (short)100, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)106, - (short)1, - (short)1, (short)109,
- (short)1, (short)111, (short)112, - (short)1, - (short)1, - (short)1, -
(short)1, (short)117, (short)118, (short)3, (short)4, (short)5, (short)6,
(short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)20,
(short)21, (short)580, - (short)1, (short)24, (short)25, - (short)1, (short)27,
(short)28, (short)29, (short)30, (short)31, (short)32, (short)33, (short)34,
(short)35, (short)36, (short)37, (short)38, (short)39, (short)40, - (short)1,
(short)42, (short)43, (short)44, - (short)1, - (short)1, (short)47, (short)48,
(short)49, (short)50, (short)51, (short)52, (short)53, (short)54, (short)55,
(short)56, (short)57, (short)58, - (short)1, (short)60, (short)61, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)83, (short)84, (short)85, (short)86, (short)87, (short)88, (short)89,
(short)90, - (short)1, (short)92, (short)93, (short)94, - (short)1, (short)96, -
(short)1, - (short)1, (short)99, (short)100, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)106, - (short)1, - (short)1, (short)109, -
(short)1, (short)111, (short)112, - (short)1, - (short)1, - (short)1, - (short)1,
(short)117, (short)118, (short)3, (short)4, (short)5, (short)6, (short)7,
(short)8, (short)9, (short)10, (short)11, (short)12, (short)13, (short)14,
(short)15, (short)16, (short)17, (short)18, (short)19, (short)20, (short)21, -
(short)1, - (short)1, (short)24, (short)25, - (short)1, (short)27, (short)28,
(short)29, (short)30, (short)31, (short)32, (short)33, (short)34, (short)35,
(short)36, (short)37, (short)38, (short)39, (short)40, - (short)1, (short)42,
(short)43, (short)44, - (short)1, - (short)1, (short)47, (short)48, (short)49,
(short)50, (short)51, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, (short)58, - (short)1, (short)60, (short)61, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)83,
(short)84, (short)85, (short)86, (short)87, (short)88, (short)89, (short)90, -
(short)1, (short)92, (short)93, - (short)1, - (short)1, (short)96, - (short)1, -
(short)1, (short)99, (short)100, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)106, - (short)1, - (short)1, (short)109, - (short)1, (short)111,
(short)112, - (short)1, - (short)1, - (short)1, - (short)1, (short)117, (short)118,
(short)3, (short)4, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)20, (short)21, - (short)1, - (short)1, (short)24, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)35, (short)36, (short)37, (short)38,
(short)39, - (short)1, - (short)1, (short)42, (short)43, - (short)1, - (short)1,
- (short)1, (short)47, (short)48, (short)49, - (short)1, - (short)1, (short)52,
(short)53, (short)54, (short)55, (short)56, (short)57, (short)58, - (short)1,
(short)60, (short)61, - (short)1, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)20, (short)21, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)83, (short)84, (short)85, (short)86, (short)87,
(short)88, (short)89, (short)90, (short)36, (short)92, - (short)1, - (short)1, -
(short)1, (short)96, - (short)1, - (short)1, (short)99, (short)100, - (short)1,
(short)47, - (short)1, - (short)1, - (short)1, (short)106, - (short)1, - (short)1,
(short)109, - (short)1, (short)111, (short)112, - (short)1, - (short)1, -
(short)1, - (short)1, (short)117, (short)118, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)20, (short)21, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)83, - (short)1, - (short)1, - (short)1, - (short)1,
(short)88, (short)89, (short)90, (short)35, (short)36, (short)37, - (short)1,
(short)39, - (short)1, - (short)1, (short)42, (short)99, (short)100, - (short)1,
- (short)1, (short)47, (short)48, (short)49, - (short)1, - (short)1, (short)52,
(short)53, (short)54, (short)55, (short)56, (short)57, (short)58, - (short)1,
(short)60, (short)61, - (short)1, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)20, (short)21, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)83, (short)84, (short)85, (short)86, (short)87,
(short)88, (short)89, (short)90, (short)36, - (short)1, (short)93, - (short)1, -
(short)1, (short)96, - (short)1, - (short)1, (short)99, (short)100, - (short)1,
(short)47, - (short)1, - (short)1, - (short)1, (short)106, - (short)1, - (short)1,
(short)109, - (short)1, (short)111, (short)112, - (short)1, - (short)1, -
(short)1, - (short)1, (short)117, (short)118, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)20, (short)21, - (short)1, - (short)1, (short)80, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)88, - (short)1, (short)90, (short)35, (short)36, (short)37, - (short)1,
(short)39, - (short)1, - (short)1, (short)42, (short)99, (short)100, - (short)1,
- (short)1, (short)47, (short)48, (short)49, - (short)1, - (short)1, (short)52,
(short)53, (short)54, (short)55, (short)56, (short)57, (short)58, - (short)1,
(short)60, (short)61, - (short)1, - (short)1, - (short)1, - (short)1, (short)8,
(short)9, (short)10, (short)11, (short)12, (short)13, (short)14, (short)15,
(short)16, (short)17, (short)18, (short)19, (short)20, (short)21, - (short)1, -
(short)1, - (short)1, (short)83, (short)84, (short)85, (short)86, (short)87,
(short)88, (short)89, (short)90, - (short)1, - (short)1, - (short)1, (short)36,
- (short)1, (short)96, - (short)1, - (short)1, (short)99, (short)100, - (short)1,
- (short)1, - (short)1, - (short)1, (short)47, (short)106, - (short)1, - (short)1,
(short)109, - (short)1, (short)111, (short)112, - (short)1, - (short)1, -
(short)1, - (short)1, (short)117, (short)118, (short)3, (short)4, (short)5,
(short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)20,
(short)21, - (short)1, - (short)1, (short)24, (short)83, - (short)1, - (short)1,
- (short)1, - (short)1, (short)88, (short)89, (short)90, - (short)1, (short)92,
- (short)1, (short)36, - (short)1, (short)96, (short)97, - (short)1, (short)99,
(short)100, (short)43, - (short)1, - (short)1, (short)104, (short)47, (short)106,
(short)107, (short)108, - (short)1, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)20, (short)21, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, (short)36, - (short)1, - (short)1,
(short)83, - (short)1, - (short)1, - (short)1, - (short)1, (short)88, (short)89,
(short)90, (short)47, - (short)1, - (short)1, - (short)1, - (short)1, (short)96,
- (short)1, - (short)1, (short)99, (short)100, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)106, (short)107, (short)108, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)88, - (short)1, (short)90, - (short)1, (short)92,
- (short)1, - (short)1, - (short)1, (short)96, (short)97, - (short)1, (short)99,
(short)100, - (short)1, - (short)1, - (short)1, (short)104, - (short)1, (short)106,
(short)107, (short)108, (short)3, (short)4, (short)5, (short)6, (short)7,
(short)8, (short)9, (short)10, (short)11, (short)12, (short)13, (short)14,
(short)15, (short)16, (short)17, (short)18, (short)19, (short)20, (short)21, -
(short)1, - (short)1, (short)24, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, (short)36, - (short)1, (short)38, - (short)1, - (short)1, - (short)1,
- (short)1, (short)43, - (short)1, - (short)1, - (short)1, (short)47, - (short)1,
- (short)1, - (short)1, - (short)1, (short)3, (short)4, (short)5, (short)6,
(short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)20,
(short)21, - (short)1, - (short)1, (short)24, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, (short)36, - (short)1, (short)38, (short)88, - (short)1,
(short)90, - (short)1, (short)43, (short)93, - (short)1, (short)95, (short)47, -
(short)1, - (short)1, (short)99, (short)100, (short)3, (short)4, (short)5,
(short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)20,
(short)21, - (short)1, - (short)1, (short)24, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, (short)36, - (short)1, - (short)1, (short)88, - (short)1,
(short)90, - (short)1, (short)43, (short)93, - (short)1, - (short)1, (short)47,
- (short)1, - (short)1, (short)99, (short)100, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)20, (short)21, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)88,
- (short)1, (short)90, (short)36, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)99, (short)100, (short)25, (short)47,
(short)27, (short)28, (short)29, (short)30, (short)31, (short)32, (short)33,
(short)34, (short)35, - (short)1, (short)37, - (short)1, (short)39, (short)40, -
(short)1, (short)42, - (short)1, (short)44, - (short)1, - (short)1, - (short)1,
(short)48, (short)49, (short)50, (short)51, (short)52, (short)53, (short)54,
(short)55, (short)56, (short)57, (short)58, - (short)1, (short)60, (short)61,
(short)83, - (short)1, - (short)1, - (short)1, - (short)1, (short)88, - (short)1,
(short)90, - (short)1, (short)92, - (short)1, - (short)1, - (short)1, - (short)1,
(short)97, - (short)1, (short)99, (short)100, - (short)1, - (short)1, - (short)1,
(short)83, (short)84, (short)85, (short)86, (short)87, - (short)1, (short)89, -
(short)1, - (short)1, (short)92, (short)93, - (short)1, - (short)1, (short)96, -
(short)1, - (short)1, - (short)1, (short)100, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)106, - (short)1, - (short)1, (short)109, -
(short)1, (short)111, (short)112, - (short)1, - (short)1, - (short)1, - (short)1,
(short)117, (short)118, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)20, (short)21, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)20, (short)21, (short)36, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)47,
- (short)1, - (short)1, (short)36, - (short)1, - (short)1, (short)30, - (short)1,
- (short)1, - (short)1, - (short)1, (short)35, - (short)1, (short)37, (short)47,
(short)39, - (short)1, - (short)1, (short)42, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)48, (short)49, - (short)1, - (short)1, (short)52,
(short)53, (short)54, (short)55, (short)56, (short)57, (short)58, - (short)1,
(short)60, (short)61, - (short)1, - (short)1, - (short)1, (short)88, - (short)1,
(short)90, - (short)1, - (short)1, (short)93, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)99, (short)100, - (short)1, (short)88, - (short)1,
(short)90, - (short)1, (short)83, (short)84, (short)85, (short)86, (short)87, -
(short)1, (short)89, (short)99, (short)100, - (short)1, (short)93, (short)94, -
(short)1, (short)96, - (short)1, - (short)1, - (short)1, (short)100, - (short)1,
- (short)1, - (short)1, (short)104, - (short)1, (short)106, - (short)1, -
(short)1, (short)109, (short)110, (short)111, (short)112, - (short)1, - (short)1,
- (short)1, - (short)1, (short)117, (short)118, (short)35, - (short)1, (short)37,
- (short)1, (short)39, - (short)1, - (short)1, (short)42, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)48, (short)49, - (short)1, - (short)1,
(short)52, (short)53, (short)54, (short)55, (short)56, (short)57, (short)58, -
(short)1, (short)60, (short)61, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)83, (short)84, (short)85,
(short)86, (short)87, - (short)1, (short)89, - (short)1, - (short)1, - (short)1,
(short)93, (short)94, - (short)1, (short)96, - (short)1, - (short)1, - (short)1,
(short)100, - (short)1, - (short)1, - (short)1, (short)104, - (short)1, (short)106,
- (short)1, - (short)1, (short)109, (short)110, (short)111, (short)112, -
(short)1, - (short)1, - (short)1, - (short)1, (short)117, (short)118, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, (short)58, - (short)1, (short)60, (short)61, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)83,
(short)84, (short)85, (short)86, (short)87, - (short)1, (short)89, - (short)1, -
(short)1, - (short)1, (short)93, (short)94, - (short)1, (short)96, - (short)1, -
(short)1, - (short)1, (short)100, - (short)1, - (short)1, - (short)1, (short)104,
- (short)1, (short)106, - (short)1, - (short)1, (short)109, (short)110, (short)111,
(short)112, - (short)1, - (short)1, - (short)1, - (short)1, (short)117, (short)118,
(short)35, - (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1,
(short)42, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48,
(short)49, - (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55,
(short)56, (short)57, (short)58, - (short)1, (short)60, (short)61, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)83, (short)84, (short)85, (short)86, (short)87, - (short)1, (short)89, -
(short)1, - (short)1, - (short)1, (short)93, (short)94, - (short)1, (short)96, -
(short)1, - (short)1, - (short)1, (short)100, - (short)1, - (short)1, - (short)1,
(short)104, - (short)1, (short)106, - (short)1, - (short)1, (short)109, (short)110,
(short)111, (short)112, - (short)1, - (short)1, - (short)1, - (short)1, (short)117,
(short)118, (short)35, - (short)1, (short)37, - (short)1, (short)39, - (short)1,
- (short)1, (short)42, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)48, (short)49, - (short)1, - (short)1, (short)52, (short)53, (short)54,
(short)55, (short)56, (short)57, (short)58, - (short)1, (short)60, (short)61, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
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
- (short)1, - (short)1, (short)96, - (short)1, - (short)1, - (short)1, (short)100,
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
- (short)1, - (short)1, - (short)1, - (short)1, (short)96, - (short)1, - (short)1,
- (short)1, (short)100, - (short)1, - (short)1, - (short)1, - (short)1, (short)105,
(short)106, - (short)1, - (short)1, (short)109, - (short)1, (short)111, (short)112,
- (short)1, - (short)1, - (short)1, - (short)1, (short)117, (short)118, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, (short)58, - (short)1, (short)60, (short)61, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)83,
(short)84, (short)85, (short)86, (short)87, - (short)1, (short)89, - (short)1, -
(short)1, - (short)1, (short)93, - (short)1, - (short)1, (short)96, - (short)1,
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
(short)89, - (short)1, - (short)1, (short)92, - (short)1, - (short)1, - (short)1,
(short)96, - (short)1, - (short)1, - (short)1, (short)100, - (short)1, - (short)1,
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
- (short)1, - (short)1, (short)96, - (short)1, - (short)1, - (short)1, (short)100,
- (short)1, - (short)1, - (short)1, - (short)1, (short)105, (short)106, -
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
- (short)1, (short)96, - (short)1, - (short)1, - (short)1, (short)100, - (short)1,
- (short)1, - (short)1, - (short)1, (short)105, (short)106, - (short)1, -
(short)1, (short)109, - (short)1, (short)111, (short)112, - (short)1, - (short)1,
- (short)1, - (short)1, (short)117, (short)118, (short)35, - (short)1, (short)37,
- (short)1, (short)39, - (short)1, - (short)1, (short)42, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)48, (short)49, - (short)1, - (short)1,
(short)52, (short)53, (short)54, (short)55, (short)56, (short)57, (short)58, -
(short)1, (short)60, (short)61, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)83, (short)84, (short)85,
(short)86, (short)87, - (short)1, (short)89, - (short)1, - (short)1, - (short)1,
(short)93, - (short)1, - (short)1, (short)96, - (short)1, - (short)1, - (short)1,
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
(short)1, (short)92, - (short)1, - (short)1, - (short)1, (short)96, - (short)1,
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
(short)89, - (short)1, - (short)1, (short)92, - (short)1, - (short)1, - (short)1,
(short)96, - (short)1, - (short)1, - (short)1, (short)100, - (short)1, - (short)1,
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
(short)1, - (short)1, (short)92, - (short)1, - (short)1, - (short)1, (short)96,
- (short)1, - (short)1, - (short)1, (short)100, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, (short)106, - (short)1, - (short)1, (short)109,
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
(short)86, (short)87, - (short)1, (short)89, - (short)1, - (short)1, - (short)1,
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
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)96, - (short)1,
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
(short)96, - (short)1, - (short)1, - (short)1, (short)100, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)106, - (short)1, - (short)1, (short)109,
- (short)1, (short)111, (short)112, - (short)1, - (short)1, - (short)1, -
(short)1, (short)117, (short)118}; char Cyc_Yyimpossible_tag[ 13u]="Yyimpossible";
struct Cyc_Yyimpossible_struct{ char* tag; } ; char Cyc_Yystack_overflow_tag[ 17u]="Yystack_overflow";
struct Cyc_Yystack_overflow_struct{ char* tag; } ; extern void Cyc_yyerror(
struct _tagged_string); extern int Cyc_yylex(); char Cyc_YYINITIALSVAL_tag[ 14u]="YYINITIALSVAL";
struct Cyc_YYINITIALSVAL_struct{ char* tag; } ; static int Cyc_yychar=( int)'\000';
static struct Cyc_YYINITIALSVAL_struct _temp1943={ Cyc_YYINITIALSVAL_tag};
struct _xenum_struct* Cyc_yylval=( struct _xenum_struct*)& _temp1943; static int
Cyc_yynerrs= 0; int Cyc_yyparse(){ int yystate; int yyn= 0; int yyerrstatus; int
yychar1= 0; int yyssp_offset; struct _tagged_ptr1 yyss=({ unsigned int _temp1944=(
unsigned int) 200; short* _temp1945=( short*) GC_malloc_atomic( sizeof( short) *
_temp1944); unsigned int i; struct _tagged_ptr1 _temp1946={ _temp1945, _temp1945,
_temp1945 + _temp1944}; for( i= 0; i < _temp1944; i ++){ _temp1945[ i]= (short)0;}
_temp1946;}); int yyvsp_offset; struct _tagged_ptr2 yyvs=({ unsigned int
_temp1947=( unsigned int) 200; struct _xenum_struct** _temp1948=( struct
_xenum_struct**) GC_malloc( sizeof( struct _xenum_struct*) * _temp1947);
unsigned int i; struct _tagged_ptr2 _temp1949={ _temp1948, _temp1948, _temp1948
+ _temp1947}; for( i= 0; i < _temp1947; i ++){ _temp1948[ i]= Cyc_yylval;}
_temp1949;}); int yylsp_offset; struct _tagged_ptr3 yyls=({ unsigned int
_temp1950=( unsigned int) 200; struct Cyc_Yyltype* _temp1951=( struct Cyc_Yyltype*)
GC_malloc( sizeof( struct Cyc_Yyltype) * _temp1950); unsigned int i; struct
_tagged_ptr3 _temp1952={ _temp1951, _temp1951, _temp1951 + _temp1950}; for( i= 0;
i < _temp1950; i ++){ _temp1951[ i]= Cyc_yynewloc();} _temp1952;}); int
yystacksize= 200; struct _xenum_struct* yyval= Cyc_yylval; int yylen; yystate= 0;
yyerrstatus= 0; Cyc_yynerrs= 0; Cyc_yychar= - 2; yyssp_offset= - 1; yyvsp_offset=
0; yylsp_offset= 0; yynewstate:({ struct _tagged_ptr1 _temp1953= yyss; short*
_temp1955= _temp1953.curr +( ++ yyssp_offset); if( _temp1955 < _temp1953.base? 1:
_temp1955 >= _temp1953.last_plus_one){ _throw( Null_Exception);}* _temp1955=(
short) yystate;}); if( yyssp_offset >= yystacksize - 1){ if( yystacksize >=
10000){ Cyc_yyerror(( struct _tagged_string)({ char* _temp1956=( char*)"parser stack overflow";
struct _tagged_string _temp1957; _temp1957.curr= _temp1956; _temp1957.base=
_temp1956; _temp1957.last_plus_one= _temp1956 + 22; _temp1957;}));( void) _throw(({
struct Cyc_Yystack_overflow_struct* _temp1958=( struct Cyc_Yystack_overflow_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Yystack_overflow_struct)); _temp1958->tag=
Cyc_Yystack_overflow_tag;( struct _xenum_struct*) _temp1958;}));} yystacksize *=
2; if( yystacksize > 10000){ yystacksize= 10000;}{ struct _tagged_ptr1 yyss1=({
unsigned int _temp1959=( unsigned int) yystacksize; short* _temp1960=( short*)
GC_malloc_atomic( sizeof( short) * _temp1959); unsigned int i; struct
_tagged_ptr1 _temp1961={ _temp1960, _temp1960, _temp1960 + _temp1959}; for( i= 0;
i < _temp1959; i ++){ _temp1960[ i]= (short)0;} _temp1961;}); struct
_tagged_ptr2 yyvs1=({ unsigned int _temp1962=( unsigned int) yystacksize; struct
_xenum_struct** _temp1963=( struct _xenum_struct**) GC_malloc( sizeof( struct
_xenum_struct*) * _temp1962); unsigned int i; struct _tagged_ptr2 _temp1964={
_temp1963, _temp1963, _temp1963 + _temp1962}; for( i= 0; i < _temp1962; i ++){
_temp1963[ i]= Cyc_yylval;} _temp1964;}); struct _tagged_ptr3 yyls1=({
unsigned int _temp1965=( unsigned int) yystacksize; struct Cyc_Yyltype*
_temp1966=( struct Cyc_Yyltype*) GC_malloc( sizeof( struct Cyc_Yyltype) *
_temp1965); unsigned int i; struct _tagged_ptr3 _temp1967={ _temp1966, _temp1966,
_temp1966 + _temp1965}; for( i= 0; i < _temp1965; i ++){ _temp1966[ i]= Cyc_yynewloc();}
_temp1967;});{ int i= 0; for( 0; i <= yyssp_offset; i ++){({ struct _tagged_ptr1
_temp1968= yyss1; short* _temp1970= _temp1968.curr + i; if( _temp1970 <
_temp1968.base? 1: _temp1970 >= _temp1968.last_plus_one){ _throw( Null_Exception);}*
_temp1970=({ struct _tagged_ptr1 _temp1971= yyss; short* _temp1973= _temp1971.curr
+ i; if( _temp1973 < _temp1971.base? 1: _temp1973 >= _temp1971.last_plus_one){
_throw( Null_Exception);}* _temp1973;});});({ struct _tagged_ptr2 _temp1974=
yyvs1; struct _xenum_struct** _temp1976= _temp1974.curr + i; if( _temp1976 <
_temp1974.base? 1: _temp1976 >= _temp1974.last_plus_one){ _throw( Null_Exception);}*
_temp1976=({ struct _tagged_ptr2 _temp1977= yyvs; struct _xenum_struct**
_temp1979= _temp1977.curr + i; if( _temp1979 < _temp1977.base? 1: _temp1979 >=
_temp1977.last_plus_one){ _throw( Null_Exception);}* _temp1979;});});({ struct
_tagged_ptr3 _temp1980= yyls1; struct Cyc_Yyltype* _temp1982= _temp1980.curr + i;
if( _temp1982 < _temp1980.base? 1: _temp1982 >= _temp1980.last_plus_one){ _throw(
Null_Exception);}* _temp1982=({ struct _tagged_ptr3 _temp1983= yyls; struct Cyc_Yyltype*
_temp1985= _temp1983.curr + i; if( _temp1985 < _temp1983.base? 1: _temp1985 >=
_temp1983.last_plus_one){ _throw( Null_Exception);}* _temp1985;});});}} yyss=
yyss1; yyvs= yyvs1; yyls= yyls1;}} goto yybackup; yybackup: yyn=( int)({ short*
_temp1986=( short*) Cyc_yypact; unsigned int _temp1987= yystate; if( _temp1987
>= 717u){ _throw( Null_Exception);} _temp1986[ _temp1987];}); if( yyn == - 32768){
goto yydefault;} if( Cyc_yychar == - 2){ Cyc_yychar= Cyc_yylex();} if( Cyc_yychar
<= 0){ yychar1= 0; Cyc_yychar= 0;} else{ yychar1=( Cyc_yychar > 0? Cyc_yychar <=
346: 0)?( int)({ short* _temp1988=( short*) Cyc_yytranslate; unsigned int
_temp1989= Cyc_yychar; if( _temp1989 >= 347u){ _throw( Null_Exception);}
_temp1988[ _temp1989];}): 221;} yyn += yychar1; if(( yyn < 0? 1: yyn > 4314)? 1:(
int)({ short* _temp1990=( short*) Cyc_yycheck; unsigned int _temp1991= yyn; if(
_temp1991 >= 4315u){ _throw( Null_Exception);} _temp1990[ _temp1991];}) !=
yychar1){ goto yydefault;} yyn=( int)({ short* _temp1992=( short*) Cyc_yytable;
unsigned int _temp1993= yyn; if( _temp1993 >= 4315u){ _throw( Null_Exception);}
_temp1992[ _temp1993];}); if( yyn < 0){ if( yyn == - 32768){ goto yyerrlab;} yyn=
- yyn; goto yyreduce;} else{ if( yyn == 0){ goto yyerrlab;}} if( yyn == 716){
return 0;} if( Cyc_yychar != 0){ Cyc_yychar= - 2;}({ struct _tagged_ptr2
_temp1994= yyvs; struct _xenum_struct** _temp1996= _temp1994.curr +( ++
yyvsp_offset); if( _temp1996 < _temp1994.base? 1: _temp1996 >= _temp1994.last_plus_one){
_throw( Null_Exception);}* _temp1996= Cyc_yylval;});({ struct _tagged_ptr3
_temp1997= yyls; struct Cyc_Yyltype* _temp1999= _temp1997.curr +( ++
yylsp_offset); if( _temp1999 < _temp1997.base? 1: _temp1999 >= _temp1997.last_plus_one){
_throw( Null_Exception);}* _temp1999= Cyc_yylloc;}); if( yyerrstatus != 0){
yyerrstatus --;} yystate= yyn; goto yynewstate; yydefault: yyn=( int)({ short*
_temp2000=( short*) Cyc_yydefact; unsigned int _temp2001= yystate; if( _temp2001
>= 717u){ _throw( Null_Exception);} _temp2000[ _temp2001];}); if( yyn == 0){
goto yyerrlab;} yyreduce: yylen=( int)({ short* _temp2002=( short*) Cyc_yyr2;
unsigned int _temp2003= yyn; if( _temp2003 >= 381u){ _throw( Null_Exception);}
_temp2002[ _temp2003];}); if( yylen > 0){ yyval=({ struct _tagged_ptr2 _temp2004=
yyvs; struct _xenum_struct** _temp2006= _temp2004.curr +(( yyvsp_offset + 1) -
yylen); if( _temp2006 < _temp2004.base? 1: _temp2006 >= _temp2004.last_plus_one){
_throw( Null_Exception);}* _temp2006;});} switch( yyn){ case 1: _LL2007: yyval=({
struct _tagged_ptr2 _temp2009= yyvs; struct _xenum_struct** _temp2011= _temp2009.curr
+ yyvsp_offset; if( _temp2011 < _temp2009.base? 1: _temp2011 >= _temp2009.last_plus_one){
_throw( Null_Exception);}* _temp2011;}); Cyc_Parse_parse_result= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2012= yyvs; struct _xenum_struct** _temp2014= _temp2012.curr
+ yyvsp_offset; if( _temp2014 < _temp2012.base? 1: _temp2014 >= _temp2012.last_plus_one){
_throw( Null_Exception);}* _temp2014;})); break; case 2: _LL2008: yyval=({
struct _tagged_ptr2 _temp2016= yyvs; struct _xenum_struct** _temp2018= _temp2016.curr
+ yyvsp_offset; if( _temp2018 < _temp2016.base? 1: _temp2018 >= _temp2016.last_plus_one){
_throw( Null_Exception);}* _temp2018;}); break; case 3: _LL2015: yyval=({ struct
Cyc_DeclList_tok_struct* _temp2020=( struct Cyc_DeclList_tok_struct*) GC_malloc(
sizeof( struct Cyc_DeclList_tok_struct)); _temp2020->tag= Cyc_DeclList_tok_tag;
_temp2020->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2
_temp2021= yyvs; struct _xenum_struct** _temp2023= _temp2021.curr +(
yyvsp_offset - 1); if( _temp2023 < _temp2021.base? 1: _temp2023 >= _temp2021.last_plus_one){
_throw( Null_Exception);}* _temp2023;})), Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp2024= yyvs; struct _xenum_struct** _temp2026= _temp2024.curr +
yyvsp_offset; if( _temp2026 < _temp2024.base? 1: _temp2026 >= _temp2024.last_plus_one){
_throw( Null_Exception);}* _temp2026;})));( struct _xenum_struct*) _temp2020;});
break; case 4: _LL2019: yyval=({ struct Cyc_DeclList_tok_struct* _temp2028=(
struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2028->tag= Cyc_DeclList_tok_tag; _temp2028->f1=({ struct Cyc_List_List*
_temp2029=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2029->hd=( void*)({ struct Cyc_Absyn_Decl* _temp2030=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp2030->r=( void*)({ struct Cyc_Absyn_Using_d_struct*
_temp2037=( struct Cyc_Absyn_Using_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp2037->tag= Cyc_Absyn_Using_d_tag; _temp2037->f1= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp2038= yyvs; struct _xenum_struct** _temp2040= _temp2038.curr
+( yyvsp_offset - 2); if( _temp2040 < _temp2038.base? 1: _temp2040 >= _temp2038.last_plus_one){
_throw( Null_Exception);}* _temp2040;})); _temp2037->f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2041= yyvs; struct _xenum_struct** _temp2043= _temp2041.curr
+ yyvsp_offset; if( _temp2043 < _temp2041.base? 1: _temp2043 >= _temp2041.last_plus_one){
_throw( Null_Exception);}* _temp2043;}));( void*) _temp2037;}); _temp2030->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2031= yyls; struct Cyc_Yyltype*
_temp2033= _temp2031.curr +( yylsp_offset - 2); if( _temp2033 < _temp2031.base?
1: _temp2033 >= _temp2031.last_plus_one){ _throw( Null_Exception);}* _temp2033;}).first_line,({
struct _tagged_ptr3 _temp2034= yyls; struct Cyc_Yyltype* _temp2036= _temp2034.curr
+ yylsp_offset; if( _temp2036 < _temp2034.base? 1: _temp2036 >= _temp2034.last_plus_one){
_throw( Null_Exception);}* _temp2036;}).last_line); _temp2030;}); _temp2029->tl=
0; _temp2029;});( struct _xenum_struct*) _temp2028;}); Cyc_Lex_leave_using();
break; case 5: _LL2027: yyval=({ struct Cyc_DeclList_tok_struct* _temp2045=(
struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2045->tag= Cyc_DeclList_tok_tag; _temp2045->f1=({ struct Cyc_List_List*
_temp2046=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2046->hd=( void*)({ struct Cyc_Absyn_Decl* _temp2050=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp2050->r=( void*)({ struct Cyc_Absyn_Using_d_struct*
_temp2057=( struct Cyc_Absyn_Using_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct));
_temp2057->tag= Cyc_Absyn_Using_d_tag; _temp2057->f1= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp2058= yyvs; struct _xenum_struct** _temp2060= _temp2058.curr
+( yyvsp_offset - 4); if( _temp2060 < _temp2058.base? 1: _temp2060 >= _temp2058.last_plus_one){
_throw( Null_Exception);}* _temp2060;})); _temp2057->f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2061= yyvs; struct _xenum_struct** _temp2063= _temp2061.curr
+( yyvsp_offset - 2); if( _temp2063 < _temp2061.base? 1: _temp2063 >= _temp2061.last_plus_one){
_throw( Null_Exception);}* _temp2063;}));( void*) _temp2057;}); _temp2050->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2051= yyls; struct Cyc_Yyltype*
_temp2053= _temp2051.curr +( yylsp_offset - 4); if( _temp2053 < _temp2051.base?
1: _temp2053 >= _temp2051.last_plus_one){ _throw( Null_Exception);}* _temp2053;}).first_line,({
struct _tagged_ptr3 _temp2054= yyls; struct Cyc_Yyltype* _temp2056= _temp2054.curr
+( yylsp_offset - 1); if( _temp2056 < _temp2054.base? 1: _temp2056 >= _temp2054.last_plus_one){
_throw( Null_Exception);}* _temp2056;}).last_line); _temp2050;}); _temp2046->tl=
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2047= yyvs; struct
_xenum_struct** _temp2049= _temp2047.curr + yyvsp_offset; if( _temp2049 <
_temp2047.base? 1: _temp2049 >= _temp2047.last_plus_one){ _throw( Null_Exception);}*
_temp2049;})); _temp2046;});( struct _xenum_struct*) _temp2045;}); break; case 6:
_LL2044: yyval=({ struct Cyc_DeclList_tok_struct* _temp2065=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2065->tag= Cyc_DeclList_tok_tag;
_temp2065->f1=({ struct Cyc_List_List* _temp2066=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2066->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2067=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2067->r=( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp2074=( struct
Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp2074->tag= Cyc_Absyn_Namespace_d_tag; _temp2074->f1=({ struct
_tagged_string* _temp2075=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp2075[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp2076= yyvs; struct _xenum_struct** _temp2078= _temp2076.curr +(
yyvsp_offset - 2); if( _temp2078 < _temp2076.base? 1: _temp2078 >= _temp2076.last_plus_one){
_throw( Null_Exception);}* _temp2078;})); _temp2075;}); _temp2074->f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2079= yyvs; struct _xenum_struct** _temp2081= _temp2079.curr
+ yyvsp_offset; if( _temp2081 < _temp2079.base? 1: _temp2081 >= _temp2079.last_plus_one){
_throw( Null_Exception);}* _temp2081;}));( void*) _temp2074;}); _temp2067->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2068= yyls; struct Cyc_Yyltype*
_temp2070= _temp2068.curr +( yylsp_offset - 2); if( _temp2070 < _temp2068.base?
1: _temp2070 >= _temp2068.last_plus_one){ _throw( Null_Exception);}* _temp2070;}).first_line,({
struct _tagged_ptr3 _temp2071= yyls; struct Cyc_Yyltype* _temp2073= _temp2071.curr
+ yylsp_offset; if( _temp2073 < _temp2071.base? 1: _temp2073 >= _temp2071.last_plus_one){
_throw( Null_Exception);}* _temp2073;}).last_line); _temp2067;}); _temp2066->tl=
0; _temp2066;});( struct _xenum_struct*) _temp2065;}); Cyc_Lex_leave_namespace();
break; case 7: _LL2064: yyval=({ struct Cyc_DeclList_tok_struct* _temp2083=(
struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2083->tag= Cyc_DeclList_tok_tag; _temp2083->f1=({ struct Cyc_List_List*
_temp2084=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2084->hd=( void*)({ struct Cyc_Absyn_Decl* _temp2088=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp2088->r=( void*)({ struct Cyc_Absyn_Namespace_d_struct*
_temp2095=( struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp2095->tag= Cyc_Absyn_Namespace_d_tag; _temp2095->f1=({ struct
_tagged_string* _temp2096=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp2096[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp2097= yyvs; struct _xenum_struct** _temp2099= _temp2097.curr +(
yyvsp_offset - 4); if( _temp2099 < _temp2097.base? 1: _temp2099 >= _temp2097.last_plus_one){
_throw( Null_Exception);}* _temp2099;})); _temp2096;}); _temp2095->f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2100= yyvs; struct _xenum_struct** _temp2102= _temp2100.curr
+( yyvsp_offset - 2); if( _temp2102 < _temp2100.base? 1: _temp2102 >= _temp2100.last_plus_one){
_throw( Null_Exception);}* _temp2102;}));( void*) _temp2095;}); _temp2088->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2089= yyls; struct Cyc_Yyltype*
_temp2091= _temp2089.curr +( yylsp_offset - 4); if( _temp2091 < _temp2089.base?
1: _temp2091 >= _temp2089.last_plus_one){ _throw( Null_Exception);}* _temp2091;}).first_line,({
struct _tagged_ptr3 _temp2092= yyls; struct Cyc_Yyltype* _temp2094= _temp2092.curr
+( yylsp_offset - 1); if( _temp2094 < _temp2092.base? 1: _temp2094 >= _temp2092.last_plus_one){
_throw( Null_Exception);}* _temp2094;}).last_line); _temp2088;}); _temp2084->tl=
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2085= yyvs; struct
_xenum_struct** _temp2087= _temp2085.curr + yyvsp_offset; if( _temp2087 <
_temp2085.base? 1: _temp2087 >= _temp2085.last_plus_one){ _throw( Null_Exception);}*
_temp2087;})); _temp2084;});( struct _xenum_struct*) _temp2083;}); break; case 8:
_LL2082: if( Cyc_String_strcmp( Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp2104= yyvs; struct _xenum_struct** _temp2106= _temp2104.curr +(
yyvsp_offset - 4); if( _temp2106 < _temp2104.base? 1: _temp2106 >= _temp2104.last_plus_one){
_throw( Null_Exception);}* _temp2106;})),( struct _tagged_string)({ char*
_temp2107=( char*)"C"; struct _tagged_string _temp2108; _temp2108.curr=
_temp2107; _temp2108.base= _temp2107; _temp2108.last_plus_one= _temp2107 + 2;
_temp2108;})) != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp2109=(
char*)"only extern \"C\" { ... } is allowed"; struct _tagged_string _temp2110;
_temp2110.curr= _temp2109; _temp2110.base= _temp2109; _temp2110.last_plus_one=
_temp2109 + 35; _temp2110;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2111= yyls; struct Cyc_Yyltype* _temp2113= _temp2111.curr +( yylsp_offset -
5); if( _temp2113 < _temp2111.base? 1: _temp2113 >= _temp2111.last_plus_one){
_throw( Null_Exception);}* _temp2113;}).first_line,({ struct _tagged_ptr3
_temp2114= yyls; struct Cyc_Yyltype* _temp2116= _temp2114.curr +( yylsp_offset -
4); if( _temp2116 < _temp2114.base? 1: _temp2116 >= _temp2114.last_plus_one){
_throw( Null_Exception);}* _temp2116;}).last_line));} yyval=({ struct Cyc_DeclList_tok_struct*
_temp2117=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2117->tag= Cyc_DeclList_tok_tag; _temp2117->f1=({ struct Cyc_List_List*
_temp2118=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2118->hd=( void*)({ struct Cyc_Absyn_Decl* _temp2122=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp2122->r=( void*)({ struct Cyc_Absyn_ExternC_d_struct*
_temp2129=( struct Cyc_Absyn_ExternC_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ExternC_d_struct));
_temp2129->tag= Cyc_Absyn_ExternC_d_tag; _temp2129->f1= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2130= yyvs; struct _xenum_struct** _temp2132= _temp2130.curr
+( yyvsp_offset - 2); if( _temp2132 < _temp2130.base? 1: _temp2132 >= _temp2130.last_plus_one){
_throw( Null_Exception);}* _temp2132;}));( void*) _temp2129;}); _temp2122->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2123= yyls; struct Cyc_Yyltype*
_temp2125= _temp2123.curr +( yylsp_offset - 5); if( _temp2125 < _temp2123.base?
1: _temp2125 >= _temp2123.last_plus_one){ _throw( Null_Exception);}* _temp2125;}).first_line,({
struct _tagged_ptr3 _temp2126= yyls; struct Cyc_Yyltype* _temp2128= _temp2126.curr
+( yylsp_offset - 1); if( _temp2128 < _temp2126.base? 1: _temp2128 >= _temp2126.last_plus_one){
_throw( Null_Exception);}* _temp2128;}).last_line); _temp2122;}); _temp2118->tl=
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2119= yyvs; struct
_xenum_struct** _temp2121= _temp2119.curr + yyvsp_offset; if( _temp2121 <
_temp2119.base? 1: _temp2121 >= _temp2119.last_plus_one){ _throw( Null_Exception);}*
_temp2121;})); _temp2118;});( struct _xenum_struct*) _temp2117;}); break; case 9:
_LL2103: yyval=({ struct Cyc_DeclList_tok_struct* _temp2134=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2134->tag= Cyc_DeclList_tok_tag;
_temp2134->f1= 0;( struct _xenum_struct*) _temp2134;}); break; case 10: _LL2133:
yyval=({ struct _tagged_ptr2 _temp2136= yyvs; struct _xenum_struct** _temp2138=
_temp2136.curr + yyvsp_offset; if( _temp2138 < _temp2136.base? 1: _temp2138 >=
_temp2136.last_plus_one){ _throw( Null_Exception);}* _temp2138;}); break; case
11: _LL2135: yyval=({ struct Cyc_DeclList_tok_struct* _temp2140=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2140->tag= Cyc_DeclList_tok_tag;
_temp2140->f1=({ struct Cyc_List_List* _temp2141=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2141->hd=( void*) Cyc_Absyn_new_decl(({
struct Cyc_Absyn_Fn_d_struct* _temp2142=( struct Cyc_Absyn_Fn_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct)); _temp2142->tag= Cyc_Absyn_Fn_d_tag;
_temp2142->f1= Cyc_yyget_FnDecl_tok(({ struct _tagged_ptr2 _temp2143= yyvs;
struct _xenum_struct** _temp2145= _temp2143.curr + yyvsp_offset; if( _temp2145 <
_temp2143.base? 1: _temp2145 >= _temp2143.last_plus_one){ _throw( Null_Exception);}*
_temp2145;}));( void*) _temp2142;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2146= yyls; struct Cyc_Yyltype* _temp2148= _temp2146.curr +
yylsp_offset; if( _temp2148 < _temp2146.base? 1: _temp2148 >= _temp2146.last_plus_one){
_throw( Null_Exception);}* _temp2148;}).first_line,({ struct _tagged_ptr3
_temp2149= yyls; struct Cyc_Yyltype* _temp2151= _temp2149.curr + yylsp_offset;
if( _temp2151 < _temp2149.base? 1: _temp2151 >= _temp2149.last_plus_one){ _throw(
Null_Exception);}* _temp2151;}).last_line)); _temp2141->tl= 0; _temp2141;});(
struct _xenum_struct*) _temp2140;}); break; case 12: _LL2139: yyval=({ struct
_tagged_ptr2 _temp2153= yyvs; struct _xenum_struct** _temp2155= _temp2153.curr +
yyvsp_offset; if( _temp2155 < _temp2153.base? 1: _temp2155 >= _temp2153.last_plus_one){
_throw( Null_Exception);}* _temp2155;}); break; case 13: _LL2152: yyval=({
struct Cyc_FnDecl_tok_struct* _temp2157=( struct Cyc_FnDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct)); _temp2157->tag= Cyc_FnDecl_tok_tag;
_temp2157->f1= Cyc_Parse_make_function( 0, Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp2158= yyvs; struct _xenum_struct** _temp2160= _temp2158.curr +(
yyvsp_offset - 1); if( _temp2160 < _temp2158.base? 1: _temp2160 >= _temp2158.last_plus_one){
_throw( Null_Exception);}* _temp2160;})), 0, Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp2161= yyvs; struct _xenum_struct** _temp2163= _temp2161.curr +
yyvsp_offset; if( _temp2163 < _temp2161.base? 1: _temp2163 >= _temp2161.last_plus_one){
_throw( Null_Exception);}* _temp2163;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2164= yyls; struct Cyc_Yyltype* _temp2166= _temp2164.curr +(
yylsp_offset - 1); if( _temp2166 < _temp2164.base? 1: _temp2166 >= _temp2164.last_plus_one){
_throw( Null_Exception);}* _temp2166;}).first_line,({ struct _tagged_ptr3
_temp2167= yyls; struct Cyc_Yyltype* _temp2169= _temp2167.curr + yylsp_offset;
if( _temp2169 < _temp2167.base? 1: _temp2169 >= _temp2167.last_plus_one){ _throw(
Null_Exception);}* _temp2169;}).last_line));( struct _xenum_struct*) _temp2157;});
break; case 14: _LL2156: yyval=({ struct Cyc_FnDecl_tok_struct* _temp2171=(
struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2171->tag= Cyc_FnDecl_tok_tag; _temp2171->f1= Cyc_Parse_make_function(({
struct Cyc_Core_Opt* _temp2172=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2172->v=( void*) Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2173= yyvs; struct _xenum_struct** _temp2175= _temp2173.curr +(
yyvsp_offset - 2); if( _temp2175 < _temp2173.base? 1: _temp2175 >= _temp2173.last_plus_one){
_throw( Null_Exception);}* _temp2175;})); _temp2172;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2176= yyvs; struct _xenum_struct** _temp2178= _temp2176.curr
+( yyvsp_offset - 1); if( _temp2178 < _temp2176.base? 1: _temp2178 >= _temp2176.last_plus_one){
_throw( Null_Exception);}* _temp2178;})), 0, Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp2179= yyvs; struct _xenum_struct** _temp2181= _temp2179.curr +
yyvsp_offset; if( _temp2181 < _temp2179.base? 1: _temp2181 >= _temp2179.last_plus_one){
_throw( Null_Exception);}* _temp2181;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2182= yyls; struct Cyc_Yyltype* _temp2184= _temp2182.curr +(
yylsp_offset - 2); if( _temp2184 < _temp2182.base? 1: _temp2184 >= _temp2182.last_plus_one){
_throw( Null_Exception);}* _temp2184;}).first_line,({ struct _tagged_ptr3
_temp2185= yyls; struct Cyc_Yyltype* _temp2187= _temp2185.curr + yylsp_offset;
if( _temp2187 < _temp2185.base? 1: _temp2187 >= _temp2185.last_plus_one){ _throw(
Null_Exception);}* _temp2187;}).last_line));( struct _xenum_struct*) _temp2171;});
break; case 15: _LL2170: yyval=({ struct Cyc_FnDecl_tok_struct* _temp2189=(
struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2189->tag= Cyc_FnDecl_tok_tag; _temp2189->f1= Cyc_Parse_make_function( 0,
Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp2190= yyvs; struct
_xenum_struct** _temp2192= _temp2190.curr +( yyvsp_offset - 2); if( _temp2192 <
_temp2190.base? 1: _temp2192 >= _temp2190.last_plus_one){ _throw( Null_Exception);}*
_temp2192;})), Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2193= yyvs;
struct _xenum_struct** _temp2195= _temp2193.curr +( yyvsp_offset - 1); if(
_temp2195 < _temp2193.base? 1: _temp2195 >= _temp2193.last_plus_one){ _throw(
Null_Exception);}* _temp2195;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp2196= yyvs; struct _xenum_struct** _temp2198= _temp2196.curr + yyvsp_offset;
if( _temp2198 < _temp2196.base? 1: _temp2198 >= _temp2196.last_plus_one){ _throw(
Null_Exception);}* _temp2198;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2199= yyls; struct Cyc_Yyltype* _temp2201= _temp2199.curr +(
yylsp_offset - 2); if( _temp2201 < _temp2199.base? 1: _temp2201 >= _temp2199.last_plus_one){
_throw( Null_Exception);}* _temp2201;}).first_line,({ struct _tagged_ptr3
_temp2202= yyls; struct Cyc_Yyltype* _temp2204= _temp2202.curr + yylsp_offset;
if( _temp2204 < _temp2202.base? 1: _temp2204 >= _temp2202.last_plus_one){ _throw(
Null_Exception);}* _temp2204;}).last_line));( struct _xenum_struct*) _temp2189;});
break; case 16: _LL2188: yyval=({ struct Cyc_FnDecl_tok_struct* _temp2206=(
struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2206->tag= Cyc_FnDecl_tok_tag; _temp2206->f1= Cyc_Parse_make_function(({
struct Cyc_Core_Opt* _temp2207=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2207->v=( void*) Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2208= yyvs; struct _xenum_struct** _temp2210= _temp2208.curr +(
yyvsp_offset - 3); if( _temp2210 < _temp2208.base? 1: _temp2210 >= _temp2208.last_plus_one){
_throw( Null_Exception);}* _temp2210;})); _temp2207;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2211= yyvs; struct _xenum_struct** _temp2213= _temp2211.curr
+( yyvsp_offset - 2); if( _temp2213 < _temp2211.base? 1: _temp2213 >= _temp2211.last_plus_one){
_throw( Null_Exception);}* _temp2213;})), Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp2214= yyvs; struct _xenum_struct** _temp2216= _temp2214.curr +(
yyvsp_offset - 1); if( _temp2216 < _temp2214.base? 1: _temp2216 >= _temp2214.last_plus_one){
_throw( Null_Exception);}* _temp2216;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp2217= yyvs; struct _xenum_struct** _temp2219= _temp2217.curr +
yyvsp_offset; if( _temp2219 < _temp2217.base? 1: _temp2219 >= _temp2217.last_plus_one){
_throw( Null_Exception);}* _temp2219;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2220= yyls; struct Cyc_Yyltype* _temp2222= _temp2220.curr +(
yylsp_offset - 3); if( _temp2222 < _temp2220.base? 1: _temp2222 >= _temp2220.last_plus_one){
_throw( Null_Exception);}* _temp2222;}).first_line,({ struct _tagged_ptr3
_temp2223= yyls; struct Cyc_Yyltype* _temp2225= _temp2223.curr + yylsp_offset;
if( _temp2225 < _temp2223.base? 1: _temp2225 >= _temp2223.last_plus_one){ _throw(
Null_Exception);}* _temp2225;}).last_line));( struct _xenum_struct*) _temp2206;});
break; case 17: _LL2205: yyval=({ struct Cyc_FnDecl_tok_struct* _temp2227=(
struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2227->tag= Cyc_FnDecl_tok_tag; _temp2227->f1= Cyc_Parse_make_function(({
struct Cyc_Core_Opt* _temp2228=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2228->v=( void*) Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2229= yyvs; struct _xenum_struct** _temp2231= _temp2229.curr +(
yyvsp_offset - 2); if( _temp2231 < _temp2229.base? 1: _temp2231 >= _temp2229.last_plus_one){
_throw( Null_Exception);}* _temp2231;})); _temp2228;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2232= yyvs; struct _xenum_struct** _temp2234= _temp2232.curr
+( yyvsp_offset - 1); if( _temp2234 < _temp2232.base? 1: _temp2234 >= _temp2232.last_plus_one){
_throw( Null_Exception);}* _temp2234;})), 0, Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp2235= yyvs; struct _xenum_struct** _temp2237= _temp2235.curr +
yyvsp_offset; if( _temp2237 < _temp2235.base? 1: _temp2237 >= _temp2235.last_plus_one){
_throw( Null_Exception);}* _temp2237;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2238= yyls; struct Cyc_Yyltype* _temp2240= _temp2238.curr +(
yylsp_offset - 2); if( _temp2240 < _temp2238.base? 1: _temp2240 >= _temp2238.last_plus_one){
_throw( Null_Exception);}* _temp2240;}).first_line,({ struct _tagged_ptr3
_temp2241= yyls; struct Cyc_Yyltype* _temp2243= _temp2241.curr + yylsp_offset;
if( _temp2243 < _temp2241.base? 1: _temp2243 >= _temp2241.last_plus_one){ _throw(
Null_Exception);}* _temp2243;}).last_line));( struct _xenum_struct*) _temp2227;});
break; case 18: _LL2226: yyval=({ struct Cyc_FnDecl_tok_struct* _temp2245=(
struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));
_temp2245->tag= Cyc_FnDecl_tok_tag; _temp2245->f1= Cyc_Parse_make_function(({
struct Cyc_Core_Opt* _temp2246=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2246->v=( void*) Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2247= yyvs; struct _xenum_struct** _temp2249= _temp2247.curr +(
yyvsp_offset - 3); if( _temp2249 < _temp2247.base? 1: _temp2249 >= _temp2247.last_plus_one){
_throw( Null_Exception);}* _temp2249;})); _temp2246;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2250= yyvs; struct _xenum_struct** _temp2252= _temp2250.curr
+( yyvsp_offset - 2); if( _temp2252 < _temp2250.base? 1: _temp2252 >= _temp2250.last_plus_one){
_throw( Null_Exception);}* _temp2252;})), Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp2253= yyvs; struct _xenum_struct** _temp2255= _temp2253.curr +(
yyvsp_offset - 1); if( _temp2255 < _temp2253.base? 1: _temp2255 >= _temp2253.last_plus_one){
_throw( Null_Exception);}* _temp2255;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp2256= yyvs; struct _xenum_struct** _temp2258= _temp2256.curr +
yyvsp_offset; if( _temp2258 < _temp2256.base? 1: _temp2258 >= _temp2256.last_plus_one){
_throw( Null_Exception);}* _temp2258;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2259= yyls; struct Cyc_Yyltype* _temp2261= _temp2259.curr +(
yylsp_offset - 3); if( _temp2261 < _temp2259.base? 1: _temp2261 >= _temp2259.last_plus_one){
_throw( Null_Exception);}* _temp2261;}).first_line,({ struct _tagged_ptr3
_temp2262= yyls; struct Cyc_Yyltype* _temp2264= _temp2262.curr + yylsp_offset;
if( _temp2264 < _temp2262.base? 1: _temp2264 >= _temp2262.last_plus_one){ _throw(
Null_Exception);}* _temp2264;}).last_line));( struct _xenum_struct*) _temp2245;});
break; case 19: _LL2244: Cyc_Lex_enter_using( Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp2266= yyvs; struct _xenum_struct** _temp2268= _temp2266.curr +
yyvsp_offset; if( _temp2268 < _temp2266.base? 1: _temp2268 >= _temp2266.last_plus_one){
_throw( Null_Exception);}* _temp2268;}))); yyval=({ struct _tagged_ptr2
_temp2269= yyvs; struct _xenum_struct** _temp2271= _temp2269.curr + yyvsp_offset;
if( _temp2271 < _temp2269.base? 1: _temp2271 >= _temp2269.last_plus_one){ _throw(
Null_Exception);}* _temp2271;}); break; case 20: _LL2265: Cyc_Lex_leave_using();
break; case 21: _LL2272: Cyc_Lex_enter_namespace(({ struct _tagged_string*
_temp2274=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp2274[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2275= yyvs;
struct _xenum_struct** _temp2277= _temp2275.curr + yyvsp_offset; if( _temp2277 <
_temp2275.base? 1: _temp2277 >= _temp2275.last_plus_one){ _throw( Null_Exception);}*
_temp2277;})); _temp2274;})); yyval=({ struct _tagged_ptr2 _temp2278= yyvs;
struct _xenum_struct** _temp2280= _temp2278.curr + yyvsp_offset; if( _temp2280 <
_temp2278.base? 1: _temp2280 >= _temp2278.last_plus_one){ _throw( Null_Exception);}*
_temp2280;}); break; case 22: _LL2273: Cyc_Lex_leave_namespace(); break; case 23:
_LL2281: yyval=({ struct Cyc_DeclList_tok_struct* _temp2283=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2283->tag= Cyc_DeclList_tok_tag;
_temp2283->f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2284= yyvs; struct _xenum_struct** _temp2286= _temp2284.curr +(
yyvsp_offset - 1); if( _temp2286 < _temp2284.base? 1: _temp2286 >= _temp2284.last_plus_one){
_throw( Null_Exception);}* _temp2286;})), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2287= yyls; struct Cyc_Yyltype* _temp2289= _temp2287.curr
+( yylsp_offset - 1); if( _temp2289 < _temp2287.base? 1: _temp2289 >= _temp2287.last_plus_one){
_throw( Null_Exception);}* _temp2289;}).first_line,({ struct _tagged_ptr3
_temp2290= yyls; struct Cyc_Yyltype* _temp2292= _temp2290.curr +( yylsp_offset -
1); if( _temp2292 < _temp2290.base? 1: _temp2292 >= _temp2290.last_plus_one){
_throw( Null_Exception);}* _temp2292;}).last_line));( struct _xenum_struct*)
_temp2283;}); break; case 24: _LL2282: yyval=({ struct Cyc_DeclList_tok_struct*
_temp2294=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2294->tag= Cyc_DeclList_tok_tag; _temp2294->f1= Cyc_Parse_make_declarations(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2295= yyvs; struct
_xenum_struct** _temp2297= _temp2295.curr +( yyvsp_offset - 2); if( _temp2297 <
_temp2295.base? 1: _temp2297 >= _temp2295.last_plus_one){ _throw( Null_Exception);}*
_temp2297;})), Cyc_yyget_InitDeclList_tok(({ struct _tagged_ptr2 _temp2298= yyvs;
struct _xenum_struct** _temp2300= _temp2298.curr +( yyvsp_offset - 1); if(
_temp2300 < _temp2298.base? 1: _temp2300 >= _temp2298.last_plus_one){ _throw(
Null_Exception);}* _temp2300;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2301= yyls; struct Cyc_Yyltype* _temp2303= _temp2301.curr +(
yylsp_offset - 2); if( _temp2303 < _temp2301.base? 1: _temp2303 >= _temp2301.last_plus_one){
_throw( Null_Exception);}* _temp2303;}).first_line,({ struct _tagged_ptr3
_temp2304= yyls; struct Cyc_Yyltype* _temp2306= _temp2304.curr + yylsp_offset;
if( _temp2306 < _temp2304.base? 1: _temp2306 >= _temp2304.last_plus_one){ _throw(
Null_Exception);}* _temp2306;}).last_line));( struct _xenum_struct*) _temp2294;});
break; case 25: _LL2293: yyval=({ struct Cyc_DeclList_tok_struct* _temp2308=(
struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));
_temp2308->tag= Cyc_DeclList_tok_tag; _temp2308->f1=({ struct Cyc_List_List*
_temp2309=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2309->hd=( void*) Cyc_Absyn_let_decl( Cyc_yyget_Pattern_tok(({ struct
_tagged_ptr2 _temp2310= yyvs; struct _xenum_struct** _temp2312= _temp2310.curr +(
yyvsp_offset - 3); if( _temp2312 < _temp2310.base? 1: _temp2312 >= _temp2310.last_plus_one){
_throw( Null_Exception);}* _temp2312;})), 0, Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp2313= yyvs; struct _xenum_struct** _temp2315= _temp2313.curr +(
yyvsp_offset - 1); if( _temp2315 < _temp2313.base? 1: _temp2315 >= _temp2313.last_plus_one){
_throw( Null_Exception);}* _temp2315;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2316= yyls; struct Cyc_Yyltype* _temp2318= _temp2316.curr +(
yylsp_offset - 4); if( _temp2318 < _temp2316.base? 1: _temp2318 >= _temp2316.last_plus_one){
_throw( Null_Exception);}* _temp2318;}).first_line,({ struct _tagged_ptr3
_temp2319= yyls; struct Cyc_Yyltype* _temp2321= _temp2319.curr + yylsp_offset;
if( _temp2321 < _temp2319.base? 1: _temp2321 >= _temp2319.last_plus_one){ _throw(
Null_Exception);}* _temp2321;}).last_line)); _temp2309->tl= 0; _temp2309;});(
struct _xenum_struct*) _temp2308;}); break; case 26: _LL2307: yyval=({ struct
_tagged_ptr2 _temp2323= yyvs; struct _xenum_struct** _temp2325= _temp2323.curr +
yyvsp_offset; if( _temp2325 < _temp2323.base? 1: _temp2325 >= _temp2323.last_plus_one){
_throw( Null_Exception);}* _temp2325;}); break; case 27: _LL2322: yyval=({
struct Cyc_DeclList_tok_struct* _temp2327=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct)); _temp2327->tag= Cyc_DeclList_tok_tag;
_temp2327->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2
_temp2328= yyvs; struct _xenum_struct** _temp2330= _temp2328.curr +(
yyvsp_offset - 1); if( _temp2330 < _temp2328.base? 1: _temp2330 >= _temp2328.last_plus_one){
_throw( Null_Exception);}* _temp2330;})), Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp2331= yyvs; struct _xenum_struct** _temp2333= _temp2331.curr +
yyvsp_offset; if( _temp2333 < _temp2331.base? 1: _temp2333 >= _temp2331.last_plus_one){
_throw( Null_Exception);}* _temp2333;})));( struct _xenum_struct*) _temp2327;});
break; case 28: _LL2326: yyval=({ struct Cyc_DeclSpec_tok_struct* _temp2335=(
struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2335->tag= Cyc_DeclSpec_tok_tag; _temp2335->f1=({ struct Cyc_Parse_Declaration_spec*
_temp2336=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2336->sc=({ struct Cyc_Core_Opt* _temp2340=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2340->v=( void*) Cyc_yyget_StorageClass_tok(({
struct _tagged_ptr2 _temp2341= yyvs; struct _xenum_struct** _temp2343= _temp2341.curr
+( yyvsp_offset - 1); if( _temp2343 < _temp2341.base? 1: _temp2343 >= _temp2341.last_plus_one){
_throw( Null_Exception);}* _temp2343;})); _temp2340;}); _temp2336->tq= Cyc_Absyn_empty_tqual();
_temp2336->type_specs= 0; _temp2336->is_inline= 0; _temp2336->attributes= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2337= yyvs; struct _xenum_struct** _temp2339= _temp2337.curr
+ yyvsp_offset; if( _temp2339 < _temp2337.base? 1: _temp2339 >= _temp2337.last_plus_one){
_throw( Null_Exception);}* _temp2339;})); _temp2336;});( struct _xenum_struct*)
_temp2335;}); break; case 29: _LL2334: if(( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2345= yyvs; struct _xenum_struct** _temp2347= _temp2345.curr +
yyvsp_offset; if( _temp2347 < _temp2345.base? 1: _temp2347 >= _temp2345.last_plus_one){
_throw( Null_Exception);}* _temp2347;})))->sc != 0){ Cyc_Parse_warn(( struct
_tagged_string)({ char* _temp2348=( char*)"Only one storage class is allowed in a declaration";
struct _tagged_string _temp2349; _temp2349.curr= _temp2348; _temp2349.base=
_temp2348; _temp2349.last_plus_one= _temp2348 + 51; _temp2349;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2350= yyls; struct Cyc_Yyltype* _temp2352= _temp2350.curr
+( yylsp_offset - 2); if( _temp2352 < _temp2350.base? 1: _temp2352 >= _temp2350.last_plus_one){
_throw( Null_Exception);}* _temp2352;}).first_line,({ struct _tagged_ptr3
_temp2353= yyls; struct Cyc_Yyltype* _temp2355= _temp2353.curr +( yylsp_offset -
1); if( _temp2355 < _temp2353.base? 1: _temp2355 >= _temp2353.last_plus_one){
_throw( Null_Exception);}* _temp2355;}).last_line));} yyval=({ struct Cyc_DeclSpec_tok_struct*
_temp2356=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2356->tag= Cyc_DeclSpec_tok_tag; _temp2356->f1=({ struct Cyc_Parse_Declaration_spec*
_temp2357=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2357->sc=({ struct Cyc_Core_Opt* _temp2373=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2373->v=( void*) Cyc_yyget_StorageClass_tok(({
struct _tagged_ptr2 _temp2374= yyvs; struct _xenum_struct** _temp2376= _temp2374.curr
+( yyvsp_offset - 2); if( _temp2376 < _temp2374.base? 1: _temp2376 >= _temp2374.last_plus_one){
_throw( Null_Exception);}* _temp2376;})); _temp2373;}); _temp2357->tq=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2370= yyvs; struct _xenum_struct** _temp2372= _temp2370.curr
+ yyvsp_offset; if( _temp2372 < _temp2370.base? 1: _temp2372 >= _temp2370.last_plus_one){
_throw( Null_Exception);}* _temp2372;})))->tq; _temp2357->type_specs=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2367= yyvs; struct _xenum_struct** _temp2369= _temp2367.curr
+ yyvsp_offset; if( _temp2369 < _temp2367.base? 1: _temp2369 >= _temp2367.last_plus_one){
_throw( Null_Exception);}* _temp2369;})))->type_specs; _temp2357->is_inline=(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2364= yyvs; struct
_xenum_struct** _temp2366= _temp2364.curr + yyvsp_offset; if( _temp2366 <
_temp2364.base? 1: _temp2366 >= _temp2364.last_plus_one){ _throw( Null_Exception);}*
_temp2366;})))->is_inline; _temp2357->attributes=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2358= yyvs; struct _xenum_struct** _temp2360= _temp2358.curr
+( yyvsp_offset - 1); if( _temp2360 < _temp2358.base? 1: _temp2360 >= _temp2358.last_plus_one){
_throw( Null_Exception);}* _temp2360;})),( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2361= yyvs; struct _xenum_struct** _temp2363= _temp2361.curr +
yyvsp_offset; if( _temp2363 < _temp2361.base? 1: _temp2363 >= _temp2361.last_plus_one){
_throw( Null_Exception);}* _temp2363;})))->attributes); _temp2357;});( struct
_xenum_struct*) _temp2356;}); break; case 30: _LL2344: yyval=({ struct Cyc_DeclSpec_tok_struct*
_temp2378=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2378->tag= Cyc_DeclSpec_tok_tag; _temp2378->f1=({ struct Cyc_Parse_Declaration_spec*
_temp2379=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2379->sc= 0; _temp2379->tq= Cyc_Absyn_empty_tqual(); _temp2379->type_specs=({
struct Cyc_List_List* _temp2383=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2383->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({
struct _tagged_ptr2 _temp2384= yyvs; struct _xenum_struct** _temp2386= _temp2384.curr
+( yyvsp_offset - 1); if( _temp2386 < _temp2384.base? 1: _temp2386 >= _temp2384.last_plus_one){
_throw( Null_Exception);}* _temp2386;})); _temp2383->tl= 0; _temp2383;});
_temp2379->is_inline= 0; _temp2379->attributes= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2380= yyvs; struct _xenum_struct** _temp2382= _temp2380.curr
+ yyvsp_offset; if( _temp2382 < _temp2380.base? 1: _temp2382 >= _temp2380.last_plus_one){
_throw( Null_Exception);}* _temp2382;})); _temp2379;});( struct _xenum_struct*)
_temp2378;}); break; case 31: _LL2377: yyval=({ struct Cyc_DeclSpec_tok_struct*
_temp2388=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2388->tag= Cyc_DeclSpec_tok_tag; _temp2388->f1=({ struct Cyc_Parse_Declaration_spec*
_temp2389=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2389->sc=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2409= yyvs;
struct _xenum_struct** _temp2411= _temp2409.curr + yyvsp_offset; if( _temp2411 <
_temp2409.base? 1: _temp2411 >= _temp2409.last_plus_one){ _throw( Null_Exception);}*
_temp2411;})))->sc; _temp2389->tq=( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2406= yyvs; struct _xenum_struct** _temp2408= _temp2406.curr +
yyvsp_offset; if( _temp2408 < _temp2406.base? 1: _temp2408 >= _temp2406.last_plus_one){
_throw( Null_Exception);}* _temp2408;})))->tq; _temp2389->type_specs=({ struct
Cyc_List_List* _temp2399=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2399->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({ struct _tagged_ptr2
_temp2403= yyvs; struct _xenum_struct** _temp2405= _temp2403.curr +(
yyvsp_offset - 2); if( _temp2405 < _temp2403.base? 1: _temp2405 >= _temp2403.last_plus_one){
_throw( Null_Exception);}* _temp2405;})); _temp2399->tl=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2400= yyvs; struct _xenum_struct** _temp2402= _temp2400.curr
+ yyvsp_offset; if( _temp2402 < _temp2400.base? 1: _temp2402 >= _temp2400.last_plus_one){
_throw( Null_Exception);}* _temp2402;})))->type_specs; _temp2399;}); _temp2389->is_inline=(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2396= yyvs; struct
_xenum_struct** _temp2398= _temp2396.curr + yyvsp_offset; if( _temp2398 <
_temp2396.base? 1: _temp2398 >= _temp2396.last_plus_one){ _throw( Null_Exception);}*
_temp2398;})))->is_inline; _temp2389->attributes=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2390= yyvs; struct _xenum_struct** _temp2392= _temp2390.curr
+( yyvsp_offset - 1); if( _temp2392 < _temp2390.base? 1: _temp2392 >= _temp2390.last_plus_one){
_throw( Null_Exception);}* _temp2392;})),( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2393= yyvs; struct _xenum_struct** _temp2395= _temp2393.curr +
yyvsp_offset; if( _temp2395 < _temp2393.base? 1: _temp2395 >= _temp2393.last_plus_one){
_throw( Null_Exception);}* _temp2395;})))->attributes); _temp2389;});( struct
_xenum_struct*) _temp2388;}); break; case 32: _LL2387: yyval=({ struct Cyc_DeclSpec_tok_struct*
_temp2413=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2413->tag= Cyc_DeclSpec_tok_tag; _temp2413->f1=({ struct Cyc_Parse_Declaration_spec*
_temp2414=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2414->sc= 0; _temp2414->tq= Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2
_temp2418= yyvs; struct _xenum_struct** _temp2420= _temp2418.curr +(
yyvsp_offset - 1); if( _temp2420 < _temp2418.base? 1: _temp2420 >= _temp2418.last_plus_one){
_throw( Null_Exception);}* _temp2420;})); _temp2414->type_specs= 0; _temp2414->is_inline=
0; _temp2414->attributes= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2
_temp2415= yyvs; struct _xenum_struct** _temp2417= _temp2415.curr + yyvsp_offset;
if( _temp2417 < _temp2415.base? 1: _temp2417 >= _temp2415.last_plus_one){ _throw(
Null_Exception);}* _temp2417;})); _temp2414;});( struct _xenum_struct*)
_temp2413;}); break; case 33: _LL2412: yyval=({ struct Cyc_DeclSpec_tok_struct*
_temp2422=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2422->tag= Cyc_DeclSpec_tok_tag; _temp2422->f1=({ struct Cyc_Parse_Declaration_spec*
_temp2423=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2423->sc=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2442= yyvs;
struct _xenum_struct** _temp2444= _temp2442.curr + yyvsp_offset; if( _temp2444 <
_temp2442.base? 1: _temp2444 >= _temp2442.last_plus_one){ _throw( Null_Exception);}*
_temp2444;})))->sc; _temp2423->tq= Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok(({
struct _tagged_ptr2 _temp2436= yyvs; struct _xenum_struct** _temp2438= _temp2436.curr
+( yyvsp_offset - 2); if( _temp2438 < _temp2436.base? 1: _temp2438 >= _temp2436.last_plus_one){
_throw( Null_Exception);}* _temp2438;})),( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2439= yyvs; struct _xenum_struct** _temp2441= _temp2439.curr +
yyvsp_offset; if( _temp2441 < _temp2439.base? 1: _temp2441 >= _temp2439.last_plus_one){
_throw( Null_Exception);}* _temp2441;})))->tq); _temp2423->type_specs=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2433= yyvs; struct _xenum_struct** _temp2435= _temp2433.curr
+ yyvsp_offset; if( _temp2435 < _temp2433.base? 1: _temp2435 >= _temp2433.last_plus_one){
_throw( Null_Exception);}* _temp2435;})))->type_specs; _temp2423->is_inline=(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2430= yyvs; struct
_xenum_struct** _temp2432= _temp2430.curr + yyvsp_offset; if( _temp2432 <
_temp2430.base? 1: _temp2432 >= _temp2430.last_plus_one){ _throw( Null_Exception);}*
_temp2432;})))->is_inline; _temp2423->attributes=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2424= yyvs; struct _xenum_struct** _temp2426= _temp2424.curr
+( yyvsp_offset - 1); if( _temp2426 < _temp2424.base? 1: _temp2426 >= _temp2424.last_plus_one){
_throw( Null_Exception);}* _temp2426;})),( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2427= yyvs; struct _xenum_struct** _temp2429= _temp2427.curr +
yyvsp_offset; if( _temp2429 < _temp2427.base? 1: _temp2429 >= _temp2427.last_plus_one){
_throw( Null_Exception);}* _temp2429;})))->attributes); _temp2423;});( struct
_xenum_struct*) _temp2422;}); break; case 34: _LL2421: yyval=({ struct Cyc_DeclSpec_tok_struct*
_temp2446=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2446->tag= Cyc_DeclSpec_tok_tag; _temp2446->f1=({ struct Cyc_Parse_Declaration_spec*
_temp2447=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2447->sc= 0; _temp2447->tq= Cyc_Absyn_empty_tqual(); _temp2447->type_specs=
0; _temp2447->is_inline= 1; _temp2447->attributes= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2448= yyvs; struct _xenum_struct** _temp2450= _temp2448.curr
+ yyvsp_offset; if( _temp2450 < _temp2448.base? 1: _temp2450 >= _temp2448.last_plus_one){
_throw( Null_Exception);}* _temp2450;})); _temp2447;});( struct _xenum_struct*)
_temp2446;}); break; case 35: _LL2445: yyval=({ struct Cyc_DeclSpec_tok_struct*
_temp2452=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));
_temp2452->tag= Cyc_DeclSpec_tok_tag; _temp2452->f1=({ struct Cyc_Parse_Declaration_spec*
_temp2453=( struct Cyc_Parse_Declaration_spec*) GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec));
_temp2453->sc=( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2466= yyvs;
struct _xenum_struct** _temp2468= _temp2466.curr + yyvsp_offset; if( _temp2468 <
_temp2466.base? 1: _temp2468 >= _temp2466.last_plus_one){ _throw( Null_Exception);}*
_temp2468;})))->sc; _temp2453->tq=( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2463= yyvs; struct _xenum_struct** _temp2465= _temp2463.curr +
yyvsp_offset; if( _temp2465 < _temp2463.base? 1: _temp2465 >= _temp2463.last_plus_one){
_throw( Null_Exception);}* _temp2465;})))->tq; _temp2453->type_specs=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2460= yyvs; struct _xenum_struct** _temp2462= _temp2460.curr
+ yyvsp_offset; if( _temp2462 < _temp2460.base? 1: _temp2462 >= _temp2460.last_plus_one){
_throw( Null_Exception);}* _temp2462;})))->type_specs; _temp2453->is_inline= 1;
_temp2453->attributes=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2454= yyvs; struct _xenum_struct** _temp2456= _temp2454.curr
+( yyvsp_offset - 1); if( _temp2456 < _temp2454.base? 1: _temp2456 >= _temp2454.last_plus_one){
_throw( Null_Exception);}* _temp2456;})),( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2457= yyvs; struct _xenum_struct** _temp2459= _temp2457.curr +
yyvsp_offset; if( _temp2459 < _temp2457.base? 1: _temp2459 >= _temp2457.last_plus_one){
_throw( Null_Exception);}* _temp2459;})))->attributes); _temp2453;});( struct
_xenum_struct*) _temp2452;}); break; case 36: _LL2451: yyval=({ struct Cyc_StorageClass_tok_struct*
_temp2470=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2470->tag= Cyc_StorageClass_tok_tag; _temp2470->f1=( void*) Cyc_Parse_Auto_sc;(
struct _xenum_struct*) _temp2470;}); break; case 37: _LL2469: yyval=({ struct
Cyc_StorageClass_tok_struct* _temp2472=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp2472->tag= Cyc_StorageClass_tok_tag;
_temp2472->f1=( void*) Cyc_Parse_Register_sc;( struct _xenum_struct*) _temp2472;});
break; case 38: _LL2471: yyval=({ struct Cyc_StorageClass_tok_struct* _temp2474=(
struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2474->tag= Cyc_StorageClass_tok_tag; _temp2474->f1=( void*) Cyc_Parse_Static_sc;(
struct _xenum_struct*) _temp2474;}); break; case 39: _LL2473: yyval=({ struct
Cyc_StorageClass_tok_struct* _temp2476=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp2476->tag= Cyc_StorageClass_tok_tag;
_temp2476->f1=( void*) Cyc_Parse_Extern_sc;( struct _xenum_struct*) _temp2476;});
break; case 40: _LL2475: if( Cyc_String_strcmp( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp2478= yyvs; struct _xenum_struct** _temp2480= _temp2478.curr +
yyvsp_offset; if( _temp2480 < _temp2478.base? 1: _temp2480 >= _temp2478.last_plus_one){
_throw( Null_Exception);}* _temp2480;})),( struct _tagged_string)({ char*
_temp2481=( char*)"C"; struct _tagged_string _temp2482; _temp2482.curr=
_temp2481; _temp2482.base= _temp2481; _temp2482.last_plus_one= _temp2481 + 2;
_temp2482;})) != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp2483=(
char*)"only extern or extern \"C\" is allowed"; struct _tagged_string _temp2484;
_temp2484.curr= _temp2483; _temp2484.base= _temp2483; _temp2484.last_plus_one=
_temp2483 + 37; _temp2484;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2485= yyls; struct Cyc_Yyltype* _temp2487= _temp2485.curr +( yylsp_offset -
1); if( _temp2487 < _temp2485.base? 1: _temp2487 >= _temp2485.last_plus_one){
_throw( Null_Exception);}* _temp2487;}).first_line,({ struct _tagged_ptr3
_temp2488= yyls; struct Cyc_Yyltype* _temp2490= _temp2488.curr + yylsp_offset;
if( _temp2490 < _temp2488.base? 1: _temp2490 >= _temp2488.last_plus_one){ _throw(
Null_Exception);}* _temp2490;}).last_line));} yyval=({ struct Cyc_StorageClass_tok_struct*
_temp2491=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2491->tag= Cyc_StorageClass_tok_tag; _temp2491->f1=( void*) Cyc_Parse_ExternC_sc;(
struct _xenum_struct*) _temp2491;}); break; case 41: _LL2477: yyval=({ struct
Cyc_StorageClass_tok_struct* _temp2493=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct)); _temp2493->tag= Cyc_StorageClass_tok_tag;
_temp2493->f1=( void*) Cyc_Parse_Typedef_sc;( struct _xenum_struct*) _temp2493;});
break; case 42: _LL2492: yyval=({ struct Cyc_StorageClass_tok_struct* _temp2495=(
struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));
_temp2495->tag= Cyc_StorageClass_tok_tag; _temp2495->f1=( void*) Cyc_Parse_Abstract_sc;(
struct _xenum_struct*) _temp2495;}); break; case 43: _LL2494: yyval=({ struct
Cyc_AttributeList_tok_struct* _temp2497=( struct Cyc_AttributeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct)); _temp2497->tag= Cyc_AttributeList_tok_tag;
_temp2497->f1= 0;( struct _xenum_struct*) _temp2497;}); break; case 44: _LL2496:
yyval=({ struct _tagged_ptr2 _temp2499= yyvs; struct _xenum_struct** _temp2501=
_temp2499.curr + yyvsp_offset; if( _temp2501 < _temp2499.base? 1: _temp2501 >=
_temp2499.last_plus_one){ _throw( Null_Exception);}* _temp2501;}); break; case
45: _LL2498: yyval=({ struct Cyc_AttributeList_tok_struct* _temp2503=( struct
Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2503->tag= Cyc_AttributeList_tok_tag; _temp2503->f1= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2504= yyvs; struct _xenum_struct** _temp2506= _temp2504.curr
+( yyvsp_offset - 2); if( _temp2506 < _temp2504.base? 1: _temp2506 >= _temp2504.last_plus_one){
_throw( Null_Exception);}* _temp2506;}));( struct _xenum_struct*) _temp2503;});
break; case 46: _LL2502: yyval=({ struct Cyc_AttributeList_tok_struct* _temp2508=(
struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2508->tag= Cyc_AttributeList_tok_tag; _temp2508->f1=({ struct Cyc_List_List*
_temp2509=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2509->hd=( void*) Cyc_yyget_Attribute_tok(({ struct _tagged_ptr2 _temp2510=
yyvs; struct _xenum_struct** _temp2512= _temp2510.curr + yyvsp_offset; if(
_temp2512 < _temp2510.base? 1: _temp2512 >= _temp2510.last_plus_one){ _throw(
Null_Exception);}* _temp2512;})); _temp2509->tl= 0; _temp2509;});( struct
_xenum_struct*) _temp2508;}); break; case 47: _LL2507: yyval=({ struct Cyc_AttributeList_tok_struct*
_temp2514=( struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));
_temp2514->tag= Cyc_AttributeList_tok_tag; _temp2514->f1=({ struct Cyc_List_List*
_temp2515=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2515->hd=( void*) Cyc_yyget_Attribute_tok(({ struct _tagged_ptr2 _temp2519=
yyvs; struct _xenum_struct** _temp2521= _temp2519.curr +( yyvsp_offset - 2); if(
_temp2521 < _temp2519.base? 1: _temp2521 >= _temp2519.last_plus_one){ _throw(
Null_Exception);}* _temp2521;})); _temp2515->tl= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2516= yyvs; struct _xenum_struct** _temp2518= _temp2516.curr
+ yyvsp_offset; if( _temp2518 < _temp2516.base? 1: _temp2518 >= _temp2516.last_plus_one){
_throw( Null_Exception);}* _temp2518;})); _temp2515;});( struct _xenum_struct*)
_temp2514;}); break; case 48: _LL2513: { struct _tagged_string s= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp2523= yyvs; struct _xenum_struct** _temp2525= _temp2523.curr
+ yyvsp_offset; if( _temp2525 < _temp2523.base? 1: _temp2525 >= _temp2523.last_plus_one){
_throw( Null_Exception);}* _temp2525;})); void* a; if( Cyc_String_zstrcmp( s,(
struct _tagged_string)({ char* _temp2526=( char*)"stdcall"; struct
_tagged_string _temp2527; _temp2527.curr= _temp2526; _temp2527.base= _temp2526;
_temp2527.last_plus_one= _temp2526 + 8; _temp2527;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2528=( char*)"__stdcall__"; struct
_tagged_string _temp2529; _temp2529.curr= _temp2528; _temp2529.base= _temp2528;
_temp2529.last_plus_one= _temp2528 + 12; _temp2529;})) == 0){ a= Cyc_Absyn_Stdcall_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2530=(
char*)"cdecl"; struct _tagged_string _temp2531; _temp2531.curr= _temp2530;
_temp2531.base= _temp2530; _temp2531.last_plus_one= _temp2530 + 6; _temp2531;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2532=( char*)"__cdecl__";
struct _tagged_string _temp2533; _temp2533.curr= _temp2532; _temp2533.base=
_temp2532; _temp2533.last_plus_one= _temp2532 + 10; _temp2533;})) == 0){ a= Cyc_Absyn_Cdecl_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2534=(
char*)"noreturn"; struct _tagged_string _temp2535; _temp2535.curr= _temp2534;
_temp2535.base= _temp2534; _temp2535.last_plus_one= _temp2534 + 9; _temp2535;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2536=( char*)"__noreturn__";
struct _tagged_string _temp2537; _temp2537.curr= _temp2536; _temp2537.base=
_temp2536; _temp2537.last_plus_one= _temp2536 + 13; _temp2537;})) == 0){ a= Cyc_Absyn_Noreturn_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2538=(
char*)"noreturn"; struct _tagged_string _temp2539; _temp2539.curr= _temp2538;
_temp2539.base= _temp2538; _temp2539.last_plus_one= _temp2538 + 9; _temp2539;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2540=( char*)"__noreturn__";
struct _tagged_string _temp2541; _temp2541.curr= _temp2540; _temp2541.base=
_temp2540; _temp2541.last_plus_one= _temp2540 + 13; _temp2541;})) == 0){ a= Cyc_Absyn_Noreturn_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2542=(
char*)"__const__"; struct _tagged_string _temp2543; _temp2543.curr= _temp2542;
_temp2543.base= _temp2542; _temp2543.last_plus_one= _temp2542 + 10; _temp2543;}))
== 0){ a= Cyc_Absyn_Const_att;} else{ if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2544=( char*)"aligned"; struct _tagged_string
_temp2545; _temp2545.curr= _temp2544; _temp2545.base= _temp2544; _temp2545.last_plus_one=
_temp2544 + 8; _temp2545;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2546=( char*)"__aligned__"; struct _tagged_string
_temp2547; _temp2547.curr= _temp2546; _temp2547.base= _temp2546; _temp2547.last_plus_one=
_temp2546 + 12; _temp2547;})) == 0){ a=({ struct Cyc_Absyn_Aligned_att_struct*
_temp2548=( struct Cyc_Absyn_Aligned_att_struct*) GC_malloc_atomic( sizeof(
struct Cyc_Absyn_Aligned_att_struct)); _temp2548->tag= Cyc_Absyn_Aligned_att_tag;
_temp2548->f1= - 1;( void*) _temp2548;});} else{ if( Cyc_String_zstrcmp( s,(
struct _tagged_string)({ char* _temp2549=( char*)"packed"; struct _tagged_string
_temp2550; _temp2550.curr= _temp2549; _temp2550.base= _temp2549; _temp2550.last_plus_one=
_temp2549 + 7; _temp2550;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2551=( char*)"__packed__"; struct _tagged_string
_temp2552; _temp2552.curr= _temp2551; _temp2552.base= _temp2551; _temp2552.last_plus_one=
_temp2551 + 11; _temp2552;})) == 0){ a= Cyc_Absyn_Packed_att;} else{ if( Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2553=( char*)"shared"; struct
_tagged_string _temp2554; _temp2554.curr= _temp2553; _temp2554.base= _temp2553;
_temp2554.last_plus_one= _temp2553 + 7; _temp2554;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2555=( char*)"__shared__"; struct
_tagged_string _temp2556; _temp2556.curr= _temp2555; _temp2556.base= _temp2555;
_temp2556.last_plus_one= _temp2555 + 11; _temp2556;})) == 0){ a= Cyc_Absyn_Shared_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2557=(
char*)"unused"; struct _tagged_string _temp2558; _temp2558.curr= _temp2557;
_temp2558.base= _temp2557; _temp2558.last_plus_one= _temp2557 + 7; _temp2558;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2559=( char*)"__unused__";
struct _tagged_string _temp2560; _temp2560.curr= _temp2559; _temp2560.base=
_temp2559; _temp2560.last_plus_one= _temp2559 + 11; _temp2560;})) == 0){ a= Cyc_Absyn_Unused_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2561=(
char*)"weak"; struct _tagged_string _temp2562; _temp2562.curr= _temp2561;
_temp2562.base= _temp2561; _temp2562.last_plus_one= _temp2561 + 5; _temp2562;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2563=( char*)"__weak__";
struct _tagged_string _temp2564; _temp2564.curr= _temp2563; _temp2564.base=
_temp2563; _temp2564.last_plus_one= _temp2563 + 9; _temp2564;})) == 0){ a= Cyc_Absyn_Weak_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2565=(
char*)"dllimport"; struct _tagged_string _temp2566; _temp2566.curr= _temp2565;
_temp2566.base= _temp2565; _temp2566.last_plus_one= _temp2565 + 10; _temp2566;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2567=( char*)"__dllimport__";
struct _tagged_string _temp2568; _temp2568.curr= _temp2567; _temp2568.base=
_temp2567; _temp2568.last_plus_one= _temp2567 + 14; _temp2568;})) == 0){ a= Cyc_Absyn_Dllimport_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2569=(
char*)"dllexport"; struct _tagged_string _temp2570; _temp2570.curr= _temp2569;
_temp2570.base= _temp2569; _temp2570.last_plus_one= _temp2569 + 10; _temp2570;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2571=( char*)"__dllexport__";
struct _tagged_string _temp2572; _temp2572.curr= _temp2571; _temp2572.base=
_temp2571; _temp2572.last_plus_one= _temp2571 + 14; _temp2572;})) == 0){ a= Cyc_Absyn_Dllexport_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2573=(
char*)"no_instrument_function"; struct _tagged_string _temp2574; _temp2574.curr=
_temp2573; _temp2574.base= _temp2573; _temp2574.last_plus_one= _temp2573 + 23;
_temp2574;})) == 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char*
_temp2575=( char*)"__no_instrument_function__"; struct _tagged_string _temp2576;
_temp2576.curr= _temp2575; _temp2576.base= _temp2575; _temp2576.last_plus_one=
_temp2575 + 27; _temp2576;})) == 0){ a= Cyc_Absyn_No_instrument_function_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2577=(
char*)"constructor"; struct _tagged_string _temp2578; _temp2578.curr= _temp2577;
_temp2578.base= _temp2577; _temp2578.last_plus_one= _temp2577 + 12; _temp2578;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2579=( char*)"__constructor__";
struct _tagged_string _temp2580; _temp2580.curr= _temp2579; _temp2580.base=
_temp2579; _temp2580.last_plus_one= _temp2579 + 16; _temp2580;})) == 0){ a= Cyc_Absyn_Constructor_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2581=(
char*)"destructor"; struct _tagged_string _temp2582; _temp2582.curr= _temp2581;
_temp2582.base= _temp2581; _temp2582.last_plus_one= _temp2581 + 11; _temp2582;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2583=( char*)"__destructor__";
struct _tagged_string _temp2584; _temp2584.curr= _temp2583; _temp2584.base=
_temp2583; _temp2584.last_plus_one= _temp2583 + 15; _temp2584;})) == 0){ a= Cyc_Absyn_Destructor_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2585=(
char*)"no_check_memory_usage"; struct _tagged_string _temp2586; _temp2586.curr=
_temp2585; _temp2586.base= _temp2585; _temp2586.last_plus_one= _temp2585 + 22;
_temp2586;})) == 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char*
_temp2587=( char*)"__no_check_memory_usage__"; struct _tagged_string _temp2588;
_temp2588.curr= _temp2587; _temp2588.base= _temp2587; _temp2588.last_plus_one=
_temp2587 + 26; _temp2588;})) == 0){ a= Cyc_Absyn_No_check_memory_usage_att;}
else{ Cyc_Parse_err(( struct _tagged_string)({ char* _temp2589=( char*)"unrecognized attribute";
struct _tagged_string _temp2590; _temp2590.curr= _temp2589; _temp2590.base=
_temp2589; _temp2590.last_plus_one= _temp2589 + 23; _temp2590;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2591= yyls; struct Cyc_Yyltype* _temp2593= _temp2591.curr
+ yylsp_offset; if( _temp2593 < _temp2591.base? 1: _temp2593 >= _temp2591.last_plus_one){
_throw( Null_Exception);}* _temp2593;}).first_line,({ struct _tagged_ptr3
_temp2594= yyls; struct Cyc_Yyltype* _temp2596= _temp2594.curr + yylsp_offset;
if( _temp2596 < _temp2594.base? 1: _temp2596 >= _temp2594.last_plus_one){ _throw(
Null_Exception);}* _temp2596;}).last_line)); a= Cyc_Absyn_Cdecl_att;}}}}}}}}}}}}}}}}
yyval=({ struct Cyc_Attribute_tok_struct* _temp2597=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp2597->tag= Cyc_Attribute_tok_tag;
_temp2597->f1=( void*) a;( struct _xenum_struct*) _temp2597;}); break;} case 49:
_LL2522: yyval=({ struct Cyc_Attribute_tok_struct* _temp2599=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp2599->tag= Cyc_Attribute_tok_tag;
_temp2599->f1=( void*) Cyc_Absyn_Const_att;( struct _xenum_struct*) _temp2599;});
break; case 50: _LL2598: { struct _tagged_string s= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp2601= yyvs; struct _xenum_struct** _temp2603= _temp2601.curr
+( yyvsp_offset - 3); if( _temp2603 < _temp2601.base? 1: _temp2603 >= _temp2601.last_plus_one){
_throw( Null_Exception);}* _temp2603;})); struct _tuple11 _temp2609; int
_temp2610; void* _temp2612; struct _tuple11* _temp2607= Cyc_yyget_Int_tok(({
struct _tagged_ptr2 _temp2604= yyvs; struct _xenum_struct** _temp2606= _temp2604.curr
+( yyvsp_offset - 1); if( _temp2606 < _temp2604.base? 1: _temp2606 >= _temp2604.last_plus_one){
_throw( Null_Exception);}* _temp2606;})); _temp2609=* _temp2607; _LL2613:
_temp2612= _temp2609.f1; goto _LL2611; _LL2611: _temp2610= _temp2609.f2; goto
_LL2608; _LL2608: { void* a; if( Cyc_String_zstrcmp( s,( struct _tagged_string)({
char* _temp2614=( char*)"regparm"; struct _tagged_string _temp2615; _temp2615.curr=
_temp2614; _temp2615.base= _temp2614; _temp2615.last_plus_one= _temp2614 + 8;
_temp2615;})) == 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char*
_temp2616=( char*)"__regparm__"; struct _tagged_string _temp2617; _temp2617.curr=
_temp2616; _temp2617.base= _temp2616; _temp2617.last_plus_one= _temp2616 + 12;
_temp2617;})) == 0){ if( _temp2610 <= 0? 1: _temp2610 > 3){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp2618=( char*)"regparm requires value between 1 and 3";
struct _tagged_string _temp2619; _temp2619.curr= _temp2618; _temp2619.base=
_temp2618; _temp2619.last_plus_one= _temp2618 + 39; _temp2619;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2620= yyls; struct Cyc_Yyltype* _temp2622= _temp2620.curr
+( yylsp_offset - 1); if( _temp2622 < _temp2620.base? 1: _temp2622 >= _temp2620.last_plus_one){
_throw( Null_Exception);}* _temp2622;}).first_line,({ struct _tagged_ptr3
_temp2623= yyls; struct Cyc_Yyltype* _temp2625= _temp2623.curr +( yylsp_offset -
1); if( _temp2625 < _temp2623.base? 1: _temp2625 >= _temp2623.last_plus_one){
_throw( Null_Exception);}* _temp2625;}).last_line));} a=({ struct Cyc_Absyn_Regparm_att_struct*
_temp2626=( struct Cyc_Absyn_Regparm_att_struct*) GC_malloc_atomic( sizeof(
struct Cyc_Absyn_Regparm_att_struct)); _temp2626->tag= Cyc_Absyn_Regparm_att_tag;
_temp2626->f1= _temp2610;( void*) _temp2626;});} else{ if( Cyc_String_zstrcmp( s,(
struct _tagged_string)({ char* _temp2627=( char*)"aligned"; struct
_tagged_string _temp2628; _temp2628.curr= _temp2627; _temp2628.base= _temp2627;
_temp2628.last_plus_one= _temp2627 + 8; _temp2628;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2629=( char*)"__aligned__"; struct
_tagged_string _temp2630; _temp2630.curr= _temp2629; _temp2630.base= _temp2629;
_temp2630.last_plus_one= _temp2629 + 12; _temp2630;})) == 0){ if( _temp2610 < 0){
Cyc_Parse_err(( struct _tagged_string)({ char* _temp2631=( char*)"aligned requires positive power of two";
struct _tagged_string _temp2632; _temp2632.curr= _temp2631; _temp2632.base=
_temp2631; _temp2632.last_plus_one= _temp2631 + 39; _temp2632;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2633= yyls; struct Cyc_Yyltype* _temp2635= _temp2633.curr
+( yylsp_offset - 1); if( _temp2635 < _temp2633.base? 1: _temp2635 >= _temp2633.last_plus_one){
_throw( Null_Exception);}* _temp2635;}).first_line,({ struct _tagged_ptr3
_temp2636= yyls; struct Cyc_Yyltype* _temp2638= _temp2636.curr +( yylsp_offset -
1); if( _temp2638 < _temp2636.base? 1: _temp2638 >= _temp2636.last_plus_one){
_throw( Null_Exception);}* _temp2638;}).last_line));}{ unsigned int j=(
unsigned int) _temp2610; for( 0;( j &( unsigned int) 1) ==( unsigned int) 0; j=
j >>( unsigned int) 1){;} j= j >>( unsigned int) 1; if( j !=( unsigned int) 0){
Cyc_Parse_err(( struct _tagged_string)({ char* _temp2639=( char*)"aligned requires positive power of two";
struct _tagged_string _temp2640; _temp2640.curr= _temp2639; _temp2640.base=
_temp2639; _temp2640.last_plus_one= _temp2639 + 39; _temp2640;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2641= yyls; struct Cyc_Yyltype* _temp2643= _temp2641.curr
+( yylsp_offset - 1); if( _temp2643 < _temp2641.base? 1: _temp2643 >= _temp2641.last_plus_one){
_throw( Null_Exception);}* _temp2643;}).first_line,({ struct _tagged_ptr3
_temp2644= yyls; struct Cyc_Yyltype* _temp2646= _temp2644.curr +( yylsp_offset -
1); if( _temp2646 < _temp2644.base? 1: _temp2646 >= _temp2644.last_plus_one){
_throw( Null_Exception);}* _temp2646;}).last_line));} a=({ struct Cyc_Absyn_Aligned_att_struct*
_temp2647=( struct Cyc_Absyn_Aligned_att_struct*) GC_malloc_atomic( sizeof(
struct Cyc_Absyn_Aligned_att_struct)); _temp2647->tag= Cyc_Absyn_Aligned_att_tag;
_temp2647->f1= _temp2610;( void*) _temp2647;});}} else{ Cyc_Parse_err(( struct
_tagged_string)({ char* _temp2648=( char*)"unrecognized attribute"; struct
_tagged_string _temp2649; _temp2649.curr= _temp2648; _temp2649.base= _temp2648;
_temp2649.last_plus_one= _temp2648 + 23; _temp2649;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2650= yyls; struct Cyc_Yyltype* _temp2652= _temp2650.curr
+( yylsp_offset - 3); if( _temp2652 < _temp2650.base? 1: _temp2652 >= _temp2650.last_plus_one){
_throw( Null_Exception);}* _temp2652;}).first_line,({ struct _tagged_ptr3
_temp2653= yyls; struct Cyc_Yyltype* _temp2655= _temp2653.curr +( yylsp_offset -
3); if( _temp2655 < _temp2653.base? 1: _temp2655 >= _temp2653.last_plus_one){
_throw( Null_Exception);}* _temp2655;}).last_line)); a= Cyc_Absyn_Cdecl_att;}}
yyval=({ struct Cyc_Attribute_tok_struct* _temp2656=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp2656->tag= Cyc_Attribute_tok_tag;
_temp2656->f1=( void*) a;( struct _xenum_struct*) _temp2656;}); break;}} case 51:
_LL2600: { struct _tagged_string s= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp2658= yyvs; struct _xenum_struct** _temp2660= _temp2658.curr +(
yyvsp_offset - 3); if( _temp2660 < _temp2658.base? 1: _temp2660 >= _temp2658.last_plus_one){
_throw( Null_Exception);}* _temp2660;})); struct _tagged_string str= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp2661= yyvs; struct _xenum_struct** _temp2663= _temp2661.curr
+( yyvsp_offset - 1); if( _temp2663 < _temp2661.base? 1: _temp2663 >= _temp2661.last_plus_one){
_throw( Null_Exception);}* _temp2663;})); void* a; if( Cyc_String_zstrcmp( s,(
struct _tagged_string)({ char* _temp2664=( char*)"section"; struct
_tagged_string _temp2665; _temp2665.curr= _temp2664; _temp2665.base= _temp2664;
_temp2665.last_plus_one= _temp2664 + 8; _temp2665;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2666=( char*)"__section__"; struct
_tagged_string _temp2667; _temp2667.curr= _temp2666; _temp2667.base= _temp2666;
_temp2667.last_plus_one= _temp2666 + 12; _temp2667;}))){ a=({ struct Cyc_Absyn_Section_att_struct*
_temp2668=( struct Cyc_Absyn_Section_att_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Section_att_struct));
_temp2668->tag= Cyc_Absyn_Section_att_tag; _temp2668->f1= str;( void*) _temp2668;});}
else{ Cyc_Parse_err(( struct _tagged_string)({ char* _temp2669=( char*)"unrecognized attribute";
struct _tagged_string _temp2670; _temp2670.curr= _temp2669; _temp2670.base=
_temp2669; _temp2670.last_plus_one= _temp2669 + 23; _temp2670;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2671= yyls; struct Cyc_Yyltype* _temp2673= _temp2671.curr
+( yylsp_offset - 3); if( _temp2673 < _temp2671.base? 1: _temp2673 >= _temp2671.last_plus_one){
_throw( Null_Exception);}* _temp2673;}).first_line,({ struct _tagged_ptr3
_temp2674= yyls; struct Cyc_Yyltype* _temp2676= _temp2674.curr +( yylsp_offset -
3); if( _temp2676 < _temp2674.base? 1: _temp2676 >= _temp2674.last_plus_one){
_throw( Null_Exception);}* _temp2676;}).last_line)); a= Cyc_Absyn_Cdecl_att;}
yyval=({ struct Cyc_Attribute_tok_struct* _temp2677=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct)); _temp2677->tag= Cyc_Attribute_tok_tag;
_temp2677->f1=( void*) a;( struct _xenum_struct*) _temp2677;}); break;} case 52:
_LL2657: yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2679=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2679->tag= Cyc_TypeSpecifier_tok_tag;
_temp2679->f1=( void*) Cyc_Parse_type_spec( Cyc_Absyn_VoidType, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2680= yyls; struct Cyc_Yyltype* _temp2682= _temp2680.curr
+ yylsp_offset; if( _temp2682 < _temp2680.base? 1: _temp2682 >= _temp2680.last_plus_one){
_throw( Null_Exception);}* _temp2682;}).first_line,({ struct _tagged_ptr3
_temp2683= yyls; struct Cyc_Yyltype* _temp2685= _temp2683.curr + yylsp_offset;
if( _temp2685 < _temp2683.base? 1: _temp2685 >= _temp2683.last_plus_one){ _throw(
Null_Exception);}* _temp2685;}).last_line));( struct _xenum_struct*) _temp2679;});
break; case 53: _LL2678: yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2687=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2687->tag= Cyc_TypeSpecifier_tok_tag; _temp2687->f1=( void*) Cyc_Parse_type_spec(
Cyc_Absyn_new_evar( Cyc_Absyn_MemKind), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2688= yyls; struct Cyc_Yyltype* _temp2690= _temp2688.curr +
yylsp_offset; if( _temp2690 < _temp2688.base? 1: _temp2690 >= _temp2688.last_plus_one){
_throw( Null_Exception);}* _temp2690;}).first_line,({ struct _tagged_ptr3
_temp2691= yyls; struct Cyc_Yyltype* _temp2693= _temp2691.curr + yylsp_offset;
if( _temp2693 < _temp2691.base? 1: _temp2693 >= _temp2691.last_plus_one){ _throw(
Null_Exception);}* _temp2693;}).last_line));( struct _xenum_struct*) _temp2687;});
break; case 54: _LL2686: yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2695=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2695->tag= Cyc_TypeSpecifier_tok_tag; _temp2695->f1=( void*) Cyc_Parse_type_spec(
Cyc_Absyn_uchar_t, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2696=
yyls; struct Cyc_Yyltype* _temp2698= _temp2696.curr + yylsp_offset; if(
_temp2698 < _temp2696.base? 1: _temp2698 >= _temp2696.last_plus_one){ _throw(
Null_Exception);}* _temp2698;}).first_line,({ struct _tagged_ptr3 _temp2699=
yyls; struct Cyc_Yyltype* _temp2701= _temp2699.curr + yylsp_offset; if(
_temp2701 < _temp2699.base? 1: _temp2701 >= _temp2699.last_plus_one){ _throw(
Null_Exception);}* _temp2701;}).last_line));( struct _xenum_struct*) _temp2695;});
break; case 55: _LL2694: yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2703=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2703->tag= Cyc_TypeSpecifier_tok_tag; _temp2703->f1=( void*)({ struct Cyc_Parse_Short_spec_struct*
_temp2704=( struct Cyc_Parse_Short_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Short_spec_struct));
_temp2704->tag= Cyc_Parse_Short_spec_tag; _temp2704->f1= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2705= yyls; struct Cyc_Yyltype* _temp2707= _temp2705.curr
+ yylsp_offset; if( _temp2707 < _temp2705.base? 1: _temp2707 >= _temp2705.last_plus_one){
_throw( Null_Exception);}* _temp2707;}).first_line,({ struct _tagged_ptr3
_temp2708= yyls; struct Cyc_Yyltype* _temp2710= _temp2708.curr + yylsp_offset;
if( _temp2710 < _temp2708.base? 1: _temp2710 >= _temp2708.last_plus_one){ _throw(
Null_Exception);}* _temp2710;}).last_line);( void*) _temp2704;});( struct
_xenum_struct*) _temp2703;}); break; case 56: _LL2702: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp2712=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2712->tag= Cyc_TypeSpecifier_tok_tag; _temp2712->f1=( void*) Cyc_Parse_type_spec(
Cyc_Absyn_sint_t, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2713=
yyls; struct Cyc_Yyltype* _temp2715= _temp2713.curr + yylsp_offset; if(
_temp2715 < _temp2713.base? 1: _temp2715 >= _temp2713.last_plus_one){ _throw(
Null_Exception);}* _temp2715;}).first_line,({ struct _tagged_ptr3 _temp2716=
yyls; struct Cyc_Yyltype* _temp2718= _temp2716.curr + yylsp_offset; if(
_temp2718 < _temp2716.base? 1: _temp2718 >= _temp2716.last_plus_one){ _throw(
Null_Exception);}* _temp2718;}).last_line));( struct _xenum_struct*) _temp2712;});
break; case 57: _LL2711: yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2720=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2720->tag= Cyc_TypeSpecifier_tok_tag; _temp2720->f1=( void*)({ struct Cyc_Parse_Long_spec_struct*
_temp2721=( struct Cyc_Parse_Long_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Long_spec_struct));
_temp2721->tag= Cyc_Parse_Long_spec_tag; _temp2721->f1= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2722= yyls; struct Cyc_Yyltype* _temp2724= _temp2722.curr
+ yylsp_offset; if( _temp2724 < _temp2722.base? 1: _temp2724 >= _temp2722.last_plus_one){
_throw( Null_Exception);}* _temp2724;}).first_line,({ struct _tagged_ptr3
_temp2725= yyls; struct Cyc_Yyltype* _temp2727= _temp2725.curr + yylsp_offset;
if( _temp2727 < _temp2725.base? 1: _temp2727 >= _temp2725.last_plus_one){ _throw(
Null_Exception);}* _temp2727;}).last_line);( void*) _temp2721;});( struct
_xenum_struct*) _temp2720;}); break; case 58: _LL2719: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp2729=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2729->tag= Cyc_TypeSpecifier_tok_tag; _temp2729->f1=( void*) Cyc_Parse_type_spec(
Cyc_Absyn_float_t, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2730=
yyls; struct Cyc_Yyltype* _temp2732= _temp2730.curr + yylsp_offset; if(
_temp2732 < _temp2730.base? 1: _temp2732 >= _temp2730.last_plus_one){ _throw(
Null_Exception);}* _temp2732;}).first_line,({ struct _tagged_ptr3 _temp2733=
yyls; struct Cyc_Yyltype* _temp2735= _temp2733.curr + yylsp_offset; if(
_temp2735 < _temp2733.base? 1: _temp2735 >= _temp2733.last_plus_one){ _throw(
Null_Exception);}* _temp2735;}).last_line));( struct _xenum_struct*) _temp2729;});
break; case 59: _LL2728: yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2737=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2737->tag= Cyc_TypeSpecifier_tok_tag; _temp2737->f1=( void*) Cyc_Parse_type_spec(
Cyc_Absyn_double_t, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2738=
yyls; struct Cyc_Yyltype* _temp2740= _temp2738.curr + yylsp_offset; if(
_temp2740 < _temp2738.base? 1: _temp2740 >= _temp2738.last_plus_one){ _throw(
Null_Exception);}* _temp2740;}).first_line,({ struct _tagged_ptr3 _temp2741=
yyls; struct Cyc_Yyltype* _temp2743= _temp2741.curr + yylsp_offset; if(
_temp2743 < _temp2741.base? 1: _temp2743 >= _temp2741.last_plus_one){ _throw(
Null_Exception);}* _temp2743;}).last_line));( struct _xenum_struct*) _temp2737;});
break; case 60: _LL2736: yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2745=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2745->tag= Cyc_TypeSpecifier_tok_tag; _temp2745->f1=( void*)({ struct Cyc_Parse_Signed_spec_struct*
_temp2746=( struct Cyc_Parse_Signed_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Signed_spec_struct));
_temp2746->tag= Cyc_Parse_Signed_spec_tag; _temp2746->f1= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2747= yyls; struct Cyc_Yyltype* _temp2749= _temp2747.curr
+ yylsp_offset; if( _temp2749 < _temp2747.base? 1: _temp2749 >= _temp2747.last_plus_one){
_throw( Null_Exception);}* _temp2749;}).first_line,({ struct _tagged_ptr3
_temp2750= yyls; struct Cyc_Yyltype* _temp2752= _temp2750.curr + yylsp_offset;
if( _temp2752 < _temp2750.base? 1: _temp2752 >= _temp2750.last_plus_one){ _throw(
Null_Exception);}* _temp2752;}).last_line);( void*) _temp2746;});( struct
_xenum_struct*) _temp2745;}); break; case 61: _LL2744: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp2754=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2754->tag= Cyc_TypeSpecifier_tok_tag; _temp2754->f1=( void*)({ struct Cyc_Parse_Unsigned_spec_struct*
_temp2755=( struct Cyc_Parse_Unsigned_spec_struct*) GC_malloc( sizeof( struct
Cyc_Parse_Unsigned_spec_struct)); _temp2755->tag= Cyc_Parse_Unsigned_spec_tag;
_temp2755->f1= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2756=
yyls; struct Cyc_Yyltype* _temp2758= _temp2756.curr + yylsp_offset; if(
_temp2758 < _temp2756.base? 1: _temp2758 >= _temp2756.last_plus_one){ _throw(
Null_Exception);}* _temp2758;}).first_line,({ struct _tagged_ptr3 _temp2759=
yyls; struct Cyc_Yyltype* _temp2761= _temp2759.curr + yylsp_offset; if(
_temp2761 < _temp2759.base? 1: _temp2761 >= _temp2759.last_plus_one){ _throw(
Null_Exception);}* _temp2761;}).last_line);( void*) _temp2755;});( struct
_xenum_struct*) _temp2754;}); break; case 62: _LL2753: yyval=({ struct
_tagged_ptr2 _temp2763= yyvs; struct _xenum_struct** _temp2765= _temp2763.curr +
yyvsp_offset; if( _temp2765 < _temp2763.base? 1: _temp2765 >= _temp2763.last_plus_one){
_throw( Null_Exception);}* _temp2765;}); break; case 63: _LL2762: yyval=({
struct _tagged_ptr2 _temp2767= yyvs; struct _xenum_struct** _temp2769= _temp2767.curr
+ yyvsp_offset; if( _temp2769 < _temp2767.base? 1: _temp2769 >= _temp2767.last_plus_one){
_throw( Null_Exception);}* _temp2769;}); break; case 64: _LL2766: yyval=({
struct Cyc_TypeSpecifier_tok_struct* _temp2771=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2771->tag= Cyc_TypeSpecifier_tok_tag;
_temp2771->f1=( void*) Cyc_Parse_type_spec( Cyc_Parse_id2type( Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp2772= yyvs; struct _xenum_struct** _temp2774= _temp2772.curr
+ yyvsp_offset; if( _temp2774 < _temp2772.base? 1: _temp2774 >= _temp2772.last_plus_one){
_throw( Null_Exception);}* _temp2774;})),(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)()),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2775= yyls; struct Cyc_Yyltype*
_temp2777= _temp2775.curr + yylsp_offset; if( _temp2777 < _temp2775.base? 1:
_temp2777 >= _temp2775.last_plus_one){ _throw( Null_Exception);}* _temp2777;}).first_line,({
struct _tagged_ptr3 _temp2778= yyls; struct Cyc_Yyltype* _temp2780= _temp2778.curr
+ yylsp_offset; if( _temp2780 < _temp2778.base? 1: _temp2780 >= _temp2778.last_plus_one){
_throw( Null_Exception);}* _temp2780;}).last_line));( struct _xenum_struct*)
_temp2771;}); break; case 65: _LL2770: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp2782=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2782->tag= Cyc_TypeSpecifier_tok_tag; _temp2782->f1=( void*) Cyc_Parse_type_spec(
Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2783= yyvs;
struct _xenum_struct** _temp2785= _temp2783.curr +( yyvsp_offset - 2); if(
_temp2785 < _temp2783.base? 1: _temp2785 >= _temp2783.last_plus_one){ _throw(
Null_Exception);}* _temp2785;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
Cyc_yyget_Kind_tok(({ struct _tagged_ptr2 _temp2786= yyvs; struct _xenum_struct**
_temp2788= _temp2786.curr + yyvsp_offset; if( _temp2788 < _temp2786.base? 1:
_temp2788 >= _temp2786.last_plus_one){ _throw( Null_Exception);}* _temp2788;})))),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2789= yyls; struct Cyc_Yyltype*
_temp2791= _temp2789.curr +( yylsp_offset - 2); if( _temp2791 < _temp2789.base?
1: _temp2791 >= _temp2789.last_plus_one){ _throw( Null_Exception);}* _temp2791;}).first_line,({
struct _tagged_ptr3 _temp2792= yyls; struct Cyc_Yyltype* _temp2794= _temp2792.curr
+ yylsp_offset; if( _temp2794 < _temp2792.base? 1: _temp2794 >= _temp2792.last_plus_one){
_throw( Null_Exception);}* _temp2794;}).last_line));( struct _xenum_struct*)
_temp2782;}); break; case 66: _LL2781: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp2796=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2796->tag= Cyc_TypeSpecifier_tok_tag; _temp2796->f1=( void*) Cyc_Parse_type_spec(({
struct Cyc_Absyn_TypedefType_struct* _temp2797=( struct Cyc_Absyn_TypedefType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct)); _temp2797->tag= Cyc_Absyn_TypedefType_tag;
_temp2797->f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp2798= yyvs;
struct _xenum_struct** _temp2800= _temp2798.curr +( yyvsp_offset - 1); if(
_temp2800 < _temp2798.base? 1: _temp2800 >= _temp2798.last_plus_one){ _throw(
Null_Exception);}* _temp2800;})); _temp2797->f2= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp2801= yyvs; struct _xenum_struct** _temp2803= _temp2801.curr
+ yyvsp_offset; if( _temp2803 < _temp2801.base? 1: _temp2803 >= _temp2801.last_plus_one){
_throw( Null_Exception);}* _temp2803;})); _temp2797->f3= 0;( void*) _temp2797;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2804= yyls; struct Cyc_Yyltype*
_temp2806= _temp2804.curr +( yylsp_offset - 1); if( _temp2806 < _temp2804.base?
1: _temp2806 >= _temp2804.last_plus_one){ _throw( Null_Exception);}* _temp2806;}).first_line,({
struct _tagged_ptr3 _temp2807= yyls; struct Cyc_Yyltype* _temp2809= _temp2807.curr
+ yylsp_offset; if( _temp2809 < _temp2807.base? 1: _temp2809 >= _temp2807.last_plus_one){
_throw( Null_Exception);}* _temp2809;}).last_line));( struct _xenum_struct*)
_temp2796;}); break; case 67: _LL2795: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp2811=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp2811->tag= Cyc_TypeSpecifier_tok_tag; _temp2811->f1=( void*) Cyc_Parse_type_spec(({
struct Cyc_Absyn_TupleType_struct* _temp2812=( struct Cyc_Absyn_TupleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp2812->tag= Cyc_Absyn_TupleType_tag;
_temp2812->f1=(( struct Cyc_List_List*(*)( struct _tuple9*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2813= yyls; struct Cyc_Yyltype* _temp2815= _temp2813.curr +(
yylsp_offset - 1); if( _temp2815 < _temp2813.base? 1: _temp2815 >= _temp2813.last_plus_one){
_throw( Null_Exception);}* _temp2815;}).first_line,({ struct _tagged_ptr3
_temp2816= yyls; struct Cyc_Yyltype* _temp2818= _temp2816.curr +( yylsp_offset -
1); if( _temp2818 < _temp2816.base? 1: _temp2818 >= _temp2816.last_plus_one){
_throw( Null_Exception);}* _temp2818;}).last_line),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp2819= yyvs; struct _xenum_struct** _temp2821= _temp2819.curr
+( yyvsp_offset - 1); if( _temp2821 < _temp2819.base? 1: _temp2821 >= _temp2819.last_plus_one){
_throw( Null_Exception);}* _temp2821;}))));( void*) _temp2812;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2822= yyls; struct Cyc_Yyltype* _temp2824= _temp2822.curr
+( yylsp_offset - 3); if( _temp2824 < _temp2822.base? 1: _temp2824 >= _temp2822.last_plus_one){
_throw( Null_Exception);}* _temp2824;}).first_line,({ struct _tagged_ptr3
_temp2825= yyls; struct Cyc_Yyltype* _temp2827= _temp2825.curr + yylsp_offset;
if( _temp2827 < _temp2825.base? 1: _temp2827 >= _temp2825.last_plus_one){ _throw(
Null_Exception);}* _temp2827;}).last_line));( struct _xenum_struct*) _temp2811;});
break; case 68: _LL2810: yyval=({ struct Cyc_Kind_tok_struct* _temp2829=( struct
Cyc_Kind_tok_struct*) GC_malloc( sizeof( struct Cyc_Kind_tok_struct)); _temp2829->tag=
Cyc_Kind_tok_tag; _temp2829->f1=( void*) Cyc_Parse_id_to_kind( Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp2830= yyvs; struct _xenum_struct** _temp2832= _temp2830.curr
+ yyvsp_offset; if( _temp2832 < _temp2830.base? 1: _temp2832 >= _temp2830.last_plus_one){
_throw( Null_Exception);}* _temp2832;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2833= yyls; struct Cyc_Yyltype* _temp2835= _temp2833.curr +
yylsp_offset; if( _temp2835 < _temp2833.base? 1: _temp2835 >= _temp2833.last_plus_one){
_throw( Null_Exception);}* _temp2835;}).first_line,({ struct _tagged_ptr3
_temp2836= yyls; struct Cyc_Yyltype* _temp2838= _temp2836.curr + yylsp_offset;
if( _temp2838 < _temp2836.base? 1: _temp2838 >= _temp2836.last_plus_one){ _throw(
Null_Exception);}* _temp2838;}).last_line));( struct _xenum_struct*) _temp2829;});
break; case 69: _LL2828: { struct _tagged_string* _temp2845; void* _temp2847;
struct _tuple1 _temp2843=* Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp2840=
yyvs; struct _xenum_struct** _temp2842= _temp2840.curr + yyvsp_offset; if(
_temp2842 < _temp2840.base? 1: _temp2842 >= _temp2840.last_plus_one){ _throw(
Null_Exception);}* _temp2842;})); _LL2848: _temp2847= _temp2843.f1; goto _LL2846;
_LL2846: _temp2845= _temp2843.f2; goto _LL2844; _LL2844: if( _temp2847 != Cyc_Absyn_Loc_n){
Cyc_Parse_err(( struct _tagged_string)({ char* _temp2849=( char*)"bad kind in type specifier";
struct _tagged_string _temp2850; _temp2850.curr= _temp2849; _temp2850.base=
_temp2849; _temp2850.last_plus_one= _temp2849 + 27; _temp2850;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2851= yyls; struct Cyc_Yyltype* _temp2853= _temp2851.curr
+ yylsp_offset; if( _temp2853 < _temp2851.base? 1: _temp2853 >= _temp2851.last_plus_one){
_throw( Null_Exception);}* _temp2853;}).first_line,({ struct _tagged_ptr3
_temp2854= yyls; struct Cyc_Yyltype* _temp2856= _temp2854.curr + yylsp_offset;
if( _temp2856 < _temp2854.base? 1: _temp2856 >= _temp2854.last_plus_one){ _throw(
Null_Exception);}* _temp2856;}).last_line));} yyval=({ struct Cyc_Kind_tok_struct*
_temp2857=( struct Cyc_Kind_tok_struct*) GC_malloc( sizeof( struct Cyc_Kind_tok_struct));
_temp2857->tag= Cyc_Kind_tok_tag; _temp2857->f1=( void*) Cyc_Parse_id_to_kind(*
_temp2845, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2858= yyls;
struct Cyc_Yyltype* _temp2860= _temp2858.curr + yylsp_offset; if( _temp2860 <
_temp2858.base? 1: _temp2860 >= _temp2858.last_plus_one){ _throw( Null_Exception);}*
_temp2860;}).first_line,({ struct _tagged_ptr3 _temp2861= yyls; struct Cyc_Yyltype*
_temp2863= _temp2861.curr + yylsp_offset; if( _temp2863 < _temp2861.base? 1:
_temp2863 >= _temp2861.last_plus_one){ _throw( Null_Exception);}* _temp2863;}).last_line));(
struct _xenum_struct*) _temp2857;}); break;} case 70: _LL2839: yyval=({ struct
Cyc_TypeQual_tok_struct* _temp2865=( struct Cyc_TypeQual_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeQual_tok_struct)); _temp2865->tag= Cyc_TypeQual_tok_tag;
_temp2865->f1=({ struct Cyc_Absyn_Tqual* _temp2866=( struct Cyc_Absyn_Tqual*)
GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual)); _temp2866->q_const= 1;
_temp2866->q_volatile= 0; _temp2866->q_restrict= 0; _temp2866;});( struct
_xenum_struct*) _temp2865;}); break; case 71: _LL2864: yyval=({ struct Cyc_TypeQual_tok_struct*
_temp2868=( struct Cyc_TypeQual_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct));
_temp2868->tag= Cyc_TypeQual_tok_tag; _temp2868->f1=({ struct Cyc_Absyn_Tqual*
_temp2869=( struct Cyc_Absyn_Tqual*) GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual));
_temp2869->q_const= 0; _temp2869->q_volatile= 1; _temp2869->q_restrict= 0;
_temp2869;});( struct _xenum_struct*) _temp2868;}); break; case 72: _LL2867:
yyval=({ struct Cyc_TypeQual_tok_struct* _temp2871=( struct Cyc_TypeQual_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct)); _temp2871->tag= Cyc_TypeQual_tok_tag;
_temp2871->f1=({ struct Cyc_Absyn_Tqual* _temp2872=( struct Cyc_Absyn_Tqual*)
GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual)); _temp2872->q_const= 0;
_temp2872->q_volatile= 0; _temp2872->q_restrict= 1; _temp2872;});( struct
_xenum_struct*) _temp2871;}); break; case 73: _LL2870: { struct Cyc_Absyn_Decl*
d;{ void* _temp2877= Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2
_temp2874= yyvs; struct _xenum_struct** _temp2876= _temp2874.curr +(
yyvsp_offset - 3); if( _temp2876 < _temp2874.base? 1: _temp2876 >= _temp2874.last_plus_one){
_throw( Null_Exception);}* _temp2876;})); _LL2879: if( _temp2877 == Cyc_Parse_Struct_su){
goto _LL2880;} else{ goto _LL2881;} _LL2881: if( _temp2877 == Cyc_Parse_Union_su){
goto _LL2882;} else{ goto _LL2878;} _LL2880: d= Cyc_Absyn_struct_decl( Cyc_Absyn_Public,
0, 0,({ struct Cyc_Core_Opt* _temp2883=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp2883->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp2884= yyvs; struct _xenum_struct** _temp2886= _temp2884.curr
+( yyvsp_offset - 1); if( _temp2886 < _temp2884.base? 1: _temp2886 >= _temp2884.last_plus_one){
_throw( Null_Exception);}* _temp2886;})); _temp2883;}), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2887= yyls; struct Cyc_Yyltype* _temp2889= _temp2887.curr
+( yylsp_offset - 3); if( _temp2889 < _temp2887.base? 1: _temp2889 >= _temp2887.last_plus_one){
_throw( Null_Exception);}* _temp2889;}).first_line,({ struct _tagged_ptr3
_temp2890= yyls; struct Cyc_Yyltype* _temp2892= _temp2890.curr + yylsp_offset;
if( _temp2892 < _temp2890.base? 1: _temp2892 >= _temp2890.last_plus_one){ _throw(
Null_Exception);}* _temp2892;}).last_line)); goto _LL2878; _LL2882: d= Cyc_Absyn_union_decl(
Cyc_Absyn_Public, 0, 0,({ struct Cyc_Core_Opt* _temp2893=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2893->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp2894= yyvs; struct _xenum_struct** _temp2896= _temp2894.curr
+( yyvsp_offset - 1); if( _temp2896 < _temp2894.base? 1: _temp2896 >= _temp2894.last_plus_one){
_throw( Null_Exception);}* _temp2896;})); _temp2893;}), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2897= yyls; struct Cyc_Yyltype* _temp2899= _temp2897.curr
+( yylsp_offset - 3); if( _temp2899 < _temp2897.base? 1: _temp2899 >= _temp2897.last_plus_one){
_throw( Null_Exception);}* _temp2899;}).first_line,({ struct _tagged_ptr3
_temp2900= yyls; struct Cyc_Yyltype* _temp2902= _temp2900.curr + yylsp_offset;
if( _temp2902 < _temp2900.base? 1: _temp2902 >= _temp2900.last_plus_one){ _throw(
Null_Exception);}* _temp2902;}).last_line)); goto _LL2878; _LL2878:;} yyval=({
struct Cyc_TypeSpecifier_tok_struct* _temp2903=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2903->tag= Cyc_TypeSpecifier_tok_tag;
_temp2903->f1=( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp2904=( struct
Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2904->tag= Cyc_Parse_Decl_spec_tag; _temp2904->f1= d;( void*) _temp2904;});(
struct _xenum_struct*) _temp2903;}); Cyc_Parse_unimp2(( struct _tagged_string)({
char* _temp2905=( char*)"anonymous structs/unions"; struct _tagged_string
_temp2906; _temp2906.curr= _temp2905; _temp2906.base= _temp2905; _temp2906.last_plus_one=
_temp2905 + 25; _temp2906;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2907= yyls; struct Cyc_Yyltype* _temp2909= _temp2907.curr +( yylsp_offset -
3); if( _temp2909 < _temp2907.base? 1: _temp2909 >= _temp2907.last_plus_one){
_throw( Null_Exception);}* _temp2909;}).first_line,({ struct _tagged_ptr3
_temp2910= yyls; struct Cyc_Yyltype* _temp2912= _temp2910.curr + yylsp_offset;
if( _temp2912 < _temp2910.base? 1: _temp2912 >= _temp2910.last_plus_one){ _throw(
Null_Exception);}* _temp2912;}).last_line)); break;} case 74: _LL2873: { struct
Cyc_List_List* ts=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2914= yyls; struct Cyc_Yyltype* _temp2916= _temp2914.curr
+( yylsp_offset - 3); if( _temp2916 < _temp2914.base? 1: _temp2916 >= _temp2914.last_plus_one){
_throw( Null_Exception);}* _temp2916;}).first_line,({ struct _tagged_ptr3
_temp2917= yyls; struct Cyc_Yyltype* _temp2919= _temp2917.curr +( yylsp_offset -
3); if( _temp2919 < _temp2917.base? 1: _temp2919 >= _temp2917.last_plus_one){
_throw( Null_Exception);}* _temp2919;}).last_line), Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp2920= yyvs; struct _xenum_struct** _temp2922= _temp2920.curr
+( yyvsp_offset - 3); if( _temp2922 < _temp2920.base? 1: _temp2922 >= _temp2920.last_plus_one){
_throw( Null_Exception);}* _temp2922;}))); struct Cyc_Absyn_Decl* d;{ void*
_temp2926= Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2 _temp2923= yyvs;
struct _xenum_struct** _temp2925= _temp2923.curr +( yyvsp_offset - 5); if(
_temp2925 < _temp2923.base? 1: _temp2925 >= _temp2923.last_plus_one){ _throw(
Null_Exception);}* _temp2925;})); _LL2928: if( _temp2926 == Cyc_Parse_Struct_su){
goto _LL2929;} else{ goto _LL2930;} _LL2930: if( _temp2926 == Cyc_Parse_Union_su){
goto _LL2931;} else{ goto _LL2927;} _LL2929: d= Cyc_Absyn_struct_decl( Cyc_Absyn_Public,({
struct Cyc_Core_Opt* _temp2932=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2932->v=( void*) Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp2933= yyvs; struct _xenum_struct** _temp2935= _temp2933.curr +(
yyvsp_offset - 4); if( _temp2935 < _temp2933.base? 1: _temp2935 >= _temp2933.last_plus_one){
_throw( Null_Exception);}* _temp2935;})); _temp2932;}), ts,({ struct Cyc_Core_Opt*
_temp2936=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2936->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({ struct _tagged_ptr2
_temp2937= yyvs; struct _xenum_struct** _temp2939= _temp2937.curr +(
yyvsp_offset - 1); if( _temp2939 < _temp2937.base? 1: _temp2939 >= _temp2937.last_plus_one){
_throw( Null_Exception);}* _temp2939;})); _temp2936;}), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2940= yyls; struct Cyc_Yyltype* _temp2942= _temp2940.curr
+( yylsp_offset - 5); if( _temp2942 < _temp2940.base? 1: _temp2942 >= _temp2940.last_plus_one){
_throw( Null_Exception);}* _temp2942;}).first_line,({ struct _tagged_ptr3
_temp2943= yyls; struct Cyc_Yyltype* _temp2945= _temp2943.curr + yylsp_offset;
if( _temp2945 < _temp2943.base? 1: _temp2945 >= _temp2943.last_plus_one){ _throw(
Null_Exception);}* _temp2945;}).last_line)); goto _LL2927; _LL2931: d= Cyc_Absyn_union_decl(
Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp2946=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2946->v=( void*) Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp2947= yyvs; struct _xenum_struct** _temp2949= _temp2947.curr
+( yyvsp_offset - 4); if( _temp2949 < _temp2947.base? 1: _temp2949 >= _temp2947.last_plus_one){
_throw( Null_Exception);}* _temp2949;})); _temp2946;}), ts,({ struct Cyc_Core_Opt*
_temp2950=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2950->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({ struct _tagged_ptr2
_temp2951= yyvs; struct _xenum_struct** _temp2953= _temp2951.curr +(
yyvsp_offset - 1); if( _temp2953 < _temp2951.base? 1: _temp2953 >= _temp2951.last_plus_one){
_throw( Null_Exception);}* _temp2953;})); _temp2950;}), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2954= yyls; struct Cyc_Yyltype* _temp2956= _temp2954.curr
+( yylsp_offset - 5); if( _temp2956 < _temp2954.base? 1: _temp2956 >= _temp2954.last_plus_one){
_throw( Null_Exception);}* _temp2956;}).first_line,({ struct _tagged_ptr3
_temp2957= yyls; struct Cyc_Yyltype* _temp2959= _temp2957.curr + yylsp_offset;
if( _temp2959 < _temp2957.base? 1: _temp2959 >= _temp2957.last_plus_one){ _throw(
Null_Exception);}* _temp2959;}).last_line)); goto _LL2927; _LL2927:;} yyval=({
struct Cyc_TypeSpecifier_tok_struct* _temp2960=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp2960->tag= Cyc_TypeSpecifier_tok_tag;
_temp2960->f1=( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp2961=( struct
Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2961->tag= Cyc_Parse_Decl_spec_tag; _temp2961->f1= d;( void*) _temp2961;});(
struct _xenum_struct*) _temp2960;}); break;} case 75: _LL2913: { struct Cyc_List_List*
ts=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2963=
yyls; struct Cyc_Yyltype* _temp2965= _temp2963.curr +( yylsp_offset - 3); if(
_temp2965 < _temp2963.base? 1: _temp2965 >= _temp2963.last_plus_one){ _throw(
Null_Exception);}* _temp2965;}).first_line,({ struct _tagged_ptr3 _temp2966=
yyls; struct Cyc_Yyltype* _temp2968= _temp2966.curr +( yylsp_offset - 3); if(
_temp2968 < _temp2966.base? 1: _temp2968 >= _temp2966.last_plus_one){ _throw(
Null_Exception);}* _temp2968;}).last_line), Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp2969= yyvs; struct _xenum_struct** _temp2971= _temp2969.curr +(
yyvsp_offset - 3); if( _temp2971 < _temp2969.base? 1: _temp2971 >= _temp2969.last_plus_one){
_throw( Null_Exception);}* _temp2971;}))); struct Cyc_Absyn_Decl* d;{ void*
_temp2975= Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2 _temp2972= yyvs;
struct _xenum_struct** _temp2974= _temp2972.curr +( yyvsp_offset - 5); if(
_temp2974 < _temp2972.base? 1: _temp2974 >= _temp2972.last_plus_one){ _throw(
Null_Exception);}* _temp2974;})); _LL2977: if( _temp2975 == Cyc_Parse_Struct_su){
goto _LL2978;} else{ goto _LL2979;} _LL2979: if( _temp2975 == Cyc_Parse_Union_su){
goto _LL2980;} else{ goto _LL2976;} _LL2978: d= Cyc_Absyn_struct_decl( Cyc_Absyn_Public,({
struct Cyc_Core_Opt* _temp2981=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2981->v=( void*) Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp2982= yyvs; struct _xenum_struct** _temp2984= _temp2982.curr +(
yyvsp_offset - 4); if( _temp2984 < _temp2982.base? 1: _temp2984 >= _temp2982.last_plus_one){
_throw( Null_Exception);}* _temp2984;})); _temp2981;}), ts,({ struct Cyc_Core_Opt*
_temp2985=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2985->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({ struct _tagged_ptr2
_temp2986= yyvs; struct _xenum_struct** _temp2988= _temp2986.curr +(
yyvsp_offset - 1); if( _temp2988 < _temp2986.base? 1: _temp2988 >= _temp2986.last_plus_one){
_throw( Null_Exception);}* _temp2988;})); _temp2985;}), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2989= yyls; struct Cyc_Yyltype* _temp2991= _temp2989.curr
+( yylsp_offset - 5); if( _temp2991 < _temp2989.base? 1: _temp2991 >= _temp2989.last_plus_one){
_throw( Null_Exception);}* _temp2991;}).first_line,({ struct _tagged_ptr3
_temp2992= yyls; struct Cyc_Yyltype* _temp2994= _temp2992.curr + yylsp_offset;
if( _temp2994 < _temp2992.base? 1: _temp2994 >= _temp2992.last_plus_one){ _throw(
Null_Exception);}* _temp2994;}).last_line)); goto _LL2976; _LL2980: d= Cyc_Absyn_union_decl(
Cyc_Absyn_Public,({ struct Cyc_Core_Opt* _temp2995=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp2995->v=( void*) Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp2996= yyvs; struct _xenum_struct** _temp2998= _temp2996.curr
+( yyvsp_offset - 4); if( _temp2998 < _temp2996.base? 1: _temp2998 >= _temp2996.last_plus_one){
_throw( Null_Exception);}* _temp2998;})); _temp2995;}), ts,({ struct Cyc_Core_Opt*
_temp2999=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2999->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({ struct _tagged_ptr2
_temp3000= yyvs; struct _xenum_struct** _temp3002= _temp3000.curr +(
yyvsp_offset - 1); if( _temp3002 < _temp3000.base? 1: _temp3002 >= _temp3000.last_plus_one){
_throw( Null_Exception);}* _temp3002;})); _temp2999;}), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3003= yyls; struct Cyc_Yyltype* _temp3005= _temp3003.curr
+( yylsp_offset - 5); if( _temp3005 < _temp3003.base? 1: _temp3005 >= _temp3003.last_plus_one){
_throw( Null_Exception);}* _temp3005;}).first_line,({ struct _tagged_ptr3
_temp3006= yyls; struct Cyc_Yyltype* _temp3008= _temp3006.curr + yylsp_offset;
if( _temp3008 < _temp3006.base? 1: _temp3008 >= _temp3006.last_plus_one){ _throw(
Null_Exception);}* _temp3008;}).last_line)); goto _LL2976; _LL2976:;} yyval=({
struct Cyc_TypeSpecifier_tok_struct* _temp3009=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3009->tag= Cyc_TypeSpecifier_tok_tag;
_temp3009->f1=( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp3010=( struct
Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3010->tag= Cyc_Parse_Decl_spec_tag; _temp3010->f1= d;( void*) _temp3010;});(
struct _xenum_struct*) _temp3009;}); break;} case 76: _LL2962:{ void* _temp3015=
Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2 _temp3012= yyvs; struct
_xenum_struct** _temp3014= _temp3012.curr +( yyvsp_offset - 2); if( _temp3014 <
_temp3012.base? 1: _temp3014 >= _temp3012.last_plus_one){ _throw( Null_Exception);}*
_temp3014;})); _LL3017: if( _temp3015 == Cyc_Parse_Struct_su){ goto _LL3018;}
else{ goto _LL3019;} _LL3019: if( _temp3015 == Cyc_Parse_Union_su){ goto _LL3020;}
else{ goto _LL3016;} _LL3018: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp3021=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3021->tag= Cyc_TypeSpecifier_tok_tag; _temp3021->f1=( void*) Cyc_Parse_type_spec(({
struct Cyc_Absyn_StructType_struct* _temp3022=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp3022->tag= Cyc_Absyn_StructType_tag;
_temp3022->f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3023= yyvs; struct _xenum_struct** _temp3025= _temp3023.curr +(
yyvsp_offset - 1); if( _temp3025 < _temp3023.base? 1: _temp3025 >= _temp3023.last_plus_one){
_throw( Null_Exception);}* _temp3025;})); _temp3022->f2= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3026= yyvs; struct _xenum_struct** _temp3028= _temp3026.curr
+ yyvsp_offset; if( _temp3028 < _temp3026.base? 1: _temp3028 >= _temp3026.last_plus_one){
_throw( Null_Exception);}* _temp3028;})); _temp3022->f3= 0;( void*) _temp3022;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3029= yyls; struct Cyc_Yyltype*
_temp3031= _temp3029.curr +( yylsp_offset - 2); if( _temp3031 < _temp3029.base?
1: _temp3031 >= _temp3029.last_plus_one){ _throw( Null_Exception);}* _temp3031;}).first_line,({
struct _tagged_ptr3 _temp3032= yyls; struct Cyc_Yyltype* _temp3034= _temp3032.curr
+ yylsp_offset; if( _temp3034 < _temp3032.base? 1: _temp3034 >= _temp3032.last_plus_one){
_throw( Null_Exception);}* _temp3034;}).last_line));( struct _xenum_struct*)
_temp3021;}); goto _LL3016; _LL3020: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp3035=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3035->tag= Cyc_TypeSpecifier_tok_tag; _temp3035->f1=( void*) Cyc_Parse_type_spec(({
struct Cyc_Absyn_UnionType_struct* _temp3036=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp3036->tag= Cyc_Absyn_UnionType_tag;
_temp3036->f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3037= yyvs; struct _xenum_struct** _temp3039= _temp3037.curr +(
yyvsp_offset - 1); if( _temp3039 < _temp3037.base? 1: _temp3039 >= _temp3037.last_plus_one){
_throw( Null_Exception);}* _temp3039;})); _temp3036->f2= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3040= yyvs; struct _xenum_struct** _temp3042= _temp3040.curr
+ yyvsp_offset; if( _temp3042 < _temp3040.base? 1: _temp3042 >= _temp3040.last_plus_one){
_throw( Null_Exception);}* _temp3042;})); _temp3036->f3= 0;( void*) _temp3036;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3043= yyls; struct Cyc_Yyltype*
_temp3045= _temp3043.curr +( yylsp_offset - 2); if( _temp3045 < _temp3043.base?
1: _temp3045 >= _temp3043.last_plus_one){ _throw( Null_Exception);}* _temp3045;}).first_line,({
struct _tagged_ptr3 _temp3046= yyls; struct Cyc_Yyltype* _temp3048= _temp3046.curr
+ yylsp_offset; if( _temp3048 < _temp3046.base? 1: _temp3048 >= _temp3046.last_plus_one){
_throw( Null_Exception);}* _temp3048;}).last_line));( struct _xenum_struct*)
_temp3035;}); goto _LL3016; _LL3016:;} break; case 77: _LL3011:{ void* _temp3053=
Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2 _temp3050= yyvs; struct
_xenum_struct** _temp3052= _temp3050.curr +( yyvsp_offset - 2); if( _temp3052 <
_temp3050.base? 1: _temp3052 >= _temp3050.last_plus_one){ _throw( Null_Exception);}*
_temp3052;})); _LL3055: if( _temp3053 == Cyc_Parse_Struct_su){ goto _LL3056;}
else{ goto _LL3057;} _LL3057: if( _temp3053 == Cyc_Parse_Union_su){ goto _LL3058;}
else{ goto _LL3054;} _LL3056: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp3059=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3059->tag= Cyc_TypeSpecifier_tok_tag; _temp3059->f1=( void*) Cyc_Parse_type_spec(({
struct Cyc_Absyn_StructType_struct* _temp3060=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp3060->tag= Cyc_Absyn_StructType_tag;
_temp3060->f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3061= yyvs; struct _xenum_struct** _temp3063= _temp3061.curr +(
yyvsp_offset - 1); if( _temp3063 < _temp3061.base? 1: _temp3063 >= _temp3061.last_plus_one){
_throw( Null_Exception);}* _temp3063;})); _temp3060->f2= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3064= yyvs; struct _xenum_struct** _temp3066= _temp3064.curr
+ yyvsp_offset; if( _temp3066 < _temp3064.base? 1: _temp3066 >= _temp3064.last_plus_one){
_throw( Null_Exception);}* _temp3066;})); _temp3060->f3= 0;( void*) _temp3060;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3067= yyls; struct Cyc_Yyltype*
_temp3069= _temp3067.curr +( yylsp_offset - 2); if( _temp3069 < _temp3067.base?
1: _temp3069 >= _temp3067.last_plus_one){ _throw( Null_Exception);}* _temp3069;}).first_line,({
struct _tagged_ptr3 _temp3070= yyls; struct Cyc_Yyltype* _temp3072= _temp3070.curr
+ yylsp_offset; if( _temp3072 < _temp3070.base? 1: _temp3072 >= _temp3070.last_plus_one){
_throw( Null_Exception);}* _temp3072;}).last_line));( struct _xenum_struct*)
_temp3059;}); goto _LL3054; _LL3058: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp3073=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3073->tag= Cyc_TypeSpecifier_tok_tag; _temp3073->f1=( void*) Cyc_Parse_type_spec(({
struct Cyc_Absyn_UnionType_struct* _temp3074=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp3074->tag= Cyc_Absyn_UnionType_tag;
_temp3074->f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3075= yyvs; struct _xenum_struct** _temp3077= _temp3075.curr +(
yyvsp_offset - 1); if( _temp3077 < _temp3075.base? 1: _temp3077 >= _temp3075.last_plus_one){
_throw( Null_Exception);}* _temp3077;})); _temp3074->f2= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3078= yyvs; struct _xenum_struct** _temp3080= _temp3078.curr
+ yyvsp_offset; if( _temp3080 < _temp3078.base? 1: _temp3080 >= _temp3078.last_plus_one){
_throw( Null_Exception);}* _temp3080;})); _temp3074->f3= 0;( void*) _temp3074;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3081= yyls; struct Cyc_Yyltype*
_temp3083= _temp3081.curr +( yylsp_offset - 2); if( _temp3083 < _temp3081.base?
1: _temp3083 >= _temp3081.last_plus_one){ _throw( Null_Exception);}* _temp3083;}).first_line,({
struct _tagged_ptr3 _temp3084= yyls; struct Cyc_Yyltype* _temp3086= _temp3084.curr
+ yylsp_offset; if( _temp3086 < _temp3084.base? 1: _temp3086 >= _temp3084.last_plus_one){
_throw( Null_Exception);}* _temp3086;}).last_line));( struct _xenum_struct*)
_temp3073;}); goto _LL3054; _LL3054:;} break; case 78: _LL3049: yyval=({ struct
Cyc_TypeList_tok_struct* _temp3088=( struct Cyc_TypeList_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeList_tok_struct)); _temp3088->tag= Cyc_TypeList_tok_tag;
_temp3088->f1= 0;( struct _xenum_struct*) _temp3088;}); break; case 79: _LL3087:
yyval=({ struct Cyc_TypeList_tok_struct* _temp3090=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3090->tag= Cyc_TypeList_tok_tag;
_temp3090->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3091= yyvs; struct
_xenum_struct** _temp3093= _temp3091.curr +( yyvsp_offset - 1); if( _temp3093 <
_temp3091.base? 1: _temp3093 >= _temp3091.last_plus_one){ _throw( Null_Exception);}*
_temp3093;})));( struct _xenum_struct*) _temp3090;}); break; case 80: _LL3089:((
struct Cyc_Lexing_lexbuf*) Cyc_Parse_lbuf->v)->lex_curr_pos -= 1; yyval=({
struct Cyc_TypeList_tok_struct* _temp3095=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3095->tag= Cyc_TypeList_tok_tag;
_temp3095->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3096= yyvs; struct
_xenum_struct** _temp3098= _temp3096.curr +( yyvsp_offset - 1); if( _temp3098 <
_temp3096.base? 1: _temp3098 >= _temp3096.last_plus_one){ _throw( Null_Exception);}*
_temp3098;})));( struct _xenum_struct*) _temp3095;}); break; case 81: _LL3094:
yyval=({ struct Cyc_StructOrUnion_tok_struct* _temp3100=( struct Cyc_StructOrUnion_tok_struct*)
GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct)); _temp3100->tag= Cyc_StructOrUnion_tok_tag;
_temp3100->f1=( void*) Cyc_Parse_Struct_su;( struct _xenum_struct*) _temp3100;});
break; case 82: _LL3099: yyval=({ struct Cyc_StructOrUnion_tok_struct* _temp3102=(
struct Cyc_StructOrUnion_tok_struct*) GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));
_temp3102->tag= Cyc_StructOrUnion_tok_tag; _temp3102->f1=( void*) Cyc_Parse_Union_su;(
struct _xenum_struct*) _temp3102;}); break; case 83: _LL3101: yyval=({ struct
Cyc_StructFieldDeclList_tok_struct* _temp3104=( struct Cyc_StructFieldDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_StructFieldDeclList_tok_struct)); _temp3104->tag=
Cyc_StructFieldDeclList_tok_tag; _temp3104->f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_flatten)((( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_StructFieldDeclListList_tok(({
struct _tagged_ptr2 _temp3105= yyvs; struct _xenum_struct** _temp3107= _temp3105.curr
+ yyvsp_offset; if( _temp3107 < _temp3105.base? 1: _temp3107 >= _temp3105.last_plus_one){
_throw( Null_Exception);}* _temp3107;}))));( struct _xenum_struct*) _temp3104;});
break; case 84: _LL3103: yyval=({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp3109=( struct Cyc_StructFieldDeclListList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct)); _temp3109->tag= Cyc_StructFieldDeclListList_tok_tag;
_temp3109->f1=({ struct Cyc_List_List* _temp3110=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3110->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp3111= yyvs; struct _xenum_struct** _temp3113= _temp3111.curr
+ yyvsp_offset; if( _temp3113 < _temp3111.base? 1: _temp3113 >= _temp3111.last_plus_one){
_throw( Null_Exception);}* _temp3113;})); _temp3110->tl= 0; _temp3110;});(
struct _xenum_struct*) _temp3109;}); break; case 85: _LL3108: yyval=({ struct
Cyc_StructFieldDeclListList_tok_struct* _temp3115=( struct Cyc_StructFieldDeclListList_tok_struct*)
GC_malloc( sizeof( struct Cyc_StructFieldDeclListList_tok_struct)); _temp3115->tag=
Cyc_StructFieldDeclListList_tok_tag; _temp3115->f1=({ struct Cyc_List_List*
_temp3116=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3116->hd=( void*) Cyc_yyget_StructFieldDeclList_tok(({ struct _tagged_ptr2
_temp3120= yyvs; struct _xenum_struct** _temp3122= _temp3120.curr + yyvsp_offset;
if( _temp3122 < _temp3120.base? 1: _temp3122 >= _temp3120.last_plus_one){ _throw(
Null_Exception);}* _temp3122;})); _temp3116->tl= Cyc_yyget_StructFieldDeclListList_tok(({
struct _tagged_ptr2 _temp3117= yyvs; struct _xenum_struct** _temp3119= _temp3117.curr
+( yyvsp_offset - 1); if( _temp3119 < _temp3117.base? 1: _temp3119 >= _temp3117.last_plus_one){
_throw( Null_Exception);}* _temp3119;})); _temp3116;});( struct _xenum_struct*)
_temp3115;}); break; case 86: _LL3114: yyval=({ struct Cyc_InitDeclList_tok_struct*
_temp3124=( struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp3124->tag= Cyc_InitDeclList_tok_tag; _temp3124->f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitDeclList_tok(({
struct _tagged_ptr2 _temp3125= yyvs; struct _xenum_struct** _temp3127= _temp3125.curr
+ yyvsp_offset; if( _temp3127 < _temp3125.base? 1: _temp3127 >= _temp3125.last_plus_one){
_throw( Null_Exception);}* _temp3127;})));( struct _xenum_struct*) _temp3124;});
break; case 87: _LL3123: yyval=({ struct Cyc_InitDeclList_tok_struct* _temp3129=(
struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp3129->tag= Cyc_InitDeclList_tok_tag; _temp3129->f1=({ struct Cyc_List_List*
_temp3130=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3130->hd=( void*) Cyc_yyget_InitDecl_tok(({ struct _tagged_ptr2 _temp3131=
yyvs; struct _xenum_struct** _temp3133= _temp3131.curr + yyvsp_offset; if(
_temp3133 < _temp3131.base? 1: _temp3133 >= _temp3131.last_plus_one){ _throw(
Null_Exception);}* _temp3133;})); _temp3130->tl= 0; _temp3130;});( struct
_xenum_struct*) _temp3129;}); break; case 88: _LL3128: yyval=({ struct Cyc_InitDeclList_tok_struct*
_temp3135=( struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));
_temp3135->tag= Cyc_InitDeclList_tok_tag; _temp3135->f1=({ struct Cyc_List_List*
_temp3136=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3136->hd=( void*) Cyc_yyget_InitDecl_tok(({ struct _tagged_ptr2 _temp3140=
yyvs; struct _xenum_struct** _temp3142= _temp3140.curr + yyvsp_offset; if(
_temp3142 < _temp3140.base? 1: _temp3142 >= _temp3140.last_plus_one){ _throw(
Null_Exception);}* _temp3142;})); _temp3136->tl= Cyc_yyget_InitDeclList_tok(({
struct _tagged_ptr2 _temp3137= yyvs; struct _xenum_struct** _temp3139= _temp3137.curr
+( yyvsp_offset - 2); if( _temp3139 < _temp3137.base? 1: _temp3139 >= _temp3137.last_plus_one){
_throw( Null_Exception);}* _temp3139;})); _temp3136;});( struct _xenum_struct*)
_temp3135;}); break; case 89: _LL3134: yyval=({ struct Cyc_InitDecl_tok_struct*
_temp3144=( struct Cyc_InitDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp3144->tag= Cyc_InitDecl_tok_tag; _temp3144->f1=({ struct _tuple12*
_temp3145=( struct _tuple12*) GC_malloc( sizeof( struct _tuple12)); _temp3145->f1=
Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3146= yyvs; struct
_xenum_struct** _temp3148= _temp3146.curr + yyvsp_offset; if( _temp3148 <
_temp3146.base? 1: _temp3148 >= _temp3146.last_plus_one){ _throw( Null_Exception);}*
_temp3148;})); _temp3145->f2= 0; _temp3145;});( struct _xenum_struct*) _temp3144;});
break; case 90: _LL3143: yyval=({ struct Cyc_InitDecl_tok_struct* _temp3150=(
struct Cyc_InitDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct));
_temp3150->tag= Cyc_InitDecl_tok_tag; _temp3150->f1=({ struct _tuple12*
_temp3151=( struct _tuple12*) GC_malloc( sizeof( struct _tuple12)); _temp3151->f1=
Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3155= yyvs; struct
_xenum_struct** _temp3157= _temp3155.curr +( yyvsp_offset - 2); if( _temp3157 <
_temp3155.base? 1: _temp3157 >= _temp3155.last_plus_one){ _throw( Null_Exception);}*
_temp3157;})); _temp3151->f2=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp3152= yyvs; struct _xenum_struct** _temp3154= _temp3152.curr
+ yyvsp_offset; if( _temp3154 < _temp3152.base? 1: _temp3154 >= _temp3152.last_plus_one){
_throw( Null_Exception);}* _temp3154;})); _temp3151;});( struct _xenum_struct*)
_temp3150;}); break; case 91: _LL3149: { struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3159= yyvs; struct _xenum_struct** _temp3161= _temp3159.curr
+( yyvsp_offset - 2); if( _temp3161 < _temp3159.base? 1: _temp3161 >= _temp3159.last_plus_one){
_throw( Null_Exception);}* _temp3161;}))).f1; struct Cyc_List_List* atts=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3162= yyvs; struct _xenum_struct** _temp3164= _temp3162.curr
+( yyvsp_offset - 2); if( _temp3164 < _temp3162.base? 1: _temp3164 >= _temp3162.last_plus_one){
_throw( Null_Exception);}* _temp3164;}))).f3; void* t= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp3165= yyvs; struct
_xenum_struct** _temp3167= _temp3165.curr +( yyvsp_offset - 2); if( _temp3167 <
_temp3165.base? 1: _temp3167 >= _temp3165.last_plus_one){ _throw( Null_Exception);}*
_temp3167;}))).f2, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3168=
yyls; struct Cyc_Yyltype* _temp3170= _temp3168.curr +( yylsp_offset - 2); if(
_temp3170 < _temp3168.base? 1: _temp3170 >= _temp3168.last_plus_one){ _throw(
Null_Exception);}* _temp3170;}).first_line,({ struct _tagged_ptr3 _temp3171=
yyls; struct Cyc_Yyltype* _temp3173= _temp3171.curr +( yylsp_offset - 2); if(
_temp3173 < _temp3171.base? 1: _temp3173 >= _temp3171.last_plus_one){ _throw(
Null_Exception);}* _temp3173;}).last_line)); struct Cyc_List_List* info= Cyc_Parse_apply_tmss(
tq, t, Cyc_yyget_DeclaratorList_tok(({ struct _tagged_ptr2 _temp3174= yyvs;
struct _xenum_struct** _temp3176= _temp3174.curr +( yyvsp_offset - 1); if(
_temp3176 < _temp3174.base? 1: _temp3176 >= _temp3174.last_plus_one){ _throw(
Null_Exception);}* _temp3176;})), atts); yyval=({ struct Cyc_StructFieldDeclList_tok_struct*
_temp3177=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct)); _temp3177->tag= Cyc_StructFieldDeclList_tok_tag;
_temp3177->f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(* f)(
struct Cyc_Position_Segment*, struct _tuple7*), struct Cyc_Position_Segment* env,
struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_make_struct_field, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3178= yyls; struct Cyc_Yyltype* _temp3180= _temp3178.curr
+( yylsp_offset - 2); if( _temp3180 < _temp3178.base? 1: _temp3180 >= _temp3178.last_plus_one){
_throw( Null_Exception);}* _temp3180;}).first_line,({ struct _tagged_ptr3
_temp3181= yyls; struct Cyc_Yyltype* _temp3183= _temp3181.curr +( yylsp_offset -
1); if( _temp3183 < _temp3181.base? 1: _temp3183 >= _temp3181.last_plus_one){
_throw( Null_Exception);}* _temp3183;}).last_line), info);( struct _xenum_struct*)
_temp3177;}); break;} case 92: _LL3158: yyval=({ struct Cyc_QualSpecList_tok_struct*
_temp3185=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp3185->tag= Cyc_QualSpecList_tok_tag; _temp3185->f1=({ struct _tuple13*
_temp3186=( struct _tuple13*) GC_malloc( sizeof( struct _tuple13)); _temp3186->f1=
Cyc_Absyn_empty_tqual(); _temp3186->f2=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp3190=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3190->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({ struct _tagged_ptr2
_temp3191= yyvs; struct _xenum_struct** _temp3193= _temp3191.curr +(
yyvsp_offset - 1); if( _temp3193 < _temp3191.base? 1: _temp3193 >= _temp3191.last_plus_one){
_throw( Null_Exception);}* _temp3193;})); _temp3190->tl= 0; _temp3190;});
_temp3186->f3= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp3187=
yyvs; struct _xenum_struct** _temp3189= _temp3187.curr + yyvsp_offset; if(
_temp3189 < _temp3187.base? 1: _temp3189 >= _temp3187.last_plus_one){ _throw(
Null_Exception);}* _temp3189;})); _temp3186;});( struct _xenum_struct*)
_temp3185;}); break; case 93: _LL3184: yyval=({ struct Cyc_QualSpecList_tok_struct*
_temp3195=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp3195->tag= Cyc_QualSpecList_tok_tag; _temp3195->f1=({ struct _tuple13*
_temp3196=( struct _tuple13*) GC_malloc( sizeof( struct _tuple13)); _temp3196->f1=(*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp3210= yyvs; struct
_xenum_struct** _temp3212= _temp3210.curr + yyvsp_offset; if( _temp3212 <
_temp3210.base? 1: _temp3212 >= _temp3210.last_plus_one){ _throw( Null_Exception);}*
_temp3212;}))).f1; _temp3196->f2=( struct Cyc_List_List*)({ struct Cyc_List_List*
_temp3203=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3203->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({ struct _tagged_ptr2
_temp3207= yyvs; struct _xenum_struct** _temp3209= _temp3207.curr +(
yyvsp_offset - 2); if( _temp3209 < _temp3207.base? 1: _temp3209 >= _temp3207.last_plus_one){
_throw( Null_Exception);}* _temp3209;})); _temp3203->tl=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3204= yyvs; struct _xenum_struct** _temp3206= _temp3204.curr
+ yyvsp_offset; if( _temp3206 < _temp3204.base? 1: _temp3206 >= _temp3204.last_plus_one){
_throw( Null_Exception);}* _temp3206;}))).f2; _temp3203;}); _temp3196->f3=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp3197= yyvs; struct
_xenum_struct** _temp3199= _temp3197.curr +( yyvsp_offset - 1); if( _temp3199 <
_temp3197.base? 1: _temp3199 >= _temp3197.last_plus_one){ _throw( Null_Exception);}*
_temp3199;})),(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp3200=
yyvs; struct _xenum_struct** _temp3202= _temp3200.curr + yyvsp_offset; if(
_temp3202 < _temp3200.base? 1: _temp3202 >= _temp3200.last_plus_one){ _throw(
Null_Exception);}* _temp3202;}))).f3); _temp3196;});( struct _xenum_struct*)
_temp3195;}); break; case 94: _LL3194: yyval=({ struct Cyc_QualSpecList_tok_struct*
_temp3214=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp3214->tag= Cyc_QualSpecList_tok_tag; _temp3214->f1=({ struct _tuple13*
_temp3215=( struct _tuple13*) GC_malloc( sizeof( struct _tuple13)); _temp3215->f1=
Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2 _temp3219= yyvs; struct
_xenum_struct** _temp3221= _temp3219.curr +( yyvsp_offset - 1); if( _temp3221 <
_temp3219.base? 1: _temp3221 >= _temp3219.last_plus_one){ _throw( Null_Exception);}*
_temp3221;})); _temp3215->f2= 0; _temp3215->f3= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp3216= yyvs; struct _xenum_struct** _temp3218= _temp3216.curr
+ yyvsp_offset; if( _temp3218 < _temp3216.base? 1: _temp3218 >= _temp3216.last_plus_one){
_throw( Null_Exception);}* _temp3218;})); _temp3215;});( struct _xenum_struct*)
_temp3214;}); break; case 95: _LL3213: yyval=({ struct Cyc_QualSpecList_tok_struct*
_temp3223=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));
_temp3223->tag= Cyc_QualSpecList_tok_tag; _temp3223->f1=({ struct _tuple13*
_temp3224=( struct _tuple13*) GC_malloc( sizeof( struct _tuple13)); _temp3224->f1=
Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2 _temp3234=
yyvs; struct _xenum_struct** _temp3236= _temp3234.curr +( yyvsp_offset - 2); if(
_temp3236 < _temp3234.base? 1: _temp3236 >= _temp3234.last_plus_one){ _throw(
Null_Exception);}* _temp3236;})),(* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp3237= yyvs; struct _xenum_struct** _temp3239= _temp3237.curr +
yyvsp_offset; if( _temp3239 < _temp3237.base? 1: _temp3239 >= _temp3237.last_plus_one){
_throw( Null_Exception);}* _temp3239;}))).f1); _temp3224->f2=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3231= yyvs; struct _xenum_struct** _temp3233= _temp3231.curr
+ yyvsp_offset; if( _temp3233 < _temp3231.base? 1: _temp3233 >= _temp3231.last_plus_one){
_throw( Null_Exception);}* _temp3233;}))).f2; _temp3224->f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp3225= yyvs; struct _xenum_struct** _temp3227= _temp3225.curr
+( yyvsp_offset - 1); if( _temp3227 < _temp3225.base? 1: _temp3227 >= _temp3225.last_plus_one){
_throw( Null_Exception);}* _temp3227;})),(* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp3228= yyvs; struct _xenum_struct** _temp3230= _temp3228.curr +
yyvsp_offset; if( _temp3230 < _temp3228.base? 1: _temp3230 >= _temp3228.last_plus_one){
_throw( Null_Exception);}* _temp3230;}))).f3); _temp3224;});( struct
_xenum_struct*) _temp3223;}); break; case 96: _LL3222: yyval=({ struct Cyc_DeclaratorList_tok_struct*
_temp3241=( struct Cyc_DeclaratorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorList_tok_struct));
_temp3241->tag= Cyc_DeclaratorList_tok_tag; _temp3241->f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_DeclaratorList_tok(({
struct _tagged_ptr2 _temp3242= yyvs; struct _xenum_struct** _temp3244= _temp3242.curr
+ yyvsp_offset; if( _temp3244 < _temp3242.base? 1: _temp3244 >= _temp3242.last_plus_one){
_throw( Null_Exception);}* _temp3244;})));( struct _xenum_struct*) _temp3241;});
break; case 97: _LL3240: yyval=({ struct Cyc_DeclaratorList_tok_struct*
_temp3246=( struct Cyc_DeclaratorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorList_tok_struct));
_temp3246->tag= Cyc_DeclaratorList_tok_tag; _temp3246->f1=({ struct Cyc_List_List*
_temp3247=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3247->hd=( void*) Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3248=
yyvs; struct _xenum_struct** _temp3250= _temp3248.curr + yyvsp_offset; if(
_temp3250 < _temp3248.base? 1: _temp3250 >= _temp3248.last_plus_one){ _throw(
Null_Exception);}* _temp3250;})); _temp3247->tl= 0; _temp3247;});( struct
_xenum_struct*) _temp3246;}); break; case 98: _LL3245: yyval=({ struct Cyc_DeclaratorList_tok_struct*
_temp3252=( struct Cyc_DeclaratorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorList_tok_struct));
_temp3252->tag= Cyc_DeclaratorList_tok_tag; _temp3252->f1=({ struct Cyc_List_List*
_temp3253=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3253->hd=( void*) Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3257=
yyvs; struct _xenum_struct** _temp3259= _temp3257.curr + yyvsp_offset; if(
_temp3259 < _temp3257.base? 1: _temp3259 >= _temp3257.last_plus_one){ _throw(
Null_Exception);}* _temp3259;})); _temp3253->tl= Cyc_yyget_DeclaratorList_tok(({
struct _tagged_ptr2 _temp3254= yyvs; struct _xenum_struct** _temp3256= _temp3254.curr
+( yyvsp_offset - 2); if( _temp3256 < _temp3254.base? 1: _temp3256 >= _temp3254.last_plus_one){
_throw( Null_Exception);}* _temp3256;})); _temp3253;});( struct _xenum_struct*)
_temp3252;}); break; case 99: _LL3251: yyval=({ struct _tagged_ptr2 _temp3261=
yyvs; struct _xenum_struct** _temp3263= _temp3261.curr + yyvsp_offset; if(
_temp3263 < _temp3261.base? 1: _temp3263 >= _temp3261.last_plus_one){ _throw(
Null_Exception);}* _temp3263;}); break; case 100: _LL3260: Cyc_Parse_unimp2((
struct _tagged_string)({ char* _temp3265=( char*)"bit fields"; struct
_tagged_string _temp3266; _temp3266.curr= _temp3265; _temp3266.base= _temp3265;
_temp3266.last_plus_one= _temp3265 + 11; _temp3266;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3267= yyls; struct Cyc_Yyltype* _temp3269= _temp3267.curr
+( yylsp_offset - 1); if( _temp3269 < _temp3267.base? 1: _temp3269 >= _temp3267.last_plus_one){
_throw( Null_Exception);}* _temp3269;}).first_line,({ struct _tagged_ptr3
_temp3270= yyls; struct Cyc_Yyltype* _temp3272= _temp3270.curr + yylsp_offset;
if( _temp3272 < _temp3270.base? 1: _temp3272 >= _temp3270.last_plus_one){ _throw(
Null_Exception);}* _temp3272;}).last_line)); yyval=({ struct Cyc_Declarator_tok_struct*
_temp3273=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3273->tag= Cyc_Declarator_tok_tag; _temp3273->f1=({ struct Cyc_Parse_Declarator*
_temp3274=( struct Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp3274->id=({ struct _tuple1* _temp3275=( struct _tuple1*) GC_malloc( sizeof(
struct _tuple1)); _temp3275->f1=({ struct Cyc_Absyn_Rel_n_struct* _temp3279=(
struct Cyc_Absyn_Rel_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct));
_temp3279->tag= Cyc_Absyn_Rel_n_tag; _temp3279->f1= 0;( void*) _temp3279;});
_temp3275->f2=({ struct _tagged_string* _temp3276=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp3276[ 0]=( struct
_tagged_string)({ char* _temp3277=( char*)""; struct _tagged_string _temp3278;
_temp3278.curr= _temp3277; _temp3278.base= _temp3277; _temp3278.last_plus_one=
_temp3277 + 1; _temp3278;}); _temp3276;}); _temp3275;}); _temp3274->tms= 0;
_temp3274;});( struct _xenum_struct*) _temp3273;}); break; case 101: _LL3264:
Cyc_Parse_unimp2(( struct _tagged_string)({ char* _temp3281=( char*)"bit fields";
struct _tagged_string _temp3282; _temp3282.curr= _temp3281; _temp3282.base=
_temp3281; _temp3282.last_plus_one= _temp3281 + 11; _temp3282;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3283= yyls; struct Cyc_Yyltype* _temp3285= _temp3283.curr
+( yylsp_offset - 2); if( _temp3285 < _temp3283.base? 1: _temp3285 >= _temp3283.last_plus_one){
_throw( Null_Exception);}* _temp3285;}).first_line,({ struct _tagged_ptr3
_temp3286= yyls; struct Cyc_Yyltype* _temp3288= _temp3286.curr +( yylsp_offset -
1); if( _temp3288 < _temp3286.base? 1: _temp3288 >= _temp3286.last_plus_one){
_throw( Null_Exception);}* _temp3288;}).last_line)); yyval=({ struct
_tagged_ptr2 _temp3289= yyvs; struct _xenum_struct** _temp3291= _temp3289.curr +(
yyvsp_offset - 2); if( _temp3291 < _temp3289.base? 1: _temp3291 >= _temp3289.last_plus_one){
_throw( Null_Exception);}* _temp3291;}); break; case 102: _LL3280: yyval=({
struct Cyc_TypeSpecifier_tok_struct* _temp3293=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct)); _temp3293->tag= Cyc_TypeSpecifier_tok_tag;
_temp3293->f1=( void*)({ struct Cyc_Parse_Decl_spec_struct* _temp3294=( struct
Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3294->tag= Cyc_Parse_Decl_spec_tag; _temp3294->f1= Cyc_Absyn_enum_decl( Cyc_Absyn_Public,
0, 0,({ struct Cyc_Core_Opt* _temp3295=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp3295->v=( void*) Cyc_yyget_EnumeratorFieldList_tok(({
struct _tagged_ptr2 _temp3296= yyvs; struct _xenum_struct** _temp3298= _temp3296.curr
+( yyvsp_offset - 1); if( _temp3298 < _temp3296.base? 1: _temp3298 >= _temp3296.last_plus_one){
_throw( Null_Exception);}* _temp3298;})); _temp3295;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3299= yyls; struct Cyc_Yyltype* _temp3301= _temp3299.curr
+( yylsp_offset - 3); if( _temp3301 < _temp3299.base? 1: _temp3301 >= _temp3299.last_plus_one){
_throw( Null_Exception);}* _temp3301;}).first_line,({ struct _tagged_ptr3
_temp3302= yyls; struct Cyc_Yyltype* _temp3304= _temp3302.curr + yylsp_offset;
if( _temp3304 < _temp3302.base? 1: _temp3304 >= _temp3302.last_plus_one){ _throw(
Null_Exception);}* _temp3304;}).last_line));( void*) _temp3294;});( struct
_xenum_struct*) _temp3293;}); Cyc_Parse_unimp2(( struct _tagged_string)({ char*
_temp3305=( char*)"anonymous enums"; struct _tagged_string _temp3306; _temp3306.curr=
_temp3305; _temp3306.base= _temp3305; _temp3306.last_plus_one= _temp3305 + 16;
_temp3306;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3307= yyls;
struct Cyc_Yyltype* _temp3309= _temp3307.curr +( yylsp_offset - 3); if(
_temp3309 < _temp3307.base? 1: _temp3309 >= _temp3307.last_plus_one){ _throw(
Null_Exception);}* _temp3309;}).first_line,({ struct _tagged_ptr3 _temp3310=
yyls; struct Cyc_Yyltype* _temp3312= _temp3310.curr + yylsp_offset; if(
_temp3312 < _temp3310.base? 1: _temp3312 >= _temp3310.last_plus_one){ _throw(
Null_Exception);}* _temp3312;}).last_line)); break; case 103: _LL3292: { struct
Cyc_List_List* ts=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3314= yyls; struct Cyc_Yyltype* _temp3316= _temp3314.curr
+( yylsp_offset - 3); if( _temp3316 < _temp3314.base? 1: _temp3316 >= _temp3314.last_plus_one){
_throw( Null_Exception);}* _temp3316;}).first_line,({ struct _tagged_ptr3
_temp3317= yyls; struct Cyc_Yyltype* _temp3319= _temp3317.curr +( yylsp_offset -
3); if( _temp3319 < _temp3317.base? 1: _temp3319 >= _temp3317.last_plus_one){
_throw( Null_Exception);}* _temp3319;}).last_line), Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3320= yyvs; struct _xenum_struct** _temp3322= _temp3320.curr
+( yyvsp_offset - 3); if( _temp3322 < _temp3320.base? 1: _temp3322 >= _temp3320.last_plus_one){
_throw( Null_Exception);}* _temp3322;}))); yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp3323=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3323->tag= Cyc_TypeSpecifier_tok_tag; _temp3323->f1=( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp3324=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3324->tag= Cyc_Parse_Decl_spec_tag; _temp3324->f1= Cyc_Absyn_enum_decl( Cyc_Absyn_Public,({
struct Cyc_Core_Opt* _temp3325=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3325->v=( void*) Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp3326= yyvs; struct _xenum_struct** _temp3328= _temp3326.curr +(
yyvsp_offset - 4); if( _temp3328 < _temp3326.base? 1: _temp3328 >= _temp3326.last_plus_one){
_throw( Null_Exception);}* _temp3328;})); _temp3325;}), ts,({ struct Cyc_Core_Opt*
_temp3329=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3329->v=( void*) Cyc_yyget_EnumeratorFieldList_tok(({ struct _tagged_ptr2
_temp3330= yyvs; struct _xenum_struct** _temp3332= _temp3330.curr +(
yyvsp_offset - 1); if( _temp3332 < _temp3330.base? 1: _temp3332 >= _temp3330.last_plus_one){
_throw( Null_Exception);}* _temp3332;})); _temp3329;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3333= yyls; struct Cyc_Yyltype* _temp3335= _temp3333.curr
+( yylsp_offset - 5); if( _temp3335 < _temp3333.base? 1: _temp3335 >= _temp3333.last_plus_one){
_throw( Null_Exception);}* _temp3335;}).first_line,({ struct _tagged_ptr3
_temp3336= yyls; struct Cyc_Yyltype* _temp3338= _temp3336.curr + yylsp_offset;
if( _temp3338 < _temp3336.base? 1: _temp3338 >= _temp3336.last_plus_one){ _throw(
Null_Exception);}* _temp3338;}).last_line));( void*) _temp3324;});( struct
_xenum_struct*) _temp3323;}); break;} case 104: _LL3313: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp3340=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3340->tag= Cyc_TypeSpecifier_tok_tag; _temp3340->f1=( void*) Cyc_Parse_type_spec(({
struct Cyc_Absyn_EnumType_struct* _temp3341=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp3341->tag= Cyc_Absyn_EnumType_tag;
_temp3341->f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3342= yyvs; struct _xenum_struct** _temp3344= _temp3342.curr +(
yyvsp_offset - 1); if( _temp3344 < _temp3342.base? 1: _temp3344 >= _temp3342.last_plus_one){
_throw( Null_Exception);}* _temp3344;})); _temp3341->f2= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3345= yyvs; struct _xenum_struct** _temp3347= _temp3345.curr
+ yyvsp_offset; if( _temp3347 < _temp3345.base? 1: _temp3347 >= _temp3345.last_plus_one){
_throw( Null_Exception);}* _temp3347;})); _temp3341->f3= 0;( void*) _temp3341;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3348= yyls; struct Cyc_Yyltype*
_temp3350= _temp3348.curr +( yylsp_offset - 2); if( _temp3350 < _temp3348.base?
1: _temp3350 >= _temp3348.last_plus_one){ _throw( Null_Exception);}* _temp3350;}).first_line,({
struct _tagged_ptr3 _temp3351= yyls; struct Cyc_Yyltype* _temp3353= _temp3351.curr
+ yylsp_offset; if( _temp3353 < _temp3351.base? 1: _temp3353 >= _temp3351.last_plus_one){
_throw( Null_Exception);}* _temp3353;}).last_line));( struct _xenum_struct*)
_temp3340;}); break; case 105: _LL3339: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp3355=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3355->tag= Cyc_TypeSpecifier_tok_tag; _temp3355->f1=( void*)({ struct Cyc_Parse_Decl_spec_struct*
_temp3356=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3356->tag= Cyc_Parse_Decl_spec_tag; _temp3356->f1= Cyc_Absyn_xenum_decl(
Cyc_Absyn_Public, Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3357= yyvs;
struct _xenum_struct** _temp3359= _temp3357.curr +( yyvsp_offset - 3); if(
_temp3359 < _temp3357.base? 1: _temp3359 >= _temp3357.last_plus_one){ _throw(
Null_Exception);}* _temp3359;})), Cyc_yyget_EnumeratorFieldList_tok(({ struct
_tagged_ptr2 _temp3360= yyvs; struct _xenum_struct** _temp3362= _temp3360.curr +(
yyvsp_offset - 1); if( _temp3362 < _temp3360.base? 1: _temp3362 >= _temp3360.last_plus_one){
_throw( Null_Exception);}* _temp3362;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3363= yyls; struct Cyc_Yyltype* _temp3365= _temp3363.curr +(
yylsp_offset - 4); if( _temp3365 < _temp3363.base? 1: _temp3365 >= _temp3363.last_plus_one){
_throw( Null_Exception);}* _temp3365;}).first_line,({ struct _tagged_ptr3
_temp3366= yyls; struct Cyc_Yyltype* _temp3368= _temp3366.curr + yylsp_offset;
if( _temp3368 < _temp3366.base? 1: _temp3368 >= _temp3366.last_plus_one){ _throw(
Null_Exception);}* _temp3368;}).last_line));( void*) _temp3356;});( struct
_xenum_struct*) _temp3355;}); break; case 106: _LL3354: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp3370=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));
_temp3370->tag= Cyc_TypeSpecifier_tok_tag; _temp3370->f1=( void*) Cyc_Parse_type_spec(({
struct Cyc_Absyn_XenumType_struct* _temp3371=( struct Cyc_Absyn_XenumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XenumType_struct)); _temp3371->tag= Cyc_Absyn_XenumType_tag;
_temp3371->f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3372= yyvs;
struct _xenum_struct** _temp3374= _temp3372.curr + yyvsp_offset; if( _temp3374 <
_temp3372.base? 1: _temp3374 >= _temp3372.last_plus_one){ _throw( Null_Exception);}*
_temp3374;})); _temp3371->f2= 0;( void*) _temp3371;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3375= yyls; struct Cyc_Yyltype* _temp3377= _temp3375.curr
+( yylsp_offset - 1); if( _temp3377 < _temp3375.base? 1: _temp3377 >= _temp3375.last_plus_one){
_throw( Null_Exception);}* _temp3377;}).first_line,({ struct _tagged_ptr3
_temp3378= yyls; struct Cyc_Yyltype* _temp3380= _temp3378.curr + yylsp_offset;
if( _temp3380 < _temp3378.base? 1: _temp3380 >= _temp3378.last_plus_one){ _throw(
Null_Exception);}* _temp3380;}).last_line));( struct _xenum_struct*) _temp3370;});
break; case 107: _LL3369: yyval=({ struct Cyc_EnumeratorFieldList_tok_struct*
_temp3382=( struct Cyc_EnumeratorFieldList_tok_struct*) GC_malloc( sizeof(
struct Cyc_EnumeratorFieldList_tok_struct)); _temp3382->tag= Cyc_EnumeratorFieldList_tok_tag;
_temp3382->f1=({ struct Cyc_List_List* _temp3383=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3383->hd=( void*) Cyc_yyget_EnumeratorField_tok(({
struct _tagged_ptr2 _temp3384= yyvs; struct _xenum_struct** _temp3386= _temp3384.curr
+ yyvsp_offset; if( _temp3386 < _temp3384.base? 1: _temp3386 >= _temp3384.last_plus_one){
_throw( Null_Exception);}* _temp3386;})); _temp3383->tl= 0; _temp3383;});(
struct _xenum_struct*) _temp3382;}); break; case 108: _LL3381: yyval=({ struct
Cyc_EnumeratorFieldList_tok_struct* _temp3388=( struct Cyc_EnumeratorFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_EnumeratorFieldList_tok_struct)); _temp3388->tag=
Cyc_EnumeratorFieldList_tok_tag; _temp3388->f1=({ struct Cyc_List_List*
_temp3389=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3389->hd=( void*) Cyc_yyget_EnumeratorField_tok(({ struct _tagged_ptr2
_temp3390= yyvs; struct _xenum_struct** _temp3392= _temp3390.curr +(
yyvsp_offset - 1); if( _temp3392 < _temp3390.base? 1: _temp3392 >= _temp3390.last_plus_one){
_throw( Null_Exception);}* _temp3392;})); _temp3389->tl= 0; _temp3389;});(
struct _xenum_struct*) _temp3388;}); break; case 109: _LL3387: yyval=({ struct
Cyc_EnumeratorFieldList_tok_struct* _temp3394=( struct Cyc_EnumeratorFieldList_tok_struct*)
GC_malloc( sizeof( struct Cyc_EnumeratorFieldList_tok_struct)); _temp3394->tag=
Cyc_EnumeratorFieldList_tok_tag; _temp3394->f1=({ struct Cyc_List_List*
_temp3395=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3395->hd=( void*) Cyc_yyget_EnumeratorField_tok(({ struct _tagged_ptr2
_temp3399= yyvs; struct _xenum_struct** _temp3401= _temp3399.curr +(
yyvsp_offset - 2); if( _temp3401 < _temp3399.base? 1: _temp3401 >= _temp3399.last_plus_one){
_throw( Null_Exception);}* _temp3401;})); _temp3395->tl= Cyc_yyget_EnumeratorFieldList_tok(({
struct _tagged_ptr2 _temp3396= yyvs; struct _xenum_struct** _temp3398= _temp3396.curr
+ yyvsp_offset; if( _temp3398 < _temp3396.base? 1: _temp3398 >= _temp3396.last_plus_one){
_throw( Null_Exception);}* _temp3398;})); _temp3395;});( struct _xenum_struct*)
_temp3394;}); break; case 110: _LL3393: yyval=({ struct Cyc_EnumeratorFieldList_tok_struct*
_temp3403=( struct Cyc_EnumeratorFieldList_tok_struct*) GC_malloc( sizeof(
struct Cyc_EnumeratorFieldList_tok_struct)); _temp3403->tag= Cyc_EnumeratorFieldList_tok_tag;
_temp3403->f1=({ struct Cyc_List_List* _temp3404=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3404->hd=( void*) Cyc_yyget_EnumeratorField_tok(({
struct _tagged_ptr2 _temp3408= yyvs; struct _xenum_struct** _temp3410= _temp3408.curr
+( yyvsp_offset - 2); if( _temp3410 < _temp3408.base? 1: _temp3410 >= _temp3408.last_plus_one){
_throw( Null_Exception);}* _temp3410;})); _temp3404->tl= Cyc_yyget_EnumeratorFieldList_tok(({
struct _tagged_ptr2 _temp3405= yyvs; struct _xenum_struct** _temp3407= _temp3405.curr
+ yyvsp_offset; if( _temp3407 < _temp3405.base? 1: _temp3407 >= _temp3405.last_plus_one){
_throw( Null_Exception);}* _temp3407;})); _temp3404;});( struct _xenum_struct*)
_temp3403;}); break; case 111: _LL3402: yyval=({ struct Cyc_EnumeratorField_tok_struct*
_temp3412=( struct Cyc_EnumeratorField_tok_struct*) GC_malloc( sizeof( struct
Cyc_EnumeratorField_tok_struct)); _temp3412->tag= Cyc_EnumeratorField_tok_tag;
_temp3412->f1=({ struct Cyc_Absyn_Enumfield* _temp3413=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp3413->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3420= yyvs; struct _xenum_struct** _temp3422= _temp3420.curr
+ yyvsp_offset; if( _temp3422 < _temp3420.base? 1: _temp3422 >= _temp3420.last_plus_one){
_throw( Null_Exception);}* _temp3422;})); _temp3413->tag= 0; _temp3413->tvs= 0;
_temp3413->typs= 0; _temp3413->loc= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3414= yyls; struct Cyc_Yyltype* _temp3416= _temp3414.curr +
yylsp_offset; if( _temp3416 < _temp3414.base? 1: _temp3416 >= _temp3414.last_plus_one){
_throw( Null_Exception);}* _temp3416;}).first_line,({ struct _tagged_ptr3
_temp3417= yyls; struct Cyc_Yyltype* _temp3419= _temp3417.curr + yylsp_offset;
if( _temp3419 < _temp3417.base? 1: _temp3419 >= _temp3417.last_plus_one){ _throw(
Null_Exception);}* _temp3419;}).last_line); _temp3413;});( struct _xenum_struct*)
_temp3412;}); break; case 112: _LL3411: yyval=({ struct Cyc_EnumeratorField_tok_struct*
_temp3424=( struct Cyc_EnumeratorField_tok_struct*) GC_malloc( sizeof( struct
Cyc_EnumeratorField_tok_struct)); _temp3424->tag= Cyc_EnumeratorField_tok_tag;
_temp3424->f1=({ struct Cyc_Absyn_Enumfield* _temp3425=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp3425->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3435= yyvs; struct _xenum_struct** _temp3437= _temp3435.curr
+( yyvsp_offset - 2); if( _temp3437 < _temp3435.base? 1: _temp3437 >= _temp3435.last_plus_one){
_throw( Null_Exception);}* _temp3437;})); _temp3425->tag=( struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp3432= yyvs; struct _xenum_struct**
_temp3434= _temp3432.curr + yyvsp_offset; if( _temp3434 < _temp3432.base? 1:
_temp3434 >= _temp3432.last_plus_one){ _throw( Null_Exception);}* _temp3434;}));
_temp3425->tvs= 0; _temp3425->typs= 0; _temp3425->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3426= yyls; struct Cyc_Yyltype* _temp3428= _temp3426.curr
+( yylsp_offset - 2); if( _temp3428 < _temp3426.base? 1: _temp3428 >= _temp3426.last_plus_one){
_throw( Null_Exception);}* _temp3428;}).first_line,({ struct _tagged_ptr3
_temp3429= yyls; struct Cyc_Yyltype* _temp3431= _temp3429.curr + yylsp_offset;
if( _temp3431 < _temp3429.base? 1: _temp3431 >= _temp3429.last_plus_one){ _throw(
Null_Exception);}* _temp3431;}).last_line); _temp3425;});( struct _xenum_struct*)
_temp3424;}); break; case 113: _LL3423: { struct Cyc_List_List* typs=(( struct
Cyc_List_List*(*)( struct _tuple9*(* f)( struct Cyc_Position_Segment*, struct
_tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp3439= yyls; struct Cyc_Yyltype* _temp3441= _temp3439.curr +( yylsp_offset -
1); if( _temp3441 < _temp3439.base? 1: _temp3441 >= _temp3439.last_plus_one){
_throw( Null_Exception);}* _temp3441;}).first_line,({ struct _tagged_ptr3
_temp3442= yyls; struct Cyc_Yyltype* _temp3444= _temp3442.curr +( yylsp_offset -
1); if( _temp3444 < _temp3442.base? 1: _temp3444 >= _temp3442.last_plus_one){
_throw( Null_Exception);}* _temp3444;}).last_line),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp3445= yyvs; struct _xenum_struct** _temp3447= _temp3445.curr
+( yyvsp_offset - 1); if( _temp3447 < _temp3445.base? 1: _temp3447 >= _temp3445.last_plus_one){
_throw( Null_Exception);}* _temp3447;})))); struct Cyc_List_List* tvs=(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3448=
yyls; struct Cyc_Yyltype* _temp3450= _temp3448.curr +( yylsp_offset - 3); if(
_temp3450 < _temp3448.base? 1: _temp3450 >= _temp3448.last_plus_one){ _throw(
Null_Exception);}* _temp3450;}).first_line,({ struct _tagged_ptr3 _temp3451=
yyls; struct Cyc_Yyltype* _temp3453= _temp3451.curr +( yylsp_offset - 3); if(
_temp3453 < _temp3451.base? 1: _temp3453 >= _temp3451.last_plus_one){ _throw(
Null_Exception);}* _temp3453;}).last_line), Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp3454= yyvs; struct _xenum_struct** _temp3456= _temp3454.curr +(
yyvsp_offset - 3); if( _temp3456 < _temp3454.base? 1: _temp3456 >= _temp3454.last_plus_one){
_throw( Null_Exception);}* _temp3456;}))); yyval=({ struct Cyc_EnumeratorField_tok_struct*
_temp3457=( struct Cyc_EnumeratorField_tok_struct*) GC_malloc( sizeof( struct
Cyc_EnumeratorField_tok_struct)); _temp3457->tag= Cyc_EnumeratorField_tok_tag;
_temp3457->f1=({ struct Cyc_Absyn_Enumfield* _temp3458=( struct Cyc_Absyn_Enumfield*)
GC_malloc( sizeof( struct Cyc_Absyn_Enumfield)); _temp3458->name= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3465= yyvs; struct _xenum_struct** _temp3467= _temp3465.curr
+( yyvsp_offset - 4); if( _temp3467 < _temp3465.base? 1: _temp3467 >= _temp3465.last_plus_one){
_throw( Null_Exception);}* _temp3467;})); _temp3458->tag= 0; _temp3458->tvs= tvs;
_temp3458->typs= typs; _temp3458->loc= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3459= yyls; struct Cyc_Yyltype* _temp3461= _temp3459.curr +(
yylsp_offset - 4); if( _temp3461 < _temp3459.base? 1: _temp3461 >= _temp3459.last_plus_one){
_throw( Null_Exception);}* _temp3461;}).first_line,({ struct _tagged_ptr3
_temp3462= yyls; struct Cyc_Yyltype* _temp3464= _temp3462.curr + yylsp_offset;
if( _temp3464 < _temp3462.base? 1: _temp3464 >= _temp3462.last_plus_one){ _throw(
Null_Exception);}* _temp3464;}).last_line); _temp3458;});( struct _xenum_struct*)
_temp3457;}); break;} case 114: _LL3438: yyval=({ struct _tagged_ptr2 _temp3469=
yyvs; struct _xenum_struct** _temp3471= _temp3469.curr + yyvsp_offset; if(
_temp3471 < _temp3469.base? 1: _temp3471 >= _temp3469.last_plus_one){ _throw(
Null_Exception);}* _temp3471;}); break; case 115: _LL3468: yyval=({ struct Cyc_Declarator_tok_struct*
_temp3473=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3473->tag= Cyc_Declarator_tok_tag; _temp3473->f1=({ struct Cyc_Parse_Declarator*
_temp3474=( struct Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp3474->id=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3481= yyvs;
struct _xenum_struct** _temp3483= _temp3481.curr + yyvsp_offset; if( _temp3483 <
_temp3481.base? 1: _temp3483 >= _temp3481.last_plus_one){ _throw( Null_Exception);}*
_temp3483;})))->id; _temp3474->tms=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_TypeModifierList_tok(({
struct _tagged_ptr2 _temp3475= yyvs; struct _xenum_struct** _temp3477= _temp3475.curr
+( yyvsp_offset - 1); if( _temp3477 < _temp3475.base? 1: _temp3477 >= _temp3475.last_plus_one){
_throw( Null_Exception);}* _temp3477;})),( Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp3478= yyvs; struct _xenum_struct** _temp3480= _temp3478.curr +
yyvsp_offset; if( _temp3480 < _temp3478.base? 1: _temp3480 >= _temp3478.last_plus_one){
_throw( Null_Exception);}* _temp3480;})))->tms); _temp3474;});( struct
_xenum_struct*) _temp3473;}); break; case 116: _LL3472: yyval=({ struct Cyc_Declarator_tok_struct*
_temp3485=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3485->tag= Cyc_Declarator_tok_tag; _temp3485->f1=({ struct Cyc_Parse_Declarator*
_temp3486=( struct Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp3486->id= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3487= yyvs;
struct _xenum_struct** _temp3489= _temp3487.curr + yyvsp_offset; if( _temp3489 <
_temp3487.base? 1: _temp3489 >= _temp3487.last_plus_one){ _throw( Null_Exception);}*
_temp3489;})); _temp3486->tms= 0; _temp3486;});( struct _xenum_struct*)
_temp3485;}); break; case 117: _LL3484: yyval=({ struct _tagged_ptr2 _temp3491=
yyvs; struct _xenum_struct** _temp3493= _temp3491.curr +( yyvsp_offset - 1); if(
_temp3493 < _temp3491.base? 1: _temp3493 >= _temp3491.last_plus_one){ _throw(
Null_Exception);}* _temp3493;}); break; case 118: _LL3490: yyval=({ struct Cyc_Declarator_tok_struct*
_temp3495=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3495->tag= Cyc_Declarator_tok_tag; _temp3495->f1=({ struct Cyc_Parse_Declarator*
_temp3496=( struct Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp3496->id=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3501= yyvs;
struct _xenum_struct** _temp3503= _temp3501.curr +( yyvsp_offset - 2); if(
_temp3503 < _temp3501.base? 1: _temp3503 >= _temp3501.last_plus_one){ _throw(
Null_Exception);}* _temp3503;})))->id; _temp3496->tms=({ struct Cyc_List_List*
_temp3497=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3497->hd=( void*) Cyc_Absyn_Carray_mod; _temp3497->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3498= yyvs; struct _xenum_struct** _temp3500= _temp3498.curr
+( yyvsp_offset - 2); if( _temp3500 < _temp3498.base? 1: _temp3500 >= _temp3498.last_plus_one){
_throw( Null_Exception);}* _temp3500;})))->tms; _temp3497;}); _temp3496;});(
struct _xenum_struct*) _temp3495;}); break; case 119: _LL3494: yyval=({ struct
Cyc_Declarator_tok_struct* _temp3505=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp3505->tag= Cyc_Declarator_tok_tag;
_temp3505->f1=({ struct Cyc_Parse_Declarator* _temp3506=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3506->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3515= yyvs; struct _xenum_struct** _temp3517= _temp3515.curr
+( yyvsp_offset - 3); if( _temp3517 < _temp3515.base? 1: _temp3517 >= _temp3515.last_plus_one){
_throw( Null_Exception);}* _temp3517;})))->id; _temp3506->tms=({ struct Cyc_List_List*
_temp3507=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3507->hd=( void*)({ struct Cyc_Absyn_ConstArray_mod_struct* _temp3511=(
struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct));
_temp3511->tag= Cyc_Absyn_ConstArray_mod_tag; _temp3511->f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp3512= yyvs; struct _xenum_struct** _temp3514= _temp3512.curr
+( yyvsp_offset - 1); if( _temp3514 < _temp3512.base? 1: _temp3514 >= _temp3512.last_plus_one){
_throw( Null_Exception);}* _temp3514;}));( void*) _temp3511;}); _temp3507->tl=(
Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3508= yyvs; struct
_xenum_struct** _temp3510= _temp3508.curr +( yyvsp_offset - 3); if( _temp3510 <
_temp3508.base? 1: _temp3510 >= _temp3508.last_plus_one){ _throw( Null_Exception);}*
_temp3510;})))->tms; _temp3507;}); _temp3506;});( struct _xenum_struct*)
_temp3505;}); break; case 120: _LL3504: { struct _tuple14 _temp3524; struct Cyc_Core_Opt*
_temp3525; int _temp3527; struct Cyc_List_List* _temp3529; struct _tuple14*
_temp3522= Cyc_yyget_ParamDeclListBool_tok(({ struct _tagged_ptr2 _temp3519=
yyvs; struct _xenum_struct** _temp3521= _temp3519.curr +( yyvsp_offset - 1); if(
_temp3521 < _temp3519.base? 1: _temp3521 >= _temp3519.last_plus_one){ _throw(
Null_Exception);}* _temp3521;})); _temp3524=* _temp3522; _LL3530: _temp3529=
_temp3524.f1; goto _LL3528; _LL3528: _temp3527= _temp3524.f2; goto _LL3526;
_LL3526: _temp3525= _temp3524.f3; goto _LL3523; _LL3523: yyval=({ struct Cyc_Declarator_tok_struct*
_temp3531=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3531->tag= Cyc_Declarator_tok_tag; _temp3531->f1=({ struct Cyc_Parse_Declarator*
_temp3532=( struct Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp3532->id=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3539= yyvs;
struct _xenum_struct** _temp3541= _temp3539.curr +( yyvsp_offset - 3); if(
_temp3541 < _temp3539.base? 1: _temp3541 >= _temp3539.last_plus_one){ _throw(
Null_Exception);}* _temp3541;})))->id; _temp3532->tms=({ struct Cyc_List_List*
_temp3533=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3533->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct* _temp3537=(
struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3537->tag= Cyc_Absyn_Function_mod_tag; _temp3537->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp3538=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3538->tag= Cyc_Absyn_WithTypes_tag; _temp3538->f1= _temp3529; _temp3538->f2=
_temp3527; _temp3538->f3= _temp3525;( void*) _temp3538;});( void*) _temp3537;});
_temp3533->tl=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3534= yyvs;
struct _xenum_struct** _temp3536= _temp3534.curr +( yyvsp_offset - 3); if(
_temp3536 < _temp3534.base? 1: _temp3536 >= _temp3534.last_plus_one){ _throw(
Null_Exception);}* _temp3536;})))->tms; _temp3533;}); _temp3532;});( struct
_xenum_struct*) _temp3531;}); break;} case 121: _LL3518: yyval=({ struct Cyc_Declarator_tok_struct*
_temp3543=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3543->tag= Cyc_Declarator_tok_tag; _temp3543->f1=({ struct Cyc_Parse_Declarator*
_temp3544=( struct Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp3544->id=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3551= yyvs;
struct _xenum_struct** _temp3553= _temp3551.curr +( yyvsp_offset - 2); if(
_temp3553 < _temp3551.base? 1: _temp3553 >= _temp3551.last_plus_one){ _throw(
Null_Exception);}* _temp3553;})))->id; _temp3544->tms=({ struct Cyc_List_List*
_temp3545=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3545->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct* _temp3549=(
struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3549->tag= Cyc_Absyn_Function_mod_tag; _temp3549->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp3550=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3550->tag= Cyc_Absyn_WithTypes_tag; _temp3550->f1= 0; _temp3550->f2= 0;
_temp3550->f3= 0;( void*) _temp3550;});( void*) _temp3549;}); _temp3545->tl=(
Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3546= yyvs; struct
_xenum_struct** _temp3548= _temp3546.curr +( yyvsp_offset - 2); if( _temp3548 <
_temp3546.base? 1: _temp3548 >= _temp3546.last_plus_one){ _throw( Null_Exception);}*
_temp3548;})))->tms; _temp3545;}); _temp3544;});( struct _xenum_struct*)
_temp3543;}); break; case 122: _LL3542: yyval=({ struct Cyc_Declarator_tok_struct*
_temp3555=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3555->tag= Cyc_Declarator_tok_tag; _temp3555->f1=({ struct Cyc_Parse_Declarator*
_temp3556=( struct Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp3556->id=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3568= yyvs;
struct _xenum_struct** _temp3570= _temp3568.curr +( yyvsp_offset - 4); if(
_temp3570 < _temp3568.base? 1: _temp3570 >= _temp3568.last_plus_one){ _throw(
Null_Exception);}* _temp3570;})))->id; _temp3556->tms=({ struct Cyc_List_List*
_temp3557=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3557->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct* _temp3561=(
struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3561->tag= Cyc_Absyn_Function_mod_tag; _temp3561->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp3562=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3562->tag= Cyc_Absyn_WithTypes_tag; _temp3562->f1= 0; _temp3562->f2= 0;
_temp3562->f3=({ struct Cyc_Core_Opt* _temp3563=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3563->v=( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp3564=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3564->tag= Cyc_Absyn_JoinEff_tag; _temp3564->f1= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3565= yyvs; struct _xenum_struct** _temp3567= _temp3565.curr
+( yyvsp_offset - 1); if( _temp3567 < _temp3565.base? 1: _temp3567 >= _temp3565.last_plus_one){
_throw( Null_Exception);}* _temp3567;}));( void*) _temp3564;}); _temp3563;});(
void*) _temp3562;});( void*) _temp3561;}); _temp3557->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3558= yyvs; struct _xenum_struct** _temp3560= _temp3558.curr
+( yyvsp_offset - 4); if( _temp3560 < _temp3558.base? 1: _temp3560 >= _temp3558.last_plus_one){
_throw( Null_Exception);}* _temp3560;})))->tms; _temp3557;}); _temp3556;});(
struct _xenum_struct*) _temp3555;}); break; case 123: _LL3554: yyval=({ struct
Cyc_Declarator_tok_struct* _temp3572=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp3572->tag= Cyc_Declarator_tok_tag;
_temp3572->f1=({ struct Cyc_Parse_Declarator* _temp3573=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3573->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3589= yyvs; struct _xenum_struct** _temp3591= _temp3589.curr
+( yyvsp_offset - 3); if( _temp3591 < _temp3589.base? 1: _temp3591 >= _temp3589.last_plus_one){
_throw( Null_Exception);}* _temp3591;})))->id; _temp3573->tms=({ struct Cyc_List_List*
_temp3574=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3574->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct* _temp3578=(
struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3578->tag= Cyc_Absyn_Function_mod_tag; _temp3578->f1=( void*)({ struct Cyc_Absyn_NoTypes_struct*
_temp3579=( struct Cyc_Absyn_NoTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_NoTypes_struct));
_temp3579->tag= Cyc_Absyn_NoTypes_tag; _temp3579->f1= Cyc_yyget_IdList_tok(({
struct _tagged_ptr2 _temp3580= yyvs; struct _xenum_struct** _temp3582= _temp3580.curr
+( yyvsp_offset - 1); if( _temp3582 < _temp3580.base? 1: _temp3582 >= _temp3580.last_plus_one){
_throw( Null_Exception);}* _temp3582;})); _temp3579->f2= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3583= yyls; struct Cyc_Yyltype* _temp3585= _temp3583.curr
+( yylsp_offset - 3); if( _temp3585 < _temp3583.base? 1: _temp3585 >= _temp3583.last_plus_one){
_throw( Null_Exception);}* _temp3585;}).first_line,({ struct _tagged_ptr3
_temp3586= yyls; struct Cyc_Yyltype* _temp3588= _temp3586.curr + yylsp_offset;
if( _temp3588 < _temp3586.base? 1: _temp3588 >= _temp3586.last_plus_one){ _throw(
Null_Exception);}* _temp3588;}).last_line);( void*) _temp3579;});( void*)
_temp3578;}); _temp3574->tl=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2
_temp3575= yyvs; struct _xenum_struct** _temp3577= _temp3575.curr +(
yyvsp_offset - 3); if( _temp3577 < _temp3575.base? 1: _temp3577 >= _temp3575.last_plus_one){
_throw( Null_Exception);}* _temp3577;})))->tms; _temp3574;}); _temp3573;});(
struct _xenum_struct*) _temp3572;}); break; case 124: _LL3571: { struct Cyc_List_List*
ts=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3593=
yyls; struct Cyc_Yyltype* _temp3595= _temp3593.curr +( yylsp_offset - 2); if(
_temp3595 < _temp3593.base? 1: _temp3595 >= _temp3593.last_plus_one){ _throw(
Null_Exception);}* _temp3595;}).first_line,({ struct _tagged_ptr3 _temp3596=
yyls; struct Cyc_Yyltype* _temp3598= _temp3596.curr + yylsp_offset; if(
_temp3598 < _temp3596.base? 1: _temp3598 >= _temp3596.last_plus_one){ _throw(
Null_Exception);}* _temp3598;}).last_line),(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp3599= yyvs; struct _xenum_struct** _temp3601= _temp3599.curr +(
yyvsp_offset - 1); if( _temp3601 < _temp3599.base? 1: _temp3601 >= _temp3599.last_plus_one){
_throw( Null_Exception);}* _temp3601;})))); yyval=({ struct Cyc_Declarator_tok_struct*
_temp3602=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3602->tag= Cyc_Declarator_tok_tag; _temp3602->f1=({ struct Cyc_Parse_Declarator*
_temp3603=( struct Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp3603->id=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3615= yyvs;
struct _xenum_struct** _temp3617= _temp3615.curr +( yyvsp_offset - 3); if(
_temp3617 < _temp3615.base? 1: _temp3617 >= _temp3615.last_plus_one){ _throw(
Null_Exception);}* _temp3617;})))->id; _temp3603->tms=({ struct Cyc_List_List*
_temp3604=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3604->hd=( void*)({ struct Cyc_Absyn_TypeParams_mod_struct* _temp3608=(
struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct));
_temp3608->tag= Cyc_Absyn_TypeParams_mod_tag; _temp3608->f1= ts; _temp3608->f2=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3609= yyls; struct Cyc_Yyltype*
_temp3611= _temp3609.curr +( yylsp_offset - 3); if( _temp3611 < _temp3609.base?
1: _temp3611 >= _temp3609.last_plus_one){ _throw( Null_Exception);}* _temp3611;}).first_line,({
struct _tagged_ptr3 _temp3612= yyls; struct Cyc_Yyltype* _temp3614= _temp3612.curr
+ yylsp_offset; if( _temp3614 < _temp3612.base? 1: _temp3614 >= _temp3612.last_plus_one){
_throw( Null_Exception);}* _temp3614;}).last_line); _temp3608->f3= 0;( void*)
_temp3608;}); _temp3604->tl=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2
_temp3605= yyvs; struct _xenum_struct** _temp3607= _temp3605.curr +(
yyvsp_offset - 3); if( _temp3607 < _temp3605.base? 1: _temp3607 >= _temp3605.last_plus_one){
_throw( Null_Exception);}* _temp3607;})))->tms; _temp3604;}); _temp3603;});(
struct _xenum_struct*) _temp3602;}); break;} case 125: _LL3592:(( struct Cyc_Lexing_lexbuf*)
Cyc_Parse_lbuf->v)->lex_curr_pos -= 1;{ struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3619= yyls; struct Cyc_Yyltype* _temp3621= _temp3619.curr
+( yylsp_offset - 2); if( _temp3621 < _temp3619.base? 1: _temp3621 >= _temp3619.last_plus_one){
_throw( Null_Exception);}* _temp3621;}).first_line,({ struct _tagged_ptr3
_temp3622= yyls; struct Cyc_Yyltype* _temp3624= _temp3622.curr + yylsp_offset;
if( _temp3624 < _temp3622.base? 1: _temp3624 >= _temp3622.last_plus_one){ _throw(
Null_Exception);}* _temp3624;}).last_line),(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp3625= yyvs; struct _xenum_struct** _temp3627= _temp3625.curr +(
yyvsp_offset - 1); if( _temp3627 < _temp3625.base? 1: _temp3627 >= _temp3625.last_plus_one){
_throw( Null_Exception);}* _temp3627;})))); yyval=({ struct Cyc_Declarator_tok_struct*
_temp3628=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));
_temp3628->tag= Cyc_Declarator_tok_tag; _temp3628->f1=({ struct Cyc_Parse_Declarator*
_temp3629=( struct Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp3629->id=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3641= yyvs;
struct _xenum_struct** _temp3643= _temp3641.curr +( yyvsp_offset - 3); if(
_temp3643 < _temp3641.base? 1: _temp3643 >= _temp3641.last_plus_one){ _throw(
Null_Exception);}* _temp3643;})))->id; _temp3629->tms=({ struct Cyc_List_List*
_temp3630=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3630->hd=( void*)({ struct Cyc_Absyn_TypeParams_mod_struct* _temp3634=(
struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct));
_temp3634->tag= Cyc_Absyn_TypeParams_mod_tag; _temp3634->f1= ts; _temp3634->f2=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3635= yyls; struct Cyc_Yyltype*
_temp3637= _temp3635.curr +( yylsp_offset - 3); if( _temp3637 < _temp3635.base?
1: _temp3637 >= _temp3635.last_plus_one){ _throw( Null_Exception);}* _temp3637;}).first_line,({
struct _tagged_ptr3 _temp3638= yyls; struct Cyc_Yyltype* _temp3640= _temp3638.curr
+ yylsp_offset; if( _temp3640 < _temp3638.base? 1: _temp3640 >= _temp3638.last_plus_one){
_throw( Null_Exception);}* _temp3640;}).last_line); _temp3634->f3= 0;( void*)
_temp3634;}); _temp3630->tl=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2
_temp3631= yyvs; struct _xenum_struct** _temp3633= _temp3631.curr +(
yyvsp_offset - 3); if( _temp3633 < _temp3631.base? 1: _temp3633 >= _temp3631.last_plus_one){
_throw( Null_Exception);}* _temp3633;})))->tms; _temp3630;}); _temp3629;});(
struct _xenum_struct*) _temp3628;}); break;} case 126: _LL3618: yyval=({ struct
Cyc_Declarator_tok_struct* _temp3645=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct)); _temp3645->tag= Cyc_Declarator_tok_tag;
_temp3645->f1=({ struct Cyc_Parse_Declarator* _temp3646=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp3646->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3661= yyvs; struct _xenum_struct** _temp3663= _temp3661.curr
+( yyvsp_offset - 1); if( _temp3663 < _temp3661.base? 1: _temp3663 >= _temp3661.last_plus_one){
_throw( Null_Exception);}* _temp3663;})))->id; _temp3646->tms=({ struct Cyc_List_List*
_temp3647=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3647->hd=( void*)({ struct Cyc_Absyn_Attributes_mod_struct* _temp3651=(
struct Cyc_Absyn_Attributes_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct));
_temp3651->tag= Cyc_Absyn_Attributes_mod_tag; _temp3651->f1= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3652= yyls; struct Cyc_Yyltype* _temp3654= _temp3652.curr
+ yylsp_offset; if( _temp3654 < _temp3652.base? 1: _temp3654 >= _temp3652.last_plus_one){
_throw( Null_Exception);}* _temp3654;}).first_line,({ struct _tagged_ptr3
_temp3655= yyls; struct Cyc_Yyltype* _temp3657= _temp3655.curr + yylsp_offset;
if( _temp3657 < _temp3655.base? 1: _temp3657 >= _temp3655.last_plus_one){ _throw(
Null_Exception);}* _temp3657;}).last_line); _temp3651->f2= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp3658= yyvs; struct _xenum_struct** _temp3660= _temp3658.curr
+ yyvsp_offset; if( _temp3660 < _temp3658.base? 1: _temp3660 >= _temp3658.last_plus_one){
_throw( Null_Exception);}* _temp3660;}));( void*) _temp3651;}); _temp3647->tl=(
Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3648= yyvs; struct
_xenum_struct** _temp3650= _temp3648.curr +( yyvsp_offset - 1); if( _temp3650 <
_temp3648.base? 1: _temp3650 >= _temp3648.last_plus_one){ _throw( Null_Exception);}*
_temp3650;})))->tms; _temp3647;}); _temp3646;});( struct _xenum_struct*)
_temp3645;}); break; case 127: _LL3644: yyval=({ struct Cyc_TypeModifierList_tok_struct*
_temp3665=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct)); _temp3665->tag= Cyc_TypeModifierList_tok_tag;
_temp3665->f1=({ struct Cyc_List_List* _temp3666=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3666->hd=( void*)({ struct Cyc_Absyn_Pointer_mod_struct*
_temp3667=( struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp3667->tag= Cyc_Absyn_Pointer_mod_tag; _temp3667->f1=( void*) Cyc_yyget_Pointer_Sort_tok(({
struct _tagged_ptr2 _temp3668= yyvs; struct _xenum_struct** _temp3670= _temp3668.curr
+( yyvsp_offset - 1); if( _temp3670 < _temp3668.base? 1: _temp3670 >= _temp3668.last_plus_one){
_throw( Null_Exception);}* _temp3670;})); _temp3667->f2=( void*) Cyc_yyget_Rgn_tok(({
struct _tagged_ptr2 _temp3671= yyvs; struct _xenum_struct** _temp3673= _temp3671.curr
+ yyvsp_offset; if( _temp3673 < _temp3671.base? 1: _temp3673 >= _temp3671.last_plus_one){
_throw( Null_Exception);}* _temp3673;})); _temp3667->f3= Cyc_Absyn_empty_tqual();(
void*) _temp3667;}); _temp3666->tl= 0; _temp3666;});( struct _xenum_struct*)
_temp3665;}); break; case 128: _LL3664: yyval=({ struct Cyc_TypeModifierList_tok_struct*
_temp3675=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct)); _temp3675->tag= Cyc_TypeModifierList_tok_tag;
_temp3675->f1=({ struct Cyc_List_List* _temp3676=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3676->hd=( void*)({ struct Cyc_Absyn_Pointer_mod_struct*
_temp3677=( struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp3677->tag= Cyc_Absyn_Pointer_mod_tag; _temp3677->f1=( void*) Cyc_yyget_Pointer_Sort_tok(({
struct _tagged_ptr2 _temp3678= yyvs; struct _xenum_struct** _temp3680= _temp3678.curr
+( yyvsp_offset - 2); if( _temp3680 < _temp3678.base? 1: _temp3680 >= _temp3678.last_plus_one){
_throw( Null_Exception);}* _temp3680;})); _temp3677->f2=( void*) Cyc_yyget_Rgn_tok(({
struct _tagged_ptr2 _temp3681= yyvs; struct _xenum_struct** _temp3683= _temp3681.curr
+( yyvsp_offset - 1); if( _temp3683 < _temp3681.base? 1: _temp3683 >= _temp3681.last_plus_one){
_throw( Null_Exception);}* _temp3683;})); _temp3677->f3= Cyc_yyget_TypeQual_tok(({
struct _tagged_ptr2 _temp3684= yyvs; struct _xenum_struct** _temp3686= _temp3684.curr
+ yyvsp_offset; if( _temp3686 < _temp3684.base? 1: _temp3686 >= _temp3684.last_plus_one){
_throw( Null_Exception);}* _temp3686;}));( void*) _temp3677;}); _temp3676->tl= 0;
_temp3676;});( struct _xenum_struct*) _temp3675;}); break; case 129: _LL3674:
yyval=({ struct Cyc_TypeModifierList_tok_struct* _temp3688=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct)); _temp3688->tag= Cyc_TypeModifierList_tok_tag;
_temp3688->f1=({ struct Cyc_List_List* _temp3689=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3689->hd=( void*)({ struct Cyc_Absyn_Pointer_mod_struct*
_temp3693=( struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp3693->tag= Cyc_Absyn_Pointer_mod_tag; _temp3693->f1=( void*) Cyc_yyget_Pointer_Sort_tok(({
struct _tagged_ptr2 _temp3694= yyvs; struct _xenum_struct** _temp3696= _temp3694.curr
+( yyvsp_offset - 2); if( _temp3696 < _temp3694.base? 1: _temp3696 >= _temp3694.last_plus_one){
_throw( Null_Exception);}* _temp3696;})); _temp3693->f2=( void*) Cyc_yyget_Rgn_tok(({
struct _tagged_ptr2 _temp3697= yyvs; struct _xenum_struct** _temp3699= _temp3697.curr
+( yyvsp_offset - 1); if( _temp3699 < _temp3697.base? 1: _temp3699 >= _temp3697.last_plus_one){
_throw( Null_Exception);}* _temp3699;})); _temp3693->f3= Cyc_Absyn_empty_tqual();(
void*) _temp3693;}); _temp3689->tl= Cyc_yyget_TypeModifierList_tok(({ struct
_tagged_ptr2 _temp3690= yyvs; struct _xenum_struct** _temp3692= _temp3690.curr +
yyvsp_offset; if( _temp3692 < _temp3690.base? 1: _temp3692 >= _temp3690.last_plus_one){
_throw( Null_Exception);}* _temp3692;})); _temp3689;});( struct _xenum_struct*)
_temp3688;}); break; case 130: _LL3687: yyval=({ struct Cyc_TypeModifierList_tok_struct*
_temp3701=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct)); _temp3701->tag= Cyc_TypeModifierList_tok_tag;
_temp3701->f1=({ struct Cyc_List_List* _temp3702=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3702->hd=( void*)({ struct Cyc_Absyn_Pointer_mod_struct*
_temp3706=( struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp3706->tag= Cyc_Absyn_Pointer_mod_tag; _temp3706->f1=( void*) Cyc_yyget_Pointer_Sort_tok(({
struct _tagged_ptr2 _temp3707= yyvs; struct _xenum_struct** _temp3709= _temp3707.curr
+( yyvsp_offset - 3); if( _temp3709 < _temp3707.base? 1: _temp3709 >= _temp3707.last_plus_one){
_throw( Null_Exception);}* _temp3709;})); _temp3706->f2=( void*) Cyc_yyget_Rgn_tok(({
struct _tagged_ptr2 _temp3710= yyvs; struct _xenum_struct** _temp3712= _temp3710.curr
+( yyvsp_offset - 2); if( _temp3712 < _temp3710.base? 1: _temp3712 >= _temp3710.last_plus_one){
_throw( Null_Exception);}* _temp3712;})); _temp3706->f3= Cyc_yyget_TypeQual_tok(({
struct _tagged_ptr2 _temp3713= yyvs; struct _xenum_struct** _temp3715= _temp3713.curr
+( yyvsp_offset - 1); if( _temp3715 < _temp3713.base? 1: _temp3715 >= _temp3713.last_plus_one){
_throw( Null_Exception);}* _temp3715;}));( void*) _temp3706;}); _temp3702->tl=
Cyc_yyget_TypeModifierList_tok(({ struct _tagged_ptr2 _temp3703= yyvs; struct
_xenum_struct** _temp3705= _temp3703.curr + yyvsp_offset; if( _temp3705 <
_temp3703.base? 1: _temp3705 >= _temp3703.last_plus_one){ _throw( Null_Exception);}*
_temp3705;})); _temp3702;});( struct _xenum_struct*) _temp3701;}); break; case
131: _LL3700: yyval=({ struct Cyc_Pointer_Sort_tok_struct* _temp3717=( struct
Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp3717->tag= Cyc_Pointer_Sort_tok_tag; _temp3717->f1=( void*)({ struct Cyc_Absyn_Nullable_ps_struct*
_temp3718=( struct Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp3718->tag= Cyc_Absyn_Nullable_ps_tag; _temp3718->f1= Cyc_Absyn_signed_int_exp(
1, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3719= yyls; struct
Cyc_Yyltype* _temp3721= _temp3719.curr + yylsp_offset; if( _temp3721 < _temp3719.base?
1: _temp3721 >= _temp3719.last_plus_one){ _throw( Null_Exception);}* _temp3721;}).first_line,({
struct _tagged_ptr3 _temp3722= yyls; struct Cyc_Yyltype* _temp3724= _temp3722.curr
+ yylsp_offset; if( _temp3724 < _temp3722.base? 1: _temp3724 >= _temp3722.last_plus_one){
_throw( Null_Exception);}* _temp3724;}).last_line));( void*) _temp3718;});(
struct _xenum_struct*) _temp3717;}); break; case 132: _LL3716: yyval=({ struct
Cyc_Pointer_Sort_tok_struct* _temp3726=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp3726->tag= Cyc_Pointer_Sort_tok_tag;
_temp3726->f1=( void*)({ struct Cyc_Absyn_NonNullable_ps_struct* _temp3727=(
struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_NonNullable_ps_struct));
_temp3727->tag= Cyc_Absyn_NonNullable_ps_tag; _temp3727->f1= Cyc_Absyn_signed_int_exp(
1, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3728= yyls; struct
Cyc_Yyltype* _temp3730= _temp3728.curr + yylsp_offset; if( _temp3730 < _temp3728.base?
1: _temp3730 >= _temp3728.last_plus_one){ _throw( Null_Exception);}* _temp3730;}).first_line,({
struct _tagged_ptr3 _temp3731= yyls; struct Cyc_Yyltype* _temp3733= _temp3731.curr
+ yylsp_offset; if( _temp3733 < _temp3731.base? 1: _temp3733 >= _temp3731.last_plus_one){
_throw( Null_Exception);}* _temp3733;}).last_line));( void*) _temp3727;});(
struct _xenum_struct*) _temp3726;}); break; case 133: _LL3725: yyval=({ struct
Cyc_Pointer_Sort_tok_struct* _temp3735=( struct Cyc_Pointer_Sort_tok_struct*)
GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct)); _temp3735->tag= Cyc_Pointer_Sort_tok_tag;
_temp3735->f1=( void*)({ struct Cyc_Absyn_Nullable_ps_struct* _temp3736=( struct
Cyc_Absyn_Nullable_ps_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct));
_temp3736->tag= Cyc_Absyn_Nullable_ps_tag; _temp3736->f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp3737= yyvs; struct _xenum_struct** _temp3739= _temp3737.curr
+( yyvsp_offset - 1); if( _temp3739 < _temp3737.base? 1: _temp3739 >= _temp3737.last_plus_one){
_throw( Null_Exception);}* _temp3739;}));( void*) _temp3736;});( struct
_xenum_struct*) _temp3735;}); break; case 134: _LL3734: yyval=({ struct Cyc_Pointer_Sort_tok_struct*
_temp3741=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp3741->tag= Cyc_Pointer_Sort_tok_tag; _temp3741->f1=( void*)({ struct Cyc_Absyn_NonNullable_ps_struct*
_temp3742=( struct Cyc_Absyn_NonNullable_ps_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NonNullable_ps_struct)); _temp3742->tag= Cyc_Absyn_NonNullable_ps_tag;
_temp3742->f1= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp3743= yyvs; struct
_xenum_struct** _temp3745= _temp3743.curr +( yyvsp_offset - 1); if( _temp3745 <
_temp3743.base? 1: _temp3745 >= _temp3743.last_plus_one){ _throw( Null_Exception);}*
_temp3745;}));( void*) _temp3742;});( struct _xenum_struct*) _temp3741;});
break; case 135: _LL3740: yyval=({ struct Cyc_Pointer_Sort_tok_struct* _temp3747=(
struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));
_temp3747->tag= Cyc_Pointer_Sort_tok_tag; _temp3747->f1=( void*) Cyc_Absyn_TaggedArray_ps;(
struct _xenum_struct*) _temp3747;}); break; case 136: _LL3746: yyval=({ struct
Cyc_Rgn_tok_struct* _temp3749=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof(
struct Cyc_Rgn_tok_struct)); _temp3749->tag= Cyc_Rgn_tok_tag; _temp3749->f1=(
void*) Cyc_Absyn_HeapRgn;( struct _xenum_struct*) _temp3749;}); break; case 137:
_LL3748: yyval=({ struct Cyc_Rgn_tok_struct* _temp3751=( struct Cyc_Rgn_tok_struct*)
GC_malloc( sizeof( struct Cyc_Rgn_tok_struct)); _temp3751->tag= Cyc_Rgn_tok_tag;
_temp3751->f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp3752= yyvs; struct _xenum_struct** _temp3754= _temp3752.curr +
yyvsp_offset; if( _temp3754 < _temp3752.base? 1: _temp3754 >= _temp3752.last_plus_one){
_throw( Null_Exception);}* _temp3754;})),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)( Cyc_Absyn_RgnKind));( struct _xenum_struct*) _temp3751;});
break; case 138: _LL3750: if( Cyc_yyget_Kind_tok(({ struct _tagged_ptr2
_temp3756= yyvs; struct _xenum_struct** _temp3758= _temp3756.curr + yyvsp_offset;
if( _temp3758 < _temp3756.base? 1: _temp3758 >= _temp3756.last_plus_one){ _throw(
Null_Exception);}* _temp3758;})) != Cyc_Absyn_RgnKind){ Cyc_Parse_err(( struct
_tagged_string)({ char* _temp3759=( char*)"expecting region kind\n"; struct
_tagged_string _temp3760; _temp3760.curr= _temp3759; _temp3760.base= _temp3759;
_temp3760.last_plus_one= _temp3759 + 23; _temp3760;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3761= yyls; struct Cyc_Yyltype* _temp3763= _temp3761.curr
+ yylsp_offset; if( _temp3763 < _temp3761.base? 1: _temp3763 >= _temp3761.last_plus_one){
_throw( Null_Exception);}* _temp3763;}).first_line,({ struct _tagged_ptr3
_temp3764= yyls; struct Cyc_Yyltype* _temp3766= _temp3764.curr + yylsp_offset;
if( _temp3766 < _temp3764.base? 1: _temp3766 >= _temp3764.last_plus_one){ _throw(
Null_Exception);}* _temp3766;}).last_line));} yyval=({ struct Cyc_Rgn_tok_struct*
_temp3767=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));
_temp3767->tag= Cyc_Rgn_tok_tag; _temp3767->f1=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp3768= yyvs; struct _xenum_struct** _temp3770= _temp3768.curr
+( yyvsp_offset - 2); if( _temp3770 < _temp3768.base? 1: _temp3770 >= _temp3768.last_plus_one){
_throw( Null_Exception);}* _temp3770;})),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)( Cyc_Absyn_RgnKind));( struct _xenum_struct*) _temp3767;});
break; case 139: _LL3755: yyval=({ struct Cyc_Rgn_tok_struct* _temp3772=( struct
Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct)); _temp3772->tag=
Cyc_Rgn_tok_tag; _temp3772->f1=( void*) Cyc_Absyn_new_evar( Cyc_Absyn_RgnKind);(
struct _xenum_struct*) _temp3772;}); break; case 140: _LL3771: yyval=({ struct
_tagged_ptr2 _temp3774= yyvs; struct _xenum_struct** _temp3776= _temp3774.curr +
yyvsp_offset; if( _temp3776 < _temp3774.base? 1: _temp3776 >= _temp3774.last_plus_one){
_throw( Null_Exception);}* _temp3776;}); break; case 141: _LL3773: yyval=({
struct Cyc_TypeQual_tok_struct* _temp3778=( struct Cyc_TypeQual_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct)); _temp3778->tag= Cyc_TypeQual_tok_tag;
_temp3778->f1= Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok(({ struct
_tagged_ptr2 _temp3779= yyvs; struct _xenum_struct** _temp3781= _temp3779.curr +(
yyvsp_offset - 1); if( _temp3781 < _temp3779.base? 1: _temp3781 >= _temp3779.last_plus_one){
_throw( Null_Exception);}* _temp3781;})), Cyc_yyget_TypeQual_tok(({ struct
_tagged_ptr2 _temp3782= yyvs; struct _xenum_struct** _temp3784= _temp3782.curr +
yyvsp_offset; if( _temp3784 < _temp3782.base? 1: _temp3784 >= _temp3782.last_plus_one){
_throw( Null_Exception);}* _temp3784;})));( struct _xenum_struct*) _temp3778;});
break; case 142: _LL3777: yyval=({ struct Cyc_ParamDeclListBool_tok_struct*
_temp3786=( struct Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct
Cyc_ParamDeclListBool_tok_struct)); _temp3786->tag= Cyc_ParamDeclListBool_tok_tag;
_temp3786->f1=({ struct _tuple14* _temp3787=( struct _tuple14*) GC_malloc(
sizeof( struct _tuple14)); _temp3787->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({ struct _tagged_ptr2
_temp3788= yyvs; struct _xenum_struct** _temp3790= _temp3788.curr + yyvsp_offset;
if( _temp3790 < _temp3788.base? 1: _temp3790 >= _temp3788.last_plus_one){ _throw(
Null_Exception);}* _temp3790;}))); _temp3787->f2= 0; _temp3787->f3= 0; _temp3787;});(
struct _xenum_struct*) _temp3786;}); break; case 143: _LL3785: yyval=({ struct
Cyc_ParamDeclListBool_tok_struct* _temp3792=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct)); _temp3792->tag=
Cyc_ParamDeclListBool_tok_tag; _temp3792->f1=({ struct _tuple14* _temp3793=(
struct _tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp3793->f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp3794= yyvs; struct _xenum_struct** _temp3796= _temp3794.curr
+( yyvsp_offset - 2); if( _temp3796 < _temp3794.base? 1: _temp3796 >= _temp3794.last_plus_one){
_throw( Null_Exception);}* _temp3796;}))); _temp3793->f2= 1; _temp3793->f3= 0;
_temp3793;});( struct _xenum_struct*) _temp3792;}); break; case 144: _LL3791:
yyval=({ struct Cyc_ParamDeclListBool_tok_struct* _temp3798=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct)); _temp3798->tag=
Cyc_ParamDeclListBool_tok_tag; _temp3798->f1=({ struct _tuple14* _temp3799=(
struct _tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp3799->f1=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp3805= yyvs; struct _xenum_struct** _temp3807= _temp3805.curr
+( yyvsp_offset - 2); if( _temp3807 < _temp3805.base? 1: _temp3807 >= _temp3805.last_plus_one){
_throw( Null_Exception);}* _temp3807;}))); _temp3799->f2= 0; _temp3799->f3=(
struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt* _temp3800=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3800->v=( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp3801=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3801->tag= Cyc_Absyn_JoinEff_tag; _temp3801->f1= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3802= yyvs; struct _xenum_struct** _temp3804= _temp3802.curr
+ yyvsp_offset; if( _temp3804 < _temp3802.base? 1: _temp3804 >= _temp3802.last_plus_one){
_throw( Null_Exception);}* _temp3804;}));( void*) _temp3801;}); _temp3800;});
_temp3799;});( struct _xenum_struct*) _temp3798;}); break; case 145: _LL3797:
yyval=({ struct _tagged_ptr2 _temp3809= yyvs; struct _xenum_struct** _temp3811=
_temp3809.curr + yyvsp_offset; if( _temp3811 < _temp3809.base? 1: _temp3811 >=
_temp3809.last_plus_one){ _throw( Null_Exception);}* _temp3811;}); break; case
146: _LL3808: yyval=({ struct Cyc_TypeList_tok_struct* _temp3813=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct)); _temp3813->tag= Cyc_TypeList_tok_tag;
_temp3813->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2
_temp3814= yyvs; struct _xenum_struct** _temp3816= _temp3814.curr +(
yyvsp_offset - 2); if( _temp3816 < _temp3814.base? 1: _temp3816 >= _temp3814.last_plus_one){
_throw( Null_Exception);}* _temp3816;})), Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp3817= yyvs; struct _xenum_struct** _temp3819= _temp3817.curr +
yyvsp_offset; if( _temp3819 < _temp3817.base? 1: _temp3819 >= _temp3817.last_plus_one){
_throw( Null_Exception);}* _temp3819;})));( struct _xenum_struct*) _temp3813;});
break; case 147: _LL3812: yyval=({ struct Cyc_TypeList_tok_struct* _temp3821=(
struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3821->tag= Cyc_TypeList_tok_tag; _temp3821->f1= 0;( struct _xenum_struct*)
_temp3821;}); break; case 148: _LL3820: yyval=({ struct _tagged_ptr2 _temp3823=
yyvs; struct _xenum_struct** _temp3825= _temp3823.curr +( yyvsp_offset - 1); if(
_temp3825 < _temp3823.base? 1: _temp3825 >= _temp3823.last_plus_one){ _throw(
Null_Exception);}* _temp3825;}); break; case 149: _LL3822: yyval=({ struct Cyc_TypeList_tok_struct*
_temp3827=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3827->tag= Cyc_TypeList_tok_tag; _temp3827->f1=({ struct Cyc_List_List*
_temp3828=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3828->hd=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp3829= yyvs; struct _xenum_struct** _temp3831= _temp3829.curr +
yyvsp_offset; if( _temp3831 < _temp3829.base? 1: _temp3831 >= _temp3829.last_plus_one){
_throw( Null_Exception);}* _temp3831;})),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)( Cyc_Absyn_EffKind)); _temp3828->tl= 0; _temp3828;});(
struct _xenum_struct*) _temp3827;}); break; case 150: _LL3826: if( Cyc_yyget_Kind_tok(({
struct _tagged_ptr2 _temp3833= yyvs; struct _xenum_struct** _temp3835= _temp3833.curr
+ yyvsp_offset; if( _temp3835 < _temp3833.base? 1: _temp3835 >= _temp3833.last_plus_one){
_throw( Null_Exception);}* _temp3835;})) != Cyc_Absyn_EffKind){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp3836=( char*)"expecing effect kind (E)";
struct _tagged_string _temp3837; _temp3837.curr= _temp3836; _temp3837.base=
_temp3836; _temp3837.last_plus_one= _temp3836 + 25; _temp3837;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3838= yyls; struct Cyc_Yyltype* _temp3840= _temp3838.curr
+ yylsp_offset; if( _temp3840 < _temp3838.base? 1: _temp3840 >= _temp3838.last_plus_one){
_throw( Null_Exception);}* _temp3840;}).first_line,({ struct _tagged_ptr3
_temp3841= yyls; struct Cyc_Yyltype* _temp3843= _temp3841.curr + yylsp_offset;
if( _temp3843 < _temp3841.base? 1: _temp3843 >= _temp3841.last_plus_one){ _throw(
Null_Exception);}* _temp3843;}).last_line));} yyval=({ struct Cyc_TypeList_tok_struct*
_temp3844=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3844->tag= Cyc_TypeList_tok_tag; _temp3844->f1=({ struct Cyc_List_List*
_temp3845=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3845->hd=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp3846= yyvs; struct _xenum_struct** _temp3848= _temp3846.curr +(
yyvsp_offset - 2); if( _temp3848 < _temp3846.base? 1: _temp3848 >= _temp3846.last_plus_one){
_throw( Null_Exception);}* _temp3848;})),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)( Cyc_Absyn_EffKind)); _temp3845->tl= 0; _temp3845;});(
struct _xenum_struct*) _temp3844;}); break; case 151: _LL3832: yyval=({ struct
Cyc_TypeList_tok_struct* _temp3850=( struct Cyc_TypeList_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeList_tok_struct)); _temp3850->tag= Cyc_TypeList_tok_tag;
_temp3850->f1=({ struct Cyc_List_List* _temp3851=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3851->hd=( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp3852=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3852->tag= Cyc_Absyn_AccessEff_tag; _temp3852->f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp3853= yyvs; struct
_xenum_struct** _temp3855= _temp3853.curr + yyvsp_offset; if( _temp3855 <
_temp3853.base? 1: _temp3855 >= _temp3853.last_plus_one){ _throw( Null_Exception);}*
_temp3855;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
Cyc_Absyn_RgnKind));( void*) _temp3852;}); _temp3851->tl= 0; _temp3851;});(
struct _xenum_struct*) _temp3850;}); break; case 152: _LL3849: yyval=({ struct
Cyc_TypeList_tok_struct* _temp3857=( struct Cyc_TypeList_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeList_tok_struct)); _temp3857->tag= Cyc_TypeList_tok_tag;
_temp3857->f1=({ struct Cyc_List_List* _temp3858=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3858->hd=( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp3862=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3862->tag= Cyc_Absyn_AccessEff_tag; _temp3862->f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp3863= yyvs; struct
_xenum_struct** _temp3865= _temp3863.curr +( yyvsp_offset - 2); if( _temp3865 <
_temp3863.base? 1: _temp3865 >= _temp3863.last_plus_one){ _throw( Null_Exception);}*
_temp3865;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
Cyc_Absyn_RgnKind));( void*) _temp3862;}); _temp3858->tl= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3859= yyvs; struct _xenum_struct** _temp3861= _temp3859.curr
+ yyvsp_offset; if( _temp3861 < _temp3859.base? 1: _temp3861 >= _temp3859.last_plus_one){
_throw( Null_Exception);}* _temp3861;})); _temp3858;});( struct _xenum_struct*)
_temp3857;}); break; case 153: _LL3856: if( Cyc_yyget_Kind_tok(({ struct
_tagged_ptr2 _temp3867= yyvs; struct _xenum_struct** _temp3869= _temp3867.curr +
yyvsp_offset; if( _temp3869 < _temp3867.base? 1: _temp3869 >= _temp3867.last_plus_one){
_throw( Null_Exception);}* _temp3869;})) != Cyc_Absyn_RgnKind){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp3870=( char*)"expecting region kind (R)";
struct _tagged_string _temp3871; _temp3871.curr= _temp3870; _temp3871.base=
_temp3870; _temp3871.last_plus_one= _temp3870 + 26; _temp3871;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3872= yyls; struct Cyc_Yyltype* _temp3874= _temp3872.curr
+ yylsp_offset; if( _temp3874 < _temp3872.base? 1: _temp3874 >= _temp3872.last_plus_one){
_throw( Null_Exception);}* _temp3874;}).first_line,({ struct _tagged_ptr3
_temp3875= yyls; struct Cyc_Yyltype* _temp3877= _temp3875.curr + yylsp_offset;
if( _temp3877 < _temp3875.base? 1: _temp3877 >= _temp3875.last_plus_one){ _throw(
Null_Exception);}* _temp3877;}).last_line));} yyval=({ struct Cyc_TypeList_tok_struct*
_temp3878=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3878->tag= Cyc_TypeList_tok_tag; _temp3878->f1=({ struct Cyc_List_List*
_temp3879=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3879->hd=( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3880=( struct
Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3880->tag= Cyc_Absyn_AccessEff_tag; _temp3880->f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp3881= yyvs; struct
_xenum_struct** _temp3883= _temp3881.curr +( yyvsp_offset - 2); if( _temp3883 <
_temp3881.base? 1: _temp3883 >= _temp3881.last_plus_one){ _throw( Null_Exception);}*
_temp3883;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
Cyc_Absyn_RgnKind));( void*) _temp3880;}); _temp3879->tl= 0; _temp3879;});(
struct _xenum_struct*) _temp3878;}); break; case 154: _LL3866: if( Cyc_yyget_Kind_tok(({
struct _tagged_ptr2 _temp3885= yyvs; struct _xenum_struct** _temp3887= _temp3885.curr
+( yyvsp_offset - 2); if( _temp3887 < _temp3885.base? 1: _temp3887 >= _temp3885.last_plus_one){
_throw( Null_Exception);}* _temp3887;})) != Cyc_Absyn_RgnKind){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp3888=( char*)"expecting region kind (R)";
struct _tagged_string _temp3889; _temp3889.curr= _temp3888; _temp3889.base=
_temp3888; _temp3889.last_plus_one= _temp3888 + 26; _temp3889;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3890= yyls; struct Cyc_Yyltype* _temp3892= _temp3890.curr
+( yylsp_offset - 2); if( _temp3892 < _temp3890.base? 1: _temp3892 >= _temp3890.last_plus_one){
_throw( Null_Exception);}* _temp3892;}).first_line,({ struct _tagged_ptr3
_temp3893= yyls; struct Cyc_Yyltype* _temp3895= _temp3893.curr +( yylsp_offset -
2); if( _temp3895 < _temp3893.base? 1: _temp3895 >= _temp3893.last_plus_one){
_throw( Null_Exception);}* _temp3895;}).last_line));} yyval=({ struct Cyc_TypeList_tok_struct*
_temp3896=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));
_temp3896->tag= Cyc_TypeList_tok_tag; _temp3896->f1=({ struct Cyc_List_List*
_temp3897=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3897->hd=( void*)({ struct Cyc_Absyn_AccessEff_struct* _temp3901=( struct
Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3901->tag= Cyc_Absyn_AccessEff_tag; _temp3901->f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp3902= yyvs; struct
_xenum_struct** _temp3904= _temp3902.curr +( yyvsp_offset - 4); if( _temp3904 <
_temp3902.base? 1: _temp3904 >= _temp3902.last_plus_one){ _throw( Null_Exception);}*
_temp3904;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
Cyc_Absyn_RgnKind));( void*) _temp3901;}); _temp3897->tl= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3898= yyvs; struct _xenum_struct** _temp3900= _temp3898.curr
+ yyvsp_offset; if( _temp3900 < _temp3898.base? 1: _temp3900 >= _temp3898.last_plus_one){
_throw( Null_Exception);}* _temp3900;})); _temp3897;});( struct _xenum_struct*)
_temp3896;}); break; case 155: _LL3884: yyval=({ struct Cyc_ParamDeclList_tok_struct*
_temp3906=( struct Cyc_ParamDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct));
_temp3906->tag= Cyc_ParamDeclList_tok_tag; _temp3906->f1=({ struct Cyc_List_List*
_temp3907=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3907->hd=( void*) Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp3908=
yyvs; struct _xenum_struct** _temp3910= _temp3908.curr + yyvsp_offset; if(
_temp3910 < _temp3908.base? 1: _temp3910 >= _temp3908.last_plus_one){ _throw(
Null_Exception);}* _temp3910;})); _temp3907->tl= 0; _temp3907;});( struct
_xenum_struct*) _temp3906;}); break; case 156: _LL3905: yyval=({ struct Cyc_ParamDeclList_tok_struct*
_temp3912=( struct Cyc_ParamDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct));
_temp3912->tag= Cyc_ParamDeclList_tok_tag; _temp3912->f1=({ struct Cyc_List_List*
_temp3913=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3913->hd=( void*) Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp3917=
yyvs; struct _xenum_struct** _temp3919= _temp3917.curr + yyvsp_offset; if(
_temp3919 < _temp3917.base? 1: _temp3919 >= _temp3917.last_plus_one){ _throw(
Null_Exception);}* _temp3919;})); _temp3913->tl= Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp3914= yyvs; struct _xenum_struct** _temp3916= _temp3914.curr
+( yyvsp_offset - 2); if( _temp3916 < _temp3914.base? 1: _temp3916 >= _temp3914.last_plus_one){
_throw( Null_Exception);}* _temp3916;})); _temp3913;});( struct _xenum_struct*)
_temp3912;}); break; case 157: _LL3911: { void* t= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3921= yyvs; struct _xenum_struct** _temp3923= _temp3921.curr
+( yyvsp_offset - 1); if( _temp3923 < _temp3921.base? 1: _temp3923 >= _temp3921.last_plus_one){
_throw( Null_Exception);}* _temp3923;}))).f2, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3924= yyls; struct Cyc_Yyltype* _temp3926= _temp3924.curr
+( yylsp_offset - 1); if( _temp3926 < _temp3924.base? 1: _temp3926 >= _temp3924.last_plus_one){
_throw( Null_Exception);}* _temp3926;}).first_line,({ struct _tagged_ptr3
_temp3927= yyls; struct Cyc_Yyltype* _temp3929= _temp3927.curr +( yylsp_offset -
1); if( _temp3929 < _temp3927.base? 1: _temp3929 >= _temp3927.last_plus_one){
_throw( Null_Exception);}* _temp3929;}).last_line)); struct Cyc_List_List* atts=(*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp3930= yyvs; struct
_xenum_struct** _temp3932= _temp3930.curr +( yyvsp_offset - 1); if( _temp3932 <
_temp3930.base? 1: _temp3932 >= _temp3930.last_plus_one){ _throw( Null_Exception);}*
_temp3932;}))).f3; struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3933= yyvs; struct _xenum_struct** _temp3935= _temp3933.curr
+( yyvsp_offset - 1); if( _temp3935 < _temp3933.base? 1: _temp3935 >= _temp3933.last_plus_one){
_throw( Null_Exception);}* _temp3935;}))).f1; struct Cyc_List_List* tms=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3936= yyvs; struct _xenum_struct** _temp3938= _temp3936.curr
+ yyvsp_offset; if( _temp3938 < _temp3936.base? 1: _temp3938 >= _temp3936.last_plus_one){
_throw( Null_Exception);}* _temp3938;})))->tms; struct _tuple6 t_info= Cyc_Parse_apply_tms(
tq, t, atts, tms); if( t_info.f3 != 0){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp3939=( char*)"parameter with bad type params"; struct _tagged_string
_temp3940; _temp3940.curr= _temp3939; _temp3940.base= _temp3939; _temp3940.last_plus_one=
_temp3939 + 31; _temp3940;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp3941= yyls; struct Cyc_Yyltype* _temp3943= _temp3941.curr + yylsp_offset;
if( _temp3943 < _temp3941.base? 1: _temp3943 >= _temp3941.last_plus_one){ _throw(
Null_Exception);}* _temp3943;}).first_line,({ struct _tagged_ptr3 _temp3944=
yyls; struct Cyc_Yyltype* _temp3946= _temp3944.curr + yylsp_offset; if(
_temp3946 < _temp3944.base? 1: _temp3946 >= _temp3944.last_plus_one){ _throw(
Null_Exception);}* _temp3946;}).last_line));}{ struct _tuple1* q=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3947= yyvs; struct _xenum_struct** _temp3949= _temp3947.curr
+ yyvsp_offset; if( _temp3949 < _temp3947.base? 1: _temp3949 >= _temp3947.last_plus_one){
_throw( Null_Exception);}* _temp3949;})))->id;{ void* _temp3950=(* q).f1; struct
Cyc_List_List* _temp3960; struct Cyc_List_List* _temp3962; _LL3952: if(
_temp3950 == Cyc_Absyn_Loc_n){ goto _LL3953;} else{ goto _LL3954;} _LL3954: if((
unsigned int) _temp3950 > 1u?(( struct _enum_struct*) _temp3950)->tag == Cyc_Absyn_Rel_n_tag:
0){ _LL3961: _temp3960=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp3950)->f1; if( _temp3960 == 0){ goto _LL3955;} else{ goto _LL3956;}} else{
goto _LL3956;} _LL3956: if(( unsigned int) _temp3950 > 1u?(( struct _enum_struct*)
_temp3950)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL3963: _temp3962=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp3950)->f1; if( _temp3962 == 0){ goto
_LL3957;} else{ goto _LL3958;}} else{ goto _LL3958;} _LL3958: goto _LL3959;
_LL3953: goto _LL3951; _LL3955: goto _LL3951; _LL3957: goto _LL3951; _LL3959:
Cyc_Parse_err(( struct _tagged_string)({ char* _temp3964=( char*)"parameter cannot be qualified with a module name";
struct _tagged_string _temp3965; _temp3965.curr= _temp3964; _temp3965.base=
_temp3964; _temp3965.last_plus_one= _temp3964 + 49; _temp3965;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3966= yyls; struct Cyc_Yyltype* _temp3968= _temp3966.curr
+( yylsp_offset - 1); if( _temp3968 < _temp3966.base? 1: _temp3968 >= _temp3966.last_plus_one){
_throw( Null_Exception);}* _temp3968;}).first_line,({ struct _tagged_ptr3
_temp3969= yyls; struct Cyc_Yyltype* _temp3971= _temp3969.curr +( yylsp_offset -
1); if( _temp3971 < _temp3969.base? 1: _temp3971 >= _temp3969.last_plus_one){
_throw( Null_Exception);}* _temp3971;}).last_line)); goto _LL3951; _LL3951:;}{
struct Cyc_Core_Opt* idopt=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt*
_temp3972=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3972->v=( void*)(* q).f2; _temp3972;}); if( t_info.f4 != 0){ Cyc_Parse_warn((
struct _tagged_string)({ char* _temp3973=( char*)"extra attributes on parameter, ignoring";
struct _tagged_string _temp3974; _temp3974.curr= _temp3973; _temp3974.base=
_temp3973; _temp3974.last_plus_one= _temp3973 + 40; _temp3974;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3975= yyls; struct Cyc_Yyltype* _temp3977= _temp3975.curr
+( yylsp_offset - 1); if( _temp3977 < _temp3975.base? 1: _temp3977 >= _temp3975.last_plus_one){
_throw( Null_Exception);}* _temp3977;}).first_line,({ struct _tagged_ptr3
_temp3978= yyls; struct Cyc_Yyltype* _temp3980= _temp3978.curr + yylsp_offset;
if( _temp3980 < _temp3978.base? 1: _temp3980 >= _temp3978.last_plus_one){ _throw(
Null_Exception);}* _temp3980;}).last_line));} if( t_info.f3 != 0){ Cyc_Parse_warn((
struct _tagged_string)({ char* _temp3981=( char*)"extra type variables on parameter, ignoring";
struct _tagged_string _temp3982; _temp3982.curr= _temp3981; _temp3982.base=
_temp3981; _temp3982.last_plus_one= _temp3981 + 44; _temp3982;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3983= yyls; struct Cyc_Yyltype* _temp3985= _temp3983.curr
+( yylsp_offset - 1); if( _temp3985 < _temp3983.base? 1: _temp3985 >= _temp3983.last_plus_one){
_throw( Null_Exception);}* _temp3985;}).first_line,({ struct _tagged_ptr3
_temp3986= yyls; struct Cyc_Yyltype* _temp3988= _temp3986.curr + yylsp_offset;
if( _temp3988 < _temp3986.base? 1: _temp3988 >= _temp3986.last_plus_one){ _throw(
Null_Exception);}* _temp3988;}).last_line));} yyval=({ struct Cyc_ParamDecl_tok_struct*
_temp3989=( struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp3989->tag= Cyc_ParamDecl_tok_tag; _temp3989->f1=({ struct _tuple2*
_temp3990=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2)); _temp3990->f1=
idopt; _temp3990->f2= t_info.f1; _temp3990->f3= t_info.f2; _temp3990;});( struct
_xenum_struct*) _temp3989;}); break;}}} case 158: _LL3920: { void* t= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp3992= yyvs; struct
_xenum_struct** _temp3994= _temp3992.curr + yyvsp_offset; if( _temp3994 <
_temp3992.base? 1: _temp3994 >= _temp3992.last_plus_one){ _throw( Null_Exception);}*
_temp3994;}))).f2, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3995=
yyls; struct Cyc_Yyltype* _temp3997= _temp3995.curr + yylsp_offset; if(
_temp3997 < _temp3995.base? 1: _temp3997 >= _temp3995.last_plus_one){ _throw(
Null_Exception);}* _temp3997;}).first_line,({ struct _tagged_ptr3 _temp3998=
yyls; struct Cyc_Yyltype* _temp4000= _temp3998.curr + yylsp_offset; if(
_temp4000 < _temp3998.base? 1: _temp4000 >= _temp3998.last_plus_one){ _throw(
Null_Exception);}* _temp4000;}).last_line)); struct Cyc_List_List* atts=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4001= yyvs; struct _xenum_struct** _temp4003= _temp4001.curr
+ yyvsp_offset; if( _temp4003 < _temp4001.base? 1: _temp4003 >= _temp4001.last_plus_one){
_throw( Null_Exception);}* _temp4003;}))).f3; struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4004= yyvs; struct _xenum_struct** _temp4006= _temp4004.curr
+ yyvsp_offset; if( _temp4006 < _temp4004.base? 1: _temp4006 >= _temp4004.last_plus_one){
_throw( Null_Exception);}* _temp4006;}))).f1; if( atts != 0){ Cyc_Parse_warn((
struct _tagged_string)({ char* _temp4007=( char*)"bad attributes on parameter, ignoring";
struct _tagged_string _temp4008; _temp4008.curr= _temp4007; _temp4008.base=
_temp4007; _temp4008.last_plus_one= _temp4007 + 38; _temp4008;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4009= yyls; struct Cyc_Yyltype* _temp4011= _temp4009.curr
+ yylsp_offset; if( _temp4011 < _temp4009.base? 1: _temp4011 >= _temp4009.last_plus_one){
_throw( Null_Exception);}* _temp4011;}).first_line,({ struct _tagged_ptr3
_temp4012= yyls; struct Cyc_Yyltype* _temp4014= _temp4012.curr + yylsp_offset;
if( _temp4014 < _temp4012.base? 1: _temp4014 >= _temp4012.last_plus_one){ _throw(
Null_Exception);}* _temp4014;}).last_line));} yyval=({ struct Cyc_ParamDecl_tok_struct*
_temp4015=( struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp4015->tag= Cyc_ParamDecl_tok_tag; _temp4015->f1=({ struct _tuple2*
_temp4016=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2)); _temp4016->f1=
0; _temp4016->f2= tq; _temp4016->f3= t; _temp4016;});( struct _xenum_struct*)
_temp4015;}); break;} case 159: _LL3991: { void* t= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp4018= yyvs; struct
_xenum_struct** _temp4020= _temp4018.curr +( yyvsp_offset - 1); if( _temp4020 <
_temp4018.base? 1: _temp4020 >= _temp4018.last_plus_one){ _throw( Null_Exception);}*
_temp4020;}))).f2, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4021=
yyls; struct Cyc_Yyltype* _temp4023= _temp4021.curr +( yylsp_offset - 1); if(
_temp4023 < _temp4021.base? 1: _temp4023 >= _temp4021.last_plus_one){ _throw(
Null_Exception);}* _temp4023;}).first_line,({ struct _tagged_ptr3 _temp4024=
yyls; struct Cyc_Yyltype* _temp4026= _temp4024.curr +( yylsp_offset - 1); if(
_temp4026 < _temp4024.base? 1: _temp4026 >= _temp4024.last_plus_one){ _throw(
Null_Exception);}* _temp4026;}).last_line)); struct Cyc_List_List* atts=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4027= yyvs; struct _xenum_struct** _temp4029= _temp4027.curr
+( yyvsp_offset - 1); if( _temp4029 < _temp4027.base? 1: _temp4029 >= _temp4027.last_plus_one){
_throw( Null_Exception);}* _temp4029;}))).f3; struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4030= yyvs; struct _xenum_struct** _temp4032= _temp4030.curr
+( yyvsp_offset - 1); if( _temp4032 < _temp4030.base? 1: _temp4032 >= _temp4030.last_plus_one){
_throw( Null_Exception);}* _temp4032;}))).f1; struct Cyc_List_List* tms=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp4033= yyvs; struct _xenum_struct** _temp4035= _temp4033.curr
+ yyvsp_offset; if( _temp4035 < _temp4033.base? 1: _temp4035 >= _temp4033.last_plus_one){
_throw( Null_Exception);}* _temp4035;})))->tms; struct _tuple6 t_info= Cyc_Parse_apply_tms(
tq, t, atts, tms); if( t_info.f3 != 0){ Cyc_Parse_warn(( struct _tagged_string)({
char* _temp4036=( char*)"bad type parameters on formal argument, ignoring";
struct _tagged_string _temp4037; _temp4037.curr= _temp4036; _temp4037.base=
_temp4036; _temp4037.last_plus_one= _temp4036 + 49; _temp4037;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4038= yyls; struct Cyc_Yyltype* _temp4040= _temp4038.curr
+( yylsp_offset - 1); if( _temp4040 < _temp4038.base? 1: _temp4040 >= _temp4038.last_plus_one){
_throw( Null_Exception);}* _temp4040;}).first_line,({ struct _tagged_ptr3
_temp4041= yyls; struct Cyc_Yyltype* _temp4043= _temp4041.curr + yylsp_offset;
if( _temp4043 < _temp4041.base? 1: _temp4043 >= _temp4041.last_plus_one){ _throw(
Null_Exception);}* _temp4043;}).last_line));} if( t_info.f4 != 0){ Cyc_Parse_warn((
struct _tagged_string)({ char* _temp4044=( char*)"bad attributes on parameter, ignoring";
struct _tagged_string _temp4045; _temp4045.curr= _temp4044; _temp4045.base=
_temp4044; _temp4045.last_plus_one= _temp4044 + 38; _temp4045;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4046= yyls; struct Cyc_Yyltype* _temp4048= _temp4046.curr
+( yylsp_offset - 1); if( _temp4048 < _temp4046.base? 1: _temp4048 >= _temp4046.last_plus_one){
_throw( Null_Exception);}* _temp4048;}).first_line,({ struct _tagged_ptr3
_temp4049= yyls; struct Cyc_Yyltype* _temp4051= _temp4049.curr + yylsp_offset;
if( _temp4051 < _temp4049.base? 1: _temp4051 >= _temp4049.last_plus_one){ _throw(
Null_Exception);}* _temp4051;}).last_line));} yyval=({ struct Cyc_ParamDecl_tok_struct*
_temp4052=( struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp4052->tag= Cyc_ParamDecl_tok_tag; _temp4052->f1=({ struct _tuple2*
_temp4053=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2)); _temp4053->f1=
0; _temp4053->f2= t_info.f1; _temp4053->f3= t_info.f2; _temp4053;});( struct
_xenum_struct*) _temp4052;}); break;} case 160: _LL4017: yyval=({ struct Cyc_IdList_tok_struct*
_temp4055=( struct Cyc_IdList_tok_struct*) GC_malloc( sizeof( struct Cyc_IdList_tok_struct));
_temp4055->tag= Cyc_IdList_tok_tag; _temp4055->f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_IdList_tok(({ struct
_tagged_ptr2 _temp4056= yyvs; struct _xenum_struct** _temp4058= _temp4056.curr +
yyvsp_offset; if( _temp4058 < _temp4056.base? 1: _temp4058 >= _temp4056.last_plus_one){
_throw( Null_Exception);}* _temp4058;})));( struct _xenum_struct*) _temp4055;});
break; case 161: _LL4054: yyval=({ struct Cyc_IdList_tok_struct* _temp4060=(
struct Cyc_IdList_tok_struct*) GC_malloc( sizeof( struct Cyc_IdList_tok_struct));
_temp4060->tag= Cyc_IdList_tok_tag; _temp4060->f1=({ struct Cyc_List_List*
_temp4061=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4061->hd=( void*)({ struct _tagged_string* _temp4062=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp4062[ 0]=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp4063= yyvs; struct
_xenum_struct** _temp4065= _temp4063.curr + yyvsp_offset; if( _temp4065 <
_temp4063.base? 1: _temp4065 >= _temp4063.last_plus_one){ _throw( Null_Exception);}*
_temp4065;})); _temp4062;}); _temp4061->tl= 0; _temp4061;});( struct
_xenum_struct*) _temp4060;}); break; case 162: _LL4059: yyval=({ struct Cyc_IdList_tok_struct*
_temp4067=( struct Cyc_IdList_tok_struct*) GC_malloc( sizeof( struct Cyc_IdList_tok_struct));
_temp4067->tag= Cyc_IdList_tok_tag; _temp4067->f1=({ struct Cyc_List_List*
_temp4068=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4068->hd=( void*)({ struct _tagged_string* _temp4072=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp4072[ 0]=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp4073= yyvs; struct
_xenum_struct** _temp4075= _temp4073.curr + yyvsp_offset; if( _temp4075 <
_temp4073.base? 1: _temp4075 >= _temp4073.last_plus_one){ _throw( Null_Exception);}*
_temp4075;})); _temp4072;}); _temp4068->tl= Cyc_yyget_IdList_tok(({ struct
_tagged_ptr2 _temp4069= yyvs; struct _xenum_struct** _temp4071= _temp4069.curr +(
yyvsp_offset - 2); if( _temp4071 < _temp4069.base? 1: _temp4071 >= _temp4069.last_plus_one){
_throw( Null_Exception);}* _temp4071;})); _temp4068;});( struct _xenum_struct*)
_temp4067;}); break; case 163: _LL4066: yyval=({ struct _tagged_ptr2 _temp4077=
yyvs; struct _xenum_struct** _temp4079= _temp4077.curr + yyvsp_offset; if(
_temp4079 < _temp4077.base? 1: _temp4079 >= _temp4077.last_plus_one){ _throw(
Null_Exception);}* _temp4079;}); break; case 164: _LL4076: yyval=({ struct Cyc_Exp_tok_struct*
_temp4081=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp4081->tag= Cyc_Exp_tok_tag; _temp4081->f1= Cyc_Absyn_new_exp(({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp4082=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp4082->tag= Cyc_Absyn_UnresolvedMem_e_tag;
_temp4082->f1= 0; _temp4082->f2= 0;( void*) _temp4082;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4083= yyls; struct Cyc_Yyltype* _temp4085= _temp4083.curr
+( yylsp_offset - 1); if( _temp4085 < _temp4083.base? 1: _temp4085 >= _temp4083.last_plus_one){
_throw( Null_Exception);}* _temp4085;}).first_line,({ struct _tagged_ptr3
_temp4086= yyls; struct Cyc_Yyltype* _temp4088= _temp4086.curr + yylsp_offset;
if( _temp4088 < _temp4086.base? 1: _temp4088 >= _temp4086.last_plus_one){ _throw(
Null_Exception);}* _temp4088;}).last_line));( struct _xenum_struct*) _temp4081;});
break; case 165: _LL4080: yyval=({ struct Cyc_Exp_tok_struct* _temp4090=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4090->tag=
Cyc_Exp_tok_tag; _temp4090->f1= Cyc_Absyn_new_exp(({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp4091=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp4091->tag= Cyc_Absyn_UnresolvedMem_e_tag;
_temp4091->f1= 0; _temp4091->f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({ struct _tagged_ptr2
_temp4092= yyvs; struct _xenum_struct** _temp4094= _temp4092.curr +(
yyvsp_offset - 1); if( _temp4094 < _temp4092.base? 1: _temp4094 >= _temp4092.last_plus_one){
_throw( Null_Exception);}* _temp4094;})));( void*) _temp4091;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4095= yyls; struct Cyc_Yyltype* _temp4097= _temp4095.curr
+( yylsp_offset - 2); if( _temp4097 < _temp4095.base? 1: _temp4097 >= _temp4095.last_plus_one){
_throw( Null_Exception);}* _temp4097;}).first_line,({ struct _tagged_ptr3
_temp4098= yyls; struct Cyc_Yyltype* _temp4100= _temp4098.curr + yylsp_offset;
if( _temp4100 < _temp4098.base? 1: _temp4100 >= _temp4098.last_plus_one){ _throw(
Null_Exception);}* _temp4100;}).last_line));( struct _xenum_struct*) _temp4090;});
break; case 166: _LL4089: yyval=({ struct Cyc_Exp_tok_struct* _temp4102=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp4102->tag=
Cyc_Exp_tok_tag; _temp4102->f1= Cyc_Absyn_new_exp(({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp4103=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp4103->tag= Cyc_Absyn_UnresolvedMem_e_tag;
_temp4103->f1= 0; _temp4103->f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({ struct _tagged_ptr2
_temp4104= yyvs; struct _xenum_struct** _temp4106= _temp4104.curr +(
yyvsp_offset - 2); if( _temp4106 < _temp4104.base? 1: _temp4106 >= _temp4104.last_plus_one){
_throw( Null_Exception);}* _temp4106;})));( void*) _temp4103;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4107= yyls; struct Cyc_Yyltype* _temp4109= _temp4107.curr
+( yylsp_offset - 3); if( _temp4109 < _temp4107.base? 1: _temp4109 >= _temp4107.last_plus_one){
_throw( Null_Exception);}* _temp4109;}).first_line,({ struct _tagged_ptr3
_temp4110= yyls; struct Cyc_Yyltype* _temp4112= _temp4110.curr + yylsp_offset;
if( _temp4112 < _temp4110.base? 1: _temp4112 >= _temp4110.last_plus_one){ _throw(
Null_Exception);}* _temp4112;}).last_line));( struct _xenum_struct*) _temp4102;});
break; case 167: _LL4101: yyval=({ struct Cyc_InitializerList_tok_struct*
_temp4114=( struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct
Cyc_InitializerList_tok_struct)); _temp4114->tag= Cyc_InitializerList_tok_tag;
_temp4114->f1=({ struct Cyc_List_List* _temp4115=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4115->hd=( void*)({ struct
_tuple16* _temp4116=( struct _tuple16*) GC_malloc( sizeof( struct _tuple16));
_temp4116->f1= 0; _temp4116->f2= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp4117= yyvs; struct _xenum_struct** _temp4119= _temp4117.curr + yyvsp_offset;
if( _temp4119 < _temp4117.base? 1: _temp4119 >= _temp4117.last_plus_one){ _throw(
Null_Exception);}* _temp4119;})); _temp4116;}); _temp4115->tl= 0; _temp4115;});(
struct _xenum_struct*) _temp4114;}); break; case 168: _LL4113: yyval=({ struct
Cyc_InitializerList_tok_struct* _temp4121=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp4121->tag= Cyc_InitializerList_tok_tag;
_temp4121->f1=({ struct Cyc_List_List* _temp4122=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4122->hd=( void*)({ struct
_tuple16* _temp4123=( struct _tuple16*) GC_malloc( sizeof( struct _tuple16));
_temp4123->f1= Cyc_yyget_DesignatorList_tok(({ struct _tagged_ptr2 _temp4127=
yyvs; struct _xenum_struct** _temp4129= _temp4127.curr +( yyvsp_offset - 1); if(
_temp4129 < _temp4127.base? 1: _temp4129 >= _temp4127.last_plus_one){ _throw(
Null_Exception);}* _temp4129;})); _temp4123->f2= Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4124= yyvs; struct _xenum_struct** _temp4126= _temp4124.curr +
yyvsp_offset; if( _temp4126 < _temp4124.base? 1: _temp4126 >= _temp4124.last_plus_one){
_throw( Null_Exception);}* _temp4126;})); _temp4123;}); _temp4122->tl= 0;
_temp4122;});( struct _xenum_struct*) _temp4121;}); break; case 169: _LL4120:
yyval=({ struct Cyc_InitializerList_tok_struct* _temp4131=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct)); _temp4131->tag= Cyc_InitializerList_tok_tag;
_temp4131->f1=({ struct Cyc_List_List* _temp4132=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4132->hd=( void*)({ struct
_tuple16* _temp4136=( struct _tuple16*) GC_malloc( sizeof( struct _tuple16));
_temp4136->f1= 0; _temp4136->f2= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp4137= yyvs; struct _xenum_struct** _temp4139= _temp4137.curr + yyvsp_offset;
if( _temp4139 < _temp4137.base? 1: _temp4139 >= _temp4137.last_plus_one){ _throw(
Null_Exception);}* _temp4139;})); _temp4136;}); _temp4132->tl= Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp4133= yyvs; struct _xenum_struct** _temp4135= _temp4133.curr
+( yyvsp_offset - 2); if( _temp4135 < _temp4133.base? 1: _temp4135 >= _temp4133.last_plus_one){
_throw( Null_Exception);}* _temp4135;})); _temp4132;});( struct _xenum_struct*)
_temp4131;}); break; case 170: _LL4130: yyval=({ struct Cyc_InitializerList_tok_struct*
_temp4141=( struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct
Cyc_InitializerList_tok_struct)); _temp4141->tag= Cyc_InitializerList_tok_tag;
_temp4141->f1=({ struct Cyc_List_List* _temp4142=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4142->hd=( void*)({ struct
_tuple16* _temp4146=( struct _tuple16*) GC_malloc( sizeof( struct _tuple16));
_temp4146->f1= Cyc_yyget_DesignatorList_tok(({ struct _tagged_ptr2 _temp4150=
yyvs; struct _xenum_struct** _temp4152= _temp4150.curr +( yyvsp_offset - 1); if(
_temp4152 < _temp4150.base? 1: _temp4152 >= _temp4150.last_plus_one){ _throw(
Null_Exception);}* _temp4152;})); _temp4146->f2= Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4147= yyvs; struct _xenum_struct** _temp4149= _temp4147.curr +
yyvsp_offset; if( _temp4149 < _temp4147.base? 1: _temp4149 >= _temp4147.last_plus_one){
_throw( Null_Exception);}* _temp4149;})); _temp4146;}); _temp4142->tl= Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp4143= yyvs; struct _xenum_struct** _temp4145= _temp4143.curr
+( yyvsp_offset - 3); if( _temp4145 < _temp4143.base? 1: _temp4145 >= _temp4143.last_plus_one){
_throw( Null_Exception);}* _temp4145;})); _temp4142;});( struct _xenum_struct*)
_temp4141;}); break; case 171: _LL4140: yyval=({ struct Cyc_DesignatorList_tok_struct*
_temp4154=( struct Cyc_DesignatorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp4154->tag= Cyc_DesignatorList_tok_tag; _temp4154->f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_DesignatorList_tok(({
struct _tagged_ptr2 _temp4155= yyvs; struct _xenum_struct** _temp4157= _temp4155.curr
+( yyvsp_offset - 1); if( _temp4157 < _temp4155.base? 1: _temp4157 >= _temp4155.last_plus_one){
_throw( Null_Exception);}* _temp4157;})));( struct _xenum_struct*) _temp4154;});
break; case 172: _LL4153: yyval=({ struct Cyc_DesignatorList_tok_struct*
_temp4159=( struct Cyc_DesignatorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp4159->tag= Cyc_DesignatorList_tok_tag; _temp4159->f1=({ struct Cyc_List_List*
_temp4160=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4160->hd=( void*) Cyc_yyget_Designator_tok(({ struct _tagged_ptr2 _temp4161=
yyvs; struct _xenum_struct** _temp4163= _temp4161.curr + yyvsp_offset; if(
_temp4163 < _temp4161.base? 1: _temp4163 >= _temp4161.last_plus_one){ _throw(
Null_Exception);}* _temp4163;})); _temp4160->tl= 0; _temp4160;});( struct
_xenum_struct*) _temp4159;}); break; case 173: _LL4158: yyval=({ struct Cyc_DesignatorList_tok_struct*
_temp4165=( struct Cyc_DesignatorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));
_temp4165->tag= Cyc_DesignatorList_tok_tag; _temp4165->f1=({ struct Cyc_List_List*
_temp4166=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4166->hd=( void*) Cyc_yyget_Designator_tok(({ struct _tagged_ptr2 _temp4170=
yyvs; struct _xenum_struct** _temp4172= _temp4170.curr + yyvsp_offset; if(
_temp4172 < _temp4170.base? 1: _temp4172 >= _temp4170.last_plus_one){ _throw(
Null_Exception);}* _temp4172;})); _temp4166->tl= Cyc_yyget_DesignatorList_tok(({
struct _tagged_ptr2 _temp4167= yyvs; struct _xenum_struct** _temp4169= _temp4167.curr
+( yyvsp_offset - 1); if( _temp4169 < _temp4167.base? 1: _temp4169 >= _temp4167.last_plus_one){
_throw( Null_Exception);}* _temp4169;})); _temp4166;});( struct _xenum_struct*)
_temp4165;}); break; case 174: _LL4164: yyval=({ struct Cyc_Designator_tok_struct*
_temp4174=( struct Cyc_Designator_tok_struct*) GC_malloc( sizeof( struct Cyc_Designator_tok_struct));
_temp4174->tag= Cyc_Designator_tok_tag; _temp4174->f1=( void*)({ struct Cyc_Absyn_ArrayElement_struct*
_temp4175=( struct Cyc_Absyn_ArrayElement_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayElement_struct));
_temp4175->tag= Cyc_Absyn_ArrayElement_tag; _temp4175->f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4176= yyvs; struct _xenum_struct** _temp4178= _temp4176.curr
+( yyvsp_offset - 1); if( _temp4178 < _temp4176.base? 1: _temp4178 >= _temp4176.last_plus_one){
_throw( Null_Exception);}* _temp4178;}));( void*) _temp4175;});( struct
_xenum_struct*) _temp4174;}); break; case 175: _LL4173: yyval=({ struct Cyc_Designator_tok_struct*
_temp4180=( struct Cyc_Designator_tok_struct*) GC_malloc( sizeof( struct Cyc_Designator_tok_struct));
_temp4180->tag= Cyc_Designator_tok_tag; _temp4180->f1=( void*)({ struct Cyc_Absyn_FieldName_struct*
_temp4181=( struct Cyc_Absyn_FieldName_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct));
_temp4181->tag= Cyc_Absyn_FieldName_tag; _temp4181->f1=({ struct _tagged_string*
_temp4182=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp4182[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp4183= yyvs;
struct _xenum_struct** _temp4185= _temp4183.curr + yyvsp_offset; if( _temp4185 <
_temp4183.base? 1: _temp4185 >= _temp4183.last_plus_one){ _throw( Null_Exception);}*
_temp4185;})); _temp4182;});( void*) _temp4181;});( struct _xenum_struct*)
_temp4180;}); break; case 176: _LL4179: { void* t= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4187= yyvs; struct _xenum_struct** _temp4189= _temp4187.curr
+ yyvsp_offset; if( _temp4189 < _temp4187.base? 1: _temp4189 >= _temp4187.last_plus_one){
_throw( Null_Exception);}* _temp4189;}))).f2, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4190= yyls; struct Cyc_Yyltype* _temp4192= _temp4190.curr
+ yylsp_offset; if( _temp4192 < _temp4190.base? 1: _temp4192 >= _temp4190.last_plus_one){
_throw( Null_Exception);}* _temp4192;}).first_line,({ struct _tagged_ptr3
_temp4193= yyls; struct Cyc_Yyltype* _temp4195= _temp4193.curr + yylsp_offset;
if( _temp4195 < _temp4193.base? 1: _temp4195 >= _temp4193.last_plus_one){ _throw(
Null_Exception);}* _temp4195;}).last_line)); struct Cyc_List_List* atts=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4196= yyvs; struct _xenum_struct** _temp4198= _temp4196.curr
+ yyvsp_offset; if( _temp4198 < _temp4196.base? 1: _temp4198 >= _temp4196.last_plus_one){
_throw( Null_Exception);}* _temp4198;}))).f3; if( atts != 0){ Cyc_Parse_warn((
struct _tagged_string)({ char* _temp4199=( char*)"ignoring attributes in type";
struct _tagged_string _temp4200; _temp4200.curr= _temp4199; _temp4200.base=
_temp4199; _temp4200.last_plus_one= _temp4199 + 28; _temp4200;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4201= yyls; struct Cyc_Yyltype* _temp4203= _temp4201.curr
+ yylsp_offset; if( _temp4203 < _temp4201.base? 1: _temp4203 >= _temp4201.last_plus_one){
_throw( Null_Exception);}* _temp4203;}).first_line,({ struct _tagged_ptr3
_temp4204= yyls; struct Cyc_Yyltype* _temp4206= _temp4204.curr + yylsp_offset;
if( _temp4206 < _temp4204.base? 1: _temp4206 >= _temp4204.last_plus_one){ _throw(
Null_Exception);}* _temp4206;}).last_line));}{ struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4207= yyvs; struct _xenum_struct** _temp4209= _temp4207.curr
+ yyvsp_offset; if( _temp4209 < _temp4207.base? 1: _temp4209 >= _temp4207.last_plus_one){
_throw( Null_Exception);}* _temp4209;}))).f1; yyval=({ struct Cyc_ParamDecl_tok_struct*
_temp4210=( struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp4210->tag= Cyc_ParamDecl_tok_tag; _temp4210->f1=({ struct _tuple2*
_temp4211=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2) * 1); _temp4211[
0]=({ struct _tuple2 _temp4212; _temp4212.f1= 0; _temp4212.f2= tq; _temp4212.f3=
t; _temp4212;}); _temp4211;});( struct _xenum_struct*) _temp4210;}); break;}}
case 177: _LL4186: { void* t= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4214= yyvs; struct _xenum_struct** _temp4216= _temp4214.curr
+( yyvsp_offset - 1); if( _temp4216 < _temp4214.base? 1: _temp4216 >= _temp4214.last_plus_one){
_throw( Null_Exception);}* _temp4216;}))).f2, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4217= yyls; struct Cyc_Yyltype* _temp4219= _temp4217.curr
+( yylsp_offset - 1); if( _temp4219 < _temp4217.base? 1: _temp4219 >= _temp4217.last_plus_one){
_throw( Null_Exception);}* _temp4219;}).first_line,({ struct _tagged_ptr3
_temp4220= yyls; struct Cyc_Yyltype* _temp4222= _temp4220.curr +( yylsp_offset -
1); if( _temp4222 < _temp4220.base? 1: _temp4222 >= _temp4220.last_plus_one){
_throw( Null_Exception);}* _temp4222;}).last_line)); struct Cyc_List_List* atts=(*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp4223= yyvs; struct
_xenum_struct** _temp4225= _temp4223.curr +( yyvsp_offset - 1); if( _temp4225 <
_temp4223.base? 1: _temp4225 >= _temp4223.last_plus_one){ _throw( Null_Exception);}*
_temp4225;}))).f3; struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4226= yyvs; struct _xenum_struct** _temp4228= _temp4226.curr
+( yyvsp_offset - 1); if( _temp4228 < _temp4226.base? 1: _temp4228 >= _temp4226.last_plus_one){
_throw( Null_Exception);}* _temp4228;}))).f1; struct Cyc_List_List* tms=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp4229= yyvs; struct _xenum_struct** _temp4231= _temp4229.curr
+ yyvsp_offset; if( _temp4231 < _temp4229.base? 1: _temp4231 >= _temp4229.last_plus_one){
_throw( Null_Exception);}* _temp4231;})))->tms; struct _tuple6 t_info= Cyc_Parse_apply_tms(
tq, t, atts, tms); if( t_info.f3 != 0){ Cyc_Parse_warn(( struct _tagged_string)({
char* _temp4232=( char*)"bad type params, ignoring"; struct _tagged_string
_temp4233; _temp4233.curr= _temp4232; _temp4233.base= _temp4232; _temp4233.last_plus_one=
_temp4232 + 26; _temp4233;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4234= yyls; struct Cyc_Yyltype* _temp4236= _temp4234.curr + yylsp_offset;
if( _temp4236 < _temp4234.base? 1: _temp4236 >= _temp4234.last_plus_one){ _throw(
Null_Exception);}* _temp4236;}).first_line,({ struct _tagged_ptr3 _temp4237=
yyls; struct Cyc_Yyltype* _temp4239= _temp4237.curr + yylsp_offset; if(
_temp4239 < _temp4237.base? 1: _temp4239 >= _temp4237.last_plus_one){ _throw(
Null_Exception);}* _temp4239;}).last_line));} if( t_info.f4 != 0){ Cyc_Parse_warn((
struct _tagged_string)({ char* _temp4240=( char*)"bad specifiers, ignoring";
struct _tagged_string _temp4241; _temp4241.curr= _temp4240; _temp4241.base=
_temp4240; _temp4241.last_plus_one= _temp4240 + 25; _temp4241;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4242= yyls; struct Cyc_Yyltype* _temp4244= _temp4242.curr
+ yylsp_offset; if( _temp4244 < _temp4242.base? 1: _temp4244 >= _temp4242.last_plus_one){
_throw( Null_Exception);}* _temp4244;}).first_line,({ struct _tagged_ptr3
_temp4245= yyls; struct Cyc_Yyltype* _temp4247= _temp4245.curr + yylsp_offset;
if( _temp4247 < _temp4245.base? 1: _temp4247 >= _temp4245.last_plus_one){ _throw(
Null_Exception);}* _temp4247;}).last_line));} yyval=({ struct Cyc_ParamDecl_tok_struct*
_temp4248=( struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));
_temp4248->tag= Cyc_ParamDecl_tok_tag; _temp4248->f1=({ struct _tuple2*
_temp4249=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2)); _temp4249->f1=
0; _temp4249->f2= t_info.f1; _temp4249->f3= t_info.f2; _temp4249;});( struct
_xenum_struct*) _temp4248;}); break;} case 178: _LL4213: yyval=({ struct Cyc_Type_tok_struct*
_temp4251=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp4251->tag= Cyc_Type_tok_tag; _temp4251->f1=( void*)(* Cyc_yyget_ParamDecl_tok(({
struct _tagged_ptr2 _temp4252= yyvs; struct _xenum_struct** _temp4254= _temp4252.curr
+ yyvsp_offset; if( _temp4254 < _temp4252.base? 1: _temp4254 >= _temp4252.last_plus_one){
_throw( Null_Exception);}* _temp4254;}))).f3;( struct _xenum_struct*) _temp4251;});
break; case 179: _LL4250: yyval=({ struct Cyc_Type_tok_struct* _temp4256=(
struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp4256->tag= Cyc_Type_tok_tag; _temp4256->f1=( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp4257=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp4257->tag= Cyc_Absyn_JoinEff_tag; _temp4257->f1= 0;( void*) _temp4257;});(
struct _xenum_struct*) _temp4256;}); break; case 180: _LL4255: yyval=({ struct
Cyc_Type_tok_struct* _temp4259=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof(
struct Cyc_Type_tok_struct)); _temp4259->tag= Cyc_Type_tok_tag; _temp4259->f1=(
void*)({ struct Cyc_Absyn_JoinEff_struct* _temp4260=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp4260->tag= Cyc_Absyn_JoinEff_tag;
_temp4260->f1= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp4261= yyvs;
struct _xenum_struct** _temp4263= _temp4261.curr +( yyvsp_offset - 1); if(
_temp4263 < _temp4261.base? 1: _temp4263 >= _temp4261.last_plus_one){ _throw(
Null_Exception);}* _temp4263;}));( void*) _temp4260;});( struct _xenum_struct*)
_temp4259;}); break; case 181: _LL4258: yyval=({ struct Cyc_Type_tok_struct*
_temp4265=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));
_temp4265->tag= Cyc_Type_tok_tag; _temp4265->f1=( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp4266=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp4266->tag= Cyc_Absyn_JoinEff_tag; _temp4266->f1=({ struct Cyc_List_List*
_temp4267=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4267->hd=( void*) Cyc_yyget_Type_tok(({ struct _tagged_ptr2 _temp4271= yyvs;
struct _xenum_struct** _temp4273= _temp4271.curr +( yyvsp_offset - 2); if(
_temp4273 < _temp4271.base? 1: _temp4273 >= _temp4271.last_plus_one){ _throw(
Null_Exception);}* _temp4273;})); _temp4267->tl= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4268= yyvs; struct _xenum_struct** _temp4270= _temp4268.curr
+ yyvsp_offset; if( _temp4270 < _temp4268.base? 1: _temp4270 >= _temp4268.last_plus_one){
_throw( Null_Exception);}* _temp4270;})); _temp4267;});( void*) _temp4266;});(
struct _xenum_struct*) _temp4265;}); break; case 182: _LL4264: yyval=({ struct
Cyc_TypeList_tok_struct* _temp4275=( struct Cyc_TypeList_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeList_tok_struct)); _temp4275->tag= Cyc_TypeList_tok_tag;
_temp4275->f1=({ struct Cyc_List_List* _temp4276=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4276->hd=( void*) Cyc_yyget_Type_tok(({
struct _tagged_ptr2 _temp4277= yyvs; struct _xenum_struct** _temp4279= _temp4277.curr
+ yyvsp_offset; if( _temp4279 < _temp4277.base? 1: _temp4279 >= _temp4277.last_plus_one){
_throw( Null_Exception);}* _temp4279;})); _temp4276->tl= 0; _temp4276;});(
struct _xenum_struct*) _temp4275;}); break; case 183: _LL4274: yyval=({ struct
Cyc_TypeList_tok_struct* _temp4281=( struct Cyc_TypeList_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeList_tok_struct)); _temp4281->tag= Cyc_TypeList_tok_tag;
_temp4281->f1=({ struct Cyc_List_List* _temp4282=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4282->hd=( void*) Cyc_yyget_Type_tok(({
struct _tagged_ptr2 _temp4286= yyvs; struct _xenum_struct** _temp4288= _temp4286.curr
+ yyvsp_offset; if( _temp4288 < _temp4286.base? 1: _temp4288 >= _temp4286.last_plus_one){
_throw( Null_Exception);}* _temp4288;})); _temp4282->tl= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4283= yyvs; struct _xenum_struct** _temp4285= _temp4283.curr
+( yyvsp_offset - 2); if( _temp4285 < _temp4283.base? 1: _temp4285 >= _temp4283.last_plus_one){
_throw( Null_Exception);}* _temp4285;})); _temp4282;});( struct _xenum_struct*)
_temp4281;}); break; case 184: _LL4280: yyval=({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4290=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4290->tag= Cyc_AbstractDeclarator_tok_tag;
_temp4290->f1=({ struct Cyc_Parse_Abstractdeclarator* _temp4291=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4291->tms= Cyc_yyget_TypeModifierList_tok(({
struct _tagged_ptr2 _temp4292= yyvs; struct _xenum_struct** _temp4294= _temp4292.curr
+ yyvsp_offset; if( _temp4294 < _temp4292.base? 1: _temp4294 >= _temp4292.last_plus_one){
_throw( Null_Exception);}* _temp4294;})); _temp4291;});( struct _xenum_struct*)
_temp4290;}); break; case 185: _LL4289: yyval=({ struct _tagged_ptr2 _temp4296=
yyvs; struct _xenum_struct** _temp4298= _temp4296.curr + yyvsp_offset; if(
_temp4298 < _temp4296.base? 1: _temp4298 >= _temp4296.last_plus_one){ _throw(
Null_Exception);}* _temp4298;}); break; case 186: _LL4295: yyval=({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4300=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4300->tag= Cyc_AbstractDeclarator_tok_tag;
_temp4300->f1=({ struct Cyc_Parse_Abstractdeclarator* _temp4301=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4301->tms=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)(
Cyc_yyget_TypeModifierList_tok(({ struct _tagged_ptr2 _temp4302= yyvs; struct
_xenum_struct** _temp4304= _temp4302.curr +( yyvsp_offset - 1); if( _temp4304 <
_temp4302.base? 1: _temp4304 >= _temp4302.last_plus_one){ _throw( Null_Exception);}*
_temp4304;})),( Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2
_temp4305= yyvs; struct _xenum_struct** _temp4307= _temp4305.curr + yyvsp_offset;
if( _temp4307 < _temp4305.base? 1: _temp4307 >= _temp4305.last_plus_one){ _throw(
Null_Exception);}* _temp4307;})))->tms); _temp4301;});( struct _xenum_struct*)
_temp4300;}); break; case 187: _LL4299: yyval=({ struct _tagged_ptr2 _temp4309=
yyvs; struct _xenum_struct** _temp4311= _temp4309.curr +( yyvsp_offset - 1); if(
_temp4311 < _temp4309.base? 1: _temp4311 >= _temp4309.last_plus_one){ _throw(
Null_Exception);}* _temp4311;}); break; case 188: _LL4308: yyval=({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4313=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4313->tag= Cyc_AbstractDeclarator_tok_tag;
_temp4313->f1=({ struct Cyc_Parse_Abstractdeclarator* _temp4314=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4314->tms=({
struct Cyc_List_List* _temp4315=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4315->hd=( void*) Cyc_Absyn_Carray_mod; _temp4315->tl=
0; _temp4315;}); _temp4314;});( struct _xenum_struct*) _temp4313;}); break; case
189: _LL4312: yyval=({ struct Cyc_AbstractDeclarator_tok_struct* _temp4317=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp4317->tag= Cyc_AbstractDeclarator_tok_tag; _temp4317->f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4318=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4318->tms=({ struct Cyc_List_List* _temp4319=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4319->hd=( void*) Cyc_Absyn_Carray_mod;
_temp4319->tl=( Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2
_temp4320= yyvs; struct _xenum_struct** _temp4322= _temp4320.curr +(
yyvsp_offset - 2); if( _temp4322 < _temp4320.base? 1: _temp4322 >= _temp4320.last_plus_one){
_throw( Null_Exception);}* _temp4322;})))->tms; _temp4319;}); _temp4318;});(
struct _xenum_struct*) _temp4317;}); break; case 190: _LL4316: yyval=({ struct
Cyc_AbstractDeclarator_tok_struct* _temp4324=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp4324->tag=
Cyc_AbstractDeclarator_tok_tag; _temp4324->f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4325=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4325->tms=({ struct Cyc_List_List* _temp4326=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4326->hd=( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp4327=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp4327->tag= Cyc_Absyn_ConstArray_mod_tag;
_temp4327->f1= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4328= yyvs; struct
_xenum_struct** _temp4330= _temp4328.curr +( yyvsp_offset - 1); if( _temp4330 <
_temp4328.base? 1: _temp4330 >= _temp4328.last_plus_one){ _throw( Null_Exception);}*
_temp4330;}));( void*) _temp4327;}); _temp4326->tl= 0; _temp4326;}); _temp4325;});(
struct _xenum_struct*) _temp4324;}); break; case 191: _LL4323: yyval=({ struct
Cyc_AbstractDeclarator_tok_struct* _temp4332=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp4332->tag=
Cyc_AbstractDeclarator_tok_tag; _temp4332->f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4333=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4333->tms=({ struct Cyc_List_List* _temp4334=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4334->hd=( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp4338=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp4338->tag= Cyc_Absyn_ConstArray_mod_tag;
_temp4338->f1= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4339= yyvs; struct
_xenum_struct** _temp4341= _temp4339.curr +( yyvsp_offset - 1); if( _temp4341 <
_temp4339.base? 1: _temp4341 >= _temp4339.last_plus_one){ _throw( Null_Exception);}*
_temp4341;}));( void*) _temp4338;}); _temp4334->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp4335= yyvs; struct _xenum_struct** _temp4337= _temp4335.curr
+( yyvsp_offset - 3); if( _temp4337 < _temp4335.base? 1: _temp4337 >= _temp4335.last_plus_one){
_throw( Null_Exception);}* _temp4337;})))->tms; _temp4334;}); _temp4333;});(
struct _xenum_struct*) _temp4332;}); break; case 192: _LL4331: yyval=({ struct
Cyc_AbstractDeclarator_tok_struct* _temp4343=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp4343->tag=
Cyc_AbstractDeclarator_tok_tag; _temp4343->f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4344=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4344->tms=({ struct Cyc_List_List* _temp4345=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4345->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4346=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4346->tag= Cyc_Absyn_Function_mod_tag; _temp4346->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4347=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4347->tag= Cyc_Absyn_WithTypes_tag; _temp4347->f1= 0; _temp4347->f2= 0;
_temp4347->f3= 0;( void*) _temp4347;});( void*) _temp4346;}); _temp4345->tl= 0;
_temp4345;}); _temp4344;});( struct _xenum_struct*) _temp4343;}); break; case
193: _LL4342: yyval=({ struct Cyc_AbstractDeclarator_tok_struct* _temp4349=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));
_temp4349->tag= Cyc_AbstractDeclarator_tok_tag; _temp4349->f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4350=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4350->tms=({ struct Cyc_List_List* _temp4351=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4351->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4352=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4352->tag= Cyc_Absyn_Function_mod_tag; _temp4352->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4353=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4353->tag= Cyc_Absyn_WithTypes_tag; _temp4353->f1= 0; _temp4353->f2= 0;
_temp4353->f3=({ struct Cyc_Core_Opt* _temp4354=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp4354->v=( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp4355=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp4355->tag= Cyc_Absyn_JoinEff_tag; _temp4355->f1= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4356= yyvs; struct _xenum_struct** _temp4358= _temp4356.curr
+( yyvsp_offset - 1); if( _temp4358 < _temp4356.base? 1: _temp4358 >= _temp4356.last_plus_one){
_throw( Null_Exception);}* _temp4358;}));( void*) _temp4355;}); _temp4354;});(
void*) _temp4353;});( void*) _temp4352;}); _temp4351->tl= 0; _temp4351;});
_temp4350;});( struct _xenum_struct*) _temp4349;}); break; case 194: _LL4348: {
struct _tuple14 _temp4365; struct Cyc_Core_Opt* _temp4366; int _temp4368; struct
Cyc_List_List* _temp4370; struct _tuple14* _temp4363= Cyc_yyget_ParamDeclListBool_tok(({
struct _tagged_ptr2 _temp4360= yyvs; struct _xenum_struct** _temp4362= _temp4360.curr
+( yyvsp_offset - 1); if( _temp4362 < _temp4360.base? 1: _temp4362 >= _temp4360.last_plus_one){
_throw( Null_Exception);}* _temp4362;})); _temp4365=* _temp4363; _LL4371:
_temp4370= _temp4365.f1; goto _LL4369; _LL4369: _temp4368= _temp4365.f2; goto
_LL4367; _LL4367: _temp4366= _temp4365.f3; goto _LL4364; _LL4364: yyval=({
struct Cyc_AbstractDeclarator_tok_struct* _temp4372=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp4372->tag=
Cyc_AbstractDeclarator_tok_tag; _temp4372->f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4373=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4373->tms=({ struct Cyc_List_List* _temp4374=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4374->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4375=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4375->tag= Cyc_Absyn_Function_mod_tag; _temp4375->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4376=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4376->tag= Cyc_Absyn_WithTypes_tag; _temp4376->f1= _temp4370; _temp4376->f2=
_temp4368; _temp4376->f3= _temp4366;( void*) _temp4376;});( void*) _temp4375;});
_temp4374->tl= 0; _temp4374;}); _temp4373;});( struct _xenum_struct*) _temp4372;});
break;} case 195: _LL4359: yyval=({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4378=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4378->tag= Cyc_AbstractDeclarator_tok_tag;
_temp4378->f1=({ struct Cyc_Parse_Abstractdeclarator* _temp4379=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4379->tms=({
struct Cyc_List_List* _temp4380=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4380->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4384=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4384->tag= Cyc_Absyn_Function_mod_tag; _temp4384->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4385=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4385->tag= Cyc_Absyn_WithTypes_tag; _temp4385->f1= 0; _temp4385->f2= 0;
_temp4385->f3= 0;( void*) _temp4385;});( void*) _temp4384;}); _temp4380->tl=(
Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2 _temp4381= yyvs; struct
_xenum_struct** _temp4383= _temp4381.curr +( yyvsp_offset - 2); if( _temp4383 <
_temp4381.base? 1: _temp4383 >= _temp4381.last_plus_one){ _throw( Null_Exception);}*
_temp4383;})))->tms; _temp4380;}); _temp4379;});( struct _xenum_struct*)
_temp4378;}); break; case 196: _LL4377: { struct Cyc_Core_Opt* eff=({ struct Cyc_Core_Opt*
_temp4387=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp4387->v=( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp4388=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp4388->tag= Cyc_Absyn_JoinEff_tag;
_temp4388->f1= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp4389= yyvs;
struct _xenum_struct** _temp4391= _temp4389.curr +( yyvsp_offset - 1); if(
_temp4391 < _temp4389.base? 1: _temp4391 >= _temp4389.last_plus_one){ _throw(
Null_Exception);}* _temp4391;}));( void*) _temp4388;}); _temp4387;}); yyval=({
struct Cyc_AbstractDeclarator_tok_struct* _temp4392=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp4392->tag=
Cyc_AbstractDeclarator_tok_tag; _temp4392->f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4393=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4393->tms=({ struct Cyc_List_List* _temp4394=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4394->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4398=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4398->tag= Cyc_Absyn_Function_mod_tag; _temp4398->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4399=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4399->tag= Cyc_Absyn_WithTypes_tag; _temp4399->f1= 0; _temp4399->f2= 0;
_temp4399->f3=( struct Cyc_Core_Opt*) eff;( void*) _temp4399;});( void*)
_temp4398;}); _temp4394->tl=( Cyc_yyget_AbstractDeclarator_tok(({ struct
_tagged_ptr2 _temp4395= yyvs; struct _xenum_struct** _temp4397= _temp4395.curr +(
yyvsp_offset - 4); if( _temp4397 < _temp4395.base? 1: _temp4397 >= _temp4395.last_plus_one){
_throw( Null_Exception);}* _temp4397;})))->tms; _temp4394;}); _temp4393;});(
struct _xenum_struct*) _temp4392;}); break;} case 197: _LL4386: { struct
_tuple14 _temp4406; struct Cyc_Core_Opt* _temp4407; int _temp4409; struct Cyc_List_List*
_temp4411; struct _tuple14* _temp4404= Cyc_yyget_ParamDeclListBool_tok(({ struct
_tagged_ptr2 _temp4401= yyvs; struct _xenum_struct** _temp4403= _temp4401.curr +(
yyvsp_offset - 1); if( _temp4403 < _temp4401.base? 1: _temp4403 >= _temp4401.last_plus_one){
_throw( Null_Exception);}* _temp4403;})); _temp4406=* _temp4404; _LL4412:
_temp4411= _temp4406.f1; goto _LL4410; _LL4410: _temp4409= _temp4406.f2; goto
_LL4408; _LL4408: _temp4407= _temp4406.f3; goto _LL4405; _LL4405: yyval=({
struct Cyc_AbstractDeclarator_tok_struct* _temp4413=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp4413->tag=
Cyc_AbstractDeclarator_tok_tag; _temp4413->f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4414=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4414->tms=({ struct Cyc_List_List* _temp4415=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4415->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4419=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4419->tag= Cyc_Absyn_Function_mod_tag; _temp4419->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4420=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4420->tag= Cyc_Absyn_WithTypes_tag; _temp4420->f1= _temp4411; _temp4420->f2=
_temp4409; _temp4420->f3= _temp4407;( void*) _temp4420;});( void*) _temp4419;});
_temp4415->tl=( Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2
_temp4416= yyvs; struct _xenum_struct** _temp4418= _temp4416.curr +(
yyvsp_offset - 3); if( _temp4418 < _temp4416.base? 1: _temp4418 >= _temp4416.last_plus_one){
_throw( Null_Exception);}* _temp4418;})))->tms; _temp4415;}); _temp4414;});(
struct _xenum_struct*) _temp4413;}); break;} case 198: _LL4400: { struct Cyc_List_List*
ts=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4422=
yyls; struct Cyc_Yyltype* _temp4424= _temp4422.curr +( yylsp_offset - 2); if(
_temp4424 < _temp4422.base? 1: _temp4424 >= _temp4422.last_plus_one){ _throw(
Null_Exception);}* _temp4424;}).first_line,({ struct _tagged_ptr3 _temp4425=
yyls; struct Cyc_Yyltype* _temp4427= _temp4425.curr + yylsp_offset; if(
_temp4427 < _temp4425.base? 1: _temp4427 >= _temp4425.last_plus_one){ _throw(
Null_Exception);}* _temp4427;}).last_line),(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp4428= yyvs; struct _xenum_struct** _temp4430= _temp4428.curr +(
yyvsp_offset - 1); if( _temp4430 < _temp4428.base? 1: _temp4430 >= _temp4428.last_plus_one){
_throw( Null_Exception);}* _temp4430;})))); yyval=({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4431=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4431->tag= Cyc_AbstractDeclarator_tok_tag;
_temp4431->f1=({ struct Cyc_Parse_Abstractdeclarator* _temp4432=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4432->tms=({
struct Cyc_List_List* _temp4433=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4433->hd=( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp4437=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp4437->tag= Cyc_Absyn_TypeParams_mod_tag;
_temp4437->f1= ts; _temp4437->f2= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4438= yyls; struct Cyc_Yyltype* _temp4440= _temp4438.curr +(
yylsp_offset - 2); if( _temp4440 < _temp4438.base? 1: _temp4440 >= _temp4438.last_plus_one){
_throw( Null_Exception);}* _temp4440;}).first_line,({ struct _tagged_ptr3
_temp4441= yyls; struct Cyc_Yyltype* _temp4443= _temp4441.curr + yylsp_offset;
if( _temp4443 < _temp4441.base? 1: _temp4443 >= _temp4441.last_plus_one){ _throw(
Null_Exception);}* _temp4443;}).last_line); _temp4437->f3= 0;( void*) _temp4437;});
_temp4433->tl=( Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2
_temp4434= yyvs; struct _xenum_struct** _temp4436= _temp4434.curr +(
yyvsp_offset - 3); if( _temp4436 < _temp4434.base? 1: _temp4436 >= _temp4434.last_plus_one){
_throw( Null_Exception);}* _temp4436;})))->tms; _temp4433;}); _temp4432;});(
struct _xenum_struct*) _temp4431;}); break;} case 199: _LL4421:(( struct Cyc_Lexing_lexbuf*)
Cyc_Parse_lbuf->v)->lex_curr_pos -= 1;{ struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4445= yyls; struct Cyc_Yyltype* _temp4447= _temp4445.curr
+( yylsp_offset - 2); if( _temp4447 < _temp4445.base? 1: _temp4447 >= _temp4445.last_plus_one){
_throw( Null_Exception);}* _temp4447;}).first_line,({ struct _tagged_ptr3
_temp4448= yyls; struct Cyc_Yyltype* _temp4450= _temp4448.curr + yylsp_offset;
if( _temp4450 < _temp4448.base? 1: _temp4450 >= _temp4448.last_plus_one){ _throw(
Null_Exception);}* _temp4450;}).last_line),(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp4451= yyvs; struct _xenum_struct** _temp4453= _temp4451.curr +(
yyvsp_offset - 1); if( _temp4453 < _temp4451.base? 1: _temp4453 >= _temp4451.last_plus_one){
_throw( Null_Exception);}* _temp4453;})))); yyval=({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4454=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct)); _temp4454->tag= Cyc_AbstractDeclarator_tok_tag;
_temp4454->f1=({ struct Cyc_Parse_Abstractdeclarator* _temp4455=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4455->tms=({
struct Cyc_List_List* _temp4456=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4456->hd=( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp4460=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp4460->tag= Cyc_Absyn_TypeParams_mod_tag;
_temp4460->f1= ts; _temp4460->f2= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4461= yyls; struct Cyc_Yyltype* _temp4463= _temp4461.curr +(
yylsp_offset - 2); if( _temp4463 < _temp4461.base? 1: _temp4463 >= _temp4461.last_plus_one){
_throw( Null_Exception);}* _temp4463;}).first_line,({ struct _tagged_ptr3
_temp4464= yyls; struct Cyc_Yyltype* _temp4466= _temp4464.curr + yylsp_offset;
if( _temp4466 < _temp4464.base? 1: _temp4466 >= _temp4464.last_plus_one){ _throw(
Null_Exception);}* _temp4466;}).last_line); _temp4460->f3= 0;( void*) _temp4460;});
_temp4456->tl=( Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2
_temp4457= yyvs; struct _xenum_struct** _temp4459= _temp4457.curr +(
yyvsp_offset - 3); if( _temp4459 < _temp4457.base? 1: _temp4459 >= _temp4457.last_plus_one){
_throw( Null_Exception);}* _temp4459;})))->tms; _temp4456;}); _temp4455;});(
struct _xenum_struct*) _temp4454;}); break;} case 200: _LL4444: yyval=({ struct
Cyc_AbstractDeclarator_tok_struct* _temp4468=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct)); _temp4468->tag=
Cyc_AbstractDeclarator_tok_tag; _temp4468->f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4469=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4469->tms=({ struct Cyc_List_List* _temp4470=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4470->hd=( void*)({ struct Cyc_Absyn_Attributes_mod_struct*
_temp4474=( struct Cyc_Absyn_Attributes_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Attributes_mod_struct)); _temp4474->tag= Cyc_Absyn_Attributes_mod_tag;
_temp4474->f1= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4475=
yyls; struct Cyc_Yyltype* _temp4477= _temp4475.curr + yylsp_offset; if(
_temp4477 < _temp4475.base? 1: _temp4477 >= _temp4475.last_plus_one){ _throw(
Null_Exception);}* _temp4477;}).first_line,({ struct _tagged_ptr3 _temp4478=
yyls; struct Cyc_Yyltype* _temp4480= _temp4478.curr + yylsp_offset; if(
_temp4480 < _temp4478.base? 1: _temp4480 >= _temp4478.last_plus_one){ _throw(
Null_Exception);}* _temp4480;}).last_line); _temp4474->f2= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp4481= yyvs; struct _xenum_struct** _temp4483= _temp4481.curr
+ yyvsp_offset; if( _temp4483 < _temp4481.base? 1: _temp4483 >= _temp4481.last_plus_one){
_throw( Null_Exception);}* _temp4483;}));( void*) _temp4474;}); _temp4470->tl=(
Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2 _temp4471= yyvs; struct
_xenum_struct** _temp4473= _temp4471.curr +( yyvsp_offset - 1); if( _temp4473 <
_temp4471.base? 1: _temp4473 >= _temp4471.last_plus_one){ _throw( Null_Exception);}*
_temp4473;})))->tms; _temp4470;}); _temp4469;});( struct _xenum_struct*)
_temp4468;}); break; case 201: _LL4467: yyval=({ struct _tagged_ptr2 _temp4485=
yyvs; struct _xenum_struct** _temp4487= _temp4485.curr + yyvsp_offset; if(
_temp4487 < _temp4485.base? 1: _temp4487 >= _temp4485.last_plus_one){ _throw(
Null_Exception);}* _temp4487;}); break; case 202: _LL4484: yyval=({ struct
_tagged_ptr2 _temp4489= yyvs; struct _xenum_struct** _temp4491= _temp4489.curr +
yyvsp_offset; if( _temp4491 < _temp4489.base? 1: _temp4491 >= _temp4489.last_plus_one){
_throw( Null_Exception);}* _temp4491;}); break; case 203: _LL4488: yyval=({
struct _tagged_ptr2 _temp4493= yyvs; struct _xenum_struct** _temp4495= _temp4493.curr
+ yyvsp_offset; if( _temp4495 < _temp4493.base? 1: _temp4495 >= _temp4493.last_plus_one){
_throw( Null_Exception);}* _temp4495;}); break; case 204: _LL4492: yyval=({
struct _tagged_ptr2 _temp4497= yyvs; struct _xenum_struct** _temp4499= _temp4497.curr
+ yyvsp_offset; if( _temp4499 < _temp4497.base? 1: _temp4499 >= _temp4497.last_plus_one){
_throw( Null_Exception);}* _temp4499;}); break; case 205: _LL4496: yyval=({
struct _tagged_ptr2 _temp4501= yyvs; struct _xenum_struct** _temp4503= _temp4501.curr
+ yyvsp_offset; if( _temp4503 < _temp4501.base? 1: _temp4503 >= _temp4501.last_plus_one){
_throw( Null_Exception);}* _temp4503;}); break; case 206: _LL4500: yyval=({
struct _tagged_ptr2 _temp4505= yyvs; struct _xenum_struct** _temp4507= _temp4505.curr
+ yyvsp_offset; if( _temp4507 < _temp4505.base? 1: _temp4507 >= _temp4505.last_plus_one){
_throw( Null_Exception);}* _temp4507;}); break; case 207: _LL4504: yyval=({
struct Cyc_Stmt_tok_struct* _temp4509=( struct Cyc_Stmt_tok_struct*) GC_malloc(
sizeof( struct Cyc_Stmt_tok_struct)); _temp4509->tag= Cyc_Stmt_tok_tag;
_temp4509->f1= Cyc_Absyn_new_stmt(({ struct Cyc_Absyn_Cut_s_struct* _temp4510=(
struct Cyc_Absyn_Cut_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Cut_s_struct));
_temp4510->tag= Cyc_Absyn_Cut_s_tag; _temp4510->f1= Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp4511= yyvs; struct _xenum_struct** _temp4513= _temp4511.curr +
yyvsp_offset; if( _temp4513 < _temp4511.base? 1: _temp4513 >= _temp4511.last_plus_one){
_throw( Null_Exception);}* _temp4513;}));( void*) _temp4510;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4514= yyls; struct Cyc_Yyltype* _temp4516= _temp4514.curr
+( yylsp_offset - 1); if( _temp4516 < _temp4514.base? 1: _temp4516 >= _temp4514.last_plus_one){
_throw( Null_Exception);}* _temp4516;}).first_line,({ struct _tagged_ptr3
_temp4517= yyls; struct Cyc_Yyltype* _temp4519= _temp4517.curr + yylsp_offset;
if( _temp4519 < _temp4517.base? 1: _temp4519 >= _temp4517.last_plus_one){ _throw(
Null_Exception);}* _temp4519;}).last_line));( struct _xenum_struct*) _temp4509;});
break; case 208: _LL4508: yyval=({ struct Cyc_Stmt_tok_struct* _temp4521=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp4521->tag= Cyc_Stmt_tok_tag; _temp4521->f1= Cyc_Absyn_new_stmt(({ struct
Cyc_Absyn_Splice_s_struct* _temp4522=( struct Cyc_Absyn_Splice_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Splice_s_struct)); _temp4522->tag= Cyc_Absyn_Splice_s_tag;
_temp4522->f1= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4523= yyvs; struct
_xenum_struct** _temp4525= _temp4523.curr + yyvsp_offset; if( _temp4525 <
_temp4523.base? 1: _temp4525 >= _temp4523.last_plus_one){ _throw( Null_Exception);}*
_temp4525;}));( void*) _temp4522;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4526= yyls; struct Cyc_Yyltype* _temp4528= _temp4526.curr +(
yylsp_offset - 1); if( _temp4528 < _temp4526.base? 1: _temp4528 >= _temp4526.last_plus_one){
_throw( Null_Exception);}* _temp4528;}).first_line,({ struct _tagged_ptr3
_temp4529= yyls; struct Cyc_Yyltype* _temp4531= _temp4529.curr + yylsp_offset;
if( _temp4531 < _temp4529.base? 1: _temp4531 >= _temp4529.last_plus_one){ _throw(
Null_Exception);}* _temp4531;}).last_line));( struct _xenum_struct*) _temp4521;});
break; case 209: _LL4520: yyval=({ struct Cyc_Stmt_tok_struct* _temp4533=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp4533->tag= Cyc_Stmt_tok_tag; _temp4533->f1= Cyc_Absyn_new_stmt(({ struct
Cyc_Absyn_Label_s_struct* _temp4534=( struct Cyc_Absyn_Label_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Label_s_struct)); _temp4534->tag= Cyc_Absyn_Label_s_tag;
_temp4534->f1=({ struct _tagged_string* _temp4535=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp4535[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp4536= yyvs; struct _xenum_struct** _temp4538= _temp4536.curr
+( yyvsp_offset - 2); if( _temp4538 < _temp4536.base? 1: _temp4538 >= _temp4536.last_plus_one){
_throw( Null_Exception);}* _temp4538;})); _temp4535;}); _temp4534->f2= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp4539= yyvs; struct _xenum_struct** _temp4541= _temp4539.curr
+ yyvsp_offset; if( _temp4541 < _temp4539.base? 1: _temp4541 >= _temp4539.last_plus_one){
_throw( Null_Exception);}* _temp4541;}));( void*) _temp4534;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4542= yyls; struct Cyc_Yyltype* _temp4544= _temp4542.curr
+( yylsp_offset - 2); if( _temp4544 < _temp4542.base? 1: _temp4544 >= _temp4542.last_plus_one){
_throw( Null_Exception);}* _temp4544;}).first_line,({ struct _tagged_ptr3
_temp4545= yyls; struct Cyc_Yyltype* _temp4547= _temp4545.curr + yylsp_offset;
if( _temp4547 < _temp4545.base? 1: _temp4547 >= _temp4545.last_plus_one){ _throw(
Null_Exception);}* _temp4547;}).last_line));( struct _xenum_struct*) _temp4533;});
break; case 210: _LL4532: yyval=({ struct Cyc_Stmt_tok_struct* _temp4549=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp4549->tag= Cyc_Stmt_tok_tag; _temp4549->f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4550= yyls; struct Cyc_Yyltype* _temp4552= _temp4550.curr
+ yylsp_offset; if( _temp4552 < _temp4550.base? 1: _temp4552 >= _temp4550.last_plus_one){
_throw( Null_Exception);}* _temp4552;}).first_line,({ struct _tagged_ptr3
_temp4553= yyls; struct Cyc_Yyltype* _temp4555= _temp4553.curr + yylsp_offset;
if( _temp4555 < _temp4553.base? 1: _temp4555 >= _temp4553.last_plus_one){ _throw(
Null_Exception);}* _temp4555;}).last_line));( struct _xenum_struct*) _temp4549;});
break; case 211: _LL4548: yyval=({ struct Cyc_Stmt_tok_struct* _temp4557=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp4557->tag= Cyc_Stmt_tok_tag; _temp4557->f1= Cyc_Absyn_exp_stmt( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4558= yyvs; struct _xenum_struct** _temp4560= _temp4558.curr
+( yyvsp_offset - 1); if( _temp4560 < _temp4558.base? 1: _temp4560 >= _temp4558.last_plus_one){
_throw( Null_Exception);}* _temp4560;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4561= yyls; struct Cyc_Yyltype* _temp4563= _temp4561.curr +(
yylsp_offset - 1); if( _temp4563 < _temp4561.base? 1: _temp4563 >= _temp4561.last_plus_one){
_throw( Null_Exception);}* _temp4563;}).first_line,({ struct _tagged_ptr3
_temp4564= yyls; struct Cyc_Yyltype* _temp4566= _temp4564.curr + yylsp_offset;
if( _temp4566 < _temp4564.base? 1: _temp4566 >= _temp4564.last_plus_one){ _throw(
Null_Exception);}* _temp4566;}).last_line));( struct _xenum_struct*) _temp4557;});
break; case 212: _LL4556: yyval=({ struct Cyc_Stmt_tok_struct* _temp4568=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp4568->tag= Cyc_Stmt_tok_tag; _temp4568->f1= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4569= yyls; struct Cyc_Yyltype* _temp4571= _temp4569.curr
+( yylsp_offset - 1); if( _temp4571 < _temp4569.base? 1: _temp4571 >= _temp4569.last_plus_one){
_throw( Null_Exception);}* _temp4571;}).first_line,({ struct _tagged_ptr3
_temp4572= yyls; struct Cyc_Yyltype* _temp4574= _temp4572.curr + yylsp_offset;
if( _temp4574 < _temp4572.base? 1: _temp4574 >= _temp4572.last_plus_one){ _throw(
Null_Exception);}* _temp4574;}).last_line));( struct _xenum_struct*) _temp4568;});
break; case 213: _LL4567: yyval=({ struct _tagged_ptr2 _temp4576= yyvs; struct
_xenum_struct** _temp4578= _temp4576.curr +( yyvsp_offset - 1); if( _temp4578 <
_temp4576.base? 1: _temp4578 >= _temp4576.last_plus_one){ _throw( Null_Exception);}*
_temp4578;}); break; case 214: _LL4575:{ void* _temp4583= Cyc_yyget_BlockItem_tok(({
struct _tagged_ptr2 _temp4580= yyvs; struct _xenum_struct** _temp4582= _temp4580.curr
+ yyvsp_offset; if( _temp4582 < _temp4580.base? 1: _temp4582 >= _temp4580.last_plus_one){
_throw( Null_Exception);}* _temp4582;})); struct Cyc_List_List* _temp4591;
struct Cyc_Absyn_Fndecl* _temp4593; struct Cyc_Absyn_Stmt* _temp4595; _LL4585:
if((( struct _enum_struct*) _temp4583)->tag == Cyc_Parse_TopDecls_bl_tag){
_LL4592: _temp4591=( struct Cyc_List_List*)(( struct Cyc_Parse_TopDecls_bl_struct*)
_temp4583)->f1; goto _LL4586;} else{ goto _LL4587;} _LL4587: if((( struct
_enum_struct*) _temp4583)->tag == Cyc_Parse_FnDecl_bl_tag){ _LL4594: _temp4593=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Parse_FnDecl_bl_struct*) _temp4583)->f1;
goto _LL4588;} else{ goto _LL4589;} _LL4589: if((( struct _enum_struct*)
_temp4583)->tag == Cyc_Parse_Stmt_bl_tag){ _LL4596: _temp4595=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Parse_Stmt_bl_struct*) _temp4583)->f1; goto _LL4590;} else{ goto
_LL4584;} _LL4586: yyval=({ struct Cyc_Stmt_tok_struct* _temp4597=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp4597->tag= Cyc_Stmt_tok_tag;
_temp4597->f1= Cyc_Parse_flatten_declarations( _temp4591, Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4598= yyls; struct Cyc_Yyltype*
_temp4600= _temp4598.curr + yylsp_offset; if( _temp4600 < _temp4598.base? 1:
_temp4600 >= _temp4598.last_plus_one){ _throw( Null_Exception);}* _temp4600;}).first_line,({
struct _tagged_ptr3 _temp4601= yyls; struct Cyc_Yyltype* _temp4603= _temp4601.curr
+ yylsp_offset; if( _temp4603 < _temp4601.base? 1: _temp4603 >= _temp4601.last_plus_one){
_throw( Null_Exception);}* _temp4603;}).last_line)));( struct _xenum_struct*)
_temp4597;}); goto _LL4584; _LL4588: yyval=({ struct Cyc_Stmt_tok_struct*
_temp4604=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp4604->tag= Cyc_Stmt_tok_tag; _temp4604->f1= Cyc_Parse_flatten_decl( Cyc_Absyn_new_decl(({
struct Cyc_Absyn_Fn_d_struct* _temp4605=( struct Cyc_Absyn_Fn_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct)); _temp4605->tag= Cyc_Absyn_Fn_d_tag;
_temp4605->f1= _temp4593;( void*) _temp4605;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4606= yyls; struct Cyc_Yyltype* _temp4608= _temp4606.curr
+ yylsp_offset; if( _temp4608 < _temp4606.base? 1: _temp4608 >= _temp4606.last_plus_one){
_throw( Null_Exception);}* _temp4608;}).first_line,({ struct _tagged_ptr3
_temp4609= yyls; struct Cyc_Yyltype* _temp4611= _temp4609.curr + yylsp_offset;
if( _temp4611 < _temp4609.base? 1: _temp4611 >= _temp4609.last_plus_one){ _throw(
Null_Exception);}* _temp4611;}).last_line)), Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4612= yyls; struct Cyc_Yyltype* _temp4614= _temp4612.curr
+ yylsp_offset; if( _temp4614 < _temp4612.base? 1: _temp4614 >= _temp4612.last_plus_one){
_throw( Null_Exception);}* _temp4614;}).first_line,({ struct _tagged_ptr3
_temp4615= yyls; struct Cyc_Yyltype* _temp4617= _temp4615.curr + yylsp_offset;
if( _temp4617 < _temp4615.base? 1: _temp4617 >= _temp4615.last_plus_one){ _throw(
Null_Exception);}* _temp4617;}).last_line)));( struct _xenum_struct*) _temp4604;});
goto _LL4584; _LL4590: yyval=({ struct Cyc_Stmt_tok_struct* _temp4618=( struct
Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp4618->tag=
Cyc_Stmt_tok_tag; _temp4618->f1= _temp4595;( struct _xenum_struct*) _temp4618;});
goto _LL4584; _LL4584:;} break; case 215: _LL4579:{ void* _temp4623= Cyc_yyget_BlockItem_tok(({
struct _tagged_ptr2 _temp4620= yyvs; struct _xenum_struct** _temp4622= _temp4620.curr
+( yyvsp_offset - 1); if( _temp4622 < _temp4620.base? 1: _temp4622 >= _temp4620.last_plus_one){
_throw( Null_Exception);}* _temp4622;})); struct Cyc_List_List* _temp4631;
struct Cyc_Absyn_Fndecl* _temp4633; struct Cyc_Absyn_Stmt* _temp4635; _LL4625:
if((( struct _enum_struct*) _temp4623)->tag == Cyc_Parse_TopDecls_bl_tag){
_LL4632: _temp4631=( struct Cyc_List_List*)(( struct Cyc_Parse_TopDecls_bl_struct*)
_temp4623)->f1; goto _LL4626;} else{ goto _LL4627;} _LL4627: if((( struct
_enum_struct*) _temp4623)->tag == Cyc_Parse_FnDecl_bl_tag){ _LL4634: _temp4633=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Parse_FnDecl_bl_struct*) _temp4623)->f1;
goto _LL4628;} else{ goto _LL4629;} _LL4629: if((( struct _enum_struct*)
_temp4623)->tag == Cyc_Parse_Stmt_bl_tag){ _LL4636: _temp4635=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Parse_Stmt_bl_struct*) _temp4623)->f1; goto _LL4630;} else{ goto
_LL4624;} _LL4626: yyval=({ struct Cyc_Stmt_tok_struct* _temp4637=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp4637->tag= Cyc_Stmt_tok_tag;
_temp4637->f1= Cyc_Parse_flatten_declarations( _temp4631, Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp4638= yyvs; struct _xenum_struct** _temp4640= _temp4638.curr
+ yyvsp_offset; if( _temp4640 < _temp4638.base? 1: _temp4640 >= _temp4638.last_plus_one){
_throw( Null_Exception);}* _temp4640;})));( struct _xenum_struct*) _temp4637;});
goto _LL4624; _LL4628: yyval=({ struct Cyc_Stmt_tok_struct* _temp4641=( struct
Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp4641->tag=
Cyc_Stmt_tok_tag; _temp4641->f1= Cyc_Parse_flatten_decl( Cyc_Absyn_new_decl(({
struct Cyc_Absyn_Fn_d_struct* _temp4642=( struct Cyc_Absyn_Fn_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct)); _temp4642->tag= Cyc_Absyn_Fn_d_tag;
_temp4642->f1= _temp4633;( void*) _temp4642;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4643= yyls; struct Cyc_Yyltype* _temp4645= _temp4643.curr
+( yylsp_offset - 1); if( _temp4645 < _temp4643.base? 1: _temp4645 >= _temp4643.last_plus_one){
_throw( Null_Exception);}* _temp4645;}).first_line,({ struct _tagged_ptr3
_temp4646= yyls; struct Cyc_Yyltype* _temp4648= _temp4646.curr +( yylsp_offset -
1); if( _temp4648 < _temp4646.base? 1: _temp4648 >= _temp4646.last_plus_one){
_throw( Null_Exception);}* _temp4648;}).last_line)), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp4649= yyvs; struct _xenum_struct** _temp4651= _temp4649.curr
+ yyvsp_offset; if( _temp4651 < _temp4649.base? 1: _temp4651 >= _temp4649.last_plus_one){
_throw( Null_Exception);}* _temp4651;})));( struct _xenum_struct*) _temp4641;});
goto _LL4624; _LL4630: yyval=({ struct Cyc_Stmt_tok_struct* _temp4652=( struct
Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp4652->tag=
Cyc_Stmt_tok_tag; _temp4652->f1= Cyc_Absyn_seq_stmt( _temp4635, Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp4653= yyvs; struct _xenum_struct** _temp4655= _temp4653.curr
+ yyvsp_offset; if( _temp4655 < _temp4653.base? 1: _temp4655 >= _temp4653.last_plus_one){
_throw( Null_Exception);}* _temp4655;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4656= yyls; struct Cyc_Yyltype* _temp4658= _temp4656.curr +(
yylsp_offset - 1); if( _temp4658 < _temp4656.base? 1: _temp4658 >= _temp4656.last_plus_one){
_throw( Null_Exception);}* _temp4658;}).first_line,({ struct _tagged_ptr3
_temp4659= yyls; struct Cyc_Yyltype* _temp4661= _temp4659.curr + yylsp_offset;
if( _temp4661 < _temp4659.base? 1: _temp4661 >= _temp4659.last_plus_one){ _throw(
Null_Exception);}* _temp4661;}).last_line));( struct _xenum_struct*) _temp4652;});
goto _LL4624; _LL4624:;} break; case 216: _LL4619: yyval=({ struct Cyc_BlockItem_tok_struct*
_temp4663=( struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));
_temp4663->tag= Cyc_BlockItem_tok_tag; _temp4663->f1=( void*)({ struct Cyc_Parse_TopDecls_bl_struct*
_temp4664=( struct Cyc_Parse_TopDecls_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_TopDecls_bl_struct));
_temp4664->tag= Cyc_Parse_TopDecls_bl_tag; _temp4664->f1= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp4665= yyvs; struct _xenum_struct** _temp4667= _temp4665.curr
+ yyvsp_offset; if( _temp4667 < _temp4665.base? 1: _temp4667 >= _temp4665.last_plus_one){
_throw( Null_Exception);}* _temp4667;}));( void*) _temp4664;});( struct
_xenum_struct*) _temp4663;}); break; case 217: _LL4662: yyval=({ struct Cyc_BlockItem_tok_struct*
_temp4669=( struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));
_temp4669->tag= Cyc_BlockItem_tok_tag; _temp4669->f1=( void*)({ struct Cyc_Parse_Stmt_bl_struct*
_temp4670=( struct Cyc_Parse_Stmt_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_Stmt_bl_struct));
_temp4670->tag= Cyc_Parse_Stmt_bl_tag; _temp4670->f1= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp4671= yyvs; struct _xenum_struct** _temp4673= _temp4671.curr
+ yyvsp_offset; if( _temp4673 < _temp4671.base? 1: _temp4673 >= _temp4671.last_plus_one){
_throw( Null_Exception);}* _temp4673;}));( void*) _temp4670;});( struct
_xenum_struct*) _temp4669;}); break; case 218: _LL4668: yyval=({ struct Cyc_BlockItem_tok_struct*
_temp4675=( struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));
_temp4675->tag= Cyc_BlockItem_tok_tag; _temp4675->f1=( void*)({ struct Cyc_Parse_FnDecl_bl_struct*
_temp4676=( struct Cyc_Parse_FnDecl_bl_struct*) GC_malloc( sizeof( struct Cyc_Parse_FnDecl_bl_struct));
_temp4676->tag= Cyc_Parse_FnDecl_bl_tag; _temp4676->f1= Cyc_yyget_FnDecl_tok(({
struct _tagged_ptr2 _temp4677= yyvs; struct _xenum_struct** _temp4679= _temp4677.curr
+ yyvsp_offset; if( _temp4679 < _temp4677.base? 1: _temp4679 >= _temp4677.last_plus_one){
_throw( Null_Exception);}* _temp4679;}));( void*) _temp4676;});( struct
_xenum_struct*) _temp4675;}); break; case 219: _LL4674: yyval=({ struct Cyc_Stmt_tok_struct*
_temp4681=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp4681->tag= Cyc_Stmt_tok_tag; _temp4681->f1= Cyc_Absyn_ifthenelse_stmt( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4682= yyvs; struct _xenum_struct** _temp4684= _temp4682.curr
+( yyvsp_offset - 2); if( _temp4684 < _temp4682.base? 1: _temp4684 >= _temp4682.last_plus_one){
_throw( Null_Exception);}* _temp4684;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp4685= yyvs; struct _xenum_struct** _temp4687= _temp4685.curr +
yyvsp_offset; if( _temp4687 < _temp4685.base? 1: _temp4687 >= _temp4685.last_plus_one){
_throw( Null_Exception);}* _temp4687;})), Cyc_Absyn_skip_stmt( 0), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4688= yyls; struct Cyc_Yyltype* _temp4690= _temp4688.curr
+( yylsp_offset - 4); if( _temp4690 < _temp4688.base? 1: _temp4690 >= _temp4688.last_plus_one){
_throw( Null_Exception);}* _temp4690;}).first_line,({ struct _tagged_ptr3
_temp4691= yyls; struct Cyc_Yyltype* _temp4693= _temp4691.curr + yylsp_offset;
if( _temp4693 < _temp4691.base? 1: _temp4693 >= _temp4691.last_plus_one){ _throw(
Null_Exception);}* _temp4693;}).last_line));( struct _xenum_struct*) _temp4681;});
break; case 220: _LL4680: yyval=({ struct Cyc_Stmt_tok_struct* _temp4695=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp4695->tag= Cyc_Stmt_tok_tag; _temp4695->f1= Cyc_Absyn_ifthenelse_stmt( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4696= yyvs; struct _xenum_struct** _temp4698= _temp4696.curr
+( yyvsp_offset - 4); if( _temp4698 < _temp4696.base? 1: _temp4698 >= _temp4696.last_plus_one){
_throw( Null_Exception);}* _temp4698;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp4699= yyvs; struct _xenum_struct** _temp4701= _temp4699.curr +(
yyvsp_offset - 2); if( _temp4701 < _temp4699.base? 1: _temp4701 >= _temp4699.last_plus_one){
_throw( Null_Exception);}* _temp4701;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp4702= yyvs; struct _xenum_struct** _temp4704= _temp4702.curr +
yyvsp_offset; if( _temp4704 < _temp4702.base? 1: _temp4704 >= _temp4702.last_plus_one){
_throw( Null_Exception);}* _temp4704;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4705= yyls; struct Cyc_Yyltype* _temp4707= _temp4705.curr +(
yylsp_offset - 6); if( _temp4707 < _temp4705.base? 1: _temp4707 >= _temp4705.last_plus_one){
_throw( Null_Exception);}* _temp4707;}).first_line,({ struct _tagged_ptr3
_temp4708= yyls; struct Cyc_Yyltype* _temp4710= _temp4708.curr + yylsp_offset;
if( _temp4710 < _temp4708.base? 1: _temp4710 >= _temp4708.last_plus_one){ _throw(
Null_Exception);}* _temp4710;}).last_line));( struct _xenum_struct*) _temp4695;});
break; case 221: _LL4694: yyval=({ struct Cyc_Stmt_tok_struct* _temp4712=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp4712->tag= Cyc_Stmt_tok_tag; _temp4712->f1= Cyc_Absyn_switch_stmt( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4713= yyvs; struct _xenum_struct** _temp4715= _temp4713.curr
+( yyvsp_offset - 4); if( _temp4715 < _temp4713.base? 1: _temp4715 >= _temp4713.last_plus_one){
_throw( Null_Exception);}* _temp4715;})), Cyc_yyget_SwitchClauseList_tok(({
struct _tagged_ptr2 _temp4716= yyvs; struct _xenum_struct** _temp4718= _temp4716.curr
+( yyvsp_offset - 1); if( _temp4718 < _temp4716.base? 1: _temp4718 >= _temp4716.last_plus_one){
_throw( Null_Exception);}* _temp4718;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4719= yyls; struct Cyc_Yyltype* _temp4721= _temp4719.curr +(
yylsp_offset - 6); if( _temp4721 < _temp4719.base? 1: _temp4721 >= _temp4719.last_plus_one){
_throw( Null_Exception);}* _temp4721;}).first_line,({ struct _tagged_ptr3
_temp4722= yyls; struct Cyc_Yyltype* _temp4724= _temp4722.curr + yylsp_offset;
if( _temp4724 < _temp4722.base? 1: _temp4724 >= _temp4722.last_plus_one){ _throw(
Null_Exception);}* _temp4724;}).last_line));( struct _xenum_struct*) _temp4712;});
break; case 222: _LL4711: yyval=({ struct Cyc_Stmt_tok_struct* _temp4726=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp4726->tag= Cyc_Stmt_tok_tag; _temp4726->f1= Cyc_Absyn_trycatch_stmt( Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp4727= yyvs; struct _xenum_struct** _temp4729= _temp4727.curr
+( yyvsp_offset - 4); if( _temp4729 < _temp4727.base? 1: _temp4729 >= _temp4727.last_plus_one){
_throw( Null_Exception);}* _temp4729;})), Cyc_yyget_SwitchClauseList_tok(({
struct _tagged_ptr2 _temp4730= yyvs; struct _xenum_struct** _temp4732= _temp4730.curr
+( yyvsp_offset - 1); if( _temp4732 < _temp4730.base? 1: _temp4732 >= _temp4730.last_plus_one){
_throw( Null_Exception);}* _temp4732;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4733= yyls; struct Cyc_Yyltype* _temp4735= _temp4733.curr +(
yylsp_offset - 5); if( _temp4735 < _temp4733.base? 1: _temp4735 >= _temp4733.last_plus_one){
_throw( Null_Exception);}* _temp4735;}).first_line,({ struct _tagged_ptr3
_temp4736= yyls; struct Cyc_Yyltype* _temp4738= _temp4736.curr + yylsp_offset;
if( _temp4738 < _temp4736.base? 1: _temp4738 >= _temp4736.last_plus_one){ _throw(
Null_Exception);}* _temp4738;}).last_line));( struct _xenum_struct*) _temp4726;});
break; case 223: _LL4725: yyval=({ struct Cyc_SwitchClauseList_tok_struct*
_temp4740=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp4740->tag= Cyc_SwitchClauseList_tok_tag;
_temp4740->f1= 0;( struct _xenum_struct*) _temp4740;}); break; case 224: _LL4739:
yyval=({ struct Cyc_SwitchClauseList_tok_struct* _temp4742=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp4742->tag= Cyc_SwitchClauseList_tok_tag;
_temp4742->f1=({ struct Cyc_List_List* _temp4743=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4743->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp4744=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp4744->pattern= Cyc_Absyn_new_pat( Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4754= yyls; struct Cyc_Yyltype* _temp4756= _temp4754.curr
+( yylsp_offset - 2); if( _temp4756 < _temp4754.base? 1: _temp4756 >= _temp4754.last_plus_one){
_throw( Null_Exception);}* _temp4756;}).first_line,({ struct _tagged_ptr3
_temp4757= yyls; struct Cyc_Yyltype* _temp4759= _temp4757.curr +( yylsp_offset -
2); if( _temp4759 < _temp4757.base? 1: _temp4759 >= _temp4757.last_plus_one){
_throw( Null_Exception);}* _temp4759;}).last_line)); _temp4744->pat_vars= 0;
_temp4744->where_clause= 0; _temp4744->body= Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp4751= yyvs; struct _xenum_struct** _temp4753= _temp4751.curr +
yyvsp_offset; if( _temp4753 < _temp4751.base? 1: _temp4753 >= _temp4751.last_plus_one){
_throw( Null_Exception);}* _temp4753;})); _temp4744->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4745= yyls; struct Cyc_Yyltype* _temp4747= _temp4745.curr
+( yylsp_offset - 2); if( _temp4747 < _temp4745.base? 1: _temp4747 >= _temp4745.last_plus_one){
_throw( Null_Exception);}* _temp4747;}).first_line,({ struct _tagged_ptr3
_temp4748= yyls; struct Cyc_Yyltype* _temp4750= _temp4748.curr + yylsp_offset;
if( _temp4750 < _temp4748.base? 1: _temp4750 >= _temp4748.last_plus_one){ _throw(
Null_Exception);}* _temp4750;}).last_line); _temp4744;}); _temp4743->tl= 0;
_temp4743;});( struct _xenum_struct*) _temp4742;}); break; case 225: _LL4741:
yyval=({ struct Cyc_SwitchClauseList_tok_struct* _temp4761=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct)); _temp4761->tag= Cyc_SwitchClauseList_tok_tag;
_temp4761->f1=({ struct Cyc_List_List* _temp4762=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4762->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp4766=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp4766->pattern= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp4779= yyvs;
struct _xenum_struct** _temp4781= _temp4779.curr +( yyvsp_offset - 2); if(
_temp4781 < _temp4779.base? 1: _temp4781 >= _temp4779.last_plus_one){ _throw(
Null_Exception);}* _temp4781;})); _temp4766->pat_vars= 0; _temp4766->where_clause=
0; _temp4766->body= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4773= yyls; struct Cyc_Yyltype* _temp4775= _temp4773.curr +(
yylsp_offset - 1); if( _temp4775 < _temp4773.base? 1: _temp4775 >= _temp4773.last_plus_one){
_throw( Null_Exception);}* _temp4775;}).first_line,({ struct _tagged_ptr3
_temp4776= yyls; struct Cyc_Yyltype* _temp4778= _temp4776.curr +( yylsp_offset -
1); if( _temp4778 < _temp4776.base? 1: _temp4778 >= _temp4776.last_plus_one){
_throw( Null_Exception);}* _temp4778;}).last_line)); _temp4766->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4767= yyls; struct Cyc_Yyltype* _temp4769= _temp4767.curr
+( yylsp_offset - 3); if( _temp4769 < _temp4767.base? 1: _temp4769 >= _temp4767.last_plus_one){
_throw( Null_Exception);}* _temp4769;}).first_line,({ struct _tagged_ptr3
_temp4770= yyls; struct Cyc_Yyltype* _temp4772= _temp4770.curr + yylsp_offset;
if( _temp4772 < _temp4770.base? 1: _temp4772 >= _temp4770.last_plus_one){ _throw(
Null_Exception);}* _temp4772;}).last_line); _temp4766;}); _temp4762->tl= Cyc_yyget_SwitchClauseList_tok(({
struct _tagged_ptr2 _temp4763= yyvs; struct _xenum_struct** _temp4765= _temp4763.curr
+ yyvsp_offset; if( _temp4765 < _temp4763.base? 1: _temp4765 >= _temp4763.last_plus_one){
_throw( Null_Exception);}* _temp4765;})); _temp4762;});( struct _xenum_struct*)
_temp4761;}); break; case 226: _LL4760: yyval=({ struct Cyc_SwitchClauseList_tok_struct*
_temp4783=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp4783->tag= Cyc_SwitchClauseList_tok_tag;
_temp4783->f1=({ struct Cyc_List_List* _temp4784=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4784->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp4788=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp4788->pattern= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp4798= yyvs;
struct _xenum_struct** _temp4800= _temp4798.curr +( yyvsp_offset - 3); if(
_temp4800 < _temp4798.base? 1: _temp4800 >= _temp4798.last_plus_one){ _throw(
Null_Exception);}* _temp4800;})); _temp4788->pat_vars= 0; _temp4788->where_clause=
0; _temp4788->body= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4795= yyvs;
struct _xenum_struct** _temp4797= _temp4795.curr +( yyvsp_offset - 1); if(
_temp4797 < _temp4795.base? 1: _temp4797 >= _temp4795.last_plus_one){ _throw(
Null_Exception);}* _temp4797;})); _temp4788->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4789= yyls; struct Cyc_Yyltype* _temp4791= _temp4789.curr
+( yylsp_offset - 4); if( _temp4791 < _temp4789.base? 1: _temp4791 >= _temp4789.last_plus_one){
_throw( Null_Exception);}* _temp4791;}).first_line,({ struct _tagged_ptr3
_temp4792= yyls; struct Cyc_Yyltype* _temp4794= _temp4792.curr +( yylsp_offset -
1); if( _temp4794 < _temp4792.base? 1: _temp4794 >= _temp4792.last_plus_one){
_throw( Null_Exception);}* _temp4794;}).last_line); _temp4788;}); _temp4784->tl=
Cyc_yyget_SwitchClauseList_tok(({ struct _tagged_ptr2 _temp4785= yyvs; struct
_xenum_struct** _temp4787= _temp4785.curr + yyvsp_offset; if( _temp4787 <
_temp4785.base? 1: _temp4787 >= _temp4785.last_plus_one){ _throw( Null_Exception);}*
_temp4787;})); _temp4784;});( struct _xenum_struct*) _temp4783;}); break; case
227: _LL4782: yyval=({ struct Cyc_SwitchClauseList_tok_struct* _temp4802=(
struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));
_temp4802->tag= Cyc_SwitchClauseList_tok_tag; _temp4802->f1=({ struct Cyc_List_List*
_temp4803=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4803->hd=( void*)({ struct Cyc_Absyn_Switch_clause* _temp4807=( struct Cyc_Absyn_Switch_clause*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause)); _temp4807->pattern= Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp4823= yyvs; struct _xenum_struct** _temp4825= _temp4823.curr
+( yyvsp_offset - 4); if( _temp4825 < _temp4823.base? 1: _temp4825 >= _temp4823.last_plus_one){
_throw( Null_Exception);}* _temp4825;})); _temp4807->pat_vars= 0; _temp4807->where_clause=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4820= yyvs;
struct _xenum_struct** _temp4822= _temp4820.curr +( yyvsp_offset - 2); if(
_temp4822 < _temp4820.base? 1: _temp4822 >= _temp4820.last_plus_one){ _throw(
Null_Exception);}* _temp4822;})); _temp4807->body= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4814= yyls; struct Cyc_Yyltype* _temp4816= _temp4814.curr
+( yylsp_offset - 1); if( _temp4816 < _temp4814.base? 1: _temp4816 >= _temp4814.last_plus_one){
_throw( Null_Exception);}* _temp4816;}).first_line,({ struct _tagged_ptr3
_temp4817= yyls; struct Cyc_Yyltype* _temp4819= _temp4817.curr +( yylsp_offset -
1); if( _temp4819 < _temp4817.base? 1: _temp4819 >= _temp4817.last_plus_one){
_throw( Null_Exception);}* _temp4819;}).last_line)); _temp4807->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4808= yyls; struct Cyc_Yyltype* _temp4810= _temp4808.curr
+( yylsp_offset - 5); if( _temp4810 < _temp4808.base? 1: _temp4810 >= _temp4808.last_plus_one){
_throw( Null_Exception);}* _temp4810;}).first_line,({ struct _tagged_ptr3
_temp4811= yyls; struct Cyc_Yyltype* _temp4813= _temp4811.curr + yylsp_offset;
if( _temp4813 < _temp4811.base? 1: _temp4813 >= _temp4811.last_plus_one){ _throw(
Null_Exception);}* _temp4813;}).last_line); _temp4807;}); _temp4803->tl= Cyc_yyget_SwitchClauseList_tok(({
struct _tagged_ptr2 _temp4804= yyvs; struct _xenum_struct** _temp4806= _temp4804.curr
+ yyvsp_offset; if( _temp4806 < _temp4804.base? 1: _temp4806 >= _temp4804.last_plus_one){
_throw( Null_Exception);}* _temp4806;})); _temp4803;});( struct _xenum_struct*)
_temp4802;}); break; case 228: _LL4801: yyval=({ struct Cyc_SwitchClauseList_tok_struct*
_temp4827=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct)); _temp4827->tag= Cyc_SwitchClauseList_tok_tag;
_temp4827->f1=({ struct Cyc_List_List* _temp4828=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4828->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp4832=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp4832->pattern= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp4845= yyvs;
struct _xenum_struct** _temp4847= _temp4845.curr +( yyvsp_offset - 5); if(
_temp4847 < _temp4845.base? 1: _temp4847 >= _temp4845.last_plus_one){ _throw(
Null_Exception);}* _temp4847;})); _temp4832->pat_vars= 0; _temp4832->where_clause=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4842= yyvs;
struct _xenum_struct** _temp4844= _temp4842.curr +( yyvsp_offset - 3); if(
_temp4844 < _temp4842.base? 1: _temp4844 >= _temp4842.last_plus_one){ _throw(
Null_Exception);}* _temp4844;})); _temp4832->body= Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp4839= yyvs; struct _xenum_struct** _temp4841= _temp4839.curr +(
yyvsp_offset - 1); if( _temp4841 < _temp4839.base? 1: _temp4841 >= _temp4839.last_plus_one){
_throw( Null_Exception);}* _temp4841;})); _temp4832->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4833= yyls; struct Cyc_Yyltype* _temp4835= _temp4833.curr
+( yylsp_offset - 6); if( _temp4835 < _temp4833.base? 1: _temp4835 >= _temp4833.last_plus_one){
_throw( Null_Exception);}* _temp4835;}).first_line,({ struct _tagged_ptr3
_temp4836= yyls; struct Cyc_Yyltype* _temp4838= _temp4836.curr + yylsp_offset;
if( _temp4838 < _temp4836.base? 1: _temp4838 >= _temp4836.last_plus_one){ _throw(
Null_Exception);}* _temp4838;}).last_line); _temp4832;}); _temp4828->tl= Cyc_yyget_SwitchClauseList_tok(({
struct _tagged_ptr2 _temp4829= yyvs; struct _xenum_struct** _temp4831= _temp4829.curr
+ yyvsp_offset; if( _temp4831 < _temp4829.base? 1: _temp4831 >= _temp4829.last_plus_one){
_throw( Null_Exception);}* _temp4831;})); _temp4828;});( struct _xenum_struct*)
_temp4827;}); break; case 229: _LL4826: yyval=({ struct Cyc_Stmt_tok_struct*
_temp4849=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp4849->tag= Cyc_Stmt_tok_tag; _temp4849->f1= Cyc_Absyn_while_stmt( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4850= yyvs; struct _xenum_struct** _temp4852= _temp4850.curr
+( yyvsp_offset - 2); if( _temp4852 < _temp4850.base? 1: _temp4852 >= _temp4850.last_plus_one){
_throw( Null_Exception);}* _temp4852;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp4853= yyvs; struct _xenum_struct** _temp4855= _temp4853.curr +
yyvsp_offset; if( _temp4855 < _temp4853.base? 1: _temp4855 >= _temp4853.last_plus_one){
_throw( Null_Exception);}* _temp4855;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4856= yyls; struct Cyc_Yyltype* _temp4858= _temp4856.curr +(
yylsp_offset - 4); if( _temp4858 < _temp4856.base? 1: _temp4858 >= _temp4856.last_plus_one){
_throw( Null_Exception);}* _temp4858;}).first_line,({ struct _tagged_ptr3
_temp4859= yyls; struct Cyc_Yyltype* _temp4861= _temp4859.curr + yylsp_offset;
if( _temp4861 < _temp4859.base? 1: _temp4861 >= _temp4859.last_plus_one){ _throw(
Null_Exception);}* _temp4861;}).last_line));( struct _xenum_struct*) _temp4849;});
break; case 230: _LL4848: yyval=({ struct Cyc_Stmt_tok_struct* _temp4863=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp4863->tag= Cyc_Stmt_tok_tag; _temp4863->f1= Cyc_Absyn_do_stmt( Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp4864= yyvs; struct _xenum_struct** _temp4866= _temp4864.curr
+( yyvsp_offset - 5); if( _temp4866 < _temp4864.base? 1: _temp4866 >= _temp4864.last_plus_one){
_throw( Null_Exception);}* _temp4866;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4867= yyvs; struct _xenum_struct** _temp4869= _temp4867.curr +(
yyvsp_offset - 2); if( _temp4869 < _temp4867.base? 1: _temp4869 >= _temp4867.last_plus_one){
_throw( Null_Exception);}* _temp4869;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4870= yyls; struct Cyc_Yyltype* _temp4872= _temp4870.curr +(
yylsp_offset - 6); if( _temp4872 < _temp4870.base? 1: _temp4872 >= _temp4870.last_plus_one){
_throw( Null_Exception);}* _temp4872;}).first_line,({ struct _tagged_ptr3
_temp4873= yyls; struct Cyc_Yyltype* _temp4875= _temp4873.curr + yylsp_offset;
if( _temp4875 < _temp4873.base? 1: _temp4875 >= _temp4873.last_plus_one){ _throw(
Null_Exception);}* _temp4875;}).last_line));( struct _xenum_struct*) _temp4863;});
break; case 231: _LL4862: yyval=({ struct Cyc_Stmt_tok_struct* _temp4877=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp4877->tag= Cyc_Stmt_tok_tag; _temp4877->f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp4878= yyvs; struct _xenum_struct** _temp4880= _temp4878.curr
+ yyvsp_offset; if( _temp4880 < _temp4878.base? 1: _temp4880 >= _temp4878.last_plus_one){
_throw( Null_Exception);}* _temp4880;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4881= yyls; struct Cyc_Yyltype* _temp4883= _temp4881.curr +(
yylsp_offset - 5); if( _temp4883 < _temp4881.base? 1: _temp4883 >= _temp4881.last_plus_one){
_throw( Null_Exception);}* _temp4883;}).first_line,({ struct _tagged_ptr3
_temp4884= yyls; struct Cyc_Yyltype* _temp4886= _temp4884.curr + yylsp_offset;
if( _temp4886 < _temp4884.base? 1: _temp4886 >= _temp4884.last_plus_one){ _throw(
Null_Exception);}* _temp4886;}).last_line));( struct _xenum_struct*) _temp4877;});
break; case 232: _LL4876: yyval=({ struct Cyc_Stmt_tok_struct* _temp4888=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp4888->tag= Cyc_Stmt_tok_tag; _temp4888->f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4889=
yyvs; struct _xenum_struct** _temp4891= _temp4889.curr +( yyvsp_offset - 2); if(
_temp4891 < _temp4889.base? 1: _temp4891 >= _temp4889.last_plus_one){ _throw(
Null_Exception);}* _temp4891;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp4892= yyvs; struct _xenum_struct** _temp4894= _temp4892.curr + yyvsp_offset;
if( _temp4894 < _temp4892.base? 1: _temp4894 >= _temp4892.last_plus_one){ _throw(
Null_Exception);}* _temp4894;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4895= yyls; struct Cyc_Yyltype* _temp4897= _temp4895.curr +(
yylsp_offset - 6); if( _temp4897 < _temp4895.base? 1: _temp4897 >= _temp4895.last_plus_one){
_throw( Null_Exception);}* _temp4897;}).first_line,({ struct _tagged_ptr3
_temp4898= yyls; struct Cyc_Yyltype* _temp4900= _temp4898.curr + yylsp_offset;
if( _temp4900 < _temp4898.base? 1: _temp4900 >= _temp4898.last_plus_one){ _throw(
Null_Exception);}* _temp4900;}).last_line));( struct _xenum_struct*) _temp4888;});
break; case 233: _LL4887: yyval=({ struct Cyc_Stmt_tok_struct* _temp4902=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp4902->tag= Cyc_Stmt_tok_tag; _temp4902->f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4903= yyvs; struct
_xenum_struct** _temp4905= _temp4903.curr +( yyvsp_offset - 3); if( _temp4905 <
_temp4903.base? 1: _temp4905 >= _temp4903.last_plus_one){ _throw( Null_Exception);}*
_temp4905;})), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp4906= yyvs; struct _xenum_struct** _temp4908= _temp4906.curr +
yyvsp_offset; if( _temp4908 < _temp4906.base? 1: _temp4908 >= _temp4906.last_plus_one){
_throw( Null_Exception);}* _temp4908;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4909= yyls; struct Cyc_Yyltype* _temp4911= _temp4909.curr +(
yylsp_offset - 6); if( _temp4911 < _temp4909.base? 1: _temp4911 >= _temp4909.last_plus_one){
_throw( Null_Exception);}* _temp4911;}).first_line,({ struct _tagged_ptr3
_temp4912= yyls; struct Cyc_Yyltype* _temp4914= _temp4912.curr + yylsp_offset;
if( _temp4914 < _temp4912.base? 1: _temp4914 >= _temp4912.last_plus_one){ _throw(
Null_Exception);}* _temp4914;}).last_line));( struct _xenum_struct*) _temp4902;});
break; case 234: _LL4901: yyval=({ struct Cyc_Stmt_tok_struct* _temp4916=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp4916->tag= Cyc_Stmt_tok_tag; _temp4916->f1= Cyc_Absyn_for_stmt( Cyc_Absyn_false_exp(
0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4917= yyvs; struct
_xenum_struct** _temp4919= _temp4917.curr +( yyvsp_offset - 4); if( _temp4919 <
_temp4917.base? 1: _temp4919 >= _temp4917.last_plus_one){ _throw( Null_Exception);}*
_temp4919;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4920= yyvs; struct
_xenum_struct** _temp4922= _temp4920.curr +( yyvsp_offset - 2); if( _temp4922 <
_temp4920.base? 1: _temp4922 >= _temp4920.last_plus_one){ _throw( Null_Exception);}*
_temp4922;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4923= yyvs; struct
_xenum_struct** _temp4925= _temp4923.curr + yyvsp_offset; if( _temp4925 <
_temp4923.base? 1: _temp4925 >= _temp4923.last_plus_one){ _throw( Null_Exception);}*
_temp4925;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4926=
yyls; struct Cyc_Yyltype* _temp4928= _temp4926.curr +( yylsp_offset - 7); if(
_temp4928 < _temp4926.base? 1: _temp4928 >= _temp4926.last_plus_one){ _throw(
Null_Exception);}* _temp4928;}).first_line,({ struct _tagged_ptr3 _temp4929=
yyls; struct Cyc_Yyltype* _temp4931= _temp4929.curr +( yylsp_offset - 1); if(
_temp4931 < _temp4929.base? 1: _temp4931 >= _temp4929.last_plus_one){ _throw(
Null_Exception);}* _temp4931;}).last_line));( struct _xenum_struct*) _temp4916;});
break; case 235: _LL4915: yyval=({ struct Cyc_Stmt_tok_struct* _temp4933=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp4933->tag= Cyc_Stmt_tok_tag; _temp4933->f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4934= yyvs; struct _xenum_struct** _temp4936= _temp4934.curr
+( yyvsp_offset - 4); if( _temp4936 < _temp4934.base? 1: _temp4936 >= _temp4934.last_plus_one){
_throw( Null_Exception);}* _temp4936;})), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp(
0), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4937= yyvs; struct
_xenum_struct** _temp4939= _temp4937.curr + yyvsp_offset; if( _temp4939 <
_temp4937.base? 1: _temp4939 >= _temp4937.last_plus_one){ _throw( Null_Exception);}*
_temp4939;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4940=
yyls; struct Cyc_Yyltype* _temp4942= _temp4940.curr +( yylsp_offset - 6); if(
_temp4942 < _temp4940.base? 1: _temp4942 >= _temp4940.last_plus_one){ _throw(
Null_Exception);}* _temp4942;}).first_line,({ struct _tagged_ptr3 _temp4943=
yyls; struct Cyc_Yyltype* _temp4945= _temp4943.curr + yylsp_offset; if(
_temp4945 < _temp4943.base? 1: _temp4945 >= _temp4943.last_plus_one){ _throw(
Null_Exception);}* _temp4945;}).last_line));( struct _xenum_struct*) _temp4933;});
break; case 236: _LL4932: yyval=({ struct Cyc_Stmt_tok_struct* _temp4947=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp4947->tag= Cyc_Stmt_tok_tag; _temp4947->f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4948= yyvs; struct _xenum_struct** _temp4950= _temp4948.curr
+( yyvsp_offset - 5); if( _temp4950 < _temp4948.base? 1: _temp4950 >= _temp4948.last_plus_one){
_throw( Null_Exception);}* _temp4950;})), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4951= yyvs; struct _xenum_struct** _temp4953= _temp4951.curr
+( yyvsp_offset - 2); if( _temp4953 < _temp4951.base? 1: _temp4953 >= _temp4951.last_plus_one){
_throw( Null_Exception);}* _temp4953;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp4954= yyvs; struct _xenum_struct** _temp4956= _temp4954.curr +
yyvsp_offset; if( _temp4956 < _temp4954.base? 1: _temp4956 >= _temp4954.last_plus_one){
_throw( Null_Exception);}* _temp4956;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4957= yyls; struct Cyc_Yyltype* _temp4959= _temp4957.curr +(
yylsp_offset - 7); if( _temp4959 < _temp4957.base? 1: _temp4959 >= _temp4957.last_plus_one){
_throw( Null_Exception);}* _temp4959;}).first_line,({ struct _tagged_ptr3
_temp4960= yyls; struct Cyc_Yyltype* _temp4962= _temp4960.curr + yylsp_offset;
if( _temp4962 < _temp4960.base? 1: _temp4962 >= _temp4960.last_plus_one){ _throw(
Null_Exception);}* _temp4962;}).last_line));( struct _xenum_struct*) _temp4947;});
break; case 237: _LL4946: yyval=({ struct Cyc_Stmt_tok_struct* _temp4964=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp4964->tag= Cyc_Stmt_tok_tag; _temp4964->f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4965= yyvs; struct _xenum_struct** _temp4967= _temp4965.curr
+( yyvsp_offset - 5); if( _temp4967 < _temp4965.base? 1: _temp4967 >= _temp4965.last_plus_one){
_throw( Null_Exception);}* _temp4967;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4968= yyvs; struct _xenum_struct** _temp4970= _temp4968.curr +(
yyvsp_offset - 3); if( _temp4970 < _temp4968.base? 1: _temp4970 >= _temp4968.last_plus_one){
_throw( Null_Exception);}* _temp4970;})), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp4971= yyvs; struct _xenum_struct** _temp4973= _temp4971.curr
+ yyvsp_offset; if( _temp4973 < _temp4971.base? 1: _temp4973 >= _temp4971.last_plus_one){
_throw( Null_Exception);}* _temp4973;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4974= yyls; struct Cyc_Yyltype* _temp4976= _temp4974.curr +(
yylsp_offset - 7); if( _temp4976 < _temp4974.base? 1: _temp4976 >= _temp4974.last_plus_one){
_throw( Null_Exception);}* _temp4976;}).first_line,({ struct _tagged_ptr3
_temp4977= yyls; struct Cyc_Yyltype* _temp4979= _temp4977.curr + yylsp_offset;
if( _temp4979 < _temp4977.base? 1: _temp4979 >= _temp4977.last_plus_one){ _throw(
Null_Exception);}* _temp4979;}).last_line));( struct _xenum_struct*) _temp4964;});
break; case 238: _LL4963: yyval=({ struct Cyc_Stmt_tok_struct* _temp4981=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp4981->tag= Cyc_Stmt_tok_tag; _temp4981->f1= Cyc_Absyn_for_stmt( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4982= yyvs; struct _xenum_struct** _temp4984= _temp4982.curr
+( yyvsp_offset - 6); if( _temp4984 < _temp4982.base? 1: _temp4984 >= _temp4982.last_plus_one){
_throw( Null_Exception);}* _temp4984;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4985= yyvs; struct _xenum_struct** _temp4987= _temp4985.curr +(
yyvsp_offset - 4); if( _temp4987 < _temp4985.base? 1: _temp4987 >= _temp4985.last_plus_one){
_throw( Null_Exception);}* _temp4987;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4988= yyvs; struct _xenum_struct** _temp4990= _temp4988.curr +(
yyvsp_offset - 2); if( _temp4990 < _temp4988.base? 1: _temp4990 >= _temp4988.last_plus_one){
_throw( Null_Exception);}* _temp4990;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp4991= yyvs; struct _xenum_struct** _temp4993= _temp4991.curr +
yyvsp_offset; if( _temp4993 < _temp4991.base? 1: _temp4993 >= _temp4991.last_plus_one){
_throw( Null_Exception);}* _temp4993;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4994= yyls; struct Cyc_Yyltype* _temp4996= _temp4994.curr +(
yylsp_offset - 8); if( _temp4996 < _temp4994.base? 1: _temp4996 >= _temp4994.last_plus_one){
_throw( Null_Exception);}* _temp4996;}).first_line,({ struct _tagged_ptr3
_temp4997= yyls; struct Cyc_Yyltype* _temp4999= _temp4997.curr + yylsp_offset;
if( _temp4999 < _temp4997.base? 1: _temp4999 >= _temp4997.last_plus_one){ _throw(
Null_Exception);}* _temp4999;}).last_line));( struct _xenum_struct*) _temp4981;});
break; case 239: _LL4980: { struct Cyc_List_List* decls= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp5001= yyvs; struct _xenum_struct** _temp5003= _temp5001.curr
+( yyvsp_offset - 3); if( _temp5003 < _temp5001.base? 1: _temp5003 >= _temp5001.last_plus_one){
_throw( Null_Exception);}* _temp5003;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5004= yyvs; struct _xenum_struct** _temp5006= _temp5004.curr
+ yyvsp_offset; if( _temp5006 < _temp5004.base? 1: _temp5006 >= _temp5004.last_plus_one){
_throw( Null_Exception);}* _temp5006;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5007= yyls; struct Cyc_Yyltype* _temp5009= _temp5007.curr +(
yylsp_offset - 5); if( _temp5009 < _temp5007.base? 1: _temp5009 >= _temp5007.last_plus_one){
_throw( Null_Exception);}* _temp5009;}).first_line,({ struct _tagged_ptr3
_temp5010= yyls; struct Cyc_Yyltype* _temp5012= _temp5010.curr + yylsp_offset;
if( _temp5012 < _temp5010.base? 1: _temp5012 >= _temp5010.last_plus_one){ _throw(
Null_Exception);}* _temp5012;}).last_line)); yyval=({ struct Cyc_Stmt_tok_struct*
_temp5013=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5013->tag= Cyc_Stmt_tok_tag; _temp5013->f1= Cyc_Parse_flatten_declarations(
decls, s);( struct _xenum_struct*) _temp5013;}); break;} case 240: _LL5000: {
struct Cyc_List_List* decls= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2
_temp5015= yyvs; struct _xenum_struct** _temp5017= _temp5015.curr +(
yyvsp_offset - 4); if( _temp5017 < _temp5015.base? 1: _temp5017 >= _temp5015.last_plus_one){
_throw( Null_Exception);}* _temp5017;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5018=
yyvs; struct _xenum_struct** _temp5020= _temp5018.curr +( yyvsp_offset - 3); if(
_temp5020 < _temp5018.base? 1: _temp5020 >= _temp5018.last_plus_one){ _throw(
Null_Exception);}* _temp5020;})), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5021= yyvs; struct _xenum_struct** _temp5023= _temp5021.curr
+ yyvsp_offset; if( _temp5023 < _temp5021.base? 1: _temp5023 >= _temp5021.last_plus_one){
_throw( Null_Exception);}* _temp5023;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5024= yyls; struct Cyc_Yyltype* _temp5026= _temp5024.curr +(
yylsp_offset - 6); if( _temp5026 < _temp5024.base? 1: _temp5026 >= _temp5024.last_plus_one){
_throw( Null_Exception);}* _temp5026;}).first_line,({ struct _tagged_ptr3
_temp5027= yyls; struct Cyc_Yyltype* _temp5029= _temp5027.curr + yylsp_offset;
if( _temp5029 < _temp5027.base? 1: _temp5029 >= _temp5027.last_plus_one){ _throw(
Null_Exception);}* _temp5029;}).last_line)); yyval=({ struct Cyc_Stmt_tok_struct*
_temp5030=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5030->tag= Cyc_Stmt_tok_tag; _temp5030->f1= Cyc_Parse_flatten_declarations(
decls, s);( struct _xenum_struct*) _temp5030;}); break;} case 241: _LL5014: {
struct Cyc_List_List* decls= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2
_temp5032= yyvs; struct _xenum_struct** _temp5034= _temp5032.curr +(
yyvsp_offset - 4); if( _temp5034 < _temp5032.base? 1: _temp5034 >= _temp5032.last_plus_one){
_throw( Null_Exception);}* _temp5034;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5035= yyvs; struct _xenum_struct** _temp5037= _temp5035.curr +(
yyvsp_offset - 2); if( _temp5037 < _temp5035.base? 1: _temp5037 >= _temp5035.last_plus_one){
_throw( Null_Exception);}* _temp5037;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp5038= yyvs; struct _xenum_struct** _temp5040= _temp5038.curr +
yyvsp_offset; if( _temp5040 < _temp5038.base? 1: _temp5040 >= _temp5038.last_plus_one){
_throw( Null_Exception);}* _temp5040;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5041= yyls; struct Cyc_Yyltype* _temp5043= _temp5041.curr +(
yylsp_offset - 6); if( _temp5043 < _temp5041.base? 1: _temp5043 >= _temp5041.last_plus_one){
_throw( Null_Exception);}* _temp5043;}).first_line,({ struct _tagged_ptr3
_temp5044= yyls; struct Cyc_Yyltype* _temp5046= _temp5044.curr + yylsp_offset;
if( _temp5046 < _temp5044.base? 1: _temp5046 >= _temp5044.last_plus_one){ _throw(
Null_Exception);}* _temp5046;}).last_line)); yyval=({ struct Cyc_Stmt_tok_struct*
_temp5047=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5047->tag= Cyc_Stmt_tok_tag; _temp5047->f1= Cyc_Parse_flatten_declarations(
decls, s);( struct _xenum_struct*) _temp5047;}); break;} case 242: _LL5031: {
struct Cyc_List_List* decls= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2
_temp5049= yyvs; struct _xenum_struct** _temp5051= _temp5049.curr +(
yyvsp_offset - 5); if( _temp5051 < _temp5049.base? 1: _temp5051 >= _temp5049.last_plus_one){
_throw( Null_Exception);}* _temp5051;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5052=
yyvs; struct _xenum_struct** _temp5054= _temp5052.curr +( yyvsp_offset - 4); if(
_temp5054 < _temp5052.base? 1: _temp5054 >= _temp5052.last_plus_one){ _throw(
Null_Exception);}* _temp5054;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5055= yyvs; struct _xenum_struct** _temp5057= _temp5055.curr +(
yyvsp_offset - 2); if( _temp5057 < _temp5055.base? 1: _temp5057 >= _temp5055.last_plus_one){
_throw( Null_Exception);}* _temp5057;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp5058= yyvs; struct _xenum_struct** _temp5060= _temp5058.curr +
yyvsp_offset; if( _temp5060 < _temp5058.base? 1: _temp5060 >= _temp5058.last_plus_one){
_throw( Null_Exception);}* _temp5060;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5061= yyls; struct Cyc_Yyltype* _temp5063= _temp5061.curr +(
yylsp_offset - 7); if( _temp5063 < _temp5061.base? 1: _temp5063 >= _temp5061.last_plus_one){
_throw( Null_Exception);}* _temp5063;}).first_line,({ struct _tagged_ptr3
_temp5064= yyls; struct Cyc_Yyltype* _temp5066= _temp5064.curr + yylsp_offset;
if( _temp5066 < _temp5064.base? 1: _temp5066 >= _temp5064.last_plus_one){ _throw(
Null_Exception);}* _temp5066;}).last_line)); yyval=({ struct Cyc_Stmt_tok_struct*
_temp5067=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5067->tag= Cyc_Stmt_tok_tag; _temp5067->f1= Cyc_Parse_flatten_declarations(
decls, s);( struct _xenum_struct*) _temp5067;}); break;} case 243: _LL5048:
yyval=({ struct Cyc_Stmt_tok_struct* _temp5069=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct)); _temp5069->tag= Cyc_Stmt_tok_tag;
_temp5069->f1= Cyc_Absyn_goto_stmt(({ struct _tagged_string* _temp5070=( struct
_tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1); _temp5070[ 0]=
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp5071= yyvs; struct
_xenum_struct** _temp5073= _temp5071.curr +( yyvsp_offset - 1); if( _temp5073 <
_temp5071.base? 1: _temp5073 >= _temp5071.last_plus_one){ _throw( Null_Exception);}*
_temp5073;})); _temp5070;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5074= yyls; struct Cyc_Yyltype* _temp5076= _temp5074.curr +( yylsp_offset -
2); if( _temp5076 < _temp5074.base? 1: _temp5076 >= _temp5074.last_plus_one){
_throw( Null_Exception);}* _temp5076;}).first_line,({ struct _tagged_ptr3
_temp5077= yyls; struct Cyc_Yyltype* _temp5079= _temp5077.curr +( yylsp_offset -
1); if( _temp5079 < _temp5077.base? 1: _temp5079 >= _temp5077.last_plus_one){
_throw( Null_Exception);}* _temp5079;}).last_line));( struct _xenum_struct*)
_temp5069;}); break; case 244: _LL5068: yyval=({ struct Cyc_Stmt_tok_struct*
_temp5081=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5081->tag= Cyc_Stmt_tok_tag; _temp5081->f1= Cyc_Absyn_continue_stmt( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5082= yyls; struct Cyc_Yyltype* _temp5084= _temp5082.curr
+( yylsp_offset - 1); if( _temp5084 < _temp5082.base? 1: _temp5084 >= _temp5082.last_plus_one){
_throw( Null_Exception);}* _temp5084;}).first_line,({ struct _tagged_ptr3
_temp5085= yyls; struct Cyc_Yyltype* _temp5087= _temp5085.curr +( yylsp_offset -
1); if( _temp5087 < _temp5085.base? 1: _temp5087 >= _temp5085.last_plus_one){
_throw( Null_Exception);}* _temp5087;}).last_line));( struct _xenum_struct*)
_temp5081;}); break; case 245: _LL5080: yyval=({ struct Cyc_Stmt_tok_struct*
_temp5089=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5089->tag= Cyc_Stmt_tok_tag; _temp5089->f1= Cyc_Absyn_break_stmt( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5090= yyls; struct Cyc_Yyltype* _temp5092= _temp5090.curr
+( yylsp_offset - 1); if( _temp5092 < _temp5090.base? 1: _temp5092 >= _temp5090.last_plus_one){
_throw( Null_Exception);}* _temp5092;}).first_line,({ struct _tagged_ptr3
_temp5093= yyls; struct Cyc_Yyltype* _temp5095= _temp5093.curr +( yylsp_offset -
1); if( _temp5095 < _temp5093.base? 1: _temp5095 >= _temp5093.last_plus_one){
_throw( Null_Exception);}* _temp5095;}).last_line));( struct _xenum_struct*)
_temp5089;}); break; case 246: _LL5088: yyval=({ struct Cyc_Stmt_tok_struct*
_temp5097=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5097->tag= Cyc_Stmt_tok_tag; _temp5097->f1= Cyc_Absyn_return_stmt( 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5098= yyls; struct Cyc_Yyltype* _temp5100= _temp5098.curr
+( yylsp_offset - 1); if( _temp5100 < _temp5098.base? 1: _temp5100 >= _temp5098.last_plus_one){
_throw( Null_Exception);}* _temp5100;}).first_line,({ struct _tagged_ptr3
_temp5101= yyls; struct Cyc_Yyltype* _temp5103= _temp5101.curr +( yylsp_offset -
1); if( _temp5103 < _temp5101.base? 1: _temp5103 >= _temp5101.last_plus_one){
_throw( Null_Exception);}* _temp5103;}).last_line));( struct _xenum_struct*)
_temp5097;}); break; case 247: _LL5096: yyval=({ struct Cyc_Stmt_tok_struct*
_temp5105=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5105->tag= Cyc_Stmt_tok_tag; _temp5105->f1= Cyc_Absyn_return_stmt(( struct
Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5106= yyvs; struct
_xenum_struct** _temp5108= _temp5106.curr +( yyvsp_offset - 1); if( _temp5108 <
_temp5106.base? 1: _temp5108 >= _temp5106.last_plus_one){ _throw( Null_Exception);}*
_temp5108;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5109=
yyls; struct Cyc_Yyltype* _temp5111= _temp5109.curr +( yylsp_offset - 2); if(
_temp5111 < _temp5109.base? 1: _temp5111 >= _temp5109.last_plus_one){ _throw(
Null_Exception);}* _temp5111;}).first_line,({ struct _tagged_ptr3 _temp5112=
yyls; struct Cyc_Yyltype* _temp5114= _temp5112.curr +( yylsp_offset - 1); if(
_temp5114 < _temp5112.base? 1: _temp5114 >= _temp5112.last_plus_one){ _throw(
Null_Exception);}* _temp5114;}).last_line));( struct _xenum_struct*) _temp5105;});
break; case 248: _LL5104: yyval=({ struct Cyc_Stmt_tok_struct* _temp5116=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5116->tag= Cyc_Stmt_tok_tag; _temp5116->f1= Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5117= yyls; struct Cyc_Yyltype* _temp5119= _temp5117.curr
+( yylsp_offset - 1); if( _temp5119 < _temp5117.base? 1: _temp5119 >= _temp5117.last_plus_one){
_throw( Null_Exception);}* _temp5119;}).first_line,({ struct _tagged_ptr3
_temp5120= yyls; struct Cyc_Yyltype* _temp5122= _temp5120.curr +( yylsp_offset -
1); if( _temp5122 < _temp5120.base? 1: _temp5122 >= _temp5120.last_plus_one){
_throw( Null_Exception);}* _temp5122;}).last_line));( struct _xenum_struct*)
_temp5116;}); break; case 249: _LL5115: yyval=({ struct Cyc_Stmt_tok_struct*
_temp5124=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5124->tag= Cyc_Stmt_tok_tag; _temp5124->f1= Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5125= yyls; struct Cyc_Yyltype* _temp5127= _temp5125.curr
+( yylsp_offset - 3); if( _temp5127 < _temp5125.base? 1: _temp5127 >= _temp5125.last_plus_one){
_throw( Null_Exception);}* _temp5127;}).first_line,({ struct _tagged_ptr3
_temp5128= yyls; struct Cyc_Yyltype* _temp5130= _temp5128.curr +( yylsp_offset -
3); if( _temp5130 < _temp5128.base? 1: _temp5130 >= _temp5128.last_plus_one){
_throw( Null_Exception);}* _temp5130;}).last_line));( struct _xenum_struct*)
_temp5124;}); break; case 250: _LL5123: yyval=({ struct Cyc_Stmt_tok_struct*
_temp5132=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));
_temp5132->tag= Cyc_Stmt_tok_tag; _temp5132->f1= Cyc_Absyn_fallthru_stmt( Cyc_yyget_ExpList_tok(({
struct _tagged_ptr2 _temp5133= yyvs; struct _xenum_struct** _temp5135= _temp5133.curr
+( yyvsp_offset - 2); if( _temp5135 < _temp5133.base? 1: _temp5135 >= _temp5133.last_plus_one){
_throw( Null_Exception);}* _temp5135;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5136= yyls; struct Cyc_Yyltype* _temp5138= _temp5136.curr +(
yylsp_offset - 4); if( _temp5138 < _temp5136.base? 1: _temp5138 >= _temp5136.last_plus_one){
_throw( Null_Exception);}* _temp5138;}).first_line,({ struct _tagged_ptr3
_temp5139= yyls; struct Cyc_Yyltype* _temp5141= _temp5139.curr +( yylsp_offset -
4); if( _temp5141 < _temp5139.base? 1: _temp5141 >= _temp5139.last_plus_one){
_throw( Null_Exception);}* _temp5141;}).last_line));( struct _xenum_struct*)
_temp5132;}); break; case 251: _LL5131: yyval=({ struct Cyc_Pattern_tok_struct*
_temp5143=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5143->tag= Cyc_Pattern_tok_tag; _temp5143->f1= Cyc_Absyn_new_pat( Cyc_Absyn_Wild_p,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5144= yyls; struct Cyc_Yyltype*
_temp5146= _temp5144.curr + yylsp_offset; if( _temp5146 < _temp5144.base? 1:
_temp5146 >= _temp5144.last_plus_one){ _throw( Null_Exception);}* _temp5146;}).first_line,({
struct _tagged_ptr3 _temp5147= yyls; struct Cyc_Yyltype* _temp5149= _temp5147.curr
+ yylsp_offset; if( _temp5149 < _temp5147.base? 1: _temp5149 >= _temp5147.last_plus_one){
_throw( Null_Exception);}* _temp5149;}).last_line));( struct _xenum_struct*)
_temp5143;}); break; case 252: _LL5142: yyval=({ struct _tagged_ptr2 _temp5151=
yyvs; struct _xenum_struct** _temp5153= _temp5151.curr +( yyvsp_offset - 1); if(
_temp5153 < _temp5151.base? 1: _temp5153 >= _temp5151.last_plus_one){ _throw(
Null_Exception);}* _temp5153;}); break; case 253: _LL5150: yyval=({ struct Cyc_Pattern_tok_struct*
_temp5155=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5155->tag= Cyc_Pattern_tok_tag; _temp5155->f1= Cyc_Absyn_new_pat(({ struct
Cyc_Absyn_Int_p_struct* _temp5156=( struct Cyc_Absyn_Int_p_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Int_p_struct)); _temp5156->tag= Cyc_Absyn_Int_p_tag;
_temp5156->f1=( void*)(* Cyc_yyget_Int_tok(({ struct _tagged_ptr2 _temp5157=
yyvs; struct _xenum_struct** _temp5159= _temp5157.curr + yyvsp_offset; if(
_temp5159 < _temp5157.base? 1: _temp5159 >= _temp5157.last_plus_one){ _throw(
Null_Exception);}* _temp5159;}))).f1; _temp5156->f2=(* Cyc_yyget_Int_tok(({
struct _tagged_ptr2 _temp5160= yyvs; struct _xenum_struct** _temp5162= _temp5160.curr
+ yyvsp_offset; if( _temp5162 < _temp5160.base? 1: _temp5162 >= _temp5160.last_plus_one){
_throw( Null_Exception);}* _temp5162;}))).f2;( void*) _temp5156;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5163= yyls; struct Cyc_Yyltype* _temp5165= _temp5163.curr
+ yylsp_offset; if( _temp5165 < _temp5163.base? 1: _temp5165 >= _temp5163.last_plus_one){
_throw( Null_Exception);}* _temp5165;}).first_line,({ struct _tagged_ptr3
_temp5166= yyls; struct Cyc_Yyltype* _temp5168= _temp5166.curr + yylsp_offset;
if( _temp5168 < _temp5166.base? 1: _temp5168 >= _temp5166.last_plus_one){ _throw(
Null_Exception);}* _temp5168;}).last_line));( struct _xenum_struct*) _temp5155;});
break; case 254: _LL5154: yyval=({ struct Cyc_Pattern_tok_struct* _temp5170=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5170->tag= Cyc_Pattern_tok_tag; _temp5170->f1= Cyc_Absyn_new_pat(({ struct
Cyc_Absyn_Int_p_struct* _temp5171=( struct Cyc_Absyn_Int_p_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Int_p_struct)); _temp5171->tag= Cyc_Absyn_Int_p_tag;
_temp5171->f1=( void*) Cyc_Absyn_Signed; _temp5171->f2= -(* Cyc_yyget_Int_tok(({
struct _tagged_ptr2 _temp5172= yyvs; struct _xenum_struct** _temp5174= _temp5172.curr
+ yyvsp_offset; if( _temp5174 < _temp5172.base? 1: _temp5174 >= _temp5172.last_plus_one){
_throw( Null_Exception);}* _temp5174;}))).f2;( void*) _temp5171;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5175= yyls; struct Cyc_Yyltype* _temp5177= _temp5175.curr
+( yylsp_offset - 1); if( _temp5177 < _temp5175.base? 1: _temp5177 >= _temp5175.last_plus_one){
_throw( Null_Exception);}* _temp5177;}).first_line,({ struct _tagged_ptr3
_temp5178= yyls; struct Cyc_Yyltype* _temp5180= _temp5178.curr + yylsp_offset;
if( _temp5180 < _temp5178.base? 1: _temp5180 >= _temp5178.last_plus_one){ _throw(
Null_Exception);}* _temp5180;}).last_line));( struct _xenum_struct*) _temp5170;});
break; case 255: _LL5169: yyval=({ struct Cyc_Pattern_tok_struct* _temp5182=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5182->tag= Cyc_Pattern_tok_tag; _temp5182->f1= Cyc_Absyn_new_pat(({ struct
Cyc_Absyn_Float_p_struct* _temp5183=( struct Cyc_Absyn_Float_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Float_p_struct)); _temp5183->tag= Cyc_Absyn_Float_p_tag;
_temp5183->f1= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp5184= yyvs;
struct _xenum_struct** _temp5186= _temp5184.curr + yyvsp_offset; if( _temp5186 <
_temp5184.base? 1: _temp5186 >= _temp5184.last_plus_one){ _throw( Null_Exception);}*
_temp5186;}));( void*) _temp5183;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5187= yyls; struct Cyc_Yyltype* _temp5189= _temp5187.curr +
yylsp_offset; if( _temp5189 < _temp5187.base? 1: _temp5189 >= _temp5187.last_plus_one){
_throw( Null_Exception);}* _temp5189;}).first_line,({ struct _tagged_ptr3
_temp5190= yyls; struct Cyc_Yyltype* _temp5192= _temp5190.curr + yylsp_offset;
if( _temp5192 < _temp5190.base? 1: _temp5192 >= _temp5190.last_plus_one){ _throw(
Null_Exception);}* _temp5192;}).last_line));( struct _xenum_struct*) _temp5182;});
break; case 256: _LL5181: yyval=({ struct Cyc_Pattern_tok_struct* _temp5194=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5194->tag= Cyc_Pattern_tok_tag; _temp5194->f1= Cyc_Absyn_new_pat(({ struct
Cyc_Absyn_Char_p_struct* _temp5195=( struct Cyc_Absyn_Char_p_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Absyn_Char_p_struct)); _temp5195->tag= Cyc_Absyn_Char_p_tag;
_temp5195->f1= Cyc_yyget_Char_tok(({ struct _tagged_ptr2 _temp5196= yyvs; struct
_xenum_struct** _temp5198= _temp5196.curr + yyvsp_offset; if( _temp5198 <
_temp5196.base? 1: _temp5198 >= _temp5196.last_plus_one){ _throw( Null_Exception);}*
_temp5198;}));( void*) _temp5195;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5199= yyls; struct Cyc_Yyltype* _temp5201= _temp5199.curr +
yylsp_offset; if( _temp5201 < _temp5199.base? 1: _temp5201 >= _temp5199.last_plus_one){
_throw( Null_Exception);}* _temp5201;}).first_line,({ struct _tagged_ptr3
_temp5202= yyls; struct Cyc_Yyltype* _temp5204= _temp5202.curr + yylsp_offset;
if( _temp5204 < _temp5202.base? 1: _temp5204 >= _temp5202.last_plus_one){ _throw(
Null_Exception);}* _temp5204;}).last_line));( struct _xenum_struct*) _temp5194;});
break; case 257: _LL5193: yyval=({ struct Cyc_Pattern_tok_struct* _temp5206=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5206->tag= Cyc_Pattern_tok_tag; _temp5206->f1= Cyc_Absyn_new_pat( Cyc_Absyn_Null_p,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5207= yyls; struct Cyc_Yyltype*
_temp5209= _temp5207.curr + yylsp_offset; if( _temp5209 < _temp5207.base? 1:
_temp5209 >= _temp5207.last_plus_one){ _throw( Null_Exception);}* _temp5209;}).first_line,({
struct _tagged_ptr3 _temp5210= yyls; struct Cyc_Yyltype* _temp5212= _temp5210.curr
+ yylsp_offset; if( _temp5212 < _temp5210.base? 1: _temp5212 >= _temp5210.last_plus_one){
_throw( Null_Exception);}* _temp5212;}).last_line));( struct _xenum_struct*)
_temp5206;}); break; case 258: _LL5205: yyval=({ struct Cyc_Pattern_tok_struct*
_temp5214=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5214->tag= Cyc_Pattern_tok_tag; _temp5214->f1= Cyc_Absyn_new_pat(({ struct
Cyc_Absyn_UnknownId_p_struct* _temp5215=( struct Cyc_Absyn_UnknownId_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_p_struct)); _temp5215->tag= Cyc_Absyn_UnknownId_p_tag;
_temp5215->f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp5216= yyvs;
struct _xenum_struct** _temp5218= _temp5216.curr + yyvsp_offset; if( _temp5218 <
_temp5216.base? 1: _temp5218 >= _temp5216.last_plus_one){ _throw( Null_Exception);}*
_temp5218;}));( void*) _temp5215;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5219= yyls; struct Cyc_Yyltype* _temp5221= _temp5219.curr +
yylsp_offset; if( _temp5221 < _temp5219.base? 1: _temp5221 >= _temp5219.last_plus_one){
_throw( Null_Exception);}* _temp5221;}).first_line,({ struct _tagged_ptr3
_temp5222= yyls; struct Cyc_Yyltype* _temp5224= _temp5222.curr + yylsp_offset;
if( _temp5224 < _temp5222.base? 1: _temp5224 >= _temp5222.last_plus_one){ _throw(
Null_Exception);}* _temp5224;}).last_line));( struct _xenum_struct*) _temp5214;});
break; case 259: _LL5213: { struct Cyc_List_List* tvs=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5226= yyls; struct Cyc_Yyltype* _temp5228= _temp5226.curr
+( yylsp_offset - 3); if( _temp5228 < _temp5226.base? 1: _temp5228 >= _temp5226.last_plus_one){
_throw( Null_Exception);}* _temp5228;}).first_line,({ struct _tagged_ptr3
_temp5229= yyls; struct Cyc_Yyltype* _temp5231= _temp5229.curr +( yylsp_offset -
3); if( _temp5231 < _temp5229.base? 1: _temp5231 >= _temp5229.last_plus_one){
_throw( Null_Exception);}* _temp5231;}).last_line), Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp5232= yyvs; struct _xenum_struct** _temp5234= _temp5232.curr
+( yyvsp_offset - 3); if( _temp5234 < _temp5232.base? 1: _temp5234 >= _temp5232.last_plus_one){
_throw( Null_Exception);}* _temp5234;}))); yyval=({ struct Cyc_Pattern_tok_struct*
_temp5235=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5235->tag= Cyc_Pattern_tok_tag; _temp5235->f1= Cyc_Absyn_new_pat(({ struct
Cyc_Absyn_UnknownCall_p_struct* _temp5236=( struct Cyc_Absyn_UnknownCall_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownCall_p_struct)); _temp5236->tag= Cyc_Absyn_UnknownCall_p_tag;
_temp5236->f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp5237= yyvs;
struct _xenum_struct** _temp5239= _temp5237.curr +( yyvsp_offset - 4); if(
_temp5239 < _temp5237.base? 1: _temp5239 >= _temp5237.last_plus_one){ _throw(
Null_Exception);}* _temp5239;})); _temp5236->f2= tvs; _temp5236->f3= Cyc_yyget_PatternList_tok(({
struct _tagged_ptr2 _temp5240= yyvs; struct _xenum_struct** _temp5242= _temp5240.curr
+( yyvsp_offset - 1); if( _temp5242 < _temp5240.base? 1: _temp5242 >= _temp5240.last_plus_one){
_throw( Null_Exception);}* _temp5242;}));( void*) _temp5236;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5243= yyls; struct Cyc_Yyltype* _temp5245= _temp5243.curr
+( yylsp_offset - 4); if( _temp5245 < _temp5243.base? 1: _temp5245 >= _temp5243.last_plus_one){
_throw( Null_Exception);}* _temp5245;}).first_line,({ struct _tagged_ptr3
_temp5246= yyls; struct Cyc_Yyltype* _temp5248= _temp5246.curr + yylsp_offset;
if( _temp5248 < _temp5246.base? 1: _temp5248 >= _temp5246.last_plus_one){ _throw(
Null_Exception);}* _temp5248;}).last_line));( struct _xenum_struct*) _temp5235;});
break;} case 260: _LL5225: yyval=({ struct Cyc_Pattern_tok_struct* _temp5250=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5250->tag= Cyc_Pattern_tok_tag; _temp5250->f1= Cyc_Absyn_new_pat(({ struct
Cyc_Absyn_Tuple_p_struct* _temp5251=( struct Cyc_Absyn_Tuple_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuple_p_struct)); _temp5251->tag= Cyc_Absyn_Tuple_p_tag;
_temp5251->f1= Cyc_yyget_PatternList_tok(({ struct _tagged_ptr2 _temp5252= yyvs;
struct _xenum_struct** _temp5254= _temp5252.curr +( yyvsp_offset - 1); if(
_temp5254 < _temp5252.base? 1: _temp5254 >= _temp5252.last_plus_one){ _throw(
Null_Exception);}* _temp5254;}));( void*) _temp5251;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5255= yyls; struct Cyc_Yyltype* _temp5257= _temp5255.curr
+( yylsp_offset - 3); if( _temp5257 < _temp5255.base? 1: _temp5257 >= _temp5255.last_plus_one){
_throw( Null_Exception);}* _temp5257;}).first_line,({ struct _tagged_ptr3
_temp5258= yyls; struct Cyc_Yyltype* _temp5260= _temp5258.curr + yylsp_offset;
if( _temp5260 < _temp5258.base? 1: _temp5260 >= _temp5258.last_plus_one){ _throw(
Null_Exception);}* _temp5260;}).last_line));( struct _xenum_struct*) _temp5250;});
break; case 261: _LL5249: { struct Cyc_List_List* tvs=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5262= yyls; struct Cyc_Yyltype* _temp5264= _temp5262.curr
+( yylsp_offset - 2); if( _temp5264 < _temp5262.base? 1: _temp5264 >= _temp5262.last_plus_one){
_throw( Null_Exception);}* _temp5264;}).first_line,({ struct _tagged_ptr3
_temp5265= yyls; struct Cyc_Yyltype* _temp5267= _temp5265.curr +( yylsp_offset -
2); if( _temp5267 < _temp5265.base? 1: _temp5267 >= _temp5265.last_plus_one){
_throw( Null_Exception);}* _temp5267;}).last_line), Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp5268= yyvs; struct _xenum_struct** _temp5270= _temp5268.curr
+( yyvsp_offset - 2); if( _temp5270 < _temp5268.base? 1: _temp5270 >= _temp5268.last_plus_one){
_throw( Null_Exception);}* _temp5270;}))); yyval=({ struct Cyc_Pattern_tok_struct*
_temp5271=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5271->tag= Cyc_Pattern_tok_tag; _temp5271->f1= Cyc_Absyn_new_pat(({ struct
Cyc_Absyn_UnknownFields_p_struct* _temp5272=( struct Cyc_Absyn_UnknownFields_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownFields_p_struct)); _temp5272->tag=
Cyc_Absyn_UnknownFields_p_tag; _temp5272->f1= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp5273= yyvs; struct _xenum_struct** _temp5275= _temp5273.curr +(
yyvsp_offset - 3); if( _temp5275 < _temp5273.base? 1: _temp5275 >= _temp5273.last_plus_one){
_throw( Null_Exception);}* _temp5275;})); _temp5272->f2= tvs; _temp5272->f3= 0;(
void*) _temp5272;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5276= yyls; struct Cyc_Yyltype* _temp5278= _temp5276.curr +( yylsp_offset -
3); if( _temp5278 < _temp5276.base? 1: _temp5278 >= _temp5276.last_plus_one){
_throw( Null_Exception);}* _temp5278;}).first_line,({ struct _tagged_ptr3
_temp5279= yyls; struct Cyc_Yyltype* _temp5281= _temp5279.curr + yylsp_offset;
if( _temp5281 < _temp5279.base? 1: _temp5281 >= _temp5279.last_plus_one){ _throw(
Null_Exception);}* _temp5281;}).last_line));( struct _xenum_struct*) _temp5271;});
break;} case 262: _LL5261: { struct Cyc_List_List* tvs=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5283= yyls; struct Cyc_Yyltype* _temp5285= _temp5283.curr
+( yylsp_offset - 3); if( _temp5285 < _temp5283.base? 1: _temp5285 >= _temp5283.last_plus_one){
_throw( Null_Exception);}* _temp5285;}).first_line,({ struct _tagged_ptr3
_temp5286= yyls; struct Cyc_Yyltype* _temp5288= _temp5286.curr +( yylsp_offset -
3); if( _temp5288 < _temp5286.base? 1: _temp5288 >= _temp5286.last_plus_one){
_throw( Null_Exception);}* _temp5288;}).last_line), Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp5289= yyvs; struct _xenum_struct** _temp5291= _temp5289.curr
+( yyvsp_offset - 3); if( _temp5291 < _temp5289.base? 1: _temp5291 >= _temp5289.last_plus_one){
_throw( Null_Exception);}* _temp5291;}))); yyval=({ struct Cyc_Pattern_tok_struct*
_temp5292=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5292->tag= Cyc_Pattern_tok_tag; _temp5292->f1= Cyc_Absyn_new_pat(({ struct
Cyc_Absyn_UnknownFields_p_struct* _temp5293=( struct Cyc_Absyn_UnknownFields_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownFields_p_struct)); _temp5293->tag=
Cyc_Absyn_UnknownFields_p_tag; _temp5293->f1= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp5294= yyvs; struct _xenum_struct** _temp5296= _temp5294.curr +(
yyvsp_offset - 4); if( _temp5296 < _temp5294.base? 1: _temp5296 >= _temp5294.last_plus_one){
_throw( Null_Exception);}* _temp5296;})); _temp5293->f2= tvs; _temp5293->f3= Cyc_yyget_FieldPatternList_tok(({
struct _tagged_ptr2 _temp5297= yyvs; struct _xenum_struct** _temp5299= _temp5297.curr
+( yyvsp_offset - 1); if( _temp5299 < _temp5297.base? 1: _temp5299 >= _temp5297.last_plus_one){
_throw( Null_Exception);}* _temp5299;}));( void*) _temp5293;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5300= yyls; struct Cyc_Yyltype* _temp5302= _temp5300.curr
+( yylsp_offset - 4); if( _temp5302 < _temp5300.base? 1: _temp5302 >= _temp5300.last_plus_one){
_throw( Null_Exception);}* _temp5302;}).first_line,({ struct _tagged_ptr3
_temp5303= yyls; struct Cyc_Yyltype* _temp5305= _temp5303.curr + yylsp_offset;
if( _temp5305 < _temp5303.base? 1: _temp5305 >= _temp5303.last_plus_one){ _throw(
Null_Exception);}* _temp5305;}).last_line));( struct _xenum_struct*) _temp5292;});
break;} case 263: _LL5282: yyval=({ struct Cyc_Pattern_tok_struct* _temp5307=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5307->tag= Cyc_Pattern_tok_tag; _temp5307->f1= Cyc_Absyn_new_pat(({ struct
Cyc_Absyn_Pointer_p_struct* _temp5308=( struct Cyc_Absyn_Pointer_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_p_struct)); _temp5308->tag= Cyc_Absyn_Pointer_p_tag;
_temp5308->f1= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp5309= yyvs;
struct _xenum_struct** _temp5311= _temp5309.curr + yyvsp_offset; if( _temp5311 <
_temp5309.base? 1: _temp5311 >= _temp5309.last_plus_one){ _throw( Null_Exception);}*
_temp5311;}));( void*) _temp5308;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5312= yyls; struct Cyc_Yyltype* _temp5314= _temp5312.curr +(
yylsp_offset - 1); if( _temp5314 < _temp5312.base? 1: _temp5314 >= _temp5312.last_plus_one){
_throw( Null_Exception);}* _temp5314;}).first_line,({ struct _tagged_ptr3
_temp5315= yyls; struct Cyc_Yyltype* _temp5317= _temp5315.curr + yylsp_offset;
if( _temp5317 < _temp5315.base? 1: _temp5317 >= _temp5315.last_plus_one){ _throw(
Null_Exception);}* _temp5317;}).last_line));( struct _xenum_struct*) _temp5307;});
break; case 264: _LL5306: yyval=({ struct Cyc_Pattern_tok_struct* _temp5319=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));
_temp5319->tag= Cyc_Pattern_tok_tag; _temp5319->f1= Cyc_Absyn_new_pat(({ struct
Cyc_Absyn_Reference_p_struct* _temp5320=( struct Cyc_Absyn_Reference_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Reference_p_struct)); _temp5320->tag= Cyc_Absyn_Reference_p_tag;
_temp5320->f1= Cyc_Absyn_new_vardecl(({ struct _tuple1* _temp5321=( struct
_tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp5321->f1= Cyc_Absyn_Loc_n;
_temp5321->f2=({ struct _tagged_string* _temp5322=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp5322[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp5323= yyvs; struct _xenum_struct** _temp5325= _temp5323.curr
+ yyvsp_offset; if( _temp5325 < _temp5323.base? 1: _temp5325 >= _temp5323.last_plus_one){
_throw( Null_Exception);}* _temp5325;})); _temp5322;}); _temp5321;}), Cyc_Absyn_VoidType,
0);( void*) _temp5320;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5326= yyls; struct Cyc_Yyltype* _temp5328= _temp5326.curr +( yylsp_offset -
1); if( _temp5328 < _temp5326.base? 1: _temp5328 >= _temp5326.last_plus_one){
_throw( Null_Exception);}* _temp5328;}).first_line,({ struct _tagged_ptr3
_temp5329= yyls; struct Cyc_Yyltype* _temp5331= _temp5329.curr + yylsp_offset;
if( _temp5331 < _temp5329.base? 1: _temp5331 >= _temp5329.last_plus_one){ _throw(
Null_Exception);}* _temp5331;}).last_line));( struct _xenum_struct*) _temp5319;});
break; case 265: _LL5318: yyval=({ struct Cyc_PatternList_tok_struct* _temp5333=(
struct Cyc_PatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));
_temp5333->tag= Cyc_PatternList_tok_tag; _temp5333->f1= 0;( struct _xenum_struct*)
_temp5333;}); break; case 266: _LL5332: yyval=({ struct Cyc_PatternList_tok_struct*
_temp5335=( struct Cyc_PatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));
_temp5335->tag= Cyc_PatternList_tok_tag; _temp5335->f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_PatternList_tok(({ struct
_tagged_ptr2 _temp5336= yyvs; struct _xenum_struct** _temp5338= _temp5336.curr +
yyvsp_offset; if( _temp5338 < _temp5336.base? 1: _temp5338 >= _temp5336.last_plus_one){
_throw( Null_Exception);}* _temp5338;})));( struct _xenum_struct*) _temp5335;});
break; case 267: _LL5334: yyval=({ struct Cyc_PatternList_tok_struct* _temp5340=(
struct Cyc_PatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));
_temp5340->tag= Cyc_PatternList_tok_tag; _temp5340->f1=({ struct Cyc_List_List*
_temp5341=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp5341->hd=( void*) Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp5342=
yyvs; struct _xenum_struct** _temp5344= _temp5342.curr + yyvsp_offset; if(
_temp5344 < _temp5342.base? 1: _temp5344 >= _temp5342.last_plus_one){ _throw(
Null_Exception);}* _temp5344;})); _temp5341->tl= 0; _temp5341;});( struct
_xenum_struct*) _temp5340;}); break; case 268: _LL5339: yyval=({ struct Cyc_PatternList_tok_struct*
_temp5346=( struct Cyc_PatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));
_temp5346->tag= Cyc_PatternList_tok_tag; _temp5346->f1=({ struct Cyc_List_List*
_temp5347=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp5347->hd=( void*) Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp5351=
yyvs; struct _xenum_struct** _temp5353= _temp5351.curr + yyvsp_offset; if(
_temp5353 < _temp5351.base? 1: _temp5353 >= _temp5351.last_plus_one){ _throw(
Null_Exception);}* _temp5353;})); _temp5347->tl= Cyc_yyget_PatternList_tok(({
struct _tagged_ptr2 _temp5348= yyvs; struct _xenum_struct** _temp5350= _temp5348.curr
+( yyvsp_offset - 2); if( _temp5350 < _temp5348.base? 1: _temp5350 >= _temp5348.last_plus_one){
_throw( Null_Exception);}* _temp5350;})); _temp5347;});( struct _xenum_struct*)
_temp5346;}); break; case 269: _LL5345: yyval=({ struct Cyc_FieldPattern_tok_struct*
_temp5355=( struct Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp5355->tag= Cyc_FieldPattern_tok_tag; _temp5355->f1=({ struct _tuple15*
_temp5356=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp5356->f1=
0; _temp5356->f2= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp5357= yyvs;
struct _xenum_struct** _temp5359= _temp5357.curr + yyvsp_offset; if( _temp5359 <
_temp5357.base? 1: _temp5359 >= _temp5357.last_plus_one){ _throw( Null_Exception);}*
_temp5359;})); _temp5356;});( struct _xenum_struct*) _temp5355;}); break; case
270: _LL5354: yyval=({ struct Cyc_FieldPattern_tok_struct* _temp5361=( struct
Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));
_temp5361->tag= Cyc_FieldPattern_tok_tag; _temp5361->f1=({ struct _tuple15*
_temp5362=( struct _tuple15*) GC_malloc( sizeof( struct _tuple15)); _temp5362->f1=
Cyc_yyget_DesignatorList_tok(({ struct _tagged_ptr2 _temp5366= yyvs; struct
_xenum_struct** _temp5368= _temp5366.curr +( yyvsp_offset - 1); if( _temp5368 <
_temp5366.base? 1: _temp5368 >= _temp5366.last_plus_one){ _throw( Null_Exception);}*
_temp5368;})); _temp5362->f2= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2
_temp5363= yyvs; struct _xenum_struct** _temp5365= _temp5363.curr + yyvsp_offset;
if( _temp5365 < _temp5363.base? 1: _temp5365 >= _temp5363.last_plus_one){ _throw(
Null_Exception);}* _temp5365;})); _temp5362;});( struct _xenum_struct*)
_temp5361;}); break; case 271: _LL5360: yyval=({ struct Cyc_FieldPatternList_tok_struct*
_temp5370=( struct Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct
Cyc_FieldPatternList_tok_struct)); _temp5370->tag= Cyc_FieldPatternList_tok_tag;
_temp5370->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
Cyc_yyget_FieldPatternList_tok(({ struct _tagged_ptr2 _temp5371= yyvs; struct
_xenum_struct** _temp5373= _temp5371.curr + yyvsp_offset; if( _temp5373 <
_temp5371.base? 1: _temp5373 >= _temp5371.last_plus_one){ _throw( Null_Exception);}*
_temp5373;})));( struct _xenum_struct*) _temp5370;}); break; case 272: _LL5369:
yyval=({ struct Cyc_FieldPatternList_tok_struct* _temp5375=( struct Cyc_FieldPatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp5375->tag= Cyc_FieldPatternList_tok_tag;
_temp5375->f1=({ struct Cyc_List_List* _temp5376=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5376->hd=( void*) Cyc_yyget_FieldPattern_tok(({
struct _tagged_ptr2 _temp5377= yyvs; struct _xenum_struct** _temp5379= _temp5377.curr
+ yyvsp_offset; if( _temp5379 < _temp5377.base? 1: _temp5379 >= _temp5377.last_plus_one){
_throw( Null_Exception);}* _temp5379;})); _temp5376->tl= 0; _temp5376;});(
struct _xenum_struct*) _temp5375;}); break; case 273: _LL5374: yyval=({ struct
Cyc_FieldPatternList_tok_struct* _temp5381=( struct Cyc_FieldPatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct)); _temp5381->tag= Cyc_FieldPatternList_tok_tag;
_temp5381->f1=({ struct Cyc_List_List* _temp5382=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5382->hd=( void*) Cyc_yyget_FieldPattern_tok(({
struct _tagged_ptr2 _temp5386= yyvs; struct _xenum_struct** _temp5388= _temp5386.curr
+ yyvsp_offset; if( _temp5388 < _temp5386.base? 1: _temp5388 >= _temp5386.last_plus_one){
_throw( Null_Exception);}* _temp5388;})); _temp5382->tl= Cyc_yyget_FieldPatternList_tok(({
struct _tagged_ptr2 _temp5383= yyvs; struct _xenum_struct** _temp5385= _temp5383.curr
+( yyvsp_offset - 2); if( _temp5385 < _temp5383.base? 1: _temp5385 >= _temp5383.last_plus_one){
_throw( Null_Exception);}* _temp5385;})); _temp5382;});( struct _xenum_struct*)
_temp5381;}); break; case 274: _LL5380: yyval=({ struct _tagged_ptr2 _temp5390=
yyvs; struct _xenum_struct** _temp5392= _temp5390.curr + yyvsp_offset; if(
_temp5392 < _temp5390.base? 1: _temp5392 >= _temp5390.last_plus_one){ _throw(
Null_Exception);}* _temp5392;}); break; case 275: _LL5389: yyval=({ struct Cyc_Exp_tok_struct*
_temp5394=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp5394->tag= Cyc_Exp_tok_tag; _temp5394->f1= Cyc_Absyn_seq_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5395= yyvs; struct _xenum_struct** _temp5397= _temp5395.curr
+( yyvsp_offset - 2); if( _temp5397 < _temp5395.base? 1: _temp5397 >= _temp5395.last_plus_one){
_throw( Null_Exception);}* _temp5397;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5398= yyvs; struct _xenum_struct** _temp5400= _temp5398.curr +
yyvsp_offset; if( _temp5400 < _temp5398.base? 1: _temp5400 >= _temp5398.last_plus_one){
_throw( Null_Exception);}* _temp5400;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5401= yyls; struct Cyc_Yyltype* _temp5403= _temp5401.curr +(
yylsp_offset - 2); if( _temp5403 < _temp5401.base? 1: _temp5403 >= _temp5401.last_plus_one){
_throw( Null_Exception);}* _temp5403;}).first_line,({ struct _tagged_ptr3
_temp5404= yyls; struct Cyc_Yyltype* _temp5406= _temp5404.curr + yylsp_offset;
if( _temp5406 < _temp5404.base? 1: _temp5406 >= _temp5404.last_plus_one){ _throw(
Null_Exception);}* _temp5406;}).last_line));( struct _xenum_struct*) _temp5394;});
break; case 276: _LL5393: yyval=({ struct _tagged_ptr2 _temp5408= yyvs; struct
_xenum_struct** _temp5410= _temp5408.curr + yyvsp_offset; if( _temp5410 <
_temp5408.base? 1: _temp5410 >= _temp5408.last_plus_one){ _throw( Null_Exception);}*
_temp5410;}); break; case 277: _LL5407: yyval=({ struct Cyc_Exp_tok_struct*
_temp5412=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp5412->tag= Cyc_Exp_tok_tag; _temp5412->f1= Cyc_Absyn_assignop_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5413= yyvs; struct _xenum_struct** _temp5415= _temp5413.curr
+( yyvsp_offset - 2); if( _temp5415 < _temp5413.base? 1: _temp5415 >= _temp5413.last_plus_one){
_throw( Null_Exception);}* _temp5415;})), Cyc_yyget_Primopopt_tok(({ struct
_tagged_ptr2 _temp5416= yyvs; struct _xenum_struct** _temp5418= _temp5416.curr +(
yyvsp_offset - 1); if( _temp5418 < _temp5416.base? 1: _temp5418 >= _temp5416.last_plus_one){
_throw( Null_Exception);}* _temp5418;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5419= yyvs; struct _xenum_struct** _temp5421= _temp5419.curr +
yyvsp_offset; if( _temp5421 < _temp5419.base? 1: _temp5421 >= _temp5419.last_plus_one){
_throw( Null_Exception);}* _temp5421;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5422= yyls; struct Cyc_Yyltype* _temp5424= _temp5422.curr +(
yylsp_offset - 2); if( _temp5424 < _temp5422.base? 1: _temp5424 >= _temp5422.last_plus_one){
_throw( Null_Exception);}* _temp5424;}).first_line,({ struct _tagged_ptr3
_temp5425= yyls; struct Cyc_Yyltype* _temp5427= _temp5425.curr + yylsp_offset;
if( _temp5427 < _temp5425.base? 1: _temp5427 >= _temp5425.last_plus_one){ _throw(
Null_Exception);}* _temp5427;}).last_line));( struct _xenum_struct*) _temp5412;});
break; case 278: _LL5411: yyval=({ struct Cyc_Primopopt_tok_struct* _temp5429=(
struct Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp5429->tag= Cyc_Primopopt_tok_tag; _temp5429->f1= 0;( struct _xenum_struct*)
_temp5429;}); break; case 279: _LL5428: yyval=({ struct Cyc_Primopopt_tok_struct*
_temp5431=( struct Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp5431->tag= Cyc_Primopopt_tok_tag; _temp5431->f1=({ struct Cyc_Core_Opt*
_temp5432=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp5432->v=( void*) Cyc_Absyn_Times; _temp5432;});( struct _xenum_struct*)
_temp5431;}); break; case 280: _LL5430: yyval=({ struct Cyc_Primopopt_tok_struct*
_temp5434=( struct Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp5434->tag= Cyc_Primopopt_tok_tag; _temp5434->f1=({ struct Cyc_Core_Opt*
_temp5435=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp5435->v=( void*) Cyc_Absyn_Div; _temp5435;});( struct _xenum_struct*)
_temp5434;}); break; case 281: _LL5433: yyval=({ struct Cyc_Primopopt_tok_struct*
_temp5437=( struct Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp5437->tag= Cyc_Primopopt_tok_tag; _temp5437->f1=({ struct Cyc_Core_Opt*
_temp5438=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp5438->v=( void*) Cyc_Absyn_Mod; _temp5438;});( struct _xenum_struct*)
_temp5437;}); break; case 282: _LL5436: yyval=({ struct Cyc_Primopopt_tok_struct*
_temp5440=( struct Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp5440->tag= Cyc_Primopopt_tok_tag; _temp5440->f1=({ struct Cyc_Core_Opt*
_temp5441=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp5441->v=( void*) Cyc_Absyn_Plus; _temp5441;});( struct _xenum_struct*)
_temp5440;}); break; case 283: _LL5439: yyval=({ struct Cyc_Primopopt_tok_struct*
_temp5443=( struct Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp5443->tag= Cyc_Primopopt_tok_tag; _temp5443->f1=({ struct Cyc_Core_Opt*
_temp5444=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp5444->v=( void*) Cyc_Absyn_Minus; _temp5444;});( struct _xenum_struct*)
_temp5443;}); break; case 284: _LL5442: yyval=({ struct Cyc_Primopopt_tok_struct*
_temp5446=( struct Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp5446->tag= Cyc_Primopopt_tok_tag; _temp5446->f1=({ struct Cyc_Core_Opt*
_temp5447=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp5447->v=( void*) Cyc_Absyn_Bitlshift; _temp5447;});( struct _xenum_struct*)
_temp5446;}); break; case 285: _LL5445: yyval=({ struct Cyc_Primopopt_tok_struct*
_temp5449=( struct Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp5449->tag= Cyc_Primopopt_tok_tag; _temp5449->f1=({ struct Cyc_Core_Opt*
_temp5450=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp5450->v=( void*) Cyc_Absyn_Bitlrshift; _temp5450;});( struct _xenum_struct*)
_temp5449;}); break; case 286: _LL5448: yyval=({ struct Cyc_Primopopt_tok_struct*
_temp5452=( struct Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp5452->tag= Cyc_Primopopt_tok_tag; _temp5452->f1=({ struct Cyc_Core_Opt*
_temp5453=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp5453->v=( void*) Cyc_Absyn_Bitand; _temp5453;});( struct _xenum_struct*)
_temp5452;}); break; case 287: _LL5451: yyval=({ struct Cyc_Primopopt_tok_struct*
_temp5455=( struct Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp5455->tag= Cyc_Primopopt_tok_tag; _temp5455->f1=({ struct Cyc_Core_Opt*
_temp5456=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp5456->v=( void*) Cyc_Absyn_Bitxor; _temp5456;});( struct _xenum_struct*)
_temp5455;}); break; case 288: _LL5454: yyval=({ struct Cyc_Primopopt_tok_struct*
_temp5458=( struct Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));
_temp5458->tag= Cyc_Primopopt_tok_tag; _temp5458->f1=({ struct Cyc_Core_Opt*
_temp5459=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp5459->v=( void*) Cyc_Absyn_Bitor; _temp5459;});( struct _xenum_struct*)
_temp5458;}); break; case 289: _LL5457: yyval=({ struct _tagged_ptr2 _temp5461=
yyvs; struct _xenum_struct** _temp5463= _temp5461.curr + yyvsp_offset; if(
_temp5463 < _temp5461.base? 1: _temp5463 >= _temp5461.last_plus_one){ _throw(
Null_Exception);}* _temp5463;}); break; case 290: _LL5460: yyval=({ struct Cyc_Exp_tok_struct*
_temp5465=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp5465->tag= Cyc_Exp_tok_tag; _temp5465->f1= Cyc_Absyn_conditional_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5466= yyvs; struct _xenum_struct** _temp5468= _temp5466.curr
+( yyvsp_offset - 4); if( _temp5468 < _temp5466.base? 1: _temp5468 >= _temp5466.last_plus_one){
_throw( Null_Exception);}* _temp5468;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5469= yyvs; struct _xenum_struct** _temp5471= _temp5469.curr +(
yyvsp_offset - 2); if( _temp5471 < _temp5469.base? 1: _temp5471 >= _temp5469.last_plus_one){
_throw( Null_Exception);}* _temp5471;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5472= yyvs; struct _xenum_struct** _temp5474= _temp5472.curr +
yyvsp_offset; if( _temp5474 < _temp5472.base? 1: _temp5474 >= _temp5472.last_plus_one){
_throw( Null_Exception);}* _temp5474;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5475= yyls; struct Cyc_Yyltype* _temp5477= _temp5475.curr +(
yylsp_offset - 4); if( _temp5477 < _temp5475.base? 1: _temp5477 >= _temp5475.last_plus_one){
_throw( Null_Exception);}* _temp5477;}).first_line,({ struct _tagged_ptr3
_temp5478= yyls; struct Cyc_Yyltype* _temp5480= _temp5478.curr + yylsp_offset;
if( _temp5480 < _temp5478.base? 1: _temp5480 >= _temp5478.last_plus_one){ _throw(
Null_Exception);}* _temp5480;}).last_line));( struct _xenum_struct*) _temp5465;});
break; case 291: _LL5464: yyval=({ struct _tagged_ptr2 _temp5482= yyvs; struct
_xenum_struct** _temp5484= _temp5482.curr + yyvsp_offset; if( _temp5484 <
_temp5482.base? 1: _temp5484 >= _temp5482.last_plus_one){ _throw( Null_Exception);}*
_temp5484;}); break; case 292: _LL5481: yyval=({ struct _tagged_ptr2 _temp5486=
yyvs; struct _xenum_struct** _temp5488= _temp5486.curr + yyvsp_offset; if(
_temp5488 < _temp5486.base? 1: _temp5488 >= _temp5486.last_plus_one){ _throw(
Null_Exception);}* _temp5488;}); break; case 293: _LL5485: yyval=({ struct Cyc_Exp_tok_struct*
_temp5490=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp5490->tag= Cyc_Exp_tok_tag; _temp5490->f1= Cyc_Absyn_or_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5491= yyvs; struct _xenum_struct** _temp5493= _temp5491.curr
+( yyvsp_offset - 2); if( _temp5493 < _temp5491.base? 1: _temp5493 >= _temp5491.last_plus_one){
_throw( Null_Exception);}* _temp5493;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5494= yyvs; struct _xenum_struct** _temp5496= _temp5494.curr +
yyvsp_offset; if( _temp5496 < _temp5494.base? 1: _temp5496 >= _temp5494.last_plus_one){
_throw( Null_Exception);}* _temp5496;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5497= yyls; struct Cyc_Yyltype* _temp5499= _temp5497.curr +(
yylsp_offset - 2); if( _temp5499 < _temp5497.base? 1: _temp5499 >= _temp5497.last_plus_one){
_throw( Null_Exception);}* _temp5499;}).first_line,({ struct _tagged_ptr3
_temp5500= yyls; struct Cyc_Yyltype* _temp5502= _temp5500.curr + yylsp_offset;
if( _temp5502 < _temp5500.base? 1: _temp5502 >= _temp5500.last_plus_one){ _throw(
Null_Exception);}* _temp5502;}).last_line));( struct _xenum_struct*) _temp5490;});
break; case 294: _LL5489: yyval=({ struct _tagged_ptr2 _temp5504= yyvs; struct
_xenum_struct** _temp5506= _temp5504.curr + yyvsp_offset; if( _temp5506 <
_temp5504.base? 1: _temp5506 >= _temp5504.last_plus_one){ _throw( Null_Exception);}*
_temp5506;}); break; case 295: _LL5503: yyval=({ struct Cyc_Exp_tok_struct*
_temp5508=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp5508->tag= Cyc_Exp_tok_tag; _temp5508->f1= Cyc_Absyn_and_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5509= yyvs; struct _xenum_struct** _temp5511= _temp5509.curr
+( yyvsp_offset - 2); if( _temp5511 < _temp5509.base? 1: _temp5511 >= _temp5509.last_plus_one){
_throw( Null_Exception);}* _temp5511;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5512= yyvs; struct _xenum_struct** _temp5514= _temp5512.curr +
yyvsp_offset; if( _temp5514 < _temp5512.base? 1: _temp5514 >= _temp5512.last_plus_one){
_throw( Null_Exception);}* _temp5514;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5515= yyls; struct Cyc_Yyltype* _temp5517= _temp5515.curr +(
yylsp_offset - 2); if( _temp5517 < _temp5515.base? 1: _temp5517 >= _temp5515.last_plus_one){
_throw( Null_Exception);}* _temp5517;}).first_line,({ struct _tagged_ptr3
_temp5518= yyls; struct Cyc_Yyltype* _temp5520= _temp5518.curr + yylsp_offset;
if( _temp5520 < _temp5518.base? 1: _temp5520 >= _temp5518.last_plus_one){ _throw(
Null_Exception);}* _temp5520;}).last_line));( struct _xenum_struct*) _temp5508;});
break; case 296: _LL5507: yyval=({ struct _tagged_ptr2 _temp5522= yyvs; struct
_xenum_struct** _temp5524= _temp5522.curr + yyvsp_offset; if( _temp5524 <
_temp5522.base? 1: _temp5524 >= _temp5522.last_plus_one){ _throw( Null_Exception);}*
_temp5524;}); break; case 297: _LL5521: yyval=({ struct Cyc_Exp_tok_struct*
_temp5526=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp5526->tag= Cyc_Exp_tok_tag; _temp5526->f1= Cyc_Absyn_prim2_exp( Cyc_Absyn_Bitor,
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5527= yyvs; struct _xenum_struct**
_temp5529= _temp5527.curr +( yyvsp_offset - 2); if( _temp5529 < _temp5527.base?
1: _temp5529 >= _temp5527.last_plus_one){ _throw( Null_Exception);}* _temp5529;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5530= yyvs; struct _xenum_struct**
_temp5532= _temp5530.curr + yyvsp_offset; if( _temp5532 < _temp5530.base? 1:
_temp5532 >= _temp5530.last_plus_one){ _throw( Null_Exception);}* _temp5532;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5533= yyls; struct Cyc_Yyltype*
_temp5535= _temp5533.curr +( yylsp_offset - 2); if( _temp5535 < _temp5533.base?
1: _temp5535 >= _temp5533.last_plus_one){ _throw( Null_Exception);}* _temp5535;}).first_line,({
struct _tagged_ptr3 _temp5536= yyls; struct Cyc_Yyltype* _temp5538= _temp5536.curr
+ yylsp_offset; if( _temp5538 < _temp5536.base? 1: _temp5538 >= _temp5536.last_plus_one){
_throw( Null_Exception);}* _temp5538;}).last_line));( struct _xenum_struct*)
_temp5526;}); break; case 298: _LL5525: yyval=({ struct _tagged_ptr2 _temp5540=
yyvs; struct _xenum_struct** _temp5542= _temp5540.curr + yyvsp_offset; if(
_temp5542 < _temp5540.base? 1: _temp5542 >= _temp5540.last_plus_one){ _throw(
Null_Exception);}* _temp5542;}); break; case 299: _LL5539: yyval=({ struct Cyc_Exp_tok_struct*
_temp5544=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp5544->tag= Cyc_Exp_tok_tag; _temp5544->f1= Cyc_Absyn_prim2_exp( Cyc_Absyn_Bitxor,
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5545= yyvs; struct _xenum_struct**
_temp5547= _temp5545.curr +( yyvsp_offset - 2); if( _temp5547 < _temp5545.base?
1: _temp5547 >= _temp5545.last_plus_one){ _throw( Null_Exception);}* _temp5547;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5548= yyvs; struct _xenum_struct**
_temp5550= _temp5548.curr + yyvsp_offset; if( _temp5550 < _temp5548.base? 1:
_temp5550 >= _temp5548.last_plus_one){ _throw( Null_Exception);}* _temp5550;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5551= yyls; struct Cyc_Yyltype*
_temp5553= _temp5551.curr +( yylsp_offset - 2); if( _temp5553 < _temp5551.base?
1: _temp5553 >= _temp5551.last_plus_one){ _throw( Null_Exception);}* _temp5553;}).first_line,({
struct _tagged_ptr3 _temp5554= yyls; struct Cyc_Yyltype* _temp5556= _temp5554.curr
+ yylsp_offset; if( _temp5556 < _temp5554.base? 1: _temp5556 >= _temp5554.last_plus_one){
_throw( Null_Exception);}* _temp5556;}).last_line));( struct _xenum_struct*)
_temp5544;}); break; case 300: _LL5543: yyval=({ struct _tagged_ptr2 _temp5558=
yyvs; struct _xenum_struct** _temp5560= _temp5558.curr + yyvsp_offset; if(
_temp5560 < _temp5558.base? 1: _temp5560 >= _temp5558.last_plus_one){ _throw(
Null_Exception);}* _temp5560;}); break; case 301: _LL5557: yyval=({ struct Cyc_Exp_tok_struct*
_temp5562=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp5562->tag= Cyc_Exp_tok_tag; _temp5562->f1= Cyc_Absyn_prim2_exp( Cyc_Absyn_Bitand,
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5563= yyvs; struct _xenum_struct**
_temp5565= _temp5563.curr +( yyvsp_offset - 2); if( _temp5565 < _temp5563.base?
1: _temp5565 >= _temp5563.last_plus_one){ _throw( Null_Exception);}* _temp5565;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5566= yyvs; struct _xenum_struct**
_temp5568= _temp5566.curr + yyvsp_offset; if( _temp5568 < _temp5566.base? 1:
_temp5568 >= _temp5566.last_plus_one){ _throw( Null_Exception);}* _temp5568;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5569= yyls; struct Cyc_Yyltype*
_temp5571= _temp5569.curr +( yylsp_offset - 2); if( _temp5571 < _temp5569.base?
1: _temp5571 >= _temp5569.last_plus_one){ _throw( Null_Exception);}* _temp5571;}).first_line,({
struct _tagged_ptr3 _temp5572= yyls; struct Cyc_Yyltype* _temp5574= _temp5572.curr
+ yylsp_offset; if( _temp5574 < _temp5572.base? 1: _temp5574 >= _temp5572.last_plus_one){
_throw( Null_Exception);}* _temp5574;}).last_line));( struct _xenum_struct*)
_temp5562;}); break; case 302: _LL5561: yyval=({ struct _tagged_ptr2 _temp5576=
yyvs; struct _xenum_struct** _temp5578= _temp5576.curr + yyvsp_offset; if(
_temp5578 < _temp5576.base? 1: _temp5578 >= _temp5576.last_plus_one){ _throw(
Null_Exception);}* _temp5578;}); break; case 303: _LL5575: yyval=({ struct Cyc_Exp_tok_struct*
_temp5580=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp5580->tag= Cyc_Exp_tok_tag; _temp5580->f1= Cyc_Absyn_eq_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5581= yyvs; struct _xenum_struct** _temp5583= _temp5581.curr
+( yyvsp_offset - 2); if( _temp5583 < _temp5581.base? 1: _temp5583 >= _temp5581.last_plus_one){
_throw( Null_Exception);}* _temp5583;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5584= yyvs; struct _xenum_struct** _temp5586= _temp5584.curr +
yyvsp_offset; if( _temp5586 < _temp5584.base? 1: _temp5586 >= _temp5584.last_plus_one){
_throw( Null_Exception);}* _temp5586;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5587= yyls; struct Cyc_Yyltype* _temp5589= _temp5587.curr +(
yylsp_offset - 2); if( _temp5589 < _temp5587.base? 1: _temp5589 >= _temp5587.last_plus_one){
_throw( Null_Exception);}* _temp5589;}).first_line,({ struct _tagged_ptr3
_temp5590= yyls; struct Cyc_Yyltype* _temp5592= _temp5590.curr + yylsp_offset;
if( _temp5592 < _temp5590.base? 1: _temp5592 >= _temp5590.last_plus_one){ _throw(
Null_Exception);}* _temp5592;}).last_line));( struct _xenum_struct*) _temp5580;});
break; case 304: _LL5579: yyval=({ struct Cyc_Exp_tok_struct* _temp5594=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp5594->tag=
Cyc_Exp_tok_tag; _temp5594->f1= Cyc_Absyn_neq_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5595= yyvs; struct _xenum_struct** _temp5597= _temp5595.curr +(
yyvsp_offset - 2); if( _temp5597 < _temp5595.base? 1: _temp5597 >= _temp5595.last_plus_one){
_throw( Null_Exception);}* _temp5597;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5598= yyvs; struct _xenum_struct** _temp5600= _temp5598.curr +
yyvsp_offset; if( _temp5600 < _temp5598.base? 1: _temp5600 >= _temp5598.last_plus_one){
_throw( Null_Exception);}* _temp5600;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5601= yyls; struct Cyc_Yyltype* _temp5603= _temp5601.curr +(
yylsp_offset - 2); if( _temp5603 < _temp5601.base? 1: _temp5603 >= _temp5601.last_plus_one){
_throw( Null_Exception);}* _temp5603;}).first_line,({ struct _tagged_ptr3
_temp5604= yyls; struct Cyc_Yyltype* _temp5606= _temp5604.curr + yylsp_offset;
if( _temp5606 < _temp5604.base? 1: _temp5606 >= _temp5604.last_plus_one){ _throw(
Null_Exception);}* _temp5606;}).last_line));( struct _xenum_struct*) _temp5594;});
break; case 305: _LL5593: yyval=({ struct _tagged_ptr2 _temp5608= yyvs; struct
_xenum_struct** _temp5610= _temp5608.curr + yyvsp_offset; if( _temp5610 <
_temp5608.base? 1: _temp5610 >= _temp5608.last_plus_one){ _throw( Null_Exception);}*
_temp5610;}); break; case 306: _LL5607: yyval=({ struct Cyc_Exp_tok_struct*
_temp5612=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp5612->tag= Cyc_Exp_tok_tag; _temp5612->f1= Cyc_Absyn_lt_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5613= yyvs; struct _xenum_struct** _temp5615= _temp5613.curr
+( yyvsp_offset - 2); if( _temp5615 < _temp5613.base? 1: _temp5615 >= _temp5613.last_plus_one){
_throw( Null_Exception);}* _temp5615;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5616= yyvs; struct _xenum_struct** _temp5618= _temp5616.curr +
yyvsp_offset; if( _temp5618 < _temp5616.base? 1: _temp5618 >= _temp5616.last_plus_one){
_throw( Null_Exception);}* _temp5618;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5619= yyls; struct Cyc_Yyltype* _temp5621= _temp5619.curr +(
yylsp_offset - 2); if( _temp5621 < _temp5619.base? 1: _temp5621 >= _temp5619.last_plus_one){
_throw( Null_Exception);}* _temp5621;}).first_line,({ struct _tagged_ptr3
_temp5622= yyls; struct Cyc_Yyltype* _temp5624= _temp5622.curr + yylsp_offset;
if( _temp5624 < _temp5622.base? 1: _temp5624 >= _temp5622.last_plus_one){ _throw(
Null_Exception);}* _temp5624;}).last_line));( struct _xenum_struct*) _temp5612;});
break; case 307: _LL5611: yyval=({ struct Cyc_Exp_tok_struct* _temp5626=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp5626->tag=
Cyc_Exp_tok_tag; _temp5626->f1= Cyc_Absyn_gt_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5627= yyvs; struct _xenum_struct** _temp5629= _temp5627.curr +(
yyvsp_offset - 2); if( _temp5629 < _temp5627.base? 1: _temp5629 >= _temp5627.last_plus_one){
_throw( Null_Exception);}* _temp5629;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5630= yyvs; struct _xenum_struct** _temp5632= _temp5630.curr +
yyvsp_offset; if( _temp5632 < _temp5630.base? 1: _temp5632 >= _temp5630.last_plus_one){
_throw( Null_Exception);}* _temp5632;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5633= yyls; struct Cyc_Yyltype* _temp5635= _temp5633.curr +(
yylsp_offset - 2); if( _temp5635 < _temp5633.base? 1: _temp5635 >= _temp5633.last_plus_one){
_throw( Null_Exception);}* _temp5635;}).first_line,({ struct _tagged_ptr3
_temp5636= yyls; struct Cyc_Yyltype* _temp5638= _temp5636.curr + yylsp_offset;
if( _temp5638 < _temp5636.base? 1: _temp5638 >= _temp5636.last_plus_one){ _throw(
Null_Exception);}* _temp5638;}).last_line));( struct _xenum_struct*) _temp5626;});
break; case 308: _LL5625: yyval=({ struct Cyc_Exp_tok_struct* _temp5640=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp5640->tag=
Cyc_Exp_tok_tag; _temp5640->f1= Cyc_Absyn_lte_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5641= yyvs; struct _xenum_struct** _temp5643= _temp5641.curr +(
yyvsp_offset - 2); if( _temp5643 < _temp5641.base? 1: _temp5643 >= _temp5641.last_plus_one){
_throw( Null_Exception);}* _temp5643;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5644= yyvs; struct _xenum_struct** _temp5646= _temp5644.curr +
yyvsp_offset; if( _temp5646 < _temp5644.base? 1: _temp5646 >= _temp5644.last_plus_one){
_throw( Null_Exception);}* _temp5646;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5647= yyls; struct Cyc_Yyltype* _temp5649= _temp5647.curr +(
yylsp_offset - 2); if( _temp5649 < _temp5647.base? 1: _temp5649 >= _temp5647.last_plus_one){
_throw( Null_Exception);}* _temp5649;}).first_line,({ struct _tagged_ptr3
_temp5650= yyls; struct Cyc_Yyltype* _temp5652= _temp5650.curr + yylsp_offset;
if( _temp5652 < _temp5650.base? 1: _temp5652 >= _temp5650.last_plus_one){ _throw(
Null_Exception);}* _temp5652;}).last_line));( struct _xenum_struct*) _temp5640;});
break; case 309: _LL5639: yyval=({ struct Cyc_Exp_tok_struct* _temp5654=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp5654->tag=
Cyc_Exp_tok_tag; _temp5654->f1= Cyc_Absyn_gte_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5655= yyvs; struct _xenum_struct** _temp5657= _temp5655.curr +(
yyvsp_offset - 2); if( _temp5657 < _temp5655.base? 1: _temp5657 >= _temp5655.last_plus_one){
_throw( Null_Exception);}* _temp5657;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5658= yyvs; struct _xenum_struct** _temp5660= _temp5658.curr +
yyvsp_offset; if( _temp5660 < _temp5658.base? 1: _temp5660 >= _temp5658.last_plus_one){
_throw( Null_Exception);}* _temp5660;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5661= yyls; struct Cyc_Yyltype* _temp5663= _temp5661.curr +(
yylsp_offset - 2); if( _temp5663 < _temp5661.base? 1: _temp5663 >= _temp5661.last_plus_one){
_throw( Null_Exception);}* _temp5663;}).first_line,({ struct _tagged_ptr3
_temp5664= yyls; struct Cyc_Yyltype* _temp5666= _temp5664.curr + yylsp_offset;
if( _temp5666 < _temp5664.base? 1: _temp5666 >= _temp5664.last_plus_one){ _throw(
Null_Exception);}* _temp5666;}).last_line));( struct _xenum_struct*) _temp5654;});
break; case 310: _LL5653: yyval=({ struct _tagged_ptr2 _temp5668= yyvs; struct
_xenum_struct** _temp5670= _temp5668.curr + yyvsp_offset; if( _temp5670 <
_temp5668.base? 1: _temp5670 >= _temp5668.last_plus_one){ _throw( Null_Exception);}*
_temp5670;}); break; case 311: _LL5667: yyval=({ struct Cyc_Exp_tok_struct*
_temp5672=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp5672->tag= Cyc_Exp_tok_tag; _temp5672->f1= Cyc_Absyn_prim2_exp( Cyc_Absyn_Bitlshift,
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5673= yyvs; struct _xenum_struct**
_temp5675= _temp5673.curr +( yyvsp_offset - 2); if( _temp5675 < _temp5673.base?
1: _temp5675 >= _temp5673.last_plus_one){ _throw( Null_Exception);}* _temp5675;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5676= yyvs; struct _xenum_struct**
_temp5678= _temp5676.curr + yyvsp_offset; if( _temp5678 < _temp5676.base? 1:
_temp5678 >= _temp5676.last_plus_one){ _throw( Null_Exception);}* _temp5678;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5679= yyls; struct Cyc_Yyltype*
_temp5681= _temp5679.curr +( yylsp_offset - 2); if( _temp5681 < _temp5679.base?
1: _temp5681 >= _temp5679.last_plus_one){ _throw( Null_Exception);}* _temp5681;}).first_line,({
struct _tagged_ptr3 _temp5682= yyls; struct Cyc_Yyltype* _temp5684= _temp5682.curr
+ yylsp_offset; if( _temp5684 < _temp5682.base? 1: _temp5684 >= _temp5682.last_plus_one){
_throw( Null_Exception);}* _temp5684;}).last_line));( struct _xenum_struct*)
_temp5672;}); break; case 312: _LL5671: yyval=({ struct Cyc_Exp_tok_struct*
_temp5686=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp5686->tag= Cyc_Exp_tok_tag; _temp5686->f1= Cyc_Absyn_prim2_exp( Cyc_Absyn_Bitlrshift,
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5687= yyvs; struct _xenum_struct**
_temp5689= _temp5687.curr +( yyvsp_offset - 2); if( _temp5689 < _temp5687.base?
1: _temp5689 >= _temp5687.last_plus_one){ _throw( Null_Exception);}* _temp5689;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5690= yyvs; struct _xenum_struct**
_temp5692= _temp5690.curr + yyvsp_offset; if( _temp5692 < _temp5690.base? 1:
_temp5692 >= _temp5690.last_plus_one){ _throw( Null_Exception);}* _temp5692;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5693= yyls; struct Cyc_Yyltype*
_temp5695= _temp5693.curr +( yylsp_offset - 2); if( _temp5695 < _temp5693.base?
1: _temp5695 >= _temp5693.last_plus_one){ _throw( Null_Exception);}* _temp5695;}).first_line,({
struct _tagged_ptr3 _temp5696= yyls; struct Cyc_Yyltype* _temp5698= _temp5696.curr
+ yylsp_offset; if( _temp5698 < _temp5696.base? 1: _temp5698 >= _temp5696.last_plus_one){
_throw( Null_Exception);}* _temp5698;}).last_line));( struct _xenum_struct*)
_temp5686;}); break; case 313: _LL5685: yyval=({ struct _tagged_ptr2 _temp5700=
yyvs; struct _xenum_struct** _temp5702= _temp5700.curr + yyvsp_offset; if(
_temp5702 < _temp5700.base? 1: _temp5702 >= _temp5700.last_plus_one){ _throw(
Null_Exception);}* _temp5702;}); break; case 314: _LL5699: yyval=({ struct Cyc_Exp_tok_struct*
_temp5704=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp5704->tag= Cyc_Exp_tok_tag; _temp5704->f1= Cyc_Absyn_prim2_exp( Cyc_Absyn_Plus,
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5705= yyvs; struct _xenum_struct**
_temp5707= _temp5705.curr +( yyvsp_offset - 2); if( _temp5707 < _temp5705.base?
1: _temp5707 >= _temp5705.last_plus_one){ _throw( Null_Exception);}* _temp5707;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5708= yyvs; struct _xenum_struct**
_temp5710= _temp5708.curr + yyvsp_offset; if( _temp5710 < _temp5708.base? 1:
_temp5710 >= _temp5708.last_plus_one){ _throw( Null_Exception);}* _temp5710;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5711= yyls; struct Cyc_Yyltype*
_temp5713= _temp5711.curr +( yylsp_offset - 2); if( _temp5713 < _temp5711.base?
1: _temp5713 >= _temp5711.last_plus_one){ _throw( Null_Exception);}* _temp5713;}).first_line,({
struct _tagged_ptr3 _temp5714= yyls; struct Cyc_Yyltype* _temp5716= _temp5714.curr
+ yylsp_offset; if( _temp5716 < _temp5714.base? 1: _temp5716 >= _temp5714.last_plus_one){
_throw( Null_Exception);}* _temp5716;}).last_line));( struct _xenum_struct*)
_temp5704;}); break; case 315: _LL5703: yyval=({ struct Cyc_Exp_tok_struct*
_temp5718=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp5718->tag= Cyc_Exp_tok_tag; _temp5718->f1= Cyc_Absyn_prim2_exp( Cyc_Absyn_Minus,
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5719= yyvs; struct _xenum_struct**
_temp5721= _temp5719.curr +( yyvsp_offset - 2); if( _temp5721 < _temp5719.base?
1: _temp5721 >= _temp5719.last_plus_one){ _throw( Null_Exception);}* _temp5721;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5722= yyvs; struct _xenum_struct**
_temp5724= _temp5722.curr + yyvsp_offset; if( _temp5724 < _temp5722.base? 1:
_temp5724 >= _temp5722.last_plus_one){ _throw( Null_Exception);}* _temp5724;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5725= yyls; struct Cyc_Yyltype*
_temp5727= _temp5725.curr +( yylsp_offset - 2); if( _temp5727 < _temp5725.base?
1: _temp5727 >= _temp5725.last_plus_one){ _throw( Null_Exception);}* _temp5727;}).first_line,({
struct _tagged_ptr3 _temp5728= yyls; struct Cyc_Yyltype* _temp5730= _temp5728.curr
+ yylsp_offset; if( _temp5730 < _temp5728.base? 1: _temp5730 >= _temp5728.last_plus_one){
_throw( Null_Exception);}* _temp5730;}).last_line));( struct _xenum_struct*)
_temp5718;}); break; case 316: _LL5717: yyval=({ struct _tagged_ptr2 _temp5732=
yyvs; struct _xenum_struct** _temp5734= _temp5732.curr + yyvsp_offset; if(
_temp5734 < _temp5732.base? 1: _temp5734 >= _temp5732.last_plus_one){ _throw(
Null_Exception);}* _temp5734;}); break; case 317: _LL5731: yyval=({ struct Cyc_Exp_tok_struct*
_temp5736=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp5736->tag= Cyc_Exp_tok_tag; _temp5736->f1= Cyc_Absyn_prim2_exp( Cyc_Absyn_Times,
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5737= yyvs; struct _xenum_struct**
_temp5739= _temp5737.curr +( yyvsp_offset - 2); if( _temp5739 < _temp5737.base?
1: _temp5739 >= _temp5737.last_plus_one){ _throw( Null_Exception);}* _temp5739;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5740= yyvs; struct _xenum_struct**
_temp5742= _temp5740.curr + yyvsp_offset; if( _temp5742 < _temp5740.base? 1:
_temp5742 >= _temp5740.last_plus_one){ _throw( Null_Exception);}* _temp5742;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5743= yyls; struct Cyc_Yyltype*
_temp5745= _temp5743.curr +( yylsp_offset - 2); if( _temp5745 < _temp5743.base?
1: _temp5745 >= _temp5743.last_plus_one){ _throw( Null_Exception);}* _temp5745;}).first_line,({
struct _tagged_ptr3 _temp5746= yyls; struct Cyc_Yyltype* _temp5748= _temp5746.curr
+ yylsp_offset; if( _temp5748 < _temp5746.base? 1: _temp5748 >= _temp5746.last_plus_one){
_throw( Null_Exception);}* _temp5748;}).last_line));( struct _xenum_struct*)
_temp5736;}); break; case 318: _LL5735: yyval=({ struct Cyc_Exp_tok_struct*
_temp5750=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp5750->tag= Cyc_Exp_tok_tag; _temp5750->f1= Cyc_Absyn_prim2_exp( Cyc_Absyn_Div,
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5751= yyvs; struct _xenum_struct**
_temp5753= _temp5751.curr +( yyvsp_offset - 2); if( _temp5753 < _temp5751.base?
1: _temp5753 >= _temp5751.last_plus_one){ _throw( Null_Exception);}* _temp5753;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5754= yyvs; struct _xenum_struct**
_temp5756= _temp5754.curr + yyvsp_offset; if( _temp5756 < _temp5754.base? 1:
_temp5756 >= _temp5754.last_plus_one){ _throw( Null_Exception);}* _temp5756;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5757= yyls; struct Cyc_Yyltype*
_temp5759= _temp5757.curr +( yylsp_offset - 2); if( _temp5759 < _temp5757.base?
1: _temp5759 >= _temp5757.last_plus_one){ _throw( Null_Exception);}* _temp5759;}).first_line,({
struct _tagged_ptr3 _temp5760= yyls; struct Cyc_Yyltype* _temp5762= _temp5760.curr
+ yylsp_offset; if( _temp5762 < _temp5760.base? 1: _temp5762 >= _temp5760.last_plus_one){
_throw( Null_Exception);}* _temp5762;}).last_line));( struct _xenum_struct*)
_temp5750;}); break; case 319: _LL5749: yyval=({ struct Cyc_Exp_tok_struct*
_temp5764=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp5764->tag= Cyc_Exp_tok_tag; _temp5764->f1= Cyc_Absyn_prim2_exp( Cyc_Absyn_Mod,
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5765= yyvs; struct _xenum_struct**
_temp5767= _temp5765.curr +( yyvsp_offset - 2); if( _temp5767 < _temp5765.base?
1: _temp5767 >= _temp5765.last_plus_one){ _throw( Null_Exception);}* _temp5767;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5768= yyvs; struct _xenum_struct**
_temp5770= _temp5768.curr + yyvsp_offset; if( _temp5770 < _temp5768.base? 1:
_temp5770 >= _temp5768.last_plus_one){ _throw( Null_Exception);}* _temp5770;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5771= yyls; struct Cyc_Yyltype*
_temp5773= _temp5771.curr +( yylsp_offset - 2); if( _temp5773 < _temp5771.base?
1: _temp5773 >= _temp5771.last_plus_one){ _throw( Null_Exception);}* _temp5773;}).first_line,({
struct _tagged_ptr3 _temp5774= yyls; struct Cyc_Yyltype* _temp5776= _temp5774.curr
+ yylsp_offset; if( _temp5776 < _temp5774.base? 1: _temp5776 >= _temp5774.last_plus_one){
_throw( Null_Exception);}* _temp5776;}).last_line));( struct _xenum_struct*)
_temp5764;}); break; case 320: _LL5763: yyval=({ struct _tagged_ptr2 _temp5778=
yyvs; struct _xenum_struct** _temp5780= _temp5778.curr + yyvsp_offset; if(
_temp5780 < _temp5778.base? 1: _temp5780 >= _temp5778.last_plus_one){ _throw(
Null_Exception);}* _temp5780;}); break; case 321: _LL5777: yyval=({ struct Cyc_Exp_tok_struct*
_temp5782=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp5782->tag= Cyc_Exp_tok_tag; _temp5782->f1= Cyc_Absyn_cast_exp((* Cyc_yyget_ParamDecl_tok(({
struct _tagged_ptr2 _temp5783= yyvs; struct _xenum_struct** _temp5785= _temp5783.curr
+( yyvsp_offset - 2); if( _temp5785 < _temp5783.base? 1: _temp5785 >= _temp5783.last_plus_one){
_throw( Null_Exception);}* _temp5785;}))).f3, Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5786= yyvs; struct _xenum_struct** _temp5788= _temp5786.curr +
yyvsp_offset; if( _temp5788 < _temp5786.base? 1: _temp5788 >= _temp5786.last_plus_one){
_throw( Null_Exception);}* _temp5788;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5789= yyls; struct Cyc_Yyltype* _temp5791= _temp5789.curr +(
yylsp_offset - 3); if( _temp5791 < _temp5789.base? 1: _temp5791 >= _temp5789.last_plus_one){
_throw( Null_Exception);}* _temp5791;}).first_line,({ struct _tagged_ptr3
_temp5792= yyls; struct Cyc_Yyltype* _temp5794= _temp5792.curr + yylsp_offset;
if( _temp5794 < _temp5792.base? 1: _temp5794 >= _temp5792.last_plus_one){ _throw(
Null_Exception);}* _temp5794;}).last_line));( struct _xenum_struct*) _temp5782;});
break; case 322: _LL5781: yyval=({ struct _tagged_ptr2 _temp5796= yyvs; struct
_xenum_struct** _temp5798= _temp5796.curr + yyvsp_offset; if( _temp5798 <
_temp5796.base? 1: _temp5798 >= _temp5796.last_plus_one){ _throw( Null_Exception);}*
_temp5798;}); break; case 323: _LL5795: yyval=({ struct Cyc_Exp_tok_struct*
_temp5800=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp5800->tag= Cyc_Exp_tok_tag; _temp5800->f1= Cyc_Absyn_pre_inc_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5801= yyvs; struct _xenum_struct** _temp5803= _temp5801.curr
+ yyvsp_offset; if( _temp5803 < _temp5801.base? 1: _temp5803 >= _temp5801.last_plus_one){
_throw( Null_Exception);}* _temp5803;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5804= yyls; struct Cyc_Yyltype* _temp5806= _temp5804.curr +(
yylsp_offset - 1); if( _temp5806 < _temp5804.base? 1: _temp5806 >= _temp5804.last_plus_one){
_throw( Null_Exception);}* _temp5806;}).first_line,({ struct _tagged_ptr3
_temp5807= yyls; struct Cyc_Yyltype* _temp5809= _temp5807.curr + yylsp_offset;
if( _temp5809 < _temp5807.base? 1: _temp5809 >= _temp5807.last_plus_one){ _throw(
Null_Exception);}* _temp5809;}).last_line));( struct _xenum_struct*) _temp5800;});
break; case 324: _LL5799: yyval=({ struct Cyc_Exp_tok_struct* _temp5811=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp5811->tag=
Cyc_Exp_tok_tag; _temp5811->f1= Cyc_Absyn_pre_dec_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5812= yyvs; struct _xenum_struct** _temp5814= _temp5812.curr
+ yyvsp_offset; if( _temp5814 < _temp5812.base? 1: _temp5814 >= _temp5812.last_plus_one){
_throw( Null_Exception);}* _temp5814;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5815= yyls; struct Cyc_Yyltype* _temp5817= _temp5815.curr +(
yylsp_offset - 1); if( _temp5817 < _temp5815.base? 1: _temp5817 >= _temp5815.last_plus_one){
_throw( Null_Exception);}* _temp5817;}).first_line,({ struct _tagged_ptr3
_temp5818= yyls; struct Cyc_Yyltype* _temp5820= _temp5818.curr + yylsp_offset;
if( _temp5820 < _temp5818.base? 1: _temp5820 >= _temp5818.last_plus_one){ _throw(
Null_Exception);}* _temp5820;}).last_line));( struct _xenum_struct*) _temp5811;});
break; case 325: _LL5810: yyval=({ struct Cyc_Exp_tok_struct* _temp5822=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp5822->tag=
Cyc_Exp_tok_tag; _temp5822->f1= Cyc_Absyn_address_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5823= yyvs; struct _xenum_struct** _temp5825= _temp5823.curr
+ yyvsp_offset; if( _temp5825 < _temp5823.base? 1: _temp5825 >= _temp5823.last_plus_one){
_throw( Null_Exception);}* _temp5825;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5826= yyls; struct Cyc_Yyltype* _temp5828= _temp5826.curr +(
yylsp_offset - 1); if( _temp5828 < _temp5826.base? 1: _temp5828 >= _temp5826.last_plus_one){
_throw( Null_Exception);}* _temp5828;}).first_line,({ struct _tagged_ptr3
_temp5829= yyls; struct Cyc_Yyltype* _temp5831= _temp5829.curr + yylsp_offset;
if( _temp5831 < _temp5829.base? 1: _temp5831 >= _temp5829.last_plus_one){ _throw(
Null_Exception);}* _temp5831;}).last_line));( struct _xenum_struct*) _temp5822;});
break; case 326: _LL5821: yyval=({ struct Cyc_Exp_tok_struct* _temp5833=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp5833->tag=
Cyc_Exp_tok_tag; _temp5833->f1= Cyc_Absyn_deref_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5834= yyvs; struct _xenum_struct** _temp5836= _temp5834.curr +
yyvsp_offset; if( _temp5836 < _temp5834.base? 1: _temp5836 >= _temp5834.last_plus_one){
_throw( Null_Exception);}* _temp5836;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5837= yyls; struct Cyc_Yyltype* _temp5839= _temp5837.curr +(
yylsp_offset - 1); if( _temp5839 < _temp5837.base? 1: _temp5839 >= _temp5837.last_plus_one){
_throw( Null_Exception);}* _temp5839;}).first_line,({ struct _tagged_ptr3
_temp5840= yyls; struct Cyc_Yyltype* _temp5842= _temp5840.curr + yylsp_offset;
if( _temp5842 < _temp5840.base? 1: _temp5842 >= _temp5840.last_plus_one){ _throw(
Null_Exception);}* _temp5842;}).last_line));( struct _xenum_struct*) _temp5833;});
break; case 327: _LL5832: yyval=({ struct _tagged_ptr2 _temp5844= yyvs; struct
_xenum_struct** _temp5846= _temp5844.curr + yyvsp_offset; if( _temp5846 <
_temp5844.base? 1: _temp5846 >= _temp5844.last_plus_one){ _throw( Null_Exception);}*
_temp5846;}); break; case 328: _LL5843: yyval=({ struct Cyc_Exp_tok_struct*
_temp5848=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp5848->tag= Cyc_Exp_tok_tag; _temp5848->f1= Cyc_Absyn_prim1_exp( Cyc_yyget_Primop_tok(({
struct _tagged_ptr2 _temp5849= yyvs; struct _xenum_struct** _temp5851= _temp5849.curr
+( yyvsp_offset - 1); if( _temp5851 < _temp5849.base? 1: _temp5851 >= _temp5849.last_plus_one){
_throw( Null_Exception);}* _temp5851;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5852= yyvs; struct _xenum_struct** _temp5854= _temp5852.curr +
yyvsp_offset; if( _temp5854 < _temp5852.base? 1: _temp5854 >= _temp5852.last_plus_one){
_throw( Null_Exception);}* _temp5854;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5855= yyls; struct Cyc_Yyltype* _temp5857= _temp5855.curr +(
yylsp_offset - 1); if( _temp5857 < _temp5855.base? 1: _temp5857 >= _temp5855.last_plus_one){
_throw( Null_Exception);}* _temp5857;}).first_line,({ struct _tagged_ptr3
_temp5858= yyls; struct Cyc_Yyltype* _temp5860= _temp5858.curr + yylsp_offset;
if( _temp5860 < _temp5858.base? 1: _temp5860 >= _temp5858.last_plus_one){ _throw(
Null_Exception);}* _temp5860;}).last_line));( struct _xenum_struct*) _temp5848;});
break; case 329: _LL5847: yyval=({ struct Cyc_Exp_tok_struct* _temp5862=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp5862->tag=
Cyc_Exp_tok_tag; _temp5862->f1= Cyc_Absyn_sizeoftyp_exp((* Cyc_yyget_ParamDecl_tok(({
struct _tagged_ptr2 _temp5863= yyvs; struct _xenum_struct** _temp5865= _temp5863.curr
+( yyvsp_offset - 1); if( _temp5865 < _temp5863.base? 1: _temp5865 >= _temp5863.last_plus_one){
_throw( Null_Exception);}* _temp5865;}))).f3, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5866= yyls; struct Cyc_Yyltype* _temp5868= _temp5866.curr
+( yylsp_offset - 3); if( _temp5868 < _temp5866.base? 1: _temp5868 >= _temp5866.last_plus_one){
_throw( Null_Exception);}* _temp5868;}).first_line,({ struct _tagged_ptr3
_temp5869= yyls; struct Cyc_Yyltype* _temp5871= _temp5869.curr + yylsp_offset;
if( _temp5871 < _temp5869.base? 1: _temp5871 >= _temp5869.last_plus_one){ _throw(
Null_Exception);}* _temp5871;}).last_line));( struct _xenum_struct*) _temp5862;});
break; case 330: _LL5861: yyval=({ struct Cyc_Exp_tok_struct* _temp5873=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp5873->tag=
Cyc_Exp_tok_tag; _temp5873->f1= Cyc_Absyn_sizeofexp_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5874= yyvs; struct _xenum_struct** _temp5876= _temp5874.curr
+ yyvsp_offset; if( _temp5876 < _temp5874.base? 1: _temp5876 >= _temp5874.last_plus_one){
_throw( Null_Exception);}* _temp5876;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5877= yyls; struct Cyc_Yyltype* _temp5879= _temp5877.curr +(
yylsp_offset - 1); if( _temp5879 < _temp5877.base? 1: _temp5879 >= _temp5877.last_plus_one){
_throw( Null_Exception);}* _temp5879;}).first_line,({ struct _tagged_ptr3
_temp5880= yyls; struct Cyc_Yyltype* _temp5882= _temp5880.curr + yylsp_offset;
if( _temp5882 < _temp5880.base? 1: _temp5882 >= _temp5880.last_plus_one){ _throw(
Null_Exception);}* _temp5882;}).last_line));( struct _xenum_struct*) _temp5873;});
break; case 331: _LL5872: yyval=({ struct Cyc_Exp_tok_struct* _temp5884=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp5884->tag=
Cyc_Exp_tok_tag; _temp5884->f1= Cyc_Absyn_throw_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5885= yyvs; struct _xenum_struct** _temp5887= _temp5885.curr +
yyvsp_offset; if( _temp5887 < _temp5885.base? 1: _temp5887 >= _temp5885.last_plus_one){
_throw( Null_Exception);}* _temp5887;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5888= yyls; struct Cyc_Yyltype* _temp5890= _temp5888.curr +(
yylsp_offset - 1); if( _temp5890 < _temp5888.base? 1: _temp5890 >= _temp5888.last_plus_one){
_throw( Null_Exception);}* _temp5890;}).first_line,({ struct _tagged_ptr3
_temp5891= yyls; struct Cyc_Yyltype* _temp5893= _temp5891.curr + yylsp_offset;
if( _temp5893 < _temp5891.base? 1: _temp5893 >= _temp5891.last_plus_one){ _throw(
Null_Exception);}* _temp5893;}).last_line));( struct _xenum_struct*) _temp5884;});
break; case 332: _LL5883: yyval=({ struct Cyc_Exp_tok_struct* _temp5895=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp5895->tag=
Cyc_Exp_tok_tag; _temp5895->f1= Cyc_Absyn_primop_exp( Cyc_yyget_Primop_tok(({
struct _tagged_ptr2 _temp5896= yyvs; struct _xenum_struct** _temp5898= _temp5896.curr
+( yyvsp_offset - 3); if( _temp5898 < _temp5896.base? 1: _temp5898 >= _temp5896.last_plus_one){
_throw( Null_Exception);}* _temp5898;})), Cyc_yyget_ExpList_tok(({ struct
_tagged_ptr2 _temp5899= yyvs; struct _xenum_struct** _temp5901= _temp5899.curr +(
yyvsp_offset - 1); if( _temp5901 < _temp5899.base? 1: _temp5901 >= _temp5899.last_plus_one){
_throw( Null_Exception);}* _temp5901;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5902= yyls; struct Cyc_Yyltype* _temp5904= _temp5902.curr +(
yylsp_offset - 3); if( _temp5904 < _temp5902.base? 1: _temp5904 >= _temp5902.last_plus_one){
_throw( Null_Exception);}* _temp5904;}).first_line,({ struct _tagged_ptr3
_temp5905= yyls; struct Cyc_Yyltype* _temp5907= _temp5905.curr + yylsp_offset;
if( _temp5907 < _temp5905.base? 1: _temp5907 >= _temp5905.last_plus_one){ _throw(
Null_Exception);}* _temp5907;}).last_line));( struct _xenum_struct*) _temp5895;});
break; case 333: _LL5894: yyval=({ struct Cyc_Exp_tok_struct* _temp5909=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp5909->tag=
Cyc_Exp_tok_tag; _temp5909->f1= Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Malloc_e_struct*
_temp5910=( struct Cyc_Absyn_Malloc_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp5910->tag= Cyc_Absyn_Malloc_e_tag; _temp5910->f1=( void*)({ struct Cyc_Absyn_Typ_m_struct*
_temp5911=( struct Cyc_Absyn_Typ_m_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Typ_m_struct));
_temp5911->tag= Cyc_Absyn_Typ_m_tag; _temp5911->f1=( void*) Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp5912= yyvs; struct
_xenum_struct** _temp5914= _temp5912.curr +( yyvsp_offset - 2); if( _temp5914 <
_temp5912.base? 1: _temp5914 >= _temp5912.last_plus_one){ _throw( Null_Exception);}*
_temp5914;}))).f2, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5915=
yyls; struct Cyc_Yyltype* _temp5917= _temp5915.curr +( yylsp_offset - 2); if(
_temp5917 < _temp5915.base? 1: _temp5917 >= _temp5915.last_plus_one){ _throw(
Null_Exception);}* _temp5917;}).first_line,({ struct _tagged_ptr3 _temp5918=
yyls; struct Cyc_Yyltype* _temp5920= _temp5918.curr +( yylsp_offset - 2); if(
_temp5920 < _temp5918.base? 1: _temp5920 >= _temp5918.last_plus_one){ _throw(
Null_Exception);}* _temp5920;}).last_line));( void*) _temp5911;});( void*)
_temp5910;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5921= yyls;
struct Cyc_Yyltype* _temp5923= _temp5921.curr +( yylsp_offset - 6); if(
_temp5923 < _temp5921.base? 1: _temp5923 >= _temp5921.last_plus_one){ _throw(
Null_Exception);}* _temp5923;}).first_line,({ struct _tagged_ptr3 _temp5924=
yyls; struct Cyc_Yyltype* _temp5926= _temp5924.curr + yylsp_offset; if(
_temp5926 < _temp5924.base? 1: _temp5926 >= _temp5924.last_plus_one){ _throw(
Null_Exception);}* _temp5926;}).last_line));( struct _xenum_struct*) _temp5909;});
break; case 334: _LL5908: yyval=({ struct Cyc_Exp_tok_struct* _temp5928=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp5928->tag=
Cyc_Exp_tok_tag; _temp5928->f1= Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Malloc_e_struct*
_temp5929=( struct Cyc_Absyn_Malloc_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct));
_temp5929->tag= Cyc_Absyn_Malloc_e_tag; _temp5929->f1=( void*)({ struct Cyc_Absyn_Unresolved_m_struct*
_temp5930=( struct Cyc_Absyn_Unresolved_m_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Unresolved_m_struct));
_temp5930->tag= Cyc_Absyn_Unresolved_m_tag; _temp5930->f1= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp5931= yyvs; struct _xenum_struct** _temp5933= _temp5931.curr
+( yyvsp_offset - 2); if( _temp5933 < _temp5931.base? 1: _temp5933 >= _temp5931.last_plus_one){
_throw( Null_Exception);}* _temp5933;}));( void*) _temp5930;});( void*)
_temp5929;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5934= yyls;
struct Cyc_Yyltype* _temp5936= _temp5934.curr +( yylsp_offset - 6); if(
_temp5936 < _temp5934.base? 1: _temp5936 >= _temp5934.last_plus_one){ _throw(
Null_Exception);}* _temp5936;}).first_line,({ struct _tagged_ptr3 _temp5937=
yyls; struct Cyc_Yyltype* _temp5939= _temp5937.curr + yylsp_offset; if(
_temp5939 < _temp5937.base? 1: _temp5939 >= _temp5937.last_plus_one){ _throw(
Null_Exception);}* _temp5939;}).last_line));( struct _xenum_struct*) _temp5928;});
break; case 335: _LL5927: yyval=({ struct Cyc_Primop_tok_struct* _temp5941=(
struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp5941->tag= Cyc_Primop_tok_tag; _temp5941->f1=( void*) Cyc_Absyn_Printf;(
struct _xenum_struct*) _temp5941;}); break; case 336: _LL5940: yyval=({ struct
Cyc_Primop_tok_struct* _temp5943=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp5943->tag= Cyc_Primop_tok_tag;
_temp5943->f1=( void*) Cyc_Absyn_Fprintf;( struct _xenum_struct*) _temp5943;});
break; case 337: _LL5942: yyval=({ struct Cyc_Primop_tok_struct* _temp5945=(
struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp5945->tag= Cyc_Primop_tok_tag; _temp5945->f1=( void*) Cyc_Absyn_Xprintf;(
struct _xenum_struct*) _temp5945;}); break; case 338: _LL5944: yyval=({ struct
Cyc_Primop_tok_struct* _temp5947=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp5947->tag= Cyc_Primop_tok_tag;
_temp5947->f1=( void*) Cyc_Absyn_Scanf;( struct _xenum_struct*) _temp5947;});
break; case 339: _LL5946: yyval=({ struct Cyc_Primop_tok_struct* _temp5949=(
struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp5949->tag= Cyc_Primop_tok_tag; _temp5949->f1=( void*) Cyc_Absyn_Fscanf;(
struct _xenum_struct*) _temp5949;}); break; case 340: _LL5948: yyval=({ struct
Cyc_Primop_tok_struct* _temp5951=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp5951->tag= Cyc_Primop_tok_tag;
_temp5951->f1=( void*) Cyc_Absyn_Sscanf;( struct _xenum_struct*) _temp5951;});
break; case 341: _LL5950: yyval=({ struct Cyc_Primop_tok_struct* _temp5953=(
struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp5953->tag= Cyc_Primop_tok_tag; _temp5953->f1=( void*) Cyc_Absyn_Bitnot;(
struct _xenum_struct*) _temp5953;}); break; case 342: _LL5952: yyval=({ struct
Cyc_Primop_tok_struct* _temp5955=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct)); _temp5955->tag= Cyc_Primop_tok_tag;
_temp5955->f1=( void*) Cyc_Absyn_Not;( struct _xenum_struct*) _temp5955;});
break; case 343: _LL5954: yyval=({ struct Cyc_Primop_tok_struct* _temp5957=(
struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));
_temp5957->tag= Cyc_Primop_tok_tag; _temp5957->f1=( void*) Cyc_Absyn_Minus;(
struct _xenum_struct*) _temp5957;}); break; case 344: _LL5956: yyval=({ struct
_tagged_ptr2 _temp5959= yyvs; struct _xenum_struct** _temp5961= _temp5959.curr +
yyvsp_offset; if( _temp5961 < _temp5959.base? 1: _temp5961 >= _temp5959.last_plus_one){
_throw( Null_Exception);}* _temp5961;}); break; case 345: _LL5958: yyval=({
struct Cyc_Exp_tok_struct* _temp5963=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp5963->tag= Cyc_Exp_tok_tag; _temp5963->f1=
Cyc_Absyn_subscript_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5964=
yyvs; struct _xenum_struct** _temp5966= _temp5964.curr +( yyvsp_offset - 3); if(
_temp5966 < _temp5964.base? 1: _temp5966 >= _temp5964.last_plus_one){ _throw(
Null_Exception);}* _temp5966;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5967= yyvs; struct _xenum_struct** _temp5969= _temp5967.curr +(
yyvsp_offset - 1); if( _temp5969 < _temp5967.base? 1: _temp5969 >= _temp5967.last_plus_one){
_throw( Null_Exception);}* _temp5969;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5970= yyls; struct Cyc_Yyltype* _temp5972= _temp5970.curr +(
yylsp_offset - 3); if( _temp5972 < _temp5970.base? 1: _temp5972 >= _temp5970.last_plus_one){
_throw( Null_Exception);}* _temp5972;}).first_line,({ struct _tagged_ptr3
_temp5973= yyls; struct Cyc_Yyltype* _temp5975= _temp5973.curr + yylsp_offset;
if( _temp5975 < _temp5973.base? 1: _temp5975 >= _temp5973.last_plus_one){ _throw(
Null_Exception);}* _temp5975;}).last_line));( struct _xenum_struct*) _temp5963;});
break; case 346: _LL5962: yyval=({ struct Cyc_Exp_tok_struct* _temp5977=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp5977->tag=
Cyc_Exp_tok_tag; _temp5977->f1= Cyc_Absyn_unknowncall_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5978= yyvs; struct _xenum_struct** _temp5980= _temp5978.curr
+( yyvsp_offset - 2); if( _temp5980 < _temp5978.base? 1: _temp5980 >= _temp5978.last_plus_one){
_throw( Null_Exception);}* _temp5980;})), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5981= yyls; struct Cyc_Yyltype* _temp5983= _temp5981.curr
+( yylsp_offset - 2); if( _temp5983 < _temp5981.base? 1: _temp5983 >= _temp5981.last_plus_one){
_throw( Null_Exception);}* _temp5983;}).first_line,({ struct _tagged_ptr3
_temp5984= yyls; struct Cyc_Yyltype* _temp5986= _temp5984.curr + yylsp_offset;
if( _temp5986 < _temp5984.base? 1: _temp5986 >= _temp5984.last_plus_one){ _throw(
Null_Exception);}* _temp5986;}).last_line));( struct _xenum_struct*) _temp5977;});
break; case 347: _LL5976: yyval=({ struct Cyc_Exp_tok_struct* _temp5988=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp5988->tag=
Cyc_Exp_tok_tag; _temp5988->f1= Cyc_Absyn_unknowncall_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp5989= yyvs; struct _xenum_struct** _temp5991= _temp5989.curr
+( yyvsp_offset - 3); if( _temp5991 < _temp5989.base? 1: _temp5991 >= _temp5989.last_plus_one){
_throw( Null_Exception);}* _temp5991;})), Cyc_yyget_ExpList_tok(({ struct
_tagged_ptr2 _temp5992= yyvs; struct _xenum_struct** _temp5994= _temp5992.curr +(
yyvsp_offset - 1); if( _temp5994 < _temp5992.base? 1: _temp5994 >= _temp5992.last_plus_one){
_throw( Null_Exception);}* _temp5994;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5995= yyls; struct Cyc_Yyltype* _temp5997= _temp5995.curr +(
yylsp_offset - 3); if( _temp5997 < _temp5995.base? 1: _temp5997 >= _temp5995.last_plus_one){
_throw( Null_Exception);}* _temp5997;}).first_line,({ struct _tagged_ptr3
_temp5998= yyls; struct Cyc_Yyltype* _temp6000= _temp5998.curr + yylsp_offset;
if( _temp6000 < _temp5998.base? 1: _temp6000 >= _temp5998.last_plus_one){ _throw(
Null_Exception);}* _temp6000;}).last_line));( struct _xenum_struct*) _temp5988;});
break; case 348: _LL5987: yyval=({ struct Cyc_Exp_tok_struct* _temp6002=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6002->tag=
Cyc_Exp_tok_tag; _temp6002->f1= Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6003= yyvs; struct _xenum_struct** _temp6005= _temp6003.curr
+( yyvsp_offset - 2); if( _temp6005 < _temp6003.base? 1: _temp6005 >= _temp6003.last_plus_one){
_throw( Null_Exception);}* _temp6005;})),({ struct _tagged_string* _temp6006=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp6006[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp6007= yyvs;
struct _xenum_struct** _temp6009= _temp6007.curr + yyvsp_offset; if( _temp6009 <
_temp6007.base? 1: _temp6009 >= _temp6007.last_plus_one){ _throw( Null_Exception);}*
_temp6009;})); _temp6006;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp6010= yyls; struct Cyc_Yyltype* _temp6012= _temp6010.curr +( yylsp_offset -
2); if( _temp6012 < _temp6010.base? 1: _temp6012 >= _temp6010.last_plus_one){
_throw( Null_Exception);}* _temp6012;}).first_line,({ struct _tagged_ptr3
_temp6013= yyls; struct Cyc_Yyltype* _temp6015= _temp6013.curr + yylsp_offset;
if( _temp6015 < _temp6013.base? 1: _temp6015 >= _temp6013.last_plus_one){ _throw(
Null_Exception);}* _temp6015;}).last_line));( struct _xenum_struct*) _temp6002;});
break; case 349: _LL6001: { struct _tuple1* q= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp6017= yyvs; struct _xenum_struct** _temp6019= _temp6017.curr +
yyvsp_offset; if( _temp6019 < _temp6017.base? 1: _temp6019 >= _temp6017.last_plus_one){
_throw( Null_Exception);}* _temp6019;}));{ void* _temp6020=(* q).f1; struct Cyc_List_List*
_temp6030; struct Cyc_List_List* _temp6032; _LL6022: if( _temp6020 == Cyc_Absyn_Loc_n){
goto _LL6023;} else{ goto _LL6024;} _LL6024: if(( unsigned int) _temp6020 > 1u?((
struct _enum_struct*) _temp6020)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL6031:
_temp6030=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp6020)->f1;
if( _temp6030 == 0){ goto _LL6025;} else{ goto _LL6026;}} else{ goto _LL6026;}
_LL6026: if(( unsigned int) _temp6020 > 1u?(( struct _enum_struct*) _temp6020)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL6033: _temp6032=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp6020)->f1; if( _temp6032 == 0){ goto _LL6027;}
else{ goto _LL6028;}} else{ goto _LL6028;} _LL6028: goto _LL6029; _LL6023: goto
_LL6021; _LL6025: goto _LL6021; _LL6027: goto _LL6021; _LL6029: Cyc_Parse_err((
struct _tagged_string)({ char* _temp6034=( char*)"struct field name is qualified";
struct _tagged_string _temp6035; _temp6035.curr= _temp6034; _temp6035.base=
_temp6034; _temp6035.last_plus_one= _temp6034 + 31; _temp6035;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6036= yyls; struct Cyc_Yyltype* _temp6038= _temp6036.curr
+ yylsp_offset; if( _temp6038 < _temp6036.base? 1: _temp6038 >= _temp6036.last_plus_one){
_throw( Null_Exception);}* _temp6038;}).first_line,({ struct _tagged_ptr3
_temp6039= yyls; struct Cyc_Yyltype* _temp6041= _temp6039.curr + yylsp_offset;
if( _temp6041 < _temp6039.base? 1: _temp6041 >= _temp6039.last_plus_one){ _throw(
Null_Exception);}* _temp6041;}).last_line)); goto _LL6021; _LL6021:;} yyval=({
struct Cyc_Exp_tok_struct* _temp6042=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp6042->tag= Cyc_Exp_tok_tag; _temp6042->f1=
Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6043=
yyvs; struct _xenum_struct** _temp6045= _temp6043.curr +( yyvsp_offset - 2); if(
_temp6045 < _temp6043.base? 1: _temp6045 >= _temp6043.last_plus_one){ _throw(
Null_Exception);}* _temp6045;})),(* q).f2, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6046= yyls; struct Cyc_Yyltype* _temp6048= _temp6046.curr +(
yylsp_offset - 2); if( _temp6048 < _temp6046.base? 1: _temp6048 >= _temp6046.last_plus_one){
_throw( Null_Exception);}* _temp6048;}).first_line,({ struct _tagged_ptr3
_temp6049= yyls; struct Cyc_Yyltype* _temp6051= _temp6049.curr + yylsp_offset;
if( _temp6051 < _temp6049.base? 1: _temp6051 >= _temp6049.last_plus_one){ _throw(
Null_Exception);}* _temp6051;}).last_line));( struct _xenum_struct*) _temp6042;});
break;} case 350: _LL6016: yyval=({ struct Cyc_Exp_tok_struct* _temp6053=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6053->tag= Cyc_Exp_tok_tag; _temp6053->f1= Cyc_Absyn_structarrow_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6054= yyvs; struct _xenum_struct** _temp6056= _temp6054.curr
+( yyvsp_offset - 2); if( _temp6056 < _temp6054.base? 1: _temp6056 >= _temp6054.last_plus_one){
_throw( Null_Exception);}* _temp6056;})),({ struct _tagged_string* _temp6057=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp6057[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp6058= yyvs;
struct _xenum_struct** _temp6060= _temp6058.curr + yyvsp_offset; if( _temp6060 <
_temp6058.base? 1: _temp6060 >= _temp6058.last_plus_one){ _throw( Null_Exception);}*
_temp6060;})); _temp6057;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp6061= yyls; struct Cyc_Yyltype* _temp6063= _temp6061.curr +( yylsp_offset -
2); if( _temp6063 < _temp6061.base? 1: _temp6063 >= _temp6061.last_plus_one){
_throw( Null_Exception);}* _temp6063;}).first_line,({ struct _tagged_ptr3
_temp6064= yyls; struct Cyc_Yyltype* _temp6066= _temp6064.curr + yylsp_offset;
if( _temp6066 < _temp6064.base? 1: _temp6066 >= _temp6064.last_plus_one){ _throw(
Null_Exception);}* _temp6066;}).last_line));( struct _xenum_struct*) _temp6053;});
break; case 351: _LL6052: { struct _tuple1* q= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp6068= yyvs; struct _xenum_struct** _temp6070= _temp6068.curr +
yyvsp_offset; if( _temp6070 < _temp6068.base? 1: _temp6070 >= _temp6068.last_plus_one){
_throw( Null_Exception);}* _temp6070;}));{ void* _temp6071=(* q).f1; struct Cyc_List_List*
_temp6081; struct Cyc_List_List* _temp6083; _LL6073: if( _temp6071 == Cyc_Absyn_Loc_n){
goto _LL6074;} else{ goto _LL6075;} _LL6075: if(( unsigned int) _temp6071 > 1u?((
struct _enum_struct*) _temp6071)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL6082:
_temp6081=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp6071)->f1;
if( _temp6081 == 0){ goto _LL6076;} else{ goto _LL6077;}} else{ goto _LL6077;}
_LL6077: if(( unsigned int) _temp6071 > 1u?(( struct _enum_struct*) _temp6071)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL6084: _temp6083=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp6071)->f1; if( _temp6083 == 0){ goto _LL6078;}
else{ goto _LL6079;}} else{ goto _LL6079;} _LL6079: goto _LL6080; _LL6074: goto
_LL6072; _LL6076: goto _LL6072; _LL6078: goto _LL6072; _LL6080: Cyc_Parse_err((
struct _tagged_string)({ char* _temp6085=( char*)"struct field is qualified with module name";
struct _tagged_string _temp6086; _temp6086.curr= _temp6085; _temp6086.base=
_temp6085; _temp6086.last_plus_one= _temp6085 + 43; _temp6086;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6087= yyls; struct Cyc_Yyltype* _temp6089= _temp6087.curr
+ yylsp_offset; if( _temp6089 < _temp6087.base? 1: _temp6089 >= _temp6087.last_plus_one){
_throw( Null_Exception);}* _temp6089;}).first_line,({ struct _tagged_ptr3
_temp6090= yyls; struct Cyc_Yyltype* _temp6092= _temp6090.curr + yylsp_offset;
if( _temp6092 < _temp6090.base? 1: _temp6092 >= _temp6090.last_plus_one){ _throw(
Null_Exception);}* _temp6092;}).last_line)); goto _LL6072; _LL6072:;} yyval=({
struct Cyc_Exp_tok_struct* _temp6093=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct)); _temp6093->tag= Cyc_Exp_tok_tag; _temp6093->f1=
Cyc_Absyn_structarrow_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6094=
yyvs; struct _xenum_struct** _temp6096= _temp6094.curr +( yyvsp_offset - 2); if(
_temp6096 < _temp6094.base? 1: _temp6096 >= _temp6094.last_plus_one){ _throw(
Null_Exception);}* _temp6096;})),(* q).f2, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6097= yyls; struct Cyc_Yyltype* _temp6099= _temp6097.curr +(
yylsp_offset - 2); if( _temp6099 < _temp6097.base? 1: _temp6099 >= _temp6097.last_plus_one){
_throw( Null_Exception);}* _temp6099;}).first_line,({ struct _tagged_ptr3
_temp6100= yyls; struct Cyc_Yyltype* _temp6102= _temp6100.curr + yylsp_offset;
if( _temp6102 < _temp6100.base? 1: _temp6102 >= _temp6100.last_plus_one){ _throw(
Null_Exception);}* _temp6102;}).last_line));( struct _xenum_struct*) _temp6093;});
break;} case 352: _LL6067: yyval=({ struct Cyc_Exp_tok_struct* _temp6104=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6104->tag= Cyc_Exp_tok_tag; _temp6104->f1= Cyc_Absyn_post_inc_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6105= yyvs; struct _xenum_struct** _temp6107= _temp6105.curr
+( yyvsp_offset - 1); if( _temp6107 < _temp6105.base? 1: _temp6107 >= _temp6105.last_plus_one){
_throw( Null_Exception);}* _temp6107;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6108= yyls; struct Cyc_Yyltype* _temp6110= _temp6108.curr +(
yylsp_offset - 1); if( _temp6110 < _temp6108.base? 1: _temp6110 >= _temp6108.last_plus_one){
_throw( Null_Exception);}* _temp6110;}).first_line,({ struct _tagged_ptr3
_temp6111= yyls; struct Cyc_Yyltype* _temp6113= _temp6111.curr + yylsp_offset;
if( _temp6113 < _temp6111.base? 1: _temp6113 >= _temp6111.last_plus_one){ _throw(
Null_Exception);}* _temp6113;}).last_line));( struct _xenum_struct*) _temp6104;});
break; case 353: _LL6103: yyval=({ struct Cyc_Exp_tok_struct* _temp6115=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6115->tag=
Cyc_Exp_tok_tag; _temp6115->f1= Cyc_Absyn_post_dec_exp( Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6116= yyvs; struct _xenum_struct** _temp6118= _temp6116.curr
+( yyvsp_offset - 1); if( _temp6118 < _temp6116.base? 1: _temp6118 >= _temp6116.last_plus_one){
_throw( Null_Exception);}* _temp6118;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6119= yyls; struct Cyc_Yyltype* _temp6121= _temp6119.curr +(
yylsp_offset - 1); if( _temp6121 < _temp6119.base? 1: _temp6121 >= _temp6119.last_plus_one){
_throw( Null_Exception);}* _temp6121;}).first_line,({ struct _tagged_ptr3
_temp6122= yyls; struct Cyc_Yyltype* _temp6124= _temp6122.curr + yylsp_offset;
if( _temp6124 < _temp6122.base? 1: _temp6124 >= _temp6122.last_plus_one){ _throw(
Null_Exception);}* _temp6124;}).last_line));( struct _xenum_struct*) _temp6115;});
break; case 354: _LL6114: yyval=({ struct Cyc_Exp_tok_struct* _temp6126=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6126->tag=
Cyc_Exp_tok_tag; _temp6126->f1= Cyc_Absyn_new_exp(({ struct Cyc_Absyn_CompoundLit_e_struct*
_temp6127=( struct Cyc_Absyn_CompoundLit_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_CompoundLit_e_struct)); _temp6127->tag= Cyc_Absyn_CompoundLit_e_tag;
_temp6127->f1= Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp6128= yyvs;
struct _xenum_struct** _temp6130= _temp6128.curr +( yyvsp_offset - 4); if(
_temp6130 < _temp6128.base? 1: _temp6130 >= _temp6128.last_plus_one){ _throw(
Null_Exception);}* _temp6130;})); _temp6127->f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp6131= yyvs; struct _xenum_struct** _temp6133= _temp6131.curr
+( yyvsp_offset - 1); if( _temp6133 < _temp6131.base? 1: _temp6133 >= _temp6131.last_plus_one){
_throw( Null_Exception);}* _temp6133;})));( void*) _temp6127;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6134= yyls; struct Cyc_Yyltype* _temp6136= _temp6134.curr
+( yylsp_offset - 5); if( _temp6136 < _temp6134.base? 1: _temp6136 >= _temp6134.last_plus_one){
_throw( Null_Exception);}* _temp6136;}).first_line,({ struct _tagged_ptr3
_temp6137= yyls; struct Cyc_Yyltype* _temp6139= _temp6137.curr + yylsp_offset;
if( _temp6139 < _temp6137.base? 1: _temp6139 >= _temp6137.last_plus_one){ _throw(
Null_Exception);}* _temp6139;}).last_line));( struct _xenum_struct*) _temp6126;});
break; case 355: _LL6125: yyval=({ struct Cyc_Exp_tok_struct* _temp6141=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6141->tag=
Cyc_Exp_tok_tag; _temp6141->f1= Cyc_Absyn_new_exp(({ struct Cyc_Absyn_CompoundLit_e_struct*
_temp6142=( struct Cyc_Absyn_CompoundLit_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_CompoundLit_e_struct)); _temp6142->tag= Cyc_Absyn_CompoundLit_e_tag;
_temp6142->f1= Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp6143= yyvs;
struct _xenum_struct** _temp6145= _temp6143.curr +( yyvsp_offset - 5); if(
_temp6145 < _temp6143.base? 1: _temp6145 >= _temp6143.last_plus_one){ _throw(
Null_Exception);}* _temp6145;})); _temp6142->f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp6146= yyvs; struct _xenum_struct** _temp6148= _temp6146.curr
+( yyvsp_offset - 2); if( _temp6148 < _temp6146.base? 1: _temp6148 >= _temp6146.last_plus_one){
_throw( Null_Exception);}* _temp6148;})));( void*) _temp6142;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6149= yyls; struct Cyc_Yyltype* _temp6151= _temp6149.curr
+( yylsp_offset - 6); if( _temp6151 < _temp6149.base? 1: _temp6151 >= _temp6149.last_plus_one){
_throw( Null_Exception);}* _temp6151;}).first_line,({ struct _tagged_ptr3
_temp6152= yyls; struct Cyc_Yyltype* _temp6154= _temp6152.curr + yylsp_offset;
if( _temp6154 < _temp6152.base? 1: _temp6154 >= _temp6152.last_plus_one){ _throw(
Null_Exception);}* _temp6154;}).last_line));( struct _xenum_struct*) _temp6141;});
break; case 356: _LL6140: yyval=({ struct Cyc_Exp_tok_struct* _temp6156=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6156->tag=
Cyc_Exp_tok_tag; _temp6156->f1= Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Array_e_struct*
_temp6157=( struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp6157->tag= Cyc_Absyn_Array_e_tag; _temp6157->f1= 1; _temp6157->f2= 0;( void*)
_temp6157;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6158= yyls;
struct Cyc_Yyltype* _temp6160= _temp6158.curr +( yylsp_offset - 2); if(
_temp6160 < _temp6158.base? 1: _temp6160 >= _temp6158.last_plus_one){ _throw(
Null_Exception);}* _temp6160;}).first_line,({ struct _tagged_ptr3 _temp6161=
yyls; struct Cyc_Yyltype* _temp6163= _temp6161.curr + yylsp_offset; if(
_temp6163 < _temp6161.base? 1: _temp6163 >= _temp6161.last_plus_one){ _throw(
Null_Exception);}* _temp6163;}).last_line));( struct _xenum_struct*) _temp6156;});
break; case 357: _LL6155: yyval=({ struct Cyc_Exp_tok_struct* _temp6165=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6165->tag=
Cyc_Exp_tok_tag; _temp6165->f1= Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Array_e_struct*
_temp6166=( struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp6166->tag= Cyc_Absyn_Array_e_tag; _temp6166->f1= 1; _temp6166->f2=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp6167= yyvs; struct _xenum_struct** _temp6169= _temp6167.curr
+( yyvsp_offset - 1); if( _temp6169 < _temp6167.base? 1: _temp6169 >= _temp6167.last_plus_one){
_throw( Null_Exception);}* _temp6169;})));( void*) _temp6166;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6170= yyls; struct Cyc_Yyltype* _temp6172= _temp6170.curr
+( yylsp_offset - 3); if( _temp6172 < _temp6170.base? 1: _temp6172 >= _temp6170.last_plus_one){
_throw( Null_Exception);}* _temp6172;}).first_line,({ struct _tagged_ptr3
_temp6173= yyls; struct Cyc_Yyltype* _temp6175= _temp6173.curr +( yylsp_offset -
1); if( _temp6175 < _temp6173.base? 1: _temp6175 >= _temp6173.last_plus_one){
_throw( Null_Exception);}* _temp6175;}).last_line));( struct _xenum_struct*)
_temp6165;}); break; case 358: _LL6164: yyval=({ struct Cyc_Exp_tok_struct*
_temp6177=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6177->tag= Cyc_Exp_tok_tag; _temp6177->f1= Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Array_e_struct*
_temp6178=( struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp6178->tag= Cyc_Absyn_Array_e_tag; _temp6178->f1= 1; _temp6178->f2=(( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp6179= yyvs; struct _xenum_struct** _temp6181= _temp6179.curr
+( yyvsp_offset - 2); if( _temp6181 < _temp6179.base? 1: _temp6181 >= _temp6179.last_plus_one){
_throw( Null_Exception);}* _temp6181;})));( void*) _temp6178;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6182= yyls; struct Cyc_Yyltype* _temp6184= _temp6182.curr
+( yylsp_offset - 4); if( _temp6184 < _temp6182.base? 1: _temp6184 >= _temp6182.last_plus_one){
_throw( Null_Exception);}* _temp6184;}).first_line,({ struct _tagged_ptr3
_temp6185= yyls; struct Cyc_Yyltype* _temp6187= _temp6185.curr +( yylsp_offset -
1); if( _temp6187 < _temp6185.base? 1: _temp6187 >= _temp6185.last_plus_one){
_throw( Null_Exception);}* _temp6187;}).last_line));( struct _xenum_struct*)
_temp6177;}); break; case 359: _LL6176: yyval=({ struct Cyc_Exp_tok_struct*
_temp6189=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6189->tag= Cyc_Exp_tok_tag; _temp6189->f1= Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Comprehension_e_struct*
_temp6190=( struct Cyc_Absyn_Comprehension_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Comprehension_e_struct)); _temp6190->tag= Cyc_Absyn_Comprehension_e_tag;
_temp6190->f1= Cyc_Absyn_new_vardecl(({ struct _tuple1* _temp6191=( struct
_tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp6191->f1= Cyc_Absyn_Loc_n;
_temp6191->f2=({ struct _tagged_string* _temp6192=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp6192[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp6193= yyvs; struct _xenum_struct** _temp6195= _temp6193.curr
+( yyvsp_offset - 5); if( _temp6195 < _temp6193.base? 1: _temp6195 >= _temp6193.last_plus_one){
_throw( Null_Exception);}* _temp6195;})); _temp6192;}); _temp6191;}), Cyc_Absyn_uint_t,(
struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp( 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6196= yyls; struct Cyc_Yyltype* _temp6198= _temp6196.curr
+( yylsp_offset - 5); if( _temp6198 < _temp6196.base? 1: _temp6198 >= _temp6196.last_plus_one){
_throw( Null_Exception);}* _temp6198;}).first_line,({ struct _tagged_ptr3
_temp6199= yyls; struct Cyc_Yyltype* _temp6201= _temp6199.curr +( yylsp_offset -
5); if( _temp6201 < _temp6199.base? 1: _temp6201 >= _temp6199.last_plus_one){
_throw( Null_Exception);}* _temp6201;}).last_line))); _temp6190->f2= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6202= yyvs; struct _xenum_struct** _temp6204= _temp6202.curr
+( yyvsp_offset - 3); if( _temp6204 < _temp6202.base? 1: _temp6204 >= _temp6202.last_plus_one){
_throw( Null_Exception);}* _temp6204;})); _temp6190->f3= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6205= yyvs; struct _xenum_struct** _temp6207= _temp6205.curr
+( yyvsp_offset - 1); if( _temp6207 < _temp6205.base? 1: _temp6207 >= _temp6205.last_plus_one){
_throw( Null_Exception);}* _temp6207;}));( void*) _temp6190;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6208= yyls; struct Cyc_Yyltype* _temp6210= _temp6208.curr
+( yylsp_offset - 8); if( _temp6210 < _temp6208.base? 1: _temp6210 >= _temp6208.last_plus_one){
_throw( Null_Exception);}* _temp6210;}).first_line,({ struct _tagged_ptr3
_temp6211= yyls; struct Cyc_Yyltype* _temp6213= _temp6211.curr + yylsp_offset;
if( _temp6213 < _temp6211.base? 1: _temp6213 >= _temp6211.last_plus_one){ _throw(
Null_Exception);}* _temp6213;}).last_line));( struct _xenum_struct*) _temp6189;});
break; case 360: _LL6188: yyval=({ struct Cyc_Exp_tok_struct* _temp6215=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6215->tag=
Cyc_Exp_tok_tag; _temp6215->f1= Cyc_Absyn_string_exp( 1, Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp6216= yyvs; struct _xenum_struct** _temp6218= _temp6216.curr
+ yyvsp_offset; if( _temp6218 < _temp6216.base? 1: _temp6218 >= _temp6216.last_plus_one){
_throw( Null_Exception);}* _temp6218;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6219= yyls; struct Cyc_Yyltype* _temp6221= _temp6219.curr +(
yylsp_offset - 1); if( _temp6221 < _temp6219.base? 1: _temp6221 >= _temp6219.last_plus_one){
_throw( Null_Exception);}* _temp6221;}).first_line,({ struct _tagged_ptr3
_temp6222= yyls; struct Cyc_Yyltype* _temp6224= _temp6222.curr + yylsp_offset;
if( _temp6224 < _temp6222.base? 1: _temp6224 >= _temp6222.last_plus_one){ _throw(
Null_Exception);}* _temp6224;}).last_line));( struct _xenum_struct*) _temp6215;});
break; case 361: _LL6214: yyval=({ struct Cyc_Exp_tok_struct* _temp6226=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6226->tag=
Cyc_Exp_tok_tag; _temp6226->f1= Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Fill_e_struct*
_temp6227=( struct Cyc_Absyn_Fill_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fill_e_struct));
_temp6227->tag= Cyc_Absyn_Fill_e_tag; _temp6227->f1= Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6228= yyvs; struct _xenum_struct** _temp6230= _temp6228.curr +(
yyvsp_offset - 1); if( _temp6230 < _temp6228.base? 1: _temp6230 >= _temp6228.last_plus_one){
_throw( Null_Exception);}* _temp6230;}));( void*) _temp6227;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6231= yyls; struct Cyc_Yyltype* _temp6233= _temp6231.curr
+( yylsp_offset - 3); if( _temp6233 < _temp6231.base? 1: _temp6233 >= _temp6231.last_plus_one){
_throw( Null_Exception);}* _temp6233;}).first_line,({ struct _tagged_ptr3
_temp6234= yyls; struct Cyc_Yyltype* _temp6236= _temp6234.curr + yylsp_offset;
if( _temp6236 < _temp6234.base? 1: _temp6236 >= _temp6234.last_plus_one){ _throw(
Null_Exception);}* _temp6236;}).last_line));( struct _xenum_struct*) _temp6226;});
break; case 362: _LL6225: yyval=({ struct Cyc_Exp_tok_struct* _temp6238=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6238->tag=
Cyc_Exp_tok_tag; _temp6238->f1= Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Codegen_e_struct*
_temp6239=( struct Cyc_Absyn_Codegen_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Codegen_e_struct));
_temp6239->tag= Cyc_Absyn_Codegen_e_tag; _temp6239->f1= Cyc_yyget_FnDecl_tok(({
struct _tagged_ptr2 _temp6240= yyvs; struct _xenum_struct** _temp6242= _temp6240.curr
+( yyvsp_offset - 1); if( _temp6242 < _temp6240.base? 1: _temp6242 >= _temp6240.last_plus_one){
_throw( Null_Exception);}* _temp6242;}));( void*) _temp6239;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6243= yyls; struct Cyc_Yyltype* _temp6245= _temp6243.curr
+( yylsp_offset - 3); if( _temp6245 < _temp6243.base? 1: _temp6245 >= _temp6243.last_plus_one){
_throw( Null_Exception);}* _temp6245;}).first_line,({ struct _tagged_ptr3
_temp6246= yyls; struct Cyc_Yyltype* _temp6248= _temp6246.curr + yylsp_offset;
if( _temp6248 < _temp6246.base? 1: _temp6248 >= _temp6246.last_plus_one){ _throw(
Null_Exception);}* _temp6248;}).last_line));( struct _xenum_struct*) _temp6238;});
break; case 363: _LL6237: yyval=({ struct Cyc_Exp_tok_struct* _temp6250=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6250->tag=
Cyc_Exp_tok_tag; _temp6250->f1= Cyc_Absyn_new_exp(({ struct Cyc_Absyn_UnknownId_e_struct*
_temp6251=( struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp6251->tag= Cyc_Absyn_UnknownId_e_tag; _temp6251->f1= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp6252= yyvs; struct _xenum_struct** _temp6254= _temp6252.curr
+ yyvsp_offset; if( _temp6254 < _temp6252.base? 1: _temp6254 >= _temp6252.last_plus_one){
_throw( Null_Exception);}* _temp6254;}));( void*) _temp6251;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6255= yyls; struct Cyc_Yyltype* _temp6257= _temp6255.curr
+ yylsp_offset; if( _temp6257 < _temp6255.base? 1: _temp6257 >= _temp6255.last_plus_one){
_throw( Null_Exception);}* _temp6257;}).first_line,({ struct _tagged_ptr3
_temp6258= yyls; struct Cyc_Yyltype* _temp6260= _temp6258.curr + yylsp_offset;
if( _temp6260 < _temp6258.base? 1: _temp6260 >= _temp6258.last_plus_one){ _throw(
Null_Exception);}* _temp6260;}).last_line));( struct _xenum_struct*) _temp6250;});
break; case 364: _LL6249: yyval=({ struct _tagged_ptr2 _temp6262= yyvs; struct
_xenum_struct** _temp6264= _temp6262.curr + yyvsp_offset; if( _temp6264 <
_temp6262.base? 1: _temp6264 >= _temp6262.last_plus_one){ _throw( Null_Exception);}*
_temp6264;}); break; case 365: _LL6261: yyval=({ struct Cyc_Exp_tok_struct*
_temp6266=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6266->tag= Cyc_Exp_tok_tag; _temp6266->f1= Cyc_Absyn_string_exp( 0, Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp6267= yyvs; struct _xenum_struct** _temp6269= _temp6267.curr
+ yyvsp_offset; if( _temp6269 < _temp6267.base? 1: _temp6269 >= _temp6267.last_plus_one){
_throw( Null_Exception);}* _temp6269;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6270= yyls; struct Cyc_Yyltype* _temp6272= _temp6270.curr +
yylsp_offset; if( _temp6272 < _temp6270.base? 1: _temp6272 >= _temp6270.last_plus_one){
_throw( Null_Exception);}* _temp6272;}).first_line,({ struct _tagged_ptr3
_temp6273= yyls; struct Cyc_Yyltype* _temp6275= _temp6273.curr + yylsp_offset;
if( _temp6275 < _temp6273.base? 1: _temp6275 >= _temp6273.last_plus_one){ _throw(
Null_Exception);}* _temp6275;}).last_line));( struct _xenum_struct*) _temp6266;});
break; case 366: _LL6265: yyval=({ struct _tagged_ptr2 _temp6277= yyvs; struct
_xenum_struct** _temp6279= _temp6277.curr +( yyvsp_offset - 1); if( _temp6279 <
_temp6277.base? 1: _temp6279 >= _temp6277.last_plus_one){ _throw( Null_Exception);}*
_temp6279;}); break; case 367: _LL6276: yyval=({ struct Cyc_Exp_tok_struct*
_temp6281=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6281->tag= Cyc_Exp_tok_tag; _temp6281->f1= Cyc_Absyn_noinstantiate_exp( Cyc_Absyn_new_exp(({
struct Cyc_Absyn_UnknownId_e_struct* _temp6282=( struct Cyc_Absyn_UnknownId_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp6282->tag= Cyc_Absyn_UnknownId_e_tag;
_temp6282->f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp6283= yyvs;
struct _xenum_struct** _temp6285= _temp6283.curr +( yyvsp_offset - 1); if(
_temp6285 < _temp6283.base? 1: _temp6285 >= _temp6283.last_plus_one){ _throw(
Null_Exception);}* _temp6285;}));( void*) _temp6282;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6286= yyls; struct Cyc_Yyltype* _temp6288= _temp6286.curr
+( yylsp_offset - 1); if( _temp6288 < _temp6286.base? 1: _temp6288 >= _temp6286.last_plus_one){
_throw( Null_Exception);}* _temp6288;}).first_line,({ struct _tagged_ptr3
_temp6289= yyls; struct Cyc_Yyltype* _temp6291= _temp6289.curr +( yylsp_offset -
1); if( _temp6291 < _temp6289.base? 1: _temp6291 >= _temp6289.last_plus_one){
_throw( Null_Exception);}* _temp6291;}).last_line)), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6292= yyls; struct Cyc_Yyltype* _temp6294= _temp6292.curr
+( yylsp_offset - 1); if( _temp6294 < _temp6292.base? 1: _temp6294 >= _temp6292.last_plus_one){
_throw( Null_Exception);}* _temp6294;}).first_line,({ struct _tagged_ptr3
_temp6295= yyls; struct Cyc_Yyltype* _temp6297= _temp6295.curr + yylsp_offset;
if( _temp6297 < _temp6295.base? 1: _temp6297 >= _temp6295.last_plus_one){ _throw(
Null_Exception);}* _temp6297;}).last_line));( struct _xenum_struct*) _temp6281;});
break; case 368: _LL6280: yyval=({ struct Cyc_Exp_tok_struct* _temp6299=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6299->tag=
Cyc_Exp_tok_tag; _temp6299->f1= Cyc_Absyn_instantiate_exp( Cyc_Absyn_new_exp(({
struct Cyc_Absyn_UnknownId_e_struct* _temp6300=( struct Cyc_Absyn_UnknownId_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp6300->tag= Cyc_Absyn_UnknownId_e_tag;
_temp6300->f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp6301= yyvs;
struct _xenum_struct** _temp6303= _temp6301.curr +( yyvsp_offset - 4); if(
_temp6303 < _temp6301.base? 1: _temp6303 >= _temp6301.last_plus_one){ _throw(
Null_Exception);}* _temp6303;}));( void*) _temp6300;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6304= yyls; struct Cyc_Yyltype* _temp6306= _temp6304.curr
+( yylsp_offset - 4); if( _temp6306 < _temp6304.base? 1: _temp6306 >= _temp6304.last_plus_one){
_throw( Null_Exception);}* _temp6306;}).first_line,({ struct _tagged_ptr3
_temp6307= yyls; struct Cyc_Yyltype* _temp6309= _temp6307.curr +( yylsp_offset -
4); if( _temp6309 < _temp6307.base? 1: _temp6309 >= _temp6307.last_plus_one){
_throw( Null_Exception);}* _temp6309;}).last_line)),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp6310= yyvs; struct _xenum_struct** _temp6312= _temp6310.curr +(
yyvsp_offset - 1); if( _temp6312 < _temp6310.base? 1: _temp6312 >= _temp6310.last_plus_one){
_throw( Null_Exception);}* _temp6312;}))), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6313= yyls; struct Cyc_Yyltype* _temp6315= _temp6313.curr +(
yylsp_offset - 4); if( _temp6315 < _temp6313.base? 1: _temp6315 >= _temp6313.last_plus_one){
_throw( Null_Exception);}* _temp6315;}).first_line,({ struct _tagged_ptr3
_temp6316= yyls; struct Cyc_Yyltype* _temp6318= _temp6316.curr + yylsp_offset;
if( _temp6318 < _temp6316.base? 1: _temp6318 >= _temp6316.last_plus_one){ _throw(
Null_Exception);}* _temp6318;}).last_line));( struct _xenum_struct*) _temp6299;});
break; case 369: _LL6298: yyval=({ struct Cyc_Exp_tok_struct* _temp6320=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6320->tag=
Cyc_Exp_tok_tag; _temp6320->f1= Cyc_Absyn_tuple_exp( Cyc_yyget_ExpList_tok(({
struct _tagged_ptr2 _temp6321= yyvs; struct _xenum_struct** _temp6323= _temp6321.curr
+( yyvsp_offset - 1); if( _temp6323 < _temp6321.base? 1: _temp6323 >= _temp6321.last_plus_one){
_throw( Null_Exception);}* _temp6323;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6324= yyls; struct Cyc_Yyltype* _temp6326= _temp6324.curr +(
yylsp_offset - 3); if( _temp6326 < _temp6324.base? 1: _temp6326 >= _temp6324.last_plus_one){
_throw( Null_Exception);}* _temp6326;}).first_line,({ struct _tagged_ptr3
_temp6327= yyls; struct Cyc_Yyltype* _temp6329= _temp6327.curr + yylsp_offset;
if( _temp6329 < _temp6327.base? 1: _temp6329 >= _temp6327.last_plus_one){ _throw(
Null_Exception);}* _temp6329;}).last_line));( struct _xenum_struct*) _temp6320;});
break; case 370: _LL6319: yyval=({ struct Cyc_Exp_tok_struct* _temp6331=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6331->tag=
Cyc_Exp_tok_tag; _temp6331->f1= Cyc_Absyn_new_exp(({ struct Cyc_Absyn_Struct_e_struct*
_temp6332=( struct Cyc_Absyn_Struct_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct));
_temp6332->tag= Cyc_Absyn_Struct_e_tag; _temp6332->f1= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp6333= yyvs; struct _xenum_struct** _temp6335= _temp6333.curr
+( yyvsp_offset - 3); if( _temp6335 < _temp6333.base? 1: _temp6335 >= _temp6333.last_plus_one){
_throw( Null_Exception);}* _temp6335;})); _temp6332->f2= 0; _temp6332->f3=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp6336= yyvs; struct _xenum_struct** _temp6338= _temp6336.curr
+( yyvsp_offset - 1); if( _temp6338 < _temp6336.base? 1: _temp6338 >= _temp6336.last_plus_one){
_throw( Null_Exception);}* _temp6338;}))); _temp6332->f4= 0;( void*) _temp6332;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6339= yyls; struct Cyc_Yyltype*
_temp6341= _temp6339.curr +( yylsp_offset - 3); if( _temp6341 < _temp6339.base?
1: _temp6341 >= _temp6339.last_plus_one){ _throw( Null_Exception);}* _temp6341;}).first_line,({
struct _tagged_ptr3 _temp6342= yyls; struct Cyc_Yyltype* _temp6344= _temp6342.curr
+ yylsp_offset; if( _temp6344 < _temp6342.base? 1: _temp6344 >= _temp6342.last_plus_one){
_throw( Null_Exception);}* _temp6344;}).last_line));( struct _xenum_struct*)
_temp6331;}); break; case 371: _LL6330: yyval=({ struct Cyc_Exp_tok_struct*
_temp6346=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6346->tag= Cyc_Exp_tok_tag; _temp6346->f1= Cyc_Absyn_stmt_exp( Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp6347= yyvs; struct _xenum_struct** _temp6349= _temp6347.curr
+( yyvsp_offset - 2); if( _temp6349 < _temp6347.base? 1: _temp6349 >= _temp6347.last_plus_one){
_throw( Null_Exception);}* _temp6349;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6350= yyls; struct Cyc_Yyltype* _temp6352= _temp6350.curr +(
yylsp_offset - 4); if( _temp6352 < _temp6350.base? 1: _temp6352 >= _temp6350.last_plus_one){
_throw( Null_Exception);}* _temp6352;}).first_line,({ struct _tagged_ptr3
_temp6353= yyls; struct Cyc_Yyltype* _temp6355= _temp6353.curr + yylsp_offset;
if( _temp6355 < _temp6353.base? 1: _temp6355 >= _temp6353.last_plus_one){ _throw(
Null_Exception);}* _temp6355;}).last_line));( struct _xenum_struct*) _temp6346;});
break; case 372: _LL6345: yyval=({ struct Cyc_ExpList_tok_struct* _temp6357=(
struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp6357->tag= Cyc_ExpList_tok_tag; _temp6357->f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ExpList_tok(({ struct
_tagged_ptr2 _temp6358= yyvs; struct _xenum_struct** _temp6360= _temp6358.curr +
yyvsp_offset; if( _temp6360 < _temp6358.base? 1: _temp6360 >= _temp6358.last_plus_one){
_throw( Null_Exception);}* _temp6360;})));( struct _xenum_struct*) _temp6357;});
break; case 373: _LL6356: yyval=({ struct Cyc_ExpList_tok_struct* _temp6362=(
struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp6362->tag= Cyc_ExpList_tok_tag; _temp6362->f1=({ struct Cyc_List_List*
_temp6363=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp6363->hd=( void*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6364= yyvs;
struct _xenum_struct** _temp6366= _temp6364.curr + yyvsp_offset; if( _temp6366 <
_temp6364.base? 1: _temp6366 >= _temp6364.last_plus_one){ _throw( Null_Exception);}*
_temp6366;})); _temp6363->tl= 0; _temp6363;});( struct _xenum_struct*) _temp6362;});
break; case 374: _LL6361: yyval=({ struct Cyc_ExpList_tok_struct* _temp6368=(
struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));
_temp6368->tag= Cyc_ExpList_tok_tag; _temp6368->f1=({ struct Cyc_List_List*
_temp6369=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp6369->hd=( void*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6373= yyvs;
struct _xenum_struct** _temp6375= _temp6373.curr + yyvsp_offset; if( _temp6375 <
_temp6373.base? 1: _temp6375 >= _temp6373.last_plus_one){ _throw( Null_Exception);}*
_temp6375;})); _temp6369->tl= Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2
_temp6370= yyvs; struct _xenum_struct** _temp6372= _temp6370.curr +(
yyvsp_offset - 2); if( _temp6372 < _temp6370.base? 1: _temp6372 >= _temp6370.last_plus_one){
_throw( Null_Exception);}* _temp6372;})); _temp6369;});( struct _xenum_struct*)
_temp6368;}); break; case 375: _LL6367: yyval=({ struct Cyc_Exp_tok_struct*
_temp6377=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));
_temp6377->tag= Cyc_Exp_tok_tag; _temp6377->f1= Cyc_Absyn_int_exp((* Cyc_yyget_Int_tok(({
struct _tagged_ptr2 _temp6378= yyvs; struct _xenum_struct** _temp6380= _temp6378.curr
+ yyvsp_offset; if( _temp6380 < _temp6378.base? 1: _temp6380 >= _temp6378.last_plus_one){
_throw( Null_Exception);}* _temp6380;}))).f1,(* Cyc_yyget_Int_tok(({ struct
_tagged_ptr2 _temp6381= yyvs; struct _xenum_struct** _temp6383= _temp6381.curr +
yyvsp_offset; if( _temp6383 < _temp6381.base? 1: _temp6383 >= _temp6381.last_plus_one){
_throw( Null_Exception);}* _temp6383;}))).f2, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6384= yyls; struct Cyc_Yyltype* _temp6386= _temp6384.curr
+ yylsp_offset; if( _temp6386 < _temp6384.base? 1: _temp6386 >= _temp6384.last_plus_one){
_throw( Null_Exception);}* _temp6386;}).first_line,({ struct _tagged_ptr3
_temp6387= yyls; struct Cyc_Yyltype* _temp6389= _temp6387.curr + yylsp_offset;
if( _temp6389 < _temp6387.base? 1: _temp6389 >= _temp6387.last_plus_one){ _throw(
Null_Exception);}* _temp6389;}).last_line));( struct _xenum_struct*) _temp6377;});
break; case 376: _LL6376: yyval=({ struct Cyc_Exp_tok_struct* _temp6391=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6391->tag=
Cyc_Exp_tok_tag; _temp6391->f1= Cyc_Absyn_char_exp( Cyc_yyget_Char_tok(({ struct
_tagged_ptr2 _temp6392= yyvs; struct _xenum_struct** _temp6394= _temp6392.curr +
yyvsp_offset; if( _temp6394 < _temp6392.base? 1: _temp6394 >= _temp6392.last_plus_one){
_throw( Null_Exception);}* _temp6394;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6395= yyls; struct Cyc_Yyltype* _temp6397= _temp6395.curr +
yylsp_offset; if( _temp6397 < _temp6395.base? 1: _temp6397 >= _temp6395.last_plus_one){
_throw( Null_Exception);}* _temp6397;}).first_line,({ struct _tagged_ptr3
_temp6398= yyls; struct Cyc_Yyltype* _temp6400= _temp6398.curr + yylsp_offset;
if( _temp6400 < _temp6398.base? 1: _temp6400 >= _temp6398.last_plus_one){ _throw(
Null_Exception);}* _temp6400;}).last_line));( struct _xenum_struct*) _temp6391;});
break; case 377: _LL6390: yyval=({ struct Cyc_Exp_tok_struct* _temp6402=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6402->tag=
Cyc_Exp_tok_tag; _temp6402->f1= Cyc_Absyn_float_exp( Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp6403= yyvs; struct _xenum_struct** _temp6405= _temp6403.curr
+ yyvsp_offset; if( _temp6405 < _temp6403.base? 1: _temp6405 >= _temp6403.last_plus_one){
_throw( Null_Exception);}* _temp6405;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6406= yyls; struct Cyc_Yyltype* _temp6408= _temp6406.curr +
yylsp_offset; if( _temp6408 < _temp6406.base? 1: _temp6408 >= _temp6406.last_plus_one){
_throw( Null_Exception);}* _temp6408;}).first_line,({ struct _tagged_ptr3
_temp6409= yyls; struct Cyc_Yyltype* _temp6411= _temp6409.curr + yylsp_offset;
if( _temp6411 < _temp6409.base? 1: _temp6411 >= _temp6409.last_plus_one){ _throw(
Null_Exception);}* _temp6411;}).last_line));( struct _xenum_struct*) _temp6402;});
break; case 378: _LL6401: yyval=({ struct Cyc_Exp_tok_struct* _temp6413=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct)); _temp6413->tag=
Cyc_Exp_tok_tag; _temp6413->f1= Cyc_Absyn_null_exp( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6414= yyls; struct Cyc_Yyltype* _temp6416= _temp6414.curr
+ yylsp_offset; if( _temp6416 < _temp6414.base? 1: _temp6416 >= _temp6414.last_plus_one){
_throw( Null_Exception);}* _temp6416;}).first_line,({ struct _tagged_ptr3
_temp6417= yyls; struct Cyc_Yyltype* _temp6419= _temp6417.curr + yylsp_offset;
if( _temp6419 < _temp6417.base? 1: _temp6419 >= _temp6417.last_plus_one){ _throw(
Null_Exception);}* _temp6419;}).last_line));( struct _xenum_struct*) _temp6413;});
break; case 379: _LL6412: yyval=({ struct Cyc_QualId_tok_struct* _temp6421=(
struct Cyc_QualId_tok_struct*) GC_malloc( sizeof( struct Cyc_QualId_tok_struct));
_temp6421->tag= Cyc_QualId_tok_tag; _temp6421->f1=({ struct _tuple1* _temp6422=(
struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp6422->f1=({ struct
Cyc_Absyn_Rel_n_struct* _temp6427=( struct Cyc_Absyn_Rel_n_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Rel_n_struct)); _temp6427->tag= Cyc_Absyn_Rel_n_tag;
_temp6427->f1= 0;( void*) _temp6427;}); _temp6422->f2=({ struct _tagged_string*
_temp6423=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp6423[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp6424= yyvs;
struct _xenum_struct** _temp6426= _temp6424.curr + yyvsp_offset; if( _temp6426 <
_temp6424.base? 1: _temp6426 >= _temp6424.last_plus_one){ _throw( Null_Exception);}*
_temp6426;})); _temp6423;}); _temp6422;});( struct _xenum_struct*) _temp6421;});
break; case 380: _LL6420: yyval=({ struct _tagged_ptr2 _temp6429= yyvs; struct
_xenum_struct** _temp6431= _temp6429.curr + yyvsp_offset; if( _temp6431 <
_temp6429.base? 1: _temp6431 >= _temp6429.last_plus_one){ _throw( Null_Exception);}*
_temp6431;}); break; default: _LL6428:( void) _throw(({ struct Cyc_Yyimpossible_struct*
_temp6433=( struct Cyc_Yyimpossible_struct*) GC_malloc_atomic( sizeof( struct
Cyc_Yyimpossible_struct)); _temp6433->tag= Cyc_Yyimpossible_tag;( struct
_xenum_struct*) _temp6433;}));} yyvsp_offset -= yylen; yyssp_offset -= yylen;
yylsp_offset -= yylen;({ struct _tagged_ptr2 _temp6434= yyvs; struct
_xenum_struct** _temp6436= _temp6434.curr +( ++ yyvsp_offset); if( _temp6436 <
_temp6434.base? 1: _temp6436 >= _temp6434.last_plus_one){ _throw( Null_Exception);}*
_temp6436= yyval;}); yylsp_offset ++; if( yylen == 0){({ struct _tagged_ptr3
_temp6437= yyls; struct Cyc_Yyltype* _temp6439= _temp6437.curr + yylsp_offset;
if( _temp6439 < _temp6437.base? 1: _temp6439 >= _temp6437.last_plus_one){ _throw(
Null_Exception);}(* _temp6439).first_line= Cyc_yylloc.first_line;});({ struct
_tagged_ptr3 _temp6440= yyls; struct Cyc_Yyltype* _temp6442= _temp6440.curr +
yylsp_offset; if( _temp6442 < _temp6440.base? 1: _temp6442 >= _temp6440.last_plus_one){
_throw( Null_Exception);}(* _temp6442).first_column= Cyc_yylloc.first_column;});({
struct _tagged_ptr3 _temp6443= yyls; struct Cyc_Yyltype* _temp6445= _temp6443.curr
+ yylsp_offset; if( _temp6445 < _temp6443.base? 1: _temp6445 >= _temp6443.last_plus_one){
_throw( Null_Exception);}(* _temp6445).last_line=({ struct _tagged_ptr3
_temp6446= yyls; struct Cyc_Yyltype* _temp6448= _temp6446.curr +( yylsp_offset -
1); if( _temp6448 < _temp6446.base? 1: _temp6448 >= _temp6446.last_plus_one){
_throw( Null_Exception);}* _temp6448;}).last_line;});({ struct _tagged_ptr3
_temp6449= yyls; struct Cyc_Yyltype* _temp6451= _temp6449.curr + yylsp_offset;
if( _temp6451 < _temp6449.base? 1: _temp6451 >= _temp6449.last_plus_one){ _throw(
Null_Exception);}(* _temp6451).last_column=({ struct _tagged_ptr3 _temp6452=
yyls; struct Cyc_Yyltype* _temp6454= _temp6452.curr +( yylsp_offset - 1); if(
_temp6454 < _temp6452.base? 1: _temp6454 >= _temp6452.last_plus_one){ _throw(
Null_Exception);}* _temp6454;}).last_column;});({ struct _tagged_ptr3 _temp6455=
yyls; struct Cyc_Yyltype* _temp6457= _temp6455.curr + yylsp_offset; if(
_temp6457 < _temp6455.base? 1: _temp6457 >= _temp6455.last_plus_one){ _throw(
Null_Exception);}(* _temp6457).text=( struct _tagged_string)({ char* _temp6458=(
char*)""; struct _tagged_string _temp6459; _temp6459.curr= _temp6458; _temp6459.base=
_temp6458; _temp6459.last_plus_one= _temp6458 + 1; _temp6459;});});} else{({
struct _tagged_ptr3 _temp6460= yyls; struct Cyc_Yyltype* _temp6462= _temp6460.curr
+ yylsp_offset; if( _temp6462 < _temp6460.base? 1: _temp6462 >= _temp6460.last_plus_one){
_throw( Null_Exception);}(* _temp6462).last_line=({ struct _tagged_ptr3
_temp6463= yyls; struct Cyc_Yyltype* _temp6465= _temp6463.curr +(( yylsp_offset
+ yylen) - 1); if( _temp6465 < _temp6463.base? 1: _temp6465 >= _temp6463.last_plus_one){
_throw( Null_Exception);}* _temp6465;}).last_line;});({ struct _tagged_ptr3
_temp6466= yyls; struct Cyc_Yyltype* _temp6468= _temp6466.curr + yylsp_offset;
if( _temp6468 < _temp6466.base? 1: _temp6468 >= _temp6466.last_plus_one){ _throw(
Null_Exception);}(* _temp6468).last_column=({ struct _tagged_ptr3 _temp6469=
yyls; struct Cyc_Yyltype* _temp6471= _temp6469.curr +(( yylsp_offset + yylen) -
1); if( _temp6471 < _temp6469.base? 1: _temp6471 >= _temp6469.last_plus_one){
_throw( Null_Exception);}* _temp6471;}).last_column;});} yyn=( int)({ short*
_temp6472=( short*) Cyc_yyr1; unsigned int _temp6473= yyn; if( _temp6473 >= 381u){
_throw( Null_Exception);} _temp6472[ _temp6473];}); yystate=( int)({ short*
_temp6474=( short*) Cyc_yypgoto; unsigned int _temp6475= yyn - 119; if(
_temp6475 >= 102u){ _throw( Null_Exception);} _temp6474[ _temp6475];}) +( int)({
struct _tagged_ptr1 _temp6476= yyss; short* _temp6478= _temp6476.curr +
yyssp_offset; if( _temp6478 < _temp6476.base? 1: _temp6478 >= _temp6476.last_plus_one){
_throw( Null_Exception);}* _temp6478;}); if(( yystate >= 0? yystate <= 4314: 0)?(
int)({ short* _temp6479=( short*) Cyc_yycheck; unsigned int _temp6480= yystate;
if( _temp6480 >= 4315u){ _throw( Null_Exception);} _temp6479[ _temp6480];}) ==(
int)({ struct _tagged_ptr1 _temp6481= yyss; short* _temp6483= _temp6481.curr +
yyssp_offset; if( _temp6483 < _temp6481.base? 1: _temp6483 >= _temp6481.last_plus_one){
_throw( Null_Exception);}* _temp6483;}): 0){ yystate=( int)({ short* _temp6484=(
short*) Cyc_yytable; unsigned int _temp6485= yystate; if( _temp6485 >= 4315u){
_throw( Null_Exception);} _temp6484[ _temp6485];});} else{ yystate=( int)({
short* _temp6486=( short*) Cyc_yydefgoto; unsigned int _temp6487= yyn - 119; if(
_temp6487 >= 102u){ _throw( Null_Exception);} _temp6486[ _temp6487];});} goto
yynewstate; yyerrlab: if( yyerrstatus == 0){ ++ Cyc_yynerrs; yyn=( int)({ short*
_temp6488=( short*) Cyc_yypact; unsigned int _temp6489= yystate; if( _temp6489
>= 717u){ _throw( Null_Exception);} _temp6488[ _temp6489];}); if( yyn > - 32768?
yyn < 4314: 0){ int sze= 0; struct _tagged_string msg; int x; int count; count=
0; for( x= yyn < 0? - yyn: 0;( unsigned int) x < 221u / sizeof( char*); x ++){
if(( int)({ short* _temp6490=( short*) Cyc_yycheck; unsigned int _temp6491= x +
yyn; if( _temp6491 >= 4315u){ _throw( Null_Exception);} _temp6490[ _temp6491];})
== x){( unsigned int)( sze += Cyc_String_strlen(({ struct _tagged_string*
_temp6492=( struct _tagged_string*) Cyc_yytname; unsigned int _temp6493= x; if(
_temp6493 >= 221u){ _throw( Null_Exception);} _temp6492[ _temp6493];})) +(
unsigned int) 15), count ++;}} msg=({ unsigned int _temp6494=( unsigned int)(
sze + 15); char* _temp6495=( char*) GC_malloc_atomic( sizeof( char) * _temp6494);
unsigned int i; struct _tagged_string _temp6496={ _temp6495, _temp6495,
_temp6495 + _temp6494}; for( i= 0; i < _temp6494; i ++){ _temp6495[ i]='\000';}
_temp6496;}); Cyc_String_strcpy( msg,( struct _tagged_string)({ char* _temp6497=(
char*)"parse error"; struct _tagged_string _temp6498; _temp6498.curr= _temp6497;
_temp6498.base= _temp6497; _temp6498.last_plus_one= _temp6497 + 12; _temp6498;}));
if( count < 5){ count= 0; for( x= yyn < 0? - yyn: 0;( unsigned int) x < 221u /
sizeof( char*); x ++){ if(( int)({ short* _temp6499=( short*) Cyc_yycheck;
unsigned int _temp6500= x + yyn; if( _temp6500 >= 4315u){ _throw( Null_Exception);}
_temp6499[ _temp6500];}) == x){ Cyc_String_strcat( msg, count == 0?( struct
_tagged_string)({ char* _temp6501=( char*)", expecting `"; struct _tagged_string
_temp6502; _temp6502.curr= _temp6501; _temp6502.base= _temp6501; _temp6502.last_plus_one=
_temp6501 + 14; _temp6502;}):( struct _tagged_string)({ char* _temp6503=( char*)" or `";
struct _tagged_string _temp6504; _temp6504.curr= _temp6503; _temp6504.base=
_temp6503; _temp6504.last_plus_one= _temp6503 + 6; _temp6504;})); Cyc_String_strcat(
msg,({ struct _tagged_string* _temp6505=( struct _tagged_string*) Cyc_yytname;
unsigned int _temp6506= x; if( _temp6506 >= 221u){ _throw( Null_Exception);}
_temp6505[ _temp6506];})); Cyc_String_strcat( msg,( struct _tagged_string)({
char* _temp6507=( char*)"'"; struct _tagged_string _temp6508; _temp6508.curr=
_temp6507; _temp6508.base= _temp6507; _temp6508.last_plus_one= _temp6507 + 2;
_temp6508;})); count ++;}}} Cyc_yyerror( msg);} else{ Cyc_yyerror(( struct
_tagged_string)({ char* _temp6509=( char*)"parse error"; struct _tagged_string
_temp6510; _temp6510.curr= _temp6509; _temp6510.base= _temp6509; _temp6510.last_plus_one=
_temp6509 + 12; _temp6510;}));}} goto yyerrlab1; yyerrlab1: if( yyerrstatus == 3){
if( Cyc_yychar == 0){ return 1;} Cyc_yychar= - 2;} yyerrstatus= 3; goto
yyerrhandle; yyerrdefault: yyerrpop: if( yyssp_offset == 0){ return 1;}
yyvsp_offset --; yystate=( int)({ struct _tagged_ptr1 _temp6511= yyss; short*
_temp6513= _temp6511.curr +( -- yyssp_offset); if( _temp6513 < _temp6511.base? 1:
_temp6513 >= _temp6511.last_plus_one){ _throw( Null_Exception);}* _temp6513;});
yylsp_offset --; yyerrhandle: yyn=( int)({ short* _temp6514=( short*) Cyc_yypact;
unsigned int _temp6515= yystate; if( _temp6515 >= 717u){ _throw( Null_Exception);}
_temp6514[ _temp6515];}); if( yyn == - 32768){ goto yyerrdefault;} yyn += 1; if((
yyn < 0? 1: yyn > 4314)? 1:( int)({ short* _temp6516=( short*) Cyc_yycheck;
unsigned int _temp6517= yyn; if( _temp6517 >= 4315u){ _throw( Null_Exception);}
_temp6516[ _temp6517];}) != 1){ goto yyerrdefault;} yyn=( int)({ short*
_temp6518=( short*) Cyc_yytable; unsigned int _temp6519= yyn; if( _temp6519 >=
4315u){ _throw( Null_Exception);} _temp6518[ _temp6519];}); if( yyn < 0){ if(
yyn == - 32768){ goto yyerrpop;} yyn= - yyn; goto yyreduce;} else{ if( yyn == 0){
goto yyerrpop;}} if( yyn == 716){ return 0;}({ struct _tagged_ptr2 _temp6520=
yyvs; struct _xenum_struct** _temp6522= _temp6520.curr +( ++ yyvsp_offset); if(
_temp6522 < _temp6520.base? 1: _temp6522 >= _temp6520.last_plus_one){ _throw(
Null_Exception);}* _temp6522= Cyc_yylval;});({ struct _tagged_ptr3 _temp6523=
yyls; struct Cyc_Yyltype* _temp6525= _temp6523.curr +( ++ yylsp_offset); if(
_temp6525 < _temp6523.base? 1: _temp6525 >= _temp6523.last_plus_one){ _throw(
Null_Exception);}* _temp6525= Cyc_yylloc;}); goto yynewstate;} void Cyc_yyprint(
int i, struct _xenum_struct* v){ struct _xenum_struct* _temp6526= v; struct
_tuple11* _temp6546; struct _tuple11 _temp6548; int _temp6549; void* _temp6551;
char _temp6553; short _temp6555; struct _tagged_string _temp6557; struct Cyc_Core_Opt*
_temp6559; struct Cyc_Core_Opt* _temp6561; struct Cyc_Core_Opt _temp6563; struct
_tagged_string* _temp6564; struct _tuple1* _temp6566; struct _tuple1 _temp6568;
struct _tagged_string* _temp6569; void* _temp6571; _LL6528: if( _temp6526->tag
== Cyc_Okay_tok_tag){ goto _LL6529;} else{ goto _LL6530;} _LL6530: if((*((
struct _xenum_struct*) _temp6526)).tag == Cyc_Int_tok_tag){ _LL6547: _temp6546=((
struct Cyc_Int_tok_struct*) _temp6526)->f1; _temp6548=* _temp6546; _LL6552:
_temp6551= _temp6548.f1; goto _LL6550; _LL6550: _temp6549= _temp6548.f2; goto
_LL6531;} else{ goto _LL6532;} _LL6532: if((*(( struct _xenum_struct*) _temp6526)).tag
== Cyc_Char_tok_tag){ _LL6554: _temp6553=(( struct Cyc_Char_tok_struct*)
_temp6526)->f1; goto _LL6533;} else{ goto _LL6534;} _LL6534: if((*(( struct
_xenum_struct*) _temp6526)).tag == Cyc_Short_tok_tag){ _LL6556: _temp6555=((
struct Cyc_Short_tok_struct*) _temp6526)->f1; goto _LL6535;} else{ goto _LL6536;}
_LL6536: if((*(( struct _xenum_struct*) _temp6526)).tag == Cyc_String_tok_tag){
_LL6558: _temp6557=(( struct Cyc_String_tok_struct*) _temp6526)->f1; goto
_LL6537;} else{ goto _LL6538;} _LL6538: if((*(( struct _xenum_struct*) _temp6526)).tag
== Cyc_Stringopt_tok_tag){ _LL6560: _temp6559=(( struct Cyc_Stringopt_tok_struct*)
_temp6526)->f1; if( _temp6559 == 0){ goto _LL6539;} else{ goto _LL6540;}} else{
goto _LL6540;} _LL6540: if((*(( struct _xenum_struct*) _temp6526)).tag == Cyc_Stringopt_tok_tag){
_LL6562: _temp6561=(( struct Cyc_Stringopt_tok_struct*) _temp6526)->f1; if(
_temp6561 == 0){ goto _LL6542;} else{ _temp6563=* _temp6561; _LL6565: _temp6564=(
struct _tagged_string*) _temp6563.v; goto _LL6541;}} else{ goto _LL6542;}
_LL6542: if((*(( struct _xenum_struct*) _temp6526)).tag == Cyc_QualId_tok_tag){
_LL6567: _temp6566=(( struct Cyc_QualId_tok_struct*) _temp6526)->f1; _temp6568=*
_temp6566; _LL6572: _temp6571= _temp6568.f1; goto _LL6570; _LL6570: _temp6569=
_temp6568.f2; goto _LL6543;} else{ goto _LL6544;} _LL6544: goto _LL6545; _LL6529:
fprintf( Cyc_Stdio_stderr,"ok"); goto _LL6527; _LL6531: fprintf( Cyc_Stdio_stderr,"%d",
_temp6549); goto _LL6527; _LL6533: fprintf( Cyc_Stdio_stderr,"%c",( int)
_temp6553); goto _LL6527; _LL6535: fprintf( Cyc_Stdio_stderr,"%ds",( int)
_temp6555); goto _LL6527; _LL6537:({ struct _tagged_string _temp6573= _temp6557;
fprintf( Cyc_Stdio_stderr,"\"%.*s\"", _temp6573.last_plus_one - _temp6573.curr,
_temp6573.curr);}); goto _LL6527; _LL6539: fprintf( Cyc_Stdio_stderr,"null");
goto _LL6527; _LL6541:({ struct _tagged_string _temp6574=* _temp6564; fprintf(
Cyc_Stdio_stderr,"\"%.*s\"", _temp6574.last_plus_one - _temp6574.curr, _temp6574.curr);});
goto _LL6527; _LL6543: { struct Cyc_List_List* prefix= 0;{ void* _temp6575=
_temp6571; struct Cyc_List_List* _temp6583; struct Cyc_List_List* _temp6585;
_LL6577: if(( unsigned int) _temp6575 > 1u?(( struct _enum_struct*) _temp6575)->tag
== Cyc_Absyn_Rel_n_tag: 0){ _LL6584: _temp6583=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Rel_n_struct*) _temp6575)->f1; goto _LL6578;} else{ goto _LL6579;}
_LL6579: if(( unsigned int) _temp6575 > 1u?(( struct _enum_struct*) _temp6575)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL6586: _temp6585=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp6575)->f1; goto _LL6580;} else{ goto _LL6581;}
_LL6581: if( _temp6575 == Cyc_Absyn_Loc_n){ goto _LL6582;} else{ goto _LL6576;}
_LL6578: prefix= _temp6583; goto _LL6576; _LL6580: prefix= _temp6585; goto
_LL6576; _LL6582: goto _LL6576; _LL6576:;} for( 0; prefix != 0; prefix= prefix->tl){({
struct _tagged_string _temp6587=*(( struct _tagged_string*) prefix->hd); fprintf(
Cyc_Stdio_stderr,"%.*s::", _temp6587.last_plus_one - _temp6587.curr, _temp6587.curr);});}({
struct _tagged_string _temp6588=* _temp6569; fprintf( Cyc_Stdio_stderr,"%.*s::",
_temp6588.last_plus_one - _temp6588.curr, _temp6588.curr);}); goto _LL6527;}
_LL6545: fprintf( Cyc_Stdio_stderr,"?"); goto _LL6527; _LL6527:;} struct Cyc_List_List*
Cyc_Parse_parse_file( struct Cyc_Stdio___sFILE* f){ Cyc_Parse_parse_result= 0;
Cyc_Parse_lbuf=({ struct Cyc_Core_Opt* _temp6589=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp6589->v=( void*) Cyc_Lexing_from_file(
f); _temp6589;}); Cyc_yyparse(); return Cyc_Parse_parse_result;}