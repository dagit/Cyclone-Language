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
struct Cyc_List_List* f2; } ; struct _tuple14{ struct Cyc_List_List* f1; int f2;
struct Cyc_Core_Opt* f3; } ; struct _tuple15{ struct Cyc_List_List* f1; struct
Cyc_Absyn_Pat* f2; } ; struct _tagged_ptr1{ short* curr; short* base; short*
last_plus_one; } ; struct _tagged_ptr2{ struct _xenum_struct** curr; struct
_xenum_struct** base; struct _xenum_struct** last_plus_one; } ; struct
_tagged_ptr3{ struct Cyc_Yyltype* curr; struct Cyc_Yyltype* base; struct Cyc_Yyltype*
last_plus_one; } ; struct _tuple16{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp*
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
_temp451, t, ds->tl, shared_atts); _temp453;});}} static struct _tuple6 Cyc_Parse_apply_tms(
struct Cyc_Absyn_Tqual* tq, void* t, struct Cyc_List_List* atts, struct Cyc_List_List*
tms){ if( tms == 0){ return({ struct _tuple6 _temp455; _temp455.f1= tq; _temp455.f2=
t; _temp455.f3= 0; _temp455.f4= atts; _temp455;});}{ void* _temp456=( void*) tms->hd;
struct Cyc_Absyn_Exp* _temp470; void* _temp472; int _temp474; struct Cyc_Position_Segment*
_temp476; struct Cyc_List_List* _temp478; struct Cyc_Absyn_Tqual* _temp480; void*
_temp482; void* _temp484; struct Cyc_List_List* _temp486; struct Cyc_Position_Segment*
_temp488; _LL458: if( _temp456 == Cyc_Absyn_Carray_mod){ goto _LL459;} else{
goto _LL460;} _LL460: if(( unsigned int) _temp456 > 1u?(( struct _enum_struct*)
_temp456)->tag == Cyc_Absyn_ConstArray_mod_tag: 0){ _LL471: _temp470=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_ConstArray_mod_struct*) _temp456)->f1; goto
_LL461;} else{ goto _LL462;} _LL462: if(( unsigned int) _temp456 > 1u?(( struct
_enum_struct*) _temp456)->tag == Cyc_Absyn_Function_mod_tag: 0){ _LL473:
_temp472=( void*)(( struct Cyc_Absyn_Function_mod_struct*) _temp456)->f1; goto
_LL463;} else{ goto _LL464;} _LL464: if(( unsigned int) _temp456 > 1u?(( struct
_enum_struct*) _temp456)->tag == Cyc_Absyn_TypeParams_mod_tag: 0){ _LL479:
_temp478=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp456)->f1; goto _LL477; _LL477: _temp476=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp456)->f2; goto _LL475; _LL475:
_temp474=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp456)->f3; goto
_LL465;} else{ goto _LL466;} _LL466: if(( unsigned int) _temp456 > 1u?(( struct
_enum_struct*) _temp456)->tag == Cyc_Absyn_Pointer_mod_tag: 0){ _LL485: _temp484=(
void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp456)->f1; goto _LL483;
_LL483: _temp482=( void*)(( struct Cyc_Absyn_Pointer_mod_struct*) _temp456)->f2;
goto _LL481; _LL481: _temp480=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_Pointer_mod_struct*)
_temp456)->f3; goto _LL467;} else{ goto _LL468;} _LL468: if(( unsigned int)
_temp456 > 1u?(( struct _enum_struct*) _temp456)->tag == Cyc_Absyn_Attributes_mod_tag:
0){ _LL489: _temp488=( struct Cyc_Position_Segment*)(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp456)->f1; goto _LL487; _LL487: _temp486=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Attributes_mod_struct*) _temp456)->f2; goto _LL469;} else{ goto _LL457;}
_LL459: return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(),({ struct Cyc_Absyn_ArrayType_struct*
_temp490=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp490->tag= Cyc_Absyn_ArrayType_tag; _temp490->f1=( void*) t; _temp490->f2=
tq; _temp490->f3= Cyc_Absyn_uint_exp( 0, 0);( void*) _temp490;}), atts, tms->tl);
_LL461: return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(),({ struct Cyc_Absyn_ArrayType_struct*
_temp491=( struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp491->tag= Cyc_Absyn_ArrayType_tag; _temp491->f1=( void*) t; _temp491->f2=
tq; _temp491->f3= _temp470;( void*) _temp491;}), atts, tms->tl); _LL463: { void*
_temp492= _temp472; struct Cyc_Core_Opt* _temp498; int _temp500; struct Cyc_List_List*
_temp502; struct Cyc_Position_Segment* _temp504; struct Cyc_List_List* _temp506;
_LL494: if((( struct _enum_struct*) _temp492)->tag == Cyc_Absyn_WithTypes_tag){
_LL503: _temp502=( struct Cyc_List_List*)(( struct Cyc_Absyn_WithTypes_struct*)
_temp492)->f1; goto _LL501; _LL501: _temp500=( int)(( struct Cyc_Absyn_WithTypes_struct*)
_temp492)->f2; goto _LL499; _LL499: _temp498=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_WithTypes_struct*) _temp492)->f3; goto _LL495;} else{ goto _LL496;}
_LL496: if((( struct _enum_struct*) _temp492)->tag == Cyc_Absyn_NoTypes_tag){
_LL507: _temp506=( struct Cyc_List_List*)(( struct Cyc_Absyn_NoTypes_struct*)
_temp492)->f1; goto _LL505; _LL505: _temp504=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_NoTypes_struct*) _temp492)->f2; goto _LL497;} else{ goto _LL493;}
_LL495: { struct Cyc_List_List* typvars= 0; struct Cyc_List_List* fn_atts= 0;
if( tms->tl != 0){ void* _temp508=( void*)( tms->tl)->hd; int _temp514; struct
Cyc_Position_Segment* _temp516; struct Cyc_List_List* _temp518; _LL510: if((
unsigned int) _temp508 > 1u?(( struct _enum_struct*) _temp508)->tag == Cyc_Absyn_TypeParams_mod_tag:
0){ _LL519: _temp518=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypeParams_mod_struct*)
_temp508)->f1; goto _LL517; _LL517: _temp516=( struct Cyc_Position_Segment*)((
struct Cyc_Absyn_TypeParams_mod_struct*) _temp508)->f2; goto _LL515; _LL515:
_temp514=( int)(( struct Cyc_Absyn_TypeParams_mod_struct*) _temp508)->f3; goto
_LL511;} else{ goto _LL512;} _LL512: goto _LL513; _LL511: typvars= _temp518; tms=
tms->tl; goto _LL509; _LL513: goto _LL509; _LL509:;} if( tms->tl != 0){ void*
_temp520=( void*)( tms->tl)->hd; struct Cyc_List_List* _temp526; struct Cyc_Position_Segment*
_temp528; _LL522: if(( unsigned int) _temp520 > 1u?(( struct _enum_struct*)
_temp520)->tag == Cyc_Absyn_Attributes_mod_tag: 0){ _LL529: _temp528=( struct
Cyc_Position_Segment*)(( struct Cyc_Absyn_Attributes_mod_struct*) _temp520)->f1;
goto _LL527; _LL527: _temp526=( struct Cyc_List_List*)(( struct Cyc_Absyn_Attributes_mod_struct*)
_temp520)->f2; goto _LL523;} else{ goto _LL524;} _LL524: goto _LL525; _LL523:
fn_atts= _temp526; tms= tms->tl; goto _LL521; _LL525: goto _LL521; _LL521:;} if((((
! _temp500? _temp502 != 0: 0)? _temp502->tl == 0: 0)?(*(( struct _tuple2*)
_temp502->hd)).f1 == 0: 0)?(*(( struct _tuple2*) _temp502->hd)).f3 == Cyc_Absyn_VoidType:
0){ _temp502= 0; _temp500= 0;} t= Cyc_Parse_array2ptr( t);(( void(*)( void(* f)(
struct _tuple2*), struct Cyc_List_List* x)) Cyc_List_iter)( Cyc_Parse_arg_array2ptr,
_temp502); return Cyc_Parse_apply_tms( Cyc_Absyn_empty_tqual(), Cyc_Absyn_function_typ(
typvars, _temp498, t, _temp502, _temp500, fn_atts), atts, tms->tl);} _LL497:(
void) _throw((( struct _xenum_struct*(*)( struct _tagged_string msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)(( struct _tagged_string)({ char* _temp530=( char*)"function declaration without parameter types";
struct _tagged_string _temp531; _temp531.curr= _temp530; _temp531.base= _temp530;
_temp531.last_plus_one= _temp530 + 45; _temp531;}), _temp504)); _LL493:;} _LL465:
if( tms->tl == 0){ return({ struct _tuple6 _temp532; _temp532.f1= tq; _temp532.f2=
t; _temp532.f3= _temp478; _temp532.f4= atts; _temp532;});}( void) _throw(((
struct _xenum_struct*(*)( struct _tagged_string msg, struct Cyc_Position_Segment*
sg)) Cyc_Parse_abort)(( struct _tagged_string)({ char* _temp533=( char*)"type parameters must appear before function arguments in declarator";
struct _tagged_string _temp534; _temp534.curr= _temp533; _temp534.base= _temp533;
_temp534.last_plus_one= _temp533 + 68; _temp534;}), _temp476)); _LL467: { void*
_temp535= _temp484; struct Cyc_Absyn_Exp* _temp543; struct Cyc_Absyn_Exp*
_temp545; _LL537: if(( unsigned int) _temp535 > 1u?(( struct _enum_struct*)
_temp535)->tag == Cyc_Absyn_NonNullable_ps_tag: 0){ _LL544: _temp543=( struct
Cyc_Absyn_Exp*)(( struct Cyc_Absyn_NonNullable_ps_struct*) _temp535)->f1; goto
_LL538;} else{ goto _LL539;} _LL539: if(( unsigned int) _temp535 > 1u?(( struct
_enum_struct*) _temp535)->tag == Cyc_Absyn_Nullable_ps_tag: 0){ _LL546: _temp545=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Nullable_ps_struct*) _temp535)->f1;
goto _LL540;} else{ goto _LL541;} _LL541: if( _temp535 == Cyc_Absyn_TaggedArray_ps){
goto _LL542;} else{ goto _LL536;} _LL538: return Cyc_Parse_apply_tms( _temp480,
Cyc_Absyn_atb_typ( t, _temp482, tq,({ struct Cyc_Absyn_Upper_b_struct* _temp547=(
struct Cyc_Absyn_Upper_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct));
_temp547->tag= Cyc_Absyn_Upper_b_tag; _temp547->f1= _temp543;( void*) _temp547;})),
atts, tms->tl); _LL540: return Cyc_Parse_apply_tms( _temp480, Cyc_Absyn_starb_typ(
t, _temp482, tq,({ struct Cyc_Absyn_Upper_b_struct* _temp548=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp548->tag= Cyc_Absyn_Upper_b_tag;
_temp548->f1= _temp545;( void*) _temp548;})), atts, tms->tl); _LL542: return Cyc_Parse_apply_tms(
_temp480, Cyc_Absyn_tagged_typ( t, _temp482, tq), atts, tms->tl); _LL536:;}
_LL469: return Cyc_Parse_apply_tms( tq, t,(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( atts, _temp486),
tms->tl); _LL457:;}} void* Cyc_Parse_speclist2typ( struct Cyc_List_List* tss,
struct Cyc_Position_Segment* loc){ struct Cyc_Core_Opt* _temp551; void* _temp553;
struct _tuple5 _temp549= Cyc_Parse_collapse_type_specifiers( tss, loc); _LL554:
_temp553= _temp549.f1; goto _LL552; _LL552: _temp551= _temp549.f2; goto _LL550;
_LL550: if( _temp551 != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char*
_temp555=( char*)"ignoring nested type declaration(s) in specifier list"; struct
_tagged_string _temp556; _temp556.curr= _temp555; _temp556.base= _temp555;
_temp556.last_plus_one= _temp555 + 54; _temp556;}), loc);} return _temp553;}
static struct Cyc_Absyn_Stmt* Cyc_Parse_flatten_decl( struct Cyc_Absyn_Decl* d,
struct Cyc_Absyn_Stmt* s){ return Cyc_Absyn_new_stmt(({ struct Cyc_Absyn_Decl_s_struct*
_temp557=( struct Cyc_Absyn_Decl_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
_temp557->tag= Cyc_Absyn_Decl_s_tag; _temp557->f1= d; _temp557->f2= s;( void*)
_temp557;}), Cyc_Position_segment_join( d->loc, s->loc));} static struct Cyc_Absyn_Stmt*
Cyc_Parse_flatten_declarations( struct Cyc_List_List* ds, struct Cyc_Absyn_Stmt*
s){ return(( struct Cyc_Absyn_Stmt*(*)( struct Cyc_Absyn_Stmt*(* f)( struct Cyc_Absyn_Decl*,
struct Cyc_Absyn_Stmt*), struct Cyc_List_List* x, struct Cyc_Absyn_Stmt* accum))
Cyc_List_fold_right)( Cyc_Parse_flatten_decl, ds, s);} static struct Cyc_List_List*
Cyc_Parse_make_declarations( struct Cyc_Parse_Declaration_spec* ds, struct Cyc_List_List*
ids, struct Cyc_Position_Segment* loc){ struct Cyc_List_List* tss= ds->type_specs;
struct Cyc_Absyn_Tqual* tq= ds->tq; int istypedef= 0; void* s= Cyc_Absyn_Public;
struct Cyc_List_List* atts= ds->attributes; if( ds->is_inline){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp558=( char*)"inline is only allowed on function definitions";
struct _tagged_string _temp559; _temp559.curr= _temp558; _temp559.base= _temp558;
_temp559.last_plus_one= _temp558 + 47; _temp559;}), loc);} if( tss == 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp560=( char*)"missing type specifiers in declaration";
struct _tagged_string _temp561; _temp561.curr= _temp560; _temp561.base= _temp560;
_temp561.last_plus_one= _temp560 + 39; _temp561;}), loc); return 0;} if( ds->sc
!= 0){ void* _temp562=( void*)( ds->sc)->v; _LL564: if( _temp562 == Cyc_Parse_Typedef_sc){
goto _LL565;} else{ goto _LL566;} _LL566: if( _temp562 == Cyc_Parse_Extern_sc){
goto _LL567;} else{ goto _LL568;} _LL568: if( _temp562 == Cyc_Parse_ExternC_sc){
goto _LL569;} else{ goto _LL570;} _LL570: if( _temp562 == Cyc_Parse_Static_sc){
goto _LL571;} else{ goto _LL572;} _LL572: if( _temp562 == Cyc_Parse_Auto_sc){
goto _LL573;} else{ goto _LL574;} _LL574: if( _temp562 == Cyc_Parse_Register_sc){
goto _LL575;} else{ goto _LL576;} _LL576: if( _temp562 == Cyc_Parse_Abstract_sc){
goto _LL577;} else{ goto _LL563;} _LL565: istypedef= 1; goto _LL563; _LL567: s=
Cyc_Absyn_Extern; goto _LL563; _LL569: s= Cyc_Absyn_ExternC; goto _LL563; _LL571:
s= Cyc_Absyn_Static; goto _LL563; _LL573: s= Cyc_Absyn_Public; goto _LL563;
_LL575: s= Cyc_Absyn_Public; goto _LL563; _LL577: s= Cyc_Absyn_Abstract; goto
_LL563; _LL563:;}{ struct Cyc_List_List* _temp580; struct Cyc_List_List*
_temp582; struct _tuple0 _temp578=(( struct _tuple0(*)( struct Cyc_List_List* x))
Cyc_List_split)( ids); _LL583: _temp582= _temp578.f1; goto _LL581; _LL581:
_temp580= _temp578.f2; goto _LL579; _LL579: { int exps_empty= 1;{ struct Cyc_List_List*
es= _temp580; for( 0; es != 0; es= es->tl){ if(( struct Cyc_Absyn_Exp*) es->hd
!= 0){ exps_empty= 0; break;}}}{ struct _tuple5 ts_info= Cyc_Parse_collapse_type_specifiers(
tss, loc); if( _temp582 == 0){ struct Cyc_Core_Opt* _temp586; void* _temp588;
struct _tuple5 _temp584= ts_info; _LL589: _temp588= _temp584.f1; goto _LL587;
_LL587: _temp586= _temp584.f2; goto _LL585; _LL585: if( _temp586 != 0){ struct
Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*) _temp586->v;{ void* _temp590=( void*)
d->r; struct Cyc_Absyn_Structdecl* _temp600; struct Cyc_Absyn_Enumdecl* _temp602;
struct Cyc_Absyn_Xenumdecl* _temp604; _LL592: if(( unsigned int) _temp590 > 1u?((
struct _enum_struct*) _temp590)->tag == Cyc_Absyn_Struct_d_tag: 0){ _LL601:
_temp600=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*)
_temp590)->f1; goto _LL593;} else{ goto _LL594;} _LL594: if(( unsigned int)
_temp590 > 1u?(( struct _enum_struct*) _temp590)->tag == Cyc_Absyn_Enum_d_tag: 0){
_LL603: _temp602=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*)
_temp590)->f1; goto _LL595;} else{ goto _LL596;} _LL596: if(( unsigned int)
_temp590 > 1u?(( struct _enum_struct*) _temp590)->tag == Cyc_Absyn_Xenum_d_tag:
0){ _LL605: _temp604=( struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_d_struct*)
_temp590)->f1; goto _LL597;} else{ goto _LL598;} _LL598: goto _LL599; _LL593:(
void*)( _temp600->sc=( void*) s); _temp600->attributes= atts; goto _LL591;
_LL595:( void*)( _temp602->sc=( void*) s); if( atts != 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp606=( char*)"bad attributes on enum"; struct
_tagged_string _temp607; _temp607.curr= _temp606; _temp607.base= _temp606;
_temp607.last_plus_one= _temp606 + 23; _temp607;}), loc);} goto _LL591; _LL597:(
void*)( _temp604->sc=( void*) s); if( atts != 0){ Cyc_Parse_err(( struct
_tagged_string)({ char* _temp608=( char*)"bad attributes on xenum"; struct
_tagged_string _temp609; _temp609.curr= _temp608; _temp609.base= _temp608;
_temp609.last_plus_one= _temp608 + 24; _temp609;}), loc);} goto _LL591; _LL599:
Cyc_Parse_err(( struct _tagged_string)({ char* _temp610=( char*)"bad declaration";
struct _tagged_string _temp611; _temp611.curr= _temp610; _temp611.base= _temp610;
_temp611.last_plus_one= _temp610 + 16; _temp611;}), loc); return 0; _LL591:;}
return({ struct Cyc_List_List* _temp612=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp612->hd=( void*) d; _temp612->tl= 0;
_temp612;});} else{ void* _temp613= _temp588; struct Cyc_Absyn_Structdecl**
_temp625; struct Cyc_List_List* _temp627; struct _tuple1* _temp629; struct Cyc_Absyn_Enumdecl**
_temp631; struct Cyc_List_List* _temp633; struct _tuple1* _temp635; struct Cyc_Absyn_Xenumdecl**
_temp637; struct _tuple1* _temp639; _LL615: if(( unsigned int) _temp613 > 5u?((
struct _enum_struct*) _temp613)->tag == Cyc_Absyn_StructType_tag: 0){ _LL630:
_temp629=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp613)->f1;
goto _LL628; _LL628: _temp627=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp613)->f2; goto _LL626; _LL626: _temp625=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp613)->f3; goto _LL616;} else{ goto
_LL617;} _LL617: if(( unsigned int) _temp613 > 5u?(( struct _enum_struct*)
_temp613)->tag == Cyc_Absyn_EnumType_tag: 0){ _LL636: _temp635=( struct _tuple1*)((
struct Cyc_Absyn_EnumType_struct*) _temp613)->f1; goto _LL634; _LL634: _temp633=(
struct Cyc_List_List*)(( struct Cyc_Absyn_EnumType_struct*) _temp613)->f2; goto
_LL632; _LL632: _temp631=( struct Cyc_Absyn_Enumdecl**)(( struct Cyc_Absyn_EnumType_struct*)
_temp613)->f3; goto _LL618;} else{ goto _LL619;} _LL619: if(( unsigned int)
_temp613 > 5u?(( struct _enum_struct*) _temp613)->tag == Cyc_Absyn_XenumType_tag:
0){ _LL640: _temp639=( struct _tuple1*)(( struct Cyc_Absyn_XenumType_struct*)
_temp613)->f1; goto _LL638; _LL638: _temp637=( struct Cyc_Absyn_Xenumdecl**)((
struct Cyc_Absyn_XenumType_struct*) _temp613)->f2; goto _LL620;} else{ goto
_LL621;} _LL621: if( _temp613 == Cyc_Absyn_UnionType){ goto _LL622;} else{ goto
_LL623;} _LL623: goto _LL624; _LL616: { struct Cyc_List_List* ts2=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc,
_temp627); struct Cyc_Absyn_Structdecl* sd=({ struct Cyc_Absyn_Structdecl*
_temp641=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp641->sc=( void*) s; _temp641->name=({ struct Cyc_Core_Opt* _temp642=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp642->v=(
void*)(( struct _tuple1*)({ struct _tuple1* _temp643= _temp629; if( _temp643 ==
0){ _throw( Null_Exception);} _temp643;})); _temp642;}); _temp641->tvs= ts2;
_temp641->fields= 0; _temp641->attributes= 0; _temp641;}); if( atts != 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp644=( char*)"bad attributes on struct";
struct _tagged_string _temp645; _temp645.curr= _temp644; _temp645.base= _temp644;
_temp645.last_plus_one= _temp644 + 25; _temp645;}), loc);} return({ struct Cyc_List_List*
_temp646=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp646->hd=( void*)({ struct Cyc_Absyn_Decl* _temp647=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp647->r=( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp648=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp648->tag= Cyc_Absyn_Struct_d_tag; _temp648->f1= sd;( void*) _temp648;});
_temp647->loc= loc; _temp647;}); _temp646->tl= 0; _temp646;});} _LL618: { struct
Cyc_List_List* ts2=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc, _temp633); struct
Cyc_Absyn_Decl* ed= Cyc_Absyn_enum_decl( s,({ struct Cyc_Core_Opt* _temp649=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp649->v=(
void*)(( struct _tuple1*)({ struct _tuple1* _temp650= _temp635; if( _temp650 ==
0){ _throw( Null_Exception);} _temp650;})); _temp649;}), ts2, 0, loc); if( atts
!= 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp651=( char*)"bad attributes on enum";
struct _tagged_string _temp652; _temp652.curr= _temp651; _temp652.base= _temp651;
_temp652.last_plus_one= _temp651 + 23; _temp652;}), loc);} return({ struct Cyc_List_List*
_temp653=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp653->hd=( void*) ed; _temp653->tl= 0; _temp653;});} _LL620: { struct Cyc_Absyn_Xenumdecl*
ed=({ struct Cyc_Absyn_Xenumdecl* _temp654=( struct Cyc_Absyn_Xenumdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Xenumdecl)); _temp654->sc=( void*) s;
_temp654->name= _temp639; _temp654->fields= 0; _temp654;}); if( atts != 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp655=( char*)"bad attributes on xenum";
struct _tagged_string _temp656; _temp656.curr= _temp655; _temp656.base= _temp655;
_temp656.last_plus_one= _temp655 + 24; _temp656;}), loc);} return({ struct Cyc_List_List*
_temp657=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp657->hd=( void*)({ struct Cyc_Absyn_Decl* _temp658=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp658->r=( void*)({ struct Cyc_Absyn_Xenum_d_struct*
_temp659=( struct Cyc_Absyn_Xenum_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Xenum_d_struct));
_temp659->tag= Cyc_Absyn_Xenum_d_tag; _temp659->f1= ed;( void*) _temp659;});
_temp658->loc= loc; _temp658;}); _temp657->tl= 0; _temp657;});} _LL622: if( atts
!= 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp660=( char*)"bad attributes on union";
struct _tagged_string _temp661; _temp661.curr= _temp660; _temp661.base= _temp660;
_temp661.last_plus_one= _temp660 + 24; _temp661;}), loc);} return({ struct Cyc_List_List*
_temp662=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp662->hd=( void*)({ struct Cyc_Absyn_Decl* _temp663=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp663->r=( void*) Cyc_Absyn_Union_d;
_temp663->loc= loc; _temp663;}); _temp662->tl= 0; _temp662;}); _LL624: Cyc_Parse_err((
struct _tagged_string)({ char* _temp664=( char*)"missing declarator"; struct
_tagged_string _temp665; _temp665.curr= _temp664; _temp665.base= _temp664;
_temp665.last_plus_one= _temp664 + 19; _temp665;}), loc); return 0; _LL614:;}}
else{ void* t= ts_info.f1; struct Cyc_List_List* fields= Cyc_Parse_apply_tmss(
tq, t, _temp582, atts); if( istypedef){ if( ! exps_empty){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp666=( char*)"initializer in typedef declaration";
struct _tagged_string _temp667; _temp667.curr= _temp666; _temp667.base= _temp666;
_temp667.last_plus_one= _temp666 + 35; _temp667;}), loc);}{ struct Cyc_List_List*
decls=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Decl*(* f)( struct Cyc_Position_Segment*,
struct _tuple7*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_v_typ_to_typedef, loc, fields); if( ts_info.f2 != 0){
struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)( ts_info.f2)->v;{ void*
_temp668=( void*) d->r; struct Cyc_Absyn_Structdecl* _temp678; struct Cyc_Absyn_Enumdecl*
_temp680; struct Cyc_Absyn_Xenumdecl* _temp682; _LL670: if(( unsigned int)
_temp668 > 1u?(( struct _enum_struct*) _temp668)->tag == Cyc_Absyn_Struct_d_tag:
0){ _LL679: _temp678=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*)
_temp668)->f1; goto _LL671;} else{ goto _LL672;} _LL672: if(( unsigned int)
_temp668 > 1u?(( struct _enum_struct*) _temp668)->tag == Cyc_Absyn_Enum_d_tag: 0){
_LL681: _temp680=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*)
_temp668)->f1; goto _LL673;} else{ goto _LL674;} _LL674: if(( unsigned int)
_temp668 > 1u?(( struct _enum_struct*) _temp668)->tag == Cyc_Absyn_Xenum_d_tag:
0){ _LL683: _temp682=( struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_d_struct*)
_temp668)->f1; goto _LL675;} else{ goto _LL676;} _LL676: goto _LL677; _LL671:(
void*)( _temp678->sc=( void*) s); _temp678->attributes= atts; atts= 0; goto
_LL669; _LL673:( void*)( _temp680->sc=( void*) s); goto _LL669; _LL675:( void*)(
_temp682->sc=( void*) s); goto _LL669; _LL677: Cyc_Parse_err(( struct
_tagged_string)({ char* _temp684=( char*)"declaration within typedef is not a struct,enum, or xenum";
struct _tagged_string _temp685; _temp685.curr= _temp684; _temp685.base= _temp684;
_temp685.last_plus_one= _temp684 + 58; _temp685;}), loc); goto _LL669; _LL669:;}
decls=({ struct Cyc_List_List* _temp686=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp686->hd=( void*) d; _temp686->tl= decls;
_temp686;});} if( atts != 0){ Cyc_Parse_err(({ struct _tagged_string _temp687=
Cyc_Absyn_attribute2string(( void*) atts->hd); xprintf("bad attribute %.*s in typedef",
_temp687.last_plus_one - _temp687.curr, _temp687.curr);}), loc);} return decls;}}
else{ if( ts_info.f2 != 0){ Cyc_Parse_unimp2(( struct _tagged_string)({ char*
_temp688=( char*)"nested type declaration within declarator"; struct
_tagged_string _temp689; _temp689.curr= _temp688; _temp689.base= _temp688;
_temp689.last_plus_one= _temp688 + 42; _temp689;}), loc);}{ struct Cyc_List_List*
decls= 0;{ struct Cyc_List_List* ds= fields; for( 0; ds != 0; ds= ds->tl,
_temp580= _temp580->tl){ struct _tuple7 _temp692; struct Cyc_List_List* _temp693;
struct Cyc_List_List* _temp695; void* _temp697; struct Cyc_Absyn_Tqual* _temp699;
struct _tuple1* _temp701; struct _tuple7* _temp690=( struct _tuple7*) ds->hd;
_temp692=* _temp690; _LL702: _temp701= _temp692.f1; goto _LL700; _LL700:
_temp699= _temp692.f2; goto _LL698; _LL698: _temp697= _temp692.f3; goto _LL696;
_LL696: _temp695= _temp692.f4; goto _LL694; _LL694: _temp693= _temp692.f5; goto
_LL691; _LL691: if( _temp695 != 0){ Cyc_Parse_warn(( struct _tagged_string)({
char* _temp703=( char*)"bad type params, ignoring"; struct _tagged_string
_temp704; _temp704.curr= _temp703; _temp704.base= _temp703; _temp704.last_plus_one=
_temp703 + 26; _temp704;}), loc);} if( _temp580 == 0){(( void(*)( struct
_tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct
_tagged_string)({ char* _temp705=( char*)"unexpected null in parse!"; struct
_tagged_string _temp706; _temp706.curr= _temp705; _temp706.base= _temp705;
_temp706.last_plus_one= _temp705 + 26; _temp706;}), loc);}{ struct Cyc_Absyn_Exp*
eopt=( struct Cyc_Absyn_Exp*) _temp580->hd; struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_new_vardecl(
_temp701, _temp697, eopt); vd->tq= _temp699;( void*)( vd->sc=( void*) s); vd->attributes=
_temp693;{ struct Cyc_Absyn_Decl* d=({ struct Cyc_Absyn_Decl* _temp707=( struct
Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp707->r=( void*)({
struct Cyc_Absyn_Var_d_struct* _temp708=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp708->tag= Cyc_Absyn_Var_d_tag;
_temp708->f1= vd;( void*) _temp708;}); _temp707->loc= loc; _temp707;}); decls=({
struct Cyc_List_List* _temp709=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp709->hd=( void*) d; _temp709->tl= decls; _temp709;});}}}}
return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
decls);}}}}}}} static void* Cyc_Parse_id_to_kind( struct _tagged_string s,
struct Cyc_Position_Segment* loc){ if( Cyc_String_strlen( s) !=( unsigned int) 1){
Cyc_Parse_err(({ struct _tagged_string _temp710= s; xprintf("bad kind: %.*s",
_temp710.last_plus_one - _temp710.curr, _temp710.curr);}), loc); return Cyc_Absyn_BoxKind;}
else{ switch(({ struct _tagged_string _temp711= s; char* _temp713= _temp711.curr
+ 0; if( _temp713 < _temp711.base? 1: _temp713 >= _temp711.last_plus_one){
_throw( Null_Exception);}* _temp713;})){ case 'A': _LL714: return Cyc_Absyn_AnyKind;
case 'M': _LL715: return Cyc_Absyn_MemKind; case 'B': _LL716: return Cyc_Absyn_BoxKind;
case 'R': _LL717: return Cyc_Absyn_RgnKind; case 'E': _LL718: return Cyc_Absyn_EffKind;
default: _LL719: Cyc_Parse_err(({ struct _tagged_string _temp721= s; xprintf("bad kind: %.*s",
_temp721.last_plus_one - _temp721.curr, _temp721.curr);}), loc); return Cyc_Absyn_BoxKind;}}}
static struct Cyc_Absyn_Decl* Cyc_Parse_v_typ_to_typedef( struct Cyc_Position_Segment*
loc, struct _tuple7* t){ struct _tuple1* x=(* t).f1; Cyc_Lex_register_typedef( x);
if((* t).f5 != 0){ Cyc_Parse_err(({ struct _tagged_string _temp722= Cyc_Absyn_attribute2string((
void*)((* t).f5)->hd); xprintf("bad attribute %.*s within typedef", _temp722.last_plus_one
- _temp722.curr, _temp722.curr);}), loc);} return Cyc_Absyn_new_decl(({ struct
Cyc_Absyn_Typedef_d_struct* _temp723=( struct Cyc_Absyn_Typedef_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct)); _temp723->tag= Cyc_Absyn_Typedef_d_tag;
_temp723->f1=({ struct Cyc_Absyn_Typedefdecl* _temp724=( struct Cyc_Absyn_Typedefdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedefdecl)); _temp724->name= x; _temp724->tvs=(*
t).f4; _temp724->defn=( void*)(* t).f3; _temp724;});( void*) _temp723;}), loc);}
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
struct _xenum_struct* yy1){ struct _xenum_struct* _temp725= yy1; struct _tuple11*
_temp731; _LL727: if((*(( struct _xenum_struct*) _temp725)).tag == Cyc_Int_tok_tag){
_LL732: _temp731=(( struct Cyc_Int_tok_struct*) _temp725)->f1; goto _LL728;}
else{ goto _LL729;} _LL729: goto _LL730; _LL728: return _temp731; _LL730:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp733=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp733=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp734=( char*)"Int_tok";
struct _tagged_string _temp735; _temp735.curr= _temp734; _temp735.base= _temp734;
_temp735.last_plus_one= _temp734 + 8; _temp735;})};( struct _xenum_struct*)
_temp733;})); _LL726:;} struct _tagged_string Cyc_yyget_String_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp736= yy1; struct _tagged_string
_temp742; _LL738: if((*(( struct _xenum_struct*) _temp736)).tag == Cyc_String_tok_tag){
_LL743: _temp742=(( struct Cyc_String_tok_struct*) _temp736)->f1; goto _LL739;}
else{ goto _LL740;} _LL740: goto _LL741; _LL739: return _temp742; _LL741:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp744=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp744=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp745=( char*)"String_tok";
struct _tagged_string _temp746; _temp746.curr= _temp745; _temp746.base= _temp745;
_temp746.last_plus_one= _temp745 + 11; _temp746;})};( struct _xenum_struct*)
_temp744;})); _LL737:;} char Cyc_yyget_Char_tok( struct _xenum_struct* yy1){
struct _xenum_struct* _temp747= yy1; char _temp753; _LL749: if((*(( struct
_xenum_struct*) _temp747)).tag == Cyc_Char_tok_tag){ _LL754: _temp753=(( struct
Cyc_Char_tok_struct*) _temp747)->f1; goto _LL750;} else{ goto _LL751;} _LL751:
goto _LL752; _LL750: return _temp753; _LL752:( void) _throw(({ struct Cyc_Core_Failure_struct*
_temp755=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));*
_temp755=( struct Cyc_Core_Failure_struct){.tag= Cyc_Core_Failure_tag,.f1=(
struct _tagged_string)({ char* _temp756=( char*)"Char_tok"; struct
_tagged_string _temp757; _temp757.curr= _temp756; _temp757.base= _temp756;
_temp757.last_plus_one= _temp756 + 9; _temp757;})};( struct _xenum_struct*)
_temp755;})); _LL748:;} void* Cyc_yyget_Pointer_Sort_tok( struct _xenum_struct*
yy1){ struct _xenum_struct* _temp758= yy1; void* _temp764; _LL760: if((*((
struct _xenum_struct*) _temp758)).tag == Cyc_Pointer_Sort_tok_tag){ _LL765:
_temp764=(( struct Cyc_Pointer_Sort_tok_struct*) _temp758)->f1; goto _LL761;}
else{ goto _LL762;} _LL762: goto _LL763; _LL761: return _temp764; _LL763:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp766=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp766=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp767=( char*)"Pointer_Sort_tok";
struct _tagged_string _temp768; _temp768.curr= _temp767; _temp768.base= _temp767;
_temp768.last_plus_one= _temp767 + 17; _temp768;})};( struct _xenum_struct*)
_temp766;})); _LL759:;} struct Cyc_Absyn_Exp* Cyc_yyget_Exp_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp769= yy1; struct Cyc_Absyn_Exp*
_temp775; _LL771: if((*(( struct _xenum_struct*) _temp769)).tag == Cyc_Exp_tok_tag){
_LL776: _temp775=(( struct Cyc_Exp_tok_struct*) _temp769)->f1; goto _LL772;}
else{ goto _LL773;} _LL773: goto _LL774; _LL772: return _temp775; _LL774:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp777=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp777=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp778=( char*)"Exp_tok";
struct _tagged_string _temp779; _temp779.curr= _temp778; _temp779.base= _temp778;
_temp779.last_plus_one= _temp778 + 8; _temp779;})};( struct _xenum_struct*)
_temp777;})); _LL770:;} struct Cyc_List_List* Cyc_yyget_ExpList_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp780= yy1; struct Cyc_List_List*
_temp786; _LL782: if((*(( struct _xenum_struct*) _temp780)).tag == Cyc_ExpList_tok_tag){
_LL787: _temp786=(( struct Cyc_ExpList_tok_struct*) _temp780)->f1; goto _LL783;}
else{ goto _LL784;} _LL784: goto _LL785; _LL783: return _temp786; _LL785:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp788=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp788=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp789=( char*)"ExpList_tok";
struct _tagged_string _temp790; _temp790.curr= _temp789; _temp790.base= _temp789;
_temp790.last_plus_one= _temp789 + 12; _temp790;})};( struct _xenum_struct*)
_temp788;})); _LL781:;} struct Cyc_List_List* Cyc_yyget_InitializerList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp791= yy1; struct Cyc_List_List*
_temp797; _LL793: if((*(( struct _xenum_struct*) _temp791)).tag == Cyc_InitializerList_tok_tag){
_LL798: _temp797=(( struct Cyc_InitializerList_tok_struct*) _temp791)->f1; goto
_LL794;} else{ goto _LL795;} _LL795: goto _LL796; _LL794: return _temp797;
_LL796:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp799=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp799=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp800=( char*)"InitializerList_tok";
struct _tagged_string _temp801; _temp801.curr= _temp800; _temp801.base= _temp800;
_temp801.last_plus_one= _temp800 + 20; _temp801;})};( struct _xenum_struct*)
_temp799;})); _LL792:;} void* Cyc_yyget_Primop_tok( struct _xenum_struct* yy1){
struct _xenum_struct* _temp802= yy1; void* _temp808; _LL804: if((*(( struct
_xenum_struct*) _temp802)).tag == Cyc_Primop_tok_tag){ _LL809: _temp808=((
struct Cyc_Primop_tok_struct*) _temp802)->f1; goto _LL805;} else{ goto _LL806;}
_LL806: goto _LL807; _LL805: return _temp808; _LL807:( void) _throw(({ struct
Cyc_Core_Failure_struct* _temp810=( struct Cyc_Core_Failure_struct*) GC_malloc(
sizeof( struct Cyc_Core_Failure_struct));* _temp810=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp811=( char*)"Primop_tok";
struct _tagged_string _temp812; _temp812.curr= _temp811; _temp812.base= _temp811;
_temp812.last_plus_one= _temp811 + 11; _temp812;})};( struct _xenum_struct*)
_temp810;})); _LL803:;} struct Cyc_Core_Opt* Cyc_yyget_Primopopt_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp813= yy1; struct Cyc_Core_Opt*
_temp819; _LL815: if((*(( struct _xenum_struct*) _temp813)).tag == Cyc_Primopopt_tok_tag){
_LL820: _temp819=(( struct Cyc_Primopopt_tok_struct*) _temp813)->f1; goto _LL816;}
else{ goto _LL817;} _LL817: goto _LL818; _LL816: return _temp819; _LL818:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp821=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp821=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp822=( char*)"Primopopt_tok";
struct _tagged_string _temp823; _temp823.curr= _temp822; _temp823.base= _temp822;
_temp823.last_plus_one= _temp822 + 14; _temp823;})};( struct _xenum_struct*)
_temp821;})); _LL814:;} struct _tuple1* Cyc_yyget_QualId_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp824= yy1; struct _tuple1*
_temp830; _LL826: if((*(( struct _xenum_struct*) _temp824)).tag == Cyc_QualId_tok_tag){
_LL831: _temp830=(( struct Cyc_QualId_tok_struct*) _temp824)->f1; goto _LL827;}
else{ goto _LL828;} _LL828: goto _LL829; _LL827: return _temp830; _LL829:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp832=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp832=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp833=( char*)"QualId_tok";
struct _tagged_string _temp834; _temp834.curr= _temp833; _temp834.base= _temp833;
_temp834.last_plus_one= _temp833 + 11; _temp834;})};( struct _xenum_struct*)
_temp832;})); _LL825:;} struct Cyc_Absyn_Stmt* Cyc_yyget_Stmt_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp835= yy1; struct Cyc_Absyn_Stmt*
_temp841; _LL837: if((*(( struct _xenum_struct*) _temp835)).tag == Cyc_Stmt_tok_tag){
_LL842: _temp841=(( struct Cyc_Stmt_tok_struct*) _temp835)->f1; goto _LL838;}
else{ goto _LL839;} _LL839: goto _LL840; _LL838: return _temp841; _LL840:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp843=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp843=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp844=( char*)"Stmt_tok";
struct _tagged_string _temp845; _temp845.curr= _temp844; _temp845.base= _temp844;
_temp845.last_plus_one= _temp844 + 9; _temp845;})};( struct _xenum_struct*)
_temp843;})); _LL836:;} void* Cyc_yyget_BlockItem_tok( struct _xenum_struct* yy1){
struct _xenum_struct* _temp846= yy1; void* _temp852; _LL848: if((*(( struct
_xenum_struct*) _temp846)).tag == Cyc_BlockItem_tok_tag){ _LL853: _temp852=((
struct Cyc_BlockItem_tok_struct*) _temp846)->f1; goto _LL849;} else{ goto _LL850;}
_LL850: goto _LL851; _LL849: return _temp852; _LL851:( void) _throw(({ struct
Cyc_Core_Failure_struct* _temp854=( struct Cyc_Core_Failure_struct*) GC_malloc(
sizeof( struct Cyc_Core_Failure_struct));* _temp854=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp855=( char*)"BlockItem_tok";
struct _tagged_string _temp856; _temp856.curr= _temp855; _temp856.base= _temp855;
_temp856.last_plus_one= _temp855 + 14; _temp856;})};( struct _xenum_struct*)
_temp854;})); _LL847:;} struct Cyc_List_List* Cyc_yyget_SwitchClauseList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp857= yy1; struct Cyc_List_List*
_temp863; _LL859: if((*(( struct _xenum_struct*) _temp857)).tag == Cyc_SwitchClauseList_tok_tag){
_LL864: _temp863=(( struct Cyc_SwitchClauseList_tok_struct*) _temp857)->f1; goto
_LL860;} else{ goto _LL861;} _LL861: goto _LL862; _LL860: return _temp863;
_LL862:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp865=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp865=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp866=( char*)"SwitchClauseList_tok";
struct _tagged_string _temp867; _temp867.curr= _temp866; _temp867.base= _temp866;
_temp867.last_plus_one= _temp866 + 21; _temp867;})};( struct _xenum_struct*)
_temp865;})); _LL858:;} struct Cyc_Absyn_Pat* Cyc_yyget_Pattern_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp868= yy1; struct Cyc_Absyn_Pat*
_temp874; _LL870: if((*(( struct _xenum_struct*) _temp868)).tag == Cyc_Pattern_tok_tag){
_LL875: _temp874=(( struct Cyc_Pattern_tok_struct*) _temp868)->f1; goto _LL871;}
else{ goto _LL872;} _LL872: goto _LL873; _LL871: return _temp874; _LL873:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp876=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp876=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp877=( char*)"Pattern_tok";
struct _tagged_string _temp878; _temp878.curr= _temp877; _temp878.base= _temp877;
_temp878.last_plus_one= _temp877 + 12; _temp878;})};( struct _xenum_struct*)
_temp876;})); _LL869:;} struct Cyc_List_List* Cyc_yyget_PatternList_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp879= yy1; struct Cyc_List_List*
_temp885; _LL881: if((*(( struct _xenum_struct*) _temp879)).tag == Cyc_PatternList_tok_tag){
_LL886: _temp885=(( struct Cyc_PatternList_tok_struct*) _temp879)->f1; goto
_LL882;} else{ goto _LL883;} _LL883: goto _LL884; _LL882: return _temp885;
_LL884:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp887=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp887=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp888=( char*)"PatternList_tok";
struct _tagged_string _temp889; _temp889.curr= _temp888; _temp889.base= _temp888;
_temp889.last_plus_one= _temp888 + 16; _temp889;})};( struct _xenum_struct*)
_temp887;})); _LL880:;} struct _tuple15* Cyc_yyget_FieldPattern_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp890= yy1; struct _tuple15*
_temp896; _LL892: if((*(( struct _xenum_struct*) _temp890)).tag == Cyc_FieldPattern_tok_tag){
_LL897: _temp896=(( struct Cyc_FieldPattern_tok_struct*) _temp890)->f1; goto
_LL893;} else{ goto _LL894;} _LL894: goto _LL895; _LL893: return _temp896;
_LL895:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp898=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp898=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp899=( char*)"FieldPattern_tok";
struct _tagged_string _temp900; _temp900.curr= _temp899; _temp900.base= _temp899;
_temp900.last_plus_one= _temp899 + 17; _temp900;})};( struct _xenum_struct*)
_temp898;})); _LL891:;} struct Cyc_List_List* Cyc_yyget_FieldPatternList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp901= yy1; struct Cyc_List_List*
_temp907; _LL903: if((*(( struct _xenum_struct*) _temp901)).tag == Cyc_FieldPatternList_tok_tag){
_LL908: _temp907=(( struct Cyc_FieldPatternList_tok_struct*) _temp901)->f1; goto
_LL904;} else{ goto _LL905;} _LL905: goto _LL906; _LL904: return _temp907;
_LL906:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp909=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp909=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp910=( char*)"FieldPatternList_tok";
struct _tagged_string _temp911; _temp911.curr= _temp910; _temp911.base= _temp910;
_temp911.last_plus_one= _temp910 + 21; _temp911;})};( struct _xenum_struct*)
_temp909;})); _LL902:;} struct Cyc_Absyn_Fndecl* Cyc_yyget_FnDecl_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp912= yy1; struct Cyc_Absyn_Fndecl*
_temp918; _LL914: if((*(( struct _xenum_struct*) _temp912)).tag == Cyc_FnDecl_tok_tag){
_LL919: _temp918=(( struct Cyc_FnDecl_tok_struct*) _temp912)->f1; goto _LL915;}
else{ goto _LL916;} _LL916: goto _LL917; _LL915: return _temp918; _LL917:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp920=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp920=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp921=( char*)"FnDecl_tok";
struct _tagged_string _temp922; _temp922.curr= _temp921; _temp922.base= _temp921;
_temp922.last_plus_one= _temp921 + 11; _temp922;})};( struct _xenum_struct*)
_temp920;})); _LL913:;} struct Cyc_List_List* Cyc_yyget_DeclList_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp923= yy1; struct Cyc_List_List*
_temp929; _LL925: if((*(( struct _xenum_struct*) _temp923)).tag == Cyc_DeclList_tok_tag){
_LL930: _temp929=(( struct Cyc_DeclList_tok_struct*) _temp923)->f1; goto _LL926;}
else{ goto _LL927;} _LL927: goto _LL928; _LL926: return _temp929; _LL928:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp931=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp931=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp932=( char*)"DeclList_tok";
struct _tagged_string _temp933; _temp933.curr= _temp932; _temp933.base= _temp932;
_temp933.last_plus_one= _temp932 + 13; _temp933;})};( struct _xenum_struct*)
_temp931;})); _LL924:;} struct Cyc_Parse_Declaration_spec* Cyc_yyget_DeclSpec_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp934= yy1; struct Cyc_Parse_Declaration_spec*
_temp940; _LL936: if((*(( struct _xenum_struct*) _temp934)).tag == Cyc_DeclSpec_tok_tag){
_LL941: _temp940=(( struct Cyc_DeclSpec_tok_struct*) _temp934)->f1; goto _LL937;}
else{ goto _LL938;} _LL938: goto _LL939; _LL937: return _temp940; _LL939:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp942=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp942=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp943=( char*)"DeclSpec_tok";
struct _tagged_string _temp944; _temp944.curr= _temp943; _temp944.base= _temp943;
_temp944.last_plus_one= _temp943 + 13; _temp944;})};( struct _xenum_struct*)
_temp942;})); _LL935:;} struct _tuple12* Cyc_yyget_InitDecl_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp945= yy1; struct _tuple12*
_temp951; _LL947: if((*(( struct _xenum_struct*) _temp945)).tag == Cyc_InitDecl_tok_tag){
_LL952: _temp951=(( struct Cyc_InitDecl_tok_struct*) _temp945)->f1; goto _LL948;}
else{ goto _LL949;} _LL949: goto _LL950; _LL948: return _temp951; _LL950:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp953=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp953=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp954=( char*)"InitDecl_tok";
struct _tagged_string _temp955; _temp955.curr= _temp954; _temp955.base= _temp954;
_temp955.last_plus_one= _temp954 + 13; _temp955;})};( struct _xenum_struct*)
_temp953;})); _LL946:;} struct Cyc_List_List* Cyc_yyget_InitDeclList_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp956= yy1; struct Cyc_List_List*
_temp962; _LL958: if((*(( struct _xenum_struct*) _temp956)).tag == Cyc_InitDeclList_tok_tag){
_LL963: _temp962=(( struct Cyc_InitDeclList_tok_struct*) _temp956)->f1; goto
_LL959;} else{ goto _LL960;} _LL960: goto _LL961; _LL959: return _temp962;
_LL961:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp964=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp964=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp965=( char*)"InitDeclList_tok";
struct _tagged_string _temp966; _temp966.curr= _temp965; _temp966.base= _temp965;
_temp966.last_plus_one= _temp965 + 17; _temp966;})};( struct _xenum_struct*)
_temp964;})); _LL957:;} void* Cyc_yyget_StorageClass_tok( struct _xenum_struct*
yy1){ struct _xenum_struct* _temp967= yy1; void* _temp973; _LL969: if((*((
struct _xenum_struct*) _temp967)).tag == Cyc_StorageClass_tok_tag){ _LL974:
_temp973=(( struct Cyc_StorageClass_tok_struct*) _temp967)->f1; goto _LL970;}
else{ goto _LL971;} _LL971: goto _LL972; _LL970: return _temp973; _LL972:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp975=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp975=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp976=( char*)"StorageClass_tok";
struct _tagged_string _temp977; _temp977.curr= _temp976; _temp977.base= _temp976;
_temp977.last_plus_one= _temp976 + 17; _temp977;})};( struct _xenum_struct*)
_temp975;})); _LL968:;} void* Cyc_yyget_TypeSpecifier_tok( struct _xenum_struct*
yy1){ struct _xenum_struct* _temp978= yy1; void* _temp984; _LL980: if((*((
struct _xenum_struct*) _temp978)).tag == Cyc_TypeSpecifier_tok_tag){ _LL985:
_temp984=(( struct Cyc_TypeSpecifier_tok_struct*) _temp978)->f1; goto _LL981;}
else{ goto _LL982;} _LL982: goto _LL983; _LL981: return _temp984; _LL983:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp986=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp986=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp987=( char*)"TypeSpecifier_tok";
struct _tagged_string _temp988; _temp988.curr= _temp987; _temp988.base= _temp987;
_temp988.last_plus_one= _temp987 + 18; _temp988;})};( struct _xenum_struct*)
_temp986;})); _LL979:;} void* Cyc_yyget_StructOrUnion_tok( struct _xenum_struct*
yy1){ struct _xenum_struct* _temp989= yy1; void* _temp995; _LL991: if((*((
struct _xenum_struct*) _temp989)).tag == Cyc_StructOrUnion_tok_tag){ _LL996:
_temp995=(( struct Cyc_StructOrUnion_tok_struct*) _temp989)->f1; goto _LL992;}
else{ goto _LL993;} _LL993: goto _LL994; _LL992: return _temp995; _LL994:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp997=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp997=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp998=( char*)"StructOrUnion_tok";
struct _tagged_string _temp999; _temp999.curr= _temp998; _temp999.base= _temp998;
_temp999.last_plus_one= _temp998 + 18; _temp999;})};( struct _xenum_struct*)
_temp997;})); _LL990:;} struct Cyc_Absyn_Tqual* Cyc_yyget_TypeQual_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp1000= yy1; struct Cyc_Absyn_Tqual*
_temp1006; _LL1002: if((*(( struct _xenum_struct*) _temp1000)).tag == Cyc_TypeQual_tok_tag){
_LL1007: _temp1006=(( struct Cyc_TypeQual_tok_struct*) _temp1000)->f1; goto
_LL1003;} else{ goto _LL1004;} _LL1004: goto _LL1005; _LL1003: return _temp1006;
_LL1005:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1008=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp1008=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp1009=( char*)"TypeQual_tok";
struct _tagged_string _temp1010; _temp1010.curr= _temp1009; _temp1010.base=
_temp1009; _temp1010.last_plus_one= _temp1009 + 13; _temp1010;})};( struct
_xenum_struct*) _temp1008;})); _LL1001:;} struct Cyc_List_List* Cyc_yyget_StructFieldDeclList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1011= yy1; struct Cyc_List_List*
_temp1017; _LL1013: if((*(( struct _xenum_struct*) _temp1011)).tag == Cyc_StructFieldDeclList_tok_tag){
_LL1018: _temp1017=(( struct Cyc_StructFieldDeclList_tok_struct*) _temp1011)->f1;
goto _LL1014;} else{ goto _LL1015;} _LL1015: goto _LL1016; _LL1014: return
_temp1017; _LL1016:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1019=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));*
_temp1019=( struct Cyc_Core_Failure_struct){.tag= Cyc_Core_Failure_tag,.f1=(
struct _tagged_string)({ char* _temp1020=( char*)"StructFieldDeclList_tok";
struct _tagged_string _temp1021; _temp1021.curr= _temp1020; _temp1021.base=
_temp1020; _temp1021.last_plus_one= _temp1020 + 24; _temp1021;})};( struct
_xenum_struct*) _temp1019;})); _LL1012:;} struct Cyc_List_List* Cyc_yyget_StructFieldDeclListList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1022= yy1; struct Cyc_List_List*
_temp1028; _LL1024: if((*(( struct _xenum_struct*) _temp1022)).tag == Cyc_StructFieldDeclListList_tok_tag){
_LL1029: _temp1028=(( struct Cyc_StructFieldDeclListList_tok_struct*) _temp1022)->f1;
goto _LL1025;} else{ goto _LL1026;} _LL1026: goto _LL1027; _LL1025: return
_temp1028; _LL1027:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1030=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));*
_temp1030=( struct Cyc_Core_Failure_struct){.tag= Cyc_Core_Failure_tag,.f1=(
struct _tagged_string)({ char* _temp1031=( char*)"StructFieldDeclListList_tok";
struct _tagged_string _temp1032; _temp1032.curr= _temp1031; _temp1032.base=
_temp1031; _temp1032.last_plus_one= _temp1031 + 28; _temp1032;})};( struct
_xenum_struct*) _temp1030;})); _LL1023:;} struct Cyc_List_List* Cyc_yyget_TypeModifierList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1033= yy1; struct Cyc_List_List*
_temp1039; _LL1035: if((*(( struct _xenum_struct*) _temp1033)).tag == Cyc_TypeModifierList_tok_tag){
_LL1040: _temp1039=(( struct Cyc_TypeModifierList_tok_struct*) _temp1033)->f1;
goto _LL1036;} else{ goto _LL1037;} _LL1037: goto _LL1038; _LL1036: return
_temp1039; _LL1038:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1041=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));*
_temp1041=( struct Cyc_Core_Failure_struct){.tag= Cyc_Core_Failure_tag,.f1=(
struct _tagged_string)({ char* _temp1042=( char*)"TypeModifierList_tok"; struct
_tagged_string _temp1043; _temp1043.curr= _temp1042; _temp1043.base= _temp1042;
_temp1043.last_plus_one= _temp1042 + 21; _temp1043;})};( struct _xenum_struct*)
_temp1041;})); _LL1034:;} void* Cyc_yyget_Rgn_tok( struct _xenum_struct* yy1){
struct _xenum_struct* _temp1044= yy1; void* _temp1050; _LL1046: if((*(( struct
_xenum_struct*) _temp1044)).tag == Cyc_Rgn_tok_tag){ _LL1051: _temp1050=((
struct Cyc_Rgn_tok_struct*) _temp1044)->f1; goto _LL1047;} else{ goto _LL1048;}
_LL1048: goto _LL1049; _LL1047: return _temp1050; _LL1049:( void) _throw(({
struct Cyc_Core_Failure_struct* _temp1052=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp1052=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp1053=( char*)"Rgn_tok";
struct _tagged_string _temp1054; _temp1054.curr= _temp1053; _temp1054.base=
_temp1053; _temp1054.last_plus_one= _temp1053 + 8; _temp1054;})};( struct
_xenum_struct*) _temp1052;})); _LL1045:;} struct Cyc_Parse_Declarator* Cyc_yyget_Declarator_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1055= yy1; struct Cyc_Parse_Declarator*
_temp1061; _LL1057: if((*(( struct _xenum_struct*) _temp1055)).tag == Cyc_Declarator_tok_tag){
_LL1062: _temp1061=(( struct Cyc_Declarator_tok_struct*) _temp1055)->f1; goto
_LL1058;} else{ goto _LL1059;} _LL1059: goto _LL1060; _LL1058: return _temp1061;
_LL1060:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1063=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp1063=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp1064=( char*)"Declarator_tok";
struct _tagged_string _temp1065; _temp1065.curr= _temp1064; _temp1065.base=
_temp1064; _temp1065.last_plus_one= _temp1064 + 15; _temp1065;})};( struct
_xenum_struct*) _temp1063;})); _LL1056:;} struct Cyc_List_List* Cyc_yyget_DeclaratorList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1066= yy1; struct Cyc_List_List*
_temp1072; _LL1068: if((*(( struct _xenum_struct*) _temp1066)).tag == Cyc_DeclaratorList_tok_tag){
_LL1073: _temp1072=(( struct Cyc_DeclaratorList_tok_struct*) _temp1066)->f1;
goto _LL1069;} else{ goto _LL1070;} _LL1070: goto _LL1071; _LL1069: return
_temp1072; _LL1071:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1074=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));*
_temp1074=( struct Cyc_Core_Failure_struct){.tag= Cyc_Core_Failure_tag,.f1=(
struct _tagged_string)({ char* _temp1075=( char*)"DeclaratorList_tok"; struct
_tagged_string _temp1076; _temp1076.curr= _temp1075; _temp1076.base= _temp1075;
_temp1076.last_plus_one= _temp1075 + 19; _temp1076;})};( struct _xenum_struct*)
_temp1074;})); _LL1067:;} struct Cyc_Parse_Abstractdeclarator* Cyc_yyget_AbstractDeclarator_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1077= yy1; struct Cyc_Parse_Abstractdeclarator*
_temp1083; _LL1079: if((*(( struct _xenum_struct*) _temp1077)).tag == Cyc_AbstractDeclarator_tok_tag){
_LL1084: _temp1083=(( struct Cyc_AbstractDeclarator_tok_struct*) _temp1077)->f1;
goto _LL1080;} else{ goto _LL1081;} _LL1081: goto _LL1082; _LL1080: return
_temp1083; _LL1082:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1085=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));*
_temp1085=( struct Cyc_Core_Failure_struct){.tag= Cyc_Core_Failure_tag,.f1=(
struct _tagged_string)({ char* _temp1086=( char*)"AbstractDeclarator_tok";
struct _tagged_string _temp1087; _temp1087.curr= _temp1086; _temp1087.base=
_temp1086; _temp1087.last_plus_one= _temp1086 + 23; _temp1087;})};( struct
_xenum_struct*) _temp1085;})); _LL1078:;} struct Cyc_Absyn_Enumfield* Cyc_yyget_EnumeratorField_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1088= yy1; struct Cyc_Absyn_Enumfield*
_temp1094; _LL1090: if((*(( struct _xenum_struct*) _temp1088)).tag == Cyc_EnumeratorField_tok_tag){
_LL1095: _temp1094=(( struct Cyc_EnumeratorField_tok_struct*) _temp1088)->f1;
goto _LL1091;} else{ goto _LL1092;} _LL1092: goto _LL1093; _LL1091: return
_temp1094; _LL1093:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1096=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));*
_temp1096=( struct Cyc_Core_Failure_struct){.tag= Cyc_Core_Failure_tag,.f1=(
struct _tagged_string)({ char* _temp1097=( char*)"EnumeratorField_tok"; struct
_tagged_string _temp1098; _temp1098.curr= _temp1097; _temp1098.base= _temp1097;
_temp1098.last_plus_one= _temp1097 + 20; _temp1098;})};( struct _xenum_struct*)
_temp1096;})); _LL1089:;} struct Cyc_List_List* Cyc_yyget_EnumeratorFieldList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1099= yy1; struct Cyc_List_List*
_temp1105; _LL1101: if((*(( struct _xenum_struct*) _temp1099)).tag == Cyc_EnumeratorFieldList_tok_tag){
_LL1106: _temp1105=(( struct Cyc_EnumeratorFieldList_tok_struct*) _temp1099)->f1;
goto _LL1102;} else{ goto _LL1103;} _LL1103: goto _LL1104; _LL1102: return
_temp1105; _LL1104:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1107=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));*
_temp1107=( struct Cyc_Core_Failure_struct){.tag= Cyc_Core_Failure_tag,.f1=(
struct _tagged_string)({ char* _temp1108=( char*)"EnumeratorFieldList_tok";
struct _tagged_string _temp1109; _temp1109.curr= _temp1108; _temp1109.base=
_temp1108; _temp1109.last_plus_one= _temp1108 + 24; _temp1109;})};( struct
_xenum_struct*) _temp1107;})); _LL1100:;} struct _tuple13* Cyc_yyget_QualSpecList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1110= yy1; struct
_tuple13* _temp1116; _LL1112: if((*(( struct _xenum_struct*) _temp1110)).tag ==
Cyc_QualSpecList_tok_tag){ _LL1117: _temp1116=(( struct Cyc_QualSpecList_tok_struct*)
_temp1110)->f1; goto _LL1113;} else{ goto _LL1114;} _LL1114: goto _LL1115;
_LL1113: return _temp1116; _LL1115:( void) _throw(({ struct Cyc_Core_Failure_struct*
_temp1118=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));*
_temp1118=( struct Cyc_Core_Failure_struct){.tag= Cyc_Core_Failure_tag,.f1=(
struct _tagged_string)({ char* _temp1119=( char*)"QualSpecList_tok"; struct
_tagged_string _temp1120; _temp1120.curr= _temp1119; _temp1120.base= _temp1119;
_temp1120.last_plus_one= _temp1119 + 17; _temp1120;})};( struct _xenum_struct*)
_temp1118;})); _LL1111:;} struct Cyc_List_List* Cyc_yyget_IdList_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp1121= yy1; struct Cyc_List_List*
_temp1127; _LL1123: if((*(( struct _xenum_struct*) _temp1121)).tag == Cyc_IdList_tok_tag){
_LL1128: _temp1127=(( struct Cyc_IdList_tok_struct*) _temp1121)->f1; goto
_LL1124;} else{ goto _LL1125;} _LL1125: goto _LL1126; _LL1124: return _temp1127;
_LL1126:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1129=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp1129=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp1130=( char*)"IdList_tok";
struct _tagged_string _temp1131; _temp1131.curr= _temp1130; _temp1131.base=
_temp1130; _temp1131.last_plus_one= _temp1130 + 11; _temp1131;})};( struct
_xenum_struct*) _temp1129;})); _LL1122:;} struct _tuple2* Cyc_yyget_ParamDecl_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1132= yy1; struct _tuple2*
_temp1138; _LL1134: if((*(( struct _xenum_struct*) _temp1132)).tag == Cyc_ParamDecl_tok_tag){
_LL1139: _temp1138=(( struct Cyc_ParamDecl_tok_struct*) _temp1132)->f1; goto
_LL1135;} else{ goto _LL1136;} _LL1136: goto _LL1137; _LL1135: return _temp1138;
_LL1137:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1140=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp1140=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp1141=( char*)"ParamDecl_tok";
struct _tagged_string _temp1142; _temp1142.curr= _temp1141; _temp1142.base=
_temp1141; _temp1142.last_plus_one= _temp1141 + 14; _temp1142;})};( struct
_xenum_struct*) _temp1140;})); _LL1133:;} struct Cyc_List_List* Cyc_yyget_ParamDeclList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1143= yy1; struct Cyc_List_List*
_temp1149; _LL1145: if((*(( struct _xenum_struct*) _temp1143)).tag == Cyc_ParamDeclList_tok_tag){
_LL1150: _temp1149=(( struct Cyc_ParamDeclList_tok_struct*) _temp1143)->f1; goto
_LL1146;} else{ goto _LL1147;} _LL1147: goto _LL1148; _LL1146: return _temp1149;
_LL1148:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1151=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp1151=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp1152=( char*)"ParamDeclList_tok";
struct _tagged_string _temp1153; _temp1153.curr= _temp1152; _temp1153.base=
_temp1152; _temp1153.last_plus_one= _temp1152 + 18; _temp1153;})};( struct
_xenum_struct*) _temp1151;})); _LL1144:;} struct _tuple14* Cyc_yyget_ParamDeclListBool_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1154= yy1; struct
_tuple14* _temp1160; _LL1156: if((*(( struct _xenum_struct*) _temp1154)).tag ==
Cyc_ParamDeclListBool_tok_tag){ _LL1161: _temp1160=(( struct Cyc_ParamDeclListBool_tok_struct*)
_temp1154)->f1; goto _LL1157;} else{ goto _LL1158;} _LL1158: goto _LL1159;
_LL1157: return _temp1160; _LL1159:( void) _throw(({ struct Cyc_Core_Failure_struct*
_temp1162=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));*
_temp1162=( struct Cyc_Core_Failure_struct){.tag= Cyc_Core_Failure_tag,.f1=(
struct _tagged_string)({ char* _temp1163=( char*)"ParamDeclListBool_tok"; struct
_tagged_string _temp1164; _temp1164.curr= _temp1163; _temp1164.base= _temp1163;
_temp1164.last_plus_one= _temp1163 + 22; _temp1164;})};( struct _xenum_struct*)
_temp1162;})); _LL1155:;} struct Cyc_List_List* Cyc_yyget_TypeList_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp1165= yy1; struct Cyc_List_List*
_temp1171; _LL1167: if((*(( struct _xenum_struct*) _temp1165)).tag == Cyc_TypeList_tok_tag){
_LL1172: _temp1171=(( struct Cyc_TypeList_tok_struct*) _temp1165)->f1; goto
_LL1168;} else{ goto _LL1169;} _LL1169: goto _LL1170; _LL1168: return _temp1171;
_LL1170:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1173=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp1173=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp1174=( char*)"TypeList_tok";
struct _tagged_string _temp1175; _temp1175.curr= _temp1174; _temp1175.base=
_temp1174; _temp1175.last_plus_one= _temp1174 + 13; _temp1175;})};( struct
_xenum_struct*) _temp1173;})); _LL1166:;} struct Cyc_List_List* Cyc_yyget_DesignatorList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1176= yy1; struct Cyc_List_List*
_temp1182; _LL1178: if((*(( struct _xenum_struct*) _temp1176)).tag == Cyc_DesignatorList_tok_tag){
_LL1183: _temp1182=(( struct Cyc_DesignatorList_tok_struct*) _temp1176)->f1;
goto _LL1179;} else{ goto _LL1180;} _LL1180: goto _LL1181; _LL1179: return
_temp1182; _LL1181:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1184=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));*
_temp1184=( struct Cyc_Core_Failure_struct){.tag= Cyc_Core_Failure_tag,.f1=(
struct _tagged_string)({ char* _temp1185=( char*)"DesignatorList_tok"; struct
_tagged_string _temp1186; _temp1186.curr= _temp1185; _temp1186.base= _temp1185;
_temp1186.last_plus_one= _temp1185 + 19; _temp1186;})};( struct _xenum_struct*)
_temp1184;})); _LL1177:;} void* Cyc_yyget_Designator_tok( struct _xenum_struct*
yy1){ struct _xenum_struct* _temp1187= yy1; void* _temp1193; _LL1189: if((*((
struct _xenum_struct*) _temp1187)).tag == Cyc_Designator_tok_tag){ _LL1194:
_temp1193=(( struct Cyc_Designator_tok_struct*) _temp1187)->f1; goto _LL1190;}
else{ goto _LL1191;} _LL1191: goto _LL1192; _LL1190: return _temp1193; _LL1192:(
void) _throw(({ struct Cyc_Core_Failure_struct* _temp1195=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp1195=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp1196=( char*)"Designator_tok";
struct _tagged_string _temp1197; _temp1197.curr= _temp1196; _temp1197.base=
_temp1196; _temp1197.last_plus_one= _temp1196 + 15; _temp1197;})};( struct
_xenum_struct*) _temp1195;})); _LL1188:;} void* Cyc_yyget_Kind_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp1198= yy1; void* _temp1204;
_LL1200: if((*(( struct _xenum_struct*) _temp1198)).tag == Cyc_Kind_tok_tag){
_LL1205: _temp1204=(( struct Cyc_Kind_tok_struct*) _temp1198)->f1; goto _LL1201;}
else{ goto _LL1202;} _LL1202: goto _LL1203; _LL1201: return _temp1204; _LL1203:(
void) _throw(({ struct Cyc_Core_Failure_struct* _temp1206=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp1206=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp1207=( char*)"Kind_tok";
struct _tagged_string _temp1208; _temp1208.curr= _temp1207; _temp1208.base=
_temp1207; _temp1208.last_plus_one= _temp1207 + 9; _temp1208;})};( struct
_xenum_struct*) _temp1206;})); _LL1199:;} void* Cyc_yyget_Type_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp1209= yy1; void* _temp1215;
_LL1211: if((*(( struct _xenum_struct*) _temp1209)).tag == Cyc_Type_tok_tag){
_LL1216: _temp1215=(( struct Cyc_Type_tok_struct*) _temp1209)->f1; goto _LL1212;}
else{ goto _LL1213;} _LL1213: goto _LL1214; _LL1212: return _temp1215; _LL1214:(
void) _throw(({ struct Cyc_Core_Failure_struct* _temp1217=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp1217=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp1218=( char*)"Type_tok";
struct _tagged_string _temp1219; _temp1219.curr= _temp1218; _temp1219.base=
_temp1218; _temp1219.last_plus_one= _temp1218 + 9; _temp1219;})};( struct
_xenum_struct*) _temp1217;})); _LL1210:;} struct Cyc_List_List* Cyc_yyget_AttributeList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1220= yy1; struct Cyc_List_List*
_temp1226; _LL1222: if((*(( struct _xenum_struct*) _temp1220)).tag == Cyc_AttributeList_tok_tag){
_LL1227: _temp1226=(( struct Cyc_AttributeList_tok_struct*) _temp1220)->f1; goto
_LL1223;} else{ goto _LL1224;} _LL1224: goto _LL1225; _LL1223: return _temp1226;
_LL1225:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1228=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp1228=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp1229=( char*)"AttributeList_tok";
struct _tagged_string _temp1230; _temp1230.curr= _temp1229; _temp1230.base=
_temp1229; _temp1230.last_plus_one= _temp1229 + 18; _temp1230;})};( struct
_xenum_struct*) _temp1228;})); _LL1221:;} void* Cyc_yyget_Attribute_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp1231= yy1; void* _temp1237;
_LL1233: if((*(( struct _xenum_struct*) _temp1231)).tag == Cyc_Attribute_tok_tag){
_LL1238: _temp1237=(( struct Cyc_Attribute_tok_struct*) _temp1231)->f1; goto
_LL1234;} else{ goto _LL1235;} _LL1235: goto _LL1236; _LL1234: return _temp1237;
_LL1236:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1239=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp1239=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp1240=( char*)"Attribute_tok";
struct _tagged_string _temp1241; _temp1241.curr= _temp1240; _temp1241.base=
_temp1240; _temp1241.last_plus_one= _temp1240 + 14; _temp1241;})};( struct
_xenum_struct*) _temp1239;})); _LL1232:;} struct Cyc_Yyltype{ int timestamp; int
first_line; int first_column; int last_line; int last_column; struct
_tagged_string text; } ; typedef struct Cyc_Yyltype Cyc_yyltype; struct Cyc_Yyltype
Cyc_yynewloc(){ return({ struct Cyc_Yyltype _temp1242; _temp1242.timestamp= 0;
_temp1242.first_line= 0; _temp1242.first_column= 0; _temp1242.last_line= 0;
_temp1242.last_column= 0; _temp1242.text=( struct _tagged_string)({ char*
_temp1243=( char*)""; struct _tagged_string _temp1244; _temp1244.curr= _temp1243;
_temp1244.base= _temp1243; _temp1244.last_plus_one= _temp1243 + 1; _temp1244;});
_temp1242;});} static char _temp1247[ 1u]=""; struct Cyc_Yyltype Cyc_yylloc=(
struct Cyc_Yyltype){.timestamp= 0,.first_line= 0,.first_column= 0,.last_line= 0,.last_column=
0,.text=( struct _tagged_string){ _temp1247, _temp1247, _temp1247 + 1u}}; static
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
_temp1250[ 2u]="$"; static char _temp1253[ 6u]="error"; static char _temp1256[
12u]="$undefined."; static char _temp1259[ 5u]="AUTO"; static char _temp1262[ 9u]="REGISTER";
static char _temp1265[ 7u]="STATIC"; static char _temp1268[ 7u]="EXTERN"; static
char _temp1271[ 8u]="TYPEDEF"; static char _temp1274[ 5u]="VOID"; static char
_temp1277[ 5u]="CHAR"; static char _temp1280[ 6u]="SHORT"; static char _temp1283[
4u]="INT"; static char _temp1286[ 5u]="LONG"; static char _temp1289[ 6u]="FLOAT";
static char _temp1292[ 7u]="DOUBLE"; static char _temp1295[ 7u]="SIGNED"; static
char _temp1298[ 9u]="UNSIGNED"; static char _temp1301[ 6u]="CONST"; static char
_temp1304[ 9u]="VOLATILE"; static char _temp1307[ 9u]="RESTRICT"; static char
_temp1310[ 7u]="STRUCT"; static char _temp1313[ 6u]="UNION"; static char
_temp1316[ 5u]="CASE"; static char _temp1319[ 8u]="DEFAULT"; static char
_temp1322[ 7u]="INLINE"; static char _temp1325[ 3u]="IF"; static char _temp1328[
5u]="ELSE"; static char _temp1331[ 7u]="SWITCH"; static char _temp1334[ 6u]="WHILE";
static char _temp1337[ 3u]="DO"; static char _temp1340[ 4u]="FOR"; static char
_temp1343[ 5u]="GOTO"; static char _temp1346[ 9u]="CONTINUE"; static char
_temp1349[ 6u]="BREAK"; static char _temp1352[ 7u]="RETURN"; static char
_temp1355[ 7u]="SIZEOF"; static char _temp1358[ 5u]="ENUM"; static char
_temp1361[ 8u]="NULL_kw"; static char _temp1364[ 4u]="LET"; static char
_temp1367[ 6u]="THROW"; static char _temp1370[ 4u]="TRY"; static char _temp1373[
6u]="CATCH"; static char _temp1376[ 4u]="NEW"; static char _temp1379[ 9u]="ABSTRACT";
static char _temp1382[ 9u]="FALLTHRU"; static char _temp1385[ 6u]="USING";
static char _temp1388[ 10u]="NAMESPACE"; static char _temp1391[ 6u]="XENUM";
static char _temp1394[ 5u]="FILL"; static char _temp1397[ 8u]="CODEGEN"; static
char _temp1400[ 4u]="CUT"; static char _temp1403[ 7u]="SPLICE"; static char
_temp1406[ 7u]="PRINTF"; static char _temp1409[ 8u]="FPRINTF"; static char
_temp1412[ 8u]="XPRINTF"; static char _temp1415[ 6u]="SCANF"; static char
_temp1418[ 7u]="FSCANF"; static char _temp1421[ 7u]="SSCANF"; static char
_temp1424[ 7u]="MALLOC"; static char _temp1427[ 7u]="PTR_OP"; static char
_temp1430[ 7u]="INC_OP"; static char _temp1433[ 7u]="DEC_OP"; static char
_temp1436[ 8u]="LEFT_OP"; static char _temp1439[ 9u]="RIGHT_OP"; static char
_temp1442[ 6u]="LE_OP"; static char _temp1445[ 6u]="GE_OP"; static char
_temp1448[ 6u]="EQ_OP"; static char _temp1451[ 6u]="NE_OP"; static char
_temp1454[ 7u]="AND_OP"; static char _temp1457[ 6u]="OR_OP"; static char
_temp1460[ 11u]="MUL_ASSIGN"; static char _temp1463[ 11u]="DIV_ASSIGN"; static
char _temp1466[ 11u]="MOD_ASSIGN"; static char _temp1469[ 11u]="ADD_ASSIGN";
static char _temp1472[ 11u]="SUB_ASSIGN"; static char _temp1475[ 12u]="LEFT_ASSIGN";
static char _temp1478[ 13u]="RIGHT_ASSIGN"; static char _temp1481[ 11u]="AND_ASSIGN";
static char _temp1484[ 11u]="XOR_ASSIGN"; static char _temp1487[ 10u]="OR_ASSIGN";
static char _temp1490[ 9u]="ELLIPSIS"; static char _temp1493[ 11u]="LEFT_RIGHT";
static char _temp1496[ 12u]="COLON_COLON"; static char _temp1499[ 11u]="IDENTIFIER";
static char _temp1502[ 17u]="INTEGER_CONSTANT"; static char _temp1505[ 7u]="STRING";
static char _temp1508[ 19u]="CHARACTER_CONSTANT"; static char _temp1511[ 18u]="FLOATING_CONSTANT";
static char _temp1514[ 9u]="TYPE_VAR"; static char _temp1517[ 16u]="QUAL_IDENTIFIER";
static char _temp1520[ 18u]="QUAL_TYPEDEF_NAME"; static char _temp1523[ 10u]="ATTRIBUTE";
static char _temp1526[ 4u]="';'"; static char _temp1529[ 4u]="'{'"; static char
_temp1532[ 4u]="'}'"; static char _temp1535[ 4u]="'='"; static char _temp1538[ 4u]="'('";
static char _temp1541[ 4u]="')'"; static char _temp1544[ 4u]="','"; static char
_temp1547[ 4u]="'_'"; static char _temp1550[ 4u]="'$'"; static char _temp1553[ 4u]="'<'";
static char _temp1556[ 4u]="'>'"; static char _temp1559[ 4u]="':'"; static char
_temp1562[ 4u]="'['"; static char _temp1565[ 4u]="']'"; static char _temp1568[ 4u]="'*'";
static char _temp1571[ 4u]="'@'"; static char _temp1574[ 4u]="'?'"; static char
_temp1577[ 4u]="'+'"; static char _temp1580[ 4u]="'.'"; static char _temp1583[ 4u]="'-'";
static char _temp1586[ 4u]="'&'"; static char _temp1589[ 4u]="'|'"; static char
_temp1592[ 4u]="'^'"; static char _temp1595[ 4u]="'/'"; static char _temp1598[ 4u]="'%'";
static char _temp1601[ 4u]="'~'"; static char _temp1604[ 4u]="'!'"; static char
_temp1607[ 5u]="prog"; static char _temp1610[ 17u]="translation_unit"; static
char _temp1613[ 21u]="translation_unit_opt"; static char _temp1616[ 21u]="external_declaration";
static char _temp1619[ 20u]="function_definition"; static char _temp1622[ 21u]="function_definition2";
static char _temp1625[ 13u]="using_action"; static char _temp1628[ 15u]="unusing_action";
static char _temp1631[ 17u]="namespace_action"; static char _temp1634[ 19u]="unnamespace_action";
static char _temp1637[ 12u]="declaration"; static char _temp1640[ 17u]="declaration_list";
static char _temp1643[ 23u]="declaration_specifiers"; static char _temp1646[ 24u]="storage_class_specifier";
static char _temp1649[ 15u]="attributes_opt"; static char _temp1652[ 11u]="attributes";
static char _temp1655[ 15u]="attribute_list"; static char _temp1658[ 10u]="attribute";
static char _temp1661[ 15u]="type_specifier"; static char _temp1664[ 5u]="kind";
static char _temp1667[ 15u]="type_qualifier"; static char _temp1670[ 26u]="struct_or_union_specifier";
static char _temp1673[ 16u]="type_params_opt"; static char _temp1676[ 16u]="struct_or_union";
static char _temp1679[ 24u]="struct_declaration_list"; static char _temp1682[ 25u]="struct_declaration_list0";
static char _temp1685[ 21u]="init_declarator_list"; static char _temp1688[ 22u]="init_declarator_list0";
static char _temp1691[ 16u]="init_declarator"; static char _temp1694[ 19u]="struct_declaration";
static char _temp1697[ 25u]="specifier_qualifier_list"; static char _temp1700[
23u]="struct_declarator_list"; static char _temp1703[ 24u]="struct_declarator_list0";
static char _temp1706[ 18u]="struct_declarator"; static char _temp1709[ 15u]="enum_specifier";
static char _temp1712[ 16u]="enumerator_list"; static char _temp1715[ 11u]="enumerator";
static char _temp1718[ 11u]="declarator"; static char _temp1721[ 18u]="direct_declarator";
static char _temp1724[ 8u]="pointer"; static char _temp1727[ 13u]="pointer_char";
static char _temp1730[ 8u]="rgn_opt"; static char _temp1733[ 20u]="type_qualifier_list";
static char _temp1736[ 20u]="parameter_type_list"; static char _temp1739[ 11u]="effect_set";
static char _temp1742[ 14u]="atomic_effect"; static char _temp1745[ 11u]="region_set";
static char _temp1748[ 15u]="parameter_list"; static char _temp1751[ 22u]="parameter_declaration";
static char _temp1754[ 16u]="identifier_list"; static char _temp1757[ 17u]="identifier_list0";
static char _temp1760[ 12u]="initializer"; static char _temp1763[ 17u]="initializer_list";
static char _temp1766[ 12u]="designation"; static char _temp1769[ 16u]="designator_list";
static char _temp1772[ 11u]="designator"; static char _temp1775[ 10u]="type_name";
static char _temp1778[ 14u]="any_type_name"; static char _temp1781[ 15u]="type_name_list";
static char _temp1784[ 20u]="abstract_declarator"; static char _temp1787[ 27u]="direct_abstract_declarator";
static char _temp1790[ 10u]="statement"; static char _temp1793[ 18u]="labeled_statement";
static char _temp1796[ 21u]="expression_statement"; static char _temp1799[ 19u]="compound_statement";
static char _temp1802[ 16u]="block_item_list"; static char _temp1805[ 11u]="block_item";
static char _temp1808[ 20u]="selection_statement"; static char _temp1811[ 15u]="switch_clauses";
static char _temp1814[ 20u]="iteration_statement"; static char _temp1817[ 15u]="jump_statement";
static char _temp1820[ 8u]="pattern"; static char _temp1823[ 19u]="tuple_pattern_list";
static char _temp1826[ 20u]="tuple_pattern_list0"; static char _temp1829[ 14u]="field_pattern";
static char _temp1832[ 19u]="field_pattern_list"; static char _temp1835[ 20u]="field_pattern_list0";
static char _temp1838[ 11u]="expression"; static char _temp1841[ 22u]="assignment_expression";
static char _temp1844[ 20u]="assignment_operator"; static char _temp1847[ 23u]="conditional_expression";
static char _temp1850[ 20u]="constant_expression"; static char _temp1853[ 22u]="logical_or_expression";
static char _temp1856[ 23u]="logical_and_expression"; static char _temp1859[ 24u]="inclusive_or_expression";
static char _temp1862[ 24u]="exclusive_or_expression"; static char _temp1865[ 15u]="and_expression";
static char _temp1868[ 20u]="equality_expression"; static char _temp1871[ 22u]="relational_expression";
static char _temp1874[ 17u]="shift_expression"; static char _temp1877[ 20u]="additive_expression";
static char _temp1880[ 26u]="multiplicative_expression"; static char _temp1883[
16u]="cast_expression"; static char _temp1886[ 17u]="unary_expression"; static
char _temp1889[ 14u]="format_primop"; static char _temp1892[ 15u]="unary_operator";
static char _temp1895[ 19u]="postfix_expression"; static char _temp1898[ 19u]="primary_expression";
static char _temp1901[ 25u]="argument_expression_list"; static char _temp1904[
26u]="argument_expression_list0"; static char _temp1907[ 9u]="constant"; static
char _temp1910[ 20u]="qual_opt_identifier"; static struct _tagged_string Cyc_yytname[
221u]={( struct _tagged_string){ _temp1250, _temp1250, _temp1250 + 2u},( struct
_tagged_string){ _temp1253, _temp1253, _temp1253 + 6u},( struct _tagged_string){
_temp1256, _temp1256, _temp1256 + 12u},( struct _tagged_string){ _temp1259,
_temp1259, _temp1259 + 5u},( struct _tagged_string){ _temp1262, _temp1262,
_temp1262 + 9u},( struct _tagged_string){ _temp1265, _temp1265, _temp1265 + 7u},(
struct _tagged_string){ _temp1268, _temp1268, _temp1268 + 7u},( struct
_tagged_string){ _temp1271, _temp1271, _temp1271 + 8u},( struct _tagged_string){
_temp1274, _temp1274, _temp1274 + 5u},( struct _tagged_string){ _temp1277,
_temp1277, _temp1277 + 5u},( struct _tagged_string){ _temp1280, _temp1280,
_temp1280 + 6u},( struct _tagged_string){ _temp1283, _temp1283, _temp1283 + 4u},(
struct _tagged_string){ _temp1286, _temp1286, _temp1286 + 5u},( struct
_tagged_string){ _temp1289, _temp1289, _temp1289 + 6u},( struct _tagged_string){
_temp1292, _temp1292, _temp1292 + 7u},( struct _tagged_string){ _temp1295,
_temp1295, _temp1295 + 7u},( struct _tagged_string){ _temp1298, _temp1298,
_temp1298 + 9u},( struct _tagged_string){ _temp1301, _temp1301, _temp1301 + 6u},(
struct _tagged_string){ _temp1304, _temp1304, _temp1304 + 9u},( struct
_tagged_string){ _temp1307, _temp1307, _temp1307 + 9u},( struct _tagged_string){
_temp1310, _temp1310, _temp1310 + 7u},( struct _tagged_string){ _temp1313,
_temp1313, _temp1313 + 6u},( struct _tagged_string){ _temp1316, _temp1316,
_temp1316 + 5u},( struct _tagged_string){ _temp1319, _temp1319, _temp1319 + 8u},(
struct _tagged_string){ _temp1322, _temp1322, _temp1322 + 7u},( struct
_tagged_string){ _temp1325, _temp1325, _temp1325 + 3u},( struct _tagged_string){
_temp1328, _temp1328, _temp1328 + 5u},( struct _tagged_string){ _temp1331,
_temp1331, _temp1331 + 7u},( struct _tagged_string){ _temp1334, _temp1334,
_temp1334 + 6u},( struct _tagged_string){ _temp1337, _temp1337, _temp1337 + 3u},(
struct _tagged_string){ _temp1340, _temp1340, _temp1340 + 4u},( struct
_tagged_string){ _temp1343, _temp1343, _temp1343 + 5u},( struct _tagged_string){
_temp1346, _temp1346, _temp1346 + 9u},( struct _tagged_string){ _temp1349,
_temp1349, _temp1349 + 6u},( struct _tagged_string){ _temp1352, _temp1352,
_temp1352 + 7u},( struct _tagged_string){ _temp1355, _temp1355, _temp1355 + 7u},(
struct _tagged_string){ _temp1358, _temp1358, _temp1358 + 5u},( struct
_tagged_string){ _temp1361, _temp1361, _temp1361 + 8u},( struct _tagged_string){
_temp1364, _temp1364, _temp1364 + 4u},( struct _tagged_string){ _temp1367,
_temp1367, _temp1367 + 6u},( struct _tagged_string){ _temp1370, _temp1370,
_temp1370 + 4u},( struct _tagged_string){ _temp1373, _temp1373, _temp1373 + 6u},(
struct _tagged_string){ _temp1376, _temp1376, _temp1376 + 4u},( struct
_tagged_string){ _temp1379, _temp1379, _temp1379 + 9u},( struct _tagged_string){
_temp1382, _temp1382, _temp1382 + 9u},( struct _tagged_string){ _temp1385,
_temp1385, _temp1385 + 6u},( struct _tagged_string){ _temp1388, _temp1388,
_temp1388 + 10u},( struct _tagged_string){ _temp1391, _temp1391, _temp1391 + 6u},(
struct _tagged_string){ _temp1394, _temp1394, _temp1394 + 5u},( struct
_tagged_string){ _temp1397, _temp1397, _temp1397 + 8u},( struct _tagged_string){
_temp1400, _temp1400, _temp1400 + 4u},( struct _tagged_string){ _temp1403,
_temp1403, _temp1403 + 7u},( struct _tagged_string){ _temp1406, _temp1406,
_temp1406 + 7u},( struct _tagged_string){ _temp1409, _temp1409, _temp1409 + 8u},(
struct _tagged_string){ _temp1412, _temp1412, _temp1412 + 8u},( struct
_tagged_string){ _temp1415, _temp1415, _temp1415 + 6u},( struct _tagged_string){
_temp1418, _temp1418, _temp1418 + 7u},( struct _tagged_string){ _temp1421,
_temp1421, _temp1421 + 7u},( struct _tagged_string){ _temp1424, _temp1424,
_temp1424 + 7u},( struct _tagged_string){ _temp1427, _temp1427, _temp1427 + 7u},(
struct _tagged_string){ _temp1430, _temp1430, _temp1430 + 7u},( struct
_tagged_string){ _temp1433, _temp1433, _temp1433 + 7u},( struct _tagged_string){
_temp1436, _temp1436, _temp1436 + 8u},( struct _tagged_string){ _temp1439,
_temp1439, _temp1439 + 9u},( struct _tagged_string){ _temp1442, _temp1442,
_temp1442 + 6u},( struct _tagged_string){ _temp1445, _temp1445, _temp1445 + 6u},(
struct _tagged_string){ _temp1448, _temp1448, _temp1448 + 6u},( struct
_tagged_string){ _temp1451, _temp1451, _temp1451 + 6u},( struct _tagged_string){
_temp1454, _temp1454, _temp1454 + 7u},( struct _tagged_string){ _temp1457,
_temp1457, _temp1457 + 6u},( struct _tagged_string){ _temp1460, _temp1460,
_temp1460 + 11u},( struct _tagged_string){ _temp1463, _temp1463, _temp1463 + 11u},(
struct _tagged_string){ _temp1466, _temp1466, _temp1466 + 11u},( struct
_tagged_string){ _temp1469, _temp1469, _temp1469 + 11u},( struct _tagged_string){
_temp1472, _temp1472, _temp1472 + 11u},( struct _tagged_string){ _temp1475,
_temp1475, _temp1475 + 12u},( struct _tagged_string){ _temp1478, _temp1478,
_temp1478 + 13u},( struct _tagged_string){ _temp1481, _temp1481, _temp1481 + 11u},(
struct _tagged_string){ _temp1484, _temp1484, _temp1484 + 11u},( struct
_tagged_string){ _temp1487, _temp1487, _temp1487 + 10u},( struct _tagged_string){
_temp1490, _temp1490, _temp1490 + 9u},( struct _tagged_string){ _temp1493,
_temp1493, _temp1493 + 11u},( struct _tagged_string){ _temp1496, _temp1496,
_temp1496 + 12u},( struct _tagged_string){ _temp1499, _temp1499, _temp1499 + 11u},(
struct _tagged_string){ _temp1502, _temp1502, _temp1502 + 17u},( struct
_tagged_string){ _temp1505, _temp1505, _temp1505 + 7u},( struct _tagged_string){
_temp1508, _temp1508, _temp1508 + 19u},( struct _tagged_string){ _temp1511,
_temp1511, _temp1511 + 18u},( struct _tagged_string){ _temp1514, _temp1514,
_temp1514 + 9u},( struct _tagged_string){ _temp1517, _temp1517, _temp1517 + 16u},(
struct _tagged_string){ _temp1520, _temp1520, _temp1520 + 18u},( struct
_tagged_string){ _temp1523, _temp1523, _temp1523 + 10u},( struct _tagged_string){
_temp1526, _temp1526, _temp1526 + 4u},( struct _tagged_string){ _temp1529,
_temp1529, _temp1529 + 4u},( struct _tagged_string){ _temp1532, _temp1532,
_temp1532 + 4u},( struct _tagged_string){ _temp1535, _temp1535, _temp1535 + 4u},(
struct _tagged_string){ _temp1538, _temp1538, _temp1538 + 4u},( struct
_tagged_string){ _temp1541, _temp1541, _temp1541 + 4u},( struct _tagged_string){
_temp1544, _temp1544, _temp1544 + 4u},( struct _tagged_string){ _temp1547,
_temp1547, _temp1547 + 4u},( struct _tagged_string){ _temp1550, _temp1550,
_temp1550 + 4u},( struct _tagged_string){ _temp1553, _temp1553, _temp1553 + 4u},(
struct _tagged_string){ _temp1556, _temp1556, _temp1556 + 4u},( struct
_tagged_string){ _temp1559, _temp1559, _temp1559 + 4u},( struct _tagged_string){
_temp1562, _temp1562, _temp1562 + 4u},( struct _tagged_string){ _temp1565,
_temp1565, _temp1565 + 4u},( struct _tagged_string){ _temp1568, _temp1568,
_temp1568 + 4u},( struct _tagged_string){ _temp1571, _temp1571, _temp1571 + 4u},(
struct _tagged_string){ _temp1574, _temp1574, _temp1574 + 4u},( struct
_tagged_string){ _temp1577, _temp1577, _temp1577 + 4u},( struct _tagged_string){
_temp1580, _temp1580, _temp1580 + 4u},( struct _tagged_string){ _temp1583,
_temp1583, _temp1583 + 4u},( struct _tagged_string){ _temp1586, _temp1586,
_temp1586 + 4u},( struct _tagged_string){ _temp1589, _temp1589, _temp1589 + 4u},(
struct _tagged_string){ _temp1592, _temp1592, _temp1592 + 4u},( struct
_tagged_string){ _temp1595, _temp1595, _temp1595 + 4u},( struct _tagged_string){
_temp1598, _temp1598, _temp1598 + 4u},( struct _tagged_string){ _temp1601,
_temp1601, _temp1601 + 4u},( struct _tagged_string){ _temp1604, _temp1604,
_temp1604 + 4u},( struct _tagged_string){ _temp1607, _temp1607, _temp1607 + 5u},(
struct _tagged_string){ _temp1610, _temp1610, _temp1610 + 17u},( struct
_tagged_string){ _temp1613, _temp1613, _temp1613 + 21u},( struct _tagged_string){
_temp1616, _temp1616, _temp1616 + 21u},( struct _tagged_string){ _temp1619,
_temp1619, _temp1619 + 20u},( struct _tagged_string){ _temp1622, _temp1622,
_temp1622 + 21u},( struct _tagged_string){ _temp1625, _temp1625, _temp1625 + 13u},(
struct _tagged_string){ _temp1628, _temp1628, _temp1628 + 15u},( struct
_tagged_string){ _temp1631, _temp1631, _temp1631 + 17u},( struct _tagged_string){
_temp1634, _temp1634, _temp1634 + 19u},( struct _tagged_string){ _temp1637,
_temp1637, _temp1637 + 12u},( struct _tagged_string){ _temp1640, _temp1640,
_temp1640 + 17u},( struct _tagged_string){ _temp1643, _temp1643, _temp1643 + 23u},(
struct _tagged_string){ _temp1646, _temp1646, _temp1646 + 24u},( struct
_tagged_string){ _temp1649, _temp1649, _temp1649 + 15u},( struct _tagged_string){
_temp1652, _temp1652, _temp1652 + 11u},( struct _tagged_string){ _temp1655,
_temp1655, _temp1655 + 15u},( struct _tagged_string){ _temp1658, _temp1658,
_temp1658 + 10u},( struct _tagged_string){ _temp1661, _temp1661, _temp1661 + 15u},(
struct _tagged_string){ _temp1664, _temp1664, _temp1664 + 5u},( struct
_tagged_string){ _temp1667, _temp1667, _temp1667 + 15u},( struct _tagged_string){
_temp1670, _temp1670, _temp1670 + 26u},( struct _tagged_string){ _temp1673,
_temp1673, _temp1673 + 16u},( struct _tagged_string){ _temp1676, _temp1676,
_temp1676 + 16u},( struct _tagged_string){ _temp1679, _temp1679, _temp1679 + 24u},(
struct _tagged_string){ _temp1682, _temp1682, _temp1682 + 25u},( struct
_tagged_string){ _temp1685, _temp1685, _temp1685 + 21u},( struct _tagged_string){
_temp1688, _temp1688, _temp1688 + 22u},( struct _tagged_string){ _temp1691,
_temp1691, _temp1691 + 16u},( struct _tagged_string){ _temp1694, _temp1694,
_temp1694 + 19u},( struct _tagged_string){ _temp1697, _temp1697, _temp1697 + 25u},(
struct _tagged_string){ _temp1700, _temp1700, _temp1700 + 23u},( struct
_tagged_string){ _temp1703, _temp1703, _temp1703 + 24u},( struct _tagged_string){
_temp1706, _temp1706, _temp1706 + 18u},( struct _tagged_string){ _temp1709,
_temp1709, _temp1709 + 15u},( struct _tagged_string){ _temp1712, _temp1712,
_temp1712 + 16u},( struct _tagged_string){ _temp1715, _temp1715, _temp1715 + 11u},(
struct _tagged_string){ _temp1718, _temp1718, _temp1718 + 11u},( struct
_tagged_string){ _temp1721, _temp1721, _temp1721 + 18u},( struct _tagged_string){
_temp1724, _temp1724, _temp1724 + 8u},( struct _tagged_string){ _temp1727,
_temp1727, _temp1727 + 13u},( struct _tagged_string){ _temp1730, _temp1730,
_temp1730 + 8u},( struct _tagged_string){ _temp1733, _temp1733, _temp1733 + 20u},(
struct _tagged_string){ _temp1736, _temp1736, _temp1736 + 20u},( struct
_tagged_string){ _temp1739, _temp1739, _temp1739 + 11u},( struct _tagged_string){
_temp1742, _temp1742, _temp1742 + 14u},( struct _tagged_string){ _temp1745,
_temp1745, _temp1745 + 11u},( struct _tagged_string){ _temp1748, _temp1748,
_temp1748 + 15u},( struct _tagged_string){ _temp1751, _temp1751, _temp1751 + 22u},(
struct _tagged_string){ _temp1754, _temp1754, _temp1754 + 16u},( struct
_tagged_string){ _temp1757, _temp1757, _temp1757 + 17u},( struct _tagged_string){
_temp1760, _temp1760, _temp1760 + 12u},( struct _tagged_string){ _temp1763,
_temp1763, _temp1763 + 17u},( struct _tagged_string){ _temp1766, _temp1766,
_temp1766 + 12u},( struct _tagged_string){ _temp1769, _temp1769, _temp1769 + 16u},(
struct _tagged_string){ _temp1772, _temp1772, _temp1772 + 11u},( struct
_tagged_string){ _temp1775, _temp1775, _temp1775 + 10u},( struct _tagged_string){
_temp1778, _temp1778, _temp1778 + 14u},( struct _tagged_string){ _temp1781,
_temp1781, _temp1781 + 15u},( struct _tagged_string){ _temp1784, _temp1784,
_temp1784 + 20u},( struct _tagged_string){ _temp1787, _temp1787, _temp1787 + 27u},(
struct _tagged_string){ _temp1790, _temp1790, _temp1790 + 10u},( struct
_tagged_string){ _temp1793, _temp1793, _temp1793 + 18u},( struct _tagged_string){
_temp1796, _temp1796, _temp1796 + 21u},( struct _tagged_string){ _temp1799,
_temp1799, _temp1799 + 19u},( struct _tagged_string){ _temp1802, _temp1802,
_temp1802 + 16u},( struct _tagged_string){ _temp1805, _temp1805, _temp1805 + 11u},(
struct _tagged_string){ _temp1808, _temp1808, _temp1808 + 20u},( struct
_tagged_string){ _temp1811, _temp1811, _temp1811 + 15u},( struct _tagged_string){
_temp1814, _temp1814, _temp1814 + 20u},( struct _tagged_string){ _temp1817,
_temp1817, _temp1817 + 15u},( struct _tagged_string){ _temp1820, _temp1820,
_temp1820 + 8u},( struct _tagged_string){ _temp1823, _temp1823, _temp1823 + 19u},(
struct _tagged_string){ _temp1826, _temp1826, _temp1826 + 20u},( struct
_tagged_string){ _temp1829, _temp1829, _temp1829 + 14u},( struct _tagged_string){
_temp1832, _temp1832, _temp1832 + 19u},( struct _tagged_string){ _temp1835,
_temp1835, _temp1835 + 20u},( struct _tagged_string){ _temp1838, _temp1838,
_temp1838 + 11u},( struct _tagged_string){ _temp1841, _temp1841, _temp1841 + 22u},(
struct _tagged_string){ _temp1844, _temp1844, _temp1844 + 20u},( struct
_tagged_string){ _temp1847, _temp1847, _temp1847 + 23u},( struct _tagged_string){
_temp1850, _temp1850, _temp1850 + 20u},( struct _tagged_string){ _temp1853,
_temp1853, _temp1853 + 22u},( struct _tagged_string){ _temp1856, _temp1856,
_temp1856 + 23u},( struct _tagged_string){ _temp1859, _temp1859, _temp1859 + 24u},(
struct _tagged_string){ _temp1862, _temp1862, _temp1862 + 24u},( struct
_tagged_string){ _temp1865, _temp1865, _temp1865 + 15u},( struct _tagged_string){
_temp1868, _temp1868, _temp1868 + 20u},( struct _tagged_string){ _temp1871,
_temp1871, _temp1871 + 22u},( struct _tagged_string){ _temp1874, _temp1874,
_temp1874 + 17u},( struct _tagged_string){ _temp1877, _temp1877, _temp1877 + 20u},(
struct _tagged_string){ _temp1880, _temp1880, _temp1880 + 26u},( struct
_tagged_string){ _temp1883, _temp1883, _temp1883 + 16u},( struct _tagged_string){
_temp1886, _temp1886, _temp1886 + 17u},( struct _tagged_string){ _temp1889,
_temp1889, _temp1889 + 14u},( struct _tagged_string){ _temp1892, _temp1892,
_temp1892 + 15u},( struct _tagged_string){ _temp1895, _temp1895, _temp1895 + 19u},(
struct _tagged_string){ _temp1898, _temp1898, _temp1898 + 19u},( struct
_tagged_string){ _temp1901, _temp1901, _temp1901 + 25u},( struct _tagged_string){
_temp1904, _temp1904, _temp1904 + 26u},( struct _tagged_string){ _temp1907,
_temp1907, _temp1907 + 9u},( struct _tagged_string){ _temp1910, _temp1910,
_temp1910 + 20u}}; static short Cyc_yyr1[ 381u]={ (short)0, (short)119, (short)120,
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
(short)2, (short)1, (short)1, (short)3, (short)1, (short)3, (short)3, (short)1,
(short)2, (short)1, (short)2, (short)1, (short)1, (short)3, (short)1, (short)2,
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
static short Cyc_yydefact[ 715u]={ (short)0, (short)36, (short)37, (short)38,
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
(short)0, (short)0, (short)68, (short)69, (short)65, (short)0, (short)92,
(short)94, (short)176, (short)178, (short)182, (short)0, (short)117, (short)158,
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
(short)0, (short)93, (short)95, (short)0, (short)0, (short)184, (short)177,
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
(short)180, (short)0, (short)192, (short)0, (short)0, (short)188, (short)0,
(short)186, (short)0, (short)0, (short)0, (short)200, (short)181, (short)183,
(short)156, (short)0, (short)0, (short)0, (short)356, (short)167, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)366,
(short)373, (short)0, (short)372, (short)293, (short)0, (short)295, (short)297,
(short)299, (short)301, (short)303, (short)304, (short)308, (short)309, (short)306,
(short)307, (short)311, (short)312, (short)314, (short)315, (short)317, (short)318,
(short)319, (short)277, (short)0, (short)350, (short)351, (short)346, (short)0,
(short)0, (short)348, (short)349, (short)0, (short)0, (short)10, (short)5,
(short)7, (short)164, (short)0, (short)0, (short)100, (short)91, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)243, (short)247, (short)0, (short)0, (short)0, (short)209,
(short)0, (short)17, (short)275, (short)0, (short)147, (short)0, (short)122,
(short)0, (short)144, (short)143, (short)162, (short)8, (short)0, (short)0,
(short)0, (short)0, (short)103, (short)268, (short)0, (short)175, (short)270,
(short)171, (short)173, (short)262, (short)0, (short)259, (short)153, (short)152,
(short)0, (short)194, (short)187, (short)190, (short)0, (short)195, (short)0,
(short)0, (short)189, (short)0, (short)329, (short)0, (short)0, (short)357,
(short)0, (short)168, (short)361, (short)362, (short)0, (short)0, (short)0,
(short)0, (short)321, (short)369, (short)0, (short)0, (short)332, (short)347,
(short)345, (short)370, (short)0, (short)0, (short)165, (short)0, (short)75,
(short)98, (short)101, (short)74, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)223, (short)249,
(short)0, (short)18, (short)150, (short)148, (short)146, (short)0, (short)0,
(short)45, (short)47, (short)113, (short)174, (short)273, (short)0, (short)193,
(short)0, (short)197, (short)199, (short)198, (short)191, (short)0, (short)358,
(short)169, (short)0, (short)0, (short)0, (short)371, (short)0, (short)374,
(short)290, (short)368, (short)166, (short)219, (short)223, (short)229, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)250, (short)50, (short)51, (short)154,
(short)196, (short)0, (short)170, (short)0, (short)0, (short)354, (short)0,
(short)0, (short)0, (short)0, (short)231, (short)0, (short)0, (short)0, (short)239,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)222, (short)0, (short)333, (short)334, (short)355, (short)220, (short)221,
(short)230, (short)232, (short)233, (short)0, (short)241, (short)240, (short)0,
(short)235, (short)0, (short)0, (short)0, (short)0, (short)223, (short)224,
(short)0, (short)234, (short)242, (short)236, (short)237, (short)0, (short)0,
(short)223, (short)225, (short)359, (short)238, (short)223, (short)226, (short)223,
(short)227, (short)228, (short)0, (short)0, (short)0}; static short Cyc_yydefgoto[
102u]={ (short)712, (short)501, (short)502, (short)38, (short)39, (short)226,
(short)40, (short)357, (short)41, (short)359, (short)42, (short)197, (short)43,
(short)44, (short)57, (short)58, (short)414, (short)415, (short)45, (short)130,
(short)46, (short)47, (short)78, (short)48, (short)203, (short)204, (short)89,
(short)90, (short)91, (short)205, (short)139, (short)369, (short)370, (short)371,
(short)49, (short)116, (short)117, (short)50, (short)51, (short)52, (short)53,
(short)112, (short)256, (short)442, (short)398, (short)399, (short)275, (short)247,
(short)141, (short)248, (short)249, (short)458, (short)459, (short)460, (short)429,
(short)430, (short)135, (short)136, (short)137, (short)443, (short)282, (short)229,
(short)230, (short)231, (short)232, (short)233, (short)234, (short)235, (short)647,
(short)236, (short)237, (short)266, (short)267, (short)268, (short)432, (short)433,
(short)434, (short)238, (short)239, (short)343, (short)170, (short)419, (short)171,
(short)172, (short)173, (short)174, (short)175, (short)176, (short)177, (short)178,
(short)179, (short)180, (short)181, (short)182, (short)183, (short)184, (short)185,
(short)186, (short)469, (short)470, (short)187, (short)188}; static short Cyc_yypact[
715u]={ (short)1514, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)62, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)21, (short)180, (short)580, - (short)-32768, (short)142,
(short)4, (short)142, - (short)-32768, (short)22, - (short)-32768, (short)61,
(short)564, - (short)-32768, (short)99, (short)86, (short)191, - (short)-32768,
- (short)-32768, (short)1514, - (short)-32768, (short)122, (short)245, - (short)-32768,
(short)370, - (short)21, - (short)21, - (short)21, - (short)-32768, (short)233,
- (short)-32768, (short)1818, (short)197, (short)3, - (short)20, - (short)-32768,
(short)194, (short)198, (short)1867, - (short)-32768, (short)142, (short)61, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)580, -
(short)-32768, (short)208, (short)223, (short)225, (short)580, (short)240,
(short)148, - (short)-32768, - (short)-32768, (short)227, (short)107, (short)1917,
- (short)-32768, (short)221, (short)2054, (short)3995, (short)3995, - (short)-32768,
(short)1514, (short)1514, (short)1514, (short)1514, - (short)-32768, (short)255,
(short)260, - (short)-32768, (short)1769, (short)1867, (short)1867, (short)1867,
(short)61, (short)2054, (short)61, (short)277, (short)944, - (short)-32768,
(short)1818, (short)370, - (short)-32768, (short)1454, (short)1917, (short)2567,
- (short)-32768, (short)197, (short)293, - (short)-32768, (short)115, (short)1514,
(short)282, - (short)-32768, (short)291, (short)154, (short)275, (short)289,
(short)290, (short)580, - (short)-32768, - (short)-32768, - (short)-32768,
(short)3995, - (short)56, (short)142, - (short)-32768, - (short)-32768, -
(short)-32768, (short)201, (short)2054, (short)2054, (short)257, - (short)-32768,
(short)285, (short)27, - (short)-32768, (short)465, (short)303, - (short)-32768,
(short)4079, - (short)-32768, (short)4163, - (short)55, (short)309, (short)319,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, (short)321, (short)4163, (short)4163, - (short)-32768, -
(short)-32768, - (short)-32768, - (short)-32768, (short)1287, (short)324,
(short)3995, (short)3995, - (short)-32768, (short)3995, - (short)-32768, -
(short)-32768, (short)371, - (short)-32768, - (short)41, (short)392, (short)359,
(short)360, (short)376, (short)388, (short)110, (short)395, - (short)38, (short)215,
- (short)-32768, (short)429, (short)400, (short)3995, (short)82, - (short)-32768,
- (short)-32768, (short)126, (short)420, - (short)-32768, (short)426, - (short)-32768,
(short)442, - (short)-32768, (short)564, (short)2651, (short)1818, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, (short)389, (short)447,
(short)2054, - (short)-32768, (short)439, (short)434, - (short)-32768, (short)438,
(short)457, (short)481, (short)1943, (short)483, (short)506, (short)509, (short)516,
(short)2735, (short)1943, (short)68, (short)1943, (short)1943, (short)511, -
(short)-32768, - (short)-32768, (short)515, - (short)-32768, - (short)-32768,
(short)370, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
(short)522, (short)1060, - (short)-32768, - (short)-32768, - (short)-32768,
(short)241, - (short)-32768, - (short)-32768, - (short)-32768, (short)523, -
(short)-32768, (short)12, - (short)-32768, (short)530, (short)274, (short)532,
(short)521, (short)52, - (short)-32768, (short)518, (short)107, - (short)-32768,
- (short)-32768, (short)115, (short)539, (short)8, - (short)-32768, (short)142,
(short)142, (short)3995, (short)538, (short)142, - (short)-32768, - (short)-32768,
(short)542, (short)543, (short)281, (short)340, (short)580, (short)546, -
(short)16, - (short)-32768, (short)548, - (short)-32768, - (short)-32768,
(short)1663, (short)2819, (short)20, - (short)-32768, (short)211, (short)12, -
(short)-32768, (short)1917, - (short)-32768, (short)1617, - (short)-32768,
(short)76, - (short)-32768, - (short)-32768, (short)2054, (short)1287, - (short)-32768,
(short)1287, - (short)-32768, - (short)-32768, (short)2063, (short)3995, (short)1562,
(short)608, - (short)-32768, - (short)-32768, (short)1060, (short)549, (short)366,
(short)3995, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, (short)3995, (short)3995, (short)3995, (short)3995, (short)3995,
(short)3995, (short)3995, (short)3995, (short)3995, (short)3995, (short)3995,
(short)3995, (short)3995, (short)3995, (short)3995, (short)3995, (short)3995,
(short)3995, (short)3995, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, (short)3995, (short)3995, -
(short)-32768, (short)116, - (short)-32768, - (short)-32768, (short)2903,
(short)3995, (short)189, - (short)-32768, (short)2483, (short)544, - (short)-32768,
- (short)-32768, (short)1514, - (short)-32768, (short)1514, - (short)-32768,
(short)2147, - (short)-32768, - (short)-32768, - (short)-32768, (short)2054, -
(short)-32768, - (short)-32768, (short)3995, (short)557, (short)554, - (short)-32768,
(short)553, (short)2054, (short)3995, (short)3995, (short)3995, (short)630,
(short)1176, (short)567, - (short)-32768, - (short)-32768, - (short)-32768,
(short)288, (short)621, - (short)-32768, (short)2987, - (short)-32768, - (short)-32768,
(short)1943, (short)1398, (short)1769, - (short)-32768, - (short)-32768, -
(short)-32768, (short)3995, (short)592, (short)302, (short)578, (short)574, -
(short)-32768, (short)12, (short)1342, - (short)-32768, (short)602, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
(short)1514, - (short)-32768, (short)591, (short)593, (short)590, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, (short)2054, (short)595, -
(short)-32768, (short)580, - (short)-32768, - (short)-32768, (short)3995,
(short)610, (short)580, (short)114, - (short)-32768, - (short)-32768, - (short)-32768,
(short)600, (short)597, (short)599, - (short)-32768, (short)107, (short)609, -
(short)-32768, (short)12, - (short)-32768, (short)603, (short)604, - (short)-32768,
(short)598, (short)211, (short)1718, (short)1917, (short)3071, - (short)-32768,
- (short)-32768, (short)285, - (short)-32768, (short)611, (short)612, (short)623,
- (short)-32768, - (short)-32768, (short)50, (short)2651, (short)373, (short)613,
(short)564, (short)615, (short)618, (short)3155, - (short)-32768, - (short)-32768,
(short)616, (short)619, (short)392, (short)19, (short)359, (short)360, (short)376,
(short)388, (short)110, (short)110, (short)395, (short)395, (short)395, (short)395,
- (short)38, - (short)38, (short)215, (short)215, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, (short)617, - (short)-32768, - (short)-32768,
- (short)-32768, (short)622, (short)40, - (short)-32768, - (short)-32768,
(short)100, (short)1917, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)-32768, (short)119, (short)624, - (short)-32768, - (short)-32768, (short)439,
(short)3995, (short)626, (short)397, (short)412, (short)415, (short)620, (short)3239,
(short)3323, (short)306, - (short)-32768, - (short)-32768, (short)614, (short)629,
(short)627, - (short)-32768, (short)1818, - (short)-32768, - (short)-32768,
(short)107, - (short)-32768, (short)631, - (short)-32768, (short)12, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, (short)433, (short)632,
(short)8, (short)440, - (short)-32768, - (short)-32768, (short)625, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)526,
- (short)-32768, (short)628, - (short)-32768, (short)634, - (short)-32768, -
(short)-32768, - (short)-32768, (short)12, - (short)-32768, (short)635, (short)55,
- (short)-32768, (short)633, (short)640, (short)640, (short)636, - (short)-32768,
(short)2231, - (short)-32768, - (short)-32768, - (short)-32768, (short)1818,
(short)1231, (short)637, (short)2483, - (short)-32768, - (short)-32768, (short)3995,
(short)3995, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
(short)2483, (short)238, - (short)-32768, (short)2315, - (short)-32768, -
(short)-32768, - (short)-32768, - (short)-32768, (short)1943, (short)642,
(short)1943, (short)3995, (short)3407, (short)318, (short)3491, (short)327,
(short)3575, (short)508, - (short)-32768, (short)644, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, (short)643, (short)645, - (short)-32768, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)609, -
(short)-32768, (short)646, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)-32768, (short)3995, - (short)-32768, - (short)-32768, (short)2651,
(short)647, (short)648, - (short)-32768, (short)219, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, (short)697, (short)508, - (short)-32768,
(short)453, (short)1943, (short)458, (short)3659, (short)1943, (short)460,
(short)3743, (short)3827, (short)349, (short)580, (short)638, (short)652, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
(short)145, - (short)-32768, (short)650, (short)651, - (short)-32768, (short)2399,
(short)1943, (short)656, (short)676, - (short)-32768, (short)1943, (short)1943,
(short)467, - (short)-32768, (short)1943, (short)1943, (short)470, (short)1943,
(short)478, (short)3911, - (short)19, (short)1060, - (short)-32768, (short)3995,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, (short)1943, - (short)-32768,
- (short)-32768, (short)1943, - (short)-32768, (short)1943, (short)1943, (short)488,
(short)3995, (short)821, - (short)-32768, (short)261, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, (short)1943, (short)164, (short)508, - (short)-32768,
- (short)-32768, - (short)-32768, (short)821, - (short)-32768, (short)508, -
(short)-32768, - (short)-32768, (short)727, (short)728, - (short)-32768}; static
short Cyc_yypgoto[ 102u]={ - (short)-32768, (short)83, - (short)330, - (short)-32768,
(short)469, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)46, - (short)34, - (short)50, - (short)-32768, (short)386,
- (short)37, (short)200, - (short)-32768, (short)150, - (short)250, (short)96, -
(short)-32768, - (short)33, - (short)-32768, - (short)202, - (short)-32768, -
(short)-32768, - (short)-32768, (short)575, (short)568, (short)49, - (short)-32768,
- (short)-32768, (short)262, - (short)-32768, - (short)125, - (short)-32768,
(short)97, - (short)30, - (short)36, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)99, - (short)366, (short)490, - (short)377, - (short)71, - (short)275,
- (short)-32768, - (short)-32768, - (short)175, - (short)335, - (short)259, -
(short)-32768, (short)345, - (short)137, (short)491, - (short)105, (short)157, -
(short)169, (short)14, - (short)-32768, - (short)-32768, - (short)45, - (short)226,
- (short)-32768, - (short)-32768, - (short)601, - (short)-32768, - (short)-32768,
- (short)10, (short)504, - (short)-32768, (short)228, - (short)-32768, - (short)-32768,
- (short)110, (short)7, - (short)-32768, - (short)249, - (short)349, - (short)-32768,
(short)466, (short)463, (short)464, (short)468, (short)471, (short)268, (short)169,
(short)269, (short)276, - (short)54, (short)436, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)285, - (short)-32768, - (short)-32768,
(short)10}; static short Cyc_yytable[ 4282u]={ (short)103, (short)250, (short)272,
(short)408, (short)101, (short)104, (short)246, (short)115, (short)393, (short)140,
(short)54, (short)428, (short)71, (short)418, (short)108, (short)269, (short)102,
(short)453, (short)499, (short)507, (short)530, (short)362, (short)109, (short)55,
(short)305, (short)412, (short)505, (short)119, (short)313, (short)503, (short)297,
(short)60, (short)72, (short)660, (short)73, (short)533, (short)75, (short)270,
(short)298, (short)126, (short)271, (short)54, (short)103, (short)199, (short)200,
(short)201, (short)101, (short)198, (short)54, (short)693, (short)228, (short)306,
(short)103, (short)54, (short)227, (short)120, (short)240, (short)241, (short)98,
(short)491, (short)124, (short)552, (short)54, (short)202, (short)495, (short)207,
(short)437, (short)314, (short)110, (short)118, (short)56, (short)327, (short)108,
(short)328, (short)553, (short)72, (short)255, (short)543, (short)465, (short)111,
(short)72, (short)536, (short)438, (short)37, (short)694, (short)263, (short)27,
(short)74, (short)169, (short)189, (short)284, (short)413, (short)29, (short)704,
(short)54, (short)54, (short)54, (short)54, (short)280, (short)31, (short)396,
(short)523, (short)708, (short)289, (short)76, (short)397, (short)710, (short)383,
(short)711, (short)308, (short)310, (short)446, (short)311, (short)54, (short)252,
(short)405, (short)278, (short)395, (short)618, (short)418, (short)446, (short)83,
(short)578, (short)54, (short)279, (short)285, (short)134, (short)453, (short)79,
(short)286, (short)345, (short)72, (short)15, (short)16, (short)17, (short)416,
(short)417, (short)118, (short)395, (short)421, (short)92, (short)346, (short)347,
(short)348, (short)566, (short)581, (short)206, (short)103, (short)567, (short)54,
(short)285, (short)240, (short)364, (short)285, (short)406, (short)134, (short)454,
(short)619, (short)455, (short)27, (short)385, (short)589, (short)77, (short)506,
(short)386, (short)29, (short)606, (short)190, (short)191, (short)192, (short)193,
(short)511, (short)287, (short)133, (short)321, (short)322, (short)133, (short)418,
(short)349, (short)81, (short)279, (short)276, (short)277, (short)306, (short)228,
(short)306, (short)350, (short)551, (short)227, (short)461, (short)128, (short)616,
(short)351, (short)133, (short)582, (short)80, (short)257, (short)129, (short)583,
(short)492, (short)242, (short)133, (short)133, (short)363, (short)472, (short)54,
(short)493, (short)352, (short)254, (short)546, (short)134, (short)323, (short)324,
(short)585, (short)84, (short)85, (short)54, (short)586, (short)426, (short)353,
(short)410, (short)34, (short)35, (short)36, (short)427, (short)27, (short)377,
(short)132, (short)133, (short)133, (short)132, (short)29, (short)384, (short)354,
(short)387, (short)388, (short)288, (short)651, (short)54, (short)628, (short)496,
- (short)78, (short)280, (short)395, - (short)78, (short)450, (short)260,
(short)132, (short)676, (short)77, (short)463, (short)289, (short)261, (short)206,
(short)228, (short)132, (short)132, (short)133, (short)227, (short)109, (short)431,
(short)418, (short)395, (short)27, (short)512, (short)513, (short)514, (short)707,
(short)518, (short)29, (short)118, (short)118, (short)497, (short)59, (short)118,
(short)487, (short)488, (short)489, (short)604, (short)498, (short)72, (short)72,
(short)132, (short)132, (short)82, (short)568, (short)445, (short)113, (short)56,
(short)273, (short)428, (short)281, (short)242, (short)105, (short)114, (short)274,
(short)290, (short)54, (short)106, (short)54, (short)133, (short)107, (short)56,
(short)372, (short)121, (short)363, (short)122, (short)447, (short)624, (short)123,
(short)54, (short)132, (short)448, (short)657, (short)468, (short)449, (short)27,
(short)658, (short)138, (short)140, (short)127, (short)329, (short)29, (short)96,
(short)624, (short)391, (short)97, (short)624, (short)103, (short)630, (short)330,
(short)331, (short)517, (short)394, (short)134, (short)125, (short)285, (short)86,
(short)87, (short)395, (short)631, (short)103, (short)134, (short)560, (short)134,
(short)101, (short)526, (short)194, (short)559, (short)540, (short)490, (short)468,
(short)409, (short)278, (short)132, (short)705, (short)468, (short)525, (short)195,
(short)395, (short)363, (short)279, (short)208, (short)34, (short)35, (short)36,
(short)401, (short)54, (short)363, (short)54, (short)262, - (short)78, (short)402,
(short)424, (short)133, (short)253, (short)77, (short)61, (short)258, (short)395,
(short)520, (short)133, (short)264, (short)133, (short)79, (short)259, (short)395,
(short)265, (short)133, (short)133, (short)273, (short)133, (short)623, (short)468,
(short)283, (short)584, (short)529, (short)468, (short)599, (short)624, (short)291,
(short)292, (short)527, (short)524, (short)395, (short)299, (short)596, (short)598,
(short)623, (short)450, (short)639, (short)623, (short)575, (short)542, (short)206,
(short)300, (short)395, (short)301, (short)545, (short)642, (short)307, (short)54,
(short)206, (short)27, (short)62, (short)395, (short)63, (short)64, (short)132,
(short)29, (short)93, (short)94, (short)95, (short)72, (short)425, (short)132,
(short)65, (short)132, (short)72, (short)66, (short)67, (short)672, (short)132,
(short)132, (short)426, (short)132, (short)68, (short)395, (short)695, (short)654,
(short)427, (short)69, (short)70, (short)27, (short)319, (short)320, (short)562,
(short)325, (short)326, (short)29, (short)315, (short)133, (short)88, (short)467,
(short)395, (short)312, (short)31, (short)363, (short)703, (short)133, (short)569,
(short)395, (short)316, (short)54, (short)317, (short)103, (short)34, (short)35,
(short)36, (short)240, (short)603, (short)709, (short)365, (short)623, (short)636,
(short)638, (short)133, (short)641, (short)318, (short)644, (short)479, (short)480,
(short)481, (short)482, (short)591, (short)395, (short)344, (short)134, (short)133,
(short)332, (short)333, (short)334, (short)335, (short)336, (short)337, (short)338,
(short)339, (short)340, (short)341, (short)592, (short)395, (short)653, (short)593,
(short)395, (short)355, (short)132, (short)133, (short)607, (short)608, (short)54,
(short)356, (short)103, (short)27, (short)132, (short)342, (short)101, (short)198,
(short)373, (short)29, (short)665, (short)645, (short)646, (short)669, (short)671,
(short)374, (short)31, (short)358, (short)611, (short)292, (short)431, (short)132,
(short)366, (short)368, (short)133, (short)133, (short)34, (short)35, (short)36,
(short)27, (short)134, (short)661, (short)395, (short)132, (short)375, (short)29,
(short)663, (short)395, (short)667, (short)395, (short)72, (short)571, (short)287,
(short)692, (short)61, (short)685, (short)395, (short)696, (short)688, (short)395,
(short)279, (short)132, (short)34, (short)35, (short)36, (short)363, (short)690,
(short)395, (short)376, (short)294, (short)378, (short)296, (short)363, (short)626,
(short)702, (short)629, (short)701, (short)395, (short)477, (short)478, (short)379,
(short)363, (short)302, (short)303, (short)363, (short)483, (short)484, (short)133,
(short)132, (short)132, (short)309, (short)309, (short)380, (short)309, (short)485,
(short)486, (short)633, (short)372, (short)635, (short)381, (short)27, (short)62,
(short)390, (short)63, (short)64, (short)389, (short)29, (short)392, (short)61,
(short)196, (short)404, (short)309, (short)625, (short)65, (short)407, (short)228,
(short)66, (short)67, (short)400, (short)227, (short)403, (short)426, (short)363,
(short)68, (short)411, (short)420, (short)673, (short)427, (short)69, (short)70,
(short)422, (short)436, (short)423, (short)439, (short)464, (short)228, (short)500,
(short)466, (short)27, (short)227, (short)508, (short)132, (short)662, (short)509,
(short)29, (short)666, (short)72, (short)510, (short)228, (short)515, (short)519,
(short)31, (short)227, (short)521, (short)27, (short)62, (short)363, (short)63,
(short)64, (short)133, (short)29, (short)34, (short)35, (short)36, (short)680,
(short)528, (short)531, (short)65, (short)683, (short)684, (short)66, (short)67,
(short)686, (short)687, (short)532, (short)689, (short)535, (short)68, (short)537,
(short)539, (short)541, (short)538, (short)69, (short)70, (short)544, (short)548,
(short)549, (short)550, (short)273, (short)309, (short)697, (short)554, (short)555,
(short)698, (short)556, (short)699, (short)700, (short)565, (short)600, (short)563,
(short)564, (short)570, (short)572, (short)573, (short)576, (short)579, (short)706,
(short)594, (short)577, (short)587, (short)580, (short)590, (short)601, (short)132,
(short)659, (short)602, (short)605, (short)614, (short)713, (short)714, (short)609,
(short)612, (short)615, (short)617, (short)574, (short)627, (short)634, (short)648,
(short)621, (short)620, (short)610, (short)649, (short)674, (short)650, (short)652,
(short)655, (short)656, (short)675, (short)677, (short)678, (short)309, (short)681,
(short)309, (short)309, (short)309, (short)309, (short)309, (short)309, (short)309,
(short)309, (short)309, (short)309, (short)309, (short)309, (short)309, (short)309,
(short)309, (short)309, (short)309, (short)682, (short)462, (short)360, (short)588,
(short)367, (short)451, (short)547, (short)435, (short)452, (short)613, (short)473,
(short)471, (short)474, (short)0, (short)0, (short)0, (short)0, (short)475,
(short)0, (short)0, (short)0, (short)476, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)309, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)1, (short)2,
(short)3, (short)99, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)645, (short)646, (short)20, (short)209, (short)0,
(short)210, (short)211, (short)212, (short)213, (short)214, (short)215, (short)216,
(short)217, (short)142, (short)21, (short)143, (short)22, (short)144, (short)218,
(short)309, (short)145, (short)23, (short)219, (short)0, (short)0, (short)26,
(short)146, (short)147, (short)220, (short)221, (short)148, (short)149, (short)150,
(short)151, (short)152, (short)153, (short)154, (short)0, (short)155, (short)156,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)309, (short)0, (short)222, (short)157,
(short)158, (short)159, (short)160, (short)28, (short)29, (short)30, (short)0,
(short)223, (short)100, (short)0, (short)0, (short)161, (short)0, (short)0,
(short)32, (short)225, (short)0, (short)0, (short)0, (short)0, (short)0, (short)163,
(short)0, (short)0, (short)164, (short)0, (short)165, (short)166, (short)0,
(short)0, (short)0, (short)0, (short)167, (short)168, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)309, (short)1, (short)2, (short)3,
(short)99, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)0, (short)0, (short)20, (short)209, (short)0, (short)210,
(short)211, (short)212, (short)213, (short)214, (short)215, (short)216, (short)217,
(short)142, (short)21, (short)143, (short)22, (short)144, (short)218, (short)0,
(short)145, (short)23, (short)219, (short)0, (short)0, (short)26, (short)146,
(short)147, (short)220, (short)221, (short)148, (short)149, (short)150, (short)151,
(short)152, (short)153, (short)154, (short)0, (short)155, (short)156, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)309,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)222, (short)157, (short)158,
(short)159, (short)160, (short)28, (short)29, (short)30, (short)0, (short)223,
(short)100, (short)224, (short)0, (short)161, (short)0, (short)0, (short)32,
(short)225, (short)0, (short)0, (short)0, (short)0, (short)0, (short)163,
(short)0, (short)0, (short)164, (short)0, (short)165, (short)166, (short)0,
(short)0, (short)0, (short)0, (short)167, (short)168, (short)1, (short)2,
(short)3, (short)99, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)0, (short)0, (short)20, (short)209, (short)0,
(short)210, (short)211, (short)212, (short)213, (short)214, (short)215, (short)216,
(short)217, (short)142, (short)21, (short)143, (short)22, (short)144, (short)218,
(short)0, (short)145, (short)23, (short)219, (short)0, (short)0, (short)26,
(short)146, (short)147, (short)220, (short)221, (short)148, (short)149, (short)150,
(short)151, (short)152, (short)153, (short)154, (short)0, (short)155, (short)156,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)222, (short)157,
(short)158, (short)159, (short)160, (short)28, (short)29, (short)30, (short)0,
(short)223, (short)100, (short)0, (short)0, (short)161, (short)0, (short)0,
(short)32, (short)225, (short)0, (short)0, (short)0, (short)0, (short)0, (short)163,
(short)0, (short)0, (short)164, (short)0, (short)165, (short)166, (short)0,
(short)0, (short)0, (short)0, (short)167, (short)168, (short)1, (short)2,
(short)3, (short)99, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)0, (short)0, (short)20, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)142,
(short)21, (short)143, (short)22, (short)144, (short)0, (short)0, (short)145,
(short)23, (short)0, (short)0, (short)0, (short)26, (short)146, (short)147,
(short)0, (short)0, (short)148, (short)149, (short)150, (short)151, (short)152,
(short)153, (short)154, (short)0, (short)155, (short)156, (short)0, (short)6,
(short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)27, (short)157, (short)158,
(short)159, (short)160, (short)28, (short)29, (short)30, (short)21, (short)516,
(short)0, (short)0, (short)0, (short)161, (short)0, (short)0, (short)32, (short)225,
(short)0, (short)26, (short)0, (short)0, (short)0, (short)163, (short)0, (short)0,
(short)164, (short)0, (short)165, (short)166, (short)0, (short)0, (short)0,
(short)0, (short)167, (short)168, (short)6, (short)7, (short)8, (short)9,
(short)10, (short)11, (short)12, (short)13, (short)14, (short)15, (short)16,
(short)17, (short)18, (short)19, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)27, (short)0, (short)0, (short)0, (short)0, (short)28, (short)29, (short)30,
(short)142, (short)21, (short)143, (short)0, (short)144, (short)0, (short)0,
(short)145, (short)32, (short)33, (short)0, (short)0, (short)26, (short)146,
(short)147, (short)0, (short)0, (short)148, (short)149, (short)150, (short)151,
(short)152, (short)153, (short)154, (short)0, (short)155, (short)156, (short)0,
(short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)27, (short)157, (short)158,
(short)159, (short)160, (short)28, (short)29, (short)30, (short)21, (short)0,
(short)304, (short)0, (short)0, (short)161, (short)0, (short)0, (short)32,
(short)225, (short)0, (short)26, (short)0, (short)0, (short)0, (short)163,
(short)0, (short)0, (short)164, (short)0, (short)165, (short)166, (short)0,
(short)0, (short)0, (short)0, (short)167, (short)168, (short)6, (short)7,
(short)8, (short)9, (short)10, (short)11, (short)12, (short)13, (short)14,
(short)15, (short)16, (short)17, (short)18, (short)19, (short)0, (short)0,
(short)534, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)28, (short)0, (short)30, (short)142, (short)21, (short)143, (short)0,
(short)144, (short)0, (short)0, (short)145, (short)32, (short)33, (short)0,
(short)0, (short)26, (short)146, (short)147, (short)0, (short)0, (short)148,
(short)149, (short)150, (short)151, (short)152, (short)153, (short)154, (short)0,
(short)155, (short)156, (short)0, (short)0, (short)6, (short)7, (short)8,
(short)9, (short)10, (short)11, (short)12, (short)13, (short)14, (short)15,
(short)16, (short)17, (short)18, (short)19, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)27, (short)157, (short)158, (short)159, (short)160,
(short)28, (short)29, (short)30, (short)0, (short)21, (short)0, (short)0,
(short)0, (short)161, (short)0, (short)0, (short)32, (short)225, (short)0,
(short)0, (short)26, (short)0, (short)0, (short)163, (short)0, (short)0, (short)164,
(short)0, (short)165, (short)166, (short)0, (short)0, (short)0, (short)0,
(short)167, (short)168, (short)1, (short)2, (short)3, (short)4, (short)5,
(short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)0,
(short)243, (short)20, (short)0, (short)0, (short)0, (short)28, (short)0,
(short)30, (short)0, (short)244, (short)0, (short)0, (short)0, (short)21,
(short)245, (short)22, (short)32, (short)33, (short)0, (short)0, (short)23,
(short)0, (short)24, (short)25, (short)26, (short)0, (short)0, (short)0, (short)1,
(short)2, (short)3, (short)99, (short)5, (short)6, (short)7, (short)8, (short)9,
(short)10, (short)11, (short)12, (short)13, (short)14, (short)15, (short)16,
(short)17, (short)18, (short)19, (short)0, (short)0, (short)20, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)27, (short)21, (short)0, (short)0, (short)0, (short)28, (short)29,
(short)30, (short)23, (short)0, (short)0, (short)0, (short)26, (short)31,
(short)0, (short)0, (short)32, (short)33, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)34, (short)35, (short)36, (short)0, (short)0, (short)6, (short)7,
(short)8, (short)9, (short)10, (short)11, (short)12, (short)13, (short)14,
(short)15, (short)16, (short)17, (short)18, (short)19, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)27, (short)0, (short)0, (short)0,
(short)0, (short)28, (short)29, (short)30, (short)21, (short)0, (short)0,
(short)0, (short)0, (short)31, (short)0, (short)0, (short)32, (short)33, (short)0,
(short)26, (short)0, (short)0, (short)0, (short)34, (short)35, (short)36,
(short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)21, (short)27, (short)0,
(short)0, (short)0, (short)0, (short)28, (short)29, (short)30, (short)0, (short)440,
(short)26, (short)0, (short)0, (short)287, (short)441, (short)0, (short)32,
(short)33, (short)0, (short)0, (short)0, (short)279, (short)0, (short)34,
(short)35, (short)36, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)28, (short)0, (short)30, (short)21,
(short)440, (short)0, (short)0, (short)0, (short)278, (short)441, (short)0,
(short)32, (short)33, (short)0, (short)26, (short)0, (short)279, (short)0,
(short)34, (short)35, (short)36, (short)1, (short)2, (short)3, (short)99,
(short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)0, (short)0, (short)20, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)21, (short)28,
(short)22, (short)30, (short)0, (short)557, (short)0, (short)23, (short)0,
(short)0, (short)558, (short)26, (short)32, (short)33, (short)0, (short)0,
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
(short)0, (short)0, (short)0, (short)0, (short)0, (short)6, (short)7, (short)8,
(short)9, (short)10, (short)11, (short)12, (short)13, (short)14, (short)15,
(short)16, (short)17, (short)18, (short)19, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)21, (short)0, (short)28, (short)0, (short)30,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)26, (short)0,
(short)32, (short)33, (short)209, (short)0, (short)210, (short)211, (short)212,
(short)213, (short)214, (short)215, (short)216, (short)217, (short)142, (short)0,
(short)143, (short)0, (short)144, (short)218, (short)0, (short)145, (short)0,
(short)219, (short)0, (short)0, (short)0, (short)146, (short)147, (short)220,
(short)221, (short)148, (short)149, (short)150, (short)151, (short)152, (short)153,
(short)154, (short)0, (short)155, (short)156, (short)28, (short)0, (short)30,
(short)0, (short)0, (short)131, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)32, (short)33, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)222, (short)157, (short)158, (short)159, (short)160,
(short)0, (short)29, (short)0, (short)0, (short)223, (short)100, (short)0,
(short)0, (short)161, (short)0, (short)0, (short)0, (short)162, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)163, (short)0, (short)0, (short)164,
(short)0, (short)165, (short)166, (short)0, (short)0, (short)0, (short)0,
(short)167, (short)168, (short)6, (short)7, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)21, (short)0, (short)0, (short)456, (short)0, (short)0, (short)0, (short)0,
(short)142, (short)0, (short)143, (short)26, (short)144, (short)0, (short)0,
(short)145, (short)0, (short)0, (short)0, (short)0, (short)0, (short)146,
(short)147, (short)0, (short)0, (short)148, (short)149, (short)150, (short)151,
(short)152, (short)153, (short)154, (short)0, (short)155, (short)156, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)28, (short)0, (short)30, (short)0, (short)27, (short)157, (short)158,
(short)159, (short)160, (short)0, (short)29, (short)32, (short)33, (short)0,
(short)361, (short)457, (short)0, (short)161, (short)0, (short)0, (short)0,
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
(short)0, (short)29, (short)0, (short)0, (short)0, (short)361, (short)504,
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
(short)0, (short)0, (short)0, (short)361, (short)622, (short)0, (short)161,
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
(short)361, (short)632, (short)0, (short)161, (short)0, (short)0, (short)0,
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
(short)0, (short)29, (short)0, (short)0, (short)0, (short)361, (short)679,
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
(short)0, (short)0, (short)162, (short)0, (short)0, (short)0, (short)0, (short)444,
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
(short)0, (short)161, (short)494, (short)0, (short)0, (short)162, (short)0,
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
(short)0, (short)0, (short)0, (short)161, (short)522, (short)0, (short)0,
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
(short)0, (short)0, (short)561, (short)163, (short)0, (short)0, (short)164,
(short)0, (short)165, (short)166, (short)0, (short)0, (short)0, (short)0,
(short)167, (short)168, (short)142, (short)0, (short)143, (short)0, (short)144,
(short)0, (short)0, (short)145, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)146, (short)147, (short)0, (short)0, (short)148, (short)149, (short)150,
(short)151, (short)152, (short)153, (short)154, (short)0, (short)155, (short)156,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)27, (short)157, (short)158,
(short)159, (short)160, (short)0, (short)29, (short)0, (short)0, (short)0,
(short)574, (short)0, (short)0, (short)161, (short)0, (short)0, (short)0,
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
(short)0, (short)29, (short)0, (short)0, (short)595, (short)0, (short)0, (short)0,
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
(short)159, (short)160, (short)0, (short)29, (short)0, (short)0, (short)597,
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
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)161, (short)637,
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
(short)0, (short)161, (short)640, (short)0, (short)0, (short)162, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)163, (short)0, (short)0, (short)164,
(short)0, (short)165, (short)166, (short)0, (short)0, (short)0, (short)0,
(short)167, (short)168, (short)142, (short)0, (short)143, (short)0, (short)144,
(short)0, (short)0, (short)145, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)146, (short)147, (short)0, (short)0, (short)148, (short)149, (short)150,
(short)151, (short)152, (short)153, (short)154, (short)0, (short)155, (short)156,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)27, (short)157, (short)158,
(short)159, (short)160, (short)0, (short)29, (short)0, (short)0, (short)643,
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
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)161, (short)664,
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
(short)0, (short)161, (short)668, (short)0, (short)0, (short)162, (short)0,
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
(short)0, (short)0, (short)0, (short)161, (short)670, (short)0, (short)0,
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
(short)161, (short)691, (short)0, (short)0, (short)162, (short)0, (short)0,
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
(short)167, (short)168}; static short Cyc_yycheck[ 4282u]={ (short)50, (short)106,
(short)127, (short)253, (short)50, (short)50, (short)105, (short)57, (short)234,
(short)80, (short)0, (short)270, (short)22, (short)262, (short)51, (short)125,
(short)50, (short)292, (short)353, (short)368, (short)397, (short)196, (short)52,
(short)85, (short)161, (short)17, (short)361, (short)60, (short)69, (short)359,
(short)85, (short)21, (short)22, (short)634, (short)24, (short)401, (short)26,
(short)93, (short)93, (short)72, (short)96, (short)31, (short)92, (short)93,
(short)94, (short)95, (short)92, (short)92, (short)38, (short)68, (short)100,
(short)161, (short)102, (short)43, (short)100, (short)65, (short)102, (short)102,
(short)48, (short)344, (short)70, (short)438, (short)52, (short)96, (short)349,
(short)98, (short)82, (short)108, (short)88, (short)59, (short)91, (short)109,
(short)109, (short)111, (short)440, (short)65, (short)112, (short)426, (short)304,
(short)99, (short)70, (short)411, (short)98, (short)0, (short)103, (short)118,
(short)83, (short)83, (short)81, (short)82, (short)63, (short)83, (short)89,
(short)694, (short)84, (short)85, (short)86, (short)87, (short)134, (short)96,
(short)88, (short)386, (short)703, (short)139, (short)82, (short)93, (short)707,
(short)217, (short)709, (short)163, (short)164, (short)280, (short)166, (short)103,
(short)107, (short)63, (short)96, (short)98, (short)63, (short)368, (short)289,
(short)38, (short)103, (short)113, (short)104, (short)98, (short)77, (short)402,
(short)31, (short)102, (short)184, (short)121, (short)17, (short)18, (short)19,
(short)260, (short)261, (short)127, (short)98, (short)264, (short)43, (short)59,
(short)60, (short)61, (short)94, (short)105, (short)97, (short)197, (short)98,
(short)139, (short)98, (short)197, (short)197, (short)98, (short)102, (short)106,
(short)293, (short)102, (short)295, (short)83, (short)92, (short)510, (short)101,
(short)365, (short)96, (short)89, (short)532, (short)84, (short)85, (short)86,
(short)87, (short)373, (short)96, (short)77, (short)64, (short)65, (short)80,
(short)426, (short)96, (short)93, (short)104, (short)132, (short)133, (short)293,
(short)234, (short)295, (short)104, (short)437, (short)234, (short)299, (short)83,
(short)557, (short)110, (short)97, (short)94, (short)96, (short)113, (short)90,
(short)98, (short)83, (short)103, (short)105, (short)106, (short)196, (short)314,
(short)195, (short)90, (short)81, (short)112, (short)95, (short)161, (short)101,
(short)102, (short)94, (short)92, (short)93, (short)206, (short)98, (short)104,
(short)93, (short)256, (short)106, (short)107, (short)108, (short)110, (short)83,
(short)212, (short)77, (short)132, (short)133, (short)80, (short)89, (short)218,
(short)107, (short)220, (short)221, (short)139, (short)614, (short)228, (short)574,
(short)350, (short)93, (short)278, (short)98, (short)96, (short)282, (short)92,
(short)97, (short)103, (short)101, (short)300, (short)287, (short)98, (short)204,
(short)304, (short)105, (short)106, (short)161, (short)304, (short)289, (short)270,
(short)510, (short)98, (short)83, (short)374, (short)375, (short)376, (short)103,
(short)378, (short)89, (short)260, (short)261, (short)83, (short)93, (short)264,
(short)329, (short)330, (short)331, (short)528, (short)90, (short)270, (short)271,
(short)132, (short)133, (short)93, (short)460, (short)279, (short)93, (short)91,
(short)88, (short)549, (short)134, (short)195, (short)96, (short)96, (short)94,
(short)139, (short)287, (short)101, (short)289, (short)204, (short)104, (short)91,
(short)206, (short)96, (short)298, (short)83, (short)96, (short)567, (short)84,
(short)300, (short)161, (short)101, (short)94, (short)307, (short)104, (short)83,
(short)98, (short)97, (short)390, (short)93, (short)106, (short)89, (short)90,
(short)583, (short)228, (short)93, (short)586, (short)378, (short)578, (short)115,
(short)116, (short)378, (short)92, (short)285, (short)95, (short)98, (short)92,
(short)93, (short)98, (short)102, (short)391, (short)293, (short)448, (short)295,
(short)391, (short)391, (short)92, (short)447, (short)420, (short)343, (short)344,
(short)256, (short)96, (short)204, (short)94, (short)349, (short)391, (short)98,
(short)98, (short)353, (short)104, (short)85, (short)106, (short)107, (short)108,
(short)92, (short)357, (short)361, (short)359, (short)95, (short)96, (short)98,
(short)92, (short)278, (short)82, (short)101, (short)37, (short)96, (short)98,
(short)92, (short)285, (short)93, (short)287, (short)287, (short)94, (short)98,
(short)97, (short)292, (short)293, (short)88, (short)295, (short)567, (short)386,
(short)109, (short)500, (short)94, (short)390, (short)92, (short)658, (short)97,
(short)98, (short)395, (short)389, (short)98, (short)96, (short)516, (short)517,
(short)583, (short)446, (short)92, (short)586, (short)466, (short)423, (short)365,
(short)96, (short)98, (short)96, (short)428, (short)92, (short)96, (short)411,
(short)373, (short)83, (short)84, (short)98, (short)86, (short)87, (short)278,
(short)89, (short)44, (short)45, (short)46, (short)423, (short)94, (short)285,
(short)96, (short)287, (short)428, (short)99, (short)100, (short)92, (short)292,
(short)293, (short)104, (short)295, (short)106, (short)98, (short)674, (short)624,
(short)110, (short)111, (short)112, (short)83, (short)66, (short)67, (short)449,
(short)62, (short)63, (short)89, (short)68, (short)365, (short)92, (short)97,
(short)98, (short)94, (short)96, (short)460, (short)694, (short)373, (short)97,
(short)98, (short)113, (short)463, (short)114, (short)525, (short)106, (short)107,
(short)108, (short)525, (short)525, (short)707, (short)93, (short)658, (short)594,
(short)595, (short)390, (short)597, (short)112, (short)599, (short)321, (short)322,
(short)323, (short)324, (short)97, (short)98, (short)96, (short)448, (short)402,
(short)70, (short)71, (short)72, (short)73, (short)74, (short)75, (short)76,
(short)77, (short)78, (short)79, (short)97, (short)98, (short)621, (short)97,
(short)98, (short)94, (short)365, (short)420, (short)84, (short)85, (short)509,
(short)94, (short)571, (short)83, (short)373, (short)95, (short)571, (short)571,
(short)93, (short)89, (short)639, (short)22, (short)23, (short)642, (short)643,
(short)96, (short)96, (short)94, (short)97, (short)98, (short)549, (short)390,
(short)94, (short)103, (short)447, (short)448, (short)106, (short)107, (short)108,
(short)83, (short)500, (short)97, (short)98, (short)402, (short)96, (short)89,
(short)97, (short)98, (short)97, (short)98, (short)549, (short)463, (short)96,
(short)672, (short)37, (short)97, (short)98, (short)676, (short)97, (short)98,
(short)104, (short)420, (short)106, (short)107, (short)108, (short)567, (short)97,
(short)98, (short)96, (short)142, (short)96, (short)144, (short)574, (short)572,
(short)693, (short)577, (short)97, (short)98, (short)319, (short)320, (short)83,
(short)583, (short)155, (short)156, (short)586, (short)325, (short)326, (short)500,
(short)447, (short)448, (short)163, (short)164, (short)92, (short)166, (short)327,
(short)328, (short)591, (short)509, (short)593, (short)92, (short)83, (short)84,
(short)96, (short)86, (short)87, (short)103, (short)89, (short)94, (short)37,
(short)95, (short)98, (short)184, (short)572, (short)96, (short)105, (short)674,
(short)99, (short)100, (short)97, (short)674, (short)97, (short)104, (short)624,
(short)106, (short)94, (short)96, (short)645, (short)110, (short)111, (short)112,
(short)97, (short)94, (short)98, (short)94, (short)35, (short)694, (short)101,
(short)97, (short)83, (short)694, (short)92, (short)500, (short)637, (short)98,
(short)89, (short)640, (short)645, (short)103, (short)707, (short)28, (short)92,
(short)96, (short)707, (short)41, (short)83, (short)84, (short)658, (short)86,
(short)87, (short)572, (short)89, (short)106, (short)107, (short)108, (short)659,
(short)82, (short)97, (short)96, (short)663, (short)664, (short)99, (short)100,
(short)667, (short)668, (short)109, (short)670, (short)83, (short)106, (short)96,
(short)98, (short)94, (short)97, (short)111, (short)112, (short)83, (short)94,
(short)98, (short)97, (short)88, (short)262, (short)685, (short)97, (short)97,
(short)688, (short)105, (short)690, (short)691, (short)83, (short)93, (short)97,
(short)97, (short)97, (short)96, (short)94, (short)97, (short)97, (short)701,
(short)96, (short)98, (short)94, (short)97, (short)94, (short)92, (short)572,
(short)26, (short)97, (short)94, (short)98, (short)0, (short)0, (short)97,
(short)105, (short)97, (short)97, (short)93, (short)97, (short)93, (short)92,
(short)101, (short)105, (short)539, (short)97, (short)103, (short)97, (short)97,
(short)97, (short)97, (short)94, (short)97, (short)97, (short)313, (short)94,
(short)315, (short)316, (short)317, (short)318, (short)319, (short)320, (short)321,
(short)322, (short)323, (short)324, (short)325, (short)326, (short)327, (short)328,
(short)329, (short)330, (short)331, (short)92, (short)300, (short)195, (short)509,
(short)204, (short)283, (short)429, (short)271, (short)285, (short)549, (short)315,
(short)313, (short)316, - (short)1, - (short)1, - (short)1, - (short)1, (short)317,
- (short)1, - (short)1, - (short)1, (short)318, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)368, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)3,
(short)4, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)20, (short)21, (short)22, (short)23, (short)24, (short)25, -
(short)1, (short)27, (short)28, (short)29, (short)30, (short)31, (short)32,
(short)33, (short)34, (short)35, (short)36, (short)37, (short)38, (short)39,
(short)40, (short)426, (short)42, (short)43, (short)44, - (short)1, - (short)1,
(short)47, (short)48, (short)49, (short)50, (short)51, (short)52, (short)53,
(short)54, (short)55, (short)56, (short)57, (short)58, - (short)1, (short)60,
(short)61, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)466, - (short)1, (short)83, (short)84, (short)85, (short)86, (short)87,
(short)88, (short)89, (short)90, - (short)1, (short)92, (short)93, - (short)1, -
(short)1, (short)96, - (short)1, - (short)1, (short)99, (short)100, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)106, - (short)1, -
(short)1, (short)109, - (short)1, (short)111, (short)112, - (short)1, - (short)1,
- (short)1, - (short)1, (short)117, (short)118, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, (short)510, (short)3, (short)4,
(short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)20, (short)21, - (short)1, - (short)1, (short)24, (short)25, - (short)1,
(short)27, (short)28, (short)29, (short)30, (short)31, (short)32, (short)33,
(short)34, (short)35, (short)36, (short)37, (short)38, (short)39, (short)40, -
(short)1, (short)42, (short)43, (short)44, - (short)1, - (short)1, (short)47,
(short)48, (short)49, (short)50, (short)51, (short)52, (short)53, (short)54,
(short)55, (short)56, (short)57, (short)58, - (short)1, (short)60, (short)61, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)578, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)83, (short)84, (short)85, (short)86, (short)87, (short)88,
(short)89, (short)90, - (short)1, (short)92, (short)93, (short)94, - (short)1,
(short)96, - (short)1, - (short)1, (short)99, (short)100, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)106, - (short)1, - (short)1, (short)109,
- (short)1, (short)111, (short)112, - (short)1, - (short)1, - (short)1, -
(short)1, (short)117, (short)118, (short)3, (short)4, (short)5, (short)6,
(short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)20,
(short)21, - (short)1, - (short)1, (short)24, (short)25, - (short)1, (short)27,
(short)28, (short)29, (short)30, (short)31, (short)32, (short)33, (short)34,
(short)35, (short)36, (short)37, (short)38, (short)39, (short)40, - (short)1,
(short)42, (short)43, (short)44, - (short)1, - (short)1, (short)47, (short)48,
(short)49, (short)50, (short)51, (short)52, (short)53, (short)54, (short)55,
(short)56, (short)57, (short)58, - (short)1, (short)60, (short)61, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)83, (short)84, (short)85, (short)86, (short)87, (short)88, (short)89,
(short)90, - (short)1, (short)92, (short)93, - (short)1, - (short)1, (short)96,
- (short)1, - (short)1, (short)99, (short)100, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)106, - (short)1, - (short)1, (short)109, -
(short)1, (short)111, (short)112, - (short)1, - (short)1, - (short)1, - (short)1,
(short)117, (short)118, (short)3, (short)4, (short)5, (short)6, (short)7,
(short)8, (short)9, (short)10, (short)11, (short)12, (short)13, (short)14,
(short)15, (short)16, (short)17, (short)18, (short)19, (short)20, (short)21, -
(short)1, - (short)1, (short)24, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)35,
(short)36, (short)37, (short)38, (short)39, - (short)1, - (short)1, (short)42,
(short)43, - (short)1, - (short)1, - (short)1, (short)47, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, (short)58, - (short)1, (short)60, (short)61, - (short)1, (short)8,
(short)9, (short)10, (short)11, (short)12, (short)13, (short)14, (short)15,
(short)16, (short)17, (short)18, (short)19, (short)20, (short)21, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)83, (short)84,
(short)85, (short)86, (short)87, (short)88, (short)89, (short)90, (short)36,
(short)92, - (short)1, - (short)1, - (short)1, (short)96, - (short)1, - (short)1,
(short)99, (short)100, - (short)1, (short)47, - (short)1, - (short)1, - (short)1,
(short)106, - (short)1, - (short)1, (short)109, - (short)1, (short)111, (short)112,
- (short)1, - (short)1, - (short)1, - (short)1, (short)117, (short)118, (short)8,
(short)9, (short)10, (short)11, (short)12, (short)13, (short)14, (short)15,
(short)16, (short)17, (short)18, (short)19, (short)20, (short)21, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, (short)83, - (short)1, - (short)1,
- (short)1, - (short)1, (short)88, (short)89, (short)90, (short)35, (short)36,
(short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42, (short)99,
(short)100, - (short)1, - (short)1, (short)47, (short)48, (short)49, - (short)1,
- (short)1, (short)52, (short)53, (short)54, (short)55, (short)56, (short)57,
(short)58, - (short)1, (short)60, (short)61, - (short)1, (short)8, (short)9,
(short)10, (short)11, (short)12, (short)13, (short)14, (short)15, (short)16,
(short)17, (short)18, (short)19, (short)20, (short)21, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, (short)83, (short)84, (short)85,
(short)86, (short)87, (short)88, (short)89, (short)90, (short)36, - (short)1,
(short)93, - (short)1, - (short)1, (short)96, - (short)1, - (short)1, (short)99,
(short)100, - (short)1, (short)47, - (short)1, - (short)1, - (short)1, (short)106,
- (short)1, - (short)1, (short)109, - (short)1, (short)111, (short)112, -
(short)1, - (short)1, - (short)1, - (short)1, (short)117, (short)118, (short)8,
(short)9, (short)10, (short)11, (short)12, (short)13, (short)14, (short)15,
(short)16, (short)17, (short)18, (short)19, (short)20, (short)21, - (short)1, -
(short)1, (short)80, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)88, - (short)1, (short)90, (short)35, (short)36,
(short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42, (short)99,
(short)100, - (short)1, - (short)1, (short)47, (short)48, (short)49, - (short)1,
- (short)1, (short)52, (short)53, (short)54, (short)55, (short)56, (short)57,
(short)58, - (short)1, (short)60, (short)61, - (short)1, - (short)1, (short)8,
(short)9, (short)10, (short)11, (short)12, (short)13, (short)14, (short)15,
(short)16, (short)17, (short)18, (short)19, (short)20, (short)21, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, (short)83, (short)84, (short)85,
(short)86, (short)87, (short)88, (short)89, (short)90, - (short)1, (short)36, -
(short)1, - (short)1, - (short)1, (short)96, - (short)1, - (short)1, (short)99,
(short)100, - (short)1, - (short)1, (short)47, - (short)1, - (short)1, (short)106,
- (short)1, - (short)1, (short)109, - (short)1, (short)111, (short)112, -
(short)1, - (short)1, - (short)1, - (short)1, (short)117, (short)118, (short)3,
(short)4, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)20, (short)21, - (short)1, (short)83, (short)24, - (short)1, -
(short)1, - (short)1, (short)88, - (short)1, (short)90, - (short)1, (short)92, -
(short)1, - (short)1, - (short)1, (short)36, (short)97, (short)38, (short)99,
(short)100, - (short)1, - (short)1, (short)43, - (short)1, (short)45, (short)46,
(short)47, - (short)1, - (short)1, - (short)1, (short)3, (short)4, (short)5,
(short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)20,
(short)21, - (short)1, - (short)1, (short)24, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, (short)83, (short)36, - (short)1, - (short)1, - (short)1, (short)88,
(short)89, (short)90, (short)43, - (short)1, - (short)1, - (short)1, (short)47,
(short)96, - (short)1, - (short)1, (short)99, (short)100, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)106, (short)107, (short)108, -
(short)1, - (short)1, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)20, (short)21, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)83, - (short)1, - (short)1, - (short)1, - (short)1, (short)88,
(short)89, (short)90, (short)36, - (short)1, - (short)1, - (short)1, - (short)1,
(short)96, - (short)1, - (short)1, (short)99, (short)100, - (short)1, (short)47,
- (short)1, - (short)1, - (short)1, (short)106, (short)107, (short)108, (short)8,
(short)9, (short)10, (short)11, (short)12, (short)13, (short)14, (short)15,
(short)16, (short)17, (short)18, (short)19, (short)20, (short)21, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)36,
(short)83, - (short)1, - (short)1, - (short)1, - (short)1, (short)88, (short)89,
(short)90, - (short)1, (short)92, (short)47, - (short)1, - (short)1, (short)96,
(short)97, - (short)1, (short)99, (short)100, - (short)1, - (short)1, - (short)1,
(short)104, - (short)1, (short)106, (short)107, (short)108, (short)8, (short)9,
(short)10, (short)11, (short)12, (short)13, (short)14, (short)15, (short)16,
(short)17, (short)18, (short)19, (short)20, (short)21, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)88, - (short)1, (short)90, (short)36, (short)92,
- (short)1, - (short)1, - (short)1, (short)96, (short)97, - (short)1, (short)99,
(short)100, - (short)1, (short)47, - (short)1, (short)104, - (short)1, (short)106,
(short)107, (short)108, (short)3, (short)4, (short)5, (short)6, (short)7,
(short)8, (short)9, (short)10, (short)11, (short)12, (short)13, (short)14,
(short)15, (short)16, (short)17, (short)18, (short)19, (short)20, (short)21, -
(short)1, - (short)1, (short)24, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, (short)36, (short)88, (short)38, (short)90, - (short)1, (short)92, -
(short)1, (short)43, - (short)1, - (short)1, (short)97, (short)47, (short)99,
(short)100, - (short)1, - (short)1, (short)3, (short)4, (short)5, (short)6,
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
- (short)1, - (short)1, - (short)1, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)20, (short)21, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, (short)36, - (short)1, (short)88,
- (short)1, (short)90, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)47, - (short)1, (short)99, (short)100, (short)25, - (short)1,
(short)27, (short)28, (short)29, (short)30, (short)31, (short)32, (short)33,
(short)34, (short)35, - (short)1, (short)37, - (short)1, (short)39, (short)40, -
(short)1, (short)42, - (short)1, (short)44, - (short)1, - (short)1, - (short)1,
(short)48, (short)49, (short)50, (short)51, (short)52, (short)53, (short)54,
(short)55, (short)56, (short)57, (short)58, - (short)1, (short)60, (short)61,
(short)88, - (short)1, (short)90, - (short)1, - (short)1, (short)93, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)99, (short)100, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, (short)83, (short)84, (short)85, (short)86, (short)87, - (short)1,
(short)89, - (short)1, - (short)1, (short)92, (short)93, - (short)1, - (short)1,
(short)96, - (short)1, - (short)1, - (short)1, (short)100, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)106, - (short)1, - (short)1, (short)109,
- (short)1, (short)111, (short)112, - (short)1, - (short)1, - (short)1, -
(short)1, (short)117, (short)118, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)20, (short)21, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, (short)36, - (short)1, - (short)1,
(short)30, - (short)1, - (short)1, - (short)1, - (short)1, (short)35, - (short)1,
(short)37, (short)47, (short)39, - (short)1, - (short)1, (short)42, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49, - (short)1,
- (short)1, (short)52, (short)53, (short)54, (short)55, (short)56, (short)57,
(short)58, - (short)1, (short)60, (short)61, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)88, - (short)1, (short)90, - (short)1, (short)83, (short)84,
(short)85, (short)86, (short)87, - (short)1, (short)89, (short)99, (short)100, -
(short)1, (short)93, (short)94, - (short)1, (short)96, - (short)1, - (short)1, -
(short)1, (short)100, - (short)1, - (short)1, - (short)1, (short)104, - (short)1,
(short)106, - (short)1, - (short)1, (short)109, (short)110, (short)111, (short)112,
- (short)1, - (short)1, - (short)1, - (short)1, (short)117, (short)118, (short)35,
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
- (short)1, (short)93, - (short)1, - (short)1, (short)96, - (short)1, - (short)1,
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
static struct Cyc_YYINITIALSVAL_struct _temp1911={ Cyc_YYINITIALSVAL_tag};
struct _xenum_struct* Cyc_yylval=( struct _xenum_struct*)& _temp1911; static int
Cyc_yynerrs= 0; int Cyc_yyparse(){ int yystate; int yyn= 0; int yyerrstatus; int
yychar1= 0; int yyssp_offset; struct _tagged_ptr1 yyss=({ unsigned int _temp1912=(
unsigned int) 200; short* _temp1913=( short*) GC_malloc_atomic( sizeof( short) *
_temp1912); unsigned int i; struct _tagged_ptr1 _temp1914={ _temp1913, _temp1913,
_temp1913 + _temp1912}; for( i= 0; i < _temp1912; i ++){ _temp1913[ i]= (short)0;}
_temp1914;}); int yyvsp_offset; struct _tagged_ptr2 yyvs=({ unsigned int
_temp1915=( unsigned int) 200; struct _xenum_struct** _temp1916=( struct
_xenum_struct**) GC_malloc( sizeof( struct _xenum_struct*) * _temp1915);
unsigned int i; struct _tagged_ptr2 _temp1917={ _temp1916, _temp1916, _temp1916
+ _temp1915}; for( i= 0; i < _temp1915; i ++){ _temp1916[ i]= Cyc_yylval;}
_temp1917;}); int yylsp_offset; struct _tagged_ptr3 yyls=({ unsigned int
_temp1918=( unsigned int) 200; struct Cyc_Yyltype* _temp1919=( struct Cyc_Yyltype*)
GC_malloc( sizeof( struct Cyc_Yyltype) * _temp1918); unsigned int i; struct
_tagged_ptr3 _temp1920={ _temp1919, _temp1919, _temp1919 + _temp1918}; for( i= 0;
i < _temp1918; i ++){ _temp1919[ i]= Cyc_yynewloc();} _temp1920;}); int
yystacksize= 200; struct _xenum_struct* yyval= Cyc_yylval; int yylen; yystate= 0;
yyerrstatus= 0; Cyc_yynerrs= 0; Cyc_yychar= - 2; yyssp_offset= - 1; yyvsp_offset=
0; yylsp_offset= 0; yynewstate:({ struct _tagged_ptr1 _temp1921= yyss; short*
_temp1923= _temp1921.curr +( ++ yyssp_offset); if( _temp1923 < _temp1921.base? 1:
_temp1923 >= _temp1921.last_plus_one){ _throw( Null_Exception);}* _temp1923=(
short) yystate;}); if( yyssp_offset >= yystacksize - 1){ if( yystacksize >=
10000){ Cyc_yyerror(( struct _tagged_string)({ char* _temp1924=( char*)"parser stack overflow";
struct _tagged_string _temp1925; _temp1925.curr= _temp1924; _temp1925.base=
_temp1924; _temp1925.last_plus_one= _temp1924 + 22; _temp1925;}));( void) _throw(({
struct Cyc_Yystack_overflow_struct* _temp1926=( struct Cyc_Yystack_overflow_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Yystack_overflow_struct));* _temp1926=(
struct Cyc_Yystack_overflow_struct){.tag= Cyc_Yystack_overflow_tag};( struct
_xenum_struct*) _temp1926;}));} yystacksize *= 2; if( yystacksize > 10000){
yystacksize= 10000;}{ struct _tagged_ptr1 yyss1=({ unsigned int _temp1927=(
unsigned int) yystacksize; short* _temp1928=( short*) GC_malloc_atomic( sizeof(
short) * _temp1927); unsigned int i; struct _tagged_ptr1 _temp1929={ _temp1928,
_temp1928, _temp1928 + _temp1927}; for( i= 0; i < _temp1927; i ++){ _temp1928[ i]=
(short)0;} _temp1929;}); struct _tagged_ptr2 yyvs1=({ unsigned int _temp1930=(
unsigned int) yystacksize; struct _xenum_struct** _temp1931=( struct
_xenum_struct**) GC_malloc( sizeof( struct _xenum_struct*) * _temp1930);
unsigned int i; struct _tagged_ptr2 _temp1932={ _temp1931, _temp1931, _temp1931
+ _temp1930}; for( i= 0; i < _temp1930; i ++){ _temp1931[ i]= Cyc_yylval;}
_temp1932;}); struct _tagged_ptr3 yyls1=({ unsigned int _temp1933=( unsigned int)
yystacksize; struct Cyc_Yyltype* _temp1934=( struct Cyc_Yyltype*) GC_malloc(
sizeof( struct Cyc_Yyltype) * _temp1933); unsigned int i; struct _tagged_ptr3
_temp1935={ _temp1934, _temp1934, _temp1934 + _temp1933}; for( i= 0; i <
_temp1933; i ++){ _temp1934[ i]= Cyc_yynewloc();} _temp1935;});{ int i= 0; for(
0; i <= yyssp_offset; i ++){({ struct _tagged_ptr1 _temp1936= yyss1; short*
_temp1938= _temp1936.curr + i; if( _temp1938 < _temp1936.base? 1: _temp1938 >=
_temp1936.last_plus_one){ _throw( Null_Exception);}* _temp1938=({ struct
_tagged_ptr1 _temp1939= yyss; short* _temp1941= _temp1939.curr + i; if(
_temp1941 < _temp1939.base? 1: _temp1941 >= _temp1939.last_plus_one){ _throw(
Null_Exception);}* _temp1941;});});({ struct _tagged_ptr2 _temp1942= yyvs1;
struct _xenum_struct** _temp1944= _temp1942.curr + i; if( _temp1944 < _temp1942.base?
1: _temp1944 >= _temp1942.last_plus_one){ _throw( Null_Exception);}* _temp1944=({
struct _tagged_ptr2 _temp1945= yyvs; struct _xenum_struct** _temp1947= _temp1945.curr
+ i; if( _temp1947 < _temp1945.base? 1: _temp1947 >= _temp1945.last_plus_one){
_throw( Null_Exception);}* _temp1947;});});({ struct _tagged_ptr3 _temp1948=
yyls1; struct Cyc_Yyltype* _temp1950= _temp1948.curr + i; if( _temp1950 <
_temp1948.base? 1: _temp1950 >= _temp1948.last_plus_one){ _throw( Null_Exception);}*
_temp1950=({ struct _tagged_ptr3 _temp1951= yyls; struct Cyc_Yyltype* _temp1953=
_temp1951.curr + i; if( _temp1953 < _temp1951.base? 1: _temp1953 >= _temp1951.last_plus_one){
_throw( Null_Exception);}* _temp1953;});});}} yyss= yyss1; yyvs= yyvs1; yyls=
yyls1;}} goto yybackup; yybackup: yyn=( int)({ short* _temp1954=( short*) Cyc_yypact;
unsigned int _temp1955= yystate; if( _temp1955 >= 715u){ _throw( Null_Exception);}
_temp1954[ _temp1955];}); if( yyn == - 32768){ goto yydefault;} if( Cyc_yychar
== - 2){ Cyc_yychar= Cyc_yylex();} if( Cyc_yychar <= 0){ yychar1= 0; Cyc_yychar=
0;} else{ yychar1=( Cyc_yychar > 0? Cyc_yychar <= 346: 0)?( int)({ short*
_temp1956=( short*) Cyc_yytranslate; unsigned int _temp1957= Cyc_yychar; if(
_temp1957 >= 347u){ _throw( Null_Exception);} _temp1956[ _temp1957];}): 221;}
yyn += yychar1; if(( yyn < 0? 1: yyn > 4281)? 1:( int)({ short* _temp1958=(
short*) Cyc_yycheck; unsigned int _temp1959= yyn; if( _temp1959 >= 4282u){
_throw( Null_Exception);} _temp1958[ _temp1959];}) != yychar1){ goto yydefault;}
yyn=( int)({ short* _temp1960=( short*) Cyc_yytable; unsigned int _temp1961= yyn;
if( _temp1961 >= 4282u){ _throw( Null_Exception);} _temp1960[ _temp1961];}); if(
yyn < 0){ if( yyn == - 32768){ goto yyerrlab;} yyn= - yyn; goto yyreduce;} else{
if( yyn == 0){ goto yyerrlab;}} if( yyn == 714){ return 0;} if( Cyc_yychar != 0){
Cyc_yychar= - 2;}({ struct _tagged_ptr2 _temp1962= yyvs; struct _xenum_struct**
_temp1964= _temp1962.curr +( ++ yyvsp_offset); if( _temp1964 < _temp1962.base? 1:
_temp1964 >= _temp1962.last_plus_one){ _throw( Null_Exception);}* _temp1964= Cyc_yylval;});({
struct _tagged_ptr3 _temp1965= yyls; struct Cyc_Yyltype* _temp1967= _temp1965.curr
+( ++ yylsp_offset); if( _temp1967 < _temp1965.base? 1: _temp1967 >= _temp1965.last_plus_one){
_throw( Null_Exception);}* _temp1967= Cyc_yylloc;}); if( yyerrstatus != 0){
yyerrstatus --;} yystate= yyn; goto yynewstate; yydefault: yyn=( int)({ short*
_temp1968=( short*) Cyc_yydefact; unsigned int _temp1969= yystate; if( _temp1969
>= 715u){ _throw( Null_Exception);} _temp1968[ _temp1969];}); if( yyn == 0){
goto yyerrlab;} yyreduce: yylen=( int)({ short* _temp1970=( short*) Cyc_yyr2;
unsigned int _temp1971= yyn; if( _temp1971 >= 381u){ _throw( Null_Exception);}
_temp1970[ _temp1971];}); if( yylen > 0){ yyval=({ struct _tagged_ptr2 _temp1972=
yyvs; struct _xenum_struct** _temp1974= _temp1972.curr +(( yyvsp_offset + 1) -
yylen); if( _temp1974 < _temp1972.base? 1: _temp1974 >= _temp1972.last_plus_one){
_throw( Null_Exception);}* _temp1974;});} switch( yyn){ case 1: _LL1975: yyval=({
struct _tagged_ptr2 _temp1977= yyvs; struct _xenum_struct** _temp1979= _temp1977.curr
+ yyvsp_offset; if( _temp1979 < _temp1977.base? 1: _temp1979 >= _temp1977.last_plus_one){
_throw( Null_Exception);}* _temp1979;}); Cyc_Parse_parse_result= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp1980= yyvs; struct _xenum_struct** _temp1982= _temp1980.curr
+ yyvsp_offset; if( _temp1982 < _temp1980.base? 1: _temp1982 >= _temp1980.last_plus_one){
_throw( Null_Exception);}* _temp1982;})); break; case 2: _LL1976: yyval=({
struct _tagged_ptr2 _temp1984= yyvs; struct _xenum_struct** _temp1986= _temp1984.curr
+ yyvsp_offset; if( _temp1986 < _temp1984.base? 1: _temp1986 >= _temp1984.last_plus_one){
_throw( Null_Exception);}* _temp1986;}); break; case 3: _LL1983: yyval=({ struct
Cyc_DeclList_tok_struct* _temp1988=( struct Cyc_DeclList_tok_struct*) GC_malloc(
sizeof( struct Cyc_DeclList_tok_struct));* _temp1988=( struct Cyc_DeclList_tok_struct){.tag=
Cyc_DeclList_tok_tag,.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp1989= yyvs; struct _xenum_struct** _temp1991= _temp1989.curr +(
yyvsp_offset - 1); if( _temp1991 < _temp1989.base? 1: _temp1991 >= _temp1989.last_plus_one){
_throw( Null_Exception);}* _temp1991;})), Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp1992= yyvs; struct _xenum_struct** _temp1994= _temp1992.curr +
yyvsp_offset; if( _temp1994 < _temp1992.base? 1: _temp1994 >= _temp1992.last_plus_one){
_throw( Null_Exception);}* _temp1994;})))};( struct _xenum_struct*) _temp1988;});
break; case 4: _LL1987: yyval=({ struct Cyc_DeclList_tok_struct* _temp1996=(
struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));*
_temp1996=( struct Cyc_DeclList_tok_struct){.tag= Cyc_DeclList_tok_tag,.f1=({
struct Cyc_List_List* _temp1997=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1997->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1998=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1998->r=( void*)({ struct Cyc_Absyn_Using_d_struct* _temp2005=( struct Cyc_Absyn_Using_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct)); _temp2005->tag= Cyc_Absyn_Using_d_tag;
_temp2005->f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp2006= yyvs;
struct _xenum_struct** _temp2008= _temp2006.curr +( yyvsp_offset - 2); if(
_temp2008 < _temp2006.base? 1: _temp2008 >= _temp2006.last_plus_one){ _throw(
Null_Exception);}* _temp2008;})); _temp2005->f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2009= yyvs; struct _xenum_struct** _temp2011= _temp2009.curr
+ yyvsp_offset; if( _temp2011 < _temp2009.base? 1: _temp2011 >= _temp2009.last_plus_one){
_throw( Null_Exception);}* _temp2011;}));( void*) _temp2005;}); _temp1998->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp1999= yyls; struct Cyc_Yyltype*
_temp2001= _temp1999.curr +( yylsp_offset - 2); if( _temp2001 < _temp1999.base?
1: _temp2001 >= _temp1999.last_plus_one){ _throw( Null_Exception);}* _temp2001;}).first_line,({
struct _tagged_ptr3 _temp2002= yyls; struct Cyc_Yyltype* _temp2004= _temp2002.curr
+ yylsp_offset; if( _temp2004 < _temp2002.base? 1: _temp2004 >= _temp2002.last_plus_one){
_throw( Null_Exception);}* _temp2004;}).last_line); _temp1998;}); _temp1997->tl=
0; _temp1997;})};( struct _xenum_struct*) _temp1996;}); Cyc_Lex_leave_using();
break; case 5: _LL1995: yyval=({ struct Cyc_DeclList_tok_struct* _temp2013=(
struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));*
_temp2013=( struct Cyc_DeclList_tok_struct){.tag= Cyc_DeclList_tok_tag,.f1=({
struct Cyc_List_List* _temp2014=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2014->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2018=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2018->r=( void*)({ struct Cyc_Absyn_Using_d_struct* _temp2025=( struct Cyc_Absyn_Using_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct)); _temp2025->tag= Cyc_Absyn_Using_d_tag;
_temp2025->f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp2026= yyvs;
struct _xenum_struct** _temp2028= _temp2026.curr +( yyvsp_offset - 4); if(
_temp2028 < _temp2026.base? 1: _temp2028 >= _temp2026.last_plus_one){ _throw(
Null_Exception);}* _temp2028;})); _temp2025->f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2029= yyvs; struct _xenum_struct** _temp2031= _temp2029.curr
+( yyvsp_offset - 2); if( _temp2031 < _temp2029.base? 1: _temp2031 >= _temp2029.last_plus_one){
_throw( Null_Exception);}* _temp2031;}));( void*) _temp2025;}); _temp2018->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2019= yyls; struct Cyc_Yyltype*
_temp2021= _temp2019.curr +( yylsp_offset - 4); if( _temp2021 < _temp2019.base?
1: _temp2021 >= _temp2019.last_plus_one){ _throw( Null_Exception);}* _temp2021;}).first_line,({
struct _tagged_ptr3 _temp2022= yyls; struct Cyc_Yyltype* _temp2024= _temp2022.curr
+( yylsp_offset - 1); if( _temp2024 < _temp2022.base? 1: _temp2024 >= _temp2022.last_plus_one){
_throw( Null_Exception);}* _temp2024;}).last_line); _temp2018;}); _temp2014->tl=
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2015= yyvs; struct
_xenum_struct** _temp2017= _temp2015.curr + yyvsp_offset; if( _temp2017 <
_temp2015.base? 1: _temp2017 >= _temp2015.last_plus_one){ _throw( Null_Exception);}*
_temp2017;})); _temp2014;})};( struct _xenum_struct*) _temp2013;}); break; case
6: _LL2012: yyval=({ struct Cyc_DeclList_tok_struct* _temp2033=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));* _temp2033=( struct Cyc_DeclList_tok_struct){.tag=
Cyc_DeclList_tok_tag,.f1=({ struct Cyc_List_List* _temp2034=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2034->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2035=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2035->r=( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp2042=( struct
Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp2042->tag= Cyc_Absyn_Namespace_d_tag; _temp2042->f1=({ struct
_tagged_string* _temp2043=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp2043[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp2044= yyvs; struct _xenum_struct** _temp2046= _temp2044.curr +(
yyvsp_offset - 2); if( _temp2046 < _temp2044.base? 1: _temp2046 >= _temp2044.last_plus_one){
_throw( Null_Exception);}* _temp2046;})); _temp2043;}); _temp2042->f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2047= yyvs; struct _xenum_struct** _temp2049= _temp2047.curr
+ yyvsp_offset; if( _temp2049 < _temp2047.base? 1: _temp2049 >= _temp2047.last_plus_one){
_throw( Null_Exception);}* _temp2049;}));( void*) _temp2042;}); _temp2035->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2036= yyls; struct Cyc_Yyltype*
_temp2038= _temp2036.curr +( yylsp_offset - 2); if( _temp2038 < _temp2036.base?
1: _temp2038 >= _temp2036.last_plus_one){ _throw( Null_Exception);}* _temp2038;}).first_line,({
struct _tagged_ptr3 _temp2039= yyls; struct Cyc_Yyltype* _temp2041= _temp2039.curr
+ yylsp_offset; if( _temp2041 < _temp2039.base? 1: _temp2041 >= _temp2039.last_plus_one){
_throw( Null_Exception);}* _temp2041;}).last_line); _temp2035;}); _temp2034->tl=
0; _temp2034;})};( struct _xenum_struct*) _temp2033;}); Cyc_Lex_leave_namespace();
break; case 7: _LL2032: yyval=({ struct Cyc_DeclList_tok_struct* _temp2051=(
struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));*
_temp2051=( struct Cyc_DeclList_tok_struct){.tag= Cyc_DeclList_tok_tag,.f1=({
struct Cyc_List_List* _temp2052=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2052->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2056=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2056->r=( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp2063=( struct
Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp2063->tag= Cyc_Absyn_Namespace_d_tag; _temp2063->f1=({ struct
_tagged_string* _temp2064=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp2064[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp2065= yyvs; struct _xenum_struct** _temp2067= _temp2065.curr +(
yyvsp_offset - 4); if( _temp2067 < _temp2065.base? 1: _temp2067 >= _temp2065.last_plus_one){
_throw( Null_Exception);}* _temp2067;})); _temp2064;}); _temp2063->f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2068= yyvs; struct _xenum_struct** _temp2070= _temp2068.curr
+( yyvsp_offset - 2); if( _temp2070 < _temp2068.base? 1: _temp2070 >= _temp2068.last_plus_one){
_throw( Null_Exception);}* _temp2070;}));( void*) _temp2063;}); _temp2056->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2057= yyls; struct Cyc_Yyltype*
_temp2059= _temp2057.curr +( yylsp_offset - 4); if( _temp2059 < _temp2057.base?
1: _temp2059 >= _temp2057.last_plus_one){ _throw( Null_Exception);}* _temp2059;}).first_line,({
struct _tagged_ptr3 _temp2060= yyls; struct Cyc_Yyltype* _temp2062= _temp2060.curr
+( yylsp_offset - 1); if( _temp2062 < _temp2060.base? 1: _temp2062 >= _temp2060.last_plus_one){
_throw( Null_Exception);}* _temp2062;}).last_line); _temp2056;}); _temp2052->tl=
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2053= yyvs; struct
_xenum_struct** _temp2055= _temp2053.curr + yyvsp_offset; if( _temp2055 <
_temp2053.base? 1: _temp2055 >= _temp2053.last_plus_one){ _throw( Null_Exception);}*
_temp2055;})); _temp2052;})};( struct _xenum_struct*) _temp2051;}); break; case
8: _LL2050: if( Cyc_String_strcmp( Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp2072= yyvs; struct _xenum_struct** _temp2074= _temp2072.curr +(
yyvsp_offset - 4); if( _temp2074 < _temp2072.base? 1: _temp2074 >= _temp2072.last_plus_one){
_throw( Null_Exception);}* _temp2074;})),( struct _tagged_string)({ char*
_temp2075=( char*)"C"; struct _tagged_string _temp2076; _temp2076.curr=
_temp2075; _temp2076.base= _temp2075; _temp2076.last_plus_one= _temp2075 + 2;
_temp2076;})) != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp2077=(
char*)"only extern \"C\" { ... } is allowed"; struct _tagged_string _temp2078;
_temp2078.curr= _temp2077; _temp2078.base= _temp2077; _temp2078.last_plus_one=
_temp2077 + 35; _temp2078;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2079= yyls; struct Cyc_Yyltype* _temp2081= _temp2079.curr +( yylsp_offset -
5); if( _temp2081 < _temp2079.base? 1: _temp2081 >= _temp2079.last_plus_one){
_throw( Null_Exception);}* _temp2081;}).first_line,({ struct _tagged_ptr3
_temp2082= yyls; struct Cyc_Yyltype* _temp2084= _temp2082.curr +( yylsp_offset -
4); if( _temp2084 < _temp2082.base? 1: _temp2084 >= _temp2082.last_plus_one){
_throw( Null_Exception);}* _temp2084;}).last_line));} yyval=({ struct Cyc_DeclList_tok_struct*
_temp2085=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));*
_temp2085=( struct Cyc_DeclList_tok_struct){.tag= Cyc_DeclList_tok_tag,.f1=({
struct Cyc_List_List* _temp2086=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2086->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2090=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2090->r=( void*)({ struct Cyc_Absyn_ExternC_d_struct* _temp2097=( struct
Cyc_Absyn_ExternC_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ExternC_d_struct));
_temp2097->tag= Cyc_Absyn_ExternC_d_tag; _temp2097->f1= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2098= yyvs; struct _xenum_struct** _temp2100= _temp2098.curr
+( yyvsp_offset - 2); if( _temp2100 < _temp2098.base? 1: _temp2100 >= _temp2098.last_plus_one){
_throw( Null_Exception);}* _temp2100;}));( void*) _temp2097;}); _temp2090->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2091= yyls; struct Cyc_Yyltype*
_temp2093= _temp2091.curr +( yylsp_offset - 5); if( _temp2093 < _temp2091.base?
1: _temp2093 >= _temp2091.last_plus_one){ _throw( Null_Exception);}* _temp2093;}).first_line,({
struct _tagged_ptr3 _temp2094= yyls; struct Cyc_Yyltype* _temp2096= _temp2094.curr
+( yylsp_offset - 1); if( _temp2096 < _temp2094.base? 1: _temp2096 >= _temp2094.last_plus_one){
_throw( Null_Exception);}* _temp2096;}).last_line); _temp2090;}); _temp2086->tl=
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2087= yyvs; struct
_xenum_struct** _temp2089= _temp2087.curr + yyvsp_offset; if( _temp2089 <
_temp2087.base? 1: _temp2089 >= _temp2087.last_plus_one){ _throw( Null_Exception);}*
_temp2089;})); _temp2086;})};( struct _xenum_struct*) _temp2085;}); break; case
9: _LL2071: yyval=({ struct Cyc_DeclList_tok_struct* _temp2102=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));* _temp2102=( struct Cyc_DeclList_tok_struct){.tag=
Cyc_DeclList_tok_tag,.f1= 0};( struct _xenum_struct*) _temp2102;}); break; case
10: _LL2101: yyval=({ struct _tagged_ptr2 _temp2104= yyvs; struct _xenum_struct**
_temp2106= _temp2104.curr + yyvsp_offset; if( _temp2106 < _temp2104.base? 1:
_temp2106 >= _temp2104.last_plus_one){ _throw( Null_Exception);}* _temp2106;});
break; case 11: _LL2103: yyval=({ struct Cyc_DeclList_tok_struct* _temp2108=(
struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));*
_temp2108=( struct Cyc_DeclList_tok_struct){.tag= Cyc_DeclList_tok_tag,.f1=({
struct Cyc_List_List* _temp2109=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2109->hd=( void*) Cyc_Absyn_new_decl(({ struct Cyc_Absyn_Fn_d_struct*
_temp2110=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2110->tag= Cyc_Absyn_Fn_d_tag; _temp2110->f1= Cyc_yyget_FnDecl_tok(({
struct _tagged_ptr2 _temp2111= yyvs; struct _xenum_struct** _temp2113= _temp2111.curr
+ yyvsp_offset; if( _temp2113 < _temp2111.base? 1: _temp2113 >= _temp2111.last_plus_one){
_throw( Null_Exception);}* _temp2113;}));( void*) _temp2110;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2114= yyls; struct Cyc_Yyltype* _temp2116= _temp2114.curr
+ yylsp_offset; if( _temp2116 < _temp2114.base? 1: _temp2116 >= _temp2114.last_plus_one){
_throw( Null_Exception);}* _temp2116;}).first_line,({ struct _tagged_ptr3
_temp2117= yyls; struct Cyc_Yyltype* _temp2119= _temp2117.curr + yylsp_offset;
if( _temp2119 < _temp2117.base? 1: _temp2119 >= _temp2117.last_plus_one){ _throw(
Null_Exception);}* _temp2119;}).last_line)); _temp2109->tl= 0; _temp2109;})};(
struct _xenum_struct*) _temp2108;}); break; case 12: _LL2107: yyval=({ struct
_tagged_ptr2 _temp2121= yyvs; struct _xenum_struct** _temp2123= _temp2121.curr +
yyvsp_offset; if( _temp2123 < _temp2121.base? 1: _temp2123 >= _temp2121.last_plus_one){
_throw( Null_Exception);}* _temp2123;}); break; case 13: _LL2120: yyval=({
struct Cyc_FnDecl_tok_struct* _temp2125=( struct Cyc_FnDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));* _temp2125=( struct Cyc_FnDecl_tok_struct){.tag=
Cyc_FnDecl_tok_tag,.f1= Cyc_Parse_make_function( 0, Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2126= yyvs; struct _xenum_struct** _temp2128= _temp2126.curr
+( yyvsp_offset - 1); if( _temp2128 < _temp2126.base? 1: _temp2128 >= _temp2126.last_plus_one){
_throw( Null_Exception);}* _temp2128;})), 0, Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp2129= yyvs; struct _xenum_struct** _temp2131= _temp2129.curr +
yyvsp_offset; if( _temp2131 < _temp2129.base? 1: _temp2131 >= _temp2129.last_plus_one){
_throw( Null_Exception);}* _temp2131;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2132= yyls; struct Cyc_Yyltype* _temp2134= _temp2132.curr +(
yylsp_offset - 1); if( _temp2134 < _temp2132.base? 1: _temp2134 >= _temp2132.last_plus_one){
_throw( Null_Exception);}* _temp2134;}).first_line,({ struct _tagged_ptr3
_temp2135= yyls; struct Cyc_Yyltype* _temp2137= _temp2135.curr + yylsp_offset;
if( _temp2137 < _temp2135.base? 1: _temp2137 >= _temp2135.last_plus_one){ _throw(
Null_Exception);}* _temp2137;}).last_line))};( struct _xenum_struct*) _temp2125;});
break; case 14: _LL2124: yyval=({ struct Cyc_FnDecl_tok_struct* _temp2139=(
struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));*
_temp2139=( struct Cyc_FnDecl_tok_struct){.tag= Cyc_FnDecl_tok_tag,.f1= Cyc_Parse_make_function(({
struct Cyc_Core_Opt* _temp2140=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2140->v=( void*) Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2141= yyvs; struct _xenum_struct** _temp2143= _temp2141.curr +(
yyvsp_offset - 2); if( _temp2143 < _temp2141.base? 1: _temp2143 >= _temp2141.last_plus_one){
_throw( Null_Exception);}* _temp2143;})); _temp2140;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2144= yyvs; struct _xenum_struct** _temp2146= _temp2144.curr
+( yyvsp_offset - 1); if( _temp2146 < _temp2144.base? 1: _temp2146 >= _temp2144.last_plus_one){
_throw( Null_Exception);}* _temp2146;})), 0, Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp2147= yyvs; struct _xenum_struct** _temp2149= _temp2147.curr +
yyvsp_offset; if( _temp2149 < _temp2147.base? 1: _temp2149 >= _temp2147.last_plus_one){
_throw( Null_Exception);}* _temp2149;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2150= yyls; struct Cyc_Yyltype* _temp2152= _temp2150.curr +(
yylsp_offset - 2); if( _temp2152 < _temp2150.base? 1: _temp2152 >= _temp2150.last_plus_one){
_throw( Null_Exception);}* _temp2152;}).first_line,({ struct _tagged_ptr3
_temp2153= yyls; struct Cyc_Yyltype* _temp2155= _temp2153.curr + yylsp_offset;
if( _temp2155 < _temp2153.base? 1: _temp2155 >= _temp2153.last_plus_one){ _throw(
Null_Exception);}* _temp2155;}).last_line))};( struct _xenum_struct*) _temp2139;});
break; case 15: _LL2138: yyval=({ struct Cyc_FnDecl_tok_struct* _temp2157=(
struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));*
_temp2157=( struct Cyc_FnDecl_tok_struct){.tag= Cyc_FnDecl_tok_tag,.f1= Cyc_Parse_make_function(
0, Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp2158= yyvs; struct
_xenum_struct** _temp2160= _temp2158.curr +( yyvsp_offset - 2); if( _temp2160 <
_temp2158.base? 1: _temp2160 >= _temp2158.last_plus_one){ _throw( Null_Exception);}*
_temp2160;})), Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2161= yyvs;
struct _xenum_struct** _temp2163= _temp2161.curr +( yyvsp_offset - 1); if(
_temp2163 < _temp2161.base? 1: _temp2163 >= _temp2161.last_plus_one){ _throw(
Null_Exception);}* _temp2163;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp2164= yyvs; struct _xenum_struct** _temp2166= _temp2164.curr + yyvsp_offset;
if( _temp2166 < _temp2164.base? 1: _temp2166 >= _temp2164.last_plus_one){ _throw(
Null_Exception);}* _temp2166;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2167= yyls; struct Cyc_Yyltype* _temp2169= _temp2167.curr +(
yylsp_offset - 2); if( _temp2169 < _temp2167.base? 1: _temp2169 >= _temp2167.last_plus_one){
_throw( Null_Exception);}* _temp2169;}).first_line,({ struct _tagged_ptr3
_temp2170= yyls; struct Cyc_Yyltype* _temp2172= _temp2170.curr + yylsp_offset;
if( _temp2172 < _temp2170.base? 1: _temp2172 >= _temp2170.last_plus_one){ _throw(
Null_Exception);}* _temp2172;}).last_line))};( struct _xenum_struct*) _temp2157;});
break; case 16: _LL2156: yyval=({ struct Cyc_FnDecl_tok_struct* _temp2174=(
struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));*
_temp2174=( struct Cyc_FnDecl_tok_struct){.tag= Cyc_FnDecl_tok_tag,.f1= Cyc_Parse_make_function(({
struct Cyc_Core_Opt* _temp2175=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2175->v=( void*) Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2176= yyvs; struct _xenum_struct** _temp2178= _temp2176.curr +(
yyvsp_offset - 3); if( _temp2178 < _temp2176.base? 1: _temp2178 >= _temp2176.last_plus_one){
_throw( Null_Exception);}* _temp2178;})); _temp2175;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2179= yyvs; struct _xenum_struct** _temp2181= _temp2179.curr
+( yyvsp_offset - 2); if( _temp2181 < _temp2179.base? 1: _temp2181 >= _temp2179.last_plus_one){
_throw( Null_Exception);}* _temp2181;})), Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp2182= yyvs; struct _xenum_struct** _temp2184= _temp2182.curr +(
yyvsp_offset - 1); if( _temp2184 < _temp2182.base? 1: _temp2184 >= _temp2182.last_plus_one){
_throw( Null_Exception);}* _temp2184;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp2185= yyvs; struct _xenum_struct** _temp2187= _temp2185.curr +
yyvsp_offset; if( _temp2187 < _temp2185.base? 1: _temp2187 >= _temp2185.last_plus_one){
_throw( Null_Exception);}* _temp2187;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2188= yyls; struct Cyc_Yyltype* _temp2190= _temp2188.curr +(
yylsp_offset - 3); if( _temp2190 < _temp2188.base? 1: _temp2190 >= _temp2188.last_plus_one){
_throw( Null_Exception);}* _temp2190;}).first_line,({ struct _tagged_ptr3
_temp2191= yyls; struct Cyc_Yyltype* _temp2193= _temp2191.curr + yylsp_offset;
if( _temp2193 < _temp2191.base? 1: _temp2193 >= _temp2191.last_plus_one){ _throw(
Null_Exception);}* _temp2193;}).last_line))};( struct _xenum_struct*) _temp2174;});
break; case 17: _LL2173: yyval=({ struct Cyc_FnDecl_tok_struct* _temp2195=(
struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));*
_temp2195=( struct Cyc_FnDecl_tok_struct){.tag= Cyc_FnDecl_tok_tag,.f1= Cyc_Parse_make_function(({
struct Cyc_Core_Opt* _temp2196=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2196->v=( void*) Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2197= yyvs; struct _xenum_struct** _temp2199= _temp2197.curr +(
yyvsp_offset - 2); if( _temp2199 < _temp2197.base? 1: _temp2199 >= _temp2197.last_plus_one){
_throw( Null_Exception);}* _temp2199;})); _temp2196;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2200= yyvs; struct _xenum_struct** _temp2202= _temp2200.curr
+( yyvsp_offset - 1); if( _temp2202 < _temp2200.base? 1: _temp2202 >= _temp2200.last_plus_one){
_throw( Null_Exception);}* _temp2202;})), 0, Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp2203= yyvs; struct _xenum_struct** _temp2205= _temp2203.curr +
yyvsp_offset; if( _temp2205 < _temp2203.base? 1: _temp2205 >= _temp2203.last_plus_one){
_throw( Null_Exception);}* _temp2205;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2206= yyls; struct Cyc_Yyltype* _temp2208= _temp2206.curr +(
yylsp_offset - 2); if( _temp2208 < _temp2206.base? 1: _temp2208 >= _temp2206.last_plus_one){
_throw( Null_Exception);}* _temp2208;}).first_line,({ struct _tagged_ptr3
_temp2209= yyls; struct Cyc_Yyltype* _temp2211= _temp2209.curr + yylsp_offset;
if( _temp2211 < _temp2209.base? 1: _temp2211 >= _temp2209.last_plus_one){ _throw(
Null_Exception);}* _temp2211;}).last_line))};( struct _xenum_struct*) _temp2195;});
break; case 18: _LL2194: yyval=({ struct Cyc_FnDecl_tok_struct* _temp2213=(
struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));*
_temp2213=( struct Cyc_FnDecl_tok_struct){.tag= Cyc_FnDecl_tok_tag,.f1= Cyc_Parse_make_function(({
struct Cyc_Core_Opt* _temp2214=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2214->v=( void*) Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2215= yyvs; struct _xenum_struct** _temp2217= _temp2215.curr +(
yyvsp_offset - 3); if( _temp2217 < _temp2215.base? 1: _temp2217 >= _temp2215.last_plus_one){
_throw( Null_Exception);}* _temp2217;})); _temp2214;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2218= yyvs; struct _xenum_struct** _temp2220= _temp2218.curr
+( yyvsp_offset - 2); if( _temp2220 < _temp2218.base? 1: _temp2220 >= _temp2218.last_plus_one){
_throw( Null_Exception);}* _temp2220;})), Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp2221= yyvs; struct _xenum_struct** _temp2223= _temp2221.curr +(
yyvsp_offset - 1); if( _temp2223 < _temp2221.base? 1: _temp2223 >= _temp2221.last_plus_one){
_throw( Null_Exception);}* _temp2223;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp2224= yyvs; struct _xenum_struct** _temp2226= _temp2224.curr +
yyvsp_offset; if( _temp2226 < _temp2224.base? 1: _temp2226 >= _temp2224.last_plus_one){
_throw( Null_Exception);}* _temp2226;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2227= yyls; struct Cyc_Yyltype* _temp2229= _temp2227.curr +(
yylsp_offset - 3); if( _temp2229 < _temp2227.base? 1: _temp2229 >= _temp2227.last_plus_one){
_throw( Null_Exception);}* _temp2229;}).first_line,({ struct _tagged_ptr3
_temp2230= yyls; struct Cyc_Yyltype* _temp2232= _temp2230.curr + yylsp_offset;
if( _temp2232 < _temp2230.base? 1: _temp2232 >= _temp2230.last_plus_one){ _throw(
Null_Exception);}* _temp2232;}).last_line))};( struct _xenum_struct*) _temp2213;});
break; case 19: _LL2212: Cyc_Lex_enter_using( Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp2234= yyvs; struct _xenum_struct** _temp2236= _temp2234.curr +
yyvsp_offset; if( _temp2236 < _temp2234.base? 1: _temp2236 >= _temp2234.last_plus_one){
_throw( Null_Exception);}* _temp2236;}))); yyval=({ struct _tagged_ptr2
_temp2237= yyvs; struct _xenum_struct** _temp2239= _temp2237.curr + yyvsp_offset;
if( _temp2239 < _temp2237.base? 1: _temp2239 >= _temp2237.last_plus_one){ _throw(
Null_Exception);}* _temp2239;}); break; case 20: _LL2233: Cyc_Lex_leave_using();
break; case 21: _LL2240: Cyc_Lex_enter_namespace(({ struct _tagged_string*
_temp2242=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp2242[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2243= yyvs;
struct _xenum_struct** _temp2245= _temp2243.curr + yyvsp_offset; if( _temp2245 <
_temp2243.base? 1: _temp2245 >= _temp2243.last_plus_one){ _throw( Null_Exception);}*
_temp2245;})); _temp2242;})); yyval=({ struct _tagged_ptr2 _temp2246= yyvs;
struct _xenum_struct** _temp2248= _temp2246.curr + yyvsp_offset; if( _temp2248 <
_temp2246.base? 1: _temp2248 >= _temp2246.last_plus_one){ _throw( Null_Exception);}*
_temp2248;}); break; case 22: _LL2241: Cyc_Lex_leave_namespace(); break; case 23:
_LL2249: yyval=({ struct Cyc_DeclList_tok_struct* _temp2251=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));* _temp2251=( struct Cyc_DeclList_tok_struct){.tag=
Cyc_DeclList_tok_tag,.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2252= yyvs; struct _xenum_struct** _temp2254= _temp2252.curr
+( yyvsp_offset - 1); if( _temp2254 < _temp2252.base? 1: _temp2254 >= _temp2252.last_plus_one){
_throw( Null_Exception);}* _temp2254;})), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2255= yyls; struct Cyc_Yyltype* _temp2257= _temp2255.curr
+( yylsp_offset - 1); if( _temp2257 < _temp2255.base? 1: _temp2257 >= _temp2255.last_plus_one){
_throw( Null_Exception);}* _temp2257;}).first_line,({ struct _tagged_ptr3
_temp2258= yyls; struct Cyc_Yyltype* _temp2260= _temp2258.curr +( yylsp_offset -
1); if( _temp2260 < _temp2258.base? 1: _temp2260 >= _temp2258.last_plus_one){
_throw( Null_Exception);}* _temp2260;}).last_line))};( struct _xenum_struct*)
_temp2251;}); break; case 24: _LL2250: yyval=({ struct Cyc_DeclList_tok_struct*
_temp2262=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));*
_temp2262=( struct Cyc_DeclList_tok_struct){.tag= Cyc_DeclList_tok_tag,.f1= Cyc_Parse_make_declarations(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2263= yyvs; struct
_xenum_struct** _temp2265= _temp2263.curr +( yyvsp_offset - 2); if( _temp2265 <
_temp2263.base? 1: _temp2265 >= _temp2263.last_plus_one){ _throw( Null_Exception);}*
_temp2265;})), Cyc_yyget_InitDeclList_tok(({ struct _tagged_ptr2 _temp2266= yyvs;
struct _xenum_struct** _temp2268= _temp2266.curr +( yyvsp_offset - 1); if(
_temp2268 < _temp2266.base? 1: _temp2268 >= _temp2266.last_plus_one){ _throw(
Null_Exception);}* _temp2268;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2269= yyls; struct Cyc_Yyltype* _temp2271= _temp2269.curr +(
yylsp_offset - 2); if( _temp2271 < _temp2269.base? 1: _temp2271 >= _temp2269.last_plus_one){
_throw( Null_Exception);}* _temp2271;}).first_line,({ struct _tagged_ptr3
_temp2272= yyls; struct Cyc_Yyltype* _temp2274= _temp2272.curr + yylsp_offset;
if( _temp2274 < _temp2272.base? 1: _temp2274 >= _temp2272.last_plus_one){ _throw(
Null_Exception);}* _temp2274;}).last_line))};( struct _xenum_struct*) _temp2262;});
break; case 25: _LL2261: yyval=({ struct Cyc_DeclList_tok_struct* _temp2276=(
struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));*
_temp2276=( struct Cyc_DeclList_tok_struct){.tag= Cyc_DeclList_tok_tag,.f1=({
struct Cyc_List_List* _temp2277=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2277->hd=( void*) Cyc_Absyn_let_decl( Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp2278= yyvs; struct _xenum_struct** _temp2280= _temp2278.curr
+( yyvsp_offset - 3); if( _temp2280 < _temp2278.base? 1: _temp2280 >= _temp2278.last_plus_one){
_throw( Null_Exception);}* _temp2280;})), 0, Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp2281= yyvs; struct _xenum_struct** _temp2283= _temp2281.curr +(
yyvsp_offset - 1); if( _temp2283 < _temp2281.base? 1: _temp2283 >= _temp2281.last_plus_one){
_throw( Null_Exception);}* _temp2283;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2284= yyls; struct Cyc_Yyltype* _temp2286= _temp2284.curr +(
yylsp_offset - 4); if( _temp2286 < _temp2284.base? 1: _temp2286 >= _temp2284.last_plus_one){
_throw( Null_Exception);}* _temp2286;}).first_line,({ struct _tagged_ptr3
_temp2287= yyls; struct Cyc_Yyltype* _temp2289= _temp2287.curr + yylsp_offset;
if( _temp2289 < _temp2287.base? 1: _temp2289 >= _temp2287.last_plus_one){ _throw(
Null_Exception);}* _temp2289;}).last_line)); _temp2277->tl= 0; _temp2277;})};(
struct _xenum_struct*) _temp2276;}); break; case 26: _LL2275: yyval=({ struct
_tagged_ptr2 _temp2291= yyvs; struct _xenum_struct** _temp2293= _temp2291.curr +
yyvsp_offset; if( _temp2293 < _temp2291.base? 1: _temp2293 >= _temp2291.last_plus_one){
_throw( Null_Exception);}* _temp2293;}); break; case 27: _LL2290: yyval=({
struct Cyc_DeclList_tok_struct* _temp2295=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));* _temp2295=( struct Cyc_DeclList_tok_struct){.tag=
Cyc_DeclList_tok_tag,.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp2296= yyvs; struct _xenum_struct** _temp2298= _temp2296.curr +(
yyvsp_offset - 1); if( _temp2298 < _temp2296.base? 1: _temp2298 >= _temp2296.last_plus_one){
_throw( Null_Exception);}* _temp2298;})), Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp2299= yyvs; struct _xenum_struct** _temp2301= _temp2299.curr +
yyvsp_offset; if( _temp2301 < _temp2299.base? 1: _temp2301 >= _temp2299.last_plus_one){
_throw( Null_Exception);}* _temp2301;})))};( struct _xenum_struct*) _temp2295;});
break; case 28: _LL2294: yyval=({ struct Cyc_DeclSpec_tok_struct* _temp2303=(
struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));*
_temp2303=( struct Cyc_DeclSpec_tok_struct){.tag= Cyc_DeclSpec_tok_tag,.f1=({
struct Cyc_Parse_Declaration_spec* _temp2304=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2304->sc=({ struct
Cyc_Core_Opt* _temp2308=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2308->v=( void*) Cyc_yyget_StorageClass_tok(({ struct _tagged_ptr2
_temp2309= yyvs; struct _xenum_struct** _temp2311= _temp2309.curr +(
yyvsp_offset - 1); if( _temp2311 < _temp2309.base? 1: _temp2311 >= _temp2309.last_plus_one){
_throw( Null_Exception);}* _temp2311;})); _temp2308;}); _temp2304->tq= Cyc_Absyn_empty_tqual();
_temp2304->type_specs= 0; _temp2304->is_inline= 0; _temp2304->attributes= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2305= yyvs; struct _xenum_struct** _temp2307= _temp2305.curr
+ yyvsp_offset; if( _temp2307 < _temp2305.base? 1: _temp2307 >= _temp2305.last_plus_one){
_throw( Null_Exception);}* _temp2307;})); _temp2304;})};( struct _xenum_struct*)
_temp2303;}); break; case 29: _LL2302: if(( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2313= yyvs; struct _xenum_struct** _temp2315= _temp2313.curr +
yyvsp_offset; if( _temp2315 < _temp2313.base? 1: _temp2315 >= _temp2313.last_plus_one){
_throw( Null_Exception);}* _temp2315;})))->sc != 0){ Cyc_Parse_warn(( struct
_tagged_string)({ char* _temp2316=( char*)"Only one storage class is allowed in a declaration";
struct _tagged_string _temp2317; _temp2317.curr= _temp2316; _temp2317.base=
_temp2316; _temp2317.last_plus_one= _temp2316 + 51; _temp2317;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2318= yyls; struct Cyc_Yyltype* _temp2320= _temp2318.curr
+( yylsp_offset - 2); if( _temp2320 < _temp2318.base? 1: _temp2320 >= _temp2318.last_plus_one){
_throw( Null_Exception);}* _temp2320;}).first_line,({ struct _tagged_ptr3
_temp2321= yyls; struct Cyc_Yyltype* _temp2323= _temp2321.curr +( yylsp_offset -
1); if( _temp2323 < _temp2321.base? 1: _temp2323 >= _temp2321.last_plus_one){
_throw( Null_Exception);}* _temp2323;}).last_line));} yyval=({ struct Cyc_DeclSpec_tok_struct*
_temp2324=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));*
_temp2324=( struct Cyc_DeclSpec_tok_struct){.tag= Cyc_DeclSpec_tok_tag,.f1=({
struct Cyc_Parse_Declaration_spec* _temp2325=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2325->sc=({ struct
Cyc_Core_Opt* _temp2341=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2341->v=( void*) Cyc_yyget_StorageClass_tok(({ struct _tagged_ptr2
_temp2342= yyvs; struct _xenum_struct** _temp2344= _temp2342.curr +(
yyvsp_offset - 2); if( _temp2344 < _temp2342.base? 1: _temp2344 >= _temp2342.last_plus_one){
_throw( Null_Exception);}* _temp2344;})); _temp2341;}); _temp2325->tq=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2338= yyvs; struct _xenum_struct** _temp2340= _temp2338.curr
+ yyvsp_offset; if( _temp2340 < _temp2338.base? 1: _temp2340 >= _temp2338.last_plus_one){
_throw( Null_Exception);}* _temp2340;})))->tq; _temp2325->type_specs=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2335= yyvs; struct _xenum_struct** _temp2337= _temp2335.curr
+ yyvsp_offset; if( _temp2337 < _temp2335.base? 1: _temp2337 >= _temp2335.last_plus_one){
_throw( Null_Exception);}* _temp2337;})))->type_specs; _temp2325->is_inline=(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2332= yyvs; struct
_xenum_struct** _temp2334= _temp2332.curr + yyvsp_offset; if( _temp2334 <
_temp2332.base? 1: _temp2334 >= _temp2332.last_plus_one){ _throw( Null_Exception);}*
_temp2334;})))->is_inline; _temp2325->attributes=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2326= yyvs; struct _xenum_struct** _temp2328= _temp2326.curr
+( yyvsp_offset - 1); if( _temp2328 < _temp2326.base? 1: _temp2328 >= _temp2326.last_plus_one){
_throw( Null_Exception);}* _temp2328;})),( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2329= yyvs; struct _xenum_struct** _temp2331= _temp2329.curr +
yyvsp_offset; if( _temp2331 < _temp2329.base? 1: _temp2331 >= _temp2329.last_plus_one){
_throw( Null_Exception);}* _temp2331;})))->attributes); _temp2325;})};( struct
_xenum_struct*) _temp2324;}); break; case 30: _LL2312: yyval=({ struct Cyc_DeclSpec_tok_struct*
_temp2346=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));*
_temp2346=( struct Cyc_DeclSpec_tok_struct){.tag= Cyc_DeclSpec_tok_tag,.f1=({
struct Cyc_Parse_Declaration_spec* _temp2347=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2347->sc= 0;
_temp2347->tq= Cyc_Absyn_empty_tqual(); _temp2347->type_specs=({ struct Cyc_List_List*
_temp2351=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2351->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({ struct _tagged_ptr2
_temp2352= yyvs; struct _xenum_struct** _temp2354= _temp2352.curr +(
yyvsp_offset - 1); if( _temp2354 < _temp2352.base? 1: _temp2354 >= _temp2352.last_plus_one){
_throw( Null_Exception);}* _temp2354;})); _temp2351->tl= 0; _temp2351;});
_temp2347->is_inline= 0; _temp2347->attributes= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2348= yyvs; struct _xenum_struct** _temp2350= _temp2348.curr
+ yyvsp_offset; if( _temp2350 < _temp2348.base? 1: _temp2350 >= _temp2348.last_plus_one){
_throw( Null_Exception);}* _temp2350;})); _temp2347;})};( struct _xenum_struct*)
_temp2346;}); break; case 31: _LL2345: yyval=({ struct Cyc_DeclSpec_tok_struct*
_temp2356=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));*
_temp2356=( struct Cyc_DeclSpec_tok_struct){.tag= Cyc_DeclSpec_tok_tag,.f1=({
struct Cyc_Parse_Declaration_spec* _temp2357=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2357->sc=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2377= yyvs; struct _xenum_struct** _temp2379= _temp2377.curr
+ yyvsp_offset; if( _temp2379 < _temp2377.base? 1: _temp2379 >= _temp2377.last_plus_one){
_throw( Null_Exception);}* _temp2379;})))->sc; _temp2357->tq=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2374= yyvs; struct _xenum_struct** _temp2376= _temp2374.curr
+ yyvsp_offset; if( _temp2376 < _temp2374.base? 1: _temp2376 >= _temp2374.last_plus_one){
_throw( Null_Exception);}* _temp2376;})))->tq; _temp2357->type_specs=({ struct
Cyc_List_List* _temp2367=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2367->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({ struct _tagged_ptr2
_temp2371= yyvs; struct _xenum_struct** _temp2373= _temp2371.curr +(
yyvsp_offset - 2); if( _temp2373 < _temp2371.base? 1: _temp2373 >= _temp2371.last_plus_one){
_throw( Null_Exception);}* _temp2373;})); _temp2367->tl=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2368= yyvs; struct _xenum_struct** _temp2370= _temp2368.curr
+ yyvsp_offset; if( _temp2370 < _temp2368.base? 1: _temp2370 >= _temp2368.last_plus_one){
_throw( Null_Exception);}* _temp2370;})))->type_specs; _temp2367;}); _temp2357->is_inline=(
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
_throw( Null_Exception);}* _temp2363;})))->attributes); _temp2357;})};( struct
_xenum_struct*) _temp2356;}); break; case 32: _LL2355: yyval=({ struct Cyc_DeclSpec_tok_struct*
_temp2381=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));*
_temp2381=( struct Cyc_DeclSpec_tok_struct){.tag= Cyc_DeclSpec_tok_tag,.f1=({
struct Cyc_Parse_Declaration_spec* _temp2382=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2382->sc= 0;
_temp2382->tq= Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2 _temp2386= yyvs;
struct _xenum_struct** _temp2388= _temp2386.curr +( yyvsp_offset - 1); if(
_temp2388 < _temp2386.base? 1: _temp2388 >= _temp2386.last_plus_one){ _throw(
Null_Exception);}* _temp2388;})); _temp2382->type_specs= 0; _temp2382->is_inline=
0; _temp2382->attributes= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2
_temp2383= yyvs; struct _xenum_struct** _temp2385= _temp2383.curr + yyvsp_offset;
if( _temp2385 < _temp2383.base? 1: _temp2385 >= _temp2383.last_plus_one){ _throw(
Null_Exception);}* _temp2385;})); _temp2382;})};( struct _xenum_struct*)
_temp2381;}); break; case 33: _LL2380: yyval=({ struct Cyc_DeclSpec_tok_struct*
_temp2390=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));*
_temp2390=( struct Cyc_DeclSpec_tok_struct){.tag= Cyc_DeclSpec_tok_tag,.f1=({
struct Cyc_Parse_Declaration_spec* _temp2391=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2391->sc=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2410= yyvs; struct _xenum_struct** _temp2412= _temp2410.curr
+ yyvsp_offset; if( _temp2412 < _temp2410.base? 1: _temp2412 >= _temp2410.last_plus_one){
_throw( Null_Exception);}* _temp2412;})))->sc; _temp2391->tq= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2 _temp2404= yyvs; struct
_xenum_struct** _temp2406= _temp2404.curr +( yyvsp_offset - 2); if( _temp2406 <
_temp2404.base? 1: _temp2406 >= _temp2404.last_plus_one){ _throw( Null_Exception);}*
_temp2406;})),( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2407= yyvs;
struct _xenum_struct** _temp2409= _temp2407.curr + yyvsp_offset; if( _temp2409 <
_temp2407.base? 1: _temp2409 >= _temp2407.last_plus_one){ _throw( Null_Exception);}*
_temp2409;})))->tq); _temp2391->type_specs=( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2401= yyvs; struct _xenum_struct** _temp2403= _temp2401.curr +
yyvsp_offset; if( _temp2403 < _temp2401.base? 1: _temp2403 >= _temp2401.last_plus_one){
_throw( Null_Exception);}* _temp2403;})))->type_specs; _temp2391->is_inline=(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2398= yyvs; struct
_xenum_struct** _temp2400= _temp2398.curr + yyvsp_offset; if( _temp2400 <
_temp2398.base? 1: _temp2400 >= _temp2398.last_plus_one){ _throw( Null_Exception);}*
_temp2400;})))->is_inline; _temp2391->attributes=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2392= yyvs; struct _xenum_struct** _temp2394= _temp2392.curr
+( yyvsp_offset - 1); if( _temp2394 < _temp2392.base? 1: _temp2394 >= _temp2392.last_plus_one){
_throw( Null_Exception);}* _temp2394;})),( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2395= yyvs; struct _xenum_struct** _temp2397= _temp2395.curr +
yyvsp_offset; if( _temp2397 < _temp2395.base? 1: _temp2397 >= _temp2395.last_plus_one){
_throw( Null_Exception);}* _temp2397;})))->attributes); _temp2391;})};( struct
_xenum_struct*) _temp2390;}); break; case 34: _LL2389: yyval=({ struct Cyc_DeclSpec_tok_struct*
_temp2414=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));*
_temp2414=( struct Cyc_DeclSpec_tok_struct){.tag= Cyc_DeclSpec_tok_tag,.f1=({
struct Cyc_Parse_Declaration_spec* _temp2415=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2415->sc= 0;
_temp2415->tq= Cyc_Absyn_empty_tqual(); _temp2415->type_specs= 0; _temp2415->is_inline=
1; _temp2415->attributes= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2
_temp2416= yyvs; struct _xenum_struct** _temp2418= _temp2416.curr + yyvsp_offset;
if( _temp2418 < _temp2416.base? 1: _temp2418 >= _temp2416.last_plus_one){ _throw(
Null_Exception);}* _temp2418;})); _temp2415;})};( struct _xenum_struct*)
_temp2414;}); break; case 35: _LL2413: yyval=({ struct Cyc_DeclSpec_tok_struct*
_temp2420=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));*
_temp2420=( struct Cyc_DeclSpec_tok_struct){.tag= Cyc_DeclSpec_tok_tag,.f1=({
struct Cyc_Parse_Declaration_spec* _temp2421=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2421->sc=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2434= yyvs; struct _xenum_struct** _temp2436= _temp2434.curr
+ yyvsp_offset; if( _temp2436 < _temp2434.base? 1: _temp2436 >= _temp2434.last_plus_one){
_throw( Null_Exception);}* _temp2436;})))->sc; _temp2421->tq=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2431= yyvs; struct _xenum_struct** _temp2433= _temp2431.curr
+ yyvsp_offset; if( _temp2433 < _temp2431.base? 1: _temp2433 >= _temp2431.last_plus_one){
_throw( Null_Exception);}* _temp2433;})))->tq; _temp2421->type_specs=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2428= yyvs; struct _xenum_struct** _temp2430= _temp2428.curr
+ yyvsp_offset; if( _temp2430 < _temp2428.base? 1: _temp2430 >= _temp2428.last_plus_one){
_throw( Null_Exception);}* _temp2430;})))->type_specs; _temp2421->is_inline= 1;
_temp2421->attributes=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2422= yyvs; struct _xenum_struct** _temp2424= _temp2422.curr
+( yyvsp_offset - 1); if( _temp2424 < _temp2422.base? 1: _temp2424 >= _temp2422.last_plus_one){
_throw( Null_Exception);}* _temp2424;})),( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2425= yyvs; struct _xenum_struct** _temp2427= _temp2425.curr +
yyvsp_offset; if( _temp2427 < _temp2425.base? 1: _temp2427 >= _temp2425.last_plus_one){
_throw( Null_Exception);}* _temp2427;})))->attributes); _temp2421;})};( struct
_xenum_struct*) _temp2420;}); break; case 36: _LL2419: yyval=({ struct Cyc_StorageClass_tok_struct*
_temp2438=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));*
_temp2438=( struct Cyc_StorageClass_tok_struct){.tag= Cyc_StorageClass_tok_tag,.f1=
Cyc_Parse_Auto_sc};( struct _xenum_struct*) _temp2438;}); break; case 37:
_LL2437: yyval=({ struct Cyc_StorageClass_tok_struct* _temp2440=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));* _temp2440=( struct Cyc_StorageClass_tok_struct){.tag=
Cyc_StorageClass_tok_tag,.f1= Cyc_Parse_Register_sc};( struct _xenum_struct*)
_temp2440;}); break; case 38: _LL2439: yyval=({ struct Cyc_StorageClass_tok_struct*
_temp2442=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));*
_temp2442=( struct Cyc_StorageClass_tok_struct){.tag= Cyc_StorageClass_tok_tag,.f1=
Cyc_Parse_Static_sc};( struct _xenum_struct*) _temp2442;}); break; case 39:
_LL2441: yyval=({ struct Cyc_StorageClass_tok_struct* _temp2444=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));* _temp2444=( struct Cyc_StorageClass_tok_struct){.tag=
Cyc_StorageClass_tok_tag,.f1= Cyc_Parse_Extern_sc};( struct _xenum_struct*)
_temp2444;}); break; case 40: _LL2443: if( Cyc_String_strcmp( Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp2446= yyvs; struct _xenum_struct** _temp2448= _temp2446.curr
+ yyvsp_offset; if( _temp2448 < _temp2446.base? 1: _temp2448 >= _temp2446.last_plus_one){
_throw( Null_Exception);}* _temp2448;})),( struct _tagged_string)({ char*
_temp2449=( char*)"C"; struct _tagged_string _temp2450; _temp2450.curr=
_temp2449; _temp2450.base= _temp2449; _temp2450.last_plus_one= _temp2449 + 2;
_temp2450;})) != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp2451=(
char*)"only extern or extern \"C\" is allowed"; struct _tagged_string _temp2452;
_temp2452.curr= _temp2451; _temp2452.base= _temp2451; _temp2452.last_plus_one=
_temp2451 + 37; _temp2452;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2453= yyls; struct Cyc_Yyltype* _temp2455= _temp2453.curr +( yylsp_offset -
1); if( _temp2455 < _temp2453.base? 1: _temp2455 >= _temp2453.last_plus_one){
_throw( Null_Exception);}* _temp2455;}).first_line,({ struct _tagged_ptr3
_temp2456= yyls; struct Cyc_Yyltype* _temp2458= _temp2456.curr + yylsp_offset;
if( _temp2458 < _temp2456.base? 1: _temp2458 >= _temp2456.last_plus_one){ _throw(
Null_Exception);}* _temp2458;}).last_line));} yyval=({ struct Cyc_StorageClass_tok_struct*
_temp2459=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));*
_temp2459=( struct Cyc_StorageClass_tok_struct){.tag= Cyc_StorageClass_tok_tag,.f1=
Cyc_Parse_ExternC_sc};( struct _xenum_struct*) _temp2459;}); break; case 41:
_LL2445: yyval=({ struct Cyc_StorageClass_tok_struct* _temp2461=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));* _temp2461=( struct Cyc_StorageClass_tok_struct){.tag=
Cyc_StorageClass_tok_tag,.f1= Cyc_Parse_Typedef_sc};( struct _xenum_struct*)
_temp2461;}); break; case 42: _LL2460: yyval=({ struct Cyc_StorageClass_tok_struct*
_temp2463=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));*
_temp2463=( struct Cyc_StorageClass_tok_struct){.tag= Cyc_StorageClass_tok_tag,.f1=
Cyc_Parse_Abstract_sc};( struct _xenum_struct*) _temp2463;}); break; case 43:
_LL2462: yyval=({ struct Cyc_AttributeList_tok_struct* _temp2465=( struct Cyc_AttributeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));* _temp2465=( struct
Cyc_AttributeList_tok_struct){.tag= Cyc_AttributeList_tok_tag,.f1= 0};( struct
_xenum_struct*) _temp2465;}); break; case 44: _LL2464: yyval=({ struct
_tagged_ptr2 _temp2467= yyvs; struct _xenum_struct** _temp2469= _temp2467.curr +
yyvsp_offset; if( _temp2469 < _temp2467.base? 1: _temp2469 >= _temp2467.last_plus_one){
_throw( Null_Exception);}* _temp2469;}); break; case 45: _LL2466: yyval=({
struct Cyc_AttributeList_tok_struct* _temp2471=( struct Cyc_AttributeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));* _temp2471=( struct
Cyc_AttributeList_tok_struct){.tag= Cyc_AttributeList_tok_tag,.f1= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2472= yyvs; struct _xenum_struct** _temp2474= _temp2472.curr
+( yyvsp_offset - 2); if( _temp2474 < _temp2472.base? 1: _temp2474 >= _temp2472.last_plus_one){
_throw( Null_Exception);}* _temp2474;}))};( struct _xenum_struct*) _temp2471;});
break; case 46: _LL2470: yyval=({ struct Cyc_AttributeList_tok_struct* _temp2476=(
struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));*
_temp2476=( struct Cyc_AttributeList_tok_struct){.tag= Cyc_AttributeList_tok_tag,.f1=({
struct Cyc_List_List* _temp2477=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2477->hd=( void*) Cyc_yyget_Attribute_tok(({ struct
_tagged_ptr2 _temp2478= yyvs; struct _xenum_struct** _temp2480= _temp2478.curr +
yyvsp_offset; if( _temp2480 < _temp2478.base? 1: _temp2480 >= _temp2478.last_plus_one){
_throw( Null_Exception);}* _temp2480;})); _temp2477->tl= 0; _temp2477;})};(
struct _xenum_struct*) _temp2476;}); break; case 47: _LL2475: yyval=({ struct
Cyc_AttributeList_tok_struct* _temp2482=( struct Cyc_AttributeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));* _temp2482=( struct
Cyc_AttributeList_tok_struct){.tag= Cyc_AttributeList_tok_tag,.f1=({ struct Cyc_List_List*
_temp2483=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2483->hd=( void*) Cyc_yyget_Attribute_tok(({ struct _tagged_ptr2 _temp2487=
yyvs; struct _xenum_struct** _temp2489= _temp2487.curr +( yyvsp_offset - 2); if(
_temp2489 < _temp2487.base? 1: _temp2489 >= _temp2487.last_plus_one){ _throw(
Null_Exception);}* _temp2489;})); _temp2483->tl= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2484= yyvs; struct _xenum_struct** _temp2486= _temp2484.curr
+ yyvsp_offset; if( _temp2486 < _temp2484.base? 1: _temp2486 >= _temp2484.last_plus_one){
_throw( Null_Exception);}* _temp2486;})); _temp2483;})};( struct _xenum_struct*)
_temp2482;}); break; case 48: _LL2481: { struct _tagged_string s= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp2491= yyvs; struct _xenum_struct** _temp2493= _temp2491.curr
+ yyvsp_offset; if( _temp2493 < _temp2491.base? 1: _temp2493 >= _temp2491.last_plus_one){
_throw( Null_Exception);}* _temp2493;})); void* a; if( Cyc_String_zstrcmp( s,(
struct _tagged_string)({ char* _temp2494=( char*)"stdcall"; struct
_tagged_string _temp2495; _temp2495.curr= _temp2494; _temp2495.base= _temp2494;
_temp2495.last_plus_one= _temp2494 + 8; _temp2495;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2496=( char*)"__stdcall__"; struct
_tagged_string _temp2497; _temp2497.curr= _temp2496; _temp2497.base= _temp2496;
_temp2497.last_plus_one= _temp2496 + 12; _temp2497;})) == 0){ a= Cyc_Absyn_Stdcall_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2498=(
char*)"cdecl"; struct _tagged_string _temp2499; _temp2499.curr= _temp2498;
_temp2499.base= _temp2498; _temp2499.last_plus_one= _temp2498 + 6; _temp2499;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2500=( char*)"__cdecl__";
struct _tagged_string _temp2501; _temp2501.curr= _temp2500; _temp2501.base=
_temp2500; _temp2501.last_plus_one= _temp2500 + 10; _temp2501;})) == 0){ a= Cyc_Absyn_Cdecl_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2502=(
char*)"noreturn"; struct _tagged_string _temp2503; _temp2503.curr= _temp2502;
_temp2503.base= _temp2502; _temp2503.last_plus_one= _temp2502 + 9; _temp2503;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2504=( char*)"__noreturn__";
struct _tagged_string _temp2505; _temp2505.curr= _temp2504; _temp2505.base=
_temp2504; _temp2505.last_plus_one= _temp2504 + 13; _temp2505;})) == 0){ a= Cyc_Absyn_Noreturn_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2506=(
char*)"noreturn"; struct _tagged_string _temp2507; _temp2507.curr= _temp2506;
_temp2507.base= _temp2506; _temp2507.last_plus_one= _temp2506 + 9; _temp2507;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2508=( char*)"__noreturn__";
struct _tagged_string _temp2509; _temp2509.curr= _temp2508; _temp2509.base=
_temp2508; _temp2509.last_plus_one= _temp2508 + 13; _temp2509;})) == 0){ a= Cyc_Absyn_Noreturn_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2510=(
char*)"__const__"; struct _tagged_string _temp2511; _temp2511.curr= _temp2510;
_temp2511.base= _temp2510; _temp2511.last_plus_one= _temp2510 + 10; _temp2511;}))
== 0){ a= Cyc_Absyn_Const_att;} else{ if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2512=( char*)"aligned"; struct _tagged_string
_temp2513; _temp2513.curr= _temp2512; _temp2513.base= _temp2512; _temp2513.last_plus_one=
_temp2512 + 8; _temp2513;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2514=( char*)"__aligned__"; struct _tagged_string
_temp2515; _temp2515.curr= _temp2514; _temp2515.base= _temp2514; _temp2515.last_plus_one=
_temp2514 + 12; _temp2515;})) == 0){ a=({ struct Cyc_Absyn_Aligned_att_struct*
_temp2516=( struct Cyc_Absyn_Aligned_att_struct*) GC_malloc_atomic( sizeof(
struct Cyc_Absyn_Aligned_att_struct)); _temp2516->tag= Cyc_Absyn_Aligned_att_tag;
_temp2516->f1= - 1;( void*) _temp2516;});} else{ if( Cyc_String_zstrcmp( s,(
struct _tagged_string)({ char* _temp2517=( char*)"packed"; struct _tagged_string
_temp2518; _temp2518.curr= _temp2517; _temp2518.base= _temp2517; _temp2518.last_plus_one=
_temp2517 + 7; _temp2518;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2519=( char*)"__packed__"; struct _tagged_string
_temp2520; _temp2520.curr= _temp2519; _temp2520.base= _temp2519; _temp2520.last_plus_one=
_temp2519 + 11; _temp2520;})) == 0){ a= Cyc_Absyn_Packed_att;} else{ if( Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2521=( char*)"shared"; struct
_tagged_string _temp2522; _temp2522.curr= _temp2521; _temp2522.base= _temp2521;
_temp2522.last_plus_one= _temp2521 + 7; _temp2522;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2523=( char*)"__shared__"; struct
_tagged_string _temp2524; _temp2524.curr= _temp2523; _temp2524.base= _temp2523;
_temp2524.last_plus_one= _temp2523 + 11; _temp2524;})) == 0){ a= Cyc_Absyn_Shared_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2525=(
char*)"unused"; struct _tagged_string _temp2526; _temp2526.curr= _temp2525;
_temp2526.base= _temp2525; _temp2526.last_plus_one= _temp2525 + 7; _temp2526;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2527=( char*)"__unused__";
struct _tagged_string _temp2528; _temp2528.curr= _temp2527; _temp2528.base=
_temp2527; _temp2528.last_plus_one= _temp2527 + 11; _temp2528;})) == 0){ a= Cyc_Absyn_Unused_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2529=(
char*)"weak"; struct _tagged_string _temp2530; _temp2530.curr= _temp2529;
_temp2530.base= _temp2529; _temp2530.last_plus_one= _temp2529 + 5; _temp2530;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2531=( char*)"__weak__";
struct _tagged_string _temp2532; _temp2532.curr= _temp2531; _temp2532.base=
_temp2531; _temp2532.last_plus_one= _temp2531 + 9; _temp2532;})) == 0){ a= Cyc_Absyn_Weak_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2533=(
char*)"dllimport"; struct _tagged_string _temp2534; _temp2534.curr= _temp2533;
_temp2534.base= _temp2533; _temp2534.last_plus_one= _temp2533 + 10; _temp2534;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2535=( char*)"__dllimport__";
struct _tagged_string _temp2536; _temp2536.curr= _temp2535; _temp2536.base=
_temp2535; _temp2536.last_plus_one= _temp2535 + 14; _temp2536;})) == 0){ a= Cyc_Absyn_Dllimport_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2537=(
char*)"dllexport"; struct _tagged_string _temp2538; _temp2538.curr= _temp2537;
_temp2538.base= _temp2537; _temp2538.last_plus_one= _temp2537 + 10; _temp2538;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2539=( char*)"__dllexport__";
struct _tagged_string _temp2540; _temp2540.curr= _temp2539; _temp2540.base=
_temp2539; _temp2540.last_plus_one= _temp2539 + 14; _temp2540;})) == 0){ a= Cyc_Absyn_Dllexport_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2541=(
char*)"no_instrument_function"; struct _tagged_string _temp2542; _temp2542.curr=
_temp2541; _temp2542.base= _temp2541; _temp2542.last_plus_one= _temp2541 + 23;
_temp2542;})) == 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char*
_temp2543=( char*)"__no_instrument_function__"; struct _tagged_string _temp2544;
_temp2544.curr= _temp2543; _temp2544.base= _temp2543; _temp2544.last_plus_one=
_temp2543 + 27; _temp2544;})) == 0){ a= Cyc_Absyn_No_instrument_function_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2545=(
char*)"constructor"; struct _tagged_string _temp2546; _temp2546.curr= _temp2545;
_temp2546.base= _temp2545; _temp2546.last_plus_one= _temp2545 + 12; _temp2546;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2547=( char*)"__constructor__";
struct _tagged_string _temp2548; _temp2548.curr= _temp2547; _temp2548.base=
_temp2547; _temp2548.last_plus_one= _temp2547 + 16; _temp2548;})) == 0){ a= Cyc_Absyn_Constructor_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2549=(
char*)"destructor"; struct _tagged_string _temp2550; _temp2550.curr= _temp2549;
_temp2550.base= _temp2549; _temp2550.last_plus_one= _temp2549 + 11; _temp2550;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2551=( char*)"__destructor__";
struct _tagged_string _temp2552; _temp2552.curr= _temp2551; _temp2552.base=
_temp2551; _temp2552.last_plus_one= _temp2551 + 15; _temp2552;})) == 0){ a= Cyc_Absyn_Destructor_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2553=(
char*)"no_check_memory_usage"; struct _tagged_string _temp2554; _temp2554.curr=
_temp2553; _temp2554.base= _temp2553; _temp2554.last_plus_one= _temp2553 + 22;
_temp2554;})) == 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char*
_temp2555=( char*)"__no_check_memory_usage__"; struct _tagged_string _temp2556;
_temp2556.curr= _temp2555; _temp2556.base= _temp2555; _temp2556.last_plus_one=
_temp2555 + 26; _temp2556;})) == 0){ a= Cyc_Absyn_No_check_memory_usage_att;}
else{ Cyc_Parse_err(( struct _tagged_string)({ char* _temp2557=( char*)"unrecognized attribute";
struct _tagged_string _temp2558; _temp2558.curr= _temp2557; _temp2558.base=
_temp2557; _temp2558.last_plus_one= _temp2557 + 23; _temp2558;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2559= yyls; struct Cyc_Yyltype* _temp2561= _temp2559.curr
+ yylsp_offset; if( _temp2561 < _temp2559.base? 1: _temp2561 >= _temp2559.last_plus_one){
_throw( Null_Exception);}* _temp2561;}).first_line,({ struct _tagged_ptr3
_temp2562= yyls; struct Cyc_Yyltype* _temp2564= _temp2562.curr + yylsp_offset;
if( _temp2564 < _temp2562.base? 1: _temp2564 >= _temp2562.last_plus_one){ _throw(
Null_Exception);}* _temp2564;}).last_line)); a= Cyc_Absyn_Cdecl_att;}}}}}}}}}}}}}}}}
yyval=({ struct Cyc_Attribute_tok_struct* _temp2565=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));* _temp2565=( struct Cyc_Attribute_tok_struct){.tag=
Cyc_Attribute_tok_tag,.f1= a};( struct _xenum_struct*) _temp2565;}); break;}
case 49: _LL2490: yyval=({ struct Cyc_Attribute_tok_struct* _temp2567=( struct
Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));*
_temp2567=( struct Cyc_Attribute_tok_struct){.tag= Cyc_Attribute_tok_tag,.f1=
Cyc_Absyn_Const_att};( struct _xenum_struct*) _temp2567;}); break; case 50:
_LL2566: { struct _tagged_string s= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp2569= yyvs; struct _xenum_struct** _temp2571= _temp2569.curr +(
yyvsp_offset - 3); if( _temp2571 < _temp2569.base? 1: _temp2571 >= _temp2569.last_plus_one){
_throw( Null_Exception);}* _temp2571;})); struct _tuple11 _temp2577; int
_temp2578; void* _temp2580; struct _tuple11* _temp2575= Cyc_yyget_Int_tok(({
struct _tagged_ptr2 _temp2572= yyvs; struct _xenum_struct** _temp2574= _temp2572.curr
+( yyvsp_offset - 1); if( _temp2574 < _temp2572.base? 1: _temp2574 >= _temp2572.last_plus_one){
_throw( Null_Exception);}* _temp2574;})); _temp2577=* _temp2575; _LL2581:
_temp2580= _temp2577.f1; goto _LL2579; _LL2579: _temp2578= _temp2577.f2; goto
_LL2576; _LL2576: { void* a; if( Cyc_String_zstrcmp( s,( struct _tagged_string)({
char* _temp2582=( char*)"regparm"; struct _tagged_string _temp2583; _temp2583.curr=
_temp2582; _temp2583.base= _temp2582; _temp2583.last_plus_one= _temp2582 + 8;
_temp2583;})) == 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char*
_temp2584=( char*)"__regparm__"; struct _tagged_string _temp2585; _temp2585.curr=
_temp2584; _temp2585.base= _temp2584; _temp2585.last_plus_one= _temp2584 + 12;
_temp2585;})) == 0){ if( _temp2578 <= 0? 1: _temp2578 > 3){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp2586=( char*)"regparm requires value between 1 and 3";
struct _tagged_string _temp2587; _temp2587.curr= _temp2586; _temp2587.base=
_temp2586; _temp2587.last_plus_one= _temp2586 + 39; _temp2587;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2588= yyls; struct Cyc_Yyltype* _temp2590= _temp2588.curr
+( yylsp_offset - 1); if( _temp2590 < _temp2588.base? 1: _temp2590 >= _temp2588.last_plus_one){
_throw( Null_Exception);}* _temp2590;}).first_line,({ struct _tagged_ptr3
_temp2591= yyls; struct Cyc_Yyltype* _temp2593= _temp2591.curr +( yylsp_offset -
1); if( _temp2593 < _temp2591.base? 1: _temp2593 >= _temp2591.last_plus_one){
_throw( Null_Exception);}* _temp2593;}).last_line));} a=({ struct Cyc_Absyn_Regparm_att_struct*
_temp2594=( struct Cyc_Absyn_Regparm_att_struct*) GC_malloc_atomic( sizeof(
struct Cyc_Absyn_Regparm_att_struct)); _temp2594->tag= Cyc_Absyn_Regparm_att_tag;
_temp2594->f1= _temp2578;( void*) _temp2594;});} else{ if( Cyc_String_zstrcmp( s,(
struct _tagged_string)({ char* _temp2595=( char*)"aligned"; struct
_tagged_string _temp2596; _temp2596.curr= _temp2595; _temp2596.base= _temp2595;
_temp2596.last_plus_one= _temp2595 + 8; _temp2596;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2597=( char*)"__aligned__"; struct
_tagged_string _temp2598; _temp2598.curr= _temp2597; _temp2598.base= _temp2597;
_temp2598.last_plus_one= _temp2597 + 12; _temp2598;})) == 0){ if( _temp2578 < 0){
Cyc_Parse_err(( struct _tagged_string)({ char* _temp2599=( char*)"aligned requires positive power of two";
struct _tagged_string _temp2600; _temp2600.curr= _temp2599; _temp2600.base=
_temp2599; _temp2600.last_plus_one= _temp2599 + 39; _temp2600;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2601= yyls; struct Cyc_Yyltype* _temp2603= _temp2601.curr
+( yylsp_offset - 1); if( _temp2603 < _temp2601.base? 1: _temp2603 >= _temp2601.last_plus_one){
_throw( Null_Exception);}* _temp2603;}).first_line,({ struct _tagged_ptr3
_temp2604= yyls; struct Cyc_Yyltype* _temp2606= _temp2604.curr +( yylsp_offset -
1); if( _temp2606 < _temp2604.base? 1: _temp2606 >= _temp2604.last_plus_one){
_throw( Null_Exception);}* _temp2606;}).last_line));}{ unsigned int j=(
unsigned int) _temp2578; for( 0;( j &( unsigned int) 1) ==( unsigned int) 0; j=
j >>( unsigned int) 1){;} j= j >>( unsigned int) 1; if( j !=( unsigned int) 0){
Cyc_Parse_err(( struct _tagged_string)({ char* _temp2607=( char*)"aligned requires positive power of two";
struct _tagged_string _temp2608; _temp2608.curr= _temp2607; _temp2608.base=
_temp2607; _temp2608.last_plus_one= _temp2607 + 39; _temp2608;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2609= yyls; struct Cyc_Yyltype* _temp2611= _temp2609.curr
+( yylsp_offset - 1); if( _temp2611 < _temp2609.base? 1: _temp2611 >= _temp2609.last_plus_one){
_throw( Null_Exception);}* _temp2611;}).first_line,({ struct _tagged_ptr3
_temp2612= yyls; struct Cyc_Yyltype* _temp2614= _temp2612.curr +( yylsp_offset -
1); if( _temp2614 < _temp2612.base? 1: _temp2614 >= _temp2612.last_plus_one){
_throw( Null_Exception);}* _temp2614;}).last_line));} a=({ struct Cyc_Absyn_Aligned_att_struct*
_temp2615=( struct Cyc_Absyn_Aligned_att_struct*) GC_malloc_atomic( sizeof(
struct Cyc_Absyn_Aligned_att_struct)); _temp2615->tag= Cyc_Absyn_Aligned_att_tag;
_temp2615->f1= _temp2578;( void*) _temp2615;});}} else{ Cyc_Parse_err(( struct
_tagged_string)({ char* _temp2616=( char*)"unrecognized attribute"; struct
_tagged_string _temp2617; _temp2617.curr= _temp2616; _temp2617.base= _temp2616;
_temp2617.last_plus_one= _temp2616 + 23; _temp2617;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2618= yyls; struct Cyc_Yyltype* _temp2620= _temp2618.curr
+( yylsp_offset - 3); if( _temp2620 < _temp2618.base? 1: _temp2620 >= _temp2618.last_plus_one){
_throw( Null_Exception);}* _temp2620;}).first_line,({ struct _tagged_ptr3
_temp2621= yyls; struct Cyc_Yyltype* _temp2623= _temp2621.curr +( yylsp_offset -
3); if( _temp2623 < _temp2621.base? 1: _temp2623 >= _temp2621.last_plus_one){
_throw( Null_Exception);}* _temp2623;}).last_line)); a= Cyc_Absyn_Cdecl_att;}}
yyval=({ struct Cyc_Attribute_tok_struct* _temp2624=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));* _temp2624=( struct Cyc_Attribute_tok_struct){.tag=
Cyc_Attribute_tok_tag,.f1= a};( struct _xenum_struct*) _temp2624;}); break;}}
case 51: _LL2568: { struct _tagged_string s= Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp2626= yyvs; struct _xenum_struct** _temp2628= _temp2626.curr +(
yyvsp_offset - 3); if( _temp2628 < _temp2626.base? 1: _temp2628 >= _temp2626.last_plus_one){
_throw( Null_Exception);}* _temp2628;})); struct _tagged_string str= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp2629= yyvs; struct _xenum_struct** _temp2631= _temp2629.curr
+( yyvsp_offset - 1); if( _temp2631 < _temp2629.base? 1: _temp2631 >= _temp2629.last_plus_one){
_throw( Null_Exception);}* _temp2631;})); void* a; if( Cyc_String_zstrcmp( s,(
struct _tagged_string)({ char* _temp2632=( char*)"section"; struct
_tagged_string _temp2633; _temp2633.curr= _temp2632; _temp2633.base= _temp2632;
_temp2633.last_plus_one= _temp2632 + 8; _temp2633;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2634=( char*)"__section__"; struct
_tagged_string _temp2635; _temp2635.curr= _temp2634; _temp2635.base= _temp2634;
_temp2635.last_plus_one= _temp2634 + 12; _temp2635;}))){ a=({ struct Cyc_Absyn_Section_att_struct*
_temp2636=( struct Cyc_Absyn_Section_att_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Section_att_struct));
_temp2636->tag= Cyc_Absyn_Section_att_tag; _temp2636->f1= str;( void*) _temp2636;});}
else{ Cyc_Parse_err(( struct _tagged_string)({ char* _temp2637=( char*)"unrecognized attribute";
struct _tagged_string _temp2638; _temp2638.curr= _temp2637; _temp2638.base=
_temp2637; _temp2638.last_plus_one= _temp2637 + 23; _temp2638;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2639= yyls; struct Cyc_Yyltype* _temp2641= _temp2639.curr
+( yylsp_offset - 3); if( _temp2641 < _temp2639.base? 1: _temp2641 >= _temp2639.last_plus_one){
_throw( Null_Exception);}* _temp2641;}).first_line,({ struct _tagged_ptr3
_temp2642= yyls; struct Cyc_Yyltype* _temp2644= _temp2642.curr +( yylsp_offset -
3); if( _temp2644 < _temp2642.base? 1: _temp2644 >= _temp2642.last_plus_one){
_throw( Null_Exception);}* _temp2644;}).last_line)); a= Cyc_Absyn_Cdecl_att;}
yyval=({ struct Cyc_Attribute_tok_struct* _temp2645=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));* _temp2645=( struct Cyc_Attribute_tok_struct){.tag=
Cyc_Attribute_tok_tag,.f1= a};( struct _xenum_struct*) _temp2645;}); break;}
case 52: _LL2625: yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2647=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2647=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec( Cyc_Absyn_VoidType, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2648= yyls; struct Cyc_Yyltype* _temp2650= _temp2648.curr +
yylsp_offset; if( _temp2650 < _temp2648.base? 1: _temp2650 >= _temp2648.last_plus_one){
_throw( Null_Exception);}* _temp2650;}).first_line,({ struct _tagged_ptr3
_temp2651= yyls; struct Cyc_Yyltype* _temp2653= _temp2651.curr + yylsp_offset;
if( _temp2653 < _temp2651.base? 1: _temp2653 >= _temp2651.last_plus_one){ _throw(
Null_Exception);}* _temp2653;}).last_line))};( struct _xenum_struct*) _temp2647;});
break; case 53: _LL2646: yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2655=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2655=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec( Cyc_Absyn_new_evar( Cyc_Absyn_MemKind), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2656= yyls; struct Cyc_Yyltype* _temp2658= _temp2656.curr
+ yylsp_offset; if( _temp2658 < _temp2656.base? 1: _temp2658 >= _temp2656.last_plus_one){
_throw( Null_Exception);}* _temp2658;}).first_line,({ struct _tagged_ptr3
_temp2659= yyls; struct Cyc_Yyltype* _temp2661= _temp2659.curr + yylsp_offset;
if( _temp2661 < _temp2659.base? 1: _temp2661 >= _temp2659.last_plus_one){ _throw(
Null_Exception);}* _temp2661;}).last_line))};( struct _xenum_struct*) _temp2655;});
break; case 54: _LL2654: yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2663=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2663=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec( Cyc_Absyn_uchar_t, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2664= yyls; struct Cyc_Yyltype* _temp2666= _temp2664.curr +
yylsp_offset; if( _temp2666 < _temp2664.base? 1: _temp2666 >= _temp2664.last_plus_one){
_throw( Null_Exception);}* _temp2666;}).first_line,({ struct _tagged_ptr3
_temp2667= yyls; struct Cyc_Yyltype* _temp2669= _temp2667.curr + yylsp_offset;
if( _temp2669 < _temp2667.base? 1: _temp2669 >= _temp2667.last_plus_one){ _throw(
Null_Exception);}* _temp2669;}).last_line))};( struct _xenum_struct*) _temp2663;});
break; case 55: _LL2662: yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2671=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2671=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=({
struct Cyc_Parse_Short_spec_struct* _temp2672=( struct Cyc_Parse_Short_spec_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Short_spec_struct)); _temp2672->tag= Cyc_Parse_Short_spec_tag;
_temp2672->f1= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2673=
yyls; struct Cyc_Yyltype* _temp2675= _temp2673.curr + yylsp_offset; if(
_temp2675 < _temp2673.base? 1: _temp2675 >= _temp2673.last_plus_one){ _throw(
Null_Exception);}* _temp2675;}).first_line,({ struct _tagged_ptr3 _temp2676=
yyls; struct Cyc_Yyltype* _temp2678= _temp2676.curr + yylsp_offset; if(
_temp2678 < _temp2676.base? 1: _temp2678 >= _temp2676.last_plus_one){ _throw(
Null_Exception);}* _temp2678;}).last_line);( void*) _temp2672;})};( struct
_xenum_struct*) _temp2671;}); break; case 56: _LL2670: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp2680=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2680=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec( Cyc_Absyn_sint_t, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2681= yyls; struct Cyc_Yyltype* _temp2683= _temp2681.curr +
yylsp_offset; if( _temp2683 < _temp2681.base? 1: _temp2683 >= _temp2681.last_plus_one){
_throw( Null_Exception);}* _temp2683;}).first_line,({ struct _tagged_ptr3
_temp2684= yyls; struct Cyc_Yyltype* _temp2686= _temp2684.curr + yylsp_offset;
if( _temp2686 < _temp2684.base? 1: _temp2686 >= _temp2684.last_plus_one){ _throw(
Null_Exception);}* _temp2686;}).last_line))};( struct _xenum_struct*) _temp2680;});
break; case 57: _LL2679: yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2688=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2688=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=({
struct Cyc_Parse_Long_spec_struct* _temp2689=( struct Cyc_Parse_Long_spec_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Long_spec_struct)); _temp2689->tag= Cyc_Parse_Long_spec_tag;
_temp2689->f1= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2690=
yyls; struct Cyc_Yyltype* _temp2692= _temp2690.curr + yylsp_offset; if(
_temp2692 < _temp2690.base? 1: _temp2692 >= _temp2690.last_plus_one){ _throw(
Null_Exception);}* _temp2692;}).first_line,({ struct _tagged_ptr3 _temp2693=
yyls; struct Cyc_Yyltype* _temp2695= _temp2693.curr + yylsp_offset; if(
_temp2695 < _temp2693.base? 1: _temp2695 >= _temp2693.last_plus_one){ _throw(
Null_Exception);}* _temp2695;}).last_line);( void*) _temp2689;})};( struct
_xenum_struct*) _temp2688;}); break; case 58: _LL2687: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp2697=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2697=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec( Cyc_Absyn_float_t, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2698= yyls; struct Cyc_Yyltype* _temp2700= _temp2698.curr +
yylsp_offset; if( _temp2700 < _temp2698.base? 1: _temp2700 >= _temp2698.last_plus_one){
_throw( Null_Exception);}* _temp2700;}).first_line,({ struct _tagged_ptr3
_temp2701= yyls; struct Cyc_Yyltype* _temp2703= _temp2701.curr + yylsp_offset;
if( _temp2703 < _temp2701.base? 1: _temp2703 >= _temp2701.last_plus_one){ _throw(
Null_Exception);}* _temp2703;}).last_line))};( struct _xenum_struct*) _temp2697;});
break; case 59: _LL2696: yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2705=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2705=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec( Cyc_Absyn_double_t, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2706= yyls; struct Cyc_Yyltype* _temp2708= _temp2706.curr +
yylsp_offset; if( _temp2708 < _temp2706.base? 1: _temp2708 >= _temp2706.last_plus_one){
_throw( Null_Exception);}* _temp2708;}).first_line,({ struct _tagged_ptr3
_temp2709= yyls; struct Cyc_Yyltype* _temp2711= _temp2709.curr + yylsp_offset;
if( _temp2711 < _temp2709.base? 1: _temp2711 >= _temp2709.last_plus_one){ _throw(
Null_Exception);}* _temp2711;}).last_line))};( struct _xenum_struct*) _temp2705;});
break; case 60: _LL2704: yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2713=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2713=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=({
struct Cyc_Parse_Signed_spec_struct* _temp2714=( struct Cyc_Parse_Signed_spec_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Signed_spec_struct)); _temp2714->tag= Cyc_Parse_Signed_spec_tag;
_temp2714->f1= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2715=
yyls; struct Cyc_Yyltype* _temp2717= _temp2715.curr + yylsp_offset; if(
_temp2717 < _temp2715.base? 1: _temp2717 >= _temp2715.last_plus_one){ _throw(
Null_Exception);}* _temp2717;}).first_line,({ struct _tagged_ptr3 _temp2718=
yyls; struct Cyc_Yyltype* _temp2720= _temp2718.curr + yylsp_offset; if(
_temp2720 < _temp2718.base? 1: _temp2720 >= _temp2718.last_plus_one){ _throw(
Null_Exception);}* _temp2720;}).last_line);( void*) _temp2714;})};( struct
_xenum_struct*) _temp2713;}); break; case 61: _LL2712: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp2722=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2722=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=({
struct Cyc_Parse_Unsigned_spec_struct* _temp2723=( struct Cyc_Parse_Unsigned_spec_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Unsigned_spec_struct)); _temp2723->tag= Cyc_Parse_Unsigned_spec_tag;
_temp2723->f1= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2724=
yyls; struct Cyc_Yyltype* _temp2726= _temp2724.curr + yylsp_offset; if(
_temp2726 < _temp2724.base? 1: _temp2726 >= _temp2724.last_plus_one){ _throw(
Null_Exception);}* _temp2726;}).first_line,({ struct _tagged_ptr3 _temp2727=
yyls; struct Cyc_Yyltype* _temp2729= _temp2727.curr + yylsp_offset; if(
_temp2729 < _temp2727.base? 1: _temp2729 >= _temp2727.last_plus_one){ _throw(
Null_Exception);}* _temp2729;}).last_line);( void*) _temp2723;})};( struct
_xenum_struct*) _temp2722;}); break; case 62: _LL2721: yyval=({ struct
_tagged_ptr2 _temp2731= yyvs; struct _xenum_struct** _temp2733= _temp2731.curr +
yyvsp_offset; if( _temp2733 < _temp2731.base? 1: _temp2733 >= _temp2731.last_plus_one){
_throw( Null_Exception);}* _temp2733;}); break; case 63: _LL2730: yyval=({
struct _tagged_ptr2 _temp2735= yyvs; struct _xenum_struct** _temp2737= _temp2735.curr
+ yyvsp_offset; if( _temp2737 < _temp2735.base? 1: _temp2737 >= _temp2735.last_plus_one){
_throw( Null_Exception);}* _temp2737;}); break; case 64: _LL2734: yyval=({
struct Cyc_TypeSpecifier_tok_struct* _temp2739=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));* _temp2739=( struct
Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1= Cyc_Parse_type_spec(
Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2740= yyvs;
struct _xenum_struct** _temp2742= _temp2740.curr + yyvsp_offset; if( _temp2742 <
_temp2740.base? 1: _temp2742 >= _temp2740.last_plus_one){ _throw( Null_Exception);}*
_temp2742;})),(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)()), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2743= yyls; struct Cyc_Yyltype* _temp2745= _temp2743.curr
+ yylsp_offset; if( _temp2745 < _temp2743.base? 1: _temp2745 >= _temp2743.last_plus_one){
_throw( Null_Exception);}* _temp2745;}).first_line,({ struct _tagged_ptr3
_temp2746= yyls; struct Cyc_Yyltype* _temp2748= _temp2746.curr + yylsp_offset;
if( _temp2748 < _temp2746.base? 1: _temp2748 >= _temp2746.last_plus_one){ _throw(
Null_Exception);}* _temp2748;}).last_line))};( struct _xenum_struct*) _temp2739;});
break; case 65: _LL2738: yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2750=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2750=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec( Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp2751= yyvs; struct _xenum_struct** _temp2753= _temp2751.curr +(
yyvsp_offset - 2); if( _temp2753 < _temp2751.base? 1: _temp2753 >= _temp2751.last_plus_one){
_throw( Null_Exception);}* _temp2753;})),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)( Cyc_yyget_Kind_tok(({ struct _tagged_ptr2 _temp2754= yyvs;
struct _xenum_struct** _temp2756= _temp2754.curr + yyvsp_offset; if( _temp2756 <
_temp2754.base? 1: _temp2756 >= _temp2754.last_plus_one){ _throw( Null_Exception);}*
_temp2756;})))), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2757=
yyls; struct Cyc_Yyltype* _temp2759= _temp2757.curr +( yylsp_offset - 2); if(
_temp2759 < _temp2757.base? 1: _temp2759 >= _temp2757.last_plus_one){ _throw(
Null_Exception);}* _temp2759;}).first_line,({ struct _tagged_ptr3 _temp2760=
yyls; struct Cyc_Yyltype* _temp2762= _temp2760.curr + yylsp_offset; if(
_temp2762 < _temp2760.base? 1: _temp2762 >= _temp2760.last_plus_one){ _throw(
Null_Exception);}* _temp2762;}).last_line))};( struct _xenum_struct*) _temp2750;});
break; case 66: _LL2749: yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2764=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2764=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec(({ struct Cyc_Absyn_TypedefType_struct* _temp2765=( struct
Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp2765->tag= Cyc_Absyn_TypedefType_tag; _temp2765->f1= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp2766= yyvs; struct _xenum_struct** _temp2768= _temp2766.curr
+( yyvsp_offset - 1); if( _temp2768 < _temp2766.base? 1: _temp2768 >= _temp2766.last_plus_one){
_throw( Null_Exception);}* _temp2768;})); _temp2765->f2= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp2769= yyvs; struct _xenum_struct** _temp2771= _temp2769.curr
+ yyvsp_offset; if( _temp2771 < _temp2769.base? 1: _temp2771 >= _temp2769.last_plus_one){
_throw( Null_Exception);}* _temp2771;})); _temp2765->f3= 0;( void*) _temp2765;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2772= yyls; struct Cyc_Yyltype*
_temp2774= _temp2772.curr +( yylsp_offset - 1); if( _temp2774 < _temp2772.base?
1: _temp2774 >= _temp2772.last_plus_one){ _throw( Null_Exception);}* _temp2774;}).first_line,({
struct _tagged_ptr3 _temp2775= yyls; struct Cyc_Yyltype* _temp2777= _temp2775.curr
+ yylsp_offset; if( _temp2777 < _temp2775.base? 1: _temp2777 >= _temp2775.last_plus_one){
_throw( Null_Exception);}* _temp2777;}).last_line))};( struct _xenum_struct*)
_temp2764;}); break; case 67: _LL2763: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp2779=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2779=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec(({ struct Cyc_Absyn_TupleType_struct* _temp2780=( struct Cyc_Absyn_TupleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp2780->tag= Cyc_Absyn_TupleType_tag;
_temp2780->f1=(( struct Cyc_List_List*(*)( struct _tuple9*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2781= yyls; struct Cyc_Yyltype* _temp2783= _temp2781.curr +(
yylsp_offset - 1); if( _temp2783 < _temp2781.base? 1: _temp2783 >= _temp2781.last_plus_one){
_throw( Null_Exception);}* _temp2783;}).first_line,({ struct _tagged_ptr3
_temp2784= yyls; struct Cyc_Yyltype* _temp2786= _temp2784.curr +( yylsp_offset -
1); if( _temp2786 < _temp2784.base? 1: _temp2786 >= _temp2784.last_plus_one){
_throw( Null_Exception);}* _temp2786;}).last_line),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp2787= yyvs; struct _xenum_struct** _temp2789= _temp2787.curr
+( yyvsp_offset - 1); if( _temp2789 < _temp2787.base? 1: _temp2789 >= _temp2787.last_plus_one){
_throw( Null_Exception);}* _temp2789;}))));( void*) _temp2780;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2790= yyls; struct Cyc_Yyltype* _temp2792= _temp2790.curr
+( yylsp_offset - 3); if( _temp2792 < _temp2790.base? 1: _temp2792 >= _temp2790.last_plus_one){
_throw( Null_Exception);}* _temp2792;}).first_line,({ struct _tagged_ptr3
_temp2793= yyls; struct Cyc_Yyltype* _temp2795= _temp2793.curr + yylsp_offset;
if( _temp2795 < _temp2793.base? 1: _temp2795 >= _temp2793.last_plus_one){ _throw(
Null_Exception);}* _temp2795;}).last_line))};( struct _xenum_struct*) _temp2779;});
break; case 68: _LL2778: yyval=({ struct Cyc_Kind_tok_struct* _temp2797=( struct
Cyc_Kind_tok_struct*) GC_malloc( sizeof( struct Cyc_Kind_tok_struct));*
_temp2797=( struct Cyc_Kind_tok_struct){.tag= Cyc_Kind_tok_tag,.f1= Cyc_Parse_id_to_kind(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2798= yyvs; struct
_xenum_struct** _temp2800= _temp2798.curr + yyvsp_offset; if( _temp2800 <
_temp2798.base? 1: _temp2800 >= _temp2798.last_plus_one){ _throw( Null_Exception);}*
_temp2800;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2801=
yyls; struct Cyc_Yyltype* _temp2803= _temp2801.curr + yylsp_offset; if(
_temp2803 < _temp2801.base? 1: _temp2803 >= _temp2801.last_plus_one){ _throw(
Null_Exception);}* _temp2803;}).first_line,({ struct _tagged_ptr3 _temp2804=
yyls; struct Cyc_Yyltype* _temp2806= _temp2804.curr + yylsp_offset; if(
_temp2806 < _temp2804.base? 1: _temp2806 >= _temp2804.last_plus_one){ _throw(
Null_Exception);}* _temp2806;}).last_line))};( struct _xenum_struct*) _temp2797;});
break; case 69: _LL2796: { struct _tagged_string* _temp2813; void* _temp2815;
struct _tuple1 _temp2811=* Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp2808=
yyvs; struct _xenum_struct** _temp2810= _temp2808.curr + yyvsp_offset; if(
_temp2810 < _temp2808.base? 1: _temp2810 >= _temp2808.last_plus_one){ _throw(
Null_Exception);}* _temp2810;})); _LL2816: _temp2815= _temp2811.f1; goto _LL2814;
_LL2814: _temp2813= _temp2811.f2; goto _LL2812; _LL2812: if( _temp2815 != Cyc_Absyn_Loc_n){
Cyc_Parse_err(( struct _tagged_string)({ char* _temp2817=( char*)"bad kind in type specifier";
struct _tagged_string _temp2818; _temp2818.curr= _temp2817; _temp2818.base=
_temp2817; _temp2818.last_plus_one= _temp2817 + 27; _temp2818;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2819= yyls; struct Cyc_Yyltype* _temp2821= _temp2819.curr
+ yylsp_offset; if( _temp2821 < _temp2819.base? 1: _temp2821 >= _temp2819.last_plus_one){
_throw( Null_Exception);}* _temp2821;}).first_line,({ struct _tagged_ptr3
_temp2822= yyls; struct Cyc_Yyltype* _temp2824= _temp2822.curr + yylsp_offset;
if( _temp2824 < _temp2822.base? 1: _temp2824 >= _temp2822.last_plus_one){ _throw(
Null_Exception);}* _temp2824;}).last_line));} yyval=({ struct Cyc_Kind_tok_struct*
_temp2825=( struct Cyc_Kind_tok_struct*) GC_malloc( sizeof( struct Cyc_Kind_tok_struct));*
_temp2825=( struct Cyc_Kind_tok_struct){.tag= Cyc_Kind_tok_tag,.f1= Cyc_Parse_id_to_kind(*
_temp2813, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2826= yyls;
struct Cyc_Yyltype* _temp2828= _temp2826.curr + yylsp_offset; if( _temp2828 <
_temp2826.base? 1: _temp2828 >= _temp2826.last_plus_one){ _throw( Null_Exception);}*
_temp2828;}).first_line,({ struct _tagged_ptr3 _temp2829= yyls; struct Cyc_Yyltype*
_temp2831= _temp2829.curr + yylsp_offset; if( _temp2831 < _temp2829.base? 1:
_temp2831 >= _temp2829.last_plus_one){ _throw( Null_Exception);}* _temp2831;}).last_line))};(
struct _xenum_struct*) _temp2825;}); break;} case 70: _LL2807: yyval=({ struct
Cyc_TypeQual_tok_struct* _temp2833=( struct Cyc_TypeQual_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeQual_tok_struct));* _temp2833=( struct Cyc_TypeQual_tok_struct){.tag=
Cyc_TypeQual_tok_tag,.f1=({ struct Cyc_Absyn_Tqual* _temp2834=( struct Cyc_Absyn_Tqual*)
GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual)); _temp2834->q_const= 1;
_temp2834->q_volatile= 0; _temp2834->q_restrict= 0; _temp2834;})};( struct
_xenum_struct*) _temp2833;}); break; case 71: _LL2832: yyval=({ struct Cyc_TypeQual_tok_struct*
_temp2836=( struct Cyc_TypeQual_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct));*
_temp2836=( struct Cyc_TypeQual_tok_struct){.tag= Cyc_TypeQual_tok_tag,.f1=({
struct Cyc_Absyn_Tqual* _temp2837=( struct Cyc_Absyn_Tqual*) GC_malloc_atomic(
sizeof( struct Cyc_Absyn_Tqual)); _temp2837->q_const= 0; _temp2837->q_volatile=
1; _temp2837->q_restrict= 0; _temp2837;})};( struct _xenum_struct*) _temp2836;});
break; case 72: _LL2835: yyval=({ struct Cyc_TypeQual_tok_struct* _temp2839=(
struct Cyc_TypeQual_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct));*
_temp2839=( struct Cyc_TypeQual_tok_struct){.tag= Cyc_TypeQual_tok_tag,.f1=({
struct Cyc_Absyn_Tqual* _temp2840=( struct Cyc_Absyn_Tqual*) GC_malloc_atomic(
sizeof( struct Cyc_Absyn_Tqual)); _temp2840->q_const= 0; _temp2840->q_volatile=
0; _temp2840->q_restrict= 1; _temp2840;})};( struct _xenum_struct*) _temp2839;});
break; case 73: _LL2838: { struct Cyc_Absyn_Decl* d;{ void* _temp2845= Cyc_yyget_StructOrUnion_tok(({
struct _tagged_ptr2 _temp2842= yyvs; struct _xenum_struct** _temp2844= _temp2842.curr
+( yyvsp_offset - 3); if( _temp2844 < _temp2842.base? 1: _temp2844 >= _temp2842.last_plus_one){
_throw( Null_Exception);}* _temp2844;})); _LL2847: if( _temp2845 == Cyc_Parse_Struct_su){
goto _LL2848;} else{ goto _LL2849;} _LL2849: if( _temp2845 == Cyc_Parse_Union_su){
goto _LL2850;} else{ goto _LL2846;} _LL2848: d= Cyc_Absyn_struct_decl( Cyc_Absyn_Public,
0, 0,({ struct Cyc_Core_Opt* _temp2851=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp2851->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp2852= yyvs; struct _xenum_struct** _temp2854= _temp2852.curr
+( yyvsp_offset - 1); if( _temp2854 < _temp2852.base? 1: _temp2854 >= _temp2852.last_plus_one){
_throw( Null_Exception);}* _temp2854;})); _temp2851;}), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2855= yyls; struct Cyc_Yyltype* _temp2857= _temp2855.curr
+( yylsp_offset - 3); if( _temp2857 < _temp2855.base? 1: _temp2857 >= _temp2855.last_plus_one){
_throw( Null_Exception);}* _temp2857;}).first_line,({ struct _tagged_ptr3
_temp2858= yyls; struct Cyc_Yyltype* _temp2860= _temp2858.curr + yylsp_offset;
if( _temp2860 < _temp2858.base? 1: _temp2860 >= _temp2858.last_plus_one){ _throw(
Null_Exception);}* _temp2860;}).last_line)); goto _LL2846; _LL2850: Cyc_Parse_unimp2((
struct _tagged_string)({ char* _temp2861=( char*)"unions"; struct _tagged_string
_temp2862; _temp2862.curr= _temp2861; _temp2862.base= _temp2861; _temp2862.last_plus_one=
_temp2861 + 7; _temp2862;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2863= yyls; struct Cyc_Yyltype* _temp2865= _temp2863.curr +( yylsp_offset -
3); if( _temp2865 < _temp2863.base? 1: _temp2865 >= _temp2863.last_plus_one){
_throw( Null_Exception);}* _temp2865;}).first_line,({ struct _tagged_ptr3
_temp2866= yyls; struct Cyc_Yyltype* _temp2868= _temp2866.curr + yylsp_offset;
if( _temp2868 < _temp2866.base? 1: _temp2868 >= _temp2866.last_plus_one){ _throw(
Null_Exception);}* _temp2868;}).last_line)); d= Cyc_Absyn_new_decl( Cyc_Absyn_Union_d,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2869= yyls; struct Cyc_Yyltype*
_temp2871= _temp2869.curr +( yylsp_offset - 3); if( _temp2871 < _temp2869.base?
1: _temp2871 >= _temp2869.last_plus_one){ _throw( Null_Exception);}* _temp2871;}).first_line,({
struct _tagged_ptr3 _temp2872= yyls; struct Cyc_Yyltype* _temp2874= _temp2872.curr
+ yylsp_offset; if( _temp2874 < _temp2872.base? 1: _temp2874 >= _temp2872.last_plus_one){
_throw( Null_Exception);}* _temp2874;}).last_line)); goto _LL2846; _LL2846:;}
yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2875=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));* _temp2875=( struct
Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=({ struct Cyc_Parse_Decl_spec_struct*
_temp2876=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2876->tag= Cyc_Parse_Decl_spec_tag; _temp2876->f1= d;( void*) _temp2876;})};(
struct _xenum_struct*) _temp2875;}); Cyc_Parse_unimp2(( struct _tagged_string)({
char* _temp2877=( char*)"anonymous structs/unions"; struct _tagged_string
_temp2878; _temp2878.curr= _temp2877; _temp2878.base= _temp2877; _temp2878.last_plus_one=
_temp2877 + 25; _temp2878;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2879= yyls; struct Cyc_Yyltype* _temp2881= _temp2879.curr +( yylsp_offset -
3); if( _temp2881 < _temp2879.base? 1: _temp2881 >= _temp2879.last_plus_one){
_throw( Null_Exception);}* _temp2881;}).first_line,({ struct _tagged_ptr3
_temp2882= yyls; struct Cyc_Yyltype* _temp2884= _temp2882.curr + yylsp_offset;
if( _temp2884 < _temp2882.base? 1: _temp2884 >= _temp2882.last_plus_one){ _throw(
Null_Exception);}* _temp2884;}).last_line)); break;} case 74: _LL2841: { struct
Cyc_List_List* ts=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2886= yyls; struct Cyc_Yyltype* _temp2888= _temp2886.curr
+( yylsp_offset - 3); if( _temp2888 < _temp2886.base? 1: _temp2888 >= _temp2886.last_plus_one){
_throw( Null_Exception);}* _temp2888;}).first_line,({ struct _tagged_ptr3
_temp2889= yyls; struct Cyc_Yyltype* _temp2891= _temp2889.curr +( yylsp_offset -
3); if( _temp2891 < _temp2889.base? 1: _temp2891 >= _temp2889.last_plus_one){
_throw( Null_Exception);}* _temp2891;}).last_line), Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp2892= yyvs; struct _xenum_struct** _temp2894= _temp2892.curr
+( yyvsp_offset - 3); if( _temp2894 < _temp2892.base? 1: _temp2894 >= _temp2892.last_plus_one){
_throw( Null_Exception);}* _temp2894;}))); struct Cyc_Absyn_Decl* d;{ void*
_temp2898= Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2 _temp2895= yyvs;
struct _xenum_struct** _temp2897= _temp2895.curr +( yyvsp_offset - 5); if(
_temp2897 < _temp2895.base? 1: _temp2897 >= _temp2895.last_plus_one){ _throw(
Null_Exception);}* _temp2897;})); _LL2900: if( _temp2898 == Cyc_Parse_Struct_su){
goto _LL2901;} else{ goto _LL2902;} _LL2902: if( _temp2898 == Cyc_Parse_Union_su){
goto _LL2903;} else{ goto _LL2899;} _LL2901: d= Cyc_Absyn_struct_decl( Cyc_Absyn_Public,({
struct Cyc_Core_Opt* _temp2904=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2904->v=( void*) Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp2905= yyvs; struct _xenum_struct** _temp2907= _temp2905.curr +(
yyvsp_offset - 4); if( _temp2907 < _temp2905.base? 1: _temp2907 >= _temp2905.last_plus_one){
_throw( Null_Exception);}* _temp2907;})); _temp2904;}), ts,({ struct Cyc_Core_Opt*
_temp2908=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2908->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({ struct _tagged_ptr2
_temp2909= yyvs; struct _xenum_struct** _temp2911= _temp2909.curr +(
yyvsp_offset - 1); if( _temp2911 < _temp2909.base? 1: _temp2911 >= _temp2909.last_plus_one){
_throw( Null_Exception);}* _temp2911;})); _temp2908;}), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2912= yyls; struct Cyc_Yyltype* _temp2914= _temp2912.curr
+( yylsp_offset - 5); if( _temp2914 < _temp2912.base? 1: _temp2914 >= _temp2912.last_plus_one){
_throw( Null_Exception);}* _temp2914;}).first_line,({ struct _tagged_ptr3
_temp2915= yyls; struct Cyc_Yyltype* _temp2917= _temp2915.curr + yylsp_offset;
if( _temp2917 < _temp2915.base? 1: _temp2917 >= _temp2915.last_plus_one){ _throw(
Null_Exception);}* _temp2917;}).last_line)); goto _LL2899; _LL2903: Cyc_Parse_unimp2((
struct _tagged_string)({ char* _temp2918=( char*)"unions"; struct _tagged_string
_temp2919; _temp2919.curr= _temp2918; _temp2919.base= _temp2918; _temp2919.last_plus_one=
_temp2918 + 7; _temp2919;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2920= yyls; struct Cyc_Yyltype* _temp2922= _temp2920.curr +( yylsp_offset -
5); if( _temp2922 < _temp2920.base? 1: _temp2922 >= _temp2920.last_plus_one){
_throw( Null_Exception);}* _temp2922;}).first_line,({ struct _tagged_ptr3
_temp2923= yyls; struct Cyc_Yyltype* _temp2925= _temp2923.curr + yylsp_offset;
if( _temp2925 < _temp2923.base? 1: _temp2925 >= _temp2923.last_plus_one){ _throw(
Null_Exception);}* _temp2925;}).last_line)); d= Cyc_Absyn_new_decl( Cyc_Absyn_Union_d,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2926= yyls; struct Cyc_Yyltype*
_temp2928= _temp2926.curr +( yylsp_offset - 5); if( _temp2928 < _temp2926.base?
1: _temp2928 >= _temp2926.last_plus_one){ _throw( Null_Exception);}* _temp2928;}).first_line,({
struct _tagged_ptr3 _temp2929= yyls; struct Cyc_Yyltype* _temp2931= _temp2929.curr
+ yylsp_offset; if( _temp2931 < _temp2929.base? 1: _temp2931 >= _temp2929.last_plus_one){
_throw( Null_Exception);}* _temp2931;}).last_line)); goto _LL2899; _LL2899:;}
yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2932=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));* _temp2932=( struct
Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=({ struct Cyc_Parse_Decl_spec_struct*
_temp2933=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2933->tag= Cyc_Parse_Decl_spec_tag; _temp2933->f1= d;( void*) _temp2933;})};(
struct _xenum_struct*) _temp2932;}); break;} case 75: _LL2885: { struct Cyc_List_List*
ts=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2935=
yyls; struct Cyc_Yyltype* _temp2937= _temp2935.curr +( yylsp_offset - 3); if(
_temp2937 < _temp2935.base? 1: _temp2937 >= _temp2935.last_plus_one){ _throw(
Null_Exception);}* _temp2937;}).first_line,({ struct _tagged_ptr3 _temp2938=
yyls; struct Cyc_Yyltype* _temp2940= _temp2938.curr +( yylsp_offset - 3); if(
_temp2940 < _temp2938.base? 1: _temp2940 >= _temp2938.last_plus_one){ _throw(
Null_Exception);}* _temp2940;}).last_line), Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp2941= yyvs; struct _xenum_struct** _temp2943= _temp2941.curr +(
yyvsp_offset - 3); if( _temp2943 < _temp2941.base? 1: _temp2943 >= _temp2941.last_plus_one){
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
Null_Exception);}* _temp2966;}).last_line)); goto _LL2948; _LL2952: Cyc_Parse_unimp2((
struct _tagged_string)({ char* _temp2967=( char*)"unions"; struct _tagged_string
_temp2968; _temp2968.curr= _temp2967; _temp2968.base= _temp2967; _temp2968.last_plus_one=
_temp2967 + 7; _temp2968;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2969= yyls; struct Cyc_Yyltype* _temp2971= _temp2969.curr +( yylsp_offset -
5); if( _temp2971 < _temp2969.base? 1: _temp2971 >= _temp2969.last_plus_one){
_throw( Null_Exception);}* _temp2971;}).first_line,({ struct _tagged_ptr3
_temp2972= yyls; struct Cyc_Yyltype* _temp2974= _temp2972.curr + yylsp_offset;
if( _temp2974 < _temp2972.base? 1: _temp2974 >= _temp2972.last_plus_one){ _throw(
Null_Exception);}* _temp2974;}).last_line)); d= Cyc_Absyn_new_decl( Cyc_Absyn_Union_d,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2975= yyls; struct Cyc_Yyltype*
_temp2977= _temp2975.curr +( yylsp_offset - 5); if( _temp2977 < _temp2975.base?
1: _temp2977 >= _temp2975.last_plus_one){ _throw( Null_Exception);}* _temp2977;}).first_line,({
struct _tagged_ptr3 _temp2978= yyls; struct Cyc_Yyltype* _temp2980= _temp2978.curr
+ yylsp_offset; if( _temp2980 < _temp2978.base? 1: _temp2980 >= _temp2978.last_plus_one){
_throw( Null_Exception);}* _temp2980;}).last_line)); goto _LL2948; _LL2948:;}
yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2981=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));* _temp2981=( struct
Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=({ struct Cyc_Parse_Decl_spec_struct*
_temp2982=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2982->tag= Cyc_Parse_Decl_spec_tag; _temp2982->f1= d;( void*) _temp2982;})};(
struct _xenum_struct*) _temp2981;}); break;} case 76: _LL2934:{ void* _temp2987=
Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2 _temp2984= yyvs; struct
_xenum_struct** _temp2986= _temp2984.curr +( yyvsp_offset - 2); if( _temp2986 <
_temp2984.base? 1: _temp2986 >= _temp2984.last_plus_one){ _throw( Null_Exception);}*
_temp2986;})); _LL2989: if( _temp2987 == Cyc_Parse_Struct_su){ goto _LL2990;}
else{ goto _LL2991;} _LL2991: if( _temp2987 == Cyc_Parse_Union_su){ goto _LL2992;}
else{ goto _LL2988;} _LL2990: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp2993=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2993=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec(({ struct Cyc_Absyn_StructType_struct* _temp2994=( struct
Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp2994->tag= Cyc_Absyn_StructType_tag; _temp2994->f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp2995= yyvs; struct _xenum_struct** _temp2997= _temp2995.curr
+( yyvsp_offset - 1); if( _temp2997 < _temp2995.base? 1: _temp2997 >= _temp2995.last_plus_one){
_throw( Null_Exception);}* _temp2997;})); _temp2994->f2= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp2998= yyvs; struct _xenum_struct** _temp3000= _temp2998.curr
+ yyvsp_offset; if( _temp3000 < _temp2998.base? 1: _temp3000 >= _temp2998.last_plus_one){
_throw( Null_Exception);}* _temp3000;})); _temp2994->f3= 0;( void*) _temp2994;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3001= yyls; struct Cyc_Yyltype*
_temp3003= _temp3001.curr +( yylsp_offset - 2); if( _temp3003 < _temp3001.base?
1: _temp3003 >= _temp3001.last_plus_one){ _throw( Null_Exception);}* _temp3003;}).first_line,({
struct _tagged_ptr3 _temp3004= yyls; struct Cyc_Yyltype* _temp3006= _temp3004.curr
+ yylsp_offset; if( _temp3006 < _temp3004.base? 1: _temp3006 >= _temp3004.last_plus_one){
_throw( Null_Exception);}* _temp3006;}).last_line))};( struct _xenum_struct*)
_temp2993;}); goto _LL2988; _LL2992: Cyc_Parse_unimp2(( struct _tagged_string)({
char* _temp3007=( char*)"unions"; struct _tagged_string _temp3008; _temp3008.curr=
_temp3007; _temp3008.base= _temp3007; _temp3008.last_plus_one= _temp3007 + 7;
_temp3008;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3009= yyls;
struct Cyc_Yyltype* _temp3011= _temp3009.curr +( yylsp_offset - 2); if(
_temp3011 < _temp3009.base? 1: _temp3011 >= _temp3009.last_plus_one){ _throw(
Null_Exception);}* _temp3011;}).first_line,({ struct _tagged_ptr3 _temp3012=
yyls; struct Cyc_Yyltype* _temp3014= _temp3012.curr + yylsp_offset; if(
_temp3014 < _temp3012.base? 1: _temp3014 >= _temp3012.last_plus_one){ _throw(
Null_Exception);}* _temp3014;}).last_line)); yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp3015=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp3015=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=({
struct Cyc_Parse_Decl_spec_struct* _temp3016=( struct Cyc_Parse_Decl_spec_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct)); _temp3016->tag= Cyc_Parse_Decl_spec_tag;
_temp3016->f1= Cyc_Absyn_new_decl( Cyc_Absyn_Union_d, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3017= yyls; struct Cyc_Yyltype* _temp3019= _temp3017.curr
+( yylsp_offset - 2); if( _temp3019 < _temp3017.base? 1: _temp3019 >= _temp3017.last_plus_one){
_throw( Null_Exception);}* _temp3019;}).first_line,({ struct _tagged_ptr3
_temp3020= yyls; struct Cyc_Yyltype* _temp3022= _temp3020.curr + yylsp_offset;
if( _temp3022 < _temp3020.base? 1: _temp3022 >= _temp3020.last_plus_one){ _throw(
Null_Exception);}* _temp3022;}).last_line));( void*) _temp3016;})};( struct
_xenum_struct*) _temp3015;}); goto _LL2988; _LL2988:;} break; case 77: _LL2983:{
void* _temp3027= Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2 _temp3024=
yyvs; struct _xenum_struct** _temp3026= _temp3024.curr +( yyvsp_offset - 2); if(
_temp3026 < _temp3024.base? 1: _temp3026 >= _temp3024.last_plus_one){ _throw(
Null_Exception);}* _temp3026;})); _LL3029: if( _temp3027 == Cyc_Parse_Struct_su){
goto _LL3030;} else{ goto _LL3031;} _LL3031: if( _temp3027 == Cyc_Parse_Union_su){
goto _LL3032;} else{ goto _LL3028;} _LL3030: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp3033=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp3033=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec(({ struct Cyc_Absyn_StructType_struct* _temp3034=( struct
Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp3034->tag= Cyc_Absyn_StructType_tag; _temp3034->f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3035= yyvs; struct _xenum_struct** _temp3037= _temp3035.curr
+( yyvsp_offset - 1); if( _temp3037 < _temp3035.base? 1: _temp3037 >= _temp3035.last_plus_one){
_throw( Null_Exception);}* _temp3037;})); _temp3034->f2= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3038= yyvs; struct _xenum_struct** _temp3040= _temp3038.curr
+ yyvsp_offset; if( _temp3040 < _temp3038.base? 1: _temp3040 >= _temp3038.last_plus_one){
_throw( Null_Exception);}* _temp3040;})); _temp3034->f3= 0;( void*) _temp3034;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3041= yyls; struct Cyc_Yyltype*
_temp3043= _temp3041.curr +( yylsp_offset - 2); if( _temp3043 < _temp3041.base?
1: _temp3043 >= _temp3041.last_plus_one){ _throw( Null_Exception);}* _temp3043;}).first_line,({
struct _tagged_ptr3 _temp3044= yyls; struct Cyc_Yyltype* _temp3046= _temp3044.curr
+ yylsp_offset; if( _temp3046 < _temp3044.base? 1: _temp3046 >= _temp3044.last_plus_one){
_throw( Null_Exception);}* _temp3046;}).last_line))};( struct _xenum_struct*)
_temp3033;}); goto _LL3028; _LL3032: Cyc_Parse_unimp2(( struct _tagged_string)({
char* _temp3047=( char*)"unions"; struct _tagged_string _temp3048; _temp3048.curr=
_temp3047; _temp3048.base= _temp3047; _temp3048.last_plus_one= _temp3047 + 7;
_temp3048;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3049= yyls;
struct Cyc_Yyltype* _temp3051= _temp3049.curr +( yylsp_offset - 2); if(
_temp3051 < _temp3049.base? 1: _temp3051 >= _temp3049.last_plus_one){ _throw(
Null_Exception);}* _temp3051;}).first_line,({ struct _tagged_ptr3 _temp3052=
yyls; struct Cyc_Yyltype* _temp3054= _temp3052.curr + yylsp_offset; if(
_temp3054 < _temp3052.base? 1: _temp3054 >= _temp3052.last_plus_one){ _throw(
Null_Exception);}* _temp3054;}).last_line)); yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp3055=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp3055=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=({
struct Cyc_Parse_Decl_spec_struct* _temp3056=( struct Cyc_Parse_Decl_spec_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct)); _temp3056->tag= Cyc_Parse_Decl_spec_tag;
_temp3056->f1= Cyc_Absyn_new_decl( Cyc_Absyn_Union_d, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3057= yyls; struct Cyc_Yyltype* _temp3059= _temp3057.curr
+( yylsp_offset - 2); if( _temp3059 < _temp3057.base? 1: _temp3059 >= _temp3057.last_plus_one){
_throw( Null_Exception);}* _temp3059;}).first_line,({ struct _tagged_ptr3
_temp3060= yyls; struct Cyc_Yyltype* _temp3062= _temp3060.curr + yylsp_offset;
if( _temp3062 < _temp3060.base? 1: _temp3062 >= _temp3060.last_plus_one){ _throw(
Null_Exception);}* _temp3062;}).last_line));( void*) _temp3056;})};( struct
_xenum_struct*) _temp3055;}); goto _LL3028; _LL3028:;} break; case 78: _LL3023:
yyval=({ struct Cyc_TypeList_tok_struct* _temp3064=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));* _temp3064=( struct Cyc_TypeList_tok_struct){.tag=
Cyc_TypeList_tok_tag,.f1= 0};( struct _xenum_struct*) _temp3064;}); break; case
79: _LL3063: yyval=({ struct Cyc_TypeList_tok_struct* _temp3066=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));* _temp3066=( struct Cyc_TypeList_tok_struct){.tag=
Cyc_TypeList_tok_tag,.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3067= yyvs;
struct _xenum_struct** _temp3069= _temp3067.curr +( yyvsp_offset - 1); if(
_temp3069 < _temp3067.base? 1: _temp3069 >= _temp3067.last_plus_one){ _throw(
Null_Exception);}* _temp3069;})))};( struct _xenum_struct*) _temp3066;}); break;
case 80: _LL3065:(( struct Cyc_Lexing_lexbuf*) Cyc_Parse_lbuf->v)->lex_curr_pos
-= 1; yyval=({ struct Cyc_TypeList_tok_struct* _temp3071=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));* _temp3071=( struct Cyc_TypeList_tok_struct){.tag=
Cyc_TypeList_tok_tag,.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3072= yyvs;
struct _xenum_struct** _temp3074= _temp3072.curr +( yyvsp_offset - 1); if(
_temp3074 < _temp3072.base? 1: _temp3074 >= _temp3072.last_plus_one){ _throw(
Null_Exception);}* _temp3074;})))};( struct _xenum_struct*) _temp3071;}); break;
case 81: _LL3070: yyval=({ struct Cyc_StructOrUnion_tok_struct* _temp3076=(
struct Cyc_StructOrUnion_tok_struct*) GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));*
_temp3076=( struct Cyc_StructOrUnion_tok_struct){.tag= Cyc_StructOrUnion_tok_tag,.f1=
Cyc_Parse_Struct_su};( struct _xenum_struct*) _temp3076;}); break; case 82:
_LL3075: yyval=({ struct Cyc_StructOrUnion_tok_struct* _temp3078=( struct Cyc_StructOrUnion_tok_struct*)
GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));* _temp3078=( struct
Cyc_StructOrUnion_tok_struct){.tag= Cyc_StructOrUnion_tok_tag,.f1= Cyc_Parse_Union_su};(
struct _xenum_struct*) _temp3078;}); break; case 83: _LL3077: yyval=({ struct
Cyc_StructFieldDeclList_tok_struct* _temp3080=( struct Cyc_StructFieldDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_StructFieldDeclList_tok_struct));* _temp3080=(
struct Cyc_StructFieldDeclList_tok_struct){.tag= Cyc_StructFieldDeclList_tok_tag,.f1=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)((( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_StructFieldDeclListList_tok(({
struct _tagged_ptr2 _temp3081= yyvs; struct _xenum_struct** _temp3083= _temp3081.curr
+ yyvsp_offset; if( _temp3083 < _temp3081.base? 1: _temp3083 >= _temp3081.last_plus_one){
_throw( Null_Exception);}* _temp3083;}))))};( struct _xenum_struct*) _temp3080;});
break; case 84: _LL3079: yyval=({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp3085=( struct Cyc_StructFieldDeclListList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct));* _temp3085=( struct Cyc_StructFieldDeclListList_tok_struct){.tag=
Cyc_StructFieldDeclListList_tok_tag,.f1=({ struct Cyc_List_List* _temp3086=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3086->hd=(
void*) Cyc_yyget_StructFieldDeclList_tok(({ struct _tagged_ptr2 _temp3087= yyvs;
struct _xenum_struct** _temp3089= _temp3087.curr + yyvsp_offset; if( _temp3089 <
_temp3087.base? 1: _temp3089 >= _temp3087.last_plus_one){ _throw( Null_Exception);}*
_temp3089;})); _temp3086->tl= 0; _temp3086;})};( struct _xenum_struct*)
_temp3085;}); break; case 85: _LL3084: yyval=({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp3091=( struct Cyc_StructFieldDeclListList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct));* _temp3091=( struct Cyc_StructFieldDeclListList_tok_struct){.tag=
Cyc_StructFieldDeclListList_tok_tag,.f1=({ struct Cyc_List_List* _temp3092=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3092->hd=(
void*) Cyc_yyget_StructFieldDeclList_tok(({ struct _tagged_ptr2 _temp3096= yyvs;
struct _xenum_struct** _temp3098= _temp3096.curr + yyvsp_offset; if( _temp3098 <
_temp3096.base? 1: _temp3098 >= _temp3096.last_plus_one){ _throw( Null_Exception);}*
_temp3098;})); _temp3092->tl= Cyc_yyget_StructFieldDeclListList_tok(({ struct
_tagged_ptr2 _temp3093= yyvs; struct _xenum_struct** _temp3095= _temp3093.curr +(
yyvsp_offset - 1); if( _temp3095 < _temp3093.base? 1: _temp3095 >= _temp3093.last_plus_one){
_throw( Null_Exception);}* _temp3095;})); _temp3092;})};( struct _xenum_struct*)
_temp3091;}); break; case 86: _LL3090: yyval=({ struct Cyc_InitDeclList_tok_struct*
_temp3100=( struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));*
_temp3100=( struct Cyc_InitDeclList_tok_struct){.tag= Cyc_InitDeclList_tok_tag,.f1=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitDeclList_tok(({
struct _tagged_ptr2 _temp3101= yyvs; struct _xenum_struct** _temp3103= _temp3101.curr
+ yyvsp_offset; if( _temp3103 < _temp3101.base? 1: _temp3103 >= _temp3101.last_plus_one){
_throw( Null_Exception);}* _temp3103;})))};( struct _xenum_struct*) _temp3100;});
break; case 87: _LL3099: yyval=({ struct Cyc_InitDeclList_tok_struct* _temp3105=(
struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));*
_temp3105=( struct Cyc_InitDeclList_tok_struct){.tag= Cyc_InitDeclList_tok_tag,.f1=({
struct Cyc_List_List* _temp3106=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3106->hd=( void*) Cyc_yyget_InitDecl_tok(({ struct
_tagged_ptr2 _temp3107= yyvs; struct _xenum_struct** _temp3109= _temp3107.curr +
yyvsp_offset; if( _temp3109 < _temp3107.base? 1: _temp3109 >= _temp3107.last_plus_one){
_throw( Null_Exception);}* _temp3109;})); _temp3106->tl= 0; _temp3106;})};(
struct _xenum_struct*) _temp3105;}); break; case 88: _LL3104: yyval=({ struct
Cyc_InitDeclList_tok_struct* _temp3111=( struct Cyc_InitDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));* _temp3111=( struct Cyc_InitDeclList_tok_struct){.tag=
Cyc_InitDeclList_tok_tag,.f1=({ struct Cyc_List_List* _temp3112=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3112->hd=( void*) Cyc_yyget_InitDecl_tok(({
struct _tagged_ptr2 _temp3116= yyvs; struct _xenum_struct** _temp3118= _temp3116.curr
+ yyvsp_offset; if( _temp3118 < _temp3116.base? 1: _temp3118 >= _temp3116.last_plus_one){
_throw( Null_Exception);}* _temp3118;})); _temp3112->tl= Cyc_yyget_InitDeclList_tok(({
struct _tagged_ptr2 _temp3113= yyvs; struct _xenum_struct** _temp3115= _temp3113.curr
+( yyvsp_offset - 2); if( _temp3115 < _temp3113.base? 1: _temp3115 >= _temp3113.last_plus_one){
_throw( Null_Exception);}* _temp3115;})); _temp3112;})};( struct _xenum_struct*)
_temp3111;}); break; case 89: _LL3110: yyval=({ struct Cyc_InitDecl_tok_struct*
_temp3120=( struct Cyc_InitDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct));*
_temp3120=( struct Cyc_InitDecl_tok_struct){.tag= Cyc_InitDecl_tok_tag,.f1=({
struct _tuple12* _temp3121=( struct _tuple12*) GC_malloc( sizeof( struct
_tuple12)); _temp3121->f1= Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2
_temp3122= yyvs; struct _xenum_struct** _temp3124= _temp3122.curr + yyvsp_offset;
if( _temp3124 < _temp3122.base? 1: _temp3124 >= _temp3122.last_plus_one){ _throw(
Null_Exception);}* _temp3124;})); _temp3121->f2= 0; _temp3121;})};( struct
_xenum_struct*) _temp3120;}); break; case 90: _LL3119: yyval=({ struct Cyc_InitDecl_tok_struct*
_temp3126=( struct Cyc_InitDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct));*
_temp3126=( struct Cyc_InitDecl_tok_struct){.tag= Cyc_InitDecl_tok_tag,.f1=({
struct _tuple12* _temp3127=( struct _tuple12*) GC_malloc( sizeof( struct
_tuple12)); _temp3127->f1= Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2
_temp3131= yyvs; struct _xenum_struct** _temp3133= _temp3131.curr +(
yyvsp_offset - 2); if( _temp3133 < _temp3131.base? 1: _temp3133 >= _temp3131.last_plus_one){
_throw( Null_Exception);}* _temp3133;})); _temp3127->f2=( struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp3128= yyvs; struct _xenum_struct**
_temp3130= _temp3128.curr + yyvsp_offset; if( _temp3130 < _temp3128.base? 1:
_temp3130 >= _temp3128.last_plus_one){ _throw( Null_Exception);}* _temp3130;}));
_temp3127;})};( struct _xenum_struct*) _temp3126;}); break; case 91: _LL3125: {
struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp3135= yyvs; struct _xenum_struct** _temp3137= _temp3135.curr +(
yyvsp_offset - 2); if( _temp3137 < _temp3135.base? 1: _temp3137 >= _temp3135.last_plus_one){
_throw( Null_Exception);}* _temp3137;}))).f1; void* t= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp3138= yyvs; struct
_xenum_struct** _temp3140= _temp3138.curr +( yyvsp_offset - 2); if( _temp3140 <
_temp3138.base? 1: _temp3140 >= _temp3138.last_plus_one){ _throw( Null_Exception);}*
_temp3140;}))).f2, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3141=
yyls; struct Cyc_Yyltype* _temp3143= _temp3141.curr +( yylsp_offset - 2); if(
_temp3143 < _temp3141.base? 1: _temp3143 >= _temp3141.last_plus_one){ _throw(
Null_Exception);}* _temp3143;}).first_line,({ struct _tagged_ptr3 _temp3144=
yyls; struct Cyc_Yyltype* _temp3146= _temp3144.curr +( yylsp_offset - 2); if(
_temp3146 < _temp3144.base? 1: _temp3146 >= _temp3144.last_plus_one){ _throw(
Null_Exception);}* _temp3146;}).last_line)); struct Cyc_List_List* info= Cyc_Parse_apply_tmss(
tq, t, Cyc_yyget_DeclaratorList_tok(({ struct _tagged_ptr2 _temp3147= yyvs;
struct _xenum_struct** _temp3149= _temp3147.curr +( yyvsp_offset - 1); if(
_temp3149 < _temp3147.base? 1: _temp3149 >= _temp3147.last_plus_one){ _throw(
Null_Exception);}* _temp3149;})), 0); yyval=({ struct Cyc_StructFieldDeclList_tok_struct*
_temp3150=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct));* _temp3150=( struct Cyc_StructFieldDeclList_tok_struct){.tag=
Cyc_StructFieldDeclList_tok_tag,.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Position_Segment*, struct _tuple7*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_make_struct_field, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3151= yyls; struct Cyc_Yyltype* _temp3153= _temp3151.curr
+( yylsp_offset - 2); if( _temp3153 < _temp3151.base? 1: _temp3153 >= _temp3151.last_plus_one){
_throw( Null_Exception);}* _temp3153;}).first_line,({ struct _tagged_ptr3
_temp3154= yyls; struct Cyc_Yyltype* _temp3156= _temp3154.curr +( yylsp_offset -
1); if( _temp3156 < _temp3154.base? 1: _temp3156 >= _temp3154.last_plus_one){
_throw( Null_Exception);}* _temp3156;}).last_line), info)};( struct
_xenum_struct*) _temp3150;}); break;} case 92: _LL3134: yyval=({ struct Cyc_QualSpecList_tok_struct*
_temp3158=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));*
_temp3158=( struct Cyc_QualSpecList_tok_struct){.tag= Cyc_QualSpecList_tok_tag,.f1=({
struct _tuple13* _temp3159=( struct _tuple13*) GC_malloc( sizeof( struct
_tuple13)); _temp3159->f1= Cyc_Absyn_empty_tqual(); _temp3159->f2=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp3160=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3160->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({
struct _tagged_ptr2 _temp3161= yyvs; struct _xenum_struct** _temp3163= _temp3161.curr
+ yyvsp_offset; if( _temp3163 < _temp3161.base? 1: _temp3163 >= _temp3161.last_plus_one){
_throw( Null_Exception);}* _temp3163;})); _temp3160->tl= 0; _temp3160;});
_temp3159;})};( struct _xenum_struct*) _temp3158;}); break; case 93: _LL3157:
yyval=({ struct Cyc_QualSpecList_tok_struct* _temp3165=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));* _temp3165=( struct Cyc_QualSpecList_tok_struct){.tag=
Cyc_QualSpecList_tok_tag,.f1=({ struct _tuple13* _temp3166=( struct _tuple13*)
GC_malloc( sizeof( struct _tuple13)); _temp3166->f1=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3174= yyvs; struct _xenum_struct** _temp3176= _temp3174.curr
+ yyvsp_offset; if( _temp3176 < _temp3174.base? 1: _temp3176 >= _temp3174.last_plus_one){
_throw( Null_Exception);}* _temp3176;}))).f1; _temp3166->f2=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp3167=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3167->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({
struct _tagged_ptr2 _temp3171= yyvs; struct _xenum_struct** _temp3173= _temp3171.curr
+( yyvsp_offset - 1); if( _temp3173 < _temp3171.base? 1: _temp3173 >= _temp3171.last_plus_one){
_throw( Null_Exception);}* _temp3173;})); _temp3167->tl=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3168= yyvs; struct _xenum_struct** _temp3170= _temp3168.curr
+ yyvsp_offset; if( _temp3170 < _temp3168.base? 1: _temp3170 >= _temp3168.last_plus_one){
_throw( Null_Exception);}* _temp3170;}))).f2; _temp3167;}); _temp3166;})};(
struct _xenum_struct*) _temp3165;}); break; case 94: _LL3164: yyval=({ struct
Cyc_QualSpecList_tok_struct* _temp3178=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));* _temp3178=( struct Cyc_QualSpecList_tok_struct){.tag=
Cyc_QualSpecList_tok_tag,.f1=({ struct _tuple13* _temp3179=( struct _tuple13*)
GC_malloc( sizeof( struct _tuple13)); _temp3179->f1= Cyc_yyget_TypeQual_tok(({
struct _tagged_ptr2 _temp3180= yyvs; struct _xenum_struct** _temp3182= _temp3180.curr
+ yyvsp_offset; if( _temp3182 < _temp3180.base? 1: _temp3182 >= _temp3180.last_plus_one){
_throw( Null_Exception);}* _temp3182;})); _temp3179->f2= 0; _temp3179;})};(
struct _xenum_struct*) _temp3178;}); break; case 95: _LL3177: yyval=({ struct
Cyc_QualSpecList_tok_struct* _temp3184=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));* _temp3184=( struct Cyc_QualSpecList_tok_struct){.tag=
Cyc_QualSpecList_tok_tag,.f1=({ struct _tuple13* _temp3185=( struct _tuple13*)
GC_malloc( sizeof( struct _tuple13)); _temp3185->f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2 _temp3189= yyvs; struct
_xenum_struct** _temp3191= _temp3189.curr +( yyvsp_offset - 1); if( _temp3191 <
_temp3189.base? 1: _temp3191 >= _temp3189.last_plus_one){ _throw( Null_Exception);}*
_temp3191;})),(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp3192=
yyvs; struct _xenum_struct** _temp3194= _temp3192.curr + yyvsp_offset; if(
_temp3194 < _temp3192.base? 1: _temp3194 >= _temp3192.last_plus_one){ _throw(
Null_Exception);}* _temp3194;}))).f1); _temp3185->f2=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3186= yyvs; struct _xenum_struct** _temp3188= _temp3186.curr
+ yyvsp_offset; if( _temp3188 < _temp3186.base? 1: _temp3188 >= _temp3186.last_plus_one){
_throw( Null_Exception);}* _temp3188;}))).f2; _temp3185;})};( struct
_xenum_struct*) _temp3184;}); break; case 96: _LL3183: yyval=({ struct Cyc_DeclaratorList_tok_struct*
_temp3196=( struct Cyc_DeclaratorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorList_tok_struct));*
_temp3196=( struct Cyc_DeclaratorList_tok_struct){.tag= Cyc_DeclaratorList_tok_tag,.f1=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_DeclaratorList_tok(({
struct _tagged_ptr2 _temp3197= yyvs; struct _xenum_struct** _temp3199= _temp3197.curr
+ yyvsp_offset; if( _temp3199 < _temp3197.base? 1: _temp3199 >= _temp3197.last_plus_one){
_throw( Null_Exception);}* _temp3199;})))};( struct _xenum_struct*) _temp3196;});
break; case 97: _LL3195: yyval=({ struct Cyc_DeclaratorList_tok_struct*
_temp3201=( struct Cyc_DeclaratorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorList_tok_struct));*
_temp3201=( struct Cyc_DeclaratorList_tok_struct){.tag= Cyc_DeclaratorList_tok_tag,.f1=({
struct Cyc_List_List* _temp3202=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3202->hd=( void*) Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3203= yyvs; struct _xenum_struct** _temp3205= _temp3203.curr
+ yyvsp_offset; if( _temp3205 < _temp3203.base? 1: _temp3205 >= _temp3203.last_plus_one){
_throw( Null_Exception);}* _temp3205;})); _temp3202->tl= 0; _temp3202;})};(
struct _xenum_struct*) _temp3201;}); break; case 98: _LL3200: yyval=({ struct
Cyc_DeclaratorList_tok_struct* _temp3207=( struct Cyc_DeclaratorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorList_tok_struct));* _temp3207=( struct
Cyc_DeclaratorList_tok_struct){.tag= Cyc_DeclaratorList_tok_tag,.f1=({ struct
Cyc_List_List* _temp3208=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3208->hd=( void*) Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3212=
yyvs; struct _xenum_struct** _temp3214= _temp3212.curr + yyvsp_offset; if(
_temp3214 < _temp3212.base? 1: _temp3214 >= _temp3212.last_plus_one){ _throw(
Null_Exception);}* _temp3214;})); _temp3208->tl= Cyc_yyget_DeclaratorList_tok(({
struct _tagged_ptr2 _temp3209= yyvs; struct _xenum_struct** _temp3211= _temp3209.curr
+( yyvsp_offset - 2); if( _temp3211 < _temp3209.base? 1: _temp3211 >= _temp3209.last_plus_one){
_throw( Null_Exception);}* _temp3211;})); _temp3208;})};( struct _xenum_struct*)
_temp3207;}); break; case 99: _LL3206: yyval=({ struct _tagged_ptr2 _temp3216=
yyvs; struct _xenum_struct** _temp3218= _temp3216.curr + yyvsp_offset; if(
_temp3218 < _temp3216.base? 1: _temp3218 >= _temp3216.last_plus_one){ _throw(
Null_Exception);}* _temp3218;}); break; case 100: _LL3215: Cyc_Parse_unimp2((
struct _tagged_string)({ char* _temp3220=( char*)"bit fields"; struct
_tagged_string _temp3221; _temp3221.curr= _temp3220; _temp3221.base= _temp3220;
_temp3221.last_plus_one= _temp3220 + 11; _temp3221;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3222= yyls; struct Cyc_Yyltype* _temp3224= _temp3222.curr
+( yylsp_offset - 1); if( _temp3224 < _temp3222.base? 1: _temp3224 >= _temp3222.last_plus_one){
_throw( Null_Exception);}* _temp3224;}).first_line,({ struct _tagged_ptr3
_temp3225= yyls; struct Cyc_Yyltype* _temp3227= _temp3225.curr + yylsp_offset;
if( _temp3227 < _temp3225.base? 1: _temp3227 >= _temp3225.last_plus_one){ _throw(
Null_Exception);}* _temp3227;}).last_line)); yyval=({ struct Cyc_Declarator_tok_struct*
_temp3228=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));*
_temp3228=( struct Cyc_Declarator_tok_struct){.tag= Cyc_Declarator_tok_tag,.f1=({
struct Cyc_Parse_Declarator* _temp3229=( struct Cyc_Parse_Declarator*) GC_malloc(
sizeof( struct Cyc_Parse_Declarator)); _temp3229->id=({ struct _tuple1*
_temp3230=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp3230->f1=({
struct Cyc_Absyn_Rel_n_struct* _temp3234=( struct Cyc_Absyn_Rel_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct)); _temp3234->tag= Cyc_Absyn_Rel_n_tag;
_temp3234->f1= 0;( void*) _temp3234;}); _temp3230->f2=({ struct _tagged_string*
_temp3231=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp3231[ 0]=( struct _tagged_string)({ char* _temp3232=( char*)""; struct
_tagged_string _temp3233; _temp3233.curr= _temp3232; _temp3233.base= _temp3232;
_temp3233.last_plus_one= _temp3232 + 1; _temp3233;}); _temp3231;}); _temp3230;});
_temp3229->tms= 0; _temp3229;})};( struct _xenum_struct*) _temp3228;}); break;
case 101: _LL3219: Cyc_Parse_unimp2(( struct _tagged_string)({ char* _temp3236=(
char*)"bit fields"; struct _tagged_string _temp3237; _temp3237.curr= _temp3236;
_temp3237.base= _temp3236; _temp3237.last_plus_one= _temp3236 + 11; _temp3237;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3238= yyls; struct Cyc_Yyltype*
_temp3240= _temp3238.curr +( yylsp_offset - 2); if( _temp3240 < _temp3238.base?
1: _temp3240 >= _temp3238.last_plus_one){ _throw( Null_Exception);}* _temp3240;}).first_line,({
struct _tagged_ptr3 _temp3241= yyls; struct Cyc_Yyltype* _temp3243= _temp3241.curr
+( yylsp_offset - 1); if( _temp3243 < _temp3241.base? 1: _temp3243 >= _temp3241.last_plus_one){
_throw( Null_Exception);}* _temp3243;}).last_line)); yyval=({ struct
_tagged_ptr2 _temp3244= yyvs; struct _xenum_struct** _temp3246= _temp3244.curr +(
yyvsp_offset - 2); if( _temp3246 < _temp3244.base? 1: _temp3246 >= _temp3244.last_plus_one){
_throw( Null_Exception);}* _temp3246;}); break; case 102: _LL3235: yyval=({
struct Cyc_TypeSpecifier_tok_struct* _temp3248=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));* _temp3248=( struct
Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=({ struct Cyc_Parse_Decl_spec_struct*
_temp3249=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3249->tag= Cyc_Parse_Decl_spec_tag; _temp3249->f1= Cyc_Absyn_enum_decl( Cyc_Absyn_Public,
0, 0,({ struct Cyc_Core_Opt* _temp3250=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp3250->v=( void*) Cyc_yyget_EnumeratorFieldList_tok(({
struct _tagged_ptr2 _temp3251= yyvs; struct _xenum_struct** _temp3253= _temp3251.curr
+( yyvsp_offset - 1); if( _temp3253 < _temp3251.base? 1: _temp3253 >= _temp3251.last_plus_one){
_throw( Null_Exception);}* _temp3253;})); _temp3250;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3254= yyls; struct Cyc_Yyltype* _temp3256= _temp3254.curr
+( yylsp_offset - 3); if( _temp3256 < _temp3254.base? 1: _temp3256 >= _temp3254.last_plus_one){
_throw( Null_Exception);}* _temp3256;}).first_line,({ struct _tagged_ptr3
_temp3257= yyls; struct Cyc_Yyltype* _temp3259= _temp3257.curr + yylsp_offset;
if( _temp3259 < _temp3257.base? 1: _temp3259 >= _temp3257.last_plus_one){ _throw(
Null_Exception);}* _temp3259;}).last_line));( void*) _temp3249;})};( struct
_xenum_struct*) _temp3248;}); Cyc_Parse_unimp2(( struct _tagged_string)({ char*
_temp3260=( char*)"anonymous enums"; struct _tagged_string _temp3261; _temp3261.curr=
_temp3260; _temp3261.base= _temp3260; _temp3261.last_plus_one= _temp3260 + 16;
_temp3261;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3262= yyls;
struct Cyc_Yyltype* _temp3264= _temp3262.curr +( yylsp_offset - 3); if(
_temp3264 < _temp3262.base? 1: _temp3264 >= _temp3262.last_plus_one){ _throw(
Null_Exception);}* _temp3264;}).first_line,({ struct _tagged_ptr3 _temp3265=
yyls; struct Cyc_Yyltype* _temp3267= _temp3265.curr + yylsp_offset; if(
_temp3267 < _temp3265.base? 1: _temp3267 >= _temp3265.last_plus_one){ _throw(
Null_Exception);}* _temp3267;}).last_line)); break; case 103: _LL3247: { struct
Cyc_List_List* ts=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3269= yyls; struct Cyc_Yyltype* _temp3271= _temp3269.curr
+( yylsp_offset - 3); if( _temp3271 < _temp3269.base? 1: _temp3271 >= _temp3269.last_plus_one){
_throw( Null_Exception);}* _temp3271;}).first_line,({ struct _tagged_ptr3
_temp3272= yyls; struct Cyc_Yyltype* _temp3274= _temp3272.curr +( yylsp_offset -
3); if( _temp3274 < _temp3272.base? 1: _temp3274 >= _temp3272.last_plus_one){
_throw( Null_Exception);}* _temp3274;}).last_line), Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3275= yyvs; struct _xenum_struct** _temp3277= _temp3275.curr
+( yyvsp_offset - 3); if( _temp3277 < _temp3275.base? 1: _temp3277 >= _temp3275.last_plus_one){
_throw( Null_Exception);}* _temp3277;}))); yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp3278=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp3278=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=({
struct Cyc_Parse_Decl_spec_struct* _temp3279=( struct Cyc_Parse_Decl_spec_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct)); _temp3279->tag= Cyc_Parse_Decl_spec_tag;
_temp3279->f1= Cyc_Absyn_enum_decl( Cyc_Absyn_Public,({ struct Cyc_Core_Opt*
_temp3280=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3280->v=( void*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3281=
yyvs; struct _xenum_struct** _temp3283= _temp3281.curr +( yyvsp_offset - 4); if(
_temp3283 < _temp3281.base? 1: _temp3283 >= _temp3281.last_plus_one){ _throw(
Null_Exception);}* _temp3283;})); _temp3280;}), ts,({ struct Cyc_Core_Opt*
_temp3284=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3284->v=( void*) Cyc_yyget_EnumeratorFieldList_tok(({ struct _tagged_ptr2
_temp3285= yyvs; struct _xenum_struct** _temp3287= _temp3285.curr +(
yyvsp_offset - 1); if( _temp3287 < _temp3285.base? 1: _temp3287 >= _temp3285.last_plus_one){
_throw( Null_Exception);}* _temp3287;})); _temp3284;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3288= yyls; struct Cyc_Yyltype* _temp3290= _temp3288.curr
+( yylsp_offset - 5); if( _temp3290 < _temp3288.base? 1: _temp3290 >= _temp3288.last_plus_one){
_throw( Null_Exception);}* _temp3290;}).first_line,({ struct _tagged_ptr3
_temp3291= yyls; struct Cyc_Yyltype* _temp3293= _temp3291.curr + yylsp_offset;
if( _temp3293 < _temp3291.base? 1: _temp3293 >= _temp3291.last_plus_one){ _throw(
Null_Exception);}* _temp3293;}).last_line));( void*) _temp3279;})};( struct
_xenum_struct*) _temp3278;}); break;} case 104: _LL3268: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp3295=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp3295=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec(({ struct Cyc_Absyn_EnumType_struct* _temp3296=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp3296->tag= Cyc_Absyn_EnumType_tag;
_temp3296->f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3297= yyvs; struct _xenum_struct** _temp3299= _temp3297.curr +(
yyvsp_offset - 1); if( _temp3299 < _temp3297.base? 1: _temp3299 >= _temp3297.last_plus_one){
_throw( Null_Exception);}* _temp3299;})); _temp3296->f2= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3300= yyvs; struct _xenum_struct** _temp3302= _temp3300.curr
+ yyvsp_offset; if( _temp3302 < _temp3300.base? 1: _temp3302 >= _temp3300.last_plus_one){
_throw( Null_Exception);}* _temp3302;})); _temp3296->f3= 0;( void*) _temp3296;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3303= yyls; struct Cyc_Yyltype*
_temp3305= _temp3303.curr +( yylsp_offset - 2); if( _temp3305 < _temp3303.base?
1: _temp3305 >= _temp3303.last_plus_one){ _throw( Null_Exception);}* _temp3305;}).first_line,({
struct _tagged_ptr3 _temp3306= yyls; struct Cyc_Yyltype* _temp3308= _temp3306.curr
+ yylsp_offset; if( _temp3308 < _temp3306.base? 1: _temp3308 >= _temp3306.last_plus_one){
_throw( Null_Exception);}* _temp3308;}).last_line))};( struct _xenum_struct*)
_temp3295;}); break; case 105: _LL3294: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp3310=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp3310=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=({
struct Cyc_Parse_Decl_spec_struct* _temp3311=( struct Cyc_Parse_Decl_spec_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct)); _temp3311->tag= Cyc_Parse_Decl_spec_tag;
_temp3311->f1= Cyc_Absyn_xenum_decl( Cyc_Absyn_Public, Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3312= yyvs; struct _xenum_struct** _temp3314= _temp3312.curr
+( yyvsp_offset - 3); if( _temp3314 < _temp3312.base? 1: _temp3314 >= _temp3312.last_plus_one){
_throw( Null_Exception);}* _temp3314;})), Cyc_yyget_EnumeratorFieldList_tok(({
struct _tagged_ptr2 _temp3315= yyvs; struct _xenum_struct** _temp3317= _temp3315.curr
+( yyvsp_offset - 1); if( _temp3317 < _temp3315.base? 1: _temp3317 >= _temp3315.last_plus_one){
_throw( Null_Exception);}* _temp3317;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3318= yyls; struct Cyc_Yyltype* _temp3320= _temp3318.curr +(
yylsp_offset - 4); if( _temp3320 < _temp3318.base? 1: _temp3320 >= _temp3318.last_plus_one){
_throw( Null_Exception);}* _temp3320;}).first_line,({ struct _tagged_ptr3
_temp3321= yyls; struct Cyc_Yyltype* _temp3323= _temp3321.curr + yylsp_offset;
if( _temp3323 < _temp3321.base? 1: _temp3323 >= _temp3321.last_plus_one){ _throw(
Null_Exception);}* _temp3323;}).last_line));( void*) _temp3311;})};( struct
_xenum_struct*) _temp3310;}); break; case 106: _LL3309: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp3325=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp3325=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec(({ struct Cyc_Absyn_XenumType_struct* _temp3326=( struct Cyc_Absyn_XenumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XenumType_struct)); _temp3326->tag= Cyc_Absyn_XenumType_tag;
_temp3326->f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3327= yyvs;
struct _xenum_struct** _temp3329= _temp3327.curr + yyvsp_offset; if( _temp3329 <
_temp3327.base? 1: _temp3329 >= _temp3327.last_plus_one){ _throw( Null_Exception);}*
_temp3329;})); _temp3326->f2= 0;( void*) _temp3326;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3330= yyls; struct Cyc_Yyltype* _temp3332= _temp3330.curr
+( yylsp_offset - 1); if( _temp3332 < _temp3330.base? 1: _temp3332 >= _temp3330.last_plus_one){
_throw( Null_Exception);}* _temp3332;}).first_line,({ struct _tagged_ptr3
_temp3333= yyls; struct Cyc_Yyltype* _temp3335= _temp3333.curr + yylsp_offset;
if( _temp3335 < _temp3333.base? 1: _temp3335 >= _temp3333.last_plus_one){ _throw(
Null_Exception);}* _temp3335;}).last_line))};( struct _xenum_struct*) _temp3325;});
break; case 107: _LL3324: yyval=({ struct Cyc_EnumeratorFieldList_tok_struct*
_temp3337=( struct Cyc_EnumeratorFieldList_tok_struct*) GC_malloc( sizeof(
struct Cyc_EnumeratorFieldList_tok_struct));* _temp3337=( struct Cyc_EnumeratorFieldList_tok_struct){.tag=
Cyc_EnumeratorFieldList_tok_tag,.f1=({ struct Cyc_List_List* _temp3338=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3338->hd=( void*)
Cyc_yyget_EnumeratorField_tok(({ struct _tagged_ptr2 _temp3339= yyvs; struct
_xenum_struct** _temp3341= _temp3339.curr + yyvsp_offset; if( _temp3341 <
_temp3339.base? 1: _temp3341 >= _temp3339.last_plus_one){ _throw( Null_Exception);}*
_temp3341;})); _temp3338->tl= 0; _temp3338;})};( struct _xenum_struct*)
_temp3337;}); break; case 108: _LL3336: yyval=({ struct Cyc_EnumeratorFieldList_tok_struct*
_temp3343=( struct Cyc_EnumeratorFieldList_tok_struct*) GC_malloc( sizeof(
struct Cyc_EnumeratorFieldList_tok_struct));* _temp3343=( struct Cyc_EnumeratorFieldList_tok_struct){.tag=
Cyc_EnumeratorFieldList_tok_tag,.f1=({ struct Cyc_List_List* _temp3344=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3344->hd=( void*)
Cyc_yyget_EnumeratorField_tok(({ struct _tagged_ptr2 _temp3345= yyvs; struct
_xenum_struct** _temp3347= _temp3345.curr +( yyvsp_offset - 1); if( _temp3347 <
_temp3345.base? 1: _temp3347 >= _temp3345.last_plus_one){ _throw( Null_Exception);}*
_temp3347;})); _temp3344->tl= 0; _temp3344;})};( struct _xenum_struct*)
_temp3343;}); break; case 109: _LL3342: yyval=({ struct Cyc_EnumeratorFieldList_tok_struct*
_temp3349=( struct Cyc_EnumeratorFieldList_tok_struct*) GC_malloc( sizeof(
struct Cyc_EnumeratorFieldList_tok_struct));* _temp3349=( struct Cyc_EnumeratorFieldList_tok_struct){.tag=
Cyc_EnumeratorFieldList_tok_tag,.f1=({ struct Cyc_List_List* _temp3350=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3350->hd=( void*)
Cyc_yyget_EnumeratorField_tok(({ struct _tagged_ptr2 _temp3354= yyvs; struct
_xenum_struct** _temp3356= _temp3354.curr +( yyvsp_offset - 2); if( _temp3356 <
_temp3354.base? 1: _temp3356 >= _temp3354.last_plus_one){ _throw( Null_Exception);}*
_temp3356;})); _temp3350->tl= Cyc_yyget_EnumeratorFieldList_tok(({ struct
_tagged_ptr2 _temp3351= yyvs; struct _xenum_struct** _temp3353= _temp3351.curr +
yyvsp_offset; if( _temp3353 < _temp3351.base? 1: _temp3353 >= _temp3351.last_plus_one){
_throw( Null_Exception);}* _temp3353;})); _temp3350;})};( struct _xenum_struct*)
_temp3349;}); break; case 110: _LL3348: yyval=({ struct Cyc_EnumeratorFieldList_tok_struct*
_temp3358=( struct Cyc_EnumeratorFieldList_tok_struct*) GC_malloc( sizeof(
struct Cyc_EnumeratorFieldList_tok_struct));* _temp3358=( struct Cyc_EnumeratorFieldList_tok_struct){.tag=
Cyc_EnumeratorFieldList_tok_tag,.f1=({ struct Cyc_List_List* _temp3359=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3359->hd=( void*)
Cyc_yyget_EnumeratorField_tok(({ struct _tagged_ptr2 _temp3363= yyvs; struct
_xenum_struct** _temp3365= _temp3363.curr +( yyvsp_offset - 2); if( _temp3365 <
_temp3363.base? 1: _temp3365 >= _temp3363.last_plus_one){ _throw( Null_Exception);}*
_temp3365;})); _temp3359->tl= Cyc_yyget_EnumeratorFieldList_tok(({ struct
_tagged_ptr2 _temp3360= yyvs; struct _xenum_struct** _temp3362= _temp3360.curr +
yyvsp_offset; if( _temp3362 < _temp3360.base? 1: _temp3362 >= _temp3360.last_plus_one){
_throw( Null_Exception);}* _temp3362;})); _temp3359;})};( struct _xenum_struct*)
_temp3358;}); break; case 111: _LL3357: yyval=({ struct Cyc_EnumeratorField_tok_struct*
_temp3367=( struct Cyc_EnumeratorField_tok_struct*) GC_malloc( sizeof( struct
Cyc_EnumeratorField_tok_struct));* _temp3367=( struct Cyc_EnumeratorField_tok_struct){.tag=
Cyc_EnumeratorField_tok_tag,.f1=({ struct Cyc_Absyn_Enumfield* _temp3368=(
struct Cyc_Absyn_Enumfield*) GC_malloc( sizeof( struct Cyc_Absyn_Enumfield));
_temp3368->name= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3375= yyvs;
struct _xenum_struct** _temp3377= _temp3375.curr + yyvsp_offset; if( _temp3377 <
_temp3375.base? 1: _temp3377 >= _temp3375.last_plus_one){ _throw( Null_Exception);}*
_temp3377;})); _temp3368->tag= 0; _temp3368->tvs= 0; _temp3368->typs= 0;
_temp3368->loc= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3369=
yyls; struct Cyc_Yyltype* _temp3371= _temp3369.curr + yylsp_offset; if(
_temp3371 < _temp3369.base? 1: _temp3371 >= _temp3369.last_plus_one){ _throw(
Null_Exception);}* _temp3371;}).first_line,({ struct _tagged_ptr3 _temp3372=
yyls; struct Cyc_Yyltype* _temp3374= _temp3372.curr + yylsp_offset; if(
_temp3374 < _temp3372.base? 1: _temp3374 >= _temp3372.last_plus_one){ _throw(
Null_Exception);}* _temp3374;}).last_line); _temp3368;})};( struct _xenum_struct*)
_temp3367;}); break; case 112: _LL3366: yyval=({ struct Cyc_EnumeratorField_tok_struct*
_temp3379=( struct Cyc_EnumeratorField_tok_struct*) GC_malloc( sizeof( struct
Cyc_EnumeratorField_tok_struct));* _temp3379=( struct Cyc_EnumeratorField_tok_struct){.tag=
Cyc_EnumeratorField_tok_tag,.f1=({ struct Cyc_Absyn_Enumfield* _temp3380=(
struct Cyc_Absyn_Enumfield*) GC_malloc( sizeof( struct Cyc_Absyn_Enumfield));
_temp3380->name= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3390= yyvs;
struct _xenum_struct** _temp3392= _temp3390.curr +( yyvsp_offset - 2); if(
_temp3392 < _temp3390.base? 1: _temp3392 >= _temp3390.last_plus_one){ _throw(
Null_Exception);}* _temp3392;})); _temp3380->tag=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp3387= yyvs; struct _xenum_struct** _temp3389= _temp3387.curr
+ yyvsp_offset; if( _temp3389 < _temp3387.base? 1: _temp3389 >= _temp3387.last_plus_one){
_throw( Null_Exception);}* _temp3389;})); _temp3380->tvs= 0; _temp3380->typs= 0;
_temp3380->loc= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3381=
yyls; struct Cyc_Yyltype* _temp3383= _temp3381.curr +( yylsp_offset - 2); if(
_temp3383 < _temp3381.base? 1: _temp3383 >= _temp3381.last_plus_one){ _throw(
Null_Exception);}* _temp3383;}).first_line,({ struct _tagged_ptr3 _temp3384=
yyls; struct Cyc_Yyltype* _temp3386= _temp3384.curr + yylsp_offset; if(
_temp3386 < _temp3384.base? 1: _temp3386 >= _temp3384.last_plus_one){ _throw(
Null_Exception);}* _temp3386;}).last_line); _temp3380;})};( struct _xenum_struct*)
_temp3379;}); break; case 113: _LL3378: { struct Cyc_List_List* typs=(( struct
Cyc_List_List*(*)( struct _tuple9*(* f)( struct Cyc_Position_Segment*, struct
_tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp3394= yyls; struct Cyc_Yyltype* _temp3396= _temp3394.curr +( yylsp_offset -
1); if( _temp3396 < _temp3394.base? 1: _temp3396 >= _temp3394.last_plus_one){
_throw( Null_Exception);}* _temp3396;}).first_line,({ struct _tagged_ptr3
_temp3397= yyls; struct Cyc_Yyltype* _temp3399= _temp3397.curr +( yylsp_offset -
1); if( _temp3399 < _temp3397.base? 1: _temp3399 >= _temp3397.last_plus_one){
_throw( Null_Exception);}* _temp3399;}).last_line),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp3400= yyvs; struct _xenum_struct** _temp3402= _temp3400.curr
+( yyvsp_offset - 1); if( _temp3402 < _temp3400.base? 1: _temp3402 >= _temp3400.last_plus_one){
_throw( Null_Exception);}* _temp3402;})))); struct Cyc_List_List* tvs=(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3403=
yyls; struct Cyc_Yyltype* _temp3405= _temp3403.curr +( yylsp_offset - 3); if(
_temp3405 < _temp3403.base? 1: _temp3405 >= _temp3403.last_plus_one){ _throw(
Null_Exception);}* _temp3405;}).first_line,({ struct _tagged_ptr3 _temp3406=
yyls; struct Cyc_Yyltype* _temp3408= _temp3406.curr +( yylsp_offset - 3); if(
_temp3408 < _temp3406.base? 1: _temp3408 >= _temp3406.last_plus_one){ _throw(
Null_Exception);}* _temp3408;}).last_line), Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp3409= yyvs; struct _xenum_struct** _temp3411= _temp3409.curr +(
yyvsp_offset - 3); if( _temp3411 < _temp3409.base? 1: _temp3411 >= _temp3409.last_plus_one){
_throw( Null_Exception);}* _temp3411;}))); yyval=({ struct Cyc_EnumeratorField_tok_struct*
_temp3412=( struct Cyc_EnumeratorField_tok_struct*) GC_malloc( sizeof( struct
Cyc_EnumeratorField_tok_struct));* _temp3412=( struct Cyc_EnumeratorField_tok_struct){.tag=
Cyc_EnumeratorField_tok_tag,.f1=({ struct Cyc_Absyn_Enumfield* _temp3413=(
struct Cyc_Absyn_Enumfield*) GC_malloc( sizeof( struct Cyc_Absyn_Enumfield));
_temp3413->name= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3420= yyvs;
struct _xenum_struct** _temp3422= _temp3420.curr +( yyvsp_offset - 4); if(
_temp3422 < _temp3420.base? 1: _temp3422 >= _temp3420.last_plus_one){ _throw(
Null_Exception);}* _temp3422;})); _temp3413->tag= 0; _temp3413->tvs= tvs;
_temp3413->typs= typs; _temp3413->loc= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3414= yyls; struct Cyc_Yyltype* _temp3416= _temp3414.curr +(
yylsp_offset - 4); if( _temp3416 < _temp3414.base? 1: _temp3416 >= _temp3414.last_plus_one){
_throw( Null_Exception);}* _temp3416;}).first_line,({ struct _tagged_ptr3
_temp3417= yyls; struct Cyc_Yyltype* _temp3419= _temp3417.curr + yylsp_offset;
if( _temp3419 < _temp3417.base? 1: _temp3419 >= _temp3417.last_plus_one){ _throw(
Null_Exception);}* _temp3419;}).last_line); _temp3413;})};( struct _xenum_struct*)
_temp3412;}); break;} case 114: _LL3393: yyval=({ struct _tagged_ptr2 _temp3424=
yyvs; struct _xenum_struct** _temp3426= _temp3424.curr + yyvsp_offset; if(
_temp3426 < _temp3424.base? 1: _temp3426 >= _temp3424.last_plus_one){ _throw(
Null_Exception);}* _temp3426;}); break; case 115: _LL3423: yyval=({ struct Cyc_Declarator_tok_struct*
_temp3428=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));*
_temp3428=( struct Cyc_Declarator_tok_struct){.tag= Cyc_Declarator_tok_tag,.f1=({
struct Cyc_Parse_Declarator* _temp3429=( struct Cyc_Parse_Declarator*) GC_malloc(
sizeof( struct Cyc_Parse_Declarator)); _temp3429->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3436= yyvs; struct _xenum_struct** _temp3438= _temp3436.curr
+ yyvsp_offset; if( _temp3438 < _temp3436.base? 1: _temp3438 >= _temp3436.last_plus_one){
_throw( Null_Exception);}* _temp3438;})))->id; _temp3429->tms=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_TypeModifierList_tok(({
struct _tagged_ptr2 _temp3430= yyvs; struct _xenum_struct** _temp3432= _temp3430.curr
+( yyvsp_offset - 1); if( _temp3432 < _temp3430.base? 1: _temp3432 >= _temp3430.last_plus_one){
_throw( Null_Exception);}* _temp3432;})),( Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp3433= yyvs; struct _xenum_struct** _temp3435= _temp3433.curr +
yyvsp_offset; if( _temp3435 < _temp3433.base? 1: _temp3435 >= _temp3433.last_plus_one){
_throw( Null_Exception);}* _temp3435;})))->tms); _temp3429;})};( struct
_xenum_struct*) _temp3428;}); break; case 116: _LL3427: yyval=({ struct Cyc_Declarator_tok_struct*
_temp3440=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));*
_temp3440=( struct Cyc_Declarator_tok_struct){.tag= Cyc_Declarator_tok_tag,.f1=({
struct Cyc_Parse_Declarator* _temp3441=( struct Cyc_Parse_Declarator*) GC_malloc(
sizeof( struct Cyc_Parse_Declarator)); _temp3441->id= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3442= yyvs; struct _xenum_struct** _temp3444= _temp3442.curr
+ yyvsp_offset; if( _temp3444 < _temp3442.base? 1: _temp3444 >= _temp3442.last_plus_one){
_throw( Null_Exception);}* _temp3444;})); _temp3441->tms= 0; _temp3441;})};(
struct _xenum_struct*) _temp3440;}); break; case 117: _LL3439: yyval=({ struct
_tagged_ptr2 _temp3446= yyvs; struct _xenum_struct** _temp3448= _temp3446.curr +(
yyvsp_offset - 1); if( _temp3448 < _temp3446.base? 1: _temp3448 >= _temp3446.last_plus_one){
_throw( Null_Exception);}* _temp3448;}); break; case 118: _LL3445: yyval=({
struct Cyc_Declarator_tok_struct* _temp3450=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));* _temp3450=( struct Cyc_Declarator_tok_struct){.tag=
Cyc_Declarator_tok_tag,.f1=({ struct Cyc_Parse_Declarator* _temp3451=( struct
Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp3451->id=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3456= yyvs;
struct _xenum_struct** _temp3458= _temp3456.curr +( yyvsp_offset - 2); if(
_temp3458 < _temp3456.base? 1: _temp3458 >= _temp3456.last_plus_one){ _throw(
Null_Exception);}* _temp3458;})))->id; _temp3451->tms=({ struct Cyc_List_List*
_temp3452=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3452->hd=( void*) Cyc_Absyn_Carray_mod; _temp3452->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3453= yyvs; struct _xenum_struct** _temp3455= _temp3453.curr
+( yyvsp_offset - 2); if( _temp3455 < _temp3453.base? 1: _temp3455 >= _temp3453.last_plus_one){
_throw( Null_Exception);}* _temp3455;})))->tms; _temp3452;}); _temp3451;})};(
struct _xenum_struct*) _temp3450;}); break; case 119: _LL3449: yyval=({ struct
Cyc_Declarator_tok_struct* _temp3460=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));* _temp3460=( struct Cyc_Declarator_tok_struct){.tag=
Cyc_Declarator_tok_tag,.f1=({ struct Cyc_Parse_Declarator* _temp3461=( struct
Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp3461->id=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3470= yyvs;
struct _xenum_struct** _temp3472= _temp3470.curr +( yyvsp_offset - 3); if(
_temp3472 < _temp3470.base? 1: _temp3472 >= _temp3470.last_plus_one){ _throw(
Null_Exception);}* _temp3472;})))->id; _temp3461->tms=({ struct Cyc_List_List*
_temp3462=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3462->hd=( void*)({ struct Cyc_Absyn_ConstArray_mod_struct* _temp3466=(
struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct));
_temp3466->tag= Cyc_Absyn_ConstArray_mod_tag; _temp3466->f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp3467= yyvs; struct _xenum_struct** _temp3469= _temp3467.curr
+( yyvsp_offset - 1); if( _temp3469 < _temp3467.base? 1: _temp3469 >= _temp3467.last_plus_one){
_throw( Null_Exception);}* _temp3469;}));( void*) _temp3466;}); _temp3462->tl=(
Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3463= yyvs; struct
_xenum_struct** _temp3465= _temp3463.curr +( yyvsp_offset - 3); if( _temp3465 <
_temp3463.base? 1: _temp3465 >= _temp3463.last_plus_one){ _throw( Null_Exception);}*
_temp3465;})))->tms; _temp3462;}); _temp3461;})};( struct _xenum_struct*)
_temp3460;}); break; case 120: _LL3459: { struct _tuple14 _temp3479; struct Cyc_Core_Opt*
_temp3480; int _temp3482; struct Cyc_List_List* _temp3484; struct _tuple14*
_temp3477= Cyc_yyget_ParamDeclListBool_tok(({ struct _tagged_ptr2 _temp3474=
yyvs; struct _xenum_struct** _temp3476= _temp3474.curr +( yyvsp_offset - 1); if(
_temp3476 < _temp3474.base? 1: _temp3476 >= _temp3474.last_plus_one){ _throw(
Null_Exception);}* _temp3476;})); _temp3479=* _temp3477; _LL3485: _temp3484=
_temp3479.f1; goto _LL3483; _LL3483: _temp3482= _temp3479.f2; goto _LL3481;
_LL3481: _temp3480= _temp3479.f3; goto _LL3478; _LL3478: yyval=({ struct Cyc_Declarator_tok_struct*
_temp3486=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));*
_temp3486=( struct Cyc_Declarator_tok_struct){.tag= Cyc_Declarator_tok_tag,.f1=({
struct Cyc_Parse_Declarator* _temp3487=( struct Cyc_Parse_Declarator*) GC_malloc(
sizeof( struct Cyc_Parse_Declarator)); _temp3487->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3494= yyvs; struct _xenum_struct** _temp3496= _temp3494.curr
+( yyvsp_offset - 3); if( _temp3496 < _temp3494.base? 1: _temp3496 >= _temp3494.last_plus_one){
_throw( Null_Exception);}* _temp3496;})))->id; _temp3487->tms=({ struct Cyc_List_List*
_temp3488=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3488->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct* _temp3492=(
struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3492->tag= Cyc_Absyn_Function_mod_tag; _temp3492->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp3493=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3493->tag= Cyc_Absyn_WithTypes_tag; _temp3493->f1= _temp3484; _temp3493->f2=
_temp3482; _temp3493->f3= _temp3480;( void*) _temp3493;});( void*) _temp3492;});
_temp3488->tl=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3489= yyvs;
struct _xenum_struct** _temp3491= _temp3489.curr +( yyvsp_offset - 3); if(
_temp3491 < _temp3489.base? 1: _temp3491 >= _temp3489.last_plus_one){ _throw(
Null_Exception);}* _temp3491;})))->tms; _temp3488;}); _temp3487;})};( struct
_xenum_struct*) _temp3486;}); break;} case 121: _LL3473: yyval=({ struct Cyc_Declarator_tok_struct*
_temp3498=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));*
_temp3498=( struct Cyc_Declarator_tok_struct){.tag= Cyc_Declarator_tok_tag,.f1=({
struct Cyc_Parse_Declarator* _temp3499=( struct Cyc_Parse_Declarator*) GC_malloc(
sizeof( struct Cyc_Parse_Declarator)); _temp3499->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3506= yyvs; struct _xenum_struct** _temp3508= _temp3506.curr
+( yyvsp_offset - 2); if( _temp3508 < _temp3506.base? 1: _temp3508 >= _temp3506.last_plus_one){
_throw( Null_Exception);}* _temp3508;})))->id; _temp3499->tms=({ struct Cyc_List_List*
_temp3500=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3500->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct* _temp3504=(
struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3504->tag= Cyc_Absyn_Function_mod_tag; _temp3504->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp3505=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3505->tag= Cyc_Absyn_WithTypes_tag; _temp3505->f1= 0; _temp3505->f2= 0;
_temp3505->f3= 0;( void*) _temp3505;});( void*) _temp3504;}); _temp3500->tl=(
Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3501= yyvs; struct
_xenum_struct** _temp3503= _temp3501.curr +( yyvsp_offset - 2); if( _temp3503 <
_temp3501.base? 1: _temp3503 >= _temp3501.last_plus_one){ _throw( Null_Exception);}*
_temp3503;})))->tms; _temp3500;}); _temp3499;})};( struct _xenum_struct*)
_temp3498;}); break; case 122: _LL3497: yyval=({ struct Cyc_Declarator_tok_struct*
_temp3510=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));*
_temp3510=( struct Cyc_Declarator_tok_struct){.tag= Cyc_Declarator_tok_tag,.f1=({
struct Cyc_Parse_Declarator* _temp3511=( struct Cyc_Parse_Declarator*) GC_malloc(
sizeof( struct Cyc_Parse_Declarator)); _temp3511->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3523= yyvs; struct _xenum_struct** _temp3525= _temp3523.curr
+( yyvsp_offset - 4); if( _temp3525 < _temp3523.base? 1: _temp3525 >= _temp3523.last_plus_one){
_throw( Null_Exception);}* _temp3525;})))->id; _temp3511->tms=({ struct Cyc_List_List*
_temp3512=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3512->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct* _temp3516=(
struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3516->tag= Cyc_Absyn_Function_mod_tag; _temp3516->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp3517=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3517->tag= Cyc_Absyn_WithTypes_tag; _temp3517->f1= 0; _temp3517->f2= 0;
_temp3517->f3=({ struct Cyc_Core_Opt* _temp3518=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3518->v=( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp3519=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3519->tag= Cyc_Absyn_JoinEff_tag; _temp3519->f1= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3520= yyvs; struct _xenum_struct** _temp3522= _temp3520.curr
+( yyvsp_offset - 1); if( _temp3522 < _temp3520.base? 1: _temp3522 >= _temp3520.last_plus_one){
_throw( Null_Exception);}* _temp3522;}));( void*) _temp3519;}); _temp3518;});(
void*) _temp3517;});( void*) _temp3516;}); _temp3512->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3513= yyvs; struct _xenum_struct** _temp3515= _temp3513.curr
+( yyvsp_offset - 4); if( _temp3515 < _temp3513.base? 1: _temp3515 >= _temp3513.last_plus_one){
_throw( Null_Exception);}* _temp3515;})))->tms; _temp3512;}); _temp3511;})};(
struct _xenum_struct*) _temp3510;}); break; case 123: _LL3509: yyval=({ struct
Cyc_Declarator_tok_struct* _temp3527=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));* _temp3527=( struct Cyc_Declarator_tok_struct){.tag=
Cyc_Declarator_tok_tag,.f1=({ struct Cyc_Parse_Declarator* _temp3528=( struct
Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp3528->id=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3544= yyvs;
struct _xenum_struct** _temp3546= _temp3544.curr +( yyvsp_offset - 3); if(
_temp3546 < _temp3544.base? 1: _temp3546 >= _temp3544.last_plus_one){ _throw(
Null_Exception);}* _temp3546;})))->id; _temp3528->tms=({ struct Cyc_List_List*
_temp3529=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3529->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct* _temp3533=(
struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3533->tag= Cyc_Absyn_Function_mod_tag; _temp3533->f1=( void*)({ struct Cyc_Absyn_NoTypes_struct*
_temp3534=( struct Cyc_Absyn_NoTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_NoTypes_struct));
_temp3534->tag= Cyc_Absyn_NoTypes_tag; _temp3534->f1= Cyc_yyget_IdList_tok(({
struct _tagged_ptr2 _temp3535= yyvs; struct _xenum_struct** _temp3537= _temp3535.curr
+( yyvsp_offset - 1); if( _temp3537 < _temp3535.base? 1: _temp3537 >= _temp3535.last_plus_one){
_throw( Null_Exception);}* _temp3537;})); _temp3534->f2= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3538= yyls; struct Cyc_Yyltype* _temp3540= _temp3538.curr
+( yylsp_offset - 3); if( _temp3540 < _temp3538.base? 1: _temp3540 >= _temp3538.last_plus_one){
_throw( Null_Exception);}* _temp3540;}).first_line,({ struct _tagged_ptr3
_temp3541= yyls; struct Cyc_Yyltype* _temp3543= _temp3541.curr + yylsp_offset;
if( _temp3543 < _temp3541.base? 1: _temp3543 >= _temp3541.last_plus_one){ _throw(
Null_Exception);}* _temp3543;}).last_line);( void*) _temp3534;});( void*)
_temp3533;}); _temp3529->tl=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2
_temp3530= yyvs; struct _xenum_struct** _temp3532= _temp3530.curr +(
yyvsp_offset - 3); if( _temp3532 < _temp3530.base? 1: _temp3532 >= _temp3530.last_plus_one){
_throw( Null_Exception);}* _temp3532;})))->tms; _temp3529;}); _temp3528;})};(
struct _xenum_struct*) _temp3527;}); break; case 124: _LL3526: { struct Cyc_List_List*
ts=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3548=
yyls; struct Cyc_Yyltype* _temp3550= _temp3548.curr +( yylsp_offset - 2); if(
_temp3550 < _temp3548.base? 1: _temp3550 >= _temp3548.last_plus_one){ _throw(
Null_Exception);}* _temp3550;}).first_line,({ struct _tagged_ptr3 _temp3551=
yyls; struct Cyc_Yyltype* _temp3553= _temp3551.curr + yylsp_offset; if(
_temp3553 < _temp3551.base? 1: _temp3553 >= _temp3551.last_plus_one){ _throw(
Null_Exception);}* _temp3553;}).last_line),(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp3554= yyvs; struct _xenum_struct** _temp3556= _temp3554.curr +(
yyvsp_offset - 1); if( _temp3556 < _temp3554.base? 1: _temp3556 >= _temp3554.last_plus_one){
_throw( Null_Exception);}* _temp3556;})))); yyval=({ struct Cyc_Declarator_tok_struct*
_temp3557=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));*
_temp3557=( struct Cyc_Declarator_tok_struct){.tag= Cyc_Declarator_tok_tag,.f1=({
struct Cyc_Parse_Declarator* _temp3558=( struct Cyc_Parse_Declarator*) GC_malloc(
sizeof( struct Cyc_Parse_Declarator)); _temp3558->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3570= yyvs; struct _xenum_struct** _temp3572= _temp3570.curr
+( yyvsp_offset - 3); if( _temp3572 < _temp3570.base? 1: _temp3572 >= _temp3570.last_plus_one){
_throw( Null_Exception);}* _temp3572;})))->id; _temp3558->tms=({ struct Cyc_List_List*
_temp3559=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3559->hd=( void*)({ struct Cyc_Absyn_TypeParams_mod_struct* _temp3563=(
struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct));
_temp3563->tag= Cyc_Absyn_TypeParams_mod_tag; _temp3563->f1= ts; _temp3563->f2=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3564= yyls; struct Cyc_Yyltype*
_temp3566= _temp3564.curr +( yylsp_offset - 3); if( _temp3566 < _temp3564.base?
1: _temp3566 >= _temp3564.last_plus_one){ _throw( Null_Exception);}* _temp3566;}).first_line,({
struct _tagged_ptr3 _temp3567= yyls; struct Cyc_Yyltype* _temp3569= _temp3567.curr
+ yylsp_offset; if( _temp3569 < _temp3567.base? 1: _temp3569 >= _temp3567.last_plus_one){
_throw( Null_Exception);}* _temp3569;}).last_line); _temp3563->f3= 0;( void*)
_temp3563;}); _temp3559->tl=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2
_temp3560= yyvs; struct _xenum_struct** _temp3562= _temp3560.curr +(
yyvsp_offset - 3); if( _temp3562 < _temp3560.base? 1: _temp3562 >= _temp3560.last_plus_one){
_throw( Null_Exception);}* _temp3562;})))->tms; _temp3559;}); _temp3558;})};(
struct _xenum_struct*) _temp3557;}); break;} case 125: _LL3547:(( struct Cyc_Lexing_lexbuf*)
Cyc_Parse_lbuf->v)->lex_curr_pos -= 1;{ struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3574= yyls; struct Cyc_Yyltype* _temp3576= _temp3574.curr
+( yylsp_offset - 2); if( _temp3576 < _temp3574.base? 1: _temp3576 >= _temp3574.last_plus_one){
_throw( Null_Exception);}* _temp3576;}).first_line,({ struct _tagged_ptr3
_temp3577= yyls; struct Cyc_Yyltype* _temp3579= _temp3577.curr + yylsp_offset;
if( _temp3579 < _temp3577.base? 1: _temp3579 >= _temp3577.last_plus_one){ _throw(
Null_Exception);}* _temp3579;}).last_line),(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp3580= yyvs; struct _xenum_struct** _temp3582= _temp3580.curr +(
yyvsp_offset - 1); if( _temp3582 < _temp3580.base? 1: _temp3582 >= _temp3580.last_plus_one){
_throw( Null_Exception);}* _temp3582;})))); yyval=({ struct Cyc_Declarator_tok_struct*
_temp3583=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));*
_temp3583=( struct Cyc_Declarator_tok_struct){.tag= Cyc_Declarator_tok_tag,.f1=({
struct Cyc_Parse_Declarator* _temp3584=( struct Cyc_Parse_Declarator*) GC_malloc(
sizeof( struct Cyc_Parse_Declarator)); _temp3584->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3596= yyvs; struct _xenum_struct** _temp3598= _temp3596.curr
+( yyvsp_offset - 3); if( _temp3598 < _temp3596.base? 1: _temp3598 >= _temp3596.last_plus_one){
_throw( Null_Exception);}* _temp3598;})))->id; _temp3584->tms=({ struct Cyc_List_List*
_temp3585=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3585->hd=( void*)({ struct Cyc_Absyn_TypeParams_mod_struct* _temp3589=(
struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct));
_temp3589->tag= Cyc_Absyn_TypeParams_mod_tag; _temp3589->f1= ts; _temp3589->f2=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3590= yyls; struct Cyc_Yyltype*
_temp3592= _temp3590.curr +( yylsp_offset - 3); if( _temp3592 < _temp3590.base?
1: _temp3592 >= _temp3590.last_plus_one){ _throw( Null_Exception);}* _temp3592;}).first_line,({
struct _tagged_ptr3 _temp3593= yyls; struct Cyc_Yyltype* _temp3595= _temp3593.curr
+ yylsp_offset; if( _temp3595 < _temp3593.base? 1: _temp3595 >= _temp3593.last_plus_one){
_throw( Null_Exception);}* _temp3595;}).last_line); _temp3589->f3= 0;( void*)
_temp3589;}); _temp3585->tl=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2
_temp3586= yyvs; struct _xenum_struct** _temp3588= _temp3586.curr +(
yyvsp_offset - 3); if( _temp3588 < _temp3586.base? 1: _temp3588 >= _temp3586.last_plus_one){
_throw( Null_Exception);}* _temp3588;})))->tms; _temp3585;}); _temp3584;})};(
struct _xenum_struct*) _temp3583;}); break;} case 126: _LL3573: yyval=({ struct
Cyc_Declarator_tok_struct* _temp3600=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));* _temp3600=( struct Cyc_Declarator_tok_struct){.tag=
Cyc_Declarator_tok_tag,.f1=({ struct Cyc_Parse_Declarator* _temp3601=( struct
Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp3601->id=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3616= yyvs;
struct _xenum_struct** _temp3618= _temp3616.curr +( yyvsp_offset - 1); if(
_temp3618 < _temp3616.base? 1: _temp3618 >= _temp3616.last_plus_one){ _throw(
Null_Exception);}* _temp3618;})))->id; _temp3601->tms=({ struct Cyc_List_List*
_temp3602=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3602->hd=( void*)({ struct Cyc_Absyn_Attributes_mod_struct* _temp3606=(
struct Cyc_Absyn_Attributes_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct));
_temp3606->tag= Cyc_Absyn_Attributes_mod_tag; _temp3606->f1= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3607= yyls; struct Cyc_Yyltype* _temp3609= _temp3607.curr
+ yylsp_offset; if( _temp3609 < _temp3607.base? 1: _temp3609 >= _temp3607.last_plus_one){
_throw( Null_Exception);}* _temp3609;}).first_line,({ struct _tagged_ptr3
_temp3610= yyls; struct Cyc_Yyltype* _temp3612= _temp3610.curr + yylsp_offset;
if( _temp3612 < _temp3610.base? 1: _temp3612 >= _temp3610.last_plus_one){ _throw(
Null_Exception);}* _temp3612;}).last_line); _temp3606->f2= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp3613= yyvs; struct _xenum_struct** _temp3615= _temp3613.curr
+ yyvsp_offset; if( _temp3615 < _temp3613.base? 1: _temp3615 >= _temp3613.last_plus_one){
_throw( Null_Exception);}* _temp3615;}));( void*) _temp3606;}); _temp3602->tl=(
Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3603= yyvs; struct
_xenum_struct** _temp3605= _temp3603.curr +( yyvsp_offset - 1); if( _temp3605 <
_temp3603.base? 1: _temp3605 >= _temp3603.last_plus_one){ _throw( Null_Exception);}*
_temp3605;})))->tms; _temp3602;}); _temp3601;})};( struct _xenum_struct*)
_temp3600;}); break; case 127: _LL3599: yyval=({ struct Cyc_TypeModifierList_tok_struct*
_temp3620=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct));* _temp3620=( struct Cyc_TypeModifierList_tok_struct){.tag=
Cyc_TypeModifierList_tok_tag,.f1=({ struct Cyc_List_List* _temp3621=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3621->hd=( void*)({ struct Cyc_Absyn_Pointer_mod_struct*
_temp3622=( struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp3622->tag= Cyc_Absyn_Pointer_mod_tag; _temp3622->f1=( void*) Cyc_yyget_Pointer_Sort_tok(({
struct _tagged_ptr2 _temp3623= yyvs; struct _xenum_struct** _temp3625= _temp3623.curr
+( yyvsp_offset - 1); if( _temp3625 < _temp3623.base? 1: _temp3625 >= _temp3623.last_plus_one){
_throw( Null_Exception);}* _temp3625;})); _temp3622->f2=( void*) Cyc_yyget_Rgn_tok(({
struct _tagged_ptr2 _temp3626= yyvs; struct _xenum_struct** _temp3628= _temp3626.curr
+ yyvsp_offset; if( _temp3628 < _temp3626.base? 1: _temp3628 >= _temp3626.last_plus_one){
_throw( Null_Exception);}* _temp3628;})); _temp3622->f3= Cyc_Absyn_empty_tqual();(
void*) _temp3622;}); _temp3621->tl= 0; _temp3621;})};( struct _xenum_struct*)
_temp3620;}); break; case 128: _LL3619: yyval=({ struct Cyc_TypeModifierList_tok_struct*
_temp3630=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct));* _temp3630=( struct Cyc_TypeModifierList_tok_struct){.tag=
Cyc_TypeModifierList_tok_tag,.f1=({ struct Cyc_List_List* _temp3631=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3631->hd=( void*)({ struct Cyc_Absyn_Pointer_mod_struct*
_temp3632=( struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp3632->tag= Cyc_Absyn_Pointer_mod_tag; _temp3632->f1=( void*) Cyc_yyget_Pointer_Sort_tok(({
struct _tagged_ptr2 _temp3633= yyvs; struct _xenum_struct** _temp3635= _temp3633.curr
+( yyvsp_offset - 2); if( _temp3635 < _temp3633.base? 1: _temp3635 >= _temp3633.last_plus_one){
_throw( Null_Exception);}* _temp3635;})); _temp3632->f2=( void*) Cyc_yyget_Rgn_tok(({
struct _tagged_ptr2 _temp3636= yyvs; struct _xenum_struct** _temp3638= _temp3636.curr
+( yyvsp_offset - 1); if( _temp3638 < _temp3636.base? 1: _temp3638 >= _temp3636.last_plus_one){
_throw( Null_Exception);}* _temp3638;})); _temp3632->f3= Cyc_yyget_TypeQual_tok(({
struct _tagged_ptr2 _temp3639= yyvs; struct _xenum_struct** _temp3641= _temp3639.curr
+ yyvsp_offset; if( _temp3641 < _temp3639.base? 1: _temp3641 >= _temp3639.last_plus_one){
_throw( Null_Exception);}* _temp3641;}));( void*) _temp3632;}); _temp3631->tl= 0;
_temp3631;})};( struct _xenum_struct*) _temp3630;}); break; case 129: _LL3629:
yyval=({ struct Cyc_TypeModifierList_tok_struct* _temp3643=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct));* _temp3643=( struct
Cyc_TypeModifierList_tok_struct){.tag= Cyc_TypeModifierList_tok_tag,.f1=({
struct Cyc_List_List* _temp3644=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3644->hd=( void*)({ struct Cyc_Absyn_Pointer_mod_struct*
_temp3648=( struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp3648->tag= Cyc_Absyn_Pointer_mod_tag; _temp3648->f1=( void*) Cyc_yyget_Pointer_Sort_tok(({
struct _tagged_ptr2 _temp3649= yyvs; struct _xenum_struct** _temp3651= _temp3649.curr
+( yyvsp_offset - 2); if( _temp3651 < _temp3649.base? 1: _temp3651 >= _temp3649.last_plus_one){
_throw( Null_Exception);}* _temp3651;})); _temp3648->f2=( void*) Cyc_yyget_Rgn_tok(({
struct _tagged_ptr2 _temp3652= yyvs; struct _xenum_struct** _temp3654= _temp3652.curr
+( yyvsp_offset - 1); if( _temp3654 < _temp3652.base? 1: _temp3654 >= _temp3652.last_plus_one){
_throw( Null_Exception);}* _temp3654;})); _temp3648->f3= Cyc_Absyn_empty_tqual();(
void*) _temp3648;}); _temp3644->tl= Cyc_yyget_TypeModifierList_tok(({ struct
_tagged_ptr2 _temp3645= yyvs; struct _xenum_struct** _temp3647= _temp3645.curr +
yyvsp_offset; if( _temp3647 < _temp3645.base? 1: _temp3647 >= _temp3645.last_plus_one){
_throw( Null_Exception);}* _temp3647;})); _temp3644;})};( struct _xenum_struct*)
_temp3643;}); break; case 130: _LL3642: yyval=({ struct Cyc_TypeModifierList_tok_struct*
_temp3656=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct));* _temp3656=( struct Cyc_TypeModifierList_tok_struct){.tag=
Cyc_TypeModifierList_tok_tag,.f1=({ struct Cyc_List_List* _temp3657=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3657->hd=( void*)({ struct Cyc_Absyn_Pointer_mod_struct*
_temp3661=( struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp3661->tag= Cyc_Absyn_Pointer_mod_tag; _temp3661->f1=( void*) Cyc_yyget_Pointer_Sort_tok(({
struct _tagged_ptr2 _temp3662= yyvs; struct _xenum_struct** _temp3664= _temp3662.curr
+( yyvsp_offset - 3); if( _temp3664 < _temp3662.base? 1: _temp3664 >= _temp3662.last_plus_one){
_throw( Null_Exception);}* _temp3664;})); _temp3661->f2=( void*) Cyc_yyget_Rgn_tok(({
struct _tagged_ptr2 _temp3665= yyvs; struct _xenum_struct** _temp3667= _temp3665.curr
+( yyvsp_offset - 2); if( _temp3667 < _temp3665.base? 1: _temp3667 >= _temp3665.last_plus_one){
_throw( Null_Exception);}* _temp3667;})); _temp3661->f3= Cyc_yyget_TypeQual_tok(({
struct _tagged_ptr2 _temp3668= yyvs; struct _xenum_struct** _temp3670= _temp3668.curr
+( yyvsp_offset - 1); if( _temp3670 < _temp3668.base? 1: _temp3670 >= _temp3668.last_plus_one){
_throw( Null_Exception);}* _temp3670;}));( void*) _temp3661;}); _temp3657->tl=
Cyc_yyget_TypeModifierList_tok(({ struct _tagged_ptr2 _temp3658= yyvs; struct
_xenum_struct** _temp3660= _temp3658.curr + yyvsp_offset; if( _temp3660 <
_temp3658.base? 1: _temp3660 >= _temp3658.last_plus_one){ _throw( Null_Exception);}*
_temp3660;})); _temp3657;})};( struct _xenum_struct*) _temp3656;}); break; case
131: _LL3655: yyval=({ struct Cyc_Pointer_Sort_tok_struct* _temp3672=( struct
Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));*
_temp3672=( struct Cyc_Pointer_Sort_tok_struct){.tag= Cyc_Pointer_Sort_tok_tag,.f1=({
struct Cyc_Absyn_Nullable_ps_struct* _temp3673=( struct Cyc_Absyn_Nullable_ps_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct)); _temp3673->tag= Cyc_Absyn_Nullable_ps_tag;
_temp3673->f1= Cyc_Absyn_signed_int_exp( 1, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3674= yyls; struct Cyc_Yyltype* _temp3676= _temp3674.curr
+ yylsp_offset; if( _temp3676 < _temp3674.base? 1: _temp3676 >= _temp3674.last_plus_one){
_throw( Null_Exception);}* _temp3676;}).first_line,({ struct _tagged_ptr3
_temp3677= yyls; struct Cyc_Yyltype* _temp3679= _temp3677.curr + yylsp_offset;
if( _temp3679 < _temp3677.base? 1: _temp3679 >= _temp3677.last_plus_one){ _throw(
Null_Exception);}* _temp3679;}).last_line));( void*) _temp3673;})};( struct
_xenum_struct*) _temp3672;}); break; case 132: _LL3671: yyval=({ struct Cyc_Pointer_Sort_tok_struct*
_temp3681=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));*
_temp3681=( struct Cyc_Pointer_Sort_tok_struct){.tag= Cyc_Pointer_Sort_tok_tag,.f1=({
struct Cyc_Absyn_NonNullable_ps_struct* _temp3682=( struct Cyc_Absyn_NonNullable_ps_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_NonNullable_ps_struct)); _temp3682->tag= Cyc_Absyn_NonNullable_ps_tag;
_temp3682->f1= Cyc_Absyn_signed_int_exp( 1, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3683= yyls; struct Cyc_Yyltype* _temp3685= _temp3683.curr
+ yylsp_offset; if( _temp3685 < _temp3683.base? 1: _temp3685 >= _temp3683.last_plus_one){
_throw( Null_Exception);}* _temp3685;}).first_line,({ struct _tagged_ptr3
_temp3686= yyls; struct Cyc_Yyltype* _temp3688= _temp3686.curr + yylsp_offset;
if( _temp3688 < _temp3686.base? 1: _temp3688 >= _temp3686.last_plus_one){ _throw(
Null_Exception);}* _temp3688;}).last_line));( void*) _temp3682;})};( struct
_xenum_struct*) _temp3681;}); break; case 133: _LL3680: yyval=({ struct Cyc_Pointer_Sort_tok_struct*
_temp3690=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));*
_temp3690=( struct Cyc_Pointer_Sort_tok_struct){.tag= Cyc_Pointer_Sort_tok_tag,.f1=({
struct Cyc_Absyn_Nullable_ps_struct* _temp3691=( struct Cyc_Absyn_Nullable_ps_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct)); _temp3691->tag= Cyc_Absyn_Nullable_ps_tag;
_temp3691->f1= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp3692= yyvs; struct
_xenum_struct** _temp3694= _temp3692.curr +( yyvsp_offset - 1); if( _temp3694 <
_temp3692.base? 1: _temp3694 >= _temp3692.last_plus_one){ _throw( Null_Exception);}*
_temp3694;}));( void*) _temp3691;})};( struct _xenum_struct*) _temp3690;});
break; case 134: _LL3689: yyval=({ struct Cyc_Pointer_Sort_tok_struct* _temp3696=(
struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));*
_temp3696=( struct Cyc_Pointer_Sort_tok_struct){.tag= Cyc_Pointer_Sort_tok_tag,.f1=({
struct Cyc_Absyn_NonNullable_ps_struct* _temp3697=( struct Cyc_Absyn_NonNullable_ps_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_NonNullable_ps_struct)); _temp3697->tag= Cyc_Absyn_NonNullable_ps_tag;
_temp3697->f1= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp3698= yyvs; struct
_xenum_struct** _temp3700= _temp3698.curr +( yyvsp_offset - 1); if( _temp3700 <
_temp3698.base? 1: _temp3700 >= _temp3698.last_plus_one){ _throw( Null_Exception);}*
_temp3700;}));( void*) _temp3697;})};( struct _xenum_struct*) _temp3696;});
break; case 135: _LL3695: yyval=({ struct Cyc_Pointer_Sort_tok_struct* _temp3702=(
struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));*
_temp3702=( struct Cyc_Pointer_Sort_tok_struct){.tag= Cyc_Pointer_Sort_tok_tag,.f1=
Cyc_Absyn_TaggedArray_ps};( struct _xenum_struct*) _temp3702;}); break; case 136:
_LL3701: yyval=({ struct Cyc_Rgn_tok_struct* _temp3704=( struct Cyc_Rgn_tok_struct*)
GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));* _temp3704=( struct Cyc_Rgn_tok_struct){.tag=
Cyc_Rgn_tok_tag,.f1= Cyc_Absyn_HeapRgn};( struct _xenum_struct*) _temp3704;});
break; case 137: _LL3703: yyval=({ struct Cyc_Rgn_tok_struct* _temp3706=( struct
Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));* _temp3706=(
struct Cyc_Rgn_tok_struct){.tag= Cyc_Rgn_tok_tag,.f1= Cyc_Parse_id2type( Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp3707= yyvs; struct _xenum_struct** _temp3709= _temp3707.curr
+ yyvsp_offset; if( _temp3709 < _temp3707.base? 1: _temp3709 >= _temp3707.last_plus_one){
_throw( Null_Exception);}* _temp3709;})),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)( Cyc_Absyn_RgnKind))};( struct _xenum_struct*) _temp3706;});
break; case 138: _LL3705: if( Cyc_yyget_Kind_tok(({ struct _tagged_ptr2
_temp3711= yyvs; struct _xenum_struct** _temp3713= _temp3711.curr + yyvsp_offset;
if( _temp3713 < _temp3711.base? 1: _temp3713 >= _temp3711.last_plus_one){ _throw(
Null_Exception);}* _temp3713;})) != Cyc_Absyn_RgnKind){ Cyc_Parse_err(( struct
_tagged_string)({ char* _temp3714=( char*)"expecting region kind\n"; struct
_tagged_string _temp3715; _temp3715.curr= _temp3714; _temp3715.base= _temp3714;
_temp3715.last_plus_one= _temp3714 + 23; _temp3715;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3716= yyls; struct Cyc_Yyltype* _temp3718= _temp3716.curr
+ yylsp_offset; if( _temp3718 < _temp3716.base? 1: _temp3718 >= _temp3716.last_plus_one){
_throw( Null_Exception);}* _temp3718;}).first_line,({ struct _tagged_ptr3
_temp3719= yyls; struct Cyc_Yyltype* _temp3721= _temp3719.curr + yylsp_offset;
if( _temp3721 < _temp3719.base? 1: _temp3721 >= _temp3719.last_plus_one){ _throw(
Null_Exception);}* _temp3721;}).last_line));} yyval=({ struct Cyc_Rgn_tok_struct*
_temp3722=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));*
_temp3722=( struct Cyc_Rgn_tok_struct){.tag= Cyc_Rgn_tok_tag,.f1= Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp3723= yyvs; struct
_xenum_struct** _temp3725= _temp3723.curr +( yyvsp_offset - 2); if( _temp3725 <
_temp3723.base? 1: _temp3725 >= _temp3723.last_plus_one){ _throw( Null_Exception);}*
_temp3725;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
Cyc_Absyn_RgnKind))};( struct _xenum_struct*) _temp3722;}); break; case 139:
_LL3710: yyval=({ struct Cyc_Rgn_tok_struct* _temp3727=( struct Cyc_Rgn_tok_struct*)
GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));* _temp3727=( struct Cyc_Rgn_tok_struct){.tag=
Cyc_Rgn_tok_tag,.f1= Cyc_Absyn_new_evar( Cyc_Absyn_RgnKind)};( struct
_xenum_struct*) _temp3727;}); break; case 140: _LL3726: yyval=({ struct
_tagged_ptr2 _temp3729= yyvs; struct _xenum_struct** _temp3731= _temp3729.curr +
yyvsp_offset; if( _temp3731 < _temp3729.base? 1: _temp3731 >= _temp3729.last_plus_one){
_throw( Null_Exception);}* _temp3731;}); break; case 141: _LL3728: yyval=({
struct Cyc_TypeQual_tok_struct* _temp3733=( struct Cyc_TypeQual_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct));* _temp3733=( struct Cyc_TypeQual_tok_struct){.tag=
Cyc_TypeQual_tok_tag,.f1= Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok(({
struct _tagged_ptr2 _temp3734= yyvs; struct _xenum_struct** _temp3736= _temp3734.curr
+( yyvsp_offset - 1); if( _temp3736 < _temp3734.base? 1: _temp3736 >= _temp3734.last_plus_one){
_throw( Null_Exception);}* _temp3736;})), Cyc_yyget_TypeQual_tok(({ struct
_tagged_ptr2 _temp3737= yyvs; struct _xenum_struct** _temp3739= _temp3737.curr +
yyvsp_offset; if( _temp3739 < _temp3737.base? 1: _temp3739 >= _temp3737.last_plus_one){
_throw( Null_Exception);}* _temp3739;})))};( struct _xenum_struct*) _temp3733;});
break; case 142: _LL3732: yyval=({ struct Cyc_ParamDeclListBool_tok_struct*
_temp3741=( struct Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct
Cyc_ParamDeclListBool_tok_struct));* _temp3741=( struct Cyc_ParamDeclListBool_tok_struct){.tag=
Cyc_ParamDeclListBool_tok_tag,.f1=({ struct _tuple14* _temp3742=( struct
_tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp3742->f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp3743= yyvs; struct _xenum_struct** _temp3745= _temp3743.curr
+ yyvsp_offset; if( _temp3745 < _temp3743.base? 1: _temp3745 >= _temp3743.last_plus_one){
_throw( Null_Exception);}* _temp3745;}))); _temp3742->f2= 0; _temp3742->f3= 0;
_temp3742;})};( struct _xenum_struct*) _temp3741;}); break; case 143: _LL3740:
yyval=({ struct Cyc_ParamDeclListBool_tok_struct* _temp3747=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct));* _temp3747=(
struct Cyc_ParamDeclListBool_tok_struct){.tag= Cyc_ParamDeclListBool_tok_tag,.f1=({
struct _tuple14* _temp3748=( struct _tuple14*) GC_malloc( sizeof( struct
_tuple14)); _temp3748->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({ struct _tagged_ptr2 _temp3749=
yyvs; struct _xenum_struct** _temp3751= _temp3749.curr +( yyvsp_offset - 2); if(
_temp3751 < _temp3749.base? 1: _temp3751 >= _temp3749.last_plus_one){ _throw(
Null_Exception);}* _temp3751;}))); _temp3748->f2= 1; _temp3748->f3= 0; _temp3748;})};(
struct _xenum_struct*) _temp3747;}); break; case 144: _LL3746: yyval=({ struct
Cyc_ParamDeclListBool_tok_struct* _temp3753=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct));* _temp3753=(
struct Cyc_ParamDeclListBool_tok_struct){.tag= Cyc_ParamDeclListBool_tok_tag,.f1=({
struct _tuple14* _temp3754=( struct _tuple14*) GC_malloc( sizeof( struct
_tuple14)); _temp3754->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({ struct _tagged_ptr2 _temp3760=
yyvs; struct _xenum_struct** _temp3762= _temp3760.curr +( yyvsp_offset - 2); if(
_temp3762 < _temp3760.base? 1: _temp3762 >= _temp3760.last_plus_one){ _throw(
Null_Exception);}* _temp3762;}))); _temp3754->f2= 0; _temp3754->f3=( struct Cyc_Core_Opt*)({
struct Cyc_Core_Opt* _temp3755=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3755->v=( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp3756=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3756->tag= Cyc_Absyn_JoinEff_tag; _temp3756->f1= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3757= yyvs; struct _xenum_struct** _temp3759= _temp3757.curr
+ yyvsp_offset; if( _temp3759 < _temp3757.base? 1: _temp3759 >= _temp3757.last_plus_one){
_throw( Null_Exception);}* _temp3759;}));( void*) _temp3756;}); _temp3755;});
_temp3754;})};( struct _xenum_struct*) _temp3753;}); break; case 145: _LL3752:
yyval=({ struct _tagged_ptr2 _temp3764= yyvs; struct _xenum_struct** _temp3766=
_temp3764.curr + yyvsp_offset; if( _temp3766 < _temp3764.base? 1: _temp3766 >=
_temp3764.last_plus_one){ _throw( Null_Exception);}* _temp3766;}); break; case
146: _LL3763: yyval=({ struct Cyc_TypeList_tok_struct* _temp3768=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));* _temp3768=( struct Cyc_TypeList_tok_struct){.tag=
Cyc_TypeList_tok_tag,.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp3769= yyvs; struct _xenum_struct** _temp3771= _temp3769.curr +(
yyvsp_offset - 2); if( _temp3771 < _temp3769.base? 1: _temp3771 >= _temp3769.last_plus_one){
_throw( Null_Exception);}* _temp3771;})), Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp3772= yyvs; struct _xenum_struct** _temp3774= _temp3772.curr +
yyvsp_offset; if( _temp3774 < _temp3772.base? 1: _temp3774 >= _temp3772.last_plus_one){
_throw( Null_Exception);}* _temp3774;})))};( struct _xenum_struct*) _temp3768;});
break; case 147: _LL3767: yyval=({ struct Cyc_TypeList_tok_struct* _temp3776=(
struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));*
_temp3776=( struct Cyc_TypeList_tok_struct){.tag= Cyc_TypeList_tok_tag,.f1= 0};(
struct _xenum_struct*) _temp3776;}); break; case 148: _LL3775: yyval=({ struct
_tagged_ptr2 _temp3778= yyvs; struct _xenum_struct** _temp3780= _temp3778.curr +(
yyvsp_offset - 1); if( _temp3780 < _temp3778.base? 1: _temp3780 >= _temp3778.last_plus_one){
_throw( Null_Exception);}* _temp3780;}); break; case 149: _LL3777: yyval=({
struct Cyc_TypeList_tok_struct* _temp3782=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));* _temp3782=( struct Cyc_TypeList_tok_struct){.tag=
Cyc_TypeList_tok_tag,.f1=({ struct Cyc_List_List* _temp3783=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3783->hd=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp3784= yyvs; struct
_xenum_struct** _temp3786= _temp3784.curr + yyvsp_offset; if( _temp3786 <
_temp3784.base? 1: _temp3786 >= _temp3784.last_plus_one){ _throw( Null_Exception);}*
_temp3786;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
Cyc_Absyn_EffKind)); _temp3783->tl= 0; _temp3783;})};( struct _xenum_struct*)
_temp3782;}); break; case 150: _LL3781: if( Cyc_yyget_Kind_tok(({ struct
_tagged_ptr2 _temp3788= yyvs; struct _xenum_struct** _temp3790= _temp3788.curr +
yyvsp_offset; if( _temp3790 < _temp3788.base? 1: _temp3790 >= _temp3788.last_plus_one){
_throw( Null_Exception);}* _temp3790;})) != Cyc_Absyn_EffKind){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp3791=( char*)"expecing effect kind (E)";
struct _tagged_string _temp3792; _temp3792.curr= _temp3791; _temp3792.base=
_temp3791; _temp3792.last_plus_one= _temp3791 + 25; _temp3792;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3793= yyls; struct Cyc_Yyltype* _temp3795= _temp3793.curr
+ yylsp_offset; if( _temp3795 < _temp3793.base? 1: _temp3795 >= _temp3793.last_plus_one){
_throw( Null_Exception);}* _temp3795;}).first_line,({ struct _tagged_ptr3
_temp3796= yyls; struct Cyc_Yyltype* _temp3798= _temp3796.curr + yylsp_offset;
if( _temp3798 < _temp3796.base? 1: _temp3798 >= _temp3796.last_plus_one){ _throw(
Null_Exception);}* _temp3798;}).last_line));} yyval=({ struct Cyc_TypeList_tok_struct*
_temp3799=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));*
_temp3799=( struct Cyc_TypeList_tok_struct){.tag= Cyc_TypeList_tok_tag,.f1=({
struct Cyc_List_List* _temp3800=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3800->hd=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp3801= yyvs; struct _xenum_struct** _temp3803= _temp3801.curr
+( yyvsp_offset - 2); if( _temp3803 < _temp3801.base? 1: _temp3803 >= _temp3801.last_plus_one){
_throw( Null_Exception);}* _temp3803;})),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)( Cyc_Absyn_EffKind)); _temp3800->tl= 0; _temp3800;})};(
struct _xenum_struct*) _temp3799;}); break; case 151: _LL3787: yyval=({ struct
Cyc_TypeList_tok_struct* _temp3805=( struct Cyc_TypeList_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeList_tok_struct));* _temp3805=( struct Cyc_TypeList_tok_struct){.tag=
Cyc_TypeList_tok_tag,.f1=({ struct Cyc_List_List* _temp3806=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3806->hd=( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp3807=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3807->tag= Cyc_Absyn_AccessEff_tag; _temp3807->f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp3808= yyvs; struct
_xenum_struct** _temp3810= _temp3808.curr + yyvsp_offset; if( _temp3810 <
_temp3808.base? 1: _temp3810 >= _temp3808.last_plus_one){ _throw( Null_Exception);}*
_temp3810;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
Cyc_Absyn_RgnKind));( void*) _temp3807;}); _temp3806->tl= 0; _temp3806;})};(
struct _xenum_struct*) _temp3805;}); break; case 152: _LL3804: yyval=({ struct
Cyc_TypeList_tok_struct* _temp3812=( struct Cyc_TypeList_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeList_tok_struct));* _temp3812=( struct Cyc_TypeList_tok_struct){.tag=
Cyc_TypeList_tok_tag,.f1=({ struct Cyc_List_List* _temp3813=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3813->hd=( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp3817=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3817->tag= Cyc_Absyn_AccessEff_tag; _temp3817->f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp3818= yyvs; struct
_xenum_struct** _temp3820= _temp3818.curr +( yyvsp_offset - 2); if( _temp3820 <
_temp3818.base? 1: _temp3820 >= _temp3818.last_plus_one){ _throw( Null_Exception);}*
_temp3820;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
Cyc_Absyn_RgnKind));( void*) _temp3817;}); _temp3813->tl= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3814= yyvs; struct _xenum_struct** _temp3816= _temp3814.curr
+ yyvsp_offset; if( _temp3816 < _temp3814.base? 1: _temp3816 >= _temp3814.last_plus_one){
_throw( Null_Exception);}* _temp3816;})); _temp3813;})};( struct _xenum_struct*)
_temp3812;}); break; case 153: _LL3811: if( Cyc_yyget_Kind_tok(({ struct
_tagged_ptr2 _temp3822= yyvs; struct _xenum_struct** _temp3824= _temp3822.curr +
yyvsp_offset; if( _temp3824 < _temp3822.base? 1: _temp3824 >= _temp3822.last_plus_one){
_throw( Null_Exception);}* _temp3824;})) != Cyc_Absyn_RgnKind){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp3825=( char*)"expecting region kind (R)";
struct _tagged_string _temp3826; _temp3826.curr= _temp3825; _temp3826.base=
_temp3825; _temp3826.last_plus_one= _temp3825 + 26; _temp3826;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3827= yyls; struct Cyc_Yyltype* _temp3829= _temp3827.curr
+ yylsp_offset; if( _temp3829 < _temp3827.base? 1: _temp3829 >= _temp3827.last_plus_one){
_throw( Null_Exception);}* _temp3829;}).first_line,({ struct _tagged_ptr3
_temp3830= yyls; struct Cyc_Yyltype* _temp3832= _temp3830.curr + yylsp_offset;
if( _temp3832 < _temp3830.base? 1: _temp3832 >= _temp3830.last_plus_one){ _throw(
Null_Exception);}* _temp3832;}).last_line));} yyval=({ struct Cyc_TypeList_tok_struct*
_temp3833=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));*
_temp3833=( struct Cyc_TypeList_tok_struct){.tag= Cyc_TypeList_tok_tag,.f1=({
struct Cyc_List_List* _temp3834=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3834->hd=( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp3835=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3835->tag= Cyc_Absyn_AccessEff_tag; _temp3835->f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp3836= yyvs; struct
_xenum_struct** _temp3838= _temp3836.curr +( yyvsp_offset - 2); if( _temp3838 <
_temp3836.base? 1: _temp3838 >= _temp3836.last_plus_one){ _throw( Null_Exception);}*
_temp3838;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
Cyc_Absyn_RgnKind));( void*) _temp3835;}); _temp3834->tl= 0; _temp3834;})};(
struct _xenum_struct*) _temp3833;}); break; case 154: _LL3821: if( Cyc_yyget_Kind_tok(({
struct _tagged_ptr2 _temp3840= yyvs; struct _xenum_struct** _temp3842= _temp3840.curr
+( yyvsp_offset - 2); if( _temp3842 < _temp3840.base? 1: _temp3842 >= _temp3840.last_plus_one){
_throw( Null_Exception);}* _temp3842;})) != Cyc_Absyn_RgnKind){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp3843=( char*)"expecting region kind (R)";
struct _tagged_string _temp3844; _temp3844.curr= _temp3843; _temp3844.base=
_temp3843; _temp3844.last_plus_one= _temp3843 + 26; _temp3844;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3845= yyls; struct Cyc_Yyltype* _temp3847= _temp3845.curr
+( yylsp_offset - 2); if( _temp3847 < _temp3845.base? 1: _temp3847 >= _temp3845.last_plus_one){
_throw( Null_Exception);}* _temp3847;}).first_line,({ struct _tagged_ptr3
_temp3848= yyls; struct Cyc_Yyltype* _temp3850= _temp3848.curr +( yylsp_offset -
2); if( _temp3850 < _temp3848.base? 1: _temp3850 >= _temp3848.last_plus_one){
_throw( Null_Exception);}* _temp3850;}).last_line));} yyval=({ struct Cyc_TypeList_tok_struct*
_temp3851=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));*
_temp3851=( struct Cyc_TypeList_tok_struct){.tag= Cyc_TypeList_tok_tag,.f1=({
struct Cyc_List_List* _temp3852=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3852->hd=( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp3856=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3856->tag= Cyc_Absyn_AccessEff_tag; _temp3856->f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp3857= yyvs; struct
_xenum_struct** _temp3859= _temp3857.curr +( yyvsp_offset - 4); if( _temp3859 <
_temp3857.base? 1: _temp3859 >= _temp3857.last_plus_one){ _throw( Null_Exception);}*
_temp3859;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
Cyc_Absyn_RgnKind));( void*) _temp3856;}); _temp3852->tl= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3853= yyvs; struct _xenum_struct** _temp3855= _temp3853.curr
+ yyvsp_offset; if( _temp3855 < _temp3853.base? 1: _temp3855 >= _temp3853.last_plus_one){
_throw( Null_Exception);}* _temp3855;})); _temp3852;})};( struct _xenum_struct*)
_temp3851;}); break; case 155: _LL3839: yyval=({ struct Cyc_ParamDeclList_tok_struct*
_temp3861=( struct Cyc_ParamDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct));*
_temp3861=( struct Cyc_ParamDeclList_tok_struct){.tag= Cyc_ParamDeclList_tok_tag,.f1=({
struct Cyc_List_List* _temp3862=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3862->hd=( void*) Cyc_yyget_ParamDecl_tok(({ struct
_tagged_ptr2 _temp3863= yyvs; struct _xenum_struct** _temp3865= _temp3863.curr +
yyvsp_offset; if( _temp3865 < _temp3863.base? 1: _temp3865 >= _temp3863.last_plus_one){
_throw( Null_Exception);}* _temp3865;})); _temp3862->tl= 0; _temp3862;})};(
struct _xenum_struct*) _temp3861;}); break; case 156: _LL3860: yyval=({ struct
Cyc_ParamDeclList_tok_struct* _temp3867=( struct Cyc_ParamDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct));* _temp3867=( struct
Cyc_ParamDeclList_tok_struct){.tag= Cyc_ParamDeclList_tok_tag,.f1=({ struct Cyc_List_List*
_temp3868=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3868->hd=( void*) Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp3872=
yyvs; struct _xenum_struct** _temp3874= _temp3872.curr + yyvsp_offset; if(
_temp3874 < _temp3872.base? 1: _temp3874 >= _temp3872.last_plus_one){ _throw(
Null_Exception);}* _temp3874;})); _temp3868->tl= Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp3869= yyvs; struct _xenum_struct** _temp3871= _temp3869.curr
+( yyvsp_offset - 2); if( _temp3871 < _temp3869.base? 1: _temp3871 >= _temp3869.last_plus_one){
_throw( Null_Exception);}* _temp3871;})); _temp3868;})};( struct _xenum_struct*)
_temp3867;}); break; case 157: _LL3866: { void* t= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3876= yyvs; struct _xenum_struct** _temp3878= _temp3876.curr
+( yyvsp_offset - 1); if( _temp3878 < _temp3876.base? 1: _temp3878 >= _temp3876.last_plus_one){
_throw( Null_Exception);}* _temp3878;}))).f2, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3879= yyls; struct Cyc_Yyltype* _temp3881= _temp3879.curr
+( yylsp_offset - 1); if( _temp3881 < _temp3879.base? 1: _temp3881 >= _temp3879.last_plus_one){
_throw( Null_Exception);}* _temp3881;}).first_line,({ struct _tagged_ptr3
_temp3882= yyls; struct Cyc_Yyltype* _temp3884= _temp3882.curr +( yylsp_offset -
1); if( _temp3884 < _temp3882.base? 1: _temp3884 >= _temp3882.last_plus_one){
_throw( Null_Exception);}* _temp3884;}).last_line)); struct Cyc_Absyn_Tqual* tq=(*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp3885= yyvs; struct
_xenum_struct** _temp3887= _temp3885.curr +( yyvsp_offset - 1); if( _temp3887 <
_temp3885.base? 1: _temp3887 >= _temp3885.last_plus_one){ _throw( Null_Exception);}*
_temp3887;}))).f1; struct Cyc_List_List* tms=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3888= yyvs; struct _xenum_struct** _temp3890= _temp3888.curr
+ yyvsp_offset; if( _temp3890 < _temp3888.base? 1: _temp3890 >= _temp3888.last_plus_one){
_throw( Null_Exception);}* _temp3890;})))->tms; struct _tuple6 t_info= Cyc_Parse_apply_tms(
tq, t, 0, tms); if( t_info.f3 != 0){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp3891=( char*)"parameter with bad type params"; struct _tagged_string
_temp3892; _temp3892.curr= _temp3891; _temp3892.base= _temp3891; _temp3892.last_plus_one=
_temp3891 + 31; _temp3892;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp3893= yyls; struct Cyc_Yyltype* _temp3895= _temp3893.curr + yylsp_offset;
if( _temp3895 < _temp3893.base? 1: _temp3895 >= _temp3893.last_plus_one){ _throw(
Null_Exception);}* _temp3895;}).first_line,({ struct _tagged_ptr3 _temp3896=
yyls; struct Cyc_Yyltype* _temp3898= _temp3896.curr + yylsp_offset; if(
_temp3898 < _temp3896.base? 1: _temp3898 >= _temp3896.last_plus_one){ _throw(
Null_Exception);}* _temp3898;}).last_line));}{ struct _tuple1* q=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3899= yyvs; struct _xenum_struct** _temp3901= _temp3899.curr
+ yyvsp_offset; if( _temp3901 < _temp3899.base? 1: _temp3901 >= _temp3899.last_plus_one){
_throw( Null_Exception);}* _temp3901;})))->id;{ void* _temp3902=(* q).f1; struct
Cyc_List_List* _temp3912; struct Cyc_List_List* _temp3914; _LL3904: if(
_temp3902 == Cyc_Absyn_Loc_n){ goto _LL3905;} else{ goto _LL3906;} _LL3906: if((
unsigned int) _temp3902 > 1u?(( struct _enum_struct*) _temp3902)->tag == Cyc_Absyn_Rel_n_tag:
0){ _LL3913: _temp3912=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp3902)->f1; if( _temp3912 == 0){ goto _LL3907;} else{ goto _LL3908;}} else{
goto _LL3908;} _LL3908: if(( unsigned int) _temp3902 > 1u?(( struct _enum_struct*)
_temp3902)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL3915: _temp3914=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp3902)->f1; if( _temp3914 == 0){ goto
_LL3909;} else{ goto _LL3910;}} else{ goto _LL3910;} _LL3910: goto _LL3911;
_LL3905: goto _LL3903; _LL3907: goto _LL3903; _LL3909: goto _LL3903; _LL3911:
Cyc_Parse_err(( struct _tagged_string)({ char* _temp3916=( char*)"parameter cannot be qualified with a module name";
struct _tagged_string _temp3917; _temp3917.curr= _temp3916; _temp3917.base=
_temp3916; _temp3917.last_plus_one= _temp3916 + 49; _temp3917;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3918= yyls; struct Cyc_Yyltype* _temp3920= _temp3918.curr
+( yylsp_offset - 1); if( _temp3920 < _temp3918.base? 1: _temp3920 >= _temp3918.last_plus_one){
_throw( Null_Exception);}* _temp3920;}).first_line,({ struct _tagged_ptr3
_temp3921= yyls; struct Cyc_Yyltype* _temp3923= _temp3921.curr +( yylsp_offset -
1); if( _temp3923 < _temp3921.base? 1: _temp3923 >= _temp3921.last_plus_one){
_throw( Null_Exception);}* _temp3923;}).last_line)); goto _LL3903; _LL3903:;}{
struct Cyc_Core_Opt* idopt=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt*
_temp3924=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3924->v=( void*)(* q).f2; _temp3924;}); yyval=({ struct Cyc_ParamDecl_tok_struct*
_temp3925=( struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));*
_temp3925=( struct Cyc_ParamDecl_tok_struct){.tag= Cyc_ParamDecl_tok_tag,.f1=({
struct _tuple2* _temp3926=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp3926->f1= idopt; _temp3926->f2= t_info.f1; _temp3926->f3= t_info.f2;
_temp3926;})};( struct _xenum_struct*) _temp3925;}); break;}}} case 158: _LL3875: {
void* t= Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp3928= yyvs; struct _xenum_struct** _temp3930= _temp3928.curr +
yyvsp_offset; if( _temp3930 < _temp3928.base? 1: _temp3930 >= _temp3928.last_plus_one){
_throw( Null_Exception);}* _temp3930;}))).f2, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3931= yyls; struct Cyc_Yyltype* _temp3933= _temp3931.curr
+ yylsp_offset; if( _temp3933 < _temp3931.base? 1: _temp3933 >= _temp3931.last_plus_one){
_throw( Null_Exception);}* _temp3933;}).first_line,({ struct _tagged_ptr3
_temp3934= yyls; struct Cyc_Yyltype* _temp3936= _temp3934.curr + yylsp_offset;
if( _temp3936 < _temp3934.base? 1: _temp3936 >= _temp3934.last_plus_one){ _throw(
Null_Exception);}* _temp3936;}).last_line)); struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3937= yyvs; struct _xenum_struct** _temp3939= _temp3937.curr
+ yyvsp_offset; if( _temp3939 < _temp3937.base? 1: _temp3939 >= _temp3937.last_plus_one){
_throw( Null_Exception);}* _temp3939;}))).f1; yyval=({ struct Cyc_ParamDecl_tok_struct*
_temp3940=( struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));*
_temp3940=( struct Cyc_ParamDecl_tok_struct){.tag= Cyc_ParamDecl_tok_tag,.f1=({
struct _tuple2* _temp3941=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp3941->f1= 0; _temp3941->f2= tq; _temp3941->f3= t; _temp3941;})};( struct
_xenum_struct*) _temp3940;}); break;} case 159: _LL3927: { void* t= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp3943= yyvs; struct
_xenum_struct** _temp3945= _temp3943.curr +( yyvsp_offset - 1); if( _temp3945 <
_temp3943.base? 1: _temp3945 >= _temp3943.last_plus_one){ _throw( Null_Exception);}*
_temp3945;}))).f2, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3946=
yyls; struct Cyc_Yyltype* _temp3948= _temp3946.curr +( yylsp_offset - 1); if(
_temp3948 < _temp3946.base? 1: _temp3948 >= _temp3946.last_plus_one){ _throw(
Null_Exception);}* _temp3948;}).first_line,({ struct _tagged_ptr3 _temp3949=
yyls; struct Cyc_Yyltype* _temp3951= _temp3949.curr +( yylsp_offset - 1); if(
_temp3951 < _temp3949.base? 1: _temp3951 >= _temp3949.last_plus_one){ _throw(
Null_Exception);}* _temp3951;}).last_line)); struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3952= yyvs; struct _xenum_struct** _temp3954= _temp3952.curr
+( yyvsp_offset - 1); if( _temp3954 < _temp3952.base? 1: _temp3954 >= _temp3952.last_plus_one){
_throw( Null_Exception);}* _temp3954;}))).f1; struct Cyc_List_List* tms=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp3955= yyvs; struct _xenum_struct** _temp3957= _temp3955.curr
+ yyvsp_offset; if( _temp3957 < _temp3955.base? 1: _temp3957 >= _temp3955.last_plus_one){
_throw( Null_Exception);}* _temp3957;})))->tms; struct _tuple6 t_info= Cyc_Parse_apply_tms(
tq, t, 0, tms); if( t_info.f3 != 0){ Cyc_Parse_warn(( struct _tagged_string)({
char* _temp3958=( char*)"bad type params, ignoring"; struct _tagged_string
_temp3959; _temp3959.curr= _temp3958; _temp3959.base= _temp3958; _temp3959.last_plus_one=
_temp3958 + 26; _temp3959;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp3960= yyls; struct Cyc_Yyltype* _temp3962= _temp3960.curr + yylsp_offset;
if( _temp3962 < _temp3960.base? 1: _temp3962 >= _temp3960.last_plus_one){ _throw(
Null_Exception);}* _temp3962;}).first_line,({ struct _tagged_ptr3 _temp3963=
yyls; struct Cyc_Yyltype* _temp3965= _temp3963.curr + yylsp_offset; if(
_temp3965 < _temp3963.base? 1: _temp3965 >= _temp3963.last_plus_one){ _throw(
Null_Exception);}* _temp3965;}).last_line));} yyval=({ struct Cyc_ParamDecl_tok_struct*
_temp3966=( struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));*
_temp3966=( struct Cyc_ParamDecl_tok_struct){.tag= Cyc_ParamDecl_tok_tag,.f1=({
struct _tuple2* _temp3967=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp3967->f1= 0; _temp3967->f2= t_info.f1; _temp3967->f3= t_info.f2; _temp3967;})};(
struct _xenum_struct*) _temp3966;}); break;} case 160: _LL3942: yyval=({ struct
Cyc_IdList_tok_struct* _temp3969=( struct Cyc_IdList_tok_struct*) GC_malloc(
sizeof( struct Cyc_IdList_tok_struct));* _temp3969=( struct Cyc_IdList_tok_struct){.tag=
Cyc_IdList_tok_tag,.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( Cyc_yyget_IdList_tok(({ struct _tagged_ptr2 _temp3970= yyvs;
struct _xenum_struct** _temp3972= _temp3970.curr + yyvsp_offset; if( _temp3972 <
_temp3970.base? 1: _temp3972 >= _temp3970.last_plus_one){ _throw( Null_Exception);}*
_temp3972;})))};( struct _xenum_struct*) _temp3969;}); break; case 161: _LL3968:
yyval=({ struct Cyc_IdList_tok_struct* _temp3974=( struct Cyc_IdList_tok_struct*)
GC_malloc( sizeof( struct Cyc_IdList_tok_struct));* _temp3974=( struct Cyc_IdList_tok_struct){.tag=
Cyc_IdList_tok_tag,.f1=({ struct Cyc_List_List* _temp3975=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3975->hd=( void*)({ struct
_tagged_string* _temp3976=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp3976[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp3977= yyvs; struct _xenum_struct** _temp3979= _temp3977.curr + yyvsp_offset;
if( _temp3979 < _temp3977.base? 1: _temp3979 >= _temp3977.last_plus_one){ _throw(
Null_Exception);}* _temp3979;})); _temp3976;}); _temp3975->tl= 0; _temp3975;})};(
struct _xenum_struct*) _temp3974;}); break; case 162: _LL3973: yyval=({ struct
Cyc_IdList_tok_struct* _temp3981=( struct Cyc_IdList_tok_struct*) GC_malloc(
sizeof( struct Cyc_IdList_tok_struct));* _temp3981=( struct Cyc_IdList_tok_struct){.tag=
Cyc_IdList_tok_tag,.f1=({ struct Cyc_List_List* _temp3982=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3982->hd=( void*)({ struct
_tagged_string* _temp3986=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp3986[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp3987= yyvs; struct _xenum_struct** _temp3989= _temp3987.curr + yyvsp_offset;
if( _temp3989 < _temp3987.base? 1: _temp3989 >= _temp3987.last_plus_one){ _throw(
Null_Exception);}* _temp3989;})); _temp3986;}); _temp3982->tl= Cyc_yyget_IdList_tok(({
struct _tagged_ptr2 _temp3983= yyvs; struct _xenum_struct** _temp3985= _temp3983.curr
+( yyvsp_offset - 2); if( _temp3985 < _temp3983.base? 1: _temp3985 >= _temp3983.last_plus_one){
_throw( Null_Exception);}* _temp3985;})); _temp3982;})};( struct _xenum_struct*)
_temp3981;}); break; case 163: _LL3980: yyval=({ struct _tagged_ptr2 _temp3991=
yyvs; struct _xenum_struct** _temp3993= _temp3991.curr + yyvsp_offset; if(
_temp3993 < _temp3991.base? 1: _temp3993 >= _temp3991.last_plus_one){ _throw(
Null_Exception);}* _temp3993;}); break; case 164: _LL3990: yyval=({ struct Cyc_Exp_tok_struct*
_temp3995=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp3995=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp3996=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp3996->tag=
Cyc_Absyn_UnresolvedMem_e_tag; _temp3996->f1= 0; _temp3996->f2= 0;( void*)
_temp3996;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3997= yyls;
struct Cyc_Yyltype* _temp3999= _temp3997.curr +( yylsp_offset - 1); if(
_temp3999 < _temp3997.base? 1: _temp3999 >= _temp3997.last_plus_one){ _throw(
Null_Exception);}* _temp3999;}).first_line,({ struct _tagged_ptr3 _temp4000=
yyls; struct Cyc_Yyltype* _temp4002= _temp4000.curr + yylsp_offset; if(
_temp4002 < _temp4000.base? 1: _temp4002 >= _temp4000.last_plus_one){ _throw(
Null_Exception);}* _temp4002;}).last_line))};( struct _xenum_struct*) _temp3995;});
break; case 165: _LL3994: yyval=({ struct Cyc_Exp_tok_struct* _temp4004=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp4004=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp4005=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp4005->tag=
Cyc_Absyn_UnresolvedMem_e_tag; _temp4005->f1= 0; _temp4005->f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp4006= yyvs; struct _xenum_struct** _temp4008= _temp4006.curr
+( yyvsp_offset - 1); if( _temp4008 < _temp4006.base? 1: _temp4008 >= _temp4006.last_plus_one){
_throw( Null_Exception);}* _temp4008;})));( void*) _temp4005;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4009= yyls; struct Cyc_Yyltype* _temp4011= _temp4009.curr
+( yylsp_offset - 2); if( _temp4011 < _temp4009.base? 1: _temp4011 >= _temp4009.last_plus_one){
_throw( Null_Exception);}* _temp4011;}).first_line,({ struct _tagged_ptr3
_temp4012= yyls; struct Cyc_Yyltype* _temp4014= _temp4012.curr + yylsp_offset;
if( _temp4014 < _temp4012.base? 1: _temp4014 >= _temp4012.last_plus_one){ _throw(
Null_Exception);}* _temp4014;}).last_line))};( struct _xenum_struct*) _temp4004;});
break; case 166: _LL4003: yyval=({ struct Cyc_Exp_tok_struct* _temp4016=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp4016=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp4017=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp4017->tag=
Cyc_Absyn_UnresolvedMem_e_tag; _temp4017->f1= 0; _temp4017->f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp4018= yyvs; struct _xenum_struct** _temp4020= _temp4018.curr
+( yyvsp_offset - 2); if( _temp4020 < _temp4018.base? 1: _temp4020 >= _temp4018.last_plus_one){
_throw( Null_Exception);}* _temp4020;})));( void*) _temp4017;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4021= yyls; struct Cyc_Yyltype* _temp4023= _temp4021.curr
+( yylsp_offset - 3); if( _temp4023 < _temp4021.base? 1: _temp4023 >= _temp4021.last_plus_one){
_throw( Null_Exception);}* _temp4023;}).first_line,({ struct _tagged_ptr3
_temp4024= yyls; struct Cyc_Yyltype* _temp4026= _temp4024.curr + yylsp_offset;
if( _temp4026 < _temp4024.base? 1: _temp4026 >= _temp4024.last_plus_one){ _throw(
Null_Exception);}* _temp4026;}).last_line))};( struct _xenum_struct*) _temp4016;});
break; case 167: _LL4015: yyval=({ struct Cyc_InitializerList_tok_struct*
_temp4028=( struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct
Cyc_InitializerList_tok_struct));* _temp4028=( struct Cyc_InitializerList_tok_struct){.tag=
Cyc_InitializerList_tok_tag,.f1=({ struct Cyc_List_List* _temp4029=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4029->hd=( void*)({ struct
_tuple16* _temp4030=( struct _tuple16*) GC_malloc( sizeof( struct _tuple16));
_temp4030->f1= 0; _temp4030->f2= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp4031= yyvs; struct _xenum_struct** _temp4033= _temp4031.curr + yyvsp_offset;
if( _temp4033 < _temp4031.base? 1: _temp4033 >= _temp4031.last_plus_one){ _throw(
Null_Exception);}* _temp4033;})); _temp4030;}); _temp4029->tl= 0; _temp4029;})};(
struct _xenum_struct*) _temp4028;}); break; case 168: _LL4027: yyval=({ struct
Cyc_InitializerList_tok_struct* _temp4035=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct));* _temp4035=( struct
Cyc_InitializerList_tok_struct){.tag= Cyc_InitializerList_tok_tag,.f1=({ struct
Cyc_List_List* _temp4036=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4036->hd=( void*)({ struct _tuple16* _temp4037=( struct _tuple16*)
GC_malloc( sizeof( struct _tuple16)); _temp4037->f1= Cyc_yyget_DesignatorList_tok(({
struct _tagged_ptr2 _temp4041= yyvs; struct _xenum_struct** _temp4043= _temp4041.curr
+( yyvsp_offset - 1); if( _temp4043 < _temp4041.base? 1: _temp4043 >= _temp4041.last_plus_one){
_throw( Null_Exception);}* _temp4043;})); _temp4037->f2= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4038= yyvs; struct _xenum_struct** _temp4040= _temp4038.curr
+ yyvsp_offset; if( _temp4040 < _temp4038.base? 1: _temp4040 >= _temp4038.last_plus_one){
_throw( Null_Exception);}* _temp4040;})); _temp4037;}); _temp4036->tl= 0;
_temp4036;})};( struct _xenum_struct*) _temp4035;}); break; case 169: _LL4034:
yyval=({ struct Cyc_InitializerList_tok_struct* _temp4045=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct));* _temp4045=( struct
Cyc_InitializerList_tok_struct){.tag= Cyc_InitializerList_tok_tag,.f1=({ struct
Cyc_List_List* _temp4046=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4046->hd=( void*)({ struct _tuple16* _temp4050=( struct _tuple16*)
GC_malloc( sizeof( struct _tuple16)); _temp4050->f1= 0; _temp4050->f2= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4051= yyvs; struct _xenum_struct** _temp4053= _temp4051.curr
+ yyvsp_offset; if( _temp4053 < _temp4051.base? 1: _temp4053 >= _temp4051.last_plus_one){
_throw( Null_Exception);}* _temp4053;})); _temp4050;}); _temp4046->tl= Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp4047= yyvs; struct _xenum_struct** _temp4049= _temp4047.curr
+( yyvsp_offset - 2); if( _temp4049 < _temp4047.base? 1: _temp4049 >= _temp4047.last_plus_one){
_throw( Null_Exception);}* _temp4049;})); _temp4046;})};( struct _xenum_struct*)
_temp4045;}); break; case 170: _LL4044: yyval=({ struct Cyc_InitializerList_tok_struct*
_temp4055=( struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct
Cyc_InitializerList_tok_struct));* _temp4055=( struct Cyc_InitializerList_tok_struct){.tag=
Cyc_InitializerList_tok_tag,.f1=({ struct Cyc_List_List* _temp4056=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4056->hd=( void*)({ struct
_tuple16* _temp4060=( struct _tuple16*) GC_malloc( sizeof( struct _tuple16));
_temp4060->f1= Cyc_yyget_DesignatorList_tok(({ struct _tagged_ptr2 _temp4064=
yyvs; struct _xenum_struct** _temp4066= _temp4064.curr +( yyvsp_offset - 1); if(
_temp4066 < _temp4064.base? 1: _temp4066 >= _temp4064.last_plus_one){ _throw(
Null_Exception);}* _temp4066;})); _temp4060->f2= Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4061= yyvs; struct _xenum_struct** _temp4063= _temp4061.curr +
yyvsp_offset; if( _temp4063 < _temp4061.base? 1: _temp4063 >= _temp4061.last_plus_one){
_throw( Null_Exception);}* _temp4063;})); _temp4060;}); _temp4056->tl= Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp4057= yyvs; struct _xenum_struct** _temp4059= _temp4057.curr
+( yyvsp_offset - 3); if( _temp4059 < _temp4057.base? 1: _temp4059 >= _temp4057.last_plus_one){
_throw( Null_Exception);}* _temp4059;})); _temp4056;})};( struct _xenum_struct*)
_temp4055;}); break; case 171: _LL4054: yyval=({ struct Cyc_DesignatorList_tok_struct*
_temp4068=( struct Cyc_DesignatorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));*
_temp4068=( struct Cyc_DesignatorList_tok_struct){.tag= Cyc_DesignatorList_tok_tag,.f1=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_DesignatorList_tok(({
struct _tagged_ptr2 _temp4069= yyvs; struct _xenum_struct** _temp4071= _temp4069.curr
+( yyvsp_offset - 1); if( _temp4071 < _temp4069.base? 1: _temp4071 >= _temp4069.last_plus_one){
_throw( Null_Exception);}* _temp4071;})))};( struct _xenum_struct*) _temp4068;});
break; case 172: _LL4067: yyval=({ struct Cyc_DesignatorList_tok_struct*
_temp4073=( struct Cyc_DesignatorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));*
_temp4073=( struct Cyc_DesignatorList_tok_struct){.tag= Cyc_DesignatorList_tok_tag,.f1=({
struct Cyc_List_List* _temp4074=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4074->hd=( void*) Cyc_yyget_Designator_tok(({
struct _tagged_ptr2 _temp4075= yyvs; struct _xenum_struct** _temp4077= _temp4075.curr
+ yyvsp_offset; if( _temp4077 < _temp4075.base? 1: _temp4077 >= _temp4075.last_plus_one){
_throw( Null_Exception);}* _temp4077;})); _temp4074->tl= 0; _temp4074;})};(
struct _xenum_struct*) _temp4073;}); break; case 173: _LL4072: yyval=({ struct
Cyc_DesignatorList_tok_struct* _temp4079=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));* _temp4079=( struct
Cyc_DesignatorList_tok_struct){.tag= Cyc_DesignatorList_tok_tag,.f1=({ struct
Cyc_List_List* _temp4080=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4080->hd=( void*) Cyc_yyget_Designator_tok(({ struct _tagged_ptr2 _temp4084=
yyvs; struct _xenum_struct** _temp4086= _temp4084.curr + yyvsp_offset; if(
_temp4086 < _temp4084.base? 1: _temp4086 >= _temp4084.last_plus_one){ _throw(
Null_Exception);}* _temp4086;})); _temp4080->tl= Cyc_yyget_DesignatorList_tok(({
struct _tagged_ptr2 _temp4081= yyvs; struct _xenum_struct** _temp4083= _temp4081.curr
+( yyvsp_offset - 1); if( _temp4083 < _temp4081.base? 1: _temp4083 >= _temp4081.last_plus_one){
_throw( Null_Exception);}* _temp4083;})); _temp4080;})};( struct _xenum_struct*)
_temp4079;}); break; case 174: _LL4078: yyval=({ struct Cyc_Designator_tok_struct*
_temp4088=( struct Cyc_Designator_tok_struct*) GC_malloc( sizeof( struct Cyc_Designator_tok_struct));*
_temp4088=( struct Cyc_Designator_tok_struct){.tag= Cyc_Designator_tok_tag,.f1=({
struct Cyc_Absyn_ArrayElement_struct* _temp4089=( struct Cyc_Absyn_ArrayElement_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayElement_struct)); _temp4089->tag= Cyc_Absyn_ArrayElement_tag;
_temp4089->f1= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4090= yyvs; struct
_xenum_struct** _temp4092= _temp4090.curr +( yyvsp_offset - 1); if( _temp4092 <
_temp4090.base? 1: _temp4092 >= _temp4090.last_plus_one){ _throw( Null_Exception);}*
_temp4092;}));( void*) _temp4089;})};( struct _xenum_struct*) _temp4088;});
break; case 175: _LL4087: yyval=({ struct Cyc_Designator_tok_struct* _temp4094=(
struct Cyc_Designator_tok_struct*) GC_malloc( sizeof( struct Cyc_Designator_tok_struct));*
_temp4094=( struct Cyc_Designator_tok_struct){.tag= Cyc_Designator_tok_tag,.f1=({
struct Cyc_Absyn_FieldName_struct* _temp4095=( struct Cyc_Absyn_FieldName_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct)); _temp4095->tag= Cyc_Absyn_FieldName_tag;
_temp4095->f1=({ struct _tagged_string* _temp4096=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp4096[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp4097= yyvs; struct _xenum_struct** _temp4099= _temp4097.curr
+ yyvsp_offset; if( _temp4099 < _temp4097.base? 1: _temp4099 >= _temp4097.last_plus_one){
_throw( Null_Exception);}* _temp4099;})); _temp4096;});( void*) _temp4095;})};(
struct _xenum_struct*) _temp4094;}); break; case 176: _LL4093: { void* t= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp4101= yyvs; struct
_xenum_struct** _temp4103= _temp4101.curr + yyvsp_offset; if( _temp4103 <
_temp4101.base? 1: _temp4103 >= _temp4101.last_plus_one){ _throw( Null_Exception);}*
_temp4103;}))).f2, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4104=
yyls; struct Cyc_Yyltype* _temp4106= _temp4104.curr + yylsp_offset; if(
_temp4106 < _temp4104.base? 1: _temp4106 >= _temp4104.last_plus_one){ _throw(
Null_Exception);}* _temp4106;}).first_line,({ struct _tagged_ptr3 _temp4107=
yyls; struct Cyc_Yyltype* _temp4109= _temp4107.curr + yylsp_offset; if(
_temp4109 < _temp4107.base? 1: _temp4109 >= _temp4107.last_plus_one){ _throw(
Null_Exception);}* _temp4109;}).last_line)); struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4110= yyvs; struct _xenum_struct** _temp4112= _temp4110.curr
+ yyvsp_offset; if( _temp4112 < _temp4110.base? 1: _temp4112 >= _temp4110.last_plus_one){
_throw( Null_Exception);}* _temp4112;}))).f1; yyval=({ struct Cyc_ParamDecl_tok_struct*
_temp4113=( struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));*
_temp4113=( struct Cyc_ParamDecl_tok_struct){.tag= Cyc_ParamDecl_tok_tag,.f1=({
struct _tuple2* _temp4114=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2)
* 1); _temp4114[ 0]=({ struct _tuple2 _temp4115; _temp4115.f1= 0; _temp4115.f2=
tq; _temp4115.f3= t; _temp4115;}); _temp4114;})};( struct _xenum_struct*)
_temp4113;}); break;} case 177: _LL4100: { void* t= Cyc_Parse_speclist2typ((*
Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp4117= yyvs; struct
_xenum_struct** _temp4119= _temp4117.curr +( yyvsp_offset - 1); if( _temp4119 <
_temp4117.base? 1: _temp4119 >= _temp4117.last_plus_one){ _throw( Null_Exception);}*
_temp4119;}))).f2, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4120=
yyls; struct Cyc_Yyltype* _temp4122= _temp4120.curr +( yylsp_offset - 1); if(
_temp4122 < _temp4120.base? 1: _temp4122 >= _temp4120.last_plus_one){ _throw(
Null_Exception);}* _temp4122;}).first_line,({ struct _tagged_ptr3 _temp4123=
yyls; struct Cyc_Yyltype* _temp4125= _temp4123.curr +( yylsp_offset - 1); if(
_temp4125 < _temp4123.base? 1: _temp4125 >= _temp4123.last_plus_one){ _throw(
Null_Exception);}* _temp4125;}).last_line)); struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp4126= yyvs; struct _xenum_struct** _temp4128= _temp4126.curr
+( yyvsp_offset - 1); if( _temp4128 < _temp4126.base? 1: _temp4128 >= _temp4126.last_plus_one){
_throw( Null_Exception);}* _temp4128;}))).f1; struct Cyc_List_List* tms=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp4129= yyvs; struct _xenum_struct** _temp4131= _temp4129.curr
+ yyvsp_offset; if( _temp4131 < _temp4129.base? 1: _temp4131 >= _temp4129.last_plus_one){
_throw( Null_Exception);}* _temp4131;})))->tms; struct _tuple6 t_info= Cyc_Parse_apply_tms(
tq, t, 0, tms); if( t_info.f3 != 0){ Cyc_Parse_warn(( struct _tagged_string)({
char* _temp4132=( char*)"bad type params, ignoring"; struct _tagged_string
_temp4133; _temp4133.curr= _temp4132; _temp4133.base= _temp4132; _temp4133.last_plus_one=
_temp4132 + 26; _temp4133;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4134= yyls; struct Cyc_Yyltype* _temp4136= _temp4134.curr + yylsp_offset;
if( _temp4136 < _temp4134.base? 1: _temp4136 >= _temp4134.last_plus_one){ _throw(
Null_Exception);}* _temp4136;}).first_line,({ struct _tagged_ptr3 _temp4137=
yyls; struct Cyc_Yyltype* _temp4139= _temp4137.curr + yylsp_offset; if(
_temp4139 < _temp4137.base? 1: _temp4139 >= _temp4137.last_plus_one){ _throw(
Null_Exception);}* _temp4139;}).last_line));} yyval=({ struct Cyc_ParamDecl_tok_struct*
_temp4140=( struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));*
_temp4140=( struct Cyc_ParamDecl_tok_struct){.tag= Cyc_ParamDecl_tok_tag,.f1=({
struct _tuple2* _temp4141=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp4141->f1= 0; _temp4141->f2= t_info.f1; _temp4141->f3= t_info.f2; _temp4141;})};(
struct _xenum_struct*) _temp4140;}); break;} case 178: _LL4116: yyval=({ struct
Cyc_Type_tok_struct* _temp4143=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof(
struct Cyc_Type_tok_struct));* _temp4143=( struct Cyc_Type_tok_struct){.tag= Cyc_Type_tok_tag,.f1=(*
Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp4144= yyvs; struct
_xenum_struct** _temp4146= _temp4144.curr + yyvsp_offset; if( _temp4146 <
_temp4144.base? 1: _temp4146 >= _temp4144.last_plus_one){ _throw( Null_Exception);}*
_temp4146;}))).f3};( struct _xenum_struct*) _temp4143;}); break; case 179:
_LL4142: yyval=({ struct Cyc_Type_tok_struct* _temp4148=( struct Cyc_Type_tok_struct*)
GC_malloc( sizeof( struct Cyc_Type_tok_struct));* _temp4148=( struct Cyc_Type_tok_struct){.tag=
Cyc_Type_tok_tag,.f1=({ struct Cyc_Absyn_JoinEff_struct* _temp4149=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp4149->tag= Cyc_Absyn_JoinEff_tag;
_temp4149->f1= 0;( void*) _temp4149;})};( struct _xenum_struct*) _temp4148;});
break; case 180: _LL4147: yyval=({ struct Cyc_Type_tok_struct* _temp4151=(
struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));*
_temp4151=( struct Cyc_Type_tok_struct){.tag= Cyc_Type_tok_tag,.f1=({ struct Cyc_Absyn_JoinEff_struct*
_temp4152=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp4152->tag= Cyc_Absyn_JoinEff_tag; _temp4152->f1= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4153= yyvs; struct _xenum_struct** _temp4155= _temp4153.curr
+( yyvsp_offset - 1); if( _temp4155 < _temp4153.base? 1: _temp4155 >= _temp4153.last_plus_one){
_throw( Null_Exception);}* _temp4155;}));( void*) _temp4152;})};( struct
_xenum_struct*) _temp4151;}); break; case 181: _LL4150: yyval=({ struct Cyc_Type_tok_struct*
_temp4157=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));*
_temp4157=( struct Cyc_Type_tok_struct){.tag= Cyc_Type_tok_tag,.f1=({ struct Cyc_Absyn_JoinEff_struct*
_temp4158=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp4158->tag= Cyc_Absyn_JoinEff_tag; _temp4158->f1=({ struct Cyc_List_List*
_temp4159=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4159->hd=( void*) Cyc_yyget_Type_tok(({ struct _tagged_ptr2 _temp4163= yyvs;
struct _xenum_struct** _temp4165= _temp4163.curr +( yyvsp_offset - 2); if(
_temp4165 < _temp4163.base? 1: _temp4165 >= _temp4163.last_plus_one){ _throw(
Null_Exception);}* _temp4165;})); _temp4159->tl= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4160= yyvs; struct _xenum_struct** _temp4162= _temp4160.curr
+ yyvsp_offset; if( _temp4162 < _temp4160.base? 1: _temp4162 >= _temp4160.last_plus_one){
_throw( Null_Exception);}* _temp4162;})); _temp4159;});( void*) _temp4158;})};(
struct _xenum_struct*) _temp4157;}); break; case 182: _LL4156: yyval=({ struct
Cyc_TypeList_tok_struct* _temp4167=( struct Cyc_TypeList_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeList_tok_struct));* _temp4167=( struct Cyc_TypeList_tok_struct){.tag=
Cyc_TypeList_tok_tag,.f1=({ struct Cyc_List_List* _temp4168=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4168->hd=( void*) Cyc_yyget_Type_tok(({
struct _tagged_ptr2 _temp4169= yyvs; struct _xenum_struct** _temp4171= _temp4169.curr
+ yyvsp_offset; if( _temp4171 < _temp4169.base? 1: _temp4171 >= _temp4169.last_plus_one){
_throw( Null_Exception);}* _temp4171;})); _temp4168->tl= 0; _temp4168;})};(
struct _xenum_struct*) _temp4167;}); break; case 183: _LL4166: yyval=({ struct
Cyc_TypeList_tok_struct* _temp4173=( struct Cyc_TypeList_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeList_tok_struct));* _temp4173=( struct Cyc_TypeList_tok_struct){.tag=
Cyc_TypeList_tok_tag,.f1=({ struct Cyc_List_List* _temp4174=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4174->hd=( void*) Cyc_yyget_Type_tok(({
struct _tagged_ptr2 _temp4178= yyvs; struct _xenum_struct** _temp4180= _temp4178.curr
+ yyvsp_offset; if( _temp4180 < _temp4178.base? 1: _temp4180 >= _temp4178.last_plus_one){
_throw( Null_Exception);}* _temp4180;})); _temp4174->tl= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4175= yyvs; struct _xenum_struct** _temp4177= _temp4175.curr
+( yyvsp_offset - 2); if( _temp4177 < _temp4175.base? 1: _temp4177 >= _temp4175.last_plus_one){
_throw( Null_Exception);}* _temp4177;})); _temp4174;})};( struct _xenum_struct*)
_temp4173;}); break; case 184: _LL4172: yyval=({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4182=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct));* _temp4182=( struct Cyc_AbstractDeclarator_tok_struct){.tag=
Cyc_AbstractDeclarator_tok_tag,.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4183=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4183->tms= Cyc_yyget_TypeModifierList_tok(({ struct _tagged_ptr2 _temp4184=
yyvs; struct _xenum_struct** _temp4186= _temp4184.curr + yyvsp_offset; if(
_temp4186 < _temp4184.base? 1: _temp4186 >= _temp4184.last_plus_one){ _throw(
Null_Exception);}* _temp4186;})); _temp4183;})};( struct _xenum_struct*)
_temp4182;}); break; case 185: _LL4181: yyval=({ struct _tagged_ptr2 _temp4188=
yyvs; struct _xenum_struct** _temp4190= _temp4188.curr + yyvsp_offset; if(
_temp4190 < _temp4188.base? 1: _temp4190 >= _temp4188.last_plus_one){ _throw(
Null_Exception);}* _temp4190;}); break; case 186: _LL4187: yyval=({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4192=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct));* _temp4192=( struct Cyc_AbstractDeclarator_tok_struct){.tag=
Cyc_AbstractDeclarator_tok_tag,.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4193=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4193->tms=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_TypeModifierList_tok(({ struct _tagged_ptr2
_temp4194= yyvs; struct _xenum_struct** _temp4196= _temp4194.curr +(
yyvsp_offset - 1); if( _temp4196 < _temp4194.base? 1: _temp4196 >= _temp4194.last_plus_one){
_throw( Null_Exception);}* _temp4196;})),( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp4197= yyvs; struct _xenum_struct** _temp4199= _temp4197.curr
+ yyvsp_offset; if( _temp4199 < _temp4197.base? 1: _temp4199 >= _temp4197.last_plus_one){
_throw( Null_Exception);}* _temp4199;})))->tms); _temp4193;})};( struct
_xenum_struct*) _temp4192;}); break; case 187: _LL4191: yyval=({ struct
_tagged_ptr2 _temp4201= yyvs; struct _xenum_struct** _temp4203= _temp4201.curr +(
yyvsp_offset - 1); if( _temp4203 < _temp4201.base? 1: _temp4203 >= _temp4201.last_plus_one){
_throw( Null_Exception);}* _temp4203;}); break; case 188: _LL4200: yyval=({
struct Cyc_AbstractDeclarator_tok_struct* _temp4205=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));* _temp4205=(
struct Cyc_AbstractDeclarator_tok_struct){.tag= Cyc_AbstractDeclarator_tok_tag,.f1=({
struct Cyc_Parse_Abstractdeclarator* _temp4206=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4206->tms=({
struct Cyc_List_List* _temp4207=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4207->hd=( void*) Cyc_Absyn_Carray_mod; _temp4207->tl=
0; _temp4207;}); _temp4206;})};( struct _xenum_struct*) _temp4205;}); break;
case 189: _LL4204: yyval=({ struct Cyc_AbstractDeclarator_tok_struct* _temp4209=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));*
_temp4209=( struct Cyc_AbstractDeclarator_tok_struct){.tag= Cyc_AbstractDeclarator_tok_tag,.f1=({
struct Cyc_Parse_Abstractdeclarator* _temp4210=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4210->tms=({
struct Cyc_List_List* _temp4211=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4211->hd=( void*) Cyc_Absyn_Carray_mod; _temp4211->tl=(
Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2 _temp4212= yyvs; struct
_xenum_struct** _temp4214= _temp4212.curr +( yyvsp_offset - 2); if( _temp4214 <
_temp4212.base? 1: _temp4214 >= _temp4212.last_plus_one){ _throw( Null_Exception);}*
_temp4214;})))->tms; _temp4211;}); _temp4210;})};( struct _xenum_struct*)
_temp4209;}); break; case 190: _LL4208: yyval=({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4216=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct));* _temp4216=( struct Cyc_AbstractDeclarator_tok_struct){.tag=
Cyc_AbstractDeclarator_tok_tag,.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4217=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4217->tms=({ struct Cyc_List_List* _temp4218=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4218->hd=( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp4219=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp4219->tag= Cyc_Absyn_ConstArray_mod_tag;
_temp4219->f1= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4220= yyvs; struct
_xenum_struct** _temp4222= _temp4220.curr +( yyvsp_offset - 1); if( _temp4222 <
_temp4220.base? 1: _temp4222 >= _temp4220.last_plus_one){ _throw( Null_Exception);}*
_temp4222;}));( void*) _temp4219;}); _temp4218->tl= 0; _temp4218;}); _temp4217;})};(
struct _xenum_struct*) _temp4216;}); break; case 191: _LL4215: yyval=({ struct
Cyc_AbstractDeclarator_tok_struct* _temp4224=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));* _temp4224=(
struct Cyc_AbstractDeclarator_tok_struct){.tag= Cyc_AbstractDeclarator_tok_tag,.f1=({
struct Cyc_Parse_Abstractdeclarator* _temp4225=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4225->tms=({
struct Cyc_List_List* _temp4226=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4226->hd=( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp4230=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp4230->tag= Cyc_Absyn_ConstArray_mod_tag;
_temp4230->f1= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4231= yyvs; struct
_xenum_struct** _temp4233= _temp4231.curr +( yyvsp_offset - 1); if( _temp4233 <
_temp4231.base? 1: _temp4233 >= _temp4231.last_plus_one){ _throw( Null_Exception);}*
_temp4233;}));( void*) _temp4230;}); _temp4226->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp4227= yyvs; struct _xenum_struct** _temp4229= _temp4227.curr
+( yyvsp_offset - 3); if( _temp4229 < _temp4227.base? 1: _temp4229 >= _temp4227.last_plus_one){
_throw( Null_Exception);}* _temp4229;})))->tms; _temp4226;}); _temp4225;})};(
struct _xenum_struct*) _temp4224;}); break; case 192: _LL4223: yyval=({ struct
Cyc_AbstractDeclarator_tok_struct* _temp4235=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));* _temp4235=(
struct Cyc_AbstractDeclarator_tok_struct){.tag= Cyc_AbstractDeclarator_tok_tag,.f1=({
struct Cyc_Parse_Abstractdeclarator* _temp4236=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4236->tms=({
struct Cyc_List_List* _temp4237=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4237->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4238=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4238->tag= Cyc_Absyn_Function_mod_tag; _temp4238->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4239=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4239->tag= Cyc_Absyn_WithTypes_tag; _temp4239->f1= 0; _temp4239->f2= 0;
_temp4239->f3= 0;( void*) _temp4239;});( void*) _temp4238;}); _temp4237->tl= 0;
_temp4237;}); _temp4236;})};( struct _xenum_struct*) _temp4235;}); break; case
193: _LL4234: yyval=({ struct Cyc_AbstractDeclarator_tok_struct* _temp4241=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));*
_temp4241=( struct Cyc_AbstractDeclarator_tok_struct){.tag= Cyc_AbstractDeclarator_tok_tag,.f1=({
struct Cyc_Parse_Abstractdeclarator* _temp4242=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4242->tms=({
struct Cyc_List_List* _temp4243=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4243->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4244=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4244->tag= Cyc_Absyn_Function_mod_tag; _temp4244->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4245=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4245->tag= Cyc_Absyn_WithTypes_tag; _temp4245->f1= 0; _temp4245->f2= 0;
_temp4245->f3=({ struct Cyc_Core_Opt* _temp4246=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp4246->v=( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp4247=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp4247->tag= Cyc_Absyn_JoinEff_tag; _temp4247->f1= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4248= yyvs; struct _xenum_struct** _temp4250= _temp4248.curr
+( yyvsp_offset - 1); if( _temp4250 < _temp4248.base? 1: _temp4250 >= _temp4248.last_plus_one){
_throw( Null_Exception);}* _temp4250;}));( void*) _temp4247;}); _temp4246;});(
void*) _temp4245;});( void*) _temp4244;}); _temp4243->tl= 0; _temp4243;});
_temp4242;})};( struct _xenum_struct*) _temp4241;}); break; case 194: _LL4240: {
struct _tuple14 _temp4257; struct Cyc_Core_Opt* _temp4258; int _temp4260; struct
Cyc_List_List* _temp4262; struct _tuple14* _temp4255= Cyc_yyget_ParamDeclListBool_tok(({
struct _tagged_ptr2 _temp4252= yyvs; struct _xenum_struct** _temp4254= _temp4252.curr
+( yyvsp_offset - 1); if( _temp4254 < _temp4252.base? 1: _temp4254 >= _temp4252.last_plus_one){
_throw( Null_Exception);}* _temp4254;})); _temp4257=* _temp4255; _LL4263:
_temp4262= _temp4257.f1; goto _LL4261; _LL4261: _temp4260= _temp4257.f2; goto
_LL4259; _LL4259: _temp4258= _temp4257.f3; goto _LL4256; _LL4256: yyval=({
struct Cyc_AbstractDeclarator_tok_struct* _temp4264=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));* _temp4264=(
struct Cyc_AbstractDeclarator_tok_struct){.tag= Cyc_AbstractDeclarator_tok_tag,.f1=({
struct Cyc_Parse_Abstractdeclarator* _temp4265=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4265->tms=({
struct Cyc_List_List* _temp4266=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4266->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4267=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4267->tag= Cyc_Absyn_Function_mod_tag; _temp4267->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4268=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4268->tag= Cyc_Absyn_WithTypes_tag; _temp4268->f1= _temp4262; _temp4268->f2=
_temp4260; _temp4268->f3= _temp4258;( void*) _temp4268;});( void*) _temp4267;});
_temp4266->tl= 0; _temp4266;}); _temp4265;})};( struct _xenum_struct*) _temp4264;});
break;} case 195: _LL4251: yyval=({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4270=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct));* _temp4270=( struct Cyc_AbstractDeclarator_tok_struct){.tag=
Cyc_AbstractDeclarator_tok_tag,.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4271=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4271->tms=({ struct Cyc_List_List* _temp4272=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4272->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4276=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4276->tag= Cyc_Absyn_Function_mod_tag; _temp4276->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4277=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4277->tag= Cyc_Absyn_WithTypes_tag; _temp4277->f1= 0; _temp4277->f2= 0;
_temp4277->f3= 0;( void*) _temp4277;});( void*) _temp4276;}); _temp4272->tl=(
Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2 _temp4273= yyvs; struct
_xenum_struct** _temp4275= _temp4273.curr +( yyvsp_offset - 2); if( _temp4275 <
_temp4273.base? 1: _temp4275 >= _temp4273.last_plus_one){ _throw( Null_Exception);}*
_temp4275;})))->tms; _temp4272;}); _temp4271;})};( struct _xenum_struct*)
_temp4270;}); break; case 196: _LL4269: { struct Cyc_Core_Opt* eff=({ struct Cyc_Core_Opt*
_temp4279=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp4279->v=( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp4280=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp4280->tag= Cyc_Absyn_JoinEff_tag;
_temp4280->f1= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp4281= yyvs;
struct _xenum_struct** _temp4283= _temp4281.curr +( yyvsp_offset - 1); if(
_temp4283 < _temp4281.base? 1: _temp4283 >= _temp4281.last_plus_one){ _throw(
Null_Exception);}* _temp4283;}));( void*) _temp4280;}); _temp4279;}); yyval=({
struct Cyc_AbstractDeclarator_tok_struct* _temp4284=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));* _temp4284=(
struct Cyc_AbstractDeclarator_tok_struct){.tag= Cyc_AbstractDeclarator_tok_tag,.f1=({
struct Cyc_Parse_Abstractdeclarator* _temp4285=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4285->tms=({
struct Cyc_List_List* _temp4286=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4286->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4290=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4290->tag= Cyc_Absyn_Function_mod_tag; _temp4290->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4291=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4291->tag= Cyc_Absyn_WithTypes_tag; _temp4291->f1= 0; _temp4291->f2= 0;
_temp4291->f3=( struct Cyc_Core_Opt*) eff;( void*) _temp4291;});( void*)
_temp4290;}); _temp4286->tl=( Cyc_yyget_AbstractDeclarator_tok(({ struct
_tagged_ptr2 _temp4287= yyvs; struct _xenum_struct** _temp4289= _temp4287.curr +(
yyvsp_offset - 4); if( _temp4289 < _temp4287.base? 1: _temp4289 >= _temp4287.last_plus_one){
_throw( Null_Exception);}* _temp4289;})))->tms; _temp4286;}); _temp4285;})};(
struct _xenum_struct*) _temp4284;}); break;} case 197: _LL4278: { struct
_tuple14 _temp4298; struct Cyc_Core_Opt* _temp4299; int _temp4301; struct Cyc_List_List*
_temp4303; struct _tuple14* _temp4296= Cyc_yyget_ParamDeclListBool_tok(({ struct
_tagged_ptr2 _temp4293= yyvs; struct _xenum_struct** _temp4295= _temp4293.curr +(
yyvsp_offset - 1); if( _temp4295 < _temp4293.base? 1: _temp4295 >= _temp4293.last_plus_one){
_throw( Null_Exception);}* _temp4295;})); _temp4298=* _temp4296; _LL4304:
_temp4303= _temp4298.f1; goto _LL4302; _LL4302: _temp4301= _temp4298.f2; goto
_LL4300; _LL4300: _temp4299= _temp4298.f3; goto _LL4297; _LL4297: yyval=({
struct Cyc_AbstractDeclarator_tok_struct* _temp4305=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));* _temp4305=(
struct Cyc_AbstractDeclarator_tok_struct){.tag= Cyc_AbstractDeclarator_tok_tag,.f1=({
struct Cyc_Parse_Abstractdeclarator* _temp4306=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4306->tms=({
struct Cyc_List_List* _temp4307=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4307->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4311=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4311->tag= Cyc_Absyn_Function_mod_tag; _temp4311->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4312=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4312->tag= Cyc_Absyn_WithTypes_tag; _temp4312->f1= _temp4303; _temp4312->f2=
_temp4301; _temp4312->f3= _temp4299;( void*) _temp4312;});( void*) _temp4311;});
_temp4307->tl=( Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2
_temp4308= yyvs; struct _xenum_struct** _temp4310= _temp4308.curr +(
yyvsp_offset - 3); if( _temp4310 < _temp4308.base? 1: _temp4310 >= _temp4308.last_plus_one){
_throw( Null_Exception);}* _temp4310;})))->tms; _temp4307;}); _temp4306;})};(
struct _xenum_struct*) _temp4305;}); break;} case 198: _LL4292: { struct Cyc_List_List*
ts=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4314=
yyls; struct Cyc_Yyltype* _temp4316= _temp4314.curr +( yylsp_offset - 2); if(
_temp4316 < _temp4314.base? 1: _temp4316 >= _temp4314.last_plus_one){ _throw(
Null_Exception);}* _temp4316;}).first_line,({ struct _tagged_ptr3 _temp4317=
yyls; struct Cyc_Yyltype* _temp4319= _temp4317.curr + yylsp_offset; if(
_temp4319 < _temp4317.base? 1: _temp4319 >= _temp4317.last_plus_one){ _throw(
Null_Exception);}* _temp4319;}).last_line),(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp4320= yyvs; struct _xenum_struct** _temp4322= _temp4320.curr +(
yyvsp_offset - 1); if( _temp4322 < _temp4320.base? 1: _temp4322 >= _temp4320.last_plus_one){
_throw( Null_Exception);}* _temp4322;})))); yyval=({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4323=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct));* _temp4323=( struct Cyc_AbstractDeclarator_tok_struct){.tag=
Cyc_AbstractDeclarator_tok_tag,.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4324=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4324->tms=({ struct Cyc_List_List* _temp4325=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4325->hd=( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp4329=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp4329->tag= Cyc_Absyn_TypeParams_mod_tag;
_temp4329->f1= ts; _temp4329->f2= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4330= yyls; struct Cyc_Yyltype* _temp4332= _temp4330.curr +(
yylsp_offset - 2); if( _temp4332 < _temp4330.base? 1: _temp4332 >= _temp4330.last_plus_one){
_throw( Null_Exception);}* _temp4332;}).first_line,({ struct _tagged_ptr3
_temp4333= yyls; struct Cyc_Yyltype* _temp4335= _temp4333.curr + yylsp_offset;
if( _temp4335 < _temp4333.base? 1: _temp4335 >= _temp4333.last_plus_one){ _throw(
Null_Exception);}* _temp4335;}).last_line); _temp4329->f3= 0;( void*) _temp4329;});
_temp4325->tl=( Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2
_temp4326= yyvs; struct _xenum_struct** _temp4328= _temp4326.curr +(
yyvsp_offset - 3); if( _temp4328 < _temp4326.base? 1: _temp4328 >= _temp4326.last_plus_one){
_throw( Null_Exception);}* _temp4328;})))->tms; _temp4325;}); _temp4324;})};(
struct _xenum_struct*) _temp4323;}); break;} case 199: _LL4313:(( struct Cyc_Lexing_lexbuf*)
Cyc_Parse_lbuf->v)->lex_curr_pos -= 1;{ struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4337= yyls; struct Cyc_Yyltype* _temp4339= _temp4337.curr
+( yylsp_offset - 2); if( _temp4339 < _temp4337.base? 1: _temp4339 >= _temp4337.last_plus_one){
_throw( Null_Exception);}* _temp4339;}).first_line,({ struct _tagged_ptr3
_temp4340= yyls; struct Cyc_Yyltype* _temp4342= _temp4340.curr + yylsp_offset;
if( _temp4342 < _temp4340.base? 1: _temp4342 >= _temp4340.last_plus_one){ _throw(
Null_Exception);}* _temp4342;}).last_line),(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp4343= yyvs; struct _xenum_struct** _temp4345= _temp4343.curr +(
yyvsp_offset - 1); if( _temp4345 < _temp4343.base? 1: _temp4345 >= _temp4343.last_plus_one){
_throw( Null_Exception);}* _temp4345;})))); yyval=({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4346=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct));* _temp4346=( struct Cyc_AbstractDeclarator_tok_struct){.tag=
Cyc_AbstractDeclarator_tok_tag,.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4347=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4347->tms=({ struct Cyc_List_List* _temp4348=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4348->hd=( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp4352=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp4352->tag= Cyc_Absyn_TypeParams_mod_tag;
_temp4352->f1= ts; _temp4352->f2= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4353= yyls; struct Cyc_Yyltype* _temp4355= _temp4353.curr +(
yylsp_offset - 2); if( _temp4355 < _temp4353.base? 1: _temp4355 >= _temp4353.last_plus_one){
_throw( Null_Exception);}* _temp4355;}).first_line,({ struct _tagged_ptr3
_temp4356= yyls; struct Cyc_Yyltype* _temp4358= _temp4356.curr + yylsp_offset;
if( _temp4358 < _temp4356.base? 1: _temp4358 >= _temp4356.last_plus_one){ _throw(
Null_Exception);}* _temp4358;}).last_line); _temp4352->f3= 0;( void*) _temp4352;});
_temp4348->tl=( Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2
_temp4349= yyvs; struct _xenum_struct** _temp4351= _temp4349.curr +(
yyvsp_offset - 3); if( _temp4351 < _temp4349.base? 1: _temp4351 >= _temp4349.last_plus_one){
_throw( Null_Exception);}* _temp4351;})))->tms; _temp4348;}); _temp4347;})};(
struct _xenum_struct*) _temp4346;}); break;} case 200: _LL4336: yyval=({ struct
Cyc_AbstractDeclarator_tok_struct* _temp4360=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));* _temp4360=(
struct Cyc_AbstractDeclarator_tok_struct){.tag= Cyc_AbstractDeclarator_tok_tag,.f1=({
struct Cyc_Parse_Abstractdeclarator* _temp4361=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4361->tms=({
struct Cyc_List_List* _temp4362=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4362->hd=( void*)({ struct Cyc_Absyn_Attributes_mod_struct*
_temp4366=( struct Cyc_Absyn_Attributes_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Attributes_mod_struct)); _temp4366->tag= Cyc_Absyn_Attributes_mod_tag;
_temp4366->f1= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4367=
yyls; struct Cyc_Yyltype* _temp4369= _temp4367.curr + yylsp_offset; if(
_temp4369 < _temp4367.base? 1: _temp4369 >= _temp4367.last_plus_one){ _throw(
Null_Exception);}* _temp4369;}).first_line,({ struct _tagged_ptr3 _temp4370=
yyls; struct Cyc_Yyltype* _temp4372= _temp4370.curr + yylsp_offset; if(
_temp4372 < _temp4370.base? 1: _temp4372 >= _temp4370.last_plus_one){ _throw(
Null_Exception);}* _temp4372;}).last_line); _temp4366->f2= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp4373= yyvs; struct _xenum_struct** _temp4375= _temp4373.curr
+ yyvsp_offset; if( _temp4375 < _temp4373.base? 1: _temp4375 >= _temp4373.last_plus_one){
_throw( Null_Exception);}* _temp4375;}));( void*) _temp4366;}); _temp4362->tl=(
Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2 _temp4363= yyvs; struct
_xenum_struct** _temp4365= _temp4363.curr +( yyvsp_offset - 1); if( _temp4365 <
_temp4363.base? 1: _temp4365 >= _temp4363.last_plus_one){ _throw( Null_Exception);}*
_temp4365;})))->tms; _temp4362;}); _temp4361;})};( struct _xenum_struct*)
_temp4360;}); break; case 201: _LL4359: yyval=({ struct _tagged_ptr2 _temp4377=
yyvs; struct _xenum_struct** _temp4379= _temp4377.curr + yyvsp_offset; if(
_temp4379 < _temp4377.base? 1: _temp4379 >= _temp4377.last_plus_one){ _throw(
Null_Exception);}* _temp4379;}); break; case 202: _LL4376: yyval=({ struct
_tagged_ptr2 _temp4381= yyvs; struct _xenum_struct** _temp4383= _temp4381.curr +
yyvsp_offset; if( _temp4383 < _temp4381.base? 1: _temp4383 >= _temp4381.last_plus_one){
_throw( Null_Exception);}* _temp4383;}); break; case 203: _LL4380: yyval=({
struct _tagged_ptr2 _temp4385= yyvs; struct _xenum_struct** _temp4387= _temp4385.curr
+ yyvsp_offset; if( _temp4387 < _temp4385.base? 1: _temp4387 >= _temp4385.last_plus_one){
_throw( Null_Exception);}* _temp4387;}); break; case 204: _LL4384: yyval=({
struct _tagged_ptr2 _temp4389= yyvs; struct _xenum_struct** _temp4391= _temp4389.curr
+ yyvsp_offset; if( _temp4391 < _temp4389.base? 1: _temp4391 >= _temp4389.last_plus_one){
_throw( Null_Exception);}* _temp4391;}); break; case 205: _LL4388: yyval=({
struct _tagged_ptr2 _temp4393= yyvs; struct _xenum_struct** _temp4395= _temp4393.curr
+ yyvsp_offset; if( _temp4395 < _temp4393.base? 1: _temp4395 >= _temp4393.last_plus_one){
_throw( Null_Exception);}* _temp4395;}); break; case 206: _LL4392: yyval=({
struct _tagged_ptr2 _temp4397= yyvs; struct _xenum_struct** _temp4399= _temp4397.curr
+ yyvsp_offset; if( _temp4399 < _temp4397.base? 1: _temp4399 >= _temp4397.last_plus_one){
_throw( Null_Exception);}* _temp4399;}); break; case 207: _LL4396: yyval=({
struct Cyc_Stmt_tok_struct* _temp4401=( struct Cyc_Stmt_tok_struct*) GC_malloc(
sizeof( struct Cyc_Stmt_tok_struct));* _temp4401=( struct Cyc_Stmt_tok_struct){.tag=
Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_new_stmt(({ struct Cyc_Absyn_Cut_s_struct*
_temp4402=( struct Cyc_Absyn_Cut_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Cut_s_struct));
_temp4402->tag= Cyc_Absyn_Cut_s_tag; _temp4402->f1= Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp4403= yyvs; struct _xenum_struct** _temp4405= _temp4403.curr +
yyvsp_offset; if( _temp4405 < _temp4403.base? 1: _temp4405 >= _temp4403.last_plus_one){
_throw( Null_Exception);}* _temp4405;}));( void*) _temp4402;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4406= yyls; struct Cyc_Yyltype* _temp4408= _temp4406.curr
+( yylsp_offset - 1); if( _temp4408 < _temp4406.base? 1: _temp4408 >= _temp4406.last_plus_one){
_throw( Null_Exception);}* _temp4408;}).first_line,({ struct _tagged_ptr3
_temp4409= yyls; struct Cyc_Yyltype* _temp4411= _temp4409.curr + yylsp_offset;
if( _temp4411 < _temp4409.base? 1: _temp4411 >= _temp4409.last_plus_one){ _throw(
Null_Exception);}* _temp4411;}).last_line))};( struct _xenum_struct*) _temp4401;});
break; case 208: _LL4400: yyval=({ struct Cyc_Stmt_tok_struct* _temp4413=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4413=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_new_stmt(({
struct Cyc_Absyn_Splice_s_struct* _temp4414=( struct Cyc_Absyn_Splice_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Splice_s_struct)); _temp4414->tag= Cyc_Absyn_Splice_s_tag;
_temp4414->f1= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4415= yyvs; struct
_xenum_struct** _temp4417= _temp4415.curr + yyvsp_offset; if( _temp4417 <
_temp4415.base? 1: _temp4417 >= _temp4415.last_plus_one){ _throw( Null_Exception);}*
_temp4417;}));( void*) _temp4414;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4418= yyls; struct Cyc_Yyltype* _temp4420= _temp4418.curr +(
yylsp_offset - 1); if( _temp4420 < _temp4418.base? 1: _temp4420 >= _temp4418.last_plus_one){
_throw( Null_Exception);}* _temp4420;}).first_line,({ struct _tagged_ptr3
_temp4421= yyls; struct Cyc_Yyltype* _temp4423= _temp4421.curr + yylsp_offset;
if( _temp4423 < _temp4421.base? 1: _temp4423 >= _temp4421.last_plus_one){ _throw(
Null_Exception);}* _temp4423;}).last_line))};( struct _xenum_struct*) _temp4413;});
break; case 209: _LL4412: yyval=({ struct Cyc_Stmt_tok_struct* _temp4425=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4425=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_new_stmt(({
struct Cyc_Absyn_Label_s_struct* _temp4426=( struct Cyc_Absyn_Label_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Label_s_struct)); _temp4426->tag= Cyc_Absyn_Label_s_tag;
_temp4426->f1=({ struct _tagged_string* _temp4427=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp4427[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp4428= yyvs; struct _xenum_struct** _temp4430= _temp4428.curr
+( yyvsp_offset - 2); if( _temp4430 < _temp4428.base? 1: _temp4430 >= _temp4428.last_plus_one){
_throw( Null_Exception);}* _temp4430;})); _temp4427;}); _temp4426->f2= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp4431= yyvs; struct _xenum_struct** _temp4433= _temp4431.curr
+ yyvsp_offset; if( _temp4433 < _temp4431.base? 1: _temp4433 >= _temp4431.last_plus_one){
_throw( Null_Exception);}* _temp4433;}));( void*) _temp4426;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4434= yyls; struct Cyc_Yyltype* _temp4436= _temp4434.curr
+( yylsp_offset - 2); if( _temp4436 < _temp4434.base? 1: _temp4436 >= _temp4434.last_plus_one){
_throw( Null_Exception);}* _temp4436;}).first_line,({ struct _tagged_ptr3
_temp4437= yyls; struct Cyc_Yyltype* _temp4439= _temp4437.curr + yylsp_offset;
if( _temp4439 < _temp4437.base? 1: _temp4439 >= _temp4437.last_plus_one){ _throw(
Null_Exception);}* _temp4439;}).last_line))};( struct _xenum_struct*) _temp4425;});
break; case 210: _LL4424: yyval=({ struct Cyc_Stmt_tok_struct* _temp4441=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4441=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4442= yyls; struct Cyc_Yyltype*
_temp4444= _temp4442.curr + yylsp_offset; if( _temp4444 < _temp4442.base? 1:
_temp4444 >= _temp4442.last_plus_one){ _throw( Null_Exception);}* _temp4444;}).first_line,({
struct _tagged_ptr3 _temp4445= yyls; struct Cyc_Yyltype* _temp4447= _temp4445.curr
+ yylsp_offset; if( _temp4447 < _temp4445.base? 1: _temp4447 >= _temp4445.last_plus_one){
_throw( Null_Exception);}* _temp4447;}).last_line))};( struct _xenum_struct*)
_temp4441;}); break; case 211: _LL4440: yyval=({ struct Cyc_Stmt_tok_struct*
_temp4449=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4449=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_exp_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4450= yyvs; struct _xenum_struct**
_temp4452= _temp4450.curr +( yyvsp_offset - 1); if( _temp4452 < _temp4450.base?
1: _temp4452 >= _temp4450.last_plus_one){ _throw( Null_Exception);}* _temp4452;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4453= yyls; struct Cyc_Yyltype*
_temp4455= _temp4453.curr +( yylsp_offset - 1); if( _temp4455 < _temp4453.base?
1: _temp4455 >= _temp4453.last_plus_one){ _throw( Null_Exception);}* _temp4455;}).first_line,({
struct _tagged_ptr3 _temp4456= yyls; struct Cyc_Yyltype* _temp4458= _temp4456.curr
+ yylsp_offset; if( _temp4458 < _temp4456.base? 1: _temp4458 >= _temp4456.last_plus_one){
_throw( Null_Exception);}* _temp4458;}).last_line))};( struct _xenum_struct*)
_temp4449;}); break; case 212: _LL4448: yyval=({ struct Cyc_Stmt_tok_struct*
_temp4460=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4460=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4461= yyls; struct Cyc_Yyltype*
_temp4463= _temp4461.curr +( yylsp_offset - 1); if( _temp4463 < _temp4461.base?
1: _temp4463 >= _temp4461.last_plus_one){ _throw( Null_Exception);}* _temp4463;}).first_line,({
struct _tagged_ptr3 _temp4464= yyls; struct Cyc_Yyltype* _temp4466= _temp4464.curr
+ yylsp_offset; if( _temp4466 < _temp4464.base? 1: _temp4466 >= _temp4464.last_plus_one){
_throw( Null_Exception);}* _temp4466;}).last_line))};( struct _xenum_struct*)
_temp4460;}); break; case 213: _LL4459: yyval=({ struct _tagged_ptr2 _temp4468=
yyvs; struct _xenum_struct** _temp4470= _temp4468.curr +( yyvsp_offset - 1); if(
_temp4470 < _temp4468.base? 1: _temp4470 >= _temp4468.last_plus_one){ _throw(
Null_Exception);}* _temp4470;}); break; case 214: _LL4467:{ void* _temp4475= Cyc_yyget_BlockItem_tok(({
struct _tagged_ptr2 _temp4472= yyvs; struct _xenum_struct** _temp4474= _temp4472.curr
+ yyvsp_offset; if( _temp4474 < _temp4472.base? 1: _temp4474 >= _temp4472.last_plus_one){
_throw( Null_Exception);}* _temp4474;})); struct Cyc_List_List* _temp4483;
struct Cyc_Absyn_Fndecl* _temp4485; struct Cyc_Absyn_Stmt* _temp4487; _LL4477:
if((( struct _enum_struct*) _temp4475)->tag == Cyc_Parse_TopDecls_bl_tag){
_LL4484: _temp4483=( struct Cyc_List_List*)(( struct Cyc_Parse_TopDecls_bl_struct*)
_temp4475)->f1; goto _LL4478;} else{ goto _LL4479;} _LL4479: if((( struct
_enum_struct*) _temp4475)->tag == Cyc_Parse_FnDecl_bl_tag){ _LL4486: _temp4485=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Parse_FnDecl_bl_struct*) _temp4475)->f1;
goto _LL4480;} else{ goto _LL4481;} _LL4481: if((( struct _enum_struct*)
_temp4475)->tag == Cyc_Parse_Stmt_bl_tag){ _LL4488: _temp4487=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Parse_Stmt_bl_struct*) _temp4475)->f1; goto _LL4482;} else{ goto
_LL4476;} _LL4478: yyval=({ struct Cyc_Stmt_tok_struct* _temp4489=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));* _temp4489=( struct Cyc_Stmt_tok_struct){.tag=
Cyc_Stmt_tok_tag,.f1= Cyc_Parse_flatten_declarations( _temp4483, Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4490= yyls; struct Cyc_Yyltype*
_temp4492= _temp4490.curr + yylsp_offset; if( _temp4492 < _temp4490.base? 1:
_temp4492 >= _temp4490.last_plus_one){ _throw( Null_Exception);}* _temp4492;}).first_line,({
struct _tagged_ptr3 _temp4493= yyls; struct Cyc_Yyltype* _temp4495= _temp4493.curr
+ yylsp_offset; if( _temp4495 < _temp4493.base? 1: _temp4495 >= _temp4493.last_plus_one){
_throw( Null_Exception);}* _temp4495;}).last_line)))};( struct _xenum_struct*)
_temp4489;}); goto _LL4476; _LL4480: yyval=({ struct Cyc_Stmt_tok_struct*
_temp4496=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4496=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(({ struct Cyc_Absyn_Fn_d_struct* _temp4497=( struct Cyc_Absyn_Fn_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct)); _temp4497->tag= Cyc_Absyn_Fn_d_tag;
_temp4497->f1= _temp4485;( void*) _temp4497;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4498= yyls; struct Cyc_Yyltype* _temp4500= _temp4498.curr
+ yylsp_offset; if( _temp4500 < _temp4498.base? 1: _temp4500 >= _temp4498.last_plus_one){
_throw( Null_Exception);}* _temp4500;}).first_line,({ struct _tagged_ptr3
_temp4501= yyls; struct Cyc_Yyltype* _temp4503= _temp4501.curr + yylsp_offset;
if( _temp4503 < _temp4501.base? 1: _temp4503 >= _temp4501.last_plus_one){ _throw(
Null_Exception);}* _temp4503;}).last_line)), Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4504= yyls; struct Cyc_Yyltype* _temp4506= _temp4504.curr
+ yylsp_offset; if( _temp4506 < _temp4504.base? 1: _temp4506 >= _temp4504.last_plus_one){
_throw( Null_Exception);}* _temp4506;}).first_line,({ struct _tagged_ptr3
_temp4507= yyls; struct Cyc_Yyltype* _temp4509= _temp4507.curr + yylsp_offset;
if( _temp4509 < _temp4507.base? 1: _temp4509 >= _temp4507.last_plus_one){ _throw(
Null_Exception);}* _temp4509;}).last_line)))};( struct _xenum_struct*) _temp4496;});
goto _LL4476; _LL4482: yyval=({ struct Cyc_Stmt_tok_struct* _temp4510=( struct
Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4510=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= _temp4487};(
struct _xenum_struct*) _temp4510;}); goto _LL4476; _LL4476:;} break; case 215:
_LL4471:{ void* _temp4515= Cyc_yyget_BlockItem_tok(({ struct _tagged_ptr2
_temp4512= yyvs; struct _xenum_struct** _temp4514= _temp4512.curr +(
yyvsp_offset - 1); if( _temp4514 < _temp4512.base? 1: _temp4514 >= _temp4512.last_plus_one){
_throw( Null_Exception);}* _temp4514;})); struct Cyc_List_List* _temp4523;
struct Cyc_Absyn_Fndecl* _temp4525; struct Cyc_Absyn_Stmt* _temp4527; _LL4517:
if((( struct _enum_struct*) _temp4515)->tag == Cyc_Parse_TopDecls_bl_tag){
_LL4524: _temp4523=( struct Cyc_List_List*)(( struct Cyc_Parse_TopDecls_bl_struct*)
_temp4515)->f1; goto _LL4518;} else{ goto _LL4519;} _LL4519: if((( struct
_enum_struct*) _temp4515)->tag == Cyc_Parse_FnDecl_bl_tag){ _LL4526: _temp4525=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Parse_FnDecl_bl_struct*) _temp4515)->f1;
goto _LL4520;} else{ goto _LL4521;} _LL4521: if((( struct _enum_struct*)
_temp4515)->tag == Cyc_Parse_Stmt_bl_tag){ _LL4528: _temp4527=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Parse_Stmt_bl_struct*) _temp4515)->f1; goto _LL4522;} else{ goto
_LL4516;} _LL4518: yyval=({ struct Cyc_Stmt_tok_struct* _temp4529=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));* _temp4529=( struct Cyc_Stmt_tok_struct){.tag=
Cyc_Stmt_tok_tag,.f1= Cyc_Parse_flatten_declarations( _temp4523, Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp4530= yyvs; struct _xenum_struct** _temp4532= _temp4530.curr
+ yyvsp_offset; if( _temp4532 < _temp4530.base? 1: _temp4532 >= _temp4530.last_plus_one){
_throw( Null_Exception);}* _temp4532;})))};( struct _xenum_struct*) _temp4529;});
goto _LL4516; _LL4520: yyval=({ struct Cyc_Stmt_tok_struct* _temp4533=( struct
Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4533=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(({ struct Cyc_Absyn_Fn_d_struct* _temp4534=( struct Cyc_Absyn_Fn_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct)); _temp4534->tag= Cyc_Absyn_Fn_d_tag;
_temp4534->f1= _temp4525;( void*) _temp4534;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4535= yyls; struct Cyc_Yyltype* _temp4537= _temp4535.curr
+( yylsp_offset - 1); if( _temp4537 < _temp4535.base? 1: _temp4537 >= _temp4535.last_plus_one){
_throw( Null_Exception);}* _temp4537;}).first_line,({ struct _tagged_ptr3
_temp4538= yyls; struct Cyc_Yyltype* _temp4540= _temp4538.curr +( yylsp_offset -
1); if( _temp4540 < _temp4538.base? 1: _temp4540 >= _temp4538.last_plus_one){
_throw( Null_Exception);}* _temp4540;}).last_line)), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp4541= yyvs; struct _xenum_struct** _temp4543= _temp4541.curr
+ yyvsp_offset; if( _temp4543 < _temp4541.base? 1: _temp4543 >= _temp4541.last_plus_one){
_throw( Null_Exception);}* _temp4543;})))};( struct _xenum_struct*) _temp4533;});
goto _LL4516; _LL4522: yyval=({ struct Cyc_Stmt_tok_struct* _temp4544=( struct
Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4544=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_seq_stmt(
_temp4527, Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4545= yyvs; struct
_xenum_struct** _temp4547= _temp4545.curr + yyvsp_offset; if( _temp4547 <
_temp4545.base? 1: _temp4547 >= _temp4545.last_plus_one){ _throw( Null_Exception);}*
_temp4547;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4548=
yyls; struct Cyc_Yyltype* _temp4550= _temp4548.curr +( yylsp_offset - 1); if(
_temp4550 < _temp4548.base? 1: _temp4550 >= _temp4548.last_plus_one){ _throw(
Null_Exception);}* _temp4550;}).first_line,({ struct _tagged_ptr3 _temp4551=
yyls; struct Cyc_Yyltype* _temp4553= _temp4551.curr + yylsp_offset; if(
_temp4553 < _temp4551.base? 1: _temp4553 >= _temp4551.last_plus_one){ _throw(
Null_Exception);}* _temp4553;}).last_line))};( struct _xenum_struct*) _temp4544;});
goto _LL4516; _LL4516:;} break; case 216: _LL4511: yyval=({ struct Cyc_BlockItem_tok_struct*
_temp4555=( struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));*
_temp4555=( struct Cyc_BlockItem_tok_struct){.tag= Cyc_BlockItem_tok_tag,.f1=({
struct Cyc_Parse_TopDecls_bl_struct* _temp4556=( struct Cyc_Parse_TopDecls_bl_struct*)
GC_malloc( sizeof( struct Cyc_Parse_TopDecls_bl_struct)); _temp4556->tag= Cyc_Parse_TopDecls_bl_tag;
_temp4556->f1= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp4557= yyvs;
struct _xenum_struct** _temp4559= _temp4557.curr + yyvsp_offset; if( _temp4559 <
_temp4557.base? 1: _temp4559 >= _temp4557.last_plus_one){ _throw( Null_Exception);}*
_temp4559;}));( void*) _temp4556;})};( struct _xenum_struct*) _temp4555;});
break; case 217: _LL4554: yyval=({ struct Cyc_BlockItem_tok_struct* _temp4561=(
struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));*
_temp4561=( struct Cyc_BlockItem_tok_struct){.tag= Cyc_BlockItem_tok_tag,.f1=({
struct Cyc_Parse_Stmt_bl_struct* _temp4562=( struct Cyc_Parse_Stmt_bl_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Stmt_bl_struct)); _temp4562->tag= Cyc_Parse_Stmt_bl_tag;
_temp4562->f1= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4563= yyvs; struct
_xenum_struct** _temp4565= _temp4563.curr + yyvsp_offset; if( _temp4565 <
_temp4563.base? 1: _temp4565 >= _temp4563.last_plus_one){ _throw( Null_Exception);}*
_temp4565;}));( void*) _temp4562;})};( struct _xenum_struct*) _temp4561;});
break; case 218: _LL4560: yyval=({ struct Cyc_BlockItem_tok_struct* _temp4567=(
struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));*
_temp4567=( struct Cyc_BlockItem_tok_struct){.tag= Cyc_BlockItem_tok_tag,.f1=({
struct Cyc_Parse_FnDecl_bl_struct* _temp4568=( struct Cyc_Parse_FnDecl_bl_struct*)
GC_malloc( sizeof( struct Cyc_Parse_FnDecl_bl_struct)); _temp4568->tag= Cyc_Parse_FnDecl_bl_tag;
_temp4568->f1= Cyc_yyget_FnDecl_tok(({ struct _tagged_ptr2 _temp4569= yyvs;
struct _xenum_struct** _temp4571= _temp4569.curr + yyvsp_offset; if( _temp4571 <
_temp4569.base? 1: _temp4571 >= _temp4569.last_plus_one){ _throw( Null_Exception);}*
_temp4571;}));( void*) _temp4568;})};( struct _xenum_struct*) _temp4567;});
break; case 219: _LL4566: yyval=({ struct Cyc_Stmt_tok_struct* _temp4573=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4573=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4574= yyvs; struct _xenum_struct**
_temp4576= _temp4574.curr +( yyvsp_offset - 2); if( _temp4576 < _temp4574.base?
1: _temp4576 >= _temp4574.last_plus_one){ _throw( Null_Exception);}* _temp4576;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4577= yyvs; struct _xenum_struct**
_temp4579= _temp4577.curr + yyvsp_offset; if( _temp4579 < _temp4577.base? 1:
_temp4579 >= _temp4577.last_plus_one){ _throw( Null_Exception);}* _temp4579;})),
Cyc_Absyn_skip_stmt( 0), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4580= yyls; struct Cyc_Yyltype* _temp4582= _temp4580.curr +( yylsp_offset -
4); if( _temp4582 < _temp4580.base? 1: _temp4582 >= _temp4580.last_plus_one){
_throw( Null_Exception);}* _temp4582;}).first_line,({ struct _tagged_ptr3
_temp4583= yyls; struct Cyc_Yyltype* _temp4585= _temp4583.curr + yylsp_offset;
if( _temp4585 < _temp4583.base? 1: _temp4585 >= _temp4583.last_plus_one){ _throw(
Null_Exception);}* _temp4585;}).last_line))};( struct _xenum_struct*) _temp4573;});
break; case 220: _LL4572: yyval=({ struct Cyc_Stmt_tok_struct* _temp4587=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4587=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4588= yyvs; struct _xenum_struct**
_temp4590= _temp4588.curr +( yyvsp_offset - 4); if( _temp4590 < _temp4588.base?
1: _temp4590 >= _temp4588.last_plus_one){ _throw( Null_Exception);}* _temp4590;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4591= yyvs; struct _xenum_struct**
_temp4593= _temp4591.curr +( yyvsp_offset - 2); if( _temp4593 < _temp4591.base?
1: _temp4593 >= _temp4591.last_plus_one){ _throw( Null_Exception);}* _temp4593;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4594= yyvs; struct _xenum_struct**
_temp4596= _temp4594.curr + yyvsp_offset; if( _temp4596 < _temp4594.base? 1:
_temp4596 >= _temp4594.last_plus_one){ _throw( Null_Exception);}* _temp4596;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4597= yyls; struct Cyc_Yyltype*
_temp4599= _temp4597.curr +( yylsp_offset - 6); if( _temp4599 < _temp4597.base?
1: _temp4599 >= _temp4597.last_plus_one){ _throw( Null_Exception);}* _temp4599;}).first_line,({
struct _tagged_ptr3 _temp4600= yyls; struct Cyc_Yyltype* _temp4602= _temp4600.curr
+ yylsp_offset; if( _temp4602 < _temp4600.base? 1: _temp4602 >= _temp4600.last_plus_one){
_throw( Null_Exception);}* _temp4602;}).last_line))};( struct _xenum_struct*)
_temp4587;}); break; case 221: _LL4586: yyval=({ struct Cyc_Stmt_tok_struct*
_temp4604=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4604=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_switch_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4605= yyvs; struct _xenum_struct**
_temp4607= _temp4605.curr +( yyvsp_offset - 4); if( _temp4607 < _temp4605.base?
1: _temp4607 >= _temp4605.last_plus_one){ _throw( Null_Exception);}* _temp4607;})),
Cyc_yyget_SwitchClauseList_tok(({ struct _tagged_ptr2 _temp4608= yyvs; struct
_xenum_struct** _temp4610= _temp4608.curr +( yyvsp_offset - 1); if( _temp4610 <
_temp4608.base? 1: _temp4610 >= _temp4608.last_plus_one){ _throw( Null_Exception);}*
_temp4610;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4611=
yyls; struct Cyc_Yyltype* _temp4613= _temp4611.curr +( yylsp_offset - 6); if(
_temp4613 < _temp4611.base? 1: _temp4613 >= _temp4611.last_plus_one){ _throw(
Null_Exception);}* _temp4613;}).first_line,({ struct _tagged_ptr3 _temp4614=
yyls; struct Cyc_Yyltype* _temp4616= _temp4614.curr + yylsp_offset; if(
_temp4616 < _temp4614.base? 1: _temp4616 >= _temp4614.last_plus_one){ _throw(
Null_Exception);}* _temp4616;}).last_line))};( struct _xenum_struct*) _temp4604;});
break; case 222: _LL4603: yyval=({ struct Cyc_Stmt_tok_struct* _temp4618=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4618=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_trycatch_stmt(
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4619= yyvs; struct _xenum_struct**
_temp4621= _temp4619.curr +( yyvsp_offset - 4); if( _temp4621 < _temp4619.base?
1: _temp4621 >= _temp4619.last_plus_one){ _throw( Null_Exception);}* _temp4621;})),
Cyc_yyget_SwitchClauseList_tok(({ struct _tagged_ptr2 _temp4622= yyvs; struct
_xenum_struct** _temp4624= _temp4622.curr +( yyvsp_offset - 1); if( _temp4624 <
_temp4622.base? 1: _temp4624 >= _temp4622.last_plus_one){ _throw( Null_Exception);}*
_temp4624;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4625=
yyls; struct Cyc_Yyltype* _temp4627= _temp4625.curr +( yylsp_offset - 5); if(
_temp4627 < _temp4625.base? 1: _temp4627 >= _temp4625.last_plus_one){ _throw(
Null_Exception);}* _temp4627;}).first_line,({ struct _tagged_ptr3 _temp4628=
yyls; struct Cyc_Yyltype* _temp4630= _temp4628.curr + yylsp_offset; if(
_temp4630 < _temp4628.base? 1: _temp4630 >= _temp4628.last_plus_one){ _throw(
Null_Exception);}* _temp4630;}).last_line))};( struct _xenum_struct*) _temp4618;});
break; case 223: _LL4617: yyval=({ struct Cyc_SwitchClauseList_tok_struct*
_temp4632=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct));* _temp4632=( struct Cyc_SwitchClauseList_tok_struct){.tag=
Cyc_SwitchClauseList_tok_tag,.f1= 0};( struct _xenum_struct*) _temp4632;});
break; case 224: _LL4631: yyval=({ struct Cyc_SwitchClauseList_tok_struct*
_temp4634=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct));* _temp4634=( struct Cyc_SwitchClauseList_tok_struct){.tag=
Cyc_SwitchClauseList_tok_tag,.f1=({ struct Cyc_List_List* _temp4635=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4635->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp4636=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp4636->pattern= Cyc_Absyn_new_pat( Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4646= yyls; struct Cyc_Yyltype* _temp4648= _temp4646.curr
+( yylsp_offset - 2); if( _temp4648 < _temp4646.base? 1: _temp4648 >= _temp4646.last_plus_one){
_throw( Null_Exception);}* _temp4648;}).first_line,({ struct _tagged_ptr3
_temp4649= yyls; struct Cyc_Yyltype* _temp4651= _temp4649.curr +( yylsp_offset -
2); if( _temp4651 < _temp4649.base? 1: _temp4651 >= _temp4649.last_plus_one){
_throw( Null_Exception);}* _temp4651;}).last_line)); _temp4636->pat_vars= 0;
_temp4636->where_clause= 0; _temp4636->body= Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp4643= yyvs; struct _xenum_struct** _temp4645= _temp4643.curr +
yyvsp_offset; if( _temp4645 < _temp4643.base? 1: _temp4645 >= _temp4643.last_plus_one){
_throw( Null_Exception);}* _temp4645;})); _temp4636->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4637= yyls; struct Cyc_Yyltype* _temp4639= _temp4637.curr
+( yylsp_offset - 2); if( _temp4639 < _temp4637.base? 1: _temp4639 >= _temp4637.last_plus_one){
_throw( Null_Exception);}* _temp4639;}).first_line,({ struct _tagged_ptr3
_temp4640= yyls; struct Cyc_Yyltype* _temp4642= _temp4640.curr + yylsp_offset;
if( _temp4642 < _temp4640.base? 1: _temp4642 >= _temp4640.last_plus_one){ _throw(
Null_Exception);}* _temp4642;}).last_line); _temp4636;}); _temp4635->tl= 0;
_temp4635;})};( struct _xenum_struct*) _temp4634;}); break; case 225: _LL4633:
yyval=({ struct Cyc_SwitchClauseList_tok_struct* _temp4653=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));* _temp4653=( struct
Cyc_SwitchClauseList_tok_struct){.tag= Cyc_SwitchClauseList_tok_tag,.f1=({
struct Cyc_List_List* _temp4654=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4654->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp4658=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp4658->pattern= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp4671= yyvs;
struct _xenum_struct** _temp4673= _temp4671.curr +( yyvsp_offset - 2); if(
_temp4673 < _temp4671.base? 1: _temp4673 >= _temp4671.last_plus_one){ _throw(
Null_Exception);}* _temp4673;})); _temp4658->pat_vars= 0; _temp4658->where_clause=
0; _temp4658->body= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4665= yyls; struct Cyc_Yyltype* _temp4667= _temp4665.curr +(
yylsp_offset - 1); if( _temp4667 < _temp4665.base? 1: _temp4667 >= _temp4665.last_plus_one){
_throw( Null_Exception);}* _temp4667;}).first_line,({ struct _tagged_ptr3
_temp4668= yyls; struct Cyc_Yyltype* _temp4670= _temp4668.curr +( yylsp_offset -
1); if( _temp4670 < _temp4668.base? 1: _temp4670 >= _temp4668.last_plus_one){
_throw( Null_Exception);}* _temp4670;}).last_line)); _temp4658->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4659= yyls; struct Cyc_Yyltype* _temp4661= _temp4659.curr
+( yylsp_offset - 3); if( _temp4661 < _temp4659.base? 1: _temp4661 >= _temp4659.last_plus_one){
_throw( Null_Exception);}* _temp4661;}).first_line,({ struct _tagged_ptr3
_temp4662= yyls; struct Cyc_Yyltype* _temp4664= _temp4662.curr + yylsp_offset;
if( _temp4664 < _temp4662.base? 1: _temp4664 >= _temp4662.last_plus_one){ _throw(
Null_Exception);}* _temp4664;}).last_line); _temp4658;}); _temp4654->tl= Cyc_yyget_SwitchClauseList_tok(({
struct _tagged_ptr2 _temp4655= yyvs; struct _xenum_struct** _temp4657= _temp4655.curr
+ yyvsp_offset; if( _temp4657 < _temp4655.base? 1: _temp4657 >= _temp4655.last_plus_one){
_throw( Null_Exception);}* _temp4657;})); _temp4654;})};( struct _xenum_struct*)
_temp4653;}); break; case 226: _LL4652: yyval=({ struct Cyc_SwitchClauseList_tok_struct*
_temp4675=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct));* _temp4675=( struct Cyc_SwitchClauseList_tok_struct){.tag=
Cyc_SwitchClauseList_tok_tag,.f1=({ struct Cyc_List_List* _temp4676=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4676->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp4680=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp4680->pattern= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp4690= yyvs;
struct _xenum_struct** _temp4692= _temp4690.curr +( yyvsp_offset - 3); if(
_temp4692 < _temp4690.base? 1: _temp4692 >= _temp4690.last_plus_one){ _throw(
Null_Exception);}* _temp4692;})); _temp4680->pat_vars= 0; _temp4680->where_clause=
0; _temp4680->body= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4687= yyvs;
struct _xenum_struct** _temp4689= _temp4687.curr +( yyvsp_offset - 1); if(
_temp4689 < _temp4687.base? 1: _temp4689 >= _temp4687.last_plus_one){ _throw(
Null_Exception);}* _temp4689;})); _temp4680->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4681= yyls; struct Cyc_Yyltype* _temp4683= _temp4681.curr
+( yylsp_offset - 4); if( _temp4683 < _temp4681.base? 1: _temp4683 >= _temp4681.last_plus_one){
_throw( Null_Exception);}* _temp4683;}).first_line,({ struct _tagged_ptr3
_temp4684= yyls; struct Cyc_Yyltype* _temp4686= _temp4684.curr +( yylsp_offset -
1); if( _temp4686 < _temp4684.base? 1: _temp4686 >= _temp4684.last_plus_one){
_throw( Null_Exception);}* _temp4686;}).last_line); _temp4680;}); _temp4676->tl=
Cyc_yyget_SwitchClauseList_tok(({ struct _tagged_ptr2 _temp4677= yyvs; struct
_xenum_struct** _temp4679= _temp4677.curr + yyvsp_offset; if( _temp4679 <
_temp4677.base? 1: _temp4679 >= _temp4677.last_plus_one){ _throw( Null_Exception);}*
_temp4679;})); _temp4676;})};( struct _xenum_struct*) _temp4675;}); break; case
227: _LL4674: yyval=({ struct Cyc_SwitchClauseList_tok_struct* _temp4694=(
struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));*
_temp4694=( struct Cyc_SwitchClauseList_tok_struct){.tag= Cyc_SwitchClauseList_tok_tag,.f1=({
struct Cyc_List_List* _temp4695=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4695->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp4699=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp4699->pattern= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp4715= yyvs;
struct _xenum_struct** _temp4717= _temp4715.curr +( yyvsp_offset - 4); if(
_temp4717 < _temp4715.base? 1: _temp4717 >= _temp4715.last_plus_one){ _throw(
Null_Exception);}* _temp4717;})); _temp4699->pat_vars= 0; _temp4699->where_clause=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4712= yyvs;
struct _xenum_struct** _temp4714= _temp4712.curr +( yyvsp_offset - 2); if(
_temp4714 < _temp4712.base? 1: _temp4714 >= _temp4712.last_plus_one){ _throw(
Null_Exception);}* _temp4714;})); _temp4699->body= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4706= yyls; struct Cyc_Yyltype* _temp4708= _temp4706.curr
+( yylsp_offset - 1); if( _temp4708 < _temp4706.base? 1: _temp4708 >= _temp4706.last_plus_one){
_throw( Null_Exception);}* _temp4708;}).first_line,({ struct _tagged_ptr3
_temp4709= yyls; struct Cyc_Yyltype* _temp4711= _temp4709.curr +( yylsp_offset -
1); if( _temp4711 < _temp4709.base? 1: _temp4711 >= _temp4709.last_plus_one){
_throw( Null_Exception);}* _temp4711;}).last_line)); _temp4699->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4700= yyls; struct Cyc_Yyltype* _temp4702= _temp4700.curr
+( yylsp_offset - 5); if( _temp4702 < _temp4700.base? 1: _temp4702 >= _temp4700.last_plus_one){
_throw( Null_Exception);}* _temp4702;}).first_line,({ struct _tagged_ptr3
_temp4703= yyls; struct Cyc_Yyltype* _temp4705= _temp4703.curr + yylsp_offset;
if( _temp4705 < _temp4703.base? 1: _temp4705 >= _temp4703.last_plus_one){ _throw(
Null_Exception);}* _temp4705;}).last_line); _temp4699;}); _temp4695->tl= Cyc_yyget_SwitchClauseList_tok(({
struct _tagged_ptr2 _temp4696= yyvs; struct _xenum_struct** _temp4698= _temp4696.curr
+ yyvsp_offset; if( _temp4698 < _temp4696.base? 1: _temp4698 >= _temp4696.last_plus_one){
_throw( Null_Exception);}* _temp4698;})); _temp4695;})};( struct _xenum_struct*)
_temp4694;}); break; case 228: _LL4693: yyval=({ struct Cyc_SwitchClauseList_tok_struct*
_temp4719=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct));* _temp4719=( struct Cyc_SwitchClauseList_tok_struct){.tag=
Cyc_SwitchClauseList_tok_tag,.f1=({ struct Cyc_List_List* _temp4720=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4720->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp4724=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp4724->pattern= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp4737= yyvs;
struct _xenum_struct** _temp4739= _temp4737.curr +( yyvsp_offset - 5); if(
_temp4739 < _temp4737.base? 1: _temp4739 >= _temp4737.last_plus_one){ _throw(
Null_Exception);}* _temp4739;})); _temp4724->pat_vars= 0; _temp4724->where_clause=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4734= yyvs;
struct _xenum_struct** _temp4736= _temp4734.curr +( yyvsp_offset - 3); if(
_temp4736 < _temp4734.base? 1: _temp4736 >= _temp4734.last_plus_one){ _throw(
Null_Exception);}* _temp4736;})); _temp4724->body= Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp4731= yyvs; struct _xenum_struct** _temp4733= _temp4731.curr +(
yyvsp_offset - 1); if( _temp4733 < _temp4731.base? 1: _temp4733 >= _temp4731.last_plus_one){
_throw( Null_Exception);}* _temp4733;})); _temp4724->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4725= yyls; struct Cyc_Yyltype* _temp4727= _temp4725.curr
+( yylsp_offset - 6); if( _temp4727 < _temp4725.base? 1: _temp4727 >= _temp4725.last_plus_one){
_throw( Null_Exception);}* _temp4727;}).first_line,({ struct _tagged_ptr3
_temp4728= yyls; struct Cyc_Yyltype* _temp4730= _temp4728.curr + yylsp_offset;
if( _temp4730 < _temp4728.base? 1: _temp4730 >= _temp4728.last_plus_one){ _throw(
Null_Exception);}* _temp4730;}).last_line); _temp4724;}); _temp4720->tl= Cyc_yyget_SwitchClauseList_tok(({
struct _tagged_ptr2 _temp4721= yyvs; struct _xenum_struct** _temp4723= _temp4721.curr
+ yyvsp_offset; if( _temp4723 < _temp4721.base? 1: _temp4723 >= _temp4721.last_plus_one){
_throw( Null_Exception);}* _temp4723;})); _temp4720;})};( struct _xenum_struct*)
_temp4719;}); break; case 229: _LL4718: yyval=({ struct Cyc_Stmt_tok_struct*
_temp4741=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4741=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_while_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4742= yyvs; struct _xenum_struct**
_temp4744= _temp4742.curr +( yyvsp_offset - 2); if( _temp4744 < _temp4742.base?
1: _temp4744 >= _temp4742.last_plus_one){ _throw( Null_Exception);}* _temp4744;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4745= yyvs; struct _xenum_struct**
_temp4747= _temp4745.curr + yyvsp_offset; if( _temp4747 < _temp4745.base? 1:
_temp4747 >= _temp4745.last_plus_one){ _throw( Null_Exception);}* _temp4747;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4748= yyls; struct Cyc_Yyltype*
_temp4750= _temp4748.curr +( yylsp_offset - 4); if( _temp4750 < _temp4748.base?
1: _temp4750 >= _temp4748.last_plus_one){ _throw( Null_Exception);}* _temp4750;}).first_line,({
struct _tagged_ptr3 _temp4751= yyls; struct Cyc_Yyltype* _temp4753= _temp4751.curr
+ yylsp_offset; if( _temp4753 < _temp4751.base? 1: _temp4753 >= _temp4751.last_plus_one){
_throw( Null_Exception);}* _temp4753;}).last_line))};( struct _xenum_struct*)
_temp4741;}); break; case 230: _LL4740: yyval=({ struct Cyc_Stmt_tok_struct*
_temp4755=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4755=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_do_stmt(
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4756= yyvs; struct _xenum_struct**
_temp4758= _temp4756.curr +( yyvsp_offset - 5); if( _temp4758 < _temp4756.base?
1: _temp4758 >= _temp4756.last_plus_one){ _throw( Null_Exception);}* _temp4758;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4759= yyvs; struct _xenum_struct**
_temp4761= _temp4759.curr +( yyvsp_offset - 2); if( _temp4761 < _temp4759.base?
1: _temp4761 >= _temp4759.last_plus_one){ _throw( Null_Exception);}* _temp4761;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4762= yyls; struct Cyc_Yyltype*
_temp4764= _temp4762.curr +( yylsp_offset - 6); if( _temp4764 < _temp4762.base?
1: _temp4764 >= _temp4762.last_plus_one){ _throw( Null_Exception);}* _temp4764;}).first_line,({
struct _tagged_ptr3 _temp4765= yyls; struct Cyc_Yyltype* _temp4767= _temp4765.curr
+ yylsp_offset; if( _temp4767 < _temp4765.base? 1: _temp4767 >= _temp4765.last_plus_one){
_throw( Null_Exception);}* _temp4767;}).last_line))};( struct _xenum_struct*)
_temp4755;}); break; case 231: _LL4754: yyval=({ struct Cyc_Stmt_tok_struct*
_temp4769=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4769=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp4770= yyvs; struct _xenum_struct** _temp4772= _temp4770.curr
+ yyvsp_offset; if( _temp4772 < _temp4770.base? 1: _temp4772 >= _temp4770.last_plus_one){
_throw( Null_Exception);}* _temp4772;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4773= yyls; struct Cyc_Yyltype* _temp4775= _temp4773.curr +(
yylsp_offset - 5); if( _temp4775 < _temp4773.base? 1: _temp4775 >= _temp4773.last_plus_one){
_throw( Null_Exception);}* _temp4775;}).first_line,({ struct _tagged_ptr3
_temp4776= yyls; struct Cyc_Yyltype* _temp4778= _temp4776.curr + yylsp_offset;
if( _temp4778 < _temp4776.base? 1: _temp4778 >= _temp4776.last_plus_one){ _throw(
Null_Exception);}* _temp4778;}).last_line))};( struct _xenum_struct*) _temp4769;});
break; case 232: _LL4768: yyval=({ struct Cyc_Stmt_tok_struct* _temp4780=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4780=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4781= yyvs; struct _xenum_struct** _temp4783= _temp4781.curr +(
yyvsp_offset - 2); if( _temp4783 < _temp4781.base? 1: _temp4783 >= _temp4781.last_plus_one){
_throw( Null_Exception);}* _temp4783;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp4784= yyvs; struct _xenum_struct** _temp4786= _temp4784.curr +
yyvsp_offset; if( _temp4786 < _temp4784.base? 1: _temp4786 >= _temp4784.last_plus_one){
_throw( Null_Exception);}* _temp4786;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4787= yyls; struct Cyc_Yyltype* _temp4789= _temp4787.curr +(
yylsp_offset - 6); if( _temp4789 < _temp4787.base? 1: _temp4789 >= _temp4787.last_plus_one){
_throw( Null_Exception);}* _temp4789;}).first_line,({ struct _tagged_ptr3
_temp4790= yyls; struct Cyc_Yyltype* _temp4792= _temp4790.curr + yylsp_offset;
if( _temp4792 < _temp4790.base? 1: _temp4792 >= _temp4790.last_plus_one){ _throw(
Null_Exception);}* _temp4792;}).last_line))};( struct _xenum_struct*) _temp4780;});
break; case 233: _LL4779: yyval=({ struct Cyc_Stmt_tok_struct* _temp4794=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4794=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4795=
yyvs; struct _xenum_struct** _temp4797= _temp4795.curr +( yyvsp_offset - 3); if(
_temp4797 < _temp4795.base? 1: _temp4797 >= _temp4795.last_plus_one){ _throw(
Null_Exception);}* _temp4797;})), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp4798= yyvs; struct _xenum_struct** _temp4800= _temp4798.curr
+ yyvsp_offset; if( _temp4800 < _temp4798.base? 1: _temp4800 >= _temp4798.last_plus_one){
_throw( Null_Exception);}* _temp4800;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4801= yyls; struct Cyc_Yyltype* _temp4803= _temp4801.curr +(
yylsp_offset - 6); if( _temp4803 < _temp4801.base? 1: _temp4803 >= _temp4801.last_plus_one){
_throw( Null_Exception);}* _temp4803;}).first_line,({ struct _tagged_ptr3
_temp4804= yyls; struct Cyc_Yyltype* _temp4806= _temp4804.curr + yylsp_offset;
if( _temp4806 < _temp4804.base? 1: _temp4806 >= _temp4804.last_plus_one){ _throw(
Null_Exception);}* _temp4806;}).last_line))};( struct _xenum_struct*) _temp4794;});
break; case 234: _LL4793: yyval=({ struct Cyc_Stmt_tok_struct* _temp4808=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4808=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4809=
yyvs; struct _xenum_struct** _temp4811= _temp4809.curr +( yyvsp_offset - 4); if(
_temp4811 < _temp4809.base? 1: _temp4811 >= _temp4809.last_plus_one){ _throw(
Null_Exception);}* _temp4811;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp4812= yyvs; struct _xenum_struct** _temp4814= _temp4812.curr +(
yyvsp_offset - 2); if( _temp4814 < _temp4812.base? 1: _temp4814 >= _temp4812.last_plus_one){
_throw( Null_Exception);}* _temp4814;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp4815= yyvs; struct _xenum_struct** _temp4817= _temp4815.curr +
yyvsp_offset; if( _temp4817 < _temp4815.base? 1: _temp4817 >= _temp4815.last_plus_one){
_throw( Null_Exception);}* _temp4817;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4818= yyls; struct Cyc_Yyltype* _temp4820= _temp4818.curr +(
yylsp_offset - 7); if( _temp4820 < _temp4818.base? 1: _temp4820 >= _temp4818.last_plus_one){
_throw( Null_Exception);}* _temp4820;}).first_line,({ struct _tagged_ptr3
_temp4821= yyls; struct Cyc_Yyltype* _temp4823= _temp4821.curr +( yylsp_offset -
1); if( _temp4823 < _temp4821.base? 1: _temp4823 >= _temp4821.last_plus_one){
_throw( Null_Exception);}* _temp4823;}).last_line))};( struct _xenum_struct*)
_temp4808;}); break; case 235: _LL4807: yyval=({ struct Cyc_Stmt_tok_struct*
_temp4825=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4825=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_for_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4826= yyvs; struct _xenum_struct**
_temp4828= _temp4826.curr +( yyvsp_offset - 4); if( _temp4828 < _temp4826.base?
1: _temp4828 >= _temp4826.last_plus_one){ _throw( Null_Exception);}* _temp4828;})),
Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp4829= yyvs; struct _xenum_struct** _temp4831= _temp4829.curr +
yyvsp_offset; if( _temp4831 < _temp4829.base? 1: _temp4831 >= _temp4829.last_plus_one){
_throw( Null_Exception);}* _temp4831;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4832= yyls; struct Cyc_Yyltype* _temp4834= _temp4832.curr +(
yylsp_offset - 6); if( _temp4834 < _temp4832.base? 1: _temp4834 >= _temp4832.last_plus_one){
_throw( Null_Exception);}* _temp4834;}).first_line,({ struct _tagged_ptr3
_temp4835= yyls; struct Cyc_Yyltype* _temp4837= _temp4835.curr + yylsp_offset;
if( _temp4837 < _temp4835.base? 1: _temp4837 >= _temp4835.last_plus_one){ _throw(
Null_Exception);}* _temp4837;}).last_line))};( struct _xenum_struct*) _temp4825;});
break; case 236: _LL4824: yyval=({ struct Cyc_Stmt_tok_struct* _temp4839=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4839=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_for_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4840= yyvs; struct _xenum_struct**
_temp4842= _temp4840.curr +( yyvsp_offset - 5); if( _temp4842 < _temp4840.base?
1: _temp4842 >= _temp4840.last_plus_one){ _throw( Null_Exception);}* _temp4842;})),
Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4843= yyvs;
struct _xenum_struct** _temp4845= _temp4843.curr +( yyvsp_offset - 2); if(
_temp4845 < _temp4843.base? 1: _temp4845 >= _temp4843.last_plus_one){ _throw(
Null_Exception);}* _temp4845;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp4846= yyvs; struct _xenum_struct** _temp4848= _temp4846.curr + yyvsp_offset;
if( _temp4848 < _temp4846.base? 1: _temp4848 >= _temp4846.last_plus_one){ _throw(
Null_Exception);}* _temp4848;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4849= yyls; struct Cyc_Yyltype* _temp4851= _temp4849.curr +(
yylsp_offset - 7); if( _temp4851 < _temp4849.base? 1: _temp4851 >= _temp4849.last_plus_one){
_throw( Null_Exception);}* _temp4851;}).first_line,({ struct _tagged_ptr3
_temp4852= yyls; struct Cyc_Yyltype* _temp4854= _temp4852.curr + yylsp_offset;
if( _temp4854 < _temp4852.base? 1: _temp4854 >= _temp4852.last_plus_one){ _throw(
Null_Exception);}* _temp4854;}).last_line))};( struct _xenum_struct*) _temp4839;});
break; case 237: _LL4838: yyval=({ struct Cyc_Stmt_tok_struct* _temp4856=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4856=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_for_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4857= yyvs; struct _xenum_struct**
_temp4859= _temp4857.curr +( yyvsp_offset - 5); if( _temp4859 < _temp4857.base?
1: _temp4859 >= _temp4857.last_plus_one){ _throw( Null_Exception);}* _temp4859;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4860= yyvs; struct _xenum_struct**
_temp4862= _temp4860.curr +( yyvsp_offset - 3); if( _temp4862 < _temp4860.base?
1: _temp4862 >= _temp4860.last_plus_one){ _throw( Null_Exception);}* _temp4862;})),
Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4863=
yyvs; struct _xenum_struct** _temp4865= _temp4863.curr + yyvsp_offset; if(
_temp4865 < _temp4863.base? 1: _temp4865 >= _temp4863.last_plus_one){ _throw(
Null_Exception);}* _temp4865;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4866= yyls; struct Cyc_Yyltype* _temp4868= _temp4866.curr +(
yylsp_offset - 7); if( _temp4868 < _temp4866.base? 1: _temp4868 >= _temp4866.last_plus_one){
_throw( Null_Exception);}* _temp4868;}).first_line,({ struct _tagged_ptr3
_temp4869= yyls; struct Cyc_Yyltype* _temp4871= _temp4869.curr + yylsp_offset;
if( _temp4871 < _temp4869.base? 1: _temp4871 >= _temp4869.last_plus_one){ _throw(
Null_Exception);}* _temp4871;}).last_line))};( struct _xenum_struct*) _temp4856;});
break; case 238: _LL4855: yyval=({ struct Cyc_Stmt_tok_struct* _temp4873=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4873=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_for_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4874= yyvs; struct _xenum_struct**
_temp4876= _temp4874.curr +( yyvsp_offset - 6); if( _temp4876 < _temp4874.base?
1: _temp4876 >= _temp4874.last_plus_one){ _throw( Null_Exception);}* _temp4876;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4877= yyvs; struct _xenum_struct**
_temp4879= _temp4877.curr +( yyvsp_offset - 4); if( _temp4879 < _temp4877.base?
1: _temp4879 >= _temp4877.last_plus_one){ _throw( Null_Exception);}* _temp4879;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4880= yyvs; struct _xenum_struct**
_temp4882= _temp4880.curr +( yyvsp_offset - 2); if( _temp4882 < _temp4880.base?
1: _temp4882 >= _temp4880.last_plus_one){ _throw( Null_Exception);}* _temp4882;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4883= yyvs; struct _xenum_struct**
_temp4885= _temp4883.curr + yyvsp_offset; if( _temp4885 < _temp4883.base? 1:
_temp4885 >= _temp4883.last_plus_one){ _throw( Null_Exception);}* _temp4885;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4886= yyls; struct Cyc_Yyltype*
_temp4888= _temp4886.curr +( yylsp_offset - 8); if( _temp4888 < _temp4886.base?
1: _temp4888 >= _temp4886.last_plus_one){ _throw( Null_Exception);}* _temp4888;}).first_line,({
struct _tagged_ptr3 _temp4889= yyls; struct Cyc_Yyltype* _temp4891= _temp4889.curr
+ yylsp_offset; if( _temp4891 < _temp4889.base? 1: _temp4891 >= _temp4889.last_plus_one){
_throw( Null_Exception);}* _temp4891;}).last_line))};( struct _xenum_struct*)
_temp4873;}); break; case 239: _LL4872: { struct Cyc_List_List* decls= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp4893= yyvs; struct _xenum_struct** _temp4895= _temp4893.curr
+( yyvsp_offset - 3); if( _temp4895 < _temp4893.base? 1: _temp4895 >= _temp4893.last_plus_one){
_throw( Null_Exception);}* _temp4895;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp4896= yyvs; struct _xenum_struct** _temp4898= _temp4896.curr
+ yyvsp_offset; if( _temp4898 < _temp4896.base? 1: _temp4898 >= _temp4896.last_plus_one){
_throw( Null_Exception);}* _temp4898;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4899= yyls; struct Cyc_Yyltype* _temp4901= _temp4899.curr +(
yylsp_offset - 5); if( _temp4901 < _temp4899.base? 1: _temp4901 >= _temp4899.last_plus_one){
_throw( Null_Exception);}* _temp4901;}).first_line,({ struct _tagged_ptr3
_temp4902= yyls; struct Cyc_Yyltype* _temp4904= _temp4902.curr + yylsp_offset;
if( _temp4904 < _temp4902.base? 1: _temp4904 >= _temp4902.last_plus_one){ _throw(
Null_Exception);}* _temp4904;}).last_line)); yyval=({ struct Cyc_Stmt_tok_struct*
_temp4905=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4905=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Parse_flatten_declarations(
decls, s)};( struct _xenum_struct*) _temp4905;}); break;} case 240: _LL4892: {
struct Cyc_List_List* decls= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2
_temp4907= yyvs; struct _xenum_struct** _temp4909= _temp4907.curr +(
yyvsp_offset - 4); if( _temp4909 < _temp4907.base? 1: _temp4909 >= _temp4907.last_plus_one){
_throw( Null_Exception);}* _temp4909;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4910=
yyvs; struct _xenum_struct** _temp4912= _temp4910.curr +( yyvsp_offset - 3); if(
_temp4912 < _temp4910.base? 1: _temp4912 >= _temp4910.last_plus_one){ _throw(
Null_Exception);}* _temp4912;})), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp4913= yyvs; struct _xenum_struct** _temp4915= _temp4913.curr
+ yyvsp_offset; if( _temp4915 < _temp4913.base? 1: _temp4915 >= _temp4913.last_plus_one){
_throw( Null_Exception);}* _temp4915;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4916= yyls; struct Cyc_Yyltype* _temp4918= _temp4916.curr +(
yylsp_offset - 6); if( _temp4918 < _temp4916.base? 1: _temp4918 >= _temp4916.last_plus_one){
_throw( Null_Exception);}* _temp4918;}).first_line,({ struct _tagged_ptr3
_temp4919= yyls; struct Cyc_Yyltype* _temp4921= _temp4919.curr + yylsp_offset;
if( _temp4921 < _temp4919.base? 1: _temp4921 >= _temp4919.last_plus_one){ _throw(
Null_Exception);}* _temp4921;}).last_line)); yyval=({ struct Cyc_Stmt_tok_struct*
_temp4922=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4922=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Parse_flatten_declarations(
decls, s)};( struct _xenum_struct*) _temp4922;}); break;} case 241: _LL4906: {
struct Cyc_List_List* decls= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2
_temp4924= yyvs; struct _xenum_struct** _temp4926= _temp4924.curr +(
yyvsp_offset - 4); if( _temp4926 < _temp4924.base? 1: _temp4926 >= _temp4924.last_plus_one){
_throw( Null_Exception);}* _temp4926;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4927= yyvs; struct _xenum_struct** _temp4929= _temp4927.curr +(
yyvsp_offset - 2); if( _temp4929 < _temp4927.base? 1: _temp4929 >= _temp4927.last_plus_one){
_throw( Null_Exception);}* _temp4929;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp4930= yyvs; struct _xenum_struct** _temp4932= _temp4930.curr +
yyvsp_offset; if( _temp4932 < _temp4930.base? 1: _temp4932 >= _temp4930.last_plus_one){
_throw( Null_Exception);}* _temp4932;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4933= yyls; struct Cyc_Yyltype* _temp4935= _temp4933.curr +(
yylsp_offset - 6); if( _temp4935 < _temp4933.base? 1: _temp4935 >= _temp4933.last_plus_one){
_throw( Null_Exception);}* _temp4935;}).first_line,({ struct _tagged_ptr3
_temp4936= yyls; struct Cyc_Yyltype* _temp4938= _temp4936.curr + yylsp_offset;
if( _temp4938 < _temp4936.base? 1: _temp4938 >= _temp4936.last_plus_one){ _throw(
Null_Exception);}* _temp4938;}).last_line)); yyval=({ struct Cyc_Stmt_tok_struct*
_temp4939=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4939=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Parse_flatten_declarations(
decls, s)};( struct _xenum_struct*) _temp4939;}); break;} case 242: _LL4923: {
struct Cyc_List_List* decls= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2
_temp4941= yyvs; struct _xenum_struct** _temp4943= _temp4941.curr +(
yyvsp_offset - 5); if( _temp4943 < _temp4941.base? 1: _temp4943 >= _temp4941.last_plus_one){
_throw( Null_Exception);}* _temp4943;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4944=
yyvs; struct _xenum_struct** _temp4946= _temp4944.curr +( yyvsp_offset - 4); if(
_temp4946 < _temp4944.base? 1: _temp4946 >= _temp4944.last_plus_one){ _throw(
Null_Exception);}* _temp4946;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp4947= yyvs; struct _xenum_struct** _temp4949= _temp4947.curr +(
yyvsp_offset - 2); if( _temp4949 < _temp4947.base? 1: _temp4949 >= _temp4947.last_plus_one){
_throw( Null_Exception);}* _temp4949;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp4950= yyvs; struct _xenum_struct** _temp4952= _temp4950.curr +
yyvsp_offset; if( _temp4952 < _temp4950.base? 1: _temp4952 >= _temp4950.last_plus_one){
_throw( Null_Exception);}* _temp4952;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4953= yyls; struct Cyc_Yyltype* _temp4955= _temp4953.curr +(
yylsp_offset - 7); if( _temp4955 < _temp4953.base? 1: _temp4955 >= _temp4953.last_plus_one){
_throw( Null_Exception);}* _temp4955;}).first_line,({ struct _tagged_ptr3
_temp4956= yyls; struct Cyc_Yyltype* _temp4958= _temp4956.curr + yylsp_offset;
if( _temp4958 < _temp4956.base? 1: _temp4958 >= _temp4956.last_plus_one){ _throw(
Null_Exception);}* _temp4958;}).last_line)); yyval=({ struct Cyc_Stmt_tok_struct*
_temp4959=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4959=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Parse_flatten_declarations(
decls, s)};( struct _xenum_struct*) _temp4959;}); break;} case 243: _LL4940:
yyval=({ struct Cyc_Stmt_tok_struct* _temp4961=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));* _temp4961=( struct Cyc_Stmt_tok_struct){.tag=
Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_goto_stmt(({ struct _tagged_string* _temp4962=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp4962[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp4963= yyvs;
struct _xenum_struct** _temp4965= _temp4963.curr +( yyvsp_offset - 1); if(
_temp4965 < _temp4963.base? 1: _temp4965 >= _temp4963.last_plus_one){ _throw(
Null_Exception);}* _temp4965;})); _temp4962;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4966= yyls; struct Cyc_Yyltype* _temp4968= _temp4966.curr
+( yylsp_offset - 2); if( _temp4968 < _temp4966.base? 1: _temp4968 >= _temp4966.last_plus_one){
_throw( Null_Exception);}* _temp4968;}).first_line,({ struct _tagged_ptr3
_temp4969= yyls; struct Cyc_Yyltype* _temp4971= _temp4969.curr +( yylsp_offset -
1); if( _temp4971 < _temp4969.base? 1: _temp4971 >= _temp4969.last_plus_one){
_throw( Null_Exception);}* _temp4971;}).last_line))};( struct _xenum_struct*)
_temp4961;}); break; case 244: _LL4960: yyval=({ struct Cyc_Stmt_tok_struct*
_temp4973=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4973=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_continue_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4974= yyls; struct Cyc_Yyltype*
_temp4976= _temp4974.curr +( yylsp_offset - 1); if( _temp4976 < _temp4974.base?
1: _temp4976 >= _temp4974.last_plus_one){ _throw( Null_Exception);}* _temp4976;}).first_line,({
struct _tagged_ptr3 _temp4977= yyls; struct Cyc_Yyltype* _temp4979= _temp4977.curr
+( yylsp_offset - 1); if( _temp4979 < _temp4977.base? 1: _temp4979 >= _temp4977.last_plus_one){
_throw( Null_Exception);}* _temp4979;}).last_line))};( struct _xenum_struct*)
_temp4973;}); break; case 245: _LL4972: yyval=({ struct Cyc_Stmt_tok_struct*
_temp4981=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4981=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_break_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4982= yyls; struct Cyc_Yyltype*
_temp4984= _temp4982.curr +( yylsp_offset - 1); if( _temp4984 < _temp4982.base?
1: _temp4984 >= _temp4982.last_plus_one){ _throw( Null_Exception);}* _temp4984;}).first_line,({
struct _tagged_ptr3 _temp4985= yyls; struct Cyc_Yyltype* _temp4987= _temp4985.curr
+( yylsp_offset - 1); if( _temp4987 < _temp4985.base? 1: _temp4987 >= _temp4985.last_plus_one){
_throw( Null_Exception);}* _temp4987;}).last_line))};( struct _xenum_struct*)
_temp4981;}); break; case 246: _LL4980: yyval=({ struct Cyc_Stmt_tok_struct*
_temp4989=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4989=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_return_stmt(
0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4990= yyls; struct
Cyc_Yyltype* _temp4992= _temp4990.curr +( yylsp_offset - 1); if( _temp4992 <
_temp4990.base? 1: _temp4992 >= _temp4990.last_plus_one){ _throw( Null_Exception);}*
_temp4992;}).first_line,({ struct _tagged_ptr3 _temp4993= yyls; struct Cyc_Yyltype*
_temp4995= _temp4993.curr +( yylsp_offset - 1); if( _temp4995 < _temp4993.base?
1: _temp4995 >= _temp4993.last_plus_one){ _throw( Null_Exception);}* _temp4995;}).last_line))};(
struct _xenum_struct*) _temp4989;}); break; case 247: _LL4988: yyval=({ struct
Cyc_Stmt_tok_struct* _temp4997=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct));* _temp4997=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1=
Cyc_Absyn_return_stmt(( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4998= yyvs; struct _xenum_struct** _temp5000= _temp4998.curr +(
yyvsp_offset - 1); if( _temp5000 < _temp4998.base? 1: _temp5000 >= _temp4998.last_plus_one){
_throw( Null_Exception);}* _temp5000;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5001= yyls; struct Cyc_Yyltype* _temp5003= _temp5001.curr +(
yylsp_offset - 2); if( _temp5003 < _temp5001.base? 1: _temp5003 >= _temp5001.last_plus_one){
_throw( Null_Exception);}* _temp5003;}).first_line,({ struct _tagged_ptr3
_temp5004= yyls; struct Cyc_Yyltype* _temp5006= _temp5004.curr +( yylsp_offset -
1); if( _temp5006 < _temp5004.base? 1: _temp5006 >= _temp5004.last_plus_one){
_throw( Null_Exception);}* _temp5006;}).last_line))};( struct _xenum_struct*)
_temp4997;}); break; case 248: _LL4996: yyval=({ struct Cyc_Stmt_tok_struct*
_temp5008=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp5008=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_fallthru_stmt(
0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5009= yyls; struct
Cyc_Yyltype* _temp5011= _temp5009.curr +( yylsp_offset - 1); if( _temp5011 <
_temp5009.base? 1: _temp5011 >= _temp5009.last_plus_one){ _throw( Null_Exception);}*
_temp5011;}).first_line,({ struct _tagged_ptr3 _temp5012= yyls; struct Cyc_Yyltype*
_temp5014= _temp5012.curr +( yylsp_offset - 1); if( _temp5014 < _temp5012.base?
1: _temp5014 >= _temp5012.last_plus_one){ _throw( Null_Exception);}* _temp5014;}).last_line))};(
struct _xenum_struct*) _temp5008;}); break; case 249: _LL5007: yyval=({ struct
Cyc_Stmt_tok_struct* _temp5016=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct));* _temp5016=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1=
Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5017= yyls; struct Cyc_Yyltype* _temp5019= _temp5017.curr +( yylsp_offset -
3); if( _temp5019 < _temp5017.base? 1: _temp5019 >= _temp5017.last_plus_one){
_throw( Null_Exception);}* _temp5019;}).first_line,({ struct _tagged_ptr3
_temp5020= yyls; struct Cyc_Yyltype* _temp5022= _temp5020.curr +( yylsp_offset -
3); if( _temp5022 < _temp5020.base? 1: _temp5022 >= _temp5020.last_plus_one){
_throw( Null_Exception);}* _temp5022;}).last_line))};( struct _xenum_struct*)
_temp5016;}); break; case 250: _LL5015: yyval=({ struct Cyc_Stmt_tok_struct*
_temp5024=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp5024=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2 _temp5025= yyvs; struct
_xenum_struct** _temp5027= _temp5025.curr +( yyvsp_offset - 2); if( _temp5027 <
_temp5025.base? 1: _temp5027 >= _temp5025.last_plus_one){ _throw( Null_Exception);}*
_temp5027;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5028=
yyls; struct Cyc_Yyltype* _temp5030= _temp5028.curr +( yylsp_offset - 4); if(
_temp5030 < _temp5028.base? 1: _temp5030 >= _temp5028.last_plus_one){ _throw(
Null_Exception);}* _temp5030;}).first_line,({ struct _tagged_ptr3 _temp5031=
yyls; struct Cyc_Yyltype* _temp5033= _temp5031.curr +( yylsp_offset - 4); if(
_temp5033 < _temp5031.base? 1: _temp5033 >= _temp5031.last_plus_one){ _throw(
Null_Exception);}* _temp5033;}).last_line))};( struct _xenum_struct*) _temp5024;});
break; case 251: _LL5023: yyval=({ struct Cyc_Pattern_tok_struct* _temp5035=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp5035=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(
Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5036=
yyls; struct Cyc_Yyltype* _temp5038= _temp5036.curr + yylsp_offset; if(
_temp5038 < _temp5036.base? 1: _temp5038 >= _temp5036.last_plus_one){ _throw(
Null_Exception);}* _temp5038;}).first_line,({ struct _tagged_ptr3 _temp5039=
yyls; struct Cyc_Yyltype* _temp5041= _temp5039.curr + yylsp_offset; if(
_temp5041 < _temp5039.base? 1: _temp5041 >= _temp5039.last_plus_one){ _throw(
Null_Exception);}* _temp5041;}).last_line))};( struct _xenum_struct*) _temp5035;});
break; case 252: _LL5034: yyval=({ struct _tagged_ptr2 _temp5043= yyvs; struct
_xenum_struct** _temp5045= _temp5043.curr +( yyvsp_offset - 1); if( _temp5045 <
_temp5043.base? 1: _temp5045 >= _temp5043.last_plus_one){ _throw( Null_Exception);}*
_temp5045;}); break; case 253: _LL5042: yyval=({ struct Cyc_Pattern_tok_struct*
_temp5047=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp5047=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(({
struct Cyc_Absyn_Int_p_struct* _temp5048=( struct Cyc_Absyn_Int_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct)); _temp5048->tag= Cyc_Absyn_Int_p_tag;
_temp5048->f1=( void*)(* Cyc_yyget_Int_tok(({ struct _tagged_ptr2 _temp5049=
yyvs; struct _xenum_struct** _temp5051= _temp5049.curr + yyvsp_offset; if(
_temp5051 < _temp5049.base? 1: _temp5051 >= _temp5049.last_plus_one){ _throw(
Null_Exception);}* _temp5051;}))).f1; _temp5048->f2=(* Cyc_yyget_Int_tok(({
struct _tagged_ptr2 _temp5052= yyvs; struct _xenum_struct** _temp5054= _temp5052.curr
+ yyvsp_offset; if( _temp5054 < _temp5052.base? 1: _temp5054 >= _temp5052.last_plus_one){
_throw( Null_Exception);}* _temp5054;}))).f2;( void*) _temp5048;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5055= yyls; struct Cyc_Yyltype* _temp5057= _temp5055.curr
+ yylsp_offset; if( _temp5057 < _temp5055.base? 1: _temp5057 >= _temp5055.last_plus_one){
_throw( Null_Exception);}* _temp5057;}).first_line,({ struct _tagged_ptr3
_temp5058= yyls; struct Cyc_Yyltype* _temp5060= _temp5058.curr + yylsp_offset;
if( _temp5060 < _temp5058.base? 1: _temp5060 >= _temp5058.last_plus_one){ _throw(
Null_Exception);}* _temp5060;}).last_line))};( struct _xenum_struct*) _temp5047;});
break; case 254: _LL5046: yyval=({ struct Cyc_Pattern_tok_struct* _temp5062=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp5062=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(({
struct Cyc_Absyn_Int_p_struct* _temp5063=( struct Cyc_Absyn_Int_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct)); _temp5063->tag= Cyc_Absyn_Int_p_tag;
_temp5063->f1=( void*) Cyc_Absyn_Signed; _temp5063->f2= -(* Cyc_yyget_Int_tok(({
struct _tagged_ptr2 _temp5064= yyvs; struct _xenum_struct** _temp5066= _temp5064.curr
+ yyvsp_offset; if( _temp5066 < _temp5064.base? 1: _temp5066 >= _temp5064.last_plus_one){
_throw( Null_Exception);}* _temp5066;}))).f2;( void*) _temp5063;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5067= yyls; struct Cyc_Yyltype* _temp5069= _temp5067.curr
+( yylsp_offset - 1); if( _temp5069 < _temp5067.base? 1: _temp5069 >= _temp5067.last_plus_one){
_throw( Null_Exception);}* _temp5069;}).first_line,({ struct _tagged_ptr3
_temp5070= yyls; struct Cyc_Yyltype* _temp5072= _temp5070.curr + yylsp_offset;
if( _temp5072 < _temp5070.base? 1: _temp5072 >= _temp5070.last_plus_one){ _throw(
Null_Exception);}* _temp5072;}).last_line))};( struct _xenum_struct*) _temp5062;});
break; case 255: _LL5061: yyval=({ struct Cyc_Pattern_tok_struct* _temp5074=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp5074=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(({
struct Cyc_Absyn_Float_p_struct* _temp5075=( struct Cyc_Absyn_Float_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Float_p_struct)); _temp5075->tag= Cyc_Absyn_Float_p_tag;
_temp5075->f1= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp5076= yyvs;
struct _xenum_struct** _temp5078= _temp5076.curr + yyvsp_offset; if( _temp5078 <
_temp5076.base? 1: _temp5078 >= _temp5076.last_plus_one){ _throw( Null_Exception);}*
_temp5078;}));( void*) _temp5075;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5079= yyls; struct Cyc_Yyltype* _temp5081= _temp5079.curr +
yylsp_offset; if( _temp5081 < _temp5079.base? 1: _temp5081 >= _temp5079.last_plus_one){
_throw( Null_Exception);}* _temp5081;}).first_line,({ struct _tagged_ptr3
_temp5082= yyls; struct Cyc_Yyltype* _temp5084= _temp5082.curr + yylsp_offset;
if( _temp5084 < _temp5082.base? 1: _temp5084 >= _temp5082.last_plus_one){ _throw(
Null_Exception);}* _temp5084;}).last_line))};( struct _xenum_struct*) _temp5074;});
break; case 256: _LL5073: yyval=({ struct Cyc_Pattern_tok_struct* _temp5086=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp5086=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(({
struct Cyc_Absyn_Char_p_struct* _temp5087=( struct Cyc_Absyn_Char_p_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Absyn_Char_p_struct)); _temp5087->tag= Cyc_Absyn_Char_p_tag;
_temp5087->f1= Cyc_yyget_Char_tok(({ struct _tagged_ptr2 _temp5088= yyvs; struct
_xenum_struct** _temp5090= _temp5088.curr + yyvsp_offset; if( _temp5090 <
_temp5088.base? 1: _temp5090 >= _temp5088.last_plus_one){ _throw( Null_Exception);}*
_temp5090;}));( void*) _temp5087;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5091= yyls; struct Cyc_Yyltype* _temp5093= _temp5091.curr +
yylsp_offset; if( _temp5093 < _temp5091.base? 1: _temp5093 >= _temp5091.last_plus_one){
_throw( Null_Exception);}* _temp5093;}).first_line,({ struct _tagged_ptr3
_temp5094= yyls; struct Cyc_Yyltype* _temp5096= _temp5094.curr + yylsp_offset;
if( _temp5096 < _temp5094.base? 1: _temp5096 >= _temp5094.last_plus_one){ _throw(
Null_Exception);}* _temp5096;}).last_line))};( struct _xenum_struct*) _temp5086;});
break; case 257: _LL5085: yyval=({ struct Cyc_Pattern_tok_struct* _temp5098=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp5098=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(
Cyc_Absyn_Null_p, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5099=
yyls; struct Cyc_Yyltype* _temp5101= _temp5099.curr + yylsp_offset; if(
_temp5101 < _temp5099.base? 1: _temp5101 >= _temp5099.last_plus_one){ _throw(
Null_Exception);}* _temp5101;}).first_line,({ struct _tagged_ptr3 _temp5102=
yyls; struct Cyc_Yyltype* _temp5104= _temp5102.curr + yylsp_offset; if(
_temp5104 < _temp5102.base? 1: _temp5104 >= _temp5102.last_plus_one){ _throw(
Null_Exception);}* _temp5104;}).last_line))};( struct _xenum_struct*) _temp5098;});
break; case 258: _LL5097: yyval=({ struct Cyc_Pattern_tok_struct* _temp5106=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp5106=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(({
struct Cyc_Absyn_UnknownId_p_struct* _temp5107=( struct Cyc_Absyn_UnknownId_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_p_struct)); _temp5107->tag= Cyc_Absyn_UnknownId_p_tag;
_temp5107->f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp5108= yyvs;
struct _xenum_struct** _temp5110= _temp5108.curr + yyvsp_offset; if( _temp5110 <
_temp5108.base? 1: _temp5110 >= _temp5108.last_plus_one){ _throw( Null_Exception);}*
_temp5110;}));( void*) _temp5107;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5111= yyls; struct Cyc_Yyltype* _temp5113= _temp5111.curr +
yylsp_offset; if( _temp5113 < _temp5111.base? 1: _temp5113 >= _temp5111.last_plus_one){
_throw( Null_Exception);}* _temp5113;}).first_line,({ struct _tagged_ptr3
_temp5114= yyls; struct Cyc_Yyltype* _temp5116= _temp5114.curr + yylsp_offset;
if( _temp5116 < _temp5114.base? 1: _temp5116 >= _temp5114.last_plus_one){ _throw(
Null_Exception);}* _temp5116;}).last_line))};( struct _xenum_struct*) _temp5106;});
break; case 259: _LL5105: { struct Cyc_List_List* tvs=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5118= yyls; struct Cyc_Yyltype* _temp5120= _temp5118.curr
+( yylsp_offset - 3); if( _temp5120 < _temp5118.base? 1: _temp5120 >= _temp5118.last_plus_one){
_throw( Null_Exception);}* _temp5120;}).first_line,({ struct _tagged_ptr3
_temp5121= yyls; struct Cyc_Yyltype* _temp5123= _temp5121.curr +( yylsp_offset -
3); if( _temp5123 < _temp5121.base? 1: _temp5123 >= _temp5121.last_plus_one){
_throw( Null_Exception);}* _temp5123;}).last_line), Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp5124= yyvs; struct _xenum_struct** _temp5126= _temp5124.curr
+( yyvsp_offset - 3); if( _temp5126 < _temp5124.base? 1: _temp5126 >= _temp5124.last_plus_one){
_throw( Null_Exception);}* _temp5126;}))); yyval=({ struct Cyc_Pattern_tok_struct*
_temp5127=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp5127=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(({
struct Cyc_Absyn_UnknownCall_p_struct* _temp5128=( struct Cyc_Absyn_UnknownCall_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownCall_p_struct)); _temp5128->tag= Cyc_Absyn_UnknownCall_p_tag;
_temp5128->f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp5129= yyvs;
struct _xenum_struct** _temp5131= _temp5129.curr +( yyvsp_offset - 4); if(
_temp5131 < _temp5129.base? 1: _temp5131 >= _temp5129.last_plus_one){ _throw(
Null_Exception);}* _temp5131;})); _temp5128->f2= tvs; _temp5128->f3= Cyc_yyget_PatternList_tok(({
struct _tagged_ptr2 _temp5132= yyvs; struct _xenum_struct** _temp5134= _temp5132.curr
+( yyvsp_offset - 1); if( _temp5134 < _temp5132.base? 1: _temp5134 >= _temp5132.last_plus_one){
_throw( Null_Exception);}* _temp5134;}));( void*) _temp5128;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5135= yyls; struct Cyc_Yyltype* _temp5137= _temp5135.curr
+( yylsp_offset - 4); if( _temp5137 < _temp5135.base? 1: _temp5137 >= _temp5135.last_plus_one){
_throw( Null_Exception);}* _temp5137;}).first_line,({ struct _tagged_ptr3
_temp5138= yyls; struct Cyc_Yyltype* _temp5140= _temp5138.curr + yylsp_offset;
if( _temp5140 < _temp5138.base? 1: _temp5140 >= _temp5138.last_plus_one){ _throw(
Null_Exception);}* _temp5140;}).last_line))};( struct _xenum_struct*) _temp5127;});
break;} case 260: _LL5117: yyval=({ struct Cyc_Pattern_tok_struct* _temp5142=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp5142=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(({
struct Cyc_Absyn_Tuple_p_struct* _temp5143=( struct Cyc_Absyn_Tuple_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuple_p_struct)); _temp5143->tag= Cyc_Absyn_Tuple_p_tag;
_temp5143->f1= Cyc_yyget_PatternList_tok(({ struct _tagged_ptr2 _temp5144= yyvs;
struct _xenum_struct** _temp5146= _temp5144.curr +( yyvsp_offset - 1); if(
_temp5146 < _temp5144.base? 1: _temp5146 >= _temp5144.last_plus_one){ _throw(
Null_Exception);}* _temp5146;}));( void*) _temp5143;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5147= yyls; struct Cyc_Yyltype* _temp5149= _temp5147.curr
+( yylsp_offset - 3); if( _temp5149 < _temp5147.base? 1: _temp5149 >= _temp5147.last_plus_one){
_throw( Null_Exception);}* _temp5149;}).first_line,({ struct _tagged_ptr3
_temp5150= yyls; struct Cyc_Yyltype* _temp5152= _temp5150.curr + yylsp_offset;
if( _temp5152 < _temp5150.base? 1: _temp5152 >= _temp5150.last_plus_one){ _throw(
Null_Exception);}* _temp5152;}).last_line))};( struct _xenum_struct*) _temp5142;});
break; case 261: _LL5141: { struct Cyc_List_List* tvs=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5154= yyls; struct Cyc_Yyltype* _temp5156= _temp5154.curr
+( yylsp_offset - 2); if( _temp5156 < _temp5154.base? 1: _temp5156 >= _temp5154.last_plus_one){
_throw( Null_Exception);}* _temp5156;}).first_line,({ struct _tagged_ptr3
_temp5157= yyls; struct Cyc_Yyltype* _temp5159= _temp5157.curr +( yylsp_offset -
2); if( _temp5159 < _temp5157.base? 1: _temp5159 >= _temp5157.last_plus_one){
_throw( Null_Exception);}* _temp5159;}).last_line), Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp5160= yyvs; struct _xenum_struct** _temp5162= _temp5160.curr
+( yyvsp_offset - 2); if( _temp5162 < _temp5160.base? 1: _temp5162 >= _temp5160.last_plus_one){
_throw( Null_Exception);}* _temp5162;}))); yyval=({ struct Cyc_Pattern_tok_struct*
_temp5163=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp5163=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(({
struct Cyc_Absyn_UnknownFields_p_struct* _temp5164=( struct Cyc_Absyn_UnknownFields_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownFields_p_struct)); _temp5164->tag=
Cyc_Absyn_UnknownFields_p_tag; _temp5164->f1= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp5165= yyvs; struct _xenum_struct** _temp5167= _temp5165.curr +(
yyvsp_offset - 3); if( _temp5167 < _temp5165.base? 1: _temp5167 >= _temp5165.last_plus_one){
_throw( Null_Exception);}* _temp5167;})); _temp5164->f2= tvs; _temp5164->f3= 0;(
void*) _temp5164;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5168= yyls; struct Cyc_Yyltype* _temp5170= _temp5168.curr +( yylsp_offset -
3); if( _temp5170 < _temp5168.base? 1: _temp5170 >= _temp5168.last_plus_one){
_throw( Null_Exception);}* _temp5170;}).first_line,({ struct _tagged_ptr3
_temp5171= yyls; struct Cyc_Yyltype* _temp5173= _temp5171.curr + yylsp_offset;
if( _temp5173 < _temp5171.base? 1: _temp5173 >= _temp5171.last_plus_one){ _throw(
Null_Exception);}* _temp5173;}).last_line))};( struct _xenum_struct*) _temp5163;});
break;} case 262: _LL5153: { struct Cyc_List_List* tvs=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5175= yyls; struct Cyc_Yyltype* _temp5177= _temp5175.curr
+( yylsp_offset - 3); if( _temp5177 < _temp5175.base? 1: _temp5177 >= _temp5175.last_plus_one){
_throw( Null_Exception);}* _temp5177;}).first_line,({ struct _tagged_ptr3
_temp5178= yyls; struct Cyc_Yyltype* _temp5180= _temp5178.curr +( yylsp_offset -
3); if( _temp5180 < _temp5178.base? 1: _temp5180 >= _temp5178.last_plus_one){
_throw( Null_Exception);}* _temp5180;}).last_line), Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp5181= yyvs; struct _xenum_struct** _temp5183= _temp5181.curr
+( yyvsp_offset - 3); if( _temp5183 < _temp5181.base? 1: _temp5183 >= _temp5181.last_plus_one){
_throw( Null_Exception);}* _temp5183;}))); yyval=({ struct Cyc_Pattern_tok_struct*
_temp5184=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp5184=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(({
struct Cyc_Absyn_UnknownFields_p_struct* _temp5185=( struct Cyc_Absyn_UnknownFields_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownFields_p_struct)); _temp5185->tag=
Cyc_Absyn_UnknownFields_p_tag; _temp5185->f1= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp5186= yyvs; struct _xenum_struct** _temp5188= _temp5186.curr +(
yyvsp_offset - 4); if( _temp5188 < _temp5186.base? 1: _temp5188 >= _temp5186.last_plus_one){
_throw( Null_Exception);}* _temp5188;})); _temp5185->f2= tvs; _temp5185->f3= Cyc_yyget_FieldPatternList_tok(({
struct _tagged_ptr2 _temp5189= yyvs; struct _xenum_struct** _temp5191= _temp5189.curr
+( yyvsp_offset - 1); if( _temp5191 < _temp5189.base? 1: _temp5191 >= _temp5189.last_plus_one){
_throw( Null_Exception);}* _temp5191;}));( void*) _temp5185;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5192= yyls; struct Cyc_Yyltype* _temp5194= _temp5192.curr
+( yylsp_offset - 4); if( _temp5194 < _temp5192.base? 1: _temp5194 >= _temp5192.last_plus_one){
_throw( Null_Exception);}* _temp5194;}).first_line,({ struct _tagged_ptr3
_temp5195= yyls; struct Cyc_Yyltype* _temp5197= _temp5195.curr + yylsp_offset;
if( _temp5197 < _temp5195.base? 1: _temp5197 >= _temp5195.last_plus_one){ _throw(
Null_Exception);}* _temp5197;}).last_line))};( struct _xenum_struct*) _temp5184;});
break;} case 263: _LL5174: yyval=({ struct Cyc_Pattern_tok_struct* _temp5199=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp5199=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(({
struct Cyc_Absyn_Pointer_p_struct* _temp5200=( struct Cyc_Absyn_Pointer_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_p_struct)); _temp5200->tag= Cyc_Absyn_Pointer_p_tag;
_temp5200->f1= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp5201= yyvs;
struct _xenum_struct** _temp5203= _temp5201.curr + yyvsp_offset; if( _temp5203 <
_temp5201.base? 1: _temp5203 >= _temp5201.last_plus_one){ _throw( Null_Exception);}*
_temp5203;}));( void*) _temp5200;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5204= yyls; struct Cyc_Yyltype* _temp5206= _temp5204.curr +(
yylsp_offset - 1); if( _temp5206 < _temp5204.base? 1: _temp5206 >= _temp5204.last_plus_one){
_throw( Null_Exception);}* _temp5206;}).first_line,({ struct _tagged_ptr3
_temp5207= yyls; struct Cyc_Yyltype* _temp5209= _temp5207.curr + yylsp_offset;
if( _temp5209 < _temp5207.base? 1: _temp5209 >= _temp5207.last_plus_one){ _throw(
Null_Exception);}* _temp5209;}).last_line))};( struct _xenum_struct*) _temp5199;});
break; case 264: _LL5198: yyval=({ struct Cyc_Pattern_tok_struct* _temp5211=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp5211=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(({
struct Cyc_Absyn_Reference_p_struct* _temp5212=( struct Cyc_Absyn_Reference_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Reference_p_struct)); _temp5212->tag= Cyc_Absyn_Reference_p_tag;
_temp5212->f1= Cyc_Absyn_new_vardecl(({ struct _tuple1* _temp5213=( struct
_tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp5213->f1= Cyc_Absyn_Loc_n;
_temp5213->f2=({ struct _tagged_string* _temp5214=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp5214[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp5215= yyvs; struct _xenum_struct** _temp5217= _temp5215.curr
+ yyvsp_offset; if( _temp5217 < _temp5215.base? 1: _temp5217 >= _temp5215.last_plus_one){
_throw( Null_Exception);}* _temp5217;})); _temp5214;}); _temp5213;}), Cyc_Absyn_VoidType,
0);( void*) _temp5212;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5218= yyls; struct Cyc_Yyltype* _temp5220= _temp5218.curr +( yylsp_offset -
1); if( _temp5220 < _temp5218.base? 1: _temp5220 >= _temp5218.last_plus_one){
_throw( Null_Exception);}* _temp5220;}).first_line,({ struct _tagged_ptr3
_temp5221= yyls; struct Cyc_Yyltype* _temp5223= _temp5221.curr + yylsp_offset;
if( _temp5223 < _temp5221.base? 1: _temp5223 >= _temp5221.last_plus_one){ _throw(
Null_Exception);}* _temp5223;}).last_line))};( struct _xenum_struct*) _temp5211;});
break; case 265: _LL5210: yyval=({ struct Cyc_PatternList_tok_struct* _temp5225=(
struct Cyc_PatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));*
_temp5225=( struct Cyc_PatternList_tok_struct){.tag= Cyc_PatternList_tok_tag,.f1=
0};( struct _xenum_struct*) _temp5225;}); break; case 266: _LL5224: yyval=({
struct Cyc_PatternList_tok_struct* _temp5227=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));* _temp5227=( struct Cyc_PatternList_tok_struct){.tag=
Cyc_PatternList_tok_tag,.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( Cyc_yyget_PatternList_tok(({ struct _tagged_ptr2 _temp5228=
yyvs; struct _xenum_struct** _temp5230= _temp5228.curr + yyvsp_offset; if(
_temp5230 < _temp5228.base? 1: _temp5230 >= _temp5228.last_plus_one){ _throw(
Null_Exception);}* _temp5230;})))};( struct _xenum_struct*) _temp5227;}); break;
case 267: _LL5226: yyval=({ struct Cyc_PatternList_tok_struct* _temp5232=(
struct Cyc_PatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));*
_temp5232=( struct Cyc_PatternList_tok_struct){.tag= Cyc_PatternList_tok_tag,.f1=({
struct Cyc_List_List* _temp5233=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp5233->hd=( void*) Cyc_yyget_Pattern_tok(({ struct
_tagged_ptr2 _temp5234= yyvs; struct _xenum_struct** _temp5236= _temp5234.curr +
yyvsp_offset; if( _temp5236 < _temp5234.base? 1: _temp5236 >= _temp5234.last_plus_one){
_throw( Null_Exception);}* _temp5236;})); _temp5233->tl= 0; _temp5233;})};(
struct _xenum_struct*) _temp5232;}); break; case 268: _LL5231: yyval=({ struct
Cyc_PatternList_tok_struct* _temp5238=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));* _temp5238=( struct Cyc_PatternList_tok_struct){.tag=
Cyc_PatternList_tok_tag,.f1=({ struct Cyc_List_List* _temp5239=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5239->hd=( void*) Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp5243= yyvs; struct _xenum_struct** _temp5245= _temp5243.curr
+ yyvsp_offset; if( _temp5245 < _temp5243.base? 1: _temp5245 >= _temp5243.last_plus_one){
_throw( Null_Exception);}* _temp5245;})); _temp5239->tl= Cyc_yyget_PatternList_tok(({
struct _tagged_ptr2 _temp5240= yyvs; struct _xenum_struct** _temp5242= _temp5240.curr
+( yyvsp_offset - 2); if( _temp5242 < _temp5240.base? 1: _temp5242 >= _temp5240.last_plus_one){
_throw( Null_Exception);}* _temp5242;})); _temp5239;})};( struct _xenum_struct*)
_temp5238;}); break; case 269: _LL5237: yyval=({ struct Cyc_FieldPattern_tok_struct*
_temp5247=( struct Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));*
_temp5247=( struct Cyc_FieldPattern_tok_struct){.tag= Cyc_FieldPattern_tok_tag,.f1=({
struct _tuple15* _temp5248=( struct _tuple15*) GC_malloc( sizeof( struct
_tuple15)); _temp5248->f1= 0; _temp5248->f2= Cyc_yyget_Pattern_tok(({ struct
_tagged_ptr2 _temp5249= yyvs; struct _xenum_struct** _temp5251= _temp5249.curr +
yyvsp_offset; if( _temp5251 < _temp5249.base? 1: _temp5251 >= _temp5249.last_plus_one){
_throw( Null_Exception);}* _temp5251;})); _temp5248;})};( struct _xenum_struct*)
_temp5247;}); break; case 270: _LL5246: yyval=({ struct Cyc_FieldPattern_tok_struct*
_temp5253=( struct Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));*
_temp5253=( struct Cyc_FieldPattern_tok_struct){.tag= Cyc_FieldPattern_tok_tag,.f1=({
struct _tuple15* _temp5254=( struct _tuple15*) GC_malloc( sizeof( struct
_tuple15)); _temp5254->f1= Cyc_yyget_DesignatorList_tok(({ struct _tagged_ptr2
_temp5258= yyvs; struct _xenum_struct** _temp5260= _temp5258.curr +(
yyvsp_offset - 1); if( _temp5260 < _temp5258.base? 1: _temp5260 >= _temp5258.last_plus_one){
_throw( Null_Exception);}* _temp5260;})); _temp5254->f2= Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp5255= yyvs; struct _xenum_struct** _temp5257= _temp5255.curr
+ yyvsp_offset; if( _temp5257 < _temp5255.base? 1: _temp5257 >= _temp5255.last_plus_one){
_throw( Null_Exception);}* _temp5257;})); _temp5254;})};( struct _xenum_struct*)
_temp5253;}); break; case 271: _LL5252: yyval=({ struct Cyc_FieldPatternList_tok_struct*
_temp5262=( struct Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct
Cyc_FieldPatternList_tok_struct));* _temp5262=( struct Cyc_FieldPatternList_tok_struct){.tag=
Cyc_FieldPatternList_tok_tag,.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_FieldPatternList_tok(({ struct _tagged_ptr2
_temp5263= yyvs; struct _xenum_struct** _temp5265= _temp5263.curr + yyvsp_offset;
if( _temp5265 < _temp5263.base? 1: _temp5265 >= _temp5263.last_plus_one){ _throw(
Null_Exception);}* _temp5265;})))};( struct _xenum_struct*) _temp5262;}); break;
case 272: _LL5261: yyval=({ struct Cyc_FieldPatternList_tok_struct* _temp5267=(
struct Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct));*
_temp5267=( struct Cyc_FieldPatternList_tok_struct){.tag= Cyc_FieldPatternList_tok_tag,.f1=({
struct Cyc_List_List* _temp5268=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp5268->hd=( void*) Cyc_yyget_FieldPattern_tok(({
struct _tagged_ptr2 _temp5269= yyvs; struct _xenum_struct** _temp5271= _temp5269.curr
+ yyvsp_offset; if( _temp5271 < _temp5269.base? 1: _temp5271 >= _temp5269.last_plus_one){
_throw( Null_Exception);}* _temp5271;})); _temp5268->tl= 0; _temp5268;})};(
struct _xenum_struct*) _temp5267;}); break; case 273: _LL5266: yyval=({ struct
Cyc_FieldPatternList_tok_struct* _temp5273=( struct Cyc_FieldPatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct));* _temp5273=( struct
Cyc_FieldPatternList_tok_struct){.tag= Cyc_FieldPatternList_tok_tag,.f1=({
struct Cyc_List_List* _temp5274=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp5274->hd=( void*) Cyc_yyget_FieldPattern_tok(({
struct _tagged_ptr2 _temp5278= yyvs; struct _xenum_struct** _temp5280= _temp5278.curr
+ yyvsp_offset; if( _temp5280 < _temp5278.base? 1: _temp5280 >= _temp5278.last_plus_one){
_throw( Null_Exception);}* _temp5280;})); _temp5274->tl= Cyc_yyget_FieldPatternList_tok(({
struct _tagged_ptr2 _temp5275= yyvs; struct _xenum_struct** _temp5277= _temp5275.curr
+( yyvsp_offset - 2); if( _temp5277 < _temp5275.base? 1: _temp5277 >= _temp5275.last_plus_one){
_throw( Null_Exception);}* _temp5277;})); _temp5274;})};( struct _xenum_struct*)
_temp5273;}); break; case 274: _LL5272: yyval=({ struct _tagged_ptr2 _temp5282=
yyvs; struct _xenum_struct** _temp5284= _temp5282.curr + yyvsp_offset; if(
_temp5284 < _temp5282.base? 1: _temp5284 >= _temp5282.last_plus_one){ _throw(
Null_Exception);}* _temp5284;}); break; case 275: _LL5281: yyval=({ struct Cyc_Exp_tok_struct*
_temp5286=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5286=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_seq_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5287= yyvs; struct _xenum_struct**
_temp5289= _temp5287.curr +( yyvsp_offset - 2); if( _temp5289 < _temp5287.base?
1: _temp5289 >= _temp5287.last_plus_one){ _throw( Null_Exception);}* _temp5289;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5290= yyvs; struct _xenum_struct**
_temp5292= _temp5290.curr + yyvsp_offset; if( _temp5292 < _temp5290.base? 1:
_temp5292 >= _temp5290.last_plus_one){ _throw( Null_Exception);}* _temp5292;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5293= yyls; struct Cyc_Yyltype*
_temp5295= _temp5293.curr +( yylsp_offset - 2); if( _temp5295 < _temp5293.base?
1: _temp5295 >= _temp5293.last_plus_one){ _throw( Null_Exception);}* _temp5295;}).first_line,({
struct _tagged_ptr3 _temp5296= yyls; struct Cyc_Yyltype* _temp5298= _temp5296.curr
+ yylsp_offset; if( _temp5298 < _temp5296.base? 1: _temp5298 >= _temp5296.last_plus_one){
_throw( Null_Exception);}* _temp5298;}).last_line))};( struct _xenum_struct*)
_temp5286;}); break; case 276: _LL5285: yyval=({ struct _tagged_ptr2 _temp5300=
yyvs; struct _xenum_struct** _temp5302= _temp5300.curr + yyvsp_offset; if(
_temp5302 < _temp5300.base? 1: _temp5302 >= _temp5300.last_plus_one){ _throw(
Null_Exception);}* _temp5302;}); break; case 277: _LL5299: yyval=({ struct Cyc_Exp_tok_struct*
_temp5304=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5304=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_assignop_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5305= yyvs; struct _xenum_struct**
_temp5307= _temp5305.curr +( yyvsp_offset - 2); if( _temp5307 < _temp5305.base?
1: _temp5307 >= _temp5305.last_plus_one){ _throw( Null_Exception);}* _temp5307;})),
Cyc_yyget_Primopopt_tok(({ struct _tagged_ptr2 _temp5308= yyvs; struct
_xenum_struct** _temp5310= _temp5308.curr +( yyvsp_offset - 1); if( _temp5310 <
_temp5308.base? 1: _temp5310 >= _temp5308.last_plus_one){ _throw( Null_Exception);}*
_temp5310;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5311= yyvs; struct
_xenum_struct** _temp5313= _temp5311.curr + yyvsp_offset; if( _temp5313 <
_temp5311.base? 1: _temp5313 >= _temp5311.last_plus_one){ _throw( Null_Exception);}*
_temp5313;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5314=
yyls; struct Cyc_Yyltype* _temp5316= _temp5314.curr +( yylsp_offset - 2); if(
_temp5316 < _temp5314.base? 1: _temp5316 >= _temp5314.last_plus_one){ _throw(
Null_Exception);}* _temp5316;}).first_line,({ struct _tagged_ptr3 _temp5317=
yyls; struct Cyc_Yyltype* _temp5319= _temp5317.curr + yylsp_offset; if(
_temp5319 < _temp5317.base? 1: _temp5319 >= _temp5317.last_plus_one){ _throw(
Null_Exception);}* _temp5319;}).last_line))};( struct _xenum_struct*) _temp5304;});
break; case 278: _LL5303: yyval=({ struct Cyc_Primopopt_tok_struct* _temp5321=(
struct Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));*
_temp5321=( struct Cyc_Primopopt_tok_struct){.tag= Cyc_Primopopt_tok_tag,.f1= 0};(
struct _xenum_struct*) _temp5321;}); break; case 279: _LL5320: yyval=({ struct
Cyc_Primopopt_tok_struct* _temp5323=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));* _temp5323=( struct Cyc_Primopopt_tok_struct){.tag=
Cyc_Primopopt_tok_tag,.f1=({ struct Cyc_Core_Opt* _temp5324=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp5324->v=( void*) Cyc_Absyn_Times;
_temp5324;})};( struct _xenum_struct*) _temp5323;}); break; case 280: _LL5322:
yyval=({ struct Cyc_Primopopt_tok_struct* _temp5326=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));* _temp5326=( struct Cyc_Primopopt_tok_struct){.tag=
Cyc_Primopopt_tok_tag,.f1=({ struct Cyc_Core_Opt* _temp5327=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp5327->v=( void*) Cyc_Absyn_Div;
_temp5327;})};( struct _xenum_struct*) _temp5326;}); break; case 281: _LL5325:
yyval=({ struct Cyc_Primopopt_tok_struct* _temp5329=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));* _temp5329=( struct Cyc_Primopopt_tok_struct){.tag=
Cyc_Primopopt_tok_tag,.f1=({ struct Cyc_Core_Opt* _temp5330=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp5330->v=( void*) Cyc_Absyn_Mod;
_temp5330;})};( struct _xenum_struct*) _temp5329;}); break; case 282: _LL5328:
yyval=({ struct Cyc_Primopopt_tok_struct* _temp5332=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));* _temp5332=( struct Cyc_Primopopt_tok_struct){.tag=
Cyc_Primopopt_tok_tag,.f1=({ struct Cyc_Core_Opt* _temp5333=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp5333->v=( void*) Cyc_Absyn_Plus;
_temp5333;})};( struct _xenum_struct*) _temp5332;}); break; case 283: _LL5331:
yyval=({ struct Cyc_Primopopt_tok_struct* _temp5335=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));* _temp5335=( struct Cyc_Primopopt_tok_struct){.tag=
Cyc_Primopopt_tok_tag,.f1=({ struct Cyc_Core_Opt* _temp5336=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp5336->v=( void*) Cyc_Absyn_Minus;
_temp5336;})};( struct _xenum_struct*) _temp5335;}); break; case 284: _LL5334:
yyval=({ struct Cyc_Primopopt_tok_struct* _temp5338=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));* _temp5338=( struct Cyc_Primopopt_tok_struct){.tag=
Cyc_Primopopt_tok_tag,.f1=({ struct Cyc_Core_Opt* _temp5339=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp5339->v=( void*) Cyc_Absyn_Bitlshift;
_temp5339;})};( struct _xenum_struct*) _temp5338;}); break; case 285: _LL5337:
yyval=({ struct Cyc_Primopopt_tok_struct* _temp5341=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));* _temp5341=( struct Cyc_Primopopt_tok_struct){.tag=
Cyc_Primopopt_tok_tag,.f1=({ struct Cyc_Core_Opt* _temp5342=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp5342->v=( void*) Cyc_Absyn_Bitlrshift;
_temp5342;})};( struct _xenum_struct*) _temp5341;}); break; case 286: _LL5340:
yyval=({ struct Cyc_Primopopt_tok_struct* _temp5344=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));* _temp5344=( struct Cyc_Primopopt_tok_struct){.tag=
Cyc_Primopopt_tok_tag,.f1=({ struct Cyc_Core_Opt* _temp5345=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp5345->v=( void*) Cyc_Absyn_Bitand;
_temp5345;})};( struct _xenum_struct*) _temp5344;}); break; case 287: _LL5343:
yyval=({ struct Cyc_Primopopt_tok_struct* _temp5347=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));* _temp5347=( struct Cyc_Primopopt_tok_struct){.tag=
Cyc_Primopopt_tok_tag,.f1=({ struct Cyc_Core_Opt* _temp5348=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp5348->v=( void*) Cyc_Absyn_Bitxor;
_temp5348;})};( struct _xenum_struct*) _temp5347;}); break; case 288: _LL5346:
yyval=({ struct Cyc_Primopopt_tok_struct* _temp5350=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));* _temp5350=( struct Cyc_Primopopt_tok_struct){.tag=
Cyc_Primopopt_tok_tag,.f1=({ struct Cyc_Core_Opt* _temp5351=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp5351->v=( void*) Cyc_Absyn_Bitor;
_temp5351;})};( struct _xenum_struct*) _temp5350;}); break; case 289: _LL5349:
yyval=({ struct _tagged_ptr2 _temp5353= yyvs; struct _xenum_struct** _temp5355=
_temp5353.curr + yyvsp_offset; if( _temp5355 < _temp5353.base? 1: _temp5355 >=
_temp5353.last_plus_one){ _throw( Null_Exception);}* _temp5355;}); break; case
290: _LL5352: yyval=({ struct Cyc_Exp_tok_struct* _temp5357=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp5357=( struct Cyc_Exp_tok_struct){.tag=
Cyc_Exp_tok_tag,.f1= Cyc_Absyn_conditional_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5358= yyvs; struct _xenum_struct** _temp5360= _temp5358.curr +(
yyvsp_offset - 4); if( _temp5360 < _temp5358.base? 1: _temp5360 >= _temp5358.last_plus_one){
_throw( Null_Exception);}* _temp5360;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5361= yyvs; struct _xenum_struct** _temp5363= _temp5361.curr +(
yyvsp_offset - 2); if( _temp5363 < _temp5361.base? 1: _temp5363 >= _temp5361.last_plus_one){
_throw( Null_Exception);}* _temp5363;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5364= yyvs; struct _xenum_struct** _temp5366= _temp5364.curr +
yyvsp_offset; if( _temp5366 < _temp5364.base? 1: _temp5366 >= _temp5364.last_plus_one){
_throw( Null_Exception);}* _temp5366;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5367= yyls; struct Cyc_Yyltype* _temp5369= _temp5367.curr +(
yylsp_offset - 4); if( _temp5369 < _temp5367.base? 1: _temp5369 >= _temp5367.last_plus_one){
_throw( Null_Exception);}* _temp5369;}).first_line,({ struct _tagged_ptr3
_temp5370= yyls; struct Cyc_Yyltype* _temp5372= _temp5370.curr + yylsp_offset;
if( _temp5372 < _temp5370.base? 1: _temp5372 >= _temp5370.last_plus_one){ _throw(
Null_Exception);}* _temp5372;}).last_line))};( struct _xenum_struct*) _temp5357;});
break; case 291: _LL5356: yyval=({ struct _tagged_ptr2 _temp5374= yyvs; struct
_xenum_struct** _temp5376= _temp5374.curr + yyvsp_offset; if( _temp5376 <
_temp5374.base? 1: _temp5376 >= _temp5374.last_plus_one){ _throw( Null_Exception);}*
_temp5376;}); break; case 292: _LL5373: yyval=({ struct _tagged_ptr2 _temp5378=
yyvs; struct _xenum_struct** _temp5380= _temp5378.curr + yyvsp_offset; if(
_temp5380 < _temp5378.base? 1: _temp5380 >= _temp5378.last_plus_one){ _throw(
Null_Exception);}* _temp5380;}); break; case 293: _LL5377: yyval=({ struct Cyc_Exp_tok_struct*
_temp5382=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5382=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_or_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5383= yyvs; struct _xenum_struct**
_temp5385= _temp5383.curr +( yyvsp_offset - 2); if( _temp5385 < _temp5383.base?
1: _temp5385 >= _temp5383.last_plus_one){ _throw( Null_Exception);}* _temp5385;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5386= yyvs; struct _xenum_struct**
_temp5388= _temp5386.curr + yyvsp_offset; if( _temp5388 < _temp5386.base? 1:
_temp5388 >= _temp5386.last_plus_one){ _throw( Null_Exception);}* _temp5388;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5389= yyls; struct Cyc_Yyltype*
_temp5391= _temp5389.curr +( yylsp_offset - 2); if( _temp5391 < _temp5389.base?
1: _temp5391 >= _temp5389.last_plus_one){ _throw( Null_Exception);}* _temp5391;}).first_line,({
struct _tagged_ptr3 _temp5392= yyls; struct Cyc_Yyltype* _temp5394= _temp5392.curr
+ yylsp_offset; if( _temp5394 < _temp5392.base? 1: _temp5394 >= _temp5392.last_plus_one){
_throw( Null_Exception);}* _temp5394;}).last_line))};( struct _xenum_struct*)
_temp5382;}); break; case 294: _LL5381: yyval=({ struct _tagged_ptr2 _temp5396=
yyvs; struct _xenum_struct** _temp5398= _temp5396.curr + yyvsp_offset; if(
_temp5398 < _temp5396.base? 1: _temp5398 >= _temp5396.last_plus_one){ _throw(
Null_Exception);}* _temp5398;}); break; case 295: _LL5395: yyval=({ struct Cyc_Exp_tok_struct*
_temp5400=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5400=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_and_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5401= yyvs; struct _xenum_struct**
_temp5403= _temp5401.curr +( yyvsp_offset - 2); if( _temp5403 < _temp5401.base?
1: _temp5403 >= _temp5401.last_plus_one){ _throw( Null_Exception);}* _temp5403;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5404= yyvs; struct _xenum_struct**
_temp5406= _temp5404.curr + yyvsp_offset; if( _temp5406 < _temp5404.base? 1:
_temp5406 >= _temp5404.last_plus_one){ _throw( Null_Exception);}* _temp5406;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5407= yyls; struct Cyc_Yyltype*
_temp5409= _temp5407.curr +( yylsp_offset - 2); if( _temp5409 < _temp5407.base?
1: _temp5409 >= _temp5407.last_plus_one){ _throw( Null_Exception);}* _temp5409;}).first_line,({
struct _tagged_ptr3 _temp5410= yyls; struct Cyc_Yyltype* _temp5412= _temp5410.curr
+ yylsp_offset; if( _temp5412 < _temp5410.base? 1: _temp5412 >= _temp5410.last_plus_one){
_throw( Null_Exception);}* _temp5412;}).last_line))};( struct _xenum_struct*)
_temp5400;}); break; case 296: _LL5399: yyval=({ struct _tagged_ptr2 _temp5414=
yyvs; struct _xenum_struct** _temp5416= _temp5414.curr + yyvsp_offset; if(
_temp5416 < _temp5414.base? 1: _temp5416 >= _temp5414.last_plus_one){ _throw(
Null_Exception);}* _temp5416;}); break; case 297: _LL5413: yyval=({ struct Cyc_Exp_tok_struct*
_temp5418=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5418=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_prim2_exp(
Cyc_Absyn_Bitor, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5419= yyvs;
struct _xenum_struct** _temp5421= _temp5419.curr +( yyvsp_offset - 2); if(
_temp5421 < _temp5419.base? 1: _temp5421 >= _temp5419.last_plus_one){ _throw(
Null_Exception);}* _temp5421;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5422= yyvs; struct _xenum_struct** _temp5424= _temp5422.curr + yyvsp_offset;
if( _temp5424 < _temp5422.base? 1: _temp5424 >= _temp5422.last_plus_one){ _throw(
Null_Exception);}* _temp5424;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5425= yyls; struct Cyc_Yyltype* _temp5427= _temp5425.curr +(
yylsp_offset - 2); if( _temp5427 < _temp5425.base? 1: _temp5427 >= _temp5425.last_plus_one){
_throw( Null_Exception);}* _temp5427;}).first_line,({ struct _tagged_ptr3
_temp5428= yyls; struct Cyc_Yyltype* _temp5430= _temp5428.curr + yylsp_offset;
if( _temp5430 < _temp5428.base? 1: _temp5430 >= _temp5428.last_plus_one){ _throw(
Null_Exception);}* _temp5430;}).last_line))};( struct _xenum_struct*) _temp5418;});
break; case 298: _LL5417: yyval=({ struct _tagged_ptr2 _temp5432= yyvs; struct
_xenum_struct** _temp5434= _temp5432.curr + yyvsp_offset; if( _temp5434 <
_temp5432.base? 1: _temp5434 >= _temp5432.last_plus_one){ _throw( Null_Exception);}*
_temp5434;}); break; case 299: _LL5431: yyval=({ struct Cyc_Exp_tok_struct*
_temp5436=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5436=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_prim2_exp(
Cyc_Absyn_Bitxor, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5437= yyvs;
struct _xenum_struct** _temp5439= _temp5437.curr +( yyvsp_offset - 2); if(
_temp5439 < _temp5437.base? 1: _temp5439 >= _temp5437.last_plus_one){ _throw(
Null_Exception);}* _temp5439;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5440= yyvs; struct _xenum_struct** _temp5442= _temp5440.curr + yyvsp_offset;
if( _temp5442 < _temp5440.base? 1: _temp5442 >= _temp5440.last_plus_one){ _throw(
Null_Exception);}* _temp5442;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5443= yyls; struct Cyc_Yyltype* _temp5445= _temp5443.curr +(
yylsp_offset - 2); if( _temp5445 < _temp5443.base? 1: _temp5445 >= _temp5443.last_plus_one){
_throw( Null_Exception);}* _temp5445;}).first_line,({ struct _tagged_ptr3
_temp5446= yyls; struct Cyc_Yyltype* _temp5448= _temp5446.curr + yylsp_offset;
if( _temp5448 < _temp5446.base? 1: _temp5448 >= _temp5446.last_plus_one){ _throw(
Null_Exception);}* _temp5448;}).last_line))};( struct _xenum_struct*) _temp5436;});
break; case 300: _LL5435: yyval=({ struct _tagged_ptr2 _temp5450= yyvs; struct
_xenum_struct** _temp5452= _temp5450.curr + yyvsp_offset; if( _temp5452 <
_temp5450.base? 1: _temp5452 >= _temp5450.last_plus_one){ _throw( Null_Exception);}*
_temp5452;}); break; case 301: _LL5449: yyval=({ struct Cyc_Exp_tok_struct*
_temp5454=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5454=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_prim2_exp(
Cyc_Absyn_Bitand, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5455= yyvs;
struct _xenum_struct** _temp5457= _temp5455.curr +( yyvsp_offset - 2); if(
_temp5457 < _temp5455.base? 1: _temp5457 >= _temp5455.last_plus_one){ _throw(
Null_Exception);}* _temp5457;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5458= yyvs; struct _xenum_struct** _temp5460= _temp5458.curr + yyvsp_offset;
if( _temp5460 < _temp5458.base? 1: _temp5460 >= _temp5458.last_plus_one){ _throw(
Null_Exception);}* _temp5460;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5461= yyls; struct Cyc_Yyltype* _temp5463= _temp5461.curr +(
yylsp_offset - 2); if( _temp5463 < _temp5461.base? 1: _temp5463 >= _temp5461.last_plus_one){
_throw( Null_Exception);}* _temp5463;}).first_line,({ struct _tagged_ptr3
_temp5464= yyls; struct Cyc_Yyltype* _temp5466= _temp5464.curr + yylsp_offset;
if( _temp5466 < _temp5464.base? 1: _temp5466 >= _temp5464.last_plus_one){ _throw(
Null_Exception);}* _temp5466;}).last_line))};( struct _xenum_struct*) _temp5454;});
break; case 302: _LL5453: yyval=({ struct _tagged_ptr2 _temp5468= yyvs; struct
_xenum_struct** _temp5470= _temp5468.curr + yyvsp_offset; if( _temp5470 <
_temp5468.base? 1: _temp5470 >= _temp5468.last_plus_one){ _throw( Null_Exception);}*
_temp5470;}); break; case 303: _LL5467: yyval=({ struct Cyc_Exp_tok_struct*
_temp5472=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5472=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_eq_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5473= yyvs; struct _xenum_struct**
_temp5475= _temp5473.curr +( yyvsp_offset - 2); if( _temp5475 < _temp5473.base?
1: _temp5475 >= _temp5473.last_plus_one){ _throw( Null_Exception);}* _temp5475;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5476= yyvs; struct _xenum_struct**
_temp5478= _temp5476.curr + yyvsp_offset; if( _temp5478 < _temp5476.base? 1:
_temp5478 >= _temp5476.last_plus_one){ _throw( Null_Exception);}* _temp5478;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5479= yyls; struct Cyc_Yyltype*
_temp5481= _temp5479.curr +( yylsp_offset - 2); if( _temp5481 < _temp5479.base?
1: _temp5481 >= _temp5479.last_plus_one){ _throw( Null_Exception);}* _temp5481;}).first_line,({
struct _tagged_ptr3 _temp5482= yyls; struct Cyc_Yyltype* _temp5484= _temp5482.curr
+ yylsp_offset; if( _temp5484 < _temp5482.base? 1: _temp5484 >= _temp5482.last_plus_one){
_throw( Null_Exception);}* _temp5484;}).last_line))};( struct _xenum_struct*)
_temp5472;}); break; case 304: _LL5471: yyval=({ struct Cyc_Exp_tok_struct*
_temp5486=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5486=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_neq_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5487= yyvs; struct _xenum_struct**
_temp5489= _temp5487.curr +( yyvsp_offset - 2); if( _temp5489 < _temp5487.base?
1: _temp5489 >= _temp5487.last_plus_one){ _throw( Null_Exception);}* _temp5489;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5490= yyvs; struct _xenum_struct**
_temp5492= _temp5490.curr + yyvsp_offset; if( _temp5492 < _temp5490.base? 1:
_temp5492 >= _temp5490.last_plus_one){ _throw( Null_Exception);}* _temp5492;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5493= yyls; struct Cyc_Yyltype*
_temp5495= _temp5493.curr +( yylsp_offset - 2); if( _temp5495 < _temp5493.base?
1: _temp5495 >= _temp5493.last_plus_one){ _throw( Null_Exception);}* _temp5495;}).first_line,({
struct _tagged_ptr3 _temp5496= yyls; struct Cyc_Yyltype* _temp5498= _temp5496.curr
+ yylsp_offset; if( _temp5498 < _temp5496.base? 1: _temp5498 >= _temp5496.last_plus_one){
_throw( Null_Exception);}* _temp5498;}).last_line))};( struct _xenum_struct*)
_temp5486;}); break; case 305: _LL5485: yyval=({ struct _tagged_ptr2 _temp5500=
yyvs; struct _xenum_struct** _temp5502= _temp5500.curr + yyvsp_offset; if(
_temp5502 < _temp5500.base? 1: _temp5502 >= _temp5500.last_plus_one){ _throw(
Null_Exception);}* _temp5502;}); break; case 306: _LL5499: yyval=({ struct Cyc_Exp_tok_struct*
_temp5504=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5504=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_lt_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5505= yyvs; struct _xenum_struct**
_temp5507= _temp5505.curr +( yyvsp_offset - 2); if( _temp5507 < _temp5505.base?
1: _temp5507 >= _temp5505.last_plus_one){ _throw( Null_Exception);}* _temp5507;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5508= yyvs; struct _xenum_struct**
_temp5510= _temp5508.curr + yyvsp_offset; if( _temp5510 < _temp5508.base? 1:
_temp5510 >= _temp5508.last_plus_one){ _throw( Null_Exception);}* _temp5510;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5511= yyls; struct Cyc_Yyltype*
_temp5513= _temp5511.curr +( yylsp_offset - 2); if( _temp5513 < _temp5511.base?
1: _temp5513 >= _temp5511.last_plus_one){ _throw( Null_Exception);}* _temp5513;}).first_line,({
struct _tagged_ptr3 _temp5514= yyls; struct Cyc_Yyltype* _temp5516= _temp5514.curr
+ yylsp_offset; if( _temp5516 < _temp5514.base? 1: _temp5516 >= _temp5514.last_plus_one){
_throw( Null_Exception);}* _temp5516;}).last_line))};( struct _xenum_struct*)
_temp5504;}); break; case 307: _LL5503: yyval=({ struct Cyc_Exp_tok_struct*
_temp5518=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5518=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_gt_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5519= yyvs; struct _xenum_struct**
_temp5521= _temp5519.curr +( yyvsp_offset - 2); if( _temp5521 < _temp5519.base?
1: _temp5521 >= _temp5519.last_plus_one){ _throw( Null_Exception);}* _temp5521;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5522= yyvs; struct _xenum_struct**
_temp5524= _temp5522.curr + yyvsp_offset; if( _temp5524 < _temp5522.base? 1:
_temp5524 >= _temp5522.last_plus_one){ _throw( Null_Exception);}* _temp5524;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5525= yyls; struct Cyc_Yyltype*
_temp5527= _temp5525.curr +( yylsp_offset - 2); if( _temp5527 < _temp5525.base?
1: _temp5527 >= _temp5525.last_plus_one){ _throw( Null_Exception);}* _temp5527;}).first_line,({
struct _tagged_ptr3 _temp5528= yyls; struct Cyc_Yyltype* _temp5530= _temp5528.curr
+ yylsp_offset; if( _temp5530 < _temp5528.base? 1: _temp5530 >= _temp5528.last_plus_one){
_throw( Null_Exception);}* _temp5530;}).last_line))};( struct _xenum_struct*)
_temp5518;}); break; case 308: _LL5517: yyval=({ struct Cyc_Exp_tok_struct*
_temp5532=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5532=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_lte_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5533= yyvs; struct _xenum_struct**
_temp5535= _temp5533.curr +( yyvsp_offset - 2); if( _temp5535 < _temp5533.base?
1: _temp5535 >= _temp5533.last_plus_one){ _throw( Null_Exception);}* _temp5535;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5536= yyvs; struct _xenum_struct**
_temp5538= _temp5536.curr + yyvsp_offset; if( _temp5538 < _temp5536.base? 1:
_temp5538 >= _temp5536.last_plus_one){ _throw( Null_Exception);}* _temp5538;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5539= yyls; struct Cyc_Yyltype*
_temp5541= _temp5539.curr +( yylsp_offset - 2); if( _temp5541 < _temp5539.base?
1: _temp5541 >= _temp5539.last_plus_one){ _throw( Null_Exception);}* _temp5541;}).first_line,({
struct _tagged_ptr3 _temp5542= yyls; struct Cyc_Yyltype* _temp5544= _temp5542.curr
+ yylsp_offset; if( _temp5544 < _temp5542.base? 1: _temp5544 >= _temp5542.last_plus_one){
_throw( Null_Exception);}* _temp5544;}).last_line))};( struct _xenum_struct*)
_temp5532;}); break; case 309: _LL5531: yyval=({ struct Cyc_Exp_tok_struct*
_temp5546=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5546=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_gte_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5547= yyvs; struct _xenum_struct**
_temp5549= _temp5547.curr +( yyvsp_offset - 2); if( _temp5549 < _temp5547.base?
1: _temp5549 >= _temp5547.last_plus_one){ _throw( Null_Exception);}* _temp5549;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5550= yyvs; struct _xenum_struct**
_temp5552= _temp5550.curr + yyvsp_offset; if( _temp5552 < _temp5550.base? 1:
_temp5552 >= _temp5550.last_plus_one){ _throw( Null_Exception);}* _temp5552;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5553= yyls; struct Cyc_Yyltype*
_temp5555= _temp5553.curr +( yylsp_offset - 2); if( _temp5555 < _temp5553.base?
1: _temp5555 >= _temp5553.last_plus_one){ _throw( Null_Exception);}* _temp5555;}).first_line,({
struct _tagged_ptr3 _temp5556= yyls; struct Cyc_Yyltype* _temp5558= _temp5556.curr
+ yylsp_offset; if( _temp5558 < _temp5556.base? 1: _temp5558 >= _temp5556.last_plus_one){
_throw( Null_Exception);}* _temp5558;}).last_line))};( struct _xenum_struct*)
_temp5546;}); break; case 310: _LL5545: yyval=({ struct _tagged_ptr2 _temp5560=
yyvs; struct _xenum_struct** _temp5562= _temp5560.curr + yyvsp_offset; if(
_temp5562 < _temp5560.base? 1: _temp5562 >= _temp5560.last_plus_one){ _throw(
Null_Exception);}* _temp5562;}); break; case 311: _LL5559: yyval=({ struct Cyc_Exp_tok_struct*
_temp5564=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5564=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_prim2_exp(
Cyc_Absyn_Bitlshift, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5565= yyvs;
struct _xenum_struct** _temp5567= _temp5565.curr +( yyvsp_offset - 2); if(
_temp5567 < _temp5565.base? 1: _temp5567 >= _temp5565.last_plus_one){ _throw(
Null_Exception);}* _temp5567;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5568= yyvs; struct _xenum_struct** _temp5570= _temp5568.curr + yyvsp_offset;
if( _temp5570 < _temp5568.base? 1: _temp5570 >= _temp5568.last_plus_one){ _throw(
Null_Exception);}* _temp5570;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5571= yyls; struct Cyc_Yyltype* _temp5573= _temp5571.curr +(
yylsp_offset - 2); if( _temp5573 < _temp5571.base? 1: _temp5573 >= _temp5571.last_plus_one){
_throw( Null_Exception);}* _temp5573;}).first_line,({ struct _tagged_ptr3
_temp5574= yyls; struct Cyc_Yyltype* _temp5576= _temp5574.curr + yylsp_offset;
if( _temp5576 < _temp5574.base? 1: _temp5576 >= _temp5574.last_plus_one){ _throw(
Null_Exception);}* _temp5576;}).last_line))};( struct _xenum_struct*) _temp5564;});
break; case 312: _LL5563: yyval=({ struct Cyc_Exp_tok_struct* _temp5578=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp5578=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_prim2_exp( Cyc_Absyn_Bitlrshift,
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
_temp5578;}); break; case 313: _LL5577: yyval=({ struct _tagged_ptr2 _temp5592=
yyvs; struct _xenum_struct** _temp5594= _temp5592.curr + yyvsp_offset; if(
_temp5594 < _temp5592.base? 1: _temp5594 >= _temp5592.last_plus_one){ _throw(
Null_Exception);}* _temp5594;}); break; case 314: _LL5591: yyval=({ struct Cyc_Exp_tok_struct*
_temp5596=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5596=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_prim2_exp(
Cyc_Absyn_Plus, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5597= yyvs; struct
_xenum_struct** _temp5599= _temp5597.curr +( yyvsp_offset - 2); if( _temp5599 <
_temp5597.base? 1: _temp5599 >= _temp5597.last_plus_one){ _throw( Null_Exception);}*
_temp5599;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5600= yyvs; struct
_xenum_struct** _temp5602= _temp5600.curr + yyvsp_offset; if( _temp5602 <
_temp5600.base? 1: _temp5602 >= _temp5600.last_plus_one){ _throw( Null_Exception);}*
_temp5602;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5603=
yyls; struct Cyc_Yyltype* _temp5605= _temp5603.curr +( yylsp_offset - 2); if(
_temp5605 < _temp5603.base? 1: _temp5605 >= _temp5603.last_plus_one){ _throw(
Null_Exception);}* _temp5605;}).first_line,({ struct _tagged_ptr3 _temp5606=
yyls; struct Cyc_Yyltype* _temp5608= _temp5606.curr + yylsp_offset; if(
_temp5608 < _temp5606.base? 1: _temp5608 >= _temp5606.last_plus_one){ _throw(
Null_Exception);}* _temp5608;}).last_line))};( struct _xenum_struct*) _temp5596;});
break; case 315: _LL5595: yyval=({ struct Cyc_Exp_tok_struct* _temp5610=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp5610=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_prim2_exp( Cyc_Absyn_Minus,
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
_temp5610;}); break; case 316: _LL5609: yyval=({ struct _tagged_ptr2 _temp5624=
yyvs; struct _xenum_struct** _temp5626= _temp5624.curr + yyvsp_offset; if(
_temp5626 < _temp5624.base? 1: _temp5626 >= _temp5624.last_plus_one){ _throw(
Null_Exception);}* _temp5626;}); break; case 317: _LL5623: yyval=({ struct Cyc_Exp_tok_struct*
_temp5628=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5628=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_prim2_exp(
Cyc_Absyn_Times, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5629= yyvs;
struct _xenum_struct** _temp5631= _temp5629.curr +( yyvsp_offset - 2); if(
_temp5631 < _temp5629.base? 1: _temp5631 >= _temp5629.last_plus_one){ _throw(
Null_Exception);}* _temp5631;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5632= yyvs; struct _xenum_struct** _temp5634= _temp5632.curr + yyvsp_offset;
if( _temp5634 < _temp5632.base? 1: _temp5634 >= _temp5632.last_plus_one){ _throw(
Null_Exception);}* _temp5634;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5635= yyls; struct Cyc_Yyltype* _temp5637= _temp5635.curr +(
yylsp_offset - 2); if( _temp5637 < _temp5635.base? 1: _temp5637 >= _temp5635.last_plus_one){
_throw( Null_Exception);}* _temp5637;}).first_line,({ struct _tagged_ptr3
_temp5638= yyls; struct Cyc_Yyltype* _temp5640= _temp5638.curr + yylsp_offset;
if( _temp5640 < _temp5638.base? 1: _temp5640 >= _temp5638.last_plus_one){ _throw(
Null_Exception);}* _temp5640;}).last_line))};( struct _xenum_struct*) _temp5628;});
break; case 318: _LL5627: yyval=({ struct Cyc_Exp_tok_struct* _temp5642=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp5642=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_prim2_exp( Cyc_Absyn_Div,
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5643= yyvs; struct _xenum_struct**
_temp5645= _temp5643.curr +( yyvsp_offset - 2); if( _temp5645 < _temp5643.base?
1: _temp5645 >= _temp5643.last_plus_one){ _throw( Null_Exception);}* _temp5645;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5646= yyvs; struct _xenum_struct**
_temp5648= _temp5646.curr + yyvsp_offset; if( _temp5648 < _temp5646.base? 1:
_temp5648 >= _temp5646.last_plus_one){ _throw( Null_Exception);}* _temp5648;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5649= yyls; struct Cyc_Yyltype*
_temp5651= _temp5649.curr +( yylsp_offset - 2); if( _temp5651 < _temp5649.base?
1: _temp5651 >= _temp5649.last_plus_one){ _throw( Null_Exception);}* _temp5651;}).first_line,({
struct _tagged_ptr3 _temp5652= yyls; struct Cyc_Yyltype* _temp5654= _temp5652.curr
+ yylsp_offset; if( _temp5654 < _temp5652.base? 1: _temp5654 >= _temp5652.last_plus_one){
_throw( Null_Exception);}* _temp5654;}).last_line))};( struct _xenum_struct*)
_temp5642;}); break; case 319: _LL5641: yyval=({ struct Cyc_Exp_tok_struct*
_temp5656=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5656=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_prim2_exp(
Cyc_Absyn_Mod, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5657= yyvs; struct
_xenum_struct** _temp5659= _temp5657.curr +( yyvsp_offset - 2); if( _temp5659 <
_temp5657.base? 1: _temp5659 >= _temp5657.last_plus_one){ _throw( Null_Exception);}*
_temp5659;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5660= yyvs; struct
_xenum_struct** _temp5662= _temp5660.curr + yyvsp_offset; if( _temp5662 <
_temp5660.base? 1: _temp5662 >= _temp5660.last_plus_one){ _throw( Null_Exception);}*
_temp5662;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5663=
yyls; struct Cyc_Yyltype* _temp5665= _temp5663.curr +( yylsp_offset - 2); if(
_temp5665 < _temp5663.base? 1: _temp5665 >= _temp5663.last_plus_one){ _throw(
Null_Exception);}* _temp5665;}).first_line,({ struct _tagged_ptr3 _temp5666=
yyls; struct Cyc_Yyltype* _temp5668= _temp5666.curr + yylsp_offset; if(
_temp5668 < _temp5666.base? 1: _temp5668 >= _temp5666.last_plus_one){ _throw(
Null_Exception);}* _temp5668;}).last_line))};( struct _xenum_struct*) _temp5656;});
break; case 320: _LL5655: yyval=({ struct _tagged_ptr2 _temp5670= yyvs; struct
_xenum_struct** _temp5672= _temp5670.curr + yyvsp_offset; if( _temp5672 <
_temp5670.base? 1: _temp5672 >= _temp5670.last_plus_one){ _throw( Null_Exception);}*
_temp5672;}); break; case 321: _LL5669: yyval=({ struct Cyc_Exp_tok_struct*
_temp5674=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5674=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_cast_exp((*
Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp5675= yyvs; struct
_xenum_struct** _temp5677= _temp5675.curr +( yyvsp_offset - 2); if( _temp5677 <
_temp5675.base? 1: _temp5677 >= _temp5675.last_plus_one){ _throw( Null_Exception);}*
_temp5677;}))).f3, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5678= yyvs;
struct _xenum_struct** _temp5680= _temp5678.curr + yyvsp_offset; if( _temp5680 <
_temp5678.base? 1: _temp5680 >= _temp5678.last_plus_one){ _throw( Null_Exception);}*
_temp5680;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5681=
yyls; struct Cyc_Yyltype* _temp5683= _temp5681.curr +( yylsp_offset - 3); if(
_temp5683 < _temp5681.base? 1: _temp5683 >= _temp5681.last_plus_one){ _throw(
Null_Exception);}* _temp5683;}).first_line,({ struct _tagged_ptr3 _temp5684=
yyls; struct Cyc_Yyltype* _temp5686= _temp5684.curr + yylsp_offset; if(
_temp5686 < _temp5684.base? 1: _temp5686 >= _temp5684.last_plus_one){ _throw(
Null_Exception);}* _temp5686;}).last_line))};( struct _xenum_struct*) _temp5674;});
break; case 322: _LL5673: yyval=({ struct _tagged_ptr2 _temp5688= yyvs; struct
_xenum_struct** _temp5690= _temp5688.curr + yyvsp_offset; if( _temp5690 <
_temp5688.base? 1: _temp5690 >= _temp5688.last_plus_one){ _throw( Null_Exception);}*
_temp5690;}); break; case 323: _LL5687: yyval=({ struct Cyc_Exp_tok_struct*
_temp5692=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5692=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_pre_inc_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5693= yyvs; struct _xenum_struct**
_temp5695= _temp5693.curr + yyvsp_offset; if( _temp5695 < _temp5693.base? 1:
_temp5695 >= _temp5693.last_plus_one){ _throw( Null_Exception);}* _temp5695;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5696= yyls; struct Cyc_Yyltype*
_temp5698= _temp5696.curr +( yylsp_offset - 1); if( _temp5698 < _temp5696.base?
1: _temp5698 >= _temp5696.last_plus_one){ _throw( Null_Exception);}* _temp5698;}).first_line,({
struct _tagged_ptr3 _temp5699= yyls; struct Cyc_Yyltype* _temp5701= _temp5699.curr
+ yylsp_offset; if( _temp5701 < _temp5699.base? 1: _temp5701 >= _temp5699.last_plus_one){
_throw( Null_Exception);}* _temp5701;}).last_line))};( struct _xenum_struct*)
_temp5692;}); break; case 324: _LL5691: yyval=({ struct Cyc_Exp_tok_struct*
_temp5703=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5703=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_pre_dec_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5704= yyvs; struct _xenum_struct**
_temp5706= _temp5704.curr + yyvsp_offset; if( _temp5706 < _temp5704.base? 1:
_temp5706 >= _temp5704.last_plus_one){ _throw( Null_Exception);}* _temp5706;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5707= yyls; struct Cyc_Yyltype*
_temp5709= _temp5707.curr +( yylsp_offset - 1); if( _temp5709 < _temp5707.base?
1: _temp5709 >= _temp5707.last_plus_one){ _throw( Null_Exception);}* _temp5709;}).first_line,({
struct _tagged_ptr3 _temp5710= yyls; struct Cyc_Yyltype* _temp5712= _temp5710.curr
+ yylsp_offset; if( _temp5712 < _temp5710.base? 1: _temp5712 >= _temp5710.last_plus_one){
_throw( Null_Exception);}* _temp5712;}).last_line))};( struct _xenum_struct*)
_temp5703;}); break; case 325: _LL5702: yyval=({ struct Cyc_Exp_tok_struct*
_temp5714=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5714=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_address_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5715= yyvs; struct _xenum_struct**
_temp5717= _temp5715.curr + yyvsp_offset; if( _temp5717 < _temp5715.base? 1:
_temp5717 >= _temp5715.last_plus_one){ _throw( Null_Exception);}* _temp5717;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5718= yyls; struct Cyc_Yyltype*
_temp5720= _temp5718.curr +( yylsp_offset - 1); if( _temp5720 < _temp5718.base?
1: _temp5720 >= _temp5718.last_plus_one){ _throw( Null_Exception);}* _temp5720;}).first_line,({
struct _tagged_ptr3 _temp5721= yyls; struct Cyc_Yyltype* _temp5723= _temp5721.curr
+ yylsp_offset; if( _temp5723 < _temp5721.base? 1: _temp5723 >= _temp5721.last_plus_one){
_throw( Null_Exception);}* _temp5723;}).last_line))};( struct _xenum_struct*)
_temp5714;}); break; case 326: _LL5713: yyval=({ struct Cyc_Exp_tok_struct*
_temp5725=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5725=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_deref_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5726= yyvs; struct _xenum_struct**
_temp5728= _temp5726.curr + yyvsp_offset; if( _temp5728 < _temp5726.base? 1:
_temp5728 >= _temp5726.last_plus_one){ _throw( Null_Exception);}* _temp5728;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5729= yyls; struct Cyc_Yyltype*
_temp5731= _temp5729.curr +( yylsp_offset - 1); if( _temp5731 < _temp5729.base?
1: _temp5731 >= _temp5729.last_plus_one){ _throw( Null_Exception);}* _temp5731;}).first_line,({
struct _tagged_ptr3 _temp5732= yyls; struct Cyc_Yyltype* _temp5734= _temp5732.curr
+ yylsp_offset; if( _temp5734 < _temp5732.base? 1: _temp5734 >= _temp5732.last_plus_one){
_throw( Null_Exception);}* _temp5734;}).last_line))};( struct _xenum_struct*)
_temp5725;}); break; case 327: _LL5724: yyval=({ struct _tagged_ptr2 _temp5736=
yyvs; struct _xenum_struct** _temp5738= _temp5736.curr + yyvsp_offset; if(
_temp5738 < _temp5736.base? 1: _temp5738 >= _temp5736.last_plus_one){ _throw(
Null_Exception);}* _temp5738;}); break; case 328: _LL5735: yyval=({ struct Cyc_Exp_tok_struct*
_temp5740=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5740=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_prim1_exp(
Cyc_yyget_Primop_tok(({ struct _tagged_ptr2 _temp5741= yyvs; struct
_xenum_struct** _temp5743= _temp5741.curr +( yyvsp_offset - 1); if( _temp5743 <
_temp5741.base? 1: _temp5743 >= _temp5741.last_plus_one){ _throw( Null_Exception);}*
_temp5743;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5744= yyvs; struct
_xenum_struct** _temp5746= _temp5744.curr + yyvsp_offset; if( _temp5746 <
_temp5744.base? 1: _temp5746 >= _temp5744.last_plus_one){ _throw( Null_Exception);}*
_temp5746;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5747=
yyls; struct Cyc_Yyltype* _temp5749= _temp5747.curr +( yylsp_offset - 1); if(
_temp5749 < _temp5747.base? 1: _temp5749 >= _temp5747.last_plus_one){ _throw(
Null_Exception);}* _temp5749;}).first_line,({ struct _tagged_ptr3 _temp5750=
yyls; struct Cyc_Yyltype* _temp5752= _temp5750.curr + yylsp_offset; if(
_temp5752 < _temp5750.base? 1: _temp5752 >= _temp5750.last_plus_one){ _throw(
Null_Exception);}* _temp5752;}).last_line))};( struct _xenum_struct*) _temp5740;});
break; case 329: _LL5739: yyval=({ struct Cyc_Exp_tok_struct* _temp5754=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp5754=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_sizeof_exp((*
Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp5755= yyvs; struct
_xenum_struct** _temp5757= _temp5755.curr +( yyvsp_offset - 1); if( _temp5757 <
_temp5755.base? 1: _temp5757 >= _temp5755.last_plus_one){ _throw( Null_Exception);}*
_temp5757;}))).f3, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5758=
yyls; struct Cyc_Yyltype* _temp5760= _temp5758.curr +( yylsp_offset - 3); if(
_temp5760 < _temp5758.base? 1: _temp5760 >= _temp5758.last_plus_one){ _throw(
Null_Exception);}* _temp5760;}).first_line,({ struct _tagged_ptr3 _temp5761=
yyls; struct Cyc_Yyltype* _temp5763= _temp5761.curr + yylsp_offset; if(
_temp5763 < _temp5761.base? 1: _temp5763 >= _temp5761.last_plus_one){ _throw(
Null_Exception);}* _temp5763;}).last_line))};( struct _xenum_struct*) _temp5754;});
break; case 330: _LL5753: yyval=({ struct Cyc_Exp_tok_struct* _temp5765=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp5765=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_prim1_exp( Cyc_Absyn_Size,
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5766= yyvs; struct _xenum_struct**
_temp5768= _temp5766.curr + yyvsp_offset; if( _temp5768 < _temp5766.base? 1:
_temp5768 >= _temp5766.last_plus_one){ _throw( Null_Exception);}* _temp5768;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5769= yyls; struct Cyc_Yyltype*
_temp5771= _temp5769.curr +( yylsp_offset - 1); if( _temp5771 < _temp5769.base?
1: _temp5771 >= _temp5769.last_plus_one){ _throw( Null_Exception);}* _temp5771;}).first_line,({
struct _tagged_ptr3 _temp5772= yyls; struct Cyc_Yyltype* _temp5774= _temp5772.curr
+ yylsp_offset; if( _temp5774 < _temp5772.base? 1: _temp5774 >= _temp5772.last_plus_one){
_throw( Null_Exception);}* _temp5774;}).last_line))};( struct _xenum_struct*)
_temp5765;}); break; case 331: _LL5764: yyval=({ struct Cyc_Exp_tok_struct*
_temp5776=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5776=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_throw_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5777= yyvs; struct _xenum_struct**
_temp5779= _temp5777.curr + yyvsp_offset; if( _temp5779 < _temp5777.base? 1:
_temp5779 >= _temp5777.last_plus_one){ _throw( Null_Exception);}* _temp5779;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5780= yyls; struct Cyc_Yyltype*
_temp5782= _temp5780.curr +( yylsp_offset - 1); if( _temp5782 < _temp5780.base?
1: _temp5782 >= _temp5780.last_plus_one){ _throw( Null_Exception);}* _temp5782;}).first_line,({
struct _tagged_ptr3 _temp5783= yyls; struct Cyc_Yyltype* _temp5785= _temp5783.curr
+ yylsp_offset; if( _temp5785 < _temp5783.base? 1: _temp5785 >= _temp5783.last_plus_one){
_throw( Null_Exception);}* _temp5785;}).last_line))};( struct _xenum_struct*)
_temp5776;}); break; case 332: _LL5775: yyval=({ struct Cyc_Exp_tok_struct*
_temp5787=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5787=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_primop_exp(
Cyc_yyget_Primop_tok(({ struct _tagged_ptr2 _temp5788= yyvs; struct
_xenum_struct** _temp5790= _temp5788.curr +( yyvsp_offset - 3); if( _temp5790 <
_temp5788.base? 1: _temp5790 >= _temp5788.last_plus_one){ _throw( Null_Exception);}*
_temp5790;})), Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2 _temp5791= yyvs;
struct _xenum_struct** _temp5793= _temp5791.curr +( yyvsp_offset - 1); if(
_temp5793 < _temp5791.base? 1: _temp5793 >= _temp5791.last_plus_one){ _throw(
Null_Exception);}* _temp5793;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5794= yyls; struct Cyc_Yyltype* _temp5796= _temp5794.curr +(
yylsp_offset - 3); if( _temp5796 < _temp5794.base? 1: _temp5796 >= _temp5794.last_plus_one){
_throw( Null_Exception);}* _temp5796;}).first_line,({ struct _tagged_ptr3
_temp5797= yyls; struct Cyc_Yyltype* _temp5799= _temp5797.curr + yylsp_offset;
if( _temp5799 < _temp5797.base? 1: _temp5799 >= _temp5797.last_plus_one){ _throw(
Null_Exception);}* _temp5799;}).last_line))};( struct _xenum_struct*) _temp5787;});
break; case 333: _LL5786: yyval=({ struct Cyc_Exp_tok_struct* _temp5801=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp5801=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_Malloc_e_struct* _temp5802=( struct Cyc_Absyn_Malloc_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct)); _temp5802->tag= Cyc_Absyn_Malloc_e_tag;
_temp5802->f1=( void*)({ struct Cyc_Absyn_Typ_m_struct* _temp5803=( struct Cyc_Absyn_Typ_m_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Typ_m_struct)); _temp5803->tag= Cyc_Absyn_Typ_m_tag;
_temp5803->f1=( void*) Cyc_Parse_speclist2typ((* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp5804= yyvs; struct _xenum_struct** _temp5806= _temp5804.curr
+( yyvsp_offset - 2); if( _temp5806 < _temp5804.base? 1: _temp5806 >= _temp5804.last_plus_one){
_throw( Null_Exception);}* _temp5806;}))).f2, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5807= yyls; struct Cyc_Yyltype* _temp5809= _temp5807.curr
+( yylsp_offset - 2); if( _temp5809 < _temp5807.base? 1: _temp5809 >= _temp5807.last_plus_one){
_throw( Null_Exception);}* _temp5809;}).first_line,({ struct _tagged_ptr3
_temp5810= yyls; struct Cyc_Yyltype* _temp5812= _temp5810.curr +( yylsp_offset -
2); if( _temp5812 < _temp5810.base? 1: _temp5812 >= _temp5810.last_plus_one){
_throw( Null_Exception);}* _temp5812;}).last_line));( void*) _temp5803;});( void*)
_temp5802;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5813= yyls;
struct Cyc_Yyltype* _temp5815= _temp5813.curr +( yylsp_offset - 6); if(
_temp5815 < _temp5813.base? 1: _temp5815 >= _temp5813.last_plus_one){ _throw(
Null_Exception);}* _temp5815;}).first_line,({ struct _tagged_ptr3 _temp5816=
yyls; struct Cyc_Yyltype* _temp5818= _temp5816.curr + yylsp_offset; if(
_temp5818 < _temp5816.base? 1: _temp5818 >= _temp5816.last_plus_one){ _throw(
Null_Exception);}* _temp5818;}).last_line))};( struct _xenum_struct*) _temp5801;});
break; case 334: _LL5800: yyval=({ struct Cyc_Exp_tok_struct* _temp5820=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp5820=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_Malloc_e_struct* _temp5821=( struct Cyc_Absyn_Malloc_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Malloc_e_struct)); _temp5821->tag= Cyc_Absyn_Malloc_e_tag;
_temp5821->f1=( void*)({ struct Cyc_Absyn_Unresolved_m_struct* _temp5822=(
struct Cyc_Absyn_Unresolved_m_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Unresolved_m_struct));
_temp5822->tag= Cyc_Absyn_Unresolved_m_tag; _temp5822->f1= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp5823= yyvs; struct _xenum_struct** _temp5825= _temp5823.curr
+( yyvsp_offset - 2); if( _temp5825 < _temp5823.base? 1: _temp5825 >= _temp5823.last_plus_one){
_throw( Null_Exception);}* _temp5825;}));( void*) _temp5822;});( void*)
_temp5821;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5826= yyls;
struct Cyc_Yyltype* _temp5828= _temp5826.curr +( yylsp_offset - 6); if(
_temp5828 < _temp5826.base? 1: _temp5828 >= _temp5826.last_plus_one){ _throw(
Null_Exception);}* _temp5828;}).first_line,({ struct _tagged_ptr3 _temp5829=
yyls; struct Cyc_Yyltype* _temp5831= _temp5829.curr + yylsp_offset; if(
_temp5831 < _temp5829.base? 1: _temp5831 >= _temp5829.last_plus_one){ _throw(
Null_Exception);}* _temp5831;}).last_line))};( struct _xenum_struct*) _temp5820;});
break; case 335: _LL5819: yyval=({ struct Cyc_Primop_tok_struct* _temp5833=(
struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));*
_temp5833=( struct Cyc_Primop_tok_struct){.tag= Cyc_Primop_tok_tag,.f1= Cyc_Absyn_Printf};(
struct _xenum_struct*) _temp5833;}); break; case 336: _LL5832: yyval=({ struct
Cyc_Primop_tok_struct* _temp5835=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct));* _temp5835=( struct Cyc_Primop_tok_struct){.tag=
Cyc_Primop_tok_tag,.f1= Cyc_Absyn_Fprintf};( struct _xenum_struct*) _temp5835;});
break; case 337: _LL5834: yyval=({ struct Cyc_Primop_tok_struct* _temp5837=(
struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));*
_temp5837=( struct Cyc_Primop_tok_struct){.tag= Cyc_Primop_tok_tag,.f1= Cyc_Absyn_Xprintf};(
struct _xenum_struct*) _temp5837;}); break; case 338: _LL5836: yyval=({ struct
Cyc_Primop_tok_struct* _temp5839=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct));* _temp5839=( struct Cyc_Primop_tok_struct){.tag=
Cyc_Primop_tok_tag,.f1= Cyc_Absyn_Scanf};( struct _xenum_struct*) _temp5839;});
break; case 339: _LL5838: yyval=({ struct Cyc_Primop_tok_struct* _temp5841=(
struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));*
_temp5841=( struct Cyc_Primop_tok_struct){.tag= Cyc_Primop_tok_tag,.f1= Cyc_Absyn_Fscanf};(
struct _xenum_struct*) _temp5841;}); break; case 340: _LL5840: yyval=({ struct
Cyc_Primop_tok_struct* _temp5843=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct));* _temp5843=( struct Cyc_Primop_tok_struct){.tag=
Cyc_Primop_tok_tag,.f1= Cyc_Absyn_Sscanf};( struct _xenum_struct*) _temp5843;});
break; case 341: _LL5842: yyval=({ struct Cyc_Primop_tok_struct* _temp5845=(
struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));*
_temp5845=( struct Cyc_Primop_tok_struct){.tag= Cyc_Primop_tok_tag,.f1= Cyc_Absyn_Bitnot};(
struct _xenum_struct*) _temp5845;}); break; case 342: _LL5844: yyval=({ struct
Cyc_Primop_tok_struct* _temp5847=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct));* _temp5847=( struct Cyc_Primop_tok_struct){.tag=
Cyc_Primop_tok_tag,.f1= Cyc_Absyn_Not};( struct _xenum_struct*) _temp5847;});
break; case 343: _LL5846: yyval=({ struct Cyc_Primop_tok_struct* _temp5849=(
struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));*
_temp5849=( struct Cyc_Primop_tok_struct){.tag= Cyc_Primop_tok_tag,.f1= Cyc_Absyn_Minus};(
struct _xenum_struct*) _temp5849;}); break; case 344: _LL5848: yyval=({ struct
_tagged_ptr2 _temp5851= yyvs; struct _xenum_struct** _temp5853= _temp5851.curr +
yyvsp_offset; if( _temp5853 < _temp5851.base? 1: _temp5853 >= _temp5851.last_plus_one){
_throw( Null_Exception);}* _temp5853;}); break; case 345: _LL5850: yyval=({
struct Cyc_Exp_tok_struct* _temp5855=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct));* _temp5855=( struct Cyc_Exp_tok_struct){.tag=
Cyc_Exp_tok_tag,.f1= Cyc_Absyn_subscript_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5856= yyvs; struct _xenum_struct** _temp5858= _temp5856.curr +(
yyvsp_offset - 3); if( _temp5858 < _temp5856.base? 1: _temp5858 >= _temp5856.last_plus_one){
_throw( Null_Exception);}* _temp5858;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5859= yyvs; struct _xenum_struct** _temp5861= _temp5859.curr +(
yyvsp_offset - 1); if( _temp5861 < _temp5859.base? 1: _temp5861 >= _temp5859.last_plus_one){
_throw( Null_Exception);}* _temp5861;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5862= yyls; struct Cyc_Yyltype* _temp5864= _temp5862.curr +(
yylsp_offset - 3); if( _temp5864 < _temp5862.base? 1: _temp5864 >= _temp5862.last_plus_one){
_throw( Null_Exception);}* _temp5864;}).first_line,({ struct _tagged_ptr3
_temp5865= yyls; struct Cyc_Yyltype* _temp5867= _temp5865.curr + yylsp_offset;
if( _temp5867 < _temp5865.base? 1: _temp5867 >= _temp5865.last_plus_one){ _throw(
Null_Exception);}* _temp5867;}).last_line))};( struct _xenum_struct*) _temp5855;});
break; case 346: _LL5854: yyval=({ struct Cyc_Exp_tok_struct* _temp5869=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp5869=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5870= yyvs; struct _xenum_struct**
_temp5872= _temp5870.curr +( yyvsp_offset - 2); if( _temp5872 < _temp5870.base?
1: _temp5872 >= _temp5870.last_plus_one){ _throw( Null_Exception);}* _temp5872;})),
0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5873= yyls; struct
Cyc_Yyltype* _temp5875= _temp5873.curr +( yylsp_offset - 2); if( _temp5875 <
_temp5873.base? 1: _temp5875 >= _temp5873.last_plus_one){ _throw( Null_Exception);}*
_temp5875;}).first_line,({ struct _tagged_ptr3 _temp5876= yyls; struct Cyc_Yyltype*
_temp5878= _temp5876.curr + yylsp_offset; if( _temp5878 < _temp5876.base? 1:
_temp5878 >= _temp5876.last_plus_one){ _throw( Null_Exception);}* _temp5878;}).last_line))};(
struct _xenum_struct*) _temp5869;}); break; case 347: _LL5868: yyval=({ struct
Cyc_Exp_tok_struct* _temp5880=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct));* _temp5880=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1=
Cyc_Absyn_unknowncall_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5881=
yyvs; struct _xenum_struct** _temp5883= _temp5881.curr +( yyvsp_offset - 3); if(
_temp5883 < _temp5881.base? 1: _temp5883 >= _temp5881.last_plus_one){ _throw(
Null_Exception);}* _temp5883;})), Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2
_temp5884= yyvs; struct _xenum_struct** _temp5886= _temp5884.curr +(
yyvsp_offset - 1); if( _temp5886 < _temp5884.base? 1: _temp5886 >= _temp5884.last_plus_one){
_throw( Null_Exception);}* _temp5886;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5887= yyls; struct Cyc_Yyltype* _temp5889= _temp5887.curr +(
yylsp_offset - 3); if( _temp5889 < _temp5887.base? 1: _temp5889 >= _temp5887.last_plus_one){
_throw( Null_Exception);}* _temp5889;}).first_line,({ struct _tagged_ptr3
_temp5890= yyls; struct Cyc_Yyltype* _temp5892= _temp5890.curr + yylsp_offset;
if( _temp5892 < _temp5890.base? 1: _temp5892 >= _temp5890.last_plus_one){ _throw(
Null_Exception);}* _temp5892;}).last_line))};( struct _xenum_struct*) _temp5880;});
break; case 348: _LL5879: yyval=({ struct Cyc_Exp_tok_struct* _temp5894=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp5894=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_structmember_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5895= yyvs; struct _xenum_struct**
_temp5897= _temp5895.curr +( yyvsp_offset - 2); if( _temp5897 < _temp5895.base?
1: _temp5897 >= _temp5895.last_plus_one){ _throw( Null_Exception);}* _temp5897;})),({
struct _tagged_string* _temp5898=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp5898[ 0]= Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp5899= yyvs; struct _xenum_struct** _temp5901= _temp5899.curr +
yyvsp_offset; if( _temp5901 < _temp5899.base? 1: _temp5901 >= _temp5899.last_plus_one){
_throw( Null_Exception);}* _temp5901;})); _temp5898;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5902= yyls; struct Cyc_Yyltype* _temp5904= _temp5902.curr
+( yylsp_offset - 2); if( _temp5904 < _temp5902.base? 1: _temp5904 >= _temp5902.last_plus_one){
_throw( Null_Exception);}* _temp5904;}).first_line,({ struct _tagged_ptr3
_temp5905= yyls; struct Cyc_Yyltype* _temp5907= _temp5905.curr + yylsp_offset;
if( _temp5907 < _temp5905.base? 1: _temp5907 >= _temp5905.last_plus_one){ _throw(
Null_Exception);}* _temp5907;}).last_line))};( struct _xenum_struct*) _temp5894;});
break; case 349: _LL5893: { struct _tuple1* q= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp5909= yyvs; struct _xenum_struct** _temp5911= _temp5909.curr +
yyvsp_offset; if( _temp5911 < _temp5909.base? 1: _temp5911 >= _temp5909.last_plus_one){
_throw( Null_Exception);}* _temp5911;}));{ void* _temp5912=(* q).f1; struct Cyc_List_List*
_temp5922; struct Cyc_List_List* _temp5924; _LL5914: if( _temp5912 == Cyc_Absyn_Loc_n){
goto _LL5915;} else{ goto _LL5916;} _LL5916: if(( unsigned int) _temp5912 > 1u?((
struct _enum_struct*) _temp5912)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL5923:
_temp5922=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp5912)->f1;
if( _temp5922 == 0){ goto _LL5917;} else{ goto _LL5918;}} else{ goto _LL5918;}
_LL5918: if(( unsigned int) _temp5912 > 1u?(( struct _enum_struct*) _temp5912)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL5925: _temp5924=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp5912)->f1; if( _temp5924 == 0){ goto _LL5919;}
else{ goto _LL5920;}} else{ goto _LL5920;} _LL5920: goto _LL5921; _LL5915: goto
_LL5913; _LL5917: goto _LL5913; _LL5919: goto _LL5913; _LL5921: Cyc_Parse_err((
struct _tagged_string)({ char* _temp5926=( char*)"struct field name is qualified";
struct _tagged_string _temp5927; _temp5927.curr= _temp5926; _temp5927.base=
_temp5926; _temp5927.last_plus_one= _temp5926 + 31; _temp5927;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5928= yyls; struct Cyc_Yyltype* _temp5930= _temp5928.curr
+ yylsp_offset; if( _temp5930 < _temp5928.base? 1: _temp5930 >= _temp5928.last_plus_one){
_throw( Null_Exception);}* _temp5930;}).first_line,({ struct _tagged_ptr3
_temp5931= yyls; struct Cyc_Yyltype* _temp5933= _temp5931.curr + yylsp_offset;
if( _temp5933 < _temp5931.base? 1: _temp5933 >= _temp5931.last_plus_one){ _throw(
Null_Exception);}* _temp5933;}).last_line)); goto _LL5913; _LL5913:;} yyval=({
struct Cyc_Exp_tok_struct* _temp5934=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct));* _temp5934=( struct Cyc_Exp_tok_struct){.tag=
Cyc_Exp_tok_tag,.f1= Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5935= yyvs; struct _xenum_struct** _temp5937= _temp5935.curr +(
yyvsp_offset - 2); if( _temp5937 < _temp5935.base? 1: _temp5937 >= _temp5935.last_plus_one){
_throw( Null_Exception);}* _temp5937;})),(* q).f2, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5938= yyls; struct Cyc_Yyltype* _temp5940= _temp5938.curr
+( yylsp_offset - 2); if( _temp5940 < _temp5938.base? 1: _temp5940 >= _temp5938.last_plus_one){
_throw( Null_Exception);}* _temp5940;}).first_line,({ struct _tagged_ptr3
_temp5941= yyls; struct Cyc_Yyltype* _temp5943= _temp5941.curr + yylsp_offset;
if( _temp5943 < _temp5941.base? 1: _temp5943 >= _temp5941.last_plus_one){ _throw(
Null_Exception);}* _temp5943;}).last_line))};( struct _xenum_struct*) _temp5934;});
break;} case 350: _LL5908: yyval=({ struct Cyc_Exp_tok_struct* _temp5945=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5945=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_structarrow_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5946= yyvs; struct _xenum_struct**
_temp5948= _temp5946.curr +( yyvsp_offset - 2); if( _temp5948 < _temp5946.base?
1: _temp5948 >= _temp5946.last_plus_one){ _throw( Null_Exception);}* _temp5948;})),({
struct _tagged_string* _temp5949=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp5949[ 0]= Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp5950= yyvs; struct _xenum_struct** _temp5952= _temp5950.curr +
yyvsp_offset; if( _temp5952 < _temp5950.base? 1: _temp5952 >= _temp5950.last_plus_one){
_throw( Null_Exception);}* _temp5952;})); _temp5949;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5953= yyls; struct Cyc_Yyltype* _temp5955= _temp5953.curr
+( yylsp_offset - 2); if( _temp5955 < _temp5953.base? 1: _temp5955 >= _temp5953.last_plus_one){
_throw( Null_Exception);}* _temp5955;}).first_line,({ struct _tagged_ptr3
_temp5956= yyls; struct Cyc_Yyltype* _temp5958= _temp5956.curr + yylsp_offset;
if( _temp5958 < _temp5956.base? 1: _temp5958 >= _temp5956.last_plus_one){ _throw(
Null_Exception);}* _temp5958;}).last_line))};( struct _xenum_struct*) _temp5945;});
break; case 351: _LL5944: { struct _tuple1* q= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp5960= yyvs; struct _xenum_struct** _temp5962= _temp5960.curr +
yyvsp_offset; if( _temp5962 < _temp5960.base? 1: _temp5962 >= _temp5960.last_plus_one){
_throw( Null_Exception);}* _temp5962;}));{ void* _temp5963=(* q).f1; struct Cyc_List_List*
_temp5973; struct Cyc_List_List* _temp5975; _LL5965: if( _temp5963 == Cyc_Absyn_Loc_n){
goto _LL5966;} else{ goto _LL5967;} _LL5967: if(( unsigned int) _temp5963 > 1u?((
struct _enum_struct*) _temp5963)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL5974:
_temp5973=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp5963)->f1;
if( _temp5973 == 0){ goto _LL5968;} else{ goto _LL5969;}} else{ goto _LL5969;}
_LL5969: if(( unsigned int) _temp5963 > 1u?(( struct _enum_struct*) _temp5963)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL5976: _temp5975=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp5963)->f1; if( _temp5975 == 0){ goto _LL5970;}
else{ goto _LL5971;}} else{ goto _LL5971;} _LL5971: goto _LL5972; _LL5966: goto
_LL5964; _LL5968: goto _LL5964; _LL5970: goto _LL5964; _LL5972: Cyc_Parse_err((
struct _tagged_string)({ char* _temp5977=( char*)"struct field is qualified with module name";
struct _tagged_string _temp5978; _temp5978.curr= _temp5977; _temp5978.base=
_temp5977; _temp5978.last_plus_one= _temp5977 + 43; _temp5978;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5979= yyls; struct Cyc_Yyltype* _temp5981= _temp5979.curr
+ yylsp_offset; if( _temp5981 < _temp5979.base? 1: _temp5981 >= _temp5979.last_plus_one){
_throw( Null_Exception);}* _temp5981;}).first_line,({ struct _tagged_ptr3
_temp5982= yyls; struct Cyc_Yyltype* _temp5984= _temp5982.curr + yylsp_offset;
if( _temp5984 < _temp5982.base? 1: _temp5984 >= _temp5982.last_plus_one){ _throw(
Null_Exception);}* _temp5984;}).last_line)); goto _LL5964; _LL5964:;} yyval=({
struct Cyc_Exp_tok_struct* _temp5985=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct));* _temp5985=( struct Cyc_Exp_tok_struct){.tag=
Cyc_Exp_tok_tag,.f1= Cyc_Absyn_structarrow_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5986= yyvs; struct _xenum_struct** _temp5988= _temp5986.curr +(
yyvsp_offset - 2); if( _temp5988 < _temp5986.base? 1: _temp5988 >= _temp5986.last_plus_one){
_throw( Null_Exception);}* _temp5988;})),(* q).f2, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5989= yyls; struct Cyc_Yyltype* _temp5991= _temp5989.curr
+( yylsp_offset - 2); if( _temp5991 < _temp5989.base? 1: _temp5991 >= _temp5989.last_plus_one){
_throw( Null_Exception);}* _temp5991;}).first_line,({ struct _tagged_ptr3
_temp5992= yyls; struct Cyc_Yyltype* _temp5994= _temp5992.curr + yylsp_offset;
if( _temp5994 < _temp5992.base? 1: _temp5994 >= _temp5992.last_plus_one){ _throw(
Null_Exception);}* _temp5994;}).last_line))};( struct _xenum_struct*) _temp5985;});
break;} case 352: _LL5959: yyval=({ struct Cyc_Exp_tok_struct* _temp5996=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5996=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_post_inc_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5997= yyvs; struct _xenum_struct**
_temp5999= _temp5997.curr +( yyvsp_offset - 1); if( _temp5999 < _temp5997.base?
1: _temp5999 >= _temp5997.last_plus_one){ _throw( Null_Exception);}* _temp5999;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6000= yyls; struct Cyc_Yyltype*
_temp6002= _temp6000.curr +( yylsp_offset - 1); if( _temp6002 < _temp6000.base?
1: _temp6002 >= _temp6000.last_plus_one){ _throw( Null_Exception);}* _temp6002;}).first_line,({
struct _tagged_ptr3 _temp6003= yyls; struct Cyc_Yyltype* _temp6005= _temp6003.curr
+ yylsp_offset; if( _temp6005 < _temp6003.base? 1: _temp6005 >= _temp6003.last_plus_one){
_throw( Null_Exception);}* _temp6005;}).last_line))};( struct _xenum_struct*)
_temp5996;}); break; case 353: _LL5995: yyval=({ struct Cyc_Exp_tok_struct*
_temp6007=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp6007=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_post_dec_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6008= yyvs; struct _xenum_struct**
_temp6010= _temp6008.curr +( yyvsp_offset - 1); if( _temp6010 < _temp6008.base?
1: _temp6010 >= _temp6008.last_plus_one){ _throw( Null_Exception);}* _temp6010;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6011= yyls; struct Cyc_Yyltype*
_temp6013= _temp6011.curr +( yylsp_offset - 1); if( _temp6013 < _temp6011.base?
1: _temp6013 >= _temp6011.last_plus_one){ _throw( Null_Exception);}* _temp6013;}).first_line,({
struct _tagged_ptr3 _temp6014= yyls; struct Cyc_Yyltype* _temp6016= _temp6014.curr
+ yylsp_offset; if( _temp6016 < _temp6014.base? 1: _temp6016 >= _temp6014.last_plus_one){
_throw( Null_Exception);}* _temp6016;}).last_line))};( struct _xenum_struct*)
_temp6007;}); break; case 354: _LL6006: yyval=({ struct Cyc_Exp_tok_struct*
_temp6018=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp6018=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_CompoundLit_e_struct* _temp6019=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp6019->tag= Cyc_Absyn_CompoundLit_e_tag;
_temp6019->f1= Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp6020= yyvs;
struct _xenum_struct** _temp6022= _temp6020.curr +( yyvsp_offset - 4); if(
_temp6022 < _temp6020.base? 1: _temp6022 >= _temp6020.last_plus_one){ _throw(
Null_Exception);}* _temp6022;})); _temp6019->f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp6023= yyvs; struct _xenum_struct** _temp6025= _temp6023.curr
+( yyvsp_offset - 1); if( _temp6025 < _temp6023.base? 1: _temp6025 >= _temp6023.last_plus_one){
_throw( Null_Exception);}* _temp6025;})));( void*) _temp6019;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6026= yyls; struct Cyc_Yyltype* _temp6028= _temp6026.curr
+( yylsp_offset - 5); if( _temp6028 < _temp6026.base? 1: _temp6028 >= _temp6026.last_plus_one){
_throw( Null_Exception);}* _temp6028;}).first_line,({ struct _tagged_ptr3
_temp6029= yyls; struct Cyc_Yyltype* _temp6031= _temp6029.curr + yylsp_offset;
if( _temp6031 < _temp6029.base? 1: _temp6031 >= _temp6029.last_plus_one){ _throw(
Null_Exception);}* _temp6031;}).last_line))};( struct _xenum_struct*) _temp6018;});
break; case 355: _LL6017: yyval=({ struct Cyc_Exp_tok_struct* _temp6033=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp6033=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_CompoundLit_e_struct* _temp6034=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp6034->tag= Cyc_Absyn_CompoundLit_e_tag;
_temp6034->f1= Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp6035= yyvs;
struct _xenum_struct** _temp6037= _temp6035.curr +( yyvsp_offset - 5); if(
_temp6037 < _temp6035.base? 1: _temp6037 >= _temp6035.last_plus_one){ _throw(
Null_Exception);}* _temp6037;})); _temp6034->f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp6038= yyvs; struct _xenum_struct** _temp6040= _temp6038.curr
+( yyvsp_offset - 2); if( _temp6040 < _temp6038.base? 1: _temp6040 >= _temp6038.last_plus_one){
_throw( Null_Exception);}* _temp6040;})));( void*) _temp6034;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6041= yyls; struct Cyc_Yyltype* _temp6043= _temp6041.curr
+( yylsp_offset - 6); if( _temp6043 < _temp6041.base? 1: _temp6043 >= _temp6041.last_plus_one){
_throw( Null_Exception);}* _temp6043;}).first_line,({ struct _tagged_ptr3
_temp6044= yyls; struct Cyc_Yyltype* _temp6046= _temp6044.curr + yylsp_offset;
if( _temp6046 < _temp6044.base? 1: _temp6046 >= _temp6044.last_plus_one){ _throw(
Null_Exception);}* _temp6046;}).last_line))};( struct _xenum_struct*) _temp6033;});
break; case 356: _LL6032: yyval=({ struct Cyc_Exp_tok_struct* _temp6048=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp6048=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_Array_e_struct* _temp6049=( struct Cyc_Absyn_Array_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp6049->tag= Cyc_Absyn_Array_e_tag;
_temp6049->f1= 1; _temp6049->f2= 0;( void*) _temp6049;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6050= yyls; struct Cyc_Yyltype* _temp6052= _temp6050.curr
+( yylsp_offset - 2); if( _temp6052 < _temp6050.base? 1: _temp6052 >= _temp6050.last_plus_one){
_throw( Null_Exception);}* _temp6052;}).first_line,({ struct _tagged_ptr3
_temp6053= yyls; struct Cyc_Yyltype* _temp6055= _temp6053.curr + yylsp_offset;
if( _temp6055 < _temp6053.base? 1: _temp6055 >= _temp6053.last_plus_one){ _throw(
Null_Exception);}* _temp6055;}).last_line))};( struct _xenum_struct*) _temp6048;});
break; case 357: _LL6047: yyval=({ struct Cyc_Exp_tok_struct* _temp6057=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp6057=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_Array_e_struct* _temp6058=( struct Cyc_Absyn_Array_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp6058->tag= Cyc_Absyn_Array_e_tag;
_temp6058->f1= 1; _temp6058->f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({ struct _tagged_ptr2
_temp6059= yyvs; struct _xenum_struct** _temp6061= _temp6059.curr +(
yyvsp_offset - 1); if( _temp6061 < _temp6059.base? 1: _temp6061 >= _temp6059.last_plus_one){
_throw( Null_Exception);}* _temp6061;})));( void*) _temp6058;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6062= yyls; struct Cyc_Yyltype* _temp6064= _temp6062.curr
+( yylsp_offset - 3); if( _temp6064 < _temp6062.base? 1: _temp6064 >= _temp6062.last_plus_one){
_throw( Null_Exception);}* _temp6064;}).first_line,({ struct _tagged_ptr3
_temp6065= yyls; struct Cyc_Yyltype* _temp6067= _temp6065.curr +( yylsp_offset -
1); if( _temp6067 < _temp6065.base? 1: _temp6067 >= _temp6065.last_plus_one){
_throw( Null_Exception);}* _temp6067;}).last_line))};( struct _xenum_struct*)
_temp6057;}); break; case 358: _LL6056: yyval=({ struct Cyc_Exp_tok_struct*
_temp6069=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp6069=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_Array_e_struct* _temp6070=( struct Cyc_Absyn_Array_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp6070->tag= Cyc_Absyn_Array_e_tag;
_temp6070->f1= 1; _temp6070->f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({ struct _tagged_ptr2
_temp6071= yyvs; struct _xenum_struct** _temp6073= _temp6071.curr +(
yyvsp_offset - 2); if( _temp6073 < _temp6071.base? 1: _temp6073 >= _temp6071.last_plus_one){
_throw( Null_Exception);}* _temp6073;})));( void*) _temp6070;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6074= yyls; struct Cyc_Yyltype* _temp6076= _temp6074.curr
+( yylsp_offset - 4); if( _temp6076 < _temp6074.base? 1: _temp6076 >= _temp6074.last_plus_one){
_throw( Null_Exception);}* _temp6076;}).first_line,({ struct _tagged_ptr3
_temp6077= yyls; struct Cyc_Yyltype* _temp6079= _temp6077.curr +( yylsp_offset -
1); if( _temp6079 < _temp6077.base? 1: _temp6079 >= _temp6077.last_plus_one){
_throw( Null_Exception);}* _temp6079;}).last_line))};( struct _xenum_struct*)
_temp6069;}); break; case 359: _LL6068: yyval=({ struct Cyc_Exp_tok_struct*
_temp6081=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp6081=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_Comprehension_e_struct* _temp6082=( struct Cyc_Absyn_Comprehension_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Comprehension_e_struct)); _temp6082->tag=
Cyc_Absyn_Comprehension_e_tag; _temp6082->f1= Cyc_Absyn_new_vardecl(({ struct
_tuple1* _temp6083=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp6083->f1= Cyc_Absyn_Loc_n; _temp6083->f2=({ struct _tagged_string*
_temp6084=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp6084[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp6085= yyvs;
struct _xenum_struct** _temp6087= _temp6085.curr +( yyvsp_offset - 5); if(
_temp6087 < _temp6085.base? 1: _temp6087 >= _temp6085.last_plus_one){ _throw(
Null_Exception);}* _temp6087;})); _temp6084;}); _temp6083;}), Cyc_Absyn_uint_t,(
struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp( 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6088= yyls; struct Cyc_Yyltype* _temp6090= _temp6088.curr
+( yylsp_offset - 5); if( _temp6090 < _temp6088.base? 1: _temp6090 >= _temp6088.last_plus_one){
_throw( Null_Exception);}* _temp6090;}).first_line,({ struct _tagged_ptr3
_temp6091= yyls; struct Cyc_Yyltype* _temp6093= _temp6091.curr +( yylsp_offset -
5); if( _temp6093 < _temp6091.base? 1: _temp6093 >= _temp6091.last_plus_one){
_throw( Null_Exception);}* _temp6093;}).last_line))); _temp6082->f2= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6094= yyvs; struct _xenum_struct** _temp6096= _temp6094.curr
+( yyvsp_offset - 3); if( _temp6096 < _temp6094.base? 1: _temp6096 >= _temp6094.last_plus_one){
_throw( Null_Exception);}* _temp6096;})); _temp6082->f3= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6097= yyvs; struct _xenum_struct** _temp6099= _temp6097.curr
+( yyvsp_offset - 1); if( _temp6099 < _temp6097.base? 1: _temp6099 >= _temp6097.last_plus_one){
_throw( Null_Exception);}* _temp6099;}));( void*) _temp6082;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6100= yyls; struct Cyc_Yyltype* _temp6102= _temp6100.curr
+( yylsp_offset - 8); if( _temp6102 < _temp6100.base? 1: _temp6102 >= _temp6100.last_plus_one){
_throw( Null_Exception);}* _temp6102;}).first_line,({ struct _tagged_ptr3
_temp6103= yyls; struct Cyc_Yyltype* _temp6105= _temp6103.curr + yylsp_offset;
if( _temp6105 < _temp6103.base? 1: _temp6105 >= _temp6103.last_plus_one){ _throw(
Null_Exception);}* _temp6105;}).last_line))};( struct _xenum_struct*) _temp6081;});
break; case 360: _LL6080: yyval=({ struct Cyc_Exp_tok_struct* _temp6107=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp6107=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_string_exp( 1,
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp6108= yyvs; struct
_xenum_struct** _temp6110= _temp6108.curr + yyvsp_offset; if( _temp6110 <
_temp6108.base? 1: _temp6110 >= _temp6108.last_plus_one){ _throw( Null_Exception);}*
_temp6110;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6111=
yyls; struct Cyc_Yyltype* _temp6113= _temp6111.curr +( yylsp_offset - 1); if(
_temp6113 < _temp6111.base? 1: _temp6113 >= _temp6111.last_plus_one){ _throw(
Null_Exception);}* _temp6113;}).first_line,({ struct _tagged_ptr3 _temp6114=
yyls; struct Cyc_Yyltype* _temp6116= _temp6114.curr + yylsp_offset; if(
_temp6116 < _temp6114.base? 1: _temp6116 >= _temp6114.last_plus_one){ _throw(
Null_Exception);}* _temp6116;}).last_line))};( struct _xenum_struct*) _temp6107;});
break; case 361: _LL6106: yyval=({ struct Cyc_Exp_tok_struct* _temp6118=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp6118=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_Fill_e_struct* _temp6119=( struct Cyc_Absyn_Fill_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fill_e_struct)); _temp6119->tag= Cyc_Absyn_Fill_e_tag;
_temp6119->f1= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6120= yyvs; struct
_xenum_struct** _temp6122= _temp6120.curr +( yyvsp_offset - 1); if( _temp6122 <
_temp6120.base? 1: _temp6122 >= _temp6120.last_plus_one){ _throw( Null_Exception);}*
_temp6122;}));( void*) _temp6119;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6123= yyls; struct Cyc_Yyltype* _temp6125= _temp6123.curr +(
yylsp_offset - 3); if( _temp6125 < _temp6123.base? 1: _temp6125 >= _temp6123.last_plus_one){
_throw( Null_Exception);}* _temp6125;}).first_line,({ struct _tagged_ptr3
_temp6126= yyls; struct Cyc_Yyltype* _temp6128= _temp6126.curr + yylsp_offset;
if( _temp6128 < _temp6126.base? 1: _temp6128 >= _temp6126.last_plus_one){ _throw(
Null_Exception);}* _temp6128;}).last_line))};( struct _xenum_struct*) _temp6118;});
break; case 362: _LL6117: yyval=({ struct Cyc_Exp_tok_struct* _temp6130=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp6130=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_Codegen_e_struct* _temp6131=( struct Cyc_Absyn_Codegen_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Codegen_e_struct)); _temp6131->tag= Cyc_Absyn_Codegen_e_tag;
_temp6131->f1= Cyc_yyget_FnDecl_tok(({ struct _tagged_ptr2 _temp6132= yyvs;
struct _xenum_struct** _temp6134= _temp6132.curr +( yyvsp_offset - 1); if(
_temp6134 < _temp6132.base? 1: _temp6134 >= _temp6132.last_plus_one){ _throw(
Null_Exception);}* _temp6134;}));( void*) _temp6131;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6135= yyls; struct Cyc_Yyltype* _temp6137= _temp6135.curr
+( yylsp_offset - 3); if( _temp6137 < _temp6135.base? 1: _temp6137 >= _temp6135.last_plus_one){
_throw( Null_Exception);}* _temp6137;}).first_line,({ struct _tagged_ptr3
_temp6138= yyls; struct Cyc_Yyltype* _temp6140= _temp6138.curr + yylsp_offset;
if( _temp6140 < _temp6138.base? 1: _temp6140 >= _temp6138.last_plus_one){ _throw(
Null_Exception);}* _temp6140;}).last_line))};( struct _xenum_struct*) _temp6130;});
break; case 363: _LL6129: yyval=({ struct Cyc_Exp_tok_struct* _temp6142=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp6142=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_UnknownId_e_struct* _temp6143=( struct Cyc_Absyn_UnknownId_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp6143->tag= Cyc_Absyn_UnknownId_e_tag;
_temp6143->f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp6144= yyvs;
struct _xenum_struct** _temp6146= _temp6144.curr + yyvsp_offset; if( _temp6146 <
_temp6144.base? 1: _temp6146 >= _temp6144.last_plus_one){ _throw( Null_Exception);}*
_temp6146;}));( void*) _temp6143;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6147= yyls; struct Cyc_Yyltype* _temp6149= _temp6147.curr +
yylsp_offset; if( _temp6149 < _temp6147.base? 1: _temp6149 >= _temp6147.last_plus_one){
_throw( Null_Exception);}* _temp6149;}).first_line,({ struct _tagged_ptr3
_temp6150= yyls; struct Cyc_Yyltype* _temp6152= _temp6150.curr + yylsp_offset;
if( _temp6152 < _temp6150.base? 1: _temp6152 >= _temp6150.last_plus_one){ _throw(
Null_Exception);}* _temp6152;}).last_line))};( struct _xenum_struct*) _temp6142;});
break; case 364: _LL6141: yyval=({ struct _tagged_ptr2 _temp6154= yyvs; struct
_xenum_struct** _temp6156= _temp6154.curr + yyvsp_offset; if( _temp6156 <
_temp6154.base? 1: _temp6156 >= _temp6154.last_plus_one){ _throw( Null_Exception);}*
_temp6156;}); break; case 365: _LL6153: yyval=({ struct Cyc_Exp_tok_struct*
_temp6158=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp6158=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_string_exp(
0, Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp6159= yyvs; struct
_xenum_struct** _temp6161= _temp6159.curr + yyvsp_offset; if( _temp6161 <
_temp6159.base? 1: _temp6161 >= _temp6159.last_plus_one){ _throw( Null_Exception);}*
_temp6161;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6162=
yyls; struct Cyc_Yyltype* _temp6164= _temp6162.curr + yylsp_offset; if(
_temp6164 < _temp6162.base? 1: _temp6164 >= _temp6162.last_plus_one){ _throw(
Null_Exception);}* _temp6164;}).first_line,({ struct _tagged_ptr3 _temp6165=
yyls; struct Cyc_Yyltype* _temp6167= _temp6165.curr + yylsp_offset; if(
_temp6167 < _temp6165.base? 1: _temp6167 >= _temp6165.last_plus_one){ _throw(
Null_Exception);}* _temp6167;}).last_line))};( struct _xenum_struct*) _temp6158;});
break; case 366: _LL6157: yyval=({ struct _tagged_ptr2 _temp6169= yyvs; struct
_xenum_struct** _temp6171= _temp6169.curr +( yyvsp_offset - 1); if( _temp6171 <
_temp6169.base? 1: _temp6171 >= _temp6169.last_plus_one){ _throw( Null_Exception);}*
_temp6171;}); break; case 367: _LL6168: yyval=({ struct Cyc_Exp_tok_struct*
_temp6173=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp6173=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_noinstantiate_exp(
Cyc_Absyn_new_exp(({ struct Cyc_Absyn_UnknownId_e_struct* _temp6174=( struct Cyc_Absyn_UnknownId_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp6174->tag= Cyc_Absyn_UnknownId_e_tag;
_temp6174->f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp6175= yyvs;
struct _xenum_struct** _temp6177= _temp6175.curr +( yyvsp_offset - 1); if(
_temp6177 < _temp6175.base? 1: _temp6177 >= _temp6175.last_plus_one){ _throw(
Null_Exception);}* _temp6177;}));( void*) _temp6174;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6178= yyls; struct Cyc_Yyltype* _temp6180= _temp6178.curr
+( yylsp_offset - 1); if( _temp6180 < _temp6178.base? 1: _temp6180 >= _temp6178.last_plus_one){
_throw( Null_Exception);}* _temp6180;}).first_line,({ struct _tagged_ptr3
_temp6181= yyls; struct Cyc_Yyltype* _temp6183= _temp6181.curr +( yylsp_offset -
1); if( _temp6183 < _temp6181.base? 1: _temp6183 >= _temp6181.last_plus_one){
_throw( Null_Exception);}* _temp6183;}).last_line)), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6184= yyls; struct Cyc_Yyltype* _temp6186= _temp6184.curr
+( yylsp_offset - 1); if( _temp6186 < _temp6184.base? 1: _temp6186 >= _temp6184.last_plus_one){
_throw( Null_Exception);}* _temp6186;}).first_line,({ struct _tagged_ptr3
_temp6187= yyls; struct Cyc_Yyltype* _temp6189= _temp6187.curr + yylsp_offset;
if( _temp6189 < _temp6187.base? 1: _temp6189 >= _temp6187.last_plus_one){ _throw(
Null_Exception);}* _temp6189;}).last_line))};( struct _xenum_struct*) _temp6173;});
break; case 368: _LL6172: yyval=({ struct Cyc_Exp_tok_struct* _temp6191=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp6191=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_instantiate_exp(
Cyc_Absyn_new_exp(({ struct Cyc_Absyn_UnknownId_e_struct* _temp6192=( struct Cyc_Absyn_UnknownId_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp6192->tag= Cyc_Absyn_UnknownId_e_tag;
_temp6192->f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp6193= yyvs;
struct _xenum_struct** _temp6195= _temp6193.curr +( yyvsp_offset - 4); if(
_temp6195 < _temp6193.base? 1: _temp6195 >= _temp6193.last_plus_one){ _throw(
Null_Exception);}* _temp6195;}));( void*) _temp6192;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6196= yyls; struct Cyc_Yyltype* _temp6198= _temp6196.curr
+( yylsp_offset - 4); if( _temp6198 < _temp6196.base? 1: _temp6198 >= _temp6196.last_plus_one){
_throw( Null_Exception);}* _temp6198;}).first_line,({ struct _tagged_ptr3
_temp6199= yyls; struct Cyc_Yyltype* _temp6201= _temp6199.curr +( yylsp_offset -
4); if( _temp6201 < _temp6199.base? 1: _temp6201 >= _temp6199.last_plus_one){
_throw( Null_Exception);}* _temp6201;}).last_line)),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp6202= yyvs; struct _xenum_struct** _temp6204= _temp6202.curr +(
yyvsp_offset - 1); if( _temp6204 < _temp6202.base? 1: _temp6204 >= _temp6202.last_plus_one){
_throw( Null_Exception);}* _temp6204;}))), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6205= yyls; struct Cyc_Yyltype* _temp6207= _temp6205.curr +(
yylsp_offset - 4); if( _temp6207 < _temp6205.base? 1: _temp6207 >= _temp6205.last_plus_one){
_throw( Null_Exception);}* _temp6207;}).first_line,({ struct _tagged_ptr3
_temp6208= yyls; struct Cyc_Yyltype* _temp6210= _temp6208.curr + yylsp_offset;
if( _temp6210 < _temp6208.base? 1: _temp6210 >= _temp6208.last_plus_one){ _throw(
Null_Exception);}* _temp6210;}).last_line))};( struct _xenum_struct*) _temp6191;});
break; case 369: _LL6190: yyval=({ struct Cyc_Exp_tok_struct* _temp6212=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp6212=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_tuple_exp( Cyc_yyget_ExpList_tok(({
struct _tagged_ptr2 _temp6213= yyvs; struct _xenum_struct** _temp6215= _temp6213.curr
+( yyvsp_offset - 1); if( _temp6215 < _temp6213.base? 1: _temp6215 >= _temp6213.last_plus_one){
_throw( Null_Exception);}* _temp6215;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6216= yyls; struct Cyc_Yyltype* _temp6218= _temp6216.curr +(
yylsp_offset - 3); if( _temp6218 < _temp6216.base? 1: _temp6218 >= _temp6216.last_plus_one){
_throw( Null_Exception);}* _temp6218;}).first_line,({ struct _tagged_ptr3
_temp6219= yyls; struct Cyc_Yyltype* _temp6221= _temp6219.curr + yylsp_offset;
if( _temp6221 < _temp6219.base? 1: _temp6221 >= _temp6219.last_plus_one){ _throw(
Null_Exception);}* _temp6221;}).last_line))};( struct _xenum_struct*) _temp6212;});
break; case 370: _LL6211: yyval=({ struct Cyc_Exp_tok_struct* _temp6223=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp6223=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_Struct_e_struct* _temp6224=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp6224->tag= Cyc_Absyn_Struct_e_tag;
_temp6224->f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp6225= yyvs;
struct _xenum_struct** _temp6227= _temp6225.curr +( yyvsp_offset - 3); if(
_temp6227 < _temp6225.base? 1: _temp6227 >= _temp6225.last_plus_one){ _throw(
Null_Exception);}* _temp6227;})); _temp6224->f2= 0; _temp6224->f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp6228= yyvs; struct _xenum_struct** _temp6230= _temp6228.curr
+( yyvsp_offset - 1); if( _temp6230 < _temp6228.base? 1: _temp6230 >= _temp6228.last_plus_one){
_throw( Null_Exception);}* _temp6230;}))); _temp6224->f4= 0;( void*) _temp6224;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6231= yyls; struct Cyc_Yyltype*
_temp6233= _temp6231.curr +( yylsp_offset - 3); if( _temp6233 < _temp6231.base?
1: _temp6233 >= _temp6231.last_plus_one){ _throw( Null_Exception);}* _temp6233;}).first_line,({
struct _tagged_ptr3 _temp6234= yyls; struct Cyc_Yyltype* _temp6236= _temp6234.curr
+ yylsp_offset; if( _temp6236 < _temp6234.base? 1: _temp6236 >= _temp6234.last_plus_one){
_throw( Null_Exception);}* _temp6236;}).last_line))};( struct _xenum_struct*)
_temp6223;}); break; case 371: _LL6222: yyval=({ struct Cyc_Exp_tok_struct*
_temp6238=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp6238=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_stmt_exp(
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp6239= yyvs; struct _xenum_struct**
_temp6241= _temp6239.curr +( yyvsp_offset - 2); if( _temp6241 < _temp6239.base?
1: _temp6241 >= _temp6239.last_plus_one){ _throw( Null_Exception);}* _temp6241;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6242= yyls; struct Cyc_Yyltype*
_temp6244= _temp6242.curr +( yylsp_offset - 4); if( _temp6244 < _temp6242.base?
1: _temp6244 >= _temp6242.last_plus_one){ _throw( Null_Exception);}* _temp6244;}).first_line,({
struct _tagged_ptr3 _temp6245= yyls; struct Cyc_Yyltype* _temp6247= _temp6245.curr
+ yylsp_offset; if( _temp6247 < _temp6245.base? 1: _temp6247 >= _temp6245.last_plus_one){
_throw( Null_Exception);}* _temp6247;}).last_line))};( struct _xenum_struct*)
_temp6238;}); break; case 372: _LL6237: yyval=({ struct Cyc_ExpList_tok_struct*
_temp6249=( struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));*
_temp6249=( struct Cyc_ExpList_tok_struct){.tag= Cyc_ExpList_tok_tag,.f1=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ExpList_tok(({
struct _tagged_ptr2 _temp6250= yyvs; struct _xenum_struct** _temp6252= _temp6250.curr
+ yyvsp_offset; if( _temp6252 < _temp6250.base? 1: _temp6252 >= _temp6250.last_plus_one){
_throw( Null_Exception);}* _temp6252;})))};( struct _xenum_struct*) _temp6249;});
break; case 373: _LL6248: yyval=({ struct Cyc_ExpList_tok_struct* _temp6254=(
struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));*
_temp6254=( struct Cyc_ExpList_tok_struct){.tag= Cyc_ExpList_tok_tag,.f1=({
struct Cyc_List_List* _temp6255=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp6255->hd=( void*) Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6256= yyvs; struct _xenum_struct** _temp6258= _temp6256.curr +
yyvsp_offset; if( _temp6258 < _temp6256.base? 1: _temp6258 >= _temp6256.last_plus_one){
_throw( Null_Exception);}* _temp6258;})); _temp6255->tl= 0; _temp6255;})};(
struct _xenum_struct*) _temp6254;}); break; case 374: _LL6253: yyval=({ struct
Cyc_ExpList_tok_struct* _temp6260=( struct Cyc_ExpList_tok_struct*) GC_malloc(
sizeof( struct Cyc_ExpList_tok_struct));* _temp6260=( struct Cyc_ExpList_tok_struct){.tag=
Cyc_ExpList_tok_tag,.f1=({ struct Cyc_List_List* _temp6261=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp6261->hd=( void*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6265= yyvs; struct _xenum_struct** _temp6267= _temp6265.curr
+ yyvsp_offset; if( _temp6267 < _temp6265.base? 1: _temp6267 >= _temp6265.last_plus_one){
_throw( Null_Exception);}* _temp6267;})); _temp6261->tl= Cyc_yyget_ExpList_tok(({
struct _tagged_ptr2 _temp6262= yyvs; struct _xenum_struct** _temp6264= _temp6262.curr
+( yyvsp_offset - 2); if( _temp6264 < _temp6262.base? 1: _temp6264 >= _temp6262.last_plus_one){
_throw( Null_Exception);}* _temp6264;})); _temp6261;})};( struct _xenum_struct*)
_temp6260;}); break; case 375: _LL6259: yyval=({ struct Cyc_Exp_tok_struct*
_temp6269=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp6269=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_int_exp((*
Cyc_yyget_Int_tok(({ struct _tagged_ptr2 _temp6270= yyvs; struct _xenum_struct**
_temp6272= _temp6270.curr + yyvsp_offset; if( _temp6272 < _temp6270.base? 1:
_temp6272 >= _temp6270.last_plus_one){ _throw( Null_Exception);}* _temp6272;}))).f1,(*
Cyc_yyget_Int_tok(({ struct _tagged_ptr2 _temp6273= yyvs; struct _xenum_struct**
_temp6275= _temp6273.curr + yyvsp_offset; if( _temp6275 < _temp6273.base? 1:
_temp6275 >= _temp6273.last_plus_one){ _throw( Null_Exception);}* _temp6275;}))).f2,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6276= yyls; struct Cyc_Yyltype*
_temp6278= _temp6276.curr + yylsp_offset; if( _temp6278 < _temp6276.base? 1:
_temp6278 >= _temp6276.last_plus_one){ _throw( Null_Exception);}* _temp6278;}).first_line,({
struct _tagged_ptr3 _temp6279= yyls; struct Cyc_Yyltype* _temp6281= _temp6279.curr
+ yylsp_offset; if( _temp6281 < _temp6279.base? 1: _temp6281 >= _temp6279.last_plus_one){
_throw( Null_Exception);}* _temp6281;}).last_line))};( struct _xenum_struct*)
_temp6269;}); break; case 376: _LL6268: yyval=({ struct Cyc_Exp_tok_struct*
_temp6283=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp6283=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_char_exp(
Cyc_yyget_Char_tok(({ struct _tagged_ptr2 _temp6284= yyvs; struct _xenum_struct**
_temp6286= _temp6284.curr + yyvsp_offset; if( _temp6286 < _temp6284.base? 1:
_temp6286 >= _temp6284.last_plus_one){ _throw( Null_Exception);}* _temp6286;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6287= yyls; struct Cyc_Yyltype*
_temp6289= _temp6287.curr + yylsp_offset; if( _temp6289 < _temp6287.base? 1:
_temp6289 >= _temp6287.last_plus_one){ _throw( Null_Exception);}* _temp6289;}).first_line,({
struct _tagged_ptr3 _temp6290= yyls; struct Cyc_Yyltype* _temp6292= _temp6290.curr
+ yylsp_offset; if( _temp6292 < _temp6290.base? 1: _temp6292 >= _temp6290.last_plus_one){
_throw( Null_Exception);}* _temp6292;}).last_line))};( struct _xenum_struct*)
_temp6283;}); break; case 377: _LL6282: yyval=({ struct Cyc_Exp_tok_struct*
_temp6294=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp6294=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_float_exp(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp6295= yyvs; struct
_xenum_struct** _temp6297= _temp6295.curr + yyvsp_offset; if( _temp6297 <
_temp6295.base? 1: _temp6297 >= _temp6295.last_plus_one){ _throw( Null_Exception);}*
_temp6297;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6298=
yyls; struct Cyc_Yyltype* _temp6300= _temp6298.curr + yylsp_offset; if(
_temp6300 < _temp6298.base? 1: _temp6300 >= _temp6298.last_plus_one){ _throw(
Null_Exception);}* _temp6300;}).first_line,({ struct _tagged_ptr3 _temp6301=
yyls; struct Cyc_Yyltype* _temp6303= _temp6301.curr + yylsp_offset; if(
_temp6303 < _temp6301.base? 1: _temp6303 >= _temp6301.last_plus_one){ _throw(
Null_Exception);}* _temp6303;}).last_line))};( struct _xenum_struct*) _temp6294;});
break; case 378: _LL6293: yyval=({ struct Cyc_Exp_tok_struct* _temp6305=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp6305=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_null_exp( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6306= yyls; struct Cyc_Yyltype* _temp6308= _temp6306.curr
+ yylsp_offset; if( _temp6308 < _temp6306.base? 1: _temp6308 >= _temp6306.last_plus_one){
_throw( Null_Exception);}* _temp6308;}).first_line,({ struct _tagged_ptr3
_temp6309= yyls; struct Cyc_Yyltype* _temp6311= _temp6309.curr + yylsp_offset;
if( _temp6311 < _temp6309.base? 1: _temp6311 >= _temp6309.last_plus_one){ _throw(
Null_Exception);}* _temp6311;}).last_line))};( struct _xenum_struct*) _temp6305;});
break; case 379: _LL6304: yyval=({ struct Cyc_QualId_tok_struct* _temp6313=(
struct Cyc_QualId_tok_struct*) GC_malloc( sizeof( struct Cyc_QualId_tok_struct));*
_temp6313=( struct Cyc_QualId_tok_struct){.tag= Cyc_QualId_tok_tag,.f1=({ struct
_tuple1* _temp6314=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp6314->f1=({ struct Cyc_Absyn_Rel_n_struct* _temp6319=( struct Cyc_Absyn_Rel_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct)); _temp6319->tag= Cyc_Absyn_Rel_n_tag;
_temp6319->f1= 0;( void*) _temp6319;}); _temp6314->f2=({ struct _tagged_string*
_temp6315=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp6315[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp6316= yyvs;
struct _xenum_struct** _temp6318= _temp6316.curr + yyvsp_offset; if( _temp6318 <
_temp6316.base? 1: _temp6318 >= _temp6316.last_plus_one){ _throw( Null_Exception);}*
_temp6318;})); _temp6315;}); _temp6314;})};( struct _xenum_struct*) _temp6313;});
break; case 380: _LL6312: yyval=({ struct _tagged_ptr2 _temp6321= yyvs; struct
_xenum_struct** _temp6323= _temp6321.curr + yyvsp_offset; if( _temp6323 <
_temp6321.base? 1: _temp6323 >= _temp6321.last_plus_one){ _throw( Null_Exception);}*
_temp6323;}); break; default: _LL6320:( void) _throw(({ struct Cyc_Yyimpossible_struct*
_temp6325=( struct Cyc_Yyimpossible_struct*) GC_malloc_atomic( sizeof( struct
Cyc_Yyimpossible_struct));* _temp6325=( struct Cyc_Yyimpossible_struct){.tag=
Cyc_Yyimpossible_tag};( struct _xenum_struct*) _temp6325;}));} yyvsp_offset -=
yylen; yyssp_offset -= yylen; yylsp_offset -= yylen;({ struct _tagged_ptr2
_temp6326= yyvs; struct _xenum_struct** _temp6328= _temp6326.curr +( ++
yyvsp_offset); if( _temp6328 < _temp6326.base? 1: _temp6328 >= _temp6326.last_plus_one){
_throw( Null_Exception);}* _temp6328= yyval;}); yylsp_offset ++; if( yylen == 0){({
struct _tagged_ptr3 _temp6329= yyls; struct Cyc_Yyltype* _temp6331= _temp6329.curr
+ yylsp_offset; if( _temp6331 < _temp6329.base? 1: _temp6331 >= _temp6329.last_plus_one){
_throw( Null_Exception);}(* _temp6331).first_line= Cyc_yylloc.first_line;});({
struct _tagged_ptr3 _temp6332= yyls; struct Cyc_Yyltype* _temp6334= _temp6332.curr
+ yylsp_offset; if( _temp6334 < _temp6332.base? 1: _temp6334 >= _temp6332.last_plus_one){
_throw( Null_Exception);}(* _temp6334).first_column= Cyc_yylloc.first_column;});({
struct _tagged_ptr3 _temp6335= yyls; struct Cyc_Yyltype* _temp6337= _temp6335.curr
+ yylsp_offset; if( _temp6337 < _temp6335.base? 1: _temp6337 >= _temp6335.last_plus_one){
_throw( Null_Exception);}(* _temp6337).last_line=({ struct _tagged_ptr3
_temp6338= yyls; struct Cyc_Yyltype* _temp6340= _temp6338.curr +( yylsp_offset -
1); if( _temp6340 < _temp6338.base? 1: _temp6340 >= _temp6338.last_plus_one){
_throw( Null_Exception);}* _temp6340;}).last_line;});({ struct _tagged_ptr3
_temp6341= yyls; struct Cyc_Yyltype* _temp6343= _temp6341.curr + yylsp_offset;
if( _temp6343 < _temp6341.base? 1: _temp6343 >= _temp6341.last_plus_one){ _throw(
Null_Exception);}(* _temp6343).last_column=({ struct _tagged_ptr3 _temp6344=
yyls; struct Cyc_Yyltype* _temp6346= _temp6344.curr +( yylsp_offset - 1); if(
_temp6346 < _temp6344.base? 1: _temp6346 >= _temp6344.last_plus_one){ _throw(
Null_Exception);}* _temp6346;}).last_column;});({ struct _tagged_ptr3 _temp6347=
yyls; struct Cyc_Yyltype* _temp6349= _temp6347.curr + yylsp_offset; if(
_temp6349 < _temp6347.base? 1: _temp6349 >= _temp6347.last_plus_one){ _throw(
Null_Exception);}(* _temp6349).text=( struct _tagged_string)({ char* _temp6350=(
char*)""; struct _tagged_string _temp6351; _temp6351.curr= _temp6350; _temp6351.base=
_temp6350; _temp6351.last_plus_one= _temp6350 + 1; _temp6351;});});} else{({
struct _tagged_ptr3 _temp6352= yyls; struct Cyc_Yyltype* _temp6354= _temp6352.curr
+ yylsp_offset; if( _temp6354 < _temp6352.base? 1: _temp6354 >= _temp6352.last_plus_one){
_throw( Null_Exception);}(* _temp6354).last_line=({ struct _tagged_ptr3
_temp6355= yyls; struct Cyc_Yyltype* _temp6357= _temp6355.curr +(( yylsp_offset
+ yylen) - 1); if( _temp6357 < _temp6355.base? 1: _temp6357 >= _temp6355.last_plus_one){
_throw( Null_Exception);}* _temp6357;}).last_line;});({ struct _tagged_ptr3
_temp6358= yyls; struct Cyc_Yyltype* _temp6360= _temp6358.curr + yylsp_offset;
if( _temp6360 < _temp6358.base? 1: _temp6360 >= _temp6358.last_plus_one){ _throw(
Null_Exception);}(* _temp6360).last_column=({ struct _tagged_ptr3 _temp6361=
yyls; struct Cyc_Yyltype* _temp6363= _temp6361.curr +(( yylsp_offset + yylen) -
1); if( _temp6363 < _temp6361.base? 1: _temp6363 >= _temp6361.last_plus_one){
_throw( Null_Exception);}* _temp6363;}).last_column;});} yyn=( int)({ short*
_temp6364=( short*) Cyc_yyr1; unsigned int _temp6365= yyn; if( _temp6365 >= 381u){
_throw( Null_Exception);} _temp6364[ _temp6365];}); yystate=( int)({ short*
_temp6366=( short*) Cyc_yypgoto; unsigned int _temp6367= yyn - 119; if(
_temp6367 >= 102u){ _throw( Null_Exception);} _temp6366[ _temp6367];}) +( int)({
struct _tagged_ptr1 _temp6368= yyss; short* _temp6370= _temp6368.curr +
yyssp_offset; if( _temp6370 < _temp6368.base? 1: _temp6370 >= _temp6368.last_plus_one){
_throw( Null_Exception);}* _temp6370;}); if(( yystate >= 0? yystate <= 4281: 0)?(
int)({ short* _temp6371=( short*) Cyc_yycheck; unsigned int _temp6372= yystate;
if( _temp6372 >= 4282u){ _throw( Null_Exception);} _temp6371[ _temp6372];}) ==(
int)({ struct _tagged_ptr1 _temp6373= yyss; short* _temp6375= _temp6373.curr +
yyssp_offset; if( _temp6375 < _temp6373.base? 1: _temp6375 >= _temp6373.last_plus_one){
_throw( Null_Exception);}* _temp6375;}): 0){ yystate=( int)({ short* _temp6376=(
short*) Cyc_yytable; unsigned int _temp6377= yystate; if( _temp6377 >= 4282u){
_throw( Null_Exception);} _temp6376[ _temp6377];});} else{ yystate=( int)({
short* _temp6378=( short*) Cyc_yydefgoto; unsigned int _temp6379= yyn - 119; if(
_temp6379 >= 102u){ _throw( Null_Exception);} _temp6378[ _temp6379];});} goto
yynewstate; yyerrlab: if( yyerrstatus == 0){ ++ Cyc_yynerrs; yyn=( int)({ short*
_temp6380=( short*) Cyc_yypact; unsigned int _temp6381= yystate; if( _temp6381
>= 715u){ _throw( Null_Exception);} _temp6380[ _temp6381];}); if( yyn > - 32768?
yyn < 4281: 0){ int sze= 0; struct _tagged_string msg; int x; int count; count=
0; for( x= yyn < 0? - yyn: 0;( unsigned int) x < 221u / sizeof( char*); x ++){
if(( int)({ short* _temp6382=( short*) Cyc_yycheck; unsigned int _temp6383= x +
yyn; if( _temp6383 >= 4282u){ _throw( Null_Exception);} _temp6382[ _temp6383];})
== x){( unsigned int)( sze += Cyc_String_strlen(({ struct _tagged_string*
_temp6384=( struct _tagged_string*) Cyc_yytname; unsigned int _temp6385= x; if(
_temp6385 >= 221u){ _throw( Null_Exception);} _temp6384[ _temp6385];})) +(
unsigned int) 15), count ++;}} msg=({ unsigned int _temp6386=( unsigned int)(
sze + 15); char* _temp6387=( char*) GC_malloc_atomic( sizeof( char) * _temp6386);
unsigned int i; struct _tagged_string _temp6388={ _temp6387, _temp6387,
_temp6387 + _temp6386}; for( i= 0; i < _temp6386; i ++){ _temp6387[ i]='\000';}
_temp6388;}); Cyc_String_strcpy( msg,( struct _tagged_string)({ char* _temp6389=(
char*)"parse error"; struct _tagged_string _temp6390; _temp6390.curr= _temp6389;
_temp6390.base= _temp6389; _temp6390.last_plus_one= _temp6389 + 12; _temp6390;}));
if( count < 5){ count= 0; for( x= yyn < 0? - yyn: 0;( unsigned int) x < 221u /
sizeof( char*); x ++){ if(( int)({ short* _temp6391=( short*) Cyc_yycheck;
unsigned int _temp6392= x + yyn; if( _temp6392 >= 4282u){ _throw( Null_Exception);}
_temp6391[ _temp6392];}) == x){ Cyc_String_strcat( msg, count == 0?( struct
_tagged_string)({ char* _temp6393=( char*)", expecting `"; struct _tagged_string
_temp6394; _temp6394.curr= _temp6393; _temp6394.base= _temp6393; _temp6394.last_plus_one=
_temp6393 + 14; _temp6394;}):( struct _tagged_string)({ char* _temp6395=( char*)" or `";
struct _tagged_string _temp6396; _temp6396.curr= _temp6395; _temp6396.base=
_temp6395; _temp6396.last_plus_one= _temp6395 + 6; _temp6396;})); Cyc_String_strcat(
msg,({ struct _tagged_string* _temp6397=( struct _tagged_string*) Cyc_yytname;
unsigned int _temp6398= x; if( _temp6398 >= 221u){ _throw( Null_Exception);}
_temp6397[ _temp6398];})); Cyc_String_strcat( msg,( struct _tagged_string)({
char* _temp6399=( char*)"'"; struct _tagged_string _temp6400; _temp6400.curr=
_temp6399; _temp6400.base= _temp6399; _temp6400.last_plus_one= _temp6399 + 2;
_temp6400;})); count ++;}}} Cyc_yyerror( msg);} else{ Cyc_yyerror(( struct
_tagged_string)({ char* _temp6401=( char*)"parse error"; struct _tagged_string
_temp6402; _temp6402.curr= _temp6401; _temp6402.base= _temp6401; _temp6402.last_plus_one=
_temp6401 + 12; _temp6402;}));}} goto yyerrlab1; yyerrlab1: if( yyerrstatus == 3){
if( Cyc_yychar == 0){ return 1;} Cyc_yychar= - 2;} yyerrstatus= 3; goto
yyerrhandle; yyerrdefault: yyerrpop: if( yyssp_offset == 0){ return 1;}
yyvsp_offset --; yystate=( int)({ struct _tagged_ptr1 _temp6403= yyss; short*
_temp6405= _temp6403.curr +( -- yyssp_offset); if( _temp6405 < _temp6403.base? 1:
_temp6405 >= _temp6403.last_plus_one){ _throw( Null_Exception);}* _temp6405;});
yylsp_offset --; yyerrhandle: yyn=( int)({ short* _temp6406=( short*) Cyc_yypact;
unsigned int _temp6407= yystate; if( _temp6407 >= 715u){ _throw( Null_Exception);}
_temp6406[ _temp6407];}); if( yyn == - 32768){ goto yyerrdefault;} yyn += 1; if((
yyn < 0? 1: yyn > 4281)? 1:( int)({ short* _temp6408=( short*) Cyc_yycheck;
unsigned int _temp6409= yyn; if( _temp6409 >= 4282u){ _throw( Null_Exception);}
_temp6408[ _temp6409];}) != 1){ goto yyerrdefault;} yyn=( int)({ short*
_temp6410=( short*) Cyc_yytable; unsigned int _temp6411= yyn; if( _temp6411 >=
4282u){ _throw( Null_Exception);} _temp6410[ _temp6411];}); if( yyn < 0){ if(
yyn == - 32768){ goto yyerrpop;} yyn= - yyn; goto yyreduce;} else{ if( yyn == 0){
goto yyerrpop;}} if( yyn == 714){ return 0;}({ struct _tagged_ptr2 _temp6412=
yyvs; struct _xenum_struct** _temp6414= _temp6412.curr +( ++ yyvsp_offset); if(
_temp6414 < _temp6412.base? 1: _temp6414 >= _temp6412.last_plus_one){ _throw(
Null_Exception);}* _temp6414= Cyc_yylval;});({ struct _tagged_ptr3 _temp6415=
yyls; struct Cyc_Yyltype* _temp6417= _temp6415.curr +( ++ yylsp_offset); if(
_temp6417 < _temp6415.base? 1: _temp6417 >= _temp6415.last_plus_one){ _throw(
Null_Exception);}* _temp6417= Cyc_yylloc;}); goto yynewstate;} void Cyc_yyprint(
int i, struct _xenum_struct* v){ struct _xenum_struct* _temp6418= v; struct
_tuple11* _temp6438; struct _tuple11 _temp6440; int _temp6441; void* _temp6443;
char _temp6445; short _temp6447; struct _tagged_string _temp6449; struct Cyc_Core_Opt*
_temp6451; struct Cyc_Core_Opt* _temp6453; struct Cyc_Core_Opt _temp6455; struct
_tagged_string* _temp6456; struct _tuple1* _temp6458; struct _tuple1 _temp6460;
struct _tagged_string* _temp6461; void* _temp6463; _LL6420: if( _temp6418->tag
== Cyc_Okay_tok_tag){ goto _LL6421;} else{ goto _LL6422;} _LL6422: if((*((
struct _xenum_struct*) _temp6418)).tag == Cyc_Int_tok_tag){ _LL6439: _temp6438=((
struct Cyc_Int_tok_struct*) _temp6418)->f1; _temp6440=* _temp6438; _LL6444:
_temp6443= _temp6440.f1; goto _LL6442; _LL6442: _temp6441= _temp6440.f2; goto
_LL6423;} else{ goto _LL6424;} _LL6424: if((*(( struct _xenum_struct*) _temp6418)).tag
== Cyc_Char_tok_tag){ _LL6446: _temp6445=(( struct Cyc_Char_tok_struct*)
_temp6418)->f1; goto _LL6425;} else{ goto _LL6426;} _LL6426: if((*(( struct
_xenum_struct*) _temp6418)).tag == Cyc_Short_tok_tag){ _LL6448: _temp6447=((
struct Cyc_Short_tok_struct*) _temp6418)->f1; goto _LL6427;} else{ goto _LL6428;}
_LL6428: if((*(( struct _xenum_struct*) _temp6418)).tag == Cyc_String_tok_tag){
_LL6450: _temp6449=(( struct Cyc_String_tok_struct*) _temp6418)->f1; goto
_LL6429;} else{ goto _LL6430;} _LL6430: if((*(( struct _xenum_struct*) _temp6418)).tag
== Cyc_Stringopt_tok_tag){ _LL6452: _temp6451=(( struct Cyc_Stringopt_tok_struct*)
_temp6418)->f1; if( _temp6451 == 0){ goto _LL6431;} else{ goto _LL6432;}} else{
goto _LL6432;} _LL6432: if((*(( struct _xenum_struct*) _temp6418)).tag == Cyc_Stringopt_tok_tag){
_LL6454: _temp6453=(( struct Cyc_Stringopt_tok_struct*) _temp6418)->f1; if(
_temp6453 == 0){ goto _LL6434;} else{ _temp6455=* _temp6453; _LL6457: _temp6456=(
struct _tagged_string*) _temp6455.v; goto _LL6433;}} else{ goto _LL6434;}
_LL6434: if((*(( struct _xenum_struct*) _temp6418)).tag == Cyc_QualId_tok_tag){
_LL6459: _temp6458=(( struct Cyc_QualId_tok_struct*) _temp6418)->f1; _temp6460=*
_temp6458; _LL6464: _temp6463= _temp6460.f1; goto _LL6462; _LL6462: _temp6461=
_temp6460.f2; goto _LL6435;} else{ goto _LL6436;} _LL6436: goto _LL6437; _LL6421:
fprintf( Cyc_Stdio_stderr,"ok"); goto _LL6419; _LL6423: fprintf( Cyc_Stdio_stderr,"%d",
_temp6441); goto _LL6419; _LL6425: fprintf( Cyc_Stdio_stderr,"%c",( int)
_temp6445); goto _LL6419; _LL6427: fprintf( Cyc_Stdio_stderr,"%ds",( int)
_temp6447); goto _LL6419; _LL6429:({ struct _tagged_string _temp6465= _temp6449;
fprintf( Cyc_Stdio_stderr,"\"%.*s\"", _temp6465.last_plus_one - _temp6465.curr,
_temp6465.curr);}); goto _LL6419; _LL6431: fprintf( Cyc_Stdio_stderr,"null");
goto _LL6419; _LL6433:({ struct _tagged_string _temp6466=* _temp6456; fprintf(
Cyc_Stdio_stderr,"\"%.*s\"", _temp6466.last_plus_one - _temp6466.curr, _temp6466.curr);});
goto _LL6419; _LL6435: { struct Cyc_List_List* prefix= 0;{ void* _temp6467=
_temp6463; struct Cyc_List_List* _temp6475; struct Cyc_List_List* _temp6477;
_LL6469: if(( unsigned int) _temp6467 > 1u?(( struct _enum_struct*) _temp6467)->tag
== Cyc_Absyn_Rel_n_tag: 0){ _LL6476: _temp6475=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Rel_n_struct*) _temp6467)->f1; goto _LL6470;} else{ goto _LL6471;}
_LL6471: if(( unsigned int) _temp6467 > 1u?(( struct _enum_struct*) _temp6467)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL6478: _temp6477=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp6467)->f1; goto _LL6472;} else{ goto _LL6473;}
_LL6473: if( _temp6467 == Cyc_Absyn_Loc_n){ goto _LL6474;} else{ goto _LL6468;}
_LL6470: prefix= _temp6475; goto _LL6468; _LL6472: prefix= _temp6477; goto
_LL6468; _LL6474: goto _LL6468; _LL6468:;} for( 0; prefix != 0; prefix= prefix->tl){({
struct _tagged_string _temp6479=*(( struct _tagged_string*) prefix->hd); fprintf(
Cyc_Stdio_stderr,"%.*s::", _temp6479.last_plus_one - _temp6479.curr, _temp6479.curr);});}({
struct _tagged_string _temp6480=* _temp6461; fprintf( Cyc_Stdio_stderr,"%.*s::",
_temp6480.last_plus_one - _temp6480.curr, _temp6480.curr);}); goto _LL6419;}
_LL6437: fprintf( Cyc_Stdio_stderr,"?"); goto _LL6419; _LL6419:;} struct Cyc_List_List*
Cyc_Parse_parse_file( struct Cyc_Stdio___sFILE* f){ Cyc_Parse_parse_result= 0;
Cyc_Parse_lbuf=({ struct Cyc_Core_Opt* _temp6481=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp6481->v=( void*) Cyc_Lexing_from_file(
f); _temp6481;}); Cyc_yyparse(); return Cyc_Parse_parse_result;}