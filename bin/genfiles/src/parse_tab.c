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
Cyc_Absyn_Const_e_tag; struct Cyc_Absyn_Const_e_struct{ int tag; void* f1; } ;
extern const int Cyc_Absyn_Var_e_tag; struct Cyc_Absyn_Var_e_struct{ int tag;
struct _tuple1* f1; void* f2; } ; extern const int Cyc_Absyn_UnknownId_e_tag;
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
tms->tl); _LL457:;}} static struct Cyc_Absyn_Stmt* Cyc_Parse_flatten_decl(
struct Cyc_Absyn_Decl* d, struct Cyc_Absyn_Stmt* s){ return Cyc_Absyn_new_stmt(({
struct Cyc_Absyn_Decl_s_struct* _temp549=( struct Cyc_Absyn_Decl_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl_s_struct)); _temp549->tag= Cyc_Absyn_Decl_s_tag;
_temp549->f1= d; _temp549->f2= s;( void*) _temp549;}), Cyc_Position_segment_join(
d->loc, s->loc));} static struct Cyc_Absyn_Stmt* Cyc_Parse_flatten_declarations(
struct Cyc_List_List* ds, struct Cyc_Absyn_Stmt* s){ return(( struct Cyc_Absyn_Stmt*(*)(
struct Cyc_Absyn_Stmt*(* f)( struct Cyc_Absyn_Decl*, struct Cyc_Absyn_Stmt*),
struct Cyc_List_List* x, struct Cyc_Absyn_Stmt* accum)) Cyc_List_fold_right)(
Cyc_Parse_flatten_decl, ds, s);} static struct Cyc_List_List* Cyc_Parse_make_declarations(
struct Cyc_Parse_Declaration_spec* ds, struct Cyc_List_List* ids, struct Cyc_Position_Segment*
loc){ struct Cyc_List_List* tss= ds->type_specs; struct Cyc_Absyn_Tqual* tq= ds->tq;
int istypedef= 0; void* s= Cyc_Absyn_Public; struct Cyc_List_List* atts= ds->attributes;
if( ds->is_inline){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp550=(
char*)"inline is only allowed on function definitions"; struct _tagged_string
_temp551; _temp551.curr= _temp550; _temp551.base= _temp550; _temp551.last_plus_one=
_temp550 + 47; _temp551;}), loc);} if( tss == 0){ Cyc_Parse_err(( struct
_tagged_string)({ char* _temp552=( char*)"missing type specifiers in declaration";
struct _tagged_string _temp553; _temp553.curr= _temp552; _temp553.base= _temp552;
_temp553.last_plus_one= _temp552 + 39; _temp553;}), loc); return 0;} if( ds->sc
!= 0){ void* _temp554=( void*)( ds->sc)->v; _LL556: if( _temp554 == Cyc_Parse_Typedef_sc){
goto _LL557;} else{ goto _LL558;} _LL558: if( _temp554 == Cyc_Parse_Extern_sc){
goto _LL559;} else{ goto _LL560;} _LL560: if( _temp554 == Cyc_Parse_ExternC_sc){
goto _LL561;} else{ goto _LL562;} _LL562: if( _temp554 == Cyc_Parse_Static_sc){
goto _LL563;} else{ goto _LL564;} _LL564: if( _temp554 == Cyc_Parse_Auto_sc){
goto _LL565;} else{ goto _LL566;} _LL566: if( _temp554 == Cyc_Parse_Register_sc){
goto _LL567;} else{ goto _LL568;} _LL568: if( _temp554 == Cyc_Parse_Abstract_sc){
goto _LL569;} else{ goto _LL555;} _LL557: istypedef= 1; goto _LL555; _LL559: s=
Cyc_Absyn_Extern; goto _LL555; _LL561: s= Cyc_Absyn_ExternC; goto _LL555; _LL563:
s= Cyc_Absyn_Static; goto _LL555; _LL565: s= Cyc_Absyn_Public; goto _LL555;
_LL567: s= Cyc_Absyn_Public; goto _LL555; _LL569: s= Cyc_Absyn_Abstract; goto
_LL555; _LL555:;}{ struct Cyc_List_List* _temp572; struct Cyc_List_List*
_temp574; struct _tuple0 _temp570=(( struct _tuple0(*)( struct Cyc_List_List* x))
Cyc_List_split)( ids); _LL575: _temp574= _temp570.f1; goto _LL573; _LL573:
_temp572= _temp570.f2; goto _LL571; _LL571: { int exps_empty= 1;{ struct Cyc_List_List*
es= _temp572; for( 0; es != 0; es= es->tl){ if(( struct Cyc_Absyn_Exp*) es->hd
!= 0){ exps_empty= 0; break;}}}{ struct _tuple5 ts_info= Cyc_Parse_collapse_type_specifiers(
tss, loc); if( _temp574 == 0){ struct Cyc_Core_Opt* _temp578; void* _temp580;
struct _tuple5 _temp576= ts_info; _LL581: _temp580= _temp576.f1; goto _LL579;
_LL579: _temp578= _temp576.f2; goto _LL577; _LL577: if( _temp578 != 0){ struct
Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*) _temp578->v;{ void* _temp582=( void*)
d->r; struct Cyc_Absyn_Structdecl* _temp592; struct Cyc_Absyn_Enumdecl* _temp594;
struct Cyc_Absyn_Xenumdecl* _temp596; _LL584: if(( unsigned int) _temp582 > 1u?((
struct _enum_struct*) _temp582)->tag == Cyc_Absyn_Struct_d_tag: 0){ _LL593:
_temp592=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*)
_temp582)->f1; goto _LL585;} else{ goto _LL586;} _LL586: if(( unsigned int)
_temp582 > 1u?(( struct _enum_struct*) _temp582)->tag == Cyc_Absyn_Enum_d_tag: 0){
_LL595: _temp594=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*)
_temp582)->f1; goto _LL587;} else{ goto _LL588;} _LL588: if(( unsigned int)
_temp582 > 1u?(( struct _enum_struct*) _temp582)->tag == Cyc_Absyn_Xenum_d_tag:
0){ _LL597: _temp596=( struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_d_struct*)
_temp582)->f1; goto _LL589;} else{ goto _LL590;} _LL590: goto _LL591; _LL585:(
void*)( _temp592->sc=( void*) s); _temp592->attributes= atts; goto _LL583;
_LL587:( void*)( _temp594->sc=( void*) s); if( atts != 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp598=( char*)"bad attributes on enum"; struct
_tagged_string _temp599; _temp599.curr= _temp598; _temp599.base= _temp598;
_temp599.last_plus_one= _temp598 + 23; _temp599;}), loc);} goto _LL583; _LL589:(
void*)( _temp596->sc=( void*) s); if( atts != 0){ Cyc_Parse_err(( struct
_tagged_string)({ char* _temp600=( char*)"bad attributes on xenum"; struct
_tagged_string _temp601; _temp601.curr= _temp600; _temp601.base= _temp600;
_temp601.last_plus_one= _temp600 + 24; _temp601;}), loc);} goto _LL583; _LL591:
Cyc_Parse_err(( struct _tagged_string)({ char* _temp602=( char*)"bad declaration";
struct _tagged_string _temp603; _temp603.curr= _temp602; _temp603.base= _temp602;
_temp603.last_plus_one= _temp602 + 16; _temp603;}), loc); return 0; _LL583:;}
return({ struct Cyc_List_List* _temp604=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp604->hd=( void*) d; _temp604->tl= 0;
_temp604;});} else{ void* _temp605= _temp580; struct Cyc_Absyn_Structdecl**
_temp617; struct Cyc_List_List* _temp619; struct _tuple1* _temp621; struct Cyc_Absyn_Enumdecl**
_temp623; struct Cyc_List_List* _temp625; struct _tuple1* _temp627; struct Cyc_Absyn_Xenumdecl**
_temp629; struct _tuple1* _temp631; _LL607: if(( unsigned int) _temp605 > 5u?((
struct _enum_struct*) _temp605)->tag == Cyc_Absyn_StructType_tag: 0){ _LL622:
_temp621=( struct _tuple1*)(( struct Cyc_Absyn_StructType_struct*) _temp605)->f1;
goto _LL620; _LL620: _temp619=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp605)->f2; goto _LL618; _LL618: _temp617=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp605)->f3; goto _LL608;} else{ goto
_LL609;} _LL609: if(( unsigned int) _temp605 > 5u?(( struct _enum_struct*)
_temp605)->tag == Cyc_Absyn_EnumType_tag: 0){ _LL628: _temp627=( struct _tuple1*)((
struct Cyc_Absyn_EnumType_struct*) _temp605)->f1; goto _LL626; _LL626: _temp625=(
struct Cyc_List_List*)(( struct Cyc_Absyn_EnumType_struct*) _temp605)->f2; goto
_LL624; _LL624: _temp623=( struct Cyc_Absyn_Enumdecl**)(( struct Cyc_Absyn_EnumType_struct*)
_temp605)->f3; goto _LL610;} else{ goto _LL611;} _LL611: if(( unsigned int)
_temp605 > 5u?(( struct _enum_struct*) _temp605)->tag == Cyc_Absyn_XenumType_tag:
0){ _LL632: _temp631=( struct _tuple1*)(( struct Cyc_Absyn_XenumType_struct*)
_temp605)->f1; goto _LL630; _LL630: _temp629=( struct Cyc_Absyn_Xenumdecl**)((
struct Cyc_Absyn_XenumType_struct*) _temp605)->f2; goto _LL612;} else{ goto
_LL613;} _LL613: if( _temp605 == Cyc_Absyn_UnionType){ goto _LL614;} else{ goto
_LL615;} _LL615: goto _LL616; _LL608: { struct Cyc_List_List* ts2=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc,
_temp619); struct Cyc_Absyn_Structdecl* sd=({ struct Cyc_Absyn_Structdecl*
_temp633=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp633->sc=( void*) s; _temp633->name=({ struct Cyc_Core_Opt* _temp634=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp634->v=(
void*)(( struct _tuple1*)({ struct _tuple1* _temp635= _temp621; if( _temp635 ==
0){ _throw( Null_Exception);} _temp635;})); _temp634;}); _temp633->tvs= ts2;
_temp633->fields= 0; _temp633->attributes= 0; _temp633;}); if( atts != 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp636=( char*)"bad attributes on struct";
struct _tagged_string _temp637; _temp637.curr= _temp636; _temp637.base= _temp636;
_temp637.last_plus_one= _temp636 + 25; _temp637;}), loc);} return({ struct Cyc_List_List*
_temp638=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp638->hd=( void*)({ struct Cyc_Absyn_Decl* _temp639=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp639->r=( void*)({ struct Cyc_Absyn_Struct_d_struct*
_temp640=( struct Cyc_Absyn_Struct_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct));
_temp640->tag= Cyc_Absyn_Struct_d_tag; _temp640->f1= sd;( void*) _temp640;});
_temp639->loc= loc; _temp639;}); _temp638->tl= 0; _temp638;});} _LL610: { struct
Cyc_List_List* ts2=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, loc, _temp625); struct
Cyc_Absyn_Decl* ed= Cyc_Absyn_enum_decl( s,({ struct Cyc_Core_Opt* _temp641=(
struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp641->v=(
void*)(( struct _tuple1*)({ struct _tuple1* _temp642= _temp627; if( _temp642 ==
0){ _throw( Null_Exception);} _temp642;})); _temp641;}), ts2, 0, loc); if( atts
!= 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp643=( char*)"bad attributes on enum";
struct _tagged_string _temp644; _temp644.curr= _temp643; _temp644.base= _temp643;
_temp644.last_plus_one= _temp643 + 23; _temp644;}), loc);} return({ struct Cyc_List_List*
_temp645=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp645->hd=( void*) ed; _temp645->tl= 0; _temp645;});} _LL612: { struct Cyc_Absyn_Xenumdecl*
ed=({ struct Cyc_Absyn_Xenumdecl* _temp646=( struct Cyc_Absyn_Xenumdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Xenumdecl)); _temp646->sc=( void*) s;
_temp646->name= _temp631; _temp646->fields= 0; _temp646;}); if( atts != 0){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp647=( char*)"bad attributes on xenum";
struct _tagged_string _temp648; _temp648.curr= _temp647; _temp648.base= _temp647;
_temp648.last_plus_one= _temp647 + 24; _temp648;}), loc);} return({ struct Cyc_List_List*
_temp649=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp649->hd=( void*)({ struct Cyc_Absyn_Decl* _temp650=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp650->r=( void*)({ struct Cyc_Absyn_Xenum_d_struct*
_temp651=( struct Cyc_Absyn_Xenum_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Xenum_d_struct));
_temp651->tag= Cyc_Absyn_Xenum_d_tag; _temp651->f1= ed;( void*) _temp651;});
_temp650->loc= loc; _temp650;}); _temp649->tl= 0; _temp649;});} _LL614: if( atts
!= 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp652=( char*)"bad attributes on union";
struct _tagged_string _temp653; _temp653.curr= _temp652; _temp653.base= _temp652;
_temp653.last_plus_one= _temp652 + 24; _temp653;}), loc);} return({ struct Cyc_List_List*
_temp654=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp654->hd=( void*)({ struct Cyc_Absyn_Decl* _temp655=( struct Cyc_Absyn_Decl*)
GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp655->r=( void*) Cyc_Absyn_Union_d;
_temp655->loc= loc; _temp655;}); _temp654->tl= 0; _temp654;}); _LL616: Cyc_Parse_err((
struct _tagged_string)({ char* _temp656=( char*)"missing declarator"; struct
_tagged_string _temp657; _temp657.curr= _temp656; _temp657.base= _temp656;
_temp657.last_plus_one= _temp656 + 19; _temp657;}), loc); return 0; _LL606:;}}
else{ void* t= ts_info.f1; struct Cyc_List_List* fields= Cyc_Parse_apply_tmss(
tq, t, _temp574, atts); if( istypedef){ if( ! exps_empty){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp658=( char*)"initializer in typedef declaration";
struct _tagged_string _temp659; _temp659.curr= _temp658; _temp659.base= _temp658;
_temp659.last_plus_one= _temp658 + 35; _temp659;}), loc);}{ struct Cyc_List_List*
decls=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Decl*(* f)( struct Cyc_Position_Segment*,
struct _tuple7*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_v_typ_to_typedef, loc, fields); if( ts_info.f2 != 0){
struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)( ts_info.f2)->v;{ void*
_temp660=( void*) d->r; struct Cyc_Absyn_Structdecl* _temp670; struct Cyc_Absyn_Enumdecl*
_temp672; struct Cyc_Absyn_Xenumdecl* _temp674; _LL662: if(( unsigned int)
_temp660 > 1u?(( struct _enum_struct*) _temp660)->tag == Cyc_Absyn_Struct_d_tag:
0){ _LL671: _temp670=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*)
_temp660)->f1; goto _LL663;} else{ goto _LL664;} _LL664: if(( unsigned int)
_temp660 > 1u?(( struct _enum_struct*) _temp660)->tag == Cyc_Absyn_Enum_d_tag: 0){
_LL673: _temp672=( struct Cyc_Absyn_Enumdecl*)(( struct Cyc_Absyn_Enum_d_struct*)
_temp660)->f1; goto _LL665;} else{ goto _LL666;} _LL666: if(( unsigned int)
_temp660 > 1u?(( struct _enum_struct*) _temp660)->tag == Cyc_Absyn_Xenum_d_tag:
0){ _LL675: _temp674=( struct Cyc_Absyn_Xenumdecl*)(( struct Cyc_Absyn_Xenum_d_struct*)
_temp660)->f1; goto _LL667;} else{ goto _LL668;} _LL668: goto _LL669; _LL663:(
void*)( _temp670->sc=( void*) s); _temp670->attributes= atts; atts= 0; goto
_LL661; _LL665:( void*)( _temp672->sc=( void*) s); goto _LL661; _LL667:( void*)(
_temp674->sc=( void*) s); goto _LL661; _LL669: Cyc_Parse_err(( struct
_tagged_string)({ char* _temp676=( char*)"declaration within typedef is not a struct,enum, or xenum";
struct _tagged_string _temp677; _temp677.curr= _temp676; _temp677.base= _temp676;
_temp677.last_plus_one= _temp676 + 58; _temp677;}), loc); goto _LL661; _LL661:;}
decls=({ struct Cyc_List_List* _temp678=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp678->hd=( void*) d; _temp678->tl= decls;
_temp678;});} if( atts != 0){ Cyc_Parse_err(({ struct _tagged_string _temp679=
Cyc_Absyn_attribute2string(( void*) atts->hd); xprintf("bad attribute %.*s in typedef",
_temp679.last_plus_one - _temp679.curr, _temp679.curr);}), loc);} return decls;}}
else{ if( ts_info.f2 != 0){ Cyc_Parse_unimp2(( struct _tagged_string)({ char*
_temp680=( char*)"nested type declaration within declarator"; struct
_tagged_string _temp681; _temp681.curr= _temp680; _temp681.base= _temp680;
_temp681.last_plus_one= _temp680 + 42; _temp681;}), loc);}{ struct Cyc_List_List*
decls= 0;{ struct Cyc_List_List* ds= fields; for( 0; ds != 0; ds= ds->tl,
_temp572= _temp572->tl){ struct _tuple7 _temp684; struct Cyc_List_List* _temp685;
struct Cyc_List_List* _temp687; void* _temp689; struct Cyc_Absyn_Tqual* _temp691;
struct _tuple1* _temp693; struct _tuple7* _temp682=( struct _tuple7*) ds->hd;
_temp684=* _temp682; _LL694: _temp693= _temp684.f1; goto _LL692; _LL692:
_temp691= _temp684.f2; goto _LL690; _LL690: _temp689= _temp684.f3; goto _LL688;
_LL688: _temp687= _temp684.f4; goto _LL686; _LL686: _temp685= _temp684.f5; goto
_LL683; _LL683: if( _temp687 != 0){ Cyc_Parse_warn(( struct _tagged_string)({
char* _temp695=( char*)"bad type params, ignoring"; struct _tagged_string
_temp696; _temp696.curr= _temp695; _temp696.base= _temp695; _temp696.last_plus_one=
_temp695 + 26; _temp696;}), loc);} if( _temp572 == 0){(( void(*)( struct
_tagged_string msg, struct Cyc_Position_Segment* sg)) Cyc_Parse_abort)(( struct
_tagged_string)({ char* _temp697=( char*)"unexpected null in parse!"; struct
_tagged_string _temp698; _temp698.curr= _temp697; _temp698.base= _temp697;
_temp698.last_plus_one= _temp697 + 26; _temp698;}), loc);}{ struct Cyc_Absyn_Exp*
eopt=( struct Cyc_Absyn_Exp*) _temp572->hd; struct Cyc_Absyn_Vardecl* vd= Cyc_Absyn_new_vardecl(
_temp693, _temp689, eopt); vd->tq= _temp691;( void*)( vd->sc=( void*) s); vd->attributes=
_temp685;{ struct Cyc_Absyn_Decl* d=({ struct Cyc_Absyn_Decl* _temp699=( struct
Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl)); _temp699->r=( void*)({
struct Cyc_Absyn_Var_d_struct* _temp700=( struct Cyc_Absyn_Var_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct)); _temp700->tag= Cyc_Absyn_Var_d_tag;
_temp700->f1= vd;( void*) _temp700;}); _temp699->loc= loc; _temp699;}); decls=({
struct Cyc_List_List* _temp701=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp701->hd=( void*) d; _temp701->tl= decls; _temp701;});}}}}
return(( struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)(
decls);}}}}}}} static void* Cyc_Parse_id_to_kind( struct _tagged_string s,
struct Cyc_Position_Segment* loc){ if( Cyc_String_strlen( s) !=( unsigned int) 1){
Cyc_Parse_err(({ struct _tagged_string _temp702= s; xprintf("bad kind: %.*s",
_temp702.last_plus_one - _temp702.curr, _temp702.curr);}), loc); return Cyc_Absyn_BoxKind;}
else{ switch(({ struct _tagged_string _temp703= s; char* _temp705= _temp703.curr
+ 0; if( _temp705 < _temp703.base? 1: _temp705 >= _temp703.last_plus_one){
_throw( Null_Exception);}* _temp705;})){ case 'A': _LL706: return Cyc_Absyn_AnyKind;
case 'M': _LL707: return Cyc_Absyn_MemKind; case 'B': _LL708: return Cyc_Absyn_BoxKind;
case 'R': _LL709: return Cyc_Absyn_RgnKind; case 'E': _LL710: return Cyc_Absyn_EffKind;
default: _LL711: Cyc_Parse_err(({ struct _tagged_string _temp713= s; xprintf("bad kind: %.*s",
_temp713.last_plus_one - _temp713.curr, _temp713.curr);}), loc); return Cyc_Absyn_BoxKind;}}}
static struct Cyc_Absyn_Decl* Cyc_Parse_v_typ_to_typedef( struct Cyc_Position_Segment*
loc, struct _tuple7* t){ struct _tuple1* x=(* t).f1; Cyc_Lex_register_typedef( x);
if((* t).f5 != 0){ Cyc_Parse_err(({ struct _tagged_string _temp714= Cyc_Absyn_attribute2string((
void*)((* t).f5)->hd); xprintf("bad attribute %.*s within typedef", _temp714.last_plus_one
- _temp714.curr, _temp714.curr);}), loc);} return Cyc_Absyn_new_decl(({ struct
Cyc_Absyn_Typedef_d_struct* _temp715=( struct Cyc_Absyn_Typedef_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedef_d_struct)); _temp715->tag= Cyc_Absyn_Typedef_d_tag;
_temp715->f1=({ struct Cyc_Absyn_Typedefdecl* _temp716=( struct Cyc_Absyn_Typedefdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Typedefdecl)); _temp716->name= x; _temp716->tvs=(*
t).f4; _temp716->defn=( void*)(* t).f3; _temp716;});( void*) _temp715;}), loc);}
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
struct _xenum_struct* yy1){ struct _xenum_struct* _temp717= yy1; struct _tuple11*
_temp723; _LL719: if((*(( struct _xenum_struct*) _temp717)).tag == Cyc_Int_tok_tag){
_LL724: _temp723=(( struct Cyc_Int_tok_struct*) _temp717)->f1; goto _LL720;}
else{ goto _LL721;} _LL721: goto _LL722; _LL720: return _temp723; _LL722:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp725=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp725=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp726=( char*)"Int_tok";
struct _tagged_string _temp727; _temp727.curr= _temp726; _temp727.base= _temp726;
_temp727.last_plus_one= _temp726 + 8; _temp727;})};( struct _xenum_struct*)
_temp725;})); _LL718:;} struct _tagged_string Cyc_yyget_String_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp728= yy1; struct _tagged_string
_temp734; _LL730: if((*(( struct _xenum_struct*) _temp728)).tag == Cyc_String_tok_tag){
_LL735: _temp734=(( struct Cyc_String_tok_struct*) _temp728)->f1; goto _LL731;}
else{ goto _LL732;} _LL732: goto _LL733; _LL731: return _temp734; _LL733:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp736=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp736=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp737=( char*)"String_tok";
struct _tagged_string _temp738; _temp738.curr= _temp737; _temp738.base= _temp737;
_temp738.last_plus_one= _temp737 + 11; _temp738;})};( struct _xenum_struct*)
_temp736;})); _LL729:;} char Cyc_yyget_Char_tok( struct _xenum_struct* yy1){
struct _xenum_struct* _temp739= yy1; char _temp745; _LL741: if((*(( struct
_xenum_struct*) _temp739)).tag == Cyc_Char_tok_tag){ _LL746: _temp745=(( struct
Cyc_Char_tok_struct*) _temp739)->f1; goto _LL742;} else{ goto _LL743;} _LL743:
goto _LL744; _LL742: return _temp745; _LL744:( void) _throw(({ struct Cyc_Core_Failure_struct*
_temp747=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));*
_temp747=( struct Cyc_Core_Failure_struct){.tag= Cyc_Core_Failure_tag,.f1=(
struct _tagged_string)({ char* _temp748=( char*)"Char_tok"; struct
_tagged_string _temp749; _temp749.curr= _temp748; _temp749.base= _temp748;
_temp749.last_plus_one= _temp748 + 9; _temp749;})};( struct _xenum_struct*)
_temp747;})); _LL740:;} void* Cyc_yyget_Pointer_Sort_tok( struct _xenum_struct*
yy1){ struct _xenum_struct* _temp750= yy1; void* _temp756; _LL752: if((*((
struct _xenum_struct*) _temp750)).tag == Cyc_Pointer_Sort_tok_tag){ _LL757:
_temp756=(( struct Cyc_Pointer_Sort_tok_struct*) _temp750)->f1; goto _LL753;}
else{ goto _LL754;} _LL754: goto _LL755; _LL753: return _temp756; _LL755:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp758=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp758=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp759=( char*)"Pointer_Sort_tok";
struct _tagged_string _temp760; _temp760.curr= _temp759; _temp760.base= _temp759;
_temp760.last_plus_one= _temp759 + 17; _temp760;})};( struct _xenum_struct*)
_temp758;})); _LL751:;} struct Cyc_Absyn_Exp* Cyc_yyget_Exp_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp761= yy1; struct Cyc_Absyn_Exp*
_temp767; _LL763: if((*(( struct _xenum_struct*) _temp761)).tag == Cyc_Exp_tok_tag){
_LL768: _temp767=(( struct Cyc_Exp_tok_struct*) _temp761)->f1; goto _LL764;}
else{ goto _LL765;} _LL765: goto _LL766; _LL764: return _temp767; _LL766:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp769=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp769=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp770=( char*)"Exp_tok";
struct _tagged_string _temp771; _temp771.curr= _temp770; _temp771.base= _temp770;
_temp771.last_plus_one= _temp770 + 8; _temp771;})};( struct _xenum_struct*)
_temp769;})); _LL762:;} struct Cyc_List_List* Cyc_yyget_ExpList_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp772= yy1; struct Cyc_List_List*
_temp778; _LL774: if((*(( struct _xenum_struct*) _temp772)).tag == Cyc_ExpList_tok_tag){
_LL779: _temp778=(( struct Cyc_ExpList_tok_struct*) _temp772)->f1; goto _LL775;}
else{ goto _LL776;} _LL776: goto _LL777; _LL775: return _temp778; _LL777:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp780=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp780=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp781=( char*)"ExpList_tok";
struct _tagged_string _temp782; _temp782.curr= _temp781; _temp782.base= _temp781;
_temp782.last_plus_one= _temp781 + 12; _temp782;})};( struct _xenum_struct*)
_temp780;})); _LL773:;} struct Cyc_List_List* Cyc_yyget_InitializerList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp783= yy1; struct Cyc_List_List*
_temp789; _LL785: if((*(( struct _xenum_struct*) _temp783)).tag == Cyc_InitializerList_tok_tag){
_LL790: _temp789=(( struct Cyc_InitializerList_tok_struct*) _temp783)->f1; goto
_LL786;} else{ goto _LL787;} _LL787: goto _LL788; _LL786: return _temp789;
_LL788:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp791=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp791=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp792=( char*)"InitializerList_tok";
struct _tagged_string _temp793; _temp793.curr= _temp792; _temp793.base= _temp792;
_temp793.last_plus_one= _temp792 + 20; _temp793;})};( struct _xenum_struct*)
_temp791;})); _LL784:;} void* Cyc_yyget_Primop_tok( struct _xenum_struct* yy1){
struct _xenum_struct* _temp794= yy1; void* _temp800; _LL796: if((*(( struct
_xenum_struct*) _temp794)).tag == Cyc_Primop_tok_tag){ _LL801: _temp800=((
struct Cyc_Primop_tok_struct*) _temp794)->f1; goto _LL797;} else{ goto _LL798;}
_LL798: goto _LL799; _LL797: return _temp800; _LL799:( void) _throw(({ struct
Cyc_Core_Failure_struct* _temp802=( struct Cyc_Core_Failure_struct*) GC_malloc(
sizeof( struct Cyc_Core_Failure_struct));* _temp802=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp803=( char*)"Primop_tok";
struct _tagged_string _temp804; _temp804.curr= _temp803; _temp804.base= _temp803;
_temp804.last_plus_one= _temp803 + 11; _temp804;})};( struct _xenum_struct*)
_temp802;})); _LL795:;} struct Cyc_Core_Opt* Cyc_yyget_Primopopt_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp805= yy1; struct Cyc_Core_Opt*
_temp811; _LL807: if((*(( struct _xenum_struct*) _temp805)).tag == Cyc_Primopopt_tok_tag){
_LL812: _temp811=(( struct Cyc_Primopopt_tok_struct*) _temp805)->f1; goto _LL808;}
else{ goto _LL809;} _LL809: goto _LL810; _LL808: return _temp811; _LL810:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp813=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp813=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp814=( char*)"Primopopt_tok";
struct _tagged_string _temp815; _temp815.curr= _temp814; _temp815.base= _temp814;
_temp815.last_plus_one= _temp814 + 14; _temp815;})};( struct _xenum_struct*)
_temp813;})); _LL806:;} struct _tuple1* Cyc_yyget_QualId_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp816= yy1; struct _tuple1*
_temp822; _LL818: if((*(( struct _xenum_struct*) _temp816)).tag == Cyc_QualId_tok_tag){
_LL823: _temp822=(( struct Cyc_QualId_tok_struct*) _temp816)->f1; goto _LL819;}
else{ goto _LL820;} _LL820: goto _LL821; _LL819: return _temp822; _LL821:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp824=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp824=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp825=( char*)"QualId_tok";
struct _tagged_string _temp826; _temp826.curr= _temp825; _temp826.base= _temp825;
_temp826.last_plus_one= _temp825 + 11; _temp826;})};( struct _xenum_struct*)
_temp824;})); _LL817:;} struct Cyc_Absyn_Stmt* Cyc_yyget_Stmt_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp827= yy1; struct Cyc_Absyn_Stmt*
_temp833; _LL829: if((*(( struct _xenum_struct*) _temp827)).tag == Cyc_Stmt_tok_tag){
_LL834: _temp833=(( struct Cyc_Stmt_tok_struct*) _temp827)->f1; goto _LL830;}
else{ goto _LL831;} _LL831: goto _LL832; _LL830: return _temp833; _LL832:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp835=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp835=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp836=( char*)"Stmt_tok";
struct _tagged_string _temp837; _temp837.curr= _temp836; _temp837.base= _temp836;
_temp837.last_plus_one= _temp836 + 9; _temp837;})};( struct _xenum_struct*)
_temp835;})); _LL828:;} void* Cyc_yyget_BlockItem_tok( struct _xenum_struct* yy1){
struct _xenum_struct* _temp838= yy1; void* _temp844; _LL840: if((*(( struct
_xenum_struct*) _temp838)).tag == Cyc_BlockItem_tok_tag){ _LL845: _temp844=((
struct Cyc_BlockItem_tok_struct*) _temp838)->f1; goto _LL841;} else{ goto _LL842;}
_LL842: goto _LL843; _LL841: return _temp844; _LL843:( void) _throw(({ struct
Cyc_Core_Failure_struct* _temp846=( struct Cyc_Core_Failure_struct*) GC_malloc(
sizeof( struct Cyc_Core_Failure_struct));* _temp846=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp847=( char*)"BlockItem_tok";
struct _tagged_string _temp848; _temp848.curr= _temp847; _temp848.base= _temp847;
_temp848.last_plus_one= _temp847 + 14; _temp848;})};( struct _xenum_struct*)
_temp846;})); _LL839:;} struct Cyc_List_List* Cyc_yyget_SwitchClauseList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp849= yy1; struct Cyc_List_List*
_temp855; _LL851: if((*(( struct _xenum_struct*) _temp849)).tag == Cyc_SwitchClauseList_tok_tag){
_LL856: _temp855=(( struct Cyc_SwitchClauseList_tok_struct*) _temp849)->f1; goto
_LL852;} else{ goto _LL853;} _LL853: goto _LL854; _LL852: return _temp855;
_LL854:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp857=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp857=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp858=( char*)"SwitchClauseList_tok";
struct _tagged_string _temp859; _temp859.curr= _temp858; _temp859.base= _temp858;
_temp859.last_plus_one= _temp858 + 21; _temp859;})};( struct _xenum_struct*)
_temp857;})); _LL850:;} struct Cyc_Absyn_Pat* Cyc_yyget_Pattern_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp860= yy1; struct Cyc_Absyn_Pat*
_temp866; _LL862: if((*(( struct _xenum_struct*) _temp860)).tag == Cyc_Pattern_tok_tag){
_LL867: _temp866=(( struct Cyc_Pattern_tok_struct*) _temp860)->f1; goto _LL863;}
else{ goto _LL864;} _LL864: goto _LL865; _LL863: return _temp866; _LL865:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp868=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp868=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp869=( char*)"Pattern_tok";
struct _tagged_string _temp870; _temp870.curr= _temp869; _temp870.base= _temp869;
_temp870.last_plus_one= _temp869 + 12; _temp870;})};( struct _xenum_struct*)
_temp868;})); _LL861:;} struct Cyc_List_List* Cyc_yyget_PatternList_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp871= yy1; struct Cyc_List_List*
_temp877; _LL873: if((*(( struct _xenum_struct*) _temp871)).tag == Cyc_PatternList_tok_tag){
_LL878: _temp877=(( struct Cyc_PatternList_tok_struct*) _temp871)->f1; goto
_LL874;} else{ goto _LL875;} _LL875: goto _LL876; _LL874: return _temp877;
_LL876:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp879=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp879=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp880=( char*)"PatternList_tok";
struct _tagged_string _temp881; _temp881.curr= _temp880; _temp881.base= _temp880;
_temp881.last_plus_one= _temp880 + 16; _temp881;})};( struct _xenum_struct*)
_temp879;})); _LL872:;} struct _tuple15* Cyc_yyget_FieldPattern_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp882= yy1; struct _tuple15*
_temp888; _LL884: if((*(( struct _xenum_struct*) _temp882)).tag == Cyc_FieldPattern_tok_tag){
_LL889: _temp888=(( struct Cyc_FieldPattern_tok_struct*) _temp882)->f1; goto
_LL885;} else{ goto _LL886;} _LL886: goto _LL887; _LL885: return _temp888;
_LL887:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp890=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp890=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp891=( char*)"FieldPattern_tok";
struct _tagged_string _temp892; _temp892.curr= _temp891; _temp892.base= _temp891;
_temp892.last_plus_one= _temp891 + 17; _temp892;})};( struct _xenum_struct*)
_temp890;})); _LL883:;} struct Cyc_List_List* Cyc_yyget_FieldPatternList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp893= yy1; struct Cyc_List_List*
_temp899; _LL895: if((*(( struct _xenum_struct*) _temp893)).tag == Cyc_FieldPatternList_tok_tag){
_LL900: _temp899=(( struct Cyc_FieldPatternList_tok_struct*) _temp893)->f1; goto
_LL896;} else{ goto _LL897;} _LL897: goto _LL898; _LL896: return _temp899;
_LL898:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp901=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp901=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp902=( char*)"FieldPatternList_tok";
struct _tagged_string _temp903; _temp903.curr= _temp902; _temp903.base= _temp902;
_temp903.last_plus_one= _temp902 + 21; _temp903;})};( struct _xenum_struct*)
_temp901;})); _LL894:;} struct Cyc_Absyn_Fndecl* Cyc_yyget_FnDecl_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp904= yy1; struct Cyc_Absyn_Fndecl*
_temp910; _LL906: if((*(( struct _xenum_struct*) _temp904)).tag == Cyc_FnDecl_tok_tag){
_LL911: _temp910=(( struct Cyc_FnDecl_tok_struct*) _temp904)->f1; goto _LL907;}
else{ goto _LL908;} _LL908: goto _LL909; _LL907: return _temp910; _LL909:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp912=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp912=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp913=( char*)"FnDecl_tok";
struct _tagged_string _temp914; _temp914.curr= _temp913; _temp914.base= _temp913;
_temp914.last_plus_one= _temp913 + 11; _temp914;})};( struct _xenum_struct*)
_temp912;})); _LL905:;} struct Cyc_List_List* Cyc_yyget_DeclList_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp915= yy1; struct Cyc_List_List*
_temp921; _LL917: if((*(( struct _xenum_struct*) _temp915)).tag == Cyc_DeclList_tok_tag){
_LL922: _temp921=(( struct Cyc_DeclList_tok_struct*) _temp915)->f1; goto _LL918;}
else{ goto _LL919;} _LL919: goto _LL920; _LL918: return _temp921; _LL920:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp923=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp923=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp924=( char*)"DeclList_tok";
struct _tagged_string _temp925; _temp925.curr= _temp924; _temp925.base= _temp924;
_temp925.last_plus_one= _temp924 + 13; _temp925;})};( struct _xenum_struct*)
_temp923;})); _LL916:;} struct Cyc_Parse_Declaration_spec* Cyc_yyget_DeclSpec_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp926= yy1; struct Cyc_Parse_Declaration_spec*
_temp932; _LL928: if((*(( struct _xenum_struct*) _temp926)).tag == Cyc_DeclSpec_tok_tag){
_LL933: _temp932=(( struct Cyc_DeclSpec_tok_struct*) _temp926)->f1; goto _LL929;}
else{ goto _LL930;} _LL930: goto _LL931; _LL929: return _temp932; _LL931:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp934=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp934=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp935=( char*)"DeclSpec_tok";
struct _tagged_string _temp936; _temp936.curr= _temp935; _temp936.base= _temp935;
_temp936.last_plus_one= _temp935 + 13; _temp936;})};( struct _xenum_struct*)
_temp934;})); _LL927:;} struct _tuple12* Cyc_yyget_InitDecl_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp937= yy1; struct _tuple12*
_temp943; _LL939: if((*(( struct _xenum_struct*) _temp937)).tag == Cyc_InitDecl_tok_tag){
_LL944: _temp943=(( struct Cyc_InitDecl_tok_struct*) _temp937)->f1; goto _LL940;}
else{ goto _LL941;} _LL941: goto _LL942; _LL940: return _temp943; _LL942:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp945=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp945=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp946=( char*)"InitDecl_tok";
struct _tagged_string _temp947; _temp947.curr= _temp946; _temp947.base= _temp946;
_temp947.last_plus_one= _temp946 + 13; _temp947;})};( struct _xenum_struct*)
_temp945;})); _LL938:;} struct Cyc_List_List* Cyc_yyget_InitDeclList_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp948= yy1; struct Cyc_List_List*
_temp954; _LL950: if((*(( struct _xenum_struct*) _temp948)).tag == Cyc_InitDeclList_tok_tag){
_LL955: _temp954=(( struct Cyc_InitDeclList_tok_struct*) _temp948)->f1; goto
_LL951;} else{ goto _LL952;} _LL952: goto _LL953; _LL951: return _temp954;
_LL953:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp956=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp956=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp957=( char*)"InitDeclList_tok";
struct _tagged_string _temp958; _temp958.curr= _temp957; _temp958.base= _temp957;
_temp958.last_plus_one= _temp957 + 17; _temp958;})};( struct _xenum_struct*)
_temp956;})); _LL949:;} void* Cyc_yyget_StorageClass_tok( struct _xenum_struct*
yy1){ struct _xenum_struct* _temp959= yy1; void* _temp965; _LL961: if((*((
struct _xenum_struct*) _temp959)).tag == Cyc_StorageClass_tok_tag){ _LL966:
_temp965=(( struct Cyc_StorageClass_tok_struct*) _temp959)->f1; goto _LL962;}
else{ goto _LL963;} _LL963: goto _LL964; _LL962: return _temp965; _LL964:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp967=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp967=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp968=( char*)"StorageClass_tok";
struct _tagged_string _temp969; _temp969.curr= _temp968; _temp969.base= _temp968;
_temp969.last_plus_one= _temp968 + 17; _temp969;})};( struct _xenum_struct*)
_temp967;})); _LL960:;} void* Cyc_yyget_TypeSpecifier_tok( struct _xenum_struct*
yy1){ struct _xenum_struct* _temp970= yy1; void* _temp976; _LL972: if((*((
struct _xenum_struct*) _temp970)).tag == Cyc_TypeSpecifier_tok_tag){ _LL977:
_temp976=(( struct Cyc_TypeSpecifier_tok_struct*) _temp970)->f1; goto _LL973;}
else{ goto _LL974;} _LL974: goto _LL975; _LL973: return _temp976; _LL975:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp978=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp978=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp979=( char*)"TypeSpecifier_tok";
struct _tagged_string _temp980; _temp980.curr= _temp979; _temp980.base= _temp979;
_temp980.last_plus_one= _temp979 + 18; _temp980;})};( struct _xenum_struct*)
_temp978;})); _LL971:;} void* Cyc_yyget_StructOrUnion_tok( struct _xenum_struct*
yy1){ struct _xenum_struct* _temp981= yy1; void* _temp987; _LL983: if((*((
struct _xenum_struct*) _temp981)).tag == Cyc_StructOrUnion_tok_tag){ _LL988:
_temp987=(( struct Cyc_StructOrUnion_tok_struct*) _temp981)->f1; goto _LL984;}
else{ goto _LL985;} _LL985: goto _LL986; _LL984: return _temp987; _LL986:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp989=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp989=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp990=( char*)"StructOrUnion_tok";
struct _tagged_string _temp991; _temp991.curr= _temp990; _temp991.base= _temp990;
_temp991.last_plus_one= _temp990 + 18; _temp991;})};( struct _xenum_struct*)
_temp989;})); _LL982:;} struct Cyc_Absyn_Tqual* Cyc_yyget_TypeQual_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp992= yy1; struct Cyc_Absyn_Tqual*
_temp998; _LL994: if((*(( struct _xenum_struct*) _temp992)).tag == Cyc_TypeQual_tok_tag){
_LL999: _temp998=(( struct Cyc_TypeQual_tok_struct*) _temp992)->f1; goto _LL995;}
else{ goto _LL996;} _LL996: goto _LL997; _LL995: return _temp998; _LL997:( void)
_throw(({ struct Cyc_Core_Failure_struct* _temp1000=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp1000=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp1001=( char*)"TypeQual_tok";
struct _tagged_string _temp1002; _temp1002.curr= _temp1001; _temp1002.base=
_temp1001; _temp1002.last_plus_one= _temp1001 + 13; _temp1002;})};( struct
_xenum_struct*) _temp1000;})); _LL993:;} struct Cyc_List_List* Cyc_yyget_StructFieldDeclList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1003= yy1; struct Cyc_List_List*
_temp1009; _LL1005: if((*(( struct _xenum_struct*) _temp1003)).tag == Cyc_StructFieldDeclList_tok_tag){
_LL1010: _temp1009=(( struct Cyc_StructFieldDeclList_tok_struct*) _temp1003)->f1;
goto _LL1006;} else{ goto _LL1007;} _LL1007: goto _LL1008; _LL1006: return
_temp1009; _LL1008:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1011=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));*
_temp1011=( struct Cyc_Core_Failure_struct){.tag= Cyc_Core_Failure_tag,.f1=(
struct _tagged_string)({ char* _temp1012=( char*)"StructFieldDeclList_tok";
struct _tagged_string _temp1013; _temp1013.curr= _temp1012; _temp1013.base=
_temp1012; _temp1013.last_plus_one= _temp1012 + 24; _temp1013;})};( struct
_xenum_struct*) _temp1011;})); _LL1004:;} struct Cyc_List_List* Cyc_yyget_StructFieldDeclListList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1014= yy1; struct Cyc_List_List*
_temp1020; _LL1016: if((*(( struct _xenum_struct*) _temp1014)).tag == Cyc_StructFieldDeclListList_tok_tag){
_LL1021: _temp1020=(( struct Cyc_StructFieldDeclListList_tok_struct*) _temp1014)->f1;
goto _LL1017;} else{ goto _LL1018;} _LL1018: goto _LL1019; _LL1017: return
_temp1020; _LL1019:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1022=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));*
_temp1022=( struct Cyc_Core_Failure_struct){.tag= Cyc_Core_Failure_tag,.f1=(
struct _tagged_string)({ char* _temp1023=( char*)"StructFieldDeclListList_tok";
struct _tagged_string _temp1024; _temp1024.curr= _temp1023; _temp1024.base=
_temp1023; _temp1024.last_plus_one= _temp1023 + 28; _temp1024;})};( struct
_xenum_struct*) _temp1022;})); _LL1015:;} struct Cyc_List_List* Cyc_yyget_TypeModifierList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1025= yy1; struct Cyc_List_List*
_temp1031; _LL1027: if((*(( struct _xenum_struct*) _temp1025)).tag == Cyc_TypeModifierList_tok_tag){
_LL1032: _temp1031=(( struct Cyc_TypeModifierList_tok_struct*) _temp1025)->f1;
goto _LL1028;} else{ goto _LL1029;} _LL1029: goto _LL1030; _LL1028: return
_temp1031; _LL1030:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1033=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));*
_temp1033=( struct Cyc_Core_Failure_struct){.tag= Cyc_Core_Failure_tag,.f1=(
struct _tagged_string)({ char* _temp1034=( char*)"TypeModifierList_tok"; struct
_tagged_string _temp1035; _temp1035.curr= _temp1034; _temp1035.base= _temp1034;
_temp1035.last_plus_one= _temp1034 + 21; _temp1035;})};( struct _xenum_struct*)
_temp1033;})); _LL1026:;} void* Cyc_yyget_Rgn_tok( struct _xenum_struct* yy1){
struct _xenum_struct* _temp1036= yy1; void* _temp1042; _LL1038: if((*(( struct
_xenum_struct*) _temp1036)).tag == Cyc_Rgn_tok_tag){ _LL1043: _temp1042=((
struct Cyc_Rgn_tok_struct*) _temp1036)->f1; goto _LL1039;} else{ goto _LL1040;}
_LL1040: goto _LL1041; _LL1039: return _temp1042; _LL1041:( void) _throw(({
struct Cyc_Core_Failure_struct* _temp1044=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp1044=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp1045=( char*)"Rgn_tok";
struct _tagged_string _temp1046; _temp1046.curr= _temp1045; _temp1046.base=
_temp1045; _temp1046.last_plus_one= _temp1045 + 8; _temp1046;})};( struct
_xenum_struct*) _temp1044;})); _LL1037:;} struct Cyc_Parse_Declarator* Cyc_yyget_Declarator_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1047= yy1; struct Cyc_Parse_Declarator*
_temp1053; _LL1049: if((*(( struct _xenum_struct*) _temp1047)).tag == Cyc_Declarator_tok_tag){
_LL1054: _temp1053=(( struct Cyc_Declarator_tok_struct*) _temp1047)->f1; goto
_LL1050;} else{ goto _LL1051;} _LL1051: goto _LL1052; _LL1050: return _temp1053;
_LL1052:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1055=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp1055=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp1056=( char*)"Declarator_tok";
struct _tagged_string _temp1057; _temp1057.curr= _temp1056; _temp1057.base=
_temp1056; _temp1057.last_plus_one= _temp1056 + 15; _temp1057;})};( struct
_xenum_struct*) _temp1055;})); _LL1048:;} struct Cyc_List_List* Cyc_yyget_DeclaratorList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1058= yy1; struct Cyc_List_List*
_temp1064; _LL1060: if((*(( struct _xenum_struct*) _temp1058)).tag == Cyc_DeclaratorList_tok_tag){
_LL1065: _temp1064=(( struct Cyc_DeclaratorList_tok_struct*) _temp1058)->f1;
goto _LL1061;} else{ goto _LL1062;} _LL1062: goto _LL1063; _LL1061: return
_temp1064; _LL1063:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1066=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));*
_temp1066=( struct Cyc_Core_Failure_struct){.tag= Cyc_Core_Failure_tag,.f1=(
struct _tagged_string)({ char* _temp1067=( char*)"DeclaratorList_tok"; struct
_tagged_string _temp1068; _temp1068.curr= _temp1067; _temp1068.base= _temp1067;
_temp1068.last_plus_one= _temp1067 + 19; _temp1068;})};( struct _xenum_struct*)
_temp1066;})); _LL1059:;} struct Cyc_Parse_Abstractdeclarator* Cyc_yyget_AbstractDeclarator_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1069= yy1; struct Cyc_Parse_Abstractdeclarator*
_temp1075; _LL1071: if((*(( struct _xenum_struct*) _temp1069)).tag == Cyc_AbstractDeclarator_tok_tag){
_LL1076: _temp1075=(( struct Cyc_AbstractDeclarator_tok_struct*) _temp1069)->f1;
goto _LL1072;} else{ goto _LL1073;} _LL1073: goto _LL1074; _LL1072: return
_temp1075; _LL1074:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1077=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));*
_temp1077=( struct Cyc_Core_Failure_struct){.tag= Cyc_Core_Failure_tag,.f1=(
struct _tagged_string)({ char* _temp1078=( char*)"AbstractDeclarator_tok";
struct _tagged_string _temp1079; _temp1079.curr= _temp1078; _temp1079.base=
_temp1078; _temp1079.last_plus_one= _temp1078 + 23; _temp1079;})};( struct
_xenum_struct*) _temp1077;})); _LL1070:;} struct Cyc_Absyn_Enumfield* Cyc_yyget_EnumeratorField_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1080= yy1; struct Cyc_Absyn_Enumfield*
_temp1086; _LL1082: if((*(( struct _xenum_struct*) _temp1080)).tag == Cyc_EnumeratorField_tok_tag){
_LL1087: _temp1086=(( struct Cyc_EnumeratorField_tok_struct*) _temp1080)->f1;
goto _LL1083;} else{ goto _LL1084;} _LL1084: goto _LL1085; _LL1083: return
_temp1086; _LL1085:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1088=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));*
_temp1088=( struct Cyc_Core_Failure_struct){.tag= Cyc_Core_Failure_tag,.f1=(
struct _tagged_string)({ char* _temp1089=( char*)"EnumeratorField_tok"; struct
_tagged_string _temp1090; _temp1090.curr= _temp1089; _temp1090.base= _temp1089;
_temp1090.last_plus_one= _temp1089 + 20; _temp1090;})};( struct _xenum_struct*)
_temp1088;})); _LL1081:;} struct Cyc_List_List* Cyc_yyget_EnumeratorFieldList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1091= yy1; struct Cyc_List_List*
_temp1097; _LL1093: if((*(( struct _xenum_struct*) _temp1091)).tag == Cyc_EnumeratorFieldList_tok_tag){
_LL1098: _temp1097=(( struct Cyc_EnumeratorFieldList_tok_struct*) _temp1091)->f1;
goto _LL1094;} else{ goto _LL1095;} _LL1095: goto _LL1096; _LL1094: return
_temp1097; _LL1096:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1099=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));*
_temp1099=( struct Cyc_Core_Failure_struct){.tag= Cyc_Core_Failure_tag,.f1=(
struct _tagged_string)({ char* _temp1100=( char*)"EnumeratorFieldList_tok";
struct _tagged_string _temp1101; _temp1101.curr= _temp1100; _temp1101.base=
_temp1100; _temp1101.last_plus_one= _temp1100 + 24; _temp1101;})};( struct
_xenum_struct*) _temp1099;})); _LL1092:;} struct _tuple13* Cyc_yyget_QualSpecList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1102= yy1; struct
_tuple13* _temp1108; _LL1104: if((*(( struct _xenum_struct*) _temp1102)).tag ==
Cyc_QualSpecList_tok_tag){ _LL1109: _temp1108=(( struct Cyc_QualSpecList_tok_struct*)
_temp1102)->f1; goto _LL1105;} else{ goto _LL1106;} _LL1106: goto _LL1107;
_LL1105: return _temp1108; _LL1107:( void) _throw(({ struct Cyc_Core_Failure_struct*
_temp1110=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));*
_temp1110=( struct Cyc_Core_Failure_struct){.tag= Cyc_Core_Failure_tag,.f1=(
struct _tagged_string)({ char* _temp1111=( char*)"QualSpecList_tok"; struct
_tagged_string _temp1112; _temp1112.curr= _temp1111; _temp1112.base= _temp1111;
_temp1112.last_plus_one= _temp1111 + 17; _temp1112;})};( struct _xenum_struct*)
_temp1110;})); _LL1103:;} struct Cyc_List_List* Cyc_yyget_IdList_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp1113= yy1; struct Cyc_List_List*
_temp1119; _LL1115: if((*(( struct _xenum_struct*) _temp1113)).tag == Cyc_IdList_tok_tag){
_LL1120: _temp1119=(( struct Cyc_IdList_tok_struct*) _temp1113)->f1; goto
_LL1116;} else{ goto _LL1117;} _LL1117: goto _LL1118; _LL1116: return _temp1119;
_LL1118:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1121=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp1121=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp1122=( char*)"IdList_tok";
struct _tagged_string _temp1123; _temp1123.curr= _temp1122; _temp1123.base=
_temp1122; _temp1123.last_plus_one= _temp1122 + 11; _temp1123;})};( struct
_xenum_struct*) _temp1121;})); _LL1114:;} struct _tuple2* Cyc_yyget_ParamDecl_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1124= yy1; struct _tuple2*
_temp1130; _LL1126: if((*(( struct _xenum_struct*) _temp1124)).tag == Cyc_ParamDecl_tok_tag){
_LL1131: _temp1130=(( struct Cyc_ParamDecl_tok_struct*) _temp1124)->f1; goto
_LL1127;} else{ goto _LL1128;} _LL1128: goto _LL1129; _LL1127: return _temp1130;
_LL1129:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1132=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp1132=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp1133=( char*)"ParamDecl_tok";
struct _tagged_string _temp1134; _temp1134.curr= _temp1133; _temp1134.base=
_temp1133; _temp1134.last_plus_one= _temp1133 + 14; _temp1134;})};( struct
_xenum_struct*) _temp1132;})); _LL1125:;} struct Cyc_List_List* Cyc_yyget_ParamDeclList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1135= yy1; struct Cyc_List_List*
_temp1141; _LL1137: if((*(( struct _xenum_struct*) _temp1135)).tag == Cyc_ParamDeclList_tok_tag){
_LL1142: _temp1141=(( struct Cyc_ParamDeclList_tok_struct*) _temp1135)->f1; goto
_LL1138;} else{ goto _LL1139;} _LL1139: goto _LL1140; _LL1138: return _temp1141;
_LL1140:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1143=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp1143=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp1144=( char*)"ParamDeclList_tok";
struct _tagged_string _temp1145; _temp1145.curr= _temp1144; _temp1145.base=
_temp1144; _temp1145.last_plus_one= _temp1144 + 18; _temp1145;})};( struct
_xenum_struct*) _temp1143;})); _LL1136:;} struct _tuple14* Cyc_yyget_ParamDeclListBool_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1146= yy1; struct
_tuple14* _temp1152; _LL1148: if((*(( struct _xenum_struct*) _temp1146)).tag ==
Cyc_ParamDeclListBool_tok_tag){ _LL1153: _temp1152=(( struct Cyc_ParamDeclListBool_tok_struct*)
_temp1146)->f1; goto _LL1149;} else{ goto _LL1150;} _LL1150: goto _LL1151;
_LL1149: return _temp1152; _LL1151:( void) _throw(({ struct Cyc_Core_Failure_struct*
_temp1154=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));*
_temp1154=( struct Cyc_Core_Failure_struct){.tag= Cyc_Core_Failure_tag,.f1=(
struct _tagged_string)({ char* _temp1155=( char*)"ParamDeclListBool_tok"; struct
_tagged_string _temp1156; _temp1156.curr= _temp1155; _temp1156.base= _temp1155;
_temp1156.last_plus_one= _temp1155 + 22; _temp1156;})};( struct _xenum_struct*)
_temp1154;})); _LL1147:;} struct Cyc_List_List* Cyc_yyget_TypeList_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp1157= yy1; struct Cyc_List_List*
_temp1163; _LL1159: if((*(( struct _xenum_struct*) _temp1157)).tag == Cyc_TypeList_tok_tag){
_LL1164: _temp1163=(( struct Cyc_TypeList_tok_struct*) _temp1157)->f1; goto
_LL1160;} else{ goto _LL1161;} _LL1161: goto _LL1162; _LL1160: return _temp1163;
_LL1162:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1165=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp1165=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp1166=( char*)"TypeList_tok";
struct _tagged_string _temp1167; _temp1167.curr= _temp1166; _temp1167.base=
_temp1166; _temp1167.last_plus_one= _temp1166 + 13; _temp1167;})};( struct
_xenum_struct*) _temp1165;})); _LL1158:;} struct Cyc_List_List* Cyc_yyget_DesignatorList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1168= yy1; struct Cyc_List_List*
_temp1174; _LL1170: if((*(( struct _xenum_struct*) _temp1168)).tag == Cyc_DesignatorList_tok_tag){
_LL1175: _temp1174=(( struct Cyc_DesignatorList_tok_struct*) _temp1168)->f1;
goto _LL1171;} else{ goto _LL1172;} _LL1172: goto _LL1173; _LL1171: return
_temp1174; _LL1173:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1176=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));*
_temp1176=( struct Cyc_Core_Failure_struct){.tag= Cyc_Core_Failure_tag,.f1=(
struct _tagged_string)({ char* _temp1177=( char*)"DesignatorList_tok"; struct
_tagged_string _temp1178; _temp1178.curr= _temp1177; _temp1178.base= _temp1177;
_temp1178.last_plus_one= _temp1177 + 19; _temp1178;})};( struct _xenum_struct*)
_temp1176;})); _LL1169:;} void* Cyc_yyget_Designator_tok( struct _xenum_struct*
yy1){ struct _xenum_struct* _temp1179= yy1; void* _temp1185; _LL1181: if((*((
struct _xenum_struct*) _temp1179)).tag == Cyc_Designator_tok_tag){ _LL1186:
_temp1185=(( struct Cyc_Designator_tok_struct*) _temp1179)->f1; goto _LL1182;}
else{ goto _LL1183;} _LL1183: goto _LL1184; _LL1182: return _temp1185; _LL1184:(
void) _throw(({ struct Cyc_Core_Failure_struct* _temp1187=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp1187=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp1188=( char*)"Designator_tok";
struct _tagged_string _temp1189; _temp1189.curr= _temp1188; _temp1189.base=
_temp1188; _temp1189.last_plus_one= _temp1188 + 15; _temp1189;})};( struct
_xenum_struct*) _temp1187;})); _LL1180:;} void* Cyc_yyget_Kind_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp1190= yy1; void* _temp1196;
_LL1192: if((*(( struct _xenum_struct*) _temp1190)).tag == Cyc_Kind_tok_tag){
_LL1197: _temp1196=(( struct Cyc_Kind_tok_struct*) _temp1190)->f1; goto _LL1193;}
else{ goto _LL1194;} _LL1194: goto _LL1195; _LL1193: return _temp1196; _LL1195:(
void) _throw(({ struct Cyc_Core_Failure_struct* _temp1198=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp1198=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp1199=( char*)"Kind_tok";
struct _tagged_string _temp1200; _temp1200.curr= _temp1199; _temp1200.base=
_temp1199; _temp1200.last_plus_one= _temp1199 + 9; _temp1200;})};( struct
_xenum_struct*) _temp1198;})); _LL1191:;} void* Cyc_yyget_Type_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp1201= yy1; void* _temp1207;
_LL1203: if((*(( struct _xenum_struct*) _temp1201)).tag == Cyc_Type_tok_tag){
_LL1208: _temp1207=(( struct Cyc_Type_tok_struct*) _temp1201)->f1; goto _LL1204;}
else{ goto _LL1205;} _LL1205: goto _LL1206; _LL1204: return _temp1207; _LL1206:(
void) _throw(({ struct Cyc_Core_Failure_struct* _temp1209=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp1209=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp1210=( char*)"Type_tok";
struct _tagged_string _temp1211; _temp1211.curr= _temp1210; _temp1211.base=
_temp1210; _temp1211.last_plus_one= _temp1210 + 9; _temp1211;})};( struct
_xenum_struct*) _temp1209;})); _LL1202:;} struct Cyc_List_List* Cyc_yyget_AttributeList_tok(
struct _xenum_struct* yy1){ struct _xenum_struct* _temp1212= yy1; struct Cyc_List_List*
_temp1218; _LL1214: if((*(( struct _xenum_struct*) _temp1212)).tag == Cyc_AttributeList_tok_tag){
_LL1219: _temp1218=(( struct Cyc_AttributeList_tok_struct*) _temp1212)->f1; goto
_LL1215;} else{ goto _LL1216;} _LL1216: goto _LL1217; _LL1215: return _temp1218;
_LL1217:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1220=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp1220=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp1221=( char*)"AttributeList_tok";
struct _tagged_string _temp1222; _temp1222.curr= _temp1221; _temp1222.base=
_temp1221; _temp1222.last_plus_one= _temp1221 + 18; _temp1222;})};( struct
_xenum_struct*) _temp1220;})); _LL1213:;} void* Cyc_yyget_Attribute_tok( struct
_xenum_struct* yy1){ struct _xenum_struct* _temp1223= yy1; void* _temp1229;
_LL1225: if((*(( struct _xenum_struct*) _temp1223)).tag == Cyc_Attribute_tok_tag){
_LL1230: _temp1229=(( struct Cyc_Attribute_tok_struct*) _temp1223)->f1; goto
_LL1226;} else{ goto _LL1227;} _LL1227: goto _LL1228; _LL1226: return _temp1229;
_LL1228:( void) _throw(({ struct Cyc_Core_Failure_struct* _temp1231=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct));* _temp1231=( struct Cyc_Core_Failure_struct){.tag=
Cyc_Core_Failure_tag,.f1=( struct _tagged_string)({ char* _temp1232=( char*)"Attribute_tok";
struct _tagged_string _temp1233; _temp1233.curr= _temp1232; _temp1233.base=
_temp1232; _temp1233.last_plus_one= _temp1232 + 14; _temp1233;})};( struct
_xenum_struct*) _temp1231;})); _LL1224:;} struct Cyc_Yyltype{ int timestamp; int
first_line; int first_column; int last_line; int last_column; struct
_tagged_string text; } ; typedef struct Cyc_Yyltype Cyc_yyltype; struct Cyc_Yyltype
Cyc_yynewloc(){ return({ struct Cyc_Yyltype _temp1234; _temp1234.timestamp= 0;
_temp1234.first_line= 0; _temp1234.first_column= 0; _temp1234.last_line= 0;
_temp1234.last_column= 0; _temp1234.text=( struct _tagged_string)({ char*
_temp1235=( char*)""; struct _tagged_string _temp1236; _temp1236.curr= _temp1235;
_temp1236.base= _temp1235; _temp1236.last_plus_one= _temp1235 + 1; _temp1236;});
_temp1234;});} static char _temp1239[ 1u]=""; struct Cyc_Yyltype Cyc_yylloc=(
struct Cyc_Yyltype){.timestamp= 0,.first_line= 0,.first_column= 0,.last_line= 0,.last_column=
0,.text=( struct _tagged_string){ _temp1239, _temp1239, _temp1239 + 1u}}; static
short Cyc_yytranslate[ 346u]={ (short)0, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)117, (short)2, (short)2, (short)99,
(short)115, (short)111, (short)2, (short)95, (short)96, (short)105, (short)108,
(short)97, (short)110, (short)109, (short)114, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)102,
(short)91, (short)100, (short)94, (short)101, (short)107, (short)106, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)103, (short)2, (short)104, (short)113, (short)98, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2, (short)2,
(short)2, (short)2, (short)92, (short)112, (short)93, (short)116, (short)2,
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
(short)86, (short)87, (short)88, (short)89, (short)90}; static char _temp1242[ 2u]="$";
static char _temp1245[ 6u]="error"; static char _temp1248[ 12u]="$undefined.";
static char _temp1251[ 5u]="AUTO"; static char _temp1254[ 9u]="REGISTER"; static
char _temp1257[ 7u]="STATIC"; static char _temp1260[ 7u]="EXTERN"; static char
_temp1263[ 8u]="TYPEDEF"; static char _temp1266[ 5u]="VOID"; static char
_temp1269[ 5u]="CHAR"; static char _temp1272[ 6u]="SHORT"; static char _temp1275[
4u]="INT"; static char _temp1278[ 5u]="LONG"; static char _temp1281[ 6u]="FLOAT";
static char _temp1284[ 7u]="DOUBLE"; static char _temp1287[ 7u]="SIGNED"; static
char _temp1290[ 9u]="UNSIGNED"; static char _temp1293[ 6u]="CONST"; static char
_temp1296[ 9u]="VOLATILE"; static char _temp1299[ 9u]="RESTRICT"; static char
_temp1302[ 7u]="STRUCT"; static char _temp1305[ 6u]="UNION"; static char
_temp1308[ 5u]="CASE"; static char _temp1311[ 8u]="DEFAULT"; static char
_temp1314[ 7u]="INLINE"; static char _temp1317[ 3u]="IF"; static char _temp1320[
5u]="ELSE"; static char _temp1323[ 7u]="SWITCH"; static char _temp1326[ 6u]="WHILE";
static char _temp1329[ 3u]="DO"; static char _temp1332[ 4u]="FOR"; static char
_temp1335[ 5u]="GOTO"; static char _temp1338[ 9u]="CONTINUE"; static char
_temp1341[ 6u]="BREAK"; static char _temp1344[ 7u]="RETURN"; static char
_temp1347[ 7u]="SIZEOF"; static char _temp1350[ 5u]="ENUM"; static char
_temp1353[ 8u]="NULL_kw"; static char _temp1356[ 4u]="LET"; static char
_temp1359[ 6u]="THROW"; static char _temp1362[ 4u]="TRY"; static char _temp1365[
6u]="CATCH"; static char _temp1368[ 4u]="NEW"; static char _temp1371[ 9u]="ABSTRACT";
static char _temp1374[ 9u]="FALLTHRU"; static char _temp1377[ 6u]="USING";
static char _temp1380[ 10u]="NAMESPACE"; static char _temp1383[ 6u]="XENUM";
static char _temp1386[ 5u]="FILL"; static char _temp1389[ 8u]="CODEGEN"; static
char _temp1392[ 4u]="CUT"; static char _temp1395[ 7u]="SPLICE"; static char
_temp1398[ 7u]="PRINTF"; static char _temp1401[ 8u]="FPRINTF"; static char
_temp1404[ 8u]="XPRINTF"; static char _temp1407[ 6u]="SCANF"; static char
_temp1410[ 7u]="FSCANF"; static char _temp1413[ 7u]="SSCANF"; static char
_temp1416[ 7u]="PTR_OP"; static char _temp1419[ 7u]="INC_OP"; static char
_temp1422[ 7u]="DEC_OP"; static char _temp1425[ 8u]="LEFT_OP"; static char
_temp1428[ 9u]="RIGHT_OP"; static char _temp1431[ 6u]="LE_OP"; static char
_temp1434[ 6u]="GE_OP"; static char _temp1437[ 6u]="EQ_OP"; static char
_temp1440[ 6u]="NE_OP"; static char _temp1443[ 7u]="AND_OP"; static char
_temp1446[ 6u]="OR_OP"; static char _temp1449[ 11u]="MUL_ASSIGN"; static char
_temp1452[ 11u]="DIV_ASSIGN"; static char _temp1455[ 11u]="MOD_ASSIGN"; static
char _temp1458[ 11u]="ADD_ASSIGN"; static char _temp1461[ 11u]="SUB_ASSIGN";
static char _temp1464[ 12u]="LEFT_ASSIGN"; static char _temp1467[ 13u]="RIGHT_ASSIGN";
static char _temp1470[ 11u]="AND_ASSIGN"; static char _temp1473[ 11u]="XOR_ASSIGN";
static char _temp1476[ 10u]="OR_ASSIGN"; static char _temp1479[ 9u]="ELLIPSIS";
static char _temp1482[ 11u]="LEFT_RIGHT"; static char _temp1485[ 12u]="COLON_COLON";
static char _temp1488[ 11u]="IDENTIFIER"; static char _temp1491[ 17u]="INTEGER_CONSTANT";
static char _temp1494[ 7u]="STRING"; static char _temp1497[ 19u]="CHARACTER_CONSTANT";
static char _temp1500[ 18u]="FLOATING_CONSTANT"; static char _temp1503[ 9u]="TYPE_VAR";
static char _temp1506[ 16u]="QUAL_IDENTIFIER"; static char _temp1509[ 18u]="QUAL_TYPEDEF_NAME";
static char _temp1512[ 10u]="ATTRIBUTE"; static char _temp1515[ 4u]="';'";
static char _temp1518[ 4u]="'{'"; static char _temp1521[ 4u]="'}'"; static char
_temp1524[ 4u]="'='"; static char _temp1527[ 4u]="'('"; static char _temp1530[ 4u]="')'";
static char _temp1533[ 4u]="','"; static char _temp1536[ 4u]="'_'"; static char
_temp1539[ 4u]="'$'"; static char _temp1542[ 4u]="'<'"; static char _temp1545[ 4u]="'>'";
static char _temp1548[ 4u]="':'"; static char _temp1551[ 4u]="'['"; static char
_temp1554[ 4u]="']'"; static char _temp1557[ 4u]="'*'"; static char _temp1560[ 4u]="'@'";
static char _temp1563[ 4u]="'?'"; static char _temp1566[ 4u]="'+'"; static char
_temp1569[ 4u]="'.'"; static char _temp1572[ 4u]="'-'"; static char _temp1575[ 4u]="'&'";
static char _temp1578[ 4u]="'|'"; static char _temp1581[ 4u]="'^'"; static char
_temp1584[ 4u]="'/'"; static char _temp1587[ 4u]="'%'"; static char _temp1590[ 4u]="'~'";
static char _temp1593[ 4u]="'!'"; static char _temp1596[ 5u]="prog"; static char
_temp1599[ 17u]="translation_unit"; static char _temp1602[ 21u]="translation_unit_opt";
static char _temp1605[ 21u]="external_declaration"; static char _temp1608[ 20u]="function_definition";
static char _temp1611[ 21u]="function_definition2"; static char _temp1614[ 13u]="using_action";
static char _temp1617[ 15u]="unusing_action"; static char _temp1620[ 17u]="namespace_action";
static char _temp1623[ 19u]="unnamespace_action"; static char _temp1626[ 12u]="declaration";
static char _temp1629[ 17u]="declaration_list"; static char _temp1632[ 23u]="declaration_specifiers";
static char _temp1635[ 24u]="storage_class_specifier"; static char _temp1638[ 15u]="attributes_opt";
static char _temp1641[ 11u]="attributes"; static char _temp1644[ 15u]="attribute_list";
static char _temp1647[ 10u]="attribute"; static char _temp1650[ 15u]="type_specifier";
static char _temp1653[ 5u]="kind"; static char _temp1656[ 15u]="type_qualifier";
static char _temp1659[ 26u]="struct_or_union_specifier"; static char _temp1662[
16u]="type_params_opt"; static char _temp1665[ 16u]="struct_or_union"; static
char _temp1668[ 24u]="struct_declaration_list"; static char _temp1671[ 25u]="struct_declaration_list0";
static char _temp1674[ 21u]="init_declarator_list"; static char _temp1677[ 22u]="init_declarator_list0";
static char _temp1680[ 16u]="init_declarator"; static char _temp1683[ 19u]="struct_declaration";
static char _temp1686[ 25u]="specifier_qualifier_list"; static char _temp1689[
23u]="struct_declarator_list"; static char _temp1692[ 24u]="struct_declarator_list0";
static char _temp1695[ 18u]="struct_declarator"; static char _temp1698[ 15u]="enum_specifier";
static char _temp1701[ 16u]="enumerator_list"; static char _temp1704[ 11u]="enumerator";
static char _temp1707[ 11u]="declarator"; static char _temp1710[ 18u]="direct_declarator";
static char _temp1713[ 8u]="pointer"; static char _temp1716[ 13u]="pointer_char";
static char _temp1719[ 8u]="rgn_opt"; static char _temp1722[ 20u]="type_qualifier_list";
static char _temp1725[ 20u]="parameter_type_list"; static char _temp1728[ 11u]="effect_set";
static char _temp1731[ 14u]="atomic_effect"; static char _temp1734[ 11u]="region_set";
static char _temp1737[ 15u]="parameter_list"; static char _temp1740[ 22u]="parameter_declaration";
static char _temp1743[ 16u]="identifier_list"; static char _temp1746[ 17u]="identifier_list0";
static char _temp1749[ 12u]="initializer"; static char _temp1752[ 17u]="initializer_list";
static char _temp1755[ 12u]="designation"; static char _temp1758[ 16u]="designator_list";
static char _temp1761[ 11u]="designator"; static char _temp1764[ 10u]="type_name";
static char _temp1767[ 14u]="any_type_name"; static char _temp1770[ 15u]="type_name_list";
static char _temp1773[ 20u]="abstract_declarator"; static char _temp1776[ 27u]="direct_abstract_declarator";
static char _temp1779[ 10u]="statement"; static char _temp1782[ 18u]="labeled_statement";
static char _temp1785[ 21u]="expression_statement"; static char _temp1788[ 19u]="compound_statement";
static char _temp1791[ 16u]="block_item_list"; static char _temp1794[ 11u]="block_item";
static char _temp1797[ 20u]="selection_statement"; static char _temp1800[ 15u]="switch_clauses";
static char _temp1803[ 20u]="iteration_statement"; static char _temp1806[ 15u]="jump_statement";
static char _temp1809[ 8u]="pattern"; static char _temp1812[ 19u]="tuple_pattern_list";
static char _temp1815[ 20u]="tuple_pattern_list0"; static char _temp1818[ 14u]="field_pattern";
static char _temp1821[ 19u]="field_pattern_list"; static char _temp1824[ 20u]="field_pattern_list0";
static char _temp1827[ 11u]="expression"; static char _temp1830[ 22u]="assignment_expression";
static char _temp1833[ 20u]="assignment_operator"; static char _temp1836[ 23u]="conditional_expression";
static char _temp1839[ 20u]="constant_expression"; static char _temp1842[ 22u]="logical_or_expression";
static char _temp1845[ 23u]="logical_and_expression"; static char _temp1848[ 24u]="inclusive_or_expression";
static char _temp1851[ 24u]="exclusive_or_expression"; static char _temp1854[ 15u]="and_expression";
static char _temp1857[ 20u]="equality_expression"; static char _temp1860[ 22u]="relational_expression";
static char _temp1863[ 17u]="shift_expression"; static char _temp1866[ 20u]="additive_expression";
static char _temp1869[ 26u]="multiplicative_expression"; static char _temp1872[
16u]="cast_expression"; static char _temp1875[ 17u]="unary_expression"; static
char _temp1878[ 14u]="format_primop"; static char _temp1881[ 15u]="unary_operator";
static char _temp1884[ 19u]="postfix_expression"; static char _temp1887[ 19u]="primary_expression";
static char _temp1890[ 25u]="argument_expression_list"; static char _temp1893[
26u]="argument_expression_list0"; static char _temp1896[ 9u]="constant"; static
char _temp1899[ 20u]="qual_opt_identifier"; static struct _tagged_string Cyc_yytname[
220u]={( struct _tagged_string){ _temp1242, _temp1242, _temp1242 + 2u},( struct
_tagged_string){ _temp1245, _temp1245, _temp1245 + 6u},( struct _tagged_string){
_temp1248, _temp1248, _temp1248 + 12u},( struct _tagged_string){ _temp1251,
_temp1251, _temp1251 + 5u},( struct _tagged_string){ _temp1254, _temp1254,
_temp1254 + 9u},( struct _tagged_string){ _temp1257, _temp1257, _temp1257 + 7u},(
struct _tagged_string){ _temp1260, _temp1260, _temp1260 + 7u},( struct
_tagged_string){ _temp1263, _temp1263, _temp1263 + 8u},( struct _tagged_string){
_temp1266, _temp1266, _temp1266 + 5u},( struct _tagged_string){ _temp1269,
_temp1269, _temp1269 + 5u},( struct _tagged_string){ _temp1272, _temp1272,
_temp1272 + 6u},( struct _tagged_string){ _temp1275, _temp1275, _temp1275 + 4u},(
struct _tagged_string){ _temp1278, _temp1278, _temp1278 + 5u},( struct
_tagged_string){ _temp1281, _temp1281, _temp1281 + 6u},( struct _tagged_string){
_temp1284, _temp1284, _temp1284 + 7u},( struct _tagged_string){ _temp1287,
_temp1287, _temp1287 + 7u},( struct _tagged_string){ _temp1290, _temp1290,
_temp1290 + 9u},( struct _tagged_string){ _temp1293, _temp1293, _temp1293 + 6u},(
struct _tagged_string){ _temp1296, _temp1296, _temp1296 + 9u},( struct
_tagged_string){ _temp1299, _temp1299, _temp1299 + 9u},( struct _tagged_string){
_temp1302, _temp1302, _temp1302 + 7u},( struct _tagged_string){ _temp1305,
_temp1305, _temp1305 + 6u},( struct _tagged_string){ _temp1308, _temp1308,
_temp1308 + 5u},( struct _tagged_string){ _temp1311, _temp1311, _temp1311 + 8u},(
struct _tagged_string){ _temp1314, _temp1314, _temp1314 + 7u},( struct
_tagged_string){ _temp1317, _temp1317, _temp1317 + 3u},( struct _tagged_string){
_temp1320, _temp1320, _temp1320 + 5u},( struct _tagged_string){ _temp1323,
_temp1323, _temp1323 + 7u},( struct _tagged_string){ _temp1326, _temp1326,
_temp1326 + 6u},( struct _tagged_string){ _temp1329, _temp1329, _temp1329 + 3u},(
struct _tagged_string){ _temp1332, _temp1332, _temp1332 + 4u},( struct
_tagged_string){ _temp1335, _temp1335, _temp1335 + 5u},( struct _tagged_string){
_temp1338, _temp1338, _temp1338 + 9u},( struct _tagged_string){ _temp1341,
_temp1341, _temp1341 + 6u},( struct _tagged_string){ _temp1344, _temp1344,
_temp1344 + 7u},( struct _tagged_string){ _temp1347, _temp1347, _temp1347 + 7u},(
struct _tagged_string){ _temp1350, _temp1350, _temp1350 + 5u},( struct
_tagged_string){ _temp1353, _temp1353, _temp1353 + 8u},( struct _tagged_string){
_temp1356, _temp1356, _temp1356 + 4u},( struct _tagged_string){ _temp1359,
_temp1359, _temp1359 + 6u},( struct _tagged_string){ _temp1362, _temp1362,
_temp1362 + 4u},( struct _tagged_string){ _temp1365, _temp1365, _temp1365 + 6u},(
struct _tagged_string){ _temp1368, _temp1368, _temp1368 + 4u},( struct
_tagged_string){ _temp1371, _temp1371, _temp1371 + 9u},( struct _tagged_string){
_temp1374, _temp1374, _temp1374 + 9u},( struct _tagged_string){ _temp1377,
_temp1377, _temp1377 + 6u},( struct _tagged_string){ _temp1380, _temp1380,
_temp1380 + 10u},( struct _tagged_string){ _temp1383, _temp1383, _temp1383 + 6u},(
struct _tagged_string){ _temp1386, _temp1386, _temp1386 + 5u},( struct
_tagged_string){ _temp1389, _temp1389, _temp1389 + 8u},( struct _tagged_string){
_temp1392, _temp1392, _temp1392 + 4u},( struct _tagged_string){ _temp1395,
_temp1395, _temp1395 + 7u},( struct _tagged_string){ _temp1398, _temp1398,
_temp1398 + 7u},( struct _tagged_string){ _temp1401, _temp1401, _temp1401 + 8u},(
struct _tagged_string){ _temp1404, _temp1404, _temp1404 + 8u},( struct
_tagged_string){ _temp1407, _temp1407, _temp1407 + 6u},( struct _tagged_string){
_temp1410, _temp1410, _temp1410 + 7u},( struct _tagged_string){ _temp1413,
_temp1413, _temp1413 + 7u},( struct _tagged_string){ _temp1416, _temp1416,
_temp1416 + 7u},( struct _tagged_string){ _temp1419, _temp1419, _temp1419 + 7u},(
struct _tagged_string){ _temp1422, _temp1422, _temp1422 + 7u},( struct
_tagged_string){ _temp1425, _temp1425, _temp1425 + 8u},( struct _tagged_string){
_temp1428, _temp1428, _temp1428 + 9u},( struct _tagged_string){ _temp1431,
_temp1431, _temp1431 + 6u},( struct _tagged_string){ _temp1434, _temp1434,
_temp1434 + 6u},( struct _tagged_string){ _temp1437, _temp1437, _temp1437 + 6u},(
struct _tagged_string){ _temp1440, _temp1440, _temp1440 + 6u},( struct
_tagged_string){ _temp1443, _temp1443, _temp1443 + 7u},( struct _tagged_string){
_temp1446, _temp1446, _temp1446 + 6u},( struct _tagged_string){ _temp1449,
_temp1449, _temp1449 + 11u},( struct _tagged_string){ _temp1452, _temp1452,
_temp1452 + 11u},( struct _tagged_string){ _temp1455, _temp1455, _temp1455 + 11u},(
struct _tagged_string){ _temp1458, _temp1458, _temp1458 + 11u},( struct
_tagged_string){ _temp1461, _temp1461, _temp1461 + 11u},( struct _tagged_string){
_temp1464, _temp1464, _temp1464 + 12u},( struct _tagged_string){ _temp1467,
_temp1467, _temp1467 + 13u},( struct _tagged_string){ _temp1470, _temp1470,
_temp1470 + 11u},( struct _tagged_string){ _temp1473, _temp1473, _temp1473 + 11u},(
struct _tagged_string){ _temp1476, _temp1476, _temp1476 + 10u},( struct
_tagged_string){ _temp1479, _temp1479, _temp1479 + 9u},( struct _tagged_string){
_temp1482, _temp1482, _temp1482 + 11u},( struct _tagged_string){ _temp1485,
_temp1485, _temp1485 + 12u},( struct _tagged_string){ _temp1488, _temp1488,
_temp1488 + 11u},( struct _tagged_string){ _temp1491, _temp1491, _temp1491 + 17u},(
struct _tagged_string){ _temp1494, _temp1494, _temp1494 + 7u},( struct
_tagged_string){ _temp1497, _temp1497, _temp1497 + 19u},( struct _tagged_string){
_temp1500, _temp1500, _temp1500 + 18u},( struct _tagged_string){ _temp1503,
_temp1503, _temp1503 + 9u},( struct _tagged_string){ _temp1506, _temp1506,
_temp1506 + 16u},( struct _tagged_string){ _temp1509, _temp1509, _temp1509 + 18u},(
struct _tagged_string){ _temp1512, _temp1512, _temp1512 + 10u},( struct
_tagged_string){ _temp1515, _temp1515, _temp1515 + 4u},( struct _tagged_string){
_temp1518, _temp1518, _temp1518 + 4u},( struct _tagged_string){ _temp1521,
_temp1521, _temp1521 + 4u},( struct _tagged_string){ _temp1524, _temp1524,
_temp1524 + 4u},( struct _tagged_string){ _temp1527, _temp1527, _temp1527 + 4u},(
struct _tagged_string){ _temp1530, _temp1530, _temp1530 + 4u},( struct
_tagged_string){ _temp1533, _temp1533, _temp1533 + 4u},( struct _tagged_string){
_temp1536, _temp1536, _temp1536 + 4u},( struct _tagged_string){ _temp1539,
_temp1539, _temp1539 + 4u},( struct _tagged_string){ _temp1542, _temp1542,
_temp1542 + 4u},( struct _tagged_string){ _temp1545, _temp1545, _temp1545 + 4u},(
struct _tagged_string){ _temp1548, _temp1548, _temp1548 + 4u},( struct
_tagged_string){ _temp1551, _temp1551, _temp1551 + 4u},( struct _tagged_string){
_temp1554, _temp1554, _temp1554 + 4u},( struct _tagged_string){ _temp1557,
_temp1557, _temp1557 + 4u},( struct _tagged_string){ _temp1560, _temp1560,
_temp1560 + 4u},( struct _tagged_string){ _temp1563, _temp1563, _temp1563 + 4u},(
struct _tagged_string){ _temp1566, _temp1566, _temp1566 + 4u},( struct
_tagged_string){ _temp1569, _temp1569, _temp1569 + 4u},( struct _tagged_string){
_temp1572, _temp1572, _temp1572 + 4u},( struct _tagged_string){ _temp1575,
_temp1575, _temp1575 + 4u},( struct _tagged_string){ _temp1578, _temp1578,
_temp1578 + 4u},( struct _tagged_string){ _temp1581, _temp1581, _temp1581 + 4u},(
struct _tagged_string){ _temp1584, _temp1584, _temp1584 + 4u},( struct
_tagged_string){ _temp1587, _temp1587, _temp1587 + 4u},( struct _tagged_string){
_temp1590, _temp1590, _temp1590 + 4u},( struct _tagged_string){ _temp1593,
_temp1593, _temp1593 + 4u},( struct _tagged_string){ _temp1596, _temp1596,
_temp1596 + 5u},( struct _tagged_string){ _temp1599, _temp1599, _temp1599 + 17u},(
struct _tagged_string){ _temp1602, _temp1602, _temp1602 + 21u},( struct
_tagged_string){ _temp1605, _temp1605, _temp1605 + 21u},( struct _tagged_string){
_temp1608, _temp1608, _temp1608 + 20u},( struct _tagged_string){ _temp1611,
_temp1611, _temp1611 + 21u},( struct _tagged_string){ _temp1614, _temp1614,
_temp1614 + 13u},( struct _tagged_string){ _temp1617, _temp1617, _temp1617 + 15u},(
struct _tagged_string){ _temp1620, _temp1620, _temp1620 + 17u},( struct
_tagged_string){ _temp1623, _temp1623, _temp1623 + 19u},( struct _tagged_string){
_temp1626, _temp1626, _temp1626 + 12u},( struct _tagged_string){ _temp1629,
_temp1629, _temp1629 + 17u},( struct _tagged_string){ _temp1632, _temp1632,
_temp1632 + 23u},( struct _tagged_string){ _temp1635, _temp1635, _temp1635 + 24u},(
struct _tagged_string){ _temp1638, _temp1638, _temp1638 + 15u},( struct
_tagged_string){ _temp1641, _temp1641, _temp1641 + 11u},( struct _tagged_string){
_temp1644, _temp1644, _temp1644 + 15u},( struct _tagged_string){ _temp1647,
_temp1647, _temp1647 + 10u},( struct _tagged_string){ _temp1650, _temp1650,
_temp1650 + 15u},( struct _tagged_string){ _temp1653, _temp1653, _temp1653 + 5u},(
struct _tagged_string){ _temp1656, _temp1656, _temp1656 + 15u},( struct
_tagged_string){ _temp1659, _temp1659, _temp1659 + 26u},( struct _tagged_string){
_temp1662, _temp1662, _temp1662 + 16u},( struct _tagged_string){ _temp1665,
_temp1665, _temp1665 + 16u},( struct _tagged_string){ _temp1668, _temp1668,
_temp1668 + 24u},( struct _tagged_string){ _temp1671, _temp1671, _temp1671 + 25u},(
struct _tagged_string){ _temp1674, _temp1674, _temp1674 + 21u},( struct
_tagged_string){ _temp1677, _temp1677, _temp1677 + 22u},( struct _tagged_string){
_temp1680, _temp1680, _temp1680 + 16u},( struct _tagged_string){ _temp1683,
_temp1683, _temp1683 + 19u},( struct _tagged_string){ _temp1686, _temp1686,
_temp1686 + 25u},( struct _tagged_string){ _temp1689, _temp1689, _temp1689 + 23u},(
struct _tagged_string){ _temp1692, _temp1692, _temp1692 + 24u},( struct
_tagged_string){ _temp1695, _temp1695, _temp1695 + 18u},( struct _tagged_string){
_temp1698, _temp1698, _temp1698 + 15u},( struct _tagged_string){ _temp1701,
_temp1701, _temp1701 + 16u},( struct _tagged_string){ _temp1704, _temp1704,
_temp1704 + 11u},( struct _tagged_string){ _temp1707, _temp1707, _temp1707 + 11u},(
struct _tagged_string){ _temp1710, _temp1710, _temp1710 + 18u},( struct
_tagged_string){ _temp1713, _temp1713, _temp1713 + 8u},( struct _tagged_string){
_temp1716, _temp1716, _temp1716 + 13u},( struct _tagged_string){ _temp1719,
_temp1719, _temp1719 + 8u},( struct _tagged_string){ _temp1722, _temp1722,
_temp1722 + 20u},( struct _tagged_string){ _temp1725, _temp1725, _temp1725 + 20u},(
struct _tagged_string){ _temp1728, _temp1728, _temp1728 + 11u},( struct
_tagged_string){ _temp1731, _temp1731, _temp1731 + 14u},( struct _tagged_string){
_temp1734, _temp1734, _temp1734 + 11u},( struct _tagged_string){ _temp1737,
_temp1737, _temp1737 + 15u},( struct _tagged_string){ _temp1740, _temp1740,
_temp1740 + 22u},( struct _tagged_string){ _temp1743, _temp1743, _temp1743 + 16u},(
struct _tagged_string){ _temp1746, _temp1746, _temp1746 + 17u},( struct
_tagged_string){ _temp1749, _temp1749, _temp1749 + 12u},( struct _tagged_string){
_temp1752, _temp1752, _temp1752 + 17u},( struct _tagged_string){ _temp1755,
_temp1755, _temp1755 + 12u},( struct _tagged_string){ _temp1758, _temp1758,
_temp1758 + 16u},( struct _tagged_string){ _temp1761, _temp1761, _temp1761 + 11u},(
struct _tagged_string){ _temp1764, _temp1764, _temp1764 + 10u},( struct
_tagged_string){ _temp1767, _temp1767, _temp1767 + 14u},( struct _tagged_string){
_temp1770, _temp1770, _temp1770 + 15u},( struct _tagged_string){ _temp1773,
_temp1773, _temp1773 + 20u},( struct _tagged_string){ _temp1776, _temp1776,
_temp1776 + 27u},( struct _tagged_string){ _temp1779, _temp1779, _temp1779 + 10u},(
struct _tagged_string){ _temp1782, _temp1782, _temp1782 + 18u},( struct
_tagged_string){ _temp1785, _temp1785, _temp1785 + 21u},( struct _tagged_string){
_temp1788, _temp1788, _temp1788 + 19u},( struct _tagged_string){ _temp1791,
_temp1791, _temp1791 + 16u},( struct _tagged_string){ _temp1794, _temp1794,
_temp1794 + 11u},( struct _tagged_string){ _temp1797, _temp1797, _temp1797 + 20u},(
struct _tagged_string){ _temp1800, _temp1800, _temp1800 + 15u},( struct
_tagged_string){ _temp1803, _temp1803, _temp1803 + 20u},( struct _tagged_string){
_temp1806, _temp1806, _temp1806 + 15u},( struct _tagged_string){ _temp1809,
_temp1809, _temp1809 + 8u},( struct _tagged_string){ _temp1812, _temp1812,
_temp1812 + 19u},( struct _tagged_string){ _temp1815, _temp1815, _temp1815 + 20u},(
struct _tagged_string){ _temp1818, _temp1818, _temp1818 + 14u},( struct
_tagged_string){ _temp1821, _temp1821, _temp1821 + 19u},( struct _tagged_string){
_temp1824, _temp1824, _temp1824 + 20u},( struct _tagged_string){ _temp1827,
_temp1827, _temp1827 + 11u},( struct _tagged_string){ _temp1830, _temp1830,
_temp1830 + 22u},( struct _tagged_string){ _temp1833, _temp1833, _temp1833 + 20u},(
struct _tagged_string){ _temp1836, _temp1836, _temp1836 + 23u},( struct
_tagged_string){ _temp1839, _temp1839, _temp1839 + 20u},( struct _tagged_string){
_temp1842, _temp1842, _temp1842 + 22u},( struct _tagged_string){ _temp1845,
_temp1845, _temp1845 + 23u},( struct _tagged_string){ _temp1848, _temp1848,
_temp1848 + 24u},( struct _tagged_string){ _temp1851, _temp1851, _temp1851 + 24u},(
struct _tagged_string){ _temp1854, _temp1854, _temp1854 + 15u},( struct
_tagged_string){ _temp1857, _temp1857, _temp1857 + 20u},( struct _tagged_string){
_temp1860, _temp1860, _temp1860 + 22u},( struct _tagged_string){ _temp1863,
_temp1863, _temp1863 + 17u},( struct _tagged_string){ _temp1866, _temp1866,
_temp1866 + 20u},( struct _tagged_string){ _temp1869, _temp1869, _temp1869 + 26u},(
struct _tagged_string){ _temp1872, _temp1872, _temp1872 + 16u},( struct
_tagged_string){ _temp1875, _temp1875, _temp1875 + 17u},( struct _tagged_string){
_temp1878, _temp1878, _temp1878 + 14u},( struct _tagged_string){ _temp1881,
_temp1881, _temp1881 + 15u},( struct _tagged_string){ _temp1884, _temp1884,
_temp1884 + 19u},( struct _tagged_string){ _temp1887, _temp1887, _temp1887 + 19u},(
struct _tagged_string){ _temp1890, _temp1890, _temp1890 + 25u},( struct
_tagged_string){ _temp1893, _temp1893, _temp1893 + 26u},( struct _tagged_string){
_temp1896, _temp1896, _temp1896 + 9u},( struct _tagged_string){ _temp1899,
_temp1899, _temp1899 + 20u}}; static short Cyc_yyr1[ 379u]={ (short)0, (short)118,
(short)119, (short)119, (short)119, (short)119, (short)119, (short)119, (short)119,
(short)120, (short)120, (short)121, (short)121, (short)122, (short)122, (short)122,
(short)122, (short)123, (short)123, (short)124, (short)125, (short)126, (short)127,
(short)128, (short)128, (short)128, (short)129, (short)129, (short)130, (short)130,
(short)130, (short)130, (short)130, (short)130, (short)130, (short)130, (short)131,
(short)131, (short)131, (short)131, (short)131, (short)131, (short)131, (short)132,
(short)132, (short)133, (short)134, (short)134, (short)135, (short)135, (short)135,
(short)135, (short)136, (short)136, (short)136, (short)136, (short)136, (short)136,
(short)136, (short)136, (short)136, (short)136, (short)136, (short)136, (short)136,
(short)136, (short)136, (short)136, (short)137, (short)137, (short)138, (short)138,
(short)138, (short)139, (short)139, (short)139, (short)139, (short)139, (short)140,
(short)140, (short)140, (short)141, (short)141, (short)142, (short)143, (short)143,
(short)144, (short)145, (short)145, (short)146, (short)146, (short)147, (short)148,
(short)148, (short)148, (short)148, (short)149, (short)150, (short)150, (short)151,
(short)151, (short)151, (short)152, (short)152, (short)152, (short)152, (short)152,
(short)153, (short)153, (short)153, (short)153, (short)154, (short)154, (short)154,
(short)155, (short)155, (short)156, (short)156, (short)156, (short)156, (short)156,
(short)156, (short)156, (short)156, (short)156, (short)156, (short)156, (short)157,
(short)157, (short)157, (short)157, (short)158, (short)158, (short)158, (short)158,
(short)158, (short)159, (short)159, (short)159, (short)159, (short)160, (short)160,
(short)161, (short)161, (short)161, (short)162, (short)162, (short)163, (short)163,
(short)163, (short)163, (short)164, (short)164, (short)164, (short)164, (short)165,
(short)165, (short)166, (short)166, (short)166, (short)167, (short)168, (short)168,
(short)169, (short)169, (short)169, (short)169, (short)170, (short)170, (short)170,
(short)170, (short)171, (short)172, (short)172, (short)173, (short)173, (short)174,
(short)174, (short)175, (short)175, (short)175, (short)175, (short)176, (short)176,
(short)177, (short)177, (short)177, (short)178, (short)178, (short)178, (short)178,
(short)178, (short)178, (short)178, (short)178, (short)178, (short)178, (short)178,
(short)178, (short)178, (short)178, (short)179, (short)179, (short)179, (short)179,
(short)179, (short)179, (short)179, (short)179, (short)180, (short)181, (short)181,
(short)182, (short)182, (short)183, (short)183, (short)184, (short)184, (short)184,
(short)185, (short)185, (short)185, (short)185, (short)186, (short)186, (short)186,
(short)186, (short)186, (short)186, (short)187, (short)187, (short)187, (short)187,
(short)187, (short)187, (short)187, (short)187, (short)187, (short)187, (short)187,
(short)187, (short)187, (short)187, (short)188, (short)188, (short)188, (short)188,
(short)188, (short)188, (short)188, (short)188, (short)189, (short)189, (short)189,
(short)189, (short)189, (short)189, (short)189, (short)189, (short)189, (short)189,
(short)189, (short)189, (short)189, (short)189, (short)190, (short)190, (short)191,
(short)191, (short)192, (short)192, (short)193, (short)194, (short)194, (short)195,
(short)195, (short)196, (short)196, (short)197, (short)197, (short)197, (short)197,
(short)197, (short)197, (short)197, (short)197, (short)197, (short)197, (short)197,
(short)198, (short)198, (short)199, (short)200, (short)200, (short)201, (short)201,
(short)202, (short)202, (short)203, (short)203, (short)204, (short)204, (short)205,
(short)205, (short)205, (short)206, (short)206, (short)206, (short)206, (short)206,
(short)207, (short)207, (short)207, (short)208, (short)208, (short)208, (short)209,
(short)209, (short)209, (short)209, (short)210, (short)210, (short)211, (short)211,
(short)211, (short)211, (short)211, (short)211, (short)211, (short)211, (short)211,
(short)211, (short)211, (short)212, (short)212, (short)212, (short)212, (short)212,
(short)212, (short)213, (short)213, (short)213, (short)214, (short)214, (short)214,
(short)214, (short)214, (short)214, (short)214, (short)214, (short)214, (short)214,
(short)214, (short)214, (short)214, (short)214, (short)214, (short)214, (short)214,
(short)214, (short)214, (short)215, (short)215, (short)215, (short)215, (short)215,
(short)215, (short)215, (short)215, (short)215, (short)216, (short)217, (short)217,
(short)218, (short)218, (short)218, (short)218, (short)219, (short)219}; static
short Cyc_yyr2[ 379u]={ (short)0, (short)1, (short)1, (short)2, (short)3,
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
(short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1, (short)1,
(short)1, (short)1, (short)4, (short)3, (short)4, (short)3, (short)3, (short)3,
(short)3, (short)2, (short)2, (short)6, (short)7, (short)3, (short)4, (short)5,
(short)9, (short)2, (short)4, (short)4, (short)1, (short)1, (short)1, (short)3,
(short)2, (short)5, (short)4, (short)4, (short)5, (short)1, (short)1, (short)3,
(short)1, (short)1, (short)1, (short)1, (short)1, (short)1}; static short Cyc_yydefact[
705u]={ (short)0, (short)36, (short)37, (short)38, (short)39, (short)41, (short)52,
(short)54, (short)55, (short)56, (short)57, (short)58, (short)59, (short)60,
(short)61, (short)70, (short)71, (short)72, (short)81, (short)82, (short)43,
(short)0, (short)0, (short)42, (short)0, (short)0, (short)0, (short)377, (short)64,
(short)378, (short)78, (short)0, (short)53, (short)0, (short)131, (short)132,
(short)135, (short)1, (short)2, (short)11, (short)0, (short)0, (short)12,
(short)0, (short)43, (short)43, (short)43, (short)62, (short)0, (short)63,
(short)0, (short)114, (short)0, (short)136, (short)116, (short)40, (short)0,
(short)34, (short)44, (short)0, (short)78, (short)257, (short)253, (short)256,
(short)255, (short)0, (short)251, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)258, (short)19, (short)21, (short)106, (short)0, (short)0,
(short)66, (short)0, (short)0, (short)0, (short)0, (short)3, (short)0, (short)0,
(short)0, (short)0, (short)23, (short)0, (short)86, (short)87, (short)89,
(short)28, (short)30, (short)32, (short)78, (short)0, (short)78, (short)39,
(short)0, (short)26, (short)0, (short)0, (short)13, (short)0, (short)0, (short)0,
(short)126, (short)115, (short)137, (short)139, (short)127, (short)0, (short)0,
(short)35, (short)0, (short)107, (short)111, (short)104, (short)0, (short)265,
(short)264, (short)254, (short)263, (short)0, (short)0, (short)0, (short)68,
(short)69, (short)65, (short)0, (short)92, (short)94, (short)176, (short)178,
(short)182, (short)0, (short)117, (short)158, (short)0, (short)155, (short)0,
(short)376, (short)0, (short)0, (short)0, (short)0, (short)333, (short)334,
(short)335, (short)336, (short)337, (short)338, (short)0, (short)0, (short)373,
(short)363, (short)374, (short)375, (short)0, (short)0, (short)0, (short)0,
(short)341, (short)0, (short)339, (short)340, (short)0, (short)276, (short)289,
(short)292, (short)294, (short)296, (short)298, (short)300, (short)302, (short)305,
(short)310, (short)313, (short)316, (short)320, (short)0, (short)0, (short)322,
(short)342, (short)362, (short)361, (short)0, (short)4, (short)0, (short)6,
(short)0, (short)24, (short)0, (short)0, (short)0, (short)14, (short)29, (short)31,
(short)33, (short)77, (short)0, (short)83, (short)84, (short)0, (short)76,
(short)40, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)377, (short)210,
(short)212, (short)0, (short)218, (short)216, (short)0, (short)217, (short)201,
(short)202, (short)203, (short)0, (short)214, (short)204, (short)205, (short)206,
(short)0, (short)274, (short)27, (short)15, (short)89, (short)161, (short)0,
(short)121, (short)0, (short)142, (short)0, (short)160, (short)0, (short)118,
(short)0, (short)0, (short)140, (short)129, (short)128, (short)0, (short)0,
(short)102, (short)108, (short)0, (short)0, (short)0, (short)0, (short)252,
(short)267, (short)0, (short)266, (short)0, (short)0, (short)265, (short)0,
(short)151, (short)179, (short)0, (short)93, (short)95, (short)0, (short)0,
(short)184, (short)177, (short)185, (short)0, (short)80, (short)0, (short)79,
(short)0, (short)157, (short)184, (short)159, (short)67, (short)0, (short)0,
(short)330, (short)0, (short)331, (short)358, (short)0, (short)0, (short)0,
(short)323, (short)324, (short)0, (short)0, (short)0, (short)0, (short)326,
(short)320, (short)327, (short)325, (short)133, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)279, (short)280, (short)281, (short)282, (short)283, (short)284, (short)285,
(short)286, (short)287, (short)288, (short)278, (short)0, (short)0, (short)328,
(short)0, (short)350, (short)351, (short)0, (short)0, (short)0, (short)365,
(short)0, (short)0, (short)134, (short)20, (short)9, (short)22, (short)9,
(short)88, (short)0, (short)90, (short)163, (short)16, (short)0, (short)73,
(short)85, (short)0, (short)0, (short)96, (short)97, (short)99, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)244, (short)245,
(short)246, (short)0, (short)0, (short)248, (short)0, (short)207, (short)208,
(short)0, (short)0, (short)89, (short)213, (short)215, (short)211, (short)0,
(short)149, (short)0, (short)0, (short)145, (short)120, (short)0, (short)0,
(short)123, (short)0, (short)125, (short)124, (short)119, (short)138, (short)141,
(short)130, (short)9, (short)49, (short)48, (short)0, (short)46, (short)110,
(short)109, (short)291, (short)112, (short)0, (short)0, (short)260, (short)0,
(short)25, (short)261, (short)0, (short)0, (short)0, (short)0, (short)172,
(short)269, (short)272, (short)0, (short)271, (short)0, (short)105, (short)0,
(short)0, (short)180, (short)0, (short)192, (short)0, (short)0, (short)188,
(short)0, (short)186, (short)0, (short)0, (short)0, (short)200, (short)181,
(short)183, (short)156, (short)0, (short)0, (short)0, (short)354, (short)167,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)364,
(short)371, (short)0, (short)370, (short)293, (short)0, (short)295, (short)297,
(short)299, (short)301, (short)303, (short)304, (short)308, (short)309, (short)306,
(short)307, (short)311, (short)312, (short)314, (short)315, (short)317, (short)318,
(short)319, (short)277, (short)0, (short)348, (short)349, (short)344, (short)0,
(short)0, (short)346, (short)347, (short)0, (short)0, (short)10, (short)5,
(short)7, (short)164, (short)0, (short)0, (short)100, (short)91, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)243, (short)247, (short)0, (short)0, (short)0, (short)209,
(short)0, (short)17, (short)275, (short)0, (short)147, (short)0, (short)122,
(short)0, (short)144, (short)143, (short)162, (short)8, (short)0, (short)0,
(short)0, (short)0, (short)103, (short)268, (short)0, (short)175, (short)270,
(short)171, (short)173, (short)262, (short)0, (short)259, (short)153, (short)152,
(short)0, (short)194, (short)187, (short)190, (short)0, (short)195, (short)0,
(short)0, (short)189, (short)0, (short)329, (short)0, (short)0, (short)355,
(short)0, (short)168, (short)359, (short)360, (short)0, (short)0, (short)0,
(short)321, (short)367, (short)0, (short)0, (short)332, (short)345, (short)343,
(short)368, (short)0, (short)0, (short)165, (short)0, (short)75, (short)98,
(short)101, (short)74, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)223, (short)249, (short)0, (short)18,
(short)150, (short)148, (short)146, (short)0, (short)0, (short)45, (short)47,
(short)113, (short)174, (short)273, (short)0, (short)193, (short)0, (short)197,
(short)199, (short)198, (short)191, (short)0, (short)356, (short)169, (short)0,
(short)369, (short)0, (short)372, (short)290, (short)366, (short)166, (short)219,
(short)223, (short)229, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)250,
(short)50, (short)51, (short)154, (short)196, (short)0, (short)170, (short)352,
(short)0, (short)0, (short)0, (short)0, (short)231, (short)0, (short)0, (short)0,
(short)239, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)222, (short)0, (short)353, (short)220, (short)221, (short)230,
(short)232, (short)233, (short)0, (short)241, (short)240, (short)0, (short)235,
(short)0, (short)0, (short)0, (short)0, (short)223, (short)224, (short)0,
(short)234, (short)242, (short)236, (short)237, (short)0, (short)0, (short)223,
(short)225, (short)357, (short)238, (short)223, (short)226, (short)223, (short)227,
(short)228, (short)0, (short)0, (short)0}; static short Cyc_yydefgoto[ 102u]={
(short)702, (short)498, (short)499, (short)38, (short)39, (short)225, (short)40,
(short)355, (short)41, (short)357, (short)42, (short)196, (short)43, (short)44,
(short)57, (short)58, (short)412, (short)413, (short)45, (short)130, (short)46,
(short)47, (short)78, (short)48, (short)202, (short)203, (short)89, (short)90,
(short)91, (short)204, (short)139, (short)367, (short)368, (short)369, (short)49,
(short)116, (short)117, (short)50, (short)51, (short)52, (short)53, (short)112,
(short)255, (short)440, (short)396, (short)397, (short)274, (short)246, (short)141,
(short)247, (short)248, (short)456, (short)457, (short)458, (short)427, (short)428,
(short)135, (short)136, (short)137, (short)441, (short)281, (short)228, (short)229,
(short)230, (short)231, (short)232, (short)233, (short)234, (short)641, (short)235,
(short)236, (short)265, (short)266, (short)267, (short)430, (short)431, (short)432,
(short)237, (short)238, (short)341, (short)169, (short)417, (short)170, (short)171,
(short)172, (short)173, (short)174, (short)175, (short)176, (short)177, (short)178,
(short)179, (short)180, (short)181, (short)182, (short)183, (short)184, (short)185,
(short)466, (short)467, (short)186, (short)187}; static short Cyc_yypact[ 705u]={
(short)788, - (short)-32768, - (short)-32768, - (short)-32768, - (short)27, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)69, (short)72, (short)2494, - (short)-32768, - (short)5, - (short)35, -
(short)5, - (short)-32768, (short)26, - (short)-32768, - (short)33, (short)350,
- (short)-32768, (short)35, (short)51, (short)66, - (short)-32768, - (short)-32768,
(short)788, - (short)-32768, (short)184, (short)229, - (short)-32768, (short)200,
- (short)69, - (short)69, - (short)69, - (short)-32768, (short)111, - (short)-32768,
(short)1749, (short)86, (short)33, - (short)6, - (short)-32768, (short)76,
(short)101, (short)1796, - (short)-32768, - (short)5, - (short)33, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, (short)2494, - (short)-32768,
(short)129, (short)160, (short)181, (short)2494, (short)146, (short)102, -
(short)-32768, - (short)-32768, (short)210, (short)96, (short)1250, - (short)-32768,
(short)217, (short)1995, (short)3830, (short)3830, - (short)-32768, (short)788,
(short)788, (short)788, (short)788, - (short)-32768, (short)261, (short)232, -
(short)-32768, (short)1690, (short)1796, (short)1796, (short)1796, - (short)33,
(short)1995, - (short)33, (short)286, (short)1078, - (short)-32768, (short)1749,
(short)200, - (short)-32768, (short)1850, (short)1250, (short)606, - (short)-32768,
(short)86, (short)296, - (short)-32768, (short)122, (short)788, (short)293, -
(short)-32768, (short)308, (short)115, (short)183, (short)323, (short)321,
(short)2494, - (short)-32768, - (short)-32768, - (short)-32768, (short)3830,
(short)87, - (short)5, - (short)-32768, - (short)-32768, - (short)-32768,
(short)197, (short)1995, (short)1995, (short)209, - (short)-32768, (short)327,
(short)47, - (short)-32768, (short)318, (short)312, - (short)-32768, (short)3913,
- (short)-32768, (short)3996, (short)19, (short)338, (short)342, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
(short)3996, (short)3996, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)-32768, (short)1418, (short)351, (short)3830, (short)3830, - (short)-32768,
(short)3830, - (short)-32768, - (short)-32768, (short)365, - (short)-32768, -
(short)28, (short)353, (short)339, (short)349, (short)355, (short)363, (short)71,
(short)379, (short)100, (short)133, - (short)-32768, (short)532, (short)374,
(short)3830, (short)162, - (short)-32768, - (short)-32768, (short)53, (short)382,
- (short)-32768, (short)389, - (short)-32768, (short)408, - (short)-32768,
(short)350, (short)2502, (short)1749, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)-32768, (short)415, (short)412, (short)1995, - (short)-32768,
(short)437, (short)419, - (short)-32768, (short)422, (short)429, (short)431,
(short)1871, (short)438, (short)453, (short)446, (short)449, (short)2585,
(short)1871, (short)9, (short)1871, (short)1871, (short)443, - (short)-32768, -
(short)-32768, (short)468, - (short)-32768, - (short)-32768, (short)200, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)475,
(short)1193, - (short)-32768, - (short)-32768, - (short)-32768, (short)206, -
(short)-32768, - (short)-32768, - (short)-32768, (short)476, - (short)-32768,
(short)244, - (short)-32768, (short)478, (short)226, (short)479, (short)474,
(short)64, - (short)-32768, (short)480, (short)96, - (short)-32768, - (short)-32768,
(short)122, (short)485, (short)6, - (short)-32768, - (short)5, - (short)5,
(short)3830, (short)484, - (short)5, - (short)-32768, - (short)-32768, (short)487,
(short)483, (short)227, (short)819, (short)2494, (short)495, (short)20, -
(short)-32768, (short)497, - (short)-32768, - (short)-32768, (short)1582,
(short)2668, (short)131, - (short)-32768, (short)163, (short)244, - (short)-32768,
(short)1250, - (short)-32768, (short)741, - (short)-32768, (short)17, - (short)-32768,
- (short)-32768, (short)1995, (short)1418, - (short)-32768, (short)1418, -
(short)-32768, - (short)-32768, (short)2004, (short)3830, (short)1643, - (short)-32768,
- (short)-32768, (short)1193, (short)496, (short)347, (short)3830, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)3830,
(short)3830, (short)3830, (short)3830, (short)3830, (short)3830, (short)3830,
(short)3830, (short)3830, (short)3830, (short)3830, (short)3830, (short)3830,
(short)3830, (short)3830, (short)3830, (short)3830, (short)3830, (short)3830, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, (short)3830, (short)3830, - (short)-32768, (short)157, -
(short)-32768, - (short)-32768, (short)2751, (short)3830, (short)173, - (short)-32768,
(short)2419, (short)491, - (short)-32768, - (short)-32768, (short)788, - (short)-32768,
(short)788, - (short)-32768, (short)2087, - (short)-32768, - (short)-32768, -
(short)-32768, (short)1995, - (short)-32768, - (short)-32768, (short)3830,
(short)502, (short)498, - (short)-32768, (short)509, (short)1995, (short)3830,
(short)3830, (short)3830, (short)566, (short)1308, (short)508, - (short)-32768,
- (short)-32768, - (short)-32768, (short)231, (short)559, - (short)-32768,
(short)2834, - (short)-32768, - (short)-32768, (short)1871, (short)1528, (short)1690,
- (short)-32768, - (short)-32768, - (short)-32768, (short)3830, (short)531,
(short)245, (short)517, (short)507, - (short)-32768, (short)244, (short)1473, -
(short)-32768, (short)535, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)-32768, - (short)-32768, (short)788, - (short)-32768,
(short)523, (short)525, (short)530, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, (short)1995, (short)529, - (short)-32768, (short)2494, -
(short)-32768, - (short)-32768, (short)3830, (short)546, (short)2494, (short)81,
- (short)-32768, - (short)-32768, - (short)-32768, (short)537, (short)534,
(short)536, - (short)-32768, (short)96, (short)548, - (short)-32768, (short)244,
- (short)-32768, (short)540, (short)541, - (short)-32768, (short)538, (short)163,
(short)1981, (short)1250, (short)2917, - (short)-32768, - (short)-32768, (short)327,
- (short)-32768, (short)543, (short)544, (short)551, - (short)-32768, - (short)-32768,
(short)256, (short)2502, (short)352, (short)550, (short)350, (short)557, (short)3000,
- (short)-32768, - (short)-32768, (short)560, (short)547, (short)353, (short)283,
(short)339, (short)349, (short)355, (short)363, (short)71, (short)71, (short)379,
(short)379, (short)379, (short)379, (short)100, (short)100, (short)133, (short)133,
- (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)568,
- (short)-32768, - (short)-32768, - (short)-32768, (short)571, (short)170, -
(short)-32768, - (short)-32768, (short)267, (short)1250, - (short)-32768, -
(short)-32768, - (short)-32768, - (short)-32768, (short)297, (short)575, -
(short)-32768, - (short)-32768, (short)437, (short)3830, (short)577, (short)357,
(short)377, (short)388, (short)598, (short)3083, (short)3166, (short)249, -
(short)-32768, - (short)-32768, (short)604, (short)607, (short)601, - (short)-32768,
(short)1749, - (short)-32768, - (short)-32768, (short)96, - (short)-32768,
(short)609, - (short)-32768, (short)244, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)-32768, (short)407, (short)603, (short)6, (short)397, -
(short)-32768, - (short)-32768, (short)596, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)-32768, - (short)-32768, (short)273, - (short)-32768,
(short)610, - (short)-32768, (short)608, - (short)-32768, - (short)-32768, -
(short)-32768, (short)244, - (short)-32768, (short)612, (short)65, - (short)-32768,
(short)599, (short)614, (short)614, (short)613, - (short)-32768, (short)2170, -
(short)-32768, - (short)-32768, - (short)-32768, (short)1749, (short)616,
(short)2419, - (short)-32768, - (short)-32768, (short)3830, (short)3830, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, (short)2419,
(short)153, - (short)-32768, (short)2253, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)-32768, (short)1871, (short)623, (short)1871, (short)3830,
(short)3249, (short)266, (short)3332, (short)278, (short)3415, (short)473, -
(short)-32768, (short)627, - (short)-32768, - (short)-32768, - (short)-32768, -
(short)-32768, (short)624, (short)625, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)-32768, - (short)-32768, (short)548, - (short)-32768,
(short)629, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
(short)3830, - (short)-32768, - (short)-32768, (short)2502, - (short)-32768,
(short)317, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
(short)693, (short)473, - (short)-32768, (short)401, (short)1871, (short)413,
(short)3498, (short)1871, (short)418, (short)3581, (short)3664, (short)282,
(short)2494, (short)626, (short)633, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, (short)284, - (short)-32768, - (short)-32768,
(short)2336, (short)1871, (short)634, (short)638, - (short)-32768, (short)1871,
(short)1871, (short)432, - (short)-32768, (short)1871, (short)1871, (short)451,
(short)1871, (short)456, (short)3747, - (short)4, (short)1193, - (short)-32768,
(short)3830, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)-32768, (short)1871, - (short)-32768, - (short)-32768,
(short)1871, - (short)-32768, (short)1871, (short)1871, (short)458, (short)3830,
(short)963, - (short)-32768, (short)325, - (short)-32768, - (short)-32768, -
(short)-32768, - (short)-32768, (short)1871, (short)301, (short)473, - (short)-32768,
- (short)-32768, - (short)-32768, (short)963, - (short)-32768, (short)473, -
(short)-32768, - (short)-32768, (short)730, (short)732, - (short)-32768}; static
short Cyc_yypgoto[ 102u]={ - (short)-32768, (short)132, - (short)318, - (short)-32768,
(short)434, - (short)-32768, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)-32768, - (short)46, - (short)42, - (short)50, - (short)-32768, (short)248,
- (short)45, (short)198, - (short)-32768, (short)492, - (short)226, (short)454,
- (short)-32768, (short)18, - (short)-32768, - (short)128, - (short)-32768, -
(short)-32768, - (short)-32768, (short)542, (short)539, (short)41, - (short)-32768,
- (short)-32768, (short)233, - (short)-32768, - (short)107, - (short)-32768, -
(short)14, - (short)37, - (short)63, - (short)-32768, - (short)-32768, - (short)-32768,
- (short)94, - (short)366, (short)455, - (short)377, - (short)77, - (short)278,
- (short)-32768, - (short)-32768, - (short)190, - (short)329, - (short)245, -
(short)-32768, (short)314, - (short)125, (short)459, - (short)104, (short)268, -
(short)137, - (short)192, - (short)-32768, - (short)-32768, - (short)41, -
(short)232, - (short)-32768, - (short)-32768, - (short)591, - (short)-32768, -
(short)-32768, - (short)10, (short)465, - (short)-32768, (short)201, - (short)-32768,
- (short)-32768, - (short)87, (short)3, - (short)-32768, - (short)247, - (short)350,
- (short)-32768, (short)433, (short)450, (short)452, (short)457, (short)448,
(short)240, (short)158, (short)238, (short)239, - (short)97, (short)358, -
(short)-32768, - (short)-32768, - (short)-32768, - (short)-32768, - (short)260,
- (short)-32768, - (short)-32768, (short)10}; static short Cyc_yytable[ 4114u]={
(short)103, (short)391, (short)249, (short)140, (short)101, (short)360, (short)108,
(short)115, (short)102, (short)104, (short)54, (short)245, (short)71, (short)451,
(short)416, (short)109, (short)504, (short)79, (short)527, (short)375, (short)271,
(short)56, (short)496, (short)410, (short)426, (short)382, (short)406, (short)385,
(short)386, (short)92, (short)502, (short)60, (short)72, (short)530, (short)73,
(short)303, (short)75, (short)652, (short)268, (short)500, (short)311, (short)54,
(short)103, (short)198, (short)199, (short)200, (short)101, (short)74, (short)54,
(short)254, (short)227, (short)197, (short)103, (short)54, (short)226, (short)120,
(short)239, (short)55, (short)98, (short)549, (short)124, (short)240, (short)54,
(short)683, (short)108, (short)306, (short)308, (short)77, (short)309, (short)118,
(short)462, (short)279, (short)550, (short)304, (short)540, (short)72, (short)288,
(short)27, (short)119, (short)312, (short)72, (short)110, (short)488, (short)29,
(short)168, (short)188, (short)343, (short)492, (short)411, (short)241, (short)126,
(short)533, (short)111, (short)694, (short)54, (short)54, (short)54, (short)54,
(short)684, (short)27, (short)383, (short)435, (short)698, (short)296, (short)384,
(short)29, (short)700, (short)76, (short)701, (short)283, (short)251, (short)297,
(short)286, (short)54, (short)201, (short)27, (short)206, (short)436, (short)134,
(short)416, (short)278, (short)29, (short)451, (short)54, (short)520, (short)287,
(short)403, (short)614, (short)31, (short)381, (short)80, (short)72, (short)37,
(short)350, (short)319, (short)320, (short)262, (short)118, (short)205, (short)15,
(short)16, (short)17, (short)444, (short)81, (short)284, (short)351, (short)103,
(short)134, (short)285, (short)54, (short)239, (short)444, (short)414, (short)415,
(short)27, (short)362, (short)419, (short)585, (short)82, (short)352, (short)29,
(short)284, (short)284, (short)602, (short)59, (short)404, (short)615, (short)452,
(short)113, (short)453, (short)83, (short)321, (short)322, (short)275, (short)276,
(short)543, (short)56, (short)416, (short)128, (short)269, (short)241, (short)105,
(short)270, (short)227, (short)424, (short)129, (short)106, (short)226, (short)612,
(short)107, (short)425, (short)370, (short)408, (short)27, - (short)78, (short)521,
(short)114, - (short)78, (short)361, (short)29, (short)96, (short)134, (short)77,
(short)97, (short)54, (short)304, (short)259, (short)304, (short)325, (short)548,
(short)326, (short)459, (short)260, (short)389, (short)279, (short)54, (short)189,
(short)190, (short)191, (short)192, (short)344, (short)345, (short)346, (short)288,
(short)121, (short)469, (short)277, (short)34, (short)35, (short)36, (short)484,
(short)485, (short)486, (short)645, (short)278, (short)503, (short)448, (short)54,
(short)327, (short)489, (short)125, (short)622, (short)122, (short)508, (short)205,
(short)256, (short)490, (short)328, (short)329, (short)461, (short)284, (short)109,
(short)227, (short)56, (short)625, (short)494, (short)226, (short)347, (short)445,
(short)429, (short)416, (short)493, (short)495, (short)446, (short)123, (short)348,
(short)447, (short)393, (short)565, (short)118, (short)118, (short)349, (short)79,
(short)118, (short)577, (short)84, (short)85, (short)261, - (short)78, (short)72,
(short)72, (short)443, (short)27, (short)77, (short)272, (short)509, (short)510,
(short)511, (short)29, (short)515, (short)273, (short)88, (short)93, (short)94,
(short)95, (short)31, (short)54, (short)392, (short)54, (short)600, (short)361,
(short)426, (short)127, (short)393, (short)277, (short)34, (short)35, (short)36,
(short)465, (short)54, (short)61, (short)140, (short)278, (short)138, (short)34,
(short)35, (short)36, (short)399, (short)422, (short)620, (short)86, (short)87,
(short)517, (short)400, (short)393, (short)134, (short)103, (short)624, (short)393,
(short)194, (short)514, (short)394, (short)272, (short)134, (short)620, (short)134,
(short)395, (short)620, (short)526, (short)103, (short)595, (short)537, (short)557,
(short)101, (short)487, (short)465, (short)393, (short)522, (short)523, (short)563,
(short)465, (short)556, (short)193, (short)564, (short)361, (short)27, (short)62,
(short)633, (short)63, (short)64, (short)578, (short)29, (short)361, (short)393,
(short)579, (short)54, (short)571, (short)54, (short)65, (short)636, (short)207,
(short)66, (short)67, (short)664, (short)619, (short)393, (short)424, (short)252,
(short)68, (short)393, (short)393, (short)393, (short)425, (short)69, (short)70,
(short)574, (short)668, (short)465, (short)257, (short)619, (short)581, (short)465,
(short)619, (short)580, (short)582, (short)627, (short)524, (short)629, (short)393,
(short)448, (short)27, (short)258, (short)280, (short)697, (short)205, (short)620,
(short)29, (short)289, (short)290, (short)291, (short)649, (short)539, (short)205,
(short)286, (short)650, (short)263, (short)542, (short)264, (short)695, (short)54,
(short)313, (short)278, (short)393, (short)34, (short)35, (short)36, (short)592,
(short)594, (short)317, (short)318, (short)648, (short)72, (short)27, (short)298,
(short)685, (short)282, (short)72, (short)299, (short)29, (short)654, (short)323,
(short)324, (short)658, (short)464, (short)393, (short)31, (short)305, (short)568,
(short)566, (short)393, (short)559, (short)314, (short)693, (short)587, (short)393,
(short)34, (short)35, (short)36, (short)310, (short)670, (short)619, (short)361,
(short)315, (short)673, (short)674, (short)699, (short)316, (short)676, (short)677,
(short)342, (short)679, (short)54, (short)103, (short)588, (short)393, (short)353,
(short)239, (short)476, (short)477, (short)478, (short)479, (short)599, (short)354,
(short)687, (short)589, (short)393, (short)688, (short)134, (short)689, (short)690,
(short)603, (short)604, (short)370, (short)607, (short)291, (short)639, (short)640,
(short)653, (short)393, (short)696, (short)293, (short)356, (short)295, (short)630,
(short)632, (short)364, (short)635, (short)363, (short)638, (short)655, (short)393,
(short)371, (short)300, (short)301, (short)659, (short)393, (short)54, (short)372,
(short)103, (short)27, (short)307, (short)307, (short)101, (short)307, (short)373,
(short)29, (short)374, (short)197, (short)675, (short)393, (short)647, (short)133,
(short)31, (short)376, (short)133, (short)377, (short)429, (short)378, (short)134,
(short)366, (short)379, (short)307, (short)34, (short)35, (short)36, (short)387,
(short)657, (short)678, (short)393, (short)661, (short)663, (short)133, (short)680,
(short)393, (short)691, (short)393, (short)72, (short)474, (short)475, (short)133,
(short)133, (short)480, (short)481, (short)388, (short)482, (short)483, (short)253,
(short)361, (short)390, (short)132, (short)195, (short)402, (short)132, (short)361,
(short)398, (short)401, (short)623, (short)682, (short)409, (short)418, (short)421,
(short)686, (short)361, (short)420, (short)405, (short)361, (short)133, (short)133,
(short)434, (short)132, (short)437, (short)497, (short)463, (short)505, (short)512,
(short)506, (short)692, (short)132, (short)132, (short)516, (short)518, (short)330,
(short)331, (short)332, (short)333, (short)334, (short)335, (short)336, (short)337,
(short)338, (short)339, (short)507, (short)525, (short)528, (short)133, (short)529,
(short)227, (short)532, (short)534, (short)307, (short)226, (short)535, (short)538,
(short)361, (short)132, (short)132, (short)340, (short)536, (short)541, (short)665,
(short)545, (short)546, (short)547, (short)562, (short)227, (short)272, (short)551,
(short)552, (short)226, (short)560, (short)561, (short)142, (short)553, (short)143,
(short)573, (short)144, (short)567, (short)227, (short)145, (short)72, (short)569,
(short)226, (short)132, (short)361, (short)146, (short)147, (short)572, (short)133,
(short)148, (short)149, (short)150, (short)151, (short)152, (short)153, (short)575,
(short)154, (short)155, (short)576, (short)583, (short)307, (short)586, (short)307,
(short)307, (short)307, (short)307, (short)307, (short)307, (short)307, (short)307,
(short)307, (short)307, (short)307, (short)307, (short)307, (short)307, (short)307,
(short)307, (short)307, (short)27, (short)156, (short)157, (short)158, (short)159,
(short)590, (short)29, (short)132, (short)596, (short)598, (short)597, (short)605,
(short)608, (short)160, (short)601, (short)616, (short)611, (short)161, (short)570,
(short)610, (short)613, (short)407, (short)250, (short)162, (short)621, (short)617,
(short)163, (short)628, (short)164, (short)165, (short)642, (short)651, (short)643,
(short)644, (short)166, (short)167, (short)307, (short)646, (short)667, (short)671,
(short)666, (short)672, (short)703, (short)133, (short)704, (short)460, (short)606,
(short)433, (short)358, (short)449, (short)133, (short)584, (short)133, (short)544,
(short)365, (short)450, (short)468, (short)133, (short)133, (short)609, (short)133,
(short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)470,
(short)473, (short)0, (short)471, (short)0, (short)0, (short)132, (short)0,
(short)0, (short)472, (short)0, (short)0, (short)0, (short)132, (short)21,
(short)132, (short)0, (short)0, (short)0, (short)307, (short)132, (short)132,
(short)0, (short)132, (short)0, (short)26, (short)0, (short)0, (short)1, (short)2,
(short)3, (short)4, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)0, (short)0, (short)20, (short)0, (short)0, (short)0,
(short)0, (short)133, (short)0, (short)0, (short)0, (short)307, (short)0,
(short)27, (short)21, (short)133, (short)22, (short)0, (short)28, (short)29,
(short)30, (short)23, (short)438, (short)24, (short)25, (short)26, (short)286,
(short)439, (short)0, (short)32, (short)33, (short)0, (short)133, (short)0,
(short)278, (short)0, (short)34, (short)35, (short)36, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)133, (short)132, (short)61, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)132, (short)0, (short)307,
(short)0, (short)0, (short)0, (short)0, (short)27, (short)0, (short)133, (short)0,
(short)0, (short)28, (short)29, (short)30, (short)0, (short)0, (short)132,
(short)0, (short)0, (short)31, (short)0, (short)0, (short)32, (short)33, (short)0,
(short)0, (short)0, (short)0, (short)132, (short)34, (short)35, (short)36,
(short)0, (short)0, (short)0, (short)133, (short)133, (short)27, (short)62,
(short)0, (short)63, (short)64, (short)0, (short)29, (short)0, (short)0, (short)132,
(short)0, (short)423, (short)0, (short)65, (short)0, (short)0, (short)66,
(short)67, (short)0, (short)0, (short)0, (short)424, (short)0, (short)68,
(short)0, (short)0, (short)0, (short)425, (short)69, (short)70, (short)0,
(short)307, (short)0, (short)0, (short)0, (short)0, (short)132, (short)132,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)133, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)1, (short)2, (short)3, (short)99, (short)5,
(short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)639,
(short)640, (short)20, (short)208, (short)132, (short)209, (short)210, (short)211,
(short)212, (short)213, (short)214, (short)215, (short)216, (short)142, (short)21,
(short)143, (short)22, (short)144, (short)217, (short)0, (short)145, (short)23,
(short)218, (short)0, (short)0, (short)26, (short)146, (short)147, (short)219,
(short)220, (short)148, (short)149, (short)150, (short)151, (short)152, (short)153,
(short)0, (short)154, (short)155, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)221, (short)156, (short)157, (short)158, (short)159, (short)28,
(short)29, (short)30, (short)0, (short)222, (short)100, (short)0, (short)0,
(short)160, (short)0, (short)0, (short)32, (short)224, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)162, (short)0, (short)0, (short)163,
(short)0, (short)164, (short)165, (short)0, (short)0, (short)0, (short)0,
(short)166, (short)167, (short)1, (short)2, (short)3, (short)99, (short)5,
(short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)0,
(short)0, (short)20, (short)208, (short)0, (short)209, (short)210, (short)211,
(short)212, (short)213, (short)214, (short)215, (short)216, (short)142, (short)21,
(short)143, (short)22, (short)144, (short)217, (short)0, (short)145, (short)23,
(short)218, (short)0, (short)0, (short)26, (short)146, (short)147, (short)219,
(short)220, (short)148, (short)149, (short)150, (short)151, (short)152, (short)153,
(short)0, (short)154, (short)155, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)221, (short)156, (short)157, (short)158, (short)159, (short)28,
(short)29, (short)30, (short)0, (short)222, (short)100, (short)223, (short)0,
(short)160, (short)0, (short)0, (short)32, (short)224, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)162, (short)0, (short)0, (short)163,
(short)0, (short)164, (short)165, (short)0, (short)0, (short)0, (short)0,
(short)166, (short)167, (short)1, (short)2, (short)3, (short)99, (short)5,
(short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)0,
(short)0, (short)20, (short)208, (short)0, (short)209, (short)210, (short)211,
(short)212, (short)213, (short)214, (short)215, (short)216, (short)142, (short)21,
(short)143, (short)22, (short)144, (short)217, (short)0, (short)145, (short)23,
(short)218, (short)0, (short)0, (short)26, (short)146, (short)147, (short)219,
(short)220, (short)148, (short)149, (short)150, (short)151, (short)152, (short)153,
(short)0, (short)154, (short)155, (short)0, (short)0, (short)0, (short)0,
(short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)0,
(short)0, (short)0, (short)221, (short)156, (short)157, (short)158, (short)159,
(short)28, (short)29, (short)30, (short)0, (short)222, (short)100, (short)21,
(short)0, (short)160, (short)0, (short)0, (short)32, (short)224, (short)0,
(short)0, (short)0, (short)0, (short)26, (short)162, (short)0, (short)0, (short)163,
(short)0, (short)164, (short)165, (short)0, (short)0, (short)0, (short)0,
(short)166, (short)167, (short)1, (short)2, (short)3, (short)99, (short)5,
(short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)0,
(short)0, (short)20, (short)0, (short)0, (short)0, (short)0, (short)28, (short)0,
(short)30, (short)0, (short)0, (short)131, (short)142, (short)21, (short)143,
(short)22, (short)144, (short)32, (short)33, (short)145, (short)23, (short)0,
(short)0, (short)0, (short)26, (short)146, (short)147, (short)0, (short)0,
(short)148, (short)149, (short)150, (short)151, (short)152, (short)153, (short)0,
(short)154, (short)155, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)27, (short)156, (short)157, (short)158, (short)159, (short)28, (short)29,
(short)30, (short)0, (short)513, (short)0, (short)0, (short)0, (short)160,
(short)0, (short)0, (short)32, (short)224, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)162, (short)0, (short)0, (short)163, (short)0, (short)164,
(short)165, (short)0, (short)0, (short)0, (short)0, (short)166, (short)167,
(short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)142, (short)21, (short)143,
(short)0, (short)144, (short)0, (short)0, (short)145, (short)0, (short)0,
(short)0, (short)0, (short)26, (short)146, (short)147, (short)0, (short)0,
(short)148, (short)149, (short)150, (short)151, (short)152, (short)153, (short)0,
(short)154, (short)155, (short)0, (short)0, (short)6, (short)7, (short)8,
(short)9, (short)10, (short)11, (short)12, (short)13, (short)14, (short)15,
(short)16, (short)17, (short)18, (short)19, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)27, (short)156, (short)157, (short)158, (short)159,
(short)28, (short)29, (short)30, (short)0, (short)21, (short)302, (short)0,
(short)0, (short)160, (short)0, (short)0, (short)32, (short)224, (short)0,
(short)0, (short)26, (short)0, (short)0, (short)162, (short)0, (short)0, (short)163,
(short)0, (short)164, (short)165, (short)0, (short)0, (short)0, (short)0,
(short)166, (short)167, (short)6, (short)7, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)0, (short)0, (short)531, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)28, (short)0, (short)30,
(short)142, (short)21, (short)143, (short)0, (short)144, (short)0, (short)0,
(short)145, (short)32, (short)33, (short)0, (short)0, (short)26, (short)146,
(short)147, (short)0, (short)0, (short)148, (short)149, (short)150, (short)151,
(short)152, (short)153, (short)0, (short)154, (short)155, (short)0, (short)6,
(short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)27, (short)156, (short)157,
(short)158, (short)159, (short)28, (short)29, (short)30, (short)21, (short)0,
(short)0, (short)0, (short)0, (short)160, (short)0, (short)0, (short)32, (short)224,
(short)0, (short)26, (short)0, (short)0, (short)0, (short)162, (short)0, (short)0,
(short)163, (short)0, (short)164, (short)165, (short)0, (short)0, (short)0,
(short)0, (short)166, (short)167, (short)1, (short)2, (short)3, (short)99,
(short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)0, (short)0, (short)20, (short)0, (short)28, (short)0, (short)30, (short)0,
(short)438, (short)0, (short)0, (short)0, (short)277, (short)439, (short)21,
(short)32, (short)33, (short)0, (short)0, (short)0, (short)278, (short)23,
(short)34, (short)35, (short)36, (short)26, (short)0, (short)0, (short)1,
(short)2, (short)3, (short)99, (short)5, (short)6, (short)7, (short)8, (short)9,
(short)10, (short)11, (short)12, (short)13, (short)14, (short)15, (short)16,
(short)17, (short)18, (short)19, (short)0, (short)0, (short)20, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)27, (short)21, (short)0, (short)22, (short)0, (short)28,
(short)29, (short)30, (short)23, (short)0, (short)0, (short)0, (short)26,
(short)31, (short)0, (short)0, (short)32, (short)33, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)34, (short)35, (short)36, (short)0, (short)1, (short)2,
(short)3, (short)99, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)0, (short)0, (short)20, (short)0, (short)0, (short)0,
(short)28, (short)0, (short)30, (short)0, (short)0, (short)100, (short)0,
(short)195, (short)21, (short)0, (short)22, (short)32, (short)33, (short)0,
(short)0, (short)23, (short)0, (short)0, (short)0, (short)26, (short)0, (short)0,
(short)1, (short)2, (short)3, (short)99, (short)5, (short)6, (short)7, (short)8,
(short)9, (short)10, (short)11, (short)12, (short)13, (short)14, (short)15,
(short)16, (short)17, (short)18, (short)19, (short)0, (short)0, (short)20,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)21, (short)0, (short)0, (short)0, (short)28,
(short)0, (short)30, (short)23, (short)0, (short)100, (short)0, (short)26,
(short)0, (short)0, (short)0, (short)32, (short)33, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)6, (short)7,
(short)8, (short)9, (short)10, (short)11, (short)12, (short)13, (short)14,
(short)15, (short)16, (short)17, (short)18, (short)19, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)28, (short)0, (short)30, (short)21, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)32, (short)33, (short)208,
(short)26, (short)209, (short)210, (short)211, (short)212, (short)213, (short)214,
(short)215, (short)216, (short)142, (short)0, (short)143, (short)0, (short)144,
(short)217, (short)0, (short)145, (short)0, (short)218, (short)0, (short)0,
(short)0, (short)146, (short)147, (short)219, (short)220, (short)148, (short)149,
(short)150, (short)151, (short)152, (short)153, (short)0, (short)154, (short)155,
(short)242, (short)0, (short)0, (short)0, (short)0, (short)28, (short)0, (short)30,
(short)0, (short)243, (short)0, (short)0, (short)0, (short)0, (short)244,
(short)0, (short)32, (short)33, (short)0, (short)0, (short)0, (short)221,
(short)156, (short)157, (short)158, (short)159, (short)0, (short)29, (short)0,
(short)0, (short)222, (short)100, (short)0, (short)0, (short)160, (short)0,
(short)0, (short)0, (short)161, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)162, (short)0, (short)0, (short)163, (short)0, (short)164, (short)165,
(short)0, (short)0, (short)0, (short)0, (short)166, (short)167, (short)6,
(short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)6,
(short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)21,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)26, (short)0, (short)0, (short)21, (short)0, (short)0,
(short)454, (short)0, (short)0, (short)0, (short)0, (short)142, (short)0,
(short)143, (short)26, (short)144, (short)0, (short)0, (short)145, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)146, (short)147, (short)0,
(short)0, (short)148, (short)149, (short)150, (short)151, (short)152, (short)153,
(short)0, (short)154, (short)155, (short)0, (short)0, (short)0, (short)28,
(short)0, (short)30, (short)0, (short)554, (short)0, (short)0, (short)0, (short)0,
(short)555, (short)0, (short)32, (short)33, (short)0, (short)28, (short)0,
(short)30, (short)0, (short)27, (short)156, (short)157, (short)158, (short)159,
(short)0, (short)29, (short)32, (short)33, (short)0, (short)359, (short)455,
(short)0, (short)160, (short)0, (short)0, (short)0, (short)161, (short)0,
(short)0, (short)0, (short)424, (short)0, (short)162, (short)0, (short)0,
(short)163, (short)425, (short)164, (short)165, (short)0, (short)0, (short)0,
(short)0, (short)166, (short)167, (short)142, (short)0, (short)143, (short)0,
(short)144, (short)0, (short)0, (short)145, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)146, (short)147, (short)0, (short)0, (short)148,
(short)149, (short)150, (short)151, (short)152, (short)153, (short)0, (short)154,
(short)155, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)27, (short)156,
(short)157, (short)158, (short)159, (short)0, (short)29, (short)0, (short)0,
(short)0, (short)359, (short)501, (short)0, (short)160, (short)0, (short)0,
(short)0, (short)161, (short)0, (short)0, (short)0, (short)424, (short)0,
(short)162, (short)0, (short)0, (short)163, (short)425, (short)164, (short)165,
(short)0, (short)0, (short)0, (short)0, (short)166, (short)167, (short)142,
(short)0, (short)143, (short)0, (short)144, (short)0, (short)0, (short)145,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)146, (short)147,
(short)0, (short)0, (short)148, (short)149, (short)150, (short)151, (short)152,
(short)153, (short)0, (short)154, (short)155, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)27, (short)156, (short)157, (short)158, (short)159,
(short)0, (short)29, (short)0, (short)0, (short)0, (short)359, (short)618,
(short)0, (short)160, (short)0, (short)0, (short)0, (short)161, (short)0,
(short)0, (short)0, (short)424, (short)0, (short)162, (short)0, (short)0,
(short)163, (short)425, (short)164, (short)165, (short)0, (short)0, (short)0,
(short)0, (short)166, (short)167, (short)142, (short)0, (short)143, (short)0,
(short)144, (short)0, (short)0, (short)145, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)146, (short)147, (short)0, (short)0, (short)148,
(short)149, (short)150, (short)151, (short)152, (short)153, (short)0, (short)154,
(short)155, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)27, (short)156,
(short)157, (short)158, (short)159, (short)0, (short)29, (short)0, (short)0,
(short)0, (short)359, (short)626, (short)0, (short)160, (short)0, (short)0,
(short)0, (short)161, (short)0, (short)0, (short)0, (short)424, (short)0,
(short)162, (short)0, (short)0, (short)163, (short)425, (short)164, (short)165,
(short)0, (short)0, (short)0, (short)0, (short)166, (short)167, (short)142,
(short)0, (short)143, (short)0, (short)144, (short)0, (short)0, (short)145,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)146, (short)147,
(short)0, (short)0, (short)148, (short)149, (short)150, (short)151, (short)152,
(short)153, (short)0, (short)154, (short)155, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)27, (short)156, (short)157, (short)158, (short)159,
(short)0, (short)29, (short)0, (short)0, (short)0, (short)359, (short)669,
(short)0, (short)160, (short)0, (short)0, (short)0, (short)161, (short)0,
(short)0, (short)0, (short)424, (short)0, (short)162, (short)0, (short)0,
(short)163, (short)425, (short)164, (short)165, (short)0, (short)0, (short)0,
(short)0, (short)166, (short)167, (short)142, (short)0, (short)143, (short)0,
(short)144, (short)0, (short)0, (short)145, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)146, (short)147, (short)0, (short)0, (short)148,
(short)149, (short)150, (short)151, (short)152, (short)153, (short)0, (short)154,
(short)155, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)27, (short)156,
(short)157, (short)158, (short)159, (short)0, (short)29, (short)0, (short)0,
(short)0, (short)359, (short)0, (short)0, (short)160, (short)0, (short)0,
(short)0, (short)161, (short)0, (short)0, (short)0, (short)424, (short)0,
(short)162, (short)0, (short)0, (short)163, (short)425, (short)164, (short)165,
(short)61, (short)0, (short)0, (short)0, (short)166, (short)167, (short)142,
(short)0, (short)143, (short)0, (short)144, (short)0, (short)0, (short)145,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)146, (short)147,
(short)0, (short)0, (short)148, (short)149, (short)150, (short)151, (short)152,
(short)153, (short)0, (short)154, (short)155, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)27, (short)62, (short)0, (short)63, (short)64,
(short)0, (short)29, (short)0, (short)27, (short)156, (short)157, (short)158,
(short)159, (short)65, (short)29, (short)0, (short)66, (short)67, (short)359,
(short)0, (short)0, (short)160, (short)0, (short)68, (short)0, (short)161,
(short)0, (short)0, (short)69, (short)70, (short)0, (short)162, (short)0,
(short)0, (short)163, (short)0, (short)164, (short)165, (short)0, (short)0,
(short)0, (short)0, (short)166, (short)167, (short)142, (short)0, (short)143,
(short)0, (short)144, (short)0, (short)0, (short)145, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)146, (short)147, (short)0, (short)0,
(short)148, (short)149, (short)150, (short)151, (short)152, (short)153, (short)0,
(short)154, (short)155, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)27, (short)156, (short)157, (short)158, (short)159, (short)0, (short)29,
(short)0, (short)0, (short)380, (short)0, (short)0, (short)0, (short)160,
(short)0, (short)0, (short)0, (short)161, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)162, (short)0, (short)0, (short)163, (short)0, (short)164,
(short)165, (short)0, (short)0, (short)0, (short)0, (short)166, (short)167,
(short)142, (short)0, (short)143, (short)0, (short)144, (short)0, (short)0,
(short)145, (short)0, (short)0, (short)0, (short)0, (short)0, (short)146,
(short)147, (short)0, (short)0, (short)148, (short)149, (short)150, (short)151,
(short)152, (short)153, (short)0, (short)154, (short)155, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)27, (short)156, (short)157, (short)158,
(short)159, (short)0, (short)29, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)160, (short)0, (short)0, (short)0, (short)161, (short)0,
(short)0, (short)0, (short)0, (short)442, (short)162, (short)0, (short)0,
(short)163, (short)0, (short)164, (short)165, (short)0, (short)0, (short)0,
(short)0, (short)166, (short)167, (short)142, (short)0, (short)143, (short)0,
(short)144, (short)0, (short)0, (short)145, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)146, (short)147, (short)0, (short)0, (short)148,
(short)149, (short)150, (short)151, (short)152, (short)153, (short)0, (short)154,
(short)155, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)27, (short)156,
(short)157, (short)158, (short)159, (short)0, (short)29, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)160, (short)491, (short)0,
(short)0, (short)161, (short)0, (short)0, (short)0, (short)0, (short)0, (short)162,
(short)0, (short)0, (short)163, (short)0, (short)164, (short)165, (short)0,
(short)0, (short)0, (short)0, (short)166, (short)167, (short)142, (short)0,
(short)143, (short)0, (short)144, (short)0, (short)0, (short)145, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)146, (short)147, (short)0,
(short)0, (short)148, (short)149, (short)150, (short)151, (short)152, (short)153,
(short)0, (short)154, (short)155, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)27, (short)156, (short)157, (short)158, (short)159, (short)0,
(short)29, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)160,
(short)519, (short)0, (short)0, (short)161, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)162, (short)0, (short)0, (short)163, (short)0,
(short)164, (short)165, (short)0, (short)0, (short)0, (short)0, (short)166,
(short)167, (short)142, (short)0, (short)143, (short)0, (short)144, (short)0,
(short)0, (short)145, (short)0, (short)0, (short)0, (short)0, (short)0, (short)146,
(short)147, (short)0, (short)0, (short)148, (short)149, (short)150, (short)151,
(short)152, (short)153, (short)0, (short)154, (short)155, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)27, (short)156, (short)157, (short)158,
(short)159, (short)0, (short)29, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)160, (short)0, (short)0, (short)0, (short)161, (short)0,
(short)0, (short)0, (short)0, (short)558, (short)162, (short)0, (short)0,
(short)163, (short)0, (short)164, (short)165, (short)0, (short)0, (short)0,
(short)0, (short)166, (short)167, (short)142, (short)0, (short)143, (short)0,
(short)144, (short)0, (short)0, (short)145, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)146, (short)147, (short)0, (short)0, (short)148,
(short)149, (short)150, (short)151, (short)152, (short)153, (short)0, (short)154,
(short)155, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)27, (short)156,
(short)157, (short)158, (short)159, (short)0, (short)29, (short)0, (short)0,
(short)0, (short)570, (short)0, (short)0, (short)160, (short)0, (short)0,
(short)0, (short)161, (short)0, (short)0, (short)0, (short)0, (short)0, (short)162,
(short)0, (short)0, (short)163, (short)0, (short)164, (short)165, (short)0,
(short)0, (short)0, (short)0, (short)166, (short)167, (short)142, (short)0,
(short)143, (short)0, (short)144, (short)0, (short)0, (short)145, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)146, (short)147, (short)0,
(short)0, (short)148, (short)149, (short)150, (short)151, (short)152, (short)153,
(short)0, (short)154, (short)155, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)27, (short)156, (short)157, (short)158, (short)159, (short)0,
(short)29, (short)0, (short)0, (short)591, (short)0, (short)0, (short)0, (short)160,
(short)0, (short)0, (short)0, (short)161, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)162, (short)0, (short)0, (short)163, (short)0, (short)164,
(short)165, (short)0, (short)0, (short)0, (short)0, (short)166, (short)167,
(short)142, (short)0, (short)143, (short)0, (short)144, (short)0, (short)0,
(short)145, (short)0, (short)0, (short)0, (short)0, (short)0, (short)146,
(short)147, (short)0, (short)0, (short)148, (short)149, (short)150, (short)151,
(short)152, (short)153, (short)0, (short)154, (short)155, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)27, (short)156, (short)157, (short)158,
(short)159, (short)0, (short)29, (short)0, (short)0, (short)593, (short)0,
(short)0, (short)0, (short)160, (short)0, (short)0, (short)0, (short)161,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)162, (short)0, (short)0,
(short)163, (short)0, (short)164, (short)165, (short)0, (short)0, (short)0,
(short)0, (short)166, (short)167, (short)142, (short)0, (short)143, (short)0,
(short)144, (short)0, (short)0, (short)145, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)146, (short)147, (short)0, (short)0, (short)148,
(short)149, (short)150, (short)151, (short)152, (short)153, (short)0, (short)154,
(short)155, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)27, (short)156,
(short)157, (short)158, (short)159, (short)0, (short)29, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)160, (short)631, (short)0,
(short)0, (short)161, (short)0, (short)0, (short)0, (short)0, (short)0, (short)162,
(short)0, (short)0, (short)163, (short)0, (short)164, (short)165, (short)0,
(short)0, (short)0, (short)0, (short)166, (short)167, (short)142, (short)0,
(short)143, (short)0, (short)144, (short)0, (short)0, (short)145, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)146, (short)147, (short)0,
(short)0, (short)148, (short)149, (short)150, (short)151, (short)152, (short)153,
(short)0, (short)154, (short)155, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)27, (short)156, (short)157, (short)158, (short)159, (short)0,
(short)29, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)160,
(short)634, (short)0, (short)0, (short)161, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)162, (short)0, (short)0, (short)163, (short)0,
(short)164, (short)165, (short)0, (short)0, (short)0, (short)0, (short)166,
(short)167, (short)142, (short)0, (short)143, (short)0, (short)144, (short)0,
(short)0, (short)145, (short)0, (short)0, (short)0, (short)0, (short)0, (short)146,
(short)147, (short)0, (short)0, (short)148, (short)149, (short)150, (short)151,
(short)152, (short)153, (short)0, (short)154, (short)155, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)27, (short)156, (short)157, (short)158,
(short)159, (short)0, (short)29, (short)0, (short)0, (short)637, (short)0,
(short)0, (short)0, (short)160, (short)0, (short)0, (short)0, (short)161,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)162, (short)0, (short)0,
(short)163, (short)0, (short)164, (short)165, (short)0, (short)0, (short)0,
(short)0, (short)166, (short)167, (short)142, (short)0, (short)143, (short)0,
(short)144, (short)0, (short)0, (short)145, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)146, (short)147, (short)0, (short)0, (short)148,
(short)149, (short)150, (short)151, (short)152, (short)153, (short)0, (short)154,
(short)155, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)27, (short)156,
(short)157, (short)158, (short)159, (short)0, (short)29, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)160, (short)656, (short)0,
(short)0, (short)161, (short)0, (short)0, (short)0, (short)0, (short)0, (short)162,
(short)0, (short)0, (short)163, (short)0, (short)164, (short)165, (short)0,
(short)0, (short)0, (short)0, (short)166, (short)167, (short)142, (short)0,
(short)143, (short)0, (short)144, (short)0, (short)0, (short)145, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)146, (short)147, (short)0,
(short)0, (short)148, (short)149, (short)150, (short)151, (short)152, (short)153,
(short)0, (short)154, (short)155, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)27, (short)156, (short)157, (short)158, (short)159, (short)0,
(short)29, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)160,
(short)660, (short)0, (short)0, (short)161, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)162, (short)0, (short)0, (short)163, (short)0,
(short)164, (short)165, (short)0, (short)0, (short)0, (short)0, (short)166,
(short)167, (short)142, (short)0, (short)143, (short)0, (short)144, (short)0,
(short)0, (short)145, (short)0, (short)0, (short)0, (short)0, (short)0, (short)146,
(short)147, (short)0, (short)0, (short)148, (short)149, (short)150, (short)151,
(short)152, (short)153, (short)0, (short)154, (short)155, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)27, (short)156, (short)157, (short)158,
(short)159, (short)0, (short)29, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)160, (short)662, (short)0, (short)0, (short)161, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)162, (short)0, (short)0, (short)163,
(short)0, (short)164, (short)165, (short)0, (short)0, (short)0, (short)0,
(short)166, (short)167, (short)142, (short)0, (short)143, (short)0, (short)144,
(short)0, (short)0, (short)145, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)146, (short)147, (short)0, (short)0, (short)148, (short)149, (short)150,
(short)151, (short)152, (short)153, (short)0, (short)154, (short)155, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)27, (short)156, (short)157,
(short)158, (short)159, (short)0, (short)29, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)160, (short)681, (short)0, (short)0,
(short)161, (short)0, (short)0, (short)0, (short)0, (short)0, (short)162,
(short)0, (short)0, (short)163, (short)0, (short)164, (short)165, (short)0,
(short)0, (short)0, (short)0, (short)166, (short)167, (short)142, (short)0,
(short)143, (short)0, (short)144, (short)0, (short)0, (short)145, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)146, (short)147, (short)0,
(short)0, (short)148, (short)149, (short)150, (short)151, (short)152, (short)153,
(short)0, (short)154, (short)155, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)27, (short)156, (short)157, (short)158, (short)159, (short)0,
(short)29, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)160,
(short)0, (short)0, (short)0, (short)161, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)162, (short)0, (short)0, (short)163, (short)0, (short)164,
(short)165, (short)0, (short)0, (short)0, (short)0, (short)166, (short)167,
(short)142, (short)0, (short)143, (short)0, (short)144, (short)0, (short)0,
(short)145, (short)0, (short)0, (short)0, (short)0, (short)0, (short)146,
(short)147, (short)0, (short)0, (short)148, (short)149, (short)150, (short)151,
(short)152, (short)153, (short)0, (short)154, (short)155, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)27, (short)156, (short)157, (short)158,
(short)159, (short)0, (short)29, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)292, (short)0, (short)0, (short)0, (short)161, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)162, (short)0, (short)0, (short)163,
(short)0, (short)164, (short)165, (short)0, (short)0, (short)0, (short)0,
(short)166, (short)167, (short)142, (short)0, (short)143, (short)0, (short)144,
(short)0, (short)0, (short)145, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)146, (short)147, (short)0, (short)0, (short)148, (short)149, (short)150,
(short)151, (short)152, (short)153, (short)0, (short)154, (short)155, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)0, (short)27, (short)156, (short)157,
(short)158, (short)159, (short)0, (short)29, (short)0, (short)0, (short)0,
(short)0, (short)0, (short)0, (short)294, (short)0, (short)0, (short)0, (short)161,
(short)0, (short)0, (short)0, (short)0, (short)0, (short)162, (short)0, (short)0,
(short)163, (short)0, (short)164, (short)165, (short)0, (short)0, (short)0,
(short)0, (short)166, (short)167}; static short Cyc_yycheck[ 4114u]={ (short)50,
(short)233, (short)106, (short)80, (short)50, (short)195, (short)51, (short)57,
(short)50, (short)50, (short)0, (short)105, (short)22, (short)291, (short)261,
(short)52, (short)366, (short)31, (short)395, (short)211, (short)127, (short)90,
(short)351, (short)17, (short)269, (short)217, (short)252, (short)219, (short)220,
(short)43, (short)359, (short)21, (short)22, (short)399, (short)24, (short)160,
(short)26, (short)628, (short)125, (short)357, (short)68, (short)31, (short)92,
(short)93, (short)94, (short)95, (short)92, (short)82, (short)38, (short)112,
(short)100, (short)92, (short)102, (short)43, (short)100, (short)65, (short)102,
(short)84, (short)48, (short)436, (short)70, (short)102, (short)52, (short)67,
(short)109, (short)162, (short)163, (short)100, (short)165, (short)59, (short)302,
(short)134, (short)438, (short)160, (short)424, (short)65, (short)139, (short)82,
(short)60, (short)107, (short)70, (short)87, (short)342, (short)88, (short)81,
(short)82, (short)183, (short)347, (short)82, (short)103, (short)72, (short)409,
(short)98, (short)684, (short)84, (short)85, (short)86, (short)87, (short)102,
(short)82, (short)91, (short)81, (short)693, (short)84, (short)95, (short)88,
(short)697, (short)81, (short)699, (short)62, (short)107, (short)92, (short)95,
(short)103, (short)96, (short)82, (short)98, (short)97, (short)77, (short)366,
(short)103, (short)88, (short)400, (short)113, (short)384, (short)139, (short)62,
(short)62, (short)95, (short)216, (short)95, (short)121, (short)0, (short)80,
(short)63, (short)64, (short)118, (short)127, (short)97, (short)17, (short)18,
(short)19, (short)279, (short)92, (short)97, (short)92, (short)196, (short)106,
(short)101, (short)139, (short)196, (short)288, (short)259, (short)260, (short)82,
(short)196, (short)263, (short)507, (short)92, (short)106, (short)88, (short)97,
(short)97, (short)529, (short)92, (short)101, (short)101, (short)292, (short)92,
(short)294, (short)38, (short)100, (short)101, (short)132, (short)133, (short)94,
(short)90, (short)424, (short)82, (short)92, (short)194, (short)95, (short)95,
(short)233, (short)103, (short)89, (short)100, (short)233, (short)554, (short)103,
(short)109, (short)205, (short)255, (short)82, (short)92, (short)387, (short)95,
(short)95, (short)195, (short)88, (short)89, (short)160, (short)100, (short)92,
(short)194, (short)292, (short)91, (short)294, (short)108, (short)435, (short)110,
(short)298, (short)97, (short)227, (short)277, (short)205, (short)84, (short)85,
(short)86, (short)87, (short)58, (short)59, (short)60, (short)286, (short)95,
(short)312, (short)95, (short)105, (short)106, (short)107, (short)327, (short)328,
(short)329, (short)610, (short)103, (short)363, (short)281, (short)227, (short)105,
(short)82, (short)94, (short)570, (short)82, (short)371, (short)203, (short)113,
(short)89, (short)114, (short)115, (short)299, (short)97, (short)288, (short)302,
(short)90, (short)101, (short)82, (short)302, (short)95, (short)95, (short)269,
(short)507, (short)348, (short)89, (short)100, (short)83, (short)103, (short)103,
(short)97, (short)458, (short)259, (short)260, (short)109, (short)286, (short)263,
(short)104, (short)91, (short)92, (short)94, (short)95, (short)269, (short)270,
(short)278, (short)82, (short)100, (short)87, (short)372, (short)373, (short)374,
(short)88, (short)376, (short)93, (short)91, (short)44, (short)45, (short)46,
(short)95, (short)286, (short)91, (short)288, (short)525, (short)297, (short)546,
(short)92, (short)97, (short)95, (short)105, (short)106, (short)107, (short)305,
(short)299, (short)37, (short)388, (short)103, (short)96, (short)105, (short)106,
(short)107, (short)91, (short)91, (short)564, (short)91, (short)92, (short)91,
(short)97, (short)97, (short)284, (short)376, (short)574, (short)97, (short)97,
(short)376, (short)87, (short)87, (short)292, (short)579, (short)294, (short)92,
(short)582, (short)93, (short)389, (short)91, (short)418, (short)446, (short)389,
(short)341, (short)342, (short)97, (short)389, (short)389, (short)93, (short)347,
(short)445, (short)91, (short)97, (short)351, (short)82, (short)83, (short)91,
(short)85, (short)86, (short)93, (short)88, (short)359, (short)97, (short)97,
(short)355, (short)463, (short)357, (short)95, (short)91, (short)84, (short)98,
(short)99, (short)91, (short)564, (short)97, (short)103, (short)81, (short)105,
(short)97, (short)97, (short)97, (short)109, (short)110, (short)111, (short)102,
(short)102, (short)384, (short)95, (short)579, (short)93, (short)388, (short)582,
(short)497, (short)97, (short)587, (short)393, (short)589, (short)97, (short)444,
(short)82, (short)93, (short)134, (short)102, (short)363, (short)650, (short)88,
(short)139, (short)96, (short)97, (short)93, (short)421, (short)371, (short)95,
(short)97, (short)92, (short)426, (short)96, (short)93, (short)409, (short)67,
(short)103, (short)97, (short)105, (short)106, (short)107, (short)513, (short)514,
(short)65, (short)66, (short)620, (short)421, (short)82, (short)95, (short)666,
(short)108, (short)426, (short)95, (short)88, (short)631, (short)61, (short)62,
(short)634, (short)96, (short)97, (short)95, (short)95, (short)461, (short)96,
(short)97, (short)447, (short)112, (short)684, (short)96, (short)97, (short)105,
(short)106, (short)107, (short)93, (short)651, (short)650, (short)458, (short)113,
(short)655, (short)656, (short)697, (short)111, (short)659, (short)660, (short)95,
(short)662, (short)461, (short)522, (short)96, (short)97, (short)93, (short)522,
(short)319, (short)320, (short)321, (short)322, (short)522, (short)93, (short)675,
(short)96, (short)97, (short)678, (short)446, (short)680, (short)681, (short)83,
(short)84, (short)506, (short)96, (short)97, (short)22, (short)23, (short)96,
(short)97, (short)691, (short)142, (short)93, (short)144, (short)590, (short)591,
(short)93, (short)593, (short)92, (short)595, (short)96, (short)97, (short)92,
(short)154, (short)155, (short)96, (short)97, (short)506, (short)95, (short)568,
(short)82, (short)162, (short)163, (short)568, (short)165, (short)95, (short)88,
(short)95, (short)568, (short)96, (short)97, (short)617, (short)77, (short)95,
(short)95, (short)80, (short)82, (short)546, (short)91, (short)497, (short)102,
(short)91, (short)183, (short)105, (short)106, (short)107, (short)102, (short)633,
(short)96, (short)97, (short)636, (short)637, (short)97, (short)96, (short)97,
(short)96, (short)97, (short)546, (short)317, (short)318, (short)105, (short)106,
(short)323, (short)324, (short)95, (short)325, (short)326, (short)112, (short)564,
(short)93, (short)77, (short)94, (short)97, (short)80, (short)570, (short)96,
(short)96, (short)573, (short)664, (short)93, (short)95, (short)97, (short)668,
(short)579, (short)96, (short)104, (short)582, (short)132, (short)133, (short)93,
(short)97, (short)93, (short)100, (short)96, (short)91, (short)28, (short)97,
(short)683, (short)105, (short)106, (short)91, (short)41, (short)69, (short)70,
(short)71, (short)72, (short)73, (short)74, (short)75, (short)76, (short)77,
(short)78, (short)102, (short)81, (short)96, (short)160, (short)108, (short)666,
(short)82, (short)95, (short)261, (short)666, (short)96, (short)93, (short)620,
(short)132, (short)133, (short)94, (short)97, (short)82, (short)639, (short)93,
(short)97, (short)96, (short)82, (short)684, (short)87, (short)96, (short)96,
(short)684, (short)96, (short)96, (short)35, (short)104, (short)37, (short)97,
(short)39, (short)96, (short)697, (short)42, (short)639, (short)93, (short)697,
(short)160, (short)650, (short)48, (short)49, (short)96, (short)203, (short)52,
(short)53, (short)54, (short)55, (short)56, (short)57, (short)96, (short)59,
(short)60, (short)96, (short)93, (short)311, (short)93, (short)313, (short)314,
(short)315, (short)316, (short)317, (short)318, (short)319, (short)320, (short)321,
(short)322, (short)323, (short)324, (short)325, (short)326, (short)327, (short)328,
(short)329, (short)82, (short)83, (short)84, (short)85, (short)86, (short)95,
(short)88, (short)203, (short)92, (short)96, (short)91, (short)96, (short)104,
(short)95, (short)93, (short)104, (short)96, (short)99, (short)92, (short)97,
(short)96, (short)255, (short)104, (short)105, (short)96, (short)100, (short)108,
(short)92, (short)110, (short)111, (short)91, (short)26, (short)96, (short)96,
(short)116, (short)117, (short)366, (short)96, (short)93, (short)93, (short)102,
(short)91, (short)0, (short)277, (short)0, (short)299, (short)536, (short)270,
(short)194, (short)282, (short)284, (short)506, (short)286, (short)427, (short)203,
(short)284, (short)311, (short)291, (short)292, (short)546, (short)294, (short)8,
(short)9, (short)10, (short)11, (short)12, (short)13, (short)14, (short)15,
(short)16, (short)17, (short)18, (short)19, (short)20, (short)21, (short)313,
(short)316, - (short)1, (short)314, - (short)1, - (short)1, (short)277, -
(short)1, - (short)1, (short)315, - (short)1, - (short)1, - (short)1, (short)284,
(short)36, (short)286, - (short)1, - (short)1, - (short)1, (short)424, (short)291,
(short)292, - (short)1, (short)294, - (short)1, (short)47, - (short)1, - (short)1,
(short)3, (short)4, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)20, (short)21, - (short)1, - (short)1, (short)24, -
(short)1, - (short)1, - (short)1, - (short)1, (short)363, - (short)1, - (short)1,
- (short)1, (short)463, - (short)1, (short)82, (short)36, (short)371, (short)38,
- (short)1, (short)87, (short)88, (short)89, (short)43, (short)91, (short)45,
(short)46, (short)47, (short)95, (short)96, - (short)1, (short)98, (short)99, -
(short)1, (short)388, - (short)1, (short)103, - (short)1, (short)105, (short)106,
(short)107, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)400,
(short)363, (short)37, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)371, - (short)1, (short)507, - (short)1, - (short)1, -
(short)1, - (short)1, (short)82, - (short)1, (short)418, - (short)1, - (short)1,
(short)87, (short)88, (short)89, - (short)1, - (short)1, (short)388, - (short)1,
- (short)1, (short)95, - (short)1, - (short)1, (short)98, (short)99, - (short)1,
- (short)1, - (short)1, - (short)1, (short)400, (short)105, (short)106, (short)107,
- (short)1, - (short)1, - (short)1, (short)445, (short)446, (short)82, (short)83,
- (short)1, (short)85, (short)86, - (short)1, (short)88, - (short)1, - (short)1,
(short)418, - (short)1, (short)93, - (short)1, (short)95, - (short)1, - (short)1,
(short)98, (short)99, - (short)1, - (short)1, - (short)1, (short)103, - (short)1,
(short)105, - (short)1, - (short)1, - (short)1, (short)109, (short)110, (short)111,
- (short)1, (short)574, - (short)1, - (short)1, - (short)1, - (short)1, (short)445,
(short)446, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)497, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, (short)3, (short)4, (short)5, (short)6, (short)7,
(short)8, (short)9, (short)10, (short)11, (short)12, (short)13, (short)14,
(short)15, (short)16, (short)17, (short)18, (short)19, (short)20, (short)21,
(short)22, (short)23, (short)24, (short)25, (short)497, (short)27, (short)28,
(short)29, (short)30, (short)31, (short)32, (short)33, (short)34, (short)35,
(short)36, (short)37, (short)38, (short)39, (short)40, - (short)1, (short)42,
(short)43, (short)44, - (short)1, - (short)1, (short)47, (short)48, (short)49,
(short)50, (short)51, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)82, (short)83,
(short)84, (short)85, (short)86, (short)87, (short)88, (short)89, - (short)1,
(short)91, (short)92, - (short)1, - (short)1, (short)95, - (short)1, - (short)1,
(short)98, (short)99, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)105, - (short)1, - (short)1, (short)108, - (short)1, (short)110, (short)111,
- (short)1, - (short)1, - (short)1, - (short)1, (short)116, (short)117, (short)3,
(short)4, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)20, (short)21, - (short)1, - (short)1, (short)24, (short)25, -
(short)1, (short)27, (short)28, (short)29, (short)30, (short)31, (short)32,
(short)33, (short)34, (short)35, (short)36, (short)37, (short)38, (short)39,
(short)40, - (short)1, (short)42, (short)43, (short)44, - (short)1, - (short)1,
(short)47, (short)48, (short)49, (short)50, (short)51, (short)52, (short)53,
(short)54, (short)55, (short)56, (short)57, - (short)1, (short)59, (short)60, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)82, (short)83, (short)84, (short)85, (short)86, (short)87,
(short)88, (short)89, - (short)1, (short)91, (short)92, (short)93, - (short)1,
(short)95, - (short)1, - (short)1, (short)98, (short)99, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)105, - (short)1, - (short)1, (short)108,
- (short)1, (short)110, (short)111, - (short)1, - (short)1, - (short)1, -
(short)1, (short)116, (short)117, (short)3, (short)4, (short)5, (short)6,
(short)7, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)20,
(short)21, - (short)1, - (short)1, (short)24, (short)25, - (short)1, (short)27,
(short)28, (short)29, (short)30, (short)31, (short)32, (short)33, (short)34,
(short)35, (short)36, (short)37, (short)38, (short)39, (short)40, - (short)1,
(short)42, (short)43, (short)44, - (short)1, - (short)1, (short)47, (short)48,
(short)49, (short)50, (short)51, (short)52, (short)53, (short)54, (short)55,
(short)56, (short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1,
- (short)1, - (short)1, (short)8, (short)9, (short)10, (short)11, (short)12,
(short)13, (short)14, (short)15, (short)16, (short)17, (short)18, (short)19,
(short)20, (short)21, - (short)1, - (short)1, - (short)1, (short)82, (short)83,
(short)84, (short)85, (short)86, (short)87, (short)88, (short)89, - (short)1,
(short)91, (short)92, (short)36, - (short)1, (short)95, - (short)1, - (short)1,
(short)98, (short)99, - (short)1, - (short)1, - (short)1, - (short)1, (short)47,
(short)105, - (short)1, - (short)1, (short)108, - (short)1, (short)110, (short)111,
- (short)1, - (short)1, - (short)1, - (short)1, (short)116, (short)117, (short)3,
(short)4, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)20, (short)21, - (short)1, - (short)1, (short)24, - (short)1,
- (short)1, - (short)1, - (short)1, (short)87, - (short)1, (short)89, - (short)1,
- (short)1, (short)92, (short)35, (short)36, (short)37, (short)38, (short)39,
(short)98, (short)99, (short)42, (short)43, - (short)1, - (short)1, - (short)1,
(short)47, (short)48, (short)49, - (short)1, - (short)1, (short)52, (short)53,
(short)54, (short)55, (short)56, (short)57, - (short)1, (short)59, (short)60, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)82, (short)83, (short)84, (short)85, (short)86, (short)87,
(short)88, (short)89, - (short)1, (short)91, - (short)1, - (short)1, - (short)1,
(short)95, - (short)1, - (short)1, (short)98, (short)99, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)105, - (short)1, - (short)1, (short)108,
- (short)1, (short)110, (short)111, - (short)1, - (short)1, - (short)1, -
(short)1, (short)116, (short)117, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)20, (short)21, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, (short)35, (short)36, (short)37, - (short)1,
(short)39, - (short)1, - (short)1, (short)42, - (short)1, - (short)1, - (short)1,
- (short)1, (short)47, (short)48, (short)49, - (short)1, - (short)1, (short)52,
(short)53, (short)54, (short)55, (short)56, (short)57, - (short)1, (short)59,
(short)60, - (short)1, - (short)1, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)20, (short)21, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, (short)82, (short)83, (short)84, (short)85, (short)86, (short)87,
(short)88, (short)89, - (short)1, (short)36, (short)92, - (short)1, - (short)1,
(short)95, - (short)1, - (short)1, (short)98, (short)99, - (short)1, - (short)1,
(short)47, - (short)1, - (short)1, (short)105, - (short)1, - (short)1, (short)108,
- (short)1, (short)110, (short)111, - (short)1, - (short)1, - (short)1, -
(short)1, (short)116, (short)117, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)20, (short)21, - (short)1, - (short)1, (short)79, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)87,
- (short)1, (short)89, (short)35, (short)36, (short)37, - (short)1, (short)39, -
(short)1, - (short)1, (short)42, (short)98, (short)99, - (short)1, - (short)1,
(short)47, (short)48, (short)49, - (short)1, - (short)1, (short)52, (short)53,
(short)54, (short)55, (short)56, (short)57, - (short)1, (short)59, (short)60, -
(short)1, (short)8, (short)9, (short)10, (short)11, (short)12, (short)13,
(short)14, (short)15, (short)16, (short)17, (short)18, (short)19, (short)20,
(short)21, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)82, (short)83, (short)84, (short)85, (short)86, (short)87, (short)88,
(short)89, (short)36, - (short)1, - (short)1, - (short)1, - (short)1, (short)95,
- (short)1, - (short)1, (short)98, (short)99, - (short)1, (short)47, - (short)1,
- (short)1, - (short)1, (short)105, - (short)1, - (short)1, (short)108, -
(short)1, (short)110, (short)111, - (short)1, - (short)1, - (short)1, - (short)1,
(short)116, (short)117, (short)3, (short)4, (short)5, (short)6, (short)7,
(short)8, (short)9, (short)10, (short)11, (short)12, (short)13, (short)14,
(short)15, (short)16, (short)17, (short)18, (short)19, (short)20, (short)21, -
(short)1, - (short)1, (short)24, - (short)1, (short)87, - (short)1, (short)89, -
(short)1, (short)91, - (short)1, - (short)1, - (short)1, (short)95, (short)96,
(short)36, (short)98, (short)99, - (short)1, - (short)1, - (short)1, (short)103,
(short)43, (short)105, (short)106, (short)107, (short)47, - (short)1, - (short)1,
(short)3, (short)4, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)20, (short)21, - (short)1, - (short)1, (short)24, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)82, (short)36, - (short)1, (short)38,
- (short)1, (short)87, (short)88, (short)89, (short)43, - (short)1, - (short)1,
- (short)1, (short)47, (short)95, - (short)1, - (short)1, (short)98, (short)99,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)105, (short)106,
(short)107, - (short)1, (short)3, (short)4, (short)5, (short)6, (short)7,
(short)8, (short)9, (short)10, (short)11, (short)12, (short)13, (short)14,
(short)15, (short)16, (short)17, (short)18, (short)19, (short)20, (short)21, -
(short)1, - (short)1, (short)24, - (short)1, - (short)1, - (short)1, (short)87,
- (short)1, (short)89, - (short)1, - (short)1, (short)92, - (short)1, (short)94,
(short)36, - (short)1, (short)38, (short)98, (short)99, - (short)1, - (short)1,
(short)43, - (short)1, - (short)1, - (short)1, (short)47, - (short)1, - (short)1,
(short)3, (short)4, (short)5, (short)6, (short)7, (short)8, (short)9, (short)10,
(short)11, (short)12, (short)13, (short)14, (short)15, (short)16, (short)17,
(short)18, (short)19, (short)20, (short)21, - (short)1, - (short)1, (short)24, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)36, - (short)1, - (short)1,
- (short)1, (short)87, - (short)1, (short)89, (short)43, - (short)1, (short)92,
- (short)1, (short)47, - (short)1, - (short)1, - (short)1, (short)98, (short)99,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)20, (short)21, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, (short)87, - (short)1, (short)89, (short)36, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)98, (short)99,
(short)25, (short)47, (short)27, (short)28, (short)29, (short)30, (short)31,
(short)32, (short)33, (short)34, (short)35, - (short)1, (short)37, - (short)1,
(short)39, (short)40, - (short)1, (short)42, - (short)1, (short)44, - (short)1,
- (short)1, - (short)1, (short)48, (short)49, (short)50, (short)51, (short)52,
(short)53, (short)54, (short)55, (short)56, (short)57, - (short)1, (short)59,
(short)60, (short)82, - (short)1, - (short)1, - (short)1, - (short)1, (short)87,
- (short)1, (short)89, - (short)1, (short)91, - (short)1, - (short)1, - (short)1,
- (short)1, (short)96, - (short)1, (short)98, (short)99, - (short)1, - (short)1,
- (short)1, (short)82, (short)83, (short)84, (short)85, (short)86, - (short)1,
(short)88, - (short)1, - (short)1, (short)91, (short)92, - (short)1, - (short)1,
(short)95, - (short)1, - (short)1, - (short)1, (short)99, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, (short)105, - (short)1, - (short)1, (short)108,
- (short)1, (short)110, (short)111, - (short)1, - (short)1, - (short)1, -
(short)1, (short)116, (short)117, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)20, (short)21, (short)8, (short)9, (short)10, (short)11,
(short)12, (short)13, (short)14, (short)15, (short)16, (short)17, (short)18,
(short)19, (short)20, (short)21, (short)36, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, (short)47, - (short)1, - (short)1, (short)36, - (short)1, - (short)1,
(short)30, - (short)1, - (short)1, - (short)1, - (short)1, (short)35, - (short)1,
(short)37, (short)47, (short)39, - (short)1, - (short)1, (short)42, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49, - (short)1,
- (short)1, (short)52, (short)53, (short)54, (short)55, (short)56, (short)57, -
(short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1, (short)87, -
(short)1, (short)89, - (short)1, (short)91, - (short)1, - (short)1, - (short)1,
- (short)1, (short)96, - (short)1, (short)98, (short)99, - (short)1, (short)87,
- (short)1, (short)89, - (short)1, (short)82, (short)83, (short)84, (short)85,
(short)86, - (short)1, (short)88, (short)98, (short)99, - (short)1, (short)92,
(short)93, - (short)1, (short)95, - (short)1, - (short)1, - (short)1, (short)99,
- (short)1, - (short)1, - (short)1, (short)103, - (short)1, (short)105, -
(short)1, - (short)1, (short)108, (short)109, (short)110, (short)111, - (short)1,
- (short)1, - (short)1, - (short)1, (short)116, (short)117, (short)35, - (short)1,
(short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49, - (short)1,
- (short)1, (short)52, (short)53, (short)54, (short)55, (short)56, (short)57, -
(short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)82, (short)83, (short)84,
(short)85, (short)86, - (short)1, (short)88, - (short)1, - (short)1, - (short)1,
(short)92, (short)93, - (short)1, (short)95, - (short)1, - (short)1, - (short)1,
(short)99, - (short)1, - (short)1, - (short)1, (short)103, - (short)1, (short)105,
- (short)1, - (short)1, (short)108, (short)109, (short)110, (short)111, -
(short)1, - (short)1, - (short)1, - (short)1, (short)116, (short)117, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)82, (short)83,
(short)84, (short)85, (short)86, - (short)1, (short)88, - (short)1, - (short)1,
- (short)1, (short)92, (short)93, - (short)1, (short)95, - (short)1, - (short)1,
- (short)1, (short)99, - (short)1, - (short)1, - (short)1, (short)103, - (short)1,
(short)105, - (short)1, - (short)1, (short)108, (short)109, (short)110, (short)111,
- (short)1, - (short)1, - (short)1, - (short)1, (short)116, (short)117, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)82, (short)83,
(short)84, (short)85, (short)86, - (short)1, (short)88, - (short)1, - (short)1,
- (short)1, (short)92, (short)93, - (short)1, (short)95, - (short)1, - (short)1,
- (short)1, (short)99, - (short)1, - (short)1, - (short)1, (short)103, - (short)1,
(short)105, - (short)1, - (short)1, (short)108, (short)109, (short)110, (short)111,
- (short)1, - (short)1, - (short)1, - (short)1, (short)116, (short)117, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)82, (short)83,
(short)84, (short)85, (short)86, - (short)1, (short)88, - (short)1, - (short)1,
- (short)1, (short)92, (short)93, - (short)1, (short)95, - (short)1, - (short)1,
- (short)1, (short)99, - (short)1, - (short)1, - (short)1, (short)103, - (short)1,
(short)105, - (short)1, - (short)1, (short)108, (short)109, (short)110, (short)111,
- (short)1, - (short)1, - (short)1, - (short)1, (short)116, (short)117, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)82, (short)83,
(short)84, (short)85, (short)86, - (short)1, (short)88, - (short)1, - (short)1,
- (short)1, (short)92, - (short)1, - (short)1, (short)95, - (short)1, - (short)1,
- (short)1, (short)99, - (short)1, - (short)1, - (short)1, (short)103, - (short)1,
(short)105, - (short)1, - (short)1, (short)108, (short)109, (short)110, (short)111,
(short)37, - (short)1, - (short)1, - (short)1, (short)116, (short)117, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, (short)82, (short)83, - (short)1,
(short)85, (short)86, - (short)1, (short)88, - (short)1, (short)82, (short)83,
(short)84, (short)85, (short)86, (short)95, (short)88, - (short)1, (short)98,
(short)99, (short)92, - (short)1, - (short)1, (short)95, - (short)1, (short)105,
- (short)1, (short)99, - (short)1, - (short)1, (short)110, (short)111, - (short)1,
(short)105, - (short)1, - (short)1, (short)108, - (short)1, (short)110, (short)111,
- (short)1, - (short)1, - (short)1, - (short)1, (short)116, (short)117, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)82, (short)83,
(short)84, (short)85, (short)86, - (short)1, (short)88, - (short)1, - (short)1,
(short)91, - (short)1, - (short)1, - (short)1, (short)95, - (short)1, - (short)1,
- (short)1, (short)99, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)105, - (short)1, - (short)1, (short)108, - (short)1, (short)110, (short)111,
- (short)1, - (short)1, - (short)1, - (short)1, (short)116, (short)117, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)82, (short)83,
(short)84, (short)85, (short)86, - (short)1, (short)88, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)95, - (short)1, - (short)1,
- (short)1, (short)99, - (short)1, - (short)1, - (short)1, - (short)1, (short)104,
(short)105, - (short)1, - (short)1, (short)108, - (short)1, (short)110, (short)111,
- (short)1, - (short)1, - (short)1, - (short)1, (short)116, (short)117, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)82, (short)83,
(short)84, (short)85, (short)86, - (short)1, (short)88, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)95, (short)96, - (short)1,
- (short)1, (short)99, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)105, - (short)1, - (short)1, (short)108, - (short)1, (short)110, (short)111,
- (short)1, - (short)1, - (short)1, - (short)1, (short)116, (short)117, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)82, (short)83,
(short)84, (short)85, (short)86, - (short)1, (short)88, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)95, (short)96, - (short)1,
- (short)1, (short)99, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)105, - (short)1, - (short)1, (short)108, - (short)1, (short)110, (short)111,
- (short)1, - (short)1, - (short)1, - (short)1, (short)116, (short)117, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)82, (short)83,
(short)84, (short)85, (short)86, - (short)1, (short)88, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)95, - (short)1, - (short)1,
- (short)1, (short)99, - (short)1, - (short)1, - (short)1, - (short)1, (short)104,
(short)105, - (short)1, - (short)1, (short)108, - (short)1, (short)110, (short)111,
- (short)1, - (short)1, - (short)1, - (short)1, (short)116, (short)117, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)82, (short)83,
(short)84, (short)85, (short)86, - (short)1, (short)88, - (short)1, - (short)1,
- (short)1, (short)92, - (short)1, - (short)1, (short)95, - (short)1, - (short)1,
- (short)1, (short)99, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)105, - (short)1, - (short)1, (short)108, - (short)1, (short)110, (short)111,
- (short)1, - (short)1, - (short)1, - (short)1, (short)116, (short)117, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)82, (short)83,
(short)84, (short)85, (short)86, - (short)1, (short)88, - (short)1, - (short)1,
(short)91, - (short)1, - (short)1, - (short)1, (short)95, - (short)1, - (short)1,
- (short)1, (short)99, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)105, - (short)1, - (short)1, (short)108, - (short)1, (short)110, (short)111,
- (short)1, - (short)1, - (short)1, - (short)1, (short)116, (short)117, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)82, (short)83,
(short)84, (short)85, (short)86, - (short)1, (short)88, - (short)1, - (short)1,
(short)91, - (short)1, - (short)1, - (short)1, (short)95, - (short)1, - (short)1,
- (short)1, (short)99, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)105, - (short)1, - (short)1, (short)108, - (short)1, (short)110, (short)111,
- (short)1, - (short)1, - (short)1, - (short)1, (short)116, (short)117, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)82, (short)83,
(short)84, (short)85, (short)86, - (short)1, (short)88, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)95, (short)96, - (short)1,
- (short)1, (short)99, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)105, - (short)1, - (short)1, (short)108, - (short)1, (short)110, (short)111,
- (short)1, - (short)1, - (short)1, - (short)1, (short)116, (short)117, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)82, (short)83,
(short)84, (short)85, (short)86, - (short)1, (short)88, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)95, (short)96, - (short)1,
- (short)1, (short)99, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)105, - (short)1, - (short)1, (short)108, - (short)1, (short)110, (short)111,
- (short)1, - (short)1, - (short)1, - (short)1, (short)116, (short)117, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)82, (short)83,
(short)84, (short)85, (short)86, - (short)1, (short)88, - (short)1, - (short)1,
(short)91, - (short)1, - (short)1, - (short)1, (short)95, - (short)1, - (short)1,
- (short)1, (short)99, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)105, - (short)1, - (short)1, (short)108, - (short)1, (short)110, (short)111,
- (short)1, - (short)1, - (short)1, - (short)1, (short)116, (short)117, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)82, (short)83,
(short)84, (short)85, (short)86, - (short)1, (short)88, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)95, (short)96, - (short)1,
- (short)1, (short)99, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)105, - (short)1, - (short)1, (short)108, - (short)1, (short)110, (short)111,
- (short)1, - (short)1, - (short)1, - (short)1, (short)116, (short)117, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)82, (short)83,
(short)84, (short)85, (short)86, - (short)1, (short)88, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)95, (short)96, - (short)1,
- (short)1, (short)99, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)105, - (short)1, - (short)1, (short)108, - (short)1, (short)110, (short)111,
- (short)1, - (short)1, - (short)1, - (short)1, (short)116, (short)117, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)82, (short)83,
(short)84, (short)85, (short)86, - (short)1, (short)88, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)95, (short)96, - (short)1,
- (short)1, (short)99, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)105, - (short)1, - (short)1, (short)108, - (short)1, (short)110, (short)111,
- (short)1, - (short)1, - (short)1, - (short)1, (short)116, (short)117, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)82, (short)83,
(short)84, (short)85, (short)86, - (short)1, (short)88, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)95, (short)96, - (short)1,
- (short)1, (short)99, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)105, - (short)1, - (short)1, (short)108, - (short)1, (short)110, (short)111,
- (short)1, - (short)1, - (short)1, - (short)1, (short)116, (short)117, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)82, (short)83,
(short)84, (short)85, (short)86, - (short)1, (short)88, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)95, - (short)1, - (short)1,
- (short)1, (short)99, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)105, - (short)1, - (short)1, (short)108, - (short)1, (short)110, (short)111,
- (short)1, - (short)1, - (short)1, - (short)1, (short)116, (short)117, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)82, (short)83,
(short)84, (short)85, (short)86, - (short)1, (short)88, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)95, - (short)1, - (short)1,
- (short)1, (short)99, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)105, - (short)1, - (short)1, (short)108, - (short)1, (short)110, (short)111,
- (short)1, - (short)1, - (short)1, - (short)1, (short)116, (short)117, (short)35,
- (short)1, (short)37, - (short)1, (short)39, - (short)1, - (short)1, (short)42,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)48, (short)49,
- (short)1, - (short)1, (short)52, (short)53, (short)54, (short)55, (short)56,
(short)57, - (short)1, (short)59, (short)60, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, -
(short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, - (short)1, (short)82, (short)83,
(short)84, (short)85, (short)86, - (short)1, (short)88, - (short)1, - (short)1,
- (short)1, - (short)1, - (short)1, - (short)1, (short)95, - (short)1, - (short)1,
- (short)1, (short)99, - (short)1, - (short)1, - (short)1, - (short)1, - (short)1,
(short)105, - (short)1, - (short)1, (short)108, - (short)1, (short)110, (short)111,
- (short)1, - (short)1, - (short)1, - (short)1, (short)116, (short)117}; char
Cyc_Yyimpossible_tag[ 13u]="Yyimpossible"; struct Cyc_Yyimpossible_struct{ char*
tag; } ; char Cyc_Yystack_overflow_tag[ 17u]="Yystack_overflow"; struct Cyc_Yystack_overflow_struct{
char* tag; } ; extern void Cyc_yyerror( struct _tagged_string); extern int Cyc_yylex();
char Cyc_YYINITIALSVAL_tag[ 14u]="YYINITIALSVAL"; struct Cyc_YYINITIALSVAL_struct{
char* tag; } ; static int Cyc_yychar=( int)'\000'; static struct Cyc_YYINITIALSVAL_struct
_temp1900={ Cyc_YYINITIALSVAL_tag}; struct _xenum_struct* Cyc_yylval=( struct
_xenum_struct*)& _temp1900; static int Cyc_yynerrs= 0; int Cyc_yyparse(){ int
yystate; int yyn= 0; int yyerrstatus; int yychar1= 0; int yyssp_offset; struct
_tagged_ptr1 yyss=({ unsigned int _temp1901=( unsigned int) 200; short*
_temp1902=( short*) GC_malloc_atomic( sizeof( short) * _temp1901); unsigned int
i; struct _tagged_ptr1 _temp1903={ _temp1902, _temp1902, _temp1902 + _temp1901};
for( i= 0; i < _temp1901; i ++){ _temp1902[ i]= (short)0;} _temp1903;}); int
yyvsp_offset; struct _tagged_ptr2 yyvs=({ unsigned int _temp1904=( unsigned int)
200; struct _xenum_struct** _temp1905=( struct _xenum_struct**) GC_malloc(
sizeof( struct _xenum_struct*) * _temp1904); unsigned int i; struct _tagged_ptr2
_temp1906={ _temp1905, _temp1905, _temp1905 + _temp1904}; for( i= 0; i <
_temp1904; i ++){ _temp1905[ i]= Cyc_yylval;} _temp1906;}); int yylsp_offset;
struct _tagged_ptr3 yyls=({ unsigned int _temp1907=( unsigned int) 200; struct
Cyc_Yyltype* _temp1908=( struct Cyc_Yyltype*) GC_malloc( sizeof( struct Cyc_Yyltype)
* _temp1907); unsigned int i; struct _tagged_ptr3 _temp1909={ _temp1908,
_temp1908, _temp1908 + _temp1907}; for( i= 0; i < _temp1907; i ++){ _temp1908[ i]=
Cyc_yynewloc();} _temp1909;}); int yystacksize= 200; struct _xenum_struct* yyval=
Cyc_yylval; int yylen; yystate= 0; yyerrstatus= 0; Cyc_yynerrs= 0; Cyc_yychar= -
2; yyssp_offset= - 1; yyvsp_offset= 0; yylsp_offset= 0; yynewstate:({ struct
_tagged_ptr1 _temp1910= yyss; short* _temp1912= _temp1910.curr +( ++
yyssp_offset); if( _temp1912 < _temp1910.base? 1: _temp1912 >= _temp1910.last_plus_one){
_throw( Null_Exception);}* _temp1912=( short) yystate;}); if( yyssp_offset >=
yystacksize - 1){ if( yystacksize >= 10000){ Cyc_yyerror(( struct _tagged_string)({
char* _temp1913=( char*)"parser stack overflow"; struct _tagged_string _temp1914;
_temp1914.curr= _temp1913; _temp1914.base= _temp1913; _temp1914.last_plus_one=
_temp1913 + 22; _temp1914;}));( void) _throw(({ struct Cyc_Yystack_overflow_struct*
_temp1915=( struct Cyc_Yystack_overflow_struct*) GC_malloc_atomic( sizeof(
struct Cyc_Yystack_overflow_struct));* _temp1915=( struct Cyc_Yystack_overflow_struct){.tag=
Cyc_Yystack_overflow_tag};( struct _xenum_struct*) _temp1915;}));} yystacksize *=
2; if( yystacksize > 10000){ yystacksize= 10000;}{ struct _tagged_ptr1 yyss1=({
unsigned int _temp1916=( unsigned int) yystacksize; short* _temp1917=( short*)
GC_malloc_atomic( sizeof( short) * _temp1916); unsigned int i; struct
_tagged_ptr1 _temp1918={ _temp1917, _temp1917, _temp1917 + _temp1916}; for( i= 0;
i < _temp1916; i ++){ _temp1917[ i]= (short)0;} _temp1918;}); struct
_tagged_ptr2 yyvs1=({ unsigned int _temp1919=( unsigned int) yystacksize; struct
_xenum_struct** _temp1920=( struct _xenum_struct**) GC_malloc( sizeof( struct
_xenum_struct*) * _temp1919); unsigned int i; struct _tagged_ptr2 _temp1921={
_temp1920, _temp1920, _temp1920 + _temp1919}; for( i= 0; i < _temp1919; i ++){
_temp1920[ i]= Cyc_yylval;} _temp1921;}); struct _tagged_ptr3 yyls1=({
unsigned int _temp1922=( unsigned int) yystacksize; struct Cyc_Yyltype*
_temp1923=( struct Cyc_Yyltype*) GC_malloc( sizeof( struct Cyc_Yyltype) *
_temp1922); unsigned int i; struct _tagged_ptr3 _temp1924={ _temp1923, _temp1923,
_temp1923 + _temp1922}; for( i= 0; i < _temp1922; i ++){ _temp1923[ i]= Cyc_yynewloc();}
_temp1924;});{ int i= 0; for( 0; i <= yyssp_offset; i ++){({ struct _tagged_ptr1
_temp1925= yyss1; short* _temp1927= _temp1925.curr + i; if( _temp1927 <
_temp1925.base? 1: _temp1927 >= _temp1925.last_plus_one){ _throw( Null_Exception);}*
_temp1927=({ struct _tagged_ptr1 _temp1928= yyss; short* _temp1930= _temp1928.curr
+ i; if( _temp1930 < _temp1928.base? 1: _temp1930 >= _temp1928.last_plus_one){
_throw( Null_Exception);}* _temp1930;});});({ struct _tagged_ptr2 _temp1931=
yyvs1; struct _xenum_struct** _temp1933= _temp1931.curr + i; if( _temp1933 <
_temp1931.base? 1: _temp1933 >= _temp1931.last_plus_one){ _throw( Null_Exception);}*
_temp1933=({ struct _tagged_ptr2 _temp1934= yyvs; struct _xenum_struct**
_temp1936= _temp1934.curr + i; if( _temp1936 < _temp1934.base? 1: _temp1936 >=
_temp1934.last_plus_one){ _throw( Null_Exception);}* _temp1936;});});({ struct
_tagged_ptr3 _temp1937= yyls1; struct Cyc_Yyltype* _temp1939= _temp1937.curr + i;
if( _temp1939 < _temp1937.base? 1: _temp1939 >= _temp1937.last_plus_one){ _throw(
Null_Exception);}* _temp1939=({ struct _tagged_ptr3 _temp1940= yyls; struct Cyc_Yyltype*
_temp1942= _temp1940.curr + i; if( _temp1942 < _temp1940.base? 1: _temp1942 >=
_temp1940.last_plus_one){ _throw( Null_Exception);}* _temp1942;});});}} yyss=
yyss1; yyvs= yyvs1; yyls= yyls1;}} goto yybackup; yybackup: yyn=( int)({ short*
_temp1943=( short*) Cyc_yypact; unsigned int _temp1944= yystate; if( _temp1944
>= 705u){ _throw( Null_Exception);} _temp1943[ _temp1944];}); if( yyn == - 32768){
goto yydefault;} if( Cyc_yychar == - 2){ Cyc_yychar= Cyc_yylex();} if( Cyc_yychar
<= 0){ yychar1= 0; Cyc_yychar= 0;} else{ yychar1=( Cyc_yychar > 0? Cyc_yychar <=
345: 0)?( int)({ short* _temp1945=( short*) Cyc_yytranslate; unsigned int
_temp1946= Cyc_yychar; if( _temp1946 >= 346u){ _throw( Null_Exception);}
_temp1945[ _temp1946];}): 220;} yyn += yychar1; if(( yyn < 0? 1: yyn > 4113)? 1:(
int)({ short* _temp1947=( short*) Cyc_yycheck; unsigned int _temp1948= yyn; if(
_temp1948 >= 4114u){ _throw( Null_Exception);} _temp1947[ _temp1948];}) !=
yychar1){ goto yydefault;} yyn=( int)({ short* _temp1949=( short*) Cyc_yytable;
unsigned int _temp1950= yyn; if( _temp1950 >= 4114u){ _throw( Null_Exception);}
_temp1949[ _temp1950];}); if( yyn < 0){ if( yyn == - 32768){ goto yyerrlab;} yyn=
- yyn; goto yyreduce;} else{ if( yyn == 0){ goto yyerrlab;}} if( yyn == 704){
return 0;} if( Cyc_yychar != 0){ Cyc_yychar= - 2;}({ struct _tagged_ptr2
_temp1951= yyvs; struct _xenum_struct** _temp1953= _temp1951.curr +( ++
yyvsp_offset); if( _temp1953 < _temp1951.base? 1: _temp1953 >= _temp1951.last_plus_one){
_throw( Null_Exception);}* _temp1953= Cyc_yylval;});({ struct _tagged_ptr3
_temp1954= yyls; struct Cyc_Yyltype* _temp1956= _temp1954.curr +( ++
yylsp_offset); if( _temp1956 < _temp1954.base? 1: _temp1956 >= _temp1954.last_plus_one){
_throw( Null_Exception);}* _temp1956= Cyc_yylloc;}); if( yyerrstatus != 0){
yyerrstatus --;} yystate= yyn; goto yynewstate; yydefault: yyn=( int)({ short*
_temp1957=( short*) Cyc_yydefact; unsigned int _temp1958= yystate; if( _temp1958
>= 705u){ _throw( Null_Exception);} _temp1957[ _temp1958];}); if( yyn == 0){
goto yyerrlab;} yyreduce: yylen=( int)({ short* _temp1959=( short*) Cyc_yyr2;
unsigned int _temp1960= yyn; if( _temp1960 >= 379u){ _throw( Null_Exception);}
_temp1959[ _temp1960];}); if( yylen > 0){ yyval=({ struct _tagged_ptr2 _temp1961=
yyvs; struct _xenum_struct** _temp1963= _temp1961.curr +(( yyvsp_offset + 1) -
yylen); if( _temp1963 < _temp1961.base? 1: _temp1963 >= _temp1961.last_plus_one){
_throw( Null_Exception);}* _temp1963;});} switch( yyn){ case 1: _LL1964: yyval=({
struct _tagged_ptr2 _temp1966= yyvs; struct _xenum_struct** _temp1968= _temp1966.curr
+ yyvsp_offset; if( _temp1968 < _temp1966.base? 1: _temp1968 >= _temp1966.last_plus_one){
_throw( Null_Exception);}* _temp1968;}); Cyc_Parse_parse_result= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp1969= yyvs; struct _xenum_struct** _temp1971= _temp1969.curr
+ yyvsp_offset; if( _temp1971 < _temp1969.base? 1: _temp1971 >= _temp1969.last_plus_one){
_throw( Null_Exception);}* _temp1971;})); break; case 2: _LL1965: yyval=({
struct _tagged_ptr2 _temp1973= yyvs; struct _xenum_struct** _temp1975= _temp1973.curr
+ yyvsp_offset; if( _temp1975 < _temp1973.base? 1: _temp1975 >= _temp1973.last_plus_one){
_throw( Null_Exception);}* _temp1975;}); break; case 3: _LL1972: yyval=({ struct
Cyc_DeclList_tok_struct* _temp1977=( struct Cyc_DeclList_tok_struct*) GC_malloc(
sizeof( struct Cyc_DeclList_tok_struct));* _temp1977=( struct Cyc_DeclList_tok_struct){.tag=
Cyc_DeclList_tok_tag,.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp1978= yyvs; struct _xenum_struct** _temp1980= _temp1978.curr +(
yyvsp_offset - 1); if( _temp1980 < _temp1978.base? 1: _temp1980 >= _temp1978.last_plus_one){
_throw( Null_Exception);}* _temp1980;})), Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp1981= yyvs; struct _xenum_struct** _temp1983= _temp1981.curr +
yyvsp_offset; if( _temp1983 < _temp1981.base? 1: _temp1983 >= _temp1981.last_plus_one){
_throw( Null_Exception);}* _temp1983;})))};( struct _xenum_struct*) _temp1977;});
break; case 4: _LL1976: yyval=({ struct Cyc_DeclList_tok_struct* _temp1985=(
struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));*
_temp1985=( struct Cyc_DeclList_tok_struct){.tag= Cyc_DeclList_tok_tag,.f1=({
struct Cyc_List_List* _temp1986=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1986->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1987=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1987->r=( void*)({ struct Cyc_Absyn_Using_d_struct* _temp1994=( struct Cyc_Absyn_Using_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct)); _temp1994->tag= Cyc_Absyn_Using_d_tag;
_temp1994->f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp1995= yyvs;
struct _xenum_struct** _temp1997= _temp1995.curr +( yyvsp_offset - 2); if(
_temp1997 < _temp1995.base? 1: _temp1997 >= _temp1995.last_plus_one){ _throw(
Null_Exception);}* _temp1997;})); _temp1994->f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp1998= yyvs; struct _xenum_struct** _temp2000= _temp1998.curr
+ yyvsp_offset; if( _temp2000 < _temp1998.base? 1: _temp2000 >= _temp1998.last_plus_one){
_throw( Null_Exception);}* _temp2000;}));( void*) _temp1994;}); _temp1987->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp1988= yyls; struct Cyc_Yyltype*
_temp1990= _temp1988.curr +( yylsp_offset - 2); if( _temp1990 < _temp1988.base?
1: _temp1990 >= _temp1988.last_plus_one){ _throw( Null_Exception);}* _temp1990;}).first_line,({
struct _tagged_ptr3 _temp1991= yyls; struct Cyc_Yyltype* _temp1993= _temp1991.curr
+ yylsp_offset; if( _temp1993 < _temp1991.base? 1: _temp1993 >= _temp1991.last_plus_one){
_throw( Null_Exception);}* _temp1993;}).last_line); _temp1987;}); _temp1986->tl=
0; _temp1986;})};( struct _xenum_struct*) _temp1985;}); Cyc_Lex_leave_using();
break; case 5: _LL1984: yyval=({ struct Cyc_DeclList_tok_struct* _temp2002=(
struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));*
_temp2002=( struct Cyc_DeclList_tok_struct){.tag= Cyc_DeclList_tok_tag,.f1=({
struct Cyc_List_List* _temp2003=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2003->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2007=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2007->r=( void*)({ struct Cyc_Absyn_Using_d_struct* _temp2014=( struct Cyc_Absyn_Using_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Using_d_struct)); _temp2014->tag= Cyc_Absyn_Using_d_tag;
_temp2014->f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp2015= yyvs;
struct _xenum_struct** _temp2017= _temp2015.curr +( yyvsp_offset - 4); if(
_temp2017 < _temp2015.base? 1: _temp2017 >= _temp2015.last_plus_one){ _throw(
Null_Exception);}* _temp2017;})); _temp2014->f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2018= yyvs; struct _xenum_struct** _temp2020= _temp2018.curr
+( yyvsp_offset - 2); if( _temp2020 < _temp2018.base? 1: _temp2020 >= _temp2018.last_plus_one){
_throw( Null_Exception);}* _temp2020;}));( void*) _temp2014;}); _temp2007->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2008= yyls; struct Cyc_Yyltype*
_temp2010= _temp2008.curr +( yylsp_offset - 4); if( _temp2010 < _temp2008.base?
1: _temp2010 >= _temp2008.last_plus_one){ _throw( Null_Exception);}* _temp2010;}).first_line,({
struct _tagged_ptr3 _temp2011= yyls; struct Cyc_Yyltype* _temp2013= _temp2011.curr
+( yylsp_offset - 1); if( _temp2013 < _temp2011.base? 1: _temp2013 >= _temp2011.last_plus_one){
_throw( Null_Exception);}* _temp2013;}).last_line); _temp2007;}); _temp2003->tl=
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2004= yyvs; struct
_xenum_struct** _temp2006= _temp2004.curr + yyvsp_offset; if( _temp2006 <
_temp2004.base? 1: _temp2006 >= _temp2004.last_plus_one){ _throw( Null_Exception);}*
_temp2006;})); _temp2003;})};( struct _xenum_struct*) _temp2002;}); break; case
6: _LL2001: yyval=({ struct Cyc_DeclList_tok_struct* _temp2022=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));* _temp2022=( struct Cyc_DeclList_tok_struct){.tag=
Cyc_DeclList_tok_tag,.f1=({ struct Cyc_List_List* _temp2023=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp2023->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2024=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2024->r=( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp2031=( struct
Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp2031->tag= Cyc_Absyn_Namespace_d_tag; _temp2031->f1=({ struct
_tagged_string* _temp2032=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp2032[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp2033= yyvs; struct _xenum_struct** _temp2035= _temp2033.curr +(
yyvsp_offset - 2); if( _temp2035 < _temp2033.base? 1: _temp2035 >= _temp2033.last_plus_one){
_throw( Null_Exception);}* _temp2035;})); _temp2032;}); _temp2031->f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2036= yyvs; struct _xenum_struct** _temp2038= _temp2036.curr
+ yyvsp_offset; if( _temp2038 < _temp2036.base? 1: _temp2038 >= _temp2036.last_plus_one){
_throw( Null_Exception);}* _temp2038;}));( void*) _temp2031;}); _temp2024->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2025= yyls; struct Cyc_Yyltype*
_temp2027= _temp2025.curr +( yylsp_offset - 2); if( _temp2027 < _temp2025.base?
1: _temp2027 >= _temp2025.last_plus_one){ _throw( Null_Exception);}* _temp2027;}).first_line,({
struct _tagged_ptr3 _temp2028= yyls; struct Cyc_Yyltype* _temp2030= _temp2028.curr
+ yylsp_offset; if( _temp2030 < _temp2028.base? 1: _temp2030 >= _temp2028.last_plus_one){
_throw( Null_Exception);}* _temp2030;}).last_line); _temp2024;}); _temp2023->tl=
0; _temp2023;})};( struct _xenum_struct*) _temp2022;}); Cyc_Lex_leave_namespace();
break; case 7: _LL2021: yyval=({ struct Cyc_DeclList_tok_struct* _temp2040=(
struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));*
_temp2040=( struct Cyc_DeclList_tok_struct){.tag= Cyc_DeclList_tok_tag,.f1=({
struct Cyc_List_List* _temp2041=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2041->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2045=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2045->r=( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp2052=( struct
Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp2052->tag= Cyc_Absyn_Namespace_d_tag; _temp2052->f1=({ struct
_tagged_string* _temp2053=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp2053[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp2054= yyvs; struct _xenum_struct** _temp2056= _temp2054.curr +(
yyvsp_offset - 4); if( _temp2056 < _temp2054.base? 1: _temp2056 >= _temp2054.last_plus_one){
_throw( Null_Exception);}* _temp2056;})); _temp2053;}); _temp2052->f2= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2057= yyvs; struct _xenum_struct** _temp2059= _temp2057.curr
+( yyvsp_offset - 2); if( _temp2059 < _temp2057.base? 1: _temp2059 >= _temp2057.last_plus_one){
_throw( Null_Exception);}* _temp2059;}));( void*) _temp2052;}); _temp2045->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2046= yyls; struct Cyc_Yyltype*
_temp2048= _temp2046.curr +( yylsp_offset - 4); if( _temp2048 < _temp2046.base?
1: _temp2048 >= _temp2046.last_plus_one){ _throw( Null_Exception);}* _temp2048;}).first_line,({
struct _tagged_ptr3 _temp2049= yyls; struct Cyc_Yyltype* _temp2051= _temp2049.curr
+( yylsp_offset - 1); if( _temp2051 < _temp2049.base? 1: _temp2051 >= _temp2049.last_plus_one){
_throw( Null_Exception);}* _temp2051;}).last_line); _temp2045;}); _temp2041->tl=
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2042= yyvs; struct
_xenum_struct** _temp2044= _temp2042.curr + yyvsp_offset; if( _temp2044 <
_temp2042.base? 1: _temp2044 >= _temp2042.last_plus_one){ _throw( Null_Exception);}*
_temp2044;})); _temp2041;})};( struct _xenum_struct*) _temp2040;}); break; case
8: _LL2039: if( Cyc_String_strcmp( Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp2061= yyvs; struct _xenum_struct** _temp2063= _temp2061.curr +(
yyvsp_offset - 4); if( _temp2063 < _temp2061.base? 1: _temp2063 >= _temp2061.last_plus_one){
_throw( Null_Exception);}* _temp2063;})),( struct _tagged_string)({ char*
_temp2064=( char*)"C"; struct _tagged_string _temp2065; _temp2065.curr=
_temp2064; _temp2065.base= _temp2064; _temp2065.last_plus_one= _temp2064 + 2;
_temp2065;})) != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp2066=(
char*)"only extern \"C\" { ... } is allowed"; struct _tagged_string _temp2067;
_temp2067.curr= _temp2066; _temp2067.base= _temp2066; _temp2067.last_plus_one=
_temp2066 + 35; _temp2067;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2068= yyls; struct Cyc_Yyltype* _temp2070= _temp2068.curr +( yylsp_offset -
5); if( _temp2070 < _temp2068.base? 1: _temp2070 >= _temp2068.last_plus_one){
_throw( Null_Exception);}* _temp2070;}).first_line,({ struct _tagged_ptr3
_temp2071= yyls; struct Cyc_Yyltype* _temp2073= _temp2071.curr +( yylsp_offset -
4); if( _temp2073 < _temp2071.base? 1: _temp2073 >= _temp2071.last_plus_one){
_throw( Null_Exception);}* _temp2073;}).last_line));} yyval=({ struct Cyc_DeclList_tok_struct*
_temp2074=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));*
_temp2074=( struct Cyc_DeclList_tok_struct){.tag= Cyc_DeclList_tok_tag,.f1=({
struct Cyc_List_List* _temp2075=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2075->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp2079=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp2079->r=( void*)({ struct Cyc_Absyn_ExternC_d_struct* _temp2086=( struct
Cyc_Absyn_ExternC_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ExternC_d_struct));
_temp2086->tag= Cyc_Absyn_ExternC_d_tag; _temp2086->f1= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp2087= yyvs; struct _xenum_struct** _temp2089= _temp2087.curr
+( yyvsp_offset - 2); if( _temp2089 < _temp2087.base? 1: _temp2089 >= _temp2087.last_plus_one){
_throw( Null_Exception);}* _temp2089;}));( void*) _temp2086;}); _temp2079->loc=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2080= yyls; struct Cyc_Yyltype*
_temp2082= _temp2080.curr +( yylsp_offset - 5); if( _temp2082 < _temp2080.base?
1: _temp2082 >= _temp2080.last_plus_one){ _throw( Null_Exception);}* _temp2082;}).first_line,({
struct _tagged_ptr3 _temp2083= yyls; struct Cyc_Yyltype* _temp2085= _temp2083.curr
+( yylsp_offset - 1); if( _temp2085 < _temp2083.base? 1: _temp2085 >= _temp2083.last_plus_one){
_throw( Null_Exception);}* _temp2085;}).last_line); _temp2079;}); _temp2075->tl=
Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2076= yyvs; struct
_xenum_struct** _temp2078= _temp2076.curr + yyvsp_offset; if( _temp2078 <
_temp2076.base? 1: _temp2078 >= _temp2076.last_plus_one){ _throw( Null_Exception);}*
_temp2078;})); _temp2075;})};( struct _xenum_struct*) _temp2074;}); break; case
9: _LL2060: yyval=({ struct Cyc_DeclList_tok_struct* _temp2091=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));* _temp2091=( struct Cyc_DeclList_tok_struct){.tag=
Cyc_DeclList_tok_tag,.f1= 0};( struct _xenum_struct*) _temp2091;}); break; case
10: _LL2090: yyval=({ struct _tagged_ptr2 _temp2093= yyvs; struct _xenum_struct**
_temp2095= _temp2093.curr + yyvsp_offset; if( _temp2095 < _temp2093.base? 1:
_temp2095 >= _temp2093.last_plus_one){ _throw( Null_Exception);}* _temp2095;});
break; case 11: _LL2092: yyval=({ struct Cyc_DeclList_tok_struct* _temp2097=(
struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));*
_temp2097=( struct Cyc_DeclList_tok_struct){.tag= Cyc_DeclList_tok_tag,.f1=({
struct Cyc_List_List* _temp2098=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2098->hd=( void*) Cyc_Absyn_new_decl(({ struct Cyc_Absyn_Fn_d_struct*
_temp2099=( struct Cyc_Absyn_Fn_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct));
_temp2099->tag= Cyc_Absyn_Fn_d_tag; _temp2099->f1= Cyc_yyget_FnDecl_tok(({
struct _tagged_ptr2 _temp2100= yyvs; struct _xenum_struct** _temp2102= _temp2100.curr
+ yyvsp_offset; if( _temp2102 < _temp2100.base? 1: _temp2102 >= _temp2100.last_plus_one){
_throw( Null_Exception);}* _temp2102;}));( void*) _temp2099;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2103= yyls; struct Cyc_Yyltype* _temp2105= _temp2103.curr
+ yylsp_offset; if( _temp2105 < _temp2103.base? 1: _temp2105 >= _temp2103.last_plus_one){
_throw( Null_Exception);}* _temp2105;}).first_line,({ struct _tagged_ptr3
_temp2106= yyls; struct Cyc_Yyltype* _temp2108= _temp2106.curr + yylsp_offset;
if( _temp2108 < _temp2106.base? 1: _temp2108 >= _temp2106.last_plus_one){ _throw(
Null_Exception);}* _temp2108;}).last_line)); _temp2098->tl= 0; _temp2098;})};(
struct _xenum_struct*) _temp2097;}); break; case 12: _LL2096: yyval=({ struct
_tagged_ptr2 _temp2110= yyvs; struct _xenum_struct** _temp2112= _temp2110.curr +
yyvsp_offset; if( _temp2112 < _temp2110.base? 1: _temp2112 >= _temp2110.last_plus_one){
_throw( Null_Exception);}* _temp2112;}); break; case 13: _LL2109: yyval=({
struct Cyc_FnDecl_tok_struct* _temp2114=( struct Cyc_FnDecl_tok_struct*)
GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));* _temp2114=( struct Cyc_FnDecl_tok_struct){.tag=
Cyc_FnDecl_tok_tag,.f1= Cyc_Parse_make_function( 0, Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2115= yyvs; struct _xenum_struct** _temp2117= _temp2115.curr
+( yyvsp_offset - 1); if( _temp2117 < _temp2115.base? 1: _temp2117 >= _temp2115.last_plus_one){
_throw( Null_Exception);}* _temp2117;})), 0, Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp2118= yyvs; struct _xenum_struct** _temp2120= _temp2118.curr +
yyvsp_offset; if( _temp2120 < _temp2118.base? 1: _temp2120 >= _temp2118.last_plus_one){
_throw( Null_Exception);}* _temp2120;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2121= yyls; struct Cyc_Yyltype* _temp2123= _temp2121.curr +(
yylsp_offset - 1); if( _temp2123 < _temp2121.base? 1: _temp2123 >= _temp2121.last_plus_one){
_throw( Null_Exception);}* _temp2123;}).first_line,({ struct _tagged_ptr3
_temp2124= yyls; struct Cyc_Yyltype* _temp2126= _temp2124.curr + yylsp_offset;
if( _temp2126 < _temp2124.base? 1: _temp2126 >= _temp2124.last_plus_one){ _throw(
Null_Exception);}* _temp2126;}).last_line))};( struct _xenum_struct*) _temp2114;});
break; case 14: _LL2113: yyval=({ struct Cyc_FnDecl_tok_struct* _temp2128=(
struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));*
_temp2128=( struct Cyc_FnDecl_tok_struct){.tag= Cyc_FnDecl_tok_tag,.f1= Cyc_Parse_make_function(({
struct Cyc_Core_Opt* _temp2129=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2129->v=( void*) Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2130= yyvs; struct _xenum_struct** _temp2132= _temp2130.curr +(
yyvsp_offset - 2); if( _temp2132 < _temp2130.base? 1: _temp2132 >= _temp2130.last_plus_one){
_throw( Null_Exception);}* _temp2132;})); _temp2129;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2133= yyvs; struct _xenum_struct** _temp2135= _temp2133.curr
+( yyvsp_offset - 1); if( _temp2135 < _temp2133.base? 1: _temp2135 >= _temp2133.last_plus_one){
_throw( Null_Exception);}* _temp2135;})), 0, Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp2136= yyvs; struct _xenum_struct** _temp2138= _temp2136.curr +
yyvsp_offset; if( _temp2138 < _temp2136.base? 1: _temp2138 >= _temp2136.last_plus_one){
_throw( Null_Exception);}* _temp2138;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2139= yyls; struct Cyc_Yyltype* _temp2141= _temp2139.curr +(
yylsp_offset - 2); if( _temp2141 < _temp2139.base? 1: _temp2141 >= _temp2139.last_plus_one){
_throw( Null_Exception);}* _temp2141;}).first_line,({ struct _tagged_ptr3
_temp2142= yyls; struct Cyc_Yyltype* _temp2144= _temp2142.curr + yylsp_offset;
if( _temp2144 < _temp2142.base? 1: _temp2144 >= _temp2142.last_plus_one){ _throw(
Null_Exception);}* _temp2144;}).last_line))};( struct _xenum_struct*) _temp2128;});
break; case 15: _LL2127: yyval=({ struct Cyc_FnDecl_tok_struct* _temp2146=(
struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));*
_temp2146=( struct Cyc_FnDecl_tok_struct){.tag= Cyc_FnDecl_tok_tag,.f1= Cyc_Parse_make_function(
0, Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp2147= yyvs; struct
_xenum_struct** _temp2149= _temp2147.curr +( yyvsp_offset - 2); if( _temp2149 <
_temp2147.base? 1: _temp2149 >= _temp2147.last_plus_one){ _throw( Null_Exception);}*
_temp2149;})), Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp2150= yyvs;
struct _xenum_struct** _temp2152= _temp2150.curr +( yyvsp_offset - 1); if(
_temp2152 < _temp2150.base? 1: _temp2152 >= _temp2150.last_plus_one){ _throw(
Null_Exception);}* _temp2152;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp2153= yyvs; struct _xenum_struct** _temp2155= _temp2153.curr + yyvsp_offset;
if( _temp2155 < _temp2153.base? 1: _temp2155 >= _temp2153.last_plus_one){ _throw(
Null_Exception);}* _temp2155;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2156= yyls; struct Cyc_Yyltype* _temp2158= _temp2156.curr +(
yylsp_offset - 2); if( _temp2158 < _temp2156.base? 1: _temp2158 >= _temp2156.last_plus_one){
_throw( Null_Exception);}* _temp2158;}).first_line,({ struct _tagged_ptr3
_temp2159= yyls; struct Cyc_Yyltype* _temp2161= _temp2159.curr + yylsp_offset;
if( _temp2161 < _temp2159.base? 1: _temp2161 >= _temp2159.last_plus_one){ _throw(
Null_Exception);}* _temp2161;}).last_line))};( struct _xenum_struct*) _temp2146;});
break; case 16: _LL2145: yyval=({ struct Cyc_FnDecl_tok_struct* _temp2163=(
struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));*
_temp2163=( struct Cyc_FnDecl_tok_struct){.tag= Cyc_FnDecl_tok_tag,.f1= Cyc_Parse_make_function(({
struct Cyc_Core_Opt* _temp2164=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2164->v=( void*) Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2165= yyvs; struct _xenum_struct** _temp2167= _temp2165.curr +(
yyvsp_offset - 3); if( _temp2167 < _temp2165.base? 1: _temp2167 >= _temp2165.last_plus_one){
_throw( Null_Exception);}* _temp2167;})); _temp2164;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2168= yyvs; struct _xenum_struct** _temp2170= _temp2168.curr
+( yyvsp_offset - 2); if( _temp2170 < _temp2168.base? 1: _temp2170 >= _temp2168.last_plus_one){
_throw( Null_Exception);}* _temp2170;})), Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp2171= yyvs; struct _xenum_struct** _temp2173= _temp2171.curr +(
yyvsp_offset - 1); if( _temp2173 < _temp2171.base? 1: _temp2173 >= _temp2171.last_plus_one){
_throw( Null_Exception);}* _temp2173;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp2174= yyvs; struct _xenum_struct** _temp2176= _temp2174.curr +
yyvsp_offset; if( _temp2176 < _temp2174.base? 1: _temp2176 >= _temp2174.last_plus_one){
_throw( Null_Exception);}* _temp2176;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2177= yyls; struct Cyc_Yyltype* _temp2179= _temp2177.curr +(
yylsp_offset - 3); if( _temp2179 < _temp2177.base? 1: _temp2179 >= _temp2177.last_plus_one){
_throw( Null_Exception);}* _temp2179;}).first_line,({ struct _tagged_ptr3
_temp2180= yyls; struct Cyc_Yyltype* _temp2182= _temp2180.curr + yylsp_offset;
if( _temp2182 < _temp2180.base? 1: _temp2182 >= _temp2180.last_plus_one){ _throw(
Null_Exception);}* _temp2182;}).last_line))};( struct _xenum_struct*) _temp2163;});
break; case 17: _LL2162: yyval=({ struct Cyc_FnDecl_tok_struct* _temp2184=(
struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));*
_temp2184=( struct Cyc_FnDecl_tok_struct){.tag= Cyc_FnDecl_tok_tag,.f1= Cyc_Parse_make_function(({
struct Cyc_Core_Opt* _temp2185=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2185->v=( void*) Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2186= yyvs; struct _xenum_struct** _temp2188= _temp2186.curr +(
yyvsp_offset - 2); if( _temp2188 < _temp2186.base? 1: _temp2188 >= _temp2186.last_plus_one){
_throw( Null_Exception);}* _temp2188;})); _temp2185;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2189= yyvs; struct _xenum_struct** _temp2191= _temp2189.curr
+( yyvsp_offset - 1); if( _temp2191 < _temp2189.base? 1: _temp2191 >= _temp2189.last_plus_one){
_throw( Null_Exception);}* _temp2191;})), 0, Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp2192= yyvs; struct _xenum_struct** _temp2194= _temp2192.curr +
yyvsp_offset; if( _temp2194 < _temp2192.base? 1: _temp2194 >= _temp2192.last_plus_one){
_throw( Null_Exception);}* _temp2194;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2195= yyls; struct Cyc_Yyltype* _temp2197= _temp2195.curr +(
yylsp_offset - 2); if( _temp2197 < _temp2195.base? 1: _temp2197 >= _temp2195.last_plus_one){
_throw( Null_Exception);}* _temp2197;}).first_line,({ struct _tagged_ptr3
_temp2198= yyls; struct Cyc_Yyltype* _temp2200= _temp2198.curr + yylsp_offset;
if( _temp2200 < _temp2198.base? 1: _temp2200 >= _temp2198.last_plus_one){ _throw(
Null_Exception);}* _temp2200;}).last_line))};( struct _xenum_struct*) _temp2184;});
break; case 18: _LL2183: yyval=({ struct Cyc_FnDecl_tok_struct* _temp2202=(
struct Cyc_FnDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_FnDecl_tok_struct));*
_temp2202=( struct Cyc_FnDecl_tok_struct){.tag= Cyc_FnDecl_tok_tag,.f1= Cyc_Parse_make_function(({
struct Cyc_Core_Opt* _temp2203=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2203->v=( void*) Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2204= yyvs; struct _xenum_struct** _temp2206= _temp2204.curr +(
yyvsp_offset - 3); if( _temp2206 < _temp2204.base? 1: _temp2206 >= _temp2204.last_plus_one){
_throw( Null_Exception);}* _temp2206;})); _temp2203;}), Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp2207= yyvs; struct _xenum_struct** _temp2209= _temp2207.curr
+( yyvsp_offset - 2); if( _temp2209 < _temp2207.base? 1: _temp2209 >= _temp2207.last_plus_one){
_throw( Null_Exception);}* _temp2209;})), Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp2210= yyvs; struct _xenum_struct** _temp2212= _temp2210.curr +(
yyvsp_offset - 1); if( _temp2212 < _temp2210.base? 1: _temp2212 >= _temp2210.last_plus_one){
_throw( Null_Exception);}* _temp2212;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp2213= yyvs; struct _xenum_struct** _temp2215= _temp2213.curr +
yyvsp_offset; if( _temp2215 < _temp2213.base? 1: _temp2215 >= _temp2213.last_plus_one){
_throw( Null_Exception);}* _temp2215;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2216= yyls; struct Cyc_Yyltype* _temp2218= _temp2216.curr +(
yylsp_offset - 3); if( _temp2218 < _temp2216.base? 1: _temp2218 >= _temp2216.last_plus_one){
_throw( Null_Exception);}* _temp2218;}).first_line,({ struct _tagged_ptr3
_temp2219= yyls; struct Cyc_Yyltype* _temp2221= _temp2219.curr + yylsp_offset;
if( _temp2221 < _temp2219.base? 1: _temp2221 >= _temp2219.last_plus_one){ _throw(
Null_Exception);}* _temp2221;}).last_line))};( struct _xenum_struct*) _temp2202;});
break; case 19: _LL2201: Cyc_Lex_enter_using( Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp2223= yyvs; struct _xenum_struct** _temp2225= _temp2223.curr +
yyvsp_offset; if( _temp2225 < _temp2223.base? 1: _temp2225 >= _temp2223.last_plus_one){
_throw( Null_Exception);}* _temp2225;}))); yyval=({ struct _tagged_ptr2
_temp2226= yyvs; struct _xenum_struct** _temp2228= _temp2226.curr + yyvsp_offset;
if( _temp2228 < _temp2226.base? 1: _temp2228 >= _temp2226.last_plus_one){ _throw(
Null_Exception);}* _temp2228;}); break; case 20: _LL2222: Cyc_Lex_leave_using();
break; case 21: _LL2229: Cyc_Lex_enter_namespace(({ struct _tagged_string*
_temp2231=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp2231[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2232= yyvs;
struct _xenum_struct** _temp2234= _temp2232.curr + yyvsp_offset; if( _temp2234 <
_temp2232.base? 1: _temp2234 >= _temp2232.last_plus_one){ _throw( Null_Exception);}*
_temp2234;})); _temp2231;})); yyval=({ struct _tagged_ptr2 _temp2235= yyvs;
struct _xenum_struct** _temp2237= _temp2235.curr + yyvsp_offset; if( _temp2237 <
_temp2235.base? 1: _temp2237 >= _temp2235.last_plus_one){ _throw( Null_Exception);}*
_temp2237;}); break; case 22: _LL2230: Cyc_Lex_leave_namespace(); break; case 23:
_LL2238: yyval=({ struct Cyc_DeclList_tok_struct* _temp2240=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));* _temp2240=( struct Cyc_DeclList_tok_struct){.tag=
Cyc_DeclList_tok_tag,.f1= Cyc_Parse_make_declarations( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2241= yyvs; struct _xenum_struct** _temp2243= _temp2241.curr
+( yyvsp_offset - 1); if( _temp2243 < _temp2241.base? 1: _temp2243 >= _temp2241.last_plus_one){
_throw( Null_Exception);}* _temp2243;})), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2244= yyls; struct Cyc_Yyltype* _temp2246= _temp2244.curr
+( yylsp_offset - 1); if( _temp2246 < _temp2244.base? 1: _temp2246 >= _temp2244.last_plus_one){
_throw( Null_Exception);}* _temp2246;}).first_line,({ struct _tagged_ptr3
_temp2247= yyls; struct Cyc_Yyltype* _temp2249= _temp2247.curr +( yylsp_offset -
1); if( _temp2249 < _temp2247.base? 1: _temp2249 >= _temp2247.last_plus_one){
_throw( Null_Exception);}* _temp2249;}).last_line))};( struct _xenum_struct*)
_temp2240;}); break; case 24: _LL2239: yyval=({ struct Cyc_DeclList_tok_struct*
_temp2251=( struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));*
_temp2251=( struct Cyc_DeclList_tok_struct){.tag= Cyc_DeclList_tok_tag,.f1= Cyc_Parse_make_declarations(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2252= yyvs; struct
_xenum_struct** _temp2254= _temp2252.curr +( yyvsp_offset - 2); if( _temp2254 <
_temp2252.base? 1: _temp2254 >= _temp2252.last_plus_one){ _throw( Null_Exception);}*
_temp2254;})), Cyc_yyget_InitDeclList_tok(({ struct _tagged_ptr2 _temp2255= yyvs;
struct _xenum_struct** _temp2257= _temp2255.curr +( yyvsp_offset - 1); if(
_temp2257 < _temp2255.base? 1: _temp2257 >= _temp2255.last_plus_one){ _throw(
Null_Exception);}* _temp2257;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2258= yyls; struct Cyc_Yyltype* _temp2260= _temp2258.curr +(
yylsp_offset - 2); if( _temp2260 < _temp2258.base? 1: _temp2260 >= _temp2258.last_plus_one){
_throw( Null_Exception);}* _temp2260;}).first_line,({ struct _tagged_ptr3
_temp2261= yyls; struct Cyc_Yyltype* _temp2263= _temp2261.curr + yylsp_offset;
if( _temp2263 < _temp2261.base? 1: _temp2263 >= _temp2261.last_plus_one){ _throw(
Null_Exception);}* _temp2263;}).last_line))};( struct _xenum_struct*) _temp2251;});
break; case 25: _LL2250: yyval=({ struct Cyc_DeclList_tok_struct* _temp2265=(
struct Cyc_DeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));*
_temp2265=( struct Cyc_DeclList_tok_struct){.tag= Cyc_DeclList_tok_tag,.f1=({
struct Cyc_List_List* _temp2266=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2266->hd=( void*) Cyc_Absyn_let_decl( Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp2267= yyvs; struct _xenum_struct** _temp2269= _temp2267.curr
+( yyvsp_offset - 3); if( _temp2269 < _temp2267.base? 1: _temp2269 >= _temp2267.last_plus_one){
_throw( Null_Exception);}* _temp2269;})), 0, Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp2270= yyvs; struct _xenum_struct** _temp2272= _temp2270.curr +(
yyvsp_offset - 1); if( _temp2272 < _temp2270.base? 1: _temp2272 >= _temp2270.last_plus_one){
_throw( Null_Exception);}* _temp2272;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2273= yyls; struct Cyc_Yyltype* _temp2275= _temp2273.curr +(
yylsp_offset - 4); if( _temp2275 < _temp2273.base? 1: _temp2275 >= _temp2273.last_plus_one){
_throw( Null_Exception);}* _temp2275;}).first_line,({ struct _tagged_ptr3
_temp2276= yyls; struct Cyc_Yyltype* _temp2278= _temp2276.curr + yylsp_offset;
if( _temp2278 < _temp2276.base? 1: _temp2278 >= _temp2276.last_plus_one){ _throw(
Null_Exception);}* _temp2278;}).last_line)); _temp2266->tl= 0; _temp2266;})};(
struct _xenum_struct*) _temp2265;}); break; case 26: _LL2264: yyval=({ struct
_tagged_ptr2 _temp2280= yyvs; struct _xenum_struct** _temp2282= _temp2280.curr +
yyvsp_offset; if( _temp2282 < _temp2280.base? 1: _temp2282 >= _temp2280.last_plus_one){
_throw( Null_Exception);}* _temp2282;}); break; case 27: _LL2279: yyval=({
struct Cyc_DeclList_tok_struct* _temp2284=( struct Cyc_DeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclList_tok_struct));* _temp2284=( struct Cyc_DeclList_tok_struct){.tag=
Cyc_DeclList_tok_tag,.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp2285= yyvs; struct _xenum_struct** _temp2287= _temp2285.curr +(
yyvsp_offset - 1); if( _temp2287 < _temp2285.base? 1: _temp2287 >= _temp2285.last_plus_one){
_throw( Null_Exception);}* _temp2287;})), Cyc_yyget_DeclList_tok(({ struct
_tagged_ptr2 _temp2288= yyvs; struct _xenum_struct** _temp2290= _temp2288.curr +
yyvsp_offset; if( _temp2290 < _temp2288.base? 1: _temp2290 >= _temp2288.last_plus_one){
_throw( Null_Exception);}* _temp2290;})))};( struct _xenum_struct*) _temp2284;});
break; case 28: _LL2283: yyval=({ struct Cyc_DeclSpec_tok_struct* _temp2292=(
struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));*
_temp2292=( struct Cyc_DeclSpec_tok_struct){.tag= Cyc_DeclSpec_tok_tag,.f1=({
struct Cyc_Parse_Declaration_spec* _temp2293=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2293->sc=({ struct
Cyc_Core_Opt* _temp2297=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2297->v=( void*) Cyc_yyget_StorageClass_tok(({ struct _tagged_ptr2
_temp2298= yyvs; struct _xenum_struct** _temp2300= _temp2298.curr +(
yyvsp_offset - 1); if( _temp2300 < _temp2298.base? 1: _temp2300 >= _temp2298.last_plus_one){
_throw( Null_Exception);}* _temp2300;})); _temp2297;}); _temp2293->tq= Cyc_Absyn_empty_tqual();
_temp2293->type_specs= 0; _temp2293->is_inline= 0; _temp2293->attributes= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2294= yyvs; struct _xenum_struct** _temp2296= _temp2294.curr
+ yyvsp_offset; if( _temp2296 < _temp2294.base? 1: _temp2296 >= _temp2294.last_plus_one){
_throw( Null_Exception);}* _temp2296;})); _temp2293;})};( struct _xenum_struct*)
_temp2292;}); break; case 29: _LL2291: if(( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2302= yyvs; struct _xenum_struct** _temp2304= _temp2302.curr +
yyvsp_offset; if( _temp2304 < _temp2302.base? 1: _temp2304 >= _temp2302.last_plus_one){
_throw( Null_Exception);}* _temp2304;})))->sc != 0){ Cyc_Parse_warn(( struct
_tagged_string)({ char* _temp2305=( char*)"Only one storage class is allowed in a declaration";
struct _tagged_string _temp2306; _temp2306.curr= _temp2305; _temp2306.base=
_temp2305; _temp2306.last_plus_one= _temp2305 + 51; _temp2306;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2307= yyls; struct Cyc_Yyltype* _temp2309= _temp2307.curr
+( yylsp_offset - 2); if( _temp2309 < _temp2307.base? 1: _temp2309 >= _temp2307.last_plus_one){
_throw( Null_Exception);}* _temp2309;}).first_line,({ struct _tagged_ptr3
_temp2310= yyls; struct Cyc_Yyltype* _temp2312= _temp2310.curr +( yylsp_offset -
1); if( _temp2312 < _temp2310.base? 1: _temp2312 >= _temp2310.last_plus_one){
_throw( Null_Exception);}* _temp2312;}).last_line));} yyval=({ struct Cyc_DeclSpec_tok_struct*
_temp2313=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));*
_temp2313=( struct Cyc_DeclSpec_tok_struct){.tag= Cyc_DeclSpec_tok_tag,.f1=({
struct Cyc_Parse_Declaration_spec* _temp2314=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2314->sc=({ struct
Cyc_Core_Opt* _temp2330=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2330->v=( void*) Cyc_yyget_StorageClass_tok(({ struct _tagged_ptr2
_temp2331= yyvs; struct _xenum_struct** _temp2333= _temp2331.curr +(
yyvsp_offset - 2); if( _temp2333 < _temp2331.base? 1: _temp2333 >= _temp2331.last_plus_one){
_throw( Null_Exception);}* _temp2333;})); _temp2330;}); _temp2314->tq=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2327= yyvs; struct _xenum_struct** _temp2329= _temp2327.curr
+ yyvsp_offset; if( _temp2329 < _temp2327.base? 1: _temp2329 >= _temp2327.last_plus_one){
_throw( Null_Exception);}* _temp2329;})))->tq; _temp2314->type_specs=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2324= yyvs; struct _xenum_struct** _temp2326= _temp2324.curr
+ yyvsp_offset; if( _temp2326 < _temp2324.base? 1: _temp2326 >= _temp2324.last_plus_one){
_throw( Null_Exception);}* _temp2326;})))->type_specs; _temp2314->is_inline=(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2321= yyvs; struct
_xenum_struct** _temp2323= _temp2321.curr + yyvsp_offset; if( _temp2323 <
_temp2321.base? 1: _temp2323 >= _temp2321.last_plus_one){ _throw( Null_Exception);}*
_temp2323;})))->is_inline; _temp2314->attributes=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2315= yyvs; struct _xenum_struct** _temp2317= _temp2315.curr
+( yyvsp_offset - 1); if( _temp2317 < _temp2315.base? 1: _temp2317 >= _temp2315.last_plus_one){
_throw( Null_Exception);}* _temp2317;})),( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2318= yyvs; struct _xenum_struct** _temp2320= _temp2318.curr +
yyvsp_offset; if( _temp2320 < _temp2318.base? 1: _temp2320 >= _temp2318.last_plus_one){
_throw( Null_Exception);}* _temp2320;})))->attributes); _temp2314;})};( struct
_xenum_struct*) _temp2313;}); break; case 30: _LL2301: yyval=({ struct Cyc_DeclSpec_tok_struct*
_temp2335=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));*
_temp2335=( struct Cyc_DeclSpec_tok_struct){.tag= Cyc_DeclSpec_tok_tag,.f1=({
struct Cyc_Parse_Declaration_spec* _temp2336=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2336->sc= 0;
_temp2336->tq= Cyc_Absyn_empty_tqual(); _temp2336->type_specs=({ struct Cyc_List_List*
_temp2340=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2340->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({ struct _tagged_ptr2
_temp2341= yyvs; struct _xenum_struct** _temp2343= _temp2341.curr +(
yyvsp_offset - 1); if( _temp2343 < _temp2341.base? 1: _temp2343 >= _temp2341.last_plus_one){
_throw( Null_Exception);}* _temp2343;})); _temp2340->tl= 0; _temp2340;});
_temp2336->is_inline= 0; _temp2336->attributes= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2337= yyvs; struct _xenum_struct** _temp2339= _temp2337.curr
+ yyvsp_offset; if( _temp2339 < _temp2337.base? 1: _temp2339 >= _temp2337.last_plus_one){
_throw( Null_Exception);}* _temp2339;})); _temp2336;})};( struct _xenum_struct*)
_temp2335;}); break; case 31: _LL2334: yyval=({ struct Cyc_DeclSpec_tok_struct*
_temp2345=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));*
_temp2345=( struct Cyc_DeclSpec_tok_struct){.tag= Cyc_DeclSpec_tok_tag,.f1=({
struct Cyc_Parse_Declaration_spec* _temp2346=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2346->sc=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2366= yyvs; struct _xenum_struct** _temp2368= _temp2366.curr
+ yyvsp_offset; if( _temp2368 < _temp2366.base? 1: _temp2368 >= _temp2366.last_plus_one){
_throw( Null_Exception);}* _temp2368;})))->sc; _temp2346->tq=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2363= yyvs; struct _xenum_struct** _temp2365= _temp2363.curr
+ yyvsp_offset; if( _temp2365 < _temp2363.base? 1: _temp2365 >= _temp2363.last_plus_one){
_throw( Null_Exception);}* _temp2365;})))->tq; _temp2346->type_specs=({ struct
Cyc_List_List* _temp2356=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2356->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({ struct _tagged_ptr2
_temp2360= yyvs; struct _xenum_struct** _temp2362= _temp2360.curr +(
yyvsp_offset - 2); if( _temp2362 < _temp2360.base? 1: _temp2362 >= _temp2360.last_plus_one){
_throw( Null_Exception);}* _temp2362;})); _temp2356->tl=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2357= yyvs; struct _xenum_struct** _temp2359= _temp2357.curr
+ yyvsp_offset; if( _temp2359 < _temp2357.base? 1: _temp2359 >= _temp2357.last_plus_one){
_throw( Null_Exception);}* _temp2359;})))->type_specs; _temp2356;}); _temp2346->is_inline=(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2353= yyvs; struct
_xenum_struct** _temp2355= _temp2353.curr + yyvsp_offset; if( _temp2355 <
_temp2353.base? 1: _temp2355 >= _temp2353.last_plus_one){ _throw( Null_Exception);}*
_temp2355;})))->is_inline; _temp2346->attributes=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2347= yyvs; struct _xenum_struct** _temp2349= _temp2347.curr
+( yyvsp_offset - 1); if( _temp2349 < _temp2347.base? 1: _temp2349 >= _temp2347.last_plus_one){
_throw( Null_Exception);}* _temp2349;})),( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2350= yyvs; struct _xenum_struct** _temp2352= _temp2350.curr +
yyvsp_offset; if( _temp2352 < _temp2350.base? 1: _temp2352 >= _temp2350.last_plus_one){
_throw( Null_Exception);}* _temp2352;})))->attributes); _temp2346;})};( struct
_xenum_struct*) _temp2345;}); break; case 32: _LL2344: yyval=({ struct Cyc_DeclSpec_tok_struct*
_temp2370=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));*
_temp2370=( struct Cyc_DeclSpec_tok_struct){.tag= Cyc_DeclSpec_tok_tag,.f1=({
struct Cyc_Parse_Declaration_spec* _temp2371=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2371->sc= 0;
_temp2371->tq= Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2 _temp2375= yyvs;
struct _xenum_struct** _temp2377= _temp2375.curr +( yyvsp_offset - 1); if(
_temp2377 < _temp2375.base? 1: _temp2377 >= _temp2375.last_plus_one){ _throw(
Null_Exception);}* _temp2377;})); _temp2371->type_specs= 0; _temp2371->is_inline=
0; _temp2371->attributes= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2
_temp2372= yyvs; struct _xenum_struct** _temp2374= _temp2372.curr + yyvsp_offset;
if( _temp2374 < _temp2372.base? 1: _temp2374 >= _temp2372.last_plus_one){ _throw(
Null_Exception);}* _temp2374;})); _temp2371;})};( struct _xenum_struct*)
_temp2370;}); break; case 33: _LL2369: yyval=({ struct Cyc_DeclSpec_tok_struct*
_temp2379=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));*
_temp2379=( struct Cyc_DeclSpec_tok_struct){.tag= Cyc_DeclSpec_tok_tag,.f1=({
struct Cyc_Parse_Declaration_spec* _temp2380=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2380->sc=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2399= yyvs; struct _xenum_struct** _temp2401= _temp2399.curr
+ yyvsp_offset; if( _temp2401 < _temp2399.base? 1: _temp2401 >= _temp2399.last_plus_one){
_throw( Null_Exception);}* _temp2401;})))->sc; _temp2380->tq= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2 _temp2393= yyvs; struct
_xenum_struct** _temp2395= _temp2393.curr +( yyvsp_offset - 2); if( _temp2395 <
_temp2393.base? 1: _temp2395 >= _temp2393.last_plus_one){ _throw( Null_Exception);}*
_temp2395;})),( Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2396= yyvs;
struct _xenum_struct** _temp2398= _temp2396.curr + yyvsp_offset; if( _temp2398 <
_temp2396.base? 1: _temp2398 >= _temp2396.last_plus_one){ _throw( Null_Exception);}*
_temp2398;})))->tq); _temp2380->type_specs=( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2390= yyvs; struct _xenum_struct** _temp2392= _temp2390.curr +
yyvsp_offset; if( _temp2392 < _temp2390.base? 1: _temp2392 >= _temp2390.last_plus_one){
_throw( Null_Exception);}* _temp2392;})))->type_specs; _temp2380->is_inline=(
Cyc_yyget_DeclSpec_tok(({ struct _tagged_ptr2 _temp2387= yyvs; struct
_xenum_struct** _temp2389= _temp2387.curr + yyvsp_offset; if( _temp2389 <
_temp2387.base? 1: _temp2389 >= _temp2387.last_plus_one){ _throw( Null_Exception);}*
_temp2389;})))->is_inline; _temp2380->attributes=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2381= yyvs; struct _xenum_struct** _temp2383= _temp2381.curr
+( yyvsp_offset - 1); if( _temp2383 < _temp2381.base? 1: _temp2383 >= _temp2381.last_plus_one){
_throw( Null_Exception);}* _temp2383;})),( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2384= yyvs; struct _xenum_struct** _temp2386= _temp2384.curr +
yyvsp_offset; if( _temp2386 < _temp2384.base? 1: _temp2386 >= _temp2384.last_plus_one){
_throw( Null_Exception);}* _temp2386;})))->attributes); _temp2380;})};( struct
_xenum_struct*) _temp2379;}); break; case 34: _LL2378: yyval=({ struct Cyc_DeclSpec_tok_struct*
_temp2403=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));*
_temp2403=( struct Cyc_DeclSpec_tok_struct){.tag= Cyc_DeclSpec_tok_tag,.f1=({
struct Cyc_Parse_Declaration_spec* _temp2404=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2404->sc= 0;
_temp2404->tq= Cyc_Absyn_empty_tqual(); _temp2404->type_specs= 0; _temp2404->is_inline=
1; _temp2404->attributes= Cyc_yyget_AttributeList_tok(({ struct _tagged_ptr2
_temp2405= yyvs; struct _xenum_struct** _temp2407= _temp2405.curr + yyvsp_offset;
if( _temp2407 < _temp2405.base? 1: _temp2407 >= _temp2405.last_plus_one){ _throw(
Null_Exception);}* _temp2407;})); _temp2404;})};( struct _xenum_struct*)
_temp2403;}); break; case 35: _LL2402: yyval=({ struct Cyc_DeclSpec_tok_struct*
_temp2409=( struct Cyc_DeclSpec_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclSpec_tok_struct));*
_temp2409=( struct Cyc_DeclSpec_tok_struct){.tag= Cyc_DeclSpec_tok_tag,.f1=({
struct Cyc_Parse_Declaration_spec* _temp2410=( struct Cyc_Parse_Declaration_spec*)
GC_malloc( sizeof( struct Cyc_Parse_Declaration_spec)); _temp2410->sc=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2423= yyvs; struct _xenum_struct** _temp2425= _temp2423.curr
+ yyvsp_offset; if( _temp2425 < _temp2423.base? 1: _temp2425 >= _temp2423.last_plus_one){
_throw( Null_Exception);}* _temp2425;})))->sc; _temp2410->tq=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2420= yyvs; struct _xenum_struct** _temp2422= _temp2420.curr
+ yyvsp_offset; if( _temp2422 < _temp2420.base? 1: _temp2422 >= _temp2420.last_plus_one){
_throw( Null_Exception);}* _temp2422;})))->tq; _temp2410->type_specs=( Cyc_yyget_DeclSpec_tok(({
struct _tagged_ptr2 _temp2417= yyvs; struct _xenum_struct** _temp2419= _temp2417.curr
+ yyvsp_offset; if( _temp2419 < _temp2417.base? 1: _temp2419 >= _temp2417.last_plus_one){
_throw( Null_Exception);}* _temp2419;})))->type_specs; _temp2410->is_inline= 1;
_temp2410->attributes=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2411= yyvs; struct _xenum_struct** _temp2413= _temp2411.curr
+( yyvsp_offset - 1); if( _temp2413 < _temp2411.base? 1: _temp2413 >= _temp2411.last_plus_one){
_throw( Null_Exception);}* _temp2413;})),( Cyc_yyget_DeclSpec_tok(({ struct
_tagged_ptr2 _temp2414= yyvs; struct _xenum_struct** _temp2416= _temp2414.curr +
yyvsp_offset; if( _temp2416 < _temp2414.base? 1: _temp2416 >= _temp2414.last_plus_one){
_throw( Null_Exception);}* _temp2416;})))->attributes); _temp2410;})};( struct
_xenum_struct*) _temp2409;}); break; case 36: _LL2408: yyval=({ struct Cyc_StorageClass_tok_struct*
_temp2427=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));*
_temp2427=( struct Cyc_StorageClass_tok_struct){.tag= Cyc_StorageClass_tok_tag,.f1=
Cyc_Parse_Auto_sc};( struct _xenum_struct*) _temp2427;}); break; case 37:
_LL2426: yyval=({ struct Cyc_StorageClass_tok_struct* _temp2429=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));* _temp2429=( struct Cyc_StorageClass_tok_struct){.tag=
Cyc_StorageClass_tok_tag,.f1= Cyc_Parse_Register_sc};( struct _xenum_struct*)
_temp2429;}); break; case 38: _LL2428: yyval=({ struct Cyc_StorageClass_tok_struct*
_temp2431=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));*
_temp2431=( struct Cyc_StorageClass_tok_struct){.tag= Cyc_StorageClass_tok_tag,.f1=
Cyc_Parse_Static_sc};( struct _xenum_struct*) _temp2431;}); break; case 39:
_LL2430: yyval=({ struct Cyc_StorageClass_tok_struct* _temp2433=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));* _temp2433=( struct Cyc_StorageClass_tok_struct){.tag=
Cyc_StorageClass_tok_tag,.f1= Cyc_Parse_Extern_sc};( struct _xenum_struct*)
_temp2433;}); break; case 40: _LL2432: if( Cyc_String_strcmp( Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp2435= yyvs; struct _xenum_struct** _temp2437= _temp2435.curr
+ yyvsp_offset; if( _temp2437 < _temp2435.base? 1: _temp2437 >= _temp2435.last_plus_one){
_throw( Null_Exception);}* _temp2437;})),( struct _tagged_string)({ char*
_temp2438=( char*)"C"; struct _tagged_string _temp2439; _temp2439.curr=
_temp2438; _temp2439.base= _temp2438; _temp2439.last_plus_one= _temp2438 + 2;
_temp2439;})) != 0){ Cyc_Parse_err(( struct _tagged_string)({ char* _temp2440=(
char*)"only extern or extern \"C\" is allowed"; struct _tagged_string _temp2441;
_temp2441.curr= _temp2440; _temp2441.base= _temp2440; _temp2441.last_plus_one=
_temp2440 + 37; _temp2441;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2442= yyls; struct Cyc_Yyltype* _temp2444= _temp2442.curr +( yylsp_offset -
1); if( _temp2444 < _temp2442.base? 1: _temp2444 >= _temp2442.last_plus_one){
_throw( Null_Exception);}* _temp2444;}).first_line,({ struct _tagged_ptr3
_temp2445= yyls; struct Cyc_Yyltype* _temp2447= _temp2445.curr + yylsp_offset;
if( _temp2447 < _temp2445.base? 1: _temp2447 >= _temp2445.last_plus_one){ _throw(
Null_Exception);}* _temp2447;}).last_line));} yyval=({ struct Cyc_StorageClass_tok_struct*
_temp2448=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));*
_temp2448=( struct Cyc_StorageClass_tok_struct){.tag= Cyc_StorageClass_tok_tag,.f1=
Cyc_Parse_ExternC_sc};( struct _xenum_struct*) _temp2448;}); break; case 41:
_LL2434: yyval=({ struct Cyc_StorageClass_tok_struct* _temp2450=( struct Cyc_StorageClass_tok_struct*)
GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));* _temp2450=( struct Cyc_StorageClass_tok_struct){.tag=
Cyc_StorageClass_tok_tag,.f1= Cyc_Parse_Typedef_sc};( struct _xenum_struct*)
_temp2450;}); break; case 42: _LL2449: yyval=({ struct Cyc_StorageClass_tok_struct*
_temp2452=( struct Cyc_StorageClass_tok_struct*) GC_malloc( sizeof( struct Cyc_StorageClass_tok_struct));*
_temp2452=( struct Cyc_StorageClass_tok_struct){.tag= Cyc_StorageClass_tok_tag,.f1=
Cyc_Parse_Abstract_sc};( struct _xenum_struct*) _temp2452;}); break; case 43:
_LL2451: yyval=({ struct Cyc_AttributeList_tok_struct* _temp2454=( struct Cyc_AttributeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));* _temp2454=( struct
Cyc_AttributeList_tok_struct){.tag= Cyc_AttributeList_tok_tag,.f1= 0};( struct
_xenum_struct*) _temp2454;}); break; case 44: _LL2453: yyval=({ struct
_tagged_ptr2 _temp2456= yyvs; struct _xenum_struct** _temp2458= _temp2456.curr +
yyvsp_offset; if( _temp2458 < _temp2456.base? 1: _temp2458 >= _temp2456.last_plus_one){
_throw( Null_Exception);}* _temp2458;}); break; case 45: _LL2455: yyval=({
struct Cyc_AttributeList_tok_struct* _temp2460=( struct Cyc_AttributeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));* _temp2460=( struct
Cyc_AttributeList_tok_struct){.tag= Cyc_AttributeList_tok_tag,.f1= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2461= yyvs; struct _xenum_struct** _temp2463= _temp2461.curr
+( yyvsp_offset - 2); if( _temp2463 < _temp2461.base? 1: _temp2463 >= _temp2461.last_plus_one){
_throw( Null_Exception);}* _temp2463;}))};( struct _xenum_struct*) _temp2460;});
break; case 46: _LL2459: yyval=({ struct Cyc_AttributeList_tok_struct* _temp2465=(
struct Cyc_AttributeList_tok_struct*) GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));*
_temp2465=( struct Cyc_AttributeList_tok_struct){.tag= Cyc_AttributeList_tok_tag,.f1=({
struct Cyc_List_List* _temp2466=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp2466->hd=( void*) Cyc_yyget_Attribute_tok(({ struct
_tagged_ptr2 _temp2467= yyvs; struct _xenum_struct** _temp2469= _temp2467.curr +
yyvsp_offset; if( _temp2469 < _temp2467.base? 1: _temp2469 >= _temp2467.last_plus_one){
_throw( Null_Exception);}* _temp2469;})); _temp2466->tl= 0; _temp2466;})};(
struct _xenum_struct*) _temp2465;}); break; case 47: _LL2464: yyval=({ struct
Cyc_AttributeList_tok_struct* _temp2471=( struct Cyc_AttributeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_AttributeList_tok_struct));* _temp2471=( struct
Cyc_AttributeList_tok_struct){.tag= Cyc_AttributeList_tok_tag,.f1=({ struct Cyc_List_List*
_temp2472=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp2472->hd=( void*) Cyc_yyget_Attribute_tok(({ struct _tagged_ptr2 _temp2476=
yyvs; struct _xenum_struct** _temp2478= _temp2476.curr +( yyvsp_offset - 2); if(
_temp2478 < _temp2476.base? 1: _temp2478 >= _temp2476.last_plus_one){ _throw(
Null_Exception);}* _temp2478;})); _temp2472->tl= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp2473= yyvs; struct _xenum_struct** _temp2475= _temp2473.curr
+ yyvsp_offset; if( _temp2475 < _temp2473.base? 1: _temp2475 >= _temp2473.last_plus_one){
_throw( Null_Exception);}* _temp2475;})); _temp2472;})};( struct _xenum_struct*)
_temp2471;}); break; case 48: _LL2470: { struct _tagged_string s= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp2480= yyvs; struct _xenum_struct** _temp2482= _temp2480.curr
+ yyvsp_offset; if( _temp2482 < _temp2480.base? 1: _temp2482 >= _temp2480.last_plus_one){
_throw( Null_Exception);}* _temp2482;})); void* a; if( Cyc_String_zstrcmp( s,(
struct _tagged_string)({ char* _temp2483=( char*)"stdcall"; struct
_tagged_string _temp2484; _temp2484.curr= _temp2483; _temp2484.base= _temp2483;
_temp2484.last_plus_one= _temp2483 + 8; _temp2484;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2485=( char*)"__stdcall__"; struct
_tagged_string _temp2486; _temp2486.curr= _temp2485; _temp2486.base= _temp2485;
_temp2486.last_plus_one= _temp2485 + 12; _temp2486;})) == 0){ a= Cyc_Absyn_Stdcall_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2487=(
char*)"cdecl"; struct _tagged_string _temp2488; _temp2488.curr= _temp2487;
_temp2488.base= _temp2487; _temp2488.last_plus_one= _temp2487 + 6; _temp2488;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2489=( char*)"__cdecl__";
struct _tagged_string _temp2490; _temp2490.curr= _temp2489; _temp2490.base=
_temp2489; _temp2490.last_plus_one= _temp2489 + 10; _temp2490;})) == 0){ a= Cyc_Absyn_Cdecl_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2491=(
char*)"noreturn"; struct _tagged_string _temp2492; _temp2492.curr= _temp2491;
_temp2492.base= _temp2491; _temp2492.last_plus_one= _temp2491 + 9; _temp2492;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2493=( char*)"__noreturn__";
struct _tagged_string _temp2494; _temp2494.curr= _temp2493; _temp2494.base=
_temp2493; _temp2494.last_plus_one= _temp2493 + 13; _temp2494;})) == 0){ a= Cyc_Absyn_Noreturn_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2495=(
char*)"noreturn"; struct _tagged_string _temp2496; _temp2496.curr= _temp2495;
_temp2496.base= _temp2495; _temp2496.last_plus_one= _temp2495 + 9; _temp2496;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2497=( char*)"__noreturn__";
struct _tagged_string _temp2498; _temp2498.curr= _temp2497; _temp2498.base=
_temp2497; _temp2498.last_plus_one= _temp2497 + 13; _temp2498;})) == 0){ a= Cyc_Absyn_Noreturn_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2499=(
char*)"__const__"; struct _tagged_string _temp2500; _temp2500.curr= _temp2499;
_temp2500.base= _temp2499; _temp2500.last_plus_one= _temp2499 + 10; _temp2500;}))
== 0){ a= Cyc_Absyn_Const_att;} else{ if( Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2501=( char*)"aligned"; struct _tagged_string
_temp2502; _temp2502.curr= _temp2501; _temp2502.base= _temp2501; _temp2502.last_plus_one=
_temp2501 + 8; _temp2502;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2503=( char*)"__aligned__"; struct _tagged_string
_temp2504; _temp2504.curr= _temp2503; _temp2504.base= _temp2503; _temp2504.last_plus_one=
_temp2503 + 12; _temp2504;})) == 0){ a=({ struct Cyc_Absyn_Aligned_att_struct*
_temp2505=( struct Cyc_Absyn_Aligned_att_struct*) GC_malloc_atomic( sizeof(
struct Cyc_Absyn_Aligned_att_struct)); _temp2505->tag= Cyc_Absyn_Aligned_att_tag;
_temp2505->f1= - 1;( void*) _temp2505;});} else{ if( Cyc_String_zstrcmp( s,(
struct _tagged_string)({ char* _temp2506=( char*)"packed"; struct _tagged_string
_temp2507; _temp2507.curr= _temp2506; _temp2507.base= _temp2506; _temp2507.last_plus_one=
_temp2506 + 7; _temp2507;})) == 0? 1: Cyc_String_zstrcmp( s,( struct
_tagged_string)({ char* _temp2508=( char*)"__packed__"; struct _tagged_string
_temp2509; _temp2509.curr= _temp2508; _temp2509.base= _temp2508; _temp2509.last_plus_one=
_temp2508 + 11; _temp2509;})) == 0){ a= Cyc_Absyn_Packed_att;} else{ if( Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2510=( char*)"shared"; struct
_tagged_string _temp2511; _temp2511.curr= _temp2510; _temp2511.base= _temp2510;
_temp2511.last_plus_one= _temp2510 + 7; _temp2511;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2512=( char*)"__shared__"; struct
_tagged_string _temp2513; _temp2513.curr= _temp2512; _temp2513.base= _temp2512;
_temp2513.last_plus_one= _temp2512 + 11; _temp2513;})) == 0){ a= Cyc_Absyn_Shared_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2514=(
char*)"unused"; struct _tagged_string _temp2515; _temp2515.curr= _temp2514;
_temp2515.base= _temp2514; _temp2515.last_plus_one= _temp2514 + 7; _temp2515;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2516=( char*)"__unused__";
struct _tagged_string _temp2517; _temp2517.curr= _temp2516; _temp2517.base=
_temp2516; _temp2517.last_plus_one= _temp2516 + 11; _temp2517;})) == 0){ a= Cyc_Absyn_Unused_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2518=(
char*)"weak"; struct _tagged_string _temp2519; _temp2519.curr= _temp2518;
_temp2519.base= _temp2518; _temp2519.last_plus_one= _temp2518 + 5; _temp2519;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2520=( char*)"__weak__";
struct _tagged_string _temp2521; _temp2521.curr= _temp2520; _temp2521.base=
_temp2520; _temp2521.last_plus_one= _temp2520 + 9; _temp2521;})) == 0){ a= Cyc_Absyn_Weak_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2522=(
char*)"dllimport"; struct _tagged_string _temp2523; _temp2523.curr= _temp2522;
_temp2523.base= _temp2522; _temp2523.last_plus_one= _temp2522 + 10; _temp2523;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2524=( char*)"__dllimport__";
struct _tagged_string _temp2525; _temp2525.curr= _temp2524; _temp2525.base=
_temp2524; _temp2525.last_plus_one= _temp2524 + 14; _temp2525;})) == 0){ a= Cyc_Absyn_Dllimport_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2526=(
char*)"dllexport"; struct _tagged_string _temp2527; _temp2527.curr= _temp2526;
_temp2527.base= _temp2526; _temp2527.last_plus_one= _temp2526 + 10; _temp2527;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2528=( char*)"__dllexport__";
struct _tagged_string _temp2529; _temp2529.curr= _temp2528; _temp2529.base=
_temp2528; _temp2529.last_plus_one= _temp2528 + 14; _temp2529;})) == 0){ a= Cyc_Absyn_Dllexport_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2530=(
char*)"no_instrument_function"; struct _tagged_string _temp2531; _temp2531.curr=
_temp2530; _temp2531.base= _temp2530; _temp2531.last_plus_one= _temp2530 + 23;
_temp2531;})) == 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char*
_temp2532=( char*)"__no_instrument_function__"; struct _tagged_string _temp2533;
_temp2533.curr= _temp2532; _temp2533.base= _temp2532; _temp2533.last_plus_one=
_temp2532 + 27; _temp2533;})) == 0){ a= Cyc_Absyn_No_instrument_function_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2534=(
char*)"constructor"; struct _tagged_string _temp2535; _temp2535.curr= _temp2534;
_temp2535.base= _temp2534; _temp2535.last_plus_one= _temp2534 + 12; _temp2535;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2536=( char*)"__constructor__";
struct _tagged_string _temp2537; _temp2537.curr= _temp2536; _temp2537.base=
_temp2536; _temp2537.last_plus_one= _temp2536 + 16; _temp2537;})) == 0){ a= Cyc_Absyn_Constructor_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2538=(
char*)"destructor"; struct _tagged_string _temp2539; _temp2539.curr= _temp2538;
_temp2539.base= _temp2538; _temp2539.last_plus_one= _temp2538 + 11; _temp2539;}))
== 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2540=( char*)"__destructor__";
struct _tagged_string _temp2541; _temp2541.curr= _temp2540; _temp2541.base=
_temp2540; _temp2541.last_plus_one= _temp2540 + 15; _temp2541;})) == 0){ a= Cyc_Absyn_Destructor_att;}
else{ if( Cyc_String_zstrcmp( s,( struct _tagged_string)({ char* _temp2542=(
char*)"no_check_memory_usage"; struct _tagged_string _temp2543; _temp2543.curr=
_temp2542; _temp2543.base= _temp2542; _temp2543.last_plus_one= _temp2542 + 22;
_temp2543;})) == 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char*
_temp2544=( char*)"__no_check_memory_usage__"; struct _tagged_string _temp2545;
_temp2545.curr= _temp2544; _temp2545.base= _temp2544; _temp2545.last_plus_one=
_temp2544 + 26; _temp2545;})) == 0){ a= Cyc_Absyn_No_check_memory_usage_att;}
else{ Cyc_Parse_err(( struct _tagged_string)({ char* _temp2546=( char*)"unrecognized attribute";
struct _tagged_string _temp2547; _temp2547.curr= _temp2546; _temp2547.base=
_temp2546; _temp2547.last_plus_one= _temp2546 + 23; _temp2547;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2548= yyls; struct Cyc_Yyltype* _temp2550= _temp2548.curr
+ yylsp_offset; if( _temp2550 < _temp2548.base? 1: _temp2550 >= _temp2548.last_plus_one){
_throw( Null_Exception);}* _temp2550;}).first_line,({ struct _tagged_ptr3
_temp2551= yyls; struct Cyc_Yyltype* _temp2553= _temp2551.curr + yylsp_offset;
if( _temp2553 < _temp2551.base? 1: _temp2553 >= _temp2551.last_plus_one){ _throw(
Null_Exception);}* _temp2553;}).last_line)); a= Cyc_Absyn_Cdecl_att;}}}}}}}}}}}}}}}}
yyval=({ struct Cyc_Attribute_tok_struct* _temp2554=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));* _temp2554=( struct Cyc_Attribute_tok_struct){.tag=
Cyc_Attribute_tok_tag,.f1= a};( struct _xenum_struct*) _temp2554;}); break;}
case 49: _LL2479: yyval=({ struct Cyc_Attribute_tok_struct* _temp2556=( struct
Cyc_Attribute_tok_struct*) GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));*
_temp2556=( struct Cyc_Attribute_tok_struct){.tag= Cyc_Attribute_tok_tag,.f1=
Cyc_Absyn_Const_att};( struct _xenum_struct*) _temp2556;}); break; case 50:
_LL2555: { struct _tagged_string s= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp2558= yyvs; struct _xenum_struct** _temp2560= _temp2558.curr +(
yyvsp_offset - 3); if( _temp2560 < _temp2558.base? 1: _temp2560 >= _temp2558.last_plus_one){
_throw( Null_Exception);}* _temp2560;})); struct _tuple11 _temp2566; int
_temp2567; void* _temp2569; struct _tuple11* _temp2564= Cyc_yyget_Int_tok(({
struct _tagged_ptr2 _temp2561= yyvs; struct _xenum_struct** _temp2563= _temp2561.curr
+( yyvsp_offset - 1); if( _temp2563 < _temp2561.base? 1: _temp2563 >= _temp2561.last_plus_one){
_throw( Null_Exception);}* _temp2563;})); _temp2566=* _temp2564; _LL2570:
_temp2569= _temp2566.f1; goto _LL2568; _LL2568: _temp2567= _temp2566.f2; goto
_LL2565; _LL2565: { void* a; if( Cyc_String_zstrcmp( s,( struct _tagged_string)({
char* _temp2571=( char*)"regparm"; struct _tagged_string _temp2572; _temp2572.curr=
_temp2571; _temp2572.base= _temp2571; _temp2572.last_plus_one= _temp2571 + 8;
_temp2572;})) == 0? 1: Cyc_String_zstrcmp( s,( struct _tagged_string)({ char*
_temp2573=( char*)"__regparm__"; struct _tagged_string _temp2574; _temp2574.curr=
_temp2573; _temp2574.base= _temp2573; _temp2574.last_plus_one= _temp2573 + 12;
_temp2574;})) == 0){ if( _temp2567 <= 0? 1: _temp2567 > 3){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp2575=( char*)"regparm requires value between 1 and 3";
struct _tagged_string _temp2576; _temp2576.curr= _temp2575; _temp2576.base=
_temp2575; _temp2576.last_plus_one= _temp2575 + 39; _temp2576;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2577= yyls; struct Cyc_Yyltype* _temp2579= _temp2577.curr
+( yylsp_offset - 1); if( _temp2579 < _temp2577.base? 1: _temp2579 >= _temp2577.last_plus_one){
_throw( Null_Exception);}* _temp2579;}).first_line,({ struct _tagged_ptr3
_temp2580= yyls; struct Cyc_Yyltype* _temp2582= _temp2580.curr +( yylsp_offset -
1); if( _temp2582 < _temp2580.base? 1: _temp2582 >= _temp2580.last_plus_one){
_throw( Null_Exception);}* _temp2582;}).last_line));} a=({ struct Cyc_Absyn_Regparm_att_struct*
_temp2583=( struct Cyc_Absyn_Regparm_att_struct*) GC_malloc_atomic( sizeof(
struct Cyc_Absyn_Regparm_att_struct)); _temp2583->tag= Cyc_Absyn_Regparm_att_tag;
_temp2583->f1= _temp2567;( void*) _temp2583;});} else{ if( Cyc_String_zstrcmp( s,(
struct _tagged_string)({ char* _temp2584=( char*)"aligned"; struct
_tagged_string _temp2585; _temp2585.curr= _temp2584; _temp2585.base= _temp2584;
_temp2585.last_plus_one= _temp2584 + 8; _temp2585;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2586=( char*)"__aligned__"; struct
_tagged_string _temp2587; _temp2587.curr= _temp2586; _temp2587.base= _temp2586;
_temp2587.last_plus_one= _temp2586 + 12; _temp2587;})) == 0){ if( _temp2567 < 0){
Cyc_Parse_err(( struct _tagged_string)({ char* _temp2588=( char*)"aligned requires positive power of two";
struct _tagged_string _temp2589; _temp2589.curr= _temp2588; _temp2589.base=
_temp2588; _temp2589.last_plus_one= _temp2588 + 39; _temp2589;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2590= yyls; struct Cyc_Yyltype* _temp2592= _temp2590.curr
+( yylsp_offset - 1); if( _temp2592 < _temp2590.base? 1: _temp2592 >= _temp2590.last_plus_one){
_throw( Null_Exception);}* _temp2592;}).first_line,({ struct _tagged_ptr3
_temp2593= yyls; struct Cyc_Yyltype* _temp2595= _temp2593.curr +( yylsp_offset -
1); if( _temp2595 < _temp2593.base? 1: _temp2595 >= _temp2593.last_plus_one){
_throw( Null_Exception);}* _temp2595;}).last_line));}{ unsigned int j=(
unsigned int) _temp2567; for( 0;( j &( unsigned int) 1) ==( unsigned int) 0; j=
j >>( unsigned int) 1){;} j= j >>( unsigned int) 1; if( j !=( unsigned int) 0){
Cyc_Parse_err(( struct _tagged_string)({ char* _temp2596=( char*)"aligned requires positive power of two";
struct _tagged_string _temp2597; _temp2597.curr= _temp2596; _temp2597.base=
_temp2596; _temp2597.last_plus_one= _temp2596 + 39; _temp2597;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2598= yyls; struct Cyc_Yyltype* _temp2600= _temp2598.curr
+( yylsp_offset - 1); if( _temp2600 < _temp2598.base? 1: _temp2600 >= _temp2598.last_plus_one){
_throw( Null_Exception);}* _temp2600;}).first_line,({ struct _tagged_ptr3
_temp2601= yyls; struct Cyc_Yyltype* _temp2603= _temp2601.curr +( yylsp_offset -
1); if( _temp2603 < _temp2601.base? 1: _temp2603 >= _temp2601.last_plus_one){
_throw( Null_Exception);}* _temp2603;}).last_line));} a=({ struct Cyc_Absyn_Aligned_att_struct*
_temp2604=( struct Cyc_Absyn_Aligned_att_struct*) GC_malloc_atomic( sizeof(
struct Cyc_Absyn_Aligned_att_struct)); _temp2604->tag= Cyc_Absyn_Aligned_att_tag;
_temp2604->f1= _temp2567;( void*) _temp2604;});}} else{ Cyc_Parse_err(( struct
_tagged_string)({ char* _temp2605=( char*)"unrecognized attribute"; struct
_tagged_string _temp2606; _temp2606.curr= _temp2605; _temp2606.base= _temp2605;
_temp2606.last_plus_one= _temp2605 + 23; _temp2606;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2607= yyls; struct Cyc_Yyltype* _temp2609= _temp2607.curr
+( yylsp_offset - 3); if( _temp2609 < _temp2607.base? 1: _temp2609 >= _temp2607.last_plus_one){
_throw( Null_Exception);}* _temp2609;}).first_line,({ struct _tagged_ptr3
_temp2610= yyls; struct Cyc_Yyltype* _temp2612= _temp2610.curr +( yylsp_offset -
3); if( _temp2612 < _temp2610.base? 1: _temp2612 >= _temp2610.last_plus_one){
_throw( Null_Exception);}* _temp2612;}).last_line)); a= Cyc_Absyn_Cdecl_att;}}
yyval=({ struct Cyc_Attribute_tok_struct* _temp2613=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));* _temp2613=( struct Cyc_Attribute_tok_struct){.tag=
Cyc_Attribute_tok_tag,.f1= a};( struct _xenum_struct*) _temp2613;}); break;}}
case 51: _LL2557: { struct _tagged_string s= Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp2615= yyvs; struct _xenum_struct** _temp2617= _temp2615.curr +(
yyvsp_offset - 3); if( _temp2617 < _temp2615.base? 1: _temp2617 >= _temp2615.last_plus_one){
_throw( Null_Exception);}* _temp2617;})); struct _tagged_string str= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp2618= yyvs; struct _xenum_struct** _temp2620= _temp2618.curr
+( yyvsp_offset - 1); if( _temp2620 < _temp2618.base? 1: _temp2620 >= _temp2618.last_plus_one){
_throw( Null_Exception);}* _temp2620;})); void* a; if( Cyc_String_zstrcmp( s,(
struct _tagged_string)({ char* _temp2621=( char*)"section"; struct
_tagged_string _temp2622; _temp2622.curr= _temp2621; _temp2622.base= _temp2621;
_temp2622.last_plus_one= _temp2621 + 8; _temp2622;})) == 0? 1: Cyc_String_zstrcmp(
s,( struct _tagged_string)({ char* _temp2623=( char*)"__section__"; struct
_tagged_string _temp2624; _temp2624.curr= _temp2623; _temp2624.base= _temp2623;
_temp2624.last_plus_one= _temp2623 + 12; _temp2624;}))){ a=({ struct Cyc_Absyn_Section_att_struct*
_temp2625=( struct Cyc_Absyn_Section_att_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Section_att_struct));
_temp2625->tag= Cyc_Absyn_Section_att_tag; _temp2625->f1= str;( void*) _temp2625;});}
else{ Cyc_Parse_err(( struct _tagged_string)({ char* _temp2626=( char*)"unrecognized attribute";
struct _tagged_string _temp2627; _temp2627.curr= _temp2626; _temp2627.base=
_temp2626; _temp2627.last_plus_one= _temp2626 + 23; _temp2627;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2628= yyls; struct Cyc_Yyltype* _temp2630= _temp2628.curr
+( yylsp_offset - 3); if( _temp2630 < _temp2628.base? 1: _temp2630 >= _temp2628.last_plus_one){
_throw( Null_Exception);}* _temp2630;}).first_line,({ struct _tagged_ptr3
_temp2631= yyls; struct Cyc_Yyltype* _temp2633= _temp2631.curr +( yylsp_offset -
3); if( _temp2633 < _temp2631.base? 1: _temp2633 >= _temp2631.last_plus_one){
_throw( Null_Exception);}* _temp2633;}).last_line)); a= Cyc_Absyn_Cdecl_att;}
yyval=({ struct Cyc_Attribute_tok_struct* _temp2634=( struct Cyc_Attribute_tok_struct*)
GC_malloc( sizeof( struct Cyc_Attribute_tok_struct));* _temp2634=( struct Cyc_Attribute_tok_struct){.tag=
Cyc_Attribute_tok_tag,.f1= a};( struct _xenum_struct*) _temp2634;}); break;}
case 52: _LL2614: yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2636=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2636=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec( Cyc_Absyn_VoidType, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2637= yyls; struct Cyc_Yyltype* _temp2639= _temp2637.curr +
yylsp_offset; if( _temp2639 < _temp2637.base? 1: _temp2639 >= _temp2637.last_plus_one){
_throw( Null_Exception);}* _temp2639;}).first_line,({ struct _tagged_ptr3
_temp2640= yyls; struct Cyc_Yyltype* _temp2642= _temp2640.curr + yylsp_offset;
if( _temp2642 < _temp2640.base? 1: _temp2642 >= _temp2640.last_plus_one){ _throw(
Null_Exception);}* _temp2642;}).last_line))};( struct _xenum_struct*) _temp2636;});
break; case 53: _LL2635: yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2644=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2644=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec( Cyc_Absyn_new_evar( Cyc_Absyn_MemKind), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2645= yyls; struct Cyc_Yyltype* _temp2647= _temp2645.curr
+ yylsp_offset; if( _temp2647 < _temp2645.base? 1: _temp2647 >= _temp2645.last_plus_one){
_throw( Null_Exception);}* _temp2647;}).first_line,({ struct _tagged_ptr3
_temp2648= yyls; struct Cyc_Yyltype* _temp2650= _temp2648.curr + yylsp_offset;
if( _temp2650 < _temp2648.base? 1: _temp2650 >= _temp2648.last_plus_one){ _throw(
Null_Exception);}* _temp2650;}).last_line))};( struct _xenum_struct*) _temp2644;});
break; case 54: _LL2643: yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2652=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2652=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec( Cyc_Absyn_uchar_t, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2653= yyls; struct Cyc_Yyltype* _temp2655= _temp2653.curr +
yylsp_offset; if( _temp2655 < _temp2653.base? 1: _temp2655 >= _temp2653.last_plus_one){
_throw( Null_Exception);}* _temp2655;}).first_line,({ struct _tagged_ptr3
_temp2656= yyls; struct Cyc_Yyltype* _temp2658= _temp2656.curr + yylsp_offset;
if( _temp2658 < _temp2656.base? 1: _temp2658 >= _temp2656.last_plus_one){ _throw(
Null_Exception);}* _temp2658;}).last_line))};( struct _xenum_struct*) _temp2652;});
break; case 55: _LL2651: yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2660=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2660=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=({
struct Cyc_Parse_Short_spec_struct* _temp2661=( struct Cyc_Parse_Short_spec_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Short_spec_struct)); _temp2661->tag= Cyc_Parse_Short_spec_tag;
_temp2661->f1= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2662=
yyls; struct Cyc_Yyltype* _temp2664= _temp2662.curr + yylsp_offset; if(
_temp2664 < _temp2662.base? 1: _temp2664 >= _temp2662.last_plus_one){ _throw(
Null_Exception);}* _temp2664;}).first_line,({ struct _tagged_ptr3 _temp2665=
yyls; struct Cyc_Yyltype* _temp2667= _temp2665.curr + yylsp_offset; if(
_temp2667 < _temp2665.base? 1: _temp2667 >= _temp2665.last_plus_one){ _throw(
Null_Exception);}* _temp2667;}).last_line);( void*) _temp2661;})};( struct
_xenum_struct*) _temp2660;}); break; case 56: _LL2659: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp2669=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2669=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec( Cyc_Absyn_sint_t, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2670= yyls; struct Cyc_Yyltype* _temp2672= _temp2670.curr +
yylsp_offset; if( _temp2672 < _temp2670.base? 1: _temp2672 >= _temp2670.last_plus_one){
_throw( Null_Exception);}* _temp2672;}).first_line,({ struct _tagged_ptr3
_temp2673= yyls; struct Cyc_Yyltype* _temp2675= _temp2673.curr + yylsp_offset;
if( _temp2675 < _temp2673.base? 1: _temp2675 >= _temp2673.last_plus_one){ _throw(
Null_Exception);}* _temp2675;}).last_line))};( struct _xenum_struct*) _temp2669;});
break; case 57: _LL2668: yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2677=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2677=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=({
struct Cyc_Parse_Long_spec_struct* _temp2678=( struct Cyc_Parse_Long_spec_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Long_spec_struct)); _temp2678->tag= Cyc_Parse_Long_spec_tag;
_temp2678->f1= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2679=
yyls; struct Cyc_Yyltype* _temp2681= _temp2679.curr + yylsp_offset; if(
_temp2681 < _temp2679.base? 1: _temp2681 >= _temp2679.last_plus_one){ _throw(
Null_Exception);}* _temp2681;}).first_line,({ struct _tagged_ptr3 _temp2682=
yyls; struct Cyc_Yyltype* _temp2684= _temp2682.curr + yylsp_offset; if(
_temp2684 < _temp2682.base? 1: _temp2684 >= _temp2682.last_plus_one){ _throw(
Null_Exception);}* _temp2684;}).last_line);( void*) _temp2678;})};( struct
_xenum_struct*) _temp2677;}); break; case 58: _LL2676: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp2686=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2686=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec( Cyc_Absyn_float_t, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2687= yyls; struct Cyc_Yyltype* _temp2689= _temp2687.curr +
yylsp_offset; if( _temp2689 < _temp2687.base? 1: _temp2689 >= _temp2687.last_plus_one){
_throw( Null_Exception);}* _temp2689;}).first_line,({ struct _tagged_ptr3
_temp2690= yyls; struct Cyc_Yyltype* _temp2692= _temp2690.curr + yylsp_offset;
if( _temp2692 < _temp2690.base? 1: _temp2692 >= _temp2690.last_plus_one){ _throw(
Null_Exception);}* _temp2692;}).last_line))};( struct _xenum_struct*) _temp2686;});
break; case 59: _LL2685: yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2694=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2694=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec( Cyc_Absyn_double_t, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2695= yyls; struct Cyc_Yyltype* _temp2697= _temp2695.curr +
yylsp_offset; if( _temp2697 < _temp2695.base? 1: _temp2697 >= _temp2695.last_plus_one){
_throw( Null_Exception);}* _temp2697;}).first_line,({ struct _tagged_ptr3
_temp2698= yyls; struct Cyc_Yyltype* _temp2700= _temp2698.curr + yylsp_offset;
if( _temp2700 < _temp2698.base? 1: _temp2700 >= _temp2698.last_plus_one){ _throw(
Null_Exception);}* _temp2700;}).last_line))};( struct _xenum_struct*) _temp2694;});
break; case 60: _LL2693: yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2702=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2702=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=({
struct Cyc_Parse_Signed_spec_struct* _temp2703=( struct Cyc_Parse_Signed_spec_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Signed_spec_struct)); _temp2703->tag= Cyc_Parse_Signed_spec_tag;
_temp2703->f1= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2704=
yyls; struct Cyc_Yyltype* _temp2706= _temp2704.curr + yylsp_offset; if(
_temp2706 < _temp2704.base? 1: _temp2706 >= _temp2704.last_plus_one){ _throw(
Null_Exception);}* _temp2706;}).first_line,({ struct _tagged_ptr3 _temp2707=
yyls; struct Cyc_Yyltype* _temp2709= _temp2707.curr + yylsp_offset; if(
_temp2709 < _temp2707.base? 1: _temp2709 >= _temp2707.last_plus_one){ _throw(
Null_Exception);}* _temp2709;}).last_line);( void*) _temp2703;})};( struct
_xenum_struct*) _temp2702;}); break; case 61: _LL2701: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp2711=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2711=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=({
struct Cyc_Parse_Unsigned_spec_struct* _temp2712=( struct Cyc_Parse_Unsigned_spec_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Unsigned_spec_struct)); _temp2712->tag= Cyc_Parse_Unsigned_spec_tag;
_temp2712->f1= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2713=
yyls; struct Cyc_Yyltype* _temp2715= _temp2713.curr + yylsp_offset; if(
_temp2715 < _temp2713.base? 1: _temp2715 >= _temp2713.last_plus_one){ _throw(
Null_Exception);}* _temp2715;}).first_line,({ struct _tagged_ptr3 _temp2716=
yyls; struct Cyc_Yyltype* _temp2718= _temp2716.curr + yylsp_offset; if(
_temp2718 < _temp2716.base? 1: _temp2718 >= _temp2716.last_plus_one){ _throw(
Null_Exception);}* _temp2718;}).last_line);( void*) _temp2712;})};( struct
_xenum_struct*) _temp2711;}); break; case 62: _LL2710: yyval=({ struct
_tagged_ptr2 _temp2720= yyvs; struct _xenum_struct** _temp2722= _temp2720.curr +
yyvsp_offset; if( _temp2722 < _temp2720.base? 1: _temp2722 >= _temp2720.last_plus_one){
_throw( Null_Exception);}* _temp2722;}); break; case 63: _LL2719: yyval=({
struct _tagged_ptr2 _temp2724= yyvs; struct _xenum_struct** _temp2726= _temp2724.curr
+ yyvsp_offset; if( _temp2726 < _temp2724.base? 1: _temp2726 >= _temp2724.last_plus_one){
_throw( Null_Exception);}* _temp2726;}); break; case 64: _LL2723: yyval=({
struct Cyc_TypeSpecifier_tok_struct* _temp2728=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));* _temp2728=( struct
Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1= Cyc_Parse_type_spec(
Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2729= yyvs;
struct _xenum_struct** _temp2731= _temp2729.curr + yyvsp_offset; if( _temp2731 <
_temp2729.base? 1: _temp2731 >= _temp2729.last_plus_one){ _throw( Null_Exception);}*
_temp2731;})),(( struct Cyc_Absyn_Conref*(*)()) Cyc_Absyn_empty_conref)()), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2732= yyls; struct Cyc_Yyltype* _temp2734= _temp2732.curr
+ yylsp_offset; if( _temp2734 < _temp2732.base? 1: _temp2734 >= _temp2732.last_plus_one){
_throw( Null_Exception);}* _temp2734;}).first_line,({ struct _tagged_ptr3
_temp2735= yyls; struct Cyc_Yyltype* _temp2737= _temp2735.curr + yylsp_offset;
if( _temp2737 < _temp2735.base? 1: _temp2737 >= _temp2735.last_plus_one){ _throw(
Null_Exception);}* _temp2737;}).last_line))};( struct _xenum_struct*) _temp2728;});
break; case 65: _LL2727: yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2739=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2739=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec( Cyc_Parse_id2type( Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp2740= yyvs; struct _xenum_struct** _temp2742= _temp2740.curr +(
yyvsp_offset - 2); if( _temp2742 < _temp2740.base? 1: _temp2742 >= _temp2740.last_plus_one){
_throw( Null_Exception);}* _temp2742;})),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)( Cyc_yyget_Kind_tok(({ struct _tagged_ptr2 _temp2743= yyvs;
struct _xenum_struct** _temp2745= _temp2743.curr + yyvsp_offset; if( _temp2745 <
_temp2743.base? 1: _temp2745 >= _temp2743.last_plus_one){ _throw( Null_Exception);}*
_temp2745;})))), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2746=
yyls; struct Cyc_Yyltype* _temp2748= _temp2746.curr +( yylsp_offset - 2); if(
_temp2748 < _temp2746.base? 1: _temp2748 >= _temp2746.last_plus_one){ _throw(
Null_Exception);}* _temp2748;}).first_line,({ struct _tagged_ptr3 _temp2749=
yyls; struct Cyc_Yyltype* _temp2751= _temp2749.curr + yylsp_offset; if(
_temp2751 < _temp2749.base? 1: _temp2751 >= _temp2749.last_plus_one){ _throw(
Null_Exception);}* _temp2751;}).last_line))};( struct _xenum_struct*) _temp2739;});
break; case 66: _LL2738: yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2753=(
struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2753=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec(({ struct Cyc_Absyn_TypedefType_struct* _temp2754=( struct
Cyc_Absyn_TypedefType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypedefType_struct));
_temp2754->tag= Cyc_Absyn_TypedefType_tag; _temp2754->f1= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp2755= yyvs; struct _xenum_struct** _temp2757= _temp2755.curr
+( yyvsp_offset - 1); if( _temp2757 < _temp2755.base? 1: _temp2757 >= _temp2755.last_plus_one){
_throw( Null_Exception);}* _temp2757;})); _temp2754->f2= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp2758= yyvs; struct _xenum_struct** _temp2760= _temp2758.curr
+ yyvsp_offset; if( _temp2760 < _temp2758.base? 1: _temp2760 >= _temp2758.last_plus_one){
_throw( Null_Exception);}* _temp2760;})); _temp2754->f3= 0;( void*) _temp2754;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2761= yyls; struct Cyc_Yyltype*
_temp2763= _temp2761.curr +( yylsp_offset - 1); if( _temp2763 < _temp2761.base?
1: _temp2763 >= _temp2761.last_plus_one){ _throw( Null_Exception);}* _temp2763;}).first_line,({
struct _tagged_ptr3 _temp2764= yyls; struct Cyc_Yyltype* _temp2766= _temp2764.curr
+ yylsp_offset; if( _temp2766 < _temp2764.base? 1: _temp2766 >= _temp2764.last_plus_one){
_throw( Null_Exception);}* _temp2766;}).last_line))};( struct _xenum_struct*)
_temp2753;}); break; case 67: _LL2752: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp2768=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2768=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec(({ struct Cyc_Absyn_TupleType_struct* _temp2769=( struct Cyc_Absyn_TupleType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TupleType_struct)); _temp2769->tag= Cyc_Absyn_TupleType_tag;
_temp2769->f1=(( struct Cyc_List_List*(*)( struct _tuple9*(* f)( struct Cyc_Position_Segment*,
struct _tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x))
Cyc_List_map_c)( Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp2770= yyls; struct Cyc_Yyltype* _temp2772= _temp2770.curr +(
yylsp_offset - 1); if( _temp2772 < _temp2770.base? 1: _temp2772 >= _temp2770.last_plus_one){
_throw( Null_Exception);}* _temp2772;}).first_line,({ struct _tagged_ptr3
_temp2773= yyls; struct Cyc_Yyltype* _temp2775= _temp2773.curr +( yylsp_offset -
1); if( _temp2775 < _temp2773.base? 1: _temp2775 >= _temp2773.last_plus_one){
_throw( Null_Exception);}* _temp2775;}).last_line),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp2776= yyvs; struct _xenum_struct** _temp2778= _temp2776.curr
+( yyvsp_offset - 1); if( _temp2778 < _temp2776.base? 1: _temp2778 >= _temp2776.last_plus_one){
_throw( Null_Exception);}* _temp2778;}))));( void*) _temp2769;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2779= yyls; struct Cyc_Yyltype* _temp2781= _temp2779.curr
+( yylsp_offset - 3); if( _temp2781 < _temp2779.base? 1: _temp2781 >= _temp2779.last_plus_one){
_throw( Null_Exception);}* _temp2781;}).first_line,({ struct _tagged_ptr3
_temp2782= yyls; struct Cyc_Yyltype* _temp2784= _temp2782.curr + yylsp_offset;
if( _temp2784 < _temp2782.base? 1: _temp2784 >= _temp2782.last_plus_one){ _throw(
Null_Exception);}* _temp2784;}).last_line))};( struct _xenum_struct*) _temp2768;});
break; case 68: _LL2767: yyval=({ struct Cyc_Kind_tok_struct* _temp2786=( struct
Cyc_Kind_tok_struct*) GC_malloc( sizeof( struct Cyc_Kind_tok_struct));*
_temp2786=( struct Cyc_Kind_tok_struct){.tag= Cyc_Kind_tok_tag,.f1= Cyc_Parse_id_to_kind(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp2787= yyvs; struct
_xenum_struct** _temp2789= _temp2787.curr + yyvsp_offset; if( _temp2789 <
_temp2787.base? 1: _temp2789 >= _temp2787.last_plus_one){ _throw( Null_Exception);}*
_temp2789;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2790=
yyls; struct Cyc_Yyltype* _temp2792= _temp2790.curr + yylsp_offset; if(
_temp2792 < _temp2790.base? 1: _temp2792 >= _temp2790.last_plus_one){ _throw(
Null_Exception);}* _temp2792;}).first_line,({ struct _tagged_ptr3 _temp2793=
yyls; struct Cyc_Yyltype* _temp2795= _temp2793.curr + yylsp_offset; if(
_temp2795 < _temp2793.base? 1: _temp2795 >= _temp2793.last_plus_one){ _throw(
Null_Exception);}* _temp2795;}).last_line))};( struct _xenum_struct*) _temp2786;});
break; case 69: _LL2785: { struct _tagged_string* _temp2802; void* _temp2804;
struct _tuple1 _temp2800=* Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp2797=
yyvs; struct _xenum_struct** _temp2799= _temp2797.curr + yyvsp_offset; if(
_temp2799 < _temp2797.base? 1: _temp2799 >= _temp2797.last_plus_one){ _throw(
Null_Exception);}* _temp2799;})); _LL2805: _temp2804= _temp2800.f1; goto _LL2803;
_LL2803: _temp2802= _temp2800.f2; goto _LL2801; _LL2801: if( _temp2804 != Cyc_Absyn_Loc_n){
Cyc_Parse_err(( struct _tagged_string)({ char* _temp2806=( char*)"bad kind in type specifier";
struct _tagged_string _temp2807; _temp2807.curr= _temp2806; _temp2807.base=
_temp2806; _temp2807.last_plus_one= _temp2806 + 27; _temp2807;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2808= yyls; struct Cyc_Yyltype* _temp2810= _temp2808.curr
+ yylsp_offset; if( _temp2810 < _temp2808.base? 1: _temp2810 >= _temp2808.last_plus_one){
_throw( Null_Exception);}* _temp2810;}).first_line,({ struct _tagged_ptr3
_temp2811= yyls; struct Cyc_Yyltype* _temp2813= _temp2811.curr + yylsp_offset;
if( _temp2813 < _temp2811.base? 1: _temp2813 >= _temp2811.last_plus_one){ _throw(
Null_Exception);}* _temp2813;}).last_line));} yyval=({ struct Cyc_Kind_tok_struct*
_temp2814=( struct Cyc_Kind_tok_struct*) GC_malloc( sizeof( struct Cyc_Kind_tok_struct));*
_temp2814=( struct Cyc_Kind_tok_struct){.tag= Cyc_Kind_tok_tag,.f1= Cyc_Parse_id_to_kind(*
_temp2802, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2815= yyls;
struct Cyc_Yyltype* _temp2817= _temp2815.curr + yylsp_offset; if( _temp2817 <
_temp2815.base? 1: _temp2817 >= _temp2815.last_plus_one){ _throw( Null_Exception);}*
_temp2817;}).first_line,({ struct _tagged_ptr3 _temp2818= yyls; struct Cyc_Yyltype*
_temp2820= _temp2818.curr + yylsp_offset; if( _temp2820 < _temp2818.base? 1:
_temp2820 >= _temp2818.last_plus_one){ _throw( Null_Exception);}* _temp2820;}).last_line))};(
struct _xenum_struct*) _temp2814;}); break;} case 70: _LL2796: yyval=({ struct
Cyc_TypeQual_tok_struct* _temp2822=( struct Cyc_TypeQual_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeQual_tok_struct));* _temp2822=( struct Cyc_TypeQual_tok_struct){.tag=
Cyc_TypeQual_tok_tag,.f1=({ struct Cyc_Absyn_Tqual* _temp2823=( struct Cyc_Absyn_Tqual*)
GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual)); _temp2823->q_const= 1;
_temp2823->q_volatile= 0; _temp2823->q_restrict= 0; _temp2823;})};( struct
_xenum_struct*) _temp2822;}); break; case 71: _LL2821: yyval=({ struct Cyc_TypeQual_tok_struct*
_temp2825=( struct Cyc_TypeQual_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct));*
_temp2825=( struct Cyc_TypeQual_tok_struct){.tag= Cyc_TypeQual_tok_tag,.f1=({
struct Cyc_Absyn_Tqual* _temp2826=( struct Cyc_Absyn_Tqual*) GC_malloc_atomic(
sizeof( struct Cyc_Absyn_Tqual)); _temp2826->q_const= 0; _temp2826->q_volatile=
1; _temp2826->q_restrict= 0; _temp2826;})};( struct _xenum_struct*) _temp2825;});
break; case 72: _LL2824: yyval=({ struct Cyc_TypeQual_tok_struct* _temp2828=(
struct Cyc_TypeQual_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct));*
_temp2828=( struct Cyc_TypeQual_tok_struct){.tag= Cyc_TypeQual_tok_tag,.f1=({
struct Cyc_Absyn_Tqual* _temp2829=( struct Cyc_Absyn_Tqual*) GC_malloc_atomic(
sizeof( struct Cyc_Absyn_Tqual)); _temp2829->q_const= 0; _temp2829->q_volatile=
0; _temp2829->q_restrict= 1; _temp2829;})};( struct _xenum_struct*) _temp2828;});
break; case 73: _LL2827: { struct Cyc_Absyn_Decl* d;{ void* _temp2834= Cyc_yyget_StructOrUnion_tok(({
struct _tagged_ptr2 _temp2831= yyvs; struct _xenum_struct** _temp2833= _temp2831.curr
+( yyvsp_offset - 3); if( _temp2833 < _temp2831.base? 1: _temp2833 >= _temp2831.last_plus_one){
_throw( Null_Exception);}* _temp2833;})); _LL2836: if( _temp2834 == Cyc_Parse_Struct_su){
goto _LL2837;} else{ goto _LL2838;} _LL2838: if( _temp2834 == Cyc_Parse_Union_su){
goto _LL2839;} else{ goto _LL2835;} _LL2837: d= Cyc_Absyn_struct_decl( Cyc_Absyn_Public,
0, 0,({ struct Cyc_Core_Opt* _temp2840=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp2840->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({
struct _tagged_ptr2 _temp2841= yyvs; struct _xenum_struct** _temp2843= _temp2841.curr
+( yyvsp_offset - 1); if( _temp2843 < _temp2841.base? 1: _temp2843 >= _temp2841.last_plus_one){
_throw( Null_Exception);}* _temp2843;})); _temp2840;}), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2844= yyls; struct Cyc_Yyltype* _temp2846= _temp2844.curr
+( yylsp_offset - 3); if( _temp2846 < _temp2844.base? 1: _temp2846 >= _temp2844.last_plus_one){
_throw( Null_Exception);}* _temp2846;}).first_line,({ struct _tagged_ptr3
_temp2847= yyls; struct Cyc_Yyltype* _temp2849= _temp2847.curr + yylsp_offset;
if( _temp2849 < _temp2847.base? 1: _temp2849 >= _temp2847.last_plus_one){ _throw(
Null_Exception);}* _temp2849;}).last_line)); goto _LL2835; _LL2839: Cyc_Parse_unimp2((
struct _tagged_string)({ char* _temp2850=( char*)"unions"; struct _tagged_string
_temp2851; _temp2851.curr= _temp2850; _temp2851.base= _temp2850; _temp2851.last_plus_one=
_temp2850 + 7; _temp2851;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2852= yyls; struct Cyc_Yyltype* _temp2854= _temp2852.curr +( yylsp_offset -
3); if( _temp2854 < _temp2852.base? 1: _temp2854 >= _temp2852.last_plus_one){
_throw( Null_Exception);}* _temp2854;}).first_line,({ struct _tagged_ptr3
_temp2855= yyls; struct Cyc_Yyltype* _temp2857= _temp2855.curr + yylsp_offset;
if( _temp2857 < _temp2855.base? 1: _temp2857 >= _temp2855.last_plus_one){ _throw(
Null_Exception);}* _temp2857;}).last_line)); d= Cyc_Absyn_new_decl( Cyc_Absyn_Union_d,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2858= yyls; struct Cyc_Yyltype*
_temp2860= _temp2858.curr +( yylsp_offset - 3); if( _temp2860 < _temp2858.base?
1: _temp2860 >= _temp2858.last_plus_one){ _throw( Null_Exception);}* _temp2860;}).first_line,({
struct _tagged_ptr3 _temp2861= yyls; struct Cyc_Yyltype* _temp2863= _temp2861.curr
+ yylsp_offset; if( _temp2863 < _temp2861.base? 1: _temp2863 >= _temp2861.last_plus_one){
_throw( Null_Exception);}* _temp2863;}).last_line)); goto _LL2835; _LL2835:;}
yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2864=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));* _temp2864=( struct
Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=({ struct Cyc_Parse_Decl_spec_struct*
_temp2865=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2865->tag= Cyc_Parse_Decl_spec_tag; _temp2865->f1= d;( void*) _temp2865;})};(
struct _xenum_struct*) _temp2864;}); Cyc_Parse_unimp2(( struct _tagged_string)({
char* _temp2866=( char*)"anonymous structs/unions"; struct _tagged_string
_temp2867; _temp2867.curr= _temp2866; _temp2867.base= _temp2866; _temp2867.last_plus_one=
_temp2866 + 25; _temp2867;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2868= yyls; struct Cyc_Yyltype* _temp2870= _temp2868.curr +( yylsp_offset -
3); if( _temp2870 < _temp2868.base? 1: _temp2870 >= _temp2868.last_plus_one){
_throw( Null_Exception);}* _temp2870;}).first_line,({ struct _tagged_ptr3
_temp2871= yyls; struct Cyc_Yyltype* _temp2873= _temp2871.curr + yylsp_offset;
if( _temp2873 < _temp2871.base? 1: _temp2873 >= _temp2871.last_plus_one){ _throw(
Null_Exception);}* _temp2873;}).last_line)); break;} case 74: _LL2830: { struct
Cyc_List_List* ts=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2875= yyls; struct Cyc_Yyltype* _temp2877= _temp2875.curr
+( yylsp_offset - 3); if( _temp2877 < _temp2875.base? 1: _temp2877 >= _temp2875.last_plus_one){
_throw( Null_Exception);}* _temp2877;}).first_line,({ struct _tagged_ptr3
_temp2878= yyls; struct Cyc_Yyltype* _temp2880= _temp2878.curr +( yylsp_offset -
3); if( _temp2880 < _temp2878.base? 1: _temp2880 >= _temp2878.last_plus_one){
_throw( Null_Exception);}* _temp2880;}).last_line), Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp2881= yyvs; struct _xenum_struct** _temp2883= _temp2881.curr
+( yyvsp_offset - 3); if( _temp2883 < _temp2881.base? 1: _temp2883 >= _temp2881.last_plus_one){
_throw( Null_Exception);}* _temp2883;}))); struct Cyc_Absyn_Decl* d;{ void*
_temp2887= Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2 _temp2884= yyvs;
struct _xenum_struct** _temp2886= _temp2884.curr +( yyvsp_offset - 5); if(
_temp2886 < _temp2884.base? 1: _temp2886 >= _temp2884.last_plus_one){ _throw(
Null_Exception);}* _temp2886;})); _LL2889: if( _temp2887 == Cyc_Parse_Struct_su){
goto _LL2890;} else{ goto _LL2891;} _LL2891: if( _temp2887 == Cyc_Parse_Union_su){
goto _LL2892;} else{ goto _LL2888;} _LL2890: d= Cyc_Absyn_struct_decl( Cyc_Absyn_Public,({
struct Cyc_Core_Opt* _temp2893=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2893->v=( void*) Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp2894= yyvs; struct _xenum_struct** _temp2896= _temp2894.curr +(
yyvsp_offset - 4); if( _temp2896 < _temp2894.base? 1: _temp2896 >= _temp2894.last_plus_one){
_throw( Null_Exception);}* _temp2896;})); _temp2893;}), ts,({ struct Cyc_Core_Opt*
_temp2897=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2897->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({ struct _tagged_ptr2
_temp2898= yyvs; struct _xenum_struct** _temp2900= _temp2898.curr +(
yyvsp_offset - 1); if( _temp2900 < _temp2898.base? 1: _temp2900 >= _temp2898.last_plus_one){
_throw( Null_Exception);}* _temp2900;})); _temp2897;}), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2901= yyls; struct Cyc_Yyltype* _temp2903= _temp2901.curr
+( yylsp_offset - 5); if( _temp2903 < _temp2901.base? 1: _temp2903 >= _temp2901.last_plus_one){
_throw( Null_Exception);}* _temp2903;}).first_line,({ struct _tagged_ptr3
_temp2904= yyls; struct Cyc_Yyltype* _temp2906= _temp2904.curr + yylsp_offset;
if( _temp2906 < _temp2904.base? 1: _temp2906 >= _temp2904.last_plus_one){ _throw(
Null_Exception);}* _temp2906;}).last_line)); goto _LL2888; _LL2892: Cyc_Parse_unimp2((
struct _tagged_string)({ char* _temp2907=( char*)"unions"; struct _tagged_string
_temp2908; _temp2908.curr= _temp2907; _temp2908.base= _temp2907; _temp2908.last_plus_one=
_temp2907 + 7; _temp2908;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2909= yyls; struct Cyc_Yyltype* _temp2911= _temp2909.curr +( yylsp_offset -
5); if( _temp2911 < _temp2909.base? 1: _temp2911 >= _temp2909.last_plus_one){
_throw( Null_Exception);}* _temp2911;}).first_line,({ struct _tagged_ptr3
_temp2912= yyls; struct Cyc_Yyltype* _temp2914= _temp2912.curr + yylsp_offset;
if( _temp2914 < _temp2912.base? 1: _temp2914 >= _temp2912.last_plus_one){ _throw(
Null_Exception);}* _temp2914;}).last_line)); d= Cyc_Absyn_new_decl( Cyc_Absyn_Union_d,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2915= yyls; struct Cyc_Yyltype*
_temp2917= _temp2915.curr +( yylsp_offset - 5); if( _temp2917 < _temp2915.base?
1: _temp2917 >= _temp2915.last_plus_one){ _throw( Null_Exception);}* _temp2917;}).first_line,({
struct _tagged_ptr3 _temp2918= yyls; struct Cyc_Yyltype* _temp2920= _temp2918.curr
+ yylsp_offset; if( _temp2920 < _temp2918.base? 1: _temp2920 >= _temp2918.last_plus_one){
_throw( Null_Exception);}* _temp2920;}).last_line)); goto _LL2888; _LL2888:;}
yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2921=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));* _temp2921=( struct
Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=({ struct Cyc_Parse_Decl_spec_struct*
_temp2922=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2922->tag= Cyc_Parse_Decl_spec_tag; _temp2922->f1= d;( void*) _temp2922;})};(
struct _xenum_struct*) _temp2921;}); break;} case 75: _LL2874: { struct Cyc_List_List*
ts=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2924=
yyls; struct Cyc_Yyltype* _temp2926= _temp2924.curr +( yylsp_offset - 3); if(
_temp2926 < _temp2924.base? 1: _temp2926 >= _temp2924.last_plus_one){ _throw(
Null_Exception);}* _temp2926;}).first_line,({ struct _tagged_ptr3 _temp2927=
yyls; struct Cyc_Yyltype* _temp2929= _temp2927.curr +( yylsp_offset - 3); if(
_temp2929 < _temp2927.base? 1: _temp2929 >= _temp2927.last_plus_one){ _throw(
Null_Exception);}* _temp2929;}).last_line), Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp2930= yyvs; struct _xenum_struct** _temp2932= _temp2930.curr +(
yyvsp_offset - 3); if( _temp2932 < _temp2930.base? 1: _temp2932 >= _temp2930.last_plus_one){
_throw( Null_Exception);}* _temp2932;}))); struct Cyc_Absyn_Decl* d;{ void*
_temp2936= Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2 _temp2933= yyvs;
struct _xenum_struct** _temp2935= _temp2933.curr +( yyvsp_offset - 5); if(
_temp2935 < _temp2933.base? 1: _temp2935 >= _temp2933.last_plus_one){ _throw(
Null_Exception);}* _temp2935;})); _LL2938: if( _temp2936 == Cyc_Parse_Struct_su){
goto _LL2939;} else{ goto _LL2940;} _LL2940: if( _temp2936 == Cyc_Parse_Union_su){
goto _LL2941;} else{ goto _LL2937;} _LL2939: d= Cyc_Absyn_struct_decl( Cyc_Absyn_Public,({
struct Cyc_Core_Opt* _temp2942=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp2942->v=( void*) Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp2943= yyvs; struct _xenum_struct** _temp2945= _temp2943.curr +(
yyvsp_offset - 4); if( _temp2945 < _temp2943.base? 1: _temp2945 >= _temp2943.last_plus_one){
_throw( Null_Exception);}* _temp2945;})); _temp2942;}), ts,({ struct Cyc_Core_Opt*
_temp2946=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp2946->v=( void*) Cyc_yyget_StructFieldDeclList_tok(({ struct _tagged_ptr2
_temp2947= yyvs; struct _xenum_struct** _temp2949= _temp2947.curr +(
yyvsp_offset - 1); if( _temp2949 < _temp2947.base? 1: _temp2949 >= _temp2947.last_plus_one){
_throw( Null_Exception);}* _temp2949;})); _temp2946;}), 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp2950= yyls; struct Cyc_Yyltype* _temp2952= _temp2950.curr
+( yylsp_offset - 5); if( _temp2952 < _temp2950.base? 1: _temp2952 >= _temp2950.last_plus_one){
_throw( Null_Exception);}* _temp2952;}).first_line,({ struct _tagged_ptr3
_temp2953= yyls; struct Cyc_Yyltype* _temp2955= _temp2953.curr + yylsp_offset;
if( _temp2955 < _temp2953.base? 1: _temp2955 >= _temp2953.last_plus_one){ _throw(
Null_Exception);}* _temp2955;}).last_line)); goto _LL2937; _LL2941: Cyc_Parse_unimp2((
struct _tagged_string)({ char* _temp2956=( char*)"unions"; struct _tagged_string
_temp2957; _temp2957.curr= _temp2956; _temp2957.base= _temp2956; _temp2957.last_plus_one=
_temp2956 + 7; _temp2957;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp2958= yyls; struct Cyc_Yyltype* _temp2960= _temp2958.curr +( yylsp_offset -
5); if( _temp2960 < _temp2958.base? 1: _temp2960 >= _temp2958.last_plus_one){
_throw( Null_Exception);}* _temp2960;}).first_line,({ struct _tagged_ptr3
_temp2961= yyls; struct Cyc_Yyltype* _temp2963= _temp2961.curr + yylsp_offset;
if( _temp2963 < _temp2961.base? 1: _temp2963 >= _temp2961.last_plus_one){ _throw(
Null_Exception);}* _temp2963;}).last_line)); d= Cyc_Absyn_new_decl( Cyc_Absyn_Union_d,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2964= yyls; struct Cyc_Yyltype*
_temp2966= _temp2964.curr +( yylsp_offset - 5); if( _temp2966 < _temp2964.base?
1: _temp2966 >= _temp2964.last_plus_one){ _throw( Null_Exception);}* _temp2966;}).first_line,({
struct _tagged_ptr3 _temp2967= yyls; struct Cyc_Yyltype* _temp2969= _temp2967.curr
+ yylsp_offset; if( _temp2969 < _temp2967.base? 1: _temp2969 >= _temp2967.last_plus_one){
_throw( Null_Exception);}* _temp2969;}).last_line)); goto _LL2937; _LL2937:;}
yyval=({ struct Cyc_TypeSpecifier_tok_struct* _temp2970=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));* _temp2970=( struct
Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=({ struct Cyc_Parse_Decl_spec_struct*
_temp2971=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp2971->tag= Cyc_Parse_Decl_spec_tag; _temp2971->f1= d;( void*) _temp2971;})};(
struct _xenum_struct*) _temp2970;}); break;} case 76: _LL2923:{ void* _temp2976=
Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2 _temp2973= yyvs; struct
_xenum_struct** _temp2975= _temp2973.curr +( yyvsp_offset - 2); if( _temp2975 <
_temp2973.base? 1: _temp2975 >= _temp2973.last_plus_one){ _throw( Null_Exception);}*
_temp2975;})); _LL2978: if( _temp2976 == Cyc_Parse_Struct_su){ goto _LL2979;}
else{ goto _LL2980;} _LL2980: if( _temp2976 == Cyc_Parse_Union_su){ goto _LL2981;}
else{ goto _LL2977;} _LL2979: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp2982=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp2982=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec(({ struct Cyc_Absyn_StructType_struct* _temp2983=( struct
Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp2983->tag= Cyc_Absyn_StructType_tag; _temp2983->f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp2984= yyvs; struct _xenum_struct** _temp2986= _temp2984.curr
+( yyvsp_offset - 1); if( _temp2986 < _temp2984.base? 1: _temp2986 >= _temp2984.last_plus_one){
_throw( Null_Exception);}* _temp2986;})); _temp2983->f2= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp2987= yyvs; struct _xenum_struct** _temp2989= _temp2987.curr
+ yyvsp_offset; if( _temp2989 < _temp2987.base? 1: _temp2989 >= _temp2987.last_plus_one){
_throw( Null_Exception);}* _temp2989;})); _temp2983->f3= 0;( void*) _temp2983;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2990= yyls; struct Cyc_Yyltype*
_temp2992= _temp2990.curr +( yylsp_offset - 2); if( _temp2992 < _temp2990.base?
1: _temp2992 >= _temp2990.last_plus_one){ _throw( Null_Exception);}* _temp2992;}).first_line,({
struct _tagged_ptr3 _temp2993= yyls; struct Cyc_Yyltype* _temp2995= _temp2993.curr
+ yylsp_offset; if( _temp2995 < _temp2993.base? 1: _temp2995 >= _temp2993.last_plus_one){
_throw( Null_Exception);}* _temp2995;}).last_line))};( struct _xenum_struct*)
_temp2982;}); goto _LL2977; _LL2981: Cyc_Parse_unimp2(( struct _tagged_string)({
char* _temp2996=( char*)"unions"; struct _tagged_string _temp2997; _temp2997.curr=
_temp2996; _temp2997.base= _temp2996; _temp2997.last_plus_one= _temp2996 + 7;
_temp2997;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp2998= yyls;
struct Cyc_Yyltype* _temp3000= _temp2998.curr +( yylsp_offset - 2); if(
_temp3000 < _temp2998.base? 1: _temp3000 >= _temp2998.last_plus_one){ _throw(
Null_Exception);}* _temp3000;}).first_line,({ struct _tagged_ptr3 _temp3001=
yyls; struct Cyc_Yyltype* _temp3003= _temp3001.curr + yylsp_offset; if(
_temp3003 < _temp3001.base? 1: _temp3003 >= _temp3001.last_plus_one){ _throw(
Null_Exception);}* _temp3003;}).last_line)); yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp3004=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp3004=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=({
struct Cyc_Parse_Decl_spec_struct* _temp3005=( struct Cyc_Parse_Decl_spec_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct)); _temp3005->tag= Cyc_Parse_Decl_spec_tag;
_temp3005->f1= Cyc_Absyn_new_decl( Cyc_Absyn_Union_d, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3006= yyls; struct Cyc_Yyltype* _temp3008= _temp3006.curr
+( yylsp_offset - 2); if( _temp3008 < _temp3006.base? 1: _temp3008 >= _temp3006.last_plus_one){
_throw( Null_Exception);}* _temp3008;}).first_line,({ struct _tagged_ptr3
_temp3009= yyls; struct Cyc_Yyltype* _temp3011= _temp3009.curr + yylsp_offset;
if( _temp3011 < _temp3009.base? 1: _temp3011 >= _temp3009.last_plus_one){ _throw(
Null_Exception);}* _temp3011;}).last_line));( void*) _temp3005;})};( struct
_xenum_struct*) _temp3004;}); goto _LL2977; _LL2977:;} break; case 77: _LL2972:{
void* _temp3016= Cyc_yyget_StructOrUnion_tok(({ struct _tagged_ptr2 _temp3013=
yyvs; struct _xenum_struct** _temp3015= _temp3013.curr +( yyvsp_offset - 2); if(
_temp3015 < _temp3013.base? 1: _temp3015 >= _temp3013.last_plus_one){ _throw(
Null_Exception);}* _temp3015;})); _LL3018: if( _temp3016 == Cyc_Parse_Struct_su){
goto _LL3019;} else{ goto _LL3020;} _LL3020: if( _temp3016 == Cyc_Parse_Union_su){
goto _LL3021;} else{ goto _LL3017;} _LL3019: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp3022=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp3022=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec(({ struct Cyc_Absyn_StructType_struct* _temp3023=( struct
Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp3023->tag= Cyc_Absyn_StructType_tag; _temp3023->f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3024= yyvs; struct _xenum_struct** _temp3026= _temp3024.curr
+( yyvsp_offset - 1); if( _temp3026 < _temp3024.base? 1: _temp3026 >= _temp3024.last_plus_one){
_throw( Null_Exception);}* _temp3026;})); _temp3023->f2= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3027= yyvs; struct _xenum_struct** _temp3029= _temp3027.curr
+ yyvsp_offset; if( _temp3029 < _temp3027.base? 1: _temp3029 >= _temp3027.last_plus_one){
_throw( Null_Exception);}* _temp3029;})); _temp3023->f3= 0;( void*) _temp3023;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3030= yyls; struct Cyc_Yyltype*
_temp3032= _temp3030.curr +( yylsp_offset - 2); if( _temp3032 < _temp3030.base?
1: _temp3032 >= _temp3030.last_plus_one){ _throw( Null_Exception);}* _temp3032;}).first_line,({
struct _tagged_ptr3 _temp3033= yyls; struct Cyc_Yyltype* _temp3035= _temp3033.curr
+ yylsp_offset; if( _temp3035 < _temp3033.base? 1: _temp3035 >= _temp3033.last_plus_one){
_throw( Null_Exception);}* _temp3035;}).last_line))};( struct _xenum_struct*)
_temp3022;}); goto _LL3017; _LL3021: Cyc_Parse_unimp2(( struct _tagged_string)({
char* _temp3036=( char*)"unions"; struct _tagged_string _temp3037; _temp3037.curr=
_temp3036; _temp3037.base= _temp3036; _temp3037.last_plus_one= _temp3036 + 7;
_temp3037;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3038= yyls;
struct Cyc_Yyltype* _temp3040= _temp3038.curr +( yylsp_offset - 2); if(
_temp3040 < _temp3038.base? 1: _temp3040 >= _temp3038.last_plus_one){ _throw(
Null_Exception);}* _temp3040;}).first_line,({ struct _tagged_ptr3 _temp3041=
yyls; struct Cyc_Yyltype* _temp3043= _temp3041.curr + yylsp_offset; if(
_temp3043 < _temp3041.base? 1: _temp3043 >= _temp3041.last_plus_one){ _throw(
Null_Exception);}* _temp3043;}).last_line)); yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp3044=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp3044=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=({
struct Cyc_Parse_Decl_spec_struct* _temp3045=( struct Cyc_Parse_Decl_spec_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct)); _temp3045->tag= Cyc_Parse_Decl_spec_tag;
_temp3045->f1= Cyc_Absyn_new_decl( Cyc_Absyn_Union_d, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3046= yyls; struct Cyc_Yyltype* _temp3048= _temp3046.curr
+( yylsp_offset - 2); if( _temp3048 < _temp3046.base? 1: _temp3048 >= _temp3046.last_plus_one){
_throw( Null_Exception);}* _temp3048;}).first_line,({ struct _tagged_ptr3
_temp3049= yyls; struct Cyc_Yyltype* _temp3051= _temp3049.curr + yylsp_offset;
if( _temp3051 < _temp3049.base? 1: _temp3051 >= _temp3049.last_plus_one){ _throw(
Null_Exception);}* _temp3051;}).last_line));( void*) _temp3045;})};( struct
_xenum_struct*) _temp3044;}); goto _LL3017; _LL3017:;} break; case 78: _LL3012:
yyval=({ struct Cyc_TypeList_tok_struct* _temp3053=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));* _temp3053=( struct Cyc_TypeList_tok_struct){.tag=
Cyc_TypeList_tok_tag,.f1= 0};( struct _xenum_struct*) _temp3053;}); break; case
79: _LL3052: yyval=({ struct Cyc_TypeList_tok_struct* _temp3055=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));* _temp3055=( struct Cyc_TypeList_tok_struct){.tag=
Cyc_TypeList_tok_tag,.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3056= yyvs;
struct _xenum_struct** _temp3058= _temp3056.curr +( yyvsp_offset - 1); if(
_temp3058 < _temp3056.base? 1: _temp3058 >= _temp3056.last_plus_one){ _throw(
Null_Exception);}* _temp3058;})))};( struct _xenum_struct*) _temp3055;}); break;
case 80: _LL3054:(( struct Cyc_Lexing_lexbuf*) Cyc_Parse_lbuf->v)->lex_curr_pos
-= 1; yyval=({ struct Cyc_TypeList_tok_struct* _temp3060=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));* _temp3060=( struct Cyc_TypeList_tok_struct){.tag=
Cyc_TypeList_tok_tag,.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp3061= yyvs;
struct _xenum_struct** _temp3063= _temp3061.curr +( yyvsp_offset - 1); if(
_temp3063 < _temp3061.base? 1: _temp3063 >= _temp3061.last_plus_one){ _throw(
Null_Exception);}* _temp3063;})))};( struct _xenum_struct*) _temp3060;}); break;
case 81: _LL3059: yyval=({ struct Cyc_StructOrUnion_tok_struct* _temp3065=(
struct Cyc_StructOrUnion_tok_struct*) GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));*
_temp3065=( struct Cyc_StructOrUnion_tok_struct){.tag= Cyc_StructOrUnion_tok_tag,.f1=
Cyc_Parse_Struct_su};( struct _xenum_struct*) _temp3065;}); break; case 82:
_LL3064: yyval=({ struct Cyc_StructOrUnion_tok_struct* _temp3067=( struct Cyc_StructOrUnion_tok_struct*)
GC_malloc( sizeof( struct Cyc_StructOrUnion_tok_struct));* _temp3067=( struct
Cyc_StructOrUnion_tok_struct){.tag= Cyc_StructOrUnion_tok_tag,.f1= Cyc_Parse_Union_su};(
struct _xenum_struct*) _temp3067;}); break; case 83: _LL3066: yyval=({ struct
Cyc_StructFieldDeclList_tok_struct* _temp3069=( struct Cyc_StructFieldDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_StructFieldDeclList_tok_struct));* _temp3069=(
struct Cyc_StructFieldDeclList_tok_struct){.tag= Cyc_StructFieldDeclList_tok_tag,.f1=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_flatten)((( struct
Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_StructFieldDeclListList_tok(({
struct _tagged_ptr2 _temp3070= yyvs; struct _xenum_struct** _temp3072= _temp3070.curr
+ yyvsp_offset; if( _temp3072 < _temp3070.base? 1: _temp3072 >= _temp3070.last_plus_one){
_throw( Null_Exception);}* _temp3072;}))))};( struct _xenum_struct*) _temp3069;});
break; case 84: _LL3068: yyval=({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp3074=( struct Cyc_StructFieldDeclListList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct));* _temp3074=( struct Cyc_StructFieldDeclListList_tok_struct){.tag=
Cyc_StructFieldDeclListList_tok_tag,.f1=({ struct Cyc_List_List* _temp3075=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3075->hd=(
void*) Cyc_yyget_StructFieldDeclList_tok(({ struct _tagged_ptr2 _temp3076= yyvs;
struct _xenum_struct** _temp3078= _temp3076.curr + yyvsp_offset; if( _temp3078 <
_temp3076.base? 1: _temp3078 >= _temp3076.last_plus_one){ _throw( Null_Exception);}*
_temp3078;})); _temp3075->tl= 0; _temp3075;})};( struct _xenum_struct*)
_temp3074;}); break; case 85: _LL3073: yyval=({ struct Cyc_StructFieldDeclListList_tok_struct*
_temp3080=( struct Cyc_StructFieldDeclListList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclListList_tok_struct));* _temp3080=( struct Cyc_StructFieldDeclListList_tok_struct){.tag=
Cyc_StructFieldDeclListList_tok_tag,.f1=({ struct Cyc_List_List* _temp3081=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3081->hd=(
void*) Cyc_yyget_StructFieldDeclList_tok(({ struct _tagged_ptr2 _temp3085= yyvs;
struct _xenum_struct** _temp3087= _temp3085.curr + yyvsp_offset; if( _temp3087 <
_temp3085.base? 1: _temp3087 >= _temp3085.last_plus_one){ _throw( Null_Exception);}*
_temp3087;})); _temp3081->tl= Cyc_yyget_StructFieldDeclListList_tok(({ struct
_tagged_ptr2 _temp3082= yyvs; struct _xenum_struct** _temp3084= _temp3082.curr +(
yyvsp_offset - 1); if( _temp3084 < _temp3082.base? 1: _temp3084 >= _temp3082.last_plus_one){
_throw( Null_Exception);}* _temp3084;})); _temp3081;})};( struct _xenum_struct*)
_temp3080;}); break; case 86: _LL3079: yyval=({ struct Cyc_InitDeclList_tok_struct*
_temp3089=( struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));*
_temp3089=( struct Cyc_InitDeclList_tok_struct){.tag= Cyc_InitDeclList_tok_tag,.f1=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitDeclList_tok(({
struct _tagged_ptr2 _temp3090= yyvs; struct _xenum_struct** _temp3092= _temp3090.curr
+ yyvsp_offset; if( _temp3092 < _temp3090.base? 1: _temp3092 >= _temp3090.last_plus_one){
_throw( Null_Exception);}* _temp3092;})))};( struct _xenum_struct*) _temp3089;});
break; case 87: _LL3088: yyval=({ struct Cyc_InitDeclList_tok_struct* _temp3094=(
struct Cyc_InitDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));*
_temp3094=( struct Cyc_InitDeclList_tok_struct){.tag= Cyc_InitDeclList_tok_tag,.f1=({
struct Cyc_List_List* _temp3095=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3095->hd=( void*) Cyc_yyget_InitDecl_tok(({ struct
_tagged_ptr2 _temp3096= yyvs; struct _xenum_struct** _temp3098= _temp3096.curr +
yyvsp_offset; if( _temp3098 < _temp3096.base? 1: _temp3098 >= _temp3096.last_plus_one){
_throw( Null_Exception);}* _temp3098;})); _temp3095->tl= 0; _temp3095;})};(
struct _xenum_struct*) _temp3094;}); break; case 88: _LL3093: yyval=({ struct
Cyc_InitDeclList_tok_struct* _temp3100=( struct Cyc_InitDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitDeclList_tok_struct));* _temp3100=( struct Cyc_InitDeclList_tok_struct){.tag=
Cyc_InitDeclList_tok_tag,.f1=({ struct Cyc_List_List* _temp3101=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3101->hd=( void*) Cyc_yyget_InitDecl_tok(({
struct _tagged_ptr2 _temp3105= yyvs; struct _xenum_struct** _temp3107= _temp3105.curr
+ yyvsp_offset; if( _temp3107 < _temp3105.base? 1: _temp3107 >= _temp3105.last_plus_one){
_throw( Null_Exception);}* _temp3107;})); _temp3101->tl= Cyc_yyget_InitDeclList_tok(({
struct _tagged_ptr2 _temp3102= yyvs; struct _xenum_struct** _temp3104= _temp3102.curr
+( yyvsp_offset - 2); if( _temp3104 < _temp3102.base? 1: _temp3104 >= _temp3102.last_plus_one){
_throw( Null_Exception);}* _temp3104;})); _temp3101;})};( struct _xenum_struct*)
_temp3100;}); break; case 89: _LL3099: yyval=({ struct Cyc_InitDecl_tok_struct*
_temp3109=( struct Cyc_InitDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct));*
_temp3109=( struct Cyc_InitDecl_tok_struct){.tag= Cyc_InitDecl_tok_tag,.f1=({
struct _tuple12* _temp3110=( struct _tuple12*) GC_malloc( sizeof( struct
_tuple12)); _temp3110->f1= Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2
_temp3111= yyvs; struct _xenum_struct** _temp3113= _temp3111.curr + yyvsp_offset;
if( _temp3113 < _temp3111.base? 1: _temp3113 >= _temp3111.last_plus_one){ _throw(
Null_Exception);}* _temp3113;})); _temp3110->f2= 0; _temp3110;})};( struct
_xenum_struct*) _temp3109;}); break; case 90: _LL3108: yyval=({ struct Cyc_InitDecl_tok_struct*
_temp3115=( struct Cyc_InitDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_InitDecl_tok_struct));*
_temp3115=( struct Cyc_InitDecl_tok_struct){.tag= Cyc_InitDecl_tok_tag,.f1=({
struct _tuple12* _temp3116=( struct _tuple12*) GC_malloc( sizeof( struct
_tuple12)); _temp3116->f1= Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2
_temp3120= yyvs; struct _xenum_struct** _temp3122= _temp3120.curr +(
yyvsp_offset - 2); if( _temp3122 < _temp3120.base? 1: _temp3122 >= _temp3120.last_plus_one){
_throw( Null_Exception);}* _temp3122;})); _temp3116->f2=( struct Cyc_Absyn_Exp*)
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp3117= yyvs; struct _xenum_struct**
_temp3119= _temp3117.curr + yyvsp_offset; if( _temp3119 < _temp3117.base? 1:
_temp3119 >= _temp3117.last_plus_one){ _throw( Null_Exception);}* _temp3119;}));
_temp3116;})};( struct _xenum_struct*) _temp3115;}); break; case 91: _LL3114: {
struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp3124= yyvs; struct _xenum_struct** _temp3126= _temp3124.curr +(
yyvsp_offset - 2); if( _temp3126 < _temp3124.base? 1: _temp3126 >= _temp3124.last_plus_one){
_throw( Null_Exception);}* _temp3126;}))).f1; struct Cyc_List_List* tss=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3127= yyvs; struct _xenum_struct** _temp3129= _temp3127.curr
+( yyvsp_offset - 2); if( _temp3129 < _temp3127.base? 1: _temp3129 >= _temp3127.last_plus_one){
_throw( Null_Exception);}* _temp3129;}))).f2; struct _tuple5 ts_info= Cyc_Parse_collapse_type_specifiers(
tss, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3130= yyls; struct
Cyc_Yyltype* _temp3132= _temp3130.curr +( yylsp_offset - 2); if( _temp3132 <
_temp3130.base? 1: _temp3132 >= _temp3130.last_plus_one){ _throw( Null_Exception);}*
_temp3132;}).first_line,({ struct _tagged_ptr3 _temp3133= yyls; struct Cyc_Yyltype*
_temp3135= _temp3133.curr +( yylsp_offset - 2); if( _temp3135 < _temp3133.base?
1: _temp3135 >= _temp3133.last_plus_one){ _throw( Null_Exception);}* _temp3135;}).last_line));
if( ts_info.f2 != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp3136=(
char*)"struct declaration contains nested type declaration"; struct
_tagged_string _temp3137; _temp3137.curr= _temp3136; _temp3137.base= _temp3136;
_temp3137.last_plus_one= _temp3136 + 52; _temp3137;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3138= yyls; struct Cyc_Yyltype* _temp3140= _temp3138.curr
+( yylsp_offset - 2); if( _temp3140 < _temp3138.base? 1: _temp3140 >= _temp3138.last_plus_one){
_throw( Null_Exception);}* _temp3140;}).first_line,({ struct _tagged_ptr3
_temp3141= yyls; struct Cyc_Yyltype* _temp3143= _temp3141.curr +( yylsp_offset -
2); if( _temp3143 < _temp3141.base? 1: _temp3143 >= _temp3141.last_plus_one){
_throw( Null_Exception);}* _temp3143;}).last_line));}{ void* t= ts_info.f1;
struct Cyc_List_List* info= Cyc_Parse_apply_tmss( tq, t, Cyc_yyget_DeclaratorList_tok(({
struct _tagged_ptr2 _temp3144= yyvs; struct _xenum_struct** _temp3146= _temp3144.curr
+( yyvsp_offset - 1); if( _temp3146 < _temp3144.base? 1: _temp3146 >= _temp3144.last_plus_one){
_throw( Null_Exception);}* _temp3146;})), 0); yyval=({ struct Cyc_StructFieldDeclList_tok_struct*
_temp3147=( struct Cyc_StructFieldDeclList_tok_struct*) GC_malloc( sizeof(
struct Cyc_StructFieldDeclList_tok_struct));* _temp3147=( struct Cyc_StructFieldDeclList_tok_struct){.tag=
Cyc_StructFieldDeclList_tok_tag,.f1=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Structfield*(*
f)( struct Cyc_Position_Segment*, struct _tuple7*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_make_struct_field, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3148= yyls; struct Cyc_Yyltype* _temp3150= _temp3148.curr
+( yylsp_offset - 2); if( _temp3150 < _temp3148.base? 1: _temp3150 >= _temp3148.last_plus_one){
_throw( Null_Exception);}* _temp3150;}).first_line,({ struct _tagged_ptr3
_temp3151= yyls; struct Cyc_Yyltype* _temp3153= _temp3151.curr +( yylsp_offset -
1); if( _temp3153 < _temp3151.base? 1: _temp3153 >= _temp3151.last_plus_one){
_throw( Null_Exception);}* _temp3153;}).last_line), info)};( struct
_xenum_struct*) _temp3147;}); break;}} case 92: _LL3123: yyval=({ struct Cyc_QualSpecList_tok_struct*
_temp3155=( struct Cyc_QualSpecList_tok_struct*) GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));*
_temp3155=( struct Cyc_QualSpecList_tok_struct){.tag= Cyc_QualSpecList_tok_tag,.f1=({
struct _tuple13* _temp3156=( struct _tuple13*) GC_malloc( sizeof( struct
_tuple13)); _temp3156->f1= Cyc_Absyn_empty_tqual(); _temp3156->f2=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp3157=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3157->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({
struct _tagged_ptr2 _temp3158= yyvs; struct _xenum_struct** _temp3160= _temp3158.curr
+ yyvsp_offset; if( _temp3160 < _temp3158.base? 1: _temp3160 >= _temp3158.last_plus_one){
_throw( Null_Exception);}* _temp3160;})); _temp3157->tl= 0; _temp3157;});
_temp3156;})};( struct _xenum_struct*) _temp3155;}); break; case 93: _LL3154:
yyval=({ struct Cyc_QualSpecList_tok_struct* _temp3162=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));* _temp3162=( struct Cyc_QualSpecList_tok_struct){.tag=
Cyc_QualSpecList_tok_tag,.f1=({ struct _tuple13* _temp3163=( struct _tuple13*)
GC_malloc( sizeof( struct _tuple13)); _temp3163->f1=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3171= yyvs; struct _xenum_struct** _temp3173= _temp3171.curr
+ yyvsp_offset; if( _temp3173 < _temp3171.base? 1: _temp3173 >= _temp3171.last_plus_one){
_throw( Null_Exception);}* _temp3173;}))).f1; _temp3163->f2=( struct Cyc_List_List*)({
struct Cyc_List_List* _temp3164=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3164->hd=( void*) Cyc_yyget_TypeSpecifier_tok(({
struct _tagged_ptr2 _temp3168= yyvs; struct _xenum_struct** _temp3170= _temp3168.curr
+( yyvsp_offset - 1); if( _temp3170 < _temp3168.base? 1: _temp3170 >= _temp3168.last_plus_one){
_throw( Null_Exception);}* _temp3170;})); _temp3164->tl=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3165= yyvs; struct _xenum_struct** _temp3167= _temp3165.curr
+ yyvsp_offset; if( _temp3167 < _temp3165.base? 1: _temp3167 >= _temp3165.last_plus_one){
_throw( Null_Exception);}* _temp3167;}))).f2; _temp3164;}); _temp3163;})};(
struct _xenum_struct*) _temp3162;}); break; case 94: _LL3161: yyval=({ struct
Cyc_QualSpecList_tok_struct* _temp3175=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));* _temp3175=( struct Cyc_QualSpecList_tok_struct){.tag=
Cyc_QualSpecList_tok_tag,.f1=({ struct _tuple13* _temp3176=( struct _tuple13*)
GC_malloc( sizeof( struct _tuple13)); _temp3176->f1= Cyc_yyget_TypeQual_tok(({
struct _tagged_ptr2 _temp3177= yyvs; struct _xenum_struct** _temp3179= _temp3177.curr
+ yyvsp_offset; if( _temp3179 < _temp3177.base? 1: _temp3179 >= _temp3177.last_plus_one){
_throw( Null_Exception);}* _temp3179;})); _temp3176->f2= 0; _temp3176;})};(
struct _xenum_struct*) _temp3175;}); break; case 95: _LL3174: yyval=({ struct
Cyc_QualSpecList_tok_struct* _temp3181=( struct Cyc_QualSpecList_tok_struct*)
GC_malloc( sizeof( struct Cyc_QualSpecList_tok_struct));* _temp3181=( struct Cyc_QualSpecList_tok_struct){.tag=
Cyc_QualSpecList_tok_tag,.f1=({ struct _tuple13* _temp3182=( struct _tuple13*)
GC_malloc( sizeof( struct _tuple13)); _temp3182->f1= Cyc_Absyn_combine_tqual(
Cyc_yyget_TypeQual_tok(({ struct _tagged_ptr2 _temp3186= yyvs; struct
_xenum_struct** _temp3188= _temp3186.curr +( yyvsp_offset - 1); if( _temp3188 <
_temp3186.base? 1: _temp3188 >= _temp3186.last_plus_one){ _throw( Null_Exception);}*
_temp3188;})),(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp3189=
yyvs; struct _xenum_struct** _temp3191= _temp3189.curr + yyvsp_offset; if(
_temp3191 < _temp3189.base? 1: _temp3191 >= _temp3189.last_plus_one){ _throw(
Null_Exception);}* _temp3191;}))).f1); _temp3182->f2=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3183= yyvs; struct _xenum_struct** _temp3185= _temp3183.curr
+ yyvsp_offset; if( _temp3185 < _temp3183.base? 1: _temp3185 >= _temp3183.last_plus_one){
_throw( Null_Exception);}* _temp3185;}))).f2; _temp3182;})};( struct
_xenum_struct*) _temp3181;}); break; case 96: _LL3180: yyval=({ struct Cyc_DeclaratorList_tok_struct*
_temp3193=( struct Cyc_DeclaratorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorList_tok_struct));*
_temp3193=( struct Cyc_DeclaratorList_tok_struct){.tag= Cyc_DeclaratorList_tok_tag,.f1=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_DeclaratorList_tok(({
struct _tagged_ptr2 _temp3194= yyvs; struct _xenum_struct** _temp3196= _temp3194.curr
+ yyvsp_offset; if( _temp3196 < _temp3194.base? 1: _temp3196 >= _temp3194.last_plus_one){
_throw( Null_Exception);}* _temp3196;})))};( struct _xenum_struct*) _temp3193;});
break; case 97: _LL3192: yyval=({ struct Cyc_DeclaratorList_tok_struct*
_temp3198=( struct Cyc_DeclaratorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DeclaratorList_tok_struct));*
_temp3198=( struct Cyc_DeclaratorList_tok_struct){.tag= Cyc_DeclaratorList_tok_tag,.f1=({
struct Cyc_List_List* _temp3199=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3199->hd=( void*) Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3200= yyvs; struct _xenum_struct** _temp3202= _temp3200.curr
+ yyvsp_offset; if( _temp3202 < _temp3200.base? 1: _temp3202 >= _temp3200.last_plus_one){
_throw( Null_Exception);}* _temp3202;})); _temp3199->tl= 0; _temp3199;})};(
struct _xenum_struct*) _temp3198;}); break; case 98: _LL3197: yyval=({ struct
Cyc_DeclaratorList_tok_struct* _temp3204=( struct Cyc_DeclaratorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DeclaratorList_tok_struct));* _temp3204=( struct
Cyc_DeclaratorList_tok_struct){.tag= Cyc_DeclaratorList_tok_tag,.f1=({ struct
Cyc_List_List* _temp3205=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3205->hd=( void*) Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3209=
yyvs; struct _xenum_struct** _temp3211= _temp3209.curr + yyvsp_offset; if(
_temp3211 < _temp3209.base? 1: _temp3211 >= _temp3209.last_plus_one){ _throw(
Null_Exception);}* _temp3211;})); _temp3205->tl= Cyc_yyget_DeclaratorList_tok(({
struct _tagged_ptr2 _temp3206= yyvs; struct _xenum_struct** _temp3208= _temp3206.curr
+( yyvsp_offset - 2); if( _temp3208 < _temp3206.base? 1: _temp3208 >= _temp3206.last_plus_one){
_throw( Null_Exception);}* _temp3208;})); _temp3205;})};( struct _xenum_struct*)
_temp3204;}); break; case 99: _LL3203: yyval=({ struct _tagged_ptr2 _temp3213=
yyvs; struct _xenum_struct** _temp3215= _temp3213.curr + yyvsp_offset; if(
_temp3215 < _temp3213.base? 1: _temp3215 >= _temp3213.last_plus_one){ _throw(
Null_Exception);}* _temp3215;}); break; case 100: _LL3212: Cyc_Parse_unimp2((
struct _tagged_string)({ char* _temp3217=( char*)"bit fields"; struct
_tagged_string _temp3218; _temp3218.curr= _temp3217; _temp3218.base= _temp3217;
_temp3218.last_plus_one= _temp3217 + 11; _temp3218;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3219= yyls; struct Cyc_Yyltype* _temp3221= _temp3219.curr
+( yylsp_offset - 1); if( _temp3221 < _temp3219.base? 1: _temp3221 >= _temp3219.last_plus_one){
_throw( Null_Exception);}* _temp3221;}).first_line,({ struct _tagged_ptr3
_temp3222= yyls; struct Cyc_Yyltype* _temp3224= _temp3222.curr + yylsp_offset;
if( _temp3224 < _temp3222.base? 1: _temp3224 >= _temp3222.last_plus_one){ _throw(
Null_Exception);}* _temp3224;}).last_line)); yyval=({ struct Cyc_Declarator_tok_struct*
_temp3225=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));*
_temp3225=( struct Cyc_Declarator_tok_struct){.tag= Cyc_Declarator_tok_tag,.f1=({
struct Cyc_Parse_Declarator* _temp3226=( struct Cyc_Parse_Declarator*) GC_malloc(
sizeof( struct Cyc_Parse_Declarator)); _temp3226->id=({ struct _tuple1*
_temp3227=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp3227->f1=({
struct Cyc_Absyn_Rel_n_struct* _temp3231=( struct Cyc_Absyn_Rel_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct)); _temp3231->tag= Cyc_Absyn_Rel_n_tag;
_temp3231->f1= 0;( void*) _temp3231;}); _temp3227->f2=({ struct _tagged_string*
_temp3228=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp3228[ 0]=( struct _tagged_string)({ char* _temp3229=( char*)""; struct
_tagged_string _temp3230; _temp3230.curr= _temp3229; _temp3230.base= _temp3229;
_temp3230.last_plus_one= _temp3229 + 1; _temp3230;}); _temp3228;}); _temp3227;});
_temp3226->tms= 0; _temp3226;})};( struct _xenum_struct*) _temp3225;}); break;
case 101: _LL3216: Cyc_Parse_unimp2(( struct _tagged_string)({ char* _temp3233=(
char*)"bit fields"; struct _tagged_string _temp3234; _temp3234.curr= _temp3233;
_temp3234.base= _temp3233; _temp3234.last_plus_one= _temp3233 + 11; _temp3234;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3235= yyls; struct Cyc_Yyltype*
_temp3237= _temp3235.curr +( yylsp_offset - 2); if( _temp3237 < _temp3235.base?
1: _temp3237 >= _temp3235.last_plus_one){ _throw( Null_Exception);}* _temp3237;}).first_line,({
struct _tagged_ptr3 _temp3238= yyls; struct Cyc_Yyltype* _temp3240= _temp3238.curr
+( yylsp_offset - 1); if( _temp3240 < _temp3238.base? 1: _temp3240 >= _temp3238.last_plus_one){
_throw( Null_Exception);}* _temp3240;}).last_line)); yyval=({ struct
_tagged_ptr2 _temp3241= yyvs; struct _xenum_struct** _temp3243= _temp3241.curr +(
yyvsp_offset - 2); if( _temp3243 < _temp3241.base? 1: _temp3243 >= _temp3241.last_plus_one){
_throw( Null_Exception);}* _temp3243;}); break; case 102: _LL3232: yyval=({
struct Cyc_TypeSpecifier_tok_struct* _temp3245=( struct Cyc_TypeSpecifier_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));* _temp3245=( struct
Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=({ struct Cyc_Parse_Decl_spec_struct*
_temp3246=( struct Cyc_Parse_Decl_spec_struct*) GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct));
_temp3246->tag= Cyc_Parse_Decl_spec_tag; _temp3246->f1= Cyc_Absyn_enum_decl( Cyc_Absyn_Public,
0, 0,({ struct Cyc_Core_Opt* _temp3247=( struct Cyc_Core_Opt*) GC_malloc(
sizeof( struct Cyc_Core_Opt)); _temp3247->v=( void*) Cyc_yyget_EnumeratorFieldList_tok(({
struct _tagged_ptr2 _temp3248= yyvs; struct _xenum_struct** _temp3250= _temp3248.curr
+( yyvsp_offset - 1); if( _temp3250 < _temp3248.base? 1: _temp3250 >= _temp3248.last_plus_one){
_throw( Null_Exception);}* _temp3250;})); _temp3247;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3251= yyls; struct Cyc_Yyltype* _temp3253= _temp3251.curr
+( yylsp_offset - 3); if( _temp3253 < _temp3251.base? 1: _temp3253 >= _temp3251.last_plus_one){
_throw( Null_Exception);}* _temp3253;}).first_line,({ struct _tagged_ptr3
_temp3254= yyls; struct Cyc_Yyltype* _temp3256= _temp3254.curr + yylsp_offset;
if( _temp3256 < _temp3254.base? 1: _temp3256 >= _temp3254.last_plus_one){ _throw(
Null_Exception);}* _temp3256;}).last_line));( void*) _temp3246;})};( struct
_xenum_struct*) _temp3245;}); Cyc_Parse_unimp2(( struct _tagged_string)({ char*
_temp3257=( char*)"anonymous enums"; struct _tagged_string _temp3258; _temp3258.curr=
_temp3257; _temp3258.base= _temp3257; _temp3258.last_plus_one= _temp3257 + 16;
_temp3258;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3259= yyls;
struct Cyc_Yyltype* _temp3261= _temp3259.curr +( yylsp_offset - 3); if(
_temp3261 < _temp3259.base? 1: _temp3261 >= _temp3259.last_plus_one){ _throw(
Null_Exception);}* _temp3261;}).first_line,({ struct _tagged_ptr3 _temp3262=
yyls; struct Cyc_Yyltype* _temp3264= _temp3262.curr + yylsp_offset; if(
_temp3264 < _temp3262.base? 1: _temp3264 >= _temp3262.last_plus_one){ _throw(
Null_Exception);}* _temp3264;}).last_line)); break; case 103: _LL3244: { struct
Cyc_List_List* ts=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)(
struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment* env, struct
Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3266= yyls; struct Cyc_Yyltype* _temp3268= _temp3266.curr
+( yylsp_offset - 3); if( _temp3268 < _temp3266.base? 1: _temp3268 >= _temp3266.last_plus_one){
_throw( Null_Exception);}* _temp3268;}).first_line,({ struct _tagged_ptr3
_temp3269= yyls; struct Cyc_Yyltype* _temp3271= _temp3269.curr +( yylsp_offset -
3); if( _temp3271 < _temp3269.base? 1: _temp3271 >= _temp3269.last_plus_one){
_throw( Null_Exception);}* _temp3271;}).last_line), Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3272= yyvs; struct _xenum_struct** _temp3274= _temp3272.curr
+( yyvsp_offset - 3); if( _temp3274 < _temp3272.base? 1: _temp3274 >= _temp3272.last_plus_one){
_throw( Null_Exception);}* _temp3274;}))); yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp3275=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp3275=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=({
struct Cyc_Parse_Decl_spec_struct* _temp3276=( struct Cyc_Parse_Decl_spec_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct)); _temp3276->tag= Cyc_Parse_Decl_spec_tag;
_temp3276->f1= Cyc_Absyn_enum_decl( Cyc_Absyn_Public,({ struct Cyc_Core_Opt*
_temp3277=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3277->v=( void*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3278=
yyvs; struct _xenum_struct** _temp3280= _temp3278.curr +( yyvsp_offset - 4); if(
_temp3280 < _temp3278.base? 1: _temp3280 >= _temp3278.last_plus_one){ _throw(
Null_Exception);}* _temp3280;})); _temp3277;}), ts,({ struct Cyc_Core_Opt*
_temp3281=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3281->v=( void*) Cyc_yyget_EnumeratorFieldList_tok(({ struct _tagged_ptr2
_temp3282= yyvs; struct _xenum_struct** _temp3284= _temp3282.curr +(
yyvsp_offset - 1); if( _temp3284 < _temp3282.base? 1: _temp3284 >= _temp3282.last_plus_one){
_throw( Null_Exception);}* _temp3284;})); _temp3281;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3285= yyls; struct Cyc_Yyltype* _temp3287= _temp3285.curr
+( yylsp_offset - 5); if( _temp3287 < _temp3285.base? 1: _temp3287 >= _temp3285.last_plus_one){
_throw( Null_Exception);}* _temp3287;}).first_line,({ struct _tagged_ptr3
_temp3288= yyls; struct Cyc_Yyltype* _temp3290= _temp3288.curr + yylsp_offset;
if( _temp3290 < _temp3288.base? 1: _temp3290 >= _temp3288.last_plus_one){ _throw(
Null_Exception);}* _temp3290;}).last_line));( void*) _temp3276;})};( struct
_xenum_struct*) _temp3275;}); break;} case 104: _LL3265: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp3292=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp3292=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec(({ struct Cyc_Absyn_EnumType_struct* _temp3293=( struct Cyc_Absyn_EnumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_EnumType_struct)); _temp3293->tag= Cyc_Absyn_EnumType_tag;
_temp3293->f1=( struct _tuple1*) Cyc_yyget_QualId_tok(({ struct _tagged_ptr2
_temp3294= yyvs; struct _xenum_struct** _temp3296= _temp3294.curr +(
yyvsp_offset - 1); if( _temp3296 < _temp3294.base? 1: _temp3296 >= _temp3294.last_plus_one){
_throw( Null_Exception);}* _temp3296;})); _temp3293->f2= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3297= yyvs; struct _xenum_struct** _temp3299= _temp3297.curr
+ yyvsp_offset; if( _temp3299 < _temp3297.base? 1: _temp3299 >= _temp3297.last_plus_one){
_throw( Null_Exception);}* _temp3299;})); _temp3293->f3= 0;( void*) _temp3293;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3300= yyls; struct Cyc_Yyltype*
_temp3302= _temp3300.curr +( yylsp_offset - 2); if( _temp3302 < _temp3300.base?
1: _temp3302 >= _temp3300.last_plus_one){ _throw( Null_Exception);}* _temp3302;}).first_line,({
struct _tagged_ptr3 _temp3303= yyls; struct Cyc_Yyltype* _temp3305= _temp3303.curr
+ yylsp_offset; if( _temp3305 < _temp3303.base? 1: _temp3305 >= _temp3303.last_plus_one){
_throw( Null_Exception);}* _temp3305;}).last_line))};( struct _xenum_struct*)
_temp3292;}); break; case 105: _LL3291: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp3307=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp3307=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=({
struct Cyc_Parse_Decl_spec_struct* _temp3308=( struct Cyc_Parse_Decl_spec_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Decl_spec_struct)); _temp3308->tag= Cyc_Parse_Decl_spec_tag;
_temp3308->f1= Cyc_Absyn_xenum_decl( Cyc_Absyn_Public, Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3309= yyvs; struct _xenum_struct** _temp3311= _temp3309.curr
+( yyvsp_offset - 3); if( _temp3311 < _temp3309.base? 1: _temp3311 >= _temp3309.last_plus_one){
_throw( Null_Exception);}* _temp3311;})), Cyc_yyget_EnumeratorFieldList_tok(({
struct _tagged_ptr2 _temp3312= yyvs; struct _xenum_struct** _temp3314= _temp3312.curr
+( yyvsp_offset - 1); if( _temp3314 < _temp3312.base? 1: _temp3314 >= _temp3312.last_plus_one){
_throw( Null_Exception);}* _temp3314;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3315= yyls; struct Cyc_Yyltype* _temp3317= _temp3315.curr +(
yylsp_offset - 4); if( _temp3317 < _temp3315.base? 1: _temp3317 >= _temp3315.last_plus_one){
_throw( Null_Exception);}* _temp3317;}).first_line,({ struct _tagged_ptr3
_temp3318= yyls; struct Cyc_Yyltype* _temp3320= _temp3318.curr + yylsp_offset;
if( _temp3320 < _temp3318.base? 1: _temp3320 >= _temp3318.last_plus_one){ _throw(
Null_Exception);}* _temp3320;}).last_line));( void*) _temp3308;})};( struct
_xenum_struct*) _temp3307;}); break; case 106: _LL3306: yyval=({ struct Cyc_TypeSpecifier_tok_struct*
_temp3322=( struct Cyc_TypeSpecifier_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeSpecifier_tok_struct));*
_temp3322=( struct Cyc_TypeSpecifier_tok_struct){.tag= Cyc_TypeSpecifier_tok_tag,.f1=
Cyc_Parse_type_spec(({ struct Cyc_Absyn_XenumType_struct* _temp3323=( struct Cyc_Absyn_XenumType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_XenumType_struct)); _temp3323->tag= Cyc_Absyn_XenumType_tag;
_temp3323->f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3324= yyvs;
struct _xenum_struct** _temp3326= _temp3324.curr + yyvsp_offset; if( _temp3326 <
_temp3324.base? 1: _temp3326 >= _temp3324.last_plus_one){ _throw( Null_Exception);}*
_temp3326;})); _temp3323->f2= 0;( void*) _temp3323;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3327= yyls; struct Cyc_Yyltype* _temp3329= _temp3327.curr
+( yylsp_offset - 1); if( _temp3329 < _temp3327.base? 1: _temp3329 >= _temp3327.last_plus_one){
_throw( Null_Exception);}* _temp3329;}).first_line,({ struct _tagged_ptr3
_temp3330= yyls; struct Cyc_Yyltype* _temp3332= _temp3330.curr + yylsp_offset;
if( _temp3332 < _temp3330.base? 1: _temp3332 >= _temp3330.last_plus_one){ _throw(
Null_Exception);}* _temp3332;}).last_line))};( struct _xenum_struct*) _temp3322;});
break; case 107: _LL3321: yyval=({ struct Cyc_EnumeratorFieldList_tok_struct*
_temp3334=( struct Cyc_EnumeratorFieldList_tok_struct*) GC_malloc( sizeof(
struct Cyc_EnumeratorFieldList_tok_struct));* _temp3334=( struct Cyc_EnumeratorFieldList_tok_struct){.tag=
Cyc_EnumeratorFieldList_tok_tag,.f1=({ struct Cyc_List_List* _temp3335=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3335->hd=( void*)
Cyc_yyget_EnumeratorField_tok(({ struct _tagged_ptr2 _temp3336= yyvs; struct
_xenum_struct** _temp3338= _temp3336.curr + yyvsp_offset; if( _temp3338 <
_temp3336.base? 1: _temp3338 >= _temp3336.last_plus_one){ _throw( Null_Exception);}*
_temp3338;})); _temp3335->tl= 0; _temp3335;})};( struct _xenum_struct*)
_temp3334;}); break; case 108: _LL3333: yyval=({ struct Cyc_EnumeratorFieldList_tok_struct*
_temp3340=( struct Cyc_EnumeratorFieldList_tok_struct*) GC_malloc( sizeof(
struct Cyc_EnumeratorFieldList_tok_struct));* _temp3340=( struct Cyc_EnumeratorFieldList_tok_struct){.tag=
Cyc_EnumeratorFieldList_tok_tag,.f1=({ struct Cyc_List_List* _temp3341=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3341->hd=( void*)
Cyc_yyget_EnumeratorField_tok(({ struct _tagged_ptr2 _temp3342= yyvs; struct
_xenum_struct** _temp3344= _temp3342.curr +( yyvsp_offset - 1); if( _temp3344 <
_temp3342.base? 1: _temp3344 >= _temp3342.last_plus_one){ _throw( Null_Exception);}*
_temp3344;})); _temp3341->tl= 0; _temp3341;})};( struct _xenum_struct*)
_temp3340;}); break; case 109: _LL3339: yyval=({ struct Cyc_EnumeratorFieldList_tok_struct*
_temp3346=( struct Cyc_EnumeratorFieldList_tok_struct*) GC_malloc( sizeof(
struct Cyc_EnumeratorFieldList_tok_struct));* _temp3346=( struct Cyc_EnumeratorFieldList_tok_struct){.tag=
Cyc_EnumeratorFieldList_tok_tag,.f1=({ struct Cyc_List_List* _temp3347=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3347->hd=( void*)
Cyc_yyget_EnumeratorField_tok(({ struct _tagged_ptr2 _temp3351= yyvs; struct
_xenum_struct** _temp3353= _temp3351.curr +( yyvsp_offset - 2); if( _temp3353 <
_temp3351.base? 1: _temp3353 >= _temp3351.last_plus_one){ _throw( Null_Exception);}*
_temp3353;})); _temp3347->tl= Cyc_yyget_EnumeratorFieldList_tok(({ struct
_tagged_ptr2 _temp3348= yyvs; struct _xenum_struct** _temp3350= _temp3348.curr +
yyvsp_offset; if( _temp3350 < _temp3348.base? 1: _temp3350 >= _temp3348.last_plus_one){
_throw( Null_Exception);}* _temp3350;})); _temp3347;})};( struct _xenum_struct*)
_temp3346;}); break; case 110: _LL3345: yyval=({ struct Cyc_EnumeratorFieldList_tok_struct*
_temp3355=( struct Cyc_EnumeratorFieldList_tok_struct*) GC_malloc( sizeof(
struct Cyc_EnumeratorFieldList_tok_struct));* _temp3355=( struct Cyc_EnumeratorFieldList_tok_struct){.tag=
Cyc_EnumeratorFieldList_tok_tag,.f1=({ struct Cyc_List_List* _temp3356=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp3356->hd=( void*)
Cyc_yyget_EnumeratorField_tok(({ struct _tagged_ptr2 _temp3360= yyvs; struct
_xenum_struct** _temp3362= _temp3360.curr +( yyvsp_offset - 2); if( _temp3362 <
_temp3360.base? 1: _temp3362 >= _temp3360.last_plus_one){ _throw( Null_Exception);}*
_temp3362;})); _temp3356->tl= Cyc_yyget_EnumeratorFieldList_tok(({ struct
_tagged_ptr2 _temp3357= yyvs; struct _xenum_struct** _temp3359= _temp3357.curr +
yyvsp_offset; if( _temp3359 < _temp3357.base? 1: _temp3359 >= _temp3357.last_plus_one){
_throw( Null_Exception);}* _temp3359;})); _temp3356;})};( struct _xenum_struct*)
_temp3355;}); break; case 111: _LL3354: yyval=({ struct Cyc_EnumeratorField_tok_struct*
_temp3364=( struct Cyc_EnumeratorField_tok_struct*) GC_malloc( sizeof( struct
Cyc_EnumeratorField_tok_struct));* _temp3364=( struct Cyc_EnumeratorField_tok_struct){.tag=
Cyc_EnumeratorField_tok_tag,.f1=({ struct Cyc_Absyn_Enumfield* _temp3365=(
struct Cyc_Absyn_Enumfield*) GC_malloc( sizeof( struct Cyc_Absyn_Enumfield));
_temp3365->name= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3372= yyvs;
struct _xenum_struct** _temp3374= _temp3372.curr + yyvsp_offset; if( _temp3374 <
_temp3372.base? 1: _temp3374 >= _temp3372.last_plus_one){ _throw( Null_Exception);}*
_temp3374;})); _temp3365->tag= 0; _temp3365->tvs= 0; _temp3365->typs= 0;
_temp3365->loc= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3366=
yyls; struct Cyc_Yyltype* _temp3368= _temp3366.curr + yylsp_offset; if(
_temp3368 < _temp3366.base? 1: _temp3368 >= _temp3366.last_plus_one){ _throw(
Null_Exception);}* _temp3368;}).first_line,({ struct _tagged_ptr3 _temp3369=
yyls; struct Cyc_Yyltype* _temp3371= _temp3369.curr + yylsp_offset; if(
_temp3371 < _temp3369.base? 1: _temp3371 >= _temp3369.last_plus_one){ _throw(
Null_Exception);}* _temp3371;}).last_line); _temp3365;})};( struct _xenum_struct*)
_temp3364;}); break; case 112: _LL3363: yyval=({ struct Cyc_EnumeratorField_tok_struct*
_temp3376=( struct Cyc_EnumeratorField_tok_struct*) GC_malloc( sizeof( struct
Cyc_EnumeratorField_tok_struct));* _temp3376=( struct Cyc_EnumeratorField_tok_struct){.tag=
Cyc_EnumeratorField_tok_tag,.f1=({ struct Cyc_Absyn_Enumfield* _temp3377=(
struct Cyc_Absyn_Enumfield*) GC_malloc( sizeof( struct Cyc_Absyn_Enumfield));
_temp3377->name= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3387= yyvs;
struct _xenum_struct** _temp3389= _temp3387.curr +( yyvsp_offset - 2); if(
_temp3389 < _temp3387.base? 1: _temp3389 >= _temp3387.last_plus_one){ _throw(
Null_Exception);}* _temp3389;})); _temp3377->tag=( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp3384= yyvs; struct _xenum_struct** _temp3386= _temp3384.curr
+ yyvsp_offset; if( _temp3386 < _temp3384.base? 1: _temp3386 >= _temp3384.last_plus_one){
_throw( Null_Exception);}* _temp3386;})); _temp3377->tvs= 0; _temp3377->typs= 0;
_temp3377->loc= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3378=
yyls; struct Cyc_Yyltype* _temp3380= _temp3378.curr +( yylsp_offset - 2); if(
_temp3380 < _temp3378.base? 1: _temp3380 >= _temp3378.last_plus_one){ _throw(
Null_Exception);}* _temp3380;}).first_line,({ struct _tagged_ptr3 _temp3381=
yyls; struct Cyc_Yyltype* _temp3383= _temp3381.curr + yylsp_offset; if(
_temp3383 < _temp3381.base? 1: _temp3383 >= _temp3381.last_plus_one){ _throw(
Null_Exception);}* _temp3383;}).last_line); _temp3377;})};( struct _xenum_struct*)
_temp3376;}); break; case 113: _LL3375: { struct Cyc_List_List* typs=(( struct
Cyc_List_List*(*)( struct _tuple9*(* f)( struct Cyc_Position_Segment*, struct
_tuple2*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_get_tqual_typ, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp3391= yyls; struct Cyc_Yyltype* _temp3393= _temp3391.curr +( yylsp_offset -
1); if( _temp3393 < _temp3391.base? 1: _temp3393 >= _temp3391.last_plus_one){
_throw( Null_Exception);}* _temp3393;}).first_line,({ struct _tagged_ptr3
_temp3394= yyls; struct Cyc_Yyltype* _temp3396= _temp3394.curr +( yylsp_offset -
1); if( _temp3396 < _temp3394.base? 1: _temp3396 >= _temp3394.last_plus_one){
_throw( Null_Exception);}* _temp3396;}).last_line),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp3397= yyvs; struct _xenum_struct** _temp3399= _temp3397.curr
+( yyvsp_offset - 1); if( _temp3399 < _temp3397.base? 1: _temp3399 >= _temp3397.last_plus_one){
_throw( Null_Exception);}* _temp3399;})))); struct Cyc_List_List* tvs=(( struct
Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3400=
yyls; struct Cyc_Yyltype* _temp3402= _temp3400.curr +( yylsp_offset - 3); if(
_temp3402 < _temp3400.base? 1: _temp3402 >= _temp3400.last_plus_one){ _throw(
Null_Exception);}* _temp3402;}).first_line,({ struct _tagged_ptr3 _temp3403=
yyls; struct Cyc_Yyltype* _temp3405= _temp3403.curr +( yylsp_offset - 3); if(
_temp3405 < _temp3403.base? 1: _temp3405 >= _temp3403.last_plus_one){ _throw(
Null_Exception);}* _temp3405;}).last_line), Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp3406= yyvs; struct _xenum_struct** _temp3408= _temp3406.curr +(
yyvsp_offset - 3); if( _temp3408 < _temp3406.base? 1: _temp3408 >= _temp3406.last_plus_one){
_throw( Null_Exception);}* _temp3408;}))); yyval=({ struct Cyc_EnumeratorField_tok_struct*
_temp3409=( struct Cyc_EnumeratorField_tok_struct*) GC_malloc( sizeof( struct
Cyc_EnumeratorField_tok_struct));* _temp3409=( struct Cyc_EnumeratorField_tok_struct){.tag=
Cyc_EnumeratorField_tok_tag,.f1=({ struct Cyc_Absyn_Enumfield* _temp3410=(
struct Cyc_Absyn_Enumfield*) GC_malloc( sizeof( struct Cyc_Absyn_Enumfield));
_temp3410->name= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp3417= yyvs;
struct _xenum_struct** _temp3419= _temp3417.curr +( yyvsp_offset - 4); if(
_temp3419 < _temp3417.base? 1: _temp3419 >= _temp3417.last_plus_one){ _throw(
Null_Exception);}* _temp3419;})); _temp3410->tag= 0; _temp3410->tvs= tvs;
_temp3410->typs= typs; _temp3410->loc= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp3411= yyls; struct Cyc_Yyltype* _temp3413= _temp3411.curr +(
yylsp_offset - 4); if( _temp3413 < _temp3411.base? 1: _temp3413 >= _temp3411.last_plus_one){
_throw( Null_Exception);}* _temp3413;}).first_line,({ struct _tagged_ptr3
_temp3414= yyls; struct Cyc_Yyltype* _temp3416= _temp3414.curr + yylsp_offset;
if( _temp3416 < _temp3414.base? 1: _temp3416 >= _temp3414.last_plus_one){ _throw(
Null_Exception);}* _temp3416;}).last_line); _temp3410;})};( struct _xenum_struct*)
_temp3409;}); break;} case 114: _LL3390: yyval=({ struct _tagged_ptr2 _temp3421=
yyvs; struct _xenum_struct** _temp3423= _temp3421.curr + yyvsp_offset; if(
_temp3423 < _temp3421.base? 1: _temp3423 >= _temp3421.last_plus_one){ _throw(
Null_Exception);}* _temp3423;}); break; case 115: _LL3420: yyval=({ struct Cyc_Declarator_tok_struct*
_temp3425=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));*
_temp3425=( struct Cyc_Declarator_tok_struct){.tag= Cyc_Declarator_tok_tag,.f1=({
struct Cyc_Parse_Declarator* _temp3426=( struct Cyc_Parse_Declarator*) GC_malloc(
sizeof( struct Cyc_Parse_Declarator)); _temp3426->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3433= yyvs; struct _xenum_struct** _temp3435= _temp3433.curr
+ yyvsp_offset; if( _temp3435 < _temp3433.base? 1: _temp3435 >= _temp3433.last_plus_one){
_throw( Null_Exception);}* _temp3435;})))->id; _temp3426->tms=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_TypeModifierList_tok(({
struct _tagged_ptr2 _temp3427= yyvs; struct _xenum_struct** _temp3429= _temp3427.curr
+( yyvsp_offset - 1); if( _temp3429 < _temp3427.base? 1: _temp3429 >= _temp3427.last_plus_one){
_throw( Null_Exception);}* _temp3429;})),( Cyc_yyget_Declarator_tok(({ struct
_tagged_ptr2 _temp3430= yyvs; struct _xenum_struct** _temp3432= _temp3430.curr +
yyvsp_offset; if( _temp3432 < _temp3430.base? 1: _temp3432 >= _temp3430.last_plus_one){
_throw( Null_Exception);}* _temp3432;})))->tms); _temp3426;})};( struct
_xenum_struct*) _temp3425;}); break; case 116: _LL3424: yyval=({ struct Cyc_Declarator_tok_struct*
_temp3437=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));*
_temp3437=( struct Cyc_Declarator_tok_struct){.tag= Cyc_Declarator_tok_tag,.f1=({
struct Cyc_Parse_Declarator* _temp3438=( struct Cyc_Parse_Declarator*) GC_malloc(
sizeof( struct Cyc_Parse_Declarator)); _temp3438->id= Cyc_yyget_QualId_tok(({
struct _tagged_ptr2 _temp3439= yyvs; struct _xenum_struct** _temp3441= _temp3439.curr
+ yyvsp_offset; if( _temp3441 < _temp3439.base? 1: _temp3441 >= _temp3439.last_plus_one){
_throw( Null_Exception);}* _temp3441;})); _temp3438->tms= 0; _temp3438;})};(
struct _xenum_struct*) _temp3437;}); break; case 117: _LL3436: yyval=({ struct
_tagged_ptr2 _temp3443= yyvs; struct _xenum_struct** _temp3445= _temp3443.curr +(
yyvsp_offset - 1); if( _temp3445 < _temp3443.base? 1: _temp3445 >= _temp3443.last_plus_one){
_throw( Null_Exception);}* _temp3445;}); break; case 118: _LL3442: yyval=({
struct Cyc_Declarator_tok_struct* _temp3447=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));* _temp3447=( struct Cyc_Declarator_tok_struct){.tag=
Cyc_Declarator_tok_tag,.f1=({ struct Cyc_Parse_Declarator* _temp3448=( struct
Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp3448->id=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3453= yyvs;
struct _xenum_struct** _temp3455= _temp3453.curr +( yyvsp_offset - 2); if(
_temp3455 < _temp3453.base? 1: _temp3455 >= _temp3453.last_plus_one){ _throw(
Null_Exception);}* _temp3455;})))->id; _temp3448->tms=({ struct Cyc_List_List*
_temp3449=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3449->hd=( void*) Cyc_Absyn_Carray_mod; _temp3449->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3450= yyvs; struct _xenum_struct** _temp3452= _temp3450.curr
+( yyvsp_offset - 2); if( _temp3452 < _temp3450.base? 1: _temp3452 >= _temp3450.last_plus_one){
_throw( Null_Exception);}* _temp3452;})))->tms; _temp3449;}); _temp3448;})};(
struct _xenum_struct*) _temp3447;}); break; case 119: _LL3446: yyval=({ struct
Cyc_Declarator_tok_struct* _temp3457=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));* _temp3457=( struct Cyc_Declarator_tok_struct){.tag=
Cyc_Declarator_tok_tag,.f1=({ struct Cyc_Parse_Declarator* _temp3458=( struct
Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp3458->id=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3467= yyvs;
struct _xenum_struct** _temp3469= _temp3467.curr +( yyvsp_offset - 3); if(
_temp3469 < _temp3467.base? 1: _temp3469 >= _temp3467.last_plus_one){ _throw(
Null_Exception);}* _temp3469;})))->id; _temp3458->tms=({ struct Cyc_List_List*
_temp3459=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3459->hd=( void*)({ struct Cyc_Absyn_ConstArray_mod_struct* _temp3463=(
struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ConstArray_mod_struct));
_temp3463->tag= Cyc_Absyn_ConstArray_mod_tag; _temp3463->f1= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp3464= yyvs; struct _xenum_struct** _temp3466= _temp3464.curr
+( yyvsp_offset - 1); if( _temp3466 < _temp3464.base? 1: _temp3466 >= _temp3464.last_plus_one){
_throw( Null_Exception);}* _temp3466;}));( void*) _temp3463;}); _temp3459->tl=(
Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3460= yyvs; struct
_xenum_struct** _temp3462= _temp3460.curr +( yyvsp_offset - 3); if( _temp3462 <
_temp3460.base? 1: _temp3462 >= _temp3460.last_plus_one){ _throw( Null_Exception);}*
_temp3462;})))->tms; _temp3459;}); _temp3458;})};( struct _xenum_struct*)
_temp3457;}); break; case 120: _LL3456: { struct _tuple14 _temp3476; struct Cyc_Core_Opt*
_temp3477; int _temp3479; struct Cyc_List_List* _temp3481; struct _tuple14*
_temp3474= Cyc_yyget_ParamDeclListBool_tok(({ struct _tagged_ptr2 _temp3471=
yyvs; struct _xenum_struct** _temp3473= _temp3471.curr +( yyvsp_offset - 1); if(
_temp3473 < _temp3471.base? 1: _temp3473 >= _temp3471.last_plus_one){ _throw(
Null_Exception);}* _temp3473;})); _temp3476=* _temp3474; _LL3482: _temp3481=
_temp3476.f1; goto _LL3480; _LL3480: _temp3479= _temp3476.f2; goto _LL3478;
_LL3478: _temp3477= _temp3476.f3; goto _LL3475; _LL3475: yyval=({ struct Cyc_Declarator_tok_struct*
_temp3483=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));*
_temp3483=( struct Cyc_Declarator_tok_struct){.tag= Cyc_Declarator_tok_tag,.f1=({
struct Cyc_Parse_Declarator* _temp3484=( struct Cyc_Parse_Declarator*) GC_malloc(
sizeof( struct Cyc_Parse_Declarator)); _temp3484->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3491= yyvs; struct _xenum_struct** _temp3493= _temp3491.curr
+( yyvsp_offset - 3); if( _temp3493 < _temp3491.base? 1: _temp3493 >= _temp3491.last_plus_one){
_throw( Null_Exception);}* _temp3493;})))->id; _temp3484->tms=({ struct Cyc_List_List*
_temp3485=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3485->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct* _temp3489=(
struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3489->tag= Cyc_Absyn_Function_mod_tag; _temp3489->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp3490=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3490->tag= Cyc_Absyn_WithTypes_tag; _temp3490->f1= _temp3481; _temp3490->f2=
_temp3479; _temp3490->f3= _temp3477;( void*) _temp3490;});( void*) _temp3489;});
_temp3485->tl=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3486= yyvs;
struct _xenum_struct** _temp3488= _temp3486.curr +( yyvsp_offset - 3); if(
_temp3488 < _temp3486.base? 1: _temp3488 >= _temp3486.last_plus_one){ _throw(
Null_Exception);}* _temp3488;})))->tms; _temp3485;}); _temp3484;})};( struct
_xenum_struct*) _temp3483;}); break;} case 121: _LL3470: yyval=({ struct Cyc_Declarator_tok_struct*
_temp3495=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));*
_temp3495=( struct Cyc_Declarator_tok_struct){.tag= Cyc_Declarator_tok_tag,.f1=({
struct Cyc_Parse_Declarator* _temp3496=( struct Cyc_Parse_Declarator*) GC_malloc(
sizeof( struct Cyc_Parse_Declarator)); _temp3496->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3503= yyvs; struct _xenum_struct** _temp3505= _temp3503.curr
+( yyvsp_offset - 2); if( _temp3505 < _temp3503.base? 1: _temp3505 >= _temp3503.last_plus_one){
_throw( Null_Exception);}* _temp3505;})))->id; _temp3496->tms=({ struct Cyc_List_List*
_temp3497=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3497->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct* _temp3501=(
struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3501->tag= Cyc_Absyn_Function_mod_tag; _temp3501->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp3502=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3502->tag= Cyc_Absyn_WithTypes_tag; _temp3502->f1= 0; _temp3502->f2= 0;
_temp3502->f3= 0;( void*) _temp3502;});( void*) _temp3501;}); _temp3497->tl=(
Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3498= yyvs; struct
_xenum_struct** _temp3500= _temp3498.curr +( yyvsp_offset - 2); if( _temp3500 <
_temp3498.base? 1: _temp3500 >= _temp3498.last_plus_one){ _throw( Null_Exception);}*
_temp3500;})))->tms; _temp3497;}); _temp3496;})};( struct _xenum_struct*)
_temp3495;}); break; case 122: _LL3494: yyval=({ struct Cyc_Declarator_tok_struct*
_temp3507=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));*
_temp3507=( struct Cyc_Declarator_tok_struct){.tag= Cyc_Declarator_tok_tag,.f1=({
struct Cyc_Parse_Declarator* _temp3508=( struct Cyc_Parse_Declarator*) GC_malloc(
sizeof( struct Cyc_Parse_Declarator)); _temp3508->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3520= yyvs; struct _xenum_struct** _temp3522= _temp3520.curr
+( yyvsp_offset - 4); if( _temp3522 < _temp3520.base? 1: _temp3522 >= _temp3520.last_plus_one){
_throw( Null_Exception);}* _temp3522;})))->id; _temp3508->tms=({ struct Cyc_List_List*
_temp3509=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3509->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct* _temp3513=(
struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3513->tag= Cyc_Absyn_Function_mod_tag; _temp3513->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp3514=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp3514->tag= Cyc_Absyn_WithTypes_tag; _temp3514->f1= 0; _temp3514->f2= 0;
_temp3514->f3=({ struct Cyc_Core_Opt* _temp3515=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp3515->v=( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp3516=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3516->tag= Cyc_Absyn_JoinEff_tag; _temp3516->f1= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3517= yyvs; struct _xenum_struct** _temp3519= _temp3517.curr
+( yyvsp_offset - 1); if( _temp3519 < _temp3517.base? 1: _temp3519 >= _temp3517.last_plus_one){
_throw( Null_Exception);}* _temp3519;}));( void*) _temp3516;}); _temp3515;});(
void*) _temp3514;});( void*) _temp3513;}); _temp3509->tl=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3510= yyvs; struct _xenum_struct** _temp3512= _temp3510.curr
+( yyvsp_offset - 4); if( _temp3512 < _temp3510.base? 1: _temp3512 >= _temp3510.last_plus_one){
_throw( Null_Exception);}* _temp3512;})))->tms; _temp3509;}); _temp3508;})};(
struct _xenum_struct*) _temp3507;}); break; case 123: _LL3506: yyval=({ struct
Cyc_Declarator_tok_struct* _temp3524=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));* _temp3524=( struct Cyc_Declarator_tok_struct){.tag=
Cyc_Declarator_tok_tag,.f1=({ struct Cyc_Parse_Declarator* _temp3525=( struct
Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp3525->id=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3541= yyvs;
struct _xenum_struct** _temp3543= _temp3541.curr +( yyvsp_offset - 3); if(
_temp3543 < _temp3541.base? 1: _temp3543 >= _temp3541.last_plus_one){ _throw(
Null_Exception);}* _temp3543;})))->id; _temp3525->tms=({ struct Cyc_List_List*
_temp3526=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3526->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct* _temp3530=(
struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp3530->tag= Cyc_Absyn_Function_mod_tag; _temp3530->f1=( void*)({ struct Cyc_Absyn_NoTypes_struct*
_temp3531=( struct Cyc_Absyn_NoTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_NoTypes_struct));
_temp3531->tag= Cyc_Absyn_NoTypes_tag; _temp3531->f1= Cyc_yyget_IdList_tok(({
struct _tagged_ptr2 _temp3532= yyvs; struct _xenum_struct** _temp3534= _temp3532.curr
+( yyvsp_offset - 1); if( _temp3534 < _temp3532.base? 1: _temp3534 >= _temp3532.last_plus_one){
_throw( Null_Exception);}* _temp3534;})); _temp3531->f2= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3535= yyls; struct Cyc_Yyltype* _temp3537= _temp3535.curr
+( yylsp_offset - 3); if( _temp3537 < _temp3535.base? 1: _temp3537 >= _temp3535.last_plus_one){
_throw( Null_Exception);}* _temp3537;}).first_line,({ struct _tagged_ptr3
_temp3538= yyls; struct Cyc_Yyltype* _temp3540= _temp3538.curr + yylsp_offset;
if( _temp3540 < _temp3538.base? 1: _temp3540 >= _temp3538.last_plus_one){ _throw(
Null_Exception);}* _temp3540;}).last_line);( void*) _temp3531;});( void*)
_temp3530;}); _temp3526->tl=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2
_temp3527= yyvs; struct _xenum_struct** _temp3529= _temp3527.curr +(
yyvsp_offset - 3); if( _temp3529 < _temp3527.base? 1: _temp3529 >= _temp3527.last_plus_one){
_throw( Null_Exception);}* _temp3529;})))->tms; _temp3526;}); _temp3525;})};(
struct _xenum_struct*) _temp3524;}); break; case 124: _LL3523: { struct Cyc_List_List*
ts=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3545=
yyls; struct Cyc_Yyltype* _temp3547= _temp3545.curr +( yylsp_offset - 2); if(
_temp3547 < _temp3545.base? 1: _temp3547 >= _temp3545.last_plus_one){ _throw(
Null_Exception);}* _temp3547;}).first_line,({ struct _tagged_ptr3 _temp3548=
yyls; struct Cyc_Yyltype* _temp3550= _temp3548.curr + yylsp_offset; if(
_temp3550 < _temp3548.base? 1: _temp3550 >= _temp3548.last_plus_one){ _throw(
Null_Exception);}* _temp3550;}).last_line),(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp3551= yyvs; struct _xenum_struct** _temp3553= _temp3551.curr +(
yyvsp_offset - 1); if( _temp3553 < _temp3551.base? 1: _temp3553 >= _temp3551.last_plus_one){
_throw( Null_Exception);}* _temp3553;})))); yyval=({ struct Cyc_Declarator_tok_struct*
_temp3554=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));*
_temp3554=( struct Cyc_Declarator_tok_struct){.tag= Cyc_Declarator_tok_tag,.f1=({
struct Cyc_Parse_Declarator* _temp3555=( struct Cyc_Parse_Declarator*) GC_malloc(
sizeof( struct Cyc_Parse_Declarator)); _temp3555->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3567= yyvs; struct _xenum_struct** _temp3569= _temp3567.curr
+( yyvsp_offset - 3); if( _temp3569 < _temp3567.base? 1: _temp3569 >= _temp3567.last_plus_one){
_throw( Null_Exception);}* _temp3569;})))->id; _temp3555->tms=({ struct Cyc_List_List*
_temp3556=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3556->hd=( void*)({ struct Cyc_Absyn_TypeParams_mod_struct* _temp3560=(
struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct));
_temp3560->tag= Cyc_Absyn_TypeParams_mod_tag; _temp3560->f1= ts; _temp3560->f2=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3561= yyls; struct Cyc_Yyltype*
_temp3563= _temp3561.curr +( yylsp_offset - 3); if( _temp3563 < _temp3561.base?
1: _temp3563 >= _temp3561.last_plus_one){ _throw( Null_Exception);}* _temp3563;}).first_line,({
struct _tagged_ptr3 _temp3564= yyls; struct Cyc_Yyltype* _temp3566= _temp3564.curr
+ yylsp_offset; if( _temp3566 < _temp3564.base? 1: _temp3566 >= _temp3564.last_plus_one){
_throw( Null_Exception);}* _temp3566;}).last_line); _temp3560->f3= 0;( void*)
_temp3560;}); _temp3556->tl=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2
_temp3557= yyvs; struct _xenum_struct** _temp3559= _temp3557.curr +(
yyvsp_offset - 3); if( _temp3559 < _temp3557.base? 1: _temp3559 >= _temp3557.last_plus_one){
_throw( Null_Exception);}* _temp3559;})))->tms; _temp3556;}); _temp3555;})};(
struct _xenum_struct*) _temp3554;}); break;} case 125: _LL3544:(( struct Cyc_Lexing_lexbuf*)
Cyc_Parse_lbuf->v)->lex_curr_pos -= 1;{ struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3571= yyls; struct Cyc_Yyltype* _temp3573= _temp3571.curr
+( yylsp_offset - 2); if( _temp3573 < _temp3571.base? 1: _temp3573 >= _temp3571.last_plus_one){
_throw( Null_Exception);}* _temp3573;}).first_line,({ struct _tagged_ptr3
_temp3574= yyls; struct Cyc_Yyltype* _temp3576= _temp3574.curr + yylsp_offset;
if( _temp3576 < _temp3574.base? 1: _temp3576 >= _temp3574.last_plus_one){ _throw(
Null_Exception);}* _temp3576;}).last_line),(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp3577= yyvs; struct _xenum_struct** _temp3579= _temp3577.curr +(
yyvsp_offset - 1); if( _temp3579 < _temp3577.base? 1: _temp3579 >= _temp3577.last_plus_one){
_throw( Null_Exception);}* _temp3579;})))); yyval=({ struct Cyc_Declarator_tok_struct*
_temp3580=( struct Cyc_Declarator_tok_struct*) GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));*
_temp3580=( struct Cyc_Declarator_tok_struct){.tag= Cyc_Declarator_tok_tag,.f1=({
struct Cyc_Parse_Declarator* _temp3581=( struct Cyc_Parse_Declarator*) GC_malloc(
sizeof( struct Cyc_Parse_Declarator)); _temp3581->id=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3593= yyvs; struct _xenum_struct** _temp3595= _temp3593.curr
+( yyvsp_offset - 3); if( _temp3595 < _temp3593.base? 1: _temp3595 >= _temp3593.last_plus_one){
_throw( Null_Exception);}* _temp3595;})))->id; _temp3581->tms=({ struct Cyc_List_List*
_temp3582=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3582->hd=( void*)({ struct Cyc_Absyn_TypeParams_mod_struct* _temp3586=(
struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_TypeParams_mod_struct));
_temp3586->tag= Cyc_Absyn_TypeParams_mod_tag; _temp3586->f1= ts; _temp3586->f2=
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3587= yyls; struct Cyc_Yyltype*
_temp3589= _temp3587.curr +( yylsp_offset - 3); if( _temp3589 < _temp3587.base?
1: _temp3589 >= _temp3587.last_plus_one){ _throw( Null_Exception);}* _temp3589;}).first_line,({
struct _tagged_ptr3 _temp3590= yyls; struct Cyc_Yyltype* _temp3592= _temp3590.curr
+ yylsp_offset; if( _temp3592 < _temp3590.base? 1: _temp3592 >= _temp3590.last_plus_one){
_throw( Null_Exception);}* _temp3592;}).last_line); _temp3586->f3= 0;( void*)
_temp3586;}); _temp3582->tl=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2
_temp3583= yyvs; struct _xenum_struct** _temp3585= _temp3583.curr +(
yyvsp_offset - 3); if( _temp3585 < _temp3583.base? 1: _temp3585 >= _temp3583.last_plus_one){
_throw( Null_Exception);}* _temp3585;})))->tms; _temp3582;}); _temp3581;})};(
struct _xenum_struct*) _temp3580;}); break;} case 126: _LL3570: yyval=({ struct
Cyc_Declarator_tok_struct* _temp3597=( struct Cyc_Declarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_Declarator_tok_struct));* _temp3597=( struct Cyc_Declarator_tok_struct){.tag=
Cyc_Declarator_tok_tag,.f1=({ struct Cyc_Parse_Declarator* _temp3598=( struct
Cyc_Parse_Declarator*) GC_malloc( sizeof( struct Cyc_Parse_Declarator));
_temp3598->id=( Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3613= yyvs;
struct _xenum_struct** _temp3615= _temp3613.curr +( yyvsp_offset - 1); if(
_temp3615 < _temp3613.base? 1: _temp3615 >= _temp3613.last_plus_one){ _throw(
Null_Exception);}* _temp3615;})))->id; _temp3598->tms=({ struct Cyc_List_List*
_temp3599=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3599->hd=( void*)({ struct Cyc_Absyn_Attributes_mod_struct* _temp3603=(
struct Cyc_Absyn_Attributes_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Attributes_mod_struct));
_temp3603->tag= Cyc_Absyn_Attributes_mod_tag; _temp3603->f1= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3604= yyls; struct Cyc_Yyltype* _temp3606= _temp3604.curr
+ yylsp_offset; if( _temp3606 < _temp3604.base? 1: _temp3606 >= _temp3604.last_plus_one){
_throw( Null_Exception);}* _temp3606;}).first_line,({ struct _tagged_ptr3
_temp3607= yyls; struct Cyc_Yyltype* _temp3609= _temp3607.curr + yylsp_offset;
if( _temp3609 < _temp3607.base? 1: _temp3609 >= _temp3607.last_plus_one){ _throw(
Null_Exception);}* _temp3609;}).last_line); _temp3603->f2= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp3610= yyvs; struct _xenum_struct** _temp3612= _temp3610.curr
+ yyvsp_offset; if( _temp3612 < _temp3610.base? 1: _temp3612 >= _temp3610.last_plus_one){
_throw( Null_Exception);}* _temp3612;}));( void*) _temp3603;}); _temp3599->tl=(
Cyc_yyget_Declarator_tok(({ struct _tagged_ptr2 _temp3600= yyvs; struct
_xenum_struct** _temp3602= _temp3600.curr +( yyvsp_offset - 1); if( _temp3602 <
_temp3600.base? 1: _temp3602 >= _temp3600.last_plus_one){ _throw( Null_Exception);}*
_temp3602;})))->tms; _temp3599;}); _temp3598;})};( struct _xenum_struct*)
_temp3597;}); break; case 127: _LL3596: yyval=({ struct Cyc_TypeModifierList_tok_struct*
_temp3617=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct));* _temp3617=( struct Cyc_TypeModifierList_tok_struct){.tag=
Cyc_TypeModifierList_tok_tag,.f1=({ struct Cyc_List_List* _temp3618=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3618->hd=( void*)({ struct Cyc_Absyn_Pointer_mod_struct*
_temp3619=( struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp3619->tag= Cyc_Absyn_Pointer_mod_tag; _temp3619->f1=( void*) Cyc_yyget_Pointer_Sort_tok(({
struct _tagged_ptr2 _temp3620= yyvs; struct _xenum_struct** _temp3622= _temp3620.curr
+( yyvsp_offset - 1); if( _temp3622 < _temp3620.base? 1: _temp3622 >= _temp3620.last_plus_one){
_throw( Null_Exception);}* _temp3622;})); _temp3619->f2=( void*) Cyc_yyget_Rgn_tok(({
struct _tagged_ptr2 _temp3623= yyvs; struct _xenum_struct** _temp3625= _temp3623.curr
+ yyvsp_offset; if( _temp3625 < _temp3623.base? 1: _temp3625 >= _temp3623.last_plus_one){
_throw( Null_Exception);}* _temp3625;})); _temp3619->f3= Cyc_Absyn_empty_tqual();(
void*) _temp3619;}); _temp3618->tl= 0; _temp3618;})};( struct _xenum_struct*)
_temp3617;}); break; case 128: _LL3616: yyval=({ struct Cyc_TypeModifierList_tok_struct*
_temp3627=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct));* _temp3627=( struct Cyc_TypeModifierList_tok_struct){.tag=
Cyc_TypeModifierList_tok_tag,.f1=({ struct Cyc_List_List* _temp3628=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3628->hd=( void*)({ struct Cyc_Absyn_Pointer_mod_struct*
_temp3629=( struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp3629->tag= Cyc_Absyn_Pointer_mod_tag; _temp3629->f1=( void*) Cyc_yyget_Pointer_Sort_tok(({
struct _tagged_ptr2 _temp3630= yyvs; struct _xenum_struct** _temp3632= _temp3630.curr
+( yyvsp_offset - 2); if( _temp3632 < _temp3630.base? 1: _temp3632 >= _temp3630.last_plus_one){
_throw( Null_Exception);}* _temp3632;})); _temp3629->f2=( void*) Cyc_yyget_Rgn_tok(({
struct _tagged_ptr2 _temp3633= yyvs; struct _xenum_struct** _temp3635= _temp3633.curr
+( yyvsp_offset - 1); if( _temp3635 < _temp3633.base? 1: _temp3635 >= _temp3633.last_plus_one){
_throw( Null_Exception);}* _temp3635;})); _temp3629->f3= Cyc_yyget_TypeQual_tok(({
struct _tagged_ptr2 _temp3636= yyvs; struct _xenum_struct** _temp3638= _temp3636.curr
+ yyvsp_offset; if( _temp3638 < _temp3636.base? 1: _temp3638 >= _temp3636.last_plus_one){
_throw( Null_Exception);}* _temp3638;}));( void*) _temp3629;}); _temp3628->tl= 0;
_temp3628;})};( struct _xenum_struct*) _temp3627;}); break; case 129: _LL3626:
yyval=({ struct Cyc_TypeModifierList_tok_struct* _temp3640=( struct Cyc_TypeModifierList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeModifierList_tok_struct));* _temp3640=( struct
Cyc_TypeModifierList_tok_struct){.tag= Cyc_TypeModifierList_tok_tag,.f1=({
struct Cyc_List_List* _temp3641=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3641->hd=( void*)({ struct Cyc_Absyn_Pointer_mod_struct*
_temp3645=( struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp3645->tag= Cyc_Absyn_Pointer_mod_tag; _temp3645->f1=( void*) Cyc_yyget_Pointer_Sort_tok(({
struct _tagged_ptr2 _temp3646= yyvs; struct _xenum_struct** _temp3648= _temp3646.curr
+( yyvsp_offset - 2); if( _temp3648 < _temp3646.base? 1: _temp3648 >= _temp3646.last_plus_one){
_throw( Null_Exception);}* _temp3648;})); _temp3645->f2=( void*) Cyc_yyget_Rgn_tok(({
struct _tagged_ptr2 _temp3649= yyvs; struct _xenum_struct** _temp3651= _temp3649.curr
+( yyvsp_offset - 1); if( _temp3651 < _temp3649.base? 1: _temp3651 >= _temp3649.last_plus_one){
_throw( Null_Exception);}* _temp3651;})); _temp3645->f3= Cyc_Absyn_empty_tqual();(
void*) _temp3645;}); _temp3641->tl= Cyc_yyget_TypeModifierList_tok(({ struct
_tagged_ptr2 _temp3642= yyvs; struct _xenum_struct** _temp3644= _temp3642.curr +
yyvsp_offset; if( _temp3644 < _temp3642.base? 1: _temp3644 >= _temp3642.last_plus_one){
_throw( Null_Exception);}* _temp3644;})); _temp3641;})};( struct _xenum_struct*)
_temp3640;}); break; case 130: _LL3639: yyval=({ struct Cyc_TypeModifierList_tok_struct*
_temp3653=( struct Cyc_TypeModifierList_tok_struct*) GC_malloc( sizeof( struct
Cyc_TypeModifierList_tok_struct));* _temp3653=( struct Cyc_TypeModifierList_tok_struct){.tag=
Cyc_TypeModifierList_tok_tag,.f1=({ struct Cyc_List_List* _temp3654=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3654->hd=( void*)({ struct Cyc_Absyn_Pointer_mod_struct*
_temp3658=( struct Cyc_Absyn_Pointer_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Pointer_mod_struct));
_temp3658->tag= Cyc_Absyn_Pointer_mod_tag; _temp3658->f1=( void*) Cyc_yyget_Pointer_Sort_tok(({
struct _tagged_ptr2 _temp3659= yyvs; struct _xenum_struct** _temp3661= _temp3659.curr
+( yyvsp_offset - 3); if( _temp3661 < _temp3659.base? 1: _temp3661 >= _temp3659.last_plus_one){
_throw( Null_Exception);}* _temp3661;})); _temp3658->f2=( void*) Cyc_yyget_Rgn_tok(({
struct _tagged_ptr2 _temp3662= yyvs; struct _xenum_struct** _temp3664= _temp3662.curr
+( yyvsp_offset - 2); if( _temp3664 < _temp3662.base? 1: _temp3664 >= _temp3662.last_plus_one){
_throw( Null_Exception);}* _temp3664;})); _temp3658->f3= Cyc_yyget_TypeQual_tok(({
struct _tagged_ptr2 _temp3665= yyvs; struct _xenum_struct** _temp3667= _temp3665.curr
+( yyvsp_offset - 1); if( _temp3667 < _temp3665.base? 1: _temp3667 >= _temp3665.last_plus_one){
_throw( Null_Exception);}* _temp3667;}));( void*) _temp3658;}); _temp3654->tl=
Cyc_yyget_TypeModifierList_tok(({ struct _tagged_ptr2 _temp3655= yyvs; struct
_xenum_struct** _temp3657= _temp3655.curr + yyvsp_offset; if( _temp3657 <
_temp3655.base? 1: _temp3657 >= _temp3655.last_plus_one){ _throw( Null_Exception);}*
_temp3657;})); _temp3654;})};( struct _xenum_struct*) _temp3653;}); break; case
131: _LL3652: yyval=({ struct Cyc_Pointer_Sort_tok_struct* _temp3669=( struct
Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));*
_temp3669=( struct Cyc_Pointer_Sort_tok_struct){.tag= Cyc_Pointer_Sort_tok_tag,.f1=({
struct Cyc_Absyn_Nullable_ps_struct* _temp3670=( struct Cyc_Absyn_Nullable_ps_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct)); _temp3670->tag= Cyc_Absyn_Nullable_ps_tag;
_temp3670->f1= Cyc_Absyn_signed_int_exp( 1, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3671= yyls; struct Cyc_Yyltype* _temp3673= _temp3671.curr
+ yylsp_offset; if( _temp3673 < _temp3671.base? 1: _temp3673 >= _temp3671.last_plus_one){
_throw( Null_Exception);}* _temp3673;}).first_line,({ struct _tagged_ptr3
_temp3674= yyls; struct Cyc_Yyltype* _temp3676= _temp3674.curr + yylsp_offset;
if( _temp3676 < _temp3674.base? 1: _temp3676 >= _temp3674.last_plus_one){ _throw(
Null_Exception);}* _temp3676;}).last_line));( void*) _temp3670;})};( struct
_xenum_struct*) _temp3669;}); break; case 132: _LL3668: yyval=({ struct Cyc_Pointer_Sort_tok_struct*
_temp3678=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));*
_temp3678=( struct Cyc_Pointer_Sort_tok_struct){.tag= Cyc_Pointer_Sort_tok_tag,.f1=({
struct Cyc_Absyn_NonNullable_ps_struct* _temp3679=( struct Cyc_Absyn_NonNullable_ps_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_NonNullable_ps_struct)); _temp3679->tag= Cyc_Absyn_NonNullable_ps_tag;
_temp3679->f1= Cyc_Absyn_signed_int_exp( 1, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3680= yyls; struct Cyc_Yyltype* _temp3682= _temp3680.curr
+ yylsp_offset; if( _temp3682 < _temp3680.base? 1: _temp3682 >= _temp3680.last_plus_one){
_throw( Null_Exception);}* _temp3682;}).first_line,({ struct _tagged_ptr3
_temp3683= yyls; struct Cyc_Yyltype* _temp3685= _temp3683.curr + yylsp_offset;
if( _temp3685 < _temp3683.base? 1: _temp3685 >= _temp3683.last_plus_one){ _throw(
Null_Exception);}* _temp3685;}).last_line));( void*) _temp3679;})};( struct
_xenum_struct*) _temp3678;}); break; case 133: _LL3677: yyval=({ struct Cyc_Pointer_Sort_tok_struct*
_temp3687=( struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));*
_temp3687=( struct Cyc_Pointer_Sort_tok_struct){.tag= Cyc_Pointer_Sort_tok_tag,.f1=({
struct Cyc_Absyn_Nullable_ps_struct* _temp3688=( struct Cyc_Absyn_Nullable_ps_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Nullable_ps_struct)); _temp3688->tag= Cyc_Absyn_Nullable_ps_tag;
_temp3688->f1= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp3689= yyvs; struct
_xenum_struct** _temp3691= _temp3689.curr +( yyvsp_offset - 1); if( _temp3691 <
_temp3689.base? 1: _temp3691 >= _temp3689.last_plus_one){ _throw( Null_Exception);}*
_temp3691;}));( void*) _temp3688;})};( struct _xenum_struct*) _temp3687;});
break; case 134: _LL3686: yyval=({ struct Cyc_Pointer_Sort_tok_struct* _temp3693=(
struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));*
_temp3693=( struct Cyc_Pointer_Sort_tok_struct){.tag= Cyc_Pointer_Sort_tok_tag,.f1=({
struct Cyc_Absyn_NonNullable_ps_struct* _temp3694=( struct Cyc_Absyn_NonNullable_ps_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_NonNullable_ps_struct)); _temp3694->tag= Cyc_Absyn_NonNullable_ps_tag;
_temp3694->f1= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp3695= yyvs; struct
_xenum_struct** _temp3697= _temp3695.curr +( yyvsp_offset - 1); if( _temp3697 <
_temp3695.base? 1: _temp3697 >= _temp3695.last_plus_one){ _throw( Null_Exception);}*
_temp3697;}));( void*) _temp3694;})};( struct _xenum_struct*) _temp3693;});
break; case 135: _LL3692: yyval=({ struct Cyc_Pointer_Sort_tok_struct* _temp3699=(
struct Cyc_Pointer_Sort_tok_struct*) GC_malloc( sizeof( struct Cyc_Pointer_Sort_tok_struct));*
_temp3699=( struct Cyc_Pointer_Sort_tok_struct){.tag= Cyc_Pointer_Sort_tok_tag,.f1=
Cyc_Absyn_TaggedArray_ps};( struct _xenum_struct*) _temp3699;}); break; case 136:
_LL3698: yyval=({ struct Cyc_Rgn_tok_struct* _temp3701=( struct Cyc_Rgn_tok_struct*)
GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));* _temp3701=( struct Cyc_Rgn_tok_struct){.tag=
Cyc_Rgn_tok_tag,.f1= Cyc_Absyn_HeapRgn};( struct _xenum_struct*) _temp3701;});
break; case 137: _LL3700: yyval=({ struct Cyc_Rgn_tok_struct* _temp3703=( struct
Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));* _temp3703=(
struct Cyc_Rgn_tok_struct){.tag= Cyc_Rgn_tok_tag,.f1= Cyc_Parse_id2type( Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp3704= yyvs; struct _xenum_struct** _temp3706= _temp3704.curr
+ yyvsp_offset; if( _temp3706 < _temp3704.base? 1: _temp3706 >= _temp3704.last_plus_one){
_throw( Null_Exception);}* _temp3706;})),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)( Cyc_Absyn_RgnKind))};( struct _xenum_struct*) _temp3703;});
break; case 138: _LL3702: if( Cyc_yyget_Kind_tok(({ struct _tagged_ptr2
_temp3708= yyvs; struct _xenum_struct** _temp3710= _temp3708.curr + yyvsp_offset;
if( _temp3710 < _temp3708.base? 1: _temp3710 >= _temp3708.last_plus_one){ _throw(
Null_Exception);}* _temp3710;})) != Cyc_Absyn_RgnKind){ Cyc_Parse_err(( struct
_tagged_string)({ char* _temp3711=( char*)"expecting region kind\n"; struct
_tagged_string _temp3712; _temp3712.curr= _temp3711; _temp3712.base= _temp3711;
_temp3712.last_plus_one= _temp3711 + 23; _temp3712;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3713= yyls; struct Cyc_Yyltype* _temp3715= _temp3713.curr
+ yylsp_offset; if( _temp3715 < _temp3713.base? 1: _temp3715 >= _temp3713.last_plus_one){
_throw( Null_Exception);}* _temp3715;}).first_line,({ struct _tagged_ptr3
_temp3716= yyls; struct Cyc_Yyltype* _temp3718= _temp3716.curr + yylsp_offset;
if( _temp3718 < _temp3716.base? 1: _temp3718 >= _temp3716.last_plus_one){ _throw(
Null_Exception);}* _temp3718;}).last_line));} yyval=({ struct Cyc_Rgn_tok_struct*
_temp3719=( struct Cyc_Rgn_tok_struct*) GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));*
_temp3719=( struct Cyc_Rgn_tok_struct){.tag= Cyc_Rgn_tok_tag,.f1= Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp3720= yyvs; struct
_xenum_struct** _temp3722= _temp3720.curr +( yyvsp_offset - 2); if( _temp3722 <
_temp3720.base? 1: _temp3722 >= _temp3720.last_plus_one){ _throw( Null_Exception);}*
_temp3722;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
Cyc_Absyn_RgnKind))};( struct _xenum_struct*) _temp3719;}); break; case 139:
_LL3707: yyval=({ struct Cyc_Rgn_tok_struct* _temp3724=( struct Cyc_Rgn_tok_struct*)
GC_malloc( sizeof( struct Cyc_Rgn_tok_struct));* _temp3724=( struct Cyc_Rgn_tok_struct){.tag=
Cyc_Rgn_tok_tag,.f1= Cyc_Absyn_new_evar( Cyc_Absyn_RgnKind)};( struct
_xenum_struct*) _temp3724;}); break; case 140: _LL3723: yyval=({ struct
_tagged_ptr2 _temp3726= yyvs; struct _xenum_struct** _temp3728= _temp3726.curr +
yyvsp_offset; if( _temp3728 < _temp3726.base? 1: _temp3728 >= _temp3726.last_plus_one){
_throw( Null_Exception);}* _temp3728;}); break; case 141: _LL3725: yyval=({
struct Cyc_TypeQual_tok_struct* _temp3730=( struct Cyc_TypeQual_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeQual_tok_struct));* _temp3730=( struct Cyc_TypeQual_tok_struct){.tag=
Cyc_TypeQual_tok_tag,.f1= Cyc_Absyn_combine_tqual( Cyc_yyget_TypeQual_tok(({
struct _tagged_ptr2 _temp3731= yyvs; struct _xenum_struct** _temp3733= _temp3731.curr
+( yyvsp_offset - 1); if( _temp3733 < _temp3731.base? 1: _temp3733 >= _temp3731.last_plus_one){
_throw( Null_Exception);}* _temp3733;})), Cyc_yyget_TypeQual_tok(({ struct
_tagged_ptr2 _temp3734= yyvs; struct _xenum_struct** _temp3736= _temp3734.curr +
yyvsp_offset; if( _temp3736 < _temp3734.base? 1: _temp3736 >= _temp3734.last_plus_one){
_throw( Null_Exception);}* _temp3736;})))};( struct _xenum_struct*) _temp3730;});
break; case 142: _LL3729: yyval=({ struct Cyc_ParamDeclListBool_tok_struct*
_temp3738=( struct Cyc_ParamDeclListBool_tok_struct*) GC_malloc( sizeof( struct
Cyc_ParamDeclListBool_tok_struct));* _temp3738=( struct Cyc_ParamDeclListBool_tok_struct){.tag=
Cyc_ParamDeclListBool_tok_tag,.f1=({ struct _tuple14* _temp3739=( struct
_tuple14*) GC_malloc( sizeof( struct _tuple14)); _temp3739->f1=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp3740= yyvs; struct _xenum_struct** _temp3742= _temp3740.curr
+ yyvsp_offset; if( _temp3742 < _temp3740.base? 1: _temp3742 >= _temp3740.last_plus_one){
_throw( Null_Exception);}* _temp3742;}))); _temp3739->f2= 0; _temp3739->f3= 0;
_temp3739;})};( struct _xenum_struct*) _temp3738;}); break; case 143: _LL3737:
yyval=({ struct Cyc_ParamDeclListBool_tok_struct* _temp3744=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct));* _temp3744=(
struct Cyc_ParamDeclListBool_tok_struct){.tag= Cyc_ParamDeclListBool_tok_tag,.f1=({
struct _tuple14* _temp3745=( struct _tuple14*) GC_malloc( sizeof( struct
_tuple14)); _temp3745->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({ struct _tagged_ptr2 _temp3746=
yyvs; struct _xenum_struct** _temp3748= _temp3746.curr +( yyvsp_offset - 2); if(
_temp3748 < _temp3746.base? 1: _temp3748 >= _temp3746.last_plus_one){ _throw(
Null_Exception);}* _temp3748;}))); _temp3745->f2= 1; _temp3745->f3= 0; _temp3745;})};(
struct _xenum_struct*) _temp3744;}); break; case 144: _LL3743: yyval=({ struct
Cyc_ParamDeclListBool_tok_struct* _temp3750=( struct Cyc_ParamDeclListBool_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclListBool_tok_struct));* _temp3750=(
struct Cyc_ParamDeclListBool_tok_struct){.tag= Cyc_ParamDeclListBool_tok_tag,.f1=({
struct _tuple14* _temp3751=( struct _tuple14*) GC_malloc( sizeof( struct
_tuple14)); _temp3751->f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( Cyc_yyget_ParamDeclList_tok(({ struct _tagged_ptr2 _temp3757=
yyvs; struct _xenum_struct** _temp3759= _temp3757.curr +( yyvsp_offset - 2); if(
_temp3759 < _temp3757.base? 1: _temp3759 >= _temp3757.last_plus_one){ _throw(
Null_Exception);}* _temp3759;}))); _temp3751->f2= 0; _temp3751->f3=( struct Cyc_Core_Opt*)({
struct Cyc_Core_Opt* _temp3752=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct
Cyc_Core_Opt)); _temp3752->v=( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp3753=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp3753->tag= Cyc_Absyn_JoinEff_tag; _temp3753->f1= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3754= yyvs; struct _xenum_struct** _temp3756= _temp3754.curr
+ yyvsp_offset; if( _temp3756 < _temp3754.base? 1: _temp3756 >= _temp3754.last_plus_one){
_throw( Null_Exception);}* _temp3756;}));( void*) _temp3753;}); _temp3752;});
_temp3751;})};( struct _xenum_struct*) _temp3750;}); break; case 145: _LL3749:
yyval=({ struct _tagged_ptr2 _temp3761= yyvs; struct _xenum_struct** _temp3763=
_temp3761.curr + yyvsp_offset; if( _temp3763 < _temp3761.base? 1: _temp3763 >=
_temp3761.last_plus_one){ _throw( Null_Exception);}* _temp3763;}); break; case
146: _LL3760: yyval=({ struct Cyc_TypeList_tok_struct* _temp3765=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));* _temp3765=( struct Cyc_TypeList_tok_struct){.tag=
Cyc_TypeList_tok_tag,.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_imp_append)( Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp3766= yyvs; struct _xenum_struct** _temp3768= _temp3766.curr +(
yyvsp_offset - 2); if( _temp3768 < _temp3766.base? 1: _temp3768 >= _temp3766.last_plus_one){
_throw( Null_Exception);}* _temp3768;})), Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp3769= yyvs; struct _xenum_struct** _temp3771= _temp3769.curr +
yyvsp_offset; if( _temp3771 < _temp3769.base? 1: _temp3771 >= _temp3769.last_plus_one){
_throw( Null_Exception);}* _temp3771;})))};( struct _xenum_struct*) _temp3765;});
break; case 147: _LL3764: yyval=({ struct Cyc_TypeList_tok_struct* _temp3773=(
struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));*
_temp3773=( struct Cyc_TypeList_tok_struct){.tag= Cyc_TypeList_tok_tag,.f1= 0};(
struct _xenum_struct*) _temp3773;}); break; case 148: _LL3772: yyval=({ struct
_tagged_ptr2 _temp3775= yyvs; struct _xenum_struct** _temp3777= _temp3775.curr +(
yyvsp_offset - 1); if( _temp3777 < _temp3775.base? 1: _temp3777 >= _temp3775.last_plus_one){
_throw( Null_Exception);}* _temp3777;}); break; case 149: _LL3774: yyval=({
struct Cyc_TypeList_tok_struct* _temp3779=( struct Cyc_TypeList_tok_struct*)
GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));* _temp3779=( struct Cyc_TypeList_tok_struct){.tag=
Cyc_TypeList_tok_tag,.f1=({ struct Cyc_List_List* _temp3780=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3780->hd=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp3781= yyvs; struct
_xenum_struct** _temp3783= _temp3781.curr + yyvsp_offset; if( _temp3783 <
_temp3781.base? 1: _temp3783 >= _temp3781.last_plus_one){ _throw( Null_Exception);}*
_temp3783;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
Cyc_Absyn_EffKind)); _temp3780->tl= 0; _temp3780;})};( struct _xenum_struct*)
_temp3779;}); break; case 150: _LL3778: if( Cyc_yyget_Kind_tok(({ struct
_tagged_ptr2 _temp3785= yyvs; struct _xenum_struct** _temp3787= _temp3785.curr +
yyvsp_offset; if( _temp3787 < _temp3785.base? 1: _temp3787 >= _temp3785.last_plus_one){
_throw( Null_Exception);}* _temp3787;})) != Cyc_Absyn_EffKind){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp3788=( char*)"expecing effect kind (E)";
struct _tagged_string _temp3789; _temp3789.curr= _temp3788; _temp3789.base=
_temp3788; _temp3789.last_plus_one= _temp3788 + 25; _temp3789;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3790= yyls; struct Cyc_Yyltype* _temp3792= _temp3790.curr
+ yylsp_offset; if( _temp3792 < _temp3790.base? 1: _temp3792 >= _temp3790.last_plus_one){
_throw( Null_Exception);}* _temp3792;}).first_line,({ struct _tagged_ptr3
_temp3793= yyls; struct Cyc_Yyltype* _temp3795= _temp3793.curr + yylsp_offset;
if( _temp3795 < _temp3793.base? 1: _temp3795 >= _temp3793.last_plus_one){ _throw(
Null_Exception);}* _temp3795;}).last_line));} yyval=({ struct Cyc_TypeList_tok_struct*
_temp3796=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));*
_temp3796=( struct Cyc_TypeList_tok_struct){.tag= Cyc_TypeList_tok_tag,.f1=({
struct Cyc_List_List* _temp3797=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3797->hd=( void*) Cyc_Parse_id2type( Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp3798= yyvs; struct _xenum_struct** _temp3800= _temp3798.curr
+( yyvsp_offset - 2); if( _temp3800 < _temp3798.base? 1: _temp3800 >= _temp3798.last_plus_one){
_throw( Null_Exception);}* _temp3800;})),(( struct Cyc_Absyn_Conref*(*)( void* x))
Cyc_Absyn_new_conref)( Cyc_Absyn_EffKind)); _temp3797->tl= 0; _temp3797;})};(
struct _xenum_struct*) _temp3796;}); break; case 151: _LL3784: yyval=({ struct
Cyc_TypeList_tok_struct* _temp3802=( struct Cyc_TypeList_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeList_tok_struct));* _temp3802=( struct Cyc_TypeList_tok_struct){.tag=
Cyc_TypeList_tok_tag,.f1=({ struct Cyc_List_List* _temp3803=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3803->hd=( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp3804=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3804->tag= Cyc_Absyn_AccessEff_tag; _temp3804->f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp3805= yyvs; struct
_xenum_struct** _temp3807= _temp3805.curr + yyvsp_offset; if( _temp3807 <
_temp3805.base? 1: _temp3807 >= _temp3805.last_plus_one){ _throw( Null_Exception);}*
_temp3807;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
Cyc_Absyn_RgnKind));( void*) _temp3804;}); _temp3803->tl= 0; _temp3803;})};(
struct _xenum_struct*) _temp3802;}); break; case 152: _LL3801: yyval=({ struct
Cyc_TypeList_tok_struct* _temp3809=( struct Cyc_TypeList_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeList_tok_struct));* _temp3809=( struct Cyc_TypeList_tok_struct){.tag=
Cyc_TypeList_tok_tag,.f1=({ struct Cyc_List_List* _temp3810=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3810->hd=( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp3814=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3814->tag= Cyc_Absyn_AccessEff_tag; _temp3814->f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp3815= yyvs; struct
_xenum_struct** _temp3817= _temp3815.curr +( yyvsp_offset - 2); if( _temp3817 <
_temp3815.base? 1: _temp3817 >= _temp3815.last_plus_one){ _throw( Null_Exception);}*
_temp3817;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
Cyc_Absyn_RgnKind));( void*) _temp3814;}); _temp3810->tl= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3811= yyvs; struct _xenum_struct** _temp3813= _temp3811.curr
+ yyvsp_offset; if( _temp3813 < _temp3811.base? 1: _temp3813 >= _temp3811.last_plus_one){
_throw( Null_Exception);}* _temp3813;})); _temp3810;})};( struct _xenum_struct*)
_temp3809;}); break; case 153: _LL3808: if( Cyc_yyget_Kind_tok(({ struct
_tagged_ptr2 _temp3819= yyvs; struct _xenum_struct** _temp3821= _temp3819.curr +
yyvsp_offset; if( _temp3821 < _temp3819.base? 1: _temp3821 >= _temp3819.last_plus_one){
_throw( Null_Exception);}* _temp3821;})) != Cyc_Absyn_RgnKind){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp3822=( char*)"expecting region kind (R)";
struct _tagged_string _temp3823; _temp3823.curr= _temp3822; _temp3823.base=
_temp3822; _temp3823.last_plus_one= _temp3822 + 26; _temp3823;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3824= yyls; struct Cyc_Yyltype* _temp3826= _temp3824.curr
+ yylsp_offset; if( _temp3826 < _temp3824.base? 1: _temp3826 >= _temp3824.last_plus_one){
_throw( Null_Exception);}* _temp3826;}).first_line,({ struct _tagged_ptr3
_temp3827= yyls; struct Cyc_Yyltype* _temp3829= _temp3827.curr + yylsp_offset;
if( _temp3829 < _temp3827.base? 1: _temp3829 >= _temp3827.last_plus_one){ _throw(
Null_Exception);}* _temp3829;}).last_line));} yyval=({ struct Cyc_TypeList_tok_struct*
_temp3830=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));*
_temp3830=( struct Cyc_TypeList_tok_struct){.tag= Cyc_TypeList_tok_tag,.f1=({
struct Cyc_List_List* _temp3831=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3831->hd=( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp3832=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3832->tag= Cyc_Absyn_AccessEff_tag; _temp3832->f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp3833= yyvs; struct
_xenum_struct** _temp3835= _temp3833.curr +( yyvsp_offset - 2); if( _temp3835 <
_temp3833.base? 1: _temp3835 >= _temp3833.last_plus_one){ _throw( Null_Exception);}*
_temp3835;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
Cyc_Absyn_RgnKind));( void*) _temp3832;}); _temp3831->tl= 0; _temp3831;})};(
struct _xenum_struct*) _temp3830;}); break; case 154: _LL3818: if( Cyc_yyget_Kind_tok(({
struct _tagged_ptr2 _temp3837= yyvs; struct _xenum_struct** _temp3839= _temp3837.curr
+( yyvsp_offset - 2); if( _temp3839 < _temp3837.base? 1: _temp3839 >= _temp3837.last_plus_one){
_throw( Null_Exception);}* _temp3839;})) != Cyc_Absyn_RgnKind){ Cyc_Parse_err((
struct _tagged_string)({ char* _temp3840=( char*)"expecting region kind (R)";
struct _tagged_string _temp3841; _temp3841.curr= _temp3840; _temp3841.base=
_temp3840; _temp3841.last_plus_one= _temp3840 + 26; _temp3841;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3842= yyls; struct Cyc_Yyltype* _temp3844= _temp3842.curr
+( yylsp_offset - 2); if( _temp3844 < _temp3842.base? 1: _temp3844 >= _temp3842.last_plus_one){
_throw( Null_Exception);}* _temp3844;}).first_line,({ struct _tagged_ptr3
_temp3845= yyls; struct Cyc_Yyltype* _temp3847= _temp3845.curr +( yylsp_offset -
2); if( _temp3847 < _temp3845.base? 1: _temp3847 >= _temp3845.last_plus_one){
_throw( Null_Exception);}* _temp3847;}).last_line));} yyval=({ struct Cyc_TypeList_tok_struct*
_temp3848=( struct Cyc_TypeList_tok_struct*) GC_malloc( sizeof( struct Cyc_TypeList_tok_struct));*
_temp3848=( struct Cyc_TypeList_tok_struct){.tag= Cyc_TypeList_tok_tag,.f1=({
struct Cyc_List_List* _temp3849=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3849->hd=( void*)({ struct Cyc_Absyn_AccessEff_struct*
_temp3853=( struct Cyc_Absyn_AccessEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_AccessEff_struct));
_temp3853->tag= Cyc_Absyn_AccessEff_tag; _temp3853->f1=( void*) Cyc_Parse_id2type(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp3854= yyvs; struct
_xenum_struct** _temp3856= _temp3854.curr +( yyvsp_offset - 4); if( _temp3856 <
_temp3854.base? 1: _temp3856 >= _temp3854.last_plus_one){ _throw( Null_Exception);}*
_temp3856;})),(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
Cyc_Absyn_RgnKind));( void*) _temp3853;}); _temp3849->tl= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp3850= yyvs; struct _xenum_struct** _temp3852= _temp3850.curr
+ yyvsp_offset; if( _temp3852 < _temp3850.base? 1: _temp3852 >= _temp3850.last_plus_one){
_throw( Null_Exception);}* _temp3852;})); _temp3849;})};( struct _xenum_struct*)
_temp3848;}); break; case 155: _LL3836: yyval=({ struct Cyc_ParamDeclList_tok_struct*
_temp3858=( struct Cyc_ParamDeclList_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct));*
_temp3858=( struct Cyc_ParamDeclList_tok_struct){.tag= Cyc_ParamDeclList_tok_tag,.f1=({
struct Cyc_List_List* _temp3859=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp3859->hd=( void*) Cyc_yyget_ParamDecl_tok(({ struct
_tagged_ptr2 _temp3860= yyvs; struct _xenum_struct** _temp3862= _temp3860.curr +
yyvsp_offset; if( _temp3862 < _temp3860.base? 1: _temp3862 >= _temp3860.last_plus_one){
_throw( Null_Exception);}* _temp3862;})); _temp3859->tl= 0; _temp3859;})};(
struct _xenum_struct*) _temp3858;}); break; case 156: _LL3857: yyval=({ struct
Cyc_ParamDeclList_tok_struct* _temp3864=( struct Cyc_ParamDeclList_tok_struct*)
GC_malloc( sizeof( struct Cyc_ParamDeclList_tok_struct));* _temp3864=( struct
Cyc_ParamDeclList_tok_struct){.tag= Cyc_ParamDeclList_tok_tag,.f1=({ struct Cyc_List_List*
_temp3865=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3865->hd=( void*) Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp3869=
yyvs; struct _xenum_struct** _temp3871= _temp3869.curr + yyvsp_offset; if(
_temp3871 < _temp3869.base? 1: _temp3871 >= _temp3869.last_plus_one){ _throw(
Null_Exception);}* _temp3871;})); _temp3865->tl= Cyc_yyget_ParamDeclList_tok(({
struct _tagged_ptr2 _temp3866= yyvs; struct _xenum_struct** _temp3868= _temp3866.curr
+( yyvsp_offset - 2); if( _temp3868 < _temp3866.base? 1: _temp3868 >= _temp3866.last_plus_one){
_throw( Null_Exception);}* _temp3868;})); _temp3865;})};( struct _xenum_struct*)
_temp3864;}); break; case 157: _LL3863: { struct Cyc_List_List* tss=(* Cyc_yyget_QualSpecList_tok(({
struct _tagged_ptr2 _temp3873= yyvs; struct _xenum_struct** _temp3875= _temp3873.curr
+( yyvsp_offset - 1); if( _temp3875 < _temp3873.base? 1: _temp3875 >= _temp3873.last_plus_one){
_throw( Null_Exception);}* _temp3875;}))).f2; struct _tuple5 ts_info= Cyc_Parse_collapse_type_specifiers(
tss, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3876= yyls; struct
Cyc_Yyltype* _temp3878= _temp3876.curr +( yylsp_offset - 1); if( _temp3878 <
_temp3876.base? 1: _temp3878 >= _temp3876.last_plus_one){ _throw( Null_Exception);}*
_temp3878;}).first_line,({ struct _tagged_ptr3 _temp3879= yyls; struct Cyc_Yyltype*
_temp3881= _temp3879.curr +( yylsp_offset - 1); if( _temp3881 < _temp3879.base?
1: _temp3881 >= _temp3879.last_plus_one){ _throw( Null_Exception);}* _temp3881;}).last_line));
if( ts_info.f2 != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp3882=(
char*)"parameter contains nested type declaration"; struct _tagged_string
_temp3883; _temp3883.curr= _temp3882; _temp3883.base= _temp3882; _temp3883.last_plus_one=
_temp3882 + 43; _temp3883;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp3884= yyls; struct Cyc_Yyltype* _temp3886= _temp3884.curr +( yylsp_offset -
1); if( _temp3886 < _temp3884.base? 1: _temp3886 >= _temp3884.last_plus_one){
_throw( Null_Exception);}* _temp3886;}).first_line,({ struct _tagged_ptr3
_temp3887= yyls; struct Cyc_Yyltype* _temp3889= _temp3887.curr +( yylsp_offset -
1); if( _temp3889 < _temp3887.base? 1: _temp3889 >= _temp3887.last_plus_one){
_throw( Null_Exception);}* _temp3889;}).last_line));}{ void* t= ts_info.f1;
struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp3890= yyvs; struct _xenum_struct** _temp3892= _temp3890.curr +(
yyvsp_offset - 1); if( _temp3892 < _temp3890.base? 1: _temp3892 >= _temp3890.last_plus_one){
_throw( Null_Exception);}* _temp3892;}))).f1; struct Cyc_List_List* tms=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3893= yyvs; struct _xenum_struct** _temp3895= _temp3893.curr
+ yyvsp_offset; if( _temp3895 < _temp3893.base? 1: _temp3895 >= _temp3893.last_plus_one){
_throw( Null_Exception);}* _temp3895;})))->tms; struct _tuple6 t_info= Cyc_Parse_apply_tms(
tq, t, 0, tms); if( t_info.f3 != 0){ Cyc_Parse_err(( struct _tagged_string)({
char* _temp3896=( char*)"parameter with bad type params"; struct _tagged_string
_temp3897; _temp3897.curr= _temp3896; _temp3897.base= _temp3896; _temp3897.last_plus_one=
_temp3896 + 31; _temp3897;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp3898= yyls; struct Cyc_Yyltype* _temp3900= _temp3898.curr + yylsp_offset;
if( _temp3900 < _temp3898.base? 1: _temp3900 >= _temp3898.last_plus_one){ _throw(
Null_Exception);}* _temp3900;}).first_line,({ struct _tagged_ptr3 _temp3901=
yyls; struct Cyc_Yyltype* _temp3903= _temp3901.curr + yylsp_offset; if(
_temp3903 < _temp3901.base? 1: _temp3903 >= _temp3901.last_plus_one){ _throw(
Null_Exception);}* _temp3903;}).last_line));}{ struct _tuple1* q=( Cyc_yyget_Declarator_tok(({
struct _tagged_ptr2 _temp3904= yyvs; struct _xenum_struct** _temp3906= _temp3904.curr
+ yyvsp_offset; if( _temp3906 < _temp3904.base? 1: _temp3906 >= _temp3904.last_plus_one){
_throw( Null_Exception);}* _temp3906;})))->id;{ void* _temp3907=(* q).f1; struct
Cyc_List_List* _temp3917; struct Cyc_List_List* _temp3919; _LL3909: if(
_temp3907 == Cyc_Absyn_Loc_n){ goto _LL3910;} else{ goto _LL3911;} _LL3911: if((
unsigned int) _temp3907 > 1u?(( struct _enum_struct*) _temp3907)->tag == Cyc_Absyn_Rel_n_tag:
0){ _LL3918: _temp3917=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp3907)->f1; if( _temp3917 == 0){ goto _LL3912;} else{ goto _LL3913;}} else{
goto _LL3913;} _LL3913: if(( unsigned int) _temp3907 > 1u?(( struct _enum_struct*)
_temp3907)->tag == Cyc_Absyn_Abs_n_tag: 0){ _LL3920: _temp3919=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp3907)->f1; if( _temp3919 == 0){ goto
_LL3914;} else{ goto _LL3915;}} else{ goto _LL3915;} _LL3915: goto _LL3916;
_LL3910: goto _LL3908; _LL3912: goto _LL3908; _LL3914: goto _LL3908; _LL3916:
Cyc_Parse_err(( struct _tagged_string)({ char* _temp3921=( char*)"parameter cannot be qualified with a module name";
struct _tagged_string _temp3922; _temp3922.curr= _temp3921; _temp3922.base=
_temp3921; _temp3922.last_plus_one= _temp3921 + 49; _temp3922;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3923= yyls; struct Cyc_Yyltype* _temp3925= _temp3923.curr
+( yylsp_offset - 1); if( _temp3925 < _temp3923.base? 1: _temp3925 >= _temp3923.last_plus_one){
_throw( Null_Exception);}* _temp3925;}).first_line,({ struct _tagged_ptr3
_temp3926= yyls; struct Cyc_Yyltype* _temp3928= _temp3926.curr +( yylsp_offset -
1); if( _temp3928 < _temp3926.base? 1: _temp3928 >= _temp3926.last_plus_one){
_throw( Null_Exception);}* _temp3928;}).last_line)); goto _LL3908; _LL3908:;}{
struct Cyc_Core_Opt* idopt=( struct Cyc_Core_Opt*)({ struct Cyc_Core_Opt*
_temp3929=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp3929->v=( void*)(* q).f2; _temp3929;}); yyval=({ struct Cyc_ParamDecl_tok_struct*
_temp3930=( struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));*
_temp3930=( struct Cyc_ParamDecl_tok_struct){.tag= Cyc_ParamDecl_tok_tag,.f1=({
struct _tuple2* _temp3931=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp3931->f1= idopt; _temp3931->f2= t_info.f1; _temp3931->f3= t_info.f2;
_temp3931;})};( struct _xenum_struct*) _temp3930;}); break;}}}} case 158:
_LL3872: { struct Cyc_List_List* tss=(* Cyc_yyget_QualSpecList_tok(({ struct
_tagged_ptr2 _temp3933= yyvs; struct _xenum_struct** _temp3935= _temp3933.curr +
yyvsp_offset; if( _temp3935 < _temp3933.base? 1: _temp3935 >= _temp3933.last_plus_one){
_throw( Null_Exception);}* _temp3935;}))).f2; struct _tuple5 ts_info= Cyc_Parse_collapse_type_specifiers(
tss, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3936= yyls; struct
Cyc_Yyltype* _temp3938= _temp3936.curr + yylsp_offset; if( _temp3938 < _temp3936.base?
1: _temp3938 >= _temp3936.last_plus_one){ _throw( Null_Exception);}* _temp3938;}).first_line,({
struct _tagged_ptr3 _temp3939= yyls; struct Cyc_Yyltype* _temp3941= _temp3939.curr
+ yylsp_offset; if( _temp3941 < _temp3939.base? 1: _temp3941 >= _temp3939.last_plus_one){
_throw( Null_Exception);}* _temp3941;}).last_line)); if( ts_info.f2 != 0){ Cyc_Parse_warn((
struct _tagged_string)({ char* _temp3942=( char*)"nested type declaration, ignoring";
struct _tagged_string _temp3943; _temp3943.curr= _temp3942; _temp3943.base=
_temp3942; _temp3943.last_plus_one= _temp3942 + 34; _temp3943;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp3944= yyls; struct Cyc_Yyltype* _temp3946= _temp3944.curr
+ yylsp_offset; if( _temp3946 < _temp3944.base? 1: _temp3946 >= _temp3944.last_plus_one){
_throw( Null_Exception);}* _temp3946;}).first_line,({ struct _tagged_ptr3
_temp3947= yyls; struct Cyc_Yyltype* _temp3949= _temp3947.curr + yylsp_offset;
if( _temp3949 < _temp3947.base? 1: _temp3949 >= _temp3947.last_plus_one){ _throw(
Null_Exception);}* _temp3949;}).last_line));}{ void* t= ts_info.f1; struct Cyc_Absyn_Tqual*
tq=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp3950= yyvs; struct
_xenum_struct** _temp3952= _temp3950.curr + yyvsp_offset; if( _temp3952 <
_temp3950.base? 1: _temp3952 >= _temp3950.last_plus_one){ _throw( Null_Exception);}*
_temp3952;}))).f1; yyval=({ struct Cyc_ParamDecl_tok_struct* _temp3953=( struct
Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));*
_temp3953=( struct Cyc_ParamDecl_tok_struct){.tag= Cyc_ParamDecl_tok_tag,.f1=({
struct _tuple2* _temp3954=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp3954->f1= 0; _temp3954->f2= tq; _temp3954->f3= t; _temp3954;})};( struct
_xenum_struct*) _temp3953;}); break;}} case 159: _LL3932: { struct Cyc_List_List*
tss=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp3956= yyvs; struct
_xenum_struct** _temp3958= _temp3956.curr +( yyvsp_offset - 1); if( _temp3958 <
_temp3956.base? 1: _temp3958 >= _temp3956.last_plus_one){ _throw( Null_Exception);}*
_temp3958;}))).f2; struct _tuple5 ts_info= Cyc_Parse_collapse_type_specifiers(
tss, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp3959= yyls; struct
Cyc_Yyltype* _temp3961= _temp3959.curr +( yylsp_offset - 1); if( _temp3961 <
_temp3959.base? 1: _temp3961 >= _temp3959.last_plus_one){ _throw( Null_Exception);}*
_temp3961;}).first_line,({ struct _tagged_ptr3 _temp3962= yyls; struct Cyc_Yyltype*
_temp3964= _temp3962.curr +( yylsp_offset - 1); if( _temp3964 < _temp3962.base?
1: _temp3964 >= _temp3962.last_plus_one){ _throw( Null_Exception);}* _temp3964;}).last_line));
if( ts_info.f2 != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp3965=(
char*)"nested type declaration, ignoring"; struct _tagged_string _temp3966;
_temp3966.curr= _temp3965; _temp3966.base= _temp3965; _temp3966.last_plus_one=
_temp3965 + 34; _temp3966;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp3967= yyls; struct Cyc_Yyltype* _temp3969= _temp3967.curr +( yylsp_offset -
1); if( _temp3969 < _temp3967.base? 1: _temp3969 >= _temp3967.last_plus_one){
_throw( Null_Exception);}* _temp3969;}).first_line,({ struct _tagged_ptr3
_temp3970= yyls; struct Cyc_Yyltype* _temp3972= _temp3970.curr +( yylsp_offset -
1); if( _temp3972 < _temp3970.base? 1: _temp3972 >= _temp3970.last_plus_one){
_throw( Null_Exception);}* _temp3972;}).last_line));}{ void* t= ts_info.f1;
struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp3973= yyvs; struct _xenum_struct** _temp3975= _temp3973.curr +(
yyvsp_offset - 1); if( _temp3975 < _temp3973.base? 1: _temp3975 >= _temp3973.last_plus_one){
_throw( Null_Exception);}* _temp3975;}))).f1; struct Cyc_List_List* tms=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp3976= yyvs; struct _xenum_struct** _temp3978= _temp3976.curr
+ yyvsp_offset; if( _temp3978 < _temp3976.base? 1: _temp3978 >= _temp3976.last_plus_one){
_throw( Null_Exception);}* _temp3978;})))->tms; struct _tuple6 t_info= Cyc_Parse_apply_tms(
tq, t, 0, tms); if( t_info.f3 != 0){ Cyc_Parse_warn(( struct _tagged_string)({
char* _temp3979=( char*)"bad type params, ignoring"; struct _tagged_string
_temp3980; _temp3980.curr= _temp3979; _temp3980.base= _temp3979; _temp3980.last_plus_one=
_temp3979 + 26; _temp3980;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp3981= yyls; struct Cyc_Yyltype* _temp3983= _temp3981.curr + yylsp_offset;
if( _temp3983 < _temp3981.base? 1: _temp3983 >= _temp3981.last_plus_one){ _throw(
Null_Exception);}* _temp3983;}).first_line,({ struct _tagged_ptr3 _temp3984=
yyls; struct Cyc_Yyltype* _temp3986= _temp3984.curr + yylsp_offset; if(
_temp3986 < _temp3984.base? 1: _temp3986 >= _temp3984.last_plus_one){ _throw(
Null_Exception);}* _temp3986;}).last_line));} yyval=({ struct Cyc_ParamDecl_tok_struct*
_temp3987=( struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));*
_temp3987=( struct Cyc_ParamDecl_tok_struct){.tag= Cyc_ParamDecl_tok_tag,.f1=({
struct _tuple2* _temp3988=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp3988->f1= 0; _temp3988->f2= t_info.f1; _temp3988->f3= t_info.f2; _temp3988;})};(
struct _xenum_struct*) _temp3987;}); break;}} case 160: _LL3955: yyval=({ struct
Cyc_IdList_tok_struct* _temp3990=( struct Cyc_IdList_tok_struct*) GC_malloc(
sizeof( struct Cyc_IdList_tok_struct));* _temp3990=( struct Cyc_IdList_tok_struct){.tag=
Cyc_IdList_tok_tag,.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( Cyc_yyget_IdList_tok(({ struct _tagged_ptr2 _temp3991= yyvs;
struct _xenum_struct** _temp3993= _temp3991.curr + yyvsp_offset; if( _temp3993 <
_temp3991.base? 1: _temp3993 >= _temp3991.last_plus_one){ _throw( Null_Exception);}*
_temp3993;})))};( struct _xenum_struct*) _temp3990;}); break; case 161: _LL3989:
yyval=({ struct Cyc_IdList_tok_struct* _temp3995=( struct Cyc_IdList_tok_struct*)
GC_malloc( sizeof( struct Cyc_IdList_tok_struct));* _temp3995=( struct Cyc_IdList_tok_struct){.tag=
Cyc_IdList_tok_tag,.f1=({ struct Cyc_List_List* _temp3996=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp3996->hd=( void*)({ struct
_tagged_string* _temp3997=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp3997[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp3998= yyvs; struct _xenum_struct** _temp4000= _temp3998.curr + yyvsp_offset;
if( _temp4000 < _temp3998.base? 1: _temp4000 >= _temp3998.last_plus_one){ _throw(
Null_Exception);}* _temp4000;})); _temp3997;}); _temp3996->tl= 0; _temp3996;})};(
struct _xenum_struct*) _temp3995;}); break; case 162: _LL3994: yyval=({ struct
Cyc_IdList_tok_struct* _temp4002=( struct Cyc_IdList_tok_struct*) GC_malloc(
sizeof( struct Cyc_IdList_tok_struct));* _temp4002=( struct Cyc_IdList_tok_struct){.tag=
Cyc_IdList_tok_tag,.f1=({ struct Cyc_List_List* _temp4003=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4003->hd=( void*)({ struct
_tagged_string* _temp4007=( struct _tagged_string*) GC_malloc( sizeof( struct
_tagged_string) * 1); _temp4007[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2
_temp4008= yyvs; struct _xenum_struct** _temp4010= _temp4008.curr + yyvsp_offset;
if( _temp4010 < _temp4008.base? 1: _temp4010 >= _temp4008.last_plus_one){ _throw(
Null_Exception);}* _temp4010;})); _temp4007;}); _temp4003->tl= Cyc_yyget_IdList_tok(({
struct _tagged_ptr2 _temp4004= yyvs; struct _xenum_struct** _temp4006= _temp4004.curr
+( yyvsp_offset - 2); if( _temp4006 < _temp4004.base? 1: _temp4006 >= _temp4004.last_plus_one){
_throw( Null_Exception);}* _temp4006;})); _temp4003;})};( struct _xenum_struct*)
_temp4002;}); break; case 163: _LL4001: yyval=({ struct _tagged_ptr2 _temp4012=
yyvs; struct _xenum_struct** _temp4014= _temp4012.curr + yyvsp_offset; if(
_temp4014 < _temp4012.base? 1: _temp4014 >= _temp4012.last_plus_one){ _throw(
Null_Exception);}* _temp4014;}); break; case 164: _LL4011: yyval=({ struct Cyc_Exp_tok_struct*
_temp4016=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp4016=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp4017=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp4017->tag=
Cyc_Absyn_UnresolvedMem_e_tag; _temp4017->f1= 0; _temp4017->f2= 0;( void*)
_temp4017;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4018= yyls;
struct Cyc_Yyltype* _temp4020= _temp4018.curr +( yylsp_offset - 1); if(
_temp4020 < _temp4018.base? 1: _temp4020 >= _temp4018.last_plus_one){ _throw(
Null_Exception);}* _temp4020;}).first_line,({ struct _tagged_ptr3 _temp4021=
yyls; struct Cyc_Yyltype* _temp4023= _temp4021.curr + yylsp_offset; if(
_temp4023 < _temp4021.base? 1: _temp4023 >= _temp4021.last_plus_one){ _throw(
Null_Exception);}* _temp4023;}).last_line))};( struct _xenum_struct*) _temp4016;});
break; case 165: _LL4015: yyval=({ struct Cyc_Exp_tok_struct* _temp4025=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp4025=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp4026=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp4026->tag=
Cyc_Absyn_UnresolvedMem_e_tag; _temp4026->f1= 0; _temp4026->f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp4027= yyvs; struct _xenum_struct** _temp4029= _temp4027.curr
+( yyvsp_offset - 1); if( _temp4029 < _temp4027.base? 1: _temp4029 >= _temp4027.last_plus_one){
_throw( Null_Exception);}* _temp4029;})));( void*) _temp4026;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4030= yyls; struct Cyc_Yyltype* _temp4032= _temp4030.curr
+( yylsp_offset - 2); if( _temp4032 < _temp4030.base? 1: _temp4032 >= _temp4030.last_plus_one){
_throw( Null_Exception);}* _temp4032;}).first_line,({ struct _tagged_ptr3
_temp4033= yyls; struct Cyc_Yyltype* _temp4035= _temp4033.curr + yylsp_offset;
if( _temp4035 < _temp4033.base? 1: _temp4035 >= _temp4033.last_plus_one){ _throw(
Null_Exception);}* _temp4035;}).last_line))};( struct _xenum_struct*) _temp4025;});
break; case 166: _LL4024: yyval=({ struct Cyc_Exp_tok_struct* _temp4037=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp4037=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_UnresolvedMem_e_struct* _temp4038=( struct Cyc_Absyn_UnresolvedMem_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnresolvedMem_e_struct)); _temp4038->tag=
Cyc_Absyn_UnresolvedMem_e_tag; _temp4038->f1= 0; _temp4038->f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp4039= yyvs; struct _xenum_struct** _temp4041= _temp4039.curr
+( yyvsp_offset - 2); if( _temp4041 < _temp4039.base? 1: _temp4041 >= _temp4039.last_plus_one){
_throw( Null_Exception);}* _temp4041;})));( void*) _temp4038;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4042= yyls; struct Cyc_Yyltype* _temp4044= _temp4042.curr
+( yylsp_offset - 3); if( _temp4044 < _temp4042.base? 1: _temp4044 >= _temp4042.last_plus_one){
_throw( Null_Exception);}* _temp4044;}).first_line,({ struct _tagged_ptr3
_temp4045= yyls; struct Cyc_Yyltype* _temp4047= _temp4045.curr + yylsp_offset;
if( _temp4047 < _temp4045.base? 1: _temp4047 >= _temp4045.last_plus_one){ _throw(
Null_Exception);}* _temp4047;}).last_line))};( struct _xenum_struct*) _temp4037;});
break; case 167: _LL4036: yyval=({ struct Cyc_InitializerList_tok_struct*
_temp4049=( struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct
Cyc_InitializerList_tok_struct));* _temp4049=( struct Cyc_InitializerList_tok_struct){.tag=
Cyc_InitializerList_tok_tag,.f1=({ struct Cyc_List_List* _temp4050=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4050->hd=( void*)({ struct
_tuple16* _temp4051=( struct _tuple16*) GC_malloc( sizeof( struct _tuple16));
_temp4051->f1= 0; _temp4051->f2= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp4052= yyvs; struct _xenum_struct** _temp4054= _temp4052.curr + yyvsp_offset;
if( _temp4054 < _temp4052.base? 1: _temp4054 >= _temp4052.last_plus_one){ _throw(
Null_Exception);}* _temp4054;})); _temp4051;}); _temp4050->tl= 0; _temp4050;})};(
struct _xenum_struct*) _temp4049;}); break; case 168: _LL4048: yyval=({ struct
Cyc_InitializerList_tok_struct* _temp4056=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct));* _temp4056=( struct
Cyc_InitializerList_tok_struct){.tag= Cyc_InitializerList_tok_tag,.f1=({ struct
Cyc_List_List* _temp4057=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4057->hd=( void*)({ struct _tuple16* _temp4058=( struct _tuple16*)
GC_malloc( sizeof( struct _tuple16)); _temp4058->f1= Cyc_yyget_DesignatorList_tok(({
struct _tagged_ptr2 _temp4062= yyvs; struct _xenum_struct** _temp4064= _temp4062.curr
+( yyvsp_offset - 1); if( _temp4064 < _temp4062.base? 1: _temp4064 >= _temp4062.last_plus_one){
_throw( Null_Exception);}* _temp4064;})); _temp4058->f2= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4059= yyvs; struct _xenum_struct** _temp4061= _temp4059.curr
+ yyvsp_offset; if( _temp4061 < _temp4059.base? 1: _temp4061 >= _temp4059.last_plus_one){
_throw( Null_Exception);}* _temp4061;})); _temp4058;}); _temp4057->tl= 0;
_temp4057;})};( struct _xenum_struct*) _temp4056;}); break; case 169: _LL4055:
yyval=({ struct Cyc_InitializerList_tok_struct* _temp4066=( struct Cyc_InitializerList_tok_struct*)
GC_malloc( sizeof( struct Cyc_InitializerList_tok_struct));* _temp4066=( struct
Cyc_InitializerList_tok_struct){.tag= Cyc_InitializerList_tok_tag,.f1=({ struct
Cyc_List_List* _temp4067=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4067->hd=( void*)({ struct _tuple16* _temp4071=( struct _tuple16*)
GC_malloc( sizeof( struct _tuple16)); _temp4071->f1= 0; _temp4071->f2= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp4072= yyvs; struct _xenum_struct** _temp4074= _temp4072.curr
+ yyvsp_offset; if( _temp4074 < _temp4072.base? 1: _temp4074 >= _temp4072.last_plus_one){
_throw( Null_Exception);}* _temp4074;})); _temp4071;}); _temp4067->tl= Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp4068= yyvs; struct _xenum_struct** _temp4070= _temp4068.curr
+( yyvsp_offset - 2); if( _temp4070 < _temp4068.base? 1: _temp4070 >= _temp4068.last_plus_one){
_throw( Null_Exception);}* _temp4070;})); _temp4067;})};( struct _xenum_struct*)
_temp4066;}); break; case 170: _LL4065: yyval=({ struct Cyc_InitializerList_tok_struct*
_temp4076=( struct Cyc_InitializerList_tok_struct*) GC_malloc( sizeof( struct
Cyc_InitializerList_tok_struct));* _temp4076=( struct Cyc_InitializerList_tok_struct){.tag=
Cyc_InitializerList_tok_tag,.f1=({ struct Cyc_List_List* _temp4077=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4077->hd=( void*)({ struct
_tuple16* _temp4081=( struct _tuple16*) GC_malloc( sizeof( struct _tuple16));
_temp4081->f1= Cyc_yyget_DesignatorList_tok(({ struct _tagged_ptr2 _temp4085=
yyvs; struct _xenum_struct** _temp4087= _temp4085.curr +( yyvsp_offset - 1); if(
_temp4087 < _temp4085.base? 1: _temp4087 >= _temp4085.last_plus_one){ _throw(
Null_Exception);}* _temp4087;})); _temp4081->f2= Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4082= yyvs; struct _xenum_struct** _temp4084= _temp4082.curr +
yyvsp_offset; if( _temp4084 < _temp4082.base? 1: _temp4084 >= _temp4082.last_plus_one){
_throw( Null_Exception);}* _temp4084;})); _temp4081;}); _temp4077->tl= Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp4078= yyvs; struct _xenum_struct** _temp4080= _temp4078.curr
+( yyvsp_offset - 3); if( _temp4080 < _temp4078.base? 1: _temp4080 >= _temp4078.last_plus_one){
_throw( Null_Exception);}* _temp4080;})); _temp4077;})};( struct _xenum_struct*)
_temp4076;}); break; case 171: _LL4075: yyval=({ struct Cyc_DesignatorList_tok_struct*
_temp4089=( struct Cyc_DesignatorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));*
_temp4089=( struct Cyc_DesignatorList_tok_struct){.tag= Cyc_DesignatorList_tok_tag,.f1=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_DesignatorList_tok(({
struct _tagged_ptr2 _temp4090= yyvs; struct _xenum_struct** _temp4092= _temp4090.curr
+( yyvsp_offset - 1); if( _temp4092 < _temp4090.base? 1: _temp4092 >= _temp4090.last_plus_one){
_throw( Null_Exception);}* _temp4092;})))};( struct _xenum_struct*) _temp4089;});
break; case 172: _LL4088: yyval=({ struct Cyc_DesignatorList_tok_struct*
_temp4094=( struct Cyc_DesignatorList_tok_struct*) GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));*
_temp4094=( struct Cyc_DesignatorList_tok_struct){.tag= Cyc_DesignatorList_tok_tag,.f1=({
struct Cyc_List_List* _temp4095=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4095->hd=( void*) Cyc_yyget_Designator_tok(({
struct _tagged_ptr2 _temp4096= yyvs; struct _xenum_struct** _temp4098= _temp4096.curr
+ yyvsp_offset; if( _temp4098 < _temp4096.base? 1: _temp4098 >= _temp4096.last_plus_one){
_throw( Null_Exception);}* _temp4098;})); _temp4095->tl= 0; _temp4095;})};(
struct _xenum_struct*) _temp4094;}); break; case 173: _LL4093: yyval=({ struct
Cyc_DesignatorList_tok_struct* _temp4100=( struct Cyc_DesignatorList_tok_struct*)
GC_malloc( sizeof( struct Cyc_DesignatorList_tok_struct));* _temp4100=( struct
Cyc_DesignatorList_tok_struct){.tag= Cyc_DesignatorList_tok_tag,.f1=({ struct
Cyc_List_List* _temp4101=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4101->hd=( void*) Cyc_yyget_Designator_tok(({ struct _tagged_ptr2 _temp4105=
yyvs; struct _xenum_struct** _temp4107= _temp4105.curr + yyvsp_offset; if(
_temp4107 < _temp4105.base? 1: _temp4107 >= _temp4105.last_plus_one){ _throw(
Null_Exception);}* _temp4107;})); _temp4101->tl= Cyc_yyget_DesignatorList_tok(({
struct _tagged_ptr2 _temp4102= yyvs; struct _xenum_struct** _temp4104= _temp4102.curr
+( yyvsp_offset - 1); if( _temp4104 < _temp4102.base? 1: _temp4104 >= _temp4102.last_plus_one){
_throw( Null_Exception);}* _temp4104;})); _temp4101;})};( struct _xenum_struct*)
_temp4100;}); break; case 174: _LL4099: yyval=({ struct Cyc_Designator_tok_struct*
_temp4109=( struct Cyc_Designator_tok_struct*) GC_malloc( sizeof( struct Cyc_Designator_tok_struct));*
_temp4109=( struct Cyc_Designator_tok_struct){.tag= Cyc_Designator_tok_tag,.f1=({
struct Cyc_Absyn_ArrayElement_struct* _temp4110=( struct Cyc_Absyn_ArrayElement_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayElement_struct)); _temp4110->tag= Cyc_Absyn_ArrayElement_tag;
_temp4110->f1= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4111= yyvs; struct
_xenum_struct** _temp4113= _temp4111.curr +( yyvsp_offset - 1); if( _temp4113 <
_temp4111.base? 1: _temp4113 >= _temp4111.last_plus_one){ _throw( Null_Exception);}*
_temp4113;}));( void*) _temp4110;})};( struct _xenum_struct*) _temp4109;});
break; case 175: _LL4108: yyval=({ struct Cyc_Designator_tok_struct* _temp4115=(
struct Cyc_Designator_tok_struct*) GC_malloc( sizeof( struct Cyc_Designator_tok_struct));*
_temp4115=( struct Cyc_Designator_tok_struct){.tag= Cyc_Designator_tok_tag,.f1=({
struct Cyc_Absyn_FieldName_struct* _temp4116=( struct Cyc_Absyn_FieldName_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_FieldName_struct)); _temp4116->tag= Cyc_Absyn_FieldName_tag;
_temp4116->f1=({ struct _tagged_string* _temp4117=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp4117[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp4118= yyvs; struct _xenum_struct** _temp4120= _temp4118.curr
+ yyvsp_offset; if( _temp4120 < _temp4118.base? 1: _temp4120 >= _temp4118.last_plus_one){
_throw( Null_Exception);}* _temp4120;})); _temp4117;});( void*) _temp4116;})};(
struct _xenum_struct*) _temp4115;}); break; case 176: _LL4114: { struct Cyc_List_List*
tss=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp4122= yyvs; struct
_xenum_struct** _temp4124= _temp4122.curr + yyvsp_offset; if( _temp4124 <
_temp4122.base? 1: _temp4124 >= _temp4122.last_plus_one){ _throw( Null_Exception);}*
_temp4124;}))).f2; struct _tuple5 ts_info= Cyc_Parse_collapse_type_specifiers(
tss, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4125= yyls; struct
Cyc_Yyltype* _temp4127= _temp4125.curr + yylsp_offset; if( _temp4127 < _temp4125.base?
1: _temp4127 >= _temp4125.last_plus_one){ _throw( Null_Exception);}* _temp4127;}).first_line,({
struct _tagged_ptr3 _temp4128= yyls; struct Cyc_Yyltype* _temp4130= _temp4128.curr
+ yylsp_offset; if( _temp4130 < _temp4128.base? 1: _temp4130 >= _temp4128.last_plus_one){
_throw( Null_Exception);}* _temp4130;}).last_line)); if( ts_info.f2 != 0){ Cyc_Parse_warn((
struct _tagged_string)({ char* _temp4131=( char*)"nested type declaration, ignoring";
struct _tagged_string _temp4132; _temp4132.curr= _temp4131; _temp4132.base=
_temp4131; _temp4132.last_plus_one= _temp4131 + 34; _temp4132;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4133= yyls; struct Cyc_Yyltype* _temp4135= _temp4133.curr
+ yylsp_offset; if( _temp4135 < _temp4133.base? 1: _temp4135 >= _temp4133.last_plus_one){
_throw( Null_Exception);}* _temp4135;}).first_line,({ struct _tagged_ptr3
_temp4136= yyls; struct Cyc_Yyltype* _temp4138= _temp4136.curr + yylsp_offset;
if( _temp4138 < _temp4136.base? 1: _temp4138 >= _temp4136.last_plus_one){ _throw(
Null_Exception);}* _temp4138;}).last_line));}{ void* t= ts_info.f1; struct Cyc_Absyn_Tqual*
tq=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp4139= yyvs; struct
_xenum_struct** _temp4141= _temp4139.curr + yyvsp_offset; if( _temp4141 <
_temp4139.base? 1: _temp4141 >= _temp4139.last_plus_one){ _throw( Null_Exception);}*
_temp4141;}))).f1; yyval=({ struct Cyc_ParamDecl_tok_struct* _temp4142=( struct
Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));*
_temp4142=( struct Cyc_ParamDecl_tok_struct){.tag= Cyc_ParamDecl_tok_tag,.f1=({
struct _tuple2* _temp4143=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp4143->f1= 0; _temp4143->f2= tq; _temp4143->f3= t; _temp4143;})};( struct
_xenum_struct*) _temp4142;}); break;}} case 177: _LL4121: { struct Cyc_List_List*
tss=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2 _temp4145= yyvs; struct
_xenum_struct** _temp4147= _temp4145.curr +( yyvsp_offset - 1); if( _temp4147 <
_temp4145.base? 1: _temp4147 >= _temp4145.last_plus_one){ _throw( Null_Exception);}*
_temp4147;}))).f2; struct _tuple5 ts_info= Cyc_Parse_collapse_type_specifiers(
tss, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4148= yyls; struct
Cyc_Yyltype* _temp4150= _temp4148.curr +( yylsp_offset - 1); if( _temp4150 <
_temp4148.base? 1: _temp4150 >= _temp4148.last_plus_one){ _throw( Null_Exception);}*
_temp4150;}).first_line,({ struct _tagged_ptr3 _temp4151= yyls; struct Cyc_Yyltype*
_temp4153= _temp4151.curr +( yylsp_offset - 1); if( _temp4153 < _temp4151.base?
1: _temp4153 >= _temp4151.last_plus_one){ _throw( Null_Exception);}* _temp4153;}).last_line));
if( ts_info.f2 != 0){ Cyc_Parse_warn(( struct _tagged_string)({ char* _temp4154=(
char*)"nested type declaration, ignoring"; struct _tagged_string _temp4155;
_temp4155.curr= _temp4154; _temp4155.base= _temp4154; _temp4155.last_plus_one=
_temp4154 + 34; _temp4155;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4156= yyls; struct Cyc_Yyltype* _temp4158= _temp4156.curr +( yylsp_offset -
1); if( _temp4158 < _temp4156.base? 1: _temp4158 >= _temp4156.last_plus_one){
_throw( Null_Exception);}* _temp4158;}).first_line,({ struct _tagged_ptr3
_temp4159= yyls; struct Cyc_Yyltype* _temp4161= _temp4159.curr +( yylsp_offset -
1); if( _temp4161 < _temp4159.base? 1: _temp4161 >= _temp4159.last_plus_one){
_throw( Null_Exception);}* _temp4161;}).last_line));}{ void* t= ts_info.f1;
struct Cyc_Absyn_Tqual* tq=(* Cyc_yyget_QualSpecList_tok(({ struct _tagged_ptr2
_temp4162= yyvs; struct _xenum_struct** _temp4164= _temp4162.curr +(
yyvsp_offset - 1); if( _temp4164 < _temp4162.base? 1: _temp4164 >= _temp4162.last_plus_one){
_throw( Null_Exception);}* _temp4164;}))).f1; struct Cyc_List_List* tms=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp4165= yyvs; struct _xenum_struct** _temp4167= _temp4165.curr
+ yyvsp_offset; if( _temp4167 < _temp4165.base? 1: _temp4167 >= _temp4165.last_plus_one){
_throw( Null_Exception);}* _temp4167;})))->tms; struct _tuple6 t_info= Cyc_Parse_apply_tms(
tq, t, 0, tms); if( t_info.f3 != 0){ Cyc_Parse_warn(( struct _tagged_string)({
char* _temp4168=( char*)"bad type params, ignoring"; struct _tagged_string
_temp4169; _temp4169.curr= _temp4168; _temp4169.base= _temp4168; _temp4169.last_plus_one=
_temp4168 + 26; _temp4169;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4170= yyls; struct Cyc_Yyltype* _temp4172= _temp4170.curr + yylsp_offset;
if( _temp4172 < _temp4170.base? 1: _temp4172 >= _temp4170.last_plus_one){ _throw(
Null_Exception);}* _temp4172;}).first_line,({ struct _tagged_ptr3 _temp4173=
yyls; struct Cyc_Yyltype* _temp4175= _temp4173.curr + yylsp_offset; if(
_temp4175 < _temp4173.base? 1: _temp4175 >= _temp4173.last_plus_one){ _throw(
Null_Exception);}* _temp4175;}).last_line));} yyval=({ struct Cyc_ParamDecl_tok_struct*
_temp4176=( struct Cyc_ParamDecl_tok_struct*) GC_malloc( sizeof( struct Cyc_ParamDecl_tok_struct));*
_temp4176=( struct Cyc_ParamDecl_tok_struct){.tag= Cyc_ParamDecl_tok_tag,.f1=({
struct _tuple2* _temp4177=( struct _tuple2*) GC_malloc( sizeof( struct _tuple2));
_temp4177->f1= 0; _temp4177->f2= t_info.f1; _temp4177->f3= t_info.f2; _temp4177;})};(
struct _xenum_struct*) _temp4176;}); break;}} case 178: _LL4144: yyval=({ struct
Cyc_Type_tok_struct* _temp4179=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof(
struct Cyc_Type_tok_struct));* _temp4179=( struct Cyc_Type_tok_struct){.tag= Cyc_Type_tok_tag,.f1=(*
Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp4180= yyvs; struct
_xenum_struct** _temp4182= _temp4180.curr + yyvsp_offset; if( _temp4182 <
_temp4180.base? 1: _temp4182 >= _temp4180.last_plus_one){ _throw( Null_Exception);}*
_temp4182;}))).f3};( struct _xenum_struct*) _temp4179;}); break; case 179:
_LL4178: yyval=({ struct Cyc_Type_tok_struct* _temp4184=( struct Cyc_Type_tok_struct*)
GC_malloc( sizeof( struct Cyc_Type_tok_struct));* _temp4184=( struct Cyc_Type_tok_struct){.tag=
Cyc_Type_tok_tag,.f1=({ struct Cyc_Absyn_JoinEff_struct* _temp4185=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp4185->tag= Cyc_Absyn_JoinEff_tag;
_temp4185->f1= 0;( void*) _temp4185;})};( struct _xenum_struct*) _temp4184;});
break; case 180: _LL4183: yyval=({ struct Cyc_Type_tok_struct* _temp4187=(
struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));*
_temp4187=( struct Cyc_Type_tok_struct){.tag= Cyc_Type_tok_tag,.f1=({ struct Cyc_Absyn_JoinEff_struct*
_temp4188=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp4188->tag= Cyc_Absyn_JoinEff_tag; _temp4188->f1= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4189= yyvs; struct _xenum_struct** _temp4191= _temp4189.curr
+( yyvsp_offset - 1); if( _temp4191 < _temp4189.base? 1: _temp4191 >= _temp4189.last_plus_one){
_throw( Null_Exception);}* _temp4191;}));( void*) _temp4188;})};( struct
_xenum_struct*) _temp4187;}); break; case 181: _LL4186: yyval=({ struct Cyc_Type_tok_struct*
_temp4193=( struct Cyc_Type_tok_struct*) GC_malloc( sizeof( struct Cyc_Type_tok_struct));*
_temp4193=( struct Cyc_Type_tok_struct){.tag= Cyc_Type_tok_tag,.f1=({ struct Cyc_Absyn_JoinEff_struct*
_temp4194=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp4194->tag= Cyc_Absyn_JoinEff_tag; _temp4194->f1=({ struct Cyc_List_List*
_temp4195=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp4195->hd=( void*) Cyc_yyget_Type_tok(({ struct _tagged_ptr2 _temp4199= yyvs;
struct _xenum_struct** _temp4201= _temp4199.curr +( yyvsp_offset - 2); if(
_temp4201 < _temp4199.base? 1: _temp4201 >= _temp4199.last_plus_one){ _throw(
Null_Exception);}* _temp4201;})); _temp4195->tl= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4196= yyvs; struct _xenum_struct** _temp4198= _temp4196.curr
+ yyvsp_offset; if( _temp4198 < _temp4196.base? 1: _temp4198 >= _temp4196.last_plus_one){
_throw( Null_Exception);}* _temp4198;})); _temp4195;});( void*) _temp4194;})};(
struct _xenum_struct*) _temp4193;}); break; case 182: _LL4192: yyval=({ struct
Cyc_TypeList_tok_struct* _temp4203=( struct Cyc_TypeList_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeList_tok_struct));* _temp4203=( struct Cyc_TypeList_tok_struct){.tag=
Cyc_TypeList_tok_tag,.f1=({ struct Cyc_List_List* _temp4204=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4204->hd=( void*) Cyc_yyget_Type_tok(({
struct _tagged_ptr2 _temp4205= yyvs; struct _xenum_struct** _temp4207= _temp4205.curr
+ yyvsp_offset; if( _temp4207 < _temp4205.base? 1: _temp4207 >= _temp4205.last_plus_one){
_throw( Null_Exception);}* _temp4207;})); _temp4204->tl= 0; _temp4204;})};(
struct _xenum_struct*) _temp4203;}); break; case 183: _LL4202: yyval=({ struct
Cyc_TypeList_tok_struct* _temp4209=( struct Cyc_TypeList_tok_struct*) GC_malloc(
sizeof( struct Cyc_TypeList_tok_struct));* _temp4209=( struct Cyc_TypeList_tok_struct){.tag=
Cyc_TypeList_tok_tag,.f1=({ struct Cyc_List_List* _temp4210=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4210->hd=( void*) Cyc_yyget_Type_tok(({
struct _tagged_ptr2 _temp4214= yyvs; struct _xenum_struct** _temp4216= _temp4214.curr
+ yyvsp_offset; if( _temp4216 < _temp4214.base? 1: _temp4216 >= _temp4214.last_plus_one){
_throw( Null_Exception);}* _temp4216;})); _temp4210->tl= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4211= yyvs; struct _xenum_struct** _temp4213= _temp4211.curr
+( yyvsp_offset - 2); if( _temp4213 < _temp4211.base? 1: _temp4213 >= _temp4211.last_plus_one){
_throw( Null_Exception);}* _temp4213;})); _temp4210;})};( struct _xenum_struct*)
_temp4209;}); break; case 184: _LL4208: yyval=({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4218=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct));* _temp4218=( struct Cyc_AbstractDeclarator_tok_struct){.tag=
Cyc_AbstractDeclarator_tok_tag,.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4219=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4219->tms= Cyc_yyget_TypeModifierList_tok(({ struct _tagged_ptr2 _temp4220=
yyvs; struct _xenum_struct** _temp4222= _temp4220.curr + yyvsp_offset; if(
_temp4222 < _temp4220.base? 1: _temp4222 >= _temp4220.last_plus_one){ _throw(
Null_Exception);}* _temp4222;})); _temp4219;})};( struct _xenum_struct*)
_temp4218;}); break; case 185: _LL4217: yyval=({ struct _tagged_ptr2 _temp4224=
yyvs; struct _xenum_struct** _temp4226= _temp4224.curr + yyvsp_offset; if(
_temp4226 < _temp4224.base? 1: _temp4226 >= _temp4224.last_plus_one){ _throw(
Null_Exception);}* _temp4226;}); break; case 186: _LL4223: yyval=({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4228=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct));* _temp4228=( struct Cyc_AbstractDeclarator_tok_struct){.tag=
Cyc_AbstractDeclarator_tok_tag,.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4229=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4229->tms=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_imp_append)( Cyc_yyget_TypeModifierList_tok(({ struct _tagged_ptr2
_temp4230= yyvs; struct _xenum_struct** _temp4232= _temp4230.curr +(
yyvsp_offset - 1); if( _temp4232 < _temp4230.base? 1: _temp4232 >= _temp4230.last_plus_one){
_throw( Null_Exception);}* _temp4232;})),( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp4233= yyvs; struct _xenum_struct** _temp4235= _temp4233.curr
+ yyvsp_offset; if( _temp4235 < _temp4233.base? 1: _temp4235 >= _temp4233.last_plus_one){
_throw( Null_Exception);}* _temp4235;})))->tms); _temp4229;})};( struct
_xenum_struct*) _temp4228;}); break; case 187: _LL4227: yyval=({ struct
_tagged_ptr2 _temp4237= yyvs; struct _xenum_struct** _temp4239= _temp4237.curr +(
yyvsp_offset - 1); if( _temp4239 < _temp4237.base? 1: _temp4239 >= _temp4237.last_plus_one){
_throw( Null_Exception);}* _temp4239;}); break; case 188: _LL4236: yyval=({
struct Cyc_AbstractDeclarator_tok_struct* _temp4241=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));* _temp4241=(
struct Cyc_AbstractDeclarator_tok_struct){.tag= Cyc_AbstractDeclarator_tok_tag,.f1=({
struct Cyc_Parse_Abstractdeclarator* _temp4242=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4242->tms=({
struct Cyc_List_List* _temp4243=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4243->hd=( void*) Cyc_Absyn_Carray_mod; _temp4243->tl=
0; _temp4243;}); _temp4242;})};( struct _xenum_struct*) _temp4241;}); break;
case 189: _LL4240: yyval=({ struct Cyc_AbstractDeclarator_tok_struct* _temp4245=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));*
_temp4245=( struct Cyc_AbstractDeclarator_tok_struct){.tag= Cyc_AbstractDeclarator_tok_tag,.f1=({
struct Cyc_Parse_Abstractdeclarator* _temp4246=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4246->tms=({
struct Cyc_List_List* _temp4247=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4247->hd=( void*) Cyc_Absyn_Carray_mod; _temp4247->tl=(
Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2 _temp4248= yyvs; struct
_xenum_struct** _temp4250= _temp4248.curr +( yyvsp_offset - 2); if( _temp4250 <
_temp4248.base? 1: _temp4250 >= _temp4248.last_plus_one){ _throw( Null_Exception);}*
_temp4250;})))->tms; _temp4247;}); _temp4246;})};( struct _xenum_struct*)
_temp4245;}); break; case 190: _LL4244: yyval=({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4252=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct));* _temp4252=( struct Cyc_AbstractDeclarator_tok_struct){.tag=
Cyc_AbstractDeclarator_tok_tag,.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4253=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4253->tms=({ struct Cyc_List_List* _temp4254=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4254->hd=( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp4255=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp4255->tag= Cyc_Absyn_ConstArray_mod_tag;
_temp4255->f1= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4256= yyvs; struct
_xenum_struct** _temp4258= _temp4256.curr +( yyvsp_offset - 1); if( _temp4258 <
_temp4256.base? 1: _temp4258 >= _temp4256.last_plus_one){ _throw( Null_Exception);}*
_temp4258;}));( void*) _temp4255;}); _temp4254->tl= 0; _temp4254;}); _temp4253;})};(
struct _xenum_struct*) _temp4252;}); break; case 191: _LL4251: yyval=({ struct
Cyc_AbstractDeclarator_tok_struct* _temp4260=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));* _temp4260=(
struct Cyc_AbstractDeclarator_tok_struct){.tag= Cyc_AbstractDeclarator_tok_tag,.f1=({
struct Cyc_Parse_Abstractdeclarator* _temp4261=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4261->tms=({
struct Cyc_List_List* _temp4262=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4262->hd=( void*)({ struct Cyc_Absyn_ConstArray_mod_struct*
_temp4266=( struct Cyc_Absyn_ConstArray_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_ConstArray_mod_struct)); _temp4266->tag= Cyc_Absyn_ConstArray_mod_tag;
_temp4266->f1= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4267= yyvs; struct
_xenum_struct** _temp4269= _temp4267.curr +( yyvsp_offset - 1); if( _temp4269 <
_temp4267.base? 1: _temp4269 >= _temp4267.last_plus_one){ _throw( Null_Exception);}*
_temp4269;}));( void*) _temp4266;}); _temp4262->tl=( Cyc_yyget_AbstractDeclarator_tok(({
struct _tagged_ptr2 _temp4263= yyvs; struct _xenum_struct** _temp4265= _temp4263.curr
+( yyvsp_offset - 3); if( _temp4265 < _temp4263.base? 1: _temp4265 >= _temp4263.last_plus_one){
_throw( Null_Exception);}* _temp4265;})))->tms; _temp4262;}); _temp4261;})};(
struct _xenum_struct*) _temp4260;}); break; case 192: _LL4259: yyval=({ struct
Cyc_AbstractDeclarator_tok_struct* _temp4271=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));* _temp4271=(
struct Cyc_AbstractDeclarator_tok_struct){.tag= Cyc_AbstractDeclarator_tok_tag,.f1=({
struct Cyc_Parse_Abstractdeclarator* _temp4272=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4272->tms=({
struct Cyc_List_List* _temp4273=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4273->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4274=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4274->tag= Cyc_Absyn_Function_mod_tag; _temp4274->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4275=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4275->tag= Cyc_Absyn_WithTypes_tag; _temp4275->f1= 0; _temp4275->f2= 0;
_temp4275->f3= 0;( void*) _temp4275;});( void*) _temp4274;}); _temp4273->tl= 0;
_temp4273;}); _temp4272;})};( struct _xenum_struct*) _temp4271;}); break; case
193: _LL4270: yyval=({ struct Cyc_AbstractDeclarator_tok_struct* _temp4277=(
struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));*
_temp4277=( struct Cyc_AbstractDeclarator_tok_struct){.tag= Cyc_AbstractDeclarator_tok_tag,.f1=({
struct Cyc_Parse_Abstractdeclarator* _temp4278=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4278->tms=({
struct Cyc_List_List* _temp4279=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4279->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4280=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4280->tag= Cyc_Absyn_Function_mod_tag; _temp4280->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4281=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4281->tag= Cyc_Absyn_WithTypes_tag; _temp4281->f1= 0; _temp4281->f2= 0;
_temp4281->f3=({ struct Cyc_Core_Opt* _temp4282=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp4282->v=( void*)({ struct Cyc_Absyn_JoinEff_struct*
_temp4283=( struct Cyc_Absyn_JoinEff_struct*) GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct));
_temp4283->tag= Cyc_Absyn_JoinEff_tag; _temp4283->f1= Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp4284= yyvs; struct _xenum_struct** _temp4286= _temp4284.curr
+( yyvsp_offset - 1); if( _temp4286 < _temp4284.base? 1: _temp4286 >= _temp4284.last_plus_one){
_throw( Null_Exception);}* _temp4286;}));( void*) _temp4283;}); _temp4282;});(
void*) _temp4281;});( void*) _temp4280;}); _temp4279->tl= 0; _temp4279;});
_temp4278;})};( struct _xenum_struct*) _temp4277;}); break; case 194: _LL4276: {
struct _tuple14 _temp4293; struct Cyc_Core_Opt* _temp4294; int _temp4296; struct
Cyc_List_List* _temp4298; struct _tuple14* _temp4291= Cyc_yyget_ParamDeclListBool_tok(({
struct _tagged_ptr2 _temp4288= yyvs; struct _xenum_struct** _temp4290= _temp4288.curr
+( yyvsp_offset - 1); if( _temp4290 < _temp4288.base? 1: _temp4290 >= _temp4288.last_plus_one){
_throw( Null_Exception);}* _temp4290;})); _temp4293=* _temp4291; _LL4299:
_temp4298= _temp4293.f1; goto _LL4297; _LL4297: _temp4296= _temp4293.f2; goto
_LL4295; _LL4295: _temp4294= _temp4293.f3; goto _LL4292; _LL4292: yyval=({
struct Cyc_AbstractDeclarator_tok_struct* _temp4300=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));* _temp4300=(
struct Cyc_AbstractDeclarator_tok_struct){.tag= Cyc_AbstractDeclarator_tok_tag,.f1=({
struct Cyc_Parse_Abstractdeclarator* _temp4301=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4301->tms=({
struct Cyc_List_List* _temp4302=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4302->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4303=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4303->tag= Cyc_Absyn_Function_mod_tag; _temp4303->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4304=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4304->tag= Cyc_Absyn_WithTypes_tag; _temp4304->f1= _temp4298; _temp4304->f2=
_temp4296; _temp4304->f3= _temp4294;( void*) _temp4304;});( void*) _temp4303;});
_temp4302->tl= 0; _temp4302;}); _temp4301;})};( struct _xenum_struct*) _temp4300;});
break;} case 195: _LL4287: yyval=({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4306=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct));* _temp4306=( struct Cyc_AbstractDeclarator_tok_struct){.tag=
Cyc_AbstractDeclarator_tok_tag,.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4307=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4307->tms=({ struct Cyc_List_List* _temp4308=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4308->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4312=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4312->tag= Cyc_Absyn_Function_mod_tag; _temp4312->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4313=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4313->tag= Cyc_Absyn_WithTypes_tag; _temp4313->f1= 0; _temp4313->f2= 0;
_temp4313->f3= 0;( void*) _temp4313;});( void*) _temp4312;}); _temp4308->tl=(
Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2 _temp4309= yyvs; struct
_xenum_struct** _temp4311= _temp4309.curr +( yyvsp_offset - 2); if( _temp4311 <
_temp4309.base? 1: _temp4311 >= _temp4309.last_plus_one){ _throw( Null_Exception);}*
_temp4311;})))->tms; _temp4308;}); _temp4307;})};( struct _xenum_struct*)
_temp4306;}); break; case 196: _LL4305: { struct Cyc_Core_Opt* eff=({ struct Cyc_Core_Opt*
_temp4315=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp4315->v=( void*)({ struct Cyc_Absyn_JoinEff_struct* _temp4316=( struct Cyc_Absyn_JoinEff_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_JoinEff_struct)); _temp4316->tag= Cyc_Absyn_JoinEff_tag;
_temp4316->f1= Cyc_yyget_TypeList_tok(({ struct _tagged_ptr2 _temp4317= yyvs;
struct _xenum_struct** _temp4319= _temp4317.curr +( yyvsp_offset - 1); if(
_temp4319 < _temp4317.base? 1: _temp4319 >= _temp4317.last_plus_one){ _throw(
Null_Exception);}* _temp4319;}));( void*) _temp4316;}); _temp4315;}); yyval=({
struct Cyc_AbstractDeclarator_tok_struct* _temp4320=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));* _temp4320=(
struct Cyc_AbstractDeclarator_tok_struct){.tag= Cyc_AbstractDeclarator_tok_tag,.f1=({
struct Cyc_Parse_Abstractdeclarator* _temp4321=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4321->tms=({
struct Cyc_List_List* _temp4322=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4322->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4326=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4326->tag= Cyc_Absyn_Function_mod_tag; _temp4326->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4327=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4327->tag= Cyc_Absyn_WithTypes_tag; _temp4327->f1= 0; _temp4327->f2= 0;
_temp4327->f3=( struct Cyc_Core_Opt*) eff;( void*) _temp4327;});( void*)
_temp4326;}); _temp4322->tl=( Cyc_yyget_AbstractDeclarator_tok(({ struct
_tagged_ptr2 _temp4323= yyvs; struct _xenum_struct** _temp4325= _temp4323.curr +(
yyvsp_offset - 4); if( _temp4325 < _temp4323.base? 1: _temp4325 >= _temp4323.last_plus_one){
_throw( Null_Exception);}* _temp4325;})))->tms; _temp4322;}); _temp4321;})};(
struct _xenum_struct*) _temp4320;}); break;} case 197: _LL4314: { struct
_tuple14 _temp4334; struct Cyc_Core_Opt* _temp4335; int _temp4337; struct Cyc_List_List*
_temp4339; struct _tuple14* _temp4332= Cyc_yyget_ParamDeclListBool_tok(({ struct
_tagged_ptr2 _temp4329= yyvs; struct _xenum_struct** _temp4331= _temp4329.curr +(
yyvsp_offset - 1); if( _temp4331 < _temp4329.base? 1: _temp4331 >= _temp4329.last_plus_one){
_throw( Null_Exception);}* _temp4331;})); _temp4334=* _temp4332; _LL4340:
_temp4339= _temp4334.f1; goto _LL4338; _LL4338: _temp4337= _temp4334.f2; goto
_LL4336; _LL4336: _temp4335= _temp4334.f3; goto _LL4333; _LL4333: yyval=({
struct Cyc_AbstractDeclarator_tok_struct* _temp4341=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));* _temp4341=(
struct Cyc_AbstractDeclarator_tok_struct){.tag= Cyc_AbstractDeclarator_tok_tag,.f1=({
struct Cyc_Parse_Abstractdeclarator* _temp4342=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4342->tms=({
struct Cyc_List_List* _temp4343=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4343->hd=( void*)({ struct Cyc_Absyn_Function_mod_struct*
_temp4347=( struct Cyc_Absyn_Function_mod_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Function_mod_struct));
_temp4347->tag= Cyc_Absyn_Function_mod_tag; _temp4347->f1=( void*)({ struct Cyc_Absyn_WithTypes_struct*
_temp4348=( struct Cyc_Absyn_WithTypes_struct*) GC_malloc( sizeof( struct Cyc_Absyn_WithTypes_struct));
_temp4348->tag= Cyc_Absyn_WithTypes_tag; _temp4348->f1= _temp4339; _temp4348->f2=
_temp4337; _temp4348->f3= _temp4335;( void*) _temp4348;});( void*) _temp4347;});
_temp4343->tl=( Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2
_temp4344= yyvs; struct _xenum_struct** _temp4346= _temp4344.curr +(
yyvsp_offset - 3); if( _temp4346 < _temp4344.base? 1: _temp4346 >= _temp4344.last_plus_one){
_throw( Null_Exception);}* _temp4346;})))->tms; _temp4343;}); _temp4342;})};(
struct _xenum_struct*) _temp4341;}); break;} case 198: _LL4328: { struct Cyc_List_List*
ts=(( struct Cyc_List_List*(*)( struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*,
void*), struct Cyc_Position_Segment* env, struct Cyc_List_List* x)) Cyc_List_map_c)(
Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4350=
yyls; struct Cyc_Yyltype* _temp4352= _temp4350.curr +( yylsp_offset - 2); if(
_temp4352 < _temp4350.base? 1: _temp4352 >= _temp4350.last_plus_one){ _throw(
Null_Exception);}* _temp4352;}).first_line,({ struct _tagged_ptr3 _temp4353=
yyls; struct Cyc_Yyltype* _temp4355= _temp4353.curr + yylsp_offset; if(
_temp4355 < _temp4353.base? 1: _temp4355 >= _temp4353.last_plus_one){ _throw(
Null_Exception);}* _temp4355;}).last_line),(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp4356= yyvs; struct _xenum_struct** _temp4358= _temp4356.curr +(
yyvsp_offset - 1); if( _temp4358 < _temp4356.base? 1: _temp4358 >= _temp4356.last_plus_one){
_throw( Null_Exception);}* _temp4358;})))); yyval=({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4359=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct));* _temp4359=( struct Cyc_AbstractDeclarator_tok_struct){.tag=
Cyc_AbstractDeclarator_tok_tag,.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4360=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4360->tms=({ struct Cyc_List_List* _temp4361=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4361->hd=( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp4365=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp4365->tag= Cyc_Absyn_TypeParams_mod_tag;
_temp4365->f1= ts; _temp4365->f2= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4366= yyls; struct Cyc_Yyltype* _temp4368= _temp4366.curr +(
yylsp_offset - 2); if( _temp4368 < _temp4366.base? 1: _temp4368 >= _temp4366.last_plus_one){
_throw( Null_Exception);}* _temp4368;}).first_line,({ struct _tagged_ptr3
_temp4369= yyls; struct Cyc_Yyltype* _temp4371= _temp4369.curr + yylsp_offset;
if( _temp4371 < _temp4369.base? 1: _temp4371 >= _temp4369.last_plus_one){ _throw(
Null_Exception);}* _temp4371;}).last_line); _temp4365->f3= 0;( void*) _temp4365;});
_temp4361->tl=( Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2
_temp4362= yyvs; struct _xenum_struct** _temp4364= _temp4362.curr +(
yyvsp_offset - 3); if( _temp4364 < _temp4362.base? 1: _temp4364 >= _temp4362.last_plus_one){
_throw( Null_Exception);}* _temp4364;})))->tms; _temp4361;}); _temp4360;})};(
struct _xenum_struct*) _temp4359;}); break;} case 199: _LL4349:(( struct Cyc_Lexing_lexbuf*)
Cyc_Parse_lbuf->v)->lex_curr_pos -= 1;{ struct Cyc_List_List* ts=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4373= yyls; struct Cyc_Yyltype* _temp4375= _temp4373.curr
+( yylsp_offset - 2); if( _temp4375 < _temp4373.base? 1: _temp4375 >= _temp4373.last_plus_one){
_throw( Null_Exception);}* _temp4375;}).first_line,({ struct _tagged_ptr3
_temp4376= yyls; struct Cyc_Yyltype* _temp4378= _temp4376.curr + yylsp_offset;
if( _temp4378 < _temp4376.base? 1: _temp4378 >= _temp4376.last_plus_one){ _throw(
Null_Exception);}* _temp4378;}).last_line),(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp4379= yyvs; struct _xenum_struct** _temp4381= _temp4379.curr +(
yyvsp_offset - 1); if( _temp4381 < _temp4379.base? 1: _temp4381 >= _temp4379.last_plus_one){
_throw( Null_Exception);}* _temp4381;})))); yyval=({ struct Cyc_AbstractDeclarator_tok_struct*
_temp4382=( struct Cyc_AbstractDeclarator_tok_struct*) GC_malloc( sizeof( struct
Cyc_AbstractDeclarator_tok_struct));* _temp4382=( struct Cyc_AbstractDeclarator_tok_struct){.tag=
Cyc_AbstractDeclarator_tok_tag,.f1=({ struct Cyc_Parse_Abstractdeclarator*
_temp4383=( struct Cyc_Parse_Abstractdeclarator*) GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator));
_temp4383->tms=({ struct Cyc_List_List* _temp4384=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4384->hd=( void*)({ struct Cyc_Absyn_TypeParams_mod_struct*
_temp4388=( struct Cyc_Absyn_TypeParams_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_TypeParams_mod_struct)); _temp4388->tag= Cyc_Absyn_TypeParams_mod_tag;
_temp4388->f1= ts; _temp4388->f2= Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4389= yyls; struct Cyc_Yyltype* _temp4391= _temp4389.curr +(
yylsp_offset - 2); if( _temp4391 < _temp4389.base? 1: _temp4391 >= _temp4389.last_plus_one){
_throw( Null_Exception);}* _temp4391;}).first_line,({ struct _tagged_ptr3
_temp4392= yyls; struct Cyc_Yyltype* _temp4394= _temp4392.curr + yylsp_offset;
if( _temp4394 < _temp4392.base? 1: _temp4394 >= _temp4392.last_plus_one){ _throw(
Null_Exception);}* _temp4394;}).last_line); _temp4388->f3= 0;( void*) _temp4388;});
_temp4384->tl=( Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2
_temp4385= yyvs; struct _xenum_struct** _temp4387= _temp4385.curr +(
yyvsp_offset - 3); if( _temp4387 < _temp4385.base? 1: _temp4387 >= _temp4385.last_plus_one){
_throw( Null_Exception);}* _temp4387;})))->tms; _temp4384;}); _temp4383;})};(
struct _xenum_struct*) _temp4382;}); break;} case 200: _LL4372: yyval=({ struct
Cyc_AbstractDeclarator_tok_struct* _temp4396=( struct Cyc_AbstractDeclarator_tok_struct*)
GC_malloc( sizeof( struct Cyc_AbstractDeclarator_tok_struct));* _temp4396=(
struct Cyc_AbstractDeclarator_tok_struct){.tag= Cyc_AbstractDeclarator_tok_tag,.f1=({
struct Cyc_Parse_Abstractdeclarator* _temp4397=( struct Cyc_Parse_Abstractdeclarator*)
GC_malloc( sizeof( struct Cyc_Parse_Abstractdeclarator)); _temp4397->tms=({
struct Cyc_List_List* _temp4398=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4398->hd=( void*)({ struct Cyc_Absyn_Attributes_mod_struct*
_temp4402=( struct Cyc_Absyn_Attributes_mod_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Attributes_mod_struct)); _temp4402->tag= Cyc_Absyn_Attributes_mod_tag;
_temp4402->f1= Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4403=
yyls; struct Cyc_Yyltype* _temp4405= _temp4403.curr + yylsp_offset; if(
_temp4405 < _temp4403.base? 1: _temp4405 >= _temp4403.last_plus_one){ _throw(
Null_Exception);}* _temp4405;}).first_line,({ struct _tagged_ptr3 _temp4406=
yyls; struct Cyc_Yyltype* _temp4408= _temp4406.curr + yylsp_offset; if(
_temp4408 < _temp4406.base? 1: _temp4408 >= _temp4406.last_plus_one){ _throw(
Null_Exception);}* _temp4408;}).last_line); _temp4402->f2= Cyc_yyget_AttributeList_tok(({
struct _tagged_ptr2 _temp4409= yyvs; struct _xenum_struct** _temp4411= _temp4409.curr
+ yyvsp_offset; if( _temp4411 < _temp4409.base? 1: _temp4411 >= _temp4409.last_plus_one){
_throw( Null_Exception);}* _temp4411;}));( void*) _temp4402;}); _temp4398->tl=(
Cyc_yyget_AbstractDeclarator_tok(({ struct _tagged_ptr2 _temp4399= yyvs; struct
_xenum_struct** _temp4401= _temp4399.curr +( yyvsp_offset - 1); if( _temp4401 <
_temp4399.base? 1: _temp4401 >= _temp4399.last_plus_one){ _throw( Null_Exception);}*
_temp4401;})))->tms; _temp4398;}); _temp4397;})};( struct _xenum_struct*)
_temp4396;}); break; case 201: _LL4395: yyval=({ struct _tagged_ptr2 _temp4413=
yyvs; struct _xenum_struct** _temp4415= _temp4413.curr + yyvsp_offset; if(
_temp4415 < _temp4413.base? 1: _temp4415 >= _temp4413.last_plus_one){ _throw(
Null_Exception);}* _temp4415;}); break; case 202: _LL4412: yyval=({ struct
_tagged_ptr2 _temp4417= yyvs; struct _xenum_struct** _temp4419= _temp4417.curr +
yyvsp_offset; if( _temp4419 < _temp4417.base? 1: _temp4419 >= _temp4417.last_plus_one){
_throw( Null_Exception);}* _temp4419;}); break; case 203: _LL4416: yyval=({
struct _tagged_ptr2 _temp4421= yyvs; struct _xenum_struct** _temp4423= _temp4421.curr
+ yyvsp_offset; if( _temp4423 < _temp4421.base? 1: _temp4423 >= _temp4421.last_plus_one){
_throw( Null_Exception);}* _temp4423;}); break; case 204: _LL4420: yyval=({
struct _tagged_ptr2 _temp4425= yyvs; struct _xenum_struct** _temp4427= _temp4425.curr
+ yyvsp_offset; if( _temp4427 < _temp4425.base? 1: _temp4427 >= _temp4425.last_plus_one){
_throw( Null_Exception);}* _temp4427;}); break; case 205: _LL4424: yyval=({
struct _tagged_ptr2 _temp4429= yyvs; struct _xenum_struct** _temp4431= _temp4429.curr
+ yyvsp_offset; if( _temp4431 < _temp4429.base? 1: _temp4431 >= _temp4429.last_plus_one){
_throw( Null_Exception);}* _temp4431;}); break; case 206: _LL4428: yyval=({
struct _tagged_ptr2 _temp4433= yyvs; struct _xenum_struct** _temp4435= _temp4433.curr
+ yyvsp_offset; if( _temp4435 < _temp4433.base? 1: _temp4435 >= _temp4433.last_plus_one){
_throw( Null_Exception);}* _temp4435;}); break; case 207: _LL4432: yyval=({
struct Cyc_Stmt_tok_struct* _temp4437=( struct Cyc_Stmt_tok_struct*) GC_malloc(
sizeof( struct Cyc_Stmt_tok_struct));* _temp4437=( struct Cyc_Stmt_tok_struct){.tag=
Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_new_stmt(({ struct Cyc_Absyn_Cut_s_struct*
_temp4438=( struct Cyc_Absyn_Cut_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Cut_s_struct));
_temp4438->tag= Cyc_Absyn_Cut_s_tag; _temp4438->f1= Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp4439= yyvs; struct _xenum_struct** _temp4441= _temp4439.curr +
yyvsp_offset; if( _temp4441 < _temp4439.base? 1: _temp4441 >= _temp4439.last_plus_one){
_throw( Null_Exception);}* _temp4441;}));( void*) _temp4438;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4442= yyls; struct Cyc_Yyltype* _temp4444= _temp4442.curr
+( yylsp_offset - 1); if( _temp4444 < _temp4442.base? 1: _temp4444 >= _temp4442.last_plus_one){
_throw( Null_Exception);}* _temp4444;}).first_line,({ struct _tagged_ptr3
_temp4445= yyls; struct Cyc_Yyltype* _temp4447= _temp4445.curr + yylsp_offset;
if( _temp4447 < _temp4445.base? 1: _temp4447 >= _temp4445.last_plus_one){ _throw(
Null_Exception);}* _temp4447;}).last_line))};( struct _xenum_struct*) _temp4437;});
break; case 208: _LL4436: yyval=({ struct Cyc_Stmt_tok_struct* _temp4449=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4449=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_new_stmt(({
struct Cyc_Absyn_Splice_s_struct* _temp4450=( struct Cyc_Absyn_Splice_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Splice_s_struct)); _temp4450->tag= Cyc_Absyn_Splice_s_tag;
_temp4450->f1= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4451= yyvs; struct
_xenum_struct** _temp4453= _temp4451.curr + yyvsp_offset; if( _temp4453 <
_temp4451.base? 1: _temp4453 >= _temp4451.last_plus_one){ _throw( Null_Exception);}*
_temp4453;}));( void*) _temp4450;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4454= yyls; struct Cyc_Yyltype* _temp4456= _temp4454.curr +(
yylsp_offset - 1); if( _temp4456 < _temp4454.base? 1: _temp4456 >= _temp4454.last_plus_one){
_throw( Null_Exception);}* _temp4456;}).first_line,({ struct _tagged_ptr3
_temp4457= yyls; struct Cyc_Yyltype* _temp4459= _temp4457.curr + yylsp_offset;
if( _temp4459 < _temp4457.base? 1: _temp4459 >= _temp4457.last_plus_one){ _throw(
Null_Exception);}* _temp4459;}).last_line))};( struct _xenum_struct*) _temp4449;});
break; case 209: _LL4448: yyval=({ struct Cyc_Stmt_tok_struct* _temp4461=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4461=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_new_stmt(({
struct Cyc_Absyn_Label_s_struct* _temp4462=( struct Cyc_Absyn_Label_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Label_s_struct)); _temp4462->tag= Cyc_Absyn_Label_s_tag;
_temp4462->f1=({ struct _tagged_string* _temp4463=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp4463[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp4464= yyvs; struct _xenum_struct** _temp4466= _temp4464.curr
+( yyvsp_offset - 2); if( _temp4466 < _temp4464.base? 1: _temp4466 >= _temp4464.last_plus_one){
_throw( Null_Exception);}* _temp4466;})); _temp4463;}); _temp4462->f2= Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp4467= yyvs; struct _xenum_struct** _temp4469= _temp4467.curr
+ yyvsp_offset; if( _temp4469 < _temp4467.base? 1: _temp4469 >= _temp4467.last_plus_one){
_throw( Null_Exception);}* _temp4469;}));( void*) _temp4462;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4470= yyls; struct Cyc_Yyltype* _temp4472= _temp4470.curr
+( yylsp_offset - 2); if( _temp4472 < _temp4470.base? 1: _temp4472 >= _temp4470.last_plus_one){
_throw( Null_Exception);}* _temp4472;}).first_line,({ struct _tagged_ptr3
_temp4473= yyls; struct Cyc_Yyltype* _temp4475= _temp4473.curr + yylsp_offset;
if( _temp4475 < _temp4473.base? 1: _temp4475 >= _temp4473.last_plus_one){ _throw(
Null_Exception);}* _temp4475;}).last_line))};( struct _xenum_struct*) _temp4461;});
break; case 210: _LL4460: yyval=({ struct Cyc_Stmt_tok_struct* _temp4477=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4477=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4478= yyls; struct Cyc_Yyltype*
_temp4480= _temp4478.curr + yylsp_offset; if( _temp4480 < _temp4478.base? 1:
_temp4480 >= _temp4478.last_plus_one){ _throw( Null_Exception);}* _temp4480;}).first_line,({
struct _tagged_ptr3 _temp4481= yyls; struct Cyc_Yyltype* _temp4483= _temp4481.curr
+ yylsp_offset; if( _temp4483 < _temp4481.base? 1: _temp4483 >= _temp4481.last_plus_one){
_throw( Null_Exception);}* _temp4483;}).last_line))};( struct _xenum_struct*)
_temp4477;}); break; case 211: _LL4476: yyval=({ struct Cyc_Stmt_tok_struct*
_temp4485=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4485=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_exp_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4486= yyvs; struct _xenum_struct**
_temp4488= _temp4486.curr +( yyvsp_offset - 1); if( _temp4488 < _temp4486.base?
1: _temp4488 >= _temp4486.last_plus_one){ _throw( Null_Exception);}* _temp4488;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4489= yyls; struct Cyc_Yyltype*
_temp4491= _temp4489.curr +( yylsp_offset - 1); if( _temp4491 < _temp4489.base?
1: _temp4491 >= _temp4489.last_plus_one){ _throw( Null_Exception);}* _temp4491;}).first_line,({
struct _tagged_ptr3 _temp4492= yyls; struct Cyc_Yyltype* _temp4494= _temp4492.curr
+ yylsp_offset; if( _temp4494 < _temp4492.base? 1: _temp4494 >= _temp4492.last_plus_one){
_throw( Null_Exception);}* _temp4494;}).last_line))};( struct _xenum_struct*)
_temp4485;}); break; case 212: _LL4484: yyval=({ struct Cyc_Stmt_tok_struct*
_temp4496=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4496=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4497= yyls; struct Cyc_Yyltype*
_temp4499= _temp4497.curr +( yylsp_offset - 1); if( _temp4499 < _temp4497.base?
1: _temp4499 >= _temp4497.last_plus_one){ _throw( Null_Exception);}* _temp4499;}).first_line,({
struct _tagged_ptr3 _temp4500= yyls; struct Cyc_Yyltype* _temp4502= _temp4500.curr
+ yylsp_offset; if( _temp4502 < _temp4500.base? 1: _temp4502 >= _temp4500.last_plus_one){
_throw( Null_Exception);}* _temp4502;}).last_line))};( struct _xenum_struct*)
_temp4496;}); break; case 213: _LL4495: yyval=({ struct _tagged_ptr2 _temp4504=
yyvs; struct _xenum_struct** _temp4506= _temp4504.curr +( yyvsp_offset - 1); if(
_temp4506 < _temp4504.base? 1: _temp4506 >= _temp4504.last_plus_one){ _throw(
Null_Exception);}* _temp4506;}); break; case 214: _LL4503:{ void* _temp4511= Cyc_yyget_BlockItem_tok(({
struct _tagged_ptr2 _temp4508= yyvs; struct _xenum_struct** _temp4510= _temp4508.curr
+ yyvsp_offset; if( _temp4510 < _temp4508.base? 1: _temp4510 >= _temp4508.last_plus_one){
_throw( Null_Exception);}* _temp4510;})); struct Cyc_List_List* _temp4519;
struct Cyc_Absyn_Fndecl* _temp4521; struct Cyc_Absyn_Stmt* _temp4523; _LL4513:
if((( struct _enum_struct*) _temp4511)->tag == Cyc_Parse_TopDecls_bl_tag){
_LL4520: _temp4519=( struct Cyc_List_List*)(( struct Cyc_Parse_TopDecls_bl_struct*)
_temp4511)->f1; goto _LL4514;} else{ goto _LL4515;} _LL4515: if((( struct
_enum_struct*) _temp4511)->tag == Cyc_Parse_FnDecl_bl_tag){ _LL4522: _temp4521=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Parse_FnDecl_bl_struct*) _temp4511)->f1;
goto _LL4516;} else{ goto _LL4517;} _LL4517: if((( struct _enum_struct*)
_temp4511)->tag == Cyc_Parse_Stmt_bl_tag){ _LL4524: _temp4523=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Parse_Stmt_bl_struct*) _temp4511)->f1; goto _LL4518;} else{ goto
_LL4512;} _LL4514: yyval=({ struct Cyc_Stmt_tok_struct* _temp4525=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));* _temp4525=( struct Cyc_Stmt_tok_struct){.tag=
Cyc_Stmt_tok_tag,.f1= Cyc_Parse_flatten_declarations( _temp4519, Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4526= yyls; struct Cyc_Yyltype*
_temp4528= _temp4526.curr + yylsp_offset; if( _temp4528 < _temp4526.base? 1:
_temp4528 >= _temp4526.last_plus_one){ _throw( Null_Exception);}* _temp4528;}).first_line,({
struct _tagged_ptr3 _temp4529= yyls; struct Cyc_Yyltype* _temp4531= _temp4529.curr
+ yylsp_offset; if( _temp4531 < _temp4529.base? 1: _temp4531 >= _temp4529.last_plus_one){
_throw( Null_Exception);}* _temp4531;}).last_line)))};( struct _xenum_struct*)
_temp4525;}); goto _LL4512; _LL4516: yyval=({ struct Cyc_Stmt_tok_struct*
_temp4532=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4532=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(({ struct Cyc_Absyn_Fn_d_struct* _temp4533=( struct Cyc_Absyn_Fn_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct)); _temp4533->tag= Cyc_Absyn_Fn_d_tag;
_temp4533->f1= _temp4521;( void*) _temp4533;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4534= yyls; struct Cyc_Yyltype* _temp4536= _temp4534.curr
+ yylsp_offset; if( _temp4536 < _temp4534.base? 1: _temp4536 >= _temp4534.last_plus_one){
_throw( Null_Exception);}* _temp4536;}).first_line,({ struct _tagged_ptr3
_temp4537= yyls; struct Cyc_Yyltype* _temp4539= _temp4537.curr + yylsp_offset;
if( _temp4539 < _temp4537.base? 1: _temp4539 >= _temp4537.last_plus_one){ _throw(
Null_Exception);}* _temp4539;}).last_line)), Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4540= yyls; struct Cyc_Yyltype* _temp4542= _temp4540.curr
+ yylsp_offset; if( _temp4542 < _temp4540.base? 1: _temp4542 >= _temp4540.last_plus_one){
_throw( Null_Exception);}* _temp4542;}).first_line,({ struct _tagged_ptr3
_temp4543= yyls; struct Cyc_Yyltype* _temp4545= _temp4543.curr + yylsp_offset;
if( _temp4545 < _temp4543.base? 1: _temp4545 >= _temp4543.last_plus_one){ _throw(
Null_Exception);}* _temp4545;}).last_line)))};( struct _xenum_struct*) _temp4532;});
goto _LL4512; _LL4518: yyval=({ struct Cyc_Stmt_tok_struct* _temp4546=( struct
Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4546=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= _temp4523};(
struct _xenum_struct*) _temp4546;}); goto _LL4512; _LL4512:;} break; case 215:
_LL4507:{ void* _temp4551= Cyc_yyget_BlockItem_tok(({ struct _tagged_ptr2
_temp4548= yyvs; struct _xenum_struct** _temp4550= _temp4548.curr +(
yyvsp_offset - 1); if( _temp4550 < _temp4548.base? 1: _temp4550 >= _temp4548.last_plus_one){
_throw( Null_Exception);}* _temp4550;})); struct Cyc_List_List* _temp4559;
struct Cyc_Absyn_Fndecl* _temp4561; struct Cyc_Absyn_Stmt* _temp4563; _LL4553:
if((( struct _enum_struct*) _temp4551)->tag == Cyc_Parse_TopDecls_bl_tag){
_LL4560: _temp4559=( struct Cyc_List_List*)(( struct Cyc_Parse_TopDecls_bl_struct*)
_temp4551)->f1; goto _LL4554;} else{ goto _LL4555;} _LL4555: if((( struct
_enum_struct*) _temp4551)->tag == Cyc_Parse_FnDecl_bl_tag){ _LL4562: _temp4561=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Parse_FnDecl_bl_struct*) _temp4551)->f1;
goto _LL4556;} else{ goto _LL4557;} _LL4557: if((( struct _enum_struct*)
_temp4551)->tag == Cyc_Parse_Stmt_bl_tag){ _LL4564: _temp4563=( struct Cyc_Absyn_Stmt*)((
struct Cyc_Parse_Stmt_bl_struct*) _temp4551)->f1; goto _LL4558;} else{ goto
_LL4552;} _LL4554: yyval=({ struct Cyc_Stmt_tok_struct* _temp4565=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));* _temp4565=( struct Cyc_Stmt_tok_struct){.tag=
Cyc_Stmt_tok_tag,.f1= Cyc_Parse_flatten_declarations( _temp4559, Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp4566= yyvs; struct _xenum_struct** _temp4568= _temp4566.curr
+ yyvsp_offset; if( _temp4568 < _temp4566.base? 1: _temp4568 >= _temp4566.last_plus_one){
_throw( Null_Exception);}* _temp4568;})))};( struct _xenum_struct*) _temp4565;});
goto _LL4552; _LL4556: yyval=({ struct Cyc_Stmt_tok_struct* _temp4569=( struct
Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4569=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl(({ struct Cyc_Absyn_Fn_d_struct* _temp4570=( struct Cyc_Absyn_Fn_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fn_d_struct)); _temp4570->tag= Cyc_Absyn_Fn_d_tag;
_temp4570->f1= _temp4561;( void*) _temp4570;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4571= yyls; struct Cyc_Yyltype* _temp4573= _temp4571.curr
+( yylsp_offset - 1); if( _temp4573 < _temp4571.base? 1: _temp4573 >= _temp4571.last_plus_one){
_throw( Null_Exception);}* _temp4573;}).first_line,({ struct _tagged_ptr3
_temp4574= yyls; struct Cyc_Yyltype* _temp4576= _temp4574.curr +( yylsp_offset -
1); if( _temp4576 < _temp4574.base? 1: _temp4576 >= _temp4574.last_plus_one){
_throw( Null_Exception);}* _temp4576;}).last_line)), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp4577= yyvs; struct _xenum_struct** _temp4579= _temp4577.curr
+ yyvsp_offset; if( _temp4579 < _temp4577.base? 1: _temp4579 >= _temp4577.last_plus_one){
_throw( Null_Exception);}* _temp4579;})))};( struct _xenum_struct*) _temp4569;});
goto _LL4552; _LL4558: yyval=({ struct Cyc_Stmt_tok_struct* _temp4580=( struct
Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4580=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_seq_stmt(
_temp4563, Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4581= yyvs; struct
_xenum_struct** _temp4583= _temp4581.curr + yyvsp_offset; if( _temp4583 <
_temp4581.base? 1: _temp4583 >= _temp4581.last_plus_one){ _throw( Null_Exception);}*
_temp4583;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4584=
yyls; struct Cyc_Yyltype* _temp4586= _temp4584.curr +( yylsp_offset - 1); if(
_temp4586 < _temp4584.base? 1: _temp4586 >= _temp4584.last_plus_one){ _throw(
Null_Exception);}* _temp4586;}).first_line,({ struct _tagged_ptr3 _temp4587=
yyls; struct Cyc_Yyltype* _temp4589= _temp4587.curr + yylsp_offset; if(
_temp4589 < _temp4587.base? 1: _temp4589 >= _temp4587.last_plus_one){ _throw(
Null_Exception);}* _temp4589;}).last_line))};( struct _xenum_struct*) _temp4580;});
goto _LL4552; _LL4552:;} break; case 216: _LL4547: yyval=({ struct Cyc_BlockItem_tok_struct*
_temp4591=( struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));*
_temp4591=( struct Cyc_BlockItem_tok_struct){.tag= Cyc_BlockItem_tok_tag,.f1=({
struct Cyc_Parse_TopDecls_bl_struct* _temp4592=( struct Cyc_Parse_TopDecls_bl_struct*)
GC_malloc( sizeof( struct Cyc_Parse_TopDecls_bl_struct)); _temp4592->tag= Cyc_Parse_TopDecls_bl_tag;
_temp4592->f1= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2 _temp4593= yyvs;
struct _xenum_struct** _temp4595= _temp4593.curr + yyvsp_offset; if( _temp4595 <
_temp4593.base? 1: _temp4595 >= _temp4593.last_plus_one){ _throw( Null_Exception);}*
_temp4595;}));( void*) _temp4592;})};( struct _xenum_struct*) _temp4591;});
break; case 217: _LL4590: yyval=({ struct Cyc_BlockItem_tok_struct* _temp4597=(
struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));*
_temp4597=( struct Cyc_BlockItem_tok_struct){.tag= Cyc_BlockItem_tok_tag,.f1=({
struct Cyc_Parse_Stmt_bl_struct* _temp4598=( struct Cyc_Parse_Stmt_bl_struct*)
GC_malloc( sizeof( struct Cyc_Parse_Stmt_bl_struct)); _temp4598->tag= Cyc_Parse_Stmt_bl_tag;
_temp4598->f1= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4599= yyvs; struct
_xenum_struct** _temp4601= _temp4599.curr + yyvsp_offset; if( _temp4601 <
_temp4599.base? 1: _temp4601 >= _temp4599.last_plus_one){ _throw( Null_Exception);}*
_temp4601;}));( void*) _temp4598;})};( struct _xenum_struct*) _temp4597;});
break; case 218: _LL4596: yyval=({ struct Cyc_BlockItem_tok_struct* _temp4603=(
struct Cyc_BlockItem_tok_struct*) GC_malloc( sizeof( struct Cyc_BlockItem_tok_struct));*
_temp4603=( struct Cyc_BlockItem_tok_struct){.tag= Cyc_BlockItem_tok_tag,.f1=({
struct Cyc_Parse_FnDecl_bl_struct* _temp4604=( struct Cyc_Parse_FnDecl_bl_struct*)
GC_malloc( sizeof( struct Cyc_Parse_FnDecl_bl_struct)); _temp4604->tag= Cyc_Parse_FnDecl_bl_tag;
_temp4604->f1= Cyc_yyget_FnDecl_tok(({ struct _tagged_ptr2 _temp4605= yyvs;
struct _xenum_struct** _temp4607= _temp4605.curr + yyvsp_offset; if( _temp4607 <
_temp4605.base? 1: _temp4607 >= _temp4605.last_plus_one){ _throw( Null_Exception);}*
_temp4607;}));( void*) _temp4604;})};( struct _xenum_struct*) _temp4603;});
break; case 219: _LL4602: yyval=({ struct Cyc_Stmt_tok_struct* _temp4609=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4609=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4610= yyvs; struct _xenum_struct**
_temp4612= _temp4610.curr +( yyvsp_offset - 2); if( _temp4612 < _temp4610.base?
1: _temp4612 >= _temp4610.last_plus_one){ _throw( Null_Exception);}* _temp4612;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4613= yyvs; struct _xenum_struct**
_temp4615= _temp4613.curr + yyvsp_offset; if( _temp4615 < _temp4613.base? 1:
_temp4615 >= _temp4613.last_plus_one){ _throw( Null_Exception);}* _temp4615;})),
Cyc_Absyn_skip_stmt( 0), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp4616= yyls; struct Cyc_Yyltype* _temp4618= _temp4616.curr +( yylsp_offset -
4); if( _temp4618 < _temp4616.base? 1: _temp4618 >= _temp4616.last_plus_one){
_throw( Null_Exception);}* _temp4618;}).first_line,({ struct _tagged_ptr3
_temp4619= yyls; struct Cyc_Yyltype* _temp4621= _temp4619.curr + yylsp_offset;
if( _temp4621 < _temp4619.base? 1: _temp4621 >= _temp4619.last_plus_one){ _throw(
Null_Exception);}* _temp4621;}).last_line))};( struct _xenum_struct*) _temp4609;});
break; case 220: _LL4608: yyval=({ struct Cyc_Stmt_tok_struct* _temp4623=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4623=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4624= yyvs; struct _xenum_struct**
_temp4626= _temp4624.curr +( yyvsp_offset - 4); if( _temp4626 < _temp4624.base?
1: _temp4626 >= _temp4624.last_plus_one){ _throw( Null_Exception);}* _temp4626;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4627= yyvs; struct _xenum_struct**
_temp4629= _temp4627.curr +( yyvsp_offset - 2); if( _temp4629 < _temp4627.base?
1: _temp4629 >= _temp4627.last_plus_one){ _throw( Null_Exception);}* _temp4629;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4630= yyvs; struct _xenum_struct**
_temp4632= _temp4630.curr + yyvsp_offset; if( _temp4632 < _temp4630.base? 1:
_temp4632 >= _temp4630.last_plus_one){ _throw( Null_Exception);}* _temp4632;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4633= yyls; struct Cyc_Yyltype*
_temp4635= _temp4633.curr +( yylsp_offset - 6); if( _temp4635 < _temp4633.base?
1: _temp4635 >= _temp4633.last_plus_one){ _throw( Null_Exception);}* _temp4635;}).first_line,({
struct _tagged_ptr3 _temp4636= yyls; struct Cyc_Yyltype* _temp4638= _temp4636.curr
+ yylsp_offset; if( _temp4638 < _temp4636.base? 1: _temp4638 >= _temp4636.last_plus_one){
_throw( Null_Exception);}* _temp4638;}).last_line))};( struct _xenum_struct*)
_temp4623;}); break; case 221: _LL4622: yyval=({ struct Cyc_Stmt_tok_struct*
_temp4640=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4640=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_switch_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4641= yyvs; struct _xenum_struct**
_temp4643= _temp4641.curr +( yyvsp_offset - 4); if( _temp4643 < _temp4641.base?
1: _temp4643 >= _temp4641.last_plus_one){ _throw( Null_Exception);}* _temp4643;})),
Cyc_yyget_SwitchClauseList_tok(({ struct _tagged_ptr2 _temp4644= yyvs; struct
_xenum_struct** _temp4646= _temp4644.curr +( yyvsp_offset - 1); if( _temp4646 <
_temp4644.base? 1: _temp4646 >= _temp4644.last_plus_one){ _throw( Null_Exception);}*
_temp4646;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4647=
yyls; struct Cyc_Yyltype* _temp4649= _temp4647.curr +( yylsp_offset - 6); if(
_temp4649 < _temp4647.base? 1: _temp4649 >= _temp4647.last_plus_one){ _throw(
Null_Exception);}* _temp4649;}).first_line,({ struct _tagged_ptr3 _temp4650=
yyls; struct Cyc_Yyltype* _temp4652= _temp4650.curr + yylsp_offset; if(
_temp4652 < _temp4650.base? 1: _temp4652 >= _temp4650.last_plus_one){ _throw(
Null_Exception);}* _temp4652;}).last_line))};( struct _xenum_struct*) _temp4640;});
break; case 222: _LL4639: yyval=({ struct Cyc_Stmt_tok_struct* _temp4654=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4654=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_trycatch_stmt(
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4655= yyvs; struct _xenum_struct**
_temp4657= _temp4655.curr +( yyvsp_offset - 4); if( _temp4657 < _temp4655.base?
1: _temp4657 >= _temp4655.last_plus_one){ _throw( Null_Exception);}* _temp4657;})),
Cyc_yyget_SwitchClauseList_tok(({ struct _tagged_ptr2 _temp4658= yyvs; struct
_xenum_struct** _temp4660= _temp4658.curr +( yyvsp_offset - 1); if( _temp4660 <
_temp4658.base? 1: _temp4660 >= _temp4658.last_plus_one){ _throw( Null_Exception);}*
_temp4660;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4661=
yyls; struct Cyc_Yyltype* _temp4663= _temp4661.curr +( yylsp_offset - 5); if(
_temp4663 < _temp4661.base? 1: _temp4663 >= _temp4661.last_plus_one){ _throw(
Null_Exception);}* _temp4663;}).first_line,({ struct _tagged_ptr3 _temp4664=
yyls; struct Cyc_Yyltype* _temp4666= _temp4664.curr + yylsp_offset; if(
_temp4666 < _temp4664.base? 1: _temp4666 >= _temp4664.last_plus_one){ _throw(
Null_Exception);}* _temp4666;}).last_line))};( struct _xenum_struct*) _temp4654;});
break; case 223: _LL4653: yyval=({ struct Cyc_SwitchClauseList_tok_struct*
_temp4668=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct));* _temp4668=( struct Cyc_SwitchClauseList_tok_struct){.tag=
Cyc_SwitchClauseList_tok_tag,.f1= 0};( struct _xenum_struct*) _temp4668;});
break; case 224: _LL4667: yyval=({ struct Cyc_SwitchClauseList_tok_struct*
_temp4670=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct));* _temp4670=( struct Cyc_SwitchClauseList_tok_struct){.tag=
Cyc_SwitchClauseList_tok_tag,.f1=({ struct Cyc_List_List* _temp4671=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4671->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp4672=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp4672->pattern= Cyc_Absyn_new_pat( Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4682= yyls; struct Cyc_Yyltype* _temp4684= _temp4682.curr
+( yylsp_offset - 2); if( _temp4684 < _temp4682.base? 1: _temp4684 >= _temp4682.last_plus_one){
_throw( Null_Exception);}* _temp4684;}).first_line,({ struct _tagged_ptr3
_temp4685= yyls; struct Cyc_Yyltype* _temp4687= _temp4685.curr +( yylsp_offset -
2); if( _temp4687 < _temp4685.base? 1: _temp4687 >= _temp4685.last_plus_one){
_throw( Null_Exception);}* _temp4687;}).last_line)); _temp4672->pat_vars= 0;
_temp4672->where_clause= 0; _temp4672->body= Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp4679= yyvs; struct _xenum_struct** _temp4681= _temp4679.curr +
yyvsp_offset; if( _temp4681 < _temp4679.base? 1: _temp4681 >= _temp4679.last_plus_one){
_throw( Null_Exception);}* _temp4681;})); _temp4672->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4673= yyls; struct Cyc_Yyltype* _temp4675= _temp4673.curr
+( yylsp_offset - 2); if( _temp4675 < _temp4673.base? 1: _temp4675 >= _temp4673.last_plus_one){
_throw( Null_Exception);}* _temp4675;}).first_line,({ struct _tagged_ptr3
_temp4676= yyls; struct Cyc_Yyltype* _temp4678= _temp4676.curr + yylsp_offset;
if( _temp4678 < _temp4676.base? 1: _temp4678 >= _temp4676.last_plus_one){ _throw(
Null_Exception);}* _temp4678;}).last_line); _temp4672;}); _temp4671->tl= 0;
_temp4671;})};( struct _xenum_struct*) _temp4670;}); break; case 225: _LL4669:
yyval=({ struct Cyc_SwitchClauseList_tok_struct* _temp4689=( struct Cyc_SwitchClauseList_tok_struct*)
GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));* _temp4689=( struct
Cyc_SwitchClauseList_tok_struct){.tag= Cyc_SwitchClauseList_tok_tag,.f1=({
struct Cyc_List_List* _temp4690=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4690->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp4694=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp4694->pattern= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp4707= yyvs;
struct _xenum_struct** _temp4709= _temp4707.curr +( yyvsp_offset - 2); if(
_temp4709 < _temp4707.base? 1: _temp4709 >= _temp4707.last_plus_one){ _throw(
Null_Exception);}* _temp4709;})); _temp4694->pat_vars= 0; _temp4694->where_clause=
0; _temp4694->body= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4701= yyls; struct Cyc_Yyltype* _temp4703= _temp4701.curr +(
yylsp_offset - 1); if( _temp4703 < _temp4701.base? 1: _temp4703 >= _temp4701.last_plus_one){
_throw( Null_Exception);}* _temp4703;}).first_line,({ struct _tagged_ptr3
_temp4704= yyls; struct Cyc_Yyltype* _temp4706= _temp4704.curr +( yylsp_offset -
1); if( _temp4706 < _temp4704.base? 1: _temp4706 >= _temp4704.last_plus_one){
_throw( Null_Exception);}* _temp4706;}).last_line)); _temp4694->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4695= yyls; struct Cyc_Yyltype* _temp4697= _temp4695.curr
+( yylsp_offset - 3); if( _temp4697 < _temp4695.base? 1: _temp4697 >= _temp4695.last_plus_one){
_throw( Null_Exception);}* _temp4697;}).first_line,({ struct _tagged_ptr3
_temp4698= yyls; struct Cyc_Yyltype* _temp4700= _temp4698.curr + yylsp_offset;
if( _temp4700 < _temp4698.base? 1: _temp4700 >= _temp4698.last_plus_one){ _throw(
Null_Exception);}* _temp4700;}).last_line); _temp4694;}); _temp4690->tl= Cyc_yyget_SwitchClauseList_tok(({
struct _tagged_ptr2 _temp4691= yyvs; struct _xenum_struct** _temp4693= _temp4691.curr
+ yyvsp_offset; if( _temp4693 < _temp4691.base? 1: _temp4693 >= _temp4691.last_plus_one){
_throw( Null_Exception);}* _temp4693;})); _temp4690;})};( struct _xenum_struct*)
_temp4689;}); break; case 226: _LL4688: yyval=({ struct Cyc_SwitchClauseList_tok_struct*
_temp4711=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct));* _temp4711=( struct Cyc_SwitchClauseList_tok_struct){.tag=
Cyc_SwitchClauseList_tok_tag,.f1=({ struct Cyc_List_List* _temp4712=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4712->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp4716=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp4716->pattern= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp4726= yyvs;
struct _xenum_struct** _temp4728= _temp4726.curr +( yyvsp_offset - 3); if(
_temp4728 < _temp4726.base? 1: _temp4728 >= _temp4726.last_plus_one){ _throw(
Null_Exception);}* _temp4728;})); _temp4716->pat_vars= 0; _temp4716->where_clause=
0; _temp4716->body= Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4723= yyvs;
struct _xenum_struct** _temp4725= _temp4723.curr +( yyvsp_offset - 1); if(
_temp4725 < _temp4723.base? 1: _temp4725 >= _temp4723.last_plus_one){ _throw(
Null_Exception);}* _temp4725;})); _temp4716->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4717= yyls; struct Cyc_Yyltype* _temp4719= _temp4717.curr
+( yylsp_offset - 4); if( _temp4719 < _temp4717.base? 1: _temp4719 >= _temp4717.last_plus_one){
_throw( Null_Exception);}* _temp4719;}).first_line,({ struct _tagged_ptr3
_temp4720= yyls; struct Cyc_Yyltype* _temp4722= _temp4720.curr +( yylsp_offset -
1); if( _temp4722 < _temp4720.base? 1: _temp4722 >= _temp4720.last_plus_one){
_throw( Null_Exception);}* _temp4722;}).last_line); _temp4716;}); _temp4712->tl=
Cyc_yyget_SwitchClauseList_tok(({ struct _tagged_ptr2 _temp4713= yyvs; struct
_xenum_struct** _temp4715= _temp4713.curr + yyvsp_offset; if( _temp4715 <
_temp4713.base? 1: _temp4715 >= _temp4713.last_plus_one){ _throw( Null_Exception);}*
_temp4715;})); _temp4712;})};( struct _xenum_struct*) _temp4711;}); break; case
227: _LL4710: yyval=({ struct Cyc_SwitchClauseList_tok_struct* _temp4730=(
struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct Cyc_SwitchClauseList_tok_struct));*
_temp4730=( struct Cyc_SwitchClauseList_tok_struct){.tag= Cyc_SwitchClauseList_tok_tag,.f1=({
struct Cyc_List_List* _temp4731=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp4731->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp4735=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp4735->pattern= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp4751= yyvs;
struct _xenum_struct** _temp4753= _temp4751.curr +( yyvsp_offset - 4); if(
_temp4753 < _temp4751.base? 1: _temp4753 >= _temp4751.last_plus_one){ _throw(
Null_Exception);}* _temp4753;})); _temp4735->pat_vars= 0; _temp4735->where_clause=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4748= yyvs;
struct _xenum_struct** _temp4750= _temp4748.curr +( yyvsp_offset - 2); if(
_temp4750 < _temp4748.base? 1: _temp4750 >= _temp4748.last_plus_one){ _throw(
Null_Exception);}* _temp4750;})); _temp4735->body= Cyc_Absyn_skip_stmt( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4742= yyls; struct Cyc_Yyltype* _temp4744= _temp4742.curr
+( yylsp_offset - 1); if( _temp4744 < _temp4742.base? 1: _temp4744 >= _temp4742.last_plus_one){
_throw( Null_Exception);}* _temp4744;}).first_line,({ struct _tagged_ptr3
_temp4745= yyls; struct Cyc_Yyltype* _temp4747= _temp4745.curr +( yylsp_offset -
1); if( _temp4747 < _temp4745.base? 1: _temp4747 >= _temp4745.last_plus_one){
_throw( Null_Exception);}* _temp4747;}).last_line)); _temp4735->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4736= yyls; struct Cyc_Yyltype* _temp4738= _temp4736.curr
+( yylsp_offset - 5); if( _temp4738 < _temp4736.base? 1: _temp4738 >= _temp4736.last_plus_one){
_throw( Null_Exception);}* _temp4738;}).first_line,({ struct _tagged_ptr3
_temp4739= yyls; struct Cyc_Yyltype* _temp4741= _temp4739.curr + yylsp_offset;
if( _temp4741 < _temp4739.base? 1: _temp4741 >= _temp4739.last_plus_one){ _throw(
Null_Exception);}* _temp4741;}).last_line); _temp4735;}); _temp4731->tl= Cyc_yyget_SwitchClauseList_tok(({
struct _tagged_ptr2 _temp4732= yyvs; struct _xenum_struct** _temp4734= _temp4732.curr
+ yyvsp_offset; if( _temp4734 < _temp4732.base? 1: _temp4734 >= _temp4732.last_plus_one){
_throw( Null_Exception);}* _temp4734;})); _temp4731;})};( struct _xenum_struct*)
_temp4730;}); break; case 228: _LL4729: yyval=({ struct Cyc_SwitchClauseList_tok_struct*
_temp4755=( struct Cyc_SwitchClauseList_tok_struct*) GC_malloc( sizeof( struct
Cyc_SwitchClauseList_tok_struct));* _temp4755=( struct Cyc_SwitchClauseList_tok_struct){.tag=
Cyc_SwitchClauseList_tok_tag,.f1=({ struct Cyc_List_List* _temp4756=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp4756->hd=( void*)({ struct Cyc_Absyn_Switch_clause*
_temp4760=( struct Cyc_Absyn_Switch_clause*) GC_malloc( sizeof( struct Cyc_Absyn_Switch_clause));
_temp4760->pattern= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp4773= yyvs;
struct _xenum_struct** _temp4775= _temp4773.curr +( yyvsp_offset - 5); if(
_temp4775 < _temp4773.base? 1: _temp4775 >= _temp4773.last_plus_one){ _throw(
Null_Exception);}* _temp4775;})); _temp4760->pat_vars= 0; _temp4760->where_clause=(
struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4770= yyvs;
struct _xenum_struct** _temp4772= _temp4770.curr +( yyvsp_offset - 3); if(
_temp4772 < _temp4770.base? 1: _temp4772 >= _temp4770.last_plus_one){ _throw(
Null_Exception);}* _temp4772;})); _temp4760->body= Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp4767= yyvs; struct _xenum_struct** _temp4769= _temp4767.curr +(
yyvsp_offset - 1); if( _temp4769 < _temp4767.base? 1: _temp4769 >= _temp4767.last_plus_one){
_throw( Null_Exception);}* _temp4769;})); _temp4760->loc= Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp4761= yyls; struct Cyc_Yyltype* _temp4763= _temp4761.curr
+( yylsp_offset - 6); if( _temp4763 < _temp4761.base? 1: _temp4763 >= _temp4761.last_plus_one){
_throw( Null_Exception);}* _temp4763;}).first_line,({ struct _tagged_ptr3
_temp4764= yyls; struct Cyc_Yyltype* _temp4766= _temp4764.curr + yylsp_offset;
if( _temp4766 < _temp4764.base? 1: _temp4766 >= _temp4764.last_plus_one){ _throw(
Null_Exception);}* _temp4766;}).last_line); _temp4760;}); _temp4756->tl= Cyc_yyget_SwitchClauseList_tok(({
struct _tagged_ptr2 _temp4757= yyvs; struct _xenum_struct** _temp4759= _temp4757.curr
+ yyvsp_offset; if( _temp4759 < _temp4757.base? 1: _temp4759 >= _temp4757.last_plus_one){
_throw( Null_Exception);}* _temp4759;})); _temp4756;})};( struct _xenum_struct*)
_temp4755;}); break; case 229: _LL4754: yyval=({ struct Cyc_Stmt_tok_struct*
_temp4777=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4777=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_while_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4778= yyvs; struct _xenum_struct**
_temp4780= _temp4778.curr +( yyvsp_offset - 2); if( _temp4780 < _temp4778.base?
1: _temp4780 >= _temp4778.last_plus_one){ _throw( Null_Exception);}* _temp4780;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4781= yyvs; struct _xenum_struct**
_temp4783= _temp4781.curr + yyvsp_offset; if( _temp4783 < _temp4781.base? 1:
_temp4783 >= _temp4781.last_plus_one){ _throw( Null_Exception);}* _temp4783;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4784= yyls; struct Cyc_Yyltype*
_temp4786= _temp4784.curr +( yylsp_offset - 4); if( _temp4786 < _temp4784.base?
1: _temp4786 >= _temp4784.last_plus_one){ _throw( Null_Exception);}* _temp4786;}).first_line,({
struct _tagged_ptr3 _temp4787= yyls; struct Cyc_Yyltype* _temp4789= _temp4787.curr
+ yylsp_offset; if( _temp4789 < _temp4787.base? 1: _temp4789 >= _temp4787.last_plus_one){
_throw( Null_Exception);}* _temp4789;}).last_line))};( struct _xenum_struct*)
_temp4777;}); break; case 230: _LL4776: yyval=({ struct Cyc_Stmt_tok_struct*
_temp4791=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4791=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_do_stmt(
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4792= yyvs; struct _xenum_struct**
_temp4794= _temp4792.curr +( yyvsp_offset - 5); if( _temp4794 < _temp4792.base?
1: _temp4794 >= _temp4792.last_plus_one){ _throw( Null_Exception);}* _temp4794;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4795= yyvs; struct _xenum_struct**
_temp4797= _temp4795.curr +( yyvsp_offset - 2); if( _temp4797 < _temp4795.base?
1: _temp4797 >= _temp4795.last_plus_one){ _throw( Null_Exception);}* _temp4797;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4798= yyls; struct Cyc_Yyltype*
_temp4800= _temp4798.curr +( yylsp_offset - 6); if( _temp4800 < _temp4798.base?
1: _temp4800 >= _temp4798.last_plus_one){ _throw( Null_Exception);}* _temp4800;}).first_line,({
struct _tagged_ptr3 _temp4801= yyls; struct Cyc_Yyltype* _temp4803= _temp4801.curr
+ yylsp_offset; if( _temp4803 < _temp4801.base? 1: _temp4803 >= _temp4801.last_plus_one){
_throw( Null_Exception);}* _temp4803;}).last_line))};( struct _xenum_struct*)
_temp4791;}); break; case 231: _LL4790: yyval=({ struct Cyc_Stmt_tok_struct*
_temp4805=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4805=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp4806= yyvs; struct _xenum_struct** _temp4808= _temp4806.curr
+ yyvsp_offset; if( _temp4808 < _temp4806.base? 1: _temp4808 >= _temp4806.last_plus_one){
_throw( Null_Exception);}* _temp4808;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4809= yyls; struct Cyc_Yyltype* _temp4811= _temp4809.curr +(
yylsp_offset - 5); if( _temp4811 < _temp4809.base? 1: _temp4811 >= _temp4809.last_plus_one){
_throw( Null_Exception);}* _temp4811;}).first_line,({ struct _tagged_ptr3
_temp4812= yyls; struct Cyc_Yyltype* _temp4814= _temp4812.curr + yylsp_offset;
if( _temp4814 < _temp4812.base? 1: _temp4814 >= _temp4812.last_plus_one){ _throw(
Null_Exception);}* _temp4814;}).last_line))};( struct _xenum_struct*) _temp4805;});
break; case 232: _LL4804: yyval=({ struct Cyc_Stmt_tok_struct* _temp4816=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4816=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4817= yyvs; struct _xenum_struct** _temp4819= _temp4817.curr +(
yyvsp_offset - 2); if( _temp4819 < _temp4817.base? 1: _temp4819 >= _temp4817.last_plus_one){
_throw( Null_Exception);}* _temp4819;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp4820= yyvs; struct _xenum_struct** _temp4822= _temp4820.curr +
yyvsp_offset; if( _temp4822 < _temp4820.base? 1: _temp4822 >= _temp4820.last_plus_one){
_throw( Null_Exception);}* _temp4822;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4823= yyls; struct Cyc_Yyltype* _temp4825= _temp4823.curr +(
yylsp_offset - 6); if( _temp4825 < _temp4823.base? 1: _temp4825 >= _temp4823.last_plus_one){
_throw( Null_Exception);}* _temp4825;}).first_line,({ struct _tagged_ptr3
_temp4826= yyls; struct Cyc_Yyltype* _temp4828= _temp4826.curr + yylsp_offset;
if( _temp4828 < _temp4826.base? 1: _temp4828 >= _temp4826.last_plus_one){ _throw(
Null_Exception);}* _temp4828;}).last_line))};( struct _xenum_struct*) _temp4816;});
break; case 233: _LL4815: yyval=({ struct Cyc_Stmt_tok_struct* _temp4830=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4830=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4831=
yyvs; struct _xenum_struct** _temp4833= _temp4831.curr +( yyvsp_offset - 3); if(
_temp4833 < _temp4831.base? 1: _temp4833 >= _temp4831.last_plus_one){ _throw(
Null_Exception);}* _temp4833;})), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp4834= yyvs; struct _xenum_struct** _temp4836= _temp4834.curr
+ yyvsp_offset; if( _temp4836 < _temp4834.base? 1: _temp4836 >= _temp4834.last_plus_one){
_throw( Null_Exception);}* _temp4836;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4837= yyls; struct Cyc_Yyltype* _temp4839= _temp4837.curr +(
yylsp_offset - 6); if( _temp4839 < _temp4837.base? 1: _temp4839 >= _temp4837.last_plus_one){
_throw( Null_Exception);}* _temp4839;}).first_line,({ struct _tagged_ptr3
_temp4840= yyls; struct Cyc_Yyltype* _temp4842= _temp4840.curr + yylsp_offset;
if( _temp4842 < _temp4840.base? 1: _temp4842 >= _temp4840.last_plus_one){ _throw(
Null_Exception);}* _temp4842;}).last_line))};( struct _xenum_struct*) _temp4830;});
break; case 234: _LL4829: yyval=({ struct Cyc_Stmt_tok_struct* _temp4844=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4844=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4845=
yyvs; struct _xenum_struct** _temp4847= _temp4845.curr +( yyvsp_offset - 4); if(
_temp4847 < _temp4845.base? 1: _temp4847 >= _temp4845.last_plus_one){ _throw(
Null_Exception);}* _temp4847;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp4848= yyvs; struct _xenum_struct** _temp4850= _temp4848.curr +(
yyvsp_offset - 2); if( _temp4850 < _temp4848.base? 1: _temp4850 >= _temp4848.last_plus_one){
_throw( Null_Exception);}* _temp4850;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp4851= yyvs; struct _xenum_struct** _temp4853= _temp4851.curr +
yyvsp_offset; if( _temp4853 < _temp4851.base? 1: _temp4853 >= _temp4851.last_plus_one){
_throw( Null_Exception);}* _temp4853;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4854= yyls; struct Cyc_Yyltype* _temp4856= _temp4854.curr +(
yylsp_offset - 7); if( _temp4856 < _temp4854.base? 1: _temp4856 >= _temp4854.last_plus_one){
_throw( Null_Exception);}* _temp4856;}).first_line,({ struct _tagged_ptr3
_temp4857= yyls; struct Cyc_Yyltype* _temp4859= _temp4857.curr +( yylsp_offset -
1); if( _temp4859 < _temp4857.base? 1: _temp4859 >= _temp4857.last_plus_one){
_throw( Null_Exception);}* _temp4859;}).last_line))};( struct _xenum_struct*)
_temp4844;}); break; case 235: _LL4843: yyval=({ struct Cyc_Stmt_tok_struct*
_temp4861=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4861=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_for_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4862= yyvs; struct _xenum_struct**
_temp4864= _temp4862.curr +( yyvsp_offset - 4); if( _temp4864 < _temp4862.base?
1: _temp4864 >= _temp4862.last_plus_one){ _throw( Null_Exception);}* _temp4864;})),
Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp4865= yyvs; struct _xenum_struct** _temp4867= _temp4865.curr +
yyvsp_offset; if( _temp4867 < _temp4865.base? 1: _temp4867 >= _temp4865.last_plus_one){
_throw( Null_Exception);}* _temp4867;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4868= yyls; struct Cyc_Yyltype* _temp4870= _temp4868.curr +(
yylsp_offset - 6); if( _temp4870 < _temp4868.base? 1: _temp4870 >= _temp4868.last_plus_one){
_throw( Null_Exception);}* _temp4870;}).first_line,({ struct _tagged_ptr3
_temp4871= yyls; struct Cyc_Yyltype* _temp4873= _temp4871.curr + yylsp_offset;
if( _temp4873 < _temp4871.base? 1: _temp4873 >= _temp4871.last_plus_one){ _throw(
Null_Exception);}* _temp4873;}).last_line))};( struct _xenum_struct*) _temp4861;});
break; case 236: _LL4860: yyval=({ struct Cyc_Stmt_tok_struct* _temp4875=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4875=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_for_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4876= yyvs; struct _xenum_struct**
_temp4878= _temp4876.curr +( yyvsp_offset - 5); if( _temp4878 < _temp4876.base?
1: _temp4878 >= _temp4876.last_plus_one){ _throw( Null_Exception);}* _temp4878;})),
Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4879= yyvs;
struct _xenum_struct** _temp4881= _temp4879.curr +( yyvsp_offset - 2); if(
_temp4881 < _temp4879.base? 1: _temp4881 >= _temp4879.last_plus_one){ _throw(
Null_Exception);}* _temp4881;})), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2
_temp4882= yyvs; struct _xenum_struct** _temp4884= _temp4882.curr + yyvsp_offset;
if( _temp4884 < _temp4882.base? 1: _temp4884 >= _temp4882.last_plus_one){ _throw(
Null_Exception);}* _temp4884;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4885= yyls; struct Cyc_Yyltype* _temp4887= _temp4885.curr +(
yylsp_offset - 7); if( _temp4887 < _temp4885.base? 1: _temp4887 >= _temp4885.last_plus_one){
_throw( Null_Exception);}* _temp4887;}).first_line,({ struct _tagged_ptr3
_temp4888= yyls; struct Cyc_Yyltype* _temp4890= _temp4888.curr + yylsp_offset;
if( _temp4890 < _temp4888.base? 1: _temp4890 >= _temp4888.last_plus_one){ _throw(
Null_Exception);}* _temp4890;}).last_line))};( struct _xenum_struct*) _temp4875;});
break; case 237: _LL4874: yyval=({ struct Cyc_Stmt_tok_struct* _temp4892=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4892=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_for_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4893= yyvs; struct _xenum_struct**
_temp4895= _temp4893.curr +( yyvsp_offset - 5); if( _temp4895 < _temp4893.base?
1: _temp4895 >= _temp4893.last_plus_one){ _throw( Null_Exception);}* _temp4895;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4896= yyvs; struct _xenum_struct**
_temp4898= _temp4896.curr +( yyvsp_offset - 3); if( _temp4898 < _temp4896.base?
1: _temp4898 >= _temp4896.last_plus_one){ _throw( Null_Exception);}* _temp4898;})),
Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4899=
yyvs; struct _xenum_struct** _temp4901= _temp4899.curr + yyvsp_offset; if(
_temp4901 < _temp4899.base? 1: _temp4901 >= _temp4899.last_plus_one){ _throw(
Null_Exception);}* _temp4901;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4902= yyls; struct Cyc_Yyltype* _temp4904= _temp4902.curr +(
yylsp_offset - 7); if( _temp4904 < _temp4902.base? 1: _temp4904 >= _temp4902.last_plus_one){
_throw( Null_Exception);}* _temp4904;}).first_line,({ struct _tagged_ptr3
_temp4905= yyls; struct Cyc_Yyltype* _temp4907= _temp4905.curr + yylsp_offset;
if( _temp4907 < _temp4905.base? 1: _temp4907 >= _temp4905.last_plus_one){ _throw(
Null_Exception);}* _temp4907;}).last_line))};( struct _xenum_struct*) _temp4892;});
break; case 238: _LL4891: yyval=({ struct Cyc_Stmt_tok_struct* _temp4909=(
struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4909=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_for_stmt(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4910= yyvs; struct _xenum_struct**
_temp4912= _temp4910.curr +( yyvsp_offset - 6); if( _temp4912 < _temp4910.base?
1: _temp4912 >= _temp4910.last_plus_one){ _throw( Null_Exception);}* _temp4912;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4913= yyvs; struct _xenum_struct**
_temp4915= _temp4913.curr +( yyvsp_offset - 4); if( _temp4915 < _temp4913.base?
1: _temp4915 >= _temp4913.last_plus_one){ _throw( Null_Exception);}* _temp4915;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4916= yyvs; struct _xenum_struct**
_temp4918= _temp4916.curr +( yyvsp_offset - 2); if( _temp4918 < _temp4916.base?
1: _temp4918 >= _temp4916.last_plus_one){ _throw( Null_Exception);}* _temp4918;})),
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp4919= yyvs; struct _xenum_struct**
_temp4921= _temp4919.curr + yyvsp_offset; if( _temp4921 < _temp4919.base? 1:
_temp4921 >= _temp4919.last_plus_one){ _throw( Null_Exception);}* _temp4921;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp4922= yyls; struct Cyc_Yyltype*
_temp4924= _temp4922.curr +( yylsp_offset - 8); if( _temp4924 < _temp4922.base?
1: _temp4924 >= _temp4922.last_plus_one){ _throw( Null_Exception);}* _temp4924;}).first_line,({
struct _tagged_ptr3 _temp4925= yyls; struct Cyc_Yyltype* _temp4927= _temp4925.curr
+ yylsp_offset; if( _temp4927 < _temp4925.base? 1: _temp4927 >= _temp4925.last_plus_one){
_throw( Null_Exception);}* _temp4927;}).last_line))};( struct _xenum_struct*)
_temp4909;}); break; case 239: _LL4908: { struct Cyc_List_List* decls= Cyc_yyget_DeclList_tok(({
struct _tagged_ptr2 _temp4929= yyvs; struct _xenum_struct** _temp4931= _temp4929.curr
+( yyvsp_offset - 3); if( _temp4931 < _temp4929.base? 1: _temp4931 >= _temp4929.last_plus_one){
_throw( Null_Exception);}* _temp4931;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp4932= yyvs; struct _xenum_struct** _temp4934= _temp4932.curr
+ yyvsp_offset; if( _temp4934 < _temp4932.base? 1: _temp4934 >= _temp4932.last_plus_one){
_throw( Null_Exception);}* _temp4934;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4935= yyls; struct Cyc_Yyltype* _temp4937= _temp4935.curr +(
yylsp_offset - 5); if( _temp4937 < _temp4935.base? 1: _temp4937 >= _temp4935.last_plus_one){
_throw( Null_Exception);}* _temp4937;}).first_line,({ struct _tagged_ptr3
_temp4938= yyls; struct Cyc_Yyltype* _temp4940= _temp4938.curr + yylsp_offset;
if( _temp4940 < _temp4938.base? 1: _temp4940 >= _temp4938.last_plus_one){ _throw(
Null_Exception);}* _temp4940;}).last_line)); yyval=({ struct Cyc_Stmt_tok_struct*
_temp4941=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4941=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Parse_flatten_declarations(
decls, s)};( struct _xenum_struct*) _temp4941;}); break;} case 240: _LL4928: {
struct Cyc_List_List* decls= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2
_temp4943= yyvs; struct _xenum_struct** _temp4945= _temp4943.curr +(
yyvsp_offset - 4); if( _temp4945 < _temp4943.base? 1: _temp4945 >= _temp4943.last_plus_one){
_throw( Null_Exception);}* _temp4945;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4946=
yyvs; struct _xenum_struct** _temp4948= _temp4946.curr +( yyvsp_offset - 3); if(
_temp4948 < _temp4946.base? 1: _temp4948 >= _temp4946.last_plus_one){ _throw(
Null_Exception);}* _temp4948;})), Cyc_Absyn_false_exp( 0), Cyc_yyget_Stmt_tok(({
struct _tagged_ptr2 _temp4949= yyvs; struct _xenum_struct** _temp4951= _temp4949.curr
+ yyvsp_offset; if( _temp4951 < _temp4949.base? 1: _temp4951 >= _temp4949.last_plus_one){
_throw( Null_Exception);}* _temp4951;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4952= yyls; struct Cyc_Yyltype* _temp4954= _temp4952.curr +(
yylsp_offset - 6); if( _temp4954 < _temp4952.base? 1: _temp4954 >= _temp4952.last_plus_one){
_throw( Null_Exception);}* _temp4954;}).first_line,({ struct _tagged_ptr3
_temp4955= yyls; struct Cyc_Yyltype* _temp4957= _temp4955.curr + yylsp_offset;
if( _temp4957 < _temp4955.base? 1: _temp4957 >= _temp4955.last_plus_one){ _throw(
Null_Exception);}* _temp4957;}).last_line)); yyval=({ struct Cyc_Stmt_tok_struct*
_temp4958=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4958=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Parse_flatten_declarations(
decls, s)};( struct _xenum_struct*) _temp4958;}); break;} case 241: _LL4942: {
struct Cyc_List_List* decls= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2
_temp4960= yyvs; struct _xenum_struct** _temp4962= _temp4960.curr +(
yyvsp_offset - 4); if( _temp4962 < _temp4960.base? 1: _temp4962 >= _temp4960.last_plus_one){
_throw( Null_Exception);}* _temp4962;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_Absyn_true_exp( 0), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp4963= yyvs; struct _xenum_struct** _temp4965= _temp4963.curr +(
yyvsp_offset - 2); if( _temp4965 < _temp4963.base? 1: _temp4965 >= _temp4963.last_plus_one){
_throw( Null_Exception);}* _temp4965;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp4966= yyvs; struct _xenum_struct** _temp4968= _temp4966.curr +
yyvsp_offset; if( _temp4968 < _temp4966.base? 1: _temp4968 >= _temp4966.last_plus_one){
_throw( Null_Exception);}* _temp4968;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4969= yyls; struct Cyc_Yyltype* _temp4971= _temp4969.curr +(
yylsp_offset - 6); if( _temp4971 < _temp4969.base? 1: _temp4971 >= _temp4969.last_plus_one){
_throw( Null_Exception);}* _temp4971;}).first_line,({ struct _tagged_ptr3
_temp4972= yyls; struct Cyc_Yyltype* _temp4974= _temp4972.curr + yylsp_offset;
if( _temp4974 < _temp4972.base? 1: _temp4974 >= _temp4972.last_plus_one){ _throw(
Null_Exception);}* _temp4974;}).last_line)); yyval=({ struct Cyc_Stmt_tok_struct*
_temp4975=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4975=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Parse_flatten_declarations(
decls, s)};( struct _xenum_struct*) _temp4975;}); break;} case 242: _LL4959: {
struct Cyc_List_List* decls= Cyc_yyget_DeclList_tok(({ struct _tagged_ptr2
_temp4977= yyvs; struct _xenum_struct** _temp4979= _temp4977.curr +(
yyvsp_offset - 5); if( _temp4979 < _temp4977.base? 1: _temp4979 >= _temp4977.last_plus_one){
_throw( Null_Exception);}* _temp4979;})); struct Cyc_Absyn_Stmt* s= Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp( 0), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp4980=
yyvs; struct _xenum_struct** _temp4982= _temp4980.curr +( yyvsp_offset - 4); if(
_temp4982 < _temp4980.base? 1: _temp4982 >= _temp4980.last_plus_one){ _throw(
Null_Exception);}* _temp4982;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp4983= yyvs; struct _xenum_struct** _temp4985= _temp4983.curr +(
yyvsp_offset - 2); if( _temp4985 < _temp4983.base? 1: _temp4985 >= _temp4983.last_plus_one){
_throw( Null_Exception);}* _temp4985;})), Cyc_yyget_Stmt_tok(({ struct
_tagged_ptr2 _temp4986= yyvs; struct _xenum_struct** _temp4988= _temp4986.curr +
yyvsp_offset; if( _temp4988 < _temp4986.base? 1: _temp4988 >= _temp4986.last_plus_one){
_throw( Null_Exception);}* _temp4988;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp4989= yyls; struct Cyc_Yyltype* _temp4991= _temp4989.curr +(
yylsp_offset - 7); if( _temp4991 < _temp4989.base? 1: _temp4991 >= _temp4989.last_plus_one){
_throw( Null_Exception);}* _temp4991;}).first_line,({ struct _tagged_ptr3
_temp4992= yyls; struct Cyc_Yyltype* _temp4994= _temp4992.curr + yylsp_offset;
if( _temp4994 < _temp4992.base? 1: _temp4994 >= _temp4992.last_plus_one){ _throw(
Null_Exception);}* _temp4994;}).last_line)); yyval=({ struct Cyc_Stmt_tok_struct*
_temp4995=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp4995=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Parse_flatten_declarations(
decls, s)};( struct _xenum_struct*) _temp4995;}); break;} case 243: _LL4976:
yyval=({ struct Cyc_Stmt_tok_struct* _temp4997=( struct Cyc_Stmt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));* _temp4997=( struct Cyc_Stmt_tok_struct){.tag=
Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_goto_stmt(({ struct _tagged_string* _temp4998=(
struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) * 1);
_temp4998[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp4999= yyvs;
struct _xenum_struct** _temp5001= _temp4999.curr +( yyvsp_offset - 1); if(
_temp5001 < _temp4999.base? 1: _temp5001 >= _temp4999.last_plus_one){ _throw(
Null_Exception);}* _temp5001;})); _temp4998;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5002= yyls; struct Cyc_Yyltype* _temp5004= _temp5002.curr
+( yylsp_offset - 2); if( _temp5004 < _temp5002.base? 1: _temp5004 >= _temp5002.last_plus_one){
_throw( Null_Exception);}* _temp5004;}).first_line,({ struct _tagged_ptr3
_temp5005= yyls; struct Cyc_Yyltype* _temp5007= _temp5005.curr +( yylsp_offset -
1); if( _temp5007 < _temp5005.base? 1: _temp5007 >= _temp5005.last_plus_one){
_throw( Null_Exception);}* _temp5007;}).last_line))};( struct _xenum_struct*)
_temp4997;}); break; case 244: _LL4996: yyval=({ struct Cyc_Stmt_tok_struct*
_temp5009=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp5009=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_continue_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5010= yyls; struct Cyc_Yyltype*
_temp5012= _temp5010.curr +( yylsp_offset - 1); if( _temp5012 < _temp5010.base?
1: _temp5012 >= _temp5010.last_plus_one){ _throw( Null_Exception);}* _temp5012;}).first_line,({
struct _tagged_ptr3 _temp5013= yyls; struct Cyc_Yyltype* _temp5015= _temp5013.curr
+( yylsp_offset - 1); if( _temp5015 < _temp5013.base? 1: _temp5015 >= _temp5013.last_plus_one){
_throw( Null_Exception);}* _temp5015;}).last_line))};( struct _xenum_struct*)
_temp5009;}); break; case 245: _LL5008: yyval=({ struct Cyc_Stmt_tok_struct*
_temp5017=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp5017=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_break_stmt(
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5018= yyls; struct Cyc_Yyltype*
_temp5020= _temp5018.curr +( yylsp_offset - 1); if( _temp5020 < _temp5018.base?
1: _temp5020 >= _temp5018.last_plus_one){ _throw( Null_Exception);}* _temp5020;}).first_line,({
struct _tagged_ptr3 _temp5021= yyls; struct Cyc_Yyltype* _temp5023= _temp5021.curr
+( yylsp_offset - 1); if( _temp5023 < _temp5021.base? 1: _temp5023 >= _temp5021.last_plus_one){
_throw( Null_Exception);}* _temp5023;}).last_line))};( struct _xenum_struct*)
_temp5017;}); break; case 246: _LL5016: yyval=({ struct Cyc_Stmt_tok_struct*
_temp5025=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp5025=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_return_stmt(
0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5026= yyls; struct
Cyc_Yyltype* _temp5028= _temp5026.curr +( yylsp_offset - 1); if( _temp5028 <
_temp5026.base? 1: _temp5028 >= _temp5026.last_plus_one){ _throw( Null_Exception);}*
_temp5028;}).first_line,({ struct _tagged_ptr3 _temp5029= yyls; struct Cyc_Yyltype*
_temp5031= _temp5029.curr +( yylsp_offset - 1); if( _temp5031 < _temp5029.base?
1: _temp5031 >= _temp5029.last_plus_one){ _throw( Null_Exception);}* _temp5031;}).last_line))};(
struct _xenum_struct*) _temp5025;}); break; case 247: _LL5024: yyval=({ struct
Cyc_Stmt_tok_struct* _temp5033=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct));* _temp5033=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1=
Cyc_Absyn_return_stmt(( struct Cyc_Absyn_Exp*) Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5034= yyvs; struct _xenum_struct** _temp5036= _temp5034.curr +(
yyvsp_offset - 1); if( _temp5036 < _temp5034.base? 1: _temp5036 >= _temp5034.last_plus_one){
_throw( Null_Exception);}* _temp5036;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5037= yyls; struct Cyc_Yyltype* _temp5039= _temp5037.curr +(
yylsp_offset - 2); if( _temp5039 < _temp5037.base? 1: _temp5039 >= _temp5037.last_plus_one){
_throw( Null_Exception);}* _temp5039;}).first_line,({ struct _tagged_ptr3
_temp5040= yyls; struct Cyc_Yyltype* _temp5042= _temp5040.curr +( yylsp_offset -
1); if( _temp5042 < _temp5040.base? 1: _temp5042 >= _temp5040.last_plus_one){
_throw( Null_Exception);}* _temp5042;}).last_line))};( struct _xenum_struct*)
_temp5033;}); break; case 248: _LL5032: yyval=({ struct Cyc_Stmt_tok_struct*
_temp5044=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp5044=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_fallthru_stmt(
0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5045= yyls; struct
Cyc_Yyltype* _temp5047= _temp5045.curr +( yylsp_offset - 1); if( _temp5047 <
_temp5045.base? 1: _temp5047 >= _temp5045.last_plus_one){ _throw( Null_Exception);}*
_temp5047;}).first_line,({ struct _tagged_ptr3 _temp5048= yyls; struct Cyc_Yyltype*
_temp5050= _temp5048.curr +( yylsp_offset - 1); if( _temp5050 < _temp5048.base?
1: _temp5050 >= _temp5048.last_plus_one){ _throw( Null_Exception);}* _temp5050;}).last_line))};(
struct _xenum_struct*) _temp5044;}); break; case 249: _LL5043: yyval=({ struct
Cyc_Stmt_tok_struct* _temp5052=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof(
struct Cyc_Stmt_tok_struct));* _temp5052=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1=
Cyc_Absyn_fallthru_stmt( 0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5053= yyls; struct Cyc_Yyltype* _temp5055= _temp5053.curr +( yylsp_offset -
3); if( _temp5055 < _temp5053.base? 1: _temp5055 >= _temp5053.last_plus_one){
_throw( Null_Exception);}* _temp5055;}).first_line,({ struct _tagged_ptr3
_temp5056= yyls; struct Cyc_Yyltype* _temp5058= _temp5056.curr +( yylsp_offset -
3); if( _temp5058 < _temp5056.base? 1: _temp5058 >= _temp5056.last_plus_one){
_throw( Null_Exception);}* _temp5058;}).last_line))};( struct _xenum_struct*)
_temp5052;}); break; case 250: _LL5051: yyval=({ struct Cyc_Stmt_tok_struct*
_temp5060=( struct Cyc_Stmt_tok_struct*) GC_malloc( sizeof( struct Cyc_Stmt_tok_struct));*
_temp5060=( struct Cyc_Stmt_tok_struct){.tag= Cyc_Stmt_tok_tag,.f1= Cyc_Absyn_fallthru_stmt(
Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2 _temp5061= yyvs; struct
_xenum_struct** _temp5063= _temp5061.curr +( yyvsp_offset - 2); if( _temp5063 <
_temp5061.base? 1: _temp5063 >= _temp5061.last_plus_one){ _throw( Null_Exception);}*
_temp5063;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5064=
yyls; struct Cyc_Yyltype* _temp5066= _temp5064.curr +( yylsp_offset - 4); if(
_temp5066 < _temp5064.base? 1: _temp5066 >= _temp5064.last_plus_one){ _throw(
Null_Exception);}* _temp5066;}).first_line,({ struct _tagged_ptr3 _temp5067=
yyls; struct Cyc_Yyltype* _temp5069= _temp5067.curr +( yylsp_offset - 4); if(
_temp5069 < _temp5067.base? 1: _temp5069 >= _temp5067.last_plus_one){ _throw(
Null_Exception);}* _temp5069;}).last_line))};( struct _xenum_struct*) _temp5060;});
break; case 251: _LL5059: yyval=({ struct Cyc_Pattern_tok_struct* _temp5071=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp5071=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(
Cyc_Absyn_Wild_p, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5072=
yyls; struct Cyc_Yyltype* _temp5074= _temp5072.curr + yylsp_offset; if(
_temp5074 < _temp5072.base? 1: _temp5074 >= _temp5072.last_plus_one){ _throw(
Null_Exception);}* _temp5074;}).first_line,({ struct _tagged_ptr3 _temp5075=
yyls; struct Cyc_Yyltype* _temp5077= _temp5075.curr + yylsp_offset; if(
_temp5077 < _temp5075.base? 1: _temp5077 >= _temp5075.last_plus_one){ _throw(
Null_Exception);}* _temp5077;}).last_line))};( struct _xenum_struct*) _temp5071;});
break; case 252: _LL5070: yyval=({ struct _tagged_ptr2 _temp5079= yyvs; struct
_xenum_struct** _temp5081= _temp5079.curr +( yyvsp_offset - 1); if( _temp5081 <
_temp5079.base? 1: _temp5081 >= _temp5079.last_plus_one){ _throw( Null_Exception);}*
_temp5081;}); break; case 253: _LL5078: yyval=({ struct Cyc_Pattern_tok_struct*
_temp5083=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp5083=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(({
struct Cyc_Absyn_Int_p_struct* _temp5084=( struct Cyc_Absyn_Int_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct)); _temp5084->tag= Cyc_Absyn_Int_p_tag;
_temp5084->f1=( void*)(* Cyc_yyget_Int_tok(({ struct _tagged_ptr2 _temp5085=
yyvs; struct _xenum_struct** _temp5087= _temp5085.curr + yyvsp_offset; if(
_temp5087 < _temp5085.base? 1: _temp5087 >= _temp5085.last_plus_one){ _throw(
Null_Exception);}* _temp5087;}))).f1; _temp5084->f2=(* Cyc_yyget_Int_tok(({
struct _tagged_ptr2 _temp5088= yyvs; struct _xenum_struct** _temp5090= _temp5088.curr
+ yyvsp_offset; if( _temp5090 < _temp5088.base? 1: _temp5090 >= _temp5088.last_plus_one){
_throw( Null_Exception);}* _temp5090;}))).f2;( void*) _temp5084;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5091= yyls; struct Cyc_Yyltype* _temp5093= _temp5091.curr
+ yylsp_offset; if( _temp5093 < _temp5091.base? 1: _temp5093 >= _temp5091.last_plus_one){
_throw( Null_Exception);}* _temp5093;}).first_line,({ struct _tagged_ptr3
_temp5094= yyls; struct Cyc_Yyltype* _temp5096= _temp5094.curr + yylsp_offset;
if( _temp5096 < _temp5094.base? 1: _temp5096 >= _temp5094.last_plus_one){ _throw(
Null_Exception);}* _temp5096;}).last_line))};( struct _xenum_struct*) _temp5083;});
break; case 254: _LL5082: yyval=({ struct Cyc_Pattern_tok_struct* _temp5098=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp5098=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(({
struct Cyc_Absyn_Int_p_struct* _temp5099=( struct Cyc_Absyn_Int_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_p_struct)); _temp5099->tag= Cyc_Absyn_Int_p_tag;
_temp5099->f1=( void*) Cyc_Absyn_Signed; _temp5099->f2= -(* Cyc_yyget_Int_tok(({
struct _tagged_ptr2 _temp5100= yyvs; struct _xenum_struct** _temp5102= _temp5100.curr
+ yyvsp_offset; if( _temp5102 < _temp5100.base? 1: _temp5102 >= _temp5100.last_plus_one){
_throw( Null_Exception);}* _temp5102;}))).f2;( void*) _temp5099;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5103= yyls; struct Cyc_Yyltype* _temp5105= _temp5103.curr
+( yylsp_offset - 1); if( _temp5105 < _temp5103.base? 1: _temp5105 >= _temp5103.last_plus_one){
_throw( Null_Exception);}* _temp5105;}).first_line,({ struct _tagged_ptr3
_temp5106= yyls; struct Cyc_Yyltype* _temp5108= _temp5106.curr + yylsp_offset;
if( _temp5108 < _temp5106.base? 1: _temp5108 >= _temp5106.last_plus_one){ _throw(
Null_Exception);}* _temp5108;}).last_line))};( struct _xenum_struct*) _temp5098;});
break; case 255: _LL5097: yyval=({ struct Cyc_Pattern_tok_struct* _temp5110=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp5110=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(({
struct Cyc_Absyn_Float_p_struct* _temp5111=( struct Cyc_Absyn_Float_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Float_p_struct)); _temp5111->tag= Cyc_Absyn_Float_p_tag;
_temp5111->f1= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp5112= yyvs;
struct _xenum_struct** _temp5114= _temp5112.curr + yyvsp_offset; if( _temp5114 <
_temp5112.base? 1: _temp5114 >= _temp5112.last_plus_one){ _throw( Null_Exception);}*
_temp5114;}));( void*) _temp5111;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5115= yyls; struct Cyc_Yyltype* _temp5117= _temp5115.curr +
yylsp_offset; if( _temp5117 < _temp5115.base? 1: _temp5117 >= _temp5115.last_plus_one){
_throw( Null_Exception);}* _temp5117;}).first_line,({ struct _tagged_ptr3
_temp5118= yyls; struct Cyc_Yyltype* _temp5120= _temp5118.curr + yylsp_offset;
if( _temp5120 < _temp5118.base? 1: _temp5120 >= _temp5118.last_plus_one){ _throw(
Null_Exception);}* _temp5120;}).last_line))};( struct _xenum_struct*) _temp5110;});
break; case 256: _LL5109: yyval=({ struct Cyc_Pattern_tok_struct* _temp5122=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp5122=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(({
struct Cyc_Absyn_Char_p_struct* _temp5123=( struct Cyc_Absyn_Char_p_struct*)
GC_malloc_atomic( sizeof( struct Cyc_Absyn_Char_p_struct)); _temp5123->tag= Cyc_Absyn_Char_p_tag;
_temp5123->f1= Cyc_yyget_Char_tok(({ struct _tagged_ptr2 _temp5124= yyvs; struct
_xenum_struct** _temp5126= _temp5124.curr + yyvsp_offset; if( _temp5126 <
_temp5124.base? 1: _temp5126 >= _temp5124.last_plus_one){ _throw( Null_Exception);}*
_temp5126;}));( void*) _temp5123;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5127= yyls; struct Cyc_Yyltype* _temp5129= _temp5127.curr +
yylsp_offset; if( _temp5129 < _temp5127.base? 1: _temp5129 >= _temp5127.last_plus_one){
_throw( Null_Exception);}* _temp5129;}).first_line,({ struct _tagged_ptr3
_temp5130= yyls; struct Cyc_Yyltype* _temp5132= _temp5130.curr + yylsp_offset;
if( _temp5132 < _temp5130.base? 1: _temp5132 >= _temp5130.last_plus_one){ _throw(
Null_Exception);}* _temp5132;}).last_line))};( struct _xenum_struct*) _temp5122;});
break; case 257: _LL5121: yyval=({ struct Cyc_Pattern_tok_struct* _temp5134=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp5134=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(
Cyc_Absyn_Null_p, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5135=
yyls; struct Cyc_Yyltype* _temp5137= _temp5135.curr + yylsp_offset; if(
_temp5137 < _temp5135.base? 1: _temp5137 >= _temp5135.last_plus_one){ _throw(
Null_Exception);}* _temp5137;}).first_line,({ struct _tagged_ptr3 _temp5138=
yyls; struct Cyc_Yyltype* _temp5140= _temp5138.curr + yylsp_offset; if(
_temp5140 < _temp5138.base? 1: _temp5140 >= _temp5138.last_plus_one){ _throw(
Null_Exception);}* _temp5140;}).last_line))};( struct _xenum_struct*) _temp5134;});
break; case 258: _LL5133: yyval=({ struct Cyc_Pattern_tok_struct* _temp5142=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp5142=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(({
struct Cyc_Absyn_UnknownId_p_struct* _temp5143=( struct Cyc_Absyn_UnknownId_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_p_struct)); _temp5143->tag= Cyc_Absyn_UnknownId_p_tag;
_temp5143->f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp5144= yyvs;
struct _xenum_struct** _temp5146= _temp5144.curr + yyvsp_offset; if( _temp5146 <
_temp5144.base? 1: _temp5146 >= _temp5144.last_plus_one){ _throw( Null_Exception);}*
_temp5146;}));( void*) _temp5143;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5147= yyls; struct Cyc_Yyltype* _temp5149= _temp5147.curr +
yylsp_offset; if( _temp5149 < _temp5147.base? 1: _temp5149 >= _temp5147.last_plus_one){
_throw( Null_Exception);}* _temp5149;}).first_line,({ struct _tagged_ptr3
_temp5150= yyls; struct Cyc_Yyltype* _temp5152= _temp5150.curr + yylsp_offset;
if( _temp5152 < _temp5150.base? 1: _temp5152 >= _temp5150.last_plus_one){ _throw(
Null_Exception);}* _temp5152;}).last_line))};( struct _xenum_struct*) _temp5142;});
break; case 259: _LL5141: { struct Cyc_List_List* tvs=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5154= yyls; struct Cyc_Yyltype* _temp5156= _temp5154.curr
+( yylsp_offset - 3); if( _temp5156 < _temp5154.base? 1: _temp5156 >= _temp5154.last_plus_one){
_throw( Null_Exception);}* _temp5156;}).first_line,({ struct _tagged_ptr3
_temp5157= yyls; struct Cyc_Yyltype* _temp5159= _temp5157.curr +( yylsp_offset -
3); if( _temp5159 < _temp5157.base? 1: _temp5159 >= _temp5157.last_plus_one){
_throw( Null_Exception);}* _temp5159;}).last_line), Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp5160= yyvs; struct _xenum_struct** _temp5162= _temp5160.curr
+( yyvsp_offset - 3); if( _temp5162 < _temp5160.base? 1: _temp5162 >= _temp5160.last_plus_one){
_throw( Null_Exception);}* _temp5162;}))); yyval=({ struct Cyc_Pattern_tok_struct*
_temp5163=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp5163=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(({
struct Cyc_Absyn_UnknownCall_p_struct* _temp5164=( struct Cyc_Absyn_UnknownCall_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownCall_p_struct)); _temp5164->tag= Cyc_Absyn_UnknownCall_p_tag;
_temp5164->f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp5165= yyvs;
struct _xenum_struct** _temp5167= _temp5165.curr +( yyvsp_offset - 4); if(
_temp5167 < _temp5165.base? 1: _temp5167 >= _temp5165.last_plus_one){ _throw(
Null_Exception);}* _temp5167;})); _temp5164->f2= tvs; _temp5164->f3= Cyc_yyget_PatternList_tok(({
struct _tagged_ptr2 _temp5168= yyvs; struct _xenum_struct** _temp5170= _temp5168.curr
+( yyvsp_offset - 1); if( _temp5170 < _temp5168.base? 1: _temp5170 >= _temp5168.last_plus_one){
_throw( Null_Exception);}* _temp5170;}));( void*) _temp5164;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5171= yyls; struct Cyc_Yyltype* _temp5173= _temp5171.curr
+( yylsp_offset - 4); if( _temp5173 < _temp5171.base? 1: _temp5173 >= _temp5171.last_plus_one){
_throw( Null_Exception);}* _temp5173;}).first_line,({ struct _tagged_ptr3
_temp5174= yyls; struct Cyc_Yyltype* _temp5176= _temp5174.curr + yylsp_offset;
if( _temp5176 < _temp5174.base? 1: _temp5176 >= _temp5174.last_plus_one){ _throw(
Null_Exception);}* _temp5176;}).last_line))};( struct _xenum_struct*) _temp5163;});
break;} case 260: _LL5153: yyval=({ struct Cyc_Pattern_tok_struct* _temp5178=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp5178=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(({
struct Cyc_Absyn_Tuple_p_struct* _temp5179=( struct Cyc_Absyn_Tuple_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuple_p_struct)); _temp5179->tag= Cyc_Absyn_Tuple_p_tag;
_temp5179->f1= Cyc_yyget_PatternList_tok(({ struct _tagged_ptr2 _temp5180= yyvs;
struct _xenum_struct** _temp5182= _temp5180.curr +( yyvsp_offset - 1); if(
_temp5182 < _temp5180.base? 1: _temp5182 >= _temp5180.last_plus_one){ _throw(
Null_Exception);}* _temp5182;}));( void*) _temp5179;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5183= yyls; struct Cyc_Yyltype* _temp5185= _temp5183.curr
+( yylsp_offset - 3); if( _temp5185 < _temp5183.base? 1: _temp5185 >= _temp5183.last_plus_one){
_throw( Null_Exception);}* _temp5185;}).first_line,({ struct _tagged_ptr3
_temp5186= yyls; struct Cyc_Yyltype* _temp5188= _temp5186.curr + yylsp_offset;
if( _temp5188 < _temp5186.base? 1: _temp5188 >= _temp5186.last_plus_one){ _throw(
Null_Exception);}* _temp5188;}).last_line))};( struct _xenum_struct*) _temp5178;});
break; case 261: _LL5177: { struct Cyc_List_List* tvs=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5190= yyls; struct Cyc_Yyltype* _temp5192= _temp5190.curr
+( yylsp_offset - 2); if( _temp5192 < _temp5190.base? 1: _temp5192 >= _temp5190.last_plus_one){
_throw( Null_Exception);}* _temp5192;}).first_line,({ struct _tagged_ptr3
_temp5193= yyls; struct Cyc_Yyltype* _temp5195= _temp5193.curr +( yylsp_offset -
2); if( _temp5195 < _temp5193.base? 1: _temp5195 >= _temp5193.last_plus_one){
_throw( Null_Exception);}* _temp5195;}).last_line), Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp5196= yyvs; struct _xenum_struct** _temp5198= _temp5196.curr
+( yyvsp_offset - 2); if( _temp5198 < _temp5196.base? 1: _temp5198 >= _temp5196.last_plus_one){
_throw( Null_Exception);}* _temp5198;}))); yyval=({ struct Cyc_Pattern_tok_struct*
_temp5199=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp5199=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(({
struct Cyc_Absyn_UnknownFields_p_struct* _temp5200=( struct Cyc_Absyn_UnknownFields_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownFields_p_struct)); _temp5200->tag=
Cyc_Absyn_UnknownFields_p_tag; _temp5200->f1= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp5201= yyvs; struct _xenum_struct** _temp5203= _temp5201.curr +(
yyvsp_offset - 3); if( _temp5203 < _temp5201.base? 1: _temp5203 >= _temp5201.last_plus_one){
_throw( Null_Exception);}* _temp5203;})); _temp5200->f2= tvs; _temp5200->f3= 0;(
void*) _temp5200;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5204= yyls; struct Cyc_Yyltype* _temp5206= _temp5204.curr +( yylsp_offset -
3); if( _temp5206 < _temp5204.base? 1: _temp5206 >= _temp5204.last_plus_one){
_throw( Null_Exception);}* _temp5206;}).first_line,({ struct _tagged_ptr3
_temp5207= yyls; struct Cyc_Yyltype* _temp5209= _temp5207.curr + yylsp_offset;
if( _temp5209 < _temp5207.base? 1: _temp5209 >= _temp5207.last_plus_one){ _throw(
Null_Exception);}* _temp5209;}).last_line))};( struct _xenum_struct*) _temp5199;});
break;} case 262: _LL5189: { struct Cyc_List_List* tvs=(( struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(* f)( struct Cyc_Position_Segment*, void*), struct Cyc_Position_Segment*
env, struct Cyc_List_List* x)) Cyc_List_map_c)( Cyc_Parse_typ2tvar, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5211= yyls; struct Cyc_Yyltype* _temp5213= _temp5211.curr
+( yylsp_offset - 3); if( _temp5213 < _temp5211.base? 1: _temp5213 >= _temp5211.last_plus_one){
_throw( Null_Exception);}* _temp5213;}).first_line,({ struct _tagged_ptr3
_temp5214= yyls; struct Cyc_Yyltype* _temp5216= _temp5214.curr +( yylsp_offset -
3); if( _temp5216 < _temp5214.base? 1: _temp5216 >= _temp5214.last_plus_one){
_throw( Null_Exception);}* _temp5216;}).last_line), Cyc_yyget_TypeList_tok(({
struct _tagged_ptr2 _temp5217= yyvs; struct _xenum_struct** _temp5219= _temp5217.curr
+( yyvsp_offset - 3); if( _temp5219 < _temp5217.base? 1: _temp5219 >= _temp5217.last_plus_one){
_throw( Null_Exception);}* _temp5219;}))); yyval=({ struct Cyc_Pattern_tok_struct*
_temp5220=( struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp5220=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(({
struct Cyc_Absyn_UnknownFields_p_struct* _temp5221=( struct Cyc_Absyn_UnknownFields_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownFields_p_struct)); _temp5221->tag=
Cyc_Absyn_UnknownFields_p_tag; _temp5221->f1= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp5222= yyvs; struct _xenum_struct** _temp5224= _temp5222.curr +(
yyvsp_offset - 4); if( _temp5224 < _temp5222.base? 1: _temp5224 >= _temp5222.last_plus_one){
_throw( Null_Exception);}* _temp5224;})); _temp5221->f2= tvs; _temp5221->f3= Cyc_yyget_FieldPatternList_tok(({
struct _tagged_ptr2 _temp5225= yyvs; struct _xenum_struct** _temp5227= _temp5225.curr
+( yyvsp_offset - 1); if( _temp5227 < _temp5225.base? 1: _temp5227 >= _temp5225.last_plus_one){
_throw( Null_Exception);}* _temp5227;}));( void*) _temp5221;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5228= yyls; struct Cyc_Yyltype* _temp5230= _temp5228.curr
+( yylsp_offset - 4); if( _temp5230 < _temp5228.base? 1: _temp5230 >= _temp5228.last_plus_one){
_throw( Null_Exception);}* _temp5230;}).first_line,({ struct _tagged_ptr3
_temp5231= yyls; struct Cyc_Yyltype* _temp5233= _temp5231.curr + yylsp_offset;
if( _temp5233 < _temp5231.base? 1: _temp5233 >= _temp5231.last_plus_one){ _throw(
Null_Exception);}* _temp5233;}).last_line))};( struct _xenum_struct*) _temp5220;});
break;} case 263: _LL5210: yyval=({ struct Cyc_Pattern_tok_struct* _temp5235=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp5235=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(({
struct Cyc_Absyn_Pointer_p_struct* _temp5236=( struct Cyc_Absyn_Pointer_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Pointer_p_struct)); _temp5236->tag= Cyc_Absyn_Pointer_p_tag;
_temp5236->f1= Cyc_yyget_Pattern_tok(({ struct _tagged_ptr2 _temp5237= yyvs;
struct _xenum_struct** _temp5239= _temp5237.curr + yyvsp_offset; if( _temp5239 <
_temp5237.base? 1: _temp5239 >= _temp5237.last_plus_one){ _throw( Null_Exception);}*
_temp5239;}));( void*) _temp5236;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5240= yyls; struct Cyc_Yyltype* _temp5242= _temp5240.curr +(
yylsp_offset - 1); if( _temp5242 < _temp5240.base? 1: _temp5242 >= _temp5240.last_plus_one){
_throw( Null_Exception);}* _temp5242;}).first_line,({ struct _tagged_ptr3
_temp5243= yyls; struct Cyc_Yyltype* _temp5245= _temp5243.curr + yylsp_offset;
if( _temp5245 < _temp5243.base? 1: _temp5245 >= _temp5243.last_plus_one){ _throw(
Null_Exception);}* _temp5245;}).last_line))};( struct _xenum_struct*) _temp5235;});
break; case 264: _LL5234: yyval=({ struct Cyc_Pattern_tok_struct* _temp5247=(
struct Cyc_Pattern_tok_struct*) GC_malloc( sizeof( struct Cyc_Pattern_tok_struct));*
_temp5247=( struct Cyc_Pattern_tok_struct){.tag= Cyc_Pattern_tok_tag,.f1= Cyc_Absyn_new_pat(({
struct Cyc_Absyn_Reference_p_struct* _temp5248=( struct Cyc_Absyn_Reference_p_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Reference_p_struct)); _temp5248->tag= Cyc_Absyn_Reference_p_tag;
_temp5248->f1= Cyc_Absyn_new_vardecl(({ struct _tuple1* _temp5249=( struct
_tuple1*) GC_malloc( sizeof( struct _tuple1)); _temp5249->f1= Cyc_Absyn_Loc_n;
_temp5249->f2=({ struct _tagged_string* _temp5250=( struct _tagged_string*)
GC_malloc( sizeof( struct _tagged_string) * 1); _temp5250[ 0]= Cyc_yyget_String_tok(({
struct _tagged_ptr2 _temp5251= yyvs; struct _xenum_struct** _temp5253= _temp5251.curr
+ yyvsp_offset; if( _temp5253 < _temp5251.base? 1: _temp5253 >= _temp5251.last_plus_one){
_throw( Null_Exception);}* _temp5253;})); _temp5250;}); _temp5249;}), Cyc_Absyn_VoidType,
0);( void*) _temp5248;}), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3
_temp5254= yyls; struct Cyc_Yyltype* _temp5256= _temp5254.curr +( yylsp_offset -
1); if( _temp5256 < _temp5254.base? 1: _temp5256 >= _temp5254.last_plus_one){
_throw( Null_Exception);}* _temp5256;}).first_line,({ struct _tagged_ptr3
_temp5257= yyls; struct Cyc_Yyltype* _temp5259= _temp5257.curr + yylsp_offset;
if( _temp5259 < _temp5257.base? 1: _temp5259 >= _temp5257.last_plus_one){ _throw(
Null_Exception);}* _temp5259;}).last_line))};( struct _xenum_struct*) _temp5247;});
break; case 265: _LL5246: yyval=({ struct Cyc_PatternList_tok_struct* _temp5261=(
struct Cyc_PatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));*
_temp5261=( struct Cyc_PatternList_tok_struct){.tag= Cyc_PatternList_tok_tag,.f1=
0};( struct _xenum_struct*) _temp5261;}); break; case 266: _LL5260: yyval=({
struct Cyc_PatternList_tok_struct* _temp5263=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));* _temp5263=( struct Cyc_PatternList_tok_struct){.tag=
Cyc_PatternList_tok_tag,.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x))
Cyc_List_imp_rev)( Cyc_yyget_PatternList_tok(({ struct _tagged_ptr2 _temp5264=
yyvs; struct _xenum_struct** _temp5266= _temp5264.curr + yyvsp_offset; if(
_temp5266 < _temp5264.base? 1: _temp5266 >= _temp5264.last_plus_one){ _throw(
Null_Exception);}* _temp5266;})))};( struct _xenum_struct*) _temp5263;}); break;
case 267: _LL5262: yyval=({ struct Cyc_PatternList_tok_struct* _temp5268=(
struct Cyc_PatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));*
_temp5268=( struct Cyc_PatternList_tok_struct){.tag= Cyc_PatternList_tok_tag,.f1=({
struct Cyc_List_List* _temp5269=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp5269->hd=( void*) Cyc_yyget_Pattern_tok(({ struct
_tagged_ptr2 _temp5270= yyvs; struct _xenum_struct** _temp5272= _temp5270.curr +
yyvsp_offset; if( _temp5272 < _temp5270.base? 1: _temp5272 >= _temp5270.last_plus_one){
_throw( Null_Exception);}* _temp5272;})); _temp5269->tl= 0; _temp5269;})};(
struct _xenum_struct*) _temp5268;}); break; case 268: _LL5267: yyval=({ struct
Cyc_PatternList_tok_struct* _temp5274=( struct Cyc_PatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_PatternList_tok_struct));* _temp5274=( struct Cyc_PatternList_tok_struct){.tag=
Cyc_PatternList_tok_tag,.f1=({ struct Cyc_List_List* _temp5275=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp5275->hd=( void*) Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp5279= yyvs; struct _xenum_struct** _temp5281= _temp5279.curr
+ yyvsp_offset; if( _temp5281 < _temp5279.base? 1: _temp5281 >= _temp5279.last_plus_one){
_throw( Null_Exception);}* _temp5281;})); _temp5275->tl= Cyc_yyget_PatternList_tok(({
struct _tagged_ptr2 _temp5276= yyvs; struct _xenum_struct** _temp5278= _temp5276.curr
+( yyvsp_offset - 2); if( _temp5278 < _temp5276.base? 1: _temp5278 >= _temp5276.last_plus_one){
_throw( Null_Exception);}* _temp5278;})); _temp5275;})};( struct _xenum_struct*)
_temp5274;}); break; case 269: _LL5273: yyval=({ struct Cyc_FieldPattern_tok_struct*
_temp5283=( struct Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));*
_temp5283=( struct Cyc_FieldPattern_tok_struct){.tag= Cyc_FieldPattern_tok_tag,.f1=({
struct _tuple15* _temp5284=( struct _tuple15*) GC_malloc( sizeof( struct
_tuple15)); _temp5284->f1= 0; _temp5284->f2= Cyc_yyget_Pattern_tok(({ struct
_tagged_ptr2 _temp5285= yyvs; struct _xenum_struct** _temp5287= _temp5285.curr +
yyvsp_offset; if( _temp5287 < _temp5285.base? 1: _temp5287 >= _temp5285.last_plus_one){
_throw( Null_Exception);}* _temp5287;})); _temp5284;})};( struct _xenum_struct*)
_temp5283;}); break; case 270: _LL5282: yyval=({ struct Cyc_FieldPattern_tok_struct*
_temp5289=( struct Cyc_FieldPattern_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPattern_tok_struct));*
_temp5289=( struct Cyc_FieldPattern_tok_struct){.tag= Cyc_FieldPattern_tok_tag,.f1=({
struct _tuple15* _temp5290=( struct _tuple15*) GC_malloc( sizeof( struct
_tuple15)); _temp5290->f1= Cyc_yyget_DesignatorList_tok(({ struct _tagged_ptr2
_temp5294= yyvs; struct _xenum_struct** _temp5296= _temp5294.curr +(
yyvsp_offset - 1); if( _temp5296 < _temp5294.base? 1: _temp5296 >= _temp5294.last_plus_one){
_throw( Null_Exception);}* _temp5296;})); _temp5290->f2= Cyc_yyget_Pattern_tok(({
struct _tagged_ptr2 _temp5291= yyvs; struct _xenum_struct** _temp5293= _temp5291.curr
+ yyvsp_offset; if( _temp5293 < _temp5291.base? 1: _temp5293 >= _temp5291.last_plus_one){
_throw( Null_Exception);}* _temp5293;})); _temp5290;})};( struct _xenum_struct*)
_temp5289;}); break; case 271: _LL5288: yyval=({ struct Cyc_FieldPatternList_tok_struct*
_temp5298=( struct Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct
Cyc_FieldPatternList_tok_struct));* _temp5298=( struct Cyc_FieldPatternList_tok_struct){.tag=
Cyc_FieldPatternList_tok_tag,.f1=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_FieldPatternList_tok(({ struct _tagged_ptr2
_temp5299= yyvs; struct _xenum_struct** _temp5301= _temp5299.curr + yyvsp_offset;
if( _temp5301 < _temp5299.base? 1: _temp5301 >= _temp5299.last_plus_one){ _throw(
Null_Exception);}* _temp5301;})))};( struct _xenum_struct*) _temp5298;}); break;
case 272: _LL5297: yyval=({ struct Cyc_FieldPatternList_tok_struct* _temp5303=(
struct Cyc_FieldPatternList_tok_struct*) GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct));*
_temp5303=( struct Cyc_FieldPatternList_tok_struct){.tag= Cyc_FieldPatternList_tok_tag,.f1=({
struct Cyc_List_List* _temp5304=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp5304->hd=( void*) Cyc_yyget_FieldPattern_tok(({
struct _tagged_ptr2 _temp5305= yyvs; struct _xenum_struct** _temp5307= _temp5305.curr
+ yyvsp_offset; if( _temp5307 < _temp5305.base? 1: _temp5307 >= _temp5305.last_plus_one){
_throw( Null_Exception);}* _temp5307;})); _temp5304->tl= 0; _temp5304;})};(
struct _xenum_struct*) _temp5303;}); break; case 273: _LL5302: yyval=({ struct
Cyc_FieldPatternList_tok_struct* _temp5309=( struct Cyc_FieldPatternList_tok_struct*)
GC_malloc( sizeof( struct Cyc_FieldPatternList_tok_struct));* _temp5309=( struct
Cyc_FieldPatternList_tok_struct){.tag= Cyc_FieldPatternList_tok_tag,.f1=({
struct Cyc_List_List* _temp5310=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp5310->hd=( void*) Cyc_yyget_FieldPattern_tok(({
struct _tagged_ptr2 _temp5314= yyvs; struct _xenum_struct** _temp5316= _temp5314.curr
+ yyvsp_offset; if( _temp5316 < _temp5314.base? 1: _temp5316 >= _temp5314.last_plus_one){
_throw( Null_Exception);}* _temp5316;})); _temp5310->tl= Cyc_yyget_FieldPatternList_tok(({
struct _tagged_ptr2 _temp5311= yyvs; struct _xenum_struct** _temp5313= _temp5311.curr
+( yyvsp_offset - 2); if( _temp5313 < _temp5311.base? 1: _temp5313 >= _temp5311.last_plus_one){
_throw( Null_Exception);}* _temp5313;})); _temp5310;})};( struct _xenum_struct*)
_temp5309;}); break; case 274: _LL5308: yyval=({ struct _tagged_ptr2 _temp5318=
yyvs; struct _xenum_struct** _temp5320= _temp5318.curr + yyvsp_offset; if(
_temp5320 < _temp5318.base? 1: _temp5320 >= _temp5318.last_plus_one){ _throw(
Null_Exception);}* _temp5320;}); break; case 275: _LL5317: yyval=({ struct Cyc_Exp_tok_struct*
_temp5322=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5322=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_seq_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5323= yyvs; struct _xenum_struct**
_temp5325= _temp5323.curr +( yyvsp_offset - 2); if( _temp5325 < _temp5323.base?
1: _temp5325 >= _temp5323.last_plus_one){ _throw( Null_Exception);}* _temp5325;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5326= yyvs; struct _xenum_struct**
_temp5328= _temp5326.curr + yyvsp_offset; if( _temp5328 < _temp5326.base? 1:
_temp5328 >= _temp5326.last_plus_one){ _throw( Null_Exception);}* _temp5328;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5329= yyls; struct Cyc_Yyltype*
_temp5331= _temp5329.curr +( yylsp_offset - 2); if( _temp5331 < _temp5329.base?
1: _temp5331 >= _temp5329.last_plus_one){ _throw( Null_Exception);}* _temp5331;}).first_line,({
struct _tagged_ptr3 _temp5332= yyls; struct Cyc_Yyltype* _temp5334= _temp5332.curr
+ yylsp_offset; if( _temp5334 < _temp5332.base? 1: _temp5334 >= _temp5332.last_plus_one){
_throw( Null_Exception);}* _temp5334;}).last_line))};( struct _xenum_struct*)
_temp5322;}); break; case 276: _LL5321: yyval=({ struct _tagged_ptr2 _temp5336=
yyvs; struct _xenum_struct** _temp5338= _temp5336.curr + yyvsp_offset; if(
_temp5338 < _temp5336.base? 1: _temp5338 >= _temp5336.last_plus_one){ _throw(
Null_Exception);}* _temp5338;}); break; case 277: _LL5335: yyval=({ struct Cyc_Exp_tok_struct*
_temp5340=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5340=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_assignop_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5341= yyvs; struct _xenum_struct**
_temp5343= _temp5341.curr +( yyvsp_offset - 2); if( _temp5343 < _temp5341.base?
1: _temp5343 >= _temp5341.last_plus_one){ _throw( Null_Exception);}* _temp5343;})),
Cyc_yyget_Primopopt_tok(({ struct _tagged_ptr2 _temp5344= yyvs; struct
_xenum_struct** _temp5346= _temp5344.curr +( yyvsp_offset - 1); if( _temp5346 <
_temp5344.base? 1: _temp5346 >= _temp5344.last_plus_one){ _throw( Null_Exception);}*
_temp5346;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5347= yyvs; struct
_xenum_struct** _temp5349= _temp5347.curr + yyvsp_offset; if( _temp5349 <
_temp5347.base? 1: _temp5349 >= _temp5347.last_plus_one){ _throw( Null_Exception);}*
_temp5349;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5350=
yyls; struct Cyc_Yyltype* _temp5352= _temp5350.curr +( yylsp_offset - 2); if(
_temp5352 < _temp5350.base? 1: _temp5352 >= _temp5350.last_plus_one){ _throw(
Null_Exception);}* _temp5352;}).first_line,({ struct _tagged_ptr3 _temp5353=
yyls; struct Cyc_Yyltype* _temp5355= _temp5353.curr + yylsp_offset; if(
_temp5355 < _temp5353.base? 1: _temp5355 >= _temp5353.last_plus_one){ _throw(
Null_Exception);}* _temp5355;}).last_line))};( struct _xenum_struct*) _temp5340;});
break; case 278: _LL5339: yyval=({ struct Cyc_Primopopt_tok_struct* _temp5357=(
struct Cyc_Primopopt_tok_struct*) GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));*
_temp5357=( struct Cyc_Primopopt_tok_struct){.tag= Cyc_Primopopt_tok_tag,.f1= 0};(
struct _xenum_struct*) _temp5357;}); break; case 279: _LL5356: yyval=({ struct
Cyc_Primopopt_tok_struct* _temp5359=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));* _temp5359=( struct Cyc_Primopopt_tok_struct){.tag=
Cyc_Primopopt_tok_tag,.f1=({ struct Cyc_Core_Opt* _temp5360=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp5360->v=( void*) Cyc_Absyn_Times;
_temp5360;})};( struct _xenum_struct*) _temp5359;}); break; case 280: _LL5358:
yyval=({ struct Cyc_Primopopt_tok_struct* _temp5362=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));* _temp5362=( struct Cyc_Primopopt_tok_struct){.tag=
Cyc_Primopopt_tok_tag,.f1=({ struct Cyc_Core_Opt* _temp5363=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp5363->v=( void*) Cyc_Absyn_Div;
_temp5363;})};( struct _xenum_struct*) _temp5362;}); break; case 281: _LL5361:
yyval=({ struct Cyc_Primopopt_tok_struct* _temp5365=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));* _temp5365=( struct Cyc_Primopopt_tok_struct){.tag=
Cyc_Primopopt_tok_tag,.f1=({ struct Cyc_Core_Opt* _temp5366=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp5366->v=( void*) Cyc_Absyn_Mod;
_temp5366;})};( struct _xenum_struct*) _temp5365;}); break; case 282: _LL5364:
yyval=({ struct Cyc_Primopopt_tok_struct* _temp5368=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));* _temp5368=( struct Cyc_Primopopt_tok_struct){.tag=
Cyc_Primopopt_tok_tag,.f1=({ struct Cyc_Core_Opt* _temp5369=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp5369->v=( void*) Cyc_Absyn_Plus;
_temp5369;})};( struct _xenum_struct*) _temp5368;}); break; case 283: _LL5367:
yyval=({ struct Cyc_Primopopt_tok_struct* _temp5371=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));* _temp5371=( struct Cyc_Primopopt_tok_struct){.tag=
Cyc_Primopopt_tok_tag,.f1=({ struct Cyc_Core_Opt* _temp5372=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp5372->v=( void*) Cyc_Absyn_Minus;
_temp5372;})};( struct _xenum_struct*) _temp5371;}); break; case 284: _LL5370:
yyval=({ struct Cyc_Primopopt_tok_struct* _temp5374=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));* _temp5374=( struct Cyc_Primopopt_tok_struct){.tag=
Cyc_Primopopt_tok_tag,.f1=({ struct Cyc_Core_Opt* _temp5375=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp5375->v=( void*) Cyc_Absyn_Bitlshift;
_temp5375;})};( struct _xenum_struct*) _temp5374;}); break; case 285: _LL5373:
yyval=({ struct Cyc_Primopopt_tok_struct* _temp5377=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));* _temp5377=( struct Cyc_Primopopt_tok_struct){.tag=
Cyc_Primopopt_tok_tag,.f1=({ struct Cyc_Core_Opt* _temp5378=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp5378->v=( void*) Cyc_Absyn_Bitlrshift;
_temp5378;})};( struct _xenum_struct*) _temp5377;}); break; case 286: _LL5376:
yyval=({ struct Cyc_Primopopt_tok_struct* _temp5380=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));* _temp5380=( struct Cyc_Primopopt_tok_struct){.tag=
Cyc_Primopopt_tok_tag,.f1=({ struct Cyc_Core_Opt* _temp5381=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp5381->v=( void*) Cyc_Absyn_Bitand;
_temp5381;})};( struct _xenum_struct*) _temp5380;}); break; case 287: _LL5379:
yyval=({ struct Cyc_Primopopt_tok_struct* _temp5383=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));* _temp5383=( struct Cyc_Primopopt_tok_struct){.tag=
Cyc_Primopopt_tok_tag,.f1=({ struct Cyc_Core_Opt* _temp5384=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp5384->v=( void*) Cyc_Absyn_Bitxor;
_temp5384;})};( struct _xenum_struct*) _temp5383;}); break; case 288: _LL5382:
yyval=({ struct Cyc_Primopopt_tok_struct* _temp5386=( struct Cyc_Primopopt_tok_struct*)
GC_malloc( sizeof( struct Cyc_Primopopt_tok_struct));* _temp5386=( struct Cyc_Primopopt_tok_struct){.tag=
Cyc_Primopopt_tok_tag,.f1=({ struct Cyc_Core_Opt* _temp5387=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp5387->v=( void*) Cyc_Absyn_Bitor;
_temp5387;})};( struct _xenum_struct*) _temp5386;}); break; case 289: _LL5385:
yyval=({ struct _tagged_ptr2 _temp5389= yyvs; struct _xenum_struct** _temp5391=
_temp5389.curr + yyvsp_offset; if( _temp5391 < _temp5389.base? 1: _temp5391 >=
_temp5389.last_plus_one){ _throw( Null_Exception);}* _temp5391;}); break; case
290: _LL5388: yyval=({ struct Cyc_Exp_tok_struct* _temp5393=( struct Cyc_Exp_tok_struct*)
GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp5393=( struct Cyc_Exp_tok_struct){.tag=
Cyc_Exp_tok_tag,.f1= Cyc_Absyn_conditional_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5394= yyvs; struct _xenum_struct** _temp5396= _temp5394.curr +(
yyvsp_offset - 4); if( _temp5396 < _temp5394.base? 1: _temp5396 >= _temp5394.last_plus_one){
_throw( Null_Exception);}* _temp5396;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5397= yyvs; struct _xenum_struct** _temp5399= _temp5397.curr +(
yyvsp_offset - 2); if( _temp5399 < _temp5397.base? 1: _temp5399 >= _temp5397.last_plus_one){
_throw( Null_Exception);}* _temp5399;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5400= yyvs; struct _xenum_struct** _temp5402= _temp5400.curr +
yyvsp_offset; if( _temp5402 < _temp5400.base? 1: _temp5402 >= _temp5400.last_plus_one){
_throw( Null_Exception);}* _temp5402;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5403= yyls; struct Cyc_Yyltype* _temp5405= _temp5403.curr +(
yylsp_offset - 4); if( _temp5405 < _temp5403.base? 1: _temp5405 >= _temp5403.last_plus_one){
_throw( Null_Exception);}* _temp5405;}).first_line,({ struct _tagged_ptr3
_temp5406= yyls; struct Cyc_Yyltype* _temp5408= _temp5406.curr + yylsp_offset;
if( _temp5408 < _temp5406.base? 1: _temp5408 >= _temp5406.last_plus_one){ _throw(
Null_Exception);}* _temp5408;}).last_line))};( struct _xenum_struct*) _temp5393;});
break; case 291: _LL5392: yyval=({ struct _tagged_ptr2 _temp5410= yyvs; struct
_xenum_struct** _temp5412= _temp5410.curr + yyvsp_offset; if( _temp5412 <
_temp5410.base? 1: _temp5412 >= _temp5410.last_plus_one){ _throw( Null_Exception);}*
_temp5412;}); break; case 292: _LL5409: yyval=({ struct _tagged_ptr2 _temp5414=
yyvs; struct _xenum_struct** _temp5416= _temp5414.curr + yyvsp_offset; if(
_temp5416 < _temp5414.base? 1: _temp5416 >= _temp5414.last_plus_one){ _throw(
Null_Exception);}* _temp5416;}); break; case 293: _LL5413: yyval=({ struct Cyc_Exp_tok_struct*
_temp5418=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5418=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_or_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5419= yyvs; struct _xenum_struct**
_temp5421= _temp5419.curr +( yyvsp_offset - 2); if( _temp5421 < _temp5419.base?
1: _temp5421 >= _temp5419.last_plus_one){ _throw( Null_Exception);}* _temp5421;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5422= yyvs; struct _xenum_struct**
_temp5424= _temp5422.curr + yyvsp_offset; if( _temp5424 < _temp5422.base? 1:
_temp5424 >= _temp5422.last_plus_one){ _throw( Null_Exception);}* _temp5424;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5425= yyls; struct Cyc_Yyltype*
_temp5427= _temp5425.curr +( yylsp_offset - 2); if( _temp5427 < _temp5425.base?
1: _temp5427 >= _temp5425.last_plus_one){ _throw( Null_Exception);}* _temp5427;}).first_line,({
struct _tagged_ptr3 _temp5428= yyls; struct Cyc_Yyltype* _temp5430= _temp5428.curr
+ yylsp_offset; if( _temp5430 < _temp5428.base? 1: _temp5430 >= _temp5428.last_plus_one){
_throw( Null_Exception);}* _temp5430;}).last_line))};( struct _xenum_struct*)
_temp5418;}); break; case 294: _LL5417: yyval=({ struct _tagged_ptr2 _temp5432=
yyvs; struct _xenum_struct** _temp5434= _temp5432.curr + yyvsp_offset; if(
_temp5434 < _temp5432.base? 1: _temp5434 >= _temp5432.last_plus_one){ _throw(
Null_Exception);}* _temp5434;}); break; case 295: _LL5431: yyval=({ struct Cyc_Exp_tok_struct*
_temp5436=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5436=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_and_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5437= yyvs; struct _xenum_struct**
_temp5439= _temp5437.curr +( yyvsp_offset - 2); if( _temp5439 < _temp5437.base?
1: _temp5439 >= _temp5437.last_plus_one){ _throw( Null_Exception);}* _temp5439;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5440= yyvs; struct _xenum_struct**
_temp5442= _temp5440.curr + yyvsp_offset; if( _temp5442 < _temp5440.base? 1:
_temp5442 >= _temp5440.last_plus_one){ _throw( Null_Exception);}* _temp5442;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5443= yyls; struct Cyc_Yyltype*
_temp5445= _temp5443.curr +( yylsp_offset - 2); if( _temp5445 < _temp5443.base?
1: _temp5445 >= _temp5443.last_plus_one){ _throw( Null_Exception);}* _temp5445;}).first_line,({
struct _tagged_ptr3 _temp5446= yyls; struct Cyc_Yyltype* _temp5448= _temp5446.curr
+ yylsp_offset; if( _temp5448 < _temp5446.base? 1: _temp5448 >= _temp5446.last_plus_one){
_throw( Null_Exception);}* _temp5448;}).last_line))};( struct _xenum_struct*)
_temp5436;}); break; case 296: _LL5435: yyval=({ struct _tagged_ptr2 _temp5450=
yyvs; struct _xenum_struct** _temp5452= _temp5450.curr + yyvsp_offset; if(
_temp5452 < _temp5450.base? 1: _temp5452 >= _temp5450.last_plus_one){ _throw(
Null_Exception);}* _temp5452;}); break; case 297: _LL5449: yyval=({ struct Cyc_Exp_tok_struct*
_temp5454=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5454=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_prim2_exp(
Cyc_Absyn_Bitor, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5455= yyvs;
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
break; case 298: _LL5453: yyval=({ struct _tagged_ptr2 _temp5468= yyvs; struct
_xenum_struct** _temp5470= _temp5468.curr + yyvsp_offset; if( _temp5470 <
_temp5468.base? 1: _temp5470 >= _temp5468.last_plus_one){ _throw( Null_Exception);}*
_temp5470;}); break; case 299: _LL5467: yyval=({ struct Cyc_Exp_tok_struct*
_temp5472=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5472=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_prim2_exp(
Cyc_Absyn_Bitxor, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5473= yyvs;
struct _xenum_struct** _temp5475= _temp5473.curr +( yyvsp_offset - 2); if(
_temp5475 < _temp5473.base? 1: _temp5475 >= _temp5473.last_plus_one){ _throw(
Null_Exception);}* _temp5475;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5476= yyvs; struct _xenum_struct** _temp5478= _temp5476.curr + yyvsp_offset;
if( _temp5478 < _temp5476.base? 1: _temp5478 >= _temp5476.last_plus_one){ _throw(
Null_Exception);}* _temp5478;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5479= yyls; struct Cyc_Yyltype* _temp5481= _temp5479.curr +(
yylsp_offset - 2); if( _temp5481 < _temp5479.base? 1: _temp5481 >= _temp5479.last_plus_one){
_throw( Null_Exception);}* _temp5481;}).first_line,({ struct _tagged_ptr3
_temp5482= yyls; struct Cyc_Yyltype* _temp5484= _temp5482.curr + yylsp_offset;
if( _temp5484 < _temp5482.base? 1: _temp5484 >= _temp5482.last_plus_one){ _throw(
Null_Exception);}* _temp5484;}).last_line))};( struct _xenum_struct*) _temp5472;});
break; case 300: _LL5471: yyval=({ struct _tagged_ptr2 _temp5486= yyvs; struct
_xenum_struct** _temp5488= _temp5486.curr + yyvsp_offset; if( _temp5488 <
_temp5486.base? 1: _temp5488 >= _temp5486.last_plus_one){ _throw( Null_Exception);}*
_temp5488;}); break; case 301: _LL5485: yyval=({ struct Cyc_Exp_tok_struct*
_temp5490=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5490=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_prim2_exp(
Cyc_Absyn_Bitand, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5491= yyvs;
struct _xenum_struct** _temp5493= _temp5491.curr +( yyvsp_offset - 2); if(
_temp5493 < _temp5491.base? 1: _temp5493 >= _temp5491.last_plus_one){ _throw(
Null_Exception);}* _temp5493;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5494= yyvs; struct _xenum_struct** _temp5496= _temp5494.curr + yyvsp_offset;
if( _temp5496 < _temp5494.base? 1: _temp5496 >= _temp5494.last_plus_one){ _throw(
Null_Exception);}* _temp5496;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5497= yyls; struct Cyc_Yyltype* _temp5499= _temp5497.curr +(
yylsp_offset - 2); if( _temp5499 < _temp5497.base? 1: _temp5499 >= _temp5497.last_plus_one){
_throw( Null_Exception);}* _temp5499;}).first_line,({ struct _tagged_ptr3
_temp5500= yyls; struct Cyc_Yyltype* _temp5502= _temp5500.curr + yylsp_offset;
if( _temp5502 < _temp5500.base? 1: _temp5502 >= _temp5500.last_plus_one){ _throw(
Null_Exception);}* _temp5502;}).last_line))};( struct _xenum_struct*) _temp5490;});
break; case 302: _LL5489: yyval=({ struct _tagged_ptr2 _temp5504= yyvs; struct
_xenum_struct** _temp5506= _temp5504.curr + yyvsp_offset; if( _temp5506 <
_temp5504.base? 1: _temp5506 >= _temp5504.last_plus_one){ _throw( Null_Exception);}*
_temp5506;}); break; case 303: _LL5503: yyval=({ struct Cyc_Exp_tok_struct*
_temp5508=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5508=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_eq_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5509= yyvs; struct _xenum_struct**
_temp5511= _temp5509.curr +( yyvsp_offset - 2); if( _temp5511 < _temp5509.base?
1: _temp5511 >= _temp5509.last_plus_one){ _throw( Null_Exception);}* _temp5511;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5512= yyvs; struct _xenum_struct**
_temp5514= _temp5512.curr + yyvsp_offset; if( _temp5514 < _temp5512.base? 1:
_temp5514 >= _temp5512.last_plus_one){ _throw( Null_Exception);}* _temp5514;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5515= yyls; struct Cyc_Yyltype*
_temp5517= _temp5515.curr +( yylsp_offset - 2); if( _temp5517 < _temp5515.base?
1: _temp5517 >= _temp5515.last_plus_one){ _throw( Null_Exception);}* _temp5517;}).first_line,({
struct _tagged_ptr3 _temp5518= yyls; struct Cyc_Yyltype* _temp5520= _temp5518.curr
+ yylsp_offset; if( _temp5520 < _temp5518.base? 1: _temp5520 >= _temp5518.last_plus_one){
_throw( Null_Exception);}* _temp5520;}).last_line))};( struct _xenum_struct*)
_temp5508;}); break; case 304: _LL5507: yyval=({ struct Cyc_Exp_tok_struct*
_temp5522=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5522=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_neq_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5523= yyvs; struct _xenum_struct**
_temp5525= _temp5523.curr +( yyvsp_offset - 2); if( _temp5525 < _temp5523.base?
1: _temp5525 >= _temp5523.last_plus_one){ _throw( Null_Exception);}* _temp5525;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5526= yyvs; struct _xenum_struct**
_temp5528= _temp5526.curr + yyvsp_offset; if( _temp5528 < _temp5526.base? 1:
_temp5528 >= _temp5526.last_plus_one){ _throw( Null_Exception);}* _temp5528;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5529= yyls; struct Cyc_Yyltype*
_temp5531= _temp5529.curr +( yylsp_offset - 2); if( _temp5531 < _temp5529.base?
1: _temp5531 >= _temp5529.last_plus_one){ _throw( Null_Exception);}* _temp5531;}).first_line,({
struct _tagged_ptr3 _temp5532= yyls; struct Cyc_Yyltype* _temp5534= _temp5532.curr
+ yylsp_offset; if( _temp5534 < _temp5532.base? 1: _temp5534 >= _temp5532.last_plus_one){
_throw( Null_Exception);}* _temp5534;}).last_line))};( struct _xenum_struct*)
_temp5522;}); break; case 305: _LL5521: yyval=({ struct _tagged_ptr2 _temp5536=
yyvs; struct _xenum_struct** _temp5538= _temp5536.curr + yyvsp_offset; if(
_temp5538 < _temp5536.base? 1: _temp5538 >= _temp5536.last_plus_one){ _throw(
Null_Exception);}* _temp5538;}); break; case 306: _LL5535: yyval=({ struct Cyc_Exp_tok_struct*
_temp5540=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5540=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_lt_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5541= yyvs; struct _xenum_struct**
_temp5543= _temp5541.curr +( yyvsp_offset - 2); if( _temp5543 < _temp5541.base?
1: _temp5543 >= _temp5541.last_plus_one){ _throw( Null_Exception);}* _temp5543;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5544= yyvs; struct _xenum_struct**
_temp5546= _temp5544.curr + yyvsp_offset; if( _temp5546 < _temp5544.base? 1:
_temp5546 >= _temp5544.last_plus_one){ _throw( Null_Exception);}* _temp5546;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5547= yyls; struct Cyc_Yyltype*
_temp5549= _temp5547.curr +( yylsp_offset - 2); if( _temp5549 < _temp5547.base?
1: _temp5549 >= _temp5547.last_plus_one){ _throw( Null_Exception);}* _temp5549;}).first_line,({
struct _tagged_ptr3 _temp5550= yyls; struct Cyc_Yyltype* _temp5552= _temp5550.curr
+ yylsp_offset; if( _temp5552 < _temp5550.base? 1: _temp5552 >= _temp5550.last_plus_one){
_throw( Null_Exception);}* _temp5552;}).last_line))};( struct _xenum_struct*)
_temp5540;}); break; case 307: _LL5539: yyval=({ struct Cyc_Exp_tok_struct*
_temp5554=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5554=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_gt_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5555= yyvs; struct _xenum_struct**
_temp5557= _temp5555.curr +( yyvsp_offset - 2); if( _temp5557 < _temp5555.base?
1: _temp5557 >= _temp5555.last_plus_one){ _throw( Null_Exception);}* _temp5557;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5558= yyvs; struct _xenum_struct**
_temp5560= _temp5558.curr + yyvsp_offset; if( _temp5560 < _temp5558.base? 1:
_temp5560 >= _temp5558.last_plus_one){ _throw( Null_Exception);}* _temp5560;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5561= yyls; struct Cyc_Yyltype*
_temp5563= _temp5561.curr +( yylsp_offset - 2); if( _temp5563 < _temp5561.base?
1: _temp5563 >= _temp5561.last_plus_one){ _throw( Null_Exception);}* _temp5563;}).first_line,({
struct _tagged_ptr3 _temp5564= yyls; struct Cyc_Yyltype* _temp5566= _temp5564.curr
+ yylsp_offset; if( _temp5566 < _temp5564.base? 1: _temp5566 >= _temp5564.last_plus_one){
_throw( Null_Exception);}* _temp5566;}).last_line))};( struct _xenum_struct*)
_temp5554;}); break; case 308: _LL5553: yyval=({ struct Cyc_Exp_tok_struct*
_temp5568=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5568=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_lte_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5569= yyvs; struct _xenum_struct**
_temp5571= _temp5569.curr +( yyvsp_offset - 2); if( _temp5571 < _temp5569.base?
1: _temp5571 >= _temp5569.last_plus_one){ _throw( Null_Exception);}* _temp5571;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5572= yyvs; struct _xenum_struct**
_temp5574= _temp5572.curr + yyvsp_offset; if( _temp5574 < _temp5572.base? 1:
_temp5574 >= _temp5572.last_plus_one){ _throw( Null_Exception);}* _temp5574;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5575= yyls; struct Cyc_Yyltype*
_temp5577= _temp5575.curr +( yylsp_offset - 2); if( _temp5577 < _temp5575.base?
1: _temp5577 >= _temp5575.last_plus_one){ _throw( Null_Exception);}* _temp5577;}).first_line,({
struct _tagged_ptr3 _temp5578= yyls; struct Cyc_Yyltype* _temp5580= _temp5578.curr
+ yylsp_offset; if( _temp5580 < _temp5578.base? 1: _temp5580 >= _temp5578.last_plus_one){
_throw( Null_Exception);}* _temp5580;}).last_line))};( struct _xenum_struct*)
_temp5568;}); break; case 309: _LL5567: yyval=({ struct Cyc_Exp_tok_struct*
_temp5582=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5582=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_gte_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5583= yyvs; struct _xenum_struct**
_temp5585= _temp5583.curr +( yyvsp_offset - 2); if( _temp5585 < _temp5583.base?
1: _temp5585 >= _temp5583.last_plus_one){ _throw( Null_Exception);}* _temp5585;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5586= yyvs; struct _xenum_struct**
_temp5588= _temp5586.curr + yyvsp_offset; if( _temp5588 < _temp5586.base? 1:
_temp5588 >= _temp5586.last_plus_one){ _throw( Null_Exception);}* _temp5588;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5589= yyls; struct Cyc_Yyltype*
_temp5591= _temp5589.curr +( yylsp_offset - 2); if( _temp5591 < _temp5589.base?
1: _temp5591 >= _temp5589.last_plus_one){ _throw( Null_Exception);}* _temp5591;}).first_line,({
struct _tagged_ptr3 _temp5592= yyls; struct Cyc_Yyltype* _temp5594= _temp5592.curr
+ yylsp_offset; if( _temp5594 < _temp5592.base? 1: _temp5594 >= _temp5592.last_plus_one){
_throw( Null_Exception);}* _temp5594;}).last_line))};( struct _xenum_struct*)
_temp5582;}); break; case 310: _LL5581: yyval=({ struct _tagged_ptr2 _temp5596=
yyvs; struct _xenum_struct** _temp5598= _temp5596.curr + yyvsp_offset; if(
_temp5598 < _temp5596.base? 1: _temp5598 >= _temp5596.last_plus_one){ _throw(
Null_Exception);}* _temp5598;}); break; case 311: _LL5595: yyval=({ struct Cyc_Exp_tok_struct*
_temp5600=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5600=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_prim2_exp(
Cyc_Absyn_Bitlshift, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5601= yyvs;
struct _xenum_struct** _temp5603= _temp5601.curr +( yyvsp_offset - 2); if(
_temp5603 < _temp5601.base? 1: _temp5603 >= _temp5601.last_plus_one){ _throw(
Null_Exception);}* _temp5603;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5604= yyvs; struct _xenum_struct** _temp5606= _temp5604.curr + yyvsp_offset;
if( _temp5606 < _temp5604.base? 1: _temp5606 >= _temp5604.last_plus_one){ _throw(
Null_Exception);}* _temp5606;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5607= yyls; struct Cyc_Yyltype* _temp5609= _temp5607.curr +(
yylsp_offset - 2); if( _temp5609 < _temp5607.base? 1: _temp5609 >= _temp5607.last_plus_one){
_throw( Null_Exception);}* _temp5609;}).first_line,({ struct _tagged_ptr3
_temp5610= yyls; struct Cyc_Yyltype* _temp5612= _temp5610.curr + yylsp_offset;
if( _temp5612 < _temp5610.base? 1: _temp5612 >= _temp5610.last_plus_one){ _throw(
Null_Exception);}* _temp5612;}).last_line))};( struct _xenum_struct*) _temp5600;});
break; case 312: _LL5599: yyval=({ struct Cyc_Exp_tok_struct* _temp5614=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp5614=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_prim2_exp( Cyc_Absyn_Bitlrshift,
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5615= yyvs; struct _xenum_struct**
_temp5617= _temp5615.curr +( yyvsp_offset - 2); if( _temp5617 < _temp5615.base?
1: _temp5617 >= _temp5615.last_plus_one){ _throw( Null_Exception);}* _temp5617;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5618= yyvs; struct _xenum_struct**
_temp5620= _temp5618.curr + yyvsp_offset; if( _temp5620 < _temp5618.base? 1:
_temp5620 >= _temp5618.last_plus_one){ _throw( Null_Exception);}* _temp5620;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5621= yyls; struct Cyc_Yyltype*
_temp5623= _temp5621.curr +( yylsp_offset - 2); if( _temp5623 < _temp5621.base?
1: _temp5623 >= _temp5621.last_plus_one){ _throw( Null_Exception);}* _temp5623;}).first_line,({
struct _tagged_ptr3 _temp5624= yyls; struct Cyc_Yyltype* _temp5626= _temp5624.curr
+ yylsp_offset; if( _temp5626 < _temp5624.base? 1: _temp5626 >= _temp5624.last_plus_one){
_throw( Null_Exception);}* _temp5626;}).last_line))};( struct _xenum_struct*)
_temp5614;}); break; case 313: _LL5613: yyval=({ struct _tagged_ptr2 _temp5628=
yyvs; struct _xenum_struct** _temp5630= _temp5628.curr + yyvsp_offset; if(
_temp5630 < _temp5628.base? 1: _temp5630 >= _temp5628.last_plus_one){ _throw(
Null_Exception);}* _temp5630;}); break; case 314: _LL5627: yyval=({ struct Cyc_Exp_tok_struct*
_temp5632=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5632=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_prim2_exp(
Cyc_Absyn_Plus, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5633= yyvs; struct
_xenum_struct** _temp5635= _temp5633.curr +( yyvsp_offset - 2); if( _temp5635 <
_temp5633.base? 1: _temp5635 >= _temp5633.last_plus_one){ _throw( Null_Exception);}*
_temp5635;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5636= yyvs; struct
_xenum_struct** _temp5638= _temp5636.curr + yyvsp_offset; if( _temp5638 <
_temp5636.base? 1: _temp5638 >= _temp5636.last_plus_one){ _throw( Null_Exception);}*
_temp5638;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5639=
yyls; struct Cyc_Yyltype* _temp5641= _temp5639.curr +( yylsp_offset - 2); if(
_temp5641 < _temp5639.base? 1: _temp5641 >= _temp5639.last_plus_one){ _throw(
Null_Exception);}* _temp5641;}).first_line,({ struct _tagged_ptr3 _temp5642=
yyls; struct Cyc_Yyltype* _temp5644= _temp5642.curr + yylsp_offset; if(
_temp5644 < _temp5642.base? 1: _temp5644 >= _temp5642.last_plus_one){ _throw(
Null_Exception);}* _temp5644;}).last_line))};( struct _xenum_struct*) _temp5632;});
break; case 315: _LL5631: yyval=({ struct Cyc_Exp_tok_struct* _temp5646=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp5646=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_prim2_exp( Cyc_Absyn_Minus,
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5647= yyvs; struct _xenum_struct**
_temp5649= _temp5647.curr +( yyvsp_offset - 2); if( _temp5649 < _temp5647.base?
1: _temp5649 >= _temp5647.last_plus_one){ _throw( Null_Exception);}* _temp5649;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5650= yyvs; struct _xenum_struct**
_temp5652= _temp5650.curr + yyvsp_offset; if( _temp5652 < _temp5650.base? 1:
_temp5652 >= _temp5650.last_plus_one){ _throw( Null_Exception);}* _temp5652;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5653= yyls; struct Cyc_Yyltype*
_temp5655= _temp5653.curr +( yylsp_offset - 2); if( _temp5655 < _temp5653.base?
1: _temp5655 >= _temp5653.last_plus_one){ _throw( Null_Exception);}* _temp5655;}).first_line,({
struct _tagged_ptr3 _temp5656= yyls; struct Cyc_Yyltype* _temp5658= _temp5656.curr
+ yylsp_offset; if( _temp5658 < _temp5656.base? 1: _temp5658 >= _temp5656.last_plus_one){
_throw( Null_Exception);}* _temp5658;}).last_line))};( struct _xenum_struct*)
_temp5646;}); break; case 316: _LL5645: yyval=({ struct _tagged_ptr2 _temp5660=
yyvs; struct _xenum_struct** _temp5662= _temp5660.curr + yyvsp_offset; if(
_temp5662 < _temp5660.base? 1: _temp5662 >= _temp5660.last_plus_one){ _throw(
Null_Exception);}* _temp5662;}); break; case 317: _LL5659: yyval=({ struct Cyc_Exp_tok_struct*
_temp5664=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5664=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_prim2_exp(
Cyc_Absyn_Times, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5665= yyvs;
struct _xenum_struct** _temp5667= _temp5665.curr +( yyvsp_offset - 2); if(
_temp5667 < _temp5665.base? 1: _temp5667 >= _temp5665.last_plus_one){ _throw(
Null_Exception);}* _temp5667;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2
_temp5668= yyvs; struct _xenum_struct** _temp5670= _temp5668.curr + yyvsp_offset;
if( _temp5670 < _temp5668.base? 1: _temp5670 >= _temp5668.last_plus_one){ _throw(
Null_Exception);}* _temp5670;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5671= yyls; struct Cyc_Yyltype* _temp5673= _temp5671.curr +(
yylsp_offset - 2); if( _temp5673 < _temp5671.base? 1: _temp5673 >= _temp5671.last_plus_one){
_throw( Null_Exception);}* _temp5673;}).first_line,({ struct _tagged_ptr3
_temp5674= yyls; struct Cyc_Yyltype* _temp5676= _temp5674.curr + yylsp_offset;
if( _temp5676 < _temp5674.base? 1: _temp5676 >= _temp5674.last_plus_one){ _throw(
Null_Exception);}* _temp5676;}).last_line))};( struct _xenum_struct*) _temp5664;});
break; case 318: _LL5663: yyval=({ struct Cyc_Exp_tok_struct* _temp5678=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp5678=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_prim2_exp( Cyc_Absyn_Div,
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5679= yyvs; struct _xenum_struct**
_temp5681= _temp5679.curr +( yyvsp_offset - 2); if( _temp5681 < _temp5679.base?
1: _temp5681 >= _temp5679.last_plus_one){ _throw( Null_Exception);}* _temp5681;})),
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5682= yyvs; struct _xenum_struct**
_temp5684= _temp5682.curr + yyvsp_offset; if( _temp5684 < _temp5682.base? 1:
_temp5684 >= _temp5682.last_plus_one){ _throw( Null_Exception);}* _temp5684;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5685= yyls; struct Cyc_Yyltype*
_temp5687= _temp5685.curr +( yylsp_offset - 2); if( _temp5687 < _temp5685.base?
1: _temp5687 >= _temp5685.last_plus_one){ _throw( Null_Exception);}* _temp5687;}).first_line,({
struct _tagged_ptr3 _temp5688= yyls; struct Cyc_Yyltype* _temp5690= _temp5688.curr
+ yylsp_offset; if( _temp5690 < _temp5688.base? 1: _temp5690 >= _temp5688.last_plus_one){
_throw( Null_Exception);}* _temp5690;}).last_line))};( struct _xenum_struct*)
_temp5678;}); break; case 319: _LL5677: yyval=({ struct Cyc_Exp_tok_struct*
_temp5692=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5692=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_prim2_exp(
Cyc_Absyn_Mod, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5693= yyvs; struct
_xenum_struct** _temp5695= _temp5693.curr +( yyvsp_offset - 2); if( _temp5695 <
_temp5693.base? 1: _temp5695 >= _temp5693.last_plus_one){ _throw( Null_Exception);}*
_temp5695;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5696= yyvs; struct
_xenum_struct** _temp5698= _temp5696.curr + yyvsp_offset; if( _temp5698 <
_temp5696.base? 1: _temp5698 >= _temp5696.last_plus_one){ _throw( Null_Exception);}*
_temp5698;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5699=
yyls; struct Cyc_Yyltype* _temp5701= _temp5699.curr +( yylsp_offset - 2); if(
_temp5701 < _temp5699.base? 1: _temp5701 >= _temp5699.last_plus_one){ _throw(
Null_Exception);}* _temp5701;}).first_line,({ struct _tagged_ptr3 _temp5702=
yyls; struct Cyc_Yyltype* _temp5704= _temp5702.curr + yylsp_offset; if(
_temp5704 < _temp5702.base? 1: _temp5704 >= _temp5702.last_plus_one){ _throw(
Null_Exception);}* _temp5704;}).last_line))};( struct _xenum_struct*) _temp5692;});
break; case 320: _LL5691: yyval=({ struct _tagged_ptr2 _temp5706= yyvs; struct
_xenum_struct** _temp5708= _temp5706.curr + yyvsp_offset; if( _temp5708 <
_temp5706.base? 1: _temp5708 >= _temp5706.last_plus_one){ _throw( Null_Exception);}*
_temp5708;}); break; case 321: _LL5705: yyval=({ struct Cyc_Exp_tok_struct*
_temp5710=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5710=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_cast_exp((*
Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp5711= yyvs; struct
_xenum_struct** _temp5713= _temp5711.curr +( yyvsp_offset - 2); if( _temp5713 <
_temp5711.base? 1: _temp5713 >= _temp5711.last_plus_one){ _throw( Null_Exception);}*
_temp5713;}))).f3, Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5714= yyvs;
struct _xenum_struct** _temp5716= _temp5714.curr + yyvsp_offset; if( _temp5716 <
_temp5714.base? 1: _temp5716 >= _temp5714.last_plus_one){ _throw( Null_Exception);}*
_temp5716;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5717=
yyls; struct Cyc_Yyltype* _temp5719= _temp5717.curr +( yylsp_offset - 3); if(
_temp5719 < _temp5717.base? 1: _temp5719 >= _temp5717.last_plus_one){ _throw(
Null_Exception);}* _temp5719;}).first_line,({ struct _tagged_ptr3 _temp5720=
yyls; struct Cyc_Yyltype* _temp5722= _temp5720.curr + yylsp_offset; if(
_temp5722 < _temp5720.base? 1: _temp5722 >= _temp5720.last_plus_one){ _throw(
Null_Exception);}* _temp5722;}).last_line))};( struct _xenum_struct*) _temp5710;});
break; case 322: _LL5709: yyval=({ struct _tagged_ptr2 _temp5724= yyvs; struct
_xenum_struct** _temp5726= _temp5724.curr + yyvsp_offset; if( _temp5726 <
_temp5724.base? 1: _temp5726 >= _temp5724.last_plus_one){ _throw( Null_Exception);}*
_temp5726;}); break; case 323: _LL5723: yyval=({ struct Cyc_Exp_tok_struct*
_temp5728=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5728=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_pre_inc_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5729= yyvs; struct _xenum_struct**
_temp5731= _temp5729.curr + yyvsp_offset; if( _temp5731 < _temp5729.base? 1:
_temp5731 >= _temp5729.last_plus_one){ _throw( Null_Exception);}* _temp5731;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5732= yyls; struct Cyc_Yyltype*
_temp5734= _temp5732.curr +( yylsp_offset - 1); if( _temp5734 < _temp5732.base?
1: _temp5734 >= _temp5732.last_plus_one){ _throw( Null_Exception);}* _temp5734;}).first_line,({
struct _tagged_ptr3 _temp5735= yyls; struct Cyc_Yyltype* _temp5737= _temp5735.curr
+ yylsp_offset; if( _temp5737 < _temp5735.base? 1: _temp5737 >= _temp5735.last_plus_one){
_throw( Null_Exception);}* _temp5737;}).last_line))};( struct _xenum_struct*)
_temp5728;}); break; case 324: _LL5727: yyval=({ struct Cyc_Exp_tok_struct*
_temp5739=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5739=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_pre_dec_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5740= yyvs; struct _xenum_struct**
_temp5742= _temp5740.curr + yyvsp_offset; if( _temp5742 < _temp5740.base? 1:
_temp5742 >= _temp5740.last_plus_one){ _throw( Null_Exception);}* _temp5742;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5743= yyls; struct Cyc_Yyltype*
_temp5745= _temp5743.curr +( yylsp_offset - 1); if( _temp5745 < _temp5743.base?
1: _temp5745 >= _temp5743.last_plus_one){ _throw( Null_Exception);}* _temp5745;}).first_line,({
struct _tagged_ptr3 _temp5746= yyls; struct Cyc_Yyltype* _temp5748= _temp5746.curr
+ yylsp_offset; if( _temp5748 < _temp5746.base? 1: _temp5748 >= _temp5746.last_plus_one){
_throw( Null_Exception);}* _temp5748;}).last_line))};( struct _xenum_struct*)
_temp5739;}); break; case 325: _LL5738: yyval=({ struct Cyc_Exp_tok_struct*
_temp5750=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5750=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_address_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5751= yyvs; struct _xenum_struct**
_temp5753= _temp5751.curr + yyvsp_offset; if( _temp5753 < _temp5751.base? 1:
_temp5753 >= _temp5751.last_plus_one){ _throw( Null_Exception);}* _temp5753;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5754= yyls; struct Cyc_Yyltype*
_temp5756= _temp5754.curr +( yylsp_offset - 1); if( _temp5756 < _temp5754.base?
1: _temp5756 >= _temp5754.last_plus_one){ _throw( Null_Exception);}* _temp5756;}).first_line,({
struct _tagged_ptr3 _temp5757= yyls; struct Cyc_Yyltype* _temp5759= _temp5757.curr
+ yylsp_offset; if( _temp5759 < _temp5757.base? 1: _temp5759 >= _temp5757.last_plus_one){
_throw( Null_Exception);}* _temp5759;}).last_line))};( struct _xenum_struct*)
_temp5750;}); break; case 326: _LL5749: yyval=({ struct Cyc_Exp_tok_struct*
_temp5761=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5761=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_deref_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5762= yyvs; struct _xenum_struct**
_temp5764= _temp5762.curr + yyvsp_offset; if( _temp5764 < _temp5762.base? 1:
_temp5764 >= _temp5762.last_plus_one){ _throw( Null_Exception);}* _temp5764;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5765= yyls; struct Cyc_Yyltype*
_temp5767= _temp5765.curr +( yylsp_offset - 1); if( _temp5767 < _temp5765.base?
1: _temp5767 >= _temp5765.last_plus_one){ _throw( Null_Exception);}* _temp5767;}).first_line,({
struct _tagged_ptr3 _temp5768= yyls; struct Cyc_Yyltype* _temp5770= _temp5768.curr
+ yylsp_offset; if( _temp5770 < _temp5768.base? 1: _temp5770 >= _temp5768.last_plus_one){
_throw( Null_Exception);}* _temp5770;}).last_line))};( struct _xenum_struct*)
_temp5761;}); break; case 327: _LL5760: yyval=({ struct _tagged_ptr2 _temp5772=
yyvs; struct _xenum_struct** _temp5774= _temp5772.curr + yyvsp_offset; if(
_temp5774 < _temp5772.base? 1: _temp5774 >= _temp5772.last_plus_one){ _throw(
Null_Exception);}* _temp5774;}); break; case 328: _LL5771: yyval=({ struct Cyc_Exp_tok_struct*
_temp5776=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5776=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_prim1_exp(
Cyc_yyget_Primop_tok(({ struct _tagged_ptr2 _temp5777= yyvs; struct
_xenum_struct** _temp5779= _temp5777.curr +( yyvsp_offset - 1); if( _temp5779 <
_temp5777.base? 1: _temp5779 >= _temp5777.last_plus_one){ _throw( Null_Exception);}*
_temp5779;})), Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5780= yyvs; struct
_xenum_struct** _temp5782= _temp5780.curr + yyvsp_offset; if( _temp5782 <
_temp5780.base? 1: _temp5782 >= _temp5780.last_plus_one){ _throw( Null_Exception);}*
_temp5782;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5783=
yyls; struct Cyc_Yyltype* _temp5785= _temp5783.curr +( yylsp_offset - 1); if(
_temp5785 < _temp5783.base? 1: _temp5785 >= _temp5783.last_plus_one){ _throw(
Null_Exception);}* _temp5785;}).first_line,({ struct _tagged_ptr3 _temp5786=
yyls; struct Cyc_Yyltype* _temp5788= _temp5786.curr + yylsp_offset; if(
_temp5788 < _temp5786.base? 1: _temp5788 >= _temp5786.last_plus_one){ _throw(
Null_Exception);}* _temp5788;}).last_line))};( struct _xenum_struct*) _temp5776;});
break; case 329: _LL5775: yyval=({ struct Cyc_Exp_tok_struct* _temp5790=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp5790=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_sizeof_exp((*
Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp5791= yyvs; struct
_xenum_struct** _temp5793= _temp5791.curr +( yyvsp_offset - 1); if( _temp5793 <
_temp5791.base? 1: _temp5793 >= _temp5791.last_plus_one){ _throw( Null_Exception);}*
_temp5793;}))).f3, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5794=
yyls; struct Cyc_Yyltype* _temp5796= _temp5794.curr +( yylsp_offset - 3); if(
_temp5796 < _temp5794.base? 1: _temp5796 >= _temp5794.last_plus_one){ _throw(
Null_Exception);}* _temp5796;}).first_line,({ struct _tagged_ptr3 _temp5797=
yyls; struct Cyc_Yyltype* _temp5799= _temp5797.curr + yylsp_offset; if(
_temp5799 < _temp5797.base? 1: _temp5799 >= _temp5797.last_plus_one){ _throw(
Null_Exception);}* _temp5799;}).last_line))};( struct _xenum_struct*) _temp5790;});
break; case 330: _LL5789: yyval=({ struct Cyc_Exp_tok_struct* _temp5801=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp5801=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_prim1_exp( Cyc_Absyn_Size,
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5802= yyvs; struct _xenum_struct**
_temp5804= _temp5802.curr + yyvsp_offset; if( _temp5804 < _temp5802.base? 1:
_temp5804 >= _temp5802.last_plus_one){ _throw( Null_Exception);}* _temp5804;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5805= yyls; struct Cyc_Yyltype*
_temp5807= _temp5805.curr +( yylsp_offset - 1); if( _temp5807 < _temp5805.base?
1: _temp5807 >= _temp5805.last_plus_one){ _throw( Null_Exception);}* _temp5807;}).first_line,({
struct _tagged_ptr3 _temp5808= yyls; struct Cyc_Yyltype* _temp5810= _temp5808.curr
+ yylsp_offset; if( _temp5810 < _temp5808.base? 1: _temp5810 >= _temp5808.last_plus_one){
_throw( Null_Exception);}* _temp5810;}).last_line))};( struct _xenum_struct*)
_temp5801;}); break; case 331: _LL5800: yyval=({ struct Cyc_Exp_tok_struct*
_temp5812=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5812=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_throw_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5813= yyvs; struct _xenum_struct**
_temp5815= _temp5813.curr + yyvsp_offset; if( _temp5815 < _temp5813.base? 1:
_temp5815 >= _temp5813.last_plus_one){ _throw( Null_Exception);}* _temp5815;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5816= yyls; struct Cyc_Yyltype*
_temp5818= _temp5816.curr +( yylsp_offset - 1); if( _temp5818 < _temp5816.base?
1: _temp5818 >= _temp5816.last_plus_one){ _throw( Null_Exception);}* _temp5818;}).first_line,({
struct _tagged_ptr3 _temp5819= yyls; struct Cyc_Yyltype* _temp5821= _temp5819.curr
+ yylsp_offset; if( _temp5821 < _temp5819.base? 1: _temp5821 >= _temp5819.last_plus_one){
_throw( Null_Exception);}* _temp5821;}).last_line))};( struct _xenum_struct*)
_temp5812;}); break; case 332: _LL5811: yyval=({ struct Cyc_Exp_tok_struct*
_temp5823=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5823=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_primop_exp(
Cyc_yyget_Primop_tok(({ struct _tagged_ptr2 _temp5824= yyvs; struct
_xenum_struct** _temp5826= _temp5824.curr +( yyvsp_offset - 3); if( _temp5826 <
_temp5824.base? 1: _temp5826 >= _temp5824.last_plus_one){ _throw( Null_Exception);}*
_temp5826;})), Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2 _temp5827= yyvs;
struct _xenum_struct** _temp5829= _temp5827.curr +( yyvsp_offset - 1); if(
_temp5829 < _temp5827.base? 1: _temp5829 >= _temp5827.last_plus_one){ _throw(
Null_Exception);}* _temp5829;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5830= yyls; struct Cyc_Yyltype* _temp5832= _temp5830.curr +(
yylsp_offset - 3); if( _temp5832 < _temp5830.base? 1: _temp5832 >= _temp5830.last_plus_one){
_throw( Null_Exception);}* _temp5832;}).first_line,({ struct _tagged_ptr3
_temp5833= yyls; struct Cyc_Yyltype* _temp5835= _temp5833.curr + yylsp_offset;
if( _temp5835 < _temp5833.base? 1: _temp5835 >= _temp5833.last_plus_one){ _throw(
Null_Exception);}* _temp5835;}).last_line))};( struct _xenum_struct*) _temp5823;});
break; case 333: _LL5822: yyval=({ struct Cyc_Primop_tok_struct* _temp5837=(
struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));*
_temp5837=( struct Cyc_Primop_tok_struct){.tag= Cyc_Primop_tok_tag,.f1= Cyc_Absyn_Printf};(
struct _xenum_struct*) _temp5837;}); break; case 334: _LL5836: yyval=({ struct
Cyc_Primop_tok_struct* _temp5839=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct));* _temp5839=( struct Cyc_Primop_tok_struct){.tag=
Cyc_Primop_tok_tag,.f1= Cyc_Absyn_Fprintf};( struct _xenum_struct*) _temp5839;});
break; case 335: _LL5838: yyval=({ struct Cyc_Primop_tok_struct* _temp5841=(
struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));*
_temp5841=( struct Cyc_Primop_tok_struct){.tag= Cyc_Primop_tok_tag,.f1= Cyc_Absyn_Xprintf};(
struct _xenum_struct*) _temp5841;}); break; case 336: _LL5840: yyval=({ struct
Cyc_Primop_tok_struct* _temp5843=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct));* _temp5843=( struct Cyc_Primop_tok_struct){.tag=
Cyc_Primop_tok_tag,.f1= Cyc_Absyn_Scanf};( struct _xenum_struct*) _temp5843;});
break; case 337: _LL5842: yyval=({ struct Cyc_Primop_tok_struct* _temp5845=(
struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));*
_temp5845=( struct Cyc_Primop_tok_struct){.tag= Cyc_Primop_tok_tag,.f1= Cyc_Absyn_Fscanf};(
struct _xenum_struct*) _temp5845;}); break; case 338: _LL5844: yyval=({ struct
Cyc_Primop_tok_struct* _temp5847=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct));* _temp5847=( struct Cyc_Primop_tok_struct){.tag=
Cyc_Primop_tok_tag,.f1= Cyc_Absyn_Sscanf};( struct _xenum_struct*) _temp5847;});
break; case 339: _LL5846: yyval=({ struct Cyc_Primop_tok_struct* _temp5849=(
struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));*
_temp5849=( struct Cyc_Primop_tok_struct){.tag= Cyc_Primop_tok_tag,.f1= Cyc_Absyn_Bitnot};(
struct _xenum_struct*) _temp5849;}); break; case 340: _LL5848: yyval=({ struct
Cyc_Primop_tok_struct* _temp5851=( struct Cyc_Primop_tok_struct*) GC_malloc(
sizeof( struct Cyc_Primop_tok_struct));* _temp5851=( struct Cyc_Primop_tok_struct){.tag=
Cyc_Primop_tok_tag,.f1= Cyc_Absyn_Not};( struct _xenum_struct*) _temp5851;});
break; case 341: _LL5850: yyval=({ struct Cyc_Primop_tok_struct* _temp5853=(
struct Cyc_Primop_tok_struct*) GC_malloc( sizeof( struct Cyc_Primop_tok_struct));*
_temp5853=( struct Cyc_Primop_tok_struct){.tag= Cyc_Primop_tok_tag,.f1= Cyc_Absyn_Minus};(
struct _xenum_struct*) _temp5853;}); break; case 342: _LL5852: yyval=({ struct
_tagged_ptr2 _temp5855= yyvs; struct _xenum_struct** _temp5857= _temp5855.curr +
yyvsp_offset; if( _temp5857 < _temp5855.base? 1: _temp5857 >= _temp5855.last_plus_one){
_throw( Null_Exception);}* _temp5857;}); break; case 343: _LL5854: yyval=({
struct Cyc_Exp_tok_struct* _temp5859=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct));* _temp5859=( struct Cyc_Exp_tok_struct){.tag=
Cyc_Exp_tok_tag,.f1= Cyc_Absyn_subscript_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5860= yyvs; struct _xenum_struct** _temp5862= _temp5860.curr +(
yyvsp_offset - 3); if( _temp5862 < _temp5860.base? 1: _temp5862 >= _temp5860.last_plus_one){
_throw( Null_Exception);}* _temp5862;})), Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5863= yyvs; struct _xenum_struct** _temp5865= _temp5863.curr +(
yyvsp_offset - 1); if( _temp5865 < _temp5863.base? 1: _temp5865 >= _temp5863.last_plus_one){
_throw( Null_Exception);}* _temp5865;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5866= yyls; struct Cyc_Yyltype* _temp5868= _temp5866.curr +(
yylsp_offset - 3); if( _temp5868 < _temp5866.base? 1: _temp5868 >= _temp5866.last_plus_one){
_throw( Null_Exception);}* _temp5868;}).first_line,({ struct _tagged_ptr3
_temp5869= yyls; struct Cyc_Yyltype* _temp5871= _temp5869.curr + yylsp_offset;
if( _temp5871 < _temp5869.base? 1: _temp5871 >= _temp5869.last_plus_one){ _throw(
Null_Exception);}* _temp5871;}).last_line))};( struct _xenum_struct*) _temp5859;});
break; case 344: _LL5858: yyval=({ struct Cyc_Exp_tok_struct* _temp5873=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp5873=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_unknowncall_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5874= yyvs; struct _xenum_struct**
_temp5876= _temp5874.curr +( yyvsp_offset - 2); if( _temp5876 < _temp5874.base?
1: _temp5876 >= _temp5874.last_plus_one){ _throw( Null_Exception);}* _temp5876;})),
0, Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp5877= yyls; struct
Cyc_Yyltype* _temp5879= _temp5877.curr +( yylsp_offset - 2); if( _temp5879 <
_temp5877.base? 1: _temp5879 >= _temp5877.last_plus_one){ _throw( Null_Exception);}*
_temp5879;}).first_line,({ struct _tagged_ptr3 _temp5880= yyls; struct Cyc_Yyltype*
_temp5882= _temp5880.curr + yylsp_offset; if( _temp5882 < _temp5880.base? 1:
_temp5882 >= _temp5880.last_plus_one){ _throw( Null_Exception);}* _temp5882;}).last_line))};(
struct _xenum_struct*) _temp5873;}); break; case 345: _LL5872: yyval=({ struct
Cyc_Exp_tok_struct* _temp5884=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof(
struct Cyc_Exp_tok_struct));* _temp5884=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1=
Cyc_Absyn_unknowncall_exp( Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5885=
yyvs; struct _xenum_struct** _temp5887= _temp5885.curr +( yyvsp_offset - 3); if(
_temp5887 < _temp5885.base? 1: _temp5887 >= _temp5885.last_plus_one){ _throw(
Null_Exception);}* _temp5887;})), Cyc_yyget_ExpList_tok(({ struct _tagged_ptr2
_temp5888= yyvs; struct _xenum_struct** _temp5890= _temp5888.curr +(
yyvsp_offset - 1); if( _temp5890 < _temp5888.base? 1: _temp5890 >= _temp5888.last_plus_one){
_throw( Null_Exception);}* _temp5890;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp5891= yyls; struct Cyc_Yyltype* _temp5893= _temp5891.curr +(
yylsp_offset - 3); if( _temp5893 < _temp5891.base? 1: _temp5893 >= _temp5891.last_plus_one){
_throw( Null_Exception);}* _temp5893;}).first_line,({ struct _tagged_ptr3
_temp5894= yyls; struct Cyc_Yyltype* _temp5896= _temp5894.curr + yylsp_offset;
if( _temp5896 < _temp5894.base? 1: _temp5896 >= _temp5894.last_plus_one){ _throw(
Null_Exception);}* _temp5896;}).last_line))};( struct _xenum_struct*) _temp5884;});
break; case 346: _LL5883: yyval=({ struct Cyc_Exp_tok_struct* _temp5898=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp5898=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_structmember_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5899= yyvs; struct _xenum_struct**
_temp5901= _temp5899.curr +( yyvsp_offset - 2); if( _temp5901 < _temp5899.base?
1: _temp5901 >= _temp5899.last_plus_one){ _throw( Null_Exception);}* _temp5901;})),({
struct _tagged_string* _temp5902=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp5902[ 0]= Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp5903= yyvs; struct _xenum_struct** _temp5905= _temp5903.curr +
yyvsp_offset; if( _temp5905 < _temp5903.base? 1: _temp5905 >= _temp5903.last_plus_one){
_throw( Null_Exception);}* _temp5905;})); _temp5902;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5906= yyls; struct Cyc_Yyltype* _temp5908= _temp5906.curr
+( yylsp_offset - 2); if( _temp5908 < _temp5906.base? 1: _temp5908 >= _temp5906.last_plus_one){
_throw( Null_Exception);}* _temp5908;}).first_line,({ struct _tagged_ptr3
_temp5909= yyls; struct Cyc_Yyltype* _temp5911= _temp5909.curr + yylsp_offset;
if( _temp5911 < _temp5909.base? 1: _temp5911 >= _temp5909.last_plus_one){ _throw(
Null_Exception);}* _temp5911;}).last_line))};( struct _xenum_struct*) _temp5898;});
break; case 347: _LL5897: { struct _tuple1* q= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp5913= yyvs; struct _xenum_struct** _temp5915= _temp5913.curr +
yyvsp_offset; if( _temp5915 < _temp5913.base? 1: _temp5915 >= _temp5913.last_plus_one){
_throw( Null_Exception);}* _temp5915;}));{ void* _temp5916=(* q).f1; struct Cyc_List_List*
_temp5926; struct Cyc_List_List* _temp5928; _LL5918: if( _temp5916 == Cyc_Absyn_Loc_n){
goto _LL5919;} else{ goto _LL5920;} _LL5920: if(( unsigned int) _temp5916 > 1u?((
struct _enum_struct*) _temp5916)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL5927:
_temp5926=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp5916)->f1;
if( _temp5926 == 0){ goto _LL5921;} else{ goto _LL5922;}} else{ goto _LL5922;}
_LL5922: if(( unsigned int) _temp5916 > 1u?(( struct _enum_struct*) _temp5916)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL5929: _temp5928=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp5916)->f1; if( _temp5928 == 0){ goto _LL5923;}
else{ goto _LL5924;}} else{ goto _LL5924;} _LL5924: goto _LL5925; _LL5919: goto
_LL5917; _LL5921: goto _LL5917; _LL5923: goto _LL5917; _LL5925: Cyc_Parse_err((
struct _tagged_string)({ char* _temp5930=( char*)"struct field name is qualified";
struct _tagged_string _temp5931; _temp5931.curr= _temp5930; _temp5931.base=
_temp5930; _temp5931.last_plus_one= _temp5930 + 31; _temp5931;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5932= yyls; struct Cyc_Yyltype* _temp5934= _temp5932.curr
+ yylsp_offset; if( _temp5934 < _temp5932.base? 1: _temp5934 >= _temp5932.last_plus_one){
_throw( Null_Exception);}* _temp5934;}).first_line,({ struct _tagged_ptr3
_temp5935= yyls; struct Cyc_Yyltype* _temp5937= _temp5935.curr + yylsp_offset;
if( _temp5937 < _temp5935.base? 1: _temp5937 >= _temp5935.last_plus_one){ _throw(
Null_Exception);}* _temp5937;}).last_line)); goto _LL5917; _LL5917:;} yyval=({
struct Cyc_Exp_tok_struct* _temp5938=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct));* _temp5938=( struct Cyc_Exp_tok_struct){.tag=
Cyc_Exp_tok_tag,.f1= Cyc_Absyn_structmember_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5939= yyvs; struct _xenum_struct** _temp5941= _temp5939.curr +(
yyvsp_offset - 2); if( _temp5941 < _temp5939.base? 1: _temp5941 >= _temp5939.last_plus_one){
_throw( Null_Exception);}* _temp5941;})),(* q).f2, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5942= yyls; struct Cyc_Yyltype* _temp5944= _temp5942.curr
+( yylsp_offset - 2); if( _temp5944 < _temp5942.base? 1: _temp5944 >= _temp5942.last_plus_one){
_throw( Null_Exception);}* _temp5944;}).first_line,({ struct _tagged_ptr3
_temp5945= yyls; struct Cyc_Yyltype* _temp5947= _temp5945.curr + yylsp_offset;
if( _temp5947 < _temp5945.base? 1: _temp5947 >= _temp5945.last_plus_one){ _throw(
Null_Exception);}* _temp5947;}).last_line))};( struct _xenum_struct*) _temp5938;});
break;} case 348: _LL5912: yyval=({ struct Cyc_Exp_tok_struct* _temp5949=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp5949=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_structarrow_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp5950= yyvs; struct _xenum_struct**
_temp5952= _temp5950.curr +( yyvsp_offset - 2); if( _temp5952 < _temp5950.base?
1: _temp5952 >= _temp5950.last_plus_one){ _throw( Null_Exception);}* _temp5952;})),({
struct _tagged_string* _temp5953=( struct _tagged_string*) GC_malloc( sizeof(
struct _tagged_string) * 1); _temp5953[ 0]= Cyc_yyget_String_tok(({ struct
_tagged_ptr2 _temp5954= yyvs; struct _xenum_struct** _temp5956= _temp5954.curr +
yyvsp_offset; if( _temp5956 < _temp5954.base? 1: _temp5956 >= _temp5954.last_plus_one){
_throw( Null_Exception);}* _temp5956;})); _temp5953;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5957= yyls; struct Cyc_Yyltype* _temp5959= _temp5957.curr
+( yylsp_offset - 2); if( _temp5959 < _temp5957.base? 1: _temp5959 >= _temp5957.last_plus_one){
_throw( Null_Exception);}* _temp5959;}).first_line,({ struct _tagged_ptr3
_temp5960= yyls; struct Cyc_Yyltype* _temp5962= _temp5960.curr + yylsp_offset;
if( _temp5962 < _temp5960.base? 1: _temp5962 >= _temp5960.last_plus_one){ _throw(
Null_Exception);}* _temp5962;}).last_line))};( struct _xenum_struct*) _temp5949;});
break; case 349: _LL5948: { struct _tuple1* q= Cyc_yyget_QualId_tok(({ struct
_tagged_ptr2 _temp5964= yyvs; struct _xenum_struct** _temp5966= _temp5964.curr +
yyvsp_offset; if( _temp5966 < _temp5964.base? 1: _temp5966 >= _temp5964.last_plus_one){
_throw( Null_Exception);}* _temp5966;}));{ void* _temp5967=(* q).f1; struct Cyc_List_List*
_temp5977; struct Cyc_List_List* _temp5979; _LL5969: if( _temp5967 == Cyc_Absyn_Loc_n){
goto _LL5970;} else{ goto _LL5971;} _LL5971: if(( unsigned int) _temp5967 > 1u?((
struct _enum_struct*) _temp5967)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL5978:
_temp5977=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp5967)->f1;
if( _temp5977 == 0){ goto _LL5972;} else{ goto _LL5973;}} else{ goto _LL5973;}
_LL5973: if(( unsigned int) _temp5967 > 1u?(( struct _enum_struct*) _temp5967)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL5980: _temp5979=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp5967)->f1; if( _temp5979 == 0){ goto _LL5974;}
else{ goto _LL5975;}} else{ goto _LL5975;} _LL5975: goto _LL5976; _LL5970: goto
_LL5968; _LL5972: goto _LL5968; _LL5974: goto _LL5968; _LL5976: Cyc_Parse_err((
struct _tagged_string)({ char* _temp5981=( char*)"struct field is qualified with module name";
struct _tagged_string _temp5982; _temp5982.curr= _temp5981; _temp5982.base=
_temp5981; _temp5982.last_plus_one= _temp5981 + 43; _temp5982;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5983= yyls; struct Cyc_Yyltype* _temp5985= _temp5983.curr
+ yylsp_offset; if( _temp5985 < _temp5983.base? 1: _temp5985 >= _temp5983.last_plus_one){
_throw( Null_Exception);}* _temp5985;}).first_line,({ struct _tagged_ptr3
_temp5986= yyls; struct Cyc_Yyltype* _temp5988= _temp5986.curr + yylsp_offset;
if( _temp5988 < _temp5986.base? 1: _temp5988 >= _temp5986.last_plus_one){ _throw(
Null_Exception);}* _temp5988;}).last_line)); goto _LL5968; _LL5968:;} yyval=({
struct Cyc_Exp_tok_struct* _temp5989=( struct Cyc_Exp_tok_struct*) GC_malloc(
sizeof( struct Cyc_Exp_tok_struct));* _temp5989=( struct Cyc_Exp_tok_struct){.tag=
Cyc_Exp_tok_tag,.f1= Cyc_Absyn_structarrow_exp( Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp5990= yyvs; struct _xenum_struct** _temp5992= _temp5990.curr +(
yyvsp_offset - 2); if( _temp5992 < _temp5990.base? 1: _temp5992 >= _temp5990.last_plus_one){
_throw( Null_Exception);}* _temp5992;})),(* q).f2, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp5993= yyls; struct Cyc_Yyltype* _temp5995= _temp5993.curr
+( yylsp_offset - 2); if( _temp5995 < _temp5993.base? 1: _temp5995 >= _temp5993.last_plus_one){
_throw( Null_Exception);}* _temp5995;}).first_line,({ struct _tagged_ptr3
_temp5996= yyls; struct Cyc_Yyltype* _temp5998= _temp5996.curr + yylsp_offset;
if( _temp5998 < _temp5996.base? 1: _temp5998 >= _temp5996.last_plus_one){ _throw(
Null_Exception);}* _temp5998;}).last_line))};( struct _xenum_struct*) _temp5989;});
break;} case 350: _LL5963: yyval=({ struct Cyc_Exp_tok_struct* _temp6000=(
struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp6000=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_post_inc_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6001= yyvs; struct _xenum_struct**
_temp6003= _temp6001.curr +( yyvsp_offset - 1); if( _temp6003 < _temp6001.base?
1: _temp6003 >= _temp6001.last_plus_one){ _throw( Null_Exception);}* _temp6003;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6004= yyls; struct Cyc_Yyltype*
_temp6006= _temp6004.curr +( yylsp_offset - 1); if( _temp6006 < _temp6004.base?
1: _temp6006 >= _temp6004.last_plus_one){ _throw( Null_Exception);}* _temp6006;}).first_line,({
struct _tagged_ptr3 _temp6007= yyls; struct Cyc_Yyltype* _temp6009= _temp6007.curr
+ yylsp_offset; if( _temp6009 < _temp6007.base? 1: _temp6009 >= _temp6007.last_plus_one){
_throw( Null_Exception);}* _temp6009;}).last_line))};( struct _xenum_struct*)
_temp6000;}); break; case 351: _LL5999: yyval=({ struct Cyc_Exp_tok_struct*
_temp6011=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp6011=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_post_dec_exp(
Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6012= yyvs; struct _xenum_struct**
_temp6014= _temp6012.curr +( yyvsp_offset - 1); if( _temp6014 < _temp6012.base?
1: _temp6014 >= _temp6012.last_plus_one){ _throw( Null_Exception);}* _temp6014;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6015= yyls; struct Cyc_Yyltype*
_temp6017= _temp6015.curr +( yylsp_offset - 1); if( _temp6017 < _temp6015.base?
1: _temp6017 >= _temp6015.last_plus_one){ _throw( Null_Exception);}* _temp6017;}).first_line,({
struct _tagged_ptr3 _temp6018= yyls; struct Cyc_Yyltype* _temp6020= _temp6018.curr
+ yylsp_offset; if( _temp6020 < _temp6018.base? 1: _temp6020 >= _temp6018.last_plus_one){
_throw( Null_Exception);}* _temp6020;}).last_line))};( struct _xenum_struct*)
_temp6011;}); break; case 352: _LL6010: yyval=({ struct Cyc_Exp_tok_struct*
_temp6022=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp6022=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_CompoundLit_e_struct* _temp6023=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp6023->tag= Cyc_Absyn_CompoundLit_e_tag;
_temp6023->f1= Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp6024= yyvs;
struct _xenum_struct** _temp6026= _temp6024.curr +( yyvsp_offset - 4); if(
_temp6026 < _temp6024.base? 1: _temp6026 >= _temp6024.last_plus_one){ _throw(
Null_Exception);}* _temp6026;})); _temp6023->f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp6027= yyvs; struct _xenum_struct** _temp6029= _temp6027.curr
+( yyvsp_offset - 1); if( _temp6029 < _temp6027.base? 1: _temp6029 >= _temp6027.last_plus_one){
_throw( Null_Exception);}* _temp6029;})));( void*) _temp6023;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6030= yyls; struct Cyc_Yyltype* _temp6032= _temp6030.curr
+( yylsp_offset - 5); if( _temp6032 < _temp6030.base? 1: _temp6032 >= _temp6030.last_plus_one){
_throw( Null_Exception);}* _temp6032;}).first_line,({ struct _tagged_ptr3
_temp6033= yyls; struct Cyc_Yyltype* _temp6035= _temp6033.curr + yylsp_offset;
if( _temp6035 < _temp6033.base? 1: _temp6035 >= _temp6033.last_plus_one){ _throw(
Null_Exception);}* _temp6035;}).last_line))};( struct _xenum_struct*) _temp6022;});
break; case 353: _LL6021: yyval=({ struct Cyc_Exp_tok_struct* _temp6037=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp6037=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_CompoundLit_e_struct* _temp6038=( struct Cyc_Absyn_CompoundLit_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_CompoundLit_e_struct)); _temp6038->tag= Cyc_Absyn_CompoundLit_e_tag;
_temp6038->f1= Cyc_yyget_ParamDecl_tok(({ struct _tagged_ptr2 _temp6039= yyvs;
struct _xenum_struct** _temp6041= _temp6039.curr +( yyvsp_offset - 5); if(
_temp6041 < _temp6039.base? 1: _temp6041 >= _temp6039.last_plus_one){ _throw(
Null_Exception);}* _temp6041;})); _temp6038->f2=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp6042= yyvs; struct _xenum_struct** _temp6044= _temp6042.curr
+( yyvsp_offset - 2); if( _temp6044 < _temp6042.base? 1: _temp6044 >= _temp6042.last_plus_one){
_throw( Null_Exception);}* _temp6044;})));( void*) _temp6038;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6045= yyls; struct Cyc_Yyltype* _temp6047= _temp6045.curr
+( yylsp_offset - 6); if( _temp6047 < _temp6045.base? 1: _temp6047 >= _temp6045.last_plus_one){
_throw( Null_Exception);}* _temp6047;}).first_line,({ struct _tagged_ptr3
_temp6048= yyls; struct Cyc_Yyltype* _temp6050= _temp6048.curr + yylsp_offset;
if( _temp6050 < _temp6048.base? 1: _temp6050 >= _temp6048.last_plus_one){ _throw(
Null_Exception);}* _temp6050;}).last_line))};( struct _xenum_struct*) _temp6037;});
break; case 354: _LL6036: yyval=({ struct Cyc_Exp_tok_struct* _temp6052=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp6052=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_Array_e_struct* _temp6053=( struct Cyc_Absyn_Array_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp6053->tag= Cyc_Absyn_Array_e_tag;
_temp6053->f1= 1; _temp6053->f2= 0;( void*) _temp6053;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6054= yyls; struct Cyc_Yyltype* _temp6056= _temp6054.curr
+( yylsp_offset - 2); if( _temp6056 < _temp6054.base? 1: _temp6056 >= _temp6054.last_plus_one){
_throw( Null_Exception);}* _temp6056;}).first_line,({ struct _tagged_ptr3
_temp6057= yyls; struct Cyc_Yyltype* _temp6059= _temp6057.curr + yylsp_offset;
if( _temp6059 < _temp6057.base? 1: _temp6059 >= _temp6057.last_plus_one){ _throw(
Null_Exception);}* _temp6059;}).last_line))};( struct _xenum_struct*) _temp6052;});
break; case 355: _LL6051: yyval=({ struct Cyc_Exp_tok_struct* _temp6061=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp6061=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_Array_e_struct* _temp6062=( struct Cyc_Absyn_Array_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp6062->tag= Cyc_Absyn_Array_e_tag;
_temp6062->f1= 1; _temp6062->f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({ struct _tagged_ptr2
_temp6063= yyvs; struct _xenum_struct** _temp6065= _temp6063.curr +(
yyvsp_offset - 1); if( _temp6065 < _temp6063.base? 1: _temp6065 >= _temp6063.last_plus_one){
_throw( Null_Exception);}* _temp6065;})));( void*) _temp6062;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6066= yyls; struct Cyc_Yyltype* _temp6068= _temp6066.curr
+( yylsp_offset - 3); if( _temp6068 < _temp6066.base? 1: _temp6068 >= _temp6066.last_plus_one){
_throw( Null_Exception);}* _temp6068;}).first_line,({ struct _tagged_ptr3
_temp6069= yyls; struct Cyc_Yyltype* _temp6071= _temp6069.curr +( yylsp_offset -
1); if( _temp6071 < _temp6069.base? 1: _temp6071 >= _temp6069.last_plus_one){
_throw( Null_Exception);}* _temp6071;}).last_line))};( struct _xenum_struct*)
_temp6061;}); break; case 356: _LL6060: yyval=({ struct Cyc_Exp_tok_struct*
_temp6073=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp6073=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_Array_e_struct* _temp6074=( struct Cyc_Absyn_Array_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct)); _temp6074->tag= Cyc_Absyn_Array_e_tag;
_temp6074->f1= 1; _temp6074->f2=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({ struct _tagged_ptr2
_temp6075= yyvs; struct _xenum_struct** _temp6077= _temp6075.curr +(
yyvsp_offset - 2); if( _temp6077 < _temp6075.base? 1: _temp6077 >= _temp6075.last_plus_one){
_throw( Null_Exception);}* _temp6077;})));( void*) _temp6074;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6078= yyls; struct Cyc_Yyltype* _temp6080= _temp6078.curr
+( yylsp_offset - 4); if( _temp6080 < _temp6078.base? 1: _temp6080 >= _temp6078.last_plus_one){
_throw( Null_Exception);}* _temp6080;}).first_line,({ struct _tagged_ptr3
_temp6081= yyls; struct Cyc_Yyltype* _temp6083= _temp6081.curr +( yylsp_offset -
1); if( _temp6083 < _temp6081.base? 1: _temp6083 >= _temp6081.last_plus_one){
_throw( Null_Exception);}* _temp6083;}).last_line))};( struct _xenum_struct*)
_temp6073;}); break; case 357: _LL6072: yyval=({ struct Cyc_Exp_tok_struct*
_temp6085=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp6085=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_Comprehension_e_struct* _temp6086=( struct Cyc_Absyn_Comprehension_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Comprehension_e_struct)); _temp6086->tag=
Cyc_Absyn_Comprehension_e_tag; _temp6086->f1= Cyc_Absyn_new_vardecl(({ struct
_tuple1* _temp6087=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp6087->f1= Cyc_Absyn_Loc_n; _temp6087->f2=({ struct _tagged_string*
_temp6088=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp6088[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp6089= yyvs;
struct _xenum_struct** _temp6091= _temp6089.curr +( yyvsp_offset - 5); if(
_temp6091 < _temp6089.base? 1: _temp6091 >= _temp6089.last_plus_one){ _throw(
Null_Exception);}* _temp6091;})); _temp6088;}); _temp6087;}), Cyc_Absyn_uint_t,(
struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp( 0, Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6092= yyls; struct Cyc_Yyltype* _temp6094= _temp6092.curr
+( yylsp_offset - 5); if( _temp6094 < _temp6092.base? 1: _temp6094 >= _temp6092.last_plus_one){
_throw( Null_Exception);}* _temp6094;}).first_line,({ struct _tagged_ptr3
_temp6095= yyls; struct Cyc_Yyltype* _temp6097= _temp6095.curr +( yylsp_offset -
5); if( _temp6097 < _temp6095.base? 1: _temp6097 >= _temp6095.last_plus_one){
_throw( Null_Exception);}* _temp6097;}).last_line))); _temp6086->f2= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6098= yyvs; struct _xenum_struct** _temp6100= _temp6098.curr
+( yyvsp_offset - 3); if( _temp6100 < _temp6098.base? 1: _temp6100 >= _temp6098.last_plus_one){
_throw( Null_Exception);}* _temp6100;})); _temp6086->f3= Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6101= yyvs; struct _xenum_struct** _temp6103= _temp6101.curr
+( yyvsp_offset - 1); if( _temp6103 < _temp6101.base? 1: _temp6103 >= _temp6101.last_plus_one){
_throw( Null_Exception);}* _temp6103;}));( void*) _temp6086;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6104= yyls; struct Cyc_Yyltype* _temp6106= _temp6104.curr
+( yylsp_offset - 8); if( _temp6106 < _temp6104.base? 1: _temp6106 >= _temp6104.last_plus_one){
_throw( Null_Exception);}* _temp6106;}).first_line,({ struct _tagged_ptr3
_temp6107= yyls; struct Cyc_Yyltype* _temp6109= _temp6107.curr + yylsp_offset;
if( _temp6109 < _temp6107.base? 1: _temp6109 >= _temp6107.last_plus_one){ _throw(
Null_Exception);}* _temp6109;}).last_line))};( struct _xenum_struct*) _temp6085;});
break; case 358: _LL6084: yyval=({ struct Cyc_Exp_tok_struct* _temp6111=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp6111=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_string_exp( 1,
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp6112= yyvs; struct
_xenum_struct** _temp6114= _temp6112.curr + yyvsp_offset; if( _temp6114 <
_temp6112.base? 1: _temp6114 >= _temp6112.last_plus_one){ _throw( Null_Exception);}*
_temp6114;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6115=
yyls; struct Cyc_Yyltype* _temp6117= _temp6115.curr +( yylsp_offset - 1); if(
_temp6117 < _temp6115.base? 1: _temp6117 >= _temp6115.last_plus_one){ _throw(
Null_Exception);}* _temp6117;}).first_line,({ struct _tagged_ptr3 _temp6118=
yyls; struct Cyc_Yyltype* _temp6120= _temp6118.curr + yylsp_offset; if(
_temp6120 < _temp6118.base? 1: _temp6120 >= _temp6118.last_plus_one){ _throw(
Null_Exception);}* _temp6120;}).last_line))};( struct _xenum_struct*) _temp6111;});
break; case 359: _LL6110: yyval=({ struct Cyc_Exp_tok_struct* _temp6122=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp6122=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_Fill_e_struct* _temp6123=( struct Cyc_Absyn_Fill_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fill_e_struct)); _temp6123->tag= Cyc_Absyn_Fill_e_tag;
_temp6123->f1= Cyc_yyget_Exp_tok(({ struct _tagged_ptr2 _temp6124= yyvs; struct
_xenum_struct** _temp6126= _temp6124.curr +( yyvsp_offset - 1); if( _temp6126 <
_temp6124.base? 1: _temp6126 >= _temp6124.last_plus_one){ _throw( Null_Exception);}*
_temp6126;}));( void*) _temp6123;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6127= yyls; struct Cyc_Yyltype* _temp6129= _temp6127.curr +(
yylsp_offset - 3); if( _temp6129 < _temp6127.base? 1: _temp6129 >= _temp6127.last_plus_one){
_throw( Null_Exception);}* _temp6129;}).first_line,({ struct _tagged_ptr3
_temp6130= yyls; struct Cyc_Yyltype* _temp6132= _temp6130.curr + yylsp_offset;
if( _temp6132 < _temp6130.base? 1: _temp6132 >= _temp6130.last_plus_one){ _throw(
Null_Exception);}* _temp6132;}).last_line))};( struct _xenum_struct*) _temp6122;});
break; case 360: _LL6121: yyval=({ struct Cyc_Exp_tok_struct* _temp6134=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp6134=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_Codegen_e_struct* _temp6135=( struct Cyc_Absyn_Codegen_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Codegen_e_struct)); _temp6135->tag= Cyc_Absyn_Codegen_e_tag;
_temp6135->f1= Cyc_yyget_FnDecl_tok(({ struct _tagged_ptr2 _temp6136= yyvs;
struct _xenum_struct** _temp6138= _temp6136.curr +( yyvsp_offset - 1); if(
_temp6138 < _temp6136.base? 1: _temp6138 >= _temp6136.last_plus_one){ _throw(
Null_Exception);}* _temp6138;}));( void*) _temp6135;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6139= yyls; struct Cyc_Yyltype* _temp6141= _temp6139.curr
+( yylsp_offset - 3); if( _temp6141 < _temp6139.base? 1: _temp6141 >= _temp6139.last_plus_one){
_throw( Null_Exception);}* _temp6141;}).first_line,({ struct _tagged_ptr3
_temp6142= yyls; struct Cyc_Yyltype* _temp6144= _temp6142.curr + yylsp_offset;
if( _temp6144 < _temp6142.base? 1: _temp6144 >= _temp6142.last_plus_one){ _throw(
Null_Exception);}* _temp6144;}).last_line))};( struct _xenum_struct*) _temp6134;});
break; case 361: _LL6133: yyval=({ struct Cyc_Exp_tok_struct* _temp6146=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp6146=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_UnknownId_e_struct* _temp6147=( struct Cyc_Absyn_UnknownId_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp6147->tag= Cyc_Absyn_UnknownId_e_tag;
_temp6147->f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp6148= yyvs;
struct _xenum_struct** _temp6150= _temp6148.curr + yyvsp_offset; if( _temp6150 <
_temp6148.base? 1: _temp6150 >= _temp6148.last_plus_one){ _throw( Null_Exception);}*
_temp6150;}));( void*) _temp6147;}), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6151= yyls; struct Cyc_Yyltype* _temp6153= _temp6151.curr +
yylsp_offset; if( _temp6153 < _temp6151.base? 1: _temp6153 >= _temp6151.last_plus_one){
_throw( Null_Exception);}* _temp6153;}).first_line,({ struct _tagged_ptr3
_temp6154= yyls; struct Cyc_Yyltype* _temp6156= _temp6154.curr + yylsp_offset;
if( _temp6156 < _temp6154.base? 1: _temp6156 >= _temp6154.last_plus_one){ _throw(
Null_Exception);}* _temp6156;}).last_line))};( struct _xenum_struct*) _temp6146;});
break; case 362: _LL6145: yyval=({ struct _tagged_ptr2 _temp6158= yyvs; struct
_xenum_struct** _temp6160= _temp6158.curr + yyvsp_offset; if( _temp6160 <
_temp6158.base? 1: _temp6160 >= _temp6158.last_plus_one){ _throw( Null_Exception);}*
_temp6160;}); break; case 363: _LL6157: yyval=({ struct Cyc_Exp_tok_struct*
_temp6162=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp6162=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_string_exp(
0, Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp6163= yyvs; struct
_xenum_struct** _temp6165= _temp6163.curr + yyvsp_offset; if( _temp6165 <
_temp6163.base? 1: _temp6165 >= _temp6163.last_plus_one){ _throw( Null_Exception);}*
_temp6165;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6166=
yyls; struct Cyc_Yyltype* _temp6168= _temp6166.curr + yylsp_offset; if(
_temp6168 < _temp6166.base? 1: _temp6168 >= _temp6166.last_plus_one){ _throw(
Null_Exception);}* _temp6168;}).first_line,({ struct _tagged_ptr3 _temp6169=
yyls; struct Cyc_Yyltype* _temp6171= _temp6169.curr + yylsp_offset; if(
_temp6171 < _temp6169.base? 1: _temp6171 >= _temp6169.last_plus_one){ _throw(
Null_Exception);}* _temp6171;}).last_line))};( struct _xenum_struct*) _temp6162;});
break; case 364: _LL6161: yyval=({ struct _tagged_ptr2 _temp6173= yyvs; struct
_xenum_struct** _temp6175= _temp6173.curr +( yyvsp_offset - 1); if( _temp6175 <
_temp6173.base? 1: _temp6175 >= _temp6173.last_plus_one){ _throw( Null_Exception);}*
_temp6175;}); break; case 365: _LL6172: yyval=({ struct Cyc_Exp_tok_struct*
_temp6177=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp6177=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_noinstantiate_exp(
Cyc_Absyn_new_exp(({ struct Cyc_Absyn_UnknownId_e_struct* _temp6178=( struct Cyc_Absyn_UnknownId_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp6178->tag= Cyc_Absyn_UnknownId_e_tag;
_temp6178->f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp6179= yyvs;
struct _xenum_struct** _temp6181= _temp6179.curr +( yyvsp_offset - 1); if(
_temp6181 < _temp6179.base? 1: _temp6181 >= _temp6179.last_plus_one){ _throw(
Null_Exception);}* _temp6181;}));( void*) _temp6178;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6182= yyls; struct Cyc_Yyltype* _temp6184= _temp6182.curr
+( yylsp_offset - 1); if( _temp6184 < _temp6182.base? 1: _temp6184 >= _temp6182.last_plus_one){
_throw( Null_Exception);}* _temp6184;}).first_line,({ struct _tagged_ptr3
_temp6185= yyls; struct Cyc_Yyltype* _temp6187= _temp6185.curr +( yylsp_offset -
1); if( _temp6187 < _temp6185.base? 1: _temp6187 >= _temp6185.last_plus_one){
_throw( Null_Exception);}* _temp6187;}).last_line)), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6188= yyls; struct Cyc_Yyltype* _temp6190= _temp6188.curr
+( yylsp_offset - 1); if( _temp6190 < _temp6188.base? 1: _temp6190 >= _temp6188.last_plus_one){
_throw( Null_Exception);}* _temp6190;}).first_line,({ struct _tagged_ptr3
_temp6191= yyls; struct Cyc_Yyltype* _temp6193= _temp6191.curr + yylsp_offset;
if( _temp6193 < _temp6191.base? 1: _temp6193 >= _temp6191.last_plus_one){ _throw(
Null_Exception);}* _temp6193;}).last_line))};( struct _xenum_struct*) _temp6177;});
break; case 366: _LL6176: yyval=({ struct Cyc_Exp_tok_struct* _temp6195=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp6195=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_instantiate_exp(
Cyc_Absyn_new_exp(({ struct Cyc_Absyn_UnknownId_e_struct* _temp6196=( struct Cyc_Absyn_UnknownId_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct)); _temp6196->tag= Cyc_Absyn_UnknownId_e_tag;
_temp6196->f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp6197= yyvs;
struct _xenum_struct** _temp6199= _temp6197.curr +( yyvsp_offset - 4); if(
_temp6199 < _temp6197.base? 1: _temp6199 >= _temp6197.last_plus_one){ _throw(
Null_Exception);}* _temp6199;}));( void*) _temp6196;}), Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6200= yyls; struct Cyc_Yyltype* _temp6202= _temp6200.curr
+( yylsp_offset - 4); if( _temp6202 < _temp6200.base? 1: _temp6202 >= _temp6200.last_plus_one){
_throw( Null_Exception);}* _temp6202;}).first_line,({ struct _tagged_ptr3
_temp6203= yyls; struct Cyc_Yyltype* _temp6205= _temp6203.curr +( yylsp_offset -
4); if( _temp6205 < _temp6203.base? 1: _temp6205 >= _temp6203.last_plus_one){
_throw( Null_Exception);}* _temp6205;}).last_line)),(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_TypeList_tok(({ struct
_tagged_ptr2 _temp6206= yyvs; struct _xenum_struct** _temp6208= _temp6206.curr +(
yyvsp_offset - 1); if( _temp6208 < _temp6206.base? 1: _temp6208 >= _temp6206.last_plus_one){
_throw( Null_Exception);}* _temp6208;}))), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6209= yyls; struct Cyc_Yyltype* _temp6211= _temp6209.curr +(
yylsp_offset - 4); if( _temp6211 < _temp6209.base? 1: _temp6211 >= _temp6209.last_plus_one){
_throw( Null_Exception);}* _temp6211;}).first_line,({ struct _tagged_ptr3
_temp6212= yyls; struct Cyc_Yyltype* _temp6214= _temp6212.curr + yylsp_offset;
if( _temp6214 < _temp6212.base? 1: _temp6214 >= _temp6212.last_plus_one){ _throw(
Null_Exception);}* _temp6214;}).last_line))};( struct _xenum_struct*) _temp6195;});
break; case 367: _LL6194: yyval=({ struct Cyc_Exp_tok_struct* _temp6216=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp6216=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_tuple_exp( Cyc_yyget_ExpList_tok(({
struct _tagged_ptr2 _temp6217= yyvs; struct _xenum_struct** _temp6219= _temp6217.curr
+( yyvsp_offset - 1); if( _temp6219 < _temp6217.base? 1: _temp6219 >= _temp6217.last_plus_one){
_throw( Null_Exception);}* _temp6219;})), Cyc_Position_segment_of_abs(({ struct
_tagged_ptr3 _temp6220= yyls; struct Cyc_Yyltype* _temp6222= _temp6220.curr +(
yylsp_offset - 3); if( _temp6222 < _temp6220.base? 1: _temp6222 >= _temp6220.last_plus_one){
_throw( Null_Exception);}* _temp6222;}).first_line,({ struct _tagged_ptr3
_temp6223= yyls; struct Cyc_Yyltype* _temp6225= _temp6223.curr + yylsp_offset;
if( _temp6225 < _temp6223.base? 1: _temp6225 >= _temp6223.last_plus_one){ _throw(
Null_Exception);}* _temp6225;}).last_line))};( struct _xenum_struct*) _temp6216;});
break; case 368: _LL6215: yyval=({ struct Cyc_Exp_tok_struct* _temp6227=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp6227=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_new_exp(({
struct Cyc_Absyn_Struct_e_struct* _temp6228=( struct Cyc_Absyn_Struct_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_e_struct)); _temp6228->tag= Cyc_Absyn_Struct_e_tag;
_temp6228->f1= Cyc_yyget_QualId_tok(({ struct _tagged_ptr2 _temp6229= yyvs;
struct _xenum_struct** _temp6231= _temp6229.curr +( yyvsp_offset - 3); if(
_temp6231 < _temp6229.base? 1: _temp6231 >= _temp6229.last_plus_one){ _throw(
Null_Exception);}* _temp6231;})); _temp6228->f2= 0; _temp6228->f3=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_InitializerList_tok(({
struct _tagged_ptr2 _temp6232= yyvs; struct _xenum_struct** _temp6234= _temp6232.curr
+( yyvsp_offset - 1); if( _temp6234 < _temp6232.base? 1: _temp6234 >= _temp6232.last_plus_one){
_throw( Null_Exception);}* _temp6234;}))); _temp6228->f4= 0;( void*) _temp6228;}),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6235= yyls; struct Cyc_Yyltype*
_temp6237= _temp6235.curr +( yylsp_offset - 3); if( _temp6237 < _temp6235.base?
1: _temp6237 >= _temp6235.last_plus_one){ _throw( Null_Exception);}* _temp6237;}).first_line,({
struct _tagged_ptr3 _temp6238= yyls; struct Cyc_Yyltype* _temp6240= _temp6238.curr
+ yylsp_offset; if( _temp6240 < _temp6238.base? 1: _temp6240 >= _temp6238.last_plus_one){
_throw( Null_Exception);}* _temp6240;}).last_line))};( struct _xenum_struct*)
_temp6227;}); break; case 369: _LL6226: yyval=({ struct Cyc_Exp_tok_struct*
_temp6242=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp6242=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_stmt_exp(
Cyc_yyget_Stmt_tok(({ struct _tagged_ptr2 _temp6243= yyvs; struct _xenum_struct**
_temp6245= _temp6243.curr +( yyvsp_offset - 2); if( _temp6245 < _temp6243.base?
1: _temp6245 >= _temp6243.last_plus_one){ _throw( Null_Exception);}* _temp6245;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6246= yyls; struct Cyc_Yyltype*
_temp6248= _temp6246.curr +( yylsp_offset - 4); if( _temp6248 < _temp6246.base?
1: _temp6248 >= _temp6246.last_plus_one){ _throw( Null_Exception);}* _temp6248;}).first_line,({
struct _tagged_ptr3 _temp6249= yyls; struct Cyc_Yyltype* _temp6251= _temp6249.curr
+ yylsp_offset; if( _temp6251 < _temp6249.base? 1: _temp6251 >= _temp6249.last_plus_one){
_throw( Null_Exception);}* _temp6251;}).last_line))};( struct _xenum_struct*)
_temp6242;}); break; case 370: _LL6241: yyval=({ struct Cyc_ExpList_tok_struct*
_temp6253=( struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));*
_temp6253=( struct Cyc_ExpList_tok_struct){.tag= Cyc_ExpList_tok_tag,.f1=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_yyget_ExpList_tok(({
struct _tagged_ptr2 _temp6254= yyvs; struct _xenum_struct** _temp6256= _temp6254.curr
+ yyvsp_offset; if( _temp6256 < _temp6254.base? 1: _temp6256 >= _temp6254.last_plus_one){
_throw( Null_Exception);}* _temp6256;})))};( struct _xenum_struct*) _temp6253;});
break; case 371: _LL6252: yyval=({ struct Cyc_ExpList_tok_struct* _temp6258=(
struct Cyc_ExpList_tok_struct*) GC_malloc( sizeof( struct Cyc_ExpList_tok_struct));*
_temp6258=( struct Cyc_ExpList_tok_struct){.tag= Cyc_ExpList_tok_tag,.f1=({
struct Cyc_List_List* _temp6259=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp6259->hd=( void*) Cyc_yyget_Exp_tok(({ struct
_tagged_ptr2 _temp6260= yyvs; struct _xenum_struct** _temp6262= _temp6260.curr +
yyvsp_offset; if( _temp6262 < _temp6260.base? 1: _temp6262 >= _temp6260.last_plus_one){
_throw( Null_Exception);}* _temp6262;})); _temp6259->tl= 0; _temp6259;})};(
struct _xenum_struct*) _temp6258;}); break; case 372: _LL6257: yyval=({ struct
Cyc_ExpList_tok_struct* _temp6264=( struct Cyc_ExpList_tok_struct*) GC_malloc(
sizeof( struct Cyc_ExpList_tok_struct));* _temp6264=( struct Cyc_ExpList_tok_struct){.tag=
Cyc_ExpList_tok_tag,.f1=({ struct Cyc_List_List* _temp6265=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp6265->hd=( void*) Cyc_yyget_Exp_tok(({
struct _tagged_ptr2 _temp6269= yyvs; struct _xenum_struct** _temp6271= _temp6269.curr
+ yyvsp_offset; if( _temp6271 < _temp6269.base? 1: _temp6271 >= _temp6269.last_plus_one){
_throw( Null_Exception);}* _temp6271;})); _temp6265->tl= Cyc_yyget_ExpList_tok(({
struct _tagged_ptr2 _temp6266= yyvs; struct _xenum_struct** _temp6268= _temp6266.curr
+( yyvsp_offset - 2); if( _temp6268 < _temp6266.base? 1: _temp6268 >= _temp6266.last_plus_one){
_throw( Null_Exception);}* _temp6268;})); _temp6265;})};( struct _xenum_struct*)
_temp6264;}); break; case 373: _LL6263: yyval=({ struct Cyc_Exp_tok_struct*
_temp6273=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp6273=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_int_exp((*
Cyc_yyget_Int_tok(({ struct _tagged_ptr2 _temp6274= yyvs; struct _xenum_struct**
_temp6276= _temp6274.curr + yyvsp_offset; if( _temp6276 < _temp6274.base? 1:
_temp6276 >= _temp6274.last_plus_one){ _throw( Null_Exception);}* _temp6276;}))).f1,(*
Cyc_yyget_Int_tok(({ struct _tagged_ptr2 _temp6277= yyvs; struct _xenum_struct**
_temp6279= _temp6277.curr + yyvsp_offset; if( _temp6279 < _temp6277.base? 1:
_temp6279 >= _temp6277.last_plus_one){ _throw( Null_Exception);}* _temp6279;}))).f2,
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6280= yyls; struct Cyc_Yyltype*
_temp6282= _temp6280.curr + yylsp_offset; if( _temp6282 < _temp6280.base? 1:
_temp6282 >= _temp6280.last_plus_one){ _throw( Null_Exception);}* _temp6282;}).first_line,({
struct _tagged_ptr3 _temp6283= yyls; struct Cyc_Yyltype* _temp6285= _temp6283.curr
+ yylsp_offset; if( _temp6285 < _temp6283.base? 1: _temp6285 >= _temp6283.last_plus_one){
_throw( Null_Exception);}* _temp6285;}).last_line))};( struct _xenum_struct*)
_temp6273;}); break; case 374: _LL6272: yyval=({ struct Cyc_Exp_tok_struct*
_temp6287=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp6287=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_char_exp(
Cyc_yyget_Char_tok(({ struct _tagged_ptr2 _temp6288= yyvs; struct _xenum_struct**
_temp6290= _temp6288.curr + yyvsp_offset; if( _temp6290 < _temp6288.base? 1:
_temp6290 >= _temp6288.last_plus_one){ _throw( Null_Exception);}* _temp6290;})),
Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6291= yyls; struct Cyc_Yyltype*
_temp6293= _temp6291.curr + yylsp_offset; if( _temp6293 < _temp6291.base? 1:
_temp6293 >= _temp6291.last_plus_one){ _throw( Null_Exception);}* _temp6293;}).first_line,({
struct _tagged_ptr3 _temp6294= yyls; struct Cyc_Yyltype* _temp6296= _temp6294.curr
+ yylsp_offset; if( _temp6296 < _temp6294.base? 1: _temp6296 >= _temp6294.last_plus_one){
_throw( Null_Exception);}* _temp6296;}).last_line))};( struct _xenum_struct*)
_temp6287;}); break; case 375: _LL6286: yyval=({ struct Cyc_Exp_tok_struct*
_temp6298=( struct Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));*
_temp6298=( struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_float_exp(
Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp6299= yyvs; struct
_xenum_struct** _temp6301= _temp6299.curr + yyvsp_offset; if( _temp6301 <
_temp6299.base? 1: _temp6301 >= _temp6299.last_plus_one){ _throw( Null_Exception);}*
_temp6301;})), Cyc_Position_segment_of_abs(({ struct _tagged_ptr3 _temp6302=
yyls; struct Cyc_Yyltype* _temp6304= _temp6302.curr + yylsp_offset; if(
_temp6304 < _temp6302.base? 1: _temp6304 >= _temp6302.last_plus_one){ _throw(
Null_Exception);}* _temp6304;}).first_line,({ struct _tagged_ptr3 _temp6305=
yyls; struct Cyc_Yyltype* _temp6307= _temp6305.curr + yylsp_offset; if(
_temp6307 < _temp6305.base? 1: _temp6307 >= _temp6305.last_plus_one){ _throw(
Null_Exception);}* _temp6307;}).last_line))};( struct _xenum_struct*) _temp6298;});
break; case 376: _LL6297: yyval=({ struct Cyc_Exp_tok_struct* _temp6309=( struct
Cyc_Exp_tok_struct*) GC_malloc( sizeof( struct Cyc_Exp_tok_struct));* _temp6309=(
struct Cyc_Exp_tok_struct){.tag= Cyc_Exp_tok_tag,.f1= Cyc_Absyn_null_exp( Cyc_Position_segment_of_abs(({
struct _tagged_ptr3 _temp6310= yyls; struct Cyc_Yyltype* _temp6312= _temp6310.curr
+ yylsp_offset; if( _temp6312 < _temp6310.base? 1: _temp6312 >= _temp6310.last_plus_one){
_throw( Null_Exception);}* _temp6312;}).first_line,({ struct _tagged_ptr3
_temp6313= yyls; struct Cyc_Yyltype* _temp6315= _temp6313.curr + yylsp_offset;
if( _temp6315 < _temp6313.base? 1: _temp6315 >= _temp6313.last_plus_one){ _throw(
Null_Exception);}* _temp6315;}).last_line))};( struct _xenum_struct*) _temp6309;});
break; case 377: _LL6308: yyval=({ struct Cyc_QualId_tok_struct* _temp6317=(
struct Cyc_QualId_tok_struct*) GC_malloc( sizeof( struct Cyc_QualId_tok_struct));*
_temp6317=( struct Cyc_QualId_tok_struct){.tag= Cyc_QualId_tok_tag,.f1=({ struct
_tuple1* _temp6318=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp6318->f1=({ struct Cyc_Absyn_Rel_n_struct* _temp6323=( struct Cyc_Absyn_Rel_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Rel_n_struct)); _temp6323->tag= Cyc_Absyn_Rel_n_tag;
_temp6323->f1= 0;( void*) _temp6323;}); _temp6318->f2=({ struct _tagged_string*
_temp6319=( struct _tagged_string*) GC_malloc( sizeof( struct _tagged_string) *
1); _temp6319[ 0]= Cyc_yyget_String_tok(({ struct _tagged_ptr2 _temp6320= yyvs;
struct _xenum_struct** _temp6322= _temp6320.curr + yyvsp_offset; if( _temp6322 <
_temp6320.base? 1: _temp6322 >= _temp6320.last_plus_one){ _throw( Null_Exception);}*
_temp6322;})); _temp6319;}); _temp6318;})};( struct _xenum_struct*) _temp6317;});
break; case 378: _LL6316: yyval=({ struct _tagged_ptr2 _temp6325= yyvs; struct
_xenum_struct** _temp6327= _temp6325.curr + yyvsp_offset; if( _temp6327 <
_temp6325.base? 1: _temp6327 >= _temp6325.last_plus_one){ _throw( Null_Exception);}*
_temp6327;}); break; default: _LL6324:( void) _throw(({ struct Cyc_Yyimpossible_struct*
_temp6329=( struct Cyc_Yyimpossible_struct*) GC_malloc_atomic( sizeof( struct
Cyc_Yyimpossible_struct));* _temp6329=( struct Cyc_Yyimpossible_struct){.tag=
Cyc_Yyimpossible_tag};( struct _xenum_struct*) _temp6329;}));} yyvsp_offset -=
yylen; yyssp_offset -= yylen; yylsp_offset -= yylen;({ struct _tagged_ptr2
_temp6330= yyvs; struct _xenum_struct** _temp6332= _temp6330.curr +( ++
yyvsp_offset); if( _temp6332 < _temp6330.base? 1: _temp6332 >= _temp6330.last_plus_one){
_throw( Null_Exception);}* _temp6332= yyval;}); yylsp_offset ++; if( yylen == 0){({
struct _tagged_ptr3 _temp6333= yyls; struct Cyc_Yyltype* _temp6335= _temp6333.curr
+ yylsp_offset; if( _temp6335 < _temp6333.base? 1: _temp6335 >= _temp6333.last_plus_one){
_throw( Null_Exception);}(* _temp6335).first_line= Cyc_yylloc.first_line;});({
struct _tagged_ptr3 _temp6336= yyls; struct Cyc_Yyltype* _temp6338= _temp6336.curr
+ yylsp_offset; if( _temp6338 < _temp6336.base? 1: _temp6338 >= _temp6336.last_plus_one){
_throw( Null_Exception);}(* _temp6338).first_column= Cyc_yylloc.first_column;});({
struct _tagged_ptr3 _temp6339= yyls; struct Cyc_Yyltype* _temp6341= _temp6339.curr
+ yylsp_offset; if( _temp6341 < _temp6339.base? 1: _temp6341 >= _temp6339.last_plus_one){
_throw( Null_Exception);}(* _temp6341).last_line=({ struct _tagged_ptr3
_temp6342= yyls; struct Cyc_Yyltype* _temp6344= _temp6342.curr +( yylsp_offset -
1); if( _temp6344 < _temp6342.base? 1: _temp6344 >= _temp6342.last_plus_one){
_throw( Null_Exception);}* _temp6344;}).last_line;});({ struct _tagged_ptr3
_temp6345= yyls; struct Cyc_Yyltype* _temp6347= _temp6345.curr + yylsp_offset;
if( _temp6347 < _temp6345.base? 1: _temp6347 >= _temp6345.last_plus_one){ _throw(
Null_Exception);}(* _temp6347).last_column=({ struct _tagged_ptr3 _temp6348=
yyls; struct Cyc_Yyltype* _temp6350= _temp6348.curr +( yylsp_offset - 1); if(
_temp6350 < _temp6348.base? 1: _temp6350 >= _temp6348.last_plus_one){ _throw(
Null_Exception);}* _temp6350;}).last_column;});({ struct _tagged_ptr3 _temp6351=
yyls; struct Cyc_Yyltype* _temp6353= _temp6351.curr + yylsp_offset; if(
_temp6353 < _temp6351.base? 1: _temp6353 >= _temp6351.last_plus_one){ _throw(
Null_Exception);}(* _temp6353).text=( struct _tagged_string)({ char* _temp6354=(
char*)""; struct _tagged_string _temp6355; _temp6355.curr= _temp6354; _temp6355.base=
_temp6354; _temp6355.last_plus_one= _temp6354 + 1; _temp6355;});});} else{({
struct _tagged_ptr3 _temp6356= yyls; struct Cyc_Yyltype* _temp6358= _temp6356.curr
+ yylsp_offset; if( _temp6358 < _temp6356.base? 1: _temp6358 >= _temp6356.last_plus_one){
_throw( Null_Exception);}(* _temp6358).last_line=({ struct _tagged_ptr3
_temp6359= yyls; struct Cyc_Yyltype* _temp6361= _temp6359.curr +(( yylsp_offset
+ yylen) - 1); if( _temp6361 < _temp6359.base? 1: _temp6361 >= _temp6359.last_plus_one){
_throw( Null_Exception);}* _temp6361;}).last_line;});({ struct _tagged_ptr3
_temp6362= yyls; struct Cyc_Yyltype* _temp6364= _temp6362.curr + yylsp_offset;
if( _temp6364 < _temp6362.base? 1: _temp6364 >= _temp6362.last_plus_one){ _throw(
Null_Exception);}(* _temp6364).last_column=({ struct _tagged_ptr3 _temp6365=
yyls; struct Cyc_Yyltype* _temp6367= _temp6365.curr +(( yylsp_offset + yylen) -
1); if( _temp6367 < _temp6365.base? 1: _temp6367 >= _temp6365.last_plus_one){
_throw( Null_Exception);}* _temp6367;}).last_column;});} yyn=( int)({ short*
_temp6368=( short*) Cyc_yyr1; unsigned int _temp6369= yyn; if( _temp6369 >= 379u){
_throw( Null_Exception);} _temp6368[ _temp6369];}); yystate=( int)({ short*
_temp6370=( short*) Cyc_yypgoto; unsigned int _temp6371= yyn - 118; if(
_temp6371 >= 102u){ _throw( Null_Exception);} _temp6370[ _temp6371];}) +( int)({
struct _tagged_ptr1 _temp6372= yyss; short* _temp6374= _temp6372.curr +
yyssp_offset; if( _temp6374 < _temp6372.base? 1: _temp6374 >= _temp6372.last_plus_one){
_throw( Null_Exception);}* _temp6374;}); if(( yystate >= 0? yystate <= 4113: 0)?(
int)({ short* _temp6375=( short*) Cyc_yycheck; unsigned int _temp6376= yystate;
if( _temp6376 >= 4114u){ _throw( Null_Exception);} _temp6375[ _temp6376];}) ==(
int)({ struct _tagged_ptr1 _temp6377= yyss; short* _temp6379= _temp6377.curr +
yyssp_offset; if( _temp6379 < _temp6377.base? 1: _temp6379 >= _temp6377.last_plus_one){
_throw( Null_Exception);}* _temp6379;}): 0){ yystate=( int)({ short* _temp6380=(
short*) Cyc_yytable; unsigned int _temp6381= yystate; if( _temp6381 >= 4114u){
_throw( Null_Exception);} _temp6380[ _temp6381];});} else{ yystate=( int)({
short* _temp6382=( short*) Cyc_yydefgoto; unsigned int _temp6383= yyn - 118; if(
_temp6383 >= 102u){ _throw( Null_Exception);} _temp6382[ _temp6383];});} goto
yynewstate; yyerrlab: if( yyerrstatus == 0){ ++ Cyc_yynerrs; yyn=( int)({ short*
_temp6384=( short*) Cyc_yypact; unsigned int _temp6385= yystate; if( _temp6385
>= 705u){ _throw( Null_Exception);} _temp6384[ _temp6385];}); if( yyn > - 32768?
yyn < 4113: 0){ int sze= 0; struct _tagged_string msg; int x; int count; count=
0; for( x= yyn < 0? - yyn: 0;( unsigned int) x < 220u / sizeof( char*); x ++){
if(( int)({ short* _temp6386=( short*) Cyc_yycheck; unsigned int _temp6387= x +
yyn; if( _temp6387 >= 4114u){ _throw( Null_Exception);} _temp6386[ _temp6387];})
== x){( unsigned int)( sze += Cyc_String_strlen(({ struct _tagged_string*
_temp6388=( struct _tagged_string*) Cyc_yytname; unsigned int _temp6389= x; if(
_temp6389 >= 220u){ _throw( Null_Exception);} _temp6388[ _temp6389];})) +(
unsigned int) 15), count ++;}} msg=({ unsigned int _temp6390=( unsigned int)(
sze + 15); char* _temp6391=( char*) GC_malloc_atomic( sizeof( char) * _temp6390);
unsigned int i; struct _tagged_string _temp6392={ _temp6391, _temp6391,
_temp6391 + _temp6390}; for( i= 0; i < _temp6390; i ++){ _temp6391[ i]='\000';}
_temp6392;}); Cyc_String_strcpy( msg,( struct _tagged_string)({ char* _temp6393=(
char*)"parse error"; struct _tagged_string _temp6394; _temp6394.curr= _temp6393;
_temp6394.base= _temp6393; _temp6394.last_plus_one= _temp6393 + 12; _temp6394;}));
if( count < 5){ count= 0; for( x= yyn < 0? - yyn: 0;( unsigned int) x < 220u /
sizeof( char*); x ++){ if(( int)({ short* _temp6395=( short*) Cyc_yycheck;
unsigned int _temp6396= x + yyn; if( _temp6396 >= 4114u){ _throw( Null_Exception);}
_temp6395[ _temp6396];}) == x){ Cyc_String_strcat( msg, count == 0?( struct
_tagged_string)({ char* _temp6397=( char*)", expecting `"; struct _tagged_string
_temp6398; _temp6398.curr= _temp6397; _temp6398.base= _temp6397; _temp6398.last_plus_one=
_temp6397 + 14; _temp6398;}):( struct _tagged_string)({ char* _temp6399=( char*)" or `";
struct _tagged_string _temp6400; _temp6400.curr= _temp6399; _temp6400.base=
_temp6399; _temp6400.last_plus_one= _temp6399 + 6; _temp6400;})); Cyc_String_strcat(
msg,({ struct _tagged_string* _temp6401=( struct _tagged_string*) Cyc_yytname;
unsigned int _temp6402= x; if( _temp6402 >= 220u){ _throw( Null_Exception);}
_temp6401[ _temp6402];})); Cyc_String_strcat( msg,( struct _tagged_string)({
char* _temp6403=( char*)"'"; struct _tagged_string _temp6404; _temp6404.curr=
_temp6403; _temp6404.base= _temp6403; _temp6404.last_plus_one= _temp6403 + 2;
_temp6404;})); count ++;}}} Cyc_yyerror( msg);} else{ Cyc_yyerror(( struct
_tagged_string)({ char* _temp6405=( char*)"parse error"; struct _tagged_string
_temp6406; _temp6406.curr= _temp6405; _temp6406.base= _temp6405; _temp6406.last_plus_one=
_temp6405 + 12; _temp6406;}));}} goto yyerrlab1; yyerrlab1: if( yyerrstatus == 3){
if( Cyc_yychar == 0){ return 1;} Cyc_yychar= - 2;} yyerrstatus= 3; goto
yyerrhandle; yyerrdefault: yyerrpop: if( yyssp_offset == 0){ return 1;}
yyvsp_offset --; yystate=( int)({ struct _tagged_ptr1 _temp6407= yyss; short*
_temp6409= _temp6407.curr +( -- yyssp_offset); if( _temp6409 < _temp6407.base? 1:
_temp6409 >= _temp6407.last_plus_one){ _throw( Null_Exception);}* _temp6409;});
yylsp_offset --; yyerrhandle: yyn=( int)({ short* _temp6410=( short*) Cyc_yypact;
unsigned int _temp6411= yystate; if( _temp6411 >= 705u){ _throw( Null_Exception);}
_temp6410[ _temp6411];}); if( yyn == - 32768){ goto yyerrdefault;} yyn += 1; if((
yyn < 0? 1: yyn > 4113)? 1:( int)({ short* _temp6412=( short*) Cyc_yycheck;
unsigned int _temp6413= yyn; if( _temp6413 >= 4114u){ _throw( Null_Exception);}
_temp6412[ _temp6413];}) != 1){ goto yyerrdefault;} yyn=( int)({ short*
_temp6414=( short*) Cyc_yytable; unsigned int _temp6415= yyn; if( _temp6415 >=
4114u){ _throw( Null_Exception);} _temp6414[ _temp6415];}); if( yyn < 0){ if(
yyn == - 32768){ goto yyerrpop;} yyn= - yyn; goto yyreduce;} else{ if( yyn == 0){
goto yyerrpop;}} if( yyn == 704){ return 0;}({ struct _tagged_ptr2 _temp6416=
yyvs; struct _xenum_struct** _temp6418= _temp6416.curr +( ++ yyvsp_offset); if(
_temp6418 < _temp6416.base? 1: _temp6418 >= _temp6416.last_plus_one){ _throw(
Null_Exception);}* _temp6418= Cyc_yylval;});({ struct _tagged_ptr3 _temp6419=
yyls; struct Cyc_Yyltype* _temp6421= _temp6419.curr +( ++ yylsp_offset); if(
_temp6421 < _temp6419.base? 1: _temp6421 >= _temp6419.last_plus_one){ _throw(
Null_Exception);}* _temp6421= Cyc_yylloc;}); goto yynewstate;} void Cyc_yyprint(
int i, struct _xenum_struct* v){ struct _xenum_struct* _temp6422= v; struct
_tuple11* _temp6442; struct _tuple11 _temp6444; int _temp6445; void* _temp6447;
char _temp6449; short _temp6451; struct _tagged_string _temp6453; struct Cyc_Core_Opt*
_temp6455; struct Cyc_Core_Opt* _temp6457; struct Cyc_Core_Opt _temp6459; struct
_tagged_string* _temp6460; struct _tuple1* _temp6462; struct _tuple1 _temp6464;
struct _tagged_string* _temp6465; void* _temp6467; _LL6424: if( _temp6422->tag
== Cyc_Okay_tok_tag){ goto _LL6425;} else{ goto _LL6426;} _LL6426: if((*((
struct _xenum_struct*) _temp6422)).tag == Cyc_Int_tok_tag){ _LL6443: _temp6442=((
struct Cyc_Int_tok_struct*) _temp6422)->f1; _temp6444=* _temp6442; _LL6448:
_temp6447= _temp6444.f1; goto _LL6446; _LL6446: _temp6445= _temp6444.f2; goto
_LL6427;} else{ goto _LL6428;} _LL6428: if((*(( struct _xenum_struct*) _temp6422)).tag
== Cyc_Char_tok_tag){ _LL6450: _temp6449=(( struct Cyc_Char_tok_struct*)
_temp6422)->f1; goto _LL6429;} else{ goto _LL6430;} _LL6430: if((*(( struct
_xenum_struct*) _temp6422)).tag == Cyc_Short_tok_tag){ _LL6452: _temp6451=((
struct Cyc_Short_tok_struct*) _temp6422)->f1; goto _LL6431;} else{ goto _LL6432;}
_LL6432: if((*(( struct _xenum_struct*) _temp6422)).tag == Cyc_String_tok_tag){
_LL6454: _temp6453=(( struct Cyc_String_tok_struct*) _temp6422)->f1; goto
_LL6433;} else{ goto _LL6434;} _LL6434: if((*(( struct _xenum_struct*) _temp6422)).tag
== Cyc_Stringopt_tok_tag){ _LL6456: _temp6455=(( struct Cyc_Stringopt_tok_struct*)
_temp6422)->f1; if( _temp6455 == 0){ goto _LL6435;} else{ goto _LL6436;}} else{
goto _LL6436;} _LL6436: if((*(( struct _xenum_struct*) _temp6422)).tag == Cyc_Stringopt_tok_tag){
_LL6458: _temp6457=(( struct Cyc_Stringopt_tok_struct*) _temp6422)->f1; if(
_temp6457 == 0){ goto _LL6438;} else{ _temp6459=* _temp6457; _LL6461: _temp6460=(
struct _tagged_string*) _temp6459.v; goto _LL6437;}} else{ goto _LL6438;}
_LL6438: if((*(( struct _xenum_struct*) _temp6422)).tag == Cyc_QualId_tok_tag){
_LL6463: _temp6462=(( struct Cyc_QualId_tok_struct*) _temp6422)->f1; _temp6464=*
_temp6462; _LL6468: _temp6467= _temp6464.f1; goto _LL6466; _LL6466: _temp6465=
_temp6464.f2; goto _LL6439;} else{ goto _LL6440;} _LL6440: goto _LL6441; _LL6425:
fprintf( Cyc_Stdio_stderr,"ok"); goto _LL6423; _LL6427: fprintf( Cyc_Stdio_stderr,"%d",
_temp6445); goto _LL6423; _LL6429: fprintf( Cyc_Stdio_stderr,"%c",( int)
_temp6449); goto _LL6423; _LL6431: fprintf( Cyc_Stdio_stderr,"%ds",( int)
_temp6451); goto _LL6423; _LL6433:({ struct _tagged_string _temp6469= _temp6453;
fprintf( Cyc_Stdio_stderr,"\"%.*s\"", _temp6469.last_plus_one - _temp6469.curr,
_temp6469.curr);}); goto _LL6423; _LL6435: fprintf( Cyc_Stdio_stderr,"null");
goto _LL6423; _LL6437:({ struct _tagged_string _temp6470=* _temp6460; fprintf(
Cyc_Stdio_stderr,"\"%.*s\"", _temp6470.last_plus_one - _temp6470.curr, _temp6470.curr);});
goto _LL6423; _LL6439: { struct Cyc_List_List* prefix= 0;{ void* _temp6471=
_temp6467; struct Cyc_List_List* _temp6479; struct Cyc_List_List* _temp6481;
_LL6473: if(( unsigned int) _temp6471 > 1u?(( struct _enum_struct*) _temp6471)->tag
== Cyc_Absyn_Rel_n_tag: 0){ _LL6480: _temp6479=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Rel_n_struct*) _temp6471)->f1; goto _LL6474;} else{ goto _LL6475;}
_LL6475: if(( unsigned int) _temp6471 > 1u?(( struct _enum_struct*) _temp6471)->tag
== Cyc_Absyn_Abs_n_tag: 0){ _LL6482: _temp6481=( struct Cyc_List_List*)(( struct
Cyc_Absyn_Abs_n_struct*) _temp6471)->f1; goto _LL6476;} else{ goto _LL6477;}
_LL6477: if( _temp6471 == Cyc_Absyn_Loc_n){ goto _LL6478;} else{ goto _LL6472;}
_LL6474: prefix= _temp6479; goto _LL6472; _LL6476: prefix= _temp6481; goto
_LL6472; _LL6478: goto _LL6472; _LL6472:;} for( 0; prefix != 0; prefix= prefix->tl){({
struct _tagged_string _temp6483=*(( struct _tagged_string*) prefix->hd); fprintf(
Cyc_Stdio_stderr,"%.*s::", _temp6483.last_plus_one - _temp6483.curr, _temp6483.curr);});}({
struct _tagged_string _temp6484=* _temp6465; fprintf( Cyc_Stdio_stderr,"%.*s::",
_temp6484.last_plus_one - _temp6484.curr, _temp6484.curr);}); goto _LL6423;}
_LL6441: fprintf( Cyc_Stdio_stderr,"?"); goto _LL6423; _LL6423:;} struct Cyc_List_List*
Cyc_Parse_parse_file( struct Cyc_Stdio___sFILE* f){ Cyc_Parse_parse_result= 0;
Cyc_Parse_lbuf=({ struct Cyc_Core_Opt* _temp6485=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp6485->v=( void*) Cyc_Lexing_from_file(
f); _temp6485;}); Cyc_yyparse(); return Cyc_Parse_parse_result;}