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
struct Cyc_Absyn_Fndecl; struct Cyc_Absyn_Structdecl; struct Cyc_Absyn_Enumfield;
struct Cyc_Absyn_Enumdecl; struct Cyc_Absyn_Xenumdecl; struct Cyc_Absyn_Typedefdecl;
struct Cyc_Absyn_Vardecl; typedef void* Cyc_Absyn_Raw_decl; struct Cyc_Absyn_Decl;
typedef void* Cyc_Absyn_Designator; typedef struct _xenum_struct* Cyc_Absyn_StmtAnnot;
typedef void* Cyc_Absyn_Attribute; struct Cyc_Absyn_Structfield; typedef void*
Cyc_Absyn_Mallocarg; typedef void* Cyc_Absyn_scope; typedef struct Cyc_Absyn_Tqual*
Cyc_Absyn_tqual; typedef void* Cyc_Absyn_size_of_t; typedef void* Cyc_Absyn_kind_t;
typedef struct Cyc_Absyn_Tvar* Cyc_Absyn_tvar; typedef void* Cyc_Absyn_sign_t;
typedef struct Cyc_Absyn_Conref* Cyc_Absyn_conref; typedef void* Cyc_Absyn_constraint_t;
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
Cyc_Absyn_stmt_annot_t; typedef void* Cyc_Absyn_attribute_t; typedef struct Cyc_List_List*
Cyc_Absyn_attributes_t; typedef struct Cyc_Absyn_Structfield* Cyc_Absyn_structfield_t;
typedef void* Cyc_Absyn_mallocarg_t; extern void* Cyc_Absyn_Loc_n; extern const
int Cyc_Absyn_Rel_n_tag; struct Cyc_Absyn_Rel_n_struct{ int tag; struct Cyc_List_List*
f1; } ; extern const int Cyc_Absyn_Abs_n_tag; struct Cyc_Absyn_Abs_n_struct{ int
tag; struct Cyc_List_List* f1; } ; extern void* Cyc_Absyn_Static; extern void*
Cyc_Absyn_Abstract; extern void* Cyc_Absyn_Public; extern void* Cyc_Absyn_Extern;
extern void* Cyc_Absyn_ExternC; struct Cyc_Absyn_Tqual{ int q_const; int
q_volatile; int q_restrict; } ; extern void* Cyc_Absyn_B1; extern void* Cyc_Absyn_B2;
extern void* Cyc_Absyn_B4; extern void* Cyc_Absyn_B8; extern void* Cyc_Absyn_AnyKind;
extern void* Cyc_Absyn_MemKind; extern void* Cyc_Absyn_BoxKind; extern void* Cyc_Absyn_RgnKind;
extern void* Cyc_Absyn_EffKind; extern void* Cyc_Absyn_Signed; extern void* Cyc_Absyn_Unsigned;
struct Cyc_Absyn_Conref{ void* v; } ; extern const int Cyc_Absyn_Eq_constr_tag;
struct Cyc_Absyn_Eq_constr_struct{ int tag; void* f1; } ; extern const int Cyc_Absyn_Forward_constr_tag;
struct Cyc_Absyn_Forward_constr_struct{ int tag; struct Cyc_Absyn_Conref* f1; }
; extern void* Cyc_Absyn_No_constr; struct Cyc_Absyn_Tvar{ struct _tagged_string*
name; struct Cyc_Absyn_Conref* kind; } ; extern void* Cyc_Absyn_Unknown_b;
extern const int Cyc_Absyn_Upper_b_tag; struct Cyc_Absyn_Upper_b_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; struct Cyc_Absyn_PtrInfo{ void* elt_typ; void*
rgn_typ; struct Cyc_Absyn_Conref* nullable; struct Cyc_Absyn_Tqual* tq; struct
Cyc_Absyn_Conref* bounds; } ; struct Cyc_Absyn_FnInfo{ struct Cyc_List_List*
tvars; struct Cyc_Core_Opt* effect; void* ret_typ; struct Cyc_List_List* args;
int varargs; struct Cyc_List_List* attributes; } ; extern void* Cyc_Absyn_VoidType;
extern const int Cyc_Absyn_Evar_tag; struct Cyc_Absyn_Evar_struct{ int tag; void*
f1; struct Cyc_Core_Opt* f2; int f3; } ; extern const int Cyc_Absyn_VarType_tag;
struct Cyc_Absyn_VarType_struct{ int tag; struct Cyc_Absyn_Tvar* f1; } ; extern
const int Cyc_Absyn_EnumType_tag; struct Cyc_Absyn_EnumType_struct{ int tag;
struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Enumdecl** f3; }
; extern const int Cyc_Absyn_XenumType_tag; struct Cyc_Absyn_XenumType_struct{
int tag; struct _tuple1* f1; struct Cyc_Absyn_Xenumdecl** f2; } ; extern const
int Cyc_Absyn_PointerType_tag; struct Cyc_Absyn_PointerType_struct{ int tag;
struct Cyc_Absyn_PtrInfo f1; } ; extern const int Cyc_Absyn_IntType_tag; struct
Cyc_Absyn_IntType_struct{ int tag; void* f1; void* f2; } ; extern void* Cyc_Absyn_FloatType;
extern void* Cyc_Absyn_DoubleType; extern const int Cyc_Absyn_ArrayType_tag;
struct Cyc_Absyn_ArrayType_struct{ int tag; void* f1; struct Cyc_Absyn_Tqual* f2;
struct Cyc_Absyn_Exp* f3; } ; extern const int Cyc_Absyn_FnType_tag; struct Cyc_Absyn_FnType_struct{
int tag; struct Cyc_Absyn_FnInfo f1; } ; extern const int Cyc_Absyn_TupleType_tag;
struct Cyc_Absyn_TupleType_struct{ int tag; struct Cyc_List_List* f1; } ; extern
const int Cyc_Absyn_StructType_tag; struct Cyc_Absyn_StructType_struct{ int tag;
struct _tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Structdecl** f3;
} ; extern void* Cyc_Absyn_UnionType; extern const int Cyc_Absyn_RgnHandleType_tag;
struct Cyc_Absyn_RgnHandleType_struct{ int tag; void* f1; } ; extern const int
Cyc_Absyn_TypedefType_tag; struct Cyc_Absyn_TypedefType_struct{ int tag; struct
_tuple1* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3; } ; extern void*
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
int tag; struct _tuple2* f1; struct Cyc_List_List* f2; } ; extern const int Cyc_Absyn_Array_e_tag;
struct Cyc_Absyn_Array_e_struct{ int tag; int f1; struct Cyc_List_List* f2; } ;
extern const int Cyc_Absyn_Comprehension_e_tag; struct Cyc_Absyn_Comprehension_e_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp*
f3; } ; extern const int Cyc_Absyn_Struct_e_tag; struct Cyc_Absyn_Struct_e_struct{
int tag; struct _tuple1* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List* f3;
struct Cyc_Absyn_Structdecl* f4; } ; extern const int Cyc_Absyn_Enum_e_tag;
struct Cyc_Absyn_Enum_e_struct{ int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Enumdecl* f4; struct Cyc_Absyn_Enumfield*
f5; } ; extern const int Cyc_Absyn_Xenum_e_tag; struct Cyc_Absyn_Xenum_e_struct{
int tag; struct Cyc_Core_Opt* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Xenumdecl*
f3; struct Cyc_Absyn_Enumfield* f4; } ; extern const int Cyc_Absyn_Malloc_e_tag;
struct Cyc_Absyn_Malloc_e_struct{ int tag; void* f1; } ; extern const int Cyc_Absyn_UnresolvedMem_e_tag;
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
Cyc_Absyn_Structdecl* f1; } ; extern void* Cyc_Absyn_Union_d; extern const int
Cyc_Absyn_Enum_d_tag; struct Cyc_Absyn_Enum_d_struct{ int tag; struct Cyc_Absyn_Enumdecl*
f1; } ; extern const int Cyc_Absyn_Xenum_d_tag; struct Cyc_Absyn_Xenum_d_struct{
int tag; struct Cyc_Absyn_Xenumdecl* f1; } ; extern const int Cyc_Absyn_Typedef_d_tag;
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
fs, struct Cyc_List_List* atts, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Decl* Cyc_Absyn_enum_decl( void* s, struct Cyc_Core_Opt* n, struct Cyc_List_List*
ts, struct Cyc_Core_Opt* fs, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Decl* Cyc_Absyn_xenum_decl( void* s, struct _tuple1* n, struct Cyc_List_List*
fs, struct Cyc_Position_Segment* loc); extern void* Cyc_Absyn_function_typ(
struct Cyc_List_List* tvs, struct Cyc_Core_Opt* eff_typ, void* ret_typ, struct
Cyc_List_List* args, int varargs, struct Cyc_List_List*); extern struct
_tagged_string Cyc_Absyn_attribute2string( void*); struct Cyc_PP_Ppstate;
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
GC_malloc_atomic( sizeof( struct Cyc_Position_Exit_struct));* _temp0=( struct
Cyc_Position_Exit_struct){.tag= Cyc_Position_Exit_tag};( struct _xenum_struct*)
_temp0;}));} static void Cyc_Parse_warn( struct _tagged_string msg, struct Cyc_Position_Segment*
sg){({ struct _tagged_string _temp1= Cyc_Position_string_of_segment( sg); struct
_tagged_string _temp2= msg; fprintf( Cyc_Stdio_stderr,"%.*s: Warning: %.*s\n",
_temp1.last_plus_one - _temp1.curr, _temp1.curr, _temp2.last_plus_one - _temp2.curr,
_temp2.curr);}); return;} static void* Cyc_Parse_unimp( struct _tagged_string
msg, struct Cyc_Position_Segment* sg){ return(( void*(*)( struct _tagged_string
msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({ struct _tagged_string
_temp3= msg; xprintf("%.*s unimplemented", _temp3.last_plus_one - _temp3.curr,
_temp3.curr);}), sg);} static void Cyc_Parse_unimp2( struct _tagged_string msg,
struct Cyc_Position_Segment* sg){({ struct _tagged_string _temp4= Cyc_Position_string_of_segment(
sg); struct _tagged_string _temp5= msg; fprintf( Cyc_Stdio_stderr,"%.*s: Warning: Cyclone does not yet support %.*s\n",
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
_temp65; void* _temp67; _LL59: if(( unsigned int) _temp57 > 5u?(( struct
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
_temp107; struct Cyc_Absyn_Structdecl* _temp109; struct Cyc_Absyn_Enumdecl*
_temp111; struct Cyc_Absyn_Typedefdecl* _temp113; struct Cyc_Absyn_Xenumdecl*
_temp115; struct Cyc_List_List* _temp117; struct _tagged_string* _temp119;
struct Cyc_List_List* _temp121; struct _tuple1* _temp123; struct Cyc_List_List*
_temp125; _LL73: if(( unsigned int) _temp71 > 1u?(( struct _enum_struct*)
_temp71)->tag == Cyc_Absyn_Var_d_tag: 0){ _LL96: _temp95=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_d_struct*) _temp71)->f1; goto _LL74;} else{ goto _LL75;}
_LL75: if(( unsigned int) _temp71 > 1u?(( struct _enum_struct*) _temp71)->tag ==
Cyc_Absyn_Let_d_tag: 0){ _LL106: _temp105=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*)
_temp71)->f1; goto _LL104; _LL104: _temp103=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp71)->f2; goto _LL102; _LL102: _temp101=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp71)->f3; goto _LL100; _LL100: _temp99=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*)
_temp71)->f4; goto _LL98; _LL98: _temp97=( int)(( struct Cyc_Absyn_Let_d_struct*)
_temp71)->f5; goto _LL76;} else{ goto _LL77;} _LL77: if(( unsigned int) _temp71
> 1u?(( struct _enum_struct*) _temp71)->tag == Cyc_Absyn_Fn_d_tag: 0){ _LL108:
_temp107=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Fn_d_struct*) _temp71)->f1;
goto _LL78;} else{ goto _LL79;} _LL79: if(( unsigned int) _temp71 > 1u?(( struct
_enum_struct*) _temp71)->tag == Cyc_Absyn_Struct_d_tag: 0){ _LL110: _temp109=(
struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*) _temp71)->f1;
goto _LL80;} else{ goto _LL81;} _LL81: if( _temp71 == Cyc_Absyn_Union_d){ goto
_LL82;} else{ goto _LL83;} _LL83: if(( unsigned int) _temp71 > 1u?(( struct
_enum_struct*) _temp71)->tag == Cyc_Absyn_Enum_d_tag: 0){ _LL112: _temp111=(
struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*) _temp71)->f1;
goto _LL84;} else{ goto _LL85;} _LL85: if(( unsigned int) _temp71 > 1u?(( struct
_enum_struct*) _temp71)->tag == Cyc_Absyn_Typedef_d_tag: 0){ _LL114: _temp113=(
struct Cyc_Absyn_Typedefdecl*)(( struct Cyc_Absyn_Typedef_d_struct*) _temp71)->f1;
goto _LL86;} else{ goto _LL87;} _LL87: if(( unsigned int) _temp71 > 1u?(( struct
_enum_struct*) _temp71)->tag == Cyc_Absyn_Xenum_d_tag: 0){ _LL116: _temp115=(
struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_d_struct*) _temp71)->f1;
goto _LL88;} else{ goto _LL89;} _LL89: if(( unsigned int) _temp71 > 1u?(( struct
_enum_struct*) _temp71)->tag == Cyc_Absyn_Namespace_d_tag: 0){ _LL120: _temp119=(
struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*) _temp71)->f1;
goto _LL118; _LL118: _temp117=( struct Cyc_List_List*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp71)->f2; goto _LL90;} else{ goto _LL91;} _LL91: if(( unsigned int) _temp71
> 1u?(( struct _enum_struct*) _temp71)->tag == Cyc_Absyn_Using_d_tag: 0){ _LL124:
_temp123=( struct _tuple1*)(( struct Cyc_Absyn_Using_d_struct*) _temp71)->f1;
goto _LL122; _LL122: _temp121=( struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*)
_temp71)->f2; goto _LL92;} else{ goto _LL93;} _LL93: if(( unsigned int) _temp71
> 1u?(( struct _enum_struct*) _temp71)->tag == Cyc_Absyn_ExternC_d_tag: 0){
_LL126: _temp125=( struct Cyc_List_List*)(( struct Cyc_Absyn_ExternC_d_struct*)
_temp71)->f1; goto _LL94;} else{ goto _LL72;} _LL74: if( _temp95->initializer !=
0){(( void(*)( struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_string)({ char* _temp127=( char*)"initializers are not allowed in parameter declarations";
struct _tagged_string _temp128; _temp128.curr= _temp127; _temp128.base= _temp127;
_temp128.last_plus_one= _temp127 + 55; _temp128;}), x->loc);}{ void* _temp129=(*
_temp95->name).f1; struct Cyc_List_List* _temp139; struct Cyc_List_List*
_temp141; _LL131: if( _temp129 == Cyc_Absyn_Loc_n){ goto _LL132;} else{ goto
_LL133;} _LL133: if(( unsigned int) _temp129 > 1u?(( struct _enum_struct*)
_temp129)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL140: _temp139=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp129)->f1; if( _temp139 == 0){ goto _LL134;}
else{ goto _LL135;}} else{ goto _LL135;} _LL135: if(( unsigned int) _temp129 > 1u?((
struct _enum_struct*) _temp129)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL142:
_temp141=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp129)->f1;
if( _temp141 == 0){ goto _LL136;} else{ goto _LL137;}} else{ goto _LL137;}
_LL137: goto _LL138; _LL132: goto _LL130; _LL134: goto _LL130; _LL136: goto
_LL130; _LL138: Cyc_Parse_err(( struct _tagged_string)({ char* _temp143=( char*)"module names not allowed on parameter declarations";
struct _tagged_string _temp144; _temp144.curr= _temp143; _temp144.base= _temp143;
_temp144.last_plus_one= _temp143 + 51; _temp144;}), x->loc); goto _LL130; _LL130:;}{
int found= 0; for( 0; params != 0; params= params->tl){ if( Cyc_String_zstrptrcmp((*
_temp95->name).f2,( struct _tagged_string*) params->hd) == 0){ found= 1; break;}}
if( ! found){(( void(*)( struct _tagged_string msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)(({ struct _tagged_string _temp145=*(* _temp95->name).f2;
xprintf("%.*s is not listed as a parameter", _temp145.last_plus_one - _temp145.curr,
_temp145.curr);}), x->loc);} return;} _LL76: decl_kind=( struct _tagged_string)({
char* _temp146=( char*)"let declaration"; struct _tagged_string _temp147;
_temp147.curr= _temp146; _temp147.base= _temp146; _temp147.last_plus_one=
_temp146 + 16; _temp147;}); goto _LL72; _LL78: decl_kind=( struct _tagged_string)({
char* _temp148=( char*)"function declaration"; struct _tagged_string _temp149;
_temp149.curr= _temp148; _temp149.base= _temp148; _temp149.last_plus_one=
_temp148 + 21; _temp149;}); goto _LL72; _LL80: decl_kind=( struct _tagged_string)({
char* _temp150=( char*)"struct declaration"; struct _tagged_string _temp151;
_temp151.curr= _temp150; _temp151.base= _temp150; _temp151.last_plus_one=
_temp150 + 19; _temp151;}); goto _LL72; _LL82: decl_kind=( struct _tagged_string)({
char* _temp152=( char*)"union declaration"; struct _tagged_string _temp153;
_temp153.curr= _temp152; _temp153.base= _temp152; _temp153.last_plus_one=
_temp152 + 18; _temp153;}); goto _LL72; _LL84: decl_kind=( struct _tagged_string)({
char* _temp154=( char*)"enum declaration"; struct _tagged_string _temp155;
_temp155.curr= _temp154; _temp155.base= _temp154; _temp155.last_plus_one=
_temp154 + 17; _temp155;}); goto _LL72; _LL86: decl_kind=( struct _tagged_string)({
char* _temp156=( char*)"typedef"; struct _tagged_string _temp157; _temp157.curr=
_temp156; _temp157.base= _temp156; _temp157.last_plus_one= _temp156 + 8;
_temp157;}); goto _LL72; _LL88: decl_kind=( struct _tagged_string)({ char*
_temp158=( char*)"xenum declaration"; struct _tagged_string _temp159; _temp159.curr=
_temp158; _temp159.base= _temp158; _temp159.last_plus_one= _temp158 + 18;
_temp159;}); goto _LL72; _LL90: decl_kind=( struct _tagged_string)({ char*
_temp160=( char*)"namespace declaration"; struct _tagged_string _temp161;
_temp161.curr= _temp160; _temp161.base= _temp160; _temp161.last_plus_one=
_temp160 + 22; _temp161;}); goto _LL72; _LL92: decl_kind=( struct _tagged_string)({
char* _temp162=( char*)"using declaration"; struct _tagged_string _temp163;
_temp163.curr= _temp162; _temp163.base= _temp162; _temp163.last_plus_one=
_temp162 + 18; _temp163;}); goto _LL72; _LL94: decl_kind=( struct _tagged_string)({
char* _temp164=( char*)"extern C declaration"; struct _tagged_string _temp165;
_temp165.curr= _temp164; _temp165.base= _temp164; _temp165.last_plus_one=
_temp164 + 21; _temp165;}); goto _LL72; _LL72:;}(( void(*)( struct
_tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({ struct
_tagged_string _temp166= decl_kind; xprintf("%.*s appears in parameter type",
_temp166.last_plus_one - _temp166.curr, _temp166.curr);}), x->loc); return;}
static struct _tuple2* Cyc_Parse_get_param_type( struct _tuple10* env, struct
_tagged_string* x){ struct _tuple10 _temp169; struct Cyc_Position_Segment*
_temp170; struct Cyc_List_List* _temp172; struct _tuple10* _temp167= env;
_temp169=* _temp167; _LL173: _temp172= _temp169.f1; goto _LL171; _LL171:
_temp170= _temp169.f2; goto _LL168; _LL168: if( _temp172 == 0){ return(( struct
_tuple2*(*)( struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(({
struct _tagged_string _temp174=* x; xprintf("missing type for parameter %.*s",
_temp174.last_plus_one - _temp174.curr, _temp174.curr);}), _temp170);}{ void*
_temp175=( void*)(( struct Cyc_Absyn_Decl*) _temp172->hd)->r; struct Cyc_Absyn_Vardecl*
_temp181; _LL177: if(( unsigned int) _temp175 > 1u?(( struct _enum_struct*)
_temp175)->tag == Cyc_Absyn_Var_d_tag: 0){ _LL182: _temp181=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_d_struct*) _temp175)->f1; goto _LL178;} else{ goto _LL179;}
_LL179: goto _LL180; _LL178:{ void* _temp183=(* _temp181->name).f1; struct Cyc_List_List*
_temp193; struct Cyc_List_List* _temp195; _LL185: if( _temp183 == Cyc_Absyn_Loc_n){
goto _LL186;} else{ goto _LL187;} _LL187: if(( unsigned int) _temp183 > 1u?((
struct _enum_struct*) _temp183)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL194:
_temp193=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp183)->f1;
if( _temp193 == 0){ goto _LL188;} else{ goto _LL189;}} else{ goto _LL189;}
_LL189: if(( unsigned int) _temp183 > 1u?(( struct _enum_struct*) _temp183)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL196: _temp195=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp183)->f1; if( _temp195 == 0){ goto _LL190;} else{
goto _LL191;}} else{ goto _LL191;} _LL191: goto _LL192; _LL186: goto _LL184;
_LL188: goto _LL184; _LL190: goto _LL184; _LL192: Cyc_Parse_err(( struct
_tagged_string)({ char* _temp197=( char*)"module name not allowed on parameter";
struct _tagged_string _temp198; _temp198.curr= _temp197; _temp198.base= _temp197;
_temp198.last_plus_one= _temp197 + 37; _temp198;}), _temp170); goto _LL184;
_LL184:;} if( Cyc_String_zstrptrcmp((* _temp181->name).f2, x) == 0){ return({
struct _tuple2* _temp199=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2) *
1); _temp199[ 0]=({ struct _tuple2 _temp200; _temp200.f1=( struct Cyc_Core_Opt*)({
struct Cyc_Core_Opt* _temp201=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt) * 1); _temp201[ 0]=({ struct Cyc_Core_Opt _temp202; _temp202.v=(
void*)(* _temp181->name).f2; _temp202;}); _temp201;}); _temp200.f2= _temp181->tq;
_temp200.f3=( void*) _temp181->type; _temp200;}); _temp199;});} else{ return Cyc_Parse_get_param_type(({
struct _tuple10* _temp203=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10));
_temp203->f1= _temp172->tl; _temp203->f2= _temp170; _temp203;}), x);} _LL180:
return(( struct _tuple2*(*)( struct _tagged_string msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)(( struct _tagged_string)({ char* _temp204=( char*)"non-variable declaration";
struct _tagged_string _temp205; _temp205.curr= _temp204; _temp205.base= _temp204;
_temp205.last_plus_one= _temp204 + 25; _temp205;}),(( struct Cyc_Absyn_Decl*)
_temp172->hd)->loc); _LL176:;}} static int Cyc_Parse_is_typeparam( void* tm){
void* _temp206= tm; int _temp212; struct Cyc_Position_Segment* _temp214; struct
Cyc_List_List* _temp216; _LL208: if(( unsigned int) _temp206 > 1u?(( struct
_enum_struct*) _temp206)->tag == Cyc_Absyn_TypeParams_mod_tag: 0){ _LL217:
_temp216=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp206)->f1; goto _LL215; _LL215: _temp214=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp206)->f2; goto _LL213; _LL213:
_temp212=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp206)->f3; goto
_LL209;} else{ goto _LL210;} _LL210: goto _LL211; _LL209: return 1; _LL211:
return 0; _LL207:;} static void* Cyc_Parse_id2type( struct _tagged_string s,
struct Cyc_Absyn_Conref* k){ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({
char* _temp218=( char*)"`H"; struct _tagged_string _temp219; _temp219.curr=
_temp218; _temp219.base= _temp218; _temp219.last_plus_one= _temp218 + 3;
_temp219;})) == 0){ return Cyc_Absyn_HeapRgn;} else{ return({ struct Cyc_Absyn_VarType_struct*
_temp220=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp220->tag= Cyc_Absyn_VarType_tag; _temp220->f1=({ struct Cyc_Absyn_Tvar*
_temp221=( struct Cyc_Absyn_Tvar*) GC_malloc( sizeof( struct Cyc_Absyn_Tvar));
_temp221->name=({ struct _tagged_string* _temp222=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp222[ 0]= s; _temp222;});
_temp221->kind= k; _temp221;});( void*) _temp220;});}} static struct Cyc_Absyn_Tvar*
Cyc_Parse_typ2tvar( struct Cyc_Position_Segment* loc, void* t){ void* _temp223=
t; struct Cyc_Absyn_Tvar* _temp229; _LL225: if(( unsigned int) _temp223 > 5u?((
struct _enum_struct*) _temp223)->tag == Cyc_Absyn_VarType_tag: 0){ _LL230:
_temp229=( struct Cyc_Absyn_Tvar*)(( struct Cyc_Absyn_VarType_struct*) _temp223)->f1;
goto _LL226;} else{ goto _LL227;} _LL227: goto _LL228; _LL226: return _temp229;
_LL228: return(( struct Cyc_Absyn_Tvar*(*)( struct _tagged_string msg, struct
Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct _tagged_string)({ char*
_temp231=( char*)"expecting a list of type variables, not types"; struct
_tagged_string _temp232; _temp232.curr= _temp231; _temp232.base= _temp231;
_temp232.last_plus_one= _temp231 + 46; _temp232;}), loc); _LL224:;} static void*
Cyc_Parse_tvar2typ( struct Cyc_Absyn_Tvar* pr){ return({ struct Cyc_Absyn_VarType_struct*
_temp233=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp233->tag= Cyc_Absyn_VarType_tag; _temp233->f1= pr;( void*) _temp233;});}
static struct Cyc_List_List* Cyc_Parse_oldstyle2newstyle( struct Cyc_List_List*
tms, struct Cyc_List_List* tds, struct Cyc_Position_Segment* loc){ if( tds == 0){
return tms;} if( tms == 0){ return 0;}{ void* _temp234=( void*) tms->hd; void*
_temp240; _LL236: if(( unsigned int) _temp234 > 1u?(( struct _enum_struct*)
_temp234)->tag == Cyc_Absyn_Function_mod_tag: 0){ _LL241: _temp240=( void*)((
struct Cyc_Absyn_Function_mod_struct*) _temp234)->f1; goto _LL237;} else{ goto
_LL238;} _LL238: goto _LL239; _LL237: if( tms->tl == 0? 1:( Cyc_Parse_is_typeparam((
void*)( tms->tl)->hd)?( tms->tl)->tl == 0: 0)){ void* _temp242= _temp240; struct
Cyc_Core_Opt* _temp248; int _temp250; struct Cyc_List_List* _temp252; struct Cyc_Position_Segment*
_temp254; struct Cyc_List_List* _temp256; _LL244: if((( struct _enum_struct*)
_temp242)->tag == Cyc_Absyn_WithTypes_tag){ _LL253: _temp252=( struct Cyc_List_List*)((
struct Cyc_Absyn_WithTypes_struct*) _temp242)->f1; goto _LL251; _LL251: _temp250=(
int)(( struct Cyc_Absyn_WithTypes_struct*) _temp242)->f2; goto _LL249; _LL249:
_temp248=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_WithTypes_struct*) _temp242)->f3;
goto _LL245;} else{ goto _LL246;} _LL246: if((( struct _enum_struct*) _temp242)->tag
== Cyc_Absyn_NoTypes_tag){ _LL257: _temp256=( struct Cyc_List_List*)(( struct
Cyc_Absyn_NoTypes_struct*) _temp242)->f1; goto _LL255; _LL255: _temp254=( struct
Cyc_Position_Segment*)(( struct Cyc_Absyn_NoTypes_struct*) _temp242)->f2; goto
_LL247;} else{ goto _LL243;} _LL245: Cyc_Parse_warn(( struct _tagged_string)({
char* _temp258=( char*)"function declaration with both new- and old-style parameterdeclarations; ignoring old-style";
struct _tagged_string _temp259; _temp259.curr= _temp258; _temp259.base= _temp258;
_temp259.last_plus_one= _temp258 + 92; _temp259;}), loc); return tms; _LL247:((
void(*)( void(* f)( struct Cyc_List_List*, struct Cyc_Absyn_Decl*), struct Cyc_List_List*
env, struct Cyc_List_List* x)) Cyc_List_iter_c)( Cyc_Parse_only_vardecl,
_temp256, tds);{ struct _tuple10* env=({ struct _tuple10* _temp260=( struct
_tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp260->f1= tds; _temp260->f2=
loc; _temp260;}); return({ struct Cyc_List_List* _temp261=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp261->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp262=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp262->tag= Cyc_Absyn_Function_mod_tag; _temp262->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp263=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp263->tag= Cyc_Absyn_WithTypes_tag; _temp263->f1=(( struct Cyc_List_List*(*)(
struct _tuple2*(* f)( struct _tuple10*, struct _tagged_string*), struct _tuple10*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_get_param_type, env,
_temp256); _temp263->f2= 0; _temp263->f3= 0;( void*) _temp263;});( void*)
_temp262;}); _temp261->tl= 0; _temp261;});} _LL243:;} else{ return({ struct Cyc_List_List*
_temp264=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp264->hd=( void*)(( void*) tms->hd); _temp264->tl= Cyc_Parse_oldstyle2newstyle(
tms->tl, tds, loc); _temp264;});} _LL239: return({ struct Cyc_List_List*
_temp265=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp265->hd=( void*)(( void*) tms->hd); _temp265->tl= Cyc_Parse_oldstyle2newstyle(
tms->tl, tds, loc); _temp265;}); _LL235:;}} static struct Cyc_Absyn_Fndecl* Cyc_Parse_make_function(
struct Cyc_Core_Opt* dso, struct Cyc_Parse_Declarator* d, struct Cyc_List_List*
tds, struct Cyc_Absyn_Stmt* body, struct Cyc_Position_Segment* loc){ if( tds !=
0){ d=({ struct Cyc_Parse_Declarator* _temp266=( struct Cyc_Parse_Declarator*)
GC_malloc( sizeof( struct Cyc_Parse_Declarator)); _temp266->id= d->id; _temp266->tms=
Cyc_Parse_oldstyle2newstyle( d->tms, tds, loc); _temp266;});}{ void* sc= Cyc_Absyn_Public;
struct Cyc_List_List* tss= 0; struct Cyc_Absyn_Tqual* tq= Cyc_Absyn_empty_tqual();
int is_inline= 0; struct Cyc_List_List* atts= 0; if( dso != 0){ tss=(( struct
Cyc_Parse_Declaration_spec*) dso->v)->type_specs; tq=(( struct Cyc_Parse_Declaration_spec*)
dso->v)->tq; is_inline=(( struct Cyc_Parse_Declaration_spec*) dso->v)->is_inline;
atts=(( struct Cyc_Parse_Declaration_spec*) dso->v)->attributes; if((( struct
Cyc_Parse_Declaration_spec*) dso->v)->sc != 0){ void* _temp267=( void*)(((
struct Cyc_Parse_Declaration_spec*) dso->v)->sc)->v; _LL269: if( _temp267 == Cyc_Parse_Extern_sc){
goto _LL270;} else{ goto _LL271;} _LL271: if( _temp267 == Cyc_Parse_ExternC_sc){
goto _LL272;} else{ goto _LL273;} _LL273: if( _temp267 == Cyc_Parse_Static_sc){
goto _LL274;} else{ goto _LL275;} _LL275: goto _LL276; _LL270: sc= Cyc_Absyn_Extern;
goto _LL268; _LL272: sc= Cyc_Absyn_ExternC; goto _LL268; _LL274: sc= Cyc_Absyn_Static;
goto _LL268; _LL276: Cyc_Parse_err(( struct _tagged_string)({ char* _temp277=(
char*)"bad storage class on function"; struct _tagged_string _temp278; _temp278.curr=
_temp277; _temp278.base= _temp277; _temp278.last_plus_one= _temp277 + 30;
_temp278;}), loc); goto _LL268; _LL268:;}}{ struct Cyc_Core_Opt* _temp281; void*
_temp283; struct _tuple5 _temp279= Cyc_Parse_collapse_type_specifiers( tss, loc);
_LL284: _temp283= _temp279.f1; goto _LL282; _LL282: _temp281= _temp279.f2; goto
_LL280; _LL280: { struct Cyc_List_List* _temp287; struct Cyc_List_List* _temp289;
void* _temp291; struct Cyc_Absyn_Tqual* _temp293; struct _tuple6 _temp285= Cyc_Parse_apply_tms(
tq, _temp283, atts, d->tms); _LL294: _temp293= _temp285.f1; goto _LL292; _LL292:
_temp291= _temp285.f2; goto _LL290; _LL290: _temp289= _temp285.f3; goto _LL288;
_LL288: _temp287= _temp285.f4; goto _LL286; _LL286: if( _temp281 != 0){ Cyc_Parse_warn((
struct _tagged_string)({ char* _temp295=( char*)"nested type declaration within function prototype";
struct _tagged_string _temp296; _temp296.curr= _temp295; _temp296.base= _temp295;
_temp296.last_plus_one= _temp295 + 50; _temp296;}), loc);} if( _temp289 != 0){
Cyc_Parse_warn(( struct _tagged_string)({ char* _temp297=( char*)"bad type params, ignoring";
struct _tagged_string _temp298; _temp298.curr= _temp297; _temp298.base= _temp297;
_temp298.last_plus_one= _temp297 + 26; _temp298;}), loc);}{ void* _temp299=
_temp291; struct Cyc_Absyn_FnInfo _temp305; struct Cyc_List_List* _temp307; int
_temp309; struct Cyc_List_List* _temp311; void* _temp313; struct Cyc_Core_Opt*
_temp315; struct Cyc_List_List* _temp317; _LL301: if(( unsigned int) _temp299 >
5u?(( struct _enum_struct*) _temp299)->tag == Cyc_Absyn_FnType_tag: 0){ _LL306:
_temp305=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*) _temp299)->f1;
_LL318: _temp317=( struct Cyc_List_List*) _temp305.tvars; goto _LL316; _LL316:
_temp315=( struct Cyc_Core_Opt*) _temp305.effect; goto _LL314; _LL314: _temp313=(
void*) _temp305.ret_typ; goto _LL312; _LL312: _temp311=( struct Cyc_List_List*)
_temp305.args; goto _LL310; _LL310: _temp309=( int) _temp305.varargs; goto
_LL308; _LL308: _temp307=( struct Cyc_List_List*) _temp305.attributes; goto
_LL302;} else{ goto _LL303;} _LL303: goto _LL304; _LL302: { struct Cyc_List_List*
args2=(( struct Cyc_List_List*(*)( struct _tuple4*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_fnargspec_to_arg, loc, _temp311); return({ struct Cyc_Absyn_Fndecl*
_temp319=( struct Cyc_Absyn_Fndecl*) GC_malloc( sizeof( struct Cyc_Absyn_Fndecl));
_temp319->sc=( void*) sc; _temp319->name= d->id; _temp319->tvs= _temp317;
_temp319->is_inline= is_inline; _temp319->effect= _temp315; _temp319->ret_type=(
void*) _temp313; _temp319->args= args2; _temp319->varargs= _temp309; _temp319->body=
body; _temp319->cached_typ= 0; _temp319->param_vardecls= 0; _temp319->attributes=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
_temp307, _temp287); _temp319;});} _LL304: return(( struct Cyc_Absyn_Fndecl*(*)(
struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_string)({ char* _temp320=( char*)"declarator is not a function prototype";
struct _tagged_string _temp321; _temp321.curr= _temp320; _temp321.base= _temp320;
_temp321.last_plus_one= _temp320 + 39; _temp321;}), loc); _LL300:;}}}}} static
struct _tuple4* Cyc_Parse_fnargspec_to_arg( struct Cyc_Position_Segment* loc,
struct _tuple2* t){ if((* t).f1 == 0){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp322=( char*)"missing argument variable in function prototype"; struct
_tagged_string _temp323; _temp323.curr= _temp322; _temp323.base= _temp322;
_temp323.last_plus_one= _temp322 + 48; _temp323;}), loc); return({ struct
_tuple4* _temp324=( struct _tuple4*) GC_malloc( sizeof( struct _tuple4));
_temp324->f1=({ struct _tagged_string* _temp325=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp325[ 0]=( struct
_tagged_string)({ char* _temp326=( char*)"?"; struct _tagged_string _temp327;
_temp327.curr= _temp326; _temp327.base= _temp326; _temp327.last_plus_one=
_temp326 + 2; _temp327;}); _temp325;}); _temp324->f2=(* t).f2; _temp324->f3=(* t).f3;
_temp324;});} else{ return({ struct _tuple4* _temp328=( struct _tuple4*)
GC_malloc( sizeof( struct _tuple4)); _temp328->f1=( struct _tagged_string*)((* t).f1)->v;
_temp328->f2=(* t).f2; _temp328->f3=(* t).f3; _temp328;});}} static char
_temp331[ 52u]="at most one type may appear within a type specifier"; static
struct _tagged_string Cyc_Parse_msg1=( struct _tagged_string){ _temp331,
_temp331, _temp331 + 52u}; static char _temp334[ 63u]="const or volatile may appear only once within a type specifier";
static struct _tagged_string Cyc_Parse_msg2=( struct _tagged_string){ _temp334,
_temp334, _temp334 + 63u}; static char _temp337[ 50u]="type specifier includes more than one declaration";
static struct _tagged_string Cyc_Parse_msg3=( struct _tagged_string){ _temp337,
_temp337, _temp337 + 50u}; static char _temp340[ 60u]="sign specifier may appear only once within a type specifier";
static struct _tagged_string Cyc_Parse_msg4=( struct _tagged_string){ _temp340,
_temp340, _temp340 + 60u}; static struct _tuple5 Cyc_Parse_collapse_type_specifiers(
struct Cyc_List_List* ts, struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt*
declopt= 0; int seen_type= 0; int seen_sign= 0; int seen_size= 0; void* t= Cyc_Absyn_VoidType;
void* sz= Cyc_Absyn_B4; void* sgn= Cyc_Absyn_Signed; struct Cyc_Position_Segment*
last_loc= loc; for( 0; ts != 0; ts= ts->tl){ void* _temp341=( void*) ts->hd;
struct Cyc_Position_Segment* _temp355; void* _temp357; struct Cyc_Position_Segment*
_temp359; struct Cyc_Position_Segment* _temp361; struct Cyc_Position_Segment*
_temp363; struct Cyc_Position_Segment* _temp365; struct Cyc_Absyn_Decl* _temp367;
_LL343: if((( struct _enum_struct*) _temp341)->tag == Cyc_Parse_Type_spec_tag){
_LL358: _temp357=( void*)(( struct Cyc_Parse_Type_spec_struct*) _temp341)->f1;
goto _LL356; _LL356: _temp355=( struct Cyc_Position_Segment*)(( struct Cyc_Parse_Type_spec_struct*)
_temp341)->f2; goto _LL344;} else{ goto _LL345;} _LL345: if((( struct
_enum_struct*) _temp341)->tag == Cyc_Parse_Signed_spec_tag){ _LL360: _temp359=(
struct Cyc_Position_Segment*)(( struct Cyc_Parse_Signed_spec_struct*) _temp341)->f1;
goto _LL346;} else{ goto _LL347;} _LL347: if((( struct _enum_struct*) _temp341)->tag
== Cyc_Parse_Unsigned_spec_tag){ _LL362: _temp361=( struct Cyc_Position_Segment*)((
struct Cyc_Parse_Unsigned_spec_struct*) _temp341)->f1; goto _LL348;} else{ goto
_LL349;} _LL349: if((( struct _enum_struct*) _temp341)->tag == Cyc_Parse_Short_spec_tag){
_LL364: _temp363=( struct Cyc_Position_Segment*)(( struct Cyc_Parse_Short_spec_struct*)
_temp341)->f1; goto _LL350;} else{ goto _LL351;} _LL351: if((( struct
_enum_struct*) _temp341)->tag == Cyc_Parse_Long_spec_tag){ _LL366: _temp365=(
struct Cyc_Position_Segment*)(( struct Cyc_Parse_Long_spec_struct*) _temp341)->f1;
goto _LL352;} else{ goto _LL353;} _LL353: if((( struct _enum_struct*) _temp341)->tag
== Cyc_Parse_Decl_spec_tag){ _LL368: _temp367=( struct Cyc_Absyn_Decl*)(( struct
Cyc_Parse_Decl_spec_struct*) _temp341)->f1; goto _LL354;} else{ goto _LL342;}
_LL344: if( seen_type){ Cyc_Parse_err( Cyc_Parse_msg1, _temp355);} last_loc=
_temp355; seen_type= 1; t= _temp357; goto _LL342; _LL346: if( seen_sign){ Cyc_Parse_err(
Cyc_Parse_msg4, _temp359);} if( seen_type){ Cyc_Parse_err(( struct
_tagged_string)({ char* _temp369=( char*)"signed qualifier must come before type";
struct _tagged_string _temp370; _temp370.curr= _temp369; _temp370.base= _temp369;
_temp370.last_plus_one= _temp369 + 39; _temp370;}), _temp359);} last_loc=
_temp359; seen_sign= 1; sgn= Cyc_Absyn_Signed; goto _LL342; _LL348: if(
seen_sign){ Cyc_Parse_err( Cyc_Parse_msg4, _temp361);} if( seen_type){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp371=( char*)"signed qualifier must come before type";
struct _tagged_string _temp372; _temp372.curr= _temp371; _temp372.base= _temp371;
_temp372.last_plus_one= _temp371 + 39; _temp372;}), _temp361);} last_loc=
_temp361; seen_sign= 1; sgn= Cyc_Absyn_Unsigned; goto _LL342; _LL350: if(
seen_size){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp373=( char*)"integral size may appear only once within a type specifier";
struct _tagged_string _temp374; _temp374.curr= _temp373; _temp374.base= _temp373;
_temp374.last_plus_one= _temp373 + 59; _temp374;}), _temp363);} if( seen_type){
Cyc_Parse_err(( struct _tagged_string)({ char* _temp375=( char*)"short modifier must come before base type";
struct _tagged_string _temp376; _temp376.curr= _temp375; _temp376.base= _temp375;
_temp376.last_plus_one= _temp375 + 42; _temp376;}), _temp363);} last_loc=
_temp363; seen_size= 1; sz= Cyc_Absyn_B2; goto _LL342; _LL352: if( seen_type){
Cyc_Parse_err(( struct _tagged_string)({ char* _temp377=( char*)"long modifier must come before base type";
struct _tagged_string _temp378; _temp378.curr= _temp377; _temp378.base= _temp377;
_temp378.last_plus_one= _temp377 + 41; _temp378;}), _temp365);} if( seen_size){
void* _temp379= sz; _LL381: if( _temp379 == Cyc_Absyn_B4){ goto _LL382;} else{
goto _LL383;} _LL383: goto _LL384; _LL382: sz= Cyc_Absyn_B8; goto _LL380; _LL384:
Cyc_Parse_err(( struct _tagged_string)({ char* _temp385=( char*)"extra long in type specifier";
struct _tagged_string _temp386; _temp386.curr= _temp385; _temp386.base= _temp385;
_temp386.last_plus_one= _temp385 + 29; _temp386;}), _temp365); goto _LL380;
_LL380:;} last_loc= _temp365; seen_size= 1; goto _LL342; _LL354: last_loc=
_temp367->loc; if( declopt == 0? ! seen_type: 0){ seen_type= 1;{ void* _temp387=(
void*) _temp367->r; struct Cyc_Absyn_Structdecl* _temp399; struct Cyc_Absyn_Enumdecl*
_temp401; struct Cyc_Absyn_Xenumdecl* _temp403; _LL389: if(( unsigned int)
_temp387 > 1u?(( struct _enum_struct*) _temp387)->tag == Cyc_Absyn_Struct_d_tag:
0){ _LL400: _temp399=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*)
_temp387)->f1; goto _LL390;} else{ goto _LL391;} _LL391: if(( unsigned int)
_temp387 > 1u?(( struct _enum_struct*) _temp387)->tag == Cyc_Absyn_Enum_d_tag: 0){
_LL402: _temp401=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*)
_temp387)->f1; goto _LL392;} else{ goto _LL393;} _LL393: if(( unsigned int)
_temp387 > 1u?(( struct _enum_struct*) _temp387)->tag == Cyc_Absyn_Xenum_d_tag:
0){ _LL404: _temp403=( struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_d_struct*)
_temp387)->f1; goto _LL394;} else{ goto _LL395;} _LL395: if( _temp387 == Cyc_Absyn_Union_d){
goto _LL396;} else{ goto _LL397;} _LL397: goto _LL398; _LL390: { struct Cyc_List_List*
args=(( struct Cyc_List_List*(*)( void*(* f)( struct Cyc_Absyn_Tvar*), struct
Cyc_List_List* x)) Cyc_List_map)( Cyc_Parse_tvar2typ, _temp399->tvs); t=({
struct Cyc_Absyn_StructType_struct* _temp405=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp405->tag= Cyc_Absyn_StructType_tag;
_temp405->f1=( struct _tuple1*)(( struct _tuple1*)( _temp399->name)->v);
_temp405->f2= args; _temp405->f3= 0;( void*) _temp405;}); if( _temp399->fields
!= 0){ declopt=({ struct Cyc_Core_Opt* _temp406=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp406->v=( void*) _temp367;
_temp406;});} goto _LL388;} _LL392: { struct Cyc_List_List* args=(( struct Cyc_List_List*(*)(
void*(* f)( struct Cyc_Absyn_Tvar*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_Parse_tvar2typ, _temp401->tvs); t=({ struct Cyc_Absyn_EnumType_struct*
_temp407=( struct Cyc_Absyn_EnumType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct));
_temp407->tag= Cyc_Absyn_EnumType_tag; _temp407->f1=( struct _tuple1*)(( struct
_tuple1*)( _temp401->name)->v); _temp407->f2= args; _temp407->f3= 0;( void*)
_temp407;}); if( _temp401->fields != 0){ declopt=({ struct Cyc_Core_Opt*
_temp408=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp408->v=( void*) _temp367; _temp408;});} goto _LL388;} _LL394: t=({ struct
Cyc_Absyn_XenumType_struct* _temp409=( struct Cyc_Absyn_XenumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XenumType_struct)); _temp409->tag= Cyc_Absyn_XenumType_tag;
_temp409->f1= _temp403->name; _temp409->f2= 0;( void*) _temp409;}); if( _temp403->fields
!= 0){ declopt=({ struct Cyc_Core_Opt* _temp410=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp410->v=( void*) _temp367;
_temp410;});} goto _LL388; _LL396: t= Cyc_Absyn_UnionType; goto _LL388; _LL398:((
void(*)( struct _tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)((
struct _tagged_string)({ char* _temp411=( char*)"bad declaration within type specifier";
struct _tagged_string _temp412; _temp412.curr= _temp411; _temp412.base= _temp411;
_temp412.last_plus_one= _temp411 + 38; _temp412;}), _temp367->loc); goto _LL388;
_LL388:;}} else{ Cyc_Parse_err( Cyc_Parse_msg3, _temp367->loc);} goto _LL342;
_LL342:;} if( ! seen_type){ if( ! seen_sign? ! seen_size: 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp413=( char*)"missing type withing specifier";
struct _tagged_string _temp414; _temp414.curr= _temp413; _temp414.base= _temp413;
_temp414.last_plus_one= _temp413 + 31; _temp414;}), last_loc);} t=({ struct Cyc_Absyn_IntType_struct*
_temp415=( struct Cyc_Absyn_IntType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp415->tag= Cyc_Absyn_IntType_tag; _temp415->f1=( void*) sgn; _temp415->f2=(
void*) sz;( void*) _temp415;});} else{ if( seen_sign){ void* _temp416= t; void*
_temp422; void* _temp424; _LL418: if(( unsigned int) _temp416 > 5u?(( struct
_enum_struct*) _temp416)->tag == Cyc_Absyn_IntType_tag: 0){ _LL425: _temp424=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp416)->f1; goto _LL423; _LL423:
_temp422=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp416)->f2; goto _LL419;}
else{ goto _LL420;} _LL420: goto _LL421; _LL419: t=({ struct Cyc_Absyn_IntType_struct*
_temp426=( struct Cyc_Absyn_IntType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct));
_temp426->tag= Cyc_Absyn_IntType_tag; _temp426->f1=( void*) sgn; _temp426->f2=(
void*) _temp422;( void*) _temp426;}); goto _LL417; _LL421: Cyc_Parse_err((
struct _tagged_string)({ char* _temp427=( char*)"sign specification on non-integral type";
struct _tagged_string _temp428; _temp428.curr= _temp427; _temp428.base= _temp427;
_temp428.last_plus_one= _temp427 + 40; _temp428;}), last_loc); goto _LL417;
_LL417:;} if( seen_size){ void* _temp429= t; void* _temp435; void* _temp437;
_LL431: if(( unsigned int) _temp429 > 5u?(( struct _enum_struct*) _temp429)->tag
== Cyc_Absyn_IntType_tag: 0){ _LL438: _temp437=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp429)->f1; goto _LL436; _LL436: _temp435=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp429)->f2; goto _LL432;} else{ goto _LL433;} _LL433: goto _LL434; _LL432: t=({
struct Cyc_Absyn_IntType_struct* _temp439=( struct Cyc_Absyn_IntType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IntType_struct)); _temp439->tag= Cyc_Absyn_IntType_tag;
_temp439->f1=( void*) _temp437; _temp439->f2=( void*) sz;( void*) _temp439;});
goto _LL430; _LL434: Cyc_Parse_err(( struct _tagged_string)({ char* _temp440=(
char*)"size qualifier on non-integral type"; struct _tagged_string _temp441;
_temp441.curr= _temp440; _temp441.base= _temp440; _temp441.last_plus_one=
_temp440 + 36; _temp441;}), last_loc); goto _LL430; _LL430:;}} return({ struct
_tuple5 _temp442; _temp442.f1= t; _temp442.f2= declopt; _temp442;});} static
struct Cyc_List_List* Cyc_Parse_apply_tmss( struct Cyc_Absyn_Tqual* tq, void* t,
struct Cyc_List_List* ds, struct Cyc_List_List* shared_atts){ if( ds == 0){
return 0;}{ struct Cyc_Parse_Declarator* d=( struct Cyc_Parse_Declarator*) ds->hd;
struct _tuple1* q= d->id; struct Cyc_List_List* _temp445; struct Cyc_List_List*
_temp447; void* _temp449; struct Cyc_Absyn_Tqual* _temp451; struct _tuple6
_temp443= Cyc_Parse_apply_tms( tq, t, shared_atts, d->tms); _LL452: _temp451=
_temp443.f1; goto _LL450; _LL450: _temp449= _temp443.f2; goto _LL448; _LL448:
_temp447= _temp443.f3; goto _LL446; _LL446: _temp445= _temp443.f4; goto _LL444;
_LL444: return({ struct Cyc_List_List* _temp453=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp453->hd=( void*)({ struct
_tuple7* _temp454=( struct _tuple7*) GC_malloc( sizeof( struct _tuple7));
_temp454->f1= q; _temp454->f2= _temp451; _temp454->f3= _temp449; _temp454->f4=
_temp447; _temp454->f5= _temp445; _temp454;}); _temp453->tl= Cyc_Parse_apply_tmss(
_temp451, t, ds->tl, shared_atts); _temp453;});}} static int Cyc_Parse_fn_type_att(
void* a){ void* _temp455= a; int _temp469; _LL457: if(( unsigned int) _temp455 >
15u?(( struct _enum_struct*) _temp455)->tag == Cyc_Absyn_Regparm_att_tag: 0){
_LL470: _temp469=( int)(( struct Cyc_Absyn_Regparm_att_struct*) _temp455)->f1;
goto _LL458;} else{ goto _LL459;} _LL459: if( _temp455 == Cyc_Absyn_Stdcall_att){
goto _LL460;} else{ goto _LL461;} _LL461: if( _temp455 == Cyc_Absyn_Cdecl_att){
goto _LL462;} else{ goto _LL463;} _LL463: if( _temp455 == Cyc_Absyn_Noreturn_att){
goto _LL464;} else{ goto _LL465;} _LL465: if( _temp455 == Cyc_Absyn_Const_att){
goto _LL466;} else{ goto _LL467;} _LL467: goto _LL468; _LL458: goto _LL460;
_LL460: goto _LL462; _LL462: goto _LL464; _LL464: goto _LL466; _LL466: return 1;
_LL468: return 0; _LL456:;} static struct _tuple6 Cyc_Parse_apply_tms( struct
Cyc_Absyn_Tqual* tq, void* t, struct Cyc_List_List* atts, struct Cyc_List_List*
tms){ if( tms == 0){ return({ struct _tuple6 _temp471; _temp471.f1= tq; _temp471.f2=
t; _temp471.f3= 0; _temp471.f4= atts; _temp471;});}{ void* _temp472=( void*) tms->hd;
struct Cyc_Absyn_Exp* _temp486; void* _temp488; int _temp490; struct Cyc_Position_Segment*
_temp492; struct Cyc_List_List* _temp494; struct Cyc_Absyn_Tqual* _temp496; void*
_temp498; void* _temp500; struct Cyc_List_List* _temp502; struct Cyc_Position_Segment*
_temp504; _LL474: if( _temp472 == Cyc_Absyn_Carray_mod){ goto _LL475;} else{
goto _LL476;} _LL476: if(( unsigned int) _temp472 > 1u?(( struct _enum_struct*)
_temp472)->tag == Cyc_Absyn_ConstArray_mod_tag: 0){ _LL487: _temp486=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ConstArray_mod_struct*) _temp472)->f1; goto
_LL477;} else{ goto _LL478;} _LL478: if(( unsigned int) _temp472 > 1u?(( struct
_enum_struct*) _temp472)->tag == Cyc_Absyn_Function_mod_tag: 0){ _LL489:
_temp488=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp472)->f1; goto
_LL479;} else{ goto _LL480;} _LL480: if(( unsigned int) _temp472 > 1u?(( struct
_enum_struct*) _temp472)->tag == Cyc_Absyn_TypeParams_mod_tag: 0){ _LL495:
_temp494=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp472)->f1; goto _LL493; _LL493: _temp492=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp472)->f2; goto _LL491; _LL491:
_temp490=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp472)->f3; goto
_LL481;} else{ goto _LL482;} _LL482: if(( unsigned int) _temp472 > 1u?(( struct
_enum_struct*) _temp472)->tag == Cyc_Absyn_Pointer_mod_tag: 0){ _LL501: _temp500=(
void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp472)->f1; goto _LL499;
_LL499: _temp498=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp472)->f2;
goto _LL497; _LL497: _temp496=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp472)->f3; goto _LL483;} else{ goto _LL484;} _LL484: if(( unsigned int)
_temp472 > 1u?(( struct _enum_struct*) _temp472)->tag == Cyc_Absyn_Attributes_mod_tag:
0){ _LL505: _temp504=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp472)->f1; goto _LL503; _LL503: _temp502=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Attributes_mod_struct*) _temp472)->f2; goto _LL485;} else{ goto _LL473;}
_LL475: return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(),({ struct Cyc_Absyn_ArrayType_struct*
_temp506=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp506->tag= Cyc_Absyn_ArrayType_tag; _temp506->f1=( void*) t; _temp506->f2=
tq; _temp506->f3= Cyc_Absyn_uint_exp( 0, 0);( void*) _temp506;}), atts, tms->tl);
_LL477: return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(),({ struct Cyc_Absyn_ArrayType_struct*
_temp507=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp507->tag= Cyc_Absyn_ArrayType_tag; _temp507->f1=( void*) t; _temp507->f2=
tq; _temp507->f3= _temp486;( void*) _temp507;}), atts, tms->tl); _LL479: { void*
_temp508= _temp488; struct Cyc_Core_Opt* _temp514; int _temp516; struct Cyc_List_List*
_temp518; struct Cyc_Position_Segment* _temp520; struct Cyc_List_List* _temp522;
_LL510: if((( struct _enum_struct*) _temp508)->tag == Cyc_Absyn_WithTypes_tag){
_LL519: _temp518=( struct Cyc_List_List*)(( struct Cyc_Absyn_WithTypes_struct*)
_temp508)->f1; goto _LL517; _LL517: _temp516=( int)(( struct Cyc_Absyn_WithTypes_struct*)
_temp508)->f2; goto _LL515; _LL515: _temp514=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_WithTypes_struct*) _temp508)->f3; goto _LL511;} else{ goto _LL512;}
_LL512: if((( struct _enum_struct*) _temp508)->tag == Cyc_Absyn_NoTypes_tag){
_LL523: _temp522=( struct Cyc_List_List*)(( struct Cyc_Absyn_NoTypes_struct*)
_temp508)->f1; goto _LL521; _LL521: _temp520=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_NoTypes_struct*) _temp508)->f2; goto _LL513;} else{ goto _LL509;}
_LL511: { struct Cyc_List_List* typvars= 0; struct Cyc_List_List* fn_atts= 0;
struct Cyc_List_List* new_atts= 0;{ struct Cyc_List_List* as= atts; for( 0; as
!= 0; as= as->tl){ if( Cyc_Parse_fn_type_att(( void*) as->hd)){ fn_atts=({
struct Cyc_List_List* _temp524=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp524->hd=( void*)(( void*) as->hd); _temp524->tl=
fn_atts; _temp524;});} else{ new_atts=({ struct Cyc_List_List* _temp525=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp525->hd=( void*)((
void*) as->hd); _temp525->tl= new_atts; _temp525;});}}} if( tms->tl != 0){ void*
_temp526=( void*)( tms->tl)->hd; int _temp532; struct Cyc_Position_Segment*
_temp534; struct Cyc_List_List* _temp536; _LL528: if(( unsigned int) _temp526 >
1u?(( struct _enum_struct*) _temp526)->tag == Cyc_Absyn_TypeParams_mod_tag: 0){
_LL537: _temp536=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp526)->f1; goto _LL535; _LL535: _temp534=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp526)->f2; goto _LL533; _LL533:
_temp532=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp526)->f3; goto
_LL529;} else{ goto _LL530;} _LL530: goto _LL531; _LL529: typvars= _temp536; tms=
tms->tl; goto _LL527; _LL531: goto _LL527; _LL527:;} if(((( ! _temp516? _temp518
!= 0: 0)? _temp518->tl == 0: 0)?(*(( struct _tuple2*) _temp518->hd)).f1 == 0: 0)?(*((
struct _tuple2*) _temp518->hd)).f3 == Cyc_Absyn_VoidType: 0){ _temp518= 0;
_temp516= 0;} t= Cyc_Parse_array2ptr( t);(( void(*)( void(* f)( struct _tuple2*),
struct Cyc_List_List* x)) Cyc_List_iter)( Cyc_Parse_arg_array2ptr, _temp518);
return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(), Cyc_Absyn_function_typ(
typvars, _temp514, t, _temp518, _temp516, fn_atts), new_atts, tms->tl);} _LL513:(
void) _throw((( struct _xenum_struct*(*)( struct _tagged_string msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)(( struct _tagged_string)({ char* _temp538=( char*)"function declaration without parameter types";
struct _tagged_string _temp539; _temp539.curr= _temp538; _temp539.base= _temp538;
_temp539.last_plus_one= _temp538 + 45; _temp539;}), _temp520)); _LL509:;} _LL481:
if( tms->tl == 0){ return({ struct _tuple6 _temp540; _temp540.f1= tq; _temp540.f2=
t; _temp540.f3= _temp494; _temp540.f4= atts; _temp540;});}( void) _throw(((
struct _xenum_struct*(*)( struct _tagged_string msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)(( struct _tagged_string)({ char* _temp541=( char*)"type parameters must appear before function arguments in declarator";
struct _tagged_string _temp542; _temp542.curr= _temp541; _temp542.base= _temp541;
_temp542.last_plus_one= _temp541 + 68; _temp542;}), _temp492)); _LL483: { void*
_temp543= _temp500; struct Cyc_Absyn_Exp* _temp551; struct Cyc_Absyn_Exp*
_temp553; _LL545: if(( unsigned int) _temp543 > 1u?(( struct _enum_struct*)
_temp543)->tag == Cyc_Absyn_NonNullable_ps_tag: 0){ _LL552: _temp551=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp543)->f1; goto
_LL546;} else{ goto _LL547;} _LL547: if(( unsigned int) _temp543 > 1u?(( struct
_enum_struct*) _temp543)->tag == Cyc_Absyn_Nullable_ps_tag: 0){ _LL554: _temp553=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Nullable_ps_struct*) _temp543)->f1;
goto _LL548;} else{ goto _LL549;} _LL549: if( _temp543 == Cyc_Absyn_TaggedArray_ps){
goto _LL550;} else{ goto _LL544;} _LL546: return Cyc_Parse_apply_tms( _temp496,
Cyc_Absyn_atb_typ( t, _temp498, tq,({ struct Cyc_Absyn_Upper_b_struct* _temp555=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp555->tag= Cyc_Absyn_Upper_b_tag; _temp555->f1= _temp551;( void*) _temp555;})),
atts, tms->tl); _LL548: return Cyc_Parse_apply_tms( _temp496, Cyc_Absyn_starb_typ(
t, _temp498, tq,({ struct Cyc_Absyn_Upper_b_struct* _temp556=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp556->tag= Cyc_Absyn_Upper_b_tag;
_temp556->f1= _temp553;( void*) _temp556;})), atts, tms->tl); _LL550: return Cyc_Parse_apply_tms(
_temp496, Cyc_Absyn_tagged_typ( t, _temp498, tq), atts, tms->tl); _LL544:;}
_LL485: return Cyc_Parse_apply_tms( tq, t,(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( atts, _temp502),
tms->tl); _LL473:;}} void* Cyc_Parse_speclist2typ( struct Cyc_List_List* tss,
struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt* _temp559; void* _temp561;
struct _tuple5 _temp557= Cyc_Parse_collapse_type_specifiers( tss, loc); _LL562:
_temp561= _temp557.f1; goto _LL560; _LL560: _temp559= _temp557.f2; goto _LL558;
_LL558: if( _temp559 != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char*
_temp563=( char*)"ignoring nested type declaration(s) in specifier list"; struct
_tagged_string _temp564; _temp564.curr= _temp563; _temp564.base= _temp563;
_temp564.last_plus_one= _temp563 + 54; _temp564;}), loc);} return _temp561;}
static struct Cyc_Absyn_Stmt* Cyc_Parse_flatten_decl( struct Cyc_Absyn_Decl* d,
struct Cyc_Absyn_Stmt* s){ return Cyc_Absyn_new_stmt(({ struct Cyc_Absyn_Decl_s_struct*
_temp565=( struct Cyc_Absyn_Decl_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
_temp565->tag= Cyc_Absyn_Decl_s_tag; _temp565->f1= d; _temp565->f2= s;( void*)
_temp565;}), Cyc_Position_segment_join( d->loc, s->loc));} static struct Cyc_Absyn_Stmt*
Cyc_Parse_flatten_declarations( struct Cyc_List_List* ds, struct Cyc_Absyn_Stmt*
s){ return(( struct Cyc_Absyn_Stmt*(*)( struct Cyc_Absyn_Stmt*(* f)( struct Cyc_Absyn_Decl*,
struct Cyc_Absyn_Stmt*), struct Cyc_List_List* x, struct Cyc_Absyn_Stmt* accum))
Cyc_List_fold_right)( Cyc_Parse_flatten_decl, ds, s);} static struct Cyc_List_List*
Cyc_Parse_make_declarations( struct Cyc_Parse_Declaration_spec* ds, struct Cyc_List_List*
ids, struct Cyc_Position_Segment* loc){ struct Cyc_List_List* tss= ds->type_specs;
struct Cyc_Absyn_Tqual* tq= ds->tq; int istypedef= 0; void* s= Cyc_Absyn_Public;
struct Cyc_List_List* atts= ds->attributes; if( ds->is_inline){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp566=( char*)"inline is only allowed on function definitions";
struct _tagged_string _temp567; _temp567.curr= _temp566; _temp567.base= _temp566;
_temp567.last_plus_one= _temp566 + 47; _temp567;}), loc);} if( tss == 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp568=( char*)"missing type specifiers in declaration";
struct _tagged_string _temp569; _temp569.curr= _temp568; _temp569.base= _temp568;
_temp569.last_plus_one= _temp568 + 39; _temp569;}), loc); return 0;} if( ds->sc
!= 0){ void* _temp570=( void*)( ds->sc)->v; _LL572: if( _temp570 == Cyc_Parse_Typedef_sc){
goto _LL573;} else{ goto _LL574;} _LL574: if( _temp570 == Cyc_Parse_Extern_sc){
goto _LL575;} else{ goto _LL576;} _LL576: if( _temp570 == Cyc_Parse_ExternC_sc){
goto _LL577;} else{ goto _LL578;} _LL578: if( _temp570 == Cyc_Parse_Static_sc){
goto _LL579;} else{ goto _LL580;} _LL580: if( _temp570 == Cyc_Parse_Auto_sc){
goto _LL581;} else{ goto _LL582;} _LL582: if( _temp570 == Cyc_Parse_Register_sc){
goto _LL583;} else{ goto _LL584;} _LL584: if( _temp570 == Cyc_Parse_Abstract_sc){
goto _LL585;} else{ goto _LL571;} _LL573: istypedef= 1; goto _LL571; _LL575: s=
Cyc_Absyn_Extern; goto _LL571; _LL577: s= Cyc_Absyn_ExternC; goto _LL571; _LL579:
s= Cyc_Absyn_Static; goto _LL571; _LL581: s= Cyc_Absyn_Public; goto _LL571;
_LL583: s= Cyc_Absyn_Public; goto _LL571; _LL585: s= Cyc_Absyn_Abstract; goto
_LL571; _LL571:;}{ struct Cyc_List_List* _temp588; struct Cyc_List_List*
_temp590; struct _tuple0 _temp586=(( struct _tuple0(*)( struct Cyc_List_List* x))
Cyc_List_split)( ids); _LL591: _temp590= _temp586.f1; goto _LL589; _LL589:
_temp588= _temp586.f2; goto _LL587; _LL587: { int exps_empty= 1;{ struct Cyc_List_List*
es= _temp588; for( 0; es != 0; es= es->tl){ if(( struct Cyc_Absyn_Exp*) es->hd
!= 0){ exps_empty= 0; break;}}}{ struct _tuple5 ts_info= Cyc_Parse_collapse_type_specifiers(
tss, loc); if( _temp590 == 0){ struct Cyc_Core_Opt* _temp594; void* _temp596;
struct _tuple5 _temp592= ts_info; _LL597: _temp596= _temp592.f1; goto _LL595;
_LL595: _temp594= _temp592.f2; goto _LL593; _LL593: if( _temp594 != 0){ struct
Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*) _temp594->v;{ void* _temp598=( void*)
d->r; struct Cyc_Absyn_Structdecl* _temp608; struct Cyc_Absyn_Enumdecl* _temp610;
struct Cyc_Absyn_Xenumdecl* _temp612; _LL600: if(( unsigned int) _temp598 > 1u?((
struct _enum_struct*) _temp598)->tag == Cyc_Absyn_Struct_d_tag: 0){ _LL609:
_temp608=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*)
_temp598)->f1; goto _LL601;} else{ goto _LL602;} _LL602: if(( unsigned int)
_temp598 > 1u?(( struct _enum_struct*) _temp598)->tag == Cyc_Absyn_Enum_d_tag: 0){
_LL611: _temp610=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*)
_temp598)->f1; goto _LL603;} else{ goto _LL604;} _LL604: if(( unsigned int)
_temp598 > 1u?(( struct _enum_struct*) _temp598)->tag == Cyc_Absyn_Xenum_d_tag:
0){ _LL613: _temp612=( struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_d_struct*)
_temp598)->f1; goto _LL605;} else{ goto _LL606;} _LL606: goto _LL607; _LL601:(
void*)( _temp608->sc=( void*) s); _temp608->attributes= atts; goto _LL599;
_LL603:( void*)( _temp610->sc=( void*) s); if( atts != 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp614=( char*)"bad attributes on enum"; struct
_tagged_string _temp615; _temp615.curr= _temp614; _temp615.base= _temp614;
_temp615.last_plus_one= _temp614 + 23; _temp615;}), loc);} goto _LL599; _LL605:(
void*)( _temp612->sc=( void*) s); if( atts != 0){ Cyc_Parse_err(( struct
_tagged_string)({ char* _temp616=( char*)"bad attributes on xenum"; struct
_tagged_string _temp617; _temp617.curr= _temp616; _temp617.base= _temp616;
_temp617.last_plus_one= _temp616 + 24; _temp617;}), loc);} goto _LL599; _LL607:
Cyc_Parse_err(( struct _tagged_string)({ char* _temp618=( char*)"bad declaration";
struct _tagged_string _temp619; _temp619.curr= _temp618; _temp619.base= _temp618;
_temp619.last_plus_one= _temp618 + 16; _temp619;}), loc); return 0; _LL599:;}
return({ struct Cyc_List_List* _temp620=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp620->hd=( void*) d; _temp620->tl= 0;
_temp620;});} else{ void* _temp621= _temp596; struct Cyc_Absyn_Structdecl**
_temp633; struct Cyc_List_List* _temp635; struct _tuple1* _temp637; struct Cyc_Absyn_Enumdecl**
_temp639; struct Cyc_List_List* _temp641; struct _tuple1* _temp643; struct Cyc_Absyn_Xenumdecl**
_temp645; struct _tuple1* _temp647; _LL623: if(( unsigned int) _temp621 > 5u?((
struct _enum_struct*) _temp621)->tag == Cyc_Absyn_StructType_tag: 0){ _LL638:
_temp637=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp621)->f1;
goto _LL636; _LL636: _temp635=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp621)->f2; goto _LL634; _LL634: _temp633=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp621)->f3; goto _LL624;} else{ goto
_LL625;} _LL625: if(( unsigned int) _temp621 > 5u?(( struct _enum_struct*)
_temp621)->tag == Cyc_Absyn_EnumType_tag: 0){ _LL644: _temp643=( struct _tuple1*)((
struct Cyc_Absyn_EnumType_struct*) _temp621)->f1; goto _LL642; _LL642: _temp641=(
struct Cyc_List_List*)(( struct Cyc_Absyn_EnumType_struct*) _temp621)->f2; goto
_LL640; _LL640: _temp639=( struct Cyc_Absyn_Enumdecl**)(( struct Cyc_Absyn_EnumType_struct*)
_temp621)->f3; goto _LL626;} else{ goto _LL627;} _LL627: if(( unsigned int)
_temp621 > 5u?(( struct _enum_struct*) _temp621)->tag == Cyc_Absyn_XenumType_tag:
0){ _LL648: _temp647=( struct _tuple1*)(( struct Cyc_Absyn_XenumType_struct*)
_temp621)->f1; goto _LL646; _LL646: _temp645=( struct Cyc_Absyn_Xenumdecl**)((
struct Cyc_Absyn_XenumType_struct*) _temp621)->f2; goto _LL628;} else{ goto
_LL629;} _LL629: if( _temp621 == Cyc_Absyn_UnionType){ goto _LL630;} else{ goto
_LL631;} _LL631: goto _LL632; _LL624: { struct Cyc_List_List* ts2=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc,
_temp635); struct Cyc_Absyn_Structdecl* sd=({ struct Cyc_Absyn_Structdecl*
_temp649=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp649->sc=( void*) s; _temp649->name=({ struct Cyc_Core_Opt* _temp650=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp650->v=(
void*)(( struct _tuple1*)({ struct _tuple1* _temp651= _temp637; if( _temp651 ==
0){ _throw( Null_Exception);} _temp651;})); _temp650;}); _temp649->tvs= ts2;
_temp649->fields= 0; _temp649->attributes= 0; _temp649;}); if( atts != 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp652=( char*)"bad attributes on struct";
struct _tagged_string _temp653; _temp653.curr= _temp652; _temp653.base= _temp652;
_temp653.last_plus_one= _temp652 + 25; _temp653;}), loc);} return({ struct Cyc_List_List*
_temp654=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp654->hd=( void*)({ struct Cyc_Absyn_Decl* _temp655=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp655->r=( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp656=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp656->tag= Cyc_Absyn_Struct_d_tag; _temp656->f1= sd;( void*) _temp656;});
_temp655->loc= loc; _temp655;}); _temp654->tl= 0; _temp654;});} _LL626: { struct
Cyc_List_List* ts2=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc, _temp641); struct
Cyc_Absyn_Decl* ed= Cyc_Absyn_enum_decl( s,({ struct Cyc_Core_Opt* _temp657=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp657->v=(
void*)(( struct _tuple1*)({ struct _tuple1* _temp658= _temp643; if( _temp658 ==
0){ _throw( Null_Exception);} _temp658;})); _temp657;}), ts2, 0, loc); if( atts
!= 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp659=( char*)"bad attributes on enum";
struct _tagged_string _temp660; _temp660.curr= _temp659; _temp660.base= _temp659;
_temp660.last_plus_one= _temp659 + 23; _temp660;}), loc);} return({ struct Cyc_List_List*
_temp661=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp661->hd=( void*) ed; _temp661->tl= 0; _temp661;});} _LL628: { struct Cyc_Absyn_Xenumdecl*
ed=({ struct Cyc_Absyn_Xenumdecl* _temp662=( struct Cyc_Absyn_Xenumdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Xenumdecl)); _temp662->sc=( void*) s;
_temp662->name= _temp647; _temp662->fields= 0; _temp662;}); if( atts != 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp663=( char*)"bad attributes on xenum";
struct _tagged_string _temp664; _temp664.curr= _temp663; _temp664.base= _temp663;
_temp664.last_plus_one= _temp663 + 24; _temp664;}), loc);} return({ struct Cyc_List_List*
_temp665=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp665->hd=( void*)({ struct Cyc_Absyn_Decl* _temp666=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp666->r=( void*)({ struct Cyc_Absyn_Xenum_d_struct*
_temp667=( struct Cyc_Absyn_Xenum_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Xenum_d_struct));
_temp667->tag= Cyc_Absyn_Xenum_d_tag; _temp667->f1= ed;( void*) _temp667;});
_temp666->loc= loc; _temp666;}); _temp665->tl= 0; _temp665;});} _LL630: if( atts
!= 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp668=( char*)"bad attributes on union";
struct _tagged_string _temp669; _temp669.curr= _temp668; _temp669.base= _temp668;
_temp669.last_plus_one= _temp668 + 24; _temp669;}), loc);} return({ struct Cyc_List_List*
_temp670=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp670->hd=( void*)({ struct Cyc_Absyn_Decl* _temp671=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp671->r=( void*) Cyc_Absyn_Union_d;
_temp671->loc= loc; _temp671;}); _temp670->tl= 0; _temp670;}); _LL632: Cyc_Parse_err((
struct _tagged_string)({ char* _temp672=( char*)"missing declarator"; struct
_tagged_string _temp673; _temp673.curr= _temp672; _temp673.base= _temp672;
_temp673.last_plus_one= _temp672 + 19; _temp673;}), loc); return 0; _LL622:;}}
else{ void* t= ts_info.f1; struct Cyc_List_List* fields= Cyc_Parse_apply_tmss(
tq, t, _temp590, atts); if( istypedef){ if( ! exps_empty){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp674=( char*)"initializer in typedef declaration";
struct _tagged_string _temp675; _temp675.curr= _temp674; _temp675.base= _temp674;
_temp675.last_plus_one= _temp674 + 35; _temp675;}), loc);}{ struct Cyc_List_List*
decls=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Decl*(* f)( struct Cyc_Position_Segment*,
struct _tuple7*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_v_typ_to_typedef, loc, fields); if( ts_info.f2 != 0){
struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)( ts_info.f2)->v;{ void*
_temp676=( void*) d->r; struct Cyc_Absyn_Structdecl* _temp686; struct Cyc_Absyn_Enumdecl*
_temp688; struct Cyc_Absyn_Xenumdecl* _temp690; _LL678: if(( unsigned int)
_temp676 > 1u?(( struct _enum_struct*) _temp676)->tag == Cyc_Absyn_Struct_d_tag:
0){ _LL687: _temp686=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*)
_temp676)->f1; goto _LL679;} else{ goto _LL680;} _LL680: if(( unsigned int)
_temp676 > 1u?(( struct _enum_struct*) _temp676)->tag == Cyc_Absyn_Enum_d_tag: 0){
_LL689: _temp688=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*)
_temp676)->f1; goto _LL681;} else{ goto _LL682;} _LL682: if(( unsigned int)
_temp676 > 1u?(( struct _enum_struct*) _temp676)->tag == Cyc_Absyn_Xenum_d_tag:
0){ _LL691: _temp690=( struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_d_struct*)
_temp676)->f1; goto _LL683;} else{ goto _LL684;} _LL684: goto _LL685; _LL679:(
void*)( _temp686->sc=( void*) s); _temp686->attributes= atts; atts= 0; goto
_LL677; _LL681:( void*)( _temp688->sc=( void*) s); goto _LL677; _LL683:( void*)(
_temp690->sc=( void*) s); goto _LL677; _LL685: Cyc_Parse_err(( struct
_tagged_string)({ char* _temp692=( char*)"declaration within typedef is not a struct,enum, or xenum";
struct _tagged_string _temp693; _temp693.curr= _temp692; _temp693.base= _temp692;
_temp693.last_plus_one= _temp692 + 58; _temp693;}), loc); goto _LL677; _LL677:;}
decls=({ struct Cyc_List_List* _temp694=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp694->hd=( void*) d; _temp694->tl= decls;
_temp694;});} if( atts != 0){ Cyc_Parse_err(({ struct _tagged_string _temp695=
Cyc_Absyn_attribute2string(( void*) atts->hd); xprintf("bad attribute %.*s in typedef",
_temp695.last_plus_one - _temp695.curr, _temp695.curr);}), loc);} return decls;}}
else{ if( ts_info.f2 != 0){ Cyc_Parse_unimp2(( struct _tagged_string)({ char*
_temp696=( char*)"nested type declaration within declarator"; struct
_tagged_string _temp697; _temp697.curr= _temp696; _temp697.base= _temp696;
_temp697.last_plus_one= _temp696 + 42; _temp697;}), loc);}{ struct Cyc_List_List*
decls= 0;{ struct Cyc_List_List* ds= fields; for( 0; ds != 0; ds= ds->tl,
_temp588= _temp588->tl){ struct _tuple7 _temp700; struct Cyc_List_List* _temp701;
struct Cyc_List_List* _temp703; void* _temp705; struct Cyc_Absyn_Tqual* _temp707;
struct _tuple1* _temp709; struct _tuple7* _temp698=( struct _tuple7*) ds->hd;
_temp700=* _temp698; _LL710: _temp709= _temp700.f1; goto _LL708; _LL708:
_temp707= _temp700.f2; goto _LL706; _LL706: _temp705= _temp700.f3; goto _LL704;
_LL704: _temp703= _temp700.f4; goto _LL702; _LL702: _temp701= _temp700.f5; goto
_LL699; _LL699: if( _temp703 != 0){ Cyc_Parse_warn(( struct _tagged_string)({
char* _temp711=( char*)"bad type params, ignoring"; struct _tagged_string
_temp712; _temp712.curr= _temp711; _temp712.base= _temp711; _temp712.last_plus_one=
_temp711 + 26; _temp712;}), loc);} if( _temp588 == 0){(( void(*)( struct
_tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct
_tagged_string)({ char* _temp713=( char*)"unexpected null in parse!"; struct
_tagged_string _temp714; _temp714.curr= _temp713; _temp714.base= _temp713;
_temp714.last_plus_one= _temp713 + 26; _temp714;}), loc);}{ struct Cyc_Absyn_Exp*
eopt=( struct Cyc_Absyn_Exp*) _temp588->hd; struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_new_vardecl(
_temp709, _temp705, eopt); vd->tq= _temp707;( void*)( vd->sc=( void*) s); vd->attributes=
_temp701;{ struct Cyc_Absyn_Decl* d=({ struct Cyc_Absyn_Decl* _temp715=( struct
Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp715->r=( void*)({
struct Cyc_Absyn_Var_d_struct* _temp716=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp716->tag= Cyc_Absyn_Var_d_tag;
_temp716->f1= vd;( void*) _temp716;}); _temp715->loc= loc; _temp715;}); decls=({
struct Cyc_List_List* _temp717=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp717->hd=( void*) d; _temp717->tl= decls; _temp717;});}}}}
return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
decls);}}}}}}} static void* Cyc_Parse_id_to_kind( struct _tagged_string s,
struct Cyc_Position_Segment* loc){ if( Cyc_String_strlen( s) !=( unsigned int) 1){
Cyc_Parse_err(({ struct _tagged_string _temp718= s; xprintf("bad kind: %.*s",
_temp718.last_plus_one - _temp718.curr, _temp718.curr);}), loc); return Cyc_Absyn_BoxKind;}
else{ switch(({ struct _tagged_string _temp719= s; char* _temp721= _temp719.curr
+ 0; if( _temp721 < _temp719.base? 1: _temp721 >= _temp719.last_plus_one){
_throw( Null_Exception);}* _temp721;})){ case 'A': _LL722: return Cyc_Absyn_AnyKind;
case 'M': _LL723: return Cyc_Absyn_MemKind; case 'B': _LL724: return Cyc_Absyn_BoxKind;
case 'R': _LL725: return Cyc_Absyn_RgnKind; case 'E': _LL726: return Cyc_Absyn_EffKind;
default: _LL727: Cyc_Parse_err(({ struct _tagged_string _temp729= s; xprintf("bad kind: %.*s",
_temp729.last_plus_one - _temp729.curr, _temp729.curr);}), loc); return Cyc_Absyn_BoxKind;}}}
static struct Cyc_Absyn_Decl* Cyc_Parse_v_typ_to_typedef( struct Cyc_Position_Segment*
loc, struct _tuple7* t){ struct _tuple1* x=(* t).f1; Cyc_Lex_register_typedef( x);
if((* t).f5 != 0){ Cyc_Parse_err(({ struct _tagged_string _temp730= Cyc_Absyn_attribute2string((
void*)((* t).f5)->hd); xprintf("bad attribute %.*s within typedef", _temp730.last_plus_one
- _temp730.curr, _temp730.curr);}), loc);} return Cyc_Absyn_new_decl(({ struct
Cyc_Absyn_Typedef_d_struct* _temp731=( struct Cyc_Absyn_Typedef_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct)); _temp731->tag= Cyc_Absyn_Typedef_d_tag;
_temp731->f1=({ struct Cyc_Absyn_Typedefdecl* _temp732=( struct Cyc_Absyn_Typedefdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedefdecl)); _temp732->name= x; _temp732->tvs=(*
t).f4; _temp732->defn=( void*)(* t).f3; _temp732;});( void*) _temp731;}), loc);}
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
struct _xenum_struct* yy1){ struct _xenum_struct* _temp733= yy1; struct _tuple11*
_temp739; _LL735: if((*(( struct _xenum_struct*) _temp733)).tag == Cyc_Int_tok_tag){
_LL740: _temp739=(( struct Cyc_Int_tok_struct*) _temp733)->f1; goto _LL736;}
else{ goto _LL737;} _LL737: goto _LL738; _LL736: return _temp739; _LL738:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp741=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp741=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp742=( char*)"Int_tok";
struct _tagged_string _temp743; _temp743.curr= _temp742; _temp743.base= _temp742;
_temp743.last_plus_one= _temp742 + 8; _temp743;})};( struct _xenum_struct*)
_temp741;})); _LL734:;} struct _tagged_string Cyc_yyget_String_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp744= yy1; struct _tagged_string
_temp750; _LL746: if((*(( struct _xenum_struct*) _temp744)).tag == Cyc_String_tok_tag){
_LL751: _temp750=(( struct Cyc_String_tok_struct*) _temp744)->f1; goto _LL747;}
else{ goto _LL748;} _LL748: goto _LL749; _LL747: return _temp750; _LL749:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp752=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp752=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp753=( char*)"String_tok";
struct _tagged_string _temp754; _temp754.curr= _temp753; _temp754.base= _temp753;
_temp754.last_plus_one= _temp753 + 11; _temp754;})};( struct _xenum_struct*)
_temp752;})); _LL745:;} char Cyc_yyget_Char_tok( struct _xenum_struct* yy1){
struct _xenum_struct* _temp755= yy1; char _temp761; _LL757: if((*(( struct
_xenum_struct*) _temp755)).tag == Cyc_Char_tok_tag){ _LL762: _temp761=(( struct
Cyc_Char_tok_struct*) _temp755)->f1; goto _LL758;} else{ goto _LL759;} _LL759:
goto _LL760; _LL758: return _temp761; _LL760:( void) _throw(({ struct Cyc_Core_Failure_struct*
_temp763=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));*
_temp763=( struct Cyc_Core_Failure_struct){.tag= Cyc_Core_Failure_tag,.f1=(
struct _tagged_string)({ char* _temp764=( char*)"Char_tok"; struct
_tagged_string _temp765; _temp765.curr= _temp764; _temp765.base= _temp764;
_temp765.last_plus_one= _temp764 + 9; _temp765;})};( struct _xenum_struct*)
_temp763;})); _LL756:;} void* Cyc_yyget_Pointer_Sort_tok( struct _xenum_struct*
yy1){ struct _xenum_struct* _temp766= yy1; void* _temp772; _LL768: if((*((
struct _xenum_struct*) _temp766)).tag == Cyc_Pointer_Sort_tok_tag){ _LL773:
_temp772=(( struct Cyc_Pointer_Sort_tok_struct*) _temp766)->f1; goto _LL769;}
else{ goto _LL770;} _LL770: goto _LL771; _LL769: return _temp772; _LL771:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp774=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp774=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp775=( char*)"Pointer_Sort_tok";
struct _tagged_string _temp776; _temp776.curr= _temp775; _temp776.base= _temp775;
_temp776.last_plus_one= _temp775 + 17; _temp776;})};( struct _xenum_struct*)
_temp774;})); _LL767:;} struct Cyc_Absyn_Exp* Cyc_yyget_Exp_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp777= yy1; struct Cyc_Absyn_Exp*
_temp783; _LL779: if((*(( struct _xenum_struct*) _temp777)).tag == Cyc_Exp_tok_tag){
_LL784: _temp783=(( struct Cyc_Exp_tok_struct*) _temp777)->f1; goto _LL780;}
else{ goto _LL781;} _LL781: goto _LL782; _LL780: return _temp783; _LL782:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp785=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp785=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp786=( char*)"Exp_tok";
struct _tagged_string _temp787; _temp787.curr= _temp786; _temp787.base= _temp786;
_temp787.last_plus_one= _temp786 + 8; _temp787;})};( struct _xenum_struct*)
_temp785;})); _LL778:;} struct Cyc_List_List* Cyc_yyget_ExpList_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp788= yy1; struct Cyc_List_List*
_temp794; _LL790: if((*(( struct _xenum_struct*) _temp788)).tag == Cyc_ExpList_tok_tag){
_LL795: _temp794=(( struct Cyc_ExpList_tok_struct*) _temp788)->f1; goto _LL791;}
else{ goto _LL792;} _LL792: goto _LL793; _LL791: return _temp794; _LL793:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp796=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp796=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp797=( char*)"ExpList_tok";
struct _tagged_string _temp798; _temp798.curr= _temp797; _temp798.base= _temp797;
_temp798.last_plus_one= _temp797 + 12; _temp798;})};( struct _xenum_struct*)
_temp796;})); _LL789:;} struct Cyc_List_List* Cyc_yyget_InitializerList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp799= yy1; struct Cyc_List_List*
_temp805; _LL801: if((*(( struct _xenum_struct*) _temp799)).tag == Cyc_InitializerList_tok_tag){
_LL806: _temp805=(( struct Cyc_InitializerList_tok_struct*) _temp799)->f1; goto
_LL802;} else{ goto _LL803;} _LL803: goto _LL804; _LL802: return _temp805;
_LL804:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp807=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp807=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp808=( char*)"InitializerList_tok";
struct _tagged_string _temp809; _temp809.curr= _temp808; _temp809.base= _temp808;
_temp809.last_plus_one= _temp808 + 20; _temp809;})};( struct _xenum_struct*)
_temp807;})); _LL800:;} void* Cyc_yyget_Primop_tok( struct _xenum_struct* yy1){
struct _xenum_struct* _temp810= yy1; void* _temp816; _LL812: if((*(( struct
_xenum_struct*) _temp810)).tag == Cyc_Primop_tok_tag){ _LL817: _temp816=((
struct Cyc_Primop_tok_struct*) _temp810)->f1; goto _LL813;} else{ goto _LL814;}
_LL814: goto _LL815; _LL813: return _temp816; _LL815:( void) _throw(({ struct
Cyc_Core_Failure_struct* _temp818=( struct Cyc_Core_Failure_struct*) GC_malloc(
sizeof( struct Cyc_Core_Failure_struct));* _temp818=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp819=( char*)"Primop_tok";
struct _tagged_string _temp820; _temp820.curr= _temp819; _temp820.base= _temp819;
_temp820.last_plus_one= _temp819 + 11; _temp820;})};( struct _xenum_struct*)
_temp818;})); _LL811:;} struct Cyc_Core_Opt* Cyc_yyget_Primopopt_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp821= yy1; struct Cyc_Core_Opt*
_temp827; _LL823: if((*(( struct _xenum_struct*) _temp821)).tag == Cyc_Primopopt_tok_tag){
_LL828: _temp827=(( struct Cyc_Primopopt_tok_struct*) _temp821)->f1; goto _LL824;}
else{ goto _LL825;} _LL825: goto _LL826; _LL824: return _temp827; _LL826:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp829=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp829=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp830=( char*)"Primopopt_tok";
struct _tagged_string _temp831; _temp831.curr= _temp830; _temp831.base= _temp830;
_temp831.last_plus_one= _temp830 + 14; _temp831;})};( struct _xenum_struct*)
_temp829;})); _LL822:;} struct _tuple1* Cyc_yyget_QualId_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp832= yy1; struct _tuple1*
_temp838; _LL834: if((*(( struct _xenum_struct*) _temp832)).tag == Cyc_QualId_tok_tag){
_LL839: _temp838=(( struct Cyc_QualId_tok_struct*) _temp832)->f1; goto _LL835;}
else{ goto _LL836;} _LL836: goto _LL837; _LL835: return _temp838; _LL837:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp840=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp840=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp841=( char*)"QualId_tok";
struct _tagged_string _temp842; _temp842.curr= _temp841; _temp842.base= _temp841;
_temp842.last_plus_one= _temp841 + 11; _temp842;})};( struct _xenum_struct*)
_temp840;})); _LL833:;} struct Cyc_Absyn_Stmt* Cyc_yyget_Stmt_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp843= yy1; struct Cyc_Absyn_Stmt*
_temp849; _LL845: if((*(( struct _xenum_struct*) _temp843)).tag == Cyc_Stmt_tok_tag){
_LL850: _temp849=(( struct Cyc_Stmt_tok_struct*) _temp843)->f1; goto _LL846;}
else{ goto _LL847;} _LL847: goto _LL848; _LL846: return _temp849; _LL848:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp851=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp851=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp852=( char*)"Stmt_tok";
struct _tagged_string _temp853; _temp853.curr= _temp852; _temp853.base= _temp852;
_temp853.last_plus_one= _temp852 + 9; _temp853;})};( struct _xenum_struct*)
_temp851;})); _LL844:;} void* Cyc_yyget_BlockItem_tok( struct _xenum_struct* yy1){
struct _xenum_struct* _temp854= yy1; void* _temp860; _LL856: if((*(( struct
_xenum_struct*) _temp854)).tag == Cyc_BlockItem_tok_tag){ _LL861: _temp860=((
struct Cyc_BlockItem_tok_struct*) _temp854)->f1; goto _LL857;} else{ goto _LL858;}
_LL858: goto _LL859; _LL857: return _temp860; _LL859:( void) _throw(({ struct
Cyc_Core_Failure_struct* _temp862=( struct Cyc_Core_Failure_struct*) GC_malloc(
sizeof( struct Cyc_Core_Failure_struct));* _temp862=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp863=( char*)"BlockItem_tok";
struct _tagged_string _temp864; _temp864.curr= _temp863; _temp864.base= _temp863;
_temp864.last_plus_one= _temp863 + 14; _temp864;})};( struct _xenum_struct*)
_temp862;})); _LL855:;} struct Cyc_List_List* Cyc_yyget_SwitchClauseList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp865= yy1; struct Cyc_List_List*
_temp871; _LL867: if((*(( struct _xenum_struct*) _temp865)).tag == Cyc_SwitchClauseList_tok_tag){
_LL872: _temp871=(( struct Cyc_SwitchClauseList_tok_struct*) _temp865)->f1; goto
_LL868;} else{ goto _LL869;} _LL869: goto _LL870; _LL868: return _temp871;
_LL870:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp873=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp873=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp874=( char*)"SwitchClauseList_tok";
struct _tagged_string _temp875; _temp875.curr= _temp874; _temp875.base= _temp874;
_temp875.last_plus_one= _temp874 + 21; _temp875;})};( struct _xenum_struct*)
_temp873;})); _LL866:;} struct Cyc_Absyn_Pat* Cyc_yyget_Pattern_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp876= yy1; struct Cyc_Absyn_Pat*
_temp882; _LL878: if((*(( struct _xenum_struct*) _temp876)).tag == Cyc_Pattern_tok_tag){
_LL883: _temp882=(( struct Cyc_Pattern_tok_struct*) _temp876)->f1; goto _LL879;}
else{ goto _LL880;} _LL880: goto _LL881; _LL879: return _temp882; _LL881:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp884=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp884=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp885=( char*)"Pattern_tok";
struct _tagged_string _temp886; _temp886.curr= _temp885; _temp886.base= _temp885;
_temp886.last_plus_one= _temp885 + 12; _temp886;})};( struct _xenum_struct*)
_temp884;})); _LL877:;} struct Cyc_List_List* Cyc_yyget_PatternList_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp887= yy1; struct Cyc_List_List*
_temp893; _LL889: if((*(( struct _xenum_struct*) _temp887)).tag == Cyc_PatternList_tok_tag){
_LL894: _temp893=(( struct Cyc_PatternList_tok_struct*) _temp887)->f1; goto
_LL890;} else{ goto _LL891;} _LL891: goto _LL892; _LL890: return _temp893;
_LL892:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp895=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp895=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp896=( char*)"PatternList_tok";
struct _tagged_string _temp897; _temp897.curr= _temp896; _temp897.base= _temp896;
_temp897.last_plus_one= _temp896 + 16; _temp897;})};( struct _xenum_struct*)
_temp895;})); _LL888:;} struct _tuple15* Cyc_yyget_FieldPattern_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp898= yy1; struct _tuple15*
_temp904; _LL900: if((*(( struct _xenum_struct*) _temp898)).tag == Cyc_FieldPattern_tok_tag){
_LL905: _temp904=(( struct Cyc_FieldPattern_tok_struct*) _temp898)->f1; goto
_LL901;} else{ goto _LL902;} _LL902: goto _LL903; _LL901: return _temp904;
_LL903:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp906=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp906=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp907=( char*)"FieldPattern_tok";
struct _tagged_string _temp908; _temp908.curr= _temp907; _temp908.base= _temp907;
_temp908.last_plus_one= _temp907 + 17; _temp908;})};( struct _xenum_struct*)
_temp906;})); _LL899:;} struct Cyc_List_List* Cyc_yyget_FieldPatternList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp909= yy1; struct Cyc_List_List*
_temp915; _LL911: if((*(( struct _xenum_struct*) _temp909)).tag == Cyc_FieldPatternList_tok_tag){
_LL916: _temp915=(( struct Cyc_FieldPatternList_tok_struct*) _temp909)->f1; goto
_LL912;} else{ goto _LL913;} _LL913: goto _LL914; _LL912: return _temp915;
_LL914:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp917=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp917=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp918=( char*)"FieldPatternList_tok";
struct _tagged_string _temp919; _temp919.curr= _temp918; _temp919.base= _temp918;
_temp919.last_plus_one= _temp918 + 21; _temp919;})};( struct _xenum_struct*)
_temp917;})); _LL910:;} struct Cyc_Absyn_Fndecl* Cyc_yyget_FnDecl_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp920= yy1; struct Cyc_Absyn_Fndecl*
_temp926; _LL922: if((*(( struct _xenum_struct*) _temp920)).tag == Cyc_FnDecl_tok_tag){
_LL927: _temp926=(( struct Cyc_FnDecl_tok_struct*) _temp920)->f1; goto _LL923;}
else{ goto _LL924;} _LL924: goto _LL925; _LL923: return _temp926; _LL925:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp928=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp928=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp929=( char*)"FnDecl_tok";
struct _tagged_string _temp930; _temp930.curr= _temp929; _temp930.base= _temp929;
_temp930.last_plus_one= _temp929 + 11; _temp930;})};( struct _xenum_struct*)
_temp928;})); _LL921:;} struct Cyc_List_List* Cyc_yyget_DeclList_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp931= yy1; struct Cyc_List_List*
_temp937; _LL933: if((*(( struct _xenum_struct*) _temp931)).tag == Cyc_DeclList_tok_tag){
_LL938: _temp937=(( struct Cyc_DeclList_tok_struct*) _temp931)->f1; goto _LL934;}
else{ goto _LL935;} _LL935: goto _LL936; _LL934: return _temp937; _LL936:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp939=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp939=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp940=( char*)"DeclList_tok";
struct _tagged_string _temp941; _temp941.curr= _temp940; _temp941.base= _temp940;
_temp941.last_plus_one= _temp940 + 13; _temp941;})};( struct _xenum_struct*)
_temp939;})); _LL932:;} struct Cyc_Parse_Declaration_spec* Cyc_yyget_DeclSpec_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp942= yy1; struct Cyc_Parse_Declaration_spec*
_temp948; _LL944: if((*(( struct _xenum_struct*) _temp942)).tag == Cyc_DeclSpec_tok_tag){
_LL949: _temp948=(( struct Cyc_DeclSpec_tok_struct*) _temp942)->f1; goto _LL945;}
else{ goto _LL946;} _LL946: goto _LL947; _LL945: return _temp948; _LL947:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp950=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp950=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp951=( char*)"DeclSpec_tok";
struct _tagged_string _temp952; _temp952.curr= _temp951; _temp952.base= _temp951;
_temp952.last_plus_one= _temp951 + 13; _temp952;})};( struct _xenum_struct*)
_temp950;})); _LL943:;} struct _tuple12* Cyc_yyget_InitDecl_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp953= yy1; struct _tuple12*
_temp959; _LL955: if((*(( struct _xenum_struct*) _temp953)).tag == Cyc_InitDecl_tok_tag){
_LL960: _temp959=(( struct Cyc_InitDecl_tok_struct*) _temp953)->f1; goto _LL956;}
else{ goto _LL957;} _LL957: goto _LL958; _LL956: return _temp959; _LL958:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp961=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp961=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp962=( char*)"InitDecl_tok";
struct _tagged_string _temp963; _temp963.curr= _temp962; _temp963.base= _temp962;
_temp963.last_plus_one= _temp962 + 13; _temp963;})};( struct _xenum_struct*)
_temp961;})); _LL954:;} struct Cyc_List_List* Cyc_yyget_InitDeclList_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp964= yy1; struct Cyc_List_List*
_temp970; _LL966: if((*(( struct _xenum_struct*) _temp964)).tag == Cyc_InitDeclList_tok_tag){
_LL971: _temp970=(( struct Cyc_InitDeclList_tok_struct*) _temp964)->f1; goto
_LL967;} else{ goto _LL968;} _LL968: goto _LL969; _LL967: return _temp970;
_LL969:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp972=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp972=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp973=( char*)"InitDeclList_tok";
struct _tagged_string _temp974; _temp974.curr= _temp973; _temp974.base= _temp973;
_temp974.last_plus_one= _temp973 + 17; _temp974;})};( struct _xenum_struct*)
_temp972;})); _LL965:;} void* Cyc_yyget_StorageClass_tok( struct _xenum_struct*
yy1){ struct _xenum_struct* _temp975= yy1; void* _temp981; _LL977: if((*((
struct _xenum_struct*) _temp975)).tag == Cyc_StorageClass_tok_tag){ _LL982:
_temp981=(( struct Cyc_StorageClass_tok_struct*) _temp975)->f1; goto _LL978;}
else{ goto _LL979;} _LL979: goto _LL980; _LL978: return _temp981; _LL980:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp983=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp983=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp984=( char*)"StorageClass_tok";
struct _tagged_string _temp985; _temp985.curr= _temp984; _temp985.base= _temp984;
_temp985.last_plus_one= _temp984 + 17; _temp985;})};( struct _xenum_struct*)
_temp983;})); _LL976:;} void* Cyc_yyget_TypeSpecifier_tok( struct _xenum_struct*
yy1){ struct _xenum_struct* _temp986= yy1; void* _temp992; _LL988: if((*((
struct _xenum_struct*) _temp986)).tag == Cyc_TypeSpecifier_tok_tag){ _LL993:
_temp992=(( struct Cyc_TypeSpecifier_tok_struct*) _temp986)->f1; goto _LL989;}
else{ goto _LL990;} _LL990: goto _LL991; _LL989: return _temp992; _LL991:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp994=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp994=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp995=( char*)"TypeSpecifier_tok";
struct _tagged_string _temp996; _temp996.curr= _temp995; _temp996.base= _temp995;
_temp996.last_plus_one= _temp995 + 18; _temp996;})};( struct _xenum_struct*)
_temp994;})); _LL987:;} void* Cyc_yyget_StructOrUnion_tok( struct _xenum_struct*
yy1){ struct _xenum_struct* _temp997= yy1; void* _temp1003; _LL999: if((*((
struct _xenum_struct*) _temp997)).tag == Cyc_StructOrUnion_tok_tag){ _LL1004:
_temp1003=(( struct Cyc_StructOrUnion_tok_struct*) _temp997)->f1; goto _LL1000;}
else{ goto _LL1001;} _LL1001: goto _LL1002; _LL1000: return _temp1003; _LL1002:(
void) _throw(({ struct Cyc_Core_Failure_struct* _temp1005=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp1005=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp1006=( char*)"StructOrUnion_tok";
struct _tagged_string _temp1007; _temp1007.curr= _temp1006; _temp1007.base=
_temp1006; _temp1007.last_plus_one= _temp1006 + 18; _temp1007;})};( struct
_xenum_struct*) _temp1005;})); _LL998:;} struct Cyc_Absyn_Tqual* Cyc_yyget_TypeQual_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1008= yy1; struct Cyc_Absyn_Tqual*
_temp1014; _LL1010: if((*(( struct _xenum_struct*) _temp1008)).tag == Cyc_TypeQual_tok_tag){
_LL1015: _temp1014=(( struct Cyc_TypeQual_tok_struct*) _temp1008)->f1; goto
_LL1011;} else{ goto _LL1012;} _LL1012: goto _LL1013; _LL1011: return _temp1014;
_LL1013:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1016=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp1016=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp1017=( char*)"TypeQual_tok";
struct _tagged_string _temp1018; _temp1018.curr= _temp1017; _temp1018.base=
_temp1017; _temp1018.last_plus_one= _temp1017 + 13; _temp1018;})};( struct
_xenum_struct*) _temp1016;})); _LL1009:;} struct Cyc_List_List* Cyc_yyget_StructFieldDeclList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1019= yy1; struct Cyc_List_List*
_temp1025; _LL1021: if((*(( struct _xenum_struct*) _temp1019)).tag == Cyc_StructFieldDeclList_tok_tag){
_LL1026: _temp1025=(( struct Cyc_StructFieldDeclList_tok_struct*) _temp1019)->f1;
goto _LL1022;} else{ goto _LL1023;} _LL1023: goto _LL1024; _LL1022: return
_temp1025; _LL1024:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1027=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));*
_temp1027=( struct Cyc_Core_Failure_struct){.tag= Cyc_Core_Failure_tag,.f1=(
struct _tagged_string)({ char* _temp1028=( char*)"StructFieldDeclList_tok";
struct _tagged_string _temp1029; _temp1029.curr= _temp1028; _temp1029.base=
_temp1028; _temp1029.last_plus_one= _temp1028 + 24; _temp1029;})};( struct
_xenum_struct*) _temp1027;})); _LL1020:;} struct Cyc_List_List* Cyc_yyget_StructFieldDeclListList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1030= yy1; struct Cyc_List_List*
_temp1036; _LL1032: if((*(( struct _xenum_struct*) _temp1030)).tag == Cyc_StructFieldDeclListList_tok_tag){
_LL1037: _temp1036=(( struct Cyc_StructFieldDeclListList_tok_struct*) _temp1030)->f1;
goto _LL1033;} else{ goto _LL1034;} _LL1034: goto _LL1035; _LL1033: return
_temp1036; _LL1035:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1038=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));*
_temp1038=( struct Cyc_Core_Failure_struct){.tag= Cyc_Core_Failure_tag,.f1=(
struct _tagged_string)({ char* _temp1039=( char*)"StructFieldDeclListList_tok";
struct _tagged_string _temp1040; _temp1040.curr= _temp1039; _temp1040.base=
_temp1039; _temp1040.last_plus_one= _temp1039 + 28; _temp1040;})};( struct
_xenum_struct*) _temp1038;})); _LL1031:;} struct Cyc_List_List* Cyc_yyget_TypeModifierList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1041= yy1; struct Cyc_List_List*
_temp1047; _LL1043: if((*(( struct _xenum_struct*) _temp1041)).tag == Cyc_TypeModifierList_tok_tag){
_LL1048: _temp1047=(( struct Cyc_TypeModifierList_tok_struct*) _temp1041)->f1;
goto _LL1044;} else{ goto _LL1045;} _LL1045: goto _LL1046; _LL1044: return
_temp1047; _LL1046:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1049=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));*
_temp1049=( struct Cyc_Core_Failure_struct){.tag= Cyc_Core_Failure_tag,.f1=(
struct _tagged_string)({ char* _temp1050=( char*)"TypeModifierList_tok"; struct
_tagged_string _temp1051; _temp1051.curr= _temp1050; _temp1051.base= _temp1050;
_temp1051.last_plus_one= _temp1050 + 21; _temp1051;})};( struct _xenum_struct*)
_temp1049;})); _LL1042:;} void* Cyc_yyget_Rgn_tok( struct _xenum_struct* yy1){
struct _xenum_struct* _temp1052= yy1; void* _temp1058; _LL1054: if((*(( struct
_xenum_struct*) _temp1052)).tag == Cyc_Rgn_tok_tag){ _LL1059: _temp1058=((
struct Cyc_Rgn_tok_struct*) _temp1052)->f1; goto _LL1055;} else{ goto _LL1056;}
_LL1056: goto _LL1057; _LL1055: return _temp1058; _LL1057:( void) _throw(({
struct Cyc_Core_Failure_struct* _temp1060=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp1060=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp1061=( char*)"Rgn_tok";
struct _tagged_string _temp1062; _temp1062.curr= _temp1061; _temp1062.base=
_temp1061; _temp1062.last_plus_one= _temp1061 + 8; _temp1062;})};( struct
_xenum_struct*) _temp1060;})); _LL1053:;} struct Cyc_Parse_Declarator* Cyc_yyget_Declarator_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1063= yy1; struct Cyc_Parse_Declarator*
_temp1069; _LL1065: if((*(( struct _xenum_struct*) _temp1063)).tag == Cyc_Declarator_tok_tag){
_LL1070: _temp1069=(( struct Cyc_Declarator_tok_struct*) _temp1063)->f1; goto
_LL1066;} else{ goto _LL1067;} _LL1067: goto _LL1068; _LL1066: return _temp1069;
_LL1068:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1071=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp1071=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp1072=( char*)"Declarator_tok";
struct _tagged_string _temp1073; _temp1073.curr= _temp1072; _temp1073.base=
_temp1072; _temp1073.last_plus_one= _temp1072 + 15; _temp1073;})};( struct
_xenum_struct*) _temp1071;})); _LL1064:;} struct Cyc_List_List* Cyc_yyget_DeclaratorList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1074= yy1; struct Cyc_List_List*
_temp1080; _LL1076: if((*(( struct _xenum_struct*) _temp1074)).tag == Cyc_DeclaratorList_tok_tag){
_LL1081: _temp1080=(( struct Cyc_DeclaratorList_tok_struct*) _temp1074)->f1;
goto _LL1077;} else{ goto _LL1078;} _LL1078: goto _LL1079; _LL1077: return
_temp1080; _LL1079:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1082=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));*
_temp1082=( struct Cyc_Core_Failure_struct){.tag= Cyc_Core_Failure_tag,.f1=(
struct _tagged_string)({ char* _temp1083=( char*)"DeclaratorList_tok"; struct
_tagged_string _temp1084; _temp1084.curr= _temp1083; _temp1084.base= _temp1083;
_temp1084.last_plus_one= _temp1083 + 19; _temp1084;})};( struct _xenum_struct*)
_temp1082;})); _LL1075:;} struct Cyc_Parse_Abstractdeclarator* Cyc_yyget_AbstractDeclarator_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1085= yy1; struct Cyc_Parse_Abstractdeclarator*
_temp1091; _LL1087: if((*(( struct _xenum_struct*) _temp1085)).tag == Cyc_AbstractDeclarator_tok_tag){
_LL1092: _temp1091=(( struct Cyc_AbstractDeclarator_tok_struct*) _temp1085)->f1;
goto _LL1088;} else{ goto _LL1089;} _LL1089: goto _LL1090; _LL1088: return
_temp1091; _LL1090:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1093=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));*
_temp1093=( struct Cyc_Core_Failure_struct){.tag= Cyc_Core_Failure_tag,.f1=(
struct _tagged_string)({ char* _temp1094=( char*)"AbstractDeclarator_tok";
struct _tagged_string _temp1095; _temp1095.curr= _temp1094; _temp1095.base=
_temp1094; _temp1095.last_plus_one= _temp1094 + 23; _temp1095;})};( struct
_xenum_struct*) _temp1093;})); _LL1086:;} struct Cyc_Absyn_Enumfield* Cyc_yyget_EnumeratorField_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1096= yy1; struct Cyc_Absyn_Enumfield*
_temp1102; _LL1098: if((*(( struct _xenum_struct*) _temp1096)).tag == Cyc_EnumeratorField_tok_tag){
_LL1103: _temp1102=(( struct Cyc_EnumeratorField_tok_struct*) _temp1096)->f1;
goto _LL1099;} else{ goto _LL1100;} _LL1100: goto _LL1101; _LL1099: return
_temp1102; _LL1101:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1104=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));*
_temp1104=( struct Cyc_Core_Failure_struct){.tag= Cyc_Core_Failure_tag,.f1=(
struct _tagged_string)({ char* _temp1105=( char*)"EnumeratorField_tok"; struct
_tagged_string _temp1106; _temp1106.curr= _temp1105; _temp1106.base= _temp1105;
_temp1106.last_plus_one= _temp1105 + 20; _temp1106;})};( struct _xenum_struct*)
_temp1104;})); _LL1097:;} struct Cyc_List_List* Cyc_yyget_EnumeratorFieldList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1107= yy1; struct Cyc_List_List*
_temp1113; _LL1109: if((*(( struct _xenum_struct*) _temp1107)).tag == Cyc_EnumeratorFieldList_tok_tag){
_LL1114: _temp1113=(( struct Cyc_EnumeratorFieldList_tok_struct*) _temp1107)->f1;
goto _LL1110;} else{ goto _LL1111;} _LL1111: goto _LL1112; _LL1110: return
_temp1113; _LL1112:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1115=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));*
_temp1115=( struct Cyc_Core_Failure_struct){.tag= Cyc_Core_Failure_tag,.f1=(
struct _tagged_string)({ char* _temp1116=( char*)"EnumeratorFieldList_tok";
struct _tagged_string _temp1117; _temp1117.curr= _temp1116; _temp1117.base=
_temp1116; _temp1117.last_plus_one= _temp1116 + 24; _temp1117;})};( struct
_xenum_struct*) _temp1115;})); _LL1108:;} struct _tuple13* Cyc_yyget_QualSpecList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1118= yy1; struct
_tuple13* _temp1124; _LL1120: if((*(( struct _xenum_struct*) _temp1118)).tag ==
Cyc_QualSpecList_tok_tag){ _LL1125: _temp1124=(( struct Cyc_QualSpecList_tok_struct*)
_temp1118)->f1; goto _LL1121;} else{ goto _LL1122;} _LL1122: goto _LL1123;
_LL1121: return _temp1124; _LL1123:( void) _throw(({ struct Cyc_Core_Failure_struct*
_temp1126=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));*
_temp1126=( struct Cyc_Core_Failure_struct){.tag= Cyc_Core_Failure_tag,.f1=(
struct _tagged_string)({ char* _temp1127=( char*)"QualSpecList_tok"; struct
_tagged_string _temp1128; _temp1128.curr= _temp1127; _temp1128.base= _temp1127;
_temp1128.last_plus_one= _temp1127 + 17; _temp1128;})};( struct _xenum_struct*)
_temp1126;})); _LL1119:;} struct Cyc_List_List* Cyc_yyget_IdList_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp1129= yy1; struct Cyc_List_List*
_temp1135; _LL1131: if((*(( struct _xenum_struct*) _temp1129)).tag == Cyc_IdList_tok_tag){
_LL1136: _temp1135=(( struct Cyc_IdList_tok_struct*) _temp1129)->f1; goto
_LL1132;} else{ goto _LL1133;} _LL1133: goto _LL1134; _LL1132: return _temp1135;
_LL1134:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1137=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp1137=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp1138=( char*)"IdList_tok";
struct _tagged_string _temp1139; _temp1139.curr= _temp1138; _temp1139.base=
_temp1138; _temp1139.last_plus_one= _temp1138 + 11; _temp1139;})};( struct
_xenum_struct*) _temp1137;})); _LL1130:;} struct _tuple2* Cyc_yyget_ParamDecl_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1140= yy1; struct _tuple2*
_temp1146; _LL1142: if((*(( struct _xenum_struct*) _temp1140)).tag == Cyc_ParamDecl_tok_tag){
_LL1147: _temp1146=(( struct Cyc_ParamDecl_tok_struct*) _temp1140)->f1; goto
_LL1143;} else{ goto _LL1144;} _LL1144: goto _LL1145; _LL1143: return _temp1146;
_LL1145:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1148=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp1148=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp1149=( char*)"ParamDecl_tok";
struct _tagged_string _temp1150; _temp1150.curr= _temp1149; _temp1150.base=
_temp1149; _temp1150.last_plus_one= _temp1149 + 14; _temp1150;})};( struct
_xenum_struct*) _temp1148;})); _LL1141:;} struct Cyc_List_List* Cyc_yyget_ParamDeclList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1151= yy1; struct Cyc_List_List*
_temp1157; _LL1153: if((*(( struct _xenum_struct*) _temp1151)).tag == Cyc_ParamDeclList_tok_tag){
_LL1158: _temp1157=(( struct Cyc_ParamDeclList_tok_struct*) _temp1151)->f1; goto
_LL1154;} else{ goto _LL1155;} _LL1155: goto _LL1156; _LL1154: return _temp1157;
_LL1156:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1159=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp1159=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp1160=( char*)"ParamDeclList_tok";
struct _tagged_string _temp1161; _temp1161.curr= _temp1160; _temp1161.base=
_temp1160; _temp1161.last_plus_one= _temp1160 + 18; _temp1161;})};( struct
_xenum_struct*) _temp1159;})); _LL1152:;} struct _tuple14* Cyc_yyget_ParamDeclListBool_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1162= yy1; struct
_tuple14* _temp1168; _LL1164: if((*(( struct _xenum_struct*) _temp1162)).tag ==
Cyc_ParamDeclListBool_tok_tag){ _LL1169: _temp1168=(( struct Cyc_ParamDeclListBool_tok_struct*)
_temp1162)->f1; goto _LL1165;} else{ goto _LL1166;} _LL1166: goto _LL1167;
_LL1165: return _temp1168; _LL1167:( void) _throw(({ struct Cyc_Core_Failure_struct*
_temp1170=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));*
_temp1170=( struct Cyc_Core_Failure_struct){.tag= Cyc_Core_Failure_tag,.f1=(
struct _tagged_string)({ char* _temp1171=( char*)"ParamDeclListBool_tok"; struct
_tagged_string _temp1172; _temp1172.curr= _temp1171; _temp1172.base= _temp1171;
_temp1172.last_plus_one= _temp1171 + 22; _temp1172;})};( struct _xenum_struct*)
_temp1170;})); _LL1163:;} struct Cyc_List_List* Cyc_yyget_TypeList_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp1173= yy1; struct Cyc_List_List*
_temp1179; _LL1175: if((*(( struct _xenum_struct*) _temp1173)).tag == Cyc_TypeList_tok_tag){
_LL1180: _temp1179=(( struct Cyc_TypeList_tok_struct*) _temp1173)->f1; goto
_LL1176;} else{ goto _LL1177;} _LL1177: goto _LL1178; _LL1176: return _temp1179;
_LL1178:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1181=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp1181=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp1182=( char*)"TypeList_tok";
struct _tagged_string _temp1183; _temp1183.curr= _temp1182; _temp1183.base=
_temp1182; _temp1183.last_plus_one= _temp1182 + 13; _temp1183;})};( struct
_xenum_struct*) _temp1181;})); _LL1174:;} struct Cyc_List_List* Cyc_yyget_DesignatorList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1184= yy1; struct Cyc_List_List*
_temp1190; _LL1186: if((*(( struct _xenum_struct*) _temp1184)).tag == Cyc_DesignatorList_tok_tag){
_LL1191: _temp1190=(( struct Cyc_DesignatorList_tok_struct*) _temp1184)->f1;
goto _LL1187;} else{ goto _LL1188;} _LL1188: goto _LL1189; _LL1187: return
_temp1190; _LL1189:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1192=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));*
_temp1192=( struct Cyc_Core_Failure_struct){.tag= Cyc_Core_Failure_tag,.f1=(
struct _tagged_string)({ char* _temp1193=( char*)"DesignatorList_tok"; struct
_tagged_string _temp1194; _temp1194.curr= _temp1193; _temp1194.base= _temp1193;
_temp1194.last_plus_one= _temp1193 + 19; _temp1194;})};( struct _xenum_struct*)
_temp1192;})); _LL1185:;} void* Cyc_yyget_Designator_tok( struct _xenum_struct*
yy1){ struct _xenum_struct* _temp1195= yy1; void* _temp1201; _LL1197: if((*((
struct _xenum_struct*) _temp1195)).tag == Cyc_Designator_tok_tag){ _LL1202:
_temp1201=(( struct Cyc_Designator_tok_struct*) _temp1195)->f1; goto _LL1198;}
else{ goto _LL1199;} _LL1199: goto _LL1200; _LL1198: return _temp1201; _LL1200:(
void) _throw(({ struct Cyc_Core_Failure_struct* _temp1203=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp1203=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp1204=( char*)"Designator_tok";
struct _tagged_string _temp1205; _temp1205.curr= _temp1204; _temp1205.base=
_temp1204; _temp1205.last_plus_one= _temp1204 + 15; _temp1205;})};( struct
_xenum_struct*) _temp1203;})); _LL1196:;} void* Cyc_yyget_Kind_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp1206= yy1; void* _temp1212;
_LL1208: if((*(( struct _xenum_struct*) _temp1206)).tag == Cyc_Kind_tok_tag){
_LL1213: _temp1212=(( struct Cyc_Kind_tok_struct*) _temp1206)->f1; goto _LL1209;}
else{ goto _LL1210;} _LL1210: goto _LL1211; _LL1209: return _temp1212; _LL1211:(
void) _throw(({ struct Cyc_Core_Failure_struct* _temp1214=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp1214=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp1215=( char*)"Kind_tok";
struct _tagged_string _temp1216; _temp1216.curr= _temp1215; _temp1216.base=
_temp1215; _temp1216.last_plus_one= _temp1215 + 9; _temp1216;})};( struct
_xenum_struct*) _temp1214;})); _LL1207:;} void* Cyc_yyget_Type_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp1217= yy1; void* _temp1223;
_LL1219: if((*(( struct _xenum_struct*) _temp1217)).tag == Cyc_Type_tok_tag){
_LL1224: _temp1223=(( struct Cyc_Type_tok_struct*) _temp1217)->f1; goto _LL1220;}
else{ goto _LL1221;} _LL1221: goto _LL1222; _LL1220: return _temp1223; _LL1222:(
void) _throw(({ struct Cyc_Core_Failure_struct* _temp1225=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp1225=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp1226=( char*)"Type_tok";
struct _tagged_string _temp1227; _temp1227.curr= _temp1226; _temp1227.base=
_temp1226; _temp1227.last_plus_one= _temp1226 + 9; _temp1227;})};( struct
_xenum_struct*) _temp1225;})); _LL1218:;} struct Cyc_List_List* Cyc_yyget_AttributeList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1228= yy1; struct Cyc_List_List*
_temp1234; _LL1230: if((*(( struct _xenum_struct*) _temp1228)).tag == Cyc_AttributeList_tok_tag){
_LL1235: _temp1234=(( struct Cyc_AttributeList_tok_struct*) _temp1228)->f1; goto
_LL1231;} else{ goto _LL1232;} _LL1232: goto _LL1233; _LL1231: return _temp1234;
_LL1233:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1236=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp1236=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp1237=( char*)"AttributeList_tok";
struct _tagged_string _temp1238; _temp1238.curr= _temp1237; _temp1238.base=
_temp1237; _temp1238.last_plus_one= _temp1237 + 18; _temp1238;})};( struct
_xenum_struct*) _temp1236;})); _LL1229:;} void* Cyc_yyget_Attribute_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp1239= yy1; void* _temp1245;
_LL1241: if((*(( struct _xenum_struct*) _temp1239)).tag == Cyc_Attribute_tok_tag){
_LL1246: _temp1245=(( struct Cyc_Attribute_tok_struct*) _temp1239)->f1; goto
_LL1242;} else{ goto _LL1243;} _LL1243: goto _LL1244; _LL1242: return _temp1245;
_LL1244:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1247=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp1247=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp1248=( char*)"Attribute_tok";
struct _tagged_string _temp1249; _temp1249.curr= _temp1248; _temp1249.base=
_temp1248; _temp1249.last_plus_one= _temp1248 + 14; _temp1249;})};( struct
_xenum_struct*) _temp1247;})); _LL1240:;} struct Cyc_Yyltype{ int timestamp; int
first_line; int first_column; int last_line; int last_column; struct
_tagged_string text; } ; typedef struct Cyc_Yyltype Cyc_yyltype; struct Cyc_Yyltype
Cyc_yynewloc(){ return({ struct Cyc_Yyltype _temp1250; _temp1250.timestamp= 0;
_temp1250.first_line= 0; _temp1250.first_column= 0; _temp1250.last_line= 0;
_temp1250.last_column= 0; _temp1250.text=( struct _tagged_string)({ char*
_temp1251=( char*)""; struct _tagged_string _temp1252; _temp1252.curr= _temp1251;
_temp1252.base= _temp1251; _temp1252.last_plus_one= _temp1251 + 1; _temp1252;});
_temp1250;});} static char _temp1255[ 1u]=""; struct Cyc_Yyltype Cyc_yylloc=(
struct Cyc_Yyltype){.timestamp= 0,.first_line= 0,.first_column= 0,.last_line= 0,.last_column=
0,.text=( struct _tagged_string){ _temp1255, _temp1255, _temp1255 + 1u}}; static
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
_temp1258[ 2u]="$"; static char _temp1261[ 6u]="error"; static char _temp1264[
12u]="$undefined."; static char _temp1267[ 5u]="AUTO"; static char _temp1270[ 9u]="REGISTER";
static char _temp1273[ 7u]="STATIC"; static char _temp1276[ 7u]="EXTERN"; static
char _temp1279[ 8u]="TYPEDEF"; static char _temp1282[ 5u]="VOID"; static char
_temp1285[ 5u]="CHAR"; static char _temp1288[ 6u]="SHORT"; static char _temp1291[
4u]="INT"; static char _temp1294[ 5u]="LONG"; static char _temp1297[ 6u]="FLOAT";
static char _temp1300[ 7u]="DOUBLE"; static char _temp1303[ 7u]="SIGNED"; static
char _temp1306[ 9u]="UNSIGNED"; static char _temp1309[ 6u]="CONST"; static char
_temp1312[ 9u]="VOLATILE"; static char _temp1315[ 9u]="RESTRICT"; static char
_temp1318[ 7u]="STRUCT"; static char _temp1321[ 6u]="UNION"; static char
_temp1324[ 5u]="CASE"; static char _temp1327[ 8u]="DEFAULT"; static char
_temp1330[ 7u]="INLINE"; static char _temp1333[ 3u]="IF"; static char _temp1336[
5u]="ELSE"; static char _temp1339[ 7u]="SWITCH"; static char _temp1342[ 6u]="WHILE";
static char _temp1345[ 3u]="DO"; static char _temp1348[ 4u]="FOR"; static char
_temp1351[ 5u]="GOTO"; static char _temp1354[ 9u]="CONTINUE"; static char
_temp1357[ 6u]="BREAK"; static char _temp1360[ 7u]="RETURN"; static char
_temp1363[ 7u]="SIZEOF"; static char _temp1366[ 5u]="ENUM"; static char
_temp1369[ 8u]="NULL_kw"; static char _temp1372[ 4u]="LET"; static char
_temp1375[ 6u]="THROW"; static char _temp1378[ 4u]="TRY"; static char _temp1381[
6u]="CATCH"; static char _temp1384[ 4u]="NEW"; static char _temp1387[ 9u]="ABSTRACT";
static char _temp1390[ 9u]="FALLTHRU"; static char _temp1393[ 6u]="USING";
static char _temp1396[ 10u]="NAMESPACE"; static char _temp1399[ 6u]="XENUM";
static char _temp1402[ 5u]="FILL"; static char _temp1405[ 8u]="CODEGEN"; static
char _temp1408[ 4u]="CUT"; static char _temp1411[ 7u]="SPLICE"; static char
_temp1414[ 7u]="PRINTF"; static char _temp1417[ 8u]="FPRINTF"; static char
_temp1420[ 8u]="XPRINTF"; static char _temp1423[ 6u]="SCANF"; static char
_temp1426[ 7u]="FSCANF"; static char _temp1429[ 7u]="SSCANF"; static char
_temp1432[ 7u]="MALLOC"; static char _temp1435[ 7u]="PTR_OP"; static char
_temp1438[ 7u]="INC_OP"; static char _temp1441[ 7u]="DEC_OP"; static char
_temp1444[ 8u]="LEFT_OP"; static char _temp1447[ 9u]="RIGHT_OP"; static char
_temp1450[ 6u]="LE_OP"; static char _temp1453[ 6u]="GE_OP"; static char
_temp1456[ 6u]="EQ_OP"; static char _temp1459[ 6u]="NE_OP"; static char
_temp1462[ 7u]="AND_OP"; static char _temp1465[ 6u]="OR_OP"; static char
_temp1468[ 11u]="MUL_ASSIGN"; static char _temp1471[ 11u]="DIV_ASSIGN"; static
char _temp1474[ 11u]="MOD_ASSIGN"; static char _temp1477[ 11u]="ADD_ASSIGN";
static char _temp1480[ 11u]="SUB_ASSIGN"; static char _temp1483[ 12u]="LEFT_ASSIGN";
static char _temp1486[ 13u]="RIGHT_ASSIGN"; static char _temp1489[ 11u]="AND_ASSIGN";
static char _temp1492[ 11u]="XOR_ASSIGN"; static char _temp1495[ 10u]="OR_ASSIGN";
static char _temp1498[ 9u]="ELLIPSIS"; static char _temp1501[ 11u]="LEFT_RIGHT";
static char _temp1504[ 12u]="COLON_COLON"; static char _temp1507[ 11u]="IDENTIFIER";
static char _temp1510[ 17u]="INTEGER_CONSTANT"; static char _temp1513[ 7u]="STRING";
static char _temp1516[ 19u]="CHARACTER_CONSTANT"; static char _temp1519[ 18u]="FLOATING_CONSTANT";
static char _temp1522[ 9u]="TYPE_VAR"; static char _temp1525[ 16u]="QUAL_IDENTIFIER";
static char _temp1528[ 18u]="QUAL_TYPEDEF_NAME"; static char _temp1531[ 10u]="ATTRIBUTE";
static char _temp1534[ 4u]="';'"; static char _temp1537[ 4u]="'{'"; static char
_temp1540[ 4u]="'}'"; static char _temp1543[ 4u]="'='"; static char _temp1546[ 4u]="'('";
static char _temp1549[ 4u]="')'"; static char _temp1552[ 4u]="','"; static char
_temp1555[ 4u]="'_'"; static char _temp1558[ 4u]="'$'"; static char _temp1561[ 4u]="'<'";
static char _temp1564[ 4u]="'>'"; static char _temp1567[ 4u]="':'"; static char
_temp1570[ 4u]="'['"; static char _temp1573[ 4u]="']'"; static char _temp1576[ 4u]="'*'";
static char _temp1579[ 4u]="'@'"; static char _temp1582[ 4u]="'?'"; static char
_temp1585[ 4u]="'+'"; static char _temp1588[ 4u]="'.'"; static char _temp1591[ 4u]="'-'";
static char _temp1594[ 4u]="'&'"; static char _temp1597[ 4u]="'|'"; static char
_temp1600[ 4u]="'^'"; static char _temp1603[ 4u]="'/'"; static char _temp1606[ 4u]="'%'";
static char _temp1609[ 4u]="'~'"; static char _temp1612[ 4u]="'!'"; static char
_temp1615[ 5u]="prog"; static char _temp1618[ 17u]="translation_unit"; static
char _temp1621[ 21u]="translation_unit_opt"; static char _temp1624[ 21u]="external_declaration";
static char _temp1627[ 20u]="function_definition"; static char _temp1630[ 21u]="function_definition2";
static char _temp1633[ 13u]="using_action"; static char _temp1636[ 15u]="unusing_action";
static char _temp1639[ 17u]="namespace_action"; static char _temp1642[ 19u]="unnamespace_action";
static char _temp1645[ 12u]="declaration"; static char _temp1648[ 17u]="declaration_list";
static char _temp1651[ 23u]="declaration_specifiers"; static char _temp1654[ 24u]="storage_class_specifier";
static char _temp1657[ 15u]="attributes_opt"; static char _temp1660[ 11u]="attributes";
static char _temp1663[ 15u]="attribute_list"; static char _temp1666[ 10u]="attribute";
static char _temp1669[ 15u]="type_specifier"; static char _temp1672[ 5u]="kind";
static char _temp1675[ 15u]="type_qualifier"; static char _temp1678[ 26u]="struct_or_union_specifier";
static char _temp1681[ 16u]="type_params_opt"; static char _temp1684[ 16u]="struct_or_union";
static char _temp1687[ 24u]="struct_declaration_list"; static char _temp1690[ 25u]="struct_declaration_list0";
static char _temp1693[ 21u]="init_declarator_list"; static char _temp1696[ 22u]="init_declarator_list0";
static char _temp1699[ 16u]="init_declarator"; static char _temp1702[ 19u]="struct_declaration";
static char _temp1705[ 25u]="specifier_qualifier_list"; static char _temp1708[
23u]="struct_declarator_list"; static char _temp1711[ 24u]="struct_declarator_list0";
static char _temp1714[ 18u]="struct_declarator"; static char _temp1717[ 15u]="enum_specifier";
static char _temp1720[ 16u]="enumerator_list"; static char _temp1723[ 11u]="enumerator";
static char _temp1726[ 11u]="declarator"; static char _temp1729[ 18u]="direct_declarator";
static char _temp1732[ 8u]="pointer"; static char _temp1735[ 13u]="pointer_char";
static char _temp1738[ 8u]="rgn_opt"; static char _temp1741[ 20u]="type_qualifier_list";
static char _temp1744[ 20u]="parameter_type_list"; static char _temp1747[ 11u]="effect_set";
static char _temp1750[ 14u]="atomic_effect"; static char _temp1753[ 11u]="region_set";
static char _temp1756[ 15u]="parameter_list"; static char _temp1759[ 22u]="parameter_declaration";
static char _temp1762[ 16u]="identifier_list"; static char _temp1765[ 17u]="identifier_list0";
static char _temp1768[ 12u]="initializer"; static char _temp1771[ 17u]="initializer_list";
static char _temp1774[ 12u]="designation"; static char _temp1777[ 16u]="designator_list";
static char _temp1780[ 11u]="designator"; static char _temp1783[ 10u]="type_name";
static char _temp1786[ 14u]="any_type_name"; static char _temp1789[ 15u]="type_name_list";
static char _temp1792[ 20u]="abstract_declarator"; static char _temp1795[ 27u]="direct_abstract_declarator";
static char _temp1798[ 10u]="statement"; static char _temp1801[ 18u]="labeled_statement";
static char _temp1804[ 21u]="expression_statement"; static char _temp1807[ 19u]="compound_statement";
static char _temp1810[ 16u]="block_item_list"; static char _temp1813[ 11u]="block_item";
static char _temp1816[ 20u]="selection_statement"; static char _temp1819[ 15u]="switch_clauses";
static char _temp1822[ 20u]="iteration_statement"; static char _temp1825[ 15u]="jump_statement";
static char _temp1828[ 8u]="pattern"; static char _temp1831[ 19u]="tuple_pattern_list";
static char _temp1834[ 20u]="tuple_pattern_list0"; static char _temp1837[ 14u]="field_pattern";
static char _temp1840[ 19u]="field_pattern_list"; static char _temp1843[ 20u]="field_pattern_list0";
static char _temp1846[ 11u]="expression"; static char _temp1849[ 22u]="assignment_expression";
static char _temp1852[ 20u]="assignment_operator"; static char _temp1855[ 23u]="conditional_expression";
static char _temp1858[ 20u]="constant_expression"; static char _temp1861[ 22u]="logical_or_expression";
static char _temp1864[ 23u]="logical_and_expression"; static char _temp1867[ 24u]="inclusive_or_expression";
static char _temp1870[ 24u]="exclusive_or_expression"; static char _temp1873[ 15u]="and_expression";
static char _temp1876[ 20u]="equality_expression"; static char _temp1879[ 22u]="relational_expression";
static char _temp1882[ 17u]="shift_expression"; static char _temp1885[ 20u]="additive_expression";
static char _temp1888[ 26u]="multiplicative_expression"; static char _temp1891[
16u]="cast_expression"; static char _temp1894[ 17u]="unary_expression"; static
char _temp1897[ 14u]="format_primop"; static char _temp1900[ 15u]="unary_operator";
static char _temp1903[ 19u]="postfix_expression"; static char _temp1906[ 19u]="primary_expression";
static char _temp1909[ 25u]="argument_expression_list"; static char _temp1912[
26u]="argument_expression_list0"; static char _temp1915[ 9u]="constant"; static
char _temp1918[ 20u]="qual_opt_identifier"; static struct _tagged_string Cyc_yytname[
221u]={( struct _tagged_string){ _temp1258, _temp1258, _temp1258 + 2u},( struct
_tagged_string){ _temp1261, _temp1261, _temp1261 + 6u},( struct _tagged_string){
_temp1264, _temp1264, _temp1264 + 12u},( struct _tagged_string){ _temp1267,
_temp1267, _temp1267 + 5u},( struct _tagged_string){ _temp1270, _temp1270,
_temp1270 + 9u},( struct _tagged_string){ _temp1273, _temp1273, _temp1273 + 7u},(
struct _tagged_string){ _temp1276, _temp1276, _temp1276 + 7u},( struct
_tagged_string){ _temp1279, _temp1279, _temp1279 + 8u},( struct _tagged_string){
_temp1282, _temp1282, _temp1282 + 5u},( struct _tagged_string){ _temp1285,
_temp1285, _temp1285 + 5u},( struct _tagged_string){ _temp1288, _temp1288,
_temp1288 + 6u},( struct _tagged_string){ _temp1291, _temp1291, _temp1291 + 4u},(
struct _tagged_string){ _temp1294, _temp1294, _temp1294 + 5u},( struct
_tagged_string){ _temp1297, _temp1297, _temp1297 + 6u},( struct _tagged_string){
_temp1300, _temp1300, _temp1300 + 7u},( struct _tagged_string){ _temp1303,
_temp1303, _temp1303 + 7u},( struct _tagged_string){ _temp1306, _temp1306,
_temp1306 + 9u},( struct _tagged_string){ _temp1309, _temp1309, _temp1309 + 6u},(
struct _tagged_string){ _temp1312, _temp1312, _temp1312 + 9u},( struct
_tagged_string){ _temp1315, _temp1315, _temp1315 + 9u},( struct _tagged_string){
_temp1318, _temp1318, _temp1318 + 7u},( struct _tagged_string){ _temp1321,
_temp1321, _temp1321 + 6u},( struct _tagged_string){ _temp1324, _temp1324,
_temp1324 + 5u},( struct _tagged_string){ _temp1327, _temp1327, _temp1327 + 8u},(
struct _tagged_string){ _temp1330, _temp1330, _temp1330 + 7u},( struct
_tagged_string){ _temp1333, _temp1333, _temp1333 + 3u},( struct _tagged_string){
_temp1336, _temp1336, _temp1336 + 5u},( struct _tagged_string){ _temp1339,
_temp1339, _temp1339 + 7u},( struct _tagged_string){ _temp1342, _temp1342,
_temp1342 + 6u},( struct _tagged_string){ _temp1345, _temp1345, _temp1345 + 3u},(
struct _tagged_string){ _temp1348, _temp1348, _temp1348 + 4u},( struct
_tagged_string){ _temp1351, _temp1351, _temp1351 + 5u},( struct _tagged_string){
_temp1354, _temp1354, _temp1354 + 9u},( struct _tagged_string){ _temp1357,
_temp1357, _temp1357 + 6u},( struct _tagged_string){ _temp1360, _temp1360,
_temp1360 + 7u},( struct _tagged_string){ _temp1363, _temp1363, _temp1363 + 7u},(
struct _tagged_string){ _temp1366, _temp1366, _temp1366 + 5u},( struct
_tagged_string){ _temp1369, _temp1369, _temp1369 + 8u},( struct _tagged_string){
_temp1372, _temp1372, _temp1372 + 4u},( struct _tagged_string){ _temp1375,
_temp1375, _temp1375 + 6u},( struct _tagged_string){ _temp1378, _temp1378,
_temp1378 + 4u},( struct _tagged_string){ _temp1381, _temp1381, _temp1381 + 6u},(
struct _tagged_string){ _temp1384, _temp1384, _temp1384 + 4u},( struct
_tagged_string){ _temp1387, _temp1387, _temp1387 + 9u},( struct _tagged_string){
_temp1390, _temp1390, _temp1390 + 9u},( struct _tagged_string){ _temp1393,
_temp1393, _temp1393 + 6u},( struct _tagged_string){ _temp1396, _temp1396,
_temp1396 + 10u},( struct _tagged_string){ _temp1399, _temp1399, _temp1399 + 6u},(
struct _tagged_string){ _temp1402, _temp1402, _temp1402 + 5u},( struct
_tagged_string){ _temp1405, _temp1405, _temp1405 + 8u},( struct _tagged_string){
_temp1408, _temp1408, _temp1408 + 4u},( struct _tagged_string){ _temp1411,
_temp1411, _temp1411 + 7u},( struct _tagged_string){ _temp1414, _temp1414,
_temp1414 + 7u},( struct _tagged_string){ _temp1417, _temp1417, _temp1417 + 8u},(
struct _tagged_string){ _temp1420, _temp1420, _temp1420 + 8u},( struct
_tagged_string){ _temp1423, _temp1423, _temp1423 + 6u},( struct _tagged_string){
_temp1426, _temp1426, _temp1426 + 7u},( struct _tagged_string){ _temp1429,
_temp1429, _temp1429 + 7u},( struct _tagged_string){ _temp1432, _temp1432,
_temp1432 + 7u},( struct _tagged_string){ _temp1435, _temp1435, _temp1435 + 7u},(
struct _tagged_string){ _temp1438, _temp1438, _temp1438 + 7u},( struct
_tagged_string){ _temp1441, _temp1441, _temp1441 + 7u},( struct _tagged_string){
_temp1444, _temp1444, _temp1444 + 8u},( struct _tagged_string){ _temp1447,
_temp1447, _temp1447 + 9u},( struct _tagged_string){ _temp1450, _temp1450,
_temp1450 + 6u},( struct _tagged_string){ _temp1453, _temp1453, _temp1453 + 6u},(
struct _tagged_string){ _temp1456, _temp1456, _temp1456 + 6u},( struct
_tagged_string){ _temp1459, _temp1459, _temp1459 + 6u},( struct _tagged_string){
_temp1462, _temp1462, _temp1462 + 7u},( struct _tagged_string){ _temp1465,
_temp1465, _temp1465 + 6u},( struct _tagged_string){ _temp1468, _temp1468,
_temp1468 + 11u},( struct _tagged_string){ _temp1471, _temp1471, _temp1471 + 11u},(
struct _tagged_string){ _temp1474, _temp1474, _temp1474 + 11u},( struct
_tagged_string){ _temp1477, _temp1477, _temp1477 + 11u},( struct _tagged_string){
_temp1480, _temp1480, _temp1480 + 11u},( struct _tagged_string){ _temp1483,
_temp1483, _temp1483 + 12u},( struct _tagged_string){ _temp1486, _temp1486,
_temp1486 + 13u},( struct _tagged_string){ _temp1489, _temp1489, _temp1489 + 11u},(
struct _tagged_string){ _temp1492, _temp1492, _temp1492 + 11u},( struct
_tagged_string){ _temp1495, _temp1495, _temp1495 + 10u},( struct _tagged_string){
_temp1498, _temp1498, _temp1498 + 9u},( struct _tagged_string){ _temp1501,
_temp1501, _temp1501 + 11u},( struct _tagged_string){ _temp1504, _temp1504,
_temp1504 + 12u},( struct _tagged_string){ _temp1507, _temp1507, _temp1507 + 11u},(
struct _tagged_string){ _temp1510, _temp1510, _temp1510 + 17u},( struct
_tagged_string){ _temp1513, _temp1513, _temp1513 + 7u},( struct _tagged_string){
_temp1516, _temp1516, _temp1516 + 19u},( struct _tagged_string){ _temp1519,
_temp1519, _temp1519 + 18u},( struct _tagged_string){ _temp1522, _temp1522,
_temp1522 + 9u},( struct _tagged_string){ _temp1525, _temp1525, _temp1525 + 16u},(
struct _tagged_string){ _temp1528, _temp1528, _temp1528 + 18u},( struct
_tagged_string){ _temp1531, _temp1531, _temp1531 + 10u},( struct _tagged_string){
_temp1534, _temp1534, _temp1534 + 4u},( struct _tagged_string){ _temp1537,
_temp1537, _temp1537 + 4u},( struct _tagged_string){ _temp1540, _temp1540,
_temp1540 + 4u},( struct _tagged_string){ _temp1543, _temp1543, _temp1543 + 4u},(
struct _tagged_string){ _temp1546, _temp1546, _temp1546 + 4u},( struct
_tagged_string){ _temp1549, _temp1549, _temp1549 + 4u},( struct _tagged_string){
_temp1552, _temp1552, _temp1552 + 4u},( struct _tagged_string){ _temp1555,
_temp1555, _temp1555 + 4u},( struct _tagged_string){ _temp1558, _temp1558,
_temp1558 + 4u},( struct _tagged_string){ _temp1561, _temp1561, _temp1561 + 4u},(
struct _tagged_string){ _temp1564, _temp1564, _temp1564 + 4u},( struct
_tagged_string){ _temp1567, _temp1567, _temp1567 + 4u},( struct _tagged_string){
_temp1570, _temp1570, _temp1570 + 4u},( struct _tagged_string){ _temp1573,
_temp1573, _temp1573 + 4u},( struct _tagged_string){ _temp1576, _temp1576,
_temp1576 + 4u},( struct _tagged_string){ _temp1579, _temp1579, _temp1579 + 4u},(
struct _tagged_string){ _temp1582, _temp1582, _temp1582 + 4u},( struct
_tagged_string){ _temp1585, _temp1585, _temp1585 + 4u},( struct _tagged_string){
_temp1588, _temp1588, _temp1588 + 4u},( struct _tagged_string){ _temp1591,
_temp1591, _temp1591 + 4u},( struct _tagged_string){ _temp1594, _temp1594,
_temp1594 + 4u},( struct _tagged_string){ _temp1597, _temp1597, _temp1597 + 4u},(
struct _tagged_string){ _temp1600, _temp1600, _temp1600 + 4u},( struct
_tagged_string){ _temp1603, _temp1603, _temp1603 + 4u},( struct _tagged_string){
_temp1606, _temp1606, _temp1606 + 4u},( struct _tagged_string){ _temp1609,
_temp1609, _temp1609 + 4u},( struct _tagged_string){ _temp1612, _temp1612,
_temp1612 + 4u},( struct _tagged_string){ _temp1615, _temp1615, _temp1615 + 5u},(
struct _tagged_string){ _temp1618, _temp1618, _temp1618 + 17u},( struct
_tagged_string){ _temp1621, _temp1621, _temp1621 + 21u},( struct _tagged_string){
_temp1624, _temp1624, _temp1624 + 21u},( struct _tagged_string){ _temp1627,
_temp1627, _temp1627 + 20u},( struct _tagged_string){ _temp1630, _temp1630,
_temp1630 + 21u},( struct _tagged_string){ _temp1633, _temp1633, _temp1633 + 13u},(
struct _tagged_string){ _temp1636, _temp1636, _temp1636 + 15u},( struct
_tagged_string){ _temp1639, _temp1639, _temp1639 + 17u},( struct _tagged_string){
_temp1642, _temp1642, _temp1642 + 19u},( struct _tagged_string){ _temp1645,
_temp1645, _temp1645 + 12u},( struct _tagged_string){ _temp1648, _temp1648,
_temp1648 + 17u},( struct _tagged_string){ _temp1651, _temp1651, _temp1651 + 23u},(
struct _tagged_string){ _temp1654, _temp1654, _temp1654 + 24u},( struct
_tagged_string){ _temp1657, _temp1657, _temp1657 + 15u},( struct _tagged_string){
_temp1660, _temp1660, _temp1660 + 11u},( struct _tagged_string){ _temp1663,
_temp1663, _temp1663 + 15u},( struct _tagged_string){ _temp1666, _temp1666,
_temp1666 + 10u},( struct _tagged_string){ _temp1669, _temp1669, _temp1669 + 15u},(
struct _tagged_string){ _temp1672, _temp1672, _temp1672 + 5u},( struct
_tagged_string){ _temp1675, _temp1675, _temp1675 + 15u},( struct _tagged_string){
_temp1678, _temp1678, _temp1678 + 26u},( struct _tagged_string){ _temp1681,
_temp1681, _temp1681 + 16u},( struct _tagged_string){ _temp1684, _temp1684,
_temp1684 + 16u},( struct _tagged_string){ _temp1687, _temp1687, _temp1687 + 24u},(
struct _tagged_string){ _temp1690, _temp1690, _temp1690 + 25u},( struct
_tagged_string){ _temp1693, _temp1693, _temp1693 + 21u},( struct _tagged_string){
_temp1696, _temp1696, _temp1696 + 22u},( struct _tagged_string){ _temp1699,
_temp1699, _temp1699 + 16u},( struct _tagged_string){ _temp1702, _temp1702,
_temp1702 + 19u},( struct _tagged_string){ _temp1705, _temp1705, _temp1705 + 25u},(
struct _tagged_string){ _temp1708, _temp1708, _temp1708 + 23u},( struct
_tagged_string){ _temp1711, _temp1711, _temp1711 + 24u},( struct _tagged_string){
_temp1714, _temp1714, _temp1714 + 18u},( struct _tagged_string){ _temp1717,
_temp1717, _temp1717 + 15u},( struct _tagged_string){ _temp1720, _temp1720,
_temp1720 + 16u},( struct _tagged_string){ _temp1723, _temp1723, _temp1723 + 11u},(
struct _tagged_string){ _temp1726, _temp1726, _temp1726 + 11u},( struct
_tagged_string){ _temp1729, _temp1729, _temp1729 + 18u},( struct _tagged_string){
_temp1732, _temp1732, _temp1732 + 8u},( struct _tagged_string){ _temp1735,
_temp1735, _temp1735 + 13u},( struct _tagged_string){ _temp1738, _temp1738,
_temp1738 + 8u},( struct _tagged_string){ _temp1741, _temp1741, _temp1741 + 20u},(
struct _tagged_string){ _temp1744, _temp1744, _temp1744 + 20u},( struct
_tagged_string){ _temp1747, _temp1747, _temp1747 + 11u},( struct _tagged_string){
_temp1750, _temp1750, _temp1750 + 14u},( struct _tagged_string){ _temp1753,
_temp1753, _temp1753 + 11u},( struct _tagged_string){ _temp1756, _temp1756,
_temp1756 + 15u},( struct _tagged_string){ _temp1759, _temp1759, _temp1759 + 22u},(
struct _tagged_string){ _temp1762, _temp1762, _temp1762 + 16u},( struct
_tagged_string){ _temp1765, _temp1765, _temp1765 + 17u},( struct _tagged_string){
_temp1768, _temp1768, _temp1768 + 12u},( struct _tagged_string){ _temp1771,
_temp1771, _temp1771 + 17u},( struct _tagged_string){ _temp1774, _temp1774,
_temp1774 + 12u},( struct _tagged_string){ _temp1777, _temp1777, _temp1777 + 16u},(
struct _tagged_string){ _temp1780, _temp1780, _temp1780 + 11u},( struct
_tagged_string){ _temp1783, _temp1783, _temp1783 + 10u},( struct _tagged_string){
_temp1786, _temp1786, _temp1786 + 14u},( struct _tagged_string){ _temp1789,
_temp1789, _temp1789 + 15u},( struct _tagged_string){ _temp1792, _temp1792,
_temp1792 + 20u},( struct _tagged_string){ _temp1795, _temp1795, _temp1795 + 27u},(
struct _tagged_string){ _temp1798, _temp1798, _temp1798 + 10u},( struct
_tagged_string){ _temp1801, _temp1801, _temp1801 + 18u},( struct _tagged_string){
_temp1804, _temp1804, _temp1804 + 21u},( struct _tagged_string){ _temp1807,
_temp1807, _temp1807 + 19u},( struct _tagged_string){ _temp1810, _temp1810,
_temp1810 + 16u},( struct _tagged_string){ _temp1813, _temp1813, _temp1813 + 11u},(
struct _tagged_string){ _temp1816, _temp1816, _temp1816 + 20u},( struct
_tagged_string){ _temp1819, _temp1819, _temp1819 + 15u},( struct _tagged_string){
_temp1822, _temp1822, _temp1822 + 20u},( struct _tagged_string){ _temp1825,
_temp1825, _temp1825 + 15u},( struct _tagged_string){ _temp1828, _temp1828,
_temp1828 + 8u},( struct _tagged_string){ _temp1831, _temp1831, _temp1831 + 19u},(
struct _tagged_string){ _temp1834, _temp1834, _temp1834 + 20u},( struct
_tagged_string){ _temp1837, _temp1837, _temp1837 + 14u},( struct _tagged_string){
_temp1840, _temp1840, _temp1840 + 19u},( struct _tagged_string){ _temp1843,
_temp1843, _temp1843 + 20u},( struct _tagged_string){ _temp1846, _temp1846,
_temp1846 + 11u},( struct _tagged_string){ _temp1849, _temp1849, _temp1849 + 22u},(
struct _tagged_string){ _temp1852, _temp1852, _temp1852 + 20u},( struct
_tagged_string){ _temp1855, _temp1855, _temp1855 + 23u},( struct _tagged_string){
_temp1858, _temp1858, _temp1858 + 20u},( struct _tagged_string){ _temp1861,
_temp1861, _temp1861 + 22u},( struct _tagged_string){ _temp1864, _temp1864,
_temp1864 + 23u},( struct _tagged_string){ _temp1867, _temp1867, _temp1867 + 24u},(
struct _tagged_string){ _temp1870, _temp1870, _temp1870 + 24u},( struct
_tagged_string){ _temp1873, _temp1873, _temp1873 + 15u},( struct _tagged_string){
_temp1876, _temp1876, _temp1876 + 20u},( struct _tagged_string){ _temp1879,
_temp1879, _temp1879 + 22u},( struct _tagged_string){ _temp1882, _temp1882,
_temp1882 + 17u},( struct _tagged_string){ _temp1885, _temp1885, _temp1885 + 20u},(
struct _tagged_string){ _temp1888, _temp1888, _temp1888 + 26u},( struct
_tagged_string){ _temp1891, _temp1891, _temp1891 + 16u},( struct _tagged_string){
_temp1894, _temp1894, _temp1894 + 17u},( struct _tagged_string){ _temp1897,
_temp1897, _temp1897 + 14u},( struct _tagged_string){ _temp1900, _temp1900,
_temp1900 + 15u},( struct _tagged_string){ _temp1903, _temp1903, _temp1903 + 19u},(
struct _tagged_string){ _temp1906, _temp1906, _temp1906 + 19u},( struct
_tagged_string){ _temp1909, _temp1909, _temp1909 + 25u},( struct _tagged_string){
_temp1912, _temp1912, _temp1912 + 26u},( struct _tagged_string){ _temp1915,
_temp1915, _temp1915 + 9u},( struct _tagged_string){ _temp1918, _temp1918,
_temp1918 + 20u}}; static short Cyc_yyr1[ 381u]={ (short)0, (short)119, (short)120,
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
static struct Cyc_YYINITIALSVAL_struct _temp1919={ Cyc_YYINITIALSVAL_tag};
struct _xenum_struct* Cyc_yylval=( struct _xenum_struct*)& _temp1919; static int
Cyc_yynerrs= 0; int Cyc_yyparse(){ int yystate; int yyn= 0; int yyerrstatus; int
yychar1= 0; int yyssp_offset; struct _tagged_ptr1 yyss=({ unsigned int _temp1920=(
unsigned int) 200; short* _temp1921=( short*) GC_malloc_atomic( sizeof( short) *
_temp1920); unsigned int i; struct _tagged_ptr1 _temp1922={ _temp1921, _temp1921,
_temp1921 + _temp1920}; for( i= 0; i < _temp1920; i ++){ _temp1921[ i]= (short)0;}
_temp1922;}); int yyvsp_offset; struct _tagged_ptr2 yyvs=({ unsigned int
_temp1923=( unsigned int) 200; struct _xenum_struct** _temp1924=( struct
_xenum_struct**) GC_malloc( sizeof( struct _xenum_struct*) * _temp1923);
unsigned int i; struct _tagged_ptr2 _temp1925={ _temp1924, _temp1924, _temp1924
+ _temp1923}; for( i= 0; i < _temp1923; i ++){ _temp1924[ i]= Cyc_yylval;}
_temp1925;}); int yylsp_offset; struct _tagged_ptr3 yyls=({ unsigned int
_temp1926=( unsigned int) 200; struct Cyc_Yyltype* _temp1927=( struct Cyc_Yyltype*)
GC_malloc( sizeof( struct Cyc_Yyltype) * _temp1926); unsigned int i; struct
_tagged_ptr3 _temp1928={ _temp1927, _temp1927, _temp1927 + _temp1926}; for( i= 0;
i < _temp1926; i ++){ _temp1927[ i]= Cyc_yynewloc();} _temp1928;}); int
yystacksize= 200; struct _xenum_struct* yyval= Cyc_yylval; int yylen; yystate= 0;
yyerrstatus= 0; Cyc_yynerrs= 0; Cyc_yychar= - 2; yyssp_offset= - 1; yyvsp_offset=
0; yylsp_offset= 0; yynewstate:({ struct _tagged_ptr1 _temp1929= yyss; short*
_temp1931= _temp1929.curr +( ++ yyssp_offset); if( _temp1931 < _temp1929.base? 1:
_temp1931 >= _temp1929.last_plus_one){ _throw( Null_Exception);}* _temp1931=(
short) yystate;}); if( yyssp_offset >= yystacksize - 1){ if( yystacksize >=
10000){ Cyc_yyerror(( struct _tagged_string)({ char* _temp1932=( char*)"parser stack overflow";
struct _tagged_string _temp1933; _temp1933.curr= _temp1932; _temp1933.base=
_temp1932; _temp1933.last_plus_one= _temp1932 + 22; _temp1933;}));( void) _throw(({
struct Cyc_Yystack_overflow_struct* _temp1934=( struct Cyc_Yystack_overflow_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Yystack_overflow_struct));* _temp1934=(
struct Cyc_Yystack_overflow_struct){.tag= Cyc_Yystack_overflow_tag};( struct
_xenum_struct*) _temp1934;}));} yystacksize *= 2; if( yystacksize > 10000){
yystacksize= 10000;}{ struct _tagged_ptr1 yyss1=({ unsigned int _temp1935=(
unsigned int) yystacksize; short* _temp1936=( short*) GC_malloc_atomic( sizeof(
short) * _temp1935); unsigned int i; struct _tagged_ptr1 _temp1937={ _temp1936,
_temp1936, _temp1936 + _temp1935}; for( i= 0; i < _temp1935; i ++){ _temp1936[ i]=
(short)0;} _temp1937;}); struct _tagged_ptr2 yyvs1=({ unsigned int _temp1938=(
unsigned int) yystacksize; struct _xenum_struct** _temp1939=( struct
_xenum_struct**) GC_malloc( sizeof( struct _xenum_struct*) * _temp1938);
unsigned int i; struct _tagged_ptr2 _temp1940={ _temp1939, _temp1939, _temp1939
+ _temp1938}; for( i= 0; i < _temp1938; i ++){ _temp1939[ i]= Cyc_yylval;}
_temp1940;}); struct _tagged_ptr3 yyls1=({ unsigned int _temp1941=( unsigned int)
yystacksize; struct Cyc_Yyltype* _temp1942=( struct Cyc_Yyltype*) GC_malloc(
sizeof( struct Cyc_Yyltype) * _temp1941); unsigned int i; struct _tagged_ptr3
_temp1943={ _temp1942, _temp1942, _temp1942 + _temp1941}; for( i= 0; i <
_temp1941; i ++){ _temp1942[ i]= Cyc_yynewloc();} _temp1943;});{ int i= 0; for(
0; i <= yyssp_offset; i ++){({ struct _tagged_ptr1 _temp1944= yyss1; short*
_temp1946= _temp1944.curr + i; if( _temp1946 < _temp1944.base? 1: _temp1946 >=
_temp1944.last_plus_one){ _throw( Null_Exception);}* _temp1946=({ struct
_tagged_ptr1 _temp1947= yyss; short* _temp1949= _temp1947.curr + i; if(
_temp1949 < _temp1947.base? 1: _temp1949 >= _temp1947.last_plus_one){ _throw(
Null_Exception);}* _temp1949;});});({ struct _tagged_ptr2 _temp1950= yyvs1;
struct _xenum_struct** _temp1952= _temp1950.curr + i; if( _temp1952 < _temp1950.base?
1: _temp1952 >= _temp1950.last_plus_one){ _throw( Null_Exception);}* _temp1952=({
struct _tagged_ptr2 _temp1953= yyvs; struct _xenum_struct** _temp1955= _temp1953.curr
+ i; if( _temp1955 < _temp1953.base? 1: _temp1955 >= _temp1953.last_plus_one){
_throw( Null_Exception);}* _temp1955;});});({ struct _tagged_ptr3 _temp1956=
yyls1; struct Cyc_Yyltype* _temp1958= _temp1956.curr + i; if( _temp1958 <
_temp1956.base? 1: _temp1958 >= _temp1956.last_plus_one){ _throw( Null_Exception);}*
_temp1958=({ struct _tagged_ptr3 _temp1959= yyls; struct Cyc_Yyltype* _temp1961=
_temp1959.curr + i; if( _temp1961 < _temp1959.base? 1: _temp1961 >= _temp1959.last_plus_one){
_throw( Null_Exception);}* _temp1961;});});}} yyss= yyss1; yyvs= yyvs1; yyls=
yyls1;}} goto yybackup; yybackup: yyn=( int)({ short* _temp1962=( short*) Cyc_yypact;
unsigned int _temp1963= yystate; if( _temp1963 >= 717u){ _throw( Null_Exception);}
_temp1962[ _temp1963];}); if( yyn == - 32768){ goto yydefault;} if( Cyc_yychar
== - 2){ Cyc_yychar= Cyc_yylex();} if( Cyc_yychar <= 0){ yychar1= 0; Cyc_yychar=
0;} else{ yychar1=( Cyc_yychar > 0? Cyc_yychar <= 346: 0)?( int)({ short*
_temp1964=( short*) Cyc_yytranslate; unsigned int _temp1965= Cyc_yychar; if(
_temp1965 >= 347u){ _throw( Null_Exception);} _temp1964[ _temp1965];}): 221;}
yyn += yychar1; if(( yyn < 0? 1: yyn > 4314)? 1:( int)({ short* _temp1966=(
short*) Cyc_yycheck; unsigned int _temp1967= yyn; if( _temp1967 >= 4315u){
_throw( Null_Exception);} _temp1966[ _temp1967];}) != yychar1){ goto yydefault;}
yyn=( int)({ short* _temp1968=( short*) Cyc_yytable; unsigned int _temp1969= yyn;
if( _temp1969 >= 4315u){ _throw( Null_Exception);} _temp1968[ _temp1969];}); if(
yyn < 0){ if( yyn == - 32768){ goto yyerrlab;} yyn= - yyn; goto yyreduce;} else{
if( yyn == 0){ goto yyerrlab;}} if( yyn == 716){ return 0;} if( Cyc_yychar != 0){
Cyc_yychar= - 2;}({ struct _tagged_ptr2 _temp1970= yyvs; struct _xenum_struct**
_temp1972= _temp1970.curr +( ++ yyvsp_offset); if( _temp1972 < _temp1970.base? 1:
_temp1972 >= _temp1970.last_plus_one){ _throw( Null_Exception);}* _temp1972= Cyc_yylval;});({
struct _tagged_ptr3 _temp1973= yyls; struct Cyc_Yyltype* _temp1975= _temp1973.curr
+( ++ yylsp_offset); if( _temp1975 < _temp1973.base? 1: _temp1975 >= _temp1973.last_plus_one){
_throw( Null_Exception);}* _temp1975= Cyc_yylloc;}); if( yyerrstatus != 0){
yyerrstatus --;} yystate= yyn; goto yynewstate; yydefault: yyn=( int)({ short*
_temp1976=( short*) Cyc_yydefact; unsigned int _temp1977= yystate; if( _temp1977
>= 717u){ _throw( Null_Exception);} _temp1976[ _temp1977];}); if( yyn == 0){
goto yyerrlab;} yyreduce: yylen=( int)({ short* _temp1978=( short*) Cyc_yyr2;
unsigned int _temp1979= yyn; if( _temp1979 >= 381u){ _throw( Null_Exception);}
_temp1978[ _temp1979];}); if( yylen > 0){ yyval=({ struct _tagged_ptr2 _temp1980=
yyvs; struct _xenum_struct** _temp1982= _temp1980.curr +(( yyvsp_offset + 1) -
yylen); if( _temp1982 < _temp1980.base? 1: _temp1982 >= _temp1980.last_plus_one){
_throw( Null_Exception);}* _temp1982;});} switch( yyn){ case 1: _LL1983: yyval=({
struct _tagged_ptr2 _temp1985= yyvs; struct _xenum_struct** _temp1987= _temp1985.curr
+ yyvsp_offset; if( _temp1987 < _temp1985.base? 1: _temp1987 >= _temp1985.last_plus_one){
_throw( Null_Exception);}* _temp1987;}); Cyc_Parse_parse_result= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp1988= yyvs; struct _xenum_struct** _temp1990= _temp1988.curr
+ yyvsp_offset; if( _temp1990 < _temp1988.base? 1: _temp1990 >= _temp1988.last_plus_one){
_throw( Null_Exception);}* _temp1990;})); break; case 2: _LL1984: yyval=({
struct _tagged_ptr2 _temp1992= yyvs; struct _xenum_struct** _temp1994= _temp1992.curr
+ yyvsp_offset; if( _temp1994 < _temp1992.base? 1: _temp1994 >= _temp1992.last_plus_one){
_throw( Null_Exception);}* _temp1994;}); break; case 3: _LL1991: yyval=({ struct
Cyc_DeclList_tok_struct* _temp1996=( struct Cyc_DeclList_tok_struct*) GC_malloc(
sizeof( struct Cyc_DeclList_tok_struct));* _temp1996=( struct Cyc_DeclList_tok_struct){.tag=
Cyc_DeclList_tok_tag,.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp1997= yyvs; struct _xenum_struct** _temp1999= _temp1997.curr +(
yyvsp_offset - 1); if( _temp1999 < _temp1997.base? 1: _temp1999 >= _temp1997.last_plus_one){
_throw( Null_Exception);}* _temp1999;})), Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp2000= yyvs; struct _xenum_struct** _temp2002= _temp2000.curr +
yyvsp_offset; if( _temp2002 < _temp2000.base? 1: _temp2002 >= _temp2000.last_plus_one){
_throw( Null_Exception);}* _temp2002;})))};( struct _xenum_struct*) _temp1996;});
break; case 4: _LL1995: yyval=({ struct Cyc_DeclList_tok_struct* _temp2004=(
struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));*
_temp2004=( struct Cyc_DeclList_tok_struct){.tag= Cyc_DeclList_tok_tag,.f1=({
struct Cyc_List_List* _temp2005=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2005->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2006=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2006->r=( void*)({ struct Cyc_Absyn_Using_d_struct* _temp2013=( struct Cyc_Absyn_Using_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct)); _temp2013->tag= Cyc_Absyn_Using_d_tag;
_temp2013->f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp2014= yyvs;
struct _xenum_struct** _temp2016= _temp2014.curr +( yyvsp_offset - 2); if(
_temp2016 < _temp2014.base? 1: _temp2016 >= _temp2014.last_plus_one){ _throw(
Null_Exception);}* _temp2016;})); _temp2013->f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2017= yyvs; struct _xenum_struct** _temp2019= _temp2017.curr
+ yyvsp_offset; if( _temp2019 < _temp2017.base? 1: _temp2019 >= _temp2017.last_plus_one){
_throw( Null_Exception);}* _temp2019;}));( void*) _temp2013;}); _temp2006->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2007= yyls; struct Cyc_Yyltype*
_temp2009= _temp2007.curr +( yylsp_offset - 2); if( _temp2009 < _temp2007.base?
1: _temp2009 >= _temp2007.last_plus_one){ _throw( Null_Exception);}* _temp2009;}).first_line,({
struct _tagged_ptr3 _temp2010= yyls; struct Cyc_Yyltype* _temp2012= _temp2010.curr
+ yylsp_offset; if( _temp2012 < _temp2010.base? 1: _temp2012 >= _temp2010.last_plus_one){
_throw( Null_Exception);}* _temp2012;}).last_line); _temp2006;}); _temp2005->tl=
0; _temp2005;})};( struct _xenum_struct*) _temp2004;}); Cyc_Lex_leave_using();
break; case 5: _LL2003: yyval=({ struct Cyc_DeclList_tok_struct* _temp2021=(
struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));*
_temp2021=( struct Cyc_DeclList_tok_struct){.tag= Cyc_DeclList_tok_tag,.f1=({
struct Cyc_List_List* _temp2022=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2022->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2026=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2026->r=( void*)({ struct Cyc_Absyn_Using_d_struct* _temp2033=( struct Cyc_Absyn_Using_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct)); _temp2033->tag= Cyc_Absyn_Using_d_tag;
_temp2033->f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp2034= yyvs;
struct _xenum_struct** _temp2036= _temp2034.curr +( yyvsp_offset - 4); if(
_temp2036 < _temp2034.base? 1: _temp2036 >= _temp2034.last_plus_one){ _throw(
Null_Exception);}* _temp2036;})); _temp2033->f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2037= yyvs; struct _xenum_struct** _temp2039= _temp2037.curr
+( yyvsp_offset - 2); if( _temp2039 < _temp2037.base? 1: _temp2039 >= _temp2037.last_plus_one){
_throw( Null_Exception);}* _temp2039;}));( void*) _temp2033;}); _temp2026->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2027= yyls; struct Cyc_Yyltype*
_temp2029= _temp2027.curr +( yylsp_offset - 4); if( _temp2029 < _temp2027.base?
1: _temp2029 >= _temp2027.last_plus_one){ _throw( Null_Exception);}* _temp2029;}).first_line,({
struct _tagged_ptr3 _temp2030= yyls; struct Cyc_Yyltype* _temp2032= _temp2030.curr
+( yylsp_offset - 1); if( _temp2032 < _temp2030.base? 1: _temp2032 >= _temp2030.last_plus_one){
_throw( Null_Exception);}* _temp2032;}).last_line); _temp2026;}); _temp2022->tl=
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2023= yyvs; struct
_xenum_struct** _temp2025= _temp2023.curr + yyvsp_offset; if( _temp2025 <
_temp2023.base? 1: _temp2025 >= _temp2023.last_plus_one){ _throw( Null_Exception);}*
_temp2025;})); _temp2022;})};( struct _xenum_struct*) _temp2021;}); break; case
6: _LL2020: yyval=({ struct Cyc_DeclList_tok_struct* _temp2041=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));* _temp2041=( struct Cyc_DeclList_tok_struct){.tag=
Cyc_DeclList_tok_tag,.f1=({ struct Cyc_List_List* _temp2042=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2042->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2043=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2043->r=( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp2050=( struct
Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp2050->tag= Cyc_Absyn_Namespace_d_tag; _temp2050->f1=({ struct
_tagged_string* _temp2051=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp2051[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp2052= yyvs; struct _xenum_struct** _temp2054= _temp2052.curr +(
yyvsp_offset - 2); if( _temp2054 < _temp2052.base? 1: _temp2054 >= _temp2052.last_plus_one){
_throw( Null_Exception);}* _temp2054;})); _temp2051;}); _temp2050->f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2055= yyvs; struct _xenum_struct** _temp2057= _temp2055.curr
+ yyvsp_offset; if( _temp2057 < _temp2055.base? 1: _temp2057 >= _temp2055.last_plus_one){
_throw( Null_Exception);}* _temp2057;}));( void*) _temp2050;}); _temp2043->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2044= yyls; struct Cyc_Yyltype*
_temp2046= _temp2044.curr +( yylsp_offset - 2); if( _temp2046 < _temp2044.base?
1: _temp2046 >= _temp2044.last_plus_one){ _throw( Null_Exception);}* _temp2046;}).first_line,({
struct _tagged_ptr3 _temp2047= yyls; struct Cyc_Yyltype* _temp2049= _temp2047.curr
+ yylsp_offset; if( _temp2049 < _temp2047.base? 1: _temp2049 >= _temp2047.last_plus_one){
_throw( Null_Exception);}* _temp2049;}).last_line); _temp2043;}); _temp2042->tl=
0; _temp2042;})};( struct _xenum_struct*) _temp2041;}); Cyc_Lex_leave_namespace();
break; case 7: _LL2040: yyval=({ struct Cyc_DeclList_tok_struct* _temp2059=(
struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));*
_temp2059=( struct Cyc_DeclList_tok_struct){.tag= Cyc_DeclList_tok_tag,.f1=({
struct Cyc_List_List* _temp2060=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2060->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2064=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2064->r=( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp2071=( struct
Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp2071->tag= Cyc_Absyn_Namespace_d_tag; _temp2071->f1=({ struct
_tagged_string* _temp2072=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp2072[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp2073= yyvs; struct _xenum_struct** _temp2075= _temp2073.curr +(
yyvsp_offset - 4); if( _temp2075 < _temp2073.base? 1: _temp2075 >= _temp2073.last_plus_one){
_throw( Null_Exception);}* _temp2075;})); _temp2072;}); _temp2071->f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2076= yyvs; struct _xenum_struct** _temp2078= _temp2076.curr
+( yyvsp_offset - 2); if( _temp2078 < _temp2076.base? 1: _temp2078 >= _temp2076.last_plus_one){
_throw( Null_Exception);}* _temp2078;}));( void*) _temp2071;}); _temp2064->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2065= yyls; struct Cyc_Yyltype*
_temp2067= _temp2065.curr +( yylsp_offset - 4); if( _temp2067 < _temp2065.base?
1: _temp2067 >= _temp2065.last_plus_one){ _throw( Null_Exception);}* _temp2067;}).first_line,({
struct _tagged_ptr3 _temp2068= yyls; struct Cyc_Yyltype* _temp2070= _temp2068.curr
+( yylsp_offset - 1); if( _temp2070 < _temp2068.base? 1: _temp2070 >= _temp2068.last_plus_one){
_throw( Null_Exception);}* _temp2070;}).last_line); _temp2064;}); _temp2060->tl=
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2061= yyvs; struct
_xenum_struct** _temp2063= _temp2061.curr + yyvsp_offset; if( _temp2063 <
_temp2061.base? 1: _temp2063 >= _temp2061.last_plus_one){ _throw( Null_Exception);}*
_temp2063;})); _temp2060;})};( struct _xenum_struct*) _temp2059;}); break; case
8: _LL2058: if( Cyc_String_strcmp( Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp2080= yyvs; struct _xenum_struct** _temp2082= _temp2080.curr +(
yyvsp_offset - 4); if( _temp2082 < _temp2080.base? 1: _temp2082 >= _temp2080.last_plus_one){
_throw( Null_Exception);}* _temp2082;})),( struct _tagged_string)({ char*
_temp2083=( char*)"C"; struct _tagged_string _temp2084; _temp2084.curr=
_temp2083; _temp2084.base= _temp2083; _temp2084.last_plus_one= _temp2083 + 2;
_temp2084;})) != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp2085=(
char*)"only extern \"C\" { ... } is allowed"; struct _tagged_string _temp2086;
_temp2086.curr= _temp2085; _temp2086.base= _temp2085; _temp2086.last_plus_one=
_temp2085 + 35; _temp2086;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2087= yyls; struct Cyc_Yyltype* _temp2089= _temp2087.curr +( yylsp_offset -
5); if( _temp2089 < _temp2087.base? 1: _temp2089 >= _temp2087.last_plus_one){
_throw( Null_Exception);}* _temp2089;}).first_line,({ struct _tagged_ptr3
_temp2090= yyls; struct Cyc_Yyltype* _temp2092= _temp2090.curr +( yylsp_offset -
4); if( _temp2092 < _temp2090.base? 1: _temp2092 >= _temp2090.last_plus_one){
_throw( Null_Exception);}* _temp2092;}).last_line));} yyval=({ struct Cyc_DeclList_tok_struct*
_temp2093=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));*
_temp2093=( struct Cyc_DeclList_tok_struct){.tag= Cyc_DeclList_tok_tag,.f1=({
struct Cyc_List_List* _temp2094=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2094->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2098=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2098->r=( void*)({ struct Cyc_Absyn_ExternC_d_struct* _temp2105=( struct
Cyc_Absyn_ExternC_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ExternC_d_struct));
_temp2105->tag= Cyc_Absyn_ExternC_d_tag; _temp2105->f1= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2106= yyvs; struct _xenum_struct** _temp2108= _temp2106.curr
+( yyvsp_offset - 2); if( _temp2108 < _temp2106.base? 1: _temp2108 >= _temp2106.last_plus_one){
_throw( Null_Exception);}* _temp2108;}));( void*) _temp2105;}); _temp2098->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2099= yyls; struct Cyc_Yyltype*
_temp2101= _temp2099.curr +( yylsp_offset - 5); if( _temp2101 < _temp2099.base?
1: _temp2101 >= _temp2099.last_plus_one){ _throw( Null_Exception);}* _temp2101;}).first_line,({
struct _tagged_ptr3 _temp2102= yyls; struct Cyc_Yyltype* _temp2104= _temp2102.curr
+( yylsp_offset - 1); if( _temp2104 < _temp2102.base? 1: _temp2104 >= _temp2102.last_plus_one){
_throw( Null_Exception);}* _temp2104;}).last_line); _temp2098;}); _temp2094->tl=
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2095= yyvs; struct
_xenum_struct** _temp2097= _temp2095.curr + yyvsp_offset; if( _temp2097 <
_temp2095.base? 1: _temp2097 >= _temp2095.last_plus_one){ _throw( Null_Exception);}*
_temp2097;})); _temp2094;})};( struct _xenum_struct*) _temp2093;}); break; case
9: _LL2079: yyval=({ struct Cyc_DeclList_tok_struct* _temp2110=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));* _temp2110=( struct Cyc_DeclList_tok_struct){.tag=
Cyc_DeclList_tok_tag,.f1= 0};( struct _xenum_struct*) _temp2110;}); break; case
10: _LL2109: yyval=({ struct _tagged_ptr2 _temp2112= yyvs; struct _xenum_struct**
_temp2114= _temp2112.curr + yyvsp_offset; if( _temp2114 < _temp2112.base? 1:
_temp2114 >= _temp2112.last_plus_one){ _throw( Null_Exception);}* _temp2114;});
break; case 11: _LL2111: yyval=({ struct Cyc_DeclList_tok_struct* _temp2116=(
struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));*
_temp2116=( struct Cyc_DeclList_tok_struct){.tag= Cyc_DeclList_tok_tag,.f1=({
struct Cyc_List_List* _temp2117=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2117->hd=( void*) Cyc_Absyn_new_decl(({ struct Cyc_Absyn_Fn_d_struct*
_temp2118=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2118->tag= Cyc_Absyn_Fn_d_tag; _temp2118->f1= Cyc_yyget_FnDecl_tok(({
struct _tagged_ptr2 _temp2119= yyvs; struct _xenum_struct** _temp2121= _temp2119.curr
+ yyvsp_offset; if( _temp2121 < _temp2119.base? 1: _temp2121 >= _temp2119.last_plus_one){
_throw( Null_Exception);}* _temp2121;}));( void*) _temp2118;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2122= yyls; struct Cyc_Yyltype* _temp2124= _temp2122.curr
+ yylsp_offset; if( _temp2124 < _temp2122.base? 1: _temp2124 >= _temp2122.last_plus_one){
_throw( Null_Exception);}* _temp2124;}).first_line,({ struct _tagged_ptr3
_temp2125= yyls; struct Cyc_Yyltype* _temp2127= _temp2125.curr + yylsp_offset;
if( _temp2127 < _temp2125.base? 1: _temp2127 >= _temp2125.last_plus_one){ _throw(
Null_Exception);}* _temp2127;}).last_line)); _temp2117->tl= 0; _temp2117;})};(
struct _xenum_struct*) _temp2116;}); break; case 12: _LL2115: yyval=({ struct
_tagged_ptr2 _temp2129= yyvs; struct _xenum_struct** _temp2131= _temp2129.curr +
yyvsp_offset; if( _temp2131 < _temp2129.base? 1: _temp2131 >= _temp2129.last_plus_one){
_throw( Null_Exception);}* _temp2131;}); break; case 13: _LL2128: yyval=({
struct Cyc_FnDecl_tok_struct* _temp2133=( struct Cyc_FnDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));* _temp2133=( struct Cyc_FnDecl_tok_struct){.tag=
Cyc_FnDecl_tok_tag,.f1= Cyc_Parse_make_function( 0, Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2134= yyvs; struct _xenum_struct** _temp2136= _temp2134.curr
+( yyvsp_offset - 1); if( _temp2136 < _temp2134.base? 1: _temp2136 >= _temp2134.last_plus_one){
_throw( Null_Exception);}* _temp2136;})), 0, Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp2137= yyvs; struct _xenum_struct** _temp2139= _temp2137.curr +
yyvsp_offset; if( _temp2139 < _temp2137.base? 1: _temp2139 >= _temp2137.last_plus_one){
_throw( Null_Exception);}* _temp2139;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2140= yyls; struct Cyc_Yyltype* _temp2142= _temp2140.curr +(
yylsp_offset - 1); if( _temp2142 < _temp2140.base? 1: _temp2142 >= _temp2140.last_plus_one){
_throw( Null_Exception);}* _temp2142;}).first_line,({ struct _tagged_ptr3
_temp2143= yyls; struct Cyc_Yyltype* _temp2145= _temp2143.curr + yylsp_offset;
if( _temp2145 < _temp2143.base? 1: _temp2145 >= _temp2143.last_plus_one){ _throw(
Null_Exception);}* _temp2145;}).last_line))};( struct _xenum_struct*) _temp2133;});
break; case 14: _LL2132: yyval=({ struct Cyc_FnDecl_tok_struct* _temp2147=(
struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));*
_temp2147=( struct Cyc_FnDecl_tok_struct){.tag= Cyc_FnDecl_tok_tag,.f1= Cyc_Parse_make_function(({
struct Cyc_Core_Opt* _temp2148=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2148->v=( void*) Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2149= yyvs; struct _xenum_struct** _temp2151= _temp2149.curr +(
yyvsp_offset - 2); if( _temp2151 < _temp2149.base? 1: _temp2151 >= _temp2149.last_plus_one){
_throw( Null_Exception);}* _temp2151;})); _temp2148;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2152= yyvs; struct _xenum_struct** _temp2154= _temp2152.curr
+( yyvsp_offset - 1); if( _temp2154 < _temp2152.base? 1: _temp2154 >= _temp2152.last_plus_one){
_throw( Null_Exception);}* _temp2154;})), 0, Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp2155= yyvs; struct _xenum_struct** _temp2157= _temp2155.curr +
yyvsp_offset; if( _temp2157 < _temp2155.base? 1: _temp2157 >= _temp2155.last_plus_one){
_throw( Null_Exception);}* _temp2157;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2158= yyls; struct Cyc_Yyltype* _temp2160= _temp2158.curr +(
yylsp_offset - 2); if( _temp2160 < _temp2158.base? 1: _temp2160 >= _temp2158.last_plus_one){
_throw( Null_Exception);}* _temp2160;}).first_line,({ struct _tagged_ptr3
_temp2161= yyls; struct Cyc_Yyltype* _temp2163= _temp2161.curr + yylsp_offset;
if( _temp2163 < _temp2161.base? 1: _temp2163 >= _temp2161.last_plus_one){ _throw(
Null_Exception);}* _temp2163;}).last_line))};( struct _xenum_struct*) _temp2147;});
break; case 15: _LL2146: yyval=({ struct Cyc_FnDecl_tok_struct* _temp2165=(
struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));*
_temp2165=( struct Cyc_FnDecl_tok_struct){.tag= Cyc_FnDecl_tok_tag,.f1= Cyc_Parse_make_function(
0, Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp2166= yyvs; struct
_xenum_struct** _temp2168= _temp2166.curr +( yyvsp_offset - 2); if( _temp2168 <
_temp2166.base? 1: _temp2168 >= _temp2166.last_plus_one){ _throw( Null_Exception);}*
_temp2168;})), Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2169= yyvs;
struct _xenum_struct** _temp2171= _temp2169.curr +( yyvsp_offset - 1); if(
_temp2171 < _temp2169.base? 1: _temp2171 >= _temp2169.last_plus_one){ _throw(
Null_Exception);}* _temp2171;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp2172= yyvs; struct _xenum_struct** _temp2174= _temp2172.curr + yyvsp_offset;
if( _temp2174 < _temp2172.base? 1: _temp2174 >= _temp2172.last_plus_one){ _throw(
Null_Exception);}* _temp2174;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2175= yyls; struct Cyc_Yyltype* _temp2177= _temp2175.curr +(
yylsp_offset - 2); if( _temp2177 < _temp2175.base? 1: _temp2177 >= _temp2175.last_plus_one){
_throw( Null_Exception);}* _temp2177;}).first_line,({ struct _tagged_ptr3
_temp2178= yyls; struct Cyc_Yyltype* _temp2180= _temp2178.curr + yylsp_offset;
if( _temp2180 < _temp2178.base? 1: _temp2180 >= _temp2178.last_plus_one){ _throw(
Null_Exception);}* _temp2180;}).last_line))};( struct _xenum_struct*) _temp2165;});
break; case 16: _LL2164: yyval=({ struct Cyc_FnDecl_tok_struct* _temp2182=(
struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));*
_temp2182=( struct Cyc_FnDecl_tok_struct){.tag= Cyc_FnDecl_tok_tag,.f1= Cyc_Parse_make_function(({
struct Cyc_Core_Opt* _temp2183=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2183->v=( void*) Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2184= yyvs; struct _xenum_struct** _temp2186= _temp2184.curr +(
yyvsp_offset - 3); if( _temp2186 < _temp2184.base? 1: _temp2186 >= _temp2184.last_plus_one){
_throw( Null_Exception);}* _temp2186;})); _temp2183;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2187= yyvs; struct _xenum_struct** _temp2189= _temp2187.curr
+( yyvsp_offset - 2); if( _temp2189 < _temp2187.base? 1: _temp2189 >= _temp2187.last_plus_one){
_throw( Null_Exception);}* _temp2189;})), Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp2190= yyvs; struct _xenum_struct** _temp2192= _temp2190.curr +(
yyvsp_offset - 1); if( _temp2192 < _temp2190.base? 1: _temp2192 >= _temp2190.last_plus_one){
_throw( Null_Exception);}* _temp2192;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp2193= yyvs; struct _xenum_struct** _temp2195= _temp2193.curr +
yyvsp_offset; if( _temp2195 < _temp2193.base? 1: _temp2195 >= _temp2193.last_plus_one){
_throw( Null_Exception);}* _temp2195;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2196= yyls; struct Cyc_Yyltype* _temp2198= _temp2196.curr +(
yylsp_offset - 3); if( _temp2198 < _temp2196.base? 1: _temp2198 >= _temp2196.last_plus_one){
_throw( Null_Exception);}* _temp2198;}).first_line,({ struct _tagged_ptr3
_temp2199= yyls; struct Cyc_Yyltype* _temp2201= _temp2199.curr + yylsp_offset;
if( _temp2201 < _temp2199.base? 1: _temp2201 >= _temp2199.last_plus_one){ _throw(
Null_Exception);}* _temp2201;}).last_line))};( struct _xenum_struct*) _temp2182;});
break; case 17: _LL2181: yyval=({ struct Cyc_FnDecl_tok_struct* _temp2203=(
struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));*
_temp2203=( struct Cyc_FnDecl_tok_struct){.tag= Cyc_FnDecl_tok_tag,.f1= Cyc_Parse_make_function(({
struct Cyc_Core_Opt* _temp2204=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2204->v=( void*) Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2205= yyvs; struct _xenum_struct** _temp2207= _temp2205.curr +(
yyvsp_offset - 2); if( _temp2207 < _temp2205.base? 1: _temp2207 >= _temp2205.last_plus_one){
_throw( Null_Exception);}* _temp2207;})); _temp2204;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2208= yyvs; struct _xenum_struct** _temp2210= _temp2208.curr
+( yyvsp_offset - 1); if( _temp2210 < _temp2208.base? 1: _temp2210 >= _temp2208.last_plus_one){
_throw( Null_Exception);}* _temp2210;})), 0, Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp2211= yyvs; struct _xenum_struct** _temp2213= _temp2211.curr +
yyvsp_offset; if( _temp2213 < _temp2211.base? 1: _temp2213 >= _temp2211.last_plus_one){
_throw( Null_Exception);}* _temp2213;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2214= yyls; struct Cyc_Yyltype* _temp2216= _temp2214.curr +(
yylsp_offset - 2); if( _temp2216 < _temp2214.base? 1: _temp2216 >= _temp2214.last_plus_one){
_throw( Null_Exception);}* _temp2216;}).first_line,({ struct _tagged_ptr3
_temp2217= yyls; struct Cyc_Yyltype* _temp2219= _temp2217.curr + yylsp_offset;
if( _temp2219 < _temp2217.base? 1: _temp2219 >= _temp2217.last_plus_one){ _throw(
Null_Exception);}* _temp2219;}).last_line))};( struct _xenum_struct*) _temp2203;});
break; case 18: _LL2202: yyval=({ struct Cyc_FnDecl_tok_struct* _temp2221=(
struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));*
_temp2221=( struct Cyc_FnDecl_tok_struct){.tag= Cyc_FnDecl_tok_tag,.f1= Cyc_Parse_make_function(({
struct Cyc_Core_Opt* _temp2222=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2222->v=( void*) Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2223= yyvs; struct _xenum_struct** _temp2225= _temp2223.curr +(
yyvsp_offset - 3); if( _temp2225 < _temp2223.base? 1: _temp2225 >= _temp2223.last_plus_one){
_throw( Null_Exception);}* _temp2225;})); _temp2222;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2226= yyvs; struct _xenum_struct** _temp2228= _temp2226.curr
+( yyvsp_offset - 2); if( _temp2228 < _temp2226.base? 1: _temp2228 >= _temp2226.last_plus_one){
_throw( Null_Exception);}* _temp2228;})), Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp2229= yyvs; struct _xenum_struct** _temp2231= _temp2229.curr +(
yyvsp_offset - 1); if( _temp2231 < _temp2229.base? 1: _temp2231 >= _temp2229.last_plus_one){
_throw( Null_Exception);}* _temp2231;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp2232= yyvs; struct _xenum_struct** _temp2234= _temp2232.curr +
yyvsp_offset; if( _temp2234 < _temp2232.base? 1: _temp2234 >= _temp2232.last_plus_one){
_throw( Null_Exception);}* _temp2234;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2235= yyls; struct Cyc_Yyltype* _temp2237= _temp2235.curr +(
yylsp_offset - 3); if( _temp2237 < _temp2235.base? 1: _temp2237 >= _temp2235.last_plus_one){
_throw( Null_Exception);}* _temp2237;}).first_line,({ struct _tagged_ptr3
_temp2238= yyls; struct Cyc_Yyltype* _temp2240= _temp2238.curr + yylsp_offset;
if( _temp2240 < _temp2238.base? 1: _temp2240 >= _temp2238.last_plus_one){ _throw(
Null_Exception);}* _temp2240;}).last_line))};( struct _xenum_struct*) _temp2221;});
break; case 19: _LL2220: Cyc_Lex_enter_using( Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp2242= yyvs; struct _xenum_struct** _temp2244= _temp2242.curr +
yyvsp_offset; if( _temp2244 < _temp2242.base? 1: _temp2244 >= _temp2242.last_plus_one){
_throw( Null_Exception);}* _temp2244;}))); yyval=({ struct _tagged_ptr2
_temp2245= yyvs; struct _xenum_struct** _temp2247= _temp2245.curr + yyvsp_offset;
if( _temp2247 < _temp2245.base? 1: _temp2247 >= _temp2245.last_plus_one){ _throw(
Null_Exception);}* _temp2247;}); break; case 20: _LL2241: Cyc_Lex_leave_using();
break; case 21: _LL2248: Cyc_Lex_enter_namespace(({ struct _tagged_string*
_temp2250=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp2250[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2251= yyvs;
struct _xenum_struct** _temp2253= _temp2251.curr + yyvsp_offset; if( _temp2253 <
_temp2251.base? 1: _temp2253 >= _temp2251.last_plus_one){ _throw( Null_Exception);}*
_temp2253;})); _temp2250;})); yyval=({ struct _tagged_ptr2 _temp2254= yyvs;
struct _xenum_struct** _temp2256= _temp2254.curr + yyvsp_offset; if( _temp2256 <
_temp2254.base? 1: _temp2256 >= _temp2254.last_plus_one){ _throw( Null_Exception);}*
_temp2256;}); break; case 22: _LL2249: Cyc_Lex_leave_namespace(); break; case 23:
_LL2257: yyval=({ struct Cyc_DeclList_tok_struct* _temp2259=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));* _temp2259=( struct Cyc_DeclList_tok_struct){.tag=
Cyc_DeclList_tok_tag,.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2260= yyvs; struct _xenum_struct** _temp2262= _temp2260.curr
+( yyvsp_offset - 1); if( _temp2262 < _temp2260.base? 1: _temp2262 >= _temp2260.last_plus_one){
_throw( Null_Exception);}* _temp2262;})), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2263= yyls; struct Cyc_Yyltype* _temp2265= _temp2263.curr
+( yylsp_offset - 1); if( _temp2265 < _temp2263.base? 1: _temp2265 >= _temp2263.last_plus_one){
_throw( Null_Exception);}* _temp2265;}).first_line,({ struct _tagged_ptr3
_temp2266= yyls; struct Cyc_Yyltype* _temp2268= _temp2266.curr +( yylsp_offset -
1); if( _temp2268 < _temp2266.base? 1: _temp2268 >= _temp2266.last_plus_one){
_throw( Null_Exception);}* _temp2268;}).last_line))};( struct _xenum_struct*)
_temp2259;}); break; case 24: _LL2258: yyval=({ struct Cyc_DeclList_tok_struct*
_temp2270=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));*
_temp2270=( struct Cyc_DeclList_tok_struct){.tag= Cyc_DeclList_tok_tag,.f1= Cyc_Parse_make_declarations(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2271= yyvs; struct
_xenum_struct** _temp2273= _temp2271.curr +( yyvsp_offset - 2); if( _temp2273 <
_temp2271.base? 1: _temp2273 >= _temp2271.last_plus_one){ _throw( Null_Exception);}*
_temp2273;})), Cyc_yyget_InitDeclList_tok(({ struct _tagged_ptr2 _temp2274= yyvs;
struct _xenum_struct** _temp2276= _temp2274.curr +( yyvsp_offset - 1); if(
_temp2276 < _temp2274.base? 1: _temp2276 >= _temp2274.last_plus_one){ _throw(
Null_Exception);}* _temp2276;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2277= yyls; struct Cyc_Yyltype* _temp2279= _temp2277.curr +(
yylsp_offset - 2); if( _temp2279 < _temp2277.base? 1: _temp2279 >= _temp2277.last_plus_one){
_throw( Null_Exception);}* _temp2279;}).first_line,({ struct _tagged_ptr3
_temp2280= yyls; struct Cyc_Yyltype* _temp2282= _temp2280.curr + yylsp_offset;
if( _temp2282 < _temp2280.base? 1: _temp2282 >= _temp2280.last_plus_one){ _throw(
Null_Exception);}* _temp2282;}).last_line))};( struct _xenum_struct*) _temp2270;});
break; case 25: _LL2269: yyval=({ struct Cyc_DeclList_tok_struct* _temp2284=(
struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));*
_temp2284=( struct Cyc_DeclList_tok_struct){.tag= Cyc_DeclList_tok_tag,.f1=({
struct Cyc_List_List* _temp2285=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2285->hd=( void*) Cyc_Absyn_let_decl( Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp2286= yyvs; struct _xenum_struct** _temp2288= _temp2286.curr
+( yyvsp_offset - 3); if( _temp2288 < _temp2286.base? 1: _temp2288 >= _temp2286.last_plus_one){
_throw( Null_Exception);}* _temp2288;})), 0, Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp2289= yyvs; struct _xenum_struct** _temp2291= _temp2289.curr +(
yyvsp_offset - 1); if( _temp2291 < _temp2289.base? 1: _temp2291 >= _temp2289.last_plus_one){
_throw( Null_Exception);}* _temp2291;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2292= yyls; struct Cyc_Yyltype* _temp2294= _temp2292.curr +(
yylsp_offset - 4); if( _temp2294 < _temp2292.base? 1: _temp2294 >= _temp2292.last_plus_one){
_throw( Null_Exception);}* _temp2294;}).first_line,({ struct _tagged_ptr3
_temp2295= yyls; struct Cyc_Yyltype* _temp2297= _temp2295.curr + yylsp_offset;
if( _temp2297 < _temp2295.base? 1: _temp2297 >= _temp2295.last_plus_one){ _throw(
Null_Exception);}* _temp2297;}).last_line)); _temp2285->tl= 0; _temp2285;})};(
struct _xenum_struct*) _temp2284;}); break; case 26: _LL2283: yyval=({ struct
_tagged_ptr2 _temp2299= yyvs; struct _xenum_struct** _temp2301= _temp2299.curr +
yyvsp_offset; if( _temp2301 < _temp2299.base? 1: _temp2301 >= _temp2299.last_plus_one){
_throw( Null_Exception);}* _temp2301;}); break; case 27: _LL2298: yyval=({
struct Cyc_DeclList_tok_struct* _temp2303=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));* _temp2303=( struct Cyc_DeclList_tok_struct){.tag=
Cyc_DeclList_tok_tag,.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp2304= yyvs; struct _xenum_struct** _temp2306= _temp2304.curr +(
yyvsp_offset - 1); if( _temp2306 < _temp2304.base? 1: _temp2306 >= _temp2304.last_plus_one){
_throw( Null_Exception);}* _temp2306;})), Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp2307= yyvs; struct _xenum_struct** _temp2309= _temp2307.curr +
yyvsp_offset; if( _temp2309 < _temp2307.base? 1: _temp2309 >= _temp2307.last_plus_one){
_throw( Null_Exception);}* _temp2309;})))};( struct _xenum_struct*) _temp2303;});
break; case 28: _LL2302: yyval=({ struct Cyc_DeclSpec_tok_struct* _temp2311=(
struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));*
_temp2311=( struct Cyc_DeclSpec_tok_struct){.tag= Cyc_DeclSpec_tok_tag,.f1=({
struct Cyc_Parse_Declaration_spec* _temp2312=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2312->sc=({ struct
Cyc_Core_Opt* _temp2316=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2316->v=( void*) Cyc_yyget_StorageClass_tok(({ struct _tagged_ptr2
_temp2317= yyvs; struct _xenum_struct** _temp2319= _temp2317.curr +(
yyvsp_offset - 1); if( _temp2319 < _temp2317.base? 1: _temp2319 >= _temp2317.last_plus_one){
_throw( Null_Exception);}* _temp2319;})); _temp2316;}); _temp2312->tq= Cyc_Absyn_empty_tqual();
_temp2312->type_specs= 0; _temp2312->is_inline= 0; _temp2312->attributes= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2313= yyvs; struct _xenum_struct** _temp2315= _temp2313.curr
+ yyvsp_offset; if( _temp2315 < _temp2313.base? 1: _temp2315 >= _temp2313.last_plus_one){
_throw( Null_Exception);}* _temp2315;})); _temp2312;})};( struct _xenum_struct*)
_temp2311;}); break; case 29: _LL2310: if(( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2321= yyvs; struct _xenum_struct** _temp2323= _temp2321.curr +
yyvsp_offset; if( _temp2323 < _temp2321.base? 1: _temp2323 >= _temp2321.last_plus_one){
_throw( Null_Exception);}* _temp2323;})))->sc != 0){ Cyc_Parse_warn(( struct
_tagged_string)({ char* _temp2324=( char*)"Only one storage class is allowed in a declaration";
struct _tagged_string _temp2325; _temp2325.curr= _temp2324; _temp2325.base=
_temp2324; _temp2325.last_plus_one= _temp2324 + 51; _temp2325;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2326= yyls; struct Cyc_Yyltype* _temp2328= _temp2326.curr
+( yylsp_offset - 2); if( _temp2328 < _temp2326.base? 1: _temp2328 >= _temp2326.last_plus_one){
_throw( Null_Exception);}* _temp2328;}).first_line,({ struct _tagged_ptr3
_temp2329= yyls; struct Cyc_Yyltype* _temp2331= _temp2329.curr +( yylsp_offset -
1); if( _temp2331 < _temp2329.base? 1: _temp2331 >= _temp2329.last_plus_one){
_throw( Null_Exception);}* _temp2331;}).last_line));} yyval=({ struct Cyc_DeclSpec_tok_struct*
_temp2332=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));*
_temp2332=( struct Cyc_DeclSpec_tok_struct){.tag= Cyc_DeclSpec_tok_tag,.f1=({
struct Cyc_Parse_Declaration_spec* _temp2333=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2333->sc=({ struct
Cyc_Core_Opt* _temp2349=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2349->v=( void*) Cyc_yyget_StorageClass_tok(({ struct _tagged_ptr2
_temp2350= yyvs; struct _xenum_struct** _temp2352= _temp2350.curr +(
yyvsp_offset - 2); if( _temp2352 < _temp2350.base? 1: _temp2352 >= _temp2350.last_plus_one){
_throw( Null_Exception);}* _temp2352;})); _temp2349;}); _temp2333->tq=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2346= yyvs; struct _xenum_struct** _temp2348= _temp2346.curr
+ yyvsp_offset; if( _temp2348 < _temp2346.base? 1: _temp2348 >= _temp2346.last_plus_one){
_throw( Null_Exception);}* _temp2348;})))->tq; _temp2333->type_specs=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2343= yyvs; struct _xenum_struct** _temp2345= _temp2343.curr
+ yyvsp_offset; if( _temp2345 < _temp2343.base? 1: _temp2345 >= _temp2343.last_plus_one){
_throw( Null_Exception);}* _temp2345;})))->type_specs; _temp2333->is_inline=(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2340= yyvs; struct
_xenum_struct** _temp2342= _temp2340.curr + yyvsp_offset; if( _temp2342 <
_temp2340.base? 1: _temp2342 >= _temp2340.last_plus_one){ _throw( Null_Exception);}*
_temp2342;})))->is_inline; _temp2333->attributes=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2334= yyvs; struct _xenum_struct** _temp2336= _temp2334.curr
+( yyvsp_offset - 1); if( _temp2336 < _temp2334.base? 1: _temp2336 >= _temp2334.last_plus_one){
_throw( Null_Exception);}* _temp2336;})),( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2337= yyvs; struct _xenum_struct** _temp2339= _temp2337.curr +
yyvsp_offset; if( _temp2339 < _temp2337.base? 1: _temp2339 >= _temp2337.last_plus_one){
_throw( Null_Exception);}* _temp2339;})))->attributes); _temp2333;})};( struct
_xenum_struct*) _temp2332;}); break; case 30: _LL2320: yyval=({ struct Cyc_DeclSpec_tok_struct*
_temp2354=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));*
_temp2354=( struct Cyc_DeclSpec_tok_struct){.tag= Cyc_DeclSpec_tok_tag,.f1=({
struct Cyc_Parse_Declaration_spec* _temp2355=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2355->sc= 0;
_temp2355->tq= Cyc_Absyn_empty_tqual(); _temp2355->type_specs=({ struct Cyc_List_List*
_temp2359=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2359->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({ struct _tagged_ptr2
_temp2360= yyvs; struct _xenum_struct** _temp2362= _temp2360.curr +(
yyvsp_offset - 1); if( _temp2362 < _temp2360.base? 1: _temp2362 >= _temp2360.last_plus_one){
_throw( Null_Exception);}* _temp2362;})); _temp2359->tl= 0; _temp2359;});
_temp2355->is_inline= 0; _temp2355->attributes= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2356= yyvs; struct _xenum_struct** _temp2358= _temp2356.curr
+ yyvsp_offset; if( _temp2358 < _temp2356.base? 1: _temp2358 >= _temp2356.last_plus_one){
_throw( Null_Exception);}* _temp2358;})); _temp2355;})};( struct _xenum_struct*)
_temp2354;}); break; case 31: _LL2353: yyval=({ struct Cyc_DeclSpec_tok_struct*
_temp2364=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));*
_temp2364=( struct Cyc_DeclSpec_tok_struct){.tag= Cyc_DeclSpec_tok_tag,.f1=({
struct Cyc_Parse_Declaration_spec* _temp2365=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2365->sc=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2385= yyvs; struct _xenum_struct** _temp2387= _temp2385.curr
+ yyvsp_offset; if( _temp2387 < _temp2385.base? 1: _temp2387 >= _temp2385.last_plus_one){
_throw( Null_Exception);}* _temp2387;})))->sc; _temp2365->tq=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2382= yyvs; struct _xenum_struct** _temp2384= _temp2382.curr
+ yyvsp_offset; if( _temp2384 < _temp2382.base? 1: _temp2384 >= _temp2382.last_plus_one){
_throw( Null_Exception);}* _temp2384;})))->tq; _temp2365->type_specs=({ struct
Cyc_List_List* _temp2375=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2375->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({ struct _tagged_ptr2
_temp2379= yyvs; struct _xenum_struct** _temp2381= _temp2379.curr +(
yyvsp_offset - 2); if( _temp2381 < _temp2379.base? 1: _temp2381 >= _temp2379.last_plus_one){
_throw( Null_Exception);}* _temp2381;})); _temp2375->tl=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2376= yyvs; struct _xenum_struct** _temp2378= _temp2376.curr
+ yyvsp_offset; if( _temp2378 < _temp2376.base? 1: _temp2378 >= _temp2376.last_plus_one){
_throw( Null_Exception);}* _temp2378;})))->type_specs; _temp2375;}); _temp2365->is_inline=(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2372= yyvs; struct
_xenum_struct** _temp2374= _temp2372.curr + yyvsp_offset; if( _temp2374 <
_temp2372.base? 1: _temp2374 >= _temp2372.last_plus_one){ _throw( Null_Exception);}*
_temp2374;})))->is_inline; _temp2365->attributes=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2366= yyvs; struct _xenum_struct** _temp2368= _temp2366.curr
+( yyvsp_offset - 1); if( _temp2368 < _temp2366.base? 1: _temp2368 >= _temp2366.last_plus_one){
_throw( Null_Exception);}* _temp2368;})),( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2369= yyvs; struct _xenum_struct** _temp2371= _temp2369.curr +
yyvsp_offset; if( _temp2371 < _temp2369.base? 1: _temp2371 >= _temp2369.last_plus_one){
_throw( Null_Exception);}* _temp2371;})))->attributes); _temp2365;})};( struct
_xenum_struct*) _temp2364;}); break; case 32: _LL2363: yyval=({ struct Cyc_DeclSpec_tok_struct*
_temp2389=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));*
_temp2389=( struct Cyc_DeclSpec_tok_struct){.tag= Cyc_DeclSpec_tok_tag,.f1=({
struct Cyc_Parse_Declaration_spec* _temp2390=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2390->sc= 0;
_temp2390->tq= Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2 _temp2394= yyvs;
struct _xenum_struct** _temp2396= _temp2394.curr +( yyvsp_offset - 1); if(
_temp2396 < _temp2394.base? 1: _temp2396 >= _temp2394.last_plus_one){ _throw(
Null_Exception);}* _temp2396;})); _temp2390->type_specs= 0; _temp2390->is_inline=
0; _temp2390->attributes= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2
_temp2391= yyvs; struct _xenum_struct** _temp2393= _temp2391.curr + yyvsp_offset;
if( _temp2393 < _temp2391.base? 1: _temp2393 >= _temp2391.last_plus_one){ _throw(
Null_Exception);}* _temp2393;})); _temp2390;})};( struct _xenum_struct*)
_temp2389;}); break; case 33: _LL2388: yyval=({ struct Cyc_DeclSpec_tok_struct*
_temp2398=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));*
_temp2398=( struct Cyc_DeclSpec_tok_struct){.tag= Cyc_DeclSpec_tok_tag,.f1=({
struct Cyc_Parse_Declaration_spec* _temp2399=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2399->sc=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2418= yyvs; struct _xenum_struct** _temp2420= _temp2418.curr
+ yyvsp_offset; if( _temp2420 < _temp2418.base? 1: _temp2420 >= _temp2418.last_plus_one){
_throw( Null_Exception);}* _temp2420;})))->sc; _temp2399->tq= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2 _temp2412= yyvs; struct
_xenum_struct** _temp2414= _temp2412.curr +( yyvsp_offset - 2); if( _temp2414 <
_temp2412.base? 1: _temp2414 >= _temp2412.last_plus_one){ _throw( Null_Exception);}*
_temp2414;})),( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2415= yyvs;
struct _xenum_struct** _temp2417= _temp2415.curr + yyvsp_offset; if( _temp2417 <
_temp2415.base? 1: _temp2417 >= _temp2415.last_plus_one){ _throw( Null_Exception);}*
_temp2417;})))->tq); _temp2399->type_specs=( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2409= yyvs; struct _xenum_struct** _temp2411= _temp2409.curr +
yyvsp_offset; if( _temp2411 < _temp2409.base? 1: _temp2411 >= _temp2409.last_plus_one){
_throw( Null_Exception);}* _temp2411;})))->type_specs; _temp2399->is_inline=(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2406= yyvs; struct
_xenum_struct** _temp2408= _temp2406.curr + yyvsp_offset; if( _temp2408 <
_temp2406.base? 1: _temp2408 >= _temp2406.last_plus_one){ _throw( Null_Exception);}*
_temp2408;})))->is_inline; _temp2399->attributes=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2400= yyvs; struct _xenum_struct** _temp2402= _temp2400.curr
+( yyvsp_offset - 1); if( _temp2402 < _temp2400.base? 1: _temp2402 >= _temp2400.last_plus_one){
_throw( Null_Exception);}* _temp2402;})),( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2403= yyvs; struct _xenum_struct** _temp2405= _temp2403.curr +
yyvsp_offset; if( _temp2405 < _temp2403.base? 1: _temp2405 >= _temp2403.last_plus_one){
_throw( Null_Exception);}* _temp2405;})))->attributes); _temp2399;})};( struct
_xenum_struct*) _temp2398;}); break; case 34: _LL2397: yyval=({ struct Cyc_DeclSpec_tok_struct*
_temp2422=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));*
_temp2422=( struct Cyc_DeclSpec_tok_struct){.tag= Cyc_DeclSpec_tok_tag,.f1=({
struct Cyc_Parse_Declaration_spec* _temp2423=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2423->sc= 0;
_temp2423->tq= Cyc_Absyn_empty_tqual(); _temp2423->type_specs= 0; _temp2423->is_inline=
1; _temp2423->attributes= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2
_temp2424= yyvs; struct _xenum_struct** _temp2426= _temp2424.curr + yyvsp_offset;
if( _temp2426 < _temp2424.base? 1: _temp2426 >= _temp2424.last_plus_one){ _throw(
Null_Exception);}* _temp2426;})); _temp2423;})};( struct _xenum_struct*)
_temp2422;}); break; case 35: _LL2421: yyval=({ struct Cyc_DeclSpec_tok_struct*
_temp2428=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));*
_temp2428=( struct Cyc_DeclSpec_tok_struct){.tag= Cyc_DeclSpec_tok_tag,.f1=({
struct Cyc_Parse_Declaration_spec* _temp2429=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2429->sc=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2442= yyvs; struct _xenum_struct** _temp2444= _temp2442.curr
+ yyvsp_offset; if( _temp2444 < _temp2442.base? 1: _temp2444 >= _temp2442.last_plus_one){
_throw( Null_Exception);}* _temp2444;})))->sc; _temp2429->tq=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2439= yyvs; struct _xenum_struct** _temp2441= _temp2439.curr
+ yyvsp_offset; if( _temp2441 < _temp2439.base? 1: _temp2441 >= _temp2439.last_plus_one){
_throw( Null_Exception);}* _temp2441;})))->tq; _temp2429->type_specs=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2436= yyvs; struct _xenum_struct** _temp2438= _temp2436.curr
+ yyvsp_offset; if( _temp2438 < _temp2436.base? 1: _temp2438 >= _temp2436.last_plus_one){
_throw( Null_Exception);}* _temp2438;})))->type_specs; _temp2429->is_inline= 1;
_temp2429->attributes=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2430= yyvs; struct _xenum_struct** _temp2432= _temp2430.curr
+( yyvsp_offset - 1); if( _temp2432 < _temp2430.base? 1: _temp2432 >= _temp2430.last_plus_one){
_throw( Null_Exception);}* _temp2432;})),( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2433= yyvs; struct _xenum_struct** _temp2435= _temp2433.curr +
yyvsp_offset; if( _temp2435 < _temp2433.base? 1: _temp2435 >= _temp2433.last_plus_one){
_throw( Null_Exception);}* _temp2435;})))->attributes); _temp2429;})};( struct
_xenum_struct*) _temp2428;}); break; case 36: _LL2427: yyval=({ struct Cyc_StorageClass_tok_struct*
_temp2446=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));*
_temp2446=( struct Cyc_StorageClass_tok_struct){.tag= Cyc_StorageClass_tok_tag,.f1=
Cyc_Parse_Auto_sc};( struct _xenum_struct*) _temp2446;}); break; case 37:
_LL2445: yyval=({ struct Cyc_StorageClass_tok_struct* _temp2448=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));* _temp2448=( struct Cyc_StorageClass_tok_struct){.tag=
Cyc_StorageClass_tok_tag,.f1= Cyc_Parse_Register_sc};( struct _xenum_struct*)
_temp2448;}); break; case 38: _LL2447: yyval=({ struct Cyc_StorageClass_tok_struct*
_temp2450=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));*
_temp2450=( struct Cyc_StorageClass_tok_struct){.tag= Cyc_StorageClass_tok_tag,.f1=
Cyc_Parse_Static_sc};( struct _xenum_struct*) _temp2450;}); break; case 39:
_LL2449: yyval=({ struct Cyc_StorageClass_tok_struct* _temp2452=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));* _temp2452=( struct Cyc_StorageClass_tok_struct){.tag=
Cyc_StorageClass_tok_tag,.f1= Cyc_Parse_Extern_sc};( struct _xenum_struct*)
_temp2452;}); break; case 40: _LL2451: if( Cyc_String_strcmp( Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp2454= yyvs; struct _xenum_struct** _temp2456= _temp2454.curr
+ yyvsp_offset; if( _temp2456 < _temp2454.base? 1: _temp2456 >= _temp2454.last_plus_one){
_throw( Null_Exception);}* _temp2456;})),( struct _tagged_string)({ char*
_temp2457=( char*)"C"; struct _tagged_string _temp2458; _temp2458.curr=
_temp2457; _temp2458.base= _temp2457; _temp2458.last_plus_one= _temp2457 + 2;
_temp2458;})) != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp2459=(
char*)"only extern or extern \"C\" is allowed"; struct _tagged_string _temp2460;
_temp2460.curr= _temp2459; _temp2460.base= _temp2459; _temp2460.last_plus_one=
_temp2459 + 37; _temp2460;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2461= yyls; struct Cyc_Yyltype* _temp2463= _temp2461.curr +( yylsp_offset -
1); if( _temp2463 < _temp2461.base? 1: _temp2463 >= _temp2461.last_plus_one){
_throw( Null_Exception);}* _temp2463;}).first_line,({ struct _tagged_ptr3
_temp2464= yyls; struct Cyc_Yyltype* _temp2466= _temp2464.curr + yylsp_offset;
if( _temp2466 < _temp2464.base? 1: _temp2466 >= _temp2464.last_plus_one){ _throw(
Null_Exception);}* _temp2466;}).last_line));} yyval=({ struct Cyc_StorageClass_tok_struct*
_temp2467=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));*
_temp2467=( struct Cyc_StorageClass_tok_struct){.tag= Cyc_StorageClass_tok_tag,.f1=
Cyc_Parse_ExternC_sc};( struct _xenum_struct*) _temp2467;}); break; case 41:
_LL2453: yyval=({ struct Cyc_StorageClass_tok_struct* _temp2469=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));* _temp2469=( struct Cyc_StorageClass_tok_struct){.tag=
Cyc_StorageClass_tok_tag,.f1= Cyc_Parse_Typedef_sc};( struct _xenum_struct*)
_temp2469;}); break; case 42: _LL2468: yyval=({ struct Cyc_StorageClass_tok_struct*
_temp2471=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));*
_temp2471=( struct Cyc_StorageClass_tok_struct){.tag= Cyc_StorageClass_tok_tag,.f1=
Cyc_Parse_Abstract_sc};( struct _xenum_struct*) _temp2471;}); break; case 43:
_LL2470: yyval=({ struct Cyc_AttributeList_tok_struct* _temp2473=( struct Cyc_AttributeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));* _temp2473=( struct
Cyc_AttributeList_tok_struct){.tag= Cyc_AttributeList_tok_tag,.f1= 0};( struct
_xenum_struct*) _temp2473;}); break; case 44: _LL2472: yyval=({ struct
_tagged_ptr2 _temp2475= yyvs; struct _xenum_struct** _temp2477= _temp2475.curr +
yyvsp_offset; if( _temp2477 < _temp2475.base? 1: _temp2477 >= _temp2475.last_plus_one){
_throw( Null_Exception);}* _temp2477;}); break; case 45: _LL2474: yyval=({
struct Cyc_AttributeList_tok_struct* _temp2479=( struct Cyc_AttributeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));* _temp2479=( struct
Cyc_AttributeList_tok_struct){.tag= Cyc_AttributeList_tok_tag,.f1= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2480= yyvs; struct _xenum_struct** _temp2482= _temp2480.curr
+( yyvsp_offset - 2); if( _temp2482 < _temp2480.base? 1: _temp2482 >= _temp2480.last_plus_one){
_throw( Null_Exception);}* _temp2482;}))};( struct _xenum_struct*) _temp2479;});
break; case 46: _LL2478: yyval=({ struct Cyc_AttributeList_tok_struct* _temp2484=(
struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));*
_temp2484=( struct Cyc_AttributeList_tok_struct){.tag= Cyc_AttributeList_tok_tag,.f1=({
struct Cyc_List_List* _temp2485=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2485->hd=( void*) Cyc_yyget_Attribute_tok(({ struct
_tagged_ptr2 _temp2486= yyvs; struct _xenum_struct** _temp2488= _temp2486.curr +
yyvsp_offset; if( _temp2488 < _temp2486.base? 1: _temp2488 >= _temp2486.last_plus_one){
_throw( Null_Exception);}* _temp2488;})); _temp2485->tl= 0; _temp2485;})};(
struct _xenum_struct*) _temp2484;}); break; case 47: _LL2483: yyval=({ struct
Cyc_AttributeList_tok_struct* _temp2490=( struct Cyc_AttributeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));* _temp2490=( struct
Cyc_AttributeList_tok_struct){.tag= Cyc_AttributeList_tok_tag,.f1=({ struct Cyc_List_List*
_temp2491=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2491->hd=( void*) Cyc_yyget_Attribute_tok(({ struct _tagged_ptr2 _temp2495=
yyvs; struct _xenum_struct** _temp2497= _temp2495.curr +( yyvsp_offset - 2); if(
_temp2497 < _temp2495.base? 1: _temp2497 >= _temp2495.last_plus_one){ _throw(
Null_Exception);}* _temp2497;})); _temp2491->tl= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2492= yyvs; struct _xenum_struct** _temp2494= _temp2492.curr
+ yyvsp_offset; if( _temp2494 < _temp2492.base? 1: _temp2494 >= _temp2492.last_plus_one){
_throw( Null_Exception);}* _temp2494;})); _temp2491;})};( struct _xenum_struct*)
_temp2490;}); break; case 48: _LL2489: { struct _tagged_string s= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp2499= yyvs; struct _xenum_struct** _temp2501= _temp2499.curr
+ yyvsp_offset; if( _temp2501 < _temp2499.base? 1: _temp2501 >= _temp2499.last_plus_one){
_throw( Null_Exception);}* _temp2501;})); void* a; if( Cyc_String_zstrcmp( s,(
struct _tagged_string)({ char* _temp2502=( char*)"stdcall"; struct
_tagged_string _temp2503; _temp2503.curr= _temp2502; _temp2503.base= _temp2502;
_temp2503.last_plus_one= _temp2502 + 8; _temp2503;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2504=( char*)"__stdcall__"; struct
_tagged_string _temp2505; _temp2505.curr= _temp2504; _temp2505.base= _temp2504;
_temp2505.last_plus_one= _temp2504 + 12; _temp2505;})) == 0){ a= Cyc_Absyn_Stdcall_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2506=(
char*)"cdecl"; struct _tagged_string _temp2507; _temp2507.curr= _temp2506;
_temp2507.base= _temp2506; _temp2507.last_plus_one= _temp2506 + 6; _temp2507;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2508=( char*)"__cdecl__";
struct _tagged_string _temp2509; _temp2509.curr= _temp2508; _temp2509.base=
_temp2508; _temp2509.last_plus_one= _temp2508 + 10; _temp2509;})) == 0){ a= Cyc_Absyn_Cdecl_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2510=(
char*)"noreturn"; struct _tagged_string _temp2511; _temp2511.curr= _temp2510;
_temp2511.base= _temp2510; _temp2511.last_plus_one= _temp2510 + 9; _temp2511;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2512=( char*)"__noreturn__";
struct _tagged_string _temp2513; _temp2513.curr= _temp2512; _temp2513.base=
_temp2512; _temp2513.last_plus_one= _temp2512 + 13; _temp2513;})) == 0){ a= Cyc_Absyn_Noreturn_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2514=(
char*)"noreturn"; struct _tagged_string _temp2515; _temp2515.curr= _temp2514;
_temp2515.base= _temp2514; _temp2515.last_plus_one= _temp2514 + 9; _temp2515;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2516=( char*)"__noreturn__";
struct _tagged_string _temp2517; _temp2517.curr= _temp2516; _temp2517.base=
_temp2516; _temp2517.last_plus_one= _temp2516 + 13; _temp2517;})) == 0){ a= Cyc_Absyn_Noreturn_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2518=(
char*)"__const__"; struct _tagged_string _temp2519; _temp2519.curr= _temp2518;
_temp2519.base= _temp2518; _temp2519.last_plus_one= _temp2518 + 10; _temp2519;}))
== 0){ a= Cyc_Absyn_Const_att;} else{ if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2520=( char*)"aligned"; struct _tagged_string
_temp2521; _temp2521.curr= _temp2520; _temp2521.base= _temp2520; _temp2521.last_plus_one=
_temp2520 + 8; _temp2521;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2522=( char*)"__aligned__"; struct _tagged_string
_temp2523; _temp2523.curr= _temp2522; _temp2523.base= _temp2522; _temp2523.last_plus_one=
_temp2522 + 12; _temp2523;})) == 0){ a=({ struct Cyc_Absyn_Aligned_att_struct*
_temp2524=( struct Cyc_Absyn_Aligned_att_struct*) GC_malloc_atomic( sizeof(
struct Cyc_Absyn_Aligned_att_struct)); _temp2524->tag= Cyc_Absyn_Aligned_att_tag;
_temp2524->f1= - 1;( void*) _temp2524;});} else{ if( Cyc_String_zstrcmp( s,(
struct _tagged_string)({ char* _temp2525=( char*)"packed"; struct _tagged_string
_temp2526; _temp2526.curr= _temp2525; _temp2526.base= _temp2525; _temp2526.last_plus_one=
_temp2525 + 7; _temp2526;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2527=( char*)"__packed__"; struct _tagged_string
_temp2528; _temp2528.curr= _temp2527; _temp2528.base= _temp2527; _temp2528.last_plus_one=
_temp2527 + 11; _temp2528;})) == 0){ a= Cyc_Absyn_Packed_att;} else{ if( Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2529=( char*)"shared"; struct
_tagged_string _temp2530; _temp2530.curr= _temp2529; _temp2530.base= _temp2529;
_temp2530.last_plus_one= _temp2529 + 7; _temp2530;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2531=( char*)"__shared__"; struct
_tagged_string _temp2532; _temp2532.curr= _temp2531; _temp2532.base= _temp2531;
_temp2532.last_plus_one= _temp2531 + 11; _temp2532;})) == 0){ a= Cyc_Absyn_Shared_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2533=(
char*)"unused"; struct _tagged_string _temp2534; _temp2534.curr= _temp2533;
_temp2534.base= _temp2533; _temp2534.last_plus_one= _temp2533 + 7; _temp2534;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2535=( char*)"__unused__";
struct _tagged_string _temp2536; _temp2536.curr= _temp2535; _temp2536.base=
_temp2535; _temp2536.last_plus_one= _temp2535 + 11; _temp2536;})) == 0){ a= Cyc_Absyn_Unused_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2537=(
char*)"weak"; struct _tagged_string _temp2538; _temp2538.curr= _temp2537;
_temp2538.base= _temp2537; _temp2538.last_plus_one= _temp2537 + 5; _temp2538;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2539=( char*)"__weak__";
struct _tagged_string _temp2540; _temp2540.curr= _temp2539; _temp2540.base=
_temp2539; _temp2540.last_plus_one= _temp2539 + 9; _temp2540;})) == 0){ a= Cyc_Absyn_Weak_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2541=(
char*)"dllimport"; struct _tagged_string _temp2542; _temp2542.curr= _temp2541;
_temp2542.base= _temp2541; _temp2542.last_plus_one= _temp2541 + 10; _temp2542;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2543=( char*)"__dllimport__";
struct _tagged_string _temp2544; _temp2544.curr= _temp2543; _temp2544.base=
_temp2543; _temp2544.last_plus_one= _temp2543 + 14; _temp2544;})) == 0){ a= Cyc_Absyn_Dllimport_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2545=(
char*)"dllexport"; struct _tagged_string _temp2546; _temp2546.curr= _temp2545;
_temp2546.base= _temp2545; _temp2546.last_plus_one= _temp2545 + 10; _temp2546;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2547=( char*)"__dllexport__";
struct _tagged_string _temp2548; _temp2548.curr= _temp2547; _temp2548.base=
_temp2547; _temp2548.last_plus_one= _temp2547 + 14; _temp2548;})) == 0){ a= Cyc_Absyn_Dllexport_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2549=(
char*)"no_instrument_function"; struct _tagged_string _temp2550; _temp2550.curr=
_temp2549; _temp2550.base= _temp2549; _temp2550.last_plus_one= _temp2549 + 23;
_temp2550;})) == 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char*
_temp2551=( char*)"__no_instrument_function__"; struct _tagged_string _temp2552;
_temp2552.curr= _temp2551; _temp2552.base= _temp2551; _temp2552.last_plus_one=
_temp2551 + 27; _temp2552;})) == 0){ a= Cyc_Absyn_No_instrument_function_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2553=(
char*)"constructor"; struct _tagged_string _temp2554; _temp2554.curr= _temp2553;
_temp2554.base= _temp2553; _temp2554.last_plus_one= _temp2553 + 12; _temp2554;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2555=( char*)"__constructor__";
struct _tagged_string _temp2556; _temp2556.curr= _temp2555; _temp2556.base=
_temp2555; _temp2556.last_plus_one= _temp2555 + 16; _temp2556;})) == 0){ a= Cyc_Absyn_Constructor_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2557=(
char*)"destructor"; struct _tagged_string _temp2558; _temp2558.curr= _temp2557;
_temp2558.base= _temp2557; _temp2558.last_plus_one= _temp2557 + 11; _temp2558;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2559=( char*)"__destructor__";
struct _tagged_string _temp2560; _temp2560.curr= _temp2559; _temp2560.base=
_temp2559; _temp2560.last_plus_one= _temp2559 + 15; _temp2560;})) == 0){ a= Cyc_Absyn_Destructor_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2561=(
char*)"no_check_memory_usage"; struct _tagged_string _temp2562; _temp2562.curr=
_temp2561; _temp2562.base= _temp2561; _temp2562.last_plus_one= _temp2561 + 22;
_temp2562;})) == 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char*
_temp2563=( char*)"__no_check_memory_usage__"; struct _tagged_string _temp2564;
_temp2564.curr= _temp2563; _temp2564.base= _temp2563; _temp2564.last_plus_one=
_temp2563 + 26; _temp2564;})) == 0){ a= Cyc_Absyn_No_check_memory_usage_att;}
else{ Cyc_Parse_err(( struct _tagged_string)({ char* _temp2565=( char*)"unrecognized attribute";
struct _tagged_string _temp2566; _temp2566.curr= _temp2565; _temp2566.base=
_temp2565; _temp2566.last_plus_one= _temp2565 + 23; _temp2566;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2567= yyls; struct Cyc_Yyltype* _temp2569= _temp2567.curr
+ yylsp_offset; if( _temp2569 < _temp2567.base? 1: _temp2569 >= _temp2567.last_plus_one){
_throw( Null_Exception);}* _temp2569;}).first_line,({ struct _tagged_ptr3
_temp2570= yyls; struct Cyc_Yyltype* _temp2572= _temp2570.curr + yylsp_offset;
if( _temp2572 < _temp2570.base? 1: _temp2572 >= _temp2570.last_plus_one){ _throw(
Null_Exception);}* _temp2572;}).last_line)); a= Cyc_Absyn_Cdecl_att;}}}}}}}}}}}}}}}}
yyval=({ struct Cyc_Attribute_tok_struct* _temp2573=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));* _temp2573=( struct Cyc_Attribute_tok_struct){.tag=
Cyc_Attribute_tok_tag,.f1= a};( struct _xenum_struct*) _temp2573;}); break;}
case 49: _LL2498: yyval=({ struct Cyc_Attribute_tok_struct* _temp2575=( struct
Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));*
_temp2575=( struct Cyc_Attribute_tok_struct){.tag= Cyc_Attribute_tok_tag,.f1=
Cyc_Absyn_Const_att};( struct _xenum_struct*) _temp2575;}); break; case 50:
_LL2574: { struct _tagged_string s= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp2577= yyvs; struct _xenum_struct** _temp2579= _temp2577.curr +(
yyvsp_offset - 3); if( _temp2579 < _temp2577.base? 1: _temp2579 >= _temp2577.last_plus_one){
_throw( Null_Exception);}* _temp2579;})); struct _tuple11 _temp2585; int
_temp2586; void* _temp2588; struct _tuple11* _temp2583= Cyc_yyget_Int_tok(({
struct _tagged_ptr2 _temp2580= yyvs; struct _xenum_struct** _temp2582= _temp2580.curr
+( yyvsp_offset - 1); if( _temp2582 < _temp2580.base? 1: _temp2582 >= _temp2580.last_plus_one){
_throw( Null_Exception);}* _temp2582;})); _temp2585=* _temp2583; _LL2589:
_temp2588= _temp2585.f1; goto _LL2587; _LL2587: _temp2586= _temp2585.f2; goto
_LL2584; _LL2584: { void* a; if( Cyc_String_zstrcmp( s,( struct _tagged_string)({
char* _temp2590=( char*)"regparm"; struct _tagged_string _temp2591; _temp2591.curr=
_temp2590; _temp2591.base= _temp2590; _temp2591.last_plus_one= _temp2590 + 8;
_temp2591;})) == 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char*
_temp2592=( char*)"__regparm__"; struct _tagged_string _temp2593; _temp2593.curr=
_temp2592; _temp2593.base= _temp2592; _temp2593.last_plus_one= _temp2592 + 12;
_temp2593;})) == 0){ if( _temp2586 <= 0? 1: _temp2586 > 3){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp2594=( char*)"regparm requires value between 1 and 3";
struct _tagged_string _temp2595; _temp2595.curr= _temp2594; _temp2595.base=
_temp2594; _temp2595.last_plus_one= _temp2594 + 39; _temp2595;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2596= yyls; struct Cyc_Yyltype* _temp2598= _temp2596.curr
+( yylsp_offset - 1); if( _temp2598 < _temp2596.base? 1: _temp2598 >= _temp2596.last_plus_one){
_throw( Null_Exception);}* _temp2598;}).first_line,({ struct _tagged_ptr3
_temp2599= yyls; struct Cyc_Yyltype* _temp2601= _temp2599.curr +( yylsp_offset -
1); if( _temp2601 < _temp2599.base? 1: _temp2601 >= _temp2599.last_plus_one){
_throw( Null_Exception);}* _temp2601;}).last_line));} a=({ struct Cyc_Absyn_Regparm_att_struct*
_temp2602=( struct Cyc_Absyn_Regparm_att_struct*) GC_malloc_atomic( sizeof(
struct Cyc_Absyn_Regparm_att_struct)); _temp2602->tag= Cyc_Absyn_Regparm_att_tag;
_temp2602->f1= _temp2586;( void*) _temp2602;});} else{ if( Cyc_String_zstrcmp( s,(
struct _tagged_string)({ char* _temp2603=( char*)"aligned"; struct
_tagged_string _temp2604; _temp2604.curr= _temp2603; _temp2604.base= _temp2603;
_temp2604.last_plus_one= _temp2603 + 8; _temp2604;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2605=( char*)"__aligned__"; struct
_tagged_string _temp2606; _temp2606.curr= _temp2605; _temp2606.base= _temp2605;
_temp2606.last_plus_one= _temp2605 + 12; _temp2606;})) == 0){ if( _temp2586 < 0){
Cyc_Parse_err(( struct _tagged_string)({ char* _temp2607=( char*)"aligned requires positive power of two";
struct _tagged_string _temp2608; _temp2608.curr= _temp2607; _temp2608.base=
_temp2607; _temp2608.last_plus_one= _temp2607 + 39; _temp2608;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2609= yyls; struct Cyc_Yyltype* _temp2611= _temp2609.curr
+( yylsp_offset - 1); if( _temp2611 < _temp2609.base? 1: _temp2611 >= _temp2609.last_plus_one){
_throw( Null_Exception);}* _temp2611;}).first_line,({ struct _tagged_ptr3
_temp2612= yyls; struct Cyc_Yyltype* _temp2614= _temp2612.curr +( yylsp_offset -
1); if( _temp2614 < _temp2612.base? 1: _temp2614 >= _temp2612.last_plus_one){
_throw( Null_Exception);}* _temp2614;}).last_line));}{ unsigned int j=(
unsigned int) _temp2586; for( 0;( j &( unsigned int) 1) ==( unsigned int) 0; j=
j >>( unsigned int) 1){;} j= j >>( unsigned int) 1; if( j !=( unsigned int) 0){
Cyc_Parse_err(( struct _tagged_string)({ char* _temp2615=( char*)"aligned requires positive power of two";
struct _tagged_string _temp2616; _temp2616.curr= _temp2615; _temp2616.base=
_temp2615; _temp2616.last_plus_one= _temp2615 + 39; _temp2616;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2617= yyls; struct Cyc_Yyltype* _temp2619= _temp2617.curr
+( yylsp_offset - 1); if( _temp2619 < _temp2617.base? 1: _temp2619 >= _temp2617.last_plus_one){
_throw( Null_Exception);}* _temp2619;}).first_line,({ struct _tagged_ptr3
_temp2620= yyls; struct Cyc_Yyltype* _temp2622= _temp2620.curr +( yylsp_offset -
1); if( _temp2622 < _temp2620.base? 1: _temp2622 >= _temp2620.last_plus_one){
_throw( Null_Exception);}* _temp2622;}).last_line));} a=({ struct Cyc_Absyn_Aligned_att_struct*
_temp2623=( struct Cyc_Absyn_Aligned_att_struct*) GC_malloc_atomic( sizeof(
struct Cyc_Absyn_Aligned_att_struct)); _temp2623->tag= Cyc_Absyn_Aligned_att_tag;
_temp2623->f1= _temp2586;( void*) _temp2623;});}} else{ Cyc_Parse_err(( struct
_tagged_string)({ char* _temp2624=( char*)"unrecognized attribute"; struct
_tagged_string _temp2625; _temp2625.curr= _temp2624; _temp2625.base= _temp2624;
_temp2625.last_plus_one= _temp2624 + 23; _temp2625;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2626= yyls; struct Cyc_Yyltype* _temp2628= _temp2626.curr
+( yylsp_offset - 3); if( _temp2628 < _temp2626.base? 1: _temp2628 >= _temp2626.last_plus_one){
_throw( Null_Exception);}* _temp2628;}).first_line,({ struct _tagged_ptr3
_temp2629= yyls; struct Cyc_Yyltype* _temp2631= _temp2629.curr +( yylsp_offset -
3); if( _temp2631 < _temp2629.base? 1: _temp2631 >= _temp2629.last_plus_one){
_throw( Null_Exception);}* _temp2631;}).last_line)); a= Cyc_Absyn_Cdecl_att;}}
yyval=({ struct Cyc_Attribute_tok_struct* _temp2632=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));* _temp2632=( struct Cyc_Attribute_tok_struct){.tag=
Cyc_Attribute_tok_tag,.f1= a};( struct _xenum_struct*) _temp2632;}); break;}}
case 51: _LL2576: { struct _tagged_string s= Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp2634= yyvs; struct _xenum_struct** _temp2636= _temp2634.curr +(
yyvsp_offset - 3); if( _temp2636 < _temp2634.base? 1: _temp2636 >= _temp2634.last_plus_one){
_throw( Null_Exception);}* _temp2636;})); struct _tagged_string str= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp2637= yyvs; struct _xenum_struct** _temp2639= _temp2637.curr
+( yyvsp_offset - 1); if( _temp2639 < _temp2637.base? 1: _temp2639 >= _temp2637.last_plus_one){
_throw( Null_Exception);}* _temp2639;})); void* a; if( Cyc_String_zstrcmp( s,(
struct _tagged_string)({ char* _temp2640=( char*)"section"; struct
_tagged_string _temp2641; _temp2641.curr= _temp2640; _temp2641.base= _temp2640;
_temp2641.last_plus_one= _temp2640 + 8; _temp2641;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2642=( char*)"__section__"; struct
_tagged_string _temp2643; _temp2643.curr= _temp2642; _temp2643.base= _temp2642;
_temp2643.last_plus_one= _temp2642 + 12; _temp2643;}))){ a=({ struct Cyc_Absyn_Section_att_struct*
_temp2644=( struct Cyc_Absyn_Section_att_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Section_att_struct));
_temp2644->tag= Cyc_Absyn_Section_att_tag; _temp2644->f1= str;( void*) _temp2644;});}
else{ Cyc_Parse_err(( struct _tagged_string)({ char* _temp2645=( char*)"unrecognized attribute";
struct _tagged_string _temp2646; _temp2646.curr= _temp2645; _temp2646.base=
_temp2645; _temp2646.last_plus_one= _temp2645 + 23; _temp2646;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2647= yyls; struct Cyc_Yyltype* _temp2649= _temp2647.curr
+( yylsp_offset - 3); if( _temp2649 < _temp2647.base? 1: _temp2649 >= _temp2647.last_plus_one){
_throw( Null_Exception);}* _temp2649;}).first_line,({ struct _tagged_ptr3
_temp2650= yyls; struct Cyc_Yyltype* _temp2652= _temp2650.curr +( yylsp_offset -
3); if( _temp2652 < _temp2650.base? 1: _temp2652 >= _temp2650.last_plus_one){
_throw( Null_Exception);}* _temp2652;}).last_line)); a= Cyc_Absyn_Cdecl_att;}
yyval=({ struct Cyc_Attribute_tok_struct* _temp2653=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));* _temp2653=( struct Cyc_Attribute_tok_struct){.tag=
Cyc_Attribute_tok_tag,.f1= a};( struct _xenum_struct*) _temp2653;}); break;}
case 52: _LL2633: yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2655=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2655=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec( Cyc_Absyn_VoidType, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2656= yyls; struct Cyc_Yyltype* _temp2658= _temp2656.curr +
yylsp_offset; if( _temp2658 < _temp2656.base? 1: _temp2658 >= _temp2656.last_plus_one){
_throw( Null_Exception);}* _temp2658;}).first_line,({ struct _tagged_ptr3
_temp2659= yyls; struct Cyc_Yyltype* _temp2661= _temp2659.curr + yylsp_offset;
if( _temp2661 < _temp2659.base? 1: _temp2661 >= _temp2659.last_plus_one){ _throw(
Null_Exception);}* _temp2661;}).last_line))};( struct _xenum_struct*) _temp2655;});
break; case 53: _LL2654: yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2663=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2663=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec( Cyc_Absyn_new_evar( Cyc_Absyn_MemKind), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2664= yyls; struct Cyc_Yyltype* _temp2666= _temp2664.curr
+ yylsp_offset; if( _temp2666 < _temp2664.base? 1: _temp2666 >= _temp2664.last_plus_one){
_throw( Null_Exception);}* _temp2666;}).first_line,({ struct _tagged_ptr3
_temp2667= yyls; struct Cyc_Yyltype* _temp2669= _temp2667.curr + yylsp_offset;
if( _temp2669 < _temp2667.base? 1: _temp2669 >= _temp2667.last_plus_one){ _throw(
Null_Exception);}* _temp2669;}).last_line))};( struct _xenum_struct*) _temp2663;});
break; case 54: _LL2662: yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2671=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2671=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec( Cyc_Absyn_uchar_t, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2672= yyls; struct Cyc_Yyltype* _temp2674= _temp2672.curr +
yylsp_offset; if( _temp2674 < _temp2672.base? 1: _temp2674 >= _temp2672.last_plus_one){
_throw( Null_Exception);}* _temp2674;}).first_line,({ struct _tagged_ptr3
_temp2675= yyls; struct Cyc_Yyltype* _temp2677= _temp2675.curr + yylsp_offset;
if( _temp2677 < _temp2675.base? 1: _temp2677 >= _temp2675.last_plus_one){ _throw(
Null_Exception);}* _temp2677;}).last_line))};( struct _xenum_struct*) _temp2671;});
break; case 55: _LL2670: yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2679=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2679=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=({
struct Cyc_Parse_Short_spec_struct* _temp2680=( struct Cyc_Parse_Short_spec_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Short_spec_struct)); _temp2680->tag= Cyc_Parse_Short_spec_tag;
_temp2680->f1= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2681=
yyls; struct Cyc_Yyltype* _temp2683= _temp2681.curr + yylsp_offset; if(
_temp2683 < _temp2681.base? 1: _temp2683 >= _temp2681.last_plus_one){ _throw(
Null_Exception);}* _temp2683;}).first_line,({ struct _tagged_ptr3 _temp2684=
yyls; struct Cyc_Yyltype* _temp2686= _temp2684.curr + yylsp_offset; if(
_temp2686 < _temp2684.base? 1: _temp2686 >= _temp2684.last_plus_one){ _throw(
Null_Exception);}* _temp2686;}).last_line);( void*) _temp2680;})};( struct
_xenum_struct*) _temp2679;}); break; case 56: _LL2678: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp2688=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2688=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec( Cyc_Absyn_sint_t, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2689= yyls; struct Cyc_Yyltype* _temp2691= _temp2689.curr +
yylsp_offset; if( _temp2691 < _temp2689.base? 1: _temp2691 >= _temp2689.last_plus_one){
_throw( Null_Exception);}* _temp2691;}).first_line,({ struct _tagged_ptr3
_temp2692= yyls; struct Cyc_Yyltype* _temp2694= _temp2692.curr + yylsp_offset;
if( _temp2694 < _temp2692.base? 1: _temp2694 >= _temp2692.last_plus_one){ _throw(
Null_Exception);}* _temp2694;}).last_line))};( struct _xenum_struct*) _temp2688;});
break; case 57: _LL2687: yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2696=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2696=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=({
struct Cyc_Parse_Long_spec_struct* _temp2697=( struct Cyc_Parse_Long_spec_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Long_spec_struct)); _temp2697->tag= Cyc_Parse_Long_spec_tag;
_temp2697->f1= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2698=
yyls; struct Cyc_Yyltype* _temp2700= _temp2698.curr + yylsp_offset; if(
_temp2700 < _temp2698.base? 1: _temp2700 >= _temp2698.last_plus_one){ _throw(
Null_Exception);}* _temp2700;}).first_line,({ struct _tagged_ptr3 _temp2701=
yyls; struct Cyc_Yyltype* _temp2703= _temp2701.curr + yylsp_offset; if(
_temp2703 < _temp2701.base? 1: _temp2703 >= _temp2701.last_plus_one){ _throw(
Null_Exception);}* _temp2703;}).last_line);( void*) _temp2697;})};( struct
_xenum_struct*) _temp2696;}); break; case 58: _LL2695: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp2705=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2705=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec( Cyc_Absyn_float_t, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2706= yyls; struct Cyc_Yyltype* _temp2708= _temp2706.curr +
yylsp_offset; if( _temp2708 < _temp2706.base? 1: _temp2708 >= _temp2706.last_plus_one){
_throw( Null_Exception);}* _temp2708;}).first_line,({ struct _tagged_ptr3
_temp2709= yyls; struct Cyc_Yyltype* _temp2711= _temp2709.curr + yylsp_offset;
if( _temp2711 < _temp2709.base? 1: _temp2711 >= _temp2709.last_plus_one){ _throw(
Null_Exception);}* _temp2711;}).last_line))};( struct _xenum_struct*) _temp2705;});
break; case 59: _LL2704: yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2713=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2713=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec( Cyc_Absyn_double_t, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2714= yyls; struct Cyc_Yyltype* _temp2716= _temp2714.curr +
yylsp_offset; if( _temp2716 < _temp2714.base? 1: _temp2716 >= _temp2714.last_plus_one){
_throw( Null_Exception);}* _temp2716;}).first_line,({ struct _tagged_ptr3
_temp2717= yyls; struct Cyc_Yyltype* _temp2719= _temp2717.curr + yylsp_offset;
if( _temp2719 < _temp2717.base? 1: _temp2719 >= _temp2717.last_plus_one){ _throw(
Null_Exception);}* _temp2719;}).last_line))};( struct _xenum_struct*) _temp2713;});
break; case 60: _LL2712: yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2721=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2721=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=({
struct Cyc_Parse_Signed_spec_struct* _temp2722=( struct Cyc_Parse_Signed_spec_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Signed_spec_struct)); _temp2722->tag= Cyc_Parse_Signed_spec_tag;
_temp2722->f1= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2723=
yyls; struct Cyc_Yyltype* _temp2725= _temp2723.curr + yylsp_offset; if(
_temp2725 < _temp2723.base? 1: _temp2725 >= _temp2723.last_plus_one){ _throw(
Null_Exception);}* _temp2725;}).first_line,({ struct _tagged_ptr3 _temp2726=
yyls; struct Cyc_Yyltype* _temp2728= _temp2726.curr + yylsp_offset; if(
_temp2728 < _temp2726.base? 1: _temp2728 >= _temp2726.last_plus_one){ _throw(
Null_Exception);}* _temp2728;}).last_line);( void*) _temp2722;})};( struct
_xenum_struct*) _temp2721;}); break; case 61: _LL2720: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp2730=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2730=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=({
struct Cyc_Parse_Unsigned_spec_struct* _temp2731=( struct Cyc_Parse_Unsigned_spec_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Unsigned_spec_struct)); _temp2731->tag= Cyc_Parse_Unsigned_spec_tag;
_temp2731->f1= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2732=
yyls; struct Cyc_Yyltype* _temp2734= _temp2732.curr + yylsp_offset; if(
_temp2734 < _temp2732.base? 1: _temp2734 >= _temp2732.last_plus_one){ _throw(
Null_Exception);}* _temp2734;}).first_line,({ struct _tagged_ptr3 _temp2735=
yyls; struct Cyc_Yyltype* _temp2737= _temp2735.curr + yylsp_offset; if(
_temp2737 < _temp2735.base? 1: _temp2737 >= _temp2735.last_plus_one){ _throw(
Null_Exception);}* _temp2737;}).last_line);( void*) _temp2731;})};( struct
_xenum_struct*) _temp2730;}); break; case 62: _LL2729: yyval=({ struct
_tagged_ptr2 _temp2739= yyvs; struct _xenum_struct** _temp2741= _temp2739.curr +
yyvsp_offset; if( _temp2741 < _temp2739.base? 1: _temp2741 >= _temp2739.last_plus_one){
_throw( Null_Exception);}* _temp2741;}); break; case 63: _LL2738: yyval=({
struct _tagged_ptr2 _temp2743= yyvs; struct _xenum_struct** _temp2745= _temp2743.curr
+ yyvsp_offset; if( _temp2745 < _temp2743.base? 1: _temp2745 >= _temp2743.last_plus_one){
_throw( Null_Exception);}* _temp2745;}); break; case 64: _LL2742: yyval=({
struct Cyc_TypeSpecifier_tok_struct* _temp2747=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));* _temp2747=( struct
Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1= Cyc_Parse_type_spec(
Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2748= yyvs;
struct _xenum_struct** _temp2750= _temp2748.curr + yyvsp_offset; if( _temp2750 <
_temp2748.base? 1: _temp2750 >= _temp2748.last_plus_one){ _throw( Null_Exception);}*
_temp2750;})),(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)()), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2751= yyls; struct Cyc_Yyltype* _temp2753= _temp2751.curr
+ yylsp_offset; if( _temp2753 < _temp2751.base? 1: _temp2753 >= _temp2751.last_plus_one){
_throw( Null_Exception);}* _temp2753;}).first_line,({ struct _tagged_ptr3
_temp2754= yyls; struct Cyc_Yyltype* _temp2756= _temp2754.curr + yylsp_offset;
if( _temp2756 < _temp2754.base? 1: _temp2756 >= _temp2754.last_plus_one){ _throw(
Null_Exception);}* _temp2756;}).last_line))};( struct _xenum_struct*) _temp2747;});
break; case 65: _LL2746: yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2758=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2758=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec( Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp2759= yyvs; struct _xenum_struct** _temp2761= _temp2759.curr +(
yyvsp_offset - 2); if( _temp2761 < _temp2759.base? 1: _temp2761 >= _temp2759.last_plus_one){
_throw( Null_Exception);}* _temp2761;})),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)( Cyc_yyget_Kind_tok(({ struct _tagged_ptr2 _temp2762= yyvs;
struct _xenum_struct** _temp2764= _temp2762.curr + yyvsp_offset; if( _temp2764 <
_temp2762.base? 1: _temp2764 >= _temp2762.last_plus_one){ _throw( Null_Exception);}*
_temp2764;})))), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2765=
yyls; struct Cyc_Yyltype* _temp2767= _temp2765.curr +( yylsp_offset - 2); if(
_temp2767 < _temp2765.base? 1: _temp2767 >= _temp2765.last_plus_one){ _throw(
Null_Exception);}* _temp2767;}).first_line,({ struct _tagged_ptr3 _temp2768=
yyls; struct Cyc_Yyltype* _temp2770= _temp2768.curr + yylsp_offset; if(
_temp2770 < _temp2768.base? 1: _temp2770 >= _temp2768.last_plus_one){ _throw(
Null_Exception);}* _temp2770;}).last_line))};( struct _xenum_struct*) _temp2758;});
break; case 66: _LL2757: yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2772=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2772=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec(({ struct Cyc_Absyn_TypedefType_struct* _temp2773=( struct
Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp2773->tag= Cyc_Absyn_TypedefType_tag; _temp2773->f1= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp2774= yyvs; struct _xenum_struct** _temp2776= _temp2774.curr
+( yyvsp_offset - 1); if( _temp2776 < _temp2774.base? 1: _temp2776 >= _temp2774.last_plus_one){
_throw( Null_Exception);}* _temp2776;})); _temp2773->f2= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp2777= yyvs; struct _xenum_struct** _temp2779= _temp2777.curr
+ yyvsp_offset; if( _temp2779 < _temp2777.base? 1: _temp2779 >= _temp2777.last_plus_one){
_throw( Null_Exception);}* _temp2779;})); _temp2773->f3= 0;( void*) _temp2773;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2780= yyls; struct Cyc_Yyltype*
_temp2782= _temp2780.curr +( yylsp_offset - 1); if( _temp2782 < _temp2780.base?
1: _temp2782 >= _temp2780.last_plus_one){ _throw( Null_Exception);}* _temp2782;}).first_line,({
struct _tagged_ptr3 _temp2783= yyls; struct Cyc_Yyltype* _temp2785= _temp2783.curr
+ yylsp_offset; if( _temp2785 < _temp2783.base? 1: _temp2785 >= _temp2783.last_plus_one){
_throw( Null_Exception);}* _temp2785;}).last_line))};( struct _xenum_struct*)
_temp2772;}); break; case 67: _LL2771: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp2787=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2787=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec(({ struct Cyc_Absyn_TupleType_struct* _temp2788=( struct Cyc_Absyn_TupleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp2788->tag= Cyc_Absyn_TupleType_tag;
_temp2788->f1=(( struct Cyc_List_List*(*)( struct _tuple9*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2789= yyls; struct Cyc_Yyltype* _temp2791= _temp2789.curr +(
yylsp_offset - 1); if( _temp2791 < _temp2789.base? 1: _temp2791 >= _temp2789.last_plus_one){
_throw( Null_Exception);}* _temp2791;}).first_line,({ struct _tagged_ptr3
_temp2792= yyls; struct Cyc_Yyltype* _temp2794= _temp2792.curr +( yylsp_offset -
1); if( _temp2794 < _temp2792.base? 1: _temp2794 >= _temp2792.last_plus_one){
_throw( Null_Exception);}* _temp2794;}).last_line),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp2795= yyvs; struct _xenum_struct** _temp2797= _temp2795.curr
+( yyvsp_offset - 1); if( _temp2797 < _temp2795.base? 1: _temp2797 >= _temp2795.last_plus_one){
_throw( Null_Exception);}* _temp2797;}))));( void*) _temp2788;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2798= yyls; struct Cyc_Yyltype* _temp2800= _temp2798.curr
+( yylsp_offset - 3); if( _temp2800 < _temp2798.base? 1: _temp2800 >= _temp2798.last_plus_one){
_throw( Null_Exception);}* _temp2800;}).first_line,({ struct _tagged_ptr3
_temp2801= yyls; struct Cyc_Yyltype* _temp2803= _temp2801.curr + yylsp_offset;
if( _temp2803 < _temp2801.base? 1: _temp2803 >= _temp2801.last_plus_one){ _throw(
Null_Exception);}* _temp2803;}).last_line))};( struct _xenum_struct*) _temp2787;});
break; case 68: _LL2786: yyval=({ struct Cyc_Kind_tok_struct* _temp2805=( struct
Cyc_Kind_tok_struct*) GC_malloc( sizeof( struct Cyc_Kind_tok_struct));*
_temp2805=( struct Cyc_Kind_tok_struct){.tag= Cyc_Kind_tok_tag,.f1= Cyc_Parse_id_to_kind(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2806= yyvs; struct
_xenum_struct** _temp2808= _temp2806.curr + yyvsp_offset; if( _temp2808 <
_temp2806.base? 1: _temp2808 >= _temp2806.last_plus_one){ _throw( Null_Exception);}*
_temp2808;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2809=
yyls; struct Cyc_Yyltype* _temp2811= _temp2809.curr + yylsp_offset; if(
_temp2811 < _temp2809.base? 1: _temp2811 >= _temp2809.last_plus_one){ _throw(
Null_Exception);}* _temp2811;}).first_line,({ struct _tagged_ptr3 _temp2812=
yyls; struct Cyc_Yyltype* _temp2814= _temp2812.curr + yylsp_offset; if(
_temp2814 < _temp2812.base? 1: _temp2814 >= _temp2812.last_plus_one){ _throw(
Null_Exception);}* _temp2814;}).last_line))};( struct _xenum_struct*) _temp2805;});
break; case 69: _LL2804: { struct _tagged_string* _temp2821; void* _temp2823;
struct _tuple1 _temp2819=* Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp2816=
yyvs; struct _xenum_struct** _temp2818= _temp2816.curr + yyvsp_offset; if(
_temp2818 < _temp2816.base? 1: _temp2818 >= _temp2816.last_plus_one){ _throw(
Null_Exception);}* _temp2818;})); _LL2824: _temp2823= _temp2819.f1; goto _LL2822;
_LL2822: _temp2821= _temp2819.f2; goto _LL2820; _LL2820: if( _temp2823 != Cyc_Absyn_Loc_n){
Cyc_Parse_err(( struct _tagged_string)({ char* _temp2825=( char*)"bad kind in type specifier";
struct _tagged_string _temp2826; _temp2826.curr= _temp2825; _temp2826.base=
_temp2825; _temp2826.last_plus_one= _temp2825 + 27; _temp2826;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2827= yyls; struct Cyc_Yyltype* _temp2829= _temp2827.curr
+ yylsp_offset; if( _temp2829 < _temp2827.base? 1: _temp2829 >= _temp2827.last_plus_one){
_throw( Null_Exception);}* _temp2829;}).first_line,({ struct _tagged_ptr3
_temp2830= yyls; struct Cyc_Yyltype* _temp2832= _temp2830.curr + yylsp_offset;
if( _temp2832 < _temp2830.base? 1: _temp2832 >= _temp2830.last_plus_one){ _throw(
Null_Exception);}* _temp2832;}).last_line));} yyval=({ struct Cyc_Kind_tok_struct*
_temp2833=( struct Cyc_Kind_tok_struct*) GC_malloc( sizeof( struct Cyc_Kind_tok_struct));*
_temp2833=( struct Cyc_Kind_tok_struct){.tag= Cyc_Kind_tok_tag,.f1= Cyc_Parse_id_to_kind(*
_temp2821, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2834= yyls;
struct Cyc_Yyltype* _temp2836= _temp2834.curr + yylsp_offset; if( _temp2836 <
_temp2834.base? 1: _temp2836 >= _temp2834.last_plus_one){ _throw( Null_Exception);}*
_temp2836;}).first_line,({ struct _tagged_ptr3 _temp2837= yyls; struct Cyc_Yyltype*
_temp2839= _temp2837.curr + yylsp_offset; if( _temp2839 < _temp2837.base? 1:
_temp2839 >= _temp2837.last_plus_one){ _throw( Null_Exception);}* _temp2839;}).last_line))};(
struct _xenum_struct*) _temp2833;}); break;} case 70: _LL2815: yyval=({ struct
Cyc_TypeQual_tok_struct* _temp2841=( struct Cyc_TypeQual_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeQual_tok_struct));* _temp2841=( struct Cyc_TypeQual_tok_struct){.tag=
Cyc_TypeQual_tok_tag,.f1=({ struct Cyc_Absyn_Tqual* _temp2842=( struct Cyc_Absyn_Tqual*)
GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual)); _temp2842->q_const= 1;
_temp2842->q_volatile= 0; _temp2842->q_restrict= 0; _temp2842;})};( struct
_xenum_struct*) _temp2841;}); break; case 71: _LL2840: yyval=({ struct Cyc_TypeQual_tok_struct*
_temp2844=( struct Cyc_TypeQual_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct));*
_temp2844=( struct Cyc_TypeQual_tok_struct){.tag= Cyc_TypeQual_tok_tag,.f1=({
struct Cyc_Absyn_Tqual* _temp2845=( struct Cyc_Absyn_Tqual*) GC_malloc_atomic(
sizeof( struct Cyc_Absyn_Tqual)); _temp2845->q_const= 0; _temp2845->q_volatile=
1; _temp2845->q_restrict= 0; _temp2845;})};( struct _xenum_struct*) _temp2844;});
break; case 72: _LL2843: yyval=({ struct Cyc_TypeQual_tok_struct* _temp2847=(
struct Cyc_TypeQual_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct));*
_temp2847=( struct Cyc_TypeQual_tok_struct){.tag= Cyc_TypeQual_tok_tag,.f1=({
struct Cyc_Absyn_Tqual* _temp2848=( struct Cyc_Absyn_Tqual*) GC_malloc_atomic(
sizeof( struct Cyc_Absyn_Tqual)); _temp2848->q_const= 0; _temp2848->q_volatile=
0; _temp2848->q_restrict= 1; _temp2848;})};( struct _xenum_struct*) _temp2847;});
break; case 73: _LL2846: { struct Cyc_Absyn_Decl* d;{ void* _temp2853= Cyc_yyget_StructOrUnion_tok(({
struct _tagged_ptr2 _temp2850= yyvs; struct _xenum_struct** _temp2852= _temp2850.curr
+( yyvsp_offset - 3); if( _temp2852 < _temp2850.base? 1: _temp2852 >= _temp2850.last_plus_one){
_throw( Null_Exception);}* _temp2852;})); _LL2855: if( _temp2853 == Cyc_Parse_Struct_su){
goto _LL2856;} else{ goto _LL2857;} _LL2857: if( _temp2853 == Cyc_Parse_Union_su){
goto _LL2858;} else{ goto _LL2854;} _LL2856: d= Cyc_Absyn_struct_decl( Cyc_Absyn_Public,
0, 0,({ struct Cyc_Core_Opt* _temp2859=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp2859->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp2860= yyvs; struct _xenum_struct** _temp2862= _temp2860.curr
+( yyvsp_offset - 1); if( _temp2862 < _temp2860.base? 1: _temp2862 >= _temp2860.last_plus_one){
_throw( Null_Exception);}* _temp2862;})); _temp2859;}), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2863= yyls; struct Cyc_Yyltype* _temp2865= _temp2863.curr
+( yylsp_offset - 3); if( _temp2865 < _temp2863.base? 1: _temp2865 >= _temp2863.last_plus_one){
_throw( Null_Exception);}* _temp2865;}).first_line,({ struct _tagged_ptr3
_temp2866= yyls; struct Cyc_Yyltype* _temp2868= _temp2866.curr + yylsp_offset;
if( _temp2868 < _temp2866.base? 1: _temp2868 >= _temp2866.last_plus_one){ _throw(
Null_Exception);}* _temp2868;}).last_line)); goto _LL2854; _LL2858: Cyc_Parse_unimp2((
struct _tagged_string)({ char* _temp2869=( char*)"unions"; struct _tagged_string
_temp2870; _temp2870.curr= _temp2869; _temp2870.base= _temp2869; _temp2870.last_plus_one=
_temp2869 + 7; _temp2870;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2871= yyls; struct Cyc_Yyltype* _temp2873= _temp2871.curr +( yylsp_offset -
3); if( _temp2873 < _temp2871.base? 1: _temp2873 >= _temp2871.last_plus_one){
_throw( Null_Exception);}* _temp2873;}).first_line,({ struct _tagged_ptr3
_temp2874= yyls; struct Cyc_Yyltype* _temp2876= _temp2874.curr + yylsp_offset;
if( _temp2876 < _temp2874.base? 1: _temp2876 >= _temp2874.last_plus_one){ _throw(
Null_Exception);}* _temp2876;}).last_line)); d= Cyc_Absyn_new_decl( Cyc_Absyn_Union_d,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2877= yyls; struct Cyc_Yyltype*
_temp2879= _temp2877.curr +( yylsp_offset - 3); if( _temp2879 < _temp2877.base?
1: _temp2879 >= _temp2877.last_plus_one){ _throw( Null_Exception);}* _temp2879;}).first_line,({
struct _tagged_ptr3 _temp2880= yyls; struct Cyc_Yyltype* _temp2882= _temp2880.curr
+ yylsp_offset; if( _temp2882 < _temp2880.base? 1: _temp2882 >= _temp2880.last_plus_one){
_throw( Null_Exception);}* _temp2882;}).last_line)); goto _LL2854; _LL2854:;}
yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2883=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));* _temp2883=( struct
Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=({ struct Cyc_Parse_Decl_spec_struct*
_temp2884=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2884->tag= Cyc_Parse_Decl_spec_tag; _temp2884->f1= d;( void*) _temp2884;})};(
struct _xenum_struct*) _temp2883;}); Cyc_Parse_unimp2(( struct _tagged_string)({
char* _temp2885=( char*)"anonymous structs/unions"; struct _tagged_string
_temp2886; _temp2886.curr= _temp2885; _temp2886.base= _temp2885; _temp2886.last_plus_one=
_temp2885 + 25; _temp2886;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2887= yyls; struct Cyc_Yyltype* _temp2889= _temp2887.curr +( yylsp_offset -
3); if( _temp2889 < _temp2887.base? 1: _temp2889 >= _temp2887.last_plus_one){
_throw( Null_Exception);}* _temp2889;}).first_line,({ struct _tagged_ptr3
_temp2890= yyls; struct Cyc_Yyltype* _temp2892= _temp2890.curr + yylsp_offset;
if( _temp2892 < _temp2890.base? 1: _temp2892 >= _temp2890.last_plus_one){ _throw(
Null_Exception);}* _temp2892;}).last_line)); break;} case 74: _LL2849: { struct
Cyc_List_List* ts=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2894= yyls; struct Cyc_Yyltype* _temp2896= _temp2894.curr
+( yylsp_offset - 3); if( _temp2896 < _temp2894.base? 1: _temp2896 >= _temp2894.last_plus_one){
_throw( Null_Exception);}* _temp2896;}).first_line,({ struct _tagged_ptr3
_temp2897= yyls; struct Cyc_Yyltype* _temp2899= _temp2897.curr +( yylsp_offset -
3); if( _temp2899 < _temp2897.base? 1: _temp2899 >= _temp2897.last_plus_one){
_throw( Null_Exception);}* _temp2899;}).last_line), Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp2900= yyvs; struct _xenum_struct** _temp2902= _temp2900.curr
+( yyvsp_offset - 3); if( _temp2902 < _temp2900.base? 1: _temp2902 >= _temp2900.last_plus_one){
_throw( Null_Exception);}* _temp2902;}))); struct Cyc_Absyn_Decl* d;{ void*
_temp2906= Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2 _temp2903= yyvs;
struct _xenum_struct** _temp2905= _temp2903.curr +( yyvsp_offset - 5); if(
_temp2905 < _temp2903.base? 1: _temp2905 >= _temp2903.last_plus_one){ _throw(
Null_Exception);}* _temp2905;})); _LL2908: if( _temp2906 == Cyc_Parse_Struct_su){
goto _LL2909;} else{ goto _LL2910;} _LL2910: if( _temp2906 == Cyc_Parse_Union_su){
goto _LL2911;} else{ goto _LL2907;} _LL2909: d= Cyc_Absyn_struct_decl( Cyc_Absyn_Public,({
struct Cyc_Core_Opt* _temp2912=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2912->v=( void*) Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp2913= yyvs; struct _xenum_struct** _temp2915= _temp2913.curr +(
yyvsp_offset - 4); if( _temp2915 < _temp2913.base? 1: _temp2915 >= _temp2913.last_plus_one){
_throw( Null_Exception);}* _temp2915;})); _temp2912;}), ts,({ struct Cyc_Core_Opt*
_temp2916=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2916->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({ struct _tagged_ptr2
_temp2917= yyvs; struct _xenum_struct** _temp2919= _temp2917.curr +(
yyvsp_offset - 1); if( _temp2919 < _temp2917.base? 1: _temp2919 >= _temp2917.last_plus_one){
_throw( Null_Exception);}* _temp2919;})); _temp2916;}), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2920= yyls; struct Cyc_Yyltype* _temp2922= _temp2920.curr
+( yylsp_offset - 5); if( _temp2922 < _temp2920.base? 1: _temp2922 >= _temp2920.last_plus_one){
_throw( Null_Exception);}* _temp2922;}).first_line,({ struct _tagged_ptr3
_temp2923= yyls; struct Cyc_Yyltype* _temp2925= _temp2923.curr + yylsp_offset;
if( _temp2925 < _temp2923.base? 1: _temp2925 >= _temp2923.last_plus_one){ _throw(
Null_Exception);}* _temp2925;}).last_line)); goto _LL2907; _LL2911: Cyc_Parse_unimp2((
struct _tagged_string)({ char* _temp2926=( char*)"unions"; struct _tagged_string
_temp2927; _temp2927.curr= _temp2926; _temp2927.base= _temp2926; _temp2927.last_plus_one=
_temp2926 + 7; _temp2927;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2928= yyls; struct Cyc_Yyltype* _temp2930= _temp2928.curr +( yylsp_offset -
5); if( _temp2930 < _temp2928.base? 1: _temp2930 >= _temp2928.last_plus_one){
_throw( Null_Exception);}* _temp2930;}).first_line,({ struct _tagged_ptr3
_temp2931= yyls; struct Cyc_Yyltype* _temp2933= _temp2931.curr + yylsp_offset;
if( _temp2933 < _temp2931.base? 1: _temp2933 >= _temp2931.last_plus_one){ _throw(
Null_Exception);}* _temp2933;}).last_line)); d= Cyc_Absyn_new_decl( Cyc_Absyn_Union_d,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2934= yyls; struct Cyc_Yyltype*
_temp2936= _temp2934.curr +( yylsp_offset - 5); if( _temp2936 < _temp2934.base?
1: _temp2936 >= _temp2934.last_plus_one){ _throw( Null_Exception);}* _temp2936;}).first_line,({
struct _tagged_ptr3 _temp2937= yyls; struct Cyc_Yyltype* _temp2939= _temp2937.curr
+ yylsp_offset; if( _temp2939 < _temp2937.base? 1: _temp2939 >= _temp2937.last_plus_one){
_throw( Null_Exception);}* _temp2939;}).last_line)); goto _LL2907; _LL2907:;}
yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2940=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));* _temp2940=( struct
Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=({ struct Cyc_Parse_Decl_spec_struct*
_temp2941=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2941->tag= Cyc_Parse_Decl_spec_tag; _temp2941->f1= d;( void*) _temp2941;})};(
struct _xenum_struct*) _temp2940;}); break;} case 75: _LL2893: { struct Cyc_List_List*
ts=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2943=
yyls; struct Cyc_Yyltype* _temp2945= _temp2943.curr +( yylsp_offset - 3); if(
_temp2945 < _temp2943.base? 1: _temp2945 >= _temp2943.last_plus_one){ _throw(
Null_Exception);}* _temp2945;}).first_line,({ struct _tagged_ptr3 _temp2946=
yyls; struct Cyc_Yyltype* _temp2948= _temp2946.curr +( yylsp_offset - 3); if(
_temp2948 < _temp2946.base? 1: _temp2948 >= _temp2946.last_plus_one){ _throw(
Null_Exception);}* _temp2948;}).last_line), Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp2949= yyvs; struct _xenum_struct** _temp2951= _temp2949.curr +(
yyvsp_offset - 3); if( _temp2951 < _temp2949.base? 1: _temp2951 >= _temp2949.last_plus_one){
_throw( Null_Exception);}* _temp2951;}))); struct Cyc_Absyn_Decl* d;{ void*
_temp2955= Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2 _temp2952= yyvs;
struct _xenum_struct** _temp2954= _temp2952.curr +( yyvsp_offset - 5); if(
_temp2954 < _temp2952.base? 1: _temp2954 >= _temp2952.last_plus_one){ _throw(
Null_Exception);}* _temp2954;})); _LL2957: if( _temp2955 == Cyc_Parse_Struct_su){
goto _LL2958;} else{ goto _LL2959;} _LL2959: if( _temp2955 == Cyc_Parse_Union_su){
goto _LL2960;} else{ goto _LL2956;} _LL2958: d= Cyc_Absyn_struct_decl( Cyc_Absyn_Public,({
struct Cyc_Core_Opt* _temp2961=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2961->v=( void*) Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp2962= yyvs; struct _xenum_struct** _temp2964= _temp2962.curr +(
yyvsp_offset - 4); if( _temp2964 < _temp2962.base? 1: _temp2964 >= _temp2962.last_plus_one){
_throw( Null_Exception);}* _temp2964;})); _temp2961;}), ts,({ struct Cyc_Core_Opt*
_temp2965=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2965->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({ struct _tagged_ptr2
_temp2966= yyvs; struct _xenum_struct** _temp2968= _temp2966.curr +(
yyvsp_offset - 1); if( _temp2968 < _temp2966.base? 1: _temp2968 >= _temp2966.last_plus_one){
_throw( Null_Exception);}* _temp2968;})); _temp2965;}), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2969= yyls; struct Cyc_Yyltype* _temp2971= _temp2969.curr
+( yylsp_offset - 5); if( _temp2971 < _temp2969.base? 1: _temp2971 >= _temp2969.last_plus_one){
_throw( Null_Exception);}* _temp2971;}).first_line,({ struct _tagged_ptr3
_temp2972= yyls; struct Cyc_Yyltype* _temp2974= _temp2972.curr + yylsp_offset;
if( _temp2974 < _temp2972.base? 1: _temp2974 >= _temp2972.last_plus_one){ _throw(
Null_Exception);}* _temp2974;}).last_line)); goto _LL2956; _LL2960: Cyc_Parse_unimp2((
struct _tagged_string)({ char* _temp2975=( char*)"unions"; struct _tagged_string
_temp2976; _temp2976.curr= _temp2975; _temp2976.base= _temp2975; _temp2976.last_plus_one=
_temp2975 + 7; _temp2976;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2977= yyls; struct Cyc_Yyltype* _temp2979= _temp2977.curr +( yylsp_offset -
5); if( _temp2979 < _temp2977.base? 1: _temp2979 >= _temp2977.last_plus_one){
_throw( Null_Exception);}* _temp2979;}).first_line,({ struct _tagged_ptr3
_temp2980= yyls; struct Cyc_Yyltype* _temp2982= _temp2980.curr + yylsp_offset;
if( _temp2982 < _temp2980.base? 1: _temp2982 >= _temp2980.last_plus_one){ _throw(
Null_Exception);}* _temp2982;}).last_line)); d= Cyc_Absyn_new_decl( Cyc_Absyn_Union_d,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2983= yyls; struct Cyc_Yyltype*
_temp2985= _temp2983.curr +( yylsp_offset - 5); if( _temp2985 < _temp2983.base?
1: _temp2985 >= _temp2983.last_plus_one){ _throw( Null_Exception);}* _temp2985;}).first_line,({
struct _tagged_ptr3 _temp2986= yyls; struct Cyc_Yyltype* _temp2988= _temp2986.curr
+ yylsp_offset; if( _temp2988 < _temp2986.base? 1: _temp2988 >= _temp2986.last_plus_one){
_throw( Null_Exception);}* _temp2988;}).last_line)); goto _LL2956; _LL2956:;}
yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2989=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));* _temp2989=( struct
Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=({ struct Cyc_Parse_Decl_spec_struct*
_temp2990=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2990->tag= Cyc_Parse_Decl_spec_tag; _temp2990->f1= d;( void*) _temp2990;})};(
struct _xenum_struct*) _temp2989;}); break;} case 76: _LL2942:{ void* _temp2995=
Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2 _temp2992= yyvs; struct
_xenum_struct** _temp2994= _temp2992.curr +( yyvsp_offset - 2); if( _temp2994 <
_temp2992.base? 1: _temp2994 >= _temp2992.last_plus_one){ _throw( Null_Exception);}*
_temp2994;})); _LL2997: if( _temp2995 == Cyc_Parse_Struct_su){ goto _LL2998;}
else{ goto _LL2999;} _LL2999: if( _temp2995 == Cyc_Parse_Union_su){ goto _LL3000;}
else{ goto _LL2996;} _LL2998: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp3001=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp3001=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec(({ struct Cyc_Absyn_StructType_struct* _temp3002=( struct
Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp3002->tag= Cyc_Absyn_StructType_tag; _temp3002->f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3003= yyvs; struct _xenum_struct** _temp3005= _temp3003.curr
+( yyvsp_offset - 1); if( _temp3005 < _temp3003.base? 1: _temp3005 >= _temp3003.last_plus_one){
_throw( Null_Exception);}* _temp3005;})); _temp3002->f2= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3006= yyvs; struct _xenum_struct** _temp3008= _temp3006.curr
+ yyvsp_offset; if( _temp3008 < _temp3006.base? 1: _temp3008 >= _temp3006.last_plus_one){
_throw( Null_Exception);}* _temp3008;})); _temp3002->f3= 0;( void*) _temp3002;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3009= yyls; struct Cyc_Yyltype*
_temp3011= _temp3009.curr +( yylsp_offset - 2); if( _temp3011 < _temp3009.base?
1: _temp3011 >= _temp3009.last_plus_one){ _throw( Null_Exception);}* _temp3011;}).first_line,({
struct _tagged_ptr3 _temp3012= yyls; struct Cyc_Yyltype* _temp3014= _temp3012.curr
+ yylsp_offset; if( _temp3014 < _temp3012.base? 1: _temp3014 >= _temp3012.last_plus_one){
_throw( Null_Exception);}* _temp3014;}).last_line))};( struct _xenum_struct*)
_temp3001;}); goto _LL2996; _LL3000: Cyc_Parse_unimp2(( struct _tagged_string)({
char* _temp3015=( char*)"unions"; struct _tagged_string _temp3016; _temp3016.curr=
_temp3015; _temp3016.base= _temp3015; _temp3016.last_plus_one= _temp3015 + 7;
_temp3016;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3017= yyls;
struct Cyc_Yyltype* _temp3019= _temp3017.curr +( yylsp_offset - 2); if(
_temp3019 < _temp3017.base? 1: _temp3019 >= _temp3017.last_plus_one){ _throw(
Null_Exception);}* _temp3019;}).first_line,({ struct _tagged_ptr3 _temp3020=
yyls; struct Cyc_Yyltype* _temp3022= _temp3020.curr + yylsp_offset; if(
_temp3022 < _temp3020.base? 1: _temp3022 >= _temp3020.last_plus_one){ _throw(
Null_Exception);}* _temp3022;}).last_line)); yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp3023=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp3023=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=({
struct Cyc_Parse_Decl_spec_struct* _temp3024=( struct Cyc_Parse_Decl_spec_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct)); _temp3024->tag= Cyc_Parse_Decl_spec_tag;
_temp3024->f1= Cyc_Absyn_new_decl( Cyc_Absyn_Union_d, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3025= yyls; struct Cyc_Yyltype* _temp3027= _temp3025.curr
+( yylsp_offset - 2); if( _temp3027 < _temp3025.base? 1: _temp3027 >= _temp3025.last_plus_one){
_throw( Null_Exception);}* _temp3027;}).first_line,({ struct _tagged_ptr3
_temp3028= yyls; struct Cyc_Yyltype* _temp3030= _temp3028.curr + yylsp_offset;
if( _temp3030 < _temp3028.base? 1: _temp3030 >= _temp3028.last_plus_one){ _throw(
Null_Exception);}* _temp3030;}).last_line));( void*) _temp3024;})};( struct
_xenum_struct*) _temp3023;}); goto _LL2996; _LL2996:;} break; case 77: _LL2991:{
void* _temp3035= Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2 _temp3032=
yyvs; struct _xenum_struct** _temp3034= _temp3032.curr +( yyvsp_offset - 2); if(
_temp3034 < _temp3032.base? 1: _temp3034 >= _temp3032.last_plus_one){ _throw(
Null_Exception);}* _temp3034;})); _LL3037: if( _temp3035 == Cyc_Parse_Struct_su){
goto _LL3038;} else{ goto _LL3039;} _LL3039: if( _temp3035 == Cyc_Parse_Union_su){
goto _LL3040;} else{ goto _LL3036;} _LL3038: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp3041=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp3041=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec(({ struct Cyc_Absyn_StructType_struct* _temp3042=( struct
Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp3042->tag= Cyc_Absyn_StructType_tag; _temp3042->f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3043= yyvs; struct _xenum_struct** _temp3045= _temp3043.curr
+( yyvsp_offset - 1); if( _temp3045 < _temp3043.base? 1: _temp3045 >= _temp3043.last_plus_one){
_throw( Null_Exception);}* _temp3045;})); _temp3042->f2= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3046= yyvs; struct _xenum_struct** _temp3048= _temp3046.curr
+ yyvsp_offset; if( _temp3048 < _temp3046.base? 1: _temp3048 >= _temp3046.last_plus_one){
_throw( Null_Exception);}* _temp3048;})); _temp3042->f3= 0;( void*) _temp3042;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3049= yyls; struct Cyc_Yyltype*
_temp3051= _temp3049.curr +( yylsp_offset - 2); if( _temp3051 < _temp3049.base?
1: _temp3051 >= _temp3049.last_plus_one){ _throw( Null_Exception);}* _temp3051;}).first_line,({
struct _tagged_ptr3 _temp3052= yyls; struct Cyc_Yyltype* _temp3054= _temp3052.curr
+ yylsp_offset; if( _temp3054 < _temp3052.base? 1: _temp3054 >= _temp3052.last_plus_one){
_throw( Null_Exception);}* _temp3054;}).last_line))};( struct _xenum_struct*)
_temp3041;}); goto _LL3036; _LL3040: Cyc_Parse_unimp2(( struct _tagged_string)({
char* _temp3055=( char*)"unions"; struct _tagged_string _temp3056; _temp3056.curr=
_temp3055; _temp3056.base= _temp3055; _temp3056.last_plus_one= _temp3055 + 7;
_temp3056;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3057= yyls;
struct Cyc_Yyltype* _temp3059= _temp3057.curr +( yylsp_offset - 2); if(
_temp3059 < _temp3057.base? 1: _temp3059 >= _temp3057.last_plus_one){ _throw(
Null_Exception);}* _temp3059;}).first_line,({ struct _tagged_ptr3 _temp3060=
yyls; struct Cyc_Yyltype* _temp3062= _temp3060.curr + yylsp_offset; if(
_temp3062 < _temp3060.base? 1: _temp3062 >= _temp3060.last_plus_one){ _throw(
Null_Exception);}* _temp3062;}).last_line)); yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp3063=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp3063=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=({
struct Cyc_Parse_Decl_spec_struct* _temp3064=( struct Cyc_Parse_Decl_spec_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct)); _temp3064->tag= Cyc_Parse_Decl_spec_tag;
_temp3064->f1= Cyc_Absyn_new_decl( Cyc_Absyn_Union_d, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3065= yyls; struct Cyc_Yyltype* _temp3067= _temp3065.curr
+( yylsp_offset - 2); if( _temp3067 < _temp3065.base? 1: _temp3067 >= _temp3065.last_plus_one){
_throw( Null_Exception);}* _temp3067;}).first_line,({ struct _tagged_ptr3
_temp3068= yyls; struct Cyc_Yyltype* _temp3070= _temp3068.curr + yylsp_offset;
if( _temp3070 < _temp3068.base? 1: _temp3070 >= _temp3068.last_plus_one){ _throw(
Null_Exception);}* _temp3070;}).last_line));( void*) _temp3064;})};( struct
_xenum_struct*) _temp3063;}); goto _LL3036; _LL3036:;} break; case 78: _LL3031:
yyval=({ struct Cyc_TypeList_tok_struct* _temp3072=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));* _temp3072=( struct Cyc_TypeList_tok_struct){.tag=
Cyc_TypeList_tok_tag,.f1= 0};( struct _xenum_struct*) _temp3072;}); break; case
79: _LL3071: yyval=({ struct Cyc_TypeList_tok_struct* _temp3074=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));* _temp3074=( struct Cyc_TypeList_tok_struct){.tag=
Cyc_TypeList_tok_tag,.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3075= yyvs;
struct _xenum_struct** _temp3077= _temp3075.curr +( yyvsp_offset - 1); if(
_temp3077 < _temp3075.base? 1: _temp3077 >= _temp3075.last_plus_one){ _throw(
Null_Exception);}* _temp3077;})))};( struct _xenum_struct*) _temp3074;}); break;
case 80: _LL3073:(( struct Cyc_Lexing_lexbuf*) Cyc_Parse_lbuf->v)->lex_curr_pos
-= 1; yyval=({ struct Cyc_TypeList_tok_struct* _temp3079=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));* _temp3079=( struct Cyc_TypeList_tok_struct){.tag=
Cyc_TypeList_tok_tag,.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3080= yyvs;
struct _xenum_struct** _temp3082= _temp3080.curr +( yyvsp_offset - 1); if(
_temp3082 < _temp3080.base? 1: _temp3082 >= _temp3080.last_plus_one){ _throw(
Null_Exception);}* _temp3082;})))};( struct _xenum_struct*) _temp3079;}); break;
case 81: _LL3078: yyval=({ struct Cyc_StructOrUnion_tok_struct* _temp3084=(
struct Cyc_StructOrUnion_tok_struct*) GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));*
_temp3084=( struct Cyc_StructOrUnion_tok_struct){.tag= Cyc_StructOrUnion_tok_tag,.f1=
Cyc_Parse_Struct_su};( struct _xenum_struct*) _temp3084;}); break; case 82:
_LL3083: yyval=({ struct Cyc_StructOrUnion_tok_struct* _temp3086=( struct Cyc_StructOrUnion_tok_struct*)
GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));* _temp3086=( struct
Cyc_StructOrUnion_tok_struct){.tag= Cyc_StructOrUnion_tok_tag,.f1= Cyc_Parse_Union_su};(
struct _xenum_struct*) _temp3086;}); break; case 83: _LL3085: yyval=({ struct
Cyc_StructFieldDeclList_tok_struct* _temp3088=( struct Cyc_StructFieldDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_StructFieldDeclList_tok_struct));* _temp3088=(
struct Cyc_StructFieldDeclList_tok_struct){.tag= Cyc_StructFieldDeclList_tok_tag,.f1=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)((( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_StructFieldDeclListList_tok(({
struct _tagged_ptr2 _temp3089= yyvs; struct _xenum_struct** _temp3091= _temp3089.curr
+ yyvsp_offset; if( _temp3091 < _temp3089.base? 1: _temp3091 >= _temp3089.last_plus_one){
_throw( Null_Exception);}* _temp3091;}))))};( struct _xenum_struct*) _temp3088;});
break; case 84: _LL3087: yyval=({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp3093=( struct Cyc_StructFieldDeclListList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct));* _temp3093=( struct Cyc_StructFieldDeclListList_tok_struct){.tag=
Cyc_StructFieldDeclListList_tok_tag,.f1=({ struct Cyc_List_List* _temp3094=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3094->hd=(
void*) Cyc_yyget_StructFieldDeclList_tok(({ struct _tagged_ptr2 _temp3095= yyvs;
struct _xenum_struct** _temp3097= _temp3095.curr + yyvsp_offset; if( _temp3097 <
_temp3095.base? 1: _temp3097 >= _temp3095.last_plus_one){ _throw( Null_Exception);}*
_temp3097;})); _temp3094->tl= 0; _temp3094;})};( struct _xenum_struct*)
_temp3093;}); break; case 85: _LL3092: yyval=({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp3099=( struct Cyc_StructFieldDeclListList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct));* _temp3099=( struct Cyc_StructFieldDeclListList_tok_struct){.tag=
Cyc_StructFieldDeclListList_tok_tag,.f1=({ struct Cyc_List_List* _temp3100=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3100->hd=(
void*) Cyc_yyget_StructFieldDeclList_tok(({ struct _tagged_ptr2 _temp3104= yyvs;
struct _xenum_struct** _temp3106= _temp3104.curr + yyvsp_offset; if( _temp3106 <
_temp3104.base? 1: _temp3106 >= _temp3104.last_plus_one){ _throw( Null_Exception);}*
_temp3106;})); _temp3100->tl= Cyc_yyget_StructFieldDeclListList_tok(({ struct
_tagged_ptr2 _temp3101= yyvs; struct _xenum_struct** _temp3103= _temp3101.curr +(
yyvsp_offset - 1); if( _temp3103 < _temp3101.base? 1: _temp3103 >= _temp3101.last_plus_one){
_throw( Null_Exception);}* _temp3103;})); _temp3100;})};( struct _xenum_struct*)
_temp3099;}); break; case 86: _LL3098: yyval=({ struct Cyc_InitDeclList_tok_struct*
_temp3108=( struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));*
_temp3108=( struct Cyc_InitDeclList_tok_struct){.tag= Cyc_InitDeclList_tok_tag,.f1=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitDeclList_tok(({
struct _tagged_ptr2 _temp3109= yyvs; struct _xenum_struct** _temp3111= _temp3109.curr
+ yyvsp_offset; if( _temp3111 < _temp3109.base? 1: _temp3111 >= _temp3109.last_plus_one){
_throw( Null_Exception);}* _temp3111;})))};( struct _xenum_struct*) _temp3108;});
break; case 87: _LL3107: yyval=({ struct Cyc_InitDeclList_tok_struct* _temp3113=(
struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));*
_temp3113=( struct Cyc_InitDeclList_tok_struct){.tag= Cyc_InitDeclList_tok_tag,.f1=({
struct Cyc_List_List* _temp3114=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3114->hd=( void*) Cyc_yyget_InitDecl_tok(({ struct
_tagged_ptr2 _temp3115= yyvs; struct _xenum_struct** _temp3117= _temp3115.curr +
yyvsp_offset; if( _temp3117 < _temp3115.base? 1: _temp3117 >= _temp3115.last_plus_one){
_throw( Null_Exception);}* _temp3117;})); _temp3114->tl= 0; _temp3114;})};(
struct _xenum_struct*) _temp3113;}); break; case 88: _LL3112: yyval=({ struct
Cyc_InitDeclList_tok_struct* _temp3119=( struct Cyc_InitDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));* _temp3119=( struct Cyc_InitDeclList_tok_struct){.tag=
Cyc_InitDeclList_tok_tag,.f1=({ struct Cyc_List_List* _temp3120=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3120->hd=( void*) Cyc_yyget_InitDecl_tok(({
struct _tagged_ptr2 _temp3124= yyvs; struct _xenum_struct** _temp3126= _temp3124.curr
+ yyvsp_offset; if( _temp3126 < _temp3124.base? 1: _temp3126 >= _temp3124.last_plus_one){
_throw( Null_Exception);}* _temp3126;})); _temp3120->tl= Cyc_yyget_InitDeclList_tok(({
struct _tagged_ptr2 _temp3121= yyvs; struct _xenum_struct** _temp3123= _temp3121.curr
+( yyvsp_offset - 2); if( _temp3123 < _temp3121.base? 1: _temp3123 >= _temp3121.last_plus_one){
_throw( Null_Exception);}* _temp3123;})); _temp3120;})};( struct _xenum_struct*)
_temp3119;}); break; case 89: _LL3118: yyval=({ struct Cyc_InitDecl_tok_struct*
_temp3128=( struct Cyc_InitDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct));*
_temp3128=( struct Cyc_InitDecl_tok_struct){.tag= Cyc_InitDecl_tok_tag,.f1=({
struct _tuple12* _temp3129=( struct _tuple12*) GC_malloc( sizeof( struct
_tuple12)); _temp3129->f1= Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2
_temp3130= yyvs; struct _xenum_struct** _temp3132= _temp3130.curr + yyvsp_offset;
if( _temp3132 < _temp3130.base? 1: _temp3132 >= _temp3130.last_plus_one){ _throw(
Null_Exception);}* _temp3132;})); _temp3129->f2= 0; _temp3129;})};( struct
_xenum_struct*) _temp3128;}); break; case 90: _LL3127: yyval=({ struct Cyc_InitDecl_tok_struct*
_temp3134=( struct Cyc_InitDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct));*
_temp3134=( struct Cyc_InitDecl_tok_struct){.tag= Cyc_InitDecl_tok_tag,.f1=({
struct _tuple12* _temp3135=( struct _tuple12*) GC_malloc( sizeof( struct
_tuple12)); _temp3135->f1= Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2
_temp3139= yyvs; struct _xenum_struct** _temp3141= _temp3139.curr +(
yyvsp_offset - 2); if( _temp3141 < _temp3139.base? 1: _temp3141 >= _temp3139.last_plus_one){
_throw( Null_Exception);}* _temp3141;})); _temp3135->f2=( struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp3136= yyvs; struct _xenum_struct**
_temp3138= _temp3136.curr + yyvsp_offset; if( _temp3138 < _temp3136.base? 1:
_temp3138 >= _temp3136.last_plus_one){ _throw( Null_Exception);}* _temp3138;}));
_temp3135;})};( struct _xenum_struct*) _temp3134;}); break; case 91: _LL3133: {
struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp3143= yyvs; struct _xenum_struct** _temp3145= _temp3143.curr +(
yyvsp_offset - 2); if( _temp3145 < _temp3143.base? 1: _temp3145 >= _temp3143.last_plus_one){
_throw( Null_Exception);}* _temp3145;}))).f1; struct Cyc_List_List* atts=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3146= yyvs; struct _xenum_struct** _temp3148= _temp3146.curr
+( yyvsp_offset - 2); if( _temp3148 < _temp3146.base? 1: _temp3148 >= _temp3146.last_plus_one){
_throw( Null_Exception);}* _temp3148;}))).f3; void* t= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp3149= yyvs; struct
_xenum_struct** _temp3151= _temp3149.curr +( yyvsp_offset - 2); if( _temp3151 <
_temp3149.base? 1: _temp3151 >= _temp3149.last_plus_one){ _throw( Null_Exception);}*
_temp3151;}))).f2, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3152=
yyls; struct Cyc_Yyltype* _temp3154= _temp3152.curr +( yylsp_offset - 2); if(
_temp3154 < _temp3152.base? 1: _temp3154 >= _temp3152.last_plus_one){ _throw(
Null_Exception);}* _temp3154;}).first_line,({ struct _tagged_ptr3 _temp3155=
yyls; struct Cyc_Yyltype* _temp3157= _temp3155.curr +( yylsp_offset - 2); if(
_temp3157 < _temp3155.base? 1: _temp3157 >= _temp3155.last_plus_one){ _throw(
Null_Exception);}* _temp3157;}).last_line)); struct Cyc_List_List* info= Cyc_Parse_apply_tmss(
tq, t, Cyc_yyget_DeclaratorList_tok(({ struct _tagged_ptr2 _temp3158= yyvs;
struct _xenum_struct** _temp3160= _temp3158.curr +( yyvsp_offset - 1); if(
_temp3160 < _temp3158.base? 1: _temp3160 >= _temp3158.last_plus_one){ _throw(
Null_Exception);}* _temp3160;})), atts); yyval=({ struct Cyc_StructFieldDeclList_tok_struct*
_temp3161=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct));* _temp3161=( struct Cyc_StructFieldDeclList_tok_struct){.tag=
Cyc_StructFieldDeclList_tok_tag,.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Position_Segment*, struct _tuple7*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_make_struct_field, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3162= yyls; struct Cyc_Yyltype* _temp3164= _temp3162.curr
+( yylsp_offset - 2); if( _temp3164 < _temp3162.base? 1: _temp3164 >= _temp3162.last_plus_one){
_throw( Null_Exception);}* _temp3164;}).first_line,({ struct _tagged_ptr3
_temp3165= yyls; struct Cyc_Yyltype* _temp3167= _temp3165.curr +( yylsp_offset -
1); if( _temp3167 < _temp3165.base? 1: _temp3167 >= _temp3165.last_plus_one){
_throw( Null_Exception);}* _temp3167;}).last_line), info)};( struct
_xenum_struct*) _temp3161;}); break;} case 92: _LL3142: yyval=({ struct Cyc_QualSpecList_tok_struct*
_temp3169=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));*
_temp3169=( struct Cyc_QualSpecList_tok_struct){.tag= Cyc_QualSpecList_tok_tag,.f1=({
struct _tuple13* _temp3170=( struct _tuple13*) GC_malloc( sizeof( struct
_tuple13)); _temp3170->f1= Cyc_Absyn_empty_tqual(); _temp3170->f2=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp3174=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3174->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({
struct _tagged_ptr2 _temp3175= yyvs; struct _xenum_struct** _temp3177= _temp3175.curr
+( yyvsp_offset - 1); if( _temp3177 < _temp3175.base? 1: _temp3177 >= _temp3175.last_plus_one){
_throw( Null_Exception);}* _temp3177;})); _temp3174->tl= 0; _temp3174;});
_temp3170->f3= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp3171=
yyvs; struct _xenum_struct** _temp3173= _temp3171.curr + yyvsp_offset; if(
_temp3173 < _temp3171.base? 1: _temp3173 >= _temp3171.last_plus_one){ _throw(
Null_Exception);}* _temp3173;})); _temp3170;})};( struct _xenum_struct*)
_temp3169;}); break; case 93: _LL3168: yyval=({ struct Cyc_QualSpecList_tok_struct*
_temp3179=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));*
_temp3179=( struct Cyc_QualSpecList_tok_struct){.tag= Cyc_QualSpecList_tok_tag,.f1=({
struct _tuple13* _temp3180=( struct _tuple13*) GC_malloc( sizeof( struct
_tuple13)); _temp3180->f1=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp3194= yyvs; struct _xenum_struct** _temp3196= _temp3194.curr + yyvsp_offset;
if( _temp3196 < _temp3194.base? 1: _temp3196 >= _temp3194.last_plus_one){ _throw(
Null_Exception);}* _temp3196;}))).f1; _temp3180->f2=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp3187=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3187->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({
struct _tagged_ptr2 _temp3191= yyvs; struct _xenum_struct** _temp3193= _temp3191.curr
+( yyvsp_offset - 2); if( _temp3193 < _temp3191.base? 1: _temp3193 >= _temp3191.last_plus_one){
_throw( Null_Exception);}* _temp3193;})); _temp3187->tl=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3188= yyvs; struct _xenum_struct** _temp3190= _temp3188.curr
+ yyvsp_offset; if( _temp3190 < _temp3188.base? 1: _temp3190 >= _temp3188.last_plus_one){
_throw( Null_Exception);}* _temp3190;}))).f2; _temp3187;}); _temp3180->f3=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)(
Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2 _temp3181= yyvs; struct
_xenum_struct** _temp3183= _temp3181.curr +( yyvsp_offset - 1); if( _temp3183 <
_temp3181.base? 1: _temp3183 >= _temp3181.last_plus_one){ _throw( Null_Exception);}*
_temp3183;})),(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp3184=
yyvs; struct _xenum_struct** _temp3186= _temp3184.curr + yyvsp_offset; if(
_temp3186 < _temp3184.base? 1: _temp3186 >= _temp3184.last_plus_one){ _throw(
Null_Exception);}* _temp3186;}))).f3); _temp3180;})};( struct _xenum_struct*)
_temp3179;}); break; case 94: _LL3178: yyval=({ struct Cyc_QualSpecList_tok_struct*
_temp3198=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));*
_temp3198=( struct Cyc_QualSpecList_tok_struct){.tag= Cyc_QualSpecList_tok_tag,.f1=({
struct _tuple13* _temp3199=( struct _tuple13*) GC_malloc( sizeof( struct
_tuple13)); _temp3199->f1= Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2
_temp3203= yyvs; struct _xenum_struct** _temp3205= _temp3203.curr +(
yyvsp_offset - 1); if( _temp3205 < _temp3203.base? 1: _temp3205 >= _temp3203.last_plus_one){
_throw( Null_Exception);}* _temp3205;})); _temp3199->f2= 0; _temp3199->f3= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp3200= yyvs; struct _xenum_struct** _temp3202= _temp3200.curr
+ yyvsp_offset; if( _temp3202 < _temp3200.base? 1: _temp3202 >= _temp3200.last_plus_one){
_throw( Null_Exception);}* _temp3202;})); _temp3199;})};( struct _xenum_struct*)
_temp3198;}); break; case 95: _LL3197: yyval=({ struct Cyc_QualSpecList_tok_struct*
_temp3207=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));*
_temp3207=( struct Cyc_QualSpecList_tok_struct){.tag= Cyc_QualSpecList_tok_tag,.f1=({
struct _tuple13* _temp3208=( struct _tuple13*) GC_malloc( sizeof( struct
_tuple13)); _temp3208->f1= Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok(({
struct _tagged_ptr2 _temp3218= yyvs; struct _xenum_struct** _temp3220= _temp3218.curr
+( yyvsp_offset - 2); if( _temp3220 < _temp3218.base? 1: _temp3220 >= _temp3218.last_plus_one){
_throw( Null_Exception);}* _temp3220;})),(* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp3221= yyvs; struct _xenum_struct** _temp3223= _temp3221.curr +
yyvsp_offset; if( _temp3223 < _temp3221.base? 1: _temp3223 >= _temp3221.last_plus_one){
_throw( Null_Exception);}* _temp3223;}))).f1); _temp3208->f2=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3215= yyvs; struct _xenum_struct** _temp3217= _temp3215.curr
+ yyvsp_offset; if( _temp3217 < _temp3215.base? 1: _temp3217 >= _temp3215.last_plus_one){
_throw( Null_Exception);}* _temp3217;}))).f2; _temp3208->f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp3209= yyvs; struct _xenum_struct** _temp3211= _temp3209.curr
+( yyvsp_offset - 1); if( _temp3211 < _temp3209.base? 1: _temp3211 >= _temp3209.last_plus_one){
_throw( Null_Exception);}* _temp3211;})),(* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp3212= yyvs; struct _xenum_struct** _temp3214= _temp3212.curr +
yyvsp_offset; if( _temp3214 < _temp3212.base? 1: _temp3214 >= _temp3212.last_plus_one){
_throw( Null_Exception);}* _temp3214;}))).f3); _temp3208;})};( struct
_xenum_struct*) _temp3207;}); break; case 96: _LL3206: yyval=({ struct Cyc_DeclaratorList_tok_struct*
_temp3225=( struct Cyc_DeclaratorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorList_tok_struct));*
_temp3225=( struct Cyc_DeclaratorList_tok_struct){.tag= Cyc_DeclaratorList_tok_tag,.f1=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_DeclaratorList_tok(({
struct _tagged_ptr2 _temp3226= yyvs; struct _xenum_struct** _temp3228= _temp3226.curr
+ yyvsp_offset; if( _temp3228 < _temp3226.base? 1: _temp3228 >= _temp3226.last_plus_one){
_throw( Null_Exception);}* _temp3228;})))};( struct _xenum_struct*) _temp3225;});
break; case 97: _LL3224: yyval=({ struct Cyc_DeclaratorList_tok_struct*
_temp3230=( struct Cyc_DeclaratorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorList_tok_struct));*
_temp3230=( struct Cyc_DeclaratorList_tok_struct){.tag= Cyc_DeclaratorList_tok_tag,.f1=({
struct Cyc_List_List* _temp3231=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3231->hd=( void*) Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3232= yyvs; struct _xenum_struct** _temp3234= _temp3232.curr
+ yyvsp_offset; if( _temp3234 < _temp3232.base? 1: _temp3234 >= _temp3232.last_plus_one){
_throw( Null_Exception);}* _temp3234;})); _temp3231->tl= 0; _temp3231;})};(
struct _xenum_struct*) _temp3230;}); break; case 98: _LL3229: yyval=({ struct
Cyc_DeclaratorList_tok_struct* _temp3236=( struct Cyc_DeclaratorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorList_tok_struct));* _temp3236=( struct
Cyc_DeclaratorList_tok_struct){.tag= Cyc_DeclaratorList_tok_tag,.f1=({ struct
Cyc_List_List* _temp3237=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3237->hd=( void*) Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3241=
yyvs; struct _xenum_struct** _temp3243= _temp3241.curr + yyvsp_offset; if(
_temp3243 < _temp3241.base? 1: _temp3243 >= _temp3241.last_plus_one){ _throw(
Null_Exception);}* _temp3243;})); _temp3237->tl= Cyc_yyget_DeclaratorList_tok(({
struct _tagged_ptr2 _temp3238= yyvs; struct _xenum_struct** _temp3240= _temp3238.curr
+( yyvsp_offset - 2); if( _temp3240 < _temp3238.base? 1: _temp3240 >= _temp3238.last_plus_one){
_throw( Null_Exception);}* _temp3240;})); _temp3237;})};( struct _xenum_struct*)
_temp3236;}); break; case 99: _LL3235: yyval=({ struct _tagged_ptr2 _temp3245=
yyvs; struct _xenum_struct** _temp3247= _temp3245.curr + yyvsp_offset; if(
_temp3247 < _temp3245.base? 1: _temp3247 >= _temp3245.last_plus_one){ _throw(
Null_Exception);}* _temp3247;}); break; case 100: _LL3244: Cyc_Parse_unimp2((
struct _tagged_string)({ char* _temp3249=( char*)"bit fields"; struct
_tagged_string _temp3250; _temp3250.curr= _temp3249; _temp3250.base= _temp3249;
_temp3250.last_plus_one= _temp3249 + 11; _temp3250;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3251= yyls; struct Cyc_Yyltype* _temp3253= _temp3251.curr
+( yylsp_offset - 1); if( _temp3253 < _temp3251.base? 1: _temp3253 >= _temp3251.last_plus_one){
_throw( Null_Exception);}* _temp3253;}).first_line,({ struct _tagged_ptr3
_temp3254= yyls; struct Cyc_Yyltype* _temp3256= _temp3254.curr + yylsp_offset;
if( _temp3256 < _temp3254.base? 1: _temp3256 >= _temp3254.last_plus_one){ _throw(
Null_Exception);}* _temp3256;}).last_line)); yyval=({ struct Cyc_Declarator_tok_struct*
_temp3257=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));*
_temp3257=( struct Cyc_Declarator_tok_struct){.tag= Cyc_Declarator_tok_tag,.f1=({
struct Cyc_Parse_Declarator* _temp3258=( struct Cyc_Parse_Declarator*) GC_malloc(
sizeof( struct Cyc_Parse_Declarator)); _temp3258->id=({ struct _tuple1*
_temp3259=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp3259->f1=({
struct Cyc_Absyn_Rel_n_struct* _temp3263=( struct Cyc_Absyn_Rel_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct)); _temp3263->tag= Cyc_Absyn_Rel_n_tag;
_temp3263->f1= 0;( void*) _temp3263;}); _temp3259->f2=({ struct _tagged_string*
_temp3260=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp3260[ 0]=( struct _tagged_string)({ char* _temp3261=( char*)""; struct
_tagged_string _temp3262; _temp3262.curr= _temp3261; _temp3262.base= _temp3261;
_temp3262.last_plus_one= _temp3261 + 1; _temp3262;}); _temp3260;}); _temp3259;});
_temp3258->tms= 0; _temp3258;})};( struct _xenum_struct*) _temp3257;}); break;
case 101: _LL3248: Cyc_Parse_unimp2(( struct _tagged_string)({ char* _temp3265=(
char*)"bit fields"; struct _tagged_string _temp3266; _temp3266.curr= _temp3265;
_temp3266.base= _temp3265; _temp3266.last_plus_one= _temp3265 + 11; _temp3266;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3267= yyls; struct Cyc_Yyltype*
_temp3269= _temp3267.curr +( yylsp_offset - 2); if( _temp3269 < _temp3267.base?
1: _temp3269 >= _temp3267.last_plus_one){ _throw( Null_Exception);}* _temp3269;}).first_line,({
struct _tagged_ptr3 _temp3270= yyls; struct Cyc_Yyltype* _temp3272= _temp3270.curr
+( yylsp_offset - 1); if( _temp3272 < _temp3270.base? 1: _temp3272 >= _temp3270.last_plus_one){
_throw( Null_Exception);}* _temp3272;}).last_line)); yyval=({ struct
_tagged_ptr2 _temp3273= yyvs; struct _xenum_struct** _temp3275= _temp3273.curr +(
yyvsp_offset - 2); if( _temp3275 < _temp3273.base? 1: _temp3275 >= _temp3273.last_plus_one){
_throw( Null_Exception);}* _temp3275;}); break; case 102: _LL3264: yyval=({
struct Cyc_TypeSpecifier_tok_struct* _temp3277=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));* _temp3277=( struct
Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=({ struct Cyc_Parse_Decl_spec_struct*
_temp3278=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3278->tag= Cyc_Parse_Decl_spec_tag; _temp3278->f1= Cyc_Absyn_enum_decl( Cyc_Absyn_Public,
0, 0,({ struct Cyc_Core_Opt* _temp3279=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp3279->v=( void*) Cyc_yyget_EnumeratorFieldList_tok(({
struct _tagged_ptr2 _temp3280= yyvs; struct _xenum_struct** _temp3282= _temp3280.curr
+( yyvsp_offset - 1); if( _temp3282 < _temp3280.base? 1: _temp3282 >= _temp3280.last_plus_one){
_throw( Null_Exception);}* _temp3282;})); _temp3279;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3283= yyls; struct Cyc_Yyltype* _temp3285= _temp3283.curr
+( yylsp_offset - 3); if( _temp3285 < _temp3283.base? 1: _temp3285 >= _temp3283.last_plus_one){
_throw( Null_Exception);}* _temp3285;}).first_line,({ struct _tagged_ptr3
_temp3286= yyls; struct Cyc_Yyltype* _temp3288= _temp3286.curr + yylsp_offset;
if( _temp3288 < _temp3286.base? 1: _temp3288 >= _temp3286.last_plus_one){ _throw(
Null_Exception);}* _temp3288;}).last_line));( void*) _temp3278;})};( struct
_xenum_struct*) _temp3277;}); Cyc_Parse_unimp2(( struct _tagged_string)({ char*
_temp3289=( char*)"anonymous enums"; struct _tagged_string _temp3290; _temp3290.curr=
_temp3289; _temp3290.base= _temp3289; _temp3290.last_plus_one= _temp3289 + 16;
_temp3290;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3291= yyls;
struct Cyc_Yyltype* _temp3293= _temp3291.curr +( yylsp_offset - 3); if(
_temp3293 < _temp3291.base? 1: _temp3293 >= _temp3291.last_plus_one){ _throw(
Null_Exception);}* _temp3293;}).first_line,({ struct _tagged_ptr3 _temp3294=
yyls; struct Cyc_Yyltype* _temp3296= _temp3294.curr + yylsp_offset; if(
_temp3296 < _temp3294.base? 1: _temp3296 >= _temp3294.last_plus_one){ _throw(
Null_Exception);}* _temp3296;}).last_line)); break; case 103: _LL3276: { struct
Cyc_List_List* ts=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3298= yyls; struct Cyc_Yyltype* _temp3300= _temp3298.curr
+( yylsp_offset - 3); if( _temp3300 < _temp3298.base? 1: _temp3300 >= _temp3298.last_plus_one){
_throw( Null_Exception);}* _temp3300;}).first_line,({ struct _tagged_ptr3
_temp3301= yyls; struct Cyc_Yyltype* _temp3303= _temp3301.curr +( yylsp_offset -
3); if( _temp3303 < _temp3301.base? 1: _temp3303 >= _temp3301.last_plus_one){
_throw( Null_Exception);}* _temp3303;}).last_line), Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3304= yyvs; struct _xenum_struct** _temp3306= _temp3304.curr
+( yyvsp_offset - 3); if( _temp3306 < _temp3304.base? 1: _temp3306 >= _temp3304.last_plus_one){
_throw( Null_Exception);}* _temp3306;}))); yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp3307=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp3307=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=({
struct Cyc_Parse_Decl_spec_struct* _temp3308=( struct Cyc_Parse_Decl_spec_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct)); _temp3308->tag= Cyc_Parse_Decl_spec_tag;
_temp3308->f1= Cyc_Absyn_enum_decl( Cyc_Absyn_Public,({ struct Cyc_Core_Opt*
_temp3309=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3309->v=( void*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3310=
yyvs; struct _xenum_struct** _temp3312= _temp3310.curr +( yyvsp_offset - 4); if(
_temp3312 < _temp3310.base? 1: _temp3312 >= _temp3310.last_plus_one){ _throw(
Null_Exception);}* _temp3312;})); _temp3309;}), ts,({ struct Cyc_Core_Opt*
_temp3313=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3313->v=( void*) Cyc_yyget_EnumeratorFieldList_tok(({ struct _tagged_ptr2
_temp3314= yyvs; struct _xenum_struct** _temp3316= _temp3314.curr +(
yyvsp_offset - 1); if( _temp3316 < _temp3314.base? 1: _temp3316 >= _temp3314.last_plus_one){
_throw( Null_Exception);}* _temp3316;})); _temp3313;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3317= yyls; struct Cyc_Yyltype* _temp3319= _temp3317.curr
+( yylsp_offset - 5); if( _temp3319 < _temp3317.base? 1: _temp3319 >= _temp3317.last_plus_one){
_throw( Null_Exception);}* _temp3319;}).first_line,({ struct _tagged_ptr3
_temp3320= yyls; struct Cyc_Yyltype* _temp3322= _temp3320.curr + yylsp_offset;
if( _temp3322 < _temp3320.base? 1: _temp3322 >= _temp3320.last_plus_one){ _throw(
Null_Exception);}* _temp3322;}).last_line));( void*) _temp3308;})};( struct
_xenum_struct*) _temp3307;}); break;} case 104: _LL3297: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp3324=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp3324=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec(({ struct Cyc_Absyn_EnumType_struct* _temp3325=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp3325->tag= Cyc_Absyn_EnumType_tag;
_temp3325->f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3326= yyvs; struct _xenum_struct** _temp3328= _temp3326.curr +(
yyvsp_offset - 1); if( _temp3328 < _temp3326.base? 1: _temp3328 >= _temp3326.last_plus_one){
_throw( Null_Exception);}* _temp3328;})); _temp3325->f2= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3329= yyvs; struct _xenum_struct** _temp3331= _temp3329.curr
+ yyvsp_offset; if( _temp3331 < _temp3329.base? 1: _temp3331 >= _temp3329.last_plus_one){
_throw( Null_Exception);}* _temp3331;})); _temp3325->f3= 0;( void*) _temp3325;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3332= yyls; struct Cyc_Yyltype*
_temp3334= _temp3332.curr +( yylsp_offset - 2); if( _temp3334 < _temp3332.base?
1: _temp3334 >= _temp3332.last_plus_one){ _throw( Null_Exception);}* _temp3334;}).first_line,({
struct _tagged_ptr3 _temp3335= yyls; struct Cyc_Yyltype* _temp3337= _temp3335.curr
+ yylsp_offset; if( _temp3337 < _temp3335.base? 1: _temp3337 >= _temp3335.last_plus_one){
_throw( Null_Exception);}* _temp3337;}).last_line))};( struct _xenum_struct*)
_temp3324;}); break; case 105: _LL3323: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp3339=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp3339=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=({
struct Cyc_Parse_Decl_spec_struct* _temp3340=( struct Cyc_Parse_Decl_spec_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct)); _temp3340->tag= Cyc_Parse_Decl_spec_tag;
_temp3340->f1= Cyc_Absyn_xenum_decl( Cyc_Absyn_Public, Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3341= yyvs; struct _xenum_struct** _temp3343= _temp3341.curr
+( yyvsp_offset - 3); if( _temp3343 < _temp3341.base? 1: _temp3343 >= _temp3341.last_plus_one){
_throw( Null_Exception);}* _temp3343;})), Cyc_yyget_EnumeratorFieldList_tok(({
struct _tagged_ptr2 _temp3344= yyvs; struct _xenum_struct** _temp3346= _temp3344.curr
+( yyvsp_offset - 1); if( _temp3346 < _temp3344.base? 1: _temp3346 >= _temp3344.last_plus_one){
_throw( Null_Exception);}* _temp3346;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3347= yyls; struct Cyc_Yyltype* _temp3349= _temp3347.curr +(
yylsp_offset - 4); if( _temp3349 < _temp3347.base? 1: _temp3349 >= _temp3347.last_plus_one){
_throw( Null_Exception);}* _temp3349;}).first_line,({ struct _tagged_ptr3
_temp3350= yyls; struct Cyc_Yyltype* _temp3352= _temp3350.curr + yylsp_offset;
if( _temp3352 < _temp3350.base? 1: _temp3352 >= _temp3350.last_plus_one){ _throw(
Null_Exception);}* _temp3352;}).last_line));( void*) _temp3340;})};( struct
_xenum_struct*) _temp3339;}); break; case 106: _LL3338: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp3354=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp3354=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec(({ struct Cyc_Absyn_XenumType_struct* _temp3355=( struct Cyc_Absyn_XenumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XenumType_struct)); _temp3355->tag= Cyc_Absyn_XenumType_tag;
_temp3355->f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3356= yyvs;
struct _xenum_struct** _temp3358= _temp3356.curr + yyvsp_offset; if( _temp3358 <
_temp3356.base? 1: _temp3358 >= _temp3356.last_plus_one){ _throw( Null_Exception);}*
_temp3358;})); _temp3355->f2= 0;( void*) _temp3355;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3359= yyls; struct Cyc_Yyltype* _temp3361= _temp3359.curr
+( yylsp_offset - 1); if( _temp3361 < _temp3359.base? 1: _temp3361 >= _temp3359.last_plus_one){
_throw( Null_Exception);}* _temp3361;}).first_line,({ struct _tagged_ptr3
_temp3362= yyls; struct Cyc_Yyltype* _temp3364= _temp3362.curr + yylsp_offset;
if( _temp3364 < _temp3362.base? 1: _temp3364 >= _temp3362.last_plus_one){ _throw(
Null_Exception);}* _temp3364;}).last_line))};( struct _xenum_struct*) _temp3354;});
break; case 107: _LL3353: yyval=({ struct Cyc_EnumeratorFieldList_tok_struct*
_temp3366=( struct Cyc_EnumeratorFieldList_tok_struct*) GC_malloc( sizeof(
struct Cyc_EnumeratorFieldList_tok_struct));* _temp3366=( struct Cyc_EnumeratorFieldList_tok_struct){.tag=
Cyc_EnumeratorFieldList_tok_tag,.f1=({ struct Cyc_List_List* _temp3367=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3367->hd=( void*)
Cyc_yyget_EnumeratorField_tok(({ struct _tagged_ptr2 _temp3368= yyvs; struct
_xenum_struct** _temp3370= _temp3368.curr + yyvsp_offset; if( _temp3370 <
_temp3368.base? 1: _temp3370 >= _temp3368.last_plus_one){ _throw( Null_Exception);}*
_temp3370;})); _temp3367->tl= 0; _temp3367;})};( struct _xenum_struct*)
_temp3366;}); break; case 108: _LL3365: yyval=({ struct Cyc_EnumeratorFieldList_tok_struct*
_temp3372=( struct Cyc_EnumeratorFieldList_tok_struct*) GC_malloc( sizeof(
struct Cyc_EnumeratorFieldList_tok_struct));* _temp3372=( struct Cyc_EnumeratorFieldList_tok_struct){.tag=
Cyc_EnumeratorFieldList_tok_tag,.f1=({ struct Cyc_List_List* _temp3373=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3373->hd=( void*)
Cyc_yyget_EnumeratorField_tok(({ struct _tagged_ptr2 _temp3374= yyvs; struct
_xenum_struct** _temp3376= _temp3374.curr +( yyvsp_offset - 1); if( _temp3376 <
_temp3374.base? 1: _temp3376 >= _temp3374.last_plus_one){ _throw( Null_Exception);}*
_temp3376;})); _temp3373->tl= 0; _temp3373;})};( struct _xenum_struct*)
_temp3372;}); break; case 109: _LL3371: yyval=({ struct Cyc_EnumeratorFieldList_tok_struct*
_temp3378=( struct Cyc_EnumeratorFieldList_tok_struct*) GC_malloc( sizeof(
struct Cyc_EnumeratorFieldList_tok_struct));* _temp3378=( struct Cyc_EnumeratorFieldList_tok_struct){.tag=
Cyc_EnumeratorFieldList_tok_tag,.f1=({ struct Cyc_List_List* _temp3379=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3379->hd=( void*)
Cyc_yyget_EnumeratorField_tok(({ struct _tagged_ptr2 _temp3383= yyvs; struct
_xenum_struct** _temp3385= _temp3383.curr +( yyvsp_offset - 2); if( _temp3385 <
_temp3383.base? 1: _temp3385 >= _temp3383.last_plus_one){ _throw( Null_Exception);}*
_temp3385;})); _temp3379->tl= Cyc_yyget_EnumeratorFieldList_tok(({ struct
_tagged_ptr2 _temp3380= yyvs; struct _xenum_struct** _temp3382= _temp3380.curr +
yyvsp_offset; if( _temp3382 < _temp3380.base? 1: _temp3382 >= _temp3380.last_plus_one){
_throw( Null_Exception);}* _temp3382;})); _temp3379;})};( struct _xenum_struct*)
_temp3378;}); break; case 110: _LL3377: yyval=({ struct Cyc_EnumeratorFieldList_tok_struct*
_temp3387=( struct Cyc_EnumeratorFieldList_tok_struct*) GC_malloc( sizeof(
struct Cyc_EnumeratorFieldList_tok_struct));* _temp3387=( struct Cyc_EnumeratorFieldList_tok_struct){.tag=
Cyc_EnumeratorFieldList_tok_tag,.f1=({ struct Cyc_List_List* _temp3388=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3388->hd=( void*)
Cyc_yyget_EnumeratorField_tok(({ struct _tagged_ptr2 _temp3392= yyvs; struct
_xenum_struct** _temp3394= _temp3392.curr +( yyvsp_offset - 2); if( _temp3394 <
_temp3392.base? 1: _temp3394 >= _temp3392.last_plus_one){ _throw( Null_Exception);}*
_temp3394;})); _temp3388->tl= Cyc_yyget_EnumeratorFieldList_tok(({ struct
_tagged_ptr2 _temp3389= yyvs; struct _xenum_struct** _temp3391= _temp3389.curr +
yyvsp_offset; if( _temp3391 < _temp3389.base? 1: _temp3391 >= _temp3389.last_plus_one){
_throw( Null_Exception);}* _temp3391;})); _temp3388;})};( struct _xenum_struct*)
_temp3387;}); break; case 111: _LL3386: yyval=({ struct Cyc_EnumeratorField_tok_struct*
_temp3396=( struct Cyc_EnumeratorField_tok_struct*) GC_malloc( sizeof( struct
Cyc_EnumeratorField_tok_struct));* _temp3396=( struct Cyc_EnumeratorField_tok_struct){.tag=
Cyc_EnumeratorField_tok_tag,.f1=({ struct Cyc_Absyn_Enumfield* _temp3397=(
struct Cyc_Absyn_Enumfield*) GC_malloc( sizeof( struct Cyc_Absyn_Enumfield));
_temp3397->name= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3404= yyvs;
struct _xenum_struct** _temp3406= _temp3404.curr + yyvsp_offset; if( _temp3406 <
_temp3404.base? 1: _temp3406 >= _temp3404.last_plus_one){ _throw( Null_Exception);}*
_temp3406;})); _temp3397->tag= 0; _temp3397->tvs= 0; _temp3397->typs= 0;
_temp3397->loc= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3398=
yyls; struct Cyc_Yyltype* _temp3400= _temp3398.curr + yylsp_offset; if(
_temp3400 < _temp3398.base? 1: _temp3400 >= _temp3398.last_plus_one){ _throw(
Null_Exception);}* _temp3400;}).first_line,({ struct _tagged_ptr3 _temp3401=
yyls; struct Cyc_Yyltype* _temp3403= _temp3401.curr + yylsp_offset; if(
_temp3403 < _temp3401.base? 1: _temp3403 >= _temp3401.last_plus_one){ _throw(
Null_Exception);}* _temp3403;}).last_line); _temp3397;})};( struct _xenum_struct*)
_temp3396;}); break; case 112: _LL3395: yyval=({ struct Cyc_EnumeratorField_tok_struct*
_temp3408=( struct Cyc_EnumeratorField_tok_struct*) GC_malloc( sizeof( struct
Cyc_EnumeratorField_tok_struct));* _temp3408=( struct Cyc_EnumeratorField_tok_struct){.tag=
Cyc_EnumeratorField_tok_tag,.f1=({ struct Cyc_Absyn_Enumfield* _temp3409=(
struct Cyc_Absyn_Enumfield*) GC_malloc( sizeof( struct Cyc_Absyn_Enumfield));
_temp3409->name= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3419= yyvs;
struct _xenum_struct** _temp3421= _temp3419.curr +( yyvsp_offset - 2); if(
_temp3421 < _temp3419.base? 1: _temp3421 >= _temp3419.last_plus_one){ _throw(
Null_Exception);}* _temp3421;})); _temp3409->tag=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp3416= yyvs; struct _xenum_struct** _temp3418= _temp3416.curr
+ yyvsp_offset; if( _temp3418 < _temp3416.base? 1: _temp3418 >= _temp3416.last_plus_one){
_throw( Null_Exception);}* _temp3418;})); _temp3409->tvs= 0; _temp3409->typs= 0;
_temp3409->loc= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3410=
yyls; struct Cyc_Yyltype* _temp3412= _temp3410.curr +( yylsp_offset - 2); if(
_temp3412 < _temp3410.base? 1: _temp3412 >= _temp3410.last_plus_one){ _throw(
Null_Exception);}* _temp3412;}).first_line,({ struct _tagged_ptr3 _temp3413=
yyls; struct Cyc_Yyltype* _temp3415= _temp3413.curr + yylsp_offset; if(
_temp3415 < _temp3413.base? 1: _temp3415 >= _temp3413.last_plus_one){ _throw(
Null_Exception);}* _temp3415;}).last_line); _temp3409;})};( struct _xenum_struct*)
_temp3408;}); break; case 113: _LL3407: { struct Cyc_List_List* typs=(( struct
Cyc_List_List*(*)( struct _tuple9*(* f)( struct Cyc_Position_Segment*, struct
_tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp3423= yyls; struct Cyc_Yyltype* _temp3425= _temp3423.curr +( yylsp_offset -
1); if( _temp3425 < _temp3423.base? 1: _temp3425 >= _temp3423.last_plus_one){
_throw( Null_Exception);}* _temp3425;}).first_line,({ struct _tagged_ptr3
_temp3426= yyls; struct Cyc_Yyltype* _temp3428= _temp3426.curr +( yylsp_offset -
1); if( _temp3428 < _temp3426.base? 1: _temp3428 >= _temp3426.last_plus_one){
_throw( Null_Exception);}* _temp3428;}).last_line),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp3429= yyvs; struct _xenum_struct** _temp3431= _temp3429.curr
+( yyvsp_offset - 1); if( _temp3431 < _temp3429.base? 1: _temp3431 >= _temp3429.last_plus_one){
_throw( Null_Exception);}* _temp3431;})))); struct Cyc_List_List* tvs=(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3432=
yyls; struct Cyc_Yyltype* _temp3434= _temp3432.curr +( yylsp_offset - 3); if(
_temp3434 < _temp3432.base? 1: _temp3434 >= _temp3432.last_plus_one){ _throw(
Null_Exception);}* _temp3434;}).first_line,({ struct _tagged_ptr3 _temp3435=
yyls; struct Cyc_Yyltype* _temp3437= _temp3435.curr +( yylsp_offset - 3); if(
_temp3437 < _temp3435.base? 1: _temp3437 >= _temp3435.last_plus_one){ _throw(
Null_Exception);}* _temp3437;}).last_line), Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp3438= yyvs; struct _xenum_struct** _temp3440= _temp3438.curr +(
yyvsp_offset - 3); if( _temp3440 < _temp3438.base? 1: _temp3440 >= _temp3438.last_plus_one){
_throw( Null_Exception);}* _temp3440;}))); yyval=({ struct Cyc_EnumeratorField_tok_struct*
_temp3441=( struct Cyc_EnumeratorField_tok_struct*) GC_malloc( sizeof( struct
Cyc_EnumeratorField_tok_struct));* _temp3441=( struct Cyc_EnumeratorField_tok_struct){.tag=
Cyc_EnumeratorField_tok_tag,.f1=({ struct Cyc_Absyn_Enumfield* _temp3442=(
struct Cyc_Absyn_Enumfield*) GC_malloc( sizeof( struct Cyc_Absyn_Enumfield));
_temp3442->name= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3449= yyvs;
struct _xenum_struct** _temp3451= _temp3449.curr +( yyvsp_offset - 4); if(
_temp3451 < _temp3449.base? 1: _temp3451 >= _temp3449.last_plus_one){ _throw(
Null_Exception);}* _temp3451;})); _temp3442->tag= 0; _temp3442->tvs= tvs;
_temp3442->typs= typs; _temp3442->loc= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3443= yyls; struct Cyc_Yyltype* _temp3445= _temp3443.curr +(
yylsp_offset - 4); if( _temp3445 < _temp3443.base? 1: _temp3445 >= _temp3443.last_plus_one){
_throw( Null_Exception);}* _temp3445;}).first_line,({ struct _tagged_ptr3
_temp3446= yyls; struct Cyc_Yyltype* _temp3448= _temp3446.curr + yylsp_offset;
if( _temp3448 < _temp3446.base? 1: _temp3448 >= _temp3446.last_plus_one){ _throw(
Null_Exception);}* _temp3448;}).last_line); _temp3442;})};( struct _xenum_struct*)
_temp3441;}); break;} case 114: _LL3422: yyval=({ struct _tagged_ptr2 _temp3453=
yyvs; struct _xenum_struct** _temp3455= _temp3453.curr + yyvsp_offset; if(
_temp3455 < _temp3453.base? 1: _temp3455 >= _temp3453.last_plus_one){ _throw(
Null_Exception);}* _temp3455;}); break; case 115: _LL3452: yyval=({ struct Cyc_Declarator_tok_struct*
_temp3457=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));*
_temp3457=( struct Cyc_Declarator_tok_struct){.tag= Cyc_Declarator_tok_tag,.f1=({
struct Cyc_Parse_Declarator* _temp3458=( struct Cyc_Parse_Declarator*) GC_malloc(
sizeof( struct Cyc_Parse_Declarator)); _temp3458->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3465= yyvs; struct _xenum_struct** _temp3467= _temp3465.curr
+ yyvsp_offset; if( _temp3467 < _temp3465.base? 1: _temp3467 >= _temp3465.last_plus_one){
_throw( Null_Exception);}* _temp3467;})))->id; _temp3458->tms=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_TypeModifierList_tok(({
struct _tagged_ptr2 _temp3459= yyvs; struct _xenum_struct** _temp3461= _temp3459.curr
+( yyvsp_offset - 1); if( _temp3461 < _temp3459.base? 1: _temp3461 >= _temp3459.last_plus_one){
_throw( Null_Exception);}* _temp3461;})),( Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp3462= yyvs; struct _xenum_struct** _temp3464= _temp3462.curr +
yyvsp_offset; if( _temp3464 < _temp3462.base? 1: _temp3464 >= _temp3462.last_plus_one){
_throw( Null_Exception);}* _temp3464;})))->tms); _temp3458;})};( struct
_xenum_struct*) _temp3457;}); break; case 116: _LL3456: yyval=({ struct Cyc_Declarator_tok_struct*
_temp3469=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));*
_temp3469=( struct Cyc_Declarator_tok_struct){.tag= Cyc_Declarator_tok_tag,.f1=({
struct Cyc_Parse_Declarator* _temp3470=( struct Cyc_Parse_Declarator*) GC_malloc(
sizeof( struct Cyc_Parse_Declarator)); _temp3470->id= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3471= yyvs; struct _xenum_struct** _temp3473= _temp3471.curr
+ yyvsp_offset; if( _temp3473 < _temp3471.base? 1: _temp3473 >= _temp3471.last_plus_one){
_throw( Null_Exception);}* _temp3473;})); _temp3470->tms= 0; _temp3470;})};(
struct _xenum_struct*) _temp3469;}); break; case 117: _LL3468: yyval=({ struct
_tagged_ptr2 _temp3475= yyvs; struct _xenum_struct** _temp3477= _temp3475.curr +(
yyvsp_offset - 1); if( _temp3477 < _temp3475.base? 1: _temp3477 >= _temp3475.last_plus_one){
_throw( Null_Exception);}* _temp3477;}); break; case 118: _LL3474: yyval=({
struct Cyc_Declarator_tok_struct* _temp3479=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));* _temp3479=( struct Cyc_Declarator_tok_struct){.tag=
Cyc_Declarator_tok_tag,.f1=({ struct Cyc_Parse_Declarator* _temp3480=( struct
Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp3480->id=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3485= yyvs;
struct _xenum_struct** _temp3487= _temp3485.curr +( yyvsp_offset - 2); if(
_temp3487 < _temp3485.base? 1: _temp3487 >= _temp3485.last_plus_one){ _throw(
Null_Exception);}* _temp3487;})))->id; _temp3480->tms=({ struct Cyc_List_List*
_temp3481=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3481->hd=( void*) Cyc_Absyn_Carray_mod; _temp3481->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3482= yyvs; struct _xenum_struct** _temp3484= _temp3482.curr
+( yyvsp_offset - 2); if( _temp3484 < _temp3482.base? 1: _temp3484 >= _temp3482.last_plus_one){
_throw( Null_Exception);}* _temp3484;})))->tms; _temp3481;}); _temp3480;})};(
struct _xenum_struct*) _temp3479;}); break; case 119: _LL3478: yyval=({ struct
Cyc_Declarator_tok_struct* _temp3489=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));* _temp3489=( struct Cyc_Declarator_tok_struct){.tag=
Cyc_Declarator_tok_tag,.f1=({ struct Cyc_Parse_Declarator* _temp3490=( struct
Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp3490->id=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3499= yyvs;
struct _xenum_struct** _temp3501= _temp3499.curr +( yyvsp_offset - 3); if(
_temp3501 < _temp3499.base? 1: _temp3501 >= _temp3499.last_plus_one){ _throw(
Null_Exception);}* _temp3501;})))->id; _temp3490->tms=({ struct Cyc_List_List*
_temp3491=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3491->hd=( void*)({ struct Cyc_Absyn_ConstArray_mod_struct* _temp3495=(
struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct));
_temp3495->tag= Cyc_Absyn_ConstArray_mod_tag; _temp3495->f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp3496= yyvs; struct _xenum_struct** _temp3498= _temp3496.curr
+( yyvsp_offset - 1); if( _temp3498 < _temp3496.base? 1: _temp3498 >= _temp3496.last_plus_one){
_throw( Null_Exception);}* _temp3498;}));( void*) _temp3495;}); _temp3491->tl=(
Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3492= yyvs; struct
_xenum_struct** _temp3494= _temp3492.curr +( yyvsp_offset - 3); if( _temp3494 <
_temp3492.base? 1: _temp3494 >= _temp3492.last_plus_one){ _throw( Null_Exception);}*
_temp3494;})))->tms; _temp3491;}); _temp3490;})};( struct _xenum_struct*)
_temp3489;}); break; case 120: _LL3488: { struct _tuple14 _temp3508; struct Cyc_Core_Opt*
_temp3509; int _temp3511; struct Cyc_List_List* _temp3513; struct _tuple14*
_temp3506= Cyc_yyget_ParamDeclListBool_tok(({ struct _tagged_ptr2 _temp3503=
yyvs; struct _xenum_struct** _temp3505= _temp3503.curr +( yyvsp_offset - 1); if(
_temp3505 < _temp3503.base? 1: _temp3505 >= _temp3503.last_plus_one){ _throw(
Null_Exception);}* _temp3505;})); _temp3508=* _temp3506; _LL3514: _temp3513=
_temp3508.f1; goto _LL3512; _LL3512: _temp3511= _temp3508.f2; goto _LL3510;
_LL3510: _temp3509= _temp3508.f3; goto _LL3507; _LL3507: yyval=({ struct Cyc_Declarator_tok_struct*
_temp3515=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));*
_temp3515=( struct Cyc_Declarator_tok_struct){.tag= Cyc_Declarator_tok_tag,.f1=({
struct Cyc_Parse_Declarator* _temp3516=( struct Cyc_Parse_Declarator*) GC_malloc(
sizeof( struct Cyc_Parse_Declarator)); _temp3516->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3523= yyvs; struct _xenum_struct** _temp3525= _temp3523.curr
+( yyvsp_offset - 3); if( _temp3525 < _temp3523.base? 1: _temp3525 >= _temp3523.last_plus_one){
_throw( Null_Exception);}* _temp3525;})))->id; _temp3516->tms=({ struct Cyc_List_List*
_temp3517=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3517->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct* _temp3521=(
struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3521->tag= Cyc_Absyn_Function_mod_tag; _temp3521->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp3522=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3522->tag= Cyc_Absyn_WithTypes_tag; _temp3522->f1= _temp3513; _temp3522->f2=
_temp3511; _temp3522->f3= _temp3509;( void*) _temp3522;});( void*) _temp3521;});
_temp3517->tl=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3518= yyvs;
struct _xenum_struct** _temp3520= _temp3518.curr +( yyvsp_offset - 3); if(
_temp3520 < _temp3518.base? 1: _temp3520 >= _temp3518.last_plus_one){ _throw(
Null_Exception);}* _temp3520;})))->tms; _temp3517;}); _temp3516;})};( struct
_xenum_struct*) _temp3515;}); break;} case 121: _LL3502: yyval=({ struct Cyc_Declarator_tok_struct*
_temp3527=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));*
_temp3527=( struct Cyc_Declarator_tok_struct){.tag= Cyc_Declarator_tok_tag,.f1=({
struct Cyc_Parse_Declarator* _temp3528=( struct Cyc_Parse_Declarator*) GC_malloc(
sizeof( struct Cyc_Parse_Declarator)); _temp3528->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3535= yyvs; struct _xenum_struct** _temp3537= _temp3535.curr
+( yyvsp_offset - 2); if( _temp3537 < _temp3535.base? 1: _temp3537 >= _temp3535.last_plus_one){
_throw( Null_Exception);}* _temp3537;})))->id; _temp3528->tms=({ struct Cyc_List_List*
_temp3529=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3529->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct* _temp3533=(
struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3533->tag= Cyc_Absyn_Function_mod_tag; _temp3533->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp3534=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3534->tag= Cyc_Absyn_WithTypes_tag; _temp3534->f1= 0; _temp3534->f2= 0;
_temp3534->f3= 0;( void*) _temp3534;});( void*) _temp3533;}); _temp3529->tl=(
Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3530= yyvs; struct
_xenum_struct** _temp3532= _temp3530.curr +( yyvsp_offset - 2); if( _temp3532 <
_temp3530.base? 1: _temp3532 >= _temp3530.last_plus_one){ _throw( Null_Exception);}*
_temp3532;})))->tms; _temp3529;}); _temp3528;})};( struct _xenum_struct*)
_temp3527;}); break; case 122: _LL3526: yyval=({ struct Cyc_Declarator_tok_struct*
_temp3539=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));*
_temp3539=( struct Cyc_Declarator_tok_struct){.tag= Cyc_Declarator_tok_tag,.f1=({
struct Cyc_Parse_Declarator* _temp3540=( struct Cyc_Parse_Declarator*) GC_malloc(
sizeof( struct Cyc_Parse_Declarator)); _temp3540->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3552= yyvs; struct _xenum_struct** _temp3554= _temp3552.curr
+( yyvsp_offset - 4); if( _temp3554 < _temp3552.base? 1: _temp3554 >= _temp3552.last_plus_one){
_throw( Null_Exception);}* _temp3554;})))->id; _temp3540->tms=({ struct Cyc_List_List*
_temp3541=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3541->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct* _temp3545=(
struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3545->tag= Cyc_Absyn_Function_mod_tag; _temp3545->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp3546=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3546->tag= Cyc_Absyn_WithTypes_tag; _temp3546->f1= 0; _temp3546->f2= 0;
_temp3546->f3=({ struct Cyc_Core_Opt* _temp3547=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3547->v=( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp3548=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3548->tag= Cyc_Absyn_JoinEff_tag; _temp3548->f1= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3549= yyvs; struct _xenum_struct** _temp3551= _temp3549.curr
+( yyvsp_offset - 1); if( _temp3551 < _temp3549.base? 1: _temp3551 >= _temp3549.last_plus_one){
_throw( Null_Exception);}* _temp3551;}));( void*) _temp3548;}); _temp3547;});(
void*) _temp3546;});( void*) _temp3545;}); _temp3541->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3542= yyvs; struct _xenum_struct** _temp3544= _temp3542.curr
+( yyvsp_offset - 4); if( _temp3544 < _temp3542.base? 1: _temp3544 >= _temp3542.last_plus_one){
_throw( Null_Exception);}* _temp3544;})))->tms; _temp3541;}); _temp3540;})};(
struct _xenum_struct*) _temp3539;}); break; case 123: _LL3538: yyval=({ struct
Cyc_Declarator_tok_struct* _temp3556=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));* _temp3556=( struct Cyc_Declarator_tok_struct){.tag=
Cyc_Declarator_tok_tag,.f1=({ struct Cyc_Parse_Declarator* _temp3557=( struct
Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp3557->id=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3573= yyvs;
struct _xenum_struct** _temp3575= _temp3573.curr +( yyvsp_offset - 3); if(
_temp3575 < _temp3573.base? 1: _temp3575 >= _temp3573.last_plus_one){ _throw(
Null_Exception);}* _temp3575;})))->id; _temp3557->tms=({ struct Cyc_List_List*
_temp3558=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3558->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct* _temp3562=(
struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3562->tag= Cyc_Absyn_Function_mod_tag; _temp3562->f1=( void*)({ struct Cyc_Absyn_NoTypes_struct*
_temp3563=( struct Cyc_Absyn_NoTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_NoTypes_struct));
_temp3563->tag= Cyc_Absyn_NoTypes_tag; _temp3563->f1= Cyc_yyget_IdList_tok(({
struct _tagged_ptr2 _temp3564= yyvs; struct _xenum_struct** _temp3566= _temp3564.curr
+( yyvsp_offset - 1); if( _temp3566 < _temp3564.base? 1: _temp3566 >= _temp3564.last_plus_one){
_throw( Null_Exception);}* _temp3566;})); _temp3563->f2= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3567= yyls; struct Cyc_Yyltype* _temp3569= _temp3567.curr
+( yylsp_offset - 3); if( _temp3569 < _temp3567.base? 1: _temp3569 >= _temp3567.last_plus_one){
_throw( Null_Exception);}* _temp3569;}).first_line,({ struct _tagged_ptr3
_temp3570= yyls; struct Cyc_Yyltype* _temp3572= _temp3570.curr + yylsp_offset;
if( _temp3572 < _temp3570.base? 1: _temp3572 >= _temp3570.last_plus_one){ _throw(
Null_Exception);}* _temp3572;}).last_line);( void*) _temp3563;});( void*)
_temp3562;}); _temp3558->tl=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2
_temp3559= yyvs; struct _xenum_struct** _temp3561= _temp3559.curr +(
yyvsp_offset - 3); if( _temp3561 < _temp3559.base? 1: _temp3561 >= _temp3559.last_plus_one){
_throw( Null_Exception);}* _temp3561;})))->tms; _temp3558;}); _temp3557;})};(
struct _xenum_struct*) _temp3556;}); break; case 124: _LL3555: { struct Cyc_List_List*
ts=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3577=
yyls; struct Cyc_Yyltype* _temp3579= _temp3577.curr +( yylsp_offset - 2); if(
_temp3579 < _temp3577.base? 1: _temp3579 >= _temp3577.last_plus_one){ _throw(
Null_Exception);}* _temp3579;}).first_line,({ struct _tagged_ptr3 _temp3580=
yyls; struct Cyc_Yyltype* _temp3582= _temp3580.curr + yylsp_offset; if(
_temp3582 < _temp3580.base? 1: _temp3582 >= _temp3580.last_plus_one){ _throw(
Null_Exception);}* _temp3582;}).last_line),(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp3583= yyvs; struct _xenum_struct** _temp3585= _temp3583.curr +(
yyvsp_offset - 1); if( _temp3585 < _temp3583.base? 1: _temp3585 >= _temp3583.last_plus_one){
_throw( Null_Exception);}* _temp3585;})))); yyval=({ struct Cyc_Declarator_tok_struct*
_temp3586=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));*
_temp3586=( struct Cyc_Declarator_tok_struct){.tag= Cyc_Declarator_tok_tag,.f1=({
struct Cyc_Parse_Declarator* _temp3587=( struct Cyc_Parse_Declarator*) GC_malloc(
sizeof( struct Cyc_Parse_Declarator)); _temp3587->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3599= yyvs; struct _xenum_struct** _temp3601= _temp3599.curr
+( yyvsp_offset - 3); if( _temp3601 < _temp3599.base? 1: _temp3601 >= _temp3599.last_plus_one){
_throw( Null_Exception);}* _temp3601;})))->id; _temp3587->tms=({ struct Cyc_List_List*
_temp3588=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3588->hd=( void*)({ struct Cyc_Absyn_TypeParams_mod_struct* _temp3592=(
struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct));
_temp3592->tag= Cyc_Absyn_TypeParams_mod_tag; _temp3592->f1= ts; _temp3592->f2=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3593= yyls; struct Cyc_Yyltype*
_temp3595= _temp3593.curr +( yylsp_offset - 3); if( _temp3595 < _temp3593.base?
1: _temp3595 >= _temp3593.last_plus_one){ _throw( Null_Exception);}* _temp3595;}).first_line,({
struct _tagged_ptr3 _temp3596= yyls; struct Cyc_Yyltype* _temp3598= _temp3596.curr
+ yylsp_offset; if( _temp3598 < _temp3596.base? 1: _temp3598 >= _temp3596.last_plus_one){
_throw( Null_Exception);}* _temp3598;}).last_line); _temp3592->f3= 0;( void*)
_temp3592;}); _temp3588->tl=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2
_temp3589= yyvs; struct _xenum_struct** _temp3591= _temp3589.curr +(
yyvsp_offset - 3); if( _temp3591 < _temp3589.base? 1: _temp3591 >= _temp3589.last_plus_one){
_throw( Null_Exception);}* _temp3591;})))->tms; _temp3588;}); _temp3587;})};(
struct _xenum_struct*) _temp3586;}); break;} case 125: _LL3576:(( struct Cyc_Lexing_lexbuf*)
Cyc_Parse_lbuf->v)->lex_curr_pos -= 1;{ struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3603= yyls; struct Cyc_Yyltype* _temp3605= _temp3603.curr
+( yylsp_offset - 2); if( _temp3605 < _temp3603.base? 1: _temp3605 >= _temp3603.last_plus_one){
_throw( Null_Exception);}* _temp3605;}).first_line,({ struct _tagged_ptr3
_temp3606= yyls; struct Cyc_Yyltype* _temp3608= _temp3606.curr + yylsp_offset;
if( _temp3608 < _temp3606.base? 1: _temp3608 >= _temp3606.last_plus_one){ _throw(
Null_Exception);}* _temp3608;}).last_line),(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp3609= yyvs; struct _xenum_struct** _temp3611= _temp3609.curr +(
yyvsp_offset - 1); if( _temp3611 < _temp3609.base? 1: _temp3611 >= _temp3609.last_plus_one){
_throw( Null_Exception);}* _temp3611;})))); yyval=({ struct Cyc_Declarator_tok_struct*
_temp3612=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));*
_temp3612=( struct Cyc_Declarator_tok_struct){.tag= Cyc_Declarator_tok_tag,.f1=({
struct Cyc_Parse_Declarator* _temp3613=( struct Cyc_Parse_Declarator*) GC_malloc(
sizeof( struct Cyc_Parse_Declarator)); _temp3613->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3625= yyvs; struct _xenum_struct** _temp3627= _temp3625.curr
+( yyvsp_offset - 3); if( _temp3627 < _temp3625.base? 1: _temp3627 >= _temp3625.last_plus_one){
_throw( Null_Exception);}* _temp3627;})))->id; _temp3613->tms=({ struct Cyc_List_List*
_temp3614=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3614->hd=( void*)({ struct Cyc_Absyn_TypeParams_mod_struct* _temp3618=(
struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct));
_temp3618->tag= Cyc_Absyn_TypeParams_mod_tag; _temp3618->f1= ts; _temp3618->f2=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3619= yyls; struct Cyc_Yyltype*
_temp3621= _temp3619.curr +( yylsp_offset - 3); if( _temp3621 < _temp3619.base?
1: _temp3621 >= _temp3619.last_plus_one){ _throw( Null_Exception);}* _temp3621;}).first_line,({
struct _tagged_ptr3 _temp3622= yyls; struct Cyc_Yyltype* _temp3624= _temp3622.curr
+ yylsp_offset; if( _temp3624 < _temp3622.base? 1: _temp3624 >= _temp3622.last_plus_one){
_throw( Null_Exception);}* _temp3624;}).last_line); _temp3618->f3= 0;( void*)
_temp3618;}); _temp3614->tl=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2
_temp3615= yyvs; struct _xenum_struct** _temp3617= _temp3615.curr +(
yyvsp_offset - 3); if( _temp3617 < _temp3615.base? 1: _temp3617 >= _temp3615.last_plus_one){
_throw( Null_Exception);}* _temp3617;})))->tms; _temp3614;}); _temp3613;})};(
struct _xenum_struct*) _temp3612;}); break;} case 126: _LL3602: yyval=({ struct
Cyc_Declarator_tok_struct* _temp3629=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));* _temp3629=( struct Cyc_Declarator_tok_struct){.tag=
Cyc_Declarator_tok_tag,.f1=({ struct Cyc_Parse_Declarator* _temp3630=( struct
Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp3630->id=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3645= yyvs;
struct _xenum_struct** _temp3647= _temp3645.curr +( yyvsp_offset - 1); if(
_temp3647 < _temp3645.base? 1: _temp3647 >= _temp3645.last_plus_one){ _throw(
Null_Exception);}* _temp3647;})))->id; _temp3630->tms=({ struct Cyc_List_List*
_temp3631=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3631->hd=( void*)({ struct Cyc_Absyn_Attributes_mod_struct* _temp3635=(
struct Cyc_Absyn_Attributes_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct));
_temp3635->tag= Cyc_Absyn_Attributes_mod_tag; _temp3635->f1= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3636= yyls; struct Cyc_Yyltype* _temp3638= _temp3636.curr
+ yylsp_offset; if( _temp3638 < _temp3636.base? 1: _temp3638 >= _temp3636.last_plus_one){
_throw( Null_Exception);}* _temp3638;}).first_line,({ struct _tagged_ptr3
_temp3639= yyls; struct Cyc_Yyltype* _temp3641= _temp3639.curr + yylsp_offset;
if( _temp3641 < _temp3639.base? 1: _temp3641 >= _temp3639.last_plus_one){ _throw(
Null_Exception);}* _temp3641;}).last_line); _temp3635->f2= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp3642= yyvs; struct _xenum_struct** _temp3644= _temp3642.curr
+ yyvsp_offset; if( _temp3644 < _temp3642.base? 1: _temp3644 >= _temp3642.last_plus_one){
_throw( Null_Exception);}* _temp3644;}));( void*) _temp3635;}); _temp3631->tl=(
Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3632= yyvs; struct
_xenum_struct** _temp3634= _temp3632.curr +( yyvsp_offset - 1); if( _temp3634 <
_temp3632.base? 1: _temp3634 >= _temp3632.last_plus_one){ _throw( Null_Exception);}*
_temp3634;})))->tms; _temp3631;}); _temp3630;})};( struct _xenum_struct*)
_temp3629;}); break; case 127: _LL3628: yyval=({ struct Cyc_TypeModifierList_tok_struct*
_temp3649=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct));* _temp3649=( struct Cyc_TypeModifierList_tok_struct){.tag=
Cyc_TypeModifierList_tok_tag,.f1=({ struct Cyc_List_List* _temp3650=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3650->hd=( void*)({ struct Cyc_Absyn_Pointer_mod_struct*
_temp3651=( struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp3651->tag= Cyc_Absyn_Pointer_mod_tag; _temp3651->f1=( void*) Cyc_yyget_Pointer_Sort_tok(({
struct _tagged_ptr2 _temp3652= yyvs; struct _xenum_struct** _temp3654= _temp3652.curr
+( yyvsp_offset - 1); if( _temp3654 < _temp3652.base? 1: _temp3654 >= _temp3652.last_plus_one){
_throw( Null_Exception);}* _temp3654;})); _temp3651->f2=( void*) Cyc_yyget_Rgn_tok(({
struct _tagged_ptr2 _temp3655= yyvs; struct _xenum_struct** _temp3657= _temp3655.curr
+ yyvsp_offset; if( _temp3657 < _temp3655.base? 1: _temp3657 >= _temp3655.last_plus_one){
_throw( Null_Exception);}* _temp3657;})); _temp3651->f3= Cyc_Absyn_empty_tqual();(
void*) _temp3651;}); _temp3650->tl= 0; _temp3650;})};( struct _xenum_struct*)
_temp3649;}); break; case 128: _LL3648: yyval=({ struct Cyc_TypeModifierList_tok_struct*
_temp3659=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct));* _temp3659=( struct Cyc_TypeModifierList_tok_struct){.tag=
Cyc_TypeModifierList_tok_tag,.f1=({ struct Cyc_List_List* _temp3660=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3660->hd=( void*)({ struct Cyc_Absyn_Pointer_mod_struct*
_temp3661=( struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp3661->tag= Cyc_Absyn_Pointer_mod_tag; _temp3661->f1=( void*) Cyc_yyget_Pointer_Sort_tok(({
struct _tagged_ptr2 _temp3662= yyvs; struct _xenum_struct** _temp3664= _temp3662.curr
+( yyvsp_offset - 2); if( _temp3664 < _temp3662.base? 1: _temp3664 >= _temp3662.last_plus_one){
_throw( Null_Exception);}* _temp3664;})); _temp3661->f2=( void*) Cyc_yyget_Rgn_tok(({
struct _tagged_ptr2 _temp3665= yyvs; struct _xenum_struct** _temp3667= _temp3665.curr
+( yyvsp_offset - 1); if( _temp3667 < _temp3665.base? 1: _temp3667 >= _temp3665.last_plus_one){
_throw( Null_Exception);}* _temp3667;})); _temp3661->f3= Cyc_yyget_TypeQual_tok(({
struct _tagged_ptr2 _temp3668= yyvs; struct _xenum_struct** _temp3670= _temp3668.curr
+ yyvsp_offset; if( _temp3670 < _temp3668.base? 1: _temp3670 >= _temp3668.last_plus_one){
_throw( Null_Exception);}* _temp3670;}));( void*) _temp3661;}); _temp3660->tl= 0;
_temp3660;})};( struct _xenum_struct*) _temp3659;}); break; case 129: _LL3658:
yyval=({ struct Cyc_TypeModifierList_tok_struct* _temp3672=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct));* _temp3672=( struct
Cyc_TypeModifierList_tok_struct){.tag= Cyc_TypeModifierList_tok_tag,.f1=({
struct Cyc_List_List* _temp3673=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3673->hd=( void*)({ struct Cyc_Absyn_Pointer_mod_struct*
_temp3677=( struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp3677->tag= Cyc_Absyn_Pointer_mod_tag; _temp3677->f1=( void*) Cyc_yyget_Pointer_Sort_tok(({
struct _tagged_ptr2 _temp3678= yyvs; struct _xenum_struct** _temp3680= _temp3678.curr
+( yyvsp_offset - 2); if( _temp3680 < _temp3678.base? 1: _temp3680 >= _temp3678.last_plus_one){
_throw( Null_Exception);}* _temp3680;})); _temp3677->f2=( void*) Cyc_yyget_Rgn_tok(({
struct _tagged_ptr2 _temp3681= yyvs; struct _xenum_struct** _temp3683= _temp3681.curr
+( yyvsp_offset - 1); if( _temp3683 < _temp3681.base? 1: _temp3683 >= _temp3681.last_plus_one){
_throw( Null_Exception);}* _temp3683;})); _temp3677->f3= Cyc_Absyn_empty_tqual();(
void*) _temp3677;}); _temp3673->tl= Cyc_yyget_TypeModifierList_tok(({ struct
_tagged_ptr2 _temp3674= yyvs; struct _xenum_struct** _temp3676= _temp3674.curr +
yyvsp_offset; if( _temp3676 < _temp3674.base? 1: _temp3676 >= _temp3674.last_plus_one){
_throw( Null_Exception);}* _temp3676;})); _temp3673;})};( struct _xenum_struct*)
_temp3672;}); break; case 130: _LL3671: yyval=({ struct Cyc_TypeModifierList_tok_struct*
_temp3685=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct));* _temp3685=( struct Cyc_TypeModifierList_tok_struct){.tag=
Cyc_TypeModifierList_tok_tag,.f1=({ struct Cyc_List_List* _temp3686=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3686->hd=( void*)({ struct Cyc_Absyn_Pointer_mod_struct*
_temp3690=( struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp3690->tag= Cyc_Absyn_Pointer_mod_tag; _temp3690->f1=( void*) Cyc_yyget_Pointer_Sort_tok(({
struct _tagged_ptr2 _temp3691= yyvs; struct _xenum_struct** _temp3693= _temp3691.curr
+( yyvsp_offset - 3); if( _temp3693 < _temp3691.base? 1: _temp3693 >= _temp3691.last_plus_one){
_throw( Null_Exception);}* _temp3693;})); _temp3690->f2=( void*) Cyc_yyget_Rgn_tok(({
struct _tagged_ptr2 _temp3694= yyvs; struct _xenum_struct** _temp3696= _temp3694.curr
+( yyvsp_offset - 2); if( _temp3696 < _temp3694.base? 1: _temp3696 >= _temp3694.last_plus_one){
_throw( Null_Exception);}* _temp3696;})); _temp3690->f3= Cyc_yyget_TypeQual_tok(({
struct _tagged_ptr2 _temp3697= yyvs; struct _xenum_struct** _temp3699= _temp3697.curr
+( yyvsp_offset - 1); if( _temp3699 < _temp3697.base? 1: _temp3699 >= _temp3697.last_plus_one){
_throw( Null_Exception);}* _temp3699;}));( void*) _temp3690;}); _temp3686->tl=
Cyc_yyget_TypeModifierList_tok(({ struct _tagged_ptr2 _temp3687= yyvs; struct
_xenum_struct** _temp3689= _temp3687.curr + yyvsp_offset; if( _temp3689 <
_temp3687.base? 1: _temp3689 >= _temp3687.last_plus_one){ _throw( Null_Exception);}*
_temp3689;})); _temp3686;})};( struct _xenum_struct*) _temp3685;}); break; case
131: _LL3684: yyval=({ struct Cyc_Pointer_Sort_tok_struct* _temp3701=( struct
Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));*
_temp3701=( struct Cyc_Pointer_Sort_tok_struct){.tag= Cyc_Pointer_Sort_tok_tag,.f1=({
struct Cyc_Absyn_Nullable_ps_struct* _temp3702=( struct Cyc_Absyn_Nullable_ps_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct)); _temp3702->tag= Cyc_Absyn_Nullable_ps_tag;
_temp3702->f1= Cyc_Absyn_signed_int_exp( 1, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3703= yyls; struct Cyc_Yyltype* _temp3705= _temp3703.curr
+ yylsp_offset; if( _temp3705 < _temp3703.base? 1: _temp3705 >= _temp3703.last_plus_one){
_throw( Null_Exception);}* _temp3705;}).first_line,({ struct _tagged_ptr3
_temp3706= yyls; struct Cyc_Yyltype* _temp3708= _temp3706.curr + yylsp_offset;
if( _temp3708 < _temp3706.base? 1: _temp3708 >= _temp3706.last_plus_one){ _throw(
Null_Exception);}* _temp3708;}).last_line));( void*) _temp3702;})};( struct
_xenum_struct*) _temp3701;}); break; case 132: _LL3700: yyval=({ struct Cyc_Pointer_Sort_tok_struct*
_temp3710=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));*
_temp3710=( struct Cyc_Pointer_Sort_tok_struct){.tag= Cyc_Pointer_Sort_tok_tag,.f1=({
struct Cyc_Absyn_NonNullable_ps_struct* _temp3711=( struct Cyc_Absyn_NonNullable_ps_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_NonNullable_ps_struct)); _temp3711->tag= Cyc_Absyn_NonNullable_ps_tag;
_temp3711->f1= Cyc_Absyn_signed_int_exp( 1, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3712= yyls; struct Cyc_Yyltype* _temp3714= _temp3712.curr
+ yylsp_offset; if( _temp3714 < _temp3712.base? 1: _temp3714 >= _temp3712.last_plus_one){
_throw( Null_Exception);}* _temp3714;}).first_line,({ struct _tagged_ptr3
_temp3715= yyls; struct Cyc_Yyltype* _temp3717= _temp3715.curr + yylsp_offset;
if( _temp3717 < _temp3715.base? 1: _temp3717 >= _temp3715.last_plus_one){ _throw(
Null_Exception);}* _temp3717;}).last_line));( void*) _temp3711;})};( struct
_xenum_struct*) _temp3710;}); break; case 133: _LL3709: yyval=({ struct Cyc_Pointer_Sort_tok_struct*
_temp3719=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));*
_temp3719=( struct Cyc_Pointer_Sort_tok_struct){.tag= Cyc_Pointer_Sort_tok_tag,.f1=({
struct Cyc_Absyn_Nullable_ps_struct* _temp3720=( struct Cyc_Absyn_Nullable_ps_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct)); _temp3720->tag= Cyc_Absyn_Nullable_ps_tag;
_temp3720->f1= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp3721= yyvs; struct
_xenum_struct** _temp3723= _temp3721.curr +( yyvsp_offset - 1); if( _temp3723 <
_temp3721.base? 1: _temp3723 >= _temp3721.last_plus_one){ _throw( Null_Exception);}*
_temp3723;}));( void*) _temp3720;})};( struct _xenum_struct*) _temp3719;});
break; case 134: _LL3718: yyval=({ struct Cyc_Pointer_Sort_tok_struct* _temp3725=(
struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));*
_temp3725=( struct Cyc_Pointer_Sort_tok_struct){.tag= Cyc_Pointer_Sort_tok_tag,.f1=({
struct Cyc_Absyn_NonNullable_ps_struct* _temp3726=( struct Cyc_Absyn_NonNullable_ps_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_NonNullable_ps_struct)); _temp3726->tag= Cyc_Absyn_NonNullable_ps_tag;
_temp3726->f1= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp3727= yyvs; struct
_xenum_struct** _temp3729= _temp3727.curr +( yyvsp_offset - 1); if( _temp3729 <
_temp3727.base? 1: _temp3729 >= _temp3727.last_plus_one){ _throw( Null_Exception);}*
_temp3729;}));( void*) _temp3726;})};( struct _xenum_struct*) _temp3725;});
break; case 135: _LL3724: yyval=({ struct Cyc_Pointer_Sort_tok_struct* _temp3731=(
struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));*
_temp3731=( struct Cyc_Pointer_Sort_tok_struct){.tag= Cyc_Pointer_Sort_tok_tag,.f1=
Cyc_Absyn_TaggedArray_ps};( struct _xenum_struct*) _temp3731;}); break; case 136:
_LL3730: yyval=({ struct Cyc_Rgn_tok_struct* _temp3733=( struct Cyc_Rgn_tok_struct*)
GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));* _temp3733=( struct Cyc_Rgn_tok_struct){.tag=
Cyc_Rgn_tok_tag,.f1= Cyc_Absyn_HeapRgn};( struct _xenum_struct*) _temp3733;});
break; case 137: _LL3732: yyval=({ struct Cyc_Rgn_tok_struct* _temp3735=( struct
Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));* _temp3735=(
struct Cyc_Rgn_tok_struct){.tag= Cyc_Rgn_tok_tag,.f1= Cyc_Parse_id2type( Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp3736= yyvs; struct _xenum_struct** _temp3738= _temp3736.curr
+ yyvsp_offset; if( _temp3738 < _temp3736.base? 1: _temp3738 >= _temp3736.last_plus_one){
_throw( Null_Exception);}* _temp3738;})),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)( Cyc_Absyn_RgnKind))};( struct _xenum_struct*) _temp3735;});
break; case 138: _LL3734: if( Cyc_yyget_Kind_tok(({ struct _tagged_ptr2
_temp3740= yyvs; struct _xenum_struct** _temp3742= _temp3740.curr + yyvsp_offset;
if( _temp3742 < _temp3740.base? 1: _temp3742 >= _temp3740.last_plus_one){ _throw(
Null_Exception);}* _temp3742;})) != Cyc_Absyn_RgnKind){ Cyc_Parse_err(( struct
_tagged_string)({ char* _temp3743=( char*)"expecting region kind\n"; struct
_tagged_string _temp3744; _temp3744.curr= _temp3743; _temp3744.base= _temp3743;
_temp3744.last_plus_one= _temp3743 + 23; _temp3744;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3745= yyls; struct Cyc_Yyltype* _temp3747= _temp3745.curr
+ yylsp_offset; if( _temp3747 < _temp3745.base? 1: _temp3747 >= _temp3745.last_plus_one){
_throw( Null_Exception);}* _temp3747;}).first_line,({ struct _tagged_ptr3
_temp3748= yyls; struct Cyc_Yyltype* _temp3750= _temp3748.curr + yylsp_offset;
if( _temp3750 < _temp3748.base? 1: _temp3750 >= _temp3748.last_plus_one){ _throw(
Null_Exception);}* _temp3750;}).last_line));} yyval=({ struct Cyc_Rgn_tok_struct*
_temp3751=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));*
_temp3751=( struct Cyc_Rgn_tok_struct){.tag= Cyc_Rgn_tok_tag,.f1= Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp3752= yyvs; struct
_xenum_struct** _temp3754= _temp3752.curr +( yyvsp_offset - 2); if( _temp3754 <
_temp3752.base? 1: _temp3754 >= _temp3752.last_plus_one){ _throw( Null_Exception);}*
_temp3754;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
Cyc_Absyn_RgnKind))};( struct _xenum_struct*) _temp3751;}); break; case 139:
_LL3739: yyval=({ struct Cyc_Rgn_tok_struct* _temp3756=( struct Cyc_Rgn_tok_struct*)
GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));* _temp3756=( struct Cyc_Rgn_tok_struct){.tag=
Cyc_Rgn_tok_tag,.f1= Cyc_Absyn_new_evar( Cyc_Absyn_RgnKind)};( struct
_xenum_struct*) _temp3756;}); break; case 140: _LL3755: yyval=({ struct
_tagged_ptr2 _temp3758= yyvs; struct _xenum_struct** _temp3760= _temp3758.curr +
yyvsp_offset; if( _temp3760 < _temp3758.base? 1: _temp3760 >= _temp3758.last_plus_one){
_throw( Null_Exception);}* _temp3760;}); break; case 141: _LL3757: yyval=({
struct Cyc_TypeQual_tok_struct* _temp3762=( struct Cyc_TypeQual_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct));* _temp3762=( struct Cyc_TypeQual_tok_struct){.tag=
Cyc_TypeQual_tok_tag,.f1= Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok(({
struct _tagged_ptr2 _temp3763= yyvs; struct _xenum_struct** _temp3765= _temp3763.curr
+( yyvsp_offset - 1); if( _temp3765 < _temp3763.base? 1: _temp3765 >= _temp3763.last_plus_one){
_throw( Null_Exception);}* _temp3765;})), Cyc_yyget_TypeQual_tok(({ struct
_tagged_ptr2 _temp3766= yyvs; struct _xenum_struct** _temp3768= _temp3766.curr +
yyvsp_offset; if( _temp3768 < _temp3766.base? 1: _temp3768 >= _temp3766.last_plus_one){
_throw( Null_Exception);}* _temp3768;})))};( struct _xenum_struct*) _temp3762;});
break; case 142: _LL3761: yyval=({ struct Cyc_ParamDeclListBool_tok_struct*
_temp3770=( struct Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct
Cyc_ParamDeclListBool_tok_struct));* _temp3770=( struct Cyc_ParamDeclListBool_tok_struct){.tag=
Cyc_ParamDeclListBool_tok_tag,.f1=({ struct _tuple14* _temp3771=( struct
_tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp3771->f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp3772= yyvs; struct _xenum_struct** _temp3774= _temp3772.curr
+ yyvsp_offset; if( _temp3774 < _temp3772.base? 1: _temp3774 >= _temp3772.last_plus_one){
_throw( Null_Exception);}* _temp3774;}))); _temp3771->f2= 0; _temp3771->f3= 0;
_temp3771;})};( struct _xenum_struct*) _temp3770;}); break; case 143: _LL3769:
yyval=({ struct Cyc_ParamDeclListBool_tok_struct* _temp3776=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct));* _temp3776=(
struct Cyc_ParamDeclListBool_tok_struct){.tag= Cyc_ParamDeclListBool_tok_tag,.f1=({
struct _tuple14* _temp3777=( struct _tuple14*) GC_malloc( sizeof( struct
_tuple14)); _temp3777->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({ struct _tagged_ptr2 _temp3778=
yyvs; struct _xenum_struct** _temp3780= _temp3778.curr +( yyvsp_offset - 2); if(
_temp3780 < _temp3778.base? 1: _temp3780 >= _temp3778.last_plus_one){ _throw(
Null_Exception);}* _temp3780;}))); _temp3777->f2= 1; _temp3777->f3= 0; _temp3777;})};(
struct _xenum_struct*) _temp3776;}); break; case 144: _LL3775: yyval=({ struct
Cyc_ParamDeclListBool_tok_struct* _temp3782=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct));* _temp3782=(
struct Cyc_ParamDeclListBool_tok_struct){.tag= Cyc_ParamDeclListBool_tok_tag,.f1=({
struct _tuple14* _temp3783=( struct _tuple14*) GC_malloc( sizeof( struct
_tuple14)); _temp3783->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({ struct _tagged_ptr2 _temp3789=
yyvs; struct _xenum_struct** _temp3791= _temp3789.curr +( yyvsp_offset - 2); if(
_temp3791 < _temp3789.base? 1: _temp3791 >= _temp3789.last_plus_one){ _throw(
Null_Exception);}* _temp3791;}))); _temp3783->f2= 0; _temp3783->f3=( struct Cyc_Core_Opt*)({
struct Cyc_Core_Opt* _temp3784=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3784->v=( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp3785=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3785->tag= Cyc_Absyn_JoinEff_tag; _temp3785->f1= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3786= yyvs; struct _xenum_struct** _temp3788= _temp3786.curr
+ yyvsp_offset; if( _temp3788 < _temp3786.base? 1: _temp3788 >= _temp3786.last_plus_one){
_throw( Null_Exception);}* _temp3788;}));( void*) _temp3785;}); _temp3784;});
_temp3783;})};( struct _xenum_struct*) _temp3782;}); break; case 145: _LL3781:
yyval=({ struct _tagged_ptr2 _temp3793= yyvs; struct _xenum_struct** _temp3795=
_temp3793.curr + yyvsp_offset; if( _temp3795 < _temp3793.base? 1: _temp3795 >=
_temp3793.last_plus_one){ _throw( Null_Exception);}* _temp3795;}); break; case
146: _LL3792: yyval=({ struct Cyc_TypeList_tok_struct* _temp3797=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));* _temp3797=( struct Cyc_TypeList_tok_struct){.tag=
Cyc_TypeList_tok_tag,.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp3798= yyvs; struct _xenum_struct** _temp3800= _temp3798.curr +(
yyvsp_offset - 2); if( _temp3800 < _temp3798.base? 1: _temp3800 >= _temp3798.last_plus_one){
_throw( Null_Exception);}* _temp3800;})), Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp3801= yyvs; struct _xenum_struct** _temp3803= _temp3801.curr +
yyvsp_offset; if( _temp3803 < _temp3801.base? 1: _temp3803 >= _temp3801.last_plus_one){
_throw( Null_Exception);}* _temp3803;})))};( struct _xenum_struct*) _temp3797;});
break; case 147: _LL3796: yyval=({ struct Cyc_TypeList_tok_struct* _temp3805=(
struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));*
_temp3805=( struct Cyc_TypeList_tok_struct){.tag= Cyc_TypeList_tok_tag,.f1= 0};(
struct _xenum_struct*) _temp3805;}); break; case 148: _LL3804: yyval=({ struct
_tagged_ptr2 _temp3807= yyvs; struct _xenum_struct** _temp3809= _temp3807.curr +(
yyvsp_offset - 1); if( _temp3809 < _temp3807.base? 1: _temp3809 >= _temp3807.last_plus_one){
_throw( Null_Exception);}* _temp3809;}); break; case 149: _LL3806: yyval=({
struct Cyc_TypeList_tok_struct* _temp3811=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));* _temp3811=( struct Cyc_TypeList_tok_struct){.tag=
Cyc_TypeList_tok_tag,.f1=({ struct Cyc_List_List* _temp3812=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3812->hd=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp3813= yyvs; struct
_xenum_struct** _temp3815= _temp3813.curr + yyvsp_offset; if( _temp3815 <
_temp3813.base? 1: _temp3815 >= _temp3813.last_plus_one){ _throw( Null_Exception);}*
_temp3815;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
Cyc_Absyn_EffKind)); _temp3812->tl= 0; _temp3812;})};( struct _xenum_struct*)
_temp3811;}); break; case 150: _LL3810: if( Cyc_yyget_Kind_tok(({ struct
_tagged_ptr2 _temp3817= yyvs; struct _xenum_struct** _temp3819= _temp3817.curr +
yyvsp_offset; if( _temp3819 < _temp3817.base? 1: _temp3819 >= _temp3817.last_plus_one){
_throw( Null_Exception);}* _temp3819;})) != Cyc_Absyn_EffKind){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp3820=( char*)"expecing effect kind (E)";
struct _tagged_string _temp3821; _temp3821.curr= _temp3820; _temp3821.base=
_temp3820; _temp3821.last_plus_one= _temp3820 + 25; _temp3821;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3822= yyls; struct Cyc_Yyltype* _temp3824= _temp3822.curr
+ yylsp_offset; if( _temp3824 < _temp3822.base? 1: _temp3824 >= _temp3822.last_plus_one){
_throw( Null_Exception);}* _temp3824;}).first_line,({ struct _tagged_ptr3
_temp3825= yyls; struct Cyc_Yyltype* _temp3827= _temp3825.curr + yylsp_offset;
if( _temp3827 < _temp3825.base? 1: _temp3827 >= _temp3825.last_plus_one){ _throw(
Null_Exception);}* _temp3827;}).last_line));} yyval=({ struct Cyc_TypeList_tok_struct*
_temp3828=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));*
_temp3828=( struct Cyc_TypeList_tok_struct){.tag= Cyc_TypeList_tok_tag,.f1=({
struct Cyc_List_List* _temp3829=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3829->hd=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp3830= yyvs; struct _xenum_struct** _temp3832= _temp3830.curr
+( yyvsp_offset - 2); if( _temp3832 < _temp3830.base? 1: _temp3832 >= _temp3830.last_plus_one){
_throw( Null_Exception);}* _temp3832;})),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)( Cyc_Absyn_EffKind)); _temp3829->tl= 0; _temp3829;})};(
struct _xenum_struct*) _temp3828;}); break; case 151: _LL3816: yyval=({ struct
Cyc_TypeList_tok_struct* _temp3834=( struct Cyc_TypeList_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeList_tok_struct));* _temp3834=( struct Cyc_TypeList_tok_struct){.tag=
Cyc_TypeList_tok_tag,.f1=({ struct Cyc_List_List* _temp3835=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3835->hd=( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp3836=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3836->tag= Cyc_Absyn_AccessEff_tag; _temp3836->f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp3837= yyvs; struct
_xenum_struct** _temp3839= _temp3837.curr + yyvsp_offset; if( _temp3839 <
_temp3837.base? 1: _temp3839 >= _temp3837.last_plus_one){ _throw( Null_Exception);}*
_temp3839;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
Cyc_Absyn_RgnKind));( void*) _temp3836;}); _temp3835->tl= 0; _temp3835;})};(
struct _xenum_struct*) _temp3834;}); break; case 152: _LL3833: yyval=({ struct
Cyc_TypeList_tok_struct* _temp3841=( struct Cyc_TypeList_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeList_tok_struct));* _temp3841=( struct Cyc_TypeList_tok_struct){.tag=
Cyc_TypeList_tok_tag,.f1=({ struct Cyc_List_List* _temp3842=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3842->hd=( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp3846=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3846->tag= Cyc_Absyn_AccessEff_tag; _temp3846->f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp3847= yyvs; struct
_xenum_struct** _temp3849= _temp3847.curr +( yyvsp_offset - 2); if( _temp3849 <
_temp3847.base? 1: _temp3849 >= _temp3847.last_plus_one){ _throw( Null_Exception);}*
_temp3849;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
Cyc_Absyn_RgnKind));( void*) _temp3846;}); _temp3842->tl= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3843= yyvs; struct _xenum_struct** _temp3845= _temp3843.curr
+ yyvsp_offset; if( _temp3845 < _temp3843.base? 1: _temp3845 >= _temp3843.last_plus_one){
_throw( Null_Exception);}* _temp3845;})); _temp3842;})};( struct _xenum_struct*)
_temp3841;}); break; case 153: _LL3840: if( Cyc_yyget_Kind_tok(({ struct
_tagged_ptr2 _temp3851= yyvs; struct _xenum_struct** _temp3853= _temp3851.curr +
yyvsp_offset; if( _temp3853 < _temp3851.base? 1: _temp3853 >= _temp3851.last_plus_one){
_throw( Null_Exception);}* _temp3853;})) != Cyc_Absyn_RgnKind){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp3854=( char*)"expecting region kind (R)";
struct _tagged_string _temp3855; _temp3855.curr= _temp3854; _temp3855.base=
_temp3854; _temp3855.last_plus_one= _temp3854 + 26; _temp3855;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3856= yyls; struct Cyc_Yyltype* _temp3858= _temp3856.curr
+ yylsp_offset; if( _temp3858 < _temp3856.base? 1: _temp3858 >= _temp3856.last_plus_one){
_throw( Null_Exception);}* _temp3858;}).first_line,({ struct _tagged_ptr3
_temp3859= yyls; struct Cyc_Yyltype* _temp3861= _temp3859.curr + yylsp_offset;
if( _temp3861 < _temp3859.base? 1: _temp3861 >= _temp3859.last_plus_one){ _throw(
Null_Exception);}* _temp3861;}).last_line));} yyval=({ struct Cyc_TypeList_tok_struct*
_temp3862=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));*
_temp3862=( struct Cyc_TypeList_tok_struct){.tag= Cyc_TypeList_tok_tag,.f1=({
struct Cyc_List_List* _temp3863=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3863->hd=( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp3864=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3864->tag= Cyc_Absyn_AccessEff_tag; _temp3864->f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp3865= yyvs; struct
_xenum_struct** _temp3867= _temp3865.curr +( yyvsp_offset - 2); if( _temp3867 <
_temp3865.base? 1: _temp3867 >= _temp3865.last_plus_one){ _throw( Null_Exception);}*
_temp3867;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
Cyc_Absyn_RgnKind));( void*) _temp3864;}); _temp3863->tl= 0; _temp3863;})};(
struct _xenum_struct*) _temp3862;}); break; case 154: _LL3850: if( Cyc_yyget_Kind_tok(({
struct _tagged_ptr2 _temp3869= yyvs; struct _xenum_struct** _temp3871= _temp3869.curr
+( yyvsp_offset - 2); if( _temp3871 < _temp3869.base? 1: _temp3871 >= _temp3869.last_plus_one){
_throw( Null_Exception);}* _temp3871;})) != Cyc_Absyn_RgnKind){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp3872=( char*)"expecting region kind (R)";
struct _tagged_string _temp3873; _temp3873.curr= _temp3872; _temp3873.base=
_temp3872; _temp3873.last_plus_one= _temp3872 + 26; _temp3873;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3874= yyls; struct Cyc_Yyltype* _temp3876= _temp3874.curr
+( yylsp_offset - 2); if( _temp3876 < _temp3874.base? 1: _temp3876 >= _temp3874.last_plus_one){
_throw( Null_Exception);}* _temp3876;}).first_line,({ struct _tagged_ptr3
_temp3877= yyls; struct Cyc_Yyltype* _temp3879= _temp3877.curr +( yylsp_offset -
2); if( _temp3879 < _temp3877.base? 1: _temp3879 >= _temp3877.last_plus_one){
_throw( Null_Exception);}* _temp3879;}).last_line));} yyval=({ struct Cyc_TypeList_tok_struct*
_temp3880=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));*
_temp3880=( struct Cyc_TypeList_tok_struct){.tag= Cyc_TypeList_tok_tag,.f1=({
struct Cyc_List_List* _temp3881=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3881->hd=( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp3885=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3885->tag= Cyc_Absyn_AccessEff_tag; _temp3885->f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp3886= yyvs; struct
_xenum_struct** _temp3888= _temp3886.curr +( yyvsp_offset - 4); if( _temp3888 <
_temp3886.base? 1: _temp3888 >= _temp3886.last_plus_one){ _throw( Null_Exception);}*
_temp3888;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
Cyc_Absyn_RgnKind));( void*) _temp3885;}); _temp3881->tl= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3882= yyvs; struct _xenum_struct** _temp3884= _temp3882.curr
+ yyvsp_offset; if( _temp3884 < _temp3882.base? 1: _temp3884 >= _temp3882.last_plus_one){
_throw( Null_Exception);}* _temp3884;})); _temp3881;})};( struct _xenum_struct*)
_temp3880;}); break; case 155: _LL3868: yyval=({ struct Cyc_ParamDeclList_tok_struct*
_temp3890=( struct Cyc_ParamDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct));*
_temp3890=( struct Cyc_ParamDeclList_tok_struct){.tag= Cyc_ParamDeclList_tok_tag,.f1=({
struct Cyc_List_List* _temp3891=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3891->hd=( void*) Cyc_yyget_ParamDecl_tok(({ struct
_tagged_ptr2 _temp3892= yyvs; struct _xenum_struct** _temp3894= _temp3892.curr +
yyvsp_offset; if( _temp3894 < _temp3892.base? 1: _temp3894 >= _temp3892.last_plus_one){
_throw( Null_Exception);}* _temp3894;})); _temp3891->tl= 0; _temp3891;})};(
struct _xenum_struct*) _temp3890;}); break; case 156: _LL3889: yyval=({ struct
Cyc_ParamDeclList_tok_struct* _temp3896=( struct Cyc_ParamDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct));* _temp3896=( struct
Cyc_ParamDeclList_tok_struct){.tag= Cyc_ParamDeclList_tok_tag,.f1=({ struct Cyc_List_List*
_temp3897=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3897->hd=( void*) Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp3901=
yyvs; struct _xenum_struct** _temp3903= _temp3901.curr + yyvsp_offset; if(
_temp3903 < _temp3901.base? 1: _temp3903 >= _temp3901.last_plus_one){ _throw(
Null_Exception);}* _temp3903;})); _temp3897->tl= Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp3898= yyvs; struct _xenum_struct** _temp3900= _temp3898.curr
+( yyvsp_offset - 2); if( _temp3900 < _temp3898.base? 1: _temp3900 >= _temp3898.last_plus_one){
_throw( Null_Exception);}* _temp3900;})); _temp3897;})};( struct _xenum_struct*)
_temp3896;}); break; case 157: _LL3895: { void* t= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3905= yyvs; struct _xenum_struct** _temp3907= _temp3905.curr
+( yyvsp_offset - 1); if( _temp3907 < _temp3905.base? 1: _temp3907 >= _temp3905.last_plus_one){
_throw( Null_Exception);}* _temp3907;}))).f2, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3908= yyls; struct Cyc_Yyltype* _temp3910= _temp3908.curr
+( yylsp_offset - 1); if( _temp3910 < _temp3908.base? 1: _temp3910 >= _temp3908.last_plus_one){
_throw( Null_Exception);}* _temp3910;}).first_line,({ struct _tagged_ptr3
_temp3911= yyls; struct Cyc_Yyltype* _temp3913= _temp3911.curr +( yylsp_offset -
1); if( _temp3913 < _temp3911.base? 1: _temp3913 >= _temp3911.last_plus_one){
_throw( Null_Exception);}* _temp3913;}).last_line)); struct Cyc_List_List* atts=(*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp3914= yyvs; struct
_xenum_struct** _temp3916= _temp3914.curr +( yyvsp_offset - 1); if( _temp3916 <
_temp3914.base? 1: _temp3916 >= _temp3914.last_plus_one){ _throw( Null_Exception);}*
_temp3916;}))).f3; struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3917= yyvs; struct _xenum_struct** _temp3919= _temp3917.curr
+( yyvsp_offset - 1); if( _temp3919 < _temp3917.base? 1: _temp3919 >= _temp3917.last_plus_one){
_throw( Null_Exception);}* _temp3919;}))).f1; struct Cyc_List_List* tms=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3920= yyvs; struct _xenum_struct** _temp3922= _temp3920.curr
+ yyvsp_offset; if( _temp3922 < _temp3920.base? 1: _temp3922 >= _temp3920.last_plus_one){
_throw( Null_Exception);}* _temp3922;})))->tms; struct _tuple6 t_info= Cyc_Parse_apply_tms(
tq, t, atts, tms); if( t_info.f3 != 0){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp3923=( char*)"parameter with bad type params"; struct _tagged_string
_temp3924; _temp3924.curr= _temp3923; _temp3924.base= _temp3923; _temp3924.last_plus_one=
_temp3923 + 31; _temp3924;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp3925= yyls; struct Cyc_Yyltype* _temp3927= _temp3925.curr + yylsp_offset;
if( _temp3927 < _temp3925.base? 1: _temp3927 >= _temp3925.last_plus_one){ _throw(
Null_Exception);}* _temp3927;}).first_line,({ struct _tagged_ptr3 _temp3928=
yyls; struct Cyc_Yyltype* _temp3930= _temp3928.curr + yylsp_offset; if(
_temp3930 < _temp3928.base? 1: _temp3930 >= _temp3928.last_plus_one){ _throw(
Null_Exception);}* _temp3930;}).last_line));}{ struct _tuple1* q=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3931= yyvs; struct _xenum_struct** _temp3933= _temp3931.curr
+ yyvsp_offset; if( _temp3933 < _temp3931.base? 1: _temp3933 >= _temp3931.last_plus_one){
_throw( Null_Exception);}* _temp3933;})))->id;{ void* _temp3934=(* q).f1; struct
Cyc_List_List* _temp3944; struct Cyc_List_List* _temp3946; _LL3936: if(
_temp3934 == Cyc_Absyn_Loc_n){ goto _LL3937;} else{ goto _LL3938;} _LL3938: if((
unsigned int) _temp3934 > 1u?(( struct _enum_struct*) _temp3934)->tag == Cyc_Absyn_Rel_n_tag:
0){ _LL3945: _temp3944=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp3934)->f1; if( _temp3944 == 0){ goto _LL3939;} else{ goto _LL3940;}} else{
goto _LL3940;} _LL3940: if(( unsigned int) _temp3934 > 1u?(( struct _enum_struct*)
_temp3934)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL3947: _temp3946=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp3934)->f1; if( _temp3946 == 0){ goto
_LL3941;} else{ goto _LL3942;}} else{ goto _LL3942;} _LL3942: goto _LL3943;
_LL3937: goto _LL3935; _LL3939: goto _LL3935; _LL3941: goto _LL3935; _LL3943:
Cyc_Parse_err(( struct _tagged_string)({ char* _temp3948=( char*)"parameter cannot be qualified with a module name";
struct _tagged_string _temp3949; _temp3949.curr= _temp3948; _temp3949.base=
_temp3948; _temp3949.last_plus_one= _temp3948 + 49; _temp3949;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3950= yyls; struct Cyc_Yyltype* _temp3952= _temp3950.curr
+( yylsp_offset - 1); if( _temp3952 < _temp3950.base? 1: _temp3952 >= _temp3950.last_plus_one){
_throw( Null_Exception);}* _temp3952;}).first_line,({ struct _tagged_ptr3
_temp3953= yyls; struct Cyc_Yyltype* _temp3955= _temp3953.curr +( yylsp_offset -
1); if( _temp3955 < _temp3953.base? 1: _temp3955 >= _temp3953.last_plus_one){
_throw( Null_Exception);}* _temp3955;}).last_line)); goto _LL3935; _LL3935:;}{
struct Cyc_Core_Opt* idopt=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt*
_temp3956=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3956->v=( void*)(* q).f2; _temp3956;}); if( t_info.f4 != 0){ Cyc_Parse_warn((
struct _tagged_string)({ char* _temp3957=( char*)"extra attributes on parameter, ignoring";
struct _tagged_string _temp3958; _temp3958.curr= _temp3957; _temp3958.base=
_temp3957; _temp3958.last_plus_one= _temp3957 + 40; _temp3958;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3959= yyls; struct Cyc_Yyltype* _temp3961= _temp3959.curr
+( yylsp_offset - 1); if( _temp3961 < _temp3959.base? 1: _temp3961 >= _temp3959.last_plus_one){
_throw( Null_Exception);}* _temp3961;}).first_line,({ struct _tagged_ptr3
_temp3962= yyls; struct Cyc_Yyltype* _temp3964= _temp3962.curr + yylsp_offset;
if( _temp3964 < _temp3962.base? 1: _temp3964 >= _temp3962.last_plus_one){ _throw(
Null_Exception);}* _temp3964;}).last_line));} if( t_info.f3 != 0){ Cyc_Parse_warn((
struct _tagged_string)({ char* _temp3965=( char*)"extra type variables on parameter, ignoring";
struct _tagged_string _temp3966; _temp3966.curr= _temp3965; _temp3966.base=
_temp3965; _temp3966.last_plus_one= _temp3965 + 44; _temp3966;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3967= yyls; struct Cyc_Yyltype* _temp3969= _temp3967.curr
+( yylsp_offset - 1); if( _temp3969 < _temp3967.base? 1: _temp3969 >= _temp3967.last_plus_one){
_throw( Null_Exception);}* _temp3969;}).first_line,({ struct _tagged_ptr3
_temp3970= yyls; struct Cyc_Yyltype* _temp3972= _temp3970.curr + yylsp_offset;
if( _temp3972 < _temp3970.base? 1: _temp3972 >= _temp3970.last_plus_one){ _throw(
Null_Exception);}* _temp3972;}).last_line));} yyval=({ struct Cyc_ParamDecl_tok_struct*
_temp3973=( struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));*
_temp3973=( struct Cyc_ParamDecl_tok_struct){.tag= Cyc_ParamDecl_tok_tag,.f1=({
struct _tuple2* _temp3974=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp3974->f1= idopt; _temp3974->f2= t_info.f1; _temp3974->f3= t_info.f2;
_temp3974;})};( struct _xenum_struct*) _temp3973;}); break;}}} case 158: _LL3904: {
void* t= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp3976= yyvs; struct _xenum_struct** _temp3978= _temp3976.curr +
yyvsp_offset; if( _temp3978 < _temp3976.base? 1: _temp3978 >= _temp3976.last_plus_one){
_throw( Null_Exception);}* _temp3978;}))).f2, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3979= yyls; struct Cyc_Yyltype* _temp3981= _temp3979.curr
+ yylsp_offset; if( _temp3981 < _temp3979.base? 1: _temp3981 >= _temp3979.last_plus_one){
_throw( Null_Exception);}* _temp3981;}).first_line,({ struct _tagged_ptr3
_temp3982= yyls; struct Cyc_Yyltype* _temp3984= _temp3982.curr + yylsp_offset;
if( _temp3984 < _temp3982.base? 1: _temp3984 >= _temp3982.last_plus_one){ _throw(
Null_Exception);}* _temp3984;}).last_line)); struct Cyc_List_List* atts=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3985= yyvs; struct _xenum_struct** _temp3987= _temp3985.curr
+ yyvsp_offset; if( _temp3987 < _temp3985.base? 1: _temp3987 >= _temp3985.last_plus_one){
_throw( Null_Exception);}* _temp3987;}))).f3; struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3988= yyvs; struct _xenum_struct** _temp3990= _temp3988.curr
+ yyvsp_offset; if( _temp3990 < _temp3988.base? 1: _temp3990 >= _temp3988.last_plus_one){
_throw( Null_Exception);}* _temp3990;}))).f1; if( atts != 0){ Cyc_Parse_warn((
struct _tagged_string)({ char* _temp3991=( char*)"bad attributes on parameter, ignoring";
struct _tagged_string _temp3992; _temp3992.curr= _temp3991; _temp3992.base=
_temp3991; _temp3992.last_plus_one= _temp3991 + 38; _temp3992;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3993= yyls; struct Cyc_Yyltype* _temp3995= _temp3993.curr
+ yylsp_offset; if( _temp3995 < _temp3993.base? 1: _temp3995 >= _temp3993.last_plus_one){
_throw( Null_Exception);}* _temp3995;}).first_line,({ struct _tagged_ptr3
_temp3996= yyls; struct Cyc_Yyltype* _temp3998= _temp3996.curr + yylsp_offset;
if( _temp3998 < _temp3996.base? 1: _temp3998 >= _temp3996.last_plus_one){ _throw(
Null_Exception);}* _temp3998;}).last_line));} yyval=({ struct Cyc_ParamDecl_tok_struct*
_temp3999=( struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));*
_temp3999=( struct Cyc_ParamDecl_tok_struct){.tag= Cyc_ParamDecl_tok_tag,.f1=({
struct _tuple2* _temp4000=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp4000->f1= 0; _temp4000->f2= tq; _temp4000->f3= t; _temp4000;})};( struct
_xenum_struct*) _temp3999;}); break;} case 159: _LL3975: { void* t= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp4002= yyvs; struct
_xenum_struct** _temp4004= _temp4002.curr +( yyvsp_offset - 1); if( _temp4004 <
_temp4002.base? 1: _temp4004 >= _temp4002.last_plus_one){ _throw( Null_Exception);}*
_temp4004;}))).f2, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4005=
yyls; struct Cyc_Yyltype* _temp4007= _temp4005.curr +( yylsp_offset - 1); if(
_temp4007 < _temp4005.base? 1: _temp4007 >= _temp4005.last_plus_one){ _throw(
Null_Exception);}* _temp4007;}).first_line,({ struct _tagged_ptr3 _temp4008=
yyls; struct Cyc_Yyltype* _temp4010= _temp4008.curr +( yylsp_offset - 1); if(
_temp4010 < _temp4008.base? 1: _temp4010 >= _temp4008.last_plus_one){ _throw(
Null_Exception);}* _temp4010;}).last_line)); struct Cyc_List_List* atts=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4011= yyvs; struct _xenum_struct** _temp4013= _temp4011.curr
+( yyvsp_offset - 1); if( _temp4013 < _temp4011.base? 1: _temp4013 >= _temp4011.last_plus_one){
_throw( Null_Exception);}* _temp4013;}))).f3; struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4014= yyvs; struct _xenum_struct** _temp4016= _temp4014.curr
+( yyvsp_offset - 1); if( _temp4016 < _temp4014.base? 1: _temp4016 >= _temp4014.last_plus_one){
_throw( Null_Exception);}* _temp4016;}))).f1; struct Cyc_List_List* tms=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp4017= yyvs; struct _xenum_struct** _temp4019= _temp4017.curr
+ yyvsp_offset; if( _temp4019 < _temp4017.base? 1: _temp4019 >= _temp4017.last_plus_one){
_throw( Null_Exception);}* _temp4019;})))->tms; struct _tuple6 t_info= Cyc_Parse_apply_tms(
tq, t, atts, tms); if( t_info.f3 != 0){ Cyc_Parse_warn(( struct _tagged_string)({
char* _temp4020=( char*)"bad type parameters on formal argument, ignoring";
struct _tagged_string _temp4021; _temp4021.curr= _temp4020; _temp4021.base=
_temp4020; _temp4021.last_plus_one= _temp4020 + 49; _temp4021;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4022= yyls; struct Cyc_Yyltype* _temp4024= _temp4022.curr
+( yylsp_offset - 1); if( _temp4024 < _temp4022.base? 1: _temp4024 >= _temp4022.last_plus_one){
_throw( Null_Exception);}* _temp4024;}).first_line,({ struct _tagged_ptr3
_temp4025= yyls; struct Cyc_Yyltype* _temp4027= _temp4025.curr + yylsp_offset;
if( _temp4027 < _temp4025.base? 1: _temp4027 >= _temp4025.last_plus_one){ _throw(
Null_Exception);}* _temp4027;}).last_line));} if( t_info.f4 != 0){ Cyc_Parse_warn((
struct _tagged_string)({ char* _temp4028=( char*)"bad attributes on parameter, ignoring";
struct _tagged_string _temp4029; _temp4029.curr= _temp4028; _temp4029.base=
_temp4028; _temp4029.last_plus_one= _temp4028 + 38; _temp4029;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4030= yyls; struct Cyc_Yyltype* _temp4032= _temp4030.curr
+( yylsp_offset - 1); if( _temp4032 < _temp4030.base? 1: _temp4032 >= _temp4030.last_plus_one){
_throw( Null_Exception);}* _temp4032;}).first_line,({ struct _tagged_ptr3
_temp4033= yyls; struct Cyc_Yyltype* _temp4035= _temp4033.curr + yylsp_offset;
if( _temp4035 < _temp4033.base? 1: _temp4035 >= _temp4033.last_plus_one){ _throw(
Null_Exception);}* _temp4035;}).last_line));} yyval=({ struct Cyc_ParamDecl_tok_struct*
_temp4036=( struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));*
_temp4036=( struct Cyc_ParamDecl_tok_struct){.tag= Cyc_ParamDecl_tok_tag,.f1=({
struct _tuple2* _temp4037=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp4037->f1= 0; _temp4037->f2= t_info.f1; _temp4037->f3= t_info.f2; _temp4037;})};(
struct _xenum_struct*) _temp4036;}); break;} case 160: _LL4001: yyval=({ struct
Cyc_IdList_tok_struct* _temp4039=( struct Cyc_IdList_tok_struct*) GC_malloc(
sizeof( struct Cyc_IdList_tok_struct));* _temp4039=( struct Cyc_IdList_tok_struct){.tag=
Cyc_IdList_tok_tag,.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( Cyc_yyget_IdList_tok(({ struct _tagged_ptr2 _temp4040= yyvs;
struct _xenum_struct** _temp4042= _temp4040.curr + yyvsp_offset; if( _temp4042 <
_temp4040.base? 1: _temp4042 >= _temp4040.last_plus_one){ _throw( Null_Exception);}*
_temp4042;})))};( struct _xenum_struct*) _temp4039;}); break; case 161: _LL4038:
yyval=({ struct Cyc_IdList_tok_struct* _temp4044=( struct Cyc_IdList_tok_struct*)
GC_malloc( sizeof( struct Cyc_IdList_tok_struct));* _temp4044=( struct Cyc_IdList_tok_struct){.tag=
Cyc_IdList_tok_tag,.f1=({ struct Cyc_List_List* _temp4045=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4045->hd=( void*)({ struct
_tagged_string* _temp4046=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp4046[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp4047= yyvs; struct _xenum_struct** _temp4049= _temp4047.curr + yyvsp_offset;
if( _temp4049 < _temp4047.base? 1: _temp4049 >= _temp4047.last_plus_one){ _throw(
Null_Exception);}* _temp4049;})); _temp4046;}); _temp4045->tl= 0; _temp4045;})};(
struct _xenum_struct*) _temp4044;}); break; case 162: _LL4043: yyval=({ struct
Cyc_IdList_tok_struct* _temp4051=( struct Cyc_IdList_tok_struct*) GC_malloc(
sizeof( struct Cyc_IdList_tok_struct));* _temp4051=( struct Cyc_IdList_tok_struct){.tag=
Cyc_IdList_tok_tag,.f1=({ struct Cyc_List_List* _temp4052=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4052->hd=( void*)({ struct
_tagged_string* _temp4056=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp4056[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp4057= yyvs; struct _xenum_struct** _temp4059= _temp4057.curr + yyvsp_offset;
if( _temp4059 < _temp4057.base? 1: _temp4059 >= _temp4057.last_plus_one){ _throw(
Null_Exception);}* _temp4059;})); _temp4056;}); _temp4052->tl= Cyc_yyget_IdList_tok(({
struct _tagged_ptr2 _temp4053= yyvs; struct _xenum_struct** _temp4055= _temp4053.curr
+( yyvsp_offset - 2); if( _temp4055 < _temp4053.base? 1: _temp4055 >= _temp4053.last_plus_one){
_throw( Null_Exception);}* _temp4055;})); _temp4052;})};( struct _xenum_struct*)
_temp4051;}); break; case 163: _LL4050: yyval=({ struct _tagged_ptr2 _temp4061=
yyvs; struct _xenum_struct** _temp4063= _temp4061.curr + yyvsp_offset; if(
_temp4063 < _temp4061.base? 1: _temp4063 >= _temp4061.last_plus_one){ _throw(
Null_Exception);}* _temp4063;}); break; case 164: _LL4060: yyval=({ struct Cyc_Exp_tok_struct*
_temp4065=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp4065=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp4066=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp4066->tag=
Cyc_Absyn_UnresolvedMem_e_tag; _temp4066->f1= 0; _temp4066->f2= 0;( void*)
_temp4066;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4067= yyls;
struct Cyc_Yyltype* _temp4069= _temp4067.curr +( yylsp_offset - 1); if(
_temp4069 < _temp4067.base? 1: _temp4069 >= _temp4067.last_plus_one){ _throw(
Null_Exception);}* _temp4069;}).first_line,({ struct _tagged_ptr3 _temp4070=
yyls; struct Cyc_Yyltype* _temp4072= _temp4070.curr + yylsp_offset; if(
_temp4072 < _temp4070.base? 1: _temp4072 >= _temp4070.last_plus_one){ _throw(
Null_Exception);}* _temp4072;}).last_line))};( struct _xenum_struct*) _temp4065;});
break; case 165: _LL4064: yyval=({ struct Cyc_Exp_tok_struct* _temp4074=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp4074=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp4075=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp4075->tag=
Cyc_Absyn_UnresolvedMem_e_tag; _temp4075->f1= 0; _temp4075->f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp4076= yyvs; struct _xenum_struct** _temp4078= _temp4076.curr
+( yyvsp_offset - 1); if( _temp4078 < _temp4076.base? 1: _temp4078 >= _temp4076.last_plus_one){
_throw( Null_Exception);}* _temp4078;})));( void*) _temp4075;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4079= yyls; struct Cyc_Yyltype* _temp4081= _temp4079.curr
+( yylsp_offset - 2); if( _temp4081 < _temp4079.base? 1: _temp4081 >= _temp4079.last_plus_one){
_throw( Null_Exception);}* _temp4081;}).first_line,({ struct _tagged_ptr3
_temp4082= yyls; struct Cyc_Yyltype* _temp4084= _temp4082.curr + yylsp_offset;
if( _temp4084 < _temp4082.base? 1: _temp4084 >= _temp4082.last_plus_one){ _throw(
Null_Exception);}* _temp4084;}).last_line))};( struct _xenum_struct*) _temp4074;});
break; case 166: _LL4073: yyval=({ struct Cyc_Exp_tok_struct* _temp4086=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp4086=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp4087=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp4087->tag=
Cyc_Absyn_UnresolvedMem_e_tag; _temp4087->f1= 0; _temp4087->f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp4088= yyvs; struct _xenum_struct** _temp4090= _temp4088.curr
+( yyvsp_offset - 2); if( _temp4090 < _temp4088.base? 1: _temp4090 >= _temp4088.last_plus_one){
_throw( Null_Exception);}* _temp4090;})));( void*) _temp4087;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4091= yyls; struct Cyc_Yyltype* _temp4093= _temp4091.curr
+( yylsp_offset - 3); if( _temp4093 < _temp4091.base? 1: _temp4093 >= _temp4091.last_plus_one){
_throw( Null_Exception);}* _temp4093;}).first_line,({ struct _tagged_ptr3
_temp4094= yyls; struct Cyc_Yyltype* _temp4096= _temp4094.curr + yylsp_offset;
if( _temp4096 < _temp4094.base? 1: _temp4096 >= _temp4094.last_plus_one){ _throw(
Null_Exception);}* _temp4096;}).last_line))};( struct _xenum_struct*) _temp4086;});
break; case 167: _LL4085: yyval=({ struct Cyc_InitializerList_tok_struct*
_temp4098=( struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct
Cyc_InitializerList_tok_struct));* _temp4098=( struct Cyc_InitializerList_tok_struct){.tag=
Cyc_InitializerList_tok_tag,.f1=({ struct Cyc_List_List* _temp4099=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4099->hd=( void*)({ struct
_tuple16* _temp4100=( struct _tuple16*) GC_malloc( sizeof( struct _tuple16));
_temp4100->f1= 0; _temp4100->f2= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp4101= yyvs; struct _xenum_struct** _temp4103= _temp4101.curr + yyvsp_offset;
if( _temp4103 < _temp4101.base? 1: _temp4103 >= _temp4101.last_plus_one){ _throw(
Null_Exception);}* _temp4103;})); _temp4100;}); _temp4099->tl= 0; _temp4099;})};(
struct _xenum_struct*) _temp4098;}); break; case 168: _LL4097: yyval=({ struct
Cyc_InitializerList_tok_struct* _temp4105=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct));* _temp4105=( struct
Cyc_InitializerList_tok_struct){.tag= Cyc_InitializerList_tok_tag,.f1=({ struct
Cyc_List_List* _temp4106=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4106->hd=( void*)({ struct _tuple16* _temp4107=( struct _tuple16*)
GC_malloc( sizeof( struct _tuple16)); _temp4107->f1= Cyc_yyget_DesignatorList_tok(({
struct _tagged_ptr2 _temp4111= yyvs; struct _xenum_struct** _temp4113= _temp4111.curr
+( yyvsp_offset - 1); if( _temp4113 < _temp4111.base? 1: _temp4113 >= _temp4111.last_plus_one){
_throw( Null_Exception);}* _temp4113;})); _temp4107->f2= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4108= yyvs; struct _xenum_struct** _temp4110= _temp4108.curr
+ yyvsp_offset; if( _temp4110 < _temp4108.base? 1: _temp4110 >= _temp4108.last_plus_one){
_throw( Null_Exception);}* _temp4110;})); _temp4107;}); _temp4106->tl= 0;
_temp4106;})};( struct _xenum_struct*) _temp4105;}); break; case 169: _LL4104:
yyval=({ struct Cyc_InitializerList_tok_struct* _temp4115=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct));* _temp4115=( struct
Cyc_InitializerList_tok_struct){.tag= Cyc_InitializerList_tok_tag,.f1=({ struct
Cyc_List_List* _temp4116=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4116->hd=( void*)({ struct _tuple16* _temp4120=( struct _tuple16*)
GC_malloc( sizeof( struct _tuple16)); _temp4120->f1= 0; _temp4120->f2= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4121= yyvs; struct _xenum_struct** _temp4123= _temp4121.curr
+ yyvsp_offset; if( _temp4123 < _temp4121.base? 1: _temp4123 >= _temp4121.last_plus_one){
_throw( Null_Exception);}* _temp4123;})); _temp4120;}); _temp4116->tl= Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp4117= yyvs; struct _xenum_struct** _temp4119= _temp4117.curr
+( yyvsp_offset - 2); if( _temp4119 < _temp4117.base? 1: _temp4119 >= _temp4117.last_plus_one){
_throw( Null_Exception);}* _temp4119;})); _temp4116;})};( struct _xenum_struct*)
_temp4115;}); break; case 170: _LL4114: yyval=({ struct Cyc_InitializerList_tok_struct*
_temp4125=( struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct
Cyc_InitializerList_tok_struct));* _temp4125=( struct Cyc_InitializerList_tok_struct){.tag=
Cyc_InitializerList_tok_tag,.f1=({ struct Cyc_List_List* _temp4126=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4126->hd=( void*)({ struct
_tuple16* _temp4130=( struct _tuple16*) GC_malloc( sizeof( struct _tuple16));
_temp4130->f1= Cyc_yyget_DesignatorList_tok(({ struct _tagged_ptr2 _temp4134=
yyvs; struct _xenum_struct** _temp4136= _temp4134.curr +( yyvsp_offset - 1); if(
_temp4136 < _temp4134.base? 1: _temp4136 >= _temp4134.last_plus_one){ _throw(
Null_Exception);}* _temp4136;})); _temp4130->f2= Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4131= yyvs; struct _xenum_struct** _temp4133= _temp4131.curr +
yyvsp_offset; if( _temp4133 < _temp4131.base? 1: _temp4133 >= _temp4131.last_plus_one){
_throw( Null_Exception);}* _temp4133;})); _temp4130;}); _temp4126->tl= Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp4127= yyvs; struct _xenum_struct** _temp4129= _temp4127.curr
+( yyvsp_offset - 3); if( _temp4129 < _temp4127.base? 1: _temp4129 >= _temp4127.last_plus_one){
_throw( Null_Exception);}* _temp4129;})); _temp4126;})};( struct _xenum_struct*)
_temp4125;}); break; case 171: _LL4124: yyval=({ struct Cyc_DesignatorList_tok_struct*
_temp4138=( struct Cyc_DesignatorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));*
_temp4138=( struct Cyc_DesignatorList_tok_struct){.tag= Cyc_DesignatorList_tok_tag,.f1=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_DesignatorList_tok(({
struct _tagged_ptr2 _temp4139= yyvs; struct _xenum_struct** _temp4141= _temp4139.curr
+( yyvsp_offset - 1); if( _temp4141 < _temp4139.base? 1: _temp4141 >= _temp4139.last_plus_one){
_throw( Null_Exception);}* _temp4141;})))};( struct _xenum_struct*) _temp4138;});
break; case 172: _LL4137: yyval=({ struct Cyc_DesignatorList_tok_struct*
_temp4143=( struct Cyc_DesignatorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));*
_temp4143=( struct Cyc_DesignatorList_tok_struct){.tag= Cyc_DesignatorList_tok_tag,.f1=({
struct Cyc_List_List* _temp4144=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4144->hd=( void*) Cyc_yyget_Designator_tok(({
struct _tagged_ptr2 _temp4145= yyvs; struct _xenum_struct** _temp4147= _temp4145.curr
+ yyvsp_offset; if( _temp4147 < _temp4145.base? 1: _temp4147 >= _temp4145.last_plus_one){
_throw( Null_Exception);}* _temp4147;})); _temp4144->tl= 0; _temp4144;})};(
struct _xenum_struct*) _temp4143;}); break; case 173: _LL4142: yyval=({ struct
Cyc_DesignatorList_tok_struct* _temp4149=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));* _temp4149=( struct
Cyc_DesignatorList_tok_struct){.tag= Cyc_DesignatorList_tok_tag,.f1=({ struct
Cyc_List_List* _temp4150=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4150->hd=( void*) Cyc_yyget_Designator_tok(({ struct _tagged_ptr2 _temp4154=
yyvs; struct _xenum_struct** _temp4156= _temp4154.curr + yyvsp_offset; if(
_temp4156 < _temp4154.base? 1: _temp4156 >= _temp4154.last_plus_one){ _throw(
Null_Exception);}* _temp4156;})); _temp4150->tl= Cyc_yyget_DesignatorList_tok(({
struct _tagged_ptr2 _temp4151= yyvs; struct _xenum_struct** _temp4153= _temp4151.curr
+( yyvsp_offset - 1); if( _temp4153 < _temp4151.base? 1: _temp4153 >= _temp4151.last_plus_one){
_throw( Null_Exception);}* _temp4153;})); _temp4150;})};( struct _xenum_struct*)
_temp4149;}); break; case 174: _LL4148: yyval=({ struct Cyc_Designator_tok_struct*
_temp4158=( struct Cyc_Designator_tok_struct*) GC_malloc( sizeof( struct Cyc_Designator_tok_struct));*
_temp4158=( struct Cyc_Designator_tok_struct){.tag= Cyc_Designator_tok_tag,.f1=({
struct Cyc_Absyn_ArrayElement_struct* _temp4159=( struct Cyc_Absyn_ArrayElement_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayElement_struct)); _temp4159->tag= Cyc_Absyn_ArrayElement_tag;
_temp4159->f1= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4160= yyvs; struct
_xenum_struct** _temp4162= _temp4160.curr +( yyvsp_offset - 1); if( _temp4162 <
_temp4160.base? 1: _temp4162 >= _temp4160.last_plus_one){ _throw( Null_Exception);}*
_temp4162;}));( void*) _temp4159;})};( struct _xenum_struct*) _temp4158;});
break; case 175: _LL4157: yyval=({ struct Cyc_Designator_tok_struct* _temp4164=(
struct Cyc_Designator_tok_struct*) GC_malloc( sizeof( struct Cyc_Designator_tok_struct));*
_temp4164=( struct Cyc_Designator_tok_struct){.tag= Cyc_Designator_tok_tag,.f1=({
struct Cyc_Absyn_FieldName_struct* _temp4165=( struct Cyc_Absyn_FieldName_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct)); _temp4165->tag= Cyc_Absyn_FieldName_tag;
_temp4165->f1=({ struct _tagged_string* _temp4166=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp4166[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp4167= yyvs; struct _xenum_struct** _temp4169= _temp4167.curr
+ yyvsp_offset; if( _temp4169 < _temp4167.base? 1: _temp4169 >= _temp4167.last_plus_one){
_throw( Null_Exception);}* _temp4169;})); _temp4166;});( void*) _temp4165;})};(
struct _xenum_struct*) _temp4164;}); break; case 176: _LL4163: { void* t= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp4171= yyvs; struct
_xenum_struct** _temp4173= _temp4171.curr + yyvsp_offset; if( _temp4173 <
_temp4171.base? 1: _temp4173 >= _temp4171.last_plus_one){ _throw( Null_Exception);}*
_temp4173;}))).f2, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4174=
yyls; struct Cyc_Yyltype* _temp4176= _temp4174.curr + yylsp_offset; if(
_temp4176 < _temp4174.base? 1: _temp4176 >= _temp4174.last_plus_one){ _throw(
Null_Exception);}* _temp4176;}).first_line,({ struct _tagged_ptr3 _temp4177=
yyls; struct Cyc_Yyltype* _temp4179= _temp4177.curr + yylsp_offset; if(
_temp4179 < _temp4177.base? 1: _temp4179 >= _temp4177.last_plus_one){ _throw(
Null_Exception);}* _temp4179;}).last_line)); struct Cyc_List_List* atts=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4180= yyvs; struct _xenum_struct** _temp4182= _temp4180.curr
+ yyvsp_offset; if( _temp4182 < _temp4180.base? 1: _temp4182 >= _temp4180.last_plus_one){
_throw( Null_Exception);}* _temp4182;}))).f3; if( atts != 0){ Cyc_Parse_warn((
struct _tagged_string)({ char* _temp4183=( char*)"ignoring attributes in type";
struct _tagged_string _temp4184; _temp4184.curr= _temp4183; _temp4184.base=
_temp4183; _temp4184.last_plus_one= _temp4183 + 28; _temp4184;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4185= yyls; struct Cyc_Yyltype* _temp4187= _temp4185.curr
+ yylsp_offset; if( _temp4187 < _temp4185.base? 1: _temp4187 >= _temp4185.last_plus_one){
_throw( Null_Exception);}* _temp4187;}).first_line,({ struct _tagged_ptr3
_temp4188= yyls; struct Cyc_Yyltype* _temp4190= _temp4188.curr + yylsp_offset;
if( _temp4190 < _temp4188.base? 1: _temp4190 >= _temp4188.last_plus_one){ _throw(
Null_Exception);}* _temp4190;}).last_line));}{ struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4191= yyvs; struct _xenum_struct** _temp4193= _temp4191.curr
+ yyvsp_offset; if( _temp4193 < _temp4191.base? 1: _temp4193 >= _temp4191.last_plus_one){
_throw( Null_Exception);}* _temp4193;}))).f1; yyval=({ struct Cyc_ParamDecl_tok_struct*
_temp4194=( struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));*
_temp4194=( struct Cyc_ParamDecl_tok_struct){.tag= Cyc_ParamDecl_tok_tag,.f1=({
struct _tuple2* _temp4195=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2)
* 1); _temp4195[ 0]=({ struct _tuple2 _temp4196; _temp4196.f1= 0; _temp4196.f2=
tq; _temp4196.f3= t; _temp4196;}); _temp4195;})};( struct _xenum_struct*)
_temp4194;}); break;}} case 177: _LL4170: { void* t= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp4198= yyvs; struct
_xenum_struct** _temp4200= _temp4198.curr +( yyvsp_offset - 1); if( _temp4200 <
_temp4198.base? 1: _temp4200 >= _temp4198.last_plus_one){ _throw( Null_Exception);}*
_temp4200;}))).f2, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4201=
yyls; struct Cyc_Yyltype* _temp4203= _temp4201.curr +( yylsp_offset - 1); if(
_temp4203 < _temp4201.base? 1: _temp4203 >= _temp4201.last_plus_one){ _throw(
Null_Exception);}* _temp4203;}).first_line,({ struct _tagged_ptr3 _temp4204=
yyls; struct Cyc_Yyltype* _temp4206= _temp4204.curr +( yylsp_offset - 1); if(
_temp4206 < _temp4204.base? 1: _temp4206 >= _temp4204.last_plus_one){ _throw(
Null_Exception);}* _temp4206;}).last_line)); struct Cyc_List_List* atts=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4207= yyvs; struct _xenum_struct** _temp4209= _temp4207.curr
+( yyvsp_offset - 1); if( _temp4209 < _temp4207.base? 1: _temp4209 >= _temp4207.last_plus_one){
_throw( Null_Exception);}* _temp4209;}))).f3; struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4210= yyvs; struct _xenum_struct** _temp4212= _temp4210.curr
+( yyvsp_offset - 1); if( _temp4212 < _temp4210.base? 1: _temp4212 >= _temp4210.last_plus_one){
_throw( Null_Exception);}* _temp4212;}))).f1; struct Cyc_List_List* tms=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp4213= yyvs; struct _xenum_struct** _temp4215= _temp4213.curr
+ yyvsp_offset; if( _temp4215 < _temp4213.base? 1: _temp4215 >= _temp4213.last_plus_one){
_throw( Null_Exception);}* _temp4215;})))->tms; struct _tuple6 t_info= Cyc_Parse_apply_tms(
tq, t, atts, tms); if( t_info.f3 != 0){ Cyc_Parse_warn(( struct _tagged_string)({
char* _temp4216=( char*)"bad type params, ignoring"; struct _tagged_string
_temp4217; _temp4217.curr= _temp4216; _temp4217.base= _temp4216; _temp4217.last_plus_one=
_temp4216 + 26; _temp4217;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4218= yyls; struct Cyc_Yyltype* _temp4220= _temp4218.curr + yylsp_offset;
if( _temp4220 < _temp4218.base? 1: _temp4220 >= _temp4218.last_plus_one){ _throw(
Null_Exception);}* _temp4220;}).first_line,({ struct _tagged_ptr3 _temp4221=
yyls; struct Cyc_Yyltype* _temp4223= _temp4221.curr + yylsp_offset; if(
_temp4223 < _temp4221.base? 1: _temp4223 >= _temp4221.last_plus_one){ _throw(
Null_Exception);}* _temp4223;}).last_line));} if( t_info.f4 != 0){ Cyc_Parse_warn((
struct _tagged_string)({ char* _temp4224=( char*)"bad specifiers, ignoring";
struct _tagged_string _temp4225; _temp4225.curr= _temp4224; _temp4225.base=
_temp4224; _temp4225.last_plus_one= _temp4224 + 25; _temp4225;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4226= yyls; struct Cyc_Yyltype* _temp4228= _temp4226.curr
+ yylsp_offset; if( _temp4228 < _temp4226.base? 1: _temp4228 >= _temp4226.last_plus_one){
_throw( Null_Exception);}* _temp4228;}).first_line,({ struct _tagged_ptr3
_temp4229= yyls; struct Cyc_Yyltype* _temp4231= _temp4229.curr + yylsp_offset;
if( _temp4231 < _temp4229.base? 1: _temp4231 >= _temp4229.last_plus_one){ _throw(
Null_Exception);}* _temp4231;}).last_line));} yyval=({ struct Cyc_ParamDecl_tok_struct*
_temp4232=( struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));*
_temp4232=( struct Cyc_ParamDecl_tok_struct){.tag= Cyc_ParamDecl_tok_tag,.f1=({
struct _tuple2* _temp4233=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp4233->f1= 0; _temp4233->f2= t_info.f1; _temp4233->f3= t_info.f2; _temp4233;})};(
struct _xenum_struct*) _temp4232;}); break;} case 178: _LL4197: yyval=({ struct
Cyc_Type_tok_struct* _temp4235=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof(
struct Cyc_Type_tok_struct));* _temp4235=( struct Cyc_Type_tok_struct){.tag= Cyc_Type_tok_tag,.f1=(*
Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp4236= yyvs; struct
_xenum_struct** _temp4238= _temp4236.curr + yyvsp_offset; if( _temp4238 <
_temp4236.base? 1: _temp4238 >= _temp4236.last_plus_one){ _throw( Null_Exception);}*
_temp4238;}))).f3};( struct _xenum_struct*) _temp4235;}); break; case 179:
_LL4234: yyval=({ struct Cyc_Type_tok_struct* _temp4240=( struct Cyc_Type_tok_struct*)
GC_malloc( sizeof( struct Cyc_Type_tok_struct));* _temp4240=( struct Cyc_Type_tok_struct){.tag=
Cyc_Type_tok_tag,.f1=({ struct Cyc_Absyn_JoinEff_struct* _temp4241=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp4241->tag= Cyc_Absyn_JoinEff_tag;
_temp4241->f1= 0;( void*) _temp4241;})};( struct _xenum_struct*) _temp4240;});
break; case 180: _LL4239: yyval=({ struct Cyc_Type_tok_struct* _temp4243=(
struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));*
_temp4243=( struct Cyc_Type_tok_struct){.tag= Cyc_Type_tok_tag,.f1=({ struct Cyc_Absyn_JoinEff_struct*
_temp4244=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp4244->tag= Cyc_Absyn_JoinEff_tag; _temp4244->f1= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4245= yyvs; struct _xenum_struct** _temp4247= _temp4245.curr
+( yyvsp_offset - 1); if( _temp4247 < _temp4245.base? 1: _temp4247 >= _temp4245.last_plus_one){
_throw( Null_Exception);}* _temp4247;}));( void*) _temp4244;})};( struct
_xenum_struct*) _temp4243;}); break; case 181: _LL4242: yyval=({ struct Cyc_Type_tok_struct*
_temp4249=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));*
_temp4249=( struct Cyc_Type_tok_struct){.tag= Cyc_Type_tok_tag,.f1=({ struct Cyc_Absyn_JoinEff_struct*
_temp4250=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp4250->tag= Cyc_Absyn_JoinEff_tag; _temp4250->f1=({ struct Cyc_List_List*
_temp4251=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4251->hd=( void*) Cyc_yyget_Type_tok(({ struct _tagged_ptr2 _temp4255= yyvs;
struct _xenum_struct** _temp4257= _temp4255.curr +( yyvsp_offset - 2); if(
_temp4257 < _temp4255.base? 1: _temp4257 >= _temp4255.last_plus_one){ _throw(
Null_Exception);}* _temp4257;})); _temp4251->tl= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4252= yyvs; struct _xenum_struct** _temp4254= _temp4252.curr
+ yyvsp_offset; if( _temp4254 < _temp4252.base? 1: _temp4254 >= _temp4252.last_plus_one){
_throw( Null_Exception);}* _temp4254;})); _temp4251;});( void*) _temp4250;})};(
struct _xenum_struct*) _temp4249;}); break; case 182: _LL4248: yyval=({ struct
Cyc_TypeList_tok_struct* _temp4259=( struct Cyc_TypeList_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeList_tok_struct));* _temp4259=( struct Cyc_TypeList_tok_struct){.tag=
Cyc_TypeList_tok_tag,.f1=({ struct Cyc_List_List* _temp4260=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4260->hd=( void*) Cyc_yyget_Type_tok(({
struct _tagged_ptr2 _temp4261= yyvs; struct _xenum_struct** _temp4263= _temp4261.curr
+ yyvsp_offset; if( _temp4263 < _temp4261.base? 1: _temp4263 >= _temp4261.last_plus_one){
_throw( Null_Exception);}* _temp4263;})); _temp4260->tl= 0; _temp4260;})};(
struct _xenum_struct*) _temp4259;}); break; case 183: _LL4258: yyval=({ struct
Cyc_TypeList_tok_struct* _temp4265=( struct Cyc_TypeList_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeList_tok_struct));* _temp4265=( struct Cyc_TypeList_tok_struct){.tag=
Cyc_TypeList_tok_tag,.f1=({ struct Cyc_List_List* _temp4266=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4266->hd=( void*) Cyc_yyget_Type_tok(({
struct _tagged_ptr2 _temp4270= yyvs; struct _xenum_struct** _temp4272= _temp4270.curr
+ yyvsp_offset; if( _temp4272 < _temp4270.base? 1: _temp4272 >= _temp4270.last_plus_one){
_throw( Null_Exception);}* _temp4272;})); _temp4266->tl= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4267= yyvs; struct _xenum_struct** _temp4269= _temp4267.curr
+( yyvsp_offset - 2); if( _temp4269 < _temp4267.base? 1: _temp4269 >= _temp4267.last_plus_one){
_throw( Null_Exception);}* _temp4269;})); _temp4266;})};( struct _xenum_struct*)
_temp4265;}); break; case 184: _LL4264: yyval=({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4274=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct));* _temp4274=( struct Cyc_AbstractDeclarator_tok_struct){.tag=
Cyc_AbstractDeclarator_tok_tag,.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4275=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4275->tms= Cyc_yyget_TypeModifierList_tok(({ struct _tagged_ptr2 _temp4276=
yyvs; struct _xenum_struct** _temp4278= _temp4276.curr + yyvsp_offset; if(
_temp4278 < _temp4276.base? 1: _temp4278 >= _temp4276.last_plus_one){ _throw(
Null_Exception);}* _temp4278;})); _temp4275;})};( struct _xenum_struct*)
_temp4274;}); break; case 185: _LL4273: yyval=({ struct _tagged_ptr2 _temp4280=
yyvs; struct _xenum_struct** _temp4282= _temp4280.curr + yyvsp_offset; if(
_temp4282 < _temp4280.base? 1: _temp4282 >= _temp4280.last_plus_one){ _throw(
Null_Exception);}* _temp4282;}); break; case 186: _LL4279: yyval=({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4284=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct));* _temp4284=( struct Cyc_AbstractDeclarator_tok_struct){.tag=
Cyc_AbstractDeclarator_tok_tag,.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4285=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4285->tms=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_TypeModifierList_tok(({ struct _tagged_ptr2
_temp4286= yyvs; struct _xenum_struct** _temp4288= _temp4286.curr +(
yyvsp_offset - 1); if( _temp4288 < _temp4286.base? 1: _temp4288 >= _temp4286.last_plus_one){
_throw( Null_Exception);}* _temp4288;})),( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp4289= yyvs; struct _xenum_struct** _temp4291= _temp4289.curr
+ yyvsp_offset; if( _temp4291 < _temp4289.base? 1: _temp4291 >= _temp4289.last_plus_one){
_throw( Null_Exception);}* _temp4291;})))->tms); _temp4285;})};( struct
_xenum_struct*) _temp4284;}); break; case 187: _LL4283: yyval=({ struct
_tagged_ptr2 _temp4293= yyvs; struct _xenum_struct** _temp4295= _temp4293.curr +(
yyvsp_offset - 1); if( _temp4295 < _temp4293.base? 1: _temp4295 >= _temp4293.last_plus_one){
_throw( Null_Exception);}* _temp4295;}); break; case 188: _LL4292: yyval=({
struct Cyc_AbstractDeclarator_tok_struct* _temp4297=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));* _temp4297=(
struct Cyc_AbstractDeclarator_tok_struct){.tag= Cyc_AbstractDeclarator_tok_tag,.f1=({
struct Cyc_Parse_Abstractdeclarator* _temp4298=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4298->tms=({
struct Cyc_List_List* _temp4299=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4299->hd=( void*) Cyc_Absyn_Carray_mod; _temp4299->tl=
0; _temp4299;}); _temp4298;})};( struct _xenum_struct*) _temp4297;}); break;
case 189: _LL4296: yyval=({ struct Cyc_AbstractDeclarator_tok_struct* _temp4301=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));*
_temp4301=( struct Cyc_AbstractDeclarator_tok_struct){.tag= Cyc_AbstractDeclarator_tok_tag,.f1=({
struct Cyc_Parse_Abstractdeclarator* _temp4302=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4302->tms=({
struct Cyc_List_List* _temp4303=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4303->hd=( void*) Cyc_Absyn_Carray_mod; _temp4303->tl=(
Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2 _temp4304= yyvs; struct
_xenum_struct** _temp4306= _temp4304.curr +( yyvsp_offset - 2); if( _temp4306 <
_temp4304.base? 1: _temp4306 >= _temp4304.last_plus_one){ _throw( Null_Exception);}*
_temp4306;})))->tms; _temp4303;}); _temp4302;})};( struct _xenum_struct*)
_temp4301;}); break; case 190: _LL4300: yyval=({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4308=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct));* _temp4308=( struct Cyc_AbstractDeclarator_tok_struct){.tag=
Cyc_AbstractDeclarator_tok_tag,.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4309=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4309->tms=({ struct Cyc_List_List* _temp4310=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4310->hd=( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp4311=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp4311->tag= Cyc_Absyn_ConstArray_mod_tag;
_temp4311->f1= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4312= yyvs; struct
_xenum_struct** _temp4314= _temp4312.curr +( yyvsp_offset - 1); if( _temp4314 <
_temp4312.base? 1: _temp4314 >= _temp4312.last_plus_one){ _throw( Null_Exception);}*
_temp4314;}));( void*) _temp4311;}); _temp4310->tl= 0; _temp4310;}); _temp4309;})};(
struct _xenum_struct*) _temp4308;}); break; case 191: _LL4307: yyval=({ struct
Cyc_AbstractDeclarator_tok_struct* _temp4316=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));* _temp4316=(
struct Cyc_AbstractDeclarator_tok_struct){.tag= Cyc_AbstractDeclarator_tok_tag,.f1=({
struct Cyc_Parse_Abstractdeclarator* _temp4317=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4317->tms=({
struct Cyc_List_List* _temp4318=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4318->hd=( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp4322=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp4322->tag= Cyc_Absyn_ConstArray_mod_tag;
_temp4322->f1= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4323= yyvs; struct
_xenum_struct** _temp4325= _temp4323.curr +( yyvsp_offset - 1); if( _temp4325 <
_temp4323.base? 1: _temp4325 >= _temp4323.last_plus_one){ _throw( Null_Exception);}*
_temp4325;}));( void*) _temp4322;}); _temp4318->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp4319= yyvs; struct _xenum_struct** _temp4321= _temp4319.curr
+( yyvsp_offset - 3); if( _temp4321 < _temp4319.base? 1: _temp4321 >= _temp4319.last_plus_one){
_throw( Null_Exception);}* _temp4321;})))->tms; _temp4318;}); _temp4317;})};(
struct _xenum_struct*) _temp4316;}); break; case 192: _LL4315: yyval=({ struct
Cyc_AbstractDeclarator_tok_struct* _temp4327=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));* _temp4327=(
struct Cyc_AbstractDeclarator_tok_struct){.tag= Cyc_AbstractDeclarator_tok_tag,.f1=({
struct Cyc_Parse_Abstractdeclarator* _temp4328=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4328->tms=({
struct Cyc_List_List* _temp4329=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4329->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4330=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4330->tag= Cyc_Absyn_Function_mod_tag; _temp4330->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4331=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4331->tag= Cyc_Absyn_WithTypes_tag; _temp4331->f1= 0; _temp4331->f2= 0;
_temp4331->f3= 0;( void*) _temp4331;});( void*) _temp4330;}); _temp4329->tl= 0;
_temp4329;}); _temp4328;})};( struct _xenum_struct*) _temp4327;}); break; case
193: _LL4326: yyval=({ struct Cyc_AbstractDeclarator_tok_struct* _temp4333=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));*
_temp4333=( struct Cyc_AbstractDeclarator_tok_struct){.tag= Cyc_AbstractDeclarator_tok_tag,.f1=({
struct Cyc_Parse_Abstractdeclarator* _temp4334=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4334->tms=({
struct Cyc_List_List* _temp4335=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4335->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4336=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4336->tag= Cyc_Absyn_Function_mod_tag; _temp4336->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4337=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4337->tag= Cyc_Absyn_WithTypes_tag; _temp4337->f1= 0; _temp4337->f2= 0;
_temp4337->f3=({ struct Cyc_Core_Opt* _temp4338=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp4338->v=( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp4339=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp4339->tag= Cyc_Absyn_JoinEff_tag; _temp4339->f1= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4340= yyvs; struct _xenum_struct** _temp4342= _temp4340.curr
+( yyvsp_offset - 1); if( _temp4342 < _temp4340.base? 1: _temp4342 >= _temp4340.last_plus_one){
_throw( Null_Exception);}* _temp4342;}));( void*) _temp4339;}); _temp4338;});(
void*) _temp4337;});( void*) _temp4336;}); _temp4335->tl= 0; _temp4335;});
_temp4334;})};( struct _xenum_struct*) _temp4333;}); break; case 194: _LL4332: {
struct _tuple14 _temp4349; struct Cyc_Core_Opt* _temp4350; int _temp4352; struct
Cyc_List_List* _temp4354; struct _tuple14* _temp4347= Cyc_yyget_ParamDeclListBool_tok(({
struct _tagged_ptr2 _temp4344= yyvs; struct _xenum_struct** _temp4346= _temp4344.curr
+( yyvsp_offset - 1); if( _temp4346 < _temp4344.base? 1: _temp4346 >= _temp4344.last_plus_one){
_throw( Null_Exception);}* _temp4346;})); _temp4349=* _temp4347; _LL4355:
_temp4354= _temp4349.f1; goto _LL4353; _LL4353: _temp4352= _temp4349.f2; goto
_LL4351; _LL4351: _temp4350= _temp4349.f3; goto _LL4348; _LL4348: yyval=({
struct Cyc_AbstractDeclarator_tok_struct* _temp4356=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));* _temp4356=(
struct Cyc_AbstractDeclarator_tok_struct){.tag= Cyc_AbstractDeclarator_tok_tag,.f1=({
struct Cyc_Parse_Abstractdeclarator* _temp4357=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4357->tms=({
struct Cyc_List_List* _temp4358=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4358->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4359=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4359->tag= Cyc_Absyn_Function_mod_tag; _temp4359->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4360=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4360->tag= Cyc_Absyn_WithTypes_tag; _temp4360->f1= _temp4354; _temp4360->f2=
_temp4352; _temp4360->f3= _temp4350;( void*) _temp4360;});( void*) _temp4359;});
_temp4358->tl= 0; _temp4358;}); _temp4357;})};( struct _xenum_struct*) _temp4356;});
break;} case 195: _LL4343: yyval=({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4362=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct));* _temp4362=( struct Cyc_AbstractDeclarator_tok_struct){.tag=
Cyc_AbstractDeclarator_tok_tag,.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4363=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4363->tms=({ struct Cyc_List_List* _temp4364=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4364->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4368=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4368->tag= Cyc_Absyn_Function_mod_tag; _temp4368->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4369=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4369->tag= Cyc_Absyn_WithTypes_tag; _temp4369->f1= 0; _temp4369->f2= 0;
_temp4369->f3= 0;( void*) _temp4369;});( void*) _temp4368;}); _temp4364->tl=(
Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2 _temp4365= yyvs; struct
_xenum_struct** _temp4367= _temp4365.curr +( yyvsp_offset - 2); if( _temp4367 <
_temp4365.base? 1: _temp4367 >= _temp4365.last_plus_one){ _throw( Null_Exception);}*
_temp4367;})))->tms; _temp4364;}); _temp4363;})};( struct _xenum_struct*)
_temp4362;}); break; case 196: _LL4361: { struct Cyc_Core_Opt* eff=({ struct Cyc_Core_Opt*
_temp4371=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp4371->v=( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp4372=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp4372->tag= Cyc_Absyn_JoinEff_tag;
_temp4372->f1= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp4373= yyvs;
struct _xenum_struct** _temp4375= _temp4373.curr +( yyvsp_offset - 1); if(
_temp4375 < _temp4373.base? 1: _temp4375 >= _temp4373.last_plus_one){ _throw(
Null_Exception);}* _temp4375;}));( void*) _temp4372;}); _temp4371;}); yyval=({
struct Cyc_AbstractDeclarator_tok_struct* _temp4376=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));* _temp4376=(
struct Cyc_AbstractDeclarator_tok_struct){.tag= Cyc_AbstractDeclarator_tok_tag,.f1=({
struct Cyc_Parse_Abstractdeclarator* _temp4377=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4377->tms=({
struct Cyc_List_List* _temp4378=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4378->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4382=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4382->tag= Cyc_Absyn_Function_mod_tag; _temp4382->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4383=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4383->tag= Cyc_Absyn_WithTypes_tag; _temp4383->f1= 0; _temp4383->f2= 0;
_temp4383->f3=( struct Cyc_Core_Opt*) eff;( void*) _temp4383;});( void*)
_temp4382;}); _temp4378->tl=( Cyc_yyget_AbstractDeclarator_tok(({ struct
_tagged_ptr2 _temp4379= yyvs; struct _xenum_struct** _temp4381= _temp4379.curr +(
yyvsp_offset - 4); if( _temp4381 < _temp4379.base? 1: _temp4381 >= _temp4379.last_plus_one){
_throw( Null_Exception);}* _temp4381;})))->tms; _temp4378;}); _temp4377;})};(
struct _xenum_struct*) _temp4376;}); break;} case 197: _LL4370: { struct
_tuple14 _temp4390; struct Cyc_Core_Opt* _temp4391; int _temp4393; struct Cyc_List_List*
_temp4395; struct _tuple14* _temp4388= Cyc_yyget_ParamDeclListBool_tok(({ struct
_tagged_ptr2 _temp4385= yyvs; struct _xenum_struct** _temp4387= _temp4385.curr +(
yyvsp_offset - 1); if( _temp4387 < _temp4385.base? 1: _temp4387 >= _temp4385.last_plus_one){
_throw( Null_Exception);}* _temp4387;})); _temp4390=* _temp4388; _LL4396:
_temp4395= _temp4390.f1; goto _LL4394; _LL4394: _temp4393= _temp4390.f2; goto
_LL4392; _LL4392: _temp4391= _temp4390.f3; goto _LL4389; _LL4389: yyval=({
struct Cyc_AbstractDeclarator_tok_struct* _temp4397=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));* _temp4397=(
struct Cyc_AbstractDeclarator_tok_struct){.tag= Cyc_AbstractDeclarator_tok_tag,.f1=({
struct Cyc_Parse_Abstractdeclarator* _temp4398=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4398->tms=({
struct Cyc_List_List* _temp4399=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4399->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4403=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4403->tag= Cyc_Absyn_Function_mod_tag; _temp4403->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4404=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4404->tag= Cyc_Absyn_WithTypes_tag; _temp4404->f1= _temp4395; _temp4404->f2=
_temp4393; _temp4404->f3= _temp4391;( void*) _temp4404;});( void*) _temp4403;});
_temp4399->tl=( Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2
_temp4400= yyvs; struct _xenum_struct** _temp4402= _temp4400.curr +(
yyvsp_offset - 3); if( _temp4402 < _temp4400.base? 1: _temp4402 >= _temp4400.last_plus_one){
_throw( Null_Exception);}* _temp4402;})))->tms; _temp4399;}); _temp4398;})};(
struct _xenum_struct*) _temp4397;}); break;} case 198: _LL4384: { struct Cyc_List_List*
ts=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4406=
yyls; struct Cyc_Yyltype* _temp4408= _temp4406.curr +( yylsp_offset - 2); if(
_temp4408 < _temp4406.base? 1: _temp4408 >= _temp4406.last_plus_one){ _throw(
Null_Exception);}* _temp4408;}).first_line,({ struct _tagged_ptr3 _temp4409=
yyls; struct Cyc_Yyltype* _temp4411= _temp4409.curr + yylsp_offset; if(
_temp4411 < _temp4409.base? 1: _temp4411 >= _temp4409.last_plus_one){ _throw(
Null_Exception);}* _temp4411;}).last_line),(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp4412= yyvs; struct _xenum_struct** _temp4414= _temp4412.curr +(
yyvsp_offset - 1); if( _temp4414 < _temp4412.base? 1: _temp4414 >= _temp4412.last_plus_one){
_throw( Null_Exception);}* _temp4414;})))); yyval=({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4415=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct));* _temp4415=( struct Cyc_AbstractDeclarator_tok_struct){.tag=
Cyc_AbstractDeclarator_tok_tag,.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4416=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4416->tms=({ struct Cyc_List_List* _temp4417=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4417->hd=( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp4421=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp4421->tag= Cyc_Absyn_TypeParams_mod_tag;
_temp4421->f1= ts; _temp4421->f2= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4422= yyls; struct Cyc_Yyltype* _temp4424= _temp4422.curr +(
yylsp_offset - 2); if( _temp4424 < _temp4422.base? 1: _temp4424 >= _temp4422.last_plus_one){
_throw( Null_Exception);}* _temp4424;}).first_line,({ struct _tagged_ptr3
_temp4425= yyls; struct Cyc_Yyltype* _temp4427= _temp4425.curr + yylsp_offset;
if( _temp4427 < _temp4425.base? 1: _temp4427 >= _temp4425.last_plus_one){ _throw(
Null_Exception);}* _temp4427;}).last_line); _temp4421->f3= 0;( void*) _temp4421;});
_temp4417->tl=( Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2
_temp4418= yyvs; struct _xenum_struct** _temp4420= _temp4418.curr +(
yyvsp_offset - 3); if( _temp4420 < _temp4418.base? 1: _temp4420 >= _temp4418.last_plus_one){
_throw( Null_Exception);}* _temp4420;})))->tms; _temp4417;}); _temp4416;})};(
struct _xenum_struct*) _temp4415;}); break;} case 199: _LL4405:(( struct Cyc_Lexing_lexbuf*)
Cyc_Parse_lbuf->v)->lex_curr_pos -= 1;{ struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4429= yyls; struct Cyc_Yyltype* _temp4431= _temp4429.curr
+( yylsp_offset - 2); if( _temp4431 < _temp4429.base? 1: _temp4431 >= _temp4429.last_plus_one){
_throw( Null_Exception);}* _temp4431;}).first_line,({ struct _tagged_ptr3
_temp4432= yyls; struct Cyc_Yyltype* _temp4434= _temp4432.curr + yylsp_offset;
if( _temp4434 < _temp4432.base? 1: _temp4434 >= _temp4432.last_plus_one){ _throw(
Null_Exception);}* _temp4434;}).last_line),(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp4435= yyvs; struct _xenum_struct** _temp4437= _temp4435.curr +(
yyvsp_offset - 1); if( _temp4437 < _temp4435.base? 1: _temp4437 >= _temp4435.last_plus_one){
_throw( Null_Exception);}* _temp4437;})))); yyval=({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4438=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct));* _temp4438=( struct Cyc_AbstractDeclarator_tok_struct){.tag=
Cyc_AbstractDeclarator_tok_tag,.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4439=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4439->tms=({ struct Cyc_List_List* _temp4440=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4440->hd=( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp4444=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp4444->tag= Cyc_Absyn_TypeParams_mod_tag;
_temp4444->f1= ts; _temp4444->f2= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4445= yyls; struct Cyc_Yyltype* _temp4447= _temp4445.curr +(
yylsp_offset - 2); if( _temp4447 < _temp4445.base? 1: _temp4447 >= _temp4445.last_plus_one){
_throw( Null_Exception);}* _temp4447;}).first_line,({ struct _tagged_ptr3
_temp4448= yyls; struct Cyc_Yyltype* _temp4450= _temp4448.curr + yylsp_offset;
if( _temp4450 < _temp4448.base? 1: _temp4450 >= _temp4448.last_plus_one){ _throw(
Null_Exception);}* _temp4450;}).last_line); _temp4444->f3= 0;( void*) _temp4444;});
_temp4440->tl=( Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2
_temp4441= yyvs; struct _xenum_struct** _temp4443= _temp4441.curr +(
yyvsp_offset - 3); if( _temp4443 < _temp4441.base? 1: _temp4443 >= _temp4441.last_plus_one){
_throw( Null_Exception);}* _temp4443;})))->tms; _temp4440;}); _temp4439;})};(
struct _xenum_struct*) _temp4438;}); break;} case 200: _LL4428: yyval=({ struct
Cyc_AbstractDeclarator_tok_struct* _temp4452=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));* _temp4452=(
struct Cyc_AbstractDeclarator_tok_struct){.tag= Cyc_AbstractDeclarator_tok_tag,.f1=({
struct Cyc_Parse_Abstractdeclarator* _temp4453=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4453->tms=({
struct Cyc_List_List* _temp4454=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4454->hd=( void*)({ struct Cyc_Absyn_Attributes_mod_struct*
_temp4458=( struct Cyc_Absyn_Attributes_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Attributes_mod_struct)); _temp4458->tag= Cyc_Absyn_Attributes_mod_tag;
_temp4458->f1= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4459=
yyls; struct Cyc_Yyltype* _temp4461= _temp4459.curr + yylsp_offset; if(
_temp4461 < _temp4459.base? 1: _temp4461 >= _temp4459.last_plus_one){ _throw(
Null_Exception);}* _temp4461;}).first_line,({ struct _tagged_ptr3 _temp4462=
yyls; struct Cyc_Yyltype* _temp4464= _temp4462.curr + yylsp_offset; if(
_temp4464 < _temp4462.base? 1: _temp4464 >= _temp4462.last_plus_one){ _throw(
Null_Exception);}* _temp4464;}).last_line); _temp4458->f2= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp4465= yyvs; struct _xenum_struct** _temp4467= _temp4465.curr
+ yyvsp_offset; if( _temp4467 < _temp4465.base? 1: _temp4467 >= _temp4465.last_plus_one){
_throw( Null_Exception);}* _temp4467;}));( void*) _temp4458;}); _temp4454->tl=(
Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2 _temp4455= yyvs; struct
_xenum_struct** _temp4457= _temp4455.curr +( yyvsp_offset - 1); if( _temp4457 <
_temp4455.base? 1: _temp4457 >= _temp4455.last_plus_one){ _throw( Null_Exception);}*
_temp4457;})))->tms; _temp4454;}); _temp4453;})};( struct _xenum_struct*)
_temp4452;}); break; case 201: _LL4451: yyval=({ struct _tagged_ptr2 _temp4469=
yyvs; struct _xenum_struct** _temp4471= _temp4469.curr + yyvsp_offset; if(
_temp4471 < _temp4469.base? 1: _temp4471 >= _temp4469.last_plus_one){ _throw(
Null_Exception);}* _temp4471;}); break; case 202: _LL4468: yyval=({ struct
_tagged_ptr2 _temp4473= yyvs; struct _xenum_struct** _temp4475= _temp4473.curr +
yyvsp_offset; if( _temp4475 < _temp4473.base? 1: _temp4475 >= _temp4473.last_plus_one){
_throw( Null_Exception);}* _temp4475;}); break; case 203: _LL4472: yyval=({
struct _tagged_ptr2 _temp4477= yyvs; struct _xenum_struct** _temp4479= _temp4477.curr
+ yyvsp_offset; if( _temp4479 < _temp4477.base? 1: _temp4479 >= _temp4477.last_plus_one){
_throw( Null_Exception);}* _temp4479;}); break; case 204: _LL4476: yyval=({
struct _tagged_ptr2 _temp4481= yyvs; struct _xenum_struct** _temp4483= _temp4481.curr
+ yyvsp_offset; if( _temp4483 < _temp4481.base? 1: _temp4483 >= _temp4481.last_plus_one){
_throw( Null_Exception);}* _temp4483;}); break; case 205: _LL4480: yyval=({
struct _tagged_ptr2 _temp4485= yyvs; struct _xenum_struct** _temp4487= _temp4485.curr
+ yyvsp_offset; if( _temp4487 < _temp4485.base? 1: _temp4487 >= _temp4485.last_plus_one){
_throw( Null_Exception);}* _temp4487;}); break; case 206: _LL4484: yyval=({
struct _tagged_ptr2 _temp4489= yyvs; struct _xenum_struct** _temp4491= _temp4489.curr
+ yyvsp_offset; if( _temp4491 < _temp4489.base? 1: _temp4491 >= _temp4489.last_plus_one){
_throw( Null_Exception);}* _temp4491;}); break; case 207: _LL4488: yyval=({
struct Cyc_Stmt_tok_struct* _temp4493=( struct Cyc_Stmt_tok_struct*) GC_malloc(
sizeof( struct Cyc_Stmt_tok_struct));* _temp4493=( struct Cyc_Stmt_tok_struct){.tag=
Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_new_stmt(({ struct Cyc_Absyn_Cut_s_struct*
_temp4494=( struct Cyc_Absyn_Cut_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Cut_s_struct));
_temp4494->tag= Cyc_Absyn_Cut_s_tag; _temp4494->f1= Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp4495= yyvs; struct _xenum_struct** _temp4497= _temp4495.curr +
yyvsp_offset; if( _temp4497 < _temp4495.base? 1: _temp4497 >= _temp4495.last_plus_one){
_throw( Null_Exception);}* _temp4497;}));( void*) _temp4494;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4498= yyls; struct Cyc_Yyltype* _temp4500= _temp4498.curr
+( yylsp_offset - 1); if( _temp4500 < _temp4498.base? 1: _temp4500 >= _temp4498.last_plus_one){
_throw( Null_Exception);}* _temp4500;}).first_line,({ struct _tagged_ptr3
_temp4501= yyls; struct Cyc_Yyltype* _temp4503= _temp4501.curr + yylsp_offset;
if( _temp4503 < _temp4501.base? 1: _temp4503 >= _temp4501.last_plus_one){ _throw(
Null_Exception);}* _temp4503;}).last_line))};( struct _xenum_struct*) _temp4493;});
break; case 208: _LL4492: yyval=({ struct Cyc_Stmt_tok_struct* _temp4505=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4505=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_new_stmt(({
struct Cyc_Absyn_Splice_s_struct* _temp4506=( struct Cyc_Absyn_Splice_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Splice_s_struct)); _temp4506->tag= Cyc_Absyn_Splice_s_tag;
_temp4506->f1= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4507= yyvs; struct
_xenum_struct** _temp4509= _temp4507.curr + yyvsp_offset; if( _temp4509 <
_temp4507.base? 1: _temp4509 >= _temp4507.last_plus_one){ _throw( Null_Exception);}*
_temp4509;}));( void*) _temp4506;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4510= yyls; struct Cyc_Yyltype* _temp4512= _temp4510.curr +(
yylsp_offset - 1); if( _temp4512 < _temp4510.base? 1: _temp4512 >= _temp4510.last_plus_one){
_throw( Null_Exception);}* _temp4512;}).first_line,({ struct _tagged_ptr3
_temp4513= yyls; struct Cyc_Yyltype* _temp4515= _temp4513.curr + yylsp_offset;
if( _temp4515 < _temp4513.base? 1: _temp4515 >= _temp4513.last_plus_one){ _throw(
Null_Exception);}* _temp4515;}).last_line))};( struct _xenum_struct*) _temp4505;});
break; case 209: _LL4504: yyval=({ struct Cyc_Stmt_tok_struct* _temp4517=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4517=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_new_stmt(({
struct Cyc_Absyn_Label_s_struct* _temp4518=( struct Cyc_Absyn_Label_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Label_s_struct)); _temp4518->tag= Cyc_Absyn_Label_s_tag;
_temp4518->f1=({ struct _tagged_string* _temp4519=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp4519[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp4520= yyvs; struct _xenum_struct** _temp4522= _temp4520.curr
+( yyvsp_offset - 2); if( _temp4522 < _temp4520.base? 1: _temp4522 >= _temp4520.last_plus_one){
_throw( Null_Exception);}* _temp4522;})); _temp4519;}); _temp4518->f2= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp4523= yyvs; struct _xenum_struct** _temp4525= _temp4523.curr
+ yyvsp_offset; if( _temp4525 < _temp4523.base? 1: _temp4525 >= _temp4523.last_plus_one){
_throw( Null_Exception);}* _temp4525;}));( void*) _temp4518;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4526= yyls; struct Cyc_Yyltype* _temp4528= _temp4526.curr
+( yylsp_offset - 2); if( _temp4528 < _temp4526.base? 1: _temp4528 >= _temp4526.last_plus_one){
_throw( Null_Exception);}* _temp4528;}).first_line,({ struct _tagged_ptr3
_temp4529= yyls; struct Cyc_Yyltype* _temp4531= _temp4529.curr + yylsp_offset;
if( _temp4531 < _temp4529.base? 1: _temp4531 >= _temp4529.last_plus_one){ _throw(
Null_Exception);}* _temp4531;}).last_line))};( struct _xenum_struct*) _temp4517;});
break; case 210: _LL4516: yyval=({ struct Cyc_Stmt_tok_struct* _temp4533=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4533=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4534= yyls; struct Cyc_Yyltype*
_temp4536= _temp4534.curr + yylsp_offset; if( _temp4536 < _temp4534.base? 1:
_temp4536 >= _temp4534.last_plus_one){ _throw( Null_Exception);}* _temp4536;}).first_line,({
struct _tagged_ptr3 _temp4537= yyls; struct Cyc_Yyltype* _temp4539= _temp4537.curr
+ yylsp_offset; if( _temp4539 < _temp4537.base? 1: _temp4539 >= _temp4537.last_plus_one){
_throw( Null_Exception);}* _temp4539;}).last_line))};( struct _xenum_struct*)
_temp4533;}); break; case 211: _LL4532: yyval=({ struct Cyc_Stmt_tok_struct*
_temp4541=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4541=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_exp_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4542= yyvs; struct _xenum_struct**
_temp4544= _temp4542.curr +( yyvsp_offset - 1); if( _temp4544 < _temp4542.base?
1: _temp4544 >= _temp4542.last_plus_one){ _throw( Null_Exception);}* _temp4544;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4545= yyls; struct Cyc_Yyltype*
_temp4547= _temp4545.curr +( yylsp_offset - 1); if( _temp4547 < _temp4545.base?
1: _temp4547 >= _temp4545.last_plus_one){ _throw( Null_Exception);}* _temp4547;}).first_line,({
struct _tagged_ptr3 _temp4548= yyls; struct Cyc_Yyltype* _temp4550= _temp4548.curr
+ yylsp_offset; if( _temp4550 < _temp4548.base? 1: _temp4550 >= _temp4548.last_plus_one){
_throw( Null_Exception);}* _temp4550;}).last_line))};( struct _xenum_struct*)
_temp4541;}); break; case 212: _LL4540: yyval=({ struct Cyc_Stmt_tok_struct*
_temp4552=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4552=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4553= yyls; struct Cyc_Yyltype*
_temp4555= _temp4553.curr +( yylsp_offset - 1); if( _temp4555 < _temp4553.base?
1: _temp4555 >= _temp4553.last_plus_one){ _throw( Null_Exception);}* _temp4555;}).first_line,({
struct _tagged_ptr3 _temp4556= yyls; struct Cyc_Yyltype* _temp4558= _temp4556.curr
+ yylsp_offset; if( _temp4558 < _temp4556.base? 1: _temp4558 >= _temp4556.last_plus_one){
_throw( Null_Exception);}* _temp4558;}).last_line))};( struct _xenum_struct*)
_temp4552;}); break; case 213: _LL4551: yyval=({ struct _tagged_ptr2 _temp4560=
yyvs; struct _xenum_struct** _temp4562= _temp4560.curr +( yyvsp_offset - 1); if(
_temp4562 < _temp4560.base? 1: _temp4562 >= _temp4560.last_plus_one){ _throw(
Null_Exception);}* _temp4562;}); break; case 214: _LL4559:{ void* _temp4567= Cyc_yyget_BlockItem_tok(({
struct _tagged_ptr2 _temp4564= yyvs; struct _xenum_struct** _temp4566= _temp4564.curr
+ yyvsp_offset; if( _temp4566 < _temp4564.base? 1: _temp4566 >= _temp4564.last_plus_one){
_throw( Null_Exception);}* _temp4566;})); struct Cyc_List_List* _temp4575;
struct Cyc_Absyn_Fndecl* _temp4577; struct Cyc_Absyn_Stmt* _temp4579; _LL4569:
if((( struct _enum_struct*) _temp4567)->tag == Cyc_Parse_TopDecls_bl_tag){
_LL4576: _temp4575=( struct Cyc_List_List*)(( struct Cyc_Parse_TopDecls_bl_struct*)
_temp4567)->f1; goto _LL4570;} else{ goto _LL4571;} _LL4571: if((( struct
_enum_struct*) _temp4567)->tag == Cyc_Parse_FnDecl_bl_tag){ _LL4578: _temp4577=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Parse_FnDecl_bl_struct*) _temp4567)->f1;
goto _LL4572;} else{ goto _LL4573;} _LL4573: if((( struct _enum_struct*)
_temp4567)->tag == Cyc_Parse_Stmt_bl_tag){ _LL4580: _temp4579=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Parse_Stmt_bl_struct*) _temp4567)->f1; goto _LL4574;} else{ goto
_LL4568;} _LL4570: yyval=({ struct Cyc_Stmt_tok_struct* _temp4581=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));* _temp4581=( struct Cyc_Stmt_tok_struct){.tag=
Cyc_Stmt_tok_tag,.f1= Cyc_Parse_flatten_declarations( _temp4575, Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4582= yyls; struct Cyc_Yyltype*
_temp4584= _temp4582.curr + yylsp_offset; if( _temp4584 < _temp4582.base? 1:
_temp4584 >= _temp4582.last_plus_one){ _throw( Null_Exception);}* _temp4584;}).first_line,({
struct _tagged_ptr3 _temp4585= yyls; struct Cyc_Yyltype* _temp4587= _temp4585.curr
+ yylsp_offset; if( _temp4587 < _temp4585.base? 1: _temp4587 >= _temp4585.last_plus_one){
_throw( Null_Exception);}* _temp4587;}).last_line)))};( struct _xenum_struct*)
_temp4581;}); goto _LL4568; _LL4572: yyval=({ struct Cyc_Stmt_tok_struct*
_temp4588=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4588=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(({ struct Cyc_Absyn_Fn_d_struct* _temp4589=( struct Cyc_Absyn_Fn_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct)); _temp4589->tag= Cyc_Absyn_Fn_d_tag;
_temp4589->f1= _temp4577;( void*) _temp4589;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4590= yyls; struct Cyc_Yyltype* _temp4592= _temp4590.curr
+ yylsp_offset; if( _temp4592 < _temp4590.base? 1: _temp4592 >= _temp4590.last_plus_one){
_throw( Null_Exception);}* _temp4592;}).first_line,({ struct _tagged_ptr3
_temp4593= yyls; struct Cyc_Yyltype* _temp4595= _temp4593.curr + yylsp_offset;
if( _temp4595 < _temp4593.base? 1: _temp4595 >= _temp4593.last_plus_one){ _throw(
Null_Exception);}* _temp4595;}).last_line)), Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4596= yyls; struct Cyc_Yyltype* _temp4598= _temp4596.curr
+ yylsp_offset; if( _temp4598 < _temp4596.base? 1: _temp4598 >= _temp4596.last_plus_one){
_throw( Null_Exception);}* _temp4598;}).first_line,({ struct _tagged_ptr3
_temp4599= yyls; struct Cyc_Yyltype* _temp4601= _temp4599.curr + yylsp_offset;
if( _temp4601 < _temp4599.base? 1: _temp4601 >= _temp4599.last_plus_one){ _throw(
Null_Exception);}* _temp4601;}).last_line)))};( struct _xenum_struct*) _temp4588;});
goto _LL4568; _LL4574: yyval=({ struct Cyc_Stmt_tok_struct* _temp4602=( struct
Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4602=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= _temp4579};(
struct _xenum_struct*) _temp4602;}); goto _LL4568; _LL4568:;} break; case 215:
_LL4563:{ void* _temp4607= Cyc_yyget_BlockItem_tok(({ struct _tagged_ptr2
_temp4604= yyvs; struct _xenum_struct** _temp4606= _temp4604.curr +(
yyvsp_offset - 1); if( _temp4606 < _temp4604.base? 1: _temp4606 >= _temp4604.last_plus_one){
_throw( Null_Exception);}* _temp4606;})); struct Cyc_List_List* _temp4615;
struct Cyc_Absyn_Fndecl* _temp4617; struct Cyc_Absyn_Stmt* _temp4619; _LL4609:
if((( struct _enum_struct*) _temp4607)->tag == Cyc_Parse_TopDecls_bl_tag){
_LL4616: _temp4615=( struct Cyc_List_List*)(( struct Cyc_Parse_TopDecls_bl_struct*)
_temp4607)->f1; goto _LL4610;} else{ goto _LL4611;} _LL4611: if((( struct
_enum_struct*) _temp4607)->tag == Cyc_Parse_FnDecl_bl_tag){ _LL4618: _temp4617=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Parse_FnDecl_bl_struct*) _temp4607)->f1;
goto _LL4612;} else{ goto _LL4613;} _LL4613: if((( struct _enum_struct*)
_temp4607)->tag == Cyc_Parse_Stmt_bl_tag){ _LL4620: _temp4619=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Parse_Stmt_bl_struct*) _temp4607)->f1; goto _LL4614;} else{ goto
_LL4608;} _LL4610: yyval=({ struct Cyc_Stmt_tok_struct* _temp4621=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));* _temp4621=( struct Cyc_Stmt_tok_struct){.tag=
Cyc_Stmt_tok_tag,.f1= Cyc_Parse_flatten_declarations( _temp4615, Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp4622= yyvs; struct _xenum_struct** _temp4624= _temp4622.curr
+ yyvsp_offset; if( _temp4624 < _temp4622.base? 1: _temp4624 >= _temp4622.last_plus_one){
_throw( Null_Exception);}* _temp4624;})))};( struct _xenum_struct*) _temp4621;});
goto _LL4608; _LL4612: yyval=({ struct Cyc_Stmt_tok_struct* _temp4625=( struct
Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4625=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(({ struct Cyc_Absyn_Fn_d_struct* _temp4626=( struct Cyc_Absyn_Fn_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct)); _temp4626->tag= Cyc_Absyn_Fn_d_tag;
_temp4626->f1= _temp4617;( void*) _temp4626;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4627= yyls; struct Cyc_Yyltype* _temp4629= _temp4627.curr
+( yylsp_offset - 1); if( _temp4629 < _temp4627.base? 1: _temp4629 >= _temp4627.last_plus_one){
_throw( Null_Exception);}* _temp4629;}).first_line,({ struct _tagged_ptr3
_temp4630= yyls; struct Cyc_Yyltype* _temp4632= _temp4630.curr +( yylsp_offset -
1); if( _temp4632 < _temp4630.base? 1: _temp4632 >= _temp4630.last_plus_one){
_throw( Null_Exception);}* _temp4632;}).last_line)), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp4633= yyvs; struct _xenum_struct** _temp4635= _temp4633.curr
+ yyvsp_offset; if( _temp4635 < _temp4633.base? 1: _temp4635 >= _temp4633.last_plus_one){
_throw( Null_Exception);}* _temp4635;})))};( struct _xenum_struct*) _temp4625;});
goto _LL4608; _LL4614: yyval=({ struct Cyc_Stmt_tok_struct* _temp4636=( struct
Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4636=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_seq_stmt(
_temp4619, Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4637= yyvs; struct
_xenum_struct** _temp4639= _temp4637.curr + yyvsp_offset; if( _temp4639 <
_temp4637.base? 1: _temp4639 >= _temp4637.last_plus_one){ _throw( Null_Exception);}*
_temp4639;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4640=
yyls; struct Cyc_Yyltype* _temp4642= _temp4640.curr +( yylsp_offset - 1); if(
_temp4642 < _temp4640.base? 1: _temp4642 >= _temp4640.last_plus_one){ _throw(
Null_Exception);}* _temp4642;}).first_line,({ struct _tagged_ptr3 _temp4643=
yyls; struct Cyc_Yyltype* _temp4645= _temp4643.curr + yylsp_offset; if(
_temp4645 < _temp4643.base? 1: _temp4645 >= _temp4643.last_plus_one){ _throw(
Null_Exception);}* _temp4645;}).last_line))};( struct _xenum_struct*) _temp4636;});
goto _LL4608; _LL4608:;} break; case 216: _LL4603: yyval=({ struct Cyc_BlockItem_tok_struct*
_temp4647=( struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));*
_temp4647=( struct Cyc_BlockItem_tok_struct){.tag= Cyc_BlockItem_tok_tag,.f1=({
struct Cyc_Parse_TopDecls_bl_struct* _temp4648=( struct Cyc_Parse_TopDecls_bl_struct*)
GC_malloc( sizeof( struct Cyc_Parse_TopDecls_bl_struct)); _temp4648->tag= Cyc_Parse_TopDecls_bl_tag;
_temp4648->f1= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp4649= yyvs;
struct _xenum_struct** _temp4651= _temp4649.curr + yyvsp_offset; if( _temp4651 <
_temp4649.base? 1: _temp4651 >= _temp4649.last_plus_one){ _throw( Null_Exception);}*
_temp4651;}));( void*) _temp4648;})};( struct _xenum_struct*) _temp4647;});
break; case 217: _LL4646: yyval=({ struct Cyc_BlockItem_tok_struct* _temp4653=(
struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));*
_temp4653=( struct Cyc_BlockItem_tok_struct){.tag= Cyc_BlockItem_tok_tag,.f1=({
struct Cyc_Parse_Stmt_bl_struct* _temp4654=( struct Cyc_Parse_Stmt_bl_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Stmt_bl_struct)); _temp4654->tag= Cyc_Parse_Stmt_bl_tag;
_temp4654->f1= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4655= yyvs; struct
_xenum_struct** _temp4657= _temp4655.curr + yyvsp_offset; if( _temp4657 <
_temp4655.base? 1: _temp4657 >= _temp4655.last_plus_one){ _throw( Null_Exception);}*
_temp4657;}));( void*) _temp4654;})};( struct _xenum_struct*) _temp4653;});
break; case 218: _LL4652: yyval=({ struct Cyc_BlockItem_tok_struct* _temp4659=(
struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));*
_temp4659=( struct Cyc_BlockItem_tok_struct){.tag= Cyc_BlockItem_tok_tag,.f1=({
struct Cyc_Parse_FnDecl_bl_struct* _temp4660=( struct Cyc_Parse_FnDecl_bl_struct*)
GC_malloc( sizeof( struct Cyc_Parse_FnDecl_bl_struct)); _temp4660->tag= Cyc_Parse_FnDecl_bl_tag;
_temp4660->f1= Cyc_yyget_FnDecl_tok(({ struct _tagged_ptr2 _temp4661= yyvs;
struct _xenum_struct** _temp4663= _temp4661.curr + yyvsp_offset; if( _temp4663 <
_temp4661.base? 1: _temp4663 >= _temp4661.last_plus_one){ _throw( Null_Exception);}*
_temp4663;}));( void*) _temp4660;})};( struct _xenum_struct*) _temp4659;});
break; case 219: _LL4658: yyval=({ struct Cyc_Stmt_tok_struct* _temp4665=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4665=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4666= yyvs; struct _xenum_struct**
_temp4668= _temp4666.curr +( yyvsp_offset - 2); if( _temp4668 < _temp4666.base?
1: _temp4668 >= _temp4666.last_plus_one){ _throw( Null_Exception);}* _temp4668;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4669= yyvs; struct _xenum_struct**
_temp4671= _temp4669.curr + yyvsp_offset; if( _temp4671 < _temp4669.base? 1:
_temp4671 >= _temp4669.last_plus_one){ _throw( Null_Exception);}* _temp4671;})),
Cyc_Absyn_skip_stmt( 0), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4672= yyls; struct Cyc_Yyltype* _temp4674= _temp4672.curr +( yylsp_offset -
4); if( _temp4674 < _temp4672.base? 1: _temp4674 >= _temp4672.last_plus_one){
_throw( Null_Exception);}* _temp4674;}).first_line,({ struct _tagged_ptr3
_temp4675= yyls; struct Cyc_Yyltype* _temp4677= _temp4675.curr + yylsp_offset;
if( _temp4677 < _temp4675.base? 1: _temp4677 >= _temp4675.last_plus_one){ _throw(
Null_Exception);}* _temp4677;}).last_line))};( struct _xenum_struct*) _temp4665;});
break; case 220: _LL4664: yyval=({ struct Cyc_Stmt_tok_struct* _temp4679=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4679=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4680= yyvs; struct _xenum_struct**
_temp4682= _temp4680.curr +( yyvsp_offset - 4); if( _temp4682 < _temp4680.base?
1: _temp4682 >= _temp4680.last_plus_one){ _throw( Null_Exception);}* _temp4682;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4683= yyvs; struct _xenum_struct**
_temp4685= _temp4683.curr +( yyvsp_offset - 2); if( _temp4685 < _temp4683.base?
1: _temp4685 >= _temp4683.last_plus_one){ _throw( Null_Exception);}* _temp4685;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4686= yyvs; struct _xenum_struct**
_temp4688= _temp4686.curr + yyvsp_offset; if( _temp4688 < _temp4686.base? 1:
_temp4688 >= _temp4686.last_plus_one){ _throw( Null_Exception);}* _temp4688;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4689= yyls; struct Cyc_Yyltype*
_temp4691= _temp4689.curr +( yylsp_offset - 6); if( _temp4691 < _temp4689.base?
1: _temp4691 >= _temp4689.last_plus_one){ _throw( Null_Exception);}* _temp4691;}).first_line,({
struct _tagged_ptr3 _temp4692= yyls; struct Cyc_Yyltype* _temp4694= _temp4692.curr
+ yylsp_offset; if( _temp4694 < _temp4692.base? 1: _temp4694 >= _temp4692.last_plus_one){
_throw( Null_Exception);}* _temp4694;}).last_line))};( struct _xenum_struct*)
_temp4679;}); break; case 221: _LL4678: yyval=({ struct Cyc_Stmt_tok_struct*
_temp4696=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4696=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_switch_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4697= yyvs; struct _xenum_struct**
_temp4699= _temp4697.curr +( yyvsp_offset - 4); if( _temp4699 < _temp4697.base?
1: _temp4699 >= _temp4697.last_plus_one){ _throw( Null_Exception);}* _temp4699;})),
Cyc_yyget_SwitchClauseList_tok(({ struct _tagged_ptr2 _temp4700= yyvs; struct
_xenum_struct** _temp4702= _temp4700.curr +( yyvsp_offset - 1); if( _temp4702 <
_temp4700.base? 1: _temp4702 >= _temp4700.last_plus_one){ _throw( Null_Exception);}*
_temp4702;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4703=
yyls; struct Cyc_Yyltype* _temp4705= _temp4703.curr +( yylsp_offset - 6); if(
_temp4705 < _temp4703.base? 1: _temp4705 >= _temp4703.last_plus_one){ _throw(
Null_Exception);}* _temp4705;}).first_line,({ struct _tagged_ptr3 _temp4706=
yyls; struct Cyc_Yyltype* _temp4708= _temp4706.curr + yylsp_offset; if(
_temp4708 < _temp4706.base? 1: _temp4708 >= _temp4706.last_plus_one){ _throw(
Null_Exception);}* _temp4708;}).last_line))};( struct _xenum_struct*) _temp4696;});
break; case 222: _LL4695: yyval=({ struct Cyc_Stmt_tok_struct* _temp4710=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4710=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_trycatch_stmt(
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4711= yyvs; struct _xenum_struct**
_temp4713= _temp4711.curr +( yyvsp_offset - 4); if( _temp4713 < _temp4711.base?
1: _temp4713 >= _temp4711.last_plus_one){ _throw( Null_Exception);}* _temp4713;})),
Cyc_yyget_SwitchClauseList_tok(({ struct _tagged_ptr2 _temp4714= yyvs; struct
_xenum_struct** _temp4716= _temp4714.curr +( yyvsp_offset - 1); if( _temp4716 <
_temp4714.base? 1: _temp4716 >= _temp4714.last_plus_one){ _throw( Null_Exception);}*
_temp4716;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4717=
yyls; struct Cyc_Yyltype* _temp4719= _temp4717.curr +( yylsp_offset - 5); if(
_temp4719 < _temp4717.base? 1: _temp4719 >= _temp4717.last_plus_one){ _throw(
Null_Exception);}* _temp4719;}).first_line,({ struct _tagged_ptr3 _temp4720=
yyls; struct Cyc_Yyltype* _temp4722= _temp4720.curr + yylsp_offset; if(
_temp4722 < _temp4720.base? 1: _temp4722 >= _temp4720.last_plus_one){ _throw(
Null_Exception);}* _temp4722;}).last_line))};( struct _xenum_struct*) _temp4710;});
break; case 223: _LL4709: yyval=({ struct Cyc_SwitchClauseList_tok_struct*
_temp4724=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct));* _temp4724=( struct Cyc_SwitchClauseList_tok_struct){.tag=
Cyc_SwitchClauseList_tok_tag,.f1= 0};( struct _xenum_struct*) _temp4724;});
break; case 224: _LL4723: yyval=({ struct Cyc_SwitchClauseList_tok_struct*
_temp4726=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct));* _temp4726=( struct Cyc_SwitchClauseList_tok_struct){.tag=
Cyc_SwitchClauseList_tok_tag,.f1=({ struct Cyc_List_List* _temp4727=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4727->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp4728=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp4728->pattern= Cyc_Absyn_new_pat( Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4738= yyls; struct Cyc_Yyltype* _temp4740= _temp4738.curr
+( yylsp_offset - 2); if( _temp4740 < _temp4738.base? 1: _temp4740 >= _temp4738.last_plus_one){
_throw( Null_Exception);}* _temp4740;}).first_line,({ struct _tagged_ptr3
_temp4741= yyls; struct Cyc_Yyltype* _temp4743= _temp4741.curr +( yylsp_offset -
2); if( _temp4743 < _temp4741.base? 1: _temp4743 >= _temp4741.last_plus_one){
_throw( Null_Exception);}* _temp4743;}).last_line)); _temp4728->pat_vars= 0;
_temp4728->where_clause= 0; _temp4728->body= Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp4735= yyvs; struct _xenum_struct** _temp4737= _temp4735.curr +
yyvsp_offset; if( _temp4737 < _temp4735.base? 1: _temp4737 >= _temp4735.last_plus_one){
_throw( Null_Exception);}* _temp4737;})); _temp4728->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4729= yyls; struct Cyc_Yyltype* _temp4731= _temp4729.curr
+( yylsp_offset - 2); if( _temp4731 < _temp4729.base? 1: _temp4731 >= _temp4729.last_plus_one){
_throw( Null_Exception);}* _temp4731;}).first_line,({ struct _tagged_ptr3
_temp4732= yyls; struct Cyc_Yyltype* _temp4734= _temp4732.curr + yylsp_offset;
if( _temp4734 < _temp4732.base? 1: _temp4734 >= _temp4732.last_plus_one){ _throw(
Null_Exception);}* _temp4734;}).last_line); _temp4728;}); _temp4727->tl= 0;
_temp4727;})};( struct _xenum_struct*) _temp4726;}); break; case 225: _LL4725:
yyval=({ struct Cyc_SwitchClauseList_tok_struct* _temp4745=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));* _temp4745=( struct
Cyc_SwitchClauseList_tok_struct){.tag= Cyc_SwitchClauseList_tok_tag,.f1=({
struct Cyc_List_List* _temp4746=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4746->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp4750=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp4750->pattern= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp4763= yyvs;
struct _xenum_struct** _temp4765= _temp4763.curr +( yyvsp_offset - 2); if(
_temp4765 < _temp4763.base? 1: _temp4765 >= _temp4763.last_plus_one){ _throw(
Null_Exception);}* _temp4765;})); _temp4750->pat_vars= 0; _temp4750->where_clause=
0; _temp4750->body= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4757= yyls; struct Cyc_Yyltype* _temp4759= _temp4757.curr +(
yylsp_offset - 1); if( _temp4759 < _temp4757.base? 1: _temp4759 >= _temp4757.last_plus_one){
_throw( Null_Exception);}* _temp4759;}).first_line,({ struct _tagged_ptr3
_temp4760= yyls; struct Cyc_Yyltype* _temp4762= _temp4760.curr +( yylsp_offset -
1); if( _temp4762 < _temp4760.base? 1: _temp4762 >= _temp4760.last_plus_one){
_throw( Null_Exception);}* _temp4762;}).last_line)); _temp4750->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4751= yyls; struct Cyc_Yyltype* _temp4753= _temp4751.curr
+( yylsp_offset - 3); if( _temp4753 < _temp4751.base? 1: _temp4753 >= _temp4751.last_plus_one){
_throw( Null_Exception);}* _temp4753;}).first_line,({ struct _tagged_ptr3
_temp4754= yyls; struct Cyc_Yyltype* _temp4756= _temp4754.curr + yylsp_offset;
if( _temp4756 < _temp4754.base? 1: _temp4756 >= _temp4754.last_plus_one){ _throw(
Null_Exception);}* _temp4756;}).last_line); _temp4750;}); _temp4746->tl= Cyc_yyget_SwitchClauseList_tok(({
struct _tagged_ptr2 _temp4747= yyvs; struct _xenum_struct** _temp4749= _temp4747.curr
+ yyvsp_offset; if( _temp4749 < _temp4747.base? 1: _temp4749 >= _temp4747.last_plus_one){
_throw( Null_Exception);}* _temp4749;})); _temp4746;})};( struct _xenum_struct*)
_temp4745;}); break; case 226: _LL4744: yyval=({ struct Cyc_SwitchClauseList_tok_struct*
_temp4767=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct));* _temp4767=( struct Cyc_SwitchClauseList_tok_struct){.tag=
Cyc_SwitchClauseList_tok_tag,.f1=({ struct Cyc_List_List* _temp4768=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4768->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp4772=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp4772->pattern= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp4782= yyvs;
struct _xenum_struct** _temp4784= _temp4782.curr +( yyvsp_offset - 3); if(
_temp4784 < _temp4782.base? 1: _temp4784 >= _temp4782.last_plus_one){ _throw(
Null_Exception);}* _temp4784;})); _temp4772->pat_vars= 0; _temp4772->where_clause=
0; _temp4772->body= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4779= yyvs;
struct _xenum_struct** _temp4781= _temp4779.curr +( yyvsp_offset - 1); if(
_temp4781 < _temp4779.base? 1: _temp4781 >= _temp4779.last_plus_one){ _throw(
Null_Exception);}* _temp4781;})); _temp4772->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4773= yyls; struct Cyc_Yyltype* _temp4775= _temp4773.curr
+( yylsp_offset - 4); if( _temp4775 < _temp4773.base? 1: _temp4775 >= _temp4773.last_plus_one){
_throw( Null_Exception);}* _temp4775;}).first_line,({ struct _tagged_ptr3
_temp4776= yyls; struct Cyc_Yyltype* _temp4778= _temp4776.curr +( yylsp_offset -
1); if( _temp4778 < _temp4776.base? 1: _temp4778 >= _temp4776.last_plus_one){
_throw( Null_Exception);}* _temp4778;}).last_line); _temp4772;}); _temp4768->tl=
Cyc_yyget_SwitchClauseList_tok(({ struct _tagged_ptr2 _temp4769= yyvs; struct
_xenum_struct** _temp4771= _temp4769.curr + yyvsp_offset; if( _temp4771 <
_temp4769.base? 1: _temp4771 >= _temp4769.last_plus_one){ _throw( Null_Exception);}*
_temp4771;})); _temp4768;})};( struct _xenum_struct*) _temp4767;}); break; case
227: _LL4766: yyval=({ struct Cyc_SwitchClauseList_tok_struct* _temp4786=(
struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));*
_temp4786=( struct Cyc_SwitchClauseList_tok_struct){.tag= Cyc_SwitchClauseList_tok_tag,.f1=({
struct Cyc_List_List* _temp4787=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4787->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp4791=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp4791->pattern= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp4807= yyvs;
struct _xenum_struct** _temp4809= _temp4807.curr +( yyvsp_offset - 4); if(
_temp4809 < _temp4807.base? 1: _temp4809 >= _temp4807.last_plus_one){ _throw(
Null_Exception);}* _temp4809;})); _temp4791->pat_vars= 0; _temp4791->where_clause=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4804= yyvs;
struct _xenum_struct** _temp4806= _temp4804.curr +( yyvsp_offset - 2); if(
_temp4806 < _temp4804.base? 1: _temp4806 >= _temp4804.last_plus_one){ _throw(
Null_Exception);}* _temp4806;})); _temp4791->body= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4798= yyls; struct Cyc_Yyltype* _temp4800= _temp4798.curr
+( yylsp_offset - 1); if( _temp4800 < _temp4798.base? 1: _temp4800 >= _temp4798.last_plus_one){
_throw( Null_Exception);}* _temp4800;}).first_line,({ struct _tagged_ptr3
_temp4801= yyls; struct Cyc_Yyltype* _temp4803= _temp4801.curr +( yylsp_offset -
1); if( _temp4803 < _temp4801.base? 1: _temp4803 >= _temp4801.last_plus_one){
_throw( Null_Exception);}* _temp4803;}).last_line)); _temp4791->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4792= yyls; struct Cyc_Yyltype* _temp4794= _temp4792.curr
+( yylsp_offset - 5); if( _temp4794 < _temp4792.base? 1: _temp4794 >= _temp4792.last_plus_one){
_throw( Null_Exception);}* _temp4794;}).first_line,({ struct _tagged_ptr3
_temp4795= yyls; struct Cyc_Yyltype* _temp4797= _temp4795.curr + yylsp_offset;
if( _temp4797 < _temp4795.base? 1: _temp4797 >= _temp4795.last_plus_one){ _throw(
Null_Exception);}* _temp4797;}).last_line); _temp4791;}); _temp4787->tl= Cyc_yyget_SwitchClauseList_tok(({
struct _tagged_ptr2 _temp4788= yyvs; struct _xenum_struct** _temp4790= _temp4788.curr
+ yyvsp_offset; if( _temp4790 < _temp4788.base? 1: _temp4790 >= _temp4788.last_plus_one){
_throw( Null_Exception);}* _temp4790;})); _temp4787;})};( struct _xenum_struct*)
_temp4786;}); break; case 228: _LL4785: yyval=({ struct Cyc_SwitchClauseList_tok_struct*
_temp4811=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct));* _temp4811=( struct Cyc_SwitchClauseList_tok_struct){.tag=
Cyc_SwitchClauseList_tok_tag,.f1=({ struct Cyc_List_List* _temp4812=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4812->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp4816=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp4816->pattern= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp4829= yyvs;
struct _xenum_struct** _temp4831= _temp4829.curr +( yyvsp_offset - 5); if(
_temp4831 < _temp4829.base? 1: _temp4831 >= _temp4829.last_plus_one){ _throw(
Null_Exception);}* _temp4831;})); _temp4816->pat_vars= 0; _temp4816->where_clause=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4826= yyvs;
struct _xenum_struct** _temp4828= _temp4826.curr +( yyvsp_offset - 3); if(
_temp4828 < _temp4826.base? 1: _temp4828 >= _temp4826.last_plus_one){ _throw(
Null_Exception);}* _temp4828;})); _temp4816->body= Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp4823= yyvs; struct _xenum_struct** _temp4825= _temp4823.curr +(
yyvsp_offset - 1); if( _temp4825 < _temp4823.base? 1: _temp4825 >= _temp4823.last_plus_one){
_throw( Null_Exception);}* _temp4825;})); _temp4816->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4817= yyls; struct Cyc_Yyltype* _temp4819= _temp4817.curr
+( yylsp_offset - 6); if( _temp4819 < _temp4817.base? 1: _temp4819 >= _temp4817.last_plus_one){
_throw( Null_Exception);}* _temp4819;}).first_line,({ struct _tagged_ptr3
_temp4820= yyls; struct Cyc_Yyltype* _temp4822= _temp4820.curr + yylsp_offset;
if( _temp4822 < _temp4820.base? 1: _temp4822 >= _temp4820.last_plus_one){ _throw(
Null_Exception);}* _temp4822;}).last_line); _temp4816;}); _temp4812->tl= Cyc_yyget_SwitchClauseList_tok(({
struct _tagged_ptr2 _temp4813= yyvs; struct _xenum_struct** _temp4815= _temp4813.curr
+ yyvsp_offset; if( _temp4815 < _temp4813.base? 1: _temp4815 >= _temp4813.last_plus_one){
_throw( Null_Exception);}* _temp4815;})); _temp4812;})};( struct _xenum_struct*)
_temp4811;}); break; case 229: _LL4810: yyval=({ struct Cyc_Stmt_tok_struct*
_temp4833=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4833=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_while_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4834= yyvs; struct _xenum_struct**
_temp4836= _temp4834.curr +( yyvsp_offset - 2); if( _temp4836 < _temp4834.base?
1: _temp4836 >= _temp4834.last_plus_one){ _throw( Null_Exception);}* _temp4836;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4837= yyvs; struct _xenum_struct**
_temp4839= _temp4837.curr + yyvsp_offset; if( _temp4839 < _temp4837.base? 1:
_temp4839 >= _temp4837.last_plus_one){ _throw( Null_Exception);}* _temp4839;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4840= yyls; struct Cyc_Yyltype*
_temp4842= _temp4840.curr +( yylsp_offset - 4); if( _temp4842 < _temp4840.base?
1: _temp4842 >= _temp4840.last_plus_one){ _throw( Null_Exception);}* _temp4842;}).first_line,({
struct _tagged_ptr3 _temp4843= yyls; struct Cyc_Yyltype* _temp4845= _temp4843.curr
+ yylsp_offset; if( _temp4845 < _temp4843.base? 1: _temp4845 >= _temp4843.last_plus_one){
_throw( Null_Exception);}* _temp4845;}).last_line))};( struct _xenum_struct*)
_temp4833;}); break; case 230: _LL4832: yyval=({ struct Cyc_Stmt_tok_struct*
_temp4847=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4847=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_do_stmt(
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4848= yyvs; struct _xenum_struct**
_temp4850= _temp4848.curr +( yyvsp_offset - 5); if( _temp4850 < _temp4848.base?
1: _temp4850 >= _temp4848.last_plus_one){ _throw( Null_Exception);}* _temp4850;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4851= yyvs; struct _xenum_struct**
_temp4853= _temp4851.curr +( yyvsp_offset - 2); if( _temp4853 < _temp4851.base?
1: _temp4853 >= _temp4851.last_plus_one){ _throw( Null_Exception);}* _temp4853;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4854= yyls; struct Cyc_Yyltype*
_temp4856= _temp4854.curr +( yylsp_offset - 6); if( _temp4856 < _temp4854.base?
1: _temp4856 >= _temp4854.last_plus_one){ _throw( Null_Exception);}* _temp4856;}).first_line,({
struct _tagged_ptr3 _temp4857= yyls; struct Cyc_Yyltype* _temp4859= _temp4857.curr
+ yylsp_offset; if( _temp4859 < _temp4857.base? 1: _temp4859 >= _temp4857.last_plus_one){
_throw( Null_Exception);}* _temp4859;}).last_line))};( struct _xenum_struct*)
_temp4847;}); break; case 231: _LL4846: yyval=({ struct Cyc_Stmt_tok_struct*
_temp4861=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4861=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp4862= yyvs; struct _xenum_struct** _temp4864= _temp4862.curr
+ yyvsp_offset; if( _temp4864 < _temp4862.base? 1: _temp4864 >= _temp4862.last_plus_one){
_throw( Null_Exception);}* _temp4864;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4865= yyls; struct Cyc_Yyltype* _temp4867= _temp4865.curr +(
yylsp_offset - 5); if( _temp4867 < _temp4865.base? 1: _temp4867 >= _temp4865.last_plus_one){
_throw( Null_Exception);}* _temp4867;}).first_line,({ struct _tagged_ptr3
_temp4868= yyls; struct Cyc_Yyltype* _temp4870= _temp4868.curr + yylsp_offset;
if( _temp4870 < _temp4868.base? 1: _temp4870 >= _temp4868.last_plus_one){ _throw(
Null_Exception);}* _temp4870;}).last_line))};( struct _xenum_struct*) _temp4861;});
break; case 232: _LL4860: yyval=({ struct Cyc_Stmt_tok_struct* _temp4872=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4872=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4873= yyvs; struct _xenum_struct** _temp4875= _temp4873.curr +(
yyvsp_offset - 2); if( _temp4875 < _temp4873.base? 1: _temp4875 >= _temp4873.last_plus_one){
_throw( Null_Exception);}* _temp4875;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp4876= yyvs; struct _xenum_struct** _temp4878= _temp4876.curr +
yyvsp_offset; if( _temp4878 < _temp4876.base? 1: _temp4878 >= _temp4876.last_plus_one){
_throw( Null_Exception);}* _temp4878;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4879= yyls; struct Cyc_Yyltype* _temp4881= _temp4879.curr +(
yylsp_offset - 6); if( _temp4881 < _temp4879.base? 1: _temp4881 >= _temp4879.last_plus_one){
_throw( Null_Exception);}* _temp4881;}).first_line,({ struct _tagged_ptr3
_temp4882= yyls; struct Cyc_Yyltype* _temp4884= _temp4882.curr + yylsp_offset;
if( _temp4884 < _temp4882.base? 1: _temp4884 >= _temp4882.last_plus_one){ _throw(
Null_Exception);}* _temp4884;}).last_line))};( struct _xenum_struct*) _temp4872;});
break; case 233: _LL4871: yyval=({ struct Cyc_Stmt_tok_struct* _temp4886=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4886=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4887=
yyvs; struct _xenum_struct** _temp4889= _temp4887.curr +( yyvsp_offset - 3); if(
_temp4889 < _temp4887.base? 1: _temp4889 >= _temp4887.last_plus_one){ _throw(
Null_Exception);}* _temp4889;})), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp4890= yyvs; struct _xenum_struct** _temp4892= _temp4890.curr
+ yyvsp_offset; if( _temp4892 < _temp4890.base? 1: _temp4892 >= _temp4890.last_plus_one){
_throw( Null_Exception);}* _temp4892;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4893= yyls; struct Cyc_Yyltype* _temp4895= _temp4893.curr +(
yylsp_offset - 6); if( _temp4895 < _temp4893.base? 1: _temp4895 >= _temp4893.last_plus_one){
_throw( Null_Exception);}* _temp4895;}).first_line,({ struct _tagged_ptr3
_temp4896= yyls; struct Cyc_Yyltype* _temp4898= _temp4896.curr + yylsp_offset;
if( _temp4898 < _temp4896.base? 1: _temp4898 >= _temp4896.last_plus_one){ _throw(
Null_Exception);}* _temp4898;}).last_line))};( struct _xenum_struct*) _temp4886;});
break; case 234: _LL4885: yyval=({ struct Cyc_Stmt_tok_struct* _temp4900=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4900=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4901=
yyvs; struct _xenum_struct** _temp4903= _temp4901.curr +( yyvsp_offset - 4); if(
_temp4903 < _temp4901.base? 1: _temp4903 >= _temp4901.last_plus_one){ _throw(
Null_Exception);}* _temp4903;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp4904= yyvs; struct _xenum_struct** _temp4906= _temp4904.curr +(
yyvsp_offset - 2); if( _temp4906 < _temp4904.base? 1: _temp4906 >= _temp4904.last_plus_one){
_throw( Null_Exception);}* _temp4906;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp4907= yyvs; struct _xenum_struct** _temp4909= _temp4907.curr +
yyvsp_offset; if( _temp4909 < _temp4907.base? 1: _temp4909 >= _temp4907.last_plus_one){
_throw( Null_Exception);}* _temp4909;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4910= yyls; struct Cyc_Yyltype* _temp4912= _temp4910.curr +(
yylsp_offset - 7); if( _temp4912 < _temp4910.base? 1: _temp4912 >= _temp4910.last_plus_one){
_throw( Null_Exception);}* _temp4912;}).first_line,({ struct _tagged_ptr3
_temp4913= yyls; struct Cyc_Yyltype* _temp4915= _temp4913.curr +( yylsp_offset -
1); if( _temp4915 < _temp4913.base? 1: _temp4915 >= _temp4913.last_plus_one){
_throw( Null_Exception);}* _temp4915;}).last_line))};( struct _xenum_struct*)
_temp4900;}); break; case 235: _LL4899: yyval=({ struct Cyc_Stmt_tok_struct*
_temp4917=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4917=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_for_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4918= yyvs; struct _xenum_struct**
_temp4920= _temp4918.curr +( yyvsp_offset - 4); if( _temp4920 < _temp4918.base?
1: _temp4920 >= _temp4918.last_plus_one){ _throw( Null_Exception);}* _temp4920;})),
Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp4921= yyvs; struct _xenum_struct** _temp4923= _temp4921.curr +
yyvsp_offset; if( _temp4923 < _temp4921.base? 1: _temp4923 >= _temp4921.last_plus_one){
_throw( Null_Exception);}* _temp4923;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4924= yyls; struct Cyc_Yyltype* _temp4926= _temp4924.curr +(
yylsp_offset - 6); if( _temp4926 < _temp4924.base? 1: _temp4926 >= _temp4924.last_plus_one){
_throw( Null_Exception);}* _temp4926;}).first_line,({ struct _tagged_ptr3
_temp4927= yyls; struct Cyc_Yyltype* _temp4929= _temp4927.curr + yylsp_offset;
if( _temp4929 < _temp4927.base? 1: _temp4929 >= _temp4927.last_plus_one){ _throw(
Null_Exception);}* _temp4929;}).last_line))};( struct _xenum_struct*) _temp4917;});
break; case 236: _LL4916: yyval=({ struct Cyc_Stmt_tok_struct* _temp4931=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4931=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_for_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4932= yyvs; struct _xenum_struct**
_temp4934= _temp4932.curr +( yyvsp_offset - 5); if( _temp4934 < _temp4932.base?
1: _temp4934 >= _temp4932.last_plus_one){ _throw( Null_Exception);}* _temp4934;})),
Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4935= yyvs;
struct _xenum_struct** _temp4937= _temp4935.curr +( yyvsp_offset - 2); if(
_temp4937 < _temp4935.base? 1: _temp4937 >= _temp4935.last_plus_one){ _throw(
Null_Exception);}* _temp4937;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp4938= yyvs; struct _xenum_struct** _temp4940= _temp4938.curr + yyvsp_offset;
if( _temp4940 < _temp4938.base? 1: _temp4940 >= _temp4938.last_plus_one){ _throw(
Null_Exception);}* _temp4940;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4941= yyls; struct Cyc_Yyltype* _temp4943= _temp4941.curr +(
yylsp_offset - 7); if( _temp4943 < _temp4941.base? 1: _temp4943 >= _temp4941.last_plus_one){
_throw( Null_Exception);}* _temp4943;}).first_line,({ struct _tagged_ptr3
_temp4944= yyls; struct Cyc_Yyltype* _temp4946= _temp4944.curr + yylsp_offset;
if( _temp4946 < _temp4944.base? 1: _temp4946 >= _temp4944.last_plus_one){ _throw(
Null_Exception);}* _temp4946;}).last_line))};( struct _xenum_struct*) _temp4931;});
break; case 237: _LL4930: yyval=({ struct Cyc_Stmt_tok_struct* _temp4948=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4948=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_for_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4949= yyvs; struct _xenum_struct**
_temp4951= _temp4949.curr +( yyvsp_offset - 5); if( _temp4951 < _temp4949.base?
1: _temp4951 >= _temp4949.last_plus_one){ _throw( Null_Exception);}* _temp4951;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4952= yyvs; struct _xenum_struct**
_temp4954= _temp4952.curr +( yyvsp_offset - 3); if( _temp4954 < _temp4952.base?
1: _temp4954 >= _temp4952.last_plus_one){ _throw( Null_Exception);}* _temp4954;})),
Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4955=
yyvs; struct _xenum_struct** _temp4957= _temp4955.curr + yyvsp_offset; if(
_temp4957 < _temp4955.base? 1: _temp4957 >= _temp4955.last_plus_one){ _throw(
Null_Exception);}* _temp4957;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4958= yyls; struct Cyc_Yyltype* _temp4960= _temp4958.curr +(
yylsp_offset - 7); if( _temp4960 < _temp4958.base? 1: _temp4960 >= _temp4958.last_plus_one){
_throw( Null_Exception);}* _temp4960;}).first_line,({ struct _tagged_ptr3
_temp4961= yyls; struct Cyc_Yyltype* _temp4963= _temp4961.curr + yylsp_offset;
if( _temp4963 < _temp4961.base? 1: _temp4963 >= _temp4961.last_plus_one){ _throw(
Null_Exception);}* _temp4963;}).last_line))};( struct _xenum_struct*) _temp4948;});
break; case 238: _LL4947: yyval=({ struct Cyc_Stmt_tok_struct* _temp4965=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4965=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_for_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4966= yyvs; struct _xenum_struct**
_temp4968= _temp4966.curr +( yyvsp_offset - 6); if( _temp4968 < _temp4966.base?
1: _temp4968 >= _temp4966.last_plus_one){ _throw( Null_Exception);}* _temp4968;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4969= yyvs; struct _xenum_struct**
_temp4971= _temp4969.curr +( yyvsp_offset - 4); if( _temp4971 < _temp4969.base?
1: _temp4971 >= _temp4969.last_plus_one){ _throw( Null_Exception);}* _temp4971;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4972= yyvs; struct _xenum_struct**
_temp4974= _temp4972.curr +( yyvsp_offset - 2); if( _temp4974 < _temp4972.base?
1: _temp4974 >= _temp4972.last_plus_one){ _throw( Null_Exception);}* _temp4974;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4975= yyvs; struct _xenum_struct**
_temp4977= _temp4975.curr + yyvsp_offset; if( _temp4977 < _temp4975.base? 1:
_temp4977 >= _temp4975.last_plus_one){ _throw( Null_Exception);}* _temp4977;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4978= yyls; struct Cyc_Yyltype*
_temp4980= _temp4978.curr +( yylsp_offset - 8); if( _temp4980 < _temp4978.base?
1: _temp4980 >= _temp4978.last_plus_one){ _throw( Null_Exception);}* _temp4980;}).first_line,({
struct _tagged_ptr3 _temp4981= yyls; struct Cyc_Yyltype* _temp4983= _temp4981.curr
+ yylsp_offset; if( _temp4983 < _temp4981.base? 1: _temp4983 >= _temp4981.last_plus_one){
_throw( Null_Exception);}* _temp4983;}).last_line))};( struct _xenum_struct*)
_temp4965;}); break; case 239: _LL4964: { struct Cyc_List_List* decls= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp4985= yyvs; struct _xenum_struct** _temp4987= _temp4985.curr
+( yyvsp_offset - 3); if( _temp4987 < _temp4985.base? 1: _temp4987 >= _temp4985.last_plus_one){
_throw( Null_Exception);}* _temp4987;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp4988= yyvs; struct _xenum_struct** _temp4990= _temp4988.curr
+ yyvsp_offset; if( _temp4990 < _temp4988.base? 1: _temp4990 >= _temp4988.last_plus_one){
_throw( Null_Exception);}* _temp4990;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4991= yyls; struct Cyc_Yyltype* _temp4993= _temp4991.curr +(
yylsp_offset - 5); if( _temp4993 < _temp4991.base? 1: _temp4993 >= _temp4991.last_plus_one){
_throw( Null_Exception);}* _temp4993;}).first_line,({ struct _tagged_ptr3
_temp4994= yyls; struct Cyc_Yyltype* _temp4996= _temp4994.curr + yylsp_offset;
if( _temp4996 < _temp4994.base? 1: _temp4996 >= _temp4994.last_plus_one){ _throw(
Null_Exception);}* _temp4996;}).last_line)); yyval=({ struct Cyc_Stmt_tok_struct*
_temp4997=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4997=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Parse_flatten_declarations(
decls, s)};( struct _xenum_struct*) _temp4997;}); break;} case 240: _LL4984: {
struct Cyc_List_List* decls= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2
_temp4999= yyvs; struct _xenum_struct** _temp5001= _temp4999.curr +(
yyvsp_offset - 4); if( _temp5001 < _temp4999.base? 1: _temp5001 >= _temp4999.last_plus_one){
_throw( Null_Exception);}* _temp5001;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5002=
yyvs; struct _xenum_struct** _temp5004= _temp5002.curr +( yyvsp_offset - 3); if(
_temp5004 < _temp5002.base? 1: _temp5004 >= _temp5002.last_plus_one){ _throw(
Null_Exception);}* _temp5004;})), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp5005= yyvs; struct _xenum_struct** _temp5007= _temp5005.curr
+ yyvsp_offset; if( _temp5007 < _temp5005.base? 1: _temp5007 >= _temp5005.last_plus_one){
_throw( Null_Exception);}* _temp5007;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5008= yyls; struct Cyc_Yyltype* _temp5010= _temp5008.curr +(
yylsp_offset - 6); if( _temp5010 < _temp5008.base? 1: _temp5010 >= _temp5008.last_plus_one){
_throw( Null_Exception);}* _temp5010;}).first_line,({ struct _tagged_ptr3
_temp5011= yyls; struct Cyc_Yyltype* _temp5013= _temp5011.curr + yylsp_offset;
if( _temp5013 < _temp5011.base? 1: _temp5013 >= _temp5011.last_plus_one){ _throw(
Null_Exception);}* _temp5013;}).last_line)); yyval=({ struct Cyc_Stmt_tok_struct*
_temp5014=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp5014=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Parse_flatten_declarations(
decls, s)};( struct _xenum_struct*) _temp5014;}); break;} case 241: _LL4998: {
struct Cyc_List_List* decls= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2
_temp5016= yyvs; struct _xenum_struct** _temp5018= _temp5016.curr +(
yyvsp_offset - 4); if( _temp5018 < _temp5016.base? 1: _temp5018 >= _temp5016.last_plus_one){
_throw( Null_Exception);}* _temp5018;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5019= yyvs; struct _xenum_struct** _temp5021= _temp5019.curr +(
yyvsp_offset - 2); if( _temp5021 < _temp5019.base? 1: _temp5021 >= _temp5019.last_plus_one){
_throw( Null_Exception);}* _temp5021;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp5022= yyvs; struct _xenum_struct** _temp5024= _temp5022.curr +
yyvsp_offset; if( _temp5024 < _temp5022.base? 1: _temp5024 >= _temp5022.last_plus_one){
_throw( Null_Exception);}* _temp5024;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5025= yyls; struct Cyc_Yyltype* _temp5027= _temp5025.curr +(
yylsp_offset - 6); if( _temp5027 < _temp5025.base? 1: _temp5027 >= _temp5025.last_plus_one){
_throw( Null_Exception);}* _temp5027;}).first_line,({ struct _tagged_ptr3
_temp5028= yyls; struct Cyc_Yyltype* _temp5030= _temp5028.curr + yylsp_offset;
if( _temp5030 < _temp5028.base? 1: _temp5030 >= _temp5028.last_plus_one){ _throw(
Null_Exception);}* _temp5030;}).last_line)); yyval=({ struct Cyc_Stmt_tok_struct*
_temp5031=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp5031=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Parse_flatten_declarations(
decls, s)};( struct _xenum_struct*) _temp5031;}); break;} case 242: _LL5015: {
struct Cyc_List_List* decls= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2
_temp5033= yyvs; struct _xenum_struct** _temp5035= _temp5033.curr +(
yyvsp_offset - 5); if( _temp5035 < _temp5033.base? 1: _temp5035 >= _temp5033.last_plus_one){
_throw( Null_Exception);}* _temp5035;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5036=
yyvs; struct _xenum_struct** _temp5038= _temp5036.curr +( yyvsp_offset - 4); if(
_temp5038 < _temp5036.base? 1: _temp5038 >= _temp5036.last_plus_one){ _throw(
Null_Exception);}* _temp5038;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5039= yyvs; struct _xenum_struct** _temp5041= _temp5039.curr +(
yyvsp_offset - 2); if( _temp5041 < _temp5039.base? 1: _temp5041 >= _temp5039.last_plus_one){
_throw( Null_Exception);}* _temp5041;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp5042= yyvs; struct _xenum_struct** _temp5044= _temp5042.curr +
yyvsp_offset; if( _temp5044 < _temp5042.base? 1: _temp5044 >= _temp5042.last_plus_one){
_throw( Null_Exception);}* _temp5044;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5045= yyls; struct Cyc_Yyltype* _temp5047= _temp5045.curr +(
yylsp_offset - 7); if( _temp5047 < _temp5045.base? 1: _temp5047 >= _temp5045.last_plus_one){
_throw( Null_Exception);}* _temp5047;}).first_line,({ struct _tagged_ptr3
_temp5048= yyls; struct Cyc_Yyltype* _temp5050= _temp5048.curr + yylsp_offset;
if( _temp5050 < _temp5048.base? 1: _temp5050 >= _temp5048.last_plus_one){ _throw(
Null_Exception);}* _temp5050;}).last_line)); yyval=({ struct Cyc_Stmt_tok_struct*
_temp5051=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp5051=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Parse_flatten_declarations(
decls, s)};( struct _xenum_struct*) _temp5051;}); break;} case 243: _LL5032:
yyval=({ struct Cyc_Stmt_tok_struct* _temp5053=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));* _temp5053=( struct Cyc_Stmt_tok_struct){.tag=
Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_goto_stmt(({ struct _tagged_string* _temp5054=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp5054[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp5055= yyvs;
struct _xenum_struct** _temp5057= _temp5055.curr +( yyvsp_offset - 1); if(
_temp5057 < _temp5055.base? 1: _temp5057 >= _temp5055.last_plus_one){ _throw(
Null_Exception);}* _temp5057;})); _temp5054;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5058= yyls; struct Cyc_Yyltype* _temp5060= _temp5058.curr
+( yylsp_offset - 2); if( _temp5060 < _temp5058.base? 1: _temp5060 >= _temp5058.last_plus_one){
_throw( Null_Exception);}* _temp5060;}).first_line,({ struct _tagged_ptr3
_temp5061= yyls; struct Cyc_Yyltype* _temp5063= _temp5061.curr +( yylsp_offset -
1); if( _temp5063 < _temp5061.base? 1: _temp5063 >= _temp5061.last_plus_one){
_throw( Null_Exception);}* _temp5063;}).last_line))};( struct _xenum_struct*)
_temp5053;}); break; case 244: _LL5052: yyval=({ struct Cyc_Stmt_tok_struct*
_temp5065=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp5065=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_continue_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5066= yyls; struct Cyc_Yyltype*
_temp5068= _temp5066.curr +( yylsp_offset - 1); if( _temp5068 < _temp5066.base?
1: _temp5068 >= _temp5066.last_plus_one){ _throw( Null_Exception);}* _temp5068;}).first_line,({
struct _tagged_ptr3 _temp5069= yyls; struct Cyc_Yyltype* _temp5071= _temp5069.curr
+( yylsp_offset - 1); if( _temp5071 < _temp5069.base? 1: _temp5071 >= _temp5069.last_plus_one){
_throw( Null_Exception);}* _temp5071;}).last_line))};( struct _xenum_struct*)
_temp5065;}); break; case 245: _LL5064: yyval=({ struct Cyc_Stmt_tok_struct*
_temp5073=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp5073=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_break_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5074= yyls; struct Cyc_Yyltype*
_temp5076= _temp5074.curr +( yylsp_offset - 1); if( _temp5076 < _temp5074.base?
1: _temp5076 >= _temp5074.last_plus_one){ _throw( Null_Exception);}* _temp5076;}).first_line,({
struct _tagged_ptr3 _temp5077= yyls; struct Cyc_Yyltype* _temp5079= _temp5077.curr
+( yylsp_offset - 1); if( _temp5079 < _temp5077.base? 1: _temp5079 >= _temp5077.last_plus_one){
_throw( Null_Exception);}* _temp5079;}).last_line))};( struct _xenum_struct*)
_temp5073;}); break; case 246: _LL5072: yyval=({ struct Cyc_Stmt_tok_struct*
_temp5081=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp5081=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_return_stmt(
0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5082= yyls; struct
Cyc_Yyltype* _temp5084= _temp5082.curr +( yylsp_offset - 1); if( _temp5084 <
_temp5082.base? 1: _temp5084 >= _temp5082.last_plus_one){ _throw( Null_Exception);}*
_temp5084;}).first_line,({ struct _tagged_ptr3 _temp5085= yyls; struct Cyc_Yyltype*
_temp5087= _temp5085.curr +( yylsp_offset - 1); if( _temp5087 < _temp5085.base?
1: _temp5087 >= _temp5085.last_plus_one){ _throw( Null_Exception);}* _temp5087;}).last_line))};(
struct _xenum_struct*) _temp5081;}); break; case 247: _LL5080: yyval=({ struct
Cyc_Stmt_tok_struct* _temp5089=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct));* _temp5089=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1=
Cyc_Absyn_return_stmt(( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5090= yyvs; struct _xenum_struct** _temp5092= _temp5090.curr +(
yyvsp_offset - 1); if( _temp5092 < _temp5090.base? 1: _temp5092 >= _temp5090.last_plus_one){
_throw( Null_Exception);}* _temp5092;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5093= yyls; struct Cyc_Yyltype* _temp5095= _temp5093.curr +(
yylsp_offset - 2); if( _temp5095 < _temp5093.base? 1: _temp5095 >= _temp5093.last_plus_one){
_throw( Null_Exception);}* _temp5095;}).first_line,({ struct _tagged_ptr3
_temp5096= yyls; struct Cyc_Yyltype* _temp5098= _temp5096.curr +( yylsp_offset -
1); if( _temp5098 < _temp5096.base? 1: _temp5098 >= _temp5096.last_plus_one){
_throw( Null_Exception);}* _temp5098;}).last_line))};( struct _xenum_struct*)
_temp5089;}); break; case 248: _LL5088: yyval=({ struct Cyc_Stmt_tok_struct*
_temp5100=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp5100=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_fallthru_stmt(
0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5101= yyls; struct
Cyc_Yyltype* _temp5103= _temp5101.curr +( yylsp_offset - 1); if( _temp5103 <
_temp5101.base? 1: _temp5103 >= _temp5101.last_plus_one){ _throw( Null_Exception);}*
_temp5103;}).first_line,({ struct _tagged_ptr3 _temp5104= yyls; struct Cyc_Yyltype*
_temp5106= _temp5104.curr +( yylsp_offset - 1); if( _temp5106 < _temp5104.base?
1: _temp5106 >= _temp5104.last_plus_one){ _throw( Null_Exception);}* _temp5106;}).last_line))};(
struct _xenum_struct*) _temp5100;}); break; case 249: _LL5099: yyval=({ struct
Cyc_Stmt_tok_struct* _temp5108=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct));* _temp5108=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1=
Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5109= yyls; struct Cyc_Yyltype* _temp5111= _temp5109.curr +( yylsp_offset -
3); if( _temp5111 < _temp5109.base? 1: _temp5111 >= _temp5109.last_plus_one){
_throw( Null_Exception);}* _temp5111;}).first_line,({ struct _tagged_ptr3
_temp5112= yyls; struct Cyc_Yyltype* _temp5114= _temp5112.curr +( yylsp_offset -
3); if( _temp5114 < _temp5112.base? 1: _temp5114 >= _temp5112.last_plus_one){
_throw( Null_Exception);}* _temp5114;}).last_line))};( struct _xenum_struct*)
_temp5108;}); break; case 250: _LL5107: yyval=({ struct Cyc_Stmt_tok_struct*
_temp5116=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp5116=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2 _temp5117= yyvs; struct
_xenum_struct** _temp5119= _temp5117.curr +( yyvsp_offset - 2); if( _temp5119 <
_temp5117.base? 1: _temp5119 >= _temp5117.last_plus_one){ _throw( Null_Exception);}*
_temp5119;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5120=
yyls; struct Cyc_Yyltype* _temp5122= _temp5120.curr +( yylsp_offset - 4); if(
_temp5122 < _temp5120.base? 1: _temp5122 >= _temp5120.last_plus_one){ _throw(
Null_Exception);}* _temp5122;}).first_line,({ struct _tagged_ptr3 _temp5123=
yyls; struct Cyc_Yyltype* _temp5125= _temp5123.curr +( yylsp_offset - 4); if(
_temp5125 < _temp5123.base? 1: _temp5125 >= _temp5123.last_plus_one){ _throw(
Null_Exception);}* _temp5125;}).last_line))};( struct _xenum_struct*) _temp5116;});
break; case 251: _LL5115: yyval=({ struct Cyc_Pattern_tok_struct* _temp5127=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp5127=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(
Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5128=
yyls; struct Cyc_Yyltype* _temp5130= _temp5128.curr + yylsp_offset; if(
_temp5130 < _temp5128.base? 1: _temp5130 >= _temp5128.last_plus_one){ _throw(
Null_Exception);}* _temp5130;}).first_line,({ struct _tagged_ptr3 _temp5131=
yyls; struct Cyc_Yyltype* _temp5133= _temp5131.curr + yylsp_offset; if(
_temp5133 < _temp5131.base? 1: _temp5133 >= _temp5131.last_plus_one){ _throw(
Null_Exception);}* _temp5133;}).last_line))};( struct _xenum_struct*) _temp5127;});
break; case 252: _LL5126: yyval=({ struct _tagged_ptr2 _temp5135= yyvs; struct
_xenum_struct** _temp5137= _temp5135.curr +( yyvsp_offset - 1); if( _temp5137 <
_temp5135.base? 1: _temp5137 >= _temp5135.last_plus_one){ _throw( Null_Exception);}*
_temp5137;}); break; case 253: _LL5134: yyval=({ struct Cyc_Pattern_tok_struct*
_temp5139=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp5139=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(({
struct Cyc_Absyn_Int_p_struct* _temp5140=( struct Cyc_Absyn_Int_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct)); _temp5140->tag= Cyc_Absyn_Int_p_tag;
_temp5140->f1=( void*)(* Cyc_yyget_Int_tok(({ struct _tagged_ptr2 _temp5141=
yyvs; struct _xenum_struct** _temp5143= _temp5141.curr + yyvsp_offset; if(
_temp5143 < _temp5141.base? 1: _temp5143 >= _temp5141.last_plus_one){ _throw(
Null_Exception);}* _temp5143;}))).f1; _temp5140->f2=(* Cyc_yyget_Int_tok(({
struct _tagged_ptr2 _temp5144= yyvs; struct _xenum_struct** _temp5146= _temp5144.curr
+ yyvsp_offset; if( _temp5146 < _temp5144.base? 1: _temp5146 >= _temp5144.last_plus_one){
_throw( Null_Exception);}* _temp5146;}))).f2;( void*) _temp5140;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5147= yyls; struct Cyc_Yyltype* _temp5149= _temp5147.curr
+ yylsp_offset; if( _temp5149 < _temp5147.base? 1: _temp5149 >= _temp5147.last_plus_one){
_throw( Null_Exception);}* _temp5149;}).first_line,({ struct _tagged_ptr3
_temp5150= yyls; struct Cyc_Yyltype* _temp5152= _temp5150.curr + yylsp_offset;
if( _temp5152 < _temp5150.base? 1: _temp5152 >= _temp5150.last_plus_one){ _throw(
Null_Exception);}* _temp5152;}).last_line))};( struct _xenum_struct*) _temp5139;});
break; case 254: _LL5138: yyval=({ struct Cyc_Pattern_tok_struct* _temp5154=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp5154=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(({
struct Cyc_Absyn_Int_p_struct* _temp5155=( struct Cyc_Absyn_Int_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct)); _temp5155->tag= Cyc_Absyn_Int_p_tag;
_temp5155->f1=( void*) Cyc_Absyn_Signed; _temp5155->f2= -(* Cyc_yyget_Int_tok(({
struct _tagged_ptr2 _temp5156= yyvs; struct _xenum_struct** _temp5158= _temp5156.curr
+ yyvsp_offset; if( _temp5158 < _temp5156.base? 1: _temp5158 >= _temp5156.last_plus_one){
_throw( Null_Exception);}* _temp5158;}))).f2;( void*) _temp5155;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5159= yyls; struct Cyc_Yyltype* _temp5161= _temp5159.curr
+( yylsp_offset - 1); if( _temp5161 < _temp5159.base? 1: _temp5161 >= _temp5159.last_plus_one){
_throw( Null_Exception);}* _temp5161;}).first_line,({ struct _tagged_ptr3
_temp5162= yyls; struct Cyc_Yyltype* _temp5164= _temp5162.curr + yylsp_offset;
if( _temp5164 < _temp5162.base? 1: _temp5164 >= _temp5162.last_plus_one){ _throw(
Null_Exception);}* _temp5164;}).last_line))};( struct _xenum_struct*) _temp5154;});
break; case 255: _LL5153: yyval=({ struct Cyc_Pattern_tok_struct* _temp5166=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp5166=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(({
struct Cyc_Absyn_Float_p_struct* _temp5167=( struct Cyc_Absyn_Float_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Float_p_struct)); _temp5167->tag= Cyc_Absyn_Float_p_tag;
_temp5167->f1= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp5168= yyvs;
struct _xenum_struct** _temp5170= _temp5168.curr + yyvsp_offset; if( _temp5170 <
_temp5168.base? 1: _temp5170 >= _temp5168.last_plus_one){ _throw( Null_Exception);}*
_temp5170;}));( void*) _temp5167;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5171= yyls; struct Cyc_Yyltype* _temp5173= _temp5171.curr +
yylsp_offset; if( _temp5173 < _temp5171.base? 1: _temp5173 >= _temp5171.last_plus_one){
_throw( Null_Exception);}* _temp5173;}).first_line,({ struct _tagged_ptr3
_temp5174= yyls; struct Cyc_Yyltype* _temp5176= _temp5174.curr + yylsp_offset;
if( _temp5176 < _temp5174.base? 1: _temp5176 >= _temp5174.last_plus_one){ _throw(
Null_Exception);}* _temp5176;}).last_line))};( struct _xenum_struct*) _temp5166;});
break; case 256: _LL5165: yyval=({ struct Cyc_Pattern_tok_struct* _temp5178=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp5178=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(({
struct Cyc_Absyn_Char_p_struct* _temp5179=( struct Cyc_Absyn_Char_p_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Absyn_Char_p_struct)); _temp5179->tag= Cyc_Absyn_Char_p_tag;
_temp5179->f1= Cyc_yyget_Char_tok(({ struct _tagged_ptr2 _temp5180= yyvs; struct
_xenum_struct** _temp5182= _temp5180.curr + yyvsp_offset; if( _temp5182 <
_temp5180.base? 1: _temp5182 >= _temp5180.last_plus_one){ _throw( Null_Exception);}*
_temp5182;}));( void*) _temp5179;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5183= yyls; struct Cyc_Yyltype* _temp5185= _temp5183.curr +
yylsp_offset; if( _temp5185 < _temp5183.base? 1: _temp5185 >= _temp5183.last_plus_one){
_throw( Null_Exception);}* _temp5185;}).first_line,({ struct _tagged_ptr3
_temp5186= yyls; struct Cyc_Yyltype* _temp5188= _temp5186.curr + yylsp_offset;
if( _temp5188 < _temp5186.base? 1: _temp5188 >= _temp5186.last_plus_one){ _throw(
Null_Exception);}* _temp5188;}).last_line))};( struct _xenum_struct*) _temp5178;});
break; case 257: _LL5177: yyval=({ struct Cyc_Pattern_tok_struct* _temp5190=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp5190=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(
Cyc_Absyn_Null_p, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5191=
yyls; struct Cyc_Yyltype* _temp5193= _temp5191.curr + yylsp_offset; if(
_temp5193 < _temp5191.base? 1: _temp5193 >= _temp5191.last_plus_one){ _throw(
Null_Exception);}* _temp5193;}).first_line,({ struct _tagged_ptr3 _temp5194=
yyls; struct Cyc_Yyltype* _temp5196= _temp5194.curr + yylsp_offset; if(
_temp5196 < _temp5194.base? 1: _temp5196 >= _temp5194.last_plus_one){ _throw(
Null_Exception);}* _temp5196;}).last_line))};( struct _xenum_struct*) _temp5190;});
break; case 258: _LL5189: yyval=({ struct Cyc_Pattern_tok_struct* _temp5198=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp5198=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(({
struct Cyc_Absyn_UnknownId_p_struct* _temp5199=( struct Cyc_Absyn_UnknownId_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_p_struct)); _temp5199->tag= Cyc_Absyn_UnknownId_p_tag;
_temp5199->f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp5200= yyvs;
struct _xenum_struct** _temp5202= _temp5200.curr + yyvsp_offset; if( _temp5202 <
_temp5200.base? 1: _temp5202 >= _temp5200.last_plus_one){ _throw( Null_Exception);}*
_temp5202;}));( void*) _temp5199;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5203= yyls; struct Cyc_Yyltype* _temp5205= _temp5203.curr +
yylsp_offset; if( _temp5205 < _temp5203.base? 1: _temp5205 >= _temp5203.last_plus_one){
_throw( Null_Exception);}* _temp5205;}).first_line,({ struct _tagged_ptr3
_temp5206= yyls; struct Cyc_Yyltype* _temp5208= _temp5206.curr + yylsp_offset;
if( _temp5208 < _temp5206.base? 1: _temp5208 >= _temp5206.last_plus_one){ _throw(
Null_Exception);}* _temp5208;}).last_line))};( struct _xenum_struct*) _temp5198;});
break; case 259: _LL5197: { struct Cyc_List_List* tvs=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5210= yyls; struct Cyc_Yyltype* _temp5212= _temp5210.curr
+( yylsp_offset - 3); if( _temp5212 < _temp5210.base? 1: _temp5212 >= _temp5210.last_plus_one){
_throw( Null_Exception);}* _temp5212;}).first_line,({ struct _tagged_ptr3
_temp5213= yyls; struct Cyc_Yyltype* _temp5215= _temp5213.curr +( yylsp_offset -
3); if( _temp5215 < _temp5213.base? 1: _temp5215 >= _temp5213.last_plus_one){
_throw( Null_Exception);}* _temp5215;}).last_line), Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp5216= yyvs; struct _xenum_struct** _temp5218= _temp5216.curr
+( yyvsp_offset - 3); if( _temp5218 < _temp5216.base? 1: _temp5218 >= _temp5216.last_plus_one){
_throw( Null_Exception);}* _temp5218;}))); yyval=({ struct Cyc_Pattern_tok_struct*
_temp5219=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp5219=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(({
struct Cyc_Absyn_UnknownCall_p_struct* _temp5220=( struct Cyc_Absyn_UnknownCall_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownCall_p_struct)); _temp5220->tag= Cyc_Absyn_UnknownCall_p_tag;
_temp5220->f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp5221= yyvs;
struct _xenum_struct** _temp5223= _temp5221.curr +( yyvsp_offset - 4); if(
_temp5223 < _temp5221.base? 1: _temp5223 >= _temp5221.last_plus_one){ _throw(
Null_Exception);}* _temp5223;})); _temp5220->f2= tvs; _temp5220->f3= Cyc_yyget_PatternList_tok(({
struct _tagged_ptr2 _temp5224= yyvs; struct _xenum_struct** _temp5226= _temp5224.curr
+( yyvsp_offset - 1); if( _temp5226 < _temp5224.base? 1: _temp5226 >= _temp5224.last_plus_one){
_throw( Null_Exception);}* _temp5226;}));( void*) _temp5220;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5227= yyls; struct Cyc_Yyltype* _temp5229= _temp5227.curr
+( yylsp_offset - 4); if( _temp5229 < _temp5227.base? 1: _temp5229 >= _temp5227.last_plus_one){
_throw( Null_Exception);}* _temp5229;}).first_line,({ struct _tagged_ptr3
_temp5230= yyls; struct Cyc_Yyltype* _temp5232= _temp5230.curr + yylsp_offset;
if( _temp5232 < _temp5230.base? 1: _temp5232 >= _temp5230.last_plus_one){ _throw(
Null_Exception);}* _temp5232;}).last_line))};( struct _xenum_struct*) _temp5219;});
break;} case 260: _LL5209: yyval=({ struct Cyc_Pattern_tok_struct* _temp5234=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp5234=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(({
struct Cyc_Absyn_Tuple_p_struct* _temp5235=( struct Cyc_Absyn_Tuple_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuple_p_struct)); _temp5235->tag= Cyc_Absyn_Tuple_p_tag;
_temp5235->f1= Cyc_yyget_PatternList_tok(({ struct _tagged_ptr2 _temp5236= yyvs;
struct _xenum_struct** _temp5238= _temp5236.curr +( yyvsp_offset - 1); if(
_temp5238 < _temp5236.base? 1: _temp5238 >= _temp5236.last_plus_one){ _throw(
Null_Exception);}* _temp5238;}));( void*) _temp5235;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5239= yyls; struct Cyc_Yyltype* _temp5241= _temp5239.curr
+( yylsp_offset - 3); if( _temp5241 < _temp5239.base? 1: _temp5241 >= _temp5239.last_plus_one){
_throw( Null_Exception);}* _temp5241;}).first_line,({ struct _tagged_ptr3
_temp5242= yyls; struct Cyc_Yyltype* _temp5244= _temp5242.curr + yylsp_offset;
if( _temp5244 < _temp5242.base? 1: _temp5244 >= _temp5242.last_plus_one){ _throw(
Null_Exception);}* _temp5244;}).last_line))};( struct _xenum_struct*) _temp5234;});
break; case 261: _LL5233: { struct Cyc_List_List* tvs=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5246= yyls; struct Cyc_Yyltype* _temp5248= _temp5246.curr
+( yylsp_offset - 2); if( _temp5248 < _temp5246.base? 1: _temp5248 >= _temp5246.last_plus_one){
_throw( Null_Exception);}* _temp5248;}).first_line,({ struct _tagged_ptr3
_temp5249= yyls; struct Cyc_Yyltype* _temp5251= _temp5249.curr +( yylsp_offset -
2); if( _temp5251 < _temp5249.base? 1: _temp5251 >= _temp5249.last_plus_one){
_throw( Null_Exception);}* _temp5251;}).last_line), Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp5252= yyvs; struct _xenum_struct** _temp5254= _temp5252.curr
+( yyvsp_offset - 2); if( _temp5254 < _temp5252.base? 1: _temp5254 >= _temp5252.last_plus_one){
_throw( Null_Exception);}* _temp5254;}))); yyval=({ struct Cyc_Pattern_tok_struct*
_temp5255=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp5255=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(({
struct Cyc_Absyn_UnknownFields_p_struct* _temp5256=( struct Cyc_Absyn_UnknownFields_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownFields_p_struct)); _temp5256->tag=
Cyc_Absyn_UnknownFields_p_tag; _temp5256->f1= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp5257= yyvs; struct _xenum_struct** _temp5259= _temp5257.curr +(
yyvsp_offset - 3); if( _temp5259 < _temp5257.base? 1: _temp5259 >= _temp5257.last_plus_one){
_throw( Null_Exception);}* _temp5259;})); _temp5256->f2= tvs; _temp5256->f3= 0;(
void*) _temp5256;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5260= yyls; struct Cyc_Yyltype* _temp5262= _temp5260.curr +( yylsp_offset -
3); if( _temp5262 < _temp5260.base? 1: _temp5262 >= _temp5260.last_plus_one){
_throw( Null_Exception);}* _temp5262;}).first_line,({ struct _tagged_ptr3
_temp5263= yyls; struct Cyc_Yyltype* _temp5265= _temp5263.curr + yylsp_offset;
if( _temp5265 < _temp5263.base? 1: _temp5265 >= _temp5263.last_plus_one){ _throw(
Null_Exception);}* _temp5265;}).last_line))};( struct _xenum_struct*) _temp5255;});
break;} case 262: _LL5245: { struct Cyc_List_List* tvs=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5267= yyls; struct Cyc_Yyltype* _temp5269= _temp5267.curr
+( yylsp_offset - 3); if( _temp5269 < _temp5267.base? 1: _temp5269 >= _temp5267.last_plus_one){
_throw( Null_Exception);}* _temp5269;}).first_line,({ struct _tagged_ptr3
_temp5270= yyls; struct Cyc_Yyltype* _temp5272= _temp5270.curr +( yylsp_offset -
3); if( _temp5272 < _temp5270.base? 1: _temp5272 >= _temp5270.last_plus_one){
_throw( Null_Exception);}* _temp5272;}).last_line), Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp5273= yyvs; struct _xenum_struct** _temp5275= _temp5273.curr
+( yyvsp_offset - 3); if( _temp5275 < _temp5273.base? 1: _temp5275 >= _temp5273.last_plus_one){
_throw( Null_Exception);}* _temp5275;}))); yyval=({ struct Cyc_Pattern_tok_struct*
_temp5276=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp5276=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(({
struct Cyc_Absyn_UnknownFields_p_struct* _temp5277=( struct Cyc_Absyn_UnknownFields_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownFields_p_struct)); _temp5277->tag=
Cyc_Absyn_UnknownFields_p_tag; _temp5277->f1= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp5278= yyvs; struct _xenum_struct** _temp5280= _temp5278.curr +(
yyvsp_offset - 4); if( _temp5280 < _temp5278.base? 1: _temp5280 >= _temp5278.last_plus_one){
_throw( Null_Exception);}* _temp5280;})); _temp5277->f2= tvs; _temp5277->f3= Cyc_yyget_FieldPatternList_tok(({
struct _tagged_ptr2 _temp5281= yyvs; struct _xenum_struct** _temp5283= _temp5281.curr
+( yyvsp_offset - 1); if( _temp5283 < _temp5281.base? 1: _temp5283 >= _temp5281.last_plus_one){
_throw( Null_Exception);}* _temp5283;}));( void*) _temp5277;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5284= yyls; struct Cyc_Yyltype* _temp5286= _temp5284.curr
+( yylsp_offset - 4); if( _temp5286 < _temp5284.base? 1: _temp5286 >= _temp5284.last_plus_one){
_throw( Null_Exception);}* _temp5286;}).first_line,({ struct _tagged_ptr3
_temp5287= yyls; struct Cyc_Yyltype* _temp5289= _temp5287.curr + yylsp_offset;
if( _temp5289 < _temp5287.base? 1: _temp5289 >= _temp5287.last_plus_one){ _throw(
Null_Exception);}* _temp5289;}).last_line))};( struct _xenum_struct*) _temp5276;});
break;} case 263: _LL5266: yyval=({ struct Cyc_Pattern_tok_struct* _temp5291=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp5291=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(({
struct Cyc_Absyn_Pointer_p_struct* _temp5292=( struct Cyc_Absyn_Pointer_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_p_struct)); _temp5292->tag= Cyc_Absyn_Pointer_p_tag;
_temp5292->f1= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp5293= yyvs;
struct _xenum_struct** _temp5295= _temp5293.curr + yyvsp_offset; if( _temp5295 <
_temp5293.base? 1: _temp5295 >= _temp5293.last_plus_one){ _throw( Null_Exception);}*
_temp5295;}));( void*) _temp5292;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5296= yyls; struct Cyc_Yyltype* _temp5298= _temp5296.curr +(
yylsp_offset - 1); if( _temp5298 < _temp5296.base? 1: _temp5298 >= _temp5296.last_plus_one){
_throw( Null_Exception);}* _temp5298;}).first_line,({ struct _tagged_ptr3
_temp5299= yyls; struct Cyc_Yyltype* _temp5301= _temp5299.curr + yylsp_offset;
if( _temp5301 < _temp5299.base? 1: _temp5301 >= _temp5299.last_plus_one){ _throw(
Null_Exception);}* _temp5301;}).last_line))};( struct _xenum_struct*) _temp5291;});
break; case 264: _LL5290: yyval=({ struct Cyc_Pattern_tok_struct* _temp5303=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp5303=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(({
struct Cyc_Absyn_Reference_p_struct* _temp5304=( struct Cyc_Absyn_Reference_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Reference_p_struct)); _temp5304->tag= Cyc_Absyn_Reference_p_tag;
_temp5304->f1= Cyc_Absyn_new_vardecl(({ struct _tuple1* _temp5305=( struct
_tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp5305->f1= Cyc_Absyn_Loc_n;
_temp5305->f2=({ struct _tagged_string* _temp5306=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp5306[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp5307= yyvs; struct _xenum_struct** _temp5309= _temp5307.curr
+ yyvsp_offset; if( _temp5309 < _temp5307.base? 1: _temp5309 >= _temp5307.last_plus_one){
_throw( Null_Exception);}* _temp5309;})); _temp5306;}); _temp5305;}), Cyc_Absyn_VoidType,
0);( void*) _temp5304;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5310= yyls; struct Cyc_Yyltype* _temp5312= _temp5310.curr +( yylsp_offset -
1); if( _temp5312 < _temp5310.base? 1: _temp5312 >= _temp5310.last_plus_one){
_throw( Null_Exception);}* _temp5312;}).first_line,({ struct _tagged_ptr3
_temp5313= yyls; struct Cyc_Yyltype* _temp5315= _temp5313.curr + yylsp_offset;
if( _temp5315 < _temp5313.base? 1: _temp5315 >= _temp5313.last_plus_one){ _throw(
Null_Exception);}* _temp5315;}).last_line))};( struct _xenum_struct*) _temp5303;});
break; case 265: _LL5302: yyval=({ struct Cyc_PatternList_tok_struct* _temp5317=(
struct Cyc_PatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));*
_temp5317=( struct Cyc_PatternList_tok_struct){.tag= Cyc_PatternList_tok_tag,.f1=
0};( struct _xenum_struct*) _temp5317;}); break; case 266: _LL5316: yyval=({
struct Cyc_PatternList_tok_struct* _temp5319=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));* _temp5319=( struct Cyc_PatternList_tok_struct){.tag=
Cyc_PatternList_tok_tag,.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( Cyc_yyget_PatternList_tok(({ struct _tagged_ptr2 _temp5320=
yyvs; struct _xenum_struct** _temp5322= _temp5320.curr + yyvsp_offset; if(
_temp5322 < _temp5320.base? 1: _temp5322 >= _temp5320.last_plus_one){ _throw(
Null_Exception);}* _temp5322;})))};( struct _xenum_struct*) _temp5319;}); break;
case 267: _LL5318: yyval=({ struct Cyc_PatternList_tok_struct* _temp5324=(
struct Cyc_PatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));*
_temp5324=( struct Cyc_PatternList_tok_struct){.tag= Cyc_PatternList_tok_tag,.f1=({
struct Cyc_List_List* _temp5325=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp5325->hd=( void*) Cyc_yyget_Pattern_tok(({ struct
_tagged_ptr2 _temp5326= yyvs; struct _xenum_struct** _temp5328= _temp5326.curr +
yyvsp_offset; if( _temp5328 < _temp5326.base? 1: _temp5328 >= _temp5326.last_plus_one){
_throw( Null_Exception);}* _temp5328;})); _temp5325->tl= 0; _temp5325;})};(
struct _xenum_struct*) _temp5324;}); break; case 268: _LL5323: yyval=({ struct
Cyc_PatternList_tok_struct* _temp5330=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));* _temp5330=( struct Cyc_PatternList_tok_struct){.tag=
Cyc_PatternList_tok_tag,.f1=({ struct Cyc_List_List* _temp5331=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5331->hd=( void*) Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp5335= yyvs; struct _xenum_struct** _temp5337= _temp5335.curr
+ yyvsp_offset; if( _temp5337 < _temp5335.base? 1: _temp5337 >= _temp5335.last_plus_one){
_throw( Null_Exception);}* _temp5337;})); _temp5331->tl= Cyc_yyget_PatternList_tok(({
struct _tagged_ptr2 _temp5332= yyvs; struct _xenum_struct** _temp5334= _temp5332.curr
+( yyvsp_offset - 2); if( _temp5334 < _temp5332.base? 1: _temp5334 >= _temp5332.last_plus_one){
_throw( Null_Exception);}* _temp5334;})); _temp5331;})};( struct _xenum_struct*)
_temp5330;}); break; case 269: _LL5329: yyval=({ struct Cyc_FieldPattern_tok_struct*
_temp5339=( struct Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));*
_temp5339=( struct Cyc_FieldPattern_tok_struct){.tag= Cyc_FieldPattern_tok_tag,.f1=({
struct _tuple15* _temp5340=( struct _tuple15*) GC_malloc( sizeof( struct
_tuple15)); _temp5340->f1= 0; _temp5340->f2= Cyc_yyget_Pattern_tok(({ struct
_tagged_ptr2 _temp5341= yyvs; struct _xenum_struct** _temp5343= _temp5341.curr +
yyvsp_offset; if( _temp5343 < _temp5341.base? 1: _temp5343 >= _temp5341.last_plus_one){
_throw( Null_Exception);}* _temp5343;})); _temp5340;})};( struct _xenum_struct*)
_temp5339;}); break; case 270: _LL5338: yyval=({ struct Cyc_FieldPattern_tok_struct*
_temp5345=( struct Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));*
_temp5345=( struct Cyc_FieldPattern_tok_struct){.tag= Cyc_FieldPattern_tok_tag,.f1=({
struct _tuple15* _temp5346=( struct _tuple15*) GC_malloc( sizeof( struct
_tuple15)); _temp5346->f1= Cyc_yyget_DesignatorList_tok(({ struct _tagged_ptr2
_temp5350= yyvs; struct _xenum_struct** _temp5352= _temp5350.curr +(
yyvsp_offset - 1); if( _temp5352 < _temp5350.base? 1: _temp5352 >= _temp5350.last_plus_one){
_throw( Null_Exception);}* _temp5352;})); _temp5346->f2= Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp5347= yyvs; struct _xenum_struct** _temp5349= _temp5347.curr
+ yyvsp_offset; if( _temp5349 < _temp5347.base? 1: _temp5349 >= _temp5347.last_plus_one){
_throw( Null_Exception);}* _temp5349;})); _temp5346;})};( struct _xenum_struct*)
_temp5345;}); break; case 271: _LL5344: yyval=({ struct Cyc_FieldPatternList_tok_struct*
_temp5354=( struct Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct
Cyc_FieldPatternList_tok_struct));* _temp5354=( struct Cyc_FieldPatternList_tok_struct){.tag=
Cyc_FieldPatternList_tok_tag,.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_FieldPatternList_tok(({ struct _tagged_ptr2
_temp5355= yyvs; struct _xenum_struct** _temp5357= _temp5355.curr + yyvsp_offset;
if( _temp5357 < _temp5355.base? 1: _temp5357 >= _temp5355.last_plus_one){ _throw(
Null_Exception);}* _temp5357;})))};( struct _xenum_struct*) _temp5354;}); break;
case 272: _LL5353: yyval=({ struct Cyc_FieldPatternList_tok_struct* _temp5359=(
struct Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct));*
_temp5359=( struct Cyc_FieldPatternList_tok_struct){.tag= Cyc_FieldPatternList_tok_tag,.f1=({
struct Cyc_List_List* _temp5360=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp5360->hd=( void*) Cyc_yyget_FieldPattern_tok(({
struct _tagged_ptr2 _temp5361= yyvs; struct _xenum_struct** _temp5363= _temp5361.curr
+ yyvsp_offset; if( _temp5363 < _temp5361.base? 1: _temp5363 >= _temp5361.last_plus_one){
_throw( Null_Exception);}* _temp5363;})); _temp5360->tl= 0; _temp5360;})};(
struct _xenum_struct*) _temp5359;}); break; case 273: _LL5358: yyval=({ struct
Cyc_FieldPatternList_tok_struct* _temp5365=( struct Cyc_FieldPatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct));* _temp5365=( struct
Cyc_FieldPatternList_tok_struct){.tag= Cyc_FieldPatternList_tok_tag,.f1=({
struct Cyc_List_List* _temp5366=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp5366->hd=( void*) Cyc_yyget_FieldPattern_tok(({
struct _tagged_ptr2 _temp5370= yyvs; struct _xenum_struct** _temp5372= _temp5370.curr
+ yyvsp_offset; if( _temp5372 < _temp5370.base? 1: _temp5372 >= _temp5370.last_plus_one){
_throw( Null_Exception);}* _temp5372;})); _temp5366->tl= Cyc_yyget_FieldPatternList_tok(({
struct _tagged_ptr2 _temp5367= yyvs; struct _xenum_struct** _temp5369= _temp5367.curr
+( yyvsp_offset - 2); if( _temp5369 < _temp5367.base? 1: _temp5369 >= _temp5367.last_plus_one){
_throw( Null_Exception);}* _temp5369;})); _temp5366;})};( struct _xenum_struct*)
_temp5365;}); break; case 274: _LL5364: yyval=({ struct _tagged_ptr2 _temp5374=
yyvs; struct _xenum_struct** _temp5376= _temp5374.curr + yyvsp_offset; if(
_temp5376 < _temp5374.base? 1: _temp5376 >= _temp5374.last_plus_one){ _throw(
Null_Exception);}* _temp5376;}); break; case 275: _LL5373: yyval=({ struct Cyc_Exp_tok_struct*
_temp5378=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5378=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_seq_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5379= yyvs; struct _xenum_struct**
_temp5381= _temp5379.curr +( yyvsp_offset - 2); if( _temp5381 < _temp5379.base?
1: _temp5381 >= _temp5379.last_plus_one){ _throw( Null_Exception);}* _temp5381;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5382= yyvs; struct _xenum_struct**
_temp5384= _temp5382.curr + yyvsp_offset; if( _temp5384 < _temp5382.base? 1:
_temp5384 >= _temp5382.last_plus_one){ _throw( Null_Exception);}* _temp5384;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5385= yyls; struct Cyc_Yyltype*
_temp5387= _temp5385.curr +( yylsp_offset - 2); if( _temp5387 < _temp5385.base?
1: _temp5387 >= _temp5385.last_plus_one){ _throw( Null_Exception);}* _temp5387;}).first_line,({
struct _tagged_ptr3 _temp5388= yyls; struct Cyc_Yyltype* _temp5390= _temp5388.curr
+ yylsp_offset; if( _temp5390 < _temp5388.base? 1: _temp5390 >= _temp5388.last_plus_one){
_throw( Null_Exception);}* _temp5390;}).last_line))};( struct _xenum_struct*)
_temp5378;}); break; case 276: _LL5377: yyval=({ struct _tagged_ptr2 _temp5392=
yyvs; struct _xenum_struct** _temp5394= _temp5392.curr + yyvsp_offset; if(
_temp5394 < _temp5392.base? 1: _temp5394 >= _temp5392.last_plus_one){ _throw(
Null_Exception);}* _temp5394;}); break; case 277: _LL5391: yyval=({ struct Cyc_Exp_tok_struct*
_temp5396=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5396=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_assignop_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5397= yyvs; struct _xenum_struct**
_temp5399= _temp5397.curr +( yyvsp_offset - 2); if( _temp5399 < _temp5397.base?
1: _temp5399 >= _temp5397.last_plus_one){ _throw( Null_Exception);}* _temp5399;})),
Cyc_yyget_Primopopt_tok(({ struct _tagged_ptr2 _temp5400= yyvs; struct
_xenum_struct** _temp5402= _temp5400.curr +( yyvsp_offset - 1); if( _temp5402 <
_temp5400.base? 1: _temp5402 >= _temp5400.last_plus_one){ _throw( Null_Exception);}*
_temp5402;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5403= yyvs; struct
_xenum_struct** _temp5405= _temp5403.curr + yyvsp_offset; if( _temp5405 <
_temp5403.base? 1: _temp5405 >= _temp5403.last_plus_one){ _throw( Null_Exception);}*
_temp5405;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5406=
yyls; struct Cyc_Yyltype* _temp5408= _temp5406.curr +( yylsp_offset - 2); if(
_temp5408 < _temp5406.base? 1: _temp5408 >= _temp5406.last_plus_one){ _throw(
Null_Exception);}* _temp5408;}).first_line,({ struct _tagged_ptr3 _temp5409=
yyls; struct Cyc_Yyltype* _temp5411= _temp5409.curr + yylsp_offset; if(
_temp5411 < _temp5409.base? 1: _temp5411 >= _temp5409.last_plus_one){ _throw(
Null_Exception);}* _temp5411;}).last_line))};( struct _xenum_struct*) _temp5396;});
break; case 278: _LL5395: yyval=({ struct Cyc_Primopopt_tok_struct* _temp5413=(
struct Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));*
_temp5413=( struct Cyc_Primopopt_tok_struct){.tag= Cyc_Primopopt_tok_tag,.f1= 0};(
struct _xenum_struct*) _temp5413;}); break; case 279: _LL5412: yyval=({ struct
Cyc_Primopopt_tok_struct* _temp5415=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));* _temp5415=( struct Cyc_Primopopt_tok_struct){.tag=
Cyc_Primopopt_tok_tag,.f1=({ struct Cyc_Core_Opt* _temp5416=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp5416->v=( void*) Cyc_Absyn_Times;
_temp5416;})};( struct _xenum_struct*) _temp5415;}); break; case 280: _LL5414:
yyval=({ struct Cyc_Primopopt_tok_struct* _temp5418=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));* _temp5418=( struct Cyc_Primopopt_tok_struct){.tag=
Cyc_Primopopt_tok_tag,.f1=({ struct Cyc_Core_Opt* _temp5419=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp5419->v=( void*) Cyc_Absyn_Div;
_temp5419;})};( struct _xenum_struct*) _temp5418;}); break; case 281: _LL5417:
yyval=({ struct Cyc_Primopopt_tok_struct* _temp5421=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));* _temp5421=( struct Cyc_Primopopt_tok_struct){.tag=
Cyc_Primopopt_tok_tag,.f1=({ struct Cyc_Core_Opt* _temp5422=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp5422->v=( void*) Cyc_Absyn_Mod;
_temp5422;})};( struct _xenum_struct*) _temp5421;}); break; case 282: _LL5420:
yyval=({ struct Cyc_Primopopt_tok_struct* _temp5424=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));* _temp5424=( struct Cyc_Primopopt_tok_struct){.tag=
Cyc_Primopopt_tok_tag,.f1=({ struct Cyc_Core_Opt* _temp5425=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp5425->v=( void*) Cyc_Absyn_Plus;
_temp5425;})};( struct _xenum_struct*) _temp5424;}); break; case 283: _LL5423:
yyval=({ struct Cyc_Primopopt_tok_struct* _temp5427=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));* _temp5427=( struct Cyc_Primopopt_tok_struct){.tag=
Cyc_Primopopt_tok_tag,.f1=({ struct Cyc_Core_Opt* _temp5428=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp5428->v=( void*) Cyc_Absyn_Minus;
_temp5428;})};( struct _xenum_struct*) _temp5427;}); break; case 284: _LL5426:
yyval=({ struct Cyc_Primopopt_tok_struct* _temp5430=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));* _temp5430=( struct Cyc_Primopopt_tok_struct){.tag=
Cyc_Primopopt_tok_tag,.f1=({ struct Cyc_Core_Opt* _temp5431=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp5431->v=( void*) Cyc_Absyn_Bitlshift;
_temp5431;})};( struct _xenum_struct*) _temp5430;}); break; case 285: _LL5429:
yyval=({ struct Cyc_Primopopt_tok_struct* _temp5433=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));* _temp5433=( struct Cyc_Primopopt_tok_struct){.tag=
Cyc_Primopopt_tok_tag,.f1=({ struct Cyc_Core_Opt* _temp5434=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp5434->v=( void*) Cyc_Absyn_Bitlrshift;
_temp5434;})};( struct _xenum_struct*) _temp5433;}); break; case 286: _LL5432:
yyval=({ struct Cyc_Primopopt_tok_struct* _temp5436=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));* _temp5436=( struct Cyc_Primopopt_tok_struct){.tag=
Cyc_Primopopt_tok_tag,.f1=({ struct Cyc_Core_Opt* _temp5437=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp5437->v=( void*) Cyc_Absyn_Bitand;
_temp5437;})};( struct _xenum_struct*) _temp5436;}); break; case 287: _LL5435:
yyval=({ struct Cyc_Primopopt_tok_struct* _temp5439=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));* _temp5439=( struct Cyc_Primopopt_tok_struct){.tag=
Cyc_Primopopt_tok_tag,.f1=({ struct Cyc_Core_Opt* _temp5440=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp5440->v=( void*) Cyc_Absyn_Bitxor;
_temp5440;})};( struct _xenum_struct*) _temp5439;}); break; case 288: _LL5438:
yyval=({ struct Cyc_Primopopt_tok_struct* _temp5442=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));* _temp5442=( struct Cyc_Primopopt_tok_struct){.tag=
Cyc_Primopopt_tok_tag,.f1=({ struct Cyc_Core_Opt* _temp5443=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp5443->v=( void*) Cyc_Absyn_Bitor;
_temp5443;})};( struct _xenum_struct*) _temp5442;}); break; case 289: _LL5441:
yyval=({ struct _tagged_ptr2 _temp5445= yyvs; struct _xenum_struct** _temp5447=
_temp5445.curr + yyvsp_offset; if( _temp5447 < _temp5445.base? 1: _temp5447 >=
_temp5445.last_plus_one){ _throw( Null_Exception);}* _temp5447;}); break; case
290: _LL5444: yyval=({ struct Cyc_Exp_tok_struct* _temp5449=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp5449=( struct Cyc_Exp_tok_struct){.tag=
Cyc_Exp_tok_tag,.f1= Cyc_Absyn_conditional_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5450= yyvs; struct _xenum_struct** _temp5452= _temp5450.curr +(
yyvsp_offset - 4); if( _temp5452 < _temp5450.base? 1: _temp5452 >= _temp5450.last_plus_one){
_throw( Null_Exception);}* _temp5452;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5453= yyvs; struct _xenum_struct** _temp5455= _temp5453.curr +(
yyvsp_offset - 2); if( _temp5455 < _temp5453.base? 1: _temp5455 >= _temp5453.last_plus_one){
_throw( Null_Exception);}* _temp5455;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5456= yyvs; struct _xenum_struct** _temp5458= _temp5456.curr +
yyvsp_offset; if( _temp5458 < _temp5456.base? 1: _temp5458 >= _temp5456.last_plus_one){
_throw( Null_Exception);}* _temp5458;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5459= yyls; struct Cyc_Yyltype* _temp5461= _temp5459.curr +(
yylsp_offset - 4); if( _temp5461 < _temp5459.base? 1: _temp5461 >= _temp5459.last_plus_one){
_throw( Null_Exception);}* _temp5461;}).first_line,({ struct _tagged_ptr3
_temp5462= yyls; struct Cyc_Yyltype* _temp5464= _temp5462.curr + yylsp_offset;
if( _temp5464 < _temp5462.base? 1: _temp5464 >= _temp5462.last_plus_one){ _throw(
Null_Exception);}* _temp5464;}).last_line))};( struct _xenum_struct*) _temp5449;});
break; case 291: _LL5448: yyval=({ struct _tagged_ptr2 _temp5466= yyvs; struct
_xenum_struct** _temp5468= _temp5466.curr + yyvsp_offset; if( _temp5468 <
_temp5466.base? 1: _temp5468 >= _temp5466.last_plus_one){ _throw( Null_Exception);}*
_temp5468;}); break; case 292: _LL5465: yyval=({ struct _tagged_ptr2 _temp5470=
yyvs; struct _xenum_struct** _temp5472= _temp5470.curr + yyvsp_offset; if(
_temp5472 < _temp5470.base? 1: _temp5472 >= _temp5470.last_plus_one){ _throw(
Null_Exception);}* _temp5472;}); break; case 293: _LL5469: yyval=({ struct Cyc_Exp_tok_struct*
_temp5474=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5474=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_or_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5475= yyvs; struct _xenum_struct**
_temp5477= _temp5475.curr +( yyvsp_offset - 2); if( _temp5477 < _temp5475.base?
1: _temp5477 >= _temp5475.last_plus_one){ _throw( Null_Exception);}* _temp5477;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5478= yyvs; struct _xenum_struct**
_temp5480= _temp5478.curr + yyvsp_offset; if( _temp5480 < _temp5478.base? 1:
_temp5480 >= _temp5478.last_plus_one){ _throw( Null_Exception);}* _temp5480;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5481= yyls; struct Cyc_Yyltype*
_temp5483= _temp5481.curr +( yylsp_offset - 2); if( _temp5483 < _temp5481.base?
1: _temp5483 >= _temp5481.last_plus_one){ _throw( Null_Exception);}* _temp5483;}).first_line,({
struct _tagged_ptr3 _temp5484= yyls; struct Cyc_Yyltype* _temp5486= _temp5484.curr
+ yylsp_offset; if( _temp5486 < _temp5484.base? 1: _temp5486 >= _temp5484.last_plus_one){
_throw( Null_Exception);}* _temp5486;}).last_line))};( struct _xenum_struct*)
_temp5474;}); break; case 294: _LL5473: yyval=({ struct _tagged_ptr2 _temp5488=
yyvs; struct _xenum_struct** _temp5490= _temp5488.curr + yyvsp_offset; if(
_temp5490 < _temp5488.base? 1: _temp5490 >= _temp5488.last_plus_one){ _throw(
Null_Exception);}* _temp5490;}); break; case 295: _LL5487: yyval=({ struct Cyc_Exp_tok_struct*
_temp5492=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5492=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_and_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5493= yyvs; struct _xenum_struct**
_temp5495= _temp5493.curr +( yyvsp_offset - 2); if( _temp5495 < _temp5493.base?
1: _temp5495 >= _temp5493.last_plus_one){ _throw( Null_Exception);}* _temp5495;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5496= yyvs; struct _xenum_struct**
_temp5498= _temp5496.curr + yyvsp_offset; if( _temp5498 < _temp5496.base? 1:
_temp5498 >= _temp5496.last_plus_one){ _throw( Null_Exception);}* _temp5498;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5499= yyls; struct Cyc_Yyltype*
_temp5501= _temp5499.curr +( yylsp_offset - 2); if( _temp5501 < _temp5499.base?
1: _temp5501 >= _temp5499.last_plus_one){ _throw( Null_Exception);}* _temp5501;}).first_line,({
struct _tagged_ptr3 _temp5502= yyls; struct Cyc_Yyltype* _temp5504= _temp5502.curr
+ yylsp_offset; if( _temp5504 < _temp5502.base? 1: _temp5504 >= _temp5502.last_plus_one){
_throw( Null_Exception);}* _temp5504;}).last_line))};( struct _xenum_struct*)
_temp5492;}); break; case 296: _LL5491: yyval=({ struct _tagged_ptr2 _temp5506=
yyvs; struct _xenum_struct** _temp5508= _temp5506.curr + yyvsp_offset; if(
_temp5508 < _temp5506.base? 1: _temp5508 >= _temp5506.last_plus_one){ _throw(
Null_Exception);}* _temp5508;}); break; case 297: _LL5505: yyval=({ struct Cyc_Exp_tok_struct*
_temp5510=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5510=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_prim2_exp(
Cyc_Absyn_Bitor, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5511= yyvs;
struct _xenum_struct** _temp5513= _temp5511.curr +( yyvsp_offset - 2); if(
_temp5513 < _temp5511.base? 1: _temp5513 >= _temp5511.last_plus_one){ _throw(
Null_Exception);}* _temp5513;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5514= yyvs; struct _xenum_struct** _temp5516= _temp5514.curr + yyvsp_offset;
if( _temp5516 < _temp5514.base? 1: _temp5516 >= _temp5514.last_plus_one){ _throw(
Null_Exception);}* _temp5516;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5517= yyls; struct Cyc_Yyltype* _temp5519= _temp5517.curr +(
yylsp_offset - 2); if( _temp5519 < _temp5517.base? 1: _temp5519 >= _temp5517.last_plus_one){
_throw( Null_Exception);}* _temp5519;}).first_line,({ struct _tagged_ptr3
_temp5520= yyls; struct Cyc_Yyltype* _temp5522= _temp5520.curr + yylsp_offset;
if( _temp5522 < _temp5520.base? 1: _temp5522 >= _temp5520.last_plus_one){ _throw(
Null_Exception);}* _temp5522;}).last_line))};( struct _xenum_struct*) _temp5510;});
break; case 298: _LL5509: yyval=({ struct _tagged_ptr2 _temp5524= yyvs; struct
_xenum_struct** _temp5526= _temp5524.curr + yyvsp_offset; if( _temp5526 <
_temp5524.base? 1: _temp5526 >= _temp5524.last_plus_one){ _throw( Null_Exception);}*
_temp5526;}); break; case 299: _LL5523: yyval=({ struct Cyc_Exp_tok_struct*
_temp5528=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5528=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_prim2_exp(
Cyc_Absyn_Bitxor, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5529= yyvs;
struct _xenum_struct** _temp5531= _temp5529.curr +( yyvsp_offset - 2); if(
_temp5531 < _temp5529.base? 1: _temp5531 >= _temp5529.last_plus_one){ _throw(
Null_Exception);}* _temp5531;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5532= yyvs; struct _xenum_struct** _temp5534= _temp5532.curr + yyvsp_offset;
if( _temp5534 < _temp5532.base? 1: _temp5534 >= _temp5532.last_plus_one){ _throw(
Null_Exception);}* _temp5534;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5535= yyls; struct Cyc_Yyltype* _temp5537= _temp5535.curr +(
yylsp_offset - 2); if( _temp5537 < _temp5535.base? 1: _temp5537 >= _temp5535.last_plus_one){
_throw( Null_Exception);}* _temp5537;}).first_line,({ struct _tagged_ptr3
_temp5538= yyls; struct Cyc_Yyltype* _temp5540= _temp5538.curr + yylsp_offset;
if( _temp5540 < _temp5538.base? 1: _temp5540 >= _temp5538.last_plus_one){ _throw(
Null_Exception);}* _temp5540;}).last_line))};( struct _xenum_struct*) _temp5528;});
break; case 300: _LL5527: yyval=({ struct _tagged_ptr2 _temp5542= yyvs; struct
_xenum_struct** _temp5544= _temp5542.curr + yyvsp_offset; if( _temp5544 <
_temp5542.base? 1: _temp5544 >= _temp5542.last_plus_one){ _throw( Null_Exception);}*
_temp5544;}); break; case 301: _LL5541: yyval=({ struct Cyc_Exp_tok_struct*
_temp5546=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5546=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_prim2_exp(
Cyc_Absyn_Bitand, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5547= yyvs;
struct _xenum_struct** _temp5549= _temp5547.curr +( yyvsp_offset - 2); if(
_temp5549 < _temp5547.base? 1: _temp5549 >= _temp5547.last_plus_one){ _throw(
Null_Exception);}* _temp5549;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5550= yyvs; struct _xenum_struct** _temp5552= _temp5550.curr + yyvsp_offset;
if( _temp5552 < _temp5550.base? 1: _temp5552 >= _temp5550.last_plus_one){ _throw(
Null_Exception);}* _temp5552;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5553= yyls; struct Cyc_Yyltype* _temp5555= _temp5553.curr +(
yylsp_offset - 2); if( _temp5555 < _temp5553.base? 1: _temp5555 >= _temp5553.last_plus_one){
_throw( Null_Exception);}* _temp5555;}).first_line,({ struct _tagged_ptr3
_temp5556= yyls; struct Cyc_Yyltype* _temp5558= _temp5556.curr + yylsp_offset;
if( _temp5558 < _temp5556.base? 1: _temp5558 >= _temp5556.last_plus_one){ _throw(
Null_Exception);}* _temp5558;}).last_line))};( struct _xenum_struct*) _temp5546;});
break; case 302: _LL5545: yyval=({ struct _tagged_ptr2 _temp5560= yyvs; struct
_xenum_struct** _temp5562= _temp5560.curr + yyvsp_offset; if( _temp5562 <
_temp5560.base? 1: _temp5562 >= _temp5560.last_plus_one){ _throw( Null_Exception);}*
_temp5562;}); break; case 303: _LL5559: yyval=({ struct Cyc_Exp_tok_struct*
_temp5564=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5564=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_eq_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5565= yyvs; struct _xenum_struct**
_temp5567= _temp5565.curr +( yyvsp_offset - 2); if( _temp5567 < _temp5565.base?
1: _temp5567 >= _temp5565.last_plus_one){ _throw( Null_Exception);}* _temp5567;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5568= yyvs; struct _xenum_struct**
_temp5570= _temp5568.curr + yyvsp_offset; if( _temp5570 < _temp5568.base? 1:
_temp5570 >= _temp5568.last_plus_one){ _throw( Null_Exception);}* _temp5570;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5571= yyls; struct Cyc_Yyltype*
_temp5573= _temp5571.curr +( yylsp_offset - 2); if( _temp5573 < _temp5571.base?
1: _temp5573 >= _temp5571.last_plus_one){ _throw( Null_Exception);}* _temp5573;}).first_line,({
struct _tagged_ptr3 _temp5574= yyls; struct Cyc_Yyltype* _temp5576= _temp5574.curr
+ yylsp_offset; if( _temp5576 < _temp5574.base? 1: _temp5576 >= _temp5574.last_plus_one){
_throw( Null_Exception);}* _temp5576;}).last_line))};( struct _xenum_struct*)
_temp5564;}); break; case 304: _LL5563: yyval=({ struct Cyc_Exp_tok_struct*
_temp5578=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5578=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_neq_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5579= yyvs; struct _xenum_struct**
_temp5581= _temp5579.curr +( yyvsp_offset - 2); if( _temp5581 < _temp5579.base?
1: _temp5581 >= _temp5579.last_plus_one){ _throw( Null_Exception);}* _temp5581;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5582= yyvs; struct _xenum_struct**
_temp5584= _temp5582.curr + yyvsp_offset; if( _temp5584 < _temp5582.base? 1:
_temp5584 >= _temp5582.last_plus_one){ _throw( Null_Exception);}* _temp5584;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5585= yyls; struct Cyc_Yyltype*
_temp5587= _temp5585.curr +( yylsp_offset - 2); if( _temp5587 < _temp5585.base?
1: _temp5587 >= _temp5585.last_plus_one){ _throw( Null_Exception);}* _temp5587;}).first_line,({
struct _tagged_ptr3 _temp5588= yyls; struct Cyc_Yyltype* _temp5590= _temp5588.curr
+ yylsp_offset; if( _temp5590 < _temp5588.base? 1: _temp5590 >= _temp5588.last_plus_one){
_throw( Null_Exception);}* _temp5590;}).last_line))};( struct _xenum_struct*)
_temp5578;}); break; case 305: _LL5577: yyval=({ struct _tagged_ptr2 _temp5592=
yyvs; struct _xenum_struct** _temp5594= _temp5592.curr + yyvsp_offset; if(
_temp5594 < _temp5592.base? 1: _temp5594 >= _temp5592.last_plus_one){ _throw(
Null_Exception);}* _temp5594;}); break; case 306: _LL5591: yyval=({ struct Cyc_Exp_tok_struct*
_temp5596=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5596=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_lt_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5597= yyvs; struct _xenum_struct**
_temp5599= _temp5597.curr +( yyvsp_offset - 2); if( _temp5599 < _temp5597.base?
1: _temp5599 >= _temp5597.last_plus_one){ _throw( Null_Exception);}* _temp5599;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5600= yyvs; struct _xenum_struct**
_temp5602= _temp5600.curr + yyvsp_offset; if( _temp5602 < _temp5600.base? 1:
_temp5602 >= _temp5600.last_plus_one){ _throw( Null_Exception);}* _temp5602;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5603= yyls; struct Cyc_Yyltype*
_temp5605= _temp5603.curr +( yylsp_offset - 2); if( _temp5605 < _temp5603.base?
1: _temp5605 >= _temp5603.last_plus_one){ _throw( Null_Exception);}* _temp5605;}).first_line,({
struct _tagged_ptr3 _temp5606= yyls; struct Cyc_Yyltype* _temp5608= _temp5606.curr
+ yylsp_offset; if( _temp5608 < _temp5606.base? 1: _temp5608 >= _temp5606.last_plus_one){
_throw( Null_Exception);}* _temp5608;}).last_line))};( struct _xenum_struct*)
_temp5596;}); break; case 307: _LL5595: yyval=({ struct Cyc_Exp_tok_struct*
_temp5610=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5610=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_gt_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5611= yyvs; struct _xenum_struct**
_temp5613= _temp5611.curr +( yyvsp_offset - 2); if( _temp5613 < _temp5611.base?
1: _temp5613 >= _temp5611.last_plus_one){ _throw( Null_Exception);}* _temp5613;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5614= yyvs; struct _xenum_struct**
_temp5616= _temp5614.curr + yyvsp_offset; if( _temp5616 < _temp5614.base? 1:
_temp5616 >= _temp5614.last_plus_one){ _throw( Null_Exception);}* _temp5616;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5617= yyls; struct Cyc_Yyltype*
_temp5619= _temp5617.curr +( yylsp_offset - 2); if( _temp5619 < _temp5617.base?
1: _temp5619 >= _temp5617.last_plus_one){ _throw( Null_Exception);}* _temp5619;}).first_line,({
struct _tagged_ptr3 _temp5620= yyls; struct Cyc_Yyltype* _temp5622= _temp5620.curr
+ yylsp_offset; if( _temp5622 < _temp5620.base? 1: _temp5622 >= _temp5620.last_plus_one){
_throw( Null_Exception);}* _temp5622;}).last_line))};( struct _xenum_struct*)
_temp5610;}); break; case 308: _LL5609: yyval=({ struct Cyc_Exp_tok_struct*
_temp5624=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5624=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_lte_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5625= yyvs; struct _xenum_struct**
_temp5627= _temp5625.curr +( yyvsp_offset - 2); if( _temp5627 < _temp5625.base?
1: _temp5627 >= _temp5625.last_plus_one){ _throw( Null_Exception);}* _temp5627;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5628= yyvs; struct _xenum_struct**
_temp5630= _temp5628.curr + yyvsp_offset; if( _temp5630 < _temp5628.base? 1:
_temp5630 >= _temp5628.last_plus_one){ _throw( Null_Exception);}* _temp5630;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5631= yyls; struct Cyc_Yyltype*
_temp5633= _temp5631.curr +( yylsp_offset - 2); if( _temp5633 < _temp5631.base?
1: _temp5633 >= _temp5631.last_plus_one){ _throw( Null_Exception);}* _temp5633;}).first_line,({
struct _tagged_ptr3 _temp5634= yyls; struct Cyc_Yyltype* _temp5636= _temp5634.curr
+ yylsp_offset; if( _temp5636 < _temp5634.base? 1: _temp5636 >= _temp5634.last_plus_one){
_throw( Null_Exception);}* _temp5636;}).last_line))};( struct _xenum_struct*)
_temp5624;}); break; case 309: _LL5623: yyval=({ struct Cyc_Exp_tok_struct*
_temp5638=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5638=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_gte_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5639= yyvs; struct _xenum_struct**
_temp5641= _temp5639.curr +( yyvsp_offset - 2); if( _temp5641 < _temp5639.base?
1: _temp5641 >= _temp5639.last_plus_one){ _throw( Null_Exception);}* _temp5641;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5642= yyvs; struct _xenum_struct**
_temp5644= _temp5642.curr + yyvsp_offset; if( _temp5644 < _temp5642.base? 1:
_temp5644 >= _temp5642.last_plus_one){ _throw( Null_Exception);}* _temp5644;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5645= yyls; struct Cyc_Yyltype*
_temp5647= _temp5645.curr +( yylsp_offset - 2); if( _temp5647 < _temp5645.base?
1: _temp5647 >= _temp5645.last_plus_one){ _throw( Null_Exception);}* _temp5647;}).first_line,({
struct _tagged_ptr3 _temp5648= yyls; struct Cyc_Yyltype* _temp5650= _temp5648.curr
+ yylsp_offset; if( _temp5650 < _temp5648.base? 1: _temp5650 >= _temp5648.last_plus_one){
_throw( Null_Exception);}* _temp5650;}).last_line))};( struct _xenum_struct*)
_temp5638;}); break; case 310: _LL5637: yyval=({ struct _tagged_ptr2 _temp5652=
yyvs; struct _xenum_struct** _temp5654= _temp5652.curr + yyvsp_offset; if(
_temp5654 < _temp5652.base? 1: _temp5654 >= _temp5652.last_plus_one){ _throw(
Null_Exception);}* _temp5654;}); break; case 311: _LL5651: yyval=({ struct Cyc_Exp_tok_struct*
_temp5656=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5656=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_prim2_exp(
Cyc_Absyn_Bitlshift, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5657= yyvs;
struct _xenum_struct** _temp5659= _temp5657.curr +( yyvsp_offset - 2); if(
_temp5659 < _temp5657.base? 1: _temp5659 >= _temp5657.last_plus_one){ _throw(
Null_Exception);}* _temp5659;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5660= yyvs; struct _xenum_struct** _temp5662= _temp5660.curr + yyvsp_offset;
if( _temp5662 < _temp5660.base? 1: _temp5662 >= _temp5660.last_plus_one){ _throw(
Null_Exception);}* _temp5662;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5663= yyls; struct Cyc_Yyltype* _temp5665= _temp5663.curr +(
yylsp_offset - 2); if( _temp5665 < _temp5663.base? 1: _temp5665 >= _temp5663.last_plus_one){
_throw( Null_Exception);}* _temp5665;}).first_line,({ struct _tagged_ptr3
_temp5666= yyls; struct Cyc_Yyltype* _temp5668= _temp5666.curr + yylsp_offset;
if( _temp5668 < _temp5666.base? 1: _temp5668 >= _temp5666.last_plus_one){ _throw(
Null_Exception);}* _temp5668;}).last_line))};( struct _xenum_struct*) _temp5656;});
break; case 312: _LL5655: yyval=({ struct Cyc_Exp_tok_struct* _temp5670=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp5670=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_prim2_exp( Cyc_Absyn_Bitlrshift,
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5671= yyvs; struct _xenum_struct**
_temp5673= _temp5671.curr +( yyvsp_offset - 2); if( _temp5673 < _temp5671.base?
1: _temp5673 >= _temp5671.last_plus_one){ _throw( Null_Exception);}* _temp5673;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5674= yyvs; struct _xenum_struct**
_temp5676= _temp5674.curr + yyvsp_offset; if( _temp5676 < _temp5674.base? 1:
_temp5676 >= _temp5674.last_plus_one){ _throw( Null_Exception);}* _temp5676;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5677= yyls; struct Cyc_Yyltype*
_temp5679= _temp5677.curr +( yylsp_offset - 2); if( _temp5679 < _temp5677.base?
1: _temp5679 >= _temp5677.last_plus_one){ _throw( Null_Exception);}* _temp5679;}).first_line,({
struct _tagged_ptr3 _temp5680= yyls; struct Cyc_Yyltype* _temp5682= _temp5680.curr
+ yylsp_offset; if( _temp5682 < _temp5680.base? 1: _temp5682 >= _temp5680.last_plus_one){
_throw( Null_Exception);}* _temp5682;}).last_line))};( struct _xenum_struct*)
_temp5670;}); break; case 313: _LL5669: yyval=({ struct _tagged_ptr2 _temp5684=
yyvs; struct _xenum_struct** _temp5686= _temp5684.curr + yyvsp_offset; if(
_temp5686 < _temp5684.base? 1: _temp5686 >= _temp5684.last_plus_one){ _throw(
Null_Exception);}* _temp5686;}); break; case 314: _LL5683: yyval=({ struct Cyc_Exp_tok_struct*
_temp5688=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5688=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_prim2_exp(
Cyc_Absyn_Plus, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5689= yyvs; struct
_xenum_struct** _temp5691= _temp5689.curr +( yyvsp_offset - 2); if( _temp5691 <
_temp5689.base? 1: _temp5691 >= _temp5689.last_plus_one){ _throw( Null_Exception);}*
_temp5691;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5692= yyvs; struct
_xenum_struct** _temp5694= _temp5692.curr + yyvsp_offset; if( _temp5694 <
_temp5692.base? 1: _temp5694 >= _temp5692.last_plus_one){ _throw( Null_Exception);}*
_temp5694;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5695=
yyls; struct Cyc_Yyltype* _temp5697= _temp5695.curr +( yylsp_offset - 2); if(
_temp5697 < _temp5695.base? 1: _temp5697 >= _temp5695.last_plus_one){ _throw(
Null_Exception);}* _temp5697;}).first_line,({ struct _tagged_ptr3 _temp5698=
yyls; struct Cyc_Yyltype* _temp5700= _temp5698.curr + yylsp_offset; if(
_temp5700 < _temp5698.base? 1: _temp5700 >= _temp5698.last_plus_one){ _throw(
Null_Exception);}* _temp5700;}).last_line))};( struct _xenum_struct*) _temp5688;});
break; case 315: _LL5687: yyval=({ struct Cyc_Exp_tok_struct* _temp5702=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp5702=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_prim2_exp( Cyc_Absyn_Minus,
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5703= yyvs; struct _xenum_struct**
_temp5705= _temp5703.curr +( yyvsp_offset - 2); if( _temp5705 < _temp5703.base?
1: _temp5705 >= _temp5703.last_plus_one){ _throw( Null_Exception);}* _temp5705;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5706= yyvs; struct _xenum_struct**
_temp5708= _temp5706.curr + yyvsp_offset; if( _temp5708 < _temp5706.base? 1:
_temp5708 >= _temp5706.last_plus_one){ _throw( Null_Exception);}* _temp5708;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5709= yyls; struct Cyc_Yyltype*
_temp5711= _temp5709.curr +( yylsp_offset - 2); if( _temp5711 < _temp5709.base?
1: _temp5711 >= _temp5709.last_plus_one){ _throw( Null_Exception);}* _temp5711;}).first_line,({
struct _tagged_ptr3 _temp5712= yyls; struct Cyc_Yyltype* _temp5714= _temp5712.curr
+ yylsp_offset; if( _temp5714 < _temp5712.base? 1: _temp5714 >= _temp5712.last_plus_one){
_throw( Null_Exception);}* _temp5714;}).last_line))};( struct _xenum_struct*)
_temp5702;}); break; case 316: _LL5701: yyval=({ struct _tagged_ptr2 _temp5716=
yyvs; struct _xenum_struct** _temp5718= _temp5716.curr + yyvsp_offset; if(
_temp5718 < _temp5716.base? 1: _temp5718 >= _temp5716.last_plus_one){ _throw(
Null_Exception);}* _temp5718;}); break; case 317: _LL5715: yyval=({ struct Cyc_Exp_tok_struct*
_temp5720=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5720=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_prim2_exp(
Cyc_Absyn_Times, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5721= yyvs;
struct _xenum_struct** _temp5723= _temp5721.curr +( yyvsp_offset - 2); if(
_temp5723 < _temp5721.base? 1: _temp5723 >= _temp5721.last_plus_one){ _throw(
Null_Exception);}* _temp5723;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5724= yyvs; struct _xenum_struct** _temp5726= _temp5724.curr + yyvsp_offset;
if( _temp5726 < _temp5724.base? 1: _temp5726 >= _temp5724.last_plus_one){ _throw(
Null_Exception);}* _temp5726;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5727= yyls; struct Cyc_Yyltype* _temp5729= _temp5727.curr +(
yylsp_offset - 2); if( _temp5729 < _temp5727.base? 1: _temp5729 >= _temp5727.last_plus_one){
_throw( Null_Exception);}* _temp5729;}).first_line,({ struct _tagged_ptr3
_temp5730= yyls; struct Cyc_Yyltype* _temp5732= _temp5730.curr + yylsp_offset;
if( _temp5732 < _temp5730.base? 1: _temp5732 >= _temp5730.last_plus_one){ _throw(
Null_Exception);}* _temp5732;}).last_line))};( struct _xenum_struct*) _temp5720;});
break; case 318: _LL5719: yyval=({ struct Cyc_Exp_tok_struct* _temp5734=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp5734=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_prim2_exp( Cyc_Absyn_Div,
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5735= yyvs; struct _xenum_struct**
_temp5737= _temp5735.curr +( yyvsp_offset - 2); if( _temp5737 < _temp5735.base?
1: _temp5737 >= _temp5735.last_plus_one){ _throw( Null_Exception);}* _temp5737;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5738= yyvs; struct _xenum_struct**
_temp5740= _temp5738.curr + yyvsp_offset; if( _temp5740 < _temp5738.base? 1:
_temp5740 >= _temp5738.last_plus_one){ _throw( Null_Exception);}* _temp5740;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5741= yyls; struct Cyc_Yyltype*
_temp5743= _temp5741.curr +( yylsp_offset - 2); if( _temp5743 < _temp5741.base?
1: _temp5743 >= _temp5741.last_plus_one){ _throw( Null_Exception);}* _temp5743;}).first_line,({
struct _tagged_ptr3 _temp5744= yyls; struct Cyc_Yyltype* _temp5746= _temp5744.curr
+ yylsp_offset; if( _temp5746 < _temp5744.base? 1: _temp5746 >= _temp5744.last_plus_one){
_throw( Null_Exception);}* _temp5746;}).last_line))};( struct _xenum_struct*)
_temp5734;}); break; case 319: _LL5733: yyval=({ struct Cyc_Exp_tok_struct*
_temp5748=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5748=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_prim2_exp(
Cyc_Absyn_Mod, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5749= yyvs; struct
_xenum_struct** _temp5751= _temp5749.curr +( yyvsp_offset - 2); if( _temp5751 <
_temp5749.base? 1: _temp5751 >= _temp5749.last_plus_one){ _throw( Null_Exception);}*
_temp5751;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5752= yyvs; struct
_xenum_struct** _temp5754= _temp5752.curr + yyvsp_offset; if( _temp5754 <
_temp5752.base? 1: _temp5754 >= _temp5752.last_plus_one){ _throw( Null_Exception);}*
_temp5754;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5755=
yyls; struct Cyc_Yyltype* _temp5757= _temp5755.curr +( yylsp_offset - 2); if(
_temp5757 < _temp5755.base? 1: _temp5757 >= _temp5755.last_plus_one){ _throw(
Null_Exception);}* _temp5757;}).first_line,({ struct _tagged_ptr3 _temp5758=
yyls; struct Cyc_Yyltype* _temp5760= _temp5758.curr + yylsp_offset; if(
_temp5760 < _temp5758.base? 1: _temp5760 >= _temp5758.last_plus_one){ _throw(
Null_Exception);}* _temp5760;}).last_line))};( struct _xenum_struct*) _temp5748;});
break; case 320: _LL5747: yyval=({ struct _tagged_ptr2 _temp5762= yyvs; struct
_xenum_struct** _temp5764= _temp5762.curr + yyvsp_offset; if( _temp5764 <
_temp5762.base? 1: _temp5764 >= _temp5762.last_plus_one){ _throw( Null_Exception);}*
_temp5764;}); break; case 321: _LL5761: yyval=({ struct Cyc_Exp_tok_struct*
_temp5766=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5766=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_cast_exp((*
Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp5767= yyvs; struct
_xenum_struct** _temp5769= _temp5767.curr +( yyvsp_offset - 2); if( _temp5769 <
_temp5767.base? 1: _temp5769 >= _temp5767.last_plus_one){ _throw( Null_Exception);}*
_temp5769;}))).f3, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5770= yyvs;
struct _xenum_struct** _temp5772= _temp5770.curr + yyvsp_offset; if( _temp5772 <
_temp5770.base? 1: _temp5772 >= _temp5770.last_plus_one){ _throw( Null_Exception);}*
_temp5772;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5773=
yyls; struct Cyc_Yyltype* _temp5775= _temp5773.curr +( yylsp_offset - 3); if(
_temp5775 < _temp5773.base? 1: _temp5775 >= _temp5773.last_plus_one){ _throw(
Null_Exception);}* _temp5775;}).first_line,({ struct _tagged_ptr3 _temp5776=
yyls; struct Cyc_Yyltype* _temp5778= _temp5776.curr + yylsp_offset; if(
_temp5778 < _temp5776.base? 1: _temp5778 >= _temp5776.last_plus_one){ _throw(
Null_Exception);}* _temp5778;}).last_line))};( struct _xenum_struct*) _temp5766;});
break; case 322: _LL5765: yyval=({ struct _tagged_ptr2 _temp5780= yyvs; struct
_xenum_struct** _temp5782= _temp5780.curr + yyvsp_offset; if( _temp5782 <
_temp5780.base? 1: _temp5782 >= _temp5780.last_plus_one){ _throw( Null_Exception);}*
_temp5782;}); break; case 323: _LL5779: yyval=({ struct Cyc_Exp_tok_struct*
_temp5784=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5784=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_pre_inc_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5785= yyvs; struct _xenum_struct**
_temp5787= _temp5785.curr + yyvsp_offset; if( _temp5787 < _temp5785.base? 1:
_temp5787 >= _temp5785.last_plus_one){ _throw( Null_Exception);}* _temp5787;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5788= yyls; struct Cyc_Yyltype*
_temp5790= _temp5788.curr +( yylsp_offset - 1); if( _temp5790 < _temp5788.base?
1: _temp5790 >= _temp5788.last_plus_one){ _throw( Null_Exception);}* _temp5790;}).first_line,({
struct _tagged_ptr3 _temp5791= yyls; struct Cyc_Yyltype* _temp5793= _temp5791.curr
+ yylsp_offset; if( _temp5793 < _temp5791.base? 1: _temp5793 >= _temp5791.last_plus_one){
_throw( Null_Exception);}* _temp5793;}).last_line))};( struct _xenum_struct*)
_temp5784;}); break; case 324: _LL5783: yyval=({ struct Cyc_Exp_tok_struct*
_temp5795=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5795=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_pre_dec_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5796= yyvs; struct _xenum_struct**
_temp5798= _temp5796.curr + yyvsp_offset; if( _temp5798 < _temp5796.base? 1:
_temp5798 >= _temp5796.last_plus_one){ _throw( Null_Exception);}* _temp5798;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5799= yyls; struct Cyc_Yyltype*
_temp5801= _temp5799.curr +( yylsp_offset - 1); if( _temp5801 < _temp5799.base?
1: _temp5801 >= _temp5799.last_plus_one){ _throw( Null_Exception);}* _temp5801;}).first_line,({
struct _tagged_ptr3 _temp5802= yyls; struct Cyc_Yyltype* _temp5804= _temp5802.curr
+ yylsp_offset; if( _temp5804 < _temp5802.base? 1: _temp5804 >= _temp5802.last_plus_one){
_throw( Null_Exception);}* _temp5804;}).last_line))};( struct _xenum_struct*)
_temp5795;}); break; case 325: _LL5794: yyval=({ struct Cyc_Exp_tok_struct*
_temp5806=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5806=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_address_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5807= yyvs; struct _xenum_struct**
_temp5809= _temp5807.curr + yyvsp_offset; if( _temp5809 < _temp5807.base? 1:
_temp5809 >= _temp5807.last_plus_one){ _throw( Null_Exception);}* _temp5809;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5810= yyls; struct Cyc_Yyltype*
_temp5812= _temp5810.curr +( yylsp_offset - 1); if( _temp5812 < _temp5810.base?
1: _temp5812 >= _temp5810.last_plus_one){ _throw( Null_Exception);}* _temp5812;}).first_line,({
struct _tagged_ptr3 _temp5813= yyls; struct Cyc_Yyltype* _temp5815= _temp5813.curr
+ yylsp_offset; if( _temp5815 < _temp5813.base? 1: _temp5815 >= _temp5813.last_plus_one){
_throw( Null_Exception);}* _temp5815;}).last_line))};( struct _xenum_struct*)
_temp5806;}); break; case 326: _LL5805: yyval=({ struct Cyc_Exp_tok_struct*
_temp5817=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5817=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_deref_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5818= yyvs; struct _xenum_struct**
_temp5820= _temp5818.curr + yyvsp_offset; if( _temp5820 < _temp5818.base? 1:
_temp5820 >= _temp5818.last_plus_one){ _throw( Null_Exception);}* _temp5820;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5821= yyls; struct Cyc_Yyltype*
_temp5823= _temp5821.curr +( yylsp_offset - 1); if( _temp5823 < _temp5821.base?
1: _temp5823 >= _temp5821.last_plus_one){ _throw( Null_Exception);}* _temp5823;}).first_line,({
struct _tagged_ptr3 _temp5824= yyls; struct Cyc_Yyltype* _temp5826= _temp5824.curr
+ yylsp_offset; if( _temp5826 < _temp5824.base? 1: _temp5826 >= _temp5824.last_plus_one){
_throw( Null_Exception);}* _temp5826;}).last_line))};( struct _xenum_struct*)
_temp5817;}); break; case 327: _LL5816: yyval=({ struct _tagged_ptr2 _temp5828=
yyvs; struct _xenum_struct** _temp5830= _temp5828.curr + yyvsp_offset; if(
_temp5830 < _temp5828.base? 1: _temp5830 >= _temp5828.last_plus_one){ _throw(
Null_Exception);}* _temp5830;}); break; case 328: _LL5827: yyval=({ struct Cyc_Exp_tok_struct*
_temp5832=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5832=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_prim1_exp(
Cyc_yyget_Primop_tok(({ struct _tagged_ptr2 _temp5833= yyvs; struct
_xenum_struct** _temp5835= _temp5833.curr +( yyvsp_offset - 1); if( _temp5835 <
_temp5833.base? 1: _temp5835 >= _temp5833.last_plus_one){ _throw( Null_Exception);}*
_temp5835;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5836= yyvs; struct
_xenum_struct** _temp5838= _temp5836.curr + yyvsp_offset; if( _temp5838 <
_temp5836.base? 1: _temp5838 >= _temp5836.last_plus_one){ _throw( Null_Exception);}*
_temp5838;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5839=
yyls; struct Cyc_Yyltype* _temp5841= _temp5839.curr +( yylsp_offset - 1); if(
_temp5841 < _temp5839.base? 1: _temp5841 >= _temp5839.last_plus_one){ _throw(
Null_Exception);}* _temp5841;}).first_line,({ struct _tagged_ptr3 _temp5842=
yyls; struct Cyc_Yyltype* _temp5844= _temp5842.curr + yylsp_offset; if(
_temp5844 < _temp5842.base? 1: _temp5844 >= _temp5842.last_plus_one){ _throw(
Null_Exception);}* _temp5844;}).last_line))};( struct _xenum_struct*) _temp5832;});
break; case 329: _LL5831: yyval=({ struct Cyc_Exp_tok_struct* _temp5846=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp5846=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_sizeof_exp((*
Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp5847= yyvs; struct
_xenum_struct** _temp5849= _temp5847.curr +( yyvsp_offset - 1); if( _temp5849 <
_temp5847.base? 1: _temp5849 >= _temp5847.last_plus_one){ _throw( Null_Exception);}*
_temp5849;}))).f3, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5850=
yyls; struct Cyc_Yyltype* _temp5852= _temp5850.curr +( yylsp_offset - 3); if(
_temp5852 < _temp5850.base? 1: _temp5852 >= _temp5850.last_plus_one){ _throw(
Null_Exception);}* _temp5852;}).first_line,({ struct _tagged_ptr3 _temp5853=
yyls; struct Cyc_Yyltype* _temp5855= _temp5853.curr + yylsp_offset; if(
_temp5855 < _temp5853.base? 1: _temp5855 >= _temp5853.last_plus_one){ _throw(
Null_Exception);}* _temp5855;}).last_line))};( struct _xenum_struct*) _temp5846;});
break; case 330: _LL5845: yyval=({ struct Cyc_Exp_tok_struct* _temp5857=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp5857=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_prim1_exp( Cyc_Absyn_Size,
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5858= yyvs; struct _xenum_struct**
_temp5860= _temp5858.curr + yyvsp_offset; if( _temp5860 < _temp5858.base? 1:
_temp5860 >= _temp5858.last_plus_one){ _throw( Null_Exception);}* _temp5860;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5861= yyls; struct Cyc_Yyltype*
_temp5863= _temp5861.curr +( yylsp_offset - 1); if( _temp5863 < _temp5861.base?
1: _temp5863 >= _temp5861.last_plus_one){ _throw( Null_Exception);}* _temp5863;}).first_line,({
struct _tagged_ptr3 _temp5864= yyls; struct Cyc_Yyltype* _temp5866= _temp5864.curr
+ yylsp_offset; if( _temp5866 < _temp5864.base? 1: _temp5866 >= _temp5864.last_plus_one){
_throw( Null_Exception);}* _temp5866;}).last_line))};( struct _xenum_struct*)
_temp5857;}); break; case 331: _LL5856: yyval=({ struct Cyc_Exp_tok_struct*
_temp5868=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5868=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_throw_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5869= yyvs; struct _xenum_struct**
_temp5871= _temp5869.curr + yyvsp_offset; if( _temp5871 < _temp5869.base? 1:
_temp5871 >= _temp5869.last_plus_one){ _throw( Null_Exception);}* _temp5871;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5872= yyls; struct Cyc_Yyltype*
_temp5874= _temp5872.curr +( yylsp_offset - 1); if( _temp5874 < _temp5872.base?
1: _temp5874 >= _temp5872.last_plus_one){ _throw( Null_Exception);}* _temp5874;}).first_line,({
struct _tagged_ptr3 _temp5875= yyls; struct Cyc_Yyltype* _temp5877= _temp5875.curr
+ yylsp_offset; if( _temp5877 < _temp5875.base? 1: _temp5877 >= _temp5875.last_plus_one){
_throw( Null_Exception);}* _temp5877;}).last_line))};( struct _xenum_struct*)
_temp5868;}); break; case 332: _LL5867: yyval=({ struct Cyc_Exp_tok_struct*
_temp5879=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5879=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_primop_exp(
Cyc_yyget_Primop_tok(({ struct _tagged_ptr2 _temp5880= yyvs; struct
_xenum_struct** _temp5882= _temp5880.curr +( yyvsp_offset - 3); if( _temp5882 <
_temp5880.base? 1: _temp5882 >= _temp5880.last_plus_one){ _throw( Null_Exception);}*
_temp5882;})), Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2 _temp5883= yyvs;
struct _xenum_struct** _temp5885= _temp5883.curr +( yyvsp_offset - 1); if(
_temp5885 < _temp5883.base? 1: _temp5885 >= _temp5883.last_plus_one){ _throw(
Null_Exception);}* _temp5885;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5886= yyls; struct Cyc_Yyltype* _temp5888= _temp5886.curr +(
yylsp_offset - 3); if( _temp5888 < _temp5886.base? 1: _temp5888 >= _temp5886.last_plus_one){
_throw( Null_Exception);}* _temp5888;}).first_line,({ struct _tagged_ptr3
_temp5889= yyls; struct Cyc_Yyltype* _temp5891= _temp5889.curr + yylsp_offset;
if( _temp5891 < _temp5889.base? 1: _temp5891 >= _temp5889.last_plus_one){ _throw(
Null_Exception);}* _temp5891;}).last_line))};( struct _xenum_struct*) _temp5879;});
break; case 333: _LL5878: yyval=({ struct Cyc_Exp_tok_struct* _temp5893=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp5893=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_Malloc_e_struct* _temp5894=( struct Cyc_Absyn_Malloc_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct)); _temp5894->tag= Cyc_Absyn_Malloc_e_tag;
_temp5894->f1=( void*)({ struct Cyc_Absyn_Typ_m_struct* _temp5895=( struct Cyc_Absyn_Typ_m_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Typ_m_struct)); _temp5895->tag= Cyc_Absyn_Typ_m_tag;
_temp5895->f1=( void*) Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp5896= yyvs; struct _xenum_struct** _temp5898= _temp5896.curr
+( yyvsp_offset - 2); if( _temp5898 < _temp5896.base? 1: _temp5898 >= _temp5896.last_plus_one){
_throw( Null_Exception);}* _temp5898;}))).f2, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5899= yyls; struct Cyc_Yyltype* _temp5901= _temp5899.curr
+( yylsp_offset - 2); if( _temp5901 < _temp5899.base? 1: _temp5901 >= _temp5899.last_plus_one){
_throw( Null_Exception);}* _temp5901;}).first_line,({ struct _tagged_ptr3
_temp5902= yyls; struct Cyc_Yyltype* _temp5904= _temp5902.curr +( yylsp_offset -
2); if( _temp5904 < _temp5902.base? 1: _temp5904 >= _temp5902.last_plus_one){
_throw( Null_Exception);}* _temp5904;}).last_line));( void*) _temp5895;});( void*)
_temp5894;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5905= yyls;
struct Cyc_Yyltype* _temp5907= _temp5905.curr +( yylsp_offset - 6); if(
_temp5907 < _temp5905.base? 1: _temp5907 >= _temp5905.last_plus_one){ _throw(
Null_Exception);}* _temp5907;}).first_line,({ struct _tagged_ptr3 _temp5908=
yyls; struct Cyc_Yyltype* _temp5910= _temp5908.curr + yylsp_offset; if(
_temp5910 < _temp5908.base? 1: _temp5910 >= _temp5908.last_plus_one){ _throw(
Null_Exception);}* _temp5910;}).last_line))};( struct _xenum_struct*) _temp5893;});
break; case 334: _LL5892: yyval=({ struct Cyc_Exp_tok_struct* _temp5912=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp5912=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_Malloc_e_struct* _temp5913=( struct Cyc_Absyn_Malloc_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct)); _temp5913->tag= Cyc_Absyn_Malloc_e_tag;
_temp5913->f1=( void*)({ struct Cyc_Absyn_Unresolved_m_struct* _temp5914=(
struct Cyc_Absyn_Unresolved_m_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Unresolved_m_struct));
_temp5914->tag= Cyc_Absyn_Unresolved_m_tag; _temp5914->f1= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp5915= yyvs; struct _xenum_struct** _temp5917= _temp5915.curr
+( yyvsp_offset - 2); if( _temp5917 < _temp5915.base? 1: _temp5917 >= _temp5915.last_plus_one){
_throw( Null_Exception);}* _temp5917;}));( void*) _temp5914;});( void*)
_temp5913;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5918= yyls;
struct Cyc_Yyltype* _temp5920= _temp5918.curr +( yylsp_offset - 6); if(
_temp5920 < _temp5918.base? 1: _temp5920 >= _temp5918.last_plus_one){ _throw(
Null_Exception);}* _temp5920;}).first_line,({ struct _tagged_ptr3 _temp5921=
yyls; struct Cyc_Yyltype* _temp5923= _temp5921.curr + yylsp_offset; if(
_temp5923 < _temp5921.base? 1: _temp5923 >= _temp5921.last_plus_one){ _throw(
Null_Exception);}* _temp5923;}).last_line))};( struct _xenum_struct*) _temp5912;});
break; case 335: _LL5911: yyval=({ struct Cyc_Primop_tok_struct* _temp5925=(
struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));*
_temp5925=( struct Cyc_Primop_tok_struct){.tag= Cyc_Primop_tok_tag,.f1= Cyc_Absyn_Printf};(
struct _xenum_struct*) _temp5925;}); break; case 336: _LL5924: yyval=({ struct
Cyc_Primop_tok_struct* _temp5927=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct));* _temp5927=( struct Cyc_Primop_tok_struct){.tag=
Cyc_Primop_tok_tag,.f1= Cyc_Absyn_Fprintf};( struct _xenum_struct*) _temp5927;});
break; case 337: _LL5926: yyval=({ struct Cyc_Primop_tok_struct* _temp5929=(
struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));*
_temp5929=( struct Cyc_Primop_tok_struct){.tag= Cyc_Primop_tok_tag,.f1= Cyc_Absyn_Xprintf};(
struct _xenum_struct*) _temp5929;}); break; case 338: _LL5928: yyval=({ struct
Cyc_Primop_tok_struct* _temp5931=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct));* _temp5931=( struct Cyc_Primop_tok_struct){.tag=
Cyc_Primop_tok_tag,.f1= Cyc_Absyn_Scanf};( struct _xenum_struct*) _temp5931;});
break; case 339: _LL5930: yyval=({ struct Cyc_Primop_tok_struct* _temp5933=(
struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));*
_temp5933=( struct Cyc_Primop_tok_struct){.tag= Cyc_Primop_tok_tag,.f1= Cyc_Absyn_Fscanf};(
struct _xenum_struct*) _temp5933;}); break; case 340: _LL5932: yyval=({ struct
Cyc_Primop_tok_struct* _temp5935=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct));* _temp5935=( struct Cyc_Primop_tok_struct){.tag=
Cyc_Primop_tok_tag,.f1= Cyc_Absyn_Sscanf};( struct _xenum_struct*) _temp5935;});
break; case 341: _LL5934: yyval=({ struct Cyc_Primop_tok_struct* _temp5937=(
struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));*
_temp5937=( struct Cyc_Primop_tok_struct){.tag= Cyc_Primop_tok_tag,.f1= Cyc_Absyn_Bitnot};(
struct _xenum_struct*) _temp5937;}); break; case 342: _LL5936: yyval=({ struct
Cyc_Primop_tok_struct* _temp5939=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct));* _temp5939=( struct Cyc_Primop_tok_struct){.tag=
Cyc_Primop_tok_tag,.f1= Cyc_Absyn_Not};( struct _xenum_struct*) _temp5939;});
break; case 343: _LL5938: yyval=({ struct Cyc_Primop_tok_struct* _temp5941=(
struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));*
_temp5941=( struct Cyc_Primop_tok_struct){.tag= Cyc_Primop_tok_tag,.f1= Cyc_Absyn_Minus};(
struct _xenum_struct*) _temp5941;}); break; case 344: _LL5940: yyval=({ struct
_tagged_ptr2 _temp5943= yyvs; struct _xenum_struct** _temp5945= _temp5943.curr +
yyvsp_offset; if( _temp5945 < _temp5943.base? 1: _temp5945 >= _temp5943.last_plus_one){
_throw( Null_Exception);}* _temp5945;}); break; case 345: _LL5942: yyval=({
struct Cyc_Exp_tok_struct* _temp5947=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct));* _temp5947=( struct Cyc_Exp_tok_struct){.tag=
Cyc_Exp_tok_tag,.f1= Cyc_Absyn_subscript_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5948= yyvs; struct _xenum_struct** _temp5950= _temp5948.curr +(
yyvsp_offset - 3); if( _temp5950 < _temp5948.base? 1: _temp5950 >= _temp5948.last_plus_one){
_throw( Null_Exception);}* _temp5950;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5951= yyvs; struct _xenum_struct** _temp5953= _temp5951.curr +(
yyvsp_offset - 1); if( _temp5953 < _temp5951.base? 1: _temp5953 >= _temp5951.last_plus_one){
_throw( Null_Exception);}* _temp5953;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5954= yyls; struct Cyc_Yyltype* _temp5956= _temp5954.curr +(
yylsp_offset - 3); if( _temp5956 < _temp5954.base? 1: _temp5956 >= _temp5954.last_plus_one){
_throw( Null_Exception);}* _temp5956;}).first_line,({ struct _tagged_ptr3
_temp5957= yyls; struct Cyc_Yyltype* _temp5959= _temp5957.curr + yylsp_offset;
if( _temp5959 < _temp5957.base? 1: _temp5959 >= _temp5957.last_plus_one){ _throw(
Null_Exception);}* _temp5959;}).last_line))};( struct _xenum_struct*) _temp5947;});
break; case 346: _LL5946: yyval=({ struct Cyc_Exp_tok_struct* _temp5961=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp5961=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5962= yyvs; struct _xenum_struct**
_temp5964= _temp5962.curr +( yyvsp_offset - 2); if( _temp5964 < _temp5962.base?
1: _temp5964 >= _temp5962.last_plus_one){ _throw( Null_Exception);}* _temp5964;})),
0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5965= yyls; struct
Cyc_Yyltype* _temp5967= _temp5965.curr +( yylsp_offset - 2); if( _temp5967 <
_temp5965.base? 1: _temp5967 >= _temp5965.last_plus_one){ _throw( Null_Exception);}*
_temp5967;}).first_line,({ struct _tagged_ptr3 _temp5968= yyls; struct Cyc_Yyltype*
_temp5970= _temp5968.curr + yylsp_offset; if( _temp5970 < _temp5968.base? 1:
_temp5970 >= _temp5968.last_plus_one){ _throw( Null_Exception);}* _temp5970;}).last_line))};(
struct _xenum_struct*) _temp5961;}); break; case 347: _LL5960: yyval=({ struct
Cyc_Exp_tok_struct* _temp5972=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct));* _temp5972=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1=
Cyc_Absyn_unknowncall_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5973=
yyvs; struct _xenum_struct** _temp5975= _temp5973.curr +( yyvsp_offset - 3); if(
_temp5975 < _temp5973.base? 1: _temp5975 >= _temp5973.last_plus_one){ _throw(
Null_Exception);}* _temp5975;})), Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2
_temp5976= yyvs; struct _xenum_struct** _temp5978= _temp5976.curr +(
yyvsp_offset - 1); if( _temp5978 < _temp5976.base? 1: _temp5978 >= _temp5976.last_plus_one){
_throw( Null_Exception);}* _temp5978;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5979= yyls; struct Cyc_Yyltype* _temp5981= _temp5979.curr +(
yylsp_offset - 3); if( _temp5981 < _temp5979.base? 1: _temp5981 >= _temp5979.last_plus_one){
_throw( Null_Exception);}* _temp5981;}).first_line,({ struct _tagged_ptr3
_temp5982= yyls; struct Cyc_Yyltype* _temp5984= _temp5982.curr + yylsp_offset;
if( _temp5984 < _temp5982.base? 1: _temp5984 >= _temp5982.last_plus_one){ _throw(
Null_Exception);}* _temp5984;}).last_line))};( struct _xenum_struct*) _temp5972;});
break; case 348: _LL5971: yyval=({ struct Cyc_Exp_tok_struct* _temp5986=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp5986=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_structmember_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5987= yyvs; struct _xenum_struct**
_temp5989= _temp5987.curr +( yyvsp_offset - 2); if( _temp5989 < _temp5987.base?
1: _temp5989 >= _temp5987.last_plus_one){ _throw( Null_Exception);}* _temp5989;})),({
struct _tagged_string* _temp5990=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp5990[ 0]= Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp5991= yyvs; struct _xenum_struct** _temp5993= _temp5991.curr +
yyvsp_offset; if( _temp5993 < _temp5991.base? 1: _temp5993 >= _temp5991.last_plus_one){
_throw( Null_Exception);}* _temp5993;})); _temp5990;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5994= yyls; struct Cyc_Yyltype* _temp5996= _temp5994.curr
+( yylsp_offset - 2); if( _temp5996 < _temp5994.base? 1: _temp5996 >= _temp5994.last_plus_one){
_throw( Null_Exception);}* _temp5996;}).first_line,({ struct _tagged_ptr3
_temp5997= yyls; struct Cyc_Yyltype* _temp5999= _temp5997.curr + yylsp_offset;
if( _temp5999 < _temp5997.base? 1: _temp5999 >= _temp5997.last_plus_one){ _throw(
Null_Exception);}* _temp5999;}).last_line))};( struct _xenum_struct*) _temp5986;});
break; case 349: _LL5985: { struct _tuple1* q= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp6001= yyvs; struct _xenum_struct** _temp6003= _temp6001.curr +
yyvsp_offset; if( _temp6003 < _temp6001.base? 1: _temp6003 >= _temp6001.last_plus_one){
_throw( Null_Exception);}* _temp6003;}));{ void* _temp6004=(* q).f1; struct Cyc_List_List*
_temp6014; struct Cyc_List_List* _temp6016; _LL6006: if( _temp6004 == Cyc_Absyn_Loc_n){
goto _LL6007;} else{ goto _LL6008;} _LL6008: if(( unsigned int) _temp6004 > 1u?((
struct _enum_struct*) _temp6004)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL6015:
_temp6014=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp6004)->f1;
if( _temp6014 == 0){ goto _LL6009;} else{ goto _LL6010;}} else{ goto _LL6010;}
_LL6010: if(( unsigned int) _temp6004 > 1u?(( struct _enum_struct*) _temp6004)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL6017: _temp6016=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp6004)->f1; if( _temp6016 == 0){ goto _LL6011;}
else{ goto _LL6012;}} else{ goto _LL6012;} _LL6012: goto _LL6013; _LL6007: goto
_LL6005; _LL6009: goto _LL6005; _LL6011: goto _LL6005; _LL6013: Cyc_Parse_err((
struct _tagged_string)({ char* _temp6018=( char*)"struct field name is qualified";
struct _tagged_string _temp6019; _temp6019.curr= _temp6018; _temp6019.base=
_temp6018; _temp6019.last_plus_one= _temp6018 + 31; _temp6019;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6020= yyls; struct Cyc_Yyltype* _temp6022= _temp6020.curr
+ yylsp_offset; if( _temp6022 < _temp6020.base? 1: _temp6022 >= _temp6020.last_plus_one){
_throw( Null_Exception);}* _temp6022;}).first_line,({ struct _tagged_ptr3
_temp6023= yyls; struct Cyc_Yyltype* _temp6025= _temp6023.curr + yylsp_offset;
if( _temp6025 < _temp6023.base? 1: _temp6025 >= _temp6023.last_plus_one){ _throw(
Null_Exception);}* _temp6025;}).last_line)); goto _LL6005; _LL6005:;} yyval=({
struct Cyc_Exp_tok_struct* _temp6026=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct));* _temp6026=( struct Cyc_Exp_tok_struct){.tag=
Cyc_Exp_tok_tag,.f1= Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6027= yyvs; struct _xenum_struct** _temp6029= _temp6027.curr +(
yyvsp_offset - 2); if( _temp6029 < _temp6027.base? 1: _temp6029 >= _temp6027.last_plus_one){
_throw( Null_Exception);}* _temp6029;})),(* q).f2, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6030= yyls; struct Cyc_Yyltype* _temp6032= _temp6030.curr
+( yylsp_offset - 2); if( _temp6032 < _temp6030.base? 1: _temp6032 >= _temp6030.last_plus_one){
_throw( Null_Exception);}* _temp6032;}).first_line,({ struct _tagged_ptr3
_temp6033= yyls; struct Cyc_Yyltype* _temp6035= _temp6033.curr + yylsp_offset;
if( _temp6035 < _temp6033.base? 1: _temp6035 >= _temp6033.last_plus_one){ _throw(
Null_Exception);}* _temp6035;}).last_line))};( struct _xenum_struct*) _temp6026;});
break;} case 350: _LL6000: yyval=({ struct Cyc_Exp_tok_struct* _temp6037=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp6037=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_structarrow_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6038= yyvs; struct _xenum_struct**
_temp6040= _temp6038.curr +( yyvsp_offset - 2); if( _temp6040 < _temp6038.base?
1: _temp6040 >= _temp6038.last_plus_one){ _throw( Null_Exception);}* _temp6040;})),({
struct _tagged_string* _temp6041=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp6041[ 0]= Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp6042= yyvs; struct _xenum_struct** _temp6044= _temp6042.curr +
yyvsp_offset; if( _temp6044 < _temp6042.base? 1: _temp6044 >= _temp6042.last_plus_one){
_throw( Null_Exception);}* _temp6044;})); _temp6041;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6045= yyls; struct Cyc_Yyltype* _temp6047= _temp6045.curr
+( yylsp_offset - 2); if( _temp6047 < _temp6045.base? 1: _temp6047 >= _temp6045.last_plus_one){
_throw( Null_Exception);}* _temp6047;}).first_line,({ struct _tagged_ptr3
_temp6048= yyls; struct Cyc_Yyltype* _temp6050= _temp6048.curr + yylsp_offset;
if( _temp6050 < _temp6048.base? 1: _temp6050 >= _temp6048.last_plus_one){ _throw(
Null_Exception);}* _temp6050;}).last_line))};( struct _xenum_struct*) _temp6037;});
break; case 351: _LL6036: { struct _tuple1* q= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp6052= yyvs; struct _xenum_struct** _temp6054= _temp6052.curr +
yyvsp_offset; if( _temp6054 < _temp6052.base? 1: _temp6054 >= _temp6052.last_plus_one){
_throw( Null_Exception);}* _temp6054;}));{ void* _temp6055=(* q).f1; struct Cyc_List_List*
_temp6065; struct Cyc_List_List* _temp6067; _LL6057: if( _temp6055 == Cyc_Absyn_Loc_n){
goto _LL6058;} else{ goto _LL6059;} _LL6059: if(( unsigned int) _temp6055 > 1u?((
struct _enum_struct*) _temp6055)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL6066:
_temp6065=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp6055)->f1;
if( _temp6065 == 0){ goto _LL6060;} else{ goto _LL6061;}} else{ goto _LL6061;}
_LL6061: if(( unsigned int) _temp6055 > 1u?(( struct _enum_struct*) _temp6055)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL6068: _temp6067=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp6055)->f1; if( _temp6067 == 0){ goto _LL6062;}
else{ goto _LL6063;}} else{ goto _LL6063;} _LL6063: goto _LL6064; _LL6058: goto
_LL6056; _LL6060: goto _LL6056; _LL6062: goto _LL6056; _LL6064: Cyc_Parse_err((
struct _tagged_string)({ char* _temp6069=( char*)"struct field is qualified with module name";
struct _tagged_string _temp6070; _temp6070.curr= _temp6069; _temp6070.base=
_temp6069; _temp6070.last_plus_one= _temp6069 + 43; _temp6070;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6071= yyls; struct Cyc_Yyltype* _temp6073= _temp6071.curr
+ yylsp_offset; if( _temp6073 < _temp6071.base? 1: _temp6073 >= _temp6071.last_plus_one){
_throw( Null_Exception);}* _temp6073;}).first_line,({ struct _tagged_ptr3
_temp6074= yyls; struct Cyc_Yyltype* _temp6076= _temp6074.curr + yylsp_offset;
if( _temp6076 < _temp6074.base? 1: _temp6076 >= _temp6074.last_plus_one){ _throw(
Null_Exception);}* _temp6076;}).last_line)); goto _LL6056; _LL6056:;} yyval=({
struct Cyc_Exp_tok_struct* _temp6077=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct));* _temp6077=( struct Cyc_Exp_tok_struct){.tag=
Cyc_Exp_tok_tag,.f1= Cyc_Absyn_structarrow_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6078= yyvs; struct _xenum_struct** _temp6080= _temp6078.curr +(
yyvsp_offset - 2); if( _temp6080 < _temp6078.base? 1: _temp6080 >= _temp6078.last_plus_one){
_throw( Null_Exception);}* _temp6080;})),(* q).f2, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6081= yyls; struct Cyc_Yyltype* _temp6083= _temp6081.curr
+( yylsp_offset - 2); if( _temp6083 < _temp6081.base? 1: _temp6083 >= _temp6081.last_plus_one){
_throw( Null_Exception);}* _temp6083;}).first_line,({ struct _tagged_ptr3
_temp6084= yyls; struct Cyc_Yyltype* _temp6086= _temp6084.curr + yylsp_offset;
if( _temp6086 < _temp6084.base? 1: _temp6086 >= _temp6084.last_plus_one){ _throw(
Null_Exception);}* _temp6086;}).last_line))};( struct _xenum_struct*) _temp6077;});
break;} case 352: _LL6051: yyval=({ struct Cyc_Exp_tok_struct* _temp6088=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp6088=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_post_inc_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6089= yyvs; struct _xenum_struct**
_temp6091= _temp6089.curr +( yyvsp_offset - 1); if( _temp6091 < _temp6089.base?
1: _temp6091 >= _temp6089.last_plus_one){ _throw( Null_Exception);}* _temp6091;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6092= yyls; struct Cyc_Yyltype*
_temp6094= _temp6092.curr +( yylsp_offset - 1); if( _temp6094 < _temp6092.base?
1: _temp6094 >= _temp6092.last_plus_one){ _throw( Null_Exception);}* _temp6094;}).first_line,({
struct _tagged_ptr3 _temp6095= yyls; struct Cyc_Yyltype* _temp6097= _temp6095.curr
+ yylsp_offset; if( _temp6097 < _temp6095.base? 1: _temp6097 >= _temp6095.last_plus_one){
_throw( Null_Exception);}* _temp6097;}).last_line))};( struct _xenum_struct*)
_temp6088;}); break; case 353: _LL6087: yyval=({ struct Cyc_Exp_tok_struct*
_temp6099=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp6099=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_post_dec_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6100= yyvs; struct _xenum_struct**
_temp6102= _temp6100.curr +( yyvsp_offset - 1); if( _temp6102 < _temp6100.base?
1: _temp6102 >= _temp6100.last_plus_one){ _throw( Null_Exception);}* _temp6102;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6103= yyls; struct Cyc_Yyltype*
_temp6105= _temp6103.curr +( yylsp_offset - 1); if( _temp6105 < _temp6103.base?
1: _temp6105 >= _temp6103.last_plus_one){ _throw( Null_Exception);}* _temp6105;}).first_line,({
struct _tagged_ptr3 _temp6106= yyls; struct Cyc_Yyltype* _temp6108= _temp6106.curr
+ yylsp_offset; if( _temp6108 < _temp6106.base? 1: _temp6108 >= _temp6106.last_plus_one){
_throw( Null_Exception);}* _temp6108;}).last_line))};( struct _xenum_struct*)
_temp6099;}); break; case 354: _LL6098: yyval=({ struct Cyc_Exp_tok_struct*
_temp6110=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp6110=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_CompoundLit_e_struct* _temp6111=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp6111->tag= Cyc_Absyn_CompoundLit_e_tag;
_temp6111->f1= Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp6112= yyvs;
struct _xenum_struct** _temp6114= _temp6112.curr +( yyvsp_offset - 4); if(
_temp6114 < _temp6112.base? 1: _temp6114 >= _temp6112.last_plus_one){ _throw(
Null_Exception);}* _temp6114;})); _temp6111->f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp6115= yyvs; struct _xenum_struct** _temp6117= _temp6115.curr
+( yyvsp_offset - 1); if( _temp6117 < _temp6115.base? 1: _temp6117 >= _temp6115.last_plus_one){
_throw( Null_Exception);}* _temp6117;})));( void*) _temp6111;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6118= yyls; struct Cyc_Yyltype* _temp6120= _temp6118.curr
+( yylsp_offset - 5); if( _temp6120 < _temp6118.base? 1: _temp6120 >= _temp6118.last_plus_one){
_throw( Null_Exception);}* _temp6120;}).first_line,({ struct _tagged_ptr3
_temp6121= yyls; struct Cyc_Yyltype* _temp6123= _temp6121.curr + yylsp_offset;
if( _temp6123 < _temp6121.base? 1: _temp6123 >= _temp6121.last_plus_one){ _throw(
Null_Exception);}* _temp6123;}).last_line))};( struct _xenum_struct*) _temp6110;});
break; case 355: _LL6109: yyval=({ struct Cyc_Exp_tok_struct* _temp6125=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp6125=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_CompoundLit_e_struct* _temp6126=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp6126->tag= Cyc_Absyn_CompoundLit_e_tag;
_temp6126->f1= Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp6127= yyvs;
struct _xenum_struct** _temp6129= _temp6127.curr +( yyvsp_offset - 5); if(
_temp6129 < _temp6127.base? 1: _temp6129 >= _temp6127.last_plus_one){ _throw(
Null_Exception);}* _temp6129;})); _temp6126->f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp6130= yyvs; struct _xenum_struct** _temp6132= _temp6130.curr
+( yyvsp_offset - 2); if( _temp6132 < _temp6130.base? 1: _temp6132 >= _temp6130.last_plus_one){
_throw( Null_Exception);}* _temp6132;})));( void*) _temp6126;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6133= yyls; struct Cyc_Yyltype* _temp6135= _temp6133.curr
+( yylsp_offset - 6); if( _temp6135 < _temp6133.base? 1: _temp6135 >= _temp6133.last_plus_one){
_throw( Null_Exception);}* _temp6135;}).first_line,({ struct _tagged_ptr3
_temp6136= yyls; struct Cyc_Yyltype* _temp6138= _temp6136.curr + yylsp_offset;
if( _temp6138 < _temp6136.base? 1: _temp6138 >= _temp6136.last_plus_one){ _throw(
Null_Exception);}* _temp6138;}).last_line))};( struct _xenum_struct*) _temp6125;});
break; case 356: _LL6124: yyval=({ struct Cyc_Exp_tok_struct* _temp6140=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp6140=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_Array_e_struct* _temp6141=( struct Cyc_Absyn_Array_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp6141->tag= Cyc_Absyn_Array_e_tag;
_temp6141->f1= 1; _temp6141->f2= 0;( void*) _temp6141;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6142= yyls; struct Cyc_Yyltype* _temp6144= _temp6142.curr
+( yylsp_offset - 2); if( _temp6144 < _temp6142.base? 1: _temp6144 >= _temp6142.last_plus_one){
_throw( Null_Exception);}* _temp6144;}).first_line,({ struct _tagged_ptr3
_temp6145= yyls; struct Cyc_Yyltype* _temp6147= _temp6145.curr + yylsp_offset;
if( _temp6147 < _temp6145.base? 1: _temp6147 >= _temp6145.last_plus_one){ _throw(
Null_Exception);}* _temp6147;}).last_line))};( struct _xenum_struct*) _temp6140;});
break; case 357: _LL6139: yyval=({ struct Cyc_Exp_tok_struct* _temp6149=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp6149=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_Array_e_struct* _temp6150=( struct Cyc_Absyn_Array_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp6150->tag= Cyc_Absyn_Array_e_tag;
_temp6150->f1= 1; _temp6150->f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({ struct _tagged_ptr2
_temp6151= yyvs; struct _xenum_struct** _temp6153= _temp6151.curr +(
yyvsp_offset - 1); if( _temp6153 < _temp6151.base? 1: _temp6153 >= _temp6151.last_plus_one){
_throw( Null_Exception);}* _temp6153;})));( void*) _temp6150;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6154= yyls; struct Cyc_Yyltype* _temp6156= _temp6154.curr
+( yylsp_offset - 3); if( _temp6156 < _temp6154.base? 1: _temp6156 >= _temp6154.last_plus_one){
_throw( Null_Exception);}* _temp6156;}).first_line,({ struct _tagged_ptr3
_temp6157= yyls; struct Cyc_Yyltype* _temp6159= _temp6157.curr +( yylsp_offset -
1); if( _temp6159 < _temp6157.base? 1: _temp6159 >= _temp6157.last_plus_one){
_throw( Null_Exception);}* _temp6159;}).last_line))};( struct _xenum_struct*)
_temp6149;}); break; case 358: _LL6148: yyval=({ struct Cyc_Exp_tok_struct*
_temp6161=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp6161=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_Array_e_struct* _temp6162=( struct Cyc_Absyn_Array_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp6162->tag= Cyc_Absyn_Array_e_tag;
_temp6162->f1= 1; _temp6162->f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({ struct _tagged_ptr2
_temp6163= yyvs; struct _xenum_struct** _temp6165= _temp6163.curr +(
yyvsp_offset - 2); if( _temp6165 < _temp6163.base? 1: _temp6165 >= _temp6163.last_plus_one){
_throw( Null_Exception);}* _temp6165;})));( void*) _temp6162;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6166= yyls; struct Cyc_Yyltype* _temp6168= _temp6166.curr
+( yylsp_offset - 4); if( _temp6168 < _temp6166.base? 1: _temp6168 >= _temp6166.last_plus_one){
_throw( Null_Exception);}* _temp6168;}).first_line,({ struct _tagged_ptr3
_temp6169= yyls; struct Cyc_Yyltype* _temp6171= _temp6169.curr +( yylsp_offset -
1); if( _temp6171 < _temp6169.base? 1: _temp6171 >= _temp6169.last_plus_one){
_throw( Null_Exception);}* _temp6171;}).last_line))};( struct _xenum_struct*)
_temp6161;}); break; case 359: _LL6160: yyval=({ struct Cyc_Exp_tok_struct*
_temp6173=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp6173=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_Comprehension_e_struct* _temp6174=( struct Cyc_Absyn_Comprehension_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Comprehension_e_struct)); _temp6174->tag=
Cyc_Absyn_Comprehension_e_tag; _temp6174->f1= Cyc_Absyn_new_vardecl(({ struct
_tuple1* _temp6175=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp6175->f1= Cyc_Absyn_Loc_n; _temp6175->f2=({ struct _tagged_string*
_temp6176=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp6176[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp6177= yyvs;
struct _xenum_struct** _temp6179= _temp6177.curr +( yyvsp_offset - 5); if(
_temp6179 < _temp6177.base? 1: _temp6179 >= _temp6177.last_plus_one){ _throw(
Null_Exception);}* _temp6179;})); _temp6176;}); _temp6175;}), Cyc_Absyn_uint_t,(
struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp( 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6180= yyls; struct Cyc_Yyltype* _temp6182= _temp6180.curr
+( yylsp_offset - 5); if( _temp6182 < _temp6180.base? 1: _temp6182 >= _temp6180.last_plus_one){
_throw( Null_Exception);}* _temp6182;}).first_line,({ struct _tagged_ptr3
_temp6183= yyls; struct Cyc_Yyltype* _temp6185= _temp6183.curr +( yylsp_offset -
5); if( _temp6185 < _temp6183.base? 1: _temp6185 >= _temp6183.last_plus_one){
_throw( Null_Exception);}* _temp6185;}).last_line))); _temp6174->f2= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6186= yyvs; struct _xenum_struct** _temp6188= _temp6186.curr
+( yyvsp_offset - 3); if( _temp6188 < _temp6186.base? 1: _temp6188 >= _temp6186.last_plus_one){
_throw( Null_Exception);}* _temp6188;})); _temp6174->f3= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6189= yyvs; struct _xenum_struct** _temp6191= _temp6189.curr
+( yyvsp_offset - 1); if( _temp6191 < _temp6189.base? 1: _temp6191 >= _temp6189.last_plus_one){
_throw( Null_Exception);}* _temp6191;}));( void*) _temp6174;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6192= yyls; struct Cyc_Yyltype* _temp6194= _temp6192.curr
+( yylsp_offset - 8); if( _temp6194 < _temp6192.base? 1: _temp6194 >= _temp6192.last_plus_one){
_throw( Null_Exception);}* _temp6194;}).first_line,({ struct _tagged_ptr3
_temp6195= yyls; struct Cyc_Yyltype* _temp6197= _temp6195.curr + yylsp_offset;
if( _temp6197 < _temp6195.base? 1: _temp6197 >= _temp6195.last_plus_one){ _throw(
Null_Exception);}* _temp6197;}).last_line))};( struct _xenum_struct*) _temp6173;});
break; case 360: _LL6172: yyval=({ struct Cyc_Exp_tok_struct* _temp6199=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp6199=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_string_exp( 1,
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp6200= yyvs; struct
_xenum_struct** _temp6202= _temp6200.curr + yyvsp_offset; if( _temp6202 <
_temp6200.base? 1: _temp6202 >= _temp6200.last_plus_one){ _throw( Null_Exception);}*
_temp6202;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6203=
yyls; struct Cyc_Yyltype* _temp6205= _temp6203.curr +( yylsp_offset - 1); if(
_temp6205 < _temp6203.base? 1: _temp6205 >= _temp6203.last_plus_one){ _throw(
Null_Exception);}* _temp6205;}).first_line,({ struct _tagged_ptr3 _temp6206=
yyls; struct Cyc_Yyltype* _temp6208= _temp6206.curr + yylsp_offset; if(
_temp6208 < _temp6206.base? 1: _temp6208 >= _temp6206.last_plus_one){ _throw(
Null_Exception);}* _temp6208;}).last_line))};( struct _xenum_struct*) _temp6199;});
break; case 361: _LL6198: yyval=({ struct Cyc_Exp_tok_struct* _temp6210=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp6210=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_Fill_e_struct* _temp6211=( struct Cyc_Absyn_Fill_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fill_e_struct)); _temp6211->tag= Cyc_Absyn_Fill_e_tag;
_temp6211->f1= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6212= yyvs; struct
_xenum_struct** _temp6214= _temp6212.curr +( yyvsp_offset - 1); if( _temp6214 <
_temp6212.base? 1: _temp6214 >= _temp6212.last_plus_one){ _throw( Null_Exception);}*
_temp6214;}));( void*) _temp6211;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6215= yyls; struct Cyc_Yyltype* _temp6217= _temp6215.curr +(
yylsp_offset - 3); if( _temp6217 < _temp6215.base? 1: _temp6217 >= _temp6215.last_plus_one){
_throw( Null_Exception);}* _temp6217;}).first_line,({ struct _tagged_ptr3
_temp6218= yyls; struct Cyc_Yyltype* _temp6220= _temp6218.curr + yylsp_offset;
if( _temp6220 < _temp6218.base? 1: _temp6220 >= _temp6218.last_plus_one){ _throw(
Null_Exception);}* _temp6220;}).last_line))};( struct _xenum_struct*) _temp6210;});
break; case 362: _LL6209: yyval=({ struct Cyc_Exp_tok_struct* _temp6222=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp6222=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_Codegen_e_struct* _temp6223=( struct Cyc_Absyn_Codegen_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Codegen_e_struct)); _temp6223->tag= Cyc_Absyn_Codegen_e_tag;
_temp6223->f1= Cyc_yyget_FnDecl_tok(({ struct _tagged_ptr2 _temp6224= yyvs;
struct _xenum_struct** _temp6226= _temp6224.curr +( yyvsp_offset - 1); if(
_temp6226 < _temp6224.base? 1: _temp6226 >= _temp6224.last_plus_one){ _throw(
Null_Exception);}* _temp6226;}));( void*) _temp6223;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6227= yyls; struct Cyc_Yyltype* _temp6229= _temp6227.curr
+( yylsp_offset - 3); if( _temp6229 < _temp6227.base? 1: _temp6229 >= _temp6227.last_plus_one){
_throw( Null_Exception);}* _temp6229;}).first_line,({ struct _tagged_ptr3
_temp6230= yyls; struct Cyc_Yyltype* _temp6232= _temp6230.curr + yylsp_offset;
if( _temp6232 < _temp6230.base? 1: _temp6232 >= _temp6230.last_plus_one){ _throw(
Null_Exception);}* _temp6232;}).last_line))};( struct _xenum_struct*) _temp6222;});
break; case 363: _LL6221: yyval=({ struct Cyc_Exp_tok_struct* _temp6234=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp6234=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_UnknownId_e_struct* _temp6235=( struct Cyc_Absyn_UnknownId_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp6235->tag= Cyc_Absyn_UnknownId_e_tag;
_temp6235->f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp6236= yyvs;
struct _xenum_struct** _temp6238= _temp6236.curr + yyvsp_offset; if( _temp6238 <
_temp6236.base? 1: _temp6238 >= _temp6236.last_plus_one){ _throw( Null_Exception);}*
_temp6238;}));( void*) _temp6235;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6239= yyls; struct Cyc_Yyltype* _temp6241= _temp6239.curr +
yylsp_offset; if( _temp6241 < _temp6239.base? 1: _temp6241 >= _temp6239.last_plus_one){
_throw( Null_Exception);}* _temp6241;}).first_line,({ struct _tagged_ptr3
_temp6242= yyls; struct Cyc_Yyltype* _temp6244= _temp6242.curr + yylsp_offset;
if( _temp6244 < _temp6242.base? 1: _temp6244 >= _temp6242.last_plus_one){ _throw(
Null_Exception);}* _temp6244;}).last_line))};( struct _xenum_struct*) _temp6234;});
break; case 364: _LL6233: yyval=({ struct _tagged_ptr2 _temp6246= yyvs; struct
_xenum_struct** _temp6248= _temp6246.curr + yyvsp_offset; if( _temp6248 <
_temp6246.base? 1: _temp6248 >= _temp6246.last_plus_one){ _throw( Null_Exception);}*
_temp6248;}); break; case 365: _LL6245: yyval=({ struct Cyc_Exp_tok_struct*
_temp6250=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp6250=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_string_exp(
0, Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp6251= yyvs; struct
_xenum_struct** _temp6253= _temp6251.curr + yyvsp_offset; if( _temp6253 <
_temp6251.base? 1: _temp6253 >= _temp6251.last_plus_one){ _throw( Null_Exception);}*
_temp6253;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6254=
yyls; struct Cyc_Yyltype* _temp6256= _temp6254.curr + yylsp_offset; if(
_temp6256 < _temp6254.base? 1: _temp6256 >= _temp6254.last_plus_one){ _throw(
Null_Exception);}* _temp6256;}).first_line,({ struct _tagged_ptr3 _temp6257=
yyls; struct Cyc_Yyltype* _temp6259= _temp6257.curr + yylsp_offset; if(
_temp6259 < _temp6257.base? 1: _temp6259 >= _temp6257.last_plus_one){ _throw(
Null_Exception);}* _temp6259;}).last_line))};( struct _xenum_struct*) _temp6250;});
break; case 366: _LL6249: yyval=({ struct _tagged_ptr2 _temp6261= yyvs; struct
_xenum_struct** _temp6263= _temp6261.curr +( yyvsp_offset - 1); if( _temp6263 <
_temp6261.base? 1: _temp6263 >= _temp6261.last_plus_one){ _throw( Null_Exception);}*
_temp6263;}); break; case 367: _LL6260: yyval=({ struct Cyc_Exp_tok_struct*
_temp6265=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp6265=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_noinstantiate_exp(
Cyc_Absyn_new_exp(({ struct Cyc_Absyn_UnknownId_e_struct* _temp6266=( struct Cyc_Absyn_UnknownId_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp6266->tag= Cyc_Absyn_UnknownId_e_tag;
_temp6266->f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp6267= yyvs;
struct _xenum_struct** _temp6269= _temp6267.curr +( yyvsp_offset - 1); if(
_temp6269 < _temp6267.base? 1: _temp6269 >= _temp6267.last_plus_one){ _throw(
Null_Exception);}* _temp6269;}));( void*) _temp6266;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6270= yyls; struct Cyc_Yyltype* _temp6272= _temp6270.curr
+( yylsp_offset - 1); if( _temp6272 < _temp6270.base? 1: _temp6272 >= _temp6270.last_plus_one){
_throw( Null_Exception);}* _temp6272;}).first_line,({ struct _tagged_ptr3
_temp6273= yyls; struct Cyc_Yyltype* _temp6275= _temp6273.curr +( yylsp_offset -
1); if( _temp6275 < _temp6273.base? 1: _temp6275 >= _temp6273.last_plus_one){
_throw( Null_Exception);}* _temp6275;}).last_line)), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6276= yyls; struct Cyc_Yyltype* _temp6278= _temp6276.curr
+( yylsp_offset - 1); if( _temp6278 < _temp6276.base? 1: _temp6278 >= _temp6276.last_plus_one){
_throw( Null_Exception);}* _temp6278;}).first_line,({ struct _tagged_ptr3
_temp6279= yyls; struct Cyc_Yyltype* _temp6281= _temp6279.curr + yylsp_offset;
if( _temp6281 < _temp6279.base? 1: _temp6281 >= _temp6279.last_plus_one){ _throw(
Null_Exception);}* _temp6281;}).last_line))};( struct _xenum_struct*) _temp6265;});
break; case 368: _LL6264: yyval=({ struct Cyc_Exp_tok_struct* _temp6283=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp6283=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_instantiate_exp(
Cyc_Absyn_new_exp(({ struct Cyc_Absyn_UnknownId_e_struct* _temp6284=( struct Cyc_Absyn_UnknownId_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp6284->tag= Cyc_Absyn_UnknownId_e_tag;
_temp6284->f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp6285= yyvs;
struct _xenum_struct** _temp6287= _temp6285.curr +( yyvsp_offset - 4); if(
_temp6287 < _temp6285.base? 1: _temp6287 >= _temp6285.last_plus_one){ _throw(
Null_Exception);}* _temp6287;}));( void*) _temp6284;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6288= yyls; struct Cyc_Yyltype* _temp6290= _temp6288.curr
+( yylsp_offset - 4); if( _temp6290 < _temp6288.base? 1: _temp6290 >= _temp6288.last_plus_one){
_throw( Null_Exception);}* _temp6290;}).first_line,({ struct _tagged_ptr3
_temp6291= yyls; struct Cyc_Yyltype* _temp6293= _temp6291.curr +( yylsp_offset -
4); if( _temp6293 < _temp6291.base? 1: _temp6293 >= _temp6291.last_plus_one){
_throw( Null_Exception);}* _temp6293;}).last_line)),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp6294= yyvs; struct _xenum_struct** _temp6296= _temp6294.curr +(
yyvsp_offset - 1); if( _temp6296 < _temp6294.base? 1: _temp6296 >= _temp6294.last_plus_one){
_throw( Null_Exception);}* _temp6296;}))), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6297= yyls; struct Cyc_Yyltype* _temp6299= _temp6297.curr +(
yylsp_offset - 4); if( _temp6299 < _temp6297.base? 1: _temp6299 >= _temp6297.last_plus_one){
_throw( Null_Exception);}* _temp6299;}).first_line,({ struct _tagged_ptr3
_temp6300= yyls; struct Cyc_Yyltype* _temp6302= _temp6300.curr + yylsp_offset;
if( _temp6302 < _temp6300.base? 1: _temp6302 >= _temp6300.last_plus_one){ _throw(
Null_Exception);}* _temp6302;}).last_line))};( struct _xenum_struct*) _temp6283;});
break; case 369: _LL6282: yyval=({ struct Cyc_Exp_tok_struct* _temp6304=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp6304=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_tuple_exp( Cyc_yyget_ExpList_tok(({
struct _tagged_ptr2 _temp6305= yyvs; struct _xenum_struct** _temp6307= _temp6305.curr
+( yyvsp_offset - 1); if( _temp6307 < _temp6305.base? 1: _temp6307 >= _temp6305.last_plus_one){
_throw( Null_Exception);}* _temp6307;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6308= yyls; struct Cyc_Yyltype* _temp6310= _temp6308.curr +(
yylsp_offset - 3); if( _temp6310 < _temp6308.base? 1: _temp6310 >= _temp6308.last_plus_one){
_throw( Null_Exception);}* _temp6310;}).first_line,({ struct _tagged_ptr3
_temp6311= yyls; struct Cyc_Yyltype* _temp6313= _temp6311.curr + yylsp_offset;
if( _temp6313 < _temp6311.base? 1: _temp6313 >= _temp6311.last_plus_one){ _throw(
Null_Exception);}* _temp6313;}).last_line))};( struct _xenum_struct*) _temp6304;});
break; case 370: _LL6303: yyval=({ struct Cyc_Exp_tok_struct* _temp6315=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp6315=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_Struct_e_struct* _temp6316=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp6316->tag= Cyc_Absyn_Struct_e_tag;
_temp6316->f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp6317= yyvs;
struct _xenum_struct** _temp6319= _temp6317.curr +( yyvsp_offset - 3); if(
_temp6319 < _temp6317.base? 1: _temp6319 >= _temp6317.last_plus_one){ _throw(
Null_Exception);}* _temp6319;})); _temp6316->f2= 0; _temp6316->f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp6320= yyvs; struct _xenum_struct** _temp6322= _temp6320.curr
+( yyvsp_offset - 1); if( _temp6322 < _temp6320.base? 1: _temp6322 >= _temp6320.last_plus_one){
_throw( Null_Exception);}* _temp6322;}))); _temp6316->f4= 0;( void*) _temp6316;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6323= yyls; struct Cyc_Yyltype*
_temp6325= _temp6323.curr +( yylsp_offset - 3); if( _temp6325 < _temp6323.base?
1: _temp6325 >= _temp6323.last_plus_one){ _throw( Null_Exception);}* _temp6325;}).first_line,({
struct _tagged_ptr3 _temp6326= yyls; struct Cyc_Yyltype* _temp6328= _temp6326.curr
+ yylsp_offset; if( _temp6328 < _temp6326.base? 1: _temp6328 >= _temp6326.last_plus_one){
_throw( Null_Exception);}* _temp6328;}).last_line))};( struct _xenum_struct*)
_temp6315;}); break; case 371: _LL6314: yyval=({ struct Cyc_Exp_tok_struct*
_temp6330=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp6330=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_stmt_exp(
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp6331= yyvs; struct _xenum_struct**
_temp6333= _temp6331.curr +( yyvsp_offset - 2); if( _temp6333 < _temp6331.base?
1: _temp6333 >= _temp6331.last_plus_one){ _throw( Null_Exception);}* _temp6333;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6334= yyls; struct Cyc_Yyltype*
_temp6336= _temp6334.curr +( yylsp_offset - 4); if( _temp6336 < _temp6334.base?
1: _temp6336 >= _temp6334.last_plus_one){ _throw( Null_Exception);}* _temp6336;}).first_line,({
struct _tagged_ptr3 _temp6337= yyls; struct Cyc_Yyltype* _temp6339= _temp6337.curr
+ yylsp_offset; if( _temp6339 < _temp6337.base? 1: _temp6339 >= _temp6337.last_plus_one){
_throw( Null_Exception);}* _temp6339;}).last_line))};( struct _xenum_struct*)
_temp6330;}); break; case 372: _LL6329: yyval=({ struct Cyc_ExpList_tok_struct*
_temp6341=( struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));*
_temp6341=( struct Cyc_ExpList_tok_struct){.tag= Cyc_ExpList_tok_tag,.f1=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ExpList_tok(({
struct _tagged_ptr2 _temp6342= yyvs; struct _xenum_struct** _temp6344= _temp6342.curr
+ yyvsp_offset; if( _temp6344 < _temp6342.base? 1: _temp6344 >= _temp6342.last_plus_one){
_throw( Null_Exception);}* _temp6344;})))};( struct _xenum_struct*) _temp6341;});
break; case 373: _LL6340: yyval=({ struct Cyc_ExpList_tok_struct* _temp6346=(
struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));*
_temp6346=( struct Cyc_ExpList_tok_struct){.tag= Cyc_ExpList_tok_tag,.f1=({
struct Cyc_List_List* _temp6347=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp6347->hd=( void*) Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6348= yyvs; struct _xenum_struct** _temp6350= _temp6348.curr +
yyvsp_offset; if( _temp6350 < _temp6348.base? 1: _temp6350 >= _temp6348.last_plus_one){
_throw( Null_Exception);}* _temp6350;})); _temp6347->tl= 0; _temp6347;})};(
struct _xenum_struct*) _temp6346;}); break; case 374: _LL6345: yyval=({ struct
Cyc_ExpList_tok_struct* _temp6352=( struct Cyc_ExpList_tok_struct*) GC_malloc(
sizeof( struct Cyc_ExpList_tok_struct));* _temp6352=( struct Cyc_ExpList_tok_struct){.tag=
Cyc_ExpList_tok_tag,.f1=({ struct Cyc_List_List* _temp6353=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp6353->hd=( void*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6357= yyvs; struct _xenum_struct** _temp6359= _temp6357.curr
+ yyvsp_offset; if( _temp6359 < _temp6357.base? 1: _temp6359 >= _temp6357.last_plus_one){
_throw( Null_Exception);}* _temp6359;})); _temp6353->tl= Cyc_yyget_ExpList_tok(({
struct _tagged_ptr2 _temp6354= yyvs; struct _xenum_struct** _temp6356= _temp6354.curr
+( yyvsp_offset - 2); if( _temp6356 < _temp6354.base? 1: _temp6356 >= _temp6354.last_plus_one){
_throw( Null_Exception);}* _temp6356;})); _temp6353;})};( struct _xenum_struct*)
_temp6352;}); break; case 375: _LL6351: yyval=({ struct Cyc_Exp_tok_struct*
_temp6361=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp6361=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_int_exp((*
Cyc_yyget_Int_tok(({ struct _tagged_ptr2 _temp6362= yyvs; struct _xenum_struct**
_temp6364= _temp6362.curr + yyvsp_offset; if( _temp6364 < _temp6362.base? 1:
_temp6364 >= _temp6362.last_plus_one){ _throw( Null_Exception);}* _temp6364;}))).f1,(*
Cyc_yyget_Int_tok(({ struct _tagged_ptr2 _temp6365= yyvs; struct _xenum_struct**
_temp6367= _temp6365.curr + yyvsp_offset; if( _temp6367 < _temp6365.base? 1:
_temp6367 >= _temp6365.last_plus_one){ _throw( Null_Exception);}* _temp6367;}))).f2,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6368= yyls; struct Cyc_Yyltype*
_temp6370= _temp6368.curr + yylsp_offset; if( _temp6370 < _temp6368.base? 1:
_temp6370 >= _temp6368.last_plus_one){ _throw( Null_Exception);}* _temp6370;}).first_line,({
struct _tagged_ptr3 _temp6371= yyls; struct Cyc_Yyltype* _temp6373= _temp6371.curr
+ yylsp_offset; if( _temp6373 < _temp6371.base? 1: _temp6373 >= _temp6371.last_plus_one){
_throw( Null_Exception);}* _temp6373;}).last_line))};( struct _xenum_struct*)
_temp6361;}); break; case 376: _LL6360: yyval=({ struct Cyc_Exp_tok_struct*
_temp6375=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp6375=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_char_exp(
Cyc_yyget_Char_tok(({ struct _tagged_ptr2 _temp6376= yyvs; struct _xenum_struct**
_temp6378= _temp6376.curr + yyvsp_offset; if( _temp6378 < _temp6376.base? 1:
_temp6378 >= _temp6376.last_plus_one){ _throw( Null_Exception);}* _temp6378;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6379= yyls; struct Cyc_Yyltype*
_temp6381= _temp6379.curr + yylsp_offset; if( _temp6381 < _temp6379.base? 1:
_temp6381 >= _temp6379.last_plus_one){ _throw( Null_Exception);}* _temp6381;}).first_line,({
struct _tagged_ptr3 _temp6382= yyls; struct Cyc_Yyltype* _temp6384= _temp6382.curr
+ yylsp_offset; if( _temp6384 < _temp6382.base? 1: _temp6384 >= _temp6382.last_plus_one){
_throw( Null_Exception);}* _temp6384;}).last_line))};( struct _xenum_struct*)
_temp6375;}); break; case 377: _LL6374: yyval=({ struct Cyc_Exp_tok_struct*
_temp6386=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp6386=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_float_exp(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp6387= yyvs; struct
_xenum_struct** _temp6389= _temp6387.curr + yyvsp_offset; if( _temp6389 <
_temp6387.base? 1: _temp6389 >= _temp6387.last_plus_one){ _throw( Null_Exception);}*
_temp6389;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6390=
yyls; struct Cyc_Yyltype* _temp6392= _temp6390.curr + yylsp_offset; if(
_temp6392 < _temp6390.base? 1: _temp6392 >= _temp6390.last_plus_one){ _throw(
Null_Exception);}* _temp6392;}).first_line,({ struct _tagged_ptr3 _temp6393=
yyls; struct Cyc_Yyltype* _temp6395= _temp6393.curr + yylsp_offset; if(
_temp6395 < _temp6393.base? 1: _temp6395 >= _temp6393.last_plus_one){ _throw(
Null_Exception);}* _temp6395;}).last_line))};( struct _xenum_struct*) _temp6386;});
break; case 378: _LL6385: yyval=({ struct Cyc_Exp_tok_struct* _temp6397=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp6397=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_null_exp( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6398= yyls; struct Cyc_Yyltype* _temp6400= _temp6398.curr
+ yylsp_offset; if( _temp6400 < _temp6398.base? 1: _temp6400 >= _temp6398.last_plus_one){
_throw( Null_Exception);}* _temp6400;}).first_line,({ struct _tagged_ptr3
_temp6401= yyls; struct Cyc_Yyltype* _temp6403= _temp6401.curr + yylsp_offset;
if( _temp6403 < _temp6401.base? 1: _temp6403 >= _temp6401.last_plus_one){ _throw(
Null_Exception);}* _temp6403;}).last_line))};( struct _xenum_struct*) _temp6397;});
break; case 379: _LL6396: yyval=({ struct Cyc_QualId_tok_struct* _temp6405=(
struct Cyc_QualId_tok_struct*) GC_malloc( sizeof( struct Cyc_QualId_tok_struct));*
_temp6405=( struct Cyc_QualId_tok_struct){.tag= Cyc_QualId_tok_tag,.f1=({ struct
_tuple1* _temp6406=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp6406->f1=({ struct Cyc_Absyn_Rel_n_struct* _temp6411=( struct Cyc_Absyn_Rel_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct)); _temp6411->tag= Cyc_Absyn_Rel_n_tag;
_temp6411->f1= 0;( void*) _temp6411;}); _temp6406->f2=({ struct _tagged_string*
_temp6407=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp6407[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp6408= yyvs;
struct _xenum_struct** _temp6410= _temp6408.curr + yyvsp_offset; if( _temp6410 <
_temp6408.base? 1: _temp6410 >= _temp6408.last_plus_one){ _throw( Null_Exception);}*
_temp6410;})); _temp6407;}); _temp6406;})};( struct _xenum_struct*) _temp6405;});
break; case 380: _LL6404: yyval=({ struct _tagged_ptr2 _temp6413= yyvs; struct
_xenum_struct** _temp6415= _temp6413.curr + yyvsp_offset; if( _temp6415 <
_temp6413.base? 1: _temp6415 >= _temp6413.last_plus_one){ _throw( Null_Exception);}*
_temp6415;}); break; default: _LL6412:( void) _throw(({ struct Cyc_Yyimpossible_struct*
_temp6417=( struct Cyc_Yyimpossible_struct*) GC_malloc_atomic( sizeof( struct
Cyc_Yyimpossible_struct));* _temp6417=( struct Cyc_Yyimpossible_struct){.tag=
Cyc_Yyimpossible_tag};( struct _xenum_struct*) _temp6417;}));} yyvsp_offset -=
yylen; yyssp_offset -= yylen; yylsp_offset -= yylen;({ struct _tagged_ptr2
_temp6418= yyvs; struct _xenum_struct** _temp6420= _temp6418.curr +( ++
yyvsp_offset); if( _temp6420 < _temp6418.base? 1: _temp6420 >= _temp6418.last_plus_one){
_throw( Null_Exception);}* _temp6420= yyval;}); yylsp_offset ++; if( yylen == 0){({
struct _tagged_ptr3 _temp6421= yyls; struct Cyc_Yyltype* _temp6423= _temp6421.curr
+ yylsp_offset; if( _temp6423 < _temp6421.base? 1: _temp6423 >= _temp6421.last_plus_one){
_throw( Null_Exception);}(* _temp6423).first_line= Cyc_yylloc.first_line;});({
struct _tagged_ptr3 _temp6424= yyls; struct Cyc_Yyltype* _temp6426= _temp6424.curr
+ yylsp_offset; if( _temp6426 < _temp6424.base? 1: _temp6426 >= _temp6424.last_plus_one){
_throw( Null_Exception);}(* _temp6426).first_column= Cyc_yylloc.first_column;});({
struct _tagged_ptr3 _temp6427= yyls; struct Cyc_Yyltype* _temp6429= _temp6427.curr
+ yylsp_offset; if( _temp6429 < _temp6427.base? 1: _temp6429 >= _temp6427.last_plus_one){
_throw( Null_Exception);}(* _temp6429).last_line=({ struct _tagged_ptr3
_temp6430= yyls; struct Cyc_Yyltype* _temp6432= _temp6430.curr +( yylsp_offset -
1); if( _temp6432 < _temp6430.base? 1: _temp6432 >= _temp6430.last_plus_one){
_throw( Null_Exception);}* _temp6432;}).last_line;});({ struct _tagged_ptr3
_temp6433= yyls; struct Cyc_Yyltype* _temp6435= _temp6433.curr + yylsp_offset;
if( _temp6435 < _temp6433.base? 1: _temp6435 >= _temp6433.last_plus_one){ _throw(
Null_Exception);}(* _temp6435).last_column=({ struct _tagged_ptr3 _temp6436=
yyls; struct Cyc_Yyltype* _temp6438= _temp6436.curr +( yylsp_offset - 1); if(
_temp6438 < _temp6436.base? 1: _temp6438 >= _temp6436.last_plus_one){ _throw(
Null_Exception);}* _temp6438;}).last_column;});({ struct _tagged_ptr3 _temp6439=
yyls; struct Cyc_Yyltype* _temp6441= _temp6439.curr + yylsp_offset; if(
_temp6441 < _temp6439.base? 1: _temp6441 >= _temp6439.last_plus_one){ _throw(
Null_Exception);}(* _temp6441).text=( struct _tagged_string)({ char* _temp6442=(
char*)""; struct _tagged_string _temp6443; _temp6443.curr= _temp6442; _temp6443.base=
_temp6442; _temp6443.last_plus_one= _temp6442 + 1; _temp6443;});});} else{({
struct _tagged_ptr3 _temp6444= yyls; struct Cyc_Yyltype* _temp6446= _temp6444.curr
+ yylsp_offset; if( _temp6446 < _temp6444.base? 1: _temp6446 >= _temp6444.last_plus_one){
_throw( Null_Exception);}(* _temp6446).last_line=({ struct _tagged_ptr3
_temp6447= yyls; struct Cyc_Yyltype* _temp6449= _temp6447.curr +(( yylsp_offset
+ yylen) - 1); if( _temp6449 < _temp6447.base? 1: _temp6449 >= _temp6447.last_plus_one){
_throw( Null_Exception);}* _temp6449;}).last_line;});({ struct _tagged_ptr3
_temp6450= yyls; struct Cyc_Yyltype* _temp6452= _temp6450.curr + yylsp_offset;
if( _temp6452 < _temp6450.base? 1: _temp6452 >= _temp6450.last_plus_one){ _throw(
Null_Exception);}(* _temp6452).last_column=({ struct _tagged_ptr3 _temp6453=
yyls; struct Cyc_Yyltype* _temp6455= _temp6453.curr +(( yylsp_offset + yylen) -
1); if( _temp6455 < _temp6453.base? 1: _temp6455 >= _temp6453.last_plus_one){
_throw( Null_Exception);}* _temp6455;}).last_column;});} yyn=( int)({ short*
_temp6456=( short*) Cyc_yyr1; unsigned int _temp6457= yyn; if( _temp6457 >= 381u){
_throw( Null_Exception);} _temp6456[ _temp6457];}); yystate=( int)({ short*
_temp6458=( short*) Cyc_yypgoto; unsigned int _temp6459= yyn - 119; if(
_temp6459 >= 102u){ _throw( Null_Exception);} _temp6458[ _temp6459];}) +( int)({
struct _tagged_ptr1 _temp6460= yyss; short* _temp6462= _temp6460.curr +
yyssp_offset; if( _temp6462 < _temp6460.base? 1: _temp6462 >= _temp6460.last_plus_one){
_throw( Null_Exception);}* _temp6462;}); if(( yystate >= 0? yystate <= 4314: 0)?(
int)({ short* _temp6463=( short*) Cyc_yycheck; unsigned int _temp6464= yystate;
if( _temp6464 >= 4315u){ _throw( Null_Exception);} _temp6463[ _temp6464];}) ==(
int)({ struct _tagged_ptr1 _temp6465= yyss; short* _temp6467= _temp6465.curr +
yyssp_offset; if( _temp6467 < _temp6465.base? 1: _temp6467 >= _temp6465.last_plus_one){
_throw( Null_Exception);}* _temp6467;}): 0){ yystate=( int)({ short* _temp6468=(
short*) Cyc_yytable; unsigned int _temp6469= yystate; if( _temp6469 >= 4315u){
_throw( Null_Exception);} _temp6468[ _temp6469];});} else{ yystate=( int)({
short* _temp6470=( short*) Cyc_yydefgoto; unsigned int _temp6471= yyn - 119; if(
_temp6471 >= 102u){ _throw( Null_Exception);} _temp6470[ _temp6471];});} goto
yynewstate; yyerrlab: if( yyerrstatus == 0){ ++ Cyc_yynerrs; yyn=( int)({ short*
_temp6472=( short*) Cyc_yypact; unsigned int _temp6473= yystate; if( _temp6473
>= 717u){ _throw( Null_Exception);} _temp6472[ _temp6473];}); if( yyn > - 32768?
yyn < 4314: 0){ int sze= 0; struct _tagged_string msg; int x; int count; count=
0; for( x= yyn < 0? - yyn: 0;( unsigned int) x < 221u / sizeof( char*); x ++){
if(( int)({ short* _temp6474=( short*) Cyc_yycheck; unsigned int _temp6475= x +
yyn; if( _temp6475 >= 4315u){ _throw( Null_Exception);} _temp6474[ _temp6475];})
== x){( unsigned int)( sze += Cyc_String_strlen(({ struct _tagged_string*
_temp6476=( struct _tagged_string*) Cyc_yytname; unsigned int _temp6477= x; if(
_temp6477 >= 221u){ _throw( Null_Exception);} _temp6476[ _temp6477];})) +(
unsigned int) 15), count ++;}} msg=({ unsigned int _temp6478=( unsigned int)(
sze + 15); char* _temp6479=( char*) GC_malloc_atomic( sizeof( char) * _temp6478);
unsigned int i; struct _tagged_string _temp6480={ _temp6479, _temp6479,
_temp6479 + _temp6478}; for( i= 0; i < _temp6478; i ++){ _temp6479[ i]='\000';}
_temp6480;}); Cyc_String_strcpy( msg,( struct _tagged_string)({ char* _temp6481=(
char*)"parse error"; struct _tagged_string _temp6482; _temp6482.curr= _temp6481;
_temp6482.base= _temp6481; _temp6482.last_plus_one= _temp6481 + 12; _temp6482;}));
if( count < 5){ count= 0; for( x= yyn < 0? - yyn: 0;( unsigned int) x < 221u /
sizeof( char*); x ++){ if(( int)({ short* _temp6483=( short*) Cyc_yycheck;
unsigned int _temp6484= x + yyn; if( _temp6484 >= 4315u){ _throw( Null_Exception);}
_temp6483[ _temp6484];}) == x){ Cyc_String_strcat( msg, count == 0?( struct
_tagged_string)({ char* _temp6485=( char*)", expecting `"; struct _tagged_string
_temp6486; _temp6486.curr= _temp6485; _temp6486.base= _temp6485; _temp6486.last_plus_one=
_temp6485 + 14; _temp6486;}):( struct _tagged_string)({ char* _temp6487=( char*)" or `";
struct _tagged_string _temp6488; _temp6488.curr= _temp6487; _temp6488.base=
_temp6487; _temp6488.last_plus_one= _temp6487 + 6; _temp6488;})); Cyc_String_strcat(
msg,({ struct _tagged_string* _temp6489=( struct _tagged_string*) Cyc_yytname;
unsigned int _temp6490= x; if( _temp6490 >= 221u){ _throw( Null_Exception);}
_temp6489[ _temp6490];})); Cyc_String_strcat( msg,( struct _tagged_string)({
char* _temp6491=( char*)"'"; struct _tagged_string _temp6492; _temp6492.curr=
_temp6491; _temp6492.base= _temp6491; _temp6492.last_plus_one= _temp6491 + 2;
_temp6492;})); count ++;}}} Cyc_yyerror( msg);} else{ Cyc_yyerror(( struct
_tagged_string)({ char* _temp6493=( char*)"parse error"; struct _tagged_string
_temp6494; _temp6494.curr= _temp6493; _temp6494.base= _temp6493; _temp6494.last_plus_one=
_temp6493 + 12; _temp6494;}));}} goto yyerrlab1; yyerrlab1: if( yyerrstatus == 3){
if( Cyc_yychar == 0){ return 1;} Cyc_yychar= - 2;} yyerrstatus= 3; goto
yyerrhandle; yyerrdefault: yyerrpop: if( yyssp_offset == 0){ return 1;}
yyvsp_offset --; yystate=( int)({ struct _tagged_ptr1 _temp6495= yyss; short*
_temp6497= _temp6495.curr +( -- yyssp_offset); if( _temp6497 < _temp6495.base? 1:
_temp6497 >= _temp6495.last_plus_one){ _throw( Null_Exception);}* _temp6497;});
yylsp_offset --; yyerrhandle: yyn=( int)({ short* _temp6498=( short*) Cyc_yypact;
unsigned int _temp6499= yystate; if( _temp6499 >= 717u){ _throw( Null_Exception);}
_temp6498[ _temp6499];}); if( yyn == - 32768){ goto yyerrdefault;} yyn += 1; if((
yyn < 0? 1: yyn > 4314)? 1:( int)({ short* _temp6500=( short*) Cyc_yycheck;
unsigned int _temp6501= yyn; if( _temp6501 >= 4315u){ _throw( Null_Exception);}
_temp6500[ _temp6501];}) != 1){ goto yyerrdefault;} yyn=( int)({ short*
_temp6502=( short*) Cyc_yytable; unsigned int _temp6503= yyn; if( _temp6503 >=
4315u){ _throw( Null_Exception);} _temp6502[ _temp6503];}); if( yyn < 0){ if(
yyn == - 32768){ goto yyerrpop;} yyn= - yyn; goto yyreduce;} else{ if( yyn == 0){
goto yyerrpop;}} if( yyn == 716){ return 0;}({ struct _tagged_ptr2 _temp6504=
yyvs; struct _xenum_struct** _temp6506= _temp6504.curr +( ++ yyvsp_offset); if(
_temp6506 < _temp6504.base? 1: _temp6506 >= _temp6504.last_plus_one){ _throw(
Null_Exception);}* _temp6506= Cyc_yylval;});({ struct _tagged_ptr3 _temp6507=
yyls; struct Cyc_Yyltype* _temp6509= _temp6507.curr +( ++ yylsp_offset); if(
_temp6509 < _temp6507.base? 1: _temp6509 >= _temp6507.last_plus_one){ _throw(
Null_Exception);}* _temp6509= Cyc_yylloc;}); goto yynewstate;} void Cyc_yyprint(
int i, struct _xenum_struct* v){ struct _xenum_struct* _temp6510= v; struct
_tuple11* _temp6530; struct _tuple11 _temp6532; int _temp6533; void* _temp6535;
char _temp6537; short _temp6539; struct _tagged_string _temp6541; struct Cyc_Core_Opt*
_temp6543; struct Cyc_Core_Opt* _temp6545; struct Cyc_Core_Opt _temp6547; struct
_tagged_string* _temp6548; struct _tuple1* _temp6550; struct _tuple1 _temp6552;
struct _tagged_string* _temp6553; void* _temp6555; _LL6512: if( _temp6510->tag
== Cyc_Okay_tok_tag){ goto _LL6513;} else{ goto _LL6514;} _LL6514: if((*((
struct _xenum_struct*) _temp6510)).tag == Cyc_Int_tok_tag){ _LL6531: _temp6530=((
struct Cyc_Int_tok_struct*) _temp6510)->f1; _temp6532=* _temp6530; _LL6536:
_temp6535= _temp6532.f1; goto _LL6534; _LL6534: _temp6533= _temp6532.f2; goto
_LL6515;} else{ goto _LL6516;} _LL6516: if((*(( struct _xenum_struct*) _temp6510)).tag
== Cyc_Char_tok_tag){ _LL6538: _temp6537=(( struct Cyc_Char_tok_struct*)
_temp6510)->f1; goto _LL6517;} else{ goto _LL6518;} _LL6518: if((*(( struct
_xenum_struct*) _temp6510)).tag == Cyc_Short_tok_tag){ _LL6540: _temp6539=((
struct Cyc_Short_tok_struct*) _temp6510)->f1; goto _LL6519;} else{ goto _LL6520;}
_LL6520: if((*(( struct _xenum_struct*) _temp6510)).tag == Cyc_String_tok_tag){
_LL6542: _temp6541=(( struct Cyc_String_tok_struct*) _temp6510)->f1; goto
_LL6521;} else{ goto _LL6522;} _LL6522: if((*(( struct _xenum_struct*) _temp6510)).tag
== Cyc_Stringopt_tok_tag){ _LL6544: _temp6543=(( struct Cyc_Stringopt_tok_struct*)
_temp6510)->f1; if( _temp6543 == 0){ goto _LL6523;} else{ goto _LL6524;}} else{
goto _LL6524;} _LL6524: if((*(( struct _xenum_struct*) _temp6510)).tag == Cyc_Stringopt_tok_tag){
_LL6546: _temp6545=(( struct Cyc_Stringopt_tok_struct*) _temp6510)->f1; if(
_temp6545 == 0){ goto _LL6526;} else{ _temp6547=* _temp6545; _LL6549: _temp6548=(
struct _tagged_string*) _temp6547.v; goto _LL6525;}} else{ goto _LL6526;}
_LL6526: if((*(( struct _xenum_struct*) _temp6510)).tag == Cyc_QualId_tok_tag){
_LL6551: _temp6550=(( struct Cyc_QualId_tok_struct*) _temp6510)->f1; _temp6552=*
_temp6550; _LL6556: _temp6555= _temp6552.f1; goto _LL6554; _LL6554: _temp6553=
_temp6552.f2; goto _LL6527;} else{ goto _LL6528;} _LL6528: goto _LL6529; _LL6513:
fprintf( Cyc_Stdio_stderr,"ok"); goto _LL6511; _LL6515: fprintf( Cyc_Stdio_stderr,"%d",
_temp6533); goto _LL6511; _LL6517: fprintf( Cyc_Stdio_stderr,"%c",( int)
_temp6537); goto _LL6511; _LL6519: fprintf( Cyc_Stdio_stderr,"%ds",( int)
_temp6539); goto _LL6511; _LL6521:({ struct _tagged_string _temp6557= _temp6541;
fprintf( Cyc_Stdio_stderr,"\"%.*s\"", _temp6557.last_plus_one - _temp6557.curr,
_temp6557.curr);}); goto _LL6511; _LL6523: fprintf( Cyc_Stdio_stderr,"null");
goto _LL6511; _LL6525:({ struct _tagged_string _temp6558=* _temp6548; fprintf(
Cyc_Stdio_stderr,"\"%.*s\"", _temp6558.last_plus_one - _temp6558.curr, _temp6558.curr);});
goto _LL6511; _LL6527: { struct Cyc_List_List* prefix= 0;{ void* _temp6559=
_temp6555; struct Cyc_List_List* _temp6567; struct Cyc_List_List* _temp6569;
_LL6561: if(( unsigned int) _temp6559 > 1u?(( struct _enum_struct*) _temp6559)->tag
== Cyc_Absyn_Rel_n_tag: 0){ _LL6568: _temp6567=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Rel_n_struct*) _temp6559)->f1; goto _LL6562;} else{ goto _LL6563;}
_LL6563: if(( unsigned int) _temp6559 > 1u?(( struct _enum_struct*) _temp6559)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL6570: _temp6569=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp6559)->f1; goto _LL6564;} else{ goto _LL6565;}
_LL6565: if( _temp6559 == Cyc_Absyn_Loc_n){ goto _LL6566;} else{ goto _LL6560;}
_LL6562: prefix= _temp6567; goto _LL6560; _LL6564: prefix= _temp6569; goto
_LL6560; _LL6566: goto _LL6560; _LL6560:;} for( 0; prefix != 0; prefix= prefix->tl){({
struct _tagged_string _temp6571=*(( struct _tagged_string*) prefix->hd); fprintf(
Cyc_Stdio_stderr,"%.*s::", _temp6571.last_plus_one - _temp6571.curr, _temp6571.curr);});}({
struct _tagged_string _temp6572=* _temp6553; fprintf( Cyc_Stdio_stderr,"%.*s::",
_temp6572.last_plus_one - _temp6572.curr, _temp6572.curr);}); goto _LL6511;}
_LL6529: fprintf( Cyc_Stdio_stderr,"?"); goto _LL6511; _LL6511:;} struct Cyc_List_List*
Cyc_Parse_parse_file( struct Cyc_Stdio___sFILE* f){ Cyc_Parse_parse_result= 0;
Cyc_Parse_lbuf=({ struct Cyc_Core_Opt* _temp6573=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp6573->v=( void*) Cyc_Lexing_from_file(
f); _temp6573;}); Cyc_yyparse(); return Cyc_Parse_parse_result;}